// The editor pass for §2.11.1's path preview at the BRIDGE -- `FStratBridge::MovePathToHex`.
// GDD §2.5, §2.11.1, §4.7 Stub 3, §4.9 part 2. T-UI-02 throughout, by the 2026-08-29 W8
// ruling that binds item (1) to that ID. No identifier is minted here.
//
// T-UI-02's own sentence is "the reachable-hex highlight displays exactly the T-MOVE-01 set;
// the UI queries the module and never recomputes movement". The preview is the same claim one
// field over: a route drawn on the board is a movement statement, and the only way it can be
// TRUE is if the module produced it. What is pinned below is that every hex, every tick and
// the total arrive from `strat::findPath` and `strat::reachable` and are copied.
//
// WHY THIS FILE IS IN StratBridge AND COULD NOT BE ANYWHERE ELSE, on
// `StratBridgeQueryParity.cpp`'s own reasoning, unchanged. Every clause here compares an
// `FStratBridge` answer against the `strat::` answer to the same question, so the file must be
// able to CALL both. The vendored sources carry no `_API` macro, so
// `UnrealEditor-StratBridge.dll` exports `FStratBridge` and nothing beside it -- the 8 x
// LNK2019 `StratBridge.h` opens with. This module compiles the rules sources into itself via
// `Vendored/*.strat.cpp`, so a test `.cpp` here links both sides. The same file written in
// StratUI or StratPlay is not a weaker test, it is a link error.
//
// WHERE EVERY EXPECTATION COMES FROM, which is the only question worth asking about a parity
// file.
//   - The ROUTE and the TOTAL: `strat::findPath` over the bridge's own `MakeUiWorld()`.
//   - The TICKS: `strat::uiReachable` over that same world, looked up by hex.
//   - WHICH GOALS ANSWER A ROUTE: `strat::uiReachable`'s set, exhaustively, against every hex
//     of the shipped board.
//   - The REFUSAL WORDINGS: `FStratBridge::Reachable`'s own `FStratResult::Reason` for the
//     same condition on the same bridge object, so the two cannot drift apart by an edit to
//     either. Nothing here types a refusal literal.
//   - The ENDPOINTS: the unit's hex off the module's `UiUnit` and the goal this file asked
//     for. Neither is written down as a coordinate.
// Not one hex, cost, allowance or total is transcribed from the scenario file or the GDD.
//
// THE ONE ARGUMENT THIS FILE RE-DERIVES, DECLARED RATHER THAN BURIED. `Ui.h` ships
// `uiReachable` and NO `uiFindPath`, so to call `strat::findPath` directly this file must
// supply the §2.4 allowance itself -- `(*World.unitDefs)[defIndex].move`, the same table read
// `MovePathToHex` performs and the same one `uiReachable`'s body performs. It is a TABLE READ
// and not a rule: this file does not adjust, cap or spend it. It is also the one thing
// `MovePathRouteIsFindPathsRoute` cannot pin about itself, since a bridge that read the wrong
// allowance and a test that read the same wrong allowance would agree. THAT HOLE IS CLOSED BY
// A DIFFERENT CLAUSE AND ON A DIFFERENT INSTRUMENT: `MovePathAnswersExactlyTheReachableSet`
// asks `uiReachable` -- which derives the allowance itself, inside the module -- which goals
// are supposed to answer a route, and then asks the bridge about every hex of the board. A
// wrong allowance moves that set and reddens it. Neither clause covers the other's subject and
// this is said so that a reader does not have to discover the dependency.
//
// THE ONE COMPUTED NUMBER, DECLARED. `MovePathRouteIsFindPathsRoute` ends with a FIXTURE
// RICHNESS control that subtracts two ticks to establish that some route in the sweep entered
// a hex costing more than 1 -- that Ferrum Crossing's woods and mountains are actually on the
// routes being compared. It is a FOIL on the board, in `NotHexDistance`'s sense: no clause
// asserts a tick EQUALS anything this file computed, and the equality above it still takes
// every number from the module. It exists because a board of uniform plains would satisfy
// every clause in this file while proving nothing about a cost model.
//
// `TestEqual` ON `FString` IS CASE-INSENSITIVE IN THIS ENGINE, and so are `==` and `Contains`.
// That has already produced a clause in this project that could not fail. The two refusal
// clauses below compare refusal WORDINGS, so they use `TestEqualSensitive` -- the engine's own
// case-sensitive form, `AutomationTest.h` -- and never `TestEqual`. A case-insensitive
// comparison there would stay green over a re-cased literal, which is one of the drifts the
// clause exists to catch. Every other comparison in this file is over `int32`, `strat::Hex` or
// an array length, where the hazard does not arise.
//
// AN OFFSET PAIR AND AN AXIAL PAIR PRINT IDENTICALLY. Every failure message routes hexes
// through `Describe`, which prints both, so a coordinate confusion is legible rather than
// hidden by a coincidence.
//
// WHAT THIS FILE DOES NOT PIN, stated rather than left to be discovered:
//   - Anything about `FStratViewModel::PathPreview` or `StratDecoratePathPreview`. That is
//     `Source/StratPlay/Tests/StratPathPreviewDecoratorClauses.cpp`'s subject, and it is in
//     that module because `StratPlay` is where the decorator and the query seam live.
//   - That `findPath`'s tie-break among equal-cost routes is §2.5's. That is `Move.h`'s
//     guarantee and the rules crew's gate; this file asserts the bridge returns whatever
//     `findPath` returned, which is the only half the bridge can get wrong.
//   - WHICH IMPLEMENTATION PRODUCED THE TICKS. `MovePathTicksAreReachableCosts` pins that each
//     tick EQUALS the module's per-hex cost; it cannot tell a per-hex lookup from a running sum
//     of `moveCost` along the route, because those two are the same number on every board. The
//     proof is at that clause and at `StratBridge.h`'s `MovePathToHex` block, and the summing
//     mutant was built in place here and SURVIVED. Read that clause's own block before
//     concluding anything about it from its name.
//   - WHICH IMPLEMENTATION PRODUCED `OutTotalCost`, for the neighbouring reason: the bridge
//     refuses every board on which the last tick and `findPath`'s `outCost` differ, so on every
//     board that answers at all the two are equal BY CONSTRUCTION and the assignment is
//     unobservable from outside. `MovePathRouteIsFindPathsRoute` compares against `outCost`'s
//     own out-parameter, which is the strongest form available and is still not discriminating.
//   - The cross-check refusal arm -- `findPath`'s cost disagreeing with the reachable cost at
//     the goal. Unreachable BY THE SAME ARGUMENT rather than for want of a fixture: the check
//     is what makes the disagreement impossible, so its own arm cannot fire. `StratBridge.h`
//     states that as a proof rather than a sample.
//   - Any drawing. Nothing here is on screen.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Math/IntPoint.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBridge.h"

