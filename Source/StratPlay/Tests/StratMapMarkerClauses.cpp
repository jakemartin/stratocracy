// The gate for §2.11.2's TWO on-map markers on `AStratUnitActor` -- the flag `H` marker and
// the unacted pip -- added 2026-08-29 alongside the components themselves.
//
// THE IDs THIS FILE CLAIMS, AND THE ONE IT DOES NOT DISCHARGE.
//
//   T-INT-05        the screen is rebuildable FROM THE VIEW MODEL ALONE. No actor holds a
//                   bit the model does not, and no frame is correct only because the
//                   previous one was.
//
//                   THIS FILE'S T-INT-05 CLAUSE ASSERTS ONE ACTOR'S SHARE OF THAT CLAIM AND
//                   NOT THE CLAIM. T-INT-05 is about the WHOLE screen -- board, units, HUD,
//                   panel, every widget -- and what
//                   `MarkersRebuildFromTheViewModelAlone` measures is that the two NEW
//                   marker components on `AStratUnitActor` are a function of the last
//                   applied model alone. That is a stretch of the ID onto new surface, not
//                   a discharge of it: `StratMatchReconcile.cpp` holds the actor-set and
//                   presentation-bit clauses, `StratViewModelParity.cpp` holds the
//                   projection's, and this file adds the marker components. W4 leaves
//                   T-INT-05 exactly as open as it found it, one surface narrower.
//
//   GATE-MAPMARKERS is a LOCAL GATE NAME. It mints no acceptance ID and claims none. It
//                   groups the two clauses that pin what each marker's visibility is a
//                   function OF, which is a §2.11.2 row and not an acceptance criterion.
//
// THE TWO ROWS THIS FILE IS ACCOUNTABLE TO, quoted from §2.11.2's table:
//
//   | Flag `H` marker (both sides, always visible) | What to protect, what to hunt | ... |
//   | Unacted pip on own units | Which units I can still give an order to | §2.1 per-unit
//     loop, via the DONE bit of §2.11.1's machine, carried in the view-model's presentation
//     block |
//
// "BOTH SIDES" IS THE WHOLE OF THE FLAG CLAUSE'S TEETH. `GuidedMarker` -- the other marker
// on this actor -- carries a `View.Side == ViewingSide` filter that a 2026-08-23 user ruling
// put there, and the obvious way to build a second marker is to copy the first. Sec 2.11.2
// forecloses that for the flag in as many words, so the decisive control below is a model in
// which `bIsFlag` and `Side != ViewingSide` COINCIDE. Without that one unit, every assertion
// in the clause would also pass a flag marker that had inherited the guided marker's side
// filter, because the viewing seat's own flag looks identical under both rules.
//
// AND THE PIP'S TEETH ARE A PAIR WITH IDENTICAL TURN FLAGS. §2.11.2's row binds the pip to
// "the DONE bit of §2.11.1's machine", and §2.11.1 rules that DONE "is not the act flag" and
// that "Space (Wait) and RMB/Esc in MOVED both reach DONE without acting, so the two bits
// come apart in ordinary play". A clause that drove only a waited unit would be satisfied by
// `!bHasActed`, which is a real and plausible wrong answer -- so the pip clause drives two
// units of the viewing seat with IDENTICAL `bHasMoved`/`bHasActed` and OPPOSITE `bDone`. No
// function whatever of the two turn flags can answer both, because its inputs are equal and
// its required outputs are not. This is `StratInfoPanelClauses.cpp`'s CLAUSE 6 construction,
// applied one layer down to a component instead of a panel field.
//
// WHERE THE EXPECTATIONS COME FROM.
//
//   - THE FLAG BIT IS A MODULE-SIDE VALUE AND IS READ, NEVER WRITTEN. `StratBuildViewModel`
//     sets `FStratUnitView::bIsFlag` from the rules snapshot's `UiUnitView::isFlag`
//     (`StratViewModel.cpp`), and `StratViewModelParity.cpp` already pins that mirror by
//     name. So the flag clause's expectation on every actor is the APPLIED MODEL'S OWN
//     `bIsFlag`, and the shipped scenario supplies the decisive control for free: it
//     deploys exactly one flag per side, so with the viewing seat at side 0 the side-1 flag
//     unit IS the `bIsFlag && Side != ViewingSide` case, produced by the rules module and
//     not decorated by this file.
//
//   - THE DONE BIT HAS NO MODULE-SIDE VALUE AND THE LITERAL IS UNAVOIDABLE. `Ui.h`'s
//     presentation block "has no module-side counterpart and no derivation from one", the
//     GDD states "the rules module has no DONE bit and no Wait command; no snapshot field
//     mirrors this bit and none is asked for", and `StratBuildViewModel` leaves it at its
//     default -- which `StratMatchReconcile.cpp`'s presentation-bit clause asserts rather
//     than assumes. Its producer is `FStratSelectionMachine::DecorateViewModel`, whose own
//     parity is `StratSelectionMachineParity.cpp`'s. So in the pip clause `bDone` is the
//     INPUT this file writes, and what is asserted is the SELECTION -- which bits on the
//     unit reach the component -- and nothing about what the bit means.
//
//   - NO MARKER RULE IS RESTATED AS AN EXPRESSION. Nowhere below does an assertion read
//     `Actor->IsUnactedPipVisible() == (!View.bDone && View.Side == Seat)`. That would be
//     the implementation copied into its own gate, green over any edit that changed both
//     copies and green over no edit at all. Each case is named, its inputs stated, and its
//     required outcome written as the §2.11.2 row's own word.
//
// WHAT THIS FILE DOES NOT PIN, AND CANNOT.
//
//   - IT PINS FLAGS AND NOT PIXELS. `IsFlagMarkerVisible` and `IsUnactedPipVisible` read
//     `USceneComponent::IsVisible` on the component, and that function NEVER CONSULTS THE
//     STATIC MESH -- the measurement `StratGuidedOpeningVisuals.cpp` recorded against
//     `IsGuidedMarkerVisible` and which both new accessors inherit by construction. A marker
//     with `FlagMarkerMesh` unset answers TRUE here and draws nothing on screen. Every
//     assertion below is therefore about a visibility flag, and NO CLAUSE IN THIS FILE
//     CLAIMS A PLAYER CAN SEE ANYTHING. The mesh assignment is the content lane's and the
//     gate for it is not a headless one.
//
//   - IT PINS NEITHER MARKER'S PLACEMENT. `FlagMarkerOffset` and `UnactedPipOffset` are
//     `EditDefaultsOnly` presentation values whose own declarations record that their
//     magnitudes are derived from `SM_GuidedMarker`'s 100 uu and are not pinned. Nothing
//     below reads a transform.
//
//   - IT WRITES NO MIRROR CLAUSE. `bIsFlag` is already asserted against the rules snapshot
//     field-for-field in `Stratocracy.StratUI.T-INT-05.MirrorsSnapshotFieldForField`. This
//     file asserts a COMPONENT against the model, which is the layer that one does not
//     reach; it does not re-assert the model against the snapshot.
//
// THE FIXTURE IS `StratMatchReconcile.cpp`'S, DELIBERATELY DUPLICATED rather than shared, on
// the precedent `StratGuidedOpeningVisuals.cpp` set for the same reason: a helper shared
// across gate files makes one file's convenience edit another file's silent change of
// subject. This fixture has no tile meshes, `StartMatch` therefore returns false while
// `IsMatchLive()` returns true -- the presentation split `StratMatchSubsystem.h` documents
// -- and every clause declares the board's warning expected.
//
// NO `strat::` FREE FUNCTION IS CALLED ANYWHERE BELOW. That is LNK2019 outside StratBridge
// and StratRules.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

