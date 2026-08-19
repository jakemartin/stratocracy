// GDD §2.9 / §4.1 / §4.9 -- the match subsystem's three AI clauses. T-INT-05.
//
// WHAT THESE CLAUSES ARE FOR. `UStratMatchSubsystem` gained five configuration fields and
// four methods in phase B, every one of them defaulted so that a hot-seat match behaves
// exactly as it did at 78/78. That default is deliberate and it is also why the suite could
// not tell you whether any of it works: `RunAiTurnsIfDue` had never reached `RunAiTurnsNow`,
// and `RunAiTurnsNow` had never reached `FStratAiTurnRunner::RunTurn`.
//
// THE THREE PROPERTIES.
//   1. THE DEFAULT IS INERT. An empty `AiSides` runs no AI turn, moves no state, records no
//      command and emits no `STRAT-AI` line -- and "nothing is due" is reported as SUCCESS,
//      because it is the ordinary state of a hot-seat game and not an error.
//   2. A CONFIGURATION FAULT IS NOT A DEAD MATCH. An unresolvable `AiBuildlistUnitIds` entry
//      refuses BY NAME and leaves the match live: §4.8's posture is that a bad id is named
//      rather than substituted, and `StratMatchSubsystem.cpp:253-257` states that "the match
//      is seeded and correct; what is wrong is one configured unit id".
//   3. BOTH SIDES AI REACHES A §2.8 RESULT WITHIN THE OUTER BOUND, in one synchronous call,
//      without the handover guard ever firing. That guard exists so a scenario whose turn cap
//      never fires cannot spin forever; a run in which it fires is a run that proved nothing
//      about the game finishing.
//
// THE ABSENCE ASSERTION IN CLAUSE 1 CARRIES A CONTROL AND IT RUNS FIRST. A capture that
// cannot see a line passes a silence assertion by being blind. So the same live `GLog`
// capture is first made to see a REAL AI turn -- `STRAT-AI applied` and
// `STRAT-AI turn-ended` lines, a moved `StateHash()` and a risen `RecordedCommandCount()` --
// and only then is its silence on the default-configured path read as meaning something.
// This cost phase 6 six rounds; the remedy is recorded in `Tools/architect/state.md`.
//
// EVERY STRING COMPARISON IS CASE SENSITIVE. `FString::Contains` and `TestEqual(FString)`
// both ignore case in UE 5.8, so an ordinary comparison of two byte-identical strings is a
// clause that cannot fail. `TestEqualSensitive`, `TestNotEqualSensitive` and
// `ESearchCase::CaseSensitive` throughout.
//
// WHERE THE EXPECTATIONS COME FROM. The AI side is `FStratMatchConfig::FirstSide` -- the
// value handed to the rules module as the first mover -- and the projection is asserted to
// agree with it rather than assumed to. Clause 3's two AI sides are the distinct sides the
// VIEW MODEL reports units for. The buildlist ids are `DT_Units`' own row names, which is
// what `FStratBridge::LoadDefinitions` uses as the id. The state comparisons are
// `FStratBridge::StateHash()` against its own earlier reading. There is no hex, unit id,
// defIndex or turn number written in this file.
//
// IT NAMES NO `strat::` TYPE AND CALLS NO `strat::` FUNCTION.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "CoreGlobals.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "HAL/CriticalSection.h"
#include "Misc/OutputDevice.h"
#include "Misc/OutputDeviceRedirector.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

// Included from a .cpp and never from a UHT-parsed header.
#include "StratBridge.h"

namespace StratAiMatchClauses
{
	static const int32 kFirstSide = 0;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** See `StratMatchLifecycle.cpp` for the same helper and the reason it is duplicated. */
	static bool MakeConfig(FStratMatchConfig& Out, FString& OutError)
	{
		Out.UnitTable    = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		Out.TerrainTable = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
		if (Out.UnitTable == nullptr || Out.TerrainTable == nullptr)
		{
			OutError = TEXT("DT_Units and/or DT_Terrain did not load from /Game/StratData");
			return false;
		}

		Out.ScenarioFile    = TEXT("Data/ferrum_crossing.json");
		Out.FirstSide       = kFirstSide;
		Out.ViewingSide     = kFirstSide;
		Out.BoardActorClass = AStratBoardActor::StaticClass();
		Out.UnitActorClass  = AStratUnitActor::StaticClass();
		return true;
	}

	/**
	 * §4.8's unit ids, read off the table the match is configured with.
	 *
	 * `FStratBridge::LoadDefinitions` takes the row NAME as the id ("the row NAME is the id:
	 * FUnitRow carries no Id field", `StratBridge.cpp:63-65`), so these are the same strings
	 * the definitions vector was built from. A hand-written list here would be this file
	 * deciding §4.8's id space.
	 */
	static TArray<FName> UnitIdsFrom(const FStratMatchConfig& Config)
	{
		return Config.UnitTable != nullptr ? Config.UnitTable->GetRowNames() : TArray<FName>();
	}

	/** See `StratMatchLifecycle.cpp` on why `InitializeActorsForPlay` is not decoration and
	 *  why the world is destroyed before its context. */
	struct FTestWorldScope
	{
		UWorld* World = nullptr;

		FTestWorldScope()
		{
			World = UWorld::CreateWorld(EWorldType::Game, /*bInformEngineOfWorld*/ false);
			if (World != nullptr && GEngine != nullptr)
			{
				FWorldContext& Context = GEngine->CreateNewWorldContext(EWorldType::Game);
				Context.SetCurrentWorld(World);
				World->InitializeActorsForPlay(FURL());
			}
		}

		~FTestWorldScope()
		{
			if (World != nullptr)
			{
				World->DestroyWorld(false);
				if (GEngine != nullptr)
				{
					GEngine->DestroyWorldContext(World);
				}
				World = nullptr;
			}
		}

		FTestWorldScope(const FTestWorldScope&) = delete;
		FTestWorldScope& operator=(const FTestWorldScope&) = delete;
	};

	/** The same device `StratAiTurnRunnerClauses.cpp` uses, and it is never the load-bearing
	 *  witness on its own -- see this file's header block on the control.
	 *
	 *  UNBUFFERED FOR THE REASON THAT FILE'S COPY RECORDS AT LENGTH: without the override the
	 *  redirector can hand a device lines emitted BEFORE it was constructed, because its
	 *  buffered queue is drained to whichever devices are registered at DRAIN time rather than
	 *  at emission time (OutputDeviceRedirector.cpp:937 and :553). The `LinesBefore` watermarks
	 *  below bound by index and do not close that. Measured as a 1-in-4 failure of
	 *  `T-UI-01.ClickedAttackIsAcceptedAndRecorded` on byte-identical code, 2026-08-14.
	 *
	 *  THE LOCK COVERS THE APPEND AND NOTHING ELSE, AND THAT RESIDUAL IS DELIBERATE RATHER
	 *  THAN OVERLOOKED. The override advertises to the redirector that this device needs no
	 *  external locking, and the engine takes it literally -- every thread's log lines now
	 *  arrive in `Serialize` directly, so the one mutation of `Lines` is made under `Mutex`.
	 *  Every READ below -- `CountFrom`, `TextFrom`, and each direct `Lines.Num()` /
	 *  `Lines[I]` at a call site -- is UNLOCKED. That is safe only because every `STRAT-AI`
	 *  emitter runs on the game thread and every reader here is on the game thread too, so
	 *  no read is ever concurrent with the append. NOTHING IN THE CODE PINS THAT PROPERTY:
	 *  it is a fact about the current emitters, not an invariant anyone enforces. Move a
	 *  `STRAT-AI` `UE_LOG` onto a worker thread and these reads become a data race that no
	 *  clause here would report -- it would surface as a flake, not as a red test. Widening
	 *  the lock to cover the reads is the fix if that day ever comes.
	 *
	 *  THE FILTER IS `ESearchCase::CaseSensitive`, as is every string comparison in this
	 *  file -- see the header block. `StartsWith` / `Contains` / `TestEqual` all default to
	 *  IgnoreCase in UE 5.8, and a case-insensitive match on a fixed-field log line is a
	 *  comparison that risks not being able to fail. */
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

