// The world-free half of GATE-TRANSIENT -- GDD §2.11.2's income and kill receipts and the
// YOUR TURN / ENEMY TURN banner. The local ID is authorized for W8 items (5) and (6) by the
// 2026-08-29 ruling in `Tools/architect/state/global.md`.
//
// WHY THE ID IS `GATE-TRANSIENT` AND NOT `T-FAME-02`, `T-FAME-07` OR `T-TURN-01`, stated
// here because a clause that borrows a stronger acceptance ID than it earns is a false
// clearance of that ID. Nothing below asserts that an AWARD was computed correctly -- the
// award is `strat::killAward`'s and the rules module's gates own it. What a receipt asserts
// is narrower and entirely presentational: that an EDGE in a durable home was noticed, and
// that the number carried across is that home's own reading with nothing done to it. The
// banner is the same shape: it asserts a viewer-relative mapping of `SideToMove`, and says
// nothing whatever about strict alternation, which is a rules property the banner merely
// mirrors. NO NEW ACCEPTANCE ID IS MINTED HERE -- a proposed identifier greps exactly like a
// defined one, and this project has already been bitten by that.
//
// WHERE EVERY EXPECTATION COMES FROM, which is the whole discipline of this directory.
//
//   - The BASE FIXTURE OF EVERY CLAUSE IS A REAL PROJECTION. Each clause seeds an
//     `FStratBridge` from the shipped tables and the shipped scenario, takes a
//     `strat::UiSnapshot` off it, and builds an `FStratViewModel` from the SAME bridge.
//     No side count, no income figure, no fame figure and no turn number is written in this
//     file.
//
//   - THE INCOME EXPECTATION IS THE RULES MODULE'S OWN FIELD, one layer BELOW the view
//     model: `strat::UiSideView::incomePerTurn`, read off the snapshot the same bridge
//     projected. So `IncomeReceiptIsTheRateVerbatim` would survive the view model being
//     rewritten and would fail if the receipt drifted from the rules value by any amount.
//
//   - THE TURN EDGE THAT FIRES THAT RECEIPT IS A REAL ONE. `FStratBridge::SubmitEndTurn` is
//     submitted and the second model is rebuilt from the advanced state, so the (Turn,
//     SideToMove) pair moves because the RULES MOVED IT. The clause asserts the pair
//     actually changed before it asserts anything about the receipt.
//
//   - THE KILL AMOUNT IS THE ONE PLACE A SUBTRACTION APPEARS ON THE EXPECTATION SIDE, AND
//     IT IS STATED RATHER THAN QUIET. There is no published delta anywhere in this tree:
//     `accrueIncome`'s return is published nowhere, `FStratCombatOutcome` is not retained by
//     the bridge (its delivery is deferred by name to the `bridge_event_list` ruling), and
//     no `UiSideView` field carries "fame gained since". So the expectation is the
//     DIFFERENCE OF TWO MODULE-SIDE READS -- `FStratReceiptMark::FameCombat[Side]`, written
//     by `StratMarkFromView`, and `FStratSideView::FameCombat` on the applied model -- and
//     each of those two operands is separately asserted against its own source first. A
//     clause that only checked `Amount == DurableAfter - DurableBefore` would be checking
//     the struct against itself; the two mirrors are pinned to the mark and the model before
//     the subtraction is ever named.
//
//   - THE FAME RISE ITSELF IS PLANTED, and the plant is fixture construction on
//     `StratMatchReconcile.cpp`'s precedent (that file edits a real model's unit list and
//     hexes and says so). It is planted rather than played because the only mid-match
//     position this tree can reach deterministically is `Data/parity_fixture.save`, which
//     needs `strat::parseSave` -- a vendored free function, LNK2019 outside StratBridge and
//     StratRules, measured 8x. EVERY PLANTED VALUE IS A MODULE FIELD, never a literal.
//
//     A CLAIM STOOD HERE UNTIL THE MUTANT PASS, NAMING THE FIELD AND WHAT IT BOUGHT:
//
//       STRUCK> "The plant is chosen to be a MODULE VALUE, not a literal:
//       STRUCK>  `FStratSideView::FameTotal`, which is nonzero at the seeded opening while
//       STRUCK>  `FameCombat` is zero. That choice buys a second property for free -- a
//       STRUCK>  decider that read `FameTotal` instead of `FameCombat` would see no rise at
//       STRUCK>  all and the clause would go red on an empty list rather than silently
//       STRUCK>  agreeing."
//
//     WITHDRAWN IN FULL, ON BOTH HALVES. The naming half stopped being true of the kill
//     fixture when that fixture was rebuilt: it plants `IncomePerTurn` as the `Before`
//     reading now, because a `Before` of zero made its amount assertion inert. The mechanism
//     half was never measured and is not what a wrong-field decider does; the measurement is
//     at `KillReceiptCarriesTheFameCombatRise`.
//
// NOTHING BELOW COMPARES TWO `FString`s. `==`, `Contains` and `TestEqual` on `FString` are
// all case-insensitive in this engine, and that has already produced a clause in this
// project that could not fail. Every value asserted here is an integer or an enumerator, and
// enumerators are compared through `static_cast<int32>` so a failure prints the ordinal.
//
// NO `strat::` FREE FUNCTION IS CALLED ANYWHERE BELOW. Naming `strat::UiSnapshot` costs no
// symbol; calling a vendored free function is LNK2019 outside StratBridge and StratRules.
// Every rules-side value here arrives through a method on `FStratBridge`.
//
// NOT ASSERTED HERE, with reasons:
//   - THAT `ApplyView` CALLS ANY OF THIS, and that it re-marks afterwards, and that both
//     teardown paths reset the mark. Those need a live subsystem and are in
//     `Source/StratPlay/Tests/StratTransientReceiptCallSite.cpp` -- the module that can see
//     `UStratMatchSubsystem`. Asserting them here would be `LNK2019`, not a test.
//   - PER-KILL GRANULARITY, VICTIM IDENTITY, TOAST DURATION, STACKING, ANIMATION AND COPY.
//     `StratTransientReceipts.h` states that this file's subject carries none of them and
//     why. A clause over a property the code deliberately does not have would be asserting
//     the absence as a requirement, which goes red the day the `bridge_event_list` ruling
//     lands and the kill arm grows a `VictimUnitId` -- exactly the failure mode the
//     2026-08-31 ordering-clause refusal was about.
//   - WHETHER `incomePerTurn` IS THE CORRECT §2.7 RATE OVER THAT SIDE'S HOLDINGS. That is
//     the projection's arithmetic and T-UI-05's subject. Recomputing it here would give this
//     gate a second opinion about a number it is only supposed to be checking the TRANSPORT
//     of.
//   - PIXELS. Every decider under test is a free function over plain reflected values, on
//     `AStratShellHUD::DecideMenuTiming`'s precedent; nothing below constructs a widget or a
//     world.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratTransientReceipts.h"
#include "StratViewModel.h"

// The bridge, included from a .cpp and never from a UHT-parsed header -- the same
// constraint `StratViewModel.h` records about itself. A test file is a .cpp, so it may
// say this.
#include "StratBridge.h"

// The vendored view-model header, by short name via StratRules' PublicIncludePaths. Named
// explicitly because this file reads `strat::UiSideView` fields directly and a reader
// should not have to discover that through a transitive include.
#include "Ui.h"

