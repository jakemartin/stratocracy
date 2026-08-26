// GDD §2.11.4 / §2.8 -- the TRIGGER for the end-of-match screen. T-INT-05.
//
// WHY T-INT-05 AND NOT THE `T-TURN-04` THE ENGINEER PROPOSED, AND THE ID WAS READ RATHER THAN
// ASSUMED. `T-TURN-04` is the GDD's Invariants row "at the turn cap, the attrition tiebreak
// resolves in the exact §2.8 order: combat Fame -> objectives held -> surviving HP -> draw". That
// is a rules-module property, it is already pinned in
// `Source/StratBridge/Tests/StratMatchResultClauses.cpp`, and it says nothing whatever about a
// screen appearing once. Filing this clause under it would have made a grep for T-TURN-04's
// coverage return a test about widget lifetime.
//
// T-INT-05 is the row this clause belongs to, and the tree already says so:
// `Source/StratPlay/Tests/StratMatchConclusion.cpp` files the whole end-of-match transition under
// T-INT-05, including `ConcludingLogsOnceAndOnlyOnce`, which counts the very latch this clause
// rides on. This is that transition's third act -- the screen -- measured beside the log line it
// is emitted next to.
//
// THE SUBJECT IS THE COMBINATION AND THAT IS THE WHOLE POINT OF ONE CLAUSE RATHER THAN TWO.
// §2.11.4's screen must go up ONCE per match and must be GONE when the next match starts. Either
// half alone is satisfied by a broken implementation:
//   - "shown once" alone passes on a `StartMatchInternal` that clears `bMatchConclusionAnnounced`
//     and leaves the last match's verdict sitting on top of the new board. Clearing a bool
//     removes nothing from the viewport, and the widget outlives `TearDownPresentation` because
//     it belongs to the HUD rather than to the subsystem.
//   - "cleared on restart" alone passes on a `ConcludeMatchIfEnded` with no latch at all, which
//     re-creates and re-pushes the screen on every presentation refresh for the rest of the
//     process -- and a WBP with an intro animation would replay it, forever, under the player's
//     cursor.
// A trigger that is unobservable except by combination is a defect class this project has
// shipped: §2.11.6-B's beat 2 watched for a state that lived only inside one stack frame, neither
// half was wrong alone, and no clause covered the combination. This clause covers it.
//
// WHERE EVERY EXPECTATION COMES FROM.
//   - "SHOWN ONCE" IS COUNTED, NOT ARGUED. `ConcludeMatchIfEnded` logs
//     `No end-of-match screen this match: <reason>` at Warning on every refusal from
//     `AStratScoreboardHUD::ShowMatchResult`, and headlessly that call ALWAYS refuses -- there is
//     no `MatchResultWidgetClass` on a HUD with no Blueprint defaults. So the count of that line
//     IS the count of `ShowMatchResult` calls, exactly, with no instrumentation added to the game.
//     Nothing is written down: the denominator is one match and a counted number of refreshes.
//   - "CLEARED ON RESTART" IS READ OFF THE HUD'S OWN PROPERTY through
//     `StratTestHasMatchResultWidget`. `AStratScoreboardHUD::IsMatchResultWidgetOpen()` cannot
//     serve: it reads `IsInViewport()`, which is false in every state a headless `-nullrhi` run
//     can reach, so it would answer "not open" whether or not the restart cleared anything.
//   - THE MATCH IS PLAYED, NOT PLANTED. §2.9's AI drives both sides until the rules module
//     declares a §2.8 result -- `StratMatchConclusion.cpp`'s own fixture, for its stated reason.
//   - THE SIDES AND THE UNIT IDS come off the projection and the configured table; no side index
//     or unit id is typed here.
//
// THE INSTRUMENT IS CONTROLLED BEFORE ITS COUNT IS TRUSTED. The capture is shown able to record
// many `STRAT-` prefixed lines from the same game, and the refresh loop is required to have
// REACHED `ApplyView` -- see the block on that inside the clause, which is the half a naive count
// would get wrong.
//
// THE FILTER IS LOOSE AND ONLY THE ASSERTIONS ARE STRICT, on this project's standing rule and for
// the reason `StratMatchConclusion.cpp` records at length: `FOutputDeviceRedirector` and
// `FString::Contains` both have traps that make a strict filter hide the defect before the claim
// can see it. `CanBeUsedOnMultipleThreads()` returning true is not optional -- without it this
// capture fails 1 run in 4 on byte-identical code, measured on this project 2026-08-14.
//
// SLOT HYGIENE. `FStratMatchConfig::bRecordCompletionOnMatchEnd` is left at its C++ default and
// asserted false before anything is played, so no path here can create a save slot and
// `Saved/SaveGames/` is untouched by this file.
//
// IT NAMES NO `strat::` TYPE AND CALLS NO `strat::` FUNCTION.
//
// WHY StratPlay. `UStratMatchSubsystem` is this module's and `AStratScoreboardHUD` is includable
// from here (StratUI sits below). The reverse is not true: a StratUI clause could not name the
// subsystem at all, so this clause could not have been written in the module that owns the
// screen. The UMG-free seam it uses to place and read the widget is
// `Source/StratUI/Tests/StratMatchResultHostProbe.h`, which exists for exactly that boundary.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "CoreGlobals.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "HAL/CriticalSection.h"
#include "Logging/LogVerbosity.h"
#include "Misc/OutputDevice.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
#include "StratScoreboardHUD.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

