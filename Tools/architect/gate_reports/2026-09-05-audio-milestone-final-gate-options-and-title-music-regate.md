# Gate: audio milestone -- FINAL GATE, RE-GATE (title music, options route, volume screen) -- strat-integration-reviewer

## Scope

Working-tree audit of `E:\MultiAgent\Stratocracy`, branch `master`,
`HEAD = 089c79c1dea49ce45eb6cf061645ff37ba9a4b1d` (`git rev-parse HEAD`). Nothing committed,
nothing staged. 18 tracked files modified; the untracked set is unchanged from the previous gate
except for that gate's own report. This report makes the tenth gate report of 2026-09-05.

The FULL standing twelve-item checklist was re-run over the whole tree, not only the delta.

**The delta since the previous gate was measured, not assumed.** Instrument, from the repo root:

```
find Source Content Config Tools Stratocracy.uproject -type f -newer \
  Tools/architect/gate_reports/2026-09-05-audio-milestone-final-gate-options-and-title-music.md
```

printed exactly **three paths**, with the `gate_reports/` directory itself filtered out:
`Source/StratPlay/StratShellGameMode.h`, `Tools/architect/state/engine.md`, and
`Tools/architect/state/global.md`. No asset, no `.Build.cs`, no `.uproject`, no `Config/`, and no
other `Source/` file moved. Both prior findings' subjects are inside that set.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass |
| 2 | No `strat::` call outside the two permitted directories | pass -- zero hits |
| 3 | No vendored header in front of UHT | pass -- zero files matched |
| 4 | `TPimplPtr`, never `TUniquePtr`, for an `FStratBridge` member | pass -- two, both correct |
| 5 | Module arrows unchanged and correct | pass -- 16 rows, 10 fields, derived |
| 6 | New modules registered | pass -- no new module directory |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` path literal in gameplay C++ | pass -- one standing exception only |
| 9 | No widget-side arithmetic | pass -- one candidate, judged previously, unmoved |
| 10 | Lane discipline | pass -- three files, two lanes, each inside its own |
| 11 | Prose blocks present, measurements as measurements | **BLOCK** -- Findings 1 and 2 |
| 12 | Nothing staged or committed | pass -- `HEAD` unmoved, index empty |

Row by row, with the instrument each result rests on.

**1.** `git diff --stat -- Source/StratRules Data` printed nothing and
`git status --porcelain -- Source/StratRules Data` printed nothing. Neither a tracked edit nor an
untracked addition exists under either path; both manifests are unmodified.

**2.** `grep -rn "strat::[a-z][A-Za-z0-9_]*[[:space:]]*("` over `Source/`, filtered of
`Source/StratBridge/` and `Source/StratRules/`, returned ZERO lines. No hit to judge this pass.

**3.** Every `.h` under `Source/` carrying a line-anchored `UCLASS(`/`USTRUCT(`/`UENUM(` was
tested for an include of `StratBridge.h`, `Ui.h`, `Data.h`, `Save.h`, `Scenario.h` or `Replay.h`.
ZERO files matched. `StratShellGameMode.h` -- the one header that moved -- includes only
`CoreMinimal.h`, `GameFramework/GameModeBase.h`, `UObject/SoftObjectPtr.h` and its own
`.generated.h`, and forward-declares `UAudioComponent`, `USoundBase`, `UStratSoundBank`.

**4.** Two declarations exist -- `Source/StratPlay/StratMatchSubsystem.h:2538` and
`Source/StratUI/StratScoreboardHUD.h:1235` -- both `TPimplPtr<FStratBridge>`. The only
`TUniquePtr<FStratBridge>` spelling in the tree is prose at `Source/StratUI/StratViewModel.h:47`
recording the measured `C4150`. No file that moved this round declares a bridge member.

**5.** Derived, not trusted; both checklist commands were extracted from the reviewer definition
and executed. **16 rows**, identical to the previous gate's derivation and to the recorded block:
`StratBridge` public `Core,CoreUObject,Engine,Stratocracy` + private `(none)` + private
`StratRules` + `PublicIncludePaths StratBridge,StratRules`; `StratPlay` public
`Core,CoreUObject,Engine,StratUI` + private `StratBridge,UMG,EnhancedInput`; `StratRules` public
`Core`; `StratUI` public `Core,CoreUObject,Engine,StratBridge` + private `UMG,Slate,SlateCore`;
`Stratocracy` public ending in `StratRules`. All five modules appear, so the extraction did not
silently fail. The `StratBridge -> StratRules` link row is the CONDITIONAL one and the guard is
intact: `StratBridge.Build.cs` declares
`bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;`, then
`if (!bCompileVendoredRulesHere) { PrivateDependencyModuleNames.Add("StratRules"); }`.
Monolithic-only, NOT drift. `StratRules` appears in no `PublicDependencyModuleNames` of
`StratBridge` and in no unconditional `.Add`. The field census printed **TEN** names, matching the
recorded set exactly, `BRIDGE=` being the known string-literal false positive from
`PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" ...)`. No eleventh name. `StratUI` has
gained no `Stratocracy` dependency; no cycle. No `.Build.cs` is in this round's three-file delta.

**6.** No new `Source/<Module>/` directory; the five directories are unchanged.
`grep -rln IMPLEMENT_MODULE Source/` returns `StratBridge.cpp`, `StratPlay.cpp`, `StratUI.cpp`,
all three named in `Stratocracy.uproject`'s `Modules` array. `Source/StratRules/` carries no
`IMPLEMENT_MODULE` and is correctly ABSENT from that array.

**7.** `git diff --stat -- Source/Stratocracy` printed nothing and
`git status --porcelain -- Source/Stratocracy` printed nothing.

**8.** Every non-`Tests/` `.cpp`/`.h` under `Source/` was read with block comments and `//` tails
stripped in Python, then scanned for `/Game/`. Exactly ONE line survives, and it is the standing
exception: `Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:47`,
`const FString PackageName = FString::Printf(TEXT("/Game/StratData/%s"), Spec.AssetName);`. Zero
`ConstructorHelpers` lookups.

**9.** Unchanged from the previous gate, re-run rather than carried. The strict scan
(`SetText(`/`FText::Format(`/`AsNumber(`/`AsPercent(` on a line also carrying an arithmetic
operator or a ternary, over non-`Tests/` `StratUI`, comments stripped) returned ZERO. The loose
scan over the four new `StratUI` code files returned one arithmetic line,
`Source/StratUI/StratOptionsWidget.cpp:31`, judged clean at length in the previous report and
byte-unmoved this round; its four companions in `StratSoundCues.cpp` are an array bound and
pointer dereferences, not displayed values.

**10.** Three files, two lanes. `Source/StratPlay/StratShellGameMode.h` and
`Tools/architect/state/engine.md` are `strat-gameplay-engineer`'s, and its entry reads
`ACTING and WRITING; IN LANE`, naming base commit `089c79c` and the absence of a worktree lane.
`Tools/architect/state/global.md` is the `coordinator`'s, whose writer that file already is. No
transcription clause and no in-session clause was invoked, and neither was needed. No test file
was written by the engineer and no production file by the test lane.

**11.** The failure is accuracy, not absence -- Findings 1 and 2. Every file in the delta carries
its why-first block; no new `.Build.cs` entry exists this round.

**12.** `git diff --cached --stat` printed NOTHING; `HEAD` is still
`089c79c1dea49ce45eb6cf061645ff37ba9a4b1d`. This report adds one further untracked path.

## The two prior findings, re-derived at their subjects

**Finding 1 of the previous gate is CLOSED at both charged sites, and the reworded declaration
does hold read in isolation -- which is the property the fix was aimed at, so it was tested
rather than accepted.**

`StratShellGameMode.h:52-57`, the paragraph the `RETRACTED>` block is about, no longer states the
falsified conclusion. It now reads that `bAutoDestroy=false` removes *"THE ENGINE'S OWN release --
the self-free that would otherwise happen when the sound finishes"*, and then, in terms:

> THAT IS NOT THE SAME AS SAYING NOTHING ELSE COULD FREE IT, and the paragraph below is the
> measurement of what else does and of exactly how much it changes.

That is the exact inference the measurement forbids, disclaimed at the site that used to make it.

The `EndPlay` declaration comment was **read with the rest of the file covered**, which is the
only way to test the claim it makes about itself. Standing alone it states: the diagnostic
(`outer=StratShellGameMode_1 owner=StratShellGameMode_1 registered=1 ownedCount=1`), the clause
that produced it by name, the conclusion *"so actor teardown WOULD reach it"*, the words
*"This override is BELT-AND-BRACES"*, and a pointer to the header's retraction. A maintainer who
jumps straight to the declaration now reads the true version and is told a fuller one exists.
**It contains no sentence that the header contradicts**, which was the specific hazard: the two
blocks agree on the diagnostic, on the verdict, and on which half of the old argument survived.

**Does the reworded declaration duplicate the diagnostic in a way that can drift?** Yes, it
duplicates it, and no, that is not chargeable. The `DIAG` values now appear three times in the
tree -- the header block, the declaration, and the test file's own measurement site -- identical
character for character, checked. Duplication is the unavoidable price of "true read in
isolation": a declaration that pointed at the header instead would fail the property the finding
asked for. The drift risk is real and belongs in Observations, not in a verdict, because the one
authoritative copy is the assertion in the clause that produces it and the other two cite that
clause by name.

**Finding 2 of the previous gate is CLOSED.** The four-item enumeration formerly at `:195-197` is
gone. What stands in its place claims no list:

> `03.53.31` IS GONE TOO, overwritten by every run since, and NO COUNT OF THOSE RUNS IS KEPT
> HERE. This stamp did keep one and it was stale within a day -- it named four supersessions
> where six had happened

and the stamp's own mitigation was widened to cover exactly the thing that rotted --
*"never from this sentence and never from a number in it"*, where the previous form protected
only the live-report identity. `grep -n superseded` over `global.md` returns no surviving
multi-item report chronology; every other hit is a single-subject supersession marker of the kind
this record has always kept.

**The tightened declaration was checked against the rest of the file rather than read on its
own.** `:37` now says *"NO RUNNING LIST OF SUPERSEDED REPORTS IS KEPT HERE, DELIBERATELY -- the
single predecessor named just above is named because the REASON for that re-run is itself worth
recording, not to start a chronology."* That is consistent with `:31-35`, which names
`2026.09.05-15.47.07` once and gives the reason (the mtime-based REPORT IDENTITY check firing on
a comment-only touch of two test files). One named predecessor with a stated cause is not a
running list, and the wording now says which of the two things is forbidden. It is also
consistent with the nested stamp, which no longer keeps a count and now distinguishes itself from
the head banner's deleted chronology in so many words -- which incidentally answers, rather than
worsens, the previous gate's second Observation about the `FOUR CONSECUTIVE GATE ROUNDS`
attribution.

## The engineer's two construction claims, tested

**"30 code lines, identical in content and order" -- the count and the shape are SOUND; the
"identical to the pre-edit file" half is unfalsifiable from this checkout and is honestly labelled
as construction.**

Measured here by stripping block comments and `//` tails from
`Source/StratPlay/StratShellGameMode.h` in Python and dropping blank lines: **exactly 30 lines**.
They are, in order, `#pragma once`; three includes; the `.generated.h`; three forward
declarations; the `UCLASS(...)` line and the `class STRATPLAY_API ...` line; the opening brace;
`GENERATED_BODY()`; `public:`; the constructor; six `UPROPERTY`/member pairs (`MatchLevel`,
`SaveSlotName`, `SoundBank`, `TitleMusic`, `TitleMusicComponent`, `LastFailureReason`);
`protected:`; the two `virtual ... override` declarations; the closing brace. The record's
enumeration names every item that carries meaning to UHT or the linker and matches what is there.

The second half -- that this set is unchanged from before the edit -- cannot be checked against
`HEAD`, because `HEAD` predates the entire milestone: diffing the comment-stripped
`git show HEAD:Source/StratPlay/StratShellGameMode.h` against the working file shows the three
forward declarations, three property pairs and the `EndPlay` declaration as additions, all of
which landed in EARLIER phases and were read by earlier gates. What CAN be shown is that the
previous gate quoted this round's two charged comment texts at `:52-54` and `:247-250` and both
are now absent, and that the declaration's line span moved without its declaration changing. The
record states the claim as construction rather than measurement, which is the correct label for
it, and it is not charged.

**The corrected REPORT IDENTITY account is RIGHT, verified against the script line by line rather
than against the record's description of it.** In `Tools/architect/strat_banner_sweep.py`:

- `check_report_identity` fires on `if result.newest_test_mtime > result.report_mtime:`.
- `report_mtime` comes from `read_report_timestamps`, whose own docstring calls it *"The report's
  OWN write time (filesystem mtime)"* and which obtains it by `os.path.getmtime(report_path)` --
  NOT `reportCreatedOn`, which that function returns separately and which drives the distinct
  "is the run nameable" failure a few lines above.
- `newest_test_mtime` is populated only by `read_macro_census`, which walks `Source/` and does
  `if not name.endswith(".cpp"): continue` **before** counting macros, and updates `newest_mtime`
  only inside `if s or c:` where those two count `IMPLEMENT_SIMPLE_AUTOMATION_TEST` and
  `IMPLEMENT_COMPLEX_AUTOMATION_TEST(_CLASS)`.

So a header is dropped by the extension test before the macro test is ever reached, exactly as
the record now says, and would be exempt from this check even with a compiled byte moved. Both
false readings -- the struck *"reddens on any source touch"* and the mirror *"no source edit can
invalidate the report"* -- are named in the entry, and the mirror one is the dangerous direction,
correctly identified as the 2026-08-21 case the check's own comment cites. **This account is
load-bearing for how the project decides staleness and it survives verification.**

## The five dispatched measurements, re-derived

Not a checklist row, and re-derived anyway because a gate that accepts a figure is not a gate.

`Saved/AutomationReport/index.json`, read with `utf-8-sig`: `succeeded 422, failed 0, notRun 0,
succeededWithWarnings 0`, over **422 entries**, `reportCreatedOn 2026.09.05-15.59.24`, filesystem
mtime `1788623964.91`. That mtime is UTC `2026-09-05 15:59:24`, equal to the stamp to the second,
while the local reading is `11:59:24` and is not -- the non-self-confirming form, unchanged.
`python Tools/architect/strat_banner_sweep.py`, unpiped, printed `SWEEP CLEAN -- no
self-contradiction found` at `EXIT=0`, reporting the macro census as
`IMPLEMENT_SIMPLE_AUTOMATION_TEST=422, COMPLEX/_CLASS=0` and the report identity as
`mtime=1788623965, reportCreatedOn 2026.09.05-15.59.24`. The arithmetic `389 + 19 + 5 + 9 = 422`
holds. **Every dispatched figure checked out**, and the report identity is unmoved because
nothing that moved this round is a test-defining `.cpp` -- which is the corrected rule above,
applied.

## Findings

**1. `Source/StratPlay/Tests/StratTitleMusicLifecycle.cpp:9-10` -- the retracted premise's FOURTH
site, in live comment prose, eleven lines above that file's own `RETRACTED>` block. Owner:
`strat-test-author`.**

The paragraph reads, unchanged this round:

> `SpawnSound2D(..., bAutoDestroy=false)` removes the engine's own release, so nothing frees the
> component on its own and `EndPlay` is the one place that stops the track and drops the handle.

The file's own measurement contradicts it. Its `RETRACTED>` block concludes: *"The component IS
outered and owned to the spawning actor and IS in its component list, so actor teardown WOULD
reach it."* Something else DOES free it. **This is not a new claim and not a near miss -- it is
the identical sentence charged in the header last round**, which `engine.md` quotes as the
header's offending text: *"nothing frees this component on its own and `EndPlay` is the one place
that stops the track and drops the handle."* The header's copy was reworded; this one was not.

**Why it is charged now and was not last round, stated rather than left to be noticed.** The
previous report said of this file *"its one loose string at `:293-294`"* and treated the compiled
assertion message as the sole survivor. That was my own under-scoping: I read the assertion and
did not read the file header. The `:293-294` string remains an Observation on its stated grounds
-- it is a compiled byte the file deliberately froze so that a prose pass changed nothing UHT or
the linker can see. **`:9-10` has no such defence.** It is a comment; changing it moves no
compiled byte; and it is live prose, not a quotation inside a retraction marker.

**And the file itself enumerates where the premise travelled and omits this paragraph.** Its
retraction block says the false reason travelled *"into `StratShellGameMode.h`, into this file's
own assertion below, and into a dispatch brief"* -- three destinations, none of them the
paragraph eleven lines above that sentence. The fix is the shape the header just took: reword
`:9-10` to say what `bAutoDestroy=false` actually removes and hand the reader to the block below,
leaving the existing `RETRACTED>` block as the file's one retraction. **No compiled byte need
move and none should.**

**2. `Tools/architect/state/engine.md` -- the sweep's coverage claim names one surviving copy
where there are two, and the one it names is not the one that matters. Owner:
`strat-gameplay-engineer`.**

The entry is explicitly framed as *"THE SWEEP, REPORTED BY WHAT IT COVERED AND NOT ONLY BY WHAT
IT FOUND"*, which is the right instinct and is why the sentence that closes it is chargeable:

> Outside it the premise survives only in `Source/StratPlay/Tests/StratTitleMusicLifecycle.cpp`,
> which is the test lane's file, carries its own `RETRACTED>` block at `:15-31`, and whose one
> remaining flat copy is a compiled assertion string the gate explicitly declined to charge.

Two copies remain in that file, not one, and the second -- `:9-10`, Finding 1 -- is live comment
prose rather than a compiled assertion string. **The sweep's own instrument found it.** Grep (a)
as recorded in that same entry is
`nothing (else )?(will |would |can )?(free|frees|release|releases|destroy|destroys)`, and
`:9`'s *"so nothing frees the component"* matches it with both optional groups empty. So this is
not a gap in coverage; it is a gap between what the greps returned and how the result was
summarised -- the sweep saw the site and the sentence collapsed it into the one hit that had a
ready-made defence.

**Why this is charged separately from Finding 1 and not folded into it.** Finding 1 is another
lane's comment. This sentence is a claim about the state of the tree, in the engineer's own
record, made in the entry whose stated purpose is that this premise *"had by then been found in
three separate rounds by three readers, each in a place the previous round did not look."* It is
the fourth round and the fourth place. The struck sentence directly above it -- *"Nothing outside
those two files in `Source/` repeats it"* -- was struck for being true of a file set and false of
a file; its replacement is true of a file and false of a paragraph. **This is the
repair-is-the-next-finding pattern the dispatch asked me to hunt, and it is in the repair's own
coverage report.** The fix is the sentence, not the sweep: name both copies and say which one is
frozen and why.

## Observations

Non-gating.

- **The `DIAG` values now exist in three places and nothing binds them.**
  `StratShellGameMode.h`'s header block, its `EndPlay` declaration, and
  `StratTitleMusicLifecycle.cpp`'s measurement site all carry
  `outer=StratShellGameMode_1 owner=StratShellGameMode_1 registered=1 ownedCount=1`. They agree
  today, checked character for character. Only the third is produced by the clause that measures
  it; the other two are transcriptions, and if the engine's ownership behaviour ever changes, two
  of the three go stale silently while the clause reddens. Recorded because the whole point of
  the accepted fix was to duplicate the diagnostic, so the cost of that choice belongs on the
  record beside it. Below a finding because both transcriptions cite the clause by name, which is
  the pointer a reader needs.
- **`StratShellGameMode.h:53`'s "the self-free that would otherwise happen when the sound
  finishes" is stated of a track the same file calls looping**, at `:32` -- *"THE TITLE MAP NOW
  OWNS A LOOPING SOUND"*. A looping cue does not finish, so with the engine default the self-free
  would arrive on the stop rather than at the end of the track, and the surviving half of the old
  argument is slightly weaker in this specific case than the sentence reads. Not charged: the
  mechanism named is correct, the conclusion (`bAutoDestroy=false` is what makes the handle a
  handle) is correct, and no reading of it changes what the code must do. Named because it is the
  one clause of the reworded paragraph a determined reader could push on.
- **`engine.md`'s enumeration of the 30 code lines names 24 of them.** It lists `#pragma once`,
  three includes, the `.generated.h`, three forward declarations, the `UCLASS`/`GENERATED_BODY`,
  six `UPROPERTY` pairs and the two `virtual ... override` declarations; the class line, the two
  braces, `public:`, the constructor and `protected:` are in the file and not in the list. The
  count is right and the omitted six carry no meaning to UHT or the linker. Cosmetic.
- **`global.md`'s replacement stamp still contains one number a suite run can rot** -- *"it named
  four supersessions where six had happened."* Six is correct as of
  `reportCreatedOn 2026.09.05-15.59.24`, verified against the head banner's own two named reports.
  It is a retrospective statement about a deleted list rather than a live claim, and the stamp now
  disclaims all its own numerals (*"never from a number in it"*), which is why it is here and not
  in Findings. A reader is entitled to know the sentence was examined on exactly the axis the
  previous finding ran on.
- **The previous gate's Observation about `Source/StratPlay/StratSoundDirector.cpp:11` and `:181`
  is unchanged and still open** -- *"`ApplyVolumes` CONTAINS NO `return` AT ALL"* while the
  function contains two, inside the `ApplyChannel` lambda. Neither site moved this round.
- **`Source/StratPlay/StratShellGameMode.cpp` still has no newline at end of file**, and
  `Source/StratPlay/StratShellSubsystem.h:89-92` still runs one line past the file's own column
  width. Both cosmetic, both unmoved.
- **Fourteen previously-gated assets under `Content/StratAudio/` remain untracked and
  undiffable.** This becomes checkable at the instant they are committed and not before, and
  saying so is not the same as saying they are clean.
- **The staging observation is unchanged and now covers ten reports.**
  `git ls-files Tools/architect/gate_reports` lists none of the 2026-09-05 reports, while
  `global.md` cites that directory as the holder of the deleted chronology. A commit that omits
  them leaves that pointer resolving to a directory that does not contain what it promises, and
  it fails silently. Owned by whoever stages.
- **Record ownership is clean across all six state files, and the audibility discipline did not
  regress.** `global.md`'s banner narrates no gate verdict and claims none. Nothing in this pass
  claims the milestone has been heard; a human at the keyboard is still the only instrument for
  any audible claim.

VERDICT: BLOCK
