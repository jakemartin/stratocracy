// GDD §2.11.5 / §2.11.2 / T-UI-03 -- which factory the player is focused on, as a value, and
// the producer of `FStratCommandBarView`'s BUILD half.
//
// WHAT GAP THIS CLOSES. After playing the prototype the player reported that no on-screen
// control opens §2.11.5's production menu -- the only route is the `B` key, and nothing on
// screen says the key exists. A BUILD button needs to know WHICH FACTORY it is about, and
// MEASURED on the tree at `fcf64d3`, NOTHING IN THIS PROJECT SELECTS A HEX:
// `FStratSelectionMachine` tracks `SelectedUnitId` and its own header says so in terms. The
// existing route around that -- `AStratPlayerController::ToggleProductionMenu` reading
// `HexUnderCursor` at the instant the key fires -- CANNOT SERVE A BUTTON, because a button
// click puts the cursor on the button and off the board. That is not a defect in the key
// path; it is the reason a second, cursor-free source of "which factory" has to exist, and
// this file is it.
//
// IT IS AN INPUT INTENT AND THE DRAWN BIT IS RECOMPUTED EVERY REFRESH, WHICH IS THE ONE IDEA
// THIS FILE CARRIES. This struct holds a hex and a bool. It does NOT hold "is the BUILD
// button visible". `StratDecorateCommandBar` looks the hex up in `Model.Factories` on every
// decorate and writes `bShowBuildButton = (found && Owner == ViewingSide)`. So a factory
// captured out from under the latch, a hot-seat hand-over, a match that concluded -- every
// one of them darkens the button on the next refresh with no clear-point code involved, and
// staleness is structurally impossible rather than dependent on remembering every clear
// point. That is the posture `FStratHoverState` already takes toward the board, and the one
// both `UStratMatchSubsystem::IsProductionMenuOpen` and
// `AStratScoreboardHUD::IsProductionMenuWidgetOpen` take in their own headers: a mirror can
// disagree with the thing it mirrors, so the mirror is not made.
//
// AND THAT IS WHAT SATISFIES T-INT-05 RATHER THAN A CONVENTION. `GetProductionTargetHex`'s
// declaration exempts itself from T-INT-05 on an explicit condition -- "appears in no view
// model, and NOTHING ON SCREEN IS DRAWN FROM IT". A BUILD button drawn from a focus latch
// does not inherit that exemption, because something on screen would then be drawn from an
// actor's member. So what the button draws is a FIELD ON THE VIEW MODEL,
// `FStratCommandBarView::bShowBuildButton`, and the only route from this struct to the screen
// is `DecorateViewModel`.
//
// IT IS A PLAIN STRUCT AND NOT A UCLASS, AND NOT AN ACTOR'S `UPROPERTY`, for
// `FStratHoverState`'s two reasons, which apply here without amendment:
//   1. TESTABILITY. A focus latch whose only observable route was a live mouse over a live
//      Slate viewport could not be pinned by a clause at all. This struct is drivable with
//      no actor, no world, no PIE and no cursor: `NoteHexPrimary` over a hand-built
//      `FStratViewModel`, then `DecorateViewModel`, is the entire seam. That is a stronger
//      claim than the key path can make --
//      `Source/StratPlay/Tests/StratProductionMenuAffordance.cpp`'s own header declares a
//      third of its subject untestable because `HexUnderCursor` returns false in every
//      headless state -- and it is a deliberate consequence of taking the hex and the model
//      as ARGUMENTS rather than resolving either.
//   2. THE T-INT-05 OBLIGATION, above.
//
// MEASURED BEFORE THIS FILE WAS WRITTEN, because the plan it was written from flagged it as
// an assumption to check rather than to trust:
// `Stratocracy.StratPlay.T-INT-05.NoActorHoldsPresentationBits`
// (`Source/StratPlay/Tests/StratMatchReconcile.cpp`) DOES NOT CENSUS CONTROLLER MEMBERS. It
// drives five decorations through `UStratMatchSubsystem::ApplyView` and compares
// `AStratUnitActor::GetLastAppliedView()` field by field against the applied model. Grepping
// the whole file for `PlayerController`, `TFieldIterator`, `FProperty` and `StaticClass()->`
// returns ZERO hits, so there is no reflected-property walk for a non-`UPROPERTY` member of
// `AStratPlayerController` to be caught by. IF THAT CLAUSE EVER GROWS ONE, this struct is the
// next thing it reaches, and the answer is the paragraph above rather than a change here.
//
// IT RESOLVES NOTHING AND ASKS THE RULES MODULE NOTHING. There is no cursor here, no
// `FHitResult`, no board pointer and no `FStratBridge`. Whether a hex holds a factory and who
// owns it are read off `FStratViewModel::Factories`, which the bridge already answered; this
// file performs a lookup, which is the same thing `FStratHoverView`'s block calls "a lookup
// and not arithmetic". There is no hex arithmetic in this file at all -- `FIntPoint::operator==`
// is the only spatial operator and it answers a question about two values this struct was
// handed.
//
// THE SETTERS RETURN WHETHER ANYTHING CHANGED, on `FStratHoverState`'s recorded rule and for
// a weaker version of its reason. No caller in this tree refreshes conditionally on this
// return today -- `HandleSelectionEvent` refreshes at the bottom regardless -- so the return
// is currently a testability affordance rather than a performance one, and that is stated
// here rather than left for a reader to discover the difference.
//
// WHAT CLEARS THE FOCUS, AND WHAT DELIBERATELY DOES NOT:
//   - A primary click on anything that is not a viewing-side factory: `NoteHexPrimary`'s else
//     arm. ONE ARM, BOTH DIRECTIONS, UNCONDITIONAL -- a latch and a clear are the same call
//     with a different model answer, so there is no path on which one runs and the other is
//     forgotten.
//   - Cancel (secondary click or Escape): `ClearFocus`, from `HandleSelectionEvent`.
//   - A turn change, INCLUDING EVERY AI TURN: `Observe`, on `LastObservedTurn`. That is
//     `FStratGuidedOpening::Observe`'s own turn-clock mechanism, and keying on the turn the
//     MODEL reports means a gated or refused end turn -- which advances nothing -- does not
//     clear, while an AI hand-over does.
//   - A hot-seat hand-over: `Observe`, on `LastObservedViewingSide`.
//   - A concluded match: `Observe` clears on `Model.Match.bHasResult`, and `NoteHexPrimary`
//     refuses to latch on it.
//   - A load or a reseed: `Reset`.
//   - THE PRODUCTION MENU CLOSING DOES NOT CLEAR, AND THAT IS A DECISION RATHER THAN AN
//     OMISSION. The exit button returns the player to the factory they were focused on with
//     BUILD still on screen; clearing here would make exit behave as a cancel, which is a
//     different gesture the player already has. Nothing in this file is called from any close
//     path, and nothing should be.
//
// EVERY ONE OF THOSE IS BELT AND BRACES OVER THE RECOMPUTE, NOT LOAD-BEARING FOR
// CORRECTNESS, and saying which is which is the point. Because the decorator re-resolves the
// hex against `Model.Factories` and `Model.ViewingSide` every refresh, a latch that survived
// a hand-over would already draw nothing. The clear points exist so that the STATE and the
// SCREEN agree -- so `HasFocus()` is not quietly true about a factory the player lost three
// turns ago -- and so a clause can pin the intent rather than only its shadow.
//
// NOT IN THIS FILE, with reasons:
// - NO SELECTION-MACHINE CHANGE. Ruled: `FStratSelectionMachine` stays untouched. A click on
//   your own factory latches here AND still goes to the machine, both, always -- so a click
//   that is simultaneously a move order and a focus gesture does both and neither is
//   swallowed.
// - NO "IS THE MENU OPEN" MIRROR. Two headers already refuse that mirror. BUILD stays visible
//   under the menu (ZOrder 20 over the command bar's 5), and
//   `AStratScoreboardHUD::OpenProductionMenuWidget` already refuses an already-open menu, so
//   a second press is inert without anything here knowing what a menu is.
// - NO BUILD LEGALITY. Whether this side can afford a §2.4 row, whether the factory has built
//   this turn, whether it is spawn-blocked: all three are the menu's rows and the rules
//   module's answers. `AStratPlayerController::ToggleProductionMenu` already records why a
//   pre-check would replace the module's reason with this class's silence, and that argument
//   is unchanged by the button.
// - NO HISTORY, NO STACK, NO PREVIOUS FOCUS. This is a value describing one frame, reconciled
//   like every other part of the model.

