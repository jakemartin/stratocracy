// The gate on `FStratGuidedOpening` -- GDD §2.11.6-B, under T-UI-03.
//
// WHAT IS PINNED HERE. §2.11.6-B gives four beats, two line-assignment rules, four
// retirement triggers, and a three-column schedule table that says which beat holds the
// strip on each of the four turns in each of three pip-timing branches. The table is the
// spec's own worked example of rules 1-2, so reproducing it end to end is the strongest
// available statement that the beat model is the GDD's and not a plausible neighbour of it.
//
// EVERY CLAUSE ASSERTS THE RULE AS WELL AS THE BEAT. `FStratGuidedOpening::
// RuleThatSelectedTheLine` exists for no other purpose -- its declaration says so: "EXPOSED
// PURELY SO THE SCHEDULE TABLE CAN BE PINNED ... a clause that could only check the beat
// would pass on a model that reproduced the beats by coincidence." The table labels every
// cell with a rule, and a beat-only clause would be blind to the difference between a rule-1
// pick and a rule-2 last call that happened to land on the same beat -- which is precisely
// what separates the wandered column's turn 3 from its turn 4.
//
// WHERE THE EXPECTATIONS COME FROM. The beats and rules asserted below are the GDD's
// schedule table, §2.11.6-B, transcribed cell for cell:
//
//   | Turn | Common case (pip turn 2)        | Wandered (pip turn 3-4)     | Fast lane (pip turn 1) |
//   | 1    | 1a, then 1b when 1a retires     | 1a, then 1b                 | 1a, then 1b; beat 2 retires unheld |
//   | 2    | beat 2 (rule 1)                 | beat 2 (rule 1)             | beat 3 (rule 1)        |
//   | 3    | beat 3 (rule 1)                 | beat 3 (rule 1)             | beat 3 -- rule 2, UNTAGGED |
//   | 4    | beat 3 -- rule 2, tagged        | beat 2 -- rule 2, tagged    | beat 3 -- rule 2 again, tagged |
//
// THAT TABLE IS A LITERAL, AND IT IS THE ONE SHAPE IN WHICH A LITERAL IS CORRECT: no
// module-side value produces it. `FStratGuidedOpening` IS the implementation of it, so
// reading the expectation off the machine under test would be the self-testing shape this
// project's test rule forbids by name. Every OTHER value in this file is read: the marked
// unit is `FStratUnitView::bIsGuidedMarked` (derived by the rules module off `placement`),
// the objective is `FStratBridge::GuidedOpeningHexes`, and the model is
// `StratBuildViewModel` over a live bridge.
//
// THE MODEL IS EDITED BY HAND, AND THAT IS FIXTURE CONSTRUCTION. Setting `bHasMoved` on the
// marked unit, raising `CaptureProgress`, and advancing `Match.Turn` are the three INPUTS
// this machine observes; the beat, the rule and the tag are its OUTPUTS and are what is
// asserted. The same standing `StratMatchReconcile.cpp` and `StratSelectionMachineParity.cpp`
// both record for the models they edit.
//
// WHY THE TURN IS SET RATHER THAN PLAYED. `FStratGuidedOpening` reads exactly one turn field
// -- `FStratMatchView::Turn`, which is a ROUND -- and reaching turn 4 by submitting real
// commands would take a scripted eight-half-turn game whose every step could refuse for
// reasons that have nothing to do with a beat. The machine cannot tell a set field from a
// played one, and the clause is about the machine.
//
// NO `strat::` TYPE IS NAMED AND NO `strat::` FREE FUNCTION IS CALLED. That is
// `StratPlay.Build.cs`'s stated property of this module and a test file is part of the
// module; measured 8 x LNK2019 the first time a call crossed a module boundary.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Math/IntPoint.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratGuidedOpening.h"
#include "StratSelectionMachine.h"
#include "StratViewModel.h"

// Included from a .cpp and never from a UHT-parsed header -- the module-wide discipline.
#include "StratBridge.h"

