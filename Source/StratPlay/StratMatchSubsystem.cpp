// GDD §4.1 / §4.9 / §4.10 / T-INT-05 -- the match owner, implementation side.
//
// WHAT IS HERE THAT COULD NOT BE IN THE HEADER, and it is exactly one thing:
// `#include "StratBridge.h"`. `StratMatchSubsystem.h` declares a UCLASS, so UHT parses it,
// and `StratBridge.h` pulls in the vendored `strat` headers -- the thing that header's own
// note ("declares no reflected types on purpose") exists to keep out of the header tool's
// way. A .cpp is not parsed by UHT, so this is the one place in this class where the
// include is legal, and it must not migrate upward.
//
// NO CROSS-MODULE `strat::` CALL APPEARS BELOW AND NONE MAY. The vendored sources carry no
// `_API` macro -- §4.9 forbids them engine headers -- so `UnrealEditor-StratBridge.dll`
// exports `FStratBridge` and nothing beside it. A direct `strat::` call from this module
// does not link: measured as 8 x LNK2019 the first time the bridge itself tried, and again
// as a single LNK2019 when a StratUI test called `strat::uiCheckSnapshotFidelity`
// directly. Every entry point this class needs is a method on `FStratBridge`. That is also
// why hexes are `FIntPoint` here and why the failure channel is `bool` + `FString` rather
// than `FStratResult` in the public API -- naming that type in the header would require
// the forbidden include.
//
// THE ORDER IN `StartMatch` IS THE POINT OF THIS FILE. Seed, then hand over. See the
// header block: `AdoptBridge` refuses an unseeded bridge, and getting it backwards leaves
// the scoreboard drawing from a second `strat::GameState` that agrees with the first
// exactly until the first command.

#include "StratMatchSubsystem.h"

#include "StratAiTurnRunner.h"
#include "StratBoardActor.h"
#include "StratPlay.h"
#include "StratSaveGame.h"
#include "StratUnitActor.h"

#include "StratScoreboardHUD.h"

// See the block above. This line is legal here and nowhere else in this class.
#include "StratBridge.h"

#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "TimerManager.h"

namespace
{
	/**
	 * Renders a bridge refusal as one log-ready sentence, tag included when the rules
	 * module supplied one.
	 *
	 * THE REASON IS FORWARDED VERBATIM. `FStratBridge`'s header states that every refusal
	 * it returns is a refusal the rules module returned; rewording it here would put this
	 * file's guess about the cause in front of the failing layer's own account of it. Only
	 * the surrounding context -- which call refused -- is added. The same helper, with the
	 * same justification, exists in `StratScoreboardHUD.cpp`; it is duplicated rather than
	 * shared because sharing it would mean exporting a formatting decision across a module
	 * boundary to save four lines.
	 */
	FString DescribeRefusal(const FString& Call, const FStratResult& Result)
	{
		if (Result.Id.IsEmpty())
		{
			return FString::Printf(TEXT("%s refused: %s"), *Call, *Result.Reason);
		}
		return FString::Printf(TEXT("%s refused [%s]: %s"), *Call, *Result.Id, *Result.Reason);
	}

	/**
	 * Turns the configured scenario string into a path the bridge can open.
	 *
	 * RELATIVE TO THE PROJECT DIRECTORY, resolved exactly as `StratScoreboardHUD.cpp` and
	 * the Automation pass resolve it. Resolving against a different root here would let the
	 * running game and the gate read different files while both looked correctly
	 * configured.
	 *
	 * No existence check: whether the file can be read is the bridge's answer to give, and
	 * asking first is a second opinion that can also race the actual open.
	 */
	FString ResolveScenarioPath(const FString& Configured)
	{
		if (FPaths::IsRelative(Configured))
		{
			return FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(), Configured));
		}
		return Configured;
	}
}

UStratMatchSubsystem::UStratMatchSubsystem() = default;

bool UStratMatchSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	// Game and PIE only. See the declaration on why an editor preview world must not get
	// one of these.
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UStratMatchSubsystem::Deinitialize()
{
	// THE PACING TIMER FIRST, AND BEFORE THE BRIDGE IS FREED. `OnAiTurnTimer` submits commands
	// through the bridge this function is about to destroy, and a timer that fired during
	// teardown would be doing that against a freed object. The timer manager dies with the
	// world too, so this is belt and braces -- which is the correct amount of caution for a
	// callback that writes to a `TPimplPtr` being reset four lines down.
	if (UWorld* const World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AiTurnTimer);
	}

	// TORN DOWN IN THE ORDER THINGS DEPEND ON EACH OTHER, even though nothing here
	// currently does. The unit actors hold no pointer into the bridge and the board holds
	// no pointer to a unit -- every one of them was handed values, per call -- so this
	// order is not load-bearing today. It is written out anyway so that a later change
	// which DOES introduce a dependency has an obvious place to be wrong in, instead of
	// being silently correct until destruction order shifts.
	for (const TPair<int32, TObjectPtr<AStratUnitActor>>& Entry : UnitActors)
	{
		if (Entry.Value != nullptr)
		{
			Entry.Value->Destroy();
		}
	}
	UnitActors.Reset();

	if (Board != nullptr)
	{
		Board->Destroy();
		Board = nullptr;
	}

	// THE SCOREBOARD IS NOT TOLD, and that is deliberate rather than an omission. This
	// subsystem is destroyed with the world; so is the HUD. `AStratScoreboardHUD::EndPlay`
	// nulls its adopted pointer and destroys NOTHING, which is correct precisely because
	// this line is the only free. An "un-adopt" call here would be a second teardown path
	// for a lifetime that already has one, and `AdoptBridge`'s contract states that handing
	// ownership back is not a supported transition.
	Bridge.Reset();

	AppliedModel = FStratViewModel();

	Super::Deinitialize();
}

FStratBridge* UStratMatchSubsystem::GetBridge() const
{
	return Bridge.Get();
}

bool UStratMatchSubsystem::IsMatchLive() const
{
	// ASKED OF THE BRIDGE AND NOT OF A MIRROR. See the header: a bool beside the pointer
	// could disagree with it, and the disagreement would present as a match reporting
	// itself live while projecting nothing.
	const FStratBridge* const Live = Bridge.Get();
	return Live != nullptr && Live->IsSeeded();
}

// The public entry is one line, and the whole content of the split is that there is
// still exactly ONE ordered sequence. See `StartMatchInternal`'s declaration.
bool UStratMatchSubsystem::StartMatch(const FStratMatchConfig& Config, FString& OutFailureReason)
{
	return StartMatchInternal(Config, nullptr, OutFailureReason);
}

