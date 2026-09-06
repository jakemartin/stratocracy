# Gate: content.md stamp density, RE-GATE round 3 (post-`c69e519`) — strat-integration-reviewer

## Scope

Uncommitted working tree at `E:\MultiAgent\Stratocracy`, branch `master`, head
`c69e519bcf0bd22c394e51cc8e44006ccb27d9f2`. Measured, not taken from the brief:

    $ git status --porcelain
     M Source/StratPlay/StratMatchSubsystem.h
     M Source/StratPlay/StratShellGameMode.h
     M Source/StratPlay/StratShellMenuWidget.h
     M Source/StratPlay/StratShellSubsystem.h
     M Source/StratUI/StratCommandBarWidget.h
     M Source/StratUI/StratOptionsWidget.cpp
     M Source/StratUI/StratScoreboardHUD.h
     M Source/StratUI/Tests/StratOptionsModelClauses.cpp
     M Tools/architect/state/content.md
     M Tools/architect/state/engine.md
     M Tools/architect/state/global.md
     M Tools/architect/state/tests.md
    ?? Tools/architect/gate_reports/2026-09-05-content-md-stamp-density.md
    ?? Tools/architect/gate_reports/2026-09-05-regate-stamp-density-round-2.md
    $ git diff --cached --stat
    (empty)
    $ git diff --stat | tail -1
     12 files changed, 584 insertions(+), 30 deletions(-)

TWELVE modified files, nothing staged, nothing committed. The two untracked entries are this
reviewer's own round-1 and round-2 reports and are not findings. This report supersedes neither;
both stand as the record of their rounds and neither is overwritten.

The audit is: the working-tree diff; round 2's single finding re-derived rather than accepted;
the four specific judgements the brief asked for; the brief's own three measurements re-derived
by this reviewer's own instruments; and an independent sweep of the area BOTH prior sweeps left
uncovered.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — with a control |
| 2 | No `strat::` call outside `StratBridge/` or `StratRules/` | pass — with a control |
| 3 | No vendored header in front of UHT | pass — with a control |
| 4 | `TPimplPtr`, not `TUniquePtr`, for any `FStratBridge` member | pass — with a control |
| 5 | Module arrows unchanged and correct | pass — DERIVED, 16 rows / 10 fields, guard read at source |
| 6 | New modules registered — unless they have no module object | pass — no new module directory |
| 7 | `Source/Stratocracy/` untouched | pass — with a control |
| 8 | No `/Game/` path literal in C++ | pass — with a control; both added hits are comment lines |
| 9 | No widget-side arithmetic | pass — the `StratUI` diff contains ZERO non-comment added lines |
| 10 | Lane discipline | pass |
| 11 | Prose blocks present / record accurate | **BLOCK** — Finding 1 |
| 12 | Nothing staged or committed | pass — with a control |

### 1. Vendored bytes untouched — pass, with a control

    $ git diff --stat -- Source/StratRules Data
    (nothing)
    CONTROL, the same instrument on a path that DID move:
    $ git diff --stat -- Source/StratUI
     Source/StratUI/StratCommandBarWidget.h            | 24 ++-
     Source/StratUI/StratOptionsWidget.cpp             |  9 ++-
     Source/StratUI/StratScoreboardHUD.h               | 20 ++-
     Source/StratUI/Tests/StratOptionsModelClauses.cpp | 47 +++++

The empty result is a measurement. The manifests are the authority and neither moved:
`rulesCommit 96d93ea9e490c8cc33261f5d9671389a9c76ecfd`,
`dataCommit c2f58608c77c60c44e6c0fc87988bd3b372beaf5`.

### 2. `strat::` calls — pass, with a control

    $ grep -rn -E "strat::[a-z][A-Za-z0-9_]*[[:space:]]*\(" Source/ --include=*.cpp --include=*.h \
        | grep -v "^Source/StratBridge/" | grep -v "^Source/StratRules/"
    (nothing)
    CONTROL, the same pattern inside the permitted directory:
    Source/StratBridge/StratBridge.cpp:168: strat::parseScenario(ToStd(Text), ...)
    Source/StratBridge/StratBridge.cpp:175: strat::validateScenario(Parsed, Units, Terrain);
    Source/StratBridge/StratBridge.cpp:182: if (!strat::seedFromScenario(GameState, Parsed, ...))

Zero hits outside the two directories, and the pattern is shown able to match. The
lowercase-initial form separates a call from a type; no type name is reported as a violation.

### 3. Vendored header in front of UHT — pass, with a control

    $ grep -rn -E '^[[:space:]]*#include[[:space:]]*"(StratBridge|Ui|Data|Save|Scenario|Replay)\.h"' \
        Source/ --include=*.h   ->   8 distinct headers:
      Source/StratBridge/StratBridge.h and Source/StratRules/{Ai,Driver,Economy,Move,Replay,Scenario,Ui}.h

