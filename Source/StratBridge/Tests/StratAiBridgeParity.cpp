// The editor pass for §2.9's opponent AI where it meets the engine -- the BRIDGE half,
// and only the bridge half. GDD §2.9, §2.7, §4.9 part 2, §4.10.
//
// WHAT THIS FILE IS NOT ABOUT, stated first because it is the fence that decides every
// clause below. §2.9's DECISION QUALITY is certified upstream against the vendored
// source: `cpp_reference/Ai.good.cpp` + `test_ai.cpp`, recorded in the §3 ledger as
// "T-AI-01..06 (6/6) + GATE-AI-SMOKE". Nothing here asserts that the AI picks a good
// target (T-AI-04), builds when it can afford to (T-AI-02), captures with idle
// Infantry (T-AI-03), or declines a losing trade (T-AI-05). Those are proven where the
// code being proven lives, and a second opinion about them formed in an Automation test
// is drift, not cover.
//
// WHAT IT IS ABOUT is the joint this project actually owns and upstream cannot see:
// that `FStratBridge` COMPOSES the AI's input faithfully out of the authoritative
// state, and hands back an output that is SUBMITTABLE UNCHANGED through the same
// validation a player's command goes through. Upstream proved the AI reasons well about
// the board it is given. This file is what makes "the board it is given" the board.
//
// ---------------------------------------------------------------------------
// ACCEPTANCE IDS -- REUSED, NOT MINTED, and each reuse argued.
//
//   T-AI-01  "legality: every AI command passes the same validation as a player
//            command; zero rejected commands across N self-play games"
//            (GDD, measured at Stratocracy_Prototype_GDD.md:1864-1865; `grep -c
//            T-AI-01` = 7 occurrences, so the ID is the GDD's and not this file's).
//
//            THE NATURAL HOME FOR EVERY SUBMITTABILITY AND TRANSLATION CLAUSE. Read
//            the clause literally: it is not about the AI's judgement at all, it is
//            about the command ARRIVING at validation intact and being accepted. On
//            the headless side that is nearly free -- `AiCommand` and the driver's
//            apply path speak one vocabulary. Across this bridge it is the whole risk
//            surface: an `AiCommand` names its Attack victim by UNIT ID and
//            `SubmitAttackAtHex` takes a HEX, `AiCommandKind` and
//            `EStratAiCommandKind` live in two repositories, and the four kinds fan
//            out to four different `Submit*` methods. "Zero rejected commands across
//            a self-play game" is exactly the property that catches all of that, and
//            it is exactly what this file's driving clause runs.
//
//   T-AI-06  "determinism: same state -> same move" (GDD:1876-1877; `grep -c` = 8).
//            Reused for ONE clause, and for a bridge-side reason rather than to
//            re-prove the AI's tiebreaks: `MakeAiState` composes a `std::vector` per
//            call out of the live `GameState`, and a composition whose ORDER varied
//            between two identical states would hand `nextCommand` two different
//            inputs and get two different -- individually defensible -- commands.
//            Upstream's T-AI-06 pins the function; this pins that this bridge feeds
//            it the same argument twice from the same state.
//
//   GATE-BRIDGE-DEFS  the existing unnumbered gate for "a defIndex means what the
//            loader says it means" (`StratBridgeParity.cpp:148-235`,
//            `MappedDefsMatchLoaderOrder`). The buildlist clauses live under it rather
//            than under a T-AI id because that is precisely what they are about: a
//            buildlist is a vector of defIndexes into the SAME table that gate already
//            pins, `SetBuildlistByIds` is the one place in the tree a §4.8 unit id
//            becomes such an index, and the failure mode is phase 0's exactly -- a
//            silently different unit type at the same index. It mints no acceptance
//            ID, on this gate's own established precedent.
//
// A minted `T-AI-07`, or a `T-BRIDGE-AI-01`, would have been an ID appearing nowhere
// in the GDD -- phase 6's `T-PLAY-01` mistake. The three above were each confirmed
// present by `grep -c` against `E:\MultiAgent\stratocracy-content\
// Stratocracy_Prototype_GDD.md` before this file was written.
//
// ---------------------------------------------------------------------------
// WHY EVERY CLAUSE IS HERE AND NOT IN `StratPlay`. `strat::nextCommand` carries no
// `_API` macro (StratBridge.h's opening measurement: 8 x LNK2019), so this module is
// the only one that can call both `FStratBridge` and `strat::`. A parity clause that
// compares an engine value to a `strat::` value has exactly one module it can compile
// in, and this is it.
//
// WHERE EVERY EXPECTATION COMES FROM. No clause below writes down a hex, a unit id, a
// defIndex, a unit name, a factory location or a turn number. Every expectation is
// read back from a module-side value at the moment of comparison:
// `Bridge.State().turn.builtThisTurn`, `Bridge.State().units`, `Bridge.UnitDefs()`,
// `Bridge.BuildlistDefIndexes()`, `Bridge.ReachableHexes()`, `Bridge.AttackTargetHexes()`,
// `Bridge.RecordedCommandCount()`, `Bridge.StateHash()`. That is the rule
// `.agents/ue-project-context.md:189-191` states, and it is why a scenario edit that
// moved every factory would move both sides of every comparison here together.
//
// THE ONE THING THIS FILE COMPUTES is the four-way dispatch from `EStratAiCommandKind`
// to a `Submit*` method, in `SubmitAsIs` below -- and that is deliberately not an
// expectation, it is the CALLER's half of the joint under test. It performs no lookup:
// every field it passes is the field `NextAiCommand` handed back, unexamined and
// unamended. That is the point of the clause, so it has to be written somewhere.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Misc/Paths.h"
#include "Templates/Function.h"
#include "UObject/UObjectGlobals.h"

#include "StratBridge.h"

// The vendored rules module, by short name via StratRules' PublicIncludePaths.
// `StratBridgeParity.cpp` names these for the same reason: a StratBridge test may see
// both worlds, and reading a module-side value is the whole point.
#include "Ai.h"
#include "Hex.h"
#include "Turn.h"

namespace StratAiBridgeParity
{
	// MUST match the value the rest of the engine-side harness seeds with, and it is a
	// harness pin rather than a rule -- `Replay.h` states that no rule in this project
	// decides which side moves first. `StratBridgeParity.cpp:57` and
	// `StratScoreboardParity.cpp:82` pin the same value so that every gate in the tree
	// is looking at the same opening position.
	static const int32 kFirstSide = 0;

	// A ceiling on the self-play driver, NOT an expectation about how long a game runs.
	// Ferrum Crossing caps at turn 20 over two sides with ten starting units, each of
	// which may move and act once per turn, plus builds -- so a completed game is on
	// the order of hundreds of commands and this is comfortably above it. It exists so
	// that a bridge which somehow stopped advancing the turn fails as a bounded, named
	// clause instead of hanging the whole Automation run. Exhausting it is a FAILURE
	// and is asserted as one; it is never a quiet early return.
	static const int32 kMaxSelfPlayCommands = 6000;

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

	static FString HexText(FIntPoint H)
	{
		return FString::Printf(TEXT("(q=%d,r=%d)"), H.X, H.Y);
	}

	static const TCHAR* KindName(EStratAiCommandKind Kind)
	{
		switch (Kind)
		{
		case EStratAiCommandKind::Build:   return TEXT("Build");
		case EStratAiCommandKind::Move:    return TEXT("Move");
		case EStratAiCommandKind::Attack:  return TEXT("Attack");
		case EStratAiCommandKind::EndTurn: return TEXT("EndTurn");
		}
		return TEXT("<unnamed>");
	}

	/**
	 * Brings a bridge to the seeded opening: the shipped tables mapped in, the shipped
	 * scenario seeded through the rules module's own `seedFromScenario`.
	 *
	 * Returns false and fills OutError rather than asserting, on StratBridgeParity's
	 * precedent -- a missing asset becomes a named failure inside the caller's own
	 * clause list instead of a crash in a helper.
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
	 * Every unit id in the loaded table, in loader order, as the `FName`s
	 * `SetBuildlistByIds` takes.
	 *
	 * READ OFF `UnitDefs()` AND NEVER TYPED OUT. Writing `{"Infantry", "Tank"}` here
	 * would put §4.8 data in a test file, and the buildlist clauses below assert that
	 * resolution AGREES WITH that table -- an expectation copied out of the same CSV the
	 * loader read would make both sides of that comparison this file's own opinion.
	 * Using every row also means the resolution clause covers the whole table rather
	 * than whichever two rows happened to get typed.
	 */
	static TArray<FName> AllUnitIds(const FStratBridge& Bridge)
	{
		TArray<FName> Ids;
		Ids.Reserve(static_cast<int32>(Bridge.UnitDefs().size()));
		for (const strat::UnitDef& D : Bridge.UnitDefs())
		{
			Ids.Add(FName(*FromStd(D.id)));
		}
		return Ids;
	}

