// GDD §2.9 / §4.9 / §4.10 -- the AI turn runner's five clauses. T-AI-01 and T-AI-06.
//
// WHAT THESE CLAUSES ARE FOR. Phase B landed `FStratAiTurnRunner` with every new property
// defaulted off, so the suite stayed at 78/78 WITHOUT `RunTurn` ever executing and without
// one arm of `FStratBridgeAiTurnPort::Submit` executing either. A green build across that
// change is evidence of nothing at all. These clauses are what turn it into evidence.
//
// THE PROPERTY THEY ALL ORBIT: A REFUSAL IS NEVER A TURN END. `FStratAiCommand`'s
// default-constructed value is `Kind = EndTurn` (`StratBridge.h:138-147`), so a runner that
// read `Kind` before it read the port's `bool` would treat every refusal -- an unseeded
// bridge, an unknown side, an attack whose target left the board -- as a clean, quiet,
// empty turn. Three of the five clauses below exist to make that failure impossible to ship
// unnoticed, and they can only exist because `IStratAiTurnPort` lets a test make the AI
// refuse. The shipping AI never refuses, never fails a submission and never loops, so all
// three fault arms are UNREACHABLE through a real `FStratBridge` -- the same shape as phase
// 4's second gate finding (`EStratSelectionCommand::Attack`, executed by no test).
//
// WHERE THE EXPECTATIONS COME FROM, which is the only question that matters.
//   - Every COMMAND driven through the runner came out of `FStratBridge::NextAiCommand`.
//     There is not one hex, unit id, defIndex or turn number written in this file.
//   - The `decide` refusal's REASON is `NextAiCommand`'s own refusal on an unseeded bridge,
//     captured from the module and handed to the scripted port, so what the runner is
//     required to forward is the rules module's sentence and not this file's.
//   - Clause 2's step count comes from a REAL turn measured on an identically seeded bridge,
//     never from a number chosen here.
//   - Clause 4 compares the port's four arms against the equivalent DIRECT `FStratBridge`
//     call by resulting `StateHash()`, and pins the recording half with
//     `RecordedCommandCount()` and `ReplayRecordedLogOnto` -- both module-side readers that
//     no logging statement can satisfy.
//   - Clause 5's two sides of the comparison are two bridges' own `StateHash()`.
//   The two places a value IS this file's are the injected `apply` refusal sentence (there
//   is no module-side apply refusal that does not require inventing an illegal command) and
//   the loop bound (a harness number, and the clause asserts the runner REPORTS it). Both
//   are stated at the clause.
//
// THE ABSENCE ASSERTIONS CARRY A CONTROL, AND THAT IS NOT DECORATION. Clause 1 reads the
// SILENCE of `STRAT-AI turn-ended` on the refusal path. A silence proves nothing until the
// instrument has been shown able to speak, so the control -- a real AI turn driven through
// the SAME live `GLog` capture, required to raise the count -- runs FIRST. Phase 6 spent six
// rounds on exactly this; the remedy is recorded in `Tools/architect/state.md`.
//
// EVERY STRING COMPARISON IS CASE SENSITIVE. `FString::operator==`, `operator!=`,
// `FString::Contains` and `TestEqual(FString)` are all case-INSENSITIVE in UE 5.8, so a
// comparison of two byte-identical strings written the ordinary way is a clause that reports
// green forever. `TestEqualSensitive` and `ESearchCase::CaseSensitive` throughout.
//
// IT NAMES NO `strat::` TYPE AND CALLS NO `strat::` FUNCTION, per `StratPlay.Build.cs`.
// Every rules answer arrives engine-typed, through `FStratBridge` or through the view model.

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

#include "StratAiTurnRunner.h"
#include "StratViewModel.h"

// Included from a .cpp and never from a UHT-parsed header -- the module-wide discipline.
#include "StratBridge.h"

namespace StratAiTurnRunnerClauses
{
	/** The same harness pin every parity file in this tree carries. `Replay.h` states that no
	 *  rule decides which side moves first, so this is a harness constant and not a rule. */
	static const int32 kFirstSide = 0;