#include "Tests/StratMatchResultHostProbe.h"

namespace StratMatchResultTrigger
{
	static const int32 kFirstSide = 0;

	/** `ConcludeMatchIfEnded`'s own sentence, and the only literal in this file. It is a LOG
	 *  FORMAT rather than a transcribed refusal reason: the reason itself is the HUD's and is
	 *  interpolated into it, and nothing below compares that half. Matching on the format's
	 *  fixed prefix is what makes the count a count of `ShowMatchResult` calls. */
	static const TCHAR* const kScreenRefusedPrefix = TEXT("No end-of-match screen this match");

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** See `StratMatchConclusion.cpp` for the same helper and the reason it is duplicated rather
	 *  than shared. `bRecordCompletionOnMatchEnd` is deliberately NOT set. */
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

	/** §4.8's unit ids, read off the table the match is configured with -- the row NAME is the
	 *  id. A hand-written list here would be this file deciding §4.8's id space. */
	static TArray<FName> UnitIdsFrom(const FStratMatchConfig& Config)
	{
		return Config.UnitTable != nullptr ? Config.UnitTable->GetRowNames() : TArray<FName>();
	}

	/** The sides the projection reports units for. Never a literal. */
	static TArray<int32> SidesFrom(const FStratViewModel& Model)
	{
		TArray<int32> Sides;
		for (const FStratUnitView& Unit : Model.Units)
		{
			Sides.AddUnique(Unit.Side);
		}
		Sides.Sort();
		return Sides;
	}

	/** See `StratMatchLifecycle.cpp` on why `InitializeActorsForPlay` is not decoration and why
	 *  the world is destroyed before its context. */
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

