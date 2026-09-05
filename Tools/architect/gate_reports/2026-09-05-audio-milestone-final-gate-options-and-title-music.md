# Gate: audio milestone -- FINAL GATE (title music, options route, volume screen) -- strat-integration-reviewer

## Scope

Working-tree audit of `E:\MultiAgent\Stratocracy`, branch `master`,
`HEAD = 089c79c1dea49ce45eb6cf061645ff37ba9a4b1d` (`git rev-parse HEAD`). Nothing committed,
nothing staged. 18 tracked files modified; 33 untracked paths in the working set, counted rather
than carried: 17 under `Content/` (16 in `Content/StratAudio/`, plus
`Content/UI/WBP_Options.uasset`), 13 under `Source/`, 8 in `Tools/architect/gate_reports/` and
2 under `Tools/editor/` -- 17 + 13 + 8 + 2 = 40, of which 7 gate reports and 1 editor script
predate this milestone's current round. This report makes the ninth gate report of 2026-09-05.

The FULL standing twelve-item checklist was re-run over the whole tree, not only the delta.

**The delta since the sixth gate was measured, not assumed.** Instrument, from the repo root:

```
find Source Content Config Tools Stratocracy.uproject -type f -newer \
  Tools/architect/gate_reports/2026-09-05-audio-milestone-phases-c-c-prime-d-regate-5.md
```

printed **41 paths**: 18 under `Content/`, 17 under `Source/`, 4 record files under
`Tools/architect/state/`, and `Tools/editor/make_strat_title_music.py`. Unlike every previous
round of this milestone, this delta is mostly CODE AND ASSETS rather than record repair.
`Config/DefaultEngine.ini`, `Stratocracy.uproject` and every `.Build.cs` are ABSENT from it --
nothing in the build graph or the engine config moved this round.

