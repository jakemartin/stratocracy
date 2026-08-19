# Evidence: `08-combat-pairing-gate`

Combat-outcome milestone, phase 3, piece (c) — the machine-repeatable pairing gate.
Editor CLOSED. Steward's lane: `Tools/architect/` only.

## The correction, stated first because it is the phase's main result

The phase brief said the gate pairs `STRAT-CMD accepted kind=Attack` 1:1 with
`STRAT-COMBAT resolved`. That is wrong. `STRAT-CMD accepted` is the **human** selection
applier's line (`StratSelectionMachine.cpp`); the AI path never emits it —
`StratAiTurnRunner.cpp:242-247` says so explicitly: routing AI commands through
`StratSubmitSelectionCommand` would mean inventing a click. The AI path emits
`STRAT-AI applied` and submits through `SubmitAttackAtHex → SubmitAttack → SubmitStamped →
Submit`, where `STRAT-COMBAT` actually fires.

Measured directly against `Saved/Logs/Stratocracy.log` (5169 lines, session opened
`08/14/26 10:16:47`), sliced to `Stratocracy.StratPlay.T-INT-05.BothSidesAiReachesAResultWithinTheBound`'s
own `Test Started.` / `Test Completed.` markers (lines 4179..4474 in this run — derived, not
transcribed, by `grep -n "BothSidesAiReachesAResultWithinTheBound" ... | grep -E "Test
Started|Test Completed"`, see below):

```
grep -c "STRAT-CMD accepted kind=Attack" slice.log   -> 0
grep -c "STRAT-AI applied kind=Attack"   slice.log   -> 68
grep -c "STRAT-COMBAT resolved"          slice.log   -> 68
grep -c "STRAT-AI refused"               slice.log   -> 1  (kind=EndTurn, [T-SAVE-05])
grep -c "STRAT-COMBAT divergence"        slice.log   -> 0
```

The real pairing is `STRAT-AI applied kind=Attack` <-> `STRAT-COMBAT resolved`, and the gate
(`Tools/architect/strat_combat_pairing_gate.py`) asserts it by **ordered identity** on
`(unit, hex)` vs `(attacker, hex)` — not just that the two totals agree. `fixtures/fail_ordering_scramble.log`
demonstrates why the distinction matters: same two `(unit, hex)` pairs, wrong order, same
totals on both sides, and the gate still fails it (see `gate_self_test_output.txt`).

## Commands run, verbatim, for every number in this record

