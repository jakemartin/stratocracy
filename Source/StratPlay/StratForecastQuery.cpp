// GDD §2.11.3 -- the forecast card's producer, implementation.
//
// IT NAMES NO `strat::` TYPE AND CALLS NO `strat::` FUNCTION. `FStratAttackForecast` is a
// plain engine-typed struct declared in `StratBridge.h`; every number on it was produced
// inside `StratBridge`, which is the only module where the vendored symbols link. A
// `strat::` call from here is `LNK2019` -- measured 8x when the bridge first tried, and
// again as a single one when a StratUI test called `strat::uiCheckSnapshotFidelity`
// directly.
//
// IT SHAPES NOTHING EITHER. The copy into the reflected card and §2.11.3's three
// presentation shapings are `StratComposeForecastView`'s, in `StratUI`, beside the struct
// they fill and inside the reach of that header's own no-arithmetic census. What is left
// here is the composition of two facts -- a hover and a selection -- and one call.

#include "StratForecastQuery.h"

#include "StratBridge.h"
#include "StratViewModel.h"

bool FStratBridgeForecastQuery::ForecastAgainst(int32 AttackerId, FIntPoint DefenderHex,
                                                FStratAttackForecast& OutForecast,
                                                FString&              OutFailureReason) const
{
	if (Bridge == nullptr)
	{
		// THE ORDINARY STATE BEFORE `StartMatch`, and a refusal rather than an empty card
		// for `FStratSelectionMachine::BuildOverlays`' reason: a caller cannot tell a
		// "there is no bridge" from a "there is no target" if both come back the same way.
		OutFailureReason = TEXT("no bridge");
		return false;
	}

	const FStratResult Asked = Bridge->AttackForecast(AttackerId, DefenderHex, OutForecast);
	if (!Asked.bOk)
	{
		// THE BRIDGE'S OWN WORDS, forwarded and not rephrased. Its two-channel block is what
		// makes this narrow: an illegal target is NOT a refusal and does not arrive here.
		OutFailureReason = Asked.Reason;
		return false;
	}

	return true;
}

bool StratDecorateForecast(FStratViewModel&           Model,
                           int32                      AttackerUnitId,
                           const IStratForecastQuery& Query,
                           FString&                   OutFailureReason)
{
	// UNCONDITIONAL AND FIRST. The model's card is cleared before anything can return, so
	// every path below -- including every early one -- leaves a complete statement rather
	// than last frame's forecast. See the declaration on why a decorator that wrote only on
	// the success path would hang a card over a hex the cursor had left.
	Model.Forecast = FStratForecastView();

	if (AttackerUnitId == INDEX_NONE)
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

	// THE ACT GATE, MIRRORING `FStratSelectionMachine`'S AND NOT INVENTING ONE. See the
	// declaration: `strat::uiForecast` answers about §2.6 and not about the turn, so it
	// calls an already-spent unit's attack legal, and a commit card for a command the
	// selection machine will refuse is worse than no card. `bHasActed` is the RULES-side
	// bit, the same one that guard reads; `bDone` is the engine's and would hide cards for
	// attacks §2.6 permits.
	//
	// A SELECTION NAMING NO UNIT IN THE MODEL PRODUCES NO CARD, and that is the same
	// arrangement as a stale id: the model is the statement of what is on the board, and a
	// forecast for a unit it does not list is a forecast about nothing.
	const FStratUnitView* Attacker = nullptr;
	for (const FStratUnitView& U : Model.Units)
	{
		if (U.UnitId == AttackerUnitId)
		{
			Attacker = &U;
			break;
		}
	}

	if (Attacker == nullptr || Attacker->bHasActed)
	{
		return false;
	}

	FStratAttackForecast Answered;
	if (!Query.ForecastAgainst(AttackerUnitId, Model.Hover.HoveredHex, Answered, OutFailureReason))
	{
		return false;
	}

	// THE COMPOSE DECIDES WHETHER THERE IS A CARD, off `bLegal`, and this function does not
	// second-guess it. Calling it on the illegal path too rather than returning early is
	// deliberate: there is then exactly ONE writer of `Model.Forecast` on every path that
	// reaches the query, and no way for the two to disagree about what an empty card is.
	StratComposeForecastView(Answered, Model.Forecast);

	return Model.Forecast.bHasForecast;
}
