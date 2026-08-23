// GDD §2.8 / §2.9 / §4.1 -- the end-of-match transition. T-INT-05 and T-AI-01.
//
// WHAT GAP THESE CLAUSES CLOSE, AND IT IS THE MOST SERIOUS ONE THIS PROJECT HAS SHIPPED.
// Measured 2026-08-23 in a human-driven PIE session and recorded in
// `Tools/architect/state/global.md`: on the AI's turn 7 its ninth applied command killed
// side 0's flag, the rules module correctly declared the match over, and NOTHING LEFT PLAY.
// The AI's own closing `EndTurn` was refused by the rules module with
// `[T-SAVE-05] no match is running`, so the turn stayed 7 and the side to move stayed 1 --
// and the human went on issuing `Move` and `Attack` commands that the rules module ACCEPTED,
// as the AI's side. The engine detected the result, persisted the completion three
// milliseconds later, and acted on neither.
//
// The reason was not a transition that misfired. There was no transition. A scan of
// `Source/StratPlay` and `Source/StratUI` outside `Tests/` returned not one end-of-match
// path, and `IsMatchRunning` did not exist anywhere in `Source/`. The delegation
// `IStratAiTurnPort`'s header describes -- "whether this side should be playing at all is
// decided before `RunTurn` is called, by the subsystem that has the view model" -- was
// correct and had no receiver.
//
// It has one now, and these five clauses are what stop it being deleted by accident.
//
// THE FIVE PROPERTIES.
//   1. THE GATE ANSWERS BOTH WAYS, and its refusal is `StratMatchConcludedRefusalText()`
//      rather than a second spelling of the same sentence.
//   2. CONCLUSION IS `bHasResult` AND IS NEVER INFERRED FROM `ResultTier`, in both
//      directions, over every tier the enum declares.
//   3. A MATCH WON MID-TURN IS NOT AN AI FAULT. `RunAiTurnsNow` returns TRUE with an EMPTY
//      reason on the turn the result lands. This is the clause that would have caught the
//      defect, and it plants nothing: a real AI-vs-AI game reaches §2.8 under its own steam.
//   4. A GENUINE AI REFUSAL IS STILL A FAULT -- the negative control for 3, without which 3
//      is green on a `RunAiTurnsNow` that reports nothing at all, ever.
//   5. THE TRANSITION LOGS ONCE AND ONLY ONCE, however many times presentation refreshes
//      afterwards. Before this clause the one-shot latch was CORROBORATED by a magnitude
//      argument over a PIE log -- six `STRAT-MATCH concluded` lines against 967
//      `STRAT-AI applied` -- and nothing had ever counted how many matches concluded. This
//      clause is the count.
//
// WHERE THE EXPECTATIONS COME FROM. Clause 1's refusal string is
// `StratMatchConcludedRefusalText()` itself, never transcribed. Clause 2's tier list is
// `StaticEnum<EStratResultTier>()`'s own enumerator walk and its null is
// `FStratMatchView().ResultTier`, so a fifth tier is REQUIRED to be handled rather than
// silently unmentioned. Clauses 3-5 plant no result at all: `FStratMatchView::bHasResult` is
// the rules module's answer, reached by playing. The AI sides are the distinct sides the view
// model reports units for. There is no hex, unit id, turn number or tier name typed in this
// file, and no refusal sentence is transcribed anywhere in it.
//
// EVERY STRING COMPARISON IS CASE SENSITIVE. `FString::operator==`, `Contains` and
// `TestEqual` all ignore case in UE 5.8, so an ordinary comparison of two byte-identical
// strings is a clause that cannot fail. The log CAPTURE's filter is deliberately LOOSE
// (case-insensitive) and only the ASSERTIONS are strict -- a strict filter would let a
// casing defect vanish before the claim could see it.
//
// SLOT HYGIENE. No clause here opts in to §2.11.6's completion writer:
// `FStratMatchConfig::bRecordCompletionOnMatchEnd` is left at its C++ default and the
// clauses that reach a §2.8 result ASSERT that it is false before playing, read off
// `FStratMatchConfig()` rather than written down. So no path here can create a save slot,
// and `Saved/SaveGames/` is untouched by this file.
//
// IT NAMES NO `strat::` TYPE AND CALLS NO `strat::` FUNCTION.
//
// WHY `StratPlay` AND NOT ELSEWHERE. `StratMatchIsConcluded`,
// `StratMatchAcceptsPlayerCommands` and `StratMatchConcludedRefusalText` are `STRATPLAY_API`
// free functions and `UStratMatchSubsystem` is this module's; `FStratBridge` is reachable
// from here privately. In any other module these are `LNK2019`, not tests.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "CoreGlobals.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "HAL/CriticalSection.h"
#include "Misc/OutputDevice.h"
#include "Misc/OutputDeviceRedirector.h"
#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