namespace StratMapMarkerClauses
{
	static const int32 kFirstSide   = 0;
	static const int32 kViewingSide = 0;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** `StratMatchReconcile.cpp`'s config, duplicated per this file's header. The actor
	 *  classes are the C++ classes themselves; phase 5's Blueprints subclass them to carry
	 *  meshes, and a headless gate has none -- see the header on why that is asserted rather
	 *  than papered over. */
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
		Out.ViewingSide     = kViewingSide;
		Out.BoardActorClass = AStratBoardActor::StaticClass();
		Out.UnitActorClass  = AStratUnitActor::StaticClass();
		return true;
	}

	/** The transient world. THE WORLD GOES FIRST AND THE CONTEXT SECOND -- measured in
	 *  `StratMatchReconcile.cpp`: the other order makes `Deinitialize` warn "World has no
	 *  context!" once per destroyed actor, and a gate that leaves warnings in a green run
	 *  trains the next reader to skim warnings. */
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

		FTestWorldScope(const FTestWorldScope&) = delete;
		FTestWorldScope& operator=(const FTestWorldScope&) = delete;
	};

	/** Every `AStratUnitActor` alive in the world. Read from the WORLD and not from the
	 *  subsystem's map, on `StratMatchReconcile.cpp`'s reasoning: only an iterator over the
	 *  world can see an actor the reconciliation map forgot about. */
	static void LiveUnitActors(UWorld* World, TArray<AStratUnitActor*>& Out)
	{
		Out.Reset();
		if (World == nullptr)
		{
			return;
		}
		for (TActorIterator<AStratUnitActor> It(World); It; ++It)
		{
			AStratUnitActor* const Actor = *It;
			if (IsValid(Actor) && !Actor->IsActorBeingDestroyed())
			{
				Out.Add(Actor);
			}
		}
	}

	/** What the two accessors answered for one actor, so a later pass can be compared
	 *  against an EARLIER READING rather than against a rule this file re-derived. */
	struct FMarkerReading
	{
		bool bFlag = false;
		bool bPip  = false;
	};

	static void ReadMarkers(UWorld* World, TMap<int32, FMarkerReading>& Out)
	{
		Out.Reset();
		TArray<AStratUnitActor*> Actors;
		LiveUnitActors(World, Actors);
		for (AStratUnitActor* const Actor : Actors)
		{
			FMarkerReading Reading;
			Reading.bFlag = Actor->IsFlagMarkerVisible();
			Reading.bPip  = Actor->IsUnactedPipVisible();
			Out.Add(Actor->GetUnitId(), Reading);
		}
	}

	static FString Yes(const bool bValue)
	{
		return bValue ? TEXT("visible") : TEXT("hidden");
	}

	/**
	 * Bring up a live match and build its model. Returns false having already reported.
	 *
	 * `StartMatch`'s return is DELIBERATELY NOT the liveness test. `StratMatchSubsystem.h`
	 * states the split: all-or-nothing on the rules side, deliberately not on the
	 * presentation side, "a configuration gap in phase 5's assets must not be able to
	 * prevent a seeded, correct match from existing".
	 */
	static bool BringUpMatch(FAutomationTestBase& Test, FTestWorldScope& Scope,
		UStratMatchSubsystem*& OutSubsystem, FStratViewModel& OutModel)
	{
		if (!Test.TestNotNull(TEXT("a transient world was created"), Scope.World))
		{
			return false;
		}

		OutSubsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
		if (!Test.TestNotNull(TEXT("the world has a match subsystem"), OutSubsystem))
		{
			return false;
		}

		FStratMatchConfig Config;
		FString Error;
		if (!Test.TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
		{
			Test.AddError(Error);
			return false;
		}

		FString StartReason;
		OutSubsystem->StartMatch(Config, StartReason);
		if (!Test.TestTrue(TEXT("the match is live"), OutSubsystem->IsMatchLive()))
		{
			Test.AddError(StartReason);
			return false;
		}

		FString BuildError;
		if (!Test.TestTrue(TEXT("the view model builds"),
				OutSubsystem->BuildViewModel(OutModel, BuildError)))
		{
			Test.AddError(BuildError);
			return false;
		}
		if (!Test.TestTrue(TEXT("the scenario ships units"), OutModel.Units.Num() > 0))
		{
			return false;
		}
		return true;
	}
}