bool UStratMatchSubsystem::StartMatchInternal(const FStratMatchConfig& Config,
                                              const UStratSaveGame*    Restore,
                                              FString&                 OutFailureReason)
{
	OutFailureReason.Reset();

	// ---- Configuration checks, before anything is allocated -----------------
	// Named individually so a log line says WHICH property is unset. One combined "the
	// config is incomplete" would collapse "DT_Units is not assigned" and "the scenario
	// file name is empty" into the same sentence, and those have different fixes.

	if (Config.UnitTable == nullptr || Config.TerrainTable == nullptr)
	{
		OutFailureReason = FString::Printf(
			TEXT("definition tables are not assigned on the GameMode's defaults (UnitTable %s, TerrainTable %s)"),
			Config.UnitTable == nullptr ? TEXT("unset") : TEXT("set"),
			Config.TerrainTable == nullptr ? TEXT("unset") : TEXT("set"));
		UE_LOG(LogStratPlay, Error, TEXT("No match this session: %s"), *OutFailureReason);
		return false;
	}

	if (Config.ScenarioFile.IsEmpty())
	{
		OutFailureReason = TEXT("ScenarioFile is empty on the GameMode's defaults");
		UE_LOG(LogStratPlay, Error, TEXT("No match this session: %s"), *OutFailureReason);
		return false;
	}

	// `FirstSide` is NOT range-checked. `Replay.h` states that no rule in this project
	// decides which side moves first, so seeding owns whatever it will accept; checking it
	// here would make this file a second author of a decision the rules module declined to
	// make. An out-of-range value comes back as `LoadScenarioFromFile`'s own refusal below.
	//
	// `ViewingSide` IS checked, and by the builder rather than here -- `StratBuildViewModel`
	// range-checks it against the snapshot's own side count, which sits nearer the data it
	// indexes than any constant this file could name.

	// ---- Whatever the last match left, before anything replaces it ---------
	// A NO-OP THE FIRST TIME and the reason a load does not end up with two boards. See
	// the declaration: this is deliberately unconditional rather than guarded on "is this
	// a restart", because a guard is a second thing that can be wrong about which it is.
	TearDownPresentation();

	ActiveConfig = Config;
	ViewingSide = Config.ViewingSide;

	// A NEW MATCH HAS NOT BEEN RECORDED AS COMPLETED, whatever the last one did. Cleared
	// here rather than in `Deinitialize` because the event it is about is a MATCH, and two
	// matches can share one world; see the member's declaration.
	bMatchResultRecorded = false;

	// ---- The bridge. STEP ONE OF THE ORDERED SEQUENCE ----------------------
	// Constructed here rather than in the constructor: a bridge that exists before its
	// inputs have been checked is a bridge `GetBridge()` could hand out unseeded, and this
	// is where the failure path can say why it did not survive.
	Bridge = MakePimpl<FStratBridge>();
	FStratBridge* const Fresh = Bridge.Get();

	const FStratResult Loaded = Fresh->LoadDefinitions(Config.UnitTable, Config.TerrainTable);
	if (!Loaded.bOk)
	{
		OutFailureReason = DescribeRefusal(TEXT("LoadDefinitions"), Loaded);

		// A HALF-LOADED BRIDGE MUST NOT BE REACHABLE. Definitions mapped but no scenario
		// seeded is a state the projection refuses anyway, but handing it out through
		// `GetBridge()` invites a caller to treat "not seeded" as "seeded and empty", which
		// is the exact confusion `FStratBridge::MakeUiSnapshot` refuses to permit.
		Bridge.Reset();

		UE_LOG(LogStratPlay, Error, TEXT("No match this session: %s"), *OutFailureReason);
		return false;
	}

	const FString ScenarioPath = ResolveScenarioPath(Config.ScenarioFile);

	// STEP TWO, AND THE ONE THAT MAKES THE HANDOVER LEGAL. After this line -- and not one
	// line before it -- `Fresh->IsSeeded()` is true, which is the precondition
	// `AStratScoreboardHUD::AdoptBridge` enforces in its own words.
	const FStratResult Seeded = Fresh->LoadScenarioFromFile(ScenarioPath, Config.FirstSide);
	if (!Seeded.bOk)
	{
		// The resolved path is added because the configured string and the file actually
		// opened differ by a root, and a "not found" without it sends the reader looking in
		// the wrong directory.
		OutFailureReason = FString::Printf(TEXT("%s (resolved path: %s)"),
			*DescribeRefusal(TEXT("LoadScenarioFromFile"), Seeded), *ScenarioPath);

		Bridge.Reset();

		UE_LOG(LogStratPlay, Error, TEXT("No match this session: %s"), *OutFailureReason);
		return false;
	}

	// ---- STEP TWO AND A HALF: the recorded log, when this is a load ---------
	// BETWEEN SEEDING AND THE HAND-OVER, and that window is the only legal one:
	// `RestoreFromSaveText` refuses an unseeded bridge and `AdoptBridge` refuses one too.
	//
	// ITS FAILURE JOINS THE TWO ABOVE IT AND NOT THE COMPLAINTS BELOW. A save that will not
	// replay is a rules-side refusal -- a bad header, a log the definitions no longer
	// resolve, or a `stateHash` that disagrees with what replaying produced -- and leaving
	// the bridge alive after one would hand out a correctly seeded match wearing a loaded
	// match's name. The bridge's own reason is forwarded verbatim, including the
	// `T-SAVE-04` / `GATE-SAVE-PARSE` / `T-SAVE-06` tag it carries, because those three
	// have three different fixes.
	if (Restore != nullptr)
	{
		FStratSaveIdentity Identity;
		Identity.RulesCommit = Restore->RulesCommit;
		Identity.DataHash    = Restore->DataHash;

		int32 RestoredCommands = 0;
		const FStratResult Restored =
			Fresh->RestoreFromSaveText(Restore->SaveText, Identity, RestoredCommands);
		if (!Restored.bOk)
		{
			OutFailureReason = DescribeRefusal(TEXT("RestoreFromSaveText"), Restored);

			Bridge.Reset();

			UE_LOG(LogStratPlay, Error, TEXT("No match this session: %s"), *OutFailureReason);
			return false;
		}

		UE_LOG(LogStratPlay, Log,
			TEXT("Restored %d recorded command(s) from a save into the seeded match."),
			RestoredCommands);
	}

	// FROM HERE THE MATCH IS LIVE AND NOTHING BELOW MAY UNDO THAT. Every remaining step is
	// presentation, and a presentation gap is reported without tearing down a correctly
	// seeded match -- see the declaration on why this function is all-or-nothing on the
	// rules side and not on the presentation side.

	// ---- §2.9's buildlist, if this match has an AI ---------------------------
	// AFTER SEEDING AND NOT BEFORE, for a reason that is the bridge's rather than this
	// file's: `LoadDefinitions` CLEARS the stored buildlist, because the values are indexes
	// into the vector it just rebuilt and after a reload they do not merely go stale, they
	// name different unit types. Setting it before that call would leave an AI that silently
	// never builds.
	//
	// AN EMPTY LIST IS SKIPPED ENTIRELY rather than passed through as an empty set. Both
	// configure an AI that never builds; skipping keeps a hot-seat match's call sequence
	// byte-for-byte what it was before this phase, so nothing that passed at 78/78 can move
	// because a property with a safe default appeared.
	//
	// A REFUSAL IS A COMPLAINT AND NOT A TEARDOWN. The match is seeded and correct; what is
	// wrong is one configured unit id, and an AI that does not build is a diminished match
	// rather than no match. It joins the same verdict a missing tile mesh joins, and for the
	// same reason -- see the declaration on why this function is all-or-nothing on the rules
	// side and not on the configuration side.
	FString BuildlistReason;
	if (ActiveConfig.AiBuildlistUnitIds.Num() > 0)
	{
		const FStratResult Buildlist = Fresh->SetBuildlistByIds(ActiveConfig.AiBuildlistUnitIds);
		if (!Buildlist.bOk)
		{
			BuildlistReason = DescribeRefusal(TEXT("SetBuildlistByIds"), Buildlist);
		}
	}

	// ---- The board ---------------------------------------------------------
	// SPAWNED RATHER THAN PLACED IN THE LEVEL, and this is a decision worth its line. A
	// placed board would be an actor a designer could delete, duplicate, or leave pointing
	// at last week's tile set, and the subsystem would then have to search for it and
	// decide what two of them mean. Spawning makes "there is exactly one board and this
	// object owns it" true by construction. The cost is that the board cannot be dressed
	// in-level; the answer to dressing it is its Blueprint defaults.
	FString PresentationReason;

	if (ActiveConfig.BoardActorClass != nullptr)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		Board = GetWorld()->SpawnActor<AStratBoardActor>(
			ActiveConfig.BoardActorClass, FTransform::Identity, Params);

		if (Board == nullptr)
		{
			PresentationReason = FString::Printf(
				TEXT("SpawnActor returned null for BoardActorClass '%s'"),
				*GetNameSafe(ActiveConfig.BoardActorClass));
		}
	}
	else
	{
		// A legitimate configuration -- a map that wants a seeded match and a scoreboard
		// and no 3D board -- so it is reported and not refused.
		PresentationReason = TEXT("no BoardActorClass is set on the GameMode's defaults; no board was spawned");
	}

	// ---- First reconciliation ----------------------------------------------
	// BUILD AND APPLY DIRECTLY RATHER THAN THROUGH `RefreshPresentation`, and the reason is
	// the ordering this whole function exists to hold. `RefreshPresentation` also refreshes
	// the scoreboard, and at this instant the scoreboard has NOT yet adopted -- it is still
	// reading whatever bridge it seeded for itself, or none. Asking it to refresh here
	// would either draw the wrong match for one frame or produce a refusal about a bridge
	// that is about to be replaced, and both would be reported as this match's problem. The
	// panel is refreshed by `HandBridgeToScoreboard` below, once it is reading THIS bridge.
	FString RefreshReason;
	bool bRefreshed = false;
	{
		FStratViewModel Model;
		bRefreshed = BuildViewModel(Model, RefreshReason);
		if (bRefreshed)
		{
			ApplyView(Model);
		}
	}

	// ---- The handover. STEP THREE, AND ONLY NOW ----------------------------
	FString AdoptionReason;
	const bool bAdopted = HandBridgeToScoreboard(AdoptionReason);

	// ---- One verdict, assembled from the parts that refused ----------------
	TArray<FString> Complaints;
	if (!BuildlistReason.IsEmpty())
	{
		Complaints.Add(BuildlistReason);
	}
	if (!PresentationReason.IsEmpty())
	{
		Complaints.Add(PresentationReason);
	}
	if (!bRefreshed && !RefreshReason.IsEmpty())
	{
		Complaints.Add(RefreshReason);
	}
	if (!bAdopted && !AdoptionReason.IsEmpty())
	{
		Complaints.Add(AdoptionReason);
	}

	if (Complaints.Num() > 0)
	{
		OutFailureReason = FString::Join(Complaints, TEXT("; "));

		// Warning and not Error: the match IS live. `IsMatchLive()` is how a caller tells a
		// seeded match with a presentation gap from a match that never seeded, and logging
		// this at Error would put the two in the same bucket for anyone reading the log
		// instead of the return value.
		UE_LOG(LogStratPlay, Warning,
			TEXT("Match seeded from '%s' (first side %d), drawn for side %d, but: %s"),
			*ScenarioPath, Config.FirstSide, ViewingSide, *OutFailureReason);
		return false;
	}

	UE_LOG(LogStratPlay, Log,
		TEXT("Match live: seeded from '%s' (first side %d), drawn for side %d, %d hexes and %d units on screen."),
		*ScenarioPath, Config.FirstSide, ViewingSide,
		Board != nullptr ? Board->GetDrawnHexCount() : 0,
		UnitActors.Num());

	return true;
}

