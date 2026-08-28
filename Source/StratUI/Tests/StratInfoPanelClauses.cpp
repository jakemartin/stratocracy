// The gate on `StratDecorateInfoPanel` -- GDD §2.11.2's bottom-left, hover-driven,
// never-modal info panel, at the DECORATION seam.
//
// `GATE-INFOPANEL` THROUGHOUT, AND THE ID QUESTION IS SETTLED IN
// `StratInfoPanelSourceParity.cpp`'s header rather than re-argued here: §2.11.2 has no
// acceptance ID in the GDD's set, `GATE-` mints none and claims none, `GATE-BUILDMENU` is the
// precedent, and `T-UI-03` (the standings scoreboard) and `T-UI-05` (snapshot fidelity) were
// both considered and refused with reasons.
//
// WHAT THIS FILE PINS, AND WHY IT IS NOT ARITHMETIC. `StratDecorateInfoPanel` computes
// nothing. It SELECTS: which hex row the panel describes, which unit stands on it, which of
// two nearly-identical bits answers "is this capturable" and "is this unit done". Every
// defect available to it is a wiring defect, and a wiring defect produces a panel of entirely
// plausible numbers about the wrong thing -- no compiler and no numeric parity gate sees one.
// So each clause below drives the shipped function over a REAL projected board and compares
// the panel against the model row it was selected FROM, never against a number typed here.
//
// THE THREE COMPARISONS THAT A NAIVE CLAUSE GETS WRONG, named up front because each has its
// own clause below and each was a stated trap:
//
//   1. CAPTURABILITY IS NOT OWNERSHIP. `Ui.h` scopes `UiHexView::owner` to "capturable hexes
//      only; OWNER_NEUTRAL elsewhere", so a NEUTRAL Factory and a Plains hex both project
//      `INDEX_NONE`. A panel that inferred `bHexCapturable` from `HexOwner != INDEX_NONE`
//      would put `· neutral` beside every Plains hex on the board, and a clause that only
//      ever looked at an OWNED capturable hex would pass over it. Clause 3 shows a neutral
//      capturable hex and a non-capturable hex, BOTH at `HexOwner == INDEX_NONE`, differing
//      on the capturable field. That is the pair the shortcut cannot survive.
//
//   2. THE RANGE BAND IS NOT ONE NUMBER. Three of the four shipped rows are a point band;
//      only one is not. Clause 5 hovers the unit whose OWN row has `rangeMin != rangeMax` --
//      chosen by reading the projection, never by naming a unit -- because a panel clause
//      built on any point-band unit cannot falsify a collapsed band.
//
//   3. THE DONE BIT IS NEITHER TURN FLAG NOR ANY PAIR OF THEM. `Ui.h` says so in as many
//      words, and §2.11.2 gives the falsifying case: "a waited unit reads `done` while its
//      act flag is unspent". Clause 6 drives four units and its decisive pair is two units
//      with IDENTICAL turn flags and OPPOSITE `bDone` -- over which no function of the turn
//      flags whatever can produce the right answer twice.
//
// WHERE THE EXPECTATIONS COME FROM. The panel is a SELECTION over `FStratViewModel`, so the
// only correct expectation for a panel field is the model field it was selected from, read
// out of the same model on the same line. That is not a copy of the subject: the subject is
// WHICH field goes where, and the comparison is against a different object than the one the
// decorator wrote. Nothing below spells a terrain name, a unit name, a move cost, a stat, a
// hex coordinate or a board dimension. The one place a value is typed is the presentation
// bits in clause 6, which have no producer in this module and no module-side value to read --
// stated there rather than left to be noticed.
//
// "EMPTY" IS ASSERTED AGAINST A DEFAULT-CONSTRUCTED `FStratInfoPanelView` and never against
// typed zeroes, so a field whose default changes moves both sides together.
//
// NOTHING HERE CONSTRUCTS A WIDGET, AN ACTOR OR A WORLD. The decorator takes the model and
// nothing else -- no bridge, no snapshot, no table -- which is the property that lets it be
// driven headlessly, and it is the same property `FStratInfoPanelView`'s own block calls the
// thing the design is arranged around.
//
// WHAT THIS FILE DOES NOT PIN, stated rather than left to be discovered:
//   - That the table values the panel restates are the TABLE's. That is
//     `StratInfoPanelSourceParity.cpp` in this directory, one layer below.
//   - The four snapshot mirrors (`Hp`, `HpMax`, `bIsFlag`, `Owner`) as snapshot fidelity.
//     `StratViewModelParity.cpp` already asserts all four against the module.
//   - `AStratPlayerController::DecorateForPresentation`'s call site and its ordering. That is
//     `Source/StratPlay/Tests/StratInfoPanelRouting.cpp`, which is the only place that can
//     see the controller.
//   - Any rendered text. `yours` / `neutral` / `enemy`, the `·` separators, `12/20`'s slash
//     and `ready` / `done` are the widget's words and this struct spells none of them.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Math/IntPoint.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratViewModel.h"

