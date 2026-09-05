// TEST-ONLY. A concrete `UStratCommandBarWidget` with a plantable `OptionsButton` and a tally
// of what `OnOptionsRequested` broadcast.
//
// WHY IT EXISTS AT ALL. `UStratCommandBarWidget` is `UCLASS(Abstract, Blueprintable)` -- the
// shipped occupant is `WBP_CommandBar`, which no headless run may depend on -- so a clause
// cannot construct one. Same shape and same reason as `StratOptionsWidgetDouble.h` and
// `StratProductionMenuHostDouble.h`.
//
// THE PLANT IS THE OTHER HALF, AND IT IS NOT BOOKKEEPING. `OptionsButton` is
// `BindWidgetOptional`, resolved by reflection when a `UWidgetBlueprint` COMPILES; a native
// subclass built by `NewObject` has it null and `NativeConstruct`'s bind is skipped. Planting a
// real `UButton` is what the Blueprint compiler would have done, and it is the only route to
// `HandleOptionsClicked`, which is `protected` and reachable solely through the button's own
// `OnClicked`. Nothing here decides anything: the tally below is the RECEIVING END of the
// broadcast under test, exactly as `UStratOptionsWidgetDouble::CommitCount` is, and there is no
// way to observe a dynamic multicast without a `UFUNCTION` on a `UObject`.
//
// LEAVING THE BUTTON UNPLANTED IS ALSO A FIXTURE, and a used one -- the null-button clause runs
// `NativeConstruct` on a double that never called `PlantOptionsButton`.
//
// THIS HEADER IS NOT INCLUDABLE FROM StratPlay and must not be made so, for the reason
// `StratOptionsWidgetDouble.h` records: it reaches `Blueprint/UserWidget.h` through
// `StratCommandBarWidget.h`, `UMG` is a PRIVATE dependency of StratUI, and this class carries no
// `_API` macro.
#pragma once

#include "CoreMinimal.h"

#include "Components/Button.h"

#include "StratCommandBarWidget.h"

#include "StratCommandBarOptionsDouble.generated.h"

/** Concrete `UStratCommandBarWidget` for automation only. */
UCLASS()
class UStratCommandBarOptionsDouble : public UStratCommandBarWidget
{
	GENERATED_BODY()

public:
	/** Bind with `OnOptionsRequested.AddDynamic(Double, &UStratCommandBarOptionsDouble::
	 *  HandleRequested)`. A dynamic multicast can bind nothing lighter. */
	UFUNCTION()
	void HandleRequested()
	{
		++RequestCount;
	}

	/** How many times the bar has broadcast `OnOptionsRequested` to this observer. */
	int32 RequestCount = 0;

	/** Fills `OptionsButton` with a real `UButton`. Call BEFORE `RunNativeConstruct`. */
	void PlantOptionsButton()
	{
		OptionsButton = NewObject<UButton>(this);
	}

	/** The planted button, or null when `PlantOptionsButton` was deliberately not called. */
	UButton* Button() const { return OptionsButton; }

	/** `NativeConstruct` is `protected` on the base. Exposed rather than reimplemented: the
	 *  binding it performs IS the subject, so a clause must run the shipped body. */
	void RunNativeConstruct() { NativeConstruct(); }

	/** `NativeDestruct` likewise. */
	void RunNativeDestruct() { NativeDestruct(); }
};
