// THE GATING FILE FOR PHASE 4. GDD §2.11.1, §4.9, §4.10, §4.11 row 10.
//
//   T-SAVE-05  a hot-seat sequence of CLICKS, applied through the selection machine and
//              `StratSubmitSelectionCommand`, re-applies to a fresh seed and reaches an
//              EQUAL canonical state hash; the number of accepted commands equals the
//              number of commands in the log; and the `STRAT-CMD accepted` line's fields
//              are the bridge's own values rather than the format string's.
//
// WHAT THIS FILE PINS, AND WHY THE RECORDING JOINT IS NOW INSIDE IT RATHER THAN BESIDE IT.
//
// The chain the criterion needs is: clicks -> outcomes -> submissions -> `RecordedLog()` ->
// replay -> equal hash. Phase 4 first shipped it as two clauses that met in the middle,
// with the JOINT -- that `StratSubmitSelectionCommand` calls a *recording* entry point at
// all -- pinned by neither. That split was not a convenience and it was not sound: this
// clause drove BOTH bridges through the same submission function, so any submission path,
// recording or not, yielded equal hashes; and it counted `STRAT-CMD accepted` lines that
// same function emitted, so the function was both subject and witness.
//
// TWO ENGINE-TYPED METHODS CLOSED IT, and they are why this file no longer has that hole:
//
//   `int32 RecordedCommandCount() const`      -- the bridge's own count of recorded
//                                                commands, in `int32`.
//   `FStratResult ReplayRecordedLogOnto(FStratBridge&) const` -- the bridge's own log,
//                                                replayed onto a fresh bridge, with no
//                                                `strat::SaveCommand` in the signature.
//
// So the count assertion below is against THE BRIDGE'S RECORDING, not against a line the
// submission path printed, and the replay is driven by THE BRIDGE'S OWN LOG, not by
// re-running the same function over the same outcomes. Route any arm of
// `StratSelectionMachine.cpp`'s switch through a non-recording apply path and this clause
// FAILS TWICE OVER: the count comes back short, and `ReplayRecordedLogOnto` either refuses
// (an empty log is a refusal, deliberately -- see `T-SAVE-05.ReplayRecordedLogOntoRefuses`)
// or replays fewer commands and lands on a different hash.
//
// `StratBridge/Tests/StratSelectionFacadeParity.cpp` still carries the typed half --
// `T-SAVE-05.RecordedLogReplaysToEqualHash`, that `RecordedLog()` is exactly the accepted
// set -- and it is still worth having, because it is stated in the format's own type. It is
// no longer load-bearing for the joint.
//
// NO `strat::` TYPE IS NAMED AND NO `strat::` FUNCTION IS CALLED ANYWHERE BELOW.
//
// WHERE THE EXPECTATIONS COME FROM.
//   - The count expectation is `FStratBridge::RecordedCommandCount()`. This file counts
//     what it submitted and compares against what the bridge recorded; the log-line count
//     is still asserted, but as a THIRD reading beside those two rather than as the
//     subject's own testimony about itself.
//   - The replay expectation is `FStratBridge::StateHash()` on the second bridge, compared
//     against the same method on the first, after `ReplayRecordedLogOnto` drove it. This
//     file predicts no hash and computes none.
//   - Every hex clicked is an element of `FStratBridgeRulesQuery::ReachableHexes`'s own
//     answer, chosen at run time. There is not one coordinate literal in this file.
//   - The `STRAT-CMD` clause compares the CAPTURED LINE against `Bridge.Turn()`,
//     `Bridge.SideToMove()` read before submission and `Bridge.StateHash()` read after --
//     module-side values, not the format string. A clause that read the same literal the
//     code writes would pin nothing, which is why the line is captured off `GLog` rather
//     than reconstructed. The chain to §4.10's log entry is closed by
//     `T-SAVE-06.TurnAndSideAreReadBeforeStamp`, which pins those same two readings
//     against the `{turn, side}` the bridge stamps onto the recorded command.
//
// THE SEQUENCE IS LONG ENOUGH AND VARIED ENOUGH TO BE WORTH SOMETHING, which is a
// requirement and not a courtesy: it crosses two turn boundaries so BOTH SIDES move, it
// contains a Wait (which produces no command at all and must therefore leave the log
// alone), and it contains a REFUSED submission -- without which "accepted count equals log
// length" cannot fail and the clause would be decoration.

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

// Included from a .cpp and never from a UHT-parsed header.
#include "StratBridge.h"

namespace StratHotSeatReplayParity
{
	// The same harness pin every parity file in this tree carries.
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
	 * Build -> decorate, which is the phase-4 path `StratMatchSubsystem.h` describes and the
	 * order the running game uses. The viewing side FOLLOWS THE SIDE TO MOVE, because that
	 * is what a hot-seat hand-over is; the machine reads the active side off
	 * `Model.Match.SideToMove` regardless, and never off `ViewingSide`.
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

	/** The first unit of the side to move that the machine has not marked DONE. */
	static const FStratUnitView* NextActiveUnit(const FStratViewModel& Model,
	                                            const FStratSelectionMachine& Machine,
	                                            int32 SkipUnitId)
	{
		for (const FStratUnitView& U : Model.Units)
		{
			if (U.Side == Model.Match.SideToMove && !Machine.IsDone(U.UnitId) &&
				U.UnitId != SkipUnitId && !U.bHasMoved)
			{
				return &U;
			}
		}
		return nullptr;
	}

	/** An element of the module's own reachable set: not the unit's hex, and unoccupied. */
	static bool PickDestination(const IStratRulesQuery& Query, const FStratViewModel& Model,
	                            int32 UnitId, FIntPoint Origin, FIntPoint& OutHex,
	                            FString& OutError)
	{
		TArray<FIntPoint> Reach;
		if (!Query.ReachableHexes(UnitId, Reach, OutError))
		{
			return false;
		}
		for (const FIntPoint& H : Reach)
		{
			if (H != Origin && FindUnitAtHex(Model, H) == nullptr)
			{
				OutHex = H;
				return true;
			}
		}
		OutError = FString::Printf(
			TEXT("unit %d has no unoccupied reachable hex other than its own"), UnitId);
		return false;
	}

