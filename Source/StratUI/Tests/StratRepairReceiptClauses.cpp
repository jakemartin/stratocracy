// The decider half of §2.11.6's repair receipt -- `StratDecideTransientReceipts`'s rule 5,
// the `EStratReceiptKind::UnitRepair` arm.
//
// THE ID IS `GATE-TRANSIENT`, WHICH IS THE ID EVERY OTHER RECEIPT CLAUSE IN THIS DIRECTORY
// ALREADY RIDES, and it is a local gate name that mints no acceptance ID. That was checked
// against the corpus rather than chosen: every transient-receipt clause in
// `Source/StratUI/Tests/` and `Source/StratPlay/Tests/` files under `GATE-TRANSIENT` and none
// under any `T-UI-` id. `T-UI-03` WAS PROPOSED FOR THESE AND IS REFUSED, on
// `StratTransientReceiptClauses.cpp`'s own reasoning one arm across: §4.11 states T-UI-03 as
// "the live standings scoreboard binds 1:1 to snapshot fields ... with no widget-side
// arithmetic", and nothing below is about the scoreboard. Borrowing a stronger acceptance ID
// than a clause earns is a false clearance of that ID.
//
// WHAT THIS FILE IS FOR, IN ONE SENTENCE: the bridge measured a repair, and this layer must
// carry that number across unchanged -- so every clause here is about TRANSPORT, and not one
// of them asserts that any repair was computed correctly. That is
// `Source/StratBridge/Tests/StratRepairReceiptParity.cpp`'s subject and it is the only place
// `strat::repairAmount` can legally be called.
//
// THE ONE CLAUSE THAT CARRIES THE FILE. `RepairAmountIsCopiedAndNotResubtracted` hands the
// decider a `FStratUnitRepairView` whose `Amount` DELIBERATELY DISAGREES with
// `HpAfter - HpBefore`. That is the only construction that can tell a copy from a
// re-subtraction: on any consistent input the two implementations agree exactly, so a clause
// built on a realistic row would be green over both and pin nothing. `StratTransientReceipts.h`
// forbids a second subtraction in this file in as many words -- "an HP delta IS that second
// subtraction and the claim is right to forbid it here" -- and this clause is what makes that
// prohibition a gate rather than a paragraph.
//
// WHERE THE EXPECTATIONS COME FROM.
//   - THE BASE FIXTURE IS A REAL PROJECTION, every time: an `FStratBridge` seeded from the
//     shipped tables and the shipped scenario, an `FStratViewModel` built from that same
//     bridge, and a REAL turn edge produced by `FStratBridge::SubmitEndTurn` -- so the
//     (Turn, SideToMove) pair that gates rule 5 moves because the RULES moved it. No turn
//     number, side count or HP figure is written in this file.
//   - THE REPAIR ROWS ARE PLANTED, and every number in them is a module reading taken off
//     that projection. The plant is fixture construction on `StratMatchReconcile.cpp`'s and
//     `StratTransientReceiptClauses.cpp`'s precedent, and it is a plant rather than a real
//     bridge measurement because reaching a genuine §2.7 repair needs
//     `strat::parseSave` -- a vendored free function, `LNK2019` outside StratBridge and
//     StratRules, measured 8x. THE REAL MEASUREMENT IS PINNED IN THAT MODULE; what is pinned
//     here is that whatever it measured arrives unaltered.
//   - THE EXPECTATION IS ALWAYS THE INPUT ROW'S OWN FIELD. Nothing below recomputes an
//     amount, and nothing below asserts `Amount == DurableAfter - DurableBefore`, which would
//     be checking the struct against itself and would pass over the very defect this file
//     exists to catch.
//
// NOTHING BELOW COMPARES TWO `FString`s. `==`, `Contains` and `TestEqual` on `FString` are all
// case-insensitive in this engine and that has already produced a clause in this project that
// could not fail. Every value asserted here is an integer or an enumerator, and enumerators
// go through `static_cast<int32>` so a failure prints the ordinal.
//
// NO `strat::` FREE FUNCTION IS CALLED. Naming `strat::UiSnapshot` costs no symbol; calling a
// vendored free function outside StratBridge and StratRules is a link error.
//
// NOT PINNED HERE, WITH REASONS.
//   - THAT `UStratMatchSubsystem::ApplyView` PASSES THE BRIDGE'S ANSWER RATHER THAN THE SHORT
//     FORM'S EMPTY ONE. That needs a live subsystem AND a bridge standing at a real repairing
//     turn opening. The second half is what blocks it, not the first -- see this file's entry
//     in `Tools/architect/state/tests.md`, which names the exact bridge method that would
//     unblock it.
//   - A BLOCKED REPAIR, A ZERO AMOUNT, OR ANYTHING DRAWN OFF AN ABSENCE.
//     `StratTransientReceipts.h`'s "positive case only" paragraph states that an absent
//     receipt cannot be told apart from full HP, from a unit off an owned objective, or from
//     §2.7's anti-fortress lock. A clause implying otherwise would assert a discrimination the
//     code deliberately does not have.
//   - COPY, TOAST DURATION, STACKING, ANIMATION AND PIXELS. Every decider under test is a free
//     function over plain reflected values; nothing below constructs a widget or a world.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratTransientReceipts.h"
#include "StratViewModel.h"

