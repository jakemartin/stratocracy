// GDD §2.11.1 / §4.9 -- the player controller. The input path, and the one object that
// wires the selection machine to a bridge, a board and a screen.
//
// WHAT GAP THIS CLOSES. `AStratGameMode.h` recorded the absence in advance: "there is no
// Strat player controller yet. Phase 4 builds it, with Enhanced Input and the selection
// state machine, and sets it here in the same pass." Until this class existed, the match
// subsystem could seed a board and reconcile actors onto it and no human could touch it --
// `UStratMatchSubsystem.h` states in its own "NOT IN THIS ROUND" list that it deliberately
// does not wrap the five `Submit*` methods, "because phase 4 owns the PlayerController, the
// selection machine and the `STRAT-CMD accepted` log line, and a wrapper landed early is a
// wrapper written without its caller." This is the caller.
//
// IT DECIDES NOTHING, AND THE MEASURE OF THAT IS HOW LITTLE IS IN THE .cpp. It turns an
// Enhanced Input trigger into an `EStratSelectionEvent`, turns a cursor position into a
// hex by asking the board, hands both to `FStratSelectionMachine`, and applies whatever
// comes back. There is no legality check here, no reachability, no damage number and no hex
// arithmetic -- not one coordinate is added to another anywhere in this class.
//
// THE SELECTION MACHINE IS A PLAIN MEMBER, BY VALUE, AND IT IS NOT A `UPROPERTY`. It cannot
// be one: `FStratSelectionMachine` is not a reflected type, on purpose, for the two reasons
// its own header gives -- it must be drivable in a test with no world, and a reflected copy
// of `bDone` on an actor is precisely what `T-INT-05.NoActorHoldsPresentationBits` forbids.
// So this controller HOLDS the machine and reads nothing out of it into a field of its own.
// The presentation bits reach the screen only through `DecorateViewModel`, on the model, on
// the way to `ApplyView`.
//
// PICKING GOES THROUGH THE INSTANCE INDEX AND NEVER THROUGH AN INVERSE. `AStratBoardActor`
// deliberately has no `HexAtWorldLocation`, and phase 3 recorded why: "a rounding inverse of
// `WorldLocationOfHex` would disagree with the forward map at tile edges, and disagreeing
// about which hex the cursor is on is how a player attacks the wrong unit." This class
// therefore traces under the cursor and hands the hit's COMPONENT and ITEM INDEX to
// `AStratBoardActor::HexAtInstance`, which is a lookup into the instance list the board
// itself built. A trace that hits something that is not a board tile is "the cursor is not
// on the board", which is an ordinary answer.
//
// THE MAPPING CONTEXT AND EVERY ACTION ARE NULL IN C++ AND SET ON A BLUEPRINT, AND THE
// NULL CASE IS SUPPORTED RATHER THAN TOLERATED. They are `EditDefaultsOnly` `TObjectPtr`
// properties -- never a `/Game/` literal, which this project forbids in gameplay C++
// outright. With them all null this controller adds no context, binds no action, logs one
// line per unset property saying so, and the match is still seeded, drawn and reconciled.
// [AMENDED, wave 0: this paragraph said "THE FOUR ACTIONS" and "all five null" -- two
// different counts of the same set, in one paragraph, and BOTH were already wrong before
// wave 0 touched anything: `OpenProductionMenuAction` landed on 2026-08-22 and `HoverAction`
// lands with this amendment, and the paragraph had counted neither. [`HoverAction` was
// REMOVED again on 2026-08-27 -- see the block where it used to be declared. This sentence is
// left standing as the history it is: it records what wave 0 did, and a set that shrinks
// invalidates a census exactly as a set that grows does, which is the same argument the
// sentence was already making.] The counts are removed
// rather than refreshed, AND NO ORDINAL IS WRITTEN IN THEIR PLACE -- not even in this note,
// whose own first draft called those two properties "the fifth" and "the sixth" and so
// restated the count it had just removed. A count of a set that is still growing is stale by
// the next property and nothing in a comment recomputes it; the properties themselves are the
// census. THE NOTES LIKE THIS ONE ARE COUNTED BY THE COMMAND IN `Tools/architect/state/
// engine.md`'s wave-0 entry, AND THAT COMMAND IS DELIBERATELY NOT SPELLED OUT HERE: its search
// token is the bracketed marker these notes open with, so any line quoting it inside `Source/`
// -- this line, had it done so -- becomes a sixteenth hit against fifteen real notes and the
// census silently counts itself. It did exactly that until the W0 gate measured it. One
// spelling, kept outside the tree it searches.] A controller that asserted on a null context
// would have made asset authoring a prerequisite for this class's build being green.
//
// THE ASSETS NOW EXIST -- an earlier spelling of this paragraph read "ARE NULL UNTIL PHASE
// 5", which describes a world that ended when the hot-seat milestone's phase 5 authored
// `IMC_Selection` and the `IA_*` actions THAT EXISTED THEN and set them on the `BP_` subclass
// a GameMode Blueprint points at. [AMENDED, wave 0: "the four `IA_*` actions". Historically
// accurate about that pass and left standing as history ever since -- but it was one more
// "four" among several in this file's opening, and a reader had no way to sort the
// historical one from the live ones. Scoped rather than deleted: what phase 5 did is
// unchanged, and only the reader's ability to mistake it for a live census is.] What is still
// true, and is the load-bearing half, is that the C++
// DEFAULTS are null and every use site below stays null-safe: this class must remain
// constructible and drivable with no asset in existence, because that is what lets it be
// built and reasoned about without the content pass.
//
// WHY THE VIEW MODEL IS REBUILT ON EVERY EVENT rather than read from
// `UStratMatchSubsystem::GetViewModel()`. That accessor is documented as "A RECORD, NOT A
// SOURCE ... nothing in this class consults it", and driving the selection machine from it
// would make what a click means depend on when someone last called `ApplyView`. Building
// fresh costs one snapshot projection per click and makes the machine a function of the
// rules state alone.
//
// IT NAMES NO `strat::` TYPE. `FStratBridge` is forward declared; `StratBridge.h` is
// included from the .cpp only, because this header declares a `UCLASS` and UHT must never
// see the vendored headers. Every bridge call the .cpp makes is one of the engine-typed
// façade methods added for exactly this caller.
//
// NOT IN THIS ROUND, with reasons:
// - CAMERA INPUT. `AStratCameraPawn` is the pawn and its own movement is its business; a
//   controller that bound camera axes would put the camera's input two objects away from
//   the camera. Phase 3 shipped the pawn without Enhanced Input and nothing in phase 4's
//   exit criterion moves it.
// - THE HOT-SEAT HAND-OVER KEY, STILL UNBOUND -- BUT NOT FOR THE REASON THIS BULLET USED
//   TO GIVE, AND THE PREMISE IS CORRECTED IN PLACE RATHER THAN DELETED. It used to read:
//   RETRACTED> "THE HOT-SEAT HAND-OVER SCREEN. `UStratMatchSubsystem::SetViewingSide`
//   RETRACTED>  exists and is deliberately NOT bound to a key here. §2.11's hand-over is a
//   RETRACTED>  screen with a confirmation on it -- who is looking matters -- and a key
//   RETRACTED>  that silently flipped the viewing side would let either player see the
//   RETRACTED>  other's board at any time. The affordance is UI work that no phase of this
//   RETRACTED>  milestone owns."
//   THE CITATION WAS FALSE. The GDD specifies no hand-over screen and no confirmation on
//   one. Measured against `E:\MultiAgent\stratocracy-content\Stratocracy_Prototype_GDD.md`
//   on 2026-08-25: four case-insensitive hits for hot-seat across the whole document, all
//   four in stretch-feature context (§1.5's resolved question 4 -- "2-player hotseat: in
//   scope or cut? RESOLVED: stretch only, off the critical path (§2.10)" -- plus the §2.10
//   scope table's STRETCH row and two pacing asides); ZERO hits for "hand-over", "handover"
//   or "pass the device"; and §2.11.5 states the prototype's complete screen list in as many
//   words -- "That is the complete screen list for the prototype: title/menu, briefing,
//   match, result." NO HAND-OVER SCREEN IS OWED. Recorded here so the next reader does not
//   re-derive the deferral from the same wrong citation and schedule a screen nobody asked
//   for.
//   THE HALF THAT SURVIVES IS THE WHOLE REASON THE KEY STAYS UNBOUND: a key that silently
//   flipped the viewing side would let either player see the other's board at any time.
//   That is a property of the BINDING and needs no screen behind it, so it does not expire
//   with the citation. If hot-seat is ever taken off the stretch list, whatever surface
//   drives the swap has to make the swap deliberate and visible; a bare keypress cannot.
// - CAPTURE and BUILD, AND THIS BULLET IS RETRACTED IN PLACE FOR BUILD AND ONLY FOR BUILD.
//   It used to read:
//   RETRACTED> "CAPTURE and BUILD. See `FStratSelectionMachine`'s own deferral list;
//   RETRACTED>  neither is on the phase list and neither has an answered affordance
//   RETRACTED>  question."
//   BUILD's affordance question was answered by the user on 2026-08-22: §2.11.5's
//   production menu opens on a DEDICATED INPUT ACTION -- `OpenProductionMenuAction` below --
//   and never on `EStratSelectionEvent::HexPrimary`. The ruling was made in exactly that
//   shape so that a primary click keeps meaning select / move / attack and
//   `FStratSelectionMachine` needs no BUILD arm; that struct is untouched by this change and
//   its own deferral bullet carries the same retraction.
//   AND THE CAPTURE HALF IS NOW RETRACTED TOO, ON EVIDENCE. It used to read "`SubmitCapture`
//   STILL HAS NO AFFORDANCE AND NO ENGINE CALLER. Nothing here covers it and its half of the
//   original bullet stands unretracted" -- carried as an unanswered affordance question.
//   IT IS ANSWERED, AND THE ANSWER IS THAT NOTHING IS OWED. `SubmitCapture` still has no
//   engine caller and must not acquire one; the three measurements are in
//   `UStratMatchSubsystem`'s own header block and in short:
//     (a) {S}2.11 rules a capture verb out in as many words -- "Capture and build need no
//         extra verbs. Capture is by presence ... a progress pip appears, NO BUTTON." So
//         this class owes no binding, not even a deferred one.
//     (b) the method is {S}4.10 SAVE-FORMAT TRANSPORT and is kept for that: `strat::
//         SaveCommandKind` is pinned at the five, and two `Source/StratBridge/Tests/` files
//         dispatch `SubmitCapture` in log replay.
//     (c) `strat::captureTick` is NOT IDEMPOTENT -- it does `turnsHeld += 1` per call and
//         `applyCommand`'s Capture arm never reads `c.unitId` -- so a key bound to it would
//         let a player advance a capture once per press. That is the reason never to wire
//         one, and it is a stronger statement than "no phase owns it".
//   RETRACTED IN PLACE, because a reader who remembers this as owed work needs to see the
//   withdrawal and not a silent absence.

