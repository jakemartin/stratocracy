// GDD §2.2 / §4.7 Stub 8 / T-UI-02 -- the board, as ONE actor.
//
// WHAT GAP THIS CLOSES. `FStratViewModel::Hexes` has been a complete, reflected
// description of all 99 hexes since phase 2, and nothing could draw it: there was no
// object in the running game that turned an axial coordinate into a world location, no
// object that owned tile geometry, and therefore no way for a cursor to name a hex. This
// class is that object, and it is the ONLY place in the project where hex axial space and
// Unreal world space meet.
//
// ONE ACTOR, NOT NINETY-NINE. A per-hex actor would be 99 transforms, 99 GC references
// and 99 registrations to reconcile every refresh, and Ferrum Crossing is the SMALL
// board. Tiles are instances of hierarchical instanced static meshes instead, grouped one
// component per terrain kind, which is also what makes a per-terrain material assignment
// a property rather than a per-instance override.
//
// THE GROUPS ARE KEYED BY `TerrainId` (an FName) AND NEVER BY `TerrainIndex`, and that is
// a ruling this class inherits rather than a preference it forms. `StratViewModel.h` on
// `FStratHexView::TerrainIndex`: the index is "LIVE WITHIN ONE BUILD ONLY ... Key
// presentation off `TerrainId` below, never off this", because DT_Terrain's row order was
// ruled not load-bearing on evidence in phase 0 and nothing pins it. An array of seven
// meshes indexed by row position would draw Water as Woods after a table edit, silently
// and with no diagnostic; a TMap keyed by name cannot, because the name is what seeding
// itself resolves terrain by (`strat::seedFromScenario`'s terrain-id resolution loop).
//
// AND THEREFORE THE SEVEN COMPONENTS ARE CREATED FROM DATA, NOT DECLARED IN C++. Ferrum
// Crossing ships seven terrain kinds and this actor ends up with seven tile components,
// but the number and the names come out of the view model at runtime. Writing
// `Plains/Woods/Mountains/Water/Town/Bridge/Factory` into seven `CreateDefaultSubobject`
// calls would make this file a second author of the §4.8 terrain table -- a scenario that
// added a terrain kind would need a C++ edit, which is the property
// `StratScoreboardHUD.cpp` states for itself ("a scenario edit must not require a C++
// edit"). The cost is that the components do not exist in the Blueprint's component tree
// to be styled individually; the answer to styling is `TerrainMeshes`, which is keyed the
// same way the data is.
//
// THE TWO OVERLAYS ARE DECLARED IN C++, and the asymmetry is deliberate. `ReachOverlay`
// and `TargetOverlay` are not data -- there are exactly two of them for as long as §2.11.1
// has a move phase and an attack phase, they need distinct materials a designer will want
// to see in the details panel, and neither is keyed by anything. They are constructor
// subobjects.
//
// PICKING IS AN INSTANCE-INDEX LOOKUP AND NOT A DISTANCE SEARCH. A trace against the tile
// components returns a component and an instance index; `HexAtInstance` maps that pair
// back to the axial coordinate that produced it, through the per-layer `InstanceHexes`
// array this class fills as it adds instances. Nothing rounds a world location back into
// hex space, and nothing scans 99 hexes for the nearest -- both would be a second,
// approximate inverse of `WorldLocationOfHex` that could disagree with it at a tile edge.
// `HexAtWorldLocation` is deliberately absent for that reason.
//
// UNITS ARE NOT DRAWN HERE AND CARRY NO COLLISION. `AStratUnitActor` is a separate actor
// per unit -- units move, are selected and are destroyed individually, which is the case
// instancing is worst at -- and its header records why the cursor must never hit one: a
// unit that blocked the trace would shadow the hex underneath it, and every rules question
// this game asks is asked about a HEX.
//
// IT ASKS NOTHING AND DECIDES NOTHING. This class holds no bridge, submits no command and
// computes no reachable set. `ShowReach` takes a set of hexes it was handed; the set comes
// from `FStratBridge::Reachable` and from nowhere else, because a hex-distance filter
// standing in for that query is the exact substitution T-UI-02 was written to catch, and a
// board actor that could compute a highlight is a board actor that would eventually be
// asked to.
//
// NOT IN THIS ROUND, with reasons:
// - Selection state, hover state, the cursor. Phase 4's PlayerController owns input and
//   the selection machine; this class exposes the two overlays it drives and nothing more.
// - Any `/Game/` path. Every mesh and material below is an EditDefaultsOnly property set
//   on a Blueprint default in phase 5. There is not one ConstructorHelpers lookup here and
//   there must never be one.
// - Elevation. `WorldLocationOfHex` is planar (Z = 0 in actor space). Terrain height is a
//   presentation feature no acceptance ID names, and a Z that varied by terrain would make
//   the unit actors' placement a function of the terrain table.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "StratViewModel.h"

