// The gate on `StratDecoratePathPreview` and `FStratBridgePathQuery` -- GDD §2.11.1's path
// preview at the DECORATION seam and at the QUERY seam. T-UI-02 throughout, by the 2026-08-29
// W8 ruling that binds item (1) to that ID, and the same ID
// `Source/StratPlay/Tests/StratInputClaimClauses.cpp` in this directory already carries. No
// identifier is minted here.
//
// STRETCH DECLARED ONCE, HERE AND AGAIN ON EACH CLAUSE. T-UI-02's own sentence is about the
// displayed set BEING the module's set, and only one clause in this file touches that --
// `PathQueryForwardsTheBridgesAnswer`, which is the seam where a module answer enters this
// module. `Source/StratBridge/Tests/StratPathPreviewParity.cpp` asserts the equality itself,
// against `strat::findPath` and `strat::uiReachable`, because StratBridge is the only module
// that can call both. What the other four clauses pin is the four things the DECORATOR decides
// that are nobody else's: WHEN there is a preview, WHICH hex it is about, THAT the model's
// field is a complete statement on every path, and THAT every number is copied rather than
// formed. Each is a property §2.11.1 needs; none is a claim T-UI-02's sentence makes.
//
// NOTHING IN THE FIRST FOUR CLAUSES CONSTRUCTS A BRIDGE, A WORLD OR AN ACTOR. (The fifth
// builds a seeded bridge and the sixth a live world, a match and a controller; both say so.)
// `IStratPathQuery` is an interface for exactly this reason -- its own declaration says a gate
// must be able to drive the decorator over routes the one shipped scenario does not contain.
// The double below is a RECORDER as much as an answerer: what the decorator ASKED, and whether
// it asked AT ALL, is the subject of two clauses, and a production query would swallow the
// question and hand back only an answer.
//
// WHERE THE EXPECTATIONS COME FROM.
//   - Every "no preview" is a FIELD-BY-FIELD comparison against a default-constructed
//     `FStratPathPreviewView`, never against typed zeroes.
//   - Every "about hex X" is compared against the hex handed to
//     `FStratHoverState::SetHoveredHex` -- the production hover struct, driven, not a value
//     written into the model by hand.
//   - Every published number is compared against THE VALUE THE QUERY HANDED BACK, held in the
//     double. The decorator's whole contract is that it copies, so the double's own payload IS
//     the expectation and nothing here re-forms it.
//   - The move gate's clause compares nothing numeric at all: what it asserts is that the
//     QUERY WAS NOT ASKED, which is the only way to tell the decorator's own gate from an
//     answer the rules module gave.
//   - `PathQueryForwardsTheBridgesAnswer` takes its expectation from
//     `FStratBridge::MovePathToHex` on the same seeded bridge, in the same run.
// Not one hex coordinate, cost or total from the scenario file or the GDD is written here.
//
// AN OFFSET PAIR AND AN AXIAL PAIR PRINT IDENTICALLY on even rows. The hexes below are chosen
// with ODD rows and with distinct X AND Y, so a coordinate confusion in either direction is
// visible in the failure text rather than hidden by a coincidence.
//
// WHAT THIS FILE DOES NOT PIN, stated rather than left to be discovered:
//   - That the route is the cheapest one, that its ticks are §2.5's, or that the total is
//     `findPath`'s. All three are `Source/StratBridge/Tests/StratPathPreviewParity.cpp`'s, for
//     the link reason that file's header gives.
//   - Nothing about the call site any more. THE CLAIM THAT STOOD HERE UNTIL 2026-08-31 WAS
//     FALSE AND IS WITHDRAWN IN FULL rather than deleted, because the species -- a limitation
//     asserted without being measured -- is one this project has paid for before. It said:
//
//     STRUCK> "`AStratPlayerController::DecorateForPresentation`'s call site. The controller
//     STRUCK>  reaches the decorator through a `FStratBridgePathQuery` it constructs itself,
//     STRUCK>  with no seam a fixture can substitute, so 'the decorator is reached at all' is
//     STRUCK>  not observable from here the way `StratInfoPanelRouting.cpp` observes the info
//     STRUCK>  panel's route."
//
//     WHY IT WAS WRONG, as the measurement that refutes it rather than as an apology.
//     `DecorateForPresentation` is PUBLIC (the enclosing specifier is `StratPlayerController.h`'s
//     `public:`) and NON-VIRTUAL, and two test files in this very directory already call it on a
//     spawned controller -- `StratHoverInputClauses.cpp` at five call sites and
//     `StratInfoPanelRouting.cpp` at four. No seam is needed and none was added: a fixture
//     drives the SHIPPED path and the controller builds its own `FStratBridgePathQuery` over a
//     real seeded bridge, which is stronger than a substitutable query rather than weaker. "The
//     query cannot be substituted" was true and was mistaken for "the call site cannot be
//     observed", which is a different sentence.
//     `DecorateForPresentationPublishesThePreview` below is that clause.
//   - That a mouse move produces a hover. Nothing in this suite drives `UPlayerInput`.
//   - Any widget, overlay, spline or tick actor. Nothing here is on screen.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Math/IntPoint.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

#include "StratBoardActor.h"
#include "StratHoverState.h"
#include "StratMatchSubsystem.h"
#include "StratPathPreviewQuery.h"
#include "StratPlayerController.h"
#include "StratSelectionMachine.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

#include "StratBridge.h"

namespace StratPathPreviewDecoratorClauses
{
	/** The selected unit's id in every fixture below. Arbitrary and distinct from every other
	 *  id used here, so a decorator that picked the wrong unit is visible. */
	static const int32 kMoverId = 7;

	/** A second unit the model lists and nothing selects. */
	static const int32 kBystanderId = 9;

	/** Two hexes with ODD rows and distinct components -- see the header block on why. */
	static const FIntPoint kHexA{2, 7};
	static const FIntPoint kHexB{4, 5};

	/** The side of the scenario that moves first, matching every other fixture in this tree. */
	static const int32 kFirstSide = 0;

	/**
	 * The query double: it ANSWERS, and it RECORDS what it was asked.
	 *
	 * `mutable` on the recording members because `PathTo` is `const` on the interface, and it
	 * is const for a good reason -- the production implementation borrows a bridge and caches
	 * nothing. A double that forced the interface to widen would be a test changing the code
	 * under test.
	 */
	struct FRecordingPathQuery final : public IStratPathQuery
	{
		/** What to hand back on the answer channel. */
		TArray<FIntPoint> AnswerHexes;
		TArray<int32>     AnswerCosts;
		int32             AnswerTotal = 0;

		/** false makes the query REFUSE -- the "no bridge" channel, not the no-route one. */
		bool    bAnswer = true;
		FString Refusal = TEXT("the double refused");

