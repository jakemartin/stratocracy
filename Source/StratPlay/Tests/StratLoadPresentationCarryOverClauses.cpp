// T-SAVE-04 -- controller-side PRESENTATION state across `UStratMatchSubsystem::LoadMatchFromSlot`.
//
// WHAT THIS FILE COVERS THAT NO OTHER ONE DOES. `StratSaveSlotClauses.cpp` pins where a save
// LIVES and that a loaded match reaches the saved canonical state hash;
// `StratBridgeRestoreParity.cpp` pins the §4.10 text. Both are about the RULES side. This file
// is about the half of the screen that is NOT on the rules side: `FStratSelectionMachine` and
// `FStratBuildAffordance`, which are members of `AStratPlayerController` and are therefore
// untouched by anything the subsystem clears.
//
// THE MECHANISM, STATED SO A READER KNOWS WHAT IS BEING MEASURED RATHER THAN GUESSED AT.
// `LoadMatchFromSlot` reaches `TearDownPresentation` through `StartMatchInternal`, and
// `TearDownPresentation` clears SUBSYSTEM-side presentation state -- the AI playback tour, the
// reel, `AppliedModel`, the receipt mark, the production menu. It cannot clear the selection
// machine or the build affordance, because those two live on a different object. The clearing
// lives on one object and the state lives on another.
//
// WHERE THE EXPECTATION COMES FROM, AND IT IS NOT COMPUTED HERE. Every expectation below is an
// ORACLE's answer: a second `FStratSelectionMachine` and a second `FStratBuildAffordance`,
// driven through the SAME public calls as the controller's own pair, and then put through
// their own `Reset()` -- the method `StratBuildAffordance.h` documents as being "FOR A LOAD OR
// A RESEED", naming the discharging condition as "a load or reseed path that calls both". So
// what is asserted is: after a load, the controller's pair answers as a pair that has been
// `Reset()` answers. Nothing in this file spells a `bDone`, a focus flag or a refusal
// sentence; the module produces both sides.
//
// ONE CLAUSE OF THAT PARAGRAPH IS RETRACTED IN PLACE, AND THIS CLAUSE'S OWN FIX IS WHAT
// FALSIFIED IT. Between "A RESEED" and "naming", it used to read:
// RETRACTED> "and records as having `NO SHIPPING CALLER TODAY`"
// True when this file was written, over `283d711`; false from `strat-gameplay-engineer`'s
// T-SAVE-04 fix onward. `AStratPlayerController::SyncPresentationToMatchEpoch` now calls
// `FStratSelectionMachine::Reset()` and `FStratBuildAffordance::Reset()` on consecutive
// statements when it observes that `UStratMatchSubsystem::GetMatchEpoch()` has moved -- the
// discharging condition those two headers named, met exactly rather than approximately. Both
// headers now carry the correction in place; this file carried a COPY of the claim, so it
// carries a copy of the correction, because a reader who arrived at the claim here would
// otherwise never reach the retraction there.
//
// WHAT IS NOT RETRACTED IS THE ORACLE. Neither `Reset()` BODY changed in that fix, so the
// expectation side of every assertion below is the same function it always was -- which is the
// only reason the green that followed the fix means anything rather than recording an oracle
// that moved under its own clause. IT ALSO MAKES EITHER `Reset()` BODY AN OFF-LIMITS FIX SITE
// FOR THIS CLAUSE, and that is stated rather than left to be discovered: editing one MOVES the
// expectation instead of being tested against it, so this clause would go green on a change
// that repaired nothing.
//
// THE STATE IS NOT REACHED THROUGH THE DEFECT. The done bit is put into the machine by
// `HandleEvent(HexPrimary)` followed by `HandleEvent(Wait)` -- §2.11.1's wait, the machine's
// own public input path, which `StratHotSeatReplayParity.cpp` drives the same way -- and the
// focus by `Observe` + `NoteHexPrimary`, which is `StratBuildAffordanceClauses.cpp`'s path. A
// fix that gives the load a `Reset()` call changes neither of those, so the fixture does not
// reach its own precondition through the thing under test.
//
// THE SLOT IS THIS FILE'S OWN. `FStratMatchConfig::SaveSlotName` defaults to
// `StratocracyMatch`, which is the PLAYER's slot; a gate that wrote there would destroy a
// developer's in-progress game on every suite run and make the suite machine-dependent. Every
// path below names `kTestSlotName` explicitly and the slot is deleted on entry and on exit.
//
// THE CONTROLLER'S `BeginPlay` IS NOT DISPATCHED, and the decorated model is obtained by
// calling `AStratPlayerController::DecorateForPresentation` DIRECTLY -- which is exactly what
// `RefreshFromMachine` does, and for the reason that function's own block gives: it decorates
// because it is that controller, not because a registration happened to be in place.
// `StratGuidanceRouteClauses.cpp` measures that asymmetry; this file relies on it.
//
// THIS FIXTURE HAS NO TILE MESHES, so the board's warning is declared, as every fixture in
// this directory declares it.
//
// NO `strat::` FREE FUNCTION IS CALLED ANYWHERE BELOW -- LNK2019 outside StratBridge and
// StratRules, measured 8 x.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Math/IntPoint.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratBuildAffordance.h"
#include "StratMatchSubsystem.h"
#include "StratPlayerController.h"
#include "StratSelectionMachine.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

