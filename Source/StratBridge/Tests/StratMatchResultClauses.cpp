// GDD §2.8 -- who won, and why. T-TURN-02, T-TURN-04 and T-TURN-05.
//
// WHAT WAS DARK BEFORE THIS FILE, and it is the gap item 1 of the five-carried-items plan
// exists to close. `strat::UiMatchView` -- the snapshot's match block -- carries `turn`,
// `turnCap`, `sideToMove`, `hasResult` and `resultTier`, and DROPS three of the four fields
// of `strat::MatchResult`. So every consumer downstream of the projection could say
// *Decisive* and could not say FOR WHOM. Upstream `96d93ea` added `strat::uiMatchResult` as
// a fourth `ui*` QUERY (beside `uiReachable`, `uiForecast` and `uiBuildOptions`, and
// deliberately NOT as a snapshot field, which would have moved T-UI-05's field
// enumeration), and `FStratBridge::MatchResult` routes it. These three clauses are what
// make that routing an assertion rather than a claim.
//
// THE ONE THING THAT HAD TO BE ARRANGED, AND WHY THE WHOLE ITEM TURNS ON IT.
// `sideToMove` is NOT a stand-in for the winner. It EQUALS the winner on a flag kill --
// the killer was the side to move -- and DISAGREES at the turn cap, where the match
// resolves at a round boundary and the side to move is whoever closed the round. A test
// suite in which every who-won clause happened to arrange `winner == sideToMove` would
// pass identically against a `sideToMove` derivation, and the gap would stay open under a
// green suite. `CappedMatchNamesTheKeyThatDiffered` below is the clause that arranges
// `winner != sideToMove` on purpose: side 0 is `firstSide`, so side 1 is the side that
// closes the capped round and holds `activeSide` when the tiebreak resolves, while the
// arrangement makes SIDE 0 lead the comparison. Both values are then READ from the module
// -- the winner from `uiMatchResult`, the side to move from `buildUiSnapshot` -- on the
// SAME world in the SAME frame, and asserted unequal.
//
// TWO LEVELS, AND THE SPLIT IS DELIBERATE RATHER THAN A CONVENIENCE.
//
//   - `MutualPassivityIsADrawWithNoWinner` runs the WHOLE ENGINE CHAIN on a bridge that
//     genuinely concluded: a variant of the shipped scenario whose `turnCap` is 1, two
//     `FStratBridge::SubmitEndTurn` calls, and then `FStratBridge::MatchResult` -- the
//     production method -- on the state those commands produced. Nothing is arranged
//     inside the rules module and no `TurnState` is touched by this file on that path.
//
//   - `FlagKillNamesTheKillerAsWinner` and `CappedMatchNamesTheKeyThatDiffered` cannot be
//     driven that way and say so. A flag kill needs a unit of one side to destroy the other
//     side's flag Tank across nine columns of board, and a cap tiebreak needs a KILL before
//     the cap (the mutual-passivity guard fires first otherwise, which is exactly what
//     T-TURN-05 pins). `FStratBridge` exposes no writer for `strat::GameState`, and this
//     file may not add one -- `Tests/` is this agent's lane. So those two arrange the
//     `TurnState` the bridge itself is holding: `MakeUiWorld()` is called on a live seeded
//     bridge, `*w.turn` is COPIED, the copy is handed to the rules module's own graders
//     (`strat::checkImmediate`, `strat::beginTurn`, `strat::endTurn`), and the graded state
//     is read back through `strat::uiMatchResult` -- the exact function
//     `FStratBridge::MatchResult` calls, and the only one between the rules and the screen.
//     WHAT THEY DO NOT PIN, stated rather than implied: they do not exercise
//     `FStratBridge::MatchResult`'s own body. `StratMatchResultRouting.cpp`'s two T-INT-05
//     clauses do that, and the clause above drives it on a concluded match.
//
// WHERE EVERY EXPECTATION COMES FROM.
//   - The `BoardSnapshot` handed to the graders is BUILT FROM THE BRIDGE'S OWN PROJECTION
//     (`BoardFromSnapshot` below) -- flags alive from the unit list's `isFlag`, combat Fame,
//     objectives held and surviving HP from `UiSideView`, factories held and factory total
//     from `UiFactoryView`. It is a real board, not an invention, and the ARRANGEMENT is a
//     single named edit to it. That edit is the clause's INPUT and never its expectation.
//   - The winner side is derived from the arrangement, both directions: the flag-kill clause
//     loops over EVERY side, kills that side's flag, and requires the winner to be the OTHER
//     one. A `winner = 0` constant fails the second pass.
//   - `decidedByKey`'s expected value is §2.8's own numbering of its three keys --
//     1 combat Fame, 2 objectives held, 3 surviving HP -- which is the GDD's, quoted in
//     T-TURN-04 ("the attrition tiebreak resolves in the exact §2.8 order: combat Fame ->
//     objectives held -> surviving HP -> draw"). No module-side value names the key, and
//     naming it is the whole content of T-TURN-04, so this is a literal from a cited
//     document rather than a computation.
//   - `SIDE_NONE` is `strat::SIDE_NONE`, never `-1`.
//   - The turn cap of the variant scenario is `turnCap`'s own value read back off the
//     projection, and the number written into the file is a FIXTURE COORDINATE.
//
// THE VARIANT SCENARIO LANDS IN `Saved/StratTests/`, NEVER IN `Data/`, and is deleted at the
// end of the clause that writes it. `Data/` is vendored and hash-gated; the same helper
// shape and the same reasoning are in `StratBridgeRestoreParity.cpp`, which writes a
// `turnCap`-edited variant for T-SAVE-06.
//
// EVERY STRING COMPARISON IS CASE SENSITIVE. `FString::operator==`, `Contains` and
// `TestEqual` all ignore case in UE 5.8, measured on this project.
//
// WHY THIS FILE IS IN StratBridge/Tests. It calls `strat::` free functions --
// `uiMatchResult`, `buildUiSnapshot`, `checkImmediate`, `beginTurn`, `endTurn`,
// `applyStartOfTurnRepair`. The vendored sources are compiled INTO `StratBridge` as
// `Vendored/*.strat.cpp` and carry no `_API` macro, so in any other module every one of
// those is `LNK2019` rather than a test.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBridge.h"