None declares a reflected type. CONTROL: 59 headers under `Source/` match
`^[[:space:]]*(UCLASS|USTRUCT|UENUM)`, so the reflected-type half of the test can speak.

### 4. `TPimplPtr` — pass, with a control

    $ grep -rn "TUniquePtr<FStratBridge>" Source/
    Source/StratUI/StratViewModel.h:47: // ... the `C4150` measured on `TUniquePtr<FStratBridge>` ...

One hit, a prose citation of the measurement, not a declaration. CONTROL: 15 `TPimplPtr` sites
across `Source/**/*.h`. **The count moved from 14 to 15 since round 2, and the reason is inside
this diff and is benign**: `StratScoreboardHUD.h`'s new stamp contains the words "the `TPimplPtr`
on that subsystem". Named here because a moving count on a safety check should never be left
unexplained.

### 5. Module arrows — pass, DERIVED, 16 rows and 10 fields

The checklist's extraction was extracted and RUN, not read. **16 rows**, all five modules
present, so the extraction did not fail silently:

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
textual and cannot show that the row is guarded — `Source/StratBridge/StratBridge.Build.cs:133-135`:

    bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
    PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
    if (!bCompileVendoredRulesHere)

So `StratBridge -> StratRules` is the monolithic-only link edge, which is deliberate and is not a
finding. The field census printed **10** names, matching the checklist as restated, including the
known `BRIDGE=` false positive that is the tail of the string literal
`"STRAT_VENDORED_RULES_IN_BRIDGE="` inside `PrivateDefinitions.Add`. No eleventh name.
No `.Build.cs` is in this diff; the derivation is a drift check, not a change review.

### 6. New modules — pass

`Source/` holds exactly five module directories and no directory is new in this diff.
`Stratocracy.uproject`'s `Modules` array names `Stratocracy`, `StratBridge`, `StratUI`,
`StratPlay`. **`StratRules` is absent and must stay absent** — it carries no `IMPLEMENT_MODULE`,
and listing it once made the editor abort at startup. Correct as-is.

### 7. `Source/Stratocracy/` untouched — pass, with a control

    $ git diff --stat -- Source/Stratocracy
    (nothing)
    CONTROL: $ git diff --stat -- Source/StratPlay
     4 files changed, 154 insertions(+), 18 deletions(-)

### 8. `/Game/` literals — pass, with a control

    $ git diff -U0 -- Source/ | grep '^+' | grep '/Game/'
    +//   `coordinator` building `/Game/UI/WBP_Options` under `CLAUDE.md`'s EDITOR-DRIVER CLAUSE,
    +//   `GlobalDefaultGameMode=/Game/StratPlay/BP_StratGameMode`. Both resolve to a GameMode that

Both added hits are comment lines. CONTROL: the same diff carries 258 added lines, so the filter
had material to work on. Across the whole tree, every `/Game/` hit outside `Tests/` and the import
commandlet is a comment; the two standing exceptions are unchanged.

### 9. Widget-side arithmetic — pass, and the instrument is stronger than "no arithmetic found"

    $ git diff -U0 -- Source/StratUI | grep '^+' | grep -vE '^\+\s*(//|\*|/\*)'
    (only the four +++ file headers)

The `StratUI` half of this diff contains **zero non-comment added lines**, so there is no
arithmetic to judge. This is a categorical pass rather than a search that happened to find
nothing.

### 10. Lane discipline — pass

Seven non-`Tests/` `Source/` files and `engine.md` -> `strat-gameplay-engineer`, in lane.
One `Tests/` file and `tests.md` -> `strat-test-author`, in lane. `global.md` -> the `coordinator`
and the steward, in lane. `content.md` -> the `coordinator` under the standing fallback clause.
No test code outside `Tests/`, no production code inside `Tests/`, no asset changed.

### 12. Nothing staged — pass, with a control

    $ git diff --cached --stat
    (empty)
    CONTROL: $ git diff --stat | tail -1
     12 files changed, 584 insertions(+), 30 deletions(-)

The index is empty against a working tree that demonstrably is not.

---

## Re-derivation of the brief's three measurements

### (a) Comment-only across all eight `Source/` files — CONFIRMED, but **the brief's control is false**

The conclusion is correct and I re-derived it independently. Stripping `^\s*//`, `^\s*\*` and
`^\s*/\*`, CR-normalised, `git show HEAD:<file>` against the worktree copy:

    SAME(code)  Source/StratPlay/StratMatchSubsystem.h
    SAME(code)  Source/StratPlay/StratShellGameMode.h
    SAME(code)  Source/StratPlay/StratShellMenuWidget.h
    SAME(code)  Source/StratPlay/StratShellSubsystem.h
    SAME(code)  Source/StratUI/StratCommandBarWidget.h
    SAME(code)  Source/StratUI/StratOptionsWidget.cpp
    SAME(code)  Source/StratUI/StratScoreboardHUD.h
    SAME(code)  Source/StratUI/Tests/StratOptionsModelClauses.cpp

