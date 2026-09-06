# Gate: unit damage alert, THIRD PASS — strat-integration-reviewer

Base commit `7e83295`, branch `master`, everything unstaged. Gated 2026-09-06, after the second
gate (`Tools/architect/gate_reports/2026-09-06-unit-damage-alert-regate.md`, `VERDICT: BLOCK`,
two findings) was worked and the tree moved again. Its two predecessors stand byte-identical;
this is a new file, as the standing rule requires. A verdict describes a tree, not a phase.

## Scope

Working-tree diff against `7e83295`. **16 paths**: 12 modified, 4 untracked.

    Source/StratPlay/StratMatchSubsystem.cpp             M
    Source/StratPlay/StratSoundDirector.cpp              M   <- new this round
    Source/StratPlay/StratUnitActor.cpp                  M
    Source/StratPlay/StratUnitActor.h                    M
    Source/StratPlay/Tests/StratRouteTweenUnitDouble.cpp M
    Source/StratPlay/Tests/StratRouteTweenUnitDouble.h   M
    Source/StratUI/StratSoundCues.cpp                    M
    Source/StratUI/StratSoundCues.h                      M
    Source/StratUI/Tests/StratSoundCueClauses.cpp        M
    Tools/architect/state/engine.md                      M
    Tools/architect/state/global.md                      M
    Tools/architect/state/tests.md                       M
    Source/StratPlay/Tests/StratDamageFlashClauses.cpp   ??
    Source/StratUI/Tests/StratDamagedUnitsClauses.cpp    ??
    Tools/architect/gate_reports/2026-09-06-unit-damage-alert-gate.md   ??
    Tools/architect/gate_reports/2026-09-06-unit-damage-alert-regate.md ??