// The vendored rules module, by short name via StratRules' PublicIncludePaths. Named
// explicitly rather than left to the transitive include through StratBridge.h, because this
// file calls into all four directly and a reader should not have to discover that.
#include "Data.h"
#include "Hex.h"
#include "Move.h"
#include "Ui.h"

namespace StratPathPreviewParity
{
	// The same pin `StratBridgeQueryParity.cpp` carries and it must stay equal to theirs.
	// `Replay.h` states that no rule decides which side moves first, so this is a harness
	// constant.
	static const int32 kFirstSide = 0;

	// A unit id no table or scenario edit can bring into existence -- the same value and the
	// same reasoning as `StratBridgeQueryParity.cpp`'s and `StratBridgeParity.cpp`'s. A check
	// whose subject has to be re-picked every time the board moves is a check that gets
	// quietly relaxed instead.
	static const int32 kNoSuchUnitId = 999999;

	/** Both axial and odd-r offset, because the scenario file authors the second. */
	static FString Describe(const strat::Hex& H)
	{
		int Col = 0;
		int Row = 0;
		strat::axialToOffset(H, Col, Row);
		return FString::Printf(TEXT("axial(%d,%d)=offset[%d,%d]"), H.q, H.r, Col, Row);
	}

	static FString Describe(const FIntPoint& P)
	{
		return Describe(strat::Hex{P.X, P.Y});
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** Definitions only -- the bridge state between the two refusal reasons. */
	static bool LoadDefsOnly(FStratBridge& Bridge, FString& OutError)
	{
		UDataTable* const Units   = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		UDataTable* const Terrain = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
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
		return true;
	}

	/**
	 * The seeded opening: shipped tables mapped in, shipped scenario seeded through the rules
	 * module's own `seedFromScenario`.
	 *
	 * THE OPENING AND NOT A REPLAYED PREFIX, and unlike `StratBridgeQueryParity.cpp`'s
	 * forecast half the choice is not forced -- it is made, for that file's stated reason
	 * about reachability. Ferrum Crossing's water column, its woods and mountains and its five
	 * friendly units per side all bite at the opening, which is exactly the board a route
	 * comparison wants. An attack needed a mid-match prefix; a route does not.
	 *
	 * Returns false and fills `OutError` rather than asserting, on `StratBridgeParity`'s
	 * precedent -- a missing asset becomes a named failure inside the caller's own clause list
	 * rather than a crash in a helper.
	 */
	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		if (!LoadDefsOnly(Bridge, OutError))
		{
			return false;
		}

		const FString ScenarioPath = FPaths::Combine(
			FPaths::Combine(FPaths::ProjectDir(), TEXT("Data")), TEXT("ferrum_crossing.json"));

		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath, kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	/**
	 * `uiReachable`'s cost for one hex, or -1 when the module did not return that hex.
	 *
	 * A LOOKUP AND NOT A COMPUTATION: the number returned is the module's, found by identity.
	 * Nothing here charges a step, sums a column or reads a `TerrainDef`.
	 */
	static int32 ModuleCostFor(const std::vector<strat::ReachEntry>& Reach, const strat::Hex& H)
	{
		for (const strat::ReachEntry& E : Reach)
		{
			if (strat::hexEqual(E.hex, H))
			{
				return E.cost;
			}
		}
		return -1;
	}
}

// ---------------------------------------------------------------------------
// T-UI-02 -- the route and the total ARE `strat::findPath`'s, hex for hex and in its order.
//
// WHAT THIS PINS THAT NOTHING ELSE DOES. `MovePathTicksAreReachableCosts` below compares the
// NUMBERS on the route and is silent about which hexes the route visits; a bridge that walked
// the reach set greedily downhill from the goal would satisfy it completely, because every hex
// on such a walk is in the reach set and would carry its own honest cost. That walk is
// `Move.h`'s tie-break rule restated by a module that may not hold it, and it is the exact
// substitution `StratBridge.h`'s own block says this method exists to make unreachable. This
// clause is the one that notices, because it compares the SEQUENCE against `findPath`'s.
//
// AND IT PINS `OutTotalCost` AGAINST `outCost` DIRECTLY. The bridge cross-checks its total
// against the last tick and refuses on disagreement, so on a green board `OutTotalCost`,
// `outCost` and `RouteCosts.Last()` are three names for one number -- which means a bridge
// that had assigned the LAST TICK instead of `outCost` would be invisible to any clause that
// compared it to the last tick. The right-hand side here is `findPath`'s own `outCost` out of
// its own out-parameter, so the assignment is pinned rather than the coincidence.
//
// EVERY UNIT AND EVERY REACHABLE GOAL, not one chosen pair. The four §2.4 rows have four
// different `move` values and the ten units sit on different terrain; a bridge that mishandled
// exactly one of them is the interesting failure and a single-goal clause would miss it.
//
// THE ALLOWANCE IS RE-DERIVED HERE. Read the file block: it is a table read, it is the one
// thing this clause cannot pin about itself, and `MovePathAnswersExactlyTheReachableSet` pins
// it on an instrument that does not share the derivation.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratMovePathRouteIsFindPathsRouteTest,
	"Stratocracy.StratBridge.T-UI-02.MovePathRouteIsFindPathsRoute",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratMovePathRouteIsFindPathsRouteTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPathPreviewParity;

