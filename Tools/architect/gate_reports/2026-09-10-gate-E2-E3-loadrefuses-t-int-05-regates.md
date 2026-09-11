# Gates E2 and E3 -- re-gates of the LoadRefusesAnUnconfiguredSubsystem rename to T-INT-05, over `72bb24b` -- strat-integration-reviewer

Persisted in substance by the coordinator, 2026-09-10. Prior: gate E (BLOCK, 1),
`E:\MultiAgent\briefs\2026-09-10-gate-E-loadrefuses-t-int-05.md`.

## E2 -- VERDICT: BLOCK (1 finding)
Only decisions.md moved since E. E's finding resolved: the quote "an unconfigured subsystem
refuses a load by name" greps to exactly one line, `StratSaveSlotClauses.cpp` (the header);
the old "T-SAVE-04 -- an unconfigured" has zero hits. Checklist 1-12 pass; `--pin-to-tree` CLEAN
(479/479); banner sweep CLEAN (74, 1 live); citation gate CLEAN (56, 0 live).
Finding: decisions.md new entry, STATE bullet -- *"The rename is carried out by
`strat-test-author` in a later dispatch; its outcome is not stated here."* -- contradicted by the
entry's own re-anchored text and by its stamp on the entry above, both of which state the outcome.
Written before the rename landed; the same pass made it false. Owner: strat-data-steward.
Observations: stamp's backticked `T-INT-05` not a literal substring of the heading (locator still
unique); "renamed ... by the entry below" credits the ruling not the test-author (loose, not
false); "re-verified" uncheckable from a checkout.

## E3 -- VERDICT: PASS (zero findings)
Only decisions.md moved since E (20:18:26 local); global.md 20:05:11, tests.md 20:03:50,
`StratSaveSlotClauses.cpp` 19:57:25 local = 23:57:25 UTC, before report `reportCreatedOn
2026.09.11-00.00.39`; DLL 19:59:15 local; `.cpp` diff still `7 7`. Checklist 1-12 pass.
`--pin-to-tree` SUITE REPORT GATE CLEAN, exit 0, 479/479; banner sweep CLEAN, exit 0 (74, 1
live); citation gate CLEAN, exit 0 (56, 0 live); exit codes read unpiped.
E2's finding resolved: STATE bullet now "RULED AND CARRIED OUT by `strat-test-author`", pointing
at tests.md's entry heading "`LoadRefusesAnUnconfiguredSubsystem` NOW RIDES `T-INT-05`, BY THE
USER'S RULING" (present); the rename is in the tree (header and macro read T-INT-05). Every other
claim and pointer opened and holds (header phrase, PRECONDITION wording, "do NOT go red on the
regression", GDD §4.9 row, the closed "WHAT THIS RULING DOES NOT COVER" item, the stamp's
did-ride-at-the-time claim at both 7f0e05b and 72bb24b). No draft narration.
Observations: stamp credits the rename to the ruling record, not the byte-writer (not false);
"no worktree or wave in flight" true only in the "in flight" sense (slot-1, slot-2 standing,
clean, merged); menu/route/"shown before ruling" uncheckable from a checkout.

VERDICT: PASS