**The control offered for it does not speak.** The brief states: *"the same filter against
`8477a43` DIFFERS, so the instrument speaks."* Run, it does not:

    $ (strip filter) 8477a43 vs HEAD, all eight files   ->  same x8
    $ git diff --stat 8477a43 HEAD -- <the eight files>
    (nothing)

**Those eight files are byte-identical at `8477a43` and at `HEAD`**, so no filter could have
differed and the reported control could not have fired as described. This does not change the
verdict on the tree — but a control that cannot speak is exactly the failure this project gates
on, and it was offered for the check that certifies no executable bytes moved.

**A control that DOES speak, supplied here.** Against a disposable copy of
`StratScoreboardHUD.h` in a scratch directory, injected immediately after `#pragma once`:

    MUTANT A (one code line, comments untouched):  static int StratMutantProbe = 1;
      -> filter output DIFFERS:  "1a2 > static int StratMutantProbe = 1;"   [SPEAKS]
    MUTANT B (one comment line, no code touched):  // MUTANT COMMENT LINE
      -> filter output identical                                            [correctly blind]

The two mutants together show the filter is sensitive to exactly the thing the claim is about and
insensitive to exactly the thing it is meant to ignore. **On that instrument, comment-only is
confirmed for all eight files.** No assertion, clause name, include or `UPROPERTY` specifier moved.

### (b) `strat_banner_sweep.py` — CONFIRMED

    $ python Tools/architect/strat_banner_sweep.py > out.txt 2>&1 ; echo EXIT=$?
    EXIT=0
      report identity: mtime=1788665294, reportCreatedOn='2026.09.06-03.28.13'
      suite claims found: 63 (1 live, 62 stamped)
      tracked items declared: 2
    SWEEP CLEAN -- no self-contradiction found

Exit code captured by redirect, per this project's own rule that an exit code is not a verdict
until measured. The liveness control is in the sweep's own output: it collected **63 suite claims
and 2 tracked items**, so `SWEEP CLEAN` is a parsed result and not a silent no-op.

### (c) The `read_macro_census` branch — CONFIRMED, and it is stronger than the engineer claimed

Read at the script, `Tools/architect/strat_banner_sweep.py:739-747`:

    s = len(re.findall(r"IMPLEMENT_SIMPLE_AUTOMATION_TEST", body))
    c = len(re.findall(r"IMPLEMENT_COMPLEX_AUTOMATION_TEST(?:_CLASS)?", body))
    if s or c:
        try:
            mtime = os.path.getmtime(path)
            if newest_mtime is None or mtime > newest_mtime:
                newest_mtime = mtime

`newest_mtime` is updated **only inside `if s or c:`**. Confirmed. And:

    $ grep -c "IMPLEMENT_SIMPLE_AUTOMATION_TEST\|IMPLEMENT_COMPLEX_AUTOMATION_TEST" \
        Source/StratUI/StratOptionsWidget.cpp                     -> 0
    $ ... Source/StratUI/Tests/StratOptionsModelClauses.cpp       -> 2

So the touched non-`Tests/` `.cpp` cannot enter `newest_test_mtime`. **The engineer stopped one
step short of the check that actually matters, and it also passes.** The OTHER `.cpp` in this diff
DOES carry macros and CAN stale the report, so I ran the script's own function directly:

    newest_test_mtime = 1788665195.48  (2026-09-05 23:26:35)  <- StratOptionsModelClauses.cpp
    report identity     1788665294     (2026-09-05 23:28:14)
    macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=435, COMPLEX/_CLASS=0

The report post-dates the newest test source by 99 seconds, and the macro census is **435**,
matching the 435-entry report exactly. **The no-re-run decision is correct on both `.cpp` files,
not just the one the record reasons about.**

---

## The four judgements the brief asked for

### 1. Declining the `BindWidget` flip on two sites — **RIGHT, and it is not a debt dressed as restraint**

Verified at the sites and against the tree:

- `.uasset` is LFS, so no checkout-based instrument can read the button off the shipped asset.
- `Source/StratPlay/Tests/StratCommandBarOptionsBindClauses.cpp:52` disclaims it verbatim:
  *"THAT `WBP_CommandBar` CARRIES AN `OptionsButton`"*, listed among what it does NOT pin.
- Both stamps quote `content.md` **verbatim and accurately** — `content.md:133`
  *"`WBP_CommandBar` given `OptionsButton` in its `Bar`"* and `content.md:131-132`
  *"`WBP_TitleMenu` REPARENTED to `/Script/StratPlay.StratShellMenuWidget` and given
  `OptionsButton` + `OptionsLabel`, with `Btn_0..Btn_3` and their graph bindings intact."*
  A stamp resting on testimony must at minimum quote the testimony correctly. These do.