#include "Turn.h"
#include "Ui.h"

#include <string>
#include <vector>

namespace StratMatchResultClauses
{
	/** The harness pin every bridge parity file in this suite carries, and it must stay
	 *  equal to theirs. `Replay.h` states that no rule in this project decides which side
	 *  moves first, so this is a harness constant and not a rule. It is also the reason the
	 *  capped clause can arrange `winner != sideToMove` deliberately: the side that closes
	 *  the capped round is the LAST one in the alternation, never `firstSide`. */
	static const int32 kFirstSide = 0;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static FString ShippedScenarioPath()
	{
		return FPaths::Combine(DataDir(), TEXT("ferrum_crossing.json"));
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	static FString FromStd(const std::string& In)
	{
		return FString(UTF8_TO_TCHAR(In.c_str()));
	}

	/** Definitions mapped in and the named scenario seeded through the rules module's own
	 *  `seedFromScenario`. Returns false and fills OutError rather than asserting, on
	 *  `StratBridgeParity.cpp`'s precedent. */
	static bool SeedBridgeFrom(FStratBridge& Bridge, const FString& ScenarioPath, FString& OutError)
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

		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath, kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		return SeedBridgeFrom(Bridge, ShippedScenarioPath(), OutError);
	}

	/**
	 * §2.8's facts about the board, READ OFF THE BRIDGE'S OWN PROJECTION.
	 *
	 * Every field here is a value the snapshot already carries -- which is the point of
	 * `strat::BoardSnapshot`'s design ("every field is a quantity the game already tracks
	 * for the economy and already displays on the standings scoreboard", `Turn.h`). Nothing
	 * is invented and nothing is summed that the module does not sum: `factoriesHeld` and
	 * `factoryTotal` are counts over `UiSnapshot::factories`, whose `owner` is the
	 * objective's own, and `flagAlive` asks whether the unit list still contains a unit of
	 * that side carrying `isFlag`.
	 *
	 * The ARRANGEMENT each clause makes is a single named edit to what this returns, and it
	 * is stated at the edit rather than hidden here.
	 */
	static strat::BoardSnapshot BoardFromSnapshot(const strat::UiSnapshot& S)
	{
		strat::BoardSnapshot B;

		B.factoryTotal = static_cast<int>(S.factories.size());

		for (int Side = 0; Side < strat::SIDE_COUNT; ++Side)
		{
			B.side[Side].fameCombat     = S.side[Side].fameCombat;
			B.side[Side].objectivesHeld = S.side[Side].objectivesHeld;
			B.side[Side].survivingHp    = S.side[Side].survivingHp;
			B.side[Side].factoriesHeld  = 0;
			B.side[Side].flagAlive      = false;
		}

		for (const strat::UiFactoryView& F : S.factories)
		{
			if (F.owner >= 0 && F.owner < strat::SIDE_COUNT)
			{
				B.side[F.owner].factoriesHeld += 1;
			}
		}

		for (const strat::UiUnitView& U : S.units)
		{
			if (U.isFlag && U.side >= 0 && U.side < strat::SIDE_COUNT)
			{
				B.side[U.side].flagAlive = true;
			}
		}

		return B;
	}

	/**
	 * The turn loop's own way from "a side is up" to "that side may act", so `endTurn` --
	 * which refuses in every other phase -- can be driven twice in a row.
	 *
	 * NO PHASE IS ASSIGNED BY THIS FILE. `beginTurn` and `applyStartOfTurnRepair` are the
	 * module's own advancers and each refuses harmlessly from a phase it does not own, so
	 * this is correct whatever phase the seeded bridge happens to hand over. The repair pass
	 * is given an EMPTY subject list: it is here to advance the phase, and offering it
	 * subjects would be this file deciding which units repair, which is `Combat.h`'s answer
	 * and not needed by any assertion below.
	 */
	static void ToActions(strat::TurnState& S, const strat::BoardSnapshot& B)
	{
		strat::beginTurn(S, B);
		strat::applyStartOfTurnRepair(S, std::vector<strat::RepairSubject>());
	}