// The bridge, included from a .cpp and never from a UHT-parsed header -- the same constraint
// `StratViewModel.h` records about itself. A test file is a .cpp, so it may say this.
#include "StratBridge.h"

namespace StratRepairReceiptClauses
{
	/** The harness pin for which side moves first. `Replay.h` states that no rule in this
	 *  project decides it, so this exists only so the projected opening is the same one every
	 *  run; the neighbouring receipt, scoreboard and view-model files pin the same value for
	 *  the same stated reason. */
	static const int32 kFirstSide = 0;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** Brings a bridge to the seeded opening. Duplicated rather than shared, which is this
	 *  project's standing posture for this helper: sharing would mean a header inside a
	 *  `Tests/` directory that several modules include. */
	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		UDataTable* Units   = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
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

		const FString ScenarioPath = FPaths::Combine(DataDir(), TEXT("ferrum_crossing.json"));
		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath, kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	/**
	 * A seeded bridge, the mark taken at the opening, and the model rebuilt after a REAL end
	 * of turn.
	 *
	 * THE EDGE IS THE RULES MODULE'S. Rule 5 fires on the same (Turn, SideToMove) edge the
	 * income arm uses, so every clause that expects a repair receipt needs one -- and a
	 * fixture that faked it by editing `FStratReceiptMark::Turn` would be pinning the gate
	 * against this file's idea of a turn rather than against a turn.
	 */
	struct FTurnEdge
	{
		FStratBridge      Bridge;
		FStratViewModel   Opening;
		FStratViewModel   Model;      // after the end of turn
		FStratReceiptMark Mark;       // taken at the opening, before the edge

		bool Build(FString& OutError)
		{
			if (!SeedBridge(Bridge, OutError))
			{
				return false;
			}
			if (!StratBuildViewModel(Bridge, /*ViewingSide*/ 0, Opening, OutError))
			{
				return false;
			}

			Mark = StratMarkFromView(Opening);

			const FStratResult Ended = Bridge.SubmitEndTurn();
			if (!Ended.bOk)
			{
				OutError = FString::Printf(TEXT("SubmitEndTurn refused: %s"), *Ended.Reason);
				return false;
			}
			return StratBuildViewModel(Bridge, /*ViewingSide*/ 0, Model, OutError);
		}

		/** True when the rules really did move the pair rule 5 is gated on. */
		bool EdgeIsReal() const
		{
			return Model.Match.Turn != Opening.Match.Turn
			    || Model.Match.SideToMove != Opening.Match.SideToMove;
		}
	};

	static int32 CountOfKind(const FStratTransientReceipts& Receipts, EStratReceiptKind Kind)
	{
		int32 Count = 0;
		for (const FStratReceiptView& Receipt : Receipts.Receipts)
		{
			if (Receipt.Kind == Kind)
			{
				++Count;
			}
		}
		return Count;
	}

	static const FStratReceiptView* FirstOfKind(const FStratTransientReceipts& Receipts,
	                                            EStratReceiptKind              Kind)
	{
		for (const FStratReceiptView& Receipt : Receipts.Receipts)
		{
			if (Receipt.Kind == Kind)
			{
				return &Receipt;
			}
		}
		return nullptr;
	}

	static FString DescribeReceipts(const FStratTransientReceipts& Receipts)
	{
		if (Receipts.Receipts.Num() == 0)
		{
			return TEXT("<empty>");
		}

		TArray<FString> Parts;
		for (const FStratReceiptView& Receipt : Receipts.Receipts)
		{
			Parts.Add(FString::Printf(
				TEXT("{kind=%d side=%d unit=%d turn=%d amount=%d before=%d after=%d}"),
				static_cast<int32>(Receipt.Kind), Receipt.Side, Receipt.UnitId, Receipt.Turn,
				Receipt.Amount, Receipt.DurableBefore, Receipt.DurableAfter));
		}
		return FString::Join(Parts, TEXT(", "));
	}
}