	/** Seeds, then configures §2.9's buildlist from the loaded table's own ids. */
	static bool SeedBridgeWithBuildlist(FStratBridge& Bridge, FString& OutError)
	{
		if (!SeedBridge(Bridge, OutError))
		{
			return false;
		}
		const FStratResult Set = Bridge.SetBuildlistByIds(AllUnitIds(Bridge));
		if (!Set.bOk)
		{
			OutError = FString::Printf(TEXT("SetBuildlistByIds refused: %s"), *Set.Reason);
			return false;
		}
		return true;
	}

	/**
	 * THE CALLER'S HALF OF THE JOINT: one AI command, submitted with NO FURTHER LOOKUP.
	 *
	 * Every field passed below is the field `NextAiCommand` handed back. Nothing here
	 * consults `State()`, resolves a `TargetId`, clamps a hex or substitutes a default.
	 * That is the whole content of §2.9's "validated like any player command": if the
	 * bridge's output needed one more lookup before it could be submitted, this function
	 * could not be written, and the clause would be un-runnable rather than green.
	 *
	 * `Attack` PASSES `Hex` AND NOT `TargetId`, which is `FStratAiCommand`'s documented
	 * contract ("`TargetId` is kept beside it for logging, and is NOT the thing to
	 * submit", StratBridge.h:135). Submitting `TargetId` would not compile against
	 * `SubmitAttackAtHex`, which is a small mercy; submitting a hex the caller looked up
	 * itself WOULD compile, and is exactly the thing this function refuses to do.
	 */
	static FStratResult SubmitAsIs(FStratBridge& Bridge, const FStratAiCommand& C)
	{
		switch (C.Kind)
		{
		case EStratAiCommandKind::Build:   return Bridge.SubmitBuildAtHex(C.Hex, C.DefIndex);
		case EStratAiCommandKind::Move:    return Bridge.SubmitMoveToHex(C.UnitId, C.Hex);
		case EStratAiCommandKind::Attack:  return Bridge.SubmitAttackAtHex(C.UnitId, C.Hex);
		case EStratAiCommandKind::EndTurn: return Bridge.SubmitEndTurn();
		}
		// Unreachable while `EStratAiCommandKind` has four enumerators, and stated as a
		// refusal rather than as a fallthrough so that a fifth one added by a re-vendor
		// fails a clause instead of silently submitting the last arm.
		return FStratResult::Fail(TEXT("this test has no submission arm for that command kind"));
	}

	struct FSelfPlayStats
	{
		int32 Commands = 0;
		int32 Builds   = 0;
		int32 Moves    = 0;
		int32 Attacks  = 0;
		int32 EndTurns = 0;

		/** The match reached a terminal §2.8 tier under its own steam. */
		bool bReachedTerminal = false;
		/** The observer asked to stop early; the run is not a completed game. */
		bool bObserverStopped = false;
		/** A `NextAiCommand` or a submission failed. The clause has already reported it. */
		bool bFaulted = false;
	};

	/**
	 * Drives a full §2.9 self-play game: ask the AI for the side to move, submit the
	 * answer unchanged, repeat until the match reaches a terminal tier.
	 *
	 * THIS IS T-AI-01's OWN SENTENCE, RUN. "Zero rejected commands across N self-play
	 * games" is asserted here and nowhere else, once per command, with the refusal's own
	 * words on failure. The count assertion beside it is the phase-4 finding-1 shape:
	 * without it a non-recording apply path leaves the game green and `RecordedLog()`
	 * empty, and §4.10 -- the save format, the replay gate, phase 6's PIE comparison --
	 * all quietly stop meaning anything.
	 *
	 * WHY THE COUNT MUST RISE BY EXACTLY ONE, not merely rise. `RecordedCommandCount` is
	 * `Recorded.size()`, and a submission path that appended twice would produce a log
	 * that replays a command twice -- a state divergence with a green hash at the moment
	 * of writing and a wrong one at the moment of loading.
	 *
	 * `Observe` RUNS BEFORE SUBMISSION and returns whether to keep going. Every clause
	 * below that needs a particular board position gets it by watching this loop rather
	 * than by constructing one: the only positions this tree can reach deterministically
	 * are the seeded opening, the parity fixture's, and whatever §2.9 itself plays into
	 * from the opening -- and the third is the only one that contains AI commands.
	 */
	static FSelfPlayStats RunSelfPlay(
		FAutomationTestBase& Test,
		FStratBridge&        Bridge,
		TFunctionRef<bool(FStratBridge&, const FStratAiCommand&)> Observe)
	{
		FSelfPlayStats Stats;

		while (Stats.Commands < kMaxSelfPlayCommands)
		{
			if (Bridge.State().turn.result.tier != strat::ResultTier::InProgress)
			{
				Stats.bReachedTerminal = true;
				break;
			}

			const int32 Side = Bridge.SideToMove();

			FStratAiCommand C;
			const FStratResult Asked = Bridge.NextAiCommand(Side, C);
			if (!Asked.bOk)
			{
				Test.AddError(FString::Printf(
					TEXT("command %d: NextAiCommand(side %d) refused on a seeded, loaded bridge: %s"),
					Stats.Commands, Side, *Asked.Reason));
				Stats.bFaulted = true;
				break;
			}

			if (!Observe(Bridge, C))
			{
				Stats.bObserverStopped = true;
				break;
			}

			// The joint. Nothing between the answer and the submission.
			const int32 CountBefore = Bridge.RecordedCommandCount();
			const FStratResult Applied = SubmitAsIs(Bridge, C);

			if (!Applied.bOk)
			{
				// T-AI-01's "zero rejected commands" failing, reported with everything a
				// reader needs to find it: which command, which turn, which side, what the
				// AI asked for, and the refusal in the refusing module's own words.
				Test.AddError(FString::Printf(
					TEXT("T-AI-01: command %d (turn %d, side %d) %s unit=%d target=%d hex=%s def=%d ")
					TEXT("was REJECTED by the same validation a player command goes through: %s%s"),
					Stats.Commands, Bridge.Turn(), Side, KindName(C.Kind),
					C.UnitId, C.TargetId, *HexText(C.Hex), C.DefIndex,
					Applied.Id.IsEmpty() ? TEXT("") : *FString::Printf(TEXT("[%s] "), *Applied.Id),
					*Applied.Reason));
				Stats.bFaulted = true;
				break;
			}

			Test.TestEqual(
				*FString::Printf(
					TEXT("T-AI-01: command %d (%s) raised RecordedCommandCount by exactly one"),
					Stats.Commands, KindName(C.Kind)),
				Bridge.RecordedCommandCount(), CountBefore + 1);

			switch (C.Kind)
			{
			case EStratAiCommandKind::Build:   ++Stats.Builds;   break;
			case EStratAiCommandKind::Move:    ++Stats.Moves;    break;
			case EStratAiCommandKind::Attack:  ++Stats.Attacks;  break;
			case EStratAiCommandKind::EndTurn: ++Stats.EndTurns; break;
			}
			++Stats.Commands;
		}

		return Stats;
	}

	/** The no-op observer, for clauses that only want the game driven. */
	static bool ObserveNothing(FStratBridge&, const FStratAiCommand&)
	{
		return true;
	}

	/** Whether `H` appears in a module-side `std::vector<strat::Hex>`. */
	static bool ContainsHex(const std::vector<strat::Hex>& Hexes, FIntPoint H)
	{
		for (const strat::Hex& X : Hexes)
		{
			if (X.q == H.X && X.r == H.Y)
			{
				return true;
			}
		}
		return false;
	}

	/** The live unit with this id, or nullptr. Read straight off the authoritative state. */
	static const strat::GameUnit* FindUnit(const FStratBridge& Bridge, int32 UnitId)
	{
		for (const strat::GameUnit& U : Bridge.State().units)
		{
			if (U.id == UnitId)
			{
				return &U;
			}
		}
		return nullptr;
	}

	static FString StatsText(const FSelfPlayStats& S)
	{
		return FString::Printf(
			TEXT("%d commands: Build %d, Move %d, Attack %d, EndTurn %d; terminal=%s, observer-stopped=%s"),
			S.Commands, S.Builds, S.Moves, S.Attacks, S.EndTurns,
			S.bReachedTerminal ? TEXT("yes") : TEXT("no"),
			S.bObserverStopped ? TEXT("yes") : TEXT("no"));
	}
}