	/** A world with this file's arranged turn state in place of the bridge's own. Everything
	 *  else -- board, units, tables, economy -- is still the bridge's. */
	static strat::UiWorld WorldWithTurn(const strat::UiWorld& Live, const strat::TurnState& S)
	{
		strat::UiWorld W = Live;
		W.turn = &S;
		return W;
	}

	static FString TierName(strat::ResultTier T)
	{
		return FromStd(std::string(strat::tierName(T)));
	}

	static FString CauseName(strat::ResultCause C)
	{
		return FromStd(std::string(strat::causeName(C)));
	}

	/** The `turnCap: 1` variant's path under `Saved/`. Never under `Data/`. */
	static FString CappedScenarioPath()
	{
		return FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("StratTests"),
			TEXT("match_result_turncap_one.json"));
	}

	/**
	 * Writes `ferrum_crossing.json` with `turnCap` set to 1, ONE neutral objective handed to
	 * side 0, and the optional declared `scenarioHash` dropped.
	 *
	 * WHY THE OBJECTIVE MOVES, and it is not cosmetic. The shipped scenario gives each side
	 * exactly one objective, so the two sides tie on §2.8's key 2 -- and a `resolveAtCap`
	 * that WRONGLY fell through the mutual-passivity guard to the keys would tie at key 2,
	 * tie at key 3, and reach step 3's draw anyway. The clause would then be green while
	 * blind to the very fall-through T-TURN-05 forbids. Handing side 0 a second objective
	 * makes the two counts unequal, so a fall-through crowns side 0 and the clause reddens.
	 * THE HEX NAMED IS A NEUTRAL TOWN AND NOT A FACTORY, and that is a measurement rather
	 * than a preference: handing side 0 a second FACTORY is refused outright by the scenario
	 * validator -- "side 0 owns 2 factories at start; exactly one home factory is required
	 * (§2.7)" -- so the variant would not load at all. A town carries the same objective count
	 * without touching the home-factory rule, and the clause still asserts off the projection
	 * that no side holds every factory, because domination (T-TURN-03) would otherwise end the
	 * match before the cap.
	 *
	 * WHY 1. `strat::endTurn` resolves the cap at the END of the round whose number equals
	 * `turnCap`, so a cap of 1 puts the resolution two `SubmitEndTurn` calls from the
	 * opening -- which is the only way this suite can reach a genuinely concluded bridge
	 * without playing a match. It is a FIXTURE COORDINATE: nothing below asserts that the
	 * cap is 1, and the clause reads the cap back off the projection.
	 *
	 * WHY THE HASH LINE GOES. It is optional and DERIVED (`Scenario.h`, reading 4), so
	 * dropping it is the correct way to say "recompute from content" rather than authoring a
	 * number for edited bytes. Same edit, same reasoning, as
	 * `StratBridgeRestoreParity.cpp`'s variant writer.
	 *
	 * BOTH EDITS ARE LOCATED CASE-SENSITIVELY and both are checked for before they are made,
	 * so a re-authored scenario file fails this helper loudly instead of silently producing
	 * a variant identical to the shipped one.
	 */
	static bool WriteCappedScenario(FString& OutPath, FString& OutError)
	{
		FString Text;
		if (!FFileHelper::LoadFileToString(Text, *ShippedScenarioPath()))
		{
			OutError = FString::Printf(TEXT("ferrum_crossing.json unreadable at %s"),
				*ShippedScenarioPath());
			return false;
		}

		const int32 KeyAt = Text.Find(TEXT("\"scenarioHash\""), ESearchCase::CaseSensitive);
		if (KeyAt == INDEX_NONE)
		{
			OutError = TEXT("ferrum_crossing.json does not carry a 'scenarioHash' member to drop");
			return false;
		}
		const int32 LineEnd = Text.Find(TEXT("\n"), ESearchCase::CaseSensitive,
			ESearchDir::FromStart, KeyAt);
		if (LineEnd == INDEX_NONE)
		{
			OutError = TEXT("the 'scenarioHash' member is on the file's last line; this helper "
			                "expects it to be followed by a newline");
			return false;
		}
		Text.RemoveAt(KeyAt, (LineEnd + 1) - KeyAt);

		if (Text.Find(TEXT("\"turnCap\": 20"), ESearchCase::CaseSensitive) == INDEX_NONE)
		{
			OutError = TEXT("ferrum_crossing.json no longer declares \"turnCap\": 20; this "
			                "helper's edit is stale and the variant would not reach the cap");
			return false;
		}
		Text = Text.Replace(TEXT("\"turnCap\": 20"), TEXT("\"turnCap\": 1"),
			ESearchCase::CaseSensitive);

		const TCHAR* const kNeutralEntry = TEXT("{\"hex\": [3, 1], \"owner\": -1}");
		const TCHAR* const kOwnedEntry   = TEXT("{\"hex\": [3, 1], \"owner\": 0}");
		if (Text.Find(kNeutralEntry, ESearchCase::CaseSensitive) == INDEX_NONE)
		{
			OutError = TEXT("ferrum_crossing.json no longer carries the neutral objective entry "
			                "this helper hands to side 0; the variant's two sides would tie on "
			                "§2.8 key 2 and the no-fall-through half of T-TURN-05 would be blind");
			return false;
		}
		Text = Text.Replace(kNeutralEntry, kOwnedEntry, ESearchCase::CaseSensitive);

		OutPath = CappedScenarioPath();

		// `FFileHelper::SaveStringToFile` does not build a missing tree, and a first run on a
		// clean checkout has no `Saved/StratTests/`.
		IPlatformFile& Files = FPlatformFileManager::Get().GetPlatformFile();
		Files.CreateDirectoryTree(*FPaths::GetPath(OutPath));

		if (!FFileHelper::SaveStringToFile(Text, *OutPath))
		{
			OutError = FString::Printf(TEXT("could not write the capped scenario to %s"), *OutPath);
			return false;
		}
		return true;
	}

	static void RemoveCappedScenario()
	{
		IPlatformFile& Files = FPlatformFileManager::Get().GetPlatformFile();
		const FString Path = CappedScenarioPath();
		if (Files.FileExists(*Path))
		{
			Files.DeleteFile(*Path);
		}
	}
}

