// GDD §2.4 / §4.7 Stub 8 / T-INT-05 -- one unit on the board, as one actor.
//
// WHAT GAP THIS CLOSES. `FStratUnitView` has described every living unit in engine types
// since phase 2 -- id, side, definition, hex, hp, both turn flags -- and nothing in the
// running game turned one into something visible. This is that something.
//
// ONE ACTOR PER UNIT, unlike the tiles, and the asymmetry is the shape of the problem
// rather than an inconsistency. Ninety-nine tiles never move, are never selected and are
// never destroyed; ten units do all three, individually. Instancing is worst at exactly
// that, and a per-unit actor is what lets `UStratMatchSubsystem::ApplyView` spawn, move and
// destroy against a `TMap<int32, AStratUnitActor*>` keyed by the model's own `UnitId`.
//
// IT CARRIES NO COLLISION AND THAT IS LOAD-BEARING, not a performance choice. Every rules
// question this game asks is asked about a HEX -- `SubmitMove` takes a destination hex,
// `SubmitAttack` takes a target hex, `Reachable` returns hexes -- so the cursor's job is
// to name a hex, and it does that by tracing to the board's tile instances. A unit that
// blocked the trace would shadow the tile underneath it, and the one hex a player most
// needs to click is the one an enemy is standing on. The unit is therefore transparent to
// the cursor and the hex under it answers, which also means "click the unit" and "click
// its hex" are the same act with one implementation.
//
// IT HOLDS NO STATE THE VIEW MODEL DOES NOT ALSO HOLD, and this is the sharpest constraint
// on this file. `StratViewModel.h` records the debt phase 3 inherits in as many words:
// `FStratUnitView::bDone` and `bLockedThisTurn` are declared on the MODEL "so that phase
// 3's selection machine has somewhere to put them that is not an actor, because state in
// an actor makes T-INT-05 false and nothing reports it." So the fields below are a CACHE
// OF THE LAST APPLIED VIEW and never a source: `ApplyUnitView` overwrites all of them from
// the model on every call, nothing else writes them, and no code path reads one to decide
// anything a later refresh could contradict. `T-INT-05.PresentationBitsAreDefaulted` pins
// the negative today; the moment an actor here owns a bit the model does not, that clause
// is quietly false with no compiler diagnostic.
//
// MESHES ARE KEYED BY `DefId` AND NOT BY `DefIndex`, even though DT_Units' row order IS
// pinned (`GATE-BRIDGE-DEFS`, phase 0). `StratViewModel.h` gives the reason on the field
// itself: the gate exists to catch a DATA divergence, "not to license `.uasset` arrays
// that would break loudly the day a fifth unit ships." A name-keyed map survives a row
// insertion; an array indexed by position does not, and the index remains available for
// anything that genuinely needs the position.
//
// SIDE IS A MATERIAL AND NOT A MESH, because §2.4's definitions are per-unit-type and both
// sides field the same types. The materials are an array indexed by `strat` side, which is
// the one index in this file that is safe: side indices are the rules module's own and
// `strat::SIDE_COUNT` does not vary with data.
//
// NOT IN THIS ROUND, with reasons:
// - Movement interpolation. `ApplyUnitView` teleports. Animating a move needs the ordered
//   event list §4.9 part 2 names and `StratBridge.h` records is not built -- the view model
//   says what IS, not what HAPPENED, and driving a slide off a position difference would be
//   inferring an event from a state, which is exactly the conflation that header warns
//   about.
// - The DONE / locked visual. The bits exist on `FStratUnitView` and nothing produces them
//   until phase 4's selection machine does. This class reads them and applies no treatment;
//   phase 5 decides what "done" looks like.
// - Health bars, damage numbers, hit flashes. All presentation over an event stream that
//   does not exist yet, and none of it is named by an acceptance ID in this milestone.
// - Any `/Game/` path. Every mesh and material is an EditDefaultsOnly property.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "StratViewModel.h"

#include "StratUnitActor.generated.h"

class UMaterialInterface;
class UStaticMesh;
class UStaticMeshComponent;