namespace StratGuidedOpeningClauses
{
	static const int32 kFirstSide = 0;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	static FString ScenarioPath()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"), TEXT("ferrum_crossing.json"));
	}

	/**
	 * A beat as a comparable, readable string.
	 *
	 * `FString` AND NOT `const TCHAR*`, and not the raw enum either. `TestEqual` over two
	 * `const TCHAR*` binds the generic template and compares POINTERS; `TestEqual` over an
	 * unscoped-castable enum prints a number a reader has to decode against the header. An
	 * `FString` gets the string overload and puts `beat 1b` in the failure message.
	 */
	static FString BeatName(EStratGuidanceBeat Beat)
	{
		switch (Beat)
		{
		case EStratGuidanceBeat::Beat1a: return TEXT("beat 1a");
		case EStratGuidanceBeat::Beat1b: return TEXT("beat 1b");
		case EStratGuidanceBeat::Beat2:  return TEXT("beat 2");
		case EStratGuidanceBeat::Beat3:  return TEXT("beat 3");
		default:                         return TEXT("no beat");
		}
	}

	/**
	 * A seeded bridge, a built model, a selection machine and an armed guided opening.
	 *
	 * THE MACHINE IS HERE BECAUSE `Observe` REQUIRES ONE. `FStratGuidedOpening::PublishLocks`
	 * writes the lock set through it on every observation, so a clause cannot drive the beat
	 * model without one. What the locks DO is pinned by
	 * `T-INT-05.LockArisesFromTheGuidanceLayer` in `StratSelectionMachineParity.cpp`; here the
	 * machine is only a sink.
	 */
	struct FGuidanceHarness
	{
		FStratBridge           Bridge;
		FStratViewModel        Model;
		FStratSelectionMachine Machine;
		FStratGuidedOpening    Guidance;
		int32                  MarkedUnitId = INDEX_NONE;
		int32                  GuidedSide   = 0;

		/** The ringed Factory, read from `FStratBridge::GuidedOpeningHexes`. Never a literal. */
		FIntPoint              ObjectiveHex = FIntPoint::ZeroValue;

		bool Arm(bool bSuppressed, FString& OutError)
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

			const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath(), kFirstSide);
			if (!Seeded.bOk)
			{
				OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
				return false;
			}

			GuidedSide = Bridge.SideToMove();
			if (!StratBuildViewModel(Bridge, GuidedSide, Model, OutError))
			{
				return false;
			}

			// The rules module's own answer, read and never derived off a hex.
			for (const FStratUnitView& U : Model.Units)
			{
				if (U.Side == GuidedSide && U.bIsGuidedMarked)
				{
					MarkedUnitId = U.UnitId;
					break;
				}
			}
			if (MarkedUnitId == INDEX_NONE)
			{
				OutError = TEXT("the shipped scenario marks no Infantry for the guided seat");
				return false;
			}

			// The ringed objective, from the only door that authored answer comes through.
			// Read here so `CapturePipHasLanded` can put the pip ON it rather than on
			// whichever friendly unit happens to be first in the array.
			FIntPoint AuthoredInfantry = FIntPoint::ZeroValue;
			const FStratResult Guided =
				Bridge.GuidedOpeningHexes(GuidedSide, AuthoredInfantry, ObjectiveHex);
			if (!Guided.bOk && !bSuppressed)
			{
				OutError = FString::Printf(TEXT("GuidedOpeningHexes refused: %s"), *Guided.Reason);
				return false;
			}

			Guidance.Begin(Bridge, GuidedSide, bSuppressed);
			return true;
		}

		/** Fixture input: beat 1a's retirement trigger, read off `bHasMoved`. */
		void MarkedInfantryHasMoved()
		{
			for (FStratUnitView& U : Model.Units)
			{
				if (U.UnitId == MarkedUnitId)
				{
					U.bHasMoved = true;
				}
			}
		}

		/**
		 * Fixture input: beat 2's retirement trigger, a capture pip ON THE RINGED OBJECTIVE.
		 *
		 * THE PIP IS PLACED ON `guidedOpening.objective` AND NOWHERE ELSE, because that is what
		 * §2.11.6-B's "Retires when" cell means as ruled. The cell is unqualified in the GDD --
		 * "A capture pip appears -- on whatever turn that happens" -- while the directive text
		 * names the ring, and the two readings disagree. The narrow reading is the ruling, made
		 * 2026-08-21, and `FStratGuidedOpening::HasCapturePipLanded` implements it: its predicate
		 * is `bHasObjective` AND a guided-seat unit with `CaptureProgress > 0` AND that unit's
		 * `Hex == Objective`.
		 *
		 * NOTHING IN THIS FILE ASSERTS THE UNQUALIFIED READING. There is deliberately no clause
		 * of the shape "a pip on any factory retires beat 2"; such a clause would have had to be
		 * deleted by the narrowing rather than merely re-run.
		 *
		 * ---------------------------------------------------------------------------
		 * WITHDRAWN 2026-08-21, AFTER THE NARROWING LANDED. Four claims stood in this block
		 * while it described a tree that no longer exists -- the three the gate named, plus the
		 * both-readings hedge, which the gate did not name and which is stale for the same
		 * reason. They are quoted under the marker
		 * convention this lane declared in `StratSelectionMachineParity.cpp` -- a reader who
		 * remembers them needs to see them retracted, not silently absent -- and nothing that
		 * carries the marker AT THE HEAD OF ITS COMMENT, as this block's `*` lines below do,
		 * asserts anything. A line that merely mentions the token -- this sentence, say -- is
		 * live prose, which is why the convention's sweep filter is anchored.
		 *
		 * RETRACTED> "the unqualified one -- a pip anywhere retires beat 2, which is what
		 * RETRACTED>  `FStratGuidedOpening::HasCapturePipLanded` implements today and says so on
		 * RETRACTED>  its own declaration"
		 *
		 *   False since the narrowing. That declaration now says the opposite, and its .cpp
		 *   carries the third conjunct.
		 *
		 * RETRACTED> "A PIP ON THE RINGED HEX SATISFIES BOTH READINGS, so every schedule clause
		 * RETRACTED>  in this file is correct under the current code AND under the narrowing"
		 *
		 *   The hedge was true when two readings were live. Only one is now, so the sentence
		 *   describes a choice that no longer has to be made. Placing the pip on the ring is
		 *   simply correct rather than defensively compatible.
		 *
		 * RETRACTED> "THE CONVERSE CLAUSE IS NOT HERE AND IS NOT AN OVERSIGHT: 'a pip on a
		 * RETRACTED>  NON-ringed factory leaves beat 2 outstanding' is the clause that pins the
		 * RETRACTED>  ruling, and it is red against this tree."
		 *
		 *   The converse clause IS here: `T-UI-03.Beat2RetiresOnlyOnAPipAtTheRingedObjective`,
		 *   at the bottom of this file. Its PRESENCE is what this retraction rests on, and that
		 *   is checkable by anyone reading the file. Its pass/fail colour is deliberately NOT
		 *   asserted here -- a comment that says "and it is green" is a claim about a suite run,
		 *   which the next commit can falsify without touching this file. The live figure lives
		 *   in `Tools/architect/state/global.md` and nowhere else.
		 *
		 * RETRACTED> "It belongs in the same change as the narrowing, where it can be shown to
		 * RETRACTED>  fail before and pass after."
		 *
		 *   THAT DEMONSTRATION WAS NEVER PERFORMED, and this is the retraction that matters
		 *   most, because the sentence read in a tree where the narrowing HAS landed asserts
		 *   that it was. No run of the converse clause against the pre-narrowing predicate was
		 *   ever executed by anyone. Its being red against that predicate is an INFERENCE from a
		 *   one-line diff and nothing more.
		 *
		 *   WHAT THE CLAUSE HAS INSTEAD is weaker than a fail-before/pass-after run and is
		 *   nonetheless real: it is DIFFERENTIAL. One fixture, one pipped unit, two observations,
		 *   and between them exactly one field changes -- that unit's `Hex`. The two halves fail
		 *   in opposite directions, so a regression to the unqualified reading reddens the
		 *   off-ring half while a fixture that never planted a pip reddens the on-ring half, and
		 *   neither can pass by accident. That is a property of the clause's shape, observable in
		 *   the run that exists; the fail-before/pass-after property would have been a fact about
		 *   history and is not claimed.
		 * ---------------------------------------------------------------------------
		 *
		 * THE HEX IS READ, NEVER SPELLED. `ObjectiveHex` came from
		 * `FStratBridge::GuidedOpeningHexes`, which is a lookup of the authored field.
		 */
		void CapturePipHasLanded()
		{
			for (FStratUnitView& U : Model.Units)
			{
				if (U.UnitId == MarkedUnitId)
				{
					// Standing on the ringed Factory with a pip showing -- §2.11.6's "arrival
					// receipt". The hex is fixture input exactly as `CaptureProgress` is.
					U.Hex             = ObjectiveHex;
					U.CaptureProgress = 1;
					return;
				}
			}
		}

		/** The unit `PlantPipOffTheRing` put a pip on, so the second half can move that one. */
		int32 PipUnitId = INDEX_NONE;

		/**
		 * Fixture input: a capture pip on a guided-seat unit that is NOT standing on the ring.
		 *
		 * THE OFF-RING HALF OF THE DIFFERENTIAL. It deliberately picks a unit OTHER than the
		 * marked Infantry where it can, so that beat 1a's own bookkeeping is not entangled with
		 * beat 2's, and it returns the hex it used so the clause can assert that hex really is
		 * not the objective rather than assuming the shipped deployment.
		 */
		bool PlantPipOffTheRing(FIntPoint& OutHex)
		{
			for (FStratUnitView& U : Model.Units)
			{
				if (U.Side == GuidedSide && U.Hex != ObjectiveHex)
				{
					U.CaptureProgress = 1;
					PipUnitId         = U.UnitId;
					OutHex            = U.Hex;
					return true;
				}
			}
			return false;
		}

		/**
		 * Moves the already-pipped unit onto the ring, and changes NOTHING else.
		 *
		 * THAT IS THE WHOLE VALUE OF THE PAIR. Between the two observations exactly one field
		 * of one unit differs -- `Hex` -- so whatever beat 2 does differently is attributable to
		 * the third conjunct of `HasCapturePipLanded` and to nothing about the fixture.
		 */
		bool MoveThePipOntoTheRing()
		{
			for (FStratUnitView& U : Model.Units)
			{
				if (U.UnitId == PipUnitId)
				{
					U.Hex = ObjectiveHex;
					return true;
				}
			}
			return false;
		}

		/** One observation at a given round. Sets the field the machine reads, then observes. */
		void ObserveAtTurn(int32 Turn)
		{
			Model.Match.Turn = Turn;
			Guidance.Observe(Model, Machine);
		}

		/** Another observation inside the SAME round -- no turn boundary. */
		void ObserveAgain()
		{
			Guidance.Observe(Model, Machine);
		}
	};

	/** The whole of one schedule-table cell, asserted together so a failure names both halves. */
	struct FCell
	{
		int32              Turn = 0;
		EStratGuidanceBeat Beat = EStratGuidanceBeat::None;
		int32              Rule = 0;
	};
}