**One item in that list is named before the checklist rather than after it.** All fourteen
assets under `Content/StratAudio/` that earlier phases gated carry new mtimes clustered at
`12:02:17` local, alongside the new `MS_Strat_TitleTheme.uasset` -- the signature of one editor
save-all, not fourteen edits. They are untracked, so no instrument in this tree can diff their
bytes against what the earlier gates read. In Observations; it does not gate.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass |
| 2 | No `strat::` call outside the two permitted directories | pass -- zero hits |
| 3 | No vendored header in front of UHT | pass |
| 4 | `TPimplPtr`, never `TUniquePtr`, for an `FStratBridge` member | pass -- two, both correct |
| 5 | Module arrows unchanged and correct | pass -- 16 rows, 10 fields, derived |
| 6 | New modules registered | pass -- no new module directory |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` path literal in gameplay C++ | pass -- one standing exception only |
| 9 | No widget-side arithmetic | pass -- one candidate, judged; see below |
| 10 | Lane discipline | pass -- four lanes, each inside its own |
| 11 | Prose blocks present, measurements as measurements | **BLOCK** -- Finding 1 |
| 12 | Nothing staged or committed | pass -- `HEAD` unmoved, index empty |

Row by row, with the instrument each result rests on.

**1.** `git diff --stat -- Source/StratRules Data` printed nothing and
`git status --porcelain -- Source/StratRules Data` printed nothing, so neither a tracked edit nor
an untracked addition exists under either path. Both manifests unmodified; the
`rulesCommit`/`dataCommit` gate is untouched.

**2.** `grep -rn "strat::[a-z][A-Za-z0-9_]*[[:space:]]*(" Source/`, filtered of
`Source/StratBridge/` and `Source/StratRules/`, returned ZERO lines. No hit to judge this pass.
The whole audio surface is engine-side: `StratSoundCues.h/.cpp` decides from `FStratViewModel`
and names no `strat` type at all.

**3.** Every `.h` under `Source/` carrying a line-anchored `UCLASS(`/`USTRUCT(`/`UENUM(` was
tested for an include of `StratBridge.h`, `Ui.h`, `Data.h`, `Save.h`, `Scenario.h`, `Replay.h`.
ZERO files matched, including the new reflected headers `StratAudioSettings.h`,
`StratOptionsWidget.h` and the modified `StratShellGameMode.h` / `StratShellSubsystem.h`.

**4.** Two declarations exist -- `Source/StratPlay/StratMatchSubsystem.h:2538` and
`Source/StratUI/StratScoreboardHUD.h:1235` -- both `TPimplPtr<FStratBridge>`. The single
`TUniquePtr<FStratBridge>` spelling is `Source/StratUI/StratViewModel.h:47`, prose recording the
measured `C4150`. No new file declares a bridge member.

**5.** Derived, not trusted; both checklist commands were extracted from the reviewer definition
and executed. **16 rows**, matching the recorded block: `StratBridge` public
`Core,CoreUObject,Engine,Stratocracy` + private `(none)` + private `StratRules` +
`PublicIncludePaths StratBridge,StratRules`; `StratPlay` public `Core,CoreUObject,Engine,StratUI`
+ private `StratBridge,UMG,EnhancedInput`; `StratRules` public `Core`; `StratUI` public
`Core,CoreUObject,Engine,StratBridge` + private `UMG,Slate,SlateCore`; `Stratocracy` public
ending in `StratRules`. All five modules appear. The `StratRules` link row is the CONDITIONAL one
and the guard is intact -- `StratBridge.Build.cs:133` declares `bCompileVendoredRulesHere` from
`Target.LinkType != TargetLinkType.Monolithic`, `:135` is `if (!bCompileVendoredRulesHere)`,
`:137` is the `.Add("StratRules")`. Monolithic-only, NOT drift. `StratRules` appears in no
`PublicDependencyModuleNames` of `StratBridge` and in no unconditional `.Add`. The field census
printed **TEN** names, matching the recorded set exactly, `BRIDGE=` being the known
string-literal false positive from `:134`. No eleventh name. **The whole audio feature added
ZERO dependency edges** -- `StratPlay.Build.cs`'s only change is an 18-line prose block arguing
why `MetasoundEngine` is NOT on the list, and the derived rows are identical to the sixth gate's.
`StratUI` has gained no `Stratocracy` dependency; no cycle.

**6.** No new `Source/<Module>/` directory. All thirteen new `Source/` files land under
`StratPlay` or `StratUI`, both already in the `Modules` array. `Source/StratRules/` carries no
`IMPLEMENT_MODULE` and is correctly ABSENT. `StratPlay.Build.cs:110-111` and
`StratUI.Build.cs:46-47` both carry `PCHUsage = PCHUsageMode.NoPCHs;` and `bUseUnity = false;`.

**7.** `git diff --stat -- Source/Stratocracy` printed nothing and
`git status --porcelain -- Source/Stratocracy` printed nothing.

**8.** Every non-`Tests/` line under `Source/` was scanned with the `//` portion stripped, and
every hit read at its site rather than counted. The only occurrence in a CODE position is
`Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54`, the standing exception. Every
other hit is prose arguing the rule -- including the new properties' own blocks:
`StratSoundBank.h:157` ("CLAUDE.md #4 forbids either arriving as a `/Game/`", of `USoundMix*`
and `USoundClass*`), and `StratShellGameMode.h`'s `SoundBank` and `TitleMusic` blocks, both
`EditDefaultsOnly` `TObjectPtr` with no initializer. `BaseMix`, `MasterSoundClass`,
`SfxSoundClass` and `MusicSoundClass` reach C++ only as `UStratSoundBank` members read through
`SoundBank->` at `StratSoundDirector.cpp:322-359`. Zero `ConstructorHelpers` lookups remain.

**9.** A Python scan of every non-`Tests/` line under `Source/StratUI`, comments stripped,
matching `SetText(`/`FText::Format(`/`AsNumber(`/`AsPercent(` against an arithmetic operator or a
ternary returned ZERO. A second, deliberately looser scan for ANY arithmetic token across the
four new `StratUI` code files returned exactly one code line,
`Source/StratUI/StratOptionsWidget.cpp:31`, judged in its own section below.

**10.** Four lanes, each inside its own. `strat-gameplay-engineer`: `Source/StratPlay/*` and
`Source/StratUI/*` outside `Tests/`, plus `engine.md`. `strat-test-author`: five files under
`Source/*/Tests/` and `tests.md`, nothing else -- no production file carries a test and no test
file carries production code (`StratOptionsWidgetDouble.h` is marked `TEST-ONLY` and lives under
`Source/StratUI/Tests/`). `coordinator`: `Content/`, `Tools/editor/make_strat_title_music.py`,
`content.md`, `global.md`. `strat-data-steward`: `data.md` and `decisions.md`, unmoved this
round. The content entry cites the EDITOR-DRIVER CLAUSE for the acting and `content.md`'s
FALLBACK CONDITION for the writing, separately and inline, names the base commit `089c79c`, and
re-measures the tool absence with a same-lookup control -- both `execute_script` names ABSENT
while `unreal_status` and `list_unreal_projects` came back SERVED. The route, a headless
`-run=pythonscript` commandlet with no editor running, is named, as the 2026-09-04 ruling
requires. No transcription clause was invoked and none was needed: both lane records read
`ACTING and WRITING; IN LANE`.

**11.** Every new file opens with a why-first block citing GDD Sec 2.11.5 and its authorizing ID,
and each of the three new clause files quotes the `global.md` ruling that authorizes its local
`GATE-` name. The failure is not absence but ACCURACY -- Finding 1.

**12.** `git diff --cached --stat` printed NOTHING; every line of `git status --porcelain`
carries a blank first column or is untracked. `HEAD` is still
`089c79c1dea49ce45eb6cf061645ff37ba9a4b1d`. This report adds one further untracked path.

## The five dispatched measurements, re-derived

Not a checklist row, and re-derived anyway because a gate that accepts a figure is not a gate.

`Saved/AutomationReport/index.json` read with `utf-8-sig`: `succeeded 422, failed 0, notRun 0,
succeededWithWarnings 0`, over **422 entries, every one `state == 'Success'`, and zero warnings
summed across all entries**. `reportCreatedOn 2026.09.05-15.59.24`; mtime `1788623964.91`, which
is local `2026-09-05 11:59:24` and UTC `2026-09-05 15:59:24` -- the UTC reading equals the stamp
to the second and the local one does not, which is the non-self-confirming form. `389 + 19 + 5 +
9 = 422` holds. The macro census over `Source/` counts **422**
`IMPLEMENT_SIMPLE_AUTOMATION_TEST`, and the nine new clauses were counted BY MACRO in their own
files: 4 + 2 + 1 + 2 = 9. `python Tools/architect/strat_banner_sweep.py`, unpiped, printed
`SWEEP CLEAN -- no self-contradiction found` at `EXIT=0`, with
`report identity: mtime=1788623965, reportCreatedOn='2026.09.05-15.59.24'`.

**Every figure in the dispatch checked out.** Both findings below are prose inside otherwise
correct work, which is where all nine findings of the previous six rounds also sat.

## The `Volume * 100` question, tested rather than accepted

The dispatch asked whether the test lane's argument survives a reviewer who thinks the rule is
broader than they read it. **The conclusion is right and the argument offered is not what carries
it.**

The single code line is `Source/StratUI/StratOptionsWidget.cpp:31`:

> `FString::Printf(TEXT("%d%%"), FMath::RoundToInt(Volume * 100.0f))`

The test lane's argument is that `T-UI-03` governs the join between a widget and a
`strat::UiSnapshot`, and an options model has no snapshot behind it. That is TRUE of `T-UI-03`
and it is not sufficient, because it would equally license the same multiplication as a node
inside `WBP_Options` -- there is no snapshot either way, so the absence of one cannot be what
makes the C++ site clean and a graph site dirty. **A defence that does not distinguish the two
shapes is not a defence of this shape.**

What actually clears it is the distinction the checklist row is about, and which
`.agents/ue-project-context.md` states in its own words -- every number a widget draws equals
exactly one field:

- The number the widget draws is `FStratAudioOptionsModel::MasterVolumeText`, **one field**, set
  in one place. The widget itself performs no arithmetic: `StratOptionsWidget.cpp` contains no
  other operator, and the three setters only rebuild the model and broadcast.
- The multiplication is in `StratBuildAudioOptionsModel`, a free model-builder function -- the
  same seat as `StratBuildScoreboardModel` and `FStratMatchResultModel::bViewerWon`, both of
  which derive displayed values in C++ and have been gated clean repeatedly on this tree. A
  reading that condemns this line condemns those two.
- The pairing is an invariant rather than a promise: `PushAudioOptions` REBUILDS from the
  argument's floats instead of copying, so no caller can hand the screen a percentage its own
  gain does not have, and `OptionsModelTextRendersItsOwnClampedField` asserts it.

**Not a finding.** Recorded at length only because the argument as offered would have cleared a
real violation on the same grounds, and the next round should not inherit it.

## Judging `WBP_Options`, which the dispatch asked me to disagree with if I could

**It does not gate, and I looked for a way to make it.** No checklist item binds it; the nearest
is item 11, and there the prose is the opposite of a failure. `StratShellSubsystem.h`'s
NOT-IN-THIS-ROUND arm states the cost in terms -- *"clicking the row runs `RequestOptionsPanel`
and nothing appears. That is one step worse than the greyed row the retracted sentence was
arguing against"*. `StratOptionsWidget.h:41-49` names this project's own "a reflected verb with
no caller reads as built" defect against itself. `content.md` records the widget as owed to a
human and says in terms that it does NOT discharge the engineer's standing debt. **Three
independent files say the screen is blank; nothing anywhere says it works.**

**And the alternative was reasoned against a measured defect class rather than waved off.** A
`bOptionsSurfaceAvailable` fact would ship FALSE with nothing able to set it -- a shipped zero
default that makes every clause over the enabled path vacuous, which is a failure this tree has
already measured and named. The row would then be greyed forever with nothing able to un-grey
it. Between a route that opens an empty panel and a route that can never open, the first is the
one a human can see is wrong. **It is a shipping-quality debt and a human's call, not a
constraint violation, and I do not gate on it.**

## Findings

**1. `Source/StratPlay/StratShellGameMode.h:247-250` and `:52-54` -- the header retracts a claim
against a measurement and then asserts that claim's conclusion twice more in the same file, once
at the declaration a reader lands on. Owner: `strat-gameplay-engineer`.**

The retraction itself, `:56-70`, is exemplary in form -- it quotes the old words, gives the
measurement, and says what survives:

> RETRACTED>  "the component is owned by the audio device rather than by this actor's
> RETRACTED>   component list, so `EndPlay` must stop AND destroy it explicitly ... a
> RETRACTED>   non-owned one that nobody destroyed would outlive the map"

> DIAG outer=StratShellGameMode_1 owner=StratShellGameMode_1 registered=1 ownedCount=1

> So actor teardown WOULD reach it, and `EndPlay`'s `Stop()` and `DestroyComponent()` are
> BELT-AND-BRACES -- a stated intent and a guaranteed ordering -- not the only thing standing
> between a menu track and a match map.

**(a) `:247-250`, the doc comment on the `EndPlay` DECLARATION**, states the falsified conclusion
as fact, with no qualifier and no pointer to the retraction ninety lines above:

> A route that travels away from the title map destroys this actor; without this override the
> component -- spawned with `bAutoDestroy` false, so nothing else will free it -- keeps
> playing the title track over the match.

The measurement says actor teardown WOULD reach it. So without the override the component would
not keep playing over the match, and "nothing else will free it" is the retracted premise wearing
a different subject. This site matters more than its distance from the retraction suggests, for
the reason `CLAUDE.md` itself gives about inline attribution: **a reader arriving by a citation
lands on the declaration, not on the file header above it.** A maintainer who jumps to `EndPlay`
reads only the false version, and the file offers them no signal that a second version exists.

**(b) `:52-54`, the paragraph the retraction block is ABOUT**, still carries it too:

> `bAutoDestroy=false` removes the engine's own release, so nothing frees this component on its
> own and `EndPlay` is the one place that stops the track and drops the handle.

`:56` opens *"AND THE REASON THIS PARAGRAPH USED TO GIVE FOR THAT WAS FALSE"* -- so the block
retracts the paragraph's FORMER reason while the paragraph's CURRENT text restates the same
conclusion the measurement contradicts. `:67-70` and `:53-54` cannot both hold.

**What is NOT charged, so the fix is not over-scoped.** `StratShellGameMode.cpp`'s `EndPlay`
comment is CORRECT -- it says *"THIS WHOLE BLOCK IS BELT-AND-BRACES RATHER THAN THE ONLY NET"*
and cites the measurement. `Source/StratPlay/Tests/StratTitleMusicLifecycle.cpp` is the test
lane's and is the best treatment of this measurement in the tree; its one loose string at
`:293-294` is an assertion message the file deliberately froze so that a prose pass changed no
compiled byte, and that reasoning is stated at `:324-328`. It is an Observation below, not part
of this finding. **The code is correct and must not change.** This is two comment sites in one
header.

**And this finding falsifies a sentence in the record that describes it.** `engine.md:7235-7236`
states *"`StratShellGameMode.cpp`'s `EndPlay` comment carried the same premise ... and is
corrected to match. Nothing outside those two files in `Source/` repeats it."* The `.cpp` is
indeed corrected; the `.h` -- one of "those two files" -- is corrected in one place and not in
the other two. That sentence goes with the fix, and it is the engineer's own file.

**2. `Tools/architect/state/global.md:195-197` -- the new banner declares that no chronology of
superseded reports is kept in this file, on the reason that such a list acquires a new way to be
wrong every time a suite runs; the same file still carries one, and it went wrong on exactly the
two runs this pass made. Owner: the `coordinator`.**

The durable statement that replaced the deleted chronology, `:37-42`:

> NO CHRONOLOGY OF SUPERSEDED REPORTS IS KEPT HERE, DELIBERATELY. ... An earlier draft of this
> banner did keep that list, and it had to be corrected on FOUR CONSECUTIVE GATE ROUNDS as it
> grew -- a running history of identities is a claim that acquires a new way to be wrong every
> time a suite runs, and suites run often.

`:195-197`, still standing in the same file, is that list:

> -- [STAMPED 2026-09-05: `03.53.31` has itself since been superseded FOUR times -- by
> `2026.09.05-04.55.48`, `2026.09.05-12.28.26`, `2026.09.05-13.03.00` and
> `2026.09.05-13.12.09`, in that order.

**It is now SIX, and this file supplies the two missing names itself.** `:31-32`: *"THE LIVE
FIGURE'S REPORT IS `reportCreatedOn 2026.09.05-15.59.24`. It supersedes
`2026.09.05-15.47.07`."* Both postdate `13.12.09`. So the enumeration is short by two and the
numeral "FOUR" is false -- falsified by the very pass whose banner declares this class of claim
retired. `grep -n "superseded" Tools/architect/state/global.md` confirms `:195-197` is the only
surviving multi-item report chronology in the live region; every other hit is a single-subject
supersession marker.

**Why this is a finding and not a stamp's ordinary staleness.** A `[STAMPED 2026-09-05]` block is
normally read as true-when-written, and the date discharges it. **Here the date cannot: both new
reports are also 2026-09-05**, so nothing in the stamp separates what it knew from what it did
not. The mitigation the stamp carries -- *"TAKE THE LIVE REPORT FROM THE BANNER'S OWN CITATION AT
THE HEAD OF THIS FILE, never from this sentence"* -- protects a reader looking for the LIVE
report and does nothing for the count, which is the thing that moved. This is the same shape as
the second gate's Finding 1 on this file, which blocked on a supersession claim contradicted
three lines from itself.

**The fix is deletion, not a fifth numeral**, and this file already says so in its own words at
`:206-208`: *"An earlier and much longer form of this stamp ... was DELETED rather than amended
again; `Tools/architect/gate_reports/` holds that history and a stamp is not the place for it."*
The enumeration at `:195-197` is the remainder of exactly that thing, left standing two lines
above the sentence retiring it. Deleting it makes `:37-42` true of the file rather than of the
head banner alone. **This is the repair-is-the-next-finding pattern the dispatch asked me to
hunt, and it is in the repair: the round of deletion removed the inner passage and left the
enumeration that the new durable statement was written to replace.**

## Observations

Non-gating.

- **`Source/StratPlay/StratSoundDirector.cpp:11` and `:181` claim "`ApplyVolumes` CONTAINS NO
  `return` AT ALL", twice and in capitals, and the function contains two.** They are `return 0;`
  at `:345` and `return 1;` at `:353`, inside the `ApplyChannel` lambda -- not returns FROM
  `ApplyVolumes`, so the invariant the claim protects is intact and the code is right. It is
  recorded because of the claim's own stated audience: *"a future reader tidying this function
  will be looking at the code and not at the header."* That reader looks at the code, finds two
  `return`s, and has no way to know which reading was meant. The neighbouring claim about
  `EmitCue` gets the precise form exactly right -- *"CONTAINS NO `return` BEFORE ITS LAST LINE"*
  -- so the wording that would hold is already in the file, eight lines up. Below the bar for a
  finding because no reading of it changes what the code must do; named because "no early return"
  is the single property the whole disposition design rests on.
- **`Tools/architect/state/global.md:40`'s attribution of "FOUR CONSECUTIVE GATE ROUNDS" to "an
  earlier draft of this banner" is arguable, and I did not charge it.** The gate reports place
  those four corrections on the NESTED STAMP at what is now `:190-208`, not on the head banner:
  the head banner's own chronology drew a finding once (`...-regate.md`, Finding 1, at the
  then-`:32`) and was an unfixed Observation from the second gate onward. If "this banner" means
  the file's live banner region broadly, the sentence holds; if it means the 2026-09-05 head
  banner, it does not. Named rather than charged because the ROUND-FOUR finding on this file ran
  on exactly this axis -- attributing one text's wording history to another -- and a reader is
  entitled to know the question was asked and how it was answered.
- **`Source/StratPlay/Tests/StratTitleMusicLifecycle.cpp:293-294`'s assertion message reads "so
  EndPlay is the only thing that can release it".** Compiled-in, and the file explains at
  `:324-328` why compiled strings were deliberately left untouched by the prose pass. The
  surrounding comment at `:290-292` scopes the claim correctly to `bAutoDestroy`. It is the test
  lane's, it is defensible, and it is recorded here so that the engineer's fix to Finding 1 does
  not reach into another lane's file.
- **`Source/StratPlay/StratShellGameMode.cpp` has no newline at end of file** -- `git diff` prints
  `\ No newline at end of file`. Cosmetic; no other `Source/` file in this delta shares it.
- **`Source/StratPlay/StratShellSubsystem.h:89-92` runs a sentence onto the end of a wrapped
  paragraph line** -- *"...stops being worth the risk. A WBP that dropped a row would be
  contradicting that sentence."* -- leaving one line well past the file's own column width.
  Cosmetic; no claim is affected.
- **Fourteen previously-gated assets under `Content/StratAudio/` carry new mtimes and cannot be
  diffed.** They cluster at `12:02:17` local with the new `MS_Strat_TitleTheme.uasset`, which is
  one save-all rather than fourteen edits, and `content.md` records the auto-staging that
  accompanied it. They are UNTRACKED, so this repository holds no baseline to compare against and
  no instrument here can say whether a byte moved. **This becomes checkable at the instant they
  are committed and not before**; until then it is an unmeasurable, not a finding, and saying so
  is not the same as saying they are clean.
- **The sixth gate's staging observation is unchanged and still open, now with one more file.**
  `git ls-files Tools/architect/gate_reports` lists NONE of the eight 2026-09-05 reports, while
  `global.md:207` cites that directory as the holder of the deleted history. A commit that omits
  the reports leaves that pointer resolving to a directory that does not contain what it promises,
  and it fails silently. Owned by whoever stages.
- **`global.md:31-36` names one superseded report five lines above `:37`'s "no chronology is kept
  here".** One named supersession with its reason is not a chronology, and the reason -- an
  mtime-based REPORT IDENTITY check that cannot see a comment-only edit -- is worth keeping.
  Named only because it sits directly above the sentence and a reader may take the two as
  contradictory before reading closely.
- **Record ownership is clean across all six files.** `engine.md` and `tests.md` each carry
  `ACTING and WRITING; IN LANE` entries from their own sole writers; `content.md` cites two
  authorities for two halves, inline; `global.md` and the two steward files are unchanged in
  ownership. `global.md`'s banner narrates no gate verdict and claims none -- its only reference
  to gating is the historical clause at `:40` treated above, which justifies a deletion rather
  than reporting an outcome.
- **The audibility discipline did not regress.** `global.md:110`, `content.md:101` and
  `content.md:169-170` all still say the milestone has not been heard and that a human at the
  keyboard is the only instrument. Nothing in this pass claims otherwise.

VERDICT: BLOCK