- The risk is asymmetric in a way the decision correctly weighs: a `BindWidget` failure is a
  Widget Blueprint COMPILE error that reds the entire widget, and the fact authorising it is a
  record sentence. Testimony is adequate to correct a false absence; it is not adequate to
  authorise a hard compile-time dependency.
- **A `BindWidget`/`BindWidgetOptional` flip is a `UPROPERTY` meta change and therefore a compiled
  change** — it moves `.gen.cpp` metadata and the WBP compile contract. Taking it would have
  breached this pass's own comment-only scope, which the eight-file derivation above certifies.

**The distinguishing test between restraint and a hidden debt is whether the decline names its
successor, and both sites do**: `StratCommandBarWidget.h` — *"DISCHARGED INSTEAD BY a clause
reading the shipped `WBP_CommandBar` CDO's `OptionsButton` ... That clause is the test lane's"*;
`StratShellMenuWidget.h` — *"DISCHARGED BY a clause reading the shipped `WBP_TitleMenu` CDO's
`OptionsButton`. That clause is the test lane's."* A debt dressed as restraint declines and names
nothing. This converts an unearned remedy into a named, runnable, owned condition — which is
precisely the remedy this pass argues for elsewhere in its own record.

### 2. Stamping on a RECORD READ rather than a measurement — **ACCEPTABLE, because the stamp says so**

Both sites label their own evidence grade in their own text, in capitals, at the site — not in a
record file a reader may never open:

    StratCommandBarWidget.h:  THAT IS A RECORD READ, NOT A MEASUREMENT: `.uasset` is LFS and no
                              clause in this tree pins the button's presence.
    StratShellMenuWidget.h:   THAT IS A RECORD READ AND NOT A MEASUREMENT -- `.uasset` is LFS,
                              and no clause in this tree asserts the button's presence on the
                              shipped asset.

The failure mode this project has measured is a stamp that presents testimony AS measurement,
which is unfalsifiable from a checkout. A stamp that names its evidence grade AND names the
stronger instrument that would replace it is a correct record, not a weaker one. The alternative —
leaving a sentence that is flatly false of the tree standing because the only available evidence
is a record read — is strictly worse: it preserves a false present tense in order to protect an
evidentiary nicety. **Correct call.**

### 3. The three-of-five accounting — **CONFIRMED, and it is stated against its own interest**

Remedy under test: *"a discharge condition must name an instrument a reader can run."*

| Shape | Claim | My finding |
|---|---|---|
| (a) correction below the claim | catches | **Confirmed.** The instrument sits IN the claim, so a reader who never reaches the correction still holds a runnable check. |
| (b) rotted amendment | catches, strongest case | **Confirmed, and I can strengthen it.** `StratScoreboardHUD.h`'s rot is provable ONLY because the amendment named its condition — *"discharged the day the subsystem lands"* — eleven lines above. I verified the criterion fires: `UStratMatchSubsystem::HandBridgeToScoreboard` at `StratMatchSubsystem.cpp:3514` calls `HUD->AdoptBridge(*Live, OutFailureReason)` at `:3546`, reached from `StartMatchInternal` at `:499`. |
| (c) uncorrected citation | catches | **Confirmed.** A cited CLAUSE NAME is re-runnable; a cited SENTENCE is not. That is the whole difference and it is correctly identified. |
| (d) bare quantifier | does NOT catch | **Confirmed.** "Both" is a claim about the instrument's ARITY, not about the instrument. The concession offered alongside it is the honest one: naming `AllThreeShippedGameModesNameOneOptionsWidgetClass` would have made the miscount FINDABLE without PREVENTING it. |
| (e) false when written | does NOT catch | **Confirmed.** An instrument named inside a false sentence is still inside a false sentence. Only running it before committing fixes that, which is a discipline and not a wording rule. |

Three of five. The accounting is correct, and a pass that had wanted to flatter its own remedy had
an easy route to four — (d) is genuinely half-caught, and claiming it would have been defensible.
It did not.

**One reinforcement from my own sweep below**: the tenth instance I found NAMES its instrument
(*"a clause reading both Blueprint CDOs"*) and rotted anyway — but it rotted into a state a reader
CAN mechanically settle, because the named clause now exists and can be grepped. That is the
accounting behaving exactly as it predicts: a rot detector, not a truth check.

### 4. Over-stamping — **NO TRUE SENTENCE WAS MARKED FALSE. Every inline marking checked.**

This was the judgement the brief most wanted checked, so every one is listed.

- **`StratMatchSubsystem.h`** — `[BOTH SENTENCES STILL TRUE OF THE CODE -- the discharge below is
  a TEST, not a reconciliation]`. **Correct.** An asset pointer still has no C++ author; a parity
  clause observes drift, it does not reconcile it.