// ---------------------------------------------------------------------------
// §2.11.6-B's schedule table, COMMON CASE -- the pip lands on turn 2.
//
//   turn 1: 1a, then 1b when 1a retires   (the hand-off happens INSIDE turn 1)
//   turn 2: beat 2, rule 1                 -- and the pip retires it mid-turn
//   turn 3: beat 3, rule 1
//   turn 4: beat 3, rule 2 last call
//
// TURN 1'S HAND-OFF IS TWO OBSERVATIONS AT THE SAME TURN, and that is the mechanism the GDD
// singles out: "A beat gives up the line either the instant it retires (which is how 1a hands
// off to 1b inside turn 1)". A clause that only observed once per turn could not see it.
//
// TURN 4 IS THE CELL THAT DISTINGUISHES THIS IMPLEMENTATION FROM A PLAUSIBLE ONE. Beat 3 is
// the line-holder on both turn 3 and turn 4, so the BEAT is identical; only the RULE changes,
// from 1 to 2. That pair of cells is why `RuleThatSelectedTheLine` was exposed.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceCommonCaseScheduleTest,
	"Stratocracy.StratPlay.T-UI-03.CommonCaseReproducesTheScheduleTable",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceCommonCaseScheduleTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidedOpeningClauses;

	FGuidanceHarness H;
	FString Error;
	if (!TestTrue(TEXT("the guided opening arms against the shipped scenario"),
			H.Arm(/*bSuppressed*/ false, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("and it is active, so the schedule below has a subject"),
			H.Guidance.IsActive()))
	{
		return false;
	}

	// ---- turn 1: 1a ----------------------------------------------------------
	H.ObserveAtTurn(1);
	TestEqual(TEXT("§2.11.6-B turn 1: beat 1a takes the line"),
		BeatName(H.Guidance.BeatOnTheLine()), BeatName(EStratGuidanceBeat::Beat1a));
	TestEqual(TEXT("§2.11.6-B turn 1: by rule 1"), H.Guidance.RuleThatSelectedTheLine(), 1);

	// ---- turn 1, still: 1a retires and 1b inherits INSIDE the turn -----------
	H.MarkedInfantryHasMoved();
	H.ObserveAgain();
	TestFalse(TEXT("beat 1a retired on the marked Infantry's move"),
		H.Guidance.IsBeatOutstanding(EStratGuidanceBeat::Beat1a));
	TestEqual(TEXT("§2.11.6-B turn 1: \"then 1b when 1a retires\" -- inside the same turn"),
		BeatName(H.Guidance.BeatOnTheLine()), BeatName(EStratGuidanceBeat::Beat1b));
	TestEqual(TEXT("§2.11.6-B turn 1: 1b arrives under rule 1, not as a last call"),
		H.Guidance.RuleThatSelectedTheLine(), 1);
	TestEqual(TEXT("and the turn did not advance to get there"), H.Model.Match.Turn, 1);

	// ---- turn 2: beat 2, rule 1 ---------------------------------------------
	H.ObserveAtTurn(2);
	TestFalse(TEXT("beat 1b retired when the round advanced past the one it took the line on"),
		H.Guidance.IsBeatOutstanding(EStratGuidanceBeat::Beat1b));
	TestEqual(TEXT("§2.11.6-B turn 2, common case: beat 2 takes the line"),
		BeatName(H.Guidance.BeatOnTheLine()), BeatName(EStratGuidanceBeat::Beat2));
	TestEqual(TEXT("§2.11.6-B turn 2, common case: by rule 1"),
		H.Guidance.RuleThatSelectedTheLine(), 1);

	// The pip lands inside turn 2, which is what defines this column.
	H.CapturePipHasLanded();
	H.ObserveAgain();
	TestFalse(TEXT("beat 2 retired on the capture pip"),
		H.Guidance.IsBeatOutstanding(EStratGuidanceBeat::Beat2));

	// ---- turn 3: beat 3, rule 1 ---------------------------------------------
	H.ObserveAtTurn(3);
	TestEqual(TEXT("§2.11.6-B turn 3, common case: beat 3 takes the line"),
		BeatName(H.Guidance.BeatOnTheLine()), BeatName(EStratGuidanceBeat::Beat3));
	TestEqual(
		TEXT("§2.11.6-B turn 3, common case: by RULE 1 -- beat 3 had not held the line at a turn "
		     "start before, and inheriting turn 2's tail is not \"its turn\" (§2.11.6 rule 2's "
		     "own wording)"),
		H.Guidance.RuleThatSelectedTheLine(), 1);

	// ---- turn 4: beat 3, rule 2 last call ------------------------------------
	H.ObserveAtTurn(4);
	TestEqual(TEXT("§2.11.6-B turn 4, common case: beat 3 is still the line"),
		BeatName(H.Guidance.BeatOnTheLine()), BeatName(EStratGuidanceBeat::Beat3));
	TestEqual(
		TEXT("§2.11.6-B turn 4, common case: by RULE 2 -- the same beat as turn 3, arrived at a "
		     "different way, which is the cell a beat-only clause cannot see"),
		H.Guidance.RuleThatSelectedTheLine(), 2);

	return true;
}

