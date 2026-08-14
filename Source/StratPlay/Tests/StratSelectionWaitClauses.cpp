// THE TWO WAIT CLAUSES PHASE 6 OWED. GDD §2.11.1, §4.9, §4.11 row 10.
//
//   T-INT-05  the presentation block is the MACHINE's, produced by `DecorateViewModel` from
//             the machine's own state. A wait is the event that makes that concrete, because
//             it spends a unit's turn and reaches no rules module at all.
//
// WHAT THESE TWO CLAUSES ARE FOR, AND WHAT THEY DELIBERATELY DO NOT DO.
//
// Phase 6 added `STRAT-WAIT spent` to `StratSelectionMachine.cpp` because the captured
// playtest could not witness a wait: a wait submits nothing, so no `STRAT-CMD` line is
// printed, and the only trace left behind was that re-clicking the unit said "unit N has
// finished this turn" -- which an accepted ATTACK also produces, because
// `NotifyCommandApplied` adds the attacker to `DoneUnits` too
// (`StratSelectionMachine.cpp:390`). Two different events, one indistinguishable trace.
//
// A CLAUSE THAT DROVE `HandleEvent(Wait)` AND THEN ASSERTED THAT A `STRAT-WAIT spent` LINE
// APPEARED WOULD BE PINNING `UE_LOG` AGAINST ITSELF -- subject and witness in one, which is
// the exact shape phase 4's first BLOCK was raised for. The line's EXISTENCE is not the
// property worth having a gate for. The property the line was added to BUY is that a wait
// and an accepted attack are DISTINGUISHABLE IN THE MACHINE'S OBSERVABLE OUTPUT, and that
// distinction lives in `FStratSelectionOutcome::Command` -- a value the machine returns,
// which no logging statement can satisfy and no logging change can break.
//
// So `WaitIsDistinguishableFromAttack` below asserts the OUTCOME STRUCTS differ while the
// DONE bits agree, and it asserts the agreeing half POSITIVELY: both units come back
// `bDone == true`, so the clause states plainly that the shared consequence is real and
// that it is the command that separates the two events. Without that half a reader could
// take the clause to mean a wait simply failed.
//
// THIS IS THE CLAUSE THAT FAILS IF SOMEBODY LATER "SIMPLIFIES" A WAIT INTO A SUBMITTED
// COMMAND. `strat::SaveCommandKind` is `{Move, Attack, Build, Capture, EndTurn}`
// (`Save.h:54`) and has no `Wait`; phase 1 recorded the orchestration brief as wrong to
// claim otherwise. A wait that emitted any command would move the state hash and would
// change `Waited.Command` away from `None`, and both are asserted here.
//
// WHERE THE EXPECTATIONS COME FROM, which is the only question that matters.
//   - The attack's target is `FStratBridgeRulesQuery::AttackTargetHexes`'s own enumeration.
//     There is not one hex literal in this file.
//   - The attack's acceptance is `FStratBridge`'s, and the "the state moved" half is
//     `FStratBridge::StateHash()` before and after -- never a predicted hash.
//   - The wait's "the state did NOT move" half is the same method, compared against its own
//     earlier reading.
//   - The DONE bits are read back off a model `StratBuildViewModel` built and
//     `DecorateViewModel` decorated. This file writes no model field.
//   - The two refusal SENTENCES in `WaitWithNothingSelectedIsANoOp` are compared against
//     EACH OTHER -- one produced by the guard under test, one produced by the machine's
//     done-set arm on the same machine -- so both sides of that comparison are module-side.
//     See the note on the refusal text on that clause; there is no accessor for it, and this
//     file does not hardcode a copy.
//
// THE ONE PLACE A LOG LINE IS OBSERVED, AND WHY IT IS NOT THE SELF-WITNESSING SHAPE.
// `WaitWithNothingSelectedIsANoOp` captures `STRAT-WAIT` off `GLog` to assert the line is
// ABSENT on the no-op path -- the guard at `StratSelectionMachine.cpp:156-160` breaks out
// before the line, so a no-op wait cannot report a success -- and the same clause then
// drives a REAL wait through the same live capture and requires exactly one line. The
// capture is therefore proven able to see a line before its silence is read as meaning
// anything. The load-bearing assertions on that path remain the outcome and the done set;
// the absence check corroborates them and cannot pass by being blind.
//
// NO `strat::` TYPE IS NAMED AND NO `strat::` FREE FUNCTION IS CALLED, per
// `StratPlay.Build.cs`. Every rules answer arrives engine-typed.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "CoreGlobals.h"
#include "Engine/DataTable.h"
#include "Math/IntPoint.h"
#include "HAL/CriticalSection.h"
#include "Misc/OutputDevice.h"
#include "Misc/OutputDeviceRedirector.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratSelectionMachine.h"
#include "StratViewModel.h"

// Included from a .cpp and never from a UHT-parsed header -- the module-wide discipline.
#include "StratBridge.h"

