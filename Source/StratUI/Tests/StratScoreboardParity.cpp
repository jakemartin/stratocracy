// The editor pass for §2.11.4's live standings scoreboard -- T-UI-03. GDD §2.11, §2.8,
// §2.13.2, §4.11 row 8.
//
//   T-UI-03  the live standings scoreboard (§2.11, §2.8) binds 1:1 to snapshot fields
//            -- enemy strength destroyed, objectives held X/N, surviving units/HP,
//            turn vs cap -- with no widget-side arithmetic.
//
// WHAT "BINDS 1:1" IS MADE TO MEAN HERE, because an acceptance ID that is not made
// operational is decoration. Every clause below takes a `strat::UiSnapshot` from the
// bridge, takes an `FStratScoreboardModel` built from the SAME bridge, and asserts
// that each displayed value is EQUAL TO ONE NAMED FIELD of that snapshot. The
// expectation side of every comparison is a field read, never a derivation: this file
// contains no sum over `Snapshot.units`, no difference of two sides' fame, no
// percentage. If the test computed the expectation, a widget and a test that made the
// same arithmetic mistake would agree, and the gate would certify the mistake.
//
// WHY THE FIDELITY CHECK RUNS HERE TOO, and it is not padding. Binding 1:1 to a
// snapshot only means anything if the snapshot is true of the state it claims to
// project. A scoreboard can be perfectly bound and still lie all match if the
// projection under it lies -- and §2.11.4's whole reason for existing is revision
// §1.5-#1, the tiebreak must never be a hidden win condition. So the last clause runs
// `strat::uiCheckSnapshotFidelity` over the bridge's own projection and fails on any
// reported failure. T-UI-05 owns that check as its subject; this file consumes it,
// because a green T-UI-03 over a lying projection would be a false clearance of the
// thing §1.5-#1 asked for.
//
// WHY THE STATE UNDER TEST IS THE SEEDED OPENING AND NOT A MID-MATCH POSITION. The
// only mid-match position this tree can reach deterministically is the one
// `Data/parity_fixture.save` replays to, and reaching it here would couple this gate
// to the §4.10 log format for no gain in what it asserts: the clauses below check that
// the model AGREES WITH the snapshot, and that agreement is a property of the builder
// rather than of any particular board. It is stated rather than hidden, though: the
// chevron clause exercises whichever §2.8 criterion Ferrum Crossing's opening happens
// to decide on, and a variant driven to a position where an EARLIER criterion decides
// is a later pass, not something this file quietly claims to have covered.
//
// NOT ASSERTED HERE, with reasons:
// - §2.11.4's cap-approach banners. StratScoreboardWidget.h records that their trigger
//   is arithmetic over `turn` and `turnCap` with no field carrying the answer, and that
//   the banner needs an owner ruled first. There is nothing to bind, so there is
//   nothing to assert.
// - The end-of-match screen and its faction-voiced result line. A different surface
//   with a different lifetime; kb/setting.md's pipeline note is why it must not be
//   folded into the persistent panel, and folding it into this file would be the same
//   mistake one layer up.
// - Whether `survivingHp` is the correct sum over that side's units. That is the
//   projection's arithmetic, which is the fidelity check's subject and T-UI-05's ID.
//   Recomputing it here would give this gate a second opinion about a number it is
//   supposed to be checking the TRANSPORT of.
// - Pixels. The builder is a free function precisely so no Slate tree is needed to see
//   a value; nothing below constructs a widget.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Internationalization/Text.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratScoreboardWidget.h"

// The bridge, included from a .cpp and never from a UHT-parsed header -- the same
// constraint StratScoreboardWidget.h and StratUI.Build.cs both record. A test file is
// a .cpp, so it may say this.
#include "StratBridge.h"

// The vendored view-model header, by short name via StratRules' PublicIncludePaths.
// Reached through StratBridge.h already; named explicitly because this file reads
// `strat::UiSnapshot` fields directly and a reader should not have to discover that
// through a transitive include.
#include "Ui.h"

namespace StratScoreboardParity
{
	// MUST match the side the rest of the engine-side harness seeds with. Replay.h
	// states that no rule in this project decides which side moves first, so this is a
	// harness pin and not a rule: it exists so the projected opening position is the
	// same one every run, and so a divergence shows up as a failed clause rather than
	// as a scoreboard that reads differently on alternate runs. StratBridgeParity.cpp
	// pins the same value for the same reason.
	static const int32 kFirstSide = 0;

