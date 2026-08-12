// The gate for GDD §4.7 Stub 8 / §4.9's reflected view model -- T-INT-05, plus the two
// T-UI-03 clauses that belong to the hot-seat viewing-side seam.
//
//   T-INT-05  the screen is rebuildable FROM THE VIEW MODEL ALONE, and the view model is
//             the snapshot in engine types with nothing transformed beyond a width cast.
//
// WHY THIS FILE LIVES IN StratUI/Tests AND NOT IN StratBridge/Tests, decided before it
// was written rather than discovered as a link error. Two of the clauses below compare
// `FStratViewModel` against a `strat::UiSnapshot` built from the SAME bridge, and the
// obvious worry is that a module which may not call `strat::` cannot obtain a snapshot.
// It can, and the distinction is the linker's:
//
//   - NAMING a vendored type (`strat::UiSnapshot`, `strat::RulesTables`) costs no symbol.
//   - CALLING a vendored free function (`strat::buildUiSnapshot`) is LNK2019, because the
//     vendored sources carry no `_API` macro. Measured 8x when the bridge first tried,
//     and once more when an earlier revision of Source/StratUI/Tests/StratScoreboardParity.cpp
//     called `strat::uiCheckSnapshotFidelity` directly.
//
// Every rules-side value below arrives through a METHOD on `FStratBridge` --
// `MakeUiSnapshot`, `Tables`, `StateHash`, `RecordedLog`, `Reachable`, `SubmitMove` --
// and `FStratBridge` is the one thing UnrealEditor-StratBridge.dll exports.
// StratScoreboardParity.cpp in this same directory already reads `strat::UiSnapshot`
// fields this way and links, which is the existing measurement that settled it. The
// subject under test (`StratBuildViewModel`) is declared in StratUI, so a StratBridge
// test could not call it at all: StratBridge is BELOW StratUI and the arrow does not
// reverse. This directory is therefore the only one that can see both sides.
//
// WHERE EVERY EXPECTATION COMES FROM. Not one number below is computed here. The
// comparison side of every assertion is either a field of the `strat::UiSnapshot` the
// same bridge projected, or a row of the definition tables the same bridge mapped,
// reached through `FStratBridge::Tables()`. There is no board size, no turn cap, no
// objective total, no side count, no terrain name and no unit name written in this file.
// A scenario or table edit moves both sides of every comparison together.
//
// THE ONE PLACE THAT IS NOT A FIELD READ is the ordinal parity between
// `EStratResultTier` and `strat::ResultTier`, which is asserted as `static_cast<int32>`
// equality. That is not a derivation of the expected value -- it is exactly the claim
// StratViewModel.h makes ("the enumerator ORDER is the vendored enum's") made checkable.
// Re-writing the builder's exhaustive switch here would have asserted that the code
// equals itself.
//
// NOT ASSERTED HERE, with reasons:
// - Whether the snapshot is faithful to the state it projects. That is
//   `strat::uiCheckSnapshotFidelity`, T-UI-05's subject, and
//   StratScoreboardParity.cpp already runs it through the bridge. A second opinion here
//   would leave two gates unable to say which of them was the authority.
// - Pixels, actors, and `ApplyView`. The builder is a free function precisely so no
//   Slate tree and no actor need exist to see a value; nothing below constructs a widget.
// - The `bDone` / `bLockedThisTurn` OWNERS. Phase 3's selection machine and guidance
//   layer do not exist. What is pinned is that the builder does not invent them, which is
//   the PresentationBitsAreDefaulted clause.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratViewModel.h"

// The bridge, included from a .cpp and never from a UHT-parsed header -- the same
// constraint StratViewModel.h records about itself. A test file is a .cpp, so it may say
// this.
#include "StratBridge.h"

// The vendored view-model header, by short name via StratRules' PublicIncludePaths.
// Reached through StratBridge.h already; named explicitly because this file reads
// `strat::UiSnapshot` fields directly and a reader should not have to discover that
// through a transitive include.
#include "Ui.h"

namespace StratViewModelParity
{
	// MUST match the side the rest of the engine-side harness seeds with. Replay.h states
	// that no rule in this project decides which side moves first, so this is a harness
	// pin and not a rule. StratScoreboardParity.cpp and StratBridgeParity.cpp pin the same
	// value for the same stated reason.
	static const int32 kFirstSide = 0;

	// The viewing side these clauses build for by default. NOT `sideToMove`.
	static const int32 kViewingSide = 0;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/**
	 * The definition row `id` -> `FName` conversion, spelled once.
	 *
	 * THIS IS TRANSPORT, NOT AN EXPECTATION. The expected VALUE is the row's own `id`,
	 * read out of the table the bridge holds; this function only carries those bytes into
	 * the type the comparison is made in. StratViewModel.cpp performs the same conversion
	 * for the same reason, and a difference between the two would be a difference about
	 * encoding rather than about which row was named.
	 */
	static FName DefinitionName(const std::string& Id)
	{
		return FName(UTF8_TO_TCHAR(Id.c_str()));
	}