// ---------------------------------------------------------------------------
// THE AMOUNT IS COPIED FROM THE BRIDGE'S ROW, NOT RE-SUBTRACTED FROM ITS TWO HP READINGS.
//
// THIS IS THE CLAUSE THE FILE EXISTS FOR. `StratTransientReceipts.cpp` opens with a standing
// claim that it contains exactly ONE subtraction, and `StratTransientReceipts.h` states that
// an HP delta would be a second one and is forbidden here -- because HP falls under §2.6, rises
// under §2.7, and arrives fresh at `hpMax` on a spawn, so a difference taken at this layer has
// three possible causes and this layer cannot say which it saw. THE ARITHMETIC LIVES IN THE
// BRIDGE, where it is bracketed around the single `applyCommand` that opens a turn.
//
// ASSERTING AGREEMENT WOULD PROVE NOTHING, WHICH IS WHY THE FIXTURE IS BUILT TO DISAGREE.
// Given a consistent row -- the only kind the bridge ever produces -- `Amount` and
// `HpAfter - HpBefore` are the same number, so `Receipt.Amount == Repair.Amount` and
// `Receipt.Amount == Repair.HpAfter - Repair.HpBefore` are both true of a copy AND of a
// re-subtraction. The row handed in below makes them different numbers, and BOTH ARE NONZERO
// AND DISTINCT so that the wrong answer is a wrong value rather than a zero that could be
// confused with the arm not firing at all.
//
// EVERY PLANTED NUMBER IS A MODULE READING. `HpBefore` is a projected unit's own `Hp`; the
// delta is that side's `IncomePerTurn`; the `Amount` is that side's `FameTotal`. Their
// PROVENANCE is the fixture and none of them is an expectation -- what is asserted is that
// the three come back out where they went in. Using module values rather than literals is
// this directory's posture and it keeps the fixture from carrying a number nobody produced.
//
// THE OTHER TWO FIELDS ARE PINNED IN THE SAME BREATH. `DurableBefore` and `DurableAfter` must
// be the row's two HP readings VERBATIM, because they are what a drawing layer would size a
// bar from -- and because a decider that had quietly recomputed `Amount` from them would most
// likely have written a consistent triple, which this clause's inequality forbids.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRepairAmountIsCopiedTest,
	"Stratocracy.StratUI.GATE-TRANSIENT.RepairAmountIsCopiedAndNotResubtracted",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRepairAmountIsCopiedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRepairReceiptClauses;

	FTurnEdge Edge;
	FString   Error;
	if (!TestTrue(TEXT("the bridge seeds, ends a turn and the model rebuilds"),
			Edge.Build(Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the (Turn, SideToMove) pair moved, and the rules moved it"),
			Edge.EdgeIsReal()))
	{
		return false;
	}
	if (!TestTrue(TEXT("the projection carries a unit and a side to plant from"),
			Edge.Model.Units.Num() > 0 && Edge.Model.Sides.Num() > 0))
	{
		return false;
	}

	const FStratUnitView& Unit = Edge.Model.Units[0];
	const FStratSideView& Side = Edge.Model.Sides[0];

	// ---- the plant: three module readings, arranged so the two candidate expressions
	//      cannot agree ---------------------------------------------------------------
	FStratUnitRepairView Repair;
	Repair.UnitId   = Unit.UnitId;
	Repair.Side     = Unit.Side;
	Repair.HpBefore = Unit.Hp;
	Repair.HpAfter  = Unit.Hp + Side.IncomePerTurn;   // the delta a re-subtraction would find
	Repair.HpMax    = Unit.HpMax;
	Repair.Amount   = Side.FameTotal;                 // the number a copy must carry

	const int32 Delta = Repair.HpAfter - Repair.HpBefore;

	// THE FIXTURE PRECONDITIONS, ASSERTED BEFORE ANYTHING IS DECIDED. Without all three this
	// clause would be inert while looking green -- which is exactly the trap the neighbouring
	// income clause records having fallen into with a zero `Before`.
	if (!TestTrue(TEXT("the planted amount is nonzero, so a copy is distinguishable from nothing"),
			Repair.Amount != 0))
	{
		return false;
	}
	if (!TestTrue(TEXT("the planted HP delta is nonzero, so a re-subtraction is distinguishable "
	                   "from nothing"),
			Delta != 0))
	{
		return false;
	}
	if (!TestTrue(*FString::Printf(
			TEXT("the fixture makes 'the bridge's amount' (%d) and 'the difference of the two HP "
			     "readings' (%d) DIFFERENT numbers -- without which a copy and a re-subtraction "
			     "produce identical output and this clause cannot fail"),
			Repair.Amount, Delta),
			Repair.Amount != Delta))
	{
		return false;
	}

	TArray<FStratUnitRepairView> TurnRepairs;
	TurnRepairs.Add(Repair);

	FStratTransientReceipts Answer;
	StratDecideTransientReceipts(Edge.Mark, Edge.Model, TurnRepairs, Answer);

	// THE DISCRIMINATION, WRITTEN INTO THE REPORT SO IT IS CHECKABLE FROM A CHECKOUT RATHER
	// THAN TAKEN ON TRUST. A reader of a green run can see that the two candidate answers were
	// genuinely different numbers and which of them the decider produced.
	AddInfo(FString::Printf(
		TEXT("the bridge's amount is %d; HpAfter - HpBefore is %d; the receipt carries %d"),
		Repair.Amount, Delta,
		FirstOfKind(Answer, EStratReceiptKind::UnitRepair) != nullptr
			? FirstOfKind(Answer, EStratReceiptKind::UnitRepair)->Amount : INDEX_NONE));

	if (!TestEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: one repair row on a real turn edge fires exactly one "
			     "UnitRepair receipt: %s"),
			*DescribeReceipts(Answer)),
			CountOfKind(Answer, EStratReceiptKind::UnitRepair), 1))
	{
		return false;
	}

	const FStratReceiptView* const Receipt =
		FirstOfKind(Answer, EStratReceiptKind::UnitRepair);
	if (!TestNotNull(TEXT("the UnitRepair receipt is readable"), Receipt))
	{
		return false;
	}

	// ---- the assertion the whole file is for ------------------------------------------
	TestEqual(TEXT("GATE-TRANSIENT: the receipt's Amount is the BRIDGE's Amount, copied"),
		Receipt->Amount, Repair.Amount);
	TestNotEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: and it is NOT HpAfter - HpBefore -- this layer performs no HP "
			     "arithmetic (%s)"),
			*DescribeReceipts(Answer)),
		Receipt->Amount, Delta);

	// ---- and the two readings are carried verbatim beside it ---------------------------
	TestEqual(TEXT("GATE-TRANSIENT: DurableBefore is the row's HpBefore, verbatim"),
		Receipt->DurableBefore, Repair.HpBefore);
	TestEqual(TEXT("GATE-TRANSIENT: DurableAfter is the row's HpAfter, verbatim"),
		Receipt->DurableAfter, Repair.HpAfter);
	TestEqual(TEXT("GATE-TRANSIENT: the receipt names the unit the row named"),
		Receipt->UnitId, Repair.UnitId);
	TestEqual(TEXT("GATE-TRANSIENT: and the side the row named, not the viewer's"),
		Receipt->Side, Repair.Side);
	TestEqual(TEXT("GATE-TRANSIENT: and it is placed on the turn the model reads"),
		Receipt->Turn, Edge.Model.Match.Turn);

	return true;
}

