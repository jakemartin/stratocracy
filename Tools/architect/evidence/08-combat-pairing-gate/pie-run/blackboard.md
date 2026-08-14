# Evidence: `08-combat-pairing-gate/pie-run`

Combat-outcome milestone, phase 4 — the same gate's second corpus, its first from a live PIE
session rather than headless automation. Editor OPEN at capture time, still appending to the
log — bound by content markers, never EOF, per the standing project discipline.

## The corpus

`Saved/Logs/Stratocracy_2.log` — a NEW file after a Claude Code restart. No line number is
carried forward from `Stratocracy.log` (which was itself rotated between phase 3 and phase 4;
its phase-3 content now lives at `Saved/Logs/Stratocracy-backup-2026.08.14-14.23.36.log`,
confirmed unaffected — see "Regression check" below). Every number in this record was re-derived
against `Stratocracy_2.log` directly.

## Markers, verified rather than trusted

The builder reported these; each was checked directly against the file before use.

```
$ awk 'NR==3462{print NR": "$0} NR==3469{print NR": "$0}' Saved/Logs/Stratocracy_2.log
3462: [...]LogPlayLevel: Creating play world package: /Game/StratMaps/UEDPIE_0_Lvl_FerrumCrossing
3469: [...]LogLoad: Game class is 'BP_StratGameMode_AiVsAi_C'

$ awk 'NR==3724{print NR": "$0} NR==3727{print NR": "$0}' Saved/Logs/Stratocracy_2.log
3724: [...]LogWorld: BeginTearingDown for /Game/StratMaps/UEDPIE_0_Lvl_FerrumCrossing
3727: [...]LogPlayLevel: Display: Shutting down PIE online subsystems
```

All four marker lines matched exactly. **Cut `3462 -> 3727`** (inclusive), per the builder's
recommendation: this puts the `Game class is 'BP_StratGameMode_AiVsAi_C'` line INSIDE the slice,
so the corpus identifies its own GameMode rather than relying on a claim in a narrative.

## The contamination check — verified, not assumed

A plain `playtest_log_contains` finds `Game class is` twice in this file — there is an earlier,
pre-travel PIE session under the shipping GameMode:

```
$ grep -n "Game class is" Saved/Logs/Stratocracy_2.log
3427:[...]LogLoad: Game class is 'BP_StratGameMode_C'
3469:[...]LogLoad: Game class is 'BP_StratGameMode_AiVsAi_C'

$ grep -n "Creating play world package" Saved/Logs/Stratocracy_2.log
3380:[...]LogPlayLevel: Creating play world package: [...]Lvl_FerrumCrossing   <- earlier session
3462:[...]LogPlayLevel: Creating play world package: [...]Lvl_FerrumCrossing   <- this slice's open

$ grep -n "STRAT-" Saved/Logs/Stratocracy_2.log | awk -F: '$1<3462' | wc -l
0
```

Zero `STRAT-*` lines exist anywhere before line 3462. The earlier shipping-GameMode session
produced none, so there is no contamination crossing the boundary — measured directly, not
inferred from the builder's claim, exactly because this is the kind of boundary claim this
project has been wrong about before (phase 5 / hot-seat milestone's confounded `Lvl_TopDown`
control is the standing precedent for checking this rather than trusting it).

## Slice cut, checked in

```
$ sed -n '3462,3727p' Saved/Logs/Stratocracy_2.log > pie-run/ai-vs-ai-pie-session-slice.log
$ wc -l pie-run/ai-vs-ai-pie-session-slice.log
266 pie-run/ai-vs-ai-pie-session-slice.log
```