// - AND THE HEX THE MENU OPENS FOR IS THE ONE UNDER THE CURSOR, NOT A SELECTED ONE, BECAUSE
//   NOTHING IN THIS PROJECT SELECTS A HEX. The ruling was phrased "the currently selected
//   hex", and the tree does not have one to read: `FStratSelectionMachine` holds
//   `SelectedUnitId` and nothing else spatial, and its `HexPrimary` arm treats a click on
//   empty ground with nothing selected as "an ordinary click and not a failure" -- so a
//   factory hex with no unit standing on it cannot be selected at all, and a factory hex
//   with one standing on it is not a hex you can build at. An accessor over the machine's
//   state would therefore have had to ADD a hex-selection concept to the machine, which is
//   precisely what the ruling was shaped to avoid.
//   `HexUnderCursor` is the same source `OnSelect` already uses to decide which hex a click
//   means, so the menu opens on the hex the player is pointing at when they press the key.
//   IT IS LATCHED AT THE MOMENT THE KEY FIRES AND NEVER RE-READ. See
//   `GetProductionTargetHex`.
// - SAVE / LOAD, AND THIS BULLET IS RETRACTED IN PLACE. It used to read:
//   RETRACTED> "`SerializeRecordedSave` still has no engine-side caller and the save-slot
//   RETRACTED>  UI is out of the milestone."
//   Wave B1 gave it one, and the caller is NOT this class:
//   `UStratMatchSubsystem::SaveMatchToSlot` / `LoadMatchFromSlot` own the slot, because a
//   PlayerController is per-player and client-local while there is exactly one match to
//   save. What is still not here is a KEY BINDING for either, and that is deliberate --
//   binding save to a key without a confirmation surface is how a player overwrites a slot
//   by leaning on the keyboard. The condition that discharges it: a save-slot UI to bind to.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "StratGuidedOpening.h"
#include "StratHoverState.h"
#include "StratSelectionMachine.h"

