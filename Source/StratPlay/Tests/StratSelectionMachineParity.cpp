// The editor pass for phase 4's selection state machine -- GDD §2.11.1, §2.5, §4.9.
//
//   T-INT-05  the screen is rebuildable FROM THE VIEW MODEL ALONE, and the presentation
//             bits `bDone` / `bLockedThisTurn` are written onto the MODEL rather than held
//             in an actor. `StratMatchReconcile.cpp` pinned the negative -- no actor holds
//             them. This file pins the positive: `DecorateViewModel` is their producer,
//             and it produces them from the machine's own state and from nothing else.
//   T-UI-02   the movement decision is membership in `FStratBridge::ReachableHexes`'s set
//             and never a distance filter (§2.5).
//   T-TURN-01 a refused command leaves the machine consistent, because the machine mirrors
//             no rules state that could go stale.
//
// NO `strat::` TYPE IS NAMED ANYWHERE BELOW, AND NO `strat::` FREE FUNCTION IS CALLED.
// That is `StratPlay.Build.cs`'s stated property of this module -- "it holds NO rules, NO
// arithmetic over a rules answer, and NOT ONE `strat::` type" -- and a test file is part of
// the module. Every rules answer arrives through the engine-typed façade
// (`ReachableHexes`, `SubmitMoveToHex`, `Turn`, `StateHash`) or through
// `FStratBridgeRulesQuery`, in `int32` and `FIntPoint`. A clause that needed a `strat::`
// value lives in `StratBridge/Tests/StratSelectionFacadeParity.cpp` instead, and that file
// says which.
//
// WHERE EVERY EXPECTATION COMES FROM, which is the only question that matters here.
//   - `SelectionMachineWritesDoneOntoModel` compares a DECORATED model against an
//     UNDECORATED one built from the same bridge, through the reflection system's own
//     `CompareScriptStruct`. The expectation for "nothing else moved" is therefore the
//     model the module itself built, field for field, including fields this file never
//     names -- a field added to `FStratUnitView` tomorrow is covered the day it lands.
//   - `SelectionMachineUsesTheQueryNotDistance` takes the reachable set from the query and
//     asserts a DIFFERENCE from a self-computed distance disc. The disc is a FOIL and not
//     an expectation, exactly as `T-UI-02.NotHexDistance` uses one: no clause asserts the
//     real set equals anything computed here, and a wrong foil makes the clause harder to
//     pass rather than easier.
//   - `RefusedCommandLeavesMachineConsistent` compares the machine's state against ITS OWN
//     earlier state, and the refusal against `FStratBridge::StateHash()`.
//
// THE TWO PLACES A MODEL IS EDITED BY HAND are declared where they happen. Editing a
// built model to set `bHasMoved` / `bHasActed`, or to name a locked unit, is FIXTURE
// CONSTRUCTION -- the same standing `StratMatchReconcile.cpp` records for the models it
// edits -- and in both cases the edited value is the INPUT and the machine's output is
// what is asserted. Neither clause computes the thing it then checks.
//
// ---------------------------------------------------------------------------
// THE RETRACTION MARKER CONVENTION, DECLARED SO A LATER SWEEP CAN RELY ON IT.
//
// This file's header block and one clause below both used to ASSERT that nothing in a
// running build ever called `FStratSelectionMachine::SetLockedThisTurn`. Wave B2 made that
// false and the sentences are withdrawn in place rather than deleted, because a reader who
// remembers the claim needs to see it retracted and not silently absent.
//
// NOTE THAT THIS VERY PARAGRAPH IS WHY THE CONVENTION BELOW IS PER-LINE AND MECHANICAL: it
// is prose ABOUT the retracted claim, and it deliberately does not reproduce the claim's
// wording, precisely so that a sweep grepping for that wording does not have to decide
// whether a sentence is an assertion or a description of one.
//
// THE PROBLEM THAT CREATES, and the reason a convention is needed at all: six header
// retractions elsewhere in this tree quote their own old wording, so a `grep` for a stale
// phrase now returns mostly CORRECT withdrawals with the occasional live assertion hiding
// among them. That is exactly how the live assertion in this file survived the header
// sweep.
//
// THE CONVENTION, and it is per-LINE and not per-paragraph, because a paragraph marker
// leaves a sweep guessing where the paragraph ends:
//
//   Every comment line that QUOTES a claim this file no longer makes begins, after its
//   leading whitespace and the `//`, with the exact token `RETRACTED> `. No line carrying
//   that token asserts anything, and nothing else in this file uses the token.
//
// So a sweep subtracts the withdrawn quotations mechanically:
//
//   grep -n '<stale phrase>' <file> | grep -v 'RETRACTED> '
//
// Anything that survives that filter is a live assertion and is the sweep's business.
// A quotation that must appear inside EXECUTABLE code -- an `AddInfo` string, say -- is not
// covered by this convention and must simply be deleted or rewritten, which is what
// happened to the one that existed below.
// ---------------------------------------------------------------------------
//
// ON `SetLockedThisTurn` -- WITHDRAWN 2026-08-20, WAVE B2. This block used to read, in this
// file's own voice:
//
// RETRACTED> "ON `SetLockedThisTurn`, STATED IN ADVANCE BECAUSE IT WOULD OTHERWISE BE A
// RETRACTED>  HALF-PINNED CLAUSE. §2.11.6's guided opening is out of this milestone, so
// RETRACTED>  NOTHING IN A SHIPPING PATH CALLS THAT WRITER: `LockedUnits` is empty in every
// RETRACTED>  running game and `bLockedThisTurn` is false on every unit, exactly as it was
// RETRACTED>  before this phase. ... No clause in this tree can observe a lock arising,
// RETRACTED>  because nothing produces one. When §2.11.6 lands, its producer needs a clause
// RETRACTED>  of its own; this one will not cover it."
//
// §2.11.6 HAS LANDED AND THE WRITER HAS A SHIPPING CALLER. `FStratGuidedOpening::PublishLocks`
// (`Source/StratPlay/StratGuidedOpening.cpp`) calls `FStratSelectionMachine::SetLockedThisTurn`
// once per friendly unit on every `Observe`, and `AStratPlayerController::RefreshFromMachine`
// and `::HandleSelectionEvent` both call `Observe` on the live path. So `LockedUnits` is
// non-empty in a running game while beat 1a is outstanding, and `bLockedThisTurn` is true on
// screen for the first time.
//
// WHAT THAT PARAGRAPH GOT RIGHT AND KEEPS. `LockedUnitIsWrittenAndNotSelectable` below still
// drives the writer itself and still pins a property of `FStratSelectionMachine` rather than
// of the game as played -- that is not a defect in it, it is its subject. What it could not
// do was witness a lock ARISING, and that sentence's own prediction has been honoured:
// `LockArisesFromTheGuidanceLayer` below is the producer's clause, and it drives
// `FStratGuidedOpening` and never touches `SetLockedThisTurn`.