	/**
	 * Brings a bridge to the seeded opening: the shipped tables mapped in, the shipped
	 * scenario seeded through the rules module's own loader.
	 *
	 * Returns false and fills OutError rather than asserting, on StratScoreboardParity's
	 * precedent: a missing asset becomes a named failure inside the caller's own clause
	 * list instead of a crash in a helper.
	 *
	 * NOTHING IS HARDCODED PAST THE ASSET PATHS -- automation fixtures are one of the two
	 * places `.agents/ue-project-context.md` licenses a `/Game/` literal, because a test
	 * has no Blueprint default to hold the property and fails loudly rather than silently.
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
	 * Seeds a bridge, projects a snapshot, and builds the view model for `ViewingSide`.
	 *
	 * TWO PROJECTIONS OF ONE STATE, safe here for a stated reason rather than by hope: no
	 * command is submitted between them, so the authoritative `GameState` is identical
	 * across both calls. The alternative -- having the builder hand its snapshot back out
	 * -- would widen the production surface purely to suit a test.
	 */
	static bool Project(
		FStratBridge&      Bridge,
		int32              ViewingSide,
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

		if (!StratBuildViewModel(Bridge, ViewingSide, OutModel, OutError))
		{
			OutError = FString::Printf(TEXT("StratBuildViewModel refused: %s"), *OutError);
			return false;
		}
		return true;
	}

	/**
	 * A view model pre-filled with values no successful build produces, so that
	 * "untouched" is distinguishable from "written with a default".
	 *
	 * `INDEX_NONE` for the two int32s and one bogus entry in each array. A refusal that
	 * had partially written the model would clear or overwrite at least one of these.
	 */
	static FStratViewModel Sentinel()
	{
		FStratViewModel Model;
		Model.ViewingSide    = INDEX_NONE;
		Model.ObjectiveTotal = INDEX_NONE;
		Model.Match.Turn     = INDEX_NONE;
		Model.Hexes.AddDefaulted();
		Model.Units.AddDefaulted();
		Model.Factories.AddDefaulted();
		Model.Sides.AddDefaulted();
		return Model;
	}

	/** Fails the clause unless the refusal left every sentinel exactly as it was. */
	static void RequireUntouched(FAutomationTestBase& Test, const FStratViewModel& Model, const TCHAR* Which)
	{
		Test.TestEqual(FString::Printf(TEXT("%s: ViewingSide untouched"), Which),
			Model.ViewingSide, static_cast<int32>(INDEX_NONE));
		Test.TestEqual(FString::Printf(TEXT("%s: ObjectiveTotal untouched"), Which),
			Model.ObjectiveTotal, static_cast<int32>(INDEX_NONE));
		Test.TestEqual(FString::Printf(TEXT("%s: Match.Turn untouched"), Which),
			Model.Match.Turn, static_cast<int32>(INDEX_NONE));
		Test.TestEqual(FString::Printf(TEXT("%s: Hexes untouched"), Which), Model.Hexes.Num(), 1);
		Test.TestEqual(FString::Printf(TEXT("%s: Units untouched"), Which), Model.Units.Num(), 1);
		Test.TestEqual(FString::Printf(TEXT("%s: Factories untouched"), Which), Model.Factories.Num(), 1);
		Test.TestEqual(FString::Printf(TEXT("%s: Sides untouched"), Which), Model.Sides.Num(), 1);
	}

	/** Axial equality against the snapshot's own `strat::Hex`, X = q and Y = r. */
	static void RequireHexEqual(
		FAutomationTestBase& Test, const FString& What, const FIntPoint& Actual, const strat::Hex& Expected)
	{
		Test.TestEqual(What + TEXT(" (q -> X)"), Actual.X, static_cast<int32>(Expected.q));
		Test.TestEqual(What + TEXT(" (r -> Y)"), Actual.Y, static_cast<int32>(Expected.r));
	}
}

