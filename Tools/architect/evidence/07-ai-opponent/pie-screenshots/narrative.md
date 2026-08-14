# PIE session 2 — the screenshot run (a separate, short session)

Slice of `Saved/Logs/Stratocracy.log`, bounded by content markers, not a line-number range into a
log a running editor is still appending to (see "Re-cut, 2026-08-13" below): opens at
`LogWorld: Bringing World /Game/StratMaps/UEDPIE_0_Lvl_FerrumCrossing.Lvl_FerrumCrossing up for
play` (full-log line 3443) and closes at this session's own `LogPlayLevel: Display: Destroying
online subsystem :Context_2` (full-log line 3480) — 38 lines, file
`session2-screenshot-run.log`. This is the second of two PIE windows in that log. Opens at
`2026.08.14-00.36.56`, teardown begins at `2026.08.14-00.37.13`. **This session is NOT the
completed game.** It is a separate, short PIE run — one AI turn played, one human `EndTurn` and one
AI turn recorded — started roughly six minutes after session 1 (`../pie-session/`) had already torn
down. The two screenshots below were taken inside this window, not session 1's.

## Re-cut, 2026-08-13: bounded by this session's own shutdown marker, not by EOF

Originally cut "3443 to EOF" — at the time, line 3481, which was already false by the time anyone
read the file (the log kept growing under a running editor; it measured 3518 lines the next time it
was checked, and larger again after that). "To EOF" of a file still being appended to is not a
reproducible boundary — it means "to whenever the cut happened to run." The tail it picked up was
also wrong on its own terms: the slice's last three lines were `[UsageMonitor]` telemetry from
*after* this PIE session had already ended, included only because the tool happened to reach EOF at
that instant. Re-cut to end at this session's own `Destroying online subsystem :Context_2` line
(3480) — the last line that is unambiguously this PIE session's own output — dropping that trailing
noise. Renamed from `session2-lines-3443-3481.log` to `session2-screenshot-run.log`, for the same
reason as session 1's rename: the old name asserted a range that was already wrong the moment the
log grew past it, and would be wrong again tomorrow. See `../blackboard.md` for the general lesson.

## Why two sessions, and why they are kept apart

The two PIE windows in `Saved/Logs/Stratocracy.log` are sequential, not concurrent: session 1 (the
five-turn completed game, `../pie-session/`) opened `00.29.41` and tore down `00.30.59`; session 2
opened `00.36.56`, five-plus minutes later, in a fresh PIE process. They do not share a match, a
seed instance, or a turn counter — session 2 starts again at `TURN 1 / 20`. Screenshot
`ScreenShot00042` (`00.36.59`) and `ScreenShot00043` (`00.37.08`) both fall inside session 2's
window only. **Do not read the screenshots as showing the completed game measured in
`../pie-session/`** — they show a different, shorter run, kept for the visual record because it is
the only PIE run of this milestone that was screenshotted.

## Gate result

`python Tools/architect/strat_ai_log_gate.py pie-screenshots/session2-screenshot-run.log`, run
directly, exit 0:

```
applied lines:            8
turn-ended lines:         1
terminal refusals (ok):   0
blocking refusals:        0
parse failures:           0
PASS
```

Reproduces the dispatching agent's reported numbers exactly (8/1/0/0/0, exit 0 PASS), unchanged
from the pre-re-cut result — the dropped tail (post-session `[UsageMonitor]` noise) carried no
`STRAT-AI`/`STRAT-CMD` content, so removing it moved zero counts. Full output captured in
`gate_output.txt`. Zero terminal refusals is expected and correct — the game did not reach a §2.8
result in one turn; this is not the completed game and was never meant to run one out.

## The two screenshots

- **`ScreenShot00042.png`** (log line 3456, `00.36.59:976`) — full editor window, PIE viewport
  plus Outliner/Details/NeoStack chat panels, not viewport-only. Scoreboard reads `TURN 1 / 20`,
  `Unit HP 60 / 60` — before either side has acted. Ferrum Crossing board visible with units
  placed at start-of-match positions. Viewed directly; confirmed.
- **`ScreenShot00043.png`** (log line 3468, `00.37.08:371`) — same full-window framing. Scoreboard
  reads `TURN 2 / 20`, `Unit HP 60 / 70` — a visibly changed board from `00042`. The Output Log
  panel, legible in the captured frame, carries: `STRAT-CMD accepted kind=EndTurn ... turn=1
  side=0` (the human closing turn 1), followed by the AI's `STRAT-AI applied kind=Build`, six
  `kind=Move` lines, `kind=EndTurn`, and `STRAT-AI turn-ended side=1 turn=1 commands=8`. Viewed
  directly; confirmed — matches this slice's own lines exactly (see below).

## Cross-check: the log panel visible in `00043` against this slice

The Output Log frame captured inside `ScreenShot00043.png` is a screenshot of the *same* log this
directory gates, taken mid-session. Confirmed against `session2-screenshot-run.log`:

```
STRAT-CMD accepted kind=EndTurn unit=-1 hex=-1,-1 turn=1 side=0 hash=4cb76dc8b02e6002
STRAT-AI applied kind=Build unit=-1 hex=7,4 def=0 target=-1 turn=1 side=1 hash=8943098f2a577b95
STRAT-AI applied kind=Move unit=8 hex=8,1 def=-1 target=-1 turn=1 side=1 hash=54d24801858aba67
STRAT-AI applied kind=Move unit=7 hex=6,3 def=-1 target=-1 turn=1 side=1 hash=43f09716c93acf66
STRAT-AI applied kind=Move unit=10 hex=4,4 def=-1 target=-1 turn=1 side=1 hash=1d9787454f21c219
STRAT-AI applied kind=Move unit=11 hex=4,6 def=-1 target=-1 turn=1 side=1 hash=1d113db16553bfd3
STRAT-AI applied kind=Move unit=6 hex=5,4 def=-1 target=-1 turn=1 side=1 hash=f167207ea6d94ef5
STRAT-AI applied kind=Move unit=9 hex=6,4 def=-1 target=-1 turn=1 side=1 hash=6918a047809e2f31
STRAT-AI applied kind=EndTurn unit=-1 hex=0,0 def=-1 target=-1 turn=1 side=1 hash=834c46ecb0bd905a
STRAT-AI turn-ended side=1 turn=1 commands=8 hash=834c46ecb0bd905a
```

Six `kind=Move` lines, counted directly from the quoted block above (`grep -c
"STRAT-AI applied kind=Move" session2-screenshot-run.log` = 6 — qualified by channel and kind
rather than the bare, unqualified `grep -c "kind=Move"`, which happens to return the same 6 in
this slice only because the human issued no `Move` command in this 38-line window; the qualified
form states that intent and stays correct even if a human `Move` is ever recorded here), plus one
`Build` and one `EndTurn` — eight commands, matching `commands=8` on the `turn-ended` line and the
eight `applied` lines this slice's gate counted overall. This is the entirety of session 2's AI
activity — this slice contains no turn 2 AI action, consistent with the screenshot capturing the
board immediately after turn 1 handed back to turn 2 and before any further command was issued.

## Measurements from this slice (session 2 only — do not merge with session 1's)

- **`AiSides=(1)` confirmed again, independently.** All 8 `applied` and the 1 `turn-ended` line
  carry `side=1`; the 1 `STRAT-CMD accepted` line carries `side=0`.
- **Build `def=0` again.** The one `STRAT-AI applied kind=Build` line in this slice carries `def=0`
  (Infantry), consistent with session 1's 11-for-11. Combined across both sessions: **12 of 12**
  `Build` lines carry `def=0`; Tank (`def=1`) appears zero times. See item 5 of the milestone-level
  account.
- **Pacing not independently re-measured here** — this slice has only one human `EndTurn` (line
  3457) followed by the AI's response (line 3458): `00.37.05:857` → `00.37.06:358` = **0.501 s**,
  a sixth data point, consistent with the five in `../pie-session/` and with
  `AiTurnDelaySeconds = 0.500000`.
- **Gate handles PIE corpus shape a second time**, on a differently-shaped (shorter,
  single-turn) session: 0 parse failures.
