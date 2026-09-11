# Gate D -- T-SAVE-04 -> T-INT-05 clause rename (third pass, over `7f0e05b`) -- strat-integration-reviewer

Persisted verbatim-in-substance by the coordinator from the reviewer's returned report, 2026-09-10.
Gated tree: uncommitted working tree on `master` over `7f0e05b82300ee5ee6b2ae32354ade03a04bb0f7`,
ten modified tracked files (four `Source/StratPlay/` headers, two `Tests/*.cpp`, `decisions.md`,
`engine.md`, `tests.md`, `global.md`); `HOW_TO_PLAY.md` untracked and out of scope.

## Standing checklist -- all pass
Vendored bytes untouched (`git diff --stat -- Source/StratRules Data` empty); no `strat::` outside
StratBridge/StratRules; no vendored header before UHT in the four headers; TPimplPtr rule holds;
module arrows unchanged (16 rows, 10 names, guarded StratRules edge in `StratBridge.Build.cs`); no
new module; `Source/Stratocracy/` untouched; no `/Game/` literal added; no StratUI change; lane
discipline held (engineer: headers + engine.md; test-author: Tests/ + tests.md; steward:
decisions.md; coordinator: global.md); nothing staged.

## Pass-specific -- all pass
- Behavioural bytes: HEAD version with `T-SAVE-04`->`T-INT-05` substituted, comments stripped
  (literals kept), IDENTICAL to working file for all six Source files. Control: the two .cpp
  files read DIFFERENT without the substitution. numstat `1 1` x4 headers, `8 8` load file,
  `27 20` reset file.
- Message prefixes (4 load, 10 reset) ID-only; the `RETRACTED>` paragraph in the reset file sound.
- Old full names, wrap-aware over Source/, Tools/architect/, .agents/, .claude/, CLAUDE.md: zero
  in Source/; record hits are rename statements or carry an inline rename bracket/stamp. Surviving
  non-Tests `T-SAVE-04`: real ID in `StratBridge.cpp` and `StratMatchSubsystem.cpp`, event label in
  `StratBuildAffordance.h`. Tests/ holds 47, down from 71.
- Report covers final tree: `reportCreatedOn 2026.09.10-21.37.28` UTC = 17:37:28 EDT = index.json
  mtime; newest Source file 17:35:26 EDT; StratPlay DLL relinked 17:36:12 EDT. Report vs tree
  macro names (479 each) set-equal both directions; both new names Success, neither old present.
- `strat_suite_report_gate.py --pin-to-tree` CLEAN exit 0; `--not-before 2026.09.10-21.36.16`
  CLEAN exit 0; both flags together refused exit 1; `--self-test` correct.
  `strat_banner_sweep.py` SWEEP CLEAN exit 0. `strat_doc_citation_gate.py` CLEAN (56, 0 live).

## Findings

1. **decisions.md -- a correction pointing at a sentence that is not there.** Quoted: *"The
   `FStratLaneTestBase`-derived class names named just above never existed in this tree -- no such
   class is defined anywhere under `Source/` -- and that sentence is corrected in place rather
   than repeated."* Nothing above names such class names; `git log -S FStratLaneTestBase` = 0
   commits, so the corrected sentence was never published and is gone. The only class names just
   above are `FStratLoadClearsControllerSidePresentationStateTest` and
   `FStratSelectionMachineResetEmptiesAllThreeFieldsTest`, which exist -- so read literally it
   says two real classes never existed. Owner: strat-data-steward.

2. **tests.md -- quotes words decisions.md no longer contains.** Quoted: *"A FINDING FOR THE
   STEWARD'S FILE, NOT FIXED HERE: the ruling entry assigns this lane "their
   `FStratLaneTestBase`-derived class names""*. Present tense; the quoted phrase occurs zero times
   in decisions.md. Removed by the steward's later dispatch (decisions.md mtime 17:44:51 EDT after
   tests.md 17:41:48 EDT). Sequential dispatch did not prevent it. Owner: strat-test-author.

3. **engine.md -- two present-tense claims falsified by the later test-lane dispatch.**
   (a) *"FOUR COMMENTS NOW NAME `Stratocracy.StratPlay.T-INT-05.LoadClearsControllerSidePresentationState`,
   A TEST THAT DOES NOT YET EXIST UNDER THAT NAME"* -- false; the macro declares it and the
   21:37:28 report exports it Success. (b) the debt *"Four `Source/StratPlay/` comments name a
   clause the suite does not yet report under that name"* -- false for the same reason; its own
   discharge condition is met and it still reads open. Owner: strat-gameplay-engineer.

## Observations (not gating)
- decisions.md GDD quotes labelled "verified verbatim by grep" substitute `->` for the arrow and
  `--` for the em dash; the T-SAVE-04 sentence also wraps in the GDD snapshot. A literal grep
  returns nothing, though the meaning is exact.
- decisions.md's control is printed as `AND NOT T-SAVE-04`; the file text is AND NOT `T-SAVE-04`
  (`StratSaveSlotClauses.cpp`); the printed form greps to nothing, and it is a phrase, not "a known
  identifier". tests.md quotes it correctly.
- tests.md arithmetic "71, less the 27 occurrences renamed": 26 renamed (8 load, 18 reset) and 1
  went with the retracted paragraph. Totals 71 -> 47 correct.
- engine.md predicts `--pin-to-tree` "will read the existing report as STALE"; it reads CLEAN.
  Framed as prediction; belongs with finding 3's repair.
- global.md's stamp sits directly under the historical sentence -- same shape as the second-pass
  stamp, sweep accepts it; judged announced at the sentence.

VERDICT: BLOCK
