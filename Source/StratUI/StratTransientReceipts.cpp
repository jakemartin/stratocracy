// Implementation of §2.11.2's transient layer. The header carries the reasoning; this file
// carries the three functions and nothing else.
//
// THERE IS ONE SUBTRACTION IN THIS FILE and it is the `KillFame` amount. Everything else is
// a copy, a comparison, or an index bound. That is the property to check first if this file
// ever grows: arithmetic here is presentation-layer arithmetic, and the header's argument for
// why the one subtraction is allowed does not extend to a second one.

#include "StratTransientReceipts.h"

#include "StratViewModel.h"

FStratReceiptMark StratMarkFromView(const FStratViewModel& Model)
{
	FStratReceiptMark Mark;

	Mark.bSeeded         = true;
	Mark.Turn            = Model.Match.Turn;
	Mark.SideToMove      = Model.Match.SideToMove;
	Mark.bMatchHadResult = Model.Match.bHasResult;
	Mark.ViewingSide     = Model.ViewingSide;

	Mark.FameCombat.Reserve(Model.Sides.Num());
	Mark.IncomePerTurn.Reserve(Model.Sides.Num());

	for (const FStratSideView& Side : Model.Sides)
	{
		// COPIED FIELD FOR FIELD, IN `Sides` ORDER, AND NEVER KEYED BY ANYTHING ELSE. The
		// mark's index IS the side index because `FStratViewModel::Sides` is indexed that way
		// and `UStratViewModelLibrary::GetSideView` reads it that way. A mark that sorted or
		// filtered would put one side's fame under another side's index, and the resulting
		// receipt would be arithmetic between two different players.
		Mark.FameCombat.Add(Side.FameCombat);
		Mark.IncomePerTurn.Add(Side.IncomePerTurn);
	}

	return Mark;
}

EStratTurnBanner StratDecideTurnBanner(int32 SideToMove, int32 ViewingSide, bool bMatchHasResult)
{
	// THE RESULT SILENCES THE BANNER FIRST, before any comparison of the two sides. A
	// concluded match still carries a `sideToMove` -- the rules do not blank it -- so testing
	// the sides first would announce a turn for a match that has ended.
	if (bMatchHasResult)
	{
		return EStratTurnBanner::None;
	}

	if (SideToMove < 0)
	{
		return EStratTurnBanner::None;
	}

	// THE WHOLE OF THE VIEWER-RELATIVE MAPPING, in one comparison, in one place. See the enum
	// doc for why it may not happen anywhere else.
	return (SideToMove == ViewingSide) ? EStratTurnBanner::YourTurn : EStratTurnBanner::EnemyTurn;
}

void StratDecideTransientReceipts(const FStratReceiptMark& Mark,
                                  const FStratViewModel& Model,
                                  FStratTransientReceipts& OutReceipts)
{
	// OVERWRITTEN WHOLE, NEVER APPENDED TO. The output is a statement about THIS refresh, and
	// a caller that reuses the same struct across refreshes must not inherit the last one's
	// receipts. `Reset` rather than `Empty` keeps the allocation, which is the only reason the
	// caller can hold this by value on a subsystem without churning memory every frame.
	OutReceipts.Receipts.Reset();
	OutReceipts.bBannerJustChanged = false;

	// ---- 1. The banner. Decided from the model alone, on every call. -------------------
	OutReceipts.Banner = StratDecideTurnBanner(Model.Match.SideToMove,
	                                           Model.ViewingSide,
	                                           Model.Match.bHasResult);

	OutReceipts.BannerSide = (OutReceipts.Banner == EStratTurnBanner::None)
		? INDEX_NONE
		: Model.Match.SideToMove;

	// ---- 2. No mark, no edges. --------------------------------------------------------
	if (!Mark.bSeeded)
	{
		return;
	}

	// The banner's own edge, asked in the banner's terms rather than in the turn's. It is
	// asked HERE rather than beside the banner above because it needs the mark, and the mark
	// is exactly what step 2 has just established exists.
	//
	// EVERY ARGUMENT COMES FROM THE MARK AND NONE FROM THE MODEL. Mixing the two asks "what
	// would the banner have been if the old side were to move in the new match", which is a
	// question about no frame that ever existed -- see `FStratReceiptMark::bMatchHadResult`
	// for the two bugs that mixing produced.
	const EStratTurnBanner BannerAtMark = StratDecideTurnBanner(Mark.SideToMove,
	                                                           Mark.ViewingSide,
	                                                           Mark.bMatchHadResult);

	OutReceipts.bBannerJustChanged = (BannerAtMark != OutReceipts.Banner);

	// ---- 3. Kill receipts, per side, in `Sides` order. --------------------------------
	for (int32 Side = 0; Side < Model.Sides.Num(); ++Side)
	{
		if (!Mark.FameCombat.IsValidIndex(Side))
		{
			// A side with no previous reading. See the header's rule 3.
			continue;
		}

		const int32 Before = Mark.FameCombat[Side];
		const int32 After  = Model.Sides[Side].FameCombat;

		// STRICTLY GREATER, SO A FALL AND A HOLD ARE BOTH SILENT. The header states why a
		// fall is not a fault here.
		if (After <= Before)
		{
			continue;
		}

		FStratReceiptView& Receipt = OutReceipts.Receipts.AddDefaulted_GetRef();

		Receipt.Kind          = EStratReceiptKind::KillFame;
		Receipt.Side          = Side;
		Receipt.Turn          = Model.Match.Turn;
		Receipt.DurableBefore = Before;
		Receipt.DurableAfter  = After;
		Receipt.Amount        = After - Before;   // THE ONE SUBTRACTION. See the file block.
	}

	// ---- 4. The income receipt, for the side now to move. -----------------------------
	// THE EDGE IS THE PAIR AND NOT EITHER HALF. See `FStratReceiptMark::SideToMove`.
	const bool bTurnStarted = (Model.Match.Turn != Mark.Turn)
	                       || (Model.Match.SideToMove != Mark.SideToMove);

	if (!bTurnStarted)
	{
		return;
	}

	const int32 Side = Model.Match.SideToMove;

	if (!Model.Sides.IsValidIndex(Side))
	{
		// A `sideToMove` with no side view is a projection this file cannot describe, and
		// inventing a zero-income receipt for it would put a number on screen that no module
		// produced. Silence is the only honest answer.
		return;
	}

	FStratReceiptView& Receipt = OutReceipts.Receipts.AddDefaulted_GetRef();

	Receipt.Kind = EStratReceiptKind::IncomeRate;
	Receipt.Side = Side;
	Receipt.Turn = Model.Match.Turn;

	// VERBATIM, AND THE ASSIGNMENT IS THE POINT. `Amount` is the module's field with nothing
	// done to it -- `DurableBefore` exists so a clause can see that the rate at the mark was
	// available and was NOT subtracted. If these two lines ever become one expression over
	// both, the income arm has become a delta and the header's argument no longer covers it.
	Receipt.DurableBefore = Mark.IncomePerTurn.IsValidIndex(Side) ? Mark.IncomePerTurn[Side] : 0;
	Receipt.DurableAfter  = Model.Sides[Side].IncomePerTurn;
	Receipt.Amount        = Model.Sides[Side].IncomePerTurn;
}