// ---------------------------------------------------------------------------
// T-TURN-02 -- "flag death ends the match immediately: Decisive win FOR THE KILLER, loss
// for the owner (§2.8)."
//
// THE SECOND HALF OF THAT SENTENCE IS THE SUBJECT. `FStratMatchView::ResultTier` has always
// been able to say *Decisive*; nothing in this engine could say for whom. This clause reads
// the winner through `strat::uiMatchResult` -- the query `FStratBridge::MatchResult` routes
// and the only path a result reaches the engine by.
//
// BOTH SIDES ARE KILLED, ONE AT A TIME, ON A FRESH ARRANGEMENT EACH TIME. The expected
// winner is therefore "the OTHER side", derived from the loop variable, and a projection
// that returned a constant 0 -- or that returned `sideToMove`, which is 0 on a fresh
// seeded bridge -- fails the second pass. That is the whole reason the loop is here rather
// than a single case.
//
// THE ARRANGEMENT IS ONE FIELD. `BoardFromSnapshot` reports the real opening board, in
// which both flags are alive; the clause sets ONE side's `flagAlive` false and hands the
// board to `strat::checkImmediate`, the module's own grader. Nothing else is edited and the
// tier, cause, winner and key are all the module's answers.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratFlagKillNamesTheKillerAsWinnerTest,
	"Stratocracy.StratBridge.T-TURN-02.FlagKillNamesTheKillerAsWinner",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratFlagKillNamesTheKillerAsWinnerTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchResultClauses;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge seeds"), SeedBridge(Bridge, Error)))
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

	const strat::UiWorld       Live    = Bridge.MakeUiWorld();
	const strat::BoardSnapshot Opening = BoardFromSnapshot(Snapshot);

	// COVERAGE, not decoration. If the shipped scenario ever stopped deploying a flag for
	// both sides, every pass below would arrange a board a legal match cannot reach and
	// `checkImmediate` would refuse to grade it -- passing nothing while looking green.
	for (int32 Side = 0; Side < strat::SIDE_COUNT; ++Side)
	{
		if (!TestTrue(
				*FString::Printf(TEXT("the shipped scenario deploys a living flag for side %d"), Side),
				Opening.side[Side].flagAlive))
		{
			return false;
		}
	}

	const FString HashBefore = Bridge.StateHash();
	const int32   LogBefore  = Bridge.RecordedCommandCount();

	for (int32 Loser = 0; Loser < strat::SIDE_COUNT; ++Loser)
	{
		// THE ARRANGEMENT. One field of a real board. The expected winner below is derived
		// from this loop, never written down.
		strat::BoardSnapshot Board = Opening;
		Board.side[Loser].flagAlive = false;

		strat::TurnState Arranged = *Live.turn;
		const strat::MatchResult Graded = strat::checkImmediate(Arranged, Board);

		const strat::UiWorld       World    = WorldWithTurn(Live, Arranged);
		const strat::UiMatchResult Reported = strat::uiMatchResult(World);
		const strat::UiSnapshot    Same     = strat::buildUiSnapshot(World);

		const FString Where = FString::Printf(TEXT("side %d's flag is destroyed"), Loser);

		TestEqual(*FString::Printf(TEXT("%s: the tier is Decisive (module says '%s')"),
				*Where, *TierName(Reported.tier)),
			static_cast<int32>(Reported.tier), static_cast<int32>(strat::ResultTier::Decisive));
		TestEqual(*FString::Printf(TEXT("%s: the cause is FlagDestroyed (module says '%s')"),
				*Where, *CauseName(Reported.cause)),
			static_cast<int32>(Reported.cause),
			static_cast<int32>(strat::ResultCause::FlagDestroyed));

		// THE CLAUSE WITH TEETH, and it is the sentence T-TURN-02 adds to `resultTier`.
		// `OtherSide` is this loop's arithmetic over SIDE_COUNT and not an expectation about
		// the rules: it names WHICH side was not the one killed.
		const int32 OtherSide = (Loser + 1) % strat::SIDE_COUNT;
		TestEqual(*FString::Printf(
				TEXT("%s: the winner is the killer -- side %d, not side %d"),
				*Where, OtherSide, Loser),
			static_cast<int32>(Reported.winner), OtherSide);

		// No tiebreak was evaluated: a flag kill ends the match before the cap (T-CAP-01,
		// T-CAP-04), so §2.8's key numbering has nothing to report.
		TestEqual(*FString::Printf(TEXT("%s: no §2.8 key decided it"), *Where),
			static_cast<int32>(Reported.decidedByKey), 0);

		// The query reports `TurnState::result` and nothing else -- so what it says must
		// equal what the module's own grader returned, field for field, on the same state.
		TestEqual(*FString::Printf(TEXT("%s: the query mirrors checkImmediate's tier"), *Where),
			static_cast<int32>(Reported.tier), static_cast<int32>(Graded.tier));
		TestEqual(*FString::Printf(TEXT("%s: the query mirrors checkImmediate's cause"), *Where),
			static_cast<int32>(Reported.cause), static_cast<int32>(Graded.cause));
		TestEqual(*FString::Printf(TEXT("%s: the query mirrors checkImmediate's winner"), *Where),
			static_cast<int32>(Reported.winner), static_cast<int32>(Graded.winner));
		TestEqual(*FString::Printf(TEXT("%s: the query mirrors checkImmediate's key"), *Where),
			static_cast<int32>(Reported.decidedByKey), static_cast<int32>(Graded.decidedByKey));

		// The snapshot's own match block, from the SAME world in the SAME frame. It must
		// agree about the tier -- one result, not two spellings of it -- and it still cannot
		// name a winner, which is why `uiMatchResult` exists.
		TestTrue(*FString::Printf(TEXT("%s: the snapshot agrees the match has a result"), *Where),
			Same.match.hasResult);
		TestEqual(*FString::Printf(TEXT("%s: the snapshot's tier is the same tier"), *Where),
			static_cast<int32>(Same.match.resultTier), static_cast<int32>(Reported.tier));

		AddInfo(FString::Printf(
			TEXT("%s -> tier '%s', cause '%s', winner %d, key %d, sideToMove %d"),
			*Where, *TierName(Reported.tier), *CauseName(Reported.cause),
			Reported.winner, Reported.decidedByKey, Same.match.sideToMove));
	}

	// Grading a copy is not playing. Both witnesses, after every pass.
	TestEqual(TEXT("arranging and grading did not move the canonical state hash"),
		Bridge.StateHash(), HashBefore);
	TestEqual(TEXT("arranging and grading submitted no command"),
		Bridge.RecordedCommandCount(), LogBefore);

	return true;
}