// ---------------------------------------------------------------------------
// §2.11.6-B's schedule table, WANDERED CASE -- the pip lands on turn 3 or 4, so beat 2 is
// still outstanding when the window's last turn arrives.
//
//   turn 2: beat 2, rule 1  -- "holds, then yields"
//   turn 3: beat 3, rule 1
//   turn 4: beat 2, rule 2 last call
//
// TURN 4 IS THE WHOLE POINT OF THIS COLUMN. Two beats are outstanding and both have already
// had their turn on the line, so rule 2 fires -- and it must pick the LOWEST-numbered
// outstanding beat, which is beat 2 and not beat 3. An implementation that returned the
// highest-numbered, or the most recent, or simply left beat 3 where it was, would reproduce
// the common-case column exactly and fail only here.
//
// NO PIP AND NO SPAWN IS APPLIED ANYWHERE IN THIS CLAUSE, which is what makes it the wandered
// column rather than the common one.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceWanderedCaseScheduleTest,
	"Stratocracy.StratPlay.T-UI-03.WanderedCaseReproducesTheScheduleTable",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceWanderedCaseScheduleTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidedOpeningClauses;

	FGuidanceHarness H;
	FString Error;
	if (!TestTrue(TEXT("the guided opening arms against the shipped scenario"),
			H.Arm(/*bSuppressed*/ false, Error)))
	{
		AddError(Error);
		return false;
	}

	H.ObserveAtTurn(1);
	H.MarkedInfantryHasMoved();
	H.ObserveAgain();
	TestEqual(TEXT("§2.11.6-B turn 1, wandered: 1a, then 1b -- identical to every column"),
		BeatName(H.Guidance.BeatOnTheLine()), BeatName(EStratGuidanceBeat::Beat1b));

	H.ObserveAtTurn(2);
	TestEqual(TEXT("§2.11.6-B turn 2, wandered: beat 2 takes the line"),
		BeatName(H.Guidance.BeatOnTheLine()), BeatName(EStratGuidanceBeat::Beat2));
	TestEqual(TEXT("§2.11.6-B turn 2, wandered: by rule 1"),
		H.Guidance.RuleThatSelectedTheLine(), 1);

	H.ObserveAtTurn(3);
	TestEqual(TEXT("§2.11.6-B turn 3, wandered: beat 2 YIELDS and beat 3 takes the line"),
		BeatName(H.Guidance.BeatOnTheLine()), BeatName(EStratGuidanceBeat::Beat3));
	TestEqual(TEXT("§2.11.6-B turn 3, wandered: by rule 1"),
		H.Guidance.RuleThatSelectedTheLine(), 1);
	TestTrue(
		TEXT("§2.11.6: \"Giving up the line is not retiring\" -- beat 2 is still outstanding"),
		H.Guidance.IsBeatOutstanding(EStratGuidanceBeat::Beat2));

	H.ObserveAtTurn(4);
	TestEqual(
		TEXT("§2.11.6-B turn 4, wandered: rule 2 returns BEAT 2 to the line -- the lowest-numbered "
		     "outstanding beat, not the highest and not the incumbent"),
		BeatName(H.Guidance.BeatOnTheLine()), BeatName(EStratGuidanceBeat::Beat2));
	TestEqual(TEXT("§2.11.6-B turn 4, wandered: as a rule-2 last call"),
		H.Guidance.RuleThatSelectedTheLine(), 2);
	TestTrue(TEXT("and beat 3 is still outstanding, so rule 2 genuinely had a choice to make"),
		H.Guidance.IsBeatOutstanding(EStratGuidanceBeat::Beat3));

	return true;
}

