// The editor pass for the bridge's two routed rules queries -- T-UI-02 and T-UI-01.
// GDD §2.5, §2.6, §2.11, §4.7 Stub 8, §4.9 part 2.
//
//   T-UI-02  the movement-highlight set is the module's `reachable` set, queried and
//            never recomputed (§2.5).
//   T-UI-01  the pre-commit forecast is exactly what resolves (§2.6) -- the bridge
//            forwards `strat::uiForecast` and computes no field of it.
//
// WHY THIS FILE IS IN StratBridge AND COULD NOT BE ANYWHERE ELSE. Every clause below
// compares an `FStratBridge` answer against the `strat::` answer for the same
// question, so the file has to be able to CALL both. The vendored rules sources carry
// no `_API` macro (§4.9 forbids them engine headers), so `UnrealEditor-StratBridge.dll`
// exports `FStratBridge` and nothing beside it -- the 8 x LNK2019 StratBridge.h opens
// with, and the single LNK2019 the StratUI pass measured on
// `strat::uiCheckSnapshotFidelity`. This module compiles the rules sources INTO itself
// via Vendored/*.strat.cpp, so a test .cpp here links both sides. A parity test of this
// shape written in StratUI or StratPlay is not a weaker test, it is a link error.
//
// WHERE EVERY EXPECTATION COMES FROM, since that is the only question that matters
// about a parity file. `MatchesModuleQuery` and `ForecastMatchesModule` take their
// right-hand side from `strat::uiReachable` / `strat::uiForecast` over the bridge's own
// `UiWorld`. Nothing here re-costs a path, re-rolls damage, or transcribes a table of
// hexes out of the scenario file. The one number this file writes down is `999999`, a
// unit id chosen because no table or scenario edit can make it exist -- the same choice
// and the same reason as StratBridgeParity.cpp's illegal command.
//
// THE ONE COMPUTED SET, DECLARED. `NotHexDistance` builds a naive
// `hexDistance(start, h) <= move` filter and asserts the real set DIFFERS from it.
// That filter is a FOIL, not an expectation: no clause asserts the reachable set
// EQUALS it, and the equality clause above still takes its expectation from the
// module. The foil exists because `MatchesModuleQuery` alone is satisfied by a bridge
// AND a module that had both regressed to a distance filter -- and more practically,
// by a future gameplay actor that skipped `Reachable` entirely. StratBridge.h states
// that the distance filter "agrees with the rules on an empty plain and lies
// everywhere else"; this clause is the measurement of that sentence on the shipped
// board, and it fails loudly rather than silently if Ferrum Crossing ever stops
// producing a divergence.
//
// THE POSITION UNDER TEST IS THE SEEDED OPENING for T-UI-02 and a REPLAYED PREFIX of
// the committed parity fixture for T-UI-01, and the difference is forced rather than
// chosen. Reachability is interesting at the opening: Ferrum Crossing's water column,
// its woods and mountains, and its five friendly units per side all bite immediately.
// An attack is not -- both sides deploy on opposite edges and no unit is in range of
// anything on turn 1, so a forecast at the opening could only ever exercise the
// illegal channel. `Data/parity_fixture.save` is the only mid-match position this tree
// can reach deterministically, and its own entries name which attacks were legal
// there, so the prefix supplies the attacker and the target without this file
// inventing either.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBridge.h"

// The vendored rules module, by short name via StratRules' PublicIncludePaths. Named
// explicitly rather than left to the transitive include through StratBridge.h, because
// this file calls into all four directly and a reader should not have to discover that.
#include "Hex.h"
#include "Move.h"
#include "Save.h"
#include "Ui.h"

namespace StratBridgeQueryParity
{
	// The same pin StratBridgeParity.cpp and StratScoreboardParity.cpp both carry, and
	// it must stay equal to theirs. Replay.h states that no rule decides which side
	// moves first, so this is a harness constant; the parity fixture's log was emitted
	// against side 0 moving first and replaying its prefix under any other value would
	// be replaying a different game.
	static const int32 kFirstSide = 0;

	// A unit id no table or scenario edit can bring into existence. Same value and same
	// reasoning as StratBridgeParity.cpp's illegal Move: a check whose subject has to be
	// re-picked every time the board moves is a check that gets quietly relaxed instead.
	static const int32 kNoSuchUnitId = 999999;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static std::string ToStd(const FString& In)
	{
		return std::string(TCHAR_TO_UTF8(*In));
	}

	static FString FromStd(const std::string& In)
	{
		return FString(UTF8_TO_TCHAR(In.c_str()));
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** Both axial and odd-r offset, because the scenario file authors the second. */
	static FString Describe(const strat::Hex& H)
	{
		int Col = 0;
		int Row = 0;
		strat::axialToOffset(H, Col, Row);
		return FString::Printf(TEXT("axial(%d,%d)=offset[%d,%d]"), H.q, H.r, Col, Row);
	}

	/** Definitions only -- the bridge state between the two refusal reasons. */
	static bool LoadDefsOnly(FStratBridge& Bridge, FString& OutError)
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
		return true;
	}