#include "Misc/AutomationTest.h"

#include "Containers/Set.h"
#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Math/IntPoint.h"
#include "Misc/Paths.h"
#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"

#include "StratGuidedOpening.h"
#include "StratSelectionMachine.h"
#include "StratViewModel.h"

// Included from a .cpp and never from a UHT-parsed header -- the module-wide discipline.
#include "StratBridge.h"

namespace StratSelectionMachineParity
{
	// The same harness pin every parity file in this tree carries. Replay.h states that no
	// rule decides which side moves first, so this is a harness constant and not a rule.
	static const int32 kFirstSide = 0;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	static FString ScenarioPath()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"), TEXT("ferrum_crossing.json"));
	}

	static FString Describe(FIntPoint Hex)
	{
		return FString::Printf(TEXT("axial(%d,%d)"), Hex.X, Hex.Y);
	}

	/** The seeded opening, through the two engine-typed loaders and nothing else. */
	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		UDataTable* Units = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		UDataTable* Terrain = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
		if (Units == nullptr || Terrain == nullptr)
		{
			OutError = TEXT("DT_Units and/or DT_Terrain did not load from /Game/StratData");
			return false;
		}

		const FStratResult Loaded = Bridge.LoadDefinitions(Units, Terrain);
		if (!Loaded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadDefinitions refused: %s"), *Loaded.Reason);
			return false;
		}

		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath(), kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	/**
	 * Seeds a bridge and builds the view model FOR THE SIDE TO MOVE.
	 *
	 * The viewing side is the active side here because these clauses are about a player
	 * acting on their own turn; `HandleEvent` reads the active side off
	 * `Model.Match.SideToMove` and never off `ViewingSide`, and the hot-seat file exercises
	 * the case where the two differ.
	 */
	static bool SeedAndBuild(FStratBridge& Bridge, FStratViewModel& OutModel, FString& OutError)
	{
		if (!SeedBridge(Bridge, OutError))
		{
			return false;
		}
		if (!StratBuildViewModel(Bridge, Bridge.SideToMove(), OutModel, OutError))
		{
			return false;
		}
		return true;
	}

	static const FStratUnitView* FindUnit(const FStratViewModel& Model, int32 UnitId)
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

	static const FStratUnitView* FindUnitAtHex(const FStratViewModel& Model, FIntPoint Hex)
	{
		for (const FStratUnitView& U : Model.Units)
		{
			if (U.Hex == Hex)
			{
				return &U;
			}
		}
		return nullptr;
	}

	/** The first unit belonging to the side whose turn it is, or null. */
	static const FStratUnitView* FirstActiveUnit(const FStratViewModel& Model)
	{
		for (const FStratUnitView& U : Model.Units)
		{
			if (U.Side == Model.Match.SideToMove)
			{
				return &U;
			}
		}
		return nullptr;
	}

	/**
	 * Axial hex distance. THE FOIL'S ARITHMETIC AND NOTHING ELSE'S.
	 *
	 * It appears in this file for one clause, `SelectionMachineUsesTheQueryNotDistance`,
	 * where it builds the naive set the real query must be shown to DIFFER from. No clause
	 * uses it to decide whether a hex is reachable, and none may: that is the exact
	 * substitution T-UI-02 exists to catch, and 122 divergent hexes across 10 of 10 units
	 * were measured on this board in phase 1.
	 */
	static int32 FoilHexDistance(FIntPoint A, FIntPoint B)
	{
		const int32 dq = A.X - B.X;
		const int32 dr = A.Y - B.Y;
		return (FMath::Abs(dq) + FMath::Abs(dq + dr) + FMath::Abs(dr)) / 2;
	}

	/** The done-set as the machine reports it, over every unit the model carries. */
	static TSet<int32> DoneSetOf(const FStratSelectionMachine& Machine, const FStratViewModel& Model)
	{
		TSet<int32> Done;
		for (const FStratUnitView& U : Model.Units)
		{
			if (Machine.IsDone(U.UnitId))
			{
				Done.Add(U.UnitId);
			}
		}
		return Done;
	}
}