// ---------------------------------------------------------------------------
// T-AI-01 -- THE JOINT CLAUSE, and the one this phase does not close without.
//
// "Every AI command passes the same validation as a player command; zero rejected
// commands across N self-play games." N is one here, and it is a COMPLETE one: the
// clause requires the match to reach a terminal §2.8 tier under its own steam, so a
// bridge that stalled after three commands fails rather than reporting three clean
// submissions.
//
// WHY THE COUNT ASSERTION IS NOT PADDING, and this is the phase-4 finding-1 shape one
// layer out. Every other clause in this file could be green while `SubmitBuildAtHex`
// (the newest of the five paths, added this phase) applied through a NON-RECORDING
// route: the state would advance, the AI would keep answering, the game would finish,
// and `RecordedLog()` would be short by every build. §4.10's save would then replay to a
// different state than the one it claims, and the first thing to notice would be a
// player loading a save. `RecordedCommandCount()` is the module-side witness -- it is
// `Recorded.size()` and not a counter kept beside it -- and it is checked per command so
// the failure names the command that was not recorded.
//
// EVERY KIND MUST ACTUALLY OCCUR, asserted at the end. A self-play game in which no
// Build was ever issued would exercise three of the four `Submit*` arms and report
// "zero rejections" with a straight face; that is a silence, and a silence means nothing
// until the instrument has spoken. Build is the arm most at risk, because
// `SubmitBuildAtHex` is the method this phase added.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAiCommandsSubmitUnchangedTest,
	"Stratocracy.StratBridge.T-AI-01.CommandsSubmitUnchangedAndRecord",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAiCommandsSubmitUnchangedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiBridgeParity;

	FStratBridge Bridge;
	FString Error;
	// Runs on its own line and not inside the TestTrue call: it WRITES Error, and a call
	// that both fills the string and formats it has unspecified argument evaluation
	// order. StratBridgeParity.cpp:251-253 records the same care.
	const bool bReady = SeedBridgeWithBuildlist(Bridge, Error);
	if (!TestTrue(TEXT("the bridge seeds and takes a buildlist"), bReady))
	{
		AddError(Error);
		return false;
	}

	const FSelfPlayStats Stats = RunSelfPlay(*this, Bridge, &ObserveNothing);
	AddInfo(FString::Printf(TEXT("self-play: %s"), *StatsText(Stats)));

	TestFalse(TEXT("T-AI-01: no command in the self-play game was rejected"), Stats.bFaulted);
	TestTrue(
		FString::Printf(
			TEXT("T-AI-01: the self-play game reached a terminal result rather than exhausting the %d-command ceiling"),
			kMaxSelfPlayCommands),
		Stats.bReachedTerminal);

	// The game is only a witness for the arms it exercised.
	TestTrue(TEXT("the game exercised the Move submission arm"),    Stats.Moves    > 0);
	TestTrue(TEXT("the game exercised the Attack submission arm"),  Stats.Attacks  > 0);
	TestTrue(TEXT("the game exercised the EndTurn submission arm"), Stats.EndTurns > 0);
	TestTrue(TEXT("the game exercised the Build submission arm (SubmitBuildAtHex, new this phase)"),
		Stats.Builds > 0);

	// The whole game, in the log. Every accepted command raised the count by one, so the
	// total is the count -- stated once at the end as well, because the per-command
	// clause would still pass if the log were TRUNCATED between commands rather than
	// never appended to.
	TestEqual(TEXT("T-AI-01/§4.10: the recorded log holds every command the AI submitted"),
		Bridge.RecordedCommandCount(), Stats.Commands);
	TestEqual(TEXT("RecordedCommandCount() agrees with RecordedLog()'s own size"),
		Bridge.RecordedCommandCount(), static_cast<int32>(Bridge.RecordedLog().size()));

	return true;
}

// ---------------------------------------------------------------------------
// T-AI-01 -- the board the AI reasoned over IS the board the rules module scores.
//
// `MakeAiState` is private, so no clause can read its nine members. What CAN be read is
// what the AI does with them, checked against the rules module's own answers about the
// same state through a DIFFERENT composition -- `MakeUiWorld` for `ReachableHexes` and
// `Forecast` for `AttackTargetHexes`. Two compositions of one state, two rules
// functions, one comparison:
//
//   Move    -- the destination must be in `ReachableHexes(UnitId)`, which is
//              `strat::uiReachable` and is terrain-weighted and occupancy-blocked
//              (Move.h). This is the clause that observes `bounds`, `terrain` and
//              `terrainDefs`: an `AiState` composed with an empty terrain vector or
//              default bounds would produce destinations off the real reach set, and a
//              destination off the real reach set is not a matter of taste.
//   Attack  -- the target hex must be in `AttackTargetHexes(UnitId)`, which is assembled
//              from `strat::uiForecast` answers. This observes `units` and `unitDefs`:
//              an AI that could not see a unit, or saw it with the wrong stat block,
//              would name a hex the forecast declines.
//   Build   -- the defIndex must be in `BuildlistDefIndexes()`. This observes
//              `buildlist`, and it is the reason an empty one would not merely make the
//              AI passive but would make it un-observable.
//
// IT IS NOT A SECOND OPINION ABOUT §2.9's JUDGEMENT. Nothing here says the AI picked
// the BEST destination or the BEST target -- T-AI-04's subject, certified upstream.
// It says the choice lies inside the set the rules module says is available from this
// state, which is a statement about the INPUT and is unavailable to upstream, because
// upstream has no `FStratBridge` to compose one wrongly.
//
// NEITHER SIDE OF ANY COMPARISON IS COMPUTED HERE. `ReachableHexes` and
// `AttackTargetHexes` are module answers routed through the bridge; a hex-distance
// filter standing in for either is the exact substitution T-UI-02 measured at 122
// divergent hexes across 10 of 10 units.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAiBoardAgreesWithRulesQueriesTest,
	"Stratocracy.StratBridge.T-AI-01.AiBoardAgreesWithRulesQueries",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAiBoardAgreesWithRulesQueriesTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiBridgeParity;

	FStratBridge Bridge;
	FString Error;
	const bool bReady = SeedBridgeWithBuildlist(Bridge, Error);
	if (!TestTrue(TEXT("the bridge seeds and takes a buildlist"), bReady))
	{
		AddError(Error);
		return false;
	}

	int32 MovesChecked   = 0;
	int32 AttacksChecked = 0;
	int32 BuildsChecked  = 0;

	const FSelfPlayStats Stats = RunSelfPlay(*this, Bridge,
		[this, &MovesChecked, &AttacksChecked, &BuildsChecked]
		(FStratBridge& B, const FStratAiCommand& C) -> bool
		{
			if (C.Kind == EStratAiCommandKind::Move)
			{
				TArray<FIntPoint> Hexes;
				TArray<int32>     Costs;
				const FStratResult Reach = B.ReachableHexes(C.UnitId, Hexes, Costs);
				if (!TestTrue(
						*FString::Printf(TEXT("turn %d: ReachableHexes answers for the unit the AI moved (%d): %s"),
							B.Turn(), C.UnitId, *Reach.Reason),
						Reach.bOk))
				{
					return true;
				}
				TestTrue(
					*FString::Printf(
						TEXT("turn %d: the AI's Move destination %s for unit %d lies in the rules module's own reach set (%d hexes)"),
						B.Turn(), *HexText(C.Hex), C.UnitId, Hexes.Num()),
					Hexes.Contains(C.Hex));
				++MovesChecked;
			}
			else if (C.Kind == EStratAiCommandKind::Attack)
			{
				TArray<FIntPoint> Targets;
				const FStratResult Legal = B.AttackTargetHexes(C.UnitId, Targets);
				if (!TestTrue(
						*FString::Printf(TEXT("turn %d: AttackTargetHexes answers for the AI's attacker (%d): %s"),
							B.Turn(), C.UnitId, *Legal.Reason),
						Legal.bOk))
				{
					return true;
				}
				TestTrue(
					*FString::Printf(
						TEXT("turn %d: the AI's Attack hex %s for unit %d lies in the rules module's own legal-target set (%d hexes)"),
						B.Turn(), *HexText(C.Hex), C.UnitId, Targets.Num()),
					Targets.Contains(C.Hex));
				++AttacksChecked;
			}
			else if (C.Kind == EStratAiCommandKind::Build)
			{
				// `buildlist` reached the AI: the type it chose is one of the types it was
				// configured with. The expectation is `BuildlistDefIndexes()` -- the value
				// actually handed to `AiState::buildlist` -- and not a re-resolution of the
				// unit ids, which would be a second derivation free to agree with itself.
				const TArray<int32> Configured = B.BuildlistDefIndexes();
				TestTrue(
					*FString::Printf(
						TEXT("turn %d: the AI's Build defIndex %d is one of the %d defIndexes the buildlist carries"),
						B.Turn(), C.DefIndex, Configured.Num()),
					Configured.Contains(C.DefIndex));
				++BuildsChecked;
			}
			return true;
		});

	AddInfo(FString::Printf(TEXT("self-play: %s; checked %d Moves, %d Attacks, %d Builds"),
		*StatsText(Stats), MovesChecked, AttacksChecked, BuildsChecked));

	TestFalse(TEXT("the self-play game ran clean"), Stats.bFaulted);

	// A silence proves nothing until the instrument has spoken. If no Move was ever
	// issued, the terrain/bounds observation above never ran and this clause would be
	// reporting a green it did not earn.
	TestTrue(TEXT("at least one Move was checked against the rules module's reach set"),
		MovesChecked > 0);
	TestTrue(TEXT("at least one Attack was checked against the rules module's target set"),
		AttacksChecked > 0);
	TestTrue(TEXT("at least one Build was checked against the configured buildlist"),
		BuildsChecked > 0);

	return true;
}

