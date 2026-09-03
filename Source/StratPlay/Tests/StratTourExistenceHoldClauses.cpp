// GDD §2.11.2 / T-TURN-09 -- THE TOUR'S EXISTENCE HOLD.
//
// WHAT GAP THESE CLAUSES CLOSE. `RunAiTurnsNow` reconciles the board to the FINAL view model
// and THEN tours the reel, so before the hold landed a unit built during the hand-over was on
// screen from the instant the AI's turn resolved, and a unit killed during it was gone from
// that same instant -- both visibly out of step with the tour that is about to show them being
// built and killed. `UStratMatchSubsystem` now holds those two populations against the reel's
// cursor: `BuildTourExistenceHolds` records which ids appear and depart at which step index,
// `ApplyTourExistenceAtCursor` hides and reveals from the cursor alone, `ApplyView`'s destroy
// loop RETAINS a corpse the tour has not yet shown dying, and `EndAiPlaybackTour` releases all
// of it. Every one of those is new and none of it was executed by any clause.
//
// WHERE THE EXPECTATIONS COME FROM, WHICH IS THE ONLY QUESTION THAT MATTERS. There is not one
// unit id, hex, step index or population count written in this file.
//   - THE TWO POPULATIONS ARE READ OFF `UStratMatchSubsystem::GetViewModel()`, TWICE, and the
//     difference between the two readings is the answer. `AppliedModel` before the hand-over is
//     the pre-hand-over roster; `AppliedModel` after it is the final one. "Departed" is the
//     first set less the second and "appeared" is the second less the first -- both computed
//     from a value the subsystem itself produced, never from a list this file composed.
//   - AND THAT IS THE SAME AUTHORITY THE CODE UNDER TEST ASKS, WHICH IS DELIBERATE AND IS NOT
//     A CIRCULARITY. `EndAiPlaybackTour`'s destroy loop is written against `AppliedModel` and
//     says in terms why: it "destroys every actor the model does not carry, which is the same
//     question `ApplyView` asks and is right even if a retention was recorded for a unit that
//     turned out to survive". So a clause that asked a DIFFERENT authority would be pinning a
//     property nobody claims. What is NOT shared is the private half: `RevealAfterStep`,
//     `HideAfterStep` and `bTourExistenceHeld` are never read here and cannot be, so the maps
//     the feature builds are compared against a roster difference derived independently of
//     them. A hold built from the wrong ids reddens these clauses.
//   - EVERY DISPOSITION IS DRIVEN THROUGH THE SHIPPING VERBS -- `RunAiTurnsNow`,
//     `AdvanceAiPlaybackOneStep`, `SkipAiPlayback` -- and never through a test-only stepping
//     path. `AdvanceAiPlaybackOneStep`'s own declaration records that it is "not a second
//     driver of the reel", which is what makes hand-driving a tour here equivalent to letting
//     the clock drive it, and the reason no clause below ticks a world.
//
// THE CONTROLS ARE LOAD-BEARING AND EACH IS WRITTEN AS AN EARLY RETURN. Every clause here
// asserts over a SET, and a set that came out empty would satisfy every assertion in it
// forever. So each clause first requires its population to be non-empty AND, where the property
// is a retention or a concealment, requires that at least one member was genuinely retained or
// genuinely concealed BEFORE it reads anything into the release. Without that, a fixture whose
// hand-over happened to kill nobody would report green over a deleted feature.
//
// A NOTE ON WHAT `AiPlaybackStepSeconds` SELECTS, because the four clauses split on it. At the
// C++ FIELD DEFAULT of `0.0f` `WillAiPlaybackRun()` returns false, no tour arms and NO HOLD MAY
// BE BUILT -- that is `TheGateIsNeverRaisedWithoutATour`'s whole subject. THE FIELD DEFAULT IS
// NOT WHAT THE SHIPPED GAME RUNS AT: both GameMode Blueprints override the field, so that one
// clause covers a path no real match takes; its own header carries the measurement. At the
// harness
// interval the tour arms for real and the other three clauses run inside it. The harness
// interval is ten minutes and no world here is ever ticked, so nothing advances a tour except a
// clause that asks it to; §2.11.2's 0.5 is NOT written in this file, on
// `AiPlaybackStepSeconds`' own recorded ruling that the pace has one home and a test is not it.
//
// IT NAMES NO `strat::` TYPE AND CALLS NO `strat::` FUNCTION beyond seeding a bridge through
// `FStratBridge`, per `StratPlay.Build.cs`.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

namespace StratTourExistenceHold
{
	/** The same harness pin every parity file in this tree carries. No rule decides which side
	 *  moves first, so this is a harness constant and not a rule. */
	static const int32 kFirstSide = 0;

