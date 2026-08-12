// GDD §4.7 Stub 8 / §4.9 -- the gameplay module's own header. See StratPlay.Build.cs for
// why this module exists on the far side of `StratUI`.
//
// It declares a log category and nothing else. In particular it declares NO reflected
// type and includes no `strat` header: the actor headers in this module forward declare
// `FStratBridge` and include `StratBridge.h` only from `StratMatchSubsystem.cpp`, so UHT
// never parses the vendored rules headers. That is the same discipline `StratBridge.h`
// states for itself and `StratUI.h` carries one module outward, carried one further.
#pragma once

#include "CoreMinimal.h"

/** Gameplay-layer log category. Separate from LogStratUI so a reconciliation failure in
 *  an actor is not read as a binding failure in a widget -- they have different fixes and
 *  different owners. */
DECLARE_LOG_CATEGORY_EXTERN(LogStratPlay, Log, All);