	/**
	 * The seeded opening: shipped tables mapped in, shipped scenario seeded through the
	 * rules module's own `seedFromScenario`.
	 *
	 * Returns false and fills OutError rather than asserting, on StratBridgeParity's
	 * precedent -- a missing asset becomes a named failure inside the caller's own
	 * clause list rather than a crash in a helper.
	 */
	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		if (!LoadDefsOnly(Bridge, OutError))
		{
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

	/** Reads the committed §4.10 fixture through the rules module's own parser. */
	static bool LoadFixture(strat::Save& Out, FString& OutError)
	{
		const FString Path = FPaths::Combine(DataDir(), TEXT("parity_fixture.save"));
		FString Text;
		if (!FFileHelper::LoadFileToString(Text, *Path))
		{
			OutError = FString::Printf(TEXT("parity_fixture.save unreadable at %s"), *Path);
			return false;
		}

		const strat::SaveLoadResult Parsed =
			strat::parseSave(ToStd(Text), "parity_fixture.save", Out);
		if (!Parsed.ok)
		{
			OutError = FString::Printf(TEXT("%s: %s"),
				*FromStd(Parsed.failedId), *FromStd(Parsed.reason));
			return false;
		}
		return true;
	}

	/**
	 * Seeds and then replays the fixture's first `PrefixLength` commands, leaving the
	 * bridge at the mid-match position the fixture's own log reached at that index.
	 *
	 * WHY A PREFIX AND NOT THE WHOLE LOG: the fixture ends `Decisive` on turn 7, and a
	 * finished match answers no forecast worth asking. The index is chosen by the
	 * caller and every caller says beside its own call why that index.
	 */
	static bool SeedAndReplayPrefix(FStratBridge& Bridge, const strat::Save& Fixture,
	                                int32 PrefixLength, FString& OutError)
	{
		if (!SeedBridge(Bridge, OutError))
		{
			return false;
		}
		if (PrefixLength < 0 || PrefixLength > static_cast<int32>(Fixture.commandLog.size()))
		{
			OutError = FString::Printf(
				TEXT("prefix %d is outside the fixture's %d commands"),
				PrefixLength, static_cast<int32>(Fixture.commandLog.size()));
			return false;
		}

		TArray<strat::SaveCommand> Prefix;
		Prefix.Reserve(PrefixLength);
		for (int32 i = 0; i < PrefixLength; ++i)
		{
			Prefix.Add(Fixture.commandLog[static_cast<size_t>(i)]);
		}

		const FStratResult Replayed = Bridge.ReplayLog(Prefix);
		if (!Replayed.bOk)
		{
			OutError = FString::Printf(TEXT("replaying the fixture's first %d commands refused: %s"),
				PrefixLength, *Replayed.Reason);
			return false;
		}
		return true;
	}

	/**
	 * INDEX 17 OF THE FIXTURE IS AN ATTACK, and this file uses that entry as its
	 * legal-forecast subject: replay [0,17) and the very next thing the emitting build
	 * did was attack. The fixture is a vendored artifact under GATE-DATA-VENDOR, so the
	 * attacker and the target hex both arrive from it rather than from a hex this file
	 * picked and would have to re-pick whenever the AI or the board moved. If the
	 * fixture is ever re-emitted and index 17 stops being an Attack, the clause below
	 * says so in as many words instead of silently forecasting something else.
	 */
	static const int32 kAttackIndex = 17;
}

// ---------------------------------------------------------------------------
// T-UI-02 clause 1 -- the bridge's set IS the module's set.
//
// Entry for entry and IN ORDER, because `reachable` returns canonical hex order and a
// highlight layer that re-sorted would still draw the right hexes today and the wrong
// path preview tomorrow. Cost is compared too: a bridge that forwarded the hexes but
// dropped the costs would satisfy a hex-only comparison and leave §2.11's move-cost
// readout with nothing true to show.
//
// The right-hand side is `strat::uiReachable` over the bridge's own `MakeUiWorld`, so
// the two calls see one state and any difference is the bridge's doing. Every unit on
// the board is checked rather than one chosen unit -- the four §2.4 rows have four
// different `move` values and sit on four different terrains, and a bridge that
// mishandled exactly one of them is the interesting failure.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeReachableMatchesModuleTest,
	"Stratocracy.StratBridge.T-UI-02.MatchesModuleQuery",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeReachableMatchesModuleTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeQueryParity;

	FStratBridge Bridge;
	FString Error;
	// On its own line and not inside the TestTrue call: it WRITES Error, and a call that
	// both fills the string and formats it has unspecified argument evaluation order.
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