- **`StratShellGameMode.h` `SaveSlotName`** — *"'both' is CORRECT here and only here: it reads TWO
  CDOs"*. **Correct, and verified at the clause.** `StratShellBlueprintSlotParity.cpp:110` and
  `:114` load exactly two paths — `BP_StratShellGameMode_C` and `BP_StratGameMode_C` — and `:77`
  states its own limit in terms: *"It does not pin `BP_StratGameMode_AiVsAi` or any other
  subclass."* **This is the trap the pass was most likely to fall into and it did not**: the same
  word is wrong on the two properties beside it and right here, and it was left alone.
- **`StratShellGameMode.h` `OptionsWidgetClass`** — `[STILL TRUE, AND IT IS THE REASON THE CLAUSE
  IS WORTH ITS COST]` on the mid-match-drift argument. **Correct.** A discharge retires the debt,
  not the reasoning that made it worth naming.
- **`StratShellSubsystem.h`** — `[STILL TRUE AS A CONDITIONAL AND DELIBERATELY NOT BRACKETED
  FALSE -- the set it quantifies over is now empty]`. **Correct, and the sharpest call in the
  pass.** The conditional *"any map whose GameMode Blueprint has no `OptionsWidgetClass` set"*
  remains true; what changed is that its extension is empty. Marking it false would itself have
  been the error, and the block says so.
- **`StratCommandBarWidget.h`** — `[... The REASONING it supports survives unchanged -- the two
  changes were still in two lanes and still could not be atomic]`. **Correct.**
- **`StratScoreboardHUD.h`** — *"WHAT DID NOT CHANGE ... the OWNED path is still here and still
  must be"*. **Correct.** The fixtures in `Source/StratUI/Tests/` still hand in a bare
  `FStratBridge`, and the two-member answer is described accurately.
- **`content.md`** — `[STILL TRUE AND DELIBERATELY NOT BRACKETED AS FALSE]` on *"nor that the
  sliders move anything a player can hear"*. **Correct, and it records its own near-miss.** An
  earlier draft claimed HEARD; nobody has reported hearing a gain change.
- **`content.md`** — `[THAT LAST CLAUSE IS STILL TRUE ... it is a claim about what THIS ENTRY
  discharged, which no later event can change]`. **Correct.** A claim scoped to an entry cannot be
  falsified by a later entry.
- **`global.md:137`** — `THE FIRST HALF IS NARROWER THAN IT LOOKS AND IS NOT BEING STAMPED FALSE`.
  **Correct.** No clause reaches whether a slider changes an audible gain; that remains a human
  instrument and is unreported.

Zero over-stamps. The one imprecision I found anywhere in the stamps is recorded under
Observations and does not rise to a finding.

---

## Spot-check of the sweep's own coverage claim

**Denominator re-derived and it is exact.**

    $ find Source/StratPlay Source/StratUI -name '*.h' -o -name '*.cpp' | grep -v '/Tests/' | wc -l
    71        (StratPlay 47 + StratUI 24)

**71 confirmed.** The claim is correctly SCOPED — it says *"71 non-`Tests/` `.h`/`.cpp` files in
`Source/StratPlay` and `Source/StratUI`"* and does not claim tree-wide coverage. That distinction
is what keeps Finding 1 below from falsifying it, and it is the first sweep on this project to
state a denominator at all.

**True positives spot-checked at their instruments — all hold:**

- `AllThreeShippedGameModesNameOneOptionsWidgetClass` — `StratShippedOptionsWidgetClassParity.cpp:117`;
  loads THREE paths at `:93`, `:97`, `:101`; `TestNotNull` at `:136`, `:157`, `:171`, `:205`.
  Reads three, asserts non-null, asserts agreement. As claimed.
- `EveryShippedGameModeNamesTheSameSoundBank` — `StratShippedSoundBankParity.cpp:378`; three paths
  at `:129`, `:136`, `:142`. As claimed.
- `HandleOptionsDismissed` — `StratOptionsPresenter.cpp:313-325`: the cue is emitted at entry
  before any check, and `:315` cites `UStratOptionsWidget::HandleBackClicked` by name, exactly as
  the stamp says.
- `StratOptionsPresenter.cpp:189` and `:308` — both line citations are **exactly right**: `:189`
  is `Created->OnAudioOptionsCommitted.AddDynamic(`, `:308` is
  `Director->CommitVolumes(InModel.MasterVolume, ...)`.
- `HandBridgeToScoreboard` / `AdoptBridge` — verified above under judgement 3.

**Confirmed NEGATIVES spot-checked — all hold, and one is stronger than stated:**

- *"the title map acquiring a controller class of ours"* — `grep -n PlayerControllerClass` over
  `StratShellGameMode.{h,cpp}` returns nothing. CONTROL: the same token appears at
  `StratGameMode.cpp:52` (`PlayerControllerClass = AStratPlayerController::StaticClass();`), so
  the instrument speaks. **Still open.**
