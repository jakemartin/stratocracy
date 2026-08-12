# Stratocracy

Turn-based hex strategy in Unreal Engine 5.8, C++17. The rules are deterministic and headless;
the engine asks them for answers and never decides an outcome itself.

## Non-negotiables

1. **Never edit `Source/StratRules/` or `Data/`.** Vendored certified bytes, hash-gated against
   `E:\MultiAgent\stratocracy-crew`. Fix upstream and re-vendor; file the request instead.
2. **Never call a `strat::` function outside `Source/StratBridge/` or `Source/StratRules/`.**
   The vendored sources carry no `_API` macro — a cross-module call is `LNK2019`, measured 8×.
   Route it through an `FStratBridge` method.
3. **Never `#include "StratBridge.h"` from a header that declares a `UCLASS`/`USTRUCT`/`UENUM`.**
   It would put the vendored headers in front of UHT. Forward declare, include from the `.cpp`,
   and hold the bridge as `TPimplPtr` — never `TUniquePtr` (measured: `C4150`).
4. **Never write a `/Game/` path literal into C++.** Asset references are `EditDefaultsOnly`
   properties set on a Blueprint default.
5. **Never `cat` the GDD.** It is at `E:\MultiAgent\stratocracy-content\Stratocracy_Prototype_GDD.md`,
   446 KB with ~100 KB lines. Grep it or slice it.
6. **Agents do not commit.** Staging and commits are the user's call.

## Where the detail lives

- **Architecture, module arrows, and the reason for each** → `.agents/ue-project-context.md`
- **What is built and what is next** → `Tools/architect/state.md`

When a note disagrees with the working tree, **the tree wins** — then fix the note.

## The crew

Five subagents in `.claude/agents/` build this project, sequenced by the `/strat-hotseat` skill.
Lanes are strict: `strat-gameplay-engineer` owns C++ outside `Tests/`, `strat-test-author` owns
`Tests/` only, `strat-editor-builder` owns `Content/` through the live editor, `strat-data-steward`
owns `Config/` and `Tools/architect/`, and `strat-integration-reviewer` is a read-only gate.