// ---------------------------------------------------------------------------
// T-AI-01 -- `builtThisTurn` reaches the AI. THE MANDATORY CLAUSE, and the sharpest one.
//
// WHY THIS MEMBER AND NOT ANOTHER. §2.7's "one build per factory per turn" is turn-scoped
// and the turn belongs to row 5, so the record moved out of the driver and onto
// `TurnState` (Turn.h:97-100). `AiState` restates it as its own member (Ai.h:55-59), so
// `MakeAiState` must assign BOTH -- and StratBridge.cpp:777-785 says so in as many words.
// If it assigned only `turn`, and `Ai.good.cpp` reads the standalone member, then every
// factory reads as unspent at every turn: the AI queues a second build at a factory it
// already used, `markBuilt` refuses it downstream, and the opponent stalls on its own
// refused command -- with a green build, a running match, and this entire suite green.
// That is the failure this clause exists to make loud.
//
// HOW IT IS OBSERVED, given that `MakeAiState` is private and must stay so. Through
// `NextAiCommand`, against `Bridge.State().turn.builtThisTurn` -- the module-side record,
// never a count this file kept. Three statements, in order:
//
//   (a) THE INSTRUMENT SPEAKS. The AI proposes Build at some factory hex H at a moment
//       when H is NOT in `builtThisTurn`. Without this, everything below is an absence
//       with no control, and phase-6's cost is on record for exactly that.
//   (b) THE BUILD REGISTERS. After the submission, H IS in `builtThisTurn` -- read off
//       the state, so the record and the clause cannot drift.
//   (c) THE INSTRUMENT THEN DECLINES. Asked again in the same turn, the AI's next
//       command is not a Build at H, nor at any other hex already in `builtThisTurn`.
//
// (c) alone would be satisfied by an AI that never builds. (a) is what forecloses that,
// and it is checked on the same factory hex within the same turn, so the only thing that
// changed between the two askings is the record itself.
//
// EVERY BUILD IN THE WHOLE GAME IS ALSO CHECKED against (a)'s form -- no proposal ever
// names a hex already in the record -- so the clause is not resting on one lucky moment.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAiBuiltThisTurnReachesAiTest,
	"Stratocracy.StratBridge.T-AI-01.BuiltThisTurnReachesTheAi",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAiBuiltThisTurnReachesAiTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiBridgeParity;

	FStratBridge Bridge;
	FString Error;
	const bool bReady = SeedBridgeWithBuildlist(Bridge, Error);
	if (!TestTrue(TEXT("the bridge seeds and takes a buildlist"), bReady))
	{
		AddError(Error);
		return false;
	}

	int32 FreshProposals    = 0;   // (a) -- Build proposed at a hex not yet in the record
	int32 RegisteredBuilds  = 0;   // (b) -- the hex appeared in the record afterwards
	int32 ReAsks            = 0;   // (c) -- the AI asked again, same turn, after a build

	const FSelfPlayStats Stats = RunSelfPlay(*this, Bridge,
		[this, &FreshProposals, &RegisteredBuilds, &ReAsks]
		(FStratBridge& B, const FStratAiCommand& C) -> bool
		{
			// (a) -- for EVERY Build the AI proposes, all game long. The expectation is
			// the module's own vector, read at the instant of the proposal.
			if (C.Kind == EStratAiCommandKind::Build)
			{
				const bool bAlreadySpent = ContainsHex(B.State().turn.builtThisTurn, C.Hex);
				TestFalse(
					*FString::Printf(
						TEXT("§2.7/T-AI-01: turn %d side %d -- the AI proposed a Build at %s, a factory already in TurnState::builtThisTurn (%d entries)"),
						B.Turn(), B.SideToMove(), *HexText(C.Hex),
						static_cast<int32>(B.State().turn.builtThisTurn.size())),
					bAlreadySpent);
				if (!bAlreadySpent)
				{
					++FreshProposals;
				}
			}
			return true;
		});

	AddInfo(FString::Printf(TEXT("self-play: %s; %d fresh Build proposals"),
		*StatsText(Stats), FreshProposals));
	TestFalse(TEXT("the self-play game ran clean"), Stats.bFaulted);

	// (a), as a control rather than as a hope. If the AI never proposed a build at an
	// unspent factory, the clause above never had anything to decline and its green is
	// vacuous. Failing here says so instead of hiding it.
	TestTrue(
		TEXT("CONTROL: the AI proposed at least one Build at a factory not yet in builtThisTurn"),
		FreshProposals > 0);

	// ---- (b) and (c), on a second game, stopped at the first Build ----------
	// A fresh bridge rather than the played-out one: the game above ran to a terminal
	// tier and no further command is legal there. Deterministic, so this reaches the same
	// first Build the game above did.
	FStratBridge Second;
	if (!TestTrue(TEXT("a second bridge seeds and takes a buildlist"),
			SeedBridgeWithBuildlist(Second, Error)))
	{
		AddError(Error);
		return false;
	}

	FIntPoint BuiltAt      = FIntPoint(0, 0);
	bool      bFoundBuild  = false;
	const FSelfPlayStats Prefix = RunSelfPlay(*this, Second,
		[&BuiltAt, &bFoundBuild](FStratBridge&, const FStratAiCommand& C) -> bool
		{
			if (C.Kind == EStratAiCommandKind::Build)
			{
				BuiltAt = C.Hex;
				bFoundBuild = true;
				return false;   // stop BEFORE submitting; the clause submits it itself
			}
			return true;
		});
	AddInfo(FString::Printf(TEXT("prefix run: %s"), *StatsText(Prefix)));

	if (!TestTrue(TEXT("the deterministic prefix reaches a Build command"), bFoundBuild))
	{
		return false;
	}

	const int32 TurnAtBuild = Second.Turn();
	const int32 SideAtBuild = Second.SideToMove();

	TestFalse(
		*FString::Printf(TEXT("before the build, %s is not in builtThisTurn"), *HexText(BuiltAt)),
		ContainsHex(Second.State().turn.builtThisTurn, BuiltAt));

	// The AI's own command, submitted unchanged -- ask it again for its answer rather
	// than replaying the captured struct, so this is the live path and not a recording.
	FStratAiCommand Build;
	const FStratResult AskedBuild = Second.NextAiCommand(SideAtBuild, Build);
	if (!TestTrue(TEXT("the AI still answers Build at the same position"), AskedBuild.bOk))
	{
		AddError(AskedBuild.Reason);
		return false;
	}
	TestEqual(TEXT("T-AI-06: the same state yields the same command kind"),
		static_cast<int32>(Build.Kind), static_cast<int32>(EStratAiCommandKind::Build));
	// Hexes are compared through TestTrue rather than the generic TestEqual template
	// throughout this file: that template's failure path calls ReportError with the
	// values, which has no formatting for FIntPoint, and HexText names q and r anyway --
	// which is the thing a reader of a failed hex comparison actually needs.
	TestTrue(
		*FString::Printf(TEXT("T-AI-06: the same state yields the same factory hex -- %s, was %s"),
			*HexText(Build.Hex), *HexText(BuiltAt)),
		Build.Hex == BuiltAt);

	const FStratResult BuiltOk = SubmitAsIs(Second, Build);
	if (!TestTrue(TEXT("T-AI-01: the AI's Build submits unchanged"), BuiltOk.bOk))
	{
		AddError(BuiltOk.Reason);
		return false;
	}

	// (b) -- the module-side record, read back.
	TestTrue(
		*FString::Printf(TEXT("§2.7: after the build, %s IS in TurnState::builtThisTurn"),
			*HexText(BuiltAt)),
		ContainsHex(Second.State().turn.builtThisTurn, BuiltAt));

	// (c) -- the same instrument, the same turn, the same side, asked again.
	TestEqual(TEXT("the build did not end the turn"), Second.Turn(), TurnAtBuild);
	TestEqual(TEXT("the build did not change the side to move"), Second.SideToMove(), SideAtBuild);

	FStratAiCommand After;
	const FStratResult AskedAgain = Second.NextAiCommand(SideAtBuild, After);
	if (!TestTrue(TEXT("the AI answers again after its own build"), AskedAgain.bOk))
	{
		AddError(AskedAgain.Reason);
		return false;
	}
	AddInfo(FString::Printf(TEXT("after the build the AI answered %s at %s"),
		KindName(After.Kind), *HexText(After.Hex)));

	if (After.Kind == EStratAiCommandKind::Build)
	{
		TestTrue(
			*FString::Printf(
				TEXT("§2.7: the AI re-proposed a Build at %s, the factory it just spent -- builtThisTurn did not reach AiState"),
				*HexText(BuiltAt)),
			After.Hex != BuiltAt);
		TestFalse(
			TEXT("§2.7: the AI's next Build names a factory not already in builtThisTurn"),
			ContainsHex(Second.State().turn.builtThisTurn, After.Hex));
	}
	++ReAsks;
	TestTrue(TEXT("the instrument was asked again after the build"), ReAsks > 0);

	return true;
}