// ---------------------------------------------------------------------------
// A REPAIR RECEIPT NAMES A UNIT; THE TWO SIDE-SCOPED ARMS NAME NONE.
//
// `FStratReceiptView::UnitId` declares both halves and neither is observable without the
// other: "NEVER `INDEX_NONE` ON `UnitRepair`" and "`INDEX_NONE` ON `IncomeRate` AND
// `KillFame`, AND THAT IS A STATEMENT RATHER THAN A DEFAULT NOBODY SET". A drawing layer that
// branched on `Side` alone would put a repair floater on a scoreboard, which is the failure
// the field exists to prevent -- so a clause that checked only the repair arm would leave the
// half that actually stops it unpinned.
//
// ALL THREE ARMS IN ONE ANSWER, which is what makes the statement about the ANSWER rather
// than about three separate calls: the same refresh carries a kill receipt, an income receipt
// and a repair receipt, and each must be identifiable by its own kind and its own unit field.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRepairReceiptCarriesTheUnitIdTest,
	"Stratocracy.StratUI.GATE-TRANSIENT.OnlyTheRepairArmNamesAUnit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRepairReceiptCarriesTheUnitIdTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRepairReceiptClauses;

	FTurnEdge Edge;
	FString   Error;
	if (!TestTrue(TEXT("the bridge seeds, ends a turn and the model rebuilds"),
			Edge.Build(Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the (Turn, SideToMove) pair moved, and the rules moved it"),
			Edge.EdgeIsReal()))
	{
		return false;
	}
	if (!TestTrue(TEXT("the projection carries a unit and two sides"),
			Edge.Model.Units.Num() > 0 && Edge.Model.Sides.Num() >= 2))
	{
		return false;
	}

	// A kill receipt too, so all three arms appear in one answer. The rise is planted from
	// the same side's own `FameTotal` -- a module reading, on the neighbouring file's
	// precedent -- and is asserted to be a real rise before it is relied on.
	const int32 RisingSide = Edge.Model.Match.SideToMove;
	if (!TestTrue(TEXT("the side to move has a mark row and a projected side view"),
			Edge.Model.Sides.IsValidIndex(RisingSide)
			&& Edge.Mark.FameCombat.IsValidIndex(RisingSide)))
	{
		return false;
	}
	if (!TestTrue(TEXT("that side's FameTotal is a usable rise over its marked FameCombat"),
			Edge.Model.Sides[RisingSide].FameTotal > Edge.Mark.FameCombat[RisingSide]))
	{
		return false;
	}

	FStratViewModel Model = Edge.Model;
	Model.Sides[RisingSide].FameCombat = Model.Sides[RisingSide].FameTotal;

	const FStratUnitView& Unit = Model.Units[0];

	FStratUnitRepairView Repair;
	Repair.UnitId   = Unit.UnitId;
	Repair.Side     = Unit.Side;
	Repair.HpBefore = Unit.Hp;
	Repair.HpAfter  = Unit.HpMax;
	Repair.HpMax    = Unit.HpMax;
	Repair.Amount   = Model.Sides[RisingSide].IncomePerTurn;

	TArray<FStratUnitRepairView> TurnRepairs;
	TurnRepairs.Add(Repair);

	FStratTransientReceipts Answer;
	StratDecideTransientReceipts(Edge.Mark, Model, TurnRepairs, Answer);

	// The fixture premise: all three arms really are present in this one answer.
	if (!TestEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: the fixture produces one receipt of each kind: %s"),
			*DescribeReceipts(Answer)),
			Answer.Receipts.Num(), 3))
	{
		return false;
	}

	for (const FStratReceiptView& Receipt : Answer.Receipts)
	{
		if (Receipt.Kind == EStratReceiptKind::UnitRepair)
		{
			TestEqual(*FString::Printf(
					TEXT("GATE-TRANSIENT: the repair arm names the unit the row named (%s)"),
					*DescribeReceipts(Answer)),
				Receipt.UnitId, Repair.UnitId);
			TestNotEqual(TEXT("GATE-TRANSIENT: and it is never INDEX_NONE on this arm"),
				Receipt.UnitId, static_cast<int32>(INDEX_NONE));
		}
		else
		{
			TestEqual(*FString::Printf(
					TEXT("GATE-TRANSIENT: the side-scoped arm of kind %d names no unit (%s)"),
					static_cast<int32>(Receipt.Kind), *DescribeReceipts(Answer)),
				Receipt.UnitId, static_cast<int32>(INDEX_NONE));
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// THE REPAIR ARM FIRES ON THE TURN EDGE AND IS SILENT ON EVERY OTHER REFRESH.
//
// THE GATE IS WHAT MAKES THE ARM SAFE ON A RECONCILED SURFACE, and it is the one place this
// arm's shape differs from the other two. `FStratBridge::RepairsAtTurnOpen` is a RECORD: it
// serves the same list unchanged for the whole turn, so an arm that fired unconditionally
// would re-emit an identical floater on every mouse move. `StratDecideTransientReceipts`'s own
// block says so in terms.
//
// THE SAME NON-EMPTY LIST IS HANDED TO BOTH CALLS, WHICH IS THE POINT. A mid-turn refresh is
// modelled as it really occurs -- the model unchanged, the mark re-taken from that same model,
// and the bridge's record still saying exactly what it said a moment ago. A decider that
// gated on "is the list non-empty" rather than on the edge is green on the first call and
// wrong on the second, and only the second call can see it.
//
// AND THE EDGE CALL IS THE POSITIVE CONTROL. Asserting silence alone is worthless if the
// fixture could not have spoken; the same list on the same model across a real edge must
// produce a receipt first, in the same clause and the same run.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRepairFiresOnlyOnTheTurnEdgeTest,
	"Stratocracy.StratUI.GATE-TRANSIENT.RepairReceiptsFireOnlyOnTheTurnEdge",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRepairFiresOnlyOnTheTurnEdgeTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRepairReceiptClauses;

	FTurnEdge Edge;
	FString   Error;
	if (!TestTrue(TEXT("the bridge seeds, ends a turn and the model rebuilds"),
			Edge.Build(Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the (Turn, SideToMove) pair moved, and the rules moved it"),
			Edge.EdgeIsReal()))
	{
		return false;
	}
	if (!TestTrue(TEXT("the projection carries a unit and a side"),
			Edge.Model.Units.Num() > 0 && Edge.Model.Sides.Num() > 0))
	{
		return false;
	}

	const FStratUnitView& Unit = Edge.Model.Units[0];

	FStratUnitRepairView Repair;
	Repair.UnitId   = Unit.UnitId;
	Repair.Side     = Unit.Side;
	Repair.HpBefore = Unit.Hp;
	Repair.HpAfter  = Unit.HpMax;
	Repair.HpMax    = Unit.HpMax;
	Repair.Amount   = Edge.Model.Sides[0].IncomePerTurn;
	if (!TestTrue(TEXT("the planted amount is nonzero, so a fired receipt is visible"),
			Repair.Amount != 0))
	{
		return false;
	}

	TArray<FStratUnitRepairView> TurnRepairs;
	TurnRepairs.Add(Repair);

	// ---- THE POSITIVE CONTROL: across the real edge, the arm speaks --------------------
	FStratTransientReceipts OnTheEdge;
	StratDecideTransientReceipts(Edge.Mark, Edge.Model, TurnRepairs, OnTheEdge);
	if (!TestEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: across a real turn edge the repair arm fires (%s)"),
			*DescribeReceipts(OnTheEdge)),
			CountOfKind(OnTheEdge, EStratReceiptKind::UnitRepair), 1))
	{
		return false;
	}

	// ---- the mid-turn refresh: same model, mark re-taken from it, same record ----------
	const FStratReceiptMark Remarked = StratMarkFromView(Edge.Model);
	if (!TestTrue(TEXT("the re-taken mark reports itself seeded"), Remarked.bSeeded))
	{
		return false;
	}
	if (!TestTrue(TEXT("the re-taken mark carries the model's own (Turn, SideToMove), so this "
	                   "really is a refresh WITHIN one turn"),
			Remarked.Turn == Edge.Model.Match.Turn
			&& Remarked.SideToMove == Edge.Model.Match.SideToMove))
	{
		return false;
	}

	FStratTransientReceipts MidTurn;
	StratDecideTransientReceipts(Remarked, Edge.Model, TurnRepairs, MidTurn);

	TestEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: a refresh within the same turn re-reads the same record and "
			     "emits no repair receipt (%s)"),
			*DescribeReceipts(MidTurn)),
		CountOfKind(MidTurn, EStratReceiptKind::UnitRepair), 0);

	return true;
}