	/**
	 * Axial hex distance. A FIXTURE DEVICE AND NEVER AN EXPECTATION.
	 *
	 * It exists for one job in this file: deciding WHICH element of the module's own
	 * reachable set the scripted advance below clicks on, so that the two sides meet and a
	 * legal attack comes into existence. No clause compares anything against it, no hex is
	 * called reachable or attackable because of it, and a wrong answer here makes the
	 * advance wander rather than making any assertion easier to pass. The same standing
	 * `StratSelectionMachineParity.cpp` records for its `FoilHexDistance`, which is the
	 * function this one is a copy of and is deliberately not shared with -- that one is a
	 * FOIL the real set must differ from, and merging the two would put a "distance"
	 * helper in general circulation in the module that is supposed to have none.
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
	 * THE SET IS THE MODULE'S; ONLY THE CHOICE WITHIN IT IS THIS FILE'S. That is the same
	 * standing `PickDestination` above has -- it takes the first element, this one takes
	 * the element nearest an enemy -- and the reason for the difference is that "the first"
	 * never brings the two sides into contact, so no click sequence a test could script
	 * would ever reach the machine's Attack branch.
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

		bool      bFound = false;
		int32     BestDistance = MAX_int32;
		FIntPoint Best = Origin;
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
	 * Captures the `STRAT-CMD` lines this module emits.
	 *
	 * OFF `GLog`, NOT OFF THE FORMAT STRING. The point of the shape clause is that the line
	 * a phase-6 gate will grep is the line this code actually produced; a test that
	 * re-formatted the same literal would agree with any mistake in it. `Serialize`
	 * receives the formatted message, so what lands in `Lines` is what a log file would
	 * hold.
	 *
	 * The filter is the string a grep would use. Both the accepted and the refused shapes
	 * begin with it, deliberately -- see below, where the two are told apart on the SECOND
	 * token, which is exactly how a counting gate must do it.
	 *
	 * THAT FILTER AND `CountStartingWith` ARE BOTH CASE-INSENSITIVE, AND UNLIKE THE
	 * `STRAT-AI` CAPTURES THEY ARE LEFT THAT WAY KNOWINGLY -- while the grep-contract
	 * ASSERTION they feed is, as of this pass, `ESearchCase::CaseSensitive`.
	 * `FString::StartsWith` defaults to `ESearchCase::IgnoreCase` in UE 5.8, and this project
	 * has already shipped a clause that could not fail because of exactly that default -- so
	 * neither the tightening nor the two omissions may be a shrug.
	 *
	 * THE LINE DRAWN HERE IS BETWEEN INSTRUMENTS AND CLAIMS. The filter and the counter are
	 * INSTRUMENTS: their job is to collect candidate lines for inspection, and they stay
	 * permissive on purpose. `IsGrepContractAcceptedLine` is the CLAIM, and it is strict.
	 * The file is therefore not half-tightened; it is tightened exactly where an assertion
	 * is made about a spelling, and left loose exactly where a line is being gathered.
	 *
	 * FIRST, WHAT CASE CANNOT REACH AT ALL. The prefixes that occur anywhere in this tree --
	 * `STRAT-CMD`, `STRAT-WAIT`, `STRAT-AI`, `STRAT-COMBAT`, and `STRAT-PROBE` (a phase-6
	 * debug probe named only inside a comment in `StratPlayerController.cpp`, with no live
	 * emitter) -- differ from one another in LETTERS, never in case. So does the second
	 * token this clause turns on, `accepted` versus `refused`. No line belonging to another
	 * prefix, and no refusal, can be admitted here by case-folding alone.
	 *
	 * SECOND, THE COUNTING ASSERTIONS ARE FAIL-SAFE BY CONSTRUCTION. `AcceptedLines` and
	 * `RefusedLines` are compared with `TestEqual` against module-side numbers
	 * (`AcceptedCommands.Num()`, `Refusals`), so an over-permissive prefix can only ADD a
	 * line and drive an exact-count comparison RED. A looser filter cannot make those pass
	 * when they should fail. THAT ARGUMENT SURVIVES THIS PASS IN ITS ORIGINAL SHAPE PRECISELY
	 * BECAUSE THE FILTER WAS NOT TIGHTENED. Had it been, the direction would have flipped --
	 * a stricter filter can only DROP a line -- which is still red against an exact count,
	 * but red in a different clause and with a different message. The counts are fail-safe
	 * either way; the reason to keep the loose direction is the next paragraph.
	 *
	 * THIRD, AND THIS IS WHY THE INSTRUMENTS STAY LOOSE: NOT EVERY ASSERTION HERE IS A COUNT.
	 * The shape clause ends with a `TestTrue` over `IsGrepContractAcceptedLine` -- "the line
	 * a phase-6 grep looks for" -- and that one is NOT fail-safe by construction, because it
	 * is the only assertion in this file whose subject is a SPELLING rather than an arity. It
	 * is therefore the assertion that must carry `ESearchCase::CaseSensitive`, and it now
	 * does. A hypothetical emitter spelling the prefix `strat-cmd accepted` is still ADMITTED
	 * by the filter, still counted as one line by the `TestEqual(Capture.Lines.Num(), 1)`
	 * above it, and still balances every count in this file -- and is then REJECTED by the
	 * `TestTrue`, which is the one clause named after the property that actually broke.
	 *
	 * TIGHTENING THE FILTER WOULD HAVE MADE THAT SAME EMITTER INVISIBLE INSTEAD:
	 * `Lines.Num()` would read 0, the arity clause would go red first and return early, and
	 * the grep contract would be reported as "no line at all" rather than as "the line is
	 * spelled wrong". The instrument is deliberately able to SEE the defect that the claim is
	 * deliberately unable to ACCEPT. `CountStartingWith` is left loose for the same reason:
	 * tightening it would move the same failure into a count and duplicate a signal the
	 * `TestTrue` already carries, under a message that names arithmetic rather than spelling.
	 *
	 * AND THE TIGHTENED CLAIM IS SHOWN ABLE TO FAIL, which is the whole reason the tightening
	 * was worth a code change at all. `T-SAVE-05.GrepContractRejectsACaseVariant` constructs
	 * this device directly, hands it a correct line and that same line lower-cased, and
	 * asserts that the FILTER admits both while `IsGrepContractAcceptedLine` accepts the
	 * first and refuses the second. It also asserts, against the bare `StartsWith` overload,
	 * that the lower-case line WOULD have passed before this pass -- so the clause names the
	 * defect it closed instead of asserting an absence. A `CaseSensitive` predicate that
	 * nothing ever exercises is the same unfalsifiable claim with better-looking source.
	 *
	 * THE EXTERNAL MEASUREMENT THAT USED TO CARRY THIS IS NOW ONLY BACKGROUND: a case-variant
	 * census over all of `Source/` (combat-outcome phase 5) found NO lower-case spelling of
	 * any log prefix anywhere in the tree -- the only lower-case occurrences that exist are
	 * the illustrative ones inside this comment, its counterpart in
	 * `StratSelectionWaitClauses.cpp`, and the synthetic line the falsifiability clause
	 * builds at run time. A census is a fact about today's tree and cannot survive a future
	 * case-ONLY change to the emitter, which is exactly the phase-6 grep contract this
	 * `TestTrue` exists to pin. That is what is now closed, and it is closed in code rather
	 * than in a comment.
	 *
	 * THE RESIDUAL, STATED PLAINLY. The capture filter and `CountStartingWith` remain
	 * `IgnoreCase`, so a case-only change to the `STRAT-CMD accepted` emitter is caught by
	 * exactly ONE assertion in this file -- the shape clause's `TestTrue` -- and by no count
	 * anywhere. That is the intended design and not an oversight; see THIRD above.
	 *
	 * THE `refused` HALF USED TO BE UNCOVERED ENTIRELY AND IS NOT ANY MORE. This block
	 * previously recorded that no clause asserted the refusal's spelling, so `RefusedLines`
	 * counted a lower-case `strat-cmd refused` right through and every count still balanced.
	 * `IsGrepContractRefusedLine` closed that, and it is exercised in TWO places rather than
	 * one -- which is where the two halves now differ, deliberately:
	 *   - `T-SAVE-05.StratCmdRefusedLineShape` asserts it over a REAL captured refusal, the
	 *     counterpart of what `StratCmdLineShape` does for the accepted line;
	 *   - the gating clause asserts a STRICT COUNT (`CountMatching`) beside the loose
	 *     `RefusedLines`, which the accepted side has no counterpart for.
	 * The strict count does not tighten the instrument -- `CountStartingWith` is untouched and
	 * still `IgnoreCase` -- it is a second CLAIM over the same loosely-gathered lines. So the
	 * see/accept asymmetry THIRD argues for is preserved on both halves: a case-mangled line is
	 * still ADMITTED and still counted by the instrument, and is then refused by the claim.
	 * `T-SAVE-05.GrepContractRejectsARefusedCaseVariant` asserts exactly that divergence, on
	 * the same two lines, so the pairing is pinned rather than asserted here in prose.
	 *
	 * AND THE OBVIOUS COUNTER-ARGUMENT TO THAT RESIDUAL DOES NOT HOLD, which is worth the
	 * lines because it is the first thing a careful reader will reach for. The gating clause
	 * declares `AddExpectedMessagePlain(TEXT("STRAT-CMD refused"), ...)`, which looks like a
	 * second net under the refusal spelling. IT IS NOT ONE: the expected-message machinery is
	 * case-insensitive by the ENGINE'S OWN CONSTRUCTION, on BOTH of its paths, so it cannot be
	 * made to care about case at all. Read out of UE 5.8's `AutomationTest.h` rather than
	 * taken on report:
	 *   - `FAutomationExpectedMessage::Matches`, non-regex path, is
	 *     `Message.Contains(MessagePatternString) && (!IsExactCompareType() || Message.Len()
	 *     == MessagePatternString.Len())`. `FString::Contains` defaults to
	 *     `ESearchCase::IgnoreCase` -- and note that even the `Exact` compare type only adds a
	 *     LENGTH equality, which a case variant satisfies, so `Exact` is case-blind too.
	 *   - the regex path is no better: BOTH `FAutomationExpectedMessage` constructors build
	 *     their pattern as `FRegexPattern(..., ERegexPatternFlags::CaseInsensitive)`,
	 *     HARDCODED rather than taken as a parameter (`ERegexPatternFlags::CaseInsensitive =
	 *     (1 << 0)`, `Internationalization/Regex.h`).
	 * SO THE CLAIM, AT EXACTLY THE WIDTH IT WAS MEASURED AT: neither
	 * `FAutomationExpectedMessage` constructor EXPOSES case sensitivity, and the PLAIN STRING
	 * this file passes folds case on both paths. That covers the call site completely --
	 * `AddExpectedMessagePlain(TEXT("STRAT-CMD refused"), ...)` passes a bare literal with no
	 * pattern syntax in it, so whichever path it routes to, it folds case. The declaration is
	 * a VERBOSITY gate, not a spelling gate, and the residual above stands.
	 *
	 * AN EARLIER DRAFT OF THIS BLOCK SAID INSTEAD that "there is NO way to spell a
	 * case-sensitive expected message in this engine version", and that is left recorded here
	 * because the way it was wrong is more useful than the correction. It was not wrong by a
	 * detail. IT CLAIMED SOMETHING THE MEASUREMENT COULD NOT REACH: what was measured is what
	 * the two constructors take as PARAMETERS, and what was written is a universal over every
	 * possible PATTERN. Those are different propositions, and no amount of re-reading
	 * `AutomationTest.h` could have settled the second one.
	 *
	 * THE CONCRETE COUNTER-ROUTE OFFERED WAS AN INLINE MODE MODIFIER -- `(?-i)` at the head of
	 * a pattern on the regex path, which Perl-family engines honour regardless of the flags
	 * the pattern was compiled with. I could NOT settle that from this tree and I am not going
	 * to assert it in either direction: `FRegexPattern` is ICU-backed
	 * (`ICURegex.cpp`'s `GetICURegexFlags` maps `ERegexPatternFlags::CaseInsensitive` to
	 * `UREGEX_CASE_INSENSITIVE`, handed to `icu::RegexPattern::compile`), and the bundled ICU
	 * at `ThirdParty/ICU/icu4c-64_1/` ships INCLUDE HEADERS AND PREBUILT LIBS ONLY -- there is
	 * no `regexcmp.cpp` to read, and `uregex.h` / `regex.h` document the API rather than the
	 * pattern syntax. Verifying it would take running a pattern, which is a clause, which is
	 * not what this pass is.
	 *
	 * AND THE RESIDUAL DOES NOT NEED IT SETTLED, which is the point. The narrow claim above is
	 * true whether or not `(?-i)` works, because this file passes no modifier. A sentence that
	 * needs an unresolved question answered was claiming more than the argument needed --
	 * which is the same defect, one layer up, and the reason the wide version should never
	 * have been written.
	 *
	 * UNBUFFERED, AND THAT ONE OVERRIDE IS WHAT BOUNDS THE WINDOW. MEASURED, 2026-08-14:
	 * without it this clause failed 1 run in 4 on byte-identical code -- "one accepted command
	 * emits exactly one line" read 4, the extra three being `STRAT-CMD accepted kind=Move`
	 * lines this test's OWN advance loop had emitted BEFORE the device was constructed. The
	 * two preserved logs settle what happened: the T-UI-01 block is byte-for-byte identical in
	 * the passing and the failing run, so nothing was emitted twice -- the three lines were
	 * DELIVERED LATE.
	 *
	 * The route is `FOutputDeviceRedirector`. A log call that cannot take the primary-thread
	 * fast path is pushed onto `BufferedItems` (OutputDeviceRedirector.cpp:937) and drained
	 * later by `FlushBufferedItems`, which broadcasts to whichever devices sit in
	 * `BufferedOutputDevices` AT DRAIN TIME (:553) -- not to the ones that were registered when
	 * the line was emitted. A device that registers mid-stream therefore inherits the queue's
	 * tail. Nothing in this file can influence that; it is a race with the redirector's own
	 * thread, which is why the rate was 1-in-4 and why the clause passed in isolation.
	 *
	 * `CanBeUsedOnMultipleThreads() == true` moves the device into `UnbufferedOutputDevices`
	 * (:440-447), which is fed ONLY by the synchronous broadcast at :905, inside the emitting
	 * `UE_LOG` call itself. `FlushBufferedItems` never touches that list. So the set of lines
	 * this device can see is exactly the set whose `UE_LOG` executed between `AddOutputDevice`
	 * and `RemoveOutputDevice` -- the window is the object's lifetime, by construction rather
	 * than by timing.
	 *
	 * THIS IS THE ENGINE'S OWN IDIOM FOR A WINDOWED CAPTURE, not an invention here:
	 * `FAutomationTestOutputDevice` and `FAutomationTestMessageFilter` both override it, with
	 * the comment "Make it unbuffered by returning true" (AutomationTest.h:1345, :1396). They
	 * are registered and unregistered around a single test, exactly as this device is.
	 *
	 * The override advertises that the device needs no external locking, and the engine takes
	 * that literally -- every thread's log lines now arrive here directly. `Lines` is therefore
	 * mutated under `Mutex`. Reading `Lines` directly is game-thread-only and every call site
	 * below is on the game thread.
	 */
	struct FStratCmdCapture final : public FOutputDevice
	{
		TArray<FString> Lines;

		FStratCmdCapture()
		{
			if (GLog != nullptr)
			{
				GLog->AddOutputDevice(this);
			}
		}

		virtual ~FStratCmdCapture()
		{
			if (GLog != nullptr)
			{
				GLog->RemoveOutputDevice(this);
			}
		}

		/** See the block above. Removing this line restores the 1-in-4 flake. */
		virtual bool CanBeUsedOnMultipleThreads() const override { return true; }

		virtual void Serialize(const TCHAR* Message, ELogVerbosity::Type /*Verbosity*/,
		                       const FName& /*Category*/) override
		{
			const FString Line(Message);
			if (Line.StartsWith(TEXT("STRAT-CMD")))
			{
				FScopeLock Lock(&Mutex);
				Lines.Add(Line);
			}
		}

		/**
		 * Kept, and no longer load-bearing. An unbuffered device has already been handed every
		 * line by the time the emitting `UE_LOG` returns, so there is nothing of ours left to
		 * settle; this now only pushes the OTHER devices' buffers, which keeps a failure
		 * message's surrounding log readable. The counts below do not depend on it.
		 */
		void Settle()
		{
			if (GLog != nullptr)
			{
				GLog->Flush();
			}
		}

		int32 CountStartingWith(const TCHAR* Prefix) const
		{
			int32 Count = 0;
			for (const FString& Line : Lines)
			{
				if (Line.StartsWith(Prefix))
				{
					++Count;
				}
			}
			return Count;
		}

	private:
		FCriticalSection Mutex;
	};