	// The viewing side these clauses build for by default. NOT `sideToMove`: the
	// snapshot carries whose turn it is, this is whose screen it is, and
	// StratScoreboardWidget.h is explicit that the two differ every other turn. The
	// ColumnsFollowViewingSide clause below builds for both.
	static const int32 kViewingSide = 0;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static FString FromStd(const std::string& In)
	{
		return FString(UTF8_TO_TCHAR(In.c_str()));
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/**
	 * Brings a bridge to the seeded opening: the shipped tables mapped in, the shipped
	 * scenario seeded through the rules module's own `seedFromScenario`.
	 *
	 * Returns false and fills OutError rather than asserting, on StratBridgeParity's
	 * precedent: a missing asset becomes a named failure inside the caller's own clause
	 * list instead of a crash in a helper, and every clause reports setup failure the
	 * same way.
	 *
	 * NOTHING IS HARDCODED PAST THE ASSET PATHS. The turn cap, the objective total and
	 * both sides' standings all arrive from `ferrum_crossing.json` and the two tables
	 * through the projection; §2.11.4's "/ 20" and "N = 8" appear nowhere in this file,
	 * and a scenario edit that changed either must not need an edit here.
	 */
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
	 * Seeds a bridge, projects a snapshot, and builds the model for `ViewingSide`.
	 *
	 * TWO PROJECTIONS OF ONE STATE, and that is safe here for a stated reason rather
	 * than by hope: no command is submitted between them, so the authoritative
	 * `GameState` is byte-identical across both calls, and `MakeUiSnapshot` is the
	 * rules module's function over that state. The alternative -- having the builder
	 * hand its snapshot back out -- would have widened the production surface purely to
	 * suit a test, which is the tail wagging the dog.
	 */
	static bool Project(
		FStratBridge&           Bridge,
		int32                   ViewingSide,
		strat::UiSnapshot&      OutSnapshot,
		FStratScoreboardModel&  OutModel,
		FString&                OutError)
	{
		if (!SeedBridge(Bridge, OutError))
		{
			return false;
		}

		const FStratResult Projected = Bridge.MakeUiSnapshot(OutSnapshot);
		if (!Projected.bOk)
		{
			OutError = FString::Printf(TEXT("MakeUiSnapshot refused: %s"), *Projected.Reason);
			return false;
		}

		if (!StratBuildScoreboardModel(Bridge, ViewingSide, OutModel, OutError))
		{
			OutError = FString::Printf(TEXT("StratBuildScoreboardModel refused: %s"), *OutError);
			return false;
		}
		return true;
	}

	/**
	 * The column that is not the viewer's, found by scanning rather than by
	 * `1 - ViewingSide` -- the same reasoning StratScoreboardWidget.cpp gives for the
	 * same choice. The subtraction is correct for two sides and is the kind of correct
	 * that stops being correct without failing to compile; the scan says "the side that
	 * is not the viewer", and the two-side check at each call site is what makes the
	 * answer unique.
	 */
	static int32 OtherSide(const strat::UiSnapshot& Snapshot, int32 ViewingSide)
	{
		const int32 SideCount = static_cast<int32>(UE_ARRAY_COUNT(Snapshot.side));
		for (int32 Index = 0; Index < SideCount; ++Index)
		{
			if (Index != ViewingSide)
			{
				return Index;
			}
		}
		return INDEX_NONE;
	}

	struct FExpectedLead
	{
		EStratScoreColumn Column    = EStratScoreColumn::None;
		int32             Criterion = INDEX_NONE;
	};

	/**
	 * §2.8's tiebreak, walked in criteria order over snapshot fields.
	 *
	 * DELIBERATELY WRITTEN FROM THE GDD TEXT AND NOT COPIED OUT OF THE BUILDER. A test
	 * that pasted the implementation would assert that the code equals itself. This
	 * walks the three criteria in the order §2.8 gives them -- combat Fame, then
	 * objectives held, then surviving strength -- stops at the first one where the two
	 * sides differ, and awards it to the higher value, which is what §2.8 says and all
	 * it says.
	 *
	 * A COMPARISON, NOT ARITHMETIC, which is the whole reason this can live outside the
	 * rules module at all. StratScoreboardWidget.h records the standing debt attached
	 * to that: when the rules module exposes its own criteria-order comparison, the
	 * builder must delegate to it -- and so must this, or the gate becomes a third
	 * opinion about one rule.
	 */
	static FExpectedLead ExpectedLead(const strat::UiSideView& You, const strat::UiSideView& Enemy)
	{
		const int32 YouByCriterion[3] =
		{
			static_cast<int32>(You.fameCombat),
			static_cast<int32>(You.objectivesHeld),
			static_cast<int32>(You.survivingHp)
		};
		const int32 EnemyByCriterion[3] =
		{
			static_cast<int32>(Enemy.fameCombat),
			static_cast<int32>(Enemy.objectivesHeld),
			static_cast<int32>(Enemy.survivingHp)
		};

		FExpectedLead Lead;
		for (int32 Criterion = 0; Criterion < 3; ++Criterion)
		{
			if (YouByCriterion[Criterion] == EnemyByCriterion[Criterion])
			{
				continue;
			}
			Lead.Column = (YouByCriterion[Criterion] > EnemyByCriterion[Criterion])
				? EStratScoreColumn::You
				: EStratScoreColumn::Enemy;
			Lead.Criterion = Criterion;
			break;
		}
		return Lead;
	}

	/**
	 * Runs the rules module's own snapshot-fidelity check over the bridge's projection.
	 *
	 * THE ONE PLACE THIS FILE NAMES THAT FUNCTION'S RESULT SHAPE, on purpose. Every
	 * clause below calls this helper and none of them mentions how a failure is spelt,
	 * so a change to `Ui.h`'s reporting is reconciled here and no clause moves.
	 *
	 * `uiCheckSnapshotFidelity` returns a `UiFidelityResult` -- a STRUCT carrying `ok`,
	 * three counters, and a `failures` vector of `{clause, field, detail}`. It is not
	 * itself iterable, and the three failure members are not `id`/`reason`.
	 *
	 * `Result.ok` IS CHECKED AND NOT INFERRED FROM AN EMPTY FAILURE LIST. Ui.h states
	 * that clause (c) fails a snapshot field carrying no contract entry; treating "no
	 * failures listed" as success would make this helper agree with a result that said
	 * otherwise, which is the same class of defect T-UI-05 exists to catch.
	 *
	 * FAILURES ARE FORWARDED, NOT SUMMARISED. A fidelity failure is the rules module
	 * saying a projected field does not match the state it projects, and the module's
	 * own words are the useful part -- collapsing them to "fidelity failed" would leave
	 * a reader of the test log with strictly less than the code already knew.
	 */
	static bool RunSnapshotFidelity(
		const strat::UiWorld&    World,
		const strat::UiSnapshot& Snapshot,
		TArray<FString>&         OutFailures)
	{
		OutFailures.Reset();
		const strat::UiFidelityResult Result =
			strat::uiCheckSnapshotFidelity(World, Snapshot);
		for (const strat::UiFidelityFailure& Failure : Result.failures)
		{
			OutFailures.Add(FString::Printf(TEXT("clause (%s) %s: %s"),
				*FromStd(Failure.clause), *FromStd(Failure.field), *FromStd(Failure.detail)));
		}
		return Result.ok && OutFailures.Num() == 0;
	}

	/**
	 * Fails the clause unless the panel is the two-column YOU/ENEMY shape §2.11.4
	 * draws. Every clause calls it before indexing `side[]`, because with more than two
	 * sides there is no single ENEMY column and any expectation this file formed would
	 * be an invention rather than a reading.
	 */
	static bool RequireTwoSides(FAutomationTestBase& Test, const strat::UiSnapshot& Snapshot)
	{
		const int32 SideCount = static_cast<int32>(UE_ARRAY_COUNT(Snapshot.side));
		return Test.TestEqual(
			TEXT("GDD 2.11.4's scoreboard is a two-column YOU/ENEMY panel"),
			SideCount, 2);
	}

	/** The three §2.8 rows, in the order §2.11.4 says the layout itself expresses. */
	static bool RequireThreeRows(FAutomationTestBase& Test, const FStratScoreboardModel& Model)
	{
		return Test.TestEqual(
			TEXT("GDD 2.8 has three attrition criteria, so the scoreboard has three rows"),
			Model.Rows.Num(), 3);
	}
}

// ---------------------------------------------------------------------------
// Turn vs cap -- T-UI-03's fourth named pair.
//
// §2.11.4: "the cap is per-scenario data, so the widget reads `turnCap` from the
// scenario rather than hardcoding a number". This clause is what makes that sentence
// checkable: both numbers are compared against `UiMatchView`, and the scenario's actual
// cap is never written down here, so a scenario that shipped a different cap would move
// both sides of the comparison together and this clause would still hold -- while a
// widget that had baked in `20` would fail it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratScoreboardTurnAgainstCapTest,
	"Stratocracy.StratUI.T-UI-03.TurnAgainstCap",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratScoreboardTurnAgainstCapTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratScoreboardParity;