		/** Kept, and no longer load-bearing -- an unbuffered device is already up to date. */
		void Settle()
		{
			if (GLog != nullptr)
			{
				GLog->Flush();
			}
		}

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
}

// ---------------------------------------------------------------------------
// T-INT-05 -- AN EMPTY `AiSides` RUNS NO AI TURN, AND SAYS SO BY SUCCEEDING.
//
// THE PROPERTY THAT MADE PHASE B SAFE TO LAND. Every §2.9 field on `FStratMatchConfig`
// defaults so that "the game phases 0-6 shipped" is what a match without an AI configuration
// still is. This clause is what makes that a fact about the code rather than a claim in a
// comment: with the defaults, `RunAiTurnsIfDue` returns TRUE -- nothing due is ordinary and
// not a refusal -- while the state hash, the recorded command count and the `STRAT-AI` log
// are all exactly where they were.
//
// THE CONTROL IS THE FIRST HALF OF THE CLAUSE AND IT IS NOT OPTIONAL. Three of the four
// assertions on the guarded path are assertions that NOTHING happened, and every one of them
// would pass on a `RunAiTurnsIfDue` that had been deleted, on a capture that saw no lines,
// and on a subsystem with no bridge. So the same subsystem is first configured WITH an AI
// side and made to play a real turn through the same live capture, and every one of those
// four readings is required to MOVE. Only then is the silence read.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAiEmptyAiSidesRunsNoAiTurnTest,
	"Stratocracy.StratPlay.T-INT-05.EmptyAiSidesRunsNoAiTurn",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAiEmptyAiSidesRunsNoAiTurnTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiMatchClauses;

	// This fixture has no Blueprint defaults, so the board reports every terrain kind as
	// unmeshed and `StartMatch` returns false with the match LIVE. Declared expected rather
	// than suppressed; `StratMatchReconcile.cpp` records why that is the documented
	// presentation split and not a failed match.
	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	// NO EXPECTATION FOR `Match seeded from`, AND THAT IS A MEASUREMENT. `Occurrences = 0`
	// means "one or more", not "any number including none" -- declared here it FAILED with
	// "expected ... did not occur". `StartMatch` logs that Warning only when it has a
	// complaint to make, and in this fixture it has none: the unmeshed terrain is reported by
	// the board actor and is not a complaint, so `StartMatch` returns true here.
	FStratAiCapture Capture;

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Subsystem))
	{
		return false;
	}

	FStratMatchConfig Base;
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Base, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- THE CONTROL: the same subsystem, the same capture, an AI side ---------
	{
		FStratMatchConfig WithAi = Base;

		// THE SIDE COMES FROM THE CONFIGURATION AND IS THEN CHECKED AGAINST THE PROJECTION.
		// `FirstSide` is what the rules module was told moves first; asserting the view model
		// agrees is what makes "this side is the one to move" a module fact here.
		WithAi.AiSides.Add(WithAi.FirstSide);
		WithAi.AiBuildlistUnitIds = UnitIdsFrom(WithAi);

		FString StartReason;
		Subsystem->StartMatch(WithAi, StartReason);
		if (!TestTrue(TEXT("CONTROL: the AI-configured match is live"), Subsystem->IsMatchLive()))
		{
			AddError(StartReason);
			return false;
		}

		const FStratBridge* const Live = Subsystem->GetBridge();
		if (!TestNotNull(TEXT("CONTROL: the live match has a bridge"), Live))
		{
			return false;
		}

		FStratViewModel Model;
		if (!TestTrue(TEXT("CONTROL: the view model builds"),
				Subsystem->BuildViewModel(Model, Error)))
		{
			AddError(Error);
			return false;
		}
		TestEqual(TEXT("CONTROL: the configured first side is the side to move at the seeded opening"),
			Model.Match.SideToMove, WithAi.FirstSide);

		TestTrue(TEXT("CONTROL: the subsystem calls that side an AI side"),
			Subsystem->IsSideAi(WithAi.FirstSide));
		TestTrue(TEXT("CONTROL: an AI turn is due"), Subsystem->IsAiTurnDue());

		const int32   LinesBefore = Capture.Lines.Num();
		const FString HashBefore  = Live->StateHash();
		const int32   CountBefore = Live->RecordedCommandCount();

		FString RunReason;
		const bool bRan = Subsystem->RunAiTurnsIfDue(RunReason);
		Capture.Settle();

		if (!TestTrue(TEXT("CONTROL: the due AI turn ran"), bRan))
		{
			AddError(RunReason);
			return false;
		}

		// The four readings the guarded path will assert are UNMOVED. Every one must move
		// here, or its stillness below means nothing.
		if (!TestTrue(
				*FString::Printf(TEXT("CONTROL: the live GLog capture saw the AI's turn (lines: %s)"),
					*Capture.TextFrom(LinesBefore)),
				Capture.CountFrom(LinesBefore, TEXT("STRAT-AI applied")) > 0))
		{
			return false;
		}
		if (!TestEqual(TEXT("CONTROL: exactly one AI turn ended -- the other side is human, so the loop stopped"),
				Capture.CountFrom(LinesBefore, TEXT("STRAT-AI turn-ended")), 1))
		{
			return false;
		}
		if (!TestNotEqualSensitive(TEXT("CONTROL: the AI's turn moved the state hash"),
				Live->StateHash(), HashBefore))
		{
			return false;
		}
		if (!TestTrue(TEXT("CONTROL: the AI's commands were recorded"),
				Live->RecordedCommandCount() > CountBefore))
		{
			return false;
		}

		// And the hand-back: after the AI's turn it is a human's turn, so nothing more is due.
		TestFalse(TEXT("CONTROL: no further AI turn is due once play returns to the human side"),
			Subsystem->IsAiTurnDue());
	}

	// ---- THE GUARDED PATH: the shipped defaults --------------------------------
	{
		// `Base` carries the defaults: `AiSides` empty, `AiBuildlistUnitIds` empty. This
		// reseeds the same subsystem -- documented behaviour, `StartMatch` is not idempotent
		// and a second call reseeds.
		FString StartReason;
		Subsystem->StartMatch(Base, StartReason);
		if (!TestTrue(TEXT("the default-configured match is live"), Subsystem->IsMatchLive()))
		{
			AddError(StartReason);
			return false;
		}

		const FStratBridge* const Live = Subsystem->GetBridge();
		if (!TestNotNull(TEXT("the default-configured match has a bridge"), Live))
		{
			return false;
		}

		TestEqual(TEXT("no side is configured as AI"), Base.AiSides.Num(), 0);
		TestFalse(TEXT("the first side is not an AI side under the defaults"),
			Subsystem->IsSideAi(Base.FirstSide));
		TestFalse(TEXT("no AI turn is due under the defaults"), Subsystem->IsAiTurnDue());

		const int32   LinesBefore = Capture.Lines.Num();
		const FString HashBefore  = Live->StateHash();
		const int32   CountBefore = Live->RecordedCommandCount();

		FString RunReason;
		const bool bRan = Subsystem->RunAiTurnsIfDue(RunReason);
		Capture.Settle();

		// "IT IS THE HUMAN'S TURN" IS A SUCCESS. A refusal here would make every caller --
		// the GameMode's BeginPlay and the PlayerController after every command -- report a
		// fault for the ordinary state of a hot-seat game.
		TestTrue(TEXT("nothing due is reported as success, not as a refusal"), bRan);
		TestTrue(*FString::Printf(TEXT("and it carries no reason (it said: '%s')"), *RunReason),
			RunReason.IsEmpty());

		TestEqual(
			*FString::Printf(TEXT("no STRAT-AI line was emitted at all (lines seen: %s)"),
				*Capture.TextFrom(LinesBefore)),
			Capture.CountFrom(LinesBefore, TEXT("STRAT-AI")), 0);

		TestEqualSensitive(TEXT("the state hash is exactly where it was"),
			Live->StateHash(), HashBefore);
		TestEqual(TEXT("no command was recorded"), Live->RecordedCommandCount(), CountBefore);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-INT-05 -- AN UNRESOLVABLE BUILDLIST ID REFUSES BY NAME AND LEAVES THE MATCH LIVE.
//
// §4.8'S POSTURE, AT THE ONE PLACE A DESIGNER CAN GET IT WRONG FROM A BLUEPRINT DEFAULT.
// `SetBuildlistByIds` "refuses on any unresolvable id AND NAMES IT, never substituting a
// default" -- because a silently-dropped entry would change §2.9's build MIX, which is data
// with no other witness in the tree. The subsystem's part is to forward that refusal without
// tearing down a match that is seeded and correct.
//
// WHAT IS AND IS NOT DISCRIMINATING HERE, STATED PLAINLY. `StartMatch` returns FALSE in this
// fixture WHATEVER happens, because the test world has no Blueprint defaults and so every
// terrain kind is unmeshed -- the documented presentation split. So the false return is
// asserted for the record and is NOT the clause. The discriminating facts are three:
// `IsMatchLive()` is true, the reason NAMES the offending id byte for byte, and -- the
// control -- the same config with a resolvable list produces a reason that does not mention
// the buildlist at all and a bridge whose `BuildlistDefIndexes()` has one entry per id.
//
// `BuildlistDefIndexes()` IS THE MODULE-SIDE READER THAT MAKES BOTH HALVES REAL. On the
// control it must be as long as the list configured; on the refusal it must be EMPTY, which
// is `SetBuildlistByIds`' all-or-nothing promise seen from the outside.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAiBuildlistRefusalLeavesTheMatchLiveTest,
	"Stratocracy.StratPlay.T-INT-05.BuildlistRefusalLeavesTheMatchLive",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAiBuildlistRefusalLeavesTheMatchLiveTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiMatchClauses;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
	AddExpectedMessagePlain(TEXT("Match seeded from"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Subsystem))
	{
		return false;
	}

	FStratMatchConfig Base;
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Base, Error)))
	{
		AddError(Error);
		return false;
	}
	Base.AiSides.Add(Base.FirstSide);

	const TArray<FName> GoodIds = UnitIdsFrom(Base);
	if (!TestTrue(TEXT("DT_Units carries rows to build a buildlist from"), GoodIds.Num() > 0))
	{
		return false;
	}

	// ---- THE CONTROL: a resolvable buildlist -----------------------------------
	{
		FStratMatchConfig Good = Base;
		Good.AiBuildlistUnitIds = GoodIds;

		FString StartReason;
		Subsystem->StartMatch(Good, StartReason);

		if (!TestTrue(TEXT("CONTROL: the match with a resolvable buildlist is live"),
				Subsystem->IsMatchLive()))
		{
			AddError(StartReason);
			return false;
		}

		const FStratBridge* const Live = Subsystem->GetBridge();
		if (!TestNotNull(TEXT("CONTROL: the live match has a bridge"), Live))
		{
			return false;
		}

		// THE CONTROL'S LOAD-BEARING HALF: the list really resolved, in the module's own
		// reader. Without this the refusal clause below could pass on a subsystem that never
		// calls `SetBuildlistByIds` at all.
		TestEqual(TEXT("CONTROL: §2.9's buildlist resolved to one defIndex per configured id"),
			Live->BuildlistDefIndexes().Num(), GoodIds.Num());

		TestFalse(
			*FString::Printf(TEXT("CONTROL: a resolvable buildlist produces no buildlist complaint (it said: '%s')"),
				*StartReason),
			StartReason.Contains(TEXT("SetBuildlistByIds"), ESearchCase::CaseSensitive));
	}

	// ---- THE REFUSAL: one unresolvable id, in the middle -----------------------
	// IN THE MIDDLE rather than at the end, on `StratAiBridgeParity.cpp`'s reasoning: a
	// trailing bad entry is the one arrangement where "prefix applied" and "nothing applied"
	// look alike from outside.
	const FName Nonsense(TEXT("NoSuchUnitIdInDT_Units"));

	FStratMatchConfig Poisoned = Base;
	Poisoned.AiBuildlistUnitIds = GoodIds;
	Poisoned.AiBuildlistUnitIds.Insert(Nonsense, GoodIds.Num() / 2);

	FString StartReason;
	const bool bStarted = Subsystem->StartMatch(Poisoned, StartReason);

	// Asserted for the record. See this clause's header block: in this fixture a good config
	// also returns false, so this line is NOT what separates the two cases.
	TestFalse(TEXT("StartMatch reports the configuration fault by returning false"), bStarted);

	// THE CLAUSE. The match is seeded and correct; one configured id is not.
	TestTrue(TEXT("the match is LIVE -- a bad buildlist id is a diminished match, not no match"),
		Subsystem->IsMatchLive());

	const FStratBridge* const Live = Subsystem->GetBridge();
	if (!TestNotNull(TEXT("the live match still has a bridge"), Live))
	{
		return false;
	}

	AddInfo(FString::Printf(TEXT("the refusal, forwarded from the bridge: '%s'"), *StartReason));

	TestTrue(
		*FString::Printf(TEXT("the reason names the offending id '%s' (it said: '%s')"),
			*Nonsense.ToString(), *StartReason),
		StartReason.Contains(Nonsense.ToString(), ESearchCase::CaseSensitive));

	TestTrue(
		*FString::Printf(TEXT("the reason names the call that refused (it said: '%s')"), *StartReason),
		StartReason.Contains(TEXT("SetBuildlistByIds"), ESearchCase::CaseSensitive));

	// ALL-OR-NOTHING, seen from outside: nothing was stored, not even the resolvable prefix.
	TestEqual(TEXT("a refused buildlist leaves §2.9 with no buildlist at all, not a prefix of one"),
		Live->BuildlistDefIndexes().Num(), 0);

	// And the match is playable: the view model builds and it is the configured side's turn.
	FStratViewModel Model;
	if (TestTrue(TEXT("the view model still builds for the live match"),
			Subsystem->BuildViewModel(Model, Error)))
	{
		TestEqual(TEXT("and it is the configured first side's turn"),
			Model.Match.SideToMove, Poisoned.FirstSide);
		TestFalse(TEXT("and the match has not been given a result by the configuration fault"),
			Model.Match.bHasResult);
	}
	else
	{
		AddError(Error);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-INT-05 -- BOTH SIDES AI REACHES A RESULT WITHIN THE OUTER BOUND.
//
// PHASE D'S GATE CONFIGURATION, RUN ONCE HERE AS A CLAUSE. `AiSides` naming both sides is a
// legal value and the thing the handover loop was written for; `AiMaxConsecutiveTurns` exists
// so that a scenario whose turn cap never fires cannot spin forever inside one call.
//
// THE `phase=handover` ABSENCE IS THE POINT OF THE CLAUSE. That line's CALL SITE is
// `RunAiTurnsNow` -- not the runner -- reached when the outer bound is reached with no result
// and no human turn, and it means the game did NOT finish. (The FORMAT it goes out through is
// `StratAiTurnRunner.cpp`'s and only that file's; `RunAiTurnsNow` reaches it by calling
// `StratLogAiTurnRefusal`. Its fields are pinned by `.T-INT-05.HandoverRefusalCarriesTheFixedFields`
// at the bottom of this file, which is the control for the silence asserted here.) A clause that only asserted
// "RunAiTurnsNow returned true" would be green on a run that spun to the bound, because the
// bound's own stop reason is a refusal the return value reports and the tester might read as
// a §2.8 result. So three facts are asserted together: the match has a result, the number of
// AI turns actually played is strictly under the bound, and no handover refusal was emitted.
//
// THE TURN COUNT IS READ OFF THE `STRAT-AI turn-ended` LINES, and that reading is safe here
// for one reason: the same capture in the same clause is required to have seen `applied`
// lines and a result to exist, so it is not being asked to prove a silence it could satisfy
// by being blind. `grep -c "STRAT-AI turn-ended"` counting AI turns is the property
// `StratAiTurnRunner.cpp` gives that third phrase for.
//
// THE TWO SIDES ARE THE VIEW MODEL'S OWN. They are the distinct `Side` values the projection
// reports units for, so a scenario re-authored with a different side count moves this clause
// with it instead of turning a real side into a literal.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAiBothSidesAiReachesAResultWithinTheBoundTest,
	"Stratocracy.StratPlay.T-INT-05.BothSidesAiReachesAResultWithinTheBound",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAiBothSidesAiReachesAResultWithinTheBoundTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiMatchClauses;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	// THIS EXPECTATION IS A TRIPWIRE AND IT DOCUMENTS A MEASURED PRODUCTION BEHAVIOUR RATHER
	// THAN ONE THIS CLAUSE ENDORSES. Measured on the first run of this file: an AI-vs-AI game
	// that reaches a §2.8 result MID-TURN ends with the rules module refusing the winning
	// side's closing EndTurn --
	//
	//   STRAT-AI refused phase=apply kind=EndTurn ... turn=6 side=1 reason=[T-SAVE-05] no match is running
	//
	// -- so `FStratAiTurnRunner::RunTurn` reports `bOk = false` and `RunAiTurnsNow` returns
	// FALSE for a game that finished correctly. The result, the turn count and the handover
	// silence below are all as they should be; what is wrong is that FINISHING is reported as
	// a fault, and telling that apart from a genuine mid-game refusal costs a reader the
	// refusal's own text. It is a production-side finding, filed rather than fixed here --
	// `Tests/` is this agent's lane and `StratAiTurnRunner.cpp` is not.
	//
	// `Occurrences = 0` means ONE OR MORE, so if that behaviour is later corrected THIS LINE
	// FAILS and the clause is revisited deliberately instead of quietly keeping a suppression
	// for a warning that no longer exists.
	AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Subsystem))
	{
		return false;
	}

	FStratMatchConfig Base;
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Base, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- Which sides exist, asked of the projection ----------------------------
	TArray<int32> Sides;
	{
		FString StartReason;
		Subsystem->StartMatch(Base, StartReason);
		if (!TestTrue(TEXT("the scouting match is live"), Subsystem->IsMatchLive()))
		{
			AddError(StartReason);
			return false;
		}

		FStratViewModel Model;
		if (!TestTrue(TEXT("the view model builds"), Subsystem->BuildViewModel(Model, Error)))
		{
			AddError(Error);
			return false;
		}
		for (const FStratUnitView& Unit : Model.Units)
		{
			Sides.AddUnique(Unit.Side);
		}
		Sides.Sort();
	}

	if (!TestTrue(
			*FString::Printf(TEXT("the seeded scenario deploys units for more than one side (it deploys for %d)"),
				Sides.Num()),
			Sides.Num() >= 2))
	{
		return false;
	}

	// ---- Both sides AI, one synchronous call -----------------------------------
	FStratMatchConfig BothAi = Base;
	BothAi.AiSides            = Sides;
	BothAi.AiBuildlistUnitIds = UnitIdsFrom(BothAi);

	FStratAiCapture Capture;

	FString StartReason;
	Subsystem->StartMatch(BothAi, StartReason);
	if (!TestTrue(TEXT("the AI-vs-AI match is live"), Subsystem->IsMatchLive()))
	{
		AddError(StartReason);
		return false;
	}

	for (const int32 Side : Sides)
	{
		TestTrue(*FString::Printf(TEXT("side %d is configured as AI"), Side),
			Subsystem->IsSideAi(Side));
	}

	const int32 LinesBefore = Capture.Lines.Num();

	FString    RunReason;
	const bool bRan = Subsystem->RunAiTurnsNow(RunReason);
	Capture.Settle();

	// THE RETURN VALUE IS RECORDED AND DELIBERATELY NOT ASSERTED, and the reason is the
	// finding declared at the top of this clause: a completed AI-vs-AI game currently comes
	// back FALSE, because the closing EndTurn of the turn that produced the result is refused
	// by the rules module with "no match is running". Asserting `true` here would fail on a
	// correct game; asserting `false` would make this clause DEMAND the defect and turn a
	// future fix into a red suite. The three facts below are what "reached a result within
	// the bound" actually means, and none of them depends on the return value.
	AddInfo(FString::Printf(TEXT("RunAiTurnsNow returned %s; reason: '%s'"),
		bRan ? TEXT("true") : TEXT("false"), *RunReason));

	// ---- The three facts -------------------------------------------------------
	FStratViewModel Model;
	if (!TestTrue(TEXT("the view model builds after the game"),
			Subsystem->BuildViewModel(Model, Error)))
	{
		AddError(Error);
		return false;
	}

	const int32 TurnsRun = Capture.CountFrom(LinesBefore, TEXT("STRAT-AI turn-ended"));
	const int32 Applied  = Capture.CountFrom(LinesBefore, TEXT("STRAT-AI applied"));
	AddInfo(FString::Printf(TEXT("%d AI turns, %d commands, bound is %d; result: %s"),
		TurnsRun, Applied, BothAi.AiMaxConsecutiveTurns,
		Model.Match.bHasResult ? TEXT("yes") : TEXT("no")));

	// The capture is required to have SEEN the game, so the handover silence below is not a
	// silence it could satisfy by being blind.
	TestTrue(TEXT("the capture saw the AI's commands"), Applied > 0);
	TestTrue(TEXT("the capture saw whole AI turns end"), TurnsRun > 0);

	TestTrue(TEXT("§2.8: the AI-vs-AI game reached a result under its own steam"),
		Model.Match.bHasResult);

	TestTrue(
		*FString::Printf(TEXT("it reached that result in %d AI turns, strictly inside the outer bound of %d"),
			TurnsRun, BothAi.AiMaxConsecutiveTurns),
		TurnsRun < BothAi.AiMaxConsecutiveTurns);

	TestEqual(
		*FString::Printf(TEXT("the handover guard never fired (STRAT-AI lines seen at the end: %s)"),
			*Capture.TextFrom(FMath::Max(LinesBefore, Capture.Lines.Num() - 4))),
		Capture.CountFrom(LinesBefore, TEXT("phase=handover")), 0);

	// And once there is a result, nothing further is due -- an AI that kept playing past a
	// §2.8 result would submit one refused command per turn, forever.
	TestFalse(TEXT("no AI turn is due once the match has a result"), Subsystem->IsAiTurnDue());

	return true;
}

