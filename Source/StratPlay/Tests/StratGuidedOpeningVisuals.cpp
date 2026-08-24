// The gate on §2.11.6-B's TWO ON-SCREEN SURFACES -- the objective ring and the turn-1a unit
// marker -- filed under T-UI-02.
//
// WHAT GAP THESE TWO CLAUSES CLOSE. §2.11.6-B beat 1a says "Select the marked Infantry" and
// beat 2 says "Move the Infantry onto the ringed Factory". Until 2026-08-23 nothing on screen
// was marked and nothing was ringed; the directives named two visuals that did not exist, and
// the player's only route to either fact was the log. `AStratBoardActor::ObjectiveOverlay` and
// `AStratUnitActor::GuidedMarker` are the seams that closed it, and neither had a gate.
//
// WHY T-UI-02 AND WHY NOT T-UI-03. T-UI-02 is `StratPlay`'s BOARD-AND-HIGHLIGHT id: it already
// carries `BoardHexRoundTrip`, `ReachOverlayIsNotComputedHere` and the four guidance input
// gates in `StratGuidanceInputGates.cpp`, and `AttackIsClosedForTheMarkedInfantry` is the
// clause the ring must not collide with -- it asserts `ShowTargets` filled exactly one overlay
// component and that the component is not the reach one. T-UI-03 is the no-widget-side-
// arithmetic id and is where the BEAT MACHINE's clauses live (`StratGuidedOpeningClauses.cpp`);
// nothing here observes a beat. No acceptance id is minted by this file.
//
// WHY THESE TWO CLAUSES ARE IN `StratPlay` AND COULD BE NOWHERE ELSE. They need
// `UStratMatchSubsystem::ApplyView`, `AStratBoardActor`, `AStratUnitActor` and `FStratBridge`
// in one translation unit. `StratPlay` is the only module that sees all four; in `StratUI` or
// `StratBridge` these are `LNK2019` and not tests.
//
// THE INSTRUMENT TRAP THAT SHAPES CLAUSE ONE, and it is measured rather than anticipated.
// `AStratBoardActor::FillOverlay` returns early -- silently, by design -- when the overlay
// component has no static mesh. On a fixture with no Blueprint defaults, every overlay accessor
// therefore reads 0 for every input, forever, and a clause asserting "the ring is lit" against
// that instrument could never go green while a clause asserting "the ring is dark" could never
// go red. `GiveTheBoardAnOverlayMesh` below is what makes the accessor able to speak, and
// `StratGuidanceInputGates.cpp` records the same measurement for `GetTargetOverlayCount`.
//
// THE INSTRUMENT TRAP THAT SHAPES CLAUSE TWO IS THE OPPOSITE ONE, AND IT IS NOW SETTLED AT ITS
// SOURCE. `AStratUnitActor::IsGuidedMarkerVisible`'s declaration used to state "FALSE WITH NO
// MARKER MESH ASSIGNED, which is the state this ships in", and to conclude that a clause
// telling "not marked" from "marked but unconfigured" apart "must assign `GuidedMarkerMesh` on
// the spawned actor first". THAT IS NOT WHAT THE ENGINE DOES: `IsGuidedMarkerVisible` returns
// `GuidedMarker->IsVisible()`, and `USceneComponent::IsVisible` consults `bHiddenInGame`, the
// visible flag and the cached level collection -- and NOT the static mesh. The measurement was
// taken here, the sentence was retracted in that header on 2026-08-23 with the measurement
// attached, and clause two consequently assigns no mesh and MEASURES the discrimination
// instead: a true and a false in the same frame on the same unconfigured actors.
//
// WHAT SURVIVES AS A LIMIT RATHER THAN AS AN ERROR: that accessor reports the visible FLAG, not
// pixels. Nothing headless gates "the marker is on screen", and no clause here tries to.
//
// WHERE THE EXPECTATIONS COME FROM, clause by clause.
//   - The ring's hex is `FStratGuidedOpening::DecorateViewModel`'s own `ObjectiveHex`, which
//     that class copied from `FStratBridge::GuidedOpeningHexes`. This file arms a real
//     `FStratGuidedOpening` on the subsystem's own bridge and lets it write the whole
//     `FStratGuidanceView`; not one guidance field is typed out here.
//   - The ring's WORLD position is `AStratBoardActor::WorldLocationOfHex`, asked of the board
//     the subsystem spawned. The axial -> world formula does not appear in this file and must
//     not: `StratMatchReconcile.cpp` records that a copy here would be arithmetic the gate then
//     asserts against itself.
//   - The marked unit is `FStratUnitView::bIsGuidedMarked`, which the rules module DECLARES
//     derived off `placement`, ANDed with `FStratUnitView::Side == FStratViewModel::ViewingSide`
//     per the 2026-08-23 user ruling. Both are read off the applied model, never recomputed and
//     never compared against a hex; clause two's own block records why an expression is the
//     honest expectation here and what makes it more than a mirror of the implementation.
//   - The marked unit's move is played through `FStratBridge::SubmitMoveToHex` to a destination
//     `FStratBridge::ReachableHexes` offered. No hex is chosen by this file.
//
// NO `strat::` TYPE IS NAMED AND NO `strat::` FREE FUNCTION IS CALLED. That is
// `StratPlay.Build.cs`'s stated property of this module and a test file is part of the module;
// measured 8 x LNK2019 the first time a call crossed that boundary.

#include "Misc/AutomationTest.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Math/IntPoint.h"
#include "Misc/Paths.h"
#include "UObject/UnrealType.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratGuidedOpening.h"
#include "StratMatchSubsystem.h"
#include "StratSelectionMachine.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

// Included from a .cpp and never from a UHT-parsed header -- the module-wide discipline.
#include "StratBridge.h"