bool UStratMatchSubsystem::BuildViewModel(FStratViewModel& OutModel, FString& OutFailureReason) const
{
	OutFailureReason.Reset();

	const FStratBridge* const Live = Bridge.Get();
	if (Live == nullptr)
	{
		OutFailureReason = TEXT("there is no bridge: StartMatch has not run, or it refused");
		return false;
	}

	// CALLS THE BUILDER AND ADDS NOTHING. `StratBuildViewModel` is the only place a
	// `strat::UiSnapshot` becomes reflected types, and it refuses rather than producing an
	// empty model when there is nothing to project -- forwarded here unchanged, because an
	// empty model is a legitimate value and reconciling against one would destroy every
	// actor on screen.
	return StratBuildViewModel(*Live, ViewingSide, OutModel, OutFailureReason);
}

void UStratMatchSubsystem::ApplyView(const FStratViewModel& Model)
{
	// ---- The board ---------------------------------------------------------
	if (Board != nullptr)
	{
		FString BoardReason;
		if (!Board->ApplyHexes(Model.Hexes, BoardReason))
		{
			// A tile-mesh gap is a phase-5 configuration matter and the rest of the board
			// was drawn. Logged here rather than propagated, because `ApplyView` returns
			// void ON PURPOSE: reconciliation is not a request that can be declined, it is
			// the act of making the world agree with a value that has already been decided.
			// A caller cannot do anything useful with "the board is missing a mesh" except
			// what this line does.
			UE_LOG(LogStratPlay, Warning, TEXT("Board: %s"), *BoardReason);
		}
	}

	// ---- The units: spawn, move, destroy ------------------------------------
	// A SET DIFFERENCE AGAINST THE MODEL, not a diff against a previous model. The model is
	// a complete statement of what should be on screen, so "present here and absent there"
	// is decidable from this call alone -- which is what makes it safe to run
	// unconditionally and what makes T-INT-05 a fact rather than a claim.

	TSet<int32> Seen;
	Seen.Reserve(Model.Units.Num());

	for (const FStratUnitView& View : Model.Units)
	{
		Seen.Add(View.UnitId);

		TObjectPtr<AStratUnitActor>* Existing = UnitActors.Find(View.UnitId);

		if (Existing == nullptr || *Existing == nullptr)
		{
			if (ActiveConfig.UnitActorClass == nullptr)
			{
				// Reported once, by `StartMatch`'s verdict, rather than once per unit --
				// ten identical sentences bury whatever else went wrong.
				continue;
			}

			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AStratUnitActor* const Spawned = GetWorld()->SpawnActor<AStratUnitActor>(
				ActiveConfig.UnitActorClass, FTransform::Identity, Params);

			if (Spawned == nullptr)
			{
				UE_LOG(LogStratPlay, Warning,
					TEXT("SpawnActor returned null for UnitActorClass '%s' (unit %d)"),
					*GetNameSafe(ActiveConfig.UnitActorClass), View.UnitId);
				continue;
			}

			Existing = &UnitActors.Add(View.UnitId, Spawned);
		}

		// THE LOCATION COMES FROM THE BOARD AND IS NOT COMPUTED HERE.
		// `AStratBoardActor::WorldLocationOfHex` is the only axial -> world conversion in
		// the project; a second copy in this file is a set of units half a tile off the day
		// `HexSize` changes. With no board there is nowhere to stand, so the unit sits at
		// the world origin and is still spawned -- a visible pile at the origin is a
		// diagnosable state; a silently absent army is not.
		const FVector Where = Board != nullptr
			? Board->WorldLocationOfHex(View.Hex)
			: FVector::ZeroVector;

		(*Existing)->ApplyUnitView(View, Where);
	}

	// DESTROY WHAT THE MODEL NO LONGER CARRIES. `FStratViewModel::Units` is "every LIVING
	// unit", so a unit that left the model died -- and the actor for it must go, because an
	// actor with no model entry is a unit the rules module does not have.
	for (auto It = UnitActors.CreateIterator(); It; ++It)
	{
		if (Seen.Contains(It.Key()))
		{
			continue;
		}

		if (It.Value() != nullptr)
		{
			It.Value()->Destroy();
		}
		It.RemoveCurrent();
	}

	// ---- Sec 2.11.6's guided-opening strip ----------------------------------
	// THE THIRD SURFACE RECONCILED FROM THIS ONE VALUE, alongside the board and the units,
	// and reached the same way they are: it is told what to show, it is not asked to find
	// out. `Model.Guidance` was written by `FStratGuidedOpening::DecorateViewModel` before
	// this call on the decorated path and is default-constructed -- `bActive` false -- on
	// every other, so an undecorated rebuild CLEARS the strip rather than leaving the last
	// directive standing. That is correct and not a gap: on that path the model really does
	// say no guidance is running.
	//
	// UNCONDITIONAL, WITH NO BRANCH ON `bActive`. Skipping the push when guidance is
	// inactive would make the strip's contents depend on the history of calls instead of on
	// the current model, which is the delta-shaped thinking this whole function is written
	// to exclude.
	//
	// THROUGH THE HUD BECAUSE THE HUD OWNS THE WIDGET. This class holds `TSubclassOf`s for
	// the board and the unit actors and could have held one for the strip too -- it was
	// rejected because creating a widget means `CreateWidget` and `AddToViewport`, and
	// `StratPlay.Build.cs` would have had to name `UMG`, `Slate` and `SlateCore` to say
	// those words. `AStratScoreboardHUD::PushGuidance` takes a reflected struct, so this
	// module needs none of them. Its own header records the debt that arrangement creates.
	//
	// NO REASON STRING AND NO RETURN VALUE TO IGNORE. `PushGuidance` is void: with no strip
	// configured there is nothing to do and nothing went wrong, which is a different thing
	// from the scoreboard's refusable refresh in `RefreshPresentation`.
	if (AStratScoreboardHUD* const HUD = FindScoreboardHUD())
	{
		HUD->PushGuidance(Model.Guidance);
	}

	// CACHED AFTER THE FACT AND NEVER READ BACK. See `GetViewModel`: this is a record of
	// what was applied, not an input to anything above.
	AppliedModel = Model;

	// §2.11.6's MATCH-ENDED HOOK, LAST AND OFF THE SAME VALUE THE SCREEN WAS DRAWN FROM.
	// After the reconciliation rather than before it, so that the frame in which the player
	// SEES the result is the frame in which it is recorded -- and so that a failure to
	// persist can never leave the board undrawn. It reads `Model` and asks the bridge
	// nothing; see the declaration of `NoteMatchResultIfEnded`.
	NoteMatchResultIfEnded(Model);
}