// ---------------------------------------------------------------------------
// The `STRAT-AI refused` splitter, mirroring `Tools/architect/strat_ai_log_gate.py`'s
// `_REFUSED_RE` field for field.
//
// IT SPLITS AND DOES NOT EXPECT. Nothing below decides what a field SHOULD say; it only turns
// one log line into named values, so the clause can compare each of them against something the
// module produced. The field NAMES and their ORDER are the assertion this helper does make --
// `phase kind unit hex def target turn side reason`, each token required to carry its own key --
// because that order is precisely what the phase-D parser's anchored regex depends on, and a
// reordered format string would slide every value one field to the left with no other symptom.
//
// THE PYTHON PARSER'S ATTRIBUTE NAMES ARE REUSED VERBATIM (`hex_x`/`hex_y` become `HexX`/`HexY`,
// `def_index` becomes `DefIndex`) so a reader can go line -> dataclass attribute -> this struct
// with no translation table. That single shared shape is the entire reason this clause was
// deferred from phase B to phase D: written earlier it would have spelled the field shape a
// second time, before the parser that owns it existed.
// ---------------------------------------------------------------------------
namespace StratAiMatchClauses
{
	struct FStratAiRefusedFields
	{
		FString Phase;
		FString Kind;
		int32   Unit     = 0;
		int32   HexX     = 0;
		int32   HexY     = 0;
		int32   DefIndex = 0;
		int32   Target   = 0;
		int32   Turn     = 0;
		int32   Side     = 0;
		FString Reason;
	};

