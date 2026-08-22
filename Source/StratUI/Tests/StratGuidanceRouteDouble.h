// TEST-ONLY. A concrete class to stand where `UStratGuidanceWidget` cannot stand itself.
//
// WHY THIS EXISTS. `UStratGuidanceWidget` is `UCLASS(Abstract, Blueprintable)` because the
// WBP_ asset derives from it and does the layout. `Abstract` is a ruling, not an oversight,
// and nothing under `Tests/` may weaken it -- but an abstract class cannot be instantiated,
// and a clause that cannot get an instance cannot observe what arrives at one. So the test
// supplies the one thing the production class is missing, concreteness, and supplies it here,
// where no shipped Blueprint default can ever point at it.
//
// IT ADDS NOTHING AND OVERRIDES NOTHING, AND THAT IS THE DESIGN. There is no `PushCount`, no
// captured-argument member, no recording hook of any kind. A double that kept its own copy of
// the pushed value would let a clause go green by comparing the double's bookkeeping against
// itself while `UStratGuidanceWidget::Guidance` -- the property a real `WBP_DirectiveStrip`
// actually binds to -- was never written. Every clause reads the INHERITED property.
//
// WHY A DECLARED SUBCLASS AND NOT A RUNTIME TRICK. Clearing `CLASS_Abstract` on the shared
// `UClass` for the duration of a test mutates global reflection state and leaks it on any
// early `return false`; and `NewObject` on an abstract class is not a supported call. A
// subclass is the ordinary Unreal answer, and UHT processes headers anywhere under the module
// directory, `Tests/` included.
//
// THIS HEADER IS NOT INCLUDABLE FROM StratPlay and must not be made so. It includes
// `StratGuidanceWidget.h`, which includes `Blueprint/UserWidget.h`, and `UMG` is a PRIVATE
// dependency of StratUI. `StratGuidanceRouteProbe.h` is the UMG-free surface StratPlay's
// clauses use instead; its block records the measurement.
#pragma once

#include "CoreMinimal.h"

#include "StratGuidanceWidget.h"

#include "StratGuidanceRouteDouble.generated.h"

/** Concrete `UStratGuidanceWidget` for automation only. Holds and overrides nothing. */
UCLASS()
class UStratGuidanceRouteDouble : public UStratGuidanceWidget
{
	GENERATED_BODY()
};
