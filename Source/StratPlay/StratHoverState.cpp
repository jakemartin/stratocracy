// GDD §2.11.3 / §2.11.2 / T-UI-01 -- the hovered hex, implementation.
//
// THIS FILE DOES NOT INCLUDE `StratBridge.h` AND HAS NO REASON TO. It asks the rules module
// nothing: a hover is a statement about the cursor, and there is no rules question whose
// answer is "which hex is the mouse over". The forecast wave will ask `FStratBridge::Forecast`
// about the hex this file carries, and it will ask it from somewhere else.
//
// SHORT FUNCTIONS THROUGHOUT, AND THE LENGTH IS THE MEASURE. Everything that could have been
// decided here -- what counts as a hex, when a hover begins, whether a refresh is warranted --
// is decided by the caller or by the board, on purpose.

#include "StratHoverState.h"

#include "StratViewModel.h"

bool FStratHoverState::SetHoveredHex(FIntPoint Hex)
{
	// THE CHANGE TEST COMES FIRST AND READS BOTH FIELDS. Comparing only the coordinate would
	// report "no change" when the cursor re-enters the board on the same hex it left, which is
	// the exact sequence a player produces by dragging the cursor across a unit's own mesh --
	// `HexUnderCursor` answers false over that mesh -- and back onto the tile underneath it.
	// The card would then stay dark until the player moved to a different tile.
	const bool bChanged = !bHasHoveredHex || HoveredHex != Hex;

	HoveredHex      = Hex;
	bHasHoveredHex  = true;

	return bChanged;
}

bool FStratHoverState::ClearHoveredHex()
{
	const bool bChanged = bHasHoveredHex;

	HoveredHex     = FIntPoint::ZeroValue;
	bHasHoveredHex = false;

	return bChanged;
}

bool FStratHoverState::GetHoveredHex(FIntPoint& OutHex) const
{
	OutHex = bHasHoveredHex ? HoveredHex : FIntPoint::ZeroValue;
	return bHasHoveredHex;
}

void FStratHoverState::DecorateViewModel(FStratViewModel& Model) const
{
	// BOTH FIELDS, EVERY CALL. See the declaration on why the not-hovering case is written
	// rather than skipped.
	Model.Hover.bHasHoveredHex = bHasHoveredHex;
	Model.Hover.HoveredHex     = bHasHoveredHex ? HoveredHex : FIntPoint::ZeroValue;
}