- *"NOTHING IN C++ CHECKS THAT PAIRING"* (`TitleMusic` vs `MusicSoundClass`) — **still open, and I
  checked the harder version of this.** `StratShippedSoundBankParity.cpp` DOES load
  `kMusicSoundClassPath` at `:937`, which could easily have looked like a discharge. Read at the
  site, the music class is used **only as a set-difference CONTROL** (`:950`: *"the SFX class and
  the music class are two DIFFERENT objects"*), and the parity assertion at `:994` reads
  `Sound->SoundClassObject` off the CUES. Nothing reads the title track's sound class. The
  engineer's one-word summary — *"the class clause is the SFX class"* — is correct in substance.
- `StratShellSubsystem.h` and `StratOptionsWidget.h` stamped last pass and unchanged — verified.
- **Two more I checked on my own initiative and both hold.** `StratOptionsPresenter.h:88`'s
  *"DISCHARGED BY a human at the keyboard"* looked unstamped to a `STAMPED`-token grep, but `:91`
  carries `DISCHARGED 2026-09-05, IN THE DIRECTION THAT LEAVES THE CODE ALONE` plus an explicit
  `WHAT THAT REPORT DOES NOT COVER` block. Correctly not re-stamped. `StratSoundBank.h:171`'s
  *"DISCHARGED WHEN a second bank exists"* — `find Content -iname "*bank*"` returns exactly one,
  `Content/StratAudio/DA_StratSoundBank.uasset`. Still open.

**Provenance re-derived, both directions, and both are right:**

    $ git log --diff-filter=A --oneline -- Content/UI/WBP_Options.uasset          -> d80b28a
    $ git log --reverse -S "THE OWNER HALF LANDED 2026-09-05 AND THE ASSET HALF DID NOT" \
        -- Source/StratPlay/StratShellSubsystem.h                                 -> 374398a
      order oldest->newest: d80b28a, c13eb81, 374398a   => TWO commits later. FALSE WHEN WRITTEN.

    $ git log --reverse -S "NOTHING ASSERTS THE" -- Source/StratPlay/StratShellGameMode.h  -> 374398a
    $ git log --reverse -- Source/StratPlay/Tests/StratShippedOptionsWidgetClassParity.cpp -> 8477a43
      8477a43 is the commit immediately after 374398a => TRUE FOR EXACTLY ONE COMMIT.

Both use `--reverse`, which is the right form for an ORIGIN question. The two findings really are
opposites, and the record's insistence on the distinction is earned.

---

## The tenth-instance sweep — MY METHOD, MY DENOMINATOR, AND IT IS NOT A NULL RESULT

**Scope, chosen to be disjoint from both prior sweeps.** The engineer's covered
`Source/StratPlay` + `Source/StratUI` non-`Tests/`. Mine covers what that left: every
`Source/**/Tests/` `.h`/`.cpp`, `Tools/editor/`, `Config/`, all six `Tools/architect/state/*.md`,
and `.agents/ue-project-context.md`.

**Method 1 — mechanical referent resolution (a different ORACLE, not a different word list).**
Rather than matching prose SHAPES, extract every backticked identifier occurring in a sentence
carrying an absence or futurity token, then mechanically test whether that identifier RESOLVES in
the tree. The instrument, inlined rather than cited by path so a later reader can re-run it:

    ABSENCE = r"(does not exist|do not exist|no such|has no|have no|not yet|yet|until the|
                once the|when the|unwritten|nothing asserts|no clause|absent|missing|
                DISCHARGED BY|DISCHARGE)"
    TICK    = r"`([A-Za-z_][A-Za-z0-9_:./-]{3,})`"
    for each line in scope: if ABSENCE matches and TICK yields ids ->
        candidate; then grep the id's core token across Source/ Tools/ Content/

    RESULT: 128 files scanned, 89 candidate files, 686 candidate lines, 759 resolved referents.

**759 is unreadable, and I say so rather than reporting a number I did not triage.** The oracle is
too weak: an identifier EXISTING is not the claim; the claim is about a STATE of that identifier.
Recorded as a measurement of the method, not thrown away — per this project's own rule that
instruments stay loose and claims stay strict, the fix is to narrow the ORACLE, not the collector.

**Method 2 — discharge-condition enumeration, same scope, narrowed oracle.** All ten known
instances share one shape: a DISCHARGE CONDITION whose condition is now met. So enumerate every
discharge line in the uncovered scope and evaluate each condition against the tree:

    PAT = r"DISCHARGE[SD]?\b|WILL BE DISCHARGED|IS DISCHARGED"   (case-insensitive)
    SCOPE:      123 files
    CANDIDATES: 402 lines across 25 files

**402 candidates, triaged at their sites.** Most are correctly-closed discharges, `RETRACTED>`
blocks, or already-stamped. **One is a true positive, and it is the tenth instance** — Finding 1.
A second, `StratTitleMusicLifecycle.cpp:137`, is a genuine open debt correctly stated (see
Observations).

**So the sweep is NOT a null result, and the denominator is stated so that it would have been a
measurement if it had been.**

---

## Findings

