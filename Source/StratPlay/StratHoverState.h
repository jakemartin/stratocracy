// GDD §2.11.3 / §2.11.2 / T-UI-01 -- the hovered hex, as a value, and the producer of
// `FStratHoverView`.
//
// WHAT GAP THIS CLOSES. An MVP-gap audit found that §2.11.3's attack forecast card and
// §2.11.2's info panel are both specified as HOVER-driven and that no hover input existed:
// `AStratPlayerController` carried `SelectAction`, `CancelAction`, `WaitAction`,
// `EndTurnAction` and `OpenProductionMenuAction`, and nothing for hover. The hover STRINGS
// on `FStratGuidanceView` -- `EndTurnGateHover` and `LockedUnitHover` -- are shipped and are
// NOT this: they are tooltip text for a widget that already knows what it is drawing, and
// neither answers the question "where is the cursor". This file answers that question and
// nothing else.
//
// IT IS A PLAIN STRUCT AND NOT A UCLASS, AND NOT AN ACTOR'S `UPROPERTY`, for the two reasons
// `FStratSelectionMachine` gives and which apply here without amendment:
//   1. TESTABILITY, AND HERE IT IS THE WHOLE POINT OF THE FILE EXISTING. A hovered hex whose
//      only observable route was a live mouse over a live Slate viewport could not be pinned
//      by a clause at all -- and a correct mapping no fixture can reach is a shape this
//      project has already lost a milestone to. This struct is drivable with no actor, no
//      world, no PIE and no cursor: `SetHoveredHex` then `DecorateViewModel` over a
//      default-constructed `FStratViewModel` is the entire seam, and it is why the input
//      resolution below is split from the state.
//   2. THE T-INT-05 OBLIGATION. A reflected copy of a presentation bit on an actor is what
//      `T-INT-05.NoActorHoldsPresentationBits` forbids. This struct is HELD by the
//      controller and read out of it by nobody: the hovered hex reaches the screen only
//      through `DecorateViewModel`, on the model, on the way to `ApplyView`.
//
// IT RESOLVES NOTHING AND TRACES NOTHING. There is no cursor here, no `FHitResult`, no
// viewport and no board pointer. `AStratPlayerController::HexUnderCursor` is the ONE
// cursor-to-hex route in this project -- an instance-index lookup into the board's own list,
// never an inverse of `WorldLocationOfHex` -- and the controller feeds this struct the answer
// it already had. A second trace path would be a second author of "which hex is that", and
// the two would disagree on the day the board's instance list is rebuilt.
//
// THERE IS NO HEX ARITHMETIC IN THIS FILE. No distance, no neighbour, no addition of two
// coordinates. `FIntPoint::operator==` is the only spatial operator, used to answer whether
// the hovered hex CHANGED -- which is a question about two values this file was handed, not
// a question about the board.
//
// THE SETTERS RETURN WHETHER ANYTHING MOVED, AND THAT RETURN IS THE FILE'S ONE LOAD-BEARING
// DESIGN CALL. A mouse-move action triggers every frame the cursor moves; refreshing the
// whole model on each of those would rebuild the board's view model at mouse-poll rate for a
// state that is identical to the previous one nearly every time. So the controller refreshes
// only when one of these returns true. The alternative shape -- an unconditional refresh with
// the de-duplication inside `RefreshFromMachine` -- was rejected because it would make every
// other caller of that method pay for a hover concern, and because "did the hover change" is
// a fact only this struct holds.
//
// NOT IN THIS ROUND, with reasons:
// - NO FORECAST AND NO INFO PANEL. Both are their own waves, both compose over this hex, and
//   neither is here. A forecast field landed now would be a field with no reader and no
//   clause, which is how this project's record describes a surface that reads as built.
// - NO HOVERED UNIT. `FStratViewModel::Units` already states which unit is on which hex and
//   a consumer looks it up. See `FStratHoverView`'s own block on why a second author of that
//   fact is worse than a lookup.
// - NO HOVER HISTORY, NO DWELL TIMER, NO ENTER/EXIT EVENTS. This is a value describing one
//   frame, reconciled like every other part of the model, and nothing on screen is produced
//   by an event the model does not also carry.

#pragma once

#include "CoreMinimal.h"

// Forward declaration only. This header declares no reflected type, and a consumer that
// needs the model includes `StratViewModel.h` for itself -- as `AStratPlayerController`
// already does.
struct FStratViewModel;