#include "StratBoardActor.generated.h"

class UHierarchicalInstancedStaticMeshComponent;
class UMaterialInterface;
class UStaticMesh;

/**
 * One terrain kind's tiles: the component that draws them, and the axial coordinate each
 * of its instances came from.
 *
 * `InstanceHexes` IS THE INVERSE OF THE ADD ORDER AND NOTHING ELSE. Element i is the hex
 * that produced instance i, appended as the instance was added, so a hit result naming
 * instance i names that hex exactly. It is a parallel array rather than a per-instance
 * custom float because the mapping must survive being read by a trace on any frame and
 * custom data is renderer state.
 *
 * REBUILT WHOLE, NEVER PATCHED. `ApplyHexes` clears every layer and re-adds, so the
 * invariant "instance count equals InstanceHexes count" holds by construction instead of
 * by every caller remembering to keep two containers in step. Removing a single instance
 * from a HISM renumbers the ones after it, which is precisely the bug a parallel array
 * patched in place would acquire.
 */
USTRUCT()
struct FStratTerrainLayer
{
	GENERATED_BODY()

	/** The terrain definition's own `id` ("Plains", "Bridge", ...). The key, per the
	 *  header block: never `FStratHexView::TerrainIndex`. */
	UPROPERTY()
	FName TerrainId;

	/** Created at runtime, owned by the board actor, one per distinct TerrainId seen. */
	UPROPERTY()
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> Tiles;

	/** Element i is the hex that produced instance i of `Tiles`. */
	UPROPERTY()
	TArray<FIntPoint> InstanceHexes;
};