namespace StratGuidedOpeningVisuals
{
	static const int32 kFirstSide = 0;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/**
	 * The match configuration a live match needs.
	 *
	 * The same shape `StratMatchReconcile.cpp` uses and for the reasons it records: the
	 * scenario path is project-relative exactly as `FStratMatchConfig::ScenarioFile`
	 * documents, and the actor classes are the C++ classes themselves, neither of which is
	 * `Abstract`. Phase 5's Blueprints subclass them to carry meshes; this file supplies the
	 * one mesh it needs through the reflection system instead.
	 */
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
		Out.ViewingSide     = 0;
		Out.BoardActorClass = AStratBoardActor::StaticClass();
		Out.UnitActorClass  = AStratUnitActor::StaticClass();
		return true;
	}

	/** The transient world, with the engine context teardown wants. `StratMatchReconcile.cpp`
	 *  records why the order in the destructor is measured rather than chosen. */
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

		// THE WORLD GOES FIRST AND THE CONTEXT SECOND. With the context destroyed first,
		// `UStratMatchSubsystem::Deinitialize` -- which destroys the board and every unit actor
		// from inside `DestroyWorld` -- makes `UWorld::DestroyActor` warn "World has no
		// context!" once per actor, and a gate that leaves warnings in a green run trains the
		// next reader to skim warnings.
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
	 * Gives the subsystem-spawned board an overlay mesh, so the overlay accessors can be
	 * non-zero at all.
	 *
	 * WITHOUT THIS EVERY OVERLAY ACCESSOR IS MUTE, AND MUTE READS AS PASSING. See the file
	 * header. Lifted from `StratGuidanceInputGates.cpp`, which measured it; duplicated rather
	 * than shared, which is this project's standing posture for a fixture helper -- sharing
	 * would mean a header inside a `Tests/` directory that other modules include.
	 *
	 * `/Engine/` AND NOT `/Game/`, so this does not touch the project content the
	 * no-path-literal rule is about. Automation fixtures are one of that rule's two named
	 * exceptions in any case.
	 *
	 * SET ON THE INSTANCE AND NEVER ON THE CDO. A `GetDefaultObject()` write would leak this
	 * mesh into every other clause in the suite that spawns a board.
	 *
	 * `OverlayMesh` IS APPLIED AT `BeginPlay` and not in the constructor -- the board's own
	 * block records why (the property is a Blueprint default, and a constructor runs on the
	 * CDO). The subsystem spawns the board into a world that has not begun play, so
	 * `DispatchBeginPlay` is the supported route, guarded so a fixture that ever does begin
	 * play cannot dispatch twice.
	 */
	static bool GiveTheBoardAnOverlayMesh(AStratBoardActor* Board, FString& OutError)
	{
		if (Board == nullptr)
		{
			OutError = TEXT("the match spawned no board to give an overlay mesh to");
			return false;
		}

		UStaticMesh* const Mesh =
			LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
		if (Mesh == nullptr)
		{
			OutError = TEXT("/Engine/BasicShapes/Cube.Cube did not load");
			return false;
		}

		FObjectPropertyBase* const Property =
			FindFProperty<FObjectPropertyBase>(Board->GetClass(), FName(TEXT("OverlayMesh")));
		if (Property == nullptr)
		{
			OutError = TEXT("no reflected object property 'OverlayMesh' on AStratBoardActor");
			return false;
		}
		Property->SetObjectPropertyValue(Property->ContainerPtrToValuePtr<void>(Board), Mesh);

		if (!Board->HasActorBegunPlay())
		{
			Board->DispatchBeginPlay();
		}
		return true;
	}

	/**
	 * The board's `ObjectiveOverlay` component, read through reflection.
	 *
	 * WHY BY NAME AND NOT BY A SCAN OVER THE BOARD'S COMPONENTS. `AStratBoardActor.h` says it
	 * outright as of 2026-08-23: with a THIRD overlay in existence, a `GetComponents` scan for
	 * "the one that is not the reach overlay" returns whichever of two the iteration order
	 * reached first. The property is `protected`, so the details-panel surface -- reflection --
	 * is the only door, and it is the same door `StratGuidanceStripClauses.cpp` uses for
	 * `GuidanceStrip` and this file's own `GiveTheBoardAnOverlayMesh` uses for `OverlayMesh`.
	 *
	 * IT PRODUCES NO VALUE THIS FILE THEN ASSERTS. It is used to read WHERE the ring is drawn;
	 * the count is read through the public `GetObjectiveOverlayCount()`, and the clause asserts
	 * the two agree, which is what makes the accessor's "off the component, not a cached
	 * number" claim a measurement.
	 */
	static UHierarchicalInstancedStaticMeshComponent* ObjectiveOverlayOf(
		AStratBoardActor* Board, FString& OutError)
	{
		if (Board == nullptr)
		{
			OutError = TEXT("no board to read an objective overlay from");
			return nullptr;
		}

		FObjectPropertyBase* const Property =
			FindFProperty<FObjectPropertyBase>(Board->GetClass(), FName(TEXT("ObjectiveOverlay")));
		if (Property == nullptr)
		{
			OutError = TEXT("no reflected object property 'ObjectiveOverlay' on AStratBoardActor");
			return nullptr;
		}

		UObject* const Value =
			Property->GetObjectPropertyValue(Property->ContainerPtrToValuePtr<void>(Board));
		UHierarchicalInstancedStaticMeshComponent* const Component =
			Cast<UHierarchicalInstancedStaticMeshComponent>(Value);
		if (Component == nullptr)
		{
			OutError = TEXT("'ObjectiveOverlay' is not a hierarchical instanced mesh component");
			return nullptr;
		}
		return Component;
	}

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

	/** The single unit the rules module marks for a side, or `INDEX_NONE`. Read off
	 *  `FStratUnitView::bIsGuidedMarked` and never derived from a hex -- see the file header
	 *  and `StratGuidedOpening.h`, which record why the hex comparison is the bug. */
	static int32 MarkedUnitId(const FStratViewModel& Model, int32 Side, int32& OutHowMany)
	{
		int32 Found = INDEX_NONE;
		OutHowMany = 0;
		for (const FStratUnitView& U : Model.Units)
		{
			if (U.Side == Side && U.bIsGuidedMarked)
			{
				++OutHowMany;
				if (Found == INDEX_NONE)
				{
					Found = U.UnitId;
				}
			}
		}
		return Found;
	}

	/**
	 * The three populations SS2.11.6-B's marker rule distinguishes, taken off one model.
	 *
	 * THREE AND NOT TWO, AND THE THIRD IS THE 2026-08-23 USER RULING.
	 * `Data/ferrum_crossing.json` authors a `guidedOpening` entry for side 0 AND for side 1,
	 * and `Ui.h` declares `isGuidedMarked` true "on the placement that the scenario file's
	 * `guidedOpening.infantry` names for this unit's SEAT", so TWO units carry the bit at
	 * once. The marked unit that is NOT on the viewing side is therefore a real, populated
	 * case on the shipped board -- and it is the case the ruling is about. A census that only
	 * split marked from unmarked would have no negative subject that is marked, and the side
	 * filter would be unfalsifiable against it. The clause asserts all three are non-empty.
	 *
	 * BOTH OPERANDS ARE FIELDS OF THE MODEL. `bIsGuidedMarked` is the rules module's own
	 * per-seat derivation off `placement`; `Side` is `UiUnit::side`; `ViewingSide` is the
	 * model's own, which is what `AStratUnitActor::ApplyUnitView` is handed. Nothing here is
	 * derived, nothing is a literal, and no hex is compared.
	 */
	struct FMarkerCensus
	{
		TArray<int32> MarkedOnViewingSide;
		TArray<int32> MarkedOffViewingSide;
		TArray<int32> Unmarked;

		void TakeFrom(const FStratViewModel& Model)
		{
			MarkedOnViewingSide.Reset();
			MarkedOffViewingSide.Reset();
			Unmarked.Reset();

			for (const FStratUnitView& U : Model.Units)
			{
				if (!U.bIsGuidedMarked)
				{
					Unmarked.Add(U.UnitId);
				}
				else if (U.Side == Model.ViewingSide)
				{
					MarkedOnViewingSide.Add(U.UnitId);
				}
				else
				{
					MarkedOffViewingSide.Add(U.UnitId);
				}
			}
		}
	};

	/** A unit view by id, or null. */
	static const FStratUnitView* UnitById(const FStratViewModel& Model, int32 UnitId)
	{
		for (const FStratUnitView& U : Model.Units)
		{
			if (U.UnitId == UnitId)
			{
				return &U;
			}
		}
		return nullptr;
	}
}

// ---------------------------------------------------------------------------
// The objective ring lights EXACTLY the guidance hex, on the objective overlay and on no
// other, and goes dark when guidance stops running.
//
// THREE THINGS ARE ASSERTED TOGETHER AND EACH CATCHES A DIFFERENT WRONG IMPLEMENTATION.
//   1. `GetObjectiveOverlayCount() == 1`. A ring that never drew, or one that drew the whole
//      objective set rather than the one authored hex, fails here. `AStratBoardActor::
//      ShowObjective` takes a single `FIntPoint` precisely so "every objective" cannot be
//      passed -- that is the "nearest objective" heuristic SS2.11.6 forbids by name arriving
//      through the back door -- and this is the count that would report it if it did.
//   2. `GetTargetOverlayCount() == 0`. THE RING MUST NOT HAVE LANDED ON SS2.6'S ATTACK
//      OVERLAY, which is the single most plausible wrong implementation because
//      `TargetOverlay` was sitting there unused in this frame. It is also the one that would
//      break an existing clause silently in the other direction:
//      `T-UI-02.AttackIsClosedForTheMarkedInfantry` asserts the attack overlay is EMPTY during
//      beat 1a, and a ring drawn there would light it at exactly that moment.
//   3. THE INSTANCE IS AT THE GUIDANCE HEX. A count of one says something was drawn, not
//      where. The drawn instance's world transform is compared, in XY, against
//      `AStratBoardActor::WorldLocationOfHex(Model.Guidance.ObjectiveHex)` -- the board's own
//      forward map, asked of the board that drew it. Z IS NOT COMPARED: `OverlayZOffset` is a
//      phase-5 presentation property this file must not guess, and the clause is about which
//      hex, not how high.
//
// AND THEN THE OTHER DIRECTION, IN THE SAME FIXTURE. `FStratGuidedOpening::SkipGuidance`
// publishes `bActive` false through the same `DecorateViewModel`, and the ring must go to 0.
// That half is what pins the `else` in `UStratMatchSubsystem::ApplyView`: a "show" with no
// "clear" leaves the ring standing after the window closed, with nothing in the model
// recording why -- the delta-shaped thinking T-INT-05 exists to refuse.
//
// THE LIT HALF IS THE OTHER HALF'S CONTROL, and that is not decoration. On a fixture with no
// overlay mesh both halves read 0 and the dark half would pass having measured nothing. Having
// shown 1 on this same board in this same clause is what gives the 0 its meaning.
//
// NOT ONE GUIDANCE FIELD IS TYPED OUT. A real `FStratGuidedOpening` is armed on the
// subsystem's own bridge and writes the whole `FStratGuidanceView` through `DecorateViewModel`,
// exactly as `AStratPlayerController` has it do in the running game.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratObjectiveRingLightsTheGuidanceHexTest,
	"Stratocracy.StratPlay.T-UI-02.ObjectiveRingLightsExactlyTheGuidanceHex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratObjectiveRingLightsTheGuidanceHexTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidedOpeningVisuals;

	// The board's own sentence about a fixture with no Blueprint tile meshes. Declared
	// expected rather than suppressed, so the line stays in the log for a human.
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
	FString Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	FString StartReason;
	Subsystem->StartMatch(Config, StartReason);
	if (!TestTrue(TEXT("the match is live whatever StartMatch returned"), Subsystem->IsMatchLive()))
	{
		AddError(StartReason);
		return false;
	}

	FStratBridge* const Bridge = Subsystem->GetBridge();
	if (!TestNotNull(TEXT("a live match owns a bridge"), Bridge))
	{
		return false;
	}

	AStratBoardActor* const Board = Subsystem->GetBoard();
	if (!TestNotNull(TEXT("the subsystem spawned a board"), Board))
	{
		return false;
	}

	// ---- the instrument is given a voice before it is trusted ---------------
	if (!TestTrue(TEXT("the board is given an overlay mesh, without which every overlay "
	                   "accessor reads 0 for every input forever"),
			GiveTheBoardAnOverlayMesh(Board, Error)))
	{
		AddError(Error);
		return false;
	}

	UHierarchicalInstancedStaticMeshComponent* const Ring = ObjectiveOverlayOf(Board, Error);
	if (!TestNotNull(TEXT("the board declares an ObjectiveOverlay component"), Ring))
	{
		AddError(Error);
		return false;
	}

	// ---- a real guided opening writes the whole guidance block --------------
	FStratViewModel Model;
	FString BuildError;
	if (!TestTrue(TEXT("the view model builds"), Subsystem->BuildViewModel(Model, BuildError)))
	{
		AddError(BuildError);
		return false;
	}

	const int32 GuidedSide = Bridge->SideToMove();
	FStratGuidedOpening    Guidance;
	FStratSelectionMachine Machine;
	Guidance.Begin(*Bridge, GuidedSide, /*bSuppressed*/ false);
	if (!TestTrue(TEXT("the shipped scenario arms a guided opening for the seat to move"),
			Guidance.IsActive()))
	{
		return false;
	}
	Guidance.Observe(Model, Machine);
	Guidance.DecorateViewModel(Model);

	if (!TestTrue(TEXT("the guided opening published bActive"), Model.Guidance.bActive) ||
		!TestTrue(TEXT("and published an objective ring, which is this clause's whole subject"),
			Model.Guidance.bHasObjectiveRing))
	{
		return false;
	}

	// ---- lit ----------------------------------------------------------------
	Subsystem->ApplyView(Model);

	TestEqual(
		*FString::Printf(
			TEXT("SS2.11.6-B: an active guidance block carrying a ring lights exactly one hex on "
			     "the objective overlay (the block names axial %s)"),
			*Model.Guidance.ObjectiveHex.ToString()),
		Board->GetObjectiveOverlayCount(), 1);

	TestEqual(
		TEXT("and NOTHING is lit on SS2.6's attack overlay -- the ring must not have been drawn "
		     "on `TargetOverlay`, which is the wrong component semantically and is the one "
		     "`T-UI-02.AttackIsClosedForTheMarkedInfantry` requires to be empty during beat 1a"),
		Board->GetTargetOverlayCount(), 0);

	// The accessor's own claim -- "off the component, not a cached number" -- turned into a
	// measurement rather than left as a sentence.
	TestEqual(
		TEXT("and `GetObjectiveOverlayCount()` agrees with the component it says it reads"),
		Board->GetObjectiveOverlayCount(), Ring->GetInstanceCount());

	if (Ring->GetInstanceCount() == 1)
	{
		FTransform Drawn;
		if (TestTrue(TEXT("the drawn ring instance has a world transform"),
				Ring->GetInstanceTransform(0, Drawn, /*bWorldSpace*/ true)))
		{
			const FVector Expected = Board->WorldLocationOfHex(Model.Guidance.ObjectiveHex);
			const FVector Actual   = Drawn.GetLocation();

			// XY ONLY. `OverlayZOffset` is a phase-5 presentation property and this clause is
			// about WHICH HEX, not how far above the tile plane the ring floats.
			TestTrue(
				*FString::Printf(
					TEXT("and it is drawn at the hex the guidance block named: "
					     "WorldLocationOfHex(axial %s) is (%.1f, %.1f), the instance is at "
					     "(%.1f, %.1f)"),
					*Model.Guidance.ObjectiveHex.ToString(),
					Expected.X, Expected.Y, Actual.X, Actual.Y),
				FMath::IsNearlyEqual(Expected.X, Actual.X, 0.5) &&
				FMath::IsNearlyEqual(Expected.Y, Actual.Y, 0.5));
		}
	}

	// ---- dark, through the module's own path --------------------------------
	// `SkipGuidance` is SS2.11.6's own "it does not return", and `DecorateViewModel` then
	// publishes a default-constructed block. Nothing here writes `bActive` by hand.
	Guidance.SkipGuidance();
	Guidance.DecorateViewModel(Model);
	if (!TestFalse(TEXT("after SkipGuidance the published block is inactive"),
			Model.Guidance.bActive))
	{
		return false;
	}

	Subsystem->ApplyView(Model);

	TestEqual(
		TEXT("SS2.11.6: \"the objective ring ... clear[s] in the same frame as the strip\" -- an "
		     "inactive guidance block leaves the objective overlay EMPTY. This is the half an "
		     "`ApplyView` with a show and no clear turns red, and the lit half above is its "
		     "control: on a board with no overlay mesh both halves would read 0"),
		Board->GetObjectiveOverlayCount(), 0);

	return true;
}

