// The gate for `AStratBoardActor`'s 2026-08-29 no-churn early-out -- GDD §2.2 / §4.7 Stub 8.
//
// WHAT THIS FILE EXISTS TO PIN, AND WHY IT COULD NOT BE PINNED THE OBVIOUS WAY.
//
// `AStratPlayerController::Tick` polls `UpdateHoverFromCursor` every frame; since `1da4198`
// (2026-08-27) a hover change runs `RefreshFromMachine` -> `ApplyView` ->
// `AStratBoardActor::ApplyHexes`, which cleared and re-added EVERY tile instance on the board.
// The user's report was tiles "flicker in what seems to me a switch between visible and hidden"
// while the cursor moved. The fix is `AStratBoardActor::DrawsExactlyTheseHexes`, asked before
// anything is cleared, plus the same guard inside `FillOverlay` for all three overlays.
//
// THE OLD CODE AND THE NEW CODE AGREE ON EVERY OUTCOME A HEADLESS ASSERTION CAN READ. Same
// instance count, same `InstanceHexes`, same `HexAtInstance` answers -- because a rebuild from
// the same model reproduces exactly what it produced. So a clause asserting the resulting
// count, or the picking answers, PASSES IDENTICALLY WITH THE FIX REVERTED and pins nothing.
// This project has a recorded failure of that exact shape (a pin that asserts a copy of its
// subject), and it is the failure this file was written against.
//
// THE INSTRUMENT IS A MUTATION PLANTED BEHIND THE BOARD'S BACK. Instance 0 of each component is
// nudged in Z through `UpdateInstanceTransform` -- a write the board neither performs nor reads:
// `DrawsExactlyTheseHexes` compares `InstanceHexes` and instance COUNTS and never a transform,
// so the probe cannot influence the decision it is measuring. Then the SAME call is made again.
//   - If the early-out fired, the mutation SURVIVES -- and survival is only possible if
//     `ClearInstances` was never called. That is the "was the renderer touched" fact, and it is
//     the only fact that distinguishes the two implementations.
//   - If a rebuild ran, the mutation is DESTROYED and the instance is back at the board's own
//     `WorldLocationOfHex` for the hex it names.
//
// AND EVERY NO-CHURN CLAUSE IS PAIRED WITH A POSITIVE CONTROL THAT DEMANDS THE MUTATION DIE.
// A probe that cannot detect a real rebuild passes exactly like an inert clause. So
// `AChangedHexListRebuildsAndPickingFollowsIt`, `AReorderedHexListRebuildsAndPickingFollowsIt`
// and `AChangedOverlaySetRebuilds` assert the destruction, against models this file changes by
// exactly one thing. They are not decoration; without them the survival clauses would be green
// on a board that had stopped drawing altogether.
//
// THE ID IS `GATE-BOARDCHURN` AND IT IS A LOCAL GATE NAME. It mints no acceptance ID and claims
// none, following `GATE-MAPMARKERS` and `GATE-INFOPANEL`. `T-UI-02` WAS PROPOSED BY THE ENGINEER
// AND IS REFUSED HERE, on the acceptance set's own text: §4.11 states T-UI-02 as "the
// reachable-hex highlight displays exactly the T-MOVE-01 set", and `StratBoardPicking.cpp`
// stretches it to the board's axial->world map and its inverse. Both are claims about WHAT IS
// DRAWN. Nothing below asserts what is drawn as its subject -- every clause here is about
// whether the RENDERER WAS TOUCHED while what is drawn stayed identical, which no acceptance ID
// in the GDD covers. Filing it under T-UI-02 would make that ID's discharge depend on a
// performance-shaped property it was never written to carry.
//
// WHERE THE EXPECTATIONS COME FROM, which is the only interesting column.
//   - The hex list is `FStratViewModel::Hexes`, built by `StratBuildViewModel` over a bridge
//     seeded with the shipped scenario. Not one coordinate is written down in this file.
//   - Every "the mutation died" expectation is `Board->WorldLocationOfHex(...)` for the hex the
//     board's own `HexAtInstance` names at that index. The axial->world formula is READ, never
//     repeated -- a test that re-derived it would assert its own arithmetic.
//   - The two changed models are the SAME model with one edit each: one hex dropped, or two
//     same-terrain entries swapped. The changed expectation is therefore the changed model, not
//     a number.
//   - The nudge offset is the one value this file invents, and it is a FIXTURE value, not an
//     expectation: it is planted and then looked for. No module-side value exists for "a Z the
//     board would never write", which is precisely what makes it a usable marker.
//
// TILE LAYERS ARE IDENTIFIED BY ASKING THE BOARD, NEVER BY NAME AND NEVER BY ELIMINATION. A
// component is a tile layer exactly when `HexAtInstance(Component, 0, Hex)` answers true; the
// overlays are absent from `TerrainLayers` and answer false. Overlay components are identified
// by which single component's instance count MOVES under `ShowReach` / `ShowTargets` /
// `ShowObjective`, cross-checked against `GetTargetOverlayCount` and `GetObjectiveOverlayCount`
// by name -- the conversion `StratBoardPicking.cpp` already records as load-bearing now that a
// third overlay exists.
//
// NOT PINNED HERE, and the reason is a finding rather than a preference:
//   `AnUnmeshedBoardIsNeverRememberedAsDrawn` was asked for as "apply with FallbackTerrainMesh
//   unset, assign it, apply the same model -- the board must draw". IT DOES NOT DRAW, and the
//   early-out is not why. `AStratBoardActor::LayerFor` assigns a layer component's static mesh
//   ONCE, at creation; a layer created during the unmeshed apply keeps a null mesh forever, so
//   the second apply rebuilds (correctly -- the early-out returns false) and still skips every
//   hex. The clause below therefore pins what the early-out is accountable for and what stays
//   true after that gap is closed: THE BOARD NEVER REPORTS A CLEAN DRAW IT HAS NOT PERFORMED.
//   An early-out that believed a request rather than what was drawn would report exactly that.
//   Asserting "0 drawn" instead would freeze the defect into the suite.
//
// NOTHING BELOW ASKS A RULES QUESTION. No `strat::` free function is called -- LNK2019 outside
// StratBridge and StratRules, measured 8x -- and the only bridge methods used are
// `LoadDefinitions` and `LoadScenarioFromFile`, both to build a model.

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

