# Gate: audio milestone -- FINAL GATE, RE-GATE 2 (title music, options route, volume screen) -- strat-integration-reviewer

## Scope

Working-tree audit of `E:\MultiAgent\Stratocracy`, branch `master`,
`HEAD = 089c79c1dea49ce45eb6cf061645ff37ba9a4b1d`. Nothing committed, nothing staged. 18 tracked
files modified; the untracked set is unchanged from the previous gate except for that gate's own
report. This is the eleventh gate report of 2026-09-05.

The FULL standing twelve-item checklist was re-run over the whole tree, not only the delta.

**The delta since the previous gate was measured, not assumed.** From the repo root,
`find Source Content Config Tools Stratocracy.uproject -type f -newer <the previous gate report>`
printed exactly **four paths**: `Source/StratPlay/Tests/StratTitleMusicLifecycle.cpp`,
`Tools/architect/state/engine.md`, `Tools/architect/state/global.md`,
`Tools/architect/state/tests.md`. No asset, no `.Build.cs`, no `.uproject`, no `Config/`, and no
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
| 9 | No widget-side arithmetic | pass -- strict scan zero; no `StratUI` file moved |
| 10 | Lane discipline | pass -- four files, three lanes, each inside its own |
| 11 | Prose blocks present, measurements as measurements | **BLOCK** -- Finding 1 |
| 12 | Nothing staged or committed | pass -- `HEAD` unmoved, index empty |

Row by row, with the instrument each result rests on.

**1.** `git diff --stat -- Source/StratRules Data` printed nothing and
`git status --porcelain -- Source/StratRules Data` printed nothing. Neither a tracked edit nor an
untracked addition exists under either path.

**2.** A grep over `Source/` for a lowercase-initial `strat::` identifier followed by an open
paren, filtered of `Source/StratBridge/` and `Source/StratRules/`, returned ZERO lines (exit 1).
No hit to judge this pass.

**3.** Every `.h` under `Source/` carrying a line-anchored `UCLASS(`/`USTRUCT(`/`UENUM(` was
tested for an include of `StratBridge.h`, `Ui.h`, `Data.h`, `Save.h`, `Scenario.h` or `Replay.h`.
ZERO files matched. No header moved this round.

**4.** Two declarations exist -- `Source/StratPlay/StratMatchSubsystem.h:2538` and
`Source/StratUI/StratScoreboardHUD.h:1235` -- both `TPimplPtr<FStratBridge>`. The only
`TUniquePtr<FStratBridge>` spelling in the tree is prose at `Source/StratUI/StratViewModel.h:47`
recording the measured `C4150`.

**5.** Derived, not trusted; both checklist commands were extracted from the reviewer definition
and executed. **16 rows**, identical to the recorded block: `StratBridge` public
`Core,CoreUObject,Engine,Stratocracy` + private `(none)` + private `StratRules` +
`PublicIncludePaths StratBridge,StratRules`; `StratPlay` public `Core,CoreUObject,Engine,StratUI`
+ private `StratBridge,UMG,EnhancedInput`; `StratRules` public `Core`; `StratUI` public
`Core,CoreUObject,Engine,StratBridge` + private `UMG,Slate,SlateCore`; `Stratocracy` public
ending in `StratRules`. All five modules appear, so the extraction did not silently fail. The
`StratBridge` link row on `StratRules` is the CONDITIONAL one and the guard is intact:
`Source/StratBridge/StratBridge.Build.cs:133` declares
`bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;`, `:135` opens
`if (!bCompileVendoredRulesHere)`, and `:137` is the guarded
`PrivateDependencyModuleNames.Add("StratRules");`. Monolithic-only, NOT drift. `StratRules`
appears in no `PublicDependencyModuleNames` of `StratBridge` and in no unconditional `.Add`. The
field census printed **TEN** names -- `BRIDGE=`, `PCHUsage =`, `PrivateDefinitions.Add`,
`PrivateDependencyModuleNames.Add`, `PrivateDependencyModuleNames.AddRange`,
`PublicDependencyModuleNames.AddRange`, `PublicIncludePaths.AddRange`,
`ShadowVariableWarningLevel =`, `bCompileVendoredRulesHere =`, `bUseUnity =` -- matching the
recorded set exactly, `BRIDGE=` being the known string-literal false positive from
`PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" ...)` at `:134`. No eleventh name.
`StratUI` has gained no `Stratocracy` dependency; no cycle. No `.Build.cs` moved this round.

