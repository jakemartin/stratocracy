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
//   RETRACTED> 2026-09-05: "THERE IS NOT EVEN AN `if` IN THIS FILE". There are now two, both
//   `if (OptionsButton != nullptr)`, and the sentence is corrected rather than deleted because
//   a reader arriving at it by a citation is entitled to learn what it stopped covering. THE
//   CLAIM IT WAS MAKING IS UNCHANGED AND IS NOW MADE IN THE NARROWER WORDS IT ALWAYS MEANT:
//   there is no branch in this file THAT LOOKS AT A VIEW-MODEL VALUE. The two new guards test a
//   sub-widget POINTER for null and are required by a fact about C++ rather than a fact about
//   §2.11.2 -- `UStratOptionsWidget::NativeConstruct` states it in full: `BindWidget` is
//   enforced by the Widget Blueprint compiler and says nothing about a native subclass, so an
//   unguarded `AddDynamic` crashes any clause that constructs one directly, and this member is
//   `BindWidgetOptional` besides, so null is its ordinary state on the shipped asset today.
//   The moment a branch here asks whether a factory is owned or whether the bar should be
//   visible, the paragraph above is being violated; these two do not.
//
// NO `StratBridge.h` INCLUDE HERE EITHER. A .cpp may include it; this one does not, because
// this class has no question to ask the rules module. A rules answer reaching the command bar
// through this file would bypass the view model the bar is supposed to be a projection of.
#include "StratCommandBarWidget.h"

// IWYU: this file calls `UButton::OnClicked`, which the header only forward declares.
#include "Components/Button.h"

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

void UStratCommandBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (OptionsButton != nullptr)
	{
		OptionsButton->OnClicked.AddDynamic(this, &UStratCommandBarWidget::HandleOptionsClicked);
	}
}

void UStratCommandBarWidget::NativeDestruct()
{
	// UNBOUND IN THE SAME SHAPE IT WAS BOUND, on `UStratOptionsWidget::NativeDestruct`'s stated
	// reading: `RemoveDynamic` on an unbound delegate is a no-op, so the guard here is about
	// the POINTER and not about the binding.
	if (OptionsButton != nullptr)
	{
		OptionsButton->OnClicked.RemoveDynamic(this, &UStratCommandBarWidget::HandleOptionsClicked);
	}

	Super::NativeDestruct();
}

void UStratCommandBarWidget::HandleOptionsClicked()
{
	// ONE LINE, AND THE WHOLE OF THE DEPARTURE FROM THIS CLASS'S "NO VERB" CONTRACT IS THAT IT
	// EXISTS AT ALL. It asks nothing, compares nothing and reads no field of `Model`. See the
	// header block for what forced a native bind and for why binding an input is not the thing
	// that contract refuses.
	OnOptionsRequested.Broadcast();
}