	static bool ParseInt(const FString& Token, int32& Out, FString& OutError)
	{
		// `Atoi` answers 0 for anything unparseable, so the numeric check comes FIRST -- without
		// it a field that had turned into a word would read as a plausible `0`.
		if (!Token.IsNumeric())
		{
			OutError = FString::Printf(TEXT("'%s' is not an integer"), *Token);
			return false;
		}
		Out = FCString::Atoi(*Token);
		return true;
	}

	static bool TakeKeyed(const TArray<FString>& Tokens, int32 Index, const TCHAR* Key,
	                      FString& OutValue, FString& OutError)
	{
		if (!Tokens.IsValidIndex(Index))
		{
			OutError = FString::Printf(TEXT("no field %d, expected '%s='"), Index, Key);
			return false;
		}

		const FString Expected = FString(Key) + TEXT("=");
		if (!Tokens[Index].StartsWith(Expected, ESearchCase::CaseSensitive))
		{
			OutError = FString::Printf(TEXT("field %d is '%s', expected it to start '%s'"),
				Index, *Tokens[Index], *Expected);
			return false;
		}

		OutValue = Tokens[Index].RightChop(Expected.Len());
		return true;
	}

	static bool ParseRefusedLine(const FString& Line, FStratAiRefusedFields& Out, FString& OutError)
	{
		static const FString Prefix(TEXT("STRAT-AI refused "));
		static const FString ReasonKey(TEXT(" reason="));

		if (!Line.StartsWith(Prefix, ESearchCase::CaseSensitive))
		{
			OutError = FString::Printf(TEXT("does not begin '%s'"), *Prefix);
			return false;
		}

		const FString Rest      = Line.RightChop(Prefix.Len());
		const int32   ReasonAt  = Rest.Find(ReasonKey, ESearchCase::CaseSensitive,
			ESearchDir::FromStart);
		if (ReasonAt == INDEX_NONE)
		{
			OutError = FString::Printf(TEXT("carries no '%s' field"), *ReasonKey);
			return false;
		}

		// `reason=` IS LAST AND TAKES THE REST OF THE LINE, exactly as the parser's `(?P<reason>.+)$`
		// does: it is the one free-text field and it contains spaces in every real refusal.
		Out.Reason = Rest.RightChop(ReasonAt + ReasonKey.Len());

		TArray<FString> Tokens;
		Rest.Left(ReasonAt).ParseIntoArray(Tokens, TEXT(" "), /*InCullEmpty=*/true);

		FString HexPair;
		if (!TakeKeyed(Tokens, 0, TEXT("phase"),  Out.Phase, OutError) ||
		    !TakeKeyed(Tokens, 1, TEXT("kind"),   Out.Kind,  OutError) ||
		    !TakeKeyed(Tokens, 3, TEXT("hex"),    HexPair,   OutError))
		{
			return false;
		}

		FString UnitText, DefText, TargetText, TurnText, SideText;
		if (!TakeKeyed(Tokens, 2, TEXT("unit"),   UnitText,   OutError) ||
		    !TakeKeyed(Tokens, 4, TEXT("def"),    DefText,    OutError) ||
		    !TakeKeyed(Tokens, 5, TEXT("target"), TargetText, OutError) ||
		    !TakeKeyed(Tokens, 6, TEXT("turn"),   TurnText,   OutError) ||
		    !TakeKeyed(Tokens, 7, TEXT("side"),   SideText,   OutError))
		{
			return false;
		}

		if (Tokens.Num() != 8)
		{
			OutError = FString::Printf(
				TEXT("expected 8 keyed fields before 'reason=', found %d"), Tokens.Num());
			return false;
		}

		FString HexXText, HexYText;
		if (!HexPair.Split(TEXT(","), &HexXText, &HexYText))
		{
			OutError = FString::Printf(TEXT("hex field '%s' is not 'x,y'"), *HexPair);
			return false;
		}

		return ParseInt(UnitText,   Out.Unit,     OutError)
		    && ParseInt(HexXText,   Out.HexX,     OutError)
		    && ParseInt(HexYText,   Out.HexY,     OutError)
		    && ParseInt(DefText,    Out.DefIndex, OutError)
		    && ParseInt(TargetText, Out.Target,   OutError)
		    && ParseInt(TurnText,   Out.Turn,     OutError)
		    && ParseInt(SideText,   Out.Side,     OutError);
	}
}

