Run 2026-09-10 (local) by `strat-integration-reviewer`, read-only, against COMMITTED `d59bf9b` on
`master` in `E:\MultiAgent\Stratocracy`. Persisted by the coordinator (findings and quotes verbatim; layout lightly reformatted) outside the repo so the
report survives the session; it is not yet under `Tools/architect/gate_reports/` (steward's lane).

# Gate: T-SAVE-04 merge re-gate (`d59bf9b`) — strat-integration-reviewer

## Scope
Committed merge `d59bf9b` on `master` in `E:\MultiAgent\Stratocracy` (parents `9fbfc5d`, `f53ca26`; branch base `283d711` sits 30 commits behind `9fbfc5d`). Change set `git diff 9fbfc5d d59bf9b`: 11 files, +1457/−12 — identical to `f53ca26`'s own-base diff except `global.md`, which the merge added. `git diff d59bf9b -- Source Data` is empty, so on-disk checks of `Source/` describe `d59bf9b`. The nine uncommitted files were ignored as instructed. No build, no UE suite run, no file written.

## Checklist
| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — no diff under `Source/StratRules`/`Data` in the merge or the working tree; manifests `rulesCommit 96d93ea…`, `dataCommit c2f5860…`; neither path changed since `5587d96` (2026-08-25) |
| 2 | No `strat::` outside the two dirs | pass — 0 hits outside (control: same pattern finds 140 inside `Source/StratBridge`); every `strat::` mention in the changed files is a type name or a comment |
| 3 | No vendored header in front of UHT | pass — changed UHT headers include no vendored header; `StratBridge.h` declares no UHT macro and is included only from `StratMatchSubsystem.cpp:44` and `StratPlayerController.cpp:24` |
| 4 | `TPimplPtr` not `TUniquePtr` | pass — both `TUniquePtr` hits are comments |
| 5 | Module arrows | pass — extraction printed 16 rows, census 10 names, matching the block; the 16th is the guarded `.Add` at `StratBridge.Build.cs:135-137` under `if (!bCompileVendoredRulesHere)`; no `.Build.cs` in the diff |
| 6 | New modules registered | pass — none new |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` literal | pass — new hits are `StratLoadPresentationCarryOverClauses.cpp:109-113`, an automation fixture (standing exception); others are comments |
| 9 | No widget-side arithmetic | pass — `StratUI` untouched |
| 10 | Lane discipline | **BLOCK** — Finding 3 |
| 11 | Prose blocks present and accurate | **BLOCK** — Findings 1 and 2 |
| 12 | Nothing staged/committed | pass — `git diff --cached` empty; the two commits are user-approved per the brief, which no checkout can verify |

**Report and instrument:**
- `Saved/AutomationReport/index.json`: `reportCreatedOn 2026.09.10-18.20.14`, 479 entries, all `Success`, 0 failed/notRun/succeededWithWarnings; both new clauses present by name as `Success`. Agrees with `global.md:30-34`.
- `strat_suite_report_gate.py --pin-to-tree` → `SUITE REPORT GATE CLEAN`, exit 0; newest `Source/` file 18.16.13 UTC; macro census 479.
- Instrument judged usable: `--self-test` passed every fixture including the edited-after-report fail. Limits: it compares working-tree mtimes, so it holds here only because `Source/` equals `d59bf9b` byte for byte; and it is uncommitted, so it cannot be re-run from `d59bf9b` alone.

## Findings

**1. The three corrected sites are right about the structure and wrong at every line number in the merged tree.** Owner: `strat-gameplay-engineer`.
- The code is unchanged in this region: the two refusal arms and the call are byte-identical to base, shifted +24 lines — the function opens at `:202`, the arms return at `:220` and `:227`, and `TearDownPresentation();` is the only call, at `:243`. The post-call refusals moved to `:308`, `:328`, `:408`.
- So the present-tense citations are false. `StratMatchSubsystem.h:2763`: *"its one call site in `StartMatchInternal` (`StratMatchSubsystem.cpp:219`)"*; `:2979`: *"exactly one call site -- `StratMatchSubsystem.cpp:219`, inside `StartMatchInternal`, unguarded"*. In the merged tree `:219` is the `UE_LOG(... "No match this session: %s" ...)` line inside the definition-tables refusal arm — a reader following the citation lands inside the guard the block says the call does not have.
- The same stale numbers appear at `:2771-2773` (*"`:219` is not the top… (`:189-197`)… (`:199-204`)"*), `:2988`, `:1102-1105`, and `:1122-1125` (*"`:266`… `:286`… `:366`"*).
- To clear: replace line numbers with anchors a checkout can re-find (the quoted `TearDownPresentation();` statement under "Whatever the last match left", the two `OutFailureReason =` arms), or re-measure against `d59bf9b` and tie each figure to that commit. The only site dated as a measurement, `:1101` (*"Measured 2026-09-03"*), names no commit.

**2. The "fourth site" handoff is not recorded anywhere, and the merged tree has five sites, not four.** Owner: `strat-test-author` for the record entry; the coordinator or `strat-data-steward` for `global.md`'s wording.
- `engine.md:3097-3098`: *"THE FOURTH IS NOT MINE AND IS HANDED OFF, NOT FIXED… See the handoff recorded in `tests.md`'s lane, not here."* `tests.md` holds no such handoff; the `2026-09-03 (local), strat-test-author` entry (`:3149-3263`) never names the phrase or the site. `global.md:36-37` repeats the claim.
- A wrap-aware sweep of `d59bf9b` finds *"runs unconditionally at the top of `StartMatchInternal`"* at:
  - `Source/StratPlay/Tests/StratTransientReceiptCallSite.cpp:291-292` — the base's fourth site;
  - `Source/StratPlay/Tests/StratSoundDirectorCallSite.cpp:684-685` — a fifth, arrived from master (file first appears in `d80b28a`, 2026-09-05, after base). The merge re-checked the seam's premise but did not re-sweep for the phrase.
- The phrase alone would not gate if the handoff were recorded: neither site builds a false conclusion on it. What gates is a handoff no record holds.
- To clear: a `tests.md` entry receiving both sites and stating each one's fate (fixed, or a debt with its discharging condition), naming `d59bf9b`; then make `global.md`'s singular "fourth site" true.

**3. A new clause file landed with no lane record.** Owner: `strat-test-author`.
- `Source/StratPlay/Tests/StratSelectionMachineResetClauses.cpp` declares `"Stratocracy.StratPlay.T-SAVE-04.SelectionMachineResetEmptiesSelectionDoneAndLocked"` (`:140`).
- File name and clause name appear 0 times in `engine.md`, `tests.md`, `data.md`, `decisions.md`, `content.md`; only in `global.md:32-33`. The receiving entry, `tests.md:3154`, still says *"ONE CLAUSE"*. Record in `global.md` only is a verdict block, and nothing in the tree says who wrote the file (its header: *"A GAP A REVIEWER FOUND RATHER THAN ONE THIS LANE PLANNED FOR"*).
- To clear: a `tests.md` entry with acting and writing attributed inline, base commit, and merge `d59bf9b`; if the coordinator wrote the file, that entry must also meet the in-session clause's obligations.

## Observations
- The four PROPOSED clauses are correctly marked (`engine.md:3117-3138`); none of the four names exists in `Source/` (0 files each; control: both real clause names found).
- The record merge is clean: against the first parent, `engine.md` (+186/−0) and `tests.md` (+116/−0) are pure additions, byte-identical to the branch's added lines, date-ordered. "Nothing in either was re-authored" is true.
- `engine.md:2978, 2983-2984, 3082` carry the same stale numbers (`:178`, `:219`, `:189-197`, `:199-204`) — true of `283d711`, stale on master; fixable with Finding 1.
- Two report citations point at reports no longer on disk: `engine.md:3064-3065` (`2026.09.03-13.08.33`) and `tests.md:3194-3196` (`2026.09.03-12.53.49`, "the one `strat_banner_sweep.py` opens"). Both predate a later test-file edit; `tests.md:3216-3219`'s own rule says such citations get stamped as history. Both entries still read "UNCOMMITTED AND UNMERGED at the time of writing" — time-qualified, but not stamped as landed.
- Clause-ID conflict: `tests.md:5113-5118` records a refusal to put clauses under `T-SAVE-04` when that ID's GDD sentence ("refusal: any header mismatch") excludes their subject; both new clauses ride `T-SAVE-04`. `tests.md:3158` cites a user ruling; nothing in `decisions.md` reconciles the two.
- `StratMatchSubsystem.cpp:3744-3745` (*"`StartMatchInternal` calls this function unconditionally"*) is imprecise the same way and predates this change; an "at the top" sweep cannot see it.
- The original BLOCK report was never persisted; its findings survive only as the engineer's paraphrase, so this verdict could not be compared against it. Persist this report under `Tools/architect/gate_reports/`.
- A lane topology is standing (`slot-1`, `slot-2` in `git worktree list`), so the in-session clause does not cover clearing these findings; dispatch is the route.

VERDICT: BLOCK