/**
 * The playing surface: every hex of the board, the reach overlay and the target overlay.
 *
 * Spawned by `UStratMatchSubsystem`, which also drives it -- see that class for why the
 * board is spawned from the subsystem rather than placed in the level.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Strat Board Actor"))
class STRATPLAY_API AStratBoardActor : public AActor
{
	GENERATED_BODY()

public:
	AStratBoardActor();

	/**
	 * Rebuilds every tile from the view model's hex list.
	 *
	 * A COMPLETE REBUILD ON EVERY CALL, and that is the reconciliation posture rather
	 * than a shortcut. `StratViewModel.h` states that the model "is a COMPLETE DESCRIPTION
	 * OF WHAT SHOULD BE ON SCREEN AND NEVER A DELTA"; a board that patched itself towards
	 * the model would be correct only if every previous frame had also been correct, which
	 * is the property T-INT-05 exists to make structurally impossible to lose. Terrain does
	 * not change during a match today, so the cost is paid once; when a §2.7 capture starts
	 * repainting a hex, this call already handles it with no new path.
	 *
	 * A HEX WHOSE `TerrainId` HAS NO MESH IS STILL DRAWN, using `FallbackTerrainMesh`, and
	 * it is REPORTED. Skipping it would leave a hole in the board that reads as a rules
	 * consequence -- a missing hex is not a thing this game has -- and refusing the whole
	 * board over one unassigned mesh would make a presentation gap look like a seeding
	 * failure. The reason names every terrain id that fell through, once.
	 *
	 * @param Hexes             the view model's `Hexes` array, in its canonical order.
	 * @param OutFailureReason  empty on a clean rebuild; otherwise which terrain ids had
	 *                          no mesh. A non-empty reason does NOT mean nothing was drawn.
	 */
	bool ApplyHexes(const TArray<FStratHexView>& Hexes, FString& OutFailureReason);

	/**
	 * The world location of a hex's centre.
	 *
	 * THE ONLY AXIAL -> WORLD CONVERSION IN THE PROJECT. Pointy-top axial, X = q and
	 * Y = r (`strat::Hex`, mirrored by `FStratHexView::Hex`). Every actor that needs to
	 * stand on a hex asks this rather than repeating the formula, because two copies of a
	 * layout constant is a board whose units are half a tile off after someone changes
	 * `HexSize` in one of them.
	 *
	 * PLANAR. Z is the actor's own. See the header block on why terrain does not vary it.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Board")
	FVector WorldLocationOfHex(FIntPoint Hex) const;

	/**
	 * Which hex an instanced-mesh hit refers to.
	 *
	 * TAKES THE COMPONENT AND THE INDEX A HIT RESULT CARRIES, so a caller passes what a
	 * trace gave it and performs no interpretation of its own. Returns false for a
	 * component this board does not own and for an index outside that layer -- both are
	 * "the cursor is not on the board", which is an ordinary answer and not a fault.
	 *
	 * NOT AN INVERSE OF `WorldLocationOfHex`, deliberately, and the header block records
	 * why: a second, rounding inverse would disagree with the forward map at tile edges,
	 * and disagreeing about which hex the cursor is on is how a player attacks the wrong
	 * unit.
	 */
	bool HexAtInstance(const UPrimitiveComponent* Component, int32 InstanceIndex, FIntPoint& OutHex) const;

	/**
	 * Draws the movement overlay on exactly these hexes, replacing whatever was there.
	 *
	 * THE SET IS THE CALLER'S AND IS NEVER COMPUTED HERE. It comes from
	 * `FStratBridge::Reachable`, whose own header records what routing it exists to
	 * foreclose: "a hex-distance filter standing in for a real query ... `distance <= move`
	 * agrees with the rules on an empty plain and lies everywhere else." This class cannot
	 * compute the set -- it holds no bridge -- and that inability is the point.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Board")
	void ShowReach(const TArray<FIntPoint>& Hexes);

	/** Clears the movement overlay. Separate from `ShowReach({})` only in intent; both do
	 *  the same thing, and this one says what it means at the call site. */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Board")
	void ClearReach();

	/** As `ShowReach`, for §2.6's attack targets. A SECOND COMPONENT rather than a second
	 *  material on one, because §2.11.1 shows both at once during a move-then-attack and a
	 *  single component cannot carry two materials over disjoint instance sets. */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Board")
	void ShowTargets(const TArray<FIntPoint>& Hexes);

	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Board")
	void ClearTargets();

	/** How many hexes are currently drawn. Exposed so a caller can tell "the board was
	 *  never built" from "the board was built and is empty" without reaching into the
	 *  layers, which is the same distinction `FStratBridge::MakeUiSnapshot` refuses to let
	 *  anyone lose. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Board")
	int32 GetDrawnHexCount() const;

protected:
	/** Nothing but a transform. The tile components are runtime-created and attach here;
	 *  the two overlays are constructor subobjects and attach here too. */
	UPROPERTY(VisibleAnywhere, Category = "Stratocracy|Board")
	TObjectPtr<USceneComponent> BoardRoot;

	/**
	 * §2.11.1's movement highlight. Filled by phase 4's selection machine from
	 * `FStratBridge::Reachable`; empty until then.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Stratocracy|Board")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> ReachOverlay;

	/** §2.6's attack targets. See `ShowTargets` on why this is a second component. */
	UPROPERTY(VisibleAnywhere, Category = "Stratocracy|Board")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> TargetOverlay;

	// ---- Configuration. All EditDefaultsOnly, all set on a Blueprint default. --------

	/**
	 * Centre-to-centre spacing, in Unreal units, of two adjacent hexes.
	 *
	 * A LAYOUT CONSTANT AND NOT A GAMEPLAY VALUE. No rule reads it, no snapshot field
	 * corresponds to it, and changing it moves the camera's problem and nothing else. It
	 * is a property rather than a constant so that it can be matched to whatever tile mesh
	 * phase 5 assigns without a rebuild.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Board")
	float HexSize = 100.0f;

	/**
	 * Tile mesh per terrain `id`.
	 *
	 * KEYED BY NAME, PER THE HEADER BLOCK. This is the map the ruling on
	 * `FStratHexView::TerrainIndex` asks for, and its keys are the §4.8 terrain table's
	 * own ids. An unassigned key is a presentation gap `ApplyHexes` reports and covers
	 * with `FallbackTerrainMesh`; it is not a seeding failure.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Board")
	TMap<FName, TObjectPtr<UStaticMesh>> TerrainMeshes;

	/** Drawn where `TerrainMeshes` has no entry. Unset is legitimate -- then such hexes
	 *  are counted, named in the reason, and not drawn. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Board")
	TObjectPtr<UStaticMesh> FallbackTerrainMesh;

	/** The flat quad both overlays instance. One mesh, two components, two materials --
	 *  see `ShowTargets`. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Board")
	TObjectPtr<UStaticMesh> OverlayMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Board")
	TObjectPtr<UMaterialInterface> ReachMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Board")
	TObjectPtr<UMaterialInterface> TargetMaterial;

	/** How far above the tile plane the overlays sit, so they do not z-fight the tiles.
	 *  Presentation, exposed because the right value depends on the meshes phase 5
	 *  assigns and this file must not guess it for them. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Board")
	float OverlayZOffset = 2.0f;

	virtual void BeginPlay() override;

private:
	/**
	 * One entry per distinct terrain id currently drawn. Created by `ApplyHexes`, in the
	 * order the ids are first encountered in the model's canonical hex order -- which
	 * makes it deterministic without this class sorting anything, the same property
	 * `FStratViewModel` keeps about its own arrays.
	 */
	UPROPERTY()
	TArray<FStratTerrainLayer> TerrainLayers;

	/**
	 * The axial -> local-space conversion, and now the ONLY copy of the formula.
	 *
	 * WHY THIS EXISTS AT ALL, recorded because it closes a finding rather than adding a
	 * feature. The header block above claims `WorldLocationOfHex` is "the only axial ->
	 * world conversion in the project", and the phase 3 gate measured that the file
	 * contained THREE copies of the expressions -- `WorldLocationOfHex`, `ApplyHexes` and
	 * `FillOverlay` -- differing only in Z. They could not disagree, because all three read
	 * the same `HexSize`; but the prose was stronger than the code, and the next edit to
	 * either constant would have had to find all three. Now there is one, and the claim in
	 * the header block is a fact about the file.
	 *
	 * LOCAL AND NOT WORLD, because two of the three callers want local: the tile and
	 * overlay components sit at the board's root, so they add instances in component space,
	 * and asking for a world location only to untransform it would be the same arithmetic
	 * twice. `WorldLocationOfHex` is this plus one `TransformPosition`.
	 *
	 * @param LocalZ  the plane to place it on. 0 for tiles, `OverlayZOffset` for a highlight.
	 */
	FVector LocalLocationOfHex(FIntPoint Hex, double LocalZ) const;

	/** Finds or creates the layer for a terrain id, registering the component. */
	FStratTerrainLayer& LayerFor(FName TerrainId);

	/** Points an overlay component at exactly these hexes. The shared tail of `ShowReach`
	 *  and `ShowTargets`, so the two cannot drift in how they clear or how they offset. */
	void FillOverlay(UHierarchicalInstancedStaticMeshComponent* Overlay, const TArray<FIntPoint>& Hexes) const;
};