// ---------------------------------------------------------------------------
// Every field of every group, against the snapshot the same bridge projected.
//
// THIS IS THE WHOLE OF "NOTHING IS TRANSFORMED BEYOND A WIDTH CAST". The view model's
// job is to restate `strat::UiSnapshot` in engine types so that phase 3 never has to name
// a vendored one; a field that quietly changed meaning on the way across would give the
// board a second author, and reconciliation against a value with two authors is how a
// screen drifts from the rules while every individual number still looks real.
//
// The two `FName` fields are NOT compared here -- they are the one addition the builder
// makes and they have a clause of their own, because their expectation comes from the
// definition tables rather than from the snapshot.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratViewModelMirrorsSnapshotTest,
	"Stratocracy.StratUI.T-INT-05.MirrorsSnapshotFieldForField",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratViewModelMirrorsSnapshotTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratViewModelParity;

	FStratBridge Bridge;
	strat::UiSnapshot Snapshot;
	FStratViewModel Model;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds, projects and builds"), Project(Bridge, kViewingSide, Snapshot, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- Group sizes. A short array is a board with hexes missing. ----------
	const int32 SideCount = static_cast<int32>(UE_ARRAY_COUNT(Snapshot.side));
	TestEqual(TEXT("Hexes.Num() == UiSnapshot::hexes.size()"),
		Model.Hexes.Num(), static_cast<int32>(Snapshot.hexes.size()));
	TestEqual(TEXT("Units.Num() == UiSnapshot::units.size()"),
		Model.Units.Num(), static_cast<int32>(Snapshot.units.size()));
	TestEqual(TEXT("Factories.Num() == UiSnapshot::factories.size()"),
		Model.Factories.Num(), static_cast<int32>(Snapshot.factories.size()));
	TestEqual(TEXT("Sides.Num() == the snapshot's own side count"), Model.Sides.Num(), SideCount);

	// A board with no hexes and no units would pass every per-element loop below
	// vacuously. The scenario supplies both; this says so out loud rather than letting an
	// empty projection certify the builder.
	TestTrue(TEXT("the seeded scenario has hexes to mirror"), Model.Hexes.Num() > 0);
	TestTrue(TEXT("the seeded scenario has units to mirror"), Model.Units.Num() > 0);
	TestTrue(TEXT("the seeded scenario has factories to mirror"), Model.Factories.Num() > 0);

	if (Model.Hexes.Num() != static_cast<int32>(Snapshot.hexes.size())
		|| Model.Units.Num() != static_cast<int32>(Snapshot.units.size())
		|| Model.Factories.Num() != static_cast<int32>(Snapshot.factories.size())
		|| Model.Sides.Num() != SideCount)
	{
		// Nothing below can mean anything once the groups are different lengths, and a
		// per-element loop over mismatched arrays reports the same defect ninety-nine
		// times.
		return false;
	}

	// ---- Match ------------------------------------------------------------
	TestEqual(TEXT("Match.Turn == UiMatchView::turn"),
		Model.Match.Turn, static_cast<int32>(Snapshot.match.turn));
	TestEqual(TEXT("Match.TurnCap == UiMatchView::turnCap"),
		Model.Match.TurnCap, static_cast<int32>(Snapshot.match.turnCap));
	TestEqual(TEXT("Match.SideToMove == UiMatchView::sideToMove"),
		Model.Match.SideToMove, static_cast<int32>(Snapshot.match.sideToMove));
	TestEqual(TEXT("Match.bHasResult == UiMatchView::hasResult"),
		Model.Match.bHasResult, Snapshot.match.hasResult);

	// ORDINAL PARITY, not a re-written switch. StratViewModel.h claims `EStratResultTier`
	// is pinned to `strat::ResultTier` one enumerator for one; this is that claim. A test
	// that restated the builder's mapping would be asserting the code equals itself.
	TestEqual(TEXT("Match.ResultTier has the same ordinal as UiMatchView::resultTier"),
		static_cast<int32>(Model.Match.ResultTier), static_cast<int32>(Snapshot.match.resultTier));

	TestEqual(TEXT("ObjectiveTotal == UiSnapshot::objectiveTotal"),
		Model.ObjectiveTotal, static_cast<int32>(Snapshot.objectiveTotal));

	// The caller's own statement, echoed back. The only field in the struct with no
	// snapshot source, which is why it is checked against what was asked for.
	TestEqual(TEXT("ViewingSide is the side that was asked for"), Model.ViewingSide, kViewingSide);

	// ---- Sides -------------------------------------------------------------
	for (int32 Index = 0; Index < SideCount; ++Index)
	{
		const strat::UiSideView& Source = Snapshot.side[Index];
		const FStratSideView&    Actual = Model.Sides[Index];
		const FString Where = FString::Printf(TEXT("Sides[%d]"), Index);

		TestEqual(Where + TEXT(".FameTotal == UiSideView::fameTotal"),
			Actual.FameTotal, static_cast<int32>(Source.fameTotal));
		TestEqual(Where + TEXT(".FameCombat == UiSideView::fameCombat"),
			Actual.FameCombat, static_cast<int32>(Source.fameCombat));
		TestEqual(Where + TEXT(".ObjectivesHeld == UiSideView::objectivesHeld"),
			Actual.ObjectivesHeld, static_cast<int32>(Source.objectivesHeld));
		TestEqual(Where + TEXT(".SurvivingHp == UiSideView::survivingHp"),
			Actual.SurvivingHp, static_cast<int32>(Source.survivingHp));
		TestEqual(Where + TEXT(".IncomePerTurn == UiSideView::incomePerTurn"),
			Actual.IncomePerTurn, static_cast<int32>(Source.incomePerTurn));
	}

	// ---- Hexes -------------------------------------------------------------
	for (int32 Index = 0; Index < Model.Hexes.Num(); ++Index)
	{
		const strat::UiHexView& Source = Snapshot.hexes[Index];
		const FStratHexView&    Actual = Model.Hexes[Index];
		const FString Where = FString::Printf(TEXT("Hexes[%d]"), Index);

		RequireHexEqual(*this, Where + TEXT(".Hex == UiHexView::hex"), Actual.Hex, Source.hex);
		TestEqual(Where + TEXT(".TerrainIndex == UiHexView::terrainId"),
			Actual.TerrainIndex, static_cast<int32>(Source.terrainId));
		TestEqual(Where + TEXT(".Owner == UiHexView::owner"),
			Actual.Owner, static_cast<int32>(Source.owner));
	}

	// ---- Units -------------------------------------------------------------
	for (int32 Index = 0; Index < Model.Units.Num(); ++Index)
	{
		const strat::UiUnitView& Source = Snapshot.units[Index];
		const FStratUnitView&    Actual = Model.Units[Index];
		const FString Where = FString::Printf(TEXT("Units[%d]"), Index);

		TestEqual(Where + TEXT(".UnitId == UiUnitView::id"),
			Actual.UnitId, static_cast<int32>(Source.id));
		TestEqual(Where + TEXT(".Side == UiUnitView::side"),
			Actual.Side, static_cast<int32>(Source.side));
		// `UiUnitView::unitId` is the §2.4 ROW INDEX whatever its spelling suggests; the
		// engine-side field is renamed `DefIndex` for what the rules module does with it.
		TestEqual(Where + TEXT(".DefIndex == UiUnitView::unitId (the row index)"),
			Actual.DefIndex, static_cast<int32>(Source.unitId));
		RequireHexEqual(*this, Where + TEXT(".Hex == UiUnitView::hex"), Actual.Hex, Source.hex);
		TestEqual(Where + TEXT(".Hp == UiUnitView::hp"), Actual.Hp, static_cast<int32>(Source.hp));
		TestEqual(Where + TEXT(".HpMax == UiUnitView::hpMax"), Actual.HpMax, static_cast<int32>(Source.hpMax));
		TestEqual(Where + TEXT(".bIsFlag == UiUnitView::isFlag"), Actual.bIsFlag, Source.isFlag);
		// TWO INDEPENDENT FLAGS (T-TURN-01), mirrored separately because one field cannot
		// express a unit that has spent exactly one of them.
		TestEqual(Where + TEXT(".bHasMoved == UiUnitView::hasMoved"), Actual.bHasMoved, Source.hasMoved);
		TestEqual(Where + TEXT(".bHasActed == UiUnitView::hasActed"), Actual.bHasActed, Source.hasActed);
		TestEqual(Where + TEXT(".CaptureProgress == UiUnitView::captureProgress"),
			Actual.CaptureProgress, static_cast<int32>(Source.captureProgress));
		TestEqual(Where + TEXT(".bIsGuidedMarked == UiUnitView::isGuidedMarked"),
			Actual.bIsGuidedMarked, Source.isGuidedMarked);
	}

	// ---- Factories ---------------------------------------------------------
	for (int32 Index = 0; Index < Model.Factories.Num(); ++Index)
	{
		const strat::UiFactoryView& Source = Snapshot.factories[Index];
		const FStratFactoryView&    Actual = Model.Factories[Index];
		const FString Where = FString::Printf(TEXT("Factories[%d]"), Index);

		RequireHexEqual(*this, Where + TEXT(".Hex == UiFactoryView::hex"), Actual.Hex, Source.hex);
		TestEqual(Where + TEXT(".Owner == UiFactoryView::owner"),
			Actual.Owner, static_cast<int32>(Source.owner));
		TestEqual(Where + TEXT(".bHasBuiltThisTurn == UiFactoryView::hasBuiltThisTurn"),
			Actual.bHasBuiltThisTurn, Source.hasBuiltThisTurn);
		TestEqual(Where + TEXT(".bBuildWaiting == UiFactoryView::buildWaiting"),
			Actual.bBuildWaiting, Source.buildWaiting);
		// DISTINCT FROM bBuildWaiting -- a boxed-in factory with nothing queued has this
		// true and that false, which one field cannot express.
		TestEqual(Where + TEXT(".bSpawnBlocked == UiFactoryView::spawnBlocked"),
			Actual.bSpawnBlocked, Source.spawnBlocked);
	}

	return true;
}