	/**
	 * A HARNESS INTERVAL AND NOT A PACE, on `StratAiPlaybackClauses.cpp`'s own reasoning.
	 * These worlds are created with `bInformEngineOfWorld = false` and are never ticked, so
	 * nothing advances a tour except a clause that asks it to. §2.11.2's 0.5 is not here.
	 */
	static const float kHarnessPlaybackInterval = 600.0f;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** See `StratMatchConclusion.cpp` on why `InitializeActorsForPlay` is not decoration and
	 *  why the world is destroyed before its context. Duplicated from the other clause files
	 *  in this directory rather than shared, which is this project's standing posture for a
	 *  test helper -- sharing would put a header inside a `Tests/` directory. */
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

		// THE ACTOR CLASSES ARE REQUIRED AND NOT OPTIONAL HERE, unlike in most subsystem
		// clauses: every property below is about an ACTOR existing, being hidden, or being
		// destroyed, so a fixture with no `UnitActorClass` would run every assertion against an
		// empty `UnitActors` map and report green over anything.
		Out.BoardActorClass = AStratBoardActor::StaticClass();
		Out.UnitActorClass  = AStratUnitActor::StaticClass();
		return true;
	}

	/**
	 * Both sides AI, so a whole §2.8 game resolves inside one synchronous `RunAiTurnsNow` --
	 * which is the hand-over these clauses are about.
	 *
	 * THE SIDES ARE READ OFF THE SCENARIO'S OWN VIEW MODEL and are not written here, which is
	 * why this needs a live subsystem to ask: it starts a PROBE match, reads which sides the
	 * scenario actually deploys units for, and hands back a config the caller starts for real.
	 * A hand-written `{0, 1}` would be this file deciding the scenario's roster.
	 *
	 * `PlaybackStepSeconds` IS THE ONE FIELD ITS CALLERS DIFFER IN, and that is the whole split
	 * between `TheGateIsNeverRaisedWithoutATour` and the other three.
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

		UDataTable* const Units = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));

		Out.AiSides               = Sides;
		Out.AiBuildlistUnitIds    = Units != nullptr ? Units->GetRowNames() : TArray<FName>();
		Out.AiPlaybackStepSeconds = PlaybackStepSeconds;
		return true;
	}

	/**
	 * Every unit id in the model the subsystem last APPLIED.
	 *
	 * `GetViewModel()` AND NOT `BuildViewModel`, which is a distinction the subsystem's own
	 * header draws and this file depends on. `AppliedModel` is "what was drawn"; a fresh
	 * `BuildViewModel` is "what the rules say right now", and after an AI-vs-AI hand-over
	 * reaches a §2.8 result those are two different questions asked at two instants. The
	 * feature under test consults the APPLIED model -- see `EndAiPlaybackTour` -- so this does
	 * too.
	 */
	static TSet<int32> AppliedUnitIds(const UStratMatchSubsystem& Match)
	{
		TSet<int32> Ids;
		for (const FStratUnitView& View : Match.GetViewModel().Units)
		{
			Ids.Add(View.UnitId);
		}
		return Ids;
	}

	/** `A` less `B`, ascending, so a red run prints a stable list. */
	static TArray<int32> Difference(const TSet<int32>& A, const TSet<int32>& B)
	{
		TArray<int32> Out;
		for (const int32 Id : A)
		{
			if (!B.Contains(Id))
			{
				Out.Add(Id);
			}
		}
		Out.Sort();
		return Out;
	}

	static FString Describe(const TArray<int32>& Ids)
	{
		if (Ids.Num() == 0)
		{
			return TEXT("(none)");
		}
		FString Out;
		for (const int32 Id : Ids)
		{
			Out += Out.IsEmpty() ? FString::Printf(TEXT("%d"), Id)
			                     : FString::Printf(TEXT(", %d"), Id);
		}
		return Out;
	}

	/**
	 * Declares the two log lines an AI-vs-AI hand-over on a fixture world always emits.
	 *
	 * DECLARED FROM A MEASURED RUN AND NOT SPECULATIVELY. `AddExpectedMessagePlain` with
	 * `Occurrences 0` FAILS a clause where the message never fires, so a defensive declaration
	 * is itself an assertion -- the point `StratAiPlaybackClauses.cpp` records about the same
	 * two lines, on the same fixture shape, and the reason both are declared together here
	 * rather than one being added "just in case".
	 */
	static void DeclareHandoverNoise(FAutomationTestBase& Test)
	{
		// A fixture world has no tile meshes, so the board reports it cannot draw them.
		Test.AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
			EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

		// THE CLOSING EndTurn OF THE TURN THAT WON THE GAME. A result lands mid-turn, so the
		// AI's own EndTurn is then refused with '[T-SAVE-05] no match is running'. That is the
		// behaviour `T-AI-01.AMatchWonMidTurnIsNotReportedAsAnAiFault` owns; these clauses only
		// need the hand-over to have happened, and declare the line rather than asserting
		// anything about it.
		Test.AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
			EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
	}
}