#pragma once

#include "CoreMinimal.h"

// Forward declaration only. This header declares no reflected type, and a consumer that
// needs the model includes `StratViewModel.h` for itself -- as `AStratPlayerController`
// already does. `FStratHoverState.h` takes the identical shape for the identical reason.
struct FStratViewModel;

/**
 * Which factory the player is focused on, and whether they are focused on one at all.
 *
 * TWO FIELDS OF STATE AND TWO OF OBSERVATION, AND NOTHING ELSE. It holds no board, no
 * bridge, no factory copy, no ownership and no "can build" -- so there is no mirror here
 * that can go stale, because the mirror was never made. See the file block.
 *
 * THE STATE IS PRIVATE AND THE ACCESS IS BY OUT-PARAMETER. `GetFocusedHex` returning false
 * is the only way to learn there is no focus, so a caller cannot read the coordinate without
 * having been told whether it means anything -- `FIntPoint(0, 0)` is a real hex on this
 * board and cannot signal its own absence, the same trap
 * `AStratPlayerController::GetProductionTargetHex` and
 * `UStratMatchSubsystem::IsProductionMenuOpen` both record.
 */
struct STRATPLAY_API FStratBuildAffordance
{
	/**
	 * Records a primary click on `Hex`, against the model that click was made over.
	 *
	 * ONE ARM, BOTH DIRECTIONS. If `Hex` holds a factory owned by `Model.ViewingSide` and the
	 * match has no result, the focus moves to it; OTHERWISE THE FOCUS IS CLEARED. There is no
	 * early return and no path on which a click leaves a previous focus standing -- which is
	 * what makes "the player clicked somewhere else" a clear without anything having to
	 * remember to call one.
	 *
	 * IT READS THE MODEL AND ASKS NOTHING ELSE. `Model.Factories` states which hexes hold
	 * factories and who owns each; `Model.ViewingSide` states whose screen this is. Both are
	 * fields of one value produced by one bridge call, so the two operands cannot be about
	 * two different boards.
	 *
	 * `Owner == Model.ViewingSide` AND NOT `Owner != INDEX_NONE`, and the one input on which
	 * those differ is stated rather than left to be met: a NEUTRAL factory carries
	 * `Owner == INDEX_NONE`, so a model whose `ViewingSide` were also `INDEX_NONE` would
	 * latch it. That model cannot come out of `StratBuildViewModel`, which range-checks
	 * `ViewingSide` against the snapshot's own side count and refuses rather than producing
	 * one; it can only be hand-built. Recorded here because a clause is exactly the thing
	 * that hand-builds one.
	 *
	 * A CONCLUDED MATCH REFUSES TO LATCH. §2.8's finished match opens no menu -- the gate is
	 * already in `ToggleProductionMenu` and in `HandleSelectionEvent` -- so a BUILD button
	 * appearing on a final board would advertise a control that is guaranteed to refuse.
	 *
	 * @return true if this CHANGED the state -- a different factory, a first factory, or a
	 *         focus cleared. False when the click landed on the factory already focused, and
	 *         false when it landed on nothing with nothing focused.
	 */
	bool NoteHexPrimary(FIntPoint Hex, const FStratViewModel& Model);