namespace StratSelectionWaitClauses
{
	// The same harness pin every parity file in this tree carries. `Replay.h` states that no
	// rule decides which side moves first, so this is a harness constant and not a rule.
	static const int32 kFirstSide = 0;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	static FString ScenarioPath()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"), TEXT("ferrum_crossing.json"));
	}

	static FString Describe(FIntPoint Hex)
	{
		return FString::Printf(TEXT("axial(%d,%d)"), Hex.X, Hex.Y);
	}

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

		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath(), kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	/**
	 * Build -> decorate, which is the path `UStratMatchSubsystem` runs and the order the
	 * running game uses. The viewing side follows the side to move, which is what a hot-seat
	 * hand-over is; the machine reads the active side off `Model.Match.SideToMove` regardless.
	 */
	static bool Refresh(const FStratBridge& Bridge, const FStratSelectionMachine& Machine,
	                    FStratViewModel& OutModel, FString& OutError)
	{
		if (!StratBuildViewModel(Bridge, Bridge.SideToMove(), OutModel, OutError))
		{
			return false;
		}
		Machine.DecorateViewModel(OutModel);
		return true;
	}

	static const FStratUnitView* FindUnit(const FStratViewModel& Model, int32 UnitId)
	{
		for (const FStratUnitView& U : Model.Units)
		{
			if (U.UnitId == UnitId)
			{
				return &U;
			}
		}
		return nullptr;
	}

	static const FStratUnitView* FindUnitAtHex(const FStratViewModel& Model, FIntPoint Hex)
	{
		for (const FStratUnitView& U : Model.Units)
		{
			if (U.Hex == Hex)
			{
				return &U;
			}
		}
		return nullptr;
	}

	/**
	 * Axial hex distance. A FIXTURE DEVICE AND NEVER AN EXPECTATION.
	 *
	 * Its one job is deciding WHICH element of the module's own reachable set the scripted
	 * advance clicks on, so that the two sides meet and a legal attack comes into existence.
	 * No clause compares anything against it, no hex is called reachable or attackable
	 * because of it, and a wrong answer here makes the advance wander rather than making any
	 * assertion easier to pass. The same standing `StratHotSeatReplayParity.cpp` records for
	 * its `AdvanceHexDistance`, and deliberately not shared with
	 * `StratSelectionMachineParity.cpp`'s `FoilHexDistance` -- that one is a FOIL the real set
	 * must DIFFER from, and merging the two would put a "distance" helper into general
	 * circulation in the module that is supposed to have none.
	 */
	static int32 AdvanceHexDistance(FIntPoint A, FIntPoint B)
	{
		const int32 dq = A.X - B.X;
		const int32 dr = A.Y - B.Y;
		return (FMath::Abs(dq) + FMath::Abs(dq + dr) + FMath::Abs(dr)) / 2;
	}

	/** How far the nearest unit of another side is, or MAX_int32 if there is none. */
	static int32 NearestEnemyDistance(const FStratViewModel& Model, FIntPoint From, int32 Side)
	{
		int32 Best = MAX_int32;
		for (const FStratUnitView& U : Model.Units)
		{
			if (U.Side == Side)
			{
				continue;
			}
			Best = FMath::Min(Best, AdvanceHexDistance(From, U.Hex));
		}
		return Best;
	}

	/**
	 * The element of `ReachableHexes`'s OWN ANSWER that stands closest to an enemy.
	 *
	 * THE SET IS THE MODULE'S; ONLY THE CHOICE WITHIN IT IS THIS FILE'S. Taking "the first"
	 * never brings the two sides into contact, so no click sequence a test could script would
	 * ever reach the machine's Attack branch -- Ferrum Crossing deploys the sides on opposite
	 * edges and nothing is in range on turn 1 (measured, and recorded in
	 * `StratSelectionFacadeParity.cpp` as `0 enumerated targets` at the seeded opening).
	 */
	static bool PickAdvance(const IStratRulesQuery& Query, const FStratViewModel& Model,
	                        int32 UnitId, FIntPoint Origin, int32 Side, FIntPoint& OutHex,
	                        FString& OutError)
	{
		TArray<FIntPoint> Reach;
		if (!Query.ReachableHexes(UnitId, Reach, OutError))
		{
			return false;
		}

		bool      bFound       = false;
		int32     BestDistance = MAX_int32;
		FIntPoint Best         = Origin;
		for (const FIntPoint& H : Reach)
		{
			if (H == Origin || FindUnitAtHex(Model, H) != nullptr)
			{
				continue;
			}
			const int32 D = NearestEnemyDistance(Model, H, Side);
			if (!bFound || D < BestDistance)
			{
				bFound       = true;
				BestDistance = D;
				Best         = H;
			}
		}

		if (!bFound)
		{
			OutError = FString::Printf(
				TEXT("unit %d has no unoccupied reachable hex other than its own"), UnitId);
			return false;
		}
		OutHex = Best;
		return true;
	}

	/**
	 * Captures the `STRAT-WAIT` lines this module emits.
	 *
	 * USED FOR ONE THING ONLY, AND IT IS NOT "THE LINE EXISTS". See this file's header block:
	 * the no-op clause reads this capture's SILENCE on the guarded path, and proves the
	 * capture is live by driving a real wait through it in the same clause.
	 *
	 * UNBUFFERED, AND THE SILENCE CLAUSE IS WHY IT MATTERS MOST HERE. Without the override,
	 * `FOutputDeviceRedirector` can deliver a device lines that were emitted BEFORE it was
	 * constructed: a line it cannot broadcast on the primary-thread fast path is queued
	 * (OutputDeviceRedirector.cpp:937) and later drained to whichever devices sit in
	 * `BufferedOutputDevices` AT DRAIN TIME (:553). An earlier clause's `STRAT-WAIT spent`
	 * line could therefore land inside this window and make `LinesAfterNoOp` non-zero with
	 * nothing at all wrong in the module -- the same defect measured as a 1-in-4 failure of
	 * `T-UI-01.ClickedAttackIsAcceptedAndRecorded` on 2026-08-14.
	 *
	 * IT DOES NOT WEAKEN THE POSITIVE CONTROL, and that was checked before it was made. The
	 * override removes only a route by which lines from OUTSIDE the window arrive; lines
	 * emitted INSIDE it now arrive synchronously, within the emitting `UE_LOG` call (:905),
	 * strictly sooner than before. The real wait's line is in `Lines` before `HandleEvent` has
	 * returned, so `LinesAfterRealWait > LinesAfterNoOp` is if anything more reliable. Both
	 * halves of the clause keep exactly the meaning they had.
	 *
	 * THE LOCK COVERS THE APPEND AND NOTHING ELSE, AND THAT RESIDUAL IS DELIBERATE RATHER
	 * THAN OVERLOOKED. The override advertises to the redirector that this device needs no
	 * external locking, and the engine takes it literally -- every thread's log lines now
	 * arrive in `Serialize` directly, so the one mutation of `Lines` is made under `Mutex`.
	 * Every READ is UNLOCKED: this device exposes no accessor at all, and the clauses below
	 * read `Lines.Num()` straight off the public member to take their watermarks. That is
	 * safe only because every `STRAT-WAIT` emitter runs on the game thread and every reader
	 * here is on the game thread too, so no read is ever concurrent with the append.
	 * NOTHING IN THE CODE PINS THAT PROPERTY: it is a fact about the current emitters, not
	 * an invariant anyone enforces. Move `StratSelectionMachine.cpp`'s `STRAT-WAIT spent`
	 * `UE_LOG` onto a worker thread and these reads become a data race that no clause here
	 * would report -- it would surface as a flake, not as a red test. Widening the lock to
	 * cover the reads is the fix if that day ever comes.
	 *
	 * THE FILTER IS CASE-INSENSITIVE, AND UNLIKE THE `STRAT-AI` CAPTURES IT IS LEFT THAT
	 * WAY KNOWINGLY. `FString::StartsWith` defaults to `ESearchCase::IgnoreCase` in UE 5.8,
	 * and this project has already shipped a clause that could not fail because of exactly
	 * that default -- so the omission needs a reason rather than a shrug. The reason is that
	 * this filter cannot be weakened by it in either direction. The prefixes that occur
	 * anywhere in this tree -- `STRAT-WAIT`, `STRAT-CMD`, `STRAT-AI`, `STRAT-COMBAT`, and
	 * `STRAT-PROBE` (a phase-6 debug probe named only inside a comment in
	 * `StratPlayerController.cpp`, with no live emitter) -- differ from one another in
	 * LETTERS, never in case, and every emitter spells its prefix upper case; so no line
	 * that should be rejected here can be admitted by case-folding alone.
	 *
	 * THE DIRECTION OF ANY HYPOTHETICAL ERROR HERE IS THE SAFE ONE, FOR BOTH HALVES OF THE
	 * CLAUSE. An over-permissive filter can only ADD lines. That pushes `LinesAfterNoOp`
	 * above zero and turns the silence clause RED, and it cannot break the positive control
	 * either, since a case-mangled line would still be counted and
	 * `LinesAfterRealWait > LinesAfterNoOp` would still hold. There is no input,
	 * case-variant or otherwise, that makes this clause pass when it should fail.
	 *
	 * EVERY ASSERTION IN THIS FILE THAT READS THE CAPTURE IS A COUNT, WHICH IS THE WHOLE OF
	 * THE ARGUMENT. The scope of that sentence is load-bearing, and an earlier draft of it
	 * dropped the qualifier and was simply false: this file makes plenty of assertions that
	 * are not counts -- `Machine.IsDone(WaiterId)`, `Waited.FailureReason.IsEmpty()`,
	 * `GetSelectedUnitId()`, `Bridge.StateHash()` against a watermark -- and the claim was
	 * written as though none of them existed. They do. The point is that NOT ONE OF THEM
	 * READS A CAPTURED LINE, so the filter's case-folding cannot reach them however it is
	 * spelled.
	 *
	 * MEASURED, rather than asserted in the general: exactly two assertions here take their
	 * subject from this device -- `LinesAfterNoOp == 0` and
	 * `LinesAfterRealWait > LinesAfterNoOp` -- and both turn on HOW MANY lines there are,
	 * never on what any one of them SAYS. They are not even the same shape of count: one is
	 * an exact `TestEqual`, the other a strict inequality. "All counts" is the property the
	 * argument needs; "all `TestEqual` on an exact count" would be another sentence claiming
	 * more than it can carry. `Capture.Lines` is read in one other place -- the `AddInfo` loop
	 * that dumps the whole window UNCONDITIONALLY after both assertions, pass or fail -- and
	 * that asserts nothing.
	 *
	 * THAT IS WHAT DISTINGUISHES THIS CAPTURE from `StratHotSeatReplayParity.cpp`'s, whose
	 * `STRAT-CMD` capture keeps the same loose filter for the same reason, but which also
	 * carries an assertion whose subject is a SPELLING rather than an arity -- the phase-6
	 * grep contract -- and that one gets no guarantee from any count. It is therefore
	 * `ESearchCase::CaseSensitive` there, through `IsGrepContractAcceptedLine`, and shown
	 * able to fail by `T-SAVE-05.GrepContractRejectsACaseVariant`. Nothing in THIS file
	 * asserts a spelling, so nothing here needs the same treatment: the split is
	 * instruments-loose / claims-strict, not file-by-file. If a clause is ever added below
	 * that asserts what a `STRAT-WAIT` line SAYS rather than how many there are, that clause
	 * -- and not this filter -- is what must be `CaseSensitive`.
	 */
	struct FStratWaitCapture final : public FOutputDevice
	{
		TArray<FString> Lines;

		FStratWaitCapture()
		{
			if (GLog != nullptr)
			{
				GLog->AddOutputDevice(this);
			}
		}

		virtual ~FStratWaitCapture()
		{
			if (GLog != nullptr)
			{
				GLog->RemoveOutputDevice(this);
			}
		}

		/** See the block above. Removing this line reopens the late-delivery hole. */
		virtual bool CanBeUsedOnMultipleThreads() const override { return true; }

		virtual void Serialize(const TCHAR* Message, ELogVerbosity::Type /*Verbosity*/,
		                       const FName& /*Category*/) override
		{
			const FString Line(Message);
			if (Line.StartsWith(TEXT("STRAT-WAIT")))
			{
				FScopeLock Lock(&Mutex);
				Lines.Add(Line);
			}
		}

		/**
		 * Kept, and no longer load-bearing. An unbuffered device already holds every line by
		 * the time the emitting `UE_LOG` returns; this now only pushes the OTHER devices'
		 * buffers so a failure message's surrounding log reads in order.
		 */
		void Settle()
		{
			if (GLog != nullptr)
			{
				GLog->Flush();
			}
		}

	private:
		FCriticalSection Mutex;
	};
}

