// GDD §2.11.2 -- the AI turn playback reel. T-TURN-09's state-equality half, and the local
// gate GATE-AITURN.
//
// WHAT THESE CLAUSES ARE FOR. W5 landed `FStratAiPlaybackReel`, a second timer on
// `UStratMatchSubsystem`, and a skip gate at the top of
// `AStratPlayerController::HandleSelectionEvent` -- and it landed all of it with
// `FStratMatchConfig::AiPlaybackStepSeconds` defaulted to `0.0f`. So `BeginAiPlayback` returns
// on its first line in every existing test, `OnAiPlaybackTimer` never fires, and
// `FocusPlaybackStep` never executes. A green build and a green suite across that change is
// evidence of nothing at all, exactly as `StratAiTurnRunnerClauses.cpp` records phase B's
// defaulted-off runner was. These clauses are what turn it into evidence.
//
// WHERE THE EXPECTATIONS COME FROM, which is the only question that matters.
//   - EVERY COMMAND driven through the runner came out of `FStratBridge::NextAiCommand`,
//     through the production `FStratBridgeAiTurnPort`. There is not one hex, unit id,
//     defIndex or turn number written in this file.
//   - The reel's CONTENT is compared against `FStratRecordingAiPort::Accepted` -- the
//     commands the PORT saw accepted -- and never against a list this file composed. The two
//     sides of that comparison are the recorder under test and an independent recorder that
//     sits at the point of acceptance.
//   - The reel's LENGTH is compared against `FStratAiTurnOutcome::CommandsApplied`, which is
//     the runner's own count and is incremented by a different statement than the one that
//     appends a step. On the refusal and bound arms that count is the whole expectation:
//     "exactly 2" is nowhere in this file, only "exactly what the runner says it applied".
//   - STATE is always two or three bridges' own `StateHash()`, and what was recorded is always
//     `RecordedCommandCount()` plus `ReplayRecordedLogOnto` onto a fresh bridge.
//
// WHY `RecordedLog()` IS PINNED THROUGH `ReplayRecordedLogOnto` AND NOT READ DIRECTLY.
// `FStratBridge::RecordedLog()` returns a `std::vector<strat::SaveCommand>`, and this module
// names no `strat::` type -- `StratAiTurnRunnerClauses.cpp` states that discipline for this
// same directory. `ReplayRecordedLogOnto` onto a freshly seeded bridge, compared by
// `StateHash()`, is the engine-typed reading of the same artifact and is STRICTLY STRONGER
// than an element count: two logs of equal length that differ in any command replay to
// different hashes.
//
// THE POSITIVE CONTROLS ARE LOAD-BEARING AND ARE NOT DECORATION.
//   - Clause 1 requires the three dispositions to have actually DIFFERED -- the never-toured
//     reel's cursor is 0 and the other two are at the end -- before it reads anything into
//     their agreeing. An equality over three identical things proves nothing.
//   - Clause 3 is an assertion that two runs AGREE, which is exactly the shape a comparator
//     that always returns "equal" satisfies forever. So both of its comparators are first run
//     over a pair that genuinely differs (a third arm whose port refuses mid-turn) and are
//     REQUIRED to report the difference. Without that control the keystone clause is
//     decoration.
//   - Clause 4 distinguishes "nothing was recorded" from "the recorder was never handed a
//     reel" with `Num()`, and asserts the discriminator works by holding a never-used reel
//     beside a skipped one. That distinction is the whole reason
//     `FStratAiPlaybackReel::SkipToEnd` clears nothing, and a default-constructed reel reads
//     identically to a used-and-skipped one on every OTHER accessor. That guarantee is about a
//     TOUR ending; a MATCH boundary clears the list on purpose, and
//     `ReseedMidTourLeavesNoStuckTour` is where the zero is pinned.
//
// EVERY STRING COMPARISON IS CASE SENSITIVE. `FString::operator==`, `FString::Equals`'s
// default, `FString::Contains` and `TestEqual(FString)` are all case-INSENSITIVE in UE 5.8, so
// the ordinary spelling of a comparison between two byte-identical strings is a clause that
// reports green forever. `TestEqualSensitive` and `ESearchCase::CaseSensitive` throughout.
//
// IT NAMES NO `strat::` TYPE AND CALLS NO `strat::` FUNCTION, per `StratPlay.Build.cs`. Every
// rules answer arrives engine-typed, through `FStratBridge` or through the view model.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Math/IntPoint.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/StrongObjectPtr.h"

#include "StratAiPlayback.h"
#include "StratAiTurnRunner.h"
#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
#include "StratPlayerController.h"
#include "StratSelectionMachine.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

// Included from a .cpp and never from a UHT-parsed header -- the module-wide discipline.
#include "StratBridge.h"

namespace StratAiPlaybackClauses
{
	/** The same harness pin every parity file in this tree carries. No rule decides which side
	 *  moves first, so this is a harness constant and not a rule. */
	static const int32 kFirstSide = 0;

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
	 * `FStratBridge::LoadDefinitions` takes the row NAME as the id, so this is the same source
	 * the definitions vector was built from. A hand-written `{"Infantry", "Tank"}` here would
	 * be this file deciding §4.8's id space.
	 */
	static TArray<FName> UnitIdsFromTable()
	{
		UDataTable* const Units = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		return Units != nullptr ? Units->GetRowNames() : TArray<FName>();
	}