		FTestWorldScope(const FTestWorldScope&)            = delete;
		FTestWorldScope& operator=(const FTestWorldScope&) = delete;
	};

	/**
	 * A live `GLog` capture over this project's `STRAT-` lines AND `ConcludeMatchIfEnded`'s
	 * screen-refusal line.
	 *
	 * TWO FILTERS BECAUSE THE SCREEN LINE CARRIES NO `STRAT-` PREFIX. `ConcludeMatchIfEnded`'s
	 * warning is plain prose, deliberately -- `STRAT-` is reserved for the fixed-field lines a
	 * gate parses, and a refusal to draw an optional widget is not one of those. Filtering only
	 * on `STRAT-` would have made this device silent about its own subject.
	 *
	 * `CanBeUsedOnMultipleThreads()` RETURNS TRUE AND THAT LINE IS NOT OPTIONAL. Without it
	 * `FOutputDeviceRedirector` may hand this device lines emitted BEFORE it was constructed,
	 * because its buffered queue drains to whichever devices are registered at DRAIN time rather
	 * than at emission time. Measured on this project as a 1-in-4 failure of a clause on
	 * byte-identical code, 2026-08-14; this is the seventh capture in the suite to carry it.
	 *
	 * THE FILTER IS CASE-INSENSITIVE AND THE COUNTING IS CASE-SENSITIVE. A strict filter would
	 * drop a mis-cased emitter's line, and the clause counting occurrences would then report ZERO
	 * -- passing an "exactly one" assertion only by being blind to the defect.
	 */
	struct FTriggerLogCapture final : public FOutputDevice
	{
		TArray<FString> Lines;

		FTriggerLogCapture()
		{
			if (GLog != nullptr)
			{
				GLog->AddOutputDevice(this);
			}
		}

		virtual ~FTriggerLogCapture()
		{
			if (GLog != nullptr)
			{
				GLog->RemoveOutputDevice(this);
			}
		}

		/** See the block above. Removing this line reopens the late-delivery flake. */
		virtual bool CanBeUsedOnMultipleThreads() const override { return true; }

		virtual void Serialize(const TCHAR* Message, ELogVerbosity::Type /*Verbosity*/,
		                       const FName& /*Category*/) override
		{
			const FString Line(Message);
			// Loose on purpose -- see the block above.
			if (Line.StartsWith(TEXT("STRAT-")) ||
			    Line.Contains(kScreenRefusedPrefix, ESearchCase::IgnoreCase))
			{
				FScopeLock Lock(&Mutex);
				Lines.Add(Line);
			}
		}

		void Settle()
		{
			if (GLog != nullptr)
			{
				GLog->Flush();
			}
		}

		/** Case SENSITIVE. This is the claim, not the filter. */
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

		FString TextFrom(int32 FromIndex, const TCHAR* Needle) const
		{
			TArray<FString> Slice;
			for (int32 I = FMath::Max(0, FromIndex); I < Lines.Num(); ++I)
			{
				if (Lines[I].Contains(Needle, ESearchCase::CaseSensitive))
				{
					Slice.Add(Lines[I]);
				}
			}
			return Slice.Num() > 0 ? FString::Join(Slice, TEXT(" | ")) : FString(TEXT("<nothing>"));
		}

	private:
		FCriticalSection Mutex;
	};
}