#include "StratBridge.h"

#include "Data.h"
#include "Ui.h"

namespace StratInfoPanelClauses
{
	static const int32 kFirstSide   = 0;
	static const int32 kViewingSide = 0;

	static FString Describe(const FIntPoint& Hex)
	{
		return FString::Printf(TEXT("hex (q %d, r %d)"), Hex.X, Hex.Y);
	}

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
		const FString ScenarioPath =
			FPaths::Combine(FPaths::Combine(FPaths::ProjectDir(), TEXT("Data")), TEXT("ferrum_crossing.json"));
		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath, kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	/** The shipped board, projected. Every fixture below starts from this and from no
	 *  hand-written hex, so the hexes and units are DATA rather than a fixture opinion. */
	static bool BuildBoard(FStratBridge& Bridge, FStratViewModel& OutModel, FString& OutError)
	{
		if (!SeedBridge(Bridge, OutError))
		{
			return false;
		}
		if (!StratBuildViewModel(Bridge, kViewingSide, OutModel, OutError))
		{
			OutError = FString::Printf(TEXT("StratBuildViewModel refused: %s"), *OutError);
			return false;
		}
		return true;
	}

	/** Puts the hover on `Hex`, runs the shipped decorator, and hands back the panel. The
	 *  model is taken by value so each observation starts from the same board. */
	static FStratInfoPanelView PanelForHover(FStratViewModel Model, const FIntPoint& Hex)
	{
		Model.Hover.bHasHoveredHex = true;
		Model.Hover.HoveredHex     = Hex;
		StratDecorateInfoPanel(Model);
		return Model.InfoPanel;
	}

	/** Runs the decorator with no hover at all. */
	static FStratInfoPanelView PanelForNoHover(FStratViewModel Model)
	{
		Model.Hover.bHasHoveredHex = false;
		Model.Hover.HoveredHex     = FIntPoint::ZeroValue;
		StratDecorateInfoPanel(Model);
		return Model.InfoPanel;
	}

	/** Fails the clause unless every field of `Panel` is a default-constructed
	 *  `FStratInfoPanelView`'s. Field by field, so the failure names the field. */
	static void RequireEmptyPanel(
		FAutomationTestBase& Test, const FStratInfoPanelView& Panel, const FString& Which)
	{
		const FStratInfoPanelView Default;

		Test.TestEqual(Which + TEXT(": bHasHex is the default"), Panel.bHasHex, Default.bHasHex);
		Test.TestEqual(Which + TEXT(": Hex is the default"), Panel.Hex, Default.Hex);
		Test.TestEqual(Which + TEXT(": TerrainId is the default"), Panel.TerrainId, Default.TerrainId);
		Test.TestEqual(Which + TEXT(": TerrainMoveCost is the default"),
			Panel.TerrainMoveCost, Default.TerrainMoveCost);
		Test.TestEqual(Which + TEXT(": TerrainDefensePct is the default"),
			Panel.TerrainDefensePct, Default.TerrainDefensePct);
		Test.TestEqual(Which + TEXT(": bTerrainImpassable is the default"),
			Panel.bTerrainImpassable, Default.bTerrainImpassable);
		Test.TestEqual(Which + TEXT(": bHexCapturable is the default"),
			Panel.bHexCapturable, Default.bHexCapturable);
		Test.TestEqual(Which + TEXT(": HexOwner is the default"), Panel.HexOwner, Default.HexOwner);
		Test.TestEqual(Which + TEXT(": HexIncomeFame is the default"),
			Panel.HexIncomeFame, Default.HexIncomeFame);
		Test.TestEqual(Which + TEXT(": bHasUnit is the default"), Panel.bHasUnit, Default.bHasUnit);
		Test.TestEqual(Which + TEXT(": UnitId is the default"), Panel.UnitId, Default.UnitId);
		Test.TestEqual(Which + TEXT(": UnitSide is the default"), Panel.UnitSide, Default.UnitSide);
		Test.TestEqual(Which + TEXT(": UnitDefId is the default"), Panel.UnitDefId, Default.UnitDefId);
		Test.TestEqual(Which + TEXT(": UnitHp is the default"), Panel.UnitHp, Default.UnitHp);
		Test.TestEqual(Which + TEXT(": UnitHpMax is the default"), Panel.UnitHpMax, Default.UnitHpMax);
		Test.TestEqual(Which + TEXT(": UnitStatAtk is the default"),
			Panel.UnitStatAtk, Default.UnitStatAtk);
		Test.TestEqual(Which + TEXT(": UnitStatDef is the default"),
			Panel.UnitStatDef, Default.UnitStatDef);
		Test.TestEqual(Which + TEXT(": UnitStatMove is the default"),
			Panel.UnitStatMove, Default.UnitStatMove);
		Test.TestEqual(Which + TEXT(": UnitStatRangeMin is the default"),
			Panel.UnitStatRangeMin, Default.UnitStatRangeMin);
		Test.TestEqual(Which + TEXT(": UnitStatRangeMax is the default"),
			Panel.UnitStatRangeMax, Default.UnitStatRangeMax);
		Test.TestEqual(Which + TEXT(": bUnitDone is the default"), Panel.bUnitDone, Default.bUnitDone);
		Test.TestEqual(Which + TEXT(": bUnitIsFlag is the default"),
			Panel.bUnitIsFlag, Default.bUnitIsFlag);
	}

