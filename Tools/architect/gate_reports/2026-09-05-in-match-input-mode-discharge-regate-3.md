# Gate: the in-match input-mode discharge (RE-GATE 3) — strat-integration-reviewer

## Scope

Uncommitted working tree at `E:\MultiAgent\Stratocracy`, branch `master`, over base commit
`c5d2c65`. This is the FOURTH gate on this tree. All three prior reports were read in full first:
`2026-09-05-in-match-input-mode-discharge.md`, `...-regate.md`, `...-regate-2.md` (BLOCK, BLOCK,
BLOCK). `.agents/ue-project-context.md` was checked against the tree and needed no correction.

`git status --porcelain` was run rather than taken from the brief, and prints **seven** modified
files, nothing staged, plus the three prior gate reports as untracked:

```
 M Source/StratPlay/StratOptionsPresenter.h
 M Source/StratPlay/Tests/StratOptionsPresenterClauses.cpp
 M Source/StratUI/StratOptionsWidget.h
 M Tools/architect/state/content.md
 M Tools/architect/state/engine.md
 M Tools/architect/state/global.md
 M Tools/architect/state/tests.md
?? Tools/architect/gate_reports/2026-09-05-in-match-input-mode-discharge-regate-2.md
?? Tools/architect/gate_reports/2026-09-05-in-match-input-mode-discharge-regate.md
?? Tools/architect/gate_reports/2026-09-05-in-match-input-mode-discharge.md
```