	FStratBridge Bridge;
	strat::UiSnapshot Snapshot;
	FStratScoreboardModel Model;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds and projects"), Project(Bridge, kViewingSide, Snapshot, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	TestEqual(TEXT("Model.Turn == UiMatchView::turn"),
		Model.Turn, static_cast<int32>(Snapshot.match.turn));
	TestEqual(TEXT("Model.TurnCap == UiMatchView::turnCap"),
		Model.TurnCap, static_cast<int32>(Snapshot.match.turnCap));

	// The composed line is checked for CONTAINMENT of the two bound numbers rather than
	// for equality against a re-derived string. Re-deriving it would mean restating
	// "TURN {0} / {1}" here, which is UI copy: this gate would then fail on a wording
	// change that broke nothing, and -- worse -- it would pass on a wording that
	// displayed the wrong field, since the format string carries no field names. What
	// T-UI-03 is about is which numbers reach the line, and that is what this asserts.
	const FString TurnLine = Model.TurnText.ToString();
	TestTrue(
		FString::Printf(TEXT("turn line '%s' shows the bound turn %d"), *TurnLine, Model.Turn),
		TurnLine.Contains(FString::FromInt(Model.Turn)));
	TestTrue(
		FString::Printf(TEXT("turn line '%s' shows the scenario's cap %d"), *TurnLine, Model.TurnCap),
		TurnLine.Contains(FString::FromInt(Model.TurnCap)));

	return true;
}