// ---------------------------------------------------------------------------
// Order -- the builder sorts nothing.
//
// WHY THIS IS A CLAUSE AND NOT A COMMENT. `strat::buildUiSnapshot` emits hexes and
// factories in canonical hex order and units by ascending id so that two runs over the
// same state produce the same bytes. Phase 3 reconciles actors against this value; a
// builder that sorted -- by terrain to batch draws, by side to group them, by anything --
// would discard a determinism property it did not pay for, and would do so without any
// number changing. Every comparison below is POSITIONAL: element i of the model against
// element i of the vector, which is the only shape of assertion that can see a reorder.
//
// WHAT THIS CANNOT SEE, said plainly: a builder that sorted into the SAME order the
// snapshot already has. That reorder is unobservable from outside and is also harmless,
// because the observable value is identical. Every order that differs -- which is every
// realistic sort -- moves at least one element and fails here.
//
// THE ORDERING PREDICATES BELOW ARE DOCUMENTATION WITH TEETH, not the expectation. They
// are applied to the SNAPSHOT as well as to the model, so they state what the module's
// order is rather than deciding it; the positional equality above them is what pins the
// builder.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratViewModelOrdersArePreservedTest,
	"Stratocracy.StratUI.T-INT-05.OrdersArePreserved",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratViewModelOrdersArePreservedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratViewModelParity;

	FStratBridge Bridge;
	strat::UiSnapshot Snapshot;
	FStratViewModel Model;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds, projects and builds"), Project(Bridge, kViewingSide, Snapshot, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	if (!TestEqual(TEXT("Hexes.Num() == UiSnapshot::hexes.size()"),
			Model.Hexes.Num(), static_cast<int32>(Snapshot.hexes.size()))
		|| !TestEqual(TEXT("Units.Num() == UiSnapshot::units.size()"),
			Model.Units.Num(), static_cast<int32>(Snapshot.units.size()))
		|| !TestEqual(TEXT("Factories.Num() == UiSnapshot::factories.size()"),
			Model.Factories.Num(), static_cast<int32>(Snapshot.factories.size())))
	{
		return false;
	}

	// ---- Positional identity: element i is element i --------------------------
	for (int32 Index = 0; Index < Model.Hexes.Num(); ++Index)
	{
		RequireHexEqual(*this,
			FString::Printf(TEXT("Hexes[%d] is UiSnapshot::hexes[%d]"), Index, Index),
			Model.Hexes[Index].Hex, Snapshot.hexes[Index].hex);
	}
	for (int32 Index = 0; Index < Model.Units.Num(); ++Index)
	{
		TestEqual(FString::Printf(TEXT("Units[%d] is UiSnapshot::units[%d]"), Index, Index),
			Model.Units[Index].UnitId, static_cast<int32>(Snapshot.units[Index].id));
	}
	for (int32 Index = 0; Index < Model.Factories.Num(); ++Index)
	{
		RequireHexEqual(*this,
			FString::Printf(TEXT("Factories[%d] is UiSnapshot::factories[%d]"), Index, Index),
			Model.Factories[Index].Hex, Snapshot.factories[Index].hex);
	}

	// ---- What that order IS, asserted of the module and of the mirror alike ----
	// Canonical hex order is ascending r, then ascending q (Hex.h:25-27). Units are by
	// ascending id. Stated here so a reader of a failure knows which property broke, and
	// applied to both sequences so this file is not the author of either.
	for (int32 Index = 1; Index < Model.Hexes.Num(); ++Index)
	{
		const strat::Hex& Previous = Snapshot.hexes[Index - 1].hex;
		const strat::Hex& Current  = Snapshot.hexes[Index].hex;
		const bool bModuleAscends =
			(Current.r > Previous.r) || (Current.r == Previous.r && Current.q > Previous.q);
		TestTrue(FString::Printf(
				TEXT("UiSnapshot::hexes is in canonical hex order at %d"), Index),
			bModuleAscends);

		const FIntPoint& Before = Model.Hexes[Index - 1].Hex;
		const FIntPoint& After  = Model.Hexes[Index].Hex;
		const bool bModelAscends =
			(After.Y > Before.Y) || (After.Y == Before.Y && After.X > Before.X);
		TestTrue(FString::Printf(
				TEXT("FStratViewModel::Hexes is still in canonical hex order at %d"), Index),
			bModelAscends);
	}

	for (int32 Index = 1; Index < Model.Units.Num(); ++Index)
	{
		TestTrue(FString::Printf(TEXT("UiSnapshot::units ascends by id at %d"), Index),
			Snapshot.units[Index].id > Snapshot.units[Index - 1].id);
		TestTrue(FString::Printf(TEXT("FStratViewModel::Units still ascends by id at %d"), Index),
			Model.Units[Index].UnitId > Model.Units[Index - 1].UnitId);
	}

	for (int32 Index = 1; Index < Model.Factories.Num(); ++Index)
	{
		const FIntPoint& Before = Model.Factories[Index - 1].Hex;
		const FIntPoint& After  = Model.Factories[Index].Hex;
		const bool bModelAscends =
			(After.Y > Before.Y) || (After.Y == Before.Y && After.X > Before.X);
		TestTrue(FString::Printf(
				TEXT("FStratViewModel::Factories is still in canonical hex order at %d"), Index),
			bModelAscends);
	}

	// More than one of each, or the ordering clauses above are vacuous.
	TestTrue(TEXT("there is more than one hex, so order is observable"), Model.Hexes.Num() > 1);
	TestTrue(TEXT("there is more than one unit, so order is observable"), Model.Units.Num() > 1);
	TestTrue(TEXT("there is more than one factory, so order is observable"), Model.Factories.Num() > 1);

	return true;
}