		mutable int32             Calls = 0;
		mutable TArray<int32>     AskedUnits;
		mutable TArray<FIntPoint> AskedHexes;

		virtual bool PathTo(int32 UnitId, FIntPoint GoalHex,
		                    TArray<FIntPoint>& OutRouteHexes,
		                    TArray<int32>&     OutRouteCosts,
		                    int32&             OutTotalCost,
		                    FString&           OutFailureReason) const override
		{
			++Calls;
			AskedUnits.Add(UnitId);
			AskedHexes.Add(GoalHex);

			OutRouteHexes.Reset();
			OutRouteCosts.Reset();
			OutTotalCost = 0;

			if (!bAnswer)
			{
				OutFailureReason = Refusal;
				return false;
			}

			OutRouteHexes = AnswerHexes;
			OutRouteCosts = AnswerCosts;
			OutTotalCost  = AnswerTotal;
			return true;
		}
	};

	/**
	 * A route the double can hand back.
	 *
	 * ITS THREE NUMBERS ARE MUTUALLY INCONSISTENT ON PURPOSE, and that is the instrument for
	 * the copy clause rather than an oversight. `AnswerTotal` is 41, which is neither the last
	 * tick (7) nor the sum of the ticks (10). The bridge would never produce such a triple --
	 * its own cross-check refuses when the total and the last tick disagree -- but the
	 * DECORATOR is below that check and its contract is that it COPIES. A decorator that
	 * assigned `RouteCosts.Last()` or summed the ticks reddens here and would be invisible to
	 * a fixture whose numbers agreed.
	 */
	static void FillAnswer(FRecordingPathQuery& Query)
	{
		Query.AnswerHexes = { FIntPoint(2, 7), FIntPoint(3, 7), FIntPoint(3, 9) };
		Query.AnswerCosts = { 0, 3, 7 };
		Query.AnswerTotal = 41;
	}

	/** A model with two units on it, the mover unspent. */
	static FStratViewModel ModelWithUnits()
	{
		FStratViewModel Model;

		FStratUnitView Mover;
		Mover.UnitId    = kMoverId;
		Mover.Side      = 0;
		Mover.Hp        = 10;
		Mover.HpMax     = 10;
		Mover.bHasMoved = false;
		Mover.bHasActed = false;
		Mover.bDone     = false;
		Model.Units.Add(Mover);

		FStratUnitView Bystander;
		Bystander.UnitId    = kBystanderId;
		Bystander.Side      = 1;
		Bystander.Hp        = 10;
		Bystander.HpMax     = 10;
		Bystander.bHasMoved = false;
		Bystander.bHasActed = false;
		Bystander.bDone     = false;
		Model.Units.Add(Bystander);

		return Model;
	}

	/**
	 * A preview on the MODEL that is loud on every field.
	 *
	 * THIS IS THE WHOLE INSTRUMENT for the unconditional-write clause, and the reason is
	 * `a-default-read-cannot-tell-silence-from-an-empty-call`: an unconditional writer called
	 * with a default payload reads EXACTLY like a writer that was never called, if the field it
	 * writes started at its default. Starting it loud makes the two distinguishable -- a
	 * decorator that returned early without writing leaves this standing, and every one of its
	 * five fields differs from the default it must be replaced by.
	 */
	static FStratPathPreviewView LoudStandingPreview()
	{
		FStratPathPreviewView View;
		View.bHasPath  = true;
		View.RouteHexes = { FIntPoint(91, 92), FIntPoint(93, 94) };
		View.RouteCosts = { 95, 96 };
		View.TotalCost  = 97;
		View.UnitId     = 4242;
		return View;
	}

	static bool IsFieldwiseEqual(const FStratPathPreviewView& A, const FStratPathPreviewView& B)
	{
		return A.bHasPath   == B.bHasPath   &&
		       A.RouteHexes == B.RouteHexes &&
		       A.RouteCosts == B.RouteCosts &&
		       A.TotalCost  == B.TotalCost  &&
		       A.UnitId     == B.UnitId;
	}

	static FString Describe(const FIntPoint& P)
	{
		return FString::Printf(TEXT("hex (q %d, r %d)"), P.X, P.Y);
	}