	/**
	 * Clears the focus if the match, the turn or the seat has moved under it.
	 *
	 * THE TURN CLOCK IS `FStratGuidedOpening::Observe`'S, arrived at by citation rather than
	 * by reinvention: `LastObservedTurn != Model.Match.Turn` is a new turn. Keying on the
	 * turn the model reports rather than on an `EndTurn` event is what makes this cover EVERY
	 * AI turn as well as the player's, and what makes a GATED or REFUSED end turn -- which
	 * advances no turn -- correctly not clear.
	 *
	 * THREE INDEPENDENT CAUSES, EACH CHECKED, NONE CONFLATED. A turn change, a viewing-side
	 * change and a concluded match are different events; a hot-seat hand-over is a turn
	 * change AND a seat change and would be covered twice, but an AI turn in a
	 * single-seat session is a turn change ONLY, and a match concluding mid-turn is neither.
	 *
	 * IT IS MUTATING AND `DecorateViewModel` IS NOT, which is why they are two calls and not
	 * one. That is `FStratGuidedOpening`'s split exactly, and it is what lets a `const`
	 * decorate be called on a path that must not advance any clock.
	 *
	 * THE OBSERVATION FIELDS ARE UPDATED ON EVERY CALL, INCLUDING THE FIRST. A first
	 * observation records the turn and the seat and clears NOTHING -- there is nothing
	 * latched on the frame before anything has been clicked, and treating the first
	 * observation as a change would be a clear with no cause.
	 *
	 * @return true if this CHANGED the state, which here means only "a focus was cleared".
	 */
	bool Observe(const FStratViewModel& Model);

