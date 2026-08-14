# PIE session 1 — the completed game

Slice of `Saved/Logs/Stratocracy.log`, bounded by content markers rather than a line-number range
into a log that is still being appended to by a running editor (see "Re-cut, 2026-08-13" below):
opens at `LogWorld: Bringing World /Game/StratMaps/UEDPIE_0_Lvl_FerrumCrossing.Lvl_FerrumCrossing
up for play` (full-log line 3249) and closes at this session's own `LogPlayLevel: Display:
Destroying online subsystem :Context_1` (full-log line 3384) — 136 lines, file
`session1-completed-game.log`. This is the first of two PIE windows in that log, timestamped
`2026.08.14-00.29.41` at open and `2026.08.14-00.30.59` at teardown (`BeginTearingDown` at line
3374 of the full log). **This is the completed game** — five human turns, five AI turns, ending in
a §2.8 result. No screenshot was taken during this session; the two PNGs in `../pie-screenshots/`
belong to the second, separate session (see that directory).

## Re-cut, 2026-08-13: bounded by this session's shutdown marker, not the next session's start

Originally cut "3249 to 3442" — the line immediately before the next PIE window's own opening line.
That range was stable (bounded by content, not by EOF of a growing file) but it was still wrong:
lines 3385–3442 are not this session's output at all — 58 lines of post-teardown noise
(`LogNeoStackAI [UsageMonitor]`, DDC maintenance, EOS SDK chatter) followed by the *next* PIE
session's own editor-side bring-up (world duplication, audio device init) up to the line before its
`LogWorld: Bringing World ... up for play`. None of it is `STRAT-AI`/`STRAT-CMD` content, so the
gate counts below are unaffected, but the slice was carrying another session's startup preamble
under this session's name. Re-cut to end at this session's own `Destroying online subsystem
:Context_1` line (3384) instead. Renamed from `session1-lines-3249-3442.log` to
`session1-completed-game.log` — a name that states what the slice *is*, not where it sat in a
rotating log at the moment of cutting; see `../blackboard.md` for the general lesson.

## Gate result

`python Tools/architect/strat_ai_log_gate.py pie-session/session1-completed-game.log`, run
directly, exit 0:

```
applied lines:            74
turn-ended lines:         4
terminal refusals (ok):   1
blocking refusals:        0
parse failures:           0
PASS
```

Reproduces the dispatching agent's reported numbers exactly (74/4/1/0/0, exit 0 PASS), unchanged
from the pre-re-cut result — dropping the 58 lines of post-teardown/next-session noise moved zero
counts, confirming that content was correctly non-`STRAT-AI` noise. Full output captured in
`gate_output.txt`.

## Measurements taken directly from this slice

**1. The paced path, discharged with a number.** Delay from each human `STRAT-CMD accepted
kind=EndTurn` to the next `STRAT-AI applied` line, all five turns, read from this slice's own
timestamps:

| Turn | Human EndTurn accepted | Next AI applied | Delay |
|---|---|---|---|
| 1 | `00.29.58:286` (line 3271) | `00.29.58:793` (line 3272) | 0.507 s |
| 2 | `00.30.20:600` (line 3289) | `00.30.21:100` (line 3290) | 0.500 s |
| 3 | `00.30.38:485` (line 3308) | `00.30.38:986` (line 3309) | 0.501 s |
| 4 | `00.30.48:264` (line 3335) | `00.30.48:763` (line 3336) | 0.499 s |
| 5 | `00.30.52:669` (line 3360) | `00.30.53:169` (line 3361) | 0.500 s |

Against `AiTurnDelaySeconds = 0.500000`. All five values read directly off this slice's log
timestamps (`grep -n` against the checked-in slice), not taken from the dispatching agent's
numbers and then trusted — they agree to the millisecond. `AiTurnDelaySeconds` had no synchronous
test coverage before this — it needs a ticking world — so this PIE session is the only thing in
the project that exercises it.

**2. Pacing is per-turn, not per-command — confirmed independently.** Within any one AI turn, every
`STRAT-AI applied` line (and the closing `turn-ended`) shares the *same millisecond timestamp* as
its siblings — e.g. turn 3's 22 applied lines (full-log 3309–3330, slice lines 61–82) all read
`00.30.38:986`, and the `turn-ended` line closing that turn (slice line 83) reads `commands=22`,
matching (`grep -c "STRAT-AI applied.*turn=3 side=1" session1-completed-game.log` = 22, run and
confirmed here). Inter-command gaps are 0.000 s. This is phase B's design decision ("pacing is
before the turn, not between commands") observed at runtime, not re-argued from the source.

(An earlier version of this derivation read `grep -n "turn=3 side=1" ... | wc -l`, which also
returns 22 — but only because `turn-ended` lines emit their fields as `side=1 turn=3`, field order
reversed from `applied`/`refused`, and so never match that pattern; the 22 it counted were really
22 `applied` lines, correct by construction against this checked-in slice but not by what the
command said. Restated above to name the channel and kind explicitly.)

**3. `AiSides=(1)` confirmed at runtime.** Every `STRAT-AI applied`/`refused`/`turn-ended` line in
this slice carries `side=1`; every `STRAT-CMD accepted` line carries `side=0`. The AI played side 1
throughout, and only side 1, across all five turns.

**4. The terminal-refusal case, observed in the wild for the first time.** Line 3371 of the full
log (line 123 of this slice):

```
STRAT-AI refused phase=apply kind=EndTurn unit=-1 hex=0,0 def=-1 target=-1 turn=5 side=1 reason=[T-SAVE-05] no match is running
```

Turn 5's AI applied 3 Builds, 3 Moves, and 4 Attacks (10 commands, counted with `grep "turn=5
side=1" session1-completed-game.log | grep -c "kind=<X>"` for each kind) before this refusal; the human's
units were destroyed during that turn, the match reached a §2.8 result mid-AI-turn, and the rules
module correctly refused the winner's own closing `EndTurn`. The gate classifies this exact shape
— `phase=apply`, `kind=EndTurn`, `reason` starting `[T-SAVE-05]` and containing `no match is
running` — as a **terminal refusal**, not a blocking one, and counts it `1`, matching. Previously
this shape existed only in a hand-authored fixture
(`../fixtures/pass_terminal_result.log`) and one headless isolated run
(`../real-game/isolated-run.log`); this is the first time it has been observed from a live PIE
session.

