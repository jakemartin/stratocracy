# Gate: audio milestone -- FINAL GATE, RE-GATE 3 (title music, options route, volume screen) -- strat-integration-reviewer

## Scope

Working-tree audit of `E:\MultiAgent\Stratocracy`, branch `master`,
`HEAD = 089c79c1dea49ce45eb6cf061645ff37ba9a4b1d`. Nothing committed, nothing staged. 18 tracked
files modified, plus the untracked set. This is the twelfth gate report of 2026-09-05.

The FULL standing twelve-item checklist was re-run over the whole tree, not only the delta.

**The delta since the previous gate was measured, not assumed.**
`find Source Content Config Tools Stratocracy.uproject -type f -newer <the regate-2 report>`,
excluding `Saved/`, `Intermediate/` and `Binaries/`, printed exactly **ONE path**:
`Tools/architect/state/tests.md`. No source file, no asset, no `.Build.cs`, no `.uproject`, no
`Config/`. The previous finding's subject is inside that set, and the compiled subject it talks
about (`StratTitleMusicLifecycle.cpp`) is byte-unmoved -- which is what the repair claimed.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass |
| 2 | No `strat::` call outside the two permitted directories | pass -- zero hits |
| 3 | No vendored header in front of UHT | pass -- zero files matched |
| 4 | `TPimplPtr`, never `TUniquePtr`, for an `FStratBridge` member | pass -- two members, both correct |
| 5 | Module arrows unchanged and correct | pass -- 16 rows, 10 fields, derived |
| 6 | New modules registered | pass -- no new module directory |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` path literal in gameplay C++ | pass -- one standing exception only |
| 9 | No widget-side arithmetic | pass -- strict scan zero |
| 10 | Lane discipline | pass -- one file, its own lane, attributed inline |
| 11 | Prose blocks present, measurements as measurements | pass |
| 12 | Nothing staged or committed | pass -- `HEAD` unmoved, index empty |

Row by row, with the instrument each result rests on.

**1.** `git diff --stat -- Source/StratRules Data` printed nothing;
`git status --porcelain -- Source/StratRules Data` printed nothing. Neither a tracked edit nor an
untracked addition exists under either path.

**2.** A grep over `Source/` for a lowercase-initial `strat::` identifier followed by an open
paren, over `.cpp` and `.h`, filtered of `Source/StratBridge/` and `Source/StratRules/`, returned
ZERO lines (exit 1). No hit to judge this pass.

**3.** Every `.h` under `Source/` carrying a line-anchored `UCLASS(`/`USTRUCT(`/`UENUM(` was
tested for an include of `StratBridge.h`, `Ui.h`, `Data.h`, `Save.h`, `Scenario.h` or `Replay.h`.
ZERO files matched.

**4.** Two declarations: `Source/StratPlay/StratMatchSubsystem.h:2538` and
`Source/StratUI/StratScoreboardHUD.h:1235`, both `TPimplPtr<FStratBridge>`. The only
`TUniquePtr<FStratBridge>` spelling in the tree is prose at `Source/StratUI/StratViewModel.h:47`
recording the measured `C4150`.

**5.** Derived, not trusted; both checklist commands were extracted from the reviewer definition
and executed. **16 rows**, all five modules present so the extraction did not silently fail:
`StratBridge` public `Core,CoreUObject,Engine,Stratocracy` + private `(none)` + private
`StratRules` + `PublicIncludePaths StratBridge,StratRules`; `StratPlay` public
`Core,CoreUObject,Engine,StratUI` + private `StratBridge,UMG,EnhancedInput`; `StratRules` public
`Core`; `StratUI` public `Core,CoreUObject,Engine,StratBridge` + private `UMG,Slate,SlateCore`;
`Stratocracy` public ending in `StratRules`.

The `StratBridge` link row on `StratRules` is the CONDITIONAL one and the guard is intact.
`Source/StratBridge/StratBridge.Build.cs:133` declares
`bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;`, `:134` emits
the `STRAT_VENDORED_RULES_IN_BRIDGE` definition from that same expression, `:135` opens
`if (!bCompileVendoredRulesHere)` and `:137` is the guarded
`PrivateDependencyModuleNames.Add("StratRules");`. Monolithic-only, NOT drift. `StratRules`
appears in no `PublicDependencyModuleNames` of `StratBridge` and in no unconditional `.Add`. The
compile half is bound to the same expression: each `Source/StratBridge/Vendored/*.strat.cpp`
errors out on an undefined `STRAT_VENDORED_RULES_IN_BRIDGE` and compiles its body only under
`#if STRAT_VENDORED_RULES_IN_BRIDGE`, so the dependency half and the compile half cannot drift.

The field census printed **TEN** names -- `BRIDGE=`, `PCHUsage =`, `PrivateDefinitions.Add`,
`PrivateDependencyModuleNames.Add`, `PrivateDependencyModuleNames.AddRange`,
`PublicDependencyModuleNames.AddRange`, `PublicIncludePaths.AddRange`,
`ShadowVariableWarningLevel =`, `bCompileVendoredRulesHere =`, `bUseUnity =` -- matching the
recorded set exactly, `BRIDGE=` being the known string-literal false positive from
`PrivateDefinitions.Add` at `:134`. No eleventh name. `StratUI` has gained no `Stratocracy`
dependency; no cycle. No `.Build.cs` moved this round.

**6.** No new `Source/<Module>/` directory: the five are `StratBridge`, `StratPlay`, `StratRules`,
`StratUI`, `Stratocracy`. `IMPLEMENT_MODULE` / `IMPLEMENT_PRIMARY_GAME_MODULE` appears in
`StratBridge.cpp:23`, `StratPlay.cpp:14`, `StratUI.cpp:10` and `Stratocracy.cpp:6`; the
`.uproject` `Modules` array is `Stratocracy, StratBridge, StratUI, StratPlay`.
`Source/StratRules/` carries no `IMPLEMENT_MODULE` and is correctly ABSENT.

**7.** `git diff --stat -- Source/Stratocracy` and `git status --porcelain -- Source/Stratocracy`
both printed nothing.

**8.** Every non-`Tests/` `.cpp`/`.h` under `Source/` was read with block comments and line-tail
comments stripped in Python, then scanned for `/Game/`. Exactly ONE line survives, the standing
exception: `Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:47` -- the
`FString::Printf` building `"/Game/StratData/%s"` from `Spec.AssetName`. Zero `ConstructorHelpers`
lookups.

**9.** Re-run rather than carried, though no `StratUI` file moved: the strict scan
(`SetText(`/`FText::Format(`/`AsNumber(`/`AsPercent(` on a line also carrying an arithmetic
operator or a ternary, over non-`Tests/` `StratUI`, comments stripped) returned ZERO.

**10.** One file in the delta, `Tools/architect/state/tests.md`, which is `strat-test-author`'s
own record. Its fifth-round addendum at `:291-293` opens with the inline attribution
`strat-test-author (ACTING and WRITING; IN LANE -- THIS FILE ONLY ... base commit 089c79c,
uncommitted)` -- acting and writing named inline, base commit named. No transcription clause and
no in-session clause was invoked, and neither was needed. No production file, no test file and no
asset was touched.

**11.** Every new source file in the untracked set carries a why-first block citing a GDD section
or a local ID -- checked at the head of `StratAudioSettings.h` and `StratOptionsWidget.h` (both
open *"GDD Sec 2.11.5"*), `StratSoundBank.h`, `StratSoundDirector.h`, `StratSoundCues.h`,
`StratAudioVolumeClauses.cpp` and `StratOptionsModelClauses.cpp`. No new `.Build.cs` entry exists
this round. The previous round's failure was accuracy, not absence, and it is closed below.

**12.** `git diff --cached --stat` printed NOTHING; `HEAD` is still
`089c79c1dea49ce45eb6cf061645ff37ba9a4b1d`. This report adds one further untracked path.

## The previous finding, re-derived at its subject

**CLOSED.** The struck sentence is struck, not reworded, and the replacement is true.

`Tools/architect/state/tests.md:224-226` now carries the sentence inside strike markers:

> ~~The file still has exactly ONE `RETRACTED>` block; a second would just be another retraction a
> reader must reach to learn the truth.~~

followed at `:227-250` by a bullet that says in terms that it *"WAS FALSE WHEN WRITTEN, AND IT IS
STRUCK RATHER THAN REWORDED BECAUSE IT WAS QUOTED OUT OF THIS FILE"*. That is the right shape: my
own regate-2 report quotes the sentence verbatim, so a reader arriving by that quotation lands on
the correction.

**Re-derived at the subject rather than accepted.** A grep for the marker token over
`Source/StratPlay/Tests/StratTitleMusicLifecycle.cpp` returns **NINE** lines -- 30, 31, 32, 38,
73, 74, 75, 318, 338. Six form two three-line blocks; three (`:38`, `:318`, `:338`) are prose
references to the token. The two blocks are exactly as the replacement list names them:

- `:30-32` -- *"`SpawnSound2D(..., bAutoDestroy=false)` hands this actor a component the AUDIO
  DEVICE owns rather than this actor's component list: it is not in `OwnedComponents`, so actor
  teardown does not touch it"*
- `:73-75` -- *"**An `AddExpectedMessagePlain` is not a positive control that the line was
  emitted** in this engine, whatever its `Occurrences` argument says ..."*

**And the true claim the struck sentence was reaching for holds.** The entry now says the
fourth-round fix *"added NO NEW BLOCK ... from two to two, not from one to one"*. Confirmed: the
compiled file is byte-unmoved this round, and the header paragraph at `:9-16` is prose rewritten
to be true read in isolation rather than a third retraction block.

## The three unprompted count repairs, each re-measured against the tree

Not accepted; each was re-derived with its own instrument.

**(a) The 97/96 gloss. CORRECT AS CORRECTED.** Enumerating every directory named `Tests` anywhere
under `Source/` gives four: `StratBridge/Tests` **18**, `Stratocracy/StratData/Tests` **1**,
`StratPlay/Tests` **48**, `StratUI/Tests` **30**; total **97**. The one-level glob resolves to
**96**, and 18 + 48 + 30 = 96. The 97th is `Source/Stratocracy/StratData/Tests/`, one level
deeper, exactly as the entry now says. Correcting in place rather than striking is the right call
and the entry states why -- no reader following the old gloss could have audited *fewer* files, so
the figure was never an over-claim.

**(b) The widened "every surviving hit" enumeration. THE SWEEP VERDICT IS UNCHANGED AND CORRECT.**
All seven shapes were reconstructed from the entry's own descriptions and re-run. The engineer's
shape (a), `nothing (else )?(will |would |can )?(free|frees|release|releases|destroy|destroys)`,
returns **ZERO lines, exit 1, over all of `Source/`**. The shape matching an audio device owning
something returns **ZERO lane-wide**. A grep over all of `Source/` for `OwnedComponents`,
`owned by the audio device` and `audio device rather than` returns **exactly three hits** --
`StratShellGameMode.h:61`, `StratTitleMusicLifecycle.cpp:32` and `:315` -- and the first two are
inside retraction blocks while the third is a quotation that names itself as retracted in the same
paragraph. **No live prose copy of either false premise survives anywhere in `Source/`.**

**(c) The `Occurrences 0` claim, bounded rather than left standing. THE BOUND IS CORRECT AND ITS
DIRECTION IS CONSERVATIVE.** The entry now says **28** files under the one-level `Tests` glob pass
`Occurrences 0` where only five were read, and asks to be read as a sample rather than a sweep.
Measured two ways: a grep for the inline `Occurrences` argument comment followed by a zero returns
**28**; a Python parse that balances parentheses and reads the fourth argument of every
`AddExpectedMessage*` call returns **27** files with an effective `0` (109 such calls; three files
pass `1`, one passes `2`). The one-file difference is
`Source/StratPlay/Tests/StratShellHudCallSiteClauses.cpp:333`, which uses `AddExpectedError` with
a zero rather than `AddExpectedMessagePlain` -- the sibling function. Counting it is the right
reading of *"pass `Occurrences 0`"*, since the unstated precondition governs both. All five named
files are inside the 28, so *"five of twenty-eight"* is coherent. The direction matters: the
larger denominator makes the entry's own sample look weaker, not stronger.

**The rest of the fifth round's self-audit checks out.** Macros per file 2 / 1 / 4 / 2 in
`StratShellOptionsRouteClauses.cpp`, `StratTitleMusicLifecycle.cpp`, `StratAudioVolumeClauses.cpp`
and `StratOptionsModelClauses.cpp`. `EStratVolumeDisposition` has exactly FIVE members --
`Applied`, `NoSettings`, `NoBank`, `NoMix`, `NoWorld`. Seven sweep shapes = (a) plus the six
named. `git worktree list` shows this tree plus seven others.

## The "no run is owed" claim, verified against the script

Load-bearing, so checked against `Tools/architect/strat_banner_sweep.py` rather than against a
description of it. `read_macro_census` skips any name that does not end in `.cpp` at `:731`,
**before** any macro test is reached, and updates `newest_mtime` only inside that path at
`:744-745`; `check_report_identity` fires at `:1143` on `newest_test_mtime > report_mtime`. A
`.md` therefore cannot move `newest_test_mtime`, and this round's single-file `.md` delta owes no
build and no suite run. The entry's own statement of this is exact.

## The dispatched figures, re-derived

Not a checklist row, and re-derived anyway because a gate that accepts a figure is not a gate.
`Saved/AutomationReport/index.json`, read with `utf-8-sig`: `succeeded 422, failed 0, notRun 0,
succeededWithWarnings 0`, over **422 entries**, `reportCreatedOn 2026.09.05-16.43.13`. Filtering
the entry list for any state other than `Success` returns an EMPTY list. Unchanged from the
previous gate, as expected of a round that moved one `.md`.

## Findings

None.

## Observations

Non-gating.

- **The five-category enumeration is still one notch narrower than a LOOSE reconstruction of its
  own sweep, and this is recorded as the reason it is not charged.** Re-running the six shapes as
  loosely-joined regexes over `StratTitleMusicLifecycle.cpp` produces hits at `:52` (*"planted a
  handle by hand and asserted only the release"*) and `:104` (*"when no track is configured;
  spawns a non-self-destroying handle"*) which fall into none of the five named categories. Both
  are incidental cross-clause matches of a loose instrument: `:52` narrates a rejected clause
  shape and `:104` describes what the clause asserts, and **neither states the ownership premise
  or the sole-custody premise in any form**. The sweep VERDICT -- no unretracted site -- is
  unaffected, and I cannot show that the lane's own tighter shapes produced these two lines at
  all. Recorded because this enumeration was corrected on exactly this axis in each of the last
  two rounds, and a reader is entitled to know it was re-examined a third time and found to cost
  nothing.
- **The `Occurrences 0` figure of 28 is instrument-dependent and the entry does not say which
  instrument.** A reader re-deriving it over `AddExpectedMessagePlain` alone gets 27. One word --
  naming `AddExpectedError` as included -- would close it. Not charged: the claim's purpose is to
  bound the entry's own sample, and both figures bound it the same way.
- **The previous gate's Observation about `Source/StratPlay/StratSoundDirector.cpp` is unchanged
  and still open** -- a comment says `ApplyVolumes` *"CONTAINS NO `return` AT ALL"* while the
  function contains two, inside the `ApplyChannel` lambda. Neither site moved this round.
- **`StratShellGameMode.h`'s "the self-free that would otherwise happen when the sound finishes"
  is still stated of a track the same file calls looping.** Unmoved; unchanged reasoning for not
  charging it.
- **`Source/StratPlay/StratShellGameMode.cpp` still has no newline at end of file.** Cosmetic,
  unmoved.
- **The `DIAG` values still exist in three places and nothing binds them.** Re-checked; all agree.
  Only the clause produces the value; the others are transcriptions and would go stale silently.
- **Fourteen previously-gated assets under `Content/StratAudio/` remain untracked and
  undiffable.** This becomes checkable at the instant they are committed and not before, and
  saying so is not the same as saying they are clean.
- **The staging observation is unchanged and now covers twelve reports.** No 2026-09-05 report
  under `Tools/architect/gate_reports/` is tracked, while `global.md` cites that directory as the
  holder of the deleted chronology. A commit that omits them leaves that pointer resolving to a
  directory that does not contain what it promises, and it fails silently. Owned by whoever
  stages.
- **On whether another round is worth running -- asked for in the dispatch, answered here so it
  sits on the record rather than only in a reply.** Across the eleven gates of this milestone,
  every finding has been in prose; `Source/`, `Content/` and `Config/` bytes have not been charged
  once. The findings shrank monotonically, and the last two rounds' repairs were **self-audited by
  the lane before this seat saw them** -- the fifth round found three further defects unprompted,
  all of which were re-measured here and all of which are correctly fixed or correctly bounded.
  The two Observations at the head of this list are the residue: both are instrument-choice
  artifacts that mislead nobody, and each would take one word to close. **That is the loop feeding
  on itself, and it is the reason this gate returns PASS rather than a twelfth prose finding.**
  The remaining open items worth a human's attention are the staging of these reports, the
  `ApplyVolumes` comment, and the fact that nothing in this milestone has been HEARD -- a human at
  the keyboard is still the only instrument for any audible claim.
- **Record ownership is clean across all six state files and the audibility discipline did not
  regress.** `global.md`'s banner narrates no gate verdict and claims none.

VERDICT: PASS