namespace StratMatchConclusion
{
	static const int32 kFirstSide = 0;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** See `StratAiMatchClauses.cpp` for the same helper and the reason it is duplicated.
	 *  `bRecordCompletionOnMatchEnd` is deliberately NOT set -- see the header block. */
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

	/** §4.8's unit ids, read off the table the match is configured with -- the row NAME is
	 *  the id. A hand-written list here would be this file deciding §4.8's id space. */
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

	/**
	 * A live `GLog` capture over this project's `STRAT-` prefixed lines.
	 *
	 * `CanBeUsedOnMultipleThreads()` RETURNS TRUE AND THAT LINE IS NOT OPTIONAL. Without it
	 * `FOutputDeviceRedirector` may hand this device lines emitted BEFORE it was constructed,
	 * because its buffered queue drains to whichever devices are registered at DRAIN time
	 * rather than at emission time. Measured on this project as a 1-in-4 failure of a clause
	 * on byte-identical code, 2026-08-14. `.agents/ue-project-context.md` records it as a
	 * standing obligation for every capture in this suite; this is the sixth.
	 *
	 * THE FILTER IS LOOSE AND ONLY THE ASSERTIONS ARE STRICT. `StartsWith` here defaults to
	 * IgnoreCase deliberately: a capture that filtered case-sensitively would drop a
	 * `strat-match` line, and the clause counting `STRAT-MATCH concluded` occurrences would
	 * then report ZERO -- passing a "exactly one" assertion only by being blind to the
	 * defect. The counting below is `ESearchCase::CaseSensitive`, which is where the claim is.
	 *
	 * THE LOCK COVERS THE APPEND AND NOTHING ELSE. Every read below is unlocked and
	 * game-thread-only, which is safe solely because every `STRAT-*` emitter is on the game
	 * thread. Nothing in the code pins that; see `StratAiMatchClauses.cpp`'s longer note.
	 */
	struct FStratLogCapture final : public FOutputDevice
	{
		TArray<FString> Lines;

		FStratLogCapture()
		{
			if (GLog != nullptr)
			{
				GLog->AddOutputDevice(this);
			}
		}

		virtual ~FStratLogCapture()
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
			if (Line.StartsWith(TEXT("STRAT-")))
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
}

// ---------------------------------------------------------------------------
// T-INT-05 -- A CONCLUDED MODEL REFUSES PLAYER COMMANDS, AND A LIVE ONE DOES NOT.
//
// THE POSITIVE CONTROL IS IN THIS CLAUSE AND NOT IN ANOTHER ONE, and that placement is the
// whole point: "a concluded model is refused" is satisfied for free by a predicate that
// returns false unconditionally, by one that was deleted and replaced with `return false`,
// and by one whose caller never runs. So the SAME predicate, in the SAME clause, over a model
// differing in EXACTLY ONE FIELD, is required to answer TRUE.
//
// THE REFUSAL IS COMPARED AGAINST `StratMatchConcludedRefusalText()` AND IS NOT TRANSCRIBED.
// The declaration's stated reason for that function existing is that there are three sites
// and a second spelling is a second thing to keep in step; a clause that typed the sentence
// out would BE the fourth site, and would go green on a drift it was written to catch.
// Comparison is `ESearchCase::CaseSensitive` -- `TestEqual` on two `FString`s is not.
//
// THE LIVE MODEL'S OUT-PARAMETER IS ASSERTED EMPTY, which is the declared contract ("reset on
// entry; written only when the answer is false") and is what makes a caller free to report
// the string without first checking the bool.
//
// NO WORLD, NO BRIDGE, NO PIE. The model is planted, which is the shape the free function was
// deliberately given -- see the block above its declaration.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratConcludedModelRefusesPlayerCommandsTest,
	"Stratocracy.StratPlay.T-INT-05.AConcludedModelRefusesPlayerCommands",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratConcludedModelRefusesPlayerCommandsTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchConclusion;

