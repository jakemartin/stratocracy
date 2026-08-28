// The gate on the FIELDS §2.11.2's info panel is drawn from -- the terrain facts on
// `FStratHexView` and the §2.4 stats on `FStratUnitView`, both added by wave 2.
//
// WHY `GATE-INFOPANEL` AND NOT AN ACCEPTANCE ID. §2.11.2's info panel has no acceptance ID
// in the GDD's set, and this project's standing convention for a surface the acceptance set
// does not grade is a `GATE-` prefix, which mints no ID and claims none. `GATE-BUILDMENU`
// carries §2.11.5's production menu the same way. Two IDs were considered and both were
// refused, and the refusals are recorded because they were nearly taken:
//
//   - `T-UI-03` is the STANDINGS SCOREBOARD -- "the live standings scoreboard (§2.11, §2.8)
//     binds 1:1 to snapshot fields -- enemy strength destroyed, objectives held X/N,
//     surviving units/HP, turn vs cap". Four side-level readouts. The info panel is a
//     different surface, and a clause riding an ID whose own text excludes its subject is
//     unfalsifiable by construction. That 42 clauses in this tree already carry `T-UI-03` is
//     not a licence to add a forty-third about something else.
//   - `T-UI-05` asks whether the snapshot tells the truth about the module's state. The four
//     snapshot mirrors this wave finally gives a reader -- `Hp`, `HpMax`, `bIsFlag`, `Owner`
//     -- are ALREADY asserted against the module in `StratViewModelParity.cpp`
//     (`.Hp == UiUnitView::hp`, `.HpMax == UiUnitView::hpMax`, `.bIsFlag ==
//     UiUnitView::isFlag`, `.Owner == UiHexView::owner`). A second clause asserting what an
//     existing one asserts is duplication, not coverage, and it is deliberately absent here.
//
// WHAT IS NEW, AND WHY IT IS NOT COVERED BY THE FILE NEXT DOOR. `StratViewModelParity.cpp`
// pins the view model as the SNAPSHOT restated in engine types, field for field. Not one of
// the nine fields below has a snapshot source: they are DEFINITION-TABLE reads, taken at the
// index the snapshot carries, in the same loop and off the same row as `TerrainId` and
// `DefId`. That file's own header names its subject as the snapshot mirror and gives the two
// `FName` table reads a clause of their own for exactly this reason -- "their expectation
// comes from the definition tables rather than from the snapshot". The nine below are NINE
// MORE OF THAT KIND -- four on `FStratHexView` and five on `FStratUnitView` -- so they are
// asserted the way that clause asserts, and here rather than by widening a walk whose subject
// is the mirror.
//
// [CORRECTED 2026-08-28. THE SENTENCE ABOVE READ "These nine are seven more of that kind",
// which is arithmetically impossible on its own terms and is struck rather than quietly
// patched: "seven" survived from this struct's narrower FIRST SHAPE, recorded on
// `FStratInfoPanelView`'s own block as having been built twice on 2026-08-27. THE FIGURE WAS
// RE-DERIVED FROM THE TREE and not taken from the report that flagged it --
// `git diff master -- Source/StratUI/StratViewModel.h` adds exactly four reflected fields to
// `FStratHexView` (`TerrainMoveCost`, `TerrainDefensePct`, `bTerrainCapturable`,
// `TerrainIncomeFame`) and five to `FStratUnitView` (`StatAtk`, `StatDef`, `StatMove`,
// `StatRangeMin`, `StatRangeMax`); every other field that diff adds belongs to
// `FStratInfoPanelView`, which is a new struct and not one of these. A COUNT WRITTEN INSIDE A
// GROWING THING IS THE RECORDED FAILURE HERE, so the two figures above are stated as the
// split that produces them rather than as a bare total a later field would silently falsify.]
//
// WHERE EVERY EXPECTATION COMES FROM. `FStratBridge::Tables()`, which hands back the
// definition vectors the bridge mapped from `DT_Units` and `DT_Terrain` -- the same bytes the
// rules module resolves scenarios by. There is no move cost, no defence bonus, no income
// rate, no attack value and no range bound written in this file, and no terrain or unit name
// either. A table edit moves both sides of every comparison together.
//
// THE GUARDS ARE ALSO TABLE READS AND THAT IS DELIBERATE. Several clauses below assert that
// the projected board actually EXERCISES a distinction -- that some row is impassable, that
// some defence bonus is negative, that some unit's range band is not a point. Every one of
// those facts is read off the row at the index the snapshot carries and none is typed here,
// so a scenario that stopped exercising a distinction reddens the guard rather than quietly
// making the comparison above it vacuous.
//
// WHY THIS FILE IS IN StratUI/Tests. It compares `FStratViewModel` (declared in StratUI, so
// no StratBridge test could see it) against `strat::UiSnapshot` and `strat::RulesTables`
// obtained through `FStratBridge` methods. NAMING a vendored type costs no symbol; CALLING a
// vendored free function is `LNK2019`. Nothing below calls one. `StratViewModelParity.cpp` in
// this same directory settled that and its header states the measurement.
//
// NOT ASSERTED HERE:
//   - `StratDecorateInfoPanel` and `FStratInfoPanelView`. That is the decorator's own gate,
//     `StratInfoPanelClauses.cpp` in this directory. What is pinned here is that the numbers
//     the panel selects are the table's before any selection happens.
//   - The four snapshot mirrors. See the `T-UI-05` note above.
//   - Any widget. Nothing below constructs one.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratViewModel.h"