// ---------------------------------------------------------------------------
// AN INVALID `SideToMove` SILENCES THE INCOME ARM AND NOTHING ELSE.
//
// THIS IS THE CLAUSE OVER THE SCOPE CHANGE, and it is the sharpest structural one in the file.
// Rule 4's "a `sideToMove` with no side view is a projection this file cannot describe"
// refusal used to be an EARLY RETURN, and an early return is a statement about the whole
// function rather than about the side projection: it also silenced rule 5, which reads no side
// view at all. `StratTransientReceipts.cpp` calls the change "load-bearing rather than
// cosmetic" and says the scope is what keeps the refusal the size of its own reason -- and
// nothing but this clause can tell the block from the return, because on every valid model
// they behave identically.
//
// THE INVALID SIDE IS TAKEN FROM THE MODEL'S OWN LENGTH, so the fixture cannot be wrong about
// which index is out of range.
//
// BOTH HALVES ARE ASSERTED. The income arm must be SILENT -- inventing a zero-income receipt
// for a side with no view would put a number on screen that no module produced -- and the
// repair arm must still fire, because a repair row carries its own side and its own unit and
// needs no side view to be describable.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRepairSurvivesAnInvalidSideTest,
	"Stratocracy.StratUI.GATE-TRANSIENT.AnInvalidSideToMoveSilencesOnlyTheIncomeArm",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRepairSurvivesAnInvalidSideTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRepairReceiptClauses;

	FTurnEdge Edge;
	FString   Error;
	if (!TestTrue(TEXT("the bridge seeds, ends a turn and the model rebuilds"),
			Edge.Build(Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the (Turn, SideToMove) pair moved, and the rules moved it"),
			Edge.EdgeIsReal()))
	{
		return false;
	}
	if (!TestTrue(TEXT("the projection carries a unit and a side"),
			Edge.Model.Units.Num() > 0 && Edge.Model.Sides.Num() > 0))
	{
		return false;
	}

	const FStratUnitView& Unit = Edge.Model.Units[0];

	FStratUnitRepairView Repair;
	Repair.UnitId   = Unit.UnitId;
	Repair.Side     = Unit.Side;
	Repair.HpBefore = Unit.Hp;
	Repair.HpAfter  = Unit.HpMax;
	Repair.HpMax    = Unit.HpMax;
	Repair.Amount   = Edge.Model.Sides[0].IncomePerTurn;
	if (!TestTrue(TEXT("the planted amount is nonzero"), Repair.Amount != 0))
	{
		return false;
	}

	TArray<FStratUnitRepairView> TurnRepairs;
	TurnRepairs.Add(Repair);

	// ---- THE POSITIVE CONTROL: on the untouched model both arms fire -------------------
	FStratTransientReceipts Healthy;
	StratDecideTransientReceipts(Edge.Mark, Edge.Model, TurnRepairs, Healthy);
	if (!TestEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: on a valid model the income arm fires (%s)"),
			*DescribeReceipts(Healthy)),
			CountOfKind(Healthy, EStratReceiptKind::IncomeRate), 1))
	{
		return false;
	}
	if (!TestEqual(TEXT("GATE-TRANSIENT: and so does the repair arm"),
			CountOfKind(Healthy, EStratReceiptKind::UnitRepair), 1))
	{
		return false;
	}

	// ---- the model whose `sideToMove` indexes no side view ----------------------------
	FStratViewModel Broken = Edge.Model;
	Broken.Match.SideToMove = Broken.Sides.Num();   // one past the end, off the model's own length
	if (!TestFalse(TEXT("the fixture's SideToMove really is out of range"),
			Broken.Sides.IsValidIndex(Broken.Match.SideToMove)))
	{
		return false;
	}
	// The mark still describes the previous turn, so the edge is still real for this model.
	if (!TestTrue(TEXT("the (Turn, SideToMove) edge is still real against the mark"),
			Edge.Mark.Turn != Broken.Match.Turn
			|| Edge.Mark.SideToMove != Broken.Match.SideToMove))
	{
		return false;
	}

	FStratTransientReceipts Answer;
	StratDecideTransientReceipts(Edge.Mark, Broken, TurnRepairs, Answer);

	TestEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: a sideToMove with no side view emits no income receipt (%s)"),
			*DescribeReceipts(Answer)),
		CountOfKind(Answer, EStratReceiptKind::IncomeRate), 0);

	TestEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: and silences NOTHING ELSE -- the repair arm reads no side view "
			     "and still fires (%s)"),
			*DescribeReceipts(Answer)),
		CountOfKind(Answer, EStratReceiptKind::UnitRepair), 1);

	const FStratReceiptView* const Receipt = FirstOfKind(Answer, EStratReceiptKind::UnitRepair);
	if (TestNotNull(TEXT("the surviving repair receipt is readable"), Receipt))
	{
		TestEqual(TEXT("GATE-TRANSIENT: and it still carries the row's own amount"),
			Receipt->Amount, Repair.Amount);
	}

	return true;
}