// ---------------------------------------------------------------------------
// T-INT-05 -- A WAIT AND AN ACCEPTED ATTACK ARE DISTINGUISHABLE, AND THE THING THAT
// DISTINGUISHES THEM IS THE OUTCOME STRUCT.
//
// THE AMBIGUITY THIS CLAUSE IS ABOUT IS REAL AND WAS OBSERVED. Both events end with
// `bDone == true` on the decorated model, and both make a re-click say "unit N has finished
// this turn" -- an accepted Attack adds to `DoneUnits` in `NotifyCommandApplied`
// (`StratSelectionMachine.cpp:390`) exactly as a Wait does in `HandleEvent`
// (`:168`). In phase 6's captured session that was the whole of the trace, which is why the
// playtest could not witness a wait at all.
//
// WHAT IS ASSERTED IS THE PROPERTY THE LOG LINE WAS ADDED TO BUY, NOT THE LINE. The wait's
// outcome carries `EStratSelectionCommand::None` and names no unit; the attack's carries
// `Attack`, the attacker's id, and the hex the module itself enumerated. That difference is
// a returned value. No `UE_LOG` can satisfy it, deleting every logging statement in the
// module cannot break it, and turning a wait into a submitted command -- into a save format
// that has no `Wait` kind -- fails it twice: on `Waited.Command` and on the state hash.
//
// THE SHARED HALF IS ASSERTED POSITIVELY AND ON PURPOSE. Both units come back DONE. A
// clause that only said "the wait produced no command" would read as though the wait had
// failed; §2.11.1's wait SPENDS the unit's turn, and that is the point of it.
//
// THE ADVANCE IS FIXTURE CONSTRUCTION AND SAYS SO. Every hex clicked to bring the sides
// into contact is an element of `ReachableHexes`'s own answer; `PickAdvance` decides only
// WHICH element, and no assertion below compares anything against it. The TARGET is not
// chosen that way -- it is `AttackTargetHexes`'s own first element.
//
// A SKIP IS A FAILURE. If contact never arises the clause has asserted nothing about the
// attack half, and it fails saying how close the two sides came.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratWaitIsDistinguishableFromAttackTest,
	"Stratocracy.StratPlay.T-INT-05.WaitIsDistinguishableFromAttack",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratWaitIsDistinguishableFromAttackTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSelectionWaitClauses;

	// NO `AddExpectedMessagePlain` FOR `STRAT-CMD refused`, and that is a measurement rather
	// than an omission -- `StratHotSeatReplayParity.cpp` records the same one. Nothing below
	// submits anything the rules module rejects: every move is an element of `ReachableHexes`
	// and the attack is an element of `AttackTargetHexes`, so no refusal line is emitted and
	// declaring one would fail the clause on "expected message did not occur".
	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratSelectionMachine Machine;
	FStratBridgeRulesQuery Query(&Bridge);

	// Generous, and bounded -- the same ceiling `T-UI-01.ClickedAttackIsAcceptedAndRecorded`
	// uses, for the same reason: room for the board to be re-authored without flakiness, and
	// still a hard stop rather than an unbounded loop.
	const int32 kMaxTurns = 12;

	bool  bPinned        = false;
	int32 ClosestContact = MAX_int32;
	int32 MovesPlayed    = 0;
	int32 TurnsPlayed    = 0;

	for (int32 Leg = 0; Leg < kMaxTurns && !bPinned; ++Leg)
	{
		FStratViewModel Model;
		if (!TestTrue(*FString::Printf(TEXT("leg %d: the model builds"), Leg),
				Refresh(Bridge, Machine, Model, Error)))
		{
			AddError(Error);
			return false;
		}
		const int32 ActiveSide = Model.Match.SideToMove;
		++TurnsPlayed;

		TArray<int32> ActiveUnitIds;
		for (const FStratUnitView& U : Model.Units)
		{
			if (U.Side == ActiveSide)
			{
				ActiveUnitIds.Add(U.UnitId);
			}
		}

		for (int32 UnitId : ActiveUnitIds)
		{
			if (bPinned)
			{
				break;
			}
			if (!Refresh(Bridge, Machine, Model, Error))
			{
				AddError(Error);
				return false;
			}

			const FStratUnitView* Unit = FindUnit(Model, UnitId);
			if (Unit == nullptr || Unit->bDone || Unit->Side != Model.Match.SideToMove)
			{
				continue;
			}
			ClosestContact = FMath::Min(ClosestContact,
				NearestEnemyDistance(Model, Unit->Hex, Unit->Side));

			Machine.HandleEvent(EStratSelectionEvent::HexPrimary, Unit->Hex, Model, Query);
			if (Machine.GetSelectedUnitId() != UnitId)
			{
				continue;
			}

			for (int32 Attempt = 0; Attempt < 2 && !bPinned; ++Attempt)
			{
				TArray<FIntPoint> Targets;
				FString           TargetReason;
				if (!Query.AttackTargetHexes(UnitId, Targets, TargetReason))
				{
					AddError(TargetReason);
					return false;
				}

				if (Targets.Num() > 0)
				{
					// ================= THE CLAUSE =================================
					if (!Refresh(Bridge, Machine, Model, Error))
					{
						AddError(Error);
						return false;
					}

					const FIntPoint Target = Targets[0];

					// ---- a second active unit, for the WAIT half ------------------
					// It must be a different unit from the attacker, on the side to move, and
					// not already finished -- a finished unit cannot even be selected, which is
					// the done-set arm this clause is not about.
					int32     WaiterId  = INDEX_NONE;
					FIntPoint WaiterHex = FIntPoint::ZeroValue;
					for (const FStratUnitView& U : Model.Units)
					{
						if (U.Side == Model.Match.SideToMove && U.UnitId != UnitId &&
							!Machine.IsDone(U.UnitId))
						{
							WaiterId  = U.UnitId;
							WaiterHex = U.Hex;
							break;
						}
					}
					if (!TestTrue(
							TEXT("the side to move still has a second unspent unit to wait with"),
							WaiterId != INDEX_NONE))
					{
						return false;
					}

					// ---- the WAIT -----------------------------------------------
					// The attacker is currently selected; the wait needs its own selection, and
					// re-selecting through `HexPrimary` is the way a player reaches it.
					Machine.HandleEvent(EStratSelectionEvent::HexPrimary, WaiterHex, Model, Query);
					if (!TestEqual(TEXT("the second unit selects"),
							Machine.GetSelectedUnitId(), WaiterId))
					{
						return false;
					}

					const FString HashBeforeWait = Bridge.StateHash();

					const FStratSelectionOutcome Waited = Machine.HandleEvent(
						EStratSelectionEvent::Wait, FIntPoint::ZeroValue, Model, Query);

					TestEqual(
						TEXT("T-INT-05: a WAIT yields EStratSelectionCommand::None -- there is no Wait "
						     "in strat::SaveCommandKind and the save format cannot carry one"),
						static_cast<int32>(Waited.Command),
						static_cast<int32>(EStratSelectionCommand::None));
					TestEqual(TEXT("T-INT-05: and it names no unit to command"),
						Waited.UnitId, static_cast<int32>(INDEX_NONE));
					TestTrue(
						TEXT("T-INT-05: a wait is not a refusal -- it carries no failure reason"),
						Waited.FailureReason.IsEmpty());
					TestEqual(TEXT("a wait deselects"),
						Machine.GetSelectedUnitId(), static_cast<int32>(INDEX_NONE));
					TestTrue(TEXT("§2.11.1: the waited unit is DONE"), Machine.IsDone(WaiterId));
					TestEqual(
						TEXT("T-INT-05: and the rules state did NOT move -- a wait reaches no rules "
						     "module"),
						Bridge.StateHash(), HashBeforeWait);

					// ---- the ATTACK ---------------------------------------------
					if (!Refresh(Bridge, Machine, Model, Error))
					{
						AddError(Error);
						return false;
					}

					const FStratUnitView* const Attacker = FindUnit(Model, UnitId);
					if (!TestNotNull(TEXT("the attacker is still on the board"), Attacker))
					{
						return false;
					}
					Machine.HandleEvent(
						EStratSelectionEvent::HexPrimary, Attacker->Hex, Model, Query);
					if (!TestEqual(TEXT("the attacker re-selects"),
							Machine.GetSelectedUnitId(), UnitId))
					{
						return false;
					}

					const FStratUnitView* const Victim = FindUnitAtHex(Model, Target);
					if (!TestNotNull(
							*FString::Printf(
								TEXT("the enumerated target %s is occupied by a unit the model carries"),
								*Describe(Target)),
							Victim))
					{
						return false;
					}
					TestTrue(TEXT("and that unit belongs to the other side"),
						Victim->Side != Model.Match.SideToMove);

					const FStratSelectionOutcome Attacked = Machine.HandleEvent(
						EStratSelectionEvent::HexPrimary, Target, Model, Query);

					if (!TestEqual(
							*FString::Printf(
								TEXT("T-INT-05: clicking the enemy at %s with unit %d selected is an "
								     "ATTACK"), *Describe(Target), UnitId),
							static_cast<int32>(Attacked.Command),
							static_cast<int32>(EStratSelectionCommand::Attack)))
					{
						AddError(Attacked.FailureReason);
						return false;
					}
					TestEqual(TEXT("T-INT-05: the attack NAMES the attacking unit"),
						Attacked.UnitId, UnitId);
					TestEqual(TEXT("T-INT-05: and carries the hex the module enumerated"),
						Attacked.Hex, Target);

					const FString HashBeforeAttack = Bridge.StateHash();
					FString AttackFailure;
					if (!TestTrue(
							TEXT("the rules module ACCEPTS the attack, so the comparison below is "
							     "against a real accepted command and not a proposed one"),
							StratSubmitSelectionCommand(Bridge, Attacked, AttackFailure)))
					{
						AddError(AttackFailure);
						return false;
					}
					Machine.NotifyCommandApplied(Attacked);

					TestNotEqual(
						TEXT("T-INT-05: the accepted attack MOVED the rules state -- which the wait, "
						     "measured above against the same method, did not"),
						Bridge.StateHash(), HashBeforeAttack);
					TestTrue(TEXT("§2.11.1: an accepted attack marks the attacker DONE"),
						Machine.IsDone(UnitId));

					// ---- THE DISTINCTION, IN ONE LINE ---------------------------
					TestNotEqual(
						TEXT("T-INT-05: the two outcomes differ in the machine's OWN RETURNED VALUE -- "
						     "a wait is None and an accepted attack is Attack. This is what makes a "
						     "wait witnessable without reading a log line, and no UE_LOG can satisfy "
						     "it"),
						static_cast<int32>(Waited.Command), static_cast<int32>(Attacked.Command));

					// ---- and the SHARED consequence is real ---------------------
					FStratViewModel Decorated;
					if (!TestTrue(TEXT("the model rebuilds and decorates after both events"),
							Refresh(Bridge, Machine, Decorated, Error)))
					{
						AddError(Error);
						return false;
					}

					const FStratUnitView* const WaiterView   = FindUnit(Decorated, WaiterId);
					const FStratUnitView* const AttackerView = FindUnit(Decorated, UnitId);
					if (!TestNotNull(TEXT("the waiter is on the decorated model"), WaiterView) ||
						!TestNotNull(TEXT("the attacker is on the decorated model"), AttackerView))
					{
						return false;
					}
					TestTrue(
						TEXT("T-INT-05: the WAITED unit's bDone is TRUE -- §2.11.1's wait SPENDS the "
						     "unit's turn; this clause is not 'the wait did nothing'"),
						WaiterView->bDone);
					TestTrue(
						TEXT("T-INT-05: the ATTACKING unit's bDone is TRUE too -- the consequence the "
						     "two events share, which is exactly why the outcome above has to be what "
						     "tells them apart"),
						AttackerView->bDone);

					// The ambiguity itself, stated where a reader will find it. Both refusals are
					// the machine's own words; this file hardcodes neither.
					const FStratSelectionOutcome ReclickWaiter = Machine.HandleEvent(
						EStratSelectionEvent::HexPrimary, WaiterView->Hex, Decorated, Query);
					const FStratSelectionOutcome ReclickAttacker = Machine.HandleEvent(
						EStratSelectionEvent::HexPrimary, AttackerView->Hex, Decorated, Query);
					TestFalse(TEXT("re-clicking the waited unit is refused, with a reason"),
						ReclickWaiter.FailureReason.IsEmpty());
					TestFalse(TEXT("re-clicking the attacked-with unit is refused, with a reason"),
						ReclickAttacker.FailureReason.IsEmpty());
					AddInfo(FString::Printf(
						TEXT("the shared trace phase 6 could not read a wait out of -- waiter: '%s' / "
						     "attacker: '%s'"),
						*ReclickWaiter.FailureReason, *ReclickAttacker.FailureReason));

					AddInfo(FString::Printf(
						TEXT("T-INT-05: unit %d waited and unit %d attacked %s on turn %d, after %d "
						     "advancing moves over %d turns"),
						WaiterId, UnitId, *Describe(Target), Bridge.Turn(), MovesPlayed, TurnsPlayed));

					bPinned = true;
					break;
					// ==============================================================
				}

				// ---- no target: advance, then look again ---------------------
				if (Attempt > 0 || Unit->bHasMoved)
				{
					break;
				}

				FIntPoint Dest;
				if (!PickAdvance(Query, Model, UnitId, Unit->Hex, Unit->Side, Dest, Error))
				{
					break;
				}

				const FStratSelectionOutcome MoveOutcome =
					Machine.HandleEvent(EStratSelectionEvent::HexPrimary, Dest, Model, Query);
				if (MoveOutcome.Command != EStratSelectionCommand::Move)
				{
					break;
				}

				FString MoveFailure;
				if (!TestTrue(
						*FString::Printf(TEXT("leg %d: the module's own listed move for unit %d is "
						                      "accepted"), Leg, UnitId),
						StratSubmitSelectionCommand(Bridge, MoveOutcome, MoveFailure)))
				{
					AddError(MoveFailure);
					return false;
				}
				Machine.NotifyCommandApplied(MoveOutcome);
				++MovesPlayed;

				if (!Refresh(Bridge, Machine, Model, Error))
				{
					AddError(Error);
					return false;
				}
				Unit = FindUnit(Model, UnitId);
				if (Unit == nullptr)
				{
					break;
				}
				ClosestContact = FMath::Min(ClosestContact,
					NearestEnemyDistance(Model, Unit->Hex, Unit->Side));
			}
		}

		if (bPinned)
		{
			break;
		}

		// ---- hand over --------------------------------------------------------
		FStratViewModel EndModel;
		if (!TestTrue(*FString::Printf(TEXT("leg %d: the model rebuilds before the hand-over"), Leg),
				Refresh(Bridge, Machine, EndModel, Error)))
		{
			AddError(Error);
			return false;
		}
		const FStratSelectionOutcome EndOutcome = Machine.HandleEvent(
			EStratSelectionEvent::EndTurn, FIntPoint::ZeroValue, EndModel, Query);
		FString EndFailure;
		if (!TestTrue(*FString::Printf(TEXT("leg %d: the end of turn is accepted"), Leg),
				StratSubmitSelectionCommand(Bridge, EndOutcome, EndFailure)))
		{
			AddError(EndFailure);
			return false;
		}
		Machine.NotifyCommandApplied(EndOutcome);
	}

	// A SKIP IS A FAILURE. Without contact the attack half asserted nothing, and a silent
	// green would be the shape this phase was blocked for twice.
	TestTrue(
		*FString::Printf(
			TEXT("T-INT-05: a wait and an accepted attack were both driven within %d turns -- the "
			     "closest the two sides came was %d hexes, over %d advancing moves"),
			kMaxTurns, ClosestContact, MovesPlayed),
		bPinned);

	return bPinned;
}

