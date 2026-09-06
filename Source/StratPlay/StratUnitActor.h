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
// [NARROWED 2026-09-02 BY THE MOVE TWEEN, WRITTEN FLAT BECAUSE THE HEADING SENTENCE BECAME
// FALSE IN THE LETTER AND A READER WHO GREPS FOR IT MUST LAND ON THE CORRECTION.] It said:
// RETRACTED> "IT HOLDS NO STATE THE VIEW MODEL DOES NOT ALSO HOLD, and this is the sharpest
// RETRACTED>  constraint on this file."
// IT HOLDS NO *BIT* THE VIEW MODEL DOES NOT ALSO HOLD, and that is still the sharpest
// constraint on this file. `StratViewModel.h` records the debt phase 3 inherits in as many
// words: `FStratUnitView::bDone` and `bLockedThisTurn` are declared on the MODEL "so that
// phase 3's selection machine has somewhere to put them that is not an actor, because state
// in an actor makes T-INT-05 false and nothing reports it." So the model-derived fields
// below are a CACHE OF THE LAST APPLIED VIEW and never a source: `ApplyUnitView` overwrites
// all of them from the model on every call, nothing else writes them, and no code path reads
// one to decide anything a later refresh could contradict. `T-INT-05.PresentationBitsAreDefaulted`
// pins the negative today; the moment an actor here owns a bit the model does not, that
// clause is quietly false with no compiler diagnostic.
//
// THE ONE EXCEPTION IS NAMED HERE RATHER THAN LEFT TO BE FOUND.
//
// [WIDENED 2026-09-02 BY THE ROUTE TWEEN, WRITTEN FLAT BECAUSE THE EXCEPTION GREW A MEMBER AND
// A SENTENCE NAMING TWO THINGS READS AS EXHAUSTIVE.] It said:
// RETRACTED> "`TweenStartOffset` and `TweenElapsed` are state no model holds."
// `TweenOffsets`, `TweenArcLengths` and `TweenElapsed` are state no model holds --
// `TweenStartOffset` no longer exists; it was one offset and is now the polyline that replaced
// it. They are not a cache of anything and `Tick` writes them. They are bounded by exactly the
// three properties the single offset was bounded by, and the "NOT IN THIS ROUND" bullet on
// movement interpolation below argues each: the terminal value is zero -- an EMPTY array and a
// `Body` at exact relative zero -- and a clock drives them there, nothing reads any of them to
// decide anything, and none is a BIT -- no marker, mesh or material answers off them. The claim
// this file may make is therefore narrower than it was and is written narrowly wherever it
// appears: the actor TRANSFORM is a function of the last model alone, always and synchronously;
// the RENDERED position is a function of the last model alone AT REST.
//
// AND THE ROUTE ITSELF IS NOT HELD HERE, WHICH IS THE HALF THAT KEEPS THAT CLAIM CHEAP.
// `ApplyUnitView` is HANDED the waypoints, converts them to offsets, and keeps no hex, no unit
// route and no goal. Between two applies this actor cannot say where it was going -- only where
// its own picture still is relative to where it now stands.
//
// [WIDENED 2026-09-06 BY THE DAMAGE FLASH, WRITTEN FLAT BECAUSE THE PARAGRAPH ABOVE NAMES THREE
// FIELDS AND A SENTENCE LISTING THREE FIELDS READS AS EXHAUSTIVE.] It said, and still says of
// those three:
// RETRACTED> "`TweenOffsets`, `TweenArcLengths` and `TweenElapsed` are state no model holds."
// THERE IS A FOURTH SANCTIONED MEMBER AND IT IS `DamageFlashElapsed`. It is bounded by exactly
// the same three properties the tween clocks are bounded by, plus the one this file added when
// it grew a second clock, and each is argued rather than asserted:
//   - ITS TERMINAL VALUE IS EXACTLY ZERO AND IS WRITTEN, NOT APPROACHED. `FinishDamageFlash`
//     writes `0.0f`, writes `nullptr` into `Body`'s OVERLAY material, and puts `Shake` at
//     `FVector::ZeroVector` through `SetRelativeLocation_Direct` plus `UpdateComponentToWorld`
//     -- the same pair `FinishTween` uses, for the same measured ~1e-14 round-trip residue,
//     restated at that function rather than cross-referenced. There is ONE retirement path, and
//     `TickDamageFlash` reaches it on every exit TAKEN WITH A FLASH RUNNING -- completion, a
//     null `Shake`, and a `DamageFlashSeconds` written to zero mid-flash. Its fourth exit is the
//     one taken when the clock is already zero, which retires nothing because there is nothing
//     to retire and must not write `Shake` on every frame of an unrelated move tween.
//   - A CLOCK DRIVES IT THERE, NOT A LATER MODEL. `PlayDamageFlash` starts it and `Tick` ends
//     it. No `ApplyUnitView` can extend, shorten, restart or observe it; the flash is armed by
//     `UStratMatchSubsystem` alone and never from the idempotent total refresh.
//   - NOTHING READS IT TO DECIDE ANYTHING. Its readers are `TickDamageFlash`, which advances
//     it, `UpdateTickEnabled`, which asks only whether it is nonzero and can only EXTEND a tick
//     the move tween already wanted, and the two accessors, which exist on
//     `GetTweenWaypointCount`'s stated permission.
//   - IT IS NOT A BIT. It cannot make one unit draw a marker another does not, and it cannot
//     survive its own clock. `bDone`, `bLockedThisTurn` and `bIsGuidedMarked` remain the
//     model's alone.
// AND `Shake` HOLDS NO SECOND OPINION ABOUT WHERE THE UNIT IS. It is a child of `Body`, which
// is a child of `UnitRoot`; the actor transform is still written in exactly one place, so
// `T-INT-05.ApplyViewSpawnsMovesAndDestroys` needs no edit, exactly as it needed none when
// `Body` stopped being the root.
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
// - [NARROWED 2026-09-02 BY THE MOVE TWEEN, BY THE LANE THAT OWNS THIS FILE. THE CORRECTION
//   IS WRITTEN FLAT AND THE REASONING IS NOT OVERTURNED: it forbids exactly what it was
//   written to forbid, and it turned out to forbid one thing more than it needed to.] It
//   said:
//   RETRACTED> "Movement interpolation. `ApplyUnitView` teleports. Animating a move needs the
//   RETRACTED>  ordered event list §4.9 part 2 names and `StratBridge.h` records is not built
//   RETRACTED>  -- the view model says what IS, not what HAPPENED, and driving a slide off a
//   RETRACTED>  position difference would be inferring an event from a state, which is
//   RETRACTED>  exactly the conflation that header warns about."
//   WHAT STANDS, UNCHANGED AND UNWEAKENED: nothing in this class may LABEL a position
//   difference. A slide that meant "this unit MOVED" -- as against attacked, retreated, was
//   built, or is simply drawn at a different hex because a different model was applied -- is
//   still inferring an event from a state, and the ordered event list that would license it
//   is still not built. Nothing below asks what happened.
//   WHAT IS NOW BUILT, AND WHY IT IS NOT THAT. `MoveTweenSeconds` eases a purely VISUAL
//   offset on `Body` to zero. It labels no move, no attack and no ordering, it names no
//   cause, and it is applied identically to a unit that moved, a unit whose hex changed
//   because a whole different model was applied, and a unit that was dragged across the
//   board by a debug command. What it interpolates is THIS ACTOR'S OWN PREVIOUS RENDERING to
//   its new one -- a fact about a transform this class wrote and can read back off its own
//   component, not a claim about what the rules did.
//   THE ACTOR TRANSFORM STILL SNAPS, WHICH IS THE STRUCTURAL PART RATHER THAN A COURTESY.
//   `ApplyUnitView`'s `SetActorLocation` line is unchanged, so `GetActorLocation()` read
//   synchronously after `UStratMatchSubsystem::ApplyView` is the same value it was before
//   this landed. `T-INT-05.ApplyViewSpawnsMovesAndDestroys` reads exactly that and needed no
//   edit; had the tween moved the actor, it would have had to.
//   THE COST IS STATE, AND IT IS STATED. `TweenOffsets`, `TweenArcLengths` and `TweenElapsed`
//   are state no model holds -- see the correction on the "no state" paragraph above, which
//   names them. Their terminal value is zero and a clock drives them there, not a later model;
//   nothing reads any of them to decide anything; and none is a presentation BIT, so none can
//   make one unit draw a marker another does not.
//   AT `MoveTweenSeconds <= 0` -- THE C++ DEFAULT -- THE PATH IS THE OLD ONE EXACTLY. No
//   offset is written, no tick is enabled, and `Body`'s relative location stays the zero it
//   has always been. That is the switch-not-a-setting posture `AiPlaybackStepSeconds` argues
//   at length; see `MoveTweenSeconds` below, which restates it locally.
//   [EXTENDED 2026-09-02: THE PICTURE NOW FOLLOWS THE ROUTE'S HEXES RATHER THAN THE CHORD, AND
//   THE ARGUMENT ABOVE IS UNCHANGED BY IT -- WHICH IS WHY THIS IS AN EXTENSION AND NOT A SECOND
//   RETRACTION.] The waypoints arrive as a parameter of `ApplyUnitView`. They are a route the
//   RULES MODULE produced -- `FStratBridge::MovePathToHex`, the same query the commit itself
//   takes -- routed here through `UStratMatchSubsystem::NotePendingMoveRoute`, and this class
//   still asks nothing about what happened: it eases its own picture along a polyline it was
//   given, exactly as it eased along a chord it derived.
//   [CORRECTED 2026-09-02, SAME DAY, BY THE LANE THAT WROTE IT, BECAUSE A USER DECISION WAS
//   REVERSED. WRITTEN FLAT: THE SENTENCE BELOW IS NOW FALSE IN ITS SECOND HALF AND A READER WHO
//   GREPS IT MUST LAND ON THE CURRENT RULE.] It said:
//   RETRACTED> "WHAT DID CHANGE IS WHICH INPUTS EXIST: an EMPTY route means SNAP, so an AI
//   RETRACTED>  move, a load, a reseed and a spawn all snap and only a player move whose route
//   RETRACTED>  was captured slides."
//   WHAT STANDS, VERBATIM: an EMPTY route means SNAP, and a load, a reseed, a spawn and a
//   refused path query all reach that arm and all snap. WHAT IS RETRACTED: "an AI move ... snaps"
//   and "ONLY a player move ... slides". **THE USER REVERSED THAT DECISION ON 2026-09-02: AI
//   MOVES NOW ANIMATE ALONG THEIR ROUTE, ONE UNIT AT A TIME, DURING §2.11.2's PLAYBACK TOUR.**
//   THE MECHANISM IS A SECOND ENTRY POINT AND NOT A CHANGE TO THIS ONE. `ApplyUnitView` is
//   untouched in its arming condition; `PlayRouteSlide` is a separate verb that arms a slide
//   which PARKS at the route's own last hex, and `UStratMatchSubsystem::PlayMoveSlideForStep`
//   is its only caller. THAT IS STILL NOT AN EVENT INFERENCE BY THIS CLASS -- nothing here
//   detects an AI turn; a caller that knows it is touring one calls the other verb.
// - [HALF-DISCHARGED 2026-08-29 BY W4. THE CORRECTION IS WRITTEN FLAT, AND THE HALF THAT
//   STILL STANDS IS NAMED SEPARATELY, because a bullet that is true of one bit and false of
//   the other reads as wholly true to whoever greps for either.] It said:
//   RETRACTED> "The DONE / locked visual. The bits exist on `FStratUnitView` and nothing
//   RETRACTED>  produces them until phase 4's selection machine does. This class reads them
//   RETRACTED>  and applies no treatment; phase 5 decides what 'done' looks like. NOTE THE
//   RETRACTED>  CONTRAST WITH THE §2.11.6-B MARKER ADDED 2026-08-23, which this class DOES
//   RETRACTED>  apply a treatment for: `bIsGuidedMarked` names one unit for the whole match
//   RETRACTED>  and its directive is unreadable without it, where `bDone` and
//   RETRACTED>  `bLockedThisTurn` change several times a turn and §2.11 has not said what
//   RETRACTED>  they look like."
//   `bDone` NOW HAS A TREATMENT -- §2.11.2's unacted pip, `UnactedPip` below -- so the
//   sentence is false of it, and so is the reason it gave. §2.11 DID say what done looks
//   like: the earn-your-pixels row reads `Unacted pip on own units | Which units I can
//   still give an order to | §2.1 per-unit loop, via the DONE bit of §2.11.1's machine,
//   carried in the view-model's presentation block`, which names the bit AND the seam. The
//   retracted clause was written when the selection machine did not exist, and was never
//   re-read against §2.11.2 once it did.
//   `bLockedThisTurn` STILL HAS NO TREATMENT AND THAT HALF STANDS UNCHANGED. §2.11.2's
//   table has no row for a lock, and drawing one by analogy with the pip's row would be
//   this file inventing a content rule the GDD did not state -- the thing the whole "the
//   engine asks, it never decides" posture exists to stop. It stays a cached field with no
//   reader in this class.
// - The §2.11.2 MARKERS' ART. **[CORRECTED 2026-09-06. THIS BULLET OPENED "`FlagMarker`
//   and `UnactedPip` ship with no mesh and no material, exactly as `GuidedMarkerMesh`
//   shipped before 2026-08-24 ... Until the content lane assigns them the components exist,
//   are positioned, and are toggled correctly on every refresh, and they draw nothing. That
//   is a content gap and not a match failure" -- AND ALL FOUR OF THOSE PROPERTIES ARE NOW
//   ASSIGNED, SO THERE IS NO GAP AND THE `GuidedMarkerMesh` ANALOGY IS INVERTED RATHER THAN
//   APT.** `BP_StratUnit`'s class default carries `FlagMarkerMesh = /Engine/BasicShapes/Cone`,
//   `FlagMarkerMaterial = MI_Marker_Flag`, `UnactedPipMesh = /Engine/BasicShapes/Sphere` and
//   `UnactedPipMaterial = MI_Marker_Pip` -- read out of a live editor BY VALUE and recorded
//   in `Tools/architect/state/content.md`, and RE-DERIVED HERE FROM THE SHIPPED BYTES rather
//   than taken on that record's word. A byte census of `Content/StratPlay/BP_StratUnit.uasset`
//   returns each of those four property names once, and `MI_Marker_Flag` and `MI_Marker_Pip`
//   twice each. THE INSTRUMENT DISCRIMINATES IN BOTH DIRECTIONS ON THIS ONE FILE, which is
//   what makes those counts evidence: the POSITIVE control is `GuidedMarkerMesh`, assigned
//   since 2026-08-24, returning the same 1; the KNOWN-UNSET control `FlagMarkerOffset`
//   returns 0, and a fabricated property name returns 0. And the package is a real one --
//   its first bytes are the Unreal magic `c1 83 2a 9e`, not a git-lfs pointer line. The
//   `FlagMarkerMesh` and `UnactedPipMesh` blocks below carry the same reading.]**
//   THE HALF OF THE RETRACTED SENTENCE THAT WAS ABOUT THIS FILE STANDS UNCHANGED, and is
//   why this bullet is corrected rather than deleted: `EditDefaultsOnly`, no initializer,
//   and no `/Game/` path here -- a project rule that the markers now having art does not
//   touch. `BeginPlay` still says so once per actor for each, because an unconfigured marker
//   and a unit that simply is not marked are indistinguishable on screen and have entirely
//   different fixes; that log is now a guard for ANOTHER Blueprint of this class rather than
//   a description of the shipped one.
// - [CORRECTED 2026-09-06 BY THE DAMAGE FLASH. WRITTEN FLAT, IN THIS FILE'S `RETRACTED>`
//   CONVENTION, BECAUSE ONE OF THE THREE THINGS THIS BULLET RULED OUT IS NOW BUILT AND A
//   READER WHO GREPS "hit flashes" MUST LAND ON THE CORRECTION.] It said:
//   RETRACTED> "Health bars, damage numbers, hit flashes. All presentation over an event
//   RETRACTED>  stream that does not exist yet, and none of it is named by an acceptance ID in
//   RETRACTED>  this milestone."
//   A HIT FLASH IS BUILT -- `PlayDamageFlash` below -- AND THE HONEST DESCRIPTION IS THAT THE
//   EVENT STREAM STILL DOES NOT EXIST. What was built is a STATE DIFF: `StratDecideDamagedUnits`
//   compares one applied model's HP against the previous one's and names the units whose HP
//   FELL. Nothing here is told that an attack happened, by whom, or in what order. The flash
//   LABELS NO CAUSE AND NAMES NO ATTACKER, and it is applied identically to a unit damaged in
//   §2.6 combat and to one whose HP is lower for any other reason at all -- the same posture
//   `MoveTweenSeconds` takes about a position difference, and it is what keeps this class out
//   of the event-inference that §4.9 part 2's list would be needed to license.
//   HEALTH BARS AND DAMAGE NUMBERS STAY OUT, AND THE REASON IS NOW SHARPER THAN "no event
//   stream": THE DAMAGE AMOUNT IS NOT AVAILABLE IN THE PLAY LAYER. A diff over two view models
//   can say THAT HP fell without deriving a magnitude; saying BY HOW MUCH is a subtraction, and
//   `StratSoundCues.cpp`'s standing "computes no magnitude about game state" claim is
//   exactly what forbids the layer that would have to do it. (This line quoted that claim as
//   "there is not one arithmetic operation in it" until 2026-09-06; that wording was false of
//   the file it named -- see its opening block, which carries the retraction and the one
//   compile-time exemption. What the claim FORBIDS is unchanged, so the sentence around it
//   still holds and no code here moved.) A number on screen wants
//   `FStratRepairApplication`'s shape from the bridge, which is not in this round.
// - Any `/Game/` path. Every mesh and material is an EditDefaultsOnly property.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "StratViewModel.h"