// ---------------------------------------------------------------------------
// T-INT-05 -- §2.11.4's SCREEN IS ASKED FOR ONCE PER MATCH, AND A RESTART TAKES IT DOWN.
//
// See this file's header block for why these are ONE clause and not two, and for where each
// expectation is read from.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratMatchResultShownOnceAndClearedOnRestartTest,
	"Stratocracy.StratPlay.T-INT-05.MatchResultScreenIsAskedForOncePerMatchAndClearedOnRestart",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratMatchResultShownOnceAndClearedOnRestartTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchResultTrigger;

	// This fixture has no Blueprint defaults, so the board reports every terrain kind as
	// unmeshed and the HUD has no widget classes at all. Both are documented configurations
	// rather than faults -- `StratMatchReconcile.cpp` records the first, and
	// `AStratScoreboardHUD::MatchResultWidgetClass`'s own block the second -- and the SECOND is
	// this clause's instrument rather than noise: the refusal it produces is what makes
	// `ShowMatchResult` countable.
	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
	AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
	AddExpectedMessagePlain(kScreenRefusedPrefix, ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	// ---- A FINDABLE HUD, which is the premise of every count below -------------
	// `UStratMatchSubsystem::FindScoreboardHUD` looks for the FIRST LOCAL PLAYER'S HUD, so a HUD
	// merely spawned into the world is not findable and every assertion here would be counting
	// zero. `StratMatchLifecycle.cpp`'s handover clause wires it the same way and for the same
	// reason.
	APlayerController* const Controller = Scope.World->SpawnActor<APlayerController>();
	if (!TestNotNull(TEXT("a player controller spawned"), Controller))
	{
		return false;
	}

	AStratScoreboardHUD* const Hud = Scope.World->SpawnActor<AStratScoreboardHUD>();
	if (!TestNotNull(TEXT("a scoreboard HUD spawned"), Hud))
	{
		return false;
	}
	Controller->MyHUD = Hud;

	const APlayerController* const FirstLocal = Scope.World->GetFirstPlayerController();
	if (!TestNotNull(TEXT("the world reports a first local player controller"), FirstLocal))
	{
		return false;
	}
	if (!TestSamePtr(
			TEXT("and its HUD is the one spawned above, so FindScoreboardHUD will reach it -- "
			     "without this every count below would be a count of zero"),
			FirstLocal->GetHUD(), static_cast<AHUD*>(Hud)))
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
	if (!TestFalse(TEXT("the completion writer's opt-in is at its C++ default (off), so this "
	                    "clause cannot create a save slot"),
			Base.bRecordCompletionOnMatchEnd))
	{
		return false;
	}

	// ---- The sides, read off the projection ------------------------------------
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
		Sides = SidesFrom(Model);
	}

	if (!TestTrue(*FString::Printf(
				TEXT("the seeded scenario deploys units for more than one side (it deploys for %d)"),
				Sides.Num()),
			Sides.Num() >= 2))
	{
		return false;
	}

	FStratMatchConfig BothAi = Base;
	BothAi.AiSides            = Sides;
	BothAi.AiBuildlistUnitIds = UnitIdsFrom(BothAi);

	FTriggerLogCapture Capture;

	FString StartReason;
	Subsystem->StartMatch(BothAi, StartReason);
	if (!TestTrue(TEXT("the AI-vs-AI match is live"), Subsystem->IsMatchLive()))
	{
		AddError(StartReason);
		return false;
	}

	const int32 LinesBefore = Capture.Lines.Num();

	// Taken BEFORE the game, so a line left over from an earlier clause in the same process
	// cannot be counted as this match's.
	TestEqual(TEXT("no end-of-match screen has been asked for before this game is played"),
		Capture.CountFrom(LinesBefore, kScreenRefusedPrefix), 0);

	// ---- Play to a §2.8 result -------------------------------------------------
	FString    RunReason;
	const bool bRan = Subsystem->RunAiTurnsNow(RunReason);
	Capture.Settle();

	FStratViewModel After;
	if (!TestTrue(TEXT("the view model builds after the game"),
			Subsystem->BuildViewModel(After, Error)))
	{
		AddError(Error);
		return false;
	}

	if (!TestTrue(TEXT("§2.8: the game reached a result -- the premise for counting the screen "
	                   "trigger at all"),
			After.Match.bHasResult))
	{
		AddError(FString::Printf(TEXT("RunAiTurnsNow returned %s: '%s'"),
			bRan ? TEXT("true") : TEXT("false"), *RunReason));
		return false;
	}

	// ---- THE INSTRUMENT'S CONTROL ---------------------------------------------
	// It recorded many lines from this same game, so a low count of the screen line below is a
	// fact about the emitter and not about a device that cannot hear.
	const int32 AiLines = Capture.CountFrom(LinesBefore, TEXT("STRAT-AI"));
	TestTrue(*FString::Printf(
			TEXT("the capture recorded many lines during this game (%d), so its count of the "
			     "screen line is not a blindness"), AiLines),
		AiLines > 1);

	// ---- HALF ONE, FIRST READING: the screen was asked for, exactly once -------
	const int32 AfterTheGame = Capture.CountFrom(LinesBefore, kScreenRefusedPrefix);
	AddInfo(FString::Printf(TEXT("%d screen-trigger line(s) at the end of the game: %s"),
		AfterTheGame, *Capture.TextFrom(LinesBefore, kScreenRefusedPrefix)));

	if (!TestEqual(
			TEXT("§2.11.4's screen was asked for exactly once when the match concluded -- a zero "
			     "means ConcludeMatchIfEnded never reached ShowMatchResult at all, which is the "
			     "state this whole surface shipped in before this pass"),
			AfterTheGame, 1))
	{
		return false;
	}

	// ---- HALF ONE, SECOND READING: and not again, however often we refresh -----
	//
	// EACH REFRESH IS REQUIRED TO HAVE REACHED `ApplyView`, WHICH IS THE HALF A NAIVE COUNT GETS
	// WRONG. `RefreshPresentation` calls `ApplyView` FIRST and asks the HUD to refresh the
	// scoreboard AFTER; with no `ScoreboardWidgetClass` on this HUD that second step refuses, so
	// the function returns FALSE on every call here even though `ConcludeMatchIfEnded` ran every
	// time. Asserting on the return value would have made this loop look like five refusals and
	// "still exactly one" would then be satisfied by five refreshes that never happened. The
	// reason string is what tells the two apart: a refusal naming the SCOREBOARD is a refusal
	// that happened downstream of `ApplyView`.
	const int32 kRefreshes = 5;
	int32       Reached    = 0;
	for (int32 I = 0; I < kRefreshes; ++I)
	{
		FString RefreshReason;
		const bool bOk = Subsystem->RefreshPresentation(RefreshReason);
		if (bOk || RefreshReason.Contains(TEXT("scoreboard"), ESearchCase::IgnoreCase))
		{
			++Reached;
		}
		else
		{
			AddInfo(FString::Printf(
				TEXT("refresh %d refused BEFORE the scoreboard step: '%s'"), I, *RefreshReason));
		}
	}
	Capture.Settle();

	if (!TestEqual(
			TEXT("every post-conclusion refresh got past ApplyView -- either it succeeded, or it "
			     "refused at the scoreboard step, which ApplyView runs before. Without this, "
			     "'still exactly one' below is satisfied by refreshes that never ran."),
			Reached, kRefreshes))
	{
		return false;
	}

	const int32 AfterRefreshes = Capture.CountFrom(LinesBefore, kScreenRefusedPrefix);
	AddInfo(FString::Printf(TEXT("%d screen-trigger line(s) after %d further refreshes"),
		AfterRefreshes, kRefreshes));

	TestEqual(
		TEXT("§2.11.4's screen is still asked for exactly once after five more presentation "
		     "refreshes of the same finished match -- the trigger is the EDGE and not the state"),
		AfterRefreshes, 1);

	// ---- HALF TWO: a restart takes the prior match's verdict down --------------
	//
	// THE SCREEN IS PLACED BY THE PROBE RATHER THAN BY `ShowMatchResult`, and the reason is
	// stated rather than hidden: `ShowMatchResult` cannot succeed headlessly at all --
	// `MatchResultWidgetClass` is unset on a HUD with no Blueprint defaults, and even set,
	// `CreateWidget` refuses a controller that is not a LOCAL player controller. So the only way
	// to reach the state a restart must clear is to write the same member `CreateMatchResultWidget`
	// writes. The probe adds no capability and records nothing; the assertion below reads
	// `AStratScoreboardHUD::MatchResultScreen` itself.
	//
	// THE POSITIVE CONTROL IS TWO READS, NOT ONE. Without the "there is nothing up yet" read and
	// the "now there is" read, the final assertion is green on a probe that silently failed to
	// install anything -- which is a gate that has stopped covering its subject.
	if (!TestFalse(TEXT("nothing is on MatchResultScreen before the probe installs one"),
			StratTestHasMatchResultWidget(Hud)))
	{
		return false;
	}
	if (!TestTrue(TEXT("the concrete result-screen double installed on the HUD's own member"),
			StratTestInstallMatchResultDouble(Hud)))
	{
		return false;
	}
	if (!TestTrue(TEXT("the HUD is now holding an end-of-match screen, which is the state a "
	                   "restart has to clear"),
			StratTestHasMatchResultWidget(Hud)))
	{
		return false;
	}

	FString RestartReason;
	Subsystem->StartMatch(BothAi, RestartReason);
	if (!TestTrue(TEXT("the restarted match is live"), Subsystem->IsMatchLive()))
	{
		AddError(RestartReason);
		return false;
	}

	TestFalse(
		TEXT("§2.11.4: starting the next match took the previous match's verdict screen down -- "
		     "clearing `bMatchConclusionAnnounced` removes nothing from the viewport, and the "
		     "widget outlives TearDownPresentation because it belongs to the HUD"),
		StratTestHasMatchResultWidget(Hud));

	// AND THE RESTART DID NOT ITSELF ASK FOR A NEW SCREEN. A fresh match has no §2.8 result, so
	// `ConcludeMatchIfEnded` must not have fired -- which is the other direction of the latch
	// `StartMatchInternal` clears.
	Capture.Settle();
	TestEqual(
		TEXT("and starting a fresh match did not ask for a verdict screen of its own -- there is "
		     "no result to show yet"),
		Capture.CountFrom(LinesBefore, kScreenRefusedPrefix), 1);

	return true;
}