	// ---- The positive control, first, and over the same predicate ---------------
	FStratViewModel Live;
	Live.Match.bHasResult = false;

	FString     LiveReason = TEXT("a value this call must overwrite");
	const bool  bLiveOk    = StratMatchAcceptsPlayerCommands(Live, LiveReason);

	TestTrue(TEXT("a model with no result ACCEPTS player commands -- the control, without "
	              "which 'refuses' is unfalsifiable"), bLiveOk);
	TestTrue(TEXT("the out-parameter is reset on entry and left empty on the accepting path"),
		LiveReason.IsEmpty());

	TestFalse(TEXT("and the predicate agrees that model is not concluded"),
		StratMatchIsConcluded(Live));

	// ---- The subject: one field different ---------------------------------------
	FStratViewModel Concluded = Live;
	Concluded.Match.bHasResult = true;

	FString    ConcludedReason;
	const bool bConcludedOk = StratMatchAcceptsPlayerCommands(Concluded, ConcludedReason);

	TestFalse(TEXT("a model that has reached a §2.8 result REFUSES player commands"),
		bConcludedOk);
	TestTrue(TEXT("and the predicate agrees that model is concluded"),
		StratMatchIsConcluded(Concluded));

	// ---- The sentence, read off the module and never written down ----------------
	const FString Expected = StratMatchConcludedRefusalText();

	TestTrue(TEXT("the module's own refusal sentence is not empty -- without this the "
	              "comparison below could be satisfied by two empty strings"),
		!Expected.IsEmpty());

	TestTrue(*FString::Printf(
			TEXT("the refusal handed back is StratMatchConcludedRefusalText() BYTE FOR BYTE, "
			     "not a second spelling: got '%s', expected '%s'"),
			*ConcludedReason, *Expected),
		ConcludedReason.Equals(Expected, ESearchCase::CaseSensitive));

	return true;
}

