// GDD §2.7 / §2.11 -- what a `Capture` command actually does. T-FAME-05.
//
// WHY THIS CLAUSE EXISTS, AND WHAT IT MAKES FALSIFIABLE. `FStratBridge::SubmitCapture` is
// KEPT -- it is §4.10's save-format command surface, `strat::SaveCommandKind` is pinned at
// the five §4.10 names, and `StratBridgeRestoreParity.cpp` and `StratBridgeSaveRecording.cpp`
// both dispatch it in log replay, so deleting it would break replay of any log containing a
// Capture entry. What is REFUSED is wiring a player affordance to it, and the design says so
// in as many words: §2.11 -- "Capture and build need no extra verbs. Capture is by presence
// ... a progress pip appears, NO BUTTON" -- while `strat::AiCommandKind` is
// `{Build, Move, Attack, EndTurn}` for the same reason and `Ai.h` states it.
//
// THE REASON NEVER TO WIRE ONE IS A MEASUREMENT AND NOT AN OPINION, AND THIS FILE IS THE
// MEASUREMENT. `strat::applyCommand`'s Capture arm never reads `c.unitId`: it builds
// occupants from every unit on the board and calls `strat::captureTick(g.economy, occ,
// c.side)` -- the identical block `strat::openTurn` already runs at every turn boundary. And
// `captureTick` IS NOT IDEMPOTENT: it INCREMENTS `turnsHeld` per call. So N Capture commands
// inside one turn advance a capture N turns' worth. An affordance wired to that method would
// ship a cheat. Recorded as a comment in three headers, it was an assertion; here it is a
// number.
//
// TWO HALVES, AND THEY MEASURE DIFFERENT THINGS.
//
//   (a) THE NON-IDEMPOTENCE ITSELF, at the module, on a hand-built `strat::EconomyState`.
//       IT CANNOT BE MEASURED THROUGH THE BRIDGE and this is the whole reason for the
//       hand-built state: `captureTurns` is per-scenario data in principle, but the seeding
//       path (`strat::seedFromScenario`, and `Replay.good.cpp`'s reseed beside it) sets it to
//       1 unconditionally and the scenario schema carries no key for it. At N = 1 the FIRST
//       tick completes the capture, flips the owner and CLEARS the progress record, so
//       `turnsHeld` never reads 2 on any board this engine can seed -- the exact masking the
//       filing predicted. The clause therefore builds one objective and one occupant with
//       `captureTurns` above 1, which is a FIXTURE INPUT, and measures `turnsHeld` after one
//       tick and after two. The one-tick state is the CONTROL: without it "2 after two calls"
//       is satisfied by a counter that jumps to 2 on the first.
//
//   (b) THAT THE COMMAND SURFACE ACCEPTS THEM UNGATED, through the shipped bridge. Two
//       `SubmitCapture` calls in ONE turn are both accepted and both recorded, with the turn
//       number and the side to move unmoved. There is no per-turn allowance on this command,
//       unlike Build (T-TURN-10) and unlike the per-unit act flag (T-TURN-01) -- the Capture
//       arm consults neither -- so nothing in the rules module would stop a button being
//       clicked twice.
//
// TOGETHER THEY SAY THE WHOLE SENTENCE: each Capture command is a full tick, and the surface
// takes as many as you send. Either half alone is quotable and misleading.
//
// WHERE THE EXPECTATIONS COME FROM.
//   - `turnsHeld` is read back off `strat::EconomyState::captures`, the module's own record,
//     never inferred from a return value.
//   - The unit id handed to `SubmitCapture` is read off the projection. It is an ARGUMENT and
//     not an expectation -- the arm ignores it, which is itself part of the finding, and (b)
//     asserts nothing about which unit was named.
//   - The turn number and side to move are `FStratBridge::Turn()` and `SideToMove()`.
//   - `captureTurns` above 1 and the single objective's hex are fixture inputs, named as such.
//
// ON THE ACCEPTANCE ID. T-FAME-05 is the GDD's capture line -- "capture: Infantry only;
// completes after N turns of HOLDING (N = 1 on the shipped scenario, per-scenario data);
// progress is tile-held and RESETS TO ZERO when the capturing Infantry leaves the hex or
// dies". `turnsHeld` is the counter that sentence is about, so this is the ID that owns the
// property. It is NOT filed under T-SAVE-05: that ID's GDD text is "no partial load -- a log
// with an illegal command at index k is refused whole", which is a different subject.
//
// WHY THIS FILE IS IN StratBridge/Tests. It calls `strat::captureTick` and names
// `strat::EconomyState`; the vendored sources are compiled into this module and carry no
// `_API` macro, so anywhere else that call is `LNK2019` rather than a test.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBridge.h"

