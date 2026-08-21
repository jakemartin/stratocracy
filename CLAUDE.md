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
4. **Never write a `/Game/` path literal into gameplay C++.** Asset references are
   `EditDefaultsOnly` properties set on a Blueprint default. Automation-test fixtures and the
   import commandlet are the two standing exceptions.
5. **Never `cat` the GDD.** It is at `E:\MultiAgent\stratocracy-content\Stratocracy_Prototype_GDD.md`,
   446 KB with ~100 KB lines. Grep it or slice it.
6. **Agents do not commit.** Staging and commits are the user's call.

## Where the detail lives

- **Architecture, module arrows, and the reason for each** → `.agents/ue-project-context.md`
- **What is built and what is next** → `Tools/architect/state/` — the live record, one file per
  owning agent. `global.md` carries the banner and milestone status and is **the only file that
  may state a suite count or a phase verdict**; the others link to it rather than restating it,
  and `strat_banner_sweep.py` enforces that mechanically.
- **The reasoning behind a closed phase** → `Tools/architect/state.md`, FROZEN 2026-08-20. Read
  it; never edit it. Nothing sweeps it.

When a note disagrees with the working tree, **the tree wins** — then fix the note.

## The crew

Five subagents in `.claude/agents/` build this project. `/strat-hotseat` sequences them in a
single tree; `/strat-parallel` runs them across worktrees, which is the mode for the remaining
milestones. Lanes are strict: `strat-gameplay-engineer` owns C++ outside `Tests/`,
`strat-test-author` owns `Tests/` only, `strat-editor-builder` owns `Content/` through the live
editor, `strat-data-steward` owns `Config/` and `Tools/architect/`, and
`strat-integration-reviewer` is a read-only gate.

**The one place a lane reaches outside its own directory is its own record file.** Since the
2026-08-20 split, `Tools/architect/state/` holds one file per owning agent, and each agent writes
**only its own**: `engine.md` is the engineer's, `tests.md` the test-author's, `content.md` the
editor-builder's, `data.md` and `decisions.md` the steward's, `global.md` the coordinator's and
the steward's. So "the steward owns `Tools/architect/`" holds for everything in that directory
**except** those three files. Each file names its sole writer in its own header.
