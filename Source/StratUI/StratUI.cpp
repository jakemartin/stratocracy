// GDD §4.7 Stub 8 / §2.11 -- the widget module. See StratUI.Build.cs.
#include "StratUI.h"

#include "Modules/ModuleManager.h"

// A REAL module object, for the reason StratBridge.cpp records: a directory listed in
// Stratocracy.uproject without an IMPLEMENT_MODULE makes the editor abort at startup
// with "The game module could not be successfully initialized". This module is our own
// code, so it carries the module object the loader requires and can be listed safely.
IMPLEMENT_MODULE(FDefaultModuleImpl, StratUI)

DEFINE_LOG_CATEGORY(LogStratUI)
