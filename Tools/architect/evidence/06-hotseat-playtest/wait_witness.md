# Phase 6 — the wait witness

From the *current* `Saved/Logs/Stratocracy.log` (a separate, later session — not the
7-turn playtest whose command log lives beside this file):

```
3266:[2026.08.13-15.19.03:526][571]LogStratPlay: STRAT-WAIT spent unit=5 turn=1 side=0 (no rules command submitted; the save format has no Wait kind)
```

## Why this is a separate sentinel, not a `STRAT-CMD` variant

A wait submits nothing to the rules module: `strat::SaveCommandKind` (`Save.h:54`) enumerates
`Move, Attack, Build, Capture, EndTurn` and has no `Wait` entry. Emitting `STRAT-CMD accepted
kind=Wait …` would misrepresent a presentation-only event as a rules-level acceptance. Keeping
`STRAT-WAIT` and `STRAT-CMD` as disjoint log-line families — under *every* prefix, not just
this one line — preserves `grep "STRAT-CMD accepted"` as an honest count of commands that
actually reached and were applied by the rules module. Confirmed for this session:
`grep "kind=Wait"` over the playtest log returns 0 matches (see `assertions.md`).

It carries no `hash=` field, deliberately. A wait moves no rules state — `StateHash()` is
unchanged by it — and printing a hash beside a wait would dress a presentation-only event as
though it had rules-level consequence, which is exactly the confusion the disjoint prefix is
built to prevent.

## What this means for replay

A wait leaves no trace in `RecordedLog()` and structurally cannot, since the save format has
no command to record. A replay of the recorded log reproduces state hashes faithfully but not
the player's experience: a unit that waited replays indistinguishably from a unit that never
acted. This is a property of the vendored save format, not a bug introduced by this project —
`Wait` was never in `strat::SaveCommandKind` to begin with (also recorded in
`Tools/architect/state.md`, phase 1, "the orchestration brief said `Wait` and was wrong").