// From a .cpp and never from a UHT-parsed header -- StratViewModel.h's own constraint.
#include "StratBridge.h"

// Named explicitly rather than taken transitively, because this file reads their types
// directly: `strat::UiSnapshot` field indices, and the two definition rows.
#include "Data.h"
#include "Ui.h"

namespace StratInfoPanelSourceParity
{
	/** The side the harness seeds with, on `StratViewModelParity.cpp`'s stated precedent. */
	static const int32 kFirstSide = 0;

	/** The viewing side the model is built for. NOT `sideToMove`. */
	static const int32 kViewingSide = 0;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	/**
	 * Brings a bridge to the seeded opening. Copied in shape from
	 * `StratViewModelParity.cpp`'s helper of the same name, including its reason for
	 * returning an error rather than asserting: a missing asset becomes a named failure in
	 * the caller's own clause list instead of a crash in a helper.
	 *
	 * The two `/Game/` literals are the automation-fixture exception
	 * `.agents/ue-project-context.md` states, and the scenario path is the shipped file.
	 */
	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		UDataTable* const Units   = LoadObject<UDataTable>(nullptr, TEXT("/Game/StratData/DT_Units.DT_Units"));
		UDataTable* const Terrain = LoadObject<UDataTable>(nullptr, TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
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

	/** Seeds, projects a snapshot, and builds the model -- two projections of one state,
	 *  with no command submitted between them. `StratViewModelParity.cpp` states why that
	 *  is safe rather than hoped for. */
	static bool Project(
		FStratBridge&      Bridge,
		strat::UiSnapshot& OutSnapshot,
		FStratViewModel&   OutModel,
		FString&           OutError)
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
		if (!StratBuildViewModel(Bridge, kViewingSide, OutModel, OutError))
		{
			OutError = FString::Printf(TEXT("StratBuildViewModel refused: %s"), *OutError);
			return false;
		}
		return true;
	}
}

