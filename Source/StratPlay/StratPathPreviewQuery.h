// GDD §2.11.1 / T-UI-02 -- the path preview's producer: the rules answer this module needs
// for it, and the decorator that puts it on the model.
//
// WHAT GAP THIS CLOSES. The movement overlay knows WHICH hexes are reachable
// (`FStratSelectionMachine::BuildOverlays` -> `AStratBoardActor::ShowReach`) and
// `FStratHoverState` knows WHERE the cursor is. Nothing brought the two together into a
// ROUTE, and there was no route to bring: `strat::findPath` was routed through no
// `FStratBridge` method at all -- measured as zero occurrences of that name in `Source/`
// outside `Source/StratRules/`. `FStratBridge::MovePathToHex` landed in the same pass as
// this file and is the only reason this one can exist without computing anything.
//
// THIS FILE COMPUTES NO ROUTE AND WOULD BE THE OBVIOUS PLACE TO. The reach set is already
// in this module, with a cost on every hex, and a greedy walk downhill from the goal would
// produce a plausible route in about fifteen lines. IT WOULD ALSO BE `Move.h`'S TIE-BREAK
// RULE RESTATED HERE -- §2.5 picks the lexicographically smallest among equal-cost paths,
// and a walk that picked any other one would draw a preview the commit does not honour.
// That is the same substitution T-UI-02 measured for the highlight itself (122 divergent
// hexes across 10 of 10 units between the real query and `distance <= move`), one layer up.
// The route below arrives from the module through one call and is copied.
//
// IT COMPOSES TWO THINGS IT DOES NOT OWN, exactly as `StratForecastQuery` does. The goal
// hex comes off `FStratViewModel::Hover`, written by `FStratHoverState`. The unit comes off
// `FStratSelectionMachine::GetSelectedUnitId`. Neither is cached here and neither is
// recomputed: this file holds no hex, no unit id, no bridge and no copy of any rules state.
//
// A FREE FUNCTION AND NOT A STRUCT, for `StratDecorateForecast`'s reason: it holds no
// state, every input arrives as an argument, and nothing survives the call. A struct here
// would be an empty object whose only purpose was to look like its stateful neighbours.
//
// THE QUERY IS AN INTERFACE, FOR `IStratRulesQuery`'S AND `IStratForecastQuery`'S REASON
// EXACTLY. A gate must be able to drive this over routes the one shipped scenario does not
// contain -- a goal blocked by a friendly unit, a two-tile detour around a mountain, a goal
// exactly at the allowance and one hex past it -- and a decorator that reached
// `FStratBridge` directly could only ever be tested on boards *Ferrum Crossing* happens to
// produce.
//
// NOT IN THIS ROUND, with reasons:
// - NO DRAWING. No spline, no arrowhead, no tick widget, no overlay component. This
//   produces the value; `AStratBoardActor` and the widget lane draw it, from the model.
// - NO COMMIT PATH. §2.11.1's click-to-move is `FStratSelectionMachine::HandleEvent`'s
//   existing Move arm and needs nothing from this file. A second route from a hovered hex
//   to a submitted move would be a second author of what a click means, and it would be
//   able to submit a move to a hex the machine would refuse.
// - NO CACHE AND NO CHANGE DETECTION. The bridge is asked once per decoration, on the same
//   schedule the forecast is asked, and the answer is a complete statement. A "route
//   changed" flag is the first step towards a screen that is only correct if every previous
//   frame was also correct -- `StratViewModel.h`'s standing rule.
// - NO ATTACK-MOVE PREVIEW. A route that ends in an attack is two commands, and §2.11.1's
//   preview is about the move. The forecast card is the other half and already exists.

#pragma once

#include "CoreMinimal.h"

// Forward declarations only. This header declares no reflected type and needs no definition
// of either: the query takes engine containers by reference and `FStratBridgePathQuery`
// borrows a pointer. `StratPathPreviewQuery.cpp` includes `StratBridge.h` and
// `StratViewModel.h`, which is the posture `StratForecastQuery` and
// `FStratSelectionMachine` both take.
class  FStratBridge;
struct FStratViewModel;

/**
 * The one rules answer §2.11.1 needs, and no others.
 *
 * ONE METHOD, AND IT IS THE WHOLE PREVIEW. There is deliberately no `IsReachable` and no
 * `ReachableHexes` beside it -- `IStratRulesQuery`'s block gives the reason and it transfers
 * unchanged: a second entry point is a place a later "fast path" can answer locally. The
 * route and its ticks arrive together, from one call, so they cannot describe two different
 * boards. A caller that also wants the reach set asks `IStratRulesQuery`, which already
 * exists and is a different object with a different lifetime.
 *
 * TWO CHANNELS, AS THE BRIDGE HAS THEM AND FOR `IStratForecastQuery`'S REASON. The return
 * says whether the question could be ASKED; an empty `OutRouteHexes` says the module
 * ANSWERED that there is no route. A cursor crossing the board produces the second on most
 * of its frames -- a mountain, an occupied hex, a hex past the allowance -- and folding
 * those into a refusal would make an ordinary hover look like a fault.
 *
 * THE ARRAYS ARE ALWAYS THE SAME LENGTH when the call succeeds, because the bridge fills
 * them in one loop and empties both on every refusal. This interface does not restate that
 * as a check; it is the bridge's guarantee and a second enforcement here would be a second
 * place it could be got wrong.
 */
struct STRATPLAY_API IStratPathQuery
{
	virtual ~IStratPathQuery() = default;

