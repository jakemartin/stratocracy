# Phase 6 — assert_log_contains results

All eight ran against the same session's log during the interactive playtest and were
reported PASS. Re-verified here independently against the preserved backup log
(`Stratocracy-backup-2026.08.13-15.07.27.log`, this directory) by direct grep — bytes, not
narration.

| ID | Pattern | Result | Matching line |
|---|---|---|---|
| A1 | `kind=Move` | PASS | present, 4× (3466, 3468, 3470, 3472) |
| A2 | `kind=Attack` | PASS | present, 10× |
| A3 | `kind=EndTurn` | PASS | present, 13× |
| A4 | `kind=Move unit=3 hex=2,4 turn=1 side=0` | PASS | `3466` |
| A5 | `kind=Move unit=7 hex=6,3 turn=1 side=1` | PASS | `3468` |
| A6 | `kind=EndTurn unit=-1 hex=-1,-1 turn=7 side=0` | PASS | `3498` |
| A7 | `kind=Attack unit=7 hex=4,4 turn=2 side=1` | PASS | `3473` |
| A8 | `kind=Attack unit=3 hex=5,3 turn=3 side=0` | PASS | `3475` |

Expected-absent, both confirmed false (i.e. correctly absent):

| Pattern | Expected | Result |
|---|---|---|
| `STRAT-CMD refused` | false | confirmed — 0 matches in the session log |
| `kind=Wait` | false | confirmed — 0 matches in the session log |

A4 + A5 together are the side-flip assertion: same `turn=1`, different `side=` (0 vs 1),
proving the hot-seat hand-over actually alternated control rather than one side acting twice.

## What this table does and does not prove

These eight assertions document that a **stored artifact** — one specific rotated log file
from one specific human-driven session — contains the lines it is claimed to contain. Re-run
today, `assert_log_contains` reads whatever is currently in `Saved/Logs/Stratocracy.log`
(now holding only the wait line below); every one of the eight would fail against a fresh
log, because NeoStack input injection does not reach `UPlayerInput` and no automated
session can reproduce the click sequence that produced these 27 lines. This is a record of
what happened once, not a gate that can be re-run.