// ---------------------------------------------------------------------------
// Destroyed -- T-UI-03's "enemy strength destroyed", criterion 1.
//
// THE FIELD CHOICE IS THE CLAUSE. §2.11.4 says the row is combat Fame -- kills and the
// flag bonus -- and that "passive income is excluded, exactly as the tiebreak excludes
// it", and that the row "deliberately does not equal the spendable Fame pool". So
// binding to `fameTotal` would produce a scoreboard that looked right, added up, and
// silently misreported the tiebreak: the exact hidden-win-condition failure §1.5-#1
// created this panel to prevent. The negative half of the clause is guarded on the two
// fields actually differing, because on a position where nothing has been earned or
// spent they legitimately coincide and a bare `!=` would be asserting a coincidence.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratScoreboardDestroyedBindsCombatFameTest,
	"Stratocracy.StratUI.T-UI-03.DestroyedBindsCombatFame",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratScoreboardDestroyedBindsCombatFameTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratScoreboardParity;

	FStratBridge Bridge;
	strat::UiSnapshot Snapshot;
	FStratScoreboardModel Model;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds and projects"), Project(Bridge, kViewingSide, Snapshot, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!RequireTwoSides(*this, Snapshot) || !RequireThreeRows(*this, Model))
	{
		return false;
	}

	const int32 EnemySide = OtherSide(Snapshot, kViewingSide);
	const strat::UiSideView& You   = Snapshot.side[kViewingSide];
	const strat::UiSideView& Enemy = Snapshot.side[EnemySide];

	const FStratScoreboardRow& Row = Model.Rows[static_cast<int32>(EStratScoreCriterion::CombatFame)];
	TestEqual(TEXT("row 0 is the CombatFame criterion"),
		static_cast<int32>(Row.Criterion), static_cast<int32>(EStratScoreCriterion::CombatFame));

	TestEqual(TEXT("Destroyed YOU == UiSideView::fameCombat"),
		Row.YouValue, static_cast<int32>(You.fameCombat));
	TestEqual(TEXT("Destroyed ENEMY == UiSideView::fameCombat"),
		Row.EnemyValue, static_cast<int32>(Enemy.fameCombat));

	if (You.fameCombat != You.fameTotal)
	{
		TestNotEqual(TEXT("Destroyed YOU is combat Fame, not the spendable pool (GDD 2.11.4)"),
			Row.YouValue, static_cast<int32>(You.fameTotal));
	}
	if (Enemy.fameCombat != Enemy.fameTotal)
	{
		TestNotEqual(TEXT("Destroyed ENEMY is combat Fame, not the spendable pool (GDD 2.11.4)"),
			Row.EnemyValue, static_cast<int32>(Enemy.fameTotal));
	}

	// A bare number, not "X of N": §2.11.4's mock shows `450` / `600` on this row, and a
	// total here would imply a ceiling on destroyable strength that no rule states.
	TestFalse(TEXT("Destroyed shows a bare value, not X of N"), Row.bHasTotal);

	// §2.11.4 gives this row a disambiguating tooltip because it is the one place the
	// single-currency design (§2.7) needs a sentence. The WORDING is UI copy and is
	// deliberately not restated here -- a test that pinned the sentence would fail on a
	// copy edit and would say nothing about the binding. That the row carries one at all
	// is the structural requirement.
	TestFalse(TEXT("Destroyed carries the GDD 2.11.4 disambiguating tooltip"),
		Row.Tooltip.IsEmpty());

	return true;
}

// ---------------------------------------------------------------------------
// Objectives held X of N -- T-UI-03's second named pair, §2.8 criterion 2.
//
// N COMES FROM THE SCENARIO AND IS NEVER WRITTEN HERE. §2.13 supplies it and §2.11.4's
// mock happens to show 8 on Ferrum Crossing (four factories plus four towns); this
// clause compares against `UiSnapshot::objectiveTotal` so that a scenario with a
// different objective count moves both sides of the comparison at once.
//
// ONE TOTAL SERVES BOTH COLUMNS, and that is asserted rather than assumed: the same
// board is being divided, so a per-side total would be a second number free to
// disagree with the board, which is precisely the widget-side derivation T-UI-03 bars.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratScoreboardObjectivesBindHeldOfTotalTest,
	"Stratocracy.StratUI.T-UI-03.ObjectivesBindHeldOfScenarioTotal",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratScoreboardObjectivesBindHeldOfTotalTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratScoreboardParity;

	FStratBridge Bridge;
	strat::UiSnapshot Snapshot;
	FStratScoreboardModel Model;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds and projects"), Project(Bridge, kViewingSide, Snapshot, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!RequireTwoSides(*this, Snapshot) || !RequireThreeRows(*this, Model))
	{
		return false;
	}

	const int32 EnemySide = OtherSide(Snapshot, kViewingSide);
	const strat::UiSideView& You   = Snapshot.side[kViewingSide];
	const strat::UiSideView& Enemy = Snapshot.side[EnemySide];

	const FStratScoreboardRow& Row = Model.Rows[static_cast<int32>(EStratScoreCriterion::Objectives)];
	TestEqual(TEXT("row 1 is the Objectives criterion"),
		static_cast<int32>(Row.Criterion), static_cast<int32>(EStratScoreCriterion::Objectives));

	TestEqual(TEXT("Objectives YOU == UiSideView::objectivesHeld"),
		Row.YouValue, static_cast<int32>(You.objectivesHeld));
	TestEqual(TEXT("Objectives ENEMY == UiSideView::objectivesHeld"),
		Row.EnemyValue, static_cast<int32>(Enemy.objectivesHeld));

	TestTrue(TEXT("Objectives is the X of N row (GDD 2.11.4)"), Row.bHasTotal);
	TestEqual(TEXT("Objectives N == UiSnapshot::objectiveTotal, which the scenario supplies"),
		Row.Total, static_cast<int32>(Snapshot.objectiveTotal));

	// Both composed cells must show the same N. Containment again, and for the same
	// reason as the turn line: "{0}/{1}" is UI copy, the field identity is the clause.
	const FString YouCell   = Row.YouText.ToString();
	const FString EnemyCell = Row.EnemyText.ToString();
	const FString TotalText = FString::FromInt(Row.Total);
	TestTrue(FString::Printf(TEXT("YOU objectives cell '%s' shows N=%s"), *YouCell, *TotalText),
		YouCell.Contains(TotalText));
	TestTrue(FString::Printf(TEXT("ENEMY objectives cell '%s' shows the same N=%s"), *EnemyCell, *TotalText),
		EnemyCell.Contains(TotalText));

	return true;
}

