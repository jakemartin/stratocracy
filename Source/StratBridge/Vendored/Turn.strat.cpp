// Compiles the vendored `Turn` module into StratBridge, beside the bridge that
// calls it. See StratBridge.Build.cs for why the rules live in this DLL rather
// than across a module boundary they cannot be exported across.
//
// ONE SHIM PER SOURCE, deliberately: each vendored file must stay its own
// translation unit, because several of them declare same-named helpers in
// anonymous namespaces. The vendored bytes are INCLUDED, never edited, so
// Source/StratRules/ stays byte-identical and T-INT-01's subject is untouched.
#include "../../StratRules/Turn.good.cpp"