	/** Seeds, then configures §2.9's buildlist from the table's own ids so the Build arm has
	 *  something to be about. An empty buildlist configures an AI that never builds. */
	static bool SeedBridgeWithBuildlist(FStratBridge& Bridge, FString& OutError)
	{
		if (!SeedBridge(Bridge, OutError))
		{
			return false;
		}

		const TArray<FName> Ids = UnitIdsFromTable();
		if (Ids.Num() == 0)
		{
			OutError = TEXT("DT_Units carries no rows, so no buildlist can be configured");
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

	/**
	 * A port that DECORATES the production one and keeps its own list of what was ACCEPTED.
	 *
	 * THE INDEPENDENT RECORDER, AND THE WHOLE REASON THE ORDERING CLAUSE IS NOT CIRCULAR.
	 * `FStratAiTurnRunner::RunTurn` appends to the reel immediately after `Port.Submit`
	 * returns true; this appends to `Accepted` immediately after the INNER port's `Submit`
	 * returns true, one stack frame lower and in a different file. So the two lists are built
	 * by two writers at the same instant from the same value, and comparing them pins that the
	 * reel is a record of what the rules module took rather than of what the runner intended.
	 * A reel compared against a list this file composed would be pinning nothing but arithmetic.
	 *
	 * IT ALSO INJECTS THE `apply` REFUSAL, on `FStratFaultInjectingAiPort`'s pattern next door
	 * and for its reason: the shipping AI never has a submission refused, so the refusal arm of
	 * the ordering clause is UNREACHABLE through a real `FStratBridge` and can only be reached
	 * by a double. `Accepted` is not appended on the refused step, which is what the clause
	 * then requires of the reel.
	 */
	struct FStratRecordingAiPort final : public IStratAiTurnPort
	{
		explicit FStratRecordingAiPort(IStratAiTurnPort& InInner)
			: Inner(InInner)
		{
		}

		IStratAiTurnPort& Inner;

		/** Every command whose submission the INNER port accepted, in submission order. */
		TArray<FStratAiCommand> Accepted;

		/** Zero-based submission index to refuse at, or `INDEX_NONE` to refuse nothing. */
		int32   RefuseSubmitAtStep = INDEX_NONE;
		FString InjectedSubmitReason;

		int32 SubmitCalls = 0;

		virtual int32   Turn() const override       { return Inner.Turn(); }
		virtual int32   SideToMove() const override { return Inner.SideToMove(); }
		virtual FString StateHash() const override  { return Inner.StateHash(); }

		virtual bool NextCommand(int32 Side, FStratAiCommand& OutCommand,
		                         FString& OutFailureReason) const override
		{
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
			if (!Inner.Submit(Command, OutFailureReason))
			{
				return false;
			}
			Accepted.Add(Command);
			return true;
		}
	};

	/** For failure messages only. No clause compares anything against this. */
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

	static FString DescribeCommand(const FStratAiCommand& C)
	{
		return FString::Printf(TEXT("%s unit=%d hex=%d,%d def=%d target=%d"),
			KindWord(C.Kind), C.UnitId, C.Hex.X, C.Hex.Y, C.DefIndex, C.TargetId);
	}

	static FString DescribeStep(const FStratAiPlaybackStep& S)
	{
		return FString::Printf(TEXT("%s unit=%d hex=%d,%d def=%d target=%d turn=%d side=%d hasHex=%s"),
			KindWord(S.Kind), S.UnitId, S.Hex.X, S.Hex.Y, S.DefIndex, S.TargetId,
			S.Turn, S.Side, S.bHasHex ? TEXT("y") : TEXT("n"));
	}

	/** Whether one recorded step carries the same command the port accepted at that index. */
	static bool StepCarriesCommand(const FStratAiPlaybackStep& S, const FStratAiCommand& C)
	{
		return S.Kind     == C.Kind
			&& S.UnitId   == C.UnitId
			&& S.TargetId == C.TargetId
			&& S.DefIndex == C.DefIndex
			&& S.Hex      == C.Hex;
	}

	/**
	 * Whether two submitted-command sequences are the same sequence, field for field.
	 *
	 * IT REPORTS THE FIRST DIFFERENCE RATHER THAN A BOOL ALONE, because clause 3's positive
	 * control asserts this function CAN say no, and a control that only knows "not equal"
	 * cannot show it looked at the right thing.
	 */
	static bool SequencesAgree(const TArray<FStratAiCommand>& A, const TArray<FStratAiCommand>& B,
	                           FString& OutWhy)
	{
		if (A.Num() != B.Num())
		{
			OutWhy = FString::Printf(TEXT("lengths differ: %d vs %d"), A.Num(), B.Num());
			return false;
		}
		for (int32 I = 0; I < A.Num(); ++I)
		{
			const FStratAiCommand& L = A[I];
			const FStratAiCommand& R = B[I];
			if (L.Kind != R.Kind || L.UnitId != R.UnitId || L.TargetId != R.TargetId
				|| L.DefIndex != R.DefIndex || L.Hex != R.Hex)
			{
				OutWhy = FString::Printf(TEXT("command %d differs: '%s' vs '%s'"),
					I, *DescribeCommand(L), *DescribeCommand(R));
				return false;
			}
		}
		OutWhy.Reset();
		return true;
	}

	/** Whether two outcomes are the same outcome, field for field. See `SequencesAgree`. */
	static bool OutcomesAgree(const FStratAiTurnOutcome& A, const FStratAiTurnOutcome& B,
	                          FString& OutWhy)
	{
		if (A.bOk != B.bOk)               { OutWhy = TEXT("bOk differs");        return false; }
		if (A.bEndedTurn != B.bEndedTurn) { OutWhy = TEXT("bEndedTurn differs"); return false; }
		if (A.Side != B.Side)             { OutWhy = TEXT("Side differs");       return false; }
		if (A.Turn != B.Turn)             { OutWhy = TEXT("Turn differs");       return false; }
		if (A.CommandsApplied != B.CommandsApplied)
		{
			OutWhy = FString::Printf(TEXT("CommandsApplied differs: %d vs %d"),
				A.CommandsApplied, B.CommandsApplied);
			return false;
		}
		// CASE SENSITIVE. `FString::Equals` defaults to IgnoreCase, which on two refusal
		// sentences from the same source is a comparison that cannot fail.
		if (!A.FailureReason.Equals(B.FailureReason, ESearchCase::CaseSensitive))
		{
			OutWhy = FString::Printf(TEXT("FailureReason differs: '%s' vs '%s'"),
				*A.FailureReason, *B.FailureReason);
			return false;
		}
		OutWhy.Reset();
		return true;
	}

	/** See `StratMatchConclusion.cpp` on why `InitializeActorsForPlay` is not decoration and
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
	 * A HARNESS INTERVAL AND NOT A PACE. Long enough that no clause below can be affected
	 * by a clock -- these worlds are created with `bInformEngineOfWorld = false` and are
	 * never ticked, so nothing advances a tour except a clause that asks it to.
	 *
	 * SECTION 2.11.2's 0.5 IS NOT WRITTEN HERE AND NO CLAUSE ASSERTS A PACE.
	 * `AiPlaybackStepSeconds`'s own declaration rules that the GDD's figure belongs on the
	 * GameMode Blueprint default and that the C++ field is 'the switch and not the setting'.
	 * A test that pinned 0.5 would be asserting a second home for it.
	 */
	static const float kHarnessPlaybackInterval = 600.0f;

	/**
	 * Both sides AI, so a result lands inside one synchronous `RunAiTurnsNow`.
	 *
	 * THE SIDES ARE READ OFF THE SCENARIO'S OWN VIEW MODEL and are not written here, which
	 * is why this needs a live subsystem to ask: it starts a PROBE match, reads which sides
	 * the scenario actually deploys units for, and hands back a config the caller starts for
	 * real. A hand-written `{0, 1}` would be this file deciding the scenario's roster.
	 *
	 * `PlaybackStepSeconds` IS THE ONE FIELD ITS CALLERS DIFFER IN, and that is deliberate:
	 * `SkipIsReachableAfterTheMatchConcludes` and `DefaultConfigConsumesNoInput` are a
	 * matched pair whose fixtures are identical but for this number, so each is the other's
	 * control -- one shows a tour arming and one shows the reel retired instead.
	 */
	static bool MakeAiVsAiConfig(UStratMatchSubsystem& Match, float PlaybackStepSeconds,
	                             FStratMatchConfig& Out, FString& OutError)
	{
		if (!MakeConfig(Out, OutError))
		{
			return false;
		}

		FStratMatchConfig Probe = Out;
		Match.StartMatch(Probe, OutError);
		if (!Match.IsMatchLive())
		{
			return false;
		}

		FStratViewModel Model;
		if (!Match.BuildViewModel(Model, OutError))
		{
			return false;
		}

		TArray<int32> Sides;
		for (const FStratUnitView& Unit : Model.Units)
		{
			Sides.AddUnique(Unit.Side);
		}
		Sides.Sort();
		if (Sides.Num() < 2)
		{
			OutError = TEXT("the shipped scenario deploys units for fewer than two sides");
			return false;
		}

		Out.AiSides               = Sides;
		Out.AiBuildlistUnitIds    = UnitIdsFromTable();
		Out.AiPlaybackStepSeconds = PlaybackStepSeconds;
		return true;
	}
}

// ---------------------------------------------------------------------------
// T-TURN-09 -- THE THREE PLAYBACK DISPOSITIONS AGREE ON STATE.
//
// §2.11.2: "this is presentation pacing only, no rules change. Any click or Esc skips to the
// end state." The same scenario and the same command sequence, resolved three ways -- never
// toured (`AiPlaybackStepSeconds = 0`, the shipped default), toured to exhaustion, and skipped
// part-way -- must reach the same state and must have recorded the same commands.
//
// THE STRETCH IS STATED RATHER THAN LEFT TO BE NOTICED. This clause reaches T-TURN-09's
// STATE-EQUALITY half and nothing else. It re-asserts nothing about §2.8's result tier, about
// which side won, or about how a result is classified; those are that ID's other subject and
// have their own clauses. The name says `PlaybackDispositionsAgreeOnState` for that reason.
//
// AND THE AGREEMENT IS CHEAP BY CONSTRUCTION, WHICH IS THE POINT AND NOT AN OBJECTION.
// `StratAiPlayback.h` argues that the board is already reconciled to the FINAL view model
// before a tour begins, so skipping abandons a camera tour and fast-forwards no simulation.
// The value of pinning it is that the cheapness is STRUCTURAL -- `FStratAiPlaybackReel` holds
// no `FStratBridge`, submits nothing, and has no route to one -- and this clause is what
// reddens on the day somebody gives it one. A per-step board, the shape that header records as
// rejected, would fail here on the first disposition.
//
// THE CONTROL: THE THREE DISPOSITIONS ARE REQUIRED TO HAVE DIFFERED. The never-toured reel's
// cursor must be 0 while the other two sit at the end, and the turn must have applied more
// than one command so "part-way" is a real position. Without that, three untouched reels would
// satisfy every equality below.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratPlaybackDispositionsAgreeOnStateTest,
	"Stratocracy.StratPlay.T-TURN-09.PlaybackDispositionsAgreeOnState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratPlaybackDispositionsAgreeOnStateTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiPlaybackClauses;

	FStratBridge NeverToured;
	FStratBridge Exhausted;
	FStratBridge SkippedPartWay;
	FString      Error;

	if (!TestTrue(TEXT("the never-toured bridge seeds from the shipped scenario"),
			SeedBridgeWithBuildlist(NeverToured, Error)) ||
		!TestTrue(TEXT("the toured bridge seeds identically"),
			SeedBridgeWithBuildlist(Exhausted, Error)) ||
		!TestTrue(TEXT("the skipped bridge seeds identically"),
			SeedBridgeWithBuildlist(SkippedPartWay, Error)))
	{
		AddError(Error);
		return false;
	}

	if (!TestEqualSensitive(TEXT("all three bridges start at the same state hash"),
			Exhausted.StateHash(), NeverToured.StateHash()) ||
		!TestEqualSensitive(TEXT("the third bridge starts there too"),
			SkippedPartWay.StateHash(), NeverToured.StateHash()))
	{
		return false;
	}

	FStratBridgeAiTurnPort PortA(&NeverToured);
	FStratBridgeAiTurnPort PortB(&Exhausted);
	FStratBridgeAiTurnPort PortC(&SkippedPartWay);

	FStratAiPlaybackReel ReelA;
	FStratAiPlaybackReel ReelB;
	FStratAiPlaybackReel ReelC;

	FStratAiTurnRunner RunnerA;
	FStratAiTurnRunner RunnerB;
	FStratAiTurnRunner RunnerC;

	const FStratAiTurnOutcome A = RunnerA.RunTurn(PortA, &ReelA);
	const FStratAiTurnOutcome B = RunnerB.RunTurn(PortB, &ReelB);
	const FStratAiTurnOutcome C = RunnerC.RunTurn(PortC, &ReelC);

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
	if (!TestTrue(TEXT("the third AI turn ran to an applied EndTurn"), C.bOk))
	{
		AddError(C.FailureReason);
		return false;
	}

	// ---- The control: there has to be a tour to have three dispositions OF ----
	if (!TestTrue(
			*FString::Printf(TEXT("the AI turn recorded more than one action, so 'part-way' is a real "
			                      "position (it recorded %d)"), ReelA.Num()),
			ReelA.Num() > 1))
	{
		return false;
	}

	// DISPOSITION 1 -- `AiPlaybackStepSeconds = 0`. `UStratMatchSubsystem::BeginAiPlayback`
	// returns on its first line in that configuration, so the reel is filled and never
	// advanced. Reproduced here by doing nothing to `ReelA`.
	const int32 CursorNeverToured = ReelA.GetCursor();

	// DISPOSITION 2 -- the tour runs to exhaustion. `OnAiPlaybackTimer` peeks, focuses, and
	// advances; the focusing is a camera and is not what this clause is about, so the advance
	// is driven directly. `Advance()` returning false is the signal that timer uses to stop
	// re-arming, and is the loop bound here for the same reason.
	int32 Advances = 0;
	while (ReelB.Advance())
	{
		++Advances;
	}
	const int32 CursorExhausted = ReelB.GetCursor();

	// DISPOSITION 3 -- one step shown, then §2.11.2's skip.
	TestTrue(TEXT("the third reel shows one action before the skip"), ReelC.Advance());
	const int32 CursorMidTour = ReelC.GetCursor();
	TestTrue(TEXT("§2.11.2's skip reports that it ended a tour that was running"),
		ReelC.SkipToEnd());
	const int32 CursorSkipped = ReelC.GetCursor();

	TestEqual(TEXT("the never-toured reel's cursor never left the start"), CursorNeverToured, 0);
	TestEqual(TEXT("the exhausted tour advanced once per recorded action"), Advances, ReelB.Num());
	TestEqual(TEXT("the exhausted tour's cursor is at the end"), CursorExhausted, ReelB.Num());
	TestTrue(TEXT("the skipped tour was genuinely part-way when it was skipped"),
		CursorMidTour > 0 && CursorMidTour < ReelC.Num());
	TestEqual(TEXT("the skipped tour's cursor is at the end"), CursorSkipped, ReelC.Num());
	TestTrue(
		TEXT("the three dispositions really were three different dispositions -- an equality "
		     "over three untouched reels would prove nothing"),
		CursorNeverToured != CursorExhausted);

	// ---- The equality the clause is named for --------------------------------
	TestEqualSensitive(
		TEXT("§2.11.2: touring the AI's action list to exhaustion reaches the state that not "
		     "touring it at all reaches"),
		Exhausted.StateHash(), NeverToured.StateHash());

	TestEqualSensitive(
		TEXT("§2.11.2: skipping the tour part-way reaches that same state -- 'skips to the end "
		     "state', and the end state was never absent"),
		SkippedPartWay.StateHash(), NeverToured.StateHash());

	// ---- And on what was recorded, which is §4.10's artifact ------------------
	TestEqual(TEXT("touring recorded no extra command"),
		Exhausted.RecordedCommandCount(), NeverToured.RecordedCommandCount());
	TestEqual(TEXT("skipping recorded no extra command"),
		SkippedPartWay.RecordedCommandCount(), NeverToured.RecordedCommandCount());
	TestTrue(TEXT("the turn recorded something -- an equality over empty logs proves nothing"),
		NeverToured.RecordedCommandCount() > 0);

	// `RecordedLog()` itself is a `std::vector<strat::SaveCommand>` and is not nameable in this
	// module. Replaying it onto a freshly seeded bridge and comparing hashes reads the same
	// artifact through the engine-typed door, and is strictly stronger than the count above:
	// two logs of equal length that differ in one command replay to different hashes.
	FStratBridge ReplayOfNeverToured;
	FStratBridge ReplayOfExhausted;
	FStratBridge ReplayOfSkipped;
	if (!TestTrue(TEXT("three fresh bridges seed for the replay comparison"),
			SeedBridge(ReplayOfNeverToured, Error) && SeedBridge(ReplayOfExhausted, Error)
			&& SeedBridge(ReplayOfSkipped, Error)))
	{
		AddError(Error);
		return false;
	}

	const FStratResult RA = NeverToured.ReplayRecordedLogOnto(ReplayOfNeverToured);
	const FStratResult RB = Exhausted.ReplayRecordedLogOnto(ReplayOfExhausted);
	const FStratResult RC = SkippedPartWay.ReplayRecordedLogOnto(ReplayOfSkipped);
	if (!TestTrue(TEXT("the never-toured turn's recorded log replays"), RA.bOk))
	{
		AddError(RA.Reason);
		return false;
	}
	if (!TestTrue(TEXT("the toured turn's recorded log replays"), RB.bOk))
	{
		AddError(RB.Reason);
		return false;
	}
	if (!TestTrue(TEXT("the skipped turn's recorded log replays"), RC.bOk))
	{
		AddError(RC.Reason);
		return false;
	}

	TestEqualSensitive(
		TEXT("what the toured turn recorded replays to what the never-toured turn's log replays to"),
		ReplayOfExhausted.StateHash(), ReplayOfNeverToured.StateHash());
	TestEqualSensitive(
		TEXT("and so does what the skipped turn recorded"),
		ReplayOfSkipped.StateHash(), ReplayOfNeverToured.StateHash());

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AITURN -- THE REEL RECORDS EVERY ACCEPTED COMMAND, IN ORDER, AND NOTHING ELSE.
//
// `GATE-AITURN` IS A LOCAL GATE NAME. It mints no acceptance ID and claims none; it groups the
// four clauses below, whose subject is a presentation wrapper that no acceptance ID's own words
// reach. `T-AI-06` is deliberately NOT used: its subject is §2.9's tie-break determinism, and a
// playback wrapper asserts nothing about tie-breaking. `T-INT-05` is deliberately not used
// either -- `FStratViewModel` gains no member in this wave, so the reel never enters that ID's
// subject.
//
// THE PROPERTY: a step exists for exactly the commands the RULES MODULE ACCEPTED, in the order
// it accepted them, and for nothing else. `FStratAiTurnRunner::RunTurn` appends after
// `Port.Submit` returns true; an append moved above that call would turn the reel from a record
// into a prediction, and `StratAiPlayback.h` rests its entire legality on that distinction --
// "IT IS A RECORDING AND NOT AN INFERENCE, WHICH IS THE DISTINCTION THAT MAKES IT LEGAL HERE".
// This clause is what makes that sentence a gate.
//
// THE COMPARISON IS AGAINST AN INDEPENDENT RECORDER AND NOT AGAINST A LIST WRITTEN HERE.
// `FStratRecordingAiPort::Accepted` is appended one stack frame below the reel's own append and
// only when the INNER production port returned true. Nothing in this file decides what the AI
// should do.
//
// THREE ARMS, AND TWO OF THEM ARE UNREACHABLE THROUGH A REAL BRIDGE. The shipping AI never has
// a submission refused and never runs away, so the refusal and bound arms exist only because
// `IStratAiTurnPort` lets a double supply the fault -- the same shape
// `StratAiTurnRunnerClauses.cpp` records for its own three fault arms.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratReelRecordsEveryAcceptedCommandInOrderTest,
	"Stratocracy.StratPlay.GATE-AITURN.ReelRecordsEveryAcceptedCommandInOrder",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratReelRecordsEveryAcceptedCommandInOrderTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiPlaybackClauses;

	// Arms 2 and 3 drive the runner's refusal and bound paths, each of which logs
	// `STRAT-AI refused` at Warning. Declared expected rather than suppressed, on
	// `StratAiTurnRunnerClauses.cpp`'s line: those lines are the fault the arm induced.
	AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FString Error;

	// ---- ARM 1: the whole turn, nothing refused ------------------------------
	FStratBridge Happy;
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"),
			SeedBridgeWithBuildlist(Happy, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratBridgeAiTurnPort HappyInner(&Happy);
	FStratRecordingAiPort  HappyPort(HappyInner);
	FStratAiPlaybackReel   HappyReel;
	FStratAiTurnRunner     HappyRunner;

	const FStratAiTurnOutcome Happily = HappyRunner.RunTurn(HappyPort, &HappyReel);
	if (!TestTrue(TEXT("the AI turn ran to an applied EndTurn"), Happily.bOk))
	{
		AddError(Happily.FailureReason);
		return false;
	}
	TestTrue(TEXT("the turn applied commands -- an ordering over an empty list proves nothing"),
		Happily.CommandsApplied > 0);

	// The runner's own count, incremented by a different statement than the one that appends a
	// step. "Exactly 12" appears nowhere in this file.
	TestEqual(
		TEXT("the reel holds exactly one step per command the runner applied"),
		HappyReel.Num(), Happily.CommandsApplied);

	TestEqual(
		TEXT("and exactly one step per command the port saw the rules module accept"),
		HappyReel.Num(), HappyPort.Accepted.Num());

	// ORDER, step by step, against the independent recorder.
	const int32 HappySteps = HappyReel.Num();
	int32 StepsChecked = 0;
	for (int32 I = 0; I < FMath::Min(HappySteps, HappyPort.Accepted.Num()); ++I)
	{
		const FStratAiPlaybackStep* const Step = HappyReel.Peek();
		if (Step == nullptr)
		{
			AddError(FString::Printf(TEXT("the reel ran out at step %d of %d"), I, HappySteps));
			break;
		}
		if (!StepCarriesCommand(*Step, HappyPort.Accepted[I]))
		{
			AddError(FString::Printf(
				TEXT("step %d is not the command accepted at that position: reel '%s' vs port '%s'"),
				I, *DescribeStep(*Step), *DescribeCommand(HappyPort.Accepted[I])));
			break;
		}
		++StepsChecked;
		HappyReel.Advance();
	}
	TestEqual(
		TEXT("every step in the reel is the command accepted at that same position, in order"),
		StepsChecked, HappyPort.Accepted.Num());

	// THE CLOSING EndTurn IS IN THE LIST. The runner's own `bEndedTurn` is the witness that the
	// turn ended with an applied EndTurn; this asserts the reel did not drop it. The kind is
	// read off the PORT's last accepted command, so what "an EndTurn" means here is the rules
	// module's answer and not this file's.
	TestTrue(TEXT("the runner reports the turn ended"), Happily.bEndedTurn);
	if (HappyPort.Accepted.Num() > 0)
	{
		const FStratAiCommand& Last = HappyPort.Accepted.Last();
		TestTrue(TEXT("the last command the rules module accepted was the EndTurn"),
			Last.Kind == EStratAiCommandKind::EndTurn);
	}

	// `bHasHex` IS THE ONE JUDGEMENT `FStratAiPlaybackReel::Record` MAKES, and it is pinned
	// because `FStratAiCommand::Hex` defaults to `(0,0)` -- a real hex -- so a camera that
	// trusted `Hex` unconditionally would pan to the board's origin corner once per AI turn.
	// A second, identical turn is run so the cursor is fresh; the loop above walked the first
	// reel to its end.
	{
		FStratBridge Second;
		if (!TestTrue(TEXT("a second bridge seeds identically"),
				SeedBridgeWithBuildlist(Second, Error)))
		{
			AddError(Error);
			return false;
		}
		FStratBridgeAiTurnPort SecondInner(&Second);
		FStratRecordingAiPort  SecondPort(SecondInner);
		FStratAiPlaybackReel   SecondReel;
		FStratAiTurnRunner     SecondRunner;
		const FStratAiTurnOutcome Again = SecondRunner.RunTurn(SecondPort, &SecondReel);
		if (!TestTrue(TEXT("the second AI turn ran to an applied EndTurn"), Again.bOk))
		{
			AddError(Again.FailureReason);
			return false;
		}

		int32 HexBearing = 0;
		int32 Hexless    = 0;
		for (int32 I = 0; I < SecondPort.Accepted.Num(); ++I)
		{
			const FStratAiPlaybackStep* const Step = SecondReel.Peek();
			if (Step == nullptr)
			{
				break;
			}
			const bool bIsEndTurn = (SecondPort.Accepted[I].Kind == EStratAiCommandKind::EndTurn);
			if (Step->bHasHex == bIsEndTurn)
			{
				AddError(FString::Printf(
					TEXT("step %d claims bHasHex=%s for a command the port accepted as %s"),
					I, Step->bHasHex ? TEXT("true") : TEXT("false"),
					KindWord(SecondPort.Accepted[I].Kind)));
				break;
			}
			if (bIsEndTurn)
			{
				++Hexless;
			}
			else
			{
				++HexBearing;
			}
			SecondReel.Advance();
		}
		TestTrue(TEXT("at least one recorded step carries a hex the camera can step to"),
			HexBearing > 0);
		TestTrue(TEXT("and the closing EndTurn carries none"), Hexless > 0);
	}

	// ---- ARM 2: the port refuses part-way ------------------------------------
	// UNREACHABLE THROUGH A REAL `FStratBridge`. The clause is that the refused command is not
	// in the list and neither is anything after it: what the reel holds is exactly what the
	// runner reports it applied, which on this arm is strictly fewer than the happy turn.
	FStratBridge Refusing;
	if (!TestTrue(TEXT("the refusing bridge seeds identically"),
			SeedBridgeWithBuildlist(Refusing, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratBridgeAiTurnPort RefusingInner(&Refusing);
	FStratRecordingAiPort  RefusingPort(RefusingInner);
	// The sentence is this file's, and that is stated rather than hidden: there is no
	// module-side apply refusal that does not require inventing an illegal command. The clause
	// asserts nothing about the sentence, only about the list.
	RefusingPort.InjectedSubmitReason = TEXT("this test refused the submission");
	// Zero-based, so the THIRD submission is refused and two stand. The assertions below
	// compare against the port's own accepted count and the runner's own applied count; the
	// number 2 is asserted nowhere.
	RefusingPort.RefuseSubmitAtStep = 2;

	FStratAiPlaybackReel RefusedReel;
	FStratAiTurnRunner   RefusingRunner;
	const FStratAiTurnOutcome Refused = RefusingRunner.RunTurn(RefusingPort, &RefusedReel);

	TestFalse(TEXT("a refused submission is reported as a fault and not as a quiet turn"),
		Refused.bOk);
	TestFalse(TEXT("a refused submission is not a turn end"), Refused.bEndedTurn);

	TestEqual(
		TEXT("nothing is appended on the refusal arm: the reel holds exactly what the runner "
		     "reports it applied before the refusal"),
		RefusedReel.Num(), Refused.CommandsApplied);
	TestEqual(
		TEXT("and exactly what the port saw the rules module accept before the refusal"),
		RefusedReel.Num(), RefusingPort.Accepted.Num());
	TestEqual(
		TEXT("the refused submission itself left no step -- the port made one more call than it "
		     "accepted"),
		RefusingPort.SubmitCalls, RefusingPort.Accepted.Num() + 1);
	TestTrue(
		*FString::Printf(TEXT("the refusal really did cut the turn short (%d steps against the "
		                      "whole turn's %d)"), RefusedReel.Num(), HappySteps),
		RefusedReel.Num() < HappySteps);
	TestTrue(TEXT("and it cut it short after something, not before everything"),
		RefusedReel.Num() > 0);

	// ---- ARM 3: the runner's own loop bound ----------------------------------
	// ALSO UNREACHABLE THROUGH A REAL BRIDGE. `MaxCommandsPerTurn` is a harness number here,
	// and the clause asserts the reel agrees with the runner's applied count at the bound --
	// not that the bound is any particular value.
	FStratBridge Bounded;
	if (!TestTrue(TEXT("the bounded bridge seeds identically"),
			SeedBridgeWithBuildlist(Bounded, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratBridgeAiTurnPort BoundedInner(&Bounded);
	FStratRecordingAiPort  BoundedPort(BoundedInner);
	FStratAiPlaybackReel   BoundedReel;
	FStratAiTurnRunner     BoundedRunner;
	BoundedRunner.MaxCommandsPerTurn = 1;

	const FStratAiTurnOutcome AtBound = BoundedRunner.RunTurn(BoundedPort, &BoundedReel);

	AddInfo(FString::Printf(TEXT("at a bound of 1 the runner reported bOk=%s, applied=%d, reason '%s'"),
		AtBound.bOk ? TEXT("true") : TEXT("false"), AtBound.CommandsApplied, *AtBound.FailureReason));

	TestEqual(
		TEXT("nothing is appended past the bound: the reel holds exactly what the runner "
		     "reports it applied"),
		BoundedReel.Num(), AtBound.CommandsApplied);
	TestEqual(
		TEXT("and exactly what the port saw the rules module accept"),
		BoundedReel.Num(), BoundedPort.Accepted.Num());
	TestTrue(TEXT("the bound really did cut the turn short"),
		BoundedReel.Num() < HappySteps);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AITURN -- THE REEL DOES NOT CHANGE THE TURN. THE KEYSTONE.
//
// `StratAiTurnRunner.h`'s PACING bullet refuses a per-command delay, on the ground that "a
// per-command delay would make this an incremental machine with resumable state, and resumable
// state is a mirror of the rules state by another name". W5 amended that bullet rather than
// reversing it, and the amendment's whole claim is one sentence: "Delete the parameter and
// every turn plays identically; that is the test of the claim." THIS IS THAT TEST. Without it,
// the claim that the EXECUTION was not split is unfalsifiable -- a reader can only re-read the
// comment that makes it.
//
// TWO IDENTICALLY SEEDED BRIDGES, ONE RUN WITH `nullptr` AND ONE WITH A REEL, compared on:
// every field of `FStratAiTurnOutcome`; the whole sequence of commands the rules module
// accepted, field for field; the resulting `StateHash()`; `RecordedCommandCount()`; and what
// the two recorded logs replay to.
//
// THE POSITIVE CONTROL, WHICH IS WHAT STOPS THIS BEING DECORATION. This clause's assertion is
// that two things AGREE, and a comparator that always answered "they agree" would satisfy it
// forever. So before either equality is read, both comparators are run over a THIRD arm that
// genuinely differs (a port that refuses its third submission) and are REQUIRED to report the
// difference and to name where. A comparator that cannot see a refusal cut a turn in half
// cannot see a reel change one either.
//
// WHAT WOULD REDDEN IT, stated so the clause is not decoration by omission either. Each of
// these one-line edits to `FStratAiTurnRunner::RunTurn` fails it:
//   - `if (OutReel != nullptr) { return Outcome; }` anywhere in the command loop -- the shape
//     a naive per-command-delay implementation takes, and the exact thing the PACING bullet
//     forbids. `CommandsApplied` and the accepted sequence both change.
//   - turning the existing `if (OutReel != nullptr) { OutReel->Record(...); }` into a
//     `continue` or a `break`.
//   - handing the reel down into `Port.Submit`, or letting `Record` take its command by
//     non-const reference and mutate it.
// NONE OF THESE WAS EXECUTED: making one would be an edit outside this lane. They are stated
// as the falsifiability argument, and the CONTROL above is the executed half of it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratReelDoesNotChangeTheTurnTest,
	"Stratocracy.StratPlay.GATE-AITURN.ReelDoesNotChangeTheTurn",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratReelDoesNotChangeTheTurnTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiPlaybackClauses;

	// The positive control below induces a submission refusal on purpose, which logs
	// `STRAT-AI refused` at Warning. Declared expected rather than suppressed: it is the
	// difference the control exists to make the comparators see.
	AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FString Error;

	FStratBridge WithoutReel;
	FStratBridge WithReel;
	FStratBridge Differing;
	if (!TestTrue(TEXT("the no-reel bridge seeds from the shipped scenario"),
			SeedBridgeWithBuildlist(WithoutReel, Error)) ||
		!TestTrue(TEXT("the reel bridge seeds identically"),
			SeedBridgeWithBuildlist(WithReel, Error)) ||
		!TestTrue(TEXT("the control bridge seeds identically"),
			SeedBridgeWithBuildlist(Differing, Error)))
	{
		AddError(Error);
		return false;
	}

	if (!TestEqualSensitive(TEXT("the two bridges start at the same state hash"),
			WithReel.StateHash(), WithoutReel.StateHash()))
	{
		return false;
	}

	FStratBridgeAiTurnPort WithoutInner(&WithoutReel);
	FStratRecordingAiPort  WithoutPort(WithoutInner);
	FStratAiTurnRunner     WithoutRunner;
	// `nullptr` SPELLED OUT AND NOT DEFAULTED, so this call reads as the arm it is rather than
	// as the arm that happens to be the default.
	const FStratAiTurnOutcome Without = WithoutRunner.RunTurn(WithoutPort, /*OutReel=*/nullptr);

	FStratBridgeAiTurnPort WithInner(&WithReel);
	FStratRecordingAiPort  WithPort(WithInner);
	FStratAiPlaybackReel   Reel;
	FStratAiTurnRunner     WithRunner;
	const FStratAiTurnOutcome With = WithRunner.RunTurn(WithPort, &Reel);

	if (!TestTrue(TEXT("the no-reel AI turn ran to an applied EndTurn"), Without.bOk))
	{
		AddError(Without.FailureReason);
		return false;
	}
	if (!TestTrue(TEXT("the reel-carrying AI turn ran to an applied EndTurn"), With.bOk))
	{
		AddError(With.FailureReason);
		return false;
	}
	TestTrue(TEXT("the turn was not empty -- an equality over nothing proves nothing"),
		Without.CommandsApplied > 0);
	TestTrue(TEXT("the reel was genuinely filled, so the arms really did differ in the one way "
	              "this clause says makes no difference"),
		Reel.Num() > 0);

	// ---- THE POSITIVE CONTROL, BEFORE ANY EQUALITY IS READ -------------------
	FStratBridgeAiTurnPort DifferingInner(&Differing);
	FStratRecordingAiPort  DifferingPort(DifferingInner);
	DifferingPort.InjectedSubmitReason = TEXT("this test refused the submission");
	DifferingPort.RefuseSubmitAtStep   = 2;
	FStratAiTurnRunner DifferingRunner;
	const FStratAiTurnOutcome Cut = DifferingRunner.RunTurn(DifferingPort, /*OutReel=*/nullptr);

	FString ControlWhy;
	TestFalse(
		TEXT("CONTROL: the outcome comparator reports a difference between a whole turn and a "
		     "turn cut short -- a comparator that could not would pass this clause blind"),
		OutcomesAgree(Without, Cut, ControlWhy));
	AddInfo(FString::Printf(TEXT("outcome comparator on the differing pair said: '%s'"), *ControlWhy));
	TestTrue(TEXT("CONTROL: and it named where they differ"), !ControlWhy.IsEmpty());

	FString SeqControlWhy;
	TestFalse(
		TEXT("CONTROL: the sequence comparator reports a difference between those same two "
		     "submitted-command sequences"),
		SequencesAgree(WithoutPort.Accepted, DifferingPort.Accepted, SeqControlWhy));
	AddInfo(FString::Printf(TEXT("sequence comparator on the differing pair said: '%s'"), *SeqControlWhy));
	TestTrue(TEXT("CONTROL: and it named where they differ"), !SeqControlWhy.IsEmpty());

	// ---- The claim ------------------------------------------------------------
	FString Why;
	const bool bOutcomesAgree = OutcomesAgree(Without, With, Why);
	TestTrue(TEXT("passing a reel changes no field of the turn's outcome"), bOutcomesAgree);
	if (!bOutcomesAgree)
	{
		AddError(FString::Printf(TEXT("the outcomes differ: %s"), *Why));
	}

	FString SeqWhy;
	const bool bSequencesAgree = SequencesAgree(WithoutPort.Accepted, WithPort.Accepted, SeqWhy);
	TestTrue(
		TEXT("passing a reel changes no command the rules module was asked to accept, and no "
		     "field of any of them"),
		bSequencesAgree);
	if (!bSequencesAgree)
	{
		AddError(FString::Printf(TEXT("the submitted sequences differ: %s"), *SeqWhy));
	}

	TestEqual(TEXT("both ports made the same number of submissions"),
		WithPort.SubmitCalls, WithoutPort.SubmitCalls);

	TestEqualSensitive(
		TEXT("§2.11.2's presentation half changes no rules state: the two turns reached the "
		     "same hash"),
		WithReel.StateHash(), WithoutReel.StateHash());

	TestEqual(TEXT("and both bridges recorded the same number of commands"),
		WithReel.RecordedCommandCount(), WithoutReel.RecordedCommandCount());

	// The §4.10 artifact, read through the engine-typed door. See this file's header block on
	// why `RecordedLog()` is not named here.
	FStratBridge ReplayWithout;
	FStratBridge ReplayWith;
	if (!TestTrue(TEXT("two fresh bridges seed for the replay comparison"),
			SeedBridge(ReplayWithout, Error) && SeedBridge(ReplayWith, Error)))
	{
		AddError(Error);
		return false;
	}
	const FStratResult RW = WithoutReel.ReplayRecordedLogOnto(ReplayWithout);
	const FStratResult RR = WithReel.ReplayRecordedLogOnto(ReplayWith);
	if (!TestTrue(TEXT("the no-reel turn's recorded log replays"), RW.bOk))
	{
		AddError(RW.Reason);
		return false;
	}
	if (!TestTrue(TEXT("the reel-carrying turn's recorded log replays"), RR.bOk))
	{
		AddError(RR.Reason);
		return false;
	}
	TestEqualSensitive(
		TEXT("and the two recorded logs replay to the same state, which is stronger than their "
		     "counts agreeing"),
		ReplayWith.StateHash(), ReplayWithout.StateHash());

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AITURN -- THE SKIP REPORTS TRUE ONLY WHILE A TOUR IS RUNNING.
//
// `FStratAiPlaybackReel::SkipToEnd`'s return value is not a convenience.
// `AStratPlayerController::HandleSelectionEvent` returns EARLY on a `true`, consuming the
// input, so a skip that reported `true` at a quiet moment would make the board unclickable --
// and one that reported `false` while a tour ran would let the click that stopped the tour ALSO
// select a unit. THE `false` ARM IS THE ONE THAT MATTERS and it is asserted on both quiet
// states: a reel that never held anything, and a reel whose tour has finished.
//
// AND ON THE DISCRIMINATOR THAT MAKES "QUIET" TWO DIFFERENT THINGS. A default-constructed reel
// and a fully skipped one agree on `IsPlaying()`, on `GetCursor() == Num()`, and on `Peek()`
// returning null; they are distinguishable only by `Num()`. That is exactly the trap a default
// payload sets -- it reads identically to a writer that was never invoked -- and it is why
// `SkipToEnd` clears nothing, which its own declaration says in terms: "the difference between
// a skipped tour and one that never had a list, and a clause asserting the skip landed needs to
// be able to tell them apart". This clause holds the two side by side and requires `Num()` to
// tell them apart. A `SkipToEnd` rewritten to `Steps.Reset()` passes every other assertion here
// and fails that one.
//
// THE SURVIVAL GUARANTEE IS SCOPED TO A TOUR ENDING AND NOT TO A MATCH BOUNDARY, which is a
// distinction worth stating here because this clause is where a reader forms the general
// belief. `Num()` survives a skip, an exhausted tour and an `EndAiPlaybackTour` -- and it is
// deliberately ZEROED by `TearDownPresentation`, because the previous match's scratchpad is
// nobody's to read on the next match. `ReseedMidTourLeavesNoStuckTour` pins that zero. The two
// are not in tension: retiring is the cursor's business and clearing is the match boundary's,
// and the halves are split precisely because forgetting the first swallows a click while
// forgetting the second only misleads a readout.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSkipReturnsTrueOnlyWhilePlayingTest,
	"Stratocracy.StratPlay.GATE-AITURN.SkipReturnsTrueOnlyWhilePlaying",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSkipReturnsTrueOnlyWhilePlayingTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiPlaybackClauses;

	// ---- The empty reel: the ordinary state, and quiet ------------------------
	FStratAiPlaybackReel NeverUsed;
	TestEqual(TEXT("a default-constructed reel holds nothing"), NeverUsed.Num(), 0);
	TestEqual(TEXT("and its cursor is at the start"), NeverUsed.GetCursor(), 0);
	TestFalse(TEXT("an empty reel is not playing"), NeverUsed.IsPlaying());
	TestTrue(TEXT("and it has nothing to peek at"), NeverUsed.Peek() == nullptr);
	TestFalse(
		TEXT("§2.11.2's skip reports FALSE on an empty reel -- this is what stops the "
		     "controller consuming a click at a quiet moment"),
		NeverUsed.SkipToEnd());
	TestFalse(TEXT("and advancing an empty reel reports there was nothing to advance past"),
		NeverUsed.Advance());
	TestEqual(TEXT("a refused skip left the empty reel empty"), NeverUsed.Num(), 0);
	TestEqual(TEXT("and left its cursor alone"), NeverUsed.GetCursor(), 0);

	// ---- A real turn's reel ---------------------------------------------------
	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"),
			SeedBridgeWithBuildlist(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratBridgeAiTurnPort Port(&Bridge);
	FStratAiPlaybackReel   Reel;
	FStratAiTurnRunner     Runner;
	const FStratAiTurnOutcome Turn = Runner.RunTurn(Port, &Reel);
	if (!TestTrue(TEXT("the AI turn ran to an applied EndTurn"), Turn.bOk))
	{
		AddError(Turn.FailureReason);
		return false;
	}

	const int32 StepsRecorded = Reel.Num();
	if (!TestTrue(
			*FString::Printf(TEXT("the turn recorded more than one action, so a mid-tour cursor "
			                      "exists (it recorded %d)"), StepsRecorded),
			StepsRecorded > 1))
	{
		return false;
	}
	TestTrue(TEXT("a filled reel that has not been toured is playing"), Reel.IsPlaying());

	// ---- Mid-tour: the one moment the skip may report true --------------------
	TestTrue(TEXT("the tour shows its first action"), Reel.Advance());
	TestTrue(
		*FString::Printf(TEXT("the cursor is genuinely part-way (%d of %d)"),
			Reel.GetCursor(), StepsRecorded),
		Reel.GetCursor() > 0 && Reel.GetCursor() < StepsRecorded);
	TestTrue(TEXT("a part-way reel is still playing"), Reel.IsPlaying());

	TestTrue(TEXT("§2.11.2's skip reports TRUE exactly once, from a mid-tour cursor"),
		Reel.SkipToEnd());

	TestEqual(
		TEXT("the skip moved the cursor to the end and did not clear the list -- `Num()` is the "
		     "only thing that can tell a skipped tour from one that never had a list"),
		Reel.Num(), StepsRecorded);
	TestEqual(TEXT("and the cursor is exactly at the end"), Reel.GetCursor(), Reel.Num());
	TestFalse(TEXT("a skipped reel is no longer playing"), Reel.IsPlaying());
	TestTrue(TEXT("and has nothing left to peek at"), Reel.Peek() == nullptr);

	// ---- Exhausted: quiet again, and the second skip must refuse --------------
	TestFalse(
		TEXT("§2.11.2's skip reports FALSE on an exhausted reel -- a second click at that "
		     "moment belongs to the selection machine"),
		Reel.SkipToEnd());
	TestEqual(TEXT("the refused second skip changed nothing"), Reel.Num(), StepsRecorded);
	TestEqual(TEXT("and moved nothing"), Reel.GetCursor(), StepsRecorded);
	TestFalse(TEXT("and advancing past the end still reports there was nothing to advance past"),
		Reel.Advance());

	// ---- The discriminator, held side by side --------------------------------
	// The two quiet reels agree on every OTHER accessor. If they agreed on `Num()` too, no
	// clause anywhere could assert that a skip landed rather than that a reel was never filled.
	TestEqual(TEXT("the never-used reel and the skipped reel agree on IsPlaying()"),
		NeverUsed.IsPlaying(), Reel.IsPlaying());
	TestEqual(TEXT("and both have their cursor at their own end"),
		NeverUsed.GetCursor() == NeverUsed.Num(), Reel.GetCursor() == Reel.Num());
	TestTrue(
		TEXT("and `Num()` is what tells them apart: the skipped tour still knows how many "
		     "actions the AI took, and the reel that was never handed anything does not"),
		Reel.Num() > NeverUsed.Num());

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AITURN -- THE SKIP IS STILL REACHABLE AFTER THE MATCH CONCLUDES.
//
// THE ORDERING THIS CLAUSE PINS IS A PLACEMENT AND NOT A BEHAVIOUR, and no other clause can
// see it. `AStratPlayerController::HandleSelectionEvent` puts `Match->SkipAiPlayback()` AHEAD
// of `StratMatchAcceptsPlayerCommands`, whose own job is to return `false` for every event once
// §2.8 has produced a result. The AI's own command is how that result most often lands -- the
// controller's comment records it measured on 2026-08-23, "the AI's turn-7 ninth command killed
// side 0's flag" -- so the single most interesting hand-over to watch is precisely the one after
// which the match is over. Move the skip below that gate and the player is stuck watching a tour
// they cannot stop, with a green build, a green ordering clause and a green reel clause.
//
// SO THE SUBJECT IS THE CONTROLLER AND NOT THE SUBSYSTEM. Asserting `SkipAiPlayback()` returns
// true on a concluded match would pass with the gate moved anywhere at all, because that method
// does not consult the match's conclusion. The event is driven through `HandleSelectionEvent` --
// the production path -- and the assertion is that it CONSUMED it.
//
// THE NEGATIVE CONTROL IS WHAT MAKES THE `true` MEAN ANYTHING. `HandleSelectionEvent` returns
// `true` for an ordinary selection as readily as for a skip, so a lone `true` proves nothing.
// The SECOND identical event, sent when the reel is exhausted and only then, must come back
// `false` with the concluded-match gate's own sentence -- which is only possible if the skip
// gate declined to consume it. The pair is the assertion; neither half is.
//
// AND NOTHING MOVED. The bridge's `StateHash()` and `RecordedCommandCount()` are read either
// side of the consumed event: §2.11.2's skip stops a camera and is not a rules command. That is
// the subsystem-level half of `T-TURN-09.PlaybackDispositionsAgreeOnState`, which runs the same
// property down the plain-struct route.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSkipIsReachableAfterTheMatchConcludesTest,
	"Stratocracy.StratPlay.GATE-AITURN.SkipIsReachableAfterTheMatchConcludes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSkipIsReachableAfterTheMatchConcludesTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiPlaybackClauses;

	// A fixture world has no tile meshes, so the board reports it cannot draw them. The
	// rules half is all-or-nothing and the presentation half is not -- see
	// `StratMatchSubsystem.h` -- so this is an ordinary fixture condition and every
	// subsystem clause in this suite declares it.
	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	// AND THE CLOSING EndTurn OF THE TURN THAT WON THE GAME. §2.8's result lands mid-turn,
	// so the AI's own EndTurn is then refused with '[T-SAVE-05] no match is running'. That
	// is the behaviour `T-AI-01.AMatchWonMidTurnIsNotReportedAsAnAiFault` owns; this clause
	// only needs the match to have concluded, and declares the line rather than asserting
	// anything about it.
	AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestTrue(TEXT("a transient world was created"), Scope.World != nullptr))
	{
		return false;
	}

	UStratMatchSubsystem* const Match = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestTrue(TEXT("the world has a UStratMatchSubsystem"), Match != nullptr))
	{
		return false;
	}

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles from the shipped tables"),
			MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- Both sides AI, and a tour that GENUINELY ARMS -----------------------
	// A POSITIVE INTERVAL, WHICH IS A RE-FIXTURE AND IS THE WHOLE OF WHAT MOVED HERE ON
	// 2026-08-29. The first version of this clause ran at the shipped `AiPlaybackStepSeconds`
	// of zero and got "a reel playing" for free -- from the input-swallow defect, which left
	// the cursor at 0 over a non-empty reel whether or not a tour existed. The fix retires
	// the reel on every path that declines to arm a timer, so that route is gone and the
	// fixture has to configure a real tour.
	//
	// THE CLAUSE'S SUBJECT DID NOT MOVE, AND I WAS WRONG WHEN I SAID IT WOULD HAVE TO. The
	// subject is the PLACEMENT of the skip gate ahead of `StratMatchAcceptsPlayerCommands`,
	// which the 2026-08-29 ruling ordered and which the fix did not touch. What I had named
	// as the clause's subject was the fixture's ROUTE to a playing reel. The clause is
	// stronger for the change: it now tests that placement against a tour that exists.
	if (!TestTrue(TEXT("an AI-vs-AI config assembles, with a tour configured"),
			MakeAiVsAiConfig(*Match, kHarnessPlaybackInterval, Config, Error)))
	{
		AddError(Error);
		return false;
	}
	TestTrue(TEXT("the interval is positive, so `BeginAiPlayback` reaches its arming path"),
		Config.AiPlaybackStepSeconds > 0.0f);

	Match->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the AI-vs-AI match is live"), Match->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	AStratPlayerController* const Controller = Scope.World->SpawnActor<AStratPlayerController>();
	if (!TestTrue(TEXT("AStratPlayerController spawned"), Controller != nullptr))
	{
		return false;
	}

	FString    RunReason;
	const bool bRan = Match->RunAiTurnsNow(RunReason);
	AddInfo(FString::Printf(TEXT("RunAiTurnsNow returned %s; reason: '%s'"),
		bRan ? TEXT("true") : TEXT("false"), *RunReason));

	FStratViewModel Model;
	if (!TestTrue(TEXT("the view model builds after the AI-vs-AI game"),
			Match->BuildViewModel(Model, Error)))
	{
		AddError(Error);
		return false;
	}

	// THE CONCLUSION IS THE RULES MODULE'S ANSWER, through the projection, and is not predicted
	// from a turn count here.
	if (!TestTrue(TEXT("§2.8: the AI-vs-AI game reached a result under its own steam"),
			Model.Match.bHasResult))
	{
		return false;
	}

	const int32 StepsRecorded = Match->GetAiPlaybackStepCount();
	// AT LEAST TWO, AND THE BOUND IS NOT COSMETIC. `BeginAiPlayback` shows the first step
	// IMMEDIATELY rather than one interval later, so a hand-over whose reel holds a single
	// action is already exhausted by the time `RunAiTurnsNow` returns and there would be no
	// tour left for the skip to end. A clause that failed for that reason would be failing
	// for something with nothing to do with its subject, so the condition is checked and
	// reported rather than assumed.
	if (!TestTrue(
			*FString::Printf(TEXT("the concluded hand-over left at least two actions to tour, so one "
			                      "remains after the immediate first step (it left %d)"), StepsRecorded),
			StepsRecorded >= 2))
	{
		return false;
	}
	AddInfo(FString::Printf(TEXT("after RunAiTurnsNow the tour stands at %d of %d"),
		Match->GetAiPlaybackCursor(), StepsRecorded));
	TestTrue(TEXT("a tour is genuinely under way, so a skip has something to end"),
		Match->IsAiPlaybackRunning());
	TestTrue(
		TEXT("and it is part-way rather than at either end: `BeginAiPlayback` armed a timer "
		     "and showed the first action immediately"),
		Match->GetAiPlaybackCursor() > 0 && Match->GetAiPlaybackCursor() < StepsRecorded);

	FStratBridge* const Bridge = Match->GetBridge();
	if (!TestTrue(TEXT("a live match has a bridge"), Bridge != nullptr))
	{
		return false;
	}
	const FString HashBefore     = Bridge->StateHash();
	const int32   RecordedBefore = Bridge->RecordedCommandCount();

	// A hex the scenario actually has, read off the model. Every gate this event passes sits
	// ahead of any use of the hex; reading a real one is about not depending on that ordering.
	const FIntPoint Hex = Model.Units.Num() > 0 ? Model.Units[0].Hex : FIntPoint::ZeroValue;

	// ---- The event: Escape, on a match that is over --------------------------
	// `EStratSelectionEvent::Cancel` is what "secondary click, or Escape" becomes, per its own
	// declaration. §2.11.2 says "any click or Esc".
	FString    FirstReason;
	const bool bFirst = Controller->HandleSelectionEvent(EStratSelectionEvent::Cancel, Hex, FirstReason);

	TestTrue(
		TEXT("§2.11.2's skip is reachable after the match concludes: the Escape was consumed "
		     "rather than refused by the concluded-match gate"),
		bFirst);
	TestTrue(
		*FString::Printf(TEXT("a consumed skip is not a refusal and owes no sentence (it said '%s')"),
			*FirstReason),
		FirstReason.IsEmpty());
	TestEqual(TEXT("and the tour it ended is at its end"),
		Match->GetAiPlaybackCursor(), StepsRecorded);
	TestEqual(TEXT("the skip cleared nothing"),
		Match->GetAiPlaybackStepCount(), StepsRecorded);
	TestFalse(TEXT("nothing is playing after the skip"), Match->IsAiPlaybackRunning());

	// ---- Nothing moved -------------------------------------------------------
	TestEqualSensitive(
		TEXT("§2.11.2's skip is presentation only: the state hash is exactly where it was"),
		Bridge->StateHash(), HashBefore);
	TestEqual(TEXT("and it recorded no command"),
		Bridge->RecordedCommandCount(), RecordedBefore);

	// ---- THE NEGATIVE CONTROL ------------------------------------------------
	// The identical event, now that the reel is exhausted. If the skip gate consumed this one
	// too, the `true` above said nothing about the gate's placement. It must fall through to
	// `StratMatchAcceptsPlayerCommands` and be refused there, WITH that gate's own sentence.
	TestFalse(TEXT("the subsystem's skip now refuses, so the gate is open"),
		Match->SkipAiPlayback());

	FString    SecondReason;
	const bool bSecond = Controller->HandleSelectionEvent(EStratSelectionEvent::Cancel, Hex, SecondReason);

	TestFalse(
		TEXT("CONTROL: with the tour over, the same event falls through the skip gate and is "
		     "refused by the concluded-match gate -- which is what makes the `true` above a "
		     "statement about the skip and not about the controller returning true generally"),
		bSecond);
	TestFalse(
		*FString::Printf(TEXT("CONTROL: and that refusal owes the player a sentence (it said '%s')"),
			*SecondReason),
		SecondReason.IsEmpty());

	TestEqualSensitive(
		TEXT("the refused event moved nothing either"),
		Bridge->StateHash(), HashBefore);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AITURN -- THE SHIPPED DEFAULT CONSUMES NO INPUT. THE REGRESSION CLAUSE.
//
// THE DEFECT THIS EXISTS FOR SHIPPED GREEN AND WAS FOUND BY A CLAUSE THAT PASSED WHILE
// MEASURING IT. `UStratMatchSubsystem::RunAiTurnsNow` fills `AiPlaybackReel` on EVERY
// hand-over -- deliberately, so `GetAiPlaybackStepCount()` answers whatever the configuration
// -- while only the TIMER was gated on `AiPlaybackStepSeconds`. At the shipped default of
// zero, `BeginAiPlayback` returned on its first line and left a non-empty reel with the cursor
// at 0. `IsAiPlaybackRunning()` read TRUE, `SkipAiPlayback()` returned TRUE, and
// `AStratPlayerController::HandleSelectionEvent` returned early on it -- so the first click or
// Esc after EVERY AI hand-over in the shipped configuration was swallowed by a tour that never
// ran. The fix retires the reel on all three paths where `BeginAiPlayback` declines to arm a
// timer.
//
// FOUR READS, AND THE FOURTH IS WHAT STOPS THE OTHER THREE PASSING FOR THE WRONG REASON.
// `IsAiPlaybackRunning() == false`, `SkipAiPlayback() == false` and
// `GetAiPlaybackCursor() == GetAiPlaybackStepCount()` are ALL satisfied by a reel that was
// never filled at all -- a `Reset()` in place of the retirement, an AI that did nothing, a
// hand-over that never happened. So `GetAiPlaybackStepCount() > 0` is asserted first: the
// clause is about a reel that HAS contents and is nonetheless quiet, which is the only state
// that distinguishes the fix from throwing the list away. Same shape as
// `SkipReturnsTrueOnlyWhilePlaying`'s discriminator, and the same reason: a default read
// cannot tell silence from an empty call.
//
// AND IT IS ASSERTED AT THE CONTROLLER TOO, because the subsystem's `false` is only half the
// claim. The regression's symptom was an INPUT being consumed, so the clause sends one and
// requires it NOT to be: the Esc must fall through the skip gate and reach the concluded-match
// gate, which refuses it with a sentence. BEFORE THE FIX this same call returned `true` with
// an EMPTY reason -- which is what `SkipIsReachableAfterTheMatchConcludes` asserts and is
// correct there, because in that clause a tour is genuinely under way. The two clauses are a
// matched pair: identical fixtures but for `AiPlaybackStepSeconds`, and OPPOSITE expectations
// of the same controller call. Neither is meaningful alone; a fix that made both `false` or
// both `true` fails one of them.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratDefaultConfigConsumesNoInputTest,
	"Stratocracy.StratPlay.GATE-AITURN.DefaultConfigConsumesNoInput",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratDefaultConfigConsumesNoInputTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiPlaybackClauses;

	// A fixture world has no tile meshes, so the board reports it cannot draw them. The
	// rules half is all-or-nothing and the presentation half is not, so this is an ordinary
	// fixture condition and every subsystem clause in this suite declares it. DECLARED
	// FROM A MEASURED RUN AND NOT SPECULATIVELY: `AddExpectedMessagePlain` with
	// `Occurrences 0` FAILS a clause where the message never fires, so a defensive
	// declaration is itself an assertion.
	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	// THE CLOSING EndTurn OF THE TURN THAT WON THE GAME. A result lands mid-turn, so the
	// AI's own EndTurn is then refused with '[T-SAVE-05] no match is running'. That is the
	// behaviour `T-AI-01.AMatchWonMidTurnIsNotReportedAsAnAiFault` owns; this clause only
	// needs the match to have concluded, and declares the line rather than asserting
	// anything about it.
	AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestTrue(TEXT("a transient world was created"), Scope.World != nullptr))
	{
		return false;
	}

	UStratMatchSubsystem* const Match = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestTrue(TEXT("the world has a UStratMatchSubsystem"), Match != nullptr))
	{
		return false;
	}

	// THE SHIPPED DEFAULT, PASSED EXPLICITLY RATHER THAN LEFT UNSET, so this reads as the arm
	// it is. It is asserted below against the field's own default rather than against `0.0f`
	// written here, so a re-defaulted field moves this clause's fixture with it.
	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("an AI-vs-AI config assembles at the shipped playback default"),
			MakeAiVsAiConfig(*Match, FStratMatchConfig().AiPlaybackStepSeconds, Config, Error)))
	{
		AddError(Error);
		return false;
	}
	TestTrue(
		TEXT("the shipped default is non-positive, so `BeginAiPlayback` takes its first exit "
		     "and arms nothing"),
		Config.AiPlaybackStepSeconds <= 0.0f);

	Match->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the AI-vs-AI match is live"), Match->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	AStratPlayerController* const Controller = Scope.World->SpawnActor<AStratPlayerController>();
	if (!TestTrue(TEXT("AStratPlayerController spawned"), Controller != nullptr))
	{
		return false;
	}

	FString    RunReason;
	const bool bRan = Match->RunAiTurnsNow(RunReason);
	AddInfo(FString::Printf(TEXT("RunAiTurnsNow returned %s; reason: '%s'"),
		bRan ? TEXT("true") : TEXT("false"), *RunReason));

	FStratViewModel Model;
	if (!TestTrue(TEXT("the view model builds after the AI-vs-AI game"),
			Match->BuildViewModel(Model, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- The read that stops the rest passing for the wrong reason -----------
	const int32 StepsRecorded = Match->GetAiPlaybackStepCount();
	if (!TestTrue(
			*FString::Printf(
				TEXT("the hand-over DID record actions, so the quiet below is a retired reel and "
				     "not an empty one (it recorded %d)"), StepsRecorded),
			StepsRecorded > 0))
	{
		return false;
	}

	// ---- The three reads the fix is about ------------------------------------
	TestFalse(
		TEXT("at the shipped default nothing is playing, though the list survives"),
		Match->IsAiPlaybackRunning());
	TestEqual(
		TEXT("the cursor was retired to the end rather than left at the start"),
		Match->GetAiPlaybackCursor(), StepsRecorded);
	TestFalse(
		TEXT("so the skip the controller consults reports FALSE and consumes nothing"),
		Match->SkipAiPlayback());
	TestFalse(
		TEXT("and there is nothing at the cursor to step to"),
		Match->AdvanceAiPlaybackOneStep());
	TestEqual(
		TEXT("none of those reads moved the list"),
		Match->GetAiPlaybackStepCount(), StepsRecorded);

	// ---- And at the controller, which is where the symptom was ---------------
	if (!TestTrue(TEXT("the concluded state is the rules module's answer"), Model.Match.bHasResult))
	{
		return false;
	}

	FStratBridge* const Bridge = Match->GetBridge();
	if (!TestTrue(TEXT("a live match has a bridge"), Bridge != nullptr))
	{
		return false;
	}
	const FString HashBefore = Bridge->StateHash();

	const FIntPoint Hex = Model.Units.Num() > 0 ? Model.Units[0].Hex : FIntPoint::ZeroValue;

	FString    Reason;
	const bool bConsumed = Controller->HandleSelectionEvent(EStratSelectionEvent::Cancel, Hex, Reason);

	TestFalse(
		TEXT("THE REGRESSION: at the shipped default the first Esc after an AI hand-over is NOT "
		     "consumed by the skip gate -- it falls through to the concluded-match gate. Before "
		     "the 2026-08-29 fix this same call returned true"),
		bConsumed);
	TestFalse(
		*FString::Printf(
			TEXT("and it is refused with the concluded-match gate's own sentence rather than "
			     "silently swallowed (it said '%s')"), *Reason),
		Reason.IsEmpty());

	TestEqualSensitive(TEXT("and nothing moved"), Bridge->StateHash(), HashBefore);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AITURN -- NO WORLD LEAVES NO STUCK TOUR.
//
// WITHOUT THIS CLAUSE THE FIX AND THE REJECTED ALTERNATIVE ARE INDISTINGUISHABLE, and that is
// the entire reason it exists. Two repairs were available for the input-swallow defect. Gating
// `SkipAiPlayback` on `AiPlaybackStepSeconds` fixes the zero case and nothing else. Putting the
// invariant in `BeginAiPlayback` -- the only function that ARMS the timer, and therefore the
// only one that knows whether a tour will start -- covers every reason not to arm one. THE
// THIRD EXIT IS WHERE THE TWO REPAIRS COME APART: a POSITIVE interval with no world to time
// against arms nothing either, and a config gate would have read that state as a tour under
// way. This clause is that state, and a config gate reddens it while passing
// `DefaultConfigConsumesNoInput`.
//
// [SUPERSEDED IN SCOPE 2026-08-29, LATER THE SAME DAY, AND THE OLD SENTENCE IS LEFT STANDING
// ABOVE BECAUSE IT IS EXACTLY THE BELIEF THAT MISSED THE SECOND DEFECT. "Covers every reason
// not to arm" was true and was NOT the whole invariant: it says nothing about the reasons to
// STOP. `StopAiPlaybackTimer` cleared the clock and left the cursor, and two of its call sites
// -- `TearDownPresentation` and `Deinitialize` -- ended a tour without retiring, so a reseed
// taken mid-tour stranded the cursor on a BRAND-NEW match and swallowed its first input,
// carrying the previous match's step list. THE INVARIANT NOW HAS TWO OWNERS AND THE CLASS
// NAMES BOTH: `BeginAiPlayback` arms, `EndAiPlaybackTour` disarms, and there is no verb left
// that stops the clock alone. `ReseedMidTourLeavesNoStuckTour` is that half's clause; this one
// remains the arming half's and its subject did not move.]
//
// A `NewObject` SUBSYSTEM AND NOT A DESTROYED WORLD. `UStratMatchSubsystem` is a
// `UWorldSubsystem`, so one obtained through `UWorld::GetSubsystem` always has a world;
// constructing one directly is the only way to reach `GetWorld() == nullptr`. It has no
// `Initialize` override, so a directly-constructed instance is as usable as a world's own.
// THE ABSENCE IS ASSERTED BEFORE ANYTHING IS READ INTO IT: a subsystem that turned out to have
// a world would satisfy every assertion below for a completely different reason.
//
// AND THE ACTOR CLASSES ARE NULLED, WHICH IS A DOCUMENTED CONFIGURATION AND NOT A DODGE.
// `StartMatchInternal` reaches `GetWorld()->SpawnActor` only inside `if (BoardActorClass !=
// nullptr)`, and its own comment calls the null case "a legitimate configuration -- a map that
// wants a seeded match and a scoreboard and no 3D board". The rules half is unaffected: this is
// the same seeded bridge and the same AI-vs-AI game the other subsystem clauses play.
//
// THE SIDES STILL COME OFF THE SCENARIO. A throwaway world is created solely so
// `MakeAiVsAiConfig` can ask a live subsystem which sides the scenario deploys units for. The
// worldless subsystem then plays that config. Nothing about the roster is decided here.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratNoWorldLeavesNoStuckTourTest,
	"Stratocracy.StratPlay.GATE-AITURN.NoWorldLeavesNoStuckTour",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratNoWorldLeavesNoStuckTourTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiPlaybackClauses;

	// A fixture world has no tile meshes, so the board reports it cannot draw them. The
	// rules half is all-or-nothing and the presentation half is not, so this is an ordinary
	// fixture condition and every subsystem clause in this suite declares it. DECLARED
	// FROM A MEASURED RUN AND NOT SPECULATIVELY: `AddExpectedMessagePlain` with
	// `Occurrences 0` FAILS a clause where the message never fires, so a defensive
	// declaration is itself an assertion.
	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	// AND THE NO-3D-BOARD VERDICT, which this clause CAUSES on purpose: nulling
	// `BoardActorClass` is how a worldless subsystem is reachable at all, and
	// `StartMatchInternal` reports that configuration rather than refusing it.
	AddExpectedMessagePlain(TEXT("no BoardActorClass is set"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	// THE CLOSING EndTurn OF THE TURN THAT WON THE GAME. A result lands mid-turn, so the
	// AI's own EndTurn is then refused with '[T-SAVE-05] no match is running'. That is the
	// behaviour `T-AI-01.AMatchWonMidTurnIsNotReportedAsAnAiFault` owns; this clause only
	// needs the match to have concluded, and declares the line rather than asserting
	// anything about it.
	AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FStratMatchConfig Config;
	FString           Error;

	// ---- A throwaway world, used only to ask the scenario for its sides ------
	{
		FTestWorldScope Probe;
		if (!TestTrue(TEXT("a probe world was created"), Probe.World != nullptr))
		{
			return false;
		}
		UStratMatchSubsystem* const ProbeMatch = Probe.World->GetSubsystem<UStratMatchSubsystem>();
		if (!TestTrue(TEXT("the probe world has a UStratMatchSubsystem"), ProbeMatch != nullptr))
		{
			return false;
		}
		if (!TestTrue(TEXT("an AI-vs-AI config assembles, with a tour configured"),
				MakeAiVsAiConfig(*ProbeMatch, kHarnessPlaybackInterval, Config, Error)))
		{
			AddError(Error);
			return false;
		}
	}

	// See this clause's header block: the documented no-3D-board configuration, so nothing in
	// `StartMatchInternal` or `RefreshPresentation` reaches for a world that is not there.
	Config.BoardActorClass = nullptr;
	Config.UnitActorClass  = nullptr;

	TestTrue(
		TEXT("the interval is POSITIVE -- this clause is not a second spelling of the shipped "
		     "default, and a repair that only gated the config would leave this state stuck"),
		Config.AiPlaybackStepSeconds > 0.0f);

	// ---- The subsystem with no world ----------------------------------------
	// Held strongly so nothing collects it mid-clause; it has no world to own it.
	// The default outer is the transient package -- `NewObject<T>()`'s own default,
	// spelled as the no-argument form because `UPackage` is not a complete type in this
	// translation unit and including `UObject/Package.h` to name a default explicitly
	// would be a second thing to keep true.
	TStrongObjectPtr<UStratMatchSubsystem> Worldless(NewObject<UStratMatchSubsystem>());
	if (!TestTrue(TEXT("a UStratMatchSubsystem was constructed directly"),
			Worldless.IsValid()))
	{
		return false;
	}

	// THE CONTROL, AND IT RUNS BEFORE ANYTHING IS READ INTO THE QUIET BELOW.
	if (!TestTrue(TEXT("and it genuinely has no world -- the condition this clause is about"),
			Worldless->GetWorld() == nullptr))
	{
		return false;
	}

	Worldless->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the match is live on the rules side even with no board and no world"),
			Worldless->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	FString    RunReason;
	const bool bRan = Worldless->RunAiTurnsNow(RunReason);
	AddInfo(FString::Printf(TEXT("RunAiTurnsNow returned %s; reason: '%s'"),
		bRan ? TEXT("true") : TEXT("false"), *RunReason));

	// ---- The same three reads, and the same discriminator --------------------
	const int32 StepsRecorded = Worldless->GetAiPlaybackStepCount();
	if (!TestTrue(
			*FString::Printf(
				TEXT("the hand-over DID record actions, so the quiet below is a retired reel and "
				     "not an empty one (it recorded %d)"), StepsRecorded),
			StepsRecorded > 0))
	{
		return false;
	}

	TestFalse(
		TEXT("a positive interval with no clock to run it reports no tour under way"),
		Worldless->IsAiPlaybackRunning());
	TestEqual(
		TEXT("the cursor was retired to the end rather than left waiting on a timer that was "
		     "never armed"),
		Worldless->GetAiPlaybackCursor(), StepsRecorded);
	TestFalse(
		TEXT("so the skip reports FALSE and would consume no input"),
		Worldless->SkipAiPlayback());
	TestFalse(
		TEXT("and there is nothing at the cursor to step to"),
		Worldless->AdvanceAiPlaybackOneStep());
	TestEqual(
		TEXT("and the list survived, which is what makes this a retirement and not a clearing"),
		Worldless->GetAiPlaybackStepCount(), StepsRecorded);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AITURN -- ONE STEP PER CALL, AND THE TOUR STOPS ON THE LAST.
//
// WHAT THIS REACHES THAT NOTHING PREVIOUSLY COULD. Before `AdvanceAiPlaybackOneStep` existed,
// the step-and-stop body lived in `OnAiPlaybackTimer`, reachable only through `FTimerManager`
// -- so the camera-stepping half of the playback was executed by no test at all, and a green
// suite said nothing about it. This clause is the first thing to execute `FocusPlaybackStep`,
// the advance, and the stop on the last step.
//
// AND IT IS ASSERTING THE TIMER'S BEHAVIOUR, NOT A COPY OF IT. `OnAiPlaybackTimer` is now one
// line calling this same method, so there is ONE driver with two entry points. Had the engineer
// added a separate test-only stepping path, this clause would have been pinning a duplicate
// that could drift from the shipping one -- which is the substitution the project's own
// `T-UI-02` exists to catch, and it is worth naming because the shape is easy to reach for.
//
// THE TOUR IS ARMED FOR REAL. A positive interval and a world, so `BeginAiPlayback` takes its
// arming path and shows the first action immediately; the cursor is therefore at 1 and not 0
// when this clause starts stepping, and that is asserted rather than assumed. The world is
// never ticked -- `bInformEngineOfWorld` is false and the harness interval is ten minutes --
// so every advance below is one this clause asked for.
//
// WHAT IT CANNOT ASSERT, STATED RATHER THAN QUIETLY OMITTED: that no timer handle is left
// armed after the last step. Every `FTimerManager` query is keyed on an `FTimerHandle`, and
// `UStratMatchSubsystem::AiPlaybackTimer` is private, so there is no route to that fact from
// `Tests/`. It is also without a behavioural consequence here -- `AdvanceAiPlaybackOneStep`'s
// null-`Peek` arm calls `EndAiPlaybackTour()` too, so a surviving handle would clear itself on
// its next tick. Asserting it would need a seam (`bool IsAiPlaybackTimerArmed() const`) and
// none was added. The clause asserts the observable consequences instead and says so here.
//
// [NAME CORRECTED 2026-08-29. THE SENTENCE ABOVE SAID `StopAiPlaybackTimer()`, WHICH NO LONGER
// EXISTS -- it is `EndAiPlaybackTour()` since the reseed defect, and it now retires the reel as
// well as clearing the clock. The reasoning is unchanged and if anything stronger: the
// retirement half is a no-op on this path, because the cursor is already at the end when this
// clause reaches it. Recorded rather than silently reworded, because a comment naming a symbol
// that has been renamed compiles clean and reads as current.]
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratStepFocusesAndStopsOnTheLastTest,
	"Stratocracy.StratPlay.GATE-AITURN.StepFocusesAndStopsOnTheLast",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratStepFocusesAndStopsOnTheLastTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiPlaybackClauses;

	// A fixture world has no tile meshes, so the board reports it cannot draw them. The
	// rules half is all-or-nothing and the presentation half is not, so this is an ordinary
	// fixture condition and every subsystem clause in this suite declares it. DECLARED
	// FROM A MEASURED RUN AND NOT SPECULATIVELY: `AddExpectedMessagePlain` with
	// `Occurrences 0` FAILS a clause where the message never fires, so a defensive
	// declaration is itself an assertion.
	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	// THE CLOSING EndTurn OF THE TURN THAT WON THE GAME. A result lands mid-turn, so the
	// AI's own EndTurn is then refused with '[T-SAVE-05] no match is running'. That is the
	// behaviour `T-AI-01.AMatchWonMidTurnIsNotReportedAsAnAiFault` owns; this clause only
	// needs the match to have concluded, and declares the line rather than asserting
	// anything about it.
	AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestTrue(TEXT("a transient world was created"), Scope.World != nullptr))
	{
		return false;
	}

	UStratMatchSubsystem* const Match = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestTrue(TEXT("the world has a UStratMatchSubsystem"), Match != nullptr))
	{
		return false;
	}

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("an AI-vs-AI config assembles, with a tour configured"),
			MakeAiVsAiConfig(*Match, kHarnessPlaybackInterval, Config, Error)))
	{
		AddError(Error);
		return false;
	}

	Match->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the AI-vs-AI match is live"), Match->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	FString    RunReason;
	const bool bRan = Match->RunAiTurnsNow(RunReason);
	AddInfo(FString::Printf(TEXT("RunAiTurnsNow returned %s; reason: '%s'"),
		bRan ? TEXT("true") : TEXT("false"), *RunReason));

	const int32 StepsRecorded = Match->GetAiPlaybackStepCount();
	if (!TestTrue(
			*FString::Printf(TEXT("the hand-over recorded at least two actions, so a step remains "
			                      "after the immediate first one (it recorded %d)"), StepsRecorded),
			StepsRecorded >= 2))
	{
		return false;
	}

	// ---- The arming path ran, and showed one action on its way through -------
	TestEqual(
		TEXT("`BeginAiPlayback` showed the first action immediately rather than one interval "
		     "later, so the cursor is at 1"),
		Match->GetAiPlaybackCursor(), 1);
	TestTrue(TEXT("and the tour is under way"), Match->IsAiPlaybackRunning());

	FStratBridge* const Bridge = Match->GetBridge();
	if (!TestTrue(TEXT("a live match has a bridge"), Bridge != nullptr))
	{
		return false;
	}
	const FString HashBefore     = Bridge->StateHash();
	const int32   RecordedBefore = Bridge->RecordedCommandCount();

	// ---- One advance per call, to the end ------------------------------------
	// The loop is bounded by the recorded count and the bound is checked afterwards, so a
	// method that returned true forever ends this clause with a failure rather than a hang.
	int32 Calls = 0;
	while (Match->GetAiPlaybackCursor() < StepsRecorded && Calls <= StepsRecorded)
	{
		const int32 CursorBefore = Match->GetAiPlaybackCursor();
		if (!TestTrue(
				*FString::Printf(TEXT("the step at cursor %d was shown"), CursorBefore),
				Match->AdvanceAiPlaybackOneStep()))
		{
			break;
		}
		++Calls;
		if (!TestEqual(
				*FString::Printf(TEXT("one call moved the cursor exactly one place, from %d"),
					CursorBefore),
				Match->GetAiPlaybackCursor(), CursorBefore + 1))
		{
			break;
		}
	}

	TestEqual(
		TEXT("it took exactly one call per action still to show -- the first was shown by the "
		     "arming path"),
		Calls, StepsRecorded - 1);
	TestEqual(TEXT("and the tour is at its end"),
		Match->GetAiPlaybackCursor(), StepsRecorded);

	// ---- Stopped, and stopped for good ---------------------------------------
	TestFalse(TEXT("nothing is playing once the last action has been shown"),
		Match->IsAiPlaybackRunning());
	TestFalse(TEXT("a further step reports there was nothing at the cursor"),
		Match->AdvanceAiPlaybackOneStep());
	TestEqual(TEXT("and it moved nothing"),
		Match->GetAiPlaybackCursor(), StepsRecorded);
	TestFalse(
		TEXT("and the skip refuses: a tour driven to its end leaves exactly the quiet state a "
		     "skipped one leaves, so a click at this moment reaches the selection machine"),
		Match->SkipAiPlayback());
	TestEqual(TEXT("the list survived the whole tour"),
		Match->GetAiPlaybackStepCount(), StepsRecorded);

	// ---- And the tour moved nothing it is not allowed to move -----------------
	TestEqualSensitive(
		TEXT("stepping the camera through the AI's actions changed no rules state"),
		Bridge->StateHash(), HashBefore);
	TestEqual(TEXT("and recorded no command"),
		Bridge->RecordedCommandCount(), RecordedBefore);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AITURN -- A RESEED TAKEN MID-TOUR LEAVES NO STUCK TOUR.
//
// THE SECOND INSTANCE OF THE SWALLOW DEFECT, AND THE MORE INTERESTING ONE, because the class
// prose asserted the property the code lacked. `BeginAiPlayback` established "the cursor is at
// the end unless a tour is actually under way" over every reason NOT TO ARM a timer -- and said
// nothing about the reasons to STOP one. `StopAiPlaybackTimer` cleared the clock and left the
// cursor where it was, and two of its call sites ended a tour that way: `TearDownPresentation`
// and `Deinitialize`. `StartMatchInternal` calls `TearDownPresentation` unconditionally and
// `LoadMatchFromSlot` reaches `StartMatchInternal` -- both verified in this tree -- so those are
// the two routes a player takes between two matches. Start a new match or load a slot while the
// AI's tour was running and the BRAND-NEW match began with `IsAiPlaybackRunning()` true,
// `SkipAiPlayback()` true, and the first click or Esc swallowed, over the PREVIOUS match's step
// list.
//
// THE FIX IS A RENAME WITH TEETH: `EndAiPlaybackTour` clears the timer and retires the reel
// atomically, and there is no verb left in the class that stops the clock alone. This clause is
// what makes that guarantee a gate rather than a promise, and it fails on the pre-fix code at
// all three of its assertions.
//
// **THE FIXTURE HAS A FALSE-PASS HAZARD AND IT IS SHARPER THAN THE ONE THAT BIT THE LAST
// CLAUSE.** `BeginAiPlayback` shows the first step IMMEDIATELY rather than one interval later.
// So a hand-over whose reel holds ONE action -- and the closing `EndTurn` is itself recorded, so
// a turn that only ends gives exactly one -- self-completes BEFORE `RunAiTurnsNow` returns:
// cursor at the end, timer already stopped, no live tour at the moment of the reseed. A clause
// fixtured that way would be reseeding an ALREADY-RETIRED reel, would assert nothing, and WOULD
// HAVE GONE GREEN ON THE DEFECTIVE CODE. `Num() >= 2` is therefore mandatory and is checked and
// reported rather than assumed.
//
// AND THE POSITIVE CONTROL IS WHAT MAKES THE QUIET AFTERWARDS MEAN ANYTHING. Every assertion
// after the reseed is an ABSENCE, and an absence proves nothing until the instrument has been
// shown able to speak. So `IsAiPlaybackRunning()` is required to be TRUE immediately before the
// reseed. Without it the clause cannot tell "the reseed retired a live tour" from "no tour ever
// started" -- which is what an absent world, a config that never took, or the one-action reel
// above would each produce, and each of those is a green clause that pins nothing.
//
// THE COUNT IS PINNED AT ZERO AND NOT AT THE PREVIOUS MATCH'S VALUE, deliberately.
// `TearDownPresentation` calls `AiPlaybackReel.Reset()` on top of the retirement, because the
// previous match's scratchpad is nobody's to read on the next match; leaving it readable through
// `GetAiPlaybackStepCount()` is a cross-match leak that cannot swallow an input but can mislead
// a readout. A clause asserting the count SURVIVED a reseed would be asserting that leak. This
// is the one place in this file where the list is required NOT to survive, and it does not
// contradict `SkipReturnsTrueOnlyWhilePlaying`: that clause's survival guarantee is about a TOUR
// ending, and this is a MATCH boundary.
//
// THE RESEED GOES THROUGH `StartMatch`, one of the two real player routes into
// `TearDownPresentation`. Tearing down some other way -- destroying the world, calling a
// narrower verb -- might not cross the defective path at all, which is how a regression clause
// ends up green against the bug it was written for.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratReseedMidTourLeavesNoStuckTourTest,
	"Stratocracy.StratPlay.GATE-AITURN.ReseedMidTourLeavesNoStuckTour",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratReseedMidTourLeavesNoStuckTourTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAiPlaybackClauses;

	// Two matches are started in this clause, so the board reports its missing tile meshes
	// more than once. Declared from a measured run and not speculatively -- see the note at
	// `DefaultConfigConsumesNoInput`.
	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	// The closing EndTurn of the turn that won the first game, refused with
	// '[T-SAVE-05] no match is running'. Owned by
	// `T-AI-01.AMatchWonMidTurnIsNotReportedAsAnAiFault`; declared here, asserted nowhere here.
	AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestTrue(TEXT("a transient world was created"), Scope.World != nullptr))
	{
		return false;
	}

	UStratMatchSubsystem* const Match = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestTrue(TEXT("the world has a UStratMatchSubsystem"), Match != nullptr))
	{
		return false;
	}

	// A POSITIVE INTERVAL, because this clause needs a tour that genuinely arms. At the shipped
	// default `BeginAiPlayback` retires on its way out and there would be no live tour for a
	// reseed to strand -- which is `DefaultConfigConsumesNoInput`'s subject and not this one's.
	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("an AI-vs-AI config assembles, with a tour configured"),
			MakeAiVsAiConfig(*Match, kHarnessPlaybackInterval, Config, Error)))
	{
		AddError(Error);
		return false;
	}
	TestTrue(TEXT("the interval is positive, so `BeginAiPlayback` reaches its arming path"),
		Config.AiPlaybackStepSeconds > 0.0f);