#include "Economy.h"
#include "Hex.h"
#include "Ui.h"

#include <vector>

namespace StratCaptureCommandClauses
{
	static const int32 kFirstSide = 0;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

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
	 * One neutral objective, one Infantry-shaped occupant standing on it, and a capture length
	 * this engine's seeding path cannot produce.
	 *
	 * `captureTurns` IS A FIXTURE INPUT AND IS DELIBERATELY NOT 1. At 1 -- which is what
	 * `seedFromScenario` writes unconditionally, and the scenario schema has no key to change
	 * it -- the first tick completes the capture and CLEARS the progress record, so the
	 * counter this clause is about is unobservable. Anything above 2 leaves both ticks below
	 * short of completion, so `turnsHeld` survives to be read.
	 *
	 * THE HEX IS A FIXTURE COORDINATE. This economy has exactly one objective and nothing
	 * else, so which hex it sits on is arbitrary and nothing below compares against it.
	 */
	static const int kCaptureTurnsForTheFixture = 3;
	static const int kOccupantUnitId            = 7;

	static strat::EconomyState MakeOneObjectiveEconomy()
	{
		strat::EconomyState E;
		E.captureTurns = kCaptureTurnsForTheFixture;

		strat::Objective O;
		O.hex          = strat::offsetToAxial(0, 0);
		O.owner        = strat::OWNER_NEUTRAL;
		O.terrainIndex = 0;
		E.objectives.push_back(O);

		return E;
	}

	static std::vector<strat::CaptureOccupant> MakeOccupantOn(const strat::Hex& H, int Side)
	{
		strat::CaptureOccupant C;
		C.hex        = H;
		C.unitId     = kOccupantUnitId;
		C.side       = Side;
		C.canCapture = true;          // Infantry only (T-DATA-03); this fixture is one.

		std::vector<strat::CaptureOccupant> Occ;
		Occ.push_back(C);
		return Occ;
	}

	/** `turnsHeld` for that hex, read off the module's own record. -1 when no record exists,
	 *  which is what a COMPLETED or a CLEARED capture leaves behind and is distinguishable
	 *  from a zero. */
	static int32 TurnsHeldAt(const strat::EconomyState& E, const strat::Hex& H)
	{
		for (const strat::CaptureProgress& P : E.captures)
		{
			if (strat::hexEqual(P.hex, H))
			{
				return P.turnsHeld;
			}
		}
		return INDEX_NONE;
	}

	static int32 OwnerAt(const strat::EconomyState& E, const strat::Hex& H)
	{
		for (const strat::Objective& O : E.objectives)
		{
			if (strat::hexEqual(O.hex, H))
			{
				return O.owner;
			}
		}
		return strat::OWNER_NEUTRAL;
	}
}