`git diff --stat`: 351 insertions, 8 deletions across those seven. `HEAD` is still `c5d2c65`.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing. Control: the same command over `Source/StratUI` printed one file, so the instrument can speak. Manifests derived, not quoted: `rulesCommit 96d93ea9e490c8cc33261f5d9671389a9c76ecfd`, `dataCommit c2f58608c77c60c44e6c0fc87988bd3b372beaf5` |
| 2 | No `strat::` call outside `StratBridge/`/`StratRules/` | pass — the lowercase-initial call grep over `Source/` minus those two directories returned zero. Control: the same pattern inside `Source/StratBridge/` returns 140. The 557 remaining `strat::` tokens outside those dirs were enumerated by name and judged, not counted: every lowercase-initial one (`strat::buildUiSnapshot` x16, `strat::findPath` x9, ...) is prose or, in one case, a string literal — `StratMatchCompletionRecording.cpp:508`, inside a `TEXT(...)` failure message. No type name was reported as a call |
| 3 | No vendored header in front of UHT | pass — eight `.h` files include a vendored header; each counted for line-start `UCLASS`/`USTRUCT`/`UENUM` and all eight returned 0. Control: the same count on `Source/StratUI/StratOptionsWidget.h` returns 2 |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` | pass — the only `TUniquePtr<FStratBridge>` in `Source/` is prose at `StratUI/StratViewModel.h:47` recording the `C4150`. Control: `TPimplPtr<FStratBridge>` returns the two real members (`StratMatchSubsystem.h:2601`, `StratScoreboardHUD.h:1235`) plus their prose |
| 5 | Module arrows unchanged and correct | pass — derived, not trusted; 16 rows, 10 field names, guard read. See below |
| 6 | New modules registered | pass — five `Source/*/` dirs, 4 carrying `IMPLEMENT_MODULE`, 4 names in `Stratocracy.uproject`'s `Modules`; `StratRules` correctly absent from both. No `.Build.cs` in the diff |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed nothing; control as in check 1 |
| 8 | No `/Game/` path literal in C++ | pass — run with `MSYS_NO_PATHCONV=1`. One non-`Tests/` executable hit, `Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54`, the standing commandlet exception; two further hits (`StratSaveGame.h:140`, `StratShellSubsystem.h:761`) are doc comments saying the path is *not* a `/Game/` literal. Instrument proven live: 325 hits across `Source/` in total |
| 9 | No widget-side arithmetic | pass — `Source/StratUI/StratOptionsWidget.h` is in the diff, and its non-comment bytes are identical to HEAD's (derivation and control below). No arithmetic could have been added |
| 10 | Lane discipline | pass — engineer in `Source/StratUI/` + `Source/StratPlay/` + `engine.md`; test-author in `Source/StratPlay/Tests/` + `tests.md`; coordinator in `content.md` (fallback) + `global.md`. No test code outside `Tests/`, no production code inside it, no asset touched |
| 11 | Prose blocks present | **BLOCK** — not on the new prose's shape, which is house-style throughout. On two claims the current tree carries that are false against it. See Findings 1 and 2 |
| 12 | Nothing staged or committed | pass — seven ` M` entries read through `cat -A`, no index column set; `HEAD` still `c5d2c65` |

### Check 5, derived rather than restated

The extraction over `Source/*/*.Build.cs` printed **16 rows**, all five modules present, matching
the carried block row for row, including the sixteenth,
`StratBridge  PrivateDependencyModuleNames  StratRules`. The field census printed **10 names** —
the documented set, including the `BRIDGE=` string-literal false positive, which was not chased
and not sed-ed away. There is no eleventh.

The sixteenth row was read for its guard rather than treated as an unconditional arrow.
`Source/StratBridge/StratBridge.Build.cs:133-137`:

```
bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
if (!bCompileVendoredRulesHere)
{
	PrivateDependencyModuleNames.Add("StratRules");
}
```

Both halves come from the one expression; the link edge is monolithic-only and correct;
`Source/StratBridge/Vendored/` still holds 12 `*.strat.cpp`. Nothing in check 5 moved this pass.

### Comment-only across all THREE `Source/` files — re-derived, with a control

The brief's measurement was re-derived rather than accepted, by the stronger whole-file
instrument. For each of `Source/StratPlay/StratOptionsPresenter.h`,
`Source/StratPlay/Tests/StratOptionsPresenterClauses.cpp` and
`Source/StratUI/StratOptionsWidget.h`: HEAD's copy and the worktree copy, both with every
comment-only line stripped and CR normalised, compare **IDENTICAL**. That is stronger than "the
added lines were comments" — it also rules out a line of code having been commented out or a
comment uncommented.

**Control, because an empty difference is worthless without one:** renaming one non-comment token
in a **disposable copy** of the stripped `StratOptionsWidget.h` makes the same `diff` report a
difference. No file in the repository was modified to obtain it.

### The exported report, read here

`Saved/AutomationReport/index.json`, `utf-8-sig`: `reportCreatedOn 2026.09.06-02.19.18`,
`succeeded 435`, `failed 0`, `notRun 0`, `succeededWithWarnings 0`, `totalDuration 5.038963...`.
**435 entries, counted from the entry list, every one in state `Success`.**

### The "no suite re-run was owed" claim — verified, not accepted

The brief asked for this to be checked rather than taken. Measured directly: the newest `.cpp`
under `Source/` carrying an `IMPLEMENT_*_AUTOMATION_TEST` macro is
`Source/StratPlay/Tests/StratOptionsPresenterClauses.cpp` at **22:17:19**, and the report's own
mtime is **22:19:18** — the report is newer than every test-defining `.cpp`, which is exactly what
the sweep's REPORT IDENTITY check compares. The files touched after that run are
`global.md` (22:22:11), `StratOptionsWidget.h` (22:35:07), `engine.md` (22:36:41),
`content.md` (22:37:56) and `tests.md` (22:39:07) — one `.h` and four `.md`, none of which can
enter `newest_test_mtime`. The claim holds as measured.

### The banner sweep, with its exit code measured here and a control

Run with a redirect so the status read is the script's own and not a pipeline's:

```
  automation report: 435 entries, all Success (2026.09.06-02.19.18)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=435, COMPLEX/_CLASS=0
  report identity: mtime=1788661159, reportCreatedOn='2026.09.06-02.19.18'
  suite claims found: 63 (1 live, 62 stamped)
  tracked items declared: 2