// ---------------------------------------------------------------------------
// T-TURN-04 -- "at the turn cap, the attrition tiebreak resolves in the exact §2.8 order:
// combat Fame -> objectives held -> surviving HP -> draw."
//
// AND THIS IS THE CLAUSE THAT ARRANGES `winner != sideToMove`. It is the only shape in
// which the two can disagree: a flag kill ends the match mid-turn with the killer to move,
// while the cap resolves at a ROUND BOUNDARY, where `activeSide` is whichever side closed
// the round -- the LAST side in the alternation, never `firstSide`. The arrangement makes
// SIDE 0 (`kFirstSide`) lead every comparison, so the winner is 0 and the side to move is
// not. Both numbers are read from the module on one world in one frame and asserted
// unequal; neither is written down.
//
// WITHOUT THIS ARRANGEMENT THE OTHER TWO CLAUSES PASS AGAINST A `sideToMove` DERIVATION,
// and the projection loss item 1 exists to close would stay open under a green suite.
//
// ALL THREE KEYS, IN §2.8's ORDER, EACH IN ITS TURN. Key 1 differs alone; then key 1 is
// tied at a NONZERO value and key 2 differs; then keys 1 and 2 are tied and key 3 differs.
// The nonzero tie is load-bearing rather than tidy: `resolveAtCap`'s first step is the
// mutual-passivity guard, and a tie at ZERO draws immediately with no fall-through --
// which is T-TURN-05's subject and is asserted there.
//
// WHERE THE EXPECTED KEY NUMBER COMES FROM. No module-side value names the key; naming it
// is the entire content of `decidedByKey` and of T-TURN-04. So the expectation is the
// GDD's own numbering, quoted in the acceptance line above, and the clause is written so
// the arrangement and the expectation are the SAME loop variable -- key `k` is made to
// differ, and `decidedByKey` is required to equal `k`.
//
// THE EDITS ARE MADE TO A REAL BOARD. `BoardFromSnapshot` reports the opening's true Fame,
// objectives and HP; each case takes that board and moves the fields it names.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCappedMatchNamesTheKeyThatDifferedTest,
	"Stratocracy.StratBridge.T-TURN-04.CappedMatchNamesTheKeyThatDiffered",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCappedMatchNamesTheKeyThatDifferedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchResultClauses;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge seeds"), SeedBridge(Bridge, Error)))
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

	const strat::UiWorld       Live    = Bridge.MakeUiWorld();
	const strat::BoardSnapshot Opening = BoardFromSnapshot(Snapshot);

	if (!TestNotNull(TEXT("a seeded bridge's world carries a turn state"), Live.turn))
	{
		return false;
	}

	// The side that will lead every comparison. It is `kFirstSide` DELIBERATELY: the round's
	// last ender holds `activeSide` when the cap resolves, and `firstSide` is never the last
	// ender, so this choice is what makes `winner != sideToMove` true by construction rather
	// than by luck.
	const int32 kLeader = kFirstSide;

	const FString HashBefore = Bridge.StateHash();
	const int32   LogBefore  = Bridge.RecordedCommandCount();

	// §2.8's three keys, in §2.8's order. `Key` is BOTH the arrangement selector and the
	// expected `decidedByKey`, which is the only way this clause can name a number without
	// restating one.
	for (int32 Key = 1; Key <= 3; ++Key)
	{
		strat::BoardSnapshot Board = Opening;

		// Somebody fought, in every case: without it `resolveAtCap` draws at step 1 and no
		// key is ever consulted. The ties below are therefore ties at a NONZERO value.
		for (int32 Side = 0; Side < strat::SIDE_COUNT; ++Side)
		{
			Board.side[Side].fameCombat     = 1;
			Board.side[Side].objectivesHeld = Opening.side[kLeader].objectivesHeld;
			Board.side[Side].survivingHp    = Opening.side[kLeader].survivingHp;
		}

		// THE ONE FIELD THAT DIFFERS, and it is the key this pass is about.
		switch (Key)
		{
		case 1:  Board.side[kLeader].fameCombat     += 1; break;
		case 2:  Board.side[kLeader].objectivesHeld += 1; break;
		default: Board.side[kLeader].survivingHp    += 1; break;
		}

		// Domination is checked at `beginTurn` and would end the match before the cap. The
		// opening board holds a factory per side, so this is the real state and not a dodge;
		// it is asserted rather than assumed, because a scenario re-authored with one factory
		// would make every pass below grade a Decisive domination win and still look green.
		if (!TestTrue(
				*FString::Printf(TEXT("key %d: no side holds every factory at the opening "
					"(side 0 holds %d, side 1 holds %d, of %d)"),
					Key, Board.side[0].factoriesHeld, Board.side[1].factoriesHeld,
					Board.factoryTotal),
				Board.factoryTotal > 0 &&
				Board.side[0].factoriesHeld != Board.factoryTotal &&
				Board.side[1].factoriesHeld != Board.factoryTotal))
		{
			return false;
		}

		// Play the round out at the cap, through the module's own loop. `turnNumber` is set
		// to the state's OWN `turnCap` -- a fixture coordinate read from the scenario, never
		// a literal 20 -- so the round that is about to close is the capped one.
		strat::TurnState Arranged = *Live.turn;
		Arranged.turnNumber = Arranged.turnCap;

		for (int32 Ender = 0; Ender < strat::SIDE_COUNT; ++Ender)
		{
			ToActions(Arranged, Board);
			strat::endTurn(Arranged, Board);
		}

		const strat::UiWorld       World    = WorldWithTurn(Live, Arranged);
		const strat::UiMatchResult Reported = strat::uiMatchResult(World);
		const strat::UiSnapshot    Same     = strat::buildUiSnapshot(World);

		const FString Where = FString::Printf(TEXT("key %d differs at the cap"), Key);

		// The round really did close on the cap rather than on something else.
		if (!TestTrue(*FString::Printf(TEXT("%s: the match ended (tier '%s')"),
					*Where, *TierName(Reported.tier)),
				Reported.tier != strat::ResultTier::InProgress))
		{
			AddError(FString::Printf(
				TEXT("%s: the arranged round did not resolve; turn %d of cap %d, tier '%s'"),
				*Where, Arranged.turnNumber, Arranged.turnCap, *TierName(Reported.tier)));
			continue;
		}
		TestEqual(*FString::Printf(TEXT("%s: the cause is AttritionLead (module says '%s')"),
				*Where, *CauseName(Reported.cause)),
			static_cast<int32>(Reported.cause),
			static_cast<int32>(strat::ResultCause::AttritionLead));
		TestEqual(*FString::Printf(TEXT("%s: a capped lead is Marginal, not Decisive (module says '%s')"),
				*Where, *TierName(Reported.tier)),
			static_cast<int32>(Reported.tier), static_cast<int32>(strat::ResultTier::Marginal));

		// THE FIRST CLAUSE WITH TEETH: the §2.8 criterion that differed is NAMED, and the
		// number it is named by is the one this pass arranged.
		TestEqual(*FString::Printf(TEXT("%s: decidedByKey names key %d"), *Where, Key),
			static_cast<int32>(Reported.decidedByKey), Key);
		TestEqual(*FString::Printf(TEXT("%s: the leader wins"), *Where),
			static_cast<int32>(Reported.winner), kLeader);

		// THE SECOND, AND IT IS THE REASON THIS FILE EXISTS. The winner and the side to move
		// are read from the module on one world in one frame, and they DISAGREE. A downstream
		// surface deriving the winner from `sideToMove` would announce the wrong faction on
		// exactly this match.
		TestNotEqual(*FString::Printf(
				TEXT("%s: the winner (%d) is NOT the side to move (%d) -- a sideToMove "
					"derivation would be wrong here"),
				*Where, Reported.winner, Same.match.sideToMove),
			static_cast<int32>(Reported.winner), static_cast<int32>(Same.match.sideToMove));

		TestTrue(*FString::Printf(TEXT("%s: the snapshot agrees the match has a result"), *Where),
			Same.match.hasResult);
		TestEqual(*FString::Printf(TEXT("%s: the snapshot's tier is the same tier"), *Where),
			static_cast<int32>(Same.match.resultTier), static_cast<int32>(Reported.tier));

		AddInfo(FString::Printf(
			TEXT("%s -> tier '%s', cause '%s', winner %d, key %d, sideToMove %d"),
			*Where, *TierName(Reported.tier), *CauseName(Reported.cause),
			Reported.winner, Reported.decidedByKey, Same.match.sideToMove));
	}

	TestEqual(TEXT("arranging and grading did not move the canonical state hash"),
		Bridge.StateHash(), HashBefore);
	TestEqual(TEXT("arranging and grading submitted no command"),
		Bridge.RecordedCommandCount(), LogBefore);

	return true;
}