namespace StratTransientReceiptClauses
{
	// MUST match the side the rest of the engine-side harness seeds with. `Replay.h` states
	// that no rule in this project decides which side moves first, so this is a harness pin
	// and not a rule: it exists so the projected opening is the same one every run.
	// `StratScoreboardParity.cpp`, `StratViewModelParity.cpp` and `StratBridgeParity.cpp`
	// pin the same value for the same stated reason.
	static const int32 kFirstSide = 0;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/**
	 * Brings a bridge to the seeded opening: the shipped tables mapped in, the shipped
	 * scenario seeded through the rules module's own loader.
	 *
	 * Returns false and fills OutError rather than asserting, on `StratViewModelParity`'s
	 * precedent: a missing asset becomes a named failure inside the caller's own clause list
	 * instead of a crash in a helper.
	 *
	 * NOTHING IS HARDCODED PAST THE ASSET PATHS -- automation fixtures are one of the two
	 * places `.agents/ue-project-context.md` licenses a `/Game/` literal.
	 */
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
	 * A seeded bridge, its snapshot, and the view model built from it for `ViewingSide`.
	 *
	 * TWO PROJECTIONS OF ONE STATE, safe here for `StratViewModelParity::Project`'s stated
	 * reason rather than by hope: no command is submitted between the two calls, so the
	 * authoritative `GameState` is identical across both.
	 */
	static bool Project(FStratBridge&      Bridge,
	                    int32              ViewingSide,
	                    strat::UiSnapshot& OutSnapshot,
	                    FStratViewModel&   OutModel,
	                    FString&           OutError)
	{
		const FStratResult Projected = Bridge.MakeUiSnapshot(OutSnapshot);
		if (!Projected.bOk)
		{
			OutError = FString::Printf(TEXT("MakeUiSnapshot refused: %s"), *Projected.Reason);
			return false;
		}
		return StratBuildViewModel(Bridge, ViewingSide, OutModel, OutError);
	}

	/** Seed and project in one step, for the clauses that need only the opening. */
	static bool SeedAndProject(FStratBridge&      Bridge,
	                           int32              ViewingSide,
	                           strat::UiSnapshot& OutSnapshot,
	                           FStratViewModel&   OutModel,
	                           FString&           OutError)
	{
		return SeedBridge(Bridge, OutError)
		    && Project(Bridge, ViewingSide, OutSnapshot, OutModel, OutError);
	}

	/** How many receipts of one kind the answer carries. */
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

	/** The first receipt of one kind, or null. */
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
		TArray<FString> Parts;
		for (const FStratReceiptView& Receipt : Receipts.Receipts)
		{
			Parts.Add(FString::Printf(TEXT("{kind=%d side=%d turn=%d amount=%d before=%d after=%d}"),
				static_cast<int32>(Receipt.Kind), Receipt.Side, Receipt.Turn,
				Receipt.Amount, Receipt.DurableBefore, Receipt.DurableAfter));
		}
		return Parts.Num() == 0 ? FString(TEXT("<empty>")) : FString::Join(Parts, TEXT(", "));
	}
}

// ---------------------------------------------------------------------------
// AN UNSEEDED MARK EMITS NOTHING -- AND STILL BANNERS.
//
// THIS IS THE SAVE-RESUME CLAUSE AND IT IS THE SHARPEST ONE IN THE FILE. Without the
// `bSeeded` rule, the first model a session ever applies has every durable home reading as
// having risen from zero, so a match resumed from a save opens with a kill toast for the
// whole war's accumulated combat Fame and an income toast for a turn that did not begin
// here. The fixture makes that concrete: the mark is left default (never written by
// `StratMarkFromView`) while the model carries a REAL nonzero fame reading, so a decider
// that inferred "unseeded" from the contents rather than from the flag would fire.
//
// THE SECOND HALF IS WHY THE CLAUSE IS NOT NAMED FOR SILENCE ALONE. `StratTransientReceipts.h`'s
// rule 1 says the banner is filled on EVERY call, seeded mark or not -- it is steady state
// and not an edge -- and `bBannerJustChanged` is false because "changed" is a relation
// between two readings and only one exists. A decider that took the early return before
// deciding the banner would leave a resumed match with no banner at all, and a clause that
// only asserted the empty list would be green over it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratTransientUnseededMarkTest,
	"Stratocracy.StratUI.GATE-TRANSIENT.UnseededMarkEmitsNoReceiptsAndStillBanners",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratTransientUnseededMarkTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratTransientReceiptClauses;

	FStratBridge      Bridge;
	strat::UiSnapshot Snapshot;
	FStratViewModel   Model;
	FString           Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, /*ViewingSide*/ 0, Snapshot, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	// THE MODEL IS MADE TO LOOK LIKE A RESUMED MATCH, which is what makes the flag do work.
	// A side that has accumulated combat Fame is exactly the state a save carries, and the
	// planted value is the model's OWN `FameTotal` -- a module number, not a literal.
	if (!TestTrue(TEXT("the scenario projects at least one side"), Model.Sides.Num() > 0))
	{
		return false;
	}
	if (!TestTrue(TEXT("the opening's FameTotal is a usable nonzero plant"),
			Model.Sides[0].FameTotal > 0))
	{
		return false;
	}
	Model.Sides[0].FameCombat = Model.Sides[0].FameTotal;

	// DEFAULT-CONSTRUCTED AND NEVER HANDED TO `StratMarkFromView`. That is the whole of the
	// precondition: the flag records THE WRITE, not the contents.
	const FStratReceiptMark Unseeded;
	TestFalse(TEXT("GATE-TRANSIENT: a default mark reports itself unseeded"), Unseeded.bSeeded);

	FStratTransientReceipts Answer;
	StratDecideTransientReceipts(Unseeded, Model, Answer);

	TestEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: an unseeded mark emits no receipts, whatever the model reads: %s"),
			*DescribeReceipts(Answer)),
		Answer.Receipts.Num(), 0);

	TestFalse(TEXT("GATE-TRANSIENT: and reports no banner CHANGE, there being only one reading"),
		Answer.bBannerJustChanged);

	// THE BANNER IS STILL DECIDED. Expectation from the model's own two fields, through the
	// decider that owns the mapping -- not re-spelled here.
	TestEqual(TEXT("GATE-TRANSIENT: the banner is filled even with no mark"),
		static_cast<int32>(Answer.Banner),
		static_cast<int32>(StratDecideTurnBanner(Model.Match.SideToMove,
		                                         Model.ViewingSide,
		                                         Model.Match.bHasResult)));
	TestNotEqual(TEXT("GATE-TRANSIENT: and it is a real banner, not the None arm"),
		static_cast<int32>(Answer.Banner), static_cast<int32>(EStratTurnBanner::None));
	TestEqual(TEXT("GATE-TRANSIENT: BannerSide names the side it was resolved against"),
		Answer.BannerSide, Model.Match.SideToMove);

	return true;
}