#include "StratPlayerController.generated.h"

// Forward declarations only, per the header block.
class FStratBridge;

class AStratBoardActor;
class UInputAction;
class UInputMappingContext;
class UStratMatchSubsystem;

/**
 * Turns clicks and keys into §4.9 commands, through `FStratSelectionMachine` and
 * `FStratBridge` and through nothing else.
 *
 * A map gets one by pointing a GameMode Blueprint at a subclass of this class. The hot-seat
 * milestone's phase 5 authored that Blueprint and the input assets it carries; an unset
 * action property runs inert and says so once, by name.
 *
 * [AMENDED, wave 0: "Phase 5 authors that Blueprint and the five input assets it carries;
 * until it does, this class runs inert and says so once." Both halves were wrong and only one
 * of them was a count. The COUNT was stale by an action; the TENSE described phase 5 as
 * pending, which it has not been since the hot-seat milestone closed; and "runs inert" was a
 * claim about the whole class where the truth is per-property. Wave 0's first draft rewrote
 * all three silently -- the only site in that pass corrected without a quote, which the gate
 * caught and which is why the quote is here now.]
 */
UCLASS(Blueprintable, meta = (DisplayName = "Strat Player Controller"))
class STRATPLAY_API AStratPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AStratPlayerController();

	/**
	 * Rebuilds the model, decorates it with the presentation bits, reconciles, and repaints
	 * the overlays. THE ONE REFRESH PATH IN THIS CLASS.
	 *
	 * [AMENDED 2026-08-23: this said "the two overlays". This path repaints THREE as of that
	 * date, and it does so by two different routes -- this class calls `ShowReach` and
	 * `ShowTargets` itself, while §2.11.6-B's ring is repainted inside the `ApplyView` step
	 * below without this class naming it. The step list further down is accurate for what
	 * THIS class does and is deliberately left alone; only the count is removed.]
	 *
	 * PUBLIC because the hand-over and the debug console both want it, and because a single
	 * named refresh is what stops a future caller from writing a second, subtly different
	 * sequence. The sequence is build -> `DecorateViewModel` -> `ApplyView` ->
	 * `BuildOverlays` -> `ShowReach` / `ShowTargets`, and every step of it is somebody
	 * else's method.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Input")
	bool RefreshFromMachine(FString& OutFailureReason);

	/**
	 * Writes this controller's two presentation layers onto a model somebody else built:
	 * `FStratSelectionMachine`'s `bDone` / `bLockedThisTurn`, and Sec 2.11.6's guidance
	 * block.
	 *
	 * WHAT GAP THIS CLOSES. This sequence used to live inline inside `RefreshFromMachine`,
	 * which made it reachable ONLY from a refresh this controller started -- so the two
	 * reconciles `UStratMatchSubsystem` starts for itself (`RefreshPresentation`, and
	 * `StartMatchInternal`'s first) applied models with an empty guidance block, and the
	 * strip cleared. Measured in PIE on 2026-08-21 through `SetViewingSide`; see
	 * `FStratViewDecorator`. Lifting it into a named method and registering it as that
	 * delegate is what makes the completion a property of the MODEL rather than of the
	 * caller.
	 *
	 * IT IS NOT A REFRESH AND MUST NOT BECOME ONE. It does not build, does not apply, does
	 * not touch the overlays and asks the subsystem for nothing -- which is what lets the
	 * subsystem call it from inside its own build without re-entering this class's refresh
	 * path. `RefreshFromMachine` calls it too, so there is one implementation of "what this
	 * controller adds to a model" and not two.
	 *
	 * IT ADVANCES THE BEATS, and that is deliberate rather than a side effect that leaked
	 * out. `FStratGuidedOpening::Observe` is documented as the only thing that advances a
	 * beat, and it belongs with every model that will be DRAWN -- including the ones drawn
	 * after an AI turn, which this controller never asked for and which are exactly where
	 * beat 1b's "the enemy turn ends" becomes observable. `Observe` is a function of the
	 * model plus the machine's own memory, so being called twice for one board state
	 * retires nothing twice: the turn clock only moves on a new `Turn`, and retirement is
	 * re-read off the model each time.
	 *
	 * NOT A `UFUNCTION`. It takes `FStratViewModel&` by non-const reference to write into,
	 * and a reflected version would hand Blueprint an authoring seat at the model -- the
	 * second-author failure `UStratGuidanceWidget::PushGuidance` declines reflection to
	 * prevent, arrived at from the other end.
	 */
	void DecorateForPresentation(FStratViewModel& Model);

	/**
	 * Feeds one event to the selection machine, submits whatever it asks for, and refreshes.
	 *
	 * PUBLIC AND NOT PROTECTED, so that a hand-over screen, a debug command or a gate can
	 * drive the same path a key does. The `EStratSelectionEvent` handlers below are one-line
	 * calls to this. [AMENDED, wave 0: "The Enhanced Input handlers below are four one-line
	 * calls to this." -- a count of a growing set, and
	 * the set has grown twice since. Not every Enhanced Input handler reaches this method:
	 * `OnToggleProductionMenu` and `OnHover` deliberately do not, and each says so.]
	 * [AMENDED 2026-08-27: `OnHover` no longer exists -- the hover is polled on `Tick`, which
	 * likewise does not reach this method. The claim is unchanged in substance and only its
	 * example moved.]
	 *
	 * THE ORDER IS THE CONTRACT: decide, submit, and only then tell the machine the command
	 * applied. `FStratSelectionMachine::NotifyCommandApplied` is documented "call it only on
	 * acceptance", and calling it before the bridge answered would advance the machine past
	 * a command the rules module refused.
	 *
	 * NOT A `UFUNCTION`, and the reason is worth recording because the fix looks trivial:
	 * `EStratSelectionEvent` is a plain `enum class` and UHT will not accept one, so a
	 * Blueprint-callable version would have to take a `uint8` and cast. That is an untyped
	 * entry point into a state machine, and the only thing it buys is a node nothing in this
	 * milestone needs. `RefreshFromMachine` above IS reflected, because it takes no enum.
	 */
	bool HandleSelectionEvent(EStratSelectionEvent Event, FIntPoint Hex, FString& OutFailureReason);

	/**
	 * Which hex the cursor is over, if any.
	 *
	 * AN INSTANCE-INDEX LOOKUP, NEVER AN INVERSE OF `WorldLocationOfHex`. See the header
	 * block, and `AStratBoardActor`'s, on why no inverse exists to call.
	 *
	 * FALSE IS AN ORDINARY ANSWER -- the cursor is off the board, over a unit's own mesh, or
	 * over nothing. It is not a fault and is not logged.
	 *
	 * NON-CONST DESPITE BEING A QUERY, because `APlayerController::GetHitResultUnderCursor`
	 * is non-const. Written honestly rather than with a `const_cast` behind a const
	 * signature: the cast would be a claim about the engine's method that this file is in no
	 * position to make.
	 */
	bool HexUnderCursor(FIntPoint& OutHex);

	// ---- §2.11.3 / §2.11.2's hover, wave 0 ---------------------------------
	// THIS CLASS RESOLVES THE HOVER AND DOES NOT STORE IT, exactly as it drives the selection
	// machine without storing what is selected. `FStratHoverState` holds the hex; this class
	// owns only the two things that need a world -- the cursor, and the decision to refresh.
	//
	// THE HOVER IS THE INPUT AND THE STRINGS ARE NOT. `FStratGuidanceView::EndTurnGateHover`
	// and `LockedUnitHover` are shipped tooltip text and were never a hover input; nothing
	// below reads either, and nothing below writes them.

	/**
	 * Re-resolves the hovered hex from the cursor and refreshes if it moved.
	 *
	 * THE ONE HOVER PATH IN THIS CLASS, and `Tick` is a one-line call to it. [AMENDED
	 * 2026-08-27: "and `OnHover` is a one-line call to it". The invariant -- one hover path,
	 * whatever drives it -- is what the sentence was for and is unchanged; the driver is what
	 * moved, from an Enhanced Input handler that was measured never to fire to a tick poll.
	 * See `Tick`'s definition for the measurement.] PUBLIC for
	 * `RefreshFromMachine`'s reason -- a hand-over, a debug command or a gate should be able
	 * to drive the same path the mouse does -- and because a hover that could only be produced
	 * by a live cursor over a live viewport is a hover no clause can reach.
	 *
	 * IT REUSES `HexUnderCursor` AND ADDS NO SECOND TRACE. There is one cursor-to-hex route in
	 * this project and `SelectAction` already uses it; a hover-specific trace would be a
	 * second author of the same answer.
	 *
	 * IT REFRESHES ONLY ON A CHANGE. `FStratHoverState`'s setters return whether anything
	 * moved and this method believes them -- a mouse-move action triggers every frame the
	 * cursor moves, and rebuilding the model at that rate for an identical state is the cost
	 * that shape was written to avoid.
	 *
	 * @return true if the hovered hex changed. A refresh REFUSAL does not change this answer:
	 *         the hover moved either way, and reporting otherwise would make the caller's
	 *         return value mean two different things. A refusal is logged, as elsewhere here.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Input")
	bool UpdateHoverFromCursor();

	/**
	 * Sets the hovered hex directly, without a cursor.
	 *
	 * THE HEADLESS SEAM, AND IT IS STATED AS ONE RATHER THAN LEFT TO BE DISCOVERED. A clause
	 * under `T-UI-01` must be able to assert that a hovered hex reaches the model AND WHICH
	 * HEX, and `GetHitResultUnderCursor` needs a viewport, a local player and a cursor
	 * position that no automation test has. This method is the same path from the hex onward
	 * -- the identical `FStratHoverState` call and the identical refresh decision -- with only
	 * the trace removed.
	 *
	 * IT IS NOT A TEST-ONLY BACK DOOR AND MUST NOT BECOME ONE. It sets no state `HexUnderCursor`
	 * could not have produced, and a caller passing a hex that is not on the board gets a model
	 * naming a hex that is not on the board -- which is what it asked for. If a future caller
	 * needs that refused, the refusal belongs in `AStratBoardActor`, which is the only object
	 * that knows the board's extent.
	 *
	 * @return true if the hovered hex changed, on `UpdateHoverFromCursor`'s terms exactly.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Input")
	bool SetHoveredHex(FIntPoint Hex);

	/**
	 * Records that nothing is hovered, and refreshes if that is news.
	 *
	 * PUBLIC BECAUSE THE CURSOR IS NOT THE ONLY WAY A HOVER ENDS. A hand-over, a match result,
	 * or a modal production menu all leave the board unhovered while the mouse has not moved,
	 * so the last mouse-move event cannot be relied on to have cleared it.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Input")
	bool ClearHoveredHex();

	/**
	 * The hovered hex, if any.
	 *
	 * A READ AND NOT THE SCREEN'S SOURCE. Nothing drawn may read this: `T-INT-05` holds only
	 * while the screen is a function of the view model alone, and `FStratViewModel::Hover` is
	 * where a widget looks. This accessor exists so a clause can assert the controller's own
	 * state beside the model's, and so a hand-over can ask.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Input")
	bool GetHoveredHex(FIntPoint& OutHex) const;

	/** The hover state, so a clause or a hand-over can drive and read it whole. */
	FStratHoverState&       GetHoverState()       { return Hover; }
	const FStratHoverState& GetHoverState() const { return Hover; }

	/**
	 * The machine, so a hand-over or a gate can read what is selected.
	 *
	 * NON-CONST BECAUSE THE GUIDANCE LAYER WRITES THE LOCKS, and that clause used to read:
	 * RETRACTED> "because §2.11.6's guidance layer will need `SetLockedThisTurn` when it
	 * RETRACTED>  exists."
	 * It exists: `FStratGuidedOpening::Observe` takes this reference and calls
	 * `SetLockedThisTurn` on it once per refresh. Retracted in place rather than deleted,
	 * because the forward-looking version is the one a reader will remember.
	 */
	FStratSelectionMachine& GetSelectionMachine() { return SelectionMachine; }
	const FStratSelectionMachine& GetSelectionMachine() const { return SelectionMachine; }

	// ---- §2.11.6's guided opening -----------------------------------------
	// THIS CLASS DRIVES IT AND DOES NOT IMPLEMENT IT, exactly as it drives the selection
	// machine. `FStratGuidedOpening` owns every beat decision; what lives here is the three
	// input gates, because §2.11.6-B calls beat 1a's End Turn constraint "the only
	// guided-opening constraint that gates a player INPUT rather than a selection, adopted
	// under Q27 (§4.7), ruled" — and input is this class's subject.
	//
	// THE GATES REFUSE BY NOT ASKING, AND NEVER BY ASKING FOR A REFUSAL. No gate calls
	// `FStratBridge`; a gated event returns before `FStratSelectionMachine::HandleEvent`
	// (so no machine state moves) or before `StratSubmitSelectionCommand` (so no command is
	// submitted). Nothing in this path can make the rules module refuse a command it would
	// otherwise accept, which is the same footing §2.11.1's machine-narrower-than-the-rule
	// note puts the SELECTED → attack restriction on.

	/** The layer, so a clause can drive it and a hand-over can read it. */
	FStratGuidedOpening& GetGuidedOpening() { return GuidedOpening; }
	const FStratGuidedOpening& GetGuidedOpening() const { return GuidedOpening; }

	/**
	 * §2.11.6's `Skip guidance` control, for the button that will eventually call it.
	 *
	 * A `UFUNCTION` WHERE `GetGuidedOpening` IS NOT, because this is the one member of the
	 * surface a widget needs and `FStratGuidedOpening` is not a reflected type. Exposing the
	 * object to Blueprint would mean making it one; exposing the single verb costs nothing.
	 *
	 * IT REFRESHES. §2.11.6 requires the ring, the marker and the strip to clear "in the
	 * same frame as the strip", and the model is only rewritten by a refresh — so the skip
	 * that did not refresh would be a skip the player sees next click.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Guidance")
	void SkipGuidance();

	/** Whether the strip is on screen. False after a skip, a suppression, or the window closing. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Guidance")
	bool IsGuidanceActive() const;

	// ---- §2.11.5's production menu ----------------------------------------
	// THIS CLASS DECIDES WHICH HEX AND NOTHING ELSE ABOUT PRODUCTION. It does not ask what
	// can be built, does not read `bAffordable` or `bAvailable`, does not submit a choice
	// and does not call the widget's `RefreshMenu` -- which is a Blueprint custom event it
	// has no C++ name for. The rows are `UStratMatchSubsystem::RefreshProductionMenu`'s, the
	// submit is `SubmitProductionChoice`'s, the widget's lifetime is
	// `AStratScoreboardHUD`'s, and the only thing left over is "which hex did the player
	// mean", which is a question about input and therefore this class's.

	/**
	 * The hex §2.11.5's menu was opened for, if one is open.
	 *
	 * WHAT GAP THIS CLOSES. `WBP_ProductionMenu` refreshes itself from its own `Construct`
	 * and needs the factory hex to do it; before this there was no reflected route from the
	 * input path to a widget graph, and `GetSelectionMachine()` is deliberately not a
	 * `UFUNCTION` (`FStratSelectionMachine` is not a reflected type, for the two reasons its
	 * own header gives, and neither of them has expired).
	 *
	 * IT IS A LATCH AND NOT A LIVE CURSOR READ, AND THAT IS THE ONE DECISION IN IT.
	 * `OnToggleProductionMenu` records `HexUnderCursor` at the instant the key fires; this
	 * hands back what was recorded. A live read would answer a DIFFERENT hex the moment the
	 * player moved the mouse off the factory -- and the widget's `Construct` runs inside
	 * `AddToViewport`, one mouse-movement later than the keypress in the worst case -- so a
	 * menu could be built for a factory the player never pointed at. What the latch costs is
	 * that it is a piece of input history held on an actor; what it buys is that "which
	 * factory is this menu about" has one answer for the life of the menu.
	 *
	 * IT IS NOT A PRESENTATION BIT AND T-INT-05 DOES NOT REACH IT. That clause forbids an
	 * actor holding `bDone` / `bLockedThisTurn` -- fields OF THE VIEW MODEL, which
	 * `DecorateViewModel` produces and which would drift if copied. This is an input intent,
	 * appears in no view model, and nothing on screen is drawn from it: the rows the menu
	 * draws come from `UStratMatchSubsystem::ProductionMenu`, and that hex is separately
	 * published as `ProductionMenuHex` by the refresh this value only ever STARTS.
	 *
	 * TWO CHANNELS, ON THIS CODEBASE'S STANDING HABIT. `FIntPoint(0, 0)` is a real hex on
	 * this board, so the hex cannot signal its own absence -- the same trap
	 * `UStratMatchSubsystem::IsProductionMenuOpen` records about `ProductionMenuHex` and the
	 * same one `FStratMatchConfig::SaveSlotName` has already been paid for once.
	 *
	 * @return false when no menu has been opened this session, or the last one was closed.
	 *         `OutHex` is left at (0, 0) and means nothing.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Production")
	bool GetProductionTargetHex(FIntPoint& OutHex) const;

	/**
	 * Opens §2.11.5's menu on the hex under the cursor, or closes the open one.
	 *
	 * A TOGGLE AND NOT AN OPEN, because the action that opens the menu is the only key
	 * bound to it and a player who pressed it by mistake needs a way back that is not
	 * "find the Cancel button". `CancelAction` is deliberately NOT wired to this: cancel
	 * means drop the selection, and overloading it would make one key's meaning depend on
	 * whether a panel happens to be up.
	 *
	 * IT ASKS NO RULES QUESTION AND MAKES NO LEGALITY CHECK. In particular it does not test
	 * whether the hex is a factory, whether this side holds it, or whether the side can pay
	 * -- all three are rows on the menu the widget is about to build, with the rules
	 * module's own reasons attached. `UStratMatchSubsystem::RefreshProductionMenu` is
	 * documented to SUCCEED on a hex that is not a build point, drawing a full menu of
	 * unavailable rows, and §2.11.5 draws exactly that. A pre-check here would replace the
	 * module's reason with this class's silence.
	 *
	 * THE ONE THING IT REFUSES IS A CURSOR THAT IS NOT ON THE BOARD, which is not a rules
	 * answer -- there is no hex to be about.
	 *
	 * PUBLIC AND `BlueprintCallable` so a menu button, a console command or a gate can
	 * drive the same path the key does; the Enhanced Input handler is one call to this.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Production")
	bool ToggleProductionMenu(FString& OutFailureReason);

protected:
	// ---- Enhanced Input assets. Null as C++ DEFAULTS; set on the `BP_` subclass. ------
	// EVERY ONE IS AN `EditDefaultsOnly` `TObjectPtr` AND NOT A PATH. The project forbids a
	// `/Game/` literal in gameplay C++, with two standing exceptions (automation fixtures
	// and the import commandlet) that this file is neither of. A null asset is handled
	// everywhere below: no context is added, no action is bound, and the controller logs
	// once rather than per frame.

	/**
	 * The §2.11.1 mapping context, added on `BeginPlay` at `MappingPriority`.
	 *
	 * NULL IS SUPPORTED. `BeginPlay` skips the add and says so at Warning -- once, because a
	 * missing input asset is a configuration gap in whatever Blueprint the map's GameMode
	 * names, and not a fault that repeats. (The shipping `BP_` subclass does set it; this
	 * arm is what keeps a map that forgot to from being a crash instead of a warning.)
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Input")
	TObjectPtr<UInputMappingContext> SelectionMappingContext;

	/**
	 * Priority for the context above.
	 *
	 * A PROPERTY RATHER THAN A LITERAL because this game will eventually stack a hand-over
	 * screen's context over this one, and the ordering between them is a design question
	 * that must not be settled by a magic number in a C++ file.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Input")
	int32 MappingPriority = 0;

	/** Primary click on a hex: select, move or attack. `EStratSelectionEvent::HexPrimary`. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Input")
	TObjectPtr<UInputAction> SelectAction;

	/** Secondary click or Escape: drop the selection. `EStratSelectionEvent::Cancel`. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Input")
	TObjectPtr<UInputAction> CancelAction;

	/**
	 * §2.11.1's wait. `EStratSelectionEvent::Wait`.
	 *
	 * IT SUBMITS NO COMMAND, and that is not an omission. `strat::SaveCommandKind` has no
	 * `Wait` -- phase 1 recorded the correction -- so a wait is the selection machine
	 * marking a unit DONE and nothing reaching the rules module at all.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Input")
	TObjectPtr<UInputAction> WaitAction;

	/** End the active side's turn. `EStratSelectionEvent::EndTurn`, and the one event that
	 *  is valid with nothing selected. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Input")
	TObjectPtr<UInputAction> EndTurnAction;

	/**
	 * §2.11.5's production menu: open it on the hex under the cursor, or close it.
	 *
	 * A SEPARATE ACTION RATHER THAN ANOTHER MEANING FOR `SelectAction`, and that is the user's
	 * ruling of 2026-08-22 rather than this file's preference. [AMENDED, wave 0: "A FIFTH
	 * ACTION RATHER THAN A FIFTH MEANING FOR `SelectAction`". The ruling is unchanged; only
	 * the two ordinals are, and the contrast the sentence is actually making -- a separate
	 * action versus another meaning for an existing one -- never needed either of them.] A primary click on a factory
	 * hex already means something -- select, move to, or attack whatever stands there -- and
	 * `FStratSelectionMachine::HandleEvent` decides which. Adding a BUILD arm there would put
	 * a §2.11.5 question inside the §2.11.1 state machine and make one click's meaning depend
	 * on the terrain under it.
	 *
	 * IT MAPS TO NO `EStratSelectionEvent`. Unlike the selection actions above, it does not reach
	 * `HandleSelectionEvent` at all -- and neither does the hover, which is polled on `Tick`.
	 * [AMENDED, wave 0: "Unlike
	 * the four above". A census, and one that silently required this property to stay the fifth
	 * declaration in the file. AMENDED AGAIN 2026-08-27: "nor does `HoverAction` below" -- that
	 * property was removed with the dead Enhanced Input hover route, so the sentence named a
	 * declaration that is no longer in the file. The claim it makes is unaffected.] It starts
	 * no command, advances no selection and submits
	 * nothing. `OnToggleProductionMenu` calls `ToggleProductionMenu` and that is the whole
	 * path.
	 *
	 * NULL IS SUPPORTED exactly as the actions above are: no binding, one Warning naming this
	 * property, and a match that is otherwise fully playable -- the production surface is
	 * still reachable from a console or a gate through `UStratMatchSubsystem`'s three
	 * reflected `Stratocracy|Production` entry points. [AMENDED, wave 0: "exactly as the four
	 * above are".]
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Input")
	TObjectPtr<UInputAction> OpenProductionMenuAction;

	// THERE IS NO `HoverAction` PROPERTY, AND IT WAS REMOVED RATHER THAN LEFT UNBOUND.
	// 2026-08-27. Wave 0 declared one here -- a 2D-axis action over mouse XY, bound on
	// `ETriggerEvent::Triggered` -- and its `IA_Hover` asset, its mapping-context row and its
	// Blueprint default were all authored and byte-verified. A human playtest on an
	// instrumented build then measured the handler running EXACTLY ONCE in three and a half
	// minutes, on a focus transition, while the cursor swept the board unregistered. The
	// measurement and the control that makes it a measurement are recorded on
	// `AStratPlayerController::Tick` in the `.cpp`, which is the route that replaced it.
	//
	// THE PROPERTY IS GONE BECAUSE A WIRED-AND-DEAD ASSET REFERENCE IS WORSE THAN NO
	// REFERENCE. Left in place it would read to the next reader as the hover mechanism --
	// `EditDefaultsOnly`, non-null on the shipping Blueprint, bound without a warning -- and
	// send anyone debugging the hover to the trigger event rather than to the tick. This
	// project has a recorded incident of the mirror-image defect, a correct reflected verb no
	// player could reach; the lesson taken from it was to check the ROUTE, and the route here
	// is `Tick`.
	//
	// WHAT THIS LEFT BEHIND IN `Content/`, AND IT HAS SINCE BEEN CLEARED. Removing the property
	// orphaned three things: the `IA_Hover` asset, its row in `IMC_Selection`, and the
	// Blueprint default on `BP_StratPlayerController` that pointed at `HoverAction`. This block
	// originally said that was a content-lane cleanup, not this file's, and that nothing broke
	// if it was never done. Both halves were true; the cleanup was then done in the same
	// working tree, before either change was committed -- `IA_Hover` deleted, the
	// `IMC_Selection` row removed, and the Blueprint re-saved so the default is DROPPED from
	// the package rather than merely ignored as an unknown property on load.
	//
	// THE PARAGRAPH IS AMENDED IN PLACE RATHER THAN DELETED, because what it records is not
	// the outstanding work -- that is finished -- but WHY those three assets became dead at
	// all. An asset deleted with no surviving explanation is the kind of removal that gets
	// undone by the next reader who finds a mapping context with a gap in it. The explanation
	// is the paragraph above this one: the trigger event was measured never to fire, and the
	// route is `Tick`.
	//
	// THIS FILE STATES NO CURRENT `Content/` FACT AND SHOULD NOT ACQUIRE ONE. The lines above
	// are history with a date on them, not a description of the working tree; `Content/` is
	// the content lane's and its live state is `content.md`'s to record. A prose block in a
	// header cannot be kept true about assets it does not own, which is exactly how the
	// sentence being amended here went stale within a day of being written.

	// ---- Lifetime ---------------------------------------------------------

	/**
	 * Adds the mapping context, registers this controller as the match's view decorator, and
	 * paints the first frame's overlays.
	 *
	 * THE REGISTRATION HAPPENS BEFORE THE FIRST REFRESH AND MAKES NO CLAIM ABOUT WHEN THE
	 * MATCH STARTED. `UStratMatchSubsystem` is a world subsystem and exists before any actor
	 * runs `BeginPlay`, so the binding lands whether or not `AStratGameMode::BeginPlay` has
	 * seeded anything yet -- which is the property that keeps the fix from depending on actor
	 * `BeginPlay` order. A binding taken before the match starts simply decorates nothing
	 * until there is something to decorate.
	 */
	virtual void BeginPlay() override;

	/**
	 * Polls the cursor for §2.11.3 / §2.11.2's hover. One call to `UpdateHoverFromCursor`.
	 *
	 * THE ONLY POLLING IN THIS CLASS, AND IT EXISTS BECAUSE THE EVENT ROUTE WAS MEASURED
	 * DEAD. Everything else here runs from an input trigger. The measurement that put this
	 * method in the file -- a human playtest in which the Enhanced Input hover handler ran
	 * once in three and a half minutes while the cursor swept the board -- is recorded in
	 * full on the definition in the `.cpp`, together with the input-mode route that was
	 * rejected and why. Read that block before replacing this with an action binding.
	 *
	 * A TRACE PER FRAME AND NOT A MODEL REBUILD PER FRAME. `HexUnderCursor` returns before
	 * tracing when there is no board; when there is one, `FStratHoverState`'s setters report
	 * whether the resolved hex actually moved and `ApplyHoverChange` refreshes only then. The
	 * de-duplication is not new and is not assumed here -- see `StratHoverState.cpp`.
	 *
	 * IT DISCARDS THE RETURN VALUE, and it is the only caller that does. See the definition.
	 */
	virtual void Tick(float DeltaSeconds) override;

	/**
	 * Releases the view-decorator registration.
	 *
	 * NOT STRICTLY REQUIRED FOR SAFETY, and that is worth saying so the next reader does not
	 * take it for one: the delegate is bound with `CreateUObject` and therefore holds a weak
	 * reference, so a destroyed controller stops decorating on its own. It is released here
	 * anyway because the subsystem outlives this actor within one world, and a subsystem
	 * reporting `HasViewDecorator()` true while pointing at a dead controller is a fact that
	 * will one day be read as evidence.
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Arms §2.11.6's guided opening once, the first time a seeded match is reachable.
	 *
	 * ONCE, AND LAZILY. `BeginPlay` ordering between a PlayerController and a GameMode's
	 * `StartMatch` is not something this file may assume — so rather than asserting an
	 * order, this is called from the refresh path and returns immediately once armed. The
	 * cost is one bool test per refresh; the alternative is a guided opening that silently
	 * does not run because two `BeginPlay`s landed the other way round on someone's machine.
	 *
	 * THE SUPPRESSION BOOL COMES FROM THE SAVE SLOT, through
	 * `UStratMatchSubsystem::HasCompletedAMatchOnSave`. §2.11.6: "Any completed match on the
	 * save skips all guidance automatically." Read here rather than inside
	 * `FStratGuidedOpening`, which owns no disk and must stay drivable with no slot present.
	 */
	void TryArmGuidedOpening();

	/**
	 * Binds this class's actions on the Enhanced Input component, each behind its own null
	 * guard. [AMENDED, wave 0: "the four actions".]
	 *
	 * IT REFUSES QUIETLY AND COMPLETELY IF THE COMPONENT IS NOT AN ENHANCED ONE, which
	 * happens when the project's default input component class has been changed out from
	 * under this module. Logged at Error, because unlike a null action asset that is a
	 * project-settings fault with no configuration that fixes it here.
	 */
	virtual void SetupInputComponent() override;

