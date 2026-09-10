// T-SAVE-04 -- `FStratSelectionMachine::Reset()` itself, asserted directly and against nothing
// else.
//
// WHY THIS FILE EXISTS, AND IT IS A GAP A REVIEWER FOUND RATHER THAN ONE THIS LANE PLANNED FOR.
// `Stratocracy.StratPlay.T-SAVE-04.LoadClearsControllerSidePresentationState`, in
// `StratLoadPresentationCarryOverClauses.cpp`, takes its ENTIRE expectation from a live oracle:
// a second `FStratSelectionMachine` and a second `FStratBuildAffordance`, driven in lockstep
// with the controller's own pair and then `Reset()`. That shape is deliberate and it is what
// keeps the clause from writing down a `bDone` -- but it has one failure mode, and the failure
// mode is ASYMMETRIC between the two halves:
//
//   - `FStratBuildAffordance::Reset()` IS independently pinned. `StratBuildAffordanceClauses.cpp`
//     asserts, under T-UI-03, that a reset drops the focus AND the observation fields, measured
//     by a subsequent `Observe` of a different turn behaving as a FIRST observation. So if that
//     body regressed, a clause other than the oracle's own would go red.
//
//   - `FStratSelectionMachine::Reset()` WAS NOT PINNED BY ANYTHING. Measured on this tree: the
//     only references to it in `Source/` were its own declaration, its own definition, the one
//     shipping call site in `AStratPlayerController::SyncPresentationToMatchEpoch`, and the
//     T-SAVE-04 oracle. **If this body ever stopped clearing `DoneUnits`, the oracle and the
//     controller's machine would carry the SAME stale bit, and T-SAVE-04's legs (1) and (3)
//     would go GREEN OVER THE RESTORED DEFECT.** A clause whose expectation is a live function
//     is only as good as a second, independent statement about that function. This is the
//     second statement.
//
// SO THE EXPECTED VALUES HERE ARE WRITTEN DOWN, AND THAT IS THE POINT RATHER THAN AN EXCEPTION
// GRUDGINGLY TAKEN. Everywhere else in this tree, reading the expectation off the code under
// test is the defect -- `.agents/ue-project-context.md` states it as the house rule. Here the
// subject IS the function every other clause in this family reads its expectation from, so
// asking it what it thinks it does is precisely the thing that would defeat the purpose.
// `INDEX_NONE` and `false` are spelled out, from `FStratSelectionMachine`'s own declaration of
// what `Reset()` means: "Drops the selection and both sets."
//
// AND IT SHARES NO MACHINERY WITH THE CLAUSE IT BACKSTOPS. No oracle, no `LoadMatchFromSlot`,
// no `AStratPlayerController`, no `UStratMatchSubsystem`, no `FStratBridge`, no world, no PIE,
// no data table, no save slot. A hand-built model, a stub query that refuses everything, and
// one struct. A defect that could take out the T-SAVE-04 fixture cannot reach this file.
//
// THE STUB QUERY REFUSES EVERY CALL ON PURPOSE. `HandleEvent` consults the query only to build
// a move or an attack; selecting a friendly unit and spending a Wait consult it not at all. A
// query that refuses everything therefore makes this file's independence a MEASURED property
// rather than a claim about its include list: if any path below had gone through the rules
// module, the clause would not pass.
//
// NO `strat::` FREE FUNCTION IS CALLED ANYWHERE BELOW -- LNK2019 outside StratBridge and
// StratRules, measured 8 x.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Math/IntPoint.h"

#include "StratSelectionMachine.h"
#include "StratViewModel.h"

namespace StratSelectionMachineReset
{
	/** The three units the clause needs, and the three roles it needs them in. */
	static const int32 kDoneUnitId     = 7;
	static const int32 kLockedUnitId   = 9;
	static const int32 kSelectedUnitId = 11;

	static const int32 kSide = 0;

	static FIntPoint HexOf(int32 UnitId)
	{
		return FIntPoint(UnitId, UnitId);
	}