namespace StratLoadPresentationCarryOver
{
	static const int32 kFirstSide   = 0;
	static const int32 kViewingSide = 0;
	static const int32 kUserIndex   = 0;

	/** This file's slot and never the configured default. See the file block. */
	static const TCHAR* kTestSlotName = TEXT("StratocracyAutomation_LoadPresentationClauses");

	/** Supplied on both the write and the read, so the round trip is self-consistent by
	 *  construction and no re-vendor can move it. `StratSaveSlotClauses.cpp`'s reasoning. */
	static const TCHAR* kRulesCommit = TEXT("automation-rules-commit");
	static const TCHAR* kDataHash    = TEXT("automation-data-hash");

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	static bool MakeConfig(FStratMatchConfig& Out, FString& OutError)
	{
		Out.UnitTable    = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		Out.TerrainTable = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
		if (Out.UnitTable == nullptr || Out.TerrainTable == nullptr)
		{
			OutError = TEXT("DT_Units and/or DT_Terrain did not load from /Game/StratData");
			return false;
		}

		Out.ScenarioFile    = TEXT("Data/ferrum_crossing.json");
		Out.FirstSide       = kFirstSide;
		Out.ViewingSide     = kViewingSide;
		Out.BoardActorClass = AStratBoardActor::StaticClass();
		Out.UnitActorClass  = AStratUnitActor::StaticClass();
		Out.RulesCommit     = kRulesCommit;
		Out.DataHash        = kDataHash;
		Out.SaveSlotName    = kTestSlotName;
		return true;
	}

	/** The transient world, with the engine context teardown wants. The world first and the
	 *  context second -- `StratSaveSlotClauses.cpp` records the measurement. */
	struct FTestWorldScope
	{
		UWorld* World = nullptr;

		FTestWorldScope()
		{
			World = UWorld::CreateWorld(EWorldType::Game, /*bInformEngineOfWorld*/ false);
			if (World != nullptr && GEngine != nullptr)
			{
				FWorldContext& Context = GEngine->CreateNewWorldContext(EWorldType::Game);
				Context.SetCurrentWorld(World);
			}
		}

		~FTestWorldScope()
		{
			if (World != nullptr)
			{
				World->DestroyWorld(false);
				if (GEngine != nullptr)
				{
					GEngine->DestroyWorldContext(World);
				}
				World = nullptr;
			}
		}

		FTestWorldScope(const FTestWorldScope&) = delete;
		FTestWorldScope& operator=(const FTestWorldScope&) = delete;
	};

