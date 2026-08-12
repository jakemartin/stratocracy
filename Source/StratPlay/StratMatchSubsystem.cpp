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

#include "StratBoardActor.h"
#include "StratPlay.h"
#include "StratUnitActor.h"

#include "StratScoreboardHUD.h"

// See the block above. This line is legal here and nowhere else in this class.
#include "StratBridge.h"

#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Misc/Paths.h"

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

bool UStratMatchSubsystem::StartMatch(const FStratMatchConfig& Config, FString& OutFailureReason)
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

	ActiveConfig = Config;
	ViewingSide = Config.ViewingSide;

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

	// FROM HERE THE MATCH IS LIVE AND NOTHING BELOW MAY UNDO THAT. Every remaining step is
	// presentation, and a presentation gap is reported without tearing down a correctly
	// seeded match -- see the declaration on why this function is all-or-nothing on the
	// rules side and not on the presentation side.

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

	// CACHED AFTER THE FACT AND NEVER READ BACK. See `GetViewModel`: this is a record of
	// what was applied, not an input to anything above.
	AppliedModel = Model;
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