#include "StratUnitActor.generated.h"

class UMaterialInterface;
class USceneComponent;
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
	 * @param RouteWorldPoints
	 *                        the hexes this unit's picture is to travel along, in order, each
	 *                        already converted by `AStratBoardActor::WorldLocationOfHex` -- the
	 *                        one axial -> world expression in the project, for the reason
	 *                        `WorldLocation` is passed rather than computed. `[0]` is the unit's
	 *                        own starting hex and `.Last()` is the hex `View.Hex` names.
	 *
	 *                        **EMPTY MEANS SNAP, AND THAT IS THE WHOLE ARMING CONDITION.** No
	 *                        route, no tween: the actor is put at its destination and its
	 *                        picture goes with it. `UStratMatchSubsystem::PendingMoveRoutes` is
	 *                        filled only by an accepted PLAYER command and is emptied on every
	 *                        apply, so the map's own contents are the signal for THIS entry
	 *                        point and no second one exists. A load, a reseed, a spawn and a
	 *                        path query that refused all reach this arm too, and snapping is
	 *                        the right answer to each.
	 *
	 *                        [CORRECTED 2026-09-02, SAME DAY, BECAUSE A USER DECISION WAS
	 *                        REVERSED. FLAT, NOT NESTED.] It said:
	 *                        RETRACTED> "This is what makes 'AI moves do not slide' true
	 *                        RETRACTED>  WITHOUT anything in this class or its caller detecting
	 *                        RETRACTED>  an AI turn."
	 *                        AI MOVES NOW SLIDE, during §2.11.2's playback tour, by the user's
	 *                        reversal of that decision on 2026-09-02. What is retracted is the
	 *                        CONSEQUENCE and not the MECHANISM: this parameter is still filled
	 *                        only from `PendingMoveRoutes`, this function still cannot tell an
	 *                        AI turn from a player one, and the AI's slide arrives through
	 *                        `PlayRouteSlide` instead -- a different verb with a different
	 *                        caller. Nothing in this class detects anything.
	 */
	void ApplyUnitView(const FStratUnitView& View, const FVector& WorldLocation, int32 ViewingSide,
	                   bool bGuidanceActive, const TArray<FVector>& RouteWorldPoints);

	/**
	 * Puts this actor over `WorldLocation` with NO animation, whatever `MoveTweenSeconds` says.
	 *
	 * THE DECLARED NO-ANIMATION ENTRY POINT, FOR A CALLER THAT KNOWS THE ACTOR IS NEW. It
	 * exists because "a unit that just appeared must not slide in from wherever it was
	 * standing" is a fact only the spawner has: a freshly spawned actor is at
	 * `FTransform::Identity`, and without this every unit on a fresh board would ease in from
	 * the world origin. `UStratMatchSubsystem::ApplyView` calls it on the spawn branch and
	 * nowhere else.
	 *
	 * THE KNOWLEDGE STAYS AT THE ONE PLACE NEWNESS IS KNOWN AND IS NOT A BIT THIS ACTOR
	 * REMEMBERS. The alternative was a `bHasAppliedAView` flag here, suppressing the first
	 * tween -- and that is precisely a presentation bit on an actor that no model holds, which
	 * is what `T-INT-05.NoActorHoldsPresentationBits` is about. The spawner already knows; it
	 * says so by calling this.
	 *
	 * IT WRITES THE SAME EXPRESSION `ApplyUnitView` WRITES, DELIBERATELY DUPLICATED. Factoring
	 * the `+ BodyZOffset` into a shared helper would have edited `ApplyUnitView`'s location
	 * line, and that line staying byte-identical is what makes "the actor transform is
	 * unchanged by this feature" checkable by reading rather than by argument. The duplication
	 * is one addition and is named here so it is maintained as a pair.
	 *
	 * IT ALSO CLEARS ANY TWEEN IN FLIGHT, so calling it mid-slide is a hard cut rather than a
	 * slide that continues from a stale start. That is what "no animation" has to mean to be
	 * usable as a repair.
	 *
	 * IT DELIBERATELY DOES **NOT** CANCEL A DAMAGE FLASH, ADDED 2026-09-06, AND THE OMISSION IS
	 * STATED HERE SO IT IS NOT READ AS ONE. This function documents itself as clearing a TWEEN,
	 * which is a claim about WHERE THE PICTURE IS, and a hard position cut is exactly the repair
	 * for a stale position. A damage window is not a position: it is a fixed-length statement
	 * that this unit's HP fell, and nothing about being teleported makes that statement stale.
	 * `CancelDamageFlash` is the verb for the other thing, and it is separate for that reason.
	 * The two clocks are independent all the way down -- see `UpdateTickEnabled`, which is the
	 * only thing that arbitrates between them.
	 *
	 * @param WorldLocation the same value `ApplyUnitView` would be handed for this unit --
	 *                      `AStratBoardActor::WorldLocationOfHex(View.Hex)`, never a value
	 *                      computed here.
	 */
	void SnapToWorldLocation(const FVector& WorldLocation);

	/**
	 * Arms a route slide that PARKS at the route's own last hex. Returns the seconds armed.
	 *
	 * WHAT GAP THIS CLOSES, AND IT IS A USER DECISION REVERSED. Until 2026-09-02 AI moves were
	 * required NOT to slide, and the mechanism was that nothing noted a route for them --
	 * `ApplyUnitView`'s empty-route arm, which is unchanged. The user reversed that: §2.11.2's
	 * playback tour now animates each AI move along the hexes the rules module actually walked,
	 * ONE UNIT AT A TIME, with the tour's clock waiting for each slide. `ApplyUnitView` cannot
	 * serve that, because by the time a tour runs the board ALREADY SHOWS THE FINAL STATE --
	 * every actor is at its last hex and no transform is going to change again. So a tour's
	 * slide is a slide the actor's TRANSFORM does not accompany, and that is the whole reason
	 * this is a second verb rather than a fifth parameter on the first one.
	 *
	 * IT PARKS, WHICH IS THE ONE PROPERTY `ApplyUnitView` CANNOT HAVE. `ApplyUnitView`'s tween
	 * always retires to `Body` at exact relative ZERO, because the actor has just been put where
	 * the model says the unit is. Here the actor is at the unit's FINAL hex and the slide being
	 * shown ends at some intermediate one, so the resting offset is non-zero and the picture
	 * stays there until the next step moves it or the tour ends. See `TweenRestOffset`.
	 *
	 * IT WRITES NOTHING BUT THE TWEEN, AND THE LIST IS EXHAUSTIVE SO THAT A READER NEED NOT
	 * DIFF IT AGAINST `ApplyUnitView`: the two tween arrays, `TweenElapsed`, `TweenRestOffset`,
	 * `Body`'s relative location and the tick flag. **No mesh, no material, no marker, no
	 * `UnitId`, no `LastAppliedView`, and -- the load-bearing one -- NO `SetActorLocation`.**
	 * A tour that moved actors would be a second writer of the transform `ApplyView` owns, and
	 * `T-INT-05`'s "the screen is a function of the model" would then be false of the board
	 * rather than merely of a picture's rest position.
	 *
	 * @param RouteWorldPoints the hexes this unit's picture is to walk, in order, each already
	 *                         through `AStratBoardActor::WorldLocationOfHex` -- the project's
	 *                         one axial -> world expression, for `ApplyUnitView`'s reason.
	 *                         Fewer than two points arms nothing and returns 0: a one-point
	 *                         route has no segment to walk.
	 * @param AnchorWorldPoint the TILE-PLANE point the offsets are measured against -- the
	 *                         world location of the hex this unit's picture rests over when the
	 *                         slide retires to nothing, which is the hex the LAST APPLIED MODEL
	 *                         puts it on.
	 *
	 *                         **IT IS SUPPLIED BY THE CALLER, FROM THE SAME SOURCE THE ROUTE
	 *                         CAME FROM, AND IT MAY NOT BE DERIVED HERE.** The obvious
	 *                         derivation is `GetActorLocation() - FVector(0, 0, BodyZOffset)`,
	 *                         and it is FORBIDDEN: `ApplyUnitView`'s own arming block records
	 *                         that differencing two points from ONE source is what cancels the
	 *                         lift exactly, and that a second copy of the `+ BodyZOffset`
	 *                         expression is the one place this arithmetic could go wrong. That
	 *                         cancellation is why `T-INT-05.TweenWaypointsSitAtTheDrawnUnitsHeight`
	 *                         is green, and it was measured able to fail: substituting a
	 *                         lift-bearing point for the route's own reddened it alone, with
	 *                         *"waypoint 1 draws at the unit's own height over hex (-1, 4):
	 *                         Z 0.00, expected 37.00"*. A derived anchor reintroduces exactly
	 *                         that expression.
	 *
	 * @return the seconds the armed slide will take, or 0 when nothing was armed. **THE ACTOR
	 *         TELLS THE CALLER RATHER THAN THE CALLER PREDICTING IT.** `MoveTweenSeconds` is
	 *         this class's, and `UStratMatchSubsystem::AiPlaybackStepSeconds`' own block states
	 *         "this subsystem never reads it" -- a sentence that has to stay true, because the
	 *         moment the tour computed a duration from a per-actor Blueprint property there
	 *         would be two answers to how long a slide lasts and a Blueprint edit would desync
	 *         the clock from the picture. Zero is returned on every refusal, so the caller's
	 *         interval collapses to its own pacing figure with no special case.
	 */
	float PlayRouteSlide(const TArray<FVector>& RouteWorldPoints, const FVector& AnchorWorldPoint);

	/**
	 * Retires any slide in flight AND clears any park, returning `Body` to exact relative zero.
	 *
	 * THE COUNTERPART TO `PlayRouteSlide` AND THE REASON A PARK IS SAFE TO CREATE. A parked
	 * picture has no clock behind it -- that is what parking means -- so nothing brings it home
	 * on its own. Without a verb that clears one, a tour skipped or abandoned mid-slide would
	 * strand a unit's picture over an intermediate hex FOREVER, with the actor and the model
	 * both saying it is somewhere else.
	 *
	 * IT RECOMPUTES NOTHING AND WRITES NO POSITION. `Body` goes to relative zero, which is the
	 * value it holds at rest on every path that does not park; the actor transform is not read,
	 * not written, and not consulted. So this cannot disagree with `ApplyView` about where the
	 * unit is -- it has no opinion about where the unit is.
	 *
	 * A NO-OP WHEN THERE IS NOTHING TO CLEAR, deliberately, so that its caller may be
	 * unconditional. See `UStratMatchSubsystem::EndAiPlaybackTour`, which calls it for every
	 * unit actor on all six of its own call sites for exactly that reason.
	 */
	void CancelRouteSlide();

	/**
	 * Parks this picture over a hex IMMEDIATELY, with no slide and no clock. A park with
	 * nothing running.
	 *
	 * WHAT GAP THIS CLOSES, AND IT IS A DEFECT A HUMAN SAW BEFORE ANY INSTRUMENT DID.
	 * §2.11.2's tour steps over a board that is ALREADY FINAL -- `RunAiTurnsNow` reconciles
	 * before `BeginAiPlayback`, deliberately, and that ordering is load-bearing. The cost, which
	 * nothing in this tree predicted and a PIE playtest reported on 2026-09-02, is that every AI
	 * unit is DRAWN at its destination the instant the hand-over resolves; `PlayRouteSlide` then
	 * displaces that picture back to the start of its leg and eases forward. **The player sees a
	 * mass snap to the destinations and then a rubber-band per unit**, in the user's own words:
	 * *"the AI units all snap to their target destination and then each one teleports back to
	 * their original position and then interps to the target location."*
	 *
	 * SO THE PICTURES ARE PUT BACK BEFORE THE TOUR'S FIRST STEP AND NOT DURING IT. The model
	 * stays final -- nothing here touches the actor transform or the view model, and the fix is
	 * emphatically NOT to reorder the refresh and the tour. What moves is the picture, before
	 * anything is shown, so no unit is ever drawn at a hex it has not yet been seen to walk to.
	 *
	 * IT IS `FinishTween` WITH A COMPUTED REST OFFSET AND NOT A SIXTH COPY OF ITS FOUR LINES.
	 * That function is the ONE retirement path in this class -- see its declaration -- and
	 * "empty both arrays, put `Body` at the rest offset, zero the clock, tick off" is exactly
	 * what a park with nothing running is. `CancelRouteSlide` is the same shape with a rest
	 * offset of zero.
	 *
	 * IT REFUSES AT `MoveTweenSeconds <= 0`, WHICH IS NOT AN OPTIMISATION. That is the shipped
	 * C++ default and the configuration every automation fixture runs at; nothing slides there,
	 * so a park would displace a picture that no step would ever move back and the tour would
	 * end with a unit drawn hexes from where the model says it is. A caller cannot get this
	 * wrong because this function will not do it.
	 *
	 * @param ParkWorldPoint   the TILE-PLANE point to draw this unit over --
	 *                         `AStratBoardActor::WorldLocationOfHex` of the hex its first
	 *                         recorded move began on.
	 * @param AnchorWorldPoint the TILE-PLANE point the offset is measured against, **from the
	 *                         same source as `ParkWorldPoint`**, exactly as `PlayRouteSlide`
	 *                         requires and for the identical reason: differencing two points
	 *                         from one source cancels `BodyZOffset` exactly, so this function
	 *                         holds no third copy of the `+ BodyZOffset` expression the
	 *                         declaration pairs across `ApplyUnitView` and `SnapToWorldLocation`.
	 *                         **Deriving it as `GetActorLocation() - FVector(0, 0, BodyZOffset)`
	 *                         is forbidden here on `PlayRouteSlide`'s recorded measurement.**
	 */
	void ParkPictureAt(const FVector& ParkWorldPoint, const FVector& AnchorWorldPoint);

	/**
	 * Flashes this unit's body and shakes its picture in X and Y for `DamageFlashSeconds`.
	 *
	 * WHAT GAP THIS CLOSES. Combat gave the player almost no signal on the board: the
	 * `UnitAttacked` cue and an HP number somewhere off the unit itself. The header block's
	 * corrected "hit flashes" bullet states what this is and -- more importantly -- what it is
	 * NOT: a STATE DIFF, not an event. This function is handed no attacker, no amount and no
	 * cause, and could not label one if it wanted to.
	 *
	 * IT IS CALLED ONLY BY `UStratMatchSubsystem` AND IS NEVER REACHED FROM `ApplyUnitView`,
	 * WHICH IS NON-NEGOTIABLE AND IS THE REASON IT IS A SEPARATE VERB. That function is
	 * IDEMPOTENT AND TOTAL by its own declaration and runs on every hover refresh --
	 * `AStratPlayerController::Tick` reaches it whenever the mouse crosses a hex boundary -- so
	 * anything armed inside it would fire on mouse movement. The same structural reason
	 * `PlayRouteSlide` is a second entry point rather than a fifth parameter on the first one.
	 *
	 * RESTARTING MID-FLASH RESTARTS THE CLOCK AND DOES NOT STACK AMPLITUDE. There is one clock
	 * and one amplitude expression; a second call writes the clock back to a fresh tick and the
	 * shake is continuous across the seam, because `TickDamageFlash`'s sin/cos pair has a
	 * continuous derivative at zero phase -- the identical property `MoveTweenEaseFraction`
	 * argues for the trapezoid, from a different curve.
	 *
	 * A NO-OP AT `DamageFlashSeconds <= 0`, WHICH IS THE SHIPPED C++ DEFAULT. Nothing is armed,
	 * no tick is enabled, no material is touched and `Shake` is not written -- so the tree is
	 * behaviourally identical to one in which this function does not exist, which is what keeps
	 * every existing fixture on the path it was written against. The switch-not-a-setting
	 * posture `MoveTweenSeconds` argues at length; see `DamageFlashSeconds`, which restates it.
	 */
	void PlayDamageFlash();

	/**
	 * Retires any flash: overlay cleared, `Shake` at exact relative zero, clock zeroed, tick off
	 * unless the move tween still wants it.
	 *
	 * IT IS `FinishDamageFlash` MADE PUBLIC, AND THAT IS DELIBERATE RATHER THAN A CONVENIENCE.
	 * `AStratUnitActor::Tick` NEVER RUNS HEADLESS and a test-only advance seam has already been
	 * refused in this project, so a clause has no way to let a flash expire -- which would make
	 * the exactly-zero retirement claim above unassertable. Exposing the ONE retirement path is
	 * the same permission `CancelRouteSlide` already takes for the tween's, for the same reason
	 * and with the same consequence: there is one answer to "how does it get back to zero", so a
	 * clause and the running game exercise the same four lines.
	 *
	 * A NO-OP WHEN NOTHING IS RUNNING, deliberately, so a caller may be unconditional.
	 */
	void CancelDamageFlash();

	/** Whether a damage flash is in flight -- `DamageFlashElapsed > 0`. FOR THE CLAUSE, on
	 *  `GetTweenWaypointCount`'s stated permission, and it reports a CLOCK and not pixels: it
	 *  cannot say whether `DamageFlashMaterial` was ever assigned and cannot say that anything
	 *  red reached the screen. That limit is `IsGuidedMarkerVisible`'s measured one, inherited
	 *  rather than re-learned. */
	bool IsDamageFlashActive() const { return DamageFlashElapsed > 0.0f; }

	/** Seconds since the current flash was armed, or 0 when none is. FOR THE CLAUSE, on the
	 *  same permission, and it reports the real member rather than a cache of it -- so a clause
	 *  that arms a flash and reads this is reading the thing `TickDamageFlash` advances. */
	float GetDamageFlashElapsed() const { return DamageFlashElapsed; }

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
	 * How many waypoints the tween currently in flight has. Zero means none is in flight.
	 *
	 * IT EXISTS FOR THE CLAUSE, AND THAT IS SAID HERE RATHER THAN LEFT TO BE INFERRED -- the
	 * same permission `IsGuidedMarkerVisible` takes and for the same reason. "The picture
	 * follows the route's hexes rather than the chord between its ends" is a claim about a
	 * POLYLINE, and a polyline is unassertable through a position sampled at one instant: a
	 * chord and a route agree at both ends and a test that only reads `Body`'s location at rest
	 * cannot tell them apart. There is no other reader in `Source/`.
	 *
	 * IT REPORTS THE REAL ARRAY AND NOT A CACHE OF IT. `TweenOffsets.Num()`, live -- so a
	 * clause that arms a tween and then reads this is reading the thing `Tick` walks, and there
	 * is no second number that could drift from it.
	 *
	 * IT IS A COUNT OF WAYPOINTS AND NOT OF HEXES, and the two differ by construction: the
	 * terminal `FVector::ZeroVector` is APPENDED rather than converted from the route's last
	 * hex, so an N-hex route arms N waypoints of which the last is exactly zero. See
	 * `TweenOffsets`, which states why that is a construction and not an arithmetic identity.
	 */
	int32 GetTweenWaypointCount() const { return TweenOffsets.Num(); }

	/**
	 * One waypoint of the tween in flight, `Body`-relative, in the order the picture visits
	 * them. Out of range answers `FVector::ZeroVector`.
	 *
	 * FOR THE CLAUSE, on `GetTweenWaypointCount`'s stated permission. Together the two let a
	 * gate assert that consecutive waypoints are one hex apart -- which is what "along the
	 * route" MEANS and is not checkable from an endpoint.
	 *
	 * OUT OF RANGE IS ZERO AND NOT A CRASH, deliberately: zero is what the terminal waypoint
	 * holds anyway, so an off-by-one in a clause reads as a clause that measured the end of the
	 * route rather than as a test that took the editor down mid-suite.
	 */
	FVector GetTweenWaypoint(int32 Index) const
	{
		return TweenOffsets.IsValidIndex(Index) ? TweenOffsets[Index] : FVector::ZeroVector;
	}

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

	/**
	 * Whether §2.11.2's flag `H` marker is showing on this unit.
	 *
	 * OFF THE COMPONENT AND NOT OFF `LastAppliedView.bIsFlag`, for `IsGuidedMarkerVisible`'s
	 * reason exactly and not a weaker one: answering from the cached view would report what
	 * this actor was TOLD and would pass whether or not anything reached the component --
	 * the lazily-armed-subject shape that makes a clause unable to fail.
	 *
	 * AND IT INHERITS THAT FUNCTION'S CORRECTED LIMIT RATHER THAN RE-LEARNING IT AT COST.
	 * `USceneComponent::IsVisible` consults `bHiddenInGame`, the visible flag and the cached
	 * level collection and NOT the static mesh, which is the measured correction that
	 * function's block carries. So this answers TRUE for the flag unit of a Blueprint with no
	 * `FlagMarkerMesh` assigned, which draws nothing. **IT REPORTS A FLAG, NOT PIXELS.**
	 * There is no headless gate on "a player can see the H", none is available from this
	 * class, and anything claiming one off this function is over-reading it.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Unit")
	bool IsFlagMarkerVisible() const;

	/**
	 * Whether §2.11.2's unacted pip is showing on this unit.
	 *
	 * SAME SEAM AND SAME LIMIT AS `IsFlagMarkerVisible` -- off the component, never off
	 * `LastAppliedView.bDone`, and a flag rather than pixels. The two are stated separately
	 * rather than by cross-reference alone because they answer about different components and
	 * a reader arriving at one must not have to find the other to learn what it cannot do.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Unit")
	bool IsUnactedPipVisible() const;

protected:
	/**
	 * The actor's root, and the thing `SetActorLocation` moves.
	 *
	 * WHY IT EXISTS AT ALL, WHICH IS THE ONE NON-OBVIOUS PART OF THE MOVE TWEEN. `Body` was
	 * the root until 2026-09-02, and A ROOT'S RELATIVE LOCATION *IS* THE ACTOR'S WORLD
	 * LOCATION -- there is nowhere to put a visual offset that the actor transform does not
	 * also carry. Inserting an empty scene component above `Body` buys exactly one thing and
	 * it is the whole design: the actor transform snaps to the destination as it always did,
	 * and `Body`'s RELATIVE location carries the offset that decays to zero. `GetActorLocation()`
	 * read synchronously after `UStratMatchSubsystem::ApplyView` therefore returns what it
	 * returned before this landed, which is what `T-INT-05.ApplyViewSpawnsMovesAndDestroys`
	 * reads.
	 *
	 * THE SHAPE IS THE PROJECT'S EXISTING ONE AND NOT AN INVENTION HERE. `AStratBoardActor`
	 * has `BoardRoot` and `AStratCameraPawn` has `PivotRoot`, both for the same structural
	 * reason: a root that is not the visual is a root that can stay still while the visual
	 * does not.
	 *
	 * IT IS EMPTY ON PURPOSE. No mesh, no collision, nothing drawn. Everything visible hangs
	 * off `Body` below, which is what keeps the three markers following the VISUAL rather than
	 * the destination.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Stratocracy|Unit")
	TObjectPtr<USceneComponent> UnitRoot;

	/**
	 * The unit's body.
	 *
	 * NO COLLISION, set in the constructor and stated in the header block: the cursor must
	 * reach the tile underneath, because every rules question is asked about a hex.
	 *
	 * NO LONGER THE ROOT AS OF 2026-09-02, and this is the sentence a reader looking for the
	 * move tween should land on. It attaches to `UnitRoot` above, whose block gives the reason,
	 * and its RELATIVE location is the visual offset the tween eases to zero. Its relative
	 * location is zero at rest and zero on every path when `MoveTweenSeconds <= 0`.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Stratocracy|Unit")
	TObjectPtr<UStaticMeshComponent> Body;

	/**
	 * The damage shake's own transform. Empty, drawn by nothing, X and Y only, Z NEVER WRITTEN.
	 *
	 * WHY IT IS A SECOND COMPONENT AND NOT A SECOND TERM SUMMED INTO `Body`. Summing two offsets
	 * into `Body`'s relative location was considered and REJECTED, and the reason is the one
	 * property this file has spent the most prose defending: `FinishTween` writes
	 * `TweenRestOffset` VERBATIM through `SetRelativeLocation_Direct`, and "exactly zero at
	 * rest" is what two clauses assert against the measured ~1e-14 round-trip residue. With a
	 * sum, that write could no longer be verbatim -- it would have to preserve whatever the
	 * shake's term happened to be, so the exactly-zero property would depend on BOTH clocks
	 * having retired, and a flash retiring first or last would give different bits. With a
	 * separate component, `ApplyUnitView`, `PlayRouteSlide`, `ParkPictureAt`, `CancelRouteSlide`,
	 * `TickMoveTween`'s lerp and `FinishTween`'s write are UNTOUCHED, and every existing tween
	 * clause keeps reporting the same numbers for the same reason it always did.
	 *
	 * THE THREE MARKERS RE-PARENT ONTO IT, WHICH IS THE POINT OF PUTTING IT HERE RATHER THAN
	 * UNDER `Body`'S MESH. `GuidedMarker`, `FlagMarker` and `UnactedPip` attached to `Body`
	 * rather than to `UnitRoot` so they would ride the move tween's visual offset instead of
	 * sitting at the destination; the identical argument one level down says they must ride the
	 * shake too, or a shaken unit's three markers hang motionless beside it. They still ride the
	 * move tween, because `Shake` is a child of `Body` and inherits it.
	 *
	 * IT IS EMPTY ON PURPOSE, on `UnitRoot`'s own reasoning: a transform that is not a visual is
	 * a transform that can move while the thing above it does not. No mesh, no collision,
	 * nothing drawn, and nothing to reintroduce a blocker in front of the cursor.
	 *
	 * ITS RELATIVE LOCATION IS EXACTLY ZERO AT REST AND ON EVERY PATH AT THE SHIPPED
	 * `DamageFlashSeconds <= 0`. Nothing writes it but `TickDamageFlash` and
	 * `FinishDamageFlash`, and the second writes zero verbatim.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Stratocracy|Unit")
	TObjectPtr<USceneComponent> Shake;

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
	 * §2.11.2's flag `H` marker: "what to protect, what to hunt".
	 *
	 * WHAT GAP THIS CLOSES. `FStratUnitView::bIsFlag` has been published since phase 2 and,
	 * outside the rules module, was written by `StratBuildViewModel` and mirrored into the
	 * info panel and read by nothing that draws. §2.11.2 lists the on-map `H` among the four
	 * surfaces that carry "the four standing decisions of every turn", and the one it answers
	 * -- *what must I protect* -- was answerable only by opening the info panel on each unit
	 * in turn. This actor was handed the bit on every refresh and threw it away.
	 *
	 * ITS VISIBILITY IS A PASS-THROUGH OF ONE PUBLISHED FIELD AND IT IS NOT ANDed WITH
	 * ANYTHING. `ApplyUnitView` sets it from `View.bIsFlag` and from nothing else. **THE
	 * ABSENCE OF A SIDE TEST IS THE SPECIFICATION AND NOT AN OVERSIGHT**, and it is stated
	 * here because `GuidedMarker` twenty lines up HAS one and a reader will otherwise carry
	 * it across. §2.11.2's earn-your-pixels row is `Flag \`H\` marker (both sides, always
	 * visible) | What to protect, what to hunt | §2.4 flag death ends the match; *Conflict*'s
	 * \`H\` convention`. "Both sides" rules out `View.Side == ViewingSide`; "always visible"
	 * rules out any window bit. The guided marker's side filter was a USER RULING about a
	 * marker that says "select this", and a marker that says "this is the flag" is a
	 * different claim about a unit the player is meant to hunt.
	 *
	 * ITS OPERAND SET IS MATCH-CONSTANT, WHICH IS THE EXACT SHAPE OF THE 2026-08-24 DEFECT,
	 * AND HERE IT IS CORRECT. `GuidedMarker`'s block records the durable finding -- "**THE
	 * BOTH-DIRECTIONS DISCIPLINE HAD BEEN APPLIED TO THE WRITE AND NOT TO THE OPERAND SET**"
	 * -- and `bIsFlag` never moves either. The difference is that a permanent marker is what
	 * §2.11.2 ASKS FOR here ("always visible"), where §2.11.6 said in terms that the turn-1a
	 * marker clears. SO THE QUESTION THAT FINDING REALLY POSES -- *by what route does this
	 * ever stop drawing?* -- IS ANSWERED RATHER THAN DUCKED: **the flag unit's death destroys
	 * the actor.** `FStratViewModel::Units` is every LIVING unit, and
	 * `UStratMatchSubsystem::ApplyView` destroys the actor for any id the model no longer
	 * carries. The hide path is actor destruction, not a visibility write, and it is
	 * therefore not this component's to arrange. §2.4 ends the match in the same breath.
	 *
	 * IT ADDS NO STATE THE MODEL DOES NOT HOLD, per the header block's sharpest constraint.
	 * Component visibility is a rendering of a field of the applied model, overwritten
	 * totally on every `ApplyUnitView`, remembered nowhere.
	 *
	 * NO COLLISION, for `Body`'s reason: the cursor must reach the tile underneath, and the
	 * flag's hex is one a player has particular reason to click.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Stratocracy|Unit")
	TObjectPtr<UStaticMeshComponent> FlagMarker;

	/** The flag marker's mesh -- §2.11.2's `H`. **[CORRECTED 2026-09-06. THIS OPENED "UNSET
	 *  IS THE STATE THIS SHIPS IN and the content lane fills it, exactly as `GuidedMarkerMesh`
	 *  was unset until 2026-08-24" -- AND THE CONTENT LANE HAS SINCE FILLED IT.**
	 *  `BP_StratUnit`'s default reads `/Engine/BasicShapes/Cone`, read out of a live editor BY
	 *  VALUE and recorded in `Tools/architect/state/content.md`; the `FlagMarkerOffset` block
	 *  below carries that reading with both of its controls named. The half of the old sentence
	 *  that was about THIS FILE is unchanged and still true -- the C++ declares no initializer
	 *  -- so the unset-behaviour sentences below stand as the description of an unconfigured
	 *  Blueprint rather than of the shipped one.]** This
	 *  file must not name a `/Game/` path, which is why it is `EditDefaultsOnly` with no
	 *  initializer. Unset means the marker never draws, and `IsFlagMarkerVisible` STILL
	 *  ANSWERS TRUE for the flag unit -- see that function. `BeginPlay` logs the gap once per
	 *  actor because the LOG is the only place "unconfigured" is distinguishable from "not
	 *  the flag". */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Unit")
	TObjectPtr<UStaticMesh> FlagMarkerMesh;

	/** Optional material override for the flag marker. Unset leaves the mesh's own. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Unit")
	TObjectPtr<UMaterialInterface> FlagMarkerMaterial;

	/**
	 * §2.11.2's unacted pip: "which units I can still give an order to".
	 *
	 * WHAT GAP THIS CLOSES. `FStratSelectionMachine::DecorateViewModel` has written
	 * `U.bDone` onto the model since phase 4 and the only readers were the controller, the
	 * idle count and the info panel's text. Nothing on the BOARD said which units were still
	 * live, so the answer required tabbing through them.
	 *
	 * ITS VISIBILITY IS A PASS-THROUGH OF TWO PUBLISHED FIELDS. `ApplyUnitView` sets it from
	 * `!View.bDone && View.Side == ViewingSide` and from nothing else -- both off ONE model,
	 * nothing computed, nothing looked up, no hex compared.
	 *
	 * `bDone` AND NOT `bHasMoved && bHasActed`, AND THE GDD PICKED IT, NOT THIS FILE.
	 * §2.11.1: "Every surface in §2.11 that says a unit *has not acted* binds to the
	 * machine's bit: ... the idle count and the unacted pip (§2.11.2)". §2.11.2's own row
	 * binds it "via the DONE bit of §2.11.1's machine, carried in the view-model's
	 * presentation block". The turn flags are the alternative and are WRONG here for the
	 * reason `StratSelectionMachine.h` gives: a WAIT reaches DONE without spending either
	 * flag, so a flag-derived pip would stay lit on a unit the player has retired -- and the
	 * §2.11.1 quote says exactly that ("a waited unit would keep its pip").
	 *
	 * `bLockedThisTurn` IS DELIBERATELY NOT AN OPERAND, and it is named so the omission does
	 * not read as an oversight. A §2.11.6-B locked unit cannot be ordered either, so the
	 * conjunction is tempting -- but §2.11.2's row names the DONE bit and only the DONE bit,
	 * §2.11.1's enumeration of pip-bearing surfaces likewise, and adding a second operand
	 * would be this class deciding a content rule. If the GDD later says a locked unit shows
	 * no pip, that is one `&& !View.bLockedThisTurn` and a `GATE-MAPMARKERS` clause.
	 *
	 * THE SIDE TEST IS THE ROW'S OWN WORDS -- "Unacted pip on **own units**" -- and
	 * `ViewingSide` is `FStratViewModel::ViewingSide` off the same model `View` came from,
	 * never `UStratMatchSubsystem::GetViewingSide`, for the reason that field's block states:
	 * a viewing side held beside the model is a second input and T-INT-05 would then be about
	 * two things.
	 *
	 * ITS OPERAND SET HAS A REAL FALSE-GOER, which is the check `GuidedMarker`'s 2026-08-24
	 * correction demands of every new marker on this actor and which `FlagMarker` above
	 * answers differently. `bDone` moves several times a turn and resets at the turn
	 * boundary, so this conjunction goes false and true again inside one match under normal
	 * play; the write is unconditional in both directions AND the operands can take both
	 * values, which is the pair of properties that finding says must be checked together.
	 *
	 * IT ADDS NO STATE THE MODEL DOES NOT HOLD. `bDone` stays a field OF THE VIEW MODEL --
	 * `StratPlayerController.h` records that "a copy of `bDone` on an actor is precisely what
	 * `T-INT-05.NoActorHoldsPresentationBits` forbids", and nothing here copies it: the
	 * component's visibility is recomputed from the incoming view on every call and read back
	 * by nobody who decides anything.
	 *
	 * NO COLLISION, for `Body`'s reason.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Stratocracy|Unit")
	TObjectPtr<UStaticMeshComponent> UnactedPip;

	/** The pip's mesh. **[CORRECTED 2026-09-06. THIS READ "Unset is the shipping state and the
	 *  content lane fills it"; IT HAS BEEN FILLED --** `BP_StratUnit`'s default reads
	 *  `/Engine/BasicShapes/Sphere`, by the same by-value reading cited on `FlagMarkerMesh`.]**
	 *  See `FlagMarkerMesh`, which states the same terms and the same accessor caveat. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Unit")
	TObjectPtr<UStaticMesh> UnactedPipMesh;

	/** Optional material override for the pip. Unset leaves the mesh's own. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Unit")
	TObjectPtr<UMaterialInterface> UnactedPipMaterial;

	/**
	 * Where the flag marker sits relative to the body, and where the pip does.
	 *
	 * A FULL `FVector` AND NOT A SCALAR Z, WHICH DEPARTS FROM `GuidedMarkerZOffset` ABOVE
	 * DELIBERATELY. That property's own block had to be rewritten once when 150 turned out to
	 * occlude the unit, and it records the residual cost of a Z-only placement: a marker at
	 * positive Z "draws over the LOWER BODY OF THE UNIT ON THE HEX BEHIND". With THREE
	 * markers available on one actor, a Z-only axis makes them collinear up-screen and each
	 * new one stacks further into the row behind. A vector lets them be separated on the one
	 * axis that costs nothing, and lets the content lane re-place them without a code change.
	 *
	 * Y IS THAT AXIS, AND IT IS DERIVED FROM THE NUMBERS `GuidedMarkerZOffset` ALREADY
	 * ESTABLISHED RATHER THAN EYEBALLED. `AStratCameraPawn` looks down `ArmPitch` = -60 with
	 * no yaw on any path, so in body space screen-up is `0.866*x + 0.5*z` and depth is
	 * `0.5*x - 0.866*z`. **NEITHER EXPRESSION CONTAINS y.** A Y offset therefore moves a
	 * marker horizontally across the screen without changing its height or its depth sort --
	 * the only axis of the three for which that is true.
	 *
	 * THE MAGNITUDES. Z is 300 on both, which is `GuidedMarkerZOffset`'s derived clearance
	 * (a 100 uu centred mesh's base rim clears the body's silhouette at Z = 273.2; 300 clears
	 * by 13.4) and it carries over because the same camera and the same body are underneath.
	 * Y is -40 and +40, with the guided marker at 0, giving three markers 40 uu apart in a
	 * row. 40 is bounded by the tile: `AStratBoardActor::LocalLocationOfHex` documents
	 * `HexSize` as the CENTRE-TO-CENTRE distance between adjacent hexes and it defaults to
	 * 100, so a marker 40 uu off centre is still 10 uu inside the halfway point to the
	 * neighbouring unit.
	 *
	 * **THE Z IS DERIVED AND THE Y SEPARATION IS DERIVED; THE MARKERS' OWN SIZES ARE NOT
	 * KNOWN TO THIS FILE AND THE ARITHMETIC ASSUMES THEY MATCH `SM_GuidedMarker`'S 100 uu.**
	 * **RETRACTED 2026-09-06, SAME DAY AND TWICE OVER: THE SENTENCE THAT STOOD HERE AND THE
	 * PARENTHESIS ADDED HOURS EARLIER TO SAVE IT ARE BOTH WITHDRAWN.** The sentence read
	 * "No such mesh exists yet." A correction pass earlier the same day appended a
	 * parenthesis disambiguating its referent, ending "`FlagMarkerMesh` and `UnactedPipMesh`
	 * are the unset ones; measured 2026-09-06, the only mesh under `Content/StratArt/Meshes/`
	 * bearing on any marker is `SM_GuidedMarker` ... so the content lane may well be reusing
	 * the one mesh under three materials -- which this file cannot see and does not assume."
	 * **The referent call was right and the verdict was wrong.** Both properties ARE set, no
	 * mesh is owed, and the reuse speculation is withdrawn with it -- the content lane used
	 * engine primitives, not `SM_GuidedMarker` under three materials.
	 *
	 * WHAT IS TRUE NOW, WITH THE INSTRUMENT AND ITS TWO CONTROLS NAMED INLINE RATHER THAN
	 * SUMMARISED AS "measured". `Tools/architect/state/content.md` records both defaults read
	 * out of a LIVE EDITOR BY VALUE: `FlagMarkerMesh = /Engine/BasicShapes/Cone`,
	 * `UnactedPipMesh = /Engine/BasicShapes/Sphere`, alongside `FlagMarkerMaterial =
	 * MI_Marker_Flag` and `UnactedPipMaterial = MI_Marker_Pip`; engine primitives are no new
	 * precedent in this class, since `FallbackMesh` on the same Blueprint already reads
	 * `/Engine/BasicShapes/Cylinder`. Independently, `grep -aoF` over
	 * `Content/StratPlay/BP_StratUnit.uasset` returns 1 for each of those four names --
	 * against 0 for `FlagMarkerOffset`, which that record separately says is NOT overridden,
	 * and 0 for a fabricated property name. **The instrument discriminates in both directions
	 * on that very asset**, which is what makes the 1s evidence rather than noise.
	 *
	 * AND THE 100 uu ASSUMPTION STATED IN THE PARAGRAPH ABOVE IS CLOSED, NOT OPEN.
	 * `content.md` measured `SM_GuidedMarker`'s `ExtendedBounds` at `BoxExtent = (50, 50, 50)`
	 * and both engine primitives at the same 100 uu -- "The assumption holds." The sizes
	 * remain unknown to THIS FILE, which reads no asset, and that is why these stay defaults;
	 * they are not unknown to the project, so the human eye below is the gate on PLACEMENT and
	 * not on scale.
	 *
	 * THE LESSON IS THE SHAPE AND NOT THIS SLIP. **A pass whose whole job was removing
	 * unmeasured claims planted one, inside a clause labelled `measured`.** "Measured" is a
	 * claim about the AUTHOR'S PROCESS, and a reader cannot check a process -- so the word
	 * obliges naming the instrument inline, which that sentence did not, and an unmeasured
	 * claim wearing it is worse than the same claim bare. The project's standing rule for
	 * Blueprint defaults is ABSENCE DECIDES, PRESENCE PROVES NOTHING: a by-value read licenses
	 * a statement about what a default IS; "this file cannot see it" licenses nothing. The
	 * content lane's record is where by-value CDO reads live, and it answered both questions
	 * this block had open in a single `Read`. That is why these are defaults and not constants, and it is
	 * why the honest gate on the final placement is a human eye, exactly as it was for the
	 * guided marker -- NO TEST PINS EITHER VECTOR. The Y derivation also assumes the board
	 * actor is unrotated in yaw, which is true of every path that spawns one today and is not
	 * enforced anywhere.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Unit")
	FVector FlagMarkerOffset = FVector(0.0, -40.0, 300.0);

	/** See `FlagMarkerOffset`, which derives both vectors and states what neither is pinned
	 *  by. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Unit")
	FVector UnactedPipOffset = FVector(0.0, 40.0, 300.0);

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

	/**
	 * How long the picture takes to cross ONE HEX STEP of the route it was handed.
	 *
	 * [REDEFINED 2026-09-02 BY THE ROUTE TWEEN, WRITTEN FLAT IN THIS FILE'S `RETRACTED>`
	 * CONVENTION BECAUSE THE SENTENCE BELOW BECAME FALSE IN THE LETTER AND A READER TUNING PACE
	 * LANDS ON IT.] It said:
	 * RETRACTED> "How long `Body`'s visual offset takes to decay to zero after the actor is
	 * RETRACTED>  moved. ... `ApplyUnitView` snaps the ACTOR to the destination exactly as it
	 * RETRACTED>  always has and then gives `Body` a relative offset equal to where the unit was
	 * RETRACTED>  DRAWN minus where it now stands; `Tick` eases that offset to zero over this
	 * RETRACTED>  many seconds."
	 * There is no longer A single offset. `ApplyUnitView` builds a POLYLINE of offsets, one per
	 * route waypoint, and the whole tween lasts `MoveTweenSeconds * (waypoints - 1)` -- so this
	 * is now a PER-STEP duration and a four-hex move takes four times as long as a one-hex hop
	 * rather than the same time. **THAT IS A USER DECISION AND IT IS WHAT MAKES SPEED READ AS
	 * CONSTANT** across moves of different lengths; the alternative, a fixed total, makes a long
	 * move look frantic and a short one look sluggish for the same number.
	 *
	 * THE NAME IS UNDER-SPECIFIC BY ONE WORD AND IS KEPT ANYWAY, WHICH IS A COST STATED RATHER
	 * THAN PAID SILENTLY. `MoveTweenSecondsPerHex` is the better name. Renaming it is REFUSED
	 * because `Content/StratPlay/BP_StratUnit.uasset` stores its authored value keyed by this
	 * property's `FName`: a rename ORPHANS that value with no diagnostic at all -- the Blueprint
	 * falls back to the C++ default of 0.0, the animation switches off entirely, the build is
	 * green, no log fires and no clause reddens, because zero is a supported configuration.
	 * Mitigating it needs a `+PropertyRedirects` line in `Config/DefaultEngine.ini`, which is
	 * the DATA STEWARD's lane and not this one's. Whoever holds both lanes at once may do the
	 * pair; nobody may do half of it.
	 *
	 * WHAT IS UNCHANGED: the ACTOR still snaps. Every synchronous reader of
	 * `GetActorLocation()` sees the destination, immediately, on the same call.
	 *
	 * ZERO IS THE C++ DEFAULT AND IT IS A USER DECISION, ON `AiPlaybackStepSeconds`' OWN
	 * ARGUMENT RESTATED HERE RATHER THAN CITED. That field is §2.11.2's playback pace and its
	 * block refuses to write the GDD's 0.5 into C++ for two reasons that both apply verbatim
	 * to this number. First, a C++ default is a SECOND place the pace is stated, and the
	 * designer-facing one on the Blueprint is then the one nobody can find when the two
	 * disagree. Second -- and this is the load-bearing half -- a non-zero default would change
	 * the path every existing test runs down, which is the change the field exists to avoid
	 * making. So this ships as THE SWITCH AND NOT THE SETTING: the shipped duration goes on
	 * `BP_StratUnit`, which is the content lane's and not this file's.
	 *
	 * AT `<= 0` THE PATH IS BIT-IDENTICAL TO THE ONE THAT SHIPPED BEFORE THIS FIELD EXISTED.
	 * `ApplyUnitView` writes no offset, enables no tick, and leaves `Body`'s relative location
	 * at the zero the constructor gave it. `bStartWithTickEnabled` is false, so an actor whose
	 * Blueprint never sets this never ticks once in its life -- not a tick that early-returns,
	 * no tick at all.
	 *
	 * NO DISTANCE CLAMP, AND THE ABSENCE IS DELIBERATE. A "don't animate a jump longer than N"
	 * threshold needs a length scale, and the only honest one is `AStratBoardActor`'s
	 * `HexSize`, which this class cannot see and must not: it holds no board pointer, by the
	 * same discipline that makes `ApplyUnitView` take a world location instead of computing
	 * one. Any constant written here would be a magic number this file cannot derive, and the
	 * failure it would guard -- a unit sliding across the whole board -- cannot occur on any
	 * path today, because `UStratMatchSubsystem::StartMatchInternal` calls
	 * `TearDownPresentation()` unconditionally and that destroys every unit actor, so after a
	 * load or a reseed every actor is NEW and takes `SnapToWorldLocation`.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Unit")
	float MoveTweenSeconds = 0.0f;

	/**
	 * What fraction of the whole slide is spent easing in, and the same again easing out.
	 * Clamped to [0, 0.5] where it is read. 0 is a linear ramp; 0.5 is a curve with no
	 * constant middle at all.
	 *
	 * A TRAPEZOID AND NOT `SmoothStep`, WHICH IS THE USER'S OWN REQUEST AND NOT A REFINEMENT.
	 * The ask was constant speed between eased ends, and `SmoothStep` has no constant middle
	 * anywhere -- its velocity peaks at the midpoint and falls away on both sides, so a unit
	 * crossing four hexes visibly races the middle two. `Tick` integrates a trapezoidal VELOCITY
	 * profile instead: a linear ramp up over the first `MoveTweenEaseFraction` of the duration,
	 * a constant plateau, a mirrored ramp down.
	 *
	 * THE `SmoothStep` BLOCK'S OWN ARGUMENT SURVIVES AND IS RESTATED HERE RATHER THAN DELETED
	 * WITH IT, because it was never an argument for that particular curve -- it was an argument
	 * for ZERO VELOCITY AT BOTH ENDS. A tween here can be replaced mid-flight by the next
	 * `ApplyUnitView`, and the replacement begins at whatever velocity its own profile has at
	 * alpha 0. A linear ramp starts at full speed, so a restart is a visible jerk; a one-sided
	 * ease-out starts at full speed too. This profile's velocity is zero at 0 and at 1 and is
	 * continuous throughout, so a slide that is interrupted and a slide that replaces it both
	 * begin from rest and the seam is invisible -- the identical property, from a different
	 * curve.
	 *
	 * A SCALAR AND DELIBERATELY NOT A `UCurveFloat`. A curve is an asset, an asset is a `/Game/`
	 * path, and a `/Game/` path may not be named in this file -- so a curve would have to arrive
	 * as a second `EditDefaultsOnly` property that is unset by default, and an unset curve is an
	 * animation that silently does nothing. One number that cannot be null is the honest shape
	 * for a knob this small.
	 *
	 * ONE REGRESSION IS DECLARED RATHER THAN DISCOVERED. A ONE-HEX HOP USED TO RUN `SmoothStep`
	 * AND NOW RUNS THIS TRAPEZOID over the same duration -- same endpoints, same zero-velocity
	 * ends, a slightly different feel in between. Keeping `SmoothStep` for one-step routes was
	 * the alternative and is REFUSED: two profiles is two numbers to tune, two blocks of prose
	 * to keep true, and a discontinuity in behaviour at a route length nobody would think to
	 * test either side of.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Unit")
	float MoveTweenEaseFraction = 0.25f;

	/**
	 * How long a damage flash lasts. **ZERO IS OFF, AND IT IS A SWITCH RATHER THAN A SETTING.**
	 *
	 * THE POSTURE IS `MoveTweenSeconds`' AND `AiPlaybackStepSeconds`', RESTATED LOCALLY RATHER
	 * THAN CITED, because a reader tuning the flash lands here. Two reasons, both of which apply
	 * verbatim. First, a non-zero C++ default is a SECOND place the duration is stated, and the
	 * designer-facing one on `BP_StratUnit` is then the one nobody can find when the two
	 * disagree. Second -- the load-bearing half -- a non-zero default would change the path
	 * every existing automation fixture runs down, which is exactly the change this number
	 * exists to avoid making.
	 *
	 * AT `<= 0` NOTHING ARMS. `PlayDamageFlash` returns having written nothing, no tick is
	 * enabled, `Body`'s overlay material is never touched and `Shake` keeps the exact zero the
	 * constructor gave it -- so the tree is behaviourally identical to one without this feature.
	 * `<= 0` and not `== 0` on `ApplyUnitView`'s reasoning: a negative duration is a
	 * mis-authored Blueprint default and "no flash" is the safe reading.
	 *
	 * IT IS ALSO RE-READ EVERY TICK, WHICH IS THE HAZARD `Tick` ALREADY HANDLES FOR THE TWEEN. A
	 * Blueprint can write this to zero while a flash is in flight; `TickDamageFlash` retires
	 * rather than suspending, so the only wrong answer -- a unit left red and displaced with no
	 * clock to bring it home -- is unreachable.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Unit")
	float DamageFlashSeconds = 0.0f;

	/**
	 * How far the picture is displaced at the START of a flash, in unreal units, in X and Y.
	 * Decays linearly to zero over the flash.
	 *
	 * ZERO BY DEFAULT FOR `DamageFlashSeconds`' SECOND REASON AND NOT ITS FIRST: this is not the
	 * switch -- the duration is -- but a shipped non-zero amplitude would still be a magnitude
	 * stated in C++ that a Blueprint would then restate. It is tuned at the keyboard against the
	 * recorded `BodyZOffset` and the shipped meshes, neither of which this file can see.
	 *
	 * NO Z, AND THE ABSENCE IS THE SPECIFICATION. The user asked for a shake in X and Y.
	 * `FlagMarkerOffset`'s block derives that the camera's screen-up is `0.866*x + 0.5*z`, so a Z
	 * term would move a unit up and down the screen and read as a hop rather than as a rattle;
	 * it would also change the depth sort against the unit on the hex behind. `TickDamageFlash`
	 * writes a literal `0.0` into Z and there is no property that could make it otherwise.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Unit")
	float DamageShakeAmplitude = 0.0f;

	/**
	 * How fast the shake oscillates, in Hz.
	 *
	 * NON-ZERO BY DEFAULT AND THAT IS NOT AN INCONSISTENCY WITH THE TWO ABOVE. It is not a
	 * switch and it cannot arm anything: at `DamageShakeAmplitude = 0` this number multiplies a
	 * displacement of zero, and at `DamageFlashSeconds <= 0` it is never read at all. A zero
	 * default here would instead be a REAL configuration -- a shake frozen at one phase for its
	 * whole duration -- which is a worse thing to ship than a rate nothing uses.
	 *
	 * 18 IS FAST ENOUGH TO READ AS A RATTLE RATHER THAN A WOBBLE AT THE ~0.2 s DURATION THE USER
	 * ASKED FOR, WHICH IS AN EYE JUDGEMENT AND IS DECLARED AS ONE. Nothing headless pins it and
	 * nothing can: `Tick` never runs in a fixture, so frequency, amplitude, decay shape and
	 * smoothness are all unobservable to the suite. A human at the keyboard is the only
	 * instrument, exactly as it is for `GuidedMarkerZOffset`.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Unit")
	float DamageShakeFrequency = 18.0f;

	/**
	 * The material written into `Body`'s OVERLAY channel for the duration of a flash. Unset
	 * means the shake happens and nothing turns red.
	 *
	 * THE OVERLAY CHANNEL AND NOT A SLOT-0 SWAP, WHICH IS THE WHOLE REASON `ApplyUnitView`
	 * NEEDED NO EDIT. That function writes `Body->SetMaterial(0, SideMaterials[View.Side])` on
	 * every refresh, and a refresh happens whenever the mouse crosses a hex boundary -- so a
	 * slot-0 swap would be cancelled by a hover, mid-flash, at random. `SetOverlayMaterial` is a
	 * DIFFERENT channel (`UMeshComponent::SetOverlayMaterial` / `GetOverlayMaterial`, both
	 * `ENGINE_API` in UE 5.8), so the two writers cannot collide and `ApplyUnitView` keeps its
	 * idempotent-and-total block unamended.
	 *
	 * THERE IS NOTHING TO RESTORE *TO*, WHICH IS THE OTHER HALF OF THE SAME CHOICE. Retirement
	 * writes `nullptr`, a constant -- so no previous-material member exists on this actor, and
	 * the "terminal value is exactly zero, written not approached" argument extends to the
	 * material verbatim instead of needing a second one.
	 *
	 * A `UMaterialInstanceDynamic` WITH A COLOUR PARAMETER IS THE EVENTUAL RIGHT ANSWER AND IS
	 * DEFERRED, NOT REJECTED. It would need a material asset declaring that parameter -- content
	 * lane work -- plus an actor-held object member that is neither a clock nor a bit, which is
	 * a widening of the header block's exception that no gap today justifies. DISCHARGED WHEN a
	 * flash needs to vary in colour or intensity.
	 *
	 * UNSET IS THE STATE THIS SHIPS IN AND IT IS A CONTENT GAP, NOT A MATCH FAILURE -- the same
	 * posture the three markers shipped under. `EditDefaultsOnly`, no initializer, and no
	 * `/Game/` path in this file; the assignment is `BP_StratUnit`'s and the content lane's.
	 * `BeginPlay` says so once per actor, for `ConfigureMarker`'s recorded reason: an
	 * unconfigured flash material and a unit that was simply never hit are indistinguishable on
	 * screen and have entirely different fixes, and the LOG is the only place the project can
	 * tell them apart -- no accessor can, `IsDamageFlashActive` least of all.
	 *
	 * WHOEVER AUTHORS IT MUST CONFIRM IT COMPILES AS AN OVERLAY PASS. Overlay materials carry
	 * shading-model constraints this file cannot check and no headless instrument can report.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Unit")
	TObjectPtr<UMaterialInterface> DamageFlashMaterial;

	/** Applies ALL THREE markers' Blueprint-default meshes, materials and offsets -- the
	 *  §2.11.6-B guided marker, §2.11.2's flag `H` and §2.11.2's unacted pip. Overridden for
	 *  the reason `AStratBoardActor::BeginPlay` gives: a constructor reading a Blueprint
	 *  default runs on the CDO and sets it on the wrong object. THE THREE ARE APPLIED BY ONE
	 *  HELPER AND NOT BY THREE COPIES OF THE SAME NINE LINES -- see `ConfigureMarker`. */
	virtual void BeginPlay() override;

	/**
	 * Runs the two clocks: the move tween's, then the damage flash's.
	 *
	 * [WIDENED 2026-09-06 BY THE DAMAGE FLASH. WRITTEN FLAT BECAUSE THE HEADING SENTENCE CLAIMED
	 * A SINGULAR SUBJECT AND A READER GREPS THAT.] It said:
	 * RETRACTED> "Walks `Body`'s visual offset along `TweenOffsets` to zero and then switches
	 * RETRACTED>  itself off. THE ONLY TICKING THING IN THIS CLASS, AND IT IS OFF UNLESS A TWEEN
	 * RETRACTED>  IS IN FLIGHT."
	 * THERE ARE TWO TICKING THINGS AND THIS FUNCTION IS NOW TWO CALLS AND NOTHING ELSE --
	 * `TickMoveTween(DeltaSeconds)` then `TickDamageFlash(DeltaSeconds)`. The extraction is
	 * VERBATIM: the tween's body moved into its own function unedited, because it carries three
	 * early `return`s and a measured trapezoid, and any one of those returns left in place here
	 * would have SKIPPED THE FLASH ENTIRELY on the frames it fires.
	 *
	 * WHAT STANDS UNCHANGED: the steady state is still no tick at all. `bStartWithTickEnabled`
	 * is false, and the actor is never polling for a change the model would have told it about.
	 * The one arbiter of the tick flag is `UpdateTickEnabled`, whose block records the concrete
	 * failure a second one would cause.
	 *
	 * IT WRITES A RELATIVE LOCATION ON `Body` AND ON `Shake` AND NEVER TOUCHES THE ACTOR
	 * TRANSFORM. That is the invariant the whole feature rests on; see `UnitRoot`.
	 */
	virtual void Tick(float DeltaSeconds) override;

