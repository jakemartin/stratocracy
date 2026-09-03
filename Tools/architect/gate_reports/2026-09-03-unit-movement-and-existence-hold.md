# Gate: unit movement, route slides and the tour existence hold — strat-integration-reviewer

## Scope

Working tree of `E:/MultiAgent/Stratocracy`, confirmed by
`git -C E:/MultiAgent/Stratocracy rev-parse --show-toplevel` -> `E:/MultiAgent/Stratocracy`.
Branch `master`, HEAD `283d7114c82c792639f28c05454a4e3e4cf974a9`. Uncommitted, as expected.
`git status --porcelain` prints **24 entries** — 20 modified, 4 untracked — matching the brief.
Audited: the full working-tree diff against `283d711` (8,247 insertions / 112 deletions across
20 tracked files) plus the four untracked files, the module graph derived from every
`Source/*/*.Build.cs`, `Stratocracy.uproject`, and the four `Tools/architect/state/` records.

Not run by me, by role: the build and the suite. The suite figures below are re-measured from
the exported report and the macro census, not from a run I made.

## Code, assets and module graph — the priority pass

**This pass is CLEAN. No finding in game code, in the asset, or in the module graph.**

- **Module arrows derive correctly.** The extraction command printed **16 rows** and the field
  census **10 names**, matching the recorded shape exactly. StratBridge's sixteenth row
  (`PrivateDependencyModuleNames StratRules`) was read at its source: it sits inside
  `if (!bCompileVendoredRulesHere)` in `StratBridge.Build.cs`, i.e. monolithic targets only, and
  is therefore correct and not a finding. No `.Build.cs` file is modified by this diff at all
  (`git diff -- 'Source/*/*.Build.cs'` is empty), so no arrow moved. No cycle. StratUI gained
  no Stratocracy dependency.
- **Vendored bytes untouched, verified rather than assumed.** `git diff --stat` prints nothing
  for `Source/StratRules` or `Data`, and `git status --porcelain` for those paths prints nothing
  (so no untracked additions either). Both directories are non-empty (26 and 6 files), and the
  same instrument is shown able to speak on `Source/StratPlay`. Manifests read
  `rulesCommit 96d93ea9...`, `dataCommit c2f58608...`, matching the recorded authority.
- **No `strat::` call escaped its two directories.** Both the lowercase-initial call form and the
  nested-namespace form return zero hits outside `Source/StratBridge/` and `Source/StratRules/`.
  Roughly 85 files outside those directories name `strat::` TYPES; every one judged, none is a
  call. `FStratAiCommandEffect` and `RosterDeltaOfLastCommand` keep the new roster observation
  behind `FStratBridge`, which is the correct shape.
- **No vendored header in front of UHT.** `StratBridge.h` is the only header including
  `Data.h` / `Replay.h` / `Save.h` / `Scenario.h` / `Ui.h`, and it declares no
  `UCLASS`/`USTRUCT`/`UENUM`. No header in the diff includes `StratBridge.h`.
- **`TPimplPtr`, never `TUniquePtr`.** Both live `FStratBridge` members
  (`UStratMatchSubsystem::Bridge`, `UStratScoreboardHUD::OwnedBridge`) are `TPimplPtr`. The only
  `TUniquePtr<FStratBridge>` occurrence in the tree is the C4150 measurement recorded in prose.
- **The move tween is structurally sound, read line by line.** `UnitRoot` is the new root and
  `Body` its child, so `ApplyUnitView`'s `SetActorLocation` line is unchanged and
  `GetActorLocation()` still snaps. The offset arithmetic cancels `BodyZOffset` exactly on all
  three arming paths, because each differences two points drawn from one source — `ApplyUnitView`
  uses `RouteWorldPoints[i] - RouteEnd`, `PlayRouteSlide` uses `Point - AnchorWorldPoint`,
  `ParkPictureAt` uses `ParkWorldPoint - AnchorWorldPoint`. `FinishTween` is the single
  retirement path, and it uses `SetRelativeLocation_Direct` + `UpdateComponentToWorld` so that
  "exactly zero" is literal rather than a round trip. `Tick`'s segment walk is bounded
  (`Segment + 2 < TweenArcLengths.Num()`), the degenerate-polyline and zero-duration arms both
  retire rather than divide, and the trapezoid helper handles `E <= 0`.
