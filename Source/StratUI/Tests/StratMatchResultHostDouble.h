// TEST-ONLY. A concrete `UStratMatchResultWidget` to stand where §2.11.4's WBP_ asset will stand.
//
// WHY THIS EXISTS. `AStratScoreboardHUD::MatchResultScreen` is
// `TObjectPtr<UStratMatchResultWidget>` and `UStratMatchResultWidget` is `UCLASS(Abstract)`, so a
// clause cannot construct one. The shipped occupant of that member is a Widget Blueprint the
// editor lane owns and which no headless run may depend on. The test supplies the one thing the
// base class is missing -- concreteness -- and supplies it here, where no shipped Blueprint
// default can ever point at it.
//
// IT ADDS NOTHING AND OVERRIDES NOTHING, on `StratProductionMenuHostDouble.h`'s stated rule and
// `StratGuidanceRouteDouble.h`'s before it. There is no push counter, no recorded model, no hook.
// A double that kept its own bookkeeping would let a clause go green comparing the double against
// itself while the production member it stands in for was never touched. The one clause that uses
// it reads `AStratScoreboardHUD`'s own property and nothing else.
//
// WHAT IT DELIBERATELY CANNOT DO. It is never added to a viewport, because a headless `-nullrhi`
// run has no game viewport to add it to -- `UGameViewportSubsystem::AddToScreen` refuses a world
// with no `UGameViewportClient`. So `IsMatchResultWidgetOpen()` is false in every state a headless
// clause can reach, and the clause that uses this double reads the POINTER through the probe
// rather than that function. `StratProductionMenuHostDouble.h` records the same condition, and
// the production-menu file turns it into a clause of its own; here it is a limitation, and it is
// named rather than traded on.
//
// THIS HEADER IS NOT INCLUDABLE FROM StratPlay and must not be made so. It includes
// `Blueprint/UserWidget.h` through `StratMatchResultWidget.h`, and `UMG` is a PRIVATE dependency
// of StratUI -- `StratGuidanceRouteProbe.h` records the ~60 `LNK2019` that measurement cost. The
// UMG-free surface StratPlay's clause uses instead is `StratMatchResultHostProbe.h`.
#pragma once

#include "CoreMinimal.h"

#include "StratMatchResultWidget.h"

#include "StratMatchResultHostDouble.generated.h"

/** Concrete `UStratMatchResultWidget` for automation only. Holds and overrides nothing. */
UCLASS()
class UStratMatchResultHostDouble : public UStratMatchResultWidget
{
	GENERATED_BODY()
};