// A .cpp may say this; a UHT-parsed header may not. Same constraint StratMatchSubsystem.cpp
// records about itself.
#include "StratBridge.h"

namespace StratBoardChurn
{
	/** The harness pin for which side moves first -- Replay.h states no rule in this project
	 *  decides it, and StratBoardPicking.cpp pins the same value for the same reason. */
	static const int32 kFirstSide = 0;

	/**
	 * The Z the board never writes.
	 *
	 * Tiles are added at local Z 0 and overlays at `OverlayZOffset` (a small presentation
	 * value, 2.0 by default). A marker one metre up is outside anything either path produces,
	 * so "is the marker still there" has one answer and not two.
	 */
	static const double kMarkerZ = 1000.0;

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
	 * Duplicated from `StratBoardPicking.cpp` rather than shared, which is this project's
	 * existing posture for this helper -- three other files each carry their own copy and say
	 * so. Sharing it would mean a header in a Tests/ directory that several modules include.
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

	/** A stand-in tile mesh. `/Engine/` rather than `/Game/`, so this does not even touch the
	 *  project content the no-path-literal rule is about; automation fixtures are one of that
	 *  rule's two standing exceptions in any case. */
	static UStaticMesh* PlaceholderMesh()
	{
		return LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	}

	/**
	 * Writes a reflected object property on an instance.
	 *
	 * THE FIXTURE'S ONLY REACH-AROUND, and it is a supported read/write of a `UPROPERTY`
	 * through the reflection system -- the surface a details panel uses. It exists because the
	 * properties it sets are Blueprint defaults a headless gate does not have. It configures
	 * the fixture and never supplies an expectation.
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
	 * THE WORLD GOES FIRST AND THE CONTEXT SECOND -- the order is measured, not chosen; the
	 * other order leaves "World has no context!" once per actor, and a gate that leaves
	 * warnings in a green run trains the next reader to skim warnings.
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

	/** Every instanced-mesh component on the board, tile layers and overlays alike. */
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

	/**
	 * The components that are tile layers of THIS board, asked of the board itself.
	 *
	 * `HexAtInstance` returns true only for a component in `TerrainLayers` with a valid index,
	 * so instance 0 answering true is a positive identification. The overlays are not in that
	 * array and answer false -- they are excluded BY THE CLASS'S OWN ANSWER and not by a name
	 * match or by elimination, which is the identification hazard `StratBoardPicking.cpp`
	 * records as live now that three overlays exist.
	 */
	static void GatherTileLayers(
		const AStratBoardActor* Board,
		TArray<UHierarchicalInstancedStaticMeshComponent*>& Out)
	{
		Out.Reset();
		TArray<UHierarchicalInstancedStaticMeshComponent*> All;
		GatherInstancedComponents(Board, All);
		for (UHierarchicalInstancedStaticMeshComponent* const Component : All)
		{
			FIntPoint Hex = FIntPoint::ZeroValue;
			if (Component != nullptr && Board->HexAtInstance(Component, 0, Hex))
			{
				Out.Add(Component);
			}
		}
	}

	/**
	 * Plants the marker on instance `Index` of `Component`, in component-local space.
	 *
	 * A WRITE THE BOARD NEITHER PERFORMS NOR READS. `DrawsExactlyTheseHexes` compares
	 * `InstanceHexes` against the model and each component's instance COUNT; it never looks at
	 * a transform. So this cannot steer the decision it is used to observe -- which is the
	 * whole reason it is a usable probe and not a second copy of the subject.
	 */
	static bool PlantMarker(UHierarchicalInstancedStaticMeshComponent* Component, int32 Index)
	{
		FTransform Current;
		if (Component == nullptr || !Component->GetInstanceTransform(Index, Current, /*bWorldSpace*/ false))
		{
			return false;
		}

		FVector Moved = Current.GetLocation();
		Moved.Z = kMarkerZ;
		Current.SetLocation(Moved);

		return Component->UpdateInstanceTransform(Index, Current,
			/*bWorldSpace*/ false, /*bMarkRenderStateDirty*/ true, /*bTeleport*/ true);
	}

	/** Is the marker still on that instance? Read in the same space it was written. */
	static bool MarkerSurvives(const UHierarchicalInstancedStaticMeshComponent* Component, int32 Index)
	{
		FTransform Now;
		if (Component == nullptr || !Component->GetInstanceTransform(Index, Now, /*bWorldSpace*/ false))
		{
			return false;
		}
		return FMath::IsNearlyEqual(Now.GetLocation().Z, kMarkerZ, 0.01);
	}