// ---------------------------------------------------------------------------
// CLAUSE 1 -- the four terrain facts are the terrain row's, at the index the snapshot
// carries.
//
// WHAT WOULD BE WRONG IF THIS WERE ABSENT. `TerrainMoveCost`, `TerrainDefensePct`,
// `bTerrainCapturable` and `TerrainIncomeFame` are the first numbers in this project to
// reach a hex view from the DEFINITION TABLE rather than from the snapshot, and none of them
// has a snapshot field to be checked against. A builder that read `defensePct` into
// `TerrainMoveCost` would produce a board on which every Woods hex costs 20 to enter, with
// every existing clause green -- the mirror walk does not look at these fields and cannot.
//
// THE SIGN IS PART OF THE CLAIM. `Data.h` marks `defensePct` `SIGNED -- Bridge is -10
// (§2.3)`, and an `unsigned`-shaped copy would turn a river crossing's penalty into a large
// bonus. The guard below asserts the board actually contains a negative row before the
// comparison is allowed to certify anything about signs.
//
// THE SENTINEL IS CARRIED AND NOT INTERPRETED HERE. `moveCost == 0` means impassable, and
// this clause only says the ZERO arrived; what the panel does with it is
// `StratInfoPanelClauses.cpp`'s.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratInfoPanelHexTerrainFactsAreTheRowsTest,
	"Stratocracy.StratUI.GATE-INFOPANEL.HexTerrainFactsAreTheTerrainRowAtTheSnapshotsIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratInfoPanelHexTerrainFactsAreTheRowsTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratInfoPanelSourceParity;

	FStratBridge      Bridge;
	strat::UiSnapshot Snapshot;
	FStratViewModel   Model;
	FString           Error;
	if (!TestTrue(TEXT("bridge seeds, projects and builds"), Project(Bridge, Snapshot, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	const strat::RulesTables Tables = Bridge.Tables();
	if (!TestTrue(TEXT("the bridge holds a terrain table to read rows from"), Tables.terrain != nullptr))
	{
		return false;
	}
	if (!TestEqual(TEXT("Hexes.Num() == UiSnapshot::hexes.size()"),
			Model.Hexes.Num(), static_cast<int32>(Snapshot.hexes.size())))
	{
		return false;
	}
	if (!TestTrue(TEXT("the seeded scenario has hexes, so the loop below is not vacuous"),
			Model.Hexes.Num() > 0))
	{
		return false;
	}

	// The distinctions the board must actually exercise, all read off the rows themselves.
	TSet<int32> DistinctCost;
	TSet<int32> DistinctDefence;
	TSet<int32> DistinctIncome;
	bool bSawCapturable    = false;
	bool bSawNonCapturable = false;
	bool bSawNegativeBonus = false;
	bool bSawZeroCost      = false;

	for (int32 Index = 0; Index < Model.Hexes.Num(); ++Index)
	{
		const int32 Row = static_cast<int32>(Snapshot.hexes[Index].terrainId);
		if (!TestTrue(FString::Printf(TEXT("Hexes[%d] names a terrain row the table has"), Index),
				Row >= 0 && Row < static_cast<int32>(Tables.terrain->size())))
		{
			continue;
		}

		// THE EXPECTATION: the definition row at the index the snapshot carries. Not a
		// literal, not a switch, not a table written here.
		const strat::TerrainDef& Def    = (*Tables.terrain)[Row];
		const FStratHexView&     Actual = Model.Hexes[Index];
		const FString Where = FString::Printf(TEXT("Hexes[%d] (terrain row %d)"), Index, Row);

		TestEqual(Where + TEXT(".TerrainMoveCost is TerrainDef::moveCost"),
			Actual.TerrainMoveCost, static_cast<int32>(Def.moveCost));
		TestEqual(Where + TEXT(".TerrainDefensePct is TerrainDef::defensePct, sign and all"),
			Actual.TerrainDefensePct, static_cast<int32>(Def.defensePct));
		TestEqual(Where + TEXT(".bTerrainCapturable is TerrainDef::capturable"),
			Actual.bTerrainCapturable, Def.capturable);
		TestEqual(Where + TEXT(".TerrainIncomeFame is TerrainDef::incomeFame"),
			Actual.TerrainIncomeFame, static_cast<int32>(Def.incomeFame));

		DistinctCost.Add(static_cast<int32>(Def.moveCost));
		DistinctDefence.Add(static_cast<int32>(Def.defensePct));
		DistinctIncome.Add(static_cast<int32>(Def.incomeFame));
		bSawCapturable    = bSawCapturable    || Def.capturable;
		bSawNonCapturable = bSawNonCapturable || !Def.capturable;
		bSawNegativeBonus = bSawNegativeBonus || (Def.defensePct < 0);
		bSawZeroCost      = bSawZeroCost      || (Def.moveCost == 0);
	}

	// ---- The comparisons above must have had something to distinguish ------
	// A board of one terrain kind would satisfy every equality above while a builder that
	// wrote a constant, or that crossed two fields, went unseen. Each figure below is the
	// TABLE'S and none is typed here.
	TestTrue(TEXT("the board exercises more than one move cost"), DistinctCost.Num() > 1);
	TestTrue(TEXT("the board exercises more than one defence bonus"), DistinctDefence.Num() > 1);
	TestTrue(TEXT("the board exercises more than one income rate"), DistinctIncome.Num() > 1);
	TestTrue(TEXT("the board carries at least one capturable hex"), bSawCapturable);
	TestTrue(TEXT("the board carries at least one non-capturable hex"), bSawNonCapturable);
	TestTrue(TEXT("the board carries a NEGATIVE defence bonus, so the sign is observable"),
		bSawNegativeBonus);
	TestTrue(TEXT("the board carries a zero-cost (impassable) row, so the sentinel is observable"),
		bSawZeroCost);

	return true;
}

// ---------------------------------------------------------------------------
// CLAUSE 2 -- the five §2.4 stats are the unit row's, at that unit's own `DefIndex`.
//
// THE RANGE BAND IS THE TRAP AND IT IS THE REASON THIS CLAUSE IS NOT A LOOP OVER ANY UNIT.
// `strat::UnitDef` carries `rangeMin` AND `rangeMax`, and in the shipped table three of the
// four rows are a POINT band -- only one row has `rangeMin != rangeMax`. A clause that
// happened to walk only point-band units would pass identically over a builder that wrote
// `StatRangeMin = rangeMax`, collapsing artillery's minimum, which is §2.6's rule and not a
// display detail. The guard below therefore asserts, off the TABLE, that the projection
// contains a unit whose band is not a point -- and the per-unit comparison then has
// something a collapse would break.
//
// `StatHpMax` IS ABSENT FROM THE MODEL ON PURPOSE and so it is absent here. `HpMax` is the
// SNAPSHOT's per-instance maximum and is already pinned in `StratViewModelParity.cpp`; a
// second copy off the definition row would be a second author of one fact.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratInfoPanelUnitStatsAreTheRowsTest,
	"Stratocracy.StratUI.GATE-INFOPANEL.UnitStatsAreTheUnitRowAtItsDefIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratInfoPanelUnitStatsAreTheRowsTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratInfoPanelSourceParity;

	FStratBridge      Bridge;
	strat::UiSnapshot Snapshot;
	FStratViewModel   Model;
	FString           Error;
	if (!TestTrue(TEXT("bridge seeds, projects and builds"), Project(Bridge, Snapshot, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	const strat::RulesTables Tables = Bridge.Tables();
	if (!TestTrue(TEXT("the bridge holds a unit table to read rows from"), Tables.units != nullptr))
	{
		return false;
	}
	if (!TestEqual(TEXT("Units.Num() == UiSnapshot::units.size()"),
			Model.Units.Num(), static_cast<int32>(Snapshot.units.size())))
	{
		return false;
	}
	if (!TestTrue(TEXT("the seeded scenario has units, so the loop below is not vacuous"),
			Model.Units.Num() > 0))
	{
		return false;
	}

	bool bSawWideBand    = false;   // rangeMin != rangeMax -- the artillery row
	bool bSawPointBand   = false;   // rangeMin == rangeMax -- everything else
	bool bSawAtkNotDef   = false;   // so a swapped pair is visible
	bool bSawMoveNotAtk  = false;   // so a third crossing is visible

	for (int32 Index = 0; Index < Model.Units.Num(); ++Index)
	{
		const int32 Row = static_cast<int32>(Snapshot.units[Index].unitId);
		if (!TestTrue(FString::Printf(TEXT("Units[%d] names a definition row the table has"), Index),
				Row >= 0 && Row < static_cast<int32>(Tables.units->size())))
		{
			continue;
		}

		// THE EXPECTATION: the §2.4 row at the index the snapshot carries.
		const strat::UnitDef& Def    = (*Tables.units)[Row];
		const FStratUnitView& Actual = Model.Units[Index];
		const FString Where = FString::Printf(TEXT("Units[%d] (definition row %d)"), Index, Row);

		TestEqual(Where + TEXT(".StatAtk is UnitDef::atk"),
			Actual.StatAtk, static_cast<int32>(Def.atk));
		TestEqual(Where + TEXT(".StatDef is UnitDef::def"),
			Actual.StatDef, static_cast<int32>(Def.def));
		TestEqual(Where + TEXT(".StatMove is UnitDef::move"),
			Actual.StatMove, static_cast<int32>(Def.move));
		TestEqual(Where + TEXT(".StatRangeMin is UnitDef::rangeMin"),
			Actual.StatRangeMin, static_cast<int32>(Def.rangeMin));
		TestEqual(Where + TEXT(".StatRangeMax is UnitDef::rangeMax"),
			Actual.StatRangeMax, static_cast<int32>(Def.rangeMax));

		// The stats and the name must name the SAME row, or two presentation layers can
		// disagree about one unit while every equality above holds.
		TestEqual(Where + TEXT(": DefIndex is the row the stats were read from"),
			Actual.DefIndex, Row);

		bSawWideBand   = bSawWideBand   || (Def.rangeMin != Def.rangeMax);
		bSawPointBand  = bSawPointBand  || (Def.rangeMin == Def.rangeMax);
		bSawAtkNotDef  = bSawAtkNotDef  || (Def.atk != Def.def);
		bSawMoveNotAtk = bSawMoveNotAtk || (Def.move != Def.atk);
	}

	// ---- The guards, every figure read off the table --------------------------
	TestTrue(
		TEXT("the projection contains a unit whose range band is NOT a point, so a collapsed "
		     "band (StatRangeMin written from rangeMax, or the reverse) is observable"),
		bSawWideBand);
	TestTrue(
		TEXT("and one whose band IS a point, so the wide band is not the only shape asserted"),
		bSawPointBand);
	TestTrue(TEXT("some unit's attack differs from its defence, so a swapped pair is observable"),
		bSawAtkNotDef);
	TestTrue(TEXT("some unit's move differs from its attack, so a third crossing is observable"),
		bSawMoveNotAtk);

	return true;
}