	/**
	 * Three friendly units of the side to move, on distinct hexes. HAND-BUILT, and the struct
	 * was designed for it -- `FStratSelectionMachine`'s header states it is "drivable with no
	 * actor, no PIE, no world and no Slate in existence".
	 */
	static FStratViewModel MakeModel()
	{
		FStratViewModel Model;
		Model.ViewingSide      = kSide;
		Model.Match.Turn       = 1;
		Model.Match.SideToMove = kSide;
		Model.Match.bHasResult = false;

		for (const int32 Id : { kDoneUnitId, kLockedUnitId, kSelectedUnitId })
		{
			FStratUnitView U;
			U.UnitId = Id;
			U.Side   = kSide;
			U.Hex    = HexOf(Id);
			Model.Units.Add(U);
		}
		return Model;
	}

	/** Refuses every question. See the file block: nothing below is allowed to need one. */
	struct FRefusingQuery final : public IStratRulesQuery
	{
		virtual bool ReachableHexes(int32 /*UnitId*/, TArray<FIntPoint>& OutHexes,
		                            FString& OutFailureReason) const override
		{
			OutHexes.Reset();
			OutFailureReason = TEXT("this clause must not need a rules answer");
			return false;
		}

		virtual bool AttackTargetHexes(int32 /*UnitId*/, TArray<FIntPoint>& OutHexes,
		                               FString& OutFailureReason) const override
		{
			OutHexes.Reset();
			OutFailureReason = TEXT("this clause must not need a rules answer");
			return false;
		}
	};
}