// ---------------------------------------------------------------------------
// THE KILL RECEIPT CARRIES THE RISE IN `FStratSideView::FameCombat`.
//
// WHERE THE THREE NUMBERS COME FROM. `DurableAfter` is asserted against the applied model's
// own field and `DurableBefore` against the mark row the receipt was decided from. Only then
// is `Amount` named, as the difference of those two -- the one place in this file a
// subtraction appears on the expectation side, for the reason the header block gives. The
// mark's own parity with the rules module is asserted FIRST and over EVERY side, against
// `strat::UiSideView::fameCombat` on the snapshot, so a mark that copied the wrong field or
// keyed by the wrong index fails there rather than surfacing as a wrong amount later.
//
// TWO ACCOUNTS OF THIS FIXTURE STOOD IN THIS BLOCK AND BOTH WERE WRONG. They are kept rather
// than edited, because what they record is the defect the mutant pass caught here, and that
// is worth more to a later reader than a tidy block.
//
//   STRUCK> "`DurableBefore` is asserted against `FStratReceiptMark::FameCombat[Side]`,
//   STRUCK>  which `StratMarkFromView` wrote off the real projection."
//
//   STRUCK> "THE PLANT IS `FameTotal`, WHICH BUYS A SECOND PROPERTY. At the seeded opening
//   STRUCK>  `FameTotal` is the starting Fame and `FameCombat` is zero, so assigning one to
//   STRUCK>  the other is a genuine rise AND makes the two fields distinguishable: a decider
//   STRUCK>  that watched `FameTotal` would see before and after both equal and emit
//   STRUCK>  nothing, and this clause would go red on an empty list rather than silently
//   STRUCK>  agreeing."
//
// THE FIRST IS WITHDRAWN BECAUSE THE FIXTURE MOVED UNDER IT. `StratMarkFromView` writes the
// mark and the risen side's row is then OVERWRITTEN by the plant, so for THAT side the
// asserted value is the plant and not the projection. It remains true of every other side --
// which is what the per-side parity loop above actually pins, and why that loop is the thing
// to read for the mark's fidelity.
//
// THE SECOND IS WITHDRAWN BECAUSE ITS MECHANISM WAS NEVER MEASURED AND IS NOT WHAT HAPPENS.
// It also describes the PRE-FIX fixture, whose `Before` was zero -- the inertness recorded at
// the plant below. Both wrong-field deciders were then built in place on this tree, against
// the DELIVERED fixture, and neither behaves as the paragraph says:
//
//   - COMPARING `FameTotal` WHILE THE MARK STILL WRITES `FameCombat` does not go quiet.
//     Side 0 holds, its mark row being the plant and its `FameTotal` having been pushed down
//     to the same number -- but SIDE 1, whose mark row is untouched at 0, rises and fires.
//     The clause reddens on `Side` (1, expected 0), on `DurableBefore` (0, expected 100), on
//     `Amount` (200, expected 100) and on the guard that `Amount` is not the reading itself.
//     The list is never empty.
//   - SWAPPING THE FIELD IN BOTH THE MARK AND THE COMPARISON reddens the per-side parity loop
//     first -- `mark.FameCombat[0]` reads 200 where the snapshot says 0 -- and only after that
//     on an empty list. So the empty list is real in this one variant, and it is not the
//     sharpest thing that fails; the parity loop is.
//
// EXACTLY ONE RECEIPT, WHICH PINS THE PER-SIDE INDEXING. Only side 0's fame is moved, so a
// decider that compared side 0's mark against side 1's model reading -- arithmetic between
// two different players, which `StratMarkFromView`'s own comment names as the hazard -- would
// produce the wrong count or the wrong `Side`.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratTransientKillRiseTest,
	"Stratocracy.StratUI.GATE-TRANSIENT.KillReceiptCarriesTheFameCombatRise",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratTransientKillRiseTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratTransientReceiptClauses;

	FStratBridge      Bridge;
	strat::UiSnapshot Snapshot;
	FStratViewModel   Model;
	FString           Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, /*ViewingSide*/ 0, Snapshot, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the scenario projects at least two sides"), Model.Sides.Num() >= 2))
	{
		return false;
	}

	const FStratReceiptMark Mark = StratMarkFromView(Model);

	// ---- the mark's own parity with the rules module, before anything is derived from it --
	TestTrue(TEXT("GATE-TRANSIENT: StratMarkFromView reports the mark seeded"), Mark.bSeeded);
	TestEqual(TEXT("GATE-TRANSIENT: the mark carries one fame reading per projected side"),
		Mark.FameCombat.Num(), Model.Sides.Num());
	for (int32 Side = 0; Side < Model.Sides.Num(); ++Side)
	{
		TestEqual(*FString::Printf(
				TEXT("GATE-TRANSIENT: mark.FameCombat[%d] == strat::UiSideView::fameCombat"), Side),
			Mark.FameCombat[Side], static_cast<int32>(Snapshot.side[Side].fameCombat));
	}

	// ---- the plant: a rise on side 0 only, from two module fields --------------------
	//
	// THE `Before` READING IS PLANTED NONZERO AND THAT IS THE WHOLE REASON THIS CLAUSE CAN
	// FAIL. MEASURED: with the mark left at the opening's `FameCombat` of ZERO, `After -
	// Before` and `After` are the SAME NUMBER, and a mutant carrying the absolute reading
	// instead of the rise ran the full suite GREEN. That is the identical coincidence the
	// income clause's fixture is built to avoid, walked into on the other arm. So the mark's
	// reading is raised to the side's standing income rate -- a module value, nonzero -- and
	// the two candidate expressions are asserted to disagree before the amount is checked.
	//
	// AND `FameTotal` IS PUSHED DOWN TO THAT SAME RATE, so the two fields hold different
	// numbers on the risen side and no decider can read one for the other unnoticed. WHAT SUCH
	// A DECIDER ACTUALLY DOES IS MEASURED RATHER THAN REASONED, in the STRUCK> block above: a
	// sentence written HERE, in the same pass that fixed the inertness, claimed it "sees no
	// rise at all on this fixture and the clause goes red on an empty list", and that was
	// wrong too. It is struck up there beside the two measurements that replace it.
	const int32 kRisenSide = 0;
	const int32 PlantedBefore = Model.Sides[kRisenSide].IncomePerTurn;
	const int32 PlantedAfter  = Model.Sides[kRisenSide].FameTotal;
	if (!TestTrue(TEXT("Ferrum Crossing's opening gives a nonzero standing rate below the "
	                   "starting Fame, without which this fixture could not separate 'the rise' "
	                   "from 'the reading'"),
			PlantedBefore > 0 && PlantedAfter > PlantedBefore))
	{
		return false;
	}

	FStratReceiptMark Planted = Mark;
	Planted.FameCombat[kRisenSide] = PlantedBefore;

	FStratViewModel Later = Model;
	Later.Sides[kRisenSide].FameCombat = PlantedAfter;
	Later.Sides[kRisenSide].FameTotal  = PlantedBefore;

	if (!TestTrue(TEXT("the fixture makes 'the rise in FameCombat' and 'the reading of "
	                   "FameCombat' different numbers"),
			(PlantedAfter - PlantedBefore) != PlantedAfter))
	{
		return false;
	}

	FStratTransientReceipts Answer;
	StratDecideTransientReceipts(Planted, Later, Answer);

	if (!TestEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: one side's fame rose, so exactly one receipt fired: %s"),
			*DescribeReceipts(Answer)),
			Answer.Receipts.Num(), 1))
	{
		return false;
	}

	const FStratReceiptView& Receipt = Answer.Receipts[0];

	TestEqual(TEXT("GATE-TRANSIENT: it is a KillFame receipt"),
		static_cast<int32>(Receipt.Kind), static_cast<int32>(EStratReceiptKind::KillFame));
	TestEqual(TEXT("GATE-TRANSIENT: it names the side whose fame moved"),
		Receipt.Side, kRisenSide);
	TestEqual(TEXT("GATE-TRANSIENT: it is placed on the applied model's turn"),
		Receipt.Turn, Later.Match.Turn);

	// ---- the two mirrors, each against its own source -------------------------------
	TestEqual(TEXT("GATE-TRANSIENT: DurableBefore is the mark's reading of FameCombat"),
		Receipt.DurableBefore, Planted.FameCombat[kRisenSide]);
	TestEqual(TEXT("GATE-TRANSIENT: DurableAfter is the applied model's reading of FameCombat"),
		Receipt.DurableAfter, Later.Sides[kRisenSide].FameCombat);

	// ---- and only then the amount ---------------------------------------------------
	// THE ONE SUBTRACTION ON AN EXPECTATION SIDE IN THIS FILE. No module-side value
	// publishes this delta -- see the header block for the three places checked.
	TestEqual(TEXT("GATE-TRANSIENT: Amount is the rise between those two module-side readings"),
		Receipt.Amount,
		Later.Sides[kRisenSide].FameCombat - Planted.FameCombat[kRisenSide]);
	TestNotEqual(TEXT("GATE-TRANSIENT: and so is NOT the reading itself"),
		Receipt.Amount, Receipt.DurableAfter);
	TestTrue(TEXT("GATE-TRANSIENT: and a kill receipt is strictly positive by construction"),
		Receipt.Amount > 0);

	return true;
}