	/** The seeded opening, on `StratInfoPanelRouting.cpp`'s pattern in this same directory. */
	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		UDataTable* const Units   = LoadObject<UDataTable>(nullptr, TEXT("/Game/StratData/DT_Units.DT_Units"));
		UDataTable* const Terrain = LoadObject<UDataTable>(nullptr, TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
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
		const FString ScenarioPath =
			FPaths::Combine(FPaths::Combine(FPaths::ProjectDir(), TEXT("Data")), TEXT("ferrum_crossing.json"));
		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath, kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	/**
	 * The match configuration, on `StratAiMatchClauses.cpp`'s `MakeConfig` -- the shipped
	 * tables and the shipped scenario, so nothing here authors a board.
	 */
	static bool MakeConfig(FStratMatchConfig& Out, FString& OutError)
	{
		Out.UnitTable    = LoadObject<UDataTable>(nullptr, TEXT("/Game/StratData/DT_Units.DT_Units"));
		Out.TerrainTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
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

	/** A transient game world, on `StratInfoPanelRouting.cpp`'s pattern in this directory. */
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
}

// ---------------------------------------------------------------------------
// T-UI-02 -- the decorator writes `PathPreview` on EVERY path, including every path that
// produces no route.
//
// STRETCH DECLARED: this is `StratDecoratePathPreview`'s own unconditional-write contract,
// which §2.11.1 needs and which T-UI-02's sentence does not state.
//
// WHAT WOULD GO WRONG WITHOUT IT, in the declaration's own words: a decorator that wrote only
// on the route path "would leave a previous frame's route standing in any model built by a
// caller that never hovered -- an AI turn's reconcile, a hand-over, a gate -- and the preview
// would hang on screen pointing at a hex the cursor left, for a unit that is no longer
// selected".
//
// HOW THIS CLAUSE CAN TELL A WRITE FROM A SILENCE, which is the trap
// `a-default-read-cannot-tell-silence-from-an-empty-call` records: the model's `PathPreview` is
// PRE-POPULATED with `LoudStandingPreview()` before every call. A decorator that returned early
// without writing leaves that preview standing, and every one of its five fields differs from
// the default the contract requires. A fixture that started the field at its default would
// report green over a decorator that had been deleted.
//
// FIVE PATHS, WHICH IS EXACTLY THE FIVE THE DECLARATION ENUMERATES. Nothing is selected; the
// cursor is over no hex; the selected unit has already moved; the module answers that there is
// no route; and the query refused. The fourth and fifth are the two that look alike from
// outside and are different channels -- the fourth is `true` with an empty array and leaves
// `OutFailureReason` untouched, the fifth is `false` with a reason -- and both are asserted to
// clear the field.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratPathPreviewWritesUnconditionallyTest,
	"Stratocracy.StratPlay.T-UI-02.PathPreviewWritesUnconditionally",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratPathPreviewWritesUnconditionallyTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPathPreviewDecoratorClauses;

	const FStratPathPreviewView Default;

	// The instrument's own control, before it is used as evidence: the standing preview must
	// actually differ from the default, or every assertion below is satisfied by nothing.
	if (!TestFalse(TEXT("control: the pre-populated standing preview differs from a default one, "
	                    "so 'cleared' and 'never written' are distinguishable"),
			IsFieldwiseEqual(LoudStandingPreview(), Default)))
	{
		return false;
	}

	struct FPath
	{
		const TCHAR* What;
		int32        Selection;
		bool         bHovering;
		bool         bHasMoved;
		bool         bQueryHasRoute;
		bool         bQueryAnswers;
	};

	const FPath Paths[] =
	{
		{ TEXT("nothing is selected"),                 INDEX_NONE, true,  false, true,  true  },
		{ TEXT("the cursor is over no hex"),           kMoverId,   false, false, true,  true  },
		{ TEXT("the selected unit has already moved"), kMoverId,   true,  true,  true,  true  },
		{ TEXT("the module answers that there is no route"), kMoverId, true, false, false, true },
		{ TEXT("the query refuses outright"),          kMoverId,   true,  false, true,  false },
	};

	for (const FPath& Path : Paths)
	{
		FStratViewModel Model = ModelWithUnits();
		Model.Units[0].bHasMoved = Path.bHasMoved;

		if (Path.bHovering)
		{
			FStratHoverState Hover;
			Hover.SetHoveredHex(kHexA);
			Hover.DecorateViewModel(Model);
		}

		// PRE-POPULATED. See the block above.
		Model.PathPreview = LoudStandingPreview();

		FRecordingPathQuery Query;
		if (Path.bQueryHasRoute)
		{
			FillAnswer(Query);
		}
		Query.bAnswer = Path.bQueryAnswers;

		FString Reason;
		const bool bPreview = StratDecoratePathPreview(Model, Path.Selection, Query, Reason);

		if (!TestFalse(*FString::Printf(TEXT("no preview is produced when %s"), Path.What), bPreview))
		{
			return false;
		}

		// ---- THE CLAUSE -----------------------------------------------------
		if (!IsFieldwiseEqual(Model.PathPreview, Default))
		{
			AddError(FString::Printf(
				TEXT("T-UI-02: when %s, the model's PathPreview was not cleared to a default "
				     "(bHasPath=%d hexes=%d costs=%d total=%d unit=%d)"),
				Path.What, Model.PathPreview.bHasPath ? 1 : 0,
				Model.PathPreview.RouteHexes.Num(), Model.PathPreview.RouteCosts.Num(),
				Model.PathPreview.TotalCost, Model.PathPreview.UnitId));
			return false;
		}
	}

	// ---- and the positive path, so the clause is not satisfied by a decorator that clears
	// ---- the field and never publishes a route at all -----------------------------
	{
		FStratViewModel Model = ModelWithUnits();
		FStratHoverState Hover;
		Hover.SetHoveredHex(kHexA);
		Hover.DecorateViewModel(Model);
		Model.PathPreview = LoudStandingPreview();

		FRecordingPathQuery Query;
		FillAnswer(Query);

		FString Reason;
		const bool bPreview = StratDecoratePathPreview(Model, kMoverId, Query, Reason);

		TestTrue(TEXT("control: a selected, unmoved unit over a hex with a route DOES produce a "
		              "preview"), bPreview);
		TestTrue(TEXT("control: and the model says so"), Model.PathPreview.bHasPath);
		TestFalse(TEXT("control: which is not the standing preview it replaced"),
			IsFieldwiseEqual(Model.PathPreview, LoudStandingPreview()));
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-02 -- the decorator asks about the HOVERED hex and the SELECTED unit, so running it
// before the hover composes a STALE goal.
//
// STRETCH DECLARED: this is the ordering constraint `StratDecoratePathPreview`'s declaration
// states, and §2.11.1 needs it because the route must be to the hex the cursor is on THIS
// frame. T-UI-02's sentence claims nothing about ordering.
//
// THE FAILURE THIS PINS IS INVISIBLE TO EVERY OTHER KIND OF TEST. A decorator run before the
// hover produces a perfectly well-formed route to last frame's hex. Every hex on it is real,
// every tick is the module's, nothing refuses and nothing logs -- the bridge clauses all stay
// green. It reads on screen as a latency bug and it is a sequencing one. This clause is what
// makes the constraint checkable rather than merely written down on three declarations.
//
// THE HOVER IS DRIVEN THROUGH THE PRODUCTION STRUCT and not written into the model by hand, so
// the clause pins the real seam. Two hexes with odd rows and distinct components -- an
// offset/axial confusion would be visible either way.
//
// AND THE UNIT IS ASSERTED TOO, in both places it appears: what the decorator ASKED the query
// about, and what it PUBLISHED as `PathPreview::UnitId`. The declaration says that id is
// carried "so a consumer never has to ask the selection machine which unit a drawn route is
// about", and a decorator that published the wrong one would draw a correct route attributed
// to a unit that is not moving.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratPathPreviewGoalIsTheHoveredHexTest,
	"Stratocracy.StratPlay.T-UI-02.PathPreviewGoalIsTheHoveredHex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratPathPreviewGoalIsTheHoveredHexTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPathPreviewDecoratorClauses;

	FStratViewModel  Model = ModelWithUnits();
	FStratHoverState Hover;

	FRecordingPathQuery Query;
	FillAnswer(Query);

	// ---- frame one: the cursor is on A, and the model has been told ----------
	Hover.SetHoveredHex(kHexA);
	Hover.DecorateViewModel(Model);

	FString Reason;
	TestTrue(TEXT("control: the first frame produces a preview"),
		StratDecoratePathPreview(Model, kMoverId, Query, Reason));

	if (!TestEqual(TEXT("control: the query was asked exactly once"), Query.Calls, 1))
	{
		return false;
	}

	// COMPONENTWISE, because `TestEqual` has no `FIntPoint` overload and because a failure that
	// prints both components is the only kind an offset/axial confusion is legible in.
	TestEqual(TEXT("T-UI-02: the goal asked about is the hovered hex's X"),
		Query.AskedHexes[0].X, kHexA.X);
	TestEqual(TEXT("T-UI-02: and the hovered hex's Y"),
		Query.AskedHexes[0].Y, kHexA.Y);
	TestEqual(TEXT("T-UI-02: and the unit asked about is the selected one"),
		Query.AskedUnits[0], kMoverId);
	TestEqual(TEXT("T-UI-02: and the published route is attributed to that same unit"),
		Model.PathPreview.UnitId, kMoverId);

	// ---- frame two: the cursor MOVES to B, and the hover has not decorated yet ----
	Hover.SetHoveredHex(kHexB);

	TestTrue(TEXT("control: the second frame still produces a preview"),
		StratDecoratePathPreview(Model, kMoverId, Query, Reason));
	if (!TestEqual(TEXT("control: the query has now been asked twice"), Query.Calls, 2))
	{
		return false;
	}

	// ---- THE CLAUSE, first direction: run BEFORE the hover, the goal is STALE ----
	// This is not the desired behaviour; it is the CONSEQUENCE that makes the ordering
	// constraint real. A decorator that read the hover from somewhere other than the model
	// would answer about B here, and the constraint recorded on three declarations would be
	// describing a dependency that does not exist.
	TestEqual(*FString::Printf(
			TEXT("T-UI-02: a decorator run BEFORE the hover decorator composes LAST frame's goal "
			     "(X) -- %s, not %s -- which is why the ordering constraint is real"),
			*Describe(kHexA), *Describe(kHexB)),
		Query.AskedHexes[1].X, kHexA.X);
	TestEqual(TEXT("T-UI-02: and LAST frame's goal (Y)"),
		Query.AskedHexes[1].Y, kHexA.Y);

	// ---- THE CLAUSE, second direction: run AFTER the hover, the goal is current ----
	Hover.DecorateViewModel(Model);

	TestTrue(TEXT("control: the third call still produces a preview"),
		StratDecoratePathPreview(Model, kMoverId, Query, Reason));
	if (!TestEqual(TEXT("control: the query has now been asked three times"), Query.Calls, 3))
	{
		return false;
	}

	TestEqual(TEXT("T-UI-02: run AFTER the hover, the goal is THIS frame's hex (X)"),
		Query.AskedHexes[2].X, kHexB.X);
	TestEqual(TEXT("T-UI-02: and THIS frame's hex (Y)"),
		Query.AskedHexes[2].Y, kHexB.Y);

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-02 -- the move gate is `bHasMoved` AND NOT `bDone`, and an already-moved unit is
// suppressed BEFORE the query is asked.
//
// STRETCH DECLARED: which bit gates the preview is `StratDecoratePathPreview`'s own decision
// and §2.11.1's need. T-UI-02's sentence says nothing about the turn.
//
// TWO PROPERTIES, AND THE SECOND IS THE ONE WITH TEETH.
//
// (1) AN ALREADY-MOVED UNIT GETS NO PREVIEW, AND THE QUERY IS NEVER ASKED. The negative
//     control matters and is the whole reason this is asserted on the double's `Calls` rather
//     than on the model: `strat::reachable` and `strat::findPath` do not consult `hasMoved` --
//     they answer about §2.5's cost model and not about the turn -- so a bridge asked about a
//     spent unit answers a perfectly good route. A clause that only checked the model would be
//     satisfied by a decorator that asked, got a route, and then discarded it, which is a
//     different program with a different cost. `Calls` is the only instrument that separates
//     them.
//
// (2) IT IS THE RULES-SIDE BIT AND NOT THE PRESENTATION ONE. `FStratUnitView` carries both:
//     `bHasMoved` mirrors `UiUnitView::hasMoved` and `bDone` is the selection machine's own
//     presentation bit, and the declaration says a unit can reach `bDone` with its move
//     unspent. A decorator gated on `bDone` would hide a preview for a move §2.5 permits, and
//     it would be INVISIBLE to every fixture in which the two bits happen to agree. So the
//     four combinations are driven, and the discriminating one is `bDone` TRUE with
//     `bHasMoved` FALSE: the shipped gate publishes a preview there and a `bDone` gate does
//     not. Without that leg this clause would be green over the wrong bit.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratPathPreviewGateIsHasMovedNotDoneTest,
	"Stratocracy.StratPlay.T-UI-02.PathPreviewGateIsHasMovedNotDone",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratPathPreviewGateIsHasMovedNotDoneTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPathPreviewDecoratorClauses;

	struct FCase
	{
		const TCHAR* What;
		bool         bHasMoved;
		bool         bDone;
		bool         bExpectPreview;
		const TCHAR* Why;
	};

	const FCase Cases[] =
	{
		{ TEXT("unspent and not done"),      false, false, true,
		  TEXT("the ordinary case") },
		{ TEXT("has moved, not done"),       true,  false, false,
		  TEXT("the RULES bit is set, so the selection machine would refuse the move") },
		{ TEXT("done but has NOT moved"),    false, true,  true,
		  TEXT("THE DISCRIMINATING LEG -- a bDone gate would wrongly suppress here, and §2.5 "
		       "permits this move") },
		{ TEXT("has moved and is done"),     true,  true,  false,
		  TEXT("both bits agree, which is the case that proves nothing on its own") },
	};

	for (const FCase& Case : Cases)
	{
		FStratViewModel Model = ModelWithUnits();
		Model.Units[0].bHasMoved = Case.bHasMoved;
		Model.Units[0].bDone     = Case.bDone;

		FStratHoverState Hover;
		Hover.SetHoveredHex(kHexA);
		Hover.DecorateViewModel(Model);

		FRecordingPathQuery Query;
		FillAnswer(Query);

		FString    Reason;
		const bool bPreview = StratDecoratePathPreview(Model, kMoverId, Query, Reason);

		// ---- THE CLAUSE ---------------------------------------------------------
		// `TestTrue` over an explicit comparison rather than `TestEqual`, because the
		// expectation is a `bool` and this file does not rely on a `bool` overload existing.
		TestTrue(*FString::Printf(TEXT("T-UI-02: %s -> preview is %s (%s)"),
				Case.What, Case.bExpectPreview ? TEXT("published") : TEXT("suppressed"), Case.Why),
			bPreview == Case.bExpectPreview);
		TestTrue(*FString::Printf(TEXT("T-UI-02: %s -> and the model agrees"), Case.What),
			Model.PathPreview.bHasPath == Case.bExpectPreview);

		// ---- THE NEGATIVE CONTROL, which is the half a model-only assertion misses ----
		if (Case.bHasMoved)
		{
			TestEqual(*FString::Printf(
					TEXT("T-UI-02: %s -> the query was NOT ASKED AT ALL, so the suppression is "
					     "this decorator's gate and not a rules answer it discarded"), Case.What),
				Query.Calls, 0);
		}
		else
		{
			TestEqual(*FString::Printf(
					TEXT("control: %s -> the query WAS asked, so `Calls == 0` above means "
					     "something"), Case.What),
				Query.Calls, 1);
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-02 -- every number the decorator publishes is the query's, COPIED. It forms none.
//
// STRETCH DECLARED: this is `StratDecoratePathPreview`'s and `FStratPathPreviewView`'s own
// no-arithmetic contract, which `StratViewModel.h`'s standing census depends on. T-UI-02's
// sentence is about the SET, not about the arithmetic.
//
// THIS IS THE CLAUSE THAT GOES RED THE MOMENT ANYONE SUMS THE TICKS, and it is the reason the
// double's payload is deliberately inconsistent. `FillAnswer` hands back ticks `{0, 3, 7}` and
// a total of `41`. Forty-one is neither the last tick nor the sum, so:
//   - `TotalCost = RouteCosts.Last()`   publishes 7  -> RED
//   - `TotalCost = Sum(RouteCosts)`     publishes 10 -> RED
//   - `TotalCost = <the query's scalar>` publishes 41 -> green, which is the contract.
// The bridge would never produce such a triple -- its own cross-check refuses when the total
// and the last tick disagree -- and that is precisely why a fixture built out of BRIDGE output
// could not pin this: on any board the bridge will answer, the three candidate implementations
// return the same number. The decorator sits below that check and its contract is to copy, so
// the double is entitled to hand it a triple the bridge would have refused. Declared here
// rather than left for a reader to trip over.
//
// AND THE ARRAYS ARE COMPARED ELEMENT FOR ELEMENT, IN ORDER, against the double's own payload.
// A decorator that reversed the route, dropped the start hex to "draw from the next step", or
// re-sorted the ticks would satisfy any length-only assertion.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratPathPreviewCopiesTheQueryAnswerTest,
	"Stratocracy.StratPlay.T-UI-02.PathPreviewCopiesTheQueryAnswer",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratPathPreviewCopiesTheQueryAnswerTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPathPreviewDecoratorClauses;

	FStratViewModel Model = ModelWithUnits();

	FStratHoverState Hover;
	Hover.SetHoveredHex(kHexA);
	Hover.DecorateViewModel(Model);

	FRecordingPathQuery Query;
	FillAnswer(Query);

	// The instrument's own control, before it is used as evidence. If the payload's total ever
	// became the last tick or the sum, every assertion below would be satisfied by a decorator
	// that computed it, and this clause would silently stop pinning anything.
	{
		int32 Sum = 0;
		for (const int32 Tick : Query.AnswerCosts)
		{
			Sum += Tick;
		}
		// `TestTrue` over an explicit `!=` rather than `TestNotEqual`, which this engine
		// declares only for the string-ish types (`AutomationTest.h` has no `int32` overload
		// of it, though it has one of `TestEqual`).
		if (!TestTrue(FString::Printf(
				TEXT("control: the double's total (%d) is NOT the last tick (%d), so a decorator "
				     "that assigned the last tick reddens"),
				Query.AnswerTotal, Query.AnswerCosts.Last()),
				Query.AnswerTotal != Query.AnswerCosts.Last()))
		{
			return false;
		}
		if (!TestTrue(FString::Printf(
				TEXT("control: and it is NOT the sum of the ticks (%d), so a decorator that "
				     "summed them reddens"), Sum),
				Query.AnswerTotal != Sum))
		{
			return false;
		}
	}

	FString Reason;
	if (!TestTrue(TEXT("control: the decorator publishes a preview"),
			StratDecoratePathPreview(Model, kMoverId, Query, Reason)))
	{
		return false;
	}

	const FStratPathPreviewView& Published = Model.PathPreview;

	// ---- THE CLAUSE -------------------------------------------------------------
	TestTrue(TEXT("T-UI-02: the model says there is a path"), Published.bHasPath);

	if (TestEqual(TEXT("T-UI-02: every route hex the query gave is published, none added or dropped"),
			Published.RouteHexes.Num(), Query.AnswerHexes.Num()))
	{
		for (int32 i = 0; i < Published.RouteHexes.Num(); ++i)
		{
			TestEqual(*FString::Printf(TEXT("T-UI-02: route hex %d is the query's, in its order (X)"), i),
				Published.RouteHexes[i].X, Query.AnswerHexes[i].X);
			TestEqual(*FString::Printf(TEXT("T-UI-02: route hex %d is the query's, in its order (Y)"), i),
				Published.RouteHexes[i].Y, Query.AnswerHexes[i].Y);
		}
	}

	if (TestEqual(TEXT("T-UI-02: there is exactly one published tick per published hex"),
			Published.RouteCosts.Num(), Query.AnswerCosts.Num()))
	{
		for (int32 i = 0; i < Published.RouteCosts.Num(); ++i)
		{
			TestEqual(*FString::Printf(
					TEXT("T-UI-02: tick %d is the query's, neither re-summed nor differenced"), i),
				Published.RouteCosts[i], Query.AnswerCosts[i]);
		}
	}

	TestEqual(TEXT("T-UI-02: the total is the query's own scalar -- NOT the last tick and NOT "
	               "the sum of the ticks"),
		Published.TotalCost, Query.AnswerTotal);

	TestEqual(TEXT("T-UI-02: and the route is attributed to the unit that was asked about"),
		Published.UnitId, kMoverId);

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-02 -- the production query IS `FStratBridge::MovePathToHex`, forwarded, and its two
// channels survive the forwarding.
//
// THIS IS THE ONLY CLAUSE IN THIS FILE ON T-UI-02'S OWN SENTENCE, and it is the seam where a
// module answer enters this module. Everything above drives a double, so all four clauses stay
// green over a `FStratBridgePathQuery` that invented its own routes -- the decorator would be
// correct and the screen would be wrong. `FStratBridgePathQuery`'s own declaration says it is a
// named type rather than a lambda at the call site precisely so "a gate can construct one
// against a seeded bridge and assert that what it hands back IS the bridge's own answer".
// This is that gate.
//
// WHERE THE EXPECTATION COMES FROM. `FStratBridge::MovePathToHex` on the same seeded bridge, in
// the same run, for the same unit and goal. The forwarding is what is under test, so the thing
// forwarded is the expectation and nothing here re-derives a route. Whether the BRIDGE'S answer
// is the module's is `Source/StratBridge/Tests/StratPathPreviewParity.cpp`'s subject and is not
// re-asserted here.
//
// THE TWO CHANNELS ARE PINNED SEPARATELY BECAUSE THE FORWARD COULD COLLAPSE THEM. An
// unreachable goal must arrive as `true` WITH AN EMPTY ROUTE, not as `false` -- the declaration
// says a caller "cannot tell 'there is no bridge' from 'there is no route' if both come back
// the same way", and a forward that returned `Asked.bOk && !OutRouteHexes.IsEmpty()` would look
// entirely reasonable and would destroy that distinction. Both directions are driven: a goal
// the module calls reachable, and every hex of the board that it does not.
//
// AND A NULL BRIDGE IS A REFUSAL WITH A REASON, which is the ordinary state before `StartMatch`
// finishes and is the third channel this seam has to keep separate from the other two.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratPathQueryForwardsTheBridgesAnswerTest,
	"Stratocracy.StratPlay.T-UI-02.PathQueryForwardsTheBridgesAnswer",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratPathQueryForwardsTheBridgesAnswerTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPathPreviewDecoratorClauses;

	// ---- the no-bridge channel, which needs no scenario -------------------------
	{
		const FStratBridgePathQuery Query(nullptr);

		TArray<FIntPoint> RouteHexes = { FIntPoint(91, 92) };
		TArray<int32>     RouteCosts = { 93 };
		int32             TotalCost  = 94;
		FString           Reason;

		const bool bAnswered = Query.PathTo(kMoverId, kHexA, RouteHexes, RouteCosts, TotalCost, Reason);

		TestFalse(TEXT("T-UI-02: with no bridge the query REFUSES -- it does not answer that "
		               "there is no route, which a caller could not tell from a real board fact"),
			bAnswered);
		TestTrue(TEXT("T-UI-02: and it says why"), !Reason.IsEmpty());
	}

	// ---- the two real channels, against a seeded bridge -------------------------
	FStratBridge Bridge;
	FString      Error;
	const bool   bSeeded = SeedBridge(Bridge, Error);
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), bSeeded))
	{
		AddError(Error);
		return false;
	}

	const FStratBridgePathQuery Query(&Bridge);

	// The units and their reachable goals come from the bridge, not from this file.
	TArray<FIntPoint> AllHexes;
	TArray<int32>     AllCosts;

	int32 RoutesForwarded = 0;
	int32 EmptiesForwarded = 0;
	int32 UnitsSeen        = 0;

	// The ids are discovered by asking: an id the bridge refuses is not a unit, and the first
	// contiguous run of ids it answers for is the board's. Nothing here types a unit id.
	for (int32 UnitId = 0; UnitId < 64; ++UnitId)
	{
		const FStratResult Exists = Bridge.ReachableHexes(UnitId, AllHexes, AllCosts);
		if (!Exists.bOk)
		{
			continue;
		}
		++UnitsSeen;

		for (const FIntPoint& Goal : AllHexes)
		{
			// ---- THE EXPECTATION: the bridge's own answer, asked directly ------
			TArray<FIntPoint> BridgeHexes;
			TArray<int32>     BridgeCosts;
			int32             BridgeTotal = 0;
			const FStratResult FromBridge =
				Bridge.MovePathToHex(UnitId, Goal, BridgeHexes, BridgeCosts, BridgeTotal);

			// ---- WHAT THE QUERY HANDS BACK -------------------------------------
			TArray<FIntPoint> QueryHexes;
			TArray<int32>     QueryCosts;
			int32             QueryTotal = 0;
			FString           Reason;
			const bool bAnswered =
				Query.PathTo(UnitId, Goal, QueryHexes, QueryCosts, QueryTotal, Reason);

			const FString At = FString::Printf(TEXT("unit %d -> %s"), UnitId, *Describe(Goal));

			TestTrue(*FString::Printf(TEXT("%s: the query's channel is the bridge's"), *At),
				bAnswered == FromBridge.bOk);
			TestTrue(*FString::Printf(TEXT("%s: the route is the bridge's, hex for hex and in "
			                               "its order"), *At),
				QueryHexes == BridgeHexes);
			TestTrue(*FString::Printf(TEXT("%s: the ticks are the bridge's"), *At),
				QueryCosts == BridgeCosts);
			TestEqual(*FString::Printf(TEXT("%s: the total is the bridge's"), *At),
				QueryTotal, BridgeTotal);

			if (QueryHexes.Num() > 0)
			{
				++RoutesForwarded;
			}
		}

		// ---- the no-route channel: every hex of the board this unit cannot reach ----
		// Derived rather than chosen: any hex NOT in `ReachableHexes`' set is one the module
		// answers "no route" for, and a forward that turned that into `false` reddens here.
		{
			TArray<FIntPoint> BridgeHexes;
			TArray<int32>     BridgeCosts;
			int32             BridgeTotal = 0;

			// A goal well off the board, derived from nothing this file typed as a board
			// coordinate: it is the reachable set's own extent pushed far past itself.
			int32 FarQ = 0;
			int32 FarR = 0;
			for (const FIntPoint& H : AllHexes)
			{
				FarQ = FMath::Max(FarQ, H.X);
				FarR = FMath::Max(FarR, H.Y);
			}
			const FIntPoint OffBoard(FarQ + 256, FarR + 256);

			const FStratResult FromBridge =
				Bridge.MovePathToHex(UnitId, OffBoard, BridgeHexes, BridgeCosts, BridgeTotal);

			TArray<FIntPoint> QueryHexes = { FIntPoint(91, 92) };
			TArray<int32>     QueryCosts = { 93 };
			int32             QueryTotal = 94;
			FString           Reason;
			const bool bAnswered =
				Query.PathTo(UnitId, OffBoard, QueryHexes, QueryCosts, QueryTotal, Reason);

			if (TestTrue(TEXT("control: the bridge answers 'no route' for a goal off the board"),
					FromBridge.bOk && BridgeHexes.Num() == 0))
			{
				TestTrue(TEXT("T-UI-02: 'there is no route' arrives through the query as an "
				              "ANSWER (true) and not as a refusal -- the two channels survive "
				              "the forwarding"),
					bAnswered);
				TestEqual(TEXT("T-UI-02: with an empty route, which is how the caller reads it"),
					QueryHexes.Num(), 0);
				TestEqual(TEXT("T-UI-02: and empty ticks"), QueryCosts.Num(), 0);
				TestEqual(TEXT("T-UI-02: and a zero total, which is not a sentinel"),
					QueryTotal, 0);
				++EmptiesForwarded;
			}
		}
	}

	TestTrue(TEXT("control: the sweep found units on the seeded board"), UnitsSeen > 0);
	TestTrue(TEXT("control: and forwarded a non-trivial number of real routes"),
		RoutesForwarded > 50);
	TestTrue(TEXT("control: and exercised the no-route channel at least once"),
		EmptiesForwarded > 0);

	AddInfo(FString::Printf(
		TEXT("T-UI-02: %d routes and %d no-route answers forwarded across %d units"),
		RoutesForwarded, EmptiesForwarded, UnitsSeen));

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-02 -- `AStratPlayerController::DecorateForPresentation` PUBLISHES the preview, and what
// it publishes is the real bridge's own answer.
//
// THE GAP THIS CLOSES, AND IT IS THE ONE THAT READS AS A BUILT FEATURE WHEN IT IS ABSENT.
// Every clause above drives `StratDecoratePathPreview` directly. All five stay green over a
// `DecorateForPresentation` that never calls it -- the decorator would be correct,
// clause-pinned and UNREACHABLE, and the screen would show no preview at all while the suite
// reported the feature done. This project has already paid for that exact shape once, recorded
// as "check the ROUTE, not the mechanism". `StratInfoPanelRouting.cpp` carries the same clause
// for §2.11.2's panel and gives the same reason.
//
// NO SEAM WAS ADDED AND NONE IS NEEDED. `DecorateForPresentation` is public and non-virtual and
// this directory already calls it on a spawned controller in two other files. The controller
// constructs its own `FStratBridgePathQuery` over the match subsystem's bridge, and that is a
// STRENGTH here rather than the obstacle the withdrawn header note called it: the fixture
// cannot substitute the query, so what this clause observes is the shipped path end to end --
// a real `UStratMatchSubsystem`, a real `FStratBridge`, a real `strat::findPath`.
//
// WHERE THE EXPECTATION COMES FROM. `FStratBridge::MovePathToHex` on THE SAME BRIDGE OBJECT the
// controller reached, via `UStratMatchSubsystem::GetBridge()`, in the same run. Not a second
// bridge seeded alongside, which could drift; the identical pointer. The unit, its hex and the
// goal all come off the subsystem's own applied model and the bridge's own reachable set --
// no hex, id or cost is written down in this clause.
//
// THE SELECTION AND THE HOVER ARE BOTH DRIVEN THROUGH THE CONTROLLER'S OWN PUBLIC SEAMS --
// `HandleSelectionEvent(HexPrimary, ...)` and `SetHoveredHex` / `ClearHoveredHex`. Neither
// `Model.Hover` nor the selection machine is written by hand, so an ordering or plumbing defect
// anywhere along the shipped path is in scope rather than routed around.
//
// AND THE CLEARED-HOVER ARM USES A LOUD STANDING PREVIEW, on the unconditional-write clause's
// instrument and for its reason: with the hover cleared, a controller that never calls the
// decorator and one that calls it and clears the field produce the SAME EMPTY MODEL if the
// field started empty. Pre-populating it separates them, and it is what makes this arm a
// second, independent witness that the call site exists.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratDecorateForPresentationPublishesThePreviewTest,
	"Stratocracy.StratPlay.T-UI-02.DecorateForPresentationPublishesThePreview",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratDecorateForPresentationPublishesThePreviewTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPathPreviewDecoratorClauses;

	// This fixture has no Blueprint defaults, so the board actor reports every terrain kind as
	// unmeshed. DECLARED EXPECTED RATHER THAN SUPPRESSED, on `StratAiMatchClauses.cpp`'s
	// precedent and for its reason: the unmeshed terrain is the board actor's report and not a
	// complaint about the match, which stays live. `Occurrences = 0` means "one or more".
	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
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
	if (!TestTrue(TEXT("the match config assembles from the shipped tables and scenario"),
			MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	FString StartReason;
	Subsystem->StartMatch(Config, StartReason);
	if (!TestTrue(TEXT("the match is live"), Subsystem->IsMatchLive()))
	{
		AddError(StartReason);
		return false;
	}

	// THE BRIDGE THE CONTROLLER WILL REACH. Not a second one seeded beside it -- this is the
	// object `AStratPlayerController::GetMatch()->GetBridge()` returns, and every expectation
	// below is asked of it.
	FStratBridge* const Bridge = Subsystem->GetBridge();
	if (!TestNotNull(TEXT("the live match owns a seeded bridge"), Bridge))
	{
		return false;
	}

	AStratPlayerController* const Controller = Scope.World->SpawnActor<AStratPlayerController>();
	if (!TestNotNull(TEXT("AStratPlayerController spawned"), Controller))
	{
		return false;
	}

	// ---- the subject: a unit of the viewing side that has not moved -------------
	// Taken off the subsystem's own applied model, so nothing here decides which unit.
	if (!TestTrue(TEXT("the applied model carries units"),
			Subsystem->GetViewModel().Units.Num() > 0))
	{
		return false;
	}

	// THE SUBJECT IS FOUND BY ASKING THE CONTROLLER, NOT BY PREDICTING IT, and that is a
	// correction rather than a style choice. The first version of this clause took the first
	// unmoved unit of the viewing side and it went RED with the selection machine's own
	// message, "unit 1 is locked this turn" -- Ferrum Crossing ships a §2.11.6 guided opening
	// whose beat locks every unit but the one it marks, so "unmoved" and "selectable" are
	// different predicates on this board and only the machine knows which. Rather than
	// restate the guided opening's rule here -- a rule this file does not hold, and the same
	// substitution T-UI-02 is about -- the loop below DRIVES the shipped selection seam and
	// keeps the first unit the controller actually accepts AND that the bridge answers a
	// multi-hex route for. Both halves are asked of the code under test.
	int32     MoverId  = INDEX_NONE;
	FIntPoint MoverHex = FIntPoint::ZeroValue;
	TArray<FIntPoint> ExpectedHexes;
	TArray<int32>     ExpectedCosts;
	int32             ExpectedTotal = 0;
	FIntPoint         GoalHex       = FIntPoint::ZeroValue;

	{
		TArray<int32>     CandidateIds;
		TArray<FIntPoint> CandidateHexes;
		for (const FStratUnitView& U : Subsystem->GetViewModel().Units)
		{
			if (U.Side == Config.ViewingSide && !U.bHasMoved)
			{
				CandidateIds.Add(U.UnitId);
				CandidateHexes.Add(U.Hex);
			}
		}
		if (!TestTrue(TEXT("the opening has unmoved units on the viewing side"),
				CandidateIds.Num() > 0))
		{
			return false;
		}

		for (int32 c = 0; c < CandidateIds.Num() && MoverId == INDEX_NONE; ++c)
		{
			// CANCELLED FIRST, EVERY TIME. `HandleSelectionEvent` is stateful and a primary
			// click means different things depending on what is already selected -- a second
			// click on a selected unit's own hex DESELECTS it, which is how the first version
			// of this loop left the machine at `-1` on a unit it had just selected
			// successfully. Starting each attempt from no selection makes each iteration a
			// clean question rather than a function of the previous one.
			FString CancelReason;
			Controller->HandleSelectionEvent(EStratSelectionEvent::Cancel,
			                                 FIntPoint::ZeroValue, CancelReason);

			FString SelectReason;
			Controller->HandleSelectionEvent(EStratSelectionEvent::HexPrimary,
			                                 CandidateHexes[c], SelectReason);
			if (Controller->GetSelectionMachine().GetSelectedUnitId() != CandidateIds[c])
			{
				// Locked by the guided opening, or otherwise refused. The machine's own call.
				continue;
			}

			// It selects. Does the bridge answer a route worth drawing?
			TArray<FIntPoint> ReachHexes;
			TArray<int32>     ReachCosts;
			if (!Bridge->ReachableHexes(CandidateIds[c], ReachHexes, ReachCosts).bOk)
			{
				continue;
			}

			for (const FIntPoint& H : ReachHexes)
			{
				if (H == CandidateHexes[c])
				{
					continue;
				}
				TArray<FIntPoint> TryHexes;
				TArray<int32>     TryCosts;
				int32             TryTotal = 0;
				const FStratResult Try =
					Bridge->MovePathToHex(CandidateIds[c], H, TryHexes, TryCosts, TryTotal);
				if (Try.bOk && TryHexes.Num() > 1)
				{
					MoverId       = CandidateIds[c];
					MoverHex      = CandidateHexes[c];
					GoalHex       = H;
					ExpectedHexes = TryHexes;
					ExpectedCosts = TryCosts;
					ExpectedTotal = TryTotal;
				}
			}
		}

		if (!TestTrue(TEXT("the opening offers a unit the controller will select AND that the "
		                   "bridge answers a multi-hex route for -- the guided opening locks "
		                   "most units on turn 1, so this is asked of the machine, not assumed"),
				MoverId != INDEX_NONE))
		{
			return false;
		}
	}

	// ---- drive the SHIPPED path: re-select, hover, decorate ---------------------
	// Re-selected rather than relying on the search loop's last state: the loop may have tried
	// -- and been refused for -- a later candidate after finding this one, and a clause that
	// depended on the machine's leftover state would be asserting about the search.
	{
		FString CancelReason;
		Controller->HandleSelectionEvent(EStratSelectionEvent::Cancel,
		                                 FIntPoint::ZeroValue, CancelReason);

		FString SelectReason;
		Controller->HandleSelectionEvent(EStratSelectionEvent::HexPrimary, MoverHex, SelectReason);
		if (!TestEqual(TEXT("control: clicking the unit's hex selects it through the "
		                    "controller's own seam"),
				Controller->GetSelectionMachine().GetSelectedUnitId(), MoverId))
		{
			AddError(SelectReason);
			return false;
		}
	}

	Controller->SetHoveredHex(GoalHex);

	FStratViewModel Model = Subsystem->GetViewModel();
	Model.PathPreview = LoudStandingPreview();   // so a silent controller is distinguishable
	Controller->DecorateForPresentation(Model);

	// ---- THE CLAUSE -------------------------------------------------------------
	// NARROWED MESSAGE, AND THE NARROWING IS THE POINT. This assertion shows only that a
	// preview STANDS on the model after decorating -- and on its own it would pass over a
	// controller that never called the decorator at all, because the model was seeded with
	// `LoudStandingPreview()` a few lines above and that preview's `bHasPath` is already true.
	// It is a gate on the assertions below, not evidence for the clause's own name. What
	// establishes that the call site exists is the route/tick/total comparison that follows
	// (the loud standing route is not the bridge's) together with the cleared-hover arm at the
	// end (the loud preview must have been REPLACED by a default). The message said this one
	// assertion proved "the decorator is reached from the shipped path at all"; it does not,
	// and overclaiming in a failure string is how a reader mislocates a defect.
	if (!TestTrue(TEXT("T-UI-02: a path preview stands on the model after "
	                   "DecorateForPresentation -- the gate on the comparisons below, which "
	                   "are what show it is the bridge's own answer"),
			Model.PathPreview.bHasPath))
	{
		AddError(TEXT("no preview stands on the decorated model at all"));
		return false;
	}

	TestEqual(TEXT("T-UI-02: and it is attributed to the selected unit"),
		Model.PathPreview.UnitId, MoverId);

	if (TestEqual(TEXT("T-UI-02: the published route is the bridge's own, hex for hex"),
			Model.PathPreview.RouteHexes.Num(), ExpectedHexes.Num()))
	{
		for (int32 i = 0; i < ExpectedHexes.Num(); ++i)
		{
			TestEqual(*FString::Printf(TEXT("T-UI-02: published route hex %d (X)"), i),
				Model.PathPreview.RouteHexes[i].X, ExpectedHexes[i].X);
			TestEqual(*FString::Printf(TEXT("T-UI-02: published route hex %d (Y)"), i),
				Model.PathPreview.RouteHexes[i].Y, ExpectedHexes[i].Y);
		}
	}

	if (TestEqual(TEXT("T-UI-02: and one published tick per published hex"),
			Model.PathPreview.RouteCosts.Num(), ExpectedCosts.Num()))
	{
		for (int32 i = 0; i < ExpectedCosts.Num(); ++i)
		{
			TestEqual(*FString::Printf(TEXT("T-UI-02: published tick %d is the bridge's"), i),
				Model.PathPreview.RouteCosts[i], ExpectedCosts[i]);
		}
	}

	TestEqual(TEXT("T-UI-02: and the published total is the bridge's"),
		Model.PathPreview.TotalCost, ExpectedTotal);

	// ---- the second witness: clearing the controller's hover clears the preview ----
	// A controller that never called the decorator would leave the LOUD standing preview here.
	{
		Controller->ClearHoveredHex();

		FStratViewModel Cleared = Subsystem->GetViewModel();
		Cleared.PathPreview = LoudStandingPreview();
		Controller->DecorateForPresentation(Cleared);

		const FStratPathPreviewView Default;
		TestFalse(TEXT("T-UI-02: with the controller's hover cleared, no preview is published"),
			Cleared.PathPreview.bHasPath);
		TestTrue(TEXT("T-UI-02: and the field was CLEARED to its default rather than left "
		             "carrying the loud standing preview -- which is how this arm tells a "
		             "controller that cleared it from one that never called the decorator"),
			IsFieldwiseEqual(Cleared.PathPreview, Default));
	}

	AddInfo(FString::Printf(
		TEXT("T-UI-02: unit %d published a %d-hex route to (q %d, r %d) at total %d through "
		     "DecorateForPresentation"),
		MoverId, Model.PathPreview.RouteHexes.Num(), GoalHex.X, GoalHex.Y, ExpectedTotal));

	return true;
}
