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
// THE OVERLAYS ARE DECLARED IN C++, and the asymmetry is deliberate. The overlay components
// are not data -- their count follows the meanings the game has, not the rows a table has;
// they need distinct materials a designer will want to see in the details panel, and none is
// keyed by anything. They are constructor subobjects.
//
// THERE ARE THREE OF THEM AS OF 2026-08-23, AND THIS BLOCK USED TO SAY TWO:
// RETRACTED> "THE TWO OVERLAYS ARE DECLARED IN C++ ... there are exactly two of them for as
// RETRACTED>  long as §2.11.1 has a move phase and an attack phase"
// The third is §2.11.6-B's objective ring. THE COUNT WAS NEVER THE INVARIANT -- one meaning
// per component was -- and the retracted wording tied a structural rule to an arithmetic
// fact that a new GDD section was always going to move. A FOURTH MEANING MUST BE A FOURTH
// COMPONENT and never a third use of an existing one; see `ShowObjective` on why reusing
// `TargetOverlay` for the ring was refused.
//
// THE FOURTH ARRIVED ON 2026-09-01 AND THE SENTENCE ABOVE IS THE ONE THAT PREDICTED IT.
// `BuildPulseOverlay` is §2.11.5's BUILD pulse. IT LANDED IN C++ BECAUSE IT COULD NOT LAND
// ANYWHERE ELSE, which is the part worth reading rather than the arithmetic: `bBuildPulse`
// was published on `FStratFactoryView` and read by nothing for the whole of W8, and the
// record described the remaining work as needing "an editor session and no C++ at all".
// That was false, and this file is why -- there is no generic overlay map and no extensible
// slot here, so a content pass had nothing to bind a fourth meaning to. The count is not a
// contract, but the ABSENCE of a keyed collection is: a fifth meaning costs another
// component and another property, deliberately, so that each one keeps a name.
//
// TWO PROSE COUNTS ELSEWHERE IN THIS FILE MOVED WITH IT AND ARE STAMPED WHERE THEY SIT --
// the `OverlayMesh` property's "three components, three materials", and the sentence
// directly above that named the three components one by one. Both are now count-free for
// the reason this block already gives.
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
//   the selection machine; this class exposes the overlays it drives and nothing more.
//   [CORRECTED 2026-08-23: this line said "the two overlays it drives". There are THREE as
//   of that date -- the third is §2.11.6-B's ring -- and the count was never what the
//   sentence was about. It is now count-free rather than re-numbered, so the next section
//   that needs a component does not have to come back here. The invariant is stated once,
//   at "THE OVERLAYS ARE DECLARED IN C++" above: one meaning per component.]
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
 *
 * [AMENDED 2026-08-29. There are now TWO outcomes and not one: rebuilt whole, or NOT
 * TOUCHED AT ALL. `ApplyHexes` compares the model it is handed against these arrays first
 * and returns without clearing anything when they already agree -- see that method's own
 * block for the per-frame churn that made the third possibility, "patched", still the one
 * this struct refuses. Nothing above is retracted: an array that is either rewritten in
 * full or left exactly as it was cannot acquire the off-by-one either.]
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
	 * RECONCILED AGAINST THE MODEL, NEVER PATCHED TOWARDS IT. `StratViewModel.h` states
	 * that the model "is a COMPLETE DESCRIPTION OF WHAT SHOULD BE ON SCREEN AND NEVER A
	 * DELTA"; a board that patched itself towards the model would be correct only if every
	 * previous frame had also been correct, which is the property T-INT-05 exists to make
	 * structurally impossible to lose. What is drawn after this call is therefore a
	 * function of `Hexes` ALONE and of no earlier call.
	 *
	 * AND IT IS IDEMPOTENT WITHOUT TOUCHING THE RENDERER. [AMENDED 2026-08-29.] This block
	 * used to read:
	 * RETRACTED>  "A COMPLETE REBUILD ON EVERY CALL, and that is the reconciliation posture
	 * RETRACTED>   rather than a shortcut. ... Terrain does not change during a match today,
	 * RETRACTED>   so the cost is paid once"
	 * The last clause was the load-bearing error, and it was TRUE WHEN WRITTEN: a refresh
	 * happened once per turn. Commit `1da4198` (2026-08-27) moved hover resolution off an
	 * Enhanced Input handler that was measured never to fire and onto
	 * `AStratPlayerController::Tick`, and `ApplyHoverChange` -> `RefreshFromMachine` ->
	 * `UStratMatchSubsystem::ApplyView` reaches this method -- so the cost became per hover
	 * change rather than per turn, and the user's report was tiles that "flicker in what
	 * seems to me a switch between visible and hidden" while the cursor moved over the
	 * board. `ClearInstances` + N `AddInstance` on a `UHierarchicalInstancedStaticMeshComponent`
	 * marks its cluster tree outdated, and the tree feeds both the draw and the trace that
	 * `HexAtInstance` reads.
	 *
	 * SO THIS METHOD NOW COMPARES BEFORE IT CLEARS. If every layer's `InstanceHexes`
	 * already spells exactly the hex sequence in `Hexes` -- same order, same terrain
	 * grouping, same count, with the component's own instance count agreeing -- it returns
	 * true having touched nothing. The comparison reads only state the class already kept
	 * for picking; there is no cached copy of the model and therefore nothing that can go
	 * stale against one. THE FIX IS HERE AND NOT IN THE HOVER PATH deliberately:
	 * `ApplyHoverChange` being the single refresh decision is stated as load-bearing by
	 * `AStratPlayerController`, and a hover-only bypass would have been a second refresh
	 * sequence -- exactly what `RefreshFromMachine`'s block exists to forbid. Every caller
	 * gets the cheap no-op, not just the one that revealed the need for it.
	 *
	 * WHAT THE EARLY-OUT CANNOT SWALLOW, by construction rather than by care: a terrain
	 * whose mesh was unset when the board was last drawn contributed no instances, so its
	 * hexes cannot match and the rebuild runs and reports again -- REPORTS, and the fact
	 * that it now also DRAWS is `LayerFor`'s and not this early-out's, which is the subject
	 * of the retraction in `DrawsExactlyTheseHexes`. Assigning `TerrainMeshes` after a
	 * layer's component exists used to reach no component, because `LayerFor` set a mesh
	 * only at creation; its find path now re-reads the configuration. **THE CONDITION IS NOT
	 * RESTATED HERE and this sentence does not claim to know where it is stated -- it says
	 * only where to READ it: `LayerFor`'s DEFINITION, which carries the measurement behind
	 * it.** An earlier draft said the declaration was "the only place that says under what
	 * condition", which was false twice over -- the definition says it too, and so does
	 * `Tools/architect/state/global.md`'s banner -- and a shape restated in this paragraph has
	 * already gone stale once. That was an older defect this early-out neither caused
	 * nor cured, and no
	 * sentence in this class may claim the cure for it. A layer holding instances for a
	 * terrain the new model dropped fails the end-of-layer check; and anything that cleared
	 * a component behind this class's back fails the instance-count agreement. When a §2.7
	 * capture starts repainting a hex, the hex list changes and this call already handles it
	 * with no new path.
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

	/**
	 * §2.11.6-B's objective ring: lights the ONE hex the guided opening's beat 2 names.
	 *
	 * WHAT GAP THIS CLOSES. §2.11.6-B beat 2's directive reads "Move the Infantry onto the
	 * ringed Factory", and until this landed nothing drew a ring. Confirmed at the keyboard
	 * on 2026-08-23: the only way a player learned which factory was meant was by reading
	 * the log. `strat-editor-builder` could not close it from `Content/` because there was
	 * nothing in C++ to bind to -- this class declared exactly two overlays.
	 *
	 * A THIRD COMPONENT AND EMPHATICALLY NOT A THIRD USE OF `TargetOverlay`. That component
	 * is §2.6's ATTACK targets and `T-UI-02.AttackIsClosedForTheMarkedInfantry` asserts
	 * `ShowTargets` filled exactly one component and that it is not the reach one -- so a
	 * ring drawn there would be both semantically wrong and clause-visible, and worse, it
	 * would be lit during beat 1a precisely when that clause says nothing may be. The ring
	 * and the attack targets are also on screen at once in the ordinary case.
	 *
	 * ONE HEX AND NOT A SET, and the signature says so rather than documenting it. §4.7's
	 * `guidedOpening.objective` is a single authored hex per seat; a `TArray` here would
	 * invite a caller to pass "every objective", which is the "nearest objective" heuristic
	 * §2.11.6 forbids by name arriving through the back door.
	 *
	 * THE HEX IS HANDED IN AND IS NEVER FOUND HERE. It comes from
	 * `FStratGuidanceView::ObjectiveHex`, which `FStratGuidedOpening::DecorateViewModel`
	 * copied from `FStratBridge::GuidedOpeningHexes`. This class holds no bridge and could
	 * not look one up if it wanted to -- the same inability `ShowReach` records.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Board")
	void ShowObjective(FIntPoint Hex);

	/** Clears the objective ring. Called on every refresh where guidance is not running, so
	 *  the ring "clears in the same frame as the strip" structurally rather than by promise
	 *  -- see `FStratGuidedOpening::SkipGuidance`. */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Board")
	void ClearObjective();

	/**
	 * How many hexes the §2.11.6-B objective ring is currently lighting: 0 or 1.
	 *
	 * OFF THE COMPONENT, NOT A CACHED NUMBER, for `GetTargetOverlayCount`'s reason -- a
	 * cached count agrees with the code that set it while disagreeing with the screen.
	 *
	 * IT EXISTS FOR A CLAUSE, and an accessor with no named caller is the shape that rots.
	 * The clause is named in this change's handoff: without it, "the ring clears when
	 * guidance goes inactive" is unobservable from outside this class, because
	 * `ObjectiveOverlay` is protected -- the same hole `GetTargetOverlayCount` was added to
	 * close for the attack overlay.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Board")
	int32 GetObjectiveOverlayCount() const;

	/** How many hexes are currently drawn. Exposed so a caller can tell "the board was
	 *  never built" from "the board was built and is empty" without reaching into the
	 *  layers, which is the same distinction `FStratBridge::MakeUiSnapshot` refuses to let
	 *  anyone lose. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Board")
	int32 GetDrawnHexCount() const;

	/**
	 * How many hexes the §2.6 ATTACK-TARGET overlay is currently lighting.
	 *
	 * NOT `GetDrawnHexCount`, WHICH IS THE TILE COUNT. The two are easy to conflate because
	 * both are "hexes on this board", and conflating them is the reason this exists: the
	 * tile count is 99 on Ferrum Crossing whatever the overlays are doing, so it can neither
	 * confirm nor deny that a target set was cleared.
	 *
	 * IT EXISTS FOR A CLAUSE AND THE CLAUSE IS NAMED, because an accessor with no stated
	 * caller is the shape that rots. `T-UI-02.AttackIsClosedForTheMarkedInfantry` pins
	 * §2.11.6-B beat 1a's "its attack targets are not lit", and before this it could assert
	 * only the SUBMISSION half -- that no command reached the bridge. `TargetOverlay` is
	 * `protected`, so the not-lit half was unobservable and the clause pinned a restriction
	 * that is TWO restrictions while looking complete.
	 *
	 * TWO READERS TODAY, NOT ONE. `Source/StratPlay/Tests/StratGuidanceInputGates.cpp` is
	 * the clause above; `Source/StratPlay/Tests/StratBoardPicking.cpp` is
	 * `T-UI-02.ReachOverlayIsNotComputedHere`, which measures the target overlay beside the
	 * reach one. Both call this by name. That count is stated because the sentence it
	 * replaces was already wrong by the time the second reader landed. It said:
	 * RETRACTED> "The one existing reader, `StratBoardPicking.cpp` …"
	 *
	 * THE HAZARD THIS WAS BUILT AGAINST IS DISCHARGED, and saying so is the point of the
	 * paragraph. `StratBoardPicking.cpp` used to reach the count by scanning `GetComponents`
	 * for the component that is NOT the reach overlay -- identification by elimination, which
	 * would have started measuring the wrong component the day a third overlay landed. That
	 * is why this accessor exists. `strat-test-author` then converted that clause to read by
	 * name and withdrew the scan, so the liability is gone from the tree rather than
	 * standing in it. This block previously described it as live:
	 * RETRACTED> "The one existing reader, `StratBoardPicking.cpp`, got at the count by scanning
	 * RETRACTED>  `GetComponents` for the component that is not the reach overlay -- a workaround
	 * RETRACTED>  that identifies the subject by elimination and would silently start measuring
	 * RETRACTED>  the wrong component the day a third overlay lands."
	 * A reader sent to that file looking for the scan would not have found one.
	 *
	 * THE HYPOTHETICAL IN THAT RETRACTED QUOTE IS NO LONGER HYPOTHETICAL, STAMPED 2026-08-23.
	 * "The day a third overlay lands" arrived: `ObjectiveOverlay` exists. Reading these
	 * counts BY NAME is therefore now load-bearing rather than merely tidier, and a
	 * `GetComponents` scan for "the one that is not the reach overlay" would today return
	 * whichever of two components the iteration order happened to reach first.
	 *
	 * READ-ONLY AND OFF THE COMPONENT, not off a cached number. There is deliberately no
	 * `SetTargetCount` and no member behind this: `ShowTargets` is the only writer, the
	 * component is the truth, and a cached count is a second answer that can disagree with
	 * what is on screen -- the same reason `UStratMatchSubsystem` refuses a `bSeeded` mirror
	 * beside its bridge pointer.
	 *
	 * ZERO WHEN THE COMPONENT DOES NOT EXIST, which is indistinguishable here from "exists
	 * and lights nothing". That collapse is safe for this accessor's one purpose -- both
	 * mean nothing is lit -- and `GetDrawnHexCount` already carries the built-versus-empty
	 * distinction for anyone who needs it about the board itself.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Board")
	int32 GetTargetOverlayCount() const;

	/**
	 * §2.11.5's BUILD pulse: lights every factory tile whose `FStratFactoryView::bBuildPulse`
	 * is set for the model's viewing side.
	 *
	 * WHAT GAP THIS CLOSES, AND IT IS A GAP THE RECORD DESCRIBED AS CLOSED.
	 * `FStratFactoryBuildPulse::bShouldPulse` was composed by the bridge, mirrored onto
	 * `FStratFactoryView::bBuildPulse`, published, and READ BY NOTHING. The remaining work
	 * was recorded as drawing-only -- "an editor session and no C++ at all" -- which this
	 * class falsified by construction: its overlays are three named components with three
	 * fixed material slots and no keyed collection, so `Content/` had nothing to bind to.
	 * This is that fourth binding, and the material behind it stays the content lane's.
	 *
	 * A SET AND NOT ONE HEX, WHICH IS THE OPPOSITE OF `ShowObjective` AND FOR THE REASON
	 * THAT SIGNATURE GIVES. §4.7's `guidedOpening.objective` is a single authored hex per
	 * seat, so a `TArray` there would invite "every objective" through the back door. The
	 * pulse is genuinely per factory and several are lit at once in the ordinary case --
	 * Ferrum Crossing ships four buildable factories -- so a single-hex signature here would
	 * be the defect, not the guard.
	 *
	 * IT DECIDES NOTHING AND CANNOT. The set is handed in. Every entry came from
	 * `FStratFactoryView::bBuildPulse`, which is `FStratFactoryBuildPulse::bShouldPulse`
	 * copied whole -- and THAT expression is `bBuildAvailable && bAnyUnitAffordable`,
	 * composed in `FStratBridge::FactoryBuildPulses` out of the rules module's own
	 * `available` and `affordable`. This class holds no bridge and could not fold the two
	 * halves if it wanted to, which is the same inability `ShowReach` records and the same
	 * substitution T-UI-02 exists to catch.
	 *
	 * AN EMPTY SET IS THE ORDINARY CASE ON HALF OF ALL TURNS AND IS NOT A FAULT. See
	 * `ClearBuildPulses`.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Board")
	void ShowBuildPulses(const TArray<FIntPoint>& Hexes);

	/**
	 * Clears the BUILD pulse overlay.
	 *
	 * A DARK BOARD IS OFTEN THE CORRECT DRAWING, AND SAYING SO HERE IS THE POINT OF THIS
	 * BLOCK. `bShouldPulse` inherits `side == activeSide` from `strat::canBuildAt`, so on
	 * the opponent's hot-seat turn EVERY factory's pulse is dark board-wide -- not because a
	 * material is missing, not because the overlay failed, but because the viewing side may
	 * not build. A reader who meets a dark board first will reach for
	 * `BuildPulseMaterial` being unset, and that is the wrong place to look on every other
	 * turn. The derivation and its nine other refusals live at
	 * `FStratFactoryBuildPulse::bShouldPulse` in `Source/StratBridge/StratBridge.h`, which
	 * is the authority; this is a pointer to it and deliberately not a copy of it.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Board")
	void ClearBuildPulses();

	/**
	 * How many factory tiles the §2.11.5 BUILD pulse is currently lighting.
	 *
	 * OFF THE COMPONENT, NOT A CACHED NUMBER, for `GetTargetOverlayCount`'s reason -- a
	 * cached count agrees with the code that set it while disagreeing with the screen.
	 *
	 * IT EXISTS FOR A CLAUSE, and an accessor with no named caller is the shape that rots.
	 * The clause is named in this change's handoff:
	 * `Stratocracy.StratPlay.T-UI-02.BuildPulseOverlayIsDrawnFromTheModelAlone`. Without it
	 * "the pulse is drawn where and only where `bBuildPulse` is set" is unobservable from
	 * outside this class, because `BuildPulseOverlay` is protected -- the same hole
	 * `GetTargetOverlayCount` and `GetObjectiveOverlayCount` were each added to close.
	 *
	 * ZERO WHEN THE COMPONENT DOES NOT EXIST, indistinguishable here from "exists and lights
	 * nothing" -- the same collapse `GetTargetOverlayCount` declares, safe for the same
	 * reason: both mean nothing is lit.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Board")
	int32 GetBuildPulseOverlayCount() const;

protected:
	/** Nothing but a transform. The tile components are runtime-created and attach here;
	 *  the overlays are constructor subobjects and attach here too.
	 *  [CORRECTED 2026-08-23: this said "the two overlays". `ObjectiveOverlay` is a third
	 *  `CreateDefaultSubobject` calling `SetupAttachment(BoardRoot)` beside the other two,
	 *  so the count was false; it is now count-free for the reason the header block gives.] */
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

	/** §2.11.6-B's objective ring. See `ShowObjective` on why this is a THIRD component and
	 *  not a third meaning loaded onto `TargetOverlay`. */
	UPROPERTY(VisibleAnywhere, Category = "Stratocracy|Board")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> ObjectiveOverlay;

	/** §2.11.5's BUILD pulse. See `ShowBuildPulses` on why this is a FOURTH component and
	 *  not a fourth meaning loaded onto `ObjectiveOverlay` -- the ring and the pulse are on
	 *  screen at once during the guided opening, whose beat 3 is a Build. */
	UPROPERTY(VisibleAnywhere, Category = "Stratocracy|Board")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> BuildPulseOverlay;

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

	/** The flat quad every overlay instances. ONE MESH, ONE COMPONENT PER MEANING, ONE
	 *  MATERIAL EACH -- see `ShowTargets`, `ShowObjective` and `ShowBuildPulses`.
	 *  [STAMPED 2026-09-01. This said "One mesh, three components, three materials", which
	 *  went false the moment `BuildPulseOverlay` landed. It is count-free now rather than
	 *  re-numbered, for the header block's stated reason: the count was never the
	 *  invariant.] */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Board")
	TObjectPtr<UStaticMesh> OverlayMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Board")
	TObjectPtr<UMaterialInterface> ReachMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Board")
	TObjectPtr<UMaterialInterface> TargetMaterial;

	/** §2.11.6-B's ring. ASSIGNED AS OF 2026-08-24 AND THAT IS THE STATE THIS SHIPS IN:
	 *  `BP_StratBoard`'s default carries `MI_Overlay_Objective` here. The material instance
	 *  and its assignment stay the CONTENT lane's, and this file must not name a `/Game/`
	 *  path to fill it -- which is why it is still `EditDefaultsOnly` and still has no
	 *  initializer. Unset remains LEGITIMATE for any other Blueprint of this class: it draws
	 *  the ring in `OverlayMesh`'s own material, which is a visibly-wrong ring rather than an
	 *  absent one. This block said unset was the shipping state until the default landed. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Board")
	TObjectPtr<UMaterialInterface> ObjectiveMaterial;

	/**
	 * §2.11.5's BUILD pulse. **[CORRECTED 2026-09-06. THIS BLOCK OPENED "UNSET IS THE STATE
	 * THIS SHIPS IN AS OF 2026-09-01, AND THAT IS STATED HERE RATHER THAN LEFT TO BE
	 * DISCOVERED. No material instance for the pulse exists in `Content/` yet; `BP_StratBoard`
	 * carries no default here" -- AND BOTH HALVES OF THAT ARE NOW FALSE.** The instance exists
	 * and is assigned: `MI_Overlay_BuildPulse` was authored by the
	 * content lane and set on `BP_StratBoard`'s class default, "which was `None` before",
	 * verified there by read-back through a different route than the write and recorded in
	 * `Tools/architect/state/content.md`. RE-DERIVED HERE FROM THE SHIPPED BYTES rather than
	 * taken on that record's word: a byte census of `Content/StratPlay/BP_StratBoard.uasset`
	 * returns `BuildPulseMaterial` once and `MI_Overlay_BuildPulse` twice. THE 1/2 PAIR IS ONLY
	 * EVIDENCE BECAUSE A KNOWN-ASSIGNED SIBLING PRINTS THE SAME SHAPE: `ObjectiveMaterial` 1
	 * and `MI_Overlay_Objective` 2, on the property directly above whose own block has read
	 * ASSIGNED since 2026-08-24. A fabricated property name returns 0, and the package is a
	 * real one -- first bytes `c1 83 2a 9e`, the Unreal magic, not a git-lfs pointer line. The
	 * `BeginPlay` arm in `StratBoardActor.cpp` carried the same false claim and is corrected
	 * with it.]** THE PART OF THE RETRACTED PARAGRAPH THAT WAS ABOUT THIS FILE IS UNTOUCHED BY
	 * ANY OF THAT, AND IS WHY THIS BLOCK IS CORRECTED RATHER THAN DELETED: the instance and
	 * its assignment are the CONTENT lane's, this class must not be given one from C++, and
	 * that is why this is `EditDefaultsOnly`, has NO initializer, and names no `/Game/` path.
	 * A material now existing is not a licence to name its path here. `ObjectiveMaterial`
	 * directly above remains the model, and both blocks now record the same arc.
	 *
	 * UNSET IS LEGITIMATE AND THE FAILURE IT PRODUCES IS VISIBLE RATHER THAN SILENT: the
	 * pulse draws in `OverlayMesh`'s own material, so a lit factory looks like a reach
	 * highlight sitting on it. That is a wrong-looking pulse and not an absent one, which is
	 * the same trade `ObjectiveMaterial` records and the reason neither is logged while a
	 * missing `OverlayMesh` is -- a missing MESH draws nothing at all and has to announce
	 * itself.
	 *
	 * NO ANIMATION IS DRIVEN FROM C++ AND NONE MAY BE ADDED HERE ON AN ASSUMPTION. **[THE RULE
	 * STANDS AND ITS 2026-09-01 PREMISE DOES NOT, CORRECTED 2026-09-06. The premise read
	 * "`MI_Overlay_Objective`, the only member of this material family today, is a
	 * `MaterialInstanceConstant` with one vector parameter, ZERO scalar parameters and ZERO
	 * static switches. So there is no parameter named anywhere in the tree for a pulse to
	 * drive" -- and the family has since grown a member that has exactly such parameters.**
	 * `MI_Overlay_BuildPulse` is parented to `M_OverlayPulse`, a
	 * duplicate of the shared `M_Translucent` master whose Opacity input the content lane
	 * rewired through a `Time`-driven sine, with three scalar parameters carrying the tuning.
	 * MEASURED FROM THE BYTES, NOT RELAYED: a census of `M_OverlayPulse.uasset` returns
	 * `PulseSpeed`, `PulseMin`, `PulseMax`, `MaterialExpressionTime` and
	 * `MaterialExpressionSine` once each; the CONTROL is the untouched
	 * `Content/AdvancedTurnBasedTileToolkit/Core/GridUI/M_Translucent.uasset`, which returns 0
	 * for all five while both files return 7 for `Color`, so the counter was shown able to
	 * speak on both before its zeros were read.
	 *
	 * SO THE RULE IS NOW SATISFIED BY CONSTRUCTION RATHER THAN BY ABSENCE, WHICH IS THE
	 * STRONGER FORM AND IS THE REASON TO KEEP IT. The old argument was "do not animate from
	 * C++ because there is no parameter to animate"; that argument has expired. The one that
	 * replaces it is that the pulse ALREADY PULSES, in the material, off `Time` -- so C++
	 * driving a dynamic instance would be a second animator fighting the first, and retuning
	 * would need a rebuild instead of a material edit. HOW THE PULSE PULSES IS THE CONTENT
	 * LANE'S DECISION, in the material itself; this class turns instances on and off and
	 * nothing more. WHAT IS NOT SETTLED FROM A CHECKOUT, said rather than implied: whether
	 * that graph COMPILES and what it looks like on screen. A byte census reads names, not
	 * pixels, and `content.md` records `get_statistics` returning zeros for a known-good
	 * control -- a human at the keyboard remains the only instrument for the visual claim.]**
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Board")
	TObjectPtr<UMaterialInterface> BuildPulseMaterial;

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
	 * The hexes each overlay component is currently drawing, in the order they were added.
	 *
	 * THE OVERLAYS' EQUIVALENT OF `FStratTerrainLayer::InstanceHexes`, and they exist for
	 * `ApplyHexes`'s reason rather than for picking: the overlays carry NO collision (see
	 * the constructor), so nothing traces against them and nothing needs to map an instance
	 * index back to a hex. `FillOverlay` reads these only to answer "am I being asked to
	 * draw what is already drawn?", which on a hover crossing it is -- the reach and target
	 * sets are a function of the SELECTION, and `RefreshFromMachine` repaints all three
	 * every time the cursor changes hex.
	 *
	 * WRITTEN FROM WHAT WAS DRAWN AND NEVER FROM WHAT WAS REQUESTED. An overlay with no
	 * `OverlayMesh` draws nothing, and caching the request in that case would make a mesh
	 * assigned afterwards never take effect -- so the miss case resets these to empty.
	 */
	UPROPERTY()
	TArray<FIntPoint> ReachDrawnHexes;

	/** As `ReachDrawnHexes`, for `TargetOverlay`. */
	UPROPERTY()
	TArray<FIntPoint> TargetDrawnHexes;

	/** As `ReachDrawnHexes`, for `ObjectiveOverlay`. */
	UPROPERTY()
	TArray<FIntPoint> ObjectiveDrawnHexes;

	/** As `ReachDrawnHexes`, for `BuildPulseOverlay`. IT EARNS ITS KEEP MORE THAN THE OTHER
	 *  THREE DO: the pulse set is a function of the TURN, not of the selection, so it is
	 *  identical across every hover refresh within one turn and this cache is what stops
	 *  `ApplyView` clearing and re-adding four instances on every mouse move. */
	UPROPERTY()
	TArray<FIntPoint> BuildPulseDrawnHexes;

	/**
	 * Is the board already drawing exactly this hex list?
	 *
	 * TRUE ONLY WHEN A REBUILD WOULD BE A NO-OP, and it is deliberately conservative: any
	 * doubt answers false and costs one rebuild, which is the cost this class paid on every
	 * call until 2026-08-29. It checks the sequence, not a set or a count -- `InstanceHexes`
	 * is `HexAtInstance`'s index map, so two boards with the same hexes in a different order
	 * are genuinely different drawings and only the sequence proves the map survives.
	 *
	 * READS NO STATE THIS CLASS DID NOT ALREADY KEEP. There is no remembered model here; the
	 * answer is derived from the components and the parallel arrays that picking depends on,
	 * so a "yes" is a statement about what is on screen rather than about what was last
	 * requested.
	 */
	bool DrawsExactlyTheseHexes(const TArray<FStratHexView>& Hexes) const;

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

	/**
	 * Points a tile layer's component at the mesh its terrain id is configured with --
	 * `TerrainMeshes` first, `FallbackTerrainMesh` second, and nothing at all if neither is
	 * set, which `ApplyHexes` reports rather than treats as a failure.
	 *
	 * ONE PROCEDURE BECAUSE THE DEFECT IT CLOSES WAS TWO SITES DISAGREEING. `LayerFor`
	 * assigned a mesh when it created a component and nowhere else, so a board applied
	 * before its meshes existed kept null-meshed components for the life of the actor. The
	 * find path calls this too now; two call sites reading one procedure cannot drift again.
	 */
	void AssignTerrainMesh(
		UHierarchicalInstancedStaticMeshComponent& Component, FName TerrainId) const;

	/**
	 * Finds or creates the layer for a terrain id, registering the component.
	 *
	 * BOTH PATHS RE-READ THE MESH CONFIGURATION, the find path unconditionally. It runs once
	 * per HEX, and `UStaticMeshComponent::SetStaticMesh` returns immediately when the mesh is
	 * unchanged, so the common case is a `TMap::Find` and a pointer compare; and the only
	 * caller reaches it after `ApplyHexes` has already cleared every layer's instances, so
	 * there is no instance state to disturb. The definition carries the measurement and the
	 * false justification an earlier narrowing of it was blocked on. Pinned by
	 * `GATE-BOARDCHURN.AMeshAssignedAfterAnUnmeshedApplyDrawsOnTheNextApply`, in both of its
	 * halves -- the mesh arriving where there was none, and the mesh being CHANGED.
	 */
	FStratTerrainLayer& LayerFor(FName TerrainId);

	/** Points an overlay component at exactly these hexes. The shared tail of `ShowReach`,
	 *  `ShowTargets` AND `ShowObjective`, so they cannot drift in how they clear or how they
	 *  offset.
	 *  [CORRECTED 2026-08-23, AND THIS ONE WAS FOUND BY CLAIM SHAPE RATHER THAN BY THE PHRASE
	 *  "two overlays", WHICH IT NEVER CONTAINED. It said "The shared tail of `ShowReach` and
	 *  `ShowTargets`, so the two cannot drift" -- an overlay-count claim spelled as a caller
	 *  list, invisible to a sweep for the wording anyone would think to grep. `ShowObjective`
	 *  calls this too.]
	 *
	 *  IT TAKES ITS CALLER'S CACHE AND IS NO LONGER `const`. [AMENDED 2026-08-29, with
	 *  `ApplyHexes`.] Handed the hexes it is already drawing it returns without clearing,
	 *  for the tiles' reason: a hover crossing repaints all three overlays with content
	 *  that did not change, and a `ClearInstances` + re-add is not free on a component
	 *  whose cluster tree the renderer is reading. The cache is passed IN rather than
	 *  selected here by comparing `Overlay` against the three members, because that
	 *  comparison would be a fourth place that knows which component means what -- the
	 *  header block's "one meaning per component" rule read backwards. */
	void FillOverlay(UHierarchicalInstancedStaticMeshComponent* Overlay,
		TArray<FIntPoint>& DrawnHexes, const TArray<FIntPoint>& Hexes);
};
