---
name: strat-hotseat
description: Drive the five-agent Stratocracy crew through the playable hot-seat milestone, phase by phase, with the editor-closed/editor-open alternation and the reviewer gate between phases. Use when the user says "run the hotseat crew", "next phase", "start the milestone", or invokes /strat-hotseat.
---

# Hot-seat milestone — crew orchestration

This skill runs in the **main thread**, because subagents cannot launch subagents. You dispatch;
the five agents do the work.

## Startup — one command, before the invariant

**Run this once at the start of every session, before dispatching anything:**

```
sh "$(git rev-parse --show-toplevel)/Tools/architect/hooks/install.sh"
```

**That command is cwd-independent on purpose, and the relative form is a trap this project has
already paid for once.** `sh Tools/architect/hooks/install.sh` works only from the repo root, and
a session's working directory is frequently *not* the repo root — the coordinator's own cwd has
been `Tools/context` for whole sessions. Measured 2026-08-19: from `Tools/context` the relative
form gives `sh: Tools/architect/hooks/install.sh: No such file or directory`, and when it is
piped anywhere the real exit code is masked by the pipe, so it can fail while appearing to
succeed. This is the same shape as the bare `Build.bat` form recorded below.

It installs `Tools/architect/hooks/pre-commit`, which runs `strat_banner_sweep.py` against the
**staged** files under `Tools/architect/state/` and refuses a commit whose record contradicts
itself or the tree. (Before 2026-08-20 the record was a single `Tools/architect/state.md`; that
file is now frozen history and is not swept. The hook derives its file list from the directory
and sweeps the whole set at once, because since the split the two halves of a contradiction can
sit in different files.) It is idempotent — on an already-installed clone it prints `already installed and identical`
and changes nothing — so running it every session costs one line of output and nothing else.

**Why this is a startup step and not a one-time chore.** Git hooks are not version-controlled.
A fresh clone, or a machine that has never run it, has **no** protection, and nothing anywhere
announces that fact.

**CORRECTION, 2026-08-20 — a linked worktree IS protected, and this file said the opposite.**
Hook lookup in a worktree created by `git worktree add` resolves through the **common**
directory, so the copy installed once in the main clone runs in every worktree of it. Measured:
`git rev-parse --git-path hooks` in a probe worktree returned
`E:/MultiAgent/Stratocracy/.git/hooks`, and the hook fired there — it refused the same staged
bytes the integration tree refused. Running `install.sh` from a worktree is a no-op that reports
`already installed and identical`. It is once per **clone**, not once per worktree. (Until
2026-08-20 running it from a worktree instead died with
`mkdir: cannot create directory '.../.git': Not a directory` and exit 1, because `.git` is a
*file* there; `install.sh` now resolves the hooks directory with
`git rev-parse --path-format=absolute --git-common-dir`.) The defect the sweep exists for (`185e88f`: a banner
claiming the suite was 107/107 and an item open, while the same file said 108 and discharged, 425
lines apart) reached a reviewer gate as `VERDICT: BLOCK`. That is the fourth instance of that
shape in this project's record and the second to cost a BLOCK.

**Report what it prints, and do not force past a refusal.** It refuses in two cases, both
deliberate: `core.hooksPath` is set (this repo carries four Git LFS hooks in `.git/hooks/`, and
redirecting hook lookup would silently disable all four), or a *different* `pre-commit` already
exists. Both mean a human has to look. Surface the message to the user and carry on with the
phase — a missing hook is a weaker session, not a blocked one.

**When `strat-data-steward`'s commit of the record is refused by that hook, the sweep is right
until proven otherwise.** Re-dispatch the steward with the sweep's output verbatim, exactly as
you would with a reviewer's `BLOCK`. Do not reach for `git commit --no-verify` on the steward's
behalf, and do not edit the record yourself to make it pass — both are the coordinator taking a
lane that is not theirs.

## The invariant, before anything else

**The editor is CLOSED for phases 0–4 and OPEN for phases 5–6.**