// ---------------------------------------------------------------------------
// T-AI-01 -- `targetId` -> hex translation, with the phase-4 transposition foil.
//
// `AiCommand` names an Attack's victim by UNIT ID and leaves `hex` default-constructed;
// `SubmitAttackAtHex` takes a HEX. `NextAiCommand` resolves the one into the other
// against the authoritative board, and that resolution is the single place in this
// phase's surface where the bridge TRANSFORMS the AI's answer rather than forwarding it.
// So it is the single place a silent error can live.
//
// WHY A DEFAULTED HEX IS NOT A SAFE FAILURE HERE, and it is the reason this clause is
// mandatory rather than nice: `FIntPoint(0,0)` is a REAL HEX on an 11x9 board.
// StratBridge.h:556-557 records the same thing. A translation that quietly did nothing
// would submit an attack at a plausible board position -- refused most of the time,
// accepted sometimes, and never obviously wrong.
//
// THE q/r TRANSPOSITION FOIL, taken from the state's own hex and not invented.
// `FIntPoint::X` is q and `FIntPoint::Y` is r by convention across this whole façade, and
// a convention held only by convention is one a future edit can flip in one place. The
// foil asserts `Out.Hex != FIntPoint(r, q)`, and it is GUARDED ON `q != r` FIRST --
// otherwise the foil is its own trivially-satisfied negation and would pass on a
// transposed implementation. A double transposition cannot cancel it because the
// expectation side is read from `Bridge.State().units` in the state's own field names.
//
// EVERY ATTACK IN THE GAME IS CHECKED, and the clause requires at least one of them to
// have been foil-eligible. A game whose every attack landed on the q==r diagonal would
// leave the foil unexercised, and that is reported rather than passed over.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAiAttackTargetResolvesToHexTest,
	"Stratocracy.StratBridge.T-AI-01.AttackTargetIdResolvesToItsHex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAiAttackTargetResolvesToHexTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiBridgeParity;

	FStratBridge Bridge;
	FString Error;
	const bool bReady = SeedBridgeWithBuildlist(Bridge, Error);
	if (!TestTrue(TEXT("the bridge seeds and takes a buildlist"), bReady))
	{
		AddError(Error);
		return false;
	}

	int32 AttacksChecked = 0;
	int32 FoilExercised  = 0;

	const FSelfPlayStats Stats = RunSelfPlay(*this, Bridge,
		[this, &AttacksChecked, &FoilExercised]
		(FStratBridge& B, const FStratAiCommand& C) -> bool
		{
			if (C.Kind != EStratAiCommandKind::Attack)
			{
				return true;
			}
			++AttacksChecked;

			// The TargetId must name a unit on the authoritative board. If it did not,
			// `NextAiCommand` is contracted to refuse rather than answer -- so reaching
			// here with an unresolvable id is itself the failure.
			const strat::GameUnit* Target = FindUnit(B, C.TargetId);
			if (!TestNotNull(
					*FString::Printf(TEXT("turn %d: the AI's TargetId %d names a unit on the board"),
						B.Turn(), C.TargetId),
					Target))
			{
				return true;
			}

			// THE COMPARISON. Expectation read off the state's own fields, in the state's
			// own spelling.
			const FIntPoint Expected(Target->hex.q, Target->hex.r);
			TestTrue(
				*FString::Printf(
					TEXT("turn %d: Out.Hex %s is the hex of unit %d (%s), the unit Out.TargetId names"),
					B.Turn(), *HexText(C.Hex), C.TargetId, *HexText(Expected)),
				C.Hex == Expected);

			// The target is an ENEMY unit -- a translation that resolved the attacker's
			// own id would also satisfy the equality above if the attacker happened to be
			// where the test looked. Sides are read off the state, never assumed.
			const strat::GameUnit* Attacker = FindUnit(B, C.UnitId);
			if (Attacker != nullptr)
			{
				TestNotEqual(
					*FString::Printf(TEXT("turn %d: the AI's target %d is not on the attacker's own side"),
						B.Turn(), C.TargetId),
					Target->side, Attacker->side);
				TestTrue(
					*FString::Printf(TEXT("turn %d: the resolved hex %s is not the attacker's own hex"),
						B.Turn(), *HexText(C.Hex)),
					C.Hex != FIntPoint(Attacker->hex.q, Attacker->hex.r));
			}

			// THE FOIL, guarded on q != r so it cannot be its own trivial negation.
			if (Target->hex.q != Target->hex.r)
			{
				TestTrue(
					*FString::Printf(
						TEXT("turn %d: Out.Hex is (q,r)=%s and not the transposition (r,q)=(%d,%d)"),
						B.Turn(), *HexText(Expected), Target->hex.r, Target->hex.q),
					C.Hex != FIntPoint(Target->hex.r, Target->hex.q));
				++FoilExercised;
			}

			return true;
		});

	AddInfo(FString::Printf(TEXT("self-play: %s; %d attacks checked, %d foil-eligible"),
		*StatsText(Stats), AttacksChecked, FoilExercised));

	TestFalse(TEXT("the self-play game ran clean"), Stats.bFaulted);
	TestTrue(TEXT("CONTROL: the game produced at least one Attack to check"), AttacksChecked > 0);
	TestTrue(
		TEXT("CONTROL: at least one target sat off the q==r diagonal, so the transposition foil was exercised"),
		FoilExercised > 0);

	return true;
}

// ---------------------------------------------------------------------------
// T-AI-01 -- a refusal is distinguishable from an answer, and from the other refusals.
//
// THE DEBT THIS PINS IS THE ENGINEER'S OWN, recorded at StratBridge.h:138-141: a
// default-constructed `FStratAiCommand` reads as a genuine `EndTurn`, so `FStratResult::bOk`
// is THE ONLY thing separating "the bridge refused to answer" from "the AI ended its
// turn". That is a real hazard rather than a stylistic one -- a caller driving an AI turn
// loop against an unseeded bridge sees a clean, instant, empty turn and no fault of any
// kind, which is precisely the silence this refusal exists to break. So the clause
// asserts both halves: the sentinel value IS EndTurn (documenting the hazard rather than
// pretending it away), and every refusal path sets `bOk` false.
//
// THREE REASONS, THREE DIFFERENT STRINGS, compared against each other rather than
// against literals. A refusal that said "no" three times would satisfy "carries a
// reason" and would be useless to whoever has to fix it; pinning the WORDING would fail
// on a copy edit and say nothing. Pairwise inequality is the property that has content
// and survives rewording.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAiRefusalsAreDistinguishableTest,
	"Stratocracy.StratBridge.T-AI-01.RefusalsAreDistinguishableFromEndTurn",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAiRefusalsAreDistinguishableTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiBridgeParity;

	// The hazard, stated as a clause. If this ever stops being true the header's warning
	// is stale and the three refusals below stop being the only signal.
	const FStratAiCommand Defaulted;
	TestEqual(
		TEXT("a default-constructed FStratAiCommand READS AS a genuine EndTurn -- bOk is the only discriminator"),
		static_cast<int32>(Defaulted.Kind), static_cast<int32>(EStratAiCommandKind::EndTurn));
	TestEqual(TEXT("the sentinel's UnitId is the module's own default"), Defaulted.UnitId, -1);
	TestEqual(TEXT("the sentinel's TargetId is the module's own default"), Defaulted.TargetId, -1);
	TestEqual(TEXT("the sentinel's DefIndex is the module's own default"), Defaulted.DefIndex, -1);

	// ---- refusal 1: nothing loaded -----------------------------------------
	FStratBridge Unloaded;
	FStratAiCommand OutA;
	OutA.Kind   = EStratAiCommandKind::Attack;   // A value no refusal would produce, so
	OutA.UnitId = 4242;                          // "reset" is distinguishable from "left".
	const FStratResult Unloaded1 = Unloaded.NextAiCommand(0, OutA);
	TestFalse(TEXT("an unloaded bridge refuses to answer for the AI"), Unloaded1.bOk);
	TestFalse(TEXT("that refusal names a reason"), Unloaded1.Reason.IsEmpty());
	TestEqual(TEXT("a refusal resets the caller's command rather than leaving a stale one"),
		static_cast<int32>(OutA.Kind), static_cast<int32>(EStratAiCommandKind::EndTurn));
	TestEqual(TEXT("a refusal resets the caller's UnitId too"), OutA.UnitId, -1);

	// ---- refusal 2: loaded, never seeded ------------------------------------
	UDataTable* Units = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
	UDataTable* Terrain = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
	if (!TestNotNull(TEXT("DT_Units loads"), Units) ||
		!TestNotNull(TEXT("DT_Terrain loads"), Terrain))
	{
		return false;
	}

	FStratBridge Unseeded;
	if (!TestTrue(TEXT("LoadDefinitions accepts the shipped tables"),
			Unseeded.LoadDefinitions(Units, Terrain).bOk))
	{
		return false;
	}
	TestFalse(TEXT("the bridge is loaded but not seeded"), Unseeded.IsSeeded());

	FStratAiCommand OutB;
	const FStratResult Unseeded2 = Unseeded.NextAiCommand(0, OutB);
	TestFalse(TEXT("a seeded-less bridge refuses to answer for the AI"), Unseeded2.bOk);
	TestFalse(TEXT("that refusal names a reason"), Unseeded2.Reason.IsEmpty());

	// ---- refusal 3: seeded, but a side that does not exist -------------------
	FStratBridge Seeded;
	FString Error;
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), SeedBridge(Seeded, Error)))
	{
		AddError(Error);
		return false;
	}

	// The out-of-range side is `strat::SIDE_COUNT` -- the module's own count, so a
	// scenario or rules change that added a third side moves this clause with it rather
	// than turning a real side into a "does not exist" assertion.
	FStratAiCommand OutC;
	const FStratResult BadSide = Seeded.NextAiCommand(strat::SIDE_COUNT, OutC);
	TestFalse(TEXT("a side outside the match is refused rather than answered with EndTurn"),
		BadSide.bOk);
	TestFalse(TEXT("that refusal names a reason"), BadSide.Reason.IsEmpty());

	// And the control that makes all three mean something: on the very same seeded
	// bridge, a REAL side gets a real answer. Without it, a NextAiCommand that refused
	// unconditionally would pass every clause above.
	FStratAiCommand Real;
	const FStratResult Good = Seeded.NextAiCommand(Seeded.SideToMove(), Real);
	TestTrue(TEXT("CONTROL: the same bridge answers for a side that does exist"), Good.bOk);
	TestTrue(TEXT("CONTROL: the successful answer carries no refusal reason"), Good.Reason.IsEmpty());

	// ---- the three reasons differ from one another ---------------------------
	AddInfo(FString::Printf(TEXT("unloaded: '%s'"),  *Unloaded1.Reason));
	AddInfo(FString::Printf(TEXT("unseeded: '%s'"),  *Unseeded2.Reason));
	AddInfo(FString::Printf(TEXT("bad side: '%s'"),  *BadSide.Reason));

	TestNotEqual(TEXT("'not loaded' and 'not seeded' are different refusals"),
		Unloaded1.Reason, Unseeded2.Reason);
	TestNotEqual(TEXT("'not loaded' and 'unknown side' are different refusals"),
		Unloaded1.Reason, BadSide.Reason);
	TestNotEqual(TEXT("'not seeded' and 'unknown side' are different refusals"),
		Unseeded2.Reason, BadSide.Reason);

	// The unknown-side refusal names the side it was asked about. A caller that passed a
	// loop variable needs to know WHICH iteration was wrong.
	TestTrue(
		*FString::Printf(TEXT("the unknown-side refusal ('%s') names the side asked for"), *BadSide.Reason),
		BadSide.Reason.Contains(FString::FromInt(strat::SIDE_COUNT)));

	return true;
}