```
$ wc -l Saved/Logs/Stratocracy.log
5169 Saved/Logs/Stratocracy.log

$ grep -n "BothSidesAiReachesAResultWithinTheBound" Saved/Logs/Stratocracy.log | grep -E "Test Started|Test Completed"
4179:[...]LogAutomationController: Display: Test Started. Name={BothSidesAiReachesAResultWithinTheBound} Path={Stratocracy.StratPlay.T-INT-05.BothSidesAiReachesAResultWithinTheBound}
4474:[...]LogAutomationController: Display: Test Completed. Result={Success} Name={BothSidesAiReachesAResultWithinTheBound} Path={Stratocracy.StratPlay.T-INT-05.BothSidesAiReachesAResultWithinTheBound}

$ sed -n '4179,4474p' Saved/Logs/Stratocracy.log > real-run/t-int-05-both-sides-ai-slice.log
$ wc -l real-run/t-int-05-both-sides-ai-slice.log
296 real-run/t-int-05-both-sides-ai-slice.log

$ grep -c "STRAT-CMD accepted kind=Attack" real-run/t-int-05-both-sides-ai-slice.log
0
$ grep -c "STRAT-AI applied kind=Attack" real-run/t-int-05-both-sides-ai-slice.log
68
$ grep -c "STRAT-COMBAT resolved" real-run/t-int-05-both-sides-ai-slice.log
68
$ grep -c "STRAT-AI refused" real-run/t-int-05-both-sides-ai-slice.log
1
$ grep "STRAT-AI refused" real-run/t-int-05-both-sides-ai-slice.log
[...]LogStratPlay: Verbose: STRAT-AI refused phase=apply kind=EndTurn unit=-1 hex=0,0 def=-1 target=-1 turn=6 side=1 reason=[T-SAVE-05] no match is running
$ grep -o "STRAT-AI applied kind=[A-Za-z]*" real-run/t-int-05-both-sides-ai-slice.log | sort | uniq -c
     68 STRAT-AI applied kind=Attack
     22 STRAT-AI applied kind=Build
     11 STRAT-AI applied kind=EndTurn
     55 STRAT-AI applied kind=Move

$ grep -c "STRAT-COMBAT resolved" Saved/Logs/Stratocracy.log
493   # whole-log — confirms slicing is mandatory: 425 of these come from tests that
      # submit directly and emit no STRAT-AI line at all; an unsliced pairing check fails
      # on both directions for a reason unrelated to what this gate checks.

$ tail -n 6 Saved/Logs/Stratocracy.log
[...] **** TEST COMPLETE. EXIT CODE: 0 ****
[...] LogWindows: FPlatformMisc::RequestExitWithStatus(...)
[...] LogCore: Engine exit requested (reason: Win RequestExit)
# confirms: no "Log file closed" line in this log. Slicing is bounded by the test's OWN
# Test Started/Completed markers, never by EOF or by this end-of-run marker.

$ grep -n "STRAT-COMBAT" Saved/Logs/Stratocracy.log | grep -v "LogStratBridge:" | head -5
3649:[...]LogAutomationController: STRAT-COMBAT resolved attacker=10 defender=5 hex=3,4 ...
# the commentary-echo trap: LogAutomationController echoes a test's captured log lines
# verbatim between its own BeginEvents:/EndEvents: markers. Byte-identical payload to the
# real LogStratBridge: emission 4 lines above it (line 3645). Confirms the category anchor
# in find_strat_combat_payload (LogStratBridge:, not a bare "STRAT-COMBAT" substring) is
# necessary, not defensive posture.

$ python Tools/architect/strat_combat_pairing_gate.py Saved/Logs/Stratocracy.log
[see real-run/gate_output.txt]
PASS, EXIT=0

$ python Tools/architect/strat_combat_pairing_gate.py --self-test
[see self_test_internal_fixtures_output.txt]
SELF-TEST: ALL FIXTURES CORRECT

$ for f in fixtures/*.log; do python Tools/architect/strat_combat_pairing_gate.py "$f" \
    --test-path "Stratocracy.Fixture.FIX-01.FixtureTest"; echo "EXIT=$?"; done
[see gate_self_test_output.txt]
```

## The gate can fail — 11 checked-in fixtures, each proving one specific failure mode

**CORRECTED, 2026-08-14, post-milestone (not a phase) — this section originally said "ten"
throughout, written before `fail_pre_sliced_zero_events.log` existed. `47ec9bf` added that
eleventh fixture (the `--pre-sliced` zero-event guard debt discharge) but never updated this
file, and `gate_self_test_output.txt` went stale the same way — `git ls-files 'fixtures/*.log'`
returns 11, `grep -c "^== " gate_self_test_output.txt` returned 10 until this correction
regenerated it. Caught by `strat-data-steward` closing the fixture-verdict binding debt recorded
in `state.md`, not by a re-gate. Both artifacts have been regenerated from a real run and
`Tools/architect/strat_fixture_verdict_binding.py` now binds every checked-in fixture to its
expected verdict AND its expected invocation, re-runnably — see that module's own docstring.
**CORRECTED AGAIN, 2026-08-14, later pass:** the derivation command below this note, and the one
just used in this sentence, originally read `git ls-files fixtures/ | wc -l` (no `*.log` glob) —
that counts tracked files in the directory, not `.log` fixtures, and returned 11 only because
`fixtures/README.md` (added by the same pass that added the eleventh fixture) was still untracked
at measurement time. The glob form reads 11 both now and after a hypothetical commit of the
README — verified: `git ls-files --cached --others --exclude-standard 'fixtures/*.log'` (the
post-commit-tracked proxy) still returns 11, while the same query without the `*.log` glob
returns 12.**

