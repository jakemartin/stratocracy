// TEST-ONLY. A concrete `UStratOptionsWidget` that records what it is told was committed.
//
// WHY IT EXISTS AT ALL. `UStratOptionsWidget` is `UCLASS(Abstract, Blueprintable)` -- the
// shipped occupant is a `WBP_` asset the editor lane owns and which no headless run may depend
// on -- so a clause cannot construct one. The double supplies the one thing the base class is
// missing: concreteness. Same shape and same reason as
// `Source/StratUI/Tests/StratProductionMenuHostDouble.h`.
//
// IT ADDS ONE THING THAT DOUBLE DELIBERATELY DOES NOT, AND THE DIFFERENCE IS ARGUED RATHER THAN
// COPIED. `StratProductionMenuHostDouble.h` states the rule -- a double that keeps its own
// bookkeeping lets a clause go green comparing the double against itself. That rule binds a
// double standing in for an object whose OWN state is the subject. Here the subject is a
// BROADCAST: `FStratAudioOptionsCommitted` is a dynamic multicast delegate, and a dynamic
// delegate can only be bound to a `UFUNCTION` on a `UObject`. There is no way to observe a
// broadcast without an observer. So the counter below is not a second copy of the widget's
// state -- it is the receiving end of the thing being measured, and every clause still reads
// the widget's own `Model` for the values.
//
// IT OVERRIDES NOTHING ON THE BASE CLASS. In particular it does not and cannot override
// `OnAudioOptionsRefreshed`, which is a `BlueprintImplementableEvent`: that function has no C++
// body to extend, and with no Blueprint under it the call is a no-op. What a refresh DID is
// therefore observed through `UStratOptionsWidget::Model`, which the widget writes before it
// fires the event, and the firing itself is not pinned by any clause here -- said in the
// clause file too, where a reader looking for it will be.
//
// THIS HEADER IS NOT INCLUDABLE FROM StratPlay and must not be made so. It reaches
// `Blueprint/UserWidget.h` through `StratOptionsWidget.h`, and `UMG` is a PRIVATE dependency of
// StratUI -- `StratGuidanceRouteProbe.h` records the ~60 `LNK2019` that measurement cost.
#pragma once

#include "CoreMinimal.h"

#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

#include "StratOptionsWidget.h"

#include "StratOptionsWidgetDouble.generated.h"

/** Concrete `UStratOptionsWidget` for automation only. Holds a commit tally and nothing else. */
UCLASS()
class UStratOptionsWidgetDouble : public UStratOptionsWidget
{
	GENERATED_BODY()

public:
	/** Bind with `OnAudioOptionsCommitted.AddDynamic(Double, &UStratOptionsWidgetDouble::
	 *  HandleCommitted)`. A dynamic multicast delegate cannot bind a lambda or a plain
	 *  function, which is the whole reason this class carries a member at all. */
	UFUNCTION()
	void HandleCommitted(const FStratAudioOptionsModel& InModel)
	{
		++CommitCount;
		LastCommitted = InModel;
	}

	/** How many times the widget has broadcast a commit to this observer. */
	int32 CommitCount = 0;

	/** The model carried by the most recent broadcast. Meaningless while `CommitCount` is 0,
	 *  and every clause checks the count before reading it. */
	UPROPERTY()
	FStratAudioOptionsModel LastCommitted;

	/** Bind with `OnOptionsDismissed.AddDynamic(Double, &UStratOptionsWidgetDouble::
	 *  HandleDismissed)`. Same reason as `HandleCommitted`: a dynamic multicast needs a
	 *  `UFUNCTION` on a `UObject` and there is no lighter observer. */
	UFUNCTION()
	void HandleDismissed()
	{
		++DismissCount;
	}

	/** How many times the widget has broadcast `OnOptionsDismissed` to this observer. */
	int32 DismissCount = 0;

	// ---- THE BOUND SUB-WIDGETS, PLANTED ------------------------------------------------
	//
	// WHY A PLANT AND NOT AN ASSET. `BindWidget` is resolved by reflection when a
	// `UWidgetBlueprint` COMPILES; a native subclass constructed by `NewObject` has no widget
	// tree, so every one of the seven pointers is null and every bind inside `NativeConstruct`
	// is skipped -- which the base class's own header says is safe and which is exactly why a
	// headless clause sees NOTHING of this surface without help. Planting the sub-widgets is
	// what the Blueprint compiler would have done, done by hand, and it is the only route to
	// the handlers: `HandleMasterSliderChanged` and its siblings are `private` and are reachable
	// only THROUGH the delegate the plant makes bindable.
	//
	// IT IS A PLANT AND NOT A SECOND BOOKKEEPING COPY, which is the line
	// `StratProductionMenuHostDouble.h` draws. Nothing here records a volume, renders a percent
	// or decides anything; the objects below are the real UMG types the shipped asset supplies,
	// handed to the real class, and every value a clause asserts is read back off
	// `UStratOptionsWidget::Model` or off the planted widget the SHIPPED code wrote to.

	/** Fills all seven `BindWidget`/`BindWidgetOptional` members with real UMG objects.
	 *  Call BEFORE `RunNativeConstruct`, which is what binds the four controls. */
	void PlantBoundWidgets()
	{
		MasterSlider = NewObject<USlider>(this);
		SfxSlider    = NewObject<USlider>(this);
		MusicSlider  = NewObject<USlider>(this);
		BackButton   = NewObject<UButton>(this);

		MasterValueText = NewObject<UTextBlock>(this);
		SfxValueText    = NewObject<UTextBlock>(this);
		MusicValueText  = NewObject<UTextBlock>(this);
	}

	/** True only when all seven plants took. A clause that finds this false reports a FIXTURE
	 *  failure and never a clause result -- a null plant silently skips the bind it was for. */
	bool AreBoundWidgetsPlanted() const
	{
		return MasterSlider != nullptr && SfxSlider != nullptr && MusicSlider != nullptr
			&& BackButton != nullptr && MasterValueText != nullptr && SfxValueText != nullptr
			&& MusicValueText != nullptr;
	}

	/** `NativeConstruct` is `protected` on the base. Exposed rather than reimplemented: the
	 *  binding this drives IS the subject, so a clause must run the shipped body. */
	void RunNativeConstruct() { NativeConstruct(); }

	/** `NativeDestruct` likewise, for the clause that asks whether the binds come back off. */
	void RunNativeDestruct() { NativeDestruct(); }

	USlider*   Master()     const { return MasterSlider; }
	USlider*   Sfx()        const { return SfxSlider; }
	USlider*   Music()      const { return MusicSlider; }
	UButton*   Back()       const { return BackButton; }
	UTextBlock* MasterText() const { return MasterValueText; }
	UTextBlock* SfxText()    const { return SfxValueText; }
	UTextBlock* MusicText()  const { return MusicValueText; }
};
