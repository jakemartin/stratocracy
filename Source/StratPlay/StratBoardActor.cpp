// GDD §2.2 / §4.7 Stub 8 -- the board, implementation side.
//
// WHAT IS HERE THAT IS NOT IN THE HEADER, and it is one thing: the axial -> world
// formula, written once. `WorldLocationOfHex` below is the only place in the project that
// knows how far apart two hexes are, and every other actor that needs to stand on a hex
// calls it. That is not tidiness -- a second copy of this formula is a board whose units
// are half a tile off the moment someone changes `HexSize` in one of the two.
//
// NO `strat::` TYPE APPEARS IN THIS FILE AND NONE MAY. Hexes are `FIntPoint` with X = q
// and Y = r, which is the convention `StratViewModel.h` established and the reason it
// established it: "`StratPlay` will never name a `strat::` type, so anything left as a
// vendored type here would be a translation phase 3 had to perform, in a module that is
// not allowed to perform it." This file does not include `StratBridge.h` and does not
// need to.
//
// NOTHING BELOW ASKS A RULES QUESTION. The hex list arrives as an argument; the reach set
// and the target set arrive as arguments. This class draws what it is handed.

#include "StratBoardActor.h"

#include "StratPlay.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"

AStratBoardActor::AStratBoardActor()
{
	// NO TICK. This actor is reconciled against the view model when something changed it,
	// by whoever changed it -- the same posture `AStratScoreboardHUD` records for the
	// scoreboard, and for the same reason: a tick-driven poll would make the board's
	// update rate a frame-rate property instead of a turn property.
	PrimaryActorTick.bCanEverTick = false;

	BoardRoot = CreateDefaultSubobject<USceneComponent>(TEXT("BoardRoot"));
	SetRootComponent(BoardRoot);

	// The two overlays are constructor subobjects and the tile layers are not. See the
	// header block: there are exactly two overlays for as long as §2.11.1 has a move phase
	// and an attack phase, and their count is not data the way the terrain kinds are.
	ReachOverlay = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("ReachOverlay"));
	ReachOverlay->SetupAttachment(BoardRoot);

	TargetOverlay = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("TargetOverlay"));
	TargetOverlay->SetupAttachment(BoardRoot);

	// NO COLLISION ON EITHER OVERLAY, and this is the same rule `AStratUnitActor` states
	// for itself: every rules question this game asks is asked about a HEX, so the cursor
	// must reach the tile underneath. An overlay that blocked the trace would make a hex
	// unpickable exactly when it is highlighted as pickable, which is the worst available
	// time for it.
	for (UHierarchicalInstancedStaticMeshComponent* const Overlay : { ReachOverlay.Get(), TargetOverlay.Get() })
	{
		Overlay->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Overlay->SetCanEverAffectNavigation(false);
		Overlay->SetCastShadow(false);
		Overlay->SetMobility(EComponentMobility::Movable);
	}
}

void AStratBoardActor::BeginPlay()
{
	Super::BeginPlay();

	// The overlay meshes and materials are applied here rather than in the constructor
	// because the properties they read are Blueprint defaults, and a constructor running
	// on the CDO sets them on the wrong object. Unset is legitimate and silent: a board
	// with no overlay mesh draws no highlight, which is a configuration a phase-5 pass
	// completes, not an error phase 3 should refuse a match over.
	if (OverlayMesh != nullptr)
	{
		ReachOverlay->SetStaticMesh(OverlayMesh);
		TargetOverlay->SetStaticMesh(OverlayMesh);

		if (ReachMaterial != nullptr)
		{
			ReachOverlay->SetMaterial(0, ReachMaterial);
		}
		if (TargetMaterial != nullptr)
		{
			TargetOverlay->SetMaterial(0, TargetMaterial);
		}
	}
	else
	{
		UE_LOG(LogStratPlay, Log,
			TEXT("Board '%s' has no OverlayMesh set; reach and target highlights will not draw."),
			*GetName());
	}
}