/**
 * The visible presence of one unit.
 *
 * Spawned, moved and destroyed by `UStratMatchSubsystem::ApplyView` to match the view
 * model. It is never spawned by hand and never placed in a level: an actor on the board
 * that no model entry produced is a unit the rules module does not have.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Strat Unit Actor"))
class STRATPLAY_API AStratUnitActor : public AActor
{
	GENERATED_BODY()

public:
	AStratUnitActor();

	/**
	 * Makes this actor look like the unit the model describes, at the world location the
	 * board computed for its hex.
	 *
	 * IDEMPOTENT AND TOTAL. Every field below is written on every call, so calling it
	 * twice with the same view is indistinguishable from calling it once, and calling it
	 * with a different view leaves nothing behind from the previous one. That is what makes
	 * "reconcile the whole screen from the model" cheap enough to do unconditionally --
	 * which is what `UStratMatchSubsystem` does, because a conditional reconcile is a
	 * reconcile that can be skipped when it was needed.
	 *
	 * THE LOCATION IS PASSED IN AND NOT COMPUTED. `AStratBoardActor::WorldLocationOfHex` is
	 * the only axial -> world conversion in the project; a second copy here would be a unit
	 * that is half a tile off the day `HexSize` changes.
	 *
	 * @param View           the model's entry for this unit. Its `UnitId` becomes this
	 *                       actor's, and the two are never allowed to disagree.
	 * @param WorldLocation  from the board actor, for `View.Hex`.
	 */
	void ApplyUnitView(const FStratUnitView& View, const FVector& WorldLocation);

	/**
	 * Which unit this actor stands for. `FStratUnitView::UnitId`, and the key
	 * `UStratMatchSubsystem` reconciles by.
	 *
	 * A CACHE OF THE LAST APPLIED VIEW, per the header block, not an authority. If this
	 * disagrees with the model, the model is right and this actor is stale.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Unit")
	int32 GetUnitId() const { return UnitId; }

	/** The last applied view, whole. Exposed so a caller that already has this actor does
	 *  not have to search the model for the entry that produced it -- the same value, not a
	 *  second one. */
	const FStratUnitView& GetLastAppliedView() const { return LastAppliedView; }

protected:
	/**
	 * The unit's body.
	 *
	 * NO COLLISION, set in the constructor and stated in the header block: the cursor must
	 * reach the tile underneath, because every rules question is asked about a hex.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Stratocracy|Unit")
	TObjectPtr<UStaticMeshComponent> Body;

	/**
	 * Mesh per §2.4 definition `id` ("Infantry", "Tank", ...).
	 *
	 * KEYED BY NAME. See the header block: DT_Units' row order is pinned by
	 * `GATE-BRIDGE-DEFS`, and that gate exists to catch a data divergence rather than to
	 * license an array here that breaks the day a fifth unit ships.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Unit")
	TMap<FName, TObjectPtr<UStaticMesh>> MeshByDefId;

	/** Drawn for a definition with no entry above. Unset means such a unit is invisible and
	 *  `ApplyUnitView` says so once per definition id. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Unit")
	TObjectPtr<UStaticMesh> FallbackMesh;

	/**
	 * Material per `strat` side, indexed by side.
	 *
	 * THE ONE POSITIONAL INDEX IN THIS FILE, and it is safe where the definition ones are
	 * not: side indices are the rules module's own and `strat::SIDE_COUNT` is not data. An
	 * index past the end leaves the mesh's own material, which is a visible "both sides look
	 * the same" rather than a crash.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Unit")
	TArray<TObjectPtr<UMaterialInterface>> SideMaterials;

	/** How far above the tile plane the body sits. Presentation; the right value depends on
	 *  the meshes phase 5 assigns. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Unit")
	float BodyZOffset = 0.0f;

private:
	/** See `GetUnitId`. Written only by `ApplyUnitView`. */
	UPROPERTY(Transient)
	int32 UnitId = INDEX_NONE;

	/**
	 * The whole of the last view applied to this actor.
	 *
	 * A CACHE AND NOT A SOURCE. Nothing in this class writes a field of it except
	 * `ApplyUnitView`, which overwrites all of it from the model; nothing decides anything
	 * from it that a later refresh could contradict. The header block records why that
	 * distinction is worth this much prose: `bDone` and `bLockedThisTurn` live on the MODEL
	 * so T-INT-05 can rebuild the screen from the model alone, and an actor that owned a
	 * copy of either would make that clause false with nothing to report it.
	 */
	UPROPERTY(Transient)
	FStratUnitView LastAppliedView;
};
