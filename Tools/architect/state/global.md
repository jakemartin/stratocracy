# Stratocracy state — global

> **Sole writer: `coordinator + strat-data-steward`.** Split out of `Tools/architect/state.md` on 2026-08-20, which is now
> FROZEN as the historical record of closed milestones. This directory is the live record.
>
> **Holds:** Milestone status, the banner, the candidate backlog, and the cross-lane carried-debt ledger. **The only file that may state a suite count or a phase verdict.**
>
> **One file owns each fact class.** Other files may *link* to a fact recorded here; they may not
> restate it. Only `global.md` may carry a live suite count or a phase verdict — a live `N/N`
> in any other file is a finding, enforced by `strat_banner_sweep.py`'s RECORD OWNERSHIP check.
> Everything under `## NEXT` is swept as live; stamp an entry that has become history rather
> than deleting it, exactly as `state.md` did.

_Last run 2026-08-28 (WAVE 2 LANDS ON MASTER: SEC 2.11.2'S INFO PANEL HAS A MODEL SIDE, AND THE
FIGURE WAS MEASURED ON THIS TREE RATHER THAN CARRIED OVER FROM THE LANE THAT PRODUCED IT.
Written by the `coordinator`, whose file this is.
THE PROVENANCE IS THE INTEGRATION TREE: this run was made in `E:/MultiAgent/Stratocracy` on
branch `master`, AFTER the `--no-ff` merge of `feat/info-panel`. NO REBASE WAS PERFORMED AND NONE
WAS NEEDED -- the branch's merge-base and `master` both read `a45a7d9`, so it was already on
master's head, and `git merge` reported `Automatic merge went well` with ZERO conflicts. The
suite is
**259/259**, every entry Success, zero failed, zero notRun, zero succeededWithWarnings.
THE DELTA OVER PRE-MERGE `master` IS **+10 AND NOTHING WAS REMOVED OR RENAMED**, by multiline
set-difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST` -- never by an acceptance-ID grep, and never
by a single-line one, which returns ZERO in this tree because the macro's argument sits on the
next line. Measured against `a45a7d9`'s own git objects: 249 there, 259 here, removed set EMPTY.
All ten added names carry `GATE-INFOPANEL`, which mints NO acceptance ID.
THE REPORT THAT CERTIFIES THE LIVE FIGURE IS THIS TREE'S: `reportCreatedOn
2026.08.28-13.29.19`, `succeeded 259`, `failed 0`, `notRun 0`, 259 entries and zero non-Success,
read directly by the `coordinator`. THAT STAMP IS UTC AND THIS ENTRY'S DATE IS LOCAL.
THE TREE WAS REBUILT HERE BEFORE IT WAS RUN: `Result: Succeeded`, `REAL_EXIT=0`, 62 compile
actions -- so the green describes THESE bytes and not the lane's. A merge is a change, and a
green from before it describes different bytes; that is why this figure was re-measured rather
than inferred from the lane's identical 259.
**[STAMPED 2026-08-28 -- THE LANE'S OWN RUN IS QUOTED BELOW AS HISTORY AND DOES NOT BACK THIS
FIGURE. It was `reportCreatedOn 2026.08.28-00.32.37`, also 259/259, made in
`E:/MultiAgent/Strat-wt/slot-1` before this merge existed. It agrees with this one, and agreement
is not provenance: the banner below states the lane as its tree and this entry states this one.
Two further runs of that wave, at 258/258 and an earlier 259/259, are stamped in that banner.]**
THE NARROW RE-GATE RETURNED `VERDICT: PASS` WITH ZERO FINDINGS on the lane, and the full gate
before it returned `VERDICT: BLOCK` on two findings, both fixed and both named in the banner
below. **NO GATE HAS RUN ON THIS TREE SINCE THE MERGE, and that is stated rather than left to be
assumed** -- a `VERDICT: PASS` describes a tree and a merge is a post-pass change, so the
post-merge re-gate is owed and is not claimed here. What IS measured here is the build, the
suite and the delta, each named above with the instrument that produced it.
WHAT THIS WAVE DOES NOT DO: no widget binds `FStratViewModel::InfoPanel`. Sec 2.11.2's panel has
a complete model side and nothing draws it, so the asset tail is owed on this tree in an editor
batch. That is scope stated honestly, not a defect -- but the recorded hazard of a correct value
with no route to a screen now applies to the panel until a later wave binds it.)

_Last run 2026-08-28 (SEC 2.11.2'S INFO PANEL HAS A MODEL SIDE, AND THE WAVE WAS WIDER THAN THIS
FILE'S OWN SUMMARY OF IT. Written by the `coordinator`, whose file this is.
THE PROVENANCE IS THE LANE TREE, NOT THE INTEGRATION TREE: this run was made in
`E:/MultiAgent/Strat-wt/slot-1` on branch `feat/info-panel`, whose merge-base, `master` and `HEAD`
all read `a45a7d9` -- so NO REBASE WAS NEEDED and none was performed. The suite is
**259/259**, every entry Success, zero failed, zero notRun, zero succeededWithWarnings.
THE DELTA OVER `master` IS **+10 AND NOTHING WAS REMOVED OR RENAMED**, by multiline set-difference
on `IMPLEMENT_SIMPLE_AUTOMATION_TEST` -- never by an acceptance-ID grep, and never by a
single-line one, which returns ZERO in this tree because the macro's argument sits on the next
line. Measured against `master`'s git objects: 249 there, 259 here, removed set EMPTY. All ten
added names carry `GATE-INFOPANEL`, which mints no acceptance ID.
THE REPORT THAT CERTIFIES THIS FIGURE IS THIS LANE TREE'S: `reportCreatedOn
2026.08.28-00.32.37`, `succeeded 259`, `failed 0`, `notRun 0`, 259 entries and zero non-Success.
THAT STAMP IS UTC AND THIS ENTRY'S DATE IS LOCAL.
**[THE THREE EARLIER RUNS OF THIS WAVE ARE QUOTED AS HISTORY AND NONE BACKS THE LIVE FIGURE:
`2026.08.28-00.03.21` at 258/258, before the gate's ordering finding was answered;
`2026.08.28-00.20.54` at 259/259, before two comment corrections; and a 244/244 report at
`2026.08.27-21.18.42` that was NOT this wave's at all -- it was the forecast-card lane's, left in
this slot when the slot was reused, and it made the sweep report four findings that named nothing
in this wave. It was moved aside rather than deleted. They are stamped rather than dropped
because a superseded citation left standing unstamped is the shape this record has been bitten
by.]**
THE FIGURE IS THE LANE'S AND NOT `master`'s, AND THAT IS THE POINT OF SAYING WHICH TREE IT CAME
FROM. `master` still reads 249/249 and is not wrong; the count moves in the merge commit, and it
will be re-measured on the integration tree there rather than carried over from this line.
THE NARROW RE-GATE RETURNED `VERDICT: PASS` WITH ZERO FINDINGS, and that is the verdict for this
lane. THE FULL GATE BEFORE IT RETURNED `VERDICT: BLOCK` ON TWO FINDINGS AND BOTH ARE FIXED --
stated rather than smoothed, because a `PASS` that hides the `BLOCK` it followed misreports how
the work was done. The first finding was in CODE, not prose: a constraint the test lane had
declared unobservable was observable in the fixture it already had, because
`FStratSelectionMachine::DecorateViewModel`'s write to `bDone` is unconditional and destructive
and the fixture owns its model by value. It is now pinned by a clause that asserts the machine's
own post-decoration output rather than a literal, and runs its own reordered control. The second
was five line citations in `decisions.md` that were exact in `master` and wrong the moment this
wave merged, since it inserts some 400 lines above them; they are cited by symbol now.
THE CODE, MODULE-GRAPH AND ASSET PASS CAME FIRST AND CAME BACK CLEAN, per the standing user
ruling that a gate audits those before the prose: the vendored bytes verified by `git hash-object`
across all 32 files rather than by a worktree diff, the module graph re-derived at 15 rows and
five arrows with `StratUI` still free of `Stratocracy`, zero `strat::` calls outside the two
permitted directories on both a single-line and a multiline scan, and no `/Game/` literal outside
the two standing exceptions.)

_Last run 2026-08-27 (SEC 2.11.2'S PERSISTENT HUD HAS A MODEL SIDE, AND THE FAME RATE THAT NO
CODE HAD EVER READ NOW HAS A ROUTE TO A SCREEN. Written by the `coordinator`, whose file this is.
THE PROVENANCE IS THE INTEGRATION TREE: this run was made in `E:/MultiAgent/Stratocracy` on
branch `master`, AFTER the `--no-ff` merge of `feat/persistent-hud`, which itself had been
rebased onto wave 1's merge at `1d4d0e0`. So this figure covers BOTH waves -- wave 1's thirteen
clauses and wave 3's five. The suite is now
**249/249**, every entry Success, zero failed, zero notRun, zero succeededWithWarnings.
THE DELTA OVER `master` IS **+5 AND NOTHING WAS REMOVED OR RENAMED**, by multiline set-difference
on `IMPLEMENT_SIMPLE_AUTOMATION_TEST` -- never by an acceptance-ID grep, and never by a
single-line one, which returns ZERO in this tree because the macro's argument sits on the next
line. Measured against `master` itself rather than against `ee7300c`: 244 there, 249 here, the
five added names listed in this pass's `## NEXT` entry, removed set EMPTY.
**THE FIGURE WAS MEASURED THREE TIMES ON THE WAY HERE AND NEVER PREDICTED, which is the point.**
A rebase is a change and a merge is a change, and a green from before either describes different
bytes -- so each step was rebuilt (`Result: Succeeded`) and re-run rather than reasoned about.
**[STAMPED 2026-08-27 -- THE TWO EARLIER RUNS OF THIS WAVE ARE QUOTED HERE AS HISTORY AND NEITHER
BACKS THE LIVE FIGURE. The lane's own pass was `reportCreatedOn 2026.08.27-21.02.15`, 236/236, run
in `E:/MultiAgent/Strat-wt/slot-2` before wave 1 existed. The REBASED branch's pass was
`reportCreatedOn 2026.08.27-21.44.34`, 249/249, run in that same lane tree after rebasing onto
`1d4d0e0`. Both are superseded by this tree's own run, named below. They are stamped rather than
deleted because a superseded citation left standing unstamped is exactly the shape this record
has been bitten by -- and the sweep refused this entry once, on these two lines, before they
carried this stamp.]**
THE REPORT THAT CERTIFIES THE LIVE FIGURE IS THIS TREE'S: `reportCreatedOn
2026.08.27-21.52.53`, `succeeded 249`, `failed 0`, `notRun 0`, 249 entries and zero non-Success,
read directly by the `coordinator`. THAT STAMP IS UTC AND THIS ENTRY'S DATE IS LOCAL.
THE REBASE WAS RE-GATED NARROWLY AND RETURNED `VERDICT: PASS` WITH ZERO FINDINGS, and that is the
verdict for this merge. Its subject was the one thing a rebase can break behind a green suite:
**three source files were modified by BOTH waves and auto-merged with no conflict** --
`StratViewModel.h`, `StratViewModel.cpp` and `StratPlayerController.cpp`. The gate confirmed both
waves' changes survived intact, that `AStratPlayerController::DecorateForPresentation` still runs
`SelectionMachine -> GuidedOpening -> Hover -> StratDecorateForecast` with ZERO diff against
`master` -- wave 1's ordering constraint, in a file wave 3 also touched -- and that
`FStratViewModel`'s field list is byte-identical to `master`, so `StratViewModelParity` keeps the
subject its clauses assume. Checks the two full gates had already cleared were scoped OUT and
said to be scoped out, on the ground that `git patch-id --stable` equality proves the rebase
moved no source line of either wave's diff.
THE GATE RETURNED `VERDICT: PASS` WITH ZERO FINDINGS ON ITS FIRST ROUND, and that is the verdict
for this pass -- zero in the code, zero in the assets, zero in the module graph, and zero
blocking record defects, across twelve structural checks. It verified the vendored bytes by
`git hash-object` across all 32 files rather than by a worktree diff, re-derived the module graph
at 15 rows and five arrows with `StratUI` still free of `Stratocracy`, and audited the wave's
three design calls on their merits rather than accepting the lane's account of them.
**THE PRIORITY ORDER WAS CHANGED BY USER RULING BEFORE THIS WAVE AND THIS IS THE ARGUMENT FOR
IT:** the gate audits the code, the assets and the module graph FIRST and reports that pass
explicitly, and a record-prose defect blocks only when it is load-bearing. The two waves before
this one cost BLOCK, BLOCK, PASS each with not one finding in the game code across all six
rounds.
THE FALSIFIABILITY PROOF STATES ITS OWN LIMIT, which is the half worth carrying. Five
simultaneous mutations gave **231 green / 5 failed** with the red set exactly the five added
names; the restore was verified by `sha256` against pre-mutation copies. **But every mutation
was INSTRUMENT-SIDE**, because this lane may not edit the code under test even temporarily. That
proves each assertion reachable and sensitive to the quantity it names; it does NOT prove the
clause would catch an arbitrary defect in the shipped function. Said at the point of the claim
rather than left for a reader to infer.)_

**[SUPERSEDED 2026-08-27 BY THE BANNER ABOVE -- the 231/231 below was the figure at the pass that
pinned the hover's tick route, and it is not live in this branch. The count moved by exactly
five.]**
**[SUPERSEDED 2026-08-27 BY THE BANNER ABOVE -- the 244/244 in the segment below, and the
`2026.08.27-21.38.56` report it cites, were the figure and the provenance when wave 1 merged to
`master`. Neither is live on this branch. The count moved by exactly five, and every other word
of that segment still stands.]**

_Last run 2026-08-27 (SEC 2.11.3'S FORECAST CARD HAS A MODEL SIDE, AND THE CENTREPIECE THE GDD
NAMES IS NO LONGER ABSENT. Written by the `coordinator`, whose file this is. THE PROVENANCE IS
THE INTEGRATION TREE AND IT IS STATED FIRST: this run was made in `E:/MultiAgent/Stratocracy` on
branch `master`, AFTER the `--no-ff` merge of `feat/forecast-card`. The suite
is now **244/244**, every entry Success, zero failed, zero notRun, zero succeededWithWarnings.
THE DELTA IS **+13 AND NOTHING WAS REMOVED OR RENAMED**, by set-difference on
`IMPLEMENT_SIMPLE_AUTOMATION_TEST` and never by an acceptance-ID grep, which would be useless here
by construction since twelve of the thirteen share one ID. **AND THE INSTRUMENT ITSELF NEEDED
FIXING BEFORE IT COULD BE TRUSTED: a single-line grep for that macro returns ZERO in this tree,
because the macro's argument sits on the NEXT line.** Zero reads exactly like `no clauses here`
rather than like a broken pattern. The census is multiline and the derived list is asserted
non-empty before the difference is taken; the `coordinator` and the gate derived 231 -> 244
independently and agreed, and the thirteen added names are listed in this pass's `## NEXT` entry.
THE REPORT THAT CERTIFIES THE LIVE FIGURE IS THIS TREE'S, RE-RUN AFTER THE MERGE RATHER THAN
CARRIED OVER FROM THE LANE: `reportCreatedOn 2026.08.27-21.38.56`, `succeeded 244`, `failed 0`,
`notRun 0`, 244 entries and zero non-Success, read directly by the `coordinator`. THAT STAMP IS
UTC AND THIS ENTRY'S DATE IS LOCAL. THE LANE'S OWN RUN, `reportCreatedOn 2026.08.27-21.18.42` in
`E:/MultiAgent/Strat-wt/slot-1` on `feat/forecast-card`, is the figure the lane commit `dca1478`
carried and is quoted here as history, not as this tree's evidence -- a merge is a change, and a
green from before it describes different bytes even when the count does not move.
ALL THIRTEEN WERE PROVED RED-UNDER-MUTATION AND GREEN-WHEN-RESTORED, AND THE MUTATIONS WERE
SUBJECT-SIDE. One combined run over thirteen simultaneous one-line mutations in
`FStratBridge::AttackForecast`, `StratComposeForecastView` and `StratDecorateForecast` gave
**231 green / 13 failed**, and the red set was EXACTLY the thirteen added names with no
pre-existing clause moving. Restored, **244/244**, the restore verified BY CONTENT against copies
held outside the repository -- `sha256` equality on all three files with `CR == LF` at 1757 / 635 /
107, so no line ending was rewritten -- and re-verified independently by the `coordinator` rather
than accepted from the lane.
**WHAT THE COMBINED RUN CANNOT CLAIM IS STATED WHERE THE CLAIM IS MADE.** It proves each assertion
reachable and sensitive to its subject; it does NOT establish per-clause attribution, and two
clauses are over-determined by construction -- `ForecastCardRepeatsTheModulesForecast` reddens
under two of the thirteen mutations and `ForecastDecoratorWritesUnconditionally` under two others.
For those two the honest claim is `red under the mutant set`, not `red under its own mutation`.
THE MUTATION PASS WAS AUTHORIZED IN ADVANCE, IN WRITING, AND THAT IS THE PROCESS RESULT WORTH
CARRYING. Every mutation landed outside the test lane, in files carrying the engineer's
uncommitted work, so `git checkout --` as a restore route would have destroyed the wave.
`strat-test-author` STOPPED AND ESCALATED with the mutation list and pre-mutation digests rather
than acting and declaring the conflict afterwards -- which is exactly what the previous wave's
gate asked for after a brief mandated a probe on a file it also forbade touching. The
`coordinator` verified the digests against the live files, took independent copies outside the
repository, and authorized the pass before it ran.
A DISPATCH INSTRUCTION WAS REFUSED BY THE ENGINEER AND THE ENGINEER WAS RIGHT. The
`coordinator`'s brief told it to bridge `strat::uiResolveForGate` for the HP before -> after
readouts. `StratCombatOutcomeParity.cpp`'s header says that function `has zero production callers
in this tree BY CONSTRUCTION` and `must stay uncalled from production`, because a bridge call
`would make every comparison below a comparison of the bridge with itself`. Complying would have
collapsed `T-UI-01`'s independent oracle while every test stayed green. The subtraction and the
clamp are written out ONCE inside `FStratBridge::AttackForecast` instead, and discharged by a
clause that calls the oracle from `Tests/`, where calling it is legitimate. The gate censused the
tree and confirmed ZERO production code callers survive.
THE GATE RETURNED `VERDICT: PASS` WITH ZERO FINDINGS ON ITS FIRST ROUND, and that is the verdict
for this pass. Zero in the code, zero in the assets, zero in the module graph, and zero blocking
record defects, across twelve structural checks. **THE PRIORITY ORDER WAS CHANGED BY USER RULING
BEFORE THIS WAVE AND THE RESULT IS THE ARGUMENT FOR IT:** the gate was told to audit the code, the
assets and the module graph FIRST and to report that pass explicitly, and that a record-prose
defect blocks only when it is load-bearing. The two waves before this one cost `BLOCK, BLOCK,
PASS` each with not one finding in the game code across all six rounds. Under the new order both
waves this session passed first time.
ITS ONE NOTE, non-gating and recorded so it is not rediscovered: `engine.md` still carries a
CONDITIONAL sentence, `until such a clause exists, the two HP-after fields are the only numbers on
the card that no test compares against anything`, whose condition was met inside this same tree by
`Stratocracy.StratBridge.T-UI-01.ForecastCardHpAfterIsTheOracles`. It is not load-bearing: the
sentence names the condition that retires it, the engineer could not write the file that records
the discharge, and `tests.md` -- which owns clause inventory -- records the discharge.)_

**[SUPERSEDED 2026-08-27 BY THE BANNER ABOVE -- the 231/231 below was the figure at the pass that
pinned the hover's tick route, and it is not live in this branch. The count moved by exactly
thirteen.]**

_Last run 2026-08-27 (THE HOVER'S INPUT ROUTE IS PINNED FOR THE FIRST TIME, AND THE CLAUSE WAS
PROVED IN BOTH DIRECTIONS. Written by the `coordinator`, whose file this is. The suite is now
**231/231**, every entry Success, zero failed, zero notRun, zero succeededWithWarnings. The delta
is **+1 and nothing was removed or renamed**, computed by set-difference on
`IMPLEMENT_SIMPLE_AUTOMATION_TEST` and re-derived independently of the lane's own report: HEAD
carries 230 clause names, the worktree 231, the added name is
`Stratocracy.StratPlay.T-UI-01.TickDrivesTheHoverPath`, and the removed set is EMPTY. THE REPORT
THAT CERTIFIES THE LIVE FIGURE IS THIS TREE'S: `reportCreatedOn 2026.08.27-19.27.46`,
`succeeded 231`, `failed 0`, `notRun 0`, zero succeededWithWarnings, 231 entries and zero
non-Success entries, run in `E:/MultiAgent/Stratocracy` on `master`. THAT STAMP IS UTC AND THIS
ENTRY'S DATE IS LOCAL. **IT IS THE SECOND RUN OF THIS PASS AND THE FIRST ONE IS NOT CITED, which
the sweep is the reason for rather than a preference:** the lane's own run
(`2026.08.27-19.22.02`) predated its last two test-file writes by three minutes, and the REPORT
IDENTITY check refused it as `evidence about a PAST tree, not the current one, whatever its count
happens to say`. Rebuilt and re-ran rather than reasoning about whether the later edits could
have mattered.
WHAT THE CLAUSE PINS AND WHY IT COULD NOT EXIST BEFORE. `T-UI-01.TickDrivesTheHoverPath` plants a
hover on a LIVE spawned controller, ticks it through an `AActor*` handle -- `Tick` is `protected`,
so an `AStratPlayerController*` will not compile -- and asserts the hover is gone, on the
controller's own `GetHoveredHex` AND on a model its own `DecorateForPresentation` wrote. The
planted hex is ENUMERATED off a view model built through a live `FStratBridge` rather than typed,
and the cleared value is a default-constructed `FStratHoverView::HoveredHex`, asked rather than
asserted. The old Enhanced Input route could not be pinned at all; this is the first hover path
any clause in this project has reached.
IT WAS PROVED BY REVERTING WHAT IT PINS, IN BOTH DIRECTIONS. With `Tick`'s body emptied the suite
went **230 passed / 1 failed of 231** and the one failure was this clause, red on six separate
legs. Restored, **231/231**, and the restore was verified BY CONTENT -- `sha256` equality against
the pre-mutation file, a mutant-token census of zero, and the controller absent from
`git status`. **AND EVERY OTHER CLAUSE STAYED GREEN UNDER THE MUTANT**, which is the finding worth
carrying: the new clause is the sole net under that route, the same shape as the phase-6
`bCanEverTick` result.
THE LANE FLAGGED A GUARD DEFECT THAT DOES NOT EXIST, AND IT IS THE SECOND AGENT TO FLAG IT.
`strat_banner_sweep.py` was reported to print `SWEEP FAILED` while returning exit code 0, which
would mean the pre-commit hook gates nothing. Measured HERE on a genuinely failing tree -- this
one, before the figure below was moved -- with the real report and no scratch file and no
redirection: `SWEEP FAILED` and **exit 1**, twice. The engineer reported the identical false
claim earlier the same day and it was disproved the same way. The cause is a shell idiom rather
than the tool: `$?` after a pipe is the LAST command's status, not the script's. The guard is
sound. **IT IS NOW THE THIRD INDEPENDENT REPORT OF THIS PHANTOM DEFECT FROM A THIRD AGENT**, and
the gate's re-derivation this pass is the durable answer: a control run gives `SWEEP CLEAN`/exit
0, a one-line mutation of the live figure gives `SWEEP FAILED`/exit 1, and piping that same
failing script through `tail` or `grep` then reading `$?` gives 0 both times. It costs a full
re-derivation every time it is reported, so it belongs in the dispatch brief rather than only
here.
THE GATE RETURNED `VERDICT: PASS` WITH ZERO FINDINGS across twelve structural checks and eight
audited claims, and that is the verdict for this pass. Its one non-gating debt is a sequencing
one worth stating: the falsifiability probe MUTATED `Source/StratPlay/StratPlayerController.cpp`,
outside the test lane, and declared the conflict AFTER the fact rather than escalating first. The
tree is byte-identical to HEAD there -- the gate proved it with `git hash-object` against the HEAD
blob rather than accepting the lane's own `sha256` -- so nothing needs fixing, and `tests.md`'s
own "not a standing licence" sentence should survive. The brief that mandated a probe on a file it
also forbade touching is the thing to fix next time.)_

**[SUPERSEDED 2026-08-27 BY THE BANNER ABOVE -- the 230/230 in the paragraph below, and the
`2026.08.27-18.05.07` report it cites, were the figure and the provenance at that pass. Neither
is live. The count moved by exactly one, and the entry under `## NEXT` that carried the same
230/230 is stamped where it stands rather than only here.]**

_Last run 2026-08-27 (WAVE 0 LANDS: THE HOVER INPUT SURFACE EXISTS, AND THE TWO SECTIONS THAT
SIT BEHIND IT CAN NOW BE BUILT. Written by the `coordinator`, whose file this is. The suite is
now **230/230**, every entry Success, zero failed, zero notRun, zero succeededWithWarnings. The
delta is **+5 and nothing was removed or renamed**, computed by set-difference on
`IMPLEMENT_SIMPLE_AUTOMATION_TEST` and never by an acceptance-ID grep -- which would be useless
here by construction, since all five new clauses share one ID. THE REPORT THAT CERTIFIES THE LIVE
FIGURE IS THIS TREE'S: `reportCreatedOn 2026.08.27-05.17.45`, `succeeded 230`, `failed 0`,
`notRun 0`, zero succeededWithWarnings, run in `E:/MultiAgent/Stratocracy` on branch `master`
after the merge, and every one of its 230 entries is Success. THAT STAMP IS UTC AND THIS ENTRY'S
DATE IS LOCAL, which is why they can name different days.
**[STAMPED 2026-08-27, LATER THE SAME DAY -- THE REPORT NAMED IN THE SENTENCE ABOVE IS
SUPERSEDED AND THE FIGURE IT CERTIFIES IS NOT. The live report is now
`reportCreatedOn 2026.08.27-18.05.07`, run in this same tree on `master` after the hover route
was replaced: `succeeded 230`, `failed 0`, `notRun 0`, zero succeededWithWarnings. THE COUNT DID
NOT MOVE, WHICH IS EXACTLY WHY THIS STAMP IS NEEDED -- the sweep's SUITE COUNT check compares the
figure and this record has already been bitten once by a provenance that was stale while the
count happened to agree, so a superseded report citation left standing here would be clean and
wrong at the same time. **[AMENDED WITHIN THIS STAMP, SAME DAY, ON TWO GATE FINDINGS, AND
RESTATED FLAT RATHER THAN NESTED -- nesting a correction inside a correction is what let a false
sentence survive above its own retraction in this file's `## NEXT` copy, so both superseded
wordings are QUOTED here and neither is left standing as prose. STRUCK: (1) `the zero-warnings
field is load-bearing in the newer run: it is the independent confirmation that a diagnostic
Warning added and removed mid-pass is gone`; (2) its first replacement, which said the field
`read zero on the 05.17.45 report too, which was run on the INSTRUMENTED tree`. WHAT IS TRUE: no
suite was ever run while the instrument existed, so there is no reading of an instrumented tree in
either direction -- verified by classifying every 2026-08-27 session log, the one instrumented
session having run zero tests and all three automation runs carrying zero `HOVER-DIAG` lines.
That is a cleaner refutation than the one first written here, not a weaker one.]** The field
confirms nothing either way, and `engine.md` gives the construction reason: no automation clause
reaches `OnHover`, and the second log site was put at
`Log` precisely so a diagnostic could not raise a clause warning. The removal is confirmed by the
diff and a token census over the source instead. The correction is written here rather than only where the claim
was repeated, because this stamp is the copy a reader of the banner meets first.]** The entry that
produced the newer run is the topmost one under `## NEXT`.]**
**[STAMPED 2026-08-27, THE MERGE COMMIT: the sentence this paragraph carried before it said the
certifying report `WAS PRODUCED IN E:/MultiAgent/Strat-wt/slot-1 on branch feat/hover-input, NOT
IN THE INTEGRATION TREE: reportCreatedOn 2026.08.27-03.59.15`. That was true when written and is
kept rather than deleted, because it records WHY a lane provenance was correct for one commit:
the wave's C++ was not merged, so no run in this tree could have exercised it. It is not the live
provenance now, and the two runs agree at 230/230 with the same clause name set.]**
WHAT LANDED. `FStratHoverState` -- a plain, world-free, non-reflected struct holding the hovered
hex -- plus a sixth `HoverAction` on `AStratPlayerController` bound `Triggered` and reusing the
one existing cursor-to-hex route, and `FStratViewModel::Hover` carrying the hex to the screen
through the decoration seam the selection machine and guided opening already use. Five clauses
ride `T-UI-01` BY USER RULING, minting no new ID; the boundary is stated in the fixture and in
`tests.md` -- none of them asserts that ID's own forecast-equals-resolution claim, and each pins
something the forecast path needs and cannot produce for itself.
THE GATE TOOK THREE ROUNDS AND EVERY FINDING WAS IN PROSE OR IN A GUARD, NEVER IN THE CODE.
`VERDICT: BLOCK` twice, then `VERDICT: PASS` with zero findings. The first block was five stale
or contradicting comment counts; the second was the census instrument itself -- the command this
record designated as its derived total did not run (no `-r`, exit 2, printing `:0`, which reads
as ZERO NOTES) and, once corrected, counted its own documenting line. It is fixed at the cause:
the only spelling of the search token now lives in this record, OUTSIDE the tree the command
searches. **AND A FALSIFIABILITY PROBE SILENTLY CORRUPTED A SOURCE FILE WHILE PRINTING THE
NUMBERS THAT WERE SUPPOSED TO PROVE IT HAD NOT** -- 17 -> 16 -> 17, exactly what an intact tree
prints, because the probe's pattern spanned a line wrap and ate the opening of a quoted sentence.
`sha256sum` was the only instrument that found it. **A COUNT-BASED INTEGRITY CHECK CANNOT DETECT
A CORRUPTION THAT CHANGES THE COUNT AND CHANGES IT BACK**: when the mutation is pattern-driven,
the verification must be content-addressed. The gate's own corruption hunt then cleared the full
diff positively rather than by absence of impression -- zero code lines removed across the three
tracked files, all 22 quoted fragments verifying verbatim against the base blob, balanced comment
and quote structure, and the damaged site restored in full.
WHAT IS NOT PINNED, AND IT IS THE INPUT HALF. Four properties are declared unpinnable rather than
faked: that `HoverAction` is bound at all and to `Triggered`, that a mouse move produces a hover,
that `UpdateHoverFromCursor` resolves a hex, and that `ApplyHoverChange` refreshes. The first
needs an asset default that cannot be set until this merges and the editor relinks; the second
needs a human at the mouse, since nothing in this project's automation reaches `UPlayerInput`.
`IA_Hover` and its `IMC_Selection` row ARE authored and verified from the package bytes; the
Blueprint default is not.
**[STAMPED 2026-08-27, LATER THE SAME DAY -- THE LAST SENTENCE ABOVE IS COMPOUND AND ONLY ITS
SECOND CLAUSE MOVES. `IA_Hover` and its `IMC_Selection` row ARE authored and verified from the
package bytes: still true, untouched. `the Blueprint default is not`: no longer true. The
sentence before it is likewise half-true. The Blueprint default IS now authored and verified from the package
bytes, on the same evidence standard as the other two assets; `cannot be set until this merges
and the editor relinks` describes a condition that has since been met, not a standing one. WHAT
SURVIVES UNCHANGED is that the BINDING is still unpinned. What was discharged is its stated
REASON -- the asset default that could not be set -- not its conclusion, and the human at the
mouse is still owed. Note the properties are not being renumbered here: `the second` in the
sentence above is *that a mouse move produces a hover*, which this pass did not touch either. Stamped here as
well as at the foot of this banner because a discharge recorded in one place and contradicted in
another asserts both. See the stamp below and `content.md`'s topmost entry.]**
THE ASSET BATCH LANDS IN THIS COMMIT, and what it could not do is measured rather than assumed.
`IA_Hover` is an `Axis2D` action -- the one deliberate difference from the five existing actions,
which are all `Boolean` -- and it carries no triggers and no modifiers, matching them. It is row
8 of `IMC_Selection` on `Mouse2D`, with the seven existing rows unchanged and in order. Both were
verified from the SAVED package bytes with a negative control, not from the authoring call's
return value -- which lied in both directions in this session: a malformed value returned `true`
with a no-op warning, and the correct value returned `[FAIL]` AND APPLIED. **THE BLUEPRINT
DEFAULT COULD NOT BE SET AND THE ABSENCE WAS MEASURED WITH A CONTROL**: `class_properties` on the
NATIVE class path returned 91 entries, of which the `Action|Mapping|Hover` filter returned exactly
seven -- the five existing actions plus the mapping context and its priority, and no
`HoverAction`. The same call on the BLUEPRINT class path answered `[OK] -> 0 entries`, which is
indistinguishable from absence and is the recorded trap. The editor was serving a DLL built
before this wave; setting that default needs a relinked editor and is the first thing the next
session owes.)_

**[STAMPED 2026-08-27, LATER THE SAME DAY -- THE OWED ITEM IN THE LAST SENTENCE ABOVE IS
DISCHARGED. **THIS IS NOT THE ONLY SENTENCE IN THIS BANNER THAT MOVES, and the earlier wording
here said it was** -- a `strat-integration-reviewer` gate blocked on exactly that, because the
`WHAT IS NOT PINNED` paragraph earlier in this same banner still asserted `the Blueprint default
is not` and this stamp's own phrasing read as an instruction to leave it standing. That paragraph
is named rather than counted to: the in-place stamp it now carries changed the distance between
the two, which is what a line count into an append-above document always does. That paragraph now
carries its own stamp in place. NO SUITE WAS RUN AND NO SUITE FIGURE
MOVES: the live figure is the one that banner cites, and this pass touched one `.uasset` and
two record files -- no source, no test, no config. **THIS PASS HAS BEEN THROUGH THE
`strat-integration-reviewer` GATE** -- the sentence here previously said it had not, which was
true when this stamp was first written and was left standing when the gate-blocked sentence was
inserted above it. That is the add-only correction defect reproduced INSIDE the fix for itself,
and the gate's second round blocked on it. **NO GATE ROUND HAS FOUND ANYTHING IN THE ASSET WORK
OR IN THE `.uasset`** -- every finding returned so far has been in this record, and the package
sha256 is unchanged across all of them. No running tally of rounds is written here on purpose: a
count of a thing still growing goes stale inside the document that states it. **THE GATE
RETURNED `VERDICT: PASS` WITH ZERO FINDINGS ON THE THIRD ROUND, over this uncommitted tree, and
that is the verdict for this pass.** It is recorded here because `global.md` is the only file
that may state one. **THIS SENTENCE IS THE ONLY CHANGE MADE AFTER THAT PASS**, and it is stated
so a reader knows exactly what the PASS covered: the tree as it stood with this stamp reading
`STATES NO PHASE VERDICT OF ITS OWN`, plus this recording of the result. Two cosmetic residuals
the gate raised and did not block on are deliberately left rather than re-opened for a fourth
round -- two spliced lines here run past the file's wrap, and the `the live figure is the one
that banner cites` phrasing further down this stamp dangles, since the stamp sits inside the
banner it points at. Both are wording no guard reads, and the second predates this pass. The editor was relaunched on the rebuilt integration tree and the
relink was witnessed by the subject itself -- `class_properties` on the NATIVE
`AStratPlayerController` path went from 91 entries / seven filtered to 92 / eight, the eighth
being `HoverAction`. `BP_StratPlayerController.HoverAction` now points at `IA_Hover`, verified
from the SAVED package bytes against a wired-action control rather than from the `set` call's
return value. The full evidence is `content.md`'s topmost entry, which is the file that owns
this fact class; the acting and the file write are attributed separately there, to
`CLAUDE.md`'s editor-driver clause and to `content.md`'s own fallback condition. **WHAT IS NOW
FIRST IN THE OWED LIST IS ITEM 2, THE HUMAN AT THE MOUSE** -- four properties stay unpinnable
by anything in this project's automation, and the editor is left OPEN for that playtest.]**

**[STAMPED 2026-08-27, LATER STILL -- THAT PLAYTEST HAS NOW RUN, AND ITS RESULT IS THE
TOPMOST ENTRY UNDER `## NEXT`. The sentence above is kept as written because it was true
when written: at that moment the playtest was owed and the editor was held open for it.
What it should no longer be read as is a statement of what is owed NOW. The playtest found
that Enhanced Input delivers no `Triggered` event for `IA_Hover` from a mouse move, so of
the four properties that paragraph calls unpinnable, one is not merely unpinned but
MEASURED FAILING, and the remaining three cannot be exercised by a mouse until it is
fixed. The fix is `strat-gameplay-engineer`'s lane. Nothing in the commit that set the
Blueprint default is falsified by this.]**

**[SUPERSEDED 2026-08-27 BY THE BANNER ABOVE -- the 225/225 in the paragraph below was the figure
at that pass, and the report it cites is that pass's. Neither is the live figure.]**
_Last run 2026-08-26 (NINE GOVERNANCE ITEMS CLOSE AND NOT ONE OF THEM WAS GAMEPLAY: A GUARD
LEARNS TO READ PROVENANCE, A CLAUSE STOPS TYPING ITS OWN REFERENCE DATA, A LANE RULE GETS THE
WORD IT HAS BEEN MISSING FOR FOUR PASSES, AND A FINDING THIS RECORD CARRIED IN TWO PLACES TURNS
OUT TO HAVE BEEN THE INSTRUMENT'S FAULT. Written by the `coordinator`, whose file this is.
**THE SUITE COUNT DID NOT MOVE, AND THAT IS THE INTENDED RESULT RATHER THAN AN OMISSION**: the
suite is now **225/225**, every entry Success, zero failed, zero notRun, zero
succeededWithWarnings. Today's only clause change REWROTE an existing clause and minted nothing,
so the clause NAME SET IS IDENTICAL across HEAD, the worktree and the report -- 225 on all three
sides, set-difference empty in every direction, computed by set-difference on
`IMPLEMENT_SIMPLE_AUTOMATION_TEST` and never by an acceptance-ID grep. THE REPORT THAT CERTIFIES
THE LIVE FIGURE IS THIS TREE'S: `reportCreatedOn 2026.08.26-15.21.53`, `succeeded 225`,
`failed 0`, `notRun 0`, run in `E:/MultiAgent/Stratocracy` on branch `master`. THAT STAMP IS UTC
AND THIS ENTRY'S DATE IS LOCAL, which is why they can name different days.
THE SWEEP CAN NOW SEE A PROVENANCE SENTENCE, WHICH IS THE ITEM `78ea508` FILED AGAINST ITSELF.
`strat_banner_sweep.py` gains a fifth check, REPORT PROVENANCE, in two parts: (a) an unstamped
`reportCreatedOn` cited in the live banner must be the report the sweep actually opened, and (b)
an unstamped sentence saying a figure was PRODUCED in a named path on a named branch must name
the tree the sweep is running in. Both sides are DERIVED -- the repo root from the script's own
location, the branch from `git rev-parse` -- and neither is typed, so the same guard is correct
in a lane tree and in `master` for opposite reasons. FALSIFIABILITY WAS MEASURED WITH OLD CODE ON
BOTH SIDES, RE-RUN INDEPENDENTLY BY THE `coordinator`, AND THEN PARTLY REFUTED BY THE GATE -- the
corrected statement is the one that stands, AND IT IS THE ONE FURTHER DOWN THIS SEGMENT rather
than this sentence -- the discriminating control is the PRE-REPAIR build, not the `HEAD` build,
for the reason given there, and nothing in this paragraph should be read as claiming otherwise.
What was run here: over `d6492c9`'s real defective bytes with `--no-tree`, the `HEAD` build
printed `SWEEP CLEAN` at exit 0 and the new build failed at exit 1 naming the defect. THAT PAIR
IS DOCUMENT-ONLY BY NECESSITY -- with the tree check active it is not discriminating at all, and
the first draft of this paragraph wrongly said it was: the `HEAD` build exits 1 there too, on
`SUITE COUNT AGREEMENT`, because those bytes claim 224/224 against a tree now at 225, which has
nothing to do with provenance. The check's falsifiability is
unaffected; the overclaim was in how it was reported.
THE TRAP THAT PRODUCED THE OVERCLAIM IS WORTH MORE THAN THE OVERCLAIM. The `coordinator` extracted
`HEAD:Tools/architect/strat_banner_sweep.py` to a SCRATCH PATH and ran it there, which is the right
way to keep old code on both sides -- but `_REPO` is derived from the SCRIPT'S OWN LOCATION, two
directories above it, so the extracted copy looked for `Saved/AutomationReport/` and `Source/`
under the scratch directory rather than under the repo, found neither,
skipped both tree checks and printed `SWEEP CLEAN` HAVING COMPARED NOTHING AGAINST THE TREE. That
is this repo's own recorded "a guard that cannot find its subject prints clean" shape, reproduced
by the very control meant to prove a guard can fail. **HOW TO REPEAT IT WITHOUT MANUFACTURING AN
INERT CONTROL, and the first draft of this instruction was itself insufficient -- the reviewer
measured that too.** `--no-tree` ALONE IS NOT ENOUGH: run from a scratch directory it still prints
`SWEEP CLEAN` at exit 0, because part (b) also skips when `git rev-parse` fails outside a
repository, so the flag removes one blind spot and leaves another. What actually works is running
the REPO'S OWN script -- its location is what `_REPO` and the branch lookup both derive from --
with the extracted documents passed as arguments, and `--no-tree` on both sides so the comparison
is document-only.
AND THE OLD SIDE NEEDS THE SAME TREATMENT, WHICH THE FIRST TWO DRAFTS OF THIS RECIPE BOTH MISSED.
A differential has TWO builds, and "run the repo's own script" can only ever be the new one --
so as written this recipe could not produce its own control, and the only route it left was the
scratch-path shape it had just warned against. `_REPO` is `join(_HERE, pardir, pardir)`, so THE
OLD BUILD MUST LIVE UNDER `<repo>/Tools/<any-dir>/` TO DERIVE THE REPO AT ALL. MEASURED, not
supposed: the pre-repair build written to `Tools/_oldbuild_probe/` derived
`E:\MultiAgent\Stratocracy`, printed `SWEEP CLEAN` at exit 0 over the mutated banner, and the
shipped script exited 1 on the same bytes -- a discriminating pair at last, with no `rev-parse`
skip note on either side. The probe directory was removed afterwards.
AND CHOOSE AN OLD BUILD THAT CONTAINS THE SUBJECT. `HEAD` does not: it has ZERO occurrences of
`check_report_provenance`. Construct the control by copying the CURRENT script and reverting the
ONE line under test -- old enough to differ, new enough to contain the subject.
AND READ THE RUN'S OWN NOTES RATHER THAN ONLY ITS VERDICT LINE: every one of these skips
announces itself in the notes, and every wrong conclusion drawn today came from reading the
verdict and not the notes above it. `--self-test` reports ALL FIXTURES CORRECT.
THE CHECK CAUGHT REAL DRIFT ON ITS FIRST DAY, and it was ours: the banner segment below cited
`reportCreatedOn 2026.08.26-03.40.42` while the report on disk had moved to `-15.21.53`, because
the suite was re-run mid-pass. That citation is now history by position, superseded by this
segment, and no live claim rests on it.
THE GATE FOUND THE CHECK INERT ON ITS OWN PRIMARY SUBJECT, AND THAT HOLE IS NOW CLOSED. Part (b)
shared `is_stamped`'s general marker set, which counts a nearby `reportCreatedOn` citation as a
stamp. That is the right signal for part (a), whose subject IS a report citation, and the wrong
one for a tree-and-branch claim -- so the citation was LAUNDERING an adjacent bad claim. The
reviewer measured it by mutating this very banner's `run in ... on branch ...` to name a foreign
lane tree AND adding the `produced` verb: `SWEEP CLEAN`, exit 0.
**[THREE CLAIMS IN THE FIRST DRAFT OF THIS PARAGRAPH WERE WRONG AND THE GATE BLOCKED ON ALL
THREE. Corrected in place below rather than appended, because a correction that only ADDS leaves
both readings standing -- a lesson this same banner has now supplied twice in one day. What the
draft said: that the sentence sat "roughly 178 characters" from its citation, that "roughly
thirty characters of prose either way" decided the outcome, and that the `HEAD` build was the
control that witnessed the repair.]**
THE DISTANCE FIGURE DEPENDS ENTIRELY ON WHERE YOU ANCHOR, AND 178 MATCHED NO CONVENTION.
THE ONE CONVENTION STATED AND REPRODUCED IS `match.start()` TO `match.start()` ON THE CHECK'S OWN
TWO REGEXES, and it is now written into the script beside the figures so the next reader cannot
repeat this: the mutated live banner measures **91** characters from its citation, and
`78ea508`'s real defect measures **249**, against a 220-character window. The reviewer and
`strat-data-steward` derived that pair independently and agree exactly; a `coordinator` reading
anchored on the literal sentence instead came out about a dozen characters lower and IS NOT
RESTATED HERE, because a figure one lane could not reproduce does not belong in the record as a
measurement. What every anchor agrees on is the part that matters: the live banner sat FAR INSIDE
the exemption window -- 129 characters to spare -- and `78ea508`'s defect sat OUTSIDE it, by 29.
So the "near miss" framing was false in both directions: the hole was not marginal, and the one
real defect this check was built for was flagged by MARGIN rather than by design. That is a worse
finding than the draft's, not a milder one.
THE CONTROL NAMED IN THE DRAFT WAS TRUE AND EVIDENTIALLY EMPTY, WHICH IS THE SUBTLER FAILURE.
`git show HEAD:Tools/architect/strat_banner_sweep.py` contains **ZERO** occurrences of
`check_report_provenance` -- check 5 is entirely uncommitted, so the `HEAD` build has no
provenance check in it at all. It prints `SWEEP CLEAN` over the mutant because it cannot look,
not because a marker set laundered anything, and the word "still" asserted a continuity that
never existed. THE DISCRIMINATING CONTROL IS THE PRE-REPAIR BUILD -- check 5 present, general
marker set -- and the reviewer ran it: `SWEEP CLEAN` at exit 0 over the mutant, against the
repaired build's exit 1 on the provenance sentence itself. (The first draft cited a LINE NUMBER
here; it named a scratch mutant of an earlier draft and identifies nothing now -- this file's own
recorded rule is to cite the sentence, never a `file:NNN` written during the diff that moves it.)
This is the same class of overclaim retracted a few
lines above, and the lesson is narrower than "use old code on both sides": THE CONTROL MUST BE OLD
ENOUGH TO DIFFER AND NEW ENOUGH TO CONTAIN THE SUBJECT.
Part (b) now has its OWN marker set, supersession annotations only, with the `reportCreatedOn`
marker dropped; PART (a) IS UNCHANGED. **[STAMPED 2026-08-26 -- THE BEHAVIOURAL PROOF OFFERED
HERE NO LONGER REPRODUCES, AND THE THING THAT DESTROYED IT WAS THIS PARAGRAPH'S OWN FILE GROWING.
It read: "that was PROVED BY BEHAVIOUR, not by reading the call site -- the reviewer built a
leaked variant passing the narrow markers into part (a) as well and it FAILS on this file's
legitimate historical citation of `reportCreatedOn 2026.08.26-03.40.42`, where the shipped build
passes." TRUE WHEN WRITTEN. Then today's segment was prepended, `current_banner_end` moved PAST the
citation the proof rested on, leaving that subject BELOW the live banner, where part (a) skips it
under BOTH builds before any marker is consulted. **[AND THIS STAMP'S OWN FIRST DRAFT NAMED TWO
LINE NUMBERS FOR THAT BOUNDARY AND SUBJECT; BOTH WERE STALE BEFORE THE GATE READ THEM, invalidated
by prose added to this same segment after the stamp was written -- the stamp's own meta-lesson
happening to the stamp, within the hour. The figures are removed rather than refreshed: a line
number in a file that is still growing is the one citation this record's own rule forbids, and
refreshing it would only reset the clock on the same defect.]** The conclusion is unchanged and
does not depend on the numbers: the subject sits below the live banner and is skipped. The
`-03.40.42` citations still INSIDE the banner cannot discriminate either -- not merely because
they sit in stamped blocks, but because EVERY citation in this segment does, which is the deeper
defect recorded below. The differential is gone and is stamped rather than replaced, because
manufacturing a fresh subject to rescue a claim is how a proof becomes a formality -- and a fresh
subject built today would be exempted by the same paragraph, giving a green differential over an
inert check. (That was written while part (a) WAS inert; it has since been repaired, and the
differential is reconstructible again -- see the account below.)
WHAT STILL STANDS: part (a)'s call site passes the FULL marker set and part (b)'s passes the
narrow one -- that is inspection, and it is now the only evidence offered. THE SEVENTH INSTANCE,
and the one worth carrying: the six before it were undone by CODE. This one was undone by PROSE
ADDED TO THE VERY FILE THAT SUPPLIED THE TEST SUBJECT. A record that measures itself moves its own
subject every time it grows.]**
THE FIXTURE COVERAGE GAP WAS REAL AND IS NOW CLOSED, AND THE PARAGRAPH THAT USED TO SIT HERE
SAID OTHERWISE FOR ONE GATE TOO LONG. **[CORRECTION, 2026-08-26: the draft here said the missing
fixture was "filed rather than claimed". It was BUILT in the same pass, and this sentence was
left standing beside its own correction -- the fifth rewrite of this banner today and the exact
failure mode the fourth was written to hunt. The correction reached `data.md` and the script and
not this file. Rewritten in place rather than appended.]**
THE GAP, AS MEASURED. Neither of the first two new fixtures was a genuinely STAMPED provenance
sentence: `_GOOD_PROVENANCE_NEAR_CITATION` is a TRUE tree claim, and the pre-existing
`_GOOD_PROVENANCE` passed for a reason its stamp did not supply. **[CORRECTION, 2026-08-26: this
paragraph first blamed the fixture's own `"It read:"` for tripping the quoted-figure exclusion.
THAT CAUSE IS FALSE and the true one is worse. `_QUOTED_FIGURE_RE` alternates on `read ` WITH A
TRAILING SPACE and cannot match `read:` at all -- verified directly. Corrected in place.]**
THE REAL CAUSE WAS A FUNCTIONAL DEFECT IN THE CHECK, NOT A QUIRK OF THE FIXTURE, AND IT IS THE
MOST SERIOUS THING THIS PASS FOUND. `_PROVENANCE_PATH_BRANCH_RE` carried the LITERAL `on branch`,
which is not wrap-aware, and `_GOOD_PROVENANCE` WRAPS EXACTLY THERE. So the regex found **ZERO**
matches in that fixture: part (b) never saw the sentence, never reached the marker set, never
reached the exclusion, never reached anything. The fixture was a VACUOUS PASS -- green for
grading nothing -- and the self-test case reading "the same tree/branch sentence, stamped, ...
PASSES" was passing because nothing was graded.
WHY THAT IS A GUARD DEFECT AND NOT A FIXTURE DEFECT. This record's banner prose wraps at about 95
columns, so A REAL LIVE PROVENANCE CLAIM COULD BE HIDDEN FROM THE GUARD BY ORDINARY TYPOGRAPHY --
nothing about it would look wrong, and the sweep would print `SWEEP CLEAN`.
`_BAD_PROVENANCE_NEAR_CITATION` fired only by LUCK: its wrap falls AFTER `branch`, where the
existing `\s*` absorbs it. A guard defeated by where a line happens to break is not a guard.
Found by `strat-integration-reviewer` on the fifth gate of the day, confirmed independently by
the `coordinator` -- 0 matches before, 1 after -- and FIXED: the pattern is now `on\s+branch`,
`_GOOD_PROVENANCE` is no longer vacuous, and `_BAD_PROVENANCE_TREE_WRAPPED` pins the wrapped
shape directionally, failing on the fixed build and passing on the pre-fix one. The same
literal-space defect was found and fixed in `_QUOTED_FIGURE_RE`'s `would have`.
AND THE FIX ITSELF IS NOW PINNED, WHICH MATTERS MORE THAN THE FIX. `check_self_test` carries a
pattern-level regression asserting that the shipped regex matches a wrapped `on\nbranch` sentence
while the literal pre-fix pattern does not, ON THE IDENTICAL TEXT. Its falsifiability was
MEASURED by the `coordinator`, not argued: reverting the one regex to the literal `on branch`
turns `--self-test` red at exit 1. Anyone who re-introduces the defect breaks the build, which is
the difference between a bug that was fixed and a bug that cannot come back.
WHAT CLOSED IT. `_GOOD_PROVENANCE_TREE_STAMP_ONLY` -- a wrong-tree provenance sentence carrying a
real stamp and NO reporting verb, so the stamp is the only thing that can spare it. The reviewer
re-ran the directional pair rather than accepting it: under an over-fire build honouring no stamp
at all, the new fixture FAILED while `_GOOD_PROVENANCE` stayed green -- the same run that proved
the old fixture was testing nothing.
**[STAMPED 2026-08-26: THE SENTENCE ABOVE DESCRIBES THE TREE AS IT STOOD BEFORE THE WRAP FIX AND
IS NO LONGER THE CURRENT BEHAVIOUR. Re-measured by the `coordinator` after the regex was made
wrap-tolerant: BOTH fixtures now exit 0 on the shipped build and exit 1 under over-fire.
`_GOOD_PROVENANCE` staying green was the SYMPTOM of the vacuity, so closing the vacuity had to
change it -- its flipping is the confirmation that it is now genuinely stamp-dependent, not a
regression.]**
THIS BANNER HAS REPEATEDLY ASSERTED SOMETHING A LATER FIX IN THE SAME PASS MADE FALSE, and the
PATTERN is worth more than any single instance of it. **[The first draft here carried a RUNNING
COUNT -- "the sixth time today". More instances were found after it was written, so the count was
stale on arrival, which is the very defect it was describing. A running tally in prose is itself
a claim that goes stale; it is removed rather than incremented, because incrementing it just
re-arms the same trap.
AND THE RULE NEEDS THE DISTINCTION IT FIRST OMITTED, because as first written it forbade something
this segment then did twice and the gate was right to say so. A RUNNING CARDINAL -- "the Nth time
so far" -- is a claim about a set that is still growing, and every new member falsifies it. AN
ORDINAL NAMING ONE INSTANCE -- "the seventh instance", meaning THAT one -- is a label on a fixed
thing and stays true however many follow it. The cardinal is removed; the ordinals stay, and are
what the entries below use.]** Each claim was TRUE WHEN WRITTEN and was invalidated by work that came
after it, in the same uncommitted tree, often within the hour. A record written DURING a pass is
not describing a finished thing, and every measurement in it is implicitly stamped with the tree
that produced it. That is precisely what check 5 exists to catch for REPORTS -- and nothing
catches it for PROSE.
THE SHARPEST INSTANCE IS NOT A STALE SENTENCE AT ALL: THIS SEGMENT'S OWN ACCRETING STAMPS TURNED
CHECK 5 PART (a) OFF OVER THE DOCUMENT IT GOVERNS. Found by `strat-integration-reviewer` on the
seventh gate, reproduced independently by the `coordinator` with a control before anything was
changed. Falsifying the live banner's OWN certifying `reportCreatedOn` gave `SWEEP CLEAN` at exit
0; the IDENTICAL mutation applied to `HEAD`'s banner, run by the SAME shipped script, exited 1. So
the check could fire yesterday and could not fire today, and the only thing that changed was this
paragraph.
THE MECHANISM, AND IT INDICTS THE SCOPE RATHER THAN THE PROSE. Part (a) scoped to the whole
PARAGRAPH, and this segment is ONE unbroken paragraph carrying several `[STAMPED` markers, several
`CORRECTION,` markers and many separate `reportCreatedOn` citations -- counts deliberately not
given, for the reason stamped below. Masking the citation under test still left other markers to
exempt it, and narrowing to the provenance marker set would not have helped:
`[STAMPED` and `CORRECTION,` live in both. **THE GUARD WAS DISABLED BY THE ACT OF DOCUMENTING ITS
OWN REPAIR** -- every correction written today added a stamp marker to this paragraph and widened
the exemption a little further. A guard scoped to "the whole paragraph", in a record whose standing
convention is STAMP AND NEVER DELETE, will always go inert as its subject accretes stamps. That is
not a paragraph that grew too long; it is a scope that cannot survive this record's own practice.
The script's own docstring had asserted the fallback was safe because the entry carried "exactly ONE
`reportCreatedOn`" -- true of `78ea508`'s entry, false of this one, and now stamped as such.
HOW IT WAS FIXED, AND THE CHOICE THAT NEARLY WENT WRONG. Part (a) now takes a 400-character window,
sized from two measurements rather than picked. That alone left the real record RED at one site --
an honest sentence saying the segment below CITED a superseded stamp -- and `strat-data-steward`
ESCALATED rather than widen the window to swallow it or bracket the prose to silence it. Both of
those would have been the wrong repair, and the second is indistinguishable from editing the record
to make the guard pass. THE ACTUAL DEFECT WAS AN ASYMMETRY: check 1 has always exempted a figure
that is REPORTED rather than ASSERTED, on the recorded reasoning that failing an honest account of a
past miscount trains a maintainer to stop running the sweep -- and part (a) did not apply it, while
needing it MORE, since its subject IS a report citation and a record documenting its own drift is
made of sentences quoting stale stamps.
AND THE EXEMPTION NEARLY SHIPPED VACUOUS, WHICH IS THE MORNING'S DEFECT ATTEMPTING A THIRD RUN.
`_QUOTED_FIGURE_RE` contained no `cite`-family verb at all. The flagged sentence was exempted by the
word `reading` sitting in a NEIGHBOURING clause -- green for a reason the sentence never supplied,
and one paragraph's edit away from flipping. The citation family was added to the exclusion so the
exemption fires on the actual reporting verb -- see below for the pattern finally chosen and why it
is wider than the first attempt -- and the isolation differential was run BOTH ways: with `cited`
removed and
`reading` left in place the site FIRES AGAIN, which proves `cited` is doing the work. A prediction
of the `coordinator`'s was refuted in the process -- `reading` never reached the citation anyway,
because `quoting_window` cuts at the sentence boundary between them, so the mechanism is STRICTER
than the raw distance suggested. The steward reported the measurement instead of confirming the
guess. The same pass found and removed an accidental `cited` inside the new fixture's own filler,
which would have made the pin for this very defect vacuous.
WHAT IS PINNED NOW, ALL MEASURED IN THIS TREE BY BOTH THE STEWARD AND THE `coordinator`:
falsifying the live banner's certifying stamp EXITS 1; the unmodified record sweeps CLEAN at exit 0;
removing `cited` re-arms the finding; `HEAD`'s control still exits 1; and `--self-test` is green
with every prior verdict unchanged. `_BAD_PROVENANCE_LONG_PARAGRAPH` fails on the fixed build and
passes on the paragraph-scoped one, so the scope defect cannot silently return.
THE EXCLUSION'S PATTERN IS AN EXPLICIT ALTERNATION OVER THE SIX CITATION INFLECTIONS, AND GETTING
THERE TOOK THREE TRIES, EACH ONE CORRECTING THE LAST BY MEASUREMENT. First `cite\w*`, which the
gate measured CANNOT reach `citing` or `citation` -- forms at least as natural as `cited` beside a
stale stamp. Then the open `cit` prefix, which reached them but also reached any word starting
those three letters. **THAT ONE WAS ACCEPTED FOR A DAY AS A "FORWARD RISK", AND THE GATE PROVED IT
WAS NOT FORWARD AT ALL: THE PARAGRAPH WRITTEN HERE TO DOCUMENT THE RISK HAD ITSELF PUT TWO SUCH
WORDS INTO THIS FILE, falsifying the very census that justified accepting it. The re-measure
trigger written into that comment had already fired, silently, in the commit that authored it.**
Nothing broke -- both words landed outside any figure's quoting window -- but a reader checking the
premise would have found it false and been unable to tell a drifting guard from a growing record.
THE TIGHTENING WAS FREE, WHICH IS WHY ACCEPTING THE TRADE WAS THE WRONG CALL AND NOT MERELY AN
UNLUCKY ONE. **THE BYTE-IDENTITY CLAIM IS ABOUT THE RECORD, AND THE FIRST DRAFT EXTENDED IT TO THE
SELF-TEST AS WELL, WHICH IS FALSE AND CONTRADICTED A SENTENCE TEN LINES BELOW IT.** Corrected in
place. On the RECORD -- all six `state/` files, `--explain --no-tree`, files passed explicitly --
the alternation and the open prefix classify BYTE-IDENTICALLY: 29 claims both sides, zero
reclassifications, derived three times over by `strat-data-steward`, the gate and the
`coordinator`. On the SELF-TEST they deliberately DIFFER, and that difference is the entire point
of the pin: swapping the alternation for the open prefix turns `--self-test` red at exit 1. Saying
both corpora were identical asserted the pin could not tell the patterns apart -- which is to say
it asserted the vacuity this pass had just repaired. Six inflections of one lemma is a CLOSED SET;
a three-letter prefix is an open word class, and it reaches none of the unrelated `cit`-initial
words. This segment's own rule says a repairable bug never belongs on a limits list, and this was
repairable by an alternation.
AND THE MEASUREMENT THAT CAUGHT THE CONTRADICTION NEARLY LIED THE OTHER WAY -- THE LOCATION TRAP
AGAIN, AND IT CAUGHT THE `coordinator` A SECOND TIME AFTER THE `coordinator` HAD RECORDED IT.
**[A RUNNING CARDINAL STOOD HERE -- "for the fourth time today" -- AND IS REMOVED UNDER THIS
SEGMENT'S OWN RULE, which it sat seventy-six lines below and broke. It was both stale-by-growth
and UNDERIVABLE: nothing in this record enumerates the occurrences, so no reader could check it,
which this segment elsewhere calls the worse half. The recurrence is what matters and it is stated
without a tally.]** Checking
the record half with a mutated copy of the script placed under `Tools/architect/_v/` reported
`DIFFERS` -- because `STATE_DIR` derives from the SCRIPT'S OWN DIRECTORY, so the copy swept
`Tools/architect/_v/state/`, found nothing, and printed `Banner sweep: NOTHING` with ZERO claims.
The comparison was a full sweep against an empty one. Passing the six files explicitly gives the
byte-identical result above. A probe of this script that is not told which files to read will
answer confidently about a corpus it never opened, and the verdict line alone will not say so.
AND THE PIN FOR IT WAS VACUOUS ON ARRIVAL -- THE FOURTH SUCH FIXTURE TODAY. The regression meant to
hold the pattern in place hand-typed BOTH patterns as fresh literals, so reverting the shipped
regex did not move it: it asserted a copy of its own subject rather than the subject. Rewritten to
assert against the live `_QUOTED_FIGURE_RE` object, and its falsifiability MEASURED by the
`coordinator` rather than argued -- reverting to the open prefix now turns `--self-test` red at
exit 1. THE COMMENT BESIDE IT NO LONGER *DEPENDS* ON ANYTHING ABOUT THE RECORD'S VOCABULARY, which
is the accurate word and not the one first written here: it still MENTIONS the record in passing,
and the gate was right that "claims nothing" over-stated it. What matters is that its argument now
rests on what the PATTERN covers -- a fact about the code, which no one can falsify by writing a
word into the record. That is the whole lesson of the last three gates in one line: a claim about a
growing document goes stale; a claim about the code does not.
AND THE FIX WAS PROVED AGAINST THE EXACT ACT THAT KILLED ITS PREDECESSOR, which is the only test
that really settles it. Writing the account above grew this segment well past the size and marker
density that switched the OLD scope off -- more of every marker that mattered, in the same single
paragraph -- and the falsified-certifying-stamp mutation STILL EXITS 1 while the unmodified record
STILL SWEEPS CLEAN at exit 0. Documenting the repair no longer disables the thing repaired, and
that -- not the window size -- is what says the scope is right this time.
**[NO FIGURES ARE GIVEN FOR THAT SIZE, DELIBERATELY, AND THE FIRST DRAFT GAVE FIVE OF THEM. Every
one was off by one before the gate read it, because writing the sentence containing them added the
markers it was counting. That is the NINTH instance of the day's pattern, sitting inside the
account of the eighth, and predicted before it was found. A COUNT OF A GROWING THING, WRITTEN
INSIDE THAT THING, IS STALE THE MOMENT IT IS WRITTEN -- and refreshing it only resets the clock, so
the figures are removed rather than corrected. Anyone who wants them can derive them; the sweep
does exactly that on every run, which is the point of having a guard instead of a sentence.]** `data.md` holds the steward's own evidence and
this banner does not restate it. The hole had survived an entire green self-test because not one
fixture in it put a bad tree-claim next to a legitimate citation. **[A FIXTURE COUNT STOOD HERE
AND IS REMOVED, NOT REFRESHED: the gate could not reconcile it with any instrument -- the build
that carried the hole prints a different number than the current one, and neither matches what
was written. It was unverifiable rather than merely stale, which is worse, and the same rule
applies as to every other count in this segment.]**
WHERE `178` PROBABLY CAME FROM, offered as the reviewer's hypothesis and not as a measurement of
mine: by the same convention, `78ea508`'s FIXED bytes put the tree sentence about 177 characters
from the nearest citation. If that is right, the retracted figure was a reading of the REPAIRED
record rather than the defective one -- not a bad anchor on the right text, but the right anchor
on the wrong text, which is the harder mistake to notice.
WHAT THE NEW CHECK STILL CANNOT SEE, MEASURED AND NOT SUPPOSED. Three synthetic fixtures sharing
one `produced` qualifier were run: naming BOTH a wrong path and a wrong branch fails as intended,
but a claim naming ONLY a path, with no `on branch` clause present at all, and one naming ONLY a
branch, with no path, each returned `SWEEP CLEAN` at exit 0. (That is CLAUSE-ABSENT, not
clause-wrong: a wrong path beside a right branch DOES fail, and so does the reverse.) Part (b) is
blind to a one-sided provenance claim by construction, and to any provenance sentence not phrased
with the word `produced`. A MISSING REPORT IS FATAL NOWHERE -- `check_report_identity` returns
early when there is no report at all, so the sweep prints `SWEEP CLEAN` at exit 0 with nothing to
verify against; part (a) now at least NAMES that gap in its notes rather than skipping in silence.
Leaving the exit code alone there is the `coordinator`'s call, deliberately, so that two checks do
not disagree about the same condition; the standing gap predates this pass and remains open.
THE WRAP BLINDNESS IS NOT ON THIS LIST BECAUSE IT WAS FIXED RATHER THAN DOCUMENTED, and that is
the distinction this list has to keep: a limit belongs here only when it is a CONSTRUCTION limit.
A REPAIRABLE BUG NEVER BELONGS ON IT -- putting one here converts a defect into a documented
feature, which is how a guard stops getting fixed. That does not mean such a thing goes unrecorded:
it gets its own adjacent paragraph, named as a bug and as either repaired or owed, exactly as the
wrap defect does above.
THE REST OF THAT DEFECT CLASS IS NOW MEASURED AND CLOSED, and the answer is better than "filed".
`strat-data-steward` flagged rather than silently skipped `_PRECEDING_STAMPED_RE` (`stood at`,
`had been`) and `_PRECEDING_LIVE_RE` (`is now`), which carry the same literal spaces and feed
checks 1 to 3 -- the SUITE COUNT, the thing this sweep was originally built for. The reviewer then
audited every `re.compile` and every marker literal in the file and MEASURED each, planting a
wrong live figure under a correct banner: `is now` fails as it should and `is\nnow` STILL fails,
because a bare `\bnow` alternative catches it and the default is live anyway; `stood\nat` and
`had\nbeen` fail too -- a wrap there makes a STAMPED claim read as LIVE, which is over-strict and
never a miss; and `_Last\nrun` fails CLOSED with an explicit refusal that freshness cannot be
checked. **NO LIVE SUITE CLAIM CAN HIDE BEHIND A WRAP.**
THE PRINCIPLE THAT SEPARATES THE ONE REAL DEFECT FROM ITS LOOK-ALIKES, and it is the part to
remember: `_PROVENANCE_PATH_BRANCH_RE` was THE ONLY literal-space regex in this file where a
non-match means SKIP THIS CLAIM. Every other one falls through to a strict default or refuses
outright. A literal space is dangerous exactly where failing to match means saying nothing.
TWO OF THE COORDINATOR'S OWN DESIGN INSTRUCTIONS WERE WRONG AND WERE
CORRECTED BY MEASUREMENT, WHICH IS WORTH RECORDING BECAUSE BOTH WOULD HAVE SHIPPED A GUARD THAT
PASSES LIKE AN INERT ONE: reusing `in_code_span` would have made part (a) PERMANENTLY BLIND,
since every `reportCreatedOn` citation in this record -- right and wrong alike, `78ea508`'s
defect included -- is written in backticks and would have been exempted as a verbatim quote
before the comparison ran; and scoping to `## NEXT` raised 21 false positives on ordinary dated
bullets that legitimately cite the report current at their own pass.
THE NINE BANNED WORDS ARE NO LONGER TYPED ANYWHERE IN THE SUITE. `kb/setting.md` is vendored to
`Tools/architect/kb_snapshot/setting.md` -- byte-identical to source by sha256 and
`git hash-object`, a SNAPSHOT AND NOT A SYNC exactly as `gdd_snapshot/` is -- and T-UI-03's
banned-register clause PARSES the nine out of that file's own bullet at run time, wrap-aware,
refusing on a missing file, a missing bullet, an unclosed italic span, a malformed token or a
parsed count other than nine. All three refusal shapes were measured RED, and the clause flipped
Success -> Fail on one edited word in the document. A perturbation to a word ABSENT from the six
result lines was run too and left the verdict green BY CONSTRUCTION -- it is a LIVENESS CONTROL
and not a falsifiability proof, and the distinction is recorded because a probe the guard cannot
fail passes exactly like a guard that is not there.
A LANE RULE FINALLY HAS THE WORD IT WAS MISSING. `CLAUDE.md`'s crew section now carries an
EDITOR-DRIVER CLAUSE naming the `coordinator` as the driver when `execute_script` is absent, with
two obligations mirroring the file-write clause's own. `content.md`'s header keeps its own clause
UNCHANGED and points at it. Writing that file and driving the editor were always two permissions
and only one was ever granted; three consecutive gates turned on that wording while the asset it
governed was clean from the first read.
A FINDING THIS RECORD CARRIED IN TWO PLACES WAS FALSE, AND THE INSTRUMENT WAS THE DEFECT.
`gdd_snapshot/MANIFEST.md` was never stale. The retraction and its measurement sit at the entry
below and in `tests.md`; `MANIFEST.md` now carries the CRLF trap and its control in its own text.
THE FOUR RULINGS CARRIED SINCE 2026-08-25 ARE ALL ANSWERED and recorded in `decisions.md`:
`Shortfall` and `captureTurns` stay DECLARED DEBTS with their discharge conditions written down,
displacement feedback is CLOSED as out of scope on a corrected premise, and the dimmed-but-live
BUILD button is CLOSED as legibility rather than a defect. None of the four was agent work.
NOT COMMITTED. Staging and the commit are the user's call.)_

_Last run 2026-08-25 (THE LATCH BECOMES A PIN: SEC 2.8'S KEYS 2 AND 3 NOW TRAVEL THE REAL
COMPOSITION PATH, AND A PRIVATE DUPLICATE SWITCH IS NOW RED. Written by the `coordinator`, whose
file this is. **THE SUITE COUNT DID NOT MOVE AND THAT IS THE HEADLINE, NOT AN OMISSION**: the
suite is **225/225**, every entry Success, zero failed, zero notRun, zero succeededWithWarnings,
and the clause NAME SET IS IDENTICAL to `04437fe`'s -- 225 names both sides, set-difference
empty in both directions. No clause was added; ONE EXISTING CLAUSE GOT TEETH IT DID NOT HAVE.
THE REPORT THAT CERTIFIES THE LIVE FIGURE IS THIS TREE'S: `reportCreatedOn 2026.08.26-03.40.42`,
`succeeded 225`, `failed 0`, `notRun 0`, `succeededWithWarnings 0`, run in
`E:/MultiAgent/Stratocracy` after the revert of the mutation described below. THAT STAMP IS UTC
AND THIS ENTRY'S DATE IS LOCAL.
WHAT WAS DISCHARGED. Carried debt item (a) from the entry below -- the extraction that would turn
the labelled latch into a real pin. `StratComposeMatchResultModel(const FStratMatchResultView&,
const FStratScoreboardModel&, int32, FStratMatchResultModel&)` is now `STRATUI_API` and holds every
copy, every lookup and SEC 2.8's key-to-criterion tag. `StratBuildMatchResultModel` is reduced to
"ask the bridge twice, then compose". Behaviour is unchanged and the clause that proves it is the
reflection-walk identity described below.
WHY THE SPLIT BUYS ANYTHING. The composer takes VALUES, and `FStratMatchResultView` is a plain
`USTRUCT` with a public defaulted `int32 DecidedByKey` -- so a clause HAND-AUTHORS a view carrying
key 2 and key 3 and drives the real composition with it. Those are states no bridge this suite can
build will ever report: they need a capped match in which both sides fought to an EQUAL combat
Fame. The tag for two of SEC 2.8's three keys was reachable by a player and by no test; it is now
reachable by both.
THE PROPERTY WAS RESTATED, NOT JUST ARMED, AND THIS IS THE PART WORTH READING. The old block
claimed to pin that the builder CALLS the seam rather than keeping a private copy. That is an
implementation-identity claim and NO black-box clause can make it -- a correct duplicate is
indistinguishable from a call, and a wrong one is caught by the output. What the clause now
asserts is the OUTPUT: the composed model's tag is right for every one of SEC 2.8's keys, and it
is the claim that actually protects the screen. **THE QUALIFIER MATTERS AND WAS ADDED ON THE
GATE'S RULING 3: that is strictly stronger than the ASSERTIONS it replaces -- two tautologies --
but it does NOT subsume the PROPERTY the old block named, which remains unpinnable by any
black-box clause. Both are true at once: the assertion was dead, AND the property was the wrong
one to have chosen.**
ALL-OR-NOTHING SURVIVED AND IS STRONGER. Both refusals happen before the compose call and the
composer assigns wholesale on its last line, so a refusing build cannot have touched the caller's
model at all.
FALSIFIABILITY WAS MEASURED, AND THE MUTANT WAS CHOSEN TO BE THE DEFECT THE OLD LATCH CLAIMED TO
CATCH. A PRIVATE DUPLICATE SWITCH was planted inside the composer, swapped on keys 2 and 3, WITH
`StratScoreCriterionForKey` LEFT CORRECT. Rebuilt, and `Automation RunTests
Stratocracy.StratUI.T-UI-03` run: **25 succeeded, 1 failed**, and the two errors are both from the
composed-model block, naming key 2 and key 3 with the criterion each should have carried. The
seam's own key loop stayed GREEN -- correctly, since the seam was untouched -- which is what shows
the two blocks grade different things. Reverted, blob re-measured identical
(`git hash-object` `e5d6e3a` before and after), rebuilt, full suite re-run.
AND THAT CONTROL WAS NOT ENOUGH, WHICH THE GATE CAUGHT AND THIS RECORD WOULD NOT HAVE. A duplicate
switch planted INSIDE the composer makes both operands of the builder-versus-composer identity
comparison wrong IDENTICALLY, so that assertion stayed GREEN under it and nothing had ever shown
the reflection walk able to report `false`. Two things now do: a `TestFalse` over a copy perturbed
in the criterion tag, on `StratSelectionMachineParity.cpp`'s precedent, and THE MIRROR MUTATION --
a private duplicate planted in `StratBuildMatchResultModel` instead, bypassing the composer, which
reddened the identity assertion ALONE and left the key loop green. **25 succeeded, 1 failed**, the
mirror image of the first run. Reverted, `e5d6e3a` before and after, rebuilt, full suite re-run;
the report named above is that run.
ITEM (b) IS STAMPED RATHER THAN DISCHARGED. It read that `strat_banner_sweep.py`'s hand-typed
`TRACKED_ITEMS` was blind to the latch. THE LATCH NO LONGER EXISTS, so that subject is gone; the
general point it rode on -- a guard whose reference data is typed by hand goes inert on its own
subject silently -- is unchanged and stays `strat-data-steward`'s to weigh. Nothing is filed
against it now.
A TEMPORARY PRODUCTION MUTATION IS NO LONGER AN EXCEPTION IN THIS MILESTONE -- IT IS A STANDING
CONDITION, WRITTEN OUT ON THE GATE'S OBSERVATION 2. It has now happened three times in two passes,
and this record's own rule is that a repeated exception has stopped describing an exception.
**THE CONDITION: only the `coordinator` may do it, never a lane agent** -- `strat-test-author`
refused exactly this in wave 6 and was right, and that lane rule is untouched. **THE TRIGGER: a
falsifiability control that cannot be built inside the clause**, which is the case precisely when
the mutant's outputs are not reconstructible from functions the clause can already call. **THE
EVIDENCE THAT MUST RIDE WITH IT, all four:** the mutant named in prose; the failing run quoted with
its counts; `git hash-object` on the mutated file BEFORE and AFTER, shown equal; and a full suite
re-run AFTER the revert, which is the report the banner then cites. Every instance so far carries
all four. **AND THE MUTANT MUST EXERCISE THE ASSERTION IT IS OFFERED AS EVIDENCE FOR** -- the gate
caught a control that did not: planting a duplicate switch inside the composer made both sides of
the identity comparison wrong IDENTICALLY, so that block stayed green and had never been shown able
to fail. The mirror mutation, in the BUILDER, is what exercised it.
NOTHING IS COMMITTED. Three modified files, all under `Source/StratUI/`, plus the three record
files; staging is the user's call.)

_Last run 2026-08-25 (SEC 2.8'S TIEBREAK KEYS 2 AND 3 ARE PINNED, AND THE HOLE THE ITEM-1e
ENTRY BELOW FILED FOR ANOTHER OWNER IS DISCHARGED. Written by the `coordinator`, whose file this
is. The suite WAS **225/225** at that pass; SUPERSEDED 2026-08-25 by the entry above, which moved
no count and strengthened the clause. It was every entry Success, zero failed, zero notRun, zero
succeededWithWarnings. The delta is **+1** and nothing was removed or renamed, measured by
set-difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST` walked over `Source/` -- 224 names at
`a808e0b`, 225 in this tree, the one addition being
`Stratocracy.StratUI.T-UI-03.EveryTiebreakKeyReachesTheScreenAsItsOwnCriterion`. THE REPORT THAT
CERTIFIES THE LIVE FIGURE IS THIS TREE'S AND IS NAMED RATHER THAN ALLUDED TO:
`reportCreatedOn 2026.08.26-02.41.12`, `succeeded 225`, `failed 0`, `notRun 0`,
`succeededWithWarnings 0`, run in `E:/MultiAgent/Stratocracy` over the working tree these edits
produced. THAT STAMP IS UTC AND THIS ENTRY'S DATE IS LOCAL, which is why they name different
days.
WHAT WAS ACTUALLY WRONG, AND IT WAS A COVERAGE HOLE RATHER THAN A WRONG ANSWER. The mapping from
SEC 2.8's key number to `EStratScoreCriterion` was CORRECT and was UNREACHABLE. It sat as a
`switch` inline in `StratBuildMatchResultModel`, and keys 2 and 3 need a capped match in which
both sides fought to an EQUAL combat Fame -- every game SEC 2.9's AI plays on the shipped
scenario ends `Decisive`/`FlagDestroyed` with `decidedByKey == 0`. So a swapped mapping would
have marked the wrong row on the verdict screen with every number on it still right, and would
have shipped green. This is the reflected-verb-with-no-caller shape inverted: the CODE was
reachable by a player and not by a test.
THE FIX IS A SEAM PLUS A CLAUSE AND NEITHER HALF IS WORTH ANYTHING ALONE. `STRATUI_API bool
StratScoreCriterionForKey(int32 Key, EStratScoreCriterion& OutCriterion)` in
`Source/StratUI/StratMatchResultWidget.cpp` -- the same switch, moved out unchanged, exactly the
discharge `tests.md` had named before it existed -- and one clause that calls it with all three
keys and five non-keys. `StratBuildMatchResultModel` now has one line where it had a switch.
**[CORRECTED 2026-08-25, same pass. This sentence continued "and the clause also pins that it
CALLS the seam rather than keeping a second copy of it". IT DOES NOT.** The clause's third block
runs over the seeded opening, where `DecidedByKey == 0`, so both of its assertions are
tautologies and a builder with a private duplicate switch would pass them. It is kept as a latch
that arms itself when a non-zero key becomes reachable, and both the block comment and `tests.md`
now say so. Caught by `strat-integration-reviewer` as observation 1 on the gate for this change
-- `VERDICT: PASS`, zero findings -- and the observation was right. THE SAME
UNREACHED-ARM-READS-AS-COVERED SHAPE THIS WHOLE FIX EXISTS TO CLOSE, REAPPEARING IN THE
COORDINATOR'S OWN PROSE ABOUT THE FIX.**]
THE EXPECTATION IS THE SCOREBOARD'S OWN ROWS AND NOT A TYPED TABLE. Key N is required to name
`StratBuildScoreboardModel`'s `Rows[N - 1].Criterion` on the seeded bridge; the only thing the
clause supplies is that SEC 2.8 counts from one. A typed table of three pairs would have been
this suite transcribing the switch it grades.
FALSIFIABILITY WAS MEASURED AND NOT INFERRED, AND THE CONTROL IS THE PART WORTH CARRYING. Keys 2
and 3 were SWAPPED in the shipped function, the target rebuilt `Result: Succeeded`, and
`Automation RunTests Stratocracy.StratUI.T-UI-03` run: **25 succeeded, 1 failed**, the new clause
the only red one, naming both swapped keys. THE PRE-EXISTING
`DecidedByKeyReachesTheScreenAsACriterionAndNotAsArithmetic` STAYED GREEN OVER THE MUTANT -- which
is the hole demonstrated rather than argued. The mutation was reverted and the blob re-measured
identical (`git hash-object` `68a842d` before and after), the target rebuilt, and the full suite
re-run afterwards; the report named above is a post-revert run over these exact bytes.
TWO GATES RAN AND BOTH RETURNED `VERDICT: PASS` WITH ZERO FINDINGS, and the second one exists
because the first changed the tree. `strat-integration-reviewer` read the UNCOMMITTED working tree
and re-derived every figure above rather than taking it -- the macro census 224 -> 225 walked from
`git ls-tree` on the HEAD side, the report's mtime proven to postdate every test-defining source,
the vendored bytes checked through git's own tracked-path machinery with filters applied --
`git diff --stat`, `git status --porcelain` and `git diff --numstat` over `Source/StratRules` and
`Data`, all empty, plus `rulesCommit 96d93ea` and `dataCommit c2f5860` re-derived from the
manifests, NOT a literal `git hash-object` per vendored file -- the module arrows parsed out of all five
`.Build.cs` files. ITS OBSERVATION 1 OF FIVE WAS A REAL DEFECT AND IS THE PART WORTH READING: the new
clause's third block, which this entry originally said pinned the builder as the seam's CALLER,
is a TAUTOLOGY on the shipped scenario -- see the correction above. The re-gate on that correction
returned `PASS` too, on a shape sweep rather than a phrase sweep, and upheld keeping the inert
block as a labelled latch on a ground the `coordinator` had not made: it lives INSIDE an existing
clause, so it cannot inflate the suite census the way a standalone inert clause would.
THE RE-GATE ALSO OVERRULED THE WORD "IMPOSSIBLE", AND IT WAS RIGHT TO. The `coordinator` told it
no way existed to make that block bite. `FStratMatchResultView` is a plain `USTRUCT` with a public
defaulted `int32 DecidedByKey`, so a clause can hand-author key 2 WITH NO BRIDGE; the only
obstacle is that `StratBuildMatchResultModel` reaches for its result through the bridge, and
extracting the key-to-tag step to take a `const FStratMatchResultView&` makes the caller property
fully falsifiable. AN UNMEASURED LIMITATION ASSERTED AS A RULE is its own defect class in this
record, and this is an instance of it caught inside one session.
FILED, NOT BUILT -- CARRIED DEBT, OWNED BY THE `coordinator`. **[(a) DISCHARGED 2026-08-25 by the
entry above -- `StratComposeMatchResultModel` exists and the latch is now a pin. (b) STAMPED: its
subject, the latch, no longer exists. Both are kept because they are what named the work.]** (a) Extract
`StratBuildMatchResultModel`'s key-to-tag step to take a `const FStratMatchResultView&`, so the
caller property stops being a latch and becomes a pin. Cost: one production change plus a gate;
declined this pass because the property's subject is one line. (b) `strat_banner_sweep.py`'s
`TRACKED_ITEMS` is a TYPED list of two and does not know about (a), so no guard will notice if
that latch is still inert a milestone from now. Registering it is `strat-data-steward`'s lane and
carries that lane's falsifiability obligation -- a guard whose reference data is typed by hand
goes inert on its own subject silently, which this record has measured before.
NOTHING IS COMMITTED. The working tree carries six modified files -- two source, one clause file
and three record files -- and staging is the user's call.)

_Last run 2026-08-25 (ITEM 1e LANDS WHOLE AND IS MERGED: SEC 2.11.4'S END-OF-MATCH SCREEN IS
ROUTED, PINNED, DRAWN AND REACHABLE BY A PLAYER. The suite WAS **224/224** at that pass,
every entry Success, zero failed and zero notRun; SUPERSEDED 2026-08-25 by the 225/225 entry
above, which added one clause and removed none. The delta is +6 and nothing was removed or
renamed, measured by set-difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST` walked over `Source/`
and RE-DERIVED BY THE `coordinator` independently of the lane report -- 218 on `master` at
`194de95` and 224 in the lane tree, the macro census and the automation report agreeing at 224.
THE REPORT IS NAMED RATHER THAN ALLUDED TO, on this file's own standing practice, and THE ONE
THAT CERTIFIES THE LIVE FIGURE IS THIS TREE'S: `reportCreatedOn 2026.08.26-01.30.10`,
`succeeded 224`, `failed 0`, `notRun 0`, `succeededWithWarnings 0`, run in
`E:/MultiAgent/Stratocracy` AFTER the merge commit `d6492c9`, over the merged bytes.
The lane tree's own earlier run -- `reportCreatedOn 2026.08.26-00.28.42`, same 224/0/0 -- is
what the branch carried and is now history: a report is evidence about the tree that produced
it, and the merge made a different tree.
THAT STAMP IS UTC AND THIS ENTRY'S DATE IS LOCAL, which is why they name different days.
**[SUPERSEDED BY THE CORRECTION ABOVE, 2026-08-25 POST-MERGE. The paragraph that follows WAS
true while this entry lived on the branch and is kept rather than deleted, on this file's
standing practice, because it records why the figure was stated inside the lane branch at all.
Its closing instruction IS ALREADY DISCHARGED -- the suite RAN in the integration tree at
`reportCreatedOn 2026.08.26-01.30.10`, 224/0/0, after `d6492c9`. Do not act on it.]** It read:
*"THE FIGURE IS THE LANE TREE'S AND NOT THE INTEGRATION TREE'S. It was produced in
`E:/MultiAgent/Strat-wt/slot-1` on branch `feat/match-result-screen`, and it is stated here
inside that branch so the claim and the clauses it counts move into `master` in ONE commit. The
suite must be RE-RUN in the integration tree after the merge, because the merge brings clauses
into a tree whose newest report predates them."* -- and that is exactly what happened: the sweep
FAILED between the merge and the suite run, all three of `TREE DISAGREES WITH ITSELF`, `SUITE
COUNT AGREEMENT` and `REPORT IDENTITY` firing, and the run cleared it.
WHY THIS ONE GOT THROUGH, WHICH IS THE PART WORTH CARRYING. `strat_banner_sweep.py` returned
`SWEEP CLEAN` over this entry while it contradicted itself, because the sweep grades the `N/N`
FIGURE and the REPORT IDENTITY -- it has nothing that compares an entry's claim about WHICH TREE
produced the report against the tree it is running in. A stale provenance sentence is invisible
to it by construction, and this one also carried a live imperative to redo work already done.
**[NO LONGER TRUE, STAMPED 2026-08-26: this paragraph is written in the present tense and the
tense is now wrong. The guard clause filed here WAS BUILT -- `strat_banner_sweep.py` gained check
5, REPORT PROVENANCE, and a provenance sentence naming a foreign tree is no longer invisible to
it. See the topmost banner segment, which also records the hole that check shipped with and the
repair. Kept rather than deleted, because this paragraph is what named the gap.]**
Filed for `strat-data-steward` as a guard clause. The 218/218 in the entry below WAS the live
figure at that pass and is SUPERSEDED BY THIS ENTRY.
Written by the `coordinator`, whose file this is.
WHAT LANDED, ENGINE HALF (`strat-gameplay-engineer`, editor closed, slot-1). A new
`UStratMatchResultWidget` in `StratUI` with `FStratMatchResultModel`, `StratResultLineFor`,
`StratResultTierTextFor`, `SideFaction` and `StratBuildMatchResultModel`; the
`MatchResultWidgetClass` / `MatchResultZOrder` / `CreateMatchResultWidget` trio on
`AStratScoreboardHUD` in the shape the guidance and production-menu ones already take; and the
trigger inside `UStratMatchSubsystem::ConcludeMatchIfEnded`'s EXISTING `bMatchConclusionAnnounced`
latch, with `StartMatchInternal` calling `HideMatchResult` so a restart clears the screen. Build
`Result: Succeeded`, exit 0, 55.67 s, green on the first attempt, and the diff is purely additive
-- 393 insertions and 0 deletions across five tracked files, plus two new ones.
THE CONSEQUENTIAL CALL WAS EMBEDDING THE LIVE SCOREBOARD'S OWN MODEL WHOLE. `FStratMatchResultModel`
carries `FStratScoreboardModel` and `StratBuildMatchResultModel` calls `StratBuildScoreboardModel`
-- the same builder the live panel uses, on the same bridge and the same frame -- rather than
composing three rows locally. SEC 2.11.4 exists because revision SEC 1.5-#1 forbids the tiebreak
being a hidden win condition, so a verdict screen spelling SEC 2.8's order a SECOND way would
reopen the hiding place one layer up. This makes "the same three rows in the same order" an
IDENTITY rather than an agreement, and there is now exactly one implementation of that order on
screen.
THE SIDE-TO-FACTION BINDING IS OURS AND NOT THE GDD'S, ruled by the user this pass. Nothing in
the GDD, the scenario JSON or the code binds a `strat` side index to a faction, so `SideFaction`
pins side 0 to the Directorate and side 1 to the Vanguard, labelled in prose as a harness pin on
`AStratScoreboardHUD::FirstSide`'s line and carrying the sentence that it must DELEGATE, not
merely agree, if the GDD ever makes the binding itself.
WHAT LANDED, CLAUSE HALF (`strat-test-author`, same tree, after the green build). Six clauses in
`Source/StratUI/Tests/StratMatchResultModelClauses.cpp` (five) and
`Source/StratPlay/Tests/StratMatchResultTrigger.cpp` (one), plus a UMG-free host double and probe
on the `StratProductionMenuHostDouble`/`Probe` precedent. The rows-by-identity clause compares via
`UScriptStruct::CompareScriptStruct` -- UE's own reflection walk, not a hand-listed field list, so
a field added tomorrow is covered on compile -- and carries a negative control that the two
seats' models must DIFFER. The six result lines are PARSED OUT OF THE GDD SNAPSHOT at run time,
keyed by the document's own bullet labels, and the 30-word budget is parsed from the sentence that
states it, so the clause does not retype the literal the code already holds.
THE ACCEPTANCE ID THE ENGINEER PROPOSED WAS WRONG AND THE TEST-AUTHOR CAUGHT IT. `T-TURN-04` is a
RULES property -- the tiebreak resolving in SEC 2.8's exact order -- and is already pinned in
`Source/StratBridge/Tests/StratMatchResultClauses.cpp`. The trigger clause is filed under
`T-INT-05` instead, beside `StratMatchConclusion.cpp`'s five on the same transition. All 85 GDD
IDs were walked and NOTHING WAS MINTED; SEC 2.11.4 has no ID of its own.
THE FALSIFIABILITY PROOF WAS BUILT INSIDE THE CLAUSE, AND THE DISPATCH THAT ASKED OTHERWISE WAS
WRONG. The `coordinator` asked `strat-test-author` to mutate `StratMatchResultWidget.cpp`, run
red, revert and run green. THAT FILE IS OUTSIDE ITS LANE and the rule admits no temporary edit;
the agent refused, correctly, and a dispatch instruction is not consent to change that. It
reconstructed the mutant's two outputs from the same two `STRATUI_API` functions the module
itself calls and asserted they differ from the shipped answer on a board SEC 2.9's AI had just
played -- 156 commands, `Decisive`, `FlagDestroyed`, winner 1, key 0 -- so the witness is
permanent rather than spent, and it is the one assertion that would speak up if the two faction
voices ever stopped being distinguishable.
SUPERSEDED WITHIN THIS PASS -- THE ASSET WAS BUILT, THE SCREEN WAS SEEN DRAWING A REAL VERDICT,
AND ALL OF IT IS NOW MERGED AT `d6492c9`. The paragraph that follows was written while it was still true and is kept because it
records what the C++ half does WITHOUT an asset, which is still the behaviour on any HUD that
leaves `MatchResultWidgetClass` unset. `WBP_MatchResult` did not exist at that moment.
Until it is authored and set on `MatchResultWidgetClass`, `ConcludeMatchIfEnded` logs at Warning
that no end-of-match screen will be shown and the match still concludes correctly. The editor was
CLOSED for this whole pass -- no `UnrealEditor` process, port 9315 refused -- which is the right
condition for the two halves that landed and the blocking one for the half that did not. Note
that `MatchResultWidgetClass` is typed to the concrete class rather than to a bare `UUserWidget`
as `ProductionMenuWidgetClass` is, so the asset will bake `/Script/StratUI.StratMatchResultWidget`
into itself irreversibly; that is deliberate, since a WBP free to compose its own verdict would be
a second author of SEC 2.8's result.
THREE THINGS ARE FILED FOR AN OWNER OTHER THAN THE LANE THAT FOUND THEM. (1) **[DISCHARGED
2026-08-25 by the 225/225 entry above; the sentence is kept because it is what named the seam.]**
SEC 2.8's tiebreak keys 2 and 3 COULD NOT BE PINNED at that pass: every game the AI plays on the
shipped scenario ends `Decisive`/`FlagDestroyed` with `decidedByKey == 0`, so a wrong mapping for
keys 2 and 3 shipped green. The seam named was a `STRATUI_API` free function exposing the
key-to-criterion switch that was then inline in `StratBuildMatchResultModel` -- it is now
`StratScoreCriterionForKey`, and one clause calls it. (2) `kb/setting.md`'s nine banned register
words are a STATED LITERAL in the clause and not a read, because that file lives only in
`E:/MultiAgent/stratocracy-content` and a headless run may not reach outside the tree; discharged
by vendoring it beside the GDD snapshot, which is `strat-data-steward`'s call. (3)
**[RETRACTED 2026-08-26 -- THE MANIFEST WAS NEVER STALE, AND THE INSTRUMENT THAT MEASURED IT WAS
THE THING THAT WAS WRONG. Kept rather than deleted, on this file's standing practice. It read:]**
*"`Tools/architect/gdd_snapshot/MANIFEST.md` is STALE -- it records 446,133 bytes against a
tracked file of 449,498, and two clauses now read that file. Steward's lane, flagged and not
fixed."* BOTH HALVES ARE WITHDRAWN, AND THE MEASUREMENT IS GIVEN HERE RATHER THAN THE CONCLUSION.
Re-derived in this tree: the vendored `Stratocracy_Prototype_GDD.md` is 446,133 bytes by `wc -c`,
by `len()` over the raw bytes, and by `git cat-file -s` on its `HEAD` blob -- three readings that
do not share a code path. Its sha256 is
`bfa4f713ab1415cff4e0e0ef09d94f751ca727f7893bc0d44900d81248ed954c`, which is EXACTLY the hash the
manifest records, and the live document in `E:/MultiAgent/stratocracy-content` is byte identical
to it. The file carries 3,365 lines and ZERO carriage returns, and 446,133 + 3,365 = 449,498 --
one added byte per line, which is precisely what a CRLF-applying reader reports for a file this
size. THE CONTROL, because a count of zero proves nothing until the instrument is shown able to
speak: the same `tr -cd` reading counted 2 carriage returns in a known 6-byte CRLF file, so its
silence over the snapshot is a reading and not a failure. 449,498 was never the size of anything
on disk. The "flagged and not fixed" half is withdrawn with the rest: NOTHING IS OWED TO
`strat-data-steward` HERE, and a reader who had acted on it would have edited a correct manifest
to match a figure no file ever had. `MANIFEST.md` now carries this trap and the measuring command
in its own text, so the next byte check has its control inside the file it is checking.
THE GATE RETURNED `VERDICT: PASS` WITH ZERO FINDINGS, and it is stamped here rather than
promised. `strat-integration-reviewer` read the UNCOMMITTED working tree -- 13 files, code,
clauses and all three record files together -- and re-derived every figure this entry states
rather than taking it: macro census 218 on `master` and 224 in the lane, the report at 224/0/0
opened `utf-8-sig`, the sweep clean at exit 0 with exactly one LIVE claim, and the module arrows
PARSED OUT OF ALL FIVE `.Build.cs` FILES with comments stripped rather than read by eye. It
confirmed the rows-by-identity claim structurally (there is no second implementation of SEC 2.8's
order to diverge from), found no widget-side arithmetic, and confirmed all three of the items
filed above for other owners.
ONE NON-GATING OBSERVATION IS WORTH CARRYING. `StratMatchResultWidget.h` tells the reader that
`Source/StratUI/StratUI.Build.cs` records the irreversible `/Script/StratUI.StratMatchResultWidget`
bake; that file records the PRINCIPLE under the scoreboard widget's name and does not name this
class, so the pointer governs but does not resolve by grep. Engineer's lane, to fold in the next
time that file is opened for another reason.
THIS VERDICT DESCRIBES THE LANE TREE AS IT STOOD BEFORE THE ASSET, AND NOT `master`. Two
post-pass changes happened since that gate read the tree: this entry's own verdict stamp, and
`WBP_MatchResult`. THE NARROW RE-GATE THEY OWED HAS NOW RUN AND IS DISCHARGED -- see the BLOCK
below. STILL OWED AND NOT DISCHARGED BY ANYTHING HERE: after the merge lands, the suite must be
re-run in the integration tree and the gate run again, because a `VERDICT: PASS` describes a tree
and never a phase.
THE ASSET HALF LANDED TOO, AND THE SCREEN WAS SEEN DRAWING A REAL VERDICT. `WBP_MatchResult`
exists, `BP_StratScoreboardHUD.MatchResultWidgetClass` points at it, and a live AI-vs-AI match on
Ferrum Crossing ended `STRAT-MATCH concluded turn=6 turnCap=20 sideToMove=1 tier=Decisive` with
`Decisive`, the Vanguard's victory line, and `Destroyed 75/775`, `Objectives 2/8 6/8`,
`Unit HP 49/115` on screen beside the live scoreboard's own `TURN 6 / 20`. **The viewer was side 0
and LOST, and was shown the WINNER's faction line** -- the one comparison SEC 2.11.4 turns on,
observed rather than argued. `content.md` carries the evidence, the eleven live `TextBlock` reads
and the API measurements; this banner does not restate them.
THE EDITOR WAS OPENED ON `slot-1` AND NOT ON THE INTEGRATION TREE, on the user's explicit
instruction, because the asset cannot derive from a class the integration tree has never compiled.
That is a deliberate departure from "`Content/` is touched on the integration tree only" and it is
recorded rather than quietly taken: it is safe only because no other lane was running, since
`BP_StratScoreboardHUD.uasset` is a binary edit on a branch and a concurrent edit elsewhere would
be lost silently rather than conflict.
THE RE-GATE RETURNED `VERDICT: BLOCK`, AND THE FINDING WAS AGAINST THE RECORD'S WORDING RATHER
THAN THE TREE -- "nothing about the asset itself needs redoing", in the reviewer's words. Two
authorities had been merged into one. `content.md`'s header licenses writing THAT FILE and says in
terms that it "is not a general licence to act in the content lane"; authoring the asset and
editing the HUD default IS acting in that lane. The asset work's authority is THE USER'S EXPLICIT
INSTRUCTION, given after they were shown the departure it required; the fallback clause's authority
covers only the file write, whose condition was measured with the control the header demands -- a
lookup naming both `execute_script` tools and `unreal_status` returned only `unreal_status`, so
`strat-editor-builder` had no route to the editor at all. Both entries have been corrected to keep
the two apart. **A RULING IS CARRIED:** the fallback condition has now recurred at least four
times and its header was already amended once, on 2026-08-23, for that same reason.
WHAT THE RE-GATE CONFIRMED ON ITS OWN INSTRUMENTS. No `Source/` file changed after the earlier
PASS -- derived from mtimes, the newest under `Source/` being `20:27:57` against the assets'
`20:53:38` and `20:55:22`. `WBP_MatchResult` bakes `/Script/StratUI.StratMatchResultWidget`,
carries NO `KismetMathLibrary`, NO `K2Node_FormatText`, NO `Conv_*`, NO branch or select node, and
serializes `PropertyBindings` as **0** -- so T-UI-03 holds on the binding path as well as the graph
path. `BP_StratScoreboardHUD.uasset` was string-diffed against its HEAD blob SMUDGED THROUGH
`git lfs smudge`, since `git show` returns the 130-byte pointer: exactly one property added, and
`MatchResultZOrder` appears in NEITHER version, which is how a reader knows it equals the C++
default of 30.
ONE THING THE RE-GATE DID NOT RUN, STATED AS `not run` RATHER THAN AS A PASS. It could not
re-derive "11 of 11 pins connected" -- it holds no editor tool and `FEdGraphPin` links are
index-encoded. Its own reading is that the live-PIE sweep is the better instrument anyway: an
unconnected exec or text pin leaves the DESIGN-TIME DEFAULT in place, which is exactly the state
observed on the instances that did not run, so eleven live values beside untouched defaults
separates a graph that RAN from a widget that merely EXISTS.
THE WORKTREE EDITOR WAS AGREED WITH, WITH ONE CAVEAT THIS BANNER OWES ITS READER. The reviewer
would have ruled the same way -- the reparent target exists only on this branch, so authoring on
the integration tree is not slower but impossible, and merging unproven C++ first would invert the
evidence order this project runs on. The caveat: the safety of the binary edit to
`BP_StratScoreboardHUD.uasset` rests on no other lane HAPPENING to run, not on any mechanism. There
is no lock. **If `/strat-parallel` spins a second slot before this merges, that asset is a
silent-loss hazard -- the merge goes first.**)

_Last run 2026-08-25 (SEC 2.11.5 CLOSES ON A HUMAN PLAYTEST, AND THE PLAYTEST CORRECTED THE
COORDINATOR. Written by the `coordinator`. An evidence-and-record pass: no source file, no test
and no asset was touched.
The suite WAS **218/218** at that pass, every entry Success, zero failed and
zero notRun; SUPERSEDED 2026-08-25 by the 224/224 entry above. NO SUITE FIGURE MOVED IN THIS PASS AND NO RUN WAS PERFORMED FOR IT: the figure is
carried forward from the entry below, over the same automation report that entry names, and
nothing under `Source/` was touched here. It is restated rather than
left below because this file's live count lives in its TOPMOST entry -- an unrestated figure
reads as superseded to `strat_banner_sweep.py`, which is how this entry first failed the sweep.
WHAT A PERSON SAW, which is the standing of this evidence and not a tool reading. The greyed
unaffordable row, the `need N` shortfall, the BUILD buttons dead once the factory had built, the
footer reading `Boxed in -- build waits for a free hex.`, AND THE Q31 HALF -- that the BUILD
buttons stay clickable while that footer shows. That last one is the half no structural argument
could close and no instrument here could reach; it is now closed by eye, which is the same
standing Sec 2.11.6-B was closed on.
THE USER'S CONSTRUCTION DIFFERED FROM THE ONE I GAVE THEM, AND THEIRS WAS THE CORRECT ONE. My
checklist said to move the newly built Infantry OFF the factory hex onto its free neighbour --
which EMPTIES THE ONE HEX THAT MUST BE OCCUPIED and makes the state unreachable by construction.
They found it directly: six adjacent hexes occupied with the centre free produced NO footer; they
then ended the round, built a second Infantry so the centre was occupied too, and the message
appeared.
THE RULE SAYS SO AND I COULD HAVE READ IT FIRST. `strat::spawnHexesBlocked` in the vendored
`Ui.good.cpp` returns FALSE when the factory hex itself is unoccupied, BEFORE it examines a single
neighbour -- the factory hex is where the spawner places first, so a free centre means there is
somewhere to build however full the ring is. **A boxed-in factory needs SEVEN hexes filled, the
centre included.** Stated in its own sentence because it cost a round here and reads like an
off-by-one when it is not one.
WHAT THE DEFECT ACTUALLY WAS, since it matters for how the next checklist gets written. Every
DISPATCH in this pass said "its own hex AND all six neighbours" and was right; only the
USER-FACING step list contradicted it, and nothing cross-checks those two against each other. The
spec was correct in the place a gate reads and wrong in the place a human acts, which is the
worse way round. Same family as the correction that must reach the handoff sentence.
THE INSTRUMENT PASS THAT PRECEDED IT confirmed the data path in a live match before the editor
crashed on PIE teardown: four rows on the real factory, `Shortfall` equal to price minus live Fame
at TWO Fame values across four prices, the live PIE widget tree carrying `need 200`/`need 100`/
`need 50`, render opacity 0.4 on exactly the unaffordable rows, and `bAffordable` and `bAvailable`
observed DISAGREEING IN BOTH DIRECTIONS. `content.md` carries that detail and this banner does not
restate it.
THE CRASH COST NOTHING AND MY FIRST ATTRIBUTION OF IT WAS WRONG. `Assertion failed: false
[PlayLevel.cpp:553]`, the `TransBuffer` holding the PIE `GameInstance` at teardown. I attributed it
to a Widget Blueprint asset editor left open from an earlier pass and stated that more firmly than
the evidence carried; the proximate trigger was an injected `Escape` stopping PIE. The assert chain
is the same either way and the distinction is between a hypothesis and a measurement. Zero
`Saving Package` lines in the whole log, `git status` clean, commit `3b22b44` intact.
ONE CONTROL WORTH KEEPING. Fifteen injected keys produced ZERO game-side effect, but `Escape` WAS
delivered and stopped PIE. So injection reaches the EDITOR and never the game's Enhanced Input --
which is sharper than "injection does not work" and is what makes the zero mean something.
NO PHASE VERDICT IS STATED HERE. Two gates returned `VERDICT: PASS` on the CODE earlier today and
`3b22b44` carries it; this entry adds observation, not a re-gate. What remains open on item 2 is
2b, displacement feedback, which is in no GDD section and is filed for a ruling.)

_Last run 2026-08-25 (WAVE 4 AND WAVE 5 LAND: SEC 2.11.5'S ROWS ARE BUILT AND ITS FOOTER HAS A
ROUTE FOR THE FIRST TIME. The suite is now 218/218, every entry Success, zero failed and zero
notRun. The delta is +5 and nothing was removed or renamed, measured by set-difference on
`IMPLEMENT_SIMPLE_AUTOMATION_TEST` walked over `Source/` and RE-DERIVED BY THE `coordinator`
independently of the lane report -- the macro census and the automation report agree at 218.
THE REPORT IS NAMED RATHER THAN ALLUDED TO, on this file's own standing practice:
`reportCreatedOn 2026.08.25-19.00.48`, `succeeded 218`, `failed 0`, `notRun 0`, run in THIS tree
on the bytes as they now stand. The 213/213 below is SUPERSEDED BY THIS ENTRY and is history.
Written by the `coordinator`, whose file this is.
WHAT LANDED, CONTENT HALF (`strat-editor-builder`, editor open). `/Game/UI/WBP_ProductionMenu`
now binds each `Row{N}Build`'s `IsEnabled` to `bAvailable` ALONE, prints `need N` off
`FStratBuildOptionView::Shortfall` where it printed the literal word `false`, shows the module's
own `Reason` in the status column, and greys an unaffordable row with `SetRenderOpacity` -- which
does not affect hit testing, so a greyed row stays clickable and Q31 holds by a property of the
node chosen rather than by intent. The two substitutions T-UI-03 exists to catch were ruled out by
DERIVING THE SINK LIST of every `Break` output rather than by eye: `Affordable` reaches eight
sinks and every one is a `Select.Index`, `Available` reaches four and every one is
`SetIsEnabled`. `content.md` carries the detail.
WHAT LANDED, ENGINE HALF (`strat-gameplay-engineer`, editor closed). `FStratBridge::
FactorySpawnBlockedAt` and a `BlueprintPure` `UStratMatchSubsystem::IsOpenMenuFactorySpawnBlocked`,
so `bSpawnBlocked` has a Blueprint route at all for the first time -- before this pass it had
exactly two sites in `Source/` outside `Tests/`, its declaration and its one assignment, and NO
READER. Build `Result: Succeeded`, exit 0, 51 actions. `BlueprintPure` was verified off the
GENERATED FLAGS (`EFunctionFlags 0x54020401` in `StratMatchSubsystem.gen.cpp`) and not off the
specifier typed, which is the difference between reading the intent and reading the artifact.
THE CONSEQUENTIAL CALL WAS REJECTING BOTH ROUTES THE DISPATCH OFFERED, and it is recorded here
because a later pass will be tempted by the cheap one. Reading `AppliedModel.Factories` is the
WRONG CLOCK: `RefreshProductionMenu` queries the bridge live and writes only the rows, while
`AppliedModel` is written only by `ApplyView`, and in `SubmitProductionChoice` an accepted build
rebuilds the menu before the screen. The build that fills a factory's last free hex is exactly
the build that flips this bit, so the divergence is on this footer's own path and not a corner.
Re-querying per read was refused too -- a `BlueprintPure` on a footer's visibility runs per frame.
The value is re-queried ON THE ROWS' OWN CLOCK and published in the same statement group, cleared
beside them, so the footer and the rows are fresh together or stale together and CANNOT be
observed disagreeing. That is structural rather than disciplinary, which is the whole reason it
was worth the extra measurement.
WHAT THE FIVE CLAUSES PIN (`strat-test-author`). That the footer copies the bridge rather than
deriving; that it is false with no menu open, by two routes; that a refusal is not a yes; that a
refusal survives a non-factory hex whose neighbourhood is FULL, so a fall-through to raw
occupancy reddens; and that it rides the rows' clock across a build. Two mutation rounds, six
mutations, all inside `Tests/`: exactly the five new clauses reddened, each at its TERMINAL
assertion, and nothing else. Shipped bytes were restored from copies held outside the repo and
proved identical by `git hash-object`, never by `git checkout --`, which this record has been
bitten by before.
TWO ACCEPTANCE IDS MOVED OFF THE ONES THE DISPATCH PROPOSED, and the lane was right both times.
The dispatch said `T-UI-03`; four seam clauses went to `GATE-BUILDMENU`, which that file already
covers in ten clauses and which is what a member written in the same statement group as
`ProductionMenu` actually belongs to, and the bridge clause went to `T-UI-04` beside its sibling.
NO NEW ACCEPTANCE ID WAS MINTED. My own dispatch prose was the wrong authority and the site was
the right one -- the same shape as the `T-SAVE-05` correction two entries down.
THREE THINGS THIS PASS DOES NOT CLAIM. (1) NO PHASE VERDICT: the reviewer gate has NOT run on
this work, and nothing here is gated. (2) NOBODY HAS SEEN THE ROWS ON A SCREEN -- no greyed row,
no `need N`, no refused BUILD click. No instrument here can fire a Slate click and injection
never reaches this project's `UPlayerInput`, so that wants a HUMAN PIE pass, on a factory with an
unaffordable row and once on a factory that has already built this turn. (3) THE FOOTER IS NOT
BUILT: the accessor is reflected, correct and HAS NO CALLER, which is the `SkipGuidance` shape
this record has already paid for once and is declared here rather than left to be found.
DISCHARGED WHEN `WBP_ProductionMenu`'s footer binds it.
TWO LIMITS THE TEST LANE DECLARED RATHER THAN PAPERED OVER. Which half keeps the closed-panel
answer false cannot be separated from `Tests/` -- both routes to "closed" run the reset, so
deleting either half reddens nothing, and the clause says in its own prose that it pins the
guarantee and not which half delivers it. And clause 5's teeth depend on `SubmitProductionChoice`
calling the refresh before the presentation; reverse that ordering and the clause goes QUIET
rather than red. Pinning it directly would need a seam in production code.)

_Last run 2026-08-25 (WAVE 2 LANDS: THE ENGINE CAN SAY WHO WON, AND TWELVE CLAUSES PIN IT. The
suite is now 213/213, every entry Success, zero failed and zero notRun. The delta is +12 and
nothing was removed, measured by set-difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST` walked over
`Source/` and RE-DERIVED BY THE `coordinator` independently of both lane reports -- the macro
census and the automation report agree at 213. THE REPORT IS NAMED RATHER THAN ALLUDED TO,
because this file already carries an observation about a banner whose cited report disagreed
with the one on disk: `reportCreatedOn 2026.08.25-17.21.56`, `succeeded 213`, `failed 0`,
`notRun 0`, and it is a run performed on the tree as it now stands. It is the THIRD run of
this figure and the citation was rewritten twice to follow it: the test lane's own
`16.20.20`, then `17.03.21`, then this one. Each rewrite was cheaper than the alternative,
which is a banner citing a report older than the tree it describes -- the exact defect this
file already carries an observation about. The two rounds of prose corrections between the
second run and this one were comment-only, and re-running is how that was ESTABLISHED
rather than assumed; the count did not move and none was expected to. Re-running was how the intervening
comment-only rounds were established as comment-only, rather than assumed.
Written by the `coordinator`, whose file this is.
WHAT LANDED. Engine half (`strat-gameplay-engineer`): `FStratBridge::MatchResult` routing
`strat::uiMatchResult`, `FStratMatchResultView` with `EStratResultCause` ordinal-pinned to
`strat::ResultCause`, `UStratMatchSubsystem::GetMatchResult`, `FStratBuildOptionView::Shortfall`,
the three `SubmitCapture` retractions, the hot-seat premise correction, and the `ViewingSide`
desync fix. Test half (`strat-test-author`): twelve clauses across T-TURN-02/04/05, T-UI-03/04/05
and T-FAME-05. `engine.md` and `tests.md` carry the detail and this banner does not restate it.
THE ONE CLAUSE THE ITEM TURNED ON WAS WRITTEN, and it is worth naming because the item closes
falsely without it. `CappedMatchNamesTheKeyThatDiffered` deliberately arranges `winner !=
sideToMove` -- reading the winner from `strat::uiMatchResult` and the side to move from
`strat::buildUiSnapshot` on the same world in the same frame. Without it every who-won clause
would pass against a `sideToMove` derivation, which is right on a flag kill and wrong at the cap,
and the gap this whole item exists to close would have stayed open under a green suite.
TWO PREVIOUSLY-GREEN CLAUSES WENT RED AND WERE CORRECTED AT THEIR PREMISE, NOT WEAKENED.
`SetViewingSideMutatesNoState` was PINNING THE DEFECT -- it asserted that a refused out-of-range
hand-over left the subsystem holding the refused side, quoting a no-rollback sentence that is
about a failed REBUILD and applying it to a failed RANGE CHECK. That is the exact conflation the
fix removed. `ARefusedRefreshLeavesTheOpenMenuAndItsHexIntact` lost its LEVER, not its subject:
it reached a refusal through `SetViewingSide`, which now refuses before assigning; its replacement
lever is `FStratMatchConfig::ViewingSide`, which `StartMatch` assigns unchecked by design.
A RED CLAUSE THAT IS PINNING A DEFECT IS THE ONE CASE WHERE GOING RED IS THE CORRECT OUTCOME, and
telling it apart from a regression takes reading its premise rather than its verdict.
AN ACCEPTANCE ID IN THE ENGINE HALF'S HANDOFF WAS WRONG AND WAS CAUGHT BY READING THE SITE.
The capture clause was proposed under `T-SAVE-05`, which EXISTS -- it is the no-partial-load
rule -- and is not about capture at all. It went under `T-FAME-05`, whose text names the holding
counter the clause measures. A proposed identifier greps exactly like a defined one; only the site
tells them apart, and a hit count never would have.
NO NEW ACCEPTANCE ID WAS MINTED and no phase verdict is stated here. Wave 3 is the reviewer gate
and it has not run; nothing in this pass is gated.)

_Last run 2026-08-25 (THE OWED RE-VENDOR IS DONE. `rulesCommit` MOVES `cae01e3` -> `96d93ea`,
AND `T-INT-01` AND `T-INT-04` BOTH PASS OVER THE NEW BYTES. Written by the `coordinator`. A
vendor-and-record pass: no test and no gameplay source file was touched, NO SUITE FIGURE MOVES AND
NONE IS CLAIMED HERE.
WHAT MOVED. `sync_stratrules.py` was run in the crew repo and vendored 24 sources plus
`StratRules.Build.cs` from `96d93ea`. Exactly three files under `Source/StratRules/` changed --
`Ui.h`, `Ui.good.cpp` and `StratRules.manifest.json` -- which is the set the 2026-08-24 banner
predicted, re-derived here from `git status` rather than carried over from that prediction.
`strat::UiMatchResult` and `strat::uiMatchResult` are DECLARED in the vendored `Ui.h` now, so the
bridge and projection work that waited on them is unblocked.
THE GATE RAN TWICE AND ONLY THE SECOND RUN GRADED BOTH CLAUSES, which is the reusable half. From a
plain shell `run.py --integration` returns PASS on `T-INT-01`, SKIP on `T-INT-04` -- no compiler on
PATH -- and exits 2, a verdict of "did not run" and not a pass. Re-run under `vcvars64.bat` it
returns INTEGRATION GATE PASS on both and exits 0. That is this box's standing condition rather
than a new finding, and a run without vcvars asserts nothing about engine independence. The
compile half matters here beyond ritual: it is the only thing that has read the new vendored bytes
through a compiler, since no UE build has run in this pass.
THE CREW GAP OVER THE VENDORED COMMIT IS ZERO AGAIN, derived with `rev-list --count 96d93ea..HEAD`
in the crew repo at the moment this was written, with that working tree clean and `main` still
ahead of its origin by two unpushed commits. A gap figure is invalidated by the next crew commit,
which this record has now been bitten by once and states again rather than trusting.)

_Last run 2026-08-24 (THE CANDIDATE BACKLOG WAS RENDERING FOUR ENTRIES WRONG, AND TWO OF THEM
BACKWARDS. Written by the `coordinator`, whose file this is. A record-only pass: no test and no
source file was touched.
NO SUITE FIGURE MOVES AND NONE IS CLAIMED HERE. The live figure is the one the block below cites
and it is unchanged; the macro census and the automation report agree on it, and nothing in this
pass could have moved either.
WHAT WAS FOUND. `buildlist_query` is DISCHARGED and its entry argues at length that it is blocked;
`production_widget` inherits that dead blocker; `presentation_statelessness` renders "(actionable)"
while FORTY-THREE T-INT-05 clauses already run; and `bridge_event_list` renders "(actionable)"
while the rules module defines no event type at all and Sec 4.9 forbids the bridge synthesizing
one. **THE CREW GAP OVER THE VENDORED COMMIT IS NOW ZERO** -- `rulesCommit` is `cae01e3`, which is
crew HEAD -- re-derived with `git rev-list --count` rather than decremented from the nineteen this
file recorded, for the reason every earlier banner gives when its own count moved.
**[SUPERSEDED THE SAME EVENING BY THIS SESSION'S OWN NEXT COMMIT, AND THE DIRECTION MATTERS:
THE GAP IS ONE, AND IT IS THE FIRST GAP IN THIS RECORD THAT ACTUALLY OWES A RE-VENDOR.** The
sentence above was true when written and was made false by `96d93ea` in the crew repo -- the
`uiMatchResult` query -- committed minutes later by the same pass that wrote it. That is the
standing trap about cross-repo counts, met head-on: **a gap figure is invalidated by your own
next commit**, so it is the last thing to measure and the first thing to re-derive.
WHAT CHANGES BESIDES THE NUMBER, and it is the half a bare count would have hidden. Every
previous gap in this file was made of SPEC commits and this record leaned, correctly, on "no
vendored file moved". **This one moved two of them**: `git diff --name-only cae01e37..HEAD --
cpp_reference/` returns `Ui.h`, `Ui.good.cpp` and `test_ui.cpp`, and the first two ARE in the
vendored set. So `Source/StratRules/` here is NO LONGER CURRENT, `rulesCommit` still reads
`cae01e3`, and a re-vendor is now OWED rather than merely available. `test_ui.cpp` is not
vendored -- it defines `main()`, which a UBT module cannot hold -- and `spec/` never was.
NOTHING IS CLAIMED ABOUT THE UE SIDE ON THIS: no re-vendor has run and none is claimed.
**AND `T-INT-01` WILL STAY GREEN THROUGH ALL OF IT, WHICH IS THE TRAP WORTH RECORDING.** An
earlier draft of this very stamp said the vendored bytes are "one commit behind a header they
must hash-match", which reads as though the gate would catch this. IT WILL NOT, and that is by
design: `T-INT-01` checks identity against the `rulesCommit` the manifest RECORDS, not against
crew HEAD. Measured with `git hash-object` on both vendored files, which applies the filters and
so survives the CRLF/LF split a plain diff trips on: `Ui.h` and `Ui.good.cpp` hash-match
`cae01e3` EXACTLY and differ from HEAD. So the vendored tree is perfectly faithful to the commit
it names while being one commit stale, and **a green `T-INT-01` means "honestly vendored", never
"current"**. Nothing in this repo can notice the difference; only a cross-repo count can, and
that count is the thing this same stamp records getting invalidated by its own next commit.
OWNED: the re-vendor step, before any lane builds on the new query.]**
**[DISCHARGED 2026-08-25 by the `coordinator` -- THE RE-VENDOR RAN. `rulesCommit` is
`96d93ea`, both moved files are vendored, and `T-INT-01` passes over the new bytes with
`T-INT-04` beside it under vcvars. Stamped here as well as at the `## NEXT` site, because
this is the sentence that assigns the work and a correction reaching only the evidence has
not been made. This file's newest banner carries the run.]** The entry at
the bottom of `## NEXT` carries the detail and this banner does not restate it.
THE SHAPE UNDERNEATH ALL FIVE, WHICH IS WORTH MORE THAN ANY ONE OF THEM. Every status in that
backlog is COMPUTED from a probe rather than stored, which is the right design and is why the
staleness was invisible: each probe still answers its own question correctly, and what had gone
wrong was the SUBJECT it was pointed at or the word hung on its answer.
`presentation_statelessness` has no probe of its own -- it borrows `scoreboard_widget` -- so the
scorer can never see it as done no matter how many of its clauses land. **A DERIVED STATUS IS ONLY
AS CORRECT AS THE SUBJECT ITS PROBE NAMES**, and a guard grading a neighbour's artifact goes inert
on its own subject silently.
AND A LANE REPORT'S FIGURE WAS WRONG BY MORE THAN A FACTOR OF TWO. An exploring agent reported
"roughly 18" T-INT-05 clauses; set-difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST` walked over
`Source/` returns FORTY-THREE. It changed no conclusion, because the conclusion only needed "many
more than zero" -- but it is the second time this record has caught a count by re-deriving one it
had been handed, and the cost of not re-deriving is not bounded by how wrong the figure happens to
be.
WHAT IS OWED AND NOT DONE HERE. `Tools/architect/candidates.py` and `scan.py` are
`strat-data-steward`'s lane and this pass did not touch them; the rendered list above is corrected
in place and the SCORER that produced it is not. Until it is repaired, the next run regenerates
the same four wrong statuses. OWNED: `strat-data-steward`, and the repair must be shown falsifiable
by breaking the real artifact rather than asserted.)

_Last run 2026-08-24 (THE `Skip guidance` BUTTON IS BUILT -- THE ONE SURFACE SEC 2.11.6-B'S
CLOSURE WAS RECORDED OVER. Written by the `coordinator`, under `content.md`'s fallback condition,
with the tool-surface control that condition requires. `/Game/UI/WBP_DirectiveStrip` now carries a
`SkipButton` whose `OnClicked` casts the owning controller and calls `Skip Guidance`; it was seen
PAINTED in a PIE capture with the guided opening armed, and seen going out TOGETHER with the strip
under a `ke * SkipGuidance` positive control. The `OWNED: strat-editor-builder` line this closure
carried is stamped DISCHARGED where it stands, twice, rather than deleted. `content.md` holds the
evidence and this banner does not restate it.
NO SUITE FIGURE MOVES AND NONE IS CLAIMED HERE. One `.uasset` changed and no test or source file
was touched; the live figure is the one the block below cites and it is unchanged.
WHAT WAS NOT MEASURED WHEN THIS PASS ENDED, AND WAS CLOSED THE SAME EVENING. No mouse event was
injected -- injection still never reaches this project's input -- so "a Slate click on that
rectangle fires `OnClicked`" was INFERRED from a compiled, fully-connected graph on a live,
painted, hit-testable widget rather than observed. **THE USER THEN CLICKED IT AND REPORTS THAT IT
WORKS.** So the last link is closed on a human observation at the keyboard, which is exactly the
standing the ring and the turn-1a marker were closed on, and for the same reason: nothing here can
fire a Slate click. **SEC 2.11.6-B NOW STANDS OVER NO UNBUILT SURFACE** -- the one item its closure
was recorded over is built and exercised. The `GuidedMarkerZOffset` item is untouched by this and
is NOT claimed either way.
ALSO FOUND AND WORTH MORE THAN THE BUTTON: `find_nodes` is filtered to `self`'s class and returns
ZERO for any other class's UFUNCTION -- proved with two engine controls, not assumed. The
pin-context form in `help("FindNodes")` is the only route. `content.md` carries it.)

_Last run 2026-08-24 (SEC 2.11.6-B CLOSES, ON A HUMAN PLAYTEST AND A USER RULING. Written by the
`coordinator`. The ring and the turn-1a marker were seen DRAWN on screen by the user and they
passed -- the last thing that section was waiting on, and the one thing no clause could ever have
discharged, because `IsGuidedMarkerVisible` reports a component flag and
`GetObjectiveOverlayCount` an instance count, so both can be green on a blank screen. The verdict
is stated in the closing entry at the top of `## NEXT` and this banner does not restate its
evidence.
NO SUITE FIGURE MOVES AND NONE IS CLAIMED HERE. Nothing in this pass touched a test or any source
file; it is a record-only pass. The live figure is the one the block below cites and it is
unchanged.
WHAT THE CLOSURE IS RECORDED OVER, because a closure that swallows an unbuilt surface is how the
surface stops being owed. Sec 2.11.6 specifies a `Skip guidance` control that "kills it instantly
for anyone". `AStratPlayerController::SkipGuidance` exists, is `BlueprintCallable`, refreshes in
the same frame and is pinned by a clause -- and HAS NO CALLER: no input binding, no widget button,
and no asset under `Content/` names it. Its own header says "for the button that will eventually
call it". Measured in this pass, raised to the user, and the user ruled the section closed
regardless. OWNED: `strat-editor-builder`, one button on one already-reflected verb.
[DISCHARGED 2026-08-24, later the same day -- see the newest banner block above and
`content.md`. True when written.]
ALSO NOT MEASURED, AND ITS ITEM IS UNTOUCHED: `GuidedMarkerZOffset` is still `150.0f` and the
standing note that it occludes the unit at this camera pitch is neither confirmed nor retracted.
A playtest that passes on what it looked at says nothing about what it did not.
[SUPERSEDED 2026-08-24, LATER THE SAME DAY. BOTH HALVES OF THAT SENTENCE ARE NOW FALSE and the
correction is written where the sentence is, not only where the work was done. The occlusion is
CONFIRMED -- derived rather than eyeballed, off three numbers that only became measurable once the
meshes landed -- and the default is `300.0f`. `StratUnitActor.h`'s own block on the property
carries the derivation and this line does not restate it. NOT OBSERVED: no eye has been on the new
value, and none could be taken here. True when written.]
[AND NOW OBSERVED. 2026-08-24, later the same day: the editor was reopened, the user looked at the
marker in the running game and ruled it FINE, TO BE KEPT. The derivation is no longer the only
thing standing behind this default.]
AND THE SECOND ITEM FILED BESIDE IT WAS ALREADY DONE, WHICH IS WORTH MORE THAN THE FIX WOULD HAVE
BEEN. `content.md` still files, as item (1) of that pair, that `StratBoardActor.h` and
`StratUnitActor.h` name `BP_StratBoardActor` / `BP_StratUnitActor` -- assets that never existed --
in the very sentence that ASSIGNS work to the content lane. The user asked for it to be fixed on
2026-08-24 and THERE WAS NOTHING TO FIX: both names are ABSENT from `Source/` (zero hits), and the
correct `BP_StratBoard` / `BP_StratUnit` sit at exactly the two sites the filing describes. It was
done on 2026-08-23, in the comment-only pass `engine.md` records. **SO THAT FILING WENT STALE BY
UNDER-CLAIMING, AND THIS RECORD ALREADY KNOWS THAT IS THE DIRECTION THAT COSTS MOST:** an
over-claim gets caught by the next measurement, an under-claim sends the next reader to redo
finished work and looks like diligence while doing it.
THE DEFECT CLASS WAS THEN CLOSED WHOLE RATHER THAN AT THE TWO NAMES FILED, and the subject set was
DERIVED, never typed -- a guard that types its own subject list is the failure this record has paid
for before. Every `BP_`/`WBP_` token in `StratPlay`, `StratUI` and `StratBridge` was extracted and
checked against the 1876 real asset filenames: NINE referenced, NINE resolve, ZERO missing. Every
`/Game/` path named in those modules was resolved the same way: SIX named, SIX resolve. **There is
no second instance of this defect anywhere in our source.**
TWO INSTRUMENT FAULTS FOUND WHILE MEASURING IT, BOTH OF WHICH WOULD HAVE MANUFACTURED FINDINGS.
(1) `grep -roE 'BP_[A-Za-z0-9_]+'` reported `BP_DirectiveStrip` TEN TIMES and `BP_ProductionMenu`
NINE, none of which exist -- because both are SUBSTRINGS of the `WBP_` assets that do. An
extraction without a word boundary invents names. (2) The `BP_`-prefixed tokens in
`Source/Stratocracy/Variant_Strategy` and `Variant_TwinStick` -- `BP_Damaged`, `BP_UnitSelected`,
`BP_CursorFeedback` and nine more -- are `BlueprintImplementableEvent` FUNCTION names in UE's own
template sample code, where `BP_` is a calling convention and not an asset reference. **READ THE
SITE, NOT THE HIT COUNT**; a count alone would have filed twelve fabricated defects here.
OWNED: `strat-editor-builder` -- `content.md` is its file and the stale item is still standing in
it. This pass did not write `content.md`: the fallback condition is `execute_script` being absent
from the session, and it was PRESENT and used throughout. NOTHING IN SOURCE WAS EDITED FOR THIS
ITEM, so no build was owed and none is claimed.
THE STAMP THAT WENT WITH IT. The `[HALF-ANSWERED 2026-08-23]` block further down held three
sentences that had gone stale by UNDER-claiming -- the assets "ship UNSET", the defaults are
"still owed", and "nobody has seen a ring or a marker on a screen". All three were true when
written and all three are now false. Stamped as a SIBLING after that block, never nested inside
it, which is the dangling-closer trap `content.md` records. Nothing deleted, no measurement
withdrawn.)

_Last run 2026-08-24 (A HUMAN PLAYTEST FOUND A LATCHED MARKER, AND THE RECORD IT SAT IN WAS
REPAIRED. This block exists partly because
`content.md` now carries an entry dated 2026-08-24 and this banner may not be older than any date
in the set -- not because a milestone moved. Written by the `coordinator`.
WHAT WAS FIXED. A stamp written on 2026-08-23 had been placed INSIDE an already-bracketed
`[SUPERSEDED ...]` block in `content.md`, which left that block's closer stranded alone on a line
24 lines below the text it closes and made the four `**` toggles render the whole passage wrong.
`strat-editor-builder` repaired it in its own file -- the coordinator fallback was NOT invoked,
because `execute_script` was present in the session's tool surface. It un-nested rather than
deleted: the outer closer moved onto its own last sentence, the bullet's stranded closing sentence
moved back under the headline it belongs to, and a `[NESTING REPAIRED 2026-08-24]` stamp records
the trap. `content.md` carries the detail and this banner does not restate it.
THE TRAP, because the sweep does not catch it: stamping a correction INSIDE an already-bracketed
block leaves a dangling closer. Stamp a correction as a SIBLING after the block it corrects and let
it say "the stamp above"; never nest one.
THE WRITE ROUTE WAS THE HAZARD, NOT THE EDIT. The content lane holds no Bash and no Write tool, so
its only route into a 58 KB CRLF record file is the NeoStack Lua file API -- whose `write_file`
emits UTF-16LE with a BOM even at `encoding="utf8"` while reporting the SOURCE length as
`bytes_written`, and whose `read_file` silently strips CR and silently truncates at 500 lines out
of 670 while reporting the full total. The splice was done inside Lua over base64 at
`encoding="binary"`, with the untouched head and tail carried as literal base64 substrings rather
than re-encoded. The coordinator re-measured the result independently of the lane's report and got
the same four figures the lane gave: 59378 bytes, 678 lines, 678 CR, 678 LF, first byte `0x23` and
no BOM, and zero matches for a lone closer alone on its own line anywhere in the file.
THE HUMAN PLAYTEST HAPPENED, AND IT FOUND A DEFECT THE SUITE COULD NOT HAVE SEEN. Beat 2 retired
LIVE on the shipped scenario, twice in two runs, on its SECOND arm -- `pip=false, held=true` --
which is the arm `4ee36dd` was written for and which nothing had ever observed running. The
objective ring cleared with it. THE TURN-1a MARKER DID NOT, and it never would have: the player
would have carried it for the rest of the match.
THE CAUSE WAS A CONJUNCTION WITH NO FALSE IN IT. `AStratUnitActor::ApplyUnitView` set the marker
from `View.bIsGuidedMarked && View.Side == ViewingSide`, and BOTH operands are constant for the
whole match -- `bIsGuidedMarked` is the rules module's derivation off `placement` and not off
`hex`, deliberately and permanently, because beat 1a's whole content is that the marked Infantry
MOVES. So the expression could never go false once true. The writer set it in both directions on
every refresh, exactly as its own comment demanded; the discipline was applied to the WRITE and
never to the OPERAND SET. That shape is worth more here than the instance.
THE FIX IS THE GDD'S OWN GATE. Sec 2.11.6 says the ring "and the turn-1a unit marker clear in the
same frame as the strip", and the strip IS `FStratGuidanceView::bActive`, so the missing operand
was `Model.Guidance.bActive` -- not "beat 1a retired", which would clear the mark one beat early.
`strat-gameplay-engineer` landed it as a fourth parameter `bool bGuidanceActive` and REFUSED the
`FStratGuidanceView&` shape that was the other candidate: the struct puts `ObjectiveHex` within
reach of the marker writer, which is the hex-keyed derivation that writer's own block forbids by
name. What the class cannot see it cannot use. `engine.md` carries the detail.
TWO FALSE SENTENCES WENT WITH IT, AND THE COORDINATOR'S DIAGNOSIS FOUND ONLY ONE. `SkipGuidance`
asserted in its body that "the marker is drawn only while guidance runs" -- a mechanism no code
implemented -- and its DECLARATION in `StratGuidedOpening.h` implied `bIsGuidedMarked` falls with
`bActive`, which it does not and must not. The lane found the second and said so. A brief that
names one instance of a wrong belief does not bound how many copies the tree holds.
WHY THE SUITE STAYED GREEN THROUGH ALL OF IT. The one clause on the marker measured it only inside
a frame where guidance was ACTIVE; nothing anywhere asserted it goes OUT. **The suite at that time was 201/201**, every entry Success, zero failed and zero
notRun. [SUPERSEDED 2026-08-25 BY WAVE 2 -- the live figure is the one this file's
newest banner carries, and it is 213. The sentence above was true when written and is
stamped rather than deleted. The tense word is placed hard against the figure on
purpose: this record's sweep reads `was` as a supersession marker only when nothing --
not even a bold marker -- sits between it and the number, and positional supersession
alone has hidden a live figure here before.] The delta is +3 and nothing was
removed, measured by set-difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST` walked over `Source/`
and re-derived by the coordinator independently of the lane's report. `Saved/SaveGames/` was
enumerated empty before and after the run.
AND ONE PREVIOUSLY-GREEN CLAUSE WENT RED, CORRECTLY. `GuidedMarkerFollowsTheMarkedBitAndNotTheHex`
never armed a guided opening, so the new operand was false throughout it and six assertions fell.
The lane confirmed the failure on identical bytes before diagnosing, repaired the missing PREMISE
rather than the assertions, and moved none of them. A clause that passes because its subject was
never switched on is a clause that was measuring its fixture.
FALSIFIABILITY WAS MEASURED AND ITS LIMIT WAS REPORTED BY THE LANE ITSELF. The pre-fix predicate
was reinstated in the instrument and reddened exactly the three new clauses and no others, each at
its TERMINAL assertion, with the ring reading `1 -> 0` while the markers read `1 -> 1` in the same
frame -- the human's report reproduced inside a clause. The shipped bytes were restored from a copy
held outside the repo and proved identical by `git hash-object`, never by `git checkout --`. THE
LIMIT: breaking the REAL subject would have meant editing `StratUnitActor.cpp`, which that lane may
not touch even temporarily, so the mutation lives in the test's own helper and these clauses are
pinned against an instrument rather than against the production writer. Filed, not resolved.
WHAT IS STILL NOT CLAIMED. NOBODY HAS SEEN THE MARKER CLEAR. The fix is green in the suite and
unobserved on a screen, and the limit that made the first playtest necessary still applies --
injected input never reaches `UPlayerInput`, so no agent can take that observation.
`IsGuidedMarkerVisible` reports the visible FLAG and not pixels, so no headless clause can ever
close it. Nothing in this pass is committed. The standing `VERDICT: PASS` describes a tree without
any of it, and the narrow re-gate is owed.
**[SUPERSEDED 2026-08-24, LATER THE SAME DAY, BY A SECOND HUMAN PLAYTEST -- ITS TWO OBSERVATION
CLAIMS ONLY. The player ran the guided opening to completion twice more and watched the instant it
ends: THE MARKER WENT OUT WITH THE RING, and THE GUIDANCE STRIP DISAPPEARED IN THE SAME FRAME. So
the fourth operand is observed in flight and not merely green in a suite, and Sec 2.11.6's "clear
in the same frame as the strip" is witnessed for all three surfaces at once. THE LOG AND THE EYE
PROVE DIFFERENT HALVES OF THAT, and the distinction is the point: the coordinator measured
`Guided opening complete on turn 3: every beat retired.` at 15.57.47 and 15.58.38 UTC, which
proves the runs REACHED the instant `bActive` falls -- the all-beats-retired branch, one of the
three that drops it. The three surfaces going dark together is the player's eye, and nothing in
this tree can corroborate that half. WHAT SURVIVES UNCHANGED, and must not be swept up:
`IsGuidedMarkerVisible` reports the visible FLAG and not pixels, so the suite still cannot gate
any of this and no clause became possible -- what changed is that the only instrument that could
ever see it was used. The three clauses stay pinned against a test-side instrument rather than the
production writer, per `tests.md`. And the two NON-observation sentences of the paragraph above
still stand exactly as written: nothing is committed, and the re-gate is owed.]**
**[BOTH SENTENCES IN THIS FILE ABOUT WHAT THE CLAUSES READ ARE RETRACTED, 2026-08-24, by the
`coordinator`, on a `strat-integration-reviewer` finding. THIS FILE ASSERTED THE FALSE CLAIM TWICE,
AND ONE OF THEM WAS WRITTEN IN THIS SAME PASS -- paragraphs after dispatching another lane to
retract it from ITS file. The originals stand above; both are quoted here so the retraction is
greppable from either.
RETRACTED> "the mutation lives in the test's own helper and these clauses are pinned against an
instrument rather than against the production writer. Filed, not resolved."
RETRACTED> "The three clauses stay pinned against a test-side instrument rather than the production
writer, per `tests.md`."
THE CLAUSES READ THE PRODUCTION WRITER. `GuidedMarkerClearsWhenGuidanceDeactivates` calls
`Match.Subsystem->ApplyView(Dark)` -- the real subsystem, and the sole caller of
`AStratUnitActor::ApplyUnitView` -- then reads markers through `LitMarkerIds`, whose body is
`Actor->IsGuidedMarkerVisible()` over the live reconciled actors. It pins as FATAL premises that
`bIsGuidedMarked && Side == ViewingSide` is STILL TRUE in the dark frame and that
`Guidance.bActive` is the only field that moved, then asserts no marker is lit. Frame one asserts
the marker IS lit and is the ONLY one lit, so the dark frame cannot pass vacuously. Delete the
fourth operand and that clause goes RED. The debt this file filed as "not resolved" does not exist.
THE CITATION WAS WRONG TOO, AND THAT IS THE WORSE HALF. `tests.md` was named as the source and
does not make the claim: it scopes the limit to the MUTATION -- each broken predicate simulated by
SUBSTITUTING THE INSTRUMENT -- which is how falsifiability was PROVED, not what the clauses READ.
That file is clean. The citation attributed this file's own conflation to the one lane that never
made it, which is how a wrong claim acquires a source and stops being questioned.
WHAT SURVIVES, NARROWED THE WAY `engine.md` NARROWED IT: no clause gates THE MARKER REACHED THE
SCREEN, because `IsGuidedMarkerVisible` reports the visible FLAG and not pixels. The flag is
gated; the pixels are not; those were never the same claim, and the over-claim was extending the
second onto the first.
THE SHAPE, AND THIS IS ITS THIRD INSTANCE IN ONE SESSION: a retraction that reaches one file has
not reached the others, and the file most likely to re-assert a withdrawn claim is the one whose
author dispatched the withdrawal. Measured alongside it: a flat `grep` for either sentence above
returns ZERO, because both wrap across lines -- so a claim search that does not flatten whitespace
reads a STANDING FALSE CLAIM as absent, which is the direction that matters.]**
**[SUPERSEDED 2026-08-24, LATER THE SAME DAY, BY "WHAT IS STILL NOT CLAIMED" ABOVE. The human
playtest this paragraph says has not happened HAS happened, and it is what found the marker
defect. Stamped rather than deleted: it was true when written, and the six-path figure is the
tree it described.
WHAT IS NOT CLAIMED HERE. No human has played the guided opening to beat 2 with the ring and
the marker in the tree; beat 2 retiring at the ringed Factory, the ring clearing in the same frame as
the strip, and the marker following a moving unit are all still UNOBSERVED, and injected input
still never reaches `UPlayerInput`. Nothing in this pass or the one below it is committed -- six
paths stand uncommitted in the working tree. The standing `VERDICT: PASS` describes a tree without
any of it, so the narrow re-gate the block below owes is owed by this block too.]**
ONE OBSERVATION, FILED RATHER THAN FIXED. The block below cites `reportCreatedOn
2026.08.23-22.37.25` as its own run, but `Saved/AutomationReport/index.json` on disk is
`2026.08.23-23.48.14`, mtime 2026-08-23 19:48 local -- a later run that overwrote the earlier
report. The counts agree and no figure is false; what is gone is the artifact the block points at.
The sweep pins the report IT reads, but nothing pins a banner's CITATION to that report, so this is
the shape of its own check 4 one level up. For whoever next runs the suite: cite the run on disk.
RESOLVED FOR THIS PASS, and this is the citation: the live figure above is the run
`reportCreatedOn 2026.08.24-14.41.49`, which IS the report on disk.)

_Last run 2026-08-23 (A MATCH THAT ENDS NOW ENDS THE GAME. The most serious thing open on this
project is fixed, linked, executed and pinned. The delegation `IStratAiTurnPort`'s header made --
"whether this side should be playing at all is decided before `RunTurn` is called, by the subsystem
that has the view model" -- finally has a receiver.
WHAT WAS BUILT, and it is `strat-gameplay-engineer`'s; `engine.md` carries it and this banner does
not restate it. A concluded match runs a one-shot `UStratMatchSubsystem::ConcludeMatchIfEnded`; the
human's input path and both build paths go inert behind `StratMatchAcceptsPlayerCommands`, a pure
free function over the `FStratViewModel` THE CALLER ALREADY BUILT rather than a second model built
at a second instant; and `RunAiTurnsNow` re-reads the model after a refused turn and reclassifies a
post-conclusion refusal as an ordinary end instead of a fault. Four source files, no `Tests/`, no
`Content/`, no vendored byte.
THE CALL THAT SHAPED IT WAS A REFUSAL. `IsMatchOver()` was NOT added to `IStratAiTurnPort` -- that
interface's own header forbids a runner that decides, and adding it would have obliged every test
double in a lane the engineer may not edit to grow an arm before the tree would build at all.
**THE SUITE WAS 198/198 AT THAT PASS.** Zero non-Success, zero `succeededWithWarnings` and zero `notRun`
across all 198 entries, every one `Success`. The delta is +4 and nothing was removed, measured by
set-difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST` walked over `Source/`, column-0 anchored, and
re-derived by the coordinator independently of the lane reports rather than copied from them. The
four newest clauses are two under `T-UI-03` -- beat 2's second retirement arm and the control that
pins its narrowing -- and two under `T-UI-02`, for the objective ring and the turn-1a marker.
`Saved/SaveGames/` was enumerated empty before and after the run, with the directory mtime moving
to the run's own minute, so that control is this pass's rather than borrowed.
THE FIGURES THIS BANNER CARRIED EARLIER ON 2026-08-23 **were 194/194** and, before that,
**189/189**; both are superseded here rather than deleted, and each was correct for the tree it
described. The 194 pass recorded +5 over 189 for the match-end clauses and reported
`reportCreatedOn 2026.08.23-20.13.33`; this pass's own run is
`reportCreatedOn 2026.08.23-22.37.25` (UTC; 18:37:25 local the same day).
A NOTE ON WHY THIS PARAGRAPH IS SHAPED LIKE THIS, because the next person to edit it will
otherwise undo it. `strat_banner_sweep.py` judges stampedness on a **220-character window** around
each figure, and `reportCreatedOn` is an explicit stamp marker that OUTRANKS the live verb. A
banner that writes "is now N/N" within 220 characters of a `reportCreatedOn` is read as history in
present tense -- the sweep reported `0 live, 21 stamped` on exactly that shape, meaning the record
carried NO live suite count at all while still passing. Keep the live figure and the report stamps
in separate sentences, well apart. `Saved/SaveGames/` held ZERO files before and after and
the directory mtime moved to the run's own minute, so the save-slot control is this pass's rather
than borrowed.
A GREEN BUILD WAS REFUSED AS EVIDENCE, and that is the methodological point of this pass. The first
plain `Build.bat` linked in two actions off objects an earlier `-NoHotReloadFromIDE` diagnostic run
had cached -- green, and describing a compile nobody performed. The four sources were `touch`ed to
force a real nineteen-action rebuild, with `git hash-object` before and after proving all four
blobs byte-identical across the touch. The green build reported here is that second run.
THE TRANSITION WAS OBSERVED EXECUTING, AND ITS FIRST INSTRUMENT WAS BLIND. `STRAT-MATCH concluded
turn=5 turnCap=20 sideToMove=1 tier=Decisive` and five more, inside the run's own window. It greps
to ZERO in `suite.log` -- which reads as the transition never firing until you notice `LogStratPlay`
greps to zero in that file too, so its silence proves nothing about any `LogStratPlay` string. The
engine log is `Saved/Logs/Stratocracy.log`; 6 lines against a 2909-line `LogStratPlay` control,
counted by the coordinator as well as the lane. **`suite.log` cannot answer a `LogStratPlay`
question on this project** and that is worth more than the count.
THE LATCH IS NOW A COUNT AND NOT AN ARGUMENT. It was corroborated by magnitude -- 6 concluded lines
against 73 turn-ends and 967 applied commands -- which is an argument, not an enumeration.
`T-INT-05.ConcludingLogsOnceAndOnlyOnce` pins exactly one across five further presentation
refreshes off a live `GLog` capture. The two latches are visibly independent: 6 `STRAT-MATCH
concluded` against 5 `now records a completed match`, with the slot still empty, so
§2.11.6's writer stayed gated on its own opt-in while the transition ran regardless. **The
save-suppression side effect that silently blanked the guided opening for three runs is closed.**
FALSIFIABILITY WAS MEASURED, NOT ASSERTED: five simultaneous mutations, all inside `Tests/`,
reddened exactly those five clauses and no others, with the shipped bytes restored from a copy held
OUTSIDE the repo and proved identical by `git hash-object` (`4e4c460f…` either side) rather than by
`git checkout --`, which on this project rewrites line endings. One honest limit the lane stated
itself: the latch mutation simulates no real broken implementation, because the latch is production
state a test may not remove.
WHAT IS NOT CLOSED, AND IS NOT CLAIMED HERE. **No human has played a match to a result with this
fix in the tree.** **[SUPERSEDED 2026-08-23, LATER THE SAME DAY: one has. The gate was observed
refusing live on a human's won match -- see the `## NEXT` stamp on the match-end entry for the
three log lines. Two of the three call sites are witnessed; the production-menu build path is not.
Stamped rather than deleted: this was true when the banner was written.]** That a finished match
stops accepting clicks ON SCREEN rests on the input gate's
source and on three CALL SITES that no clause reaches -- a transient world never begins play, so
`HandleSelectionEvent`, `ToggleProductionMenu`'s open branch and `SubmitProductionChoice` are
unreachable headlessly, and **a controller that stopped calling the gate would pass all five new
clauses.** That needs a human at the keyboard, unchanged. Also open and filed rather than fixed:
nothing downstream can say WHO won, because `strat::UiMatchView` carries no winning side for
`FStratMatchView` to mirror.
A NARROW RE-GATE IS OWED. The standing `VERDICT: PASS` describes a tree without any of this.)

_Last run 2026-08-23 (THE STRIP PAINTS. The question the section had been narrowed to -- what the
binding function RETURNS rather than whether it runs -- is answered, the defect it named is fixed
in the asset, and the screen was re-measured against this record's own baseline.
WHAT THE DEFECT WAS. All six of this project's UMG property-binding functions were returning their
type's default, because no exec wire ran from the `K2Node_FunctionEntry` to the
`K2Node_FunctionResult` in any of them. **[CORRECTED 2026-08-23 afternoon: this sentence read
"Each was authored IMPURE, so the data wire..." and the six are in fact PURE -- measured, and
`content.md`'s topmost CORRECTION bullet carries the control. Purity does not exempt a function
body from needing an exec wire, so the missing wire, the defect and the fix are all unchanged;
only the word IMPURE was wrong.]** The data wire that three
sessions had verified as "connected" fed a return node that was never reached. An empty `FText` is
what painted zero-width, and `ESlateVisibility`'s zero value being `Visible` is why the strip
painted a visible empty box instead of collapsing -- one cause under both halves of what was seen.
Six wires, two assets, no source file touched. `content.md` carries the assets, the API findings
and the full measurements; this banner does not restate them.
CAUSATION WAS MEASURED, NOT INFERRED. The first wire went onto `GetStripVisibility` ALONE and
flipped it `Visible` -> `Collapsed` in the same call in which the still-unwired `GetDirectiveText`
returned empty as the live negative control, with the planted model read back identical either
side. Both visibility functions then moved in BOTH directions on the model value, and a vary-text
control shows the text functions track their input rather than returning one cached answer.
THE PIXEL RESULT, AND THE INSTRUMENT WAS CONTROLLED BEFORE IT WAS TRUSTED. In the same 2544x1320
frame the strip rectangle went from x 750-805, w=56, h=38, 2128 border pixels and ZERO glyph
pixels, to x 434-1121, w=688, h=38, 22298 border pixels and 3017 glyph pixels -- same height, same
centre 777.5, grown symmetrically around it. The measuring script reproduces the recorded baseline
digit for digit from the retained capture before it was believed on the new one.
**SEC 2.11.6-B IS NOT CLOSED AND IS NOT CLAIMED HERE.** Only BEAT 1a was observed -- the directive
present at match start, which needs no input. Beats 1b and 2, the window-end tag and the gated
end-turn hovers all require actual play, and injected input still never reaches `UPlayerInput`
here, so what closes the section is a human-driven playtest, unchanged. The glyph count is a SHAPE
claim; nothing read the painted text back.
NO SUITE FIGURE MOVES AND NONE IS MEASURED HERE. No source file changed this pass and the suite was
not re-run; the standing figure is the one the 2026-08-22 banner below carries, and this banner
neither restates nor supersedes it.
GATES: NOT RUN, AND THAT IS OWED RATHER THAN OVERLOOKED. **[SUPERSEDED 2026-08-23: THE RE-GATE HAS
SINCE BEEN RUN AND RETURNED `VERDICT: PASS` WITH ZERO FINDINGS.** Twelve standing checks, both guard
scripts re-run CLEAN with their self-tests all correct, all four in-scope hashes re-derived rather
than carried from the brief, and an arithmetic-node scan returning NONE across all three UI packages
-- so the added wires introduced no computation. The `WBP_OneShotTip` byte question the rest of this
paragraph raises was HALF-settled by the gate: it recovered both packages' HEAD bytes from
`.git/lfs/objects/` and proved every probe string absent, twice by independent instruments, but
could not settle the explicitly-set-versus-default-constructed empty `FText` form, because UE
serialises that property by deduplicated FName index and no instrument it had could see it. **That
half is now settled too, and the answer is that the round trip is BYTE-LOSSLESS** -- `content.md`
carries the controlled experiment and the falsifiability check that makes it mean something. The
gate's one non-gating observation concerns the lane-deviation header and is recorded in `content.md`
where that header lives.]** This pass changed two shipped assets --
`Content/UI/WBP_DirectiveStrip.uasset` and `Content/UI/WBP_OneShotTip.uasset` -- after a
`VERDICT: PASS` describing a tree without them, so it needs a narrow re-gate. One thing for that
gate specifically: `WBP_OneShotTip`'s probe wrote ordinary Blueprint variables on the CDO and the
restore is a REPRESENTATIONAL match, not a proved byte match, so its saved bytes may carry more
than the two wires. `WBP_DirectiveStrip` has no such exposure because `Guidance` is `Transient`.
A LANE DEVIATION, RECORDED RATHER THAN HIDDEN: `execute_script` was again absent from the session's
tool surface, so `strat-editor-builder` had no route to the editor and the coordinator drove it
over the editor's own MCP endpoint and wrote `content.md`. Second occurrence, same measured cause;
`content.md`'s own top entry carries it.)

_Last run 2026-08-22 (BOTH DELIVERY DEFECTS ARE FIXED AND PINNED, AND THE STRIP HAS NOW BEEN
WATCHED WITH THE FIX IN THE TREE, AND IT STILL PAINTS NO DIRECTIVE. **[HEADLINE AMENDED
2026-08-22 12:01 local, coordinator, after the PIE session this banner asked for was actually
run. It formerly recorded that the strip HAD NOT BEEN SEEN since the fix; it has now been seen,
and the answer is negative. The delivery half of this banner is CONFIRMED ON A LIVE WIDGET -- the
widget's guidance now arrives fully populated where it used to arrive default -- and the painted
strip did not follow it. The newest `## NEXT` entry carries the session.]**
**[BOTH THE HEADLINE ABOVE AND THE 2026-08-22 AMENDMENT BEFORE IT ARE SUPERSEDED
2026-08-23: THE STRIP NOW PAINTS. It was never watched with the WHOLE fix in the tree -- the
remaining half was in the widget asset, six binding functions returning their type's default
for want of an exec wire. Superseded rather than deleted: every measurement this banner
reports was correct for the tree it described, and the negative result it records is exactly
what separated the C++ from the asset. See the 2026-08-23 banner above.]** Both halves are
load-bearing: the two mechanisms that kept it blank are closed in C++ and held by clauses, and the
screen still shows no directive. **[SECOND HALF SUPERSEDED 2026-08-23 -- see the stamp
above and the banner at the top of this file.]**
**Sec 2.11.6-B IS NOT CLOSED AND IS NOT CLAIMED HERE.** What closes it is a human-driven playtest; the
coordinator PIE session it also named was RUN on 2026-08-22 and returned NO, so it is discharged
rather than owed -- the 08:01-local `## NEXT` entry carries its console measurements.
**[AMENDED 2026-08-22 11:30 local / 15:30 UTC, coordinator, after a SECOND coordinator PIE session.
Sec 2.11.6-B IS STILL NOT CLOSED and this amendment does not close it.** What changed is one rung
below the section: the design-time sentinel experiment the entry above assigned to
`strat-editor-builder` HAS NOW BEEN RUN -- by the coordinator over the editor's own MCP endpoint,
because the proxy served the two-tool surface again and the builder has no Bash. It answers the
fork rather than the section: the binding IS delivering and the bound function supplies a value
that paints zero-width, with the sentinel confirmed present on the live PIE instance as the
positive control, and the widget asset restored to its gated bytes afterwards. **Do not re-dispatch
that experiment** -- the topmost `## NEXT` entry carries it and names the one question left.
Pointer fixed while here: this sentence used to say "the newest `## NEXT` entry", which silently
re-aimed at whatever was inserted next. **And the `12:01 local` stamp in the headline above is
really 12:01 UTC** -- its own capture, `ScreenShot00059.png`, has mtime `08:01:04 -0400` against a
log line reading `12.01.00`, so that session ran FOUR HOURS EARLIER than its stamp reads and this
11:30-local amendment is after it, not before it. Recorded rather than silently rewritten, because
the same mislabel is what makes two entries look out of order.]
**THE SUITE WAS 189/189 ON THIS PASS.** **[SUPERSEDED 2026-08-23 -- the live figure has since
moved and the banner at the top of this file carries it. Stamped rather than deleted: every
measurement below was correct for the tree it described. This entry deliberately does not restate
the new figure, because two live claims in one file is the contradiction the sweep exists to
catch.]** The figure this banner carried earlier on 2026-08-22
**was 181/181**, before that **was 171/171**, and before that **was 160/160**; all three are
superseded here rather than deleted. 181 -> 189 by set-difference on
`IMPLEMENT_SIMPLE_AUTOMATION_TEST` walked over `Source/` -- +8, none removed, column-0 anchored,
and re-derived independently against `HEAD` (`ac77bc2`), which counts exactly 181 because the
eight new clauses live in two files that are not committed yet,
`Source/StratUI/Tests/StratProductionMenuHostClauses.cpp` and
`Source/StratPlay/Tests/StratProductionMenuAffordance.cpp`. Zero non-Success and `notRun` 0 across
189 entries, every one `Success`. The run is THIS tree's own, with the editor closed, and the
report it wrote is `reportCreatedOn 2026.08.23-02.59.58`. **THAT UTC STAMP READS 08-23 WHILE THIS
ENTRY IS DATED 08-22, AND BOTH ARE RIGHT**: 02:59:58 UTC is 22:59:58 local on 2026-08-22, which is
exactly the crossing this record's local-vs-UTC rule exists for -- do not "correct" either one to
match the other. All eight new clauses are present in the report by name and `Success`, four under
`Stratocracy.StratUI` and four under `Stratocracy.StratPlay`, every one carrying `GATE-BUILDMENU`.
`Saved/SaveGames/` holds ZERO files and the directory mtime reads 22:59 local -- the run's own
minute -- so the save-slot control is discharged on this pass rather than borrowed from the last.
WHAT THE TWO FIXES ARE. Defect 2 is fixed AT THE BUILD, NOT AT THE PUSH: `ApplyView`'s
unconditional-push ruling is untouched and `BuildViewModelForPresentation` -- `BuildViewModel` plus
one `ExecuteIfBound` on a new plain, single-cast, weak, unreflected `FStratViewDecorator` -- now
completes every model the subsystem builds for the screen, so a model missing its guidance section
no longer exists rather than being tolerated at the last moment. `BuildViewModel` stays undecorated
on purpose, because the AI loop reads models nobody draws and decorating there would advance a beat
for a question with no screen behind it. Defect 1 is fixed WITHOUT ASSERTING ANY `BeginPlay` ORDER:
the HUD records every value it is handed and replays it from `CreateGuidanceWidget` once the strip
is in the viewport.
A RECORDED RULING WAS INTERPRETED RATHER THAN FOLLOWED, DELIBERATELY AND IN THE OPEN.
`CreateGuidanceWidget` said the strip CANNOT be pushed to at creation because the HUD "has no way to
ask for that without becoming a second thing that runs matches". That forbids a PULL, and a cache of
what you were handed is not one -- it never asks, has one writer, and every later push overwrites
it, so it changes WHEN a value is delivered and never WHICH. The argument is written into the header
rather than assumed, the sentence that was actually wrong is retracted in place, and the gate judged
the reading correct rather than a weakening. If a later Director disagrees, the alternative is a
HUD-readiness delegate with a check-then-subscribe idiom, and it is strictly more machinery.
GATES: `VERDICT: PASS` with zero findings on the branch in slot-1 -- twelve standing checks plus
eight directed ones, including that no `.Build.cs` is in the diff at all, so `StratPlay` gained no
`UMG`/`Slate`/`SlateCore` -- and a re-gate on this merged tree afterwards.
WHAT THE EVIDENCE STILL DOES NOT REACH, stated because the last four entries were each corrected for
reaching too far: the controller's `BeginPlay` registration and `EndPlay` release are NOT pinned by
any clause, because a transient world never begins play, so that one line still rests on the PIE
session of 2026-08-21; and the first-AI-turn clearing was never observed, only inferred from the
call graph. Both are recorded in `tests.md` and `engine.md` by their own authors.)

_Last run 2026-08-21 (THREE CLAUSES LANDED AND THE STRIP WAS SEEN TO DRAW BLANK. Two separate
things in one merge, and neither is the other's evidence.
THE SUITE **WAS 151/151** at that pass, 148 -> 151 by set-difference on
`IMPLEMENT_SIMPLE_AUTOMATION_TEST`
walked over `Source/` -- +3, none removed, column-0 anchored, re-derived independently by the gate
in the merged tree (SIMPLE 151, COMPLEX/_CLASS 0) and never incremented from a brief -- zero
non-Success, `notRun` 0, 151 entries, `Counter({'Success': 151})`. Build `Result: Succeeded`,
`REAL_EXIT=0`; report `reportCreatedOn 2026.08.22-02.46.56`.
DATES IN THIS RECORD ARE LOCAL; `reportCreatedOn` AND THE LOG ARE UTC, and this entry was first
written dated A DAY AHEAD because its date was taken off a UTC stamp. Every commit of this pass is
`2026-08-21 -0400`, and the entry below it dates itself 2026-08-21 while quoting a `reportCreatedOn`
already past UTC midnight -- so local is the convention and the day-ahead dating was wrong.
Corrected in place, and stated rather than silently fixed, because the next writer reads a UTC stamp
too. (The sweep has a BANNER DATE FRESHNESS check that compares the banner's date against entry
dates, so an ISO date written in PROSE trips it -- measured here, which is why this paragraph
describes the wrong date instead of spelling it.)
THE RUN IS THIS TREE'S OWN, and it was made rather than argued for. An earlier draft of this banner
backed the figure on `slot-1`'s report plus whole-tree byte identity; the sweep's REPORT IDENTITY
check refused it, correctly -- this tree's report was older than the clause files it would have been
describing -- and the editor was closed so the build and the suite could run here. That guard is the
reason the number in this banner means what it says. `Saved/SaveGames/` held ZERO files either side
and the directory mtime MOVED, 20:21:06 -> 22:46:55: the control discharged on this tree again, not
borrowed.
WHAT LANDED. `AlterOneField` in `StratGuidanceRouteClauses.cpp` was reinterpreting ANY
`FStructProperty` as an `FIntPoint` while promising to refuse kinds it could not vary -- the
going-inert shape the property-walk control exists to prevent, relocated one level down. It now
gates on `TBaseStructure<FIntPoint>::Get()`. And the two shipped-asset facts the last entry named as
an unpinned gap are pinned: `WBP_DirectiveStrip`'s ancestry and `BP_StratScoreboardHUD`'s non-null
`GuidanceWidgetClass`, both read off the CDO, both checking their own premise first. The author's
two mutation builds showing both directions red are a SELF-REPORT; the restored-bytes identity is
verified (`git hash-object` -> `c0b8693dbb7d2d6874b7b59264ba83fa9b75a3e4`), the mutation runs are
not, and the gate said so.
THE STRIP DRAWS, AND WHAT IT DRAWS IS AN EMPTY BOX THAT IGNORES THE MODEL. Landed separately at
`d0d2e2e`; the account and both defects are under `## NEXT` and are not restated here.
**Sec 2.11.6-B IS NOT CLOSED AND IS NOT CLAIMED.**
GATES: `VERDICT: PASS` with zero findings on the branch in slot-1 (twelve checks), and `VERDICT:
PASS` with zero findings on the merged tree afterwards, because a verdict describes a tree and a
merge is a post-pass change. The post-merge gate's one substantive remark was about how to attribute
a lane run; the run above makes that attribution unnecessary rather than merely careful.)

_Last run 2026-08-21 (THE STRIP IS WIRED END TO END AND HAS NEVER DRAWN. Both halves of that
sentence are load-bearing and the second is the one a later reader will otherwise lose.
`WBP_DirectiveStrip` is reparented onto `UStratGuidanceWidget` and
`BP_StratScoreboardHUD.GuidanceWidgetClass` now names it, so the route from
`FStratViewModel::Guidance` to a widget is complete in every link. NOTHING HAS BEEN OBSERVED ON
SCREEN. Authored by the coordinator over the editor's HTTP endpoint under the standing ruling;
gated and PASSED with zero findings.
WHAT THE EVIDENCE REACHES, AND THE THREE THINGS IT DOES NOT. It proves the property path
resolves, the parent class binds, all four `Return Value` pins are `connected`, and the suite
**was 148/148** across the change. It proves NOTHING about (a) whether the strip's Slate hierarchy
lays out at top centre, (b) whether `AddToViewport` yields a VISIBLE widget for this class, or
(c) whether `StripBorder.Visibility` evaluates to `Visible` on a real frame while a beat holds the
line. A reparent that destroys a variable and re-scopes four bindings is PRECISELY the change
that leaves all of those green and the screen blank. **§2.11.6-B IS NOT CLOSED AND IS NOT CLAIMED
HERE.** It closes when a human-driven playtest has seen a directive on screen -- automated PIE
input cannot do it, because injection never reaches `UPlayerInput`. OWNED: coordinator, to
schedule with the user.
THE REPARENT WAS PROVED AT THE PACKAGE-BYTE LEVEL, NOT BY THE COMPILER'S SILENCE. The gate
diffed the real bytes and found the corroboration this pass had not: `NewVariables` and
`EStratGuidanceBeat::None` BOTH DISAPPEAR -- the local `Guidance` variable AND its serialized
default, so the shadow is genuinely gone rather than hidden -- while `StratGuidanceWidget:Guidance`
APPEARS, which is the member reference re-scoping to the native class. That is byte-level proof
the four bindings resolve to the INHERITED property. `remove_variable`'s "variable not found" was
truthful rather than a miss. Re-measured independently afterwards: `WontTick` True -> False,
`WillTick` False -> True, `NewVariables` True -> False, `StratGuidanceWidget:Guidance` False ->
True.
THE REPARENT FLIPPED THE WIDGET'S TICK PREDICTION AND THIS PASS DID NOT NOTICE. `WBP_DirectiveStrip`
serialized `WontTick` before and serializes `WillTick` now, with `bClassRequiresNativeTick` newly
present -- while `UStratGuidanceWidget` overrides NO `NativeTick` (grepped: the only out-of-line
definition in its `.cpp` is `PushGuidance`). The likely mechanism is the UMG compiler treating any
native `UUserWidget` subclass parent as tick-requiring; THE MECHANISM IS NOT VERIFIED and is
recorded as an inference from serialized bytes. Cost is one per-frame no-op tick on one HUD widget,
negligible -- but it is a real behavioural delta this pass introduced and the next profiling pass
should not have to rediscover why the strip ticks.
`GuidanceZOrder` STAYS AT ITS C++ DEFAULT OF 10 AND IS NOT DERIVABLE FROM THE GDD -- do not cite
§2.11 for it. The GDD carries no z-order language anywhere; what it fixes is GEOMETRY, and it
SEPARATES these two surfaces: §2.11.4 puts the scoreboard TOP-LEFT, §2.11.2 draws the directive
strip TOP CENTRE. They are not designed to overlap, so ordering is not load-bearing for occlusion
in the intended layout. 10-over-0 is right as the SAFE DIRECTION UNDER A LAYOUT NOBODY HAS SEEN:
a full-width strip can overlap a top-left panel at real resolutions whatever the mock-up says, and
§2.11.6's "no beat expires unheard" fails SILENTLY if the line is drawn behind something. Above is
recoverable; behind is a blank teach. It is deliberately NOT overridden on the Blueprint -- the
property is ABSENT from the `.uasset`, which under this tree's own rule is STRONGER evidence that
it equals the C++ default than setting it would be.
NEITHER EDITED ASSET IS PINNED BY ANY CLAUSE, and that is a new gap this pass created. Nothing
asserts `WBP_DirectiveStrip`'s parent is `UStratGuidanceWidget`, and nothing asserts
`BP_StratScoreboardHUD.GuidanceWidgetClass` is non-null; both facts are held by `.uasset` bytes
alone and would regress silently under an editor accident. A CDO-default clause is cheap and would
have caught the pre-existing `None`. CANDIDATE FOR `strat-test-author`, not owed by this pass.
THE SUITE WAS RE-RUN AFTER THE ASSET EDITS RATHER THAN ASSUMED UNAFFECTED, because clauses in this
tree do read `.uasset` bytes: `succeeded 148 / failed 0 / notRun 0`, zero non-Success,
`reportCreatedOn 2026.08.22-00.21.06`. `Saved/SaveGames/` held ZERO files either side and the
directory mtime MOVED, 20:15:10 -> 20:21:06 -- the control discharged a second time on this tree,
not borrowed.
A MEASUREMENT INSTRUMENT WAS WRONG TWICE IN ONE HOUR AND BOTH WOULD HAVE PRODUCED CONFIDENT
FALSEHOODS. Reading pins, `p.linked` DOES NOT EXIST -- the field is `connected` -- so a check
written against it reported ZERO connected bindings immediately after the reparent, which reads
exactly like the reparent silently breaking every binding, and a clean compile would not have
contradicted it because an unconnected return simply yields a default. Separately, `.uasset` is
LFS-FILTERED here, so `git show HEAD:<path>` returns the THREE-LINE POINTER and not the package;
a byte comparison against it disagreed with the gate on two tokens until the bytes were smudged.
`git hash-object` applies the filters and stays trustworthy; `git show` does not. Both times the
gate's reading was correct and this pass's was not.)


_Last run 2026-08-21 (THE GUIDANCE ROUTE LANDED AND THE THREE WIDGETS ARE NO LONGER INERT BY
DESIGN -- though nothing has DRAWN yet, and that distinction is kept below rather than blurred.
The suite **was 148/148** at that pass, 141 -> 148 by set-difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST`
walked over `Source/` -- +7, none removed, derived independently three times and never
incremented -- zero non-Success, `notRun` 0, `reportCreatedOn 2026.08.21-23.56.04`. Build
`Result: Succeeded`, `REAL_EXIT=0`.
WHAT CLOSED THE GAP THE LAST ENTRY NAMED. `UStratGuidanceWidget` (`Source/StratUI/`) is a
`UCLASS(Abstract, Blueprintable)` `UUserWidget` holding `UPROPERTY(BlueprintReadOnly, Transient)
FStratGuidanceView Guidance`, a `BlueprintImplementableEvent` refresh hook, and a NON-REFLECTED
`PushGuidance`. Lifetime is owned by `AStratScoreboardHUD`; the push happens in
`UStratMatchSubsystem::ApplyView`. `GetViewModel()` was NOT made a `UFUNCTION` -- its header
comment is a ruling with a stated reason, not an oversight, and it stands untouched.
TWO ENGINEERING CALLS WERE BETTER THAN THE BRIEF THAT COMMISSIONED THEM, and the gate improved
on one again. Widget lifetime sits on the HUD rather than the subsystem because creating a
widget from StratPlay would have forced `UMG`, `Slate` and `SlateCore` into `StratPlay.Build.cs`;
no `.Build.cs` changed and no arrow moved. And the push is in `ApplyView` rather than
`RefreshPresentation` because the latter is the UNDECORATED path -- the reviewer then traced it
further and found `RefreshPresentation` ITSELF calls `ApplyView`, so a push there would have been
a DOUBLE push on that path, not merely a dead one. The header records the weaker of the two
reasons.
THE ACCEPTANCE ID WAS RULED `T-INT-05`, AGAINST THE ENGINEER'S PROPOSAL, AND THE GATE AGREED
INDEPENDENTLY. `T-UI-02` is *"the UI queries the module and never recomputes movement"* -- a
query-result comparison of the highlight set against `Move.h::reachable`. `T-UI-06` is the beat-1a
AVAILABILITY property, and its own spec warns that a clause reaching past availability *"has
wandered back into `T-UI-02`"*. These seven pin neither: they pin TRANSPORT, which is
`T-INT-05`'s *"rebuild the screen from the view model alone"*. Precedent decided it rather than
taste -- `T-INT-05.LockArisesFromTheGuidanceLayer` ALREADY files guidance-layer state reaching
presentation under this ID, and `T-INT-05.ApplyViewSpawnsMovesAndDestroys` is the direct sibling
of the new `ApplyViewPushesTheModelsGuidance`. Two consequences worth naming: no ID was minted,
nothing went upstream, and the `T-UI-02` PARTIAL FIT recorded above was not deepened by seven
more clauses.
NO CLAUSE TYPES A GUIDANCE FIELD VALUE ANYWHERE. Equality is
`FStratGuidanceView::StaticStruct()->CompareScriptStruct` and every varied shape comes from
`TFieldIterator<FProperty>`, so no `FString`/`FText` comparison appears in any assertion and the
case-insensitivity trap -- which has already produced one clause that could not fail -- cannot
bite. One of the seven is an INSTRUMENT CONTROL: `GuidanceComparisonDistinguishesViews` forces
the comparator to answer NO once per declared field before any sibling relies on it answering
YES, and asserts `FieldsSeen > 0` so an empty iteration cannot pass it vacuously.
THE STANDING SAVE CONTROL IS DISCHARGED ON THIS PASS AND NO LONGER BORROWED. `Saved/SaveGames/`
held ZERO files before and after, and the directory mtime MOVED, 11:54:15 -> 19:56:04, so the
absence was measured here rather than inherited from a neighbouring run. The borrowed 13:56:49
stamp the last two entries carried is retired.
A BRIEF OF MINE WAS WRONG AND IT COST A RED BUILD. I assumed the test-only concrete subclass could
live in `Source/StratPlay/Tests/`. `UMG` is a PRIVATE dependency of StratUI, so StratPlay cannot
name `UStratGuidanceWidget` by include or by link: ~60 `LNK2019` in `Module.StratPlay.gen.cpp.obj`.
The author refused to fix it by editing `StratPlay.Build.cs`, which is outside its lane, and split
the clauses across two modules behind a UMG-free probe instead. The gate confirmed the probe leaks
no UMG type and no `.Build.cs` moved.
TWO RESIDUES, NAMED RATHER THAN DISCOVERED LATER. `AStratScoreboardHUD::PushGuidance`'s literal
SOURCE SHAPE is not pinned: automation cannot read a function body, and a `.cpp` text scan would
flake on a comment edit while passing on an unexpected branch form. What IS pinned is one
distinguishable shape per declared field, each preceded by a clear-to-default push so "unchanged"
cannot be satisfied by a resident value -- but A BRANCH THAT READS A FIELD AND CHANGES NOTHING
SURVIVES IT, and the green tick is not proof the source is field-blind. Separately,
`AlterOneField` reinterprets ANY `FStructProperty` as an `FIntPoint` while its own block promises
to refuse kinds it cannot vary; harmless today because `FIntPoint` is the struct's only such
field, but it is the reflection-driven design's ONE typed assumption and it is the going-inert
failure that design exists to avoid, relocated. Gate it on
`TBaseStructure<FIntPoint>::Get()`. OWED BY `strat-test-author`.
STILL NOT DRAWN, AND THIS IS THE HONEST STATE OF IT. `WBP_DirectiveStrip` is not yet reparented
onto `UStratGuidanceWidget` and `BP_StratScoreboardHUD` has no `GuidanceWidgetClass` set, so the
route compiles, is pinned by seven clauses, and puts nothing on screen. The reparent must DELETE
the Blueprint's own `Guidance` variable rather than rename the C++ property -- renaming would
orphan all 33 bindings -- and a Set node on that variable would fail to compile against the
inherited `BlueprintReadOnly` one. MEASURED BEFOREHAND: `WBP_DirectiveStrip` holds 21 nodes, 4 of
them `Get Guidance` and ZERO `Set`, with the detector controlled by planting a real `Set Guidance`
node in a throwaway widget and confirming it was seen. `CreateGuidanceWidget` and `GuidanceZOrder`
stay unpinned until then, because they end in `AddToViewport` and a `-nullrhi` run has no viewport.
Gated twice, PASS both times, zero findings each: once over the C++ alone, once over the clauses
as a post-pass change.)


_Last run 2026-08-21 (2.11.6'S FIRST THREE SURFACES ARE BUILT AND THE FIRST GATE ON THEM RETURNED
`VERDICT: BLOCK` -- ON THIS VERY RECORD'S ABSENCE, WHICH IS WHY THIS ENTRY EXISTS. `Content/UI/`
gained `WBP_DirectiveStrip`, `WBP_PreMatchBriefing` and `WBP_OneShotTip`, authored by the
coordinator over the editor's own HTTP MCP endpoint because `strat-editor-builder` still cannot
reach it and has no Bash with which to bypass the proxy. The user ruled on that crossing BEFORE any
asset was touched. Recorded here and NOT in `content.md` on the precedent this file already set for
the `BP_StratGameMode` edit: no lane owns a coordinator-authored asset, and `content.md` has a
different sole writer. The reviewer proposed writing both; it named the right facts and the wrong
file, and the lane rule outranks a gate's filing suggestion.
THE GATE PASSED EVERY BYTE-LEVEL AND STRUCTURAL CHECK AND BLOCKED ON PROSE ALONE. Vendored
`Source/StratRules/` and `Data/` compared BY BLOB across all 32 tracked files, zero mismatches;
zero `Source/`, `Config/` and `.uproject` delta; module arrows RE-DERIVED from every
`Source/*/*.Build.cs` and matching; no widget-side arithmetic; nothing staged. Its single finding
was that all three widget names grepped, AT GATE TIME, to ZERO hits across `Tools/`, `.agents/`
and `CLAUDE.md` -- a grep that THIS ENTRY deliberately falsifies, which is the point of it,
so three real debts lived only in conversation. That is the stale-status-line failure in its
worst direction: committing these files would have made the tree assert a guided-opening UI exists
with nothing saying it is inert.
THE THIRD EXPLANATION FOR THE PROXY LATCH IS NOW ALSO DEAD, disproved rather than argued. The
suspect was `NEOSTACK_PROJECT_DIR` pinning at a stale tree. `runtimes.json` holds EXACTLY ONE
runtime -- `editorPid` 49156, `projectPath` `E:/MultiAgent/Stratocracy/` -- so there is no wrong
tree to be pinned to. Better: `list_unreal_projects` READ THAT SAME FILE SUCCESSFULLY and named
Stratocracy in the same round in which `unreal_status` claimed the file contained no active editors.
That is a contradiction INSIDE the proxy between two of its own tools. Three explanations are now
measured false: missing capability, startup order, project pinning.
THE ABSENCE WAS MEASURED WITH A CONTROL, because an absent tool name proves nothing alone. The
`execute_script` lookup failed twice; the SAME keyword search returned `unreal_status` and
`list_unreal_projects`, which is what shows the instrument could speak. Independently the editor's
own endpoint served `serverInfo: unreal-editor 1.0.0-r4254`, exactly one tool, and round-tripped Lua
against that same pid.
`WBP_DirectiveStrip` CARRIES NO AUTHORED DIRECTIVE STRING. It holds one member, `Guidance`, of type
`FStratGuidanceView`, and drives four properties from four PURE functions that break it --
`DirectiveText.Text`, `WindowEndTag.Text`, and the two visibilities off `bActive` and
`bShowsWindowEndTag` through `To Visibility (Boolean)`. §2.11.6-B's four directives stay where
`FStratGuidedOpening::DirectiveTextFor` already puts them.
THAT THE STRUCT WAS THE RIGHT ONE WAS PROVED THREE WAYS, never assumed from an `[OK]`. A
deliberately bogus type (`FStratNoSuchStructXyzzy`) returned `[FAIL] unknown type` where the real
name returned `true`, so the type oracle can fail. Reflection then offered `Break Strat Guidance
View` and `Switch on EStratGuidanceBeat` from inside that Blueprint. And the Break node's pins are
exactly the struct's TEN fields.
`pure=true` IS LOAD-BEARING AND THE COMPILER SAID SO: the first binding attempt failed with
"needs to be bound to a pure function, 'Get Directive Text' is not pure." Quoted rather than
paraphrased, because it is the entire reason those functions are declared as they are.
A CLEAN COMPILE WAS CAUGHT LYING ABOUT WIRING. `WBP_OneShotTip`'s first build reported `0 errors,
0 warnings` while its text getter was connected to NOTHING: a member variable named `TipText`
collided with the widget also named `TipText`, so the getter lookup resolved to the wrong node.
Variables were renamed and every `connect` is now asserted against `true` rather than inferred from
a green compile. EXIT CODE IS NOT A VERDICT, again, this time wearing a compiler's colours.
VERIFICATION WAS READ BACK INDEPENDENTLY AND ITS INSTRUMENT WAS CONTROLLED. All six binding
functions report their `Return Value` pin `linked=true`; a throwaway function with nothing connected
reports `linked=nil`. The six `true`s mean something only because the check can also say nil.
THE THREE WIDGETS ARE INERT AND THAT IS A C++ GAP, NOT AN EDITOR ONE. There is NO Blueprint-reachable
route to `FStratViewModel::Guidance`: `UStratMatchSubsystem::GetViewModel()` is explicitly not a
`UFUNCTION` (the header says so, and says why -- UHT rejects a reference return), `AppliedModel` is
`UPROPERTY(Transient)` with no `BlueprintReadOnly`, and the subsystem is a bare `UCLASS()`. No amount
of editor access would have closed this. It is `strat-gameplay-engineer`'s and it is not discharged
here.
THE TWO SEAMS ARE NOT EQUALLY SOUND, and the gate found the asymmetry that this session's own write-up
had folded flat. Measured on the bytes afterwards rather than taken on report: `WBP_DirectiveStrip`
carries `/Script/StratUI` once and `StratGuidanceView` THIRTY-THREE times, so its member really is the
reflected struct and reparenting to a future C++ base preserves the bindings. `WBP_OneShotTip` carries
`/Script/StratUI` ZERO times -- its `CurrentTip`, `TipIsShowing` and `TipId` are invented
Blueprint-only names with no counterpart anywhere in `Source/` (grep: zero hits). When the engineer
defines the real tip view struct, nothing constrains its field names, so those two bindings will most
likely NOT survive reparenting. The tip widget is a layout and a placeholder seam; the strip is a
wired one.
A CONTENT DEBT, RULED NON-BLOCKING BY THE GATE: §2.11.6-A's three callout strings are typed into
`WBP_PreMatchBriefing`, because `StratGuidedOpening.h` deliberately declares no C++ home for them.
Its stated reason was that "declaring content for a surface that does not exist is how a string
drifts from the GDD unnoticed" -- the surface now EXISTS, which inverts that reasoning. Note the
asymmetry the gate named: §2.11.6-**B**'s directive strings ARE in C++ and are therefore testable,
while A's three now sit in a binary that no grep or clause can compare against the GDD.
NO SUITE RAN THIS PASS AND NONE IS CLAIMED: no C++ changed, so there was nothing to compile and
nothing to re-measure. `Saved/SaveGames/` was likewise not re-controlled this pass, for the same
reason -- the borrowed 13:56:49 stamp from the previous session's run is still borrowed and must be
re-stamped either side of the next real run.)


_Last run 2026-08-21 (THE PASS BELOW WAS GATED AND PASSED, AND BOTH UPSTREAM REQUESTS WERE
FILED. `strat-integration-reviewer` returned `VERDICT: PASS` with ZERO findings over the shipped
opt-in, the new clause, the hardened banner sweep and the coordinator's `Content/` edit. Twelve
checks; vendored `Source/StratRules/` and `Data/` compared by blob rather than by worktree diff.
It confirmed at its source that the fabricated `StratRules -> (nothing)` finding is dead: it
DERIVED the module arrows from every `Source/*/*.Build.cs` and then read
`.agents/ue-project-context.md` before assigning blame -- reading BOTH artifacts, which is the
step that never happened the four times that finding was reported. The note matched the tree
exactly, and the coordinator's independent derivation agreed. THE FIRST GATE SINCE 314ad99 FIXED
THE CHECKLIST, AND IT CAME BACK CLEAN.
ITS ONE NON-GATING CATCH WAS THE COORDINATOR'S OWN LITTER: `build.log` and `suite.log`, which the
documented build and suite templates write to every tree's root on every pass, were untracked AND
un-ignored, so `git add -A` would have committed them. Closed at `6bc9e00`.
BOTH UPSTREAM REQUESTS ARE FILED, ON THE USER'S EXPLICIT INSTRUCTION, AND NEITHER IS GRANTED. The
guided-opening adjacency request landed at crew `a8c8cdb`; the Q27 acceptance-ID request at crew
`3510bc2`. Both spec-only -- `data/` and `cpp_reference/` untouched in each, no manifest or hash
moved, no scenario authored, no acceptance ID minted. Recorded at `b17c359` with the inverse
hazard named for the first time: this tree has long kept FILED IS NOT LANDED, and the REVERSE
costs as much -- a document still reading "not filed" after its request is committed upstream
sends the next reader to file a duplicate. Both evidence files said exactly that, both were true
when written, and neither is now; the superseded text is stamped and quoted, never deleted.
A CORRECTION WENT UPSTREAM WITH THE Q27 FILING RATHER THAN BEING SMOOTHED OVER. Its draft read as
though Q27 ruled all three beat-1a input gates. Read out of the GDD directly: Q27's own row names
END TURN. The beat-1a row is what extends the gate to SPACE, on the stated footing that those are
the machine's only routes from SELECTED to DONE not passing through MOVED, and separately closes
the ATTACK transition. The filed text states the split and the evidence file quotes the request AS
LANDED, because the landed wording and the drafted wording differ for exactly that reason.
THE VENDORED PIN GAP GREW BY THIS SESSION'S OWN WORK and was re-derived rather than left to
stand: `git rev-list --count cb8e12b..HEAD` in the crew repo, measured independently twice, is
now seventeen commits, up from the fifteen the Q27 draft recorded. Both filings are spec-only so
neither creates re-vendor pressure of its own, but Lane A's eventual re-vendor carries all of
them.
WHAT REMAINS NEEDS SOMEBODY THIS SESSION COULD NOT SUPPLY. 2.11.6's unbuilt widgets need an
editor `strat-editor-builder` can actually reach, and it cannot while the `NeoStack_Connect` proxy
latches, because it has no Bash with which to drive the editor's own endpoint -- the workaround
that carried this pass is not available to that agent. Lane A stays blocked until `uiBuildOptions`
is a real header change rather than a spec entry. And both filings now wait on a Director.)

_[STAMPED 2026-08-21, later session — "both filings now wait on a Director" was true when
written and is now HALF false, which is the direction that wastes a reader: the Q27 filing (crew
`3510bc2`) HAS BEEN RULED. The Director minted the ID, and it is `T-UI-06`. The ruling is written
into crew `spec/ui_spec.md` and committed there at `745dfd8` on the user's instruction, which
takes the crew gap over the vendored `rulesCommit` `cb8e12b` to EIGHTEEN commits, re-derived after
the commit rather than incremented from the seventeen recorded above. And
the number was re-enumerated across the GDD, `data/`, `cpp_reference/` and downstream `Source/`
before it was taken, because its only prior occurrence anywhere was the request's own "e.g." and a
grep that stopped there would have called the number taken. TWO HALVES STAY OPEN and are named
rather than assumed done: the shipped GDD still defines only `T-UI-01`—`T-UI-05`, so it does not
yet know the ID; and downstream's clauses still sit under `T-UI-02` as a recorded partial fit,
because re-pointing them is a test-lane edit needing its own build and suite run. The OTHER filing,
crew `a8c8cdb`, is unchanged and still waits on a Director.]_

_Last run 2026-08-21 (THE SHIPPED OPT-IN LANDED AND THE CLAUSE THAT PINS IT IS NO LONGER
OWED. `BP_StratGameMode`'s `MatchConfig` now carries `bRecordCompletionOnMatchEnd` TRUE, so the
packaged game records a completed match and §2.11.6's guided opening retires instead of
re-arming for every player forever. The suite **was 141/141** at that pass, 140 -> 141 by set-difference
on `IMPLEMENT_SIMPLE_AUTOMATION_TEST` walked over `Source/` -- +1, none removed -- zero
non-Success, `notRun` 0. Build `Result: Succeeded`, `REAL_EXIT=0`, and the new translation unit
is named in the build log as `[3/7] Compile StratShippedGameModeOptIn.cpp` rather than assumed
from a green.
THE ASSET DEFAULT WAS READ BEFORE IT WAS WRITTEN, because the trap here inverts the obvious
reading: a property ABSENT from a `.uasset` proves it equals the C++ default, which is FALSE,
WHICH IS THE DEFECT -- so "not found, therefore unchanged, therefore fine" would have reported
the broken state as healthy. Two instruments agreed, each with its own control. The CDO export
carried `AiMaxCommandsPerTurn=256` and `SaveSlotName="StratocracyMatch"` while the asset's name
table carried NEITHER, which is what proves the asset holds only OVERRIDES and the export holds
the EFFECTIVE value; on both readings the flag was unset, hence false. After the write the same
name-table scan shows the flag present and those two still absent, so the whole-struct write did
not materialise inherited fields as overrides -- the instrument that would have shown that harm
is the one that shows its absence.
`BP_StratGameMode_AiVsAi` was deliberately LEFT FALSE and is byte-identical across the pass. An
AI-vs-AI run reaching a result would write the human player's slot, which is the exact failure
the opt-in predicate exists to prevent. The asymmetry is intentional and is not pinned by a
clause yet; a clause requiring FALSE there is a SECOND clause, never a widening of this one.
THE STANDING CHECK PASSED WITH ITS CONTROL: `Saved/SaveGames/` holds ZERO files after the run
and the directory mtime MOVED, 11:17:34 -> 13:56:49, so the absence was measured on this pass
rather than borrowed from a neighbouring one.
THE LANE WAS CROSSED KNOWINGLY AND THE USER RULED ON IT FIRST. `Content/` is
`strat-editor-builder`'s, but the builder had no route to the editor: the `NeoStack_Connect`
proxy exposed exactly two tools all session, and the builder has no Bash with which to reach
the editor's own MCP endpoint. THE STARTUP-ORDER EXPLANATION FOR THAT LATCH IS FALSE, measured
here -- this session STARTED with the editor already up, ~85 minutes old and heartbeating, and
still got the two-tool surface. The latch is in the proxy alone; the editor's own server was
live and complete throughout and served the read, the write, the compile and the save. So the
coordinator made the asset edit, on the user's explicit ruling, and it is recorded here rather
than in `content.md` because no lane owns it.
The banner sweep now PINS THE ARTIFACT IT READS and would have caught the pre-merge report this
entry replaces -- see `data.md`. Its first quiet pass on a real tree with a fresh report is this
one; before the run it failed on both the count and the identity, and both findings cleared.
NOT YET GATED AT THIS WRITING -- no `VERDICT` has been sought for this pass, and this entry
asserts none. **[STAMPED 2026-08-21: it was gated afterwards and PASSED with zero findings; the
entry above this one carries the verdict. That sentence was true when written and is kept as
written.]**)

_Last run 2026-08-21 (THE WRITER WAS GATED `VERDICT: BLOCK` AND THE BLOCK WAS RIGHT. The hook
guarded on `ResolveSaveSlotName(FString()).IsEmpty()` while `FStratMatchConfig::SaveSlotName`
defaults to `TEXT("StratocracyMatch")` -- THE PLAYER'S SLOT, never empty -- so the guard
protected nothing in the one case its own comment named. `StratAiMatchClauses.cpp` sets no slot
and plays an AI match to an asserted result, so EVERY SUITE RUN WROTE THE PLAYER'S SLOT, and
the change inverted its own purpose: it landed to stop the guided opening re-arming, and as
written it PERMANENTLY SUPPRESSED the guided opening after one suite run, the flag persisting
in the slot across runs with no diff to blame. IT HAD ALREADY HAPPENED, and an accidental
two-directional control proved it rather than an argument: the integration tree ran the suite
WITHOUT the writer and its `Saved/SaveGames/` held zero files; slot-1 ran it WITH the writer and
held `StratocracyMatch.sav` at 2096 bytes. Nobody designed that experiment.
THE FIX WAS THE PREDICATE, NOT THE FIXTURE: emptiness cannot distinguish UNSET from CHOSEN, so
a slot name answers WHERE and can never answer WHETHER. `FStratMatchConfig::
bRecordCompletionOnMatchEnd` now gates `NoteMatchResultIfEnded` -- the one writer nobody asked
for -- and defaults FALSE in C++, so fixtures NOBODY HAS WRITTEN YET inherit silence.
`RecordMatchCompletionOnSave` and `SaveMatchToSlot` stay ungated, deliberately: a caller that
named a slot has already chosen, and that partition is what keeps the direct-writer clauses
green as controls for an absence. `TOptional<FString>` was rejected on a measured constraint --
not reflectable as a `UPROPERTY`, and this struct reaches a designer through a details panel.
The suite **was 140/140** at that pass, 133 -> 140 by set-difference on
`IMPLEMENT_SIMPLE_AUTOMATION_TEST`, +7 and none removed, re-derived independently, zero
non-Success. Build `Result: Succeeded`. THE CHECK THAT ACTUALLY PINS THIS is stronger than any
clause and is run by hand: after a full suite run `Saved/SaveGames/` holds ZERO files where the
blocked pass left the player's slot. `TheCompletionHookIsSilentWithoutTheOptIn` now carries
that property into the suite, with its positive control INSIDE THE SAME CLAUSE rather than
borrowed from a neighbour -- a borrowed control is an assumption about another clause's health.
A DEBT SHIPS WITH THIS AND IT IS A LIVE DEFECT, NOT A HYPOTHETICAL: the packaged game now
records nothing unless `BP_StratGameMode`'s `MatchConfig` carries the new flag, which no C++
asserts and no clause yet reads. The failure mode moved from CORRUPTING the player's slot to
FORGETTING the player's history, which §2.11.6 prefers, but forgetting is still wrong.
`T-UI-03.TheShippedGameModeOptsIn` is OWED, recorded in `tests.md`, and deliberately unwritten
because it would be red until an editor pass that cannot happen before this C++ merges.
Not yet merged and not yet re-gated at this writing. The entry that follows is preserved
verbatim: THE §2.11.6 MATCH-COMPLETION WRITER LANDED, and the clauses that can see
it. `UStratMatchSubsystem::RecordMatchCompletionOnSave` is called from a private
`NoteMatchResultIfEnded` at the tail of `ApplyView`, gated on `FStratMatchView::bHasResult`,
with a `bMatchResultRecorded` latch cleared in `StartMatchInternal`. It closes a defect where
`bHasCompletedAMatch` had a READER AND NO WRITER, so the guided opening re-armed on every match
instead of only the first -- measured four times independently before it was fixed. The hook
hangs off `ApplyView` rather than the submit path because §2.9's AI ends a match through
`RunAiTurnsNow` -> `RefreshPresentation`, which a submit-path hook would miss. The suite
**was 138/138** at that pass, +5 clauses and none removed, counted by set-difference on
`IMPLEMENT_SIMPLE_AUTOMATION_TEST` and re-derived independently by the coordinator, with zero
non-Success. Build `Result: Succeeded`. Four of the five clauses pin the WRITER without ever
planting the field, driving a real AI-vs-AI result; the fifth turns the marked Infantry's
attack unreachability from prose inference into a MEASUREMENT, carrying its own control that
shows `AttackTargetHexes` returning non-empty for a unit in contact -- an empty answer proves
nothing until the instrument is shown able to speak. A HAZARD THIS WRITER CREATES IS RECORDED
RATHER THAN LEFT TO BE DISCOVERED: now that a completed match writes to the slot its config
names, ANY fixture that plays to a result is a writer, and a fixture that reached a result on
the slot the guided-opening clauses need ABSENT would have disarmed that whole file silently
and greenly, on that run and every run after. Not yet merged, not yet re-gated at this
writing. The Lane B entry that follows is preserved verbatim: LANE B LANDED -- save-slot I/O and the §2.11.6 guided opening, built in
the `slot-1` worktree on `feat/save-and-guidance` off `870c611` and merged to `master`.
`FStratBridge::RestoreFromSaveText` replays a save onto a COPY and verifies `canonicalStateHash`
before adopting, refusing a mutated hash rather than restoring it; `UStratSaveGame` carries the
slot; `SaveMatchToSlot` and `LoadMatchFromSlot` both route through an extracted
`StartMatchInternal`, so exactly one `LoadDefinitions -> LoadScenarioFromFile -> restore ->
AdoptBridge` order survives instead of two that could drift apart; `FStratGuidedOpening` is the
first shipping caller of `SetLockedThisTurn`. The suite **was 133/133** at that pass, +25 clauses and none
removed, counted by set-difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST` rather than by
acceptance-ID grep, and zero non-Success. Build `Result: Succeeded`. Reviewer `VERDICT: PASS`
with zero findings on the branch. The narrow re-gate of the merged tree plus the three
uncommitted record files then also returned `VERDICT: PASS`, zero findings, 2026-08-21 --
twelve checks, with the vendored bytes compared by git blob hash over 32 files, zero
mismatches, rather than by a text diff. WHEN THIS SENTENCE WAS FIRST WRITTEN IT ASSERTED
THAT RE-GATE IN THE PAST TENSE BEFORE IT HAD RUN. The re-gate itself caught that as a
non-gating observation and it is amended here rather than quietly smoothed. It happened to
come out PASS, which is why it was worth catching at all: a pre-declared gate outcome is
unfalsifiable at the moment of writing, whichever way it later lands. Beat 2 of the
guided opening retires ONLY on a capture pip at `guidedOpening.objective`, following the
2026-08-21 USER RULING and not a reading of §2.11.6, which stays ambiguous -- see
`decisions.md`. TWO ACCEPTANCE IDS WERE REASSIGNED AND NONE MINTED: the guided-opening lookup
moves from `T-SCN-02` to `T-SCN-07`, because its subject is the two authored `guidedOpening`
fields and every guided-opening refusal in `Source/StratRules/Scenario.good.cpp` carries
`T-SCN-07` and no other id; and the Q27 input gates are filed under `T-UI-02`, on the precedent
`T-UI-02.ControllerTicksSoInputDispatches` and `T-UI-02.BoardHexRoundTrip` already set in this
tree for the StratPlay input surface. THE SECOND ONE IS A PARTIAL FIT AND IS RECORDED AS ONE:
`T-UI-02`'s written invariant is the reachable-hex highlight, and the GDD ships no acceptance id
at all for §2.11.6-B's one input-gating constraint -- there is no onboarding family among
T-AI/CAP/COMBAT/DATA/FAME/HEX/INT/MOVE/REPAIR/SAVE/SCN/TURN/UI, enumerated over the GDD rather
than recalled. The missing id is FILED UPSTREAM, not minted here. The 2026-08-20 entry that
follows is preserved verbatim: TOOLING ONLY -- no C++, no editor, and no suite run: three pass-on-failure
sites in this project's own pre-commit hook, committed at `95d955b` and `ebbe20d`. Nothing in that
pass re-measures the suite, so every suite figure in this banner still describes `185e88f` and is
unchanged. See "Pre-commit hook" at the end of this file. The 2026-08-19 entry that follows is
preserved verbatim: post-milestone work: the `chooseBuild` cap RULED and FILED upstream at
crew `85995b8`; the content-independence SCENARIO axis FILED upstream against the same crew HEAD
as a change request in `spec/scenario_spec.md` — filing is not closing, and that axis is STILL
OPEN; ten rotted line-number citations replaced by function-and-branch citations; the
content-independence corpus DISCHARGED on two of three axes at `185e88f`, gated `VERDICT: BLOCK`
then `VERDICT: PASS` after this banner itself was found contradicting the entry below it — the
defect that produced `strat_banner_sweep.py`. Suite 107 -> 108. The milestone narrative that
follows is unchanged and still describes 2026-08-14: log-backed combat outcome milestone: phase 1 CLOSED — the `STRAT-COMBAT`
emitter on `FStratBridge::Submit`, gated three times, zero findings, plus an unplanned 1-in-4 test
flake in a `GLog` capture found and fixed on source-level proof, not a probe; phase 2 CLOSED,
`VERDICT: PASS`, zero findings — six new clauses in `StratCombatOutcomeParity.cpp`, suite 87 → 93,
including the first-ever execution of the `adied=1` counter-kill arm; phase 3 CLOSED
MET-AS-CORRECTED, gated THREE TIMES: first two `VERDICT: BLOCK` (a fixture-count miscount in this
file; then this file's OWN mid-file summary line still claiming "no reviewer verdict yet" after
this same file's phase-3 entry already recorded the first `BLOCK` — the other half of the exact
mistake phase D's entry already documents paying for once), both fixed in place; third
`VERDICT: PASS`, zero findings, 2026-08-14 — the brief's `STRAT-CMD accepted` ↔ `STRAT-COMBAT
resolved` pairing is unreachable in an
AI-vs-AI match (measured 0), the real pairing is `STRAT-AI applied kind=Attack` ↔ `STRAT-COMBAT
resolved` by ordered identity, proven 68/68 with zero mismatches by
`Tools/architect/strat_combat_pairing_gate.py`, suite 93 → 103; phase 4 CLOSED MET, gated once,
`VERDICT: PASS`, zero findings, 2026-08-14 — the only phase this milestone that did not need a
re-gate — the live PIE path (`Saved/Logs/Stratocracy_2.log`, `--pre-sliced` mode added to the
gate for its first non-automation corpus), same gate PASS, 68/68 again, and all eleven
`STRAT-AI turn-ended` hashes byte-identical to phase 3's headless run — the phase-D "avoid
one-corpus proof" precedent is now PARTLY discharged: the HOST-independence half IS discharged,
the CONTENT-independence half is NOT and is carried forward as its own future work **[BANNER
CORRECTED 2026-08-19: the content half is now discharged on two of its three axes at `185e88f` —
first side and buildlist moved, a second corpus graded 42/42 with zero mismatches; the SCENARIO
axis alone remains open. This clause described the state at phase 4 and is corrected here rather
than deleted]**; phase 5
CLOSED (editor closed throughout), the doc pass, gated three times, all `VERDICT: PASS`, zero
findings each — 9 files, 231 insertions/23 deletions vs `HEAD` `ae2f22a`, proven comment-only
(254 changed lines, zero fail the comment-body shape test; `/*`/`*/` counts identical HEAD-vs-
worktree in all nine files), suite unmoved at 103/103, and the standing finding that
`strat::chooseBuild`'s cheapest-affordable selection makes a buildlist's repeated entries
inert — a vendored-behaviour question for `stratocracy-crew`, not a defect here. **THE
LOG-BACKED COMBAT OUTCOME MILESTONE IS NOW COMPLETE — five phases closed, no phase 6 was ever
planned for it.** See "Log-backed combat outcome milestone" below. **POST-MILESTONE, 2026-08-14,
NOT A PHASE:** `a2d370a` discharged the deferred `ESearchCase::CaseSensitive` tightening on the
T-SAVE-05 grep-contract clause and opened a narrower `STRAT-CMD refused`-shape residual in its
place; suite is now **104/104** (was 103/103 as of phase 5, above). See "Grep-contract
case-sensitivity tightening" below. **ALSO POST-MILESTONE, 2026-08-14, NOT A PHASE:** the
`--pre-sliced` zero-event gate debt (phase 5's item 4, carried in NEXT) is DISCHARGED —
`Tools/architect/strat_combat_pairing_gate.py` now refuses a zero-applied/zero-resolved
`--pre-sliced` corpus by default and accepts an explicit `--expect-min-pairs N` floor (any N,
including 0, as an opt-out of the default); a 14th in-script self-test case proves the guard is
falsifiable, and a new 11th checked-in fixture (`fixtures/fail_pre_sliced_zero_events.log`, empty)
demonstrates it against the actual defect. This is a Python-only change — no `IMPLEMENT_SIMPLE_
AUTOMATION_TEST` clause, so the 104/104 C++ suite count is unmoved. See "Pre-sliced zero-event
guard" below. **ALSO POST-MILESTONE, 2026-08-14, NOT A PHASE:** the `STRAT-CMD refused`
grep-contract residual that the previous item opened is now DISCHARGED — two new clauses,
`T-SAVE-05.StratCmdRefusedLineShape` and `T-SAVE-05.GrepContractRejectsARefusedCaseVariant`,
close it the same way the `accepted` side was closed; suite **was 106/106** at that pass (**was 104/104**).
See "STRAT-CMD refused grep-contract residual" below. **ALSO POST-MILESTONE, 2026-08-14, NOT A
PHASE, this steward's own lane:** the fixture-verdict binding debt (open since phase 3, carried
through the pre-sliced zero-event guard entry and the grep-contract residual entry above, both
of which left it untouched) is now DISCHARGED. `Tools/architect/strat_fixture_verdict_binding.py`
(new) re-runnably binds all 11 checked-in `.log` fixtures under `evidence/08-combat-pairing-gate/
fixtures/` to their expected verdict AND their exact invocation, and is shown falsifiable by two
deliberate breaks, both reverted. `fixtures/README.md` (new) documents the `--test-path` /
`--pre-sliced` split in the fixtures directory itself, closing the second, smaller item. This
pass also found and fixed staleness in two checked-in artifacts the debt predicted would go
stale silently: `gate_self_test_output.txt` (10 entries, missing
`fail_pre_sliced_zero_events.log` — confirmed by `grep -c "^== "` against `git ls-files`'s 11)
and `self_test_internal_fixtures_output.txt` (11 entries against the in-script `--self-test`'s
actual 14), both regenerated from a real run; no Python or C++ suite count changes (Python-and-
doc-only work). See "Fixture-verdict binding" below. **ALSO POST-MILESTONE, 2026-08-14, NOT A
PHASE, this steward's own lane — three corrections, all found by the wave-1 reviewer gate as
non-gating observations:** (1) this header's own suite count was stale at 106/106; re-derived
directly against `Saved/AutomationReport/index.json` (read `utf-8-sig`, `reportCreatedOn
2026.08.14-21.47.35`, `succeeded 107 / succeededWithWarnings 0 / failed 0 / notRun 0`, 107
entries, zero non-Success) and corroborated by an independent macro census
(`IMPLEMENT_SIMPLE_AUTOMATION_TEST` sums to 107, `IMPLEMENT_COMPLEX_AUTOMATION_TEST`/`_CLASS`
sums to 0) — suite **was 107/107** at that pass (**was 106/106**) and **was 108/108** as of
`185e88f`, the content-independence clause, SUPERSEDED 2026-08-21 by Lane B and stamped
rather than deleted, per this file's own convention — re-derived both ways, `succeeded 108 / failed 0 /
notRun 0` in `Saved/AutomationReport/index.json` and an independent macro census summing to 108
with COMPLEX at 0. (2) A correction bullet this steward wrote
in the prior pass asserted `T-INT-05.AlreadyActedGuardFiresOnAForeignModel` was still open; it is
not — `Source/StratPlay/Tests/StratSelectionForeignModelClauses.cpp` (untracked) ran green in the
same report above, having landed mid-wave, after that pass. Corrections appended in place at all
three prior mentions (the "Four items carried out of..." NEXT bullet, the "STRAT-CMD refused
grep-contract residual" section, and the "Fixture-verdict binding" section) rather than rewritten
— each was true when written. The other two items those same sentences named,
`chooseBuild`'s buildlist-ratio question and the content-independence corpus, have moved
differently and are kept distinct: `chooseBuild` is now investigated and written up at
`Tools/architect/evidence/upstream-chooseBuild-buildlist-ratio.md` (a request draft awaiting a
Director ruling, **not discharged**); content-independence remains genuinely open. **[STAMPED 2026-08-19 — BOTH halves of this sentence have since moved, and it is stamped rather than rewritten because it was true when written: `chooseBuild` is RULED (per-type population cap) and FILED upstream at crew `85995b8`; content-independence is DISCHARGED on two of its three axes at `185e88f`, the scenario axis alone still open. See the two NEXT bullets dated 2026-08-19.]** (3) The
fixture-count derivation command recorded in this file and in
`evidence/08-combat-pairing-gate/blackboard.md` — `git ls-files fixtures/ | wc -l` — measures
tracked files in the directory, not `.log` fixtures; it read 11 only because this pass's own
`fixtures/README.md` was still untracked, and would read 12 the moment that file is committed
while the `.log` count stays 11. Both prose copies corrected to `git ls-files
'.../fixtures/*.log' | wc -l`, which reads **11** both now and after a hypothetical commit of the
README (verified: `git ls-files --cached --others --exclude-standard '.../fixtures/*.log'` — the
proxy for post-commit tracked state — still returns 11, while the same query without the `*.log`
glob returns 12). See "Fixture-verdict binding" below for the full account. The prior
entry (AI-opponent milestone, phase D CLOSED, COMPLETE) is preserved under "AI-opponent milestone"
further down. **ALSO 2026-08-20, TOOLING ONLY, THIS STEWARD'S OWN LANE -- and it is the
reason this banner now lives in a different file:** the 3,524-line `state.md` is SPLIT into
`Tools/architect/state/`, one file per owning agent, and `state.md` is frozen as the closed-phase
history. The sweep that polices the record now DERIVES its file list from that directory instead of
naming `state.md`, compares live claims ACROSS the set rather than file by file, and enforces that
only this file may carry a live suite count. Two defects were fixed in it and both were shown
falsifiable against a control running HEAD's own bytes: a suite claim ending in a full stop was
invisible to its regex, and the pre-commit hook's typed path meant a commit touching only
`state/tests.md` drew ZERO bytes of output and exit 0. `install.sh` no longer dies in a linked
worktree, and the `strat-hotseat` skill's claim that a new worktree is unprotected is corrected --
it is protected, through the shared common directory. `.claude/skills/strat-parallel/SKILL.md` is
new and carries the worktree topology, the seven concurrency rules, per-tree command templates that
derive every path from that tree's own `git rev-parse --show-toplevel`, and the merge protocol.
**NO SUITE RUN AND NO C++ IN THIS PASS** -- every suite figure above still describes `185e88f` and
is unchanged; the only build was a re-verification that the `slot-1` worktree still compiles
(`Result: Succeeded`, `REAL_EXIT=0`, 82 of 82 actions, editor closed).)_

## BUILT

- `Source/StratUI/StratScoreboardHUD.h` — applied (iteration 1, step `host_h`, live)
- `Source/StratUI/StratScoreboardHUD.cpp` — applied (iteration 1, step `host_cpp`, live)
- `FStratBridge::Reachable` — landed at `e0cc53d` with zero tests; its five clauses are now
  covered (`StratBridgeQueryParity.cpp`, T-UI-02, phase 1, 2026-08-12). Debt discharged.
## NEXT

- **2026-08-27, COORDINATOR -- USER RULING, WIDENED THE SAME DAY ON THE GDD'S OWN Sec 2.11.2:
  WAVE 2'S FOUR SNAPSHOT MIRRORS RIDE `T-UI-05`, AND ITS FIVE TABLE-AND-PRESENTATION READOUTS
  RIDE NOTHING AND LAND UNPINNED. NO ID IS MINTED HERE, AND AN UPSTREAM MINT IS FILED. RECORD-ONLY; NO SOURCE, NO TEST, NO ASSET AND NO CONFIG FILE WAS TOUCHED; NO SUITE WAS
  RUN AND NO SUITE FIGURE MOVES -- the live figure is the one this file's topmost banner cites.
  NO PHASE VERDICT IS STATED HERE and nothing below has been through a gate.**
  W2 (the info panel, and with it per-unit HP) had no acceptance ID, and a clause may not be named
  without one -- a proposed identifier greps exactly like a defined one, which is a failure this
  record has already been caught by. This entry is written FLAT, because a correction that only
  adds leaves both claims live and a nested one has already let a false sentence survive directly
  above its own retraction in this file.
  - **STRUCK, AND IT WAS THE COORDINATOR'S OWN WORDING BEFORE THE GDD WAS READ:** `The
    per-unit HP stat line rides T-UI-05. The terrain name, the move cost and the defense bonus
    ride NOTHING and land unpinned`. **That is not false, it is INCOMPLETE -- it enumerates four
    of W2's nine readouts and is silent on the other five**, and a ruling that is silent on an
    item ships it unpinned without saying so. It is quoted here rather than left standing,
    because a correction that only adds leaves both claims live.
  - **THE RULING AS IT STANDS, WIDENED 2026-08-27 BY USER RULING, AND IT IS THE WHOLE PANEL.**
    Four readouts ride `T-UI-05`: the **per-unit HP** pair, the hex's **capturable status**
    (`strat::UiHexView::owner`), and the **flag marker** (`strat::UiUnitView::isFlag`). Five ride
    NOTHING and land **unpinned**: the **terrain name**, the **move cost**, the **defense bonus**,
    the unit's **Atk/Def/Move/Range**, and the **`ready`/`done`** bit. The upstream mint filed
    against `stratocracy-content` covers them. This ruling covers W2 alone and widens no other
    wave.
  - **HOW THE WIDENING WAS CAUSED, and it was a defect in the dispatch rather than in the tree.**
    The first ruling was taken from THIS FILE's own wave-plan entry, whose W2 bullet reads `a hex
    info struct (terrain name, move cost, defense bonus) and a unit stat line`. The GDD's Sec
    2.11.2 **Info panel** specifies more than that summary carries -- a fourth hex readout
    (`status if capturable`, rendered `Factory - move 1 - def +15% - yours (+100/turn)`), and a
    unit line of `name, HP as 12/20, Atk/Def/Move/Range, and ready or done` plus the flag unit's
    `FLAG -- its loss ends the match.` **The wave plan is a summary and the GDD is the
    specification; the summary was read in the specification's place.** This is the recorded
    failure that a dispatch brief is not ground truth, and it was the `coordinator`'s. It was
    found by `strat-data-steward` while filing the mint request, from the GDD itself.
  - **AND THE FOUR THAT RIDE `T-UI-05` ARE ALREADY PINNED, so W2 mints no `T-UI-05` clause at
    all.** `Source/StratUI/Tests/StratViewModelParity.cpp` already asserts all four against the
    module: `.Hp == UiUnitView::hp`, `.HpMax == UiUnitView::hpMax`, `.bIsFlag ==
    UiUnitView::isFlag` and `.Owner == UiHexView::owner`. **This is a good outcome, not a
    gap** -- the ruling's `T-UI-05` half is satisfied by clauses that already exist and already
    run, and the wave adds a route to a screen rather than new state. It is recorded because an
    unstated `already covered` reads exactly like an oversight to the next author, who would
    otherwise write a second clause asserting what the first one does.
  - **WHY THE FOUR THAT RIDE `T-UI-05`, RIDE IT.** `strat::UiUnitView::hp`, `::hpMax`, `::isFlag`
    and `strat::UiHexView::owner` are UNMARKED fields of the
    snapshot, which by `Ui.h`'s own two-kinds accounting makes each a MIRROR, and both are
    enumerated by `uiEnumerateSnapshot`, which is what clause (a) quantifies over. `T-UI-05`'s
    clause (a) is `every unmarked field equals
    the module-side value it names, exactly, with nothing widened, narrowed, rounded or
    reordered`. So all four are squarely its subject, and by a shorter argument than W3's income
    line needed: that one had to be shown a snapshot field first, while these four are mirrors on
    the face of the stub. `owner` and `isFlag` are ALREADY PROJECTED on this side, as
    `FStratHexView::Owner` and `FStratUnitView::bIsFlag`, so this half of the wave is a route to
    a screen rather than new state.
  - **WHY THE FIVE THAT DO NOT, CANNOT, and each is refused by a different text.** (1) THE THREE
    TERRAIN READOUTS: `T-UI-05` quantifies over `uiEnumerateSnapshot`, and NO snapshot field
    carries a terrain name, a move cost or a defense bonus -- `strat::UiHexView` carries
    `terrainId`, an index, and the three are §4.8 TerrainDef rows reached THROUGH it. This tree
    says so about the name already: `FStratHexView::TerrainId`'s own comment calls it `THE ONE
    FIELD HERE THAT IS NOT A SNAPSHOT MIRROR`. (2) ATK/DEF/MOVE/RANGE are §2.4 `strat::UnitDef`
    rows and are refused for the same reason, one table over. (3) THE `ready`/`done` BIT is
    refused not by inference but by `Ui.h`'s own words: the PRESENTATION BLOCK's header states
    `It is NOT in T-UI-05's subject`, because its members `have no module-side counterpart and no
    derivation from one, so there is nothing for it to compare them against`. **This is the same
    wall W3's idle count hit, for the same reason and in the same block** -- W3's ruling is the
    precedent and this one does not re-argue it. (4) `T-UI-03` is `the live standings scoreboard (§2.11, §2.8)
    binds 1:1 to snapshot fields -- enemy strength destroyed, objectives held X/N, surviving
    units/HP, turn vs cap -- with no widget-side arithmetic`. Its enumeration is four SIDE-level
    readouts and no per-hex one. A clause riding an ID whose own text excludes its subject is
    unfalsifiable by construction.
  - **THE TRAP ON THIS WAVE IS NAMED, because it reads the other way at a glance.** `T-UI-03`'s
    `surviving units/HP` looks like it covers a unit HP readout and does not: it names
    `strat::UiSideView::survivingHp`, the side aggregate, which `FStratSideView::SurvivingHp`
    already mirrors. Anyone re-deriving this ruling will meet that phrase first.
  - **STRUCK 2026-08-27, SAME DAY, ON A CENSUS OF THE TREE'S OWN CLAUSE NAMES:** `The hex info
    panel's terrain name, move cost and defense bonus, the unit's Atk/Def/Move/Range, and the
    ready/done bit all ship with no clause`. **`UNPINNED` DOES NOT MEAN `UNTESTED` IN THIS
    PROJECT, and this entry asserted that it did.** The sentence is quoted rather than left
    standing, because a correction that only adds leaves both claims live.
  - **WHAT LANDS UNPINNED IS NAMED, AND IT IS AN ID GAP AND NOT A TEST GAP.** Those five readouts
    carry no ACCEPTANCE ID. **They ARE covered, by `GATE-INFOPANEL` clauses that live on
    `feat/info-panel` and NOT YET IN THIS TREE -- the census two sentences below is of `master`
    and correctly shows no `GATE-INFOPANEL` among its `GATE-` names.** The two statements are
    about different trees and the tension closes when the lane merges; it is stamped rather than
    smoothed, because a record that asserts present-tense coverage `master` does not hold is the
    shape a later reader takes as fact. `GATE-` naming is this
    project's standing convention for a surface with no ID and NOT a new practice: a multiline
    census of `IMPLEMENT_SIMPLE_AUTOMATION_TEST` over `Source/**/Tests/` returns 249 clause names,
    of which 38 already ride a locally-minted `GATE-` name -- `GATE-BUILDMENU` 33 across three
    modules, `GATE-BRIDGE-DEFS` 3, `GATE-SAVE-PARSE` 1, `GATE-DATA-VENDOR` 1. **`GATE-BUILDMENU`
    is the exact precedent**: §2.11.5's production menu had W2's problem -- a whole surface with
    no acceptance ID -- and was covered this way rather than shipped bare. A `GATE-` name mints no
    acceptance ID and claims none, so it does not touch the ruling above or pre-empt the upstream
    mint. **The gap that remains is real and narrower than stated before: these five are not
    graded by the GDD's acceptance set.** `T-UI-03`'s no-widget-side-arithmetic rule still BINDS the implementation
    -- those values must reach the view model already resolved from their tables, the
    shape `FStratBridge::Forecast` already uses for `DefenderTerrainDefensePct` and
    `DefenderTerrainId` -- but a rule that binds an implementation is not an ID that pins a clause,
    and this entry does not let the one stand in for the other.
  - **STILL UNRULED: W4, W5, W6 and W8.** Each is the user's to rule the same way, or to mint
    upstream.


- **2026-08-27, COORDINATOR -- W3 LANDS: SEC 2.11.2'S PERSISTENT HUD HAS A MODEL SIDE, AND A TURN
  CAN NOW BE ENDED BY SOMETHING OTHER THAN A KEY.** Lane commit on `feat/persistent-hud` in
  `E:/MultiAgent/Strat-wt/slot-2`; the banner above carries this branch's figure, and the count is
  re-derived on `master` in the merge commit. Gate: `VERDICT: PASS`, zero findings, first round.
  - **WHAT EXISTS NOW.** `UStratViewModelLibrary` -- four `BlueprintPure` selectors over
    `FStratViewModel`, giving `FStratSideView::IncomePerTurn` its FIRST reader in the project and
    the idle count a named home; a six-argument `StratBuildProductionMenu` returning
    `OutPurseFame`, with the five-argument form kept as a one-line forward;
    `UStratMatchSubsystem::ProductionMenuFameTotal`; and `AStratPlayerController::RequestEndTurn`.
    **NO WIDGET AND NO `.uasset`** -- the HUD and command bar are a later lane on the integration
    tree.
  - **THE FIVE CLAUSES.**
    `Stratocracy.StratUI.T-UI-04.ReturnedPurseIsTheOneEveryShortfallWasComputedAgainst`,
    `Stratocracy.StratUI.T-UI-04.TheFiveArgumentFormIsTheSixArgumentFormMinusThePurse`,
    `Stratocracy.StratPlay.T-UI-04.PublishedHeaderPurseIsTheOpenMenusPurseAndZeroWhenClosed`,
    `Stratocracy.StratUI.T-UI-05.SideSelectorReturnsTheModelsRowAndRefusesOutOfRange`,
    `Stratocracy.StratUI.T-UI-05.ViewingSideSelectorTracksTheViewerAndNotTheSideToMove`.
  - **TWO SURFACES SHIP DELIBERATELY UNPINNED, and the gap is named rather than hidden.**
    `AStratPlayerController::RequestEndTurn` and
    `UStratViewModelLibrary::CountUnitsAbleToAct` / `CountViewingSideUnitsAbleToAct` carry NO
    clause. The reason is the acceptance-ID ruling recorded below, and the upstream mint is filed
    in `decisions.md`. **A missing clause for those three is the ruling working, not an
    oversight** -- and each new test file names its own omission in its header block.
  - **THE DESIGN CALL WORTH KEEPING: the idle count is a function OVER the model, not a field ON
    it.** A `UnitsAbleToActCount` field would have been the first arithmetic inside
    `FStratViewModel` and the first entry in `StratViewModelParity`'s walk with no snapshot field
    to check against -- a number that looks pinned and is not. The gate verified the consequence
    rather than the intention: `StratViewModelParity.cpp` is unmodified and reaches
    `UStratViewModelLibrary` nowhere, so the walk's subject is unchanged.
  - **THE PURSE IS AN OUTPUT RATHER THAN A SECOND READ, because it must ride the rows' own
    clock.** A separately-refreshed model can print `Fame: 250` above a 275-cost row reading
    `need 50`, each correct at a different instant. The gate confirmed the guarantee is
    structural: one `FameTotal` local in the function, every `Shortfall` computed against it,
    `OutPurseFame` assigned from that same local, and no second read of the purse in the body.
  - **A MEASUREMENT FOUND WHILE WRITING A FIXTURE, worth recording because it is not what a
    reader would assume:** `StratBuildProductionMenu` at a hex that is NO FACTORY does not
    refuse -- it answers with `bAvailable` false rows. The refusal channel is reserved for a
    malformed question, such as a side outside the match. A clause's refusal arm was empty
    because of it and went red on its own vacuity guard, which is the guard doing its job.
  - **AND A FIXTURE THAT COULD NOT HAVE FAILED WAS CAUGHT BEFORE IT SHIPPED.**
    `ferrum_crossing.json` seeds `startingFame` 200/200 on a symmetric board, so at turn 1 the two
    side rows are identical and the viewer clause would have passed whichever row it returned.
    The fixture now has side 0 spend Fame on a real build first, and the clause ASSERTS the
    divergence before relying on it.
  - **CARRY INTO THE CONTENT LANE, from the gate's own non-gating observations.** (1)
    `FStratViewModel::Sides[ViewingSide].FameTotal` is still `BlueprintReadOnly` beside
    `ProductionMenuFameTotal`, so a HUD Blueprint can still bind the stale purse the new output
    exists to prevent -- the prose warns, nothing structural forbids. (2) `GetSideView`'s refusal
    is a bare return bool, and a caller that ignores it gets a default `FStratSideView` reading
    `Fame: 0`, the precise outcome its own header says must never reach the screen; an
    `ExpandBoolAsExecs` meta on the return value would make that branch unskippable.
- **2026-08-27, COORDINATOR -- W1 LANDS: SEC 2.11.3'S ATTACK FORECAST HAS A MODEL SIDE, AND THE
  SECTION THE GDD CALLS THE CENTREPIECE IS NO LONGER ABSENT FROM THIS ENGINE.** Lane commit on
  `feat/forecast-card` in `E:/MultiAgent/Strat-wt/slot-1`; the banner above carries this branch's
  figure and provenance, and the count is re-derived on `master` in the merge commit. The gate
  returned `VERDICT: PASS` with zero findings on its FIRST round.
  - **WHAT EXISTS NOW.** One new bridge surface, `FStratBridge::AttackForecast`, gathering all six
    of the card's readouts; a reflected `FStratForecastView` with a three-way `EStratCounterReason`;
    `StratComposeForecastView` in `StratUI`; and `StratDecorateForecast` in the new
    `Source/StratPlay/StratForecastQuery.h`/`.cpp`, wired into
    `AStratPlayerController::DecorateForPresentation` AFTER the hover. **NO WIDGET AND NO
    `.uasset` -- the asset tail is a separate, later lane on the integration tree.**
  - **THE THIRTEEN CLAUSES, all under `T-UI-01` except one, and none minted.**
    `Stratocracy.StratBridge.T-UI-01.ForecastCardHpAfterIsTheOracles`,
    `.ForecastCardRepeatsTheModulesForecast`, `.ForecastLegalityIsTheLitTargetSet`,
    `.ForecastTerrainBonusIsTheDefenderRows`;
    `Stratocracy.StratBridge.T-FAME-01.ForecastKillAwardIsTheModulesAward`;
    `Stratocracy.StratUI.T-UI-01.AttackerHpRowFollowsTheCounterDamage`,
    `.CounterReasonMatchesTheDefendersBand`, `.FlagBandRaisesForEitherFlag`,
    `.IllegalForecastComposesADefaultCard`, `.TerrainPenaltyReachesTheCardSigned`;
    `Stratocracy.StratPlay.T-UI-01.ForecastComposesTheHoveredHex`,
    `.ForecastDecoratorWritesUnconditionally`, `.ForecastRefusesASpentUnit`.
    **W1 RIDES `T-UI-01` LEGITIMATELY -- it IS the forecast ID -- BUT THE STRETCH IS STILL STATED
    RATHER THAN HIDDEN**, as W0's ruling required: `T-UI-01`'s own GDD sentence is about the
    forecast EQUALLING the resolution, and a clause about the counter-reason mapping or the flag
    band asserts no such equality. Each such clause says so in its own comment. **Six clauses the
    engineer proposed under `T-UI-03` and `T-INT-05` were RENAMED to `T-UI-01` by the
    `coordinator` before they were written:** `T-UI-03`'s subject is the standings scoreboard and
    its `no widget-side arithmetic` sentence is a design rule cited across the GDD, not a licence
    to name forecast clauses under it.
  - **THE SHARPEST CLAUSE IS `CounterReasonMatchesTheDefendersBand`, and the reason is the shape
    of the defect it catches.** The three-way counter reason is a MAPPING over two module
    booleans, not a combat formula, so a later rules change could make the card say `out of range`
    about a defender that is in range **while every number on the card stayed correct** -- no
    compiler and no numeric parity gate would see it. The clause asserts the reason against the
    defender's own `[rangeMin, rangeMax]` band read off the projection, NEVER against the two
    booleans the mapping was chosen from. Its mutation produced exactly that sentence: `the card
    says 'out of range' at distance 1, which is INSIDE unit 10's own band [1, 1]`.
  - **A DISPATCH INSTRUCTION WAS REFUSED AND THE ENGINEER WAS RIGHT, which is recorded because
    the `coordinator` wrote the instruction.** The brief said to bridge `strat::uiResolveForGate`
    for the HP before -> after readouts. It is `T-UI-01`'s INDEPENDENT ORACLE:
    `StratCombatOutcomeParity.cpp`'s header says it `has zero production callers in this tree BY
    CONSTRUCTION` and `must stay uncalled from production`, because a bridge call `would make
    every comparison below a comparison of the bridge with itself`. The mention of it inside
    `StratBridge.cpp` is a PROHIBITION, not a note that it has not been wrapped yet, and reading
    it off a grep hit rather than at its site is how the instruction was written. The clamp is
    written out once inside `FStratBridge::AttackForecast` instead and discharged by a clause that
    calls the oracle from `Tests/`; the gate confirmed ZERO production code callers survive.
  - **FOUR DEBTS WERE NAMED BY THE ENGINEER. TWO ARE DISCHARGED IN THIS SAME TREE**, by
    `ForecastCardHpAfterIsTheOracles` and `CounterReasonMatchesTheDefendersBand`. **TWO REMAIN
    OPEN AND ARE STATED AS OPEN.** (3) the `bHasActed` gate in `StratDecorateForecast` duplicates
    `FStratSelectionMachine`'s guard -- two copies of `has this unit spent its act` -- discharged
    the day the machine exposes that predicate or a clause asserts both refuse the same unit.
    (4) `bFlagAtRisk` WHEN BOTH FLAGS FALL: Sec 2.11.3 describes one band and gives no rule for
    the pair. The engineer chose the DEFENDER's side and said so at the code site rather than
    leaving it to branch order. **That one is a design ruling for the user, not a defect.**
  - **TWO GAPS ARE DECLARED RATHER THAN PAPERED OVER.** The flat-500 flag award is UNEXERCISED
    through `AttackForecast` on a real board -- 0 of 64 lethal forecasts swept are against a flag
    -- so the clause reports it in an `AddInfo` instead of implying coverage, and proves the
    `isFlag` argument load-bearing by control. Closing it needs a scenario reaching a flag kill,
    and `Data/` is read-only, so it is a filing rather than a change. And **no clause pins the
    decorator's PLACEMENT** in `DecorateForPresentation`, only that its ordering constraint is
    real; pinning the call site would need a seam that does not exist and none was invented.
  - **WHAT IS OWED NEXT.** (1) The forecast card's Widget Blueprint, on the integration tree, in
    the editor. (2) A user ruling on debt (4). (3) `engine.md` carries a conditional sentence
    whose condition was met inside this same tree -- non-gating, named by the gate, and `tests.md`
    records the discharge.
- **2026-08-27, COORDINATOR -- USER RULING, AMENDED THE SAME DAY ON EVIDENCE FROM `Ui.h` ITSELF:
  WAVE 3'S FOUR READOUTS SPLIT ACROSS TWO EXISTING IDS AND TWO UNPINNED ITEMS. NO ID IS MINTED
  HERE, AND AN UPSTREAM MINT IS FILED. RECORD-ONLY; NO CODE, NO ASSET, NO TEST AND NO SUITE RUN.**
  W3 (the persistent HUD, Sec 2.11.2) had no acceptance ID, and a clause may not be named without
  one -- a proposed identifier greps exactly like a defined one, which is a failure this record
  has already been caught by.
  **THIS ENTRY IS WRITTEN FLAT AND ITS FIRST WORDING IS QUOTED RATHER THAN LEFT STANDING**, because
  a correction that only ADDS leaves both claims live and a NESTED one has already let a false
  sentence survive directly above its own retraction in this file.
  - **STRUCK, and it was the coordinator's own recommendation before the evidence was read:**
    `The +X/turn income line, the End Turn surface and the idle count ride T-UI-05`, on the ground
    that `the income line, the End Turn surface and the idle count are each a module-held field
    displayed truthfully`. **That is false for two of the three**, and `Ui.h` says so in its own
    words rather than by inference.
  - **THE RULING AS IT STANDS.** The Fame **purse** rides `T-UI-04`. The **`+X/turn` income line**
    rides `T-UI-05`. The **End Turn surface** and the **idle count** ride NOTHING and land
    **unpinned**, and an upstream mint is filed against `stratocracy-content` for an ID covering
    Sec 2.11.2's HUD verbs and per-unit presentation state.
  - **WHY THE TWO THAT HOLD, HOLD.** `T-UI-04`'s own GDD sentence is `the production menu binds to
    the buildlist derived from the four Stub-2 unit rows plus current fameTotal` -- the purse IS
    `fameTotal`, named in the ID's own text, so that half is not a stretch at all. The income rate
    is a SNAPSHOT FIELD by `Ui.h`'s own accounting, which lists it among the members `RULED on
    2026-08-04` and says `The first two are snapshot fields below`; `T-UI-05` asks whether the
    snapshot tells the truth about the state the module holds, so the income line is squarely its
    subject.
  - **WHY THE TWO THAT DO NOT, CANNOT, and the header refuses them by name.** The idle count is
    derived from `FStratUnitView::bDone` and `FStratUnitView::bLockedThisTurn`
    (`Source/StratUI/StratViewModel.h`), which are the UE-side mirrors of `done` and
    `lockedThisTurn` in `Ui.h`'s PRESENTATION BLOCK (`strat::UiPresentationUnit`).
    **[CORRECTED 2026-08-27, SAME DAY, BY `strat-data-steward` DURING THE UPSTREAM FILING, AND
    STATED FLAT RATHER THAN NESTED. STRUCK: `bDone` and `bLockedThisTurn`, which are members of
    `Ui.h`'s PRESENTATION BLOCK. WHAT IS TRUE: those two names appear ZERO times in `Ui.h` -- the
    module spells them `done` and `lockedThisTurn`, and the `b`-prefixed pair is
    `FStratUnitView`'s. THE RULING IS UNAFFECTED, because the exclusion quoted below is a sentence
    about the block itself and not about either spelling; what was wrong was the OWNERSHIP, and an
    ownership claim that does not survive a grep of the file it names is worth striking even when
    it changes no outcome. The coordinator wrote it; the steward caught it while verifying its
    dispatch rather than carrying it forward.]** That block's own header says: `It is NOT in T-UI-05's subject. That invariant asks whether
    the snapshot tells the truth about the module's state; these members have no module-side
    counterpart and no derivation from one, so there is nothing for it to compare them against.`
    `UiSnapshot` carries no idle count and, by that ruling, could not. The End Turn surface is a
    COMMAND PATH and not a snapshot field at all, so the same invariant does not reach it either.
    A clause riding an ID whose own text excludes its subject is unfalsifiable by construction.
  - **HOW THIS WAS CAUGHT, because the route matters more than the answer.** `strat-gameplay-engineer`
    disputed the ruling in its lane report rather than complying with it, and the premises it cited
    were then measured against `Ui.h` before the dispute was accepted. The record's standing rule
    -- verify a crew agent's objection rather than overriding it, and rather than taking it -- paid
    here.
  - **WHAT LANDS UNPINNED IS NAMED, not left for a reader to discover.**
    `AStratPlayerController::RequestEndTurn` and the idle-count selector ship with no clause. That
    is a real coverage gap and it is stated as one.
  - **STILL UNRULED: W2, W4, W5, W6 and W8.** Each is the user's to rule the same way, or to mint
    upstream. This ruling covers W3 alone and widens nothing.

- **2026-08-27, COORDINATOR -- W0'S HOVER NOW REACHES THE GAME. THE INPUT ROUTE WAS THE DEFECT,
  IT IS REPLACED, AND A HUMAN HAS SEEN IT WORK.** The entry below records the playtest that found
  wave 0's hover dead; this one records the diagnosis being settled by instrumentation, the route
  being replaced, and the replacement being witnessed. **[STAMPED 2026-08-27, LATER: the figure in the next
  sentence was live at that pass and is not now -- the suite is 231/231 and the live provenance is
  this file's topmost banner. It is stamped here, where it stands, and not only in that banner.]**
  **THE SUITE FIGURE AT THAT PASS WAS 230/230**, re-run and
  unchanged, every entry Success, zero failed, zero notRun, zero succeededWithWarnings. **[The
  tense was moved onto the figure itself on 2026-08-27: the stamp above this sentence did not
  make the sweep read it as history, because positional supersession alone does not -- the word
  that dates a figure has to TOUCH it. The live figure is 231/231 in this file's topmost banner.]** **[CORRECTED 2026-08-27, AND REWRITTEN FLAT ON THE SECOND GATE
  FINDING RATHER THAN NESTED A THIRD TIME -- the nesting is what let a false outer layer survive
  directly above its own retraction, so the whole block is restated once, correctly, and the
  superseded wordings are quoted rather than left standing as sentences. TWO WORDINGS ARE STRUCK.
  (1) `the independent confirmation that the diagnostic Warning added mid-pass is gone`. (2) `The
  field read zero BEFORE the instrumentation, WITH it, and after it`. WHAT IS TRUE: the field
  confirms nothing, because THERE IS NO READING OF AN INSTRUMENTED TREE AT ALL -- no suite was
  ever run while the instrument existed. The gate re-derived that by classifying every
  2026-08-27 session log: the one instrumented session ran ZERO tests, and all three automation
  runs carry ZERO `HOVER-DIAG` lines, with each half of the table acting as the other's control.
  `engine.md` gives the construction reason the field could not have moved in any case: no
  automation clause reaches `OnHover`, and the second log site was deliberately put at `Log` for
  exactly that reason.]** A figure that cannot move is not evidence that something changed. WHAT ACTUALLY CONFIRMS
  THE REMOVAL is a token census over the source, WITH A CONTROL -- and NOT the diff, which
  `engine.md` withdrew on the sharper ground that a churn count cannot separate an instrument line
  from a refactor line, so it never bore on the claim at all. The two files are aligned on that
  deliberately: naming `the diff` here after it was struck there is how a withdrawn item walks back
  in. The census: no `HOVER-DIAG`, no counter and no
  handler survives as code. The suite's role here is regression only -- it says the removal broke
  nothing, which is worth having and is all it says.]**
  The report is this tree's: `reportCreatedOn 2026.08.27-18.05.07`, run in
  `E:/MultiAgent/Stratocracy` on `master`. THAT STAMP IS UTC AND THIS ENTRY'S DATE IS LOCAL.
  **THE REPORT PREDATES THE CURRENT BYTES AND THAT IS STATED RATHER THAN LEFT FOR A READER TO
  NOTICE.** It is 14:05:07 local; the two source files were last written at 14:49:15 and relinked
  at 14:49:39. Everything written in between was PROSE -- the amendments two gate findings asked
  for -- and a comment cannot change a clause's outcome, which needs no corroborating build; a
  green rebuild would only say the current bytes compile. So the figure is not claimed to have been
  measured on the exact current bytes, only on the last bytes that could have moved it.
  **AND THE GATE THEN DERIVED THAT RATHER THAN LEAVING IT STATED.** It ran the comment-stripped
  diff of both source files: the ENTIRE non-comment delta is four things -- the `HoverAction`
  `UPROPERTY` removed, `OnHover`'s declaration and definition removed, the `Triggered` binding
  removed, `Tick`'s declaration and definition added. Nothing else in either file. And
  `Intermediate/.../StratPlay/UHT/StratPlayerController.gen.cpp` is still stamped 14:00:44, so the
  14:49 header edit changed no reflected surface. The header's CODE content at report time equals
  its code content now, so no re-run is owed.
  Lanes: the C++ and `engine.md` are `strat-gameplay-engineer`'s and are its own work; the editor
  driving, the asset cleanup and `content.md` are the `coordinator`'s under the editor-driver
  clause and that file's fallback, cited separately there.
  **THE GATE RETURNED `VERDICT: PASS` WITH ZERO FINDINGS ON ITS THIRD ROUND, and that is the
  verdict for this pass** -- recorded here because `global.md` is the only file that may state one.
  It blocked twice first, and NEITHER ROUND FOUND ANYTHING IN THE GAME CODE, THE ASSETS OR THE
  MODULE GRAPH: round one was three record claims, round two was two claims inside the corrections
  for round one, and the sharpest was a false sentence surviving directly ABOVE its own retraction
  because the correction had been nested rather than restated. Both correction blocks were then
  rewritten FLAT, which is the structural lesson and not merely the fix. **TWO CHANGES WERE MADE
  AFTER THAT PASS AND ARE NAMED SO A READER KNOWS WHAT IT DID NOT COVER**, both non-gating
  observations the gate raised and neither touching a figure: this entry no longer names `the diff`
  as confirming the instrument's removal, since `engine.md` withdrew that item and leaving the word
  here is how a withdrawn claim walks back in; and the provenance paragraph no longer leans on a
  green rebuild as corroboration, carrying the gate's own comment-stripped derivation instead. This
  sentence is the third.
  - **THE DIAGNOSIS WAS SETTLED BY AN INSTRUMENT, NOT BY INFERENCE, and the instrument was
    designed better than it was asked for.** The engineer added a counter and log site in
    `OnHover` plus a CONSOLE-FORCIBLE CONTROL in `UpdateHoverFromCursor` -- so one `ke` call
    proved the channel prints, proved the editor held the rebuilt DLL, and read the handler count
    out of the running game, with no planted state. It answered: **`OnHover` ran exactly ONCE in
    three and a half minutes**, at a focus transition seven seconds after the match seeded, with
    the cursor off the board; a fifteen-second human sweep across the board did not move it.
  - **THAT ALSO EXPLAINED THE ONE OBSERVATION THE PREVIOUS ENTRY COULD NOT.** A planted hover had
    cleared during the first playtest at the moment the cursor left the window -- read then as the
    binding firing normally, and retracted. Both events are the same phenomenon: a focus
    transition delivers one axis event, ordinary movement delivers none.
  - **THE ROUTE IS NOW A TICK POLL, AND WHAT WAS REJECTED MATTERS AS MUCH AS WHAT WAS CHOSEN.**
    `SetInputMode` with capture was rejected on two grounds: `bShowMouseCursor = true` is
    load-bearing for a mouse-driven hex game and every mode that reliably feeds axis input does so
    by capturing, which changes how a click lands; and it would be unfalsifiable at this project's
    cost, since nothing here reaches `UPlayerInput`, so it could only ever be re-confirmed by
    another human playtest. **The tick route is the first hover path a clause can pin at all.**
  - **WHAT IS PROVEN AND WHAT IS NOT, kept apart deliberately.** PROVEN: mouse movement produced
    no `Triggered` event for a mouse-axis action on this controller as configured, and the tick
    route produces a hover that follows the cursor across 25 distinct hexes and clears off-board.
    **NOT PROVEN: WHY.** The uncaptured-visible-cursor mechanism was never isolated, and neither
    the code nor `engine.md` nor `content.md` asserts it. The fix working is evidence the route
    was wrong, not proof of the engine mechanism.
  - **A TOOLING CLAIM WAS RAISED AND DISPROVED RATHER THAN RECORDED**, and it is noted because a
    finding this alarming would have been worth acting on had it been true. The engineer reported
    `strat_banner_sweep.py` printing `SWEEP FAILED` while returning exit code 0 -- which would
    mean the pre-commit hook gates nothing. Measured directly with a control: a scratch report
    with `succeeded=17` gives `SWEEP FAILED` and **exit 1**, the real report gives `SWEEP CLEAN`
    and **exit 0**. `render()` and `main()` read the same `result.passed`, so no individual check
    can diverge from the code. The near-certain cause is reading `$?` after a PIPE, which returns
    the last command's status. **The guard is sound and the claim is not in the record as a
    finding.**
  - **WHAT IS OWED NEXT.** (1) `Stratocracy.StratPlay.T-UI-01.TickDrivesTheHoverPath` -- newly
    pinnable and previously not, because a tick with no viewport clears a hover planted via
    `SetHoveredHex`, which makes it falsifiable by emptying the `Tick` body. **[CORRECTED
    2026-08-27 ON A GATE FINDING: this reason first read `since Tick is public and virtual`.
    `Tick` IS DECLARED `protected` in `AStratPlayerController` -- between that class's `protected:`
    and `private:` sections -- so `Controller->Tick(dt)` through an `AStratPlayerController*` does
    NOT compile. The clause is still writable, because `AActor::Tick` is public in the base and an
    `APlayerController*` or `AActor*` handle dispatches virtually; what was wrong was the stated
    route, which the receiving lane would have met as a compile error. The engineer was asked to
    settle whether the access level or the handoff wording is the thing to change.]** `strat-test-author`'s lane. (2) `Source/StratPlay/Tests/StratPlayerControllerTick.cpp`
    pins `bCanEverTick == true` and gives only the Enhanced Input reason; that clause now protects
    two mechanisms and names one. Same lane, and the assertion is correct today. (3) W1 and W2 are
    now unblocked in behaviour as well as in code.

- **2026-08-27, COORDINATOR -- THE HUMAN PLAYTEST RAN AND WAVE 0'S INPUT HALF DOES NOT REACH THE
  GAME. FINDING ONLY; NO CODE, NO ASSET AND NO TEST WAS TOUCHED BY IT.** The playtest that the
  banner above named as the next thing owed has now happened, with a human at the mouse, in the
  live editor on this tree. **NO SUITE WAS RUN AND NO SUITE FIGURE MOVES** -- the live figure is
  the one the topmost banner cites. **THE GATE RETURNED `VERDICT: PASS` WITH ZERO
  FINDINGS ON ITS SECOND ROUND, over this uncommitted tree, and that is the verdict for this
  record-only pass** -- stated here because `global.md` is the only file that may state one. The
  sentence this replaces said no verdict was stated, which was true until the gate answered;
  it is replaced rather than joined, because an entry carrying both asserts both. The first round
  blocked on ONE finding, a miscounted poll split in `content.md` whose cause -- an instrument
  that double-counted its own summary -- is recorded where the figure sits. **TWO CHANGES WERE
  MADE AFTER THAT PASS AND ARE NAMED SO A READER KNOWS WHAT IT DID NOT COVER**: both are
  non-gating observations the gate raised and neither touches a figure -- a backticked token
  quoted from a poller file that did not match the file's own text, and a "seven key rows work"
  support that had become load-bearing without saying it was measured in earlier passes rather
  than this session's log. This sentence is the third. What this entry records beyond the verdict
  is a measurement and the work that measurement makes owed. The full evidence, the poll designs
  and the two instrument defects found along the way are in `content.md`, which owns PIE evidence;
  this entry does not restate the numbers it does not need.
  - **THE RESULT IN ONE LINE.** Every link of the hover chain is proven working EXCEPT the first:
    Enhanced Input delivers no `Triggered` event for `IA_Hover` from a mouse move. The
    `HoverAction` default loads and binds, the `IA_Hover` -> `Mouse2D` row is live in the running
    game's mapping context, and `UpdateHoverFromCursor` resolves a hex and refreshes the model
    correctly when called directly. Driven by the mouse, the hovered hex never changes.
  - **THE COMMIT THAT SET THE DEFAULT IS NOT FALSIFIED BY THIS, and the distinction is the point.**
    `db3de97` claimed the default is set and byte-verified; it is, and the running game confirms
    it independently. What this finding falsifies is any reading of WAVE 0 -- `1da4198` and its
    merge -- in which the hover input surface reaches the game. The wave's five clauses are
    likewise untouched: every one of them was written to pin something OTHER than the four
    properties declared unpinnable, and this is a failure in exactly one of those four.
  - **WHAT IS OWED, and it is `strat-gameplay-engineer`'s lane, not the content lane.** The
    leading hypothesis -- stated as a hypothesis in `content.md`, not as a finding -- is that a
    visible uncaptured cursor feeds no mouse AXIS input, which would make `Mouse2D` the wrong
    route regardless of how the asset is authored. Proving or refuting it needs one log line
    inside `AStratPlayerController::OnHover` and a rebuild with the editor closed. If it holds,
    the fix is a route change rather than an asset change, and this project already has the
    seam for it: `UpdateHoverFromCursor` is `BlueprintCallable`, was called thirty times from a
    console during the playtest, and answered correctly every time.
  - **W1 AND W2 ARE AFFECTED AND THE WAVE PLAN SHOULD SAY SO.** Both were recorded as unblocked by
    W0. They still are for their view-model and widget work, which does not depend on the input
    route -- but neither can be SEEN working end to end until a hover reaches the game, because a
    forecast card with no hovered hex has nothing to draw. Treat W0 as landed in code and open in
    behaviour.

- **2026-08-26, COORDINATOR -- THE MVP-GAP AUDIT IS CUT INTO WAVES, AND CUTTING IT CORRECTED TWO
  OF ITS OWN CLAIMS. ASSIGNMENT ONLY; RECORD-ONLY.** The entry below ends by saying nothing is
  assigned in it and that turning it into lanes is a separate pass. This is that pass. **NO
  SOURCE, NO TEST, NO ASSET AND NO CONFIG FILE WAS TOUCHED; NO SUITE WAS RUN AND NO SUITE FIGURE
  MOVES -- the live figure is the one this file's topmost banner cites. NO PHASE VERDICT IS
  STATED HERE, and nothing below has been through a gate.** Nothing is dispatched either: a wave
  is a proposed unit of work until the user starts it.
  - **TWO OF THE AUDIT'S CLAIMS DID NOT SURVIVE THE TREE, AND BOTH ARE STAMPED WHERE THEY STAND
    rather than only corrected here** -- an entry carrying the corrected claim in one place and
    the original in another asserts both. (1) `AStratPlayerController::SkipGuidance` HAS a
    caller -- `WBP_DirectiveStrip`'s `SkipButton`, built 2026-08-24 at `229fe76` and recorded in
    `content.md` and in this file's banner of that date. (2) The hover scan did not return zero;
    45 lines match, two of them shipped `FStratGuidanceView` fields, and `ReachOverlay` contains
    the substring `hOverlay`, which is why an overlay-only reading looked like silence. **The
    hover FINDING survives both corrections** -- what is absent is the hover INPUT, not hover
    vocabulary -- so Wave 0 below is unchanged by them, while the Skip-guidance item is struck
    from the list entirely.
  - **THE CONTENTION MAP, because it decides the ordering more than the value does.** Waves 1, 2,
    3 and 8 all add fields to `StratViewModel.h` and a compose function to `StratViewModel.cpp`.
    That is a TEXT conflict, which rebases; `Content/` is the one that cannot merge, so every
    wave's asset tail is authored on the integration tree in its own editor batch, per this
    project's standing rule. Wave 4 touches `AStratUnitActor::ApplyView` and no view-model field,
    and Wave 5 touches `FStratAiTurnRunner` and no widget -- those two are the ones that can run
    beside anything.
  - **THE WAVES.** Each names its lane, the C++ half, the asset tail, and what it unblocks.
    - **W0 -- THE HOVER INPUT SURFACE. The unblocker, and the only wave two others sit behind.**
      Engineer: a hover-shaped `UInputAction` property on `AStratPlayerController` beside the
      five it carries, hovered-hex resolution reusing the cursor-to-hex route `SelectAction`
      already uses, hovered-hex state, and the hovered hex carried into the view model. Asset
      tail: one Enhanced Input asset under `/Game/StratInput`, its mapping-context row, and the
      Blueprint default on `BP_StratPlayerController` -- no `/Game/` literal in C++. Unblocks W1
      and W2 and nothing else.
    - **W1 -- SEC 2.11.3, THE ATTACK FORECAST CARD. The largest gap, and the section the GDD
      calls the centrepiece.** Engineer: a forecast view struct and its compose function over
      `FStratBridge::Forecast`, which exists and whose numbers `FStratSelectionMachine` currently
      discards. Six readouts: damage, counter and its stated reason, HP before -> after, the
      lethal `Destroys <unit>` line, and the FLAG AT RISK band. Asset tail: one Widget Blueprint.
      Depends on W0.
    - **W2 -- THE INFO PANEL, AND WITH IT PER-UNIT HP.** Engineer: a hex info struct (terrain
      name, move cost, defense bonus) and a unit stat line over `FStratUnitView::Hp`/`HpMax`,
      which are projected and read by nothing outside `Tests/`. Asset tail: one Widget Blueprint.
      Depends on W0. **Its consequence is sharper than the element**: today a player cannot see
      which of their own units is damaged, while Sec 2.6's formula makes a damaged attacker hit
      softer.
    - **W3 -- THE PERSISTENT HUD, three of Sec 2.11.2's four elements.** Engineer: the Fame pool
      and `+X/turn` (`FStratSideView::FameTotal` has exactly one reader and `IncomePerTurn` has
      none), a purse field for the production menu's own header, an End Turn surface, and a count
      of units still able to act. Asset tail: the HUD Blueprint and one command-bar widget.
      Independent of W0 -- this is the wave to run in the second slot.
    - **W4 -- THE ON-MAP MARKERS. The cheapest wave with a visible payoff.** Engineer:
      `AStratUnitActor::ApplyView` draws mesh-by-`DefId`, the side material and the guided marker;
      `bIsFlag` and `bDone` are already projected and read by no drawing code. No new model field.
      Asset tail: material and mesh work for the flag marker and the unacted pip.
    - **W5 -- THE AI'S TURN IS NEVER SHOWN.** Engineer, and the largest engine risk on this list:
      `FStratAiTurnRunner`'s own header states it runs a whole turn SYNCHRONOUSLY, and
      `FStratMatchConfig::AiTurnDelaySeconds` paces between TURNS. Sec 2.11.2 wants the action
      list replayed at a watchable pace with the camera stepping and any click skipping to the
      end. No asset tail beyond the skip affordance. Independent of every other wave.
    - **W6 -- THE TITLE/MENU SCREEN, and it is a shell rather than a readout.** Sec 2.11.5 fixes
      the prototype's screen list at four; briefing, match and result are built. Needs a level, a
      menu widget, new-match/restart/quit routes, and `GameDefaultMap` moved off
      `Lvl_FerrumCrossing` in `Config/DefaultEngine.ini` -- which is the steward's file, so this
      is the only wave with a data-lane half. **It gates the packaged build**, since a package
      today boots straight into a match with no way out of it.
    - **W7 -- SEC 2.9'S EASY DEFAULT.** `ferrum_crossing.json` is symmetric at 200/200,
      `Scenario.h` says in as many words that the handicap is a match-setup parameter and not a
      scenario field, and `FStratMatchConfig` has no field for it. Ours to build, not an upstream
      re-vendor. Small, and it is the guided opening's own unmet premise.
    - **W8 -- THE TRANSIENT LAYER, batchable and each item small.** Path preview with cost ticks,
      the capture-progress pip (`CaptureProgress` projected, unread), the repair-eligibility pip,
      the BUILD pulse on an affordable factory (`bHasBuiltThisTurn` projected, unread), the income
      and kill toasts, and the `YOUR TURN`/`ENEMY TURN` banner.
    - **NOT WAVES.** The packaged build follows W6 rather than standing beside it. The self-play
      balance sims and the balance lock are crew-side, in `stratocracy-crew`, and no lane here
      moves them.
  - **NO WAVE MAY NAME A CLAUSE UNTIL IT HAS AN ACCEPTANCE ID, AND ONLY TWO DO.** The GDD's
    acceptance set carries five UI IDs, `T-UI-01` through `T-UI-05`, and no others; a clause name
    in this project is `Stratocracy.<Module>.<AcceptanceID>.<Clause>`. **W1 has an ID on its
    own**: `T-UI-01` IS the forecast ID -- the forecast shown before commit being produced by the
    resolution path -- pinned headlessly today, so W1 extends an existing ID to the presentation
    side and mints nothing.
    - **W0 RIDES `T-UI-01` TOO. USER RULING, 2026-08-26, RECORDED BY THE `coordinator`.** W0's
      clauses are named `Stratocracy.<Module>.T-UI-01.<Clause>` and NO NEW ID IS MINTED for it.
      **The reason the ruling is coherent, stated so a later reader does not have to reconstruct
      it:** W0 exists to carry a hovered hex to the view model, and the thing that consumes a
      hovered hex is the forecast card -- W0 is the input half of the same acceptance, split out
      only because the input is shared with W2. **AND THE STRETCH IS STATED RATHER THAN HIDDEN:**
      `T-UI-01`'s own GDD sentence is about the forecast EQUALLING the resolution, and a
      hover-input clause asserts no such equality. So a W0 clause under this ID must assert
      something the forecast path needs -- that a hovered hex reaches the model, and which hex --
      and must not be written as though the ID's own claim covered it. A clause that rides an ID
      it does not test is the unfalsifiable shape this record has been caught by before.
    - For W2, W3, W4, W5, W6 and W8 there is still no ID and no ruling, and **inventing one in a
      test name is a failure this record has already recorded**: a proposed identifier greps
      exactly like a defined one. Each is the user's to rule the same way, or to mint upstream in
      the GDD.
  - **AND THE ORDERING IS A JUDGEMENT, NOT A MEASUREMENT** -- the same caveat the audit ended on,
    undischarged by cutting the list into waves. The sequence is by what the player loses without
    each item, with W0 first only because two waves cannot start without it. This record still
    holds no MVP burndown to check the ordering against.

- **2026-08-26, COORDINATOR -- AN MVP-GAP AUDIT READ OFF THE TREE, AND THE LARGEST GAP IS THE
  SECTION THE GDD ITSELF CALLS THE CENTREPIECE. RECORD-ONLY.** The user asked what the MVP
  still lacks. Answered by reading `Source/`, `Content/`, `Config/` and `Data/` rather than this
  record, on this file's own rule that the tree outranks the note. **NO SOURCE, NO TEST, NO
  ASSET AND NO CONFIG FILE WAS TOUCHED; NO SUITE WAS RUN AND NO SUITE FIGURE MOVES ON IT** --
  the live figure is the one this file's topmost banner cites and it is unchanged. **NO PHASE
  VERDICT IS STATED HERE**: an audit is not a gate, and nothing below has been through one.
  The checklist is the GDD'S OWN and not one assembled here -- Sec 2.10's IN column, Sec
  2.11.2's earn-your-pixels table, and Sec 2.11.5's "complete screen list for the prototype".
  - **WHAT IS BUILT, so the gaps are read against it and not against nothing.** The rules half
    is complete and vendored: grid, terrains, four units, move, attack, capture, Fame,
    production, the heuristic AI, win-by-flag, the turn cap and the tiebreak. On this side: the
    board with its three overlays, the scoreboard, the directive strip and the guided opening,
    the pre-match briefing, the production menu, the match-result screen, save-slot I/O with
    the completion record, and the one shipped scenario. A human has played a match to a result
    against the AI. **So every finding below is about what the player is not TOLD, not about
    what the game cannot DO** -- which is why not one of them has ever reddened a clause.
  - **SEC 2.11.3, THE ATTACK FORECAST, DOES NOT EXIST ON THIS SIDE.** `FStratBridge::Forecast`
    works and `T-UI-01` pins it. Its only non-test consumer is `FStratSelectionMachine`, which
    asks it which enemy hexes are legal targets and DISCARDS THE NUMBERS. No forecast struct
    exists in `StratViewModel.h`, no widget for it exists under `Content/UI/`, and no clause
    asserts a forecast is drawn. Absent, therefore: the damage line, the counter line and its
    stated reason, the HP before -> after, the lethal `Destroys <unit>` reward line, and the
    FLAG AT RISK band that Sec 2.11.3 says no player may end a match without having seen. The
    player commits attacks blind against a rules module whose entire design claim is that the
    forecast IS the resolution, shown early.
    - **THIS RECORD HAD NEVER MENTIONED THAT SECTION, AND THE CHECK IS NOT REPEATABLE AFTER
      THIS ENTRY.** Before this bullet was written, a sweep of all six files in
      `Tools/architect/state/` for that section number returned NOTHING. This paragraph is now
      the first mention, so re-running it finds this and not the silence -- stamped that way
      because the silence IS the finding: a centrepiece section can go four milestones without
      one line in the record and read exactly like a section that was weighed and deferred.
  - **ONE MISSING INPUT ACCOUNTS FOR TWO OF THE GAPS, WHICH MAKES THEM ONE FIX AND NOT TWO.**
    HOVER IS NOT WIRED ANYWHERE: a scan of `Source/StratPlay` and `Source/StratUI` outside
    `Tests/` for hover vocabulary returns ZERO. Sec 2.11.3's card and Sec 2.11.2's info panel
    are both specified as hover-driven, so both sit behind the same absent Enhanced Input
    surface. `AStratPlayerController` carries `SelectAction`, `CancelAction`, `WaitAction`,
    `EndTurnAction` and `OpenProductionMenuAction`, and nothing for hover.
    - **[STAMPED 2026-08-26, LATER PASS, SAME WRITER -- THE SECOND SENTENCE OF THIS BULLET IS
      WRONG AND ITS LAST ONE IS RIGHT.** The scan did NOT return zero: a case-insensitive `hover`
      scan of `Source/` returns 45 lines, and two of them are SHIPPED view-model fields --
      `FStratGuidanceView::EndTurnGateHover` and `FStratGuidanceView::LockedUnitHover`, filled by
      `FStratGuidedOpening::LockedUnitHoverText` and `EndTurnGateHoverText`. Hover STRINGS for a
      widget to render are built and clause-pinned. **THE INSTRUMENT EXPLAINS THE ZERO:**
      `ReachOverlay` contains the substring `hOverlay`, so that scan matches every overlay
      identifier in `StratBoardActor`, and a truncated read of that output looks like nothing but
      overlays. WHAT IS ACTUALLY ABSENT is the hover INPUT -- no hover-shaped `UInputAction` on
      `AStratPlayerController`, no hovered-hex state, and nothing carrying a hovered hex into the
      view model -- which is this bullet's LAST sentence and is untouched by the correction. The
      finding stands; the measurement offered for it did not.]**
  - **THREE OF SEC 2.11.2'S FOUR PERSISTENT ELEMENTS ARE UNBUILT.** The scoreboard is the one
    that exists.
    - **The Fame pool and its `+X/turn` are drawn nowhere.** `FStratSideView::FameTotal` is
      projected and read by exactly one caller -- `StratBuildProductionMenu`, to compute
      `Shortfall` -- and `FStratSideView::IncomePerTurn` is projected and read by NOTHING.
      **The player's current Fame is never printed on screen**, not even in the production
      menu's own header, which Sec 2.11.5's mock shows carrying `Fame: 250`:
      `FStratBuildOptionView` has the per-row cost and shortfall and no field for the pool.
    - **End Turn and the idle-unit count have no on-screen home.** `EndTurnAction` is a key
      binding; there is no button, and no count of units still able to act.
    - **The on-map half of that same list is unbuilt too** -- the flag `H` markers for both
      sides and the unacted pip on own units. `AStratUnitActor::ApplyView` applies mesh-by-
      `DefId`, the side material and the guided marker, and nothing else; `bIsFlag` and `bDone`
      are populated in the view model and read by no drawing code.
  - **THE INFO PANEL IS ABSENT, AND ITS CONSEQUENCE IS SHARPER THAN THE ELEMENT.** No widget
    carries a hovered hex's terrain name, move cost or defense bonus, and none carries a unit's
    stat line. So **PER-UNIT HP IS INVISIBLE TO THE PLAYER**: `FStratUnitView::Hp` and `HpMax`
    are projected and read by nothing outside `Tests/`. The only HP anywhere on screen is the
    scoreboard's aggregate `Unit HP` row. A damaged attacker hits softer by Sec 2.6's formula,
    and the player cannot see which of their own units is damaged.
  - **THE CONTEXTUAL AND TRANSIENT LAYERS ARE THIN.** Built: the reachable-hex highlight, the
    attack-target highlight, the objective ring, the production menu, the guided strip and its
    one-shot tip. Absent: the path preview with cost ticks, the capture-progress pip
    (`CaptureProgress` projected, unread), the repair-eligibility pip, the `BUILD` pulse on an
    affordable factory (`bHasBuiltThisTurn` projected, unread), the income and kill toasts, and
    the `YOUR TURN` / `ENEMY TURN` banner.
  - **THE AI'S TURN IS NEVER SHOWN.** Sec 2.11.2 requires the AI's action list replayed at a
    watchable fixed pace, the camera stepping to each action, any click skipping to the end.
    `FStratAiTurnRunner`'s own header states it runs a whole turn SYNCHRONOUSLY, and
    `FStratMatchConfig::AiTurnDelaySeconds` paces BETWEEN turns rather than between actions.
    The player sees a board that has changed and gets no account of how.
  - **THERE IS NO TITLE/MENU SCREEN, AND SEC 2.11.5 FIXES THE LIST AT FOUR.** Briefing, match
    and result are built; the menu is not. `Config/DefaultEngine.ini` sets `GameDefaultMap` to
    `Lvl_FerrumCrossing`, so a packaged build boots straight into a match with no new-match, no
    restart and no quit route. This is the one gap that is not a readout: it is the shell the
    other three screens hang off, and it is what makes the packaging item below more than a
    build step.
  - **TWO SPECIFIED CONTROLS EXIST IN C++ AND NO PLAYER CAN REACH EITHER.**
    - `AStratPlayerController::SkipGuidance` is correct and `BlueprintCallable` and HAS NO
      CALLER -- no input action, no button. Sec 2.11.6 specifies a `Skip guidance` control that
      kills guidance instantly for anyone. Already on this record since 2026-08-24 and still
      true; restated here because it is an MVP surface and not only a loose end.
    - **[STAMPED 2026-08-26, LATER PASS, SAME WRITER -- THE BULLET ABOVE IS FALSE, AND THIS
      RECORD HAD ALREADY SAID SO.** `AStratPlayerController::SkipGuidance` HAS A CALLER:
      `WBP_DirectiveStrip`'s `SkipButton`, `On Clicked` -> `Cast To StratPlayerController` ->
      `Skip Guidance`, authored 2026-08-24 and committed at `229fe76`. `content.md` records the
      authoring, the fresh `read_graph` link check and the PIE capture that shows the chip
      painted; this file's own 2026-08-24 banner records what it did for that `OWNED:` line.
      RE-VERIFIED INDEPENDENTLY HERE WITH NO EDITOR, by reading the package's string table: the
      same `.uasset` carries `SkipButton`, `SkipLabel`, `Skip guidance`, a `K2Node_CallFunction`,
      the bound-event name
      `BndEvt__WBP_DirectiveStrip_SkipButton_K2Node_ComponentBoundEvent_0_OnButtonClickedEvent__DelegateSignature`,
      and a graph comment quoting Sec 2.11.6. **THE ROUTE OF THE ERROR IS WORTH MORE THAN THE
      ERROR.** The sentence "already on this record since 2026-08-24 and still true" cited this
      record for a claim this record RETIRED on that very date: it came from a memory note
      written before the button existed, and was never checked against the tree the rest of that
      entry was read off. The parent entry's own stated limit -- no Widget Blueprint graph was
      read -- names exactly where it would have been caught, and a string-table read costs one
      command and no editor. So the bullet's heading is wrong too: ONE specified control is
      unreachable, not two. What remains unmeasured about the Skip control is only the Slate
      click itself, for the injection reason `content.md` gives.]**
    - **Sec 2.9's Easy default is not applied.** Sec 2.11.6 says the first match runs at Easy
      with the player on an opening-Fame handicap. `ferrum_crossing.json` carries a symmetric
      `startingFame` of 200/200, `Scenario.h` says in as many words that the difficulty
      handicap is a match-setup parameter applied on top of the scenario rather than a field in
      it, and `FStratMatchConfig` has no field for it. **Nothing applies it**, so the guided
      opening's own premise about the first session is unmet.
  - **TWO ITEMS ON THE MVP LINE THAT ARE NOT FEATURES.** Sec 4.4 wk 7 names the PACKAGED BUILD
    as the deliverable and no package has ever been produced here -- this record discusses "the
    packaged game" only hypothetically and there is no packaging output in the tree. Sec 4.4
    wk 4 and wk 6 name the self-play balance sims and the balance lock; both are crew-side and
    unstarted.
  - **WHAT THIS AUDIT DOES NOT COVER, said plainly.** The six Widget Blueprint graphs under
    `Content/UI/` were NOT read -- `.uasset` is not text and no editor was driven for this
    pass. That limit does not reach the findings: for every missing readout above, the C++
    model the widget binds to carries no field holding the value, and this project forbids
    widget-side arithmetic, so a graph could not be quietly supplying one. Where a graph COULD
    differ from this account is in the layout and wording of what is already modelled.
  - **AND THE ORDERING IS A JUDGEMENT, NOT A MEASUREMENT.** The sequence above is by what the
    player loses without each item. **This record holds no MVP burndown to check that against**
    -- the candidate backlog further down scores CANDIDATES, not GDD coverage, and that is the
    same shape as the four entries corrected there on 2026-08-24 one level up: a status that is
    derived is only as correct as the subject the probe was pointed at, and no probe in
    `scan.py` or `candidates.py` is pointed at Sec 2.11.2's table at all. **NOTHING IS ASSIGNED
    HERE**: this is a finding list, and turning it into lanes is a separate pass.

- **SEC 2.11.6-B CLOSES. 2026-08-24, HUMAN-DRIVEN PLAYTEST, THE USER AT THE KEYBOARD: THE RING
  AND THE MARKER WERE SEEN DRAWN AND THEY PASSED. RULED CLOSED BY THE USER; RECORDED BY THE
  `coordinator`, WHICH IS THIS FILE'S TO STATE.** This is the verdict the entries below have
  refused for four days, and it is stated here and nowhere else -- `global.md` is the only file
  that may carry one. No suite count moves on it: nothing in this pass touched a test.
  - **WHY THE HUMAN EYE WAS THE ONLY INSTRUMENT THAT COULD DO IT, restated because it is the
    reusable half.** `content.md` already carried an AGENT-driven PIE sighting of both, each with
    a positive control. That was never sufficient and was never claimed to be:
    `IsGuidedMarkerVisible` reports a component FLAG, not pixels, and `GetObjectiveOverlayCount`
    reports an instance count -- so every headless and every scripted route can be green on a
    blank screen. No clause can be written that discharges this, which is why it sat open rather
    than being automated away.
  - **WHAT THIS RESTS ON, AND WHERE ITS DETAIL IS.** The assets are committed and were measured
    off the package bytes, not off a lane report: `BP_StratBoard` carries `MI_Overlay_Objective`
    against `ObjectiveMaterial`; `BP_StratUnit` carries `SM_GuidedMarker` and `MI_Marker_Guided`
    against `GuidedMarkerMesh` and `GuidedMarkerMaterial`. The four directives, both input-gate
    hovers, the window-end tag and all four beats retiring were read back by the user on
    2026-08-23 and -24; beat 2's live retirement on its second arm and the ring, marker and strip
    going dark in ONE frame were witnessed on 2026-08-24. `content.md` and `engine.md` carry the
    detail and this entry does not restate it.
  - **ONE SPECIFIED SURFACE OF SEC 2.11.6-B IS NOT BUILT, AND THE CLOSURE IS RECORDED OVER IT
    RATHER THAN AROUND IT.** The GDD says a `Skip guidance` control "kills it instantly for
    anyone". `AStratPlayerController::SkipGuidance` exists, is `BlueprintCallable`, refreshes in
    the same frame, and is pinned by `GuidedMarkerGoesOutAfterSkipGuidance` -- but **it has no
    caller.** No input binding, no widget button, and `grep` over `Content/` returns no asset
    naming it; the header says so itself, in the words "for the button that will eventually call
    it". So no player can reach it. This was measured in the pass that recorded the closure and
    raised to the user, who ruled the section closed regardless. It is recorded here because a
    closure that quietly swallows an unbuilt surface is how the surface stops being owed at all.
    **OWNED: `strat-editor-builder`** -- one button bound to one already-reflected verb.
    **[DISCHARGED 2026-08-24, later the same day, by the `coordinator` under `content.md`'s
    fallback condition. `/Game/UI/WBP_DirectiveStrip` now carries a `SkipButton` whose
    `OnClicked` casts the owning controller and calls `Skip Guidance`. Seen painted in PIE
    with the opening armed, and seen going out together with the strip under a
    `ke * SkipGuidance` positive control; `content.md` holds the evidence and this entry does
    not restate it. THE LAST LINK IS CLOSED TOO: no mouse event could be injected, so the
    Slate-click-to-delegate link was inferred rather than observed -- and the USER then clicked the
    button and reports that it works, which closes it on the same human-observation standing as the
    ring and the marker. Everything this bullet says was true when written.]**
  - **`GuidedMarkerZOffset` WAS NOT MEASURED BY THIS PLAYTEST, and its item stands untouched.**
    It is still `150.0f` in `StratUnitActor.h` and the standing note that it occludes the unit at
    this camera pitch is neither confirmed nor retracted here. A playtest that passes on the
    things it looked at says nothing about the thing it did not.
    **[SUPERSEDED 2026-08-24, LATER THE SAME DAY. The item did not need a playtest after all: the
    occlusion is DERIVABLE, and was derived once `SM_GuidedMarker` (100 uu cone, centred on its
    origin), `FallbackMesh` (`/Engine/BasicShapes/Cylinder`, 100 uu, also centred) and
    `AStratCameraPawn`'s `ArmPitch` of -60 with NO YAW on any path were all in hand. In body space
    screen-up is `0.866*x + 0.5*z`: the body tops out at 68.3, the marker's base rim at 150
    bottomed out at 6.7 and sat 111.6 nearer the camera, so it drew IN FRONT OF the head of the
    unit it names. Contact is Z = 273.2; the default is now `300.0f`, clearing by 13.4.
    **THE COST IS STATED IN THE HEADER AND NOT HIDDEN:** any positive Z draws over whatever is
    up-screen, so the marker now crowds the lower body of the unit on the hex BEHIND it. That is a
    trade taken deliberately -- a marker that hides its own subject is unreadable, one that crowds
    a neighbour is not.
    **WHAT IS NOT CLAIMED.** No eye has been on the new value, no suite covers it -- `T-INT-05`
    compares unit Z deltas against each other and never names a marker offset -- and the build that
    carried it compiled all 14 actions clean but could not LINK: `LNK1104`, the open editor holding
    `UnrealEditor-StratPlay.dll`, which is this project's own documented outcome for an in-place
    build and not a finding. **SO THE VALUE IS IN THE TREE AND NOT YET IN A RUNNING EDITOR.**
    **[THE LINK HALF OF THIS IS NOW FALSE. 2026-08-24, LATER THE SAME DAY, ON THE USER'S
    INSTRUCTION.** The editor was closed and the build re-run: `Result: Succeeded`, exit 0, and
    `UnrealEditor-StratPlay.dll` rewritten -- 2 actions, because the 14 objects from the refused
    run were still good and only the link was owed. **THE VALUE IS NOW IN THE BINARY.** The rest of
    this bullet stands unchanged: no eye has been on it and no suite covers it.
    **THE CLOSE WAS GRACEFUL AND THE DATA WAS NEVER AT RISK,** which was established BEFORE the
    close rather than after: `playtest_status` reported no PIE world, and `git status Content/` was
    empty, so every asset was committed. `CloseMainWindow()` returned true, the process exited
    inside the timeout with NO `Save Content` dialog and NO `CrashReportClientEditor`, and the DLL
    lock was then proved released by a rename round-trip before the build was started.
    **AND ONE NEGATIVE RESULT THAT IS NOT A FINDING, RECORDED SO THE NEXT LANE DOES NOT RE-DERIVE
    IT:** `exec_command` could not quit the editor -- `QUIT_EDITOR`, `CLOSE_SLATE_MAINFRAME`,
    `quit` and `exit` all came back `success=false / unrecognized`. **THAT IS THE CHANNEL, NOT THE
    COMMANDS.** The control settles it: `stat none` and `obj list class=World` are indisputably
    valid and returned `success=false` in the same call, with `has_pie_world = false`. So
    `exec_command` reaches no console without a world, exactly as `playtest_console` does. **Note
    the shape:** every one of those calls printed a leading `[OK]`, which this record already knows
    is not a verdict -- the field to read is `success`. True when written.]**
    OWNED: coordinator, to schedule a human look. `engine.md` and `content.md` each carry this item
    in their own lane's words and are owed by their own writers; this file does not write either.
    Everything this bullet says was true when written.]**
    **[DISCHARGED 2026-08-24, LATER THE SAME DAY, BY THE USER'S OWN EYE. THE HUMAN LOOK THAT LINE
    SCHEDULES HAS BEEN TAKEN.** The editor was reopened on `Lvl_FerrumCrossing` and the user
    reports the marker FINE and to be KEPT. **`GuidedMarkerZOffset = 300.0f` IS NOW A SHIPPED,
    HUMAN-CONFIRMED VALUE** and this item is closed.
    **THE BINARY WAS CONFIRMED BEFORE THE LOOK, so the eye and the tree cannot have disagreed:**
    `class_properties("/Script/StratPlay.StratUnitActor")` read `GuidedMarkerZOffset` back as
    `default=300.000000` off the CDO -- which IS the shipping value here, `BP_StratUnit` carrying
    no override for it -- and returned the NEW tooltip text verbatim, a string that exists only in
    this edit. A stale DLL could not have produced it.
    **WHAT THE RULING COVERS AND WHAT IT DOES NOT.** It is a SINGLE global verdict on what the user
    saw. Three questions were put -- the clearance over the unit's own head, the deliberate cost of
    crowding the unit on the hex BEHIND, and whether the marker still reads as naming ITS unit --
    and the answer itemised none of them. So the acceptance is recorded as an acceptance and NOT
    as three separate measurements; in particular the neighbour-crowding trade is accepted in
    effect, not inspected on the record. **The standing caveat is unchanged and still load-bearing:
    the whole derivation rests on `ArmPitch` = -60, and if that pitch is ever retuned this number
    wants re-deriving with nothing headless to say so.** True when written.]**

- **[SUPERSEDED 2026-08-24 BY THE ENTRY ABOVE, WHICH CLOSES SEC 2.11.6-B. Kept entire: its
  "TWO BUILD ITEMS" framing is the rung this section was on for four days, and its distinction
  between "unobserved" and "unbuilt" is the part worth carrying forward.]**
  **SEC 2.11.6-B: EVERYTHING OBSERVABLE IS NOW OBSERVED, AND WHAT REMAINS IS TWO BUILD ITEMS.
  2026-08-23, HUMAN-DRIVEN PIE, THE USER AT THE KEYBOARD. THE SECTION STILL DOES NOT CLOSE AND IS
  NOT CLAIMED CLOSED HERE** -- but the reason has changed from "unobserved" to "unbuilt", and that
  is a different kind of open.
  - **THE PAINTED DIRECTIVE TEXT WAS READ BACK BY A HUMAN EYE, WHICH HAD NEVER HAPPENED.** Every
    prior claim about the strip was a SHAPE claim -- 688 px wide, 3017 glyph pixels -- and a strip
    can rasterise glyphs and still paint the wrong string. The user read beat 2's line off the
    screen as `Move the Infantry onto the ringed Factory. Only Infantry captures.`, byte-exact
    against `FStratGuidedOpening::DirectiveTextFor`, capitalisation and both full stops included.
    **The last unverified piece of the exec-wire binding fix is closed.**
  - **THE WINDOW-END TAG RENDERS, ON TURN 4, ALONGSIDE THE DIRECTIVE.** Read back as
    `guidance ends this turn`, byte-exact against `WindowEndTagText()`. The pairing is itself
    correct per the beat model: turn 4 is always a rule-2 last call and beat 2 was the lowest
    outstanding beat, so the schedule table reproduced itself on screen.
  - **BOTH INPUT-GATE HOVERS SHOW THE RIGHT TEXT** -- `Move the marked Infantry first.` on End Turn
    and `Locked this turn.` on a non-marked unit, hovered before the marked Infantry moved, which
    is the only window in which either is armed.
  - **BEATS 1a AND 1b RETIRED AND LOGGED**, and the window closed at the end of turn 4. Beat 3
    retired in an earlier session the same day.
  - **WHAT IS LEFT IS NOT OBSERVATION. It is two things nobody built, and one confirmed defect.**
    See the two entries below; neither is the coordinator's to fix.
    **[AMENDED 2026-08-23, LATER THE SAME DAY, IN THE COMMIT THIS ENTRY IS COMMITTED IN. The
    CONFIRMED DEFECT is fixed and pinned, and the TWO THINGS NOBODY BUILT now have their C++ seams
    and are down to assets. So what is left is neither observation nor C++: it is the ring and
    marker MESHES, MATERIALS and Blueprint defaults, plus one human-driven playtest to see all of
    it on screen. Both entries below carry their own stamps.]**

- **BEAT 2'S RETIREMENT TRIGGER CANNOT FIRE ON THE SHIPPED SCENARIO. NEW, CONFIRMED 2026-08-23 --
  [ANSWERED 2026-08-23, LATER THE SAME DAY, IN THE COMMIT THIS ENTRY IS COMMITTED IN. Both
  `OWNED:` lines at the foot of this entry are DISCHARGED.** `FStratGuidedOpening::
  IsRingedObjectiveHeldByGuidedSide` reads `FStratHexView::Owner` at the hex
  `FStratBridge::GuidedOpeningHexes` answers, and beat 2 now retires on the pip **OR** that; the
  pip arm was kept rather than replaced, so §2.11.6's named trigger survives and every clause that
  plants `CaptureProgress` stays green. `strat-test-author` pinned it with
  `T-UI-03.Beat2RetiresWhenTheRingedObjectiveBecomesTheGuidedSides`, which asserts
  `CaptureProgress` is 0 on every unit before AND after the flip so the pip arm provably cannot
  account for the retirement, plus `T-UI-03.Beat2StaysOutstandingWhenAnotherObjectiveFlips` as its
  narrowing control. The entry is kept ENTIRE below because the mechanism it records is what the
  fix was built from, and because the create-and-erase-in-one-call finding generalises past this
  beat. **The fix is verified headlessly and has NOT been seen by a human in play.**]** --
  PREDICTED FROM THE RULES, THEN OBSERVED FAILING, THEN PROVED BY READING THE PROJECTION.**
  The user moved the guided Infantry onto the ringed objective `(2, 7)` and ended the turn. The
  factory became theirs. **No `Guided beat 2 retired` line was emitted**, with the control that
  `Guided beat 1a retired` and `Guided beat 1b retired` are both present in the same session, so
  the instrument can speak.
  - **THE MECHANISM, and it is create-and-erase inside one call rather than a race.** The shipped
    scenario sets `captureTurns = 1`. `captureTick` pushes a `CaptureProgress` with `turnsHeld = 1`,
    immediately tests `turnsHeld >= captureTurns`, flips the objective's owner and calls
    `clearProgress`, which ERASES the entry -- all within `captureTick`. The projection's
    `progressForUnit` then searches `economy.captures` for that unit and finds nothing, so
    `UiUnitView::captureProgress` is **0** in every snapshot that can ever be taken. The pip exists
    only inside `captureTick`'s own stack frame.
  - **SO BEAT 2 WATCHES FOR A STATE NO OBSERVER CAN SEE.** `FStratGuidedOpening::HasCapturePipLanded`
    reads `CaptureProgress > 0` on a guided-seat unit standing on `guidedOpening.objective`. That
    is unreachable at `captureTurns = 1`. It would work at 2 or more.
  - **NEITHER HALF IS WRONG ON ITS OWN, WHICH IS WHY THIS SURVIVED REVIEW.** `captureTurns = 1` is
    authored scenario data. The narrowing of beat 2 to the ringed objective is the 2026-08-21 USER
    RULING, recorded in `decisions.md`, and it was the right call on the question it answered --
    it prevented a nearest-factory heuristic retiring the beat on the wrong tile. The defect is the
    COMBINATION, and no clause covers a combination: `T-SCN-07` pins the objective hex against the
    scenario file and says nothing about whether the pip is ever visible.
  - **THE PLAYER-FACING SYMPTOM IS WORSE THAN A MISSING BEAT.** Beat 2 stays outstanding, so rule 1
    puts it back on the line every turn. The user was instructed to capture a factory they had
    already captured, and read that instruction off the screen on turns 3 and 4. A beat that cannot
    retire does not fall silent; it repeats.
  - **NOT DIAGNOSED BEYOND THIS, deliberately.** Whether the fix is a different observable for beat
    2, a projection that survives the flip, or scenario data is an engine-and-rules call and may
    need an upstream re-vendor. OWNED: `strat-gameplay-engineer` to scope; a clause is owed from
    `strat-test-author`, and note it must not be written against a planted `CaptureProgress`, since
    planting the field would pass on exactly the tree that fails.

- **[HALF-ANSWERED 2026-08-23, LATER THE SAME DAY, IN THE COMMIT THIS ENTRY IS COMMITTED IN, AND
  THE OWNER MOVED. The C++ SEAMS NOW EXIST** -- `AStratBoardActor::ObjectiveOverlay` (a third
  constructor subobject, never a third meaning on `TargetOverlay`, which `T-UI-02` requires to be
  dark during beat 1a) with `ShowObjective` / `ClearObjective` / `GetObjectiveOverlayCount`, driven
  from one site in `UStratMatchSubsystem::ApplyView` with an `else` that clears; and
  `AStratUnitActor::GuidedMarker` with `IsGuidedMarkerVisible`, its visibility a pass-through of
  `bIsGuidedMarked` ANDed with the viewing side per the 2026-08-23 user ruling recorded in
  `decisions.md`. Both are pinned by `T-UI-02` clauses. **THE ENTRY DOES NOT CLOSE.** The three
  `EditDefaultsOnly` ASSET REFERENCES ship UNSET as null `TObjectPtr`s -- `ObjectiveMaterial`,
  `GuidedMarkerMesh` and `GuidedMarkerMaterial`; the fourth new property, `GuidedMarkerZOffset`,
  is a numeric with a real default and is not part of this claim. So nothing draws: the meshes, the material instances
  and the Blueprint defaults on `BP_StratBoardActor` / `BP_StratUnitActor` are still owed, and
  **nobody has seen a ring or a marker on a screen.** OWNED, unchanged: `strat-editor-builder` --
  and note its route to the editor was itself broken for four days and was fixed in this same
  commit, see `content.md`. What changed is that the lane is no longer blocked on C++ that does
  not exist; it is blocked on assets it can now actually author.]**
  **[DISCHARGED 2026-08-24. A SIBLING STAMP, NOT A NESTED ONE -- the stamp above is the block
  this corrects, and nesting one inside it is the dangling-closer trap `content.md` records.
  Three sentences in that stamp have gone stale by UNDER-claiming, which is the direction that
  sends the next reader to redo settled work: "the three `EditDefaultsOnly` ASSET REFERENCES ship
  UNSET", "the meshes, the material instances and the Blueprint defaults ... are still owed", and
  "nobody has seen a ring or a marker on a screen". All three were true when written. All three
  are now false: the defaults are set and committed, and a HUMAN saw both drawn on 2026-08-24.
  The `OWNED: strat-editor-builder` line is discharged for the assets and survives for one thing
  only -- the `Skip guidance` button, which has never had a caller.
  **[FULLY DISCHARGED 2026-08-24, later the same day: the button is built. This sentence was
  true when written; see the newest banner block and `content.md`.]** Nothing above is deleted and
  no measurement is withdrawn. See the closing entry at the top of `## NEXT`; it is not restated
  here.]**
  **THE TURN-1a UNIT MARKER AND THE OBJECTIVE RING WERE NEVER BUILT, AND TWO OF THE FOUR DIRECTIVES
  ARE UNFOLLOWABLE WITHOUT THEM. Raised by the user at the keyboard 2026-08-23.**
  Beat 1a says "Select the marked Infantry" with nothing marked. Beat 2 says "the ringed Factory"
  with nothing ringed. The only way the user learned the objective was hex `(2, 7)` was by reading
  the log.
  - **THE DATA IS PUBLISHED AND REACHES THE VIEW MODEL; NOTHING DRAWS IT.**
    `FStratUnitView::bIsGuidedMarked` is written in `UStratViewModel`'s projection off the rules
    module's own `isGuidedMarked`, and `FStratGuidanceView` carries `bHasObjectiveRing` and
    `ObjectiveHex`, written by `FStratGuidedOpening::DecorateViewModel`. Measured:
    `bHasObjectiveRing` has exactly TWO non-test sites tree-wide -- the line that writes it and the
    comment describing it. **No reader.**
  - **THIS IS A DIVISION OF LABOUR THAT NEVER GOT ITS SECOND HALF, not an oversight in the C++.**
    `StratGuidedOpening.h` states it: "NO WIDGET, AND NO `/Game/` PATH. The directive strip, the
    ring mesh and the turn-1a marker are `strat-editor-builder`'s lane." The strip was built. The
    other two were not. OWNED: `strat-editor-builder`.

- **A MATCH THAT ENDS DOES NOT END THE GAME: THE HUMAN INHERITS THE AI'S SIDE AND KEEPS PLAYING.
  NEW, MEASURED 2026-08-23 IN A HUMAN-DRIVEN PIE SESSION, AND IT IS THE MOST SERIOUS THING OPEN.**
  **[ANSWERED 2026-08-23, LATER THE SAME DAY. The transition is built, linked, executed and pinned
  by five clauses; the banner at the top of this file carries the account. This entry is kept
  ENTIRE and unedited below because it is the diagnosis the fix was built from, and because the
  half it could not reach is still open: NO HUMAN HAS PLAYED A MATCH TO A RESULT WITH THE FIX IN
  THE TREE, and the three call sites on the player's input path are unreachable headlessly, so a
  controller that stopped calling the gate would pass all five clauses. The ENGINE half is closed;
  the PLAYER-FACING half is not, and it is not claimed. OWNED: coordinator, to schedule with the
  user, alongside the §2.11.6-B playtest below -- one session covers both.]**
  **[THE PLAYER-FACING HALF IS NOW DISCHARGED TOO, 2026-08-23, LATER THE SAME DAY. The user played
  a match to a result with the fix in the tree and destroyed the AI's flag, and the gate was
  OBSERVED REFUSING LIVE:
  `STRAT-MATCH concluded turn=5 turnCap=20 sideToMove=0 tier=Decisive`, then
  `End turn: the match is over; no further commands are accepted` and
  `Selection: the match is over; no further commands are accepted` TWICE as they kept clicking.
  Note `sideToMove=0` -- they won, the mirror image of the turn-7 session that found the defect.
  So TWO of the three gated call sites are now witnessed by a human where no clause can reach
  them; only the production-menu build path is still unwitnessed. **This defect is closed.** The
  entry stays because the diagnosis is what the fix was built from.]**
  Reported by the user as "at turn 7 the teams switch and I'm no longer in control of team 1 but
  have taken over control of team 2, and the AI no longer is in control". The log carries the whole
  chain and the report is accurate.
  - **THE TRIGGER IS A LEGITIMATE WIN.** On the AI's turn 7 its NINTH AND LAST applied command
    killed side 0's flag: `STRAT-COMBAT resolved attacker=6 defender=1 hex=-2,4 ... ddied=1`, with side 1's
    Fame moving 600 -> 1100 on the kill award. Axial `(-2, 4)` is side 0's Tank FLAG start hex, and
    `Turn.h` states a downed flag ends the match. So the RULES are right and the match is over.
  - **THE ENGINE HALF-NOTICES.** Three milliseconds later, at `15.14.34:394` against the kill's
    `15.14.34:391`, it logged
    `Slot 'StratocracyMatch' now records a completed match; ~2.11.6 guidance is suppressed from
    here on.` -- so the completion was detected and persisted. What did not happen is any
    transition out of play: no victory state, no input lockout, no turn advance.
  - **AND THE REASON IS THAT NO END-OF-MATCH TRANSITION EXISTS TO MISFIRE.** Stated this way on the
    re-gate's suggestion, because it is stronger than the log's silence and does not depend on it:
    there is no victory or defeat log line anywhere in the codebase, so an absence in the log would
    have proved nothing on its own. What proves it is the CODE. A scan of `Source/StratPlay` and
    `Source/StratUI` outside `Tests/` for
    `MatchOver|EndMatch|OnMatchEnd|MatchEnded|Victory|Defeat|GameOver`
    returns five hits and NOT ONE is a transition: four are the
    `bRecordCompletionOnMatchEnd` save-write flag, and the fifth is a comment saying the thing does
    not exist. **[CORRECTED 2026-08-23 by `strat-gameplay-engineer`, re-run in the tree and
    confirmed by the coordinator: that fifth hit and the `IStratAiTurnPort` design note quoted two
    paragraphs below are THE SAME HIT, not two independent facts. The count of five is right; the
    reading that five separate sites each declined to be a transition is not. The conclusion is
    unaffected -- none of the five is a transition either way.]** And no `IsMatchRunning` gate
    stands on the player input path -- stated exactly:
    that symbol does not exist anywhere in `Source/`, 0 hits tree-wide, so there is no gate to be
    unwired. The `no match is running` refusal in the log is the RULES module's own
    (`[T-SAVE-05]`), not `StratPlay`'s, which is the point.
    **THE DESIGN INTENT IS ON RECORD AND ONLY HALF OF IT IS BUILT, which is the whole defect in one
    sentence.** `IStratAiTurnPort`'s own header -- the INTERFACE, not the concrete
    `FStratBridgeAiTurnPort` that implements it -- says there is "deliberately no `IsMatchOver()` and
    no `IsSideAi()`: whether this side should be playing at all is decided before `RunTurn` is
    called, by the subsystem that has the view model." The runner is right to delegate; the
    subsystem never decides. So this is not a transition that fired wrongly -- it is a delegation
    with no receiver, and the AI's refused `EndTurn` is the first thing downstream that notices.
    **THE CONTROL on that scan**: the same pattern across all of `Source/` returns 33 hits, so the
    grep can speak; the five above are what survives excluding `Tests/`.
  - **WHAT THE PLAYER THEN GETS.** The AI's own `EndTurn` was refused --
    `STRAT-AI refused phase=apply kind=EndTurn ... turn=7 side=1 reason=[T-SAVE-05] no match is
    running`, and the runner reported `the paced AI turn refused ... after 9 applied command(s)`.
    Because that EndTurn never lands, the turn stays 7 and the side to move stays 1, so the human
    is now driving the AI's side. Measured, not inferred: the refusal SHAPE
    `Selection: unit <N> belongs to side 0 and side 1 is to move` fired four times as the user
    clicked their OWN units, for units 24, 24, 5 and 21 -- the count is the shape's, and no single
    one of those lines carries it -- while
    `STRAT-CMD accepted kind=Move unit=14 ... side=1` and
    `STRAT-CMD accepted kind=Attack unit=14 ... side=1` show side 1's units accepting the human's
    commands. The AI is not "no longer in control" by any switch -- its turn never terminated.
  - **AND THIS IS WHERE THE SUPPRESSING SAVE COMES FROM, which links it to ~2.11.6-B.** The
    completed-match slot that suppressed the guided opening on three consecutive runs is written by
    exactly this path. A match ending in a lost flag leaves a save that silently disables ~2.11.6
    guidance for every later run, and the only symptom is a blank strip.
  - **NOT DIAGNOSED BEYOND THIS, deliberately.** Whether the fix belongs at the AI turn runner, at
    the match subsystem's completion handling, or at both is an engine call. OWNED:
    `strat-gameplay-engineer`; `engine.md` is its file and nothing here is written into it. A
    clause is owed alongside -- OWNED: `strat-test-author`.

- **SEC 2.11.6-B: THE GUIDED OPENING NOW ARMS, AND THAT IS ONE RUNG, NOT THE SECTION.**
  2026-08-23. `Guided opening armed for side 0: objective hex (2, 7), window turns 1-4.` was
  logged at 15:09:49 in a human-driven session, with the control that the suppression line is
  present in all three earlier sessions and absent from that one. The cause was the completed-match
  save and nothing in the widget or the C++; `content.md` carries the evidence.
  **THE SECTION DOES NOT CLOSE AND IS NOT CLAIMED HERE.** The armed run lasted 24 seconds and
  issued no `EndTurn`, so BEAT 1a remains the only beat observed from the strip's side. Beats 1b
  and 2, the window-end tag and the gated end-turn hovers are all still unseen, and the window-end
  tag needs turns 1-4 to elapse. **One thing is newly known and is worth more than it looks:**
  `Guided beat 3 retired on turn 1: a unit spawned.` fired unprompted, which is a beat past 1a
  observing the world and retiring correctly -- the first evidence in this record that the beat
  machinery advances at all. It is not a strip claim: nothing read painted text back this session,
  so whether the strip PAINTS a readable directive with the fix in the tree is STILL unverified by
  a human eye.

- **2026-08-23, COORDINATOR, THE STRIP'S LAST QUESTION IS ANSWERED AND THE DEFECT IS FIXED IN THE
  ASSET. TWO SHIPPED ASSETS CHANGED, NO SOURCE. SEC 2.11.6-B IS STILL NOT CLOSED AND IS NOT CLAIMED
  HERE.** The banner at the top of this file carries the account; `content.md` carries the assets,
  the measurements and the API findings. This entry carries only what is now owed.
  - **THE PRIOR SEPARATION HELD, WHICH IS WORTH SAYING BECAUSE IT WAS THE EXPENSIVE PART.** The
    entry that read "what remains is inside the widget asset, and that is now separated from C++ by
    measurement rather than by argument" was RIGHT, and the exclusion list the two sessions before
    it built is what made the remaining question small enough to answer in one pass. Nothing in
    those entries is retracted by this one.
  - **WHAT CLOSES SEC 2.11.6-B, UNCHANGED: A HUMAN-DRIVEN PLAYTEST.** Only BEAT 1a has been seen --
    the directive present at match start, which needs no input. Beats 1b and 2, the window-end tag
    and the gated end-turn hovers all need someone at the keyboard, because injected input never
    reaches `UPlayerInput` on this project. OWNED: coordinator, to schedule with the user.
  - **A NARROW RE-GATE IS OWED, ON A TREE NO VERDICT DESCRIBES.** **[DISCHARGED 2026-08-23:
    `VERDICT: PASS`, zero findings, and the byte question this bullet hands the gate is settled as
    byte-lossless. See the banner. Kept rather than deleted because the reasoning for why it was
    owed is what a later reader needs when the next asset-only pass lands.]** The standing `VERDICT: PASS`
    describes a tree without `Content/UI/WBP_DirectiveStrip.uasset` or
    `Content/UI/WBP_OneShotTip.uasset` as they now stand. One thing for that gate specifically:
    `WBP_OneShotTip`'s probe wrote ordinary Blueprint variables on the CDO, so its restore is a
    representational match rather than a proved byte match and its saved bytes may carry more than
    the two wires. `WBP_DirectiveStrip` has no such exposure -- `Guidance` is `Transient`, so
    nothing planted there could reach disk, and that asymmetry is the reason only one of the two
    needs looking at.
  - **T-UI-04 IS UNTOUCHED BY THIS PASS AND STILL NEEDS ITS OWN HUMAN-DRIVEN PLAYTEST.** The
    production menu was not opened, not driven and not measured here; the census that found the
    binding defect confirms `WBP_ProductionMenu`'s one function graph does NOT carry it, and that
    is the only thing this pass says about it.
  - **BEFORE ANY FUTURE GUIDED-OPENING CAPTURE, READ THE ARMING LINE.** An existing
    `Saved/SaveGames/StratocracyMatch.sav` makes the game log `Guided opening suppressed for side
    0: this save has a completed match`, leaving the live projection all-default with no error
    anywhere -- a capture taken then is a FALSE NEGATIVE indistinguishable from the defect that was
    just fixed. It cost this session a cycle. `content.md` records the handling.

- **2026-08-22, COORDINATOR + STRAT-DATA-STEWARD, SEC 2.11.5 STEPS 1-3 ARE LANDED: THE RULES
  MODULE'S BUILDLIST ANSWER IS RE-VENDORED, ROUTED THROUGH THE BRIDGE, AND MIRRORED INTO THE VIEW
  MODEL. THIS RECORDS A LANDED STEP AND NOT A CLOSED PHASE.** Step 4 -- the production menu widget
  itself -- HAS NOT RUN, it is `strat-editor-builder`'s lane and needs a live editor, and **no
  verdict on Sec 2.11.5 is claimed anywhere in this entry.** The banner above carries the suite
  count for the tree these three steps produced; this entry does not restate it.
  - **THE RE-VENDOR, COMPARED BY BLOB RATHER THAN BY WORKTREE DIFF.** `rulesCommit` moved
    `cb8e12b` -> `cae01e3`, the crew repo's `main`, whose working tree is clean. Exactly two
    vendored hashes moved in `StratRules.manifest.json`, `Ui.h` and `Ui.good.cpp`; the vendored
    file count is steady at 24 and `Data/` is byte-untouched. The comparison is `git hash-object`
    and the manifest's own recorded digests, NOT a plain diff against the crew checkout -- that
    checkout is CRLF and the vendored copy is LF, so a plain diff reports identical bytes as
    moved. Upstream, the answer is gated by `GATE-BUILDLIST`, 14 clauses in the crew's
    `cpp_reference/test_ui.cpp`.
  - **WHAT STEP 3 ADDED, CITED AS SYMBOLS.** `FStratBridge::BuildOptions` is the new bridge seam,
    carrying the module's answer on the same two channels `Forecast` already uses -- `FStratResult`
    says whether the query was answerable, and a row's own `bAvailable` says whether it may be
    built. **[THE SENTENCE THAT STOOD HERE IS WITHDRAWN AS FALSE, 2026-08-22 -- it said "A hex
    that is not a factory is a REFUSAL rather than an empty menu, which keeps `bOk` false meaning
    one thing." It was never measured. `FStratBridge::BuildOptions` passes the factory hex through
    UNTOUCHED and refuses only on unloaded definitions, an unseeded scenario, a side outside the
    match, and an empty set; a hex that is not a build point is an ANSWER on each row's
    `bAvailable`, carrying the module's own reason. THE HALF THAT SURVIVES IS THE ONE ABOUT
    EMPTINESS -- a successful call always carries one row per §2.4 row, so an empty menu is never
    an answer. THIS BULLET IS WHERE THE CLAIM ORIGINATED: it reached the step-4 kickoff note from
    here, and the coordinator's dispatch brief carried it on into
    `UStratMatchSubsystem::RefreshProductionMenu`'s declaration and into `engine.md`, all since
    corrected. `strat-test-author` measured it false and refused the clause asserting it;
    `GATE-BUILDMENU.ANonFactoryHexOpensAnUnavailableMenuAndIsNotRefused` pins the truth. Found by
    `strat-integration-reviewer` gating step 4, which BLOCKED on this one finding.]**
    `FStratBuildOptionView` is the reflected row, carrying `DefIndex`,
    `CostFame`, `bAffordable` and `bAvailable`; `StratBuildProductionMenu` fills a caller's array
    from it. **`bAffordable` is resolved module-side on purpose** -- T-UI-03 forbids widget-side
    arithmetic, and this is what stops the menu ever comparing a price to a purse.
  - **THE ELEVEN NEW CLAUSES ARE DELIBERATELY NOT T-UI-04, and Row 8's ledger row does not flip on
    any of this.** T-UI-04 asserts that the production MENU BINDS, which is an in-editor claim over
    a widget that does not exist yet. Counting these clauses against it would close a row on
    evidence that never touched the thing the row is about. They are `GATE-BUILDMENU` instead.
  - **THE INTEGRATION GATE, AND THE EXIT CODE THAT IS NOT A VERDICT.** Row 9 reports
    `VERDICT: passed` at exit 0 with both rows graded, T-INT-01 and T-INT-04. **T-INT-04 only
    becomes gradeable with a compiler on PATH.** Run `run.py --integration` from inside the VS
    environment; without `vcvars` this box has no compiler the gate can find, T-INT-04 SKIPs, and
    the runner exits 2 -- which means "could not run", not "passed". Read the verdict line, never
    the exit code alone.
  - **A GUARD DEFECT FOUND AND FIXED IN THE SAME PASS, and the handoff's diagnosis of it was
    WRONG.** `strat_doc_citation_gate.py` reported 30 findings, every one inside a linked
    `git worktree` under `.claude/worktrees/` holding that worktree's own copy of the FROZEN
    `Tools/architect/state.md`. The real tree had zero. The handoff called that directory
    gitignored; it is not -- `git status` lists it `??` and `git check-ignore` on a file inside it
    exits 1. Git reports it as ONE untracked entry and never descends, because the `.git` entry
    inside it marks a repository boundary; `os.walk` has no such notion and walked straight in. The
    gate now prunes at that boundary and also skips genuinely ignored files, both filters carrying
    fixtures in `--self-test` in BOTH directions. Falsifiability was measured, not assumed: a live
    vendored reference written as ordinary prose into this very file was still caught and named,
    the file restored byte-identical by SHA-256 afterwards; and with each filter sabotaged in turn
    its own fixtures report `**WRONG**`. The scan went from 84 documents to 25. **The pre-commit
    hook's temp tree of staged blobs is not a git repository and is therefore filtered not at all**,
    which is correct rather than tolerated -- those blobs come out of the index and are tracked by
    construction. This is the fourth time this project has found the fault in a guard rather than
    in the code the guard watches.
  - **A KNOWN GAP, FLAGGED RATHER THAN QUIETLY CLOSED.** "Zero Fame leaves every row available" is
    pinned **BY PROXY, NOT LITERALLY**: no bridge seam sets a side's purse and `Data/` is read-only,
    so the clause asserts the general property against a row found on the shipped board. Pinning
    the literal case needs a spend-to-zero fixture sequence or a bridge-side test seam. **Neither
    was added**, because adding a seam to the bridge to make a test easier is a design change
    wearing a test's clothes. Whether it is worth one is a deliberate decision for a later lane.

- **2026-08-22, COORDINATOR, THE DESIGN-TIME SENTINEL EXPERIMENT WAS RUN AND IT ANSWERS THE FORK
  THE ENTRY BELOW LEFT OPEN: THE BINDING IS DELIVERING, AND THE BOUND FUNCTION SUPPLIES A VALUE
  THAT PAINTS ZERO-WIDTH. Sec 2.11.6-B STILL DOES NOT CLOSE, and nothing here claims it does --
  what closes that section is a human-driven playtest, unchanged, and the strip still paints no
  directive.** The entry below assigned this experiment to `strat-editor-builder` and it was run by
  the coordinator instead, for the reason that entry already recorded: `ToolSearch` for
  `execute_script` returned nothing while the same keyword search returned `unreal_status` and
  `list_unreal_projects`, so the builder -- NeoStack tools plus Read/Grep/Glob/Skill and no Bash --
  had no route to the editor. Driven over the editor's own MCP endpoint with `Tools/neostack/ns.py`.
  - **THE EDITOR WAS NEVER THE PROBLEM, AND A RESTART WAS PROPOSED AND REFUSED ON MEASUREMENT.**
    `unreal_status` reported *"No active NeoStackAI editors were found"* while `UnrealEditor.exe`
    was alive, `LISTENING` on 9315 by `netstat`, carrying a heartbeat in `runtimes.json` that
    matched the file mtime to the second, and answering `initialize` with
    `serverInfo: unreal-editor`. Its `tools/list` served `execute_script`. The proxy's "Reason"
    string describes a read it never performed. Restarting the editor would have cost the session
    and fixed nothing.
  - **THE MUTATION, AND IT IS FULLY REVERTED.** `DirectiveText`'s design-time `Text` was set to
    `SENTINEL9137SENTINEL9137` via `configure_widget`, compiled and saved with PIE stopped, then
    restored to `NSLOCTEXT("UMG", "TextBlockDefaultValue", "Text Block")` the same way. Saving
    moved the package bytes even though the value round-tripped, so the gated bytes were put back
    with `git restore`: `git hash-object` on `Content/UI/WBP_DirectiveStrip.uasset` reads
    `c4eb264740ffaa0daeab8843caf7a9a38690dada`, identical to `git rev-parse HEAD:<path>`. The
    editor reports zero dirty content packages and zero dirty map packages afterwards.
  - **THE NEGATIVE OBSERVATION: THE SENTINEL DID NOT PAINT, TO THE PIXEL.** PIE from the game's own
    startup path on `Lvl_FerrumCrossing`, guidance populated on the live widget exactly as in the
    entry below -- `bActive=True, Beat=Beat1a`, the same 76-character directive,
    `bHasObjectiveRing=True, ObjectiveHex=(X=2,Y=7), bEndTurnGated=True`. `Shot showui` at
    2544x1320 gives the strip rectangle at x 750-805, y 157-194, w=56, h=38, 2128 px, centre 777.5
    -- byte-for-byte the recorded baseline, and identical to the earlier capture retained as
    `ScreenShot00059.png`. Width 56 is still the border's own 28+28 padding, so the painted content
    is STILL ZERO-WIDTH with a 24-character sentinel sitting in the property.
  - **THE POSITIVE CONTROL IS THE WHOLE VALUE OF THIS ENTRY, because a blank screen is also what a
    broken experiment looks like.** A sentinel that never reached the running widget would paint
    nothing for reasons having nothing to do with the binding. It reached it: `GetAll TextBlock
    Text` on the live PIE instance returned
    `GameInstance_3.WBP_DirectiveStrip_C_0.WidgetTree_0.DirectiveText.Text =
    NSLOCTEXT("", "78DBFAF7426A5FBCC04C17B9C4577B9A", "SENTINEL9137SENTINEL9137")`. The same
    command returned real `LOCGEN_FORMAT` values for the scoreboard's live TextBlocks in the same
    output, so the reader was shown able to speak on populated live state rather than assumed to
    be. This works because a UMG binding drives the Slate attribute WITHOUT writing the UPROPERTY
    back, so `GetAll` on a bound property reports the design value -- the property read and the
    painted read are two different measurements, which is exactly what this experiment needed.
  - **THE INFERENCE, AND ITS LIMIT STATED RATHER THAN ROUNDED OFF.** A TextBlock with no active
    binding paints its UPROPERTY. This one holds a 24-character sentinel, is `Visible` on the live
    instance, and paints zero-width. So a binding IS active and IS supplying the painted value, and
    that value is not the sentinel and not the 76-character directive. **What was measured is
    zero-width painted content, NOT literally `FText::IsEmpty()`** -- an empty `FText` is the
    natural reading and the only survivor of the exclusion list below, but the measurement is the
    width. `DirectiveText`'s own `Visibility` is unbound, so the `Visible` read is true runtime
    state and a collapsed-text-block branch is dead too.
  - **WHAT IS NOW EXCLUDED, added to the list the entry below built rather than replacing it.** Not
    the input wiring, not variable shadowing, not an unregistered binding, not a dead widget tree,
    not the value on the instance, not the purity of the four graphs -- and now **not a binding
    that fails to run**. The binding runs, wins over the UPROPERTY, and returns something that
    renders to nothing.
  - **THE ONE QUESTION LEFT, and it is narrower than anything this section has carried before.** Why
    does a pure graph wired `Get Guidance` to `Break Strat Guidance View` to the `DirectiveText`
    member to the Return node yield a zero-width value on the very instance whose `Guidance`
    reads a populated 76-character `DirectiveText` in the same PIE session, seconds apart, from the
    same console? Both halves of that sentence are measured on `WBP_DirectiveStrip_C_0`. Next lane
    should read what the binding function actually RETURNS rather than whether it runs -- `ke`
    confirms execution but prints no return value, which is the gap that kept this open.
  - **INSTRUMENT NOTE.** All asset work was done with PIE stopped, per the hazard the entry below
    records; the editor survived both PIE teardowns and no widget was opened while PIE was live.
    The captures live under `Saved/Screenshots/WindowsEditor/`, which is not tracked, so quote the
    geometry above rather than relying on those files surviving.

- **2026-08-22, COORDINATOR, LIVE PIE ON THE INTEGRATION TREE, NO SOURCE AND NO ASSET CHANGE --
  THE DELIVERY FIX IS CONFIRMED ON A LIVE WIDGET, AND THE STRIP STILL PAINTS NO DIRECTIVE.
  Sec 2.11.6-B DOES NOT CLOSE. What remains is inside the widget asset, and that is now separated
  from C++ by measurement rather than by argument -- editor-builder's lane.** The session this
  banner asked for was run: editor launched on this tree, PIE from the game's own startup path
  (`Game class is 'BP_StratGameMode_C'`, then `Guided opening armed for side 0: objective hex
  (2, 7), window turns 1-4` at `12.01.00:065`), and no asset opened at any point in it.
  - **THE DELIVERY HALF WORKS, and this is the first time it has been seen on a live widget.**
    At t+2.4 s, `GetAll StratGuidanceWidget Guidance` on `WBP_DirectiveStrip_C_0` returned
    `bActive=True, Beat=Beat1a, DirectiveText="Select the marked Infantry. Lit hexes are its true
    reach. Click one to move.", bHasObjectiveRing=True, ObjectiveHex=(X=2,Y=7), bEndTurnGated=True`
    -- the whole decorated projection, with no `ke` and no input. The comparable read on
    2026-08-21 was taken at t+2.5 s and returned all-default. Same instrument, same moment in the
    match, opposite result.
  - **THE PAINTED STRIP DID NOT MOVE, to the pixel.** `Shot showui` gives the same 2128-pixel
    rectangle at x 750-805, y 157-194, centre 777.5 in a 2544x1320 frame, which is what all six
    2026-08-21 captures gave. Width 56 is the border's own 28+28 padding, so the content is still
    ZERO-WIDTH: no glyph of the 76-character directive is on screen.
  - **THE DIFFERENTIAL THAT IS NEW, and it is why this run was worth taking.** Every prior capture
    was taken while the guidance was default, so "the painted state is not a function of the
    guidance" rested on three states that never differed in the variable. This run supplies the
    other side: guidance populated, output byte-identical. The variable was finally VARIED, and the
    screen did not respond.
  - **What is already correct, so the next lane does not re-measure it.** `bp:list_bindings()`
    returns all four as `kind=Function` with the right `object_name`/`property_name`/
    `function_name` -- `DirectiveText.Text` to `GetDirectiveText`, `StripBorder.Visibility` to
    `GetStripVisibility`, and the two tag ones. All four graphs are fully wired: `Get Guidance` to
    `Break Strat Guidance View` to the right member to the Return node, and `Active` to
    `To Visibility (Boolean)` whose False pin is `Collapsed`. The blueprint has ZERO local
    variables and its parent is native `StratGuidanceWidget`, so the variable-shadowing branch the
    2026-08-21 correction kept alive is DEAD -- there is nothing to shadow with. And in the live
    PIE world the tree is fully instantiated: `WidgetTree_0` with `Root`, `StripBorder`,
    `StripRow`, `DirectiveText` and `WindowEndTag`, every one of them `Visible`.
  - **A LEAD WAS RAISED HERE AND IS WITHDRAWN IN THE SAME PASS, BEFORE ANY LANE ACTED ON IT.**
    What was observed is real: in all four binding graphs the `K2Node_FunctionEntry` carries an
    exec output pin and the `K2Node_FunctionResult` an exec input pin, and BOTH ARE UNCONNECTED.
    The reading built on it was that these are ordinary impure graphs, so no body ever executes and
    each binding takes its Return pin default -- which would have matched the screen exactly
    (`Visible` for the border whatever `bActive` is, empty for the Text ones). **That reading is
    almost certainly WRONG, and the thing that refutes it was already recorded.** The UMG compiler
    REFUSES a binding to a non-pure function, in its own words -- *"property 'TextDelegate' on
    widget 'DirectiveText' needs to be bound to a pure function, 'Get Directive Text' is not
    pure"* -- and that error is why these four were authored with `add_function(..., pure=true)` in
    the first place. The generated class carries all four bindings, so the blueprint compiled with
    them; therefore the four functions ARE pure, and this reader reports entry/result exec pins on
    a pure graph too. **The pin dump was never evidence of impurity, and it is the reader's shape
    rather than the asset's.** Recorded rather than deleted because the observation stands and the
    next reader will otherwise draw the same inference from the same dump.
    **[CONFIRMED BY MEASUREMENT 2026-08-23 afternoon, and this bullet was RIGHT -- including its
    prediction of what the next reader would do. The 2026-08-23 morning pass drew exactly the same
    inference from the same dump and wrote it into the record as fact; it has now been corrected in
    both files. A `pure=true` function does show entry/result exec pins, verified side by side
    against a `pure=false` twin on a throwaway probe, and the compiler's refusal of the impure twin
    is what proves the shipped six are pure. What this bullet did NOT reach, and what kept the
    section open for another day, is that purity does not exempt the body from needing an exec
    wire -- so "the four functions ARE pure" and "no body ever executes" are BOTH true at once, and
    the mechanism this bullet then called still-open was in fact the unconnected exec pins it had
    just finished describing. `content.md`'s topmost CORRECTION bullet carries the experiment.]**
  - **SO THE MECHANISM IS STILL OPEN, and these are excluded rather than guessed at.** Not the
    input wiring (`Return Value` reports `connected=true, linked_to_count=1` on every binding
    function). Not variable shadowing (zero local variables; the reparent re-scoped the member to
    native `StratGuidanceWidget:Guidance`). Not an unregistered binding (all four present, correct
    triple). Not a dead widget tree (fully instantiated, all five `Visible`). Not the value
    (`bActive=True` with the 76-character directive on the live instance). The four functions also
    EXECUTE on demand: `ke * GetDirectiveText`, `ke * GetStripVisibility` and
    `ke * GetWindowEndTagText` each answered `1 instances succeeded` with no `Bad or missing
    property` line above the count, which is the positive control on that call -- though `ke`
    prints no return value, so what they RETURN is still unmeasured.
  - **THE ONE EXPERIMENT THAT SEPARATES THE TWO SURVIVORS, and it is a Content/ write, so it is
    the editor-builder's to run.** Set `DirectiveText`'s DESIGN-TIME `Text` to a distinctive string
    and take one PIE capture. If that string PAINTS, the binding is not delivering and Slate is
    showing the UPROPERTY. If the strip stays blank, the binding IS delivering and the function is
    returning an empty `FText`. Today the design value is the stock `"Text Block"` and the screen
    shows NEITHER it nor the 76 characters, which is what makes the two indistinguishable from
    outside. Revert the design value afterwards; the asset is gated bytes.
  - **INSTRUMENT WARNING, AND IT COST A FALSE FINDING THAT WAS CAUGHT BEFORE IT REACHED THIS FILE.
    `open_asset` ON A WIDGET BLUEPRINT RECOMPILES IT, AND DOING THAT DURING PIE DESTROYS THE LIVE
    WIDGET TREE.** In the first session of this pass I opened `WBP_DirectiveStrip` to read its
    graphs while PIE was running. Afterwards `GetAll UserWidget WidgetTree` returned
    `WBP_DirectiveStrip_C_0.WidgetTree = None` with zero live children, while the scoreboard's
    `WidgetTree_0` and its fifteen children answered in the SAME command -- a control that made the
    absence look real. It was not real; it was my own recompile. What exposed it was that the box
    had already left the screen BEFORE the removal test meant to identify it, so the timeline did
    not fit. Re-run clean, with a fresh editor and nothing opened, the tree is fully live.
    **Nothing after the first `open_asset` in a PIE session is evidence about the game.** The same
    session then crashed the editor on PIE teardown at `PlayLevel.cpp:553`, the leak chain naming
    `TransBuffer` -- the editor's own undo buffer -- and no game object; a later `playtest_stop`
    with no recompile behind it ended PIE cleanly and the editor survived, which is the control on
    that half. Read the asset with PIE STOPPED, or not at all.
  - **Consequence for the lanes, stated because two of them have been waiting on it.** The
    engineer's delivery work is done as far as the screen can see: the value arrives. What remains
    is inside `WBP_DirectiveStrip` and belongs to `strat-editor-builder`. The two questions the
    2026-08-21 entry left open collapse to one, and it is NOT "does the binding take, or does its
    input read something other than the native guidance" -- the input wiring is confirmed correct
    and the shadowing branch is dead. It is: **does the delivered value reach Slate, or does the
    bound function return an empty one?** The design-time-text swap above answers it in one PIE
    capture.
  - **THE BUILDER COULD NOT BE DISPATCHED IN THIS SESSION, and the reason is the proxy, not the
    editor.** `ToolSearch` for `execute_script` returned nothing while the same keyword search
    returned `unreal_status` and `list_unreal_projects` -- the two-tool fallback surface, control
    passing. `strat-editor-builder` carries the NeoStack tools plus Read/Grep/Glob/Skill and **no
    Bash**, and a subagent only receives tools the session already has, so with `execute_script`
    absent it has no route to the editor at all. The editor itself was fine throughout: every
    measurement above was taken by the coordinator over the editor's own HTTP endpoint. Dispatch
    the builder from a session whose surface actually carries `execute_script`; do not spend a
    round having it discover the wall.

- **2026-08-22, COORDINATOR, MEASURED WHILE WRITING THIS PASS'S BANNER -- `strat_banner_sweep.py`
  CAN BE MADE TO SKIP THE BANNER'S LIVE COUNT ENTIRELY BY AN ORDINARY ENGLISH WORD, AND IT PASSES
  CLEAN WHEN IT DOES. Steward's lane; this is a guard going inert on its own subject.**
  **[STAMPED 2026-08-22, later session -- FIXED IN THE SCRIPT; true when written. Both repairs
  named below shipped, plus four the fix itself uncovered. The closing sub-bullet carries what
  landed and how it was measured; the diagnosis above is kept because the mechanism it names is
  the one that was repaired.]** The first
  draft of the banner above ended a sentence with "repeating the console reads with the fix in the
  tree" immediately before `THE SUITE **IS NOW 160/160**`. The sweep then reported
  `suite claims found: 18 (0 live, 18 stamped)` and `SWEEP CLEAN`, exit 0 -- with a banner asserting
  a suite count that no check had compared to anything.
  - **The mechanism, read out of the script rather than guessed.** `_QUOTED_FIGURE_RE` matches
    `said|says|saying|read |reads |reading|claim|shape|editing|planting|planted|wrongly|stale|
    defect|would have|reported` anywhere in the **120 characters before** a figure, and a match
    makes the loop `continue` -- the claim is not merely called stamped, it is never collected. The
    exclusion exists for a good reason (an honest account of a past miscount must be able to write
    the wrong number down) but it is far broader than that purpose: `reads`, `claim`, `shape` and
    `defect` are words this record uses constantly, and any of them within 120 characters upstream
    silences the live figure.
  - **The compounding half, and the reason this is worth a phase rather than a note: ZERO LIVE
    CLAIMS IS NOT AN ERROR TO THIS SCRIPT.** `global.md` is the only file permitted a live suite
    count, and a run in which it has none means either the banner is missing its figure or the
    figure was swallowed. Either way nothing was checked, and the script says `SWEEP CLEAN`.
  - **Rewording proved it, both directions.** With "reads" in that sentence: 18 claims, 0 live,
    exit 0. With the single word changed to "measurements" and nothing else touched: 19 claims,
    1 live, and the sweep printed its own line number back: `global.md:19 LIVE 160/160`, exit 0.
    (That figure is the sweep's OUTPUT quoted verbatim, not a citation this entry is making --
    stated 2026-08-26, when the widened doc-citation gate began reading record line numbers and
    reached this transcript.) The banner in this commit is the second version.
  - **Two repairs, and the second is the one that matters.** (1) Do not let the quoted-figure
    exclusion outrank an explicit live tense -- `is now` immediately before the figure is a
    deliberate assertion and should beat a stray `reads` 100 characters upstream. (2) **Refuse a
    sweep of `global.md` that finds no live suite claim at all**, the same way the pre-sliced
    corpus guard was made to refuse a zero-event corpus rather than pass it. A guard that cannot
    find its subject must say so, not report clean.
  - Owner: `strat-data-steward` (`Tools/architect/` is its lane). Found by the coordinator while
    writing a banner, not by the guard's own self-tests.
  - **FIXED 2026-08-22, later session, coordinator, in `Tools/architect/strat_banner_sweep.py`
    alone -- no record file's figures were touched.** The two repairs above landed, and four
    more that only became visible once they had. **[STAMPED with this sub-bullet: everything
    here is the state of the script as of this commit.]**
    - **The exclusion is scoped to the figure's own sentence** (`quoting_window`,
      `_SENTENCE_BREAK_RE`), so a reporting verb one sentence upstream no longer silences the
      claim after it. The boundary is tuned to this record's prose: `2.11.6-B`, `global.md` and
      a `reportCreatedOn` timestamp are not read as sentence ends.
    - **An explicit live tense outranks the exclusion** -- repair (1) as filed.
    - **`check_live_count_present` refuses a sweep of this file that finds no live count** --
      repair (2) as filed, and the one that makes the whole class non-silent.
    - **`in_code_span` replaces the single-character backtick test.** Uncovered by repair (1):
      the old test caught a figure written `` `107/107` `` but not one inside
      `` `THE SUITE **IS NOW 160/160**` ``, because the character before it is an asterisk.
      This record quotes its own banner verbatim, so the sentence above that reproduces the
      banner's first-draft wording was then collected as a SECOND live claim. It agreed with
      the banner by luck, so the sweep stayed green while carrying a claim it had misread.
    - **The tense markers now reach across a bold opener.** `was **N/N**` puts `**` between the
      verb and the number and the old `\s*$` anchor could not span it, so the verb was never
      seen and the claim fell through to the default, which is LIVE. Every such figure in this
      record carries an explicit stamp, which outranks the verb anyway, so the record masked it
      and so did two fixtures.
    - **The banner region is a stack of banners, newest first** (`current_banner_end`). This
      file has no heading until `## BUILT`, so `section_of` called everything above it "BANNER"
      -- eleven successive `_Last run` blocks. The existing "last banner figure wins" rule is
      right within one block and backwards across the stack: a post-milestone figure written
      2026-08-14 was collected as this file's live claim and superseded the real one.
      `check_banner_date` already reads only the FIRST `_Last run` as current; the figures now
      follow the same rule.
    - **Measured, both directions, against this record rather than a fixture.** Planting a live
      count at each of the 766 non-blank lines of `## NEXT`: never-collected went from 132 to 38
      for the bolded wording and 134 to 38 for plain prose, and the residual 38 are the probe
      landing inside an unclosed code span on the line it was appended to. In `engine.md`, 29 to
      zero. Seven realistic wordings planted as their own bullet -- sentence-final full stop,
      closing paren, after a colon, all caps, an em-dash clause, mid-list with commas, and one
      with no live verb at all -- exit 0 on the old script for all seven, exit 1 on the new one
      for all seven. Stamping the banner's own live figure out: exit 0 before, exit 1 now with
      `LIVE COUNT MISSING`. The untouched record exits 0 on both.
    - **The bolded-versus-plain framing this entry was filed under was a symptom.** The figure
      pattern has matched a bare `N/N` since 2026-08-20. Both wordings were swallowed at the
      same 132 sites; they diverged at exactly two, where the bold form's extra two characters
      pushed the reporting verb past the 120-character cut. Read the window, not the markup.
    - Self-test now carries 29 inline fixtures, six of them for these repairs, including a pair
      that fails in one direction and passes in its near-twin so the scoping cannot quietly
      widen. CI's three steps all exit 0.

- **2026-08-21, COORDINATOR, LIVE PIE, NO SOURCE AND NO ASSET CHANGE -- THE THREE t+0 BRANCHES ARE
  SEPARATED, AND TWO OF THEM ARE REAL.** Branch 1 is eliminated by an artifact already in the log,
  branch 2 is confirmed, and branch 3 is CONFIRMED AS A MECHANISM -- an undecorated reconcile was
  driven and the strip's guidance was watched being wiped. This answers question (2) of the entry
  below. Question (1), the binding, is untouched and still open. Measured 23:40-23:53 LOCAL on
  2026-08-21 and filed after local midnight, so the commit that carries it is stamped the 22nd:
  the entry date records when the work was measured, not when the filing landed, and the log's own
  dual stamp settles it (`[2026.08.22-03.53.09]` carries `at 2026.08.21-23.53.09`). **Both defects
  have to be fixed:
  correcting the match-start ordering alone would leave any later undecorated reconcile clearing
  the strip -- and the AI turn is one, by the same call, though that specific caller is inferred
  rather than driven (see branch 3).**
  - **BRANCH 1 -- `FindScoreboardHUD` returned null: ELIMINATED, on evidence every log in this
    investigation already carried.** `UStratMatchSubsystem::HandBridgeToScoreboard` obtains the HUD
    through `FindScoreboardHUD()` and returns early on the null branch, so `AdoptBridge` is
    unreachable without one; `Bridge adopted: this HUD now draws a match it does not own` appears at
    match start in every session. Nothing new had to be run.
  - **BRANCH 2 -- `GuidanceStrip` was still null: CONFIRMED, by a chain whose every link is a log
    artifact, a straight-line reading, or a graph with zero connections.**
    (a) The HUD's terminal `BeginPlay` line reads `Scoreboard live on an adopted bridge (this HUD
    seeded nothing)`, the branch taken only when `AdoptedBridge` is already set -- so that
    `BeginPlay` ran AFTER the handover.
    (b) Nothing that `AStratScoreboardHUD::BeginPlay` reaches can log the arming line, so its
    terminal line bounds its whole execution. Checked on BOTH sides rather than asserted: in C++,
    the adopted path calls `Super::BeginPlay`, `ViewingSideInRange`, `CreateGuidanceWidget`,
    `CreateScoreboardWidget` and `RefreshScoreboard`, none of which reaches
    `AStratPlayerController::RefreshFromMachine`, the sole route to that log line; and on the
    Blueprint side, which is where re-entry would hide, `BP_StratScoreboardHUD`'s EventGraph and
    `WBP_DirectiveStrip`'s EventGraph each hold three event stubs with **zero connections** --
    `ReceiveBeginPlay`, `PreConstruct` and `Construct` execute nothing. `WBP_Scoreboard` is the
    ONLY one of the three constructed late enough to have broken the chain, and it is closed by its
    ENTRY POINTS rather than by counting its interior: its four events are `PreConstruct`,
    `Construct`, `Tick` and `OnScoreboardRefreshed`, and the first three have UNCONNECTED exec pins
    -- only `OnScoreboardRefreshed` runs anything. Nothing in that graph executes at
    `CreateWidget`/`AddToViewport` time at all, which settles it whatever the interior holds.
    Corroboration rather than the load-bearing link: 39 nodes, 44 connections, and all nine of its
    `K2Node_CallFunction` nodes are `SetText (Text)`. Only a re-entry AFTER `CreateGuidanceWidget`
    could have broken the chain, and the one graph that could have is closed at its door.
    (c) `CreateGuidanceWidget` runs inside that `BeginPlay`, above the scoreboard block and so
    before the terminal log.
    (d) In FIVE fresh PIE sessions the order was identical -- adopt, `Guided opening armed`, then
    the terminal line -- and by (b) the `BeginPlay` cannot straddle the arming line, so it STARTED
    after it.
    Therefore at the decorated `ApplyView`, which runs in the same `RefreshFromMachine` call that
    logged the arming, `GuidanceStrip` was null and `AStratScoreboardHUD::PushGuidance` -- whose
    whole body is a null check and a forward -- dropped it silently. That silence is deliberate and
    documented: "no strip is a configuration, not a refusal". The strip is then built holding the
    default, and in those five sessions the read ~2.9 s later is all-default.
  - **BRANCH 3 -- a later UNDECORATED `ApplyView` overwrites it: CONFIRMED AS A MECHANISM, driven
    and observed.** `ke * RefreshFromMachine` put the full decorated projection on the widget;
    `ke * SetViewingSide 0` then ran `UStratMatchSubsystem::SetViewingSide` ->
    `RefreshPresentation` -> `BuildViewModel` + undecorated `ApplyView` -> `PushGuidance`, and the
    next read one second later was `bActive=False, Beat=None, DirectiveText=""`. **An undecorated
    reconcile clears the strip, watched happening.** It does NOT fire spontaneously: after an
    earlier decorated push the projection survived on the widget for 46 seconds of live idle match.
    Production callers of an undecorated `ApplyView` are exactly three --
    `UStratMatchSubsystem::StartMatchInternal`'s first reconciliation, which does it deliberately
    and says why in its own comment, and `RefreshPresentation`, reached from `SetViewingSide` and
    from `RunAiTurnsNow`. The AI route is the one that matters in play and is the one NOT directly
    driven here, because taking a turn needs input; it is the same function through the same call,
    so it is a strong inference rather than an observation, and is labelled as such.
  - **THE SHAPE OF THE DEFECT, which is larger than either branch.** The guided-opening decoration
    lives on `AStratPlayerController::RefreshFromMachine` alone, while three subsystem-side paths
    reconcile the same screen without it. At match start the strip does not yet exist for the only
    decorated push, and afterwards any subsystem-side reconcile clears what a decorated push left.
    Engineer's lane. Neither a call site nor a null check is the whole answer.
  - **INSTRUMENT, and it cost a wrong claim before it paid for one.** `ke` parses arguments
    POSITIONALLY: `ke * SetViewingSide 0` works, `ke * SetViewingSide InViewingSide=0` does not, and
    the bare form does not either -- both print `'SetViewingSide': Bad or missing property
    'InViewingSide'` and then, on the SAME command, `Called 'SetViewingSide' on everything in the
    world and 2 instances succeeded`. **A `ke` success count is not evidence the function executed**;
    read the lines above it. An earlier draft of this bullet concluded from those two failures that
    `ke` could not reach the function at all, and spent that conclusion to leave branch 3
    unmeasured -- a capability claim about the instrument, drawn from one unsupported syntax, that
    the gate caught and the positional form disproved in a single run.

- **2026-08-21, COORDINATOR, LIVE PIE, NO SOURCE AND NO ASSET CHANGE -- THE PUSH PATH WORKS, AND THE
  PAINTED STRIP IS NOT A FUNCTION OF THE DATA IT HOLDS.** **[THE FIRST HALF OF THAT HEADLINE IS
  RETRACTED -- read the CORRECTION that ENDS this bullet before acting on anything in it. The push
  path works once every participant exists; it did NOT work at match start, and the two links this
  bullet clears are not cleared.]** The ordered experiment the bullet below asked for was run, and
  it answers BOTH questions -- differently from the way either was framed. One fresh PIE session,
  three states, `GetAll` for the data and `Shot showui` for the screen at each.
  - **Question (2) is ANSWERED: the third branch is the one, and `FindScoreboardHUD` and
    `PushGuidance` are both innocent.** **[RETRACTED IN FULL -- see item 1 and item 2 of the
    CORRECTION ending this bullet. This is the one sentence here that changes what the next lane
    does, and it is wrong: it retires a branch that must still be measured at t+0.]**
    **[THAT MEASUREMENT HAS SINCE BEEN MADE -- newest bullet, five sessions of ordering plus a
    driven reconcile. DO NOT REDO IT. `GuidanceStrip` was null at t+0. Kept as a sibling marker
    rather than nested inside the one above, because markdown does not nest `**` and the nested
    form rendered this sentence UNbolded inside a bold retraction -- emphasis inverted on the one
    clause meant to stop a redo.]** State A, 2.5 s
    after `Guided opening armed for side 0`: the widget's `Guidance` reads all default. Then `ke *
    RefreshFromMachine` on the live controller, and state B reads `bActive=True, Beat=Beat1a,
    DirectiveText="Select the marked Infantry. Lit hexes are its true reach. Click one to move.",
    bHasObjectiveRing=True, ObjectiveHex=(X=2,Y=7), bEndTurnGated=True, EndTurnGateHover="Move the
    marked Infantry first.", LockedUnitHover="Locked this turn."` -- the whole decorated projection,
    delivered to the widget by the ordinary route. So the route from
    `FStratGuidedOpening::DecorateViewModel` through `ApplyView` and `PushGuidance` to the widget is
    WORKING. What is missing is the CALL: nothing runs `RefreshFromMachine` at match start, so the
    model that reaches `ApplyView` then is UNDECORATED and the unconditional push delivers a
    default. Engineer's lane, and the fix is a call site rather than a mechanism.
  - **Question (1) is ANSWERED AND ITS LEAD IS DEAD -- the answer is bigger than the question.**
    **[THE "LEAD IS DEAD" HALF IS RETRACTED -- see item 4 of the CORRECTION ending this bullet. This
    run CORROBORATES that lead's open branch rather than killing it. The pixel and channel wording
    in the rest of this sub-bullet is repaired there too, in the correction's trailing paragraph.]**
    The screen is PIXEL-IDENTICAL in all three states: the same 56x38 rectangle at x 750-805, y
    157-194, n=2128, in state A (`Guidance` default), in state B (fully populated, quoted above) and
    in state C after `ke * SkipGuidance` put it back to default. **A widget holding a 76-character
    directive drew exactly the same empty box as one holding an empty string.** So the fault is not
    specific to `Visibility` and there is nothing left to explain about `ToVisibility`'s false
    branch: the strip's Slate state is not a function of `Guidance` in ANY channel. The conditional
    lead recorded below -- "if the false pin resolves to a non-painting visibility then `Active`
    must be reading something true" -- is now MOOT rather than merely unmeasured, and is superseded
    here rather than deleted. Its premise never got to matter.
  - **What the text channel shows, and it is the sharpest clue in the run.** **[RETRACTED IN FULL,
    AND IT ENDS IN AN INSTRUCTION THAT MUST NOT BE ACTED ON -- see item 3 of the CORRECTION ending
    this bullet, which withdraws the whole `GetAll` elimination below, and item 4, which says of
    this sub-bullet's closing handoff: do not dispatch an engineer on it. This is the most
    actionable-looking paragraph in the entry and it is the one most fully withdrawn.]**
    `DirectiveText` renders NEITHER the bound value (state B's 76 characters) NOR its design-time
    default, which `GetAll TextBlock Text NAME=DirectiveText` confirms is still `"Text Block"` on
    the live instance. It renders EMPTY. An empty string is what the Text binding returns when
    `Guidance` is default -- which is what `Guidance` held at construction. That reads as a binding
    evaluated ONCE and never again, rather than one never applied; a binding never applied would
    have left "Text Block" on screen. `StripBorder` is the opposite shape: it shows its DESIGN value
    (`Visible`), where a binding evaluated once at construction would have returned `Collapsed` and
    drawn nothing. Two channels, two different failures, one widget. NOT DIAGNOSED HERE, and
    deliberately not: the next step is an engineer reading `UWidget::SynchronizeProperties` and the
    generated class's `InitializeWidget` against this asset, not another black-box round.
  - **Instrument note.** `ke * RefreshFromMachine` is a way to drive a reconcile with no simulated
    input at all, and this run is the first time it has been used to CHANGE observed state rather
    than to confirm one. `AStratPlayerController::RefreshFromMachine` is `BlueprintCallable`, which
    is what makes it reachable; `GetAll` then reads the result off the live widget.
  - **CORRECTION, 2026-08-21, same session, raised by the record gate on `68d56d1` and adopted --
    THE BULLET ABOVE EXONERATED THE WRONG BRANCH ON A TEST THAT COULD NOT SEE IT, AND ITS COMMIT
    MESSAGE CARRIES THE SAME ERROR WHERE IT CANNOT BE EDITED.** Four repairs, in order of how much
    they change what the next lane does. One method note, stated once because it produced three of
    the findings that got this bullet here: a fix applied to the sentences a gate ENUMERATES, rather
    than derived over the whole passage, leaves the tail asserting what the head withdrew. The
    attempt-by-attempt history lives in the commit messages of `68d56d1`, `de517f0`, `c21e7a5`,
    `d67471b` and `a55f924`, where it cannot be edited and does not compete with the state of the
    world; this file answers what the next lane should DO.
    1. **"nothing runs `RefreshFromMachine` at match start" IS FALSE, and the tree says so twice.**
       `AStratPlayerController::BeginPlay` calls it unconditionally, and its comment there
       anticipates this exact ordering ("actor BeginPlay order is not guaranteed"). Independently,
       `Guided opening armed for side 0` is emitted only by `FStratGuidedOpening::Begin`, reachable
       only through `TryArmGuidedOpening`, which has exactly TWO call sites -- `RefreshFromMachine`
       and `HandleSelectionEvent` -- and no input event occurred at match start. So
       `RefreshFromMachine` ran, cleared its bridge and build-model guards (they precede
       `TryArmGuidedOpening` in its body), decorated, and reached `ApplyView` with a DECORATED
       model, all in frame 27 at `03.06.08:571`. **And that model was decorated with a POPULATED
       block, which is measured rather than inferred:** `FStratGuidedOpening::Begin` returns before
       the arming line on BOTH its early exits -- the `bSuppressed` branch and the `!Guided.bOk`
       branch -- and sets `bActive = true` on the same straight-line path above it, with no
       intervening return, so the log line cannot be emitted by an armed-but-inactive opening. A
       later reader asking "armed, but suppressed?" has the answer here rather than a reopened
       question.
    2. **`FindScoreboardHUD` and `PushGuidance` are therefore NOT exonerated, and the bullet above
       had no evidence about them.** It tested them at t+2.5 s and t+6 s, when the strip already
       existed. The question is the state at t+0. `AStratScoreboardHUD::BeginPlay` creates the strip
       before the scoreboard, and its terminal `Scoreboard live` line lands at `08:573`, two
       milliseconds AFTER the arming line, in the same frame; `CreateGuidanceWidget` logs nothing on
       success, so **the log cannot resolve the order and neither can this pass.** What IS measured:
       a decorated `ApplyView` ran at match start, and 2.4 s later the widget still held defaults --
       so that push did not land on the strip, or did not survive. THREE branches, and this pass
       cannot separate them: `FindScoreboardHUD` returned null; `GuidanceStrip` was still null; or
       the decorated push DID land and a later UNDECORATED `ApplyView` overwrote it, `PushGuidance`
       being documented as assigning unconditionally. The third is not excluded by "no later
       `ApplyView` comes without input" -- `ApplyView` logs nothing, so the log's silence is not a
       control. TWO of the three sit inside the branch the bullet above calls innocent; the THIRD
       does not -- there both links did their job and the fault is a later undecorated `ApplyView`,
       which is the call-site story rather than the link story. That is exactly why this pass can
       neither convict nor exonerate those links, and why the t+0 ordering still has to be
       measured. **[ANSWERED by the newest bullet at the top of `## NEXT`, dated the same day: the
       t+0 ordering WAS measured, five sessions of it. Branch 1 is eliminated; branch 2 is
       CONFIRMED; branch 3 is CONFIRMED AS A MECHANISM -- an undecorated reconcile was driven and
       the strip's guidance was watched being cleared -- though it does not fire spontaneously.
       TWO of the three are real, and both need fixing. This item's refusal to convict or exonerate
       was right for the evidence it had; it is superseded, not wrong. (This marker first said
       "branch 3 does not fire at match start", which is the framing the newest bullet retired in
       the same pass for reading as an exoneration -- carried over verbatim instead of re-derived
       against the text it summarises.)]**
       `CreateGuidanceWidget`'s own comment already names the shape: "The strip draws its own
       defaults until the first `ApplyView`" -- it assumes a LATER `ApplyView`, and whether one
       comes without input is exactly what is NOT established here. **[NOW ESTABLISHED, BOTH WAYS,
       by the newest bullet: none comes spontaneously -- a decorated projection survived 46 s of
       live idle match -- and a driven one CLEARS the strip. The question this sentence leaves open
       is closed.]**
    3. **The GetAll-based elimination is WITHDRAWN -- it used an instrument this same file rules out
       for that purpose four bullets down.** The 2026-08-21 entry says: a UMG property binding
       drives the Slate attribute and never writes the UPROPERTY back, so read `GetAll` for unbound
       state only. Then "the live instance still holds `Text Block`" is equally what a FULLY WORKING
       binding leaves behind, and it eliminates nothing. So "a binding evaluated ONCE and never
       again", "a binding never applied would have left Text Block on screen", and "two channels,
       two different failures, one widget" are all unsupported and are retracted. Also disclosed,
       because the bullet above did not: those two reads were taken at `03.07.57`, ~98 s after the
       last capture and after `SkipGuidance` -- in state C, not in state B where the 76 characters
       were loaded.
    4. **The `ToVisibility` lead is NOT moot; this run is CONSISTENT with its open branch and
       predicts the text channel exactly.** That lead's second branch is that the binding takes and
       returns `Visible` because `GetStripVisibility`'s `Active` is not fed by the native
       `Guidance`. Three captures showing the screen is not a function of the native `Guidance` is
       precisely what that branch predicts, and ONE such defect -- bindings reading a source the
       native `Guidance` never reaches -- would explain both channels at once -- PROVIDED the false
       pin does not resolve to `Collapsed` at runtime, which is still only read off the graph, since
       a shadowed always-default source would take that false pin and the border would not have
       painted at all. "Both stand until one is measured" is restored. The handoff in the bullet
       above (read `UWidget::SynchronizeProperties` and `InitializeWidget`) rests on the reading
       withdrawn in item 3; **do not dispatch an engineer on it.** The binding's input wiring is at
       least as good a place to look.
    Two wording repairs, neither load-bearing but both stated because this record has been blocked
    for smaller: "pixel-identical" overstates the artifact -- re-derived independently, 283 to 309
    of the 2128 pixels differ between any two of the three frames, all by a single LSB of dither,
    max per-channel delta 1, with 3 to 4 unique colours and no glyphs in any state; the conclusion
    that nothing painted, resized or appeared is untouched. And "not a function of `Guidance` in ANY
    channel" should read "in any channel a composited capture can observe" -- the hover strings were
    never exercised and the objective ring is drawn by the board, not the strip.
    **What survives the correction, unaffected:** the route from `DecorateViewModel` through
    `ApplyView` and `PushGuidance` to the widget WORKS once every participant exists -- state B is a
    direct observation of the full projection arriving -- and the strip's painted state did not
    follow it. Those two facts are what the next lane should start from.

- **2026-08-21, COORDINATOR, LIVE PIE ON THE INTEGRATION TREE, NO SOURCE AND NO ASSET CHANGE --
  THE STRIP DRAWS, AND IT DRAWS BLANK AND UNCONDITIONALLY.** The three things the previous entry
  said its evidence did not reach are now measured, and measuring them opened a defect that no
  clause can see. Method: PIE from the game's own startup path (log: `Game class is
  'BP_StratGameMode_C'`, then `Guided opening armed for side 0: objective hex (2, 7), window turns
  1-4`), captured with `Shot showui` -- the only capture route that composites the UMG layer.
  - **What is now observed.** A solid rectangle at x 750-805, y 157-194 in a 2544x1320 frame whose
    viewport centre measures x 778; the box centre is 777.5. So (a) the Slate hierarchy DOES lay
    out at top centre, (b) `AddToViewport` DOES yield a visible widget for this class, and (c) it
    DOES paint on a real frame. Those were the three open unknowns.
  - **Identity was established by REMOVAL, not by geometry.** `ke * RemoveFromParent` through the
    PIE console removed every game widget (22 instances, the live strip's `Root` named in the log)
    and the box went to ZERO pixels in the same frame the scoreboard did -- the scoreboard being
    the positive control that the command took. Three cheaper measurements agreed and none of them
    would have been sufficient: only one live `Border` existed in the world and it was
    `StripBorder`; the box width is exactly the border's own `Left`+`Right` padding (28+28); and
    the composited colour matches `BrushColor` over the sky within 1/255 on all three channels.
    A targeted `ke <InstanceName> RemoveFromParent` reported SUCCESS and changed nothing, because
    it resolved a same-named object under a different outer -- one contradicting result against
    three agreeing ones, and it was the one that was wrong.
  - **NO DIRECTIVE TEXT EVER APPEARED, in any frame.** Six captures across four different model
    states are pixel-identical: the same 2128-pixel rectangle, which is padding alone with
    zero-width content. §2.11.6-B's line has still never been on screen.
  - **The text bindings ARE evaluating.** `GetAll TextBlock Text NAME=DirectiveText` reports the
    LIVE instance still holding the design-time `"Text Block"` while the screen shows nothing: a
    UMG property binding drives the Slate attribute and never writes the UPROPERTY back. Read
    `GetAll` for unbound state only -- for painted state it is the wrong instrument.
  - **THE FINDING. The border paints while the widget's own guidance is all default.** `GetAll
    StratGuidanceWidget Guidance` read the live copy as `bActive=False, Beat=None,
    DirectiveText=""`. `GetStripVisibility` is `ToVisibility(Active)` whose False pin default reads
    `Collapsed` off the graph, so a false `bActive` must COLLAPSE the border. It did not. The
    compiled `Bindings` array on the generated class carries all four entries with the right
    `ObjectName`/`PropertyName`/`FunctionName`, `StripBorder`/`Visibility`/`GetStripVisibility`
    among them. So the strip is drawn whenever the HUD creates it, guidance or no guidance, and
    **no push has been observed to reach the widget at all** -- the paint is not evidence of one.
    - **CORRECTION, same session, raised by the post-merge record gate and adopted before anyone
      relied on it: this names ONE hypothesis where the evidence carries TWO.** What is measured is
      that the border paints while `Guidance.bActive` reads false. "The bound `Visibility` does not
      take" is one explanation. The other is that the binding DOES take and correctly returns
      `Visible` because `GetStripVisibility`'s `Active` input is not fed by the native
      `Guidance` -- the variable-shadowing failure this project has already met once, which
      compiles clean and wires to nothing. The `Bindings` array proves the binding is REGISTERED,
      not what its body reads, and the `Collapsed` false-pin default was read off the graph rather
      than at runtime. `ae4795f` reports the reparent destroying the Blueprint-local `Guidance` and
      re-scoping to `StratGuidanceWidget:Guidance` at the package-byte level, which makes the
      shadowing branch less likely -- less likely is not excluded, and the two are different
      defects in different lanes. Both stand until one is measured.
  - **SECOND DEFECT, measured directly in a SECOND, FRESH PIE session rather than inferred from the
    first: the widget's guidance is still all default two seconds after the opening arms.**
    **[THE "demonstrably non-null" CLAIM BELOW IS SUPERSEDED -- see the newest bullet at the top of
    `## NEXT`. `GuidanceStrip` was measured non-null at t+2.5 s and it IS null at t+0, which is the
    moment that decides; the reading was true of the time it sampled and wrong as an exclusion.]**
    The log line `Guided opening armed for side 0: objective hex (2, 7), window turns 1-4` had
    already been
    written when `GetAll StratGuidanceWidget Guidance` read `bActive=False, Beat=None,
    DirectiveText=""` off the live instance, and the same session's `GetAll StratScoreboardHUD
    GuidanceStrip` shows the HUD holding a pointer to THAT instance -- so the widget the HUD
    created is the widget being read. `ApplyView`'s `HUD->PushGuidance(Model.Guidance)` is guarded
    only by `FindScoreboardHUD()`, and `AStratScoreboardHUD::PushGuidance` is guarded only by
    `GuidanceStrip != nullptr`, which is demonstrably non-null. That is a C++ question, answerable
    in one PIE session with the console route, and it is separate from the visibility-binding
    question above: EITHER defect alone would leave the strip blank, and both are live.
    - **CORRECTION, same session, raised by the post-merge record gate and adopted: the sentence
      that stood here -- "nothing has been pushed into it ... one of those two links does not run"
      -- was a FALSE DICHOTOMY, and the tree's own prose says so.**
      `UStratMatchSubsystem::ApplyView`'s guided-opening block records that `Model.Guidance` is
      written by `FStratGuidedOpening::DecorateViewModel` on the decorated path and is
      DEFAULT-CONSTRUCTED on every other, and that the push is unconditional with no branch on
      `bActive`; `UStratGuidanceWidget::PushGuidance` then assigns with no guard. So an all-default
      read is equally consistent with a push that SUCCEEDED and carried nothing -- an undecorated
      rebuild, or one decorated for a different side. THREE branches, not two: `FindScoreboardHUD`
      returns null, `GuidanceStrip` is null (excluded -- it is measured non-null), or the model
      handed to `ApplyView` was never decorated. The third is the one this pass overlooked and it
      is the cheapest to test. **[SUPERSEDED -- see the newest bullet at the top of `## NEXT`. The
      exclusion of the `GuidanceStrip`-null branch was drawn from a t+2.5 s sample and does not
      hold at t+0, where it is now CONFIRMED; and the third branch is confirmed too, as a mechanism
      that clears the strip on any undecorated reconcile. Two of the three are real.]**
      What is measured is only that the widget's copy is default; nothing here establishes that no
      call was made.
  - **Driven both directions with no simulated input.** The guided opening was skipped on the model
    side and PROVED skipped: a second `ke * SkipGuidance` reported 1 instance succeeded while
    printing no "skipped by the player" line, that log sitting after the `!bActive` early return.
    An explicit `ke * RefreshFromMachine` reconcile also succeeded on the live controller. The
    screen did not move for either. `playtest_key`/`playtest_click` remain unable to drive
    anything; `ke` on a `BlueprintCallable` entry point is a route that does not need them.
  - **Consequence: §2.11.6-B IS NOT CLOSED and must still not be claimed.** What remains is two
    questions in the engineer's and editor's lanes, and NEITHER IS ASKED AS A "WHY", because a
    "why X" presupposes X and X is what is in doubt.
    (1) Does `StripBorder`'s `Visibility` binding fail to take -- or does it take while its `Active`
    input reads something other than the native `Guidance.bActive`? CONDITIONAL, and the condition
    is the thing to measure first: *if* the false pin resolves at runtime to a non-painting
    visibility -- the `Collapsed` the graph shows, or `Hidden` -- then `Active` must be reading
    something TRUE, since the border paints; a DEAD input would default false and the border would
    not be there at all. On that condition "wires to nothing" is not a sufficient mechanism for
    what is on screen, and the shadowing branch above needs a live-but-wrong source rather than an
    unfed pin. The premise is NOT measured: the `Collapsed` default was read off the graph, not at
    runtime, which is exactly the caveat the note above attaches to it, and if the false branch
    resolves to something that paints then a false `Active` explains the screen and this whole
    lead dissolves. So the cheapest measurement in this bullet is `ToVisibility`'s false-branch
    return AT RUNTIME, in the same PIE session -- it may settle question (1) outright without
    touching the shadowing hypothesis at all.
    (2) **[ANSWERED -- see the newest bullet at the top of `## NEXT`. `GuidanceStrip` was null,
    because the HUD's `BeginPlay` had not run yet. Question (1), the binding, is still open.]**
    Where does the guided-opening projection stop -- `FindScoreboardHUD` returning null,
    `GuidanceStrip` null (excluded, measured non-null), or the model reaching `ApplyView`
    UNDECORATED so that an unconditional push delivers a default? Three branches, not the two an
    earlier draft of this bullet named; the third is the cheapest to test and the one this pass
    overlooked.
    Neither is answerable from the record; both are answerable in one PIE session now that the
    console route exists.
    - **CORRECTION, same session, second gate: this sub-bullet kept BOTH retracted framings after
      the notes above were written, and it is the sentence the next lane reads as its task.** It
      asked "why a bound `Visibility` does not take", presupposing the claim demoted 30 lines
      earlier, and framed the push question as "whether it ever reaches this instance", the
      two-branch shape the other note refuted. A correction that does not reach the handoff
      sentence has not been made. Worth knowing for the next sweep: the phrase straddled a line
      break, so a line-oriented `grep "does not take"` did not return it at all -- use `rg -U`.

- **A claim in a document is checkable against an artifact, and the artifact wins — this has now
  cost six corrections across four separate occasions in this milestone alone.** (Corrected count:
  the earlier "four separate corrections" undercounted its own tally by conflating occasions with
  corrections — the fourth occasion below is itself three miscounts across two files plus one
  propagation into this file, so six corrections total, on four occasions.) Phase D's gate-half
  produced three prose errors caught by the reviewer's gate on the first occasion: two technical
  (misreading UE 5.8's `AddExpectedMessagePlain(Occurrences=0)` semantics; overstating what the
  `:610` tripwire's substring match actually pins down), one pure bookkeeping (this file and the
  phase evidence blackboard both claiming "no reviewer verdict exists yet" a hundred-plus lines
  after a `BLOCK` verdict on the same piece was already recorded in the same file) — caught
  respectively by checking the code against the claim and by reading the document's own history. A
  fourth occasion, same class, different artifact: `pie-session/narrative.md` and
  `pie-screenshots/narrative.md` hand-transcribed `STRAT-AI applied` command counts (a turn-3 span
  and count, a turn-5 Build/Move/Attack breakdown, a session-2 Move count sourced from "the brief"
  rather than the log printed three lines above the claim) that a `grep -c` against the checked-in
  slice — the artifact sitting in the same directory — falsifies; propagated once into `state.md`
  before the reviewer's re-gate caught it (2026-08-13, `pie-session`/`pie-screenshots`
  `narrative.md` BLOCK findings) — three miscounts plus the one propagation, six corrections in
  total across the four occasions. Where a narrative states a count a one-line command counts
  exactly, derive it and name the command, rather than reading it off a log by eye or trusting a
  dispatching agent's number over the artifact in hand — this applies to counts, to "has a verdict
  run yet" bookkeeping, and to slice boundaries alike; a document is never its own authority over
  the tree or the log it describes.
- `presentation_statelessness` — Presentation statelessness pass (rebuild widgets from the view model) (actionable)
- `production_widget` — Production menu widget (§2.11.5) (blocked on buildlist_query)
- `bridge_event_list` — Bridge ordered event list (§4.9 'command in / events out') (actionable)
- `buildlist_query` — Buildlist query on the Ui.h contract (actionable, excluded: shape unstated in the GDD by explicit decision, and the file is vendored certified bytes in another repo -- T-INT-01 hash-matches it)

  **[CORRECTED 2026-08-22, coordinator — the exclusion reason above has gone stale by
  UNDER-claiming, which is the direction that sends the next reader to re-open a settled
  question.** "Shape unstated in the GDD by explicit decision" was true when the candidate was
  scored and is FALSE now: the Director **RULED the shape on 2026-08-20** — it is a QUERY, a
  third `ui*` function beside `uiReachable` and `uiForecast`, `uiBuildOptions(w, side,
  factoryHex)` returning `std::vector<UiBuildOption>`. Recorded in `decisions.md` and filed
  upstream in crew `spec/ui_spec.md` at `4d36a16`. **The half of the reason that still holds is
  the second half**, and it is now the whole blocker: the ruling is SPEC-ONLY. Verified against
  both trees on 2026-08-22 — crew HEAD's `cpp_reference/Ui.h` still carries the refusal
  verbatim at its Queries comment, and no `uiBuildOptions` or `UiBuildOption` symbol exists
  anywhere in `cpp_reference/`. So `buildlist_query` is NOT excluded and NOT actionable here; it
  is **blocked on an upstream header change**, after which this side re-vendors, `rulesCommit`
  moves off `cb8e12b`, and `T-INT-01` hash-matches. `production_widget` above inherits that
  blocker unchanged and stays the last feature on §4.5's hard MVP line. The crew gap over the
  vendored commit is **18 commits**, re-derived with `git rev-list --count` rather than
  incremented from the seventeen and eighteen recorded in older banners; none of the 24 vendored
  files moved in that span, so there is no re-vendor pressure yet and `Source/StratRules/` here
  is current. **[SUPERSEDED WITHIN THE HOUR BY THIS SESSION'S OWN WORK, 2026-08-22
  — the gap is NINETEEN, not the eighteen recorded one commit earlier in this same file.**
  The Director's rulings on the three residual questions were committed to the crew repo at
  `6532a46`, which is a nineteenth commit over `cb8e12b`. Re-derived with
  `git rev-list --count cb8e12b..HEAD` after that commit rather than incremented, for the same
  reason the 2026-08-21 banner gives when its own count moved. **The rest of the sentence still
  holds and is the part that matters:** `git diff --name-only cb8e12b..HEAD -- cpp_reference/`
  returns only `test_balance.cpp`, `test_replay.cpp` and `test_save.cpp`, none of which is among
  the 24 vendored files, so no vendored byte has moved, there is still no re-vendor pressure, and
  `Source/StratRules/` here is still current. A gap that grows by spec commits is not a gap that
  owes a re-vendor. **What DID change is the blocker's shape:** with `6532a46` every decision
  blocking `uiBuildOptions` is ruled, so the item is no longer waiting on a Director at all —
  it waits on one header change in `cpp_reference/Ui.h` plus two comment corrections.] **Three residual decisions the ruling deliberately did not settle are already
  filed upstream** — affordable-only vs all four rows with a module-computed `affordable`
  (upstream recommends all four), what factory state does to the answer (Q31), and whether the
  per-type population cap binds the player — and they await a Director ruling, not another
  filing.]
- **CORRECTED 2026-08-24, coordinator -- FOUR OF THE CANDIDATE BACKLOG'S ENTRIES ABOVE RENDER A
  STATUS THAT IS NO LONGER TRUE, AND TWO OF THEM RENDER IT BACKWARDS.** Stamped as a SIBLING after
  the `buildlist_query` block rather than nested inside it, which is the dangling-closer trap
  `content.md` records. Nothing above is deleted and no measurement is withdrawn. **This is a
  record-only pass: no test and no source file was touched, no suite figure moves, and none is
  claimed here** -- the live figure is the one this file's newest banner cites and it is unchanged.
  - **`buildlist_query` IS DISCHARGED, and the entry above says the opposite at length.** That
    block's correction was right when written and its blocker is now gone: the ruling is no longer
    SPEC-ONLY. `uiBuildOptions` and `UiBuildOption` are DEFINED in the vendored
    `Source/StratRules/Ui.h`, `FStratBridge::BuildOptions` routes the answer across the module
    boundary, and `Source/StratBridge/Tests/StratBuildOptionRouting.cpp` pins the transport
    against `strat::uiBuildOptions` over the bridge's own `MakeUiWorld()`. It landed upstream at
    `cae01e3` on 2026-08-22 -- "The buildlist query exists now, and four comments that said it
    could not are wrong".
  - **AND THE RE-VENDOR THAT ENTRY ANTICIPATES HAS ALREADY HAPPENED.** `rulesCommit` has moved off
    `cb8e12b`; `StratRules.manifest.json` records `cae01e3737ebc9e712814865908670cc314b3dfe`.
    **[CORRECTED SAME EVENING -- THE GAP IS ONE AND A RE-VENDOR IS NOW OWED. The claim in
    this bullet was true when written and this pass's own next commit (`96d93ea`, the
    `uiMatchResult` query) falsified it. Two VENDORED files moved in that span -- `Ui.h`
    and `Ui.good.cpp` -- so the "no vendored file moved, therefore no re-vendor pressure"
    reasoning this record has leaned on for every earlier gap DOES NOT APPLY here. The
    correction is stamped at BOTH sites, this one included, because this is the sentence a
    lane reads as its task and a correction that reaches only the evidence has not been
    made. See the banner for the detail.]**
    **THE CREW GAP OVER THE VENDORED COMMIT IS NOW ZERO**, derived with
    `git -C E:/MultiAgent/stratocracy-crew rev-list --count cae01e37..HEAD` rather than
    decremented from the nineteen recorded above, for the same reason every earlier banner gives
    when its own count moved. The vendored tree is not behind by one commit; it is AT upstream
    HEAD, and that crew tree is clean.
    **[RE-STAMPED 2026-08-25, coordinator -- BOTH READINGS ABOVE ARE HISTORY AND THE OWED WORK
    IS DONE.** The re-vendor ran: `rulesCommit` is `96d93ea`, the vendored `Ui.h` and
    `Ui.good.cpp` carry the `uiMatchResult` query, and the gap over the vendored commit is ZERO
    again -- re-derived with `rev-list --count 96d93ea..HEAD`, not decremented from the ONE
    stamped above. **`T-INT-01` PASSES OVER THE NEW BYTES AND IS NOT WHAT DISCHARGES THIS**: it
    would have passed over the stale ones too, for the reason the bracket above gives. What
    discharges it is that the manifest now RECORDS crew HEAD. The banner carries the run.]**
  - **SO `production_widget` IS UNBLOCKED.** Its rendered "(blocked on buildlist_query)" inherits
    a blocker that no longer exists. What it is NOT is finished -- see the entry below on where
    `WBP_ProductionMenu` actually stands against Sec 2.11.5 -- but nothing upstream stands in
    front of it any more.
  - **`presentation_statelessness` RENDERS "(actionable)" AND IS ESSENTIALLY DONE.** Its
    `closes` is `["T-INT-05"]` and `candidates.py`'s `build()` drops any candidate whose every
    `closes` ID is already asserted. **FORTY-THREE T-INT-05 clauses exist and run**, counted by
    set-difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST` walked over `Source/` and NOT by an
    acceptance-ID grep, across `Source/StratUI/Tests/` and `Source/StratPlay/Tests/`. It survives
    the drop only because it has **no probe of its own** -- it borrows `scoreboard_widget`, so the
    scorer can never see it as done. **A GUARD THAT GRADES A NEIGHBOUR'S ARTIFACT INSTEAD OF ITS
    OWN SUBJECT HAS GONE INERT ON THAT SUBJECT**, which is a failure this record has paid for
    before, and it went inert silently.
    - **A FIGURE FROM A LANE REPORT WAS WRONG BY MORE THAN A FACTOR OF TWO, AND RE-DERIVING IS
      WHAT CAUGHT IT.** An exploring agent reported "roughly 18" of these clauses. The macro
      census returns forty-three. The direction does not matter and the lesson is the one already
      on the books: a count in a report is a claim, and the tree is the authority over it.
  - **`bridge_event_list` RENDERS "(actionable)" AND IS NOT ACTIONABLE ON THIS SIDE AT ALL.** Its
    probe is `FStratEvent` and answers NO correctly -- `grep -rn "FStratEvent" Source/`
    returns ZERO. What is wrong is the word "actionable" hung on that NO. **THE RULES MODULE
    DEFINES NO EVENT TYPE**, here or upstream, and Sec 4.9's own Determinism line reads "the
    bridge never reorders, drops, or synthesizes events" -- so the bridge may not manufacture the
    list it is missing. `StratBridge.h` says the same from this side: "The event list is ruled to
    live headless, and no acceptance ID names it as its subject." It is blocked on upstream,
    exactly as `buildlist_query` was, and it should render that way.
  - **AND BENEATH IT SITS AN UNRECORDED CONFLICT THAT IS THE REAL BLOCKER.**
    `UStratMatchSubsystem::ApplyView`'s stated rule is that presentation is RECONCILED, NOT
    EVENTED, and `StratGuidanceWidget.h` and `StratScoreboardHUD.h` refuse queue and latch
    semantics in as many words ("NOT A CATCH-UP QUEUE"). Sec 4.9 specifies an ordered event list,
    which is a second delivery model alongside the reconcile. **NOBODY HAS RULED ON WHICH
    GOVERNS**, and that ruling -- not a header change -- is what `bridge_event_list` is actually
    waiting on. Filed for the Director in `decisions.md`; OWNED: `strat-data-steward` to file,
    then the Director to rule. Nothing is built on it here and nothing should be.
  - **WHY ALL FIVE ARE THE SAME DEFECT.** Every one of these entries renders a status the scorer
    COMPUTES from a probe, and in each case the probe is sound while the classification hung on it
    is stale or borrowed. **A STATUS THAT IS DERIVED IS NOT THEREBY CORRECT** -- it is only as
    correct as the subject the probe was pointed at. Repairing `candidates.py` and `scan.py` is
    OWNED: `strat-data-steward`, and the repair must be shown FALSIFIABLE by breaking the real
    artifact and watching the probe go red, because a probe the scorer cannot parse passes exactly
    like an inert one.
- **Hot-seat milestone is COMPLETE (phase 6 closed 2026-08-13); see "Hot-seat milestone —
  COMPLETE" below.** The AI-opponent milestone is now current (phase C closed 2026-08-13; phase D
  — PIE playtest and a machine-repeatable AI-vs-AI gate — is next; see "## AI-opponent milestone"
  below). Remaining out of scope, unchanged: production menu (§2.11.5), guided opening (§2.11.6),
  info panel, toasts, save-slot UI, move-undo.
- **CORRECTED, 2026-08-14 — this bullet's own claim had itself gone stale and was repeated
  uncritically.** It previously carried the citation "`.agents/ue-project-context.md:195` reads
  '51/51, hot-seat phase 3'", true only at phase 4. The line has been rewritten by later phases since; a
  direct read on 2026-08-14, at the start of the combat-outcome milestone, found it instead reading
  "Last observed here: 86/86, AI-opponent phase B, 2026-08-13" — not the phase-4 text this bullet
  claimed — and that number was itself carried into a dispatch prompt before being checked, the
  same class of error this bullet exists to warn against. The `EnhancedInput` module-arrow gap this
  bullet also raised has not been independently re-checked and is left as an open question for
  `.agents/`'s owner. That file remains outside this steward's lane
  (`.agents/` is not `Config/` or `Tools/architect/`); the count in that file is now current — see the
  new milestone's header above and `.agents/ue-project-context.md`'s own `Last observed here:`
  line, which as of 2026-08-14 read "93/93, combat-outcome phase 2, 2026-08-14" (updated by the
  user, not this steward).
  **[CORRECTED 2026-08-26 — this bullet's repair had itself rotted the same way, and it took a
  widened guard to see it.]** The sentence above said "the line-195 count is now current — see
  `.agents/ue-project-context.md:195` itself". It named a line number in a file that has grown
  since, so the pointer went stale exactly as the citation this bullet was written to retract
  did: measured 2026-08-26, that file is 354 lines and line 195 is the heading "The map →
  GameMode binding is a single unguarded line in `Config/`", while the `Last observed here:`
  line it meant to point at sits at 245 and reads "140/140, 2026-08-21". The pointer is now the
  quoted `Last observed here:` anchor, which does not move. `strat_doc_citation_gate.py`'s
  subject set was widened on 2026-08-26 to cover record documents and the record's own tooling;
  this was one of three live record citations it found on its first run, and the only one of
  the three that was wrong rather than merely quoted.
- **Phase 6 risk, and the most consequential open item: the `STRAT-CMD` click-to-command gate
  is unclosed.** No `playtest_key` or `playtest_click` produced any `LogStratPlay` output.
  **Correction to this record, made in phase 6:** the original claim here — "total input
  silence, reproduced on Epic's own shipped TopDown template with known-good assets" — had a
  confounded control and is **not established**. The "TopDown template" the control actually
  drove was `Lvl_FerrumCrossing` launched under the TopDown template's GameMode (a consequence
  of the stale-config bug found in phase 6 — see below), and `Lvl_FerrumCrossing`'s `Floor`
  actor had been deleted in phase 5 (recorded above, "The template's `Floor` actor was deleted
  from `Lvl_FerrumCrossing`"). A click on that map hits nothing under the cursor, so
  `CharMoveComp.Velocity` legitimately stayed zero for a reason having nothing to do with
  whether input reaches the game. `Escape` still ending PIE is real and unaffected by this
  correction. The underlying question — does simulated input reach `StratPlayerController` at
  all — is **still open**, restated accurately with new, still-confounded-a-different-way
  measurements under Phase 6 below. Phase 4's standing debt — "`ETriggerEvent::Started` on all
  four input actions is asserted, not measured" — remains open, and phase 6's
  `assert_log_contains` gate still depends on solving it. **Do not action the phase-6 escalation
  that asks to repoint `Lvl_FerrumCrossing`'s GameMode or `GlobalDefaultGameMode` again** — see
  Phase 6 below: the running editor was serving a stale, pre-flip config, and the on-disk
  `Config/DefaultEngine.ini` needs no change.
- **Four items carried out of the log-backed combat outcome milestone's phase 5 (now CLOSED,
  milestone COMPLETE) — none owned by this steward, none built in that doc pass by design:**
  1. `Stratocracy.StratPlay.T-INT-05.AlreadyActedGuardFiresOnAForeignModel` — pin that
     `FStratSelectionMachine` refuses an enemy click when the selected unit has
     `bHasActed == true` while the machine's own `DoneUnits` is empty (a machine built against a
     match already in progress). Proposed by `strat-gameplay-engineer`, correctly not written in
     a doc pass. Owner: `strat-test-author`.
     - **CORRECTION, 2026-08-14, this steward's own lane, later pass — DISCHARGED, landed mid-wave
       after this entry was written.** This item was correctly open when written. It is not open
       now: `Source/StratPlay/Tests/StratSelectionForeignModelClauses.cpp` is in the tree
       (untracked at time of writing — `git status --porcelain` lists it `??`), and
       `Stratocracy.StratPlay.T-INT-05.AlreadyActedGuardFiresOnAForeignModel` reads `state:
       "Success"` in `Saved/AutomationReport/index.json` (read `utf-8-sig`, `reportCreatedOn
       2026.08.14-21.47.35`). Suite is now **107/107** (`succeeded 107 / succeededWithWarnings 0 /
       failed 0 / notRun 0`, 107 entries, zero non-Success — re-derived directly, not taken from a
       report or a dispatch number), corroborated independently by
       `grep -rc IMPLEMENT_SIMPLE_AUTOMATION_TEST Source --include=*.cpp` summing to **107** and
       `IMPLEMENT_COMPLEX_AUTOMATION_TEST`/`_CLASS` summing to **0**. The other three items this
       same numbered list names — the `chooseBuild` buildlist-ratio question and the
       content-independence corpus — have NOT moved the same way: `chooseBuild` is now
       investigated and written up at
       `Tools/architect/evidence/upstream-chooseBuild-buildlist-ratio.md`, a request draft
       awaiting a Director ruling, **not discharged**; content-independence remains genuinely open,
       untouched. These three items are three different states, not one word. **[STAMPED 2026-08-19 — BOTH halves of this sentence have since moved, and it is stamped rather than rewritten because it was true when written: `chooseBuild` is RULED (per-type population cap) and FILED upstream at crew `85995b8`; content-independence is DISCHARGED on two of its three axes at `185e88f`, the scenario axis alone still open. See the two NEXT bullets dated 2026-08-19.]**
  2. **DISCHARGED, `a2d370a`, 2026-08-14 — see "Grep-contract case-sensitivity tightening"
     below.** Tighten `Source/StratPlay/Tests/StratHotSeatReplayParity.cpp`'s shape `TestTrue`
     (`Line.StartsWith(TEXT("STRAT-CMD accepted "))`) to `ESearchCase::CaseSensitive`. Residual
     as originally recorded: a case-only change to the `STRAT-CMD accepted` emitter would slip
     past today — confirmed by re-reading the assertion directly; it currently relies on a
     whole-tree case-variant census (also re-confirmed this phase, exactly two lowercase hits,
     both illustrative, in the test files themselves) rather than on its own shape. Deliberately
     not done in phase 5: retightening an assertion is a code change, not a doc change. Owner: a
     code phase. **A new, narrower residual on the `refused` shape replaces this one — see NEXT
     below and the new section.**
  3. A line-number citation living inside a `TestEqual` message string in
     `Source/StratPlay/Tests/StratSelectionWaitClauses.cpp`
     (`StratSelectionMachine.cpp:156-160`). Currently accurate, but unreachable by a doc pass
     because it sits inside an assertion string rather than a comment. Same owner class as item 2.
     - **DISCHARGED 2026-08-19 — and the file held SEVEN citations, not one, one of which had
       already rotted.** The item named the two in-string ones; a grep for
       `StratSelectionMachine.cpp:` in that file returned seven sites (two inside assertion
       message strings, five in comments), all replaced with function-and-branch citations.
       **`:390`, cited twice as `NotifyCommandApplied`'s `DoneUnits.Add`, was already wrong when
       this pass opened it** — that add is at `411`, and `390` now lands on a `break;` in
       `HandleEvent`'s Move arm, a different function. Re-derived, not assumed:
       `grep -n "DoneUnits.Add(Outcome.UnitId)"` → 411; `sed -n '390p'` → `break;`. The other
       five (`:156-160`, `:158`, `:168`) were still accurate and were replaced anyway, which is
       the point — accuracy today is not the property, unreachability by the next diff is.
       Rebuilt (`Result: Succeeded`, one TU recompiled) and re-run headless: suite **107/107**,
       `succeeded 107 / succeededWithWarnings 0 / failed 0 / notRun 0`, 107 entries, zero
       non-Success, `reportCreatedOn 2026.08.19-17.49.48`; both wait clauses `Success`. Count
       unmoved, as expected for a comment-and-message-string change.
     - **NOT closed by this, and measured rather than guessed: four more line-number citations
       survive in two other files**, outside the file this item named. `StratBridge.cpp:448`
       cites `StratSelectionMachine.cpp:571-575` — checked, still accurate (the submit `switch`).
       `StratHotSeatReplayParity.cpp` carries three, and **two of them have already rotted**:
       `:255-287` (cited twice, once INSIDE an assertion message string at its ATTACK clause)
       lands on the selection arm, not the attack branch — the real enemy-click arm runs from
       roughly the `bHasActed` guard to the `Command = Attack` write; and `:350-356`, cited for
       "an attack ends the unit's turn", lands on the Move arm's "already moved this turn"
       refusal -- that one is `NotifyCommandApplied`'s `Attack` arm, the same add the wait file
       cites. A fifth, `StratSelectionMachine.cpp:519`'s `Save.good.cpp:294-300`, cites a
       VENDORED file by line and is a different problem again.
     - **The three `StratHotSeatReplayParity.cpp` citations were then fixed too, on the user's
       instruction, in the same pass** -- both rotted ones and the one in-string site among them,
       all replaced with the branch names above. `grep -rn "StratSelectionMachine.cpp:"
       Source/StratPlay/Tests/` now returns nothing: the Tests lane carries no line-number
       citation into the machine at all. Rebuilt and re-run: `Result: Succeeded`, suite
       **107/107**, `succeeded 107 / succeededWithWarnings 0 / failed 0 / notRun 0`, 107 entries,
       zero non-Success, `reportCreatedOn 2026.08.19-18.05.58`; `T-UI-01.ClickedAttackIsAccepted
       AndRecorded` -- the clause whose assertion message was edited -- `Success`, as are
       `T-SAVE-05.HotSeatReplayParity` and `T-INT-02.ReplayParityWithHeadless`.
     - **A build trap worth the record: the editor blocks the build, and the first build of this
       pass slipped through anyway.** The second build failed `Result: Failed
       (OtherCompilationError)` on `Unable to build while Live Coding is active` with the editor
       running (PID 51424, started 13:54, i.e. ALREADY RUNNING during the first build, which
       succeeded). So "the editor was open" does not predict the failure on its own -- Live
       Coding has to have engaged -- and an agent that treats a green build as proof the editor
       was closed has it backwards. Verification here waited for the user to close it rather than
       committing on the strength of the edit being "the same class" as one already proven, which
       is not a measurement.
     - **The vendored citation is now fixed too, on the user's instruction -- and measuring it
       turned up 48 more.** `StratSelectionMachine.cpp`'s `CommandKindName` doc block carried
       three line citations, all three now by symbol: `Save.good.cpp:294-300` ->
       `strat::saveCommandName`'s switch; `Save.h:135` / `Save.good.cpp:292` -> declared in
       `Save.h`, defined in `Save.good.cpp`; and `.agents/ue-project-context.md:189-191` -> that
       file's read-only-territory heading. The block now says WHY in as many words: vendored
       bytes move on a re-vendor and nothing in this repository would catch a stale number.
       **`.agents/…:189-191` had already rotted, and this steward rotted it** -- the map ->
       GameMode section committed at `0b70a18` was inserted above it, so the citation now lands
       on that new prose rather than the sharper-clause text it names. Same class as the §2
       staleness this steward's own filing caused in the `chooseBuild` document, on the same day.
       Verified: `Result: Succeeded`; suite **107/107**, zero non-Success, `reportCreatedOn
       2026.08.19-18.28.07`; and the diff proven comment-only, not asserted to be -- every added
       and removed line matches a comment-body shape, non-comment changed lines **0**.
     - **THE REAL SIZE OF THIS, measured rather than estimated: 46 line citations into vendored
       headers survive across 16 files** -- 48 before this pass, less the two the block above
       just fixed. **Both numbers are stated because the command below returns the SECOND one,
       and a note whose figure its own command contradicts is the exact defect this file keeps
       paying for.** Caught by running it after writing the number, not before. Kept on ONE
       line, unwrapped, because a wrapped command is a command nobody can run:

       ```
       grep -rEn "(Save|Ai|Combat|Economy|Move|Turn|Ui|Data|Hex|Driver|Rules)\.(h|good\.cpp|buggy\.cpp):[0-9]" Source/StratPlay Source/StratBridge Source/StratUI Source/Stratocracy | wc -l
       ```

       Run over the four non-vendored `Source/` modules. Concentrated in
       `StratBridge.cpp` (11), `StratUI/StratViewModel.h` (8), `StratBridge.h` (7),
       `StratCombatOutcomeParity.cpp` (4). These are the WORST class of the whole family: a
       re-vendor moves them all at once, silently, and no clause in this repository reads a
       vendored line number to notice. Spot-checked while here: `Save.h:54`'s
       `SaveCommandKind` enum is still accurate. The other 47 were NOT checked one by one and
       are not claimed either way. Unowned -- `StratBridge` and `StratUI` are
       `strat-gameplay-engineer`'s lane, not this steward's, and this is a phase-sized sweep
       rather than a fix to slip into another pass.
     - **Still not fixed, and still measured: one citation outside the Tests lane.**
       `StratBridge.cpp:448`'s `StratSelectionMachine.cpp:571-575` -- accurate today (it names
       the submit `switch`); production code, `strat-gameplay-engineer`'s lane.
  4. **DISCHARGED, 2026-08-14 — see "Pre-sliced zero-event guard" below.** `--pre-sliced` gate
     debt on `Tools/architect/strat_combat_pairing_gate.py`, confirmed untouched by phase 5
     (`git diff --stat ae2f22a -- Tools/architect/` empty at the time). It returned `PASS` and
     exit 0 on an empty or wrong pre-sliced corpus. Shipped both halves of the owed shape — a
     structural default refusal on a zero-event `--pre-sliced` corpus, AND a caller-supplied
     `--expect-min-pairs N` floor, the latter opting out of the former when passed explicitly
     (including `N=0`) — plus a 14th self-test case proving the guard can itself fail.
  Also unchanged and still open, carried again: the content-independence half of the phase-D
  "avoid one-corpus proof" precedent (a different scenario/buildlist/first side, genuinely
  different game content) — the host-independence half was discharged in this milestone's
  phase 4. **[STAMPED 2026-08-19 — DISCHARGED on two of three axes at `185e88f` (first side and buildlist moved; the scenario axis is still open because `Data/` is vendored and holds one scenario). True when written; see the NEXT bullet dated 2026-08-19.]**
- **DISCHARGED, this pass, 2026-08-14 — see "STRAT-CMD refused grep-contract residual" below.**
  The residual as originally opened by `a2d370a`: a case-only change to the `STRAT-CMD refused`
  shape was caught by nothing. Cannot be closed with an expected-message gate — verified against
  `UE_5.8\Engine\Source\Runtime\Core\Public\Misc\AutomationTest.h`:
  `FAutomationExpectedMessage::Matches` uses `FString::Contains` at its `IgnoreCase` default;
  `EAutomationExpectedMessageFlags::Exact` adds only a length equality, and a case variant has
  identical length; both constructors build the pattern with `ERegexPatternFlags::CaseInsensitive`
  hardcoded, not parameterized. Closed instead with `IsGrepContractRefusedLine`, a dedicated
  `ESearchCase::CaseSensitive` predicate, and two new clauses — see the new section for the full
  account, including the falsifiability measurement's stated limit (it exercised the predicate,
  not an edited emitter, so the closure is a sound inference rather than a directly observed one).
