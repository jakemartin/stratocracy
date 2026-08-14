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

## The gate can fail — 10 checked-in fixtures, each proving one specific failure mode

Per the phase-D precedent (`evidence/07-ai-opponent/gate_self_test_output.txt`) and the
recorded lesson that the phase-D gate was itself blocked once for fixtures that could not
fail it, `fixtures/` holds ten small `.log` files, each run through the real CLI (not a
mocked function) with its own `Test Started.`/`Test Completed.` markers. Split derived from
the directory, not counted by eye:

```
$ ls fixtures/ | grep -c '^fail_'
7
$ ls fixtures/ | grep -c '^pass_'
3
```

— seven FAIL fixtures, three PASS controls, matching the seven `EXIT=1` / three `EXIT=0` blocks
in `gate_self_test_output.txt` below. (This table itself was already correct; the miscount the
reviewer found — "eight FAIL fixtures plus two PASS controls," naming a checked-in
"unmatched resolved line" fixture that does not exist — was in `state.md` only. That scenario is
case 4 of the in-script `check_self_test()` below, over an inline `tempfile` corpus, and is not
one of these ten checked-in files; the two sets are kept distinct and are never totalled
together.)

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

`gate_self_test_output.txt` is every fixture's real, unedited CLI output plus its exit code.
`self_test_internal_fixtures_output.txt` is the script's own `--self-test` mode (the same
eleven checks, run as Python assertions against temp files rather than checked-in fixtures) —
both are included because the checked-in `.log` files are the artifact a future reader can
open and rerun by hand, while `--self-test` is what CI would invoke.

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