// ---------------------------------------------------------------------------
// §2.11.6-B's schedule table, FAST LANE -- the pip lands inside turn 1, so beat 2 retires
// "without ever holding the line".
//
//   turn 1: 1a, then 1b; beat 2 retires unheld
//   turn 2: beat 3, rule 1
//   turn 3: beat 3, rule 2 last call, UNTAGGED
//   turn 4: beat 3, rule 2 last call again, tagged
//
// THIS COLUMN IS WHERE THE TAG'S KEYING IS OBSERVABLE, and the tag half of it lives in
// `WindowEndTagRendersOnTurnFourOnly` below rather than here, so that a tag regression names
// the tag rather than the schedule.
//
// "WITHOUT EVER HOLDING THE LINE" IS ASSERTED DIRECTLY, because it is the property that makes
// this a different column rather than a faster common case: beat 2 must be retired at the end
// of turn 1 and must never have been `BeatOnTheLine()`.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceFastLaneScheduleTest,
	"Stratocracy.StratPlay.T-UI-03.FastLaneReproducesTheScheduleTable",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceFastLaneScheduleTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidedOpeningClauses;

	FGuidanceHarness H;
	FString Error;
	if (!TestTrue(TEXT("the guided opening arms against the shipped scenario"),
			H.Arm(/*bSuppressed*/ false, Error)))
	{
		AddError(Error);
		return false;
	}

	H.ObserveAtTurn(1);
	TestEqual(TEXT("§2.11.6-B turn 1, fast lane: beat 1a takes the line"),
		BeatName(H.Guidance.BeatOnTheLine()), BeatName(EStratGuidanceBeat::Beat1a));

	// The move and the pip both land inside turn 1 -- that is what the column means.
	H.MarkedInfantryHasMoved();
	H.CapturePipHasLanded();
	H.ObserveAgain();

	TestEqual(TEXT("§2.11.6-B turn 1, fast lane: 1b still inherits the line inside turn 1"),
		BeatName(H.Guidance.BeatOnTheLine()), BeatName(EStratGuidanceBeat::Beat1b));
	TestFalse(
		TEXT("§2.11.6-B turn 1, fast lane: beat 2 retires \"without ever holding the line\""),
		H.Guidance.IsBeatOutstanding(EStratGuidanceBeat::Beat2));

	H.ObserveAtTurn(2);
	TestEqual(TEXT("§2.11.6-B turn 2, fast lane: beat 3 takes the line"),
		BeatName(H.Guidance.BeatOnTheLine()), BeatName(EStratGuidanceBeat::Beat3));
	TestEqual(TEXT("§2.11.6-B turn 2, fast lane: by rule 1"),
		H.Guidance.RuleThatSelectedTheLine(), 1);

	H.ObserveAtTurn(3);
	TestEqual(TEXT("§2.11.6-B turn 3, fast lane: beat 3 again"),
		BeatName(H.Guidance.BeatOnTheLine()), BeatName(EStratGuidanceBeat::Beat3));
	TestEqual(
		TEXT("§2.11.6-B turn 3, fast lane: as a RULE 2 last call -- a rule-2 call before turn 4, "
		     "which is the case the tag must not react to"),
		H.Guidance.RuleThatSelectedTheLine(), 2);

	H.ObserveAtTurn(4);
	TestEqual(TEXT("§2.11.6-B turn 4, fast lane: beat 3 \"again\""),
		BeatName(H.Guidance.BeatOnTheLine()), BeatName(EStratGuidanceBeat::Beat3));
	TestEqual(TEXT("§2.11.6-B turn 4, fast lane: rule 2 once more"),
		H.Guidance.RuleThatSelectedTheLine(), 2);

	return true;
}

