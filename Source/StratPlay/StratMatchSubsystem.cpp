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
#include "StratCameraPawn.h"
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

	// AND THE PLAYBACK TIMER, WHICH IS A WEAKER CASE AND IS CLEARED ANYWAY. `OnAiPlaybackTimer`
	// submits nothing and never touches the bridge, so it could not corrupt anything the way
	// `OnAiTurnTimer` could; it does dereference `Board` and the possessed pawn, and a callback
	// running while those are being torn down four lines below is a crash for a decoration.
	EndAiPlaybackTour();

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

	// AND A NEW MATCH HAS NOT CONCLUDED, whatever the last one did. Same event, same reason;
	// see `ConcludeMatchIfEnded` and the member's declaration.
	bMatchConclusionAnnounced = false;

	// AND SEC 2.11.4'S VERDICT SCREEN COMES DOWN WITH IT. The latch above is what stops the
	// screen being shown twice for one match; this is what stops the LAST match's screen still
	// being on top of the new one's board. Both are needed and neither implies the other --
	// clearing a bool removes nothing from the viewport, and the widget outlives
	// `TearDownPresentation` because it belongs to the HUD rather than to this object.
	//
	// UNCONDITIONAL AND UNGUARDED, on `TearDownPresentation`'s stated reasoning: `HideMatchResult`
	// is a no-op with no screen up, and a guard on "is this a restart" would be a second thing
	// that can be wrong about which it is. Null HUD is the ordinary case on a first start.
	if (AStratScoreboardHUD* const PriorHUD = FindScoreboardHUD())
	{
		PriorHUD->HideMatchResult();
	}

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

	// ---- STEP TWO AND A QUARTER: Sec 2.9's difficulty handicap --------------
	// AFTER SEEDING, BECAUSE SEEDING IS WHAT CREATES THE PURSE IT MOVES. The scenario
	// owns the baseline -- `Scenario.h` says the handicap is "a match-setup parameter
	// applied on top" and deliberately not a scenario field -- so this is the first
	// instant at which there is a configured value to apply it on top of.
	//
	// AND BEFORE THE RESTORE, WHICH IS THE ORDERING DECISION IN THIS BLOCK.
	// `RestoreFromSaveText` replays the save's log onto whatever this bridge is holding
	// and then compares `strat::canonicalStateHash`. A save written at Easy was written
	// from a handicapped base, so the replay must run on a handicapped base or the hash
	// cannot agree. Applying it AFTER the restore would shift a state whose hash had
	// already been certified, and the slot would then load "successfully" into a match
	// worth 150 Fame more than the one that was saved. This order makes a tier mismatch
	// a REFUSAL instead: the handicap is not a `strat::SaveCommand`, so the log cannot
	// carry it, and it is not a scenario field, so `scenarioHash` cannot either --
	// loading a slot written at another tier lands on T-SAVE-06, whose refusal now names
	// this as its fourth cause rather than leaving a reader to blame the log.
	//
	// A REFUSAL IS A COMPLAINT AND NOT A TEARDOWN, on `SetBuildlistByIds`' line below.
	// The only ways this refuses are an unseeded bridge, which the line above just ruled
	// out, and a `ViewingSide` that names no real side -- a configuration fault whose
	// consequence is a match played at Normal. Tearing down a correctly seeded match over
	// a difficulty setting would be the larger harm.
	FString HandicapReason;
	{
		const int32 HandicappedSide = StratHandicappedSide(ActiveConfig);
		if (HandicappedSide != INDEX_NONE)
		{
			const int32 Delta = StratDifficultyFameDelta(ActiveConfig.Difficulty);

			int32 FameAfter = INDEX_NONE;
			const FStratResult Handicap =
				Fresh->ApplyStartingFameHandicap(HandicappedSide, Delta, FameAfter);

			if (!Handicap.bOk)
			{
				HandicapReason = DescribeRefusal(TEXT("ApplyStartingFameHandicap"), Handicap);
			}
			else
			{
				// LOGGED WITH THE DELTA AND THE RESULT, not just the tier. The tier alone
				// does not tell a reader what the scenario's baseline was, and the pair is
				// what makes "applied on top" checkable from a log line.
				UE_LOG(LogStratPlay, Log,
					TEXT("Sec 2.9 handicap: side %d opens on %d Fame (%+d on the scenario's value)."),
					HandicappedSide, FameAfter, Delta);
			}
		}
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
	//
	// DECORATED, THOUGH, AND THAT IS NOT THE SAME QUESTION. Skipping `RefreshPresentation`
	// here is about the SCOREBOARD's bridge, not about the model: this reconciliation still
	// draws the board, the units and Sec 2.11.6's strip, so the model it applies must be
	// complete. Whether anybody is registered to complete it depends on whether the
	// controller's `BeginPlay` has run yet, and actor `BeginPlay` order is not something this
	// file may assume -- which is why the fix does not rest on it. If nobody is registered
	// yet, this applies an undecorated model and the controller's own first refresh, moments
	// later, applies a decorated one; if somebody is, this one is already decorated. Both
	// orders end with a decorated model applied, and `AStratScoreboardHUD` holds the last
	// pushed value for a strip that does not exist yet, so neither order loses it.
	FString RefreshReason;
	bool bRefreshed = false;
	{
		FStratViewModel Model;
		bRefreshed = BuildViewModelForPresentation(Model, RefreshReason);
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
	if (!HandicapReason.IsEmpty())
	{
		Complaints.Add(HandicapReason);
	}
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

bool UStratMatchSubsystem::BuildViewModelForPresentation(FStratViewModel& OutModel,
                                                         FString&         OutFailureReason)
{
	if (!BuildViewModel(OutModel, OutFailureReason))
	{
		// DECORATING A MODEL THAT DID NOT BUILD IS NOT AN OPTION AND NOT A CHOICE MADE HERE.
		// `BuildViewModel` refuses rather than handing back an empty model, so on this branch
		// `OutModel` is whatever the caller brought; running the decorator over it would ask
		// `FStratGuidedOpening::Observe` to advance beats off a board that was never read.
		return false;
	}

	// THE ONE DECORATION POINT. `ExecuteIfBound` and not a null test plus a call, so that
	// "nobody is registered" and "the registered object has been destroyed" take the same
	// path -- both mean the model is drawn exactly as the rules module produced it, which is
	// correct and is what every fixture with no controller in its world gets.
	ViewDecorator.ExecuteIfBound(OutModel);
	return true;
}

void UStratMatchSubsystem::SetViewDecorator(FStratViewDecorator InDecorator)
{
	ViewDecorator = MoveTemp(InDecorator);
}

void UStratMatchSubsystem::ClearViewDecorator()
{
	ViewDecorator.Unbind();
}

bool UStratMatchSubsystem::HasViewDecorator() const
{
	// `IsBound()` and not a stored bool. A `CreateUObject` binding whose object has been
	// destroyed still reports bound here, which is honest -- somebody DID register -- and it
	// is why nothing in this class branches on this answer.
	return ViewDecorator.IsBound();
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

		// THE VIEWING SIDE COMES OFF THE MODEL AND NOT OFF THIS CLASS'S OWN `ViewingSide`
		// MEMBER, even though the two are equal on every path that reaches here. The model's
		// field is the one `FStratViewModel` declares as "the caller's statement", and its
		// block forbids the alternative in as many words: "a viewing side held beside the
		// model is a second input, and T-INT-05 would then be about two things". Passing
		// `ViewingSide` here would make the marker a function of the model PLUS a member,
		// and a stale member would put the mark on the wrong seat with a green build.
		//
		// AND THE GUIDANCE-ACTIVE BIT COMES OFF THAT SAME MODEL, ADDED 2026-08-24. It is the
		// value the ring is driven from thirty lines below and the value the strip is pushed
		// from below that -- read ONCE per refresh from one `FStratViewModel`, so §2.11.6's
		// "the ring and the turn-1a unit marker clear in the same frame as the strip" is a
		// property of this function's structure rather than of three surfaces agreeing. This
		// function still decides nothing: it forwards two fields it already holds.
		(*Existing)->ApplyUnitView(View, Where, Model.ViewingSide, Model.Guidance.bActive);
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

	// ---- Sec 2.11.6-B's objective ring --------------------------------------
	// ONE CALL SITE AND IT IS THIS ONE. `ApplyView` is the only place every model reaches
	// the screen through, so driving the ring here makes "the ring and the marker clear in
	// the same frame as the strip" -- `FStratGuidedOpening::SkipGuidance`'s wording --
	// structural rather than a promise: the clear and the strip's push are eight lines
	// apart in one function, on one value.
	//
	// UNCONDITIONAL IN BOTH DIRECTIONS, with an else. Showing without clearing would leave
	// the ring standing after the window closed, and the model would carry no record of why
	// -- the delta-shaped thinking the strip's block below refuses in the same terms.
	//
	// TWO FIELDS ARE READ AND NEITHER IS COMBINED WITH ANYTHING ELSE. `bActive` is whether
	// guidance is running at all; `bHasObjectiveRing` is whether the scenario authored an
	// objective for the guided seat, and `FStratGuidanceView` declares that it alone
	// qualifies `ObjectiveHex` -- `FIntPoint(0, 0)` is a real hex and cannot signal its own
	// absence. THIS IS NOT T-UI-03'S FORBIDDEN ARITHMETIC: it is a visibility condition over
	// two booleans, not a number drawn on screen, and no widget renders the conjunction.
	//
	// THE MARKER HAS NO CALL SITE HERE BY DESIGN, AND THAT IS STILL TRUE WITH ONE AMENDMENT.
	// `AStratUnitActor::ApplyUnitView` sets it from the `FStratUnitView` this function
	// already hands that actor, so the two visuals ride the same refresh and there is no
	// second driver. What changed on 2026-08-23 is that this function now also hands over
	// `Model.ViewingSide`, for the user ruling that filters the marker to the viewing seat;
	// this sentence previously said this function knew "nothing about either", which is no
	// longer exact. It supplies arguments; it still decides nothing and calls nothing
	// marker-shaped.
	//
	// [AMENDED 2026-08-24.] It now hands over `Model.Guidance.bActive` as well -- THE SAME
	// FIELD THE `if` DIRECTLY BELOW READS FOR THE RING. That is the point rather than a
	// coincidence: with the marker keyed on it, the ring's `else` branch and the marker's
	// hide are two readings of one bool inside one `ApplyView`, so neither can outlive the
	// other. Before this, the marker's operands were all match-constant and it never cleared
	// at all -- found in a human playtest, not by any clause.
	if (Board != nullptr)
	{
		if (Model.Guidance.bActive && Model.Guidance.bHasObjectiveRing)
		{
			Board->ShowObjective(Model.Guidance.ObjectiveHex);
		}
		else
		{
			Board->ClearObjective();
		}
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
	// ---- Sec 2.11.2's hover info panel --------------------------------------
	// THE FOURTH SURFACE RECONCILED FROM THIS ONE VALUE, on the same line as the strip and
	// through the same HUD lookup, so the two cannot fall out of step with each other or with
	// the board. `Model.InfoPanel` was written by `StratDecorateInfoPanel` before this call --
	// `AStratPlayerController::RefreshFromMachine` decorates, and its own block records that
	// the info-panel decoration must run after the selection machine has written this frame's
	// DONE bits -- and is default-constructed on any path that did not decorate, which is
	// Sec 2.11.2's empty panel and the honest reading of a model that states no hover.
	//
	// UNCONDITIONAL, WITH NO BRANCH ON `bHasHex`. Skipping the push when nothing is hovered
	// would leave the last hovered hex's readout on screen after the cursor left the board --
	// the delta-shaped thinking this whole function is written to exclude, and the one outcome
	// Sec 2.11.2's "Empty when nothing is hovered" cannot produce.
	//
	// `Model.ViewingSide` AND NOT THE HUD'S OWN. Sec 2.11.2's `yours` / `neutral` / `enemy`
	// clause is viewer-relative and `FStratInfoPanelView` deliberately carries a SIDE rather
	// than a boolean -- its "NOT IN THIS ROUND" block rules on that -- so the resolution needs
	// the seat this model was rendered for. Both operands are fields of the one value being
	// applied, which is what makes the panel rebuildable from the view model alone.
	//
	// THROUGH THE HUD BECAUSE THE HUD OWNS THE WIDGET, and the module-arrow argument the strip
	// records above applies here word for word: `AStratScoreboardHUD::PushInfoPanel` takes a
	// reflected struct and an `int32`, so this module needs none of `UMG`, `Slate` or
	// `SlateCore`. Its own header records the debt that arrangement creates.
	//
	// NO REASON STRING AND NO RETURN VALUE TO IGNORE, on `PushGuidance`'s reasoning.
	if (AStratScoreboardHUD* const HUD = FindScoreboardHUD())
	{
		HUD->PushGuidance(Model.Guidance);
		HUD->PushInfoPanel(Model.InfoPanel, Model.ViewingSide);
	}

	// CACHED AFTER THE FACT AND NEVER READ BACK. See `GetViewModel`: this is a record of
	// what was applied, not an input to anything above.
	AppliedModel = Model;

	// §2.8's END-OF-MATCH TRANSITION, LAST AND OFF THE SAME VALUE THE SCREEN WAS DRAWN FROM.
	// After the reconciliation rather than before it, so that the frame in which the player
	// SEES the result is the frame in which the match stops -- and so that a failure to
	// persist can never leave the board undrawn. It reads `Model` and asks the bridge
	// nothing; see the declaration of `ConcludeMatchIfEnded`.
	//
	// §2.11.6's completion writer -- which is what used to be called from this line, and is
	// the whole of what a result used to cause -- is now one of the things that transition
	// does, in the same order it always ran in.
	ConcludeMatchIfEnded(Model);
}

bool UStratMatchSubsystem::RefreshPresentation(FString& OutFailureReason)
{
	OutFailureReason.Reset();

	// WRITTEN IN TERMS OF THE TWO PUBLIC HALVES, per the declaration, so that this path and
	// phase 4's decorated path cannot drift. There is no third reconciliation
	// implementation in this class.
	//
	// THROUGH `BuildViewModelForPresentation` AND NOT `BuildViewModel`, which is the whole of
	// the second guidance-delivery fix. This function's callers -- `SetViewingSide` and
	// `RunAiTurnsNow` -- have no beat of their own to add, but the model they draw from still
	// needs its Sec 2.11.6 section filled in by whoever owns the beats. Built the other way,
	// every hot-seat hand-over and every AI turn applied a model whose `Guidance` block was
	// default-constructed, and `ApplyView` correctly pushed that default onto the strip.
	FStratViewModel Model;
	if (!BuildViewModelForPresentation(Model, OutFailureReason))
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

	// STILL NOT RANGE-CHECKED HERE, and that half is unchanged. `StratBuildViewModel`
	// checks it against the snapshot's own side count and `AStratScoreboardHUD::
	// SetViewingSide` checks it against `strat::SIDE_COUNT` -- both sit nearer the data
	// they index than any constant this file could name without the forbidden include. A
	// third check here would be a third authority that can disagree with the other two.
	//
	// WHAT DID CHANGE (2026-08-25) IS WHEN AN EXISTING AUTHORITY IS CONSULTED, and this
	// paragraph is stamped rather than deleted because the claim it replaces was HALF right.
	// It used to read:
	// RETRACTED> "THE ASSIGNMENT HAPPENS FIRST AND IS NOT ROLLED BACK ON A FAILED REBUILD,
	// RETRACTED>  matching `AStratScoreboardHUD::SetViewingSide` exactly..."
	// The reason it gave is sound and survives below, but it is about a FAILED REBUILD and
	// the code applied it to a FAILED RANGE CHECK as well. `AStratScoreboardHUD::
	// SetViewingSide` does NOT match: it range-checks BEFORE assigning, so a refused call
	// changes nothing there. This one assigned first, so a refused out-of-range hand-over
	// left this class holding a side the HUD had rejected -- and every later refresh then
	// failed inside `StratBuildViewModel` naming the builder rather than the hand-over.
	//
	// THE TWO FAILURE MODES ARE NOW SPLIT BY THE ORDER OF THE LINES BELOW, which is the only
	// way a reader can be sure of them: every REFUSAL is above the assignment, and the
	// rebuild -- the one failure that deliberately does NOT roll back -- is below it.

	// ---- Refusal, before any mutation --------------------------------------
	// The HUD keeps its own viewing side -- it is a separate projection with a separate
	// column layout -- so it is told rather than inferred, and its refusal IS the range
	// check, forwarded in its own words. Told BEFORE this class commits, so that on a
	// refusal neither member moved.
	AStratScoreboardHUD* const HUD = FindScoreboardHUD();
	if (HUD != nullptr)
	{
		FString HudReason;
		if (!HUD->SetViewingSide(InViewingSide, HudReason))
		{
			OutFailureReason = FString::Printf(TEXT("scoreboard refused side %d: %s"), InViewingSide, *HudReason);
			return false;
		}
	}
	else if (const FStratBridge* const Live = Bridge.Get())
	{
		// WITH NO HUD, NOTHING RANGE-CHECKED THE SIDE AT ALL -- and that was true before this
		// change too, silently. A HUD-less subsystem is a legitimate configuration (every
		// fixture with no actors in its world is one), and it used to adopt any `int32`
		// handed to it and discover the problem on the next refresh, or never.
		//
		// A TRIAL BUILD AT THE CANDIDATE SIDE, AND NOT A NEW CONSTANT. This asks
		// `StratBuildViewModel` -- the SAME authority `RefreshPresentation` would reach a few
		// lines further down -- for the side being proposed rather than for the one this
		// class currently holds, which is the whole trick: it gets the check without
		// committing to the value. The model built here is DISCARDED and never applied; the
		// one the screen gets is built by `RefreshPresentation` below, after the assignment,
		// so nothing on screen comes from this probe.
		//
		// IT REFUSES ON ANY BUILD FAILURE AND NOT ONLY ON AN OUT-OF-RANGE SIDE, which is
		// deliberate and is the conservative direction: if the model cannot be built for the
		// proposed side, adopting the side would put this class into a state whose only
		// observable consequence is a refresh that fails afterwards. Refusing here says so
		// once, in the builder's own words, with both members untouched.
		FStratViewModel Probe;
		FString ProbeReason;
		if (!StratBuildViewModel(*Live, InViewingSide, Probe, ProbeReason))
		{
			OutFailureReason = FString::Printf(
				TEXT("no scoreboard is present and side %d does not build: %s"),
				InViewingSide, *ProbeReason);
			return false;
		}
	}
	// NO HUD AND NO BRIDGE FALLS THROUGH ON PURPOSE. There is no authority in existence to
	// ask, and inventing one here is exactly the third check refused above. The assignment
	// stands and `RefreshPresentation` reports "there is no bridge" -- which is the true
	// reason, and is a REBUILD failure rather than a refused side.

	// ---- Commit, then rebuild ----------------------------------------------
	// ON A FAILED REBUILD THE SIDE HAS STILL CHANGED, matching `AStratScoreboardHUD::
	// SetViewingSide` exactly and for the reason its declaration gives: rolling back would
	// make a hot-seat hand-over silently stay with the previous player, which is the one
	// outcome this game must not produce quietly. Everything that could refuse the side has
	// already run.
	ViewingSide = InViewingSide;

	return RefreshPresentation(OutFailureReason);
}

// ---------------------------------------------------------------------------
// Sec 2.11.5 -- the production menu. TWO DIRECTIONS, ONE RULES AUTHORITY.
//
// EVERY ANSWER BELOW IS `FStratBridge`'S. The read is `StratBuildProductionMenu`, a StratUI
// free function over `FStratBridge::BuildOptions`; the write is
// `FStratBridge::SubmitBuildAtHex` unchanged. Nothing here filters a row, sorts a row,
// combines two of a row's booleans, or decides that a build is legal -- see the
// declarations for why each of those four was specifically refused.
// ---------------------------------------------------------------------------

bool UStratMatchSubsystem::RefreshProductionMenu(FIntPoint FactoryHex, FString& OutFailureReason)
{
	OutFailureReason.Reset();

	FStratBridge* const Live = Bridge.Get();
	if (Live == nullptr)
	{
		// NAMED HERE rather than left to arrive as some later refusal. "There is no match" and
		// "that side is outside this match" send the next reader to different files.
		//
		// THE CONTRAST THIS USED TO DRAW WAS THE WRONG ONE and is corrected rather than dropped:
		// it named "that hex is not a factory" as the later refusal being distinguished from.
		// That is not a refusal at all -- `FStratBridge::BuildOptions` does not pre-check the
		// factory hex, so a non-build-point hex comes back as a FULL MENU with every row
		// `bAvailable` false carrying the module's own reason. See the declaration's two-channel
		// block for what rides which channel.
		OutFailureReason = TEXT("there is no live match to open a production menu on");
		return false;
	}

	// BUILT INTO A LOCAL AND MOVED ACROSS ONLY ON SUCCESS. `StratBuildProductionMenu` is
	// already all-or-nothing on the array it fills; the hex beside it is ours, and written
	// before the call it could move on a call that produced no rows.
	// THE SIX-ARGUMENT OVERLOAD, FOR §2.11.5's HEADER NUMBER. `BuiltPurse` is the very purse
	// each row's `Shortfall` was computed against, returned by the same call in the same frame,
	// so the header and the `need N` rows cannot describe two different amounts of Fame. Read
	// through this overload rather than off `AppliedModel.Sides` for the reason the spawn-block
	// query below is asked here rather than read off `AppliedModel.Factories`.
	TArray<FStratBuildOptionView> Built;
	int32                         BuiltPurse = 0;
	if (!StratBuildProductionMenu(
			*Live, ViewingSide, FactoryHex, Built, BuiltPurse, OutFailureReason))
	{
		// FORWARDED UNCHANGED AND NOT CONVERTED INTO AN EMPTY MENU. The caller keeps
		// whatever menu it already had, which is what makes a transient refusal invisible
		// to a player who was reading correct rows a moment ago.
		return false;
	}

	// §2.11.5'S FOOTER FACT, ASKED HERE SO THAT IT RIDES THE ROWS' CLOCK. Same bridge,
	// same call, same frame as the rows above it -- which is the whole reason this is not
	// read off `AppliedModel.Factories` later. `IsOpenMenuFactorySpawnBlocked`'s block
	// carries the three findings that rejected that route.
	//
	// ASKED BEFORE THE MOVE AND ASSIGNED AFTER IT, so the three members still change
	// together or not at all. `RefreshProductionMenu`'s all-or-nothing contract is about
	// the ROWS and is untouched by this: a refusal from `StratBuildProductionMenu` above
	// has already returned, and a refusal from THIS query is not a refused menu.
	bool bBlocked = false;
	const bool bBlockedKnown = Live->FactorySpawnBlockedAt(FactoryHex, bBlocked).bOk;

	ProductionMenu              = MoveTemp(Built);
	ProductionMenuHex           = FactoryHex;
	bProductionMenuSpawnBlocked = bBlockedKnown && bBlocked;
	// IN THE SAME STATEMENT GROUP, so the four members still change together or not at all.
	// [AMENDED with this line: the block above said "the three members". A count of a growing
	// set, which is the shape this project has been caught by before; the invariant -- they
	// move together or not at all -- is what the sentence is for and needs no number.]
	ProductionMenuFameTotal     = BuiltPurse;

	// A REFUSAL HERE IS NOT LOGGED, AND THAT IS A DECISION RATHER THAN AN OVERSIGHT. Two
	// refusals are reachable past a successful menu build, and neither wants a warning:
	//   - "NOT A BUILD POINT", which is the ordinary case and not a fault at all -- a
	//     player pressed the menu key over a hex that is not a factory, the rows say so in
	//     the module's own words, and a `Warning` per keypress would be noise on top of a
	//     surface that is already explaining itself.
	//   - THE CORRUPT-`defIndex` CASE `MakeUiSnapshot` guards, which is a real fault and is
	//     already loud somewhere better: `BuildViewModel` goes through the same projection,
	//     so the board itself has stopped rebuilding and `RefreshPresentation` is reporting
	//     it by name.
	// The fallback is `false` on both, which is the direction that claims nothing.
	return true;
}

bool UStratMatchSubsystem::SubmitProductionChoice(int32 DefIndex, FString& OutFailureReason)
{
	OutFailureReason.Reset();

	if (bAiTurnRunning)
	{
		// `RunAiTurnsNow` is submitting into this same bridge. `LoadMatchFromSlot` refuses
		// on the same flag for the neighbouring reason.
		OutFailureReason = TEXT(
			"an AI turn is running; a player build would interleave into its command log");
		UE_LOG(LogStratPlay, Warning, TEXT("Build refused: %s"), *OutFailureReason);
		return false;
	}

	FStratBridge* const Live = Bridge.Get();
	if (Live == nullptr)
	{
		OutFailureReason = TEXT("there is no live match to build in");
		return false;
	}

	if (!IsProductionMenuOpen())
	{
		OutFailureReason = TEXT(
			"no production menu is open, so there is no factory to build at and no row to build");
		return false;
	}

	// §2.8: THE SECOND PLAYER COMMAND PATH INTO THE BRIDGE, GATED LIKE THE FIRST.
	// `AStratPlayerController::HandleSelectionEvent` carries move, attack, wait and end turn;
	// a build arrives here instead, and a gate on one of the two would have been a lockout
	// with a hole in it. Reachable when the AI's turn ended the match while a menu was still
	// on screen. The sentence is `StratMatchAcceptsPlayerCommands`' own, so the two paths
	// cannot come to say different things.
	// ONE MODEL, BUILT HERE AND GATED ON, for the reason the free function's declaration
	// gives: a caller with a model in hand asks that model. A refusal to BUILD the model is
	// not a refusal to build a unit -- it is this object being unable to tell, and the submit
	// below is left to answer, which is the authority that owns the question anyway.
	{
		FStratViewModel Current;
		FString         ModelReason;
		FString         GateReason;
		if (BuildViewModel(Current, ModelReason)
			&& !StratMatchAcceptsPlayerCommands(Current, GateReason))
		{
			OutFailureReason = GateReason;
			return false;
		}
	}

	// THE PAIRING GUARD, AND THE ONLY THING THIS FUNCTION DECIDES. It asks whether the module
	// OFFERED this row for this factory -- never whether the module would ACCEPT it.
	// `bAffordable` and `bAvailable` are deliberately not read here: those are rules answers
	// and the submit below asks for them again, freshly, from the authority that owns them.
	const bool bOffered = ProductionMenu.ContainsByPredicate(
		[DefIndex](const FStratBuildOptionView& Row) { return Row.DefIndex == DefIndex; });
	if (!bOffered)
	{
		OutFailureReason = FString::Printf(
			TEXT("definition index %d is not a row of the production menu open at (%d, %d)"),
			DefIndex, ProductionMenuHex.X, ProductionMenuHex.Y);
		return false;
	}

	// THE ONE WRITE. Routed through `SubmitBuildAtHex` so it is stamped `{turn, side}` and
	// recorded in `RecordedLog()` on the same path as every other command -- that method's
	// own block states the obligation, and a second, non-recording apply path is the precise
	// defect `ReplayRecordedLogOnto` exists to make visible.
	const FStratResult Applied = Live->SubmitBuildAtHex(ProductionMenuHex, DefIndex);
	if (!Applied.bOk)
	{
		// REFUSED, AND THE MENU IS UNTOUCHED. Nothing moved in the rules, so nothing moves on
		// screen: the player sees the same rows and the module's own sentence about them.
		OutFailureReason = Applied.Reason;
		UE_LOG(LogStratPlay, Warning, TEXT("STRAT-CMD refused: %s"), *OutFailureReason);
		return false;
	}

	UE_LOG(LogStratPlay, Log,
		TEXT("STRAT-CMD accepted: Build definition index %d at factory (%d, %d), menu drawn for side %d."),
		DefIndex, ProductionMenuHex.X, ProductionMenuHex.Y, ViewingSide);

	// THE MENU FIRST, THE SCREEN SECOND, AND BOTH BECAUSE THE BUILD CHANGED BOTH. The factory
	// has now built this turn and the fame has been spent, so every row's answers are stale;
	// and there is a unit in the rules with no actor beside it. Both are REBUILT from a freshly
	// asked answer and neither is patched.
	//
	// A MENU REBUILD THAT REFUSES LEAVES THE PREVIOUS ROWS, by `RefreshProductionMenu`'s own
	// all-or-nothing contract, and is reported rather than swallowed.
	FString MenuReason;
	const bool bMenuRebuilt = RefreshProductionMenu(ProductionMenuHex, MenuReason);

	FString RefreshReason;
	const bool bRefreshed = RefreshPresentation(RefreshReason);

	// THE COMMAND IS ALREADY RECORDED AND CANNOT BE ROLLED BACK, so a failure past this point
	// is REPORTED AND NOT UNDONE -- exactly what `SetViewingSide` documents about its own
	// already-changed side. Both halves are attempted before either is reported, so a menu
	// that could not rebuild does not also cost the board its redraw.
	if (!bMenuRebuilt)
	{
		OutFailureReason = FString::Printf(
			TEXT("build accepted, but the menu did not rebuild: %s"), *MenuReason);
		UE_LOG(LogStratPlay, Warning, TEXT("%s"), *OutFailureReason);
		return false;
	}

	if (!bRefreshed)
	{
		OutFailureReason = FString::Printf(
			TEXT("build accepted, but the screen did not rebuild: %s"), *RefreshReason);
		UE_LOG(LogStratPlay, Warning, TEXT("%s"), *OutFailureReason);
		return false;
	}

	return true;
}

void UStratMatchSubsystem::CloseProductionMenu()
{
	ProductionMenu.Reset();

	// THE HEX GOES WITH THE ROWS, for the reason `TearDownPresentation` clears `AppliedModel`
	// beside `UnitActors`: a value left behind an emptied container reads like live state to
	// anyone who consults it without consulting the container first.
	ProductionMenuHex = FIntPoint(0, 0);

	// AND SO DOES §2.11.5'S FOOTER FACT, for the same reason and in the same statement
	// group. `IsOpenMenuFactorySpawnBlocked` also ANDs `IsProductionMenuOpen()` in, so the
	// closed-panel answer is false twice over -- deliberately, because that accessor's
	// guarantee must survive a future writer that clears the rows and forgets this line.
	bProductionMenuSpawnBlocked = false;

	// AND SO DOES §2.11.5'S HEADER NUMBER, for the hex's reason exactly. A stale purse beside
	// an emptied menu is the same defect as a stale hex: it reads as live state. ZERO IS NOT A
	// CLAIM THAT THE PLAYER HAS NO FAME -- it is what a closed panel's header is worth, and
	// `IsProductionMenuOpen()` is the discriminator, as it is for the three above.
	ProductionMenuFameTotal = 0;
}

bool UStratMatchSubsystem::IsOpenMenuFactorySpawnBlocked() const
{
	// THE OPENNESS TEST IS PART OF THE ANSWER AND NOT A GUARD IN FRONT OF IT, which is why
	// it is one expression and not an early return. `bProductionMenuSpawnBlocked` describes
	// the factory `ProductionMenu` is for; with no menu open there is no such factory, and
	// "no factory is boxed in" is the honest reading of that state rather than a fallback
	// for it. Reading the rows for the openness half keeps this on the same authority
	// `IsProductionMenuOpen` uses, so the two can never disagree about whether a menu is up.
	return IsProductionMenuOpen() && bProductionMenuSpawnBlocked;
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

	// ---- §2.11.2's action list -------------------------------------------
	// RESET ONCE, HERE, AND NOT PER TURN. A reel spans one HAND-OVER, and this loop is the
	// hand-over: an AI-vs-AI block plays several turns inside this call and the player watches
	// the whole stretch as one. `FStratAiTurnRunner::RunTurn` only ever appends, for exactly
	// this reason -- clearing there would throw away every turn but the last.
	//
	// A RUNNING TOUR IS STOPPED BEFORE THE LIST IT IS TOURING IS DESTROYED. `Peek()` returns a
	// pointer into `Steps`, and `Reset()` is what invalidates it; a timer that fired between
	// the two would read freed memory. This is the only place the two can race, and it is
	// closed by ordering rather than by a flag.
	EndAiPlaybackTour();
	AiPlaybackReel.Reset();

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

		// THE REEL IS AN OUT-PARAMETER AND CHANGES NOTHING ABOUT THE TURN. The runner writes it
		// and never reads it -- `StratAiTurnRunner.h`'s amended PACING bullet is the ruling --
		// so the turn resolves in this one synchronous call exactly as it did before §2.11.2
		// had a presentation half. Pass `nullptr` and the match is bit-identical.
		//
		// PASSED UNCONDITIONALLY AND NOT GATED ON `AiPlaybackStepSeconds`, WHICH IS A CHOICE
		// AND WAS ONCE HALF OF A DEFECT. Recording costs a few structs and makes
		// `GetAiPlaybackStepCount()` answer "what did the AI just do" in every configuration,
		// including the shipped default where no tour runs -- which is worth having. What was
		// wrong was never this line: it was that nothing then told the reel it would not be
		// toured, so a filled reel with an un-advanced cursor read as a tour in progress.
		// `BeginAiPlayback` closes that below by retiring the reel whenever it declines to arm
		// a timer. Gate this line instead and the count goes away with the defect.
		const FStratAiTurnOutcome Outcome = Runner.RunTurn(Port, &AiPlaybackReel);
		++TurnsRun;

		if (!Outcome.bOk)
		{
			// ---- ONE REFUSAL IS NOT A FAULT, AND IT IS THIS ONE --------------
			// A COMMAND THE AI APPLIED THIS TURN MAY HAVE ENDED THE MATCH. §2.8's primary
			// win is a flag kill, and the rules module then refuses the AI's own `EndTurn`
			// with `[T-SAVE-05] no match is running` -- measured 2026-08-23: the AI's turn-7
			// ninth command killed side 0's flag and its tenth, the EndTurn, was refused.
			// Reported as a fault, that reads as "the AI broke" for the single most ordinary
			// way an AI-vs-human match finishes, and it left the caller with a failure it
			// could do nothing about while the real event -- the match ending -- had no
			// name anywhere.
			//
			// ASKED AFTER, NOT PREDICTED. The model is rebuilt here rather than inferred
			// from the refusal's text, so this arm keys on the rules module's own answer and
			// not on a string that a re-vendor could reword.
			//
			// THE RUNNER IS NOT TOLD AND MUST NOT BE. `IStratAiTurnPort`'s header rules that
			// there is deliberately no `IsMatchOver()` on it, "decided before `RunTurn` is
			// called, by the subsystem that has the view model" -- this is that subsystem
			// deciding, one call later than the header's sentence implies but in the same
			// place. Adding the question to the port was the alternative and was rejected:
			// it would let a runner decide, and every test double would have to grow an arm.
			FStratViewModel AfterModel;
			FString         AfterReason;
			if (BuildViewModel(AfterModel, AfterReason) && StratMatchIsConcluded(AfterModel))
			{
				// ORDINARY, AND `StopReason` STAYS EMPTY. The refresh below reconciles the
				// final board and `ApplyView` runs `ConcludeMatchIfEnded`, which is where
				// the match actually leaves play.
				break;
			}

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

	// ---- §2.11.2's playback, AFTER the reconcile and before any return ----
	// AFTER, WHICH IS THE ORDERING THE WHOLE DESIGN RESTS ON. `RefreshPresentation` has just
	// put the actors where the FINAL view model says they are, so the tour steps a camera over
	// a finished board rather than an unfolding one. `StratAiPlayback.h` argues why that is
	// what §2.11.2 asks of a presentation layer that is reconciled rather than evented, and
	// what the alternative would have cost.
	//
	// ON EVERY PATH OUT, INCLUDING BOTH FAILURES BELOW. What the AI got through is worth
	// watching whether or not its turn finished and whether or not the refresh did -- and a
	// tour that went missing on the refusal path would go missing in exactly the case a player
	// most wants to see. It is inert unless `AiPlaybackStepSeconds` is positive.
	BeginAiPlayback();

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
// §2.11.2's playback. Six functions, and not one of them can move the match:
// nothing below constructs an `FStratAiTurnRunner`, names an `IStratAiTurnPort`,
// calls an `FStratBridge` method or writes an `FStratViewModel`. The most any of
// them does is point a camera. That is the property `T-TURN-09`'s state-equality
// half is asserted over, and it is structural -- there is no route from here to a
// command -- rather than a promise this file is making about its own behaviour.
// ---------------------------------------------------------------------------

void UStratMatchSubsystem::BeginAiPlayback()
{
	// ---- THE ARMING HALF OF THE INVARIANT ---------------------------------
	// THE CURSOR IS AT THE END UNLESS A TOUR IS ACTUALLY UNDER WAY. This is the only function
	// that arms `AiPlaybackTimer`, and every exit below that declines to arm one retires the
	// reel before returning.
	//
	// [CORRECTED 2026-08-29, SECOND FIX. THIS BLOCK WAS HEADED "THE INVARIANT THIS FUNCTION
	// OWNS" AND SAID THIS FUNCTION WAS "THE ONLY ONE IN A POSITION TO KNOW". That was true of
	// ARMING and false of DISARMING: the clock was stopped from five other places and two of
	// them -- `Deinitialize` and `TearDownPresentation` -- stopped it without retiring, so a
	// reseed mid-tour stranded the cursor and reinstated the swallow on a brand-new match.
	// The disarming half now lives in `EndAiPlaybackTour`, which is the only thing that stops
	// the clock and always retires. TWO OWNERS, BOTH NAMED. A prose block that asserts a
	// sole-owner property the code does not have is the same failure as the one struck in
	// `StratPlayerController.cpp`, and it is corrected here rather than only reported.]
	//
	// IT IS A DEFECT FIX AND THE DEFECT IS WORTH NAMING RATHER THAN QUIETLY REPAIRING. The
	// reel is filled on EVERY hand-over -- `RunAiTurnsNow` passes it to the runner
	// unconditionally, deliberately, so `GetAiPlaybackStepCount()` answers whatever the
	// configuration -- while only the timer was gated. So at the shipped
	// `AiPlaybackStepSeconds` of zero the reel ended each hand-over non-empty with the cursor
	// at 0, `IsAiPlaybackRunning()` read true, `SkipAiPlayback()` succeeded, and
	// `AStratPlayerController::HandleSelectionEvent` consumed the first click or Esc after
	// every single AI turn. Measured by the test author and confirmed at the source.
	//
	// GATING `SkipAiPlayback` ON THE CONFIG WAS THE ALTERNATIVE AND IT WAS REJECTED. It
	// repairs the zero case and leaves the third exit below -- a positive interval with no
	// world -- reporting a tour that nothing will ever step. Putting the invariant where the
	// arming decision is made covers every reason not to arm, including reasons nobody has
	// written yet, because there is one decision and it is here.
	//
	// `RetireReel` AND NOT `AiPlaybackReel.Reset()`. Retiring moves the cursor and KEEPS
	// `Steps`, so the count survives; `Reset()` would throw away the one fact that
	// distinguishes a tour that was cut short from a reel that was never filled. `Reset()`
	// belongs to the START of a hand-over and to a MATCH boundary, and is called from exactly
	// those two places.
	const auto RetireReel = [this]()
	{
		AiPlaybackReel.SkipToEnd();
	};

	if (ActiveConfig.AiPlaybackStepSeconds <= 0.0f)
	{
		// THE SHIPPED DEFAULT, AND ORDINARY. See the field: zero plays nothing, so every
		// existing caller and every automation test runs down the path it always did -- which
		// now includes the input path, because a retired reel consumes no click.
		RetireReel();
		return;
	}

	if (AiPlaybackReel.Num() <= 0)
	{
		// The AI did nothing this hand-over -- a side with no units, or a turn that refused on
		// its first command. Not a fault and nothing to tour. Retired for form and for the
		// next reader rather than out of need: an empty reel is already not playing, and an
		// exit here that did not retire would be the one exception someone later has to
		// re-derive.
		RetireReel();
		return;
	}

	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		// CONFIGURED FOR PLAYBACK WITH NO WORLD TO TIME AGAINST, which is `RunAiTurnsIfDue`'s
		// case and gets the same treatment for the same reason: presentation degrades and the
		// match does not. The reel keeps its CONTENTS, so `GetAiPlaybackStepCount()` still
		// answers; the cursor is retired, so nothing waits on a clock that does not exist.
		// THIS IS THE EXIT A CONFIG GATE ON `SkipAiPlayback` WOULD HAVE MISSED.
		RetireReel();
		return;
	}

	// ONE TIMER, RESET RATHER THAN STACKED, on `RunAiTurnsIfDue`'s line. The reset above
	// already cleared it; this is the arming.
	World->GetTimerManager().SetTimer(AiPlaybackTimer, this,
		&UStratMatchSubsystem::OnAiPlaybackTimer,
		ActiveConfig.AiPlaybackStepSeconds, /*bLoop=*/true);

	// THE FIRST STEP IS SHOWN IMMEDIATELY AND NOT ONE INTERVAL LATER. A looping timer fires
	// first at `Rate`, so without this the camera would sit still for half a second at the
	// start of every hand-over -- which reads as the playback having failed to start rather
	// than as pacing.
	OnAiPlaybackTimer();
}

void UStratMatchSubsystem::OnAiPlaybackTimer()
{
	// ONE LINE, AND THE WHOLE POINT OF IT IS THAT IT IS ONE LINE. See the declaration: the
	// step-and-stop body used to live here, where nothing without a ticking world could reach
	// it, and so nothing did. It is `AdvanceAiPlaybackOneStep` now and this is a caller of it
	// rather than a second copy of it.
	AdvanceAiPlaybackOneStep();
}

bool UStratMatchSubsystem::AdvanceAiPlaybackOneStep()
{
	const FStratAiPlaybackStep* const Step = AiPlaybackReel.Peek();
	if (Step == nullptr)
	{
		// NOTHING AT THE CURSOR. Three ways to be here and all are ordinary: the last
		// `Advance()` ran off the end, `SkipAiPlayback` moved the cursor there between two
		// ticks, or `BeginAiPlayback` retired the reel because no tour was ever going to run
		// -- which is the shipped default and is how this reads for a caller that hand-drives
		// a subsystem nobody configured for playback.
		//
		// STOPS THE TIMER ANYWAY. Usually there is none, and clearing an unarmed handle is
		// free; when there IS one it is the arm that stops a tour that outlived its reel.
		EndAiPlaybackTour();
		return false;
	}

	// A COPY AND NOT THE POINTER, because `FocusPlaybackStep` reaches actors and this
	// function calls `Advance()` afterwards. Neither can reallocate `Steps` today -- nothing
	// on either path records -- and the copy is six ints, so the cost of not having to
	// re-establish that argument after some later change is zero.
	const FStratAiPlaybackStep Current = *Step;

	FocusPlaybackStep(Current);
	AiPlaybackReel.Advance();

	if (!AiPlaybackReel.IsPlaying())
	{
		// STOPPED HERE RATHER THAN ON THE NEXT TICK, so the timer does not sit armed for one
		// more interval after the last action was shown. THIS IS ALSO WHAT MAKES HAND-DRIVING
		// EQUIVALENT TO LETTING THE CLOCK RUN: a caller that steps this to the end leaves
		// exactly the state the timer would have left, with no armed handle behind it.
		EndAiPlaybackTour();
	}

	return true;
}

void UStratMatchSubsystem::EndAiPlaybackTour()
{
	// THE CLOCK FIRST AND THE CURSOR SECOND, which is ordering and not style: `Peek()` hands
	// out a pointer into `Steps` and a timer callback holds one for the length of a tick. The
	// cursor move below cannot reallocate anything, so this order is not load-bearing today --
	// it is written this way so that a later change which DOES touch `Steps` here has an
	// obvious place to be wrong in, on `Deinitialize`'s stated principle.
	if (UWorld* const World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AiPlaybackTimer);
	}

	// THE HALF THAT USED TO BE MISSING. See the declaration: stopping the clock and leaving a
	// live cursor is what let a reseed mid-tour swallow the first input of the NEXT match, and
	// there is deliberately no verb in this class that does one without the other.
	//
	// A NO-OP AT FOUR OF THE SIX CALL SITES AND THAT IS THE POINT. `SkipToEnd` returns false
	// and changes nothing when the cursor is already at the end, so the three sites that
	// arrive here finished pay nothing, and `RunAiTurnsNow` -- which `Reset()`s on its next
	// line -- is unaffected. The cost of the guarantee is one comparison on the paths that
	// did not need it.
	AiPlaybackReel.SkipToEnd();
}

void UStratMatchSubsystem::FocusPlaybackStep(const FStratAiPlaybackStep& Step) const
{
	if (!Step.bHasHex)
	{
		// THE CLOSING EndTurn. It is recorded -- the list is "what the AI did" and ending the
		// turn is one of the things it did -- and it has no hex to look at. See
		// `FStratAiPlaybackStep::bHasHex` on why `Hex` cannot be trusted to say so itself.
		return;
	}

	if (Board == nullptr)
	{
		return;
	}

	const UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	const APlayerController* const PC = World->GetFirstPlayerController();
	if (PC == nullptr)
	{
		return;
	}

	// THE POSSESSED PAWN AND NOT A `TActorIterator`, which was the other shape. A level with
	// two camera pawns in it would give an iterator no way to choose, and the one the player is
	// looking through is by definition the one they are possessing. A level where the pawn is
	// something else gets no camera motion and no complaint -- see the declaration on why this
	// degrades instead of refusing.
	AStratCameraPawn* const Camera = Cast<AStratCameraPawn>(PC->GetPawn());
	if (Camera == nullptr)
	{
		return;
	}

	// THE BOARD OWNS THE CONVERSION. See the declaration: `WorldLocationOfHex` is the one
	// hex -> world map in the project and this file does not get a second one.
	Camera->FocusWorldLocation(Board->WorldLocationOfHex(Step.Hex));
}

bool UStratMatchSubsystem::SkipAiPlayback()
{
	// THE REEL DECIDES WHETHER ANYTHING HAPPENED, NOT THE TIMER. `FStratAiPlaybackReel::SkipToEnd`
	// returns false when the cursor was already at the end, which is the same question
	// "was a tour running" asked of the thing that actually knows -- a timer handle can be
	// armed for one more tick after the last action was shown.
	if (!AiPlaybackReel.SkipToEnd())
	{
		return false;
	}

	EndAiPlaybackTour();

	// NOTHING IS RECONCILED HERE AND NOTHING NEEDS TO BE. §2.11.2's "skips to the end state"
	// is already satisfied: `RunAiTurnsNow` reconciled to the final view model before the tour
	// began, so the end state has been on screen the whole time. A `RefreshPresentation` call
	// here would repaint an unchanged screen and would be the first thing to look at when
	// somebody later believes this function moves the board. It does not.
	UE_LOG(LogStratPlay, Verbose,
		TEXT("STRAT-AI playback skipped at step %d of %d"),
		AiPlaybackReel.GetCursor(), AiPlaybackReel.Num());

	return true;
}

bool UStratMatchSubsystem::IsAiPlaybackRunning() const
{
	// ASKED OF THE REEL AND NOT OF `FTimerManager::IsTimerActive`, for `SkipAiPlayback`'s
	// reason: the cursor is the fact and the timer is a consequence of it, and a caller that
	// consulted the timer would get `true` for one interval after the final action was shown.
	return AiPlaybackReel.IsPlaying();
}

int32 UStratMatchSubsystem::GetAiPlaybackStepCount() const
{
	return AiPlaybackReel.Num();
}

int32 UStratMatchSubsystem::GetAiPlaybackCursor() const
{
	return AiPlaybackReel.GetCursor();
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

bool UStratMatchSubsystem::IsPayloadRestorable(const UStratSaveGame* const Payload,
                                               FText&                      OutRefusalReason)
{
	OutRefusalReason = FText::GetEmpty();

	if (Payload == nullptr)
	{
		// `LoadGameFromSlot` returns a `USaveGame*` and the cast is the class check. A slot
		// written by a different game, or by a build in which this class was renamed, lands
		// here rather than as a crash on the first field read.
		OutRefusalReason = FText::FromString(TEXT("does not hold a UStratSaveGame"));
		return false;
	}

	// THE VERSION GATE, AND IT REFUSES RATHER THAN GUESSES. `UStratSaveGame`'s header states
	// the contract: a change to the MEANING of a field bumps the version and grows an arm
	// here. There is one version today, so there is one arm and it is equality -- written
	// out anyway, because the failure a version exists to prevent is the one where a newer
	// slot loads into an older build and every new field reads as its default with nothing
	// said.
	//
	// IT CANNOT CATCH A FRESHLY CREATED PAYLOAD AND NEVER COULD, which is worth stating where
	// a reader might expect it to. `UStratSaveGame::SavedDataVersion` is initialised TO
	// `kCurrentSavedDataVersion`, so anything this build creates is current by construction.
	// The check below is what catches a completion-flag-only slot; this one is not.
	if (Payload->SavedDataVersion != UStratSaveGame::kCurrentSavedDataVersion)
	{
		OutRefusalReason = FText::FromString(FString::Printf(
			TEXT("was written at SavedDataVersion %d and this build reads %d"),
			Payload->SavedDataVersion, UStratSaveGame::kCurrentSavedDataVersion));
		return false;
	}

	// THE ONE THAT ACTUALLY SEPARATES A SAVED MATCH FROM A TOUCHED SLOT.
	// `RecordMatchCompletionOnSave` writes a payload with an empty `SaveText` on purpose --
	// its own comment says so -- so a file existing proves only that the player finished a
	// match, never that they saved one.
	if (Payload->SaveText.IsEmpty())
	{
		OutRefusalReason = FText::FromString(TEXT("carries no §4.10 text"));
		return false;
	}

	return true;
}

bool UStratMatchSubsystem::DoesSlotHoldARestorableMatch(const FString& SlotName,
                                                        FText&         OutRefusalReason)
{
	OutRefusalReason = FText::GetEmpty();

	if (SlotName.IsEmpty())
	{
		// REFUSED RATHER THAN RESOLVED. This function is static precisely so a caller with no
		// configured subsystem can use it, and such a caller has no `ActiveConfig` to fall
		// back to; inventing one here would be the second author of a slot name that
		// `ResolveSaveSlotName` refuses in its own words.
		OutRefusalReason = FText::FromString(TEXT("no slot name was given"));
		return false;
	}

	if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		OutRefusalReason = FText::FromString(TEXT("no save exists in this slot"));
		return false;
	}

	return IsPayloadRestorable(
		Cast<UStratSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0)),
		OutRefusalReason);
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

// ---------------------------------------------------------------------------
// §2.8's end of match: the predicate, the gate, and the one-shot transition.
// See the block above the two free functions in the header for the measurement
// that made all three necessary.
// ---------------------------------------------------------------------------

bool StratMatchIsConcluded(const FStratViewModel& Model)
{
	// ONE FIELD. See the declaration on why `ResultTier` is not consulted here.
	return Model.Match.bHasResult;
}

int32 StratDifficultyFameDelta(EStratDifficulty Difficulty)
{
	// Sec 2.9's three numbers and nothing else. NO `default:` LABEL, so that adding a
	// tier to the enum is a compiler warning here rather than a silent 0 at runtime --
	// and a silent 0 is exactly the failure that would read as "the handicap works,
	// this tier just happens to be even".
	switch (Difficulty)
	{
	case EStratDifficulty::Easy:   return 150;
	case EStratDifficulty::Normal: return 0;
	case EStratDifficulty::Hard:   return -100;
	}

	// Unreachable over the enum above, and present only because a `UENUM` is a `uint8`
	// and a Blueprint can hold a byte the enum never named. Even is the safe answer:
	// it leaves the scenario's configured value standing.
	return 0;
}

int32 StratHandicappedSide(const FStratMatchConfig& Config)
{
	// ARM ONE -- the shipped hot seat. See the declaration: Sec 2.7 calls this
	// "Single-player difficulty", and with no AI seat there is no player-versus-opponent
	// asymmetry for a starting-Fame handicap to express. This is the arm that keeps every
	// existing hot-seat opening at the scenario's own 200/200.
	if (Config.AiSides.Num() == 0)
	{
		return INDEX_NONE;
	}

	// ARM TWO -- the screen opens on an AI seat. Either both sides are AI, which is what
	// phase D's AI-vs-AI gate configures and which must not move, or the configuration is
	// inverted. Neither is a match with a human whose side Sec 2.7 would move.
	if (Config.AiSides.Contains(Config.ViewingSide))
	{
		return INDEX_NONE;
	}

	return Config.ViewingSide;
}

bool StratMatchAcceptsPlayerCommands(const FStratViewModel& Model, FString& OutRefusalReason)
{
	OutRefusalReason.Reset();

	if (!StratMatchIsConcluded(Model))
	{
		return true;
	}

	OutRefusalReason = StratMatchConcludedRefusalText();
	return false;
}

FString StratMatchConcludedRefusalText()
{
	// A SENTENCE FOR A HUMAN AND NOT A RULES VERDICT, on `FStratGuidedOpening`'s line: an
	// inert input is one that was never asked about. It deliberately does not name a winner,
	// because this layer has none to name -- see the header.
	return TEXT("the match is over; no further commands are accepted");
}

bool UStratMatchSubsystem::IsMatchConcluded() const
{
	FStratViewModel Model;
	FString         Reason;
	if (!BuildViewModel(Model, Reason))
	{
		// NOT "THE MATCH IS OVER". This object could not read the state, which establishes
		// nothing -- see the declaration.
		return false;
	}

	return StratMatchIsConcluded(Model);
}

FStratMatchView UStratMatchSubsystem::GetConcludedMatchView() const
{
	// FROM WHAT WAS DRAWN AND NOT FROM A FRESH QUERY, exactly as `GetViewModel` answers.
	// Before any `ApplyView` this is a default-constructed view whose `bHasResult` is false,
	// which is the right answer to "what result was shown" when none was.
	return AppliedModel.Match;
}

bool UStratMatchSubsystem::GetMatchResult(FStratMatchResultView& OutResult,
                                          FString&               OutFailureReason)
{
	OutFailureReason.Reset();

	const FStratBridge* const Live = Bridge.Get();
	if (Live == nullptr)
	{
		// NAMED HERE rather than left to arrive as some later refusal, on
		// `RefreshProductionMenu`'s reasoning: "there is no match" and "the match has no
		// result yet" send the next reader to different files, and a result screen must
		// never be told the second when the first is true.
		OutFailureReason = TEXT("there is no live match to read a result from");
		return false;
	}

	// A FRESH QUERY AND NOT `AppliedModel`, which is the whole difference from
	// `GetConcludedMatchView` above. The winner is not on the view model and is not going to
	// be; there is therefore nothing cached to project, and this is the only honest shape.
	// The declaration records the trade that buys.
	//
	// ALL-OR-NOTHING IS `StratBuildMatchResult`'S and is not re-implemented here: it assigns
	// on its last line, so a refusal leaves the caller's value untouched and this function
	// has nothing of its own to roll back.
	return StratBuildMatchResult(*Live, OutResult, OutFailureReason);
}

void UStratMatchSubsystem::ConcludeMatchIfEnded(const FStratViewModel& Model)
{
	// §2.11.6's completion writer runs FIRST and on every refresh, unlatched by this
	// function, because it has its own latch, its own opt-in and its own retry-on-failure
	// behaviour -- see its declaration. Ordering it inside this function's latch would have
	// made a transient write failure permanent.
	NoteMatchResultIfEnded(Model);

	if (bMatchConclusionAnnounced || !StratMatchIsConcluded(Model))
	{
		return;
	}

	bMatchConclusionAnnounced = true;

	// THE PACING TIMER, CLEARED RATHER THAN LEFT TO FIRE AND FIND NOTHING TO DO. Belt and
	// braces; the reason is in the declaration.
	if (UWorld* const World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AiTurnTimer);
	}

	// ONE LINE, ONCE PER MATCH, AND IT IS THE FIRST END-OF-MATCH LINE THIS PROJECT HAS EVER
	// LOGGED. The 2026-08-23 session's diagnosis had to be built entirely out of the ABSENCE
	// of a transition, because there was no line whose absence would have proved anything.
	// `STRAT-MATCH` is a new prefix and it is fixed-field on the `STRAT-CMD` / `STRAT-AI`
	// pattern, so a gate can split it without knowing its content.
	UE_LOG(LogStratPlay, Log,
		TEXT("STRAT-MATCH concluded turn=%d turnCap=%d sideToMove=%d tier=%s"),
		Model.Match.Turn, Model.Match.TurnCap, Model.Match.SideToMove,
		*StaticEnum<EStratResultTier>()->GetNameStringByValue(
			static_cast<int64>(Model.Match.ResultTier)));

	// ---- GDD Sec 2.11.4's END-OF-MATCH SCREEN ------------------------------
	// AFTER THE LOG LINE AND INSIDE THE LATCH, which places it exactly: the log records that
	// the transition happened whether or not anything drew it, and the latch is what makes the
	// screen appear once rather than on every refresh of a finished match. Sec 2.11.4 says the
	// screen shows the verdict; it does not say the verdict stops existing when nobody
	// configured a widget, and a gate reading `STRAT-MATCH concluded` must not depend on an
	// asset.
	//
	// THROUGH THE HUD BECAUSE THE HUD OWNS THE WIDGET, and this module names no `UMG`, `Slate`
	// or `SlateCore` -- the module-arrow reasoning `ApplyView`'s guidance block already states
	// in full for `PushGuidance`. `ShowMatchResult` takes and returns only engine types.
	//
	// IT REBUILDS FROM THE BRIDGE RATHER THAN BEING HANDED `Model`, and that is deliberate.
	// The winner is NOT on the view model and is not going to be --
	// `FStratMatchResultView`'s own block records that decision made three times down the
	// stack -- so there is nothing in `Model` to pass. `StratBuildMatchResultModel` asks the
	// same live bridge this model was built from, on the same frame, which is the arrangement
	// `GetMatchResult`'s declaration already describes for the reflected reader.
	//
	// REFUSALS ARE LOGGED AND NOT PROPAGATED, for this function's stated reason: reconciliation
	// is not a request that can be declined, and an unset `MatchResultWidgetClass` is a
	// configuration rather than a fault. Warning rather than Error because the match is over
	// either way and every other surface is correct -- what is missing is the screen that says
	// so, and the log line above already said it.
	if (AStratScoreboardHUD* const HUD = FindScoreboardHUD())
	{
		FString ResultReason;
		if (!HUD->ShowMatchResult(ResultReason))
		{
			UE_LOG(LogStratPlay, Warning,
				TEXT("No end-of-match screen this match: %s"), *ResultReason);
		}
	}
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

	// THE THREE PAYLOAD REFUSALS -- wrong class, wrong version, no §4.10 text -- MOVED TO
	// `IsPayloadRestorable` AND ARE ASKED HERE RATHER THAN RESTATED. §2.11.5's menu has to ask
	// the same question before it offers Continue, and a second copy of these conditions is
	// exactly the drift that let it offer a restore of a completion-flag-only slot. The
	// messages are unchanged: each refusal is a phrase completing "slot 'X' ...", so this line
	// produces the sentences this function has always produced.
	FText Unrestorable;
	if (!IsPayloadRestorable(Payload, Unrestorable))
	{
		OutFailureReason = FString::Printf(
			TEXT("slot '%s' %s"), *Slot, *Unrestorable.ToString());
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

	// AND THE PLAYBACK TOUR, FOR A REASON THIS FUNCTION OWNS RATHER THAN INHERITS: it is about
	// to `Destroy()` every unit actor and drop `Board`, and `FocusPlaybackStep` reads `Board`.
	// A tour left running across a reseed would also be panning around the OLD match's hexes on
	// the new one's board, which is the more visible of the two failures.
	//
	// THIS CALL SITE IS ONE OF THE TWO THE RESEED DEFECT CAME THROUGH, and it is fixed by
	// `EndAiPlaybackTour` retiring rather than by anything written here -- which is the whole
	// point of moving the guarantee into the verb. `StartMatchInternal` calls this function
	// unconditionally and `LoadMatchFromSlot` reaches it through that, so this is the path a
	// player takes between two matches.
	EndAiPlaybackTour();

	// AND THE LIST ITSELF, WHICH IS THIS FUNCTION'S ALONE AND IS NOT FOLDED INTO THE VERB
	// ABOVE. `EndAiPlaybackTour` deliberately keeps `Steps` -- `AdvanceAiPlaybackOneStep` and
	// `SkipAiPlayback` both need `GetAiPlaybackStepCount()` to survive a tour ending, and it is
	// the only thing that tells a tour cut short from a reel never filled. But a MATCH boundary
	// is different in kind from a tour ending: this is the previous match's scratchpad, nobody
	// can tour it and nobody should read it, so it goes.
	//
	// THE SPLIT IS DELIBERATE AND THE TWO HALVES ARE NOT EQUALLY DANGEROUS TO FORGET. Failing
	// to retire the cursor SWALLOWS THE PLAYER'S INPUT, so that half is inside the verb and
	// cannot be forgotten by a new call site. Failing to clear `Steps` can only make a readout
	// report the wrong match's count until the next hand-over's `Reset()`, so that half is one
	// explicit line at the one place a match boundary is crossed. Folding it into the verb
	// would break the three sites that need the count to survive.
	//
	// AFTER THE CALL ABOVE AND NEVER BEFORE IT: `Reset()` is what invalidates a `Peek()`
	// pointer, and the timer that could be holding one is cleared on the line above. Same
	// ordering as `RunAiTurnsNow`, for the same reason.
	AiPlaybackReel.Reset();

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

	// THE PRODUCTION MENU GOES WITH THE MATCH, not with the world. A buildlist describes one
	// factory in one `strat::GameState`; carried across a reseed it would offer rows against a
	// board that no longer exists, and `SubmitProductionChoice` would submit one at a hex the
	// new scenario need not have a factory on. Cleared here rather than only in `Deinitialize`
	// because a reseed and a world death are different events and both invalidate it.
	CloseProductionMenu();

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
