// GDD §2.11.2 / §2.11.5 -- see StratCommandBarWidget.h for why this class exists at all.
//
// THIS FILE IS DELIBERATELY THIS SMALL, and its size is the acceptance criterion rather than
// an accident of an early wave, exactly as `StratInfoPanelWidget.cpp` and
// `StratGuidanceWidget.cpp` each record for their own surface. Everything the command bar
// shows -- whether BUILD is on screen, which factory it is about, whether END TURN is
// highlighted, whether it is dimmed, what its hover says -- is decided in
// `StratDecorateCommandBar` and arrives here finished. The moment a branch appears below that
// asks whether a factory is owned, or counts a unit, or decides whether the bar should be
// visible, this file has become a second author of §2.11.2 and the projection has stopped
// being the single description of the screen.
//
// THERE IS NOT EVEN AN `if` IN THIS FILE, which is one step further than the info panel, and
// the reason is the one its header states: nothing viewer-relative is left unresolved, so
// there is no comparison for a compose step to make and no compose step to hold it.
//
// NO `StratBridge.h` INCLUDE HERE EITHER. A .cpp may include it; this one does not, because
// this class has no question to ask the rules module. A rules answer reaching the command bar
// through this file would bypass the view model the bar is supposed to be a projection of.
#include "StratCommandBarWidget.h"

void UStratCommandBarWidget::PushCommandBar(const FStratCommandBarView& InCommandBar)
{
	// A PLAIN COPY AND NOT A `MoveTemp`, for `UStratInfoPanelWidget::PushInfoPanel`'s stated
	// reason: the caller owns the value -- `AStratScoreboardHUD` caches it as
	// `LastPushedCommandBar` so a bar created mid-session can be brought up to it -- so
	// moving out of it would take something the push site still has a use for.
	Model = InCommandBar;

	// Fired AFTER the assignment, so a graph reading `Model` off the widget and a graph
	// reading the event argument see the same values in the same frame.
	OnCommandBarRefreshed(Model);
}