private:
	/** The match subsystem of this controller's world, or null outside a Game/PIE world. */
	UStratMatchSubsystem* GetMatch() const;

	/** The spawned board, or null. Read through the subsystem so there is one owner of it. */
	AStratBoardActor* GetBoard() const;

	// The `EStratSelectionEvent` handlers. Each is one call to `HandleSelectionEvent`; the
	// only one that reads the cursor is `OnSelect`, because the others are statements about
	// the selection rather than about a hex. [AMENDED, wave 0: "because the other three are
	// statements about the selection rather than about a hex", now "because the others are";
	// and "The four Enhanced Input
	// handlers" -- there are more Enhanced Input handlers than these, and two of them
	// (`OnToggleProductionMenu`, `OnHover`) do not call `HandleSelectionEvent` at all, so the
	// old sentence was mis-describing the group as well as miscounting it.
	// AMENDED AGAIN 2026-08-27: `OnHover` is gone -- the hover is polled on `Tick` now -- so
	// `OnToggleProductionMenu` is once more the only Enhanced Input handler that skips
	// `HandleSelectionEvent`. The 2026-08-27 pass deliberately did NOT rewrite the sentence to
	// say "one of them", because that is the count this note exists to warn against writing.]
	void OnSelect();
	void OnCancel();
	void OnWait();
	void OnEndTurn();

	/**
	 * §2.11.5's production-menu handler. One call to `ToggleProductionMenu`, and it does not go
	 * through `HandleSelectionEvent` at all -- and neither does the tick-polled hover.
	 * [AMENDED 2026-08-27: "nor does `OnHover`", a handler that no longer exists.]
	 *
	 * [AMENDED, wave 0: "§2.11.5's fifth handler. One call to `ToggleProductionMenu`, and the
	 * only one of the five that does not go through `HandleSelectionEvent`." A count of a
	 * growing set, and by the end of wave 0's own pass it was contradicted by wave 0's own
	 * added text a few lines above, which names `OnHover` as a second such handler. Two
	 * comments in one block disagreeing about the same fact is worse than either being stale.
	 * The `OnHover` half of that history ended on 2026-08-27 with the handler itself; the
	 * warning about counting a moving set is why this note is not being rewritten to a number.]
	 */
	void OnToggleProductionMenu();

	/**
	 * The one place a hover decides whether to refresh.
	 *
	 * TAKES THE ANSWER RATHER THAN COMPUTING IT. `FStratHoverState`'s setters already know
	 * whether anything moved, and re-deriving it here would need a copy of the previous hex on
	 * this actor -- which is the copy T-INT-05 forbids, reintroduced for a bookkeeping reason.
	 *
	 * PRIVATE, because it is a step and not an entry point: every public hover method above
	 * ends here, and a caller reaching this directly would be asserting a change it had not
	 * made.
	 *
	 * @param bChanged  whether the hovered hex moved.
	 * @return `bChanged`, unmodified, so the public methods can return it straight.
	 */
	bool ApplyHoverChange(bool bChanged);

	/**
	 * The latched target hex and whether anything is latched. See `GetProductionTargetHex`.
	 *
	 * TWO MEMBERS AND NOT ONE, because `FIntPoint(0, 0)` is a real hex and cannot mean
	 * "none". NOT `UPROPERTY`s: `FIntPoint` and `bool` hold nothing for the garbage
	 * collector, and publishing them would give a Widget Blueprint a second place to read
	 * the target from -- one that could be WRITTEN, which would make "which factory is this
	 * menu about" a question with two authors. The reflected route is the accessor.
	 */
	FIntPoint ProductionTargetHex = FIntPoint::ZeroValue;
	bool      bHasProductionTargetHex = false;

	/**
	 * §2.11.1's selection state and the producer of `bDone` / `bLockedThisTurn`.
	 *
	 * BY VALUE, NOT A `UPROPERTY`, AND THAT IS THE POINT. See the header block: a reflected
	 * copy of the presentation bits on an actor is exactly what T-INT-05 forbids, and this
	 * member cannot become one because `FStratSelectionMachine` is not a reflected type.
	 */
	FStratSelectionMachine SelectionMachine;

	/**
	 * §2.11.3 / §2.11.2's hovered hex and the producer of `FStratHoverView`.
	 *
	 * BY VALUE AND BESIDE THE MACHINE, for the machine's reasons exactly and for one of its
	 * own: `FStratHoverState` is not a reflected type, so this member cannot become the
	 * reflected copy of a presentation bit on an actor that T-INT-05 forbids -- and the
	 * hovered hex is a presentation bit in the fullest sense, since the only thing that reads
	 * it is a card that appears because of it.
	 *
	 * NOT ARMED, NOT RESET AT BEGINPLAY, AND HOLDING NOTHING ACROSS A RESEED. Its default is
	 * "not hovering", which is the truth for a controller whose cursor has not yet moved, so
	 * unlike `GuidedOpening` there is nothing here to arm and no bridge to wait for.
	 */
	FStratHoverState Hover;

	/**
	 * §2.11.6's guided opening. BY VALUE AND BESIDE THE MACHINE, for the machine's reasons.
	 *
	 * ARMED IN `BeginPlay` AND NOT IN THE CONSTRUCTOR, because arming needs a seeded bridge
	 * (for `GuidedOpeningHexes`) and the save slot (for the suppression bool), and neither
	 * exists when a controller is constructed. A controller whose `BeginPlay` runs before
	 * `StartMatch` finds no bridge and leaves guidance inactive, which is the same
	 * inactive state a scenario with no `guidedOpening` produces — see `TryArmGuidedOpening`.
	 */
	FStratGuidedOpening GuidedOpening;

	/**
	 * Whether `TryArmGuidedOpening` has succeeded, so it is not retried every refresh.
	 *
	 * IT IS NOT A MIRROR OF `GuidedOpening.IsActive()` and the two differ in the case that
	 * matters: an armed-then-skipped layer is armed and inactive, and re-arming it would
	 * undo the skip §2.11.6 calls permanent. This records a fact about THIS OBJECT'S OWN
	 * call history, which is the same justification `bAiTurnRunning` carries on the
	 * subsystem.
	 */
	bool bGuidanceArmed = false;

	/** Whether the "no input assets" warning has been said. Said once, not per click. */
	bool bReportedMissingInputAssets = false;
};
