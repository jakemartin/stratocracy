// GDD §2.11.3 / T-UI-01 -- the attack forecast card's producer: the rules answer this
// module needs for it, and the decorator that puts it on the model.
//
// WHAT GAP THIS CLOSES. Wave 0 landed `FStratHoverState`, which knows WHERE the cursor is
// and nothing else -- its own header says the forecast wave "will ask
// `FStratBridge::Forecast` about the hex this file carries, and it will ask it from
// somewhere else". This is somewhere else. Until it existed the two facts §2.11.3 needs
// composed -- a hovered hex and a selected attacker -- were held by two different structs
// and were never brought together, so the centrepiece display had no producer at all and
// `FStratViewModel` had no field for it.
//
// IT COMPOSES TWO THINGS IT DOES NOT OWN, and that is the whole of what it does. The hex
// comes off `FStratViewModel::Hover`, written by `FStratHoverState`. The attacker comes off
// `FStratSelectionMachine::GetSelectedUnitId`. Neither is cached here and neither is
// recomputed: this file holds no hex, no unit id, no bridge and no copy of any rules state,
// exactly as `FStratHoverState` holds no board.
//
// A FREE FUNCTION AND NOT A STRUCT, where the other three decorators on this seam are
// structs. They are structs because they hold state -- a selection, a hovered hex, a beat.
// This one holds none: every input arrives as an argument and nothing survives the call. A
// struct here would be an empty object whose only purpose was to look like its neighbours,
// and the next reader would go looking for the state it does not have.
//
// THE QUERY IS AN INTERFACE, FOR `IStratRulesQuery`'S REASON EXACTLY. A gate must be able to
// drive this over positions the one shipped scenario does not contain -- a counter-kill on a
// flag, a bridge hex's negative bonus, a lethal forecast against the enemy flag -- and a
// decorator that reached `FStratBridge` directly could only ever be tested on boards
// *Ferrum Crossing* happens to produce. `a-correct-mapping-can-be-unreachable-by-test` is
// this project's own record of what that costs.
//
// IT ASKS ONE QUESTION AND MAKES NO RULES DECISION. There is no range check here, no
// distance, no membership test against the lit target set, and no re-derivation of anything
// §2.6 owns. `FStratBridge::AttackForecast` answers `bLegal`, and `bLegal` IS "lit" --
// `FStratBridge::AttackTargetHexes` lights a hex on that same answer from that same
// function, so this file does not consult the target set and cannot disagree with it.
//
// NOT IN THIS ROUND, with reasons:
// - NO WIDGET AND NO ASSET. The card's Widget Blueprint is a separate lane. This produces
//   the value; nothing here draws.
// - NO ATTACK RING AND NO RANGE-1 HOLE. §2.11.3's dead-zone drawing is an OVERLAY, off
//   `FStratSelectionMachine::BuildOverlays`, and it is about the selection rather than about
//   one hovered target.
// - NO HOVER-DWELL, NO FADE, NO CARD ANIMATION STATE. The card is on screen on exactly the
//   frames the model says it should be. Nothing on screen is produced by an event the model
//   does not also carry.
// - NO COMMIT PATH. §2.11.3's `[LMB] Commit` is `FStratSelectionMachine::HandleEvent`'s
//   existing Attack arm and needs nothing from this file; a second route from a hovered hex
//   to a submitted attack would be a second author of what a click means.

#pragma once

#include "CoreMinimal.h"

// Forward declarations only. This header declares no reflected type and needs no definition
// of any of these: the query takes and returns them by reference, and
// `FStratBridgeForecastQuery` borrows a pointer. Consumers include what they need --
// `StratForecastQuery.cpp` includes `StratBridge.h` and `StratViewModel.h`, which is the
// same posture `FStratSelectionMachine` takes toward both.
class  FStratBridge;
struct FStratAttackForecast;
struct FStratViewModel;

/**
 * The one rules answer §2.11.3 needs, and no others.
 *
 * ONE METHOD, AND IT IS THE WHOLE CARD. There is deliberately no `IsLegalTarget` and no
 * `TargetHexes` beside it -- `IStratRulesQuery`'s block gives the reason for movement and it
 * transfers unchanged: a second entry point is a place a later "fast path" can answer
 * locally. The card's legality and the card's numbers arrive together, from one call, so
 * they cannot describe two different boards.
 *
 * TWO CHANNELS, AS THE BRIDGE HAS THEM. The return says whether the question could be ASKED;
 * `OutForecast.bLegal` says what the rules ANSWERED. An empty hex, a friendly unit and an
 * out-of-range target are all answers -- a cursor crossing the board produces every one of
 * them every second -- and folding them into a refusal would make an ordinary hover look
 * like a fault.
 */
struct STRATPLAY_API IStratForecastQuery
{
	virtual ~IStratForecastQuery() = default;

	/** §2.11.3's card for one attacker against one hex. `false` means the question was malformed. */
	virtual bool ForecastAgainst(int32 AttackerId, FIntPoint DefenderHex,
	                             FStratAttackForecast& OutForecast,
	                             FString&              OutFailureReason) const = 0;
};

