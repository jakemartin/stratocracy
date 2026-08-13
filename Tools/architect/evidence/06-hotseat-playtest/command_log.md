# Phase 6 — 7-turn hot-seat playtest command log

Source: `Stratocracy-backup-2026.08.13-15.07.27.log` (preserved byte-identical in this
directory, md5 `baa918eade6b1061e9346c47a3b82d54`, matching the copy still under
`Saved/Logs/`). This is the log the 27 `STRAT-CMD accepted` lines rotated into after an
editor restart moved them out of the then-current `Saved/Logs/Stratocracy.log`. All line
numbers below are against this file.

## Session marker

```
3458:[2026.08.13-14.42.11:431][386]LogStratPlay: Match live: seeded from 'E:/MultiAgent/Stratocracy/Data/ferrum_crossing.json' (first side 0), drawn for side 0, 99 hexes and 10 units on screen.
```

## The 27 `STRAT-CMD accepted` lines, verbatim

```
3466:[2026.08.13-14.42.19:151][305]LogStratPlay: STRAT-CMD accepted kind=Move unit=3 hex=2,4 turn=1 side=0 hash=283faaba94137d7d
3467:[2026.08.13-14.42.28:356][407]LogStratPlay: STRAT-CMD accepted kind=EndTurn unit=-1 hex=-1,-1 turn=1 side=0 hash=1d35da278ac23f25
3468:[2026.08.13-14.42.38:748][653]LogStratPlay: STRAT-CMD accepted kind=Move unit=7 hex=6,3 turn=1 side=1 hash=dbd9d20738265d58
3469:[2026.08.13-14.42.45:240][430]LogStratPlay: STRAT-CMD accepted kind=EndTurn unit=-1 hex=-1,-1 turn=1 side=1 hash=3bd01b99daeb7cc0
3470:[2026.08.13-14.42.56:437][772]LogStratPlay: STRAT-CMD accepted kind=Move unit=3 hex=4,4 turn=2 side=0 hash=f418d2fdd36c467f
3471:[2026.08.13-14.42.57:996][959]LogStratPlay: STRAT-CMD accepted kind=EndTurn unit=-1 hex=-1,-1 turn=2 side=0 hash=3819b2bb05530530
3472:[2026.08.13-14.42.59:660][158]LogStratPlay: STRAT-CMD accepted kind=Move unit=7 hex=5,3 turn=2 side=1 hash=b3156b591851eb98
3473:[2026.08.13-14.43.00:577][268]LogStratPlay: STRAT-CMD accepted kind=Attack unit=7 hex=4,4 turn=2 side=1 hash=b646598b07a5dcbf
3474:[2026.08.13-14.43.03:001][559]LogStratPlay: STRAT-CMD accepted kind=EndTurn unit=-1 hex=-1,-1 turn=2 side=1 hash=93eb9e89d1e1361e
3475:[2026.08.13-14.43.05:018][800]LogStratPlay: STRAT-CMD accepted kind=Attack unit=3 hex=5,3 turn=3 side=0 hash=6a4a58db7faf15a5
3480:[2026.08.13-14.43.29:733][759]LogStratPlay: STRAT-CMD accepted kind=EndTurn unit=-1 hex=-1,-1 turn=3 side=0 hash=fb3dde38272f84ce
3482:[2026.08.13-14.43.30:658][870]LogStratPlay: STRAT-CMD accepted kind=EndTurn unit=-1 hex=-1,-1 turn=3 side=1 hash=8c092f82d8efa441
3484:[2026.08.13-14.43.32:508][ 92]LogStratPlay: STRAT-CMD accepted kind=Attack unit=3 hex=5,3 turn=4 side=0 hash=9d293cc6db161fe6
3485:[2026.08.13-14.43.33:367][195]LogStratPlay: STRAT-CMD accepted kind=EndTurn unit=-1 hex=-1,-1 turn=4 side=0 hash=2b7622e1a39f29ff
3486:[2026.08.13-14.43.34:395][318]LogStratPlay: STRAT-CMD accepted kind=Attack unit=7 hex=4,4 turn=4 side=1 hash=86b90a98e955e370
3487:[2026.08.13-14.43.34:911][380]LogStratPlay: STRAT-CMD accepted kind=EndTurn unit=-1 hex=-1,-1 turn=4 side=1 hash=c6865b3cad22f548
3488:[2026.08.13-14.43.35:737][479]LogStratPlay: STRAT-CMD accepted kind=Attack unit=3 hex=5,3 turn=5 side=0 hash=71e94dcca81a5eb3
3489:[2026.08.13-14.43.36:053][517]LogStratPlay: STRAT-CMD accepted kind=EndTurn unit=-1 hex=-1,-1 turn=5 side=0 hash=bee62a88c818cdfc
3490:[2026.08.13-14.43.36:879][616]LogStratPlay: STRAT-CMD accepted kind=Attack unit=7 hex=4,4 turn=5 side=1 hash=66dba7efc0b9f47f
3491:[2026.08.13-14.43.37:196][654]LogStratPlay: STRAT-CMD accepted kind=EndTurn unit=-1 hex=-1,-1 turn=5 side=1 hash=88a6481a3f3d0bc3
3492:[2026.08.13-14.43.38:104][763]LogStratPlay: STRAT-CMD accepted kind=Attack unit=3 hex=5,3 turn=6 side=0 hash=7ef5f92325565590
3493:[2026.08.13-14.43.38:478][808]LogStratPlay: STRAT-CMD accepted kind=EndTurn unit=-1 hex=-1,-1 turn=6 side=0 hash=05cb6e0945935ab1
3495:[2026.08.13-14.43.39:700][954]LogStratPlay: STRAT-CMD accepted kind=Attack unit=7 hex=4,4 turn=6 side=1 hash=419d14aeccd72512
3496:[2026.08.13-14.43.40:224][ 17]LogStratPlay: STRAT-CMD accepted kind=EndTurn unit=-1 hex=-1,-1 turn=6 side=1 hash=c9abbce437fd0e8a
3497:[2026.08.13-14.43.41:700][194]LogStratPlay: STRAT-CMD accepted kind=Attack unit=3 hex=5,3 turn=7 side=0 hash=2396231871e29aa5
3498:[2026.08.13-14.43.42:300][266]LogStratPlay: STRAT-CMD accepted kind=EndTurn unit=-1 hex=-1,-1 turn=7 side=0 hash=18e2db666c91a25e
3499:[2026.08.13-14.43.43:916][460]LogStratPlay: STRAT-CMD accepted kind=Attack unit=7 hex=4,4 turn=7 side=1 hash=7617ce50420ae45c
```

