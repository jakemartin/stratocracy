// GDD §2.11.1 -- the path preview's producer, implementation.
//
// IT NAMES NO `strat::` TYPE AND CALLS NO `strat::` FUNCTION. The route arrives as
// `TArray<FIntPoint>` and the ticks as `TArray<int32>`, produced inside `StratBridge`, which
// is the only module where the vendored symbols link. A `strat::` call from here is
// `LNK2019` -- measured 8x when the bridge first tried, and again as a single one when a
// StratUI test called `strat::uiCheckSnapshotFidelity` directly.
//
// IT SHAPES NOTHING. There is no compose step in `StratUI` for this field, unlike
// `StratComposeForecastView`, and that absence is deliberate rather than an omission: the
// forecast has three presentation shapings to perform and this has zero -- every field of
// `FStratPathPreviewView` is one bridge output copied across. A compose function here would
// be a second file with nothing in it, and the next reader would go looking for the shaping
// it does not do. The day §2.11.1 needs one -- a per-tick style, a truncated route -- it
// lands in `StratUI` beside the struct, on `StratComposeForecastView`'s precedent, and this
// file forwards to it.

#include "StratPathPreviewQuery.h"

#include "StratBridge.h"
#include "StratViewModel.h"

bool FStratBridgePathQuery::PathTo(int32 UnitId, FIntPoint GoalHex,
                                   TArray<FIntPoint>& OutRouteHexes,
                                   TArray<int32>&     OutRouteCosts,
                                   int32&             OutTotalCost,
                                   FString&           OutFailureReason) const
{
	if (Bridge == nullptr)
	{
		// THE ORDINARY STATE BEFORE `StartMatch`, and a refusal rather than an empty route
		// for `FStratBridgeForecastQuery`'s reason: a caller cannot tell "there is no
		// bridge" from "there is no route" if both come back the same way.
		OutFailureReason = TEXT("no bridge");
		return false;
	}

	const FStratResult Asked = Bridge->MovePathToHex(UnitId, GoalHex,
	                                                 OutRouteHexes, OutRouteCosts, OutTotalCost);
	if (!Asked.bOk)
	{
		// THE BRIDGE'S OWN WORDS, forwarded and not rephrased. Its two-channel block is what
		// makes this narrow: an unreachable goal is NOT a refusal and does not arrive here.
		OutFailureReason = Asked.Reason;
		return false;
	}

	return true;
}

bool StratDecoratePathPreview(FStratViewModel&       Model,
                              int32                  UnitId,
                              const IStratPathQuery& Query,
                              FString&               OutFailureReason)
{
	// UNCONDITIONAL AND FIRST. The model's preview is cleared before anything can return, so
	// every path below -- including every early one -- leaves a complete statement rather
	// than last frame's route. See the declaration on why a decorator that wrote only on the
	// success path would hang a route over a hex the cursor had left.
	Model.PathPreview = FStratPathPreviewView();

	if (UnitId == INDEX_NONE)
	{
		return false;
	}

	if (!Model.Hover.bHasHoveredHex)
	{
		// READ FIRST, ALWAYS. `FStratHoverView`'s own block: there is no hex value that
		// cannot be a real hex, so `HoveredHex` means nothing until this flag is checked,
		// and (0,0) is an ordinary board corner on *Ferrum Crossing*.
		return false;
	}

	// THE MOVE GATE, MIRRORING `FStratSelectionMachine`'S AND NOT INVENTING ONE. See the
	// declaration: §2.5's cost model does not consult `hasMoved`, so an already-moved unit
	// still gets a route, and a preview of a command the selection machine will refuse is
	// worse than no preview. `bHasMoved` is the RULES-side bit; `bDone` is the engine's and
	// would hide previews for moves §2.5 permits.
	//
	// A SELECTION NAMING NO UNIT IN THE MODEL PRODUCES NO PREVIEW, and that is the same
	// arrangement as a stale id: the model is the statement of what is on the board, and a
	// route for a unit it does not list is a route about nothing.
	const FStratUnitView* Selected = nullptr;
	for (const FStratUnitView& U : Model.Units)
	{
		if (U.UnitId == UnitId)
		{
			Selected = &U;
			break;
		}
	}

	if (Selected == nullptr || Selected->bHasMoved)
	{
		return false;
	}

	// ASKED INTO LOCALS AND MOVED ACROSS ON SUCCESS, on `StratBuildViewModel`'s
	// all-or-nothing construction. The bridge does empty both arrays on every refusal, so
	// writing through to the model directly would be sound today -- the locals are here so
	// that it stays sound if that ever stops being true, and so that this function has
	// exactly one writer of `Model.PathPreview` after the clear above.
	TArray<FIntPoint> RouteHexes;
	TArray<int32>     RouteCosts;
	int32             TotalCost = 0;

	if (!Query.PathTo(UnitId, Model.Hover.HoveredHex, RouteHexes, RouteCosts, TotalCost,
	                  OutFailureReason))
	{
		return false;
	}

	if (RouteHexes.IsEmpty())
	{
		// THE MODULE'S ANSWER, NOT A FAULT. The goal is out of the unit's §2.4 allowance,
		// blocked by a unit, or impassable. No preview, no failure reason, and
		// `OutFailureReason` is left exactly as the caller passed it.
		return false;
	}

	// COPIED, NOT SHAPED. `TotalCost` is `findPath`'s own `outCost` and is assigned rather
	// than recomputed from `RouteCosts`; the bridge has already refused any board on which
	// those two disagree. Nothing in these five lines adds, subtracts or compares two
	// numbers.
	Model.PathPreview.bHasPath   = true;
	Model.PathPreview.RouteHexes = MoveTemp(RouteHexes);
	Model.PathPreview.RouteCosts = MoveTemp(RouteCosts);
	Model.PathPreview.TotalCost  = TotalCost;
	Model.PathPreview.UnitId     = UnitId;

	return true;
}
