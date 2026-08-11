// GDD §4.7 Stub 8 / §2.11 -- the widget module's own header. See StratUI.Build.cs for
// why this module exists separately from `StratBridge`.
//
// It declares a log category and nothing else. In particular it declares NO reflected
// type and includes no `strat` header: the widget headers in this module forward
// declare `FStratBridge` and include `StratBridge.h` only from their .cpp files, so
// UHT never parses the vendored rules headers. That is the same discipline
// `StratBridge.h` states for itself, carried one module outward.
#pragma once

#include "CoreMinimal.h"

/** Widget-layer log category. Separate from LogStratocracy so a binding failure in a
 *  widget is not read as a failure in the game module. */
DECLARE_LOG_CATEGORY_EXTERN(LogStratUI, Log, All);
