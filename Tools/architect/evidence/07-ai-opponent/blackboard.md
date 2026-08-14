# Phase D evidence — AI-opponent milestone, piece (a)

Assembled 2026-08-13 by `strat-data-steward`, **updated 2026-08-13** in the same day once
`strat-test-author` landed the owed clause (suite 87/87), once the gate was run against a real
AI-vs-AI log for the first time, and once the reviewer's narrow re-gate on this piece returned
**`VERDICT: PASS`** after an initial `BLOCK` on a prose finding was fixed (see "Reviewer gate on
this piece" near the bottom) — and **updated again, later still the same day**, once the user
drove the PIE playtest and this steward assembled `pie-session/` and `pie-screenshots/` from the
two resulting sessions (see "Update — 2026-08-13, later still the same day" below). **Phase D is now
CLOSED** — the PIE piece's own narrow re-gate has since run, an initial `BLOCK` on three miscounted
claims followed by a fix and `VERDICT: PASS` on the fourth gate; see "Fourth gate — PIE half —
BLOCK, fix, PASS" and "Phase D — CLOSED" near the bottom.

## Contents of this directory

| File | What |
|---|---|
| `../strat_ai_log_gate.py` | The parser and gate itself (lives one level up, in `Tools/architect/`, this steward's lane — see "Files changed" in the dispatch report). |
| `fixtures/pass_terminal_result.log` | Synthetic, hand-written log fixture: a short well-formed AI turn (Build, Move, EndTurn, turn-ended) followed by an `applied Attack` and the one refusal shape that must PASS — the terminal `phase=apply kind=EndTurn reason=[T-SAVE-05] no match is running` line that means the match already finished correctly. |
| `fixtures/fail_mixed_faults.log` | Synthetic fixture exercising two failure modes at once: a non-terminal refusal (`phase=apply kind=Move reason=[T-MOVE-02] ...`) and a single corrupted field (`unti=` instead of `unit=`) on an otherwise well-formed `applied` line. |
| `fixtures/fail_format_drift.log` | Synthetic fixture where every `applied` line has been renamed `hex=` → `pos=` (simulating an upstream rename that drifted the format string) — all three lines are unparseable, proving a wholesale format drift is reported as N parse failures, not read as "zero faults found." |
| `fixtures/empty.log` | A log with no `STRAT-AI` lines at all (only a `STRAT-CMD accepted` line, the human-command family), to exercise the "no AI activity in this log" path and the `--allow-empty` escape hatch. |
| `fixtures/pass_harness_echo_ignored.log` | **New.** Two genuine `LogStratPlay:` lines plus one verbatim `LogAutomationController: Suppressed expected ('Warning') level log message ... matching 'STRAT-AI refused' 1 times.` line copied from the real isolated-run log — pins the harness-echo defect below so it cannot regress silently. |
| `gate_self_test_output.txt` | Captured stdout and exit code of running `strat_ai_log_gate.py` against all five fixtures above, plus the empty fixture run twice (with and without `--allow-empty`). Seven runs, seven exit codes, all reproduced by direct execution in this session — not narrated. Regenerated after the harness-echo fix; `fail_format_drift.log` still reports 3/3, confirming the fix did not weaken the drift check. |
| `suite-87/index.json`, `suite-87/suite-run.log` | The full 87/0/0 automation report and its console log, folded in from where the dispatching agent placed them at the top of this directory. See "Suite-log corpus" below for why this is NOT what the gate is run against. |
| `real-game/isolated-run.log` | Copy of `Saved/Logs/Stratocracy.log`, captured while it held exactly one isolated automation run: `Stratocracy.StratPlay.T-INT-05.BothSidesAiReachesAResultWithinTheBound`, `-ReportExportPath=...\Saved\AutomationReportIsolated`, so the 87/87 report artifacts were not clobbered. Confirmed by grepping the log itself: `Cmd: Automation RunTests Stratocracy.StratPlay.T-INT-05.BothSidesAiReachesAResultWithinTheBound;Quit`, `Test Completed. Result={Success}`. |
| `real-game/index.json` | Copy of `Saved/AutomationReportIsolated/index.json` — the 1/1 report for that isolated run. |
| `real-game/gate_output.txt` | The gate (post-fix) run directly against `real-game/isolated-run.log`: **PASS, exit 0.** |
| `pie-session/session1-completed-game.log`, `pie-session/gate_output.txt`, `pie-session/narrative.md` | Session 1 of the user-driven PIE playtest — the completed game, sliced from `Saved/Logs/Stratocracy.log` bounded by its own open/shutdown markers (full-log lines 3249-3384) — its gate run and the full measurement account (pacing, terminal refusal, Tank absence, channel disjointness). See "Update — 2026-08-13, later still the same day" and "Re-cut — 2026-08-13" below. |
| `pie-screenshots/session2-screenshot-run.log`, `pie-screenshots/gate_output.txt`, `pie-screenshots/narrative.md`, `pie-screenshots/ScreenShot00042.png`, `pie-screenshots/ScreenShot00043.png` | Session 2 of the same PIE playtest — a separate, shorter run, **not** the completed game — sliced from `Saved/Logs/Stratocracy.log` bounded by its own open/shutdown markers (full-log lines 3443-3480) — its gate run, the two screenshots taken inside this session's window, and the cross-check between the Output Log visible in `ScreenShot00043.png` and this slice's own lines. |

## Why fixtures instead of a real log

**SUPERSEDED by the "Update" section below, dated the same day.** At the time this section was
written, the brief was explicit that this dispatch must not touch the editor or run a build, and
that was true then. It stopped being true later the same day, once the owed clause landed and the
gate was run against `real-game/isolated-run.log` (see "Update" and "Run 1"/"Run 2" below) — a
real `STRAT-AI` log now exists and has been gated. Left in place, unedited otherwise, so a reader
can see what was known at each stage; do not cite this section alone as current.

The brief was explicit that this dispatch must not touch the editor or run a build. There is
therefore no real `STRAT-AI` log to gate yet — the AI opponent has never actually played a turn
under this steward's observation. The fixtures above are **hand-authored strings matching the
three format strings read directly from `Source/StratPlay/StratAiTurnRunner.cpp`** (lines 78-89,
208-210, 318-336 — see the dispatch report for exact reproduction), not samples pulled from a
run. They exist solely to prove the parser and gate behave as specified before any real evidence
depends on them. When a later piece of phase D produces a genuine AI-vs-AI PIE or headless log,
it belongs in this directory alongside these fixtures, and the gate should be re-run against it
directly (`python Tools/architect/strat_ai_log_gate.py <path-to-real-log>`).

## What is proven, and how

- **All three `STRAT-AI` shapes parse into structured fields**, not a boolean match. Verified by
  running the gate against `pass_terminal_result.log` and reading `gate_self_test_output.txt`:
  4 `applied` lines, 1 `turn-ended` line, 1 terminal refusal, all counted correctly, exit 0.
- **A non-terminal refusal fails the gate.** `fail_mixed_faults.log`'s `phase=apply kind=Move
  reason=[T-MOVE-02] ...` line is reported by name (`FAIL line 2: non-terminal STRAT-AI
  refused ...`), exit 1.
- **A single malformed field fails the gate as a parse failure, not a skip.** The same fixture's
  `unti=-1` typo (line 3) is reported (`FAIL line 3: unparseable STRAT-AI line: ...`) rather than
  silently dropped — both faults present in one exit-1 run, neither masking the other.
- **A wholesale format drift reads as N failures, never as a clean pass.** `fail_format_drift.log`
  renames `hex=` to `pos=` on all three lines; the gate reports **3** parse failures and exits 1 —
  this is the caveat the brief named explicitly ("a format drift that makes every line
  unparseable must not read as zero faults found"), and it is demonstrated here, not asserted.
- **The terminal `T-SAVE-05` handover refusal is the one refusal shape that passes**, and nothing
  else does. `is_terminal_handover_refusal()` requires `phase=apply`, `kind=EndTurn`, and a
  `reason` that both starts with `[T-SAVE-05]` and contains `no match is running` — all four
  conditions together, not the phrase alone, so an unrelated refusal ending in similar words is
  never waved through. Demonstrated by contrast: `pass_terminal_result.log`'s refusal (all four
  conditions met) passes; `fail_mixed_faults.log`'s refusal (`phase=apply`, but `kind=Move`, not
  `EndTurn`) does not.
- **An AI-less log fails by default and passes only when told to.** `empty.log` (a `STRAT-CMD`
  line only, no `STRAT-AI` lines) exits 1 without `--allow-empty` and exits 0 with it — both runs
  captured in `gate_self_test_output.txt`.

## Limits — read this before citing anything above as proof the AI opponent works

- **Nothing here proves `strat::nextCommand` was ever called.** The fixtures are hand-written
  strings that happen to match the format the runner emits; they are not derived from a running
  process. This directory proves the *parser and gate*, not the *AI*.
- **CORRECTED below, per the reviewer's initial `BLOCK` on this piece's prose (see "Reviewer gate
  on this piece" at the bottom for the BLOCK → fix → re-gate `PASS` sequence). Convention note:**
  superseded passages in this directory are quoted inline and marked CORRECTED/SUPERSEDED rather
  than struck through — there is no Markdown strikethrough (`~~`) anywhere in this file; a reader
  should not expect one. The original
  text here said the tripwire "legitimately expects zero refusals in its own run" and that the
  real AI-vs-AI log is "a situation that tripwire's fixture does not produce." Both are wrong.
  `Occurrences = 0` in UE 5.8's `AddExpectedMessagePlain` means "must be seen ONE OR MORE times,"
  not "expect zero" (`AutomationTest.h:1817-1819`; the clause's own comment says this correctly at
  `StratAiMatchClauses.cpp:607-609`). The tripwire is at `:610`, inside
  `Stratocracy.StratPlay.T-INT-05.BothSidesAiReachesAResultWithinTheBound` (`:581`), which drives
  the real production subsystem and asserts `Model.Match.bHasResult` true (`:722-723`) — it is not
  a scripted port that avoids a real §2.8 result. And `real-game/isolated-run.log` **is** an
  isolated run of that very clause (see "Run 2" below); the gate's own output against it
  (`real-game/gate_output.txt`) reads `terminal refusals (ok): 1` — the exact situation this
  paragraph originally said did not occur. **Precisely stated** (`:610`'s own comment at
  `:934-938` is the more careful sibling text this borrows from): the tripwire requires one or more
  `Warning`-or-higher messages containing the plain substring `STRAT-AI refused`, not specifically
  the terminal `[T-SAVE-05]` refusal. In this clause's run that substring is satisfied by exactly
  one line — the real terminal refusal, confirmed by one production `STRAT-AI refused` line in
  `isolated-run.log` and by the harness's own echo (line 3290) reporting the match count as `1
  times` — so the operative claim holds today. But if `T-SAVE-05` were fixed *and* some unrelated
  refusal appeared in the same run, `:610` would stay green while this gate's `terminal refusals
  (ok)` count dropped to 0 — the "designed to go red together" guarantee below does not cover that
  one case. Subject to that caveat: a future engineer who fixes `T-SAVE-05` with no other refusal
  present should expect `StratAiMatchClauses.cpp:610` to fail and this gate's `terminal refusals
  (ok)` count to drop to 0, not for either to stay green. See `strat_ai_log_gate.py`'s comment above
  `is_terminal_handover_refusal` for the full statement; this directory does not itself run the C++
  test, and could not without a build.
- **`RunAiTurnsNow`'s return value is not used anywhere in this gate.** Per the brief's hard
  constraint and `state.md`'s "deferred finding," that return value is `false` for a correctly
  finished game and is not a fit basis for pass/fail. The gate is built entirely on parsed log
  fields instead.
- **SUPERSEDED by the "Update" section below.** At original write time: "The parser has not yet
  been exercised against `T-INT-05.HandoverRefusalCarriesTheFixedFields` — that clause does not
  exist yet; this dispatch is what makes it possible to write, not the writing of it." That clause
  now exists (`StratAiMatchClauses.cpp:922`) and passes as part of the 87/87 suite — see "Update"
  below. Left unedited above for the record of what was known at dispatch time.

## Update — 2026-08-13, later the same day: proven against a real log for the first time

`Stratocracy.StratPlay.T-INT-05.HandoverRefusalCarriesTheFixedFields` now exists
(`StratAiMatchClauses.cpp:922`), asserting against this parser's ten-field contract. Full suite:
**87 succeeded / 0 failed / 0 notRun, 87 entries** — read directly from
`suite-87/index.json` (`reportCreatedOn: "2026.08.13-21.38.06"`), not narrated. Macro
set-difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST` in `suite-87/suite-run.log` against the prior
86-test baseline: delta +1, agreeing exactly with the report's own count.

**A correction carried in, not generated here:** `phase=handover` is not one of
`FStratAiTurnRunner`'s fault arms and is not reachable through `IStratAiTurnPort`. Read directly
at `StratMatchSubsystem.cpp:706-724`: it is `UStratMatchSubsystem::RunAiTurnsNow`'s own outer
bound (`StopReason.IsEmpty() && TurnsRun >= MaxTurns`), emitted via `StratLogAiTurnRefusal`
*by that function*, after a run of turns that all SUCCEEDED — the source comment at `:712-713`
states this outright ("`FStratAiTurnRunner` plays ONE turn and cannot see that this is the ninth
in a row"). Checked against this file: nothing above claims `phase=handover` is a runner fault arm
or attributes it to `IStratAiTurnPort`, so no correction to this document was needed — the parser
and gate treat `phase=handover` the same as any other `phase` value (a plain field on
`StratAiRefused`), which is correct regardless of which function emits it.

### Run 1 — the full suite log is the WRONG corpus for this gate

Gating `Saved/Logs/` full-suite output (249 `STRAT-AI` lines at the time) produced exit 1: 5 parse
failures and 6 blocking refusals. **All 11 are correct behaviour being misreported, not
regressions.** The suite deliberately manufactures refusals as test fixtures — scripted-port
refusals, a `MaxCommandsPerTurn is 0` clause, a `definitions are not loaded` clause, and the two
new handover-adjacent lines the new clause exercises. **Finding, stated plainly so nobody points
this gate at a suite log and reads 11 failures as a regression: a synchronous automation-suite log
is not this gate's corpus.** The gate is built for one thing — an isolated, real AI-vs-AI match —
and a suite log's fixtures will always trip it by design.

### Run 2 — an isolated real AI-vs-AI game, gated clean after the fix

`real-game/isolated-run.log` is `Saved/Logs/Stratocracy.log` captured while it held exactly one
isolated run of `Stratocracy.StratPlay.T-INT-05.BothSidesAiReachesAResultWithinTheBound`
(`-ReportExportPath=...\Saved\AutomationReportIsolated`, so the 87/87 artifacts were not
clobbered — confirmed 1/1 in `real-game/index.json`). Gate output, pre-fix:

```
applied lines:            156
turn-ended lines:         11
terminal refusals (ok):   1
blocking refusals:        0
parse failures:           1
  FAIL line 3290: unparseable STRAT-AI line: "STRAT-AI refused' 1 times."
```

**156 commands across 11 turns is exactly what phase A measured for a self-play game** —
independent corroboration the gate is reading a real, complete, deterministic match, and that its
terminal-refusal branch fires correctly on a real finish (one `phase=apply kind=EndTurn
reason=[T-SAVE-05] no match is running` line, correctly counted as `terminal refusals (ok)`, not
a blocking refusal).

### THE DEFECT — found and fixed here

Line 3290 was never a production line:

```
[2026.08.13-21.41.20:766][599]LogAutomationController: Suppressed expected ('Warning') level log message or higher matching 'STRAT-AI refused' 1 times.
```

versus a genuine line two entries earlier:

```
[2026.08.13-21.41.20:735][598]LogStratPlay: STRAT-AI turn-ended side=0 turn=1 commands=8 hash=02cd799951334a5f
```

`find_strat_ai_payload` matched the bare substring `STRAT-AI` anywhere in the line, so the
automation harness's own commentary *about* a suppressed expected-error match was parsed as if it
were the runner's own output. **Fix:** the function now anchors on the `LogStratPlay:` category —
the only category `StratAiTurnRunner.cpp` ever logs through — and only searches for the
`STRAT-AI ` payload *after* that marker in the same line. This is an allowlist of one known-good
category, not a blocklist of `LogAutomationController` specifically, so it stays correct against
any other category that happens to mention the substring `STRAT-AI` in prose. Confirmed the fix
does not weaken the drift check: `fail_format_drift.log` (three genuine, malformed `LogStratPlay:`
lines) still reports **3 parse failures**, unchanged — a real production line that drifts format
is still caught; only lines lacking the `LogStratPlay:` category at all are now skipped.
**Pinned with a new fixture,** `fixtures/pass_harness_echo_ignored.log` (two genuine lines plus
the exact harness-echo string copied from the real log), so this is fixed once and stays fixed,
not fixed and re-breakable.

Re-run against `real-game/isolated-run.log` post-fix: **exit 0, clean** (`real-game/gate_output.txt`):

```
applied lines:            156
turn-ended lines:         11
terminal refusals (ok):   1
blocking refusals:        0
parse failures:           0
PASS
```

**Lesson worth generalising, and worth writing down because the self-test could not have caught
this on its own:** every fixture built for this gate before today was hand-authored *in
production shape* — realistic `LogStratPlay:`-prefixed lines only. A parser tested exclusively
against inputs shaped like its happy path cannot discover a defect that only a genuinely
adversarial, differently-shaped input (here: the test harness's own log commentary, which shares
a substring with the target but not its category) will trigger. The harness-echo fixture is the
first fixture in this directory that was not authored by imagining a well-formed or
deliberately-corrupted `STRAT-AI` line, but by taking an actual byte-for-byte line from a real
run that happened to be adjacent, camouflaged noise.

## Update — 2026-08-13, later still the same day: the PIE half assembled, exit criterion MET

The user drove the PIE playtest. **Corrected below (2026-08-13, later still) — the "3481 vs 3480"
figure this paragraph originally recorded was never a count error; `Saved/Logs/Stratocracy.log` is
a live log a running editor keeps appending to, so its line count is not a fixed fact about the
file — it was 3480 when the dispatch brief counted it, 3481 moments later when this steward first
counted it, and 3518 when re-measured for the re-cut below, mtime still advancing.** Both the
brief's 3480 and this steward's original 3481 were correct *at the instant each was taken*; neither
was a slicing error, and there is no discrepancy to explain beyond "the file is not static." Holds
**two separate PIE windows**, kept distinct throughout:

- **Session 1 — the completed game.** `pie-session/session1-completed-game.log`, bounded by its own
  open (`LogWorld: Bringing World ...`, full-log line 3249) and shutdown
  (`Destroying online subsystem :Context_1`, full-log line 3384) markers — see "Re-cut" below.
  Gated directly: `applied 74 / turn-ended 4 / terminal refusals (ok) 1 / blocking 0 / parse
  failures 0`, exit 0, `PASS`. Reproduces the dispatching agent's reported numbers exactly.
- **Session 2 — the screenshot run, a separate short session.**
  `pie-screenshots/session2-screenshot-run.log`, bounded by its own open (full-log line 3443) and
  shutdown (`Destroying online subsystem :Context_2`, full-log line 3480) markers — see "Re-cut"
  below. Gated directly: `applied 8 / turn-ended 1 / terminal 0 / blocking 0 / parse failures 0`,
  exit 0, `PASS`. Also reproduces exactly.

## Re-cut — 2026-08-13, later still: both slices bounded by content markers, not EOF or a neighbour's start

Session 2's original cut, "3443 to EOF," was taken against a log still being appended to by a
running editor — that made the cut's tail arbitrary (its last lines were `[UsageMonitor]` telemetry
from *after* PIE shutdown, present only because they happened to be at EOF at cut time) and made the
filename `session2-lines-3443-3481.log` assert a range already false by the time anyone read it (the
log was 3518 lines minutes later). Re-cut to end at this session's own
`LogPlayLevel: Display: Destroying online subsystem :Context_2` line (3480) instead of EOF, and
renamed `session2-screenshot-run.log` — a name describing what the file *is*, not where it sat in a
rotating log at cut time.

Checking whether session 1 needed the same treatment turned up a different problem with the same
root cause: `session1-lines-3249-3442.log`'s range was stable (bounded by the *next* PIE window's
opening line, not by EOF), but "stable" was not the same as "correct" — those 194 lines included 58
lines that were not session 1's output at all: post-teardown noise plus the *next* session's own
editor-side bring-up, both riding along because the cut used the neighbour's start rather than this
session's own end. Re-cut to end at session 1's own `Destroying online subsystem :Context_1` line
(3384) and renamed `session1-completed-game.log`, for consistency and for the same reason.

**Both re-cuts reproduce the pre-existing gate counts exactly** — 74/4/1/0/0 and 8/1/0/0/0, both
exit 0 `PASS` — confirming the dropped tails carried no `STRAT-AI`/`STRAT-CMD` content in either
case; see each `narrative.md`'s "Re-cut" section and `gate_output.txt` for the re-run output.

**The generalisable lesson, for any future PIE or headless capture, not just this one: slicing a
log against EOF produces a non-reproducible artifact whenever the source is still being written.**
"To EOF" means "to whenever the cut happened to run," not a fixed boundary — bound evidence slices
by markers in the content (a session's own open/shutdown line, a turn boundary, a result line),
never by the end of a file a running process may still append to. A line-number range into a
rotating log is itself a fragile name even when it happens to be stable at cut time, because a
neighbouring session's content can ride along under a boundary chosen for a different reason (here:
the next session's start, rather than this session's own end) — name evidence files for what they
*are*, not for where they sat in the file when cut.

Full measurement detail — the five-turn pacing table (0.507/0.500/0.501/0.499/0.500 s against
`AiTurnDelaySeconds = 0.500000`), the terminal refusal at full-log line 3371, the 12-of-12 `def=0`
Tank-absence count, command-channel disjointness, and the screenshot cross-check — is written out
in `pie-session/narrative.md` and `pie-screenshots/narrative.md`, and summarised in `state.md`'s
phase D section rather than repeated a third time here.

**`ScreenShot00042.png` and `ScreenShot00043.png` both fall inside session 2's window, not
session 1's.** They show a short, separate run — `TURN 1/20` → `TURN 2/20` — not the completed
five-turn game measured in `pie-session/`. This is stated explicitly in
`pie-screenshots/narrative.md`'s title and first paragraph so it cannot be conflated by a later
reader skimming only the images.

**Phase D's exit criterion — PIE playtest done, machine-repeatable AI-vs-AI gate built and proven,
evidence assembled — is MET.** At the time this paragraph was first written, the narrow re-gate on
this PIE piece (this update) had not yet run; it has since run (initial `BLOCK`, fix, `VERDICT: PASS`
on the fourth gate) and phase D is CLOSED — see "Fourth gate — PIE half — BLOCK, fix, PASS" and
"Phase D — CLOSED" near the bottom of this file for the sequence.

## What was open at the time of the update above — closed by the fourth gate

- **The narrow re-gate on this PIE piece has since run and returned `VERDICT: PASS`** (after an
  initial `BLOCK` on three miscounted claims — see "Fourth gate — PIE half" below). Piece (a)'s gate
  half already carried `VERDICT: PASS` (see "Reviewer gate on this piece" below); that verdict did
  not and could not cover the PIE evidence assembled in the update immediately above it, which
  postdated it — hence the separate, later gate on this piece, now also `PASS`.
- The suite-log-is-the-wrong-corpus caveat above should be read by anyone tempted to point this
  gate at `Saved/Logs/` suite output expecting it to summarise test health — it does not, and is
  not meant to. This caveat is now also stated in `strat_ai_log_gate.py --help`, not just here, so
  someone driving PIE directly from the CLI sees it without first finding this directory.
- **CLOSED by the update above — kept here as the record of what was open before it closed:** this
  gate had only ever been proven against a headless automation-test log
  (`real-game/isolated-run.log`); a PIE session log is a different corpus shape, and the
  `LogStratPlay:` category anchor `find_strat_ai_payload` keys on had not been demonstrated against
  PIE output. It now has, twice (both `pie-session/` and `pie-screenshots/` slices gate at 0 parse
  failures), so this bullet documents a limit that no longer applies rather than one that does.

## Reviewer gate on this piece — BLOCK, fix, PASS

`strat-integration-reviewer` gated this evidence directory's prose (not code correctness — the
parser/gate behaviour was already proven above by direct execution). Sequence, recorded honestly:

1. **First gate: `VERDICT: BLOCK`**, on a prose finding in the "Limits" section above — the
   original text asserted the tripwire "legitimately expects zero refusals" and that the real
   AI-vs-AI log was "a situation that tripwire's fixture does not produce," both wrong readings of
   UE 5.8's `Occurrences = 0` semantics (see the CORRECTED passage above).
2. **Fix:** the wrong passage was corrected in place (superseded text kept, quoted inline and
   marked CORRECTED, not deleted, per this directory's convention).
3. **Narrow re-gate: `VERDICT: PASS`, zero gating findings.** Scope confirmed by file mtime, not by
   diffing against an intermediate commit — none exists between the `BLOCK` and this re-gate, so
   mtime is the only signal available that the re-gate covers exactly the files touched by the fix
   and nothing beyond it.

Five further non-gating observations came out of that same pass (prose staleness in this file and
in `state.md`, and two precision gaps — one in this file, one in `strat_ai_log_gate.py` — about
exactly what the `:610` tripwire requires). All five are fixed in this same steward pass; see the
CORRECTED/SUPERSEDED passages above and the `state.md` phase D section.

**Pattern worth carrying past this milestone:** three documents in this phase were caught stating
something checkable that was, in fact, false or imprecise — two were technical (the `Occurrences`
semantics, the T-SAVE-05 exclusivity claim) and one was pure bookkeeping (this file and `state.md`
both claiming "no verdict exists yet" after one already existed 100+ lines earlier in the same
file). The technical ones were caught by the gate reading the code; the bookkeeping one was a
document going stale about its own history mid-edit. As phase D produces more updates, "does this
document still describe itself correctly" is now as much a check as "is the code right" — see
`state.md`'s NEXT for the durable version of this note.

## Fourth gate — PIE half — BLOCK, fix, PASS

`strat-integration-reviewer` gated the PIE-half prose assembled in "Update — 2026-08-13, later
still the same day" above (the gate-half prose above it had already reached `PASS` on its own,
earlier, third gate). Sequence, recorded honestly:

1. **First gate on the PIE half (the milestone's third gate): `VERDICT: BLOCK`, three findings**,
   all miscounted claims against the checked-in log slices: a turn-3 span/count in
   `pie-session/narrative.md`, a turn-5 Build/Move/Attack breakdown in the same file, and a
   session-2 Move count in `pie-screenshots/narrative.md` sourced from "the brief" rather than from
   the log printed three lines above the claim. Each was falsified by a `grep -c` against the
   artifact sitting in the same directory. This is the fourth occasion, in this milestone, of the
   class recorded under `state.md`'s NEXT ("a claim in a document is checkable against an artifact,
   and the artifact wins") — three miscounts here plus one propagation into `state.md`, six
   corrections across four occasions in total.
2. **Fix:** each miscounted claim was replaced by a derived count, re-run and confirmed against the
   checked-in slice, in `pie-session/narrative.md` and `pie-screenshots/narrative.md`.
3. **Fourth gate: `VERDICT: PASS`, zero gating findings.**

Five further non-gating observations came out of that fourth gate — all fixed in this same
phase-closing steward pass: the tally-noun imprecision under `state.md`'s NEXT ("four separate
corrections" → "six corrections across four occasions"); three now-stale "has not run" sites in
`state.md` (`:3`, the piece-(a)-era bullet immediately above "Piece (b)", and the sentence opening
"Piece (b)" itself) rewritten to state the actual `BLOCK`→fix→`PASS` sequence on each half; the
piece-(a)-era "the PIE playtest itself has not been run" bullet, left in place and marked dated and
superseded rather than deleted, since it was true of the moment it describes and its immediate
neighbour (piece (b)) already supersedes it; confirmation the NEXT-block merge lost nothing (no
action needed); and two derivation commands in the narrative files that returned the right number
for the wrong reason — `pie-session/narrative.md`'s `grep -n "turn=3 side=1" ... | wc -l` (matches
only because `turn-ended` lines emit `side=1 turn=3`, reversed field order, so it was really
counting 22 `applied` lines under a pattern that does not name them) and
`pie-screenshots/narrative.md`'s unqualified `grep -c "kind=Move"` (correct only because the human
issued no `Move` in that 38-line slice) — both now state their intent with a channel- and
kind-qualified `grep -c`, confirmed to return the same numbers.

## Phase D — CLOSED

**Completed 2026-08-13.** Both halves of this phase now carry a `PASS` reached after an initial
`BLOCK` and a fix: the gate half (piece (a)) at the third gate overall (see "Reviewer gate on this
piece" above), the PIE half (piece (b)) at the fourth gate overall (immediately above). Phase D's
exit criterion — PIE playtest run, plus a machine-repeatable AI-vs-AI gate built and proven, evidence
assembled under this directory — is MET. The AI-opponent milestone (phases A–D) is now COMPLETE. See
`state.md`'s "Phase D — CLOSED" entry for the full measured record carried forward to any future
phase.

Both gates re-run directly at the close of this pass, unchanged by its prose-only edits: session 1 →
`74 applied / 4 turn-ended / 1 terminal / 0 blocking / 0 parse failures`, exit 0; session 2 →
`8 applied / 1 turn-ended / 0 terminal / 0 blocking / 0 parse failures`, exit 0.
