# Gate: content.md stamp density, RE-GATE round 2 (post-`c69e519`) — strat-integration-reviewer

## Scope

Uncommitted working tree at `E:\MultiAgent\Stratocracy`, branch `master`, head
`c69e519bcf0bd22c394e51cc8e44006ccb27d9f2`. Measured, not taken from the brief:

    $ git status --porcelain
     M Source/StratUI/Tests/StratOptionsModelClauses.cpp
     M Tools/architect/state/content.md
     M Tools/architect/state/global.md
     M Tools/architect/state/tests.md
    ?? Tools/architect/gate_reports/2026-09-05-content-md-stamp-density.md
    $ git diff --cached --stat
    (empty)
    $ git diff --stat
     Source/StratUI/Tests/StratOptionsModelClauses.cpp | 47 ++++++++++++++++
     Tools/architect/state/content.md                  | 43 ++++++++++---
     Tools/architect/state/global.md                   | 10 +++-
     Tools/architect/state/tests.md                    | 66 +++++++++++++++++++++++
     4 files changed, 158 insertions(+), 8 deletions(-)

FOUR modified files, nothing staged, nothing committed. The untracked entry is this reviewer's
own round-1 report, which is expected and is not a finding. The audit is the working-tree diff,
the four answers to the prior round's findings re-derived rather than accepted, and an
independent sweep of the record, `Source/`, `Tools/editor/` and `.agents/`.

