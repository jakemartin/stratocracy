# Gate D4 -- third re-gate of the T-SAVE-04 -> T-INT-05 rename, over `7f0e05b` -- strat-integration-reviewer

Persisted in substance by the coordinator, 2026-09-10. Priors: D (BLOCK 3), D2 (BLOCK 1), D3 (BLOCK 2),
all in record prose, none in code.

Since D3 only `Tools/architect/state/decisions.md` moved (18:14:53 -> 18:26:05 EDT); tests.md
17:59:04, engine.md 18:01:57, global.md 17:46:07 unchanged; newest Source file
`StratSelectionMachineResetClauses.cpp` 17:35:26.91 EDT; DLL 17:36:12 EDT; report
`reportCreatedOn 2026.09.10-21.37.28` UTC = 17:37:28 EDT = index.json mtime. decisions.md diff is
one pure append hunk `@@ -1102,3 +1102,48 @@`. `.neostack/skills-manifest.json` mtime moved, no
git content change.

Checklist 1-12 all pass (vendored untouched; no strat:: outside StratBridge/StratRules, control
143 legal hits; no vendored header before UHT; no TUniquePtr; 16 arrow rows, 10 census names; no
new module; Source/Stratocracy untouched; no /Game/; no StratUI change; lane discipline; no new
files; nothing staged, HEAD 7f0e05b).

`strat_suite_report_gate.py --pin-to-tree` SUITE REPORT GATE CLEAN, 479/479, census 479.
`strat_banner_sweep.py` SWEEP CLEAN (73 claims, 1 live). `strat_doc_citation_gate.py` CLEAN (56, 0 live).

D3 findings: both gone. Every claim and pointer in the rewritten entry opened and landed (renames,
class ids, §4.10 and §4.9 quotes inside their sections, first `T-SAVE-04` in decisions.md is the
entry's own heading, relay-not-ruling matches tests.md, LoadRefusesAnUnconfiguredSubsystem present,
tests.md bullet in the `3143049` entry, tests.md and engine.md headings, four headers). Inbound
cross-references from tests.md, engine.md, global.md and the reset clause file still land. No
self-narration.

## Findings
None.

## Observations
- "Discharges tests.md's entry based at `3143049`, the bullet headed ..." reads as a full
  discharge; tests.md closes it "for the two clauses" only and holds the
  LoadRefusesAnUnconfiguredSubsystem half OPEN. Stated one bullet earlier; not false in context.
- "presentation-block state, which is what both clauses pin" is not exclusive: the clauses also
  pin build-affordance focus and SelectedUnitId, not members of UiPresentationUnit.
- "no worktree or wave in flight" leans on "in flight": slot-1 and slot-2 exist, clean, merged.
- Menu and "the user was not asked" uncheckable from a checkout; agree with global.md.
- §4.9 quote not marked normalised; differs only by line wrap.
- Earlier D3 and D2 observations stand unchanged.

VERDICT: PASS
