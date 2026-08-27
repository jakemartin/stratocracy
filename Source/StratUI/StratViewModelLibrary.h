// GDD §2.11.2 -- PURE SELECTORS OVER `FStratViewModel`. Wave 3's model surface for the
// persistent HUD, and the file that keeps that HUD from computing anything.
//
// WHAT GAP THIS CLOSES. §2.11.2's HUD needs three numbers that nothing in this project
// could hand a widget at `ee7300c`. Two of them -- the Fame pool and the `+X/turn`
// income -- ALREADY EXIST as `FStratSideView::FameTotal` and `FStratSideView::IncomePerTurn`,
// mirrored one-for-one from `strat::UiSideView`; what did not exist was a way to reach the
// RIGHT side's copy. `FStratViewModel::Sides` is a `TArray` indexed by `strat` side,
// deliberately so -- writing `You`/`Enemy` there "would bake §2.11.4's two-column panel into
// the general description of the board" -- and the only correct index is
// `FStratViewModel::ViewingSide`. A Blueprint left to do that pairing does an unguarded
// `Get (a copy)` on an array, which is one scenario away from reading side 1's purse onto
// side 0's HUD in a hot-seat game.
//
// The third -- the count of units still able to act -- did not exist in any form.
//
// MEASURED, in this tree at `ee7300c`, before any of this was designed. `FameTotal` had
// exactly ONE reader outside its own projection and the parity clauses: the `Shortfall`
// subtraction inside `StratBuildProductionMenu`. `IncomePerTurn` had NONE AT ALL -- written
// by `StratBuildViewModel`, asserted by `StratViewModelParity` and by
// `StratScoreboardHUDSeam`, and read by nothing that draws. Recorded as a measurement rather
// than as an adjective because it is the fact that makes this file a gap-closer instead of a
// convenience.
//
// EVERYTHING HERE IS A FUNCTION OF THE MODEL AND OF NOTHING ELSE, and that is the whole
// design rather than a tidiness preference. §4.9's T-INT-05 requires the screen to be
// rebuildable FROM THE VIEW MODEL ALONE. A `BlueprintPure` on `AStratPlayerController` or on
// `UStratMatchSubsystem` returning any of these numbers would work on the first day and would
// make T-INT-05 false -- `FStratHoverView`'s own block states that trap in those words, one
// wave earlier, about the hovered hex. Every function below takes `const FStratViewModel&`
// and reads nothing else: no bridge, no subsystem, no world, no member state. This class
// holds no state and is never instantiated.
//
// WHY A SELECTOR AND NOT A FIELD, which was the other shape and is the one a reader will
// reach for. A `UnitsAbleToActCount` on `FStratViewModel` would have been the first piece of
// ARITHMETIC INSIDE THE MODEL. `StratViewModel.h`'s own block states that absolute and
// records the single exception already carved out of it -- `FStratBuildOptionView::Shortfall`,
// which is in a struct that is NOT part of the model at all. A count field would have been
// the second exception and the first one INSIDE the walk `StratViewModelParity` performs,
// where its only possible parity assertion would be against a snapshot field that does not
// exist. So the count is computed OVER the model, by a caller, on demand, and the model does
// not grow. The precedent is `FStratHoverView`'s: "a widget that wants the unit under the
// cursor looks it up in `Units` by hex; that is a lookup and not arithmetic".
//
// AND IT IS STILL NOT WIDGET-SIDE ARITHMETIC, which is the rule this file sits closest to.
// The widget calls a named C++ function whose derivation is written down and pinnable; it does
// not sum a list in a Blueprint loop. T-UI-03's clause is that every number a widget DRAWS
// equals exactly one `strat::UiSnapshot` field -- and the two side numbers below satisfy it
// literally, because they ARE that field, selected. The count is the one number that does not,
// and the next block is why it cannot.
//
// THE IDLE COUNT HAS NO MODULE ANSWER TO BRIDGE, AND THAT IS UPSTREAM'S OWN RULING RATHER
// THAN A GAP HERE. `strat::UiUnitView` carries `hasMoved` and `hasActed` and then says, in
// terms: "NEITHER IS §2.11.1's DONE bit -- that bit is the selection machine's own, every
// §2.11 surface reading 'has not acted' binds to it, and it is deliberately absent here
// because where per-unit presentation state lives is unruled." `strat::UiSnapshot` therefore
// carries no idle count, and it could not: the count is over a bit the rules module refuses
// to hold. `FStratSelectionMachine::DecorateViewModel` is that bit's producer, writing
// `bDone` onto the built model between `StratBuildViewModel` and `ApplyView`; this file counts
// what that decorator wrote.
//
// NO FILING WAS OPENED AGAINST `Source/StratRules/` FOR IT, and that is a decision rather than
// an omission. A rules-side idle count would be the module asserting presentation state it has
// ruled it does not own. What WOULD justify one is a §2.11.2 requirement that the count mean
// "has a legal command available" -- that is a rules answer and no engine module may derive
// it. Nothing in §2.11.2 asks for that today; see `CountUnitsAbleToAct` on the distinction.
//
// NOT IN THIS FILE, with reasons:
//  - Any widget. Wave 3 is model-side; the HUD Blueprint and the command bar are a later lane.
//  - Any writer. Nothing here mutates a model. The decoration seam has three writers already,
//    and a fourth that only reads today is a writer waiting to happen.
//  - A "can this unit legally do anything" query, per the paragraph above.
//  - An End Turn verb. The command half of §2.11.2's End Turn is
//    `AStratPlayerController::RequestEndTurn`, because ending a turn must pass through the
//    selection machine -- which clears its DONE set on an accepted EndTurn -- and this file
//    reaches no machine and no bridge by construction.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "StratViewModel.h"