// ---------------------------------------------------------------------------
// T-INT-05 -- THE HANDOVER REFUSAL CARRIES THE FIXED FIELDS.
//
// THE DEBT THIS DISCHARGES, AND WHY IT WAITED. Phase B's re-gate deferred this clause to phase D
// on one condition: phase D owns the field parser (`Tools/architect/strat_ai_log_gate.py`), and
// writing the clause earlier would have spelled the expected field shape a second time, in a
// second place, with nothing keeping the two in step. The clause is written against that
// parser's `StratAiRefused` shape and reuses its attribute names.
//
// WHAT WAS DARK BEFORE IT. `.BothSidesAiReachesAResultWithinTheBound` asserts the handover
// line's ABSENCE and nothing else, so the `handover` arm and its one call site --
// `StratMatchSubsystem.cpp:724`'s `StratLogAiTurnRefusal` and the forward it goes through --
// were executed by no test at all. Three of the shared formatter's four `phase` values were
// already executed by the runner's own clauses; this is the fourth, and it is the only one whose
// caller lives outside `StratAiTurnRunner.cpp`. An absence assertion over a line no test has
// ever seen produced is a silence with no instrument behind it -- THIS CLAUSE IS THAT
// INSTRUMENT, and it is what makes the other clause's zero mean something.
//
// HOW THE ARM IS DRIVEN, AND A CORRECTION TO THE BRIEF THAT ASKED FOR THIS. No scripted port is
// used and none is needed: `phase=handover` is NOT one of `FStratAiTurnRunner`'s three fault
// arms, which is why it is not reachable through the runner's port at all. It is the
// SUBSYSTEM's outer bound, and `RunAiTurnsNow` reaches it after turns that all SUCCEEDED
// (`StopReason.IsEmpty() && TurnsRun >= MaxTurns`). So the production path is driven whole:
// both sides AI -- the configuration the guard was written for -- and `AiMaxConsecutiveTurns`
// lowered to 1, which is a `FStratMatchConfig` field a Blueprint default can set. The AI never
// refuses anything here; it simply never gives the board back inside the bound, which is the
// literal meaning of the phase.
//
// WHERE EVERY EXPECTED VALUE COMES FROM -- the point of the clause and the reason for its name:
//   - `turn` and `side` are `FStratBridge::Turn()` / `SideToMove()`, READ OFF THE LIVE BRIDGE
//     after the call. `StratLogAiTurnRefusal` is passed `Port.Turn()` / `Port.SideToMove()`,
//     which are forwards onto those same two methods, and nothing between the log call and the
//     return moves the rules state -- `RefreshPresentation` is presentation.
//   - `reason` is compared BYTE FOR BYTE against `RunAiTurnsNow`'s own `OutFailureReason`.
//     `StopReason` is both logged and returned, so the sentence is never transcribed here.
//   - `unit`, `hex`, `def` and `target` are `INDEX_NONE`, the engine constant the formatter
//     itself prints for `bHasCommand=false` -- not a hand-written -1.
//   - `phase` and `kind` are the two unavoidable literals. `handover` is the arm selector
//     (`StratMatchSubsystem.cpp:724`) and `None` is the formatter's spelling for "there was no
//     command" (`StratAiTurnRunner.cpp:82`); both are in the parser's declared domains
//     (`strat_ai_log_gate.py:61-62`). There is no module-side reader that returns either.
//
// EVERY STRING COMPARISON IS CASE SENSITIVE, this file's rule throughout.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAiHandoverRefusalCarriesTheFixedFieldsTest,
	"Stratocracy.StratPlay.T-INT-05.HandoverRefusalCarriesTheFixedFields",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAiHandoverRefusalCarriesTheFixedFieldsTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiMatchClauses;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	// THIS CLAUSE PRODUCES THE WARNING ON PURPOSE -- it is the thing under test. `Occurrences = 0`
	// means one or more. It is deliberately the broad phrase and not `phase=handover`, so that a
	// second, unintended refusal is absorbed here rather than failing as an unexpected warning
	// and hiding behind the framework's message instead of this clause's own: the count of
	// handover lines is asserted below, exactly, and that is where a stray refusal must surface.
	AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Subsystem))
	{
		return false;
	}

	FStratMatchConfig Base;
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Base, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- Which sides exist, asked of the projection ----------------------------
	// The same reasoning as the clause above: a real side is never a literal here.
	TArray<int32> Sides;
	{
		FString StartReason;
		Subsystem->StartMatch(Base, StartReason);
		if (!TestTrue(TEXT("the scouting match is live"), Subsystem->IsMatchLive()))
		{
			AddError(StartReason);
			return false;
		}

		FStratViewModel Model;
		if (!TestTrue(TEXT("the view model builds"), Subsystem->BuildViewModel(Model, Error)))
		{
			AddError(Error);
			return false;
		}
		for (const FStratUnitView& Unit : Model.Units)
		{
			Sides.AddUnique(Unit.Side);
		}
		Sides.Sort();
	}

	if (!TestTrue(TEXT("the seeded scenario deploys units for more than one side"), Sides.Num() >= 2))
	{
		return false;
	}

	// ---- Both sides AI, and a bound the game cannot finish inside --------------
	FStratMatchConfig BothAi = Base;
	BothAi.AiSides               = Sides;
	BothAi.AiBuildlistUnitIds    = UnitIdsFrom(BothAi);
	BothAi.AiMaxConsecutiveTurns = 1;

	FStratAiCapture Capture;

	FString StartReason;
	Subsystem->StartMatch(BothAi, StartReason);
	if (!TestTrue(TEXT("the AI-vs-AI match is live"), Subsystem->IsMatchLive()))
	{
		AddError(StartReason);
		return false;
	}

	const FStratBridge* const Live = Subsystem->GetBridge();
	if (!TestNotNull(TEXT("the live match has a bridge"), Live))
	{
		return false;
	}

	const int32 LinesBefore = Capture.Lines.Num();

	FString    RunReason;
	const bool bRan = Subsystem->RunAiTurnsNow(RunReason);
	Capture.Settle();

	// RECORDED, NOT ASSERTED -- this file's standing rule for this return value.
	AddInfo(FString::Printf(TEXT("RunAiTurnsNow returned %s; reason: '%s'"),
		bRan ? TEXT("true") : TEXT("false"), *RunReason));

	// ---- The bound really was the stop, and the AI really did not hand back ----
	// Both are module-side readings and both are what makes `phase=handover` the CORRECT phase
	// for this line rather than merely the one that was printed.
	if (!TestTrue(TEXT("the capture saw the AI's commands, so it is not blind"),
			Capture.CountFrom(LinesBefore, TEXT("STRAT-AI applied")) > 0))
	{
		return false;
	}
	TestEqual(TEXT("exactly one AI turn ran, which is the bound this match was configured with"),
		Capture.CountFrom(LinesBefore, TEXT("STRAT-AI turn-ended")), BothAi.AiMaxConsecutiveTurns);

	FStratViewModel After;
	if (TestTrue(TEXT("the view model builds after the bounded run"),
			Subsystem->BuildViewModel(After, Error)))
	{
		TestFalse(TEXT("the bound stopped a game that had NOT reached a §2.8 result"),
			After.Match.bHasResult);
	}
	else
	{
		AddError(Error);
	}
	TestTrue(TEXT("an AI turn is still due -- the board was never handed back, which is what 'handover' means"),
		Subsystem->IsAiTurnDue());

	TestFalse(TEXT("and the bound reported itself: the call came back with a reason"),
		RunReason.IsEmpty());

	// ---- The line, and its fields ---------------------------------------------
	TArray<FString> Handovers;
	for (int32 I = FMath::Max(0, LinesBefore); I < Capture.Lines.Num(); ++I)
	{
		if (Capture.Lines[I].Contains(TEXT("phase=handover"), ESearchCase::CaseSensitive))
		{
			Handovers.Add(Capture.Lines[I]);
		}
	}

	if (!TestEqual(
			*FString::Printf(TEXT("exactly one handover refusal was emitted (STRAT-AI lines seen: %s)"),
				*Capture.TextFrom(LinesBefore)),
			Handovers.Num(), 1))
	{
		return false;
	}

	AddInfo(FString::Printf(TEXT("the handover line: '%s'"), *Handovers[0]));

	FStratAiRefusedFields Fields;
	FString               ParseError;
	if (!TestTrue(
			*FString::Printf(TEXT("the handover line splits on the phase-D field contract (%s): '%s'"),
				*ParseError, *Handovers[0]),
			ParseRefusedLine(Handovers[0], Fields, ParseError)))
	{
		AddError(FString::Printf(TEXT("%s -- in: '%s'"), *ParseError, *Handovers[0]));
		return false;
	}

	// The two literals. See the header block: neither has a module-side reader.
	TestEqualSensitive(TEXT("phase= is the fourth phase, spelled as the parser's domain has it"),
		*Fields.Phase, TEXT("handover"));
	TestEqualSensitive(TEXT("kind= is None: a handover refusal is about no command at all"),
		*Fields.Kind, TEXT("None"));

	// The four fixed numerics. `INDEX_NONE` is the constant the formatter itself prints.
	TestEqual(TEXT("unit= carries INDEX_NONE"),   Fields.Unit,     INDEX_NONE);
	TestEqual(TEXT("hex= carries INDEX_NONE,"),   Fields.HexX,     INDEX_NONE);
	TestEqual(TEXT("hex= carries ,INDEX_NONE"),   Fields.HexY,     INDEX_NONE);
	TestEqual(TEXT("def= carries INDEX_NONE"),    Fields.DefIndex, INDEX_NONE);
	TestEqual(TEXT("target= carries INDEX_NONE"), Fields.Target,   INDEX_NONE);

	// The three that are read off the module rather than written down here.
	TestEqual(TEXT("turn= is the bridge's live turn at the bound"),
		Fields.Turn, Live->Turn());
	TestEqual(TEXT("side= is the bridge's live side to move at the bound"),
		Fields.Side, Live->SideToMove());
	TestEqualSensitive(
		TEXT("reason= is byte for byte the reason RunAiTurnsNow returned -- one sentence, not two"),
		*Fields.Reason, *RunReason);

	return true;
}