bool UStratMatchSubsystem::RefreshPresentation(FString& OutFailureReason)
{
	OutFailureReason.Reset();

	// WRITTEN IN TERMS OF THE TWO PUBLIC HALVES, per the declaration, so that this path and
	// phase 4's decorated path cannot drift. There is no third reconciliation
	// implementation in this class.
	FStratViewModel Model;
	if (!BuildViewModel(Model, OutFailureReason))
	{
		return false;
	}

	ApplyView(Model);

	// THE SCOREBOARD REFRESHES ITSELF FROM THE BRIDGE, not from the model above. It
	// projects `FStratScoreboardModel` through its own path, and giving it a second source
	// here would be two answers to what one panel shows. Asked last so that a refusal names
	// the panel rather than the board.
	if (AStratScoreboardHUD* const HUD = FindScoreboardHUD())
	{
		FString ScoreboardReason;
		if (!HUD->RefreshScoreboard(ScoreboardReason))
		{
			OutFailureReason = FString::Printf(TEXT("scoreboard refresh refused: %s"), *ScoreboardReason);
			return false;
		}
	}

	return true;
}

bool UStratMatchSubsystem::SetViewingSide(int32 InViewingSide, FString& OutFailureReason)
{
	OutFailureReason.Reset();

	// NOT RANGE-CHECKED HERE. `StratBuildViewModel` checks it against the snapshot's own
	// side count and `AStratScoreboardHUD::SetViewingSide` checks it against
	// `strat::SIDE_COUNT` -- both sit nearer the data they index than any constant this
	// file could name without the forbidden include. A third check here would be a third
	// authority that can disagree with the other two.
	//
	// THE ASSIGNMENT HAPPENS FIRST AND IS NOT ROLLED BACK ON A FAILED REBUILD, matching
	// `AStratScoreboardHUD::SetViewingSide` exactly and for the reason its declaration
	// gives: rolling back would make a hot-seat hand-over silently stay with the previous
	// player, which is the one outcome this game must not produce quietly.
	ViewingSide = InViewingSide;

	// The HUD keeps its own viewing side -- it is a separate projection with a separate
	// column layout -- so it is told rather than inferred. Its refusal is the range check,
	// forwarded in its own words.
	if (AStratScoreboardHUD* const HUD = FindScoreboardHUD())
	{
		FString HudReason;
		if (!HUD->SetViewingSide(InViewingSide, HudReason))
		{
			OutFailureReason = FString::Printf(TEXT("scoreboard refused side %d: %s"), InViewingSide, *HudReason);
			return false;
		}
	}

	return RefreshPresentation(OutFailureReason);
}

AStratUnitActor* UStratMatchSubsystem::FindUnitActor(int32 UnitId) const
{
	const TObjectPtr<AStratUnitActor>* const Found = UnitActors.Find(UnitId);
	return Found != nullptr ? Found->Get() : nullptr;
}

