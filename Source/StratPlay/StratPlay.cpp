// GDD §4.7 Stub 8 / §4.9 -- the gameplay module. See StratPlay.Build.cs.
#include "StratPlay.h"

#include "Modules/ModuleManager.h"

// A REAL module object, for the reason StratBridge.cpp and StratUI.cpp both record: a
// directory listed in Stratocracy.uproject WITHOUT an IMPLEMENT_MODULE makes the editor
// abort at startup with "The game module could not be successfully initialized" -- which
// is why `Source/StratRules/` is deliberately absent from that array. This module is our
// own code and carries the module object the loader requires, so it is listed, and it
// MUST be: nothing in C++ depends on StratPlay (the arrow runs the other way), so UBT
// would not build it otherwise and the failure would present as a GREEN build with
// classes that never appear in the editor.
IMPLEMENT_MODULE(FDefaultModuleImpl, StratPlay)

DEFINE_LOG_CATEGORY(LogStratPlay)