// ---------------------------------------------------------------------------
// The two FName fields -- the one thing the builder adds, and why it matters more than
// it looks.
//
// Phase 0 ruled `DT_Terrain` row order NOT load-bearing, on evidence: no SaveCommand
// field carries a terrain index, seeding resolves every hex's terrain BY NAME, and
// `canonicalStateBytes` emits no terrain field at all. A terrain index is therefore live
// within one build and meaningless across two. So if phase 3 keyed its meshes off
// `TerrainIndex`, a table reordered upstream would draw Water as Woods -- silently, on
// ninety-nine hexes, with every gate still green, because no gate pins that order.
// `TerrainId` is what stops that, and this clause is what makes `TerrainId` trustworthy.
//
// THE EXPECTATION IS A TABLE ROW, READ THROUGH THE BRIDGE. `FStratBridge::Tables()` hands
// back the definition vectors the bridge mapped from DT_Units and DT_Terrain -- the same
// bytes the rules module resolves scenarios by. There is not one terrain or unit name
// spelled in this file. A renamed row moves both sides of every comparison together.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratViewModelNamesResolveThroughTablesTest,
	"Stratocracy.StratUI.T-INT-05.NamesResolveThroughTheTables",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratViewModelNamesResolveThroughTablesTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratViewModelParity;

	FStratBridge Bridge;
	strat::UiSnapshot Snapshot;
	FStratViewModel Model;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds, projects and builds"), Project(Bridge, kViewingSide, Snapshot, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	// Borrowed and const, the way the builder itself takes them. The vectors live in the
	// bridge and both outlive this stack frame.
	const strat::RulesTables Tables = Bridge.Tables();
	if (!TestTrue(TEXT("the bridge holds a terrain table to name rows from"), Tables.terrain != nullptr)
		|| !TestTrue(TEXT("the bridge holds a unit table to name rows from"), Tables.units != nullptr))
	{
		return false;
	}
	if (!TestEqual(TEXT("Hexes.Num() == UiSnapshot::hexes.size()"),
			Model.Hexes.Num(), static_cast<int32>(Snapshot.hexes.size()))
		|| !TestEqual(TEXT("Units.Num() == UiSnapshot::units.size()"),
			Model.Units.Num(), static_cast<int32>(Snapshot.units.size())))
	{
		return false;
	}

	TSet<FName> DistinctTerrain;
	for (int32 Index = 0; Index < Model.Hexes.Num(); ++Index)
	{
		const int32 Row = static_cast<int32>(Snapshot.hexes[Index].terrainId);
		if (!TestTrue(FString::Printf(TEXT("Hexes[%d] names a terrain row the table has"), Index),
				Row >= 0 && Row < static_cast<int32>(Tables.terrain->size())))
		{
			continue;
		}

		// THE EXPECTATION: the definition row's own `id`, at the index the snapshot
		// carries. Not a literal, not a switch, not a map written here.
		const FName Expected = DefinitionName((*Tables.terrain)[Row].id);

		TestEqual(FString::Printf(
				TEXT("Hexes[%d].TerrainId is TerrainDef[%d].id, read through FStratBridge::Tables()"),
				Index, Row),
			Model.Hexes[Index].TerrainId, Expected);

		// A nameless hex would reconcile to whatever phase 3's mesh map returns for an
		// unknown key, which is the failure this field exists to prevent.
		TestNotEqual(FString::Printf(TEXT("Hexes[%d].TerrainId is not NAME_None"), Index),
			Model.Hexes[Index].TerrainId, FName(NAME_None));

		DistinctTerrain.Add(Model.Hexes[Index].TerrainId);
	}

	// Every hex resolving to ONE name would satisfy every comparison above and would still
	// be a board that could not be drawn. The scenario ships several terrain kinds; the
	// number is the scenario's and is deliberately not written here.
	TestTrue(TEXT("the board resolves to more than one terrain name"), DistinctTerrain.Num() > 1);

	for (int32 Index = 0; Index < Model.Units.Num(); ++Index)
	{
		const int32 Row = static_cast<int32>(Snapshot.units[Index].unitId);
		if (!TestTrue(FString::Printf(TEXT("Units[%d] names a definition row the table has"), Index),
				Row >= 0 && Row < static_cast<int32>(Tables.units->size())))
		{
			continue;
		}

		const FName Expected = DefinitionName((*Tables.units)[Row].id);

		TestEqual(FString::Printf(
				TEXT("Units[%d].DefId is UnitDef[%d].id, read through FStratBridge::Tables()"),
				Index, Row),
			Model.Units[Index].DefId, Expected);
		TestNotEqual(FString::Printf(TEXT("Units[%d].DefId is not NAME_None"), Index),
			Model.Units[Index].DefId, FName(NAME_None));

		// The index and the name must name the SAME row. A `DefId` that had drifted from
		// `DefIndex` would let two presentation layers disagree about one unit.
		TestEqual(FString::Printf(TEXT("Units[%d].DefIndex is the row DefId was read from"), Index),
			Model.Units[Index].DefIndex, Row);
	}

	return true;
}