/**
 * The one production implementation: `FStratBridge::AttackForecast`, forwarded.
 *
 * A NAMED TYPE RATHER THAN A LAMBDA AT THE CALL SITE, for `FStratBridgeRulesQuery`'s reason:
 * a gate can construct one against a seeded bridge and assert that what it hands back IS the
 * bridge's own answer, which is unassertable if the forwarding exists only inside a
 * controller method.
 *
 * BORROWS AND NEVER OWNS, as `FStratBridgeRulesQuery` does. The bridge is the match
 * subsystem's; outliving it is the caller's problem to avoid and not this object's to solve
 * with a copy of a `strat::GameState`.
 */
struct STRATPLAY_API FStratBridgeForecastQuery final : public IStratForecastQuery
{
	explicit FStratBridgeForecastQuery(const FStratBridge* InBridge)
		: Bridge(InBridge)
	{
	}

	virtual bool ForecastAgainst(int32 AttackerId, FIntPoint DefenderHex,
	                             FStratAttackForecast& OutForecast,
	                             FString&              OutFailureReason) const override;

private:
	const FStratBridge* Bridge = nullptr;
};

/**
 * Writes §2.11.3's card onto a model somebody else built.
 *
 * UNCONDITIONAL, on `FStratHoverState::DecorateViewModel`'s rule and for its reason: it
 * writes `FStratViewModel::Forecast` on EVERY call, including every path that produces no
 * card, so a decorated model is a complete statement and never a difference from a previous
 * one. A decorator that wrote only on the card path would leave a previous frame's forecast
 * standing in any model built by a caller that never hovered -- an AI turn's reconcile, a
 * hand-over, a gate -- and the card would hang on screen over a hex the cursor left, quoting
 * an attacker that is no longer selected.
 *
 * IT HAS AN ORDERING CONSTRAINT AND IT IS THE FIRST DECORATOR ON THIS SEAM THAT DOES,
 * `FStratGuidedOpening::Observe` aside. It READS `Model.Hover`, so it must run after
 * `FStratHoverState::DecorateViewModel`; run before it, it composes this frame's selection
 * against last frame's hex and the card is silently one mouse-move stale.
 * `FStratHoverState::DecorateViewModel`'s own declaration was amended on wave 0's second
 * gate to say that its unorderedness would stop being the whole story the day W1 or W2 added
 * a decorator. This is that day, and the constraint is one-directional: it reads the hover
 * and writes nothing the hover reads.
 *
 * THE FOUR PATHS THAT PRODUCE NO CARD, and none of them is a fault:
 *   - nothing is selected (`AttackerUnitId == INDEX_NONE`);
 *   - the cursor is over no hex (`Model.Hover.bHasHoveredHex` false);
 *   - the selected unit has already spent its act this turn;
 *   - the rules module calls the attack illegal -- an empty hex, a friendly unit, out of
 *     range, the attacker's own hex. This is by far the commonest and is what makes the card
 *     appear over lit targets only.
 *
 * THE ACT CHECK IS READ OFF THE MODEL AND MIRRORS THE SELECTION MACHINE'S, and is named here
 * because it is the one gate in this file that is not the rules module's own.
 * `strat::uiForecast` does not consult `hasActed` -- it answers about §2.6 and not about the
 * turn -- so a unit that has attacked already still gets a `legal` forecast, and a commit
 * card for a command `FStratSelectionMachine::HandleEvent` will refuse is worse than no
 * card. The bit read is `FStratUnitView::bHasActed`, the same RULES-side bit the machine's
 * own `has already acted this turn` guard reads, so the two cannot drift. Deliberately NOT
 * `bDone`: that is the engine's presentation bit and a unit can reach it with its act
 * unspent, which would hide a card for an attack §2.6 permits.
 *
 * IT TOUCHES NO OTHER FIELD of the model. It reads `Hover` and `Units` and writes
 * `Forecast`, and nothing else in either direction.
 *
 * A QUERY REFUSAL LEAVES NO CARD AND IS REPORTED. The usual cause is an unseeded bridge,
 * which is the ordinary state before `StartMatch` finishes; drawing a stale card would be
 * worse than drawing none, and the reason goes back to the caller rather than to a log this
 * file chooses the verbosity of.
 *
 * @param Model            the model to decorate. `Hover` and `Units` are read; `Forecast` is written.
 * @param AttackerUnitId   the selection, or `INDEX_NONE`. `FStratSelectionMachine::GetSelectedUnitId`.
 * @param Query            the rules answer. Consulted at most once per call.
 * @param OutFailureReason set only when the query refused; untouched otherwise.
 * @return true when a card was produced.
 */
STRATPLAY_API bool StratDecorateForecast(FStratViewModel&           Model,
                                         int32                      AttackerUnitId,
                                         const IStratForecastQuery& Query,
                                         FString&                   OutFailureReason);