// ---------------------------------------------------------------------------
// A FALL OR A HOLD IN `FameCombat` IS SILENT.
//
// TWO CASES, AND THE HOLD IS THE ONE THAT DOES THE MOST WORK. The hold case applies a model
// against a mark taken from that same model -- the overwhelmingly common refresh, where
// nothing moved -- and requires an empty list. A decider using `!=` instead of `>` would
// pass the hold and fail the fall; one using `>=` would fail the hold and pass the fall.
// Both are asserted, so neither slip survives.
//
// THE FALL IS NOT A FAULT, AND THIS CLAUSE PINS THE REQUIREMENT RATHER THAN THE HAZARD.
// `FameCombat` only accrues, so a fall means the mark and the model describe different
// matches -- a reseed or a load -- and the caller resets the mark on both of those paths.
// The requirement is SILENCE, which stays true on the day anything about kills changes; the
// clause deliberately does not assert an error, a warning or a log line, because a decider
// that cannot see WHICH of the two happened has no diagnosis to offer and one added later
// would redden this clause for being repaired.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratTransientFameFallSilentTest,
	"Stratocracy.StratUI.GATE-TRANSIENT.FameCombatFallOrHoldIsSilent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratTransientFameFallSilentTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratTransientReceiptClauses;

	FStratBridge      Bridge;
	strat::UiSnapshot Snapshot;
	FStratViewModel   Model;
	FString           Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, /*ViewingSide*/ 0, Snapshot, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the scenario projects at least one side"), Model.Sides.Num() > 0))
	{
		return false;
	}

	// ---- the hold: the model compared against a mark taken from itself ---------------
	{
		const FStratReceiptMark  Mark = StratMarkFromView(Model);
		FStratTransientReceipts  Answer;
		StratDecideTransientReceipts(Mark, Model, Answer);

		TestEqual(*FString::Printf(
				TEXT("GATE-TRANSIENT: nothing moved, so nothing fired: %s"),
				*DescribeReceipts(Answer)),
			Answer.Receipts.Num(), 0);
		TestFalse(TEXT("GATE-TRANSIENT: and the banner did not change against itself"),
			Answer.bBannerJustChanged);
	}

	// ---- the fall: a mark reading HIGHER than the model -------------------------------
	// Planted on the mark rather than on the model, because `FameCombat` is zero at the
	// opening and there is nothing below it to fall to. The planted value is the model's own
	// `FameTotal`.
	//
	//   STRUCK> ", as in the rise clause."
	//
	// Withdrawn: the rise clause stopped planting `FameTotal` as its `Before` reading when
	// it was rebuilt to escape the inertness its own mutant found. It plants `IncomePerTurn`
	// there now, so the two clauses no longer plant the same thing and a reader should not
	// carry one's reasoning across to the other.
	{
		if (!TestTrue(TEXT("the opening's FameTotal is a usable nonzero plant"),
				Model.Sides[0].FameTotal > Model.Sides[0].FameCombat))
		{
			return false;
		}

		FStratReceiptMark Mark = StratMarkFromView(Model);
		Mark.FameCombat[0] = Model.Sides[0].FameTotal;
		TestTrue(TEXT("the fixture really is a fall"),
			Mark.FameCombat[0] > Model.Sides[0].FameCombat);

		FStratTransientReceipts Answer;
		StratDecideTransientReceipts(Mark, Model, Answer);

		TestEqual(*FString::Printf(
				TEXT("GATE-TRANSIENT: a fall in FameCombat emits nothing: %s"),
				*DescribeReceipts(Answer)),
			Answer.Receipts.Num(), 0);
	}

	return true;
}

// ---------------------------------------------------------------------------
// A SIDE THE MARK NEVER SAW IS SKIPPED, AND THE SIDES IT DID SEE ARE NOT.
//
// RULE 2 AT PER-SIDE GRANULARITY. A side present in the model but absent from the mark has
// no previous reading, so there is no edge to report about it -- the same argument the
// `bSeeded` flag makes about the whole mark, applied to one row of it. The branch is a real
// one (`Mark.FameCombat.IsValidIndex`) and nothing else in this file executes it.
//
// THE FIXTURE MOVES BOTH SIDES AND TRUNCATES THE MARK TO ONE. So the clause is not merely
// "no crash": exactly one receipt must fire, for the side the mark HAS. A decider that
// skipped defensively on any bounds question -- or that read `Mark.FameCombat[Side]` out of
// range and got a garbage `Before` -- lands on a different count or a different side.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratTransientSideAbsentFromMarkTest,
	"Stratocracy.StratUI.GATE-TRANSIENT.SideAbsentFromTheMarkIsSkipped",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratTransientSideAbsentFromMarkTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratTransientReceiptClauses;

	FStratBridge      Bridge;
	strat::UiSnapshot Snapshot;
	FStratViewModel   Model;
	FString           Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, /*ViewingSide*/ 0, Snapshot, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the scenario projects at least two sides, so one can be dropped"),
			Model.Sides.Num() >= 2))
	{
		return false;
	}

	FStratReceiptMark Mark = StratMarkFromView(Model);

	// The mark keeps side 0 and forgets everything after it.
	Mark.FameCombat.SetNum(1);

	FStratViewModel Later = Model;
	for (int32 Side = 0; Side < Later.Sides.Num(); ++Side)
	{
		if (!TestTrue(TEXT("every side's FameTotal is a usable plant"),
				Model.Sides[Side].FameTotal > Model.Sides[Side].FameCombat))
		{
			return false;
		}
		Later.Sides[Side].FameCombat = Model.Sides[Side].FameTotal;
	}

	FStratTransientReceipts Answer;
	StratDecideTransientReceipts(Mark, Later, Answer);

	if (!TestEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: both sides rose but only one had a previous reading: %s"),
			*DescribeReceipts(Answer)),
			Answer.Receipts.Num(), 1))
	{
		return false;
	}
	TestEqual(TEXT("GATE-TRANSIENT: and it is the side the mark carries"),
		Answer.Receipts[0].Side, 0);
	TestEqual(TEXT("GATE-TRANSIENT: whose DurableBefore is that mark row"),
		Answer.Receipts[0].DurableBefore, Mark.FameCombat[0]);

	return true;
}