	/**
	 * Drops the focus.
	 *
	 * THE HEX IS RESET TO (0,0) TOO, on `FStratHoverState::ClearHoveredHex`'s stated reason:
	 * two cleared states compare equal field for field, and a stale coordinate cannot survive
	 * behind a false flag to be read by something that forgot to check it.
	 *
	 * @return true if this CHANGED the state. False when nothing was focused already.
	 */
	bool ClearFocus();

	/**
	 * The focused factory's hex, if there is one.
	 *
	 * `OutHex` IS ZEROED ON THE FALSE PATH rather than left alone, matching
	 * `FStratHoverState::GetHoveredHex` and `AStratPlayerController::HexUnderCursor`, so the
	 * three cannot be told apart by a caller that ignores the return value in the way that
	 * matters.
	 */
	bool GetFocusedHex(FIntPoint& OutHex) const;

	/** Whether a factory is focused at all. */
	bool HasFocus() const { return bHasFocusedHex; }

	/**
	 * Back to the state a fresh one is in.
	 *
	 * FOR A LOAD OR A RESEED, where the board the focus was about no longer exists. It clears
	 * the OBSERVATION fields as well as the focus, which `ClearFocus` deliberately does not:
	 * after a reseed the recorded turn and seat describe a match that is over, and leaving
	 * them would make the FIRST `Observe` of the new match see a spurious change.
	 *
	 * NO SHIPPING CALLER TODAY, AND THAT IS STATED RATHER THAN IMPLIED. Measured on the tree
	 * at `fcf64d3`: `FStratSelectionMachine::Reset()` -- the method this one was written for
	 * parity with -- is declared, defined, and called by NOTHING, in `Source/` including
	 * `Tests/`. So the parity is with an uncalled method and this one inherits that. It is a
	 * debt, and the condition that discharges it is a load or reseed path that calls both:
	 * whatever eventually resets the machine must reset this in the same place, because a
	 * focus surviving a reseed is a BUILD button about a factory on a board that is gone.
	 */
	void Reset();

	/**
	 * Writes §2.11.5's BUILD affordance onto a model somebody else built.
	 *
	 * THE POSITIVE OBLIGATION, in `FStratHoverState::DecorateViewModel`'s sense: it is the
	 * only route from this state to anything on screen, and it is what makes the focused
	 * factory a property of the MODEL rather than of the controller.
	 *
	 * IT FORWARDS TO `StratDecorateCommandBar` AND DECIDES NOTHING. This method hands over a
	 * hex and a bool; every question about what is drawn -- is that hex a factory, is it
	 * this seat's, is END TURN suggested, is it dimmed -- is answered in one place in
	 * `StratUI`, beside the model whose census governs it. A copy of that resolution here
	 * would be a second author of the same field.
	 *
	 * UNCONDITIONAL. The decorator writes all five fields on every call, including the
	 * nothing-focused case, so a decorated model is a complete statement about the command
	 * bar and never a difference from a previous one.
	 *
	 * IT MUST RUN AFTER `FStratGuidedOpening::DecorateViewModel`, and that constraint is
	 * REAL and runs one way: the decorator reads `Model.Guidance.bEndTurnGated` and
	 * `EndTurnGateHover`, which the guidance layer writes, and writes nothing the guidance
	 * layer reads. Run before it, the END TURN control would be drawn against last frame's
	 * gate.
	 */
	void DecorateViewModel(FStratViewModel& Model) const;

private:
	/** The focused factory's hex, X = q and Y = r. Meaningless unless `bHasFocusedHex`. */
	FIntPoint FocusedHex = FIntPoint::ZeroValue;

	/** Whether `FocusedHex` means anything. See the struct block on why there is no sentinel. */
	bool bHasFocusedHex = false;

	/**
	 * The turn and seat this struct last saw, or `INDEX_NONE` before the first `Observe`.
	 *
	 * `INDEX_NONE` AND NOT 0, because turn 0 and side 0 are both real values and a first
	 * observation must be distinguishable from having seen turn 0. See `Observe` on why the
	 * first observation clears nothing.
	 */
	int32 LastObservedTurn = INDEX_NONE;
	int32 LastObservedViewingSide = INDEX_NONE;
};
