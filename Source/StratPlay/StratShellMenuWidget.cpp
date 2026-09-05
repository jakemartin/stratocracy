// GDD §2.11.5 -- see StratShellMenuWidget.h for why this class exists, and for the module-arrow
// finding that put it in `StratPlay` rather than in `StratUI` where the request placed it.
//
// THIS FILE IS DELIBERATELY THIS SMALL. It binds one button, copies one label across, copies one
// enabled bit across, and takes one route. Every question it might have asked -- what the row is
// called, whether it is available, what happens when it is taken -- is already answered in
// `UStratShellSubsystem`, and asking any of them a second time here would make this class a
// second author of §2.11.5's menu.
//
// THE ONLY BRANCHES BELOW TEST A POINTER FOR NULL. None of them reads a menu fact to decide
// anything: the `bEnabled` bit is COPIED onto the button and never branched on, which is the
// difference between carrying a decision and making one. `StratCommandBarWidget.cpp` states the
// same distinction from the other side of the arrow.
//
// NO `StratBridge.h` INCLUDE, AND THERE IS NO CONDITION UNDER WHICH THIS FILE WOULD WANT ONE.
// The title menu asks the rules module nothing -- there is no match, no board and no snapshot at
// the title -- and `UStratShellSubsystem::GatherFacts` is already the one place the shell touches
// anything else.
#include "StratShellMenuWidget.h"

#include "StratPlay.h"
#include "StratShellSubsystem.h"

// IWYU: this file calls `UButton::OnClicked` / `SetIsEnabled` and `UTextBlock::SetText`, both of
// which the header only forward declares.
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"

void UStratShellMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (OptionsButton != nullptr)
	{
		OptionsButton->OnClicked.AddDynamic(this, &UStratShellMenuWidget::HandleOptionsClicked);
	}

	// ONCE, HERE, AND NOT ON A TIMER OR A TICK. `UStratShellSubsystem::GetMenuModel` touches the
	// disk on every call -- its own block says so at the node an asset author binds -- and the
	// `Options` row is the one row in the model that cannot move: `IsRoutePermitted` grants it in
	// every fact combination and `BuildMenuModel` labels it from no fact. So there is no event
	// this class knows of that could change what it just drew. A graph that learns otherwise
	// calls `RefreshOptionsRow` again; that is what it is `BlueprintCallable` for.
	RefreshOptionsRow();
}

void UStratShellMenuWidget::NativeDestruct()
{
	// UNBOUND IN THE SAME SHAPE IT WAS BOUND. `RemoveDynamic` on an unbound delegate is a no-op,
	// so the guard is about the POINTER and not about the binding.
	if (OptionsButton != nullptr)
	{
		OptionsButton->OnClicked.RemoveDynamic(this, &UStratShellMenuWidget::HandleOptionsClicked);
	}

	Super::NativeDestruct();
}

void UStratShellMenuWidget::RefreshOptionsRow()
{
	const UWorld* const OwningWorld = GetWorld();
	const UGameInstance* const Instance =
		(OwningWorld != nullptr) ? OwningWorld->GetGameInstance() : nullptr;
	const UStratShellSubsystem* const Shell =
		(Instance != nullptr) ? Instance->GetSubsystem<UStratShellSubsystem>() : nullptr;

	if (Shell == nullptr)
	{
		// SILENT. A `UUserWidget` constructed in a fixture, or in the widget designer's preview,
		// has no game instance and correctly has no shell; drawing whatever the asset author
		// typed is the right answer there. `UStratMatchSubsystem::FindSoundDirector` records the
		// general form of this: a null that is an ordinary answer is not reported.
		return;
	}

	// BY ROUTE AND NOT BY INDEX -- see the declaration for why the shipped widget wants the
	// opposite instrument from the clause that pins the ordering.
	const FStratShellMenuModel Model = Shell->GetMenuModel();
	const FStratShellOption* const Row = Model.Options.FindByPredicate(
		[](const FStratShellOption& Candidate)
		{
			return Candidate.Route == EStratShellRoute::Options;
		});

	if (Row == nullptr)
	{
		// SILENT AND NON-DESTRUCTIVE. Unreachable while `BuildMenuModel` emits every declared
		// route; if it ever becomes reachable, leaving both controls as the asset authored them
		// is more honest than blanking a caption.
		return;
	}

	if (OptionsLabel != nullptr)
	{
		// ASSIGNED ACROSS, NOT COMPOSED. `BuildMenuModel` chose these words.
		OptionsLabel->SetText(Row->Label);
	}

	if (OptionsButton != nullptr)
	{
		// COPIED, NOT DERIVED. This is not a decision about whether the row should be available
		// -- `IsRoutePermitted` made that one -- and there is deliberately no `if (Row->bEnabled)`
		// here, because a branch would be this class holding an opinion about the answer.
		OptionsButton->SetIsEnabled(Row->bEnabled);
	}
}

void UStratShellMenuWidget::HandleOptionsClicked()
{
	UWorld* const OwningWorld = GetWorld();
	UGameInstance* const Instance =
		(OwningWorld != nullptr) ? OwningWorld->GetGameInstance() : nullptr;
	UStratShellSubsystem* const Shell =
		(Instance != nullptr) ? Instance->GetSubsystem<UStratShellSubsystem>() : nullptr;

	if (Shell == nullptr)
	{
		// REPORTED, UNLIKE THE SAME NULL IN `RefreshOptionsRow`, AND THE ASYMMETRY IS THE POINT.
		// A refresh runs in the designer preview and in fixtures, where no shell is correct. A
		// CLICK means a player is looking at a live title menu, and a live world without a
		// `UGameInstanceSubsystem` is structural -- `AStratPlayerController::RequestOptionsScreen`
		// makes the same call about the same null in the same words.
		UE_LOG(LogStratPlay, Warning,
		       TEXT("Options: the title menu was clicked with no UStratShellSubsystem reachable."));
		return;
	}

	FString Reason;
	if (!Shell->ExecuteRoute(EStratShellRoute::Options, Reason))
	{
		UE_LOG(LogStratPlay, Warning, TEXT("Options: %s"), *Reason);
	}
}