// ---------------------------------------------------------------------------
// T-INT-05 -- CONCLUSION IS READ FROM `bHasResult` AND IS NEVER INFERRED FROM `ResultTier`.
//
// WHY THIS IS A CLAUSE AND NOT A COMMENT. `FStratMatchView` says on the field itself that the
// two are read separately "so the two cannot disagree", and `StratMatchIsConcluded`'s
// declaration says a predicate asking `ResultTier != InProgress` instead "would be a second
// author of the same fact". Both sentences are prose until something asks. A rewrite to
// `Model.Match.ResultTier != EStratResultTier::InProgress` compiles, reads better to a
// stranger, and passes every other clause in this suite.
//
// THE TIER LIST IS A REFLECTION WALK AND NOT A TYPED LIST. Every enumerator
// `EStratResultTier` declares other than the null is driven through both directions. A fifth
// tier added to that enum is therefore REQUIRED to be handled rather than silently
// unmentioned -- the going-inert failure a typed list guarantees.
//
// THE NULL VALUE IS `FStratMatchView().ResultTier`, the module's own default, rather than the
// enumerator named in source. A clause that named `InProgress` would be a statement about a
// symbol; this one is a statement about the value the projection actually starts at.
//
// IT PLANTS AN IMPOSSIBLE PAIR ON PURPOSE. `StratBuildViewModel` never produces
// `bHasResult == false` beside a decided tier -- that is exactly why the pairing isolates
// which field is being read, and why no rules-module value exists to compare against here.
// The subject IS the independence of the two fields.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratConclusionIsReadFromHasResultTest,
	"Stratocracy.StratPlay.T-INT-05.ConclusionIsReadFromHasResultAndNotFromResultTier",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratConclusionIsReadFromHasResultTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchConclusion;

	UEnum* const TierEnum = StaticEnum<EStratResultTier>();
	if (!TestNotNull(TEXT("EStratResultTier is reflected and its enumerator list is readable"),
			TierEnum))
	{
		return false;
	}

	// The null, read off the module rather than named.
	const EStratResultTier NullTier = FStratMatchView().ResultTier;

	int32 DecidedTiersSeen = 0;

	// `NumEnums()` counts the compiler-generated `_MAX` sentinel; the walk skips it by name.
	for (int32 I = 0; I < TierEnum->NumEnums(); ++I)
	{
		const FString EnumeratorName = TierEnum->GetNameStringByIndex(I);
		if (EnumeratorName.IsEmpty() || EnumeratorName.EndsWith(TEXT("_MAX")))
		{
			continue;
		}

		const EStratResultTier Tier =
			static_cast<EStratResultTier>(TierEnum->GetValueByIndex(I));

		if (Tier == NullTier)
		{
			continue;
		}

		++DecidedTiersSeen;

		// ---- Direction A: a decided tier with NO result is NOT concluded ---------
		FStratViewModel TierWithoutResult;
		TierWithoutResult.Match.bHasResult = false;
		TierWithoutResult.Match.ResultTier = Tier;

		TestFalse(*FString::Printf(
				TEXT("tier '%s' beside bHasResult=false is NOT concluded -- conclusion is not "
				     "inferred from the tier"),
				*EnumeratorName),
			StratMatchIsConcluded(TierWithoutResult));

		FString TierReason;
		TestTrue(*FString::Printf(
				TEXT("and player commands are still accepted with tier '%s' and no result"),
				*EnumeratorName),
			StratMatchAcceptsPlayerCommands(TierWithoutResult, TierReason));
	}

	// A walk that found no decided tier would have asserted nothing at all and would still be
	// green. This is the coverage assertion that stops that.
	TestTrue(*FString::Printf(
			TEXT("EStratResultTier declares at least one decided tier for the walk above to "
			     "drive (it drove %d)"),
			DecidedTiersSeen),
		DecidedTiersSeen > 0);

	// ---- Direction B: a result at the NULL tier IS concluded ---------------------
	FStratViewModel ResultAtNullTier;
	ResultAtNullTier.Match.bHasResult = true;
	ResultAtNullTier.Match.ResultTier = NullTier;

	TestTrue(*FString::Printf(
			TEXT("bHasResult=true at the null tier ('%s') IS concluded -- the tier is not "
			     "consulted in this direction either"),
			*TierEnum->GetNameStringByValue(static_cast<int64>(NullTier))),
		StratMatchIsConcluded(ResultAtNullTier));

	FString NullTierReason;
	TestFalse(TEXT("and player commands are refused there"),
		StratMatchAcceptsPlayerCommands(ResultAtNullTier, NullTierReason));

	return true;
}