private:
	/**
	 * Places one marker component and applies its Blueprint-default mesh and material,
	 * logging once if the mesh is unset.
	 *
	 * ONE HELPER RATHER THAN THREE COPIES, AND THE REASON IS THE LOG AND NOT BREVITY. The
	 * missing-mesh log is the ONLY place "this marker is unconfigured" is distinguishable
	 * from "this unit is not marked" -- `IsGuidedMarkerVisible`'s block records that
	 * measurement and `IsFlagMarkerVisible` inherits it. Three hand-copied blocks are three
	 * chances for the fourth marker's author to place a component and omit its log, and the
	 * absence of a log is silent by construction.
	 *
	 * THE OFFSET IS A VECTOR FOR ALL THREE even though `GuidedMarkerZOffset` is a scalar; the
	 * call site widens it, so the guided marker's shipped placement is bit-identical to what
	 * it was before this helper existed and `FlagMarkerOffset`'s block does not have to
	 * describe two placement paths.
	 *
	 * @param Marker      the component. Null is tolerated and returns, because a Blueprint
	 *                    can fail to construct one and a match should not.
	 * @param Mesh        the `EditDefaultsOnly` mesh. Null logs and leaves the component
	 *                    meshless, which draws nothing.
	 * @param Material    optional override; null leaves the mesh's own.
	 * @param Offset      relative to `Shake`, which is a child of `Body` and sits at exact
	 *                    relative zero at rest -- so the SHIPPED PLACEMENT IS UNCHANGED BY THE
	 *                    2026-09-06 re-parent, to the bit, and `FlagMarkerOffset`'s derivation
	 *                    (which is stated in BODY space) is untouched. See that property on how
	 *                    the defaults were derived and on what is not pinned.
	 * @param MarkerName  names the marker in the log. A literal at each call site rather than
	 *                    `Marker->GetName()`, so the message says which GDD surface is
	 *                    unconfigured rather than which subobject.
	 */
	void ConfigureMarker(UStaticMeshComponent* Marker, UStaticMesh* Mesh, UMaterialInterface* Material,
	                     const FVector& Offset, const TCHAR* MarkerName);

	/**
	 * Retires the move tween: both arrays emptied, `Body` at exactly `TweenRestOffset`, elapsed
	 * zero, tick off.
	 *
	 * ONE RETIREMENT PATH AND NOT ONE PER EXIT, which is why it is a function rather than seven
	 * copies of four lines. `Tick` reaches it four ways -- completion, a null `Body`, a
	 * `MoveTweenSeconds` written to zero mid-slide, and a polyline of zero total length --
	 * `SnapToWorldLocation` is the fifth caller, `CancelRouteSlide` the sixth, and
	 * `ParkPictureAt` the seventh. A missed clear on any one of them leaves `Body` displaced with
	 * no clock to bring it back, which is invisible in a build and obvious on a screen.
	 *
	 * THE SEVENTH IS THE ONE THAT SHOWS WHY THIS IS A FUNCTION. `ParkPictureAt` sets a NON-ZERO
	 * rest offset and then calls this, and "empty both arrays, put `Body` at the rest offset,
	 * zero the clock, tick off" is the entire definition of a park with nothing running -- so a
	 * whole feature was added without a line of retirement logic being written a second time.
	 *
	 * [AMENDED 2026-09-02, SAME DAY, BY `PlayRouteSlide`. WRITTEN FLAT BECAUSE THE SENTENCE
	 * BELOW NAMED A LITERAL AND IS NOW A FIELD READ.] It said:
	 * RETRACTED> "IT WRITES EXACTLY ZERO rather than letting a curve converge to it."
	 * IT WRITES EXACTLY `TweenRestOffset` rather than letting a curve converge to anything, AND
	 * IT EMPTIES BOTH ARRAYS rather than leaving a spent polyline behind. **`TweenRestOffset` IS
	 * ZERO ON EVERY PATH THAT DOES NOT PARK**, so this is byte-for-byte the old behaviour for
	 * `ApplyUnitView`'s tween, for `SnapToWorldLocation`, and at the shipped
	 * `MoveTweenSeconds <= 0` default; the writers of a non-zero rest offset are
	 * `PlayRouteSlide` and `ParkPictureAt`, and there are no others. The "no state the model
	 * holds" claim in the header block rested on the
	 * terminal value BEING zero, and it is RE-SCOPED rather than weakened: see `TweenRestOffset`,
	 * which states the one invariant that is now true of both paths.
	 *
	 * "EXACTLY" IS LITERAL AND IT COST A LINE TO KEEP, WHICH IS RECORDED HERE BECAUSE THE
	 * OBVIOUS WRITE DOES NOT DELIVER IT. `USceneComponent::SetRelativeLocation` moves in WORLD
	 * space and converts back through the parent's inverse, so it stores a ROUND TRIP of the
	 * value it was handed -- measured 2026-09-02 with a probe in `CancelRouteSlide`: the same
	 * call asking for `FVector::ZeroVector` stored an exact zero for one unit and
	 * `Y=-0.000` for another, differing only in where their actors stood. The body uses
	 * `SetRelativeLocation_Direct` plus `UpdateComponentToWorld` instead, which computes the
	 * world transform FROM the relative one and so stores what it was asked for. The residue was
	 * ~1e-14 uu and invisible on a screen; it was NOT invisible to `IsZero()`, which is what
	 * this claim promises and what two clauses assert.
	 *
	 * IT EMPTIES BOTH ARRAYS for its own separate reason, unchanged: a route this actor is no
	 * longer walking is state by any honest reading, and `GetTweenWaypointCount` reports the
	 * array directly, so emptying it here is what makes "zero means no tween in flight" true for
	 * the clause rather than merely intended.
	 */
	void FinishTween();

	/**
	 * `Tick`'s move-tween half, MOVED VERBATIM out of it on 2026-09-06 and not tidied.
	 *
	 * IT EXISTS SO THE TWEEN'S THREE EARLY `return`s CANNOT SKIP THE FLASH. Every one of them is
	 * a legitimate retirement -- a null `Body`, a `MoveTweenSeconds` written to zero mid-slide, a
	 * degenerate polyline, a completed alpha -- and every one of them used to return out of
	 * `Tick` itself. A second feature added below them would have run on some frames and not
	 * others, silently, with a green build. The body is byte-for-byte what it was: same guards,
	 * same trapezoid, same arc-length scan, same lerp.
	 */
	void TickMoveTween(float DeltaSeconds);

	/**
	 * `Tick`'s damage-flash half: advance the clock, displace `Shake` in X and Y, retire at the
	 * end.
	 *
	 * THE AMPLITUDE DECAYS LINEARLY AND THE PHASE IS A SIN/COS PAIR, WHICH GIVES CIRCULAR X/Y
	 * JITTER WITH A CONTINUOUS DERIVATIVE. That last property is the same one
	 * `MoveTweenEaseFraction` argues for the trapezoid and it matters for the same reason: a
	 * flash can be restarted mid-flight by a second hit, and a profile that jumped at re-arming
	 * would read as a pop. It also means the displacement CONVERGES to the exact zero that
	 * retirement then writes, so the write is a confirmation rather than a correction.
	 *
	 * IT RETIRES RATHER THAN SUSPENDING, ON `TickMoveTween`'S OWN REASONING. A null `Shake` and a
	 * `DamageFlashSeconds` written to zero mid-flash both reach `FinishDamageFlash`, because the
	 * only wrong answer is a unit left red and displaced with no clock to bring it home.
	 *
	 * Z IS NEVER WRITTEN. See `DamageShakeAmplitude`, which states why that is the specification
	 * and not an economy.
	 */
	void TickDamageFlash(float DeltaSeconds);

	/**
	 * Retires the damage flash: clock zero, overlay `nullptr`, `Shake` at exactly relative zero,
	 * tick re-arbitrated.
	 *
	 * THE ONE RETIREMENT PATH, ON `FinishTween`'S PRECEDENT AND FOR ITS REASON. `TickDamageFlash`
	 * reaches it three ways -- completion, a null `Shake`, a duration written to zero mid-flash
	 * -- and `CancelDamageFlash` is the public fourth. A missed clear on any one of them leaves
	 * a unit red and displaced with nothing to bring it back, which is invisible in a build and
	 * obvious on a screen.
	 *
	 * "EXACTLY ZERO" IS LITERAL AND IT COSTS THE SAME LINE IT COSTS `FinishTween`, AND THE REASON
	 * IS RESTATED HERE RATHER THAN CROSS-REFERENCED BECAUSE A READER ARRIVING AT THIS FUNCTION
	 * MUST NOT HAVE TO FIND THAT ONE. `USceneComponent::SetRelativeLocation` moves in WORLD space
	 * and converts back through the parent's inverse, so it stores a ROUND TRIP of the value it
	 * was handed -- measured 2026-09-02 with a probe in `CancelRouteSlide`: the same call asking
	 * for `FVector::ZeroVector` stored an exact zero for one unit and `Y=-0.000` for another,
	 * differing only in where their actors stood. The residue was ~1e-14 uu, invisible on a
	 * screen and NOT invisible to `IsZero()`. `SetRelativeLocation_Direct` plus
	 * `UpdateComponentToWorld` computes the world transform FROM the relative one, so the stored
	 * value is the value asked for. Safe on `Shake` for a reason specific to it: it carries no
	 * mesh and no collision at all, so there is no sweep, no overlap and no physics state the
	 * move path would have been responsible for.
	 *
	 * IT ENDS BY ASKING `UpdateTickEnabled` AND NEVER BY WRITING THE TICK FLAG ITSELF. See that
	 * function; this is the half of the pair that would otherwise freeze a unit mid-slide.
	 */
	void FinishDamageFlash();

	/**
	 * The ONE arbiter of `SetActorTickEnabled`: on while either clock wants it, off otherwise.
	 *
	 * IT EXISTS BECAUSE `SetActorTickEnabled` IS A SINGLE BOOLEAN AND TWO FEATURES NOW WANT IT,
	 * AND THE FAILURE IT PREVENTS IS CONCRETE RATHER THAN THEORETICAL: a damage flash retiring
	 * while a move tween is in flight would disable the tick and **freeze a unit halfway between
	 * two hexes, forever, with a green build and no log**. The mirror is equally real -- a tween
	 * retiring mid-flash would leave a unit permanently red and displaced.
	 *
	 * IT IS THE ONLY CALLER OF `SetActorTickEnabled(false)` IN THIS CLASS, AND THAT IS THE
	 * INVARIANT RATHER THAN A DESCRIPTION. `FinishTween`'s trailing disable became a call to this
	 * function on 2026-09-06 and that was the ONLY edit that function received. A third feature
	 * that writes the flag directly reintroduces the defect on both of its exits at once.
	 *
	 * THE ARMING SIDE IS DELIBERATELY NOT ROUTED THROUGH IT. `PlayRouteSlide` and `ApplyUnitView`
	 * still call `SetActorTickEnabled(true)` at the point they arm, because turning a tick ON
	 * that another clock also wants is idempotent and cannot strand anything; only the OFF
	 * direction can, and only the OFF direction needs an arbiter.
	 */
	void UpdateTickEnabled();

	/**
	 * How long the tween currently armed lasts, in seconds. Meaningless with fewer than two
	 * waypoints.
	 *
	 * ONE EXPRESSION AND NOT THREE, WHICH IS A DRIFT RISK CLOSED RATHER THAN A TIDY-UP.
	 * `MoveTweenSeconds * (waypoints - 1)` is the per-hex-step user decision that field's block
	 * argues; `Tick` held the only copy of it until `PlayRouteSlide` needed to REPORT the same
	 * number to a caller that paces a tour by it. Two copies of that product is a tour whose
	 * clock and whose picture disagree the day the profile changes, and the disagreement is
	 * silent -- the unit simply arrives early or the camera waits too long.
	 *
	 * TWO CALL SITES TODAY, AND THAT IS SAID BECAUSE IT WAS BRIEFED AS THREE: `Tick`, which
	 * divides `TweenElapsed` by it, and `PlayRouteSlide`, which returns it. `ApplyUnitView` arms
	 * a tween and needs no duration, because nothing waits on its slide.
	 *
	 * `double` AND NOT `float`, matching `Tick`'s own arithmetic exactly so that inlining it
	 * here changes no result.
	 */
	double TweenDurationSeconds() const
	{
		return static_cast<double>(MoveTweenSeconds) * static_cast<double>(TweenOffsets.Num() - 1);
	}

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

	/**
	 * The polyline the picture walks, `Body`-relative, in visiting order. Empty when no tween
	 * is in flight.
	 *
	 * [REPLACED `TweenStartOffset` ON 2026-09-02. Everything that field's block claimed
	 * transfers VERBATIM and is restated here rather than cited, because a citation to a
	 * deleted member is a dangling one.]
	 *
	 * IT IS STATE IN THIS CLASS THAT NO MODEL HOLDS, together with `TweenArcLengths` and
	 * `TweenElapsed` below, and the header block's correction names all three rather than
	 * leaving them to be discovered. Its terminal value is EMPTY with `Body` at exact relative
	 * zero; a clock drives it there; nothing reads it to decide anything; and it is not a BIT,
	 * so it cannot make one unit draw a marker another does not.
	 *
	 * IT IS NOT A `UPROPERTY`, AND THAT IS A DECISION RATHER THAN AN OMISSION. It needs no
	 * garbage collection -- an array of `FVector` owns no object -- and leaving it unreflected
	 * keeps it off any future reflected walk that a clause about actor-held presentation state
	 * might grow. `FStratBuildAffordance` on the controller is unreflected for the same reason
	 * and says so in its own header.
	 *
	 * ELEMENT 0 IS RE-READ OFF `Body` ON EVERY ARMING AND IS NEVER A REMEMBERED DESTINATION.
	 * See `ApplyUnitView`: it is OVERWRITTEN with `Body->GetComponentLocation()` measured before
	 * the actor snaps, so an order arriving mid-slide restarts from where the unit is ACTUALLY
	 * DRAWN and cannot pop. That is also why there is no `LastAppliedLocation` field here.
	 *
	 * [RE-SCOPED 2026-09-02, SAME DAY, BY `PlayRouteSlide`. WRITTEN FLAT. THE INVARIANT IS
	 * NARROWED TO THE PATH IT WAS ALWAYS ABOUT AND IS NOT WEAKENED ON THAT PATH.] It said:
	 * RETRACTED> "THE LAST ELEMENT IS EXACTLY `FVector::ZeroVector` BY CONSTRUCTION AND NEVER BY
	 * RETRACTED>  ARITHMETIC. `ApplyUnitView` APPENDS it rather than converting the route's final
	 * RETRACTED>  hex, which would produce zero only if two floating-point expressions agreed to
	 * RETRACTED>  the bit."
	 *
	 * ON `ApplyUnitView`'S ARMING PATH THAT IS STILL TRUE, WORD FOR WORD, AND THE LINE THAT
	 * MAKES IT TRUE -- `TweenOffsets.Add(FVector::ZeroVector)` -- IS UNCHANGED BYTE FOR BYTE.
	 * It is legitimate there because `UStratMatchSubsystem::ApplyView` has already checked
	 * `PendingRoute->Last() == View.Hex` before converting, so the route's final hex IS the hex
	 * the actor was just put on: zero is both the right answer AND a construction rather than an
	 * arithmetic coincidence.
	 *
	 * ON `PlayRouteSlide`'S PATH THE LAST ELEMENT IS COMPUTED AND IS ZERO ONLY BY ARITHMETIC,
	 * on the one step where a unit's route happens to end on the hex it finishes the tour on.
	 * That is not a defect and it is not a second rule -- it is the definition of a PARK.
	 *
	 * SO THE MEMBER CARRIES ONE INVARIANT TRUE OF BOTH PATHS, AND IT IS THIS: **`Body` at
	 * relative `TweenOffsets.Last()` draws the unit over the route's own last hex.** Both paths
	 * satisfy it, the terminal-zero property is the special case of it where that hex is also
	 * the actor's, and `TweenRestOffset` is where the slide comes to rest for exactly that
	 * reason.
	 */
	TArray<FVector> TweenOffsets;

	/**
	 * Cumulative distance along `TweenOffsets`, same length, `[0] == 0`.
	 *
	 * WHY IT IS STORED RATHER THAN RE-DERIVED EVERY TICK: constant speed means constant speed
	 * IN WORLD UNITS, so the eased profile produces a DISTANCE and the distance has to be turned
	 * back into a segment. Re-summing the polyline every frame would be the same arithmetic
	 * repeated at 60 Hz for a value that cannot change while the tween is in flight -- nothing
	 * writes `TweenOffsets` between arming and retirement.
	 *
	 * AND WHY IT MATTERS AT ALL FOR A HEX ROUTE, WHOSE STEPS LOOK EQUAL: they are equal except
	 * for the FIRST one, which is overwritten with the drawn-position offset when an order
	 * arrives mid-slide and is then not exactly one hex long. Walking by index instead of by
	 * arc length would make that one segment run at a different speed from its neighbours, which
	 * is exactly the artefact the whole trapezoid exists to avoid.
	 */
	TArray<double> TweenArcLengths;

	/** Seconds since the current tween started. Meaningless unless `TweenOffsets` is non-empty
	 *  and the actor is ticking; see `TweenOffsets`, which states the terms all three are held
	 *  under. */
	float TweenElapsed = 0.0f;

	/**
	 * Where `Body` rests when the current tween retires. Zero on every path but the two that
	 * park.
	 *
	 * WHAT IT IS FOR. §2.11.2's tour shows AI moves on a board that ALREADY holds the final
	 * state, so a slide of the AI's third move must come to rest over the hex that move ended
	 * on -- not over the hex the unit is standing on now. `FinishTween` writes this instead of a
	 * zero literal, and that is the whole of parking.
	 *
	 * ITS TERMINAL VALUE IS ZERO AND SOMETHING ALWAYS DRIVES IT THERE, WHICH IS THE TEST THE
	 * HEADER BLOCK'S "no state the model holds" PARAGRAPH APPLIES TO THE OTHER THREE TWEEN
	 * MEMBERS AND THIS ONE MUST PASS TOO. Three writers set it to zero:
	 * `ApplyUnitView`'s arming branch, `CancelRouteSlide`, and `SnapToWorldLocation` (which
	 * zeroes it BEFORE `FinishTween`, because a hard cut must clear a park and not retire into
	 * one). `UStratMatchSubsystem::EndAiPlaybackTour` is what guarantees a driver exists for
	 * every way a tour can stop, including the ways nobody has written yet.
	 *
	 * [AMENDED 2026-09-02, SAME DAY, BY `ParkPictureAt`. WRITTEN FLAT BECAUSE THE HEADING
	 * SENTENCE NAMED A SINGULAR WRITER AND A READER GREPS THAT.] It said:
	 * RETRACTED> "Zero on every path but `PlayRouteSlide`."
	 * **TWO FUNCTIONS WRITE A NON-ZERO REST OFFSET AND THEY ARE THE ONLY TWO:** `PlayRouteSlide`,
	 * which parks at the end of a slide it arms, and `ParkPictureAt`, which parks with nothing
	 * running at all. The three zeroing writers are unchanged and still cover both, and
	 * `EndAiPlaybackTour`'s cancel reaches a picture parked by either -- a pre-park is an empty
	 * polyline with a non-zero rest offset, which `CancelRouteSlide` clears exactly as it clears
	 * a spent slide's.
	 *
	 * AND IT IS NOT A BIT, on the same test as the other three. No marker, mesh, material or
	 * visibility answers off it; it is a displacement with a clock behind it, and a match played
	 * with it permanently zero differs only in that AI moves do not animate.
	 *
	 * IT IS OUTSIDE `T-INT-05.NoActorHoldsPresentationBits`' SUBJECT, AND THAT WAS READ OFF THE
	 * CLAUSE RATHER THAN ASSUMED. Every assertion in that clause compares a field of
	 * `GetLastAppliedView()` against the model; a member that is not part of the cached view sits
	 * outside it, on exactly the footing the three tween members already there sit on.
	 *
	 * `ApplyUnitView` MUST NOT ZERO IT UNCONDITIONALLY, AND THIS IS THE EASIEST THING IN THE
	 * FILE TO GET WRONG. `AStratPlayerController::Tick` calls `UpdateHoverFromCursor` every
	 * frame and a hover crossing into a new hex reaches `ApplyView`, so `ApplyUnitView` FIRES
	 * DURING A TOUR whenever the player moves the mouse across a hex boundary. An unconditional
	 * clear there would pop every parked unit to its final hex on the next mouse move, and it
	 * would read as a random glitch rather than as a rule. The `bActorMoved` guard is what makes
	 * a hover harmless; the clear is conditioned on it.
	 */
	FVector TweenRestOffset = FVector::ZeroVector;

	/**
	 * Seconds since the current damage flash was armed. Zero means none is in flight, and zero
	 * is the ONLY value that means it.
	 *
	 * THE FOURTH SANCTIONED MEMBER, AND THE HEADER BLOCK'S "IT HOLDS NO *BIT*" PARAGRAPH NAMES
	 * IT RATHER THAN LEAVING IT TO BE DISCOVERED -- a sentence listing three fields reads as
	 * exhaustive, which is why that block was widened flat rather than edited in place. All four
	 * of the bounds it is held under are argued there: the terminal value is exactly zero and is
	 * WRITTEN not approached, a clock drives it there rather than a later model, nothing reads
	 * it to decide anything, and it is not a BIT.
	 *
	 * IT DOUBLES AS THE "IS A FLASH RUNNING" ANSWER RATHER THAN CARRYING A `bDamageFlashActive`
	 * BESIDE IT, AND THAT IS A DECISION. A second field would be a second answer to one
	 * question, and the two would disagree the first time a retirement path cleared one and not
	 * the other -- the shape `GetTweenWaypointCount` already refuses by reporting the live array
	 * instead of a cached count. `IsDamageFlashActive` is `> 0.0f` and nothing else.
	 *
	 * IT IS NOT A `UPROPERTY`, on `TweenElapsed`'s reasoning: a float owns no object, needs no
	 * garbage collection, and leaving it unreflected keeps it off any future reflected walk that
	 * a clause about actor-held presentation state might grow.
	 */
	float DamageFlashElapsed = 0.0f;
};