// ---------------------------------------------------------------------------
// Two Capture commands in one turn advance holding TWICE -- and the surface takes both.
//
// See this file's header block for why the first half is measured at the module and the
// second at the bridge, and for why neither half can be measured where the other one is.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratTwoCaptureCommandsAdvanceHoldingTwiceTest,
	"Stratocracy.StratBridge.T-FAME-05.TwoCaptureCommandsInOneTurnAdvanceHoldingTwice",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratTwoCaptureCommandsAdvanceHoldingTwiceTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratCaptureCommandClauses;

	// ---- (a) The non-idempotence, at the module -----------------------------
	{
		const strat::Hex Objective = strat::offsetToAxial(0, 0);

		// THE CONTROL, AND IT IS NOT DECORATION. Without a one-tick state to compare against,
		// "turnsHeld is 2 after two ticks" is satisfied by a counter that jumps straight to 2
		// on the first tick -- which would be a different defect reading as the same number.
		strat::EconomyState Once = MakeOneObjectiveEconomy();
		strat::captureTick(Once, MakeOccupantOn(Objective, kFirstSide), kFirstSide);

		strat::EconomyState Twice = MakeOneObjectiveEconomy();
		strat::captureTick(Twice, MakeOccupantOn(Objective, kFirstSide), kFirstSide);
		strat::captureTick(Twice, MakeOccupantOn(Objective, kFirstSide), kFirstSide);

		const int32 AfterOne = TurnsHeldAt(Once,  Objective);
		const int32 AfterTwo = TurnsHeldAt(Twice, Objective);

		// The fixture is doing what it says: neither state completed, so both still carry a
		// progress record to read. A completed capture clears the record, and `TurnsHeldAt`
		// would answer INDEX_NONE -- which is why that value is distinguishable from 0 here.
		if (!TestTrue(
				*FString::Printf(TEXT("the fixture's captureTurns of %d is longer than two "
					"ticks, so neither state completed (one tick -> %d, two -> %d)"),
					kCaptureTurnsForTheFixture, AfterOne, AfterTwo),
				AfterOne != INDEX_NONE && AfterTwo != INDEX_NONE))
		{
			return false;
		}

		TestEqual(TEXT("one captureTick advances holding by one turn"), AfterOne, 1);

		// THE CLAUSE WITH TEETH. `captureTick` is not idempotent: called twice inside a single
		// turn it advances the capture two turns' worth. This is the number that makes "never
		// wire a Capture affordance" a finding rather than an opinion.
		TestEqual(
			*FString::Printf(TEXT("TWO captureTicks inside one turn advance holding to 2, not "
				"to 1 -- the command is NOT idempotent (one tick left it at %d)"), AfterOne),
			AfterTwo, 2);
		TestEqual(TEXT("and the second tick really did move it"), AfterTwo - AfterOne, 1);

		// Neither reached the fixture's length, so ownership has not flipped -- which is what
		// keeps the two numbers above readable rather than cleared.
		TestEqual(TEXT("the objective is still unowned after one tick"),
			OwnerAt(Once, Objective), static_cast<int32>(strat::OWNER_NEUTRAL));
		TestEqual(TEXT("and after two"),
			OwnerAt(Twice, Objective), static_cast<int32>(strat::OWNER_NEUTRAL));

		AddInfo(FString::Printf(
			TEXT("captureTurns=%d: turnsHeld is %d after one tick and %d after two"),
			kCaptureTurnsForTheFixture, AfterOne, AfterTwo));
	}

	// ---- (b) The command surface accepts them ungated, at the bridge --------
	{
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

		// AN ARGUMENT, NOT AN EXPECTATION. The Capture arm never reads it -- it builds
		// occupants from every unit on the board -- and that is part of what this clause
		// records. It is read off the projection rather than invented so the call is a
		// well-formed §4.10 command.
		int32 AnyUnitId = INDEX_NONE;
		for (const strat::UiUnitView& U : Snapshot.units)
		{
			if (U.side == Bridge.SideToMove())
			{
				AnyUnitId = U.id;
				break;
			}
		}
		if (!TestTrue(TEXT("the side to move has a unit to name in a Capture command"),
				AnyUnitId != INDEX_NONE))
		{
			return false;
		}

		const int32 TurnBefore = Bridge.Turn();
		const int32 SideBefore = Bridge.SideToMove();
		const int32 LogBefore  = Bridge.RecordedCommandCount();

		const FStratResult First  = Bridge.SubmitCapture(AnyUnitId);
		const FStratResult Second = Bridge.SubmitCapture(AnyUnitId);

		if (!TestTrue(TEXT("the first Capture command is accepted"), First.bOk))
		{
			AddError(First.Reason);
			return false;
		}

		// THE HALF WITH TEETH ON THIS SIDE. Nothing refuses the second one: the Capture arm
		// consults neither T-TURN-10's per-factory allowance nor T-TURN-01's per-unit act
		// flag, both of which gate the other command kinds. A button wired here could be
		// clicked as many times as the player liked, inside one turn.
		if (!TestTrue(
				*FString::Printf(TEXT("the SECOND Capture command in the same turn is ALSO "
					"accepted -- nothing gates it (reason if refused: '%s')"), *Second.Reason),
				Second.bOk))
		{
			return false;
		}

		TestEqual(TEXT("both were inside ONE turn: the turn number did not move"),
			Bridge.Turn(), TurnBefore);
		TestEqual(TEXT("and the side to move did not change"),
			Bridge.SideToMove(), SideBefore);
		TestEqual(TEXT("and BOTH were recorded into the §4.10 log"),
			Bridge.RecordedCommandCount(), LogBefore + 2);

		AddInfo(FString::Printf(
			TEXT("two Capture commands accepted at turn %d, side %d; log %d -> %d"),
			TurnBefore, SideBefore, LogBefore, Bridge.RecordedCommandCount()));
	}

	return true;
}