// ---------------------------------------------------------------------------
// CLAUSE 1 -- the two markers are a function of the LAST applied model alone.
//
// SEE THE FILE HEADER ON WHAT SHARE OF T-INT-05 THIS IS. The ID is about the whole screen;
// this clause is one actor's two components' share of it, and W4 does not discharge the ID.
//
// FOUR PASSES, AND THE THIRD IS THE ONE WITH TEETH.
//
//   Pass 1  the builder's own model. Every actor's pair of accessors is RECORDED, not
//           asserted -- there is nothing to compare it to yet, and a `TestTrue` here would
//           be this file inventing an expectation it has no source for.
//   Pass 2  the same model with `bIsFlag` and `bDone` INVERTED on every unit. This is the
//           intervening model that flips them. That it really did flip something is
//           asserted, because a pass-3 that "returned" to a state it never left would prove
//           nothing at all.
//   Pass 3  pass 1's model again, byte for byte. Every actor must answer EXACTLY what it
//           answered in pass 1. The expectation is pass 1's own reading -- a value this
//           file measured off the module, never one it computed. A component that latched
//           anything of its own across pass 2 is red here, and no compiler would have said
//           so.
//   Pass 4  `bIsFlag` false and `bDone` true on every unit. Every marker on every actor must
//           be hidden. This is "neither survives a model that turns the bit off"; a marker
//           that was ever set and never cleared -- a `SetVisibility(true)` inside an `if`
//           with no `else` -- passes passes 1 to 3 and dies here.
//
// WHAT A ONE-LINE EDIT WOULD DO TO IT. Deleting either `SetVisibility` call in
// `ApplyUnitView` leaves the component at its constructor value forever: pass 2 stops
// differing and its own premise assertion goes red. Changing either to a latch --
// `if (X) { Set(true); }` -- reddens pass 4.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratMarkersRebuildFromTheViewModelAloneTest,
	"Stratocracy.StratPlay.T-INT-05.MarkersRebuildFromTheViewModelAlone",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratMarkersRebuildFromTheViewModelAloneTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMapMarkerClauses;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	UStratMatchSubsystem* Subsystem = nullptr;
	FStratViewModel Original;
	if (!BringUpMatch(*this, Scope, Subsystem, Original))
	{
		return false;
	}

	// ---- pass 1: the builder's model, recorded -------------------------------
	Subsystem->ApplyView(Original);

	TMap<int32, FMarkerReading> First;
	ReadMarkers(Scope.World, First);
	if (!TestEqual(TEXT("every unit in the model has an actor after the first apply"),
			First.Num(), Original.Units.Num()))
	{
		return false;
	}

	// ---- pass 2: the intervening model that flips them ------------------------
	FStratViewModel Flipped = Original;
	for (FStratUnitView& U : Flipped.Units)
	{
		U.bIsFlag = !U.bIsFlag;
		U.bDone   = !U.bDone;
	}
	Subsystem->ApplyView(Flipped);

	TMap<int32, FMarkerReading> Middle;
	ReadMarkers(Scope.World, Middle);

	// FIXTURE PREMISE, NOT A CLAUSE ASSERTION. Counted rather than probed: if the flip moved
	// nothing, pass 3 below would be comparing a state against itself and would be green over
	// an actor that never listens to the model at all.
	int32 FlagsMoved = 0;
	int32 PipsMoved  = 0;
	for (const TPair<int32, FMarkerReading>& Pair : First)
	{
		const FMarkerReading* const Now = Middle.Find(Pair.Key);
		if (Now == nullptr)
		{
			continue;
		}
		if (Now->bFlag != Pair.Value.bFlag)
		{
			++FlagsMoved;
		}
		if (Now->bPip != Pair.Value.bPip)
		{
			++PipsMoved;
		}
	}
	if (!TestTrue(*FString::Printf(
			TEXT("the intervening model really moved the flag marker on at least one actor "
			     "(moved on %d of %d), so the return below is a return"),
			FlagsMoved, First.Num()), FlagsMoved > 0))
	{
		return false;
	}
	if (!TestTrue(*FString::Printf(
			TEXT("and the unacted pip on at least one actor (moved on %d of %d)"),
			PipsMoved, First.Num()), PipsMoved > 0))
	{
		return false;
	}

	// ---- pass 3: pass 1's model again ----------------------------------------
	Subsystem->ApplyView(Original);

	TMap<int32, FMarkerReading> Again;
	ReadMarkers(Scope.World, Again);

	TestEqual(TEXT("re-applying the first model leaves the same actor set"),
		Again.Num(), First.Num());

	for (const TPair<int32, FMarkerReading>& Pair : First)
	{
		const FMarkerReading* const Now = Again.Find(Pair.Key);
		if (Now == nullptr)
		{
			AddError(FString::Printf(
				TEXT("unit %d has no actor after the first model was re-applied"), Pair.Key));
			continue;
		}

		// THE EXPECTATION IS PASS 1'S OWN READING. Nothing here re-derives what the marker
		// rule ought to say; the same model must reproduce the same answer.
		TestEqual(*FString::Printf(
				TEXT("unit %d: the flag marker is %s again, exactly as it was the first time "
				     "this model was applied"),
				Pair.Key, *Yes(Pair.Value.bFlag)),
			Now->bFlag, Pair.Value.bFlag);
		TestEqual(*FString::Printf(
				TEXT("unit %d: the unacted pip is %s again, exactly as it was the first time "
				     "this model was applied"),
				Pair.Key, *Yes(Pair.Value.bPip)),
			Now->bPip, Pair.Value.bPip);
	}

	// ---- pass 4: a model that turns both bits off ----------------------------
	// The input is the literal; the assertion is the ABSENCE. `bDone` true hides the pip
	// whatever the side, and `bIsFlag` false hides the flag marker unconditionally, so this
	// pass requires a clean board and nothing subtler.
	FStratViewModel Dark = Original;
	for (FStratUnitView& U : Dark.Units)
	{
		U.bIsFlag = false;
		U.bDone   = true;
	}
	Subsystem->ApplyView(Dark);

	TMap<int32, FMarkerReading> Off;
	ReadMarkers(Scope.World, Off);
	TestEqual(TEXT("the dark model leaves the same actor set"), Off.Num(), First.Num());

	for (const TPair<int32, FMarkerReading>& Pair : Off)
	{
		TestFalse(*FString::Printf(
				TEXT("unit %d: no unit is a flag in this model, so no flag marker survives it"),
				Pair.Key),
			Pair.Value.bFlag);
		TestFalse(*FString::Printf(
				TEXT("unit %d: every unit is done in this model, so no unacted pip survives it"),
				Pair.Key),
			Pair.Value.bPip);
	}

	return true;
}