SWEEP CLEAN -- no self-contradiction found
```

Exit code **0**. **Control:** the six record files were copied to a scratch directory, the single
live `435/435` in the COPY of `global.md` changed to `434/434`, and the script run against those
copies by passing them as its `state_path` arguments. It printed two `SUITE COUNT AGREEMENT`
findings and `SWEEP FAILED -- the record contradicts itself or the tree`, and exited **1**.
`git status --porcelain -- Tools/architect/state/` was re-read afterwards and shows the same four
modified entries as before; no repository file was modified to obtain the control.

`--explain` finds **one** LIVE suite claim in the whole record, at `global.md:21` (`435/435`).
Every other three-digit claim is classified stamped. Check on the "only `global.md`" rule: pass.

No build and no suite were run by this gate.

### Attribution and base commit, per lane

- `engine.md:18-19` and `:98-99` — two entries, both `strat-gameplay-engineer` (ACTING and
  WRITING; IN LANE ..., base commit `c5d2c65`, UNCOMMITTED), inline, both saying in terms that no
  exception clause applies and none is cited.
- `tests.md:17-19` — `strat-test-author` (ACTING and WRITING; IN LANE ... base commit `c5d2c65`
  ... UNCOMMITTED), inline.
- `content.md:178-190` — the `coordinator` named inline, under this file's fallback condition,
  with its control re-measured at the time of writing.
- `global.md` — the coordinator's own file; no clause needed.

All four attribute inline; the three lane entries name `c5d2c65`.

### The two prior findings, judged on their answers

**Regate-2 Finding 1 (`StratOptionsWidget.h:57`) — answered.** The stamp at `:58-90` says what is
true now, names `d80b28a` as the commit that added the asset and `374398a` as the sentence's
origin, says plainly the sentence was false when written rather than stale, names the pinning
clause, and declares the `AMENDED 2026-09-05` block below it unaffected so its silence is not
read as agreement. Every element was re-derived here independently and every one stands:
`git merge-base --is-ancestor d80b28a 374398a` exits 0; `git log --no-walk --format='%h %ad'
--date=short` prints `2026-09-05` for BOTH, so the date genuinely cannot order them;
`git log --oneline --diff-filter=A -- Content/UI/WBP_Options.uasset` names `d80b28a`;
`grep -a -c` returns `WBP_Options=2, OptionsWidgetClass=1` on all three shipped GameMode assets
and `0/0` on the negative control `Content/TopDown/Blueprints/BP_TopDownGameMode.uasset`;
`command -v strings` is empty, so the reported silent instrument is real. **One sentence inside
that otherwise sound stamp is a finding — see Finding 2.**

**Regate-2 Finding 2 (`tests.md`'s "byte-identical" bullet) — answered, and the replacement is the
more durable form.** The brief asked me to judge whether it has merely moved the problem. It has
not. The bullet now asserts only what this lane DID ("This pass wrote exactly two files ... That is
a fact about this lane's own actions, and no other lane can falsify it") and names `git diff
--stat` as the instrument for the state question rather than answering it. That is durable in the
way a time qualifier is not: a claim about a completed act by the writing lane cannot be falsified
by a concurrent lane, whereas a qualifier's window can be zero. The following bullet records the
first draft's three falsified clauses by name and draws the lesson without narrating the gate. Two
of the three would be false again today if restated, which is the point: they are quoted as
history, not asserted. **Judged: the correct shape, and better than a stamp.**

**The coordinator's own repair (`content.md:172`) — sufficient.** The false clause
*"The unseen half above is unchanged and still owed"* is now followed, inside the SAME bracketed
stamp and one line down, by *"CORRECTED BY THE STAMP DIRECTLY BELOW, WHICH THIS SENTENCE NOW
POINTS FORWARD TO."* A reader arriving at `:172` by citation cannot leave the bracket without
meeting the correction, which is the whole requirement. Stamping the sentence itself would have
been no better and would have destroyed what was true when written. **Judged sufficient.**

## Findings

**1. A `## NEXT` entry in `content.md` still tells a reader the options screen has no controls on
it, and that file's own header says an entry which has become history must be stamped. Owner:
`strat-editor-builder`, or the `coordinator` under this file's fallback condition, which is
currently open and measured.**