	Match->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the first match is live"), Match->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	FString    RunReason;
	const bool bRan = Match->RunAiTurnsNow(RunReason);
	AddInfo(FString::Printf(TEXT("RunAiTurnsNow returned %s; reason: '%s'"),
		bRan ? TEXT("true") : TEXT("false"), *RunReason));

	// ---- The fixture condition that would otherwise fake a pass ---------------
	const int32 StepsRecorded = Match->GetAiPlaybackStepCount();
	if (!TestTrue(
			*FString::Printf(
				TEXT("the hand-over recorded at least TWO actions, so a tour is still under way "
				     "when the reseed lands -- at one action it self-completes before "
				     "RunAiTurnsNow returns and this clause would pass on the defective code "
				     "(it recorded %d)"), StepsRecorded),
			StepsRecorded >= 2))
	{
		return false;
	}

	// ---- THE POSITIVE CONTROL. Everything after the reseed is an absence. -----
	AddInfo(FString::Printf(TEXT("before the reseed the tour stands at %d of %d"),
		Match->GetAiPlaybackCursor(), StepsRecorded));
	if (!TestTrue(
			TEXT("CONTROL: a tour is genuinely under way immediately before the reseed -- without "
			     "this the quiet below could be 'no tour ever started'"),
			Match->IsAiPlaybackRunning()))
	{
		return false;
	}
	if (!TestTrue(
			*FString::Printf(TEXT("CONTROL: and its cursor is part-way, at %d of %d"),
				Match->GetAiPlaybackCursor(), StepsRecorded),
			Match->GetAiPlaybackCursor() > 0 && Match->GetAiPlaybackCursor() < StepsRecorded))
	{
		return false;
	}