// ---------------------------------------------------------------------------
// CLAUSE 2 -- the flag `H` marker shows for BOTH SIDES, always.
//
// §2.11.2: `| Flag `H` marker (both sides, always visible) | What to protect, what to hunt |`.
// "Both sides" rules out a `View.Side == ViewingSide` test. That is the whole clause, and it
// exists because the guided marker eighteen lines away in the same function DOES carry that
// test, put there by a 2026-08-23 user ruling, and copying it here would be the natural way
// to build a second marker.
//
// THE DECISIVE CONTROL IS ONE UNIT: `bIsFlag == true` and `Side != ViewingSide` in the same
// unit. The shipped scenario supplies it -- `ferrum_crossing.json` deploys exactly one flag
// per side and the fixture views from side 0 -- so this file decorates NOTHING. Its
// existence is asserted and its evaluation is COUNTED, because a control that quietly failed
// to reach the board would leave this clause green and empty.
//
// AND THE EXPECTATION IS THE MODEL'S OWN `bIsFlag`, a module-side value: `StratBuildViewModel`
// sets it from the rules snapshot's `UiUnitView::isFlag`. This file never writes that bit.
//
// WHAT A ONE-LINE EDIT WOULD DO TO IT. `SetVisibility(View.bIsFlag && View.Side ==
// ViewingSide)` reddens the named enemy-flag assertion. `SetVisibility(View.bIsGuidedMarked)`
// or `SetVisibility(false)` reddens both flag units. `SetVisibility(true)` reddens the eight
// non-flag units.
//
// IT PINS A FLAG AND NOT PIXELS -- see the file header. `IsFlagMarkerVisible` answers TRUE
// for a flag unit whose `FlagMarkerMesh` is unset, which in this headless fixture is every
// one of them.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratFlagMarkerShowsForBothSidesAlwaysTest,
	"Stratocracy.StratPlay.GATE-MAPMARKERS.FlagMarkerShowsForBothSidesAlways",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratFlagMarkerShowsForBothSidesAlwaysTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMapMarkerClauses;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	UStratMatchSubsystem* Subsystem = nullptr;
	FStratViewModel Model;
	if (!BringUpMatch(*this, Scope, Subsystem, Model))
	{
		return false;
	}

	const int32 Seat = Model.ViewingSide;

	// ---- the control's existence, found and not assumed ----------------------
	// Scanned rather than probed with an assertion: `TestTrue` on a hypothesis is a claim
	// about the subject, and what is wanted here is a question about the fixture.
	int32 OwnFlagId     = INDEX_NONE;
	int32 EnemyFlagId   = INDEX_NONE;
	int32 OwnFlags      = 0;
	int32 EnemyFlags    = 0;
	int32 NonFlagCount  = 0;
	for (const FStratUnitView& U : Model.Units)
	{
		if (!U.bIsFlag)
		{
			++NonFlagCount;
			continue;
		}
		if (U.Side == Seat)
		{
			++OwnFlags;
			if (OwnFlagId == INDEX_NONE) { OwnFlagId = U.UnitId; }
		}
		else
		{
			++EnemyFlags;
			if (EnemyFlagId == INDEX_NONE) { EnemyFlagId = U.UnitId; }
		}
	}

	// FIXTURE PREMISES. Each is a fact about `ferrum_crossing.json` as the rules module
	// projects it, and each is required for the assertions below to mean anything.
	if (!TestTrue(*FString::Printf(
			TEXT("the scenario gives the viewing seat %d a flag unit of its own (found %d)"),
			Seat, OwnFlags), OwnFlagId != INDEX_NONE))
	{
		return false;
	}
	if (!TestTrue(*FString::Printf(
			TEXT("THE DECISIVE CONTROL: the scenario gives a NON-viewing side a flag unit, so "
			     "this clause contains a unit with `bIsFlag` true and `Side != %d` (found %d)"),
			Seat, EnemyFlags), EnemyFlagId != INDEX_NONE))
	{
		return false;
	}
	if (!TestTrue(*FString::Printf(
			TEXT("and it ships non-flag units too (found %d), so \"shows for flags\" is "
			     "distinguishable from \"shows for everything\""),
			NonFlagCount), NonFlagCount > 0))
	{
		return false;
	}

	Subsystem->ApplyView(Model);

	TArray<AStratUnitActor*> Actors;
	LiveUnitActors(Scope.World, Actors);
	if (!TestEqual(TEXT("every unit in the model reached the board"),
			Actors.Num(), Model.Units.Num()))
	{
		return false;
	}

	int32 SawEnemyFlag = 0;
	int32 SawOwnFlag   = 0;
	for (AStratUnitActor* const Actor : Actors)
	{
		const int32 Id = Actor->GetUnitId();

		const FStratUnitView* Source = nullptr;
		for (const FStratUnitView& U : Model.Units)
		{
			if (U.UnitId == Id) { Source = &U; break; }
		}
		if (Source == nullptr)
		{
			AddError(FString::Printf(TEXT("actor for unit %d has no entry in the model"), Id));
			continue;
		}

		// THE EXPECTATION IS THE MODEL'S OWN MODULE-SOURCED BIT, on every actor and with no
		// side term anywhere in the comparison.
		TestEqual(*FString::Printf(
				TEXT("unit %d on side %d: the flag marker follows the model's own `bIsFlag` "
				     "(%s) and nothing else"),
				Id, Source->Side, *Yes(Source->bIsFlag)),
			Actor->IsFlagMarkerVisible(), Source->bIsFlag);

		if (Id == EnemyFlagId)
		{
			++SawEnemyFlag;
			TestTrue(*FString::Printf(
					TEXT("Sec 2.11.2 -- \"both sides, always visible\": unit %d is a flag and is "
					     "NOT on the viewing side %d, so it MUST wear the flag marker. This is "
					     "the assertion a copy of the guided marker's `View.Side == ViewingSide` "
					     "filter turns red, and it is the only one in the clause that can"),
					Id, Seat),
				Actor->IsFlagMarkerVisible());
		}
		else if (Id == OwnFlagId)
		{
			++SawOwnFlag;
			TestTrue(*FString::Printf(
					TEXT("and unit %d, the viewing seat's own flag, wears it too"), Id),
				Actor->IsFlagMarkerVisible());
		}
	}

	// The controls were EVALUATED, not merely written. A control that never reached the board
	// leaves the loop above silent and this clause green over the defect it exists to catch.
	TestEqual(TEXT("the non-viewing side's flag unit really did reach the board, so the "
	               "decisive assertion above was actually evaluated"),
		SawEnemyFlag, 1);
	TestEqual(TEXT("and so did the viewing seat's own"), SawOwnFlag, 1);

	return true;
}