// ---------------------------------------------------------------------------
// A refusal is not an empty view model.
//
// THIS CLAUSE HAS SHARPER TEETH HERE THAN ANYWHERE ELSE IN THE TREE. An empty
// `FStratViewModel` is a perfectly legitimate value -- it is what a board with no hexes
// looks like -- so a caller handed one cannot tell it apart from "the bridge is not
// seeded yet". Phase 3's reconciler spawns what the model has and DESTROYS what it does
// not; handed an empty model during startup it would not draw an empty board, it would
// tear down the one on screen. Refusing is the only answer that cannot be mistaken.
//
// ALL-OR-NOTHING is the other half: the caller's model must survive the refusal
// untouched, so a transient failure cannot blank a description that was correct a moment
// ago. The sentinel exists so that "untouched" is distinguishable from "written with a
// default-constructed value".
//
// THE REASON IS THE BRIDGE'S OWN WORDS, and that is asserted by equality against what the
// same unseeded bridge says when asked directly -- not by matching a sentence written
// here, which would fail on a copy edit and pass on a substituted one.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratViewModelRefusesUnseededBridgeTest,
	"Stratocracy.StratUI.T-INT-05.RefusesUnseededBridge",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratViewModelRefusesUnseededBridgeTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratViewModelParity;

	// Never loaded, never seeded.
	FStratBridge Bridge;
	TestFalse(TEXT("the bridge under test is genuinely unseeded"), Bridge.IsSeeded());

	// The refusing layer's own sentence, obtained from the refusing layer.
	strat::UiSnapshot Unused;
	const FStratResult Direct = Bridge.MakeUiSnapshot(Unused);
	TestFalse(TEXT("MakeUiSnapshot refuses an unseeded bridge"), Direct.bOk);

	FStratViewModel Model = Sentinel();
	FString Reason;
	const bool bBuilt = StratBuildViewModel(Bridge, kViewingSide, Model, Reason);

	TestFalse(TEXT("the builder refuses rather than producing an empty model"), bBuilt);
	TestFalse(TEXT("the refusal carries a reason"), Reason.IsEmpty());
	TestEqual(TEXT("the reason is the bridge's own words, forwarded unchanged"), Reason, Direct.Reason);

	RequireUntouched(*this, Model, TEXT("unseeded refusal"));

	return true;
}