**6.** No new `Source/<Module>/` directory. A recursive grep for `IMPLEMENT_MODULE` over `Source/`
returns `StratBridge.cpp`, `StratPlay.cpp`, `StratUI.cpp`, all three named in
`Stratocracy.uproject`'s `Modules` array. `Source/StratRules/` carries no `IMPLEMENT_MODULE` and
is correctly ABSENT from that array.

**7.** `git diff --stat -- Source/Stratocracy` and `git status --porcelain -- Source/Stratocracy`
both printed nothing.

**8.** Every non-`Tests/` `.cpp`/`.h` under `Source/` was read with block comments and `//` tails
stripped in Python, then scanned for `/Game/`. Exactly ONE line survives, the standing exception:
`Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:47`, the `FString::Printf` building
`"/Game/StratData/%s"` from `Spec.AssetName`. Zero `ConstructorHelpers` lookups.

**9.** No `StratUI` file is in this round's delta. Re-run rather than carried: the strict scan
(`SetText(`/`FText::Format(`/`AsNumber(`/`AsPercent(` on a line also carrying an arithmetic
operator or a ternary, over non-`Tests/` `StratUI`, comments stripped) returned ZERO.
`Source/StratUI/StratOptionsWidget.cpp`'s arithmetic line, judged clean two gates ago, is
byte-unmoved.

**10.** Four files, three lanes. `Source/StratPlay/Tests/StratTitleMusicLifecycle.cpp` and
`Tools/architect/state/tests.md` are `strat-test-author`'s, and its addendum entry reads
`strat-test-author (ACTING and WRITING; IN LANE ... base commit 089c79c, uncommitted)`.
`Tools/architect/state/engine.md` is `strat-gameplay-engineer`'s, its correction blocks likewise
`ACTING and WRITING; IN LANE`, base `089c79c`. `Tools/architect/state/global.md` is the
`coordinator`'s, whose writer that file already is. No transcription clause and no in-session
clause was invoked, and neither was needed. No production file was touched by either lane.

**11.** The failure is accuracy, not absence -- Finding 1. Every file in the delta carries its
why-first block; no new `.Build.cs` entry exists this round.

**12.** `git diff --cached --stat` printed NOTHING; `HEAD` is still
`089c79c1dea49ce45eb6cf061645ff37ba9a4b1d`. This report adds one further untracked path.

## The two prior findings, re-derived at their subjects

**Finding 1 of the previous gate is CLOSED, and the reworded paragraph does hold read in
isolation -- tested with the rest of the file covered rather than accepted.**

`Source/StratPlay/Tests/StratTitleMusicLifecycle.cpp:8-16` now opens:

> `SpawnSound2D(..., bAutoDestroy=false)` removes exactly ONE release and only that one: the
> ENGINE'S OWN, by which a finished component destroys itself and nulls the member. THAT IS NOT
> THE SAME CLAIM AS "nothing else could free the component", and this paragraph asserted the
> second one until 2026-09-05 ... a reader who takes nothing else from here should take that the
> actor's own teardown WOULD reach this component.

Read alone, every clause is true against the file's own
`DIAG outer=StratShellGameMode_1 owner=StratShellGameMode_1 registered=1 ownedCount=1`: the
engine's self-free is what `bAutoDestroy=false` removes, and teardown does reach the component.
It states the measured answer BEFORE handing the reader on, so a maintainer who stops at the
paragraph stops holding the true version. **It contradicts nothing in the retraction block below
it** -- both say teardown would reach it, both keep the surviving half (`bAutoDestroy=false` is
what makes the handle a handle), and its closing clause -- that what `EndPlay` owes "is not sole
custody of a component nothing else can touch" -- is the retracted claim disclaimed at the site
that used to make it.

The engineer's shape (a), the regex beginning `nothing (else )?(will |would |can )?`, re-run here
over the whole of `Source/` returns **ZERO lines, exit 1**. Independently, a scan for
`audio device owns`, `not in .OwnedComponents` and `teardown does not touch` over `Source/`,
`Tools/` and `.agents/` returns three relevant hits: the retraction marker line in this file, an
unrelated 2026-08-29 ruling sentence in `StratViewingSideSelectorClauses.cpp`, and the quotation
inside `tests.md`'s own retraction entry. **No live prose copy of the premise survives anywhere.**

**Finding 2 of the previous gate is CLOSED.** `Tools/architect/state/engine.md:62-65` is struck
and the replacement block names both copies:

> (i) The copy it named is a COMPILED ASSERTION MESSAGE ... (ii) The copy it did not name is LIVE
> COMMENT PROSE in that file's own header, eleven lines above the retraction, with no freeze
> defence of any kind

and states in terms that grep (a) *"matches that prose's 'so nothing frees the component' with
BOTH optional groups empty. The sweep saw the site; the sentence collapsed it into the single hit
that had a ready-made defence."* That is the correct diagnosis, verified against the regex itself.
The block also records the tree **as found** -- *"The dispatch said the test lane was fixing that
prose in parallel; it HAS LANDED, and this was measured rather than assumed"* -- which is the
right posture and is confirmed by my own re-run above.

**The corrected block's claim about what now stands in that file is ACCURATE.** It says the
premise survives there *"only as TWO COMPILED ASSERTION STRINGS"*: the `TestFalse` message
*"the component is not self-destroying, so `EndPlay` is the only thing that can release it"*, and
the `TestTrue` message *"contradicts the class header's stated premise and is recorded as a
finding"*. Both located; no third compiled string carries it; the second references the premise
rather than restating it, exactly as the entry says.

**The unprompted second repair is sound.** The 30-code-line enumeration now lists 30 items --
`#pragma once`, three includes, the `.generated.h`, three forward declarations, the `UCLASS` line,
`class STRATPLAY_API`, the opening brace, `GENERATED_BODY()`, `public:`, the constructor, six
`UPROPERTY` pairs, `protected:`, the two `virtual ... override` declarations, the closing brace --
and I re-derived the count by stripping comments and blank lines from
`Source/StratPlay/StratShellGameMode.h` in Python: **exactly 30**. The added bullet attributes the
addition inline with base `089c79c` and names the Observation it answers.

**Its flag about line numbers is correct and is worth acting on.** The entry records that every
number the struck sentence carried had rotted -- the retraction markers and the assertion message
both moved by the OTHER lane's comment-only edit, with no byte of either subject changed.
Measured: the assertion string the first of these gates cited at `:293-294` is now at `:305-306`,
and the ownership retraction markers are at `:30-32`. Cite the words. This report does.

## The rule-instead-of-chronology text in `global.md`, verified against the script

Load-bearing, so checked against `Tools/architect/strat_banner_sweep.py` itself rather than
against any description of it. The replacement text reads:

> `strat_banner_sweep.py`'s REPORT IDENTITY check fires on `newest_test_mtime > report_mtime`,
> comparing against the report FILE'S OWN mtime, and `read_macro_census` populates
> `newest_test_mtime` ONLY from `.cpp` files carrying an `IMPLEMENT_*_AUTOMATION_TEST` macro. So a
> HEADER is dropped by the EXTENSION test before the macro test is ever reached and is exempt even
> if a compiled byte moved; a COMMENT in a `Tests/` file reddens the sweep and forces a re-run.

Against the script:

- `check_report_identity` fires on `if result.newest_test_mtime > result.report_mtime:`. **Exact.**
- `report_mtime` comes from `read_report_timestamps`, which obtains it by
  `os.path.getmtime(report_path)`; its docstring calls it *"The report's OWN write time
  (filesystem mtime)"* and returns `reportCreatedOn` separately, that one driving the distinct
  is-the-run-nameable failure. **Exact -- and this is the half the record got wrong before.**
- `read_macro_census` walks `Source/`, does `if not name.endswith(".cpp"): continue` **before** any
  macro test, and updates `newest_mtime` only inside `if s or c:`, where those two count
  `IMPLEMENT_SIMPLE_AUTOMATION_TEST` and `IMPLEMENT_COMPLEX_AUTOMATION_TEST` with an optional
  `_CLASS` suffix. **Exact.**
- Both named false readings are stated, and the dangerous one -- that the check ignores source
  mtimes entirely -- is correctly identified as the direction that would license trusting a stale
  report after editing a test file. **Correct.**

**The mechanism sentence is right.** The gloss that follows it is one notch loose and is recorded
under Observations rather than charged. The rule as stated is the one that governs, it is the one
this milestone actually paid twice, and it is now stated durably rather than as a chronology.

## The dispatched figures, re-derived

Not a checklist row, and re-derived anyway because a gate that accepts a figure is not a gate.