// ---------------------------------------------------------------------------------------------
// T-SAVE-04 -- `Reset()` empties the selection, the done set and the lock set.
//
// THE ACCEPTANCE ID IS THE USER'S, RULED IN SESSION. It rides T-SAVE-04 rather than T-UI-02 or
// T-INT-05 because what this clause protects is the LOAD clause's oracle, and an id is worth
// more pointing at the property it defends than at the file it happens to sit beside.
//
// THE THREE FIELDS ARE ASSERTED SEPARATELY AND NONE IS INFERRED FROM ANOTHER.
// `FStratSelectionMachine` holds exactly `SelectedUnitId`, `DoneUnits` and `LockedUnits`, and
// they have three different lifecycles -- `NotifyCommandApplied` clears `DoneUnits` on an
// `EndTurn` and deliberately leaves `LockedUnits` alone, because a lock's life ends when beat 1a
// retires and not at a turn boundary. A clause that checked one and assumed the others would
// pass over a `Reset()` that had quietly become a turn boundary.
//
// EACH FIELD CARRIES A BEFORE-READING THAT IS ASSERTED, NOT ASSUMED. Without it, a `Reset()`
// that cleared nothing would be indistinguishable from a fixture that had set nothing, and the
// clause would pass green on a machine it never managed to dirty. That control is the
// difference between this file backstopping the oracle and merely appearing to.
//
// AND THE LAST LEG TIES THE FIELDS TO THE CONSEQUENCE. After the reset, the unit that was done
// is SELECTABLE again -- the observable half of what the load clause's leg (3) measures,
// asserted here with no load path, no controller and no subsystem anywhere near it.
// ---------------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSelectionMachineResetEmptiesAllThreeFieldsTest,
	"Stratocracy.StratPlay.T-SAVE-04.SelectionMachineResetEmptiesSelectionDoneAndLocked",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSelectionMachineResetEmptiesAllThreeFieldsTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSelectionMachineReset;

	const FStratViewModel Model = MakeModel();
	const FRefusingQuery  Query;

	FStratSelectionMachine Machine;

	// ---- dirty all three fields, through the struct's own public calls ----------------------
	//
	// A Wait spends unit 7's turn and adds it to `DoneUnits` -- there is no `Wait` in
	// `strat::SaveCommandKind`, so this reaches no rules module and needs no query.
	Machine.HandleEvent(EStratSelectionEvent::HexPrimary, HexOf(kDoneUnitId), Model, Query);
	Machine.HandleEvent(EStratSelectionEvent::Wait,       HexOf(kDoneUnitId), Model, Query);

	// The per-turn lock, through the guidance layer's own writer.
	Machine.SetLockedThisTurn(kLockedUnitId, true);

	// And a live selection, taken LAST so the Wait's own deselect cannot undo it.
	Machine.HandleEvent(EStratSelectionEvent::HexPrimary, HexOf(kSelectedUnitId), Model, Query);

	// ---- THE CONTROL: all three are really dirty --------------------------------------------
	TestTrue(TEXT("CONTROL: unit 7 is in the done set before the reset -- without this, a Reset "
	              "that cleared nothing would be indistinguishable from a fixture that set "
	              "nothing"),
		Machine.IsDone(kDoneUnitId));
	TestTrue(TEXT("CONTROL: unit 9 is in the lock set before the reset"),
		Machine.IsLockedThisTurn(kLockedUnitId));
	TestEqual(TEXT("CONTROL: unit 11 is selected before the reset"),
		Machine.GetSelectedUnitId(), kSelectedUnitId);

	// ---- THE CLAIM ---------------------------------------------------------------------------
	//
	// THE EXPECTED VALUES ARE SPELLED OUT AND NOT ASKED FOR. See the file block: this is the one
	// place in this family where reading the expectation off the code under test would defeat
	// the purpose, because the code under test is what every other clause in the family reads
	// ITS expectation from.
	Machine.Reset();

	TestEqual(
		TEXT("T-SAVE-04: Reset() drops the selection -- FStratSelectionMachine declares it as "
		     "\"Drops the selection and both sets\", and INDEX_NONE is how \"no selection\" is "
		     "spelled on GetSelectedUnitId"),
		Machine.GetSelectedUnitId(), INDEX_NONE);

	TestFalse(
		TEXT("T-SAVE-04: Reset() empties DoneUnits. THIS IS THE ASSERTION THE T-SAVE-04 LOAD "
		     "CLAUSE CANNOT MAKE ABOUT ITSELF: that clause takes its whole expectation from a "
		     "reset oracle, so a Reset() that stopped clearing the done set would leave the "
		     "oracle and the controller carrying the SAME stale bit and turn its legs (1) and "
		     "(3) green over the restored defect"),
		Machine.IsDone(kDoneUnitId));

	TestFalse(
		TEXT("T-SAVE-04: Reset() empties LockedUnits, which is a THIRD lifecycle and not "
		     "inferable from the other two -- NotifyCommandApplied clears DoneUnits on an "
		     "EndTurn and deliberately leaves the locks alone, so a Reset() that had become a "
		     "turn boundary would pass a clause that checked only the done set"),
		Machine.IsLockedThisTurn(kLockedUnitId));

	// Every unit, in both sets, so the assertions above are about a SET BEING EMPTY rather than
	// about one id having been removed from it.
	for (const int32 Id : { kDoneUnitId, kLockedUnitId, kSelectedUnitId })
	{
		TestFalse(*FString::Printf(
				TEXT("T-SAVE-04: no unit is done after the reset, including unit %d"), Id),
			Machine.IsDone(Id));
		TestFalse(*FString::Printf(
				TEXT("T-SAVE-04: no unit is locked after the reset, including unit %d"), Id),
			Machine.IsLockedThisTurn(Id));
	}

	// ---- THE CONSEQUENCE, tied to the fields with nothing else in between --------------------
	//
	// `HandleEvent`'s selection gate refuses a unit in `DoneUnits` with "unit N has finished
	// this turn". After the reset it must not, and that is the observable half of what the load
	// clause's leg (3) measures -- asserted here with no load path in sight.
	const FStratSelectionOutcome AfterReset = Machine.HandleEvent(
		EStratSelectionEvent::HexPrimary, HexOf(kDoneUnitId), Model, Query);

	TestEqual(
		TEXT("T-SAVE-04: and the unit that was done is selectable again after the reset"),
		AfterReset.SelectedUnitId, kDoneUnitId);
	TestEqual(
		*FString::Printf(
			TEXT("T-SAVE-04: with no refusal sentence -- the done-set gate is what would "
			     "produce one (got: '%s')"),
			*AfterReset.FailureReason),
		AfterReset.FailureReason, FString());

	// ---- AND IT IS IDEMPOTENT ----------------------------------------------------------------
	//
	// Cheap, and it pins that `Reset()` on an already-clean machine is not a state change --
	// `SyncPresentationToMatchEpoch` is free to call it on an epoch move that followed another.
	Machine.Reset();
	TestEqual(TEXT("T-SAVE-04: a second Reset() leaves the selection dropped"),
		Machine.GetSelectedUnitId(), INDEX_NONE);
	TestFalse(TEXT("T-SAVE-04: and the done set empty"), Machine.IsDone(kDoneUnitId));
	TestFalse(TEXT("T-SAVE-04: and the lock set empty"),
		Machine.IsLockedThisTurn(kLockedUnitId));

	return true;
}