This is the fourth instance of the shape the brief predicted, found by the sweep it commissioned.

`Tools/architect/state/content.md:192-193`, the bullet's own bolded heading:

> **THE TITLE THEME AND THE OPTIONS WIDGET EXIST; THE OPTIONS SCREEN HAS NO CONTROLS ON IT AND
> THAT IS A HUMAN'S WORK, NOT A MISSING STEP.**

and three sentences inside it, `:217-224`:

> **THE OPTIONS SCREEN HAS NO SLIDERS AND CANNOT GET THEM ON THIS ROUTE.**
> ... THE THREE SLIDERS AND THE BACK CONTROL ARE OWED TO A HUMAN IN THE EDITOR ...
> Until then the `Options` route opens a blank panel, which is worse for a player than no route
> at all ...

and, worst of the four because it is itself a later-added STAMP and so reads as current,
`:243-244`:

> It does NOT cover the three volume sliders, **which no surface reaches**, nor the options screen.

**All of it is false against the tree this gate would be passing, and the correction is seventy
lines ABOVE it in the same file, pointing nowhere back.** Measured:

- `content.md:129-131`, the entry over base `c13eb81`, records what was built: *"a `Stack` holding
  three label/slider/value triples plus `BackButton`."*
- The asset agrees. `grep -a -c` on `Content/UI/WBP_Options.uasset` returns `MasterSlider=1`,
  `MusicSlider=1`, `SfxSlider=1`, `BackButton=1`; the negative control
  `Content/UI/WBP_TitleMenu.uasset` returns `Slider=0`, so the instrument discriminates rather
  than matching everything.
- `Source/StratUI/StratOptionsWidget.h:364-377` declares those four as `BindWidget`, which is a
  Blueprint compile error if the named child is missing — and all three shipped GameModes load
  green in the 435/435 run above.
- `content.md:185` — written THIS PASS, eight lines from the top of the file — says *"A human has
  seen and operated the screen and signed off on the layout pass that followed."*

**Why this gates rather than sitting under Observations, stated so it can be argued with.** I
declined to gate on `content.md:172` at the previous gate on the ground that a dated record entry
is history and this file stamps in place. That reasoning does not reach this one, because the
file's own header removes the excuse. `content.md:48-49`:

> Everything under `## NEXT` is swept as live; stamp an entry that has become history rather than
> deleting it, exactly as `state.md` did.

The entry is under `## NEXT`. It has become history. It is not stamped for it — its only stamp, at
`:239`, is about the listening pass and *repeats* the falsified claim. The base commit in the
entry's second line (`089c79c`) does not discharge this either: the header declares the whole
section live, and `089c79c` is older than the `c13eb81`-based entry that built the sliders, so the
date is what makes the staleness measurable rather than what excuses it. The fix is one stamp in
this file's established shape — the sliders and the back control landed in the pass recorded over
`c13eb81`, name it, and say the paragraph reads as history.

**Scope of the claim, stated because an over-broad one names another thing.** I am not asserting
that every superseded entry in this 2,500-line file needs a stamp today. This one is named because
it is about the exact surface this phase is closing, because its headline is what a reader
scanning the file's bold headings sees, and because the brief commissioned precisely this sweep.

**2. `engine.md` makes an affirmative false claim about a `git` command, in the same bullet in
which this lane corrects itself for having done exactly that. Owner: `strat-gameplay-engineer`.**

`Tools/architect/state/engine.md:56-59`:

> `git log --oneline --reverse -S "still owed is the WBP" -- Source/StratUI/StratOptionsWidget.h`
> names `374398a` as the sentence's ORIGIN -- `--reverse` deliberately, this project's recorded
> rule that the plain form answers MOST RECENT, and **here the rule genuinely bites rather than
> being cited by analogy.**

Run, both forms of that exact command return the identical single line:

```
$ git log --oneline --reverse -S "still owed is the WBP" -- Source/StratUI/StratOptionsWidget.h
374398a The volume screen exists and is reachable from the title menu and from inside a match ...
$ git log --oneline -S "still owed is the WBP" -- Source/StratUI/StratOptionsWidget.h
374398a The volume screen exists and is reachable from the title menu and from inside a match ...
```

Only one commit matches that token in that file, so ordering is vacuous and `--reverse` changed
nothing. **The rule does not bite here; it is cited by analogy, which is the one thing that
sentence affirmatively denies.** The conclusion — `374398a` is the origin — is correct and is
independently confirmed by the ancestry walk above. It is the justification that is invented.

This is the same defect the previous gate blocked on, one file over, and the same defect this very
bullet's neighbour at `engine.md:160-172` diagnoses in its own words:

> **A justification sentence that names a command is a claim ABOUT THAT COMMAND, and the only way
> to write one is to run it and paste back what came out.**

**The second site, milder, and named because the fix is the same edit.**
`Source/StratUI/StratOptionsWidget.h:61-63` carries the weaker form:

> `git log --reverse -S "still owed is the WBP"` over this file names `374398a` as the sentence's
> ORIGIN -- `--reverse` deliberately, because the plain form answers most-recent rather than first.

That stops short of asserting the two forms differ here, but *deliberately* implies `--reverse`
was load-bearing when it was not, and a reader who runs it will find it inert. Fixing one site and
not the other leaves the implicature standing in `Source/`, which is the harder place to find it.
The honest repair for both is the instrument that actually settles the question and is already in
the tree: `git log --diff-filter=A` for the asset, plus `merge-base --is-ancestor` for the order.

## Observations

Non-gating.

- The `content.md` stamp added this pass (`:178-190`) discharges both of the fallback condition's
  own obligations — the control is re-measured and the writer is named inline — but it names no
  base commit, while the three entries above it each name theirs. Nothing in that clause requires
  it to; recorded only because a reader cannot otherwise date the stamp against the tree.
- `Source/StratPlay/Tests/StratShippedSoundBankParity.cpp:874` says *"the volume control the audio
  milestone still owes -- which will hang on a sound class."* Read in place, its subject is the
  MIX-ROUTING consequence of a wrong sound class, and the sliders now exist: the sentence is stale
  in its noun rather than false in its argument, and the clause it documents is green. It is the
  nearest miss in `Source/` to Finding 1's shape and was judged rather than forwarded.
- Every other surviving `unseen` / `nobody has seen` / `still owed` hit across `Source/`,
  `Tools/architect/state/` and `.agents/` was read this pass and none is about the options screen,
  the WBP or the in-match input mode: `content.md:2049`/`:2142`/`:2330`/`:2539`,
  `decisions.md:643`/`:663`, `engine.md:4693`/`:6531`/`:6756`/`:6914`/`:7709`,
  `global.md:2089`/`:3450`/`:3756`/`:3961`/`:4116`/`:4819`/`:5544`/`:7045`/`:7089`/`:7207`, and
  `StratMatchSubsystem.h:2183`, `StratGuidedOpening.h:115`/`:480`, `StratBuildAffordance.cpp:71`,
  `StratInfoPanelSourceParity.cpp:266`. `Content/` was swept too, with `grep -a`: no debt prose in
  any asset, and the control (`grep -a -c WBP_Options` on a GameMode asset returning 2) proves that
  scan could have spoken.
- The `input mode` sweep across `Source/`, `Tools/architect/state/` and `.agents/` finds no
  surviving sentence calling the in-match input mode open or owed. `content.md:169`'s *"takes no
  input mode"* is now correct as a recorded decision, and its own stamp at `:186-189` says so.
- All seven files in the diff still emit `warning: ... LF will be replaced by CRLF ...` on every
  `git diff`. Known behaviour of this tree under `core.autocrlf=true`; no bytes at risk.
- `.agents/ue-project-context.md` was checked against the tree this pass and needed no correction.

VERDICT: BLOCK
