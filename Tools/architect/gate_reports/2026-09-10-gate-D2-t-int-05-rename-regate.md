# Gate D2 -- re-gate of the T-SAVE-04 -> T-INT-05 rename, over `7f0e05b` -- strat-integration-reviewer

Persisted in substance by the coordinator from the reviewer's returned report, 2026-09-10.
Prior BLOCK: `E:\MultiAgent\briefs\2026-09-10-gate-D-t-int-05-rename.md`.

No `Source/` byte changed since gate D: newest Source file `StratSelectionMachineResetClauses.cpp`
17:35:26 EDT; report `reportCreatedOn 2026.09.10-21.37.28` UTC = 17:37:28 EDT = index.json mtime;
numstat unchanged. Record writes in dispatch order: decisions.md 17:56:14, tests.md 17:59:04,
engine.md 18:01:57; global.md 17:46:07.

Standing checklist 1-12 all pass. `--pin-to-tree` CLEAN (479 macros = counters);
`--not-before 2026.09.10-21.36.16` CLEAN; banner sweep CLEAN (73 claims, 1 live); doc citation
gate CLEAN (56, 0 live). Own parse of index.json: 479 all Success, names set-equal both ways,
both T-INT-05 names Success, old T-SAVE-04 forms absent.

Gate D findings: 1 RESOLVED (FStratLaneTestBase 0 in all four records; class ids stated
positively and verified). 2 RESOLVED (every tests.md phrase decisions.md quotes or says tests.md
confirms is present now; control hit real at StratSaveSlotClauses.cpp). 3 RESOLVED (lede true,
STALE prediction replaced with matching gate output, debt DISCHARGED with checkable evidence).
global.md THIRD PASS banner and its three stamps: true against the tree.

## Finding
1. **decisions.md -- a quote attributed to the wrong entry.** In the ruling's "WHAT THIS RULING
   DOES NOT COVER" bullet: *"and, at the wave-8 entry, "The pre-existing
   `Stratocracy.StratPlay.T-SAVE-04.LoadRefusesAnUnconfiguredSubsystem` is likewise outside that
   sentence ... Renaming it is not this wave's change to make.""* The passage is in tests.md's
   entry headed "2026-08-30 (local), in the LANE tree ... on `feat/title-menu` ... W6'S TITLE/MENU
   SHELL IS PINNED BY 23 CLAUSES" -- W6, not W8; tests.md never uses the label "wave-8" and its
   W8 entries do not contain the passage. Whether the steward's repair introduced it or gate D
   missed it cannot be told: the entry was never committed. Owner: strat-data-steward.

## Observations (not gating)
- tests.md "A plain per-line count ... agrees at both ends: 71 in HEAD": occurrence count is 71;
  a per-LINE count is 70 (one reset-file line carries two). Totals right; wording invites the
  wrong instrument.
- engine.md quotes the W6 entry as "`T-SAVE-04` was **proposed and refused**"; source reads
  "**proposed and refused by the lane it was offered to**" -- truncated inside the bold. Not
  labelled verbatim; meaning intact.
- decisions.md credits the engine half to the "Debts taken on, 2026-09-10 (`T-INT-05` rename,
  engine half)" *entry*; that heading is the debt list, the work is in the `## NEXT` bullet above.
- engine.md "At base `7f0e05b` the macro string in `Tests/` still reads `T-SAVE-04`" -- anchored
  to the base commit, true of it.

VERDICT: BLOCK
