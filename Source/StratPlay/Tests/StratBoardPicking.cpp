// The gate for AStratBoardActor -- GDD §2.2 / §4.7 Stub 8, under T-UI-02.
//
//   T-UI-02  the board turns an axial coordinate into a world location, names the hex a
//            cursor is on EXACTLY, and highlights only what it is handed.
//
// TWO CLAUSES LIVE HERE AND THEY ARE THE TWO WAYS THIS ACTOR CAN LIE.
//
//   1. `BoardHexRoundTrip`. The board is the only object in the project that maps axial
//      space to world space, and `HexAtInstance` is the only map back. Its header records
//      that the inverse is an INSTANCE-INDEX LOOKUP and never a distance search, because
//      "a second, rounding inverse would disagree with the forward map at tile edges, and
//      disagreeing about which hex the cursor is on is how a player attacks the wrong
//      unit." So this clause does not merely check that the lookup returns *a* hex: it
//      checks that every drawn instance names a hex of the model, that no hex is named
//      twice, that the count matches, and that the instance's own world transform is
//      EQUAL TO `WorldLocationOfHex` for the hex it names. The last of those is what makes
//      "exact, not approximate" a measurement rather than a hope -- an implementation that
//      rounded a location back into hex space would still return a plausible hex, and only
//      the transform comparison catches it.
//
//   2. `ReachOverlayIsNotComputedHere`. `AStratBoardActor::ShowReach` takes a set of
//      hexes; `FStratBridge::Reachable` is where such a set comes from. The header for
//      that bridge method records the substitution it exists to foreclose -- "a
//      hex-distance filter standing in for a real query ... `distance <= move` agrees with
//      the rules on an empty plain and lies everywhere else" -- and the GDD records that
//      the substitution has already happened once. `T-UI-02.NotHexDistance` pins it at the
//      bridge; this clause pins it one layer up, at the only object that could reintroduce
//      it. The instrument is a DELIBERATELY NON-CONTIGUOUS reach set: a board that filled
//      in a disc, or that filtered the input by distance from anything, cannot draw
//      exactly those hexes and nothing else.
//
// WHERE THE EXPECTATIONS COME FROM, since that is the only interesting column.
//   - The hex list is `FStratViewModel::Hexes`, built by `StratBuildViewModel` from a
//     bridge seeded with the shipped scenario. Not one coordinate is written down here.
//   - Every expected world location is `Board->WorldLocationOfHex(...)`. THE AXIAL -> WORLD
//     FORMULA IS NOT REPEATED IN THIS FILE, and must never be: a test that re-derived
//     `HexSize * (q + r/2)` would be asserting its own arithmetic, would pass while the
//     board was wrong in the same way, and would fail on a `HexSize` change that broke
//     nothing. The forward map is READ and round-tripped.
//   - The hex count, the terrain ids and the number of sides all come from the model.
//     `99`, `11x9` and `7` appear nowhere below.
//
// THE FIXTURE ASSIGNS MESHES THROUGH REFLECTION, and that is worth its paragraph because
// it looks like a trick. `FallbackTerrainMesh` and `OverlayMesh` are `EditDefaultsOnly`
// properties whose real value is set on a Blueprint default in phase 5, and `ApplyHexes`
// draws nothing for a terrain kind with no mesh (by design -- it reports the gap and keeps
// going). A headless gate has no Blueprint, so with the properties unset every clause here
// would measure an empty board and pass vacuously. The properties are therefore set on the
// SPAWNED INSTANCE through the reflection system, which is a supported read/write of a
// reflected field and touches no production source file. It configures the FIXTURE; it
// never supplies an expectation. `/Engine/BasicShapes/Cube.Cube` is the placeholder --
// automation-test fixtures are one of the two standing exceptions to the no-path-literal
// rule, and this one does not even name `/Game/`.
//
// NOTHING BELOW ASKS A RULES QUESTION. No `strat::` free function is called -- that is
// LNK2019 outside StratBridge and StratRules, measured 8x -- and the only bridge methods
// used are `LoadDefinitions` and `LoadScenarioFromFile`, both to build a model.
//
// NOT ASSERTED HERE, with reasons:
// - Which mesh or material a terrain kind draws with. That is phase 5's Blueprint
//   configuration, and this file assigns a placeholder precisely so that it is measuring
//   the mapping and not the art.
// - `HexAtWorldLocation`. It is deliberately absent from the class; see the header block
//   there. A test for it would be a test for a method whose non-existence is the feature.
// - The cursor, hover and selection. Phase 4 owns input; this actor exposes two overlays
//   and a lookup, and that is the whole of its surface.