	// ---- The reseed, by a route a player actually takes ----------------------
	// `StartMatch` -> `StartMatchInternal` -> `TearDownPresentation`, which is one of the two
	// call sites the defect came through. `LoadMatchFromSlot` reaches the same function through
	// the same `StartMatchInternal` -- verified in this tree -- so this crosses the path both
	// player routes cross.
	FString RestartError;
	Match->StartMatch(Config, RestartError);
	if (!TestTrue(TEXT("the reseeded match is live"), Match->IsMatchLive()))
	{
		AddError(RestartError);
		return false;
	}

	// ---- The three reads the fix is about ------------------------------------
	TestFalse(
		TEXT("a brand-new match reports no tour under way, though one was running when it "
		     "started"),
		Match->IsAiPlaybackRunning());

	TestFalse(
		TEXT("so the predicate `AStratPlayerController::HandleSelectionEvent` branches on "
		     "reports FALSE, and the new match's first click or Esc is not consumed"),
		Match->SkipAiPlayback());

	TestEqual(
		TEXT("and the previous match's step list is gone rather than readable on the new match "
		     "-- a count that survived here would be a cross-match leak"),
		Match->GetAiPlaybackStepCount(), 0);

	// ---- And the three of them agree with each other -------------------------
	TestEqual(TEXT("the cursor is at the end of an empty list"),
		Match->GetAiPlaybackCursor(), Match->GetAiPlaybackStepCount());
	TestFalse(TEXT("and there is nothing at the cursor to step to"),
		Match->AdvanceAiPlaybackOneStep());
	TestEqual(TEXT("none of those reads resurrected anything"),
		Match->GetAiPlaybackStepCount(), 0);

	return true;
}