// ---------------------------------------------------------------------------
// T-INT-05 -- a Wait writes `bDone` onto the model, on that unit and on no other, and
// changes NOTHING else about the model.
//
// THE DEBT THIS CLOSES IS QUOTED ON THE FIELD ITSELF. `StratViewModel.h` on `bDone`:
// "when phase 3's selection machine lands, it writes these two bits onto the built model
// and does NOT keep its own copy in an actor -- the moment it does, T-INT-05 is false and
// no compiler says so." Until this phase nothing produced the bit at all, so
// `NoActorHoldsPresentationBits` could only pin that no actor held it. This is the
// positive obligation.
//
// WHY THE COMPARISON IS `CompareScriptStruct` AND NOT A LIST OF FIELDS. The claim is that
// the decorator "writes only those two fields and touches nothing else on the model", and
// a clause that enumerated the fields it checked would be silent about the field somebody
// adds next year -- which is precisely when a decorator quietly grows a third write. The
// reflection system already knows every field, so the clause clears the ONE bit it expects
// to differ and then demands byte-for-byte equality of the whole `FStratViewModel` against
// the undecorated build. The expectation side of that comparison is a model the module
// built; this file constructs no model.
//
// THE WAIT IS THE RIGHT EVENT FOR THIS. §2.11.1's wait spends a unit's turn and reaches no
// rules module -- there is no `Wait` in `strat::SaveCommandKind` -- so `bDone` becomes true
// with the state hash unmoved, which is the sharpest possible demonstration that this bit
// is the machine's and not a projection of anything the rules module holds.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSelectionWritesDoneOntoModelTest,
	"Stratocracy.StratPlay.T-INT-05.SelectionMachineWritesDoneOntoModel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSelectionWritesDoneOntoModelTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSelectionMachineParity;

	FStratBridge Bridge;
	FStratViewModel Undecorated;
	FString Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndBuild(Bridge, Undecorated, Error)))
	{
		AddError(Error);
		return false;
	}

	const FStratUnitView* const Unit = FirstActiveUnit(Undecorated);
	if (!TestNotNull(TEXT("the side to move has a unit on the board"), Unit))
	{
		return false;
	}
	const int32     WaitedId  = Unit->UnitId;
	const FIntPoint WaitedHex = Unit->Hex;

	// Every unit starts NOT done, so the flip below is a change and not a coincidence.
	for (const FStratUnitView& U : Undecorated.Units)
	{
		TestFalse(*FString::Printf(TEXT("unit %d is not done on a freshly built model"), U.UnitId),
			U.bDone);
	}

	FStratSelectionMachine   Machine;
	FStratBridgeRulesQuery   Query(&Bridge);

	const FStratSelectionOutcome Selected =
		Machine.HandleEvent(EStratSelectionEvent::HexPrimary, WaitedHex, Undecorated, Query);
	TestEqual(TEXT("clicking a friendly unit selects it and submits nothing"),
		static_cast<int32>(Selected.Command), static_cast<int32>(EStratSelectionCommand::None));
	if (!TestEqual(TEXT("the clicked unit is the selected one"),
			Machine.GetSelectedUnitId(), WaitedId))
	{
		AddError(Selected.FailureReason);
		return false;
	}

	const FString HashBefore = Bridge.StateHash();

	const FStratSelectionOutcome Waited =
		Machine.HandleEvent(EStratSelectionEvent::Wait, FIntPoint::ZeroValue, Undecorated, Query);
	TestEqual(TEXT("a Wait produces no command -- the save format cannot carry one"),
		static_cast<int32>(Waited.Command), static_cast<int32>(EStratSelectionCommand::None));
	TestEqual(TEXT("a Wait deselects"), Machine.GetSelectedUnitId(), static_cast<int32>(INDEX_NONE));
	TestTrue(TEXT("§2.11.1: the waited unit is DONE"), Machine.IsDone(WaitedId));
	TestEqual(TEXT("and the rules state did not move -- a Wait reaches no rules module"),
		Bridge.StateHash(), HashBefore);

	// ---- the decoration -----------------------------------------------------
	FStratViewModel Decorated = Undecorated;
	Machine.DecorateViewModel(Decorated);

	if (!TestEqual(TEXT("decoration adds and removes no units"),
			Decorated.Units.Num(), Undecorated.Units.Num()))
	{
		return false;
	}

	int32 DoneCount = 0;
	for (const FStratUnitView& U : Decorated.Units)
	{
		const bool bShouldBeDone = (U.UnitId == WaitedId);
		TestEqual(
			*FString::Printf(TEXT("T-INT-05: unit %d's bDone is %s after a Wait on unit %d"),
				U.UnitId, bShouldBeDone ? TEXT("set") : TEXT("clear"), WaitedId),
			U.bDone, bShouldBeDone);
		DoneCount += U.bDone ? 1 : 0;

		// Nothing locked anything, and nothing in a shipping path can. See this file's
		// header block; the lock clause below is the one that drives it deliberately.
		TestFalse(*FString::Printf(TEXT("unit %d is not locked -- nothing wrote a lock"), U.UnitId),
			U.bLockedThisTurn);
	}
	TestEqual(TEXT("T-INT-05: exactly one unit is DONE"), DoneCount, 1);

	// ---- and NOTHING else moved ---------------------------------------------
	// The one expected difference is cleared, and then the WHOLE struct is compared
	// through the reflection system -- every hex, every factory, both sides' standings,
	// the match view, the viewing side, and every other field of every unit.
	FStratViewModel Restored = Decorated;
	for (FStratUnitView& U : Restored.Units)
	{
		if (U.UnitId == WaitedId)
		{
			U.bDone = false;
		}
	}

	const UScriptStruct* const ModelStruct = FStratViewModel::StaticStruct();
	TestTrue(
		TEXT("T-INT-05: with bDone put back, the decorated model is IDENTICAL to the undecorated "
		     "one -- the decorator writes the presentation block and touches nothing else"),
		ModelStruct->CompareScriptStruct(&Restored, &Undecorated, 0));

	// The negative control for the comparison itself. If `CompareScriptStruct` reported
	// equality for two models that genuinely differ, the clause above would be vacuous.
	TestFalse(
		TEXT("the comparison can see a difference -- the decorated model is NOT identical to the "
		     "undecorated one"),
		ModelStruct->CompareScriptStruct(&Decorated, &Undecorated, 0));

	return true;
}