// ---------------------------------------------------------------------------
// T-AI-01 -- A MATCH WON MID-TURN IS NOT REPORTED AS AN AI FAULT.
//
// THIS IS THE CLAUSE THAT WOULD HAVE CAUGHT THE DEFECT. §2.8's primary win is a flag kill,
// and the AI's OWN command is what lands it; the rules module then refuses that same turn's
// closing `EndTurn` with `[T-SAVE-05] no match is running`. Before the fix,
// `RunAiTurnsNow` propagated that refusal as a failure reason and returned FALSE for a game
// that had finished perfectly -- and `T-INT-05.BothSidesAiReachesAResultWithinTheBound`
// recorded the return value with `AddInfo` and deliberately did not assert it, precisely
// because asserting it either way would have been wrong at the time. That clause's silence is
// what this one ends.
//
// IT PLANTS NOTHING. `bHasResult` is the rules module's own answer and it is reached by
// PLAYING: both sides AI, one synchronous call, a real §2.8 result. A clause that planted a
// result and then asked `RunAiTurnsNow` about it would be asking a different question --
// whether the loop's PRE-turn exit fires -- and would never reach the post-refusal
// reclassification arm at all.
//
// THREE FACTS TOGETHER, because none of them is sufficient alone. (a) The match reached a
// result. (b) `RunAiTurnsNow` returned TRUE. (c) Its `OutFailureReason` is EMPTY. Without (c)
// a fix that returned true while still filling in the reason string would pass, and the
// caller that logs the reason would still say the AI broke. Without (a) the whole clause is
// about a game that never finished.
//
// AND A FOURTH: THE REFUSAL ACTUALLY HAPPENED. `STRAT-AI refused` is required to appear, so
// this clause cannot go green over a run in which the match ended tidily at a turn boundary
// and the reclassification arm was never entered. That is a real reachable state on other
// content and it would make (a)-(c) true for the wrong reason.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratMatchWonMidTurnIsNotAnAiFaultTest,
	"Stratocracy.StratPlay.T-AI-01.AMatchWonMidTurnIsNotReportedAsAnAiFault",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratMatchWonMidTurnIsNotAnAiFaultTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchConclusion;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	// THE RUNNER STILL LOGS ITS REFUSAL AND MUST. The fix is a RECLASSIFICATION by
	// `RunAiTurnsNow`, one layer above `FStratAiTurnRunner`, which correctly reports the
	// rules module's refusal of its EndTurn and knows nothing about why. `Occurrences = 0`
	// means ONE OR MORE, so this expectation is also the tripwire for a later change that
	// stops the runner logging at all.
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

	// SLOT HYGIENE, ASSERTED RATHER THAN ASSUMED, and the value is read off the module.
	// This clause plays to a §2.8 result, which is the one condition §2.11.6's completion
	// writer fires on; with the opt-in off it cannot write, so no slot is created.
	if (!TestFalse(TEXT("the completion writer's opt-in is at its C++ default (off), so this "
	                    "clause cannot create a save slot"),
			Base.bRecordCompletionOnMatchEnd))
	{
		return false;
	}
	TestFalse(TEXT("and FStratMatchConfig()'s own default agrees"),
		FStratMatchConfig().bRecordCompletionOnMatchEnd);

	// ---- Which sides exist, asked of the projection ------------------------------
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

	// ---- Both sides AI, one synchronous call --------------------------------------
	FStratMatchConfig BothAi = Base;
	BothAi.AiSides            = Sides;
	BothAi.AiBuildlistUnitIds = UnitIdsFrom(BothAi);

	FStratLogCapture Capture;

	FString StartReason;
	Subsystem->StartMatch(BothAi, StartReason);
	if (!TestTrue(TEXT("the AI-vs-AI match is live"), Subsystem->IsMatchLive()))
	{
		AddError(StartReason);
		return false;
	}

	const int32 LinesBefore = Capture.Lines.Num();

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

	const int32 Applied  = Capture.CountFrom(LinesBefore, TEXT("STRAT-AI applied"));
	const int32 Refusals = Capture.CountFrom(LinesBefore, TEXT("STRAT-AI refused"));
	AddInfo(FString::Printf(
		TEXT("%d applied commands, %d STRAT-AI refused line(s); RunAiTurnsNow returned %s "
		     "with reason '%s'"),
		Applied, Refusals, bRan ? TEXT("true") : TEXT("false"), *RunReason));

	// (a) The game actually finished, under its own steam.
	if (!TestTrue(TEXT("§2.8: the AI-vs-AI game reached a result -- without this the three "
	                   "assertions below are about a game that never ended"),
			After.Match.bHasResult))
	{
		return false;
	}

	// The capture is required to have seen the game, so the counts below are not readings a
	// blind instrument could satisfy.
	TestTrue(TEXT("the capture saw the AI's commands"), Applied > 0);

	// (d) The mid-turn win path is the one that was taken.
	TestTrue(*FString::Printf(
			TEXT("the match ended MID-TURN, so the rules module refused the winning side's "
			     "closing EndTurn and the reclassification arm was entered (%d refusal line(s): %s)"),
			Refusals, *Capture.TextFrom(LinesBefore, TEXT("STRAT-AI refused"))),
		Refusals > 0);

	// (b) and (c) -- the property this clause exists for.
	TestTrue(*FString::Printf(
			TEXT("finishing is NOT a fault: RunAiTurnsNow returned true (it returned %s, "
			     "reason '%s')"),
			bRan ? TEXT("true") : TEXT("false"), *RunReason),
		bRan);

	TestTrue(*FString::Printf(
			TEXT("and it reported no failure reason at all (it reported '%s')"), *RunReason),
		RunReason.IsEmpty());

	// The transition itself ran off the same model the screen was drawn from.
	TestTrue(TEXT("and the subsystem's own convenience form agrees the match is concluded"),
		Subsystem->IsMatchConcluded());
	TestTrue(TEXT("and GetConcludedMatchView() -- what was APPLIED, not a fresh query -- "
	              "carries the result"),
		Subsystem->GetConcludedMatchView().bHasResult);

	return true;
}

