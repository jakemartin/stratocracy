// TEST-ONLY. Implementation of the UMG-free probe declared in StratGuidanceRouteProbe.h.
//
// This is the ONLY translation unit in the test lane that touches both `UStratGuidanceWidget`
// and `AStratScoreboardHUD` as concrete types, which is exactly what StratUI is allowed to do
// and StratPlay is not. Everything here is placement and reading; no clause logic lives in
// this file, so a change here cannot quietly turn an assertion into a tautology.
#include "StratGuidanceRouteProbe.h"

#include "StratGuidanceRouteDouble.h"
#include "StratGuidanceWidget.h"
#include "StratScoreboardHUD.h"

#include "UObject/UObjectGlobals.h"

bool StratTestInstallGuidanceStripDouble(AStratScoreboardHUD* Hud)
{
	if (Hud == nullptr)
	{
		return false;
	}

	// OUTERED TO THE HUD, so the strip's lifetime is the HUD's and the `TObjectPtr` below is
	// not the only thing keeping it reachable while a clause is mid-flight.
	UStratGuidanceRouteDouble* const Double = NewObject<UStratGuidanceRouteDouble>(Hud);
	if (Double == nullptr)
	{
		return false;
	}

	// THE SAME MEMBER `AStratScoreboardHUD::CreateGuidanceWidget` ASSIGNS. What differs is only
	// the missing `AddToViewport`, which a headless run has no viewport for.
	Hud->GuidanceStrip = Double;
	return true;
}

bool StratTestReadGuidanceStrip(const AStratScoreboardHUD* Hud, FStratGuidanceView& Out)
{
	if (Hud == nullptr)
	{
		return false;
	}

	const UStratGuidanceWidget* const Strip = Hud->GuidanceStrip;
	if (Strip == nullptr)
	{
		// LEFT UNTOUCHED ON PURPOSE. Writing a default view here would make "no strip" and "a
		// strip holding a default" indistinguishable, and the no-strip clause turns on exactly
		// that distinction.
		return false;
	}

	// THE PRODUCTION PROPERTY, not a recording. This is the field a reparented
	// `WBP_DirectiveStrip` binds its four pure functions to.
	Out = Strip->Guidance;
	return true;
}

bool StratTestPushAtGuidanceStripDirectly(AStratScoreboardHUD* Hud, const FStratGuidanceView& In)
{
	if (Hud == nullptr)
	{
		return false;
	}

	UStratGuidanceWidget* const Strip = Hud->GuidanceStrip;
	if (Strip == nullptr)
	{
		return false;
	}

	Strip->PushGuidance(In);
	return true;
}
