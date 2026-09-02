// TEST-ONLY. Implementation of the UMG-free probe declared in StratProductionMenuHostProbe.h.
//
// This is the only translation unit in the test lane that touches both `UUserWidget` and
// `AStratScoreboardHUD::ProductionMenu` as concrete types, which is exactly what StratUI is
// allowed to do and StratPlay is not. Everything here is placement and reading; no clause logic
// lives in this file, so a change here cannot quietly turn an assertion into a tautology.
#include "Tests/StratProductionMenuHostProbe.h"

#include "Tests/StratProductionMenuHostDouble.h"
#include "Tests/StratProductionMenuOrderDouble.h"

#include "Blueprint/UserWidget.h"
#include "StratScoreboardHUD.h"

#include "UObject/UObjectGlobals.h"

bool StratTestInstallProductionMenuDouble(AStratScoreboardHUD* Hud)
{
	if (Hud == nullptr)
	{
		return false;
	}

	// OUTERED TO THE HUD, so the widget's lifetime is the HUD's and the `TObjectPtr` below is
	// not the only thing keeping it reachable while a clause is mid-flight. Same reason
	// `StratGuidanceRouteProbe.cpp` gives for the strip.
	//
	// `NewObject` AND NOT `CreateWidget`, and the difference is measured rather than stylistic.
	// `UUserWidget::CreateWidgetInstance` refuses a player controller that is not a LOCAL
	// player controller, and a transient world has no `ULocalPlayer` -- so `CreateWidget`
	// returns null headlessly and `AStratScoreboardHUD::CreateProductionMenuWidget`'s success
	// path is not reachable from a headless clause at all. That gap is stated in
	// `StratProductionMenuHostClauses.cpp`'s block rather than papered over here.
	UStratProductionMenuHostDouble* const Double = NewObject<UStratProductionMenuHostDouble>(Hud);
	if (Double == nullptr)
	{
		return false;
	}

	// THE SAME MEMBER `AStratScoreboardHUD::CreateProductionMenuWidget` ASSIGNS. What differs is
	// only the `AddToViewport` that follows it there, which a headless run has no viewport for.
	Hud->ProductionMenu = Double;
	return true;
}

bool StratTestHasProductionMenuWidget(const AStratScoreboardHUD* Hud)
{
	return Hud != nullptr && Hud->ProductionMenu != nullptr;
}

bool StratTestProductionMenuWidgetIsInViewport(const AStratScoreboardHUD* Hud)
{
	if (Hud == nullptr || Hud->ProductionMenu == nullptr)
	{
		// LEFT AS FALSE ON PURPOSE, and the caller is required to have ruled this out. Writing
		// anything else here would make "no widget" and "a widget that is off the viewport"
		// indistinguishable, and the menu-open clause turns on exactly that distinction.
		return false;
	}

	// THE ENGINE'S OWN ANSWER, read off the widget rather than recomputed. `UWidget::
	// IsInViewport()` consults `UGameViewportSubsystem::IsWidgetAdded`, which is the same
	// authority `AStratScoreboardHUD::IsProductionMenuWidgetOpen` reaches through.
	return Hud->ProductionMenu->IsInViewport();
}

// ---------------------------------------------------------------------------------------------
// The order-observing double. See the header block added beside these declarations.
// ---------------------------------------------------------------------------------------------

bool StratTestInstallProductionMenuOrderDouble(
	AStratScoreboardHUD* Hud,
	TFunction<void()>    OnPanelDown)
{
	if (Hud == nullptr)
	{
		return false;
	}

	// OUTERED TO THE HUD, on `StratTestInstallProductionMenuDouble`'s stated reason: the
	// widget's lifetime is the HUD's, so the `TObjectPtr` below is not the only thing keeping
	// it reachable while a clause is mid-flight.
	UStratProductionMenuOrderDouble* const Double = NewObject<UStratProductionMenuOrderDouble>(Hud);
	if (Double == nullptr)
	{
		return false;
	}

	Double->OnRemovedFromParent = MoveTemp(OnPanelDown);

	// THE SAME MEMBER `AStratScoreboardHUD::CreateProductionMenuWidget` ASSIGNS.
	Hud->ProductionMenu = Double;
	return true;
}

void StratTestClearProductionMenuOrderHook(AStratScoreboardHUD* Hud)
{
	if (Hud == nullptr)
	{
		return;
	}

	// A `Cast` AND NOT AN ASSUMPTION. The member is typed `UUserWidget` and may hold the
	// plain host double, a real WBP, or nothing at all; only the order double has a hook.
	if (UStratProductionMenuOrderDouble* const Double =
			Cast<UStratProductionMenuOrderDouble>(Hud->ProductionMenu))
	{
		Double->OnRemovedFromParent = nullptr;
	}
}