FVector AStratBoardActor::LocalLocationOfHex(FIntPoint Hex, double LocalZ) const
{
	// POINTY-TOP AXIAL, X = q and Y = r. `HexSize` is the CENTRE-TO-CENTRE distance
	// between two adjacent hexes, which is why the row step carries the sqrt(3)/2 and the
	// column step does not: for a pointy-top layout the horizontal neighbour is exactly
	// one spacing away and the row below is offset by half of one.
	//
	// THE ONLY COPY OF THIS FORMULA IN THE PROJECT, and it is the only copy as of phase 4 --
	// see the declaration for the three-copy finding this consolidation closes.
	const double Q = static_cast<double>(Hex.X);
	const double R = static_cast<double>(Hex.Y);

	return FVector(
		HexSize * (Q + R * 0.5),
		HexSize * (UE_DOUBLE_SQRT_3 * 0.5) * R,
		LocalZ);
}

FVector AStratBoardActor::WorldLocationOfHex(FIntPoint Hex) const
{
	// The board plane is the actor's XY. Z is the actor's own -- the header block records
	// why terrain does not vary it.
	return GetActorTransform().TransformPosition(LocalLocationOfHex(Hex, 0.0));
}

FStratTerrainLayer& AStratBoardActor::LayerFor(FName TerrainId)
{
	for (FStratTerrainLayer& Existing : TerrainLayers)
	{
		if (Existing.TerrainId == TerrainId)
		{
			return Existing;
		}
	}

	FStratTerrainLayer NewLayer;
	NewLayer.TerrainId = TerrainId;

	// Created at runtime because the SET of terrain kinds is data -- see the header block
	// on why seven `CreateDefaultSubobject` calls would make this file a second author of
	// the §4.8 terrain table. The component is named after the id so that a reader looking
	// at the world outliner or a stat capture sees "Tiles_Woods" rather than "Tiles_3".
	UHierarchicalInstancedStaticMeshComponent* const Component =
		NewObject<UHierarchicalInstancedStaticMeshComponent>(
			this, UHierarchicalInstancedStaticMeshComponent::StaticClass(),
			*FString::Printf(TEXT("Tiles_%s"), *TerrainId.ToString()));

	Component->SetupAttachment(BoardRoot);
	Component->SetMobility(EComponentMobility::Movable);

	// TILES ARE THE ONLY THING THE CURSOR MAY HIT. Query-only, blocking Visibility: a
	// cursor trace names a hex, and the hex is what every rules question is about. Units
	// and overlays are non-colliding for the same reason from the other side.
	Component->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Component->SetCollisionObjectType(ECC_WorldStatic);
	Component->SetCollisionResponseToAllChannels(ECR_Ignore);
	Component->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Component->SetCanEverAffectNavigation(false);

	if (const TObjectPtr<UStaticMesh>* const Configured = TerrainMeshes.Find(TerrainId))
	{
		Component->SetStaticMesh(*Configured);
	}
	else if (FallbackTerrainMesh != nullptr)
	{
		Component->SetStaticMesh(FallbackTerrainMesh);
	}

	Component->RegisterComponent();

	NewLayer.Tiles = Component;

	const int32 Added = TerrainLayers.Add(MoveTemp(NewLayer));
	return TerrainLayers[Added];
}

bool AStratBoardActor::ApplyHexes(const TArray<FStratHexView>& Hexes, FString& OutFailureReason)
{
	OutFailureReason.Reset();

	// CLEARED WHOLE AND RE-ADDED. The header records why: removing a single instance from
	// a HISM renumbers every instance after it, so a parallel array patched in place
	// acquires an off-by-one that presents as the cursor naming the wrong hex. A full
	// rebuild keeps "instance count equals InstanceHexes count" true by construction.
	//
	// The components themselves are kept rather than destroyed and recreated: the set of
	// terrain kinds does not change between two models of the same scenario, and
	// destroying a registered component per refresh would be a per-turn cost paid for
	// nothing.
	for (FStratTerrainLayer& Layer : TerrainLayers)
	{
		if (Layer.Tiles != nullptr)
		{
			Layer.Tiles->ClearInstances();
		}
		Layer.InstanceHexes.Reset();
	}

	TSet<FName> Unmeshed;

	for (const FStratHexView& Hex : Hexes)
	{
		FStratTerrainLayer& Layer = LayerFor(Hex.TerrainId);

		if (Layer.Tiles == nullptr)
		{
			continue;
		}

		if (Layer.Tiles->GetStaticMesh() == nullptr)
		{
			// Reported once per id, not once per hex: 99 identical sentences would bury
			// the second missing kind under the first.
			Unmeshed.Add(Hex.TerrainId);
			continue;
		}

		// Added in COMPONENT-LOCAL space -- the component sits at the board's root, so the
		// local offset is the same one `WorldLocationOfHex` transforms, and asking for the
		// world location and then untransforming it would be the same arithmetic twice.
		// THE FORMULA ITSELF IS `LocalLocationOfHex`'s and used to be repeated here.
		const FVector Local = LocalLocationOfHex(Hex.Hex, 0.0);

		const int32 InstanceIndex = Layer.Tiles->AddInstance(FTransform(Local), /*bWorldSpace=*/false);

		// APPENDED IN LOCKSTEP WITH THE ADD, and checked rather than assumed: if the
		// component ever returned an index that is not the next one, the parallel array
		// would silently start naming the wrong hex, which is a defect with no visible
		// symptom until a player attacks the wrong unit.
		checkf(InstanceIndex == Layer.InstanceHexes.Num(),
			TEXT("HISM instance index %d does not match the next InstanceHexes slot %d for terrain '%s'"),
			InstanceIndex, Layer.InstanceHexes.Num(), *Hex.TerrainId.ToString());

		Layer.InstanceHexes.Add(Hex.Hex);
	}

	if (Unmeshed.Num() > 0)
	{
		TArray<FString> Names;
		Names.Reserve(Unmeshed.Num());
		for (const FName Id : Unmeshed)
		{
			Names.Add(Id.ToString());
		}
		Names.Sort();

		OutFailureReason = FString::Printf(
			TEXT("no tile mesh for terrain %s -- assign it in TerrainMeshes, or set FallbackTerrainMesh; ")
			TEXT("the rest of the board was drawn"),
			*FString::Join(Names, TEXT(", ")));

		return false;
	}

	return true;
}