	/** A hard stop on the self-play loop in clause 4. Phase A measured a whole game at 156
	 *  commands, so this is an order of magnitude clear of anything §2.9 has been seen to do
	 *  and is a runaway guard rather than an expectation. */
	static const int32 kMaxSelfPlayCommands = 2000;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	static FString ScenarioPath()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"), TEXT("ferrum_crossing.json"));
	}

	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
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

		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath(), kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	/**
	 * §4.8's unit ids, READ OFF THE TABLE THE MATCH IS LOADED FROM.
	 *
	 * `FStratBridge::LoadDefinitions` takes the row NAME as the id -- "the row NAME is the
	 * id: FUnitRow carries no Id field" (`StratBridge.cpp:63-65`) -- so this is the same
	 * source the definitions vector was built from and not a second spelling of it. A
	 * hand-written `{"Infantry", "Tank"}` here would be this file deciding §4.8's id space.
	 */
	static TArray<FName> UnitIdsFromTable(FString& OutError)
	{
		TArray<FName> Ids;
		UDataTable* const Units = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		if (Units == nullptr)
		{
			OutError = TEXT("DT_Units did not load from /Game/StratData");
			return Ids;
		}
		Ids = Units->GetRowNames();
		return Ids;
	}

	/** Seeds, then configures §2.9's buildlist from the table's own ids so the Build arm has
	 *  something to be about. An empty buildlist configures an AI that never builds. */
	static bool SeedBridgeWithBuildlist(FStratBridge& Bridge, FString& OutError)
	{
		if (!SeedBridge(Bridge, OutError))
		{
			return false;
		}

		const TArray<FName> Ids = UnitIdsFromTable(OutError);
		if (Ids.Num() == 0)
		{
			if (OutError.IsEmpty())
			{
				OutError = TEXT("DT_Units carries no rows, so no buildlist can be configured");
			}
			return false;
		}

		const FStratResult Set = Bridge.SetBuildlistByIds(Ids);
		if (!Set.bOk)
		{
			OutError = FString::Printf(TEXT("SetBuildlistByIds refused: %s"), *Set.Reason);
			return false;
		}
		return true;
	}

	/** Whether the match has reached a §2.8 result, asked of the rules module through the
	 *  projection rather than predicted from a turn count. */
	static bool MatchHasResult(const FStratBridge& Bridge)
	{
		FStratViewModel Model;
		FString         Reason;
		if (!StratBuildViewModel(Bridge, Bridge.SideToMove(), Model, Reason))
		{
			// Unreadable is not "finished", and the caller's loop bound is what stops it.
			return false;
		}
		return Model.Match.bHasResult;
	}

	/**
	 * THE REFERENCE ROUTE for clause 4: one AI command submitted through the engine-typed
	 * façade DIRECTLY, with no further lookup.
	 *
	 * This is deliberately a second, independent spelling of the switch inside
	 * `FStratBridgeAiTurnPort::Submit`, and that is the entire point of the clause -- the
	 * production switch is the subject and this is the comparison. A production arm wired to
	 * the wrong method moves one bridge's `StateHash()` and not the other's.
	 *
	 * `Attack` PASSES `Hex` AND NOT `TargetId`, which is `FStratAiCommand`'s documented
	 * contract ("`TargetId` is kept beside it for logging, and is NOT the thing to submit",
	 * `StratBridge.h:135`).
	 */
	static FStratResult SubmitDirect(FStratBridge& Bridge, const FStratAiCommand& C)
	{
		switch (C.Kind)
		{
		case EStratAiCommandKind::Build:   return Bridge.SubmitBuildAtHex(C.Hex, C.DefIndex);
		case EStratAiCommandKind::Move:    return Bridge.SubmitMoveToHex(C.UnitId, C.Hex);
		case EStratAiCommandKind::Attack:  return Bridge.SubmitAttackAtHex(C.UnitId, C.Hex);
		case EStratAiCommandKind::EndTurn: return Bridge.SubmitEndTurn();
		}
		// Unreachable while `EStratAiCommandKind` has four enumerators, and written as a
		// refusal rather than a fallthrough so a fifth one added by a re-vendor fails a clause
		// instead of silently submitting the last arm.
		return FStratResult::Fail(TEXT("this test has no submission arm for that command kind"));
	}

	/** For messages only. No clause compares anything against this. */
	static const TCHAR* KindWord(EStratAiCommandKind Kind)
	{
		switch (Kind)
		{
		case EStratAiCommandKind::Build:   return TEXT("Build");
		case EStratAiCommandKind::Move:    return TEXT("Move");
		case EStratAiCommandKind::Attack:  return TEXT("Attack");
		case EStratAiCommandKind::EndTurn: return TEXT("EndTurn");
		}
		return TEXT("Unknown");
	}

	/**
	 * Captures the `STRAT-AI` lines this module emits.
	 *
	 * IT IS NEVER THE LOAD-BEARING WITNESS ON ITS OWN. Where a clause below reads this
	 * capture's SILENCE, the same clause first drives a real AI turn through the same live
	 * device and requires the count to rise -- see this file's header block on why an
	 * absence needs a control.
	 *
	 * `StartsWith` AND `Contains` ARE CASE SENSITIVE HERE, on purpose. Both default to
	 * IgnoreCase in UE 5.8, and a case-insensitive match on a fixed-field log line is a
	 * comparison that cannot fail.
	 *
	 * UNBUFFERED, AND THE OVERRIDE IS LOAD-BEARING. The `LinesBefore` watermarks the clauses
	 * below take do NOT make this device safe on their own: a watermark bounds by INDEX, and
	 * the failure mode bounds by TIME. `FOutputDeviceRedirector` queues a line it cannot
	 * broadcast on the primary-thread fast path (OutputDeviceRedirector.cpp:937) and later
	 * drains it to whichever devices sit in `BufferedOutputDevices` AT DRAIN TIME (:553), so a
	 * line emitted before the watermark can still be APPENDED after it and counted as though
	 * it fell inside the window. That is MEASURED on this project, in
	 * `StratHotSeatReplayParity.cpp` -- 1 failure in 4 runs on byte-identical code, three
	 * pre-window lines landing inside a window that should have held one.
	 *
	 * `CanBeUsedOnMultipleThreads() == true` puts the device in `UnbufferedOutputDevices`
	 * (:440-447), which only the synchronous broadcast at :905 -- inside the emitting `UE_LOG`
	 * itself -- ever feeds. `FlushBufferedItems` cannot reach it. Emission order and delivery
	 * order become the same order, and a watermark then means what it reads as. It is also the
	 * engine's own idiom for a device registered around a test: `FAutomationTestOutputDevice`
	 * overrides it with the comment "Make it unbuffered by returning true"
	 * (AutomationTest.h:1345).
	 *
	 * The override promises the device needs no external locking, so `Lines` is mutated under
	 * `Mutex`. Direct reads of `Lines` are game-thread-only, as every call site below is.
	 */
	struct FStratAiCapture final : public FOutputDevice
	{
		TArray<FString> Lines;

		FStratAiCapture()
		{
			if (GLog != nullptr)
			{
				GLog->AddOutputDevice(this);
			}
		}

		virtual ~FStratAiCapture()
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
			if (Line.StartsWith(TEXT("STRAT-AI"), ESearchCase::CaseSensitive))
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

		/** How many captured lines from `FromIndex` onward contain `Needle`, byte for byte. */
		int32 CountFrom(int32 FromIndex, const TCHAR* Needle) const
		{
			int32 Count = 0;
			for (int32 I = FMath::Max(0, FromIndex); I < Lines.Num(); ++I)
			{
				if (Lines[I].Contains(Needle, ESearchCase::CaseSensitive))
				{
					++Count;
				}
			}
			return Count;
		}

		/** The captured lines from `FromIndex` onward, joined, for a failure message. */
		FString TextFrom(int32 FromIndex) const
		{
			TArray<FString> Slice;
			for (int32 I = FMath::Max(0, FromIndex); I < Lines.Num(); ++I)
			{
				Slice.Add(Lines[I]);
			}
			return Slice.Num() > 0 ? FString::Join(Slice, TEXT(" | ")) : FString(TEXT("<nothing>"));
		}

	private:
		FCriticalSection Mutex;
	};

	/**
	 * A port that DECORATES the production one and refuses at a chosen step.
	 *
	 * A DECORATOR AND NOT A SIMULATOR, and the difference is what keeps these clauses
	 * honest: every command it hands the runner came out of `FStratBridge::NextAiCommand` and
	 * every submission it does not refuse went into the real bridge through
	 * `FStratBridgeAiTurnPort::Submit`. The only thing this object contributes is the fault
	 * -- which is the one thing a real bridge cannot contribute, because the shipping AI
	 * never refuses and never fails a submission.
	 *
	 * ON A REFUSED `NextCommand` IT LEAVES `OutCommand` UNTOUCHED, deliberately. A
	 * default-constructed `FStratAiCommand` reads as a genuine EndTurn, so this is the exact
	 * value a refusing implementation leaves behind and the exact value a runner that
	 * branched on `Kind` first would mistake for "the AI is done".
	 */
	struct FStratFaultInjectingAiPort final : public IStratAiTurnPort
	{
		explicit FStratFaultInjectingAiPort(IStratAiTurnPort& InInner)
			: Inner(InInner)
		{
		}

		IStratAiTurnPort& Inner;

		int32   RefuseDecideAtStep = INDEX_NONE;
		int32   RefuseSubmitAtStep = INDEX_NONE;
		FString InjectedDecideReason;
		FString InjectedSubmitReason;

		mutable int32 DecideCalls = 0;
		int32         SubmitCalls = 0;

		virtual int32   Turn() const override       { return Inner.Turn(); }
		virtual int32   SideToMove() const override { return Inner.SideToMove(); }
		virtual FString StateHash() const override  { return Inner.StateHash(); }

		virtual bool NextCommand(int32 Side, FStratAiCommand& OutCommand,
		                         FString& OutFailureReason) const override
		{
			const int32 Step = DecideCalls++;
			if (Step == RefuseDecideAtStep)
			{
				// `OutCommand` IS NOT WRITTEN. See this struct's header block.
				OutFailureReason = InjectedDecideReason;
				return false;
			}
			return Inner.NextCommand(Side, OutCommand, OutFailureReason);
		}

		virtual bool Submit(const FStratAiCommand& Command, FString& OutFailureReason) override
		{
			const int32 Step = SubmitCalls++;
			if (Step == RefuseSubmitAtStep)
			{
				OutFailureReason = InjectedSubmitReason;
				return false;
			}
			return Inner.Submit(Command, OutFailureReason);
		}
	};

	/**
	 * A port that answers one real command forever and never says EndTurn.
	 *
	 * THE ONE CONFIGURATION A REAL BRIDGE CANNOT PRODUCE. `strat::nextCommand` is a pure
	 * function of state, so the runaway it guards against is an AI that answers a command
	 * which applies without changing the state it read -- unreachable from the shipping AI
	 * and the reason `MaxCommandsPerTurn` exists at all.
	 *
	 * ITS `Submit` ACCEPTS WITHOUT APPLYING, which is stated because it matters: this clause
	 * is about the runner's termination bound and nothing else, and driving real submissions
	 * would end the turn through the rules module long before the bound was reached.
	 * `Turn`, `SideToMove` and `StateHash` still come off a real seeded bridge.
	 */
	struct FStratNeverEndingAiPort final : public IStratAiTurnPort
	{
		FStratNeverEndingAiPort(IStratAiTurnPort& InInner, const FStratAiCommand& InRepeated)
			: Inner(InInner)
			, Repeated(InRepeated)
		{
		}

		IStratAiTurnPort& Inner;
		FStratAiCommand   Repeated;

		mutable int32 DecideCalls = 0;
		int32         SubmitCalls = 0;

		virtual int32   Turn() const override       { return Inner.Turn(); }
		virtual int32   SideToMove() const override { return Inner.SideToMove(); }
		virtual FString StateHash() const override  { return Inner.StateHash(); }

		virtual bool NextCommand(int32 /*Side*/, FStratAiCommand& OutCommand,
		                         FString& /*OutFailureReason*/) const override
		{
			++DecideCalls;
			OutCommand = Repeated;
			return true;
		}

		virtual bool Submit(const FStratAiCommand& /*Command*/, FString& /*OutFailureReason*/) override
		{
			++SubmitCalls;
			return true;
		}
	};
}