	for (const strat::UiUnit& U : World.units)
	{
		const FString At = FString::Printf(TEXT("unit %d (side %d) at %s"),
			U.id, U.side, *Describe(U.hex));

		std::vector<strat::ReachEntry> FromBridge;
		const FStratResult Queried = Bridge.Reachable(U.id, FromBridge);
		if (!TestTrue(*FString::Printf(TEXT("%s: Reachable answers"), *At), Queried.bOk))
		{
			AddError(FString::Printf(TEXT("%s: %s"), *At, *Queried.Reason));
			continue;
		}

		// THE EXPECTATION. The module's own answer over the bridge's own world -- not a
		// path this file walked, not a cost it charged.
		const std::vector<strat::ReachEntry> FromModule = strat::uiReachable(World, U.id);

		if (!TestEqual(*FString::Printf(TEXT("%s: the bridge forwards every entry, none added or dropped"), *At),
			static_cast<int32>(FromBridge.size()), static_cast<int32>(FromModule.size())))
		{
			continue;
		}

		for (size_t i = 0; i < FromModule.size(); ++i)
		{
			const strat::ReachEntry& B = FromBridge[i];
			const strat::ReachEntry& M = FromModule[i];
			const FString Slot = FString::Printf(TEXT("%s: entry %d"), *At, static_cast<int32>(i));

			// hexEqual is the module's own comparison, used rather than a field-by-field
			// one written here, for the same reason the costs are not re-derived.
			TestTrue(*FString::Printf(TEXT("%s: hex is the module's (%s vs %s), order included"),
					*Slot, *Describe(B.hex), *Describe(M.hex)),
				strat::hexEqual(B.hex, M.hex));
			TestEqual(*FString::Printf(TEXT("%s: cost is the module's, not re-costed"), *Slot),
				B.cost, M.cost);
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-02 clause 2 -- the set is NOT a hex-distance filter.
//
// WHY THIS IS NOT REDUNDANT WITH THE CLAUSE ABOVE, and it is the reason the clause
// exists at all. `MatchesModuleQuery` asserts bridge == module. It is silent about what
// either of them computes, so it stays green under two failures it cannot tell apart
// from success: a rules module that itself regressed to a distance filter, and -- the
// live one on this project -- a movement-highlight actor that never called `Reachable`
// and filtered by distance at the call site. StratBridge.h says the routed query exists
// precisely so a gate can PREFER it; a gate can only prefer it if it can also say what
// the wrong answer looks like. This clause says that, on the shipped board, with real
// hexes named in the log.
//
// THE FILTER IS A FOIL AND NOT AN EXPECTATION. Nothing here asserts the reachable set
// equals anything this file computed. The assertion is a DIFFERENCE, which is exactly
// the direction in which a test may compute: a wrong self-computed foil makes this
// clause harder to pass, never easier.
//
// IF FERRUM CROSSING EVER STOPS DIVERGING, THIS FAILS. That is deliberate and is the
// only honest behaviour: a board on which distance and cost agree everywhere is a board
// on which T-UI-02 has nothing to gate, and the right response is a scenario or a
// clause change made in the open, not a quietly green run.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeReachableNotHexDistanceTest,
	"Stratocracy.StratBridge.T-UI-02.NotHexDistance",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeReachableNotHexDistanceTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeQueryParity;

	FStratBridge Bridge;
	FString Error;
	const bool bSeeded = SeedBridge(Bridge, Error);
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), bSeeded))
	{
		AddError(Error);
		return false;
	}

	const strat::UiWorld World = Bridge.MakeUiWorld();
	const std::vector<strat::UnitDef>& Defs = Bridge.UnitDefs();
	const strat::MapBounds Bounds = World.board.bounds;

	if (!TestTrue(TEXT("the scenario supplies map bounds"), Bounds.cols > 0 && Bounds.rows > 0))
	{
		return false;
	}

	int32 UnitsThatDiverge = 0;
	int32 TotalDivergentHexes = 0;

	for (const strat::UiUnit& U : World.units)
	{
		if (U.defIndex < 0 || static_cast<size_t>(U.defIndex) >= Defs.size())
		{
			AddError(FString::Printf(TEXT("unit %d carries defIndex %d, outside the loaded table"),
				U.id, U.defIndex));
			return false;
		}
		// The §2.4 row's move allowance, READ from the table the bridge mapped. The naive
		// filter has to be the plausible wrong answer to be worth ruling out, and the
		// plausible wrong answer uses the unit's real move.
		const int32 Move = Defs[static_cast<size_t>(U.defIndex)].move;

		std::vector<strat::ReachEntry> Truth;
		const FStratResult Queried = Bridge.Reachable(U.id, Truth);
		if (!TestTrue(*FString::Printf(TEXT("unit %d: Reachable answers"), U.id), Queried.bOk))
		{
			AddError(Queried.Reason);
			continue;
		}

		// THE FOIL: every in-bounds hex within `move` steps, terrain and occupancy
		// ignored -- which is exactly what a call site that skipped the routed query
		// would produce.
		TArray<strat::Hex> Naive;
		for (int32 Row = 0; Row < Bounds.rows; ++Row)
		{
			for (int32 Col = 0; Col < Bounds.cols; ++Col)
			{
				const strat::Hex H = strat::offsetToAxial(Col, Row);
				if (strat::hexDistance(U.hex, H) <= Move)
				{
					Naive.Add(H);
				}
			}
		}

		// In the naive set and not in the real one: the hexes a distance filter would
		// highlight and the unit cannot actually enter. This is the direction that
		// matters -- it is what a player would click on and be refused.
		TArray<strat::Hex> OverReach;
		for (const strat::Hex& H : Naive)
		{
			bool bInTruth = false;
			for (const strat::ReachEntry& E : Truth)
			{
				if (strat::hexEqual(E.hex, H))
				{
					bInTruth = true;
					break;
				}
			}
			if (!bInTruth)
			{
				OverReach.Add(H);
			}
		}

		if (OverReach.Num() > 0)
		{
			++UnitsThatDiverge;
			TotalDivergentHexes += OverReach.Num();

			// Reported hex by hex WITH THE TERRAIN AND THE OCCUPANT, because "the sets
			// differ" is a verdict and the useful part of this clause is the evidence.
			// Capped so a board change cannot turn one clause into a thousand log lines.
			const int32 Shown = FMath::Min(OverReach.Num(), 6);
			for (int32 i = 0; i < Shown; ++i)
			{
				const strat::Hex& H = OverReach[i];
				const int32 TerrainIndex = World.board.terrainAt(H);
				const int32 Occupant = World.board.occupantAt(H);
				const FString TerrainName =
					(World.terrain != nullptr && TerrainIndex >= 0 &&
					 static_cast<size_t>(TerrainIndex) < World.terrain->size())
						? FromStd((*World.terrain)[static_cast<size_t>(TerrainIndex)].id)
						: FString(TEXT("<none>"));
				const int32 MoveCost =
					(World.terrain != nullptr && TerrainIndex >= 0 &&
					 static_cast<size_t>(TerrainIndex) < World.terrain->size())
						? (*World.terrain)[static_cast<size_t>(TerrainIndex)].moveCost
						: -1;

				AddInfo(FString::Printf(
					TEXT("unit %d (%s, move %d) at %s: distance filter would offer %s -- terrain %s (moveCost %d), occupant %d, hexDistance %d"),
					U.id, *FromStd(Defs[static_cast<size_t>(U.defIndex)].id), Move, *Describe(U.hex),
					*Describe(H), *TerrainName, MoveCost, Occupant,
					strat::hexDistance(U.hex, H)));
			}
			if (OverReach.Num() > Shown)
			{
				AddInfo(FString::Printf(TEXT("unit %d: %d further divergent hexes not listed"),
					U.id, OverReach.Num() - Shown));
			}
		}
	}

	AddInfo(FString::Printf(
		TEXT("T-UI-02: %d of %d units diverge from a hexDistance filter, %d divergent hexes in total"),
		UnitsThatDiverge, static_cast<int32>(World.units.size()), TotalDivergentHexes));

	TestTrue(
		TEXT("T-UI-02: on Ferrum Crossing the reachable set differs from a hexDistance <= move filter -- "
		     "if this fails, the board no longer distinguishes the routed query from the substitution "
		     "the clause exists to forbid, and that is a scenario finding, not a licence to drop the clause"),
		TotalDivergentHexes > 0);

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-02 clause 3 -- an unknown unit id is a refusal, not an empty set.
//
// StratBridge.h: `uiReachable` returns the empty vector for an unknown unit id and for
// missing tables, and those are its ONLY empty results, because `reachable` always
// includes the unit's own hex at cost 0. So an empty vector is never an answer and a
// caller cannot tell which failure it is looking at -- the refusal channel is what
// carries that, and this clause is what makes it load-bearing.
//
// THE OUT-PARAM IS PRE-FILLED WITH A PREVIOUS CALL'S SHAPE ON PURPOSE. The bridge
// clears it BEFORE any check, so a refused call cannot leave a caller reading the last
// unit's highlight as this unit's. A test that passed an already-empty vector would
// assert nothing about that ordering.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeReachableRefusesUnknownUnitTest,
	"Stratocracy.StratBridge.T-UI-02.RefusesUnknownUnit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeReachableRefusesUnknownUnitTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeQueryParity;

	FStratBridge Bridge;
	FString Error;
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

	// A real answer first, so the out-param carries a previous call's set when the
	// refusal arrives -- and so this clause also witnesses that a legitimate result and
	// a refusal are distinguishable rather than merely differently spelled.
	std::vector<strat::ReachEntry> Reach;
	const int32 RealUnitId = World.units.front().id;
	const FStratResult Legitimate = Bridge.Reachable(RealUnitId, Reach);
	if (!TestTrue(TEXT("a real unit id is answered"), Legitimate.bOk))
	{
		AddError(Legitimate.Reason);
		return false;
	}
	TestTrue(TEXT("that legitimate answer is non-empty, so it is distinguishable from a refusal"),
		!Reach.empty());

	const FStratResult Refused = Bridge.Reachable(kNoSuchUnitId, Reach);
	TestFalse(TEXT("T-UI-02: an unknown unit id is refused, not answered with an empty set"),
		Refused.bOk);
	TestTrue(TEXT("T-UI-02: the refusal carries a reason"), !Refused.Reason.IsEmpty());
	TestTrue(TEXT("T-UI-02: a refused query clears the caller's set, so no previous answer is read as this one"),
		Reach.empty());

	// The bridge's own convention, pinned rather than paraphrased: `Reachable` and
	// `Forecast` spell an unknown unit the same way, and the clause is that they agree.
	// It is the CROSS-METHOD equality that is checked, not one literal in isolation --
	// a caller that switched from one query to the other must not have to learn a second
	// vocabulary for the same fault.
	const FString ExpectedReason = FString::Printf(TEXT("no unit with id %d"), kNoSuchUnitId);
	TestEqual(TEXT("T-UI-02: the unknown-unit reason is the bridge's stated one"),
		Refused.Reason, ExpectedReason);

	strat::UiForecast Forecast;
	const FStratResult ForecastRefused =
		Bridge.Forecast(kNoSuchUnitId, World.units.front().hex, Forecast);
	TestEqual(TEXT("Forecast spells an unknown unit exactly as Reachable does"),
		ForecastRefused.Reason, Refused.Reason);

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-02 clause 4 -- a successful call is never empty.
//
// This is the property that makes the refusal channel MEAN anything. If a success could
// legitimately be empty, then "empty" would be ambiguous again and clause 3 would be
// pinning a distinction the type system does not actually carry. Move.h states the
// reason -- the null move: a unit is not blocked by itself, so its own hex is always in
// the set at cost 0 -- and this clause asserts that on every unit the shipped scenario
// deploys, including the two Infantry that begin adjacent to friendly units and the
// Artillery in the corner.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeReachableSuccessIsNeverEmptyTest,
	"Stratocracy.StratBridge.T-UI-02.SuccessIsNeverEmpty",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeReachableSuccessIsNeverEmptyTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeQueryParity;

	FStratBridge Bridge;
	FString Error;
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

	for (const strat::UiUnit& U : World.units)
	{
		const FString At = FString::Printf(TEXT("unit %d (side %d) at %s"),
			U.id, U.side, *Describe(U.hex));

		std::vector<strat::ReachEntry> Reach;
		const FStratResult Queried = Bridge.Reachable(U.id, Reach);
		if (!TestTrue(*FString::Printf(TEXT("%s: Reachable answers"), *At), Queried.bOk))
		{
			AddError(FString::Printf(TEXT("%s: %s"), *At, *Queried.Reason));
			continue;
		}

		TestTrue(*FString::Printf(TEXT("%s: a successful query is never empty"), *At),
			!Reach.empty());

		// The unit's OWN hex at cost 0 -- Move.h's null move, and the specific entry that
		// makes "never empty" a rule rather than an observation about this board.
		bool bFoundSelf = false;
		for (const strat::ReachEntry& E : Reach)
		{
			if (strat::hexEqual(E.hex, U.hex))
			{
				bFoundSelf = true;
				TestEqual(*FString::Printf(TEXT("%s: the unit's own hex costs 0 (Move.h's null move)"), *At),
					E.cost, 0);
				break;
			}
		}
		TestTrue(*FString::Printf(TEXT("%s: the set contains the unit's own hex"), *At), bFoundSelf);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-02 clause 5 -- an unseeded bridge refuses, with a reason that says WHICH
// seeding step is missing.
//
// TWO STATES AND TWO REASONS, and the second is the one worth having. "Nothing loaded
// at all" and "tables loaded, no scenario" are different faults with different fixes,
// and a bridge that spelled them the same way would send a caller looking at the wrong
// half of its startup. The bridge checks definitions first and the scenario second, so
// the clause asserts both reasons AND that they differ from each other.
//
// AND THE SET IS EMPTY IN BOTH, which is the other half of clause 3's distinction:
// the empty vector is what a refusal leaves behind, so it must never also be what a
// success leaves behind. Clause 4 holds the other end of that.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeReachableRefusesUnseededTest,
	"Stratocracy.StratBridge.T-UI-02.RefusesUnseeded",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeReachableRefusesUnseededTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeQueryParity;

	// ---- nothing loaded at all ---------------------------------------------
	// Pre-filled with a plausible previous answer, so "cleared" is distinguishable from
	// "never written".
	std::vector<strat::ReachEntry> Reach;
	Reach.push_back(strat::ReachEntry{ strat::Hex{ 3, 3 }, 2 });

	FStratBridge Empty;
	TestFalse(TEXT("a bridge that never loaded anything is not seeded"), Empty.IsSeeded());
	TestFalse(TEXT("a bridge that never loaded anything has no definitions"), Empty.IsLoaded());

	const FStratResult NoDefs = Empty.Reachable(0, Reach);
	TestFalse(TEXT("T-UI-02: an unloaded bridge refuses rather than answering an empty set"),
		NoDefs.bOk);
	TestTrue(TEXT("T-UI-02: that refusal carries a reason"), !NoDefs.Reason.IsEmpty());
	TestTrue(TEXT("T-UI-02: the refusal cleared the caller's set"), Reach.empty());
	TestEqual(TEXT("T-UI-02: the unloaded reason names the definitions"),
		NoDefs.Reason, FString(TEXT("definitions are not loaded")));

	// ---- tables loaded, no scenario ----------------------------------------
	FStratBridge DefsOnly;
	FString Error;
	const bool bLoaded = LoadDefsOnly(DefsOnly, Error);
	if (!TestTrue(TEXT("the shipped tables map in"), bLoaded))
	{
		AddError(Error);
		return false;
	}
	TestTrue(TEXT("definitions are loaded"), DefsOnly.IsLoaded());
	TestFalse(TEXT("but nothing is seeded"), DefsOnly.IsSeeded());

	Reach.push_back(strat::ReachEntry{ strat::Hex{ 3, 3 }, 2 });
	const FStratResult NoScenario = DefsOnly.Reachable(0, Reach);
	TestFalse(TEXT("T-UI-02: a bridge with tables but no scenario still refuses"), NoScenario.bOk);
	TestTrue(TEXT("T-UI-02: that refusal carries a reason too"), !NoScenario.Reason.IsEmpty());
	TestTrue(TEXT("T-UI-02: it cleared the caller's set as well"), Reach.empty());
	TestEqual(TEXT("T-UI-02: the unseeded reason names the scenario"),
		NoScenario.Reason, FString(TEXT("no scenario is loaded")));

	// THE DISTINCTION IS THE CLAUSE. Two faults, two fixes, two sentences.
	TestNotEqual(TEXT("T-UI-02: 'no definitions' and 'no scenario' are distinct refusals"),
		NoDefs.Reason, NoScenario.Reason);

	// The same two reasons, in the same order, from the other routed query. A caller
	// that swaps `Reachable` for `Forecast` during startup must not get a different
	// account of the same missing step.
	strat::UiForecast Forecast;
	FStratBridge Empty2;
	TestEqual(TEXT("Forecast gives the same unloaded reason"),
		Empty2.Forecast(0, strat::Hex{ 0, 0 }, Forecast).Reason, NoDefs.Reason);

	FStratBridge DefsOnly2;
	if (LoadDefsOnly(DefsOnly2, Error))
	{
		TestEqual(TEXT("Forecast gives the same unseeded reason"),
			DefsOnly2.Forecast(0, strat::Hex{ 0, 0 }, Forecast).Reason, NoScenario.Reason);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-01 clause 1 -- the forecast's fields are the module's fields.
//
// §2.6's "the forecast is exactly what resolves" is structural in the rules module --
// Ui.h states that `uiForecast` is produced by `resolveDamage` and `defenderCanCounter`
// AND BY NOTHING ELSE. That structure survives the trip through the bridge only if the
// bridge forwards it unchanged, and this clause is the measurement of that: SEVEN
// FIELDS, one at a time, against `strat::uiForecast` over the bridge's own world.
//
// Field by field rather than by a memcmp or an operator==, deliberately: a mismatch
// then names the field, and `UiForecast` declares no comparison operator this file
// would be silently agreeing with.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeForecastMatchesModuleTest,
	"Stratocracy.StratBridge.T-UI-01.ForecastMatchesModule",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeForecastMatchesModuleTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeQueryParity;

	strat::Save Fixture;
	FString Error;
	const bool bFixture = LoadFixture(Fixture, Error);
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"), bFixture))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the fixture is long enough to supply an attack"),
		static_cast<int32>(Fixture.commandLog.size()) > kAttackIndex))
	{
		return false;
	}

	const strat::SaveCommand& TheAttack = Fixture.commandLog[static_cast<size_t>(kAttackIndex)];
	if (!TestEqual(
			*FString::Printf(TEXT("fixture entry %d is an Attack, which is what this clause forecasts"),
				kAttackIndex),
			static_cast<int32>(TheAttack.kind), static_cast<int32>(strat::SaveCommandKind::Attack)))
	{
		AddError(TEXT("the fixture was re-emitted and its command order moved; re-pick kAttackIndex "
		              "rather than forecasting whatever now sits at that index"));
		return false;
	}

	FStratBridge Bridge;
	const bool bReplayed = SeedAndReplayPrefix(Bridge, Fixture, kAttackIndex, Error);
	if (!TestTrue(TEXT("the bridge reaches the position the fixture's attack was made from"), bReplayed))
	{
		AddError(Error);
		return false;
	}

	AddInfo(FString::Printf(TEXT("forecasting the fixture's own turn %d side %d attack: unit %d -> %s"),
		TheAttack.turn, TheAttack.side, TheAttack.unitId, *Describe(TheAttack.hex)));

	strat::UiForecast FromBridge;
	const FStratResult Asked = Bridge.Forecast(TheAttack.unitId, TheAttack.hex, FromBridge);
	if (!TestTrue(TEXT("T-UI-01: the forecast can be asked"), Asked.bOk))
	{
		AddError(Asked.Reason);
		return false;
	}

	// THE EXPECTATION: the module's own answer over the bridge's own world. Nothing in
	// this file computed a distance, a damage number or a counter.
	const strat::UiWorld World = Bridge.MakeUiWorld();
	const strat::UiForecast FromModule =
		strat::uiForecast(World, TheAttack.unitId, TheAttack.hex);

	TestEqual(TEXT("T-UI-01: legal"),          FromBridge.legal,         FromModule.legal);
	TestEqual(TEXT("T-UI-01: reason"),         FromStd(FromBridge.reason), FromStd(FromModule.reason));
	TestEqual(TEXT("T-UI-01: distance"),       FromBridge.distance,      FromModule.distance);
	TestEqual(TEXT("T-UI-01: damage"),         FromBridge.damage,        FromModule.damage);
	TestEqual(TEXT("T-UI-01: defenderDies"),   FromBridge.defenderDies,  FromModule.defenderDies);
	TestEqual(TEXT("T-UI-01: counterFires"),   FromBridge.counterFires,  FromModule.counterFires);
	TestEqual(TEXT("T-UI-01: counterDamage"),  FromBridge.counterDamage, FromModule.counterDamage);

	// The fixture chose this attack because it was legal there, so a forecast that came
	// back illegal would mean the replayed position is not the one the fixture was in --
	// a harness failure wearing a rules answer, and worth separating from a field
	// mismatch. It also keeps the comparison above from passing on seven zeroes.
	TestTrue(TEXT("T-UI-01: the fixture's own attack forecasts as legal from the position it was made in"),
		FromBridge.legal);
	TestTrue(TEXT("T-UI-01: a legal forecast carries a damage number, so the field comparison is not seven zeroes"),
		FromBridge.damage > 0);

	AddInfo(FString::Printf(
		TEXT("forecast: legal=%d distance=%d damage=%d defenderDies=%d counterFires=%d counterDamage=%d"),
		FromBridge.legal ? 1 : 0, FromBridge.distance, FromBridge.damage,
		FromBridge.defenderDies ? 1 : 0, FromBridge.counterFires ? 1 : 0, FromBridge.counterDamage));

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-01 clause 2 -- `bOk` answers "could the question be ASKED", never "what did the
// rules SAY".
//
// THE SPLIT IS THE CLAUSE, and StratBridge.h states it: "out of range", "same side",
// "no unit on that hex" and "a unit cannot attack itself" are ANSWERS. A player hovering
// a hex out of range is having an ordinary conversation with the rules, and folding that
// into a refusal would make every second mouse-move look like a fault -- while an
// attacker id that is not on the board is a MALFORMED QUESTION and must not come back
// wearing `legal == false` alongside the ordinary ones.
//
// EVERY PROBE IS READ OFF THE BOARD, not written down here. The friendly target is a
// unit the world says shares the attacker's side; the empty hex is one the board says
// has no occupant; the self hex is the attacker's own. The only literal is the unit id
// that cannot exist.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeForecastRefusalSplitTest,
	"Stratocracy.StratBridge.T-UI-01.ForecastRefusesUnaskableAndAnswersIllegal",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeForecastRefusalSplitTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeQueryParity;

	FStratBridge Bridge;
	FString Error;
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

	const strat::UiUnit& Attacker = World.units.front();

	// ---- UNASKABLE: an attacker that is not on the board --------------------
	{
		strat::UiForecast Out;
		const FStratResult R = Bridge.Forecast(kNoSuchUnitId, Attacker.hex, Out);
		TestFalse(TEXT("T-UI-01: an unknown attacker is a refusal -- the question was malformed"),
			R.bOk);
		TestTrue(TEXT("T-UI-01: that refusal carries a reason"), !R.Reason.IsEmpty());
		TestEqual(TEXT("T-UI-01: and it is the bridge's stated unknown-unit reason"),
			R.Reason, FString::Printf(TEXT("no unit with id %d"), kNoSuchUnitId));
	}

	// ---- ASKABLE, ANSWERED ILLEGAL: the attacker's own hex -------------------
	{
		strat::UiForecast Out;
		const FStratResult R = Bridge.Forecast(Attacker.id, Attacker.hex, Out);
		TestTrue(TEXT("T-UI-01: a unit targeting its own hex is an ANSWER, not a refusal"), R.bOk);
		TestFalse(TEXT("T-UI-01: and the rules answer it illegal"), Out.legal);
		TestTrue(TEXT("T-UI-01: with the module's own reason"), !Out.reason.empty());
		AddInfo(FString::Printf(TEXT("self-target reason: %s"), *FromStd(Out.reason)));
	}

	// ---- ASKABLE, ANSWERED ILLEGAL: a friendly target -----------------------
	{
		const strat::UiUnit* Friendly = nullptr;
		for (const strat::UiUnit& U : World.units)
		{
			if (U.side == Attacker.side && U.id != Attacker.id)
			{
				Friendly = &U;
				break;
			}
		}
		if (TestNotNull(TEXT("the scenario deploys a second unit on the attacker's side"), Friendly))
		{
			strat::UiForecast Out;
			const FStratResult R = Bridge.Forecast(Attacker.id, Friendly->hex, Out);
			TestTrue(TEXT("T-UI-01: a friendly target is an ANSWER, not a refusal"), R.bOk);
			TestFalse(TEXT("T-UI-01: and the rules answer it illegal"), Out.legal);
			TestTrue(TEXT("T-UI-01: with the module's own reason"), !Out.reason.empty());
			AddInfo(FString::Printf(TEXT("friendly-target reason: %s"), *FromStd(Out.reason)));
		}
	}

	// ---- ASKABLE, ANSWERED ILLEGAL: an empty hex ----------------------------
	{
		// Found by asking the board, so a scenario that moved every unit would move this
		// probe with it.
		bool bFound = false;
		strat::Hex EmptyHex;
		for (int32 Row = 0; Row < World.board.bounds.rows && !bFound; ++Row)
		{
			for (int32 Col = 0; Col < World.board.bounds.cols && !bFound; ++Col)
			{
				const strat::Hex H = strat::offsetToAxial(Col, Row);
				if (World.board.occupantAt(H) == strat::OCCUPANT_NONE)
				{
					EmptyHex = H;
					bFound = true;
				}
			}
		}
		if (TestTrue(TEXT("the board has at least one unoccupied hex"), bFound))
		{
			strat::UiForecast Out;
			const FStratResult R = Bridge.Forecast(Attacker.id, EmptyHex, Out);
			TestTrue(TEXT("T-UI-01: an empty target hex is an ANSWER, not a refusal"), R.bOk);
			TestFalse(TEXT("T-UI-01: and the rules answer it illegal"), Out.legal);
			TestTrue(TEXT("T-UI-01: with the module's own reason"), !Out.reason.empty());
			AddInfo(FString::Printf(TEXT("empty-target reason at %s: %s"),
				*Describe(EmptyHex), *FromStd(Out.reason)));
		}
	}

	// ---- ASKABLE, ANSWERED ILLEGAL: an enemy out of range -------------------
	// The probe is SELECTED with hexDistance and the verdict is the module's; the
	// distance is used to pick a target that ought to be out of range, never to decide
	// whether the module was right about it. On Ferrum Crossing's opening the two sides
	// deploy on opposite edges, so every enemy qualifies; the guard is there so that a
	// board on which none did would say so rather than silently skip.
	{
		const std::vector<strat::UnitDef>& Defs = Bridge.UnitDefs();
		const int32 RangeMax = (Attacker.defIndex >= 0 &&
		                        static_cast<size_t>(Attacker.defIndex) < Defs.size())
			? Defs[static_cast<size_t>(Attacker.defIndex)].rangeMax
			: 0;

		const strat::UiUnit* FarEnemy = nullptr;
		int32 FarDistance = 0;
		for (const strat::UiUnit& U : World.units)
		{
			if (U.side == Attacker.side)
			{
				continue;
			}
			const int32 D = strat::hexDistance(Attacker.hex, U.hex);
			if (D > RangeMax && D > FarDistance)
			{
				FarEnemy = &U;
				FarDistance = D;
			}
		}

		if (FarEnemy != nullptr)
		{
			strat::UiForecast Out;
			const FStratResult R = Bridge.Forecast(Attacker.id, FarEnemy->hex, Out);
			TestTrue(TEXT("T-UI-01: an out-of-range enemy is an ANSWER, not a refusal"), R.bOk);
			TestFalse(TEXT("T-UI-01: and the rules answer it illegal"), Out.legal);
			TestTrue(TEXT("T-UI-01: with the module's own reason"), !Out.reason.empty());
			AddInfo(FString::Printf(TEXT("out-of-range reason (distance %d vs rangeMax %d): %s"),
				FarDistance, RangeMax, *FromStd(Out.reason)));
		}
		else
		{
			AddInfo(TEXT("no enemy is out of this attacker's range on the seeded opening; "
			             "the out-of-range sub-case was not exercised"));
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-01 clause 3 -- a refusal leaves the out-param DEFAULT, never stale.
//
// The failure this forbids is specific and quiet: a hover moves from a legal target to
// an unknown attacker, the bridge refuses, and a caller that checked `legal` before
// `bOk` draws the PREVIOUS target's damage number over the new hex. StratBridge.cpp
// resets `OutForecast` before any check for exactly this reason, and an ordering stated
// only in a comment is not a guarantee.
//
// SUCCESS FIRST, ON PURPOSE. A clause that refused into a fresh `UiForecast` would pass
// against a bridge that never wrote the out-param at all, which is the same green for
// the opposite reason.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeForecastResetsOnRefusalTest,
	"Stratocracy.StratBridge.T-UI-01.ForecastResetsOutParamOnRefusal",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeForecastResetsOnRefusalTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeQueryParity;

	strat::Save Fixture;
	FString Error;
	const bool bFixture = LoadFixture(Fixture, Error);
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"), bFixture))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the fixture is long enough to supply an attack"),
		static_cast<int32>(Fixture.commandLog.size()) > kAttackIndex))
	{
		return false;
	}
	const strat::SaveCommand& TheAttack = Fixture.commandLog[static_cast<size_t>(kAttackIndex)];

	FStratBridge Bridge;
	const bool bReplayed = SeedAndReplayPrefix(Bridge, Fixture, kAttackIndex, Error);
	if (!TestTrue(TEXT("the bridge reaches the position the fixture's attack was made from"), bReplayed))
	{
		AddError(Error);
		return false;
	}

	// ---- a real answer, so there is something stale to be left behind -------
	strat::UiForecast Out;
	const FStratResult Legitimate = Bridge.Forecast(TheAttack.unitId, TheAttack.hex, Out);
	if (!TestTrue(TEXT("the fixture's own attack forecasts"), Legitimate.bOk))
	{
		AddError(Legitimate.Reason);
		return false;
	}
	if (!TestTrue(TEXT("that forecast is legal and non-trivial, so 'stale' is detectable"),
			Out.legal && Out.damage > 0))
	{
		return false;
	}

	// ---- then the refusal ---------------------------------------------------
	const FStratResult Refused = Bridge.Forecast(kNoSuchUnitId, TheAttack.hex, Out);
	TestFalse(TEXT("T-UI-01: the unknown attacker is refused"), Refused.bOk);

	// Compared against a freshly default-constructed one rather than against seven
	// written-out zeroes, so a field added to `UiForecast` is covered the day it lands.
	const strat::UiForecast Default;
	TestEqual(TEXT("T-UI-01: legal is reset"),         Out.legal,         Default.legal);
	TestEqual(TEXT("T-UI-01: reason is reset"),        FromStd(Out.reason), FromStd(Default.reason));
	TestEqual(TEXT("T-UI-01: distance is reset"),      Out.distance,      Default.distance);
	TestEqual(TEXT("T-UI-01: damage is reset -- no previous target's number survives"),
		Out.damage, Default.damage);
	TestEqual(TEXT("T-UI-01: defenderDies is reset"),  Out.defenderDies,  Default.defenderDies);
	TestEqual(TEXT("T-UI-01: counterFires is reset"),  Out.counterFires,  Default.counterFires);
	TestEqual(TEXT("T-UI-01: counterDamage is reset"), Out.counterDamage, Default.counterDamage);

	// The other refusal path -- an unseeded bridge -- resets it too. Same guarantee,
	// reached through the earlier of the two checks, so the reset really is BEFORE both.
	strat::UiForecast Stale;
	const FStratResult Refilled = Bridge.Forecast(TheAttack.unitId, TheAttack.hex, Stale);
	if (TestTrue(TEXT("the forecast can be refilled"), Refilled.bOk && Stale.legal))
	{
		FStratBridge Unloaded;
		const FStratResult Unseeded = Unloaded.Forecast(TheAttack.unitId, TheAttack.hex, Stale);
		TestFalse(TEXT("T-UI-01: an unloaded bridge refuses"), Unseeded.bOk);
		TestEqual(TEXT("T-UI-01: and resets the out-param before that check too"),
			Stale.damage, Default.damage);
		TestEqual(TEXT("T-UI-01: including legal"), Stale.legal, Default.legal);
	}

	return true;
}
