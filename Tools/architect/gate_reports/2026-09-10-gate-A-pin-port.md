Run 2026-09-10 (local) by `strat-integration-reviewer`, read-only, against the UNCOMMITTED working
tree over `d59bf9b` in `E:\MultiAgent\Stratocracy` (9 files, +493 −4, nothing staged). Persisted by
the coordinator outside the repo (findings and quotes verbatim; checklist condensed).

# Gate: pin-port + skill sync — strat-integration-reviewer — VERDICT: BLOCK

## Checklist (condensed)
All structural checks pass: vendored bytes untouched (`rulesCommit 96d93ea…`, `dataCommit c2f5860…`);
0 `strat::` outside the two dirs; no vendored header before UHT; module arrows 16 rows / 10 names;
no new module; no `/Game/` literal beyond the commandlet exception; no `StratUI` change; lanes kept
(`CLAUDE.md` assigns `.claude/skills/` to no lane); nothing staged.
- D1 port = plugin `df8c1a9` + 4 renames, `diff --strip-trailing-cr` exit 0; HEAD's pre-pin file =
  plugin pre-`71c0664` + same renames, exit 0.
- D2 `python -W error … --self-test` ALL FIXTURES CORRECT exit 0; main-tree `--pin-to-tree` CLEAN exit 0
  (newest `StratSelectionMachine.h` 2026.09.10-18.16.13 < report 18.20.14).
- D3 all six steward mutant claims reproduced with the stated kill sets; flag-ignored mutant passes
  `--self-test` (0 `[BAD]`) and passes live (CLEAN exit 0) vs unmutated FAILED/`STALE REPORT` exit 1.
- D4 **BLOCK** (F1). D5 five `neostack-*` equal plugin blob `df8c1a9` after CRLF→LF, 0 CR on disk,
  every HEAD line retained in order, 0 deletions; `template_sync check` exit 1, 12 findings, 0 `[drift]`.
- G `strat_banner_sweep.py --self-test` 0, sweep 0 `SWEEP CLEAN`; `strat_doc_citation_gate.py
  --self-test` 0, `--explain` 0 `DOC CITATION GATE CLEAN`.

## Findings
**F1.** `strat-parallel\SKILL.md:229-230` — `TOP=$(git -C <lane> rev-parse --show-toplevel)` /
`cd "$TOP" && python Tools/architect/strat_suite_report_gate.py --pin-to-tree` runs the LANE's copy of
the gate; both standing lanes carry the old script → `error: unrecognized arguments: --pin-to-tree`,
exit 2 on slot-1 and slot-2. Step 0 runs before the rebase, so every lane cut before this commit hits
it. The integration tree's copy with `--report …/<slot>/Saved/AutomationReport/index.json
--source-root …/<slot>/Source` is CLEAN exit 0 on both. Owner: coordinator.

**F2.** `data.md:49-50` "4 lines removed (the old 4-argument-less signature and its 3 call sites)" and
`:75` "the pre-pin `check()` signature and its three call sites": `check()` has TWO call sites at
HEAD (`:434`, `:494`); the fourth deleted line is the self-test's `report, root, pin = build(Path(td))`;
the old signature took three arguments. Owner: strat-data-steward.

**F3.** `strat-hotseat\SKILL.md:263` "had been false for eleven days" and `strat-parallel\SKILL.md:243`
"eleven days after both had merged": `1576e40`/`ed09973` merged 2026-08-31 19:35 -0400; trial handoff
mtimes 2026-09-10 10:20 -0400 — under ten days; and the hotseat handoff was written 2026-09-10, so it
was never false "for" a span. Upstream `E:\ue-agent-kit\plugins\ue-agent-kit\skills\ue-parallel\SKILL.md:174`
carries the same "eleven". Owner: coordinator here; ue-agent-kit upstream.

## Observations
- Flag-ignored mutant gap is non-gating (port equals upstream; flag confirmed live twice; `--not-before`
  wiring has had the same blindness; a local fix would fork the file). But the skills now make
  `--pin-to-tree` mandatory at every boundary while CI calls only `--self-test`/`--not-before`; a
  regression of that one line would make every boundary silently unpinned and CLEAN. Fix belongs
  upstream as a `main()`-level fixture. `data.md` states it honestly but inside a DONE entry with no
  open item and no upstream route.
- `template_sync check` lists `strat_suite_report_gate.py` as "template-only scripts, NOT MEASURED" —
  future drift from the plugin is unguarded.
- `strat-hotseat:240` "The coordinator does not write `Tools/architect/`" overclaims against
  `CLAUDE.md` (coordinator writes `global.md`, is `content.md` fallback writer, transcribes).
- `strat-parallel:217` "lives beside the lane's draft record entries" names no defined location; the
  steward's `Tools/architect/handoffs/` acceptance covers the hotseat path only.
- CRLF warnings are commit-harmless: HEAD blobs of all eight warned files have 0 CR.

VERDICT: BLOCK