	/** Fails the clause unless the panel's unit half is entirely default -- the "hovered a
	 *  hex nobody is standing on" shape, where the hex half is filled and this is not. */
	static void RequireNoUnitHalf(
		FAutomationTestBase& Test, const FStratInfoPanelView& Panel, const FString& Which)
	{
		const FStratInfoPanelView Default;

		Test.TestEqual(Which + TEXT(": bHasUnit is false"), Panel.bHasUnit, Default.bHasUnit);
		Test.TestEqual(Which + TEXT(": UnitId is the default"), Panel.UnitId, Default.UnitId);
		Test.TestEqual(Which + TEXT(": UnitSide is the default"), Panel.UnitSide, Default.UnitSide);
		Test.TestEqual(Which + TEXT(": UnitDefId is the default"), Panel.UnitDefId, Default.UnitDefId);
		Test.TestEqual(Which + TEXT(": UnitHp is the default"), Panel.UnitHp, Default.UnitHp);
		Test.TestEqual(Which + TEXT(": UnitHpMax is the default"), Panel.UnitHpMax, Default.UnitHpMax);
		Test.TestEqual(Which + TEXT(": UnitStatAtk is the default"),
			Panel.UnitStatAtk, Default.UnitStatAtk);
		Test.TestEqual(Which + TEXT(": bUnitDone is the default"), Panel.bUnitDone, Default.bUnitDone);
		Test.TestEqual(Which + TEXT(": bUnitIsFlag is the default"),
			Panel.bUnitIsFlag, Default.bUnitIsFlag);
	}

	/** Fails the clause unless every terrain field of the panel is the hex row's own. */
	static void RequirePanelDescribesHex(
		FAutomationTestBase&       Test,
		const FStratInfoPanelView& Panel,
		const FStratHexView&       Hex,
		const FString&             Which)
	{
		Test.TestTrue(Which + TEXT(": the panel is on screen"), Panel.bHasHex);
		Test.TestEqual(Which + TEXT(": Hex is that hex's own"), Panel.Hex, Hex.Hex);
		Test.TestEqual(Which + TEXT(": TerrainId is that hex's own"), Panel.TerrainId, Hex.TerrainId);
		Test.TestEqual(Which + TEXT(": TerrainMoveCost is that hex's own"),
			Panel.TerrainMoveCost, Hex.TerrainMoveCost);
		Test.TestEqual(Which + TEXT(": TerrainDefensePct is that hex's own"),
			Panel.TerrainDefensePct, Hex.TerrainDefensePct);
		Test.TestEqual(Which + TEXT(": bHexCapturable is that hex's own bTerrainCapturable"),
			Panel.bHexCapturable, Hex.bTerrainCapturable);
		Test.TestEqual(Which + TEXT(": HexOwner is that hex's own Owner"), Panel.HexOwner, Hex.Owner);
		Test.TestEqual(Which + TEXT(": HexIncomeFame is that hex's own TerrainIncomeFame"),
			Panel.HexIncomeFame, Hex.TerrainIncomeFame);
	}

	/** The index of the first hex satisfying `Predicate`, or `INDEX_NONE`. */
	template <typename PredicateType>
	static int32 FindHex(const FStratViewModel& Model, PredicateType&& Predicate)
	{
		return Model.Hexes.IndexOfByPredicate(Forward<PredicateType>(Predicate));
	}

	/** True if some unit in the model stands on `Hex`. */
	static bool AnyUnitOn(const FStratViewModel& Model, const FIntPoint& Hex)
	{
		return Model.Units.ContainsByPredicate(
			[&Hex](const FStratUnitView& Unit) { return Unit.Hex == Hex; });
	}
}

