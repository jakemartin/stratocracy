// GDD §2.11.6 -- see StratGuidanceWidget.h for why this class exists at all.
//
// THIS FILE IS DELIBERATELY THIS SMALL, and its size is the acceptance criterion rather
// than an accident of an early phase. Everything §2.11.6-B decides -- which beat is
// outstanding, whether the window-end tag shows, whether End Turn is gated -- is decided
// in `FStratGuidedOpening::DecorateViewModel` and arrives here finished. The moment a
// branch appears below that asks WHICH beat this is, or recomputes a visibility, this
// file has become a second author of the guided opening and the projection has stopped
// being the single description of the screen. There is no `if` on `Guidance` here and
// there must not be one.
//
// NO `StratBridge.h` INCLUDE HERE EITHER, and unlike the header that is a choice rather
// than a constraint: a .cpp may include it. It does not, because this class has no
// question to ask the rules module. A rules answer reaching the strip through this file
// would bypass the view model that the strip is supposed to be a projection of.
#include "StratGuidanceWidget.h"

void UStratGuidanceWidget::PushGuidance(const FStratGuidanceView& InGuidance)
{
	// A PLAIN COPY AND NOT A `MoveTemp`, unlike `UStratScoreboardWidget::Refresh`. That
	// one builds into a local and moves, because a REFUSED build must leave the previous
	// standings on screen and constructing aside is how it gets that by construction.
	// Here the caller owns the value -- it is a field of the model being applied, which
	// the subsystem then caches as `AppliedModel` -- so moving out of it would take
	// something the reconcile point still has a use for.
	Guidance = InGuidance;

	// Fired AFTER the assignment, so a graph reading `Guidance` off the widget and a graph
	// reading the event argument see the same values in the same frame.
	OnGuidanceRefreshed(Guidance);
}
