Run 2026-09-10 (local) by `strat-integration-reviewer`, read-only, against the UNCOMMITTED working
tree over `3143049` on `master` in `E:\MultiAgent\Stratocracy` (16 files, +570/−73). Answers
`2026-09-10-gate-B-t-save-04-regate.md`. Persisted by the coordinator outside the repo (findings and
quotes verbatim; evidence condensed).

# Gate: T-SAVE-04 merge repair (second pass over `3143049`) — VERDICT: BLOCK

## Checklist
1–10 and 12 pass: vendored bytes untouched (`rulesCommit 96d93ea…`, `dataCommit c2f5860…`); no
`strat::` outside the two dirs (control: 144 inside `StratBridge`); no vendored header in the four
changed UHT headers; `TUniquePtr` hits are comments; module arrows 16 rows / 10 names; no new
module; `Source/Stratocracy` untouched; no `/Game/` on any added line; `StratUI` untouched; lanes
wrote only their own files, attribution inline (a checkout cannot verify who typed the bytes);
nothing staged, HEAD `3143049`. 11 (prose) **BLOCK** — Finding 1.

## Evidence re-derived
- Comment-only: a literal-aware stripper over all 13 `Source/` files, `3143049:` vs working tree →
  0 code differences. Controls: appending `++MatchEpoch;` → DIFF; editing a clause-name literal in
  `StratSaveSlotClauses.cpp` → DIFF. 80 `Tests/` lines, 0 without `//`; 56 engineer lines.
- Anchors in `StratMatchSubsystem.cpp` each resolve once: `TearDownPresentation();` `:243` (only
  call; definition `:3712`), banners `:208`/`:239`, arm texts `:216`/`:225`, `DescribeRefusal(TEXT("…")`
  `:299`/`:323`/`:403`, `Bridge = MakePimpl<FStratBridge>();` `:293`.
- Dated figures: `283d711` `:178/196/203/219/266/286/366`; `d59bf9b` `:202/220/227/243/308/328/408`.
- Writers: `DoneUnits` add `StratSelectionMachine.cpp:168` (Wait), `:411` (Attack); clear `:420`
  (EndTurn), `:506` (`Reset`). `LockedUnits` `:495/:499` via `SetLockedThisTurn`, sole caller
  `StratGuidedOpening.cpp:465` in `PublishLocks`, called only from `Observe` (`:182/204/243`); clear
  `:507`. Sole `SelectionMachine.Reset()` caller `StratPlayerController.cpp:1637` in
  `SyncPresentationToMatchEpoch`. `DecorateForPresentation`: sync `:1450`, `TryArmGuidedOpening()`
  `:1458`, `Observe` `:1465`. Every new writer claim true.
- Report `reportCreatedOn 2026.09.10-20.38.51`, 479 all Success, 0 failed/notRun/sWW; five
  `T-SAVE-04` clauses errors 0. `--not-before 2026.09.10-20.37.34` CLEAN; `--pin-to-tree` CLEAN
  (newest 20.31.55 UTC); sweep CLEAN; citation CLEAN; control `--not-before 2026.09.10-23.59.59`
  → `STALE REPORT` exit 1. Reset clause blob `fcec6baf` at `f53ca26`, `d59bf9b`, `3143049`, tree.
- Prior findings all cleared: (1) present-tense citations are statement anchors, remaining numbers
  tied to a commit in the same sentence; (2) both sites fixed and received, wrap-aware sweep
  (controlled on `d59bf9b`) finds no phrase-family hit left in `Tests/`; (3) clause file received,
  attribution openly resting on an out-of-tree transcript.

## Findings
**1. `global.md` understates `engine.md`'s open debts.** Owner: coordinator.
`global.md:40-42`: *"`engine.md` carries one debt: a lock-gate comment in `StratSelectionMachine.cpp`
that has been false since wave B2."* `engine.md:138`'s `### Debts taken on, 2026-09-10 (T-SAVE-04
citation repair)` lists TWO debts, neither of which is that one: `GetMatchEpoch`'s retraction still
carries dated line numbers; the 2026-09-03 entry's *"UNCOMMITTED AND UNMERGED"* lede is unstamped and
cites a report no longer on disk. "Unobservable today" appears only in a *"NOT CHANGED, AND WHY"*
bullet (`engine.md:127-130`) with no discharge condition. To clear: state the true count and name
all three, or "two recorded debts plus one known-false comment left unfixed".

## Observations
- `Source/StratPlay/StratUnitActor.h:1172-1173` *"`UStratMatchSubsystem::StartMatchInternal` calls
  `TearDownPresentation()` unconditionally and that destroys every unit actor"* survives — arrived
  in `0b17685` (2026-09-03) after base `283d711`; conclusion true; neither lane's sweep could see it
  (engineer matched a narrower phrase; test lane swept `Tests/` only). `strat-gameplay-engineer`'s.
  Non-gating.
- "+24 above / +42 below" (`StratMatchSubsystem.h:2782-2783`, repeated in `engine.md`, `global.md`)
  is slightly imprecise: the 18-line insertion lands two lines after the call (`@@ -221,0 +246,18`),
  so the call and old `:220-221` moved +24; every cited post-call site did move +42.
- `global.md` stamp *"Both halves of that are now history"* sits on a three-item sentence; the
  proposed-clauses item is not history.
- `engine.md`'s *"a planted code line shown passing the same filter"* is ambiguous; the reviewer's
  own control settles comment-only independently.
- Carried items acceptable: "Unobservable today" (`StratSelectionMachine.cpp:239`, false since B2,
  nothing derived from it) once recorded as a debt; the OPEN clause-ID conflict (bracketed at both
  ends of `tests.md`, stated in `global.md`) needs a user ruling in `decisions.md`.
- This report and its predecessors live outside the repo, not under `Tools/architect/gate_reports/`.

VERDICT: BLOCK