// ---------------------------------------------------------------------------
// THE INCOME RECEIPT IS THE STANDING RATE, VERBATIM -- NEVER A DELTA.
//
// THIS IS THE CLAUSE THAT CATCHES THE INCOME ARM SILENTLY BECOMING A SUBTRACTION, and the
// fixture is built so the two candidate expressions cannot agree. `Ui.h` is explicit that
// `incomePerTurn` "is the STANDING rate" and is "deliberately NOT `accrueIncome`'s return
// value", and `accrueIncome`'s return is published nowhere in this tree -- so the rate is
// the only thing there is to carry, and carrying a difference of two readings of it would
// be a number no module produced.
//
// THE EDGE IS REAL AND THE EXPECTATION IS THE RULES MODULE'S OWN FIELD. `SubmitEndTurn` is
// submitted through the bridge and the second model is rebuilt from the advanced state, so
// the (Turn, SideToMove) pair moves because the rules moved it -- asserted before anything
// else. The expected amount is `strat::UiSideView::incomePerTurn` off the snapshot the same
// bridge projected AFTER the advance, which is one layer below `FStratSideView` and is the
// authority the view model itself is checked against elsewhere.
//
// HOW THE TWO EXPRESSIONS ARE MADE TO DISAGREE. The mark's rate for the side now to move is
// planted EQUAL to the model's, so `After - Before` is zero while the rate is not. The
// clause asserts that inequality as a fixture precondition before it asserts the amount,
// because with a zero `Before` -- the natural default -- the delta and the rate coincide
// exactly and the clause would be inert while looking green. That coincidence is the trap
// this fixture exists to avoid.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratTransientIncomeVerbatimTest,
	"Stratocracy.StratUI.GATE-TRANSIENT.IncomeReceiptIsTheRateVerbatim",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratTransientIncomeVerbatimTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratTransientReceiptClauses;

	FStratBridge      Bridge;
	strat::UiSnapshot Opening;
	FStratViewModel   Before;
	FString           Error;
	if (!TestTrue(TEXT("the bridge seeds and the opening model builds"),
			SeedAndProject(Bridge, /*ViewingSide*/ 0, Opening, Before, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratReceiptMark Mark = StratMarkFromView(Before);

	// ---- a REAL turn edge, produced by the rules module -------------------------------
	const FStratResult Ended = Bridge.SubmitEndTurn();
	if (!TestTrue(TEXT("the rules module accepts an end of turn at the opening"), Ended.bOk))
	{
		AddError(Ended.Reason);
		return false;
	}

	strat::UiSnapshot Advanced;
	FStratViewModel   After;
	if (!TestTrue(TEXT("the model rebuilds from the advanced state"),
			Project(Bridge, /*ViewingSide*/ 0, Advanced, After, Error)))
	{
		AddError(Error);
		return false;
	}

	if (!TestTrue(TEXT("the (Turn, SideToMove) pair actually moved, and the rules moved it"),
			After.Match.Turn != Before.Match.Turn
			|| After.Match.SideToMove != Before.Match.SideToMove))
	{
		return false;
	}

	const int32 Side = After.Match.SideToMove;
	if (!TestTrue(TEXT("the side now to move has a projected side view"),
			After.Sides.IsValidIndex(Side) && Mark.IncomePerTurn.IsValidIndex(Side)))
	{
		return false;
	}

	// The expectation, from the rules module's own field.
	const int32 RulesRate = static_cast<int32>(Advanced.side[Side].incomePerTurn);
	if (!TestTrue(TEXT("Ferrum Crossing pays the side to move a nonzero standing rate, without "
	                   "which the delta and the rate would coincide and this clause could not fail"),
			RulesRate != 0))
	{
		return false;
	}

	// ---- the plant that makes the two expressions disagree ----------------------------
	Mark.IncomePerTurn[Side] = RulesRate;
	if (!TestTrue(TEXT("the fixture makes 'the rate' and 'the rise in the rate' different numbers"),
			(RulesRate - Mark.IncomePerTurn[Side]) != RulesRate))
	{
		return false;
	}

	FStratTransientReceipts Answer;
	StratDecideTransientReceipts(Mark, After, Answer);

	TestEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: one turn edge and no fame movement fires exactly one receipt: %s"),
			*DescribeReceipts(Answer)),
		Answer.Receipts.Num(), 1);

	const FStratReceiptView* const Receipt = FirstOfKind(Answer, EStratReceiptKind::IncomeRate);
	if (!TestNotNull(*FString::Printf(TEXT("GATE-TRANSIENT: an IncomeRate receipt fired: %s"),
			*DescribeReceipts(Answer)), Receipt))
	{
		return false;
	}

	TestEqual(TEXT("GATE-TRANSIENT: it belongs to the side now to move"), Receipt->Side, Side);
	TestEqual(TEXT("GATE-TRANSIENT: it is placed on the turn that just began"),
		Receipt->Turn, After.Match.Turn);

	// THE CLAUSE. The amount is the rules module's standing rate with nothing done to it.
	TestEqual(TEXT("GATE-TRANSIENT: Amount == strat::UiSideView::incomePerTurn, verbatim"),
		Receipt->Amount, RulesRate);
	TestEqual(TEXT("GATE-TRANSIENT: DurableAfter is that same rate"),
		Receipt->DurableAfter, RulesRate);
	TestEqual(TEXT("GATE-TRANSIENT: DurableBefore is the mark's rate, present and NOT subtracted"),
		Receipt->DurableBefore, Mark.IncomePerTurn[Side]);
	TestNotEqual(TEXT("GATE-TRANSIENT: and Amount is therefore not the rise in the rate"),
		Receipt->Amount, Receipt->DurableAfter - Receipt->DurableBefore);

	// Q8(a)'S TURN-1 ZERO IS DELIBERATELY NOT TRANSCRIBED, and this asserts the refusal
	// rather than leaving it to prose: the receipt carries the standing rate whatever turn it
	// is, because suppressing it on turn 1 would put a rules fact in the presentation layer
	// where nothing rechecks it against §2.7.
	TestTrue(TEXT("GATE-TRANSIENT: the receipt is not suppressed on the game's first turn"),
		Receipt->Amount == RulesRate);

	return true;
}