// ---------------------------------------------------------------------------
// CLAUSE 1 -- the panel describes the hovered hex's OWN row, and following the hover is
// observable.
//
// WHY TWO HEXES AND NOT ONE. A decorator that always described `Hexes[0]`, or that wrote a
// constant, satisfies every equality a single-hex clause can make. The two hexes below are
// chosen off the projection for having DIFFERENT terrain rows, so the two panels must differ,
// and the clause asserts that they do before asserting what each one says.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratInfoPanelDescribesTheHoveredHexTest,
	"Stratocracy.StratUI.GATE-INFOPANEL.PanelDescribesTheHoveredHexsOwnRow",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratInfoPanelDescribesTheHoveredHexTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratInfoPanelClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the shipped scenario projects a board"), BuildBoard(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the board has hexes to hover"), Model.Hexes.Num() > 0))
	{
		return false;
	}

	// Two hexes of DIFFERENT terrain, taken off the projection. No name is spelled here.
	const int32 FirstIndex = 0;
	const int32 OtherIndex = FindHex(Model, [&Model](const FStratHexView& Candidate)
		{ return Candidate.TerrainId != Model.Hexes[0].TerrainId; });
	if (!TestTrue(TEXT("the board carries two hexes of different terrain, so following the "
	                   "hover is observable"),
			OtherIndex != INDEX_NONE))
	{
		return false;
	}

	const FStratHexView& First = Model.Hexes[FirstIndex];
	const FStratHexView& Other = Model.Hexes[OtherIndex];

	const FStratInfoPanelView FirstPanel = PanelForHover(Model, First.Hex);
	const FStratInfoPanelView OtherPanel = PanelForHover(Model, Other.Hex);

	RequirePanelDescribesHex(*this, FirstPanel, First,
		FString::Printf(TEXT("hovering %s"), *Describe(First.Hex)));
	RequirePanelDescribesHex(*this, OtherPanel, Other,
		FString::Printf(TEXT("hovering %s"), *Describe(Other.Hex)));

	// The identity leg: a decorator that wrote a constant would satisfy nothing here.
	TestNotEqual(TEXT("the two panels name different hexes"), FirstPanel.Hex, OtherPanel.Hex);
	TestNotEqual(TEXT("and different terrain rows"), FirstPanel.TerrainId, OtherPanel.TerrainId);

	return true;
}

// ---------------------------------------------------------------------------
// CLAUSE 2 -- the panel is written on EVERY path, and "no panel" is what an absent hover and
// an off-board hover both get.
//
// THE UNCONDITIONAL WRITE IS THE WHOLE POINT. §2.11.2 says "empty when nothing is hovered",
// and a decorator that returned early on the not-hovering path would leave the LAST panel on
// screen -- a cursor leaving the board would freeze a description of wherever it left. That
// is a stale-state defect a per-field clause on the hovering path cannot see, so this clause
// fills the panel first and then takes the hover away.
//
// AND AN OFF-BOARD HOVER IS THE SAME ANSWER, deliberately. `FStratInfoPanelView::bHasHex`'s
// own block rules that a "hovered a hex that does not exist" state a widget could render
// would be a diagnostic wearing a player-facing surface. The off-board hex below is derived
// from the board's own extent and then ASSERTED absent, rather than typed and hoped for.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratInfoPanelIsWrittenUnconditionallyTest,
	"Stratocracy.StratUI.GATE-INFOPANEL.PanelIsWrittenUnconditionallyAndIsEmptyOffTheBoard",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratInfoPanelIsWrittenUnconditionallyTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratInfoPanelClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the shipped scenario projects a board"), BuildBoard(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the board has hexes to hover"), Model.Hexes.Num() > 0))
	{
		return false;
	}

	// ---- A hovered board hex fills the panel, so "empty" below means something ----
	const FStratHexView&      Hovered = Model.Hexes[0];
	const FStratInfoPanelView Filled  = PanelForHover(Model, Hovered.Hex);
	if (!TestTrue(TEXT("hovering a board hex fills the panel, so the clears below are not vacuous"),
			Filled.bHasHex))
	{
		return false;
	}

	// ---- Path A: the model arrives carrying a stale panel and stops hovering ----
	// This is the freeze this clause exists to catch. The stale panel is a REAL one, produced
	// by the shipped decorator a line above, not a hand-filled struct.
	FStratViewModel Stale = Model;
	Stale.InfoPanel = Filled;
	const FStratInfoPanelView AfterHoverStops = PanelForNoHover(Stale);
	RequireEmptyPanel(*this, AfterHoverStops,
		TEXT("a model that carried a filled panel and is no longer hovering"));

	// ---- Path B: hovering a hex that is not on the board ----
	// Derived from the board's own extent. Nothing about the board's shape is assumed: the
	// candidate is then asserted absent from `Hexes` before it is used.
	int32 MaxQ = Model.Hexes[0].Hex.X;
	int32 MaxR = Model.Hexes[0].Hex.Y;
	for (const FStratHexView& Hex : Model.Hexes)
	{
		MaxQ = FMath::Max(MaxQ, Hex.Hex.X);
		MaxR = FMath::Max(MaxR, Hex.Hex.Y);
	}
	const FIntPoint OffBoard{MaxQ + 1, MaxR + 1};
	if (!TestTrue(
			*FString::Printf(TEXT("%s is genuinely absent from the projected board"), *Describe(OffBoard)),
			FindHex(Model, [&OffBoard](const FStratHexView& Candidate)
				{ return Candidate.Hex == OffBoard; }) == INDEX_NONE))
	{
		return false;
	}

	FStratViewModel StaleAgain = Model;
	StaleAgain.InfoPanel = Filled;
	StaleAgain.Hover.bHasHoveredHex = true;
	StaleAgain.Hover.HoveredHex     = OffBoard;
	StratDecorateInfoPanel(StaleAgain);
	RequireEmptyPanel(*this, StaleAgain.InfoPanel,
		*FString::Printf(TEXT("a model carrying a filled panel and hovering off-board %s"),
			*Describe(OffBoard)));

	return true;
}