// ---------------------------------------------------------------------------
// The turn-1a marker shows on the VIEWING SEAT'S marked Infantry and on nothing else --
// including after that Infantry has moved off the hex the scenario deployed it on.
//
// THREE PROPERTIES ARE PINNED HERE AND THEY ARE INDEPENDENT. Losing any loses a real defect.
//   (A) THE 2026-08-23 USER RULING. `Data/ferrum_crossing.json` authors a `guidedOpening`
//       entry for side 0 AND for side 1, and `Ui.h` declares `isGuidedMarked` true "on the
//       placement that the scenario file's `guidedOpening.infantry` names for this unit's
//       SEAT", so TWO units carry the bit at once. Until the ruling landed, `ApplyUnitView`
//       passed the bit through unfiltered and the ENEMY seat's Infantry wore the marker on the
//       player's screen -- a "select this" pointing at a unit the player cannot select. The
//       marker is now `View.bIsGuidedMarked && View.Side == ViewingSide`.
//   (B) THE MARK IS NOT KEYED TO A HEX. `bIsGuidedMarked` is DECLARED derived off `placement`
//       and not off the current hex, because beat 1a's entire content is that the marked
//       Infantry MOVES; a hex-keyed re-derivation would unmark it at the exact moment the
//       directive needs it. So the unit is MOVED, THROUGH THE RULES, off
//       `guidedOpening.infantry`, and the marker is required to still be showing afterwards.
//       Nothing else in the tree measures this.
//   (C) THE VIEWING SIDE IS READ OFF THE MODEL AND NOT OFF THE SUBSYSTEM. Frame three applies
//       the SAME model with `FStratViewModel::ViewingSide` alone changed and requires the
//       marker to MOVE to the other seat's Infantry. `UStratMatchSubsystem`'s own member is
//       untouched by that edit, so this is the only frame in which the two sources disagree
//       and the only place in the tree that can tell them apart -- on every other path they
//       agree. `AStratUnitActor::GuidedMarker`'s block forbids the member by name for
//       T-INT-05's reason; this is that sentence turned into a measurement.
//
// THE FIXTURE CONTAINS THE RULING'S DISCRIMINATING PAIR, AND THAT IS ASSERTED RATHER THAN
// HOPED FOR. Three populations are enumerated off the applied model and EACH IS REQUIRED TO BE
// NON-EMPTY, so no case can go vacuous in silence:
//   1. marked, on the viewing side      -> the marker MUST show   (unit 3 on this scenario)
//   2. marked, NOT on the viewing side  -> the marker MUST NOT show (unit 7) -- THE RULING
//   3. not marked                       -> the marker MUST NOT show
// 1 and 2 DIFFER IN EXACTLY ONE FIELD -- `FStratUnitView::Side` -- and the clause asserts that
// too: both carry `bIsGuidedMarked` true and their sides differ. A positive and a negative
// subject separated by one field is what makes this stronger than the unfiltered clause it
// replaces, which had no negative subject that was marked at all.
//
// WHY THE EXPECTATION IS AN EXPRESSION HERE, WHICH IS THE ONE SHAPE THIS PROJECT NORMALLY
// FORBIDS. No module-side value produces "should this actor's marker be lit" -- the ruling IS
// the conjunction, and the only cached answer available is `AStratUnitActor::LastAppliedView`,
// which is exactly what `IsGuidedMarkerVisible`'s declaration refuses to answer from, because
// a clause reading it would report what the actor was TOLD and pass whether or not anything
// reached the component. So both operands are read off the applied model -- neither is derived,
// neither is a literal, and no hex is compared -- and the clause earns its keep through the
// three-population coverage above rather than through the conjunction itself. This is the same
// standing `T-INT-05.ConclusionIsReadFromHasResultAndNotFromResultTier` records for its planted
// pair: when the INDEPENDENCE of two fields is the subject, the pairing is what isolates which
// field the code consults.
//
// BOTH DIRECTIONS, IN BOTH FRAMES. `ApplyUnitView` writes the visibility unconditionally in
// both directions on every call; a writer that only ever SHOWS passes a one-directional clause
// and leaves a permanent marker on a unit SS2.11.6's window closed on.
//
// NO MARKER MESH IS ASSIGNED, AND THAT IS SETTLED RATHER THAN A GAMBLE.
// `USceneComponent::IsVisible` consults `bHiddenInGame`, the visible flag and the cached level
// collection -- and NOT the static mesh. `IsGuidedMarkerVisible`'s declaration said otherwise
// until 2026-08-23 and now carries that sentence under `RETRACTED>` with this measurement
// attached. The discrimination is therefore available on unconfigured actors and is MEASURED
// here (a true and a false in the same frame) rather than assumed. WHAT THIS CLAUSE
// CONSEQUENTLY DOES NOT PIN is that anything is on screen: the accessor reports a FLAG, not
// pixels, and nothing headless can gate the marker's mesh. That limit is in the header now.
//
// THE VIEWING SIDE IS `FStratViewModel::ViewingSide`, off the same model the views came from,
// which is what `ApplyUnitView` reads. This clause deliberately does NOT compare against
// `UStratMatchSubsystem`'s own member: that member is a SECOND INPUT into what should be on
// screen, T-INT-05 forbids exactly that, and a clause that checked the actor against it would
// pass a stale-member defect the production code was rearranged to make impossible.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidedMarkerFollowsTheMarkedBitTest,
	"Stratocracy.StratPlay.T-UI-02.GuidedMarkerFollowsTheMarkedBitAndNotTheHex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidedMarkerFollowsTheMarkedBitTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidedOpeningVisuals;

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
	FString Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	FString StartReason;
	Subsystem->StartMatch(Config, StartReason);
	if (!TestTrue(TEXT("the match is live whatever StartMatch returned"), Subsystem->IsMatchLive()))
	{
		AddError(StartReason);
		return false;
	}

	FStratBridge* const Bridge = Subsystem->GetBridge();
	if (!TestNotNull(TEXT("a live match owns a bridge"), Bridge))
	{
		return false;
	}

	FStratViewModel Model;
	FString BuildError;
	if (!TestTrue(TEXT("the view model builds"), Subsystem->BuildViewModel(Model, BuildError)))
	{
		AddError(BuildError);
		return false;
	}

	// ---- guidance is ARMED, which this clause used to leave implicit --------
	// ADDED 2026-08-24, AND IT IS A MISSING PREMISE RATHER THAN A CONCESSION. `ApplyUnitView`
	// gained `FStratViewModel::Guidance.bActive` as its FIRST operand that morning, closing the
	// latch a human playtest found. This clause built its model with `BuildViewModel` and never
	// armed a guided opening, so `Model.Guidance` was default-constructed and `bActive` was
	// FALSE in every frame -- and the six positive assertions below went red on a tree that is
	// correct. `Stratocracy.StratPlay.T-UI-02.GuidedMarkerFollowsTheMarkedBitAndNotTheHex`
	// failed at `reportCreatedOn 2026.08.24-14.34.09` with *"exactly one marker is lit on the
	// whole board, on unit 3' to be 1, but it was 0"*.
	//
	// NOT ONE ASSERTION MOVED AND NOTHING WAS RELAXED. What is added is the premise the clause
	// always depended on and never stated: guidance is RUNNING while the marker is expected to
	// show. The three properties (A), (B) and (C) above are untouched, and the arming is done
	// the way the ring clause does it -- a real `FStratGuidedOpening` on the subsystem's own
	// bridge writing the whole block through `DecorateViewModel`, with no guidance field typed
	// out here. The bit is then ASSERTED true, so a future `DecorateViewModel` that stopped
	// publishing it reddens this clause at a premise instead of silently emptying it.
	//
	// WHETHER THE MARKER GOES OUT WHEN THAT BIT FALLS IS DELIBERATELY NOT THIS CLAUSE'S SUBJECT.
	// Three clauses at the end of this file own that direction.
	const int32            GuidedSide = Bridge->SideToMove();
	FStratGuidedOpening    Guidance;
	FStratSelectionMachine Machine;
	Guidance.Begin(*Bridge, GuidedSide, /*bSuppressed*/ false);
	if (!TestTrue(TEXT("the shipped scenario arms a guided opening for the seat to move"),
			Guidance.IsActive()))
	{
		return false;
	}
	Guidance.Observe(Model, Machine);
	Guidance.DecorateViewModel(Model);
	if (!TestTrue(
			TEXT("guidance is RUNNING in the frames this clause expects a marker in -- "
			     "`ApplyUnitView`'s first operand is `FStratViewModel::Guidance.bActive`, so "
			     "without this premise every positive assertion below is red on a correct tree"),
			Model.Guidance.bActive))
	{
		return false;
	}

	// ---- the three populations, enumerated off the model --------------------
	FMarkerCensus Census;
	Census.TakeFrom(Model);

	if (!TestEqual(
			*FString::Printf(
				TEXT("the viewing seat has exactly one marked Infantry (side %d; the model marks "
				     "%d unit(s) in total, one per authored `guidedOpening` seat)"),
				Model.ViewingSide, Census.MarkedOnViewingSide.Num() +
					Census.MarkedOffViewingSide.Num()),
			Census.MarkedOnViewingSide.Num(), 1))
	{
		return false;
	}
	if (!TestTrue(
			TEXT("AND THE RULING'S DISCRIMINATING CASE EXISTS: some OTHER seat also has a marked "
			     "Infantry. Without it this clause has no marked negative subject and the side "
			     "filter is unfalsifiable here"),
			Census.MarkedOffViewingSide.Num() > 0))
	{
		return false;
	}
	if (!TestTrue(
			TEXT("and the board carries unmarked units too, so the plain false direction is not "
			     "satisfied by an empty set"),
			Census.Unmarked.Num() > 0))
	{
		return false;
	}

	const int32 MarkedId      = Census.MarkedOnViewingSide[0];
	const int32 EnemyMarkedId = Census.MarkedOffViewingSide[0];

	// ---- the pair differs in EXACTLY ONE FIELD ------------------------------
	{
		const FStratUnitView* const Mine  = UnitById(Model, MarkedId);
		const FStratUnitView* const Theirs    = UnitById(Model, EnemyMarkedId);
		if (!TestNotNull(TEXT("the viewing seat's marked unit is in the model"), Mine) ||
			!TestNotNull(TEXT("the other seat's marked unit is in the model"), Theirs))
		{
			return false;
		}
		TestTrue(
			*FString::Printf(
				TEXT("units %d and %d BOTH carry `bIsGuidedMarked` -- the pair is separated by "
				     "one field and not by two"),
				MarkedId, EnemyMarkedId),
			Mine->bIsGuidedMarked && Theirs->bIsGuidedMarked);
		TestTrue(
			*FString::Printf(
				TEXT("and that one field is `Side`: unit %d is on side %d, unit %d on side %d, "
				     "and the model is viewed from side %d"),
				MarkedId, Mine->Side, EnemyMarkedId, Theirs->Side, Model.ViewingSide),
			Mine->Side != Theirs->Side && Mine->Side == Model.ViewingSide);
	}

	// ---- fixture premise for the move step ----------------------------------
	// THE VIEWING SEAT MUST ALSO BE THE SEAT TO MOVE, or the move below is refused for a
	// reason that has nothing to do with a marker. Asserted rather than assumed of the
	// config, which sets `FirstSide` and `ViewingSide` independently.
	if (!TestEqual(
			TEXT("the viewing seat is the seat to move, so its Infantry can legally be moved"),
			Model.ViewingSide, Bridge->SideToMove()))
	{
		return false;
	}

	// ---- the premise: it is still standing on its placement hex -------------
	// READ FROM `FStratBridge::GuidedOpeningHexes`, the lookup of `guidedOpening.infantry`,
	// so the "moved off it" assertion further down is against the AUTHORED hex and not
	// against wherever the unit happened to be a moment ago.
	FIntPoint AuthoredInfantryHex  = FIntPoint::ZeroValue;
	FIntPoint AuthoredObjectiveHex = FIntPoint::ZeroValue;
	const FStratResult Guided =
		Bridge->GuidedOpeningHexes(Model.ViewingSide, AuthoredInfantryHex, AuthoredObjectiveHex);
	if (!TestTrue(TEXT("the scenario names a guided opening for the viewing seat"), Guided.bOk))
	{
		AddError(Guided.Reason);
		return false;
	}

	const FStratUnitView* const Before = UnitById(Model, MarkedId);
	if (!TestNotNull(TEXT("the marked unit is in the model"), Before))
	{
		return false;
	}
	if (!TestTrue(
			*FString::Printf(
				TEXT("and it starts on the hex the scenario deployed it on, axial %s -- asserted "
				     "so that \"it moved off it\" below is a real change"),
				*AuthoredInfantryHex.ToString()),
			Before->Hex == AuthoredInfantryHex))
	{
		return false;
	}

	// ---- frame one: the ruling, before the move -----------------------------
	Subsystem->ApplyView(Model);

	TArray<AStratUnitActor*> Actors;
	LiveUnitActors(Scope.World, Actors);
	if (!TestTrue(TEXT("ApplyView put unit actors in the world"), Actors.Num() > 0))
	{
		return false;
	}

	int32 SawLit = 0;
	int32 SawEnemyMarked = 0;
	for (AStratUnitActor* const Actor : Actors)
	{
		const int32 Id = Actor->GetUnitId();
		const bool bLit = Actor->IsGuidedMarkerVisible();
		if (bLit)
		{
			++SawLit;
		}

		if (Id == MarkedId)
		{
			TestTrue(
				*FString::Printf(
					TEXT("before the move: the VIEWING seat's marked Infantry, unit %d on side "
					     "%d, wears the turn-1a marker"),
					Id, Model.ViewingSide),
				bLit);
		}
		else if (Id == EnemyMarkedId)
		{
			++SawEnemyMarked;
			TestFalse(
				*FString::Printf(
					TEXT("THE 2026-08-23 RULING: unit %d carries `bIsGuidedMarked` and is NOT on "
					     "the viewing side %d, so it must NOT wear the marker. This is the "
					     "assertion an unfiltered pass-through turns red, and it is the defect "
					     "that shipped -- a \"select this\" on a unit the player cannot select"),
					Id, Model.ViewingSide),
				bLit);
		}
		else
		{
			TestFalse(
				*FString::Printf(
					TEXT("before the move: unit %d carries no mark and wears no marker"), Id),
				bLit);
		}
	}
	TestEqual(TEXT("and the other seat's marked unit really did reach the board, so the ruling's "
	               "assertion above was actually evaluated"),
		SawEnemyMarked, 1);
	TestEqual(
		*FString::Printf(
			TEXT("exactly one marker is lit on the whole board, on unit %d"), MarkedId),
		SawLit, 1);

	// ---- the move, through the rules ---------------------------------------
	TArray<FIntPoint> Reach;
	TArray<int32>     Costs;
	const FStratResult Reachable = Bridge->ReachableHexes(MarkedId, Reach, Costs);
	if (!TestTrue(TEXT("the module answers where the marked Infantry may go"), Reachable.bOk))
	{
		AddError(Reachable.Reason);
		return false;
	}

	FIntPoint Destination = FIntPoint::ZeroValue;
	bool bHaveDestination = false;
	for (const FIntPoint& Hex : Reach)
	{
		if (Hex != AuthoredInfantryHex)
		{
			Destination      = Hex;
			bHaveDestination = true;
			break;
		}
	}
	if (!TestTrue(
			TEXT("the module offers the marked Infantry somewhere OTHER than its placement hex "
			     "-- the destination is the module's, never this file's"),
			bHaveDestination))
	{
		return false;
	}

	const FStratResult Moved = Bridge->SubmitMoveToHex(MarkedId, Destination);
	if (!TestTrue(
			*FString::Printf(TEXT("the marked Infantry moves to axial %s"),
				*Destination.ToString()),
			Moved.bOk))
	{
		AddError(Moved.Reason);
		return false;
	}

	FStratViewModel AfterModel;
	if (!TestTrue(TEXT("the view model rebuilds after the move"),
			Subsystem->BuildViewModel(AfterModel, BuildError)))
	{
		AddError(BuildError);
		return false;
	}

	// THE REBUILT MODEL IS DECORATED TOO. `BuildViewModel` default-constructs the guidance
	// block, so a rebuild drops `bActive` back to false and frames two and three would run
	// with guidance apparently stopped. `Observe` is deliberately NOT re-run: it advances
	// beats, and this clause is about the marker's operands rather than about a beat machine
	// -- `DecorateViewModel` republishes the same still-active block and changes nothing else.
	Guidance.DecorateViewModel(AfterModel);
	if (!TestTrue(TEXT("guidance is still running after the move, which frames two and three "
	                   "both depend on"),
			AfterModel.Guidance.bActive))
	{
		return false;
	}

	const FStratUnitView* const After = UnitById(AfterModel, MarkedId);
	if (!TestNotNull(TEXT("the marked unit survived its own move"), After))
	{
		return false;
	}

	// ---- the module's own answer about a unit off its placement hex ---------
	if (!TestTrue(
			*FString::Printf(
				TEXT("the marked Infantry really is off `guidedOpening.infantry` now: it stands "
				     "at axial %s, the scenario deployed it at axial %s"),
				*After->Hex.ToString(), *AuthoredInfantryHex.ToString()),
			After->Hex != AuthoredInfantryHex))
	{
		return false;
	}
	TestTrue(
		*FString::Printf(
			TEXT("and the RULES MODULE still marks it: `bIsGuidedMarked` is derived off "
			     "`placement`, so unit %d keeps the mark after leaving axial %s. A hex-keyed "
			     "derivation anywhere in the chain turns this red"),
			MarkedId, *AuthoredInfantryHex.ToString()),
		After->bIsGuidedMarked);

	// ---- frame two: PROPERTY (B), and the ruling again ----------------------
	// THE CENSUS IS RETAKEN off the REBUILT model rather than reused. The move changed the
	// board, and a clause that carried frame one's classification forward would be asserting
	// against a snapshot the actors were not applied from.
	FMarkerCensus CensusAfter;
	CensusAfter.TakeFrom(AfterModel);
	if (!TestTrue(
			TEXT("the viewing seat still has its marked Infantry after the move, and the other "
			     "seat still has its own -- the ruling's pair survives the move"),
			CensusAfter.MarkedOnViewingSide.Contains(MarkedId) &&
			CensusAfter.MarkedOffViewingSide.Contains(EnemyMarkedId)))
	{
		return false;
	}

	Subsystem->ApplyView(AfterModel);
	LiveUnitActors(Scope.World, Actors);

	int32 SawLitAfter = 0;
	bool  bFoundMarkedActor = false;
	for (AStratUnitActor* const Actor : Actors)
	{
		const int32 Id = Actor->GetUnitId();
		const bool bLit = Actor->IsGuidedMarkerVisible();
		if (bLit)
		{
			++SawLitAfter;
		}

		if (Id == MarkedId)
		{
			bFoundMarkedActor = true;
			// PROPERTY (B), AND IT IS THE STEP NOTHING ELSE IN THE TREE MAKES.
			TestTrue(
				*FString::Printf(
					TEXT("AFTER THE MOVE: the marked Infantry, unit %d, has left axial %s for "
					     "axial %s and its turn-1a marker is STILL SHOWING. Beat 1a's whole "
					     "content is that this unit MOVES, so a hex-keyed re-derivation "
					     "anywhere in the chain would put the marker out at the exact moment "
					     "the directive needs it"),
					Id, *AuthoredInfantryHex.ToString(), *After->Hex.ToString()),
				bLit);
		}
		else if (Id == EnemyMarkedId)
		{
			TestFalse(
				*FString::Printf(
					TEXT("and the ruling still holds after the move: unit %d is marked, is not "
					     "on side %d, and wears no marker"),
					Id, AfterModel.ViewingSide),
				bLit);
		}
		else
		{
			TestFalse(
				*FString::Printf(
					TEXT("after the move: unit %d carries no mark and wears no marker"), Id),
				bLit);
		}
	}
	TestTrue(TEXT("the marked Infantry still has an actor on the board to have measured"),
		bFoundMarkedActor);
	TestEqual(
		*FString::Printf(
			TEXT("and exactly one marker is lit after the move, still on unit %d"), MarkedId),
		SawLitAfter, 1);

	// ---- frame three: THE SAME MODEL, VIEWED FROM THE OTHER SEAT ------------
	// THIS IS THE RULING MEASURED IN BOTH DIRECTIONS ON REAL ACTORS, AND IT IS WHY THIS
	// CLAUSE NEEDS NO ARGUMENT ABOUT WHAT A REMOVED FILTER WOULD DO. Between frame two and
	// frame three exactly ONE FIELD of the model changes -- `FStratViewModel::ViewingSide` --
	// and the marker must MOVE from unit `MarkedId` to unit `EnemyMarkedId`. Every unit, every
	// hex, every `bIsGuidedMarked` bit is byte-identical across the two frames.
	//
	// WHAT EACH WRONG IMPLEMENTATION DOES TO IT, and there is no state of the tree in which
	// all of it passes:
	//   - THE FILTER REMOVED (the pre-ruling pass-through): BOTH marked units are lit in
	//     BOTH frames. The enemy `TestFalse` in frames one and two fires, and both
	//     "exactly one marker is lit" counts read 2.
	//   - THE FILTER INVERTED, or widened so a marked unit on the viewing seat is hidden:
	//     frames one and two fire on the viewing seat's own unit, and this frame fires the
	//     other way.
	//   - THE MARKER KEYED TO THE SUBSYSTEM'S `GetViewingSide` INSTEAD OF THE MODEL'S: this
	//     frame is the only one that can see it. `ApplyView` is handed a model whose
	//     `ViewingSide` differs from the subsystem's member, which is precisely the
	//     second-input divergence `AStratUnitActor::GuidedMarker`'s block forbids, and an
	//     actor reading the member would keep the marker on `MarkedId` here.
	// That last one is worth the whole frame on its own: nothing else in the tree
	// distinguishes the two sources, because on every other path they agree.
	//
	// `SetViewingSide` IS NOT USED, DELIBERATELY. That method rebuilds and re-applies through
	// the subsystem, which would move the member and the model together and destroy exactly
	// the divergence this frame exists to create. The model is edited and applied directly --
	// fixture construction, the same standing `StratMatchReconcile.cpp` records for the models
	// it edits.
	FStratViewModel Swapped = AfterModel;
	const FStratUnitView* const TheirUnit = UnitById(AfterModel, EnemyMarkedId);
	if (!TestNotNull(TEXT("the other seat's marked unit is still in the model"), TheirUnit))
	{
		return false;
	}
	Swapped.ViewingSide = TheirUnit->Side;
	if (!TestNotEqual(
			TEXT("and the swapped viewing side really is a different seat, so this frame is a "
			     "real change and not a repeat of the last one"),
			Swapped.ViewingSide, AfterModel.ViewingSide))
	{
		return false;
	}

	Subsystem->ApplyView(Swapped);
	LiveUnitActors(Scope.World, Actors);

	int32 SawLitSwapped = 0;
	for (AStratUnitActor* const Actor : Actors)
	{
		const int32 Id   = Actor->GetUnitId();
		const bool  bLit = Actor->IsGuidedMarkerVisible();
		if (bLit)
		{
			++SawLitSwapped;
		}

		if (Id == EnemyMarkedId)
		{
			TestTrue(
				*FString::Printf(
					TEXT("VIEWED FROM SEAT %d: unit %d -- marked, and now ON the viewing side -- "
					     "wears the marker. One field of the model moved and the marker moved "
					     "with it"),
					Swapped.ViewingSide, Id),
				bLit);
		}
		else if (Id == MarkedId)
		{
			TestFalse(
				*FString::Printf(
					TEXT("and unit %d -- still marked, still standing where it moved to, but no "
					     "longer on the viewing side %d -- has GONE DARK. This is the direction "
					     "an unfiltered pass-through cannot produce, and the direction a marker "
					     "keyed to `UStratMatchSubsystem::GetViewingSide` rather than to "
					     "`FStratViewModel::ViewingSide` would fail, since that member did not "
					     "move"),
					Id, Swapped.ViewingSide),
				bLit);
		}
		else
		{
			TestFalse(
				*FString::Printf(
					TEXT("viewed from seat %d: unit %d carries no mark and wears no marker"),
					Swapped.ViewingSide, Id),
				bLit);
		}
	}
	TestEqual(
		*FString::Printf(
			TEXT("and still EXACTLY ONE marker is lit on the whole board, now on unit %d -- a "
			     "count of two is what a removed filter reads"),
			EnemyMarkedId),
		SawLitSwapped, 1);

	return true;
}

