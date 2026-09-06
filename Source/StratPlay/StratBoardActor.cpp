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

	// The overlays are constructor subobjects and the tile layers are not. See the header
	// block: one component per MEANING, and the count is not data the way the terrain kinds
	// are. It was two until §2.11.6-B's ring made it three.
	ReachOverlay = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("ReachOverlay"));
	ReachOverlay->SetupAttachment(BoardRoot);

	TargetOverlay = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("TargetOverlay"));
	TargetOverlay->SetupAttachment(BoardRoot);

	ObjectiveOverlay = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("ObjectiveOverlay"));
	ObjectiveOverlay->SetupAttachment(BoardRoot);

	BuildPulseOverlay = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("BuildPulseOverlay"));
	BuildPulseOverlay->SetupAttachment(BoardRoot);

	// NO COLLISION ON ANY OVERLAY, and this is the same rule `AStratUnitActor` states
	// for itself: every rules question this game asks is asked about a HEX, so the cursor
	// must reach the tile underneath. An overlay that blocked the trace would make a hex
	// unpickable exactly when it is highlighted as pickable, which is the worst available
	// time for it. THE BUILD PULSE IS THE CASE THAT MAKES THIS SHARPEST: it lights the very
	// factory tiles a player is about to click on to open a production menu, so a pulse with
	// collision would make the pulse itself the reason the build could not be started.
	// [STAMPED 2026-09-01: this said "ON EITHER OVERLAY" back when there were two. It is
	// count-free now for the header block's reason.]
	for (UHierarchicalInstancedStaticMeshComponent* const Overlay :
	     { ReachOverlay.Get(), TargetOverlay.Get(), ObjectiveOverlay.Get(), BuildPulseOverlay.Get() })
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
		ObjectiveOverlay->SetStaticMesh(OverlayMesh);
		BuildPulseOverlay->SetStaticMesh(OverlayMesh);

		if (ReachMaterial != nullptr)
		{
			ReachOverlay->SetMaterial(0, ReachMaterial);
		}
		if (TargetMaterial != nullptr)
		{
			TargetOverlay->SetMaterial(0, TargetMaterial);
		}
		// UNSET IS SILENT HERE AND NOT LOGGED, unlike a missing `OverlayMesh`. A ring with no
		// material of its own still draws, in the mesh's material -- so the failure is a ring
		// that looks like a reach highlight, which is visible on screen and needs no line in
		// a log to find. A missing MESH draws nothing at all, which is the case that has to
		// announce itself.
		if (ObjectiveMaterial != nullptr)
		{
			ObjectiveOverlay->SetMaterial(0, ObjectiveMaterial);
		}
		// UNSET IS SILENT HERE TOO, ON THE RING'S REASONING. **[CORRECTED 2026-09-06. THIS
		// COMMENT WENT ON "WITH ONE DIFFERENCE WORTH KNOWING: unset is the state this SHIPS in
		// as of 2026-09-01, because no pulse material exists in `Content/` yet. So this branch
		// is not taken on any board in the tree today and the pulse draws in `OverlayMesh`'s
		// material" -- AND THAT DIFFERENCE IS GONE: THE PULSE MATERIAL EXISTS, IT IS ASSIGNED,
		// AND THIS BRANCH IS TAKEN ON THE SHIPPED BOARD.** `MI_Overlay_BuildPulse` is set on
		// `BP_StratBoard`'s class default; a byte census of `Content/StratPlay/BP_StratBoard.uasset`
		// returns `BuildPulseMaterial` 1 and `MI_Overlay_BuildPulse` 2, the same shape the
		// long-assigned `ObjectiveMaterial` / `MI_Overlay_Objective` prints as the positive
		// control, against 0 for a fabricated name. The property's own block in the header
		// carries that reading in full. THE COMMENT IS CORRECTED AND NOT DELETED BECAUSE THE
		// BRANCH ITSELF IS UNCHANGED AND STILL EARNS ITS SILENCE: unset stays legitimate for
		// any OTHER Blueprint of this class, and the failure it produces there is a pulse that
		// looks like a reach highlight -- wrong on screen rather than absent from it, which is
		// the ring's own trade and the reason neither is logged while a missing `OverlayMesh`
		// is. What was a content-lane handoff here is discharged.]**
		if (BuildPulseMaterial != nullptr)
		{
			BuildPulseOverlay->SetMaterial(0, BuildPulseMaterial);
		}
	}
	else
	{
		UE_LOG(LogStratPlay, Log,
			TEXT("Board '%s' has no OverlayMesh set; reach, target, objective-ring and "
			     "BUILD-pulse highlights will not draw."),
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

void AStratBoardActor::AssignTerrainMesh(
	UHierarchicalInstancedStaticMeshComponent& Component, FName TerrainId) const
{
	// THE ONLY PLACE THAT DECIDES WHICH MESH A TILE LAYER WEARS, extracted rather than
	// repeated because the defect this closes WAS two sites disagreeing about that: creation
	// assigned a mesh and the find path did not, so a component created before its mesh
	// existed never acquired one. Two call sites reading one procedure cannot drift again.
	if (const TObjectPtr<UStaticMesh>* const Configured = TerrainMeshes.Find(TerrainId))
	{
		Component.SetStaticMesh(*Configured);
	}
	else if (FallbackTerrainMesh != nullptr)
	{
		Component.SetStaticMesh(FallbackTerrainMesh);
	}
}

FStratTerrainLayer& AStratBoardActor::LayerFor(FName TerrainId)
{
	for (FStratTerrainLayer& Existing : TerrainLayers)
	{
		if (Existing.TerrainId == TerrainId)
		{
			// THE FIND PATH RE-READS THE MESH CONFIGURATION TOO, WHICH IT DID NOT USED TO.
			// This method assigned a mesh at creation and nowhere else, so `TerrainMeshes` or
			// `FallbackTerrainMesh` set after a layer's component existed -- by a fixture, or
			// by an editor assignment on a live board -- reached no component, and `ApplyHexes`
			// skipped and re-reported every hex of that terrain on every refresh thereafter.
			// That defect predates the `DrawsExactlyTheseHexes` early-out and was neither
			// caused nor cured by it; it is cured here.
			//
			// UNCONDITIONAL, AND THE COST IS MEASURED RATHER THAN ASSUMED. This runs once per
			// HEX, not once per layer, so "is it free when nothing changed" is the only
			// question worth asking. `UStaticMeshComponent::SetStaticMesh` opens with
			// `if (NewMesh == GetStaticMesh()) { return false; }` (UE 5.8,
			// `StaticMeshComponent.cpp`), so an unchanged configuration costs one `TMap::Find`
			// and one pointer compare and touches neither the component nor the renderer.
			//
			// AND THERE IS NO INSTANCE STATE HERE TO DISTURB, which is the other thing a reader
			// will want to check before widening or narrowing this: the ONLY caller is
			// `ApplyHexes`, below, which returns at its early-out before reaching this and then
			// runs `ClearInstances()` on every layer before the loop that calls it. A no-op
			// refresh never executes this line at all.
			//
			// AN EARLIER VERSION OF THIS GUARD READ `... && GetStaticMesh() == nullptr` and
			// justified itself with the opposite of both facts above -- that an unconditional
			// re-read would hit components "already drawing instances on every hex of every
			// refresh" and that a HISM drops its instances when its mesh is set. The first is
			// false of this call site by the ordering just described; the second is not
			// supported by any override in `InstancedStaticMesh.cpp`, which does not touch the
			// mesh-change path at all. `strat-integration-reviewer` blocked on it. It is
			// recorded here rather than merely deleted because the narrowing it argued for
			// would have left half the defect standing.
			if (Existing.Tiles != nullptr)
			{
				AssignTerrainMesh(*Existing.Tiles, TerrainId);
			}

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

	AssignTerrainMesh(*Component, TerrainId);

	Component->RegisterComponent();

	NewLayer.Tiles = Component;

	const int32 Added = TerrainLayers.Add(MoveTemp(NewLayer));
	return TerrainLayers[Added];
}

bool AStratBoardActor::DrawsExactlyTheseHexes(const TArray<FStratHexView>& Hexes) const
{
	// A CURSOR PER LAYER, WALKED IN THE MODEL'S ORDER. `ApplyHexes` appends each hex to its
	// terrain's layer in the order the model lists it, so the drawn state equals the model
	// exactly when consuming the model that way lands on each layer's array element by
	// element and finishes every one of them. Inline for seven terrain kinds; the board is
	// 99 hexes and Ferrum Crossing is the SMALL one, so this is a walk of the same length as
	// the rebuild it is deciding whether to skip -- and it adds nothing to the renderer.
	TArray<int32, TInlineAllocator<8>> Cursors;
	Cursors.SetNumZeroed(TerrainLayers.Num());

	for (const FStratTerrainLayer& Layer : TerrainLayers)
	{
		// A LAYER WITH NO COMPONENT CANNOT VOUCH FOR WHAT IS ON SCREEN, and neither can one
		// whose component disagrees with the parallel array -- that is the exact condition
		// `ApplyHexes` keeps true by construction, so if it is false here something outside
		// this class cleared the component and the rebuild must run.
		if (Layer.Tiles == nullptr || Layer.Tiles->GetInstanceCount() != Layer.InstanceHexes.Num())
		{
			return false;
		}
	}

	for (const FStratHexView& Hex : Hexes)
	{
		int32 LayerIndex = INDEX_NONE;
		for (int32 Index = 0; Index < TerrainLayers.Num(); ++Index)
		{
			if (TerrainLayers[Index].TerrainId == Hex.TerrainId)
			{
				LayerIndex = Index;
				break;
			}
		}

		// A terrain kind this board has never drawn -- the first call, or a scenario with a
		// kind the last one lacked. `LayerFor` creates it during the rebuild.
		if (LayerIndex == INDEX_NONE)
		{
			return false;
		}

		const FStratTerrainLayer& Layer = TerrainLayers[LayerIndex];
		int32& Cursor = Cursors[LayerIndex];

		// COVERS THE UNMESHED CASE WITHOUT NAMING IT, AND COVERS ONLY ITS OWN HALF OF IT.
		// A terrain whose mesh was unset when the board was last drawn contributed no
		// instances at all, so its first hex runs off the end of an empty array here and this
		// returns false -- the early-out never remembers an undrawn board as drawn, and that
		// is the whole of what this procedure is accountable for in the unmeshed case.
		//
		// WHAT THE REBUILD THEN DOES IS `LayerFor`'S AND NOT THIS BLOCK'S. The history is kept
		// because the sentence that once stood here claimed the second half from the first:
		// RETRACTED>  "-- which is what makes a mesh assigned after the fact take effect on
		// RETRACTED>   the very next refresh."
		// That was TRUE OF THE EARLY-OUT AND FALSE OF THE BOARD on the day it was written:
		// `LayerFor` returned an existing layer by `TerrainId` before it reached its
		// `SetStaticMesh` calls, so a component created during an unmeshed apply kept a null
		// mesh for the life of the actor and `ApplyHexes`'s `GetStaticMesh() == nullptr` arm
		// skipped and re-reported every hex of that terrain on every refresh thereafter. THE
		// BOARD NOW DOES TAKE A LATE MESH -- `LayerFor`'s find path re-reads the mesh
		// configuration, and `GATE-BOARDCHURN.AMeshAssignedAfterAnUnmeshedApplyDrawsOnTheNextApply`
		// goes red if that stops being true -- but it is still not this block that provides it,
		// and no sentence here may claim it. **THE CONDITION UNDER WHICH IT RE-READS IS NOT
		// RESTATED HERE, DELIBERATELY.** An earlier draft of this sentence spelled it out, the
		// condition then changed, and this line went stale without any diff touching it -- twice,
		// in consecutive gates. NO OTHER COMMENT IN THIS CLASS RESTATES THE CONDITION --
		// `LayerFor`'s declaration and definition are the only two, and they are the same
		// function. THE QUANTIFIER IS "IN THIS CLASS" AND NOT "IN THE TREE", because that is
		// what was actually established: `Tools/architect/state/global.md`'s banner states the
		// condition too, correctly and in present tense, and a record entry describing what a
		// pass changed is the right home for it -- but it is a copy, and it needs stamping the
		// day this moves. An earlier draft of this sentence claimed the tree-wide property and
		// was over-broad by exactly one site.
		if (!Layer.InstanceHexes.IsValidIndex(Cursor) || Layer.InstanceHexes[Cursor] != Hex.Hex)
		{
			return false;
		}

		++Cursor;
	}

	for (int32 Index = 0; Index < TerrainLayers.Num(); ++Index)
	{
		// Instances the model no longer accounts for. Without this a shrinking board would
		// keep drawing -- and keep PICKING -- hexes the rules module has stopped describing.
		if (Cursors[Index] != TerrainLayers[Index].InstanceHexes.Num())
		{
			return false;
		}
	}

	return true;
}

bool AStratBoardActor::ApplyHexes(const TArray<FStratHexView>& Hexes, FString& OutFailureReason)
{
	OutFailureReason.Reset();

	// ASKED BEFORE ANYTHING IS CLEARED. The declaration carries the measurement and the
	// regression that made this necessary; the shape to keep in mind while reading below is
	// that this is NOT a delta path -- it does not compute what changed and apply the
	// difference. It answers one yes/no question, and a "no" runs the identical whole-board
	// rebuild this method has always run.
	if (DrawsExactlyTheseHexes(Hexes))
	{
		return true;
	}

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

void AStratBoardActor::FillOverlay(UHierarchicalInstancedStaticMeshComponent* Overlay,
	TArray<FIntPoint>& DrawnHexes, const TArray<FIntPoint>& Hexes)
{
	if (Overlay == nullptr)
	{
		return;
	}

	// ALREADY DRAWING EXACTLY THIS. `ApplyHexes`'s early-out, in the one line the overlays
	// need it in -- they keep no index map, so the cached list IS the whole of the state.
	// The instance-count agreement is the same guard the tiles use: it catches a component
	// cleared behind this class's back, which is the only way the cache can be a lie.
	if (Overlay->GetInstanceCount() == DrawnHexes.Num() && DrawnHexes == Hexes)
	{
		return;
	}

	Overlay->ClearInstances();

	if (Overlay->GetStaticMesh() == nullptr)
	{
		// Nothing to instance. Silent here rather than logged per call: BeginPlay already
		// said so once, and a highlight request per cursor move would say it thousands of
		// times.
		//
		// AND THE CACHE RECORDS THE EMPTINESS, NOT THE REQUEST. Caching `Hexes` here would
		// make an `OverlayMesh` assigned afterwards -- which is exactly what `BeginPlay`
		// and every fixture that sets the property do -- draw nothing forever, on an
		// early-out that believed a highlight was already on screen.
		DrawnHexes.Reset();
		return;
	}

	for (const FIntPoint& Hex : Hexes)
	{
		// The overlay plane only -- the third former copy of the formula, now the same
		// call the tiles make with a different Z.
		const FVector Local = LocalLocationOfHex(Hex, static_cast<double>(OverlayZOffset));

		Overlay->AddInstance(FTransform(Local), /*bWorldSpace=*/false);
	}

	// WHAT WAS DRAWN, AND ONLY REACHED WHEN A MESH EXISTED. See the miss case above.
	DrawnHexes = Hexes;
}

void AStratBoardActor::ShowReach(const TArray<FIntPoint>& Hexes)
{
	FillOverlay(ReachOverlay, ReachDrawnHexes, Hexes);
}

void AStratBoardActor::ClearReach()
{
	FillOverlay(ReachOverlay, ReachDrawnHexes, TArray<FIntPoint>());
}

void AStratBoardActor::ShowTargets(const TArray<FIntPoint>& Hexes)
{
	FillOverlay(TargetOverlay, TargetDrawnHexes, Hexes);
}

void AStratBoardActor::ClearTargets()
{
	FillOverlay(TargetOverlay, TargetDrawnHexes, TArray<FIntPoint>());
}

void AStratBoardActor::ShowObjective(FIntPoint Hex)
{
	// ONE HEX, THROUGH THE SAME `FillOverlay` THE OTHER TWO USE, so the ring cannot drift
	// from the highlights in how it clears or how it is offset off the tile plane. See the
	// declaration on why the parameter is a hex and not a set.
	FillOverlay(ObjectiveOverlay, ObjectiveDrawnHexes, TArray<FIntPoint>({ Hex }));
}

void AStratBoardActor::ClearObjective()
{
	FillOverlay(ObjectiveOverlay, ObjectiveDrawnHexes, TArray<FIntPoint>());
}

void AStratBoardActor::ShowBuildPulses(const TArray<FIntPoint>& Hexes)
{
	// THROUGH THE SAME `FillOverlay` THE OTHER THREE USE, so the pulse cannot drift from the
	// highlights in how it clears or how it is offset off the tile plane -- and so the
	// no-op-when-unchanged path arrives for free, which this overlay needs more than they do
	// (see `BuildPulseDrawnHexes`). THE SET IS PASSED STRAIGHT THROUGH: no sort, no dedupe,
	// no filter. It arrives in `FStratViewModel::Factories` order, which is the module's
	// canonical hex order, and re-ordering it here would make this class a second author of
	// an order `StratViewModel.h` declares load-bearing.
	FillOverlay(BuildPulseOverlay, BuildPulseDrawnHexes, Hexes);
}

void AStratBoardActor::ClearBuildPulses()
{
	FillOverlay(BuildPulseOverlay, BuildPulseDrawnHexes, TArray<FIntPoint>());
}

int32 AStratBoardActor::GetBuildPulseOverlayCount() const
{
	// Off the component and never off a cached count -- see the declaration.
	return BuildPulseOverlay != nullptr ? BuildPulseOverlay->GetInstanceCount() : 0;
}

int32 AStratBoardActor::GetObjectiveOverlayCount() const
{
	// Off the component and never off a cached count -- see the declaration.
	return ObjectiveOverlay != nullptr ? ObjectiveOverlay->GetInstanceCount() : 0;
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