#include "Misc/AutomationTest.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Misc/Paths.h"
#include "UObject/UnrealType.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratViewModel.h"

// The bridge, included from a .cpp and never from a UHT-parsed header -- the constraint
// StratMatchSubsystem.cpp records about itself. A test file is a .cpp, so it may say this.
#include "StratBridge.h"

namespace StratBoardPicking
{
	// The harness pin for which side moves first. Replay.h states that no rule in this
	// project decides it; StratScoreboardParity.cpp, StratBridgeParity.cpp and
	// StratScoreboardHUDSeam.cpp all pin the same value for the same stated reason.
	static const int32 kFirstSide = 0;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/**
	 * Brings a bridge to the seeded opening.
	 *
	 * Duplicated from Source/StratUI/Tests/ rather than shared, which is this project's
	 * existing posture for this helper -- StratScoreboardParity.cpp, StratViewModelParity.cpp
	 * and StratScoreboardHUDSeam.cpp each carry their own copy and say so. Sharing it would
	 * mean a header in a Tests/ directory that three modules include.
	 */
	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		UDataTable* Units = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
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

		const FString ScenarioPath = FPaths::Combine(DataDir(), TEXT("ferrum_crossing.json"));
		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath, kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	/**
	 * A stand-in tile mesh. See the header block on why the fixture assigns one at all.
	 * `/Engine/` rather than `/Game/`, so this does not even touch the project content the
	 * no-path-literal rule is about.
	 */
	static UStaticMesh* PlaceholderMesh()
	{
		return LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	}

	/**
	 * Writes a reflected object property on an instance.
	 *
	 * THE FIXTURE'S ONLY REACH-AROUND, and it is a read/write of a `UPROPERTY` through the
	 * reflection system -- the same surface a details panel uses. It exists because the
	 * properties it sets are configured on a Blueprint default that phase 5 authors and a
	 * headless gate does not have. It never produces a value this file then asserts.
	 */
	static bool SetObjectProperty(UObject* Object, const TCHAR* PropertyName, UObject* Value, FString& OutError)
	{
		if (Object == nullptr)
		{
			OutError = FString::Printf(TEXT("cannot set '%s' on a null object"), PropertyName);
			return false;
		}

		FObjectPropertyBase* const Property =
			FindFProperty<FObjectPropertyBase>(Object->GetClass(), FName(PropertyName));
		if (Property == nullptr)
		{
			OutError = FString::Printf(TEXT("no reflected object property '%s' on '%s'"),
				PropertyName, *Object->GetClass()->GetName());
			return false;
		}

		Property->SetObjectPropertyValue(Property->ContainerPtrToValuePtr<void>(Object), Value);
		return true;
	}

	/**
	 * A transient world, with the engine world context `UWorld::DestroyActor` wants.
	 *
	 * The same construction StratScoreboardHUDSeam.cpp uses and for the same reason it
	 * records: without the context, teardown warns "World has no context!", and a gate that
	 * leaves a warning in a green run trains the next reader to skim warnings.
	 */
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

		// THE WORLD GOES FIRST AND THE CONTEXT SECOND, and the order is measured rather than
		// chosen. Tearing the context down first leaves `UWorld::DestroyActor` -- which the
		// match subsystem's `Deinitialize` calls for the board and every unit, from inside
		// `DestroyWorld` -- warning "World has no context!" once per actor. A gate that
		// leaves warnings in a green run trains the next reader to skim warnings.
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

	/** Every instanced-mesh component on the board, tile layers and overlays alike. The
	 *  clauses below tell them apart by INSTANCE COUNT and never by component name, so a
	 *  rename in the production file is not a test failure. */
	static void GatherInstancedComponents(
		const AStratBoardActor* Board,
		TArray<UHierarchicalInstancedStaticMeshComponent*>& Out)
	{
		Out.Reset();
		if (Board != nullptr)
		{
			Board->GetComponents(Out);
		}
	}
}

