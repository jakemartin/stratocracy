// TEST-ONLY. Implementation of the UMG-free probe declared in StratMatchResultHostProbe.h.
//
// This is the only translation unit in the test lane that touches both `UStratMatchResultWidget`
// and `AStratScoreboardHUD::MatchResultScreen` as concrete types, which is exactly what StratUI is
// allowed to do and StratPlay is not. Everything here is placement and reading; no clause logic
// lives in this file, so a change here cannot quietly turn an assertion into a tautology.
#include "Tests/StratMatchResultHostProbe.h"

#include "Tests/StratMatchResultHostDouble.h"

#include "StratMatchResultWidget.h"
#include "StratScoreboardHUD.h"

#include "UObject/UObjectGlobals.h"

bool StratTestInstallMatchResultDouble(AStratScoreboardHUD* Hud)
{
	if (Hud == nullptr)
	{
		return false;
	}

	// OUTERED TO THE HUD, so the widget's lifetime is the HUD's and the `TObjectPtr` below is not
	// the only thing keeping it reachable while a clause is mid-flight. Same reason
	// `StratProductionMenuHostProbe.cpp` and `StratGuidanceRouteProbe.cpp` both give.
	UStratMatchResultHostDouble* const Double = NewObject<UStratMatchResultHostDouble>(Hud);
	if (Double == nullptr)
	{
		return false;
	}

	// THE SAME MEMBER `AStratScoreboardHUD::CreateMatchResultWidget` ASSIGNS. What differs is only
	// the `AddToViewport` that precedes it there, which a headless run has no viewport for -- and
	// the model push `ShowMatchResult` does after it, which this probe deliberately omits: the
	// clause using it is about the screen's LIFETIME, and a pushed model would be a second thing
	// it could accidentally be measuring.
	Hud->MatchResultScreen = Double;
	return true;
}

bool StratTestHasMatchResultWidget(const AStratScoreboardHUD* Hud)
{
	return Hud != nullptr && Hud->MatchResultScreen != nullptr;
}