// ---------------------------------------------------------------------------
// Unit HP -- T-UI-03's "surviving units/HP", §2.8 criterion 3.
//
// §2.11.4: "listed last because it IS last". The ordering half of that is the
// RowsInTiebreakOrder clause; this one is the binding.
//
// WHAT THIS DELIBERATELY DOES NOT DO is sum `Snapshot.units[].hp` for the side and
// compare. That sum is the projection's own arithmetic, whose correctness is the
// fidelity check's subject and T-UI-05's acceptance ID. Recomputing it here would give
// this gate a second opinion about a number whose TRANSPORT is all it is meant to be
// checking -- and if the two ever disagreed, this file could not say which of them was
// the authority.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratScoreboardUnitHpBindsSurvivingStrengthTest,
	"Stratocracy.StratUI.T-UI-03.UnitHpBindsSurvivingStrength",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratScoreboardUnitHpBindsSurvivingStrengthTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratScoreboardParity;

	FStratBridge Bridge;
	strat::UiSnapshot Snapshot;
	FStratScoreboardModel Model;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds and projects"), Project(Bridge, kViewingSide, Snapshot, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!RequireTwoSides(*this, Snapshot) || !RequireThreeRows(*this, Model))
	{
		return false;
	}

	const int32 EnemySide = OtherSide(Snapshot, kViewingSide);
	const strat::UiSideView& You   = Snapshot.side[kViewingSide];
	const strat::UiSideView& Enemy = Snapshot.side[EnemySide];

	const FStratScoreboardRow& Row = Model.Rows[static_cast<int32>(EStratScoreCriterion::SurvivingHp)];
	TestEqual(TEXT("row 2 is the SurvivingHp criterion"),
		static_cast<int32>(Row.Criterion), static_cast<int32>(EStratScoreCriterion::SurvivingHp));

	TestEqual(TEXT("Unit HP YOU == UiSideView::survivingHp"),
		Row.YouValue, static_cast<int32>(You.survivingHp));
	TestEqual(TEXT("Unit HP ENEMY == UiSideView::survivingHp"),
		Row.EnemyValue, static_cast<int32>(Enemy.survivingHp));
	TestFalse(TEXT("Unit HP shows a bare value, not X of N"), Row.bHasTotal);

	return true;
}

// ---------------------------------------------------------------------------
// Row order -- §2.11.4's "the layout IS the rule".
//
// The rows are ordered top-to-bottom in exact tiebreak order (§2.8) so that the panel
// can be read passively all match and the tiebreak is never hidden (§1.5-#1). A model
// whose rows were sorted, filtered or reordered would be EDITING §2.8 through a layout
// decision, which is why this is a clause and not a comment. The enum's numeric values
// are §2.8's criteria positions, so the assertion is that array position and criterion
// ordinal coincide.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratScoreboardRowsInTiebreakOrderTest,
	"Stratocracy.StratUI.T-UI-03.RowsInTiebreakOrder",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratScoreboardRowsInTiebreakOrderTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratScoreboardParity;

	FStratBridge Bridge;
	strat::UiSnapshot Snapshot;
	FStratScoreboardModel Model;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds and projects"), Project(Bridge, kViewingSide, Snapshot, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!RequireThreeRows(*this, Model))
	{
		return false;
	}

	for (int32 RowIndex = 0; RowIndex < Model.Rows.Num(); ++RowIndex)
	{
		TestEqual(
			FString::Printf(TEXT("row %d carries GDD 2.8 criterion %d"), RowIndex, RowIndex),
			static_cast<int32>(Model.Rows[RowIndex].Criterion), RowIndex);

		// Every row is labelled. The words are UI copy and are not pinned here; a
		// nameless row would leave the panel unreadable, which is a structural failure
		// of a surface whose purpose is being read.
		TestFalse(
			FString::Printf(TEXT("row %d carries a label"), RowIndex),
			Model.Rows[RowIndex].Label.IsEmpty());
	}

	TestEqual(TEXT("the viewing side is the one that was asked for"),
		Model.ViewingSide, kViewingSide);

	return true;
}