// ---------------------------------------------------------------------------
// §2.9's opponent. WHEN a turn runs, and nothing about WHAT it does.
//
// EVERY COMMAND BELOW COMES FROM `FStratAiTurnRunner`, which gets each one from
// `FStratBridge::NextAiCommand`. This class contributes three facts and no fourth: whether
// the config calls this side AI, whether the match is still running, and how many turns may
// run back to back. None of the three is a rules answer.
//
// THE LOOP ASKS THE VIEW MODEL EVERY ITERATION AND CACHES NOTHING. `sideToMove` and
// `hasResult` are the rules module's, re-read after every AI turn, on the same discipline
// `FStratSelectionMachine` states for `bHasMoved`/`bHasActed`: there is no mirror to go
// stale, because the mirror was never made.
// ---------------------------------------------------------------------------

bool UStratMatchSubsystem::IsSideAi(int32 Side) const
{
	return ActiveConfig.AiSides.Contains(Side);
}

bool UStratMatchSubsystem::IsAiTurnDue() const
{
	if (!IsMatchLive())
	{
		return false;
	}

	// THE MATCH-OVER CHECK COMES FROM `hasResult` AND IS NEVER INFERRED FROM `ResultTier`.
	// `FStratMatchView` says so on the field: the two are read separately so they cannot
	// disagree. An AI that kept playing past a §2.8 result would be submitting commands the
	// rules module refuses, one per turn, forever.
	FStratViewModel Model;
	FString         Reason;
	if (!BuildViewModel(Model, Reason))
	{
		// Not "no turn is due" as a fact about the match -- it is this object being unable to
		// tell. Reported by the caller that acts on it; here it is simply not due, because
		// running an AI turn against a state we could not read would be worse.
		return false;
	}

	return !Model.Match.bHasResult && IsSideAi(Model.Match.SideToMove);
}

bool UStratMatchSubsystem::RunAiTurnsIfDue(FString& OutFailureReason)
{
	OutFailureReason.Reset();

	if (!IsAiTurnDue())
	{
		// ORDINARY AND NOT A REFUSAL. It is the human's turn, or the match is over, or there
		// is no match. See the declaration.
		return true;
	}

	// THE SYNCHRONOUS PATH IS THE DEFAULT AND THE TESTED ONE. `AiTurnDelaySeconds` is 0 out of
	// the box, so unless a Blueprint default asks for pacing this call plays the turn before
	// it returns -- which is what makes the AI drivable from an automation test with no
	// ticking world.
	if (ActiveConfig.AiTurnDelaySeconds <= 0.0f)
	{
		return RunAiTurnsNow(OutFailureReason);
	}

	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		// Configured for pacing with no world to time against. Falls back to running rather
		// than to refusing: the turn is due, and a match that silently stops handing over is a
		// worse failure than one that paces badly.
		return RunAiTurnsNow(OutFailureReason);
	}

	// ONE TIMER, RESET RATHER THAN STACKED. A second call while one is pending replaces it;
	// two pending timers would run two AI turn loops against the same state, and the second
	// would find it is no longer the AI's turn and do nothing -- which is harmless today and
	// is exactly the kind of thing that stops being harmless when both sides are AI.
	World->GetTimerManager().ClearTimer(AiTurnTimer);
	World->GetTimerManager().SetTimer(AiTurnTimer, this, &UStratMatchSubsystem::OnAiTurnTimer,
		ActiveConfig.AiTurnDelaySeconds, /*bLoop=*/false);

	return true;
}

void UStratMatchSubsystem::OnAiTurnTimer()
{
	FString Reason;
	if (!RunAiTurnsNow(Reason))
	{
		// NOWHERE TO RETURN IT TO. A timer has no caller, so the refusal is logged at Warning
		// here rather than dropped -- the runner has already logged the specific
		// `STRAT-AI refused` line, and this one says which path was driving.
		UE_LOG(LogStratPlay, Warning, TEXT("The paced AI turn refused: %s"), *Reason);
	}
}

bool UStratMatchSubsystem::RunAiTurnsNow(FString& OutFailureReason)
{
	OutFailureReason.Reset();

	FStratBridge* const Live = Bridge.Get();
	if (Live == nullptr)
	{
		OutFailureReason = TEXT("there is no bridge: StartMatch has not run, or it refused");
		return false;
	}

	if (bAiTurnRunning)
	{
		// See the member's declaration on why this flag is not the mirror this class refuses
		// elsewhere. Reported rather than silently skipped, because a re-entrant call means a
		// caller ordering that nobody intended.
		OutFailureReason = TEXT("an AI turn is already running on this subsystem");
		return false;
	}

	TGuardValue<bool> ReentrancyGuard(bAiTurnRunning, true);

	FStratAiTurnRunner Runner;
	Runner.MaxCommandsPerTurn = ActiveConfig.AiMaxCommandsPerTurn;

	FStratBridgeAiTurnPort Port(Live);

	int32       TurnsRun     = 0;
	const int32 MaxTurns     = ActiveConfig.AiMaxConsecutiveTurns;
	FString     StopReason;

	while (TurnsRun < MaxTurns)
	{
		// ASKED AGAIN EVERY ITERATION. After an AI turn the side has changed and the match may
		// have reached a §2.8 result; both are the rules module's answers and both are re-read
		// rather than predicted from what the runner reported.
		FStratViewModel Model;
		FString         ModelReason;
		if (!BuildViewModel(Model, ModelReason))
		{
			StopReason = ModelReason;
			break;
		}

		if (Model.Match.bHasResult || !IsSideAi(Model.Match.SideToMove))
		{
			// The ordinary exit: the match ended, or it is a human's turn.
			break;
		}

		const FStratAiTurnOutcome Outcome = Runner.RunTurn(Port);
		++TurnsRun;

		if (!Outcome.bOk)
		{
			// THE HARD STOP. Whatever the runner applied stands; it has already logged the
			// `STRAT-AI refused` line naming the phase. Handing play back with the AI's turn
			// half-played and no fault reported is the failure this whole phase is shaped
			// around, so the reason travels up.
			StopReason = Outcome.FailureReason;
			break;
		}
	}

	if (StopReason.IsEmpty() && TurnsRun >= MaxTurns)
	{
		// THE OUTER BOUND, REPORTED FOR `FStratAiTurnRunner::MaxCommandsPerTurn`'s REASON. A
		// silent stop here reads as an AI that decided to hand over, which is precisely what
		// it did not do.
		//
		// `phase=handover` IS THE FOURTH PHASE AND IT IS THIS FUNCTION'S, not the runner's:
		// `FStratAiTurnRunner` plays ONE turn and cannot see that this is the ninth in a row.
		// It goes out through `StratLogAiTurnRefusal` and NOT through a `UE_LOG` here, which
		// is a correction rather than a preference -- the line was written by hand in this
		// file for the length of one diff, carried `phase=` and `reason=` alone, and so was
		// the single `STRAT-AI` line a phase-D field parser built on the declared format
		// would have failed to split. `StratAiTurnRunner.cpp` holds the only copy of that
		// format string; `Port` supplies the live turn and side the bound was reached at.
		StopReason = FString::Printf(
			TEXT("%d consecutive AI turns ran without the match reaching a human turn or a ")
			TEXT("result (AiMaxConsecutiveTurns is %d)"),
			TurnsRun, MaxTurns);
		StratLogAiTurnRefusal(TEXT("handover"), Port.Turn(), Port.SideToMove(), StopReason);
	}

	// RECONCILED ON BOTH PATHS. The board moved whether or not the turn finished; a screen
	// still showing the pre-AI board disagrees with the rules module either way. The refresh's
	// own refusal does not overwrite an AI refusal -- the AI's is the one that explains the
	// other.
	FString RefreshReason;
	const bool bRefreshed = RefreshPresentation(RefreshReason);

	if (!StopReason.IsEmpty())
	{
		OutFailureReason = StopReason;
		return false;
	}

	if (!bRefreshed)
	{
		OutFailureReason = RefreshReason;
		return false;
	}

	return true;
}