// ---------------------------------------------------------------------------
// The window-end tag states a fact about the WINDOW and not about rule 2.
//
// §2.11.6: "The tag states a fact about the *window*, not about rule 2, so it renders on turn
// 4 only: an earlier rule-2 last call (fast lane, turn 3) shows the same line untagged,
// because guidance does not end that turn and the strip must not say it does."
//
// THE FALSE CASE IS THE WHOLE CLAUSE. On the fast lane, turn 3 and turn 4 are both rule-2 last
// calls on beat 3. An implementation that keyed the tag on `LineRule == 2` -- which is the
// obvious reading, and the wrong one -- would render it on BOTH, and a clause that only
// checked turn 4 could not fail. So turn 3 is asserted untagged first, and its rule is
// asserted to be 2 in the same breath, so a future refactor cannot quietly make the turn-3
// case a rule-1 call and leave this clause passing for a reason it does not mean.
//
// THE TAG'S TEXT IS THE MACHINE'S OWN, not a literal here. `FStratGuidedOpening::
// WindowEndTagText()` is the single source of the GDD's `guidance ends this turn`, and the
// clause compares the projected `FStratGuidanceView::WindowEndTagText` against it -- so a
// typo in the string is a `StratGuidedOpening.cpp` review question and not this file's, while
// the tag being on the WRONG LINE is this file's and is what it measures.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceWindowEndTagTest,
	"Stratocracy.StratPlay.T-UI-03.WindowEndTagRendersOnTurnFourOnly",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceWindowEndTagTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidedOpeningClauses;

	FGuidanceHarness H;
	FString Error;
	if (!TestTrue(TEXT("the guided opening arms against the shipped scenario"),
			H.Arm(/*bSuppressed*/ false, Error)))
	{
		AddError(Error);
		return false;
	}

	// The fast lane, because it is the only column with a rule-2 last call before turn 4.
	H.ObserveAtTurn(1);
	H.MarkedInfantryHasMoved();
	H.CapturePipHasLanded();
	H.ObserveAgain();
	H.ObserveAtTurn(2);

	// ---- turn 3: a rule-2 last call, and it must be UNTAGGED -----------------
	H.ObserveAtTurn(3);
	if (!TestEqual(
			TEXT("the fixture really is on a rule-2 last call at turn 3 -- otherwise the false "
			     "case below is not the case the GDD is talking about"),
			H.Guidance.RuleThatSelectedTheLine(), 2))
	{
		return false;
	}

	H.Guidance.DecorateViewModel(H.Model);
	TestTrue(TEXT("the strip is still on screen at turn 3"), H.Model.Guidance.bActive);
	TestFalse(
		TEXT("§2.11.6: a rule-2 last call on TURN 3 is shown UNTAGGED -- the tag is keyed on the "
		     "window, not on the rule, and \"the strip must not say\" guidance ends that turn"),
		H.Model.Guidance.bShowsWindowEndTag);
	TestTrue(TEXT("and the tag string is empty when the tag is not shown"),
		H.Model.Guidance.WindowEndTagText.IsEmpty());

	// ---- turn 4: the same beat, the same rule, and now TAGGED ----------------
	H.ObserveAtTurn(4);
	TestEqual(TEXT("turn 4 is a rule-2 last call too, so the rule did not change"),
		H.Guidance.RuleThatSelectedTheLine(), 2);
	TestEqual(TEXT("and it is the same beat, so the beat did not change either"),
		BeatName(H.Guidance.BeatOnTheLine()), BeatName(EStratGuidanceBeat::Beat3));

	H.Guidance.DecorateViewModel(H.Model);
	TestTrue(
		TEXT("§2.11.6: the tag renders on TURN 4 -- the only thing that changed between these two "
		     "assertions is the turn, which is exactly what the GDD says the tag is about"),
		H.Model.Guidance.bShowsWindowEndTag);
	TestTrue(
		TEXT("and the tag carries FStratGuidedOpening::WindowEndTagText(), the single source of "
		     "the GDD's string"),
		H.Model.Guidance.WindowEndTagText.EqualTo(FStratGuidedOpening::WindowEndTagText()));

	return true;
}

// ---------------------------------------------------------------------------
// `Skip guidance` clears the strip, the ring and the locks TOGETHER.
//
// §2.11.6: it "kills it instantly for anyone, and kills the guided opening's board state with
// it -- the objective ring and the turn-1a unit marker clear in the same frame as the strip."
//
// "IN THE SAME FRAME" IS MEASURED AS "ON ONE MODEL". The clause takes ONE `FStratViewModel`
// after the skip and requires every guided element on it to be off at once: `bActive` false,
// `bHasObjectiveRing` false, the beat `None`, the directive empty. A staged clean-up -- ring
// this frame, strip the next -- would have to leave one of them true on that single model.
//
// THE LOCKS ARE THE PART THAT COULD PLAUSIBLY LAG. `SkipGuidance` deliberately does NOT clear
// them; `Observe` does, on the next refresh, "from the same single place that sets them". So
// the clause requires a lock to EXIST first -- otherwise it would pass on a machine that never
// locked anything -- and then requires every one to be gone after the skip's `Observe`.
//
// AND IT IS PERMANENT. §2.11.6 says "gone for good"; a further observation at a later turn
// must not bring the strip back, which is what a `bActive` recomputed from the turn each
// observation would do.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceSkipClearsEverythingTest,
	"Stratocracy.StratPlay.T-UI-03.SkipGuidanceClearsStripRingAndMarkerTogether",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceSkipClearsEverythingTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidedOpeningClauses;

	FGuidanceHarness H;
	FString Error;
	if (!TestTrue(TEXT("the guided opening arms against the shipped scenario"),
			H.Arm(/*bSuppressed*/ false, Error)))
	{
		AddError(Error);
		return false;
	}

	H.ObserveAtTurn(1);
	H.Guidance.DecorateViewModel(H.Model);

	// ---- the control: everything the skip must clear is ON first -------------
	if (!TestTrue(TEXT("the strip is on screen before the skip"), H.Model.Guidance.bActive))
	{
		return false;
	}
	if (!TestTrue(TEXT("the objective ring is on screen before the skip"),
			H.Model.Guidance.bHasObjectiveRing))
	{
		return false;
	}

	int32 LocksBefore = 0;
	for (const FStratUnitView& U : H.Model.Units)
	{
		LocksBefore += H.Machine.IsLockedThisTurn(U.UnitId) ? 1 : 0;
	}
	if (!TestTrue(
			TEXT("at least one unit is locked before the skip -- otherwise \"every lock cleared\" "
			     "below would pass on a machine that never locked anything"),
			LocksBefore > 0))
	{
		return false;
	}

	// ---- the skip, and the one refresh the controller performs with it -------
	H.Guidance.SkipGuidance();
	H.ObserveAgain();
	H.Guidance.DecorateViewModel(H.Model);

	TestFalse(TEXT("§2.11.6: the strip is gone"), H.Model.Guidance.bActive);
	TestFalse(TEXT("§2.11.6: the objective ring clears in the same frame as the strip"),
		H.Model.Guidance.bHasObjectiveRing);
	TestEqual(TEXT("§2.11.6: and no beat holds a line that does not exist"),
		BeatName(H.Model.Guidance.Beat), BeatName(EStratGuidanceBeat::None));
	TestTrue(TEXT("§2.11.6: the directive text is empty, not a stale line"),
		H.Model.Guidance.DirectiveText.IsEmpty());
	TestFalse(TEXT("§2.11.6: and the End Turn gate is open again"),
		H.Model.Guidance.bEndTurnGated);

	for (const FStratUnitView& U : H.Model.Units)
	{
		TestFalse(
			*FString::Printf(TEXT("§2.11.6: unit %d's lock cleared with the strip"), U.UnitId),
			H.Machine.IsLockedThisTurn(U.UnitId));
	}

	// ---- and it does not come back -------------------------------------------
	H.ObserveAtTurn(2);
	H.Guidance.DecorateViewModel(H.Model);
	TestFalse(TEXT("§2.11.6: \"gone for good\" -- a later turn does not revive the strip"),
		H.Model.Guidance.bActive);
	TestFalse(TEXT("nor the ring"), H.Model.Guidance.bHasObjectiveRing);

	return true;
}