	/** The three overlay components, each identified by the single component whose instance
	 *  count MOVES under its own call. Filled by `IdentifyOverlays` below. */
	struct FOverlayComponents
	{
		UHierarchicalInstancedStaticMeshComponent* Reach = nullptr;
		UHierarchicalInstancedStaticMeshComponent* Target = nullptr;
		UHierarchicalInstancedStaticMeshComponent* Objective = nullptr;
	};

	/** The one component whose instance count differs from `Before`, or null if that is not
	 *  exactly one component. Positive identification: the subject is named by what it DID. */
	static UHierarchicalInstancedStaticMeshComponent* SoleMovedComponent(
		const TArray<UHierarchicalInstancedStaticMeshComponent*>& Components,
		const TArray<int32>& Before)
	{
		UHierarchicalInstancedStaticMeshComponent* Moved = nullptr;
		int32 MovedCount = 0;
		for (int32 Index = 0; Index < Components.Num(); ++Index)
		{
			const int32 Now = Components[Index] != nullptr ? Components[Index]->GetInstanceCount() : 0;
			if (Before.IsValidIndex(Index) && Now != Before[Index])
			{
				++MovedCount;
				Moved = Components[Index];
			}
		}
		return MovedCount == 1 ? Moved : nullptr;
	}

	static void SnapshotCounts(
		const TArray<UHierarchicalInstancedStaticMeshComponent*>& Components,
		TArray<int32>& Out)
	{
		Out.Reset();
		Out.Reserve(Components.Num());
		for (UHierarchicalInstancedStaticMeshComponent* const Component : Components)
		{
			Out.Add(Component != nullptr ? Component->GetInstanceCount() : 0);
		}
	}
}