// ---------------------------------------------------------------------------
// T-AI-01 -- A GENUINE AI REFUSAL IS STILL A FAULT. The negative control for the clause above.
//
// WITHOUT THIS CLAUSE THE ONE ABOVE IS GREEN ON A `RunAiTurnsNow` THAT NEVER REPORTS
// ANYTHING. "Returned true with an empty reason" is satisfied for free by a function whose
// failure arms were deleted, by one that swallows every `Outcome.FailureReason`, and by one
// that returns true unconditionally. So the same method, on the same fixture, is required to
// return FALSE with a NON-EMPTY reason for a refusal that is NOT the match ending.
//
// THE REFUSAL IS MANUFACTURED THROUGH A PRODUCTION SEAM AND NOT A SCRIPTED PORT.
// `RunAiTurnsNow` constructs its own `FStratBridgeAiTurnPort` and takes no port argument, so
// there is no injection point -- and adding one would be a change outside `Tests/`, which is
// not this lane's to make. `FStratMatchConfig::AiMaxCommandsPerTurn` is the seam that already
// exists: set to one, the runner applies a single command, never reaches its `EndTurn`, and
// reports its own bound as a fault with `STRAT-AI refused phase=bound`. The match is one
// command old, so it is emphatically still running.
//
// AND THAT IS EXACTLY THE DISCRIMINATION UNDER TEST. `RunAiTurnsNow`'s new arm rebuilds the
// model after a refused turn and asks `StratMatchIsConcluded`. Here the answer is NO, so the
// refusal must travel up unchanged. A fix that reclassified EVERY refusal as an ordinary end
// -- the obvious over-broad version of the same change, and the one a hurried reader would
// write -- passes the clause above and fails this one.
//
// THE REASON IS COMPARED FOR NON-EMPTINESS AND NOT FOR CONTENT. The bound's sentence is
// `StratAiTurnRunner.cpp`'s and is pinned there by
// `T-AI-01.LoopBoundIsAReportedFault`; transcribing it here would make this file a second
// author of it. What is asserted is that the reason SURVIVED, which is the property the
// reclassification threatens.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGenuineAiRefusalIsStillAFaultTest,
	"Stratocracy.StratPlay.T-AI-01.AGenuineAiRefusalIsStillAFault",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGenuineAiRefusalIsStillAFaultTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchConclusion;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
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

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	Config.AiSides = { kFirstSide };
	Config.AiBuildlistUnitIds = UnitIdsFrom(Config);

	// ONE COMMAND PER TURN. The runner applies it, never reaches EndTurn, and reports its own
	// bound. See the block above on why this is the seam and not a scripted port.
	Config.AiMaxCommandsPerTurn = 1;

	FStratLogCapture Capture;

	FString StartReason;
	Subsystem->StartMatch(Config, StartReason);
	if (!TestTrue(TEXT("the match is live"), Subsystem->IsMatchLive()))
	{
		AddError(StartReason);
		return false;
	}

	if (!TestTrue(*FString::Printf(TEXT("side %d is configured as AI and moves first"), kFirstSide),
			Subsystem->IsSideAi(kFirstSide)))
	{
		return false;
	}

	const int32 LinesBefore = Capture.Lines.Num();

	FString    RunReason;
	const bool bRan = Subsystem->RunAiTurnsNow(RunReason);
	Capture.Settle();

	FStratViewModel After;
	if (!TestTrue(TEXT("the view model builds after the refused turn"),
			Subsystem->BuildViewModel(After, Error)))
	{
		AddError(Error);
		return false;
	}

	AddInfo(FString::Printf(TEXT("RunAiTurnsNow returned %s; reason: '%s'; refusal lines: %s"),
		bRan ? TEXT("true") : TEXT("false"), *RunReason,
		*Capture.TextFrom(LinesBefore, TEXT("STRAT-AI refused"))));

	// THE PREMISE, AND IT IS FATAL. If the single applied command had somehow ended the
	// match, the reclassification arm would be CORRECT to fire and this clause would be
	// asserting the opposite of the truth.
	if (!TestFalse(TEXT("the match is still running after the refused turn -- the premise this "
	                    "whole clause rests on"),
			After.Match.bHasResult))
	{
		return false;
	}
	TestFalse(TEXT("and the subsystem's convenience form agrees it is not concluded"),
		Subsystem->IsMatchConcluded());

	// The instrument is shown able to speak before anything is read from it.
	TestTrue(TEXT("the runner logged its refusal"),
		Capture.CountFrom(LinesBefore, TEXT("STRAT-AI refused")) > 0);

	// The property.
	TestFalse(*FString::Printf(
			TEXT("a refusal on a STILL RUNNING match is reported as a fault: RunAiTurnsNow "
			     "returned %s"),
			bRan ? TEXT("true") : TEXT("false")),
		bRan);

	TestFalse(*FString::Printf(
			TEXT("and the AI's own reason survived to the caller rather than being swallowed "
			     "by the end-of-match reclassification (reason: '%s')"),
			*RunReason),
		RunReason.IsEmpty());

	return true;
}

