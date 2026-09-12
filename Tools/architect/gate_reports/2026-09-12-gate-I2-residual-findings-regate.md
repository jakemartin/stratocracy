# Gate I2: re-gate of the 2026-09-12 residual-findings pass — strat-integration-reviewer

> Persisted by the `coordinator` from the `strat-integration-reviewer` dispatch's final report,
> 2026-09-12. Reviewer agent id `a28e023749bc37f8d`. The reviewer is read-only and writes no file.
> Scope, checklist and tool output summarised; verdict and observations in substance.

Scope: 12 modified files over HEAD 83f33bf, nothing staged; the persisted gate I copy is the one
expected untracked addition. Checklist 1-12 pass (16-row derivation, 10-name census, the guarded
monolithic edge, vendored bytes untouched, lane discipline clean).

Findings 1, 2 and 3 all verified fixed, with the replacement text read and found true. BOTH
censuses were re-run independently rather than taken on report:
- Production: 137 .h/.cpp files (the lane's 143 is the same scope plus six non-vendored .cs files --
  reconciled, not a discrepancy). THIRTEEN corrected claim sites, split exactly as claimed
  (StratSoundDirector.h 5, .cpp 3, StratSoundBank.h 2, StratMatchSubsystem.cpp 2,
  StratMatchSubsystem.h 1), ZERO residuals. "Zero production literals carry the claim" confirmed by
  a comment-stripping tokenizer over all 137 files: 0 hits, with a control injecting one literal
  that the same channel found.
- Tests: 116 files; using the lane's own needles, EXACTLY 65 bank-subject sites, ZERO residuals.
  Control: the literal channel against `git show HEAD:` of the pre-edit file returns 3 message
  literals and 0 on the current tree.
Token stream, comments stripped and literals kept: 7 of 8 files IDENTICAL; StratSoundDirectorCallSite.cpp
differs by exactly 3 literal pairs, each the FIRST argument of a TestNull/TestEqual -- a message. No
asserted value, clause name or outcome moved. Mutant controls speak in every direction; the
reviewer discarded its own first value control as silent and re-pointed it rather than counting it.
Report: index.json (UTF-8 BOM) reportCreatedOn 2026.09.12-03.51.28, 479/0/0/0, postdating the newest
Source file. Persisted gate I copy byte-identical, 5332 bytes, sha256 fa061c03... both sides.
Tools, unpiped, all exit 0: banner sweep CLEAN; --self-test ALL FIXTURES CORRECT (76 fixtures);
doc citation gate CLEAN; suite report gate --pin-to-tree CLEAN; template_sync ports PORTS IN STEP.

## Findings
None.

## Observations
1. tests.md's census scope over-claims by one file and the omitted file is clean: 116 such files
   exist, not 115 -- Source/Stratocracy/StratData/Tests/StratDataParity.cpp sits under
   Source/<Module>/StratData/Tests/, outside the three Source/*/Tests roots walked. The reviewer
   censused the superset itself and that file yields zero candidates, so the entry's load-bearing
   conclusion holds over a WIDER scope than it claims. global.md's restatement is literally true.
   Not gating. Owner if ever tidied: strat-test-author.
2. tests.md's raw pre-narrowing figure ("921 occurrences of ship*") is not reproducible to the
   digit -- the reviewer measures 748, 840, 953 or 1083 depending on the counting unit. It is
   pre-narrowing and carries no weight; both load-bearing figures beside it (65 sites, ZERO
   residuals) reproduce exactly under the lane's own stated needles.
3. engine.md says "Six bank-adjacent 'shipped' sentences survive the census unstamped" and then
   enumerates five categories, one of which it also describes as carrying its own 2026-09-11 stamp,
   so "unstamped" does loose work. Every sentence it names checks out against the tree. Cosmetic.
4. Line endings verified from the bytes rather than with grep: StratSoundDirector.h is 0 CRLF / 575
   LF, matching engine.md. git's LF->CRLF warnings are normalisation notices, not content changes.
5. The 2026-09-12 record entries name the reviewer as the cause of two corrections but state no
   verdict for this pass and narrate no commit or push. That is the correct shape.
6. Shell trap re-confirmed: the Bash heredoc collapses a doubled backslash even under a quoted
   delimiter. Avoided by writing both python instruments with no backslash byte at all and asserting
   chr(92) not in the file at the top of each -- the assertion is the control, so a corrupted script
   fails loudly instead of silently matching nothing.

VERDICT: PASS