`ai-vs-ai-pie-session-slice.log` — named for what it is (one PIE session's own AI-vs-AI match),
not for where it sat in the parent log. First line is the `Creating play world package` open
marker, last line is the `Shutting down PIE online subsystems` close marker — both present in
the checked-in file itself, so a reader does not have to trust the cut, only look at the file.

## Counts, re-derived (all builder figures confirmed)

```
$ grep -c "STRAT-AI applied kind=Attack" ai-vs-ai-pie-session-slice.log
68
$ grep -c "STRAT-COMBAT resolved" ai-vs-ai-pie-session-slice.log
68
$ grep -c "STRAT-COMBAT refused" ai-vs-ai-pie-session-slice.log
0
$ grep -c "STRAT-COMBAT divergence" ai-vs-ai-pie-session-slice.log
0
$ grep -c "STRAT-CMD accepted" ai-vs-ai-pie-session-slice.log
0
$ grep -c "STRAT-AI refused" ai-vs-ai-pie-session-slice.log
1
$ grep "STRAT-AI refused" ai-vs-ai-pie-session-slice.log
[...] STRAT-AI refused phase=apply kind=EndTurn unit=-1 hex=0,0 def=-1 target=-1 turn=6 side=1 reason=[T-SAVE-05] no match is running
$ grep -o "STRAT-AI applied kind=[A-Za-z]*" ai-vs-ai-pie-session-slice.log | sort | uniq -c
     68 STRAT-AI applied kind=Attack
     22 STRAT-AI applied kind=Build
     11 STRAT-AI applied kind=EndTurn
     55 STRAT-AI applied kind=Move
$ grep -o "agree=[0-9-]* diverge=[0-9-]*" ai-vs-ai-pie-session-slice.log | sort | uniq -c
     68 agree=1 diverge=0
```

`resolved`=68 is the positive control behind the three zeros (`refused`, `divergence`,
`STRAT-CMD accepted`): the same grep on the same file finds 68 `STRAT-COMBAT` lines, so the
instrument can see this family on this corpus, and a zero elsewhere is a real absence rather than
the grep itself being blind on this file shape.

## Ordering, confirmed rather than assumed

`STRAT-COMBAT resolved` is emitted BEFORE its `STRAT-AI applied` partner (the bridge logs during
`Submit`, the runner after) — checked directly on this corpus, not inferred from phase 3's:

```
$ grep -n "STRAT-COMBAT resolved\|STRAT-AI applied kind=Attack" ai-vs-ai-pie-session-slice.log | head -4
33:[...]LogStratBridge: STRAT-COMBAT resolved attacker=10 defender=5 hex=3,4 ...
34:[...]LogStratPlay: STRAT-AI applied kind=Attack unit=10 hex=3,4 ...
38:[...]LogStratBridge: STRAT-COMBAT resolved attacker=9 defender=5 hex=3,4 ...
39:[...]LogStratPlay: STRAT-AI applied kind=Attack unit=9 hex=3,4 ...
```

`resolved` immediately precedes its `applied` partner throughout. The gate's own ordered-identity
pairing check does not assume this ordering — it pairs by INDEX after independently collecting
both lists in file order — so this ordering is a property of the corpus, confirmed, not a
precondition the gate silently relies on.

## The gate's first PIE corpus — a new flag added, no pairing logic touched

`strat_combat_pairing_gate.py`'s slicer was built for headless automation logs and hard-requires
`Test Started.` / `Test Completed.` markers — this PIE log has neither. Added `--pre-sliced`
(mirrors `strat_ai_log_gate.py`'s own established posture of "point this at an ALREADY isolated
log"): with the flag, the entire given file is treated as the slice and the automation-marker
search is skipped entirely; the pairing logic, the parsers, and every failure mode are
UNCHANGED. Proven backward-compatible and proven able to fail in the new mode both:

```
$ python strat_combat_pairing_gate.py Saved/Logs/Stratocracy-backup-2026.08.14-14.23.36.log
[... unchanged phase-3 content, still PASS -- confirms no regression from the --pre-sliced add]

$ python strat_combat_pairing_gate.py --self-test
[13 fixtures, up from 11 -- two new: "--pre-sliced mode passes a clean corpus with no
automation markers at all" and "--pre-sliced mode still fails a real ordering-scramble fault"]
SELF-TEST: ALL FIXTURES CORRECT

$ for f in ../fixtures/*.log; do python strat_combat_pairing_gate.py "$f" \
    --test-path "Stratocracy.Fixture.FIX-01.FixtureTest"; echo "EXIT=$?"; done
[unchanged: 7x EXIT=1, 3x EXIT=0 -- same as before the flag was added]
```

## The gate run against the checked-in PIE slice

```
$ python strat_combat_pairing_gate.py pie-run/ai-vs-ai-pie-session-slice.log --pre-sliced
```

Verbatim output in `gate_output.txt`:

```
STRAT-COMBAT pairing gate: Tools\architect\evidence\08-combat-pairing-gate\pie-run\ai-vs-ai-pie-session-slice.log
  test:                       (pre-sliced input, no automation markers)
  slice (1-based lines):      1..266
  STRAT-AI applied kind=Attack:  68
  STRAT-COMBAT resolved:         68
  STRAT-COMBAT refused:          0
  STRAT-COMBAT divergence:       0
  STRAT-AI terminal refusals:    1
  STRAT-AI blocking refusals:    0
  STRAT-COMBAT parse failures:   0
  STRAT-AI parse failures:       0
  pairing mismatches:            0
PASS
EXIT=0
```

## Host independence, not content independence — the hash diff that settles it

68/68 is EXACTLY phase 3's headless number. With deterministic rules, a deterministic AI, and
the same scenario / buildlist / first side, that is expected: the same game replayed through a
different host, not a new game. Settled by measurement, not left as a coincidence:

```
$ grep -o "STRAT-AI turn-ended.*hash=[a-f0-9]*" pie-run/ai-vs-ai-pie-session-slice.log \
    | grep -oE "side=[0-9]+ turn=[0-9]+ commands=[0-9]+ hash=[a-f0-9]+" > pie_hashes.txt
$ grep -o "STRAT-AI turn-ended.*hash=[a-f0-9]*" ../real-run/t-int-05-both-sides-ai-slice.log \
    | grep -oE "side=[0-9]+ turn=[0-9]+ commands=[0-9]+ hash=[a-f0-9]+" > headless_hashes.txt
$ diff pie_hashes.txt headless_hashes.txt
[no output]
$ echo $?
0
```

All eleven `STRAT-AI turn-ended` hashes are byte-identical between the PIE corpus and phase 3's
headless corpus, turn for turn, side for side, command-count for command-count. Full listing in
`hash_diff.txt`.

**Coordinator's ruling, recorded as written: the phase-D "avoid one-corpus proof" precedent is
PARTLY discharged by this run, not fully and not still fully open.** The HOST-independence half
— the same live-PIE path with no simulated input reaching the same result as headless automation
— IS discharged, confirmed by the byte-identical hash diff above. The CONTENT-independence half —
a different scenario, buildlist, or first side, genuinely different game content rather than a
different way of driving the same one — is NOT discharged and is out of phase 4's scope; see
`state.md`'s phase-4 entry for where it is carried forward as its own future work. The
identical-hash result makes that remaining half concrete rather than theoretical: this project
now has two runs of the exact same game through two different hosts, not two different games.

## Two honest limits, self-reported by the builder, recorded rather than smoothed over

1. **`AiTurnDelaySeconds` does not pace per turn.** The whole game ran in ~4 ms:
   ```
   $ grep -n "16.15.29:141\|16.15.29:145" pie-run/ai-vs-ai-pie-session-slice.log | head -1
   20:[2026.08.14-16.15.29:141][ 15]LogStratPlay: STRAT-AI applied kind=Build ...
   $ grep "STRAT-AI refused" pie-run/ai-vs-ai-pie-session-slice.log
   [2026.08.14-16.15.29:145][ 15]LogStratPlay: Warning: STRAT-AI refused phase=apply kind=EndTurn ...
   ```
   `:141` -> `:145`, four milliseconds for the whole match. The delay paces *entry into*
   `RunAiTurnsIfDue`; once inside, `RunAiTurnsNow` — "plays every consecutive AI turn that is
   due, synchronously, and reconciles after," per its own doc comment in
   `StratMatchSubsystem.h` — plays every remaining AI turn in one call. Cited by the function's
   doc comment, not a line number, per the standing "cite functions, not line numbers" lesson.
   **Consequence: there are no distinct seed / mid-progression / result screenshots of this
   match** — confirmed by directory listing:
   ```
   $ ls -la Saved/Screenshots/WindowsEditor/*.png | tail -2
   [... ScreenShot00042.png / ScreenShot00043.png, both dated Aug 13 20:37 -- phase D's captures,
   already checked into evidence/07-ai-opponent/pie-screenshots/. No screenshot in the directory
   postdates this phase-4 PIE run.]
   ```
   The builder explicitly refused to present the earlier shipping-GameMode session's pristine seed
   image as this match's seed — recorded here approvingly; that refusal is the correct call and
   the opposite of what this project has been burned by (a confounded control standing in for the
   real one). This also corrects phase D's "0.5 s pacing" reading as it applies to this run: the
   pacing is real, but it is not per-turn once a paced entry fires.

   **A committed message this correction cannot reach:** `eda3b4b` ("An AI-vs-AI GameMode, so the
   corpus does not come from a config that exists nowhere") carries the premise
   "`AiTurnDelaySeconds` stays 0.5 so turns pace as phase D measured and mid-progression
   screenshots mean something." This run falsifies that premise. The commit message is immutable
   and was the coordinator's; this record is the correction, kept where a reader of `state.md`
   (and this blackboard) will find it, since `git log` alone will not carry it.
2. **No result tier is logged.** No `STRAT-*` line carries one, and the match subsystem is
   unreachable from Lua. The observable finish evidence is the turn count plus the
   `[T-SAVE-05] no match is running` signature:
   ```
   $ sed -n '3714,3717p' Saved/Logs/Stratocracy_2.log
   [...] STRAT-COMBAT resolved attacker=6 defender=1 hex=3,4 turn=6 side=1 [...] fameBefore=200 fameAfter=700 [...] agree=1 diverge=0
   [...] STRAT-AI applied kind=Attack unit=6 hex=3,4 def=-1 target=1 turn=6 side=1 hash=cde6dc4a3ed2bf36
   [...] STRAT-AI refused phase=apply kind=EndTurn ... reason=[T-SAVE-05] no match is running
   [...] The paced AI turn refused: the rules module refused the AI's EndTurn for side 1 after 16 applied command(s): [T-SAVE-05] no match is running
   ```
   which is what a CORRECT §2.8 finish looks like, not a fault (the same terminal-refusal shape
   phase 1 and 3 already established). Reached turn 6; eleven `STRAT-AI turn-ended` lines;
   the killing blow at line 3714 (whole-file numbering) took side 1's fame from `fameBefore=200`
   to `fameAfter=700` — confirmed verbatim in the `sed` output above, matching the report exactly.

## Gate debt — real, non-gating, NOT to be built now

`--pre-sliced` returns `PASS` and exit 0 on an EMPTY or WRONG corpus. Measured directly:

```
$ : > empty.log
$ python strat_combat_pairing_gate.py empty.log --pre-sliced
STRAT-COMBAT pairing gate: empty.log
  test:                       (pre-sliced input, no automation markers)
  slice (1-based lines):      1..0
  STRAT-AI applied kind=Attack:  0
  STRAT-COMBAT resolved:         0
  ...
  pairing mismatches:            0
PASS
$ echo $?
0
```

The marker-slicing path hard-fails on a wrong file via `SliceFailure`; `--pre-sliced` has no
equivalent self-identification check, and `GateResult.passed` carries no non-zero floor — zero
applied attacks and zero resolved lines currently reads as a trivially-satisfied pairing rather
than "nothing was actually checked." **This does not touch this run's result** — the checked-in
slice is content-bounded, carries both boundary markers as its own first and last lines, and has
68 real pairs — but the flag trades a structural corpus-identity guard for caller discipline, and
that trade was not previously recorded here or in `state.md`. This project has already shipped a
commit titled "a gate whose own fixtures could not fail it"; a known silent-pass in the
instrument this milestone's results rest on is exactly that class of debt, and it should be
discharged before this gate is pointed at a corpus anyone has not eyeballed. **Owed shape:** a
`--expect-min-pairs N` argument, or an outright refusal on a zero-event pre-sliced corpus, plus a
14th self-test fixture proving the new guard can itself fail. **Not built now** — the scope fence
stands, phase 5 is the doc pass, and this is deferred to the next time the gate script is
touched. See `state.md`'s phase-4 entry for the same record.

## Regression check on the log rotation

`Saved/Logs/Stratocracy.log` (no `_2`) was rotated between phase 3 and phase 4 — it is now a
short, unrelated 3492-line session containing zero occurrences of
`BothSidesAiReachesAResultWithinTheBound`. This is expected editor behavior on restart, not a
data loss: the byte-identical phase-3 content is preserved at
`Saved/Logs/Stratocracy-backup-2026.08.14-14.23.36.log`, confirmed still gate-clean (see the
`--pre-sliced` regression check above). Nothing in phase 3's evidence or `state.md` entry depends
on the live path `Saved/Logs/Stratocracy.log` remaining stable — the phase-3 checked-in slice
(`../real-run/t-int-05-both-sides-ai-slice.log`) is unaffected either way.

## Verdict status

**Gated 2026-08-14, `VERDICT: PASS`, zero findings, first gate — the only phase this milestone
that did not need a re-gate.** The reviewer re-derived everything independently as a third
derivation: the slice diffs identical to `sed -n '3462,3727p'`, contamination clean (first
`STRAT-` line is 3481, after the 3462 cut), counts 68/68/0/0/0 plus the one `[T-SAVE-05]`
refusal, 68× `agree=1 diverge=0`, the 11-hash diff at exit 0, and `fameBefore=200 fameAfter=700`
verbatim — plus confirmed no screenshot substitution anywhere and that `--pre-sliced` leaves the
pairing loop, both parsers, and `GateResult.passed` byte-unchanged. Per the standing lesson (now
paid for three times in `state.md`'s own history), before writing this line the command
`grep -n "gated\|VERDICT\|verdict" Tools/architect/state.md` was run and every phase-4-relevant
site checked — the top-of-file banner and mid-file summary as well as the phase-4 entry itself,
the exact site that produced a `BLOCK` last time.
