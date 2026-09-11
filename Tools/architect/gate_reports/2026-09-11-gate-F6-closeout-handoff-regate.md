# Gate F6: 2026-09-11 closeout, narrow re-gate of the handoff fix and the F5 copy — strat-integration-reviewer

> Persisted by the `coordinator`, OUTSIDE the repo on purpose: the handoff F6 passed counts the
> 2026-09-10/11 gate reports as fifteen, so committing this sixteenth would falsify a gated file.
> Reviewer agent id `a464ffb3824b86b1e`. Scope and checklist summarised; verdict verbatim.

Scope: only the handoff and the F5 copy moved since F5 (`find -newer` with a control). F5 copy
byte-identical (sha256 db2cfa76…81ecd32). Citation gate CLEAN exit 0; sweep CLEAN exit 0;
`--pin-to-tree` CLEAN exit 0, 479/479. Checklist 1-12 pass. F5 findings 1-3 verified fixed
(HOW_TO_PLAY.md now has its own bullet and the Next command excludes it from the closeout commit;
verdict list F=BLOCK, F2=PASS, F3=PASS, F4=BLOCK, F5=BLOCK matches each persisted file; the owner
sentence matches global.md).

## Findings
None.

## Observations
1. global.md's third-pass paragraph still reads, unstamped: "The four PROPOSED clauses in `engine.md` still carry `T-SAVE-04` names that no ruling backs, and `engine.md` now says so." The fourth-pass copy of that sentence is stamped. Below the bar; owner strat-data-steward.
2. The F5-3 owner list is not complete (two more second-pass items name only a file); "some" hedges it.
3. HOW_TO_PLAY.md is untracked and not gitignored, so `git add -A` would sweep it in; the handoff's exclusion is the only guard.
4. "Four dispatches" is further out of date.
5. The F4/F5 copies' headers say "Persisted by the coordinator" while the handoff lists them as steward-owned; the header describes the briefs/ original.
6. Carried: F5 observations 2-6, including no mechanical guard over handoffs/ or gate_reports/.

VERDICT: PASS
