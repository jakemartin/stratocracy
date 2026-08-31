// GDD §2.11.5 / GATE-TITLEMENU -- the title map's HUD, implementation side.
//
// WHAT IS HERE THAT COULD NOT BE IN THE HEADER. `Blueprint/UserWidget.h`. The header holds a
// `TSubclassOf<UUserWidget>` over a forward declaration, which is all UHT needs; `CreateWidget`
// and `AddToViewport` need the definition, and UMG is a PRIVATE dependency of this module so
// the include must not migrate upward. That is the same arrangement `StratScoreboardHUD.cpp`
// documents for `StratBridge.h`, for a smaller reason: nothing here is vendored and nothing
// here would confuse UHT. It is a dependency-surface decision, not a header-tool one.
//
// THE SEQUENCE, and why it is a loop rather than a straight line. Decide, wait, create, take
// input. The waiting is the only part that is not obvious, and the header gives its reason at
// length: this class exists to make "the shell is configured before the menu is asked for its
// model" true by construction rather than by an ordering somebody has to remember. Everything
// that decides is `DecideMenuTiming`, which is static and touches no world; everything below
// it is one `CreateWidget`, one `AddToViewport` and one `SetInputMode`.
//
// THE RE-ARMING TIMER IS A NEXT-TICK CALLBACK AND NOT A TICK OVERRIDE, deliberately.
// `AHUD` ticks, so an override would have worked; a next-tick timer stops firing the moment
// the menu is up, whereas a tick override would run for the life of the screen and have to
// remember to do nothing. This project has already paid for a tick that had to be reasoned
// about -- `bCanEverTick=false` swallowing all input on a controller -- so a per-frame hook
// that is dead after the first few frames is not the shape to reach for. The budget is
// bounded, so a shell that is never configured costs `MaxConfigurationWaitTicks` timer
// callbacks and then stops, rather than polling forever behind a drawn menu.

#include "StratShellHUD.h"

#include "Blueprint/UserWidget.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

#include "StratPlay.h"
#include "StratShellSubsystem.h"

AStratShellHUD::AStratShellHUD()
{
	// THE CANVAS PASS IS OFF BECAUSE THIS CLASS OVERRIDES NO `DrawHUD`, and that is a
	// statement about this file rather than a measurement of a saving. Everything this HUD
	// puts on screen is UMG, which `bShowHUD` does not touch; a subclass that wants the
	// immediate-mode canvas back sets it true, since it is `BlueprintReadWrite` on `AHUD`.
	bShowHUD = false;
}

EStratShellMenuTiming AStratShellHUD::DecideMenuTiming(const bool  bShellIsConfigured,
                                                       const int32 TicksWaited,
                                                       const int32 MaxTicksToWait)
{
	// THE CONFIGURED ANSWER IS FIRST AND UNCONDITIONAL, which is what makes the wait budget
	// invisible on a healthy boot: a shell that is already configured never reaches the
	// arithmetic below, so `MaxTicksToWait` cannot delay a menu that could have been drawn.
	if (bShellIsConfigured)
	{
		return EStratShellMenuTiming::CreateNow;
	}

	// A NEGATIVE BUDGET FALLS THROUGH TO `CreateUnconfigured` RATHER THAN BEING CLAMPED HERE,
	// and it is left that way on purpose: `ClampMin` on the property is what stops a designer
	// typing one, and a second clamp in this function would mean two authors of the same rule.
	// Zero is the honest reading of "do not wait", and negative reads the same.
	if (TicksWaited < MaxTicksToWait)
	{
		return EStratShellMenuTiming::WaitForConfiguration;
	}

	return EStratShellMenuTiming::CreateUnconfigured;
}

void AStratShellHUD::BeginPlay()
{
	Super::BeginPlay();

	// CLEARED FIRST, on `AStratShellGameMode::BeginPlay`'s stated reason: a stale sentence
	// must never explain a new success.
	LastFailureReason.Reset();
	ConfigurationTicksWaited        = 0;
	bMenuDrewOnAConfiguredShell     = false;

	// UNSET IS A CONFIGURATION AND NOT AN ERROR -- `AStratScoreboardHUD::BeginPlay` says so of
	// its own five slots and the reasoning is identical: a map whose Blueprint leaves this
	// alone is saying "no menu here", and logging that at Error would train a reader to
	// ignore the channel a real failure arrives on. It returns rather than starting the wait,
	// so a map with no menu costs no timer callbacks at all.
	if (MenuWidgetClass == nullptr)
	{
		LastFailureReason = TEXT("no MenuWidgetClass is set on this HUD's Blueprint defaults");
		UE_LOG(LogStratPlay, Log,
			TEXT("No shell menu requested: %s"), *LastFailureReason);
		return;
	}

	ResolveMenuTiming();
}

void AStratShellHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// THE TIMER GOES FIRST. A pending next-tick callback into a HUD that is being torn down
	// is the one way this class could outlive its own world, and clearing by object rather
	// than by handle covers the re-arming call sites without a handle to keep in sync.
	if (const UWorld* const World = GetWorld())
	{
		World->GetTimerManager().ClearAllTimersForObject(this);
	}

	// REMOVED RATHER THAN LEFT TO THE GARBAGE COLLECTOR. A widget added to the viewport is
	// referenced by the viewport, so dropping the pointer alone would leave a menu drawn over
	// whatever the next level puts on screen -- which is exactly the failure a title screen
	// that travels into a match would produce.
	if (MenuWidget != nullptr)
	{
		MenuWidget->RemoveFromParent();
		MenuWidget = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void AStratShellHUD::ResolveMenuTiming()
{
	// RE-ENTRANT BY DESIGN AND GUARDED HERE RATHER THAN AT EVERY CALL SITE. BeginPlay calls
	// this once and the timer calls it again; a menu that already exists ends the sequence,
	// so a second create can never happen even if somebody arms the timer twice.
	if (MenuWidget != nullptr)
	{
		return;
	}

	const UStratShellSubsystem* const Shell = FindShell();

	// A MISSING SUBSYSTEM READS AS "NOT CONFIGURED" AND NOT AS A FAILURE, matching
	// `UStratShellSubsystem::GatherFacts`, which treats a missing match subsystem as "no
	// match" rather than as an error for the same reason: on the surface this class draws,
	// the unconfigured answer is a state the model already describes in full.
	const bool bConfigured = (Shell != nullptr) && Shell->HasMatchDestinationBeenConfigured();

	const EStratShellMenuTiming Timing =
		DecideMenuTiming(bConfigured, ConfigurationTicksWaited, MaxConfigurationWaitTicks);

	if (Timing == EStratShellMenuTiming::WaitForConfiguration)
	{
		++ConfigurationTicksWaited;
		GetWorldTimerManager().SetTimerForNextTick(this, &AStratShellHUD::ResolveMenuTiming);
		return;
	}

	if (Timing == EStratShellMenuTiming::CreateUnconfigured)
	{
		LastFailureReason = FString::Printf(
			TEXT("the shell was still unconfigured after %d tick(s); the menu will draw every ")
			TEXT("route greyed with its own reason"),
			ConfigurationTicksWaited);
		UE_LOG(LogStratPlay, Warning,
			TEXT("Shell menu drawn on an unconfigured shell: %s"), *LastFailureReason);
	}

	FString Reason;
	if (!CreateMenuWidget(Reason))
	{
		LastFailureReason = Reason;
		UE_LOG(LogStratPlay, Error, TEXT("No shell menu this session: %s"), *LastFailureReason);
		return;
	}

	// WRITTEN AFTER THE CREATE SUCCEEDED, so that "the menu drew on a configured shell" and
	// "there is a menu" cannot disagree. A failed create leaves it false with no widget, which
	// is the truthful pair.
	bMenuDrewOnAConfiguredShell = bConfigured;

	// INPUT LAST, AND ONLY ON SUCCESS. A UI-only mode with no UI on screen is a controller
	// that answers nothing, which is the state a failed create would otherwise have left the
	// player in -- an empty screen that also ignores the keyboard.
	ApplyMenuInputMode();
}

bool AStratShellHUD::CreateMenuWidget(FString& OutFailureReason)
{
	// Unset is handled by BeginPlay as a configuration, not an error; reaching here with it
	// null means someone called this directly, and that IS an error. `AStratScoreboardHUD`'s
	// create functions say the same thing in the same place, deliberately.
	if (MenuWidgetClass == nullptr)
	{
		OutFailureReason = TEXT("MenuWidgetClass is unset");
		return false;
	}

	// The owning player is the widget's outer, so the menu belongs to this local player rather
	// than to the world -- and it is what `CreateWidget` needs to give the widget a player
	// context, which is what the menu's own bindings read the game instance through.
	APlayerController* const OwningPlayer = GetOwningPlayerController();
	if (OwningPlayer == nullptr)
	{
		OutFailureReason = TEXT("this HUD has no owning player controller to parent the menu to");
		return false;
	}

	UUserWidget* const Created = CreateWidget<UUserWidget>(OwningPlayer, MenuWidgetClass);
	if (Created == nullptr)
	{
		OutFailureReason = FString::Printf(
			TEXT("CreateWidget returned null for MenuWidgetClass '%s'"),
			*GetNameSafe(MenuWidgetClass));
		return false;
	}

	Created->AddToViewport(MenuZOrder);

	// Assigned only after it is on screen, so `MenuWidget != nullptr` and "there is a menu"
	// never disagree -- `IsMenuLive` reports on exactly that.
	MenuWidget = Created;

	return true;
}

UStratShellSubsystem* AStratShellHUD::FindShell() const
{
	UGameInstance* const Instance = GetGameInstance();
	if (Instance == nullptr)
	{
		return nullptr;
	}

	return Instance->GetSubsystem<UStratShellSubsystem>();
}

void AStratShellHUD::ApplyMenuInputMode()
{
	if (!bTakeUiOnlyInputMode)
	{
		return;
	}

	APlayerController* const OwningPlayer = GetOwningPlayerController();
	if (OwningPlayer == nullptr)
	{
		return;
	}

	// NO `SetWidgetToFocus`, DELIBERATELY. Focusing the menu's own widget from here would
	// require this file to know which child is the first control, which is the asset's fact
	// and not this class's -- the widget sets its own desired focus. UI-only with no focus
	// target leaves the mouse working, which is what the shipped menu is driven with.
	const FInputModeUIOnly InputMode;
	OwningPlayer->SetInputMode(InputMode);
	OwningPlayer->bShowMouseCursor = true;
}