// ---------------------------------------------------------------------------
// T-AI-01 -- A REFUSAL IS NOT A TURN END.
//
// THE FAILURE THIS CLAUSE EXISTS FOR IS SILENT AND WAS WRITTEN DOWN IN ADVANCE.
// `FStratAiCommand`'s default value is `Kind = EndTurn`, so a `NextAiCommand` that refused
// would leave behind a value indistinguishable by inspection from a genuine end of turn. A
// runner that read `Kind` before it read the `bool` would report `bOk = true`,
// `bEndedTurn = true`, no fault anywhere, and hand play back having played nothing.
//
// THE REFUSAL SENTENCE IS THE RULES MODULE'S OWN. It is taken from `NextAiCommand` on a
// bridge that has neither definitions nor a scenario -- a real refusal, on the path
// `StratBridge.h:559-565` documents ("an unseeded bridge holds a default `GameState` with no
// units, and `nextCommand` over it answers EndTurn -- perfectly reasonably, and
// indistinguishably from a turn in which a live AI genuinely has nothing left to do") -- and
// handed to the scripted port. So what the runner is required to forward is a module-side
// string and not one invented here. MEASURED: the sentence that comes back is 'definitions
// are not loaded', which is the FIRST of that method's guards; which of the two guards
// answers is the bridge's business and this clause reads whichever it gives.
//
// THE CONTROL RUNS FIRST AND IT IS LOAD-BEARING. This clause asserts that NO
// `STRAT-AI turn-ended` line was emitted. A capture that could not see one would pass that
// assertion while seeing nothing at all, which is the exact shape that cost phase 6 six
// rounds. So a real AI turn is driven through the SAME live capture before the guarded path
// is touched, and the count of `turn-ended` lines is required to rise by exactly one.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAiRefusalIsNotATurnEndTest,
	"Stratocracy.StratPlay.T-AI-01.RefusalIsNotATurnEnd",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAiRefusalIsNotATurnEndTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiTurnRunnerClauses;

	// The refusal path logs `STRAT-AI refused` at Warning. Declared expected rather than
	// suppressed: it is the line the clause is about.
	AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FStratAiCapture Capture;

	// ---- THE CONTROL: the instrument is shown able to speak -------------------
	FStratBridge Live;
	FString      Error;
	if (!TestTrue(TEXT("the control bridge seeds from the shipped scenario"),
			SeedBridgeWithBuildlist(Live, Error)))
	{
		AddError(Error);
		return false;
	}

	const int32 BeforeControl = Capture.Lines.Num();
	{
		FStratBridgeAiTurnPort ControlPort(&Live);
		FStratAiTurnRunner     ControlRunner;

		const FStratAiTurnOutcome Control = ControlRunner.RunTurn(ControlPort);
		Capture.Settle();

		if (!TestTrue(TEXT("CONTROL: a real AI turn ran to an applied EndTurn"), Control.bOk))
		{
			AddError(Control.FailureReason);
			return false;
		}
		TestTrue(TEXT("CONTROL: the real turn applied at least one command"),
			Control.CommandsApplied > 0);
	}

	Capture.Settle();
	const int32 ControlEnded = Capture.CountFrom(BeforeControl, TEXT("STRAT-AI turn-ended"));
	if (!TestEqual(
			TEXT("CONTROL: the live GLog capture saw exactly one 'STRAT-AI turn-ended' line for the real turn -- "
			     "without this, the silence asserted below would mean nothing"),
			ControlEnded, 1))
	{
		AddError(FString::Printf(TEXT("what the capture actually saw: %s"),
			*Capture.TextFrom(BeforeControl)));
		return false;
	}

	// ---- The module's own refusal sentence ------------------------------------
	FStratBridge Unseeded;
	if (!TestFalse(TEXT("the second bridge is deliberately unseeded"), Unseeded.IsSeeded()))
	{
		return false;
	}

	FStratAiCommand   Untouched;
	const FStratResult ModuleRefusal = Unseeded.NextAiCommand(Unseeded.SideToMove(), Untouched);
	if (!TestFalse(TEXT("NextAiCommand refuses on an unseeded bridge rather than answering EndTurn"),
			ModuleRefusal.bOk))
	{
		return false;
	}
	if (!TestFalse(TEXT("that module-side refusal names a reason"), ModuleRefusal.Reason.IsEmpty()))
	{
		return false;
	}
	AddInfo(FString::Printf(TEXT("the rules module's refusal, forwarded below: '%s'"),
		*ModuleRefusal.Reason));

	// THE HAZARD, STATED AS AN ASSERTION. The value a refusing `NextAiCommand` leaves behind
	// reads as a genuine EndTurn. If this ever stops being true the clause below is testing
	// something easier than it claims to.
	TestTrue(TEXT("the value a refusal leaves behind reads as a genuine EndTurn -- which is why "
	              "`bOk` and not `Kind` is what the runner may branch on"),
		Untouched.Kind == EStratAiCommandKind::EndTurn);

	// ---- THE GUARDED PATH: the AI refuses to answer at all ---------------------
	FStratBridgeAiTurnPort   InnerPort(&Live);
	FStratFaultInjectingAiPort RefusingPort(InnerPort);
	RefusingPort.RefuseDecideAtStep   = 0;
	RefusingPort.InjectedDecideReason = ModuleRefusal.Reason;

	const int32 BeforeGuarded = Capture.Lines.Num();

	FStratAiTurnRunner Runner;
	const FStratAiTurnOutcome Outcome = Runner.RunTurn(RefusingPort);
	Capture.Settle();

	TestFalse(TEXT("a refused NextCommand is not an ok turn"),  Outcome.bOk);
	TestFalse(TEXT("a refused NextCommand is NOT a turn end"),  Outcome.bEndedTurn);
	TestEqual(TEXT("a turn that never got an answer applied no commands"),
		Outcome.CommandsApplied, 0);
	TestFalse(TEXT("the outcome carries a reason"), Outcome.FailureReason.IsEmpty());

	// The forwarding, byte for byte. The right-hand side is the rules module's sentence.
	TestTrue(
		*FString::Printf(TEXT("the outcome's reason ('%s') carries the refusing layer's own words"),
			*Outcome.FailureReason),
		Outcome.FailureReason.Contains(ModuleRefusal.Reason, ESearchCase::CaseSensitive));

	TestEqual(TEXT("the port was asked exactly once before the runner stopped"),
		RefusingPort.DecideCalls, 1);
	TestEqual(TEXT("nothing was submitted after the AI declined to answer"),
		RefusingPort.SubmitCalls, 0);

	// ---- The silence, now that the capture is known to speak -------------------
	TestEqual(
		*FString::Printf(TEXT("no 'STRAT-AI turn-ended' line was emitted for a refused turn (lines seen: %s)"),
			*Capture.TextFrom(BeforeGuarded)),
		Capture.CountFrom(BeforeGuarded, TEXT("STRAT-AI turn-ended")), 0);

	TestEqual(TEXT("nothing was reported as applied either"),
		Capture.CountFrom(BeforeGuarded, TEXT("STRAT-AI applied")), 0);

	TestEqual(TEXT("exactly one refusal line was emitted"),
		Capture.CountFrom(BeforeGuarded, TEXT("STRAT-AI refused")), 1);

	// `phase=decide` sends a reader to the bridge and the vendored AI rather than to the
	// rules module's validation. The three phases are the whole reason the field exists.
	TestEqual(TEXT("the refusal names phase=decide"),
		Capture.CountFrom(BeforeGuarded, TEXT("phase=decide")), 1);

	// And it prints no command, because there was none. `kind=None` appears on a decide
	// refusal only -- see `AiCommandKindName` in the runner's .cpp.
	TestEqual(TEXT("the decide refusal prints kind=None rather than a plausible EndTurn nobody decided"),
		Capture.CountFrom(BeforeGuarded, TEXT("kind=None")), 1);

	return true;
}