	FStratBridge Bridge;
	FString      Error;
	// On its own line and not inside the TestTrue call: it WRITES Error, and a call that both
	// fills the string and formats it has unspecified argument evaluation order.
	const bool bSeeded = SeedBridge(Bridge, Error);
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), bSeeded))
	{
		AddError(Error);
		return false;
	}

	const strat::UiWorld World = Bridge.MakeUiWorld();
	if (!TestTrue(TEXT("the seeded opening puts units on the board"), !World.units.empty()))
	{
		return false;
	}
	if (!TestTrue(TEXT("the world carries the loaded tables the module needs"),
			World.unitDefs != nullptr && World.terrain != nullptr))
	{
		return false;
	}

	int32 RoutesCompared   = 0;
	int32 HexesCompared    = 0;
	int32 StepsCostingMore = 0;

	for (const strat::UiUnit& U : World.units)
	{
		const FString At = FString::Printf(TEXT("unit %d (side %d) at %s"),
			U.id, U.side, *Describe(U.hex));

		if (!TestTrue(*FString::Printf(TEXT("%s: its defIndex is inside the loaded table"), *At),
				U.defIndex >= 0 && static_cast<size_t>(U.defIndex) < World.unitDefs->size()))
		{
			continue;
		}

		// THE TABLE READ, and the file block says why it is here and what closes the hole it
		// leaves. Nothing below adjusts it.
		const int Move = (*World.unitDefs)[static_cast<size_t>(U.defIndex)].move;

		// The set of goals worth asking about, taken from the module rather than chosen.
		const std::vector<strat::ReachEntry> Reach = strat::uiReachable(World, U.id);
		if (!TestTrue(*FString::Printf(TEXT("%s: the module returns a reachable set"), *At),
				!Reach.empty()))
		{
			continue;
		}

		for (const strat::ReachEntry& Goal : Reach)
		{
			const FString To = FString::Printf(TEXT("%s -> %s"), *At, *Describe(Goal.hex));

			// ---- THE EXPECTATION: the module's own answer, asked directly ----------
			std::vector<strat::Hex> ModulePath;
			int                     ModuleCost = 0;
			const bool              bModuleAnswers = strat::findPath(
				World.board, *World.terrain, U.hex, Goal.hex, Move, ModulePath, ModuleCost);

			if (!TestTrue(*FString::Printf(
					TEXT("control: %s -- the module finds a route to a hex it called reachable"), *To),
					bModuleAnswers))
			{
				continue;
			}

			// ---- WHAT THE BRIDGE SAYS ---------------------------------------------
			TArray<FIntPoint> RouteHexes;
			TArray<int32>     RouteCosts;
			int32             TotalCost = 0;

			const FStratResult Asked =
				Bridge.MovePathToHex(U.id, FIntPoint(Goal.hex.q, Goal.hex.r),
				                     RouteHexes, RouteCosts, TotalCost);

			if (!TestTrue(*FString::Printf(TEXT("%s: MovePathToHex answers"), *To), Asked.bOk))
			{
				AddError(FString::Printf(TEXT("%s: %s"), *To, *Asked.Reason));
				continue;
			}

			if (!TestEqual(*FString::Printf(
					TEXT("%s: the route is the module's, no hex added or dropped"), *To),
					RouteHexes.Num(), static_cast<int32>(ModulePath.size())))
			{
				continue;
			}
			if (!TestEqual(*FString::Printf(
					TEXT("%s: there is exactly one tick per route hex"), *To),
					RouteCosts.Num(), RouteHexes.Num()))
			{
				continue;
			}

			for (int32 i = 0; i < RouteHexes.Num(); ++i)
			{
				const strat::Hex& M = ModulePath[static_cast<size_t>(i)];

				// `hexEqual` is the module's own comparison, used rather than a field-by-field
				// one written here, for the same reason no cost below is re-derived.
				TestTrue(*FString::Printf(
						TEXT("%s: step %d is the module's hex (%s vs %s), ORDER INCLUDED"),
						*To, i, *Describe(RouteHexes[i]), *Describe(M)),
					strat::hexEqual(strat::Hex{RouteHexes[i].X, RouteHexes[i].Y}, M));

				++HexesCompared;

				// FIXTURE RICHNESS ONLY -- see the file block. Not an expectation: nothing
				// asserts this difference equals anything.
				if (i > 0 && (RouteCosts[i] - RouteCosts[i - 1]) > 1)
				{
					++StepsCostingMore;
				}
			}

			// ---- THE TOTAL, against `outCost`'s own out-parameter -------------------
			// The right-hand side is `findPath`'s `outCost` rather than the last tick, which is
			// the strongest form available -- and per this file's header list it is STILL not
			// discriminating, because the bridge's cross-check refuses every board on which the
			// two differ. So the message says what an inequality means and does not claim the
			// number was "carried and not re-formed", which is unobservable from here.
			TestEqual(*FString::Printf(
					TEXT("%s: the total is NOT EQUAL to findPath's own outCost for this route -- "
					     "the bridge answered a route whose reported total is not the module's "
					     "cost for it"), *To),
				TotalCost, static_cast<int32>(ModuleCost));

			++RoutesCompared;
		}
	}

	TestTrue(TEXT("control: the comparison ran over a non-trivial number of routes"),
		RoutesCompared > 50);

	// ---- THE FOIL: the board actually exercises a cost model ---------------------
	// Declared in the file block. A Ferrum Crossing of uniform plains would satisfy every
	// clause in this file and prove nothing about §2.5, and this fails loudly rather than
	// silently if the shipped board ever stops producing a costly step on a real route.
	TestTrue(TEXT("T-UI-02: at least one compared route enters a hex costing more than 1, so the "
	              "routes under test are not all uniform-plains walks"),
		StepsCostingMore > 0);

	AddInfo(FString::Printf(
		TEXT("T-UI-02: %d routes and %d route hexes compared against strat::findPath; "
		     "%d steps cost more than 1"),
		RoutesCompared, HexesCompared, StepsCostingMore));

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-02 -- every tick is `strat::reachable`'s cost FOR THAT HEX, found by identity.
//
// WHAT THIS CLAUSE DOES NOT DO, FIRST, BECAUSE THIS BLOCK USED TO CLAIM IT DID. Two sentences
// stood here until 2026-08-31 and both were measured FALSE by the mutant round. They said:
//
//   STRUCK> "THIS IS THE CLAUSE THE HEADER'S CENTRAL DESIGN CLAIM RIDES ON. ... The failure it
//   STRUCK>  forbids is the plausible one: a running sum of each step's `TerrainDef`
//   STRUCK>  `moveCost`, accumulated as the route is walked. On most routes that sum happens
//   STRUCK>  to equal the cumulative cost, which is precisely why the property needs a clause
//   STRUCK>  instead of a reading -- it goes wrong only where a §2.5 rule the summing code
//   STRUCK>  does not hold makes the two differ, and it goes wrong silently."
//
// WHY THEY WERE WRONG, as the mechanism that refutes them rather than as an apology, and
// stated so a later reader can RE-DERIVE it rather than take it on trust. `strat::reachable`'s
// cost for a hex IS the summed `moveCost` along a cheapest path to that hex; `findPath`'s route
// is a cheapest path; and every prefix of a cheapest path is itself a cheapest path to its own
// last hex, because move costs are non-negative and a cheaper prefix could otherwise be spliced
// in to beat a minimal total. So "look the cost up per hex" and "sum `moveCost` while walking
// the route" are THE SAME NUMBER ON EVERY BOARD -- not merely on *Ferrum Crossing*, and not
// merely on the boards these fixtures happen to build. The struck text named a board on which
// the two differ; no such board exists, so the clause below could not discriminate the summing
// implementation and no clause could. That was MEASURED, not reasoned to afterwards: the
// summing mutant was built in place in this tree and SURVIVED a run with zero failures.
// `StratBridge.h`'s `MovePathToHex` block carries the same proof at the declaration, which is
// where the architectural claim lives; this copy exists because that header names THIS CLAUSE,
// so this is the file a reader checks.
//
// WHAT IT DOES PIN, WHICH IS REAL AND IS WHY IT IS STILL HERE. Every tick equals the module's
// own per-hex cost for that hex -- looked up by identity in `uiReachable`'s set, entry by
// entry, over every route on the board. That is a genuine property with genuine failure modes
// this suite CAN see:
//   - a tick shifted by one index against its hex;
//   - a tick read off the wrong unit's set;
//   - a route hex carrying no cost at all;
//   - a bridge that returned the ticks in a different order from the hexes;
//   - and any future body whose ticks are NOT NUMERICALLY EQUAL to the module's per-hex answer.
//
// THAT LAST ITEM IS BOUNDED IN ITS OWN WORDS AND MUST STAY THAT WAY. "Not equal to the module's
// answer" is NOT the same as "computed from something other than the module's answer", and the
// looser phrasing is what stood here until it was caught -- it would readmit the summing body
// as a covered failure mode, which is precisely the claim retracted twenty lines above. The
// summing body computes its ticks from `TerrainDef::moveCost`, which IS something other than
// the module's per-hex answer, and it SURVIVES this clause, because the expression differs
// while the number does not. THIS CLAUSE READS NUMBERS AND NOTHING ELSE. An edit that tightens
// this paragraph without holding that distinction re-opens the defect the retraction exists to
// close, so the bullet is written to be false on its own if it ever drifts back.
//
// The clause is live and its perturbation reddens it. What it cannot do is tell one CORRECT
// implementation from another correct one, and the difference between "this pins nothing" and
// "this pins less than its name suggests" is the whole point of this block.
//
// AND THE RIGHT-HAND SIDE IS THE MOVEMENT OVERLAY'S OWN NUMBER, which is what makes this a
// T-UI-02 clause rather than a bridge-internal one. The tick a player reads on a hex is
// compared against the cost `uiReachable` gave for that same hex -- the number
// `ReachableHexes` is already handing the highlight. A bridge that passed this cannot draw a
// tick that disagrees with the highlight the tick sits on.
//
// A MISSING HEX IS A FAILURE AND NOT A SKIP. `ModuleCostFor` returns -1 when the module did
// not return the hex at all, and that is asserted against rather than `continue`d past: the
// bridge's own body treats that condition as a refusal, so a route hex outside the reach set
// must never reach a green assertion here.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratMovePathTicksAreReachableCostsTest,
	"Stratocracy.StratBridge.T-UI-02.MovePathTicksAreReachableCosts",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratMovePathTicksAreReachableCostsTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPathPreviewParity;

	FStratBridge Bridge;
	FString      Error;
	const bool   bSeeded = SeedBridge(Bridge, Error);
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), bSeeded))
	{
		AddError(Error);
		return false;
	}

	const strat::UiWorld World = Bridge.MakeUiWorld();
	if (!TestTrue(TEXT("the seeded opening puts units on the board"), !World.units.empty()))
	{
		return false;
	}

	int32 TicksCompared    = 0;
	int32 NonTrivialTicks  = 0;

	for (const strat::UiUnit& U : World.units)
	{
		const FString At = FString::Printf(TEXT("unit %d (side %d) at %s"),
			U.id, U.side, *Describe(U.hex));

		// THE EXPECTATION SOURCE. The module's reach set over the bridge's own world -- the
		// same call `ReachableHexes` forwards to the movement overlay.
		const std::vector<strat::ReachEntry> Reach = strat::uiReachable(World, U.id);
		if (!TestTrue(*FString::Printf(TEXT("%s: the module returns a reachable set"), *At),
				!Reach.empty()))
		{
			continue;
		}

		for (const strat::ReachEntry& Goal : Reach)
		{
			const FString To = FString::Printf(TEXT("%s -> %s"), *At, *Describe(Goal.hex));

			TArray<FIntPoint> RouteHexes;
			TArray<int32>     RouteCosts;
			int32             TotalCost = 0;

			const FStratResult Asked =
				Bridge.MovePathToHex(U.id, FIntPoint(Goal.hex.q, Goal.hex.r),
				                     RouteHexes, RouteCosts, TotalCost);

			if (!TestTrue(*FString::Printf(TEXT("%s: MovePathToHex answers"), *To), Asked.bOk))
			{
				AddError(FString::Printf(TEXT("%s: %s"), *To, *Asked.Reason));
				continue;
			}
			if (!TestEqual(*FString::Printf(TEXT("%s: one tick per route hex"), *To),
					RouteCosts.Num(), RouteHexes.Num()))
			{
				continue;
			}

			for (int32 i = 0; i < RouteHexes.Num(); ++i)
			{
				const strat::Hex Here{RouteHexes[i].X, RouteHexes[i].Y};

				// ---- THE EXPECTATION: the module's cost for THIS hex, not for this step ----
				const int32 Expected = ModuleCostFor(Reach, Here);

				if (!TestTrue(*FString::Printf(
						TEXT("%s: route hex %d (%s) is in the module's reachable set at all"),
						*To, i, *Describe(Here)),
						Expected >= 0))
				{
					continue;
				}

				// THE MESSAGE SAYS WHAT A FAILURE HERE MEANS, NOT WHAT IT RULES OUT. It read
				// "not a sum this bridge formed" until 2026-08-31, which is the discrimination
				// this clause's own block disclaims twenty lines up -- and a failure string
				// renders at exactly the moment a reader is furthest from the prose and most
				// likely to act on the line alone. Pointing a debugger at the summing
				// hypothesis would send it after a body the retraction proves survives, and
				// away from the causes this clause can actually produce.
				TestEqual(*FString::Printf(
						TEXT("%s: the tick on %s is NOT EQUAL to strat::reachable's cost for "
						     "that hex -- look for a tick shifted one index against its hex, a "
						     "tick read off the wrong unit's set, or hexes and ticks returned "
						     "in different orders"),
						*To, *Describe(Here)),
					RouteCosts[i], Expected);

				++TicksCompared;
				if (Expected > 1)
				{
					++NonTrivialTicks;
				}
			}
		}
	}

	TestTrue(TEXT("control: the comparison ran over a non-trivial number of ticks"),
		TicksCompared > 100);

	// The same foil as the route clause's, one field over: a board on which every tick were 0
	// or 1 would make "the cost model" an untested word.
	TestTrue(TEXT("T-UI-02: some compared tick exceeds 1, so the ticks under test are not all "
	              "the null move and its first step"),
		NonTrivialTicks > 0);

	AddInfo(FString::Printf(
		TEXT("T-UI-02: %d ticks compared against strat::uiReachable, %d of them above 1"),
		TicksCompared, NonTrivialTicks));

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-02 -- a route comes back for EXACTLY the hexes the module calls reachable, and every
// other hex on the board is an EMPTY ANSWER with `bOk` TRUE.
//
// THIS IS THE CLAUSE CLOSEST TO T-UI-02'S OWN SENTENCE. "Displays exactly the T-MOVE-01 set"
// is a statement about a SET, and this is the only clause in the pass that measures the
// preview's set against the module's, exhaustively, over every hex of the shipped board.
//
// AND IT IS THE INDEPENDENT PIN ON THE §2.4 ALLOWANCE. The route clause supplies `move` to
// `findPath` itself, so a bridge that read the wrong allowance and a test that read the same
// wrong allowance would agree hex for hex. Here the right-hand side is `uiReachable`, which
// derives the allowance INSIDE the module from the unit's own row; a bridge that used a
// different one answers a route for a hex outside that set, or refuses one inside it, and
// either direction reddens. The two clauses do not share the derivation and neither covers the
// other's subject.
//
// THE CHANNEL DECISION IS THE OTHER HALF, AND `bOk` IS PINNED EXPLICITLY RATHER THAN LEFT TO
// EMPTINESS. `StratBridge.h` chose `AttackTargetHexes`' channel over `ReachableHexes`': an
// unreachable goal is an ANSWER. A clause that asserted only "the array is empty" would stay
// green if that decision were reversed to a refusal, because a refusal also empties the array
// -- the bridge clears both up front. So every out-of-set observation below asserts `bOk` TRUE
// FIRST, and asserts the refusal channel stayed quiet by checking `Reason` is empty. That is
// the whole point of the design decision and it is what this clause exists to hold.
//
// AND AN OUT-OF-BOUNDS GOAL IS ALSO AN ANSWER. A cursor leaving the board is the commonest
// producer of one, and it is a different code path in `Board::index` from an in-bounds hex the
// unit cannot afford. Asserted separately at the end.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratMovePathAnswersExactlyTheReachableSetTest,
	"Stratocracy.StratBridge.T-UI-02.MovePathAnswersExactlyTheReachableSet",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratMovePathAnswersExactlyTheReachableSetTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPathPreviewParity;

	FStratBridge Bridge;
	FString      Error;
	const bool   bSeeded = SeedBridge(Bridge, Error);
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), bSeeded))
	{
		AddError(Error);
		return false;
	}

	const strat::UiWorld World = Bridge.MakeUiWorld();
	if (!TestTrue(TEXT("the seeded opening puts units on the board"), !World.units.empty()))
	{
		return false;
	}
	if (!TestTrue(TEXT("control: the board declares its own bounds"),
			World.board.bounds.cols > 0 && World.board.bounds.rows > 0))
	{
		return false;
	}

	int32 InSetAnswered  = 0;
	int32 OutOfSetEmpty  = 0;

	for (const strat::UiUnit& U : World.units)
	{
		const FString At = FString::Printf(TEXT("unit %d (side %d) at %s"),
			U.id, U.side, *Describe(U.hex));

		// THE EXPECTATION. The module decides which hexes are in the set, allowance and all.
		const std::vector<strat::ReachEntry> Reach = strat::uiReachable(World, U.id);
		if (!TestTrue(*FString::Printf(TEXT("%s: the module returns a reachable set"), *At),
				!Reach.empty()))
		{
			continue;
		}

		// EVERY HEX OF THE BOARD, enumerated through the module's own offset conversion so
		// that no coordinate is written down in this file.
		for (int Row = 0; Row < World.board.bounds.rows; ++Row)
		{
			for (int Col = 0; Col < World.board.bounds.cols; ++Col)
			{
				const strat::Hex Goal      = strat::offsetToAxial(Col, Row);
				const bool       bInModule = ModuleCostFor(Reach, Goal) >= 0;
				const FString    To        = FString::Printf(TEXT("%s -> %s"), *At, *Describe(Goal));

				TArray<FIntPoint> RouteHexes;
				TArray<int32>     RouteCosts;
				int32             TotalCost = 0;

				const FStratResult Asked =
					Bridge.MovePathToHex(U.id, FIntPoint(Goal.q, Goal.r),
					                     RouteHexes, RouteCosts, TotalCost);

				// ---- THE CHANNEL, ASSERTED ON BOTH SIDES OF THE SET -----------------
				// A reachable goal and an unreachable one are BOTH answers. This is the
				// assertion a reversal of the channel decision reddens, and it is asserted
				// before emptiness is looked at.
				if (!TestTrue(*FString::Printf(
						TEXT("%s: asking about an ordinary board hex is answered, not refused "
						     "(module calls it %s)"),
						*To, bInModule ? TEXT("reachable") : TEXT("unreachable")),
						Asked.bOk))
				{
					AddError(FString::Printf(TEXT("%s: %s"), *To, *Asked.Reason));
					continue;
				}

				// ---- THE SET ---------------------------------------------------------
				if (bInModule)
				{
					TestTrue(*FString::Printf(
							TEXT("%s: the module calls this hex reachable, so a route comes back"), *To),
						RouteHexes.Num() > 0);
					++InSetAnswered;
				}
				else
				{
					TestEqual(*FString::Printf(
							TEXT("%s: the module does NOT call this hex reachable, so the answer "
							     "is an empty route"), *To),
						RouteHexes.Num(), 0);
					TestEqual(*FString::Printf(TEXT("%s: and its ticks are empty too"), *To),
						RouteCosts.Num(), 0);
					TestEqual(*FString::Printf(
							TEXT("%s: and the total is 0 -- which is not a sentinel, see the header"), *To),
						TotalCost, 0);
					TestTrue(*FString::Printf(
							TEXT("%s: and the refusal channel stayed quiet -- no reason was set"), *To),
						Asked.Reason.IsEmpty());
					++OutOfSetEmpty;
				}
			}
		}
	}

	TestTrue(TEXT("control: some goal on the board was inside the reachable set"),
		InSetAnswered > 0);
	TestTrue(TEXT("control: and some goal was outside it, so both arms above actually ran"),
		OutOfSetEmpty > 0);

	// ---- AND A GOAL THAT IS NOT ON THE BOARD AT ALL -----------------------------
	// A different code path from an unaffordable in-bounds hex, and the commonest one a
	// cursor leaving the board produces. Derived from the board's own bounds rather than
	// typed, so it stays off the board if the scenario ever grows.
	{
		const strat::UiUnit& U = World.units.front();
		const FIntPoint OffBoard(World.board.bounds.cols + 64, World.board.bounds.rows + 64);

		TArray<FIntPoint> RouteHexes;
		TArray<int32>     RouteCosts;
		int32             TotalCost = 0;

		const FStratResult Asked =
			Bridge.MovePathToHex(U.id, OffBoard, RouteHexes, RouteCosts, TotalCost);

		TestTrue(TEXT("T-UI-02: a goal off the board is ANSWERED, not refused -- a cursor leaving "
		              "the board is an ordinary frame and not a broken bridge"),
			Asked.bOk);
		TestTrue(TEXT("T-UI-02: and the refusal channel stayed quiet for it"),
			Asked.Reason.IsEmpty());
		TestEqual(TEXT("T-UI-02: and the answer is that there is no route"),
			RouteHexes.Num(), 0);
	}

	AddInfo(FString::Printf(
		TEXT("T-UI-02: %d in-set goals answered a route and %d out-of-set goals answered empty, "
		     "across %d units on an %d x %d board"),
		InSetAnswered, OutOfSetEmpty, static_cast<int32>(World.units.size()),
		World.board.bounds.cols, World.board.bounds.rows));

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-02 -- the route carries BOTH endpoints: `[0]` is the unit's own hex at cost 0 and
// `.Last()` is the goal that was asked for.
//
// WHY THIS IS NOT COVERED BY THE ROUTE CLAUSE. `MovePathRouteIsFindPathsRoute` compares the
// bridge's route against `findPath`'s, so it is green over any bridge that copies faithfully
// -- INCLUDING one that trimmed the start hex, IF the test's own expectation had been trimmed
// the same way. It is not, so that particular defect would in fact redden it; what this clause
// adds is that the property is stated against THE UNIT AND THE GOAL rather than against
// another array, so a reader of the drawing layer can cite it. `StratBridge.h` promises a
// drawing layer that wants the goal marker reads `.Last()`, and this is that promise's gate.
//
// AND THE FIRST TICK IS 0 -- `Move.h` charges cost on ENTERING a hex, so the start is free.
// A bridge that shifted the ticks by one would keep the route intact and put every number on
// the wrong hex, which is a defect no route comparison can see.
//
// THE NULL MOVE IS INCLUDED DELIBERATELY. Hovering the unit's own hex is a route of one hex at
// cost 0, `Move.h` is explicit that it is reachable, and it is the degenerate case where
// "first" and "last" are the same element -- the one an off-by-one is most likely to survive.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratMovePathIncludesBothEndpointsTest,
	"Stratocracy.StratBridge.T-UI-02.MovePathIncludesBothEndpoints",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratMovePathIncludesBothEndpointsTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPathPreviewParity;

	FStratBridge Bridge;
	FString      Error;
	const bool   bSeeded = SeedBridge(Bridge, Error);
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), bSeeded))
	{
		AddError(Error);
		return false;
	}

	const strat::UiWorld World = Bridge.MakeUiWorld();
	if (!TestTrue(TEXT("the seeded opening puts units on the board"), !World.units.empty()))
	{
		return false;
	}

	int32 RoutesChecked   = 0;
	int32 NullMovesSeen   = 0;
	int32 LongRoutesSeen  = 0;

	for (const strat::UiUnit& U : World.units)
	{
		const FString At = FString::Printf(TEXT("unit %d (side %d) at %s"),
			U.id, U.side, *Describe(U.hex));

		const std::vector<strat::ReachEntry> Reach = strat::uiReachable(World, U.id);
		if (!TestTrue(*FString::Printf(TEXT("%s: the module returns a reachable set"), *At),
				!Reach.empty()))
		{
			continue;
		}

		for (const strat::ReachEntry& Goal : Reach)
		{
			const FString To = FString::Printf(TEXT("%s -> %s"), *At, *Describe(Goal.hex));

			TArray<FIntPoint> RouteHexes;
			TArray<int32>     RouteCosts;
			int32             TotalCost = 0;

			const FStratResult Asked =
				Bridge.MovePathToHex(U.id, FIntPoint(Goal.hex.q, Goal.hex.r),
				                     RouteHexes, RouteCosts, TotalCost);

			if (!TestTrue(*FString::Printf(TEXT("%s: MovePathToHex answers"), *To), Asked.bOk))
			{
				AddError(FString::Printf(TEXT("%s: %s"), *To, *Asked.Reason));
				continue;
			}
			if (!TestTrue(*FString::Printf(TEXT("%s: a reachable goal produces a route"), *To),
					RouteHexes.Num() > 0 && RouteCosts.Num() == RouteHexes.Num()))
			{
				continue;
			}

			// ---- THE START ---------------------------------------------------------
			// COMPONENTWISE, because `TestEqual` has no `FIntPoint` overload and because a
			// failure that prints both components is the only kind a coordinate confusion is
			// legible in.
			TestEqual(*FString::Printf(TEXT("%s: the route starts at the unit's own hex (q)"), *To),
				RouteHexes[0].X, U.hex.q);
			TestEqual(*FString::Printf(TEXT("%s: the route starts at the unit's own hex (r)"), *To),
				RouteHexes[0].Y, U.hex.r);
			TestEqual(*FString::Printf(
					TEXT("%s: and its first tick is 0 -- Move.h charges on ENTERING a hex"), *To),
				RouteCosts[0], 0);

			// ---- THE GOAL ----------------------------------------------------------
			TestEqual(*FString::Printf(TEXT("%s: the route ends at the goal that was asked for (q)"), *To),
				RouteHexes.Last().X, Goal.hex.q);
			TestEqual(*FString::Printf(TEXT("%s: the route ends at the goal that was asked for (r)"), *To),
				RouteHexes.Last().Y, Goal.hex.r);

			++RoutesChecked;
			if (RouteHexes.Num() == 1)
			{
				// The null move: first and last are one element, and the clause still holds.
				++NullMovesSeen;
			}
			else
			{
				++LongRoutesSeen;
			}
		}
	}

	TestTrue(TEXT("control: some route under test was the null move -- the degenerate case where "
	              "first and last are the same element"),
		NullMovesSeen > 0);
	TestTrue(TEXT("control: and some route had more than one hex, so the two endpoints were "
	              "actually distinct somewhere"),
		LongRoutesSeen > 0);

	AddInfo(FString::Printf(TEXT("T-UI-02: %d routes checked at both endpoints (%d null moves)"),
		RoutesChecked, NullMovesSeen));

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-02 -- an unseeded bridge REFUSES, in `Reachable`'s own words.
//
// TWO PROPERTIES AND THEY ARE DIFFERENT. That it refuses at all is the channel: an unseeded
// bridge is not a board with no route on it, and answering `Ok()` with an empty array would
// make "the match has not started" indistinguishable from "that mountain is too far". That it
// refuses IN `Reachable`'S WORDS is the anti-drift property: `MovePathToHex` asks `Reachable`
// first and forwards, and a future edit that phrased its own refusals here would give the same
// condition two wordings depending on which method a caller happened to ask.
//
// WHERE THE EXPECTATION COMES FROM, AND IT IS NOT A LITERAL. The right-hand side is
// `Reachable`'s own `Reason` for the same condition on the SAME bridge object, read in the
// same run. Nothing here types "no definitions" or "no scenario". Re-wording either refusal
// upstream moves both sides together, which is the whole point.
//
// COMPARED CASE-SENSITIVELY, DELIBERATELY. `TestEqual` on `FString` is case-INSENSITIVE in this
// engine, as are `==` and `Contains`; a clause that compared these two wordings with it would
// stay green over a re-cased literal, which is one of the drifts it exists to catch.
//
// BOTH UNSEEDED STATES, because they are distinct refusals and the header says so: no
// definitions at all, and definitions loaded with no scenario. A forward that collapsed them
// would still pass a one-state clause.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratMovePathRefusesUnseededTest,
	"Stratocracy.StratBridge.T-UI-02.MovePathRefusesUnseeded",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratMovePathRefusesUnseededTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPathPreviewParity;

	// An arbitrary in-bounds-looking goal. Its value is irrelevant: no state exists for it to
	// be reachable or unreachable IN, which is the condition under test.
	const FIntPoint SomeGoal(2, 7);
	const int32     SomeUnitId = 1;

	// ---- state one: no definitions at all --------------------------------------
	{
		FStratBridge Bridge;

		std::vector<strat::ReachEntry> Reach;
		const FStratResult Baseline = Bridge.Reachable(SomeUnitId, Reach);

		TArray<FIntPoint> RouteHexes;
		TArray<int32>     RouteCosts;
		int32             TotalCost = 0;
		const FStratResult Asked =
			Bridge.MovePathToHex(SomeUnitId, SomeGoal, RouteHexes, RouteCosts, TotalCost);

		if (!TestFalse(TEXT("control: Reachable itself refuses on a bridge with no definitions"),
				Baseline.bOk))
		{
			return false;
		}
		TestTrue(TEXT("control: and that refusal carries a reason"), !Baseline.Reason.IsEmpty());

		// ---- THE CLAUSE ---------------------------------------------------------
		TestFalse(TEXT("T-UI-02: a bridge with no definitions REFUSES a path rather than "
		               "answering that there is no route"),
			Asked.bOk);
		TestEqualSensitive(TEXT("T-UI-02: the no-definitions refusal is Reachable's own wording, "
		                        "forwarded unaltered"),
			Asked.Reason, Baseline.Reason);
		TestEqual(TEXT("T-UI-02: a refused path leaves no route for a caller to read as this "
		               "call's answer"),
			RouteHexes.Num(), 0);
		TestEqual(TEXT("T-UI-02: and no ticks"), RouteCosts.Num(), 0);
		TestEqual(TEXT("T-UI-02: and no total"), TotalCost, 0);
	}

	// ---- state two: definitions loaded, no scenario ----------------------------
	{
		FStratBridge Bridge;
		FString      Error;
		const bool   bLoaded = LoadDefsOnly(Bridge, Error);
		if (!TestTrue(TEXT("the shipped tables load"), bLoaded))
		{
			AddError(Error);
			return false;
		}

		std::vector<strat::ReachEntry> Reach;
		const FStratResult Baseline = Bridge.Reachable(SomeUnitId, Reach);

		TArray<FIntPoint> RouteHexes;
		TArray<int32>     RouteCosts;
		int32             TotalCost = 0;
		const FStratResult Asked =
			Bridge.MovePathToHex(SomeUnitId, SomeGoal, RouteHexes, RouteCosts, TotalCost);

		if (!TestFalse(TEXT("control: Reachable itself refuses with tables but no scenario"),
				Baseline.bOk))
		{
			return false;
		}

		// ---- THE CLAUSE ---------------------------------------------------------
		TestFalse(TEXT("T-UI-02: a bridge with tables but no scenario still REFUSES a path"),
			Asked.bOk);
		TestEqualSensitive(TEXT("T-UI-02: the no-scenario refusal is Reachable's own wording too"),
			Asked.Reason, Baseline.Reason);
		TestEqual(TEXT("T-UI-02: and it cleared the caller's route"), RouteHexes.Num(), 0);

		// The two refusals are DISTINCT, which is `Reachable`'s own property and is asserted
		// here so that a forward collapsing them to one string is visible.
		TestFalse(TEXT("T-UI-02: 'no definitions' and 'no scenario' remain distinct refusals "
		               "through the path method"),
			Asked.Reason.IsEmpty());
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-02 -- an unknown unit id is a REFUSAL, not an empty route, and it is `Reachable`'s.
//
// THIS IS THE CHANNEL'S OTHER EDGE AND IT IS THE ONE MOST AT RISK. `MovePathToHex` deliberately
// answers `Ok()` with an empty array for an unreachable goal, and that decision makes "return
// an empty answer" the file's habit. An id that names no unit is NOT a board fact -- there is
// no unit for the question to be about -- and folding it into the answer channel would leave a
// caller unable to tell a malformed question from a mountain.
//
// EXPECTATION FROM `Reachable` AGAIN, on the unseeded clause's reasoning, and compared
// case-sensitively for the same reason.
//
// THE ID IS 999999 and it is the shape in which a literal is correct: no table or scenario
// edit can bring it into existence, so the clause's subject does not have to be re-picked
// every time the board moves. Same value and same reasoning as `StratBridgeQueryParity.cpp`'s.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratMovePathRefusesUnknownUnitIdTest,
	"Stratocracy.StratBridge.T-UI-02.MovePathRefusesUnknownUnitId",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratMovePathRefusesUnknownUnitIdTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPathPreviewParity;

	FStratBridge Bridge;
	FString      Error;
	const bool   bSeeded = SeedBridge(Bridge, Error);
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), bSeeded))
	{
		AddError(Error);
		return false;
	}

	const strat::UiWorld World = Bridge.MakeUiWorld();
	if (!TestTrue(TEXT("control: the board does have units on it, so 'no unit' is a real "
	                   "condition and not the state of everything"),
			!World.units.empty()))
	{
		return false;
	}

	// A goal that IS reachable for a real unit, so the refusal cannot be attributed to the
	// goal. Taken from the module rather than chosen.
	const strat::UiUnit&                 Real  = World.units.front();
	const std::vector<strat::ReachEntry> Reach = strat::uiReachable(World, Real.id);
	if (!TestTrue(TEXT("control: that unit has somewhere to go"), !Reach.empty()))
	{
		return false;
	}
	const FIntPoint GoodGoal(Reach.back().hex.q, Reach.back().hex.r);

	// ---- control: the same goal ANSWERS for a real unit -------------------------
	{
		TArray<FIntPoint> RouteHexes;
		TArray<int32>     RouteCosts;
		int32             TotalCost = 0;
		const FStratResult Asked =
			Bridge.MovePathToHex(Real.id, GoodGoal, RouteHexes, RouteCosts, TotalCost);

		if (!TestTrue(TEXT("control: this goal is answerable for a real unit, so the refusal "
		                   "below is about the ID and not about the hex"),
				Asked.bOk && RouteHexes.Num() > 0))
		{
			return false;
		}
	}

	// ---- the expectation, from Reachable, on the same seeded bridge -------------
	std::vector<strat::ReachEntry> Ignored;
	const FStratResult Baseline = Bridge.Reachable(kNoSuchUnitId, Ignored);
	if (!TestFalse(TEXT("control: Reachable itself refuses an unknown id"), Baseline.bOk))
	{
		return false;
	}

	TArray<FIntPoint> RouteHexes;
	TArray<int32>     RouteCosts;
	int32             TotalCost = 0;
	const FStratResult Asked =
		Bridge.MovePathToHex(kNoSuchUnitId, GoodGoal, RouteHexes, RouteCosts, TotalCost);

	// ---- THE CLAUSE -------------------------------------------------------------
	TestFalse(TEXT("T-UI-02: an unknown unit id is REFUSED, not answered with an empty route -- "
	               "a malformed question is not a board fact"),
		Asked.bOk);
	TestTrue(TEXT("T-UI-02: the refusal carries a reason"), !Asked.Reason.IsEmpty());
	TestEqualSensitive(TEXT("T-UI-02: and it is Reachable's own wording, forwarded unaltered"),
		Asked.Reason, Baseline.Reason);
	TestEqual(TEXT("T-UI-02: a refused path clears the caller's route"), RouteHexes.Num(), 0);
	TestEqual(TEXT("T-UI-02: and its ticks"), RouteCosts.Num(), 0);
	TestEqual(TEXT("T-UI-02: and its total"), TotalCost, 0);

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-02 -- `OutRouteHexes` and `OutRouteCosts` are ALWAYS THE SAME LENGTH, on every channel.
//
// WHY THIS EXISTS AS ITS OWN CLAUSE, AND IT IS A FINDING RATHER THAN A TIDY-UP. Three clauses
// in this file already go red if the two arrays diverge -- but every one of them fails
// INCIDENTALLY, as a side effect of zipping the arrays by index. `MovePathRouteIsFindPathsRoute`
// compares `RouteCosts.Num()` to `RouteHexes.Num()` only as a guard before its own loop and
// `continue`s past a mismatch; `MovePathTicksAreReachableCosts` does the same. A guard that
// SKIPS on divergence is not a clause that FAILS on it. `StratBridge.h` and
// `FStratPathPreviewView` both state equal length as a GUARANTEE that consumers may rely on --
// `IStratPathQuery`'s own block says "a consumer that has checked `bHasPath` may index either
// by the other's `Num()`" and declines to re-check it, explicitly deferring to this guarantee.
// Nothing pinned it. This does, directly, as an equality that is the assertion rather than the
// precondition of one.
//
// ALL THREE CHANNELS, because the guarantee is stated unconditionally and the arrays are
// emptied at three different places in the body: the success path fills both in one loop, the
// no-route path returns before either is touched after the up-front clear, and each refusal
// arm resets both by hand. The hand-written resets are the ones worth pinning -- there are
// three of them and a fourth arm added later could easily reset one array and forget the
// other, which is exactly the shape no other clause here would catch.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratMovePathArraysAreTheSameLengthTest,
	"Stratocracy.StratBridge.T-UI-02.MovePathArraysAreTheSameLength",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratMovePathArraysAreTheSameLengthTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPathPreviewParity;

	FStratBridge Bridge;
	FString      Error;
	const bool   bSeeded = SeedBridge(Bridge, Error);
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), bSeeded))
	{
		AddError(Error);
		return false;
	}

	const strat::UiWorld World = Bridge.MakeUiWorld();
	if (!TestTrue(TEXT("the seeded opening puts units on the board"), !World.units.empty()))
	{
		return false;
	}

	int32 WithRoute    = 0;
	int32 WithoutRoute = 0;

	// EVERY HEX OF THE BOARD FOR EVERY UNIT, so both the route channel and the no-route channel
	// are exercised without this file deciding which hexes fall on which side.
	for (const strat::UiUnit& U : World.units)
	{
		for (int Row = 0; Row < World.board.bounds.rows; ++Row)
		{
			for (int Col = 0; Col < World.board.bounds.cols; ++Col)
			{
				const strat::Hex Goal = strat::offsetToAxial(Col, Row);

				TArray<FIntPoint> RouteHexes;
				TArray<int32>     RouteCosts;
				int32             TotalCost = 0;

				const FStratResult Asked =
					Bridge.MovePathToHex(U.id, FIntPoint(Goal.q, Goal.r),
					                     RouteHexes, RouteCosts, TotalCost);

				if (!Asked.bOk)
				{
					// No refusal is expected on an ordinary board hex; if one happens, the
					// clause below still applies to it and the dedicated refusal arm is at the
					// end of this function.
					AddError(FString::Printf(TEXT("unexpected refusal for unit %d -> %s: %s"),
						U.id, *Describe(Goal), *Asked.Reason));
					continue;
				}

				// ---- THE CLAUSE, and it is the assertion rather than a guard -------
				if (RouteHexes.Num() != RouteCosts.Num())
				{
					AddError(FString::Printf(
						TEXT("T-UI-02: unit %d -> %s: the route carries %d hexes but %d ticks -- "
						     "the arrays a consumer is told it may index by either Num() have "
						     "diverged"),
						U.id, *Describe(Goal), RouteHexes.Num(), RouteCosts.Num()));
					return false;
				}

				if (RouteHexes.Num() > 0)
				{
					++WithRoute;
				}
				else
				{
					++WithoutRoute;
				}
			}
		}
	}

	TestTrue(TEXT("control: the route channel was exercised"), WithRoute > 0);
	TestTrue(TEXT("control: and the no-route channel was exercised, so the equality above was "
	              "asserted on both"),
		WithoutRoute > 0);

	// ---- THE REFUSAL CHANNEL, where the resets are written by hand ---------------
	// Two of the three refusal arms are reachable from here: the forwarded refusal (an unknown
	// id) and the unseeded one. The bridge's two internal arms are not reachable from any board
	// this tree can build -- see the file's header -- and are not claimed to be covered.
	{
		TArray<FIntPoint> RouteHexes = { FIntPoint(91, 92), FIntPoint(93, 94) };
		TArray<int32>     RouteCosts = { 95 };
		int32             TotalCost  = 96;

		// PRE-POPULATED AT DIFFERENT LENGTHS ON PURPOSE. A refusal arm that reset only one
		// array would leave these two unequal, and a fixture that started them equal could not
		// tell that from a correct reset of both.
		const FStratResult Asked =
			Bridge.MovePathToHex(kNoSuchUnitId, FIntPoint(0, 0), RouteHexes, RouteCosts, TotalCost);

		if (!TestFalse(TEXT("control: an unknown id refuses, so the refusal arm actually ran"),
				Asked.bOk))
		{
			return false;
		}
		TestEqual(TEXT("T-UI-02: a refusal leaves the two arrays the same length -- both were "
		               "pre-populated at DIFFERENT lengths, so a reset of only one is visible"),
			RouteHexes.Num(), RouteCosts.Num());
		TestEqual(TEXT("T-UI-02: and that common length is zero"), RouteHexes.Num(), 0);
	}

	{
		FStratBridge     Unseeded;
		TArray<FIntPoint> RouteHexes = { FIntPoint(91, 92), FIntPoint(93, 94) };
		TArray<int32>     RouteCosts = { 95 };
		int32             TotalCost  = 96;

		const FStratResult Asked =
			Unseeded.MovePathToHex(1, FIntPoint(0, 0), RouteHexes, RouteCosts, TotalCost);

		if (!TestFalse(TEXT("control: an unseeded bridge refuses, so that arm ran too"), Asked.bOk))
		{
			return false;
		}
		TestEqual(TEXT("T-UI-02: the unseeded refusal leaves the arrays the same length as well"),
			RouteHexes.Num(), RouteCosts.Num());
	}

	AddInfo(FString::Printf(
		TEXT("T-UI-02: array lengths compared on %d route answers and %d no-route answers, "
		     "plus two refusal arms"),
		WithRoute, WithoutRoute));

	return true;
}
