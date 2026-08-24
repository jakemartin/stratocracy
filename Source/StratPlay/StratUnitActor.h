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
//   phase 5 decides what "done" looks like. NOTE THE CONTRAST WITH THE §2.11.6-B MARKER
//   ADDED 2026-08-23, which this class DOES apply a treatment for: `bIsGuidedMarked` names
//   one unit for the whole match and its directive is unreadable without it, where `bDone`
//   and `bLockedThisTurn` change several times a turn and §2.11 has not said what they look
//   like. See `GuidedMarker`.
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
	 * THE VIEWING SIDE IS PASSED IN AND IS NOT ASKED FOR. `FStratViewModel::ViewingSide` is
	 * where it lives, and that field's own block says why an actor must not fetch it instead:
	 * "a viewing side held beside the model is a second input, and T-INT-05 would then be
	 * about two things". `UStratMatchSubsystem::GetViewingSide` was the other candidate and
	 * was refused for exactly that sentence -- reading it here would let the screen disagree
	 * with the model it was reconciled from. Only the marker reads it; see `GuidedMarker`.
	 *
	 * THE GUIDANCE-ACTIVE BIT IS PASSED IN FOR THE SAME REASON AND BY THE SAME PRECEDENT,
	 * ADDED 2026-08-24. `FStratViewModel::Guidance.bActive` off the same model, not
	 * `UStratMatchSubsystem`'s `FStratGuidedOpening` asked directly -- an actor that queried
	 * the guidance machine would be drawing from the model PLUS a live object, and could show
	 * a marker for a window the model it was handed says is closed. It is a BOOL and not the
	 * `FStratGuidanceView` itself, deliberately: handing this class the whole struct would put
	 * `ObjectiveHex` within reach of a marker writer, which is precisely the hex-keyed
	 * derivation `GuidedMarker`'s block forbids by name. What it cannot see, it cannot use.
	 *
	 * @param View            the model's entry for this unit. Its `UnitId` becomes this
	 *                        actor's, and the two are never allowed to disagree.
	 * @param WorldLocation   from the board actor, for `View.Hex`.
	 * @param ViewingSide     `FStratViewModel::ViewingSide` off the SAME model `View` came
	 *                        from. A value that is no side at all -- `INDEX_NONE` -- shows no
	 *                        marker, which is the safe direction and is not a sentinel: no
	 *                        real side equals it, so nothing is collapsed.
	 * @param bGuidanceActive `FStratViewModel::Guidance.bActive` off that same model. False
	 *                        shows no marker on any unit, which is §2.11.6's own sentence
	 *                        ("the turn-1a unit marker clear[s] in the same frame as the
	 *                        strip") and is the only operand of the three that can ever go
	 *                        false mid-match.
	 */
	void ApplyUnitView(const FStratUnitView& View, const FVector& WorldLocation, int32 ViewingSide,
	                   bool bGuidanceActive);

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

	/**
	 * Whether §2.11.6-B's turn-1a marker is showing on this unit.
	 *
	 * OFF THE COMPONENT AND NOT OFF `LastAppliedView.bIsGuidedMarked`, and the distinction
	 * is the whole value of the accessor. Answering from the cached view would report what
	 * this actor was TOLD and pass whether or not anything reached the screen -- the
	 * lazily-armed-subject shape that makes a clause unable to fail. `IsVisible()` is what a
	 * player would see.
	 *
	 * [CORRECTED 2026-08-23 BY THIS BLOCK'S OWN AUTHOR. THE SENTENCE BELOW WAS FALSE, IT WAS
	 * NEVER MEASURED, AND IT PROPAGATED OFF THIS FILE INTO A DISPATCH BRIEF AND FROM THERE
	 * INTO A TEST AUTHOR'S INSTRUCTIONS.] It said:
	 * RETRACTED> "FALSE WITH NO MARKER MESH ASSIGNED, which is the state this ships in until
	 * RETRACTED>  the content lane assigns one. That is honest rather than a defect: nothing
	 * RETRACTED>  is drawn, so nothing is showing. A clause distinguishing 'not marked' from
	 * RETRACTED>  'marked but unconfigured' must assign `GuidedMarkerMesh` on the spawned
	 * RETRACTED>  actor first."
	 * `USceneComponent::IsVisible` consults `bHiddenInGame`, the visible flag and the cached
	 * level collection -- and NOT the static mesh. So this answers TRUE for a marked unit
	 * whose marker has no mesh and draws nothing. Measured by `strat-test-author` in the
	 * suite, not argued: a fixture assigning no mesh reads true for the marked units.
	 * The mandated "assign the mesh first" step rested on the retracted sentence and is
	 * withdrawn with it -- the marked/unmarked discrimination is available on an
	 * unconfigured actor.
	 *
	 * WHAT THIS ACCESSOR THEREFORE CANNOT DO, WHICH IS THE HONEST LIMIT OF THE WHOLE SEAM:
	 * IT REPORTS A FLAG, NOT PIXELS. It cannot tell anyone whether `GuidedMarkerMesh` was
	 * ever assigned, and it cannot tell anyone that a marker reached the screen. **There is
	 * no headless gate on "the marker is actually visible to a player"** and none is
	 * available from this class -- that needs the content lane's defaults plus a human at
	 * the keyboard. Anything claiming otherwise off this function is over-reading it.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Unit")
	bool IsGuidedMarkerVisible() const;

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
	 * §2.11.6-B's turn-1a marker: the thing that makes "Select the marked Infantry" readable.
	 *
	 * WHAT GAP THIS CLOSES. Beat 1a's directive names a marked unit and, until this landed,
	 * nothing on screen was marked. Confirmed at the keyboard on 2026-08-23. The rules module
	 * has published the fact since phase 2 -- `FStratUnitView::bIsGuidedMarked` -- and this
	 * actor was already handed it on every refresh and threw it away.
	 *
	 * ITS VISIBILITY IS A PASS-THROUGH OF THREE PUBLISHED FIELDS ANDed, AND NONE IS
	 * DERIVED. `ApplyUnitView` sets it from `bGuidanceActive && View.bIsGuidedMarked &&
	 * View.Side == ViewingSide` and from nothing else. THE COUNT WAS ONE UNTIL 2026-08-23 and
	 * TWO UNTIL 2026-08-24; this block said so, in that order:
	 * RETRACTED> "ITS VISIBILITY IS A PASS-THROUGH OF ONE PUBLISHED FIELD ... `ApplyUnitView`
	 * RETRACTED>  sets it from `View.bIsGuidedMarked` and from nothing else."
	 * RETRACTED> "ITS VISIBILITY IS A PASS-THROUGH OF TWO PUBLISHED FIELDS ANDed ...
	 * RETRACTED>  `ApplyUnitView` sets it from `View.bIsGuidedMarked && View.Side ==
	 * RETRACTED>  ViewingSide` and from nothing else."
	 *
	 * THE THIRD FIELD IS A DEFECT FOUND IN A HUMAN PLAYTEST, AND IT IS THE ONLY OPERAND THAT
	 * CAN EVER MOVE. Added 2026-08-24, `FStratViewModel::Guidance.bActive`. With only the
	 * first two, EVERY OPERAND WAS CONSTANT FOR THE WHOLE MATCH -- `bIsGuidedMarked` is
	 * derived off `placement` and therefore deliberately never moves, and both sides are
	 * fixed -- so the conjunction could not go false once true and the marker stayed lit
	 * after §2.11.6's window closed. The player saw the objective ring clear with the
	 * Infantry's marker still on. **THE BOTH-DIRECTIONS DISCIPLINE HAD BEEN APPLIED TO THE
	 * WRITE AND NOT TO THE OPERAND SET**, which is how a correctly-written unconditional
	 * writer produced a latch; that is the durable finding, not the missing bit.
	 *
	 * `bActive` AND NOT "BEAT 1a RETIRED", AND THE GDD PICKED IT. §2.11.6 says the ring "and
	 * the turn-1a unit marker clear in the same frame as the strip", and the strip is
	 * `bActive`. It is also what makes the ring and the marker structurally simultaneous:
	 * `FStratGuidedOpening` writes `bHasObjective = false` in exactly three places and all
	 * three set `bActive = false` beside it, and `DecorateViewModel` publishes the guidance
	 * block whole -- so a frame in which the ring is out is a frame in which `bActive` is
	 * out, and now a frame in which no marker draws.
	 *
	 * THE SECOND FIELD IS A USER RULING AND NOT A READING. `Data/ferrum_crossing.json`
	 * authors a `guidedOpening` for BOTH seats, so TWO units carry `bIsGuidedMarked` at once
	 * -- measured by `strat-test-author`, who saw unit 3 (side 0) and unit 7 (side 1) both
	 * marked in one frame -- and the unfiltered pass-through therefore marked the ENEMY
	 * seat's Infantry on the player's screen. The user ruled it out: a marker that says
	 * "select this" pointing at a unit the player cannot select is confusing.
	 *
	 * IT IS STILL A PASS-THROUGH AND NEITHER THE RULING NOR THE 2026-08-24 FIX COST THAT. All
	 * three operands are fields this actor was handed off ONE model -- `View.bIsGuidedMarked`
	 * and `View.Side` on the unit entry, `ViewingSide` and `bGuidanceActive` off the model
	 * that entry came from; nothing is computed, nothing is looked up, and no
	 * hex is compared. IN PARTICULAR IT IS NEVER RE-DERIVED FROM A HEX COMPARISON against
	 * `guidedOpening.infantry`: the rules module derives that flag off `placement`, and
	 * `StratGuidedOpening.h` records why -- beat 1a's entire content is that the marked
	 * Infantry MOVES, so a hex-keyed derivation would unmark it at the exact moment the mark
	 * is needed. This class holds no bridge and no scenario and could not make that mistake
	 * cheaply, which is the arrangement working rather than luck.
	 *
	 * IT ADDS NO STATE THE MODEL DOES NOT HOLD. The header block's sharpest constraint is
	 * intact: nothing here is a bit an actor owns. Component visibility is a rendering of
	 * fields of the applied model -- COUNT-FREE ON PURPOSE, because this block has now had to
	 * move that number twice and the invariant was never the count; it is that every operand
	 * comes off ONE model and none is remembered -- overwritten totally on every
	 * `ApplyUnitView`, and
	 * `T-INT-05`'s "rebuild the screen from the view model alone" stays true by construction
	 * -- WHICH IS PRECISELY WHY THE VIEWING SIDE COMES FROM `FStratViewModel::ViewingSide`
	 * AND NOT FROM `UStratMatchSubsystem::GetViewingSide`. The subsystem's member is the same
	 * number today and is a SECOND INPUT in principle; reading it here would make the screen
	 * a function of the model plus one other thing, which is the exact wording that field's
	 * own block uses to forbid it.
	 *
	 * NO COLLISION, for `Body`'s reason and not a weaker one: the cursor must reach the tile
	 * underneath, and a marker is by definition on the one unit a player is being told to
	 * click.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Stratocracy|Unit")
	TObjectPtr<UStaticMeshComponent> GuidedMarker;

	/** The marker's mesh. ASSIGNED AS OF 2026-08-24 AND THAT IS THE STATE THIS SHIPS IN --
	 *  `BP_StratUnit`'s default carries `SM_GuidedMarker` here, and `MI_Marker_Guided` in
	 *  `GuidedMarkerMaterial` below. The mesh and its assignment stay the CONTENT lane's, and
	 *  this file must not name a `/Game/` path to fill it -- which is why it is still
	 *  `EditDefaultsOnly` and still has no initializer. This block said unset was the shipping
	 *  state until the default landed. Unset remains LEGITIMATE for any other Blueprint of
	 *  this class, and it means the marker never draws -- but
	 *  `IsGuidedMarkerVisible` STILL ANSWERS TRUE, which is the correction that function's
	 *  block now carries; this sentence used to claim it answered false. It is reported once
	 *  per actor at spawn precisely because no accessor can tell the two apart: the LOG is
	 *  the only place "the marker is unconfigured" is distinguishable from "nothing is
	 *  marked". */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Unit")
	TObjectPtr<UStaticMesh> GuidedMarkerMesh;

	/** Optional material override for the marker. Unset leaves the mesh's own. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Unit")
	TObjectPtr<UMaterialInterface> GuidedMarkerMaterial;

	/**
	 * How far above the body the marker floats. Presentation, and still EditDefaultsOnly so
	 * the content lane keeps the last word.
	 *
	 * WAS 150.0f, WHICH PUT THE MARKER ON TOP OF THE UNIT IT NAMES. `content.md` filed that
	 * as an eyeball observation -- "at this camera pitch it OCCLUDES the unit" -- and it is
	 * now DERIVED, because every operand became a measured number once the meshes landed:
	 * `SM_GuidedMarker` is a 100 uu cone centred on its origin, `FallbackMesh` is
	 * `/Engine/BasicShapes/Cylinder`, 100 uu and also centred, and `AStratCameraPawn` looks
	 * down `ArmPitch` = -60 with NO YAW on any path (`bInheritYaw` is false, the arm's
	 * rotation is `FRotator(ArmPitch, 0, 0)`, and `PanBy` is a world offset that never
	 * rotates the pawn). So screen-up is `0.866*x + 0.5*z` in body space and depth is
	 * `0.5*x - 0.866*z`. The body's silhouette tops out at 68.3; the marker's base rim
	 * bottoms out at `0.5*(Z - 50) - 43.3`. Equal at Z = 273.2. AT 150 THE MARKER'S BASE SAT
	 * 61.6 BELOW THE BODY'S TOP AND 111.6 NEARER THE CAMERA, so it drew in front of the head
	 * of the unit -- the one thing a "this unit" marker must not hide. 300 clears by 13.4,
	 * about 10% past contact.
	 *
	 * THE COST, STATED RATHER THAN DISCOVERED LATER: a marker at positive Z is always nearer
	 * the camera than anything up-screen of it, so it draws over the LOWER BODY OF THE UNIT
	 * ON THE HEX BEHIND. That trade is deliberate. The defect was the marker hiding its own
	 * subject, which makes it unreadable; crowding a neighbour leaves it readable.
	 *
	 * THE ARITHMETIC ASSUMES THE SHIPPED CAMERA PITCH AND UNIT SCALE, and this file cannot
	 * see either -- it holds no camera pointer and reads no component scale. That is why the
	 * number is a default and not a constant: change `ArmPitch`, or scale `Body` or
	 * `GuidedMarker` on the Blueprint, and this wants re-deriving. NO TEST PINS IT. `T-INT-05`
	 * compares unit Z deltas against each other and never names a marker offset, so nothing
	 * headless goes red if it drifts; the gate is a human eye, as it was for the marker
	 * itself.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Unit")
	float GuidedMarkerZOffset = 300.0f;

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

	/** Applies the marker's Blueprint-default mesh, material and offset. Overridden for the
	 *  reason `AStratBoardActor::BeginPlay` gives: a constructor reading a Blueprint default
	 *  runs on the CDO and sets it on the wrong object. */
	virtual void BeginPlay() override;

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