- **The existence hold's ordering is correct.** `BuildTourExistenceHolds()` is hoisted above the
  reconcile in `RunAiTurnsNow` — the only place the pre-hand-over roster still exists — and is
  gated on the same `WillAiPlaybackRun()` that `BeginAiPlayback` uses, so "gate up, no tour" is
  unconstructible rather than merely unlikely. `ApplyTourExistenceAtCursor` is a pure function of
  the cursor, so a skip needs no special case. `EndAiPlaybackTour` unhides unconditionally and
  then destroys every actor absent from `AppliedModel`, and it is reachable from six call sites.
- **The bridge roster delta is symmetric.** `AppearedOfLastCommand` / `DepartedOfLastCommand` are
  cleared on the `!R.ok` path and written only on the success path; `RosterDeltaOfLastCommand`
  empties its outputs before its guard.
- **No `/Game/` literal in gameplay C++.** Every hit is under a `Tests/` directory or in
  `ImportStratDataCommandlet.cpp` — the two standing exceptions.
- **No widget-side arithmetic.** No `Source/StratUI/` file is touched by this diff.
- **The asset.** `Content/StratPlay/BP_StratUnit.uasset` serialises `UnitRoot`, `Body` and
  `MoveTweenSeconds`. `content.md` records the hierarchy read off both the CDO and live PIE, with
  a positive control for the instrument after the first control was found invalid. In lane.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched (`Source/StratRules`, `Data`) | pass |
