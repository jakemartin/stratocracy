# Evidence: `09-content-independence`

The second half of phase D's "avoid one-corpus proof" precedent. Editor CLOSED throughout.
Post-milestone, 2026-08-19. Nothing staged, nothing committed by this pass.

## What was open, in the words the record used

Phase D asked that the pairing invariant not rest on a single corpus. The combat-outcome
milestone's phase 4 discharged the **host** half — the same content re-graded through a live PIE
session instead of the headless suite, all eleven `STRAT-AI turn-ended` hashes byte-identical.
The **content** half stayed open and was carried, untouched, through five later passes:

> a different scenario/buildlist/first side, genuinely different game content

The reason it matters is not ceremony. A gate that has only ever graded one game cannot be told
apart from a gate that has memorised it.

## What was built

One new clause, `Stratocracy.StratPlay.T-INT-05.BothSidesAiReachesAResultOnDifferentContent`,
appended to `Source/StratPlay/Tests/StratAiMatchClauses.cpp` so it reuses that file's world
scope, config helper and `GLog` capture rather than duplicating them. Suite **107 -> 108**.

It plays an AI-vs-AI game on two moved axes, **both derived, neither typed**:

| Axis | Phase-3 corpus | This corpus | Derived from |
|---|---|---|---|
| First side | `0` (`kFirstSide`) | `1` | the projection's own side list, taking the side phase 3 did not start |
| Buildlist | the whole table, 4 rows | one entry, `Recon` | the unit table's own row order, last row |

The clause asserts each axis actually moved rather than trusting it: `TestNotEqual` against
`kFirstSide`, and a size comparison against the full table.

## Why the buildlist had to shrink to ONE entry, and the live proof of it

`strat::chooseBuild` returns the cheapest **affordable** entry, and no Fame level makes a dearer
unit affordable while a cheaper one is not. Handing this run a different *multi-entry* list would
therefore have fielded the same army again. Measured on the phase-3 corpus's own checked-in
slice:

```
$ grep -o "STRAT-AI applied kind=Build[^\"]*def=[0-9-]*" \
    Tools/architect/evidence/08-combat-pairing-gate/real-run/t-int-05-both-sides-ai-slice.log \
    | grep -o "def=[0-9-]*" | sort | uniq -c
     22 def=0
```

Twenty-two builds, every one `def=0` — Infantry, the cheapest row. **This is the first live
confirmation of the `chooseBuild` finding**, which until now had only been argued from source in
`Tools/architect/evidence/upstream-chooseBuild-buildlist-ratio.md` and filed upstream at crew
`85995b8`. It is corroboration of that request, not a substitute for it: the finding is still
that a ratio cannot be expressed, and the cap ruling is still unimplemented.

## The corpora, side by side

Both graded in the SAME session log, by the same gate, minutes apart.

| | phase-3 corpus | this corpus |
|---|---|---|
| First side | 0 | 1 |
| Buildlist | whole table | `Recon` alone |
| AI turns | 11 | 8 |
| Commands | 156 | 100 |
| `applied kind=Attack` | **68** | **42** |
| `STRAT-COMBAT resolved` | **68** | **42** |
| Builds | 22 (all `def=0`) | 10 |
| Pairing mismatches | **0** | **0** |
| Verdict | PASS | PASS |

The attack counts differ by 26 and the turn counts by 3, which is the point: this is not the same
game re-run, and the invariant holds across both.

## The four runs, verbatim

`gate_output.txt` holds all four with their exit codes:

1. The new corpus graded inside the full session log, the gate finding its own slice — 42/42, 0
   mismatches, PASS, `EXIT=0`.
2. The same corpus as the standalone checked-in slice — identical figures, PASS, `EXIT=0`, which
   is what makes `different-content-slice.log` a faithful self-contained copy rather than an
   excerpt someone trimmed by eye.
3. **The control**: the phase-3 corpus, same session, same gate — 68/68, 0 mismatches, PASS,
   `EXIT=0`. Identical to the figures phase 3 and phase 4 both recorded, from a third session on
   a different day, which is a determinism reading this pass got for free.
4. **Falsifiability on the NEW content**: the first two `applied kind=Attack` lines swapped in a
   scratch copy, nothing else touched. The gate reports the same 42/42 totals and **fails
   anyway** — `pairing mismatches: 2`, naming both, `FAIL`, `EXIT=1`. Ordered identity, not
   counts, on this content as on the old.

The scramble was done in the session scratchpad and **deliberately not** added to
`08-combat-pairing-gate/fixtures/`: `strat_fixture_verdict_binding.py` asserts that directory's
contents against its declaration set in both directions, so a 12th `.log` there would have failed
the binding rather than extended it.

## The slice

`different-content-slice.log` — 201 lines, cut at the test's own `Test Started.` /
`Test Completed.` markers (lines 4208..4408 of the session log), both present as the file's first
and last lines. Bounded by content, never by EOF or by a line range, per the standing rule in
`state.md`'s NEXT. Named for what it is.

## Post-gate additions, 2026-08-19

The reviewer's gate on `185e88f` returned `VERDICT: BLOCK` on a `state.md` banner contradiction
(fixed there, not here) plus two non-gating observations against this directory, both acted on:

- **The falsifiability run is now reproducible from the repository alone.** `gate_output.txt`
  run 4 originally printed only its output, against a session scratchpad path that no longer
  exists — so the one run proving the gate can still fail was the one a reader could not repeat.
  It now carries the scramble as a RECIPE (a one-line `python -c` over the checked-in slice, then
  the gate invocation). The recipe was run verbatim afterwards and reproduces the same two named
  mismatches on the same lines, `FAIL`, `EXIT=1`.
- **A third reading, taken while re-running after the fix: this corpus is deterministic across
  sessions too.** Re-graded from a completely fresh headless session
  (`reportCreatedOn 2026.08.19-19.01.29`): 42 applied / 42 resolved / 0 mismatches, PASS —
  identical to the figures above from `18.40.01`. The control corpus already had that property
  across days; the new one now has it as well, which was not claimed when this document was
  first written. **Recorded verbatim as run 5 of `gate_output.txt`** — the re-gate observed that
  this reading rested on prose while run 4's had just been given a reproducible artifact, which
  is the same evidentiary gap one bullet apart; the run, its exit code, and both session
  identities are now in the file.

## What this does NOT prove

- **The scenario did not move.** `Data/ferrum_crossing.json` is the only scenario in the tree and
  `Data/` is vendored certified bytes, so "a different scenario" was not available to this pass
  without authoring one upstream. Two of the three axes the precedent named moved; the third did
  not, and this document does not claim otherwise. A second scenario remains the honest way to
  close the last of it.
- **Two corpora are not many.** This discharges "the gate has only ever seen one game". It does
  not establish the invariant across the space of games, and no finite number of corpora would.
- **The `!bSeeded` silent path fired zero times here too** (`STRAT-COMBAT refused: 0`, and the
  applied/resolved counts are equal). Same standing as phase 3: a measurement of these runs, not
  a structural guarantee.
- **`ReplayLog` still does not route through `Submit`** — phase 1's hole, unchanged and untouched
  by this pass. This gate reads live-session logs only.
- **Nothing here re-gates the milestone.** The milestone is complete; this is post-milestone work
  in `Tools/architect/` plus one clause in `Tests/`, and it has had no reviewer verdict.