Per the phase-D precedent (`evidence/07-ai-opponent/gate_self_test_output.txt`) and the
recorded lesson that the phase-D gate was itself blocked once for fixtures that could not
fail it, `fixtures/` holds eleven small `.log` files, each run through the real CLI (not a
mocked function). Ten carry their own `Test Started.`/`Test Completed.` markers under
`Stratocracy.Fixture.FIX-01.FixtureTest`; the eleventh (`fail_pre_sliced_zero_events.log`) is
run with `--pre-sliced` and no `--test-path` — `--pre-sliced` ignores `--test-path` entirely,
and this file is empty on disk with no markers to slice on regardless, so it cannot be run the
same way as the other ten. Split derived from the directory, not counted by eye:

```
$ git ls-files 'fixtures/*.log' | wc -l
11
$ ls fixtures/ | grep -c '^fail_'
8
$ ls fixtures/ | grep -c '^pass_'
3
```

— eight FAIL fixtures (seven run with `--test-path`, one — `fail_pre_sliced_zero_events.log` —
with `--pre-sliced`), three PASS controls, matching the eight `EXIT=1` / three `EXIT=0` blocks in
`gate_self_test_output.txt` below. (The reviewer's earlier miscount — "eight FAIL fixtures plus
two PASS controls," naming a checked-in "unmatched resolved line" fixture that does not exist —
was in `state.md` only, and predates the eleventh fixture entirely; it is unrelated to this
correction. That "unmatched resolved line" scenario is case 4 of the in-script
`check_self_test()` below, over an inline `tempfile` corpus, and is not one of these eleven
checked-in files; the two sets are kept distinct and are never totalled together.)

| Fixture | Proves |
|---|---|
| `pass_clean_pairing.log` | Two attacks, ordered identity holds → PASS |
| `fail_ordering_scramble.log` | Same `(unit,hex)` multiset, wrong order → FAIL (2 mismatches) — the case a count-only or set-only gate would pass |
| `fail_silent_path_gap.log` | An applied attack with no matching resolved line (the `!bSeeded` silent-path shape) → FAIL, reported as a real gap with the cause named in the message |
| `fail_format_drift.log` | A malformed `STRAT-COMBAT` line → parse failure, not silence, plus the resulting length-mismatch |
| `fail_divergence_present.log` | A `STRAT-COMBAT divergence` line → FAIL (blocking, `Error`-level fault per project posture) |
| `fail_blocking_ai_refusal.log` | A non-terminal `STRAT-AI refused` line → FAIL |
| `pass_terminal_refusal_ok.log` | The one `[T-SAVE-05]` terminal handover refusal, alone → does NOT fail the gate (positive control matching the phase-D carve-out) |
| `fail_missing_markers.log` | No `Test Started.`/`Test Completed.` markers at all → hard failure, never a silent whole-file scan |
| `fail_truncated_no_completed.log` | `Test Started.` with no matching `Test Completed.` → hard failure, not a fallback to EOF |
| `pass_commentary_echo_ignored.log` | A `LogAutomationController:`-echoed `STRAT-COMBAT` payload is ignored entirely; the real `LogStratBridge:` pairing still passes clean |
| `fail_pre_sliced_zero_events.log` | An empty file run with `--pre-sliced` (no `--test-path` — it is ignored in that mode) → the structural zero-event guard refuses it by default, `GUARD REFUSED`, FAIL; added `47ec9bf`, discharging the `--pre-sliced` zero-event gate debt |

`gate_self_test_output.txt` is every fixture's real, unedited CLI output plus its exit code —
**11 entries, one per checked-in `.log` file** (`grep -c "^== " gate_self_test_output.txt`).
`self_test_internal_fixtures_output.txt` is the script's own `--self-test` mode — **14 checks**
as of `47ec9bf` (`grep -c "^    record(" strat_combat_pairing_gate.py`, excluding the `def
record(...)` definition itself), run as Python assertions against temp files rather than
checked-in fixtures. **These are two different counts and are never totalled together** — the
fixture-file count (11) and the in-script self-test case count (14) have drifted apart before
and state.md names both independently for that reason. Both artifacts are included because the
checked-in `.log` files are what a future reader can open and rerun by hand, while `--self-test`
is what CI would invoke. `Tools/architect/strat_fixture_verdict_binding.py` (new, this pass)
binds the 11 checked-in fixtures — filename, expected verdict, exact invocation — to a
re-runnable assertion; it does not touch or duplicate the 14-case in-script self-test.

## The real run

