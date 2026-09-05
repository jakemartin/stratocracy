// TEST-ONLY. The four `UObject`s the options-panel clauses in this module need and cannot get
// any other way.
//
// WHY ANY OF THEM EXIST. Three of the four classes under test are `UCLASS(Abstract,
// Blueprintable)` -- `UStratOptionsWidget`, `UStratShellMenuWidget` and
// `UStratCommandBarWidget` -- so a clause cannot construct one, and the shipped occupants are
// `WBP_` assets the editor lane owns and which no headless run may depend on. The fourth is an
// observer: `FStratOptionsPanelStateChanged` is a DYNAMIC multicast, and a dynamic delegate can
// only be bound to a `UFUNCTION` on a `UObject`. There is no way to observe a broadcast without
// an observer. Same shape and same reasoning as `Source/StratUI/Tests/StratOptionsWidgetDouble.h`
// and `StratProductionMenuHostDouble.h`.
//
// THE RULE THESE STAY INSIDE, RESTATED BECAUSE IT IS THE ONE THAT MATTERS.
// `StratProductionMenuHostDouble.h` states it: a double that keeps its own bookkeeping lets a
// clause go green comparing the double against itself. That rule binds a double standing in for
// an object whose OWN state is the subject. The tallies below are the RECEIVING END of the
// things being measured -- a broadcast has no other observable -- and every VALUE any clause
// asserts is read back off the shipped object: `UStratShellSubsystem::GetOptionsPanelRequestCount`,
// `UStratOptionsPresenter::GetPanelsShownCount`, `UStratOptionsWidget::Model`, or the planted
// UMG widget the shipped code wrote to. Nothing here renders a percent, decides a route, or
// holds a second copy of a gain.
//
// WHY THIS HEADER IS IN StratPlay AND `StratOptionsWidgetDouble.h` COULD NOT BE REUSED. That
// header states in terms that it "is not includable from StratPlay and must not be made so".
// The reason is not the UMG include -- StratPlay carries `UMG` privately of its own, recorded in
// `StratPlay.Build.cs` -- but the class: `UStratOptionsWidgetDouble` is declared inside StratUI
// with no `_API` macro, so naming it from another module is `LNK2019` on its `StaticClass` and
// its constructor. `StratGuidanceRouteProbe.h` records the ~60-link-error measurement of the
// general form. A second double, one per module, is this project's standing posture for a test
// helper and is deliberate duplication rather than an oversight.
#pragma once

#include "CoreMinimal.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "StratCommandBarWidget.h"
#include "StratOptionsWidget.h"
#include "StratShellMenuWidget.h"
#include "StratShellSubsystem.h"

#include "StratOptionsPanelDoubles.generated.h"

/**
 * Records `UStratShellSubsystem::OnOptionsPanelStateChanged`.
 *
 * IT RECORDS THE REQUEST COUNT AS SEEN FROM INSIDE THE HANDLER, which is the one field here
 * that is not obvious. `RequestOptionsPanel` writes the flag, then the counter, THEN broadcasts,
 * and its own body says why: *"An observer that reads `GetOptionsPanelRequestCount` from inside
 * the handler -- which is the natural thing for a clause to do -- must not see a count that lags
 * the flag it was just handed."* That ordering is a real property with a real failure mode, and
 * the only place it is observable is here, during the broadcast. Set `Watched` before binding.
 */
UCLASS()
class UStratOptionsPanelStateObserver : public UObject
{
	GENERATED_BODY()

public:
	/** Bind with `OnOptionsPanelStateChanged.AddDynamic(Observer,
	 *  &UStratOptionsPanelStateObserver::HandleStateChanged)`. */
	UFUNCTION()
	void HandleStateChanged(bool bIsOpen)
	{
		++BroadcastCount;
		bLastFlag = bIsOpen;

		if (Watched != nullptr)
		{
			CountSeenInsideHandler = Watched->GetOptionsPanelRequestCount();
			bFlagSeenInsideHandler = Watched->IsOptionsPanelOpen();
		}
	}

	/** The shell whose members are read back from inside the handler. May be left null. */
	UPROPERTY()
	TObjectPtr<UStratShellSubsystem> Watched;

	/** How many broadcasts this observer has received. */
	int32 BroadcastCount = 0;

	/** The argument carried by the most recent broadcast. */
	bool bLastFlag = false;

	/** `GetOptionsPanelRequestCount()` as it read during the most recent broadcast. */
	int32 CountSeenInsideHandler = -1;

	/** `IsOptionsPanelOpen()` as it read during the most recent broadcast. */
	bool bFlagSeenInsideHandler = false;
};

/**
 * Concrete `UStratOptionsWidget` for `UStratOptionsPresenter::ConfigureOptionsPanel`.
 *
 * IT PLANTS NOTHING AND OVERRIDES NOTHING. The presenter clauses are about the presenter --
 * whether it creates a panel, seeds it without committing, and forwards what the panel says --
 * so this double supplies concreteness only, and every value is read off the SHIPPED
 * `UStratOptionsWidget::Model` and the SHIPPED setters. The bound-sub-widget facts are
 * `Source/StratUI/Tests/StratOptionsWidgetBindingClauses.cpp`'s subject, one module over.
 */
UCLASS()
class UStratOptionsPanelWidgetDouble : public UStratOptionsWidget
{
	GENERATED_BODY()
};

/**
 * Concrete `UStratShellMenuWidget` with a plantable `OptionsLabel` and `OptionsButton`.
 *
 * BOTH ARE `BindWidgetOptional` AND THEREFORE NULL ON A NATIVE SUBCLASS, so without the plant
 * `RefreshOptionsRow` runs its whole body and writes to nothing -- which would make a clause on
 * the label vacuous rather than red. `NativeConstruct` is exposed for the same reason the other
 * doubles expose theirs: the bind it performs is part of the subject.
 */
UCLASS()
class UStratShellMenuWidgetDouble : public UStratShellMenuWidget
{
	GENERATED_BODY()

public:
	/** Fills both optional members with real UMG objects. Call BEFORE `RunNativeConstruct`. */
	void PlantOptionsRow()
	{
		OptionsButton = NewObject<UButton>(this);
		OptionsLabel  = NewObject<UTextBlock>(this);
	}

	UButton*    Button() const { return OptionsButton; }
	UTextBlock* Label()  const { return OptionsLabel; }

	void RunNativeConstruct() { NativeConstruct(); }
	void RunNativeDestruct()  { NativeDestruct(); }
};

/**
 * Concrete `UStratCommandBarWidget` for `AStratScoreboardHUD::CommandBar`.
 *
 * NO PLANT AND NO TALLY. The clause that uses it broadcasts `OnOptionsRequested` directly --
 * which is public and `BlueprintAssignable` -- because its subject is what
 * `UStratMatchSubsystem` did with the BINDING, not what the button did with a click. The click
 * half is pinned one module over, against `UStratCommandBarOptionsDouble`, and duplicating it
 * here would put a StratUI fact in a StratPlay file.
 */
UCLASS()
class UStratCommandBarPlayDouble : public UStratCommandBarWidget
{
	GENERATED_BODY()
};