// ===========================================================================
// THE MARKER'S OTHER DIRECTION -- THREE CLAUSES ADDED 2026-08-24, AND THE HOLE
// THEY CLOSE IS THE ONE A HUMAN PLAYTEST FOUND AND THIS SUITE DID NOT.
//
// WHAT WAS UNMEASURED. Every marker assertion in this file above ran inside a frame where
// guidance was ACTIVE. `AStratUnitActor::ApplyUnitView` set the marker from
// `View.bIsGuidedMarked && View.Side == ViewingSide` -- and BOTH OPERANDS ARE CONSTANT FOR THE
// WHOLE MATCH. `bIsGuidedMarked` is the rules module's derivation off `placement`, deliberately
// and permanently so, because beat 1a's whole content is that the marked Infantry MOVES; `Side`
// and `ViewingSide` do not move either. So the conjunction could never go false once true, the
// marker LATCHED ON, and it stayed lit for the rest of the match with the ring already dark
// beside it. Nothing anywhere asserted the marker goes OUT, so the suite stayed green over a
// defect that was on screen.
//
// THE LESSON THAT OUTLIVES THE DEFECT, and it is why these three exist rather than one. The
// clause above DID assert both directions -- a lit unit and a dark unit in the same frame -- and
// `ApplyUnitView` DID write the visibility unconditionally in both directions on every call.
// Both of those were true while the bug was live. **BOTH DIRECTIONS OVER AN OPERAND SET THAT
// CONTAINS NO FALSE IS ONE DIRECTION.** What was missing was not an `else`, it was a frame in
// which the writer's own input could be false.
//
// WHAT EACH OF THE THREE IS FOR, AND THEY ARE NOT THREE READINGS OF ONE THING.
//   - `GuidedMarkerClearsWhenGuidanceDeactivates` pins WHICH FIELD. One field of one model
//     moves, and in the dark frame the OTHER TWO OPERANDS ARE ASSERTED STILL TRUE -- so the
//     pre-fix conjunction still evaluates true there, and this clause is red on exactly the
//     tree that shipped.
//   - `GuidedMarkerAndObjectiveRingClearInTheSameFrame` pins the TWO SURFACES TOGETHER out of
//     ONE `ApplyView` call. That is SS2.11.6's "clear in the same frame as the strip", which
//     until now was asserted in prose in three files and pinned in none.
//   - `GuidedMarkerGoesOutAfterSkipGuidance` pins the PRODUCTION ROUTE. It writes no model field
//     at all: `FStratGuidedOpening::SkipGuidance` carried a comment claiming this mechanism for
//     a tree that did not implement it, and that comment now carries a `[STAMPED 2026-08-24]`
//     retraction. A sentence in one file about what another file does is a claim; this is the
//     clause that makes it a measurement.
//
// THE NAME. The engineer proposed `GuidedMarkerReturnsNothingAfterSkipGuidance` for the third.
// Renamed to `GuidedMarkerGoesOutAfterSkipGuidance` -- "returns nothing" reads as a claim about
// a return value, and `SkipGuidance` returns `void`. Nothing else moved; the id stays `T-UI-02`.
//
// THE INSTRUMENT LIMIT, RESTATED HERE BECAUSE THESE THREE ARE THE CLAUSES MOST LIKELY TO BE
// OVER-READ. `IsGuidedMarkerVisible` reports the component's VISIBLE FLAG. It returns
// `GuidedMarker->IsVisible()`, and `USceneComponent::IsVisible` consults `bHiddenInGame` and the
// visible flag and NOT the static mesh -- so it answers TRUE for a marked unit whose marker has
// no mesh and draws nothing at all. **NONE OF THESE THREE CLAUSES SAYS ANYTHING ABOUT PIXELS.**
// What they say is that the flag falls when guidance stops. The mesh and the material are
// Blueprint defaults and are the content lane's; nothing headless can gate them.
//
// WHY A HAND-WRITTEN `bActive` IS FIXTURE CONSTRUCTION AND NOT A COMPUTED EXPECTATION. In the
// first two clauses the deactivated frame is made by copying the applied model and assigning
// `Guidance.bActive = false` and nothing else. That is an INPUT, on the same standing as frame
// three's `ViewingSide` edit above and the models `StratMatchReconcile.cpp` edits. The
// EXPECTATION -- the marker is out, the ring is empty -- is read off the actor and off the board
// and is nowhere typed. The third clause does not even do that much: it drives the module.
// ===========================================================================