// ---------------------------------------------------------------------------
// CLAUSE 3 -- capturability is the terrain row's flag and is NOT ownership.
//
// THE PAIR THAT CATCHES THE SHORTCUT. A neutral capturable hex and a non-capturable hex BOTH
// project `Owner == INDEX_NONE`, because `Ui.h` scopes `owner` to "capturable hexes only;
// OWNER_NEUTRAL elsewhere". So a panel that wrote `bHexCapturable = (HexOwner != INDEX_NONE)`
// agrees with the truth on every OWNED hex and is wrong on every neutral objective -- and a
// clause that only ever hovered an owned Factory would certify it. This clause asserts the
// two hexes agree on owner and DISAGREE on capturability, which is the one observation the
// shortcut cannot produce.
//
// BOTH HEXES ARE FOUND BY SEARCHING THE PROJECTION, never named. If the shipped scenario ever
// stopped containing one of them the search fails loudly instead of the clause going vacuous.
//
// THE OWNED ARM IS HERE TOO, and it is what stops the clause from being satisfiable by a
// panel that wrote `HexOwner = INDEX_NONE` always: a hex the scenario gives to a side must
// arrive carrying that side. `HexOwner` is a SIDE and not an own/enemy boolean, on
// `FStratForecastView::RiskedFlagSide`'s rule; nothing here compares it to `ViewingSide`.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratInfoPanelCapturabilityIsNotOwnershipTest,
	"Stratocracy.StratUI.GATE-INFOPANEL.CapturabilityIsTheTerrainFlagAndNotOwnership",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratInfoPanelCapturabilityIsNotOwnershipTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratInfoPanelClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the shipped scenario projects a board"), BuildBoard(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	const int32 NeutralCapturable = FindHex(Model, [](const FStratHexView& Hex)
		{ return Hex.bTerrainCapturable && Hex.Owner == INDEX_NONE; });
	const int32 NonCapturable = FindHex(Model, [](const FStratHexView& Hex)
		{ return !Hex.bTerrainCapturable && Hex.Owner == INDEX_NONE; });
	const int32 OwnedCapturable = FindHex(Model, [](const FStratHexView& Hex)
		{ return Hex.bTerrainCapturable && Hex.Owner != INDEX_NONE; });

	if (!TestTrue(TEXT("the board carries a NEUTRAL CAPTURABLE hex -- the one an ownership "
	                   "shortcut gets wrong"),
			NeutralCapturable != INDEX_NONE)
		|| !TestTrue(TEXT("and a NON-CAPTURABLE hex, which carries the same owner"),
			NonCapturable != INDEX_NONE))
	{
		return false;
	}

	const FStratHexView& Neutral = Model.Hexes[NeutralCapturable];
	const FStratHexView& Plain   = Model.Hexes[NonCapturable];

	const FStratInfoPanelView NeutralPanel = PanelForHover(Model, Neutral.Hex);
	const FStratInfoPanelView PlainPanel   = PanelForHover(Model, Plain.Hex);

	// ---- The two agree on the owner ---------------------------------------
	TestEqual(
		*FString::Printf(TEXT("the neutral capturable %s projects no owner"), *Describe(Neutral.Hex)),
		NeutralPanel.HexOwner, static_cast<int32>(INDEX_NONE));
	TestEqual(
		*FString::Printf(TEXT("and the non-capturable %s projects no owner either"), *Describe(Plain.Hex)),
		PlainPanel.HexOwner, static_cast<int32>(INDEX_NONE));

	// ---- And they DISAGREE on capturability -------------------------------
	TestTrue(
		*FString::Printf(TEXT("the neutral capturable %s reports bHexCapturable"), *Describe(Neutral.Hex)),
		NeutralPanel.bHexCapturable);
	TestFalse(
		*FString::Printf(TEXT("the non-capturable %s does not, at the SAME owner -- an "
		                      "`Owner != INDEX_NONE` inference cannot produce this pair"),
			*Describe(Plain.Hex)),
		PlainPanel.bHexCapturable);

	// Each equals its own hex's flag, not merely each other's opposite.
	TestEqual(TEXT("the neutral panel's flag is that hex's own bTerrainCapturable"),
		NeutralPanel.bHexCapturable, Neutral.bTerrainCapturable);
	TestEqual(TEXT("the non-capturable panel's flag is that hex's own bTerrainCapturable"),
		PlainPanel.bHexCapturable, Plain.bTerrainCapturable);

	// The §2.7 rate rides with it and is the HEX's, not a side's total.
	TestEqual(TEXT("the neutral panel's HexIncomeFame is that hex's own TerrainIncomeFame"),
		NeutralPanel.HexIncomeFame, Neutral.TerrainIncomeFame);
	TestEqual(TEXT("the non-capturable panel's HexIncomeFame is that hex's own"),
		PlainPanel.HexIncomeFame, Plain.TerrainIncomeFame);
	TestTrue(TEXT("the capturable hex's own rate is non-zero, so the income comparison above "
	              "is not two zeroes agreeing"),
		Neutral.TerrainIncomeFame != 0);

	// ---- The owned arm: `HexOwner` really is a side ------------------------
	if (TestTrue(TEXT("the scenario gives at least one capturable hex to a side"),
			OwnedCapturable != INDEX_NONE))
	{
		const FStratHexView&      Owned      = Model.Hexes[OwnedCapturable];
		const FStratInfoPanelView OwnedPanel = PanelForHover(Model, Owned.Hex);

		TestEqual(
			*FString::Printf(TEXT("the owned capturable %s carries its own side and not INDEX_NONE"),
				*Describe(Owned.Hex)),
			OwnedPanel.HexOwner, Owned.Owner);
		TestNotEqual(TEXT("and that side is not INDEX_NONE, so HexOwner is not a constant"),
			OwnedPanel.HexOwner, static_cast<int32>(INDEX_NONE));
		TestTrue(TEXT("the owned hex is capturable too"), OwnedPanel.bHexCapturable);
	}

	return true;
}