	/** Deletes this file's slot on the way in AND on the way out. */
	struct FSlotScope
	{
		FSlotScope()  { Clear(); }
		~FSlotScope() { Clear(); }

		static void Clear()
		{
			if (UGameplayStatics::DoesSaveGameExist(kTestSlotName, kUserIndex))
			{
				UGameplayStatics::DeleteGameInSlot(kTestSlotName, kUserIndex);
			}
		}

		FSlotScope(const FSlotScope&) = delete;
		FSlotScope& operator=(const FSlotScope&) = delete;
	};

	/**
	 * The first unit of the side to move that this machine will actually let the player
	 * select. ENUMERATED FROM THE DECORATED MODEL and never spelled, so a scenario edit needs
	 * no edit here.
	 *
	 * `bLockedThisTurn` IS IN THE FILTER AND IT IS NOT DEFENSIVE PADDING -- IT WAS MEASURED.
	 * `AStratPlayerController::DecorateForPresentation` arms §2.11.6's guided opening and
	 * `FStratGuidedOpening::Observe` writes the lock set into the very machine this fixture
	 * then drives, so the first unit of the side to move is LOCKED on the opening and
	 * `HandleEvent`'s selection gate refuses it. The first version of this file did not
	 * filter and failed with "the wait did not mark unit 1 done" -- a fixture that never
	 * reached its own precondition, which would have been a red clause measuring nothing.
	 */
	static const FStratUnitView* FindMovableUnit(const FStratViewModel& Model)
	{
		for (const FStratUnitView& U : Model.Units)
		{
			if (U.Side == Model.Match.SideToMove && !U.bHasMoved && !U.bHasActed
				&& !U.bLockedThisTurn && !U.bDone)
			{
				return &U;
			}
		}
		return nullptr;
	}

	static const FStratUnitView* FindUnitById(const FStratViewModel& Model, int32 UnitId)
	{
		for (const FStratUnitView& U : Model.Units)
		{
			if (U.UnitId == UnitId)
			{
				return &U;
			}
		}
		return nullptr;
	}

	/** A factory the viewing seat holds, if the scenario gives it one. */
	static bool FindHeldFactory(const FStratViewModel& Model, FIntPoint& OutHex)
	{
		for (const FStratFactoryView& Factory : Model.Factories)
		{
			if (Factory.Owner == Model.ViewingSide)
			{
				OutHex = Factory.Hex;
				return true;
			}
		}
		return false;
	}

	/**
	 * §2.11.1's wait, through the machine's own two public calls and nothing else.
	 *
	 * `Wait` PRODUCES `EStratSelectionCommand::None` and adds to `DoneUnits` inside
	 * `HandleEvent` -- there is no `Wait` in `strat::SaveCommandKind` and the machine's header
	 * says so. `NotifyCommandApplied` is called anyway because that is what the controller's
	 * submit path does with every outcome, and driving the oracle differently from the
	 * controller's pair would be the one thing that could make this comparison meaningless.
	 */
	static void SpendOneWait(FStratSelectionMachine&  Machine,
	                         FIntPoint                UnitHex,
	                         const FStratViewModel&   Model,
	                         const IStratRulesQuery&  Query)
	{
		Machine.HandleEvent(EStratSelectionEvent::HexPrimary, UnitHex, Model, Query);
		const FStratSelectionOutcome Waited =
			Machine.HandleEvent(EStratSelectionEvent::Wait, UnitHex, Model, Query);
		Machine.NotifyCommandApplied(Waited);
	}

	static FString Describe(const TArray<int32>& Ids)
	{
		TArray<FString> Parts;
		for (const int32 Id : Ids)
		{
			Parts.Add(FString::FromInt(Id));
		}
		return Parts.Num() == 0 ? FString(TEXT("<none>")) : FString::Join(Parts, TEXT(", "));
	}
}