// ---------------------------------------------------------------------------
// An out-of-range viewing side is refused, NOT clamped.
//
// A CLAMP IS THE WORST AVAILABLE BEHAVIOUR IN A HOT-SEAT GAME: it renders a coherent,
// correct-looking screen for the wrong player. Nothing about the picture says which
// belligerent it belongs to, so the mistake is invisible at exactly the moment it costs a
// match. The clause therefore asserts the refusal AND that the model's `ViewingSide` was
// not written with a substituted value.
//
// THE RANGE IS THE SNAPSHOT'S OWN SIDE COUNT, read off the projection rather than written
// down. A build with a different side count moves the boundary and this clause follows it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratViewModelRefusesOutOfRangeViewingSideTest,
	"Stratocracy.StratUI.T-INT-05.RefusesOutOfRangeViewingSide",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratViewModelRefusesOutOfRangeViewingSideTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratViewModelParity;

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
	const int32 SideCount = static_cast<int32>(UE_ARRAY_COUNT(Snapshot.side));

	// One past the end, and below the start. Both are outside; neither has an answer the
	// builder could substitute that would not be an invention about who is playing.
	const int32 OutOfRange[] = { SideCount, -1 };
	for (const int32 Side : OutOfRange)
	{
		FStratViewModel Model = Sentinel();
		FString Reason;
		const bool bBuilt = StratBuildViewModel(Bridge, Side, Model, Reason);

		TestFalse(FString::Printf(TEXT("viewing side %d is refused"), Side), bBuilt);
		TestFalse(FString::Printf(TEXT("the refusal of %d names a reason"), Side), Reason.IsEmpty());
		RequireUntouched(*this, Model, *FString::Printf(TEXT("viewing side %d"), Side));

		// The clamp check, stated separately from "untouched" because it is the specific
		// failure this clause exists to bar: a clamped value would be in range.
		TestNotEqual(FString::Printf(TEXT("viewing side %d was not clamped into range"), Side),
			Model.ViewingSide, FMath::Clamp(Side, 0, SideCount - 1));
	}

	// Every side that IS in range must be accepted, or the check above would be satisfied
	// by a builder that refused everything.
	for (int32 Side = 0; Side < SideCount; ++Side)
	{
		FStratViewModel Model;
		FString Reason;
		if (TestTrue(FString::Printf(TEXT("viewing side %d is accepted"), Side),
				StratBuildViewModel(Bridge, Side, Model, Reason)))
		{
			TestEqual(FString::Printf(TEXT("the model is built FOR side %d"), Side),
				Model.ViewingSide, Side);
		}
		else
		{
			AddError(Reason);
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// The presentation block is declared and empty -- the builder does not invent the DONE
// bit.
//
// Ui.h:186-197 rules that §2.11.1's DONE bit is derivable from NEITHER turn flag nor from
// any pair of them, and names the two cases: Wait and RMB-in-MOVED both reach DONE
// without spending the act flag. So a builder that set `bDone` from `hasMoved` /
// `hasActed` would be wrong in exactly the two cases the bit was separated out for -- and
// it would be wrong in a way that looks right on the seeded opening, where nothing has
// moved and every flag is false.
//
// WHICH IS WHY THIS CLAUSE MOVES A UNIT FIRST. On the opening position every turn flag is
// already false, so "bDone is false" would hold for a builder that copied `hasMoved`
// straight into it, and the clause would certify the defect it exists to catch. The move
// makes exactly one unit's `hasMoved` true through the rules module's own command path;
// `bDone` and `bLockedThisTurn` must still be false on it, and `bHasMoved` must now be
// true -- both compared against the snapshot the same bridge projects, never against a
// value written here.
//
// THE MOVE IS CHOSEN BY THE RULES MODULE, not by this file. `FStratBridge::Reachable`
// supplies the legal destinations and the first one that is not the unit's own hex is
// taken. No coordinate is spelled here, so a scenario edit that moved the units does not
// need an edit here.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratViewModelPresentationBitsAreDefaultedTest,
	"Stratocracy.StratUI.T-INT-05.PresentationBitsAreDefaulted",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratViewModelPresentationBitsAreDefaultedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratViewModelParity;

	FStratBridge Bridge;
	strat::UiSnapshot Opening;
	FStratViewModel OpeningModel;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds, projects and builds"), Project(Bridge, kViewingSide, Opening, OpeningModel, Error)))
	{
		AddError(Error);
		return false;
	}

	// The opening: nothing is done and nothing is locked, and nothing has moved either --
	// which is why this half alone would not be a gate.
	for (int32 Index = 0; Index < OpeningModel.Units.Num(); ++Index)
	{
		TestFalse(FString::Printf(TEXT("opening Units[%d].bDone is left at its default"), Index),
			OpeningModel.Units[Index].bDone);
		TestFalse(FString::Printf(TEXT("opening Units[%d].bLockedThisTurn is left at its default"), Index),
			OpeningModel.Units[Index].bLockedThisTurn);
	}

	// ---- Spend one unit's move flag, through the rules module's own command path ----
	int32 MovedUnitId = INDEX_NONE;
	for (const strat::UiUnitView& Candidate : Opening.units)
	{
		if (Candidate.side != Opening.match.sideToMove || Candidate.hasMoved)
		{
			continue;
		}

		std::vector<strat::ReachEntry> Reach;
		const FStratResult Asked = Bridge.Reachable(static_cast<int32>(Candidate.id), Reach);
		if (!Asked.bOk)
		{
			continue;
		}

		for (const strat::ReachEntry& Entry : Reach)
		{
			// `reachable` always includes the unit's own hex at cost 0 -- the null move --
			// and moving there would not spend the flag.
			if (Entry.hex.q == Candidate.hex.q && Entry.hex.r == Candidate.hex.r)
			{
				continue;
			}
			if (Bridge.SubmitMove(static_cast<int32>(Candidate.id), Entry.hex).bOk)
			{
				MovedUnitId = static_cast<int32>(Candidate.id);
				break;
			}
		}

		if (MovedUnitId != INDEX_NONE)
		{
			break;
		}
	}

	if (!TestTrue(TEXT("a unit of the side to move could be moved, so the clause is not vacuous"),
			MovedUnitId != INDEX_NONE))
	{
		return false;
	}

	strat::UiSnapshot AfterMove;
	const FStratResult Reprojected = Bridge.MakeUiSnapshot(AfterMove);
	if (!TestTrue(TEXT("MakeUiSnapshot succeeds after the move"), Reprojected.bOk))
	{
		AddError(Reprojected.Reason);
		return false;
	}

	FStratViewModel AfterModel;
	FString BuildError;
	if (!TestTrue(TEXT("the view model rebuilds after the move"),
			StratBuildViewModel(Bridge, kViewingSide, AfterModel, BuildError)))
	{
		AddError(BuildError);
		return false;
	}

	if (!TestEqual(TEXT("Units.Num() == UiSnapshot::units.size() after the move"),
			AfterModel.Units.Num(), static_cast<int32>(AfterMove.units.size())))
	{
		return false;
	}

	bool bSawTheMovedUnit = false;
	for (int32 Index = 0; Index < AfterModel.Units.Num(); ++Index)
	{
		const FStratUnitView& Unit = AfterModel.Units[Index];

		// The turn flags still mirror the module, so this clause cannot pass by the builder
		// having stopped copying them.
		TestEqual(FString::Printf(TEXT("Units[%d].bHasMoved still mirrors UiUnitView::hasMoved"), Index),
			Unit.bHasMoved, AfterMove.units[Index].hasMoved);
		TestEqual(FString::Printf(TEXT("Units[%d].bHasActed still mirrors UiUnitView::hasActed"), Index),
			Unit.bHasActed, AfterMove.units[Index].hasActed);

		// The presentation block is still empty. On the moved unit this is the assertion
		// that a builder deriving bDone from bHasMoved would fail.
		TestFalse(FString::Printf(TEXT("Units[%d].bDone is not invented from the turn flags"), Index),
			Unit.bDone);
		TestFalse(FString::Printf(TEXT("Units[%d].bLockedThisTurn is not invented from the turn flags"), Index),
			Unit.bLockedThisTurn);

		if (Unit.UnitId == MovedUnitId)
		{
			bSawTheMovedUnit = true;
			TestTrue(TEXT("the moved unit has genuinely spent its move flag"), Unit.bHasMoved);
		}
	}

	TestTrue(TEXT("the moved unit is still in the model"), bSawTheMovedUnit);

	return true;
}