// ---------------------------------------------------------------------------
// §2.11.6: "any completed match on the save skips all guidance automatically."
//
// THE SUPPRESSION IS AN ARGUMENT AND NOT A DISK READ, deliberately -- `FStratGuidedOpening`
// "owns no disk", and `AStratPlayerController::TryArmGuidedOpening` is where
// `UStratMatchSubsystem::HasCompletedAMatchOnSave` is asked. So the subject here is what the
// machine does with the answer, and the slot half is
// `T-SAVE-06.SaveOverASlotPreservesOnboardingState` in `StratSaveSlotClauses.cpp`.
//
// THE CONTROL IS IN THE SAME CLAUSE, and without it the clause proves nothing: an
// arming that failed for an unrelated reason -- a scenario that named no guided opening, say
// -- would produce exactly the same inactive machine. So the identical fixture is armed
// UNSUPPRESSED first and required to be active, and only then armed suppressed and required
// not to be. The single difference between the two runs is the bool.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceSuppressedByCompletedMatchTest,
	"Stratocracy.StratPlay.T-UI-03.GuidanceIsSuppressedByACompletedMatchOnTheSave",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceSuppressedByCompletedMatchTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidedOpeningClauses;

	// ---- the control: the same fixture, unsuppressed, runs -------------------
	{
		FGuidanceHarness Control;
		FString Error;
		if (!TestTrue(TEXT("the control fixture arms"), Control.Arm(/*bSuppressed*/ false, Error)))
		{
			AddError(Error);
			return false;
		}
		Control.ObserveAtTurn(1);
		Control.Guidance.DecorateViewModel(Control.Model);

		if (!TestTrue(
				TEXT("CONTROL: with suppression off, this exact fixture DOES run a guided opening "
				     "-- so the absence below is attributable to the bool and to nothing else"),
				Control.Model.Guidance.bActive))
		{
			return false;
		}
	}

	// ---- the case: suppressed --------------------------------------------------
	FGuidanceHarness H;
	FString Error;
	if (!TestTrue(TEXT("the suppressed fixture arms"), H.Arm(/*bSuppressed*/ true, Error)))
	{
		AddError(Error);
		return false;
	}

	TestFalse(
		TEXT("§2.11.6: a completed match on the save skips all guidance -- the machine is not "
		     "active from the moment it is armed"),
		H.Guidance.IsActive());

	H.ObserveAtTurn(1);
	H.Guidance.DecorateViewModel(H.Model);

	TestFalse(TEXT("§2.11.6: no strip"), H.Model.Guidance.bActive);
	TestEqual(TEXT("§2.11.6: no beat"),
		BeatName(H.Model.Guidance.Beat), BeatName(EStratGuidanceBeat::None));
	TestFalse(TEXT("§2.11.6: no objective ring"), H.Model.Guidance.bHasObjectiveRing);
	TestFalse(TEXT("§2.11.6: and the End Turn gate never closes"),
		H.Model.Guidance.bEndTurnGated);
	TestFalse(TEXT("§2.11.6: the Q27 End Turn gate agrees at the machine, not only on the model"),
		H.Guidance.IsEndTurnGated());
	TestFalse(TEXT("§2.11.6: and the marked Infantry's input is not gated either"),
		H.Guidance.IsUnitInputGated(H.MarkedUnitId));

	for (const FStratUnitView& U : H.Model.Units)
	{
		TestFalse(
			*FString::Printf(TEXT("§2.11.6: unit %d is not locked under a suppressed opening"),
				U.UnitId),
			H.Machine.IsLockedThisTurn(U.UnitId));
	}

	return true;
}