// ---------------------------------------------------------------------------
// CLAUSE 4 -- impassable is the zero-cost sentinel read, and the raw cost survives it.
//
// `Data.h` states the meaning on the field: `0 == impassable (§4.8 sentinel)`. The panel
// performs that comparison one layer below the widget deliberately -- the field's own block
// says the alternative is a widget holding a rules constant -- so the comparison is a subject
// a clause can have.
//
// BOTH ARMS, AND THE COST STILL THERE. A panel that reported impassability and threw the
// number away would fail §2.11.2's `move 1` on every passable hex, so the raw cost is
// asserted on both hexes as well.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratInfoPanelImpassableIsTheSentinelTest,
	"Stratocracy.StratUI.GATE-INFOPANEL.ImpassableIsTheZeroCostSentinelAndTheCostSurvives",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratInfoPanelImpassableIsTheSentinelTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratInfoPanelClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the shipped scenario projects a board"), BuildBoard(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	const int32 ImpassableIndex = FindHex(Model, [](const FStratHexView& Hex)
		{ return Hex.TerrainMoveCost == 0; });
	const int32 PassableIndex = FindHex(Model, [](const FStratHexView& Hex)
		{ return Hex.TerrainMoveCost != 0; });

	if (!TestTrue(TEXT("the board carries a zero-cost hex, so the sentinel arm is reachable"),
			ImpassableIndex != INDEX_NONE)
		|| !TestTrue(TEXT("and a non-zero-cost hex, so the other arm is too"),
			PassableIndex != INDEX_NONE))
	{
		return false;
	}

	const FStratHexView& Impassable = Model.Hexes[ImpassableIndex];
	const FStratHexView& Passable   = Model.Hexes[PassableIndex];

	const FStratInfoPanelView ImpassablePanel = PanelForHover(Model, Impassable.Hex);
	const FStratInfoPanelView PassablePanel   = PanelForHover(Model, Passable.Hex);

	TestTrue(
		*FString::Printf(TEXT("the zero-cost %s reports bTerrainImpassable"), *Describe(Impassable.Hex)),
		ImpassablePanel.bTerrainImpassable);
	TestFalse(
		*FString::Printf(TEXT("the non-zero-cost %s does not"), *Describe(Passable.Hex)),
		PassablePanel.bTerrainImpassable);

	// The number is still on the panel, on both hexes, and it is that hex's own.
	TestEqual(TEXT("the impassable panel still carries the hex's own raw move cost"),
		ImpassablePanel.TerrainMoveCost, Impassable.TerrainMoveCost);
	TestEqual(TEXT("and the passable panel carries the hex's own raw move cost"),
		PassablePanel.TerrainMoveCost, Passable.TerrainMoveCost);

	return true;
}