#include "StratViewModelLibrary.generated.h"

/**
 * §2.11.2's HUD numbers, selected from the view model.
 *
 * A `UBlueprintFunctionLibrary` because every member is static, pure, and about a value the
 * caller already holds. `UStratMatchSubsystem.h` records rejecting this shape for ITS own job,
 * and the reasons do not transfer: that class owns a bridge, a world and a lifetime, and none
 * of the three appears below.
 */
UCLASS()
class STRATUI_API UStratViewModelLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * One side's row of the model, by `strat` side index.
	 *
	 * RANGE-CHECKED AND REFUSING, NEVER DEFAULTING. A model asked about a side outside the
	 * match is not a side with a zero purse -- it is a caller asking the wrong question -- and
	 * answering `Fame: 0` would put a number on screen that no rules state produced. `OutSide`
	 * is left default-constructed on a refusal and the return value is the only discriminator.
	 *
	 * @return false when `Side` is outside `Model.Sides`, including on an empty model.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|View",
		meta = (DisplayName = "Get Side View"))
	static bool GetSideView(const FStratViewModel& Model, int32 Side, FStratSideView& OutSide);

	/**
	 * The row of the model for the side the model was RENDERED FOR.
	 *
	 * `ViewingSide` AND NEVER `Match.SideToMove`, and that distinction is the model's own
	 * rather than this file's: `FStratMatchView::SideToMove` is "the rules module's answer
	 * about the match", `ViewingSide` is the caller's statement about whose screen this is,
	 * "and in hot-seat the two differ every other turn". §2.11.2's HUD is a statement about
	 * whose screen this is, so it binds to the second. A HUD wired to `SideToMove` would show
	 * the opponent's purse on every screen drawn between the turn flipping and the hand-over
	 * completing.
	 *
	 * THE ONE PLACE THAT PAIRING IS MADE, so that no Blueprint performs it. That is the whole
	 * reason this exists beside `GetSideView` rather than being spelled out at each call.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|View",
		meta = (DisplayName = "Get Viewing Side View"))
	static bool GetViewingSideView(const FStratViewModel& Model, FStratSideView& OutSide);

	/**
	 * §2.11.2's idle count: how many of `Side`'s units the player has not finished with.
	 *
	 * IT IS A NUDGE AND NEVER A LEGALITY CLAIM. It answers "have you forgotten someone?" and
	 * it does NOT answer "could this unit legally do something right now". The second question
	 * is the rules module's, costs a `FStratBridge::Reachable` per unit, and would make a HUD
	 * number depend on a query the model does not carry -- T-INT-05 false again. A count of 1
	 * therefore does not promise the player that a legal command exists; it promises only that
	 * one unit is neither retired, nor locked, nor spent.
	 *
	 * THE DERIVATION, WRITTEN OUT SO IT CAN BE PINNED RATHER THAN INFERRED FROM THE CODE. A
	 * `FStratUnitView` U counts exactly when ALL FOUR hold:
	 *
	 *   1. `U.Side == Side`                -- the other side's units are not this HUD's.
	 *   2. `!U.bDone`                      -- §2.11.1's retirement. The player said so.
	 *   3. `!U.bLockedThisTurn`            -- §2.11.6-B's guided-opening lock. The player is
	 *                                         FORBIDDEN to command it, so counting it would be
	 *                                         the HUD nagging for something the input layer
	 *                                         refuses.
	 *   4. `!(U.bHasMoved && U.bHasActed)` -- both §2.1 allowances spent. This is the arm that
	 *                                         catches a unit which used everything up without
	 *                                         ever being marked DONE.
	 *
	 * CLAUSE 4 IS A CONJUNCTION UNDER A NEGATION AND NOT A DISJUNCTION, and getting it
	 * backwards is the defect to watch for. `hasMoved` and `hasActed` are TWO INDEPENDENT FLAGS
	 * by T-TURN-01, and `strat::UiUnitView` says so -- "one field cannot express a unit that
	 * has spent exactly one of them". A unit that has moved but not attacked still has an
	 * attack, so it is still able to act. Written as `!(A && B)` because that is the sentence
	 * "both are spent", negated, which is what §2.1 says; `!A && !B` would count only untouched
	 * units and `!A || !B` spelled without the outer negation is the same thing written in a
	 * way the next reader has to re-derive.
	 *
	 * THE FLAG UNIT IS COUNTED LIKE ANY OTHER. `bIsFlag` is not consulted: the flag is a unit
	 * that moves, and excluding it would be this file writing a §2.5 rule.
	 *
	 * A SIDE OUTSIDE THE MATCH ANSWERS 0 RATHER THAN REFUSING, and the asymmetry with
	 * `GetSideView` above is deliberate rather than an inconsistency. There is no such thing as
	 * a unit of a side that does not exist, so 0 is the TRUE count and not a fallback; whereas
	 * there is no such thing as THE PURSE of a side that does not exist, which is why that one
	 * refuses. `Model.Sides` is not consulted here at all -- this walks `Model.Units` -- so an
	 * empty `Sides` cannot make the count wrong.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|View",
		meta = (DisplayName = "Count Units Able To Act"))
	static int32 CountUnitsAbleToAct(const FStratViewModel& Model, int32 Side);

	/**
	 * The idle count for `Model.ViewingSide`. `GetViewingSideView`'s pairing, for the count.
	 *
	 * SAME REASON AND SAME HAZARD: this is the number §2.11.2's HUD draws, and a Blueprint that
	 * passed `Match.SideToMove` to the overload above would count the opponent's units on a
	 * hand-over screen.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|View",
		meta = (DisplayName = "Count Viewing Side Units Able To Act"))
	static int32 CountViewingSideUnitsAbleToAct(const FStratViewModel& Model);
};