// ---------------------------------------------------------------------------
// GATE-BRIDGE-DEFS -- the buildlist resolves in loader order, duplicates preserved.
//
// WHY THIS GATE AND NOT A T-AI ID. `AiState::buildlist` is a vector of defIndexes, and a
// defIndex is the same raw, bounds-checked-only index a §4.10 Build command carries in
// the field the format spells `unitId` (`Replay.good.cpp:486-487`). GATE-BRIDGE-DEFS
// already exists for exactly one sentence -- "a defIndex means what the loader says it
// means" -- and `MappedDefsMatchLoaderOrder` pins the table. This is the second reader of
// that same table, and its failure mode is phase 0's: a buildlist that resolved
// "Infantry" to the wrong index would configure the AI to build a different unit type in
// silence, in a §2.9 build MIX that has no other witness in the tree.
//
// IN `MappedDefsMatchLoaderOrder`'s SHAPE. That clause compares the mapped vector
// against `strat::loadUnits` over the same vendored CSV, position for position. This
// compares `BuildlistDefIndexes()` against `UnitDefs()`, position for position: entry i
// of the returned list must index a def whose `id` is byte-for-byte the id at position i
// of the list that was set. Nothing is written down -- the ids come from `UnitDefs()`
// and the expectation is that same table read back.
//
// DUPLICATES ARE THE POINT, NOT AN EDGE CASE. §2.9 says "mostly Infantry, an occasional
// Tank" and gives no ratio, so `Ai.h:49-53` makes the list caller-supplied DATA and
// repetition is how a mix is expressed in it. A `SetBuildlistByIds` that deduplicated
// would flatten every mix to 1:1 -- data changed by a tidiness decision, silently. So the
// list set below deliberately repeats the first id, and the clause asserts the repetition
// survives IN POSITION.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAiBuildlistResolvesInLoaderOrderTest,
	"Stratocracy.StratBridge.GATE-BRIDGE-DEFS.BuildlistResolvesInLoaderOrder",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAiBuildlistResolvesInLoaderOrderTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiBridgeParity;

	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	// An unset buildlist is empty and that is an ordinary configuration, not a fault --
	// `chooseBuild` returns -1 with nothing to choose. Pinned so that "empty" is known to
	// be the starting point of every clause below.
	TestEqual(TEXT("a freshly seeded bridge carries no buildlist"),
		Bridge.BuildlistDefIndexes().Num(), 0);

	// Every id in the table, in loader order, with the FIRST ONE REPEATED at the end so
	// the duplicate clause has something to be about. Both the ids and the expectation
	// come from `UnitDefs()`.
	TArray<FName> Wanted = AllUnitIds(Bridge);
	if (!TestTrue(TEXT("the loaded unit table has rows to build a list from"), Wanted.Num() > 0))
	{
		return false;
	}
	const FName Repeated = Wanted[0];
	Wanted.Add(Repeated);

	const FStratResult Set = Bridge.SetBuildlistByIds(Wanted);
	if (!TestTrue(TEXT("SetBuildlistByIds accepts every id the loaded table carries"), Set.bOk))
	{
		AddError(Set.Reason);
		return false;
	}

	const TArray<int32> Resolved = Bridge.BuildlistDefIndexes();
	if (!TestEqual(TEXT("the resolved list is as long as the list that was set -- nothing dropped, nothing deduplicated"),
			Resolved.Num(), Wanted.Num()))
	{
		return false;
	}

	const std::vector<strat::UnitDef>& Defs = Bridge.UnitDefs();
	for (int32 Index = 0; Index < Resolved.Num(); ++Index)
	{
		const int32 Def = Resolved[Index];
		if (!TestTrue(
				*FString::Printf(TEXT("entry %d resolves to a defIndex inside the loaded table (%d of %d)"),
					Index, Def, static_cast<int32>(Defs.size())),
				Def >= 0 && Def < static_cast<int32>(Defs.size())))
		{
			continue;
		}

		// THE COMPARISON. Left side: the id the loader put at that defIndex. Right side:
		// the id that was asked for at that position. Neither is written in this file.
		TestEqual(
			*FString::Printf(TEXT("buildlist entry %d resolves to the def whose id was asked for"), Index),
			FromStd(Defs[static_cast<size_t>(Def)].id), Wanted[Index].ToString());
	}

	// POSITIONAL, and stated separately because the id comparison above would also pass
	// on a list that had been sorted into a different order carrying the same multiset.
	// The first entry and the appended duplicate name the same id, so they must resolve
	// to the same defIndex -- and the last entry must be that defIndex and not merely
	// contain it somewhere.
	TestEqual(TEXT("the repeated id at the end resolves to the same defIndex as the first entry"),
		Resolved.Last(), Resolved[0]);

	int32 Occurrences = 0;
	for (const int32 Def : Resolved)
	{
		if (Def == Resolved[0])
		{
			++Occurrences;
		}
	}
	TestEqual(
		*FString::Printf(TEXT("§2.9's build MIX: '%s' appears twice in the resolved list, as it did in the list set"),
			*Repeated.ToString()),
		Occurrences, 2);

	// An empty list is an ORDINARY configuration and not a refusal -- it configures an AI
	// that never builds. Distinguishable from the unresolvable-id case precisely because
	// that one refuses (next clause).
	TestTrue(TEXT("an empty buildlist is accepted rather than refused"),
		Bridge.SetBuildlistByIds(TArray<FName>()).bOk);
	TestEqual(TEXT("and it leaves an empty list"), Bridge.BuildlistDefIndexes().Num(), 0);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-BRIDGE-DEFS -- a refused buildlist changes nothing, and case is not a match.