	/**
	 * Splits `key=value key=value ...` while KEEPING THE ORDER, because the field order is
	 * part of what the line promises: StratSelectionMachine.h calls the format load-bearing
	 * and says it is "one `UE_LOG` with a fixed field order and no conditional fields".
	 */
	static void ParseFields(const FString& Line, TArray<FString>& OutKeys, TArray<FString>& OutValues)
	{
		OutKeys.Reset();
		OutValues.Reset();

		TArray<FString> Tokens;
		Line.ParseIntoArray(Tokens, TEXT(" "), /*InCullEmpty*/ true);
		for (const FString& Token : Tokens)
		{
			FString Key;
			FString Value;
			if (Token.Split(TEXT("="), &Key, &Value))
			{
				OutKeys.Add(Key);
				OutValues.Add(Value);
			}
		}
	}

	/**
	 * THE PHASE-6 GREP CONTRACT, AS A PREDICATE. `ESearchCase::CaseSensitive`, and that is
	 * the point of the function existing rather than the comparison being written inline.
	 *
	 * `FString::StartsWith` defaults to `ESearchCase::IgnoreCase` in UE 5.8, so the inline
	 * form this replaces admitted `strat-cmd accepted ` -- a spelling a real `grep` would
	 * miss, on the one assertion in this file that exists specifically to pin what a real
	 * grep will find. See the block on `FStratCmdCapture` for why the capture filter and
	 * `CountStartingWith` are deliberately NOT tightened alongside it.
	 *
	 * IT IS A NAMED FUNCTION SO THAT THE CLAUSE THAT ASSERTS THE CONTRACT AND THE CLAUSE THAT
	 * PROVES THAT ASSERTION CAN FAIL ARE ASKING THE SAME CODE. Spelling the predicate twice
	 * would let `T-SAVE-05.GrepContractRejectsACaseVariant` go on passing over its own
	 * private copy after somebody loosened the shipped one, which is the exact shape of
	 * unfalsifiability this pass was opened to remove.
	 */
	static bool IsGrepContractAcceptedLine(const FString& Line)
	{
		return Line.StartsWith(TEXT("STRAT-CMD accepted "), ESearchCase::CaseSensitive);
	}

	/**
	 * THE SAME CONTRACT FOR THE OTHER HALF OF THE LINE PAIR. `ESearchCase::CaseSensitive`,
	 * shared for the same reason, and NOT a copy of the accepted predicate with a word
	 * swapped -- the two lines are not parallel and this file should not pretend they are.
	 *
	 * WHAT IS THE SAME: both begin `STRAT-CMD `, both are told apart on the SECOND TOKEN, and
	 * both end that token with a trailing space so the predicate cannot be satisfied by a
	 * longer word starting with the same letters.
	 *
	 * WHAT DIFFERS, MEASURED OFF THE EMITTER RATHER THAN ASSUMED. The two `UE_LOG` calls in
	 * `StratSubmitSelectionCommand` are, VERBATIM AND WITH NO PADDING INSIDE THE BACKTICKS --
	 * every space between the delimiters is a space the emitter really writes, and the columns
	 * below are aligned by padding OUTSIDE the closing backtick instead:
	 *     `STRAT-CMD accepted kind=%s unit=%d hex=%d,%d turn=%d side=%d hash=%s`     (Log)
	 *     `STRAT-CMD refused kind=%s unit=%d hex=%d,%d turn=%d side=%d reason=%s`    (Warning)
	 * ONE space after `accepted` and ONE after `refused`, which is what the two predicates'
	 * trailing spaces are matching. An earlier draft of this block padded the refusal to two
	 * spaces to line the columns up, which put layout inside a quotation whose entire authority
	 * is that it is verbatim: a reader checking the single-space predicate against it would have
	 * found the two disagree with no way to tell which was wrong. The predicate was always
	 * right. Alignment now happens where it cannot be mistaken for content.
	 *
	 * -- the sixth field is `reason=` and not `hash=`, and its value is the RULES MODULE'S
	 * PROSE, which contains spaces and is of no fixed length. So this predicate deliberately
	 * pins the PREFIX ONLY and no clause below tries to `ParseFields` a refusal into exactly
	 * six values, which the accepted side can do and this side cannot. A predicate written by
	 * analogy would have claimed a shape the emitter does not have.
	 *
	 * THE VERBOSITY DIFFERENCE IS WHY THIS PREDICATE IS OWED AT ALL. Because refusals are
	 * `Warning`, every clause that provokes one declares
	 * `AddExpectedMessagePlain(TEXT("STRAT-CMD refused"), ...)`, and that declaration LOOKS
	 * like a spelling gate. It is not one -- see the block on `FStratCmdCapture` for the
	 * read out of `AutomationTest.h`: `Matches` folds case on the plain path, `Exact` adds
	 * only a length equality that a case variant satisfies, and both constructors hardcode
	 * `ERegexPatternFlags::CaseInsensitive` on the regex path. It gates VERBOSITY. Until this
	 * predicate shipped, nothing in the tree asserted what a `refused` line SAYS.
	 *
	 * FILE-LOCAL AND NAMED, for the reason `IsGrepContractAcceptedLine` is: the clause that
	 * ASSERTS the contract and the clause that PROVES the assertion can fail must ask the
	 * same code, or the proof goes on passing over a private copy after the shipped one is
	 * loosened.
	 */
	static bool IsGrepContractRefusedLine(const FString& Line)
	{
		return Line.StartsWith(TEXT("STRAT-CMD refused "), ESearchCase::CaseSensitive);
	}

	/**
	 * How many gathered lines satisfy a STRICT predicate. The counterpart of
	 * `FStratCmdCapture::CountStartingWith`, and deliberately NOT a replacement for it: the
	 * counter on the device is an INSTRUMENT and stays `IgnoreCase`, so a case-mangled line is
	 * still SEEN; this is a CLAIM over what was seen, so it is strict. Keeping them separate is
	 * what lets a clause assert both "the instrument admitted it" and "the contract refuses it"
	 * about the same line.
	 */
	static int32 CountMatching(const TArray<FString>& Lines, bool (*Predicate)(const FString&))
	{
		int32 Count = 0;
		for (const FString& Line : Lines)
		{
			if (Predicate(Line))
			{
				++Count;
			}
		}
		return Count;
	}
}