// ---------------------------------------------------------------------------
// THE REPAIR RECEIPTS COME AFTER THE INCOME RECEIPT, WHICH COMES AFTER THE KILLS.
//
// THE ORDER IS THE ORDER THE TURN RAN. `StratDecideTransientReceipts`'s rule list says the
// kills happened on the turn that is ENDING and the income belongs to the one BEGINNING, and
// rule 5 is placed after rule 4 "because §2.7's repair belongs to the turn that is beginning,
// exactly as the income does". A reader of the list must never see a repair attributed to the
// turn that just ended, and the list's order is the only thing that says so.
//
// ONE ANSWER CARRYING ALL THREE KINDS, AND TWO REPAIR ROWS, so the clause also pins that the
// repair rows keep the order they arrived in -- the bridge's ascending-unit-id order, which
// `UStratMatchSubsystem::ApplyView` copies across by walking rather than rebuilding. A decider
// that sorted or reversed them would put the floaters out in an order no module chose.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRepairReceiptsFollowTheIncomeTest,
	"Stratocracy.StratUI.GATE-TRANSIENT.RepairReceiptsFollowTheIncomeReceiptInOrder",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRepairReceiptsFollowTheIncomeTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRepairReceiptClauses;

	FTurnEdge Edge;
	FString   Error;
	if (!TestTrue(TEXT("the bridge seeds, ends a turn and the model rebuilds"),
			Edge.Build(Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the (Turn, SideToMove) pair moved, and the rules moved it"),
			Edge.EdgeIsReal()))
	{
		return false;
	}
	if (!TestTrue(TEXT("the projection carries two units and two sides"),
			Edge.Model.Units.Num() >= 2 && Edge.Model.Sides.Num() >= 2))
	{
		return false;
	}

	const int32 RisingSide = Edge.Model.Match.SideToMove;
	if (!TestTrue(TEXT("the side to move has a mark row and a projected side view"),
			Edge.Model.Sides.IsValidIndex(RisingSide)
			&& Edge.Mark.FameCombat.IsValidIndex(RisingSide)))
	{
		return false;
	}
	if (!TestTrue(TEXT("that side's FameTotal is a usable rise over its marked FameCombat"),
			Edge.Model.Sides[RisingSide].FameTotal > Edge.Mark.FameCombat[RisingSide]))
	{
		return false;
	}

	FStratViewModel Model = Edge.Model;
	Model.Sides[RisingSide].FameCombat = Model.Sides[RisingSide].FameTotal;

	// TWO ROWS, IN THE ORDER THE BRIDGE WOULD HAND THEM OVER. The ids are the projection's
	// own; the clause asserts they come back in this sequence, whatever it is.
	TArray<FStratUnitRepairView> TurnRepairs;
	for (int32 i = 0; i < 2; ++i)
	{
		const FStratUnitView& Unit = Model.Units[i];

		FStratUnitRepairView Repair;
		Repair.UnitId   = Unit.UnitId;
		Repair.Side     = Unit.Side;
		Repair.HpBefore = Unit.Hp;
		Repair.HpAfter  = Unit.HpMax;
		Repair.HpMax    = Unit.HpMax;
		Repair.Amount   = Model.Sides[RisingSide].IncomePerTurn;
		TurnRepairs.Add(Repair);
	}
	if (!TestNotEqual(TEXT("the two planted rows name different units, so an order is visible"),
			TurnRepairs[0].UnitId, TurnRepairs[1].UnitId))
	{
		return false;
	}

	FStratTransientReceipts Answer;
	StratDecideTransientReceipts(Edge.Mark, Model, TurnRepairs, Answer);

	// The premise: one kill, one income, two repairs.
	if (!TestEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: the fixture produces four receipts: %s"),
			*DescribeReceipts(Answer)),
			Answer.Receipts.Num(), 4))
	{
		return false;
	}

	int32 LastKillIndex   = INDEX_NONE;
	int32 IncomeIndex     = INDEX_NONE;
	int32 FirstRepairIdx  = INDEX_NONE;
	TArray<int32> RepairUnitIdsInOrder;
	for (int32 Index = 0; Index < Answer.Receipts.Num(); ++Index)
	{
		switch (Answer.Receipts[Index].Kind)
		{
		case EStratReceiptKind::KillFame:
			LastKillIndex = Index;
			break;
		case EStratReceiptKind::IncomeRate:
			IncomeIndex = Index;
			break;
		case EStratReceiptKind::UnitRepair:
			if (FirstRepairIdx == INDEX_NONE)
			{
				FirstRepairIdx = Index;
			}
			RepairUnitIdsInOrder.Add(Answer.Receipts[Index].UnitId);
			break;
		default:
			break;
		}
	}

	if (!TestTrue(TEXT("all three kinds are present"),
			LastKillIndex != INDEX_NONE && IncomeIndex != INDEX_NONE
			&& FirstRepairIdx != INDEX_NONE))
	{
		return false;
	}

	TestTrue(*FString::Printf(
			TEXT("GATE-TRANSIENT: the kill receipts precede the income receipt (%s)"),
			*DescribeReceipts(Answer)),
		LastKillIndex < IncomeIndex);
	TestTrue(*FString::Printf(
			TEXT("GATE-TRANSIENT: and the repair receipts follow it (%s)"),
			*DescribeReceipts(Answer)),
		IncomeIndex < FirstRepairIdx);

	if (TestEqual(TEXT("GATE-TRANSIENT: both repair rows are emitted"),
			RepairUnitIdsInOrder.Num(), 2))
	{
		TestEqual(TEXT("GATE-TRANSIENT: the repair rows keep the order they arrived in (first)"),
			RepairUnitIdsInOrder[0], TurnRepairs[0].UnitId);
		TestEqual(TEXT("GATE-TRANSIENT: the repair rows keep the order they arrived in (second)"),
			RepairUnitIdsInOrder[1], TurnRepairs[1].UnitId);
	}

	return true;
}