// ---------------------------------------------------------------------------
// The chevron -- §2.11.4's attrition-tiebreak marker, "evaluated in criteria order".
//
// A COMPARISON, NOT ARITHMETIC, and still decided outside the widget: the model carries
// it as a field, and this clause checks that field against a walk of the snapshot's own
// values in §2.8's order. See ExpectedLead above for why that walk is written from the
// GDD rather than copied from the builder.
//
// WHAT THIS CANNOT PROMISE, said plainly: it exercises whichever criterion Ferrum
// Crossing's seeded opening decides on. It does not construct a position where an
// earlier criterion decides, because the builder takes a bridge and the only other
// reachable position is the parity fixture's -- see this file's header block. The
// invariants it CAN check on any position are checked unconditionally: at most one
// chevron in the model, and the model-level `Leader` agreeing with the row-level one,
// which §2.11.4 needs because it requires the chevron to "flip visibly when the lead
// changes" and a caller detects that flip off `Leader`.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratScoreboardChevronFollowsCriteriaOrderTest,
	"Stratocracy.StratUI.T-UI-03.ChevronFollowsCriteriaOrder",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratScoreboardChevronFollowsCriteriaOrderTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratScoreboardParity;

	FStratBridge Bridge;
	strat::UiSnapshot Snapshot;
	FStratScoreboardModel Model;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds and projects"), Project(Bridge, kViewingSide, Snapshot, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!RequireTwoSides(*this, Snapshot) || !RequireThreeRows(*this, Model))
	{
		return false;
	}

	const int32 EnemySide = OtherSide(Snapshot, kViewingSide);
	const FExpectedLead Expected =
		ExpectedLead(Snapshot.side[kViewingSide], Snapshot.side[EnemySide]);

	TestEqual(TEXT("Leader is the GDD 2.8 criteria-order winner over snapshot fields"),
		static_cast<int32>(Model.Leader), static_cast<int32>(Expected.Column));
	TestEqual(TEXT("LeadingCriterion names the deciding row"),
		Model.LeadingCriterion, Expected.Criterion);

	// AT MOST ONE CHEVRON, on any position. §2.11.4 draws one marker beside the leading
	// side's value; two would state that the tiebreak was decided twice.
	int32 ChevronRows = INDEX_NONE;
	int32 ChevronCount = 0;
	for (int32 RowIndex = 0; RowIndex < Model.Rows.Num(); ++RowIndex)
	{
		if (Model.Rows[RowIndex].Chevron != EStratScoreColumn::None)
		{
			++ChevronCount;
			ChevronRows = RowIndex;
		}
	}
	TestTrue(TEXT("at most one row carries the chevron"), ChevronCount <= 1);

	if (Expected.Criterion == INDEX_NONE)
	{
		TestEqual(TEXT("all three criteria tied, so no row carries a chevron"), ChevronCount, 0);
	}
	else
	{
		TestEqual(TEXT("the deciding row carries the chevron"), ChevronRows, Expected.Criterion);
		if (ChevronRows != INDEX_NONE)
		{
			TestEqual(TEXT("the row's chevron column is the model's Leader"),
				static_cast<int32>(Model.Rows[ChevronRows].Chevron),
				static_cast<int32>(Model.Leader));

			// The chevron sits beside the HIGHER value -- §2.8: higher wins. Read off the
			// row's own two bound values, which the binding clauses above have already
			// tied to snapshot fields.
			const FStratScoreboardRow& Deciding = Model.Rows[ChevronRows];
			const bool bYouLeads = (Deciding.Chevron == EStratScoreColumn::You);
			TestTrue(TEXT("the chevron marks the higher value at the deciding criterion"),
				bYouLeads ? (Deciding.YouValue > Deciding.EnemyValue)
				          : (Deciding.EnemyValue > Deciding.YouValue));
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// "-- no engagements --" -- §2.11.4's mutual-passivity case.
//
// The flag mirrors exactly one condition: both sides' `fameCombat` at zero. It is the
// passivity draw (§2.8) made visible BEFORE it bites, which is why it is a distinct
// statement from "nobody leads yet" -- a match can have no leader at criterion 1 and a
// clear one at criterion 2, and only the first of those gets this text. That
// distinction is asserted here rather than left to reading: the flag must not suppress
// a chevron sitting on a later criterion.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratScoreboardNoEngagementsTest,
	"Stratocracy.StratUI.T-UI-03.NoEngagementsMirrorsZeroCombatFame",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratScoreboardNoEngagementsTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratScoreboardParity;

	FStratBridge Bridge;
	strat::UiSnapshot Snapshot;
	FStratScoreboardModel Model;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds and projects"), Project(Bridge, kViewingSide, Snapshot, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!RequireTwoSides(*this, Snapshot) || !RequireThreeRows(*this, Model))
	{
		return false;
	}

	const int32 EnemySide = OtherSide(Snapshot, kViewingSide);
	const bool bBothZero =
		(Snapshot.side[kViewingSide].fameCombat == 0) && (Snapshot.side[EnemySide].fameCombat == 0);

	TestEqual(TEXT("bNoEngagements mirrors both sides' fameCombat being zero"),
		Model.bNoEngagements, bBothZero);

	// The flag is about the Destroyed row's DISPLAY, not about the tiebreak. Two zeroes
	// are tied, so criterion 1 cannot be the deciding one while the flag is set -- but a
	// later criterion still can be, and must not be suppressed. §1.5-#1 depends on that:
	// a lead on objectives is still a lead, and this panel exists so no lead is hidden.
	if (Model.bNoEngagements)
	{
		TestNotEqual(TEXT("criterion 1 cannot decide the lead when both sides are at zero"),
			Model.LeadingCriterion, static_cast<int32>(EStratScoreCriterion::CombatFame));
		TestEqual(TEXT("the Destroyed row carries no chevron when both sides are at zero"),
			static_cast<int32>(Model.Rows[static_cast<int32>(EStratScoreCriterion::CombatFame)].Chevron),
			static_cast<int32>(EStratScoreColumn::None));
	}

	return true;
}

// ---------------------------------------------------------------------------
// Columns follow the VIEWING side, not `sideToMove`.
//
// StratScoreboardWidget.h is explicit that the snapshot's `sideToMove` is whose turn it
// is and not whose screen it is, and that the two differ every other turn. A widget that
// sourced its YOU column from `sideToMove` would swap the whole panel mid-match without
// anything failing -- the numbers would still all be real snapshot fields, which is
// exactly why "binds 1:1" needs this clause to mean something: the right field for the
// wrong side is still wrong.
//
// Built twice from one bridge, so both models project the same unchanged state.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratScoreboardColumnsFollowViewingSideTest,
	"Stratocracy.StratUI.T-UI-03.ColumnsFollowViewingSide",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratScoreboardColumnsFollowViewingSideTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratScoreboardParity;

	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	strat::UiSnapshot Snapshot;
	const FStratResult Projected = Bridge.MakeUiSnapshot(Snapshot);
	if (!TestTrue(TEXT("MakeUiSnapshot succeeds on a seeded bridge"), Projected.bOk))
	{
		AddError(Projected.Reason);
		return false;
	}
	if (!RequireTwoSides(*this, Snapshot))
	{
		return false;
	}

	FStratScoreboardModel FirstView;
	FStratScoreboardModel SecondView;
	if (!TestTrue(TEXT("model builds for side 0"),
			StratBuildScoreboardModel(Bridge, 0, FirstView, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("model builds for side 1"),
			StratBuildScoreboardModel(Bridge, 1, SecondView, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!RequireThreeRows(*this, FirstView) || !RequireThreeRows(*this, SecondView))
	{
		return false;
	}

	for (int32 RowIndex = 0; RowIndex < FirstView.Rows.Num(); ++RowIndex)
	{
		const FStratScoreboardRow& FromZero = FirstView.Rows[RowIndex];
		const FStratScoreboardRow& FromOne  = SecondView.Rows[RowIndex];

		TestEqual(
			FString::Printf(TEXT("row %d: side 1's ENEMY column is side 0's YOU column"), RowIndex),
			FromOne.EnemyValue, FromZero.YouValue);
		TestEqual(
			FString::Printf(TEXT("row %d: side 1's YOU column is side 0's ENEMY column"), RowIndex),
			FromOne.YouValue, FromZero.EnemyValue);

		// The shared N is a property of the board, not of who is looking at it.
		TestEqual(
			FString::Printf(TEXT("row %d: the objective total does not depend on the viewer"), RowIndex),
			FromOne.Total, FromZero.Total);
	}

	// The turn line is the same panel-wide fact from either seat.
	TestEqual(TEXT("turn does not depend on the viewer"), SecondView.Turn, FirstView.Turn);
	TestEqual(TEXT("turn cap does not depend on the viewer"), SecondView.TurnCap, FirstView.TurnCap);

	// The chevron is expressed in the VIEWER's terms, so a lead flips columns between
	// the two seats while naming the same criterion. `None` mirrors itself.
	TestEqual(TEXT("both seats agree on which criterion decides"),
		SecondView.LeadingCriterion, FirstView.LeadingCriterion);
	if (FirstView.Leader == EStratScoreColumn::None)
	{
		TestEqual(TEXT("no leader from either seat"),
			static_cast<int32>(SecondView.Leader), static_cast<int32>(EStratScoreColumn::None));
	}
	else
	{
		const EStratScoreColumn Mirrored = (FirstView.Leader == EStratScoreColumn::You)
			? EStratScoreColumn::Enemy
			: EStratScoreColumn::You;
		TestEqual(TEXT("the leading side is the same side seen from the other seat"),
			static_cast<int32>(SecondView.Leader), static_cast<int32>(Mirrored));
	}

	// The passivity statement is about both sides' combat Fame, so it cannot depend on
	// which of them is looking.
	TestEqual(TEXT("bNoEngagements does not depend on the viewer"),
		SecondView.bNoEngagements, FirstView.bNoEngagements);

	return true;
}

// ---------------------------------------------------------------------------
// A refusal is not an empty scoreboard.
//
// The bridge refuses to project rather than handing back an empty snapshot, and its
// header gives the reason: a caller must not be able to mistake "not seeded yet" for "a
// match where both sides have destroyed nothing". On this panel that distinction has
// teeth -- the second of those two gets §2.11.4's "-- no engagements --" text and the
// mutual-passivity draw behind it, and a scoreboard that rendered three confident zeroes
// during startup would be asserting a draw-in-progress that nobody is playing.
//
// The model must also be left ALONE on failure, which is `Refresh`'s promise: a
// transient refusal must not blank standings that were correct a moment ago.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratScoreboardRefusesWithoutProjectionTest,
	"Stratocracy.StratUI.T-UI-03.RefusesRatherThanShowingZeroes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratScoreboardRefusesWithoutProjectionTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratScoreboardParity;

	// Never loaded, never seeded.
	FStratBridge Bridge;

	FStratScoreboardModel Model;
	Model.Turn = INDEX_NONE;   // A value no successful build would produce, so that
	Model.TurnCap = INDEX_NONE; // "untouched" is distinguishable from "written with 0".

	FString Reason;
	const bool bBuilt = StratBuildScoreboardModel(Bridge, kViewingSide, Model, Reason);

	TestFalse(TEXT("the builder refuses when there is nothing to project"), bBuilt);
	TestFalse(TEXT("the refusal carries the bridge's own reason"), Reason.IsEmpty());
	TestEqual(TEXT("a refused build leaves the caller's model untouched (Turn)"),
		Model.Turn, static_cast<int32>(INDEX_NONE));
	TestEqual(TEXT("a refused build leaves the caller's model untouched (TurnCap)"),
		Model.TurnCap, static_cast<int32>(INDEX_NONE));
	TestEqual(TEXT("a refused build produces no rows to draw"), Model.Rows.Num(), 0);

	// An out-of-range viewing side is the other refusal path: the panel has two columns,
	// and there is no side index the builder could substitute that would not be an
	// invention about which belligerent the player is.
	FStratBridge Seeded;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds"), SeedBridge(Seeded, Error)))
	{
		AddError(Error);
		return false;
	}

	strat::UiSnapshot Snapshot;
	if (!TestTrue(TEXT("MakeUiSnapshot succeeds on a seeded bridge"), Seeded.MakeUiSnapshot(Snapshot).bOk))
	{
		return false;
	}
	const int32 SideCount = static_cast<int32>(UE_ARRAY_COUNT(Snapshot.side));

	FStratScoreboardModel OutOfRange;
	FString OutOfRangeReason;
	TestFalse(TEXT("a viewing side outside the snapshot's sides is refused"),
		StratBuildScoreboardModel(Seeded, SideCount, OutOfRange, OutOfRangeReason));
	TestFalse(TEXT("that refusal names a reason"), OutOfRangeReason.IsEmpty());

	return true;
}

// ---------------------------------------------------------------------------
// The projection under the panel must not lie -- the rules module's own fidelity check.
//
// WHY A SCOREBOARD GATE RUNS THIS AT ALL. Every clause above establishes that the panel
// displays the snapshot faithfully. None of them can see whether the SNAPSHOT is
// faithful to the state it projects, and a scoreboard that faithfully displays a lying
// projection is indistinguishable, from the player's seat, from a scoreboard that lies
// -- which is the hidden win condition §1.5-#1 created this panel to abolish. So the
// module's own check runs over the same world and snapshot the builder consumed, and
// any failure it reports fails this clause with the module's own words.
//
// IT IS NOT REIMPLEMENTED HERE, and must not be: T-UI-05 owns that check as its subject
// and the rules module owns the check itself. This file calls it. A second copy of the
// fidelity rules living in a UI test would be the drift §4.9 exists to prevent, one
// module further out than usual.
//
// THE WORLD IS BORROWED. `MakeUiWorld` returns a world pointing into the bridge, and the
// bridge's header warns it must not outlive it or survive a LoadDefinitions. Both live
// in this stack frame and nothing reloads underneath them.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratScoreboardSnapshotFidelityTest,
	"Stratocracy.StratUI.T-UI-03.SnapshotFidelity",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratScoreboardSnapshotFidelityTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratScoreboardParity;

	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	strat::UiSnapshot Snapshot;
	const FStratResult Projected = Bridge.MakeUiSnapshot(Snapshot);
	if (!TestTrue(TEXT("MakeUiSnapshot succeeds on a seeded bridge"), Projected.bOk))
	{
		AddError(Projected.Reason);
		return false;
	}

	// The same gathering the bridge hands to `strat::buildUiSnapshot`, so the check sees
	// the projection's actual input rather than a second assembly of it.
	const strat::UiWorld World = Bridge.MakeUiWorld();

	TArray<FString> Failures;
	const bool bFaithful = RunSnapshotFidelity(World, Snapshot, Failures);

	// Reported one per line. A collapsed count would leave a reader of the log knowing
	// only that the panel is untrustworthy, not which field made it so.
	for (const FString& Failure : Failures)
	{
		AddError(FString::Printf(TEXT("uiCheckSnapshotFidelity: %s"), *Failure));
	}
	TestTrue(TEXT("the projection the scoreboard binds to is faithful to the state"), bFaithful);

	// The scoreboard is built from that same projection in the same frame, so a green
	// fidelity result and the binding clauses above together say the panel is telling
	// the truth -- which is the whole claim T-UI-03 is making.
	FStratScoreboardModel Model;
	FString BuildError;
	if (!TestTrue(TEXT("the scoreboard builds over the checked projection"),
			StratBuildScoreboardModel(Bridge, kViewingSide, Model, BuildError)))
	{
		AddError(BuildError);
		return false;
	}

	return true;
}
