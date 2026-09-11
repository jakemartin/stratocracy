# Gate E -- rename of LoadRefusesAnUnconfiguredSubsystem to T-INT-05 (fourth pass, over `72bb24b`) -- strat-integration-reviewer

Persisted in substance by the coordinator, 2026-09-10.

Tree: uncommitted, four files (`StratSaveSlotClauses.cpp`, `decisions.md`, `global.md`,
`tests.md`), nothing staged, HEAD = `72bb24be30841fe0e4f8a29b4d6c883481be81f2`.

Checklist 1-12 pass. `.cpp` names-only: numstat `7 7`, 7 lines all name-only (header ID, macro,
five message prefixes); `LoadRefusesAWrongSavedDataVersion` and the "AND NOT `T-SAVE-04`" block
untouched; Tests/ `T-SAVE-04` 47 -> 40 across 116 files. `--pin-to-tree` CLEAN (newest file
23.57.25 UTC predates report); `--not-before 2026.09.10-23.59.34` CLEAN; control
`--not-before 2026.09.11-00.00.40` FAILED exit 1. DLL relinked 23.59.15 UTC. Report/tree names
479 = 479, set-equal both ways; new name Success, old absent. Banner sweep CLEAN (1 live, 73
stamped); citation gate CLEAN (56, 0 live).

## Finding
1. **decisions.md, new entry** -- *"this clause's own header (`StratSaveSlotClauses.cpp`, the
   "T-SAVE-04 -- an unconfigured subsystem refuses a load by name" block) names its subject as a
   PRECONDITION REFUSAL"*. The header now reads `// T-INT-05 -- an unconfigured subsystem refuses
   a load by name`; the quoted phrase has zero hits in Source/. Written by the steward before the
   test-author's rename in the same pass made it false. Owner: strat-data-steward.

## Observations (not gating)
1. The committed decisions.md bullet's *"also rides `T-SAVE-04` without being a header
   mismatch"* is present tense and now false; the new stamp names only "It stays open", though it
   says two sentences later the clause is renamed. "The user was not asked about it in this
   session" -- uncheckable from a checkout.
2. tests.md sites naming the old full name without a RENAMED bracket (a third-pass sweep control,
   a Finding-2 locator, the W6 passage) -- each anchored to its own pass and true of it.
3. engine.md "three other `T-SAVE-04` names" anchored to report 2026.09.10-21.37.28; true of it.
4. tests.md's committed third-pass "115 .cpp/.h files" -- both 7f0e05b and HEAD hold 116.
5. global.md banner and stamps correctly shaped; other new pointers land.

VERDICT: BLOCK