// ---------------------------------------------------------------------------
// THE SHORT FORM MEANS "NOTHING WAS REPAIRED", AND AN UNSEEDED MARK SILENCES THIS ARM TOO.
//
// TWO PROPERTIES OF THE SAME SHAPE, AND EACH IS A RULE THE OTHER CLAUSES CANNOT SEE.
//
// THE OVERLOAD. `StratDecideTransientReceipts.h` states the three-argument form exists so
// that "no repairs" is spellable without an empty array at every call site, and that it is an
// overload rather than a default argument precisely so a caller that SHOULD pass repairs
// cannot silently pass none. The property to pin is that it decides nothing of its own: the
// same mark and the same model through both forms must differ in exactly the repair arm and
// nowhere else. A short form that had drifted into a second implementation of the five rules
// would show up here as a difference in the OTHER arms, which is the failure the forwarding
// exists to make impossible.
//
// THE UNSEEDED MARK. Rule 1 -- an unseeded mark emits no receipts at all -- is the
// save-resume rule, and the repair arm is the newest thing that could have escaped it: it
// reads no durable home and no mark row, so a decider that gated it on the LIST being
// non-empty rather than on the turn edge would emit a full set of repair floaters on the first
// refresh of a resumed match. The fixture hands a non-empty list to a default-constructed
// mark, which is exactly that state.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRepairShortFormAndUnseededTest,
	"Stratocracy.StratUI.GATE-TRANSIENT.TheShortFormAndAnUnseededMarkBothEmitNoRepair",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRepairShortFormAndUnseededTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRepairReceiptClauses;

	FTurnEdge Edge;
	FString   Error;
	if (!TestTrue(TEXT("the bridge seeds, ends a turn and the model rebuilds"),
			Edge.Build(Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the (Turn, SideToMove) pair moved, and the rules moved it"),
			Edge.EdgeIsReal()))
	{
		return false;
	}
	if (!TestTrue(TEXT("the projection carries a unit and a side"),
			Edge.Model.Units.Num() > 0 && Edge.Model.Sides.Num() > 0))
	{
		return false;
	}

	const FStratUnitView& Unit = Edge.Model.Units[0];

	FStratUnitRepairView Repair;
	Repair.UnitId   = Unit.UnitId;
	Repair.Side     = Unit.Side;
	Repair.HpBefore = Unit.Hp;
	Repair.HpAfter  = Unit.HpMax;
	Repair.HpMax    = Unit.HpMax;
	Repair.Amount   = Edge.Model.Sides[0].IncomePerTurn;
	if (!TestTrue(TEXT("the planted amount is nonzero"), Repair.Amount != 0))
	{
		return false;
	}

	TArray<FStratUnitRepairView> TurnRepairs;
	TurnRepairs.Add(Repair);

	// ---- the two forms, side by side --------------------------------------------------
	FStratTransientReceipts Long;
	StratDecideTransientReceipts(Edge.Mark, Edge.Model, TurnRepairs, Long);

	FStratTransientReceipts Short;
	StratDecideTransientReceipts(Edge.Mark, Edge.Model, Short);

	if (!TestEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: the long form with one row fires one repair receipt (%s)"),
			*DescribeReceipts(Long)),
			CountOfKind(Long, EStratReceiptKind::UnitRepair), 1))
	{
		return false;
	}
	TestEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: the short form fires none (%s)"), *DescribeReceipts(Short)),
		CountOfKind(Short, EStratReceiptKind::UnitRepair), 0);

	// AND DIFFERS IN NOTHING ELSE. The short form forwards; it decides nothing of its own.
	TestEqual(TEXT("GATE-TRANSIENT: the two forms agree on the income arm"),
		CountOfKind(Short, EStratReceiptKind::IncomeRate),
		CountOfKind(Long, EStratReceiptKind::IncomeRate));
	TestEqual(TEXT("GATE-TRANSIENT: and on the kill arm"),
		CountOfKind(Short, EStratReceiptKind::KillFame),
		CountOfKind(Long, EStratReceiptKind::KillFame));
	TestEqual(TEXT("GATE-TRANSIENT: and on the banner"),
		static_cast<int32>(Short.Banner), static_cast<int32>(Long.Banner));
	TestEqual(TEXT("GATE-TRANSIENT: and on BannerSide"), Short.BannerSide, Long.BannerSide);
	TestEqual(TEXT("GATE-TRANSIENT: and on whether the banner changed"),
		Short.bBannerJustChanged, Long.bBannerJustChanged);
	TestEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: so the two answers differ by exactly the repair arm (long %s, "
			     "short %s)"),
			*DescribeReceipts(Long), *DescribeReceipts(Short)),
		Short.Receipts.Num(), Long.Receipts.Num() - 1);

	// ---- rule 1 reaches this arm too --------------------------------------------------
	const FStratReceiptMark Unseeded;
	if (!TestFalse(TEXT("a default mark reports itself unseeded"), Unseeded.bSeeded))
	{
		return false;
	}

	FStratTransientReceipts OnAResume;
	StratDecideTransientReceipts(Unseeded, Edge.Model, TurnRepairs, OnAResume);

	TestEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: an unseeded mark emits no receipts at all, a non-empty repair "
			     "record notwithstanding (%s)"),
			*DescribeReceipts(OnAResume)),
		OnAResume.Receipts.Num(), 0);

	return true;
}