// ---------------------------------------------------------------------------
// T-SAVE-05 -- THE GATING CLAUSE. A scripted hot-seat click sequence, re-applied to a
// fresh seed, reaches an equal state hash.
//
// WHAT MAKES THIS A REPLAY TEST AND NOT A "RUN IT TWICE" TEST. The first bridge is driven
// by CLICKS -- `HandleEvent` decides, `StratSubmitSelectionCommand` applies,
// `NotifyCommandApplied` records the acceptance, and the model is rebuilt between every
// step. The second bridge is driven by THE FIRST BRIDGE'S OWN RECORDED LOG, through
// `ReplayRecordedLogOnto`, with no machine, no model, no clicks, and NO SECOND CALL INTO
// THE SUBMISSION FUNCTION. That last part is the difference between this clause and the
// one it replaces: driving both bridges through the same function made equal hashes
// inevitable whether or not anything was ever recorded. If any part of the state depended
// on something the machine or the model was holding -- a cached flag, a selection, a done
// set -- the two hashes would differ; and if the submission path stopped recording, the
// count assertion fails and the replay refuses outright. That is the property §4.10 needs
// and the property phase 6's playtest gate will lean on.
//
// THE REFUSED COMMAND IS DELIBERATE AND ITS SHAPE IS DECLARED. It is a hand-built Move for
// a unit belonging to the side that is NOT to move -- hand-built because the machine will
// never emit an illegal command, which is itself the T-UI-02 property. Handing it to
// `StratSubmitSelectionCommand` is exactly what that function is for, and it must come
// back false, log a refusal rather than an acceptance, leave the hash alone, and NOT be
// carried into the replay list.
//
// THE WAIT IS THERE FOR THE SAME REASON IN THE OPPOSITE DIRECTION: it is a real player
// action that produces NO command, so a submission path that logged intent rather than
// acceptance would show up as an extra accepted line with no command behind it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratHotSeatReplayParityTest,
	"Stratocracy.StratPlay.T-SAVE-05.HotSeatReplayParity",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratHotSeatReplayParityTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratHotSeatReplayParity;

	AddExpectedMessagePlain(TEXT("STRAT-CMD refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	const FString SeedHash = Bridge.StateHash();

	FStratSelectionMachine Machine;
	FStratBridgeRulesQuery Query(&Bridge);

	// The accepted commands, in order. AN ENGINE TYPE -- `FStratSelectionOutcome` -- which
	// is why this list can exist in this module at all.
	TArray<FStratSelectionOutcome> AcceptedCommands;

	TSet<int32> SidesThatMoved;
	int32       Refusals = 0;
	int32       Waits    = 0;

	FStratCmdCapture Capture;

	for (int32 Leg = 0; Leg < 3; ++Leg)
	{
		FStratViewModel Model;
		if (!TestTrue(*FString::Printf(TEXT("leg %d: the model builds"), Leg),
				Refresh(Bridge, Machine, Model, Error)))
		{
			AddError(Error);
			return false;
		}

		const int32 ActiveSide = Model.Match.SideToMove;
		SidesThatMoved.Add(ActiveSide);

		// ---- select and move ------------------------------------------------
		const FStratUnitView* const Mover = NextActiveUnit(Model, Machine, INDEX_NONE);
		if (!TestNotNull(*FString::Printf(TEXT("leg %d: side %d has an unspent unit"), Leg, ActiveSide),
				Mover))
		{
			return false;
		}
		const int32     MoverId  = Mover->UnitId;
		const FIntPoint MoverHex = Mover->Hex;

		Machine.HandleEvent(EStratSelectionEvent::HexPrimary, MoverHex, Model, Query);
		if (!TestEqual(*FString::Printf(TEXT("leg %d: unit %d selects"), Leg, MoverId),
				Machine.GetSelectedUnitId(), MoverId))
		{
			return false;
		}

		FIntPoint Dest;
		if (!TestTrue(*FString::Printf(TEXT("leg %d: the module lists a destination for unit %d"),
				Leg, MoverId),
				PickDestination(Query, Model, MoverId, MoverHex, Dest, Error)))
		{
			AddError(Error);
			return false;
		}

		const FStratSelectionOutcome MoveOutcome =
			Machine.HandleEvent(EStratSelectionEvent::HexPrimary, Dest, Model, Query);
		if (!TestEqual(*FString::Printf(TEXT("leg %d: the click on %s is a Move"), Leg, *Describe(Dest)),
				static_cast<int32>(MoveOutcome.Command),
				static_cast<int32>(EStratSelectionCommand::Move)))
		{
			AddError(MoveOutcome.FailureReason);
			return false;
		}

		FString MoveFailure;
		if (!TestTrue(*FString::Printf(TEXT("leg %d: the rules module accepts the listed move"), Leg),
				StratSubmitSelectionCommand(Bridge, MoveOutcome, MoveFailure)))
		{
			AddError(MoveFailure);
			return false;
		}
		Machine.NotifyCommandApplied(MoveOutcome);
		AcceptedCommands.Add(MoveOutcome);
		AddInfo(FString::Printf(TEXT("leg %d: {turn %d, side %d} unit %d -> %s"),
			Leg, Bridge.Turn(), ActiveSide, MoverId, *Describe(Dest)));

		// ---- leg 0 only: a Wait, and a refused submission --------------------
		if (Leg == 0)
		{
			if (!TestTrue(TEXT("the model rebuilds after the move"),
					Refresh(Bridge, Machine, Model, Error)))
			{
				AddError(Error);
				return false;
			}

			// A WAIT. No command, and the accepted-line count must not move.
			const FStratUnitView* const Waiter = NextActiveUnit(Model, Machine, MoverId);
			if (TestNotNull(TEXT("the active side has a second unspent unit to wait with"), Waiter))
			{
				const int32 WaiterId = Waiter->UnitId;
				Machine.HandleEvent(EStratSelectionEvent::HexPrimary, Waiter->Hex, Model, Query);
				const FStratSelectionOutcome Waited = Machine.HandleEvent(
					EStratSelectionEvent::Wait, FIntPoint::ZeroValue, Model, Query);

				TestEqual(TEXT("a Wait produces no command"),
					static_cast<int32>(Waited.Command),
					static_cast<int32>(EStratSelectionCommand::None));
				TestTrue(TEXT("and the unit is DONE"), Machine.IsDone(WaiterId));

				const int32 CountBeforeWait = Bridge.RecordedCommandCount();

				FString WaitFailure;
				TestTrue(TEXT("submitting a None outcome succeeds having done nothing"),
					StratSubmitSelectionCommand(Bridge, Waited, WaitFailure));
				TestEqual(
					TEXT("T-SAVE-05: and records nothing -- a Wait is a player action with no "
					     "command behind it, and the bridge's own count is what says so"),
					Bridge.RecordedCommandCount(), CountBeforeWait);
				++Waits;
			}

			// A REFUSED SUBMISSION. Hand-built, because the machine will never emit one --
			// see the clause header.
			const FStratUnitView* Enemy = nullptr;
			for (const FStratUnitView& U : Model.Units)
			{
				if (U.Side != ActiveSide)
				{
					Enemy = &U;
					break;
				}
			}
			if (TestNotNull(TEXT("the other side has a unit to build a refused command around"), Enemy))
			{
				FStratSelectionOutcome Illegal;
				Illegal.Command = EStratSelectionCommand::Move;
				Illegal.UnitId  = Enemy->UnitId;
				Illegal.Hex     = Enemy->Hex;

				const FString HashBefore  = Bridge.StateHash();
				const int32   CountBefore = Bridge.RecordedCommandCount();

				FString RefusalReason;
				const bool bApplied = StratSubmitSelectionCommand(Bridge, Illegal, RefusalReason);

				TestFalse(TEXT("T-SAVE-05: a command for the side that is not to move is refused"),
					bApplied);
				TestFalse(TEXT("T-SAVE-05: the refusal carries the rules module's own reason"),
					RefusalReason.IsEmpty());
				TestEqual(TEXT("T-SAVE-05: and changes nothing"), Bridge.StateHash(), HashBefore);
				TestEqual(TEXT("T-SAVE-05: and is NOT recorded -- the bridge's own count, not a log line"),
					Bridge.RecordedCommandCount(), CountBefore);
				++Refusals;
				AddInfo(FString::Printf(TEXT("refused: %s"), *RefusalReason));

				// It is NOT added to AcceptedCommands. That omission is the thing the count
				// clause below can catch if the submission path ever starts lying.
			}
		}

		// ---- end the turn, twice out of three legs ---------------------------
		if (Leg < 2)
		{
			FStratViewModel EndModel;
			if (!TestTrue(TEXT("the model rebuilds before the hand-over"),
					Refresh(Bridge, Machine, EndModel, Error)))
			{
				AddError(Error);
				return false;
			}

			const FStratSelectionOutcome EndOutcome = Machine.HandleEvent(
				EStratSelectionEvent::EndTurn, FIntPoint::ZeroValue, EndModel, Query);
			if (!TestEqual(*FString::Printf(TEXT("leg %d: the EndTurn event asks for an EndTurn"), Leg),
					static_cast<int32>(EndOutcome.Command),
					static_cast<int32>(EStratSelectionCommand::EndTurn)))
			{
				AddError(EndOutcome.FailureReason);
				return false;
			}
			TestEqual(TEXT("an EndTurn names no unit"), EndOutcome.UnitId, static_cast<int32>(INDEX_NONE));

			FString EndFailure;
			if (!TestTrue(*FString::Printf(TEXT("leg %d: the end of turn is accepted"), Leg),
					StratSubmitSelectionCommand(Bridge, EndOutcome, EndFailure)))
			{
				AddError(EndFailure);
				return false;
			}
			Machine.NotifyCommandApplied(EndOutcome);
			AcceptedCommands.Add(EndOutcome);

			// §2.11.1: the done set is a per-turn fact and the boundary clears it.
			TestEqual(TEXT("the hand-over clears the selection"),
				Machine.GetSelectedUnitId(), static_cast<int32>(INDEX_NONE));
			TestTrue(TEXT("T-SAVE-05: the side to move changed at the hand-over"),
				Bridge.SideToMove() != ActiveSide);
		}
	}

	Capture.Settle();

	// ---- what the sequence actually was -------------------------------------
	TestTrue(TEXT("T-SAVE-05: the sequence crossed a hand-over -- BOTH sides moved"),
		SidesThatMoved.Num() >= 2);
	TestTrue(TEXT("T-SAVE-05: the sequence contains a refused command, so the count clause can fail"),
		Refusals > 0);
	TestTrue(TEXT("T-SAVE-05: and a Wait, which is a player action that produces no command"),
		Waits > 0);
	TestTrue(TEXT("T-SAVE-05: the sequence is longer than one move"), AcceptedCommands.Num() > 1);

	// ---- THE COUNT, AGAINST THE BRIDGE'S OWN RECORDING -----------------------
	// THIS IS THE JOINT. `RecordedCommandCount()` is the bridge's count of what it
	// RECORDED; `AcceptedCommands.Num()` is this file's count of what it SUBMITTED AND SAW
	// ACCEPTED. Nothing the submission path printed is involved, so the subject is no
	// longer its own witness: route any arm of `StratSubmitSelectionCommand`'s switch
	// through a non-recording apply and the two numbers part company here.
	TestEqual(
		TEXT("T-SAVE-05: the bridge RECORDED exactly the commands this hot-seat sequence had "
		     "accepted -- one entry per acceptance, none for the Wait, none for the refusal"),
		Bridge.RecordedCommandCount(), AcceptedCommands.Num());

	// The log lines are a THIRD reading beside those two, and still worth asserting: a
	// phase-6 gate greps them. They are no longer the count the clause rests on.
	const int32 AcceptedLines = Capture.CountStartingWith(TEXT("STRAT-CMD accepted"));
	const int32 RefusedLines  = Capture.CountStartingWith(TEXT("STRAT-CMD refused"));
	TestEqual(
		TEXT("T-SAVE-05: exactly one `STRAT-CMD accepted` line per accepted command -- no line for "
		     "the Wait, and none for the refusal"),
		AcceptedLines, AcceptedCommands.Num());
	TestEqual(TEXT("T-SAVE-05: the refusal logged in the OTHER shape, so a grep for `accepted` "
	               "counts only commands that applied"),
		RefusedLines, Refusals);

	// AND THE SAME COUNT AGAIN, STRICTLY. `RefusedLines` above came from `CountStartingWith`,
	// which is `IgnoreCase` by design and would count `strat-cmd refused` as a refusal; this
	// one is `IsGrepContractRefusedLine`, which is what a phase-6 `grep` would actually find.
	// THE TWO ARE NOT REDUNDANT AND THE PAIRING IS THE POINT: the loose count proves the line
	// was EMITTED and reached the capture, the strict count proves it was SPELLED the way the
	// gate reads it. A case-only change to the emitter leaves the first green and turns this
	// one red -- which is exactly the residual this pass was opened to close.
	// `T-SAVE-05.GrepContractRejectsARefusedCaseVariant` is what shows this can go red.
	TestEqual(
		TEXT("T-SAVE-05: and the refusal is spelled the way a phase-6 `grep \"STRAT-CMD refused \"` "
		     "would find it -- case-sensitively, which the IgnoreCase count above cannot say"),
		CountMatching(Capture.Lines, &IsGrepContractRefusedLine), Refusals);

	const FString DrivenHash = Bridge.StateHash();
	TestNotEqual(
		TEXT("T-SAVE-05: the sequence moved the state, so the equality below is not the seed hash "
		     "compared with itself"),
		DrivenHash, SeedHash);

	// ---- THE REPLAY, FROM THE BRIDGE'S OWN LOG -------------------------------
	// A second bridge, the same seed, and THE FIRST BRIDGE'S RECORDED LOG -- not this
	// file's outcome list re-run through the same submission function, which is what made
	// the old shape self-referential. No machine, no model, no clicks, and no second call
	// into `StratSubmitSelectionCommand` at all.
	FStratBridge Replayed;
	if (!TestTrue(TEXT("a second bridge seeds from the same scenario"), SeedBridge(Replayed, Error)))
	{
		AddError(Error);
		return false;
	}
	TestEqual(TEXT("the two seeds agree before either is driven"), Replayed.StateHash(), SeedHash);
	TestEqual(TEXT("and the fresh bridge has recorded nothing, which the replay requires"),
		Replayed.RecordedCommandCount(), 0);

	const FStratResult ReplayResult = Bridge.ReplayRecordedLogOnto(Replayed);
	if (!TestTrue(
			TEXT("T-SAVE-05: the bridge's OWN recorded log replays onto the fresh seed -- a hot-seat "
			     "session that recorded nothing is refused here rather than passing as an equal hash"),
			ReplayResult.bOk))
	{
		AddError(ReplayResult.Reason);
		return false;
	}

	// THE CLAUSE. Two `canonicalStateHash` readings, neither predicted here.
	TestEqual(
		TEXT("T-SAVE-05: replaying the hot-seat session's RECORDED log onto a fresh seed reaches an "
		     "EQUAL canonical state hash -- the state is a function of the recorded command stream "
		     "and of nothing the machine, the model or the controller was holding"),
		Replayed.StateHash(), DrivenHash);

	TestEqual(TEXT("and the replayed bridge agrees about whose turn it is"),
		Replayed.SideToMove(), Bridge.SideToMove());
	TestEqual(TEXT("and about which turn it is"), Replayed.Turn(), Bridge.Turn());
	TestEqual(TEXT("and it records what it replayed, which is what makes seed -> replay a fixed point"),
		Replayed.RecordedCommandCount(), Bridge.RecordedCommandCount());

	AddInfo(FString::Printf(
		TEXT("T-SAVE-05: %d accepted commands, %d recorded by the bridge (%d accepted lines), "
		     "%d refused, %d waits; hash %s on both bridges"),
		AcceptedCommands.Num(), Bridge.RecordedCommandCount(), AcceptedLines, Refusals, Waits,
		*DrivenHash));

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-05 -- the `STRAT-CMD accepted` line's shape, and its fields' provenance.
//
// WHY THE LINE IS A GATE'S SUBJECT AT ALL. StratSelectionMachine.h calls the format
// load-bearing: it "is what turns a PIE playtest into a machine-checkable gate in phase 6",
// which means phase 6 will parse it. A format that drifted -- a renamed key, a reordered
// field, a word where a number should be -- would break that gate silently, from the far
// side of a milestone boundary.
//
// THE LINE IS CAPTURED, NOT RECONSTRUCTED, and that is the whole methodological point. A
// clause that formatted the same literal the code writes would agree with any mistake in
// it and would pin nothing at all. What is captured off `GLog` is what a log file would
// hold, and every value in it is compared against a MODULE-SIDE reading:
//   - `turn` and `side` against `Bridge.Turn()` / `Bridge.SideToMove()` READ BEFORE
//     submission, which is where the function itself reads them.
//   - `hash` against `Bridge.StateHash()` read after, which is where the function reads it.
//   - `unit` and `hex` against the outcome that was submitted.
//   - `kind` against the SAVE FORMAT's spelling -- and that is the one field whose
//     expectation is a literal HERE. The reason is the module boundary and NOT the
//     function's linkage, and the difference matters because the earlier wording of this
//     block said "file-local", which was false and told the next reader that no pin was
//     possible anywhere. `strat::saveCommandName` is declared at
//     `strat::saveCommandName` in `Source/StratRules/Save.h` and defined in `Save.good.cpp` at namespace scope
//     with no `static`: it has EXTERNAL LINKAGE. What it lacks is an `_API` macro, and an
//     editor target is a modular build, so THIS module cannot link a call to it (8 x
//     LNK2019, `StratBridge.h`'s opening measurement) -- while `StratBridge/Tests/` can
//     call it directly, and does:
//     `Stratocracy.StratBridge.T-SAVE-05.CommandKindNamesMatchFormat` asserts all five
//     words against the format's own function. The literals below are pinned there; the
//     whole point of the spelling is that a phase-6 gate can compare a line against a
//     `commandLog` entry without a translation table.
//
// THE CHAIN TO §4.10's LOG ENTRY, since this module cannot see one:
//   line's {turn, side} == Turn()/SideToMove() before submission   (here)
//   Turn()/SideToMove() before submission == the recorded entry's {turn, side}
//                                     (`T-SAVE-06.TurnAndSideAreReadBeforeStamp`)
//   therefore the line's {turn, side} agree with the log entry, field for field.
//
// ENDTURN IS THE INTERESTING LINE and it is checked separately: it is the only command
// whose turn tag differs before and after, and the only one that names no unit and no hex
// -- which the format carries as `-1` and `-1,-1` rather than as a word, so that the
// line's SHAPE does not depend on its content.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCmdLineShapeTest,
	"Stratocracy.StratPlay.T-SAVE-05.StratCmdLineShape",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCmdLineShapeTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratHotSeatReplayParity;

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

	const FStratUnitView* const Mover = NextActiveUnit(Model, Machine, INDEX_NONE);
	if (!TestNotNull(TEXT("the side to move has an unspent unit"), Mover))
	{
		return false;
	}
	const int32     MoverId  = Mover->UnitId;
	const FIntPoint MoverHex = Mover->Hex;

	Machine.HandleEvent(EStratSelectionEvent::HexPrimary, MoverHex, Model, Query);
	FIntPoint Dest;
	if (!TestTrue(TEXT("the module lists a destination"),
			PickDestination(Query, Model, MoverId, MoverHex, Dest, Error)))
	{
		AddError(Error);
		return false;
	}

	const FStratSelectionOutcome MoveOutcome =
		Machine.HandleEvent(EStratSelectionEvent::HexPrimary, Dest, Model, Query);
	if (!TestEqual(TEXT("the click is a Move"), static_cast<int32>(MoveOutcome.Command),
			static_cast<int32>(EStratSelectionCommand::Move)))
	{
		AddError(MoveOutcome.FailureReason);
		return false;
	}

	// ---- the Move line ------------------------------------------------------
	{
		FStratCmdCapture Capture;

		const int32 TurnBefore = Bridge.Turn();
		const int32 SideBefore = Bridge.SideToMove();

		FString Failure;
		if (!TestTrue(TEXT("the move is accepted"),
				StratSubmitSelectionCommand(Bridge, MoveOutcome, Failure)))
		{
			AddError(Failure);
			return false;
		}
		Machine.NotifyCommandApplied(MoveOutcome);

		const FString HashAfter = Bridge.StateHash();
		Capture.Settle();

		if (!TestEqual(TEXT("one accepted command emits exactly one line"), Capture.Lines.Num(), 1))
		{
			for (const FString& L : Capture.Lines)
			{
				AddInfo(L);
			}
			return false;
		}

		const FString Line = Capture.Lines[0];
		AddInfo(Line);

		// CASE-SENSITIVE, via the shared predicate. A real `grep` would break on a case-only
		// change to the emitter, so the clause that stands in for it must too --
		// `T-SAVE-05.GrepContractRejectsACaseVariant` is what shows this can go red.
		TestTrue(TEXT("T-SAVE-05: the line a phase-6 grep looks for"),
			IsGrepContractAcceptedLine(Line));

		TArray<FString> Keys;
		TArray<FString> Values;
		ParseFields(Line, Keys, Values);

		// FIXED FIELD ORDER, no conditional fields. The order is part of the promise.
		const TCHAR* const Expected[] = { TEXT("kind"), TEXT("unit"), TEXT("hex"), TEXT("turn"),
		                                  TEXT("side"), TEXT("hash") };
		if (!TestEqual(TEXT("T-SAVE-05: the line carries six key=value fields"),
				Keys.Num(), static_cast<int32>(UE_ARRAY_COUNT(Expected))))
		{
			return false;
		}
		for (int32 i = 0; i < Keys.Num(); ++i)
		{
			TestEqual(*FString::Printf(TEXT("T-SAVE-05: field %d is `%s`"), i, Expected[i]),
				Keys[i], FString(Expected[i]));
		}

		// THE PROVENANCE OF EVERY VALUE.
		TestEqual(TEXT("T-SAVE-05: kind is the SAVE FORMAT's spelling, so a gate needs no translation table"),
			Values[0], FString(TEXT("Move")));
		TestEqual(TEXT("T-SAVE-05: unit is the submitted outcome's unit, as a number"),
			Values[1], FString::FromInt(MoveOutcome.UnitId));
		TestEqual(TEXT("T-SAVE-05: hex is the submitted outcome's hex, q,r"),
			Values[2], FString::Printf(TEXT("%d,%d"), MoveOutcome.Hex.X, MoveOutcome.Hex.Y));
		TestEqual(TEXT("T-SAVE-05: turn is the bridge's turn READ BEFORE submission"),
			Values[3], FString::FromInt(TurnBefore));
		TestEqual(TEXT("T-SAVE-05: side is the bridge's side to move READ BEFORE submission"),
			Values[4], FString::FromInt(SideBefore));
		TestEqual(TEXT("T-SAVE-05: hash is StateHash() AFTER the command applied"),
			Values[5], HashAfter);
		TestFalse(TEXT("and that hash is not empty"), HashAfter.IsEmpty());
	}

	// ---- the EndTurn line ---------------------------------------------------
	{
		FStratViewModel EndModel;
		if (!TestTrue(TEXT("the model rebuilds"), Refresh(Bridge, Machine, EndModel, Error)))
		{
			AddError(Error);
			return false;
		}

		const FStratSelectionOutcome EndOutcome = Machine.HandleEvent(
			EStratSelectionEvent::EndTurn, FIntPoint::ZeroValue, EndModel, Query);

		FStratCmdCapture Capture;

		const int32 TurnBefore = Bridge.Turn();
		const int32 SideBefore = Bridge.SideToMove();

		FString Failure;
		if (!TestTrue(TEXT("the end of turn is accepted"),
				StratSubmitSelectionCommand(Bridge, EndOutcome, Failure)))
		{
			AddError(Failure);
			return false;
		}
		Machine.NotifyCommandApplied(EndOutcome);

		const FString HashAfter = Bridge.StateHash();
		const int32   TurnAfter = Bridge.Turn();
		const int32   SideAfter = Bridge.SideToMove();
		Capture.Settle();

		if (!TestEqual(TEXT("one accepted command emits exactly one line"), Capture.Lines.Num(), 1))
		{
			for (const FString& L : Capture.Lines)
			{
				AddInfo(L);
			}
			return false;
		}

		const FString Line = Capture.Lines[0];
		AddInfo(Line);

		TArray<FString> Keys;
		TArray<FString> Values;
		ParseFields(Line, Keys, Values);
		if (!TestEqual(TEXT("the EndTurn line has the SAME six fields -- the shape does not depend "
		                    "on the content"),
				Keys.Num(), 6))
		{
			return false;
		}

		TestEqual(TEXT("T-SAVE-05: kind is the format's `EndTurn`"), Values[0], FString(TEXT("EndTurn")));
		TestEqual(TEXT("T-SAVE-05: an EndTurn names no unit, and says so as -1 rather than as a word"),
			Values[1], FString(TEXT("-1")));
		TestEqual(TEXT("T-SAVE-05: and no hex, as -1,-1"), Values[2], FString(TEXT("-1,-1")));

		// THE CASE THE WHOLE READ-BEFORE ORDERING EXISTS FOR.
		TestEqual(TEXT("T-SAVE-05: the command that CLOSES turn N is logged as turn N"),
			Values[3], FString::FromInt(TurnBefore));
		TestEqual(TEXT("T-SAVE-05: and with the side that closed it"),
			Values[4], FString::FromInt(SideBefore));
		TestTrue(
			FString::Printf(TEXT("T-SAVE-05: the bridge has moved on to {turn %d, side %d}, so "
			                     "'before' and 'after' are distinguishable and the two clauses above "
			                     "are not asserting the same reading twice"),
				TurnAfter, SideAfter),
			(TurnAfter != TurnBefore) || (SideAfter != SideBefore));

		TestEqual(TEXT("T-SAVE-05: hash is StateHash() after the hand-over applied"),
			Values[5], HashAfter);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-01 / T-SAVE-05 -- A CLICKED ATTACK, END TO END, THROUGH THE REAL RULES MODULE.
//
// WHAT WAS UNPINNED BEFORE THIS CLAUSE, and it was a real hole rather than a tidy-up. No
// test in the tree produced an Attack outcome at all: `EStratSelectionCommand::Attack`
// appeared four times in `StratSelectionMachine.cpp` and nowhere else, and
// `SubmitAttackAtHex` had exactly one caller -- that file -- and no test caller. So the
// machine's attack branch, the DONE-and-deselect on an accepted attack, and `kind=Attack`
// in the `STRAT-CMD` line were all executed by nothing, on a path a player takes every
// turn.
//
// WHY IT IS A DRIVEN ADVANCE AND NOT A REPLAYED FIXTURE. Ferrum Crossing deploys the two
// sides on opposite edges and NOTHING IS IN RANGE ON TURN 1 -- measured, and recorded in
// `StratSelectionFacadeParity.cpp` as `0 enumerated targets` at the seeded opening. That
// file reaches an attack-legal position by replaying `Data/parity_fixture.save`, which
// needs `strat::SaveCommand`; THIS module may not name one. The only route to contact from
// here is to play the game: advance both sides through the machine, turn by turn, until the
// rules module itself offers a target.
//
// THE ADVANCE IS FIXTURE CONSTRUCTION AND THE CLAUSE SAYS SO PLAINLY. Every hex clicked is
// an element of `ReachableHexes`'s own answer; `AdvanceHexDistance` decides only WHICH
// element, and no assertion anywhere below compares anything against it. THE TARGET IS NOT
// CHOSEN THAT WAY -- it comes from `AttackTargetHexes`, the module's own enumeration, which
// `T-UI-01.AttackTargetsAreForecastLegal` pins in both directions.
//
// WHAT IS ACTUALLY ASSERTED, once contact exists:
//   - the machine turns a click on an enemy-occupied hex into an ATTACK naming that unit
//     and that hex (the enemy-click arm of `HandleEvent`'s `HexPrimary` case, the one ending
//     in the `Command = Attack` write);
//   - the rules module ACCEPTS it -- which is what makes this more than the stub-driven
//     branch clause in `StratSelectionMachineParity.cpp`, and which a transposed
//     `FIntPoint -> strat::Hex` conversion would fail
//     (`T-UI-01.SubmitAttackAtHexIsNotTransposed` is the sharp form of that);
//   - the bridge RECORDED it, read off `RecordedCommandCount()` and not off a log line;
//   - the machine marks the attacker DONE and drops the selection (`:350-356`), which is
//     §2.11.1's "an attack ends the unit's turn";
//   - the emitted line says `kind=Attack` (`:474`), captured off `GLog`.
//
// IF NO ATTACK EVER BECOMES LEGAL the clause FAILS and reports the closest the two sides
// came. That is deliberate: a silent skip would be a green clause asserting nothing, which
// is the exact shape this phase was blocked for twice.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratHotSeatClickedAttackTest,
	"Stratocracy.StratPlay.T-UI-01.ClickedAttackIsAcceptedAndRecorded",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratHotSeatClickedAttackTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratHotSeatReplayParity;

	// NO `AddExpectedMessagePlain` FOR `STRAT-CMD refused` HERE, and that is a measurement
	// rather than an omission: with one, this clause failed on `Expected suppressed
	// ('Warning') level log message or higher matching 'STRAT-CMD refused' did not occur`.
	// The advance below submits nothing the rules module rejects -- every move is an
	// element of `ReachableHexes` and the attack is an element of `AttackTargetHexes` -- so
	// no refusal line is emitted at all. The gating clause above declares one because it
	// deliberately hand-builds an illegal command; this one must not.
	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratSelectionMachine Machine;
	FStratBridgeRulesQuery Query(&Bridge);

	// Generous, and bounded. The fixture's own emitting build found an attack on turn 1
	// after both sides advanced; this is room for the board to be re-authored without the
	// clause becoming flaky, and it is still a hard stop rather than an unbounded loop.
	const int32 kMaxTurns = 12;

	bool  bAttacked      = false;
	int32 ClosestContact = MAX_int32;
	int32 TurnsPlayed    = 0;
	int32 MovesPlayed    = 0;

	for (int32 Leg = 0; Leg < kMaxTurns && !bAttacked; ++Leg)
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

		// The active side's unit ids, taken once: the model is rebuilt inside the loop and
		// a reference into it would not survive.
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
			if (bAttacked)
			{
				break;
			}
			if (!Refresh(Bridge, Machine, Model, Error))
			{
				AddError(Error);
				return false;
			}

			const FStratUnitView* Unit = nullptr;
			for (const FStratUnitView& U : Model.Units)
			{
				if (U.UnitId == UnitId)
				{
					Unit = &U;
					break;
				}
			}
			if (Unit == nullptr || Unit->bDone || Unit->Side != Model.Match.SideToMove)
			{
				continue;
			}

			ClosestContact = FMath::Min(ClosestContact,
				NearestEnemyDistance(Model, Unit->Hex, Unit->Side));

			// SELECT, then ask the module whether this unit has anything to shoot at.
			Machine.HandleEvent(EStratSelectionEvent::HexPrimary, Unit->Hex, Model, Query);
			if (Machine.GetSelectedUnitId() != UnitId)
			{
				continue;
			}

			for (int32 Attempt = 0; Attempt < 2 && !bAttacked; ++Attempt)
			{
				// ---- is there a target? -----------------------------------------
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
					const FStratUnitView* const Victim = FindUnitAtHex(Model, Target);
					if (!TestNotNull(
							*FString::Printf(TEXT("the enumerated target %s is occupied by a unit the "
							                      "model carries"), *Describe(Target)),
							Victim))
					{
						return false;
					}
					TestTrue(TEXT("T-UI-01: and that unit belongs to the other side"),
						Victim->Side != Model.Match.SideToMove);

					// ---- THE NEGATIVE DIRECTION, FIRST -------------------------
					// An enemy the module did NOT list is refused, with a reason, and the
					// selection survives. Without this the branch would be pinned only where
					// it says yes, and a machine that answered Attack for every enemy hex --
					// substituting its own range rule for §2.6's -- would be green.
					{
						const FStratUnitView* Unlisted = nullptr;
						for (const FStratUnitView& U : Model.Units)
						{
							if (U.Side != Model.Match.SideToMove && !Targets.Contains(U.Hex))
							{
								Unlisted = &U;
								break;
							}
						}
						if (Unlisted != nullptr)
						{
							const FStratSelectionOutcome Refused = Machine.HandleEvent(
								EStratSelectionEvent::HexPrimary, Unlisted->Hex, Model, Query);
							TestEqual(
								*FString::Printf(
									TEXT("T-UI-01: the enemy at %s is NOT in the module's target set, so "
									     "the click produces no command"), *Describe(Unlisted->Hex)),
								static_cast<int32>(Refused.Command),
								static_cast<int32>(EStratSelectionCommand::None));
							TestFalse(TEXT("T-UI-01: and the machine says why"),
								Refused.FailureReason.IsEmpty());
							TestEqual(TEXT("T-UI-01: and the selection survives a refused click"),
								Machine.GetSelectedUnitId(), UnitId);
						}
						else
						{
							AddInfo(TEXT("T-UI-01: every enemy on the board is a listed target at this "
							             "position, so the negative direction had nothing to run on"));
						}
					}

					const FStratSelectionOutcome Outcome = Machine.HandleEvent(
						EStratSelectionEvent::HexPrimary, Target, Model, Query);

					if (!TestEqual(
							*FString::Printf(
								TEXT("T-UI-01: clicking the enemy at %s with unit %d selected is an "
								     "ATTACK -- the enemy-click arm of HandleEvent's HexPrimary case"),
								*Describe(Target), UnitId),
							static_cast<int32>(Outcome.Command),
							static_cast<int32>(EStratSelectionCommand::Attack)))
					{
						AddError(Outcome.FailureReason);
						return false;
					}
					TestEqual(TEXT("T-UI-01: it names the selected unit"), Outcome.UnitId, UnitId);
					TestEqual(TEXT("T-UI-01: and the hex that was clicked"), Outcome.Hex, Target);

					const int32   CountBefore = Bridge.RecordedCommandCount();
					const FString HashBefore  = Bridge.StateHash();
					const int32   TurnBefore  = Bridge.Turn();
					const int32   SideBefore  = Bridge.SideToMove();

					FStratCmdCapture Capture;

					FString AttackFailure;
					if (!TestTrue(
							TEXT("T-UI-01: the RULES MODULE accepts the attack the machine emitted -- a "
							     "transposed FIntPoint -> strat::Hex conversion would be refused here"),
							StratSubmitSelectionCommand(Bridge, Outcome, AttackFailure)))
					{
						AddError(AttackFailure);
						return false;
					}

					const FString HashAfter = Bridge.StateHash();
					Capture.Settle();

					TestEqual(
						TEXT("T-SAVE-05: the accepted attack was RECORDED, exactly once -- the bridge's "
						     "own count, not a log line"),
						Bridge.RecordedCommandCount(), CountBefore + 1);
					TestNotEqual(TEXT("T-UI-01: and it moved the state, so 'accepted' is not a no-op"),
						HashAfter, HashBefore);

					// §2.11.1 -- an attack ends the unit's turn: `NotifyCommandApplied`'s `Attack`
					// arm is what adds the attacker to `DoneUnits`.
					Machine.NotifyCommandApplied(Outcome);
					TestTrue(
						TEXT("T-UI-01: §2.11.1 -- an accepted attack marks the attacker DONE"),
						Machine.IsDone(UnitId));
					TestEqual(TEXT("T-UI-01: and drops the selection"),
						Machine.GetSelectedUnitId(), static_cast<int32>(INDEX_NONE));

					// ---- the line, captured off GLog ----------------------------
					if (TestEqual(TEXT("one accepted command emits exactly one line"),
							Capture.Lines.Num(), 1))
					{
						const FString Line = Capture.Lines[0];
						AddInfo(Line);

						TArray<FString> Keys;
						TArray<FString> Values;
						ParseFields(Line, Keys, Values);
						if (TestEqual(TEXT("the attack line carries the same six fields"), Keys.Num(), 6))
						{
							// The word itself is pinned against the save format's own
							// `strat::saveCommandName` by
							// `Stratocracy.StratBridge.T-SAVE-05.CommandKindNamesMatchFormat`.
							TestEqual(
								TEXT("T-SAVE-05: kind is the format's `Attack` -- the third arm of the "
								     "switch, which no line in this tree had ever produced"),
								Values[0], FString(TEXT("Attack")));
							TestEqual(TEXT("T-SAVE-05: unit is the attacker"),
								Values[1], FString::FromInt(UnitId));
							TestEqual(TEXT("T-SAVE-05: hex is the TARGET, q,r"),
								Values[2], FString::Printf(TEXT("%d,%d"), Target.X, Target.Y));
							TestEqual(TEXT("T-SAVE-05: turn is the bridge's turn read before submission"),
								Values[3], FString::FromInt(TurnBefore));
							TestEqual(TEXT("T-SAVE-05: side is the bridge's side read before submission"),
								Values[4], FString::FromInt(SideBefore));
							TestEqual(TEXT("T-SAVE-05: hash is StateHash() after the attack applied"),
								Values[5], HashAfter);
						}
					}
					else
					{
						for (const FString& L : Capture.Lines)
						{
							AddInfo(L);
						}
					}

					AddInfo(FString::Printf(
						TEXT("T-UI-01: unit %d (side %d) attacked %s on turn %d, after %d advancing "
						     "moves over %d turns"),
						UnitId, ActiveSide, *Describe(Target), TurnBefore, MovesPlayed, TurnsPlayed));
					bAttacked = true;
					break;
					// ==============================================================
				}

				// ---- no target: advance, then look again -------------------------
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
				Unit = nullptr;
				for (const FStratUnitView& U : Model.Units)
				{
					if (U.UnitId == UnitId)
					{
						Unit = &U;
						break;
					}
				}
				if (Unit == nullptr)
				{
					break;
				}
				ClosestContact = FMath::Min(ClosestContact,
					NearestEnemyDistance(Model, Unit->Hex, Unit->Side));
			}
		}

		if (bAttacked)
		{
			break;
		}

		// ---- hand over ------------------------------------------------------
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

	// A SKIP IS A FAILURE. If the two sides never came into contact the clause asserted
	// nothing about the attack path, and it says so with the number that would let the next
	// reader judge whether the board or the advance is at fault.
	TestTrue(
		*FString::Printf(
			TEXT("T-UI-01: a legal attack came into existence within %d turns and was clicked -- the "
			     "closest the two sides came was %d hexes, over %d advancing moves"),
			kMaxTurns, ClosestContact, MovesPlayed),
		bAttacked);

	return bAttacked;
}

