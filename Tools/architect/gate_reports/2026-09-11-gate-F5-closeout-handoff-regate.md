# Gate F5: 2026-09-11 closeout, narrow re-gate of the handoff rewrite and the F4 copy — strat-integration-reviewer

> Persisted by the `coordinator` from the `strat-integration-reviewer` dispatch's final report,
> 2026-09-11. Reviewer agent id `a6a2e991225d5ca10`. The reviewer is read-only and writes no file.
> Scope and checklist summarised; findings verbatim.

Scope: only the F4 copy and the rewritten handoff moved since F4 (`find -newer` with a control).
F4 copy byte-identical (sha256 67e126b3…c18c7e5). Tool runs: citation gate CLEAN exit 0; sweep CLEAN
exit 0; `--pin-to-tree` CLEAN exit 0, 479/479. Checklist 1-10 and 12 pass; 11 BLOCK (handoff prose).
All seven F4 findings verified fixed.

## Findings

1. The handoff files a pre-existing, never-gated file under this pass's doc split. This meets the bar. File: `Tools/architect/handoffs/2026-09-11-closeout.md`, section "Workspace and ownership". Offending text: "the four `Source/*/.claude/skills/*/SKILL.md` files, and untracked `HOW_TO_PLAY.md` (coordinator's own doc split and skill edits)." `HOW_TO_PLAY.md` has an mtime of 2026-09-08, predates the pass, is not in `global.md`'s DOC SPLIT paragraph, and every 2026-09-11 gate excluded it by instruction. The handoff's "Next command" is a single closeout commit over the tree then a push; nothing warns a resuming session to leave it out. Owner: strat-data-steward. The line should say what the file is: untracked, from before this pass, never gated, and a decision for the user about the closeout commit.

2. The rewrite still describes the tree as it was before the F4 copy was added: "the thirteen 2026-09-10/11 `gate_reports/` files" (14 on disk); "this pass added the F3 report copy and this handoff" (omits F4); "Last `VERDICT:` lines, verbatim" lists F, F2, F3 only while the title reads "gates F/F2/F3/F4". Does not meet the bar. Owner: strat-data-steward.

3. "`global.md`'s earlier-pass `WHAT IS NOT DONE` items — ownership named at each heading there." is false: the fourth-pass and third-pass WHAT IS NOT DONE paragraphs, and the second pass's `HexPrimary` item, name no owner, and they are inline paragraphs, not headings. Does not meet the bar. Owner: strat-data-steward.

## Observations
1. "Four dispatches" is doubtful given the steward's later continuations; no action turns on it.
2. The pointer to `state/data.md`, `decisions.md` for this pass's other steward writes leads nowhere for the gate-report copies and the handoff.
3. `engine.md`'s LEFT OPEN heading holds three bullets, one of them closed by strat-test-author.
4. The filename does not match the `<YYYY-MM-DD>-phase-<N>.md` pattern in strat-hotseat (the relayed brief named this path explicitly).
5. No mechanical guard covers `handoffs/` or `gate_reports/`.
6. Carried open, unchanged: gate F observations 1, 6 (first two bullets), 7; F3 observation 3.

VERDICT: BLOCK
