# Gate I: 2026-09-12 residual-findings pass (sixth pass) — strat-integration-reviewer

> Persisted by the `coordinator` from the `strat-integration-reviewer` dispatch's final report,
> 2026-09-12. Reviewer agent id `a44dad2cda476b9ec`. The reviewer is read-only and writes no file.
> Scope, checklist and tool output summarised; findings and observations verbatim in substance.

Scope: 12 modified files, nothing staged, over HEAD 83f33bf. Checklist 1-12 pass (16-row module
derivation, 10-name census, the guarded monolithic edge intact, vendored bytes untouched).
Comment-only verified by token stream with comments stripped and literals kept: 7 of 8 files
IDENTICAL; StratSoundDirectorCallSite.cpp differs by exactly one literal pair, the FIRST argument
of TestNull (the message), with the asserted value, clause name and outcome unchanged. Mutant
controls speak both ways. Report index.json (UTF-8 BOM) reportCreatedOn 2026.09.12-03.24.21,
479/0/0/0; report names and tree macro declarations set-equal in both directions. Engineer's two
premises verified: UStratSoundDirector::SoundBank is private, Transient, no editable specifier, so
no Blueprint default exists to read; the three GameMode CDOs are pinned by the committed clause
Stratocracy.StratPlay.GATE-AUDIO.EveryShippedGameModeNamesTheSameSoundBank. data.md's one-word
change is sole, and its CORRECTED bracket and DISCHARGED stamp survive verbatim.
Tools, unpiped, all EXIT 0: banner sweep CLEAN (76 claims, 1 live, 75 stamped); sweep --self-test
ALL FIXTURES CORRECT; doc citation gate CLEAN; suite report gate --pin-to-tree CLEAN; template_sync
ports PORTS IN STEP.

## Findings

1. BLOCK -- the corrected claim survives, uncorrected and unstamped, three lines above a paragraph
   this pass did edit. Source/StratPlay/StratSoundDirector.h:340-341: "why at length; the short form
   is that an early return on a null bank makes every clause / in this feature vacuous in the
   shipped configuration." That is the pass's own target defect verbatim: a null bank asserted to be
   the shipped configuration. It sits inside EmitCue's doc comment, and the diff's fourth hunk in
   this file edits the paragraph four lines below it, so the site was read past, not unreachable. It
   is worse than an untouched file would be, because the file's new header stamp enumerates the
   scope -- "AND SO DID THREE MORE BELOW AND TWO IN THE .cpp" -- an enumeration this site falsifies.
   Owner: strat-gameplay-engineer.

2. BLOCK -- two TestEqual message literals still name NoBank "the shipped disposition", in the file
   whose header was corrected to disclaim exactly that. StratSoundDirectorCallSite.cpp:658
   'TestEqual(TEXT("GATE-AUDIO: and recorded with the shipped disposition"),' and :1576
   'TestEqual(TEXT("GATE-AUDIO: recorded with the shipped disposition"),'. Both assert
   EStratSoundDisposition::NoBank, while the same file's corrected header now says "Nothing in THIS
   file opens a shipped asset, so it asserts nothing either way about what the shipped Blueprints
   assign." Unlike a comment, these reach the suite output on a red run. Same category as the one
   TestNull message the pass did change. Confirmed the only two residuals of this shape by a Python
   comment-block census of all eight edited files. Owner: strat-test-author.

3. BLOCK -- three record claims of a complete, measured census are falsified by findings 1 and 2.
   engine.md's 2026-09-12 entry: "The brief named six sites; a phrase census found twelve in
   production code, and the count is the measured one." The measured count in production code is at
   least thirteen. tests.md frames its site list as the output of a widened census and misses two
   message literals of the same kind in the same file. global.md's sixth-pass banner restates both
   counts and inherits the error. Load-bearing rather than cosmetic: the warrant for these entries
   is measurement, and a census presented as measured-and-complete tells the next reader the subject
   is closed. Owners: strat-gameplay-engineer (engine.md), strat-test-author (tests.md),
   strat-data-steward (global.md), after findings 1 and 2 land.

## Observations

- engine.md's omitted build line is NOT a defect: the entry claims no suite and nothing a build
  would support or refute, global.md flags the absence, and the later rebuild plus the 479/479
  report covers the final tree. A measurement taken and not recorded -- a recordkeeping debt for the
  finding-3 rewrite, not a gate.
- Sentences checked and deliberately NOT reported: StratSoundDirector.h:108-110, :226, :417, :457,
  StratAudioSettings.h:59-67 and StratAudioVolumeClauses.cpp:40 use "shipped state/default" about
  the SAVE SLOT and unity gains, not the bank. Different subject; correcting them would be wrong.
- StratSoundDirector.h's stamp says "TWO IN THE .cpp" while engine.md says three. Both defensible
  (the .cpp's top block read "in its shipped configuration" and carries its own stamp); recorded
  because the next reader will notice.
- Shell trap measured this session, worth promoting: the Bash-tool heredoc collapses a doubled
  backslash to a single one even under a quoted delimiter, which turned a Python script into a
  SyntaxError and would silently corrupt any regex or path written that way. Verified with cat -A.

VERDICT: BLOCK
