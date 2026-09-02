// GDD §2.11.5 -- the focused factory, implementation. See StratBuildAffordance.h for why
// this file exists at all.
//
// THIS FILE DOES NOT INCLUDE `StratBridge.h` AND HAS NO REASON TO, on
// `StratHoverState.cpp`'s rule. It asks the rules module nothing: whether a hex holds a
// factory and who owns it were answered by `strat::buildUiSnapshot` and copied onto
// `FStratViewModel::Factories` by `StratBuildViewModel` before any function below runs. A
// `strat::` call from this module does not link in any case -- measured as 8 x `LNK2019`
// when the bridge first tried, recorded in `StratBridge.h`'s own opening block.
//
// SHORT FUNCTIONS THROUGHOUT, AND THE LENGTH IS THE MEASURE, on `StratHoverState.cpp`'s rule
// again. Everything that could have been decided here -- what the button draws, whether END
// TURN is suggested, whether it is dimmed -- is decided in `StratDecorateCommandBar`, in
// StratUI, beside the model whose census governs those fields.
//
// THE ONE LOOKUP IN THIS FILE IS A LOOKUP AND NOT A RULES QUESTION. `FindByPredicate` over
// `Model.Factories` by hex equality is the same shape `StratDecorateInfoPanel` uses to find
// the hovered hex's unit, and `FStratHoverView`'s own block rules on the distinction: a
// consumer that looks a fact up in the model is reading the model, and a consumer that
// derives one is authoring a second copy of it.

#include "StratBuildAffordance.h"

#include "StratViewModel.h"

bool FStratBuildAffordance::NoteHexPrimary(FIntPoint Hex, const FStratViewModel& Model)
{
	// ---- Is `Hex` a factory this seat holds, on a match still running? -------
	//
	// THE MATCH GATE IS FIRST AND IS NOT A RULES QUESTION. `bHasResult` is a field of the
	// model the click was made over; §2.8's finished match opens no menu, so a focus latched
	// on one would advertise a control guaranteed to refuse. See the declaration.
	bool bWantFocus = false;

	if (!Model.Match.bHasResult)
	{
		const FStratFactoryView* const Factory = Model.Factories.FindByPredicate(
			[Hex](const FStratFactoryView& Candidate) { return Candidate.Hex == Hex; });

		// `Owner == Model.ViewingSide` AND NOT `Owner != INDEX_NONE`. The declaration states
		// the one input on which those differ and why `StratBuildViewModel` cannot produce
		// it.
		bWantFocus = (Factory != nullptr) && (Factory->Owner == Model.ViewingSide);
	}

	// ---- One arm, both directions -------------------------------------------
	//
	// NO EARLY RETURN ON THE FALSE PATH. A click that is not on a viewing-side factory
	// CLEARS, and it clears through the same call the latch goes through, so there is no
	// path on which one runs and the other is forgotten. That is the whole of what makes
	// "the player clicked somewhere else" a clear point without any clear-point code.
	if (!bWantFocus)
	{
		return ClearFocus();
	}

	// THE CHANGE TEST READS BOTH FIELDS, on `FStratHoverState::SetHoveredHex`'s reasoning:
	// comparing only the coordinate would report "no change" for the first click on the hex
	// a previous focus had been cleared from.
	const bool bChanged = !bHasFocusedHex || FocusedHex != Hex;

	FocusedHex     = Hex;
	bHasFocusedHex = true;

	return bChanged;
}

bool FStratBuildAffordance::Observe(const FStratViewModel& Model)
{
	// THE FIRST OBSERVATION RECORDS AND CLEARS NOTHING. `INDEX_NONE` in either field means
	// this struct has not seen a model yet, and there is nothing latched on the frame before
	// anything has been clicked; treating it as a change would be a clear with no cause.
	const bool bFirstObservation =
		(LastObservedTurn == INDEX_NONE) || (LastObservedViewingSide == INDEX_NONE);

	// THE TURN CLOCK IS `FStratGuidedOpening::Observe`'S, cited rather than reinvented. See
	// the declaration on why keying on the MODEL'S turn covers every AI turn and correctly
	// does not fire on a gated or refused end turn.
	const bool bTurnChanged = !bFirstObservation && (LastObservedTurn != Model.Match.Turn);
	const bool bSeatChanged = !bFirstObservation && (LastObservedViewingSide != Model.ViewingSide);

	// RECORDED BEFORE THE CLEAR AND UNCONDITIONALLY. The observation fields describe what
	// this struct has SEEN, not what it did about it, so they move on every call including
	// the ones that clear nothing.
	LastObservedTurn        = Model.Match.Turn;
	LastObservedViewingSide = Model.ViewingSide;

	// THREE INDEPENDENT CAUSES, NONE CONFLATED. See the declaration: a hot-seat hand-over is
	// covered twice and an AI turn in a single-seat session only once, which is why the turn
	// arm cannot be dropped as redundant with the seat arm.
	if (bTurnChanged || bSeatChanged || Model.Match.bHasResult)
	{
		return ClearFocus();
	}

	return false;
}

bool FStratBuildAffordance::ClearFocus()
{
	const bool bChanged = bHasFocusedHex;

	FocusedHex     = FIntPoint::ZeroValue;
	bHasFocusedHex = false;

	return bChanged;
}

bool FStratBuildAffordance::GetFocusedHex(FIntPoint& OutHex) const
{
	OutHex = bHasFocusedHex ? FocusedHex : FIntPoint::ZeroValue;
	return bHasFocusedHex;
}

void FStratBuildAffordance::Reset()
{
	// THE OBSERVATION FIELDS GO TOO, WHICH `ClearFocus` DELIBERATELY DOES NOT DO. See the
	// declaration: after a reseed the recorded turn and seat describe a match that is over,
	// and leaving them would make the first `Observe` of the new match see a spurious change.
	*this = FStratBuildAffordance();
}

void FStratBuildAffordance::DecorateViewModel(FStratViewModel& Model) const
{
	// A HEX AND A BOOL, AND EVERY DECISION IS THE DECORATOR'S. See the declaration: the
	// resolution against `Model.Factories` and `Model.ViewingSide` happens on the far side of
	// this call, on every refresh, which is what makes a stale latch undrawable rather than
	// merely unlikely.
	StratDecorateCommandBar(Model, bHasFocusedHex, FocusedHex);
}