// ---------------------------------------------------------------------------
// CLAUSE 5 -- the unit row is the unit STANDING ON the hovered hex, stats and all, and the
// range band is not collapsed.
//
// THE RANGE BAND IS WHY THE UNIT IS CHOSEN RATHER THAN TAKEN. Three of the four shipped rows
// have `rangeMin == rangeMax`; over any of those, a panel that wrote `UnitStatRangeMin` from
// `StatRangeMax` (or the reverse) is indistinguishable from a correct one. This clause hovers
// the hex of a unit whose OWN projected band is not a point -- found by search, never by
// name -- so the two fields are separately observable.
//
// THE TWO PRESENCE BITS ARE INDEPENDENT and the empty-hex arm is what says so. A hovered hex
// with nobody on it is the common case: `bHasHex` true, `bHasUnit` false, and the whole unit
// half at its defaults rather than carrying the last unit hovered.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratInfoPanelUnitRowIsTheUnitOnTheHexTest,
	"Stratocracy.StratUI.GATE-INFOPANEL.UnitRowIsTheUnitStandingOnTheHoveredHex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratInfoPanelUnitRowIsTheUnitOnTheHexTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratInfoPanelClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the shipped scenario projects a board"), BuildBoard(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the board has units"), Model.Units.Num() > 0))
	{
		return false;
	}

	// The unit whose band is NOT a point -- the only kind that can falsify a collapse.
	const int32 WideBandIndex = Model.Units.IndexOfByPredicate(
		[](const FStratUnitView& Unit) { return Unit.StatRangeMin != Unit.StatRangeMax; });
	if (!TestTrue(TEXT("the projection contains a unit whose range band is not a point, so a "
	                   "collapsed band would be visible on the panel"),
			WideBandIndex != INDEX_NONE))
	{
		return false;
	}

	const FStratUnitView&     Unit  = Model.Units[WideBandIndex];
	const FStratInfoPanelView Panel = PanelForHover(Model, Unit.Hex);

	TestTrue(*FString::Printf(TEXT("hovering %s, where a unit stands, reports a hex"),
			*Describe(Unit.Hex)),
		Panel.bHasHex);
	TestTrue(TEXT("and reports a unit"), Panel.bHasUnit);

	// Every unit field is that unit's own, read out of the model beside the assertion.
	TestEqual(TEXT("UnitId is that unit's own"), Panel.UnitId, Unit.UnitId);
	TestEqual(TEXT("UnitSide is that unit's own"), Panel.UnitSide, Unit.Side);
	TestEqual(TEXT("UnitDefId is that unit's own"), Panel.UnitDefId, Unit.DefId);
	TestEqual(TEXT("UnitHp is that unit's own"), Panel.UnitHp, Unit.Hp);
	TestEqual(TEXT("UnitHpMax is that unit's own"), Panel.UnitHpMax, Unit.HpMax);
	TestEqual(TEXT("UnitStatAtk is that unit's own"), Panel.UnitStatAtk, Unit.StatAtk);
	TestEqual(TEXT("UnitStatDef is that unit's own"), Panel.UnitStatDef, Unit.StatDef);
	TestEqual(TEXT("UnitStatMove is that unit's own"), Panel.UnitStatMove, Unit.StatMove);
	TestEqual(TEXT("UnitStatRangeMin is that unit's own"), Panel.UnitStatRangeMin, Unit.StatRangeMin);
	TestEqual(TEXT("UnitStatRangeMax is that unit's own"), Panel.UnitStatRangeMax, Unit.StatRangeMax);
	TestEqual(TEXT("bUnitIsFlag is that unit's own bIsFlag"), Panel.bUnitIsFlag, Unit.bIsFlag);

	// The band survived as two numbers. Both halves, so a collapse in either direction shows.
	TestNotEqual(TEXT("the panel's range band is still not a point -- the minimum was not "
	                  "overwritten by the maximum"),
		Panel.UnitStatRangeMin, Panel.UnitStatRangeMax);

	// The unit really is the one on the hovered hex, and not merely some unit.
	TestEqual(TEXT("and the unit the panel names stands on the hovered hex"),
		Unit.Hex, Panel.Hex);

	// ---- The empty-hex arm -------------------------------------------------
	const int32 EmptyIndex = FindHex(Model, [&Model](const FStratHexView& Hex)
		{ return !AnyUnitOn(Model, Hex.Hex); });
	if (TestTrue(TEXT("the board has a hex nobody stands on"), EmptyIndex != INDEX_NONE))
	{
		const FStratHexView&      Empty      = Model.Hexes[EmptyIndex];
		const FStratInfoPanelView EmptyPanel = PanelForHover(Model, Empty.Hex);

		TestTrue(*FString::Printf(TEXT("hovering the empty %s still describes the hex"),
				*Describe(Empty.Hex)),
			EmptyPanel.bHasHex);
		RequireNoUnitHalf(*this, EmptyPanel,
			*FString::Printf(TEXT("hovering the empty %s"), *Describe(Empty.Hex)));
	}

	return true;
}

