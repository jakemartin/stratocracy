// GDD §2.11.2 -- see StratInfoPanelWidget.h for why this class exists at all.
//
// THIS FILE IS DELIBERATELY THIS SMALL, and its size is the acceptance criterion rather than
// an accident of an early phase, exactly as `StratGuidanceWidget.cpp` records for the strip.
// Everything §2.11.2 shows -- which hex, which terrain row, whether it is passable, whether
// a unit stands there, which §2.4 stats it has, whether it is done -- is decided in
// `StratDecorateInfoPanel` and arrives here finished. The moment a branch appears below that
// asks WHICH terrain this is, or recomputes a stat, or decides whether the panel should be
// visible, this file has become a second author of §2.11.2 and the projection has stopped
// being the single description of the screen.
//
// THE ONE `if` CHAIN IN THIS FILE IS THE ONE THE HEADER SAYS IS HERE, and it reads two
// fields of one value. It is not a rules question and it is not a rendering decision; see
// `StratComposeInfoPanelModel` below.
//
// NO `StratBridge.h` INCLUDE HERE EITHER, and unlike the header that is a choice rather than
// a constraint: a .cpp may include it. It does not, because this class and this function have
// no question to ask the rules module. A rules answer reaching the panel through this file
// would bypass the view model that the panel is supposed to be a projection of, which is the
// second-opinion failure `T-UI-03` names about the scoreboard.
#include "StratInfoPanelWidget.h"

void StratComposeInfoPanelModel(
	const FStratInfoPanelView& Panel,
	int32                      ViewingSide,
	FStratInfoPanelModel&      OutModel)
{
	// BUILT ASIDE AND ASSIGNED ON THE LAST LINE, matching every other builder and composer
	// in this module. Nothing below can fail, so this is not defensive -- it is what keeps
	// a caller's model from being half-filled if a later hand adds a branch here.
	FStratInfoPanelModel Built;

	// THE PANEL CROSSES WHOLE AND UNTOUCHED. No field is re-derived, none is dropped, and
	// none is clamped -- `TerrainDefensePct` is signed by design (`DT_Terrain`'s Bridge row
	// is `-10`) and a clamp here would be this file editing a rules number on its way to a
	// screen.
	Built.Panel       = Panel;
	Built.ViewingSide = ViewingSide;

	// ---- The one comparison, made once, here --------------------------------
	//
	// THE ORDER OF THE THREE QUESTIONS IS LOAD-BEARING.
	//
	// `bHexCapturable` IS ASKED FIRST, AND IT IS NOT `HexOwner != INDEX_NONE`. Those are
	// different questions: a NEUTRAL Factory and a Plains hex both carry `INDEX_NONE`, so
	// deriving capturability from the owner would print `· neutral` beside every Plains hex
	// on the board. `StratDecorateInfoPanel` records that trap in its own comment, against
	// `Ui.h`'s wording, and this is the reader it was recorded for.
	//
	// `bHasHex` IS FOLDED INTO IT RATHER THAN ASKED SEPARATELY, and that is safe by
	// construction rather than by inspection: `StratDecorateInfoPanel` builds `Built` aside
	// from a default-constructed `FStratInfoPanelView` and writes `bHexCapturable` only
	// inside the branch that has already set `bHasHex` true. An unhovered panel therefore
	// carries `bHexCapturable == false` and lands on `NotCapturable`, which is the correct
	// answer for a panel with no hex to own.
	//
	// THE VIEWER COMPARISON IS ASKED LAST, ON A HEX ALREADY KNOWN TO BE CAPTURABLE, so
	// `HexOwner == INDEX_NONE` at that point means exactly one thing -- §2.11.2's `neutral`,
	// held by no side -- and never "this hex has no ownership concept".
	//
	// `ViewingSide` IS NOT RANGE-CHECKED, DELIBERATELY. There is nothing to refuse with: this
	// function is total and returns no `bool`, and every out-of-range value already has a
	// correct answer here. A viewing side that is neither the owner nor a side at all reads
	// the hex as `Enemy`, which is the honest projection of "held by somebody who is not
	// you". The range check that matters lives at the seams that can refuse --
	// `StratBuildScoreboardModel` and `AStratScoreboardHUD::SetViewingSide` -- and a second,
	// silent one here would be this file forming an opinion about a value it was handed.
	if (!Panel.bHexCapturable)
	{
		Built.HexOwnership = EStratHexOwnership::NotCapturable;
	}
	else if (Panel.HexOwner == INDEX_NONE)
	{
		Built.HexOwnership = EStratHexOwnership::Neutral;
	}
	else if (Panel.HexOwner == ViewingSide)
	{
		Built.HexOwnership = EStratHexOwnership::Yours;
	}
	else
	{
		Built.HexOwnership = EStratHexOwnership::Enemy;
	}

	OutModel = Built;
}

void UStratInfoPanelWidget::PushInfoPanel(const FStratInfoPanelModel& InModel)
{
	// A PLAIN COPY AND NOT A `MoveTemp`, for `UStratGuidanceWidget::PushGuidance`'s stated
	// reason: the caller owns the value -- `AStratScoreboardHUD` caches it as
	// `LastPushedInfoPanel` so a strip created mid-session can be brought up to it -- so
	// moving out of it would take something the push site still has a use for.
	Model = InModel;

	// Fired AFTER the assignment, so a graph reading `Model` off the widget and a graph
	// reading the event argument see the same values in the same frame.
	OnInfoPanelRefreshed(Model);
}