`real-run/t-int-05-both-sides-ai-slice.log` — the checked-in slice itself, lines 4179..4474 of
the 2026-08-14 session log, bounded by its own `Test Started.`/`Test Completed.` markers (both
present as the file's first and last lines) rather than by any position in the parent log.
Named for what it is (one test's own log slice), not for where it sat.

`real-run/gate_output.txt` — the gate run twice: once against the full
`Saved/Logs/Stratocracy.log` (proving the gate's own internal slicing finds the same 4179..4474
bounds independently), once against the standalone checked-in slice file. Both report identical
figures — `68 applied kind=Attack` / `68 resolved` / `0 pairing mismatches` / `PASS` — confirming
the checked-in slice is a faithful, self-contained copy of the corpus the gate actually graded.

## What this gate does not and cannot prove

- **`ReplayLog` does not route through `Submit`** (phase 1's hole, carried forward): a combat
  log loaded from disk in a fresh process emits no `STRAT-COMBAT` line at all. This gate reads
  a live-session log; it says nothing about a replayed one, and a reader pointing it at a
  replay expecting the pairing to hold will see it fail here first.
- **The `!bSeeded` silent path fired zero times in this corpus** (measured: `combat_refused`
  count is 0, and `applied_attack_count == combat_resolved_count`). That is a measurement of
  this run, not a structural guarantee the path can never fire — the gate's length-mismatch
  check is what would catch it if it ever does, per `fail_silent_path_gap.log`.
- **Verdict status, kept current rather than restated as a snapshot claim:** gated THREE times.
  First two `VERDICT: BLOCK`, both on prose findings in `Tools/architect/state.md` (a
  fixture-count miscount, then a stale "no reviewer verdict yet" line that sat ~130 lines *above*
  the phase-3 entry recording the first `BLOCK`, and ~1,740 lines *below* the top-of-file record —
  neither on the gate, the fixtures, or this blackboard) — both fixed in place. Third
  `VERDICT: PASS`, zero findings — **phase 3 is CLOSED.** See `state.md`'s phase-3 entry for the
  full account of all three gates.

## Fixture-verdict binding (post-milestone, this pass, `strat-data-steward`'s own lane)

`Tools/architect/strat_fixture_verdict_binding.py` closes the debt this file's own earlier
staleness demonstrated: `gate_self_test_output.txt` was a hand-recorded table with no
re-runnable assertion tying each checked-in fixture to its expected verdict, and it went stale
(missing `fail_pre_sliced_zero_events.log`) the moment `47ec9bf` added that fixture without
updating the table. The new module declares, per fixture, both its expected verdict AND its
exact CLI invocation (ten fixtures run with `--test-path Stratocracy.Fixture.FIX-01.FixtureTest`;
`fail_pre_sliced_zero_events.log` runs with `--pre-sliced` and no `--test-path`, since
`--pre-sliced` ignores that flag and the file has no markers to slice on), re-runs the real gate
against the real file for each, and checks both the verdict AND the on-disk/declared set
symmetric difference (an undeclared 12th fixture or a declaration for a deleted one is caught
the same way a wrong verdict is).

**Falsifiability measured, not asserted.** Two deliberate breaks, each reverted after
observation:
1. Mis-declared `pass_clean_pairing.log` as `expect_pass=False` → `[**MISMATCH**]
   pass_clean_pairing.log ... expected FAIL, actual PASS, EXIT=0`, `AT LEAST ONE BINDING
   FAILED`, exit 1.
2. Dropped the `fail_missing_markers.log` declaration entirely (file left on disk) →
   `**UNDECLARED FIXTURES ON DISK (no binding covers these): ['fail_missing_markers.log']`,
   `AT LEAST ONE BINDING FAILED`, exit 1.
Both reverted; `python Tools/architect/strat_fixture_verdict_binding.py` returns to `ALL
BINDINGS HOLD`, exit 0, 11/11 fixtures `[OK]`.

**Scope, stated plainly.** This binds the 11 checked-in `.log` fixtures only. It does not bind,
duplicate, or re-derive the in-script `--self-test` case count (14, `check_self_test()`'s
`record()` calls) — the two counts are named independently everywhere in this file and in
`state.md` for exactly the reason state.md gives: they are different numbers that have drifted
apart before, and conflating them would hide the next drift rather than catch it.
