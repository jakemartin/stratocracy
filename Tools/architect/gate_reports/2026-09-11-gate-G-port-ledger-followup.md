# Gate G: follow-up to closeout 7e65587 (suite gate main(argv) port, plugin-port ledger, F6 persisted) — strat-integration-reviewer

> Persisted by the `coordinator` from the `strat-integration-reviewer` dispatch's final report,
> 2026-09-11. Reviewer agent id `abba3ebdd9e608e46`. The reviewer is read-only and writes no file.
> Scope, command table and checklist summarised; findings and observations verbatim in substance.

Scope: 5 files, all under Tools/architect/, written by strat-data-steward; `git diff --stat -- Source
Content Config Data .claude` empty. All five commands exit 0: banner sweep CLEAN; sweep --self-test
ALL FIXTURES CORRECT; doc citation gate CLEAN; strat_suite_report_gate.py --self-test ALL FIXTURES
CORRECT (four new `[OK] main …` lines); --pin-to-tree CLEAN.
Port faithful to ue-agent-kit ac792af (whole-file diff 15 lines: one docstring phrase plus the
ue-/strat- rename). Mutants M1 (False for args.pin_to_tree), M2 (None for args.not_before), M3b, M4
each go red; M5 (M1 plus unwiring _main_level_cases) goes green, so the wiring is what makes M1
visible. CI invocations (--self-test, bare, --not-before) unaffected. Ledger: `ports` PORTS IN STEP
exit 0; pins current normalised bytes of all three ports (accept taken after the edit); can go red
(port-drift, PLUGIN moved, unrecorded) on disposable copies. F6 copy cmp-identical, ends VERDICT: PASS.
Checklist 1-12 pass.

## Findings
1. The `data.md` entry claims an upstream filing that does not exist, and the new ledger now suppresses the only instrument that would raise it again. Load-bearing record defect. Quoted: "One PROJECT-SIDE FIX the plugin lacks: this file's own 2026-09-11 LIVE COUNT MISSING two-cause message and its fixture (`data.md`'s prior entry, above) — filed upstream below." Nothing below it files anything upstream, and the plugin's banner_sweep.py at ac792af still lacks the fix. This same change certified that divergence into plugin_ports.json, so `ports` reports it accepted; a next session reads that it is filed and does not file it. Owner: strat-data-steward.

## Observations
1. Handoff stamp 1 says "and the follow-up's own gate report is persisted too" — false in the gated tree, and contradicted by stamp 2's "Nothing about the follow-up's own gate is spoken to here". Becomes true only once the coordinator persists this report; "the follow-up's own gate report" (singular) is ambiguous if re-gated.
2. data.md's older "OPEN, UPSTREAM, NOT FILED THIS PASS." item (the main()-level fixture; "drift between this file and its plugin source is unguarded in either direction") is now resolved on both halves and not stamped at that sentence.
3. One N/N inside a code span as quoted gate output; not a live count.
4. No inline actor in the new data.md entry (sole-writer convention).
5. Mixed line endings across ports; the ledger hashes normalised bytes.
6. Mutant M3 (plain parse_args([])) not counted; M3b cited instead.

VERDICT: BLOCK
