// Compiles the vendored `Combat` module into StratBridge, beside the bridge that
// calls it. See StratBridge.Build.cs for why the rules live in this DLL rather
// than across a module boundary they cannot be exported across.
//
// ONE SHIM PER SOURCE, deliberately: each vendored file must stay its own
// translation unit, because several of them declare same-named helpers in
// anonymous namespaces. The vendored bytes are INCLUDED, never edited, so
// Source/StratRules/ stays byte-identical and T-INT-01's subject is untouched.
//
// AND IT IS COMPILED ONLY WHERE THAT BOUNDARY EXISTS. A MONOLITHIC target links
// every module into one binary, so there is no boundary for a `strat::` call to
// cross and no reason for this shim -- while `Source/Stratocracy/Stratocracy.Build.cs`
// still depends on the `StratRules` MODULE, whose own objects carry the same code.
// Both object sets then enter `Stratocracy.exe`: measured 2026-08-31 as 110 x
// LNK2005 (`Ai.strat.cpp.obj ... already defined in Ai.good.cpp.obj`) then LNK1169.
// `STRAT_VENDORED_RULES_IN_BRIDGE` is emitted by StratBridge.Build.cs from the same
// single expression that decides whether this module depends on `StratRules`, so the
// two halves cannot disagree. The `#error` is deliberate: an ABSENT definition must
// not read as 0, because that failure would surface as a link error in the modular
// build rather than here.
#if !defined(STRAT_VENDORED_RULES_IN_BRIDGE)
#	error "STRAT_VENDORED_RULES_IN_BRIDGE is emitted by StratBridge.Build.cs. This file only compiles as part of the StratBridge module."
#endif

#if STRAT_VENDORED_RULES_IN_BRIDGE
#include "../../StratRules/Combat.good.cpp"
#endif