// ---------------------------------------------------------------------------
// T-TURN-09 -- A UNIT BUILT DURING THE HAND-OVER IS HIDDEN UNTIL ITS STEP.
//
// §2.11.2 asks the presentation layer to "replay its action list at a watchable fixed pace".
// A unit the AI built on step 90 of 156 standing on the board while the tour shows step 1 is
// the action list and the board disagreeing, and it is what `RevealAfterStep` exists to stop.
//
// THE POPULATION IS DERIVED AND NOT NAMED. "Appeared" is the applied model AFTER the hand-over
// less the applied model BEFORE it -- which is exactly the id set `BuildTourExistenceHolds`
// puts in `RevealAfterStep` once its both-maps purge has run, arrived at by a different route.
// Note what is deliberately NOT in that set and is not expected to be: a unit built AND killed
// inside one hand-over is in neither model, has no actor and is out of this feature's scope by
// construction -- the subsystem states that limit and this clause does not contradict it.
//
// THE THREE PROPERTIES, IN THE ORDER THEY ARE ASSERTED.
//   (1) AT THE TOUR'S START AT LEAST ONE APPEARED UNIT IS CONCEALED. This is both a property
//       and the control: if none were hidden every other assertion below would be about
//       nothing. It is "at least one" and not "all", and that is not a softening -- the tour's
//       first step is shown inside `BeginAiPlayback`, so the cursor is at 1 when
//       `RunAiTurnsNow` returns and a unit that appeared on the very first command is
//       legitimately already visible. Asserting "all" would be asserting a fixture property.
//   (2) EVERY CONCEALED UNIT IS REVEALED WHILE THE TOUR IS STILL RUNNING, and the cursor at
//       which each is revealed is recorded and reported. "While still running" is the
//       load-bearing half: `EndAiPlaybackTour` unhides unconditionally, so a unit that only
//       became visible at the tour's end would be evidence of the RELEASE working and of the
//       reveal doing nothing at all.
//   (3) AND NOTHING IS LEFT HIDDEN once the tour is over.
//
// WHAT THIS CLAUSE DOES NOT PIN, STATED HERE RATHER THAN LEFT TO BE DISCOVERED. It does not
// pin the `ApplyTourExistenceAtCursor()` call in `BeginAiPlayback`, and no headless clause can:
// that function ends by calling `OnAiPlaybackTimer()`, which reaches
// `AdvanceAiPlaybackOneStep`, which calls the same pure function again at cursor 1 before
// `BeginAiPlayback` returns. The two calls differ only for a unit revealed at step index 0 and
// only in the window between them -- a window the subsystem's own comment says no frame is
// rendered in. MEASURED, NOT ASSUMED: deleting that call was run as a mutation against this
// clause and this clause stayed green, and `Tools/architect/state/tests.md` records it as an
// unpinnable line together with the seam that would close it. What IS pinned is that the reveal
// mechanism works at all -- deleting `ApplyTourExistenceAtCursor()` from
// `AdvanceAiPlaybackOneStep` reddens property (2), measured.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratUnitBuiltDuringHandoverIsHiddenTest,
	"Stratocracy.StratPlay.T-TURN-09.AUnitBuiltDuringTheHandoverIsHiddenUntilItsStep",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratUnitBuiltDuringHandoverIsHiddenTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratTourExistenceHold;

	DeclareHandoverNoise(*this);

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
	if (!TestTrue(
			TEXT("CONTROL: the interval is POSITIVE, so `WillAiPlaybackRun` is true and a hold "
			     "is built at all -- at a non-positive interval no hold exists and every read "
			     "below "
			     "would be about an absent feature"),
			Config.AiPlaybackStepSeconds > 0.0f))
	{
		return false;
	}

	Match->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the match is live"), Match->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	// THE PRE-HAND-OVER ROSTER, off the model the subsystem has already applied.
	const TSet<int32> Before = AppliedUnitIds(*Match);
	if (!TestTrue(TEXT("CONTROL: the opening roster is non-empty"), Before.Num() > 0))
	{
		return false;
	}

	FString    RunReason;
	const bool bRan = Match->RunAiTurnsNow(RunReason);
	AddInfo(FString::Printf(TEXT("RunAiTurnsNow returned %s; reason: '%s'; %d steps recorded"),
		bRan ? TEXT("true") : TEXT("false"), *RunReason, Match->GetAiPlaybackStepCount()));

	const TSet<int32>   After    = AppliedUnitIds(*Match);
	const TArray<int32> Appeared = Difference(After, Before);

	AddInfo(FString::Printf(TEXT("the hand-over added these unit ids to the roster: %s"),
		*Describe(Appeared)));

	if (!TestTrue(
			TEXT("CONTROL: the hand-over built at least one unit that survived it -- without "
			     "this the assertions below would all be about the empty set"),
			Appeared.Num() > 0))
	{
		return false;
	}

	// ---- (1) At the tour's start, at least one of them is concealed -----------
	TArray<int32> Concealed;
	for (const int32 Id : Appeared)
	{
		AStratUnitActor* const Actor = Match->FindUnitActor(Id);
		if (!TestTrue(*FString::Printf(
				TEXT("unit %d appeared during the hand-over and is in the final model, so the "
				     "reconcile spawned an actor for it"), Id),
				Actor != nullptr))
		{
			return false;
		}
		if (Actor->IsHidden())
		{
			Concealed.Add(Id);
		}
	}

	AddInfo(FString::Printf(
		TEXT("at cursor %d of %d these appeared units are concealed: %s"),
		Match->GetAiPlaybackCursor(), Match->GetAiPlaybackStepCount(), *Describe(Concealed)));

	if (!TestTrue(
			*FString::Printf(
				TEXT("T-TURN-09: a unit built during the hand-over is CONCEALED when the tour "
				     "begins -- %d of the %d units the hand-over built are hidden. Zero here is "
				     "the defect this hold exists to repair and is also the condition under "
				     "which everything below would assert nothing"),
				Concealed.Num(), Appeared.Num()),
			Concealed.Num() > 0))
	{
		return false;
	}

	// ---- (2) Each is revealed WHILE THE TOUR IS STILL RUNNING -----------------
	// A MAP FROM ID TO THE CURSOR IT WAS FIRST SEEN AT, so a red run says which unit was still
	// concealed and how far the tour had got, rather than only that something was wrong.
	TMap<int32, int32> RevealedAtCursor;

	if (!TestTrue(TEXT("CONTROL: a tour is genuinely under way before any step is driven"),
			Match->IsAiPlaybackRunning()))
	{
		return false;
	}

	// HAND-DRIVEN THROUGH THE SHIPPING VERB. `AdvanceAiPlaybackOneStep` is the one
	// implementation of stepping and the timer is a caller of it, so this drives what the
	// clock drives. The loop is bounded by the recorded step count and not by `IsPlaying`
	// alone, so a stepping bug that failed to advance cannot hang the suite.
	const int32 StepBound = Match->GetAiPlaybackStepCount();
	for (int32 Taken = 0; Taken < StepBound && Match->IsAiPlaybackRunning(); ++Taken)
	{
		if (!Match->AdvanceAiPlaybackOneStep())
		{
			break;
		}

		// READ AFTER THE ADVANCE AND ONLY WHILE THE TOUR IS STILL RUNNING. The advance that
		// takes the last step calls `EndAiPlaybackTour`, which unhides EVERY actor
		// unconditionally -- so a visibility read taken after that moment cannot tell a reveal
		// from a release, and recording one would be the clause measuring the wrong verb.
		if (!Match->IsAiPlaybackRunning())
		{
			break;
		}

		const int32 Cursor = Match->GetAiPlaybackCursor();
		for (const int32 Id : Concealed)
		{
			if (RevealedAtCursor.Contains(Id))
			{
				continue;
			}
			AStratUnitActor* const Actor = Match->FindUnitActor(Id);
			if (Actor != nullptr && !Actor->IsHidden())
			{
				RevealedAtCursor.Add(Id, Cursor);
			}
		}
	}

	for (const int32 Id : Concealed)
	{
		const int32* const At = RevealedAtCursor.Find(Id);
		if (TestTrue(*FString::Printf(
				TEXT("T-TURN-09: unit %d, concealed when the tour began, is REVEALED at a step "
				     "of the tour rather than only when the tour ends -- a unit that first "
				     "became visible at the release would be evidence of `EndAiPlaybackTour` "
				     "working and of the reveal doing nothing"), Id),
				At != nullptr))
		{
			AddInfo(FString::Printf(TEXT("unit %d was revealed at cursor %d of %d"),
				Id, *At, StepBound));
		}
	}

	// ---- (3) And nothing is left hidden once the tour is over ------------------
	Match->SkipAiPlayback();
	TestFalse(TEXT("the tour is over"), Match->IsAiPlaybackRunning());

	for (const int32 Id : Appeared)
	{
		AStratUnitActor* const Actor = Match->FindUnitActor(Id);
		if (TestTrue(*FString::Printf(
				TEXT("unit %d is still on the board after the tour, because the final model "
				     "carries it"), Id),
				Actor != nullptr))
		{
			TestFalse(*FString::Printf(
				TEXT("T-TURN-09: and unit %d is VISIBLE once the tour is over -- no unit the "
				     "hold concealed may outlive it hidden"), Id),
				Actor->IsHidden());
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-TURN-09 -- A UNIT KILLED DURING THE HAND-OVER SURVIVES UNTIL ITS STEP.
//
// THE OTHER HALF, AND IT IS THE HARDER ONE. A concealment is reversible; a destruction is not,
// which is why `ApplyView`'s destroy loop RETAINS rather than destroys and why the retention is
// a `continue` and not a hide -- hiding there "would make it vanish at exactly the moment the
// hold exists to prevent". This clause is the first thing to execute that branch.
//
// THE POPULATION IS DERIVED AND NOT NAMED. "Departed" is the applied model BEFORE the hand-over
// less the applied model AFTER it. That set is exactly `HideAfterStep`'s key set: every unit in
// the pre-hand-over model had an actor, which is the `UnitActors` filter
// `BuildTourExistenceHolds` applies, and a unit in both maps is purged from both. Reached by a
// different route from the feature's own, so a hold built from the wrong ids reddens here.
//
// THE TWO PROPERTIES.
//   (1) AFTER THE RECONCILE, EVERY DEPARTED UNIT STILL HAS AN ACTOR. `RefreshPresentation` ran
//       inside `RunAiTurnsNow` and `ApplyView`'s destroy loop saw an id the final model does
//       not carry; without the hold that actor is gone by the time this clause looks, which is
//       the behaviour that shipped before. THIS IS THE ASSERTION THE ONE-LINE MUTATION TARGETS.
//   (2) AND AFTER THE TOUR IS DRIVEN TO ITS END, NONE OF THEM DOES. A retention that outlived
//       its tour would strand a corpse on the board permanently, which is worse than the defect
//       being repaired -- the subsystem says so in terms.
//
// THE END IS REACHED BY STEPPING AND NOT BY SKIPPING, which is what separates this clause from
// `EndingATourDestroysEveryRetainedCorpse` next door: that one asserts the SKIP route releases
// the hold, this one asserts the ordinary route does. `AdvanceAiPlaybackOneStep`'s declaration
// promises the two leave identical state; the pair of clauses is what would notice if they
// stopped.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratUnitKilledDuringHandoverSurvivesTest,
	"Stratocracy.StratPlay.T-TURN-09.AUnitKilledDuringTheHandoverSurvivesUntilItsStep",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratUnitKilledDuringHandoverSurvivesTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratTourExistenceHold;

	DeclareHandoverNoise(*this);

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
	if (!TestTrue(
			TEXT("CONTROL: the interval is POSITIVE, so a hold is built at all"),
			Config.AiPlaybackStepSeconds > 0.0f))
	{
		return false;
	}

	Match->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the match is live"), Match->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	const TSet<int32> Before = AppliedUnitIds(*Match);
	if (!TestTrue(TEXT("CONTROL: the opening roster is non-empty"), Before.Num() > 0))
	{
		return false;
	}

	// CONTROL: EVERY UNIT IN THE OPENING ROSTER HAS AN ACTOR, ASSERTED BEFORE THE HAND-OVER
	// RUNS. Without this, "the actor is still there afterwards" could be satisfied by a fixture
	// in which no actor was ever spawned -- `FindUnitActor` would answer null both times and
	// only property (2) would notice.
	for (const int32 Id : Before)
	{
		if (!TestTrue(*FString::Printf(
				TEXT("CONTROL: unit %d of the opening roster has an actor before the hand-over"),
				Id),
				Match->FindUnitActor(Id) != nullptr))
		{
			return false;
		}
	}

	FString    RunReason;
	const bool bRan = Match->RunAiTurnsNow(RunReason);
	AddInfo(FString::Printf(TEXT("RunAiTurnsNow returned %s; reason: '%s'; %d steps recorded"),
		bRan ? TEXT("true") : TEXT("false"), *RunReason, Match->GetAiPlaybackStepCount()));

	const TSet<int32>   After    = AppliedUnitIds(*Match);
	const TArray<int32> Departed = Difference(Before, After);

	AddInfo(FString::Printf(TEXT("the hand-over removed these unit ids from the roster: %s"),
		*Describe(Departed)));

	if (!TestTrue(
			TEXT("CONTROL: the hand-over killed at least one unit that was on the board when it "
			     "started -- without this every assertion below is about the empty set"),
			Departed.Num() > 0))
	{
		return false;
	}

	// ---- (1) The retention, which is the property the mutation removes --------
	for (const int32 Id : Departed)
	{
		TestTrue(*FString::Printf(
			TEXT("T-TURN-09: unit %d left the model during the hand-over and its actor SURVIVED "
			     "the reconcile, because the tour has not yet shown it dying -- `ApplyView`'s "
			     "destroy loop destroys it without the hold and the player never sees the death"),
			Id),
			Match->FindUnitActor(Id) != nullptr);
	}

	if (!TestTrue(TEXT("CONTROL: and a tour is genuinely under way holding them there"),
			Match->IsAiPlaybackRunning()))
	{
		return false;
	}

	// ---- (2) And the ordinary route -- stepping to the end -- releases them ----
	const int32 StepBound = Match->GetAiPlaybackStepCount();
	int32       Taken     = 0;
	while (Taken < StepBound && Match->IsAiPlaybackRunning())
	{
		if (!Match->AdvanceAiPlaybackOneStep())
		{
			break;
		}
		++Taken;
	}
	AddInfo(FString::Printf(TEXT("the tour was hand-driven %d steps to cursor %d of %d"),
		Taken, Match->GetAiPlaybackCursor(), StepBound));

	if (!TestFalse(
			TEXT("CONTROL: stepping to the end ended the tour, so what follows is a released "
			     "hold and not a running one"),
			Match->IsAiPlaybackRunning()))
	{
		return false;
	}

	for (const int32 Id : Departed)
	{
		TestTrue(*FString::Printf(
			TEXT("T-TURN-09: and unit %d has NO actor once the tour is over -- a retention that "
			     "outlived its tour would stand a corpse on the board for the rest of the match"),
			Id),
			Match->FindUnitActor(Id) == nullptr);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-TURN-09 -- ENDING A TOUR DESTROYS EVERY RETAINED CORPSE.
//
// **THE SHARPEST OF THE FOUR, AND THE REASON IS THE ALTERNATIVE IT RULES OUT RATHER THAN THE
// BEHAVIOUR IT ASSERTS.** `EndAiPlaybackTour` destroys the retained actors itself. The obvious
// alternative -- and the one a future reader tidying this function would reach for -- is to
// DEFER: drop the gate and let the next `ApplyView` destroy loop do it, which it would do
// correctly, with less code, and with no second authority. That refactor is WRONG, and it is
// wrong for a reason no ordinary test would notice: after a tour ends the next `ApplyView` may
// be a mouse crossing a hex boundary, a command, or NOTHING AT ALL. If the player's cursor
// never moves again, a unit the rules module killed forty steps ago stands on the board for the
// rest of the match.
//
// SO THIS CLAUSE'S SHAPE IS DECIDED BY THAT ALTERNATIVE AND NOT BY THE FEATURE. It reads the
// board **after the skip and before any further model is applied**, and it applies none -- no
// `RefreshPresentation`, no command, no second `StartMatch`, nothing that could reach
// `ApplyView`. A clause that refreshed anything between the skip and the read would GO GREEN
// OVER THE DEFERRED IMPLEMENTATION, which is precisely the defect, and would then stand as
// permanent evidence that the wrong design is fine. The absence of a refresh call in this
// clause is therefore load-bearing and must not be "tidied" either.
//
// THE SKIP IS §2.11.2's OWN VERB -- "any click or Esc skips to the end state" -- reached
// through `SkipAiPlayback`, which is what `AStratPlayerController::HandleSelectionEvent` calls.
//
// THE CONTROL. Every assertion here is an ABSENCE, and an absence over an empty set is free. So
// the clause first requires that at least one corpse was genuinely RETAINED at the moment
// before the skip: an id the applied model does not carry that nonetheless still has an actor.
// Without that read, a tree in which the retention never happened at all would satisfy every
// line below.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratEndingATourDestroysEveryRetainedCorpseTest,
	"Stratocracy.StratPlay.T-TURN-09.EndingATourDestroysEveryRetainedCorpse",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratEndingATourDestroysEveryRetainedCorpseTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratTourExistenceHold;

	DeclareHandoverNoise(*this);

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
	if (!TestTrue(TEXT("CONTROL: the interval is POSITIVE, so a hold is built at all"),
			Config.AiPlaybackStepSeconds > 0.0f))
	{
		return false;
	}

	Match->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the match is live"), Match->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	const TSet<int32> Before = AppliedUnitIds(*Match);

	FString    RunReason;
	const bool bRan = Match->RunAiTurnsNow(RunReason);
	AddInfo(FString::Printf(TEXT("RunAiTurnsNow returned %s; reason: '%s'; %d steps recorded"),
		bRan ? TEXT("true") : TEXT("false"), *RunReason, Match->GetAiPlaybackStepCount()));

	const TSet<int32>   After    = AppliedUnitIds(*Match);
	const TArray<int32> Departed = Difference(Before, After);

	// ---- THE CONTROL: A CORPSE IS GENUINELY BEING RETAINED RIGHT NOW ----------
	TArray<int32> Retained;
	for (const int32 Id : Departed)
	{
		if (Match->FindUnitActor(Id) != nullptr)
		{
			Retained.Add(Id);
		}
	}
	AddInfo(FString::Printf(
		TEXT("the applied model does not carry these ids: %s; of those, these still have an "
		     "actor immediately before the skip: %s"),
		*Describe(Departed), *Describe(Retained)));

	if (!TestTrue(
			*FString::Printf(
				TEXT("CONTROL: at least one actor is being RETAINED against the applied model at "
				     "the instant before the skip (%d of %d departed ids). Every assertion below "
				     "is an absence, and an absence over the empty set is free"),
				Retained.Num(), Departed.Num()),
			Retained.Num() > 0))
	{
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: and a tour is genuinely under way to be skipped"),
			Match->IsAiPlaybackRunning()))
	{
		return false;
	}

	// ---- §2.11.2's own verb, and NOTHING AFTER IT -----------------------------
	TestTrue(TEXT("the skip reports that it ended a tour that was running"),
		Match->SkipAiPlayback());
	TestFalse(TEXT("and no tour is under way afterwards"), Match->IsAiPlaybackRunning());

	// **NO MODEL IS APPLIED BETWEEN THE SKIP AND THIS READ, AND THAT IS THE CLAUSE.** See the
	// block above: a `RefreshPresentation` here would let a deferred implementation -- gate
	// dropped, destruction left to the next `ApplyView` -- pass this clause forever, and the
	// deferred implementation is the defect. The next thing this file does is read the board.
	for (const int32 Id : Departed)
	{
		TestTrue(*FString::Printf(
			TEXT("T-TURN-09: unit %d is not in the applied model, and ending the tour DESTROYED "
			     "its actor itself rather than leaving it for a next `ApplyView` that may never "
			     "come -- with no model applied since the skip"), Id),
			Match->FindUnitActor(Id) == nullptr);
	}

	// AND EVERY SURVIVOR IS STILL THERE. The release destroys against the APPLIED MODEL, so a
	// version that destroyed against `HideAfterStep` -- or against everything -- would be
	// caught here rather than by the absences above, which such a version would satisfy.
	for (const int32 Id : After)
	{
		TestTrue(*FString::Printf(
			TEXT("T-TURN-09: and unit %d, which the applied model DOES carry, still has its "
			     "actor -- the release destroys against the model and not against the hold"), Id),
			Match->FindUnitActor(Id) != nullptr);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-TURN-09 -- THE GATE IS NEVER RAISED WITHOUT A TOUR.
//
// THE STRANDING HAZARD, AND IT IS THE ONE FAILURE OF THIS FEATURE WITH NO RECOVERY. The hold is
// BUILT in `RunAiTurnsNow`, above `RefreshPresentation`, because that is the last instant at
// which "which units did this hand-over create and destroy" is answerable. It is RELEASED by
// `EndAiPlaybackTour`, which only ever runs for a tour. So if the build were gated on anything
// other than the exact condition that starts a tour, a hand-over could raise the gate with no
// tour to release it -- and `ApplyView`'s destroy loop would then retain every killed unit
// FOREVER, with no verb left in the class to remove it. `WillAiPlaybackRun()` exists to make
// that state unconstructible: one `const` function, two callers, and "gate up, no tour" is
// unreachable by construction rather than by care.
//
// THE FIXTURE IS THE C++ FIELD DEFAULT, AND THAT IS A PATH THE SHIPPED GAME NEVER TAKES.
// `AiPlaybackStepSeconds` initialises to `0.0f` in `FStratMatchConfig`, and it would be easy to
// call that "the shipped default" -- it is not. BOTH shipped GameMode Blueprints override the
// field: the property name is serialised in the raw bytes of
// `Content/StratPlay/BP_StratGameMode.uasset` and of `BP_StratGameMode_AiVsAi.uasset`, measured
// with `grep -aF` and discriminated by controls -- `MoveTweenSeconds` gives the
// mirror-image result (present in `BP_StratUnit`, absent from both GameModes) and a fabricated
// property name is absent from all three. `Tools/architect/state/global.md` records the measured
// value as 0.5 on both. So `WillAiPlaybackRun()` is TRUE in every real match and THIS CLAUSE
// COVERS NO SHIPPED PATH.
//
// WHAT IT PINS IS STILL WORTH PINNING, stated as a property of the code rather than of the game:
// the gate cannot be raised at a non-positive interval, whoever produces one -- a designer
// zeroing the Blueprint, a config path that leaves the field unwritten, or any future caller
// assembling an `FStratMatchConfig` in C++ and never touching this field. The stranding hazard
// above is unrecoverable, so the guard is worth a clause even on a path content currently steers
// around; what it must not claim is coverage of the path content actually takes. Its matched pair
// is any of the three clauses above, which run the identical fixture with the identical
// hand-over at a POSITIVE interval and assert the OPPOSITE disposition -- retained and
// concealed. Between them, the interval is shown to be the only thing that decides.
//
// WHAT IS OBSERVED, SINCE THE GATE ITSELF CANNOT BE. `bTourExistenceHeld`, `RevealAfterStep`
// and `HideAfterStep` are private and there is no route to them from `Tests/`. The gate is
// observed through its two and only two consequences: a retained actor and a hidden one. That
// is not a weakening -- a gate raised with no consequence is a gate nobody can be harmed by,
// and the consequences are what the feature is.
//
// THE CONTROL. Both assertions are absences, so the clause first requires that the hand-over
// genuinely killed somebody and genuinely recorded a reel. A hand-over in which nobody died
// would satisfy the retention half over the empty set.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGateIsNeverRaisedWithoutATourTest,
	"Stratocracy.StratPlay.T-TURN-09.TheGateIsNeverRaisedWithoutATour",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGateIsNeverRaisedWithoutATourTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratTourExistenceHold;

	DeclareHandoverNoise(*this);

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

	// THE C++ FIELD DEFAULT, TAKEN OFF A DEFAULT-CONSTRUCTED `FStratMatchConfig` AND NOT WRITTEN
	// AS A LITERAL. `0.0f` appears nowhere in this clause: the number comes from the field
	// initialiser itself, so if that initialiser ever changes this fixture changes with it rather
	// than silently continuing to test a value nothing produces. It is NOT the value a shipping
	// match gets -- both GameMode Blueprints override the field -- so read it as "whatever
	// non-positive value the struct hands you", never as the pace the game runs at.
	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(
			TEXT("an AI-vs-AI config assembles at the C++ field default for playback, which no "
			     "shipped GameMode Blueprint uses"),
			MakeAiVsAiConfig(*Match, FStratMatchConfig().AiPlaybackStepSeconds, Config, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(
			TEXT("CONTROL: the interval is NON-POSITIVE, so `WillAiPlaybackRun` is false and no "
			     "tour can arm -- this clause is about the path with no releaser, and at a "
			     "positive interval it would be asserting the opposite of the other three"),
			Config.AiPlaybackStepSeconds <= 0.0f))
	{
		return false;
	}

	Match->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the match is live"), Match->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	const TSet<int32> Before = AppliedUnitIds(*Match);

	FString    RunReason;
	const bool bRan = Match->RunAiTurnsNow(RunReason);
	AddInfo(FString::Printf(TEXT("RunAiTurnsNow returned %s; reason: '%s'"),
		bRan ? TEXT("true") : TEXT("false"), *RunReason));

	const TSet<int32>   After    = AppliedUnitIds(*Match);
	const TArray<int32> Departed = Difference(Before, After);

	// ---- THE CONTROLS ---------------------------------------------------------
	const int32 StepsRecorded = Match->GetAiPlaybackStepCount();
	if (!TestTrue(
			*FString::Printf(
				TEXT("CONTROL: the hand-over recorded a reel (%d steps), so a hold COULD have "
				     "been built from it -- an empty reel builds an empty hold and this clause "
				     "would then be about nothing"), StepsRecorded),
			StepsRecorded > 0))
	{
		return false;
	}
	if (!TestTrue(
			*FString::Printf(
				TEXT("CONTROL: the hand-over killed at least one unit that was on the board when "
				     "it started (%s), so there is something a raised gate would retain"),
				*Describe(Departed)),
			Departed.Num() > 0))
	{
		return false;
	}
	TestFalse(TEXT("CONTROL: and no tour is running, which is what leaves no releaser"),
		Match->IsAiPlaybackRunning());

	// ---- CONSEQUENCE ONE: NOTHING IS RETAINED ---------------------------------
	for (const int32 Id : Departed)
	{
		TestTrue(*FString::Printf(
			TEXT("T-TURN-09: at a non-positive playback interval unit %d left the model and its "
			     "actor was "
			     "DESTROYED at the reconcile -- a hold raised here has no tour to release it and "
			     "would stand this corpse on the board for the rest of the match"), Id),
			Match->FindUnitActor(Id) == nullptr);
	}

	// ---- CONSEQUENCE TWO: NOTHING IS HIDDEN -----------------------------------
	for (const int32 Id : After)
	{
		AStratUnitActor* const Actor = Match->FindUnitActor(Id);
		if (TestTrue(*FString::Printf(
				TEXT("unit %d is in the applied model and has an actor"), Id),
				Actor != nullptr))
		{
			TestFalse(*FString::Printf(
				TEXT("T-TURN-09: and unit %d is VISIBLE -- with no tour running nothing may be "
				     "concealed against a cursor that will never move"), Id),
				Actor->IsHidden());
		}
	}

	return true;
}