namespace StratGuidedOpeningVisuals
{
	/**
	 * A live match on the shipped scenario, with the four handles a marker clause needs.
	 *
	 * Lifted verbatim out of the two clauses above rather than invented -- same config, same
	 * world scope, same order -- because three more copies of that preamble is three more
	 * places a fixture premise can drift from the clause it serves.
	 */
	struct FLiveMatch
	{
		FTestWorldScope       Scope;
		UStratMatchSubsystem* Subsystem = nullptr;
		FStratBridge*         Bridge    = nullptr;
		AStratBoardActor*     Board     = nullptr;

		bool Start(FString& OutError)
		{
			if (Scope.World == nullptr)
			{
				OutError = TEXT("no transient world was created");
				return false;
			}

			Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
			if (Subsystem == nullptr)
			{
				OutError = TEXT("the world has no match subsystem");
				return false;
			}

			FStratMatchConfig Config;
			if (!MakeConfig(Config, OutError))
			{
				return false;
			}

			FString StartReason;
			Subsystem->StartMatch(Config, StartReason);
			if (!Subsystem->IsMatchLive())
			{
				OutError = StartReason;
				return false;
			}

			Bridge = Subsystem->GetBridge();
			if (Bridge == nullptr)
			{
				OutError = TEXT("a live match owns no bridge");
				return false;
			}

			Board = Subsystem->GetBoard();
			if (Board == nullptr)
			{
				OutError = TEXT("the subsystem spawned no board");
				return false;
			}
			return true;
		}
	};