- `Build.bat` fails while the editor holds a write lock on `UnrealEditor-Strat*.dll`.
- Live Coding does not support new `UCLASS`es, so every new reflected class costs a full
  close → build → reopen cycle. This is why all of phase 3's classes land in **one** batch.
- **Never dispatch `strat-gameplay-engineer` and `strat-editor-builder` in the same phase.**
- **Never dispatch two builder agents concurrently.** `strat-test-author` follows a green build,
  never runs beside the engineer — they both touch `Source/StratUI/`, in different files, but the
  test asserts against symbols the engineer may still be moving.

At the start of every phase, state which mode the editor must be in and confirm with the user
before dispatching.

## The gate rule

**After every phase, dispatch `strat-integration-reviewer`.**

Its report ends with a literal `VERDICT: PASS` or `VERDICT: BLOCK` line. Branch on that token:

- `PASS` → advance to the next phase.
- `BLOCK` → re-dispatch the owning builder with the reviewer's findings **verbatim**, then
  re-gate. Do not advance. Do not fix it yourself.

If a report has no `VERDICT:` line, treat it as `BLOCK` and re-dispatch the reviewer.

## The phases

Each row's exit criterion is literal. A phase is not done until its criterion is met and the
gate returns `PASS`.

| # | Editor | Agents | Exit criterion |
|---|---|---|---|
| **0** | closed | `strat-data-steward` | `DT_Units`/`DT_Terrain` row order proven equal to `units.csv`/`terrain.csv` (or explicitly escalated); 18/18 tests green; `git status` clean |
| **1** | closed | `strat-gameplay-engineer` → `strat-test-author` | `FStratBridge` gains `Forecast`, the five `Submit*` façade methods, `RecordedLog`, `SerializeRecordedSave` (`Reachable` is **already landed and untested** — see below); build green; parity tests green for **every** one of those methods **including `Reachable`** |
| **2** | closed | `strat-gameplay-engineer` → `strat-test-author` | `StratViewModel.h/.cpp` in `StratUI`; `AdoptBridge` + `SetViewingSide` on `AStratScoreboardHUD`; build green; **all 18 existing tests still green** |
| **3** | closed | `strat-gameplay-engineer` → `strat-test-author` | `StratPlay` module created **and registered in `Stratocracy.uproject`**; board actor, unit actor, match subsystem, camera pawn, game mode; build green; hex-layout round-trip and view-reconcile tests green |
| **4** | closed | `strat-gameplay-engineer` → `strat-test-author` | PlayerController with Enhanced Input, the selection state machine as a plain testable struct, the `STRAT-CMD` log line; build green; **hot-seat replay-parity test green** |
| **5** | **open** | `strat-editor-builder` → `strat-data-steward` | Hex mesh + terrain material instances, Input Mapping Context + Input Actions, the Blueprints and Widget Blueprints, `Lvl_FerrumCrossing`; `Config` map defaults flipped last |
| **6** | **open** | `strat-editor-builder` → `strat-test-author` → `strat-data-steward` | PIE playtest screenshots plus every `assert_log_contains` passing; full suite green; evidence assembled under `Tools/architect/evidence/` |

### Phase 1 carries a debt from the smoke test — do not let it close silently

`FStratBridge::Reachable` was landed early by smoke test S5 and shipped at `e0cc53d` **with no
test**. Build green, gated `VERDICT: PASS`, zero coverage.

The trap: phase 1's brief used to list `Reachable` among the methods to *add*, so an agent reading
it sees the method already present, ticks it off, and writes tests only for what it actually wrote.
"New StratBridge parity tests green" is then satisfied by tests for `Forecast` and `Submit*` alone,
and `Reachable` ships permanently untested — including the clause that exists precisely to catch a
hex-distance filter standing in for a real reachability query, which the GDD records as having
already happened once.

**Phase 1 does not close until these five clauses exist and pass**, named by the engineer in its
own report:

| Clause | Property it pins |
|---|---|
| `T-UI-02.MatchesModuleQuery` | `Reachable` returns exactly what `strat::uiReachable` returns — entry for entry, in order. Catches the bridge sorting, filtering or re-costing. |
| `T-UI-02.NotHexDistance` | The set differs from a naive `hexDistance <= move` filter on Ferrum Crossing. **Without this, a regression to a distance filter passes the clause above whenever that clause is also broken.** |
| `T-UI-02.RefusesUnknownUnit` | An unknown id returns `bOk == false` with an empty out-param, distinguishable from a legitimate result. |
| `T-UI-02.SuccessIsNeverEmpty` | Every `bOk == true` call yields at least the unit's own hex at cost 0 — the property that makes the refusal channel meaningful. |
| `T-UI-02.RefusesUnseeded` | An unloaded or unseeded bridge refuses with a distinct reason rather than an empty set. |

All five live in `Source/StratBridge/Tests/`, because `StratBridge` is the only module that can call
both sides.

## Commands — verbatim, never reconstructed

> **THESE PATHS ARE THE INTEGRATION TREE'S, AND THEY ARE ONLY SAFE THERE.** Every command below
> hardcodes `E:\MultiAgent\Stratocracy\…`. Since 2026-08-20 this project also builds in linked
> worktrees under `E:\MultiAgent\Strat-wt\`, and an agent working in one of those that runs the
> lines below **builds and tests the integration tree and reports green for code it never
> compiled** — a false pass with nothing in the output to reveal it. For any tree that is not
> `E:\MultiAgent\Stratocracy`, use `.claude/skills/strat-parallel/SKILL.md`, whose templates
> derive every path from that tree's own `git rev-parse --show-toplevel`.

Build:

```
"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" StratocracyEditor Win64 Development -project="E:\MultiAgent\Stratocracy\Stratocracy.uproject" -waitmutex
```

Headless test run:

```
"C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "E:\MultiAgent\Stratocracy\Stratocracy.uproject" -ExecCmds="Automation RunTests Stratocracy;Quit" -unattended -nopause -nosplash -nullrhi -ReportExportPath="E:\MultiAgent\Stratocracy\Saved\AutomationReport"
```

Banner sweep, when a record change needs checking without a commit:

```
python "$(git rev-parse --show-toplevel)/Tools/architect/strat_banner_sweep.py" --explain
```

Pass these into the agent prompt rather than letting an agent recall them.

**`Build.bat` is not on `PATH`** — measured 2026-08-14, after this file had carried the bare
`Build.bat …` form long enough for every agent to silently work around it. Pass the full Engine
path above. From PowerShell it also needs the call operator (`& "C:\…\Build.bat" …`).

**Both paths in the test command must be absolute.** A bare `"Stratocracy.uproject"` exits in
about a second with *"Project file not found"*, before any test runs and without writing a log —
`cd`-ing to the repo root does not help. A run reporting that has run **zero** tests; never read
it as a pass.

## The scope fence

**The phase list above is closed.** The milestone is: Ferrum Crossing renders, units spawn from
the scenario, click-to-select → move → attack with the deterministic forecast, wait, end turn
advances sides, the existing scoreboard follows the active side.

Explicitly **out**, and to be recorded as `NEXT` in the owning file under `Tools/architect/state/` by
`strat-data-steward` rather than built: the production menu (§2.11.5), the guided opening
(§2.11.6), the info panel, toasts, save-slot UI, an AI opponent, and move-undo.

Anything discovered mid-phase that is not on the list goes to the owning file under `state/`. It does not enter this
milestone, however small it looks.

## Architectural decisions the crew inherits

These are settled. Pass them into agent prompts; do not re-litigate them.

- **`StratPlay` is a new module**, arrow `Stratocracy → StratBridge → StratUI → StratPlay`. The
  game module cannot host gameplay actors because `StratBridge → Stratocracy` already exists and
  the reverse arrow would be a cycle. `Source/Stratocracy/` stays untouched; wiring is asset data.
- **`UStratMatchSubsystem : UWorldSubsystem` owns the bridge; the HUD drops to borrowing.**
  `StratScoreboardHUD.h:29-36` prescribes exactly this in advance. Keep the owned-bridge path
  alive so the existing tests still pass.
- **`FStratBridge` gains `Reachable` and `Forecast`.** `strat::uiReachable` / `strat::uiForecast`
  exist in `Ui.h` but are unlinkable from any other module — this is the largest single gap
  between the current tree and a playable match. Highlighting comes from `Reachable`, **never** a
  hex-distance filter.
- **The reflected view model lives in `StratUI`.** `StratBridge.h:31-34` pre-authorizes it as "a
  later, separate file". `StratUI` is the lowest module both widgets and `StratPlay` actors see.
- **`StratPlay` never names a `strat::` type** — `FIntPoint` and `int32` only.
- **Board is 7 hierarchical instanced static mesh components**, one per terrain kind, plus reach
  and target overlays, on one board actor. Picking is a cursor hit test against the grid's
  instance index; units carry no collision. Harvest `Content/AdvancedTurnBasedTileToolkit/` for
  **meshes and materials only, never for logic** — its grid and pathfinding would be a second
  source of truth for movement.
- **Presentation is reconciled, not evented** — `ApplyView` spawns, moves and destroys to match
  the view model every refresh.
- **Log `STRAT-CMD accepted kind=… unit=… hex=… turn=… side=… hash=…`** on every accepted
  command. This is what turns a PIE playtest into a machine-checkable gate.
- **Leave `Lvl_TopDown` and `GlobalDefaultGameMode` alone until phase 5's last step**, so the
  existing scoreboard evidence stays reproducible.

## State — resuming in a fresh session

Read **`Tools/architect/state/global.md`** to learn which phase is current. After every gate
returns `PASS`, dispatch `strat-data-steward` to update it: the phase completed, the exit
criterion met, and anything newly deferred.

**CORRECTED 2026-08-20 — this section used to send a fresh session to
`Tools/architect/state.md`, and after the record split that is the wrong file in both
directions.** It is frozen history, so it cannot tell you what is current; and it must not be
edited, which `.claude/agents/strat-data-steward.md` now forbids in as many words — so the
instruction to "dispatch the steward to update it" ordered the steward to break its own brief.
Read `state.md` only for the recorded reasoning behind a phase that is already closed.

If `state/global.md` records no phase for this milestone, phase 0 has not run.

## Crew smoke test

Before phase 0, if the crew has never run, verify each agent with one dispatch. Each has a pass
condition a plausible-but-wrong agent fails:

| Agent | Task | Pass |
|---|---|---|
| `strat-integration-reviewer` | Audit the clean tree | Names all module arrows **including the deliberate `StratBridge → Stratocracy`**; zero findings; `VERDICT: PASS`. Flagging that arrow means bad ground truth — fix `.agents/ue-project-context.md`, not the tree. |
| `strat-test-author` | Run the suite headless | 18/18, full test names reported, tree clean afterwards |
| `strat-editor-builder` | Report NeoStack status, then read `WBP_Scoreboard`'s graph | **Editor closed:** one clean diagnostic, no retry loop, no invented Lua. **Editor open:** the actual graph. Test both ways. |
| `strat-data-steward` | Prove `DT_Units` row order equals `units.csv` | A real comparison *or* an explicit escalation — both pass. A guess fails. |
| `strat-gameplay-engineer` | Add `FStratBridge::Reachable` and build | Green build; diff touches only `StratBridge.h/.cpp`; no `Tests/` file touched |

## What you do not do

- Do not edit source, assets, config or tests yourself. Dispatch the owner.
- Do not `--no-verify` past the banner-sweep hook, and do not edit the record to satisfy it.
  Re-dispatch `strat-data-steward` with the sweep's output, the same as any `BLOCK`.
- Do not commit, stage, or push. That is the user's call, always.
- Do not advance past a `BLOCK`.
- Do not compress two phases into one because they look small.