The first attack, `3473`, lands on hex `4,4` — the hex side 0's unit 3 had moved to one line
earlier, at `3470` (`kind=Move unit=3 hex=4,4 turn=2 side=0`).

## The six interface-level `Selection:` refusals, same session

```
3476:[2026.08.13-14.43.26:078][321]LogStratPlay: Selection: unit 7 belongs to side 1 and side 0 is to move
3477:[2026.08.13-14.43.26:745][401]LogStratPlay: Selection: unit 3 has finished this turn
3478:[2026.08.13-14.43.28:345][593]LogStratPlay: Selection: unit 7 belongs to side 1 and side 0 is to move
3479:[2026.08.13-14.43.28:987][670]LogStratPlay: Selection: unit 3 has finished this turn
3481:[2026.08.13-14.43.30:134][807]LogStratPlay: Selection: unit 3 belongs to side 0 and side 1 is to move
3483:[2026.08.13-14.43.31:109][924]LogStratPlay: Selection: unit 7 belongs to side 1 and side 0 is to move
```

These are pre-submission selection-machine refusals (wrong side to move, unit already
finished) — logged at `Selection:`, not `STRAT-CMD refused`, and confirmed by grep to be a
disjoint prefix from every `STRAT-CMD` line in this file.

## Measured shape

- `grep -c "STRAT-CMD accepted"` → **27**.
- By kind: `Move` × **4**, `Attack` × **10**, `EndTurn` × **13**. (4+10+13 = 27.)
- `grep -c "STRAT-CMD refused"` → **0**.
- `grep "kind=Wait"` → **0 matches**, confirming the family is disjoint (see wait witness below).
- Every one of the 27 `hash=` values is distinct — verified by `sort | uniq -c`, max count 1.
- Turn sequence runs 1 → 7 against a stated cap of 20 (`turnCap 20`, GDD Ferrum Crossing
  scenario), all 27 lines from `turn=1` through `turn=7`.
- **Side-flip count, measured, not taken from the task brief: 13, not 12.** Walking the 27
  `side=` values as an ordered sequence and counting each place consecutive values differ:
  `0,0,1,1,0,0,1,1,1,0,0,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1` — transitions at positions
  (2→3, 4→5, 6→7, 9→10, 11→12, 12→13, 14→15, 16→17, 18→19, 20→21, 22→23, 24→25, 26→27),
  which is 13. Recorded as a correction to this task's own framing, per "the tree wins" —
  the brief said 12, the log says 13. This does not change any pass/fail result above; it is
  a count correction only.