**FINDING 1 — BLOCK. The TENTH instance: a flat, present-tense, false absence claim about two
shipped assets, in `Tests/`, whose discharge condition is met on BOTH halves — in a file that
already carries nine corrections in exactly the idiom the remedy calls for.**

`Source/StratPlay/Tests/StratShellRouteClauses.cpp:1852-1854`:

    // WHAT IT DOES **NOT** PIN, and the debt is real: a Blueprint that OVERRIDES either property
    // re-opens exactly the drift this pins in C++. That discharges on a clause reading both
    // Blueprint CDOs, and those assets do not exist yet.

**Both halves of that discharge condition are met, measured:**

1. **The assets exist and are tracked.**

        $ git ls-files Content/StratPlay/BP_StratShellGameMode.uasset Content/StratPlay/BP_StratGameMode.uasset
        Content/StratPlay/BP_StratGameMode.uasset
        Content/StratPlay/BP_StratShellGameMode.uasset
        $ git log --diff-filter=A --oneline -- Content/StratPlay/BP_StratShellGameMode.uasset  -> e4a21b0
        $ git log --diff-filter=A --oneline -- Content/StratPlay/BP_StratGameMode.uasset       -> d310aa1
        CONTROL, an asset that genuinely does not exist:
        $ ls Content/StratPlay/BP_ThisDoesNotExist.uasset
        ls: cannot access ...: No such file or directory

2. **The clause the sentence waits for already exists.**
   `Stratocracy.StratPlay.GATE-TITLEMENU.BothShippedGameModeBlueprintsNameOneSaveSlot` at
   `Source/StratPlay/Tests/StratShellBlueprintSlotParity.cpp:124`, loading both CDOs at `:110`
   and `:114`.

**It was TRUE when written and aged into falsity**, which places it in the same class as this
pass's `StratShellGameMode.h` finding rather than the `StratShellSubsystem.h` one:

    $ git log --reverse -S "those assets do not exist yet" \
        -- Source/StratPlay/Tests/StratShellRouteClauses.cpp   -> a6ebbe2  (2026-08-30 19:59)
    $ e4a21b0 (BP_StratShellGameMode added)                                 2026-08-31 00:03
    $ git merge-base --is-ancestor a6ebbe2 e4a21b0  -> true

**What makes this the sharpest instance yet, and why it is a BLOCK rather than an observation:**

- **The file that discharges it KNOWS the assets exist and says so.**
  `StratShellBlueprintSlotParity.cpp:13-14` reads: *"W6's asset tail created
  `BP_StratShellGameMode` and the debt became payable. This file pays it."* Two files in the same
  lane, one recording the assets landing and paying the debt, the other still saying they do not
  exist. That is not a subtle drift; it is a flat contradiction inside one lane.
- **The remedy is this file's own established idiom, not a convention it lacks.** The file already
  carries nine corrections in the house form —
  `grep -nE "STAMPED" Source/StratPlay/Tests/StratShellRouteClauses.cpp` returns lines
  `59, 73, 80, 146, 166, 255, 1448, 1455, 1624`, including `**[STAMPED 2026-08-30]**` at `:1448`
  and `**[STAMPED 2026-08-30]** That was false` at `:1455` — six and four lines above the
  offending sentence. A reader landing on line 1854 is inside a region whose neighbours were
  corrected and this one was not.
- It is the same defect class as round 1's Findings 3 and 4 and round 2's Finding 1, now in a
  fifth file and a third lane. Blocking on it is the consistent call; not blocking would make the
  two prior BLOCKs arbitrary.

**This does NOT falsify the engineer's sweep coverage claim.** That claim is explicitly scoped to
71 non-`Tests/` files in `StratPlay` and `StratUI`; `Source/StratPlay/Tests/` is outside it by
construction. The sweep scoped itself honestly and its 71 / 86 / 7-plus-1 figures stand.

**Owner: `strat-test-author`** — `Source/StratPlay/Tests/StratShellRouteClauses.cpp` is test code
in `Tests/`, and the entry belongs in `tests.md`. The remedy is a bracket at the sentence plus a
stamp naming `GATE-TITLEMENU.BothShippedGameModeBlueprintsNameOneSaveSlot` as the instrument, in
the file's own `**[STAMPED ...]**` form.

## Observations

Non-gating.

- **The brief's comment-only CONTROL is false and could not have fired as described** (see the
  re-derivation section). The conclusion it supported is correct, and I re-derived it with a
  two-mutant control that does speak. Recorded because the failure mode — an absence reported as
  measured, on a control that could not have fired — is one this project gates on, and here it
  appeared in the gate REQUEST rather than in the tree.