This report supersedes nothing. `Tools/architect/gate_reports/2026-09-05-content-md-stamp-density.md`
stands as the record of round 1 and is not overwritten.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass |
| 2 | No `strat::` call outside `StratBridge/` or `StratRules/` | pass |
| 3 | No vendored header in front of UHT | pass |
| 4 | `TPimplPtr`, not `TUniquePtr`, for any `FStratBridge` member | pass |
| 5 | Module arrows unchanged and correct | pass — derived, 16 rows / 10 fields |
| 6 | New modules registered — unless they have no module object | pass |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` path literal in C++ | pass |
| 9 | No widget-side arithmetic | pass, scoped: the only C++ in the diff is a comment block; `StratUI` was not re-derived whole |
| 10 | Lane discipline | pass |
| 11 | Prose blocks present / record accurate | **BLOCK** — Finding 1 |
| 12 | Nothing staged or committed | pass |

### 1. Vendored bytes untouched — pass, with a control

    $ git diff --stat -- Source/StratRules Data
    (nothing)
    CONTROL, same instrument on a path that DID move:
    $ git diff --stat -- Source/StratUI
     Source/StratUI/Tests/StratOptionsModelClauses.cpp | 47 +++++++++++++++++++++

The empty result is a measurement, not a silence. The manifests were read and are the authority
they claim to be: `rulesCommit 96d93ea9e490c8cc33261f5d9671389a9c76ecfd`,
`dataCommit c2f58608c77c60c44e6c0fc87988bd3b372beaf5`. Neither moved.

### 2. `strat::` calls — pass, with a control

    $ grep -rn -E "strat::[a-z][A-Za-z0-9_]*[[:space:]]*\(" Source/ --include=*.cpp --include=*.h \
        | grep -v "^Source/StratBridge/" | grep -v "^Source/StratRules/"
    (nothing)
    CONTROL, the same pattern inside the permitted directory:
    Source/StratBridge/StratBridge.cpp:168:  strat::parseScenario(ToStd(Text), ...)
    Source/StratBridge/StratBridge.cpp:175:  strat::validateScenario(Parsed, Units, Terrain);
    Source/StratBridge/StratBridge.cpp:182:  if (!strat::seedFromScenario(GameState, Parsed, ...))

Zero hits outside the two directories; the pattern is shown able to match. The lowercase-initial
form is what separates a call from a type, and no type name was reported as a violation.

### 3. Vendored header in front of UHT — pass, with a control

    $ grep -rn -E '^[[:space:]]*#include[[:space:]]*"(StratBridge|Ui|Data|Save|Scenario|Replay)\.h"' \
        Source/ --include=*.h
    -> 8 distinct headers: Source/StratBridge/StratBridge.h and
       Source/StratRules/{Ai,Driver,Economy,Move,Replay,Scenario,Ui}.h

None declares a reflected type. CONTROL: 59 headers under `Source/` match
`^[[:space:]]*(UCLASS|USTRUCT|UENUM)`, so the reflected-type half of the test can speak.
A prose-matching grep on the bare string is deliberately NOT used here; it reported fourteen
false violations in round 1 by matching the string inside comments.

### 4. `TPimplPtr` — pass, with a control

    $ grep -rn "TUniquePtr<FStratBridge>" Source/
    Source/StratUI/StratViewModel.h:47: // holds a bridge, so the `C4150` measured on `TUniquePtr<FStratBridge>` -- recorded in

One hit, a prose citation of the measurement, not a declaration. CONTROL: 14 `TPimplPtr` sites
across `Source/**/*.h`.

### 5. Module arrows — pass, DERIVED, 16 rows and 10 fields

The extraction in this reviewer's own checklist was extracted and RUN, not read. **16 rows**,
all five modules present, so the extraction did not fail silently:

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

Row 3 was read at its source rather than treated as unconditional, because the extraction is
textual and cannot show that the row is guarded — `Source/StratBridge/StratBridge.Build.cs:133-138`:

    bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
    PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
    if (!bCompileVendoredRulesHere)
    {
        PrivateDependencyModuleNames.Add("StratRules");
    }

`StratRules` appears in `PublicDependencyModuleNames` nowhere and in
`PrivateDependencyModuleNames` only inside that guard. Not a finding, by the corrected rule.
`StratUI` has gained no `Stratocracy` dependency. No cycle.

The field census printed **10**, no eleventh: `BRIDGE=` (the known string-literal false positive,
deliberately not stripped), `PCHUsage =`, `PrivateDefinitions.Add`,
`PrivateDependencyModuleNames.Add`, `PrivateDependencyModuleNames.AddRange`,
`PublicDependencyModuleNames.AddRange`, `PublicIncludePaths.AddRange`,
`ShadowVariableWarningLevel =`, `bCompileVendoredRulesHere =`, `bUseUnity =`.

No `.Build.cs` is in the diff; this is a re-derivation, not a change audit.

### 6. Module registration — pass

    Stratocracy.uproject Modules: Stratocracy, StratBridge, StratUI, StratPlay
    dirs carrying IMPLEMENT_MODULE / IMPLEMENT_PRIMARY_GAME_MODULE:
      Source/StratBridge/StratBridge.cpp, Source/Stratocracy/Stratocracy.cpp,
      Source/StratPlay/StratPlay.cpp, Source/StratUI/StratUI.cpp

Four and four. `Source/StratRules/` carries no module object and is correctly absent. The diff
adds no `Source/` directory — its only `Source/` path is an existing file.

### 7. `Source/Stratocracy/` untouched — pass

    $ git diff --stat -- Source/Stratocracy
    (nothing)

Same control as check 1.

### 8. `/Game/` literal — pass, with a control

The grep was deliberately run wide (the literal across all `.cpp` and `.h`) rather than narrowed
to the expected defect. It returns 133 hits. Every one is under a `Tests/` directory — the
standing automation-fixture exception — except exactly one:

    Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54
      const FString PackageName = FString::Printf(TEXT("/Game/StratData/%s"), Spec.AssetName);

which is the import commandlet, the other standing exception. No gameplay C++ outside those two
carries a `/Game/` literal, and no `ConstructorHelpers` lookup appears. CONTROL: 326 total
occurrences of the substring across `Source/**/*.{h,cpp}`, so the path was not mangled and the
grep was not running on an empty set.

### 9. Widget-side arithmetic — scoped pass

The only C++ in the diff is a 47-line comment block in a `Tests/` file, verified comment-only
below; no arithmetic could have been introduced by it. `StratUI` as a whole was not re-derived
this pass, and that is stated so the row is not read as a clean full sweep.

### 10. Lane discipline — pass

`Source/StratUI/Tests/StratOptionsModelClauses.cpp` and `Tools/architect/state/tests.md` are
`strat-test-author`'s, and its entry attributes both inline and names its base commit:
"2026-09-05 (local), `strat-test-author` (ACTING and WRITING; IN LANE ... base commit `c69e519`,
and this pass is UNCOMMITTED)". No exception clause is cited and none is needed.
`content.md` and `global.md` are the `coordinator`'s, under the fallback-writer condition and the
`global.md` writer rule respectively. No test code outside `Tests/`, no production code inside
it, no asset changed — `git diff --name-only` names nothing under `Content/`.

### 12. Nothing staged — pass

`git diff --cached --stat` empty; the index carries nothing. The single untracked entry is the
round-1 report.

## The four prior findings, re-derived

**FINDING 1 (the "heard" overclaim) — ANSWERED, AND IT DID NOT UNDER-CLAIM IN THE OPPOSITE
DIRECTION.** The bracket is split at `content.md:176-186`. "is visible, hittable or sensibly
placed" carries `[FALSE SINCE 2026-09-05: a human has seen both buttons and operated them]`,
which `global.md:192-202` supports in terms — the user said the options button works, the sliders
work, the menu exits, and later the same day "all changes to the option menu are good and
considered done". "nor that the sliders move anything a player can hear" is kept
`[STILL TRUE AND DELIBERATELY NOT BRACKETED AS FALSE]`, which is correct: `global.md:194-196`
says of that report, in terms, that it does NOT say "that anything was heard", and
`global.md:126-130` bounds the listening pass to the seven cues and the title music.

**The two files now agree, and the agreement was checked in both directions, not only the one the
finding named.** `content.md:180-186` says the listening pass covered the seven cues and the
title music and NOT the sliders; `global.md:136-143` says the same thing from its own side.
Neither file now asserts a hearing report the other denies. **No under-claim.** The retained
sentence is retained on its own merits and not merely spared: nothing in the tree reports a human
hearing a gain change when a slider moved, and that was searched for BY VALUE rather than by
phrase — the subject tokens heard/audible/hear/listening crossed with slider/volume/gain across
all six record files and all of `Source/`. Every hit is a statement that it has NOT been reported.

The overclaim is recorded rather than deleted, with the reason stated: "a pass whose whole purpose
was record honesty introduced an overclaim into the correction itself". That is the right disposal.

**FINDING 2 (the false justification) — ANSWERED, AND THE REPLACEMENT REASON IS TRUE.**
`content.md:167-175` now says the first draft's "would reach neither" was false "because both are
below this line and both are met by reading downward". Verified against the file's actual
geometry: the sentence is at line 167 and the two stamps open at lines 191 and 199, both below it
and both in normal reading order. The replacement reason — "a reader who arrives at THIS SENTENCE
by a citation may stop at the sentence" — is weaker, sufficient, and true; it is also the reason
Bracket B at line 252 gives, so the file is now internally consistent about why its brackets
exist rather than giving opposite geometric accounts in two places.

**Recording the bad reason in place is judged the right call rather than noise**, for a reason
specific to this file: the defect being repaired throughout is a correction that exists somewhere
the reader does not land, and a bracket that silently swapped its own justification would be an
instance of exactly that. Not a finding either way — it is non-gating prose density.

**FINDING 3 (`StratOptionsModelClauses.cpp`) — ANSWERED. All three sub-claims re-derived and all
three hold.**

*The headings were kept and only the parentheses stamped.* Correct. Lines 38 and 40 still read
"ANY PIXEL, ANY SLIDER, ANY LAYOUT" and "THAT A COMMIT REACHES A SLOT", and both remain true of
that file: it constructs no Slate and asserts nothing about a commit reaching a slot.

*The "yet" analysis.* **The distinction holds, and it is the correct reading.** A bare "not ...
yet" asserts the absence at the time of reading and adds only an expectation of change; it has no
anchor, so it goes false when the absence ends. `StratOptionsWidget.h:43`'s "As of this file's
first commit nothing binds `OnAudioOptionsCommitted` and no WBP derives from this class" names a
moment, so it is still true today — and that file's own stamp at line 93 independently reaches
the same conclusion and spares it in terms. Same subject, opposite outcome, the anchor being the
whole difference. Neither test-file sentence had an anchor, so neither was spared. **This is the
judgement most at risk of over-stamping and it was made correctly.**

*Stamping the citation as well as the claim.* **The wording achieves it.** Lines 79-90 name both
citation phrases verbatim, state the general rule ("A CITATION IS A CLAIM ABOUT ANOTHER FILE'S
CURRENT TEXT AND AGES EXACTLY AS ANY OTHER CLAIM DOES"), and — the part that makes it falsifiable
rather than an assertion — give the instrument. Both halves were re-run here:

    $ git log --reverse --oneline -S "WHAT IS TRUE NOW, each part measured rather than asserted" \
        -- Source/StratUI/StratOptionsWidget.h
    c69e519 The in-match input mode is discharged rather than fixed ...
    $ git show c69e519:Source/StratUI/Tests/StratOptionsModelClauses.cpp \
        | grep -n -E "STAMPED|CORRECTED|\[FALSE|SINCE 2026"
    (nothing)

So the authority was corrected in `c69e519` while the citing site carried no stamp of any kind at
that commit. The claim is exactly as stated, and `--reverse` is the right form for an origin
question.

*The `grep -a -c` versus `grep -a -o` figures.* **Confirmed, both re-measured:**

    $ grep -a -c "StratOptionsWidget" Content/UI/WBP_Options.uasset
    2
    $ grep -a -o "[A-Za-z/._]*StratOptionsWidget[A-Za-z_]*" Content/UI/WBP_Options.uasset | sort | uniq -c
          4 /Script/StratUI.StratOptionsWidget
          1 Default__StratOptionsWidget
          1 StratOptionsWidget
    NEGATIVE CONTROL:
    $ grep -a -c "StratOptionsWidget" Content/UI/WBP_TitleMenu.uasset   -> 0
    $ grep -a -c "Widget"             Content/UI/WBP_TitleMenu.uasset   -> 16

`-c` counts LINES and the file is binary, so 2 and 6 are both right; the stamp says so, which is
what stops a reader comparing them cold from concluding one instrument is broken. The `strings`
premise is also true here: `command -v strings` prints nothing on this box, so a `strings`-based
check would have returned a zero indistinguishable from "not found" — which is why the negative
control is not optional and why it was run. Both clause names the stamp cites exist:
`GATE-TITLEMENU.AllThreeShippedGameModesNameOneOptionsWidgetClass` at
`StratShippedOptionsWidgetClassParity.cpp:117`, and
`GATE-AUDIO-SETTINGS.ThePresenterSeedsThePanelWithoutForwardingACommitAndForwardsARealOne` at
`StratOptionsPresenterClauses.cpp:486`. The binder facts hold: `StratOptionsPresenter.cpp:189` is
the `AddDynamic`, `:308` is the `CommitVolumes` call.

**FINDING 4 (`global.md:136`) — ANSWERED, AND THE TRUE HALF WAS NOT OVER-STAMPED.** The stamp at
`global.md:137-143` marks false exactly "a surface reaches them" and keeps, unstamped, the
narrower claim that "what no clause reaches is whether moving a slider CHANGES AN AUDIBLE GAIN,
which remains a human instrument and has still not been reported". Both dispositions check out:
`WBP_Options` carries the sliders and thirteen clauses stand over that surface, so the first is
correctly false; and no clause anywhere reads an audible gain — `StratSoundCueClauses.cpp:55`
records that the suite runs with no audio device — so the second is correctly kept. **This is the
mirror of Finding 1 and it did not repeat Finding 1's error.**

## What the brief supplied, re-measured here rather than accepted

**Comment-only on `StratOptionsModelClauses.cpp` — CONFIRMED, with three controls.** A stronger
form than the brief's was used: `HEAD:<file>` and the worktree copy were each stripped of
comment-leading and blank lines, line-ending-normalised, whitespace-collapsed, and compared as
sequences — so the comparison also refuses a line commented OUT or uncommented, which a plain
inverted grep can mask.

    HEAD code lines: 114   worktree code lines: 114   -> IDENTICAL
    CONTROL A (a commented line turned into code):  DIFFERS
    CONTROL B (a real code line commented out):     DIFFERS
    CONTROL C (worktree against itself, no-op):     IDENTICAL

The instrument is shown able to say DIFFERS twice before its IDENTICAL is read as a measurement.
No assertion, clause name, fixture or include moved.

**The exported suite report — READ, not inferred.** `Saved/AutomationReport/index.json`, parsed
with `utf-8-sig`:

    reportCreatedOn 2026.09.06-03.28.13
    succeeded 435   succeededWithWarnings 0   failed 0   notRun 0
    entries 435     state histogram: 435 Success
    non-Success list: empty

and it is downstream of every test source:

    report mtime                        2026-09-05 23:28:13 (local)
    newest .cpp carrying IMPLEMENT_*_AUTOMATION_TEST
      2026-09-05 23:26:35  Source/StratUI/Tests/StratOptionsModelClauses.cpp
    report postdates it by 1.6 minutes

The UTC/local day skew is the already-recorded after-20:00 effect, not a discrepancy, and
`tests.md` says so itself. `global.md:33`'s citation was moved to `2026.09.06-03.28.13` and now
matches the file on disk. This reviewer did not run the build or the suite; it read the artefact.

**`strat_banner_sweep.py` — RUN, AND ITS GREEN IS A MEASUREMENT BECAUSE ITS RED WAS MEASURED
FIRST.**

    $ python Tools/architect/strat_banner_sweep.py ; echo EXIT=$?
      automation report: 435 entries, all Success (2026.09.06-03.28.13)
      report identity: mtime=1788665294, reportCreatedOn=2026.09.06-03.28.13
      suite claims found: 63 (1 live, 62 stamped)
    SWEEP CLEAN -- no self-contradiction found
    EXIT=0

CONTROL, on a disposable copy of `Tools/`, `Source/` and `Saved/AutomationReport` outside the
repo, with one line appended to its `content.md` reading "The suite is 999/999 and the phase
verdict is PASS.":

    SWEEP FAILED -- the record contradicts itself or the tree
    EXIT=1
    fired: SUITE COUNT AGREEMENT (three times), RECORD OWNERSHIP, REPORT IDENTITY

The exit code was not taken as a verdict without a failing run first. The real tree carries
exactly one live suite claim, in `global.md`, which is the only file permitted to carry one.

## The independent sweep — TWO methods, and the eighth instance is real

**Method A, subject x polarity, re-run.** Every line naming a subject whose truth moved this
milestone (seen / controls / clauses / heard / input-mode), crossed with an options-or-audio
context token and any absence token however phrased, minus lines whose surrounding 28 lines carry
a correction marker. Corpus: all six files in `Tools/architect/state/`, the Python under
`Tools/editor/`, and every `.h` and `.cpp` under `Source/` including `Tests/` — **278 files, 135
uncovered candidates, all 135 read.** 134 are true statements about what a clause does not pin,
or about a route's limits, and are correctly left alone.

**Method B, NEW THIS ROUND, and it is the one that paid.** The brief was right that the citation
class was invisible to every earlier sweep, so a second instrument was written for it: lines that
rest a claim on ANOTHER file's prose (says so itself, its own statement, a named file records or
states or carries, see a named file, already records, the ruling, a named record file), crossed
with the same context filter and a polarity token in a two-line window either way. **50 citation
sites on moved subjects, 36 uncovered, all 36 read and followed to their authorities.** Three
deserved the trip and survived it:

- `Source/StratPlay/Tests/StratTitleMusicLifecycle.cpp:135-137` cites `AStratShellGameMode`'s
  header for "nothing in C++ checks that pairing". Followed to
  `Source/StratPlay/StratShellGameMode.h:212` and verified against the tree: no clause anywhere
  reads the title track's `SoundClassObject` against the bank's `MusicSoundClass` — a repo-wide
  grep for `SoundClassObject` returns only that header and `StratShippedSoundBankParity.cpp`'s
  CUE clause, which is a different subject. **Still true.**
- `Source/StratPlay/StratOptionsPresenter.h:5-12` quotes `StratOptionsWidget.h`'s debt sentence.
  The quote INCLUDES the anchor — "as of this file's first commit" — so the citation carries the
  qualifier that saves it. **Still true**, and it is the positive example of the shape Finding 3
  was about.
- `Source/StratUI/Tests/StratOptionsWidgetBindingClauses.cpp:61` — "THAT THE SLIDERS ARE LAID
  OUT, VISIBLE, OR OPERABLE BY A MOUSE. No clause in this suite can observe Slate geometry."
  **Still true**; a human report is not a clause.

One did not survive, and it is the finding below.

## Findings

**FINDING 1 — BLOCK. The eighth instance: a discharge condition that has been met, in production
C++ prose, with no correction at the site and none anywhere in the file.**

`Source/StratPlay/StratShellSubsystem.h:125-133` — the amendment block that was written to
correct the paragraph above it, and has now itself gone stale:

    //     THE OWNER HALF LANDED 2026-09-05 AND THE ASSET HALF DID NOT, WHICH IS WHY THE PARAGRAPH
    //   ABOVE IS AMENDED RATHER THAN STRUCK. ... so the sentence "clicking the row runs
    //   `RequestOptionsPanel` and nothing appears" is no longer true OF THE CODE. IT REMAINS
    //   TRUE OF ANY MAP WHOSE GameMode BLUEPRINT HAS NO `OptionsWidgetClass` SET, and that is
    //   every map until the editor lane authors the WBP -- so the weak point named above has
    //   moved from "nothing can show it" to "nothing is configured to", which is a different
    //   and much smaller thing but is not nothing. FULLY DISCHARGED BY the WBP existing and
    //   both GameMode Blueprints naming it.

Three things in that block are false of the current tree, measured:

1. "that is every map" — no shipped map is in that state. `OptionsWidgetClass` is set on all
   three shipped GameModes. Measured on the assets themselves: `grep -a -c "WBP_Options"` returns
   **2** on each of `Content/StratPlay/BP_StratShellGameMode.uasset`, `BP_StratGameMode.uasset`
   and `BP_StratGameMode_AiVsAi.uasset`, against a negative control of **0** on
   `Content/UI/WBP_TitleMenu.uasset`. It is also pinned by a named clause,
   `Stratocracy.StratPlay.GATE-TITLEMENU.AllThreeShippedGameModesNameOneOptionsWidgetClass`
   (`Source/StratPlay/Tests/StratShippedOptionsWidgetClassParity.cpp:117`).
2. "until the editor lane authors the WBP" — the WBP exists and the editor lane did not author
   it. `content.md` records the `coordinator` building it under the editor-driver clause, which
   is the same third route Bracket B in `content.md:252-257` was corrected for failing to model.
   The identical error is uncorrected here.
3. "FULLY DISCHARGED BY the WBP existing and both GameMode Blueprints naming it" — that condition
   is met, and unmarked. **"both" is also wrong on its own count**: there are three, which
   `StratShippedOptionsWidgetClassParity.cpp:24` calls out in terms — "THREE ASSETS AND NOT TWO,
   WHICH IS THE ONE PLACE THIS DEPARTS FROM ITS PRECEDENT."

There is no correction anywhere in the file:

    $ grep -n "STAMPED|WHAT IS TRUE NOW|RESOLVED>|SINCE 2026-09-05" \
        Source/StratPlay/StratShellSubsystem.h     (alternation escaped for basic grep)
    (nothing)

against a control showing the convention live in sibling production headers
(`StratOptionsWidget.h`, `StratOptionsPresenter.h`). **And because this block IS the amendment for
the paragraph at lines 116-124, its going stale leaves that earlier sentence — "no `WBP` bound to
`UStratOptionsWidget` exists yet" — with no live correction at all.** A reader landing at line 118
by a citation now reads a false absence, follows the amendment that promises to correct it, and
gets a second false absence.

Same defect class as round 1's Findings 3 and 4, in a third lane and a fourth file, exactly as the
brief predicted. **Why Method A nearly missed it, stated so the next sweep does not:** the line
carrying the false claim, line 130, DOES pass the context filter and DOES carry a polarity token,
and was excluded only because its subject is the Blueprint property name `OptionsWidgetClass`,
which is not in the five-subject dictionary Method A types out. That is the typed-subject-list
failure this reviewer's own checklist warns about, committed by this reviewer's own instrument.
Method B reached the file by a different route, which is the argument for keeping both.

Owner: `strat-gameplay-engineer` — `Source/StratPlay/StratShellSubsystem.h` is production C++
outside `Tests/`.

## Observations

Non-gating.

- **The four prior findings are all correctly and completely answered.** Round 1's Findings 1-4
  are closed by this report; none is re-raised, and none was closed by deleting the sentence it
  was about.
- **The `.cpp` stamp is a block below the bullets, not an inline bracket, and `content.md` chose
  inline brackets for the same defect shape in the same pass.** The stamp opens "THE TWO BULLETS
  ABOVE" and sits six lines under them in one contiguous comment block, which is close enough that
  a reader opening the file's header meets it; that is why this is not a finding. But two lanes
  have now applied two different remedies to one defect class on one day, and if the inline-bracket
  standard is the house one, `StratOptionsModelClauses.cpp:38` and `:40` want a four-word marker
  apiece.
- **`content.md`'s three new brackets STILL carry no writer attribution or control of their own.**
  Raised as an observation in round 1 and not acted on. The brackets at lines 167, 177 and 179 say
  "FALSE SINCE 2026-09-05" and "A gate caught it" but name no actor; a reader landing on one cannot
  tell who wrote it or under which clause without reading twenty lines on to the stamp at line 199.
  The same is now true of `global.md:137`'s new stamp. One dated attribution line each.
- **`tests.md`'s "totals 6" and the `.cpp` stamp's "4x and ... once" do not visibly reconcile.**
  Both are correct — the sixth occurrence is a bare `StratOptionsWidget` that the `.cpp` does not
  enumerate — but a reader adding the `.cpp`'s figures gets 5 and reads `tests.md`'s 6 as a
  contradiction. One clause naming the third string would close it.
- **`global.md:137-143`'s "THE FIRST HALF / THE SECOND HALF" labels are unanchored.** The stamp is
  unambiguous today about WHICH claim it marks false, because it restates both in its own words —
  so this costs nothing now. But "first half" and "second half" of a sentence whose halves are not
  marked will not survive an edit to the sentence.
- **`global.md:4684`, inside the acceptance-ID ruling, still reads "the two sliders and texts not
  yet exercised".** They are exercised now, by
  `Source/StratUI/Tests/StratOptionsWidgetBindingClauses.cpp`. Left non-gating on the same
  reasoning round 1 applied to `global.md:4811-4818`: the parenthetical identifies a set by its
  state at ruling time, inside a ruling whose whole purpose was to authorise exercising them, and
  it has a citing site (`StratOptionsWidgetBindingClauses.cpp:6`) that quotes it accurately AS
  ruling text. Recorded because it is the same shape as the finding and the distinction between
  them is thinner than a reader should have to reconstruct.
- **`global.md:4811-4818`'s "have no clause proposed yet" and "is undecided" are unchanged from
  round 1.** Still non-gating for the same reason, still one forward pointer away from being
  unambiguous.
- **`.agents/ue-project-context.md` shows no drift** against this pass's derivation — the module
  table, the conditional `StratBridge -> StratRules` edge, the `IMPLEMENT_MODULE` exception for
  `Source/StratRules/`, and the manifest-is-the-authority wording all match the tree.
- **The fallback clause's obligation to measure the absent tool with a control is unfalsifiable
  from a checkout and is recorded as unverified, not as passed.** A tool surface lives in the
  session transcript; no instrument in this tree can see it. The control's SHAPE, as reported, is
  right. That is all a gate can say.

VERDICT: BLOCK