/**
 * Which hex the cursor is over, and whether it is over one at all.
 *
 * TWO FIELDS AND NOTHING ELSE. It holds no board, no bridge, no unit id and no copy of any
 * rules state -- so there is no mirror here that can go stale, because the mirror was never
 * made. That is the same posture `FStratSelectionMachine` takes toward `bHasMoved` and
 * `bHasActed`, arrived at from the other end: the machine declines to cache what the rules
 * module owns, and this struct declines to cache what the board owns.
 *
 * THE STATE IS PRIVATE AND THE ACCESS IS BY OUT-PARAMETER, deliberately. `GetHoveredHex`
 * returning false is the only way to learn there is no hovered hex, so a caller cannot read
 * the coordinate without having been told whether it means anything -- which is precisely the
 * mistake `FStratHoverView`'s block warns a widget against, closed here at the source.
 */
struct STRATPLAY_API FStratHoverState
{
	/**
	 * Records that the cursor is over `Hex`.
	 *
	 * @return true if this CHANGED the state -- a different hex, or the first hex after
	 *         nothing. False means the cursor is over the hex it was already over, which is
	 *         the common case on any frame the mouse moves within one tile.
	 *
	 * NO VALIDATION, AND NONE IS POSSIBLE HERE. This struct does not know the board's extent
	 * and must not learn it: the caller got this hex from `AStratBoardActor` by instance
	 * index, so a hex that reaches this method is on the board by construction. A bounds
	 * check here would be a second, weaker statement of the board's own geometry.
	 */
	bool SetHoveredHex(FIntPoint Hex);

	/**
	 * Records that the cursor is over no hex.
	 *
	 * @return true if this CHANGED the state. False when nothing was hovered already -- the
	 *         state every frame the cursor spends off the board after the first.
	 *
	 * THE HEX IS RESET TO (0,0) TOO, so that two cleared states compare equal field for field
	 * and a stale coordinate cannot survive behind a false flag to be read by something that
	 * forgot to check it.
	 */
	bool ClearHoveredHex();

	/**
	 * The hovered hex, if there is one.
	 *
	 * `OutHex` IS ZEROED ON THE FALSE PATH rather than left alone, matching
	 * `AStratPlayerController::HexUnderCursor`'s own contract, so that the two functions
	 * cannot be told apart by a caller that ignores the return value in the way that matters.
	 */
	bool GetHoveredHex(FIntPoint& OutHex) const;

	/** Whether the cursor is over a hex at all. */
	bool IsHovering() const { return bHasHoveredHex; }

	/**
	 * Writes this state onto a model somebody else built.
	 *
	 * THE POSITIVE OBLIGATION, in `FStratSelectionMachine::DecorateViewModel`'s sense: it is
	 * the only route from this state to anything on screen, and it is what makes a hovered
	 * hex a property of the MODEL rather than of the controller.
	 *
	 * UNCONDITIONAL. It writes both fields on every call, including the not-hovering case, so
	 * a model that has been decorated is a complete statement about the cursor and never a
	 * difference from a previous one. A decorator that wrote only on the true path would
	 * leave a previous frame's hover standing in any model built by a caller that never
	 * hovers -- an AI turn's reconcile, a hand-over, a gate -- and the card would hang on
	 * screen over a hex the cursor left.
	 *
	 * IT TOUCHES NO OTHER FIELD of the model, and that is the whole of what makes it safe to
	 * call in any order relative to the OTHER DECORATORS, however many there come to be.
	 * [AMENDED, wave 0, on the second W0 gate: "relative to the other two decorators". True on
	 * the day it was written -- `AStratPlayerController::DecorateForPresentation` called
	 * exactly two others -- and false the day W1 or W2 adds one, which is the whole point of
	 * this wave's own keep/remove rule and the last live census in it. The ordering claim is
	 * unchanged and does not depend on the number: this decorator writes one field of the
	 * model and reads none, so nothing it does can be ordered wrongly against anything.]
	 */
	void DecorateViewModel(FStratViewModel& Model) const;

private:
	/** The hex, X = q and Y = r. Meaningless unless `bHasHoveredHex`. */
	FIntPoint HoveredHex = FIntPoint::ZeroValue;

	/** Whether `HoveredHex` means anything. See the struct block on why there is no sentinel. */
	bool bHasHoveredHex = false;
};