// ---------------------------------------------------------------------------
// T-INT-05 -- THE SAME GAME ON DIFFERENT CONTENT. This is the half of the phase-D
// "avoid one-corpus proof" precedent that was still open.
//
// WHAT THIS IS FOR, AND WHY IT IS NOT A DUPLICATE OF `BothSidesAiReachesAResultWithinTheBound`.
// That clause produces the corpus `Tools/architect/strat_combat_pairing_gate.py` graded in the
// combat-outcome milestone's phase 3, and phase 4 re-graded the SAME content through a
// different HOST -- a live PIE session instead of the headless suite -- and got byte-identical
// turn hashes. That discharged HOST-independence and left CONTENT-independence open in as many
// words: the gate had only ever seen one game.
//
// A gate that has only ever graded one corpus cannot be told apart from a gate that has
// memorised it. This clause plays a DIFFERENT game -- the other side moving first, a different
// army on the board -- so the pairing invariant is exercised against a sequence of attacks with
// no reason to resemble the first.
//
// THE TWO AXES ARE DERIVED FROM THE PROJECT'S OWN DATA, NEVER TYPED:
//
//   1. FIRST SIDE is taken from the projection's side list, choosing the side the phase-3
//      corpus did NOT start with. `kFirstSide` is a constant in this file, so the clause
//      ASSERTS the axis moved rather than trusting it. Re-number the scenario's sides and this
//      moves with them.
//
//   2. THE BUILDLIST is the LAST id in the unit table's own row order, handed in alone. No row
//      name is written here. Row order is load-bearing project-wide (`DT_Units` row order IS
//      the defIndex space, phase 0), so "the last row" is a derived choice against a stable
//      ordering rather than a name this file decides.
//
// WHY THE BUILDLIST HAS TO SHRINK TO ONE ENTRY TO CHANGE ANYTHING AT ALL, and this is the
// live demonstration of a finding the project had otherwise only argued from source:
// `strat::chooseBuild` returns the CHEAPEST AFFORDABLE entry, and no Fame level makes a dearer
// unit affordable while a cheaper one is not. The phase-3 corpus handed it the WHOLE table and
// got 22 builds, every one `def=0` -- measured off that corpus's own checked-in slice. So
// handing this run a different multi-entry list would have produced the same army again. The
// crew's own `T-AI-06` fixture removes Infantry for exactly this reason. Filed upstream as the
// per-type population cap ruling; until that lands, exclusion is the only mechanism there is.
//
// WHAT IS ASSERTED HERE IS THAT THE GAME IS A VALID, DIFFERENT ONE -- NOT THAT THE PAIRING
// HOLDS. The pairing is the Python gate's job, graded out of band against this clause's own log
// slice, exactly as phase 3 graded the clause above. Re-deriving the pairing in C++ here would
// be a second implementation of the gate and the two would drift. What this clause owes the
// gate is a corpus that is genuinely different content AND a real finished game rather than one
// that spun out to the bound.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAiBothSidesAiReachesAResultOnDifferentContentTest,
	"Stratocracy.StratPlay.T-INT-05.BothSidesAiReachesAResultOnDifferentContent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAiBothSidesAiReachesAResultOnDifferentContentTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiMatchClauses;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	// The same tripwire the phase-3 clause carries, for the same measured reason: a game that
	// reaches a §2.8 result MID-TURN ends with the rules module refusing the winning side's
	// closing EndTurn. `Occurrences = 0` means ONE OR MORE, so a future fix to that production
	// behaviour fails this line and the clause is revisited deliberately.
	AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Subsystem))
	{
		return false;
	}

	FStratMatchConfig Base;
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Base, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- Which sides exist, asked of the projection ----------------------------
	TArray<int32> Sides;
	{
		FString StartReason;
		Subsystem->StartMatch(Base, StartReason);
		if (!TestTrue(TEXT("the scouting match is live"), Subsystem->IsMatchLive()))
		{
			AddError(StartReason);
			return false;
		}

		FStratViewModel Model;
		if (!TestTrue(TEXT("the view model builds"), Subsystem->BuildViewModel(Model, Error)))
		{
			AddError(Error);
			return false;
		}
		for (const FStratUnitView& Unit : Model.Units)
		{
			Sides.AddUnique(Unit.Side);
		}
		Sides.Sort();
	}

	if (!TestTrue(
			*FString::Printf(TEXT("the seeded scenario deploys units for more than one side (it deploys for %d)"),
				Sides.Num()),
			Sides.Num() >= 2))
	{
		return false;
	}

	// ---- AXIS 1: the other side moves first ------------------------------------
	const int32 OtherFirstSide = Sides.Last();
	if (!TestNotEqual(
			TEXT("AXIS 1 MOVED: the first side differs from the side the phase-3 corpus started, so "
			     "this is not the same game under a new name"),
			OtherFirstSide, kFirstSide))
	{
		return false;
	}

	// ---- AXIS 2: one entry, the table's last row -------------------------------
	const TArray<FName> AllIds = UnitIdsFrom(Base);
	if (!TestTrue(TEXT("the unit table names rows to build from"), AllIds.Num() > 1))
	{
		return false;
	}

	const TArray<FName> OneEntryBuildlist = { AllIds.Last() };

	if (!TestTrue(
			*FString::Printf(TEXT("AXIS 2 MOVED: the buildlist is one entry ('%s'), not the whole "
			                      "%d-row table the phase-3 corpus handed in"),
				*AllIds.Last().ToString(), AllIds.Num()),
			OneEntryBuildlist.Num() < AllIds.Num()))
	{
		return false;
	}

	// ---- Both sides AI, on the moved content -----------------------------------
	FStratMatchConfig BothAi = Base;
	BothAi.AiSides            = Sides;
	BothAi.AiBuildlistUnitIds = OneEntryBuildlist;
	BothAi.FirstSide          = OtherFirstSide;
	BothAi.ViewingSide        = OtherFirstSide;

	FStratAiCapture Capture;

	FString StartReason;
	Subsystem->StartMatch(BothAi, StartReason);
	if (!TestTrue(TEXT("the AI-vs-AI match is live on the moved content"), Subsystem->IsMatchLive()))
	{
		AddError(StartReason);
		return false;
	}

	for (const int32 Side : Sides)
	{
		TestTrue(*FString::Printf(TEXT("side %d is configured as AI"), Side),
			Subsystem->IsSideAi(Side));
	}

	const int32 LinesBefore = Capture.Lines.Num();

	FString    RunReason;
	const bool bRan = Subsystem->RunAiTurnsNow(RunReason);
	Capture.Settle();

	// Recorded, not asserted -- the same standing production finding the phase-3 clause
	// declares: a correctly finished game currently comes back false.
	AddInfo(FString::Printf(TEXT("RunAiTurnsNow returned %s; reason: '%s'"),
		bRan ? TEXT("true") : TEXT("false"), *RunReason));

	FStratViewModel Model;
	if (!TestTrue(TEXT("the view model builds after the game"),
			Subsystem->BuildViewModel(Model, Error)))
	{
		AddError(Error);
		return false;
	}

	const int32 TurnsRun = Capture.CountFrom(LinesBefore, TEXT("STRAT-AI turn-ended"));
	const int32 Applied  = Capture.CountFrom(LinesBefore, TEXT("STRAT-AI applied"));
	const int32 Attacks  = Capture.CountFrom(LinesBefore, TEXT("STRAT-AI applied kind=Attack"));
	const int32 Builds   = Capture.CountFrom(LinesBefore, TEXT("STRAT-AI applied kind=Build"));

	// THE CORPUS FINGERPRINT, PRINTED ON PURPOSE -- AND IT LANDS IN THE AUTOMATION REPORT, NOT
	// IN THE LOG SLICE. `AddInfo` writes to `Saved/AutomationReport/index.json`, so a reader
	// holding only `evidence/09-content-independence/different-content-slice.log` will NOT find
	// this line there and must re-derive the figures from the slice's own `STRAT-AI` lines
	// (`grep -c`), which is where the evidence blackboard's table gets them from anyway. Said
	// exactly this way because the first draft of this comment implied the line travelled with
	// the slice, and a reader who believed it would go looking for something that is not there.
	AddInfo(FString::Printf(
		TEXT("CORPUS: first side %d, buildlist '%s'; %d AI turns, %d commands "
		     "(%d attacks, %d builds), bound %d; result: %s"),
		OtherFirstSide, *AllIds.Last().ToString(), TurnsRun, Applied, Attacks, Builds,
		BothAi.AiMaxConsecutiveTurns, Model.Match.bHasResult ? TEXT("yes") : TEXT("no")));

	// ---- The same three facts the phase-3 corpus had to satisfy ----------------
	// The capture is required to have SEEN the game, so nothing below is a silence it could
	// satisfy by being blind.
	TestTrue(TEXT("the capture saw the AI's commands"), Applied > 0);
	TestTrue(TEXT("the capture saw whole AI turns end"), TurnsRun > 0);

	TestTrue(TEXT("§2.8: the AI-vs-AI game reached a result under its own steam"),
		Model.Match.bHasResult);

	TestTrue(
		*FString::Printf(TEXT("it reached that result in %d AI turns, strictly inside the outer bound of %d"),
			TurnsRun, BothAi.AiMaxConsecutiveTurns),
		TurnsRun < BothAi.AiMaxConsecutiveTurns);

	TestEqual(
		*FString::Printf(TEXT("the handover guard never fired (STRAT-AI lines seen at the end: %s)"),
			*Capture.TextFrom(FMath::Max(LinesBefore, Capture.Lines.Num() - 4))),
		Capture.CountFrom(LinesBefore, TEXT("phase=handover")), 0);

	TestFalse(TEXT("no AI turn is due once the match has a result"), Subsystem->IsAiTurnDue());

	// ---- And the corpus is worth grading: it contains combat -------------------
	// A game that reached a result with no attack in it would satisfy every fact above and hand
	// the pairing gate nothing to pair. This is the one assertion here that exists for the
	// gate's benefit rather than the game's.
	TestTrue(TEXT("the corpus contains combat for the pairing gate to grade"), Attacks > 0);

	return true;
}