// ---------------------------------------------------------------------------
// 1. The same model, applied twice, touches no tile instance.
//
// THE ONE FACT THAT DISTINGUISHES THE FIX FROM WHAT IT REPLACED. Every count, every hex and
// every `HexAtInstance` answer below is identical under both implementations; only the marker's
// survival is not. The clause also re-asserts the drawn count and the index map afterwards --
// not as its teeth, but so that "nothing was touched" cannot be satisfied by a board that had
// quietly stopped drawing.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBoardChurnSameModelTest,
	"Stratocracy.StratPlay.GATE-BOARDCHURN.ReapplyingTheSameHexesTouchesNoTileInstance",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBoardChurnSameModelTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBoardChurn;

	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Model;
	if (!TestTrue(TEXT("the view model builds from the seeded bridge"),
			StratBuildViewModel(Bridge, kFirstSide, Model, Error)))
	{
		AddError(Error);
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
			SetObjectProperty(Board, TEXT("FallbackTerrainMesh"), Mesh, Error)))
	{
		AddError(Error);
		return false;
	}
	Board->DispatchBeginPlay();

	FString Reason;
	if (!TestTrue(TEXT("the board draws the model's hexes"), Board->ApplyHexes(Model.Hexes, Reason)))
	{
		AddError(Reason);
		return false;
	}

	TArray<UHierarchicalInstancedStaticMeshComponent*> Layers;
	GatherTileLayers(Board, Layers);
	if (!TestTrue(TEXT("the board has tile layers, identified by HexAtInstance answering for them"),
			Layers.Num() > 0))
	{
		return false;
	}

	// The hex each layer's instance 0 names BEFORE the second apply. Read from the board, so
	// the after-comparison is against the board's own answer and not against a coordinate this
	// file chose.
	TArray<FIntPoint> HexAtZeroBefore;
	for (UHierarchicalInstancedStaticMeshComponent* const Layer : Layers)
	{
		FIntPoint Hex = FIntPoint::ZeroValue;
		Board->HexAtInstance(Layer, 0, Hex);
		HexAtZeroBefore.Add(Hex);

		if (!TestTrue(TEXT("the marker is planted on this layer's instance 0"), PlantMarker(Layer, 0)))
		{
			return false;
		}
		if (!TestTrue(TEXT("the marker reads back before the second apply -- the probe is live"),
				MarkerSurvives(Layer, 0)))
		{
			return false;
		}
	}

	const int32 DrawnBefore = Board->GetDrawnHexCount();

	// ---- THE CLAUSE ---------------------------------------------------------
	// The identical model, applied again -- exactly what a hover change does every frame.
	FString SecondReason;
	TestTrue(TEXT("the second apply of the identical model succeeds"),
		Board->ApplyHexes(Model.Hexes, SecondReason));
	TestEqual(TEXT("the second apply reports no gap"), SecondReason, FString());

	for (int32 Index = 0; Index < Layers.Num(); ++Index)
	{
		// SURVIVAL IS THE MEASUREMENT. `ClearInstances` + re-add cannot preserve this, because
		// the rebuild writes `LocalLocationOfHex(Hex, 0.0)` -- a Z of zero -- over it.
		TestTrue(*FString::Printf(
				TEXT("tile layer %d was not rebuilt: the marker planted behind the board's back survived the second apply"),
				Index),
			MarkerSurvives(Layers[Index], 0));
	}

	// ---- And the board is still the board ----------------------------------
	TestEqual(TEXT("the drawn hex count is unchanged"), Board->GetDrawnHexCount(), DrawnBefore);
	TestEqual(TEXT("the drawn hex count is still the model's hex count"),
		Board->GetDrawnHexCount(), Model.Hexes.Num());

	for (int32 Index = 0; Index < Layers.Num(); ++Index)
	{
		FIntPoint Hex = FIntPoint::ZeroValue;
		if (TestTrue(TEXT("instance 0 of each layer still names a hex"),
				Board->HexAtInstance(Layers[Index], 0, Hex)))
		{
			TestEqual(TEXT("and it names the same hex it named before the second apply"),
				Hex, HexAtZeroBefore[Index]);
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// 2. A CHANGED hex list rebuilds -- the positive control, and the index map follows.
//
// WITHOUT THIS THE CLAUSE ABOVE IS INERT. A board that had stopped drawing, or a probe that
// could not see a rebuild, would satisfy "the marker survived" trivially. Here the marker MUST
// die, and the instance it was planted on must be back at the board's own `WorldLocationOfHex`
// for the hex `HexAtInstance` names there.
//
// THE MODEL IS CHANGED BY EXACTLY ONE THING: the last hex is dropped. So the expectation is the
// changed model -- 98 where the model has 98 -- and no number in this file.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBoardChurnChangedListTest,
	"Stratocracy.StratPlay.GATE-BOARDCHURN.AChangedHexListRebuildsAndPickingFollowsIt",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBoardChurnChangedListTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBoardChurn;

	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Model;
	if (!TestTrue(TEXT("the view model builds from the seeded bridge"),
			StratBuildViewModel(Bridge, kFirstSide, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the board has hexes to drop one from"), Model.Hexes.Num() >= 2))
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
			SetObjectProperty(Board, TEXT("FallbackTerrainMesh"), Mesh, Error)))
	{
		AddError(Error);
		return false;
	}
	Board->DispatchBeginPlay();

	FString Reason;
	if (!TestTrue(TEXT("the board draws the model's hexes"), Board->ApplyHexes(Model.Hexes, Reason)))
	{
		AddError(Reason);
		return false;
	}

	TArray<UHierarchicalInstancedStaticMeshComponent*> Layers;
	GatherTileLayers(Board, Layers);
	if (!TestTrue(TEXT("the board has tile layers"), Layers.Num() > 0))
	{
		return false;
	}
	for (UHierarchicalInstancedStaticMeshComponent* const Layer : Layers)
	{
		if (!TestTrue(TEXT("the marker is planted on this layer's instance 0"), PlantMarker(Layer, 0)))
		{
			return false;
		}
	}

	// ---- One hex fewer, and nothing else different -------------------------
	TArray<FStratHexView> Shorter = Model.Hexes;
	Shorter.Pop();

	FString ShorterReason;
	TestTrue(TEXT("the board draws the shortened model"), Board->ApplyHexes(Shorter, ShorterReason));
	TestEqual(TEXT("the shortened apply reports no gap"), ShorterReason, FString());

	// ---- THE CONTROL: the rebuild happened ---------------------------------
	for (int32 Index = 0; Index < Layers.Num(); ++Index)
	{
		TestFalse(*FString::Printf(
				TEXT("tile layer %d WAS rebuilt for a changed model: the planted marker is gone"),
				Index),
			MarkerSurvives(Layers[Index], 0));
	}

	// ---- And it rebuilt to the model it was handed -------------------------
	TestEqual(TEXT("the drawn hex count is the shortened model's own count"),
		Board->GetDrawnHexCount(), Shorter.Num());

	// Every instance of every layer names a hex of the SHORTENED model, at the board's own
	// forward-mapped location. The expected location is READ from the board -- the axial->world
	// formula is never repeated here.
	int32 Walked = 0;
	for (UHierarchicalInstancedStaticMeshComponent* const Layer : Layers)
	{
		const int32 Count = Layer->GetInstanceCount();
		for (int32 Instance = 0; Instance < Count; ++Instance)
		{
			FIntPoint Hex = FIntPoint::ZeroValue;
			if (!TestTrue(TEXT("every drawn instance names a hex"),
					Board->HexAtInstance(Layer, Instance, Hex)))
			{
				continue;
			}
			++Walked;

			const bool bInModel = Shorter.ContainsByPredicate(
				[&Hex](const FStratHexView& View) { return View.Hex == Hex; });
			if (!TestTrue(*FString::Printf(TEXT("hex (%d, %d) is in the shortened model"), Hex.X, Hex.Y),
					bInModel))
			{
				continue;
			}

			FTransform DrawnAt;
			if (TestTrue(TEXT("the instance has a readable transform"),
					Layer->GetInstanceTransform(Instance, DrawnAt, /*bWorldSpace*/ true)))
			{
				const FVector Actual = DrawnAt.GetLocation();
				const FVector Expected = Board->WorldLocationOfHex(Hex);
				TestTrue(*FString::Printf(
						TEXT("the rebuilt instance for hex (%d, %d) sits at the board's own WorldLocationOfHex"),
						Hex.X, Hex.Y),
					FMath::IsNearlyEqual(Actual.X, Expected.X, 0.01) &&
					FMath::IsNearlyEqual(Actual.Y, Expected.Y, 0.01) &&
					FMath::IsNearlyEqual(Actual.Z, Expected.Z, 0.01));
			}
		}
	}
	TestEqual(TEXT("every drawn instance was walked"), Walked, Shorter.Num());

	// The dropped hex is drawn nowhere.
	const FIntPoint Dropped = Model.Hexes.Last().Hex;
	const bool bDroppedStillReachable = [&]()
	{
		for (UHierarchicalInstancedStaticMeshComponent* const Layer : Layers)
		{
			const int32 Count = Layer->GetInstanceCount();
			for (int32 Instance = 0; Instance < Count; ++Instance)
			{
				FIntPoint Hex = FIntPoint::ZeroValue;
				if (Board->HexAtInstance(Layer, Instance, Hex) && Hex == Dropped)
				{
					return true;
				}
			}
		}
		return false;
	}();
	TestFalse(TEXT("the hex the model dropped is named by no instance"), bDroppedStillReachable);

	return true;
}

// ---------------------------------------------------------------------------
// 3. A REORDERED hex list of the SAME LENGTH rebuilds, and the index map follows it.
//
// THE SHARP CASE FOR AN EARLY-OUT THAT COMPARED A SET OR A COUNT. `InstanceHexes` is
// `HexAtInstance`'s index map -- it is what picking reads -- so two boards drawing the same
// hexes in a different order are genuinely different drawings, and an early-out that fired here
// would leave the cursor naming the wrong hex for as long as the board was up.
//
// THE SWAP IS WITHIN ONE TERRAIN LAYER, AND THAT IS NOT AN IMPLEMENTATION DETAIL. Swapping two
// hexes of DIFFERENT terrain kinds leaves each layer's own sequence untouched, so the drawing --
// and the index map -- is identical and the early-out is RIGHT to fire. The two entries swapped
// here share a `TerrainId`, found in the model rather than named.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBoardChurnReorderedListTest,
	"Stratocracy.StratPlay.GATE-BOARDCHURN.AReorderedHexListRebuildsAndPickingFollowsIt",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBoardChurnReorderedListTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBoardChurn;

	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Model;
	if (!TestTrue(TEXT("the view model builds from the seeded bridge"),
			StratBuildViewModel(Bridge, kFirstSide, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	// Two entries of the same terrain kind, taken from the model. Which kind is not this file's
	// business and is not named.
	int32 SwapA = INDEX_NONE;
	int32 SwapB = INDEX_NONE;
	for (int32 First = 0; First < Model.Hexes.Num() && SwapA == INDEX_NONE; ++First)
	{
		for (int32 Second = First + 1; Second < Model.Hexes.Num(); ++Second)
		{
			if (Model.Hexes[First].TerrainId == Model.Hexes[Second].TerrainId)
			{
				SwapA = First;
				SwapB = Second;
				break;
			}
		}
	}
	if (!TestTrue(TEXT("the model has two hexes of the same terrain kind to reorder"),
			SwapA != INDEX_NONE && SwapB != INDEX_NONE))
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
			SetObjectProperty(Board, TEXT("FallbackTerrainMesh"), Mesh, Error)))
	{
		AddError(Error);
		return false;
	}
	Board->DispatchBeginPlay();

	FString Reason;
	if (!TestTrue(TEXT("the board draws the model's hexes"), Board->ApplyHexes(Model.Hexes, Reason)))
	{
		AddError(Reason);
		return false;
	}

	TArray<UHierarchicalInstancedStaticMeshComponent*> Layers;
	GatherTileLayers(Board, Layers);
	if (!TestTrue(TEXT("the board has tile layers"), Layers.Num() > 0))
	{
		return false;
	}
	for (UHierarchicalInstancedStaticMeshComponent* const Layer : Layers)
	{
		if (!TestTrue(TEXT("the marker is planted on this layer's instance 0"), PlantMarker(Layer, 0)))
		{
			return false;
		}
	}

	// ---- Same hexes, same length, one pair swapped -------------------------
	TArray<FStratHexView> Reordered = Model.Hexes;
	Reordered.Swap(SwapA, SwapB);

	FString ReorderedReason;
	TestTrue(TEXT("the board draws the reordered model"), Board->ApplyHexes(Reordered, ReorderedReason));
	TestEqual(TEXT("the reordered apply reports no gap"), ReorderedReason, FString());

	// ---- THE CONTROL: it rebuilt, despite the length being identical -------
	for (int32 Index = 0; Index < Layers.Num(); ++Index)
	{
		TestFalse(*FString::Printf(
				TEXT("tile layer %d WAS rebuilt for a same-length reordering: the planted marker is gone"),
				Index),
			MarkerSurvives(Layers[Index], 0));
	}

	TestEqual(TEXT("the drawn hex count is unchanged by a reordering"),
		Board->GetDrawnHexCount(), Reordered.Num());

	// ---- AND THE INDEX MAP IS IN STEP WITH THE NEW ORDER -------------------
	// The expectation is the reordered model itself: for the swapped pair's terrain kind, the
	// hexes in the order that model lists them. Built by walking the model, never written down.
	const FName SwappedTerrain = Reordered[SwapA].TerrainId;

	TArray<FIntPoint> ExpectedSequence;
	for (const FStratHexView& View : Reordered)
	{
		if (View.TerrainId == SwappedTerrain)
		{
			ExpectedSequence.Add(View.Hex);
		}
	}

	// The layer that draws that terrain kind, found by asking the board which hex its instance 0
	// names and looking that hex up in the model -- not by component name.
	UHierarchicalInstancedStaticMeshComponent* SwappedLayer = nullptr;
	for (UHierarchicalInstancedStaticMeshComponent* const Layer : Layers)
	{
		FIntPoint Hex = FIntPoint::ZeroValue;
		if (!Board->HexAtInstance(Layer, 0, Hex))
		{
			continue;
		}
		const FStratHexView* const View = Reordered.FindByPredicate(
			[&Hex](const FStratHexView& Candidate) { return Candidate.Hex == Hex; });
		if (View != nullptr && View->TerrainId == SwappedTerrain)
		{
			SwappedLayer = Layer;
			break;
		}
	}
	if (!TestNotNull(TEXT("the layer drawing the swapped pair's terrain kind was found"), SwappedLayer))
	{
		return false;
	}

	if (TestEqual(TEXT("that layer draws one instance per hex of that terrain kind in the model"),
			SwappedLayer->GetInstanceCount(), ExpectedSequence.Num()))
	{
		for (int32 Instance = 0; Instance < ExpectedSequence.Num(); ++Instance)
		{
			FIntPoint Hex = FIntPoint::ZeroValue;
			if (TestTrue(TEXT("the instance names a hex"),
					Board->HexAtInstance(SwappedLayer, Instance, Hex)))
			{
				TestEqual(*FString::Printf(
						TEXT("instance %d names the hex the REORDERED model lists at that position"),
						Instance),
					Hex, ExpectedSequence[Instance]);
			}
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// 4. The early-out never remembers a request as a drawing.
//
// THE SHARPEST FAILURE MODE OF AN EARLY-OUT, and the reason `DrawsExactlyTheseHexes` reads the
// components rather than a cached model: a board asked to draw a hex list it CANNOT draw --
// because the terrain has no mesh -- must not conclude on the next identical request that the
// work is already done. That board would stay blank forever, on a "yes" about a request.
//
// WHAT THIS CLAUSE ASSERTS AND WHY IT IS NOT "THE BOARD DRAWS". The brief for this pass asked
// for "assign the mesh, apply the same model, the board must draw". IT DOES NOT DRAW, and the
// early-out is not the reason: `AStratBoardActor::LayerFor` sets a layer component's static mesh
// ONCE, at creation, so a layer created during an unmeshed apply keeps a null mesh and the
// second apply -- which DOES rebuild, the early-out correctly returning false -- skips every hex
// again. Asserting "0 drawn" would freeze that gap into the suite and go red the day it is
// fixed. So the invariant asserted is the one that holds on both sides of that fix and that only
// a request-believing early-out can break: THE BOARD NEVER REPORTS A CLEAN DRAW IT HAS NOT
// PERFORMED. The branch taken is stated in the run's own output.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBoardChurnUnmeshedTest,
	"Stratocracy.StratPlay.GATE-BOARDCHURN.AnUnmeshedBoardIsNeverRememberedAsDrawn",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBoardChurnUnmeshedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBoardChurn;

	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Model;
	if (!TestTrue(TEXT("the view model builds from the seeded bridge"),
			StratBuildViewModel(Bridge, kFirstSide, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the model has hexes"), Model.Hexes.Num() > 0))
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

	// NO MESH ASSIGNED. This is the configuration `ApplyHexes` documents as legitimate: a
	// presentation gap it reports and keeps going through, not a seeding failure.
	AStratBoardActor* const Board = Scope.World->SpawnActor<AStratBoardActor>();
	if (!TestNotNull(TEXT("the board spawned"), Board))
	{
		return false;
	}
	Board->DispatchBeginPlay();

	FString FirstReason;
	const bool bFirstOk = Board->ApplyHexes(Model.Hexes, FirstReason);

	TestFalse(TEXT("an unmeshed board reports the gap rather than a clean draw"), bFirstOk);
	TestTrue(TEXT("and the reason names what was missing"), !FirstReason.IsEmpty());
	TestEqual(TEXT("and nothing was drawn"), Board->GetDrawnHexCount(), 0);

	// ---- The mesh arrives, and the identical model is applied again --------
	if (!TestTrue(TEXT("the fixture assigns a fallback tile mesh after the first apply"),
			SetObjectProperty(Board, TEXT("FallbackTerrainMesh"), Mesh, Error)))
	{
		AddError(Error);
		return false;
	}

	FString SecondReason;
	const bool bSecondOk = Board->ApplyHexes(Model.Hexes, SecondReason);

	// ---- THE CLAUSE --------------------------------------------------------
	// A "yes" here is a statement about what is ON SCREEN. If the board says the model is drawn
	// cleanly, every hex of it must actually be drawn.
	if (bSecondOk && SecondReason.IsEmpty())
	{
		AddInfo(TEXT("the second apply reported a clean draw -- asserting that it actually drew the model"));
		TestEqual(TEXT("a board reporting a clean draw has drawn every hex of the model"),
			Board->GetDrawnHexCount(), Model.Hexes.Num());
	}
	else
	{
		AddInfo(TEXT("the second apply reported the gap again -- asserting it did not claim a clean board"));
		TestFalse(TEXT("the early-out did not swallow an undrawn request as satisfied"), bSecondOk);
		TestTrue(TEXT("and the gap is reported again rather than forgotten"), !SecondReason.IsEmpty());
	}

	return true;
}

// ---------------------------------------------------------------------------
// 5. Re-showing the same overlay hexes touches no overlay instance.
//
// ALL THREE OVERLAYS IN ONE CLAUSE, because they are one code path -- `FillOverlay` -- and the
// property is that the path's early-out holds for each component's own cache. A hover crossing
// repaints all three every time the cursor changes hex, with content that did not change.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBoardChurnOverlaySameSetTest,
	"Stratocracy.StratPlay.GATE-BOARDCHURN.ReshowingTheSameOverlayHexesTouchesNoInstance",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBoardChurnOverlaySameSetTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBoardChurn;

	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Model;
	if (!TestTrue(TEXT("the view model builds from the seeded bridge"),
			StratBuildViewModel(Bridge, kFirstSide, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the board is big enough to take three hexes from"), Model.Hexes.Num() >= 3))
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
	Board->DispatchBeginPlay();

	FString Reason;
	TestTrue(TEXT("the board draws the model's hexes"), Board->ApplyHexes(Model.Hexes, Reason));

	TArray<UHierarchicalInstancedStaticMeshComponent*> Components;
	GatherInstancedComponents(Board, Components);
	TArray<int32> Counts;
	SnapshotCounts(Components, Counts);

	const TArray<FIntPoint> ReachSet = { Model.Hexes[0].Hex, Model.Hexes[Model.Hexes.Num() / 2].Hex };
	const TArray<FIntPoint> TargetSet = { Model.Hexes[Model.Hexes.Num() - 1].Hex };
	const FIntPoint ObjectiveHex = Model.Hexes[1].Hex;

	// ---- Identify each overlay by the one component its own call moved -----
	Board->ShowReach(ReachSet);
	UHierarchicalInstancedStaticMeshComponent* const Reach = SoleMovedComponent(Components, Counts);
	if (!TestNotNull(TEXT("ShowReach moved exactly one component"), Reach))
	{
		return false;
	}
	SnapshotCounts(Components, Counts);

	Board->ShowTargets(TargetSet);
	UHierarchicalInstancedStaticMeshComponent* const Target = SoleMovedComponent(Components, Counts);
	if (!TestNotNull(TEXT("ShowTargets moved exactly one component"), Target))
	{
		return false;
	}
	TestTrue(TEXT("and it is not the reach overlay"), Target != Reach);
	// Cross-checked BY NAME against the class's own accessor, which is the conversion
	// StratBoardPicking.cpp records as load-bearing now that a third overlay exists.
	TestEqual(TEXT("the component ShowTargets moved is the one GetTargetOverlayCount reports on"),
		Target->GetInstanceCount(), Board->GetTargetOverlayCount());
	SnapshotCounts(Components, Counts);

	Board->ShowObjective(ObjectiveHex);
	UHierarchicalInstancedStaticMeshComponent* const Objective = SoleMovedComponent(Components, Counts);
	if (!TestNotNull(TEXT("ShowObjective moved exactly one component"), Objective))
	{
		return false;
	}
	TestTrue(TEXT("and it is neither the reach nor the target overlay"),
		Objective != Reach && Objective != Target);
	TestEqual(TEXT("the component ShowObjective moved is the one GetObjectiveOverlayCount reports on"),
		Objective->GetInstanceCount(), Board->GetObjectiveOverlayCount());

	// ---- Plant the markers -------------------------------------------------
	if (!TestTrue(TEXT("a marker is planted on the reach overlay"), PlantMarker(Reach, 0)) ||
		!TestTrue(TEXT("a marker is planted on the target overlay"), PlantMarker(Target, 0)) ||
		!TestTrue(TEXT("a marker is planted on the objective ring"), PlantMarker(Objective, 0)))
	{
		return false;
	}

	// ---- THE CLAUSE: the identical sets, again -----------------------------
	Board->ShowReach(ReachSet);
	Board->ShowTargets(TargetSet);
	Board->ShowObjective(ObjectiveHex);

	TestTrue(TEXT("the reach overlay was not rebuilt for the same hexes: the marker survived"),
		MarkerSurvives(Reach, 0));
	TestTrue(TEXT("the target overlay was not rebuilt for the same hexes: the marker survived"),
		MarkerSurvives(Target, 0));
	TestTrue(TEXT("the objective ring was not rebuilt for the same hex: the marker survived"),
		MarkerSurvives(Objective, 0));

	// And each is still lighting what it was handed -- so "untouched" cannot be satisfied by an
	// overlay that had quietly emptied.
	TestEqual(TEXT("the reach overlay still lights the set it was handed"),
		Reach->GetInstanceCount(), ReachSet.Num());
	TestEqual(TEXT("the target overlay still lights the set it was handed"),
		Board->GetTargetOverlayCount(), TargetSet.Num());
	TestEqual(TEXT("the objective ring still lights one hex"), Board->GetObjectiveOverlayCount(), 1);

	return true;
}

// ---------------------------------------------------------------------------
// 6. A CHANGED overlay set rebuilds -- the positive control for clause 5.
//
// Each overlay is handed a DIFFERENT set and the marker must die, at each of the three
// components. Without this, clause 5's three survivals would also be green on a `FillOverlay`
// that had stopped clearing at all.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBoardChurnOverlayChangedSetTest,
	"Stratocracy.StratPlay.GATE-BOARDCHURN.AChangedOverlaySetRebuilds",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBoardChurnOverlayChangedSetTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBoardChurn;

	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Model;
	if (!TestTrue(TEXT("the view model builds from the seeded bridge"),
			StratBuildViewModel(Bridge, kFirstSide, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the board is big enough to take four hexes from"), Model.Hexes.Num() >= 4))
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
	Board->DispatchBeginPlay();

	FString Reason;
	TestTrue(TEXT("the board draws the model's hexes"), Board->ApplyHexes(Model.Hexes, Reason));

	TArray<UHierarchicalInstancedStaticMeshComponent*> Components;
	GatherInstancedComponents(Board, Components);
	TArray<int32> Counts;
	SnapshotCounts(Components, Counts);

	const TArray<FIntPoint> ReachSet = { Model.Hexes[0].Hex };
	const TArray<FIntPoint> TargetSet = { Model.Hexes[1].Hex };
	const FIntPoint ObjectiveHex = Model.Hexes[2].Hex;

	Board->ShowReach(ReachSet);
	UHierarchicalInstancedStaticMeshComponent* const Reach = SoleMovedComponent(Components, Counts);
	if (!TestNotNull(TEXT("ShowReach moved exactly one component"), Reach))
	{
		return false;
	}
	SnapshotCounts(Components, Counts);

	Board->ShowTargets(TargetSet);
	UHierarchicalInstancedStaticMeshComponent* const Target = SoleMovedComponent(Components, Counts);
	if (!TestNotNull(TEXT("ShowTargets moved exactly one component"), Target))
	{
		return false;
	}
	SnapshotCounts(Components, Counts);

	Board->ShowObjective(ObjectiveHex);
	UHierarchicalInstancedStaticMeshComponent* const Objective = SoleMovedComponent(Components, Counts);
	if (!TestNotNull(TEXT("ShowObjective moved exactly one component"), Objective))
	{
		return false;
	}

	if (!TestTrue(TEXT("a marker is planted on the reach overlay"), PlantMarker(Reach, 0)) ||
		!TestTrue(TEXT("a marker is planted on the target overlay"), PlantMarker(Target, 0)) ||
		!TestTrue(TEXT("a marker is planted on the objective ring"), PlantMarker(Objective, 0)))
	{
		return false;
	}

	// ---- Different sets, one edit each -------------------------------------
	const TArray<FIntPoint> OtherReach = { Model.Hexes[0].Hex, Model.Hexes[3].Hex };
	const TArray<FIntPoint> OtherTarget = { Model.Hexes[3].Hex };
	const FIntPoint OtherObjective = Model.Hexes[3].Hex;

	Board->ShowReach(OtherReach);
	Board->ShowTargets(OtherTarget);
	Board->ShowObjective(OtherObjective);

	// ---- THE CONTROL -------------------------------------------------------
	TestFalse(TEXT("the reach overlay WAS rebuilt for a changed set: the marker is gone"),
		MarkerSurvives(Reach, 0));
	TestFalse(TEXT("the target overlay WAS rebuilt for a changed set: the marker is gone"),
		MarkerSurvives(Target, 0));
	TestFalse(TEXT("the objective ring WAS rebuilt for a changed hex: the marker is gone"),
		MarkerSurvives(Objective, 0));

	TestEqual(TEXT("the reach overlay lights the new set"), Reach->GetInstanceCount(), OtherReach.Num());
	TestEqual(TEXT("the target overlay lights the new set"),
		Board->GetTargetOverlayCount(), OtherTarget.Num());
	TestEqual(TEXT("the objective ring lights one hex"), Board->GetObjectiveOverlayCount(), 1);

	// And at the hexes handed in, read through the board's own forward map -- the axial->world
	// formula is not repeated here.
	{
		FTransform DrawnAt;
		if (TestTrue(TEXT("the objective ring's instance has a readable transform"),
				Objective->GetInstanceTransform(0, DrawnAt, /*bWorldSpace*/ true)))
		{
			const FVector Actual = DrawnAt.GetLocation();
			const FVector Expected = Board->WorldLocationOfHex(OtherObjective);
			TestTrue(TEXT("the rebuilt ring sits over the hex it was handed, on the board's own map"),
				FMath::IsNearlyEqual(Actual.X, Expected.X, 0.01) &&
				FMath::IsNearlyEqual(Actual.Y, Expected.Y, 0.01));
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// 7. An overlay mesh arriving late still draws.
//
// `FillOverlay`'s cache is WRITTEN FROM WHAT WAS DRAWN AND NEVER FROM WHAT WAS REQUESTED, and
// this is the case that makes the distinction observable. With no `OverlayMesh`, `ShowTargets`
// draws nothing; if it cached the request, the identical call after the mesh arrived would
// early-out on a highlight that was never on screen and the overlay would stay dark forever.
//
// THE MESH ARRIVES BY THE PRODUCTION ROUTE. `BeginPlay` is what applies `OverlayMesh` to the
// three components -- the class records why it is not the constructor -- so the fixture shows
// the targets BEFORE dispatching it, then assigns the property and dispatches. No production
// file is touched and no component is reconfigured behind the class's back.
//
// THE COUNT IS READ BY NAME, through `GetTargetOverlayCount`, which reads the component and
// caches nothing.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBoardChurnOverlayMeshLateTest,
	"Stratocracy.StratPlay.GATE-BOARDCHURN.AnOverlayMeshArrivingLateStillDraws",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBoardChurnOverlayMeshLateTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBoardChurn;

	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Model;
	if (!TestTrue(TEXT("the view model builds from the seeded bridge"),
			StratBuildViewModel(Bridge, kFirstSide, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the board is big enough to take two hexes from"), Model.Hexes.Num() >= 2))
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
			SetObjectProperty(Board, TEXT("FallbackTerrainMesh"), Mesh, Error)))
	{
		AddError(Error);
		return false;
	}

	const TArray<FIntPoint> TargetSet = { Model.Hexes[0].Hex, Model.Hexes[1].Hex };

	// ---- No overlay mesh yet: the request draws nothing --------------------
	Board->ShowTargets(TargetSet);
	TestEqual(TEXT("with no OverlayMesh the target overlay lights nothing"),
		Board->GetTargetOverlayCount(), 0);

	// ---- The mesh arrives, by BeginPlay, which is how it arrives in the game
	if (!TestTrue(TEXT("the fixture assigns the overlay mesh after the first request"),
			SetObjectProperty(Board, TEXT("OverlayMesh"), Mesh, Error)))
	{
		AddError(Error);
		return false;
	}
	Board->DispatchBeginPlay();

	// ---- THE CLAUSE: the IDENTICAL request now draws -----------------------
	Board->ShowTargets(TargetSet);
	TestEqual(TEXT("the identical request draws once the mesh exists -- the cache recorded what was drawn, not what was asked for"),
		Board->GetTargetOverlayCount(), TargetSet.Num());

	return true;
}