// ---------------------------------------------------------------------------
// Picking is exact -- T-UI-02's round trip.
//
// THE ROUND TRIP IS RUN IN THE DIRECTION THE CURSOR RUNS IT: from (component, instance
// index), which is what an `FHitResult` carries, back to an axial coordinate. Every
// instance the board drew is walked, and the four things that could be wrong are asserted
// separately so a failure says which:
//   - an instance that maps back to nothing (the parallel array fell behind the adds);
//   - an instance that maps back to a hex the model does not have (it mapped to garbage);
//   - two instances mapping back to the same hex (the array is off by one somewhere);
//   - an instance whose transform is not `WorldLocationOfHex` of the hex it named (the
//     drawn tile and the published forward map disagree, which is the tile-edge
//     disagreement the class's header refuses to permit).
// The last one is the one that makes "exact, not approximate" checkable.
//
// AND THE TWO NEGATIVES, which the brief names and which matter more than they look. A
// foreign component and an out-of-range index must both come back FALSE and must leave the
// caller's hex ALONE. A lookup that returned a plausible-looking hex for either would put
// the cursor on a tile it is not over, and phase 4 would submit a move to it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBoardHexRoundTripTest,
	"Stratocracy.StratPlay.T-UI-02.BoardHexRoundTrip",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBoardHexRoundTripTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBoardPicking;

	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Model;
	if (!TestTrue(TEXT("the view model builds from the seeded bridge"),
			StratBuildViewModel(Bridge, 0, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the shipped scenario has hexes to draw"), Model.Hexes.Num() > 0))
	{
		return false;
	}

	UStaticMesh* const Mesh = PlaceholderMesh();
	if (!TestNotNull(TEXT("a placeholder tile mesh loaded"), Mesh))
	{
		return false;
	}

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created to spawn a board into"), Scope.World))
	{
		return false;
	}

	AStratBoardActor* const Board = Scope.World->SpawnActor<AStratBoardActor>();
	if (!TestNotNull(TEXT("the board spawned"), Board))
	{
		return false;
	}
	if (!TestTrue(TEXT("the fixture assigns a fallback tile mesh"),
			SetObjectProperty(Board, TEXT("FallbackTerrainMesh"), Mesh, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- The board is drawn from the model and nothing else ----------------
	FString ApplyReason;
	TestTrue(TEXT("ApplyHexes draws the model's board"), Board->ApplyHexes(Model.Hexes, ApplyReason));
	TestTrue(*FString::Printf(TEXT("a clean rebuild reports no gap (it said: '%s')"), *ApplyReason),
		ApplyReason.IsEmpty());

	// THE COUNT COMES FROM THE MODEL. `99` is not written here, and a scenario with a
	// different board would move both sides of this comparison together.
	TestEqual(TEXT("the board drew one tile per hex the model describes"),
		Board->GetDrawnHexCount(), Model.Hexes.Num());

	TArray<UHierarchicalInstancedStaticMeshComponent*> Components;
	GatherInstancedComponents(Board, Components);
	TestTrue(TEXT("the board created at least one tile component"), Components.Num() > 0);

	// ---- The round trip ----------------------------------------------------
	// Every hex the model carries, so an instance naming a coordinate that is not on this
	// board is a failure rather than a curiosity.
	TSet<FIntPoint> ModelHexes;
	ModelHexes.Reserve(Model.Hexes.Num());
	for (const FStratHexView& HexView : Model.Hexes)
	{
		ModelHexes.Add(HexView.Hex);
	}
	TestEqual(TEXT("the model names each hex once"), ModelHexes.Num(), Model.Hexes.Num());

	TSet<FIntPoint> NamedByAnInstance;
	int32 InstancesWalked = 0;
	int32 DuplicateNames  = 0;

	for (UHierarchicalInstancedStaticMeshComponent* const Component : Components)
	{
		if (Component == nullptr)
		{
			continue;
		}

		const int32 InstanceCount = Component->GetInstanceCount();
		for (int32 Index = 0; Index < InstanceCount; ++Index)
		{
			++InstancesWalked;

			FIntPoint Named = FIntPoint::ZeroValue;
			if (!Board->HexAtInstance(Component, Index, Named))
			{
				AddError(FString::Printf(
					TEXT("instance %d of '%s' is drawn by this board and maps back to no hex"),
					Index, *Component->GetName()));
				continue;
			}

			if (!ModelHexes.Contains(Named))
			{
				AddError(FString::Printf(
					TEXT("instance %d of '%s' maps back to (%d, %d), which is not a hex of the model"),
					Index, *Component->GetName(), Named.X, Named.Y));
				continue;
			}

			bool bAlready = false;
			NamedByAnInstance.Add(Named, &bAlready);
			if (bAlready)
			{
				++DuplicateNames;
				AddError(FString::Printf(
					TEXT("hex (%d, %d) is named by more than one instance -- the parallel array has drifted"),
					Named.X, Named.Y));
			}

			// EXACT, NOT APPROXIMATE. The drawn tile's own transform against the board's
			// published forward map for the hex the lookup just named. The formula is not
			// repeated here; `WorldLocationOfHex` is asked.
			FTransform InstanceTransform;
			if (Component->GetInstanceTransform(Index, InstanceTransform, /*bWorldSpace*/ true))
			{
				const FVector Expected = Board->WorldLocationOfHex(Named);
				const FVector Actual   = InstanceTransform.GetLocation();
				if (!Actual.Equals(Expected, 0.01))
				{
					AddError(FString::Printf(
						TEXT("the tile drawn for hex (%d, %d) sits at %s, but WorldLocationOfHex says %s"),
						Named.X, Named.Y, *Actual.ToString(), *Expected.ToString()));
				}
			}
			else
			{
				AddError(FString::Printf(TEXT("instance %d of '%s' has no readable transform"),
					Index, *Component->GetName()));
			}
		}
	}

	TestEqual(TEXT("every instance the board drew was walked"), InstancesWalked, Model.Hexes.Num());
	TestEqual(TEXT("no hex was named by two instances"), DuplicateNames, 0);
	TestEqual(TEXT("every hex of the model is named by exactly one instance"),
		NamedByAnInstance.Num(), Model.Hexes.Num());

	// The set difference, reported by name rather than as a count, so a failure says WHICH
	// hex went missing instead of only that one did.
	for (const FIntPoint& Hex : ModelHexes)
	{
		if (!NamedByAnInstance.Contains(Hex))
		{
			AddError(FString::Printf(TEXT("hex (%d, %d) is in the model and no instance names it"),
				Hex.X, Hex.Y));
		}
	}

	// ---- The two negatives -------------------------------------------------
	// A sentinel no board could produce, so "left alone" is distinguishable from "written
	// with a plausible value".
	const FIntPoint Sentinel(-9999, -9999);

	UHierarchicalInstancedStaticMeshComponent* MineWithInstances = nullptr;
	for (UHierarchicalInstancedStaticMeshComponent* const Component : Components)
	{
		if (Component != nullptr && Component->GetInstanceCount() > 0)
		{
			MineWithInstances = Component;
			break;
		}
	}
	if (!TestNotNull(TEXT("this board has a tile component carrying instances"), MineWithInstances))
	{
		return false;
	}

	{
		FIntPoint Out = Sentinel;
		TestFalse(TEXT("an index past the end of a component this board owns is not on the board"),
			Board->HexAtInstance(MineWithInstances, MineWithInstances->GetInstanceCount(), Out));
		TestTrue(TEXT("...and the caller's hex was left alone"), Out == Sentinel);
	}
	{
		FIntPoint Out = Sentinel;
		TestFalse(TEXT("a negative index is not on the board"),
			Board->HexAtInstance(MineWithInstances, -1, Out));
		TestTrue(TEXT("...and the caller's hex was left alone"), Out == Sentinel);
	}
	{
		FIntPoint Out = Sentinel;
		TestFalse(TEXT("a null component is not on the board"),
			Board->HexAtInstance(nullptr, 0, Out));
		TestTrue(TEXT("...and the caller's hex was left alone"), Out == Sentinel);
	}

	// A SECOND, FULLY DRAWN BOARD, so the foreign component is not an empty stand-in but a
	// component carrying instances at index 0 -- exactly the "plausible-looking" case. A
	// lookup that keyed off the index alone would answer confidently and wrongly.
	{
		AStratBoardActor* const Other = Scope.World->SpawnActor<AStratBoardActor>();
		if (TestNotNull(TEXT("a second board spawned to supply a foreign component"), Other))
		{
			FString OtherError;
			if (TestTrue(TEXT("the second board is given a mesh too"),
					SetObjectProperty(Other, TEXT("FallbackTerrainMesh"), Mesh, OtherError)))
			{
				FString OtherReason;
				Other->ApplyHexes(Model.Hexes, OtherReason);

				TArray<UHierarchicalInstancedStaticMeshComponent*> OtherComponents;
				GatherInstancedComponents(Other, OtherComponents);

				UHierarchicalInstancedStaticMeshComponent* Foreign = nullptr;
				for (UHierarchicalInstancedStaticMeshComponent* const Component : OtherComponents)
				{
					if (Component != nullptr && Component->GetInstanceCount() > 0)
					{
						Foreign = Component;
						break;
					}
				}

				if (TestNotNull(TEXT("the second board drew instances of its own"), Foreign))
				{
					FIntPoint Out = Sentinel;
					TestFalse(
						TEXT("a component another board owns is not on this board, even at a valid index"),
						Board->HexAtInstance(Foreign, 0, Out));
					TestTrue(TEXT("...and the caller's hex was left alone"), Out == Sentinel);

					// The converse, so the refusal is about OWNERSHIP and not about the
					// component being unusable: the other board answers for its own.
					FIntPoint Theirs = Sentinel;
					TestTrue(TEXT("the board that owns the component does answer for it"),
						Other->HexAtInstance(Foreign, 0, Theirs));
					TestTrue(TEXT("...with a hex of the model"), ModelHexes.Contains(Theirs));
				}
			}
			else
			{
				AddError(OtherError);
			}
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// The reach overlay draws exactly what it was handed -- T-UI-02's second clause.
//
// THE NON-CONTIGUOUS SET IS THE INSTRUMENT. Three hexes taken from opposite ends of the
// model's canonical order, whose axial rows are far enough apart that no two of them are
// neighbours -- asserted below off the coordinates themselves rather than through any
// distance function, because a distance function is the very thing under suspicion. A
// board that derived its highlight from a radius would draw a filled region and fail the
// count; a board that filtered the caller's set by distance from anything would drop hexes
// and fail it the other way. Only a board that draws the set it was handed passes.
//
// AND THE POSITIONS, MATCHED AS A SET. Each drawn overlay instance is paired with the hex
// whose `WorldLocationOfHex` it sits over, and the pairing must be a bijection with the
// input. Matched as a set rather than by index so the clause says nothing about the order
// instances are added in -- that is an implementation detail of `FillOverlay`, and a test
// that pinned it would fail on a harmless change while proving nothing extra.
//
// THE OVERLAY'S Z IS NOT WRITTEN DOWN. `OverlayZOffset` is a presentation property phase 5
// sets; this clause asserts that every instance shares ONE offset from the tile plane,
// which is what "the overlay sits above the tiles" means, and never that the offset is 2.
//
// THE TWO OVERLAYS ARE TOLD APART BY WHICH COMPONENT'S COUNT MOVED, not by name. §2.11.1
// shows a move set and an attack set at once, so the clause also checks that filling one
// leaves the other exactly where it was -- a single component carrying both would be a
// board that could not draw the two phases together, and it would show up here as one
// count clobbering the other.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBoardReachOverlayIsNotComputedHereTest,
	"Stratocracy.StratPlay.T-UI-02.ReachOverlayIsNotComputedHere",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBoardReachOverlayIsNotComputedHereTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBoardPicking;

	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Model;
	if (!TestTrue(TEXT("the view model builds from the seeded bridge"),
			StratBuildViewModel(Bridge, 0, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the board is big enough to pick three separated hexes from"),
			Model.Hexes.Num() >= 3))
	{
		return false;
	}

	UStaticMesh* const Mesh = PlaceholderMesh();
	if (!TestNotNull(TEXT("a placeholder mesh loaded"), Mesh))
	{
		return false;
	}

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	AStratBoardActor* const Board = Scope.World->SpawnActor<AStratBoardActor>();
	if (!TestNotNull(TEXT("the board spawned"), Board))
	{
		return false;
	}
	if (!TestTrue(TEXT("the fixture assigns a fallback tile mesh"),
			SetObjectProperty(Board, TEXT("FallbackTerrainMesh"), Mesh, Error)) ||
		!TestTrue(TEXT("the fixture assigns an overlay mesh"),
			SetObjectProperty(Board, TEXT("OverlayMesh"), Mesh, Error)))
	{
		AddError(Error);
		return false;
	}

	// BeginPlay is what applies `OverlayMesh` to the two overlay components -- the class
	// records why it is not done in the constructor (the properties are Blueprint defaults
	// and a constructor runs on the CDO). `DispatchBeginPlay` is the supported route.
	Board->DispatchBeginPlay();

	FString ApplyReason;
	TestTrue(TEXT("the board draws the model's hexes"), Board->ApplyHexes(Model.Hexes, ApplyReason));

	// ---- The instrument: three hexes that are not neighbours ---------------
	TArray<FIntPoint> Reach;
	Reach.Add(Model.Hexes[0].Hex);
	Reach.Add(Model.Hexes[Model.Hexes.Num() / 2].Hex);
	Reach.Add(Model.Hexes[Model.Hexes.Num() - 1].Hex);

	// NON-CONTIGUITY, ASSERTED OFF THE COORDINATES AND NOT THROUGH A DISTANCE FUNCTION. Two
	// axial hexes whose `r` differs by more than one cannot be adjacent under any hex
	// metric, which is all this clause needs and is a plain field comparison rather than a
	// second opinion about hex geometry.
	for (int32 First = 0; First < Reach.Num(); ++First)
	{
		for (int32 Second = First + 1; Second < Reach.Num(); ++Second)
		{
			TestTrue(*FString::Printf(
					TEXT("the reach set is deliberately non-contiguous: (%d,%d) and (%d,%d) are rows apart"),
					Reach[First].X, Reach[First].Y, Reach[Second].X, Reach[Second].Y),
				FMath::Abs(Reach[First].Y - Reach[Second].Y) > 1);
		}
	}

	TArray<UHierarchicalInstancedStaticMeshComponent*> Components;
	GatherInstancedComponents(Board, Components);
	if (!TestTrue(TEXT("the board has instanced components to inspect"), Components.Num() > 0))
	{
		return false;
	}

	TArray<int32> Before;
	Before.Reserve(Components.Num());
	for (UHierarchicalInstancedStaticMeshComponent* const Component : Components)
	{
		Before.Add(Component != nullptr ? Component->GetInstanceCount() : 0);
	}

	// ---- Fill the reach overlay --------------------------------------------
	Board->ShowReach(Reach);

	int32 ReachComponentIndex = INDEX_NONE;
	int32 ChangedCount = 0;
	for (int32 Index = 0; Index < Components.Num(); ++Index)
	{
		const int32 Now = Components[Index] != nullptr ? Components[Index]->GetInstanceCount() : 0;
		if (Now != Before[Index])
		{
			++ChangedCount;
			ReachComponentIndex = Index;
		}
	}

	if (!TestEqual(TEXT("ShowReach filled exactly one component"), ChangedCount, 1) ||
		!TestTrue(TEXT("that component was found"), Components.IsValidIndex(ReachComponentIndex)))
	{
		return false;
	}

	UHierarchicalInstancedStaticMeshComponent* const ReachOverlay = Components[ReachComponentIndex];

	// THE CLAUSE. Exactly the hexes handed in -- no more (a derived disc), no fewer (a
	// distance filter applied to the caller's set).
	TestEqual(TEXT("the reach overlay draws exactly the hexes it was handed, and no others"),
		ReachOverlay->GetInstanceCount(), Reach.Num());

	// ---- And at exactly those hexes ----------------------------------------
	// A bijection between drawn instances and the input set, established through the
	// board's own forward map. Not one coordinate is converted by this file.
	{
		TArray<bool> Claimed;
		Claimed.Init(false, Reach.Num());
		TArray<double> ZOffsets;

		const int32 DrawnCount = ReachOverlay->GetInstanceCount();
		for (int32 Index = 0; Index < DrawnCount; ++Index)
		{
			FTransform DrawnAt;
			if (!ReachOverlay->GetInstanceTransform(Index, DrawnAt, /*bWorldSpace*/ true))
			{
				AddError(FString::Printf(TEXT("overlay instance %d has no readable transform"), Index));
				continue;
			}
			const FVector Actual = DrawnAt.GetLocation();

			int32 Matched = INDEX_NONE;
			for (int32 Candidate = 0; Candidate < Reach.Num(); ++Candidate)
			{
				if (Claimed[Candidate])
				{
					continue;
				}
				const FVector Expected = Board->WorldLocationOfHex(Reach[Candidate]);
				if (FMath::IsNearlyEqual(Actual.X, Expected.X, 0.01) &&
					FMath::IsNearlyEqual(Actual.Y, Expected.Y, 0.01))
				{
					Matched = Candidate;
					ZOffsets.Add(Actual.Z - Expected.Z);
					break;
				}
			}

			if (Matched == INDEX_NONE)
			{
				AddError(FString::Printf(
					TEXT("overlay instance %d is drawn at %s, which is not WorldLocationOfHex of any hex passed to ShowReach"),
					Index, *Actual.ToString()));
			}
			else
			{
				Claimed[Matched] = true;
			}
		}

		for (int32 Candidate = 0; Candidate < Reach.Num(); ++Candidate)
		{
			TestTrue(*FString::Printf(
					TEXT("hex (%d, %d) was passed to ShowReach and is drawn"),
					Reach[Candidate].X, Reach[Candidate].Y),
				Claimed[Candidate]);
		}

		// ONE offset, not a per-hex or distance-derived one. The value itself is phase 5's
		// and is deliberately not named.
		if (TestTrue(TEXT("every drawn overlay instance was placed"), ZOffsets.Num() > 0))
		{
			for (int32 Index = 1; Index < ZOffsets.Num(); ++Index)
			{
				TestTrue(TEXT("every overlay instance sits the same height above the tile plane"),
					FMath::IsNearlyEqual(ZOffsets[Index], ZOffsets[0], 0.01));
			}
		}
	}

	// ---- The target overlay is a second component --------------------------
	// §2.11.1 shows both at once, so filling one must not disturb the other.
	TArray<FIntPoint> Targets;
	Targets.Add(Model.Hexes[1].Hex);

	Board->ShowTargets(Targets);

	UHierarchicalInstancedStaticMeshComponent* TargetOverlay = nullptr;
	for (int32 Index = 0; Index < Components.Num(); ++Index)
	{
		if (Index == ReachComponentIndex || Components[Index] == nullptr)
		{
			continue;
		}
		if (Components[Index]->GetInstanceCount() != Before[Index])
		{
			TargetOverlay = Components[Index];
			break;
		}
	}

	if (TestNotNull(TEXT("ShowTargets filled a component that is not the reach overlay"), TargetOverlay))
	{
		TestEqual(TEXT("the target overlay draws exactly the hexes it was handed"),
			TargetOverlay->GetInstanceCount(), Targets.Num());
	}

	TestEqual(TEXT("showing targets did not disturb the reach overlay"),
		ReachOverlay->GetInstanceCount(), Reach.Num());

	// ---- Clearing is exact too ---------------------------------------------
	Board->ClearReach();
	TestEqual(TEXT("ClearReach leaves no reach instances"), ReachOverlay->GetInstanceCount(), 0);

	// A second, differently sized set over the cleared overlay, so the count is a function
	// of the argument rather than of anything the board remembered.
	TArray<FIntPoint> Second;
	for (int32 Index = 0; Index < FMath::Min(7, Model.Hexes.Num()); ++Index)
	{
		Second.Add(Model.Hexes[Index].Hex);
	}
	Board->ShowReach(Second);
	TestEqual(TEXT("a second, larger set is drawn in full and in place of the first"),
		ReachOverlay->GetInstanceCount(), Second.Num());

	// And the tiles were never touched by any of it -- the highlight is a separate surface.
	TestEqual(TEXT("highlighting changed nothing about the board itself"),
		Board->GetDrawnHexCount(), Model.Hexes.Num());

	return true;
}