| 2 | No `strat::` call outside StratBridge/StratRules | pass |
| 3 | No vendored header in front of UHT | pass |
| 4 | `TPimplPtr` and not `TUniquePtr` for `FStratBridge` | pass |
| 5 | Module arrows unchanged and correct (16 rows, 10 fields) | pass |
| 6 | New modules registered (none added; StratRules correctly absent) | pass |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` path literal in gameplay C++ | pass |
| 9 | No widget-side arithmetic (`Source/StratUI/` untouched) | pass |
| 10 | Lane discipline | pass |
| 11 | Prose blocks present | pass |
| 12 | Nothing staged or committed (`git diff --cached` empty) | pass |
| — | Record accuracy (prose) | **BLOCK** — three findings below |

## Findings

### F1 — A clause justifies itself with the C++ default and calls it the shipped path. BLOCK.

`Source/StratPlay/Tests/StratTourExistenceHoldClauses.cpp:826-829`

    // THE FIXTURE IS THE SHIPPED DEFAULT AND THAT IS THE WHOLE POINT. `AiPlaybackStepSeconds`
    // defaults to `0.0f`, which is what every real match and every other automation fixture runs
    // at, so this clause pins the behaviour on the path the game actually takes.

and the same conflation at line 43 ("At the shipped default of `0.0f`") and in the test message
at line 872 ("an AI-vs-AI config assembles at the shipped playback default"), which reaches the
automation report.

`0.0f` is the **C++** default and is not what any real match runs at. `AiPlaybackStepSeconds` is
serialised as an override in BOTH shipped GameMode Blueprints — the property name is present in
the bytes of `Content/StratPlay/BP_StratGameMode.uasset` and
`Content/StratPlay/BP_StratGameMode_AiVsAi.uasset`, measured this pass — and the same pass's own
banner records the measured value at `Tools/architect/state/global.md:70-72`: "AI playback was
believed to ship INERT on a `0.0f` C++ default -- it does not: `AiPlaybackStepSeconds` is 0.5 on
both shipped GameMode Blueprints."

So `WillAiPlaybackRun()` is TRUE in every real match, and the no-tour path this clause covers is a
path the shipped game never takes. The clause is CORRECT and worth keeping — it pins that the gate
cannot be raised at a non-positive interval — but "this clause pins the behaviour on the path the
game actually takes" is an affirmative false coverage claim, and it is the exact defect this
project already recorded once: a comment describing the C++ default rather than the shipped
Blueprint value. Note the same file already knows better at lines 46-48: "§2.11.2's 0.5 is NOT
written in this file." One file, both claims live.

Owner: **`strat-test-author`** (`Source/*/Tests/` is its lane).

### F2 — engine.md's LIVE entry says the tree does not compile, says the five clauses are unwritten, and briefs a mutation since measured inert. BLOCK.

`Tools/architect/state/engine.md:18-22` (the topmost `## NEXT` entry, i.e. the live one):

    THIS STEP LEAVES THE TREE DELIBERATELY UNCOMPILABLE AND THE DEBT IS NAMED BELOW.

`Tools/architect/state/engine.md:146-151`:

    **THE TREE DOES NOT COMPILE AND THAT IS THIS STEP'S NAMED DEBT.** Build:
    `Result: Failed (OtherCompilationError)`. ... **NO SUITE WAS RUN AND NO REPORT WAS DELETED**

`Tools/architect/state/engine.md:155-156`:

    **WHAT THE TEST LANE IS OWED FOR THE HOLD, EACH WITH ITS ONE-LINE MUTATION. NOT WRITTEN AND
    NOT BUILT THIS PASS.**

All three are false of this tree. The four call sites were repaired, the five clauses exist, and
the exported report at `reportCreatedOn 2026.09.03-18.43.07` carries 386 entries all Success —
verified this pass by `strat_banner_sweep.py` (exit 0, `SWEEP CLEAN`, run without a pipe) and by
an independent macro census (386 in the worktree against 370 at `283d711`, +16 across three
waves). Unlike the identically-shaped block at `engine.md:825`, this one names no discharge
condition, so nothing in the file tells a reader it closed. This is the stale status line running
both ways: the engine lane's live entry reports a broken tree that is green.

The sharper half is at `Tools/architect/state/engine.md:160-162`:

    **MUTATION: in `UStratMatchSubsystem::BeginAiPlayback`, delete the
    `ApplyTourExistenceAtCursor();` call** -- the unit is then visible from the tour's start,
    which is the defect.

That mutation was measured INERT. `BeginAiPlayback` ends by calling `OnAiPlaybackTimer()` which
reaches `AdvanceAiPlaybackOneStep()`, which calls the same pure function again at cursor 1 inside
the same synchronous call, so the clause stays green under it. Both
`Source/StratPlay/Tests/StratTourExistenceHoldClauses.cpp:299-310` and
`Tools/architect/state/global.md:37-45` record that measurement. `engine.md` still states the
mutation unqualified and unstamped, in the same lane's live instruction sheet. A future reader who
runs it gets a green suite and concludes a live clause is inert. That is an instruction sheet
ordering a false finding — the failure this repository already committed a fix for in `f079b9f`.

Owner: **`strat-gameplay-engineer`** (`engine.md` is its file; the tree wins, so the record is
what changes, not the code).

### F3 — global.md's live banner asserts both five and six new clauses. BLOCK.

`Tools/architect/state/global.md:21` (live, this wave):

    THE COUNT MOVED 381 -> 386 AND EVERY ONE OF THE NEW CLAUSES IS AN ADDITION: **+5 added, 0
    removed, 0 state changes**

`Tools/architect/state/global.md:34`: "THE FIVE EXISTENCE-HOLD CLAUSES ARE THE +5".

`Tools/architect/state/global.md:81-85`, inside the same unstamped block:

    ALL SIX NEW CLAUSES ARE DISCHARGED BY MEASUREMENT, each by its own banner's named one-line
    mutation ... Both requested pairings hold: the anchor mutant reddens the height clause ALONE,
    and the two halves of `ApplyUnitView`'s new branch are reddened by one mutation each

The evidence that paragraph cites — the anchor mutant, the height clause, `ApplyUnitView`'s new
branch — belongs to the **+6 route-slide wave**, whose figure (381) IS stamped at lines 26-31.
The narrative was carried forward into the restamped block; the count sentence was not stamped
with it. The result is one live block asserting two different clause counts for one wave, and
asserting unqualified discharge where lines 37-45 of the same block say the opposite of one of
the five: "ONE BRIEFED MUTATION WAS MEASURED INERT ... Closing it properly needs a `const`
accessor ... which is NOT written." A reader landing at line 81 by citation takes away full
discharge. This is the inherited-prose-phrase shape: a sentence true of the wave it was written
for, false of the wave it now sits in, and invisible to `strat_banner_sweep.py`, which polices
suite counts and not clause counts.

Owner: **`coordinator`** (`global.md` is its file, with the steward).

## Observations

Non-gating.

- **The `AUnitBuiltDuringTheHandoverIsHiddenUntilItsStep` pin: ACCEPTABLE, not a gap.** You asked
  for a call. The clause pins the reveal MECHANISM by a real mutation — deleting
  `ApplyTourExistenceAtCursor()` from `AdvanceAiPlaybackOneStep`, which reddens property (2) — and
  the one line it cannot pin, the call in `BeginAiPlayback`, is declared at four sites: the
  clause's own header, `StratMatchSubsystem.cpp`, `tests.md` and `global.md`, together with the
  seam that would close it. A declared gap with a named closing condition is the discipline
  working, not failing. The residual risk is small and correctly scoped: the two calls differ only
  for a unit revealed at step index 0, in a window no frame is rendered in. What I would ask for
  is the `const` accessor `global.md:44` already names, so the gap stops being permanent. F2 is
  about the RECORD of that measurement, not about the pin.
- **The two undischarged roster-delta mutants: CONFIRMED OPEN, and correctly declared.**
  `Tools/architect/state/tests.md:200-213` names both one-line mutations in
  `Source/StratPlay/StratAiTurnRunner.cpp` — passing a default-constructed
  `FStratAiCommandEffect{}` to `Record`, and appending `Command.TargetId` on every accepted Attack
  — and closes with "Neither was built and neither may be cited as discharged; the engineer runs
  them." That is the right sentence and I re-scope nothing. My only note is that `global.md`'s
  live banner does not surface this debt at all, and its "ALL SIX NEW CLAUSES ARE DISCHARGED"
  (F3) reads across it.
- **Two standing unpinnable lines are declared rather than hidden**, and both are correct as
  written: `AdvanceAiPlaybackOneStep`'s arm-last ordering (`StratMatchSubsystem.cpp:2102-2116` — a
  clause for it was written, run, failed on the unmutated tree, and deleted rather than softened)
  and `ApplyView`'s unconditional `PendingMoveRoutes.Reset()`, an equivalent mutant.
- **`AdvanceOneStepStillArmsNoTimer`** (`Tools/architect/state/engine.md:551`) is a clause name
  that exists nowhere in `Source/`. The surrounding prose is clear that it is proposed and
  unpinnable, so this is not a fabricated citation — but the name greps like a defined one, and a
  reader searching for it finds only the record. Worth a marker.
- **`MoveTweenSeconds` carries a rename hazard that is declared and half-owned.**
  `Source/StratPlay/StratUnitActor.h` records that renaming it to `MoveTweenSecondsPerHex` would
  orphan `BP_StratUnit`'s authored 0.2 with no diagnostic, and that the mitigation is a
  `+PropertyRedirects` line in `Config/DefaultEngine.ini` — the data steward's lane. Recorded here
  so it is not lost between the two lanes.
- **Attribution is clean everywhere I checked.** `content.md` names acting and writing inline and
  MEASURES the fallback condition with a control (`execute_script` answered; `project_dir()`
  returned this tree), so the editor-driver clause was correctly NOT invoked. `engine.md` and
  `tests.md` entries carry `(ACTING and WRITING; IN LANE ...)` with the base commit named.
  `global.md` is the coordinator's own file and says so.
- `Source/StratPlay/Tests/StratRouteTweenUnitDouble.{h,cpp}` is a test double inside `Tests/`,
  carries a why-first block, and sets `MoveTweenSeconds = 0.25f` locally rather than copying
  `BP_StratUnit`'s shipped values — the right call, and it says why.
- Git reports LF/CRLF conversion warnings on `Source/StratBridge/StratCombatLog.h`,
  `Source/StratPlay/Tests/StratAiTurnRunnerClauses.cpp` and
  `Source/StratPlay/Tests/StratMatchReconcile.cpp`. Known repo condition — line endings vary per
  file here — preserve per file. Not a finding.

VERDICT: BLOCK