// ---------------------------------------------------------------------------
// T-INT-05 -- `bDone` is NOT `bHasMoved && bHasActed`.
//
// `Ui.h` rules against the one-line derivation and names the cases that break it: Wait and
// RMB-in-MOVED both reach DONE without spending the act flag. `StratViewModel.h` repeats
// it on the field -- "anything that sets this from `bHasMoved` / `bHasActed` is wrong" --
// and `StratSelectionMachine.h` repeats it again. Three headers stating a rule is not a
// gate; this is the gate.
//
// THE DERIVATION IS ONE LINE AWAY AND IT AGREES MOST OF THE TIME, which is what makes it
// dangerous rather than merely wrong: a unit that has moved and attacked usually IS done,
// so a decorator that computed the bit would look correct all the way through a playtest
// and would then grey out a unit that had merely spent both flags to a Capture, or fail to
// grey out one that waited.
//
// THE TURN FLAGS ARE SET BY EDITING THE BUILT MODEL, and that is declared. They are the
// INPUT to this clause -- the provenance of the two bits is irrelevant to what is being
// asserted, which is that the DECORATOR does not read them -- and reaching a real
// both-flags position would need a mid-match state this module cannot construct without
// naming a `strat::` type to replay a log with. The output side, `bDone`, is entirely the
// machine's and is never written by this file.
//
// THE POSITIVE CONTROL IS PART OF THE CLAUSE. A decorator that wrote `false` onto every
// unit unconditionally would satisfy the negative half, so the same model carries a second
// unit that DID wait, and that one must come back true.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSelectionDoneIsNotDerivedTest,
	"Stratocracy.StratPlay.T-INT-05.DoneIsNotDerivedFromTurnFlags",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSelectionDoneIsNotDerivedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSelectionMachineParity;

	FStratBridge Bridge;
	FStratViewModel Model;
	FString Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"), SeedAndBuild(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	// Two units of the side to move: one will be given both turn flags and never acted on,
	// the other will actually wait.
	int32 SpentId  = INDEX_NONE;
	int32 WaiterId = INDEX_NONE;
	for (const FStratUnitView& U : Model.Units)
	{
		if (U.Side != Model.Match.SideToMove)
		{
			continue;
		}
		if (SpentId == INDEX_NONE)
		{
			SpentId = U.UnitId;
		}
		else if (WaiterId == INDEX_NONE)
		{
			WaiterId = U.UnitId;
			break;
		}
	}
	if (!TestTrue(TEXT("the side to move deploys at least two units"),
			SpentId != INDEX_NONE && WaiterId != INDEX_NONE))
	{
		return false;
	}

	// FIXTURE CONSTRUCTION, declared. These two bits are this clause's input.
	FIntPoint WaiterHex = FIntPoint::ZeroValue;
	for (FStratUnitView& U : Model.Units)
	{
		if (U.UnitId == SpentId)
		{
			U.bHasMoved = true;
			U.bHasActed = true;
		}
		if (U.UnitId == WaiterId)
		{
			WaiterHex = U.Hex;
		}
	}

	FStratSelectionMachine Machine;
	FStratBridgeRulesQuery Query(&Bridge);

	// The waiter, through the machine's own two events.
	Machine.HandleEvent(EStratSelectionEvent::HexPrimary, WaiterHex, Model, Query);
	if (!TestEqual(TEXT("the second unit selects"), Machine.GetSelectedUnitId(), WaiterId))
	{
		return false;
	}
	Machine.HandleEvent(EStratSelectionEvent::Wait, FIntPoint::ZeroValue, Model, Query);

	// The machine was never told anything about the spent unit.
	TestFalse(
		TEXT("T-INT-05: a unit with bHasMoved AND bHasActed, never waited on and never attacked "
		     "with, is NOT done -- bDone is not the turn flags (Ui.h)"),
		Machine.IsDone(SpentId));
	TestTrue(TEXT("the unit that actually waited IS done, so the clause above is not 'nothing is done'"),
		Machine.IsDone(WaiterId));

	Machine.DecorateViewModel(Model);

	for (const FStratUnitView& U : Model.Units)
	{
		if (U.UnitId == SpentId)
		{
			TestTrue(TEXT("the spent unit still carries both turn flags -- the fixture held"),
				U.bHasMoved && U.bHasActed);
			TestFalse(
				TEXT("T-INT-05: and the decorated model reports bDone FALSE for it"),
				U.bDone);
		}
		else if (U.UnitId == WaiterId)
		{
			TestTrue(TEXT("T-INT-05: the waiter's bDone is TRUE, with neither turn flag spent"),
				U.bDone);
			TestFalse(TEXT("the waiter never moved"), U.bHasMoved);
			TestFalse(TEXT("and never acted -- so bDone here is unreachable from the flags"),
				U.bHasActed);
		}
		else
		{
			TestFalse(*FString::Printf(TEXT("unit %d, untouched, is not done"), U.UnitId), U.bDone);
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-INT-05 -- the §2.11.6 lock: written onto the model, enforced in selection, and NOT
// cleared by a turn boundary.
//
// READ THE HEADER BLOCK BEFORE READING THIS CLAUSE, and read its retraction convention
// first. This paragraph used to read:
//
// RETRACTED> "Nothing in a shipping path calls `SetLockedThisTurn`; §2.11.6's guided
// RETRACTED>  opening is out of this milestone. ... in every running build today
// RETRACTED>  `LockedUnits` is empty and `bLockedThisTurn` is false everywhere, identical
// RETRACTED>  to before this phase. ... What no clause in this tree can do is witness a
// RETRACTED>  lock ARISING, and that is the honest limit of this one."
//
// BOTH HALVES ARE WITHDRAWN. `FStratGuidedOpening::PublishLocks` is the shipping caller, so
// `LockedUnits` is non-empty in a running game while beat 1a is outstanding; and
// `LockArisesFromTheGuidanceLayer` below witnesses a lock arising from that producer.
//
// THIS CLAUSE'S SUBJECT IS UNCHANGED BY THAT. It still drives the writer itself, and what
// it pins is still a property of `FStratSelectionMachine` and not of the game as played --
// deliberately, for the reason the machine's header gives for the writer existing at all: a
// guidance layer that had to add the enforcement later would be adding a rule to a state
// machine it does not own, and it would find the rule already there, with a gate saying
// what the rule is. The two clauses are complements, not duplicates: this one pins the
// enforcement in isolation from any beat, and its neighbour pins the beat's production of
// the lock without touching the setter.
//
// THE THIRD ASSERTION IS THE ONE WORTH THE MOST. `StratViewModel.h` states that the lock's
// lifecycle is NOT `bDone`'s -- it clears when beat 1a retires, INSIDE turn 1, and not at
// the turn boundary -- and `NotifyCommandApplied` therefore clears `DoneUnits` on an
// EndTurn and deliberately does not clear `LockedUnits`. Two sets cleared by the same line
// is the mistake that reads as tidy, and it would make §2.11.6's opening un-implementable
// without the guidance layer noticing.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSelectionLockedUnitTest,
	"Stratocracy.StratPlay.T-INT-05.LockedUnitIsWrittenAndNotSelectable",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSelectionLockedUnitTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSelectionMachineParity;

	FStratBridge Bridge;
	FStratViewModel Model;
	FString Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"), SeedAndBuild(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	int32     LockedId  = INDEX_NONE;
	FIntPoint LockedHex = FIntPoint::ZeroValue;
	int32     OtherId   = INDEX_NONE;
	FIntPoint OtherHex  = FIntPoint::ZeroValue;
	for (const FStratUnitView& U : Model.Units)
	{
		if (U.Side != Model.Match.SideToMove)
		{
			continue;
		}
		if (LockedId == INDEX_NONE)
		{
			LockedId  = U.UnitId;
			LockedHex = U.Hex;
		}
		else if (OtherId == INDEX_NONE)
		{
			OtherId  = U.UnitId;
			OtherHex = U.Hex;
			break;
		}
	}
	if (!TestTrue(TEXT("the side to move deploys at least two units"),
			LockedId != INDEX_NONE && OtherId != INDEX_NONE))
	{
		return false;
	}

	FStratSelectionMachine Machine;
	FStratBridgeRulesQuery Query(&Bridge);

	// THE `AddInfo` THAT USED TO STAND HERE IS DELETED RATHER THAN MARKED, and its text is
	// quoted under the marker so the deletion is still legible as a withdrawal:
	//
	// RETRACTED> AddInfo(TEXT("NOTE: SetLockedThisTurn has no caller in any shipping path --
	// RETRACTED>              §2.11.6 is out of this milestone -- so this clause drives it
	// RETRACTED>              directly and pins the machine's behaviour, not the game's."));
	//
	// It printed that into the automation log on every run, and wave B2 made the first clause
	// of it false. An EXECUTABLE string is outside the reach of a comment convention -- there
	// is nowhere inside a `TEXT(...)` literal to withdraw a claim in place -- so the only
	// honest options were deletion or rewriting, and the quotation above is where the marker
	// convention picks the loss up. The replacement says what is true and names the clause
	// that covers the rest.
	AddInfo(TEXT("NOTE: this clause drives SetLockedThisTurn directly and pins the machine's "
	             "behaviour, not the game's. The shipping caller is "
	             "FStratGuidedOpening::PublishLocks, and T-INT-05.LockArisesFromTheGuidanceLayer "
	             "is the clause that witnesses a lock arising from it."));
	Machine.SetLockedThisTurn(LockedId, true);
	TestTrue(TEXT("the machine reports the unit locked"), Machine.IsLockedThisTurn(LockedId));

	// ---- written onto the model, on that unit and no other -------------------
	FStratViewModel Decorated = Model;
	Machine.DecorateViewModel(Decorated);
	for (const FStratUnitView& U : Decorated.Units)
	{
		TestEqual(
			*FString::Printf(TEXT("T-INT-05: unit %d's bLockedThisTurn"), U.UnitId),
			U.bLockedThisTurn, U.UnitId == LockedId);
		TestFalse(*FString::Printf(TEXT("a lock is not a DONE: unit %d"), U.UnitId), U.bDone);
	}

	// ---- and not selectable -------------------------------------------------
	const FStratSelectionOutcome Blocked =
		Machine.HandleEvent(EStratSelectionEvent::HexPrimary, LockedHex, Decorated, Query);
	TestEqual(TEXT("T-INT-05: a locked unit is not selected"),
		Machine.GetSelectedUnitId(), static_cast<int32>(INDEX_NONE));
	TestEqual(TEXT("and nothing is submitted on its behalf"),
		static_cast<int32>(Blocked.Command), static_cast<int32>(EStratSelectionCommand::None));
	TestFalse(TEXT("the refusal says why, so a surface can tell the player"),
		Blocked.FailureReason.IsEmpty());
	AddInfo(FString::Printf(TEXT("locked-selection reason: %s"), *Blocked.FailureReason));

	// An unlocked stablemate still selects, so the clause above is not "nothing selects".
	Machine.HandleEvent(EStratSelectionEvent::HexPrimary, OtherHex, Decorated, Query);
	TestEqual(TEXT("an unlocked unit on the same side still selects"),
		Machine.GetSelectedUnitId(), OtherId);
	Machine.HandleEvent(EStratSelectionEvent::Wait, FIntPoint::ZeroValue, Decorated, Query);
	TestTrue(TEXT("and can be waited to DONE"), Machine.IsDone(OtherId));

	// ---- the turn boundary clears one set and not the other ------------------
	FStratSelectionOutcome EndTurn;
	EndTurn.Command = EStratSelectionCommand::EndTurn;
	Machine.NotifyCommandApplied(EndTurn);

	TestFalse(TEXT("T-INT-05: an accepted EndTurn clears the DONE set -- it is a per-turn fact"),
		Machine.IsDone(OtherId));
	TestTrue(
		TEXT("T-INT-05: and does NOT clear the lock -- §2.11.6's lock retires inside turn 1, on a "
		     "different lifecycle (StratViewModel.h)"),
		Machine.IsLockedThisTurn(LockedId));

	// ---- and the writer takes it off again ----------------------------------
	Machine.SetLockedThisTurn(LockedId, false);
	TestFalse(TEXT("the lock clears when its owner clears it"), Machine.IsLockedThisTurn(LockedId));

	FStratViewModel Unlocked = Model;
	Machine.DecorateViewModel(Unlocked);
	for (const FStratUnitView& U : Unlocked.Units)
	{
		TestFalse(*FString::Printf(TEXT("unit %d is no longer locked on the model"), U.UnitId),
			U.bLockedThisTurn);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-02 -- the machine decides a move by MEMBERSHIP IN THE QUERY'S SET, not by distance.
//
// THIS IS THE PHASE-4 ANALOGUE OF `T-UI-02.NotHexDistance`, AND IT MATTERS FOR THE SAME
// REASON. That clause pins the BRIDGE's set against a distance filter. It is silent about
// the call site, and the call site is where the substitution actually happens: a selection
// machine that filtered clicks by `distance <= move` would submit moves the rules module
// then refuses, and would refuse clicks the rules module would have accepted -- with the
// bridge's own gate green throughout. StratSelectionMachine.h states there is no hex
// arithmetic in the machine at all; this clause is the measurement of that sentence.
//
// THE FOIL, AND WHY ITS RADIUS IS READ RATHER THAN TAKEN FROM `move`. The plausible wrong
// implementation is `hexDistance(origin, hex) <= move`. This module cannot see `move` --
// `FStratUnitView` carries no movement allowance, deliberately, and the §2.4 table is
// `StratBridge`'s -- so the radius here is the LARGEST DISTANCE THE REAL REACHABLE SET
// ITSELF CONTAINS. That radius is never greater than `move`, so this foil is a SUBSET of
// the classic one and every divergence it finds is a divergence the classic one would also
// have found. It is the conservative choice in the direction that makes the clause harder
// to pass, which is the only direction a self-computed value may lean.
//
// THE DIVERGENT HEX IS FOUND BY MEASUREMENT AND NEVER WRITTEN DOWN. Phase 1 measured 122
// divergent hexes across 10 of 10 units on Ferrum Crossing from three causes -- friendly
// occupancy, impassable Water at moveCost 0, and weighted Woods=2 / Mountains=3 -- so a
// real sample exists; hard-coding one would be a coordinate that has to be re-picked every
// time the board moves, which is how a clause gets quietly relaxed. The search below
// reports what it found, by terrain, and fails loudly if the board ever stops diverging.
//
// THE CANDIDATE MUST BE EMPTY GROUND. A click on an occupied hex takes the attack or
// re-select branch and would be testing a different decision; the move branch is the one
// this clause is about.
//
// THE LAST ASSERTION CLOSES THE LOOP. The machine's refusal is checked against the RULES
// MODULE's refusal of the same move, so this clause cannot pass by the machine being
// merely different from a distance filter -- it has to be different in the direction the
// rules module is.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSelectionUsesQueryNotDistanceTest,
	"Stratocracy.StratPlay.T-UI-02.SelectionMachineUsesTheQueryNotDistance",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSelectionUsesQueryNotDistanceTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSelectionMachineParity;

	FStratBridge Bridge;
	FStratViewModel Model;
	FString Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"), SeedAndBuild(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratBridgeRulesQuery Query(&Bridge);

	int32 UnitsThatDiverge     = 0;
	int32 UnitsConsidered      = 0;
	int32 TotalDivergentHexes  = 0;
	int32 DivergentAndOccupied = 0;
	int32 DivergentAndEmpty    = 0;

	// The chosen probe: a unit, and a hex inside the naive disc but outside the real set.
	int32     ProbeUnit = INDEX_NONE;
	FIntPoint ProbeUnitHex = FIntPoint::ZeroValue;
	FIntPoint ProbeHex  = FIntPoint::ZeroValue;
	FName     ProbeTerrain;

	for (const FStratUnitView& U : Model.Units)
	{
		if (U.Side != Model.Match.SideToMove)
		{
			continue;
		}
		++UnitsConsidered;

		TArray<FIntPoint> Reach;
		FString QueryReason;
		if (!Query.ReachableHexes(U.UnitId, Reach, QueryReason))
		{
			AddError(FString::Printf(TEXT("unit %d: ReachableHexes refused: %s"),
				U.UnitId, *QueryReason));
			return false;
		}
		if (!TestTrue(*FString::Printf(TEXT("unit %d's reachable set is non-empty (the null move)"),
				U.UnitId), Reach.Num() > 0))
		{
			continue;
		}

		// The foil's radius, read off the real set. See the header block.
		int32 Radius = 0;
		for (const FIntPoint& H : Reach)
		{
			Radius = FMath::Max(Radius, FoilHexDistance(U.Hex, H));
		}

		int32 DivergentForThisUnit = 0;
		for (const FStratHexView& Hex : Model.Hexes)
		{
			if (FoilHexDistance(U.Hex, Hex.Hex) > Radius)
			{
				continue;
			}
			if (Reach.Contains(Hex.Hex))
			{
				continue;
			}

			// In the naive disc, not in the real set: a hex a distance filter would offer
			// and the unit cannot enter.
			++DivergentForThisUnit;
			++TotalDivergentHexes;

			const FStratUnitView* const Occupant = FindUnitAtHex(Model, Hex.Hex);
			if (Occupant != nullptr)
			{
				++DivergentAndOccupied;
				continue;
			}
			++DivergentAndEmpty;

			if (ProbeUnit == INDEX_NONE && !U.bHasMoved)
			{
				ProbeUnit    = U.UnitId;
				ProbeUnitHex = U.Hex;
				ProbeHex     = Hex.Hex;
				ProbeTerrain = Hex.TerrainId;
			}

			if (DivergentForThisUnit <= 4)
			{
				AddInfo(FString::Printf(
					TEXT("unit %d (%s) at %s, foil radius %d: a distance filter would offer %s -- "
					     "terrain %s, occupant %d, distance %d"),
					U.UnitId, *U.DefId.ToString(), *Describe(U.Hex), Radius, *Describe(Hex.Hex),
					*Hex.TerrainId.ToString(),
					Occupant != nullptr ? Occupant->UnitId : INDEX_NONE,
					FoilHexDistance(U.Hex, Hex.Hex)));
			}
		}

		if (DivergentForThisUnit > 0)
		{
			++UnitsThatDiverge;
		}
	}

	AddInfo(FString::Printf(
		TEXT("T-UI-02 (phase 4 foil): %d of %d active-side units diverge; %d divergent hexes total "
		     "-- %d occupied, %d empty ground"),
		UnitsThatDiverge, UnitsConsidered, TotalDivergentHexes, DivergentAndOccupied,
		DivergentAndEmpty));

	if (!TestTrue(
			TEXT("T-UI-02: on Ferrum Crossing the reachable set differs from a distance disc of the "
			     "same radius -- if this fails, the board no longer distinguishes the routed query "
			     "from the substitution the clause forbids, and that is a scenario finding rather "
			     "than a licence to drop the clause"),
			TotalDivergentHexes > 0))
	{
		return false;
	}
	if (!TestTrue(
			TEXT("T-UI-02: at least one divergent hex is EMPTY GROUND, so the click can exercise the "
			     "MOVE branch rather than the attack or re-select branch"),
			ProbeUnit != INDEX_NONE))
	{
		return false;
	}

	AddInfo(FString::Printf(
		TEXT("probe: unit %d at %s clicking %s (terrain %s) -- inside the naive disc, outside "
		     "ReachableHexes"),
		ProbeUnit, *Describe(ProbeUnitHex), *Describe(ProbeHex), *ProbeTerrain.ToString()));

	// ---- the click ----------------------------------------------------------
	FStratSelectionMachine Machine;
	Machine.HandleEvent(EStratSelectionEvent::HexPrimary, ProbeUnitHex, Model, Query);
	if (!TestEqual(TEXT("the probe unit selects"), Machine.GetSelectedUnitId(), ProbeUnit))
	{
		return false;
	}

	const FStratSelectionOutcome Clicked =
		Machine.HandleEvent(EStratSelectionEvent::HexPrimary, ProbeHex, Model, Query);

	TestEqual(
		TEXT("T-UI-02: a click inside the naive disc but outside the module's reachable set yields "
		     "NO command"),
		static_cast<int32>(Clicked.Command), static_cast<int32>(EStratSelectionCommand::None));
	TestFalse(TEXT("T-UI-02: and the machine says why"), Clicked.FailureReason.IsEmpty());
	TestEqual(TEXT("a refused click does not drop the selection"),
		Machine.GetSelectedUnitId(), ProbeUnit);
	AddInfo(FString::Printf(TEXT("machine's refusal: %s"), *Clicked.FailureReason));

	// ---- and the rules module agrees ---------------------------------------
	// The machine being merely DIFFERENT from a distance filter is not enough; it has to
	// be different in the direction the rules module is. A refused submission changes
	// nothing (§4.9), so this is safe to run last against the same bridge.
	const FString HashBefore = Bridge.StateHash();
	const FStratResult Submitted = Bridge.SubmitMoveToHex(ProbeUnit, ProbeHex);
	TestFalse(
		TEXT("T-UI-02: the rules module refuses that same move, so the machine's refusal agrees "
		     "with the authority rather than merely disagreeing with the foil"),
		Submitted.bOk);
	TestEqual(TEXT("and the refused submission changed nothing"), Bridge.StateHash(), HashBefore);
	AddInfo(FString::Printf(TEXT("rules module's refusal: %s"), *Submitted.Reason));

	return true;
}

// ---------------------------------------------------------------------------
// T-TURN-01 -- a refused command leaves the machine exactly where it was.
//
// WHY THIS IS THE CLAUSE THAT PROTECTS THE DESIGN. StratSelectionMachine.h states the
// load-bearing call in the file: the machine holds a selected id and two id sets and
// NOTHING ELSE, because "a rejected command cannot desynchronise the machine: there is no
// mirror of the rules state to go stale, because the mirror was never made". The rejected
// alternative -- an optimistic Moved/Acted state rolled back on refusal -- was rejected
// because "the rollback path is the one no test ever exercises". This is that test, and it
// exists so the shape cannot be quietly changed back.
//
// THE REFUSAL IS THE RULES MODULE'S AND IS NOT FAKED. The machine only ever emits a Move
// it found in the reachable set, so the bridge would accept it -- which is the point of
// the routing. To get a genuine refusal the TURN IS ENDED OUT OF BAND, directly on the
// bridge, between the machine deciding and the command being submitted. That is exactly
// the real-world race the split exists for: the world moved under a decision that was
// correct when it was made.
//
// `NotifyCommandApplied` IS NOT CALLED, which is the contract for a refusal, and the
// assertion is that the machine needed no help: its selection and its done-set are
// compared against the values they held BEFORE the event that produced the command.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSelectionRefusedCommandIsConsistentTest,
	"Stratocracy.StratPlay.T-TURN-01.RefusedCommandLeavesMachineConsistent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSelectionRefusedCommandIsConsistentTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSelectionMachineParity;

	// `StratSubmitSelectionCommand` logs a refusal as a Warning in a deliberately different
	// shape from the accepted line. It is expected here, and its absence would mean the
	// refusal path stopped reporting.
	AddExpectedMessagePlain(TEXT("STRAT-CMD refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FStratBridge Bridge;
	FStratViewModel Model;
	FString Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"), SeedAndBuild(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratSelectionMachine Machine;
	FStratBridgeRulesQuery Query(&Bridge);

	const FStratUnitView* const Unit = FirstActiveUnit(Model);
	if (!TestNotNull(TEXT("the side to move has a unit"), Unit))
	{
		return false;
	}
	const int32     UnitId  = Unit->UnitId;
	const FIntPoint UnitHex = Unit->Hex;

	Machine.HandleEvent(EStratSelectionEvent::HexPrimary, UnitHex, Model, Query);
	if (!TestEqual(TEXT("the unit selects"), Machine.GetSelectedUnitId(), UnitId))
	{
		return false;
	}

	// A destination the module itself listed, and one the unit is not already standing on.
	TArray<FIntPoint> Reach;
	FString QueryReason;
	if (!TestTrue(TEXT("the reachable set can be asked"),
			Query.ReachableHexes(UnitId, Reach, QueryReason)))
	{
		AddError(QueryReason);
		return false;
	}
	FIntPoint Dest = FIntPoint::ZeroValue;
	bool bFound = false;
	for (const FIntPoint& H : Reach)
	{
		if (H != UnitHex && FindUnitAtHex(Model, H) == nullptr)
		{
			Dest = H;
			bFound = true;
			break;
		}
	}
	if (!TestTrue(TEXT("the module lists somewhere for it to go"), bFound))
	{
		return false;
	}

	// ---- the state the clause compares against ------------------------------
	const int32       SelectedBefore = Machine.GetSelectedUnitId();
	const TSet<int32> DoneBefore     = DoneSetOf(Machine, Model);

	const FStratSelectionOutcome Outcome =
		Machine.HandleEvent(EStratSelectionEvent::HexPrimary, Dest, Model, Query);
	if (!TestEqual(TEXT("the click on a reachable empty hex is a Move"),
			static_cast<int32>(Outcome.Command), static_cast<int32>(EStratSelectionCommand::Move)))
	{
		AddError(Outcome.FailureReason);
		return false;
	}
	TestEqual(TEXT("the Move names the selected unit"), Outcome.UnitId, UnitId);
	TestEqual(TEXT("and the clicked hex, q"), Outcome.Hex.X, Dest.X);
	TestEqual(TEXT("and the clicked hex, r"), Outcome.Hex.Y, Dest.Y);

	// ---- the world moves under the decision ---------------------------------
	const FStratResult Ended = Bridge.SubmitEndTurn();
	if (!TestTrue(TEXT("the turn ends out of band, under the machine's decision"), Ended.bOk))
	{
		AddError(Ended.Reason);
		return false;
	}
	const FString HashAfterEndTurn = Bridge.StateHash();

	// ---- the submission, refused --------------------------------------------
	FString FailureReason;
	const bool bApplied = StratSubmitSelectionCommand(Bridge, Outcome, FailureReason);

	TestFalse(TEXT("T-TURN-01: the rules module refuses a command for the side that is no longer to move"),
		bApplied);
	TestFalse(TEXT("T-TURN-01: the refusal carries the rules module's own reason"),
		FailureReason.IsEmpty());
	TestEqual(TEXT("T-TURN-01: a refused command changes nothing (§4.9's no-partial-application)"),
		Bridge.StateHash(), HashAfterEndTurn);
	AddInfo(FString::Printf(TEXT("refusal: %s"), *FailureReason));

	// NotifyCommandApplied is NOT called. That is the contract, and the machine needs no
	// rollback because it holds nothing that could have run ahead.

	TestEqual(
		TEXT("T-TURN-01: the selection is what it was before the event -- no rollback was needed"),
		Machine.GetSelectedUnitId(), SelectedBefore);

	const TSet<int32> DoneAfter = DoneSetOf(Machine, Model);
	TestEqual(TEXT("T-TURN-01: the done set has the same size as before the event"),
		DoneAfter.Num(), DoneBefore.Num());
	for (const FStratUnitView& U : Model.Units)
	{
		TestEqual(
			*FString::Printf(TEXT("T-TURN-01: unit %d's DONE bit is unchanged by the refusal"), U.UnitId),
			DoneAfter.Contains(U.UnitId), DoneBefore.Contains(U.UnitId));
	}

	// And the model the screen would be rebuilt from carries no trace of the refused
	// command either -- which is the T-INT-05 half of the same property.
	FStratViewModel Decorated = Model;
	Machine.DecorateViewModel(Decorated);
	for (const FStratUnitView& U : Decorated.Units)
	{
		TestFalse(
			*FString::Printf(TEXT("unit %d is not marked DONE by a refused Move"), U.UnitId),
			U.bDone);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-INT-05 -- a lock ARISING from the guidance layer, which is the gap this file's header
// block predicted and, until wave B2, could not close.
//
// THE SUBJECT IS `FStratGuidedOpening`, NOT `FStratSelectionMachine`. This clause never
// calls `SetLockedThisTurn`. It arms the guided opening against a seeded bridge, hands it a
// model, and then asks the SELECTION MACHINE what it holds -- so the only thing that can
// have produced a lock is `FStratGuidedOpening::PublishLocks`. Its sibling above drives the
// setter directly and pins the enforcement; this one pins the production.
//
// WHERE THE EXPECTATION COMES FROM. Nothing here decides which unit is exempt. The marked
// unit is read off `FStratUnitView::bIsGuidedMarked`, which `strat::buildUiSnapshot` derives
// from the scenario's `guidedOpening.infantry` PLACEMENT -- so the partition of the guided
// side's units into "the one marked unit" and "everyone else" is the rules module's answer
// and this file merely reads it. The lock set is then compared against that partition
// element for element, including the requirement that the marked unit is NOT locked.
//
// THE ENEMY SIDE IS ASSERTED UNTOUCHED, and that is not padding: `PublishLocks` skips units
// whose `Side` is not the guided seat, and a writer that lost that filter would lock the
// board's other half without any clause here noticing.
//
// THE FALSE HALF IS THE HALF THAT CATCHES A REGRESSION. A `PublishLocks` that only ever set
// -- never cleared -- would pass every assertion up to the retirement of beat 1a. So the
// marked unit's move is applied to the MODEL as fixture input, `Observe` is called again in
// the SAME turn, and every lock must be gone. That is `StratViewModel.h`'s "un-locked and
// not-done in the same turn" observed at the machine.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSelectionLockArisesFromGuidanceTest,
	"Stratocracy.StratPlay.T-INT-05.LockArisesFromTheGuidanceLayer",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSelectionLockArisesFromGuidanceTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSelectionMachineParity;

	FStratBridge Bridge;
	FStratViewModel Model;
	FString Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"), SeedAndBuild(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	const int32 GuidedSide = Model.ViewingSide;

	// The rules module's own answer for who beat 1a is about. Read, never derived: a hex
	// comparison against `guidedOpening.infantry` would stop being true the instant the
	// marked unit moved, which is the case the second half of this clause exercises.
	int32 MarkedId = INDEX_NONE;
	for (const FStratUnitView& U : Model.Units)
	{
		if (U.Side == GuidedSide && U.bIsGuidedMarked)
		{
			MarkedId = U.UnitId;
			break;
		}
	}
	if (!TestTrue(TEXT("the shipped scenario marks an Infantry for the viewing seat"),
			MarkedId != INDEX_NONE))
	{
		return false;
	}

	FStratSelectionMachine Machine;
	FStratGuidedOpening    Guidance;

	// Nothing is locked before the guidance layer runs, so what follows is attributable.
	for (const FStratUnitView& U : Model.Units)
	{
		if (!TestFalse(
				*FString::Printf(TEXT("unit %d starts unlocked, so the lock below is the "
				                      "guidance layer's doing"), U.UnitId),
				Machine.IsLockedThisTurn(U.UnitId)))
		{
			return false;
		}
	}

	Guidance.Begin(Bridge, GuidedSide, /*bSuppressed*/ false);
	if (!TestTrue(TEXT("the shipped scenario arms a guided opening for the viewing seat"),
			Guidance.IsActive()))
	{
		return false;
	}

	Model.Match.Turn = FStratGuidedOpening::kFirstGuidedTurn;
	Guidance.Observe(Model, Machine);

	if (!TestTrue(TEXT("beat 1a is outstanding on turn 1, which is the lock's condition"),
			Guidance.IsBeatOutstanding(EStratGuidanceBeat::Beat1a)))
	{
		return false;
	}

	// ---- the lock arose, on exactly the module's partition -------------------
	int32 LockedCount = 0;
	for (const FStratUnitView& U : Model.Units)
	{
		const bool bIsGuidedSeat = (U.Side == GuidedSide);
		const bool bExpected     = bIsGuidedSeat && (U.UnitId != MarkedId);

		TestEqual(
			*FString::Printf(
				TEXT("T-INT-05: unit %d (side %d, marked=%d) is locked exactly when it is a "
				     "non-marked unit of the guided seat"),
				U.UnitId, U.Side, U.bIsGuidedMarked ? 1 : 0),
			Machine.IsLockedThisTurn(U.UnitId), bExpected);

		LockedCount += Machine.IsLockedThisTurn(U.UnitId) ? 1 : 0;
	}

	// The clause would be vacuous on a scenario that deploys one unit per side; assert it is
	// not, rather than assuming the shipped board.
	TestTrue(
		TEXT("T-INT-05: at least one lock actually arose, so the comparison above had content"),
		LockedCount > 0);
	TestFalse(TEXT("T-INT-05: and the marked unit is never the one locked"),
		Machine.IsLockedThisTurn(MarkedId));

	// ---- and it clears INSIDE turn 1, when beat 1a retires -------------------
	// FIXTURE CONSTRUCTION, declared where it happens: `bHasMoved` is the INPUT and the lock
	// set is the output being asserted. The turn is deliberately not advanced -- the whole
	// point is that the lock's lifecycle is not the turn boundary's.
	for (FStratUnitView& U : Model.Units)
	{
		if (U.UnitId == MarkedId)
		{
			U.bHasMoved = true;
		}
	}
	Guidance.Observe(Model, Machine);

	TestFalse(TEXT("beat 1a retired on the marked Infantry's move"),
		Guidance.IsBeatOutstanding(EStratGuidanceBeat::Beat1a));
	TestEqual(TEXT("T-INT-05: the turn did not advance, so this is a mid-turn clear"),
		Model.Match.Turn, FStratGuidedOpening::kFirstGuidedTurn);

	for (const FStratUnitView& U : Model.Units)
	{
		TestFalse(
			*FString::Printf(
				TEXT("T-INT-05: unit %d is un-locked in the same turn beat 1a retired "
				     "(StratViewModel.h: \"un-locked and not-done in the same turn\")"),
				U.UnitId),
			Machine.IsLockedThisTurn(U.UnitId));
		TestFalse(
			*FString::Printf(TEXT("and unit %d is still NOT done -- the two lifecycles differ"),
				U.UnitId),
			Machine.IsDone(U.UnitId));
	}

	return true;
}