// ---------------------------------------------------------------------------
// §4.10 save slots. See the declarations for why the disk is in this file and
// nowhere else.
// ---------------------------------------------------------------------------

FString UStratMatchSubsystem::ResolveSaveSlotName(const FString& Requested) const
{
	// EMPTY MEANS CONFIGURED, and there is no third case. A slot name that fell back to a
	// literal in this file would be a second author of the same string, and the designer's
	// property would then be advisory.
	return Requested.IsEmpty() ? ActiveConfig.SaveSlotName : Requested;
}

bool UStratMatchSubsystem::DoesSaveSlotExist(const FString& SlotName) const
{
	return UGameplayStatics::DoesSaveGameExist(ResolveSaveSlotName(SlotName), 0);
}

bool UStratMatchSubsystem::HasCompletedAMatchOnSave(const FString& SlotName) const
{
	const FString Slot = ResolveSaveSlotName(SlotName);
	if (Slot.IsEmpty() || !UGameplayStatics::DoesSaveGameExist(Slot, 0))
	{
		return false;
	}

	const UStratSaveGame* const Payload =
		Cast<UStratSaveGame>(UGameplayStatics::LoadGameFromSlot(Slot, 0));

	// NO VERSION GATE HERE, UNLIKE `LoadMatchFromSlot`, and the asymmetry is deliberate. A
	// version mismatch there means the §4.10 text cannot be trusted to restore a match, which
	// is a refusal. Here the question is one bool about the player's history, and refusing to
	// answer it would show a veteran the guided opening -- harmless, but for a reason that has
	// nothing to do with them. If the field ever moves or changes meaning, this is where an
	// arm goes and `SavedDataVersion` is what it reads.
	return Payload != nullptr && Payload->bHasCompletedAMatch;
}

bool UStratMatchSubsystem::RecordMatchCompletionOnSave(const FString& SlotName,
                                                      FString&       OutFailureReason)
{
	OutFailureReason.Reset();

	const FString Slot = ResolveSaveSlotName(SlotName);
	if (Slot.IsEmpty())
	{
		OutFailureReason = TEXT("no slot name was given and SaveSlotName is empty on the GameMode's defaults");
		return false;
	}

	// READ FIRST, ALWAYS. Two reasons, and only the second is about this method: an existing
	// slot holds a match's §4.10 text that must survive this write, and a slot that already
	// carries the bit has nothing left to record.
	UStratSaveGame* Payload = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(Slot, 0))
	{
		Payload = Cast<UStratSaveGame>(UGameplayStatics::LoadGameFromSlot(Slot, 0));

		if (Payload != nullptr && Payload->bHasCompletedAMatch)
		{
			// ALREADY TRUE IS SUCCESS. See the declaration: `ApplyView` can observe a
			// finished match on every refresh, and this is what keeps that from being a
			// disk write per frame.
			return true;
		}
	}

	// NO VERSION GATE, MATCHING `HasCompletedAMatchOnSave` AND NOT `LoadMatchFromSlot`. The
	// reader's block gives the reason and it is symmetric here: this touches one bool about
	// the player's history and does not interpret `SaveText`, so a slot from another shape of
	// this struct is not a reason to refuse to remember that a match ended. If the field ever
	// moves, an arm goes in both places and `SavedDataVersion` is what it reads.

	if (Payload == nullptr)
	{
		// EITHER THERE IS NO SLOT, OR THERE IS ONE THIS BUILD CANNOT CAST. Both land on a
		// fresh payload, and the second case overwrites something -- which is the honest
		// trade, because a slot that does not hold a `UStratSaveGame` is a slot
		// `LoadMatchFromSlot` already refuses by name and there is nothing of ours in it.
		Payload = Cast<UStratSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UStratSaveGame::StaticClass()));
	}

	if (Payload == nullptr)
	{
		OutFailureReason = TEXT("CreateSaveGameObject returned null for UStratSaveGame");
		return false;
	}

	// ONE FIELD. `SavedDataVersion` is stamped because the shape being written is this
	// build's shape; everything else on the payload is left exactly as it was read, which is
	// what makes this safe to run in the middle of a match whose text is already in the slot.
	// A payload created above therefore goes to disk with an EMPTY `SaveText`, and that is
	// intended: `LoadMatchFromSlot` refuses it by name rather than restoring an empty board.
	Payload->SavedDataVersion    = UStratSaveGame::kCurrentSavedDataVersion;
	Payload->bHasCompletedAMatch = true;

	if (!UGameplayStatics::SaveGameToSlot(Payload, Slot, 0))
	{
		OutFailureReason = FString::Printf(
			TEXT("SaveGameToSlot failed writing slot '%s'"), *Slot);
		return false;
	}

	UE_LOG(LogStratPlay, Log,
		TEXT("Slot '%s' now records a completed match; §2.11.6 guidance is suppressed from here on."),
		*Slot);
	return true;
}

void UStratMatchSubsystem::NoteMatchResultIfEnded(const FStratViewModel& Model)
{
	if (bMatchResultRecorded || !Model.Match.bHasResult)
	{
		return;
	}

	// NOBODY ASKED FOR THIS WRITE, SO SOMEBODY HAS TO HAVE SAID YES. The shipped GameMode
	// Blueprint's default says yes; a `FStratMatchConfig` built in C++ says no, because the
	// field is declared false. Read that field's block: the check this replaced was
	// `ResolveSaveSlotName(FString()).IsEmpty()`, which could not distinguish "no slot was
	// chosen" from "the shipped slot was inherited", and so wrote a completed match onto the
	// player's slot from an AI-vs-AI automation clause.
	if (!ActiveConfig.bRecordCompletionOnMatchEnd)
	{
		return;
	}

	// AN EMPTY SLOT NAME IS SILENT AND DOES NOT LATCH. Reachable now only by a deliberate
	// clear -- opted in, and `SaveSlotName` emptied on the defaults -- which is a
	// misconfiguration rather than a fault, and `ApplyView` runs on every refresh, so a
	// warning here would be a warning per frame and that is how a real refusal gets buried.
	// Not latching means a slot name configured later still takes effect.
	if (ResolveSaveSlotName(FString()).IsEmpty())
	{
		return;
	}

	FString Reason;
	if (!RecordMatchCompletionOnSave(FString(), Reason))
	{
		// NOT LATCHED ON A FAILURE, so a transient write failure is retried on the next
		// refresh rather than losing the fact for the rest of the match.
		UE_LOG(LogStratPlay, Warning,
			TEXT("The match reached a result but §2.11.6's completion flag was not persisted: %s"),
			*Reason);
		return;
	}

	bMatchResultRecorded = true;
}

