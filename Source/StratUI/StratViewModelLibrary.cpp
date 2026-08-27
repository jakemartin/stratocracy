// §2.11.2's selectors. See `StratViewModelLibrary.h` for why every one of them is a
// function OVER the model rather than a field ON it.
//
// THIS FILE INCLUDES NO `StratBridge.h` AND NAMES NO `strat::` TYPE, which is unusual for
// this module and is worth its line: `StratViewModel.cpp` beside it does both, because it is
// the projection. This file is downstream of the projection and reads only reflected types,
// so it has nothing to ask a bridge and nothing to translate. The `LNK2019` that the
// neighbouring file's header block records -- a `strat::` free function called from StratUI --
// is not reachable from here at all, and it stays that way if nothing below ever grows a
// bridge parameter.

#include "StratViewModelLibrary.h"

bool UStratViewModelLibrary::GetSideView(const FStratViewModel& Model,
                                         int32                  Side,
                                         FStratSideView&        OutSide)
{
	// RESET FIRST, so a refusal cannot leave a previous call's row in the caller's value. The
	// same all-or-nothing discipline `StratBuildViewModel` uses on the model itself, held at
	// the one-struct scale.
	OutSide = FStratSideView();

	if (!Model.Sides.IsValidIndex(Side))
	{
		return false;
	}

	OutSide = Model.Sides[Side];
	return true;
}

bool UStratViewModelLibrary::GetViewingSideView(const FStratViewModel& Model,
                                                FStratSideView&        OutSide)
{
	// THE PAIRING, AND IT IS THE ONLY LINE OF THIS FUNCTION. `ViewingSide` is not re-checked
	// here: `GetSideView` range-checks whatever it is handed, and a second check would be a
	// second author of the same refusal.
	return GetSideView(Model, Model.ViewingSide, OutSide);
}

int32 UStratViewModelLibrary::CountUnitsAbleToAct(const FStratViewModel& Model, int32 Side)
{
	int32 Count = 0;

	for (const FStratUnitView& U : Model.Units)
	{
		if (U.Side != Side)
		{
			continue;
		}

		// §2.11.1's retirement and §2.11.6-B's lock, in that order and as two separate tests.
		// Both are presentation bits written on the decoration seam -- `bDone` by
		// `FStratSelectionMachine::DecorateViewModel`, `bLockedThisTurn` by that decorator from
		// `FStratGuidedOpening`'s call to `SetLockedThisTurn`. An UNDECORATED model therefore
		// has both false for every unit, which is the correct reading of a model built for a
		// hand-over, a gate or an AI turn: nobody has retired anything on it and nothing is
		// locked. That is the same default `StratBuildViewModel` leaves and the same
		// interpretation `FStratHoverView`'s block gives its own.
		if (U.bDone || U.bLockedThisTurn)
		{
			continue;
		}

		// BOTH ALLOWANCES SPENT. `!(A && B)` -- see the declaration for why the shape of this
		// expression is load-bearing and what the two wrong spellings of it would count.
		if (U.bHasMoved && U.bHasActed)
		{
			continue;
		}

		++Count;
	}

	return Count;
}

int32 UStratViewModelLibrary::CountViewingSideUnitsAbleToAct(const FStratViewModel& Model)
{
	return CountUnitsAbleToAct(Model, Model.ViewingSide);
}