// ---------------------------------------------------------------------------------------------
// T-SAVE-04 -- a load leaves the controller's presentation pair answering as a RESET pair does.
//
// THE PROPERTY, IN ONE SENTENCE. `UStratMatchSubsystem::LoadMatchFromSlot` replaces the match;
// `FStratSelectionMachine::DoneUnits` and `FStratBuildAffordance`'s focus describe the match it
// replaced, and a bit about a match that no longer exists must not reach the new one's screen.
//
// THREE LEGS, AND THE THIRD IS THE ONE A PLAYER WOULD REPORT.
//   (1) THE DRAWN MODEL. `bDone` is published by `FStratSelectionMachine::DecorateViewModel`
//       onto `FStratUnitView`, which is drawn AND gates input. Every unit view in the model
//       decorated after the load is compared against the oracle's answer for that same id.
//   (2) THE BUILD AFFORDANCE. `StratBuildAffordance.h` names the discharging condition for its
//       own `Reset()` as "a load or reseed path that calls BOTH", so this leg is not a second
//       clause -- it is half of the same one, and a fix that resets only the machine must not
//       turn this green. ONE WORD OF THAT SENTENCE IS RETRACTED IN PLACE. It read:
//       RETRACTED> "names the discharging condition for its own uncalled `Reset()`"
//       True over `283d711` and false from the T-SAVE-04 fix onward -- the file block above
//       names the caller and the commit. THE POINT SURVIVES THE CORRECTION UNCHANGED, which is
//       why the sentence is corrected rather than dropped: what makes this leg half of one
//       clause is that BOTH resets are named in a SINGLE condition, and the fix discharged it
//       by calling both in one place -- which is the thing this leg was here to force.
//   (3) THE SELECTION REFUSAL. `HandleEvent`'s selection gates refuse a unit in `DoneUnits`
//       with "unit N has finished this turn". A done bit carried across a load therefore makes
//       a unit of the freshly loaded match UNSELECTABLE, on a turn in which it has neither
//       moved nor acted. Both machines are handed the same event over the same loaded model,
//       and the ORACLE's outcome is the expectation -- including its `FailureReason`, so
//       neither the sentence nor its absence is written down here.
//
// THE CONTROL FOR ALL THREE IS THE PRE-LOAD READING, taken and required before the save is
// loaded, so "the bit is present afterwards" is not being read off a fixture that never
// managed to set one.
//
// AND THE SAVE IS TAKEN BEFORE THE WAIT, DELIBERATELY. The slot therefore holds the OPENING, in
// which the unit has not moved and has not acted -- so a `bDone` on it after the load is
// unambiguously about a match that is gone, rather than a plausible reading of the restored
// one.
// ---------------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratLoadClearsControllerSidePresentationStateTest,
	"Stratocracy.StratPlay.T-SAVE-04.LoadClearsControllerSidePresentationState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratLoadClearsControllerSidePresentationStateTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratLoadPresentationCarryOver;

	// The board's own sentence about a fixture carrying no Blueprint defaults.
	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FSlotScope       Slot;
	FTestWorldScope  Scope;
	if (Scope.World == nullptr)
	{
		AddError(TEXT("fixture: no transient world was created"));
		return false;
	}

	AStratPlayerController* const Controller = Scope.World->SpawnActor<AStratPlayerController>();
	if (Controller == nullptr)
	{
		AddError(TEXT("fixture: AStratPlayerController did not spawn"));
		return false;
	}

	UStratMatchSubsystem* const Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (Subsystem == nullptr)
	{
		AddError(TEXT("fixture: the world has no match subsystem"));
		return false;
	}

	FStratMatchConfig Config;
	FString           Error;
	if (!MakeConfig(Config, Error))
	{
		AddError(FString::Printf(TEXT("fixture: %s"), *Error));
		return false;
	}

	// The return is false in a fixture with no tile meshes; `IsMatchLive()` is the question.
	Subsystem->StartMatch(Config, Error);
	if (!Subsystem->IsMatchLive() || Subsystem->GetBridge() == nullptr)
	{
		AddError(FString::Printf(TEXT("fixture: no live match: %s"), *Error));
		return false;
	}

	// ---- the opening goes into the slot, before anything is done to the screen --------------
	if (!Subsystem->SaveMatchToSlot(FString(kTestSlotName), Error))
	{
		AddError(FString::Printf(TEXT("fixture: the opening did not save: %s"), *Error));
		return false;
	}

	// ---- the pre-load model, decorated by the controller exactly as a refresh decorates it ---
	FStratViewModel Before;
	if (!Subsystem->BuildViewModel(Before, Error))
	{
		AddError(FString::Printf(TEXT("fixture: the view model did not build: %s"), *Error));
		return false;
	}
	Controller->DecorateForPresentation(Before);

	const FStratUnitView* const Movable = FindMovableUnit(Before);
	if (Movable == nullptr)
	{
		AddError(TEXT("fixture: the opening offers no unit of the side to move that has "
		              "neither moved nor acted and is neither locked nor done"));
		return false;
	}
	const int32     WaitedUnitId = Movable->UnitId;
	const FIntPoint WaitedHex    = Movable->Hex;

	FIntPoint HeldFactoryHex = FIntPoint::ZeroValue;
	if (!FindHeldFactory(Before, HeldFactoryHex))
	{
		AddError(TEXT("fixture: the opening gives the viewing seat no factory, so the build "
		              "affordance leg has nothing to latch"));
		return false;
	}

	// ---- put presentation state on the controller, through its own public paths -------------
	//
	// THE ORACLE IS DRIVEN IN LOCKSTEP with the controller's pair, over the same model and the
	// same query, so that after its `Reset()` the two differ only by the load.
	FStratBridgeRulesQuery Query(Subsystem->GetBridge());

	FStratSelectionMachine OracleMachine;
	FStratBuildAffordance  OracleAffordance;

	SpendOneWait(Controller->GetSelectionMachine(), WaitedHex, Before, Query);
	SpendOneWait(OracleMachine,                     WaitedHex, Before, Query);

	Controller->GetBuildAffordance().Observe(Before);
	Controller->GetBuildAffordance().NoteHexPrimary(HeldFactoryHex, Before);
	OracleAffordance.Observe(Before);
	OracleAffordance.NoteHexPrimary(HeldFactoryHex, Before);

	// ---- the control: the state is really there, and it really reaches the model ------------
	if (!Controller->GetSelectionMachine().IsDone(WaitedUnitId))
	{
		AddError(FString::Printf(
			TEXT("fixture: the wait did not mark unit %d done, so there is no carried state to "
			     "measure"), WaitedUnitId));
		return false;
	}
	if (!Controller->GetBuildAffordance().HasFocus())
	{
		AddError(TEXT("fixture: the click on a held factory did not latch a focus, so there is "
		              "no carried focus to measure"));
		return false;
	}
	{
		FStratViewModel BeforeDrawn;
		if (!Subsystem->BuildViewModel(BeforeDrawn, Error))
		{
			AddError(FString::Printf(TEXT("fixture: the pre-load model did not rebuild: %s"),
				*Error));
			return false;
		}
		Controller->DecorateForPresentation(BeforeDrawn);
		const FStratUnitView* const Drawn = FindUnitById(BeforeDrawn, WaitedUnitId);
		if (Drawn == nullptr || !Drawn->bDone)
		{
			AddError(FString::Printf(
				TEXT("fixture: unit %d is not drawn as done before the load, so leg (1) would "
				     "measure nothing"), WaitedUnitId));
			return false;
		}
	}

	// ---- THE LOAD ---------------------------------------------------------------------------
	//
	// The return is false for the tile-mesh gap again, exactly as `StartMatch`'s was; the
	// subsystem's own liveness is the question. And the oracle is reset HERE, at the same
	// point, because `Reset()` is what a load is supposed to do to this pair.
	Subsystem->LoadMatchFromSlot(FString(kTestSlotName), Error);
	if (!Subsystem->IsMatchLive() || Subsystem->GetBridge() == nullptr)
	{
		AddError(FString::Printf(TEXT("fixture: the match is not live after the load: %s"),
			*Error));
		return false;
	}

	OracleMachine.Reset();
	OracleAffordance.Reset();

	FStratViewModel After;
	if (!Subsystem->BuildViewModel(After, Error))
	{
		AddError(FString::Printf(TEXT("fixture: the loaded view model did not build: %s"),
			*Error));
		return false;
	}
	Controller->DecorateForPresentation(After);

	// ---- LEG (1): the drawn model ------------------------------------------------------------
	TArray<int32> DisagreeingUnits;
	for (const FStratUnitView& U : After.Units)
	{
		if (U.bDone != OracleMachine.IsDone(U.UnitId))
		{
			DisagreeingUnits.Add(U.UnitId);
		}
	}
	TestTrue(*FString::Printf(
			TEXT("T-SAVE-04: every unit of the LOADED match is drawn with the bDone a reset "
			     "selection machine gives it. Units where the decorated model and the reset "
			     "oracle disagree: %s. A bDone carried across LoadMatchFromSlot is a unit of "
			     "the new match greyed out for something it did in the old one -- and unit ids "
			     "collide across matches, so the bit need not even land on the same unit."),
			*Describe(DisagreeingUnits)),
		DisagreeingUnits.Num() == 0);

	// ---- LEG (2): the build affordance -------------------------------------------------------
	TestEqual(
		TEXT("T-SAVE-04: the build affordance's focus after the load is a reset affordance's "
		     "focus. StratBuildAffordance.h names the discharging condition for its own Reset() "
		     "as a load path that calls BOTH, so a focus surviving here is a BUILD button about "
		     "a factory on a board that is gone"),
		Controller->GetBuildAffordance().HasFocus(), OracleAffordance.HasFocus());

	// ---- LEG (3): the refusal a player would report -------------------------------------------
	//
	// LAST, BECAUSE IT MUTATES BOTH MACHINES. The same event over the same loaded model is
	// handed to the controller's machine and to the oracle, and the oracle's outcome is the
	// whole of the expectation -- the id it selects and the sentence it does or does not
	// produce.
	const FStratUnitView* const Reloaded = FindUnitById(After, WaitedUnitId);
	if (Reloaded == nullptr)
	{
		AddError(FString::Printf(
			TEXT("fixture: unit %d is absent from the loaded match, so leg (3) has nothing to "
			     "click"), WaitedUnitId));
		return false;
	}

	FStratBridgeRulesQuery LoadedQuery(Subsystem->GetBridge());
	const FStratSelectionOutcome OracleClick = OracleMachine.HandleEvent(
		EStratSelectionEvent::HexPrimary, Reloaded->Hex, After, LoadedQuery);
	const FStratSelectionOutcome LiveClick = Controller->GetSelectionMachine().HandleEvent(
		EStratSelectionEvent::HexPrimary, Reloaded->Hex, After, LoadedQuery);

	TestEqual(
		*FString::Printf(
			TEXT("T-SAVE-04: clicking unit %d in the LOADED match selects whatever a reset "
			     "machine selects. It has neither moved nor acted on this turn, and "
			     "HandleEvent's done-set gate is what would refuse it"),
			WaitedUnitId),
		LiveClick.SelectedUnitId, OracleClick.SelectedUnitId);

	TestEqual(
		*FString::Printf(
			TEXT("T-SAVE-04: and it produces the refusal sentence a reset machine produces for "
			     "that same click -- neither the sentence nor its absence is written down in "
			     "this file (live: '%s')"),
			*LiveClick.FailureReason),
		LiveClick.FailureReason, OracleClick.FailureReason);

	return true;
}
