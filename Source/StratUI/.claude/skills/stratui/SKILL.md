---
name: stratui
description: Work in Source/StratUI — the UMG surface and the reflected view model (FStratViewModel), the lowest module both widgets and StratPlay actors can see. Covers the no-widget-side-arithmetic rule (T-UI-03), why StratUI does not depend on the game module, and why nothing here may name a StratPlay type. Use when editing a widget, the view model, or StratUI.Build.cs.
---

# Source/StratUI — widgets and the view model

## Purpose

The UMG surface and the reflected view model. `StratBridge.h` pre-authorised the view model as "a
later, separate file", and it lives here because `StratUI` is the lowest module that both the
widgets and the `StratPlay` actors can see. `FStratBridge::MakeUiSnapshot` is the single
projection that feeds it.

## Patterns and invariants

- **No widget-side arithmetic.** Every number a widget draws equals **exactly one**
  `strat::UiSnapshot` field. No sums, no ratios, no derived values. This is `T-UI-03`'s defining
  clause, and it is what keeps the screen and the rules from drifting. A value the screen needs and
  the snapshot does not carry is a new bridge query, not a widget calculation.
- **`StratUI` deliberately does NOT depend on `Stratocracy`.** It binds to the projected view model,
  not to DataTable rows. `StratUI.Build.cs` records this under "NOTE ON WHAT IS NOT HERE".
- **`StratPlay → StratUI` never reverses.** Nothing here may name a `StratPlay` type; a widget that
  must reach match state broadcasts, and `StratPlay` reconciles.
- **`PCHUsage = NoPCHs` and `bUseUnity = false`**, because `.cpp` files here reach the vendored
  headers through `StratBridge.h`. The project-wide reason is in `.agents/ue-project-context.md`.
- **No reflected header here includes `StratBridge.h`.** `StratScoreboardHUD.h` and
  `StratScoreboardWidget.h` both refuse that include, forward-declare `FStratBridge`, and include it
  only from the `.cpp`. The rule is `CLAUDE.md` non-negotiable 3.

## Gotchas

- **A derived number that looks harmless is still a second source of truth.** A widget summing two
  snapshot fields agrees with the rules until the rules change one of them.
- **A Blueprint deriving from a class here bakes `/Script/StratUI.<Class>` irreversibly.** Decide a
  widget's module before any asset derives from it.