**5. Phase C's Tank prediction, confirmed on this slice.** All 11 `STRAT-AI applied kind=Build`
lines in this slice carry `def=0`:

```
$ grep "STRAT-AI applied kind=Build" session1-completed-game.log | grep -c "def=0"
11
```

`Data/units.csv` row order is `Infantry, Tank, Artillery, Recon` (read-only, vendored — not
re-verified here beyond citing phase 0's proven row order), so `def=0` is Infantry; Tank (`def=1`)
never appears despite being a quarter of the authored buildlist. See item 5 of the milestone-level
account below for the combined count across both sessions.

**6. The gate handles PIE corpus shape — the open limit from the gate-half evidence is now
closed.** `gate_output.txt` above shows **0 parse failures** against this PIE slice. The
`LogStratPlay:` anchor `find_strat_ai_payload` keys on was previously proven only against a
headless automation-test log (`../real-game/isolated-run.log`); it now parses genuine PIE output
correctly. `../blackboard.md`'s "What remains open" section is updated to record this closed.

**7. Command-channel disjointness held in a real session.** This slice carries **23**
`STRAT-CMD accepted` lines (12 `Move`, 6 `Attack`, 5 `EndTurn` — the human's side) and **74**
`STRAT-AI applied` lines (11 `Build`, 37 `Move`, 22 `Attack`, 4 `EndTurn` — the AI's side), counted
directly:

```
$ grep -c "STRAT-CMD accepted" session1-completed-game.log
23
$ grep "STRAT-CMD accepted" session1-completed-game.log | grep -c "kind=Move"
12
$ grep "STRAT-CMD accepted" session1-completed-game.log | grep -c "kind=Attack"
6
$ grep "STRAT-CMD accepted" session1-completed-game.log | grep -c "kind=EndTurn"
5
```

No line in this slice carries both an accepted human command and an applied AI command with the
same kind/turn ambiguity — the two channels (`STRAT-CMD` for the human, `STRAT-AI` for the
opponent) never overlap in the same line, by construction of the two distinct format strings.

**8. `BP_StratGameMode`'s `MatchConfig` — not re-measured here.** `AiSides=(1)`,
`AiBuildlistUnitIds=("Infantry","Infantry","Infantry","Tank")`, `AiTurnDelaySeconds=0.500000`,
`AiMaxCommandsPerTurn=256`, `AiMaxConsecutiveTurns=64` were read live off the running CDO by
`strat-editor-builder` and independently re-read by the dispatching agent in the same session that
produced this PIE log; that measurement is not repeated here because `execute_script` is
disconnected in this session (the editor was opened after this session started — the known
NeoStack connector latch) and no config file backs those values (they are Blueprint-default
struct fields, not `.ini` entries). What this slice *does* independently confirm is the runtime
consequence: `AiSides=(1)` (item 3 above), `AiTurnDelaySeconds=0.500000` (item 1), and the
Infantry/Infantry/Infantry/Tank buildlist's Tank entry never being reached (item 5) — three of the
five values are corroborated behaviourally by this log; `AiMaxCommandsPerTurn=256` and
`AiMaxConsecutiveTurns=64` are not directly falsifiable from a 5-turn, ≤22-command-per-turn game
and are taken on the prior CDO reading, not re-measured here.