bool AStratBoardActor::HexAtInstance(const UPrimitiveComponent* Component, int32 InstanceIndex, FIntPoint& OutHex) const
{
	if (Component == nullptr || InstanceIndex < 0)
	{
		return false;
	}

	for (const FStratTerrainLayer& Layer : TerrainLayers)
	{
		if (static_cast<const UPrimitiveComponent*>(Layer.Tiles.Get()) != Component)
		{
			continue;
		}

		if (!Layer.InstanceHexes.IsValidIndex(InstanceIndex))
		{
			// The component is ours and the index is not. Reported as "not on the board"
			// rather than asserted: a trace can outlive a rebuild by a frame, and a cursor
			// briefly naming nothing is an ordinary answer.
			return false;
		}

		OutHex = Layer.InstanceHexes[InstanceIndex];
		return true;
	}

	// A component this board does not own. Ordinary -- the cursor is somewhere else.
	return false;
}

void AStratBoardActor::FillOverlay(UHierarchicalInstancedStaticMeshComponent* Overlay, const TArray<FIntPoint>& Hexes) const
{
	if (Overlay == nullptr)
	{
		return;
	}

	Overlay->ClearInstances();

	if (Overlay->GetStaticMesh() == nullptr)
	{
		// Nothing to instance. Silent here rather than logged per call: BeginPlay already
		// said so once, and a highlight request per cursor move would say it thousands of
		// times.
		return;
	}

	for (const FIntPoint& Hex : Hexes)
	{
		// The overlay plane only -- the third former copy of the formula, now the same
		// call the tiles make with a different Z.
		const FVector Local = LocalLocationOfHex(Hex, static_cast<double>(OverlayZOffset));

		Overlay->AddInstance(FTransform(Local), /*bWorldSpace=*/false);
	}
}

void AStratBoardActor::ShowReach(const TArray<FIntPoint>& Hexes)
{
	FillOverlay(ReachOverlay, Hexes);
}

void AStratBoardActor::ClearReach()
{
	FillOverlay(ReachOverlay, TArray<FIntPoint>());
}

void AStratBoardActor::ShowTargets(const TArray<FIntPoint>& Hexes)
{
	FillOverlay(TargetOverlay, Hexes);
}

void AStratBoardActor::ClearTargets()
{
	FillOverlay(TargetOverlay, TArray<FIntPoint>());
}

int32 AStratBoardActor::GetTargetOverlayCount() const
{
	// Off the component and never off a cached count -- see the declaration.
	return TargetOverlay != nullptr ? TargetOverlay->GetInstanceCount() : 0;
}

int32 AStratBoardActor::GetDrawnHexCount() const
{
	int32 Total = 0;
	for (const FStratTerrainLayer& Layer : TerrainLayers)
	{
		Total += Layer.InstanceHexes.Num();
	}
	return Total;
}