`Saved/AutomationReport/index.json`, read with `utf-8-sig`: `succeeded 422, failed 0, notRun 0,
succeededWithWarnings 0`, over **422 entries**, `reportCreatedOn 2026.09.05-16.43.13`. Filtering
the entry list for any state other than `Success` returns an EMPTY list -- zero non-Success
entries, as claimed. `python Tools/architect/strat_banner_sweep.py`, unpiped, printed
`SWEEP CLEAN -- no self-contradiction found` at `EXIT=0`, reporting
`IMPLEMENT_SIMPLE_AUTOMATION_TEST=422, COMPLEX/_CLASS=0` and
`report identity: mtime=1788626593, reportCreatedOn='2026.09.05-16.43.13'`.

The banner's arithmetic was re-derived from the tree rather than from the banner, by counting
macros per file: `StratSoundCueClauses.cpp` 10 + `StratSoundDirectorCallSite.cpp` 9 = **+19**;
`StratShippedSoundBankParity.cpp` **+5**; `StratShellOptionsRouteClauses.cpp` 2 +
`StratTitleMusicLifecycle.cpp` 1 + `StratAudioVolumeClauses.cpp` 4 +
`StratOptionsModelClauses.cpp` 2 = **+9**. `389 + 19 + 5 + 9 = 422`. The banner's `24 GATE-AUDIO.`
and `6 GATE-AUDIO-SETTINGS.` name counts were re-counted against the report text: **24 and 6.**
Every dispatched figure checks out.

The test lane's sweep-coverage figure was re-derived rather than accepted: every `.cpp` and `.h`
under `Source/*/Tests/` numbers **97** (`StratBridge/Tests` 18, `StratPlay/Tests` 48,
`StratUI/Tests` 30). The entry lists all seven regex shapes it claims, and the engineer's entry
lists all four of its own. Neither of those enumerations under-counts.

## Findings

**1. `Tools/architect/state/tests.md:224-225` -- a count of the retraction blocks in
`StratTitleMusicLifecycle.cpp` that is wrong by one, in the bullet that justifies the fix's shape,
inside the entry whose own subject is an enumeration that was wrong by one. Owner:
`strat-test-author`.**

The sentence, bold in the source:

> **The file still has exactly ONE `RETRACTED>` block; a second would just be another retraction a
> reader must reach to learn the truth.**

`Source/StratPlay/Tests/StratTitleMusicLifecycle.cpp` carries **TWO**, not one. The marker token
appears on nine lines -- 30, 31, 32, 38, 73, 74, 75, 318 and 338; lines 38, 318 and 338 are
references to the token in running prose, and the remaining six form two distinct blocks on two
unrelated subjects:

- `:30-32` -- the OWNERSHIP premise: *"`SpawnSound2D(..., bAutoDestroy=false)` hands this actor a
  component the AUDIO DEVICE owns rather than this actor's component list ..."*
- `:73-75` -- the EXPECTED-MESSAGE generalisation: *"**An `AddExpectedMessagePlain` is not a
  positive control that the line was emitted** in this engine, whatever its `Occurrences` argument
  says ..."*

**The second block was created by this lane, and this same record file records creating it, four
bullets above the sentence that denies it exists.** `tests.md`'s item (3) in the 2026-09-05 prose
correction says of that generalisation, in this very file: *"Retracted in place, and the mechanism
that reconciles all three is now recorded in both files."* So the record contradicts itself inside
one entry chain, in the countable direction.