// ---------------------------------------------------------------------------
// Beat 2 retires ONLY on a capture pip at the RINGED objective -- the ruling of 2026-08-21,
// made into a gate.
//
// WHAT WAS RULED, AND WHY IT NEEDED A CLAUSE AT ALL. §2.11.6-B's "Retires when" cell is
// unqualified -- "A capture pip appears -- on whatever turn that happens" -- while the
// directive text names the ring. `FStratGuidedOpening::HasCapturePipLanded` first shipped
// taking the unqualified reading and said so on its own declaration. The narrow reading is the
// ruling, and the third conjunct of that predicate (`U.Hex == Objective`) is the whole of the
// difference between the two. A one-line conjunct is exactly the kind of thing a later tidy-up
// removes, so it gets a clause whose only subject is that conjunct.
//
// THE FORM IS DIFFERENTIAL, AND THAT IS THE POINT RATHER THAN A CONVENIENCE. One fixture, one
// pipped unit, two observations, and between them exactly one field changes: that unit's `Hex`.
//   - OFF THE RING: beat 2 must stay OUTSTANDING and keep the line.
//   - ON THE RING: the same pip, the same unit, the same turn -- beat 2 must RETIRE.
// The two halves fail in OPPOSITE DIRECTIONS, and that is what makes the pair worth more than
// either half alone: a regression to the unqualified reading reddens the off-ring half, while a
// fixture that never managed to plant a pip at all -- a wrong side, a stale objective, a model
// the machine is not reading -- reddens the on-ring half. Neither can pass by accident, and
// there is no state of the world in which both pass for the wrong reason.
//
// NO BEFORE/AFTER RUN IS CLAIMED HERE. The off-ring half is red-by-construction against the
// pre-narrowing predicate, but that is an INFERENCE from a one-line diff and no such run was
// executed by anyone. This block records what the clause's shape buys, which is measurable, and
// not a history it did not observe.
//
// THE OBJECTIVE IS READ, NEVER SPELLED. `FGuidanceHarness::ObjectiveHex` came from
// `FStratBridge::GuidedOpeningHexes`, which is a lookup of `guidedOpening.objective` -- and
// §2.11.6 forbids the alternative by name. The clause additionally ASSERTS that the off-ring
// hex it used differs from it, rather than trusting the shipped deployment to keep them apart.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceBeat2RingOnlyTest,
	"Stratocracy.StratPlay.T-UI-03.Beat2RetiresOnlyOnAPipAtTheRingedObjective",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceBeat2RingOnlyTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidedOpeningClauses;

	FGuidanceHarness H;
	FString Error;
	if (!TestTrue(TEXT("the guided opening arms against the shipped scenario"),
			H.Arm(/*bSuppressed*/ false, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the scenario rings an objective, which is this clause's whole subject"),
			H.Guidance.HasObjective()))
	{
		return false;
	}
	TestTrue(
		*FString::Printf(
			TEXT("the machine's ringed hex is the one FStratBridge::GuidedOpeningHexes answered "
			     "with, axial %s"),
			*H.ObjectiveHex.ToString()),
		H.Guidance.ObjectiveHex() == H.ObjectiveHex);

	// ---- get to turn 2, where beat 2 holds the line --------------------------
	H.ObserveAtTurn(1);
	H.MarkedInfantryHasMoved();
	H.ObserveAgain();
	H.ObserveAtTurn(2);

	if (!TestEqual(TEXT("beat 2 holds the line on turn 2, so there is something to retire"),
			BeatName(H.Guidance.BeatOnTheLine()), BeatName(EStratGuidanceBeat::Beat2)))
	{
		return false;
	}

	// ---- half one: a pip somewhere that is NOT the ring ----------------------
	FIntPoint OffRingHex = FIntPoint::ZeroValue;
	if (!TestTrue(TEXT("a guided-seat unit is standing somewhere other than the ring"),
			H.PlantPipOffTheRing(OffRingHex)))
	{
		return false;
	}
	if (!TestTrue(
			*FString::Printf(
				TEXT("and the hex the pip was planted on, axial %s, really is NOT the ringed "
				     "objective, axial %s -- asserted rather than assumed of the shipped "
				     "deployment"),
				*OffRingHex.ToString(), *H.ObjectiveHex.ToString()),
			OffRingHex != H.ObjectiveHex))
	{
		return false;
	}

	H.ObserveAgain();

	TestTrue(
		*FString::Printf(
			TEXT("§2.11.6 as ruled: a capture pip at axial %s -- NOT the ringed Factory -- leaves "
			     "beat 2 OUTSTANDING. This is the half a regression to the unqualified \"a pip "
			     "anywhere\" reading turns red"),
			*OffRingHex.ToString()),
		H.Guidance.IsBeatOutstanding(EStratGuidanceBeat::Beat2));
	TestEqual(
		TEXT("and it keeps the line -- an outstanding beat that had silently lost the strip would "
		     "be a different defect wearing the same pass"),
		BeatName(H.Guidance.BeatOnTheLine()), BeatName(EStratGuidanceBeat::Beat2));

	// ---- half two: the same pip, moved onto the ring -------------------------
	// ONE FIELD CHANGES. Same unit, same `CaptureProgress`, same turn, same observation call.
	if (!TestTrue(TEXT("the pipped unit moves onto the ringed Factory"),
			H.MoveThePipOntoTheRing()))
	{
		return false;
	}
	H.ObserveAgain();

	TestFalse(
		*FString::Printf(
			TEXT("§2.11.6 as ruled: the SAME pip on the SAME unit RETIRES beat 2 once that unit "
			     "stands on the ringed Factory at axial %s. This is the half a fixture that never "
			     "planted a pip at all turns red"),
			*H.ObjectiveHex.ToString()),
		H.Guidance.IsBeatOutstanding(EStratGuidanceBeat::Beat2));

	// The strip does not go blank on a mid-turn retirement -- §2.11.6: "Rule 2 has no exit".
	TestEqual(
		TEXT("and the line passes on rather than emptying, since beat 3 is still outstanding"),
		BeatName(H.Guidance.BeatOnTheLine()), BeatName(EStratGuidanceBeat::Beat3));

	return true;
}