//
// ALL-OR-NOTHING. `SetBuildlistByIds` builds aside and assigns only after every entry
// resolved, which is `loadUnits`' own "out is left untouched on any defect" discipline. A
// half-resolved list would silently change §2.9's build MIX -- data with no other witness
// in the tree -- so the clause asserts the PREVIOUS list is still byte-for-byte there
// after a refusal, not merely that the call returned false.
//
// CASE IS NOT A MATCH, AND THAT IS A DECISION WITH A COST. `FName::operator==` compares
// case-insensitively, so `{"infantry"}` would resolve under it while `Data/units.csv`
// says `Infantry` -- a widening of the §4.8 id space decided in this bridge rather than
// in the data. The engineer used exact UTF-8 byte comparison deliberately
// (StratBridge.cpp:917-920); this pins that the case-insensitive path is not in play,
// which no other clause in the tree can see, because both spellings produce an equal
// `FName` and the difference only appears at the byte comparison.
//
// THE LOWERCASED ID IS DERIVED FROM THE TABLE'S OWN ID, not typed. A literal "infantry"
// would go stale the day a row is renamed and would then be asserting a refusal for the
// wrong reason -- a green clause about a unit that no longer exists. The clause is
// skipped, loudly, if the table's first id is already lowercase, since then the two
// spellings are the same bytes and there is nothing to distinguish.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAiBuildlistRefusalsLeaveListIntactTest,
	"Stratocracy.StratBridge.GATE-BRIDGE-DEFS.BuildlistRefusalsLeavePriorListIntact",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAiBuildlistRefusalsLeaveListIntactTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiBridgeParity;

	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	const TArray<FName> Good = AllUnitIds(Bridge);
	if (!TestTrue(TEXT("the loaded unit table has rows"), Good.Num() > 0))
	{
		return false;
	}
	if (!TestTrue(TEXT("a good buildlist is accepted"), Bridge.SetBuildlistByIds(Good).bOk))
	{
		return false;
	}
	const TArray<int32> Established = Bridge.BuildlistDefIndexes();
	TestEqual(TEXT("the established list has one entry per id set"), Established.Num(), Good.Num());

	// ---- an unresolvable id, in the MIDDLE of an otherwise good list ---------
	// In the middle rather than at the end, because an implementation that appended as it
	// went would leave a prefix behind and a trailing bad entry is the one arrangement
	// where "prefix applied" and "nothing applied" can look alike at the call site.
	TArray<FName> Poisoned = Good;
	const FName Nonsense(TEXT("NoSuchUnitIdInDT_Units"));
	const int32 BadPosition = Poisoned.Num() / 2;
	Poisoned.Insert(Nonsense, BadPosition);

	const FStratResult Refused = Bridge.SetBuildlistByIds(Poisoned);
	TestFalse(TEXT("§4.8: an unresolvable unit id is refused rather than substituted"), Refused.bOk);
	TestFalse(TEXT("that refusal names a reason"), Refused.Reason.IsEmpty());
	AddInfo(FString::Printf(TEXT("unresolvable-id refusal: '%s'"), *Refused.Reason));

	// The reason names BOTH the id and the position. Duplicates are legal, so the id
	// alone does not say which entry failed -- and a caller with "Infantry" three times
	// and one typo needs the index to find it.
	TestTrue(
		*FString::Printf(TEXT("the refusal ('%s') names the offending id"), *Refused.Reason),
		Refused.Reason.Contains(Nonsense.ToString()));
	TestTrue(
		*FString::Printf(TEXT("the refusal ('%s') names the offending position %d"),
			*Refused.Reason, BadPosition),
		Refused.Reason.Contains(FString::FromInt(BadPosition)));

	// ALL-OR-NOTHING: the previously established list is untouched, entry for entry.
	const TArray<int32> AfterRefusal = Bridge.BuildlistDefIndexes();
	if (TestEqual(TEXT("a refused SetBuildlistByIds leaves the previous list's length untouched"),
			AfterRefusal.Num(), Established.Num()))
	{
		for (int32 Index = 0; Index < Established.Num(); ++Index)
		{
			TestEqual(
				*FString::Printf(TEXT("entry %d of the previous list survived the refusal"), Index),
				AfterRefusal[Index], Established[Index]);
		}
	}

	// ---- case is not a match ------------------------------------------------
	const FString FirstId = Good[0].ToString();
	const FString Lowered = FirstId.ToLower();

	// `ESearchCase::CaseSensitive`, AND IT IS NOT A FLOURISH -- IT IS THE BUG THIS
	// CLAUSE ALMOST SHIPPED WITH. `FString::operator==` is case-INSENSITIVE in Unreal,
	// exactly as `FName::operator==` is, so the first draft's `if (Lowered != FirstId)`
	// read false for "infantry" vs "Infantry" and the whole case clause skipped itself,
	// announcing that skip through a warning that would have been easy to read past.
	// MEASURED: the first suite run reported this test as "succeeded with warnings" with
	// the message "the first unit id ('Infantry') is already lowercase" -- an obviously
	// false statement that only the warning made visible. The guard for a clause about
	// byte-exact comparison must itself compare bytes.
	if (!FirstId.Equals(Lowered, ESearchCase::CaseSensitive))
	{
		TArray<FName> WrongCase;
		WrongCase.Add(FName(*Lowered));
		const FStratResult CaseRefused = Bridge.SetBuildlistByIds(WrongCase);
		TestFalse(
			*FString::Printf(
				TEXT("§4.8: '%s' does not resolve where the table says '%s' -- the match is on bytes, not on FName"),
				*Lowered, *FirstId),
			CaseRefused.bOk);
		TestFalse(TEXT("the case refusal names a reason"), CaseRefused.Reason.IsEmpty());

		// And the list is still the established one.
		TestEqual(TEXT("a refused case-mismatched list also leaves the previous list in place"),
			Bridge.BuildlistDefIndexes().Num(), Established.Num());
	}
	else
	{
		// Reported as an ERROR and not a warning. This branch means the §4.8 table's
		// first id carries no uppercase byte, so there are no two spellings to
		// distinguish and the case clause is UNOBSERVABLE -- and an unobservable clause
		// that reports itself as a pass is how a half-pinned property closes a phase.
		AddError(FString::Printf(
			TEXT("the first unit id ('%s') is already lowercase, so the byte-exactness clause had no two spellings to distinguish and could not run"),
			*FirstId));
	}

	// ---- what clears the list, and what does not ----------------------------
	// StratBridge.h:609-612 and :760-762: the buildlist is CONFIGURATION, not match
	// state. A reseed does not move the table the stored indexes point into; a table
	// reload does, and after one they do not go stale, they name different unit types.
	// Both halves are asserted, because a `LoadScenarioFromFile` that also cleared would
	// silently disarm every AI mid-milestone with no error anywhere.
	if (!TestTrue(TEXT("the list is re-established for the clearing clauses"),
			Bridge.SetBuildlistByIds(Good).bOk))
	{
		return false;
	}

	const FString ScenarioPath = FPaths::Combine(DataDir(), TEXT("ferrum_crossing.json"));
	if (TestTrue(TEXT("the scenario reseeds"),
			Bridge.LoadScenarioFromFile(ScenarioPath, kFirstSide).bOk))
	{
		TestEqual(
			TEXT("LoadScenarioFromFile does NOT clear the buildlist -- it is configuration, not match state"),
			Bridge.BuildlistDefIndexes().Num(), Good.Num());
	}

	UDataTable* Units = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
	UDataTable* Terrain = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
	if (TestNotNull(TEXT("DT_Units loads"), Units) && TestNotNull(TEXT("DT_Terrain loads"), Terrain))
	{
		if (TestTrue(TEXT("the definitions reload"), Bridge.LoadDefinitions(Units, Terrain).bOk))
		{
			TestEqual(
				TEXT("LoadDefinitions DOES clear the buildlist -- the indexes point into a table that just moved"),
				Bridge.BuildlistDefIndexes().Num(), 0);
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-AI-06 -- same state, same move, across two independently composed bridges.
//
// NOT A SECOND OPINION ABOUT §2.9's TIEBREAKS, which are `Ai.good.cpp`'s and are
// certified upstream by `test_ai.cpp`. The subject here is `MakeAiState`: it builds
// fresh `std::vector`s out of the live `GameState` on every call, and a composition
// whose ORDER varied between two identical states -- units gathered from a container
// iterated differently, a buildlist assembled per call -- would hand `nextCommand` two
// different arguments and get back two different commands, each perfectly defensible
// under Q9's stated tiebreaks. Upstream cannot see that failure, because upstream has no
// bridge composing the argument.
//
// TWO BRIDGES AND NOT ONE ASKED TWICE. Asking one bridge twice would share every
// container, every allocation and every iteration order; two separately seeded objects
// share only the scenario file and the tables, which is the level the property is
// actually claimed at.
//
// THE WHOLE GAME AND NOT THE FIRST MOVE. The opening position is one state; a divergence
// in composition is far more likely to appear once units have been destroyed and built
// -- which is where `GameState::units` stops being the scenario's own order.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAiSameStateSameMoveTest,
	"Stratocracy.StratBridge.T-AI-06.SameStateYieldsSameCommandSequence",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAiSameStateSameMoveTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiBridgeParity;

	FStratBridge Left;
	FStratBridge Right;
	FString Error;
	if (!TestTrue(TEXT("the first bridge seeds and takes a buildlist"),
			SeedBridgeWithBuildlist(Left, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the second bridge seeds and takes a buildlist"),
			SeedBridgeWithBuildlist(Right, Error)))
	{
		AddError(Error);
		return false;
	}

	TestEqual(TEXT("the two seeded bridges start from the same canonical state"),
		Left.StateHash(), Right.StateHash());

	int32 Step = 0;
	while (Step < kMaxSelfPlayCommands &&
		   Left.State().turn.result.tier == strat::ResultTier::InProgress)
	{
		const int32 Side = Left.SideToMove();
		TestEqual(*FString::Printf(TEXT("step %d: both bridges agree on the side to move"), Step),
			Right.SideToMove(), Side);

		FStratAiCommand A;
		FStratAiCommand B;
		const FStratResult AskedA = Left.NextAiCommand(Side, A);
		const FStratResult AskedB = Right.NextAiCommand(Side, B);
		if (!TestTrue(*FString::Printf(TEXT("step %d: both bridges answer"), Step),
				AskedA.bOk && AskedB.bOk))
		{
			AddError(FString::Printf(TEXT("left: %s / right: %s"), *AskedA.Reason, *AskedB.Reason));
			return false;
		}

		// THE COMPARISON, field for field. Neither side is this file's arithmetic; both
		// are the same method over two states already asserted equal by hash.
		const FString At = FString::Printf(TEXT("step %d (turn %d, side %d)"), Step, Left.Turn(), Side);
		TestEqual(*FString::Printf(TEXT("%s: same command kind"), *At),
			static_cast<int32>(A.Kind), static_cast<int32>(B.Kind));
		TestEqual(*FString::Printf(TEXT("%s: same acting unit"), *At),   A.UnitId,   B.UnitId);
		TestEqual(*FString::Printf(TEXT("%s: same target unit"), *At),   A.TargetId, B.TargetId);
		TestTrue(*FString::Printf(TEXT("%s: same hex -- %s vs %s"), *At, *HexText(A.Hex), *HexText(B.Hex)),
			A.Hex == B.Hex);
		TestEqual(*FString::Printf(TEXT("%s: same defIndex"), *At),      A.DefIndex, B.DefIndex);

		const FStratResult AppliedA = SubmitAsIs(Left, A);
		const FStratResult AppliedB = SubmitAsIs(Right, B);
		if (!TestTrue(*FString::Printf(TEXT("%s: both submissions are accepted"), *At),
				AppliedA.bOk && AppliedB.bOk))
		{
			AddError(FString::Printf(TEXT("left: %s / right: %s"), *AppliedA.Reason, *AppliedB.Reason));
			return false;
		}

		// The states stay equal, which is what makes the NEXT step's comparison a
		// same-state comparison rather than a coincidence.
		TestEqual(*FString::Printf(TEXT("%s: the two states remain canonically equal"), *At),
			Left.StateHash(), Right.StateHash());

		++Step;
	}

	AddInfo(FString::Printf(TEXT("T-AI-06: %d identical commands across two independently seeded bridges"), Step));
	TestTrue(TEXT("the paired game reached a terminal result rather than exhausting the ceiling"),
		Left.State().turn.result.tier != strat::ResultTier::InProgress);
	TestTrue(TEXT("CONTROL: the comparison had commands to compare"), Step > 0);
	TestEqual(TEXT("both bridges recorded the same number of commands"),
		Left.RecordedCommandCount(), Right.RecordedCommandCount());

	return true;
}

// ---------------------------------------------------------------------------
// T-AI-01 -- `SubmitBuildAtHex` is `SubmitBuild`, and is on the recording path.
//
// WHY THIS IS A CLAUSE. `SubmitBuildAtHex` is the method this phase added, and it closed
// the one hole in the engine-typed façade: Build was the single §4.9 command a module
// that cannot spell `strat::` could not issue. §2.9's AI emits Build as one of its four
// kinds, so the hole was about to become load-bearing. A new method beside an old one is
// exactly where a second, NON-RECORDING apply path gets introduced -- StratBridge.h:528-530
// says so in advance, and the phase-4 finding it points at is the reason to check rather
// than to trust the comment.
//
// TWO IDENTICALLY-SEEDED BRIDGES DRIVEN TO THE SAME POSITION BY THE SAME AI, then the
// same build submitted through the two spellings. The expectation on each side is the
// other side's module-side value -- `StateHash()` and `RecordedCommandCount()` -- and
// this file authors neither.
//
// THE POSITION IS THE AI's, NOT THIS FILE's. A hardcoded factory hex would be §2.13 data
// living in a test, and would need re-picking every time the scenario moved. Driving both
// bridges with the same deterministic self-play prefix, stopping at the first Build,
// reaches a legal build position that the scenario itself supplies.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAiSubmitBuildAtHexMatchesSubmitBuildTest,
	"Stratocracy.StratBridge.T-AI-01.SubmitBuildAtHexMatchesSubmitBuild",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAiSubmitBuildAtHexMatchesSubmitBuildTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiBridgeParity;

	FStratBridge ViaHex;
	FStratBridge ViaTyped;
	FString Error;
	if (!TestTrue(TEXT("the FIntPoint-side bridge seeds and takes a buildlist"),
			SeedBridgeWithBuildlist(ViaHex, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the strat::Hex-side bridge seeds and takes a buildlist"),
			SeedBridgeWithBuildlist(ViaTyped, Error)))
	{
		AddError(Error);
		return false;
	}

	// The same deterministic prefix on both, stopped at the first Build without
	// submitting it.
	FIntPoint HexA = FIntPoint(0, 0);
	FIntPoint HexB = FIntPoint(0, 0);
	int32     DefA = INDEX_NONE;
	int32     DefB = INDEX_NONE;
	bool      bFoundA = false;
	bool      bFoundB = false;

	// Written out twice rather than produced by a lambda factory. A factory taking
	// `FIntPoint&` and returning a lambda that captured that reference would be
	// capturing the factory's own parameter, whose lifetime ends when the factory
	// returns -- it happens to work on MSVC and it is undefined, which is the worst
	// combination a test file can carry.
	const auto ObserveA = [&HexA, &DefA, &bFoundA](FStratBridge&, const FStratAiCommand& C) -> bool
	{
		if (C.Kind == EStratAiCommandKind::Build)
		{
			HexA = C.Hex;
			DefA = C.DefIndex;
			bFoundA = true;
			return false;   // stop BEFORE submitting; the clause submits it itself
		}
		return true;
	};
	const auto ObserveB = [&HexB, &DefB, &bFoundB](FStratBridge&, const FStratAiCommand& C) -> bool
	{
		if (C.Kind == EStratAiCommandKind::Build)
		{
			HexB = C.Hex;
			DefB = C.DefIndex;
			bFoundB = true;
			return false;
		}
		return true;
	};

	const FSelfPlayStats StatsA = RunSelfPlay(*this, ViaHex,   ObserveA);
	const FSelfPlayStats StatsB = RunSelfPlay(*this, ViaTyped, ObserveB);

	AddInfo(FString::Printf(TEXT("prefix A: %s"), *StatsText(StatsA)));
	AddInfo(FString::Printf(TEXT("prefix B: %s"), *StatsText(StatsB)));

	if (!TestTrue(TEXT("the deterministic prefix reaches a Build on the FIntPoint-side bridge"), bFoundA) ||
		!TestTrue(TEXT("the deterministic prefix reaches a Build on the strat::Hex-side bridge"), bFoundB))
	{
		return false;
	}

	// The two prefixes are the same prefix; asserted rather than assumed, because if they
	// were not, the comparison below would be about two different board positions and its
	// verdict would mean nothing.
	TestTrue(
		*FString::Printf(TEXT("both prefixes reached the same factory hex -- %s vs %s"),
			*HexText(HexA), *HexText(HexB)),
		HexA == HexB);
	TestEqual(TEXT("both prefixes reached the same defIndex"), DefA, DefB);
	TestEqual(TEXT("both bridges stand on the same canonical state before the build"),
		ViaHex.StateHash(), ViaTyped.StateHash());
	TestEqual(TEXT("both bridges have recorded the same number of commands before the build"),
		ViaHex.RecordedCommandCount(), ViaTyped.RecordedCommandCount());

	const int32 CountBefore = ViaHex.RecordedCommandCount();

	// The same build, spelled two ways.
	const FStratResult ByHex   = ViaHex.SubmitBuildAtHex(HexA, DefA);
	const FStratResult ByTyped = ViaTyped.SubmitBuild(strat::Hex{HexB.X, HexB.Y}, DefB);

	if (!TestTrue(TEXT("SubmitBuildAtHex accepts the AI's build"), ByHex.bOk))
	{
		AddError(ByHex.Reason);
	}
	if (!TestTrue(TEXT("SubmitBuild accepts the same build"), ByTyped.bOk))
	{
		AddError(ByTyped.Reason);
	}
	if (!ByHex.bOk || !ByTyped.bOk)
	{
		return false;
	}

	TestEqual(TEXT("the two spellings reach the same canonical state"),
		ViaHex.StateHash(), ViaTyped.StateHash());
	TestEqual(TEXT("§4.10: SubmitBuildAtHex records, exactly as SubmitBuild does"),
		ViaHex.RecordedCommandCount(), ViaTyped.RecordedCommandCount());
	TestEqual(TEXT("§4.10: and it recorded exactly one command"),
		ViaHex.RecordedCommandCount(), CountBefore + 1);

	return true;
}