	/**
	 * §2.11.1's route for one unit to one goal hex. `false` means the question was
	 * malformed or the bridge could not answer it; an empty `OutRouteHexes` with `true`
	 * means the module answered "no route", which is an ordinary board fact.
	 */
	virtual bool PathTo(int32 UnitId, FIntPoint GoalHex,
	                    TArray<FIntPoint>& OutRouteHexes,
	                    TArray<int32>&     OutRouteCosts,
	                    int32&             OutTotalCost,
	                    FString&           OutFailureReason) const = 0;
};

/**
 * The one production implementation: `FStratBridge::MovePathToHex`, forwarded.
 *
 * A NAMED TYPE RATHER THAN A LAMBDA AT THE CALL SITE, for `FStratBridgeRulesQuery`'s and
 * `FStratBridgeForecastQuery`'s reason: a gate can construct one against a seeded bridge and
 * assert that what it hands back IS the bridge's own answer, which is unassertable if the
 * forwarding exists only inside a controller method.
 *
 * BORROWS AND NEVER OWNS. The bridge is the match subsystem's; outliving it is the caller's
 * problem to avoid and not this object's to solve with a copy of a `strat::GameState`.
 */
struct STRATPLAY_API FStratBridgePathQuery final : public IStratPathQuery
{
	explicit FStratBridgePathQuery(const FStratBridge* InBridge)
		: Bridge(InBridge)
	{
	}

	virtual bool PathTo(int32 UnitId, FIntPoint GoalHex,
	                    TArray<FIntPoint>& OutRouteHexes,
	                    TArray<int32>&     OutRouteCosts,
	                    int32&             OutTotalCost,
	                    FString&           OutFailureReason) const override;

private:
	const FStratBridge* Bridge = nullptr;
};

/**
 * Writes §2.11.1's preview onto a model somebody else built.
 *
 * UNCONDITIONAL, on `FStratHoverState::DecorateViewModel`'s and `StratDecorateForecast`'s
 * rule and for their reason: it writes `FStratViewModel::PathPreview` on EVERY call,
 * including every path that produces no route, so a decorated model is a complete statement
 * and never a difference from a previous one. A decorator that wrote only on the success
 * path would leave a previous frame's route standing in any model built by a caller that
 * never hovered -- an AI turn's reconcile, a hand-over, a gate -- and the preview would hang
 * on screen pointing at a hex the cursor left, for a unit that is no longer selected.
 *
 * IT MUST RUN AFTER `FStratHoverState::DecorateViewModel`, the same one-directional
 * constraint `StratDecorateForecast` carries: it READS `Model.Hover` and writes nothing the
 * hover reads. Run before it, the route is composed against last frame's hex and is
 * silently one mouse-move stale -- the failure that looks like a latency bug and is a
 * sequencing one. It has NO constraint against the forecast decorator: the two touch no
 * common field and ask the bridge different questions.
 *
 * THE FIVE PATHS THAT PRODUCE NO ROUTE, and none of them is a fault:
 *   - nothing is selected (`UnitId == INDEX_NONE`);
 *   - the cursor is over no hex (`Model.Hover.bHasHoveredHex` false);
 *   - the selected unit has already moved this turn;
 *   - the module answers that there is no route -- out of the §2.4 allowance, blocked by a
 *     unit, or onto impassable terrain. This is the commonest by far and is what makes the
 *     preview appear over reachable hexes only;
 *   - the query refused, which before `StartMatch` finishes is the ordinary state.
 *
 * THE MOVE CHECK IS READ OFF THE MODEL AND MIRRORS THE SELECTION MACHINE'S, and is named
 * here because it is the one gate in this file that is not the rules module's own.
 * `strat::reachable` and `strat::findPath` do not consult `hasMoved` -- they answer about
 * §2.5's cost model and not about the turn -- so a unit that has already moved still gets a
 * route, and previewing a command `FStratSelectionMachine::HandleEvent` will refuse is
 * worse than previewing none. The bit read is `FStratUnitView::bHasMoved`, the RULES-side
 * bit. Deliberately NOT `bDone`: that is the engine's presentation bit and a unit can reach
 * it with its move unspent, which would hide a preview for a move §2.5 permits. This is
 * `StratDecorateForecast`'s `bHasActed` gate, one verb over, and the split between the two
 * bits is `FStratUnitView`'s own.
 *
 * IT TOUCHES NO OTHER FIELD of the model. It reads `Hover` and `Units` and writes
 * `PathPreview`, and nothing else in either direction.
 *
 * IT PERFORMS NO ARITHMETIC. The arrays are copied element for element and `TotalCost` is
 * assigned from the bridge's scalar; nothing here sums the ticks, subtracts two of them, or
 * compares a cost to an allowance. `FStratPathPreviewView`'s own block states the same of
 * the struct, and `StratViewModel.h`'s no-arithmetic census is unmoved by either.
 *
 * A ROUTE OF A SINGLE HEX IS A ROUTE AND IS NOT SUPPRESSED HERE. Hovering the selected
 * unit's own hex answers `{start}` at cost 0 -- the null move, which `Move.h` is explicit is
 * reachable -- and this decorator publishes it. Whether the drawing layer renders anything
 * for a one-hex route is the drawing layer's call; hiding it here would make the model
 * disagree with the module about what is reachable, which is the exact drift T-UI-02 is
 * about.
 *
 * @param Model            the model to decorate. `Hover` and `Units` are read;
 *                         `PathPreview` is written, on every path.
 * @param UnitId           the selection, or `INDEX_NONE`. `FStratSelectionMachine::GetSelectedUnitId`.
 * @param Query            the rules answer. Consulted at most once per call.
 * @param OutFailureReason set only when the query refused; untouched otherwise.
 * @return true when a route was published.
 */
STRATPLAY_API bool StratDecoratePathPreview(FStratViewModel&       Model,
                                            int32                  UnitId,
                                            const IStratPathQuery& Query,
                                            FString&               OutFailureReason);