// ---------------------------------------------------------------------------
// CLAUSE 6 -- `done` is the presentation block's bit and is derivable from NEITHER turn flag
// NOR any pair of them.
//
// §2.11.2 rules on this by name: the panel shows "the machine's DONE bit (§2.11.1), read from
// the view-model's presentation block and not from a snapshot flag: a waited unit reads
// `done` while its act flag is unspent". `Ui.h`'s presentation block says the same thing more
// strongly -- the bit "is DERIVABLE FROM NEITHER turn flag nor from any pair of them".
//
// HOW THIS CLAUSE MAKES THAT FALSIFIABLE, which is the part a naive clause gets wrong. Four
// units are driven, and the DECISIVE ONE IS THE THIRD PAIR: units B and C carry IDENTICAL
// turn flags (`bHasMoved` true, `bHasActed` true) and OPPOSITE `bDone`. No function whatever
// of `(bHasMoved, bHasActed)` -- not `bHasActed`, not `bHasMoved && bHasActed`, not
// `!bHasActed`, not any of the sixteen -- can answer both of them correctly, because its two
// inputs are equal and its two required outputs are not. A clause that drove only the waited
// unit would be satisfied by `!bHasActed`, which is a real and plausible wrong answer; that
// is why B and C are here.
//
// WHERE THE EXPECTATION COMES FROM, STATED PLAINLY BECAUSE IT IS THE ONE TYPED VALUE IN THIS
// FILE. `bDone` and the two turn flags are set on the model by this clause, and the expected
// panel value is the `bDone` it set. There is no module-side value to read: `Ui.h`'s
// presentation block "has no module-side counterpart and no derivation from one", which is
// the same ruling that keeps these members out of T-UI-05's subject, and `StratBuildViewModel`
// leaves all three at their defaults by design. The producer is
// `FStratSelectionMachine::DecorateViewModel`, a module above this one, and its own parity is
// `StratSelectionMachineParity.cpp`'s. So what is asserted here is exactly the SELECTION --
// which of three bits on the unit reaches the panel -- and nothing about what the bit means.
//
// THE UNITS AND THEIR HEXES ARE THE SCENARIO'S. Only the three bits are written, on units the
// projection supplied, standing where the projection put them.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratInfoPanelDoneIsThePresentationBitTest,
	"Stratocracy.StratUI.GATE-INFOPANEL.DoneIsThePresentationBitAndNotATurnFlag",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratInfoPanelDoneIsThePresentationBitTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratInfoPanelClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the shipped scenario projects a board"), BuildBoard(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the scenario supplies at least four units to drive"), Model.Units.Num() >= 4))
	{
		return false;
	}

	// Four units on four distinct hexes -- the projection's own, checked rather than assumed,
	// because two units sharing a hex would make the hovers ambiguous.
	TSet<FIntPoint> Occupied;
	for (int32 Index = 0; Index < 4; ++Index)
	{
		bool bAlready = false;
		Occupied.Add(Model.Units[Index].Hex, &bAlready);
		if (!TestFalse(
				*FString::Printf(TEXT("Units[%d] stands on a hex no earlier fixture unit occupies"), Index),
				bAlready))
		{
			return false;
		}
	}

	// A -- THE WAITED UNIT: §2.11.2's own falsifying case. Done, and its act flag unspent.
	Model.Units[0].bDone     = true;
	Model.Units[0].bHasMoved = true;
	Model.Units[0].bHasActed = false;

	// B and C -- IDENTICAL TURN FLAGS, OPPOSITE `bDone`. The decisive pair.
	Model.Units[1].bDone     = false;
	Model.Units[1].bHasMoved = true;
	Model.Units[1].bHasActed = true;

	Model.Units[2].bDone     = true;
	Model.Units[2].bHasMoved = true;
	Model.Units[2].bHasActed = true;

	// D -- the untouched unit: nothing spent and not done.
	Model.Units[3].bDone     = false;
	Model.Units[3].bHasMoved = false;
	Model.Units[3].bHasActed = false;

	for (int32 Index = 0; Index < 4; ++Index)
	{
		const FStratUnitView&     Unit  = Model.Units[Index];
		const FStratInfoPanelView Panel = PanelForHover(Model, Unit.Hex);

		if (!TestTrue(
				*FString::Printf(TEXT("hovering Units[%d]'s %s finds that unit"), Index, *Describe(Unit.Hex)),
				Panel.bHasUnit))
		{
			continue;
		}
		TestEqual(
			*FString::Printf(TEXT("and it is Units[%d]"), Index),
			Panel.UnitId, Unit.UnitId);

		// THE ASSERTION: the panel's bit is the unit's `bDone`, whatever the turn flags say.
		TestEqual(
			*FString::Printf(
				TEXT("Units[%d]: bUnitDone is the presentation block's bDone (%s), with "
				     "bHasMoved %s and bHasActed %s"),
				Index,
				Unit.bDone     ? TEXT("true") : TEXT("false"),
				Unit.bHasMoved ? TEXT("true") : TEXT("false"),
				Unit.bHasActed ? TEXT("true") : TEXT("false")),
			Panel.bUnitDone, Unit.bDone);
	}

	// ---- The decisive pair, said out loud so a reader sees the argument -----
	// If either of these ever stopped holding, the clause above would still be green while
	// the property it exists for had become untestable.
	TestEqual(TEXT("the decisive pair carries the same bHasMoved"),
		Model.Units[1].bHasMoved, Model.Units[2].bHasMoved);
	TestEqual(TEXT("and the same bHasActed"),
		Model.Units[1].bHasActed, Model.Units[2].bHasActed);
	TestNotEqual(TEXT("and OPPOSITE bDone -- so no function of the turn flags can answer both"),
		Model.Units[1].bDone, Model.Units[2].bDone);

	const FStratInfoPanelView PanelB = PanelForHover(Model, Model.Units[1].Hex);
	const FStratInfoPanelView PanelC = PanelForHover(Model, Model.Units[2].Hex);
	TestNotEqual(TEXT("and the two panels disagree, which only reading bDone can produce"),
		PanelB.bUnitDone, PanelC.bUnitDone);

	return true;
}