// ---------------------------------------------------------------------------
// T-INT-05 -- A WAIT WITH NOTHING SELECTED IS A NO-OP, AND THE GUARD IS WHAT MAKES THE
// `STRAT-WAIT spent` LINE'S PLACEMENT HONEST.
//
// THE ARM UNDER TEST is `StratSelectionMachine.cpp:156-160`: with `SelectedUnitId ==
// INDEX_NONE` the Wait case sets a failure reason and BREAKS OUT -- before `DoneUnits.Add`,
// and before the log line. So nothing is spent, nothing is marked, and a no-op wait cannot
// report a success. A guard that fell through would add `INDEX_NONE` to the done set and
// print a line claiming a unit had spent its turn.
//
// WHAT IS ASSERTED, and in this order of weight:
//   1. `Command == None`, the selection is still `INDEX_NONE`, and a reason is set. The
//      refusal is the machine's returned value.
//   2. NOTHING WAS ADDED TO `DoneUnits`, checked the way the screen would see it --
//      `DecorateViewModel` leaves every `bDone` false. That is the observable form of the
//      set, and it is the one T-INT-05 is about.
//   3. The rules state did not move, against `FStratBridge::StateHash()`'s own earlier
//      reading. A no-op that submitted something would fail here.
//   4. The `STRAT-WAIT spent` line is ABSENT. See this file's header block for why this is
//      not the self-witnessing shape: the same clause then drives a REAL wait through the
//      SAME live capture and requires exactly one line, so the silence in (4) is read only
//      after the capture has been shown able to speak.
//
// ON THE REFUSAL TEXT, SAID PLAINLY BECAUSE IT WAS ASKED. There is NO module-side accessor
// for it: it is a `TEXT(...)` literal built inline in `HandleEvent`, `FStratSelectionOutcome`
// exposes nothing but the composed `FailureReason`, and nothing in `FStratSelectionMachine`
// hands the string out. `StratSelectionMachine.cpp:158` reads
//
//     Outcome.FailureReason = TEXT("nothing is selected");
//
// and that line is quoted here in prose rather than copied into an assertion, on the same
// standing the scoreboard file records for UI copy: a clause that pinned the sentence would
// fail on a wording change that broke nothing, and would still pass if the guard were
// removed and a different arm produced some other sentence. What is asserted instead is
// structural and module-side on BOTH sides: the reason is non-empty, and it DIFFERS from the
// refusal the same machine produces for a unit that genuinely is finished. Both strings come
// out of the machine; this file writes neither. If the guard were deleted, (1)-(4) fail.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratWaitWithNothingSelectedIsANoOpTest,
	"Stratocracy.StratPlay.T-INT-05.WaitWithNothingSelectedIsANoOp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratWaitWithNothingSelectedIsANoOpTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSelectionWaitClauses;

	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratSelectionMachine Machine;
	FStratBridgeRulesQuery Query(&Bridge);

	FStratViewModel Model;
	if (!TestTrue(TEXT("the model builds"), Refresh(Bridge, Machine, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	// A fresh machine selects nothing. Asserted rather than assumed -- it is this clause's
	// entire precondition.
	if (!TestEqual(TEXT("a fresh machine has nothing selected"),
			Machine.GetSelectedUnitId(), static_cast<int32>(INDEX_NONE)))
	{
		return false;
	}

	const FString HashBefore = Bridge.StateHash();

	// ---- the no-op wait, under a live capture -------------------------------
	FStratWaitCapture Capture;

	const FStratSelectionOutcome NoOp =
		Machine.HandleEvent(EStratSelectionEvent::Wait, FIntPoint::ZeroValue, Model, Query);
	Capture.Settle();
	const int32 LinesAfterNoOp = Capture.Lines.Num();

	TestEqual(TEXT("T-INT-05: a Wait with nothing selected yields no command"),
		static_cast<int32>(NoOp.Command), static_cast<int32>(EStratSelectionCommand::None));
	TestEqual(TEXT("T-INT-05: and it names no unit"),
		NoOp.UnitId, static_cast<int32>(INDEX_NONE));
	TestEqual(TEXT("T-INT-05: and nothing became selected"),
		Machine.GetSelectedUnitId(), static_cast<int32>(INDEX_NONE));
	TestEqual(TEXT("the outcome reports the selection it left behind"),
		NoOp.SelectedUnitId, static_cast<int32>(INDEX_NONE));
	TestFalse(
		TEXT("T-INT-05: the machine REFUSES in its own words -- see this clause's header block "
		     "for why the sentence itself is not pinned (StratSelectionMachine.cpp:158)"),
		NoOp.FailureReason.IsEmpty());
	AddInfo(FString::Printf(TEXT("the machine's refusal: %s"), *NoOp.FailureReason));

	// ---- nothing was spent, seen the way the screen sees it ------------------
	for (const FStratUnitView& U : Model.Units)
	{
		TestFalse(
			*FString::Printf(TEXT("T-INT-05: unit %d was not added to the done set"), U.UnitId),
			Machine.IsDone(U.UnitId));
	}

	FStratViewModel Decorated;
	if (!TestTrue(TEXT("the model rebuilds and decorates after the no-op"),
			Refresh(Bridge, Machine, Decorated, Error)))
	{
		AddError(Error);
		return false;
	}
	for (const FStratUnitView& U : Decorated.Units)
	{
		TestFalse(
			*FString::Printf(
				TEXT("T-INT-05: unit %d's bDone is FALSE on the decorated model -- a no-op wait "
				     "spends nobody's turn"), U.UnitId),
			U.bDone);
	}

	TestEqual(TEXT("T-INT-05: and the rules state did not move"), Bridge.StateHash(), HashBefore);

	// ---- the positive control: the SAME capture, on a real wait --------------
	// Until this runs, "no line was emitted" is worth nothing: a capture that saw nothing
	// ever would report the same silence. This is what makes the count above readable.
	const FStratUnitView* Waiter = nullptr;
	for (const FStratUnitView& U : Decorated.Units)
	{
		if (U.Side == Decorated.Match.SideToMove)
		{
			Waiter = &U;
			break;
		}
	}
	if (!TestNotNull(TEXT("the side to move has a unit to wait with"), Waiter))
	{
		return false;
	}
	const int32     WaiterId  = Waiter->UnitId;
	const FIntPoint WaiterHex = Waiter->Hex;

	Machine.HandleEvent(EStratSelectionEvent::HexPrimary, WaiterHex, Decorated, Query);
	if (!TestEqual(TEXT("the unit selects"), Machine.GetSelectedUnitId(), WaiterId))
	{
		return false;
	}

	const FStratSelectionOutcome RealWait =
		Machine.HandleEvent(EStratSelectionEvent::Wait, FIntPoint::ZeroValue, Decorated, Query);
	Capture.Settle();
	const int32 LinesAfterRealWait = Capture.Lines.Num();

	TestTrue(TEXT("the real wait spends its unit"), Machine.IsDone(WaiterId));
	TestTrue(TEXT("and a real wait is not a refusal"), RealWait.FailureReason.IsEmpty());

	TestTrue(
		TEXT("the capture is live -- a REAL wait emitted a STRAT-WAIT line, so the count taken "
		     "on the no-op path is a reading and not a blind spot"),
		LinesAfterRealWait > LinesAfterNoOp);
	TestEqual(
		TEXT("T-INT-05: the no-op wait emitted NO STRAT-WAIT line -- the nothing-is-selected arm "
		     "breaks out before it (StratSelectionMachine.cpp:156-160), so a no-op cannot report a "
		     "success"),
		LinesAfterNoOp, 0);
	for (const FString& Line : Capture.Lines)
	{
		AddInfo(Line);
	}

	// ---- and the guard's refusal is its OWN, not the done-set arm's ----------
	// Both sentences below are produced by the machine on this same run. The comparison has a
	// module-side value on each side; this file writes neither string.
	const FStratSelectionOutcome Finished =
		Machine.HandleEvent(EStratSelectionEvent::HexPrimary, WaiterHex, Decorated, Query);
	TestFalse(TEXT("clicking the finished unit is refused, with a reason"),
		Finished.FailureReason.IsEmpty());
	TestNotEqual(
		TEXT("T-INT-05: the nothing-is-selected refusal is a DIFFERENT statement from the "
		     "already-finished one -- the guard has its own arm and did not fall through into "
		     "the done set"),
		NoOp.FailureReason, Finished.FailureReason);
	AddInfo(FString::Printf(TEXT("the done-set refusal, for contrast: %s"),
		*Finished.FailureReason));

	return true;
}