// ---------------------------------------------------------------------------
// CLAUSE 3 -- the unacted pip is OWN SIDE and the DONE BIT, and nothing else.
//
// §2.11.2: `| Unacted pip on own units | ... via the DONE bit of §2.11.1's machine, carried
// in the view-model's presentation block |`. Two operands, and this clause pins both by
// making each one, alone, decisive.
//
// THE DISCRIMINATING PAIR. Units B and C are BOTH on the viewing seat, carry IDENTICAL
// `bHasMoved` and `bHasActed`, and carry OPPOSITE `bDone`. No function whatever of the two
// turn flags -- not `!bHasActed`, not `!(bHasMoved && bHasActed)`, not any of the sixteen --
// can answer both correctly, because its inputs are equal and its required outputs are not.
// §2.11.1 is why a wrong answer here is plausible rather than exotic: "Space (Wait) and
// RMB/Esc in MOVED both reach DONE without acting, so the two bits come apart in ordinary
// play", and a pip bound to the act flag would keep a waited unit's pip lit.
//
// THE SIDE TERM'S OWN CONTROL. Unit E is on the NON-viewing side and is NOT done -- the same
// `bDone` as unit C, whose pip must be lit. So the two units differ in side alone, and a pip
// that had dropped the side test is red on E while everything else stays green.
//
// AND `bLockedThisTurn` IS DRIVEN APART FROM `bDone`, because `StratViewModel.h` records that
// their lifecycles differ -- lock clears when §2.11.6 beat 1a retires, inside a turn, so "a
// unit can be un-locked and not-done in the same turn". `StratUnitActor.h` states that the
// lock bit is deliberately NOT an operand of this pip. Unit C is not done and IS locked; if
// the lock bit ever entered the expression, C goes dark and this clause says so.
//
// WHERE THE EXPECTATIONS COME FROM, STATED PLAINLY BECAUSE THEY ARE THE TYPED VALUES IN THIS
// FILE. `bDone`, `bLockedThisTurn` and the two turn flags are written onto units the
// projection supplied, standing where the projection put them, and the required outcome of
// each case is §2.11.2's own word -- "on own units", via the DONE bit. There is no
// module-side value to read: the GDD states "the rules module has no DONE bit ... no snapshot
// field mirrors this bit and none is asked for", and its producer,
// `FStratSelectionMachine::DecorateViewModel`, has its own parity gate one module up. What is
// asserted here is the SELECTION -- which bits on the unit reach the component -- and nothing
// about what the bit means. NO ASSERTION BELOW RESTATES `!bDone && Side == ViewingSide` as an
// expression; each case is named and its outcome written out.
//
// WHAT A ONE-LINE EDIT WOULD DO TO IT. Dropping the side term reddens unit E. Dropping the
// `bDone` term reddens unit B. Substituting `!View.bHasActed` reddens B or C. Adding
// `&& !View.bLockedThisTurn` reddens C.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratUnactedPipIsOwnSideAndTheDoneBitTest,
	"Stratocracy.StratPlay.GATE-MAPMARKERS.UnactedPipIsOwnSideAndTheDoneBit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratUnactedPipIsOwnSideAndTheDoneBitTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMapMarkerClauses;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	UStratMatchSubsystem* Subsystem = nullptr;
	FStratViewModel Model;
	if (!BringUpMatch(*this, Scope, Subsystem, Model))
	{
		return false;
	}

	const int32 Seat = Model.ViewingSide;

	// ---- pick the cast, by scanning and not by probing ------------------------
	TArray<int32> OwnIds;
	TArray<int32> OtherIds;
	for (const FStratUnitView& U : Model.Units)
	{
		if (U.Side == Seat) { OwnIds.Add(U.UnitId); } else { OtherIds.Add(U.UnitId); }
	}

	if (!TestTrue(*FString::Printf(
			TEXT("the scenario gives the viewing seat %d at least two units, so the "
			     "identical-turn-flags pair can exist (found %d)"),
			Seat, OwnIds.Num()), OwnIds.Num() >= 2))
	{
		return false;
	}
	if (!TestTrue(*FString::Printf(
			TEXT("and at least one unit off it, so the side term has a control (found %d)"),
			OtherIds.Num()), OtherIds.Num() >= 1))
	{
		return false;
	}

	const int32 IdB = OwnIds[0];   // own side, done      -> no pip
	const int32 IdC = OwnIds[1];   // own side, NOT done  -> pip
	const int32 IdE = OtherIds[0]; // other side, NOT done -> no pip

	// ---- drive the bits -------------------------------------------------------
	// THE TURN FLAGS ARE MADE EQUAL ON B AND C ON PURPOSE. Both are driven to "has moved and
	// has acted", which is the state a unit that spent its whole turn is in -- so the ONLY
	// difference between B and C is the presentation bit.
	FStratViewModel Driven = Model;
	for (FStratUnitView& U : Driven.Units)
	{
		U.bHasMoved       = true;
		U.bHasActed       = true;
		U.bDone           = true;
		U.bLockedThisTurn = false;

		if (U.UnitId == IdC)
		{
			U.bDone           = false;
			// DRIVEN APART FROM `bDone`, per `StratViewModel.h`: this unit is not done AND is
			// locked, which is a state the guidance layer really produces.
			U.bLockedThisTurn = true;
		}
		else if (U.UnitId == IdE)
		{
			U.bDone = false;
		}
	}

	Subsystem->ApplyView(Driven);

	AStratUnitActor* const B = Subsystem->FindUnitActor(IdB);
	AStratUnitActor* const C = Subsystem->FindUnitActor(IdC);
	AStratUnitActor* const E = Subsystem->FindUnitActor(IdE);
	if (!TestNotNull(TEXT("the done own-side unit has an actor"), B)
		|| !TestNotNull(TEXT("the not-done own-side unit has an actor"), C)
		|| !TestNotNull(TEXT("the not-done other-side unit has an actor"), E))
	{
		return false;
	}

	// ---- the fixture premise the pair rests on --------------------------------
	// Read back off the APPLIED MODEL, so this states what was actually applied rather than
	// what was intended. If these two ever stopped being equal, the pair below would stop
	// discriminating and nothing else would notice.
	const FStratUnitView* ViewB = nullptr;
	const FStratUnitView* ViewC = nullptr;
	const FStratUnitView* ViewE = nullptr;
	for (const FStratUnitView& U : Driven.Units)
	{
		if (U.UnitId == IdB) { ViewB = &U; }
		else if (U.UnitId == IdC) { ViewC = &U; }
		else if (U.UnitId == IdE) { ViewE = &U; }
	}
	if (ViewB == nullptr || ViewC == nullptr || ViewE == nullptr)
	{
		AddError(TEXT("the driven model lost one of its three subject units"));
		return false;
	}

	TestEqual(TEXT("PREMISE: units B and C carry the same `bHasMoved`"),
		ViewC->bHasMoved, ViewB->bHasMoved);
	TestEqual(TEXT("PREMISE: and the same `bHasActed`, so no function of the turn flags can "
	               "answer both of them"),
		ViewC->bHasActed, ViewB->bHasActed);
	TestNotEqual(TEXT("PREMISE: and OPPOSITE `bDone`, which is the only thing that differs"),
		ViewC->bDone, ViewB->bDone);
	TestEqual(TEXT("PREMISE: units C and E carry the same `bDone`, so they differ in SIDE alone"),
		ViewE->bDone, ViewC->bDone);
	TestNotEqual(TEXT("PREMISE: and they really are on different sides"),
		ViewE->Side, ViewC->Side);
	TestEqual(*FString::Printf(TEXT("PREMISE: unit C is on the viewing seat %d"), Seat),
		ViewC->Side, Seat);

	// ---- the four cases, each named --------------------------------------------
	TestTrue(*FString::Printf(
			TEXT("Sec 2.11.2 \"unacted pip on own units\": unit %d is on the viewing seat %d and "
			     "is NOT done, so it wears the pip -- and it is LOCKED while wearing it, which "
			     "is the assertion an added `!bLockedThisTurn` term turns red"),
			IdC, Seat),
		C->IsUnactedPipVisible());

	TestFalse(*FString::Printf(
			TEXT("unit %d is on the same seat and carries the SAME two turn flags as unit %d, "
			     "and differs from it only in the DONE bit -- so it wears no pip. This is the "
			     "assertion any pip bound to `bHasActed` instead of DONE turns red"),
			IdB, IdC),
		B->IsUnactedPipVisible());

	TestFalse(*FString::Printf(
			TEXT("unit %d is NOT done -- the same `bDone` as unit %d, whose pip is lit -- but "
			     "it is on side %d and not on the viewing seat %d, so it wears no pip. This is "
			     "the assertion a pip that dropped its side test turns red"),
			IdE, IdC, ViewE->Side, Seat),
		E->IsUnactedPipVisible());

	// ---- and the rest of the board, so "on own units" is a board-wide fact -----
	// Every other unit was driven done, so exactly ONE pip may be lit anywhere.
	TArray<AStratUnitActor*> Actors;
	LiveUnitActors(Scope.World, Actors);
	int32 LitPips = 0;
	for (AStratUnitActor* const Actor : Actors)
	{
		if (Actor->IsUnactedPipVisible())
		{
			++LitPips;
			TestEqual(TEXT("the only pip lit on the whole board is the not-done own-side unit's"),
				Actor->GetUnitId(), IdC);
		}
	}
	TestEqual(*FString::Printf(
			TEXT("exactly one pip is lit across all %d actors"), Actors.Num()),
		LitPips, 1);

	return true;
}