bool UStratMatchSubsystem::SaveMatchToSlot(const FString& SlotName, FString& OutFailureReason)
{
	OutFailureReason.Reset();

	FStratBridge* const Live = Bridge.Get();
	if (Live == nullptr || !Live->IsSeeded())
	{
		OutFailureReason = TEXT("there is no live match to save");
		UE_LOG(LogStratPlay, Warning, TEXT("Save refused: %s"), *OutFailureReason);
		return false;
	}

	const FString Slot = ResolveSaveSlotName(SlotName);
	if (Slot.IsEmpty())
	{
		OutFailureReason = TEXT("no slot name was given and SaveSlotName is empty on the GameMode's defaults");
		UE_LOG(LogStratPlay, Warning, TEXT("Save refused: %s"), *OutFailureReason);
		return false;
	}

	// THE BYTES ARE THE BRIDGE'S. Nothing here composes §4.10; `SerializeRecordedSave` owns
	// every field and its source, including the two this file supplies through
	// `FStratSaveIdentity` and the three it must NOT supply.
	FStratSaveIdentity Identity;
	Identity.RulesCommit = ActiveConfig.RulesCommit;
	Identity.DataHash    = ActiveConfig.DataHash;

	FString Text;
	const FStratResult Serialized = Live->SerializeRecordedSave(Identity, Text);
	if (!Serialized.bOk)
	{
		OutFailureReason = DescribeRefusal(TEXT("SerializeRecordedSave"), Serialized);
		UE_LOG(LogStratPlay, Warning, TEXT("Save refused: %s"), *OutFailureReason);
		return false;
	}

	// READ THE EXISTING SLOT FIRST, so §2.11.6's onboarding state survives being saved over.
	// A fresh `UStratSaveGame` every time would silently reset `bHasCompletedAMatch` and
	// re-arm every one-shot tip on the first mid-match save -- a guidance bug with a
	// save-system cause. That is already load-bearing rather than anticipatory:
	// `HasCompletedAMatchOnSave` reads the bool today and `FStratGuidedOpening` suppresses
	// the guided opening on it, so a payload rebuilt from scratch here would un-suppress
	// guidance for a player who had finished a match.
	UStratSaveGame* Payload = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(Slot, 0))
	{
		Payload = Cast<UStratSaveGame>(UGameplayStatics::LoadGameFromSlot(Slot, 0));
	}
	if (Payload == nullptr)
	{
		Payload = Cast<UStratSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UStratSaveGame::StaticClass()));
	}
	if (Payload == nullptr)
	{
		OutFailureReason = TEXT("CreateSaveGameObject returned null for UStratSaveGame");
		UE_LOG(LogStratPlay, Error, TEXT("Save refused: %s"), *OutFailureReason);
		return false;
	}

	// EVERY FIELD OVERWRITTEN EXCEPT THE ONBOARDING PAIR, which is carried forward by not
	// being written. `SavedDataVersion` is stamped to the CURRENT value because the shape
	// being written is the current shape, whatever the slot held before.
	Payload->SavedDataVersion = UStratSaveGame::kCurrentSavedDataVersion;
	Payload->SaveText         = Text;
	Payload->RulesCommit      = Identity.RulesCommit;
	Payload->DataHash         = Identity.DataHash;
	Payload->ScenarioFile     = ActiveConfig.ScenarioFile;
	Payload->FirstSide        = ActiveConfig.FirstSide;
	Payload->ViewingSide      = ViewingSide;

	if (!UGameplayStatics::SaveGameToSlot(Payload, Slot, 0))
	{
		OutFailureReason = FString::Printf(
			TEXT("SaveGameToSlot failed writing slot '%s'"), *Slot);
		UE_LOG(LogStratPlay, Error, TEXT("Save refused: %s"), *OutFailureReason);
		return false;
	}

	UE_LOG(LogStratPlay, Log,
		TEXT("Match saved to slot '%s': %d recorded command(s), scenario '%s' (first side %d), drawn for side %d."),
		*Slot, Live->RecordedCommandCount(), *ActiveConfig.ScenarioFile,
		ActiveConfig.FirstSide, ViewingSide);
	return true;
}

bool UStratMatchSubsystem::LoadMatchFromSlot(const FString& SlotName, FString& OutFailureReason)
{
	OutFailureReason.Reset();

	if (bAiTurnRunning)
	{
		// The AI loop submits into the bridge this call is about to free.
		OutFailureReason = TEXT("an AI turn is running; a load would free the bridge under it");
		UE_LOG(LogStratPlay, Warning, TEXT("Load refused: %s"), *OutFailureReason);
		return false;
	}

	// A CONFIGURED SUBSYSTEM IS THE PRECONDITION, and it is checked on the tables rather
	// than on a "has StartMatch run" bool -- for the reason this class refuses a `bSeeded`
	// mirror: a bool beside the thing can disagree with the thing. The tables ARE what a
	// reseed needs, so their absence is the honest question.
	if (ActiveConfig.UnitTable == nullptr || ActiveConfig.TerrainTable == nullptr)
	{
		OutFailureReason = TEXT(
			"this subsystem has never been configured: StartMatch must run once (the GameMode does it) "
			"before a slot can be loaded, because a slot carries no definition tables");
		UE_LOG(LogStratPlay, Warning, TEXT("Load refused: %s"), *OutFailureReason);
		return false;
	}

	const FString Slot = ResolveSaveSlotName(SlotName);
	if (!UGameplayStatics::DoesSaveGameExist(Slot, 0))
	{
		OutFailureReason = FString::Printf(TEXT("no save exists in slot '%s'"), *Slot);
		UE_LOG(LogStratPlay, Warning, TEXT("Load refused: %s"), *OutFailureReason);
		return false;
	}

	UStratSaveGame* const Payload =
		Cast<UStratSaveGame>(UGameplayStatics::LoadGameFromSlot(Slot, 0));
	if (Payload == nullptr)
	{
		// `LoadGameFromSlot` returns a `USaveGame*` and the cast is the class check. A slot
		// written by a different game, or by a build in which this class was renamed, lands
		// here rather than as a crash on the first field read.
		OutFailureReason = FString::Printf(
			TEXT("slot '%s' does not hold a UStratSaveGame"), *Slot);
		UE_LOG(LogStratPlay, Warning, TEXT("Load refused: %s"), *OutFailureReason);
		return false;
	}

	// THE VERSION GATE, AND IT REFUSES RATHER THAN GUESSES. `UStratSaveGame`'s header states
	// the contract: a change to the MEANING of a field bumps the version and grows an arm
	// here. There is one version today, so there is one arm and it is equality -- written
	// out anyway, because the failure a version exists to prevent is the one where a newer
	// slot loads into an older build and every new field reads as its default with nothing
	// said.
	if (Payload->SavedDataVersion != UStratSaveGame::kCurrentSavedDataVersion)
	{
		OutFailureReason = FString::Printf(
			TEXT("slot '%s' was written at SavedDataVersion %d and this build reads %d"),
			*Slot, Payload->SavedDataVersion, UStratSaveGame::kCurrentSavedDataVersion);
		UE_LOG(LogStratPlay, Warning, TEXT("Load refused: %s"), *OutFailureReason);
		return false;
	}

	if (Payload->SaveText.IsEmpty())
	{
		OutFailureReason = FString::Printf(
			TEXT("slot '%s' carries no §4.10 text"), *Slot);
		UE_LOG(LogStratPlay, Warning, TEXT("Load refused: %s"), *OutFailureReason);
		return false;
	}

	// THREE FIELDS OVERRIDDEN AND NO MORE -- the three §4.10 cannot carry. Everything else,
	// including both definition tables and both actor classes, stays whatever the GameMode
	// configured, because a slot that pinned an asset would break on a rename.
	FStratMatchConfig Config = ActiveConfig;
	Config.ScenarioFile = Payload->ScenarioFile;
	Config.FirstSide    = Payload->FirstSide;
	Config.ViewingSide  = Payload->ViewingSide;

	// THE ONE SEQUENCE. Nothing about reconciliation, adoption or ordering is repeated here.
	const bool bStarted = StartMatchInternal(Config, Payload, OutFailureReason);

	if (bStarted)
	{
		UE_LOG(LogStratPlay, Log, TEXT("Match loaded from slot '%s'."), *Slot);
	}
	else
	{
		// `StartMatchInternal` returns false for a presentation gap on a LIVE match as well
		// as for a rules-side teardown, exactly as `StartMatch` does; `IsMatchLive()` is
		// what tells those apart, and this line does not re-decide it.
		UE_LOG(LogStratPlay, Warning,
			TEXT("Load of slot '%s' reported: %s (match live: %s)"),
			*Slot, *OutFailureReason, IsMatchLive() ? TEXT("yes") : TEXT("no"));
	}

	return bStarted;
}

