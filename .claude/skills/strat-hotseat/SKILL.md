---
name: strat-hotseat
description: Drive the five-agent Stratocracy crew through the playable hot-seat milestone, phase by phase, with the editor-closed/editor-open alternation and the reviewer gate between phases. Use when the user says "run the hotseat crew", "next phase", "start the milestone", or invokes /strat-hotseat.
---

# Hot-seat milestone — crew orchestration

This skill runs in the **main thread**, because subagents cannot launch subagents. You dispatch;
the five agents do the work.

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
| **1** | closed | `strat-gameplay-engineer` → `strat-test-author` | `FStratBridge` gains `Reachable`, `Forecast`, the five `Submit*` façade methods, `RecordedLog`, `SerializeRecordedSave`; build green; new StratBridge parity tests green |
| **2** | closed | `strat-gameplay-engineer` → `strat-test-author` | `StratViewModel.h/.cpp` in `StratUI`; `AdoptBridge` + `SetViewingSide` on `AStratScoreboardHUD`; build green; **all 18 existing tests still green** |
| **3** | closed | `strat-gameplay-engineer` → `strat-test-author` | `StratPlay` module created **and registered in `Stratocracy.uproject`**; board actor, unit actor, match subsystem, camera pawn, game mode; build green; hex-layout round-trip and view-reconcile tests green |
| **4** | closed | `strat-gameplay-engineer` → `strat-test-author` | PlayerController with Enhanced Input, the selection state machine as a plain testable struct, the `STRAT-CMD` log line; build green; **hot-seat replay-parity test green** |
| **5** | **open** | `strat-editor-builder` → `strat-data-steward` | Hex mesh + terrain material instances, Input Mapping Context + Input Actions, the Blueprints and Widget Blueprints, `Lvl_FerrumCrossing`; `Config` map defaults flipped last |
| **6** | **open** | `strat-editor-builder` → `strat-test-author` → `strat-data-steward` | PIE playtest screenshots plus every `assert_log_contains` passing; full suite green; evidence assembled under `Tools/architect/evidence/` |

## Commands — verbatim, never reconstructed

Build:

```
Build.bat StratocracyEditor Win64 Development -project="E:\MultiAgent\Stratocracy\Stratocracy.uproject" -waitmutex
```

Headless test run:

```
"C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "Stratocracy.uproject" -ExecCmds="Automation RunTests Stratocracy;Quit" -unattended -nopause -nosplash -nullrhi -ReportExportPath="Saved/AutomationReport"
```

Pass these into the agent prompt rather than letting an agent recall them.

## The scope fence

**The phase list above is closed.** The milestone is: Ferrum Crossing renders, units spawn from
the scenario, click-to-select → move → attack with the deterministic forecast, wait, end turn
advances sides, the existing scoreboard follows the active side.

Explicitly **out**, and to be recorded as `NEXT` in `Tools/architect/state.md` by
`strat-data-steward` rather than built: the production menu (§2.11.5), the guided opening
(§2.11.6), the info panel, toasts, save-slot UI, an AI opponent, and move-undo.

Anything discovered mid-phase that is not on the list goes to `state.md`. It does not enter this
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

Read the `## Hot-seat milestone` section of `Tools/architect/state.md` to learn which phase is
current. After every gate returns `PASS`, dispatch `strat-data-steward` to update it: the phase
completed, the exit criterion met, and anything newly deferred.

If that section does not exist yet, phase 0 has not run.

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
- Do not commit, stage, or push. That is the user's call, always.
- Do not advance past a `BLOCK`.
- Do not compress two phases into one because they look small.