// ---------------------------------------------------------------------------
// T-INT-05 -- CONCLUDING LOGS ONCE AND ONLY ONCE.
//
// WHAT THIS CONVERTS FROM AN ARGUMENT INTO A COUNT. `bMatchConclusionAnnounced` is a one-shot
// latch and `ApplyView` runs on EVERY presentation refresh, so an unlatched transition would
// emit one `STRAT-MATCH concluded` line per refresh for the rest of the process -- and would
// clear the pacing timer, re-run its side effects, and bury the one line that matters in a
// stream of duplicates. Up to this clause the latch was supported only by a magnitude
// argument over a PIE log: six `STRAT-MATCH concluded` lines against 73 `STRAT-AI turn-ended`
// and 967 `STRAT-AI applied`. Nothing had ever enumerated how many matches actually
// concluded, so the ratio was suggestive and not a measurement. Here the denominator is
// known: ONE match, and a counted number of refreshes after it.
//
// THE REFRESHES ARE REQUIRED TO HAVE SUCCEEDED, which is what stops "exactly one" being
// satisfied by refreshes that never reached `ApplyView`. `RefreshPresentation` returns true
// in this fixture because `FindScoreboardHUD()` finds nothing and the scoreboard step -- the
// only refusable one -- is skipped; that return is therefore a real signal here rather than
// the always-false it is in the guidance-route fixture.
//
// THE INSTRUMENT IS CONTROLLED BEFORE ITS SILENCE IS TRUSTED, twice over. It is shown able to
// record MANY lines carrying a `STRAT-` prefix (the AI's own, during the same game), so
// "it only saw one" is not a statement about a blind device. And "exactly one" is a
// two-sided assertion -- a zero fails it as loudly as a five -- so it needs no separate
// existence check.
//
// THE FILTER IS CASE-INSENSITIVE AND THE COUNT IS CASE-SENSITIVE. See `FStratLogCapture`.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratConcludingLogsOnceAndOnlyOnceTest,
	"Stratocracy.StratPlay.T-INT-05.ConcludingLogsOnceAndOnlyOnce",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratConcludingLogsOnceAndOnlyOnceTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchConclusion;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
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

	if (!TestFalse(TEXT("the completion writer's opt-in is at its C++ default (off), so this "
	                    "clause cannot create a save slot"),
			Base.bRecordCompletionOnMatchEnd))
	{
		return false;
	}

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

	FStratLogCapture Capture;

	FString StartReason;
	Subsystem->StartMatch(BothAi, StartReason);
	if (!TestTrue(TEXT("the AI-vs-AI match is live"), Subsystem->IsMatchLive()))
	{
		AddError(StartReason);
		return false;
	}

	const int32 LinesBefore = Capture.Lines.Num();

	// NOTHING HAS CONCLUDED YET, and this reading is taken before the game so that a
	// `STRAT-MATCH` line left over from a previous clause in the same process cannot be
	// counted as this match's.
	TestEqual(TEXT("no match has concluded before this game is played"),
		Capture.CountFrom(LinesBefore, TEXT("STRAT-MATCH concluded")), 0);

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

	if (!TestTrue(TEXT("§2.8: the game reached a result -- the premise for counting the "
	                   "conclusion line at all"),
			After.Match.bHasResult))
	{
		AddError(FString::Printf(TEXT("RunAiTurnsNow returned %s: '%s'"),
			bRan ? TEXT("true") : TEXT("false"), *RunReason));
		return false;
	}

	// ---- THE INSTRUMENT'S CONTROL -------------------------------------------------
	// It recorded many `STRAT-` prefixed lines from this same game, so a low count below is
	// a fact about the emitter and not about a device that cannot hear.
	const int32 AiLines = Capture.CountFrom(LinesBefore, TEXT("STRAT-AI"));
	TestTrue(*FString::Printf(
			TEXT("the capture recorded many STRAT- prefixed lines during this game (%d), so "
			     "its count of the conclusion line is not a blindness"),
			AiLines),
		AiLines > 1);

	const int32 AfterTheGame = Capture.CountFrom(LinesBefore, TEXT("STRAT-MATCH concluded"));
	AddInfo(FString::Printf(TEXT("%d STRAT-MATCH concluded line(s) at the end of the game: %s"),
		AfterTheGame, *Capture.TextFrom(LinesBefore, TEXT("STRAT-MATCH concluded"))));

	TestEqual(TEXT("the transition logged exactly once when the match concluded"),
		AfterTheGame, 1);

	// ---- THE LATCH: refresh, repeatedly, and count again --------------------------
	const int32 kRefreshes = 5;
	int32       Succeeded  = 0;
	for (int32 I = 0; I < kRefreshes; ++I)
	{
		FString RefreshReason;
		if (Subsystem->RefreshPresentation(RefreshReason))
		{
			++Succeeded;
		}
		else
		{
			AddInfo(FString::Printf(TEXT("refresh %d refused: '%s'"), I, *RefreshReason));
		}
	}
	Capture.Settle();

	// Without this, "still exactly one" is satisfied by refreshes that never ran.
	if (!TestEqual(
			TEXT("every post-conclusion RefreshPresentation succeeded, so each of them reached "
			     "ApplyView and therefore ConcludeMatchIfEnded"),
			Succeeded, kRefreshes))
	{
		return false;
	}

	const int32 AfterRefreshes = Capture.CountFrom(LinesBefore, TEXT("STRAT-MATCH concluded"));
	AddInfo(FString::Printf(
		TEXT("%d STRAT-MATCH concluded line(s) after %d further refreshes: %s"),
		AfterRefreshes, kRefreshes,
		*Capture.TextFrom(LinesBefore, TEXT("STRAT-MATCH concluded"))));

	TestEqual(*FString::Printf(
			TEXT("STILL exactly one after %d further presentation refreshes -- the one-shot "
			     "latch holds, and an unlatched transition would read %d here"),
			kRefreshes, 1 + kRefreshes),
		AfterRefreshes, 1);

	return true;
}
