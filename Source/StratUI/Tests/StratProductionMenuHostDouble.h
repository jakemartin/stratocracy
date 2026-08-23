// TEST-ONLY. A concrete `UUserWidget` to stand where §2.11.5's menu asset will stand.
//
// WHY THIS EXISTS. `AStratScoreboardHUD::ProductionMenu` is `TObjectPtr<UUserWidget>` and
// `UUserWidget` is `UCLASS(Abstract)` in this engine, so a clause cannot construct one. The
// shipped occupant of that member is `WBP_ProductionMenu`, an asset the editor lane owns and
// which no headless run may depend on. The test therefore supplies the one thing the base
// class is missing -- concreteness -- and supplies it here, where no shipped Blueprint default
// can ever point at it.
//
// IT ADDS NOTHING AND OVERRIDES NOTHING, on `StratGuidanceRouteDouble.h`'s stated rule. There
// is no `bConstructWasCalled`, no recorded hex, no hook of any kind. A double that kept its own
// bookkeeping would let a clause go green comparing the double against itself while the
// production member it is standing in for was never touched. Every clause reads the HUD's own
// property and `UUserWidget`'s own `IsInViewport()`.
//
// WHAT IT DELIBERATELY CANNOT DO, and this is the honest half. It is never added to a viewport,
// because a headless `-nullrhi` run has no game viewport to add it to:
// `UGameViewportSubsystem::AddToScreen` refuses a world that is not a game world and refuses a
// game world with no `UGameViewportClient`, so `AddToViewport` is a no-op here and
// `IsInViewport()` stays false. That is not a limitation the clauses work around -- it is the
// exact condition `GATE-BUILDMENU.MenuOpenReadsTheViewportAndNotThePointer` needs, because it
// is the one state where a pointer-only implementation and the shipped one disagree.
//
// THIS HEADER IS NOT INCLUDABLE FROM StratPlay and must not be made so. It includes
// `Blueprint/UserWidget.h`, and `UMG` is a PRIVATE dependency of StratUI --
// `StratGuidanceRouteProbe.h` records the ~60 `LNK2019` that measurement cost. The UMG-free
// surface StratPlay's clauses use instead is `StratProductionMenuHostProbe.h`.
#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "StratProductionMenuHostDouble.generated.h"

/** Concrete `UUserWidget` for automation only. Holds and overrides nothing. */
UCLASS()
class UStratProductionMenuHostDouble : public UUserWidget
{
	GENERATED_BODY()
};