	/**
	 * Every unit actor in the world whose turn-1a marker is showing, by unit id.
	 *
	 * A BOARD-WIDE CENSUS AND NOT A PREDICATE ON ONE ACTOR, which is what makes "the marker
	 * went out" a statement about the BOARD rather than about the one unit a clause remembered
	 * to look at. A writer that hid the marked Infantry and left a marker standing on some
	 * other actor reads as 1 here, not as 0, and the clause names the offender in its message.
	 */
	static int32 LitMarkerIds(UWorld* World, TArray<int32>& OutIds)
	{
		OutIds.Reset();

		TArray<AStratUnitActor*> Actors;
		LiveUnitActors(World, Actors);
		for (AStratUnitActor* const Actor : Actors)
		{
			if (Actor->IsGuidedMarkerVisible())
			{
				OutIds.Add(Actor->GetUnitId());
			}
		}
		return OutIds.Num();
	}

	/** `TArray<int32>` as "3, 7", so a red run names what it found rather than only a count. */
	static FString IdsAsText(const TArray<int32>& Ids)
	{
		if (Ids.Num() == 0)
		{
			return TEXT("none");
		}

		FString Text;
		for (int32 Index = 0; Index < Ids.Num(); ++Index)
		{
			if (Index > 0)
			{
				Text += TEXT(", ");
			}
			Text += FString::FromInt(Ids[Index]);
		}
		return Text;
	}

	/**
	 * Arms a real `FStratGuidedOpening` on a live match's own bridge and lets it write the
	 * whole guidance block into `Model`.
	 *
	 * NOT ONE GUIDANCE FIELD IS TYPED OUT BY A CALLER. Same posture as the ring clause above:
	 * the block is the module's, written through `DecorateViewModel` exactly as
	 * `AStratPlayerController` has it written in the running game.
	 */
	static bool ArmGuidance(FLiveMatch& Match, FStratGuidedOpening& Guidance,
		FStratSelectionMachine& Machine, FStratViewModel& Model, FString& OutError)
	{
		if (!Match.Subsystem->BuildViewModel(Model, OutError))
		{
			return false;
		}

		Guidance.Begin(*Match.Bridge, Match.Bridge->SideToMove(), /*bSuppressed*/ false);
		if (!Guidance.IsActive())
		{
			OutError = TEXT("the shipped scenario armed no guided opening for the seat to move");
			return false;
		}

		Guidance.Observe(Model, Machine);
		Guidance.DecorateViewModel(Model);
		if (!Model.Guidance.bActive)
		{
			OutError = TEXT("DecorateViewModel published an inactive guidance block");
			return false;
		}
		return true;
	}
}