// ---------------------------------------------------------------------------
// THE INCOME RECEIPT FIRES ON THE (Turn, SideToMove) PAIR, AND ON NEITHER HALF ALONE.
//
// THREE CASES, BECAUSE THE PAIR IS THE ONLY CORRECT PREDICATE AND EACH HALF ALONE IS WRONG
// IN A DIFFERENT SCENARIO. Hot-seat moves the SIDE within a turn, so a detector watching
// `Turn` alone stays silent for the whole second half of every turn; a one-sided scenario
// rolls the TURN over with the side unchanged, so a detector watching `SideToMove` alone
// stays silent forever. And with neither moving -- the common refresh -- nothing may fire,
// which is what stops a toast appearing on every mouse move.
//
// THE TWO MOVED FIXTURES ARE PLANTED ON A REAL MODEL and the plants are declared: the turn
// case advances `Turn` by one, and the side case sets `SideToMove` to another index the
// model actually projects. `IncomeReceiptIsTheRateVerbatim` is where the edge is a real
// rules-driven one; here the subject is the PREDICATE, and driving the rules could only
// produce the pair moving together, which is the case that cannot separate the two halves.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratTransientIncomePairEdgeTest,
	"Stratocracy.StratUI.GATE-TRANSIENT.IncomeFiresOnlyOnTheTurnSidePair",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratTransientIncomePairEdgeTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratTransientReceiptClauses;

	FStratBridge      Bridge;
	strat::UiSnapshot Snapshot;
	FStratViewModel   Model;
	FString           Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, /*ViewingSide*/ 0, Snapshot, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the scenario projects at least two sides"), Model.Sides.Num() >= 2))
	{
		return false;
	}

	const FStratReceiptMark Mark = StratMarkFromView(Model);

	// ---- neither half moved ----------------------------------------------------------
	{
		FStratTransientReceipts Answer;
		StratDecideTransientReceipts(Mark, Model, Answer);
		TestEqual(*FString::Printf(
				TEXT("GATE-TRANSIENT: the pair is unchanged, so no income receipt: %s"),
				*DescribeReceipts(Answer)),
			CountOfKind(Answer, EStratReceiptKind::IncomeRate), 0);
	}

	// ---- the turn rolled over, the side did not (the one-sided case) -----------------
	{
		FStratViewModel Later = Model;
		Later.Match.Turn = Model.Match.Turn + 1;

		FStratTransientReceipts Answer;
		StratDecideTransientReceipts(Mark, Later, Answer);
		TestEqual(*FString::Printf(
				TEXT("GATE-TRANSIENT: the turn moved with the side unchanged, so income fires: %s"),
				*DescribeReceipts(Answer)),
			CountOfKind(Answer, EStratReceiptKind::IncomeRate), 1);

		const FStratReceiptView* const Receipt = FirstOfKind(Answer, EStratReceiptKind::IncomeRate);
		if (Receipt != nullptr)
		{
			TestEqual(TEXT("GATE-TRANSIENT: for the side to move on the applied model"),
				Receipt->Side, Later.Match.SideToMove);
			TestEqual(TEXT("GATE-TRANSIENT: carrying that side's rate off the applied model"),
				Receipt->Amount, Later.Sides[Later.Match.SideToMove].IncomePerTurn);
		}
	}

	// ---- the side handed over within the turn (the hot-seat case) --------------------
	{
		FStratViewModel Later = Model;
		Later.Match.SideToMove = Model.Sides.Num() - 1;
		if (!TestTrue(TEXT("the fixture's other side really is a different index"),
				Later.Match.SideToMove != Model.Match.SideToMove))
		{
			return false;
		}
		TestEqual(TEXT("and the turn number is untouched"), Later.Match.Turn, Model.Match.Turn);

		FStratTransientReceipts Answer;
		StratDecideTransientReceipts(Mark, Later, Answer);
		TestEqual(*FString::Printf(
				TEXT("GATE-TRANSIENT: the side moved within the turn, so income fires: %s"),
				*DescribeReceipts(Answer)),
			CountOfKind(Answer, EStratReceiptKind::IncomeRate), 1);

		const FStratReceiptView* const Receipt = FirstOfKind(Answer, EStratReceiptKind::IncomeRate);
		if (Receipt != nullptr)
		{
			TestEqual(TEXT("GATE-TRANSIENT: for the side that just took over"),
				Receipt->Side, Later.Match.SideToMove);
			TestEqual(TEXT("GATE-TRANSIENT: carrying THAT side's rate and not the previous one's"),
				Receipt->Amount, Later.Sides[Later.Match.SideToMove].IncomePerTurn);
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// THE KILL RECEIPTS PRECEDE THE INCOME RECEIPT, AND THE KILLS ARE IN `Sides` ORDER.
//
// A CONTRACT AND NOT A HAZARD, which is the question this project requires be asked of every
// clause. `StratTransientReceipts.h` states the order as a decision with a reason -- the
// kills happened on the turn that is ENDING and the income belongs to the one BEGINNING, so
// a refresh carrying both reads in the order the turn actually ran. The day anything about
// receipts is repaired, that sentence is still the requirement; this clause does not
// describe a defect and so cannot go red for a defect being fixed.
//
// THE FIXTURE MUST CARRY BOTH KINDS OR THE CLAUSE ASSERTS NOTHING, so it asserts that first:
// at least one `KillFame` and exactly one `IncomeRate`. The turn edge is the real one from
// `SubmitEndTurn`; the fame rises are planted on both sides, so the `Sides`-order half has
// two entries to be in an order at all.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratTransientReceiptOrderTest,
	"Stratocracy.StratUI.GATE-TRANSIENT.KillReceiptsPrecedeTheIncomeReceipt",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratTransientReceiptOrderTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratTransientReceiptClauses;

	FStratBridge      Bridge;
	strat::UiSnapshot Opening;
	FStratViewModel   Before;
	FString           Error;
	if (!TestTrue(TEXT("the bridge seeds and the opening model builds"),
			SeedAndProject(Bridge, /*ViewingSide*/ 0, Opening, Before, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the scenario projects at least two sides"), Before.Sides.Num() >= 2))
	{
		return false;
	}

	const FStratReceiptMark Mark = StratMarkFromView(Before);

	const FStratResult Ended = Bridge.SubmitEndTurn();
	if (!TestTrue(TEXT("the rules module accepts an end of turn at the opening"), Ended.bOk))
	{
		AddError(Ended.Reason);
		return false;
	}

	strat::UiSnapshot Advanced;
	FStratViewModel   After;
	if (!TestTrue(TEXT("the model rebuilds from the advanced state"),
			Project(Bridge, /*ViewingSide*/ 0, Advanced, After, Error)))
	{
		AddError(Error);
		return false;
	}

	// Both sides' fame planted up, so there are two kill receipts to be in an order.
	for (int32 Side = 0; Side < After.Sides.Num(); ++Side)
	{
		if (!TestTrue(TEXT("every side's FameTotal is a usable plant"),
				After.Sides[Side].FameTotal > After.Sides[Side].FameCombat))
		{
			return false;
		}
		After.Sides[Side].FameCombat = After.Sides[Side].FameTotal;
	}

	FStratTransientReceipts Answer;
	StratDecideTransientReceipts(Mark, After, Answer);

	const int32 Kills  = CountOfKind(Answer, EStratReceiptKind::KillFame);
	const int32 Income = CountOfKind(Answer, EStratReceiptKind::IncomeRate);
	if (!TestTrue(*FString::Printf(
			TEXT("the fixture carries both kinds, or this clause asserts nothing: %s"),
			*DescribeReceipts(Answer)),
			Kills >= 2 && Income == 1))
	{
		return false;
	}

	int32 IncomeIndex = INDEX_NONE;
	int32 LastKillSide = INDEX_NONE;
	for (int32 Index = 0; Index < Answer.Receipts.Num(); ++Index)
	{
		const FStratReceiptView& Receipt = Answer.Receipts[Index];
		if (Receipt.Kind == EStratReceiptKind::IncomeRate)
		{
			TestEqual(TEXT("GATE-TRANSIENT: there is one income receipt and it is the last entry"),
				Index, Answer.Receipts.Num() - 1);
			IncomeIndex = Index;
		}
		else
		{
			TestEqual(*FString::Printf(
					TEXT("GATE-TRANSIENT: kill receipt at %d precedes the income receipt"), Index),
				IncomeIndex, INDEX_NONE);
			TestTrue(*FString::Printf(
					TEXT("GATE-TRANSIENT: kill receipts run in ascending Sides order (%d after %d)"),
					Receipt.Side, LastKillSide),
				Receipt.Side > LastKillSide);
			LastKillSide = Receipt.Side;
		}
	}
	// `INDEX_NONE` is spelled into an `int32` local because `TestNotEqual`'s overload set is
	// ambiguous over the macro's own type, while `TestEqual`'s is not. Measured, one C2668.
	const int32 kNotFound = INDEX_NONE;
	TestNotEqual(TEXT("GATE-TRANSIENT: the income receipt was found in the list"),
		IncomeIndex, kNotFound);

	return true;
}

// ---------------------------------------------------------------------------
// THE BANNER FOLLOWS `SideToMove` AGAINST THE SEAT THE MODEL WAS RENDERED FOR.
//
// BOTH SEATS ARE REAL MODELS, NOT PLANTED ONES. `StratBuildViewModel` is called twice off
// the SAME seeded bridge, once for each seat, so `FStratViewModel::ViewingSide` is the
// builder's own answer and the two models differ in nothing else. That is what makes this a
// test of the viewer-relative MAPPING rather than of a field a test wrote itself: with
// `SideToMove` fixed by the rules at the first side, seat 0 must read YOUR TURN and seat 1
// must read ENEMY TURN, and a decider that ignored the seat -- or compared the seat against
// the wrong field -- gives the same answer for both.
//
// THE NEGATIVE SENTINEL IS ASSERTED THROUGH THE THREE-SCALAR ENTRY POINT, which is what that
// entry point exists for: `FStratMatchView` defaults `SideToMove` to 0, a real side, so the
// only sentinel a caller can hand in is a negative one and no projection this tree can build
// carries it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratTransientBannerSeatTest,
	"Stratocracy.StratUI.GATE-TRANSIENT.BannerFollowsSideToMoveAgainstTheSeat",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratTransientBannerSeatTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratTransientReceiptClauses;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	strat::UiSnapshot Snapshot;
	FStratViewModel   AtSeatToMove;
	if (!TestTrue(TEXT("the model builds for the seat that moves first"),
			Project(Bridge, kFirstSide, Snapshot, AtSeatToMove, Error)))
	{
		AddError(Error);
		return false;
	}

	const int32 SideToMove = AtSeatToMove.Match.SideToMove;
	TestEqual(TEXT("the builder recorded the seat it was asked for"),
		AtSeatToMove.ViewingSide, kFirstSide);
	TestEqual(TEXT("and the rules module agrees about who moves"),
		SideToMove, static_cast<int32>(Snapshot.match.sideToMove));

	// The other seat: a real model, from the same bridge, for a different viewer.
	const int32 OtherSeat = AtSeatToMove.Sides.Num() - 1 - SideToMove;
	if (!TestTrue(TEXT("the scenario has a second seat to render for"),
			OtherSeat != SideToMove && OtherSeat >= 0))
	{
		return false;
	}

	strat::UiSnapshot OtherSnapshot;
	FStratViewModel   AtOtherSeat;
	if (!TestTrue(TEXT("the model builds for the other seat"),
			Project(Bridge, OtherSeat, OtherSnapshot, AtOtherSeat, Error)))
	{
		AddError(Error);
		return false;
	}
	TestEqual(TEXT("the two models describe one match and differ only in the seat"),
		AtOtherSeat.Match.SideToMove, SideToMove);

	// ---- through the whole decider, which is what a caller actually gets --------------
	{
		FStratTransientReceipts Mine;
		StratDecideTransientReceipts(FStratReceiptMark(), AtSeatToMove, Mine);
		TestEqual(TEXT("GATE-TRANSIENT: the seat that moves reads YOUR TURN"),
			static_cast<int32>(Mine.Banner), static_cast<int32>(EStratTurnBanner::YourTurn));
		TestEqual(TEXT("GATE-TRANSIENT: and BannerSide names the side it was resolved against"),
			Mine.BannerSide, SideToMove);

		FStratTransientReceipts Theirs;
		StratDecideTransientReceipts(FStratReceiptMark(), AtOtherSeat, Theirs);
		TestEqual(TEXT("GATE-TRANSIENT: the other seat reads ENEMY TURN over the same match"),
			static_cast<int32>(Theirs.Banner), static_cast<int32>(EStratTurnBanner::EnemyTurn));
		TestEqual(TEXT("GATE-TRANSIENT: and names the same side, the answer being viewer-relative "
		               "and not side-relative"),
			Theirs.BannerSide, SideToMove);
	}

	// ---- and the sentinel, through the three-scalar entry point ----------------------
	TestEqual(TEXT("GATE-TRANSIENT: a negative SideToMove is None"),
		static_cast<int32>(StratDecideTurnBanner(INDEX_NONE, kFirstSide, /*bHasResult*/ false)),
		static_cast<int32>(EStratTurnBanner::None));

	return true;
}

// ---------------------------------------------------------------------------
// A RESULT SILENCES THE BANNER, WHATEVER THE SIDES SAY.
//
// `None` IS A REAL STATE AND NOT A NULL. A concluded match has no turn to announce, and the
// rules do not blank `sideToMove` when the match ends -- so a decider that compared the sides
// FIRST would announce a turn for a match that is over, and §2.11.4's end-of-match screen
// would be arguing with a banner behind it.
//
// BOTH SEATS, BECAUSE THE DEFECT HAS TWO FACES. Silencing only the enemy arm leaves the
// winner's screen reading YOUR TURN forever; silencing only the your-turn arm leaves the
// loser's reading ENEMY TURN. The clause drives the result over a model rendered for each
// seat and requires `None` from both, plus `BannerSide == INDEX_NONE` -- the field is the
// only thing that says WHICH side an answer was resolved against, and leaving a live side
// index on a `None` banner would let a widget resurrect the announcement.
//
// THE RESULT FLAG IS PLANTED, WHICH IS WHAT THE THREE-SCALAR ENTRY POINT IS FOR. This tree
// cannot reach a concluded `strat::GameState` without playing a match to §2.8's end, and
// `StratTransientReceipts.h` states in terms that the decider takes scalars "so that a clause
// can drive the `None` arm without constructing a concluded match". The flag is an INPUT
// here and never an expectation.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratTransientResultSilencesBannerTest,
	"Stratocracy.StratUI.GATE-TRANSIENT.ResultSilencesTheBanner",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratTransientResultSilencesBannerTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratTransientReceiptClauses;

	FStratBridge      Bridge;
	strat::UiSnapshot Snapshot;
	FStratViewModel   Model;
	FString           Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, kFirstSide, Snapshot, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	// The control: while the match is live this model banners a real turn, so the silence
	// below is caused by the flag and not by the fixture being mute to begin with.
	{
		FStratTransientReceipts Live;
		StratDecideTransientReceipts(FStratReceiptMark(), Model, Live);
		if (!TestNotEqual(TEXT("the same model banners a real turn while the match is live"),
				static_cast<int32>(Live.Banner), static_cast<int32>(EStratTurnBanner::None)))
		{
			return false;
		}
	}

	for (int32 Seat = 0; Seat < Model.Sides.Num(); ++Seat)
	{
		FStratViewModel Concluded;
		if (!TestTrue(TEXT("the model rebuilds for this seat"),
				StratBuildViewModel(Bridge, Seat, Concluded, Error)))
		{
			AddError(Error);
			return false;
		}
		Concluded.Match.bHasResult = true;   // the planted INPUT; see the block above

		FStratTransientReceipts Answer;
		StratDecideTransientReceipts(FStratReceiptMark(), Concluded, Answer);

		TestEqual(*FString::Printf(
				TEXT("GATE-TRANSIENT: seat %d over a concluded match banners None, though "
				     "SideToMove still reads %d"), Seat, Concluded.Match.SideToMove),
			static_cast<int32>(Answer.Banner), static_cast<int32>(EStratTurnBanner::None));
		TestEqual(*FString::Printf(
				TEXT("GATE-TRANSIENT: and seat %d's BannerSide is INDEX_NONE, so no widget can "
				     "resurrect the announcement"), Seat),
			Answer.BannerSide, INDEX_NONE);
	}

	// And directly, at both seat/side combinations, through the entry point that owns the
	// mapping -- so the result arm is pinned independent of any model at all.
	TestEqual(TEXT("GATE-TRANSIENT: the result silences the your-turn arm"),
		static_cast<int32>(StratDecideTurnBanner(kFirstSide, kFirstSide, /*bHasResult*/ true)),
		static_cast<int32>(EStratTurnBanner::None));
	TestEqual(TEXT("GATE-TRANSIENT: and the enemy-turn arm"),
		static_cast<int32>(StratDecideTurnBanner(kFirstSide,
		                                         Model.Sides.Num() - 1 - kFirstSide,
		                                         /*bHasResult*/ true)),
		static_cast<int32>(EStratTurnBanner::None));

	return true;
}

// ---------------------------------------------------------------------------
// `bBannerJustChanged` IS DECIDED AGAINST THE MARK'S OWN INPUTS -- THE REGRESSION CLAUSE.
//
// THIS PINS TWO DEFECTS THE FIRST SPELLING OF `FStratReceiptMark` HAD, both caused by the
// mark storing only `SideToMove` and the previous banner being recomputed with a CURRENT
// value for the other two inputs. Both are silent: neither changes a number on screen, and
// both are only visible as an entrance animation that never stops or never starts. THE
// CLAUSE RUNS FOUR CASES, not two, and the reason the count grew is recorded below rather
// than smoothed over.
//
//   0. THE CONTROL. The same model applied against its own mark reports no change, so the
//      cases below cannot all be satisfied by a flag stuck at true.
//
//   1. THE CONCLUDED MATCH THAT MUST NOT CHANGE FOREVER. With no `bMatchHadResult` on the
//      mark, the previous banner is recomputed with the flag FALSE, so the old side always
//      maps to a turn arm while the current answer is always `None` -- and every refresh of a
//      finished match reports a change. The mark is taken FROM the concluded model and that
//      same model applied again: nothing moved, so `bBannerJustChanged` must be false.
//
//   1b. THE REFRESH ON WHICH THE MATCH ENDS, WHICH MUST CHANGE. The mark is taken while the
//      match is LIVE and a concluded model is applied against it.
//
//   2. THE SEAT THAT CHANGES NOTHING. `SetViewingSide` flips the banner with no rules field
//      moving at all. With no `ViewingSide` on the mark, the previous banner is recomputed
//      with the NEW seat and the flip reads as no change -- so the one transition hot-seat
//      exists to announce is the one that goes unannounced. The mark is taken from a model
//      rendered for one seat and a real model rendered for the other is applied, over the
//      identical rules state.
//
// CASE 1b IS HERE BECAUSE THIS BLOCK ONCE CLAIMED CASE 1 ALONE COVERED DEFECT 1, AND A
// MUTANT SAID OTHERWISE:
//
//   STRUCK> "THIS PINS TWO DEFECTS ... 1. THE CONCLUDED MATCH THAT CHANGES FOREVER. ... The
//   STRUCK>  fixture takes the mark FROM the concluded model and applies that same model
//   STRUCK>  again: nothing whatever moved, so `bBannerJustChanged` must be false. ... AND A
//   STRUCK>  CONTROL ... the same model applied against its own mark reports no change, and
//   STRUCK>  case 2 above reports a change. Both directions are measured."
//
// WITHDRAWN AS TO COVERAGE. Case 1 is a true and useful assertion and is still run; what was
// false was the claim that it pins defect 1. Measured: a decider recomputing the previous
// banner with the CURRENT result flag ran the FULL SUITE GREEN against cases 0, 1 and 2,
// because case 1 takes the mark OVER the concluded model, so the mark's flag and the model's
// already agree and the fixture cannot tell one from the other. Case 1b is the fixture in
// which they differ, and it reddens that decider on its own. "Both directions are measured"
// was true of the seat and false of the result.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratTransientBannerChangeAgainstMarkTest,
	"Stratocracy.StratUI.GATE-TRANSIENT.BannerChangeIsRelativeToTheMarksOwnInputs",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratTransientBannerChangeAgainstMarkTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratTransientReceiptClauses;

	FStratBridge      Bridge;
	strat::UiSnapshot Snapshot;
	FStratViewModel   Model;
	FString           Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, kFirstSide, Snapshot, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the scenario projects at least two seats"), Model.Sides.Num() >= 2))
	{
		return false;
	}

	// ---- the control: the same model against its own mark reports no change ----------
	{
		const FStratReceiptMark Mark = StratMarkFromView(Model);
		FStratTransientReceipts Answer;
		StratDecideTransientReceipts(Mark, Model, Answer);
		TestFalse(TEXT("GATE-TRANSIENT: a refresh in which nothing moved is not a banner change"),
			Answer.bBannerJustChanged);
	}

	// ---- defect 1: a concluded match must not report a change on every refresh -------
	{
		FStratViewModel Concluded = Model;
		Concluded.Match.bHasResult = true;

		const FStratReceiptMark Mark = StratMarkFromView(Concluded);
		TestTrue(TEXT("the mark records the result flag it was taken over"), Mark.bMatchHadResult);

		FStratTransientReceipts Answer;
		StratDecideTransientReceipts(Mark, Concluded, Answer);

		TestEqual(TEXT("the fixture's current banner really is the None arm"),
			static_cast<int32>(Answer.Banner), static_cast<int32>(EStratTurnBanner::None));
		TestFalse(TEXT("GATE-TRANSIENT: a concluded match reports no banner change on a refresh "
		               "where nothing moved -- the mark's own result flag is what it is compared "
		               "against"),
			Answer.bBannerJustChanged);
	}

	// ---- defect 1, THE SHARP HALF: the refresh on which the match ENDS -----------------
	//
	// MEASURED, AND THE REASON THIS CASE EXISTS SEPARATELY FROM THE ONE ABOVE. A mutant that
	// recomputed the previous banner with the CURRENT result flag ran the whole suite GREEN
	// against the case above, because there the mark was TAKEN OVER the concluded model and
	// the two flags already agreed -- the fixture could not tell the mark's flag from the
	// model's. Here they differ: the mark is taken while the match is LIVE and the model
	// applied against it is concluded, which is the one refresh in a match where the banner
	// goes from a turn arm to `None`. Forcing the previous banner to use the current flag
	// makes both sides read `None` and hides exactly this transition -- the half the header
	// says the naive fix "hides", as opposed to the half it over-reports.
	{
		const FStratReceiptMark Mark = StratMarkFromView(Model);
		TestFalse(TEXT("the mark was taken while the match was still live"), Mark.bMatchHadResult);

		FStratViewModel Concluded = Model;
		Concluded.Match.bHasResult = true;

		FStratTransientReceipts Answer;
		StratDecideTransientReceipts(Mark, Concluded, Answer);

		TestEqual(TEXT("the fixture's current banner really is the None arm"),
			static_cast<int32>(Answer.Banner), static_cast<int32>(EStratTurnBanner::None));
		TestTrue(TEXT("GATE-TRANSIENT: the refresh on which the match ends IS a banner change -- "
		              "the mark's own result flag, not the model's, is what it is compared against"),
			Answer.bBannerJustChanged);
	}

	// ---- defect 2: a seat flip alone IS a banner change ------------------------------
	{
		const int32 OtherSeat = Model.Sides.Num() - 1 - Model.ViewingSide;
		if (!TestTrue(TEXT("the fixture's other seat is a different index"),
				OtherSeat != Model.ViewingSide))
		{
			return false;
		}

		FStratViewModel AtOtherSeat;
		if (!TestTrue(TEXT("a real model builds for the other seat"),
				StratBuildViewModel(Bridge, OtherSeat, AtOtherSeat, Error)))
		{
			AddError(Error);
			return false;
		}
		TestEqual(TEXT("and no rules field moved between the two models"),
			AtOtherSeat.Match.SideToMove, Model.Match.SideToMove);
		TestEqual(TEXT("nor the turn"), AtOtherSeat.Match.Turn, Model.Match.Turn);

		const FStratReceiptMark Mark = StratMarkFromView(Model);
		TestEqual(TEXT("the mark records the seat it was taken at"),
			Mark.ViewingSide, Model.ViewingSide);

		FStratTransientReceipts Answer;
		StratDecideTransientReceipts(Mark, AtOtherSeat, Answer);

		TestNotEqual(TEXT("the fixture really did flip the banner"),
			static_cast<int32>(Answer.Banner),
			static_cast<int32>(StratDecideTurnBanner(Mark.SideToMove, Mark.ViewingSide,
			                                         Mark.bMatchHadResult)));
		TestTrue(TEXT("GATE-TRANSIENT: a seat hand-over with no rules field moving IS a banner "
		              "change -- the mark's own seat is what it is compared against"),
			Answer.bBannerJustChanged);
	}

	return true;
}

// ---------------------------------------------------------------------------
// THE ANSWER IS OVERWRITTEN WHOLE ON EVERY CALL, INCLUDING EMPTIED.
//
// WHY THIS IS NOT COVERED BY ANY CLAUSE ABOVE. Every one of them hands the decider a fresh
// `FStratTransientReceipts`, so an implementation that APPENDED would pass all of them. The
// live caller does not: `UStratMatchSubsystem` holds one by value and reuses it on every
// refresh, so an append would make a toast list grow without bound for the length of a match
// and would redraw the first turn's income receipt on the last turn.
//
// THE SECOND CALL IS THE QUIET REFRESH -- the model applied against a mark taken from itself,
// which is what most frames are. The list must come back empty, and `bBannerJustChanged` must
// come back false, both having been true and non-empty one call earlier over the same struct.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratTransientOverwriteNotAccumulateTest,
	"Stratocracy.StratUI.GATE-TRANSIENT.ReceiptsAreOverwrittenNotAccumulated",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratTransientOverwriteNotAccumulateTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratTransientReceiptClauses;

	FStratBridge      Bridge;
	strat::UiSnapshot Snapshot;
	FStratViewModel   Model;
	FString           Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, kFirstSide, Snapshot, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the scenario projects at least two seats"), Model.Sides.Num() >= 2))
	{
		return false;
	}
	if (!TestTrue(TEXT("the opening's FameTotal is a usable plant"),
			Model.Sides[0].FameTotal > Model.Sides[0].FameCombat))
	{
		return false;
	}

	// ---- one loud refresh: a fame rise AND a seat flip, into a reused struct ---------
	FStratTransientReceipts Answer;
	{
		const int32 OtherSeat = Model.Sides.Num() - 1 - Model.ViewingSide;
		FStratViewModel AtOtherSeat;
		if (!TestTrue(TEXT("a real model builds for the other seat"),
				StratBuildViewModel(Bridge, OtherSeat, AtOtherSeat, Error)))
		{
			AddError(Error);
			return false;
		}
		AtOtherSeat.Sides[0].FameCombat = Model.Sides[0].FameTotal;

		StratDecideTransientReceipts(StratMarkFromView(Model), AtOtherSeat, Answer);

		if (!TestTrue(*FString::Printf(TEXT("the first call really was loud: %s"),
				*DescribeReceipts(Answer)),
				Answer.Receipts.Num() > 0 && Answer.bBannerJustChanged))
		{
			return false;
		}
	}

	// ---- then the quiet one, into the SAME struct ------------------------------------
	StratDecideTransientReceipts(StratMarkFromView(Model), Model, Answer);

	TestEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: a quiet refresh empties the answer it is given: %s"),
			*DescribeReceipts(Answer)),
		Answer.Receipts.Num(), 0);
	TestFalse(TEXT("GATE-TRANSIENT: and clears the banner-change flag it found set"),
		Answer.bBannerJustChanged);

	return true;
}
