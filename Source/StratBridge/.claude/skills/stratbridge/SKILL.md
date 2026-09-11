---
name: stratbridge
description: Work in Source/StratBridge — the only module allowed to call `strat::`. Covers the two API layers, the FStratResult refusal contract, the single-projection rule, and the parity tests that bind the bridge to the module. Use when editing the bridge, adding a query or command method, or diagnosing an LNK2019 from a cross-module call.
---

# Source/StratBridge — the `strat::` seam

## Purpose
<!-- agent-docs:fill:purpose -->

The seam between the vendored headless rules module (`strat::`) and everything Unreal. This is the
**only** module permitted to name a `strat::` symbol. Every engine-side caller reaches the rules
through `FStratBridge` or not at all.

## Mental model and key files
<!-- agent-docs:fill:model -->

Four files and a test directory:

| File | Owns |
|---|---|
| `StratBridge.h` / `.cpp` | `FStratBridge` (`STRATBRIDGE_API`) — the whole surface |
| `StratCombatLog.h` | the combat log types |
| `StratBridge.Build.cs` | the dependency arrows; read it rather than assuming what may be called |
| `Tests/` | 15 files, mostly **parity** suites binding bridge output to module output |

`FStratBridge` has **two API layers over one implementation**, and confusing them is the most
common way to write a method that looks right:

- **`strat::`-typed** — `Submit`, `SubmitMove`, `Reachable`, `Forecast`, `BuildOptions`,
  `MakeUiWorld`. Takes and returns module types.
- **The engine-typed façade** — `ReachableHexes`, `MovePathToHex`, `AttackTargetHexes`,
  `SubmitMoveToHex`, `SubmitAttackAtHex`, `SubmitBuildAtHex`. Same behaviour in `FIntPoint` /
  `TArray`, for callers that must not see `strat::`.

Everything that mutates funnels through `SubmitStamped`, which stamps `{turn, side}` off the live
state. `MakeUiSnapshot` is the single projection to the view model.

## Patterns and invariants
<!-- agent-docs:fill:patterns -->

- **`strat::` appears in this module and nowhere else.** A cross-module call is not a compile
  error — it is `LNK2019` at link time, and it has been measured **eight times** on this project.
- **Every fallible entry point returns `FStratResult`.** `Fail` carries the reason **the owning
  module gave**; the bridge does not compose its own explanation over the top.
- **One projection.** `MakeUiSnapshot` is the only path to the view model. A second projection is
  a second source of truth, and they will disagree.
- **`Tables()` is borrowed and const** — the way a rules module takes what it does not own. Do not
  store it past the bridge's lifetime.
- **A new typed command joins the shared tail.** Route it through `SubmitStamped` rather than
  stamping at the call site, or the recorded log and the live state drift apart.
- **Never edit `Source/StratRules/` or `Data/`** to make something here compile. Those are
  vendored certified bytes, hash-gated against `stratocracy-crew`. File an upstream re-vendor
  request instead.

**Stop for:** a rules behaviour that would need a `strat::` change, a parity test whose expected
value only the module can supply, or a dependency arrow not already in `StratBridge.Build.cs`.
**Do not stop** because one build failed — read the first error, not the last.

## Common tasks → first action
<!-- agent-docs:fill:tasks -->

- **Adding a query.** Find the `strat::` call that already answers it, then add the thin wrapper —
  both layers if engine callers need it. First action: read the nearest existing query in
  `StratBridge.h` and copy its `FStratResult` shape.
- **Adding a command.** Write the `strat::`-typed method, route it through `SubmitStamped`, then
  add the `...AtHex` façade. First action: read `SubmitMove` and `SubmitMoveToHex` as the pair.
- **An `LNK2019` mentioning `strat::`.** Something outside this module named a module symbol.
  First action: find the caller, not the symbol — the fix is nearly always to move the call here
  and expose a bridge method.
- **A parity test failing.** First action: ask the module for the value. Never compute the
  expectation in the test.

## Gotchas
<!-- agent-docs:fill:gotchas -->

Things that get mistaken for one another in this module.

- **A cross-module `strat::` call is not a compile error.** It builds clean and fails at link with
  `LNK2019`. Measured eight times.
- **A parity test passing is not the bridge and the module agreeing.** It is agreement on the one
  clause that test asserts. Fifteen parity files exist because each buys one clause.
- **An expectation computed in a test is not a parity check.** If the module can be asked for the
  value, asking is the test; recomputing it makes the test agree with itself.
- **The façade is not a second implementation.** `SubmitMoveToHex` and `SubmitMove` must stamp
  identically. If they diverge, the recorded log stops describing the state.
- **`FStratResult::Ok()` is not "it happened".** It is "the owning module did not refuse". What
  actually changed is in the state, not the result.
- **A refusal reason written here is not the module's reason.** Pass the module's through; a
  helpful rephrasing is a fabricated diagnosis.
- **`Tables()` is not owned.** It is borrowed and const. Storing it outlives what it points at.
- **A vendored header in front of UHT is not a warning.** It is a build failure, and the fix is
  never to edit the vendored bytes.
- **The `Vendored/*.strat.cpp` compile-in is not unconditional.** It holds only in a modular
  (editor) build. A monolithic Game target already links the rules through the game module, so
  compiling them here too put every `strat::` symbol in the binary twice: measured 2026-08-31 as
  110 × `LNK2005` then `LNK1169`, while the editor build was green. `STRAT_VENDORED_RULES_IN_BRIDGE`
  and `StratBridge.Build.cs` switch both halves on one `Target.LinkType` expression. Both branches
  are reasoned in `.agents/ue-project-context.md` under "AND THAT COMPILE-IN IS CONDITIONAL", which
  the reviewer's checklist cites. It stays there because it spans two modules' `.Build.cs` files.