// ---------------------------------------------------------------------------
// THE TURN-1a MARKER GOES OUT WHEN GUIDANCE STOPS RUNNING -- AND THE TWO OPERANDS THAT USED TO
// BE THE WHOLE CONDITION ARE ASSERTED STILL TRUE IN THE FRAME WHERE IT IS OUT.
//
// THAT SECOND HALF IS THE CLAUSE. Without it this is a clause that could be satisfied by the
// rules module having quietly dropped `bIsGuidedMarked`, or by the seat having changed -- either
// of which would put the marker out for a reason SS2.11.6 does not want and would take beat 1a's
// mark with it. With it, the dark frame is one in which `View.bIsGuidedMarked` is TRUE and
// `View.Side == FStratViewModel::ViewingSide` is TRUE, so the pre-2026-08-24 expression
// evaluates TRUE there and the shipped tree of that morning reads this assertion as red. It is
// the only clause in the tree with that property.
//
// EXACTLY ONE FIELD SEPARATES THE TWO FRAMES. The dark model is a COPY of the lit one with
// `Guidance.bActive` assigned false and nothing else touched -- every unit, every hex, every
// `bIsGuidedMarked` bit, `ViewingSide` itself. That is asserted rather than arranged and hoped
// for: the marked unit's view is re-read out of the DARK model and its two old operands are
// checked there.
//
// THE LIT FRAME IS THE CONTROL AND IT IS NOT DECORATION. A marker that never lit at all reads
// dark in the second frame and would pass a clause that only looked at the second frame. Having
// shown exactly one lit marker on this same board in this same clause is what gives the zero its
// meaning -- the same standing the ring clause above records for its own mute-instrument trap.
//
// WHAT THIS DOES NOT PIN: that anything is on screen. See the block above; the accessor reports
// a FLAG. And it does not pin WHY `bActive` fell -- that is
// `GuidedMarkerGoesOutAfterSkipGuidance` below, which drives the module instead of writing the
// field.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidedMarkerClearsWhenGuidanceDeactivatesTest,
	"Stratocracy.StratPlay.T-UI-02.GuidedMarkerClearsWhenGuidanceDeactivates",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidedMarkerClearsWhenGuidanceDeactivatesTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidedOpeningVisuals;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FLiveMatch Match;
	FString    Error;
	if (!TestTrue(TEXT("a live match starts on the shipped scenario"), Match.Start(Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel        Lit;
	FStratGuidedOpening    Guidance;
	FStratSelectionMachine Machine;
	if (!TestTrue(TEXT("a real guided opening writes the whole guidance block"),
			ArmGuidance(Match, Guidance, Machine, Lit, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- the fixture premise: the seat being viewed is the guided seat -------
	// Asserted rather than assumed of the config, which sets `FirstSide` and `ViewingSide`
	// independently. Without it the marked-on-the-viewing-side population below can be empty
	// and every assertion in this clause goes vacuous.
	if (!TestEqual(TEXT("the viewing seat is the seat guidance was armed for"),
			Lit.ViewingSide, Match.Bridge->SideToMove()))
	{
		return false;
	}

	FMarkerCensus Census;
	Census.TakeFrom(Lit);
	if (!TestEqual(
			TEXT("the viewing seat has exactly one marked Infantry to watch go dark"),
			Census.MarkedOnViewingSide.Num(), 1))
	{
		return false;
	}
	const int32 MarkedId = Census.MarkedOnViewingSide[0];

	// ---- frame one: guidance running. THE CONTROL ---------------------------
	Match.Subsystem->ApplyView(Lit);

	TArray<int32> LitIds;
	LitMarkerIds(Match.Scope.World, LitIds);
	if (!TestTrue(
			*FString::Printf(
				TEXT("WITH GUIDANCE RUNNING the viewing seat's marked Infantry, unit %d, wears the "
				     "turn-1a marker -- this is the control, and without it the dark frame below "
				     "would pass over a marker that never lit at all (lit: %s)"),
				MarkedId, *IdsAsText(LitIds)),
			LitIds.Contains(MarkedId)))
	{
		return false;
	}
	TestEqual(
		*FString::Printf(TEXT("and it is the only marker lit on the board (lit: %s)"),
			*IdsAsText(LitIds)),
		LitIds.Num(), 1);

	// ---- frame two: ONE FIELD MOVES -----------------------------------------
	FStratViewModel Dark = Lit;
	Dark.Guidance.bActive = false;

	// THE ASSERTION THIS WHOLE CLAUSE EXISTS FOR. In the frame where the marker must be out,
	// the two operands that WERE the entire condition are still true. A tree that reads only
	// those two cannot produce a dark marker here, which is exactly the tree that shipped on
	// the morning of 2026-08-24 and exactly what the human at the keyboard saw.
	const FStratUnitView* const Marked = UnitById(Dark, MarkedId);
	if (!TestNotNull(TEXT("the marked unit survives into the deactivated model"), Marked))
	{
		return false;
	}
	if (!TestTrue(
			*FString::Printf(
				TEXT("IN THE DEACTIVATED FRAME the old operands are BOTH STILL TRUE: unit %d still "
				     "carries `bIsGuidedMarked` (the rules module derives it off `placement`, which "
				     "does not move) and is still on the viewing side %d. So "
				     "`bIsGuidedMarked && Side == ViewingSide` -- the whole condition before "
				     "2026-08-24 -- evaluates TRUE right here"),
				MarkedId, Dark.ViewingSide),
			Marked->bIsGuidedMarked && Marked->Side == Dark.ViewingSide))
	{
		return false;
	}
	if (!TestFalse(TEXT("and the ONE field that moved is `Guidance.bActive`"),
			Dark.Guidance.bActive))
	{
		return false;
	}
	if (!TestEqual(TEXT("nothing else moved: the viewing side is the same seat it was"),
			Dark.ViewingSide, Lit.ViewingSide))
	{
		return false;
	}
	if (!TestEqual(TEXT("and the same units are in the model"), Dark.Units.Num(), Lit.Units.Num()))
	{
		return false;
	}

	Match.Subsystem->ApplyView(Dark);

	TArray<int32> StillLit;
	LitMarkerIds(Match.Scope.World, StillLit);
	TestEqual(
		*FString::Printf(
			TEXT("SS2.11.6: with guidance no longer running NO turn-1a marker is showing anywhere "
			     "on the board. A marker keyed only on `bIsGuidedMarked && Side == ViewingSide` "
			     "LATCHES ON -- both operands are match-constant -- and reads %s here"),
			*IdsAsText(StillLit)),
		StillLit.Num(), 0);

	return true;
}

// ---------------------------------------------------------------------------
// ONE `ApplyView` TAKES BOTH SURFACES DOWN -- SS2.11.6's "the objective ring ... and the turn-1a
// unit marker clear in the same frame as the strip", WHICH UNTIL NOW WAS PROSE IN THREE FILES
// AND A GATE IN NONE.
//
// WHAT "THE SAME FRAME" MEANS HERE AND WHAT IT DOES NOT. It means: between the frame in which
// both surfaces are up and the frame in which both are down there is exactly ONE
// `UStratMatchSubsystem::ApplyView` call, and both are read after it. It does NOT mean anything
// about the STRIP -- `FStratGuidanceView` reaches a UMG widget this module cannot construct
// headlessly, and no clause here observes it. What is pinned is the two surfaces the sentence
// names beside the strip, driven off the one bool the strip is driven off.
//
// WHY THIS IS NOT A SECOND READING OF THE CLAUSE ABOVE. That one passes on a tree where the ring
// is broken, and this one passes on a tree where the ring and the marker fall together one
// refresh LATE. Together they say: the marker falls on `bActive`, and it falls BESIDE the ring
// rather than after it. Losing either half of the AND loses a real defect -- a marker that
// outlives the ring by one frame is precisely what a human sees as "the ring cleared and the
// marker did not", which is the report that started this.
//
// BOTH SURFACES ARE LIT FIRST, AND THAT IS TWO CONTROLS RATHER THAN ONE. The ring's is forced by
// the instrument: `AStratBoardActor::FillOverlay` returns early and silently when the overlay
// component has no static mesh, so on a fixture with no Blueprint defaults every overlay
// accessor reads 0 forever and "the ring is dark" could never go red.
// `GiveTheBoardAnOverlayMesh` is what gives it a voice -- the measurement
// `StratGuidanceInputGates.cpp` took first -- and having read 1 on this same board in this same
// clause is what makes the 0 a measurement. The marker's control is the plain one: a marker that
// never lit reads dark for free.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidedMarkerAndRingClearTogetherTest,
	"Stratocracy.StratPlay.T-UI-02.GuidedMarkerAndObjectiveRingClearInTheSameFrame",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidedMarkerAndRingClearTogetherTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidedOpeningVisuals;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FLiveMatch Match;
	FString    Error;
	if (!TestTrue(TEXT("a live match starts on the shipped scenario"), Match.Start(Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- the ring's instrument is given a voice before it is trusted --------
	if (!TestTrue(TEXT("the board is given an overlay mesh, without which every overlay accessor "
	                   "reads 0 for every input forever and the dark half could not go red"),
			GiveTheBoardAnOverlayMesh(Match.Board, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel        Lit;
	FStratGuidedOpening    Guidance;
	FStratSelectionMachine Machine;
	if (!TestTrue(TEXT("a real guided opening writes the whole guidance block"),
			ArmGuidance(Match, Guidance, Machine, Lit, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("and it published an objective ring, without which this clause has only "
	                   "one surface to watch"),
			Lit.Guidance.bHasObjectiveRing))
	{
		return false;
	}
	if (!TestEqual(TEXT("the viewing seat is the seat guidance was armed for"),
			Lit.ViewingSide, Match.Bridge->SideToMove()))
	{
		return false;
	}

	FMarkerCensus Census;
	Census.TakeFrom(Lit);
	if (!TestEqual(TEXT("the viewing seat has exactly one marked Infantry to watch go dark"),
			Census.MarkedOnViewingSide.Num(), 1))
	{
		return false;
	}
	const int32 MarkedId = Census.MarkedOnViewingSide[0];

	// ---- frame one: BOTH SURFACES UP ----------------------------------------
	Match.Subsystem->ApplyView(Lit);

	TArray<int32> LitIds;
	LitMarkerIds(Match.Scope.World, LitIds);

	const int32 RingWhileRunning    = Match.Board->GetObjectiveOverlayCount();
	const bool  bMarkerWhileRunning = LitIds.Contains(MarkedId);

	if (!TestEqual(
			*FString::Printf(
				TEXT("CONTROL, SURFACE ONE: with guidance running the objective ring is lit at the "
				     "hex the guidance block named, axial %s"),
				*Lit.Guidance.ObjectiveHex.ToString()),
			RingWhileRunning, 1))
	{
		return false;
	}
	if (!TestTrue(
			*FString::Printf(
				TEXT("CONTROL, SURFACE TWO: and unit %d wears the turn-1a marker in that same frame "
				     "(lit: %s). Both surfaces are up, which is what makes the single call below a "
				     "measurement of both going down"),
				MarkedId, *IdsAsText(LitIds)),
			bMarkerWhileRunning))
	{
		return false;
	}

	// ---- ONE CALL. Between these two readings there is exactly one `ApplyView`.
	FStratViewModel Dark = Lit;
	Dark.Guidance.bActive = false;

	Match.Subsystem->ApplyView(Dark);

	const int32   RingAfter = Match.Board->GetObjectiveOverlayCount();
	TArray<int32> StillLit;
	LitMarkerIds(Match.Scope.World, StillLit);

	TestEqual(
		TEXT("SS2.11.6, SURFACE ONE: one `ApplyView` with an inactive guidance block leaves the "
		     "objective overlay EMPTY"),
		RingAfter, 0);
	TestEqual(
		*FString::Printf(
			TEXT("SS2.11.6, SURFACE TWO, OUT OF THE SAME SINGLE `ApplyView`: no turn-1a marker is "
			     "showing anywhere on the board either (still lit: %s). A marker that outlives the "
			     "ring by even one refresh is what the 2026-08-24 playtest reported -- the ring "
			     "cleared and the marker did not"),
			*IdsAsText(StillLit)),
		StillLit.Num(), 0);

	// The conjunction stated as one fact, so a reader of a red run sees which half survived
	// rather than two separate numbers to correlate.
	TestTrue(
		*FString::Printf(
			TEXT("BOTH SURFACES CLEARED IN THE SAME FRAME: ring %d -> %d, markers lit %d -> %d, "
			     "with one `ApplyView` between the two readings"),
			RingWhileRunning, RingAfter, LitIds.Num(), StillLit.Num()),
		RingAfter == 0 && StillLit.Num() == 0);

	return true;
}

// ---------------------------------------------------------------------------
// SS2.11.6's `Skip guidance` CONTROL PUTS THE MARKER OUT -- DRIVEN THROUGH THE MODULE, WITH NO
// MODEL FIELD WRITTEN BY THIS CLAUSE AT ALL.
//
// WHY THIS IS WORTH A THIRD CLAUSE. The two above assign `Guidance.bActive = false` themselves,
// which is fixture construction and is honest, but it means neither would notice a
// `SkipGuidance` that stopped dropping `bActive` -- they would go on measuring a bool they wrote
// themselves. This one calls `SkipGuidance` and `DecorateViewModel` and then reads the board, so
// the whole chain is the module's: the player's control, the block it publishes, the argument
// `UStratMatchSubsystem::ApplyView` forwards, and the visibility the actor sets.
//
// AND BECAUSE THE COMMENT WAS A CLAIM RATHER THAN A SPECIFICATION.
// `FStratGuidedOpening::SkipGuidance` has said since it was written that "the marker clears
// because that same call publishes `bActive` false and the marker is drawn only while guidance
// runs" -- a sentence about what ANOTHER file does, written for a tree in which no file did it.
// It now carries a `[STAMPED 2026-08-24]` note recording that it was false for a day. THIS IS
// THE CLAUSE THAT WOULD HAVE CAUGHT IT, and the general shape is worth more than this instance:
// a file asserting a second file's behaviour in prose has no gate under it until someone writes
// one.
//
// THE PREMISE IS ASSERTED AND IS FATAL. `SkipGuidance` returns early if guidance is not active,
// so a fixture in which it was never armed would exercise nothing while reading exactly like a
// pass. The clause requires an ACTIVE block and a LIT marker first, then requires the block to
// have gone inactive as a result of the call, and only then reads the board.
//
// AND THE MARK ITSELF IS REQUIRED TO SURVIVE. `bIsGuidedMarked` is asserted still true after the
// skip, which is what says the marker went out because guidance stopped rather than because beat
// 1a's mark was collaterally dropped -- the second of those would be a rules-side regression
// wearing this clause's green as cover.
//
// WHAT IT DOES NOT PIN. That the marker is on screen (the accessor is a flag -- see the block
// above). The other two routes out of the window: the turn-4 close and the all-beats-retired
// branch. Both set `bActive = false` in the same statement group as `bHasObjective`, so the
// clauses above cover the CONSEQUENCE for any route; only this one route has its trigger pinned.
// THE RING IS NOT ASKED ABOUT HERE: this clause assigns no `OverlayMesh`, so every overlay
// accessor is MUTE and would read 0 whatever happened. Saying so is cheaper than an assertion
// that cannot fail; the ring's gate is the clause above.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidedMarkerGoesOutAfterSkipGuidanceTest,
	"Stratocracy.StratPlay.T-UI-02.GuidedMarkerGoesOutAfterSkipGuidance",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidedMarkerGoesOutAfterSkipGuidanceTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidedOpeningVisuals;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FLiveMatch Match;
	FString    Error;
	if (!TestTrue(TEXT("a live match starts on the shipped scenario"), Match.Start(Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel        Model;
	FStratGuidedOpening    Guidance;
	FStratSelectionMachine Machine;
	if (!TestTrue(TEXT("a real guided opening writes the whole guidance block"),
			ArmGuidance(Match, Guidance, Machine, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestEqual(TEXT("the viewing seat is the seat guidance was armed for"),
			Model.ViewingSide, Match.Bridge->SideToMove()))
	{
		return false;
	}

	FMarkerCensus Census;
	Census.TakeFrom(Model);
	if (!TestEqual(TEXT("the viewing seat has exactly one marked Infantry to watch go dark"),
			Census.MarkedOnViewingSide.Num(), 1))
	{
		return false;
	}
	const int32 MarkedId = Census.MarkedOnViewingSide[0];

	// ---- the premise: guidance is running and the marker is up --------------
	Match.Subsystem->ApplyView(Model);

	TArray<int32> LitIds;
	LitMarkerIds(Match.Scope.World, LitIds);
	if (!TestTrue(
			*FString::Printf(
				TEXT("BEFORE THE SKIP guidance is running and unit %d wears the turn-1a marker "
				     "(lit: %s). `SkipGuidance` returns early on an inactive opening, so without "
				     "this premise the call below would do nothing and read exactly like a pass"),
				MarkedId, *IdsAsText(LitIds)),
			Guidance.IsActive() && LitIds.Contains(MarkedId)))
	{
		return false;
	}

	// ---- the player's own control, and nothing else -------------------------
	Guidance.SkipGuidance();
	Guidance.DecorateViewModel(Model);

	if (!TestFalse(
			TEXT("`SkipGuidance` dropped `bActive` in the block it published -- asserted because "
			     "this clause writes no model field of its own and would otherwise be measuring a "
			     "call that did nothing"),
			Model.Guidance.bActive))
	{
		return false;
	}

	const FStratUnitView* const Marked = UnitById(Model, MarkedId);
	if (!TestNotNull(TEXT("the marked unit survives the skip"), Marked))
	{
		return false;
	}
	TestTrue(
		*FString::Printf(
			TEXT("and the RULES MODULE still marks unit %d after the skip -- the marker must go "
			     "out because guidance stopped, not because the mark was dropped with it"),
			MarkedId),
		Marked->bIsGuidedMarked);

	Match.Subsystem->ApplyView(Model);

	TArray<int32> StillLit;
	LitMarkerIds(Match.Scope.World, StillLit);
	TestEqual(
		*FString::Printf(
			TEXT("SS2.11.6: after `Skip guidance` NO turn-1a marker is showing anywhere on the "
			     "board (still lit: %s). This is the sentence `FStratGuidedOpening::SkipGuidance` "
			     "asserted about `AStratUnitActor` for a day before anything implemented it"),
			*IdsAsText(StillLit)),
		StillLit.Num(), 0);

	return true;
}