// ---------------------------------------------------------------------------
// T-TURN-05 -- "mutual-passivity guard: both sides' fameCombat == 0 at the cap -> immediate
// draw, with NO fall-through to objectives held (§2.8)."
//
// THIS ONE IS DRIVEN AND NOT ARRANGED, END TO END, THROUGH THE PRODUCTION SURFACE. A
// variant of the shipped scenario with `turnCap` 1 puts the cap two commands away, so the
// clause plays the match: `SubmitEndTurn`, `SubmitEndTurn`, and then
// `FStratBridge::MatchResult` -- the method §2.11.4's screen will call -- on a bridge whose
// own `strat::GameState` holds the result. No `TurnState` is touched anywhere below.
//
// NOBODY FIGHTS, BECAUSE NOBODY IS ASKED TO. Two `EndTurn` commands and nothing else, so
// both sides reach the cap with zero combat Fame under their own steam. That the guard then
// fires is the module's answer and not this file's arrangement -- and the clause ASSERTS the
// precondition off the projection rather than assuming it, so a scenario that started a side
// with combat Fame would fail loudly instead of grading a different rule.
//
// NO FALL-THROUGH IS THE HALF WITH TEETH. §2.8 step 1 draws IMMEDIATELY and must not consult
// objectives held -- otherwise a turtle who sat on more factories is re-crowned, which is
// the outcome the guard exists to forbid. So the clause requires the two sides to hold
// UNEQUAL objective counts at the cap (read off the projection; the shipped scenario gives
// side 0 and side 1 different holdings only if it does, so this is a COVERAGE GUARD that
// bails out loudly rather than a silent pass) and still requires a draw with no winner. A
// `resolveAtCap` that fell through to key 2 would crown the leader and fail here.
//
// `SIDE_NONE` IS `strat::SIDE_NONE`, and `FStratMatchResultView` maps it to `INDEX_NONE`
// one layer up; neither constant is written as `-1` anywhere in this suite.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratMutualPassivityIsADrawWithNoWinnerTest,
	"Stratocracy.StratBridge.T-TURN-05.MutualPassivityIsADrawWithNoWinner",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratMutualPassivityIsADrawWithNoWinnerTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchResultClauses;

	FString VariantPath;
	FString Error;
	if (!TestTrue(TEXT("a turnCap-1 variant of the shipped scenario is written under Saved/"),
			WriteCappedScenario(VariantPath, Error)))
	{
		AddError(Error);
		RemoveCappedScenario();
		return false;
	}

	{
		FStratBridge Bridge;
		if (!TestTrue(TEXT("the bridge seeds from the variant"),
				SeedBridgeFrom(Bridge, VariantPath, Error)))
		{
			AddError(Error);
			RemoveCappedScenario();
			return false;
		}

		strat::UiSnapshot Opening;
		const FStratResult Projected = Bridge.MakeUiSnapshot(Opening);
		if (!TestTrue(TEXT("MakeUiSnapshot succeeds on the seeded variant"), Projected.bOk))
		{
			AddError(Projected.Reason);
			RemoveCappedScenario();
			return false;
		}

		// The fixture coordinate, read back rather than restated.
		AddInfo(FString::Printf(TEXT("the variant's cap is turn %d"), Opening.match.turnCap));

		// PRECONDITION, ASSERTED. The guard is about BOTH sides being at zero, so a scenario
		// that opened a side with combat Fame would send this clause to a different rule.
		for (int32 Side = 0; Side < strat::SIDE_COUNT; ++Side)
		{
			if (!TestEqual(
					*FString::Printf(TEXT("side %d opens with zero combat Fame"), Side),
					Opening.side[Side].fameCombat, 0))
			{
				RemoveCappedScenario();
				return false;
			}
		}

		// COVERAGE for the "no fall-through" half. If the two sides held EQUAL objective
		// counts, a `resolveAtCap` that wrongly fell through to key 2 would tie there and
		// reach step 3's draw anyway -- the same verdict for the wrong reason, and this
		// clause would be green while blind. Bail out loudly instead of passing.
		if (!TestNotEqual(
				*FString::Printf(TEXT("the two sides hold UNEQUAL objective counts (%d vs %d), "
					"so a fall-through to §2.8 key 2 would crown one of them"),
					Opening.side[0].objectivesHeld, Opening.side[1].objectivesHeld),
				Opening.side[0].objectivesHeld, Opening.side[1].objectivesHeld))
		{
			RemoveCappedScenario();
			return false;
		}

		// Domination (T-TURN-03) would end the match at `beginTurn`, before the cap. Read off
		// the projection rather than assumed, because the variant hands one factory over and
		// a re-authored map could make that the whole set.
		{
			const strat::BoardSnapshot OpeningBoard = BoardFromSnapshot(Opening);
			if (!TestTrue(
					*FString::Printf(TEXT("no side holds every factory (side 0 holds %d, side 1 "
						"holds %d, of %d), so the match reaches the cap"),
						OpeningBoard.side[0].factoriesHeld, OpeningBoard.side[1].factoriesHeld,
						OpeningBoard.factoryTotal),
					OpeningBoard.factoryTotal > 0 &&
					OpeningBoard.side[0].factoriesHeld != OpeningBoard.factoryTotal &&
					OpeningBoard.side[1].factoriesHeld != OpeningBoard.factoryTotal))
			{
				RemoveCappedScenario();
				return false;
			}
		}

		// A result is not already sitting there. Without this the two EndTurns below could
		// prove nothing.
		{
			strat::UiMatchResult BeforePlay;
			const FStratResult Asked = Bridge.MatchResult(BeforePlay);
			TestTrue(TEXT("MatchResult answers a seeded bridge"), Asked.bOk);
			TestEqual(TEXT("the opening is InProgress"),
				static_cast<int32>(BeforePlay.tier),
				static_cast<int32>(strat::ResultTier::InProgress));
			TestEqual(TEXT("the opening names no winner"),
				static_cast<int32>(BeforePlay.winner), static_cast<int32>(strat::SIDE_NONE));
		}

		// ---- Play the capped round out -----------------------------------------
		for (int32 Ender = 0; Ender < strat::SIDE_COUNT; ++Ender)
		{
			const int32 SideToMove = Bridge.SideToMove();
			const FStratResult Ended = Bridge.SubmitEndTurn();
			if (!TestTrue(*FString::Printf(TEXT("side %d's EndTurn is accepted"), SideToMove),
					Ended.bOk))
			{
				AddError(Ended.Reason);
				RemoveCappedScenario();
				return false;
			}
		}

		// ---- What the production method says -----------------------------------
		strat::UiMatchResult Result;
		const FStratResult Asked = Bridge.MatchResult(Result);
		if (!TestTrue(TEXT("MatchResult answers the concluded bridge"), Asked.bOk))
		{
			AddError(Asked.Reason);
			RemoveCappedScenario();
			return false;
		}

		TestEqual(*FString::Printf(TEXT("the capped match is a Draw (module says '%s')"),
				*TierName(Result.tier)),
			static_cast<int32>(Result.tier), static_cast<int32>(strat::ResultTier::Draw));
		TestEqual(*FString::Printf(TEXT("the cause is the PassivityGuard (module says '%s')"),
				*CauseName(Result.cause)),
			static_cast<int32>(Result.cause),
			static_cast<int32>(strat::ResultCause::PassivityGuard));

		// THE CLAUSE WITH TEETH. A draw names NOBODY, and the objective counts asserted
		// unequal above are what makes this a statement about the guard rather than a
		// coincidence.
		TestEqual(TEXT("a draw names no winner"),
			static_cast<int32>(Result.winner), static_cast<int32>(strat::SIDE_NONE));
		TestEqual(TEXT("and no §2.8 key was ever evaluated"),
			static_cast<int32>(Result.decidedByKey), 0);

		// The snapshot, from the same bridge, must agree the match ended and must agree about
		// the tier -- one result, not two spellings of it.
		strat::UiSnapshot Concluded;
		const FStratResult Reprojected = Bridge.MakeUiSnapshot(Concluded);
		if (TestTrue(TEXT("MakeUiSnapshot succeeds on the concluded bridge"), Reprojected.bOk))
		{
			TestTrue(TEXT("the snapshot agrees the match has a result"), Concluded.match.hasResult);
			TestEqual(TEXT("the snapshot's tier is the same tier"),
				static_cast<int32>(Concluded.match.resultTier), static_cast<int32>(Result.tier));

			AddInfo(FString::Printf(
				TEXT("concluded at turn %d of %d: tier '%s', cause '%s', winner %d, key %d, "
					"sideToMove %d, objectives %d vs %d"),
				Concluded.match.turn, Concluded.match.turnCap,
				*TierName(Result.tier), *CauseName(Result.cause),
				Result.winner, Result.decidedByKey, Concluded.match.sideToMove,
				Concluded.side[0].objectivesHeld, Concluded.side[1].objectivesHeld));
		}
	}

	RemoveCappedScenario();
	return true;
}