// ---------------------------------------------------------------------------
// T-AI-01 -- A REFUSED SUBMISSION STOPS THE TURN WHERE IT STOOD.
//
// THE SECOND OF THE THREE FAULTS A REAL BRIDGE CANNOT PRODUCE, and `StratAiTurnRunner.cpp`
// calls it the most interesting of them: a `phase=apply` refusal is a genuine disagreement
// between §2.9 and §2.5/§2.6 -- the AI named a command the rules module then rejected.
//
// THE STEP IT REFUSES AT IS MEASURED AND NOT CHOSEN. A real AI turn runs first on an
// identically seeded bridge; its `CommandsApplied` is the length of the turn, and the
// injection point is the middle of that. A number written here would be a number that goes
// wrong the moment §2.9 or the scenario changes, and it would silently stop being mid-turn.
//
// WHAT IS THIS FILE'S: the injected refusal SENTENCE. There is no module-side apply refusal
// available without inventing an illegal command -- which would mean writing a hex or a unit
// id into this file, the one thing the lane forbids. So the sentence is the port's own, and
// what is pinned is that the runner FORWARDS it verbatim and stops.
//
// `CommandsApplied` IS ASSERTED AGAINST THE BRIDGE'S OWN `RecordedCommandCount()` as well as
// against the injection step, so "the runner counted n" and "the rules module accepted n"
// are two facts and not one.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAiSubmitRefusalStopsTheTurnTest,
	"Stratocracy.StratPlay.T-AI-01.SubmitRefusalStopsTheTurn",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAiSubmitRefusalStopsTheTurnTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiTurnRunnerClauses;

	AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	// ---- How long a real turn is, measured -----------------------------------
	FStratBridge Measured;
	FString      Error;
	if (!TestTrue(TEXT("the measuring bridge seeds from the shipped scenario"),
			SeedBridgeWithBuildlist(Measured, Error)))
	{
		AddError(Error);
		return false;
	}

	int32 TurnLength = 0;
	{
		FStratBridgeAiTurnPort Port(&Measured);
		FStratAiTurnRunner     Runner;
		const FStratAiTurnOutcome Whole = Runner.RunTurn(Port);
		if (!TestTrue(TEXT("the measured AI turn ran to an applied EndTurn"), Whole.bOk))
		{
			AddError(Whole.FailureReason);
			return false;
		}
		TurnLength = Whole.CommandsApplied;
	}

	// Three is the least length at which "the middle" is neither the first command nor the
	// EndTurn. A shorter opening turn would make this clause about a different property, so
	// it fails rather than quietly testing that.
	if (!TestTrue(
			*FString::Printf(TEXT("§2.9's opening turn is long enough to be refused mid-turn (it applied %d commands)"),
				TurnLength),
			TurnLength >= 3))
	{
		return false;
	}
	const int32 RefuseAt = TurnLength / 2;
	AddInfo(FString::Printf(TEXT("the measured opening turn is %d commands; the submission of command %d is refused"),
		TurnLength, RefuseAt));

	// ---- The same turn, refused in the middle ---------------------------------
	FStratBridge Subject;
	if (!TestTrue(TEXT("the subject bridge seeds identically"),
			SeedBridgeWithBuildlist(Subject, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratAiCapture Capture;
	const int32     Before = Capture.Lines.Num();

	FStratBridgeAiTurnPort     InnerPort(&Subject);
	FStratFaultInjectingAiPort RefusingPort(InnerPort);
	RefusingPort.RefuseSubmitAtStep   = RefuseAt;
	RefusingPort.InjectedSubmitReason =
		TEXT("the scripted port refused this submission so the runner's apply arm could be executed");

	FStratAiTurnRunner Runner;
	const FStratAiTurnOutcome Outcome = Runner.RunTurn(RefusingPort);
	Capture.Settle();

	TestFalse(TEXT("a refused submission is not an ok turn"), Outcome.bOk);
	TestFalse(TEXT("a refused submission is NOT a turn end"), Outcome.bEndedTurn);

	TestEqual(TEXT("the turn stopped at the refused command: exactly the commands before it applied"),
		Outcome.CommandsApplied, RefuseAt);

	// The second, independent reading of the same fact -- the rules module's own count.
	TestEqual(TEXT("the bridge recorded exactly the commands the runner reported applying"),
		Subject.RecordedCommandCount(), Outcome.CommandsApplied);

	TestTrue(
		*FString::Printf(TEXT("the outcome's reason ('%s') carries the refusing layer's own words"),
			*Outcome.FailureReason),
		Outcome.FailureReason.Contains(RefusingPort.InjectedSubmitReason, ESearchCase::CaseSensitive));

	TestEqual(TEXT("the runner stopped asking after the refusal"),
		RefusingPort.DecideCalls, RefuseAt + 1);

	// ---- The log line ---------------------------------------------------------
	TestEqual(
		*FString::Printf(TEXT("exactly one refusal line was emitted (lines seen: %s)"),
			*Capture.TextFrom(Before)),
		Capture.CountFrom(Before, TEXT("STRAT-AI refused")), 1);

	TestEqual(TEXT("the refusal names phase=apply -- the AI named a command the rules module rejected"),
		Capture.CountFrom(Before, TEXT("phase=apply")), 1);

	TestEqual(TEXT("no turn-ended line was emitted for a turn that was stopped"),
		Capture.CountFrom(Before, TEXT("STRAT-AI turn-ended")), 0);

	// The applied lines are the commands that really applied, and the refused one is not
	// among them: `grep "STRAT-AI applied"` must count commands that reached the state.
	TestEqual(TEXT("one 'applied' line per command that actually applied, and none for the refused one"),
		Capture.CountFrom(Before, TEXT("STRAT-AI applied")), Outcome.CommandsApplied);

	// An apply refusal HAS a command, so it must not print the decide refusal's `kind=None`.
	TestEqual(TEXT("the apply refusal prints the command's own kind rather than kind=None"),
		Capture.CountFrom(Before, TEXT("kind=None")), 0);

	return true;
}

// ---------------------------------------------------------------------------
// T-AI-01 -- THE TERMINATION BOUND IS A REPORTED FAULT AND NOT A QUIET STOP.
//
// WHY THE BOUND EXISTS AT ALL. `strat::nextCommand` is a pure function of state, so an AI
// that answered a command which applied without changing the state it read would loop
// forever, taking PIE and the automation suite with it. `MaxCommandsPerTurn` bounds it.
//
// WHY A QUIET STOP WOULD BE WORSE THAN THE LOOP. An AI that mysteriously stops halfway
// through its turn is indistinguishable from an AI that decided to -- so the bound returns
// `bOk = false` with a reason NAMING THE COUNT and logs `STRAT-AI refused phase=bound`.
// That sentence is the thing this clause pins.
//
// THE COMMAND REPEATED FOREVER IS A REAL ONE, taken from `NextAiCommand`; the bound is a
// harness number and the clause asserts the runner reports THAT NUMBER rather than assuming
// it. `MaxCommandsPerTurn = 0` is the second half: a refusal with zero commands, and NOT
// "unbounded" -- an unbounded configuration is the one in which a looping AI hangs the suite
// and it is reachable from a Blueprint default with no compiler diagnostic.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAiLoopBoundIsAReportedFaultTest,
	"Stratocracy.StratPlay.T-AI-01.LoopBoundIsAReportedFault",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAiLoopBoundIsAReportedFaultTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiTurnRunnerClauses;

	AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"),
			SeedBridgeWithBuildlist(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	// A REAL COMMAND TO REPEAT, and it must not be EndTurn or the loop would end on its own
	// merits and the bound would never be reached.
	FStratAiCommand    Repeated;
	const FStratResult Asked = Bridge.NextAiCommand(Bridge.SideToMove(), Repeated);
	if (!TestTrue(TEXT("the AI answers on a seeded bridge"), Asked.bOk))
	{
		AddError(Asked.Reason);
		return false;
	}
	if (!TestTrue(
			*FString::Printf(TEXT("§2.9's opening answer is not itself an EndTurn (it was %s)"),
				KindWord(Repeated.Kind)),
			Repeated.Kind != EStratAiCommandKind::EndTurn))
	{
		return false;
	}

	FStratAiCapture Capture;

	// ---- The bound fires and reports ------------------------------------------
	{
		FStratBridgeAiTurnPort  InnerPort(&Bridge);
		FStratNeverEndingAiPort NeverEnds(InnerPort, Repeated);

		// A HARNESS NUMBER, small so the clause is quick, and it is the number the runner is
		// required to report back. Nothing else in this clause is chosen here.
		const int32 Bound  = 3;
		const int32 Before = Capture.Lines.Num();

		FStratAiTurnRunner Runner;
		Runner.MaxCommandsPerTurn = Bound;

		const FStratAiTurnOutcome Outcome = Runner.RunTurn(NeverEnds);
		Capture.Settle();

		TestFalse(TEXT("exceeding the bound is not an ok turn"), Outcome.bOk);
		TestFalse(TEXT("exceeding the bound is NOT a turn end"), Outcome.bEndedTurn);
		TestEqual(TEXT("everything applied up to the bound stands, and is counted"),
			Outcome.CommandsApplied, Bound);
		TestEqual(TEXT("the runner stopped asking at the bound"), NeverEnds.DecideCalls, Bound);
		TestEqual(TEXT("the runner stopped submitting at the bound"), NeverEnds.SubmitCalls, Bound);

		TestFalse(TEXT("the bound carries a reason"), Outcome.FailureReason.IsEmpty());
		TestTrue(
			*FString::Printf(TEXT("the reason ('%s') names the count"), *Outcome.FailureReason),
			Outcome.FailureReason.Contains(FString::FromInt(Bound), ESearchCase::CaseSensitive));

		TestEqual(
			*FString::Printf(TEXT("exactly one refusal line was emitted (lines seen: %s)"),
				*Capture.TextFrom(Before)),
			Capture.CountFrom(Before, TEXT("STRAT-AI refused")), 1);
		TestEqual(TEXT("the refusal names phase=bound"),
			Capture.CountFrom(Before, TEXT("phase=bound")), 1);
		TestEqual(TEXT("no turn-ended line was emitted for a turn that hit the bound"),
			Capture.CountFrom(Before, TEXT("STRAT-AI turn-ended")), 0);
	}

	// ---- Zero refuses immediately and does not mean "unbounded" ----------------
	{
		FStratBridgeAiTurnPort  InnerPort(&Bridge);
		FStratNeverEndingAiPort NeverEnds(InnerPort, Repeated);

		const int32 Before = Capture.Lines.Num();

		FStratAiTurnRunner Runner;
		Runner.MaxCommandsPerTurn = 0;

		const FStratAiTurnOutcome Outcome = Runner.RunTurn(NeverEnds);
		Capture.Settle();

		TestFalse(TEXT("MaxCommandsPerTurn = 0 refuses"), Outcome.bOk);
		TestFalse(TEXT("MaxCommandsPerTurn = 0 is NOT a turn end"), Outcome.bEndedTurn);
		TestEqual(TEXT("MaxCommandsPerTurn = 0 applies nothing"), Outcome.CommandsApplied, 0);
		TestEqual(TEXT("MaxCommandsPerTurn = 0 does not even ask -- it is a refusal, not an unbounded run"),
			NeverEnds.DecideCalls, 0);
		TestFalse(TEXT("the zero-bound refusal carries a reason"), Outcome.FailureReason.IsEmpty());

		TestEqual(TEXT("the zero bound reports one refusal line"),
			Capture.CountFrom(Before, TEXT("STRAT-AI refused")), 1);
		TestEqual(TEXT("the zero bound reports phase=bound"),
			Capture.CountFrom(Before, TEXT("phase=bound")), 1);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-AI-01 -- EVERY KIND ROUTES TO A RECORDING SUBMIT.
//
// THIS IS THE JOINT PHASE 4'S GATE BLOCKED ON, RE-OPENED FOR THE AI PATH, AND IT IS WRITTEN
// SO IT CANNOT BE SELF-REFERENTIAL. `StratBridge.h:629-643` states the failure of the
// earlier attempt in its own words: the StratPlay clause "drove both bridges through the same
// submission function, so any path -- recording or not -- yielded equal hashes, and its
// command count came from log lines that same function emitted. Subject and witness were one
// object."
//
// SO THERE ARE TWO INDEPENDENT ROUTES AND THREE INDEPENDENT WITNESSES.
//   ROUTE A is `FStratBridgeAiTurnPort::Submit` -- the production switch, the subject.
//   ROUTE B is `SubmitDirect` above -- the engine-typed façade called straight, the
//     comparison. A production arm wired to the wrong method moves one hash and not the
//     other, and the clause says which command and which kind.
//   WITNESS 1 is `StateHash()` on both bridges after every single command.
//   WITNESS 2 is `RecordedCommandCount()`, which no `UE_LOG` can satisfy: a whole AI turn
//     must raise it by EXACTLY the runner's `CommandsApplied`. A non-recording apply path
//     leaves the hashes equal and this number short.
//   WITNESS 3 is `ReplayRecordedLogOnto` a fresh bridge, which must reach an equal hash --
//     the property §4.10's save format actually needs, and the one that fails if the log
//     records something other than what was applied.
//
// ALL FOUR ARMS ARE REQUIRED TO EXECUTE. The four kinds are counted and each is asserted to
// have occurred; a run in which §2.9 never attacked would leave the Attack arm unexecuted
// and this clause FAILS saying so, rather than passing over the gap. That is the whole
// lesson of the phase-4 finding this clause exists to close.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAiEveryKindRoutesToARecordingSubmitTest,
	"Stratocracy.StratPlay.T-AI-01.EveryKindRoutesToARecordingSubmit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAiEveryKindRoutesToARecordingSubmitTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiTurnRunnerClauses;

	FStratBridge Ported;
	FStratBridge Direct;
	FString      Error;
	if (!TestTrue(TEXT("the ported bridge seeds from the shipped scenario"),
			SeedBridgeWithBuildlist(Ported, Error)) ||
		!TestTrue(TEXT("the direct bridge seeds identically"),
			SeedBridgeWithBuildlist(Direct, Error)))
	{
		AddError(Error);
		return false;
	}

	// The starting point, so that "they agree" is known to be true before anything is
	// submitted rather than assumed.
	if (!TestEqualSensitive(TEXT("the two identically seeded bridges start at the same state hash"),
			Ported.StateHash(), Direct.StateHash()))
	{
		return false;
	}

	FStratBridgeAiTurnPort Port(&Ported);

	int32 Builds = 0, Moves = 0, Attacks = 0, EndTurns = 0, Commands = 0;

	while (Commands < kMaxSelfPlayCommands)
	{
		if (MatchHasResult(Ported))
		{
			break;
		}
		if (Builds > 0 && Moves > 0 && Attacks > 0 && EndTurns > 0)
		{
			// Every arm has been executed at least once. The clause's subject is covered and
			// there is nothing to be gained from playing the rest of the game here.
			break;
		}

		// THE ANSWER IS ASKED ONCE AND SUBMITTED TWICE. Both routes receive the same
		// `FStratAiCommand` value -- nothing between the answer and the two submissions.
		const int32     Side = Ported.SideToMove();
		FStratAiCommand C;
		const FStratResult Asked = Ported.NextAiCommand(Side, C);
		if (!Asked.bOk)
		{
			AddError(FString::Printf(
				TEXT("command %d: NextAiCommand(side %d) refused on a seeded, loaded bridge: %s"),
				Commands, Side, *Asked.Reason));
			return false;
		}

		const int32 PortedCountBefore = Ported.RecordedCommandCount();

		FString PortReason;
		const bool bPorted = Port.Submit(C, PortReason);
		if (!bPorted)
		{
			AddError(FString::Printf(
				TEXT("command %d (%s): FStratBridgeAiTurnPort::Submit refused what §2.9 answered: %s"),
				Commands, KindWord(C.Kind), *PortReason));
			return false;
		}

		const FStratResult Applied = SubmitDirect(Direct, C);
		if (!Applied.bOk)
		{
			AddError(FString::Printf(
				TEXT("command %d (%s): the direct façade call refused what the port accepted: %s"),
				Commands, KindWord(C.Kind), *Applied.Reason));
			return false;
		}

		// WITNESS 1, once per command. The two routes must reach the same state.
		if (!TestEqualSensitive(
				*FString::Printf(TEXT("command %d (%s): the port's arm and the direct façade call reach the same state"),
					Commands, KindWord(C.Kind)),
				Ported.StateHash(), Direct.StateHash()))
		{
			return false;
		}

		// WITNESS 2, per command. A submission path that appended twice would produce a log
		// that replays a command twice -- green at the moment of writing, wrong at the moment
		// of loading.
		TestEqual(
			*FString::Printf(TEXT("command %d (%s): the port's submission raised RecordedCommandCount by exactly one"),
				Commands, KindWord(C.Kind)),
			Ported.RecordedCommandCount(), PortedCountBefore + 1);

		switch (C.Kind)
		{
		case EStratAiCommandKind::Build:   ++Builds;   break;
		case EStratAiCommandKind::Move:    ++Moves;    break;
		case EStratAiCommandKind::Attack:  ++Attacks;  break;
		case EStratAiCommandKind::EndTurn: ++EndTurns; break;
		}
		++Commands;
	}

	AddInfo(FString::Printf(TEXT("%d commands through both routes: %d Build, %d Move, %d Attack, %d EndTurn"),
		Commands, Builds, Moves, Attacks, EndTurns));

	// THE COVERAGE ASSERTION, and it is the point of the clause. An arm that never executed
	// is an arm this gate does not cover, and saying so out loud is what phase 4's finding
	// asked for.
	TestTrue(TEXT("the Build arm of FStratBridgeAiTurnPort::Submit executed"),   Builds   > 0);
	TestTrue(TEXT("the Move arm of FStratBridgeAiTurnPort::Submit executed"),    Moves    > 0);
	TestTrue(TEXT("the Attack arm of FStratBridgeAiTurnPort::Submit executed"),  Attacks  > 0);
	TestTrue(TEXT("the EndTurn arm of FStratBridgeAiTurnPort::Submit executed"), EndTurns > 0);

	// ---- WITNESS 2, over a whole runner-driven turn ---------------------------
	FStratBridge Recorded;
	if (!TestTrue(TEXT("the recording bridge seeds from the shipped scenario"),
			SeedBridgeWithBuildlist(Recorded, Error)))
	{
		AddError(Error);
		return false;
	}
	TestEqual(TEXT("a freshly seeded bridge has recorded nothing"),
		Recorded.RecordedCommandCount(), 0);

	FStratBridgeAiTurnPort RecordingPort(&Recorded);
	FStratAiTurnRunner     Runner;

	const int32 CountBefore = Recorded.RecordedCommandCount();
	const FStratAiTurnOutcome Turn = Runner.RunTurn(RecordingPort);
	if (!TestTrue(TEXT("the recorded AI turn ran to an applied EndTurn"), Turn.bOk))
	{
		AddError(Turn.FailureReason);
		return false;
	}
	TestTrue(TEXT("the recorded AI turn applied commands"), Turn.CommandsApplied > 0);

	TestEqual(
		TEXT("a whole AI turn raised RecordedCommandCount by exactly the commands the runner applied -- "
		     "a non-recording apply path is short here and equal everywhere else"),
		Recorded.RecordedCommandCount(), CountBefore + Turn.CommandsApplied);

	// ---- WITNESS 3: what was recorded replays to the same state ---------------
	FStratBridge Fresh;
	if (!TestTrue(TEXT("a fresh bridge seeds from the same scenario"), SeedBridge(Fresh, Error)))
	{
		AddError(Error);
		return false;
	}

	const FStratResult Replayed = Recorded.ReplayRecordedLogOnto(Fresh);
	if (!TestTrue(TEXT("the AI turn's recorded log replays onto a fresh bridge"), Replayed.bOk))
	{
		AddError(Replayed.Reason);
		return false;
	}

	TestEqualSensitive(
		TEXT("replaying what the AI turn recorded reaches the state the AI turn produced"),
		Fresh.StateHash(), Recorded.StateHash());

	TestEqual(TEXT("the replayed bridge ends up recording the same number of commands"),
		Fresh.RecordedCommandCount(), Recorded.RecordedCommandCount());

	return true;
}

// ---------------------------------------------------------------------------
// T-AI-06 -- THE SAME STATE YIELDS THE SAME AI TURN.
//
// §2.9's determinism, at the granularity a turn is actually played at. `strat::nextCommand`
// is a pure function of state, and phase A pinned that one command at a time; this pins that
// the RUNNER -- which asks again after every applied command, deliberately, rather than
// executing a plan decided against an older board -- does not introduce a source of variation
// of its own between two identical starting states.
//
// BOTH SIDES OF EVERY COMPARISON ARE MODULE-SIDE. Two bridges, seeded from the same tables,
// the same scenario file and the same first side, each play one AI turn through their own
// production port; what is compared is their own `StateHash()`, their own
// `RecordedCommandCount()`, and the two outcomes' own `CommandsApplied`. Nothing is
// predicted here.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAiSameStateYieldsSameAiTurnTest,
	"Stratocracy.StratPlay.T-AI-06.SameStateYieldsSameAiTurn",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAiSameStateYieldsSameAiTurnTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiTurnRunnerClauses;

	FStratBridge First;
	FStratBridge Second;
	FString      Error;
	if (!TestTrue(TEXT("the first bridge seeds from the shipped scenario"),
			SeedBridgeWithBuildlist(First, Error)) ||
		!TestTrue(TEXT("the second bridge seeds identically"),
			SeedBridgeWithBuildlist(Second, Error)))
	{
		AddError(Error);
		return false;
	}

	if (!TestEqualSensitive(TEXT("the two bridges start at the same state hash"),
			First.StateHash(), Second.StateHash()))
	{
		return false;
	}

	FStratBridgeAiTurnPort FirstPort(&First);
	FStratBridgeAiTurnPort SecondPort(&Second);

	FStratAiTurnRunner FirstRunner;
	FStratAiTurnRunner SecondRunner;

	const FStratAiTurnOutcome A = FirstRunner.RunTurn(FirstPort);
	const FStratAiTurnOutcome B = SecondRunner.RunTurn(SecondPort);

	if (!TestTrue(TEXT("the first AI turn ran to an applied EndTurn"), A.bOk))
	{
		AddError(A.FailureReason);
		return false;
	}
	if (!TestTrue(TEXT("the second AI turn ran to an applied EndTurn"), B.bOk))
	{
		AddError(B.FailureReason);
		return false;
	}

	TestTrue(TEXT("the turn was not empty -- an equality over nothing proves nothing"),
		A.CommandsApplied > 0);

	TestEqual(TEXT("both turns applied the same number of commands"),
		B.CommandsApplied, A.CommandsApplied);
	TestEqual(TEXT("both turns were played for the same side"), B.Side, A.Side);
	TestEqual(TEXT("both turns were played on the same turn number"), B.Turn, A.Turn);
	TestTrue(TEXT("both turns ended"), A.bEndedTurn && B.bEndedTurn);

	TestEqualSensitive(TEXT("§2.9 is deterministic: both turns reached the same state"),
		Second.StateHash(), First.StateHash());

	TestEqual(TEXT("both bridges recorded the same number of commands"),
		Second.RecordedCommandCount(), First.RecordedCommandCount());

	return true;
}