void UStratMatchSubsystem::TearDownPresentation()
{
	// THE TIMER FIRST AND BEFORE ANYTHING IS FREED, for `Deinitialize`'s reason exactly:
	// `OnAiTurnTimer` submits through a bridge a reseed is about to replace.
	if (UWorld* const World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AiTurnTimer);
	}

	for (const TPair<int32, TObjectPtr<AStratUnitActor>>& Entry : UnitActors)
	{
		if (Entry.Value != nullptr)
		{
			Entry.Value->Destroy();
		}
	}
	UnitActors.Reset();

	if (Board != nullptr)
	{
		Board->Destroy();
		Board = nullptr;
	}

	// THE APPLIED MODEL GOES WITH THE ACTORS. `ApplyView` reconciles against `UnitActors`
	// and `GetViewModel` claims to describe what is on screen; leaving last match's model
	// behind an emptied map would make that claim false for exactly as long as it took
	// someone to read it.
	AppliedModel = FStratViewModel();

	// THE BRIDGE IS NOT TOUCHED HERE, and the asymmetry with `Deinitialize` is the point.
	// `StartMatchInternal` replaces it a few lines later with a `MakePimpl` whose assignment
	// frees the old one; resetting it here as well would be a second free path for the one
	// pointer this project frees in one place.
}

AStratScoreboardHUD* UStratMatchSubsystem::FindScoreboardHUD() const
{
	const UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return nullptr;
	}

	// THE FIRST LOCAL PLAYER'S HUD. A scoreboard is client-local and per-player -- the
	// third of `StratScoreboardHUD.h`'s reasons for it being an AHUD at all -- and hot-seat
	// has exactly one local player by definition: the two players share a screen, they do
	// not each get one. A split-screen build would iterate; this milestone has no such
	// build, and pretending otherwise here would be untested code shaped like a feature.
	const APlayerController* const PC = World->GetFirstPlayerController();
	if (PC == nullptr)
	{
		return nullptr;
	}

	return Cast<AStratScoreboardHUD>(PC->GetHUD());
}

bool UStratMatchSubsystem::HandBridgeToScoreboard(FString& OutFailureReason)
{
	OutFailureReason.Reset();

	FStratBridge* const Live = Bridge.Get();
	if (Live == nullptr)
	{
		OutFailureReason = TEXT("there is no bridge to hand over");
		return false;
	}

	AStratScoreboardHUD* const HUD = FindScoreboardHUD();
	if (HUD == nullptr)
	{
		// NOT A FAILURE. A map whose GameMode names no scoreboard HUD runs the match
		// without one; that is a configuration, and phase 5 is where the GameMode Blueprint
		// gains a HUD class. Logged so that "why is there no scoreboard" has an answer
		// later, at Log because nothing went wrong.
		UE_LOG(LogStratPlay, Log,
			TEXT("No AStratScoreboardHUD on the first local player; the match runs without a scoreboard."));
		return true;
	}

	// THE HANDOVER. Everything above `StartMatch`'s call to this function exists to make
	// this line legal: `AdoptBridge` refuses an unseeded bridge, in its own words, so that
	// "nothing is loaded" cannot be drawn as "a match in which nothing has happened".
	//
	// AFTER THIS THE HUD FREES NOTHING. Its `EndPlay` nulls a raw observing pointer; the
	// `TPimplPtr` on this subsystem is the only free in the project for a bridge a running
	// game owns. If the HUD had already seeded a bridge of its own -- which happens when its
	// BeginPlay ran before this call -- `AdoptBridge` destroys that one first, so no map
	// ever carries two `strat::GameState`s past this point.
	if (!HUD->AdoptBridge(*Live, OutFailureReason))
	{
		// The ONLY way this refuses is an unseeded bridge, which would mean this function
		// was called before `LoadScenarioFromFile`. Said plainly, because the fix is an
		// ordering fix in `StartMatch` and not a configuration fix anywhere.
		OutFailureReason = FString::Printf(
			TEXT("the scoreboard refused the bridge (this means it was handed over before seeding): %s"),
			*OutFailureReason);
		UE_LOG(LogStratPlay, Error, TEXT("%s"), *OutFailureReason);
		return false;
	}

	// The HUD's own viewing side is set from the same configured value the model uses, so
	// the panel and the board start out describing the same player's screen. `AdoptBridge`
	// deliberately does not refresh -- adopting is a change of source, not of state -- and
	// `SetViewingSide` performs the refresh that puts the adopted bridge's match on the
	// panel.
	FString SideReason;
	if (!HUD->SetViewingSide(ViewingSide, SideReason))
	{
		OutFailureReason = FString::Printf(
			TEXT("the bridge was adopted but the scoreboard refused viewing side %d: %s"),
			ViewingSide, *SideReason);
		return false;
	}

	return true;
}