- **`StratShellGameMode.h`'s new `SoundBank` stamp slightly mischaracterises the second clause.**
  It says `ShippedGameModesAuthorTheirSoundBank` *"covers the hole an agreement-only clause leaves
  -- three inherited nulls agree."* But the FIRST clause is not agreement-only: it asserts
  non-null per asset at `StratShippedSoundBankParity.cpp:389`, `:407` and `:439`, so that hole was
  already closed. The second clause also reads two CLASSES (`AStratGameMode`,
  `AStratShellGameMode`), not three. The conclusion is right and no reader is misled about the
  tree; the reasoning is one degree looser than the rest of the pass.
- **Both new `BindWidget` sites cite `StratShippedOptionsWidgetClassParity.cpp` as the precedent
  for the clause they propose, and the precedent is not a drop-in.** That file reads a
  `TSubclassOf` property off a GameMode CDO. Reading a `BindWidget`-resolved `UWidget*` off a
  `UWidgetBlueprintGeneratedClass` CDO is a different read with traps this project has already
  measured — a widget CDO's tree is not populated the way a live instance's is. The test lane
  should be told the precedent is directional, not mechanical.
- **`StratOptionsModelClauses.cpp`'s stamp cites `StratOptionsPresenter.cpp:189` and `:308`.**
  Both are exactly right today. They are also the one citation form this project has recorded as
  rotting silently, and the same stamp argues in its own words that a citation ages like a claim.
  Quoted text would fix it.
- **`content.md`'s brackets STILL carry no writer attribution.** Raised in round 1, again in round
  2, unacted-on in round 3. The new brackets at `content.md:164-190` and `:252-266` say *"A gate
  caught it"* and *"A gate caught the bad reason"* but name no actor, no date-line and no clause.
  `content.md` is written by the `coordinator` under the standing fallback clause, whose own
  obligation is that acting and writing are attributed separately; an in-place bracket inherits no
  attribution from a header a citing reader never reaches. One dated attribution line each.
  Recorded a third time rather than escalated, because the checklist item it would fall under
  governs new files and new `.Build.cs` entries, not in-place record brackets.
- **`Source/StratPlay/Tests/StratTitleMusicLifecycle.cpp:137`** — *"names the clause that
  discharges it, once both assets carry it. Not this file's, and not written here."* Surfaced by
  my sweep and triaged as a **genuine open debt**, correctly stated, per the sound-class
  verification above. Recorded so that "swept and cleared" is distinguishable from "not looked at".
- **`engine.md`'s entry is the strongest record entry in this milestone.** It names its base
  commit, states that no exception clause applies, gives its sweep's denominator BEFORE its
  result, separates true positives from confirmed negatives from already-stamped, records the
  build with an action list AND an immediate-re-run control AND independent DLL mtimes, and
  answers the judgement question against its own interest.
- **`.agents/ue-project-context.md` shows no drift** against this pass's derivation — the module
  table, the conditional `StratBridge -> StratRules` edge, the `IMPLEMENT_MODULE` exception for
  `Source/StratRules/`, and the manifest-is-the-authority wording all match the tree.
- **Ten instances across ten passes, none ever found by the pass that created it, and this one
  again not found by its own lane.** The pattern is stable enough to be a design input rather than
  an incident: the only sweeps that have ever found one are sweeps run by someone other than the
  author, over a scope the author's own sweep declared out of bounds.

## Disclosure — this reviewer modified the tree and reverted it

**Recorded here rather than left to be discovered, because a gate that quietly edits the tree it
is auditing has invalidated its own subject.** While building the negative control for the banner
sweep, this reviewer ran a guarded append against `Tools/architect/state/global.md`:

    cp Tools/architect/state/global.md <scratch>/global.bak
    echo "" >> Tools/architect/state/global.md 2>/dev/null && echo "(cannot write ...)"

The guard was wrong. The append SUCCEEDED and added one blank line, which surfaced in the diff as
a lone `+` at `global.md:8253`. It was caught by re-reading my own diff output rather than by any
instrument.

**REVERTED, byte-exactly, from the copy taken in the same command immediately before the append:**

    sha256 BEFORE revert  efddc6905a80917355dcfc3bed9a8ed77adfbb40646412aec5c296db99012cd4
    sha256 AFTER  revert  15044760ef21d414fb8dc507b67c7a220893889bdcdcc7267cad1523b40e32b9
    sha256 of backup      15044760ef21d414fb8dc507b67c7a220893889bdcdcc7267cad1523b40e32b9
    $ diff <scratch>/global.bak Tools/architect/state/global.md   -> IDENTICAL

`git status --porcelain` after the revert shows the same twelve modified files and no thirteenth,
and `strat_banner_sweep.py` re-run on the restored tree still prints `SWEEP CLEAN`, exit `0`.

**What this does and does not contaminate.** The banner-sweep result quoted earlier in this report
was measured BEFORE the append (the sweep ran first in the same command block) and again AFTER the
revert, with the same result both times. `global.md` is not a `Source/` file, so the eight-file
comment-only derivation is untouched. **No finding in this report rests on any byte I wrote**, and
the tree handed back is byte-identical to the tree handed over, plus this report as an untracked
file.


VERDICT: BLOCK