**Why this is a Finding and not an Observation, stated rather than left to be inferred.** The
argument the sentence supports is sound -- there is no second retraction *of this premise*, and
adding one would have been worse. But the sentence as written is not that argument. It is a claim
about the file, checkable in one grep, and false. It is the same defect class the bullet three
lines above it charges against this lane's own retraction block -- *"An enumeration of where a
false premise travelled is itself a claim, and a low one hides work from every reader who audits
the spread against the list and stops"* -- restated one scale down, in the repair's own summary,
which is precisely the pattern the previous two gates blocked on. **And it has already travelled:
it reached this gate as a stated fact in the dispatch** (*"The file still carries exactly one
`RETRACTED>` block"*), which is the spread the retraction discipline exists to catch, occurring on
the first hop.

The fix is the sentence and nothing else: say what is true and what was meant -- the file carries
two retraction blocks on two unrelated subjects, and this pass deliberately added no third
retraction of the ownership premise. **No compiled byte need move and none should.**

## Observations

Non-gating.

- **`StratTitleMusicLifecycle.cpp`'s four-destination list is CORRECT AS A LIST OF SITES THAT
  NEEDED FIXING and short by one as a list of sites, and that difference is why it is not a
  Finding.** The block names *"`StratShellGameMode.h`, ... this file's own assertion below, ... a
  dispatch brief, AND ... THE PARAGRAPH DIRECTLY ABOVE THIS ONE"*. A fifth site exists: the
  comment at `:308-312`, which *"previously went on to say the component 'is not one of the
  actor's own components ... actor teardown does not reach it'"*. It is not in the list. It is
  also **disclosed at its own site, in its own parenthesis, naming itself as the same retracted
  claim** -- so a reader auditing the spread against the list arrives there while looking for
  "the assertion below" and is told. The previous charge turned on a site that hid work; this one
  hides none. Recorded because the enumeration was corrected on exactly this axis one round ago
  and a reader is entitled to know it was re-examined on it.
- **`global.md`'s gloss "a COMMENT in a `Tests/` file reddens the sweep" is one notch broader than
  the mechanism stated in the sentence before it.** Nine `.h` files live under `Source/*/Tests/`
  (`StratOptionsWidgetDouble.h`, `StratGuidanceRouteDouble.h`, `StratMatchResultHostDouble.h` and
  six more); a comment edit to any of them reddens nothing, because the extension test drops it.
  Conversely the check has nothing to do with the `Tests/` path -- it is macro-carrying `.cpp`,
  wherever it sits. Measured: every `.cpp` in `Source/` carrying an `IMPLEMENT_*_AUTOMATION_TEST`
  macro today is under a `Tests/` directory, so the gloss is true of the tree as it stands and
  would stop being true the day a macro lands outside one. Not charged: the governing sentence
  immediately above it states the rule exactly right, and the gloss reads as its illustration
  rather than as its definition. Tightening it to "a macro-carrying `.cpp`" would cost one word.
- **`engine.md`'s "both frozen on purpose and both disclaimed in place" is true of both strings,
  but the disclaimer for the first sits BELOW it rather than above.** `tests.md` says the
  sole-custody gloss in that message *"is retracted in the prose above it and in the header"*; the
  prose above it, at `:302-304`, argues only that `bAutoDestroy=false` is load-bearing, and the
  sentence that actually retracts sole custody is below the assertion, in the MEASURED
  CONTRADICTION block -- *"`EndPlay`'s `DestroyComponent()` is belt-and-braces rather than the only
  thing standing between a menu track and a level transition."* The disclaimer exists, at the site,
  and a reader will reach it; only the direction word is wrong. Cosmetic.
- **The `DIAG` values now exist in three places and nothing binds them**, unchanged from the
  previous gate: `StratShellGameMode.h`'s header block, its `EndPlay` declaration, and
  `StratTitleMusicLifecycle.cpp:35` and `:322`. Re-checked character for character; all agree.
  Only the clause produces the value; the others are transcriptions and would go stale silently.
- **`StratShellGameMode.h`'s "the self-free that would otherwise happen when the sound finishes"
  is still stated of a track the same file calls looping**, in its own earlier paragraph. Unmoved
  this round; unchanged reasoning for not charging it.
- **The previous gate's Observation about `Source/StratPlay/StratSoundDirector.cpp` is unchanged
  and still open** -- a comment says `ApplyVolumes` *"CONTAINS NO `return` AT ALL"* while the
  function contains two, inside the `ApplyChannel` lambda. Neither site moved this round.
- **`Source/StratPlay/StratShellGameMode.cpp` still has no newline at end of file.** Cosmetic,
  unmoved.
- **Fourteen previously-gated assets under `Content/StratAudio/` remain untracked and
  undiffable.** This becomes checkable at the instant they are committed and not before, and
  saying so is not the same as saying they are clean.
- **The staging observation is unchanged and now covers eleven reports.** `git ls-files
  Tools/architect/gate_reports` lists none of the 2026-09-05 reports, while `global.md` cites that
  directory as the holder of the deleted chronology. A commit that omits them leaves that pointer
  resolving to a directory that does not contain what it promises, and it fails silently. Owned by
  whoever stages.
- **Record ownership is clean across all six state files and the audibility discipline did not
  regress.** `global.md`'s banner narrates no gate verdict and claims none. Nothing in this pass
  claims the milestone has been heard; a human at the keyboard is still the only instrument for any
  audible claim.

VERDICT: BLOCK