// ---------------------------------------------------------------------------
// T-SAVE-05 -- THE GREP CONTRACT REJECTS A CASE VARIANT, AND THE INSTRUMENT THAT FEEDS IT
// DOES NOT.
//
// WHY THIS CLAUSE EXISTS AT ALL. `IsGrepContractAcceptedLine` is `ESearchCase::CaseSensitive`
// as of this pass, and the shape clause above leans on it for "the line a phase-6 grep looks
// for". But NO INPUT THIS TREE CAN PRODUCE EXERCISES THE STRICTNESS: a case-variant census
// over all of `Source/` found no lower-case spelling of any log prefix, so on every real run
// the tightened predicate and the old loose one return the same answer for the same reason.
// A `CaseSensitive` comparison that nothing ever pushes against is the same unfalsifiable
// claim as the `IgnoreCase` one it replaced, wearing better-looking source. This project has
// already shipped a commit titled "a gate whose own fixtures could not fail it"; an absence
// proves nothing until the instrument is shown able to speak.
//
// SO THE INPUT IS SYNTHETIC, AND IT HAS TO BE. The lower-case line is manufactured HERE
// because manufacturing it in the emitter would be a change to code this file is not allowed
// to touch -- and, more to the point, a permanent lie in the shipped log format. What is fed
// in is not a hand-typed second literal either: it is the correct line put through
// `FString::ToLower()`, so the two differ in CASE ONLY BY CONSTRUCTION rather than by my
// having typed them carefully. `T-SAVE-05.StratCmdLineShape` above is what pins that the
// correct line's shape is the one the emitter really produces; this clause pins only what the
// predicate does with a case variant of it.
//
// THE THREE THINGS ASSERTED, AND THE ORDER MATTERS:
//   1. A CONTROL. The capture filter ADMITS the lower-case line. Without this the clause
//      could pass because the device silently swallowed its own fixture, and "rejected by
//      the predicate" and "never reached the predicate" are different facts.
//   2. THE POSITIVE DIRECTION. The predicate accepts the correctly-spelled line -- otherwise
//      a predicate that returned `false` for everything would satisfy point 3 and pin
//      nothing.
//   3. THE NEGATIVE DIRECTION. The predicate REFUSES the lower-case line.
// plus a fourth, which is the receipt: the BARE `StartsWith` overload -- the exact expression
// this pass replaced -- still ACCEPTS that same lower-case line. That is the defect, named
// rather than described, and it is what makes point 3 a change in behaviour instead of a
// restatement.
//
// `FString`'s `==`, `Contains` and `TestEqual` are all `IgnoreCase` in UE 5.8, which is why
// every comparison below spells `ESearchCase::CaseSensitive` explicitly. A `TestEqual` on the
// two lines would report them EQUAL and the clause would be quietly meaningless.
//
// NO LOG IS EMITTED HERE. The device is driven through `Serialize` directly, which is the
// method `FOutputDeviceRedirector` would call with the formatted message anyway, so the
// clause needs no engine timing, no `GLog->Flush()`, and no emitter it does not own.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGrepContractCaseTest,
	"Stratocracy.StratPlay.T-SAVE-05.GrepContractRejectsACaseVariant",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGrepContractCaseTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratHotSeatReplayParity;

	// The shape `StratSelectionMachine.cpp` emits and `T-SAVE-05.StratCmdLineShape` pins,
	// field for field. The VALUES are immaterial here -- this clause is about the prefix.
	const FString Correct =
		TEXT("STRAT-CMD accepted kind=Move unit=0 hex=0,0 turn=1 side=0 hash=0000");
	const FString Lowered = Correct.ToLower();

	// The fixture is a CASE VARIANT and nothing else, and this is asserted rather than
	// assumed -- `FString::operator==` is IgnoreCase in UE 5.8, so the two would compare
	// EQUAL and a reader could not tell a case variant from a copy.
	TestFalse(TEXT("T-SAVE-05: the two fixture lines differ, case-sensitively"),
		Correct.Equals(Lowered, ESearchCase::CaseSensitive));
	TestTrue(TEXT("T-SAVE-05: and differ in NOTHING BUT case -- so what the predicate does "
	              "with them turns on case alone"),
		Correct.Equals(Lowered, ESearchCase::IgnoreCase));

	FStratCmdCapture Capture;
	TestEqual(TEXT("the capture's window starts empty, so both lines below are its own"),
		Capture.Lines.Num(), 0);

	Capture.Serialize(*Correct, ELogVerbosity::Log, FName(TEXT("LogStrat")));
	Capture.Serialize(*Lowered, ELogVerbosity::Log, FName(TEXT("LogStrat")));

	// ---- 1. THE CONTROL: the instrument is shown able to speak ------------------
	// The filter is DELIBERATELY case-insensitive -- see the block on `FStratCmdCapture`.
	// This asserts that choice as a fact rather than leaving it as a comment, and it is what
	// makes the rejection below a statement about the PREDICATE and not about the device.
	if (!TestEqual(
			TEXT("T-SAVE-05: the capture filter is deliberately IgnoreCase, so the lower-case "
			     "line REACHES the assertion instead of vanishing into the filter"),
			Capture.Lines.Num(), 2))
	{
		for (const FString& L : Capture.Lines)
		{
			AddInfo(L);
		}
		return false;
	}

	const FString Admitted = Capture.Lines[0];
	const FString Variant  = Capture.Lines[1];
	AddInfo(Admitted);
	AddInfo(Variant);

	// The counter is left loose for the same reason, and that is pinned here too: a
	// case-mangled line is still counted as one accepted line, so every count in this file
	// still balances and the `TestTrue` is the ONLY thing standing between the tree and a
	// broken phase-6 grep.
	TestEqual(
		TEXT("T-SAVE-05: `CountStartingWith` is deliberately IgnoreCase too -- it counts the "
		     "case variant, which is why no count in this file can catch a spelling change"),
		Capture.CountStartingWith(TEXT("STRAT-CMD accepted")), 2);

	// ---- 2. THE POSITIVE DIRECTION ---------------------------------------------
	TestTrue(
		TEXT("T-SAVE-05: the grep contract ACCEPTS the correctly-spelled line -- without this "
		     "a predicate that refused everything would satisfy the clause below"),
		IsGrepContractAcceptedLine(Admitted));

	// ---- 3. THE NEGATIVE DIRECTION: the property this whole clause exists for ---
	TestFalse(
		TEXT("T-SAVE-05: and REFUSES a case-only variant of it -- a real `grep` for "
		     "`STRAT-CMD accepted ` would find nothing, and so does the clause that stands in "
		     "for that grep"),
		IsGrepContractAcceptedLine(Variant));

	// ---- 4. THE RECEIPT: the expression this pass replaced still admits it ------
	// NOT a duplicate of point 3 with the sense flipped. This is the OLD predicate, spelled
	// exactly as the shape clause used to spell it, and it is green -- which is the evidence
	// that the tightening changed behaviour rather than decorating source. If UE ever changes
	// `StartsWith`'s default, this line goes red and the residual paragraph in the
	// `FStratCmdCapture` block needs rewriting, which is the right thing for it to do.
	TestTrue(
		TEXT("T-SAVE-05: the bare `StartsWith` overload this pass replaced ACCEPTS the same "
		     "case variant -- that is the defect that was closed, named rather than described"),
		Variant.StartsWith(TEXT("STRAT-CMD accepted ")));

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-05 -- THE `STRAT-CMD refused` LINE'S SHAPE, ON A REAL REFUSAL.
//
// WHAT WAS UNPINNED BEFORE THIS CLAUSE, stated at the width it was measured at. No assertion
// anywhere in this tree said what a `refused` line SAYS. `RefusedLines` came from
// `CountStartingWith`, which is `IgnoreCase` by design; and the
// `AddExpectedMessagePlain(TEXT("STRAT-CMD refused"), ...)` declarations that sit beside every
// provoked refusal are a VERBOSITY gate rather than a spelling gate -- read out of UE 5.8's
// `AutomationTest.h` in the block on `FStratCmdCapture` above. So a case-only change to the
// refusal emitter was caught by nothing, while the `accepted` half had both a strict predicate
// and a clause proving that predicate could fail. This closes the asymmetry.
//
// THE LINE IS CAPTURED OFF `GLog`, NOT RECONSTRUCTED, for the same reason
// `T-SAVE-05.StratCmdLineShape` captures its own: a clause that re-formatted the literal the
// emitter writes would agree with any mistake in it. The synthetic-line clause below is about
// what the PREDICATE does with a case variant; THIS clause is what ties the predicate to the
// string the module really produces. Neither is sufficient alone -- a strict predicate no
// shipped assertion runs is a hole one level up, which is the shape this pass exists to avoid
// repeating.
//
// WHERE THE EXPECTATIONS COME FROM, field by field:
//   - `kind`, `unit`, `hex`  -- the hand-built outcome that was submitted.
//   - `turn`, `side`         -- `Bridge.Turn()` / `Bridge.SideToMove()` READ BEFORE
//                               submission, which is where the emitter reads them.
//   - `reason`               -- `StratSubmitSelectionCommand`'s OWN out-parameter, the rules
//                               module's prose. Not a literal, and not re-derived here.
// The refusal is HAND-BUILT because the machine will never emit an illegal command -- that is
// the T-UI-02 property -- and `StratSubmitSelectionCommand` is precisely the function whose job
// is to refuse one.
//
// WHY THIS CLAUSE DOES NOT ASSERT SIX PARSED FIELDS THE WAY THE ACCEPTED ONE DOES, and this is
// the place the two shapes genuinely part company rather than a shortcut. The emitter's sixth
// field is `reason=%s`, and `%s` is the rules module's prose: it contains spaces, so
// `ParseFields` -- which splits on space and keeps only `key=value` tokens -- cannot recover it
// as one value, and its token count is not fixed. The accepted line ends in `hash=%s`, a single
// space-free token, which is why THAT clause can assert `Keys.Num() == 6` and this one asserts
// the first six keys IN ORDER out of at least six. Writing this clause by analogy with the
// other would have claimed a shape the emitter does not have.
//
// THE REFUSAL IS EMITTED AT `Warning`, so the declaration below is REQUIRED: without it the
// automation framework charges the warning to this test and the suite comes back
// `succeededWithWarnings`.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCmdRefusedLineShapeTest,
	"Stratocracy.StratPlay.T-SAVE-05.StratCmdRefusedLineShape",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCmdRefusedLineShapeTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratHotSeatReplayParity;

	AddExpectedMessagePlain(TEXT("STRAT-CMD refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

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

	// A unit of the side that is NOT to move. Moving it is illegal by §2.11.1, and the
	// UNIT IS THE MODEL'S OWN -- there is no coordinate literal here.
	const FStratUnitView* Enemy = nullptr;
	for (const FStratUnitView& U : Model.Units)
	{
		if (U.Side != Model.Match.SideToMove)
		{
			Enemy = &U;
			break;
		}
	}
	if (!TestNotNull(TEXT("the side that is not to move has a unit to build a refusal around"), Enemy))
	{
		return false;
	}

	FStratSelectionOutcome Illegal;
	Illegal.Command = EStratSelectionCommand::Move;
	Illegal.UnitId  = Enemy->UnitId;
	Illegal.Hex     = Enemy->Hex;

	const FString HashBefore  = Bridge.StateHash();
	const int32   CountBefore = Bridge.RecordedCommandCount();
	const int32   TurnBefore  = Bridge.Turn();
	const int32   SideBefore  = Bridge.SideToMove();

	FString RefusalReason;
	FString Line;
	{
		FStratCmdCapture Capture;

		const bool bApplied = StratSubmitSelectionCommand(Bridge, Illegal, RefusalReason);
		Capture.Settle();

		if (!TestFalse(TEXT("T-SAVE-05: a command for the side that is not to move is refused"),
				bApplied))
		{
			return false;
		}
		TestEqual(TEXT("T-SAVE-05: and changes nothing"), Bridge.StateHash(), HashBefore);
		TestEqual(TEXT("T-SAVE-05: and is not recorded"),
			Bridge.RecordedCommandCount(), CountBefore);

		if (!TestEqual(TEXT("one refused command emits exactly one STRAT-CMD line"),
				Capture.Lines.Num(), 1))
		{
			for (const FString& L : Capture.Lines)
			{
				AddInfo(L);
			}
			return false;
		}
		Line = Capture.Lines[0];
	}
	AddInfo(Line);

	// ---- THE GREP CONTRACT, CASE-SENSITIVELY --------------------------------
	// The shipped assertion the predicate exists for. `T-SAVE-05.GrepContractRejectsARefused
	// CaseVariant` is what shows this can go red on a case-only change.
	TestTrue(
		TEXT("T-SAVE-05: the refusal is spelled the way a phase-6 `grep \"STRAT-CMD refused \"` "
		     "would find it"),
		IsGrepContractRefusedLine(Line));

	// AND IT IS NOT THE OTHER LINE. §4.10's counting gate rests on `grep \"STRAT-CMD accepted\"`
	// returning only commands that APPLIED, which the emitter's own comment calls out; that
	// property is a claim about the refusal, so it is asserted here rather than assumed.
	TestFalse(
		TEXT("T-SAVE-05: and does NOT carry the accepted phrase, so a grep for `accepted` counts "
		     "only commands that applied"),
		IsGrepContractAcceptedLine(Line));
	TestFalse(
		TEXT("T-SAVE-05: nor anywhere within the line -- including inside the rules module's own "
		     "reason text"),
		Line.Contains(TEXT("STRAT-CMD accepted"), ESearchCase::CaseSensitive));

	// ---- THE FIELDS ---------------------------------------------------------
	// The first six keys IN ORDER. `>=` rather than `==` because `reason=`'s value is prose and
	// may contribute further `key=value`-shaped tokens -- see the clause header for why this is
	// a real difference from the accepted line rather than a weaker assertion of the same thing.
	TArray<FString> Keys;
	TArray<FString> Values;
	ParseFields(Line, Keys, Values);

	const TCHAR* const ExpectedKeys[] = { TEXT("kind"), TEXT("unit"), TEXT("hex"), TEXT("turn"),
	                                      TEXT("side"), TEXT("reason") };
	if (!TestTrue(
			*FString::Printf(TEXT("T-SAVE-05: the refused line opens with six key=value fields "
			                      "(parsed %d)"), Keys.Num()),
			Keys.Num() >= static_cast<int32>(UE_ARRAY_COUNT(ExpectedKeys))))
	{
		return false;
	}
	for (int32 i = 0; i < static_cast<int32>(UE_ARRAY_COUNT(ExpectedKeys)); ++i)
	{
		TestEqual(*FString::Printf(TEXT("T-SAVE-05: field %d is `%s`"), i, ExpectedKeys[i]),
			Keys[i], FString(ExpectedKeys[i]));
	}

	// THE PROVENANCE OF EVERY VALUE. `kind` is the SAVE FORMAT's spelling, pinned against
	// `strat::saveCommandName` by `Stratocracy.StratBridge.T-SAVE-05.CommandKindNamesMatchFormat`
	// -- this module cannot call that function (8 x LNK2019), which is why the word is a literal
	// HERE and not unpinned anywhere.
	TestEqual(TEXT("T-SAVE-05: kind is the save format's `Move`"), Values[0], FString(TEXT("Move")));
	TestEqual(TEXT("T-SAVE-05: unit is the submitted outcome's unit"),
		Values[1], FString::FromInt(Illegal.UnitId));
	TestEqual(TEXT("T-SAVE-05: hex is the submitted outcome's hex, q,r"),
		Values[2], FString::Printf(TEXT("%d,%d"), Illegal.Hex.X, Illegal.Hex.Y));
	TestEqual(TEXT("T-SAVE-05: turn is the bridge's turn READ BEFORE submission"),
		Values[3], FString::FromInt(TurnBefore));
	TestEqual(TEXT("T-SAVE-05: side is the bridge's side to move READ BEFORE submission"),
		Values[4], FString::FromInt(SideBefore));

	// `reason` is the one field the accepted line has no counterpart for, and its expectation is
	// the OUT-PARAMETER the same call filled -- the rules module's own prose, compared whole
	// rather than through `ParseFields`, which cannot recover a value containing spaces.
	TestFalse(TEXT("T-SAVE-05: the refusal carries the rules module's own reason"),
		RefusalReason.IsEmpty());
	TestTrue(
		*FString::Printf(TEXT("T-SAVE-05: and the line's `reason=` is that same string, whole -- "
		                      "`%s`"), *RefusalReason),
		Line.Contains(FString::Printf(TEXT("reason=%s"), *RefusalReason), ESearchCase::CaseSensitive));
	TestTrue(TEXT("T-SAVE-05: and `reason=` is the LAST field, so the prose cannot swallow one"),
		Line.EndsWith(FString::Printf(TEXT("reason=%s"), *RefusalReason), ESearchCase::CaseSensitive));

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-05 -- THE REFUSED GREP CONTRACT REJECTS A CASE VARIANT, AND THE INSTRUMENT THAT
// FEEDS IT DOES NOT.
//
// The `refused` counterpart of `T-SAVE-05.GrepContractRejectsACaseVariant`, and it exists for
// the same reason: `IsGrepContractRefusedLine` is `ESearchCase::CaseSensitive`, two shipped
// assertions now lean on it, and NO INPUT THIS TREE CAN PRODUCE EXERCISES THE STRICTNESS --
// every real emitter spells the prefix correctly, so on every real run the strict predicate and
// a loose one agree for the same reason. A `CaseSensitive` comparison nothing pushes against is
// unfalsifiable no matter how it is spelled.
//
// SAME FOUR LOAD-BEARING ASSERTIONS AS THE ACCEPTED CLAUSE, and the order matters:
//   1. A CONTROL -- the capture filter ADMITS the lower-case line, so "rejected by the
//      predicate" is distinguishable from "never reached the predicate".
//   2. THE POSITIVE -- the predicate accepts the correctly-spelled line, without which a
//      predicate returning `false` for everything would satisfy point 3.
//   3. THE NEGATIVE -- the predicate REFUSES the case variant.
//   4. THE RECEIPT -- the loose expression still ACCEPTS the same variant, which names the
//      defect rather than describing it.
//
// AND ONE MORE THAN THAT CLAUSE HAS, which is the improvement rather than a copy. §4.10's
// counting gate rests on the two shapes being TOLD APART ON THE SECOND TOKEN: a `grep
// "STRAT-CMD accepted"` must count applied commands and no refusals. Point 5 asserts the two
// predicates are mutually exclusive on correctly-spelled lines of both kinds -- which neither
// predicate's own clause can say, because each sees only one shape. A refused predicate that
// had been written as `StartsWith(TEXT("STRAT-CMD "))` would pass points 1-4 unchanged and fail
// only here.
//
// THE FIXTURE-INTEGRITY TRAP IS PINNED, NOT ASSUMED. `FString`'s `==`, `Contains` and
// `TestEqual` are all `IgnoreCase` in UE 5.8, so `TestEqual(Correct, Lowered)` would report the
// two EQUAL and render the fixture meaningless. Both directions are asserted explicitly below.
//
// NO LOG IS EMITTED HERE, so no `AddExpectedMessagePlain` is owed: the device is driven through
// `Serialize` directly -- the method `FOutputDeviceRedirector` would call with the formatted
// message anyway -- and nothing reaches `GLog` at `Warning` or any other verbosity.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGrepContractRefusedCaseTest,
	"Stratocracy.StratPlay.T-SAVE-05.GrepContractRejectsARefusedCaseVariant",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGrepContractRefusedCaseTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratHotSeatReplayParity;

	// The shape `StratSelectionMachine.cpp` emits and `T-SAVE-05.StratCmdRefusedLineShape` pins
	// against a REAL captured refusal. The values are immaterial here -- this clause is about
	// the prefix -- but the trailing `reason=` prose is kept because it is what makes this line
	// structurally unlike the accepted one.
	const FString CorrectRefused =
		TEXT("STRAT-CMD refused kind=Move unit=0 hex=0,0 turn=1 side=0 reason=not your unit");
	const FString LoweredRefused = CorrectRefused.ToLower();

	// The fixture is a CASE VARIANT and nothing else -- asserted, because `FString::operator==`
	// is IgnoreCase in UE 5.8 and a reader could not otherwise tell a variant from a copy.
	TestFalse(TEXT("T-SAVE-05: the two fixture lines differ, case-sensitively"),
		CorrectRefused.Equals(LoweredRefused, ESearchCase::CaseSensitive));
	TestTrue(TEXT("T-SAVE-05: and differ in NOTHING BUT case -- so what the predicate does with "
	              "them turns on case alone"),
		CorrectRefused.Equals(LoweredRefused, ESearchCase::IgnoreCase));

	FStratCmdCapture Capture;
	TestEqual(TEXT("the capture's window starts empty, so both lines below are its own"),
		Capture.Lines.Num(), 0);

	Capture.Serialize(*CorrectRefused, ELogVerbosity::Warning, FName(TEXT("LogStratPlay")));
	Capture.Serialize(*LoweredRefused, ELogVerbosity::Warning, FName(TEXT("LogStratPlay")));

	// ---- 1. THE CONTROL -----------------------------------------------------
	if (!TestEqual(
			TEXT("T-SAVE-05: the capture filter is deliberately IgnoreCase, so the lower-case "
			     "refusal REACHES the assertion instead of vanishing into the filter"),
			Capture.Lines.Num(), 2))
	{
		for (const FString& L : Capture.Lines)
		{
			AddInfo(L);
		}
		return false;
	}

	const FString Admitted = Capture.Lines[0];
	const FString Variant  = Capture.Lines[1];
	AddInfo(Admitted);
	AddInfo(Variant);

	// The counter is loose for the same reason, and that is pinned rather than left in a
	// comment: a case-mangled refusal is still counted as a refusal by `CountStartingWith`, so
	// the count clause in the gating test still balances and cannot catch a spelling change.
	// That is precisely why the STRICT count beside it exists.
	TestEqual(
		TEXT("T-SAVE-05: `CountStartingWith` counts the case variant as a refusal -- which is why "
		     "the IgnoreCase count in the gating clause cannot catch a spelling change"),
		Capture.CountStartingWith(TEXT("STRAT-CMD refused")), 2);
	TestEqual(
		TEXT("T-SAVE-05: while the STRICT count over the same two lines finds one -- the pair of "
		     "counts in the gating clause part company exactly here"),
		CountMatching(Capture.Lines, &IsGrepContractRefusedLine), 1);

	// ---- 2. THE POSITIVE DIRECTION ------------------------------------------
	TestTrue(
		TEXT("T-SAVE-05: the refused grep contract ACCEPTS the correctly-spelled line -- without "
		     "this a predicate that refused everything would satisfy the clause below"),
		IsGrepContractRefusedLine(Admitted));

	// ---- 3. THE NEGATIVE DIRECTION: the property this clause exists for -----
	TestFalse(
		TEXT("T-SAVE-05: and REFUSES a case-only variant of it -- a real `grep \"STRAT-CMD "
		     "refused \"` would find nothing, and so does the clause that stands in for it"),
		IsGrepContractRefusedLine(Variant));

	// ---- 4. THE RECEIPT -----------------------------------------------------
	// The loose expression -- the bare `StartsWith` default, which is what every existing
	// reading of a refusal in this file uses -- still ACCEPTS the variant. That is the defect,
	// named rather than described. If UE ever changes `StartsWith`'s default this goes red, and
	// the residual paragraph in the `FStratCmdCapture` block needs rewriting.
	TestTrue(
		TEXT("T-SAVE-05: the bare `StartsWith` overload ACCEPTS the same case variant -- that is "
		     "the hole this predicate closes, named rather than described"),
		Variant.StartsWith(TEXT("STRAT-CMD refused ")));

	// ---- 5. THE TWO SHAPES ARE TOLD APART ON THE SECOND TOKEN ---------------
	// Not present in the accepted clause, and the reason it is here: each predicate's own
	// falsifiability clause sees only its own shape, so neither can say the pair discriminates.
	// A refused predicate spelled `StartsWith(TEXT("STRAT-CMD "))` passes 1-4 and fails here.
	const FString CorrectAccepted =
		TEXT("STRAT-CMD accepted kind=Move unit=0 hex=0,0 turn=1 side=0 hash=0000");
	TestFalse(TEXT("T-SAVE-05: the refused contract does not admit an ACCEPTED line"),
		IsGrepContractRefusedLine(CorrectAccepted));
	TestFalse(TEXT("T-SAVE-05: and the accepted contract does not admit a REFUSED line -- the two "
	              "are told apart on the second token, which is what §4.10's counting gate rests on"),
		IsGrepContractAcceptedLine(Admitted));

	return true;
}
