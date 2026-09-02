// TEST-ONLY. Implementation of the order-observing double. See the header for why an ORDER
// needs a moment rather than an aftermath.
#include "Tests/StratProductionMenuOrderDouble.h"

void UStratProductionMenuOrderDouble::RemoveFromParent()
{
	// COPIED BEFORE IT IS CALLED. A hook that cleared or replaced itself would otherwise be
	// mutating the member it is being invoked through, and this class must not be the reason a
	// clause's own lambda has a lifetime rule.
	const TFunction<void()> Hook = OnRemovedFromParent;
	if (Hook)
	{
		Hook();
	}

	// ALWAYS, AND UNCONDITIONALLY. The engine's own behaviour is the thing under observation;
	// swallowing it would make the aftermath assertions in the same clause measure this class
	// instead of `AStratScoreboardHUD::CloseProductionMenuWidget`.
	Super::RemoveFromParent();
}