`git status --porcelain` flags every modified entry in the SECOND column and none in the first:
nothing staged, no commit. **What moved in THIS round, measured on mtimes rather than accepted
from the lane:** `StratSoundDirector.cpp` 14:35:01, `global.md` 14:38:33, `engine.md` 14:42:26.
Every other path is 14:19:14 or older, i.e. unchanged since the second gate read it.
`.agents/ue-project-context.md` was read and needed no correction; its `StratBridge` row already
states the monolithic-only link edge and matches the derivation below.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing |
| 2 | No `strat::` call outside the two permitted directories | pass — zero hits, not one candidate to judge |
| 3 | No vendored header in front of UHT | pass — see instrument below; the one grep hit is prose |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass — the only `TUniquePtr<FStratBridge>` in the tree is inside a comment |
| 5 | Module arrows unchanged and correct | pass — 16 rows, 10 field names, derived not quoted |
| 6 | New modules registered (unless no module object) | pass — no new module directory this round or last |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` path literal in gameplay C++ | pass — one hit, `ImportStratDataCommandlet.cpp:54`, a standing exception in an untouched file |
| 9 | No widget-side arithmetic | pass — no widget file changed; incidental census of `StratOptionsWidget.cpp` below |
| 10 | Lane discipline | pass |
| 11 | Prose blocks present | pass |
| 12 | Nothing staged or committed | pass |
| A | Task 1 — both prior findings actually closed | pass |
| B | Task 2 — eighth-site hunt by a method neither lane used | pass — null result, method stated below |
| C | Task 3 — surviving "one subtraction" claim true; the named `+`s are what the comment says | pass on both; wording note in Observations |
| D | Task 4 — discharge attribution separates acting from writing | pass |
| E | Task 5 — prose-only, verified by comment-stripped diff with a control | pass |
| F | Task 6 — pass-specific checks re-run against the current tree | pass |
| G | Task 7 — `global.md` honesty and arithmetic | **BLOCK** — finding 1 |

### Instruments, inlined

**Checklist 1, 7, 12.** `git diff --stat -- Source/StratRules Data`, `git diff --stat --
Source/Stratocracy` and `git diff --cached --stat` each printed nothing.

**Checklist 2.**

    grep -rnE 'strat::[a-z][A-Za-z0-9_]*[[:space:]]*\(' Source/ |
      grep -v '^Source/StratBridge/' | grep -v '^Source/StratRules/'

Zero hits — no candidate to judge either way.

**Checklist 3.** Every header under `Source/` containing `UCLASS`/`USTRUCT`/`UENUM` was tested for
an include of `StratBridge.h`, `Ui.h`, `Data.h`, `Save.h`, `Scenario.h` or `Replay.h`. One file
matched, `Source/StratBridge/StratBridge.h` — and a grep for a real declaration
(a line-anchored `UCLASS`, `USTRUCT` or `UENUM` followed by an open paren) on that file returns
**nothing**: its six hits are prose saying that this header must declare no reflected type.
Judged, not forwarded.

**Checklist 5, module arrows, derived from the five `.Build.cs` files and not quoted.** The
standing derivation was extracted from the reviewer brief and executed, comments stripped, both
call forms matched. **16 rows, all five modules present:**

    StratBridge  PublicDependencyModuleNames    Core,CoreUObject,Engine,Stratocracy
    StratBridge  PrivateDependencyModuleNames   (none)
    StratBridge  PrivateDependencyModuleNames   StratRules
    StratBridge  PublicIncludePaths             StratBridge,StratRules
    StratPlay    PublicDependencyModuleNames    Core,CoreUObject,Engine,StratUI
    StratPlay    PrivateDependencyModuleNames   StratBridge,UMG,EnhancedInput
    StratPlay    PublicIncludePaths             StratPlay
    StratRules   PublicDependencyModuleNames    Core
    StratRules   PrivateDependencyModuleNames   (none)
    StratRules   PublicIncludePaths             StratRules
    StratUI      PublicDependencyModuleNames    Core,CoreUObject,Engine,StratBridge
    StratUI      PrivateDependencyModuleNames   UMG,Slate,SlateCore
    StratUI      PublicIncludePaths             StratUI
    Stratocracy  PublicDependencyModuleNames    Core,CoreUObject,Engine,InputCore,EnhancedInput,
                                                AIModule,NavigationSystem,StateTreeModule,
                                                GameplayStateTreeModule,Niagara,UMG,Slate,Json,StratRules
    Stratocracy  PrivateDependencyModuleNames   (none)
    Stratocracy  PublicIncludePaths             Stratocracy,Stratocracy/Variant_Strategy,...

`StratUI` takes **no `StratPlay` edge and no `Stratocracy` edge; no cycle.** The sixteenth row was
read in source rather than treated as unconditional. `StratBridge.Build.cs`:

    bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
    PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
    if (!bCompileVendoredRulesHere)
    {
        PrivateDependencyModuleNames.Add("StratRules");
    }

Guarded, monolithic-only, from the same one expression that drives the define. NOT drift.

**Field census, ten names and no eleventh:** `BRIDGE=` (the known false positive — the tail of the
string literal inside `PrivateDefinitions.Add`, left unfiltered deliberately), `PCHUsage =`,
`PrivateDefinitions.Add`, `PrivateDependencyModuleNames.Add`,
`PrivateDependencyModuleNames.AddRange`, `PublicDependencyModuleNames.AddRange`,
`PublicIncludePaths.AddRange`, `ShadowVariableWarningLevel =`, `bCompileVendoredRulesHere =`,
`bUseUnity =`.

**Task 1a — finding 1 of the second gate is CLOSED.** `Source/StratPlay/StratSoundDirector.cpp`
now reads, at the false words and not below them:

    // ANYTHING THE RULES MODULE OWNS. `StratSoundCues.cpp` states that it COMPUTES NO
    // MAGNITUDE ABOUT GAME STATE; this is the other side of that boundary, ...
    //
    //   CORRECTED 2026-09-06 AT THE WORDS ABOVE. This sentence cited that file as
    //   stating "that it performs no arithmetic at all" until this date. ...

It names the old paraphrase, the retraction it paraphrased, and where the bounded exemption lives;
the boundary argument is untouched. `engine.md` no longer claims exhaustiveness: the completeness
sentence carries **[CORRECTED 2026-09-06 AT THE WORD "every" IN THE SENTENCE BEFORE THIS STAMP: it
was a COMPLETENESS CLAIM over a census that was not complete, and it was false when written]**, and
the "SIX sites" sentence in that file carries a stamp raising it to seven and naming the method's
limit rather than merely bumping a number.

**Task 1b — finding 2 of the second gate is CLOSED.** `grep -c '444' Tools/architect/state/global.md`
returns **0**, and the stale "nine clauses" returns zero occurrences. The stamp that carried the
rotted restatement now reads *"superseded within the same pass by the live figure at the head of
this banner, which is stated THERE AND ONLY THERE -- an earlier form of this stamp restated it and
went stale within the hour"*, and states no figure of its own.

**Task 2 — the eighth-site hunt, by a third method. NULL RESULT, and here is the instrument so a
reader can tell it from an unattempted one.** The first lane swept the retracted claim's WORDING;
the second swept the SUBJECT (the token `StratSoundCues`). Neither of those is what I ran. I swept
the **PREDICATE** — the thing being asserted — and then intersected it with a subject set widened
to include the prose names the file is called by:

    predicate (case-insensitive):
        arithmetic | no magnitude | magnitude | subtraction | computes no | performs no |
        not one .{0,20} operation | operator
    subject (case-insensitive):
        StratSoundCues | StratDecideSoundCues | StratDecideDamagedUnits | sound ?cue decider |
        cue decider | the decider | StratClampVolume | StratSoundCueClauses |
        StratDamagedUnitsClauses

over every .cpp/.h/.cs/.md/.py/.json under `Source/` and `Tools/architect/`, with each file
**flattened to a single line** (every run of spaces, tabs, CRs and newlines collapsed to one
space) so that a claim wrapped across a comment line break is one string — the exact defect that
hid site seven — and a plus/minus 400-character window around each predicate hit tested for a
subject token. **84 intersection hits.** Under `Source/` they fall in six files and I read every
one:

- `StratSoundDirector.cpp` — the seventh site, now corrected. Judged closed, above.
- `StratSoundCues.cpp` / `.h` — the claim itself and its declaration block. Corrected in round one.
- `StratUnitActor.h` — the cross-file citation corrected in round one.
- `Source/StratPlay/Tests/StratDamageFlashClauses.cpp:1079` — *"THE PLANT IS ONE SUBTRACTION AND IT
  IS A PLANT, NEVER AN EXPECTATION"*. A statement about the clause's own fixture, not about
  `StratSoundCues.cpp`'s arithmetic. Not a site.
- `Source/StratUI/Tests/StratDamagedUnitsClauses.cpp:390` — *"which is what keeps the arithmetic
  out of the expectation side"*. About the clause's own construction. Not a site.

Two files the widened subject set pulled in that **no previous sweep named**, and both are clean:
`Source/StratUI/StratOptionsWidget.cpp:4` (*"there is exactly ONE arithmetic expression below --
the percent"*) with `:39` and `:240`, and `StratOptionsWidget.h:22`, `:222`, `:261` (*"performs no
arithmetic"*, said of the widget CLASS). These are claims about their own file, not about
`StratSoundCues.cpp`, and they are TRUE: the comment-stripped operator census of
`StratOptionsWidget.cpp` returns exactly one line carrying a multiply, an add or a bare minus,

    FString::Printf(TEXT("%d%%"), FMath::RoundToInt(Volume * 100.0f))

inside the free function `StratBuildAudioOptionsModel`, which is where `.cpp:4`, `:240` and the
class-scoped `.h:261` all say it is. Pre-existing, outside this diff, judged and not forwarded.

Under `Tools/architect/` the hits are the two persisted gate reports (dated evidence, standing
unedited by ruling), `engine.md`'s own stamped entries, and `global.md`'s observation-(a)
paragraph — which is finding 1 below, for a different reason.

**NO EIGHTH SITE. Three methods, one new site between them; treat none as exhaustive.**

**Task 3 — the surviving claim and the newly-named operators.** Both hold, measured over a
comment-stripped `StratSoundDirector.cpp` (state machine: line comments, block comments, string
and char literals with escape pairs consumed):

- **One subtraction, and it is the one the comment names.** 19 stripped lines carry the minus
  character; **18 of them are the token for member access through a pointer** (`SoundBank->`,
  `Settings->`, and the lambda trailing return at `:367`). The single subtraction is `:115`,
  `(Now - *LastAt) < MinGap` — a wall clock off `World->GetTimeSeconds()`. The claim is TRUE.
- **The two `+` operators are exactly what the comment now says.** `ApplyVolumes` (`:319-393`):

      const auto ApplyChannel = [&](USoundClass* const Class, const float Volume) -> int32
      { if (Class == nullptr) { return 0; } ... return 1; };

      Record.ChannelsApplied =
            ApplyChannel(SoundBank->MasterSoundClass, Record.MasterVolume)
          + ApplyChannel(SoundBank->SfxSoundClass,    Record.SfxVolume)
          + ApplyChannel(SoundBank->MusicSoundClass,  Record.MusicVolume);

  Three 0-or-1 returns, two `+`s, a count of channels overridden — not a subtraction, and not a
  magnitude about the board. The only other plus characters in the stripped file are the
  pre-increments at `:69` and `:77`, outside `ApplyVolumes` and outside the comment's scope.

**Task 4 — the discharge attribution.** `engine.md` stamps the debt in place (**[DISCHARGED
2026-09-06 AT THE WORDS "NOT build-verified" ABOVE]**) rather than deleting it, and the bullet
below it opens *"THE HALF THAT MUST NOT BE BLURRED IS THAT THIS LANE DID NOT RUN THE BUILD"*: the
`coordinator` (ACTING) confirmed the editor gone and ran the build; `strat-gameplay-engineer`
(WRITING) *"compiled nothing, launched nothing, and is recording a result it was handed rather than
one it took"*. Acting and writing are separated inline, in the entry, where a reader arriving by a
citation lands. The entry does not read as though the lane compiled anything.

**Task 5 — prose-only, verified against `HEAD` and not accepted from the lane.** Comment-stripped
unified diff of `git show HEAD:<path>` against the working-tree file, line numbers dropped:

    Source/StratPlay/StratSoundDirector.cpp   changed stripped lines = 0
    Source/StratUI/StratSoundCues.cpp                              = 29
    Source/StratUI/StratSoundCues.h                                =  3
    Source/StratPlay/StratUnitActor.h                              = 19
    Source/StratPlay/StratUnitActor.cpp                            = 82
    Source/StratPlay/StratMatchSubsystem.cpp                       = 16

`StratSoundDirector.cpp` — the only source file this round touched — is **prose-only, zero
executable lines moved.** The four non-zero figures are the FIRST (feature) round, already gated,
and 29 / 3 / 19 are **bit-for-bit the numbers the second gate measured on the same three files**,
so nothing executable moved in them between that gate and this one either.

**The instrument was proven able to speak on the same run.** A mutant rewriting
`++ApplyViewObservationCount;` as `ApplyViewObservationCount += 1;` in `StratSoundDirector.cpp`
produced `27c27` against the same baseline. An empty diff from this stripper is a measurement, not
a silence.

**Task 6 — pass-specific re-checks, all re-run against the CURRENT tree.**

- `SetActorTickEnabled` across `Source/`: the only `false` argument in the class is
  `StratUnitActor.cpp:1167`, `SetActorTickEnabled(TweenOffsets.Num() > 0 || DamageFlashElapsed >
  0.0f);` inside `UpdateTickEnabled`. The other three call sites (`:509`, `:785`, `:1044`) pass
  `true`. Every remaining hit is prose or a clause assertion message.
- `ApplyUnitView` appears in the diff only inside comment text; its body is untouched and nothing
  arms a flash from it.
- The `ApplyView` flash loop sits inside `if (!bTourExistenceHeld)` and OUTSIDE
  `FindSoundDirector()`'s `if`; the tour flash in `AdvanceAiPlaybackOneStep` is likewise outside it
  and keys on `Current.TargetId` — the defender — guarded on `>= 0`.
- `FinishDamageFlash` retires through `Shake->SetRelativeLocation_Direct(FVector::ZeroVector);`
  then `Shake->UpdateComponentToWorld();`, and ends with `UpdateTickEnabled()` rather than a
  direct switch-off.
- The extended double adds accessors only; a grep for `Tick` across
  `Tests/StratRouteTweenUnitDouble.{h,cpp}` returns prose and no advance seam. **No test-only
  `Tick` advance.**
- `StratUI.Build.cs` names no `StratPlay` and no `Stratocracy` dependency.
- Marker re-parent, checked in code: `Shake->SetupAttachment(Body)` at `StratUnitActor.cpp:67`,
  then `GuidedMarker`, `FlagMarker` and `UnactedPip` all `SetupAttachment(Shake)` at `:92`, `:119`
  and `:128`.

**Task 7 arithmetic.** `Saved/AutomationReport/index.json` read with `utf-8-sig`:
`reportCreatedOn 2026.09.06-18.17.58`, `succeeded 445`, `failed 0`, `notRun 0`,
`succeededWithWarnings 0`, entry array **445** long. Macro census by `grep -c` on the two
automation-test macros: 6 in `StratDamageFlashClauses.cpp`, 3 in `StratDamagedUnitsClauses.cpp`,
11 in `StratSoundCueClauses.cpp`, and **0 in `StratSoundDirector.cpp`** — which is the measured
basis for owing no suite re-run this round. 435 + 10 = 445. Two instruments, one figure.

**Task 7 honesty, item by item against the tree.** Each of `global.md`'s not-verified claims was
checked and each is accurate and unsoftened: the feature is INVISIBLE IN PIE and NO HUMAN HAS SEEN
IT (`DamageFlashMaterial` ships unset); no clause pins amplitude, frequency, X/Y ratio, decay
shape, smoothness or redness, because `Tick` never runs headless; the marker re-parent is
UNMEASURED against `BP_StratUnit` and a green build is explicitly disclaimed as evidence about
reinstancing; the tour path is DEFENDER-ONLY because no `FStratAiPlaybackStep` field reports
counter-damage — confirmed both in the trigger's comment and in the code, which reads
`Current.TargetId`; and the `static_assert` is NOT WRITTEN — a grep for `static_assert` across
`Source/StratUI/` and `Source/StratPlay/` returns exactly one hit, `StratSoundCueClauses.cpp:1115`,
which is the prose recording that it is the stronger form and was not written. Finding 1 is about
a different sentence in the same banner.

## Findings

**1. BLOCK — `global.md` states this pass's gate outcome as an unqualified `VERDICT: PASS` with
zero findings, after two later gates, one of which BLOCKED on this very file.**

`Tools/architect/state/global.md`, in the live damage-alert banner:

    THE GATE RETURNED `VERDICT: PASS` WITH ZERO FINDINGS AND FOUR OBSERVATIONS, AND THREE OF THE
    FOUR WERE ACTED ON RATHER THAN INHERITED.

There is no stamp on that sentence, no statement that it describes a tree that no longer exists,
and **no reference anywhere in the banner to either persisted gate report** — a grep over the
banner's own lines for `gate_report`, `re-gat` and `RE-GAT` returns nothing. The second gate
returned `VERDICT: BLOCK` with two findings, and its finding 2 was against `global.md` itself.
Under `CLAUDE.md` this is **the only file that may state a phase verdict**, so a reader who wants
this pass's gate status has exactly one place to look and it currently answers PASS.

**This file has already ruled the correct shape, and ruled it out of a blocking finding of its
own.** `global.md`, in the 2026-09-04 shipped-difficulty-pin banner:

    **THE FIRST GATE'S `VERDICT: PASS` DESCRIBES A TREE THIS ONE IS NO LONGER**, since a
    verdict describes a tree and not a phase ... It was therefore re-gated. **The gate reports
    are the files matching `Tools/architect/gate_reports/2026-09-04-shipped-difficulty-pin*.md`.**
    ... WHAT THIS BANNER DOES AND DOES NOT RESTATE, SAID EXACTLY, BECAUSE A LOOSER VERSION OF
    THIS SENTENCE WAS ITSELF A FINDING. ... **It NEITHER ENUMERATES NOR COUNTS the re-gates**

The damage-alert banner is the looser version. The fix is the shape already recorded in this same
file: qualify the first gate's PASS as describing a superseded tree, point at the report FILENAME
PATTERN (`Tools/architect/gate_reports/2026-09-06-unit-damage-alert*.md`) so a reader can open
them, and neither count nor enumerate the re-gates — that set is still growing, and this report is
itself the artifact that would falsify any closed statement about it.

**The same paragraph carries a second, smaller falsity of the same origin**, three lines on:

    the correction landed at SIX
    sites, not the three this seat listed, the extra two being a second self-asserted re-check in
    `StratClampVolume`'s banner and a prior `engine.md` entry quoting the claim as true.

It landed at **seven**. `engine.md` stamped exactly this sentence's counterpart in its own file
this round — *"there were SEVEN sites, not six"* — and `global.md` did not, so the two records now
disagree on the count, in the same direction the second gate already flagged them as disagreeing.
(That gate also recorded that this enumeration names five items while calling them six; that half
is unchanged.)

**Owner: the `coordinator`** (writer of `global.md`, jointly with `strat-data-steward`). One stamp
covering both sentences closes it; neither half needs a re-run of anything.

## Observations

Non-gating.

- **The minus-census sentence is TRUE at the token level and FALSE under the naive instrument it
  names, at two sites, and one clause fixes both.** `StratSoundDirector.cpp` ends its correction
  block with *"Measured over the comment-stripped file: `-` appears on one line, this one"*, and
  `engine.md` restates it as *"over the comment-stripped file, `-` appears on exactly one line"*. A
  reader who runs that instrument gets **19 lines**, not one. Eighteen are pointer-member-access
  arrows, so the claim is exactly right about the minus OPERATOR and wrong about the minus
  CHARACTER — and it is the sentence offering itself as the proof of the claim above it. Not a
  finding, because the substantive property is true and I verified it independently (Task 3), and
  because a C++ reader tokenizes the arrow as an arrow rather than a minus. But this record has
  retracted a miscounted control inside a correction block before, at `7e83295`, and the
  disambiguation costs eight words: *"as a subtraction operator; the other 18 are arrows"*. Worth
  taking in whatever pass touches finding 1. Owner if taken: `strat-gameplay-engineer`, for the
  source comment and for its own `engine.md`.
- **Both persisted gate reports must continue to stand unedited**, and this one joins them. Each
  records what a gate read in a tree that existed at the time; the second gate ruled this for the
  first and the ruling extends unchanged. The second report quotes the pre-correction seventh site
  verbatim; that quotation is correct AS HISTORY and must not be updated to match the corrected
  comment.
- **`StratOptionsWidget.cpp` / `.h` carry file-scoped and class-scoped arithmetic claims that no
  previous sweep named.** They are TRUE, measured (Task 2), pre-existing and outside this diff.
  Recorded because the next sweep of this shape will surface them again and someone will otherwise
  spend a round on them.
- **`CancelDamageFlash` remains public with no production caller** — carried unchanged from the
  first gate's observation (d) and from the second gate's, recorded rather than changed. Still
  non-gating: it is the retirement path made reachable from `Tests/` in place of a `Tick` advance
  seam, and that trade was already ruled.
- **The `static_assert` against a `Count` sentinel is still not written and is still open**, routed
  to `strat-gameplay-engineer`. Both `StratSoundCueClauses.cpp:1115` and `global.md` state it as
  the strictly stronger form and state plainly that it was not done. Unchanged this round.
- **The build behind this tree was run by the `coordinator`, not by this reviewer.** I do not run
  builds or suites. `Result: Succeeded`, one TU and zero warnings are read from the output quoted
  in `engine.md`; the 445/445 figure I read myself, out of the exported report, together with the
  macro census that agrees with it.

VERDICT: BLOCK
