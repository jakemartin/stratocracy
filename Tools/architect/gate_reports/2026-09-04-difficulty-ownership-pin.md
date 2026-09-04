# Gate: Difficulty-ownership pin (T-FAME-02.ShippedGameModeInheritsDifficultyFromCpp) -- strat-integration-reviewer

## Scope

The UNCOMMITTED working tree at `E:/MultiAgent/Stratocracy`, branch `master`.

Base verified by this seat, not accepted from the brief:

    $ git log --oneline -1
    15bd1b0 A line count in the banner became a quoted-words pointer, and the thing worth
            reading is that the count was wrong before anyone touched it and the previous
            commit widened the error by nine without going near the sentence

    $ git status --porcelain --untracked-files=all
     M Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
     M Tools/architect/state/global.md
     M Tools/architect/state/tests.md

Three modified paths, no untracked strays, nothing staged. `git diff --stat`: 3 files,
518 insertions, 22 deletions. Code and clauses were audited first, the record second, per the
standing user ruling on gate priority.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass |
| 2 | No `strat::` call outside `StratBridge`/`StratRules` | pass |
| 3 | No vendored header in front of UHT | pass |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass |
| 5 | Module arrows unchanged and correct | pass |
| 6 | New modules registered (unless no module object) | pass -- no new module |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` path literal in C++ | pass -- fixture exception, pre-existing |
| 9 | No widget-side arithmetic | pass -- no `StratUI` change in the diff |
| 10 | Lane discipline | pass |
| 11 | Prose blocks present | pass |
| 12 | Nothing staged or committed | pass |

### 1 -- vendored bytes

    $ git diff --stat -- Source/StratRules Data
    (no output)

Nothing. The hash gate stands at whatever `Source/StratRules/StratRules.manifest.json` and
`Data/StratData.manifest.json` record; neither manifest is in the diff.

### 2 -- strat:: containment

    $ grep -rnE 'strat::[a-z][A-Za-z0-9_]*[[:space:]]*\(' Source/ --include=*.cpp --include=*.h \
        | grep -v '^Source/StratBridge/' | grep -v '^Source/StratRules/'
    (no hits)

    $ grep -n 'strat::' Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp
    (none)

No candidates to judge this pass -- neither a call nor a type name. The new clause reaches
everything through `FProperty` reflection and `AStratGameMode`, both module-side.

### 3 -- vendored headers in front of UHT

Every header containing the token `UCLASS`/`USTRUCT`/`UENUM` was scanned for a direct include of
`StratBridge.h`, `Ui.h`, `Data.h`, `Save.h`, `Scenario.h` or `Replay.h`. The only file returned
was `Source/StratBridge/StratBridge.h` itself (lines 53-58). JUDGED, NOT FORWARDED: that file
declares no UHT macro --

    $ grep -nE '^[[:space:]]*(UCLASS|USTRUCT|UENUM)[[:space:]]*\(' Source/StratBridge/StratBridge.h
    (no macro at line start)

-- the six hits are its own prose saying so, e.g. `StratBridge.h:262`: "this header must declare
no `UCLASS`/`USTRUCT`/`UENUM`, because ... includes go in front of the header tool." A false
positive; not a finding. The changed `.cpp` adds one include, `UObject/UnrealType.h`, an engine
header in a `.cpp`.

### 4 -- pimpl

    $ grep -rn 'TUniquePtr<[[:space:]]*FStratBridge' Source/
    Source/StratUI/StratViewModel.h:47:// holds a bridge, so the `C4150` measured on `TUniquePtr<FStratBridge>` -- recorded in

A comment recording the measurement, not a declaration. No member changed in this diff.

### 5 -- module arrows, DERIVED

No `.Build.cs` is in the diff, but the graph was derived from the tree anyway rather than trusted.
Extraction (comments stripped, both `.Add` and `.AddRange` call forms):

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
                                                GameplayStateTreeModule,Niagara,UMG,Slate,Json,
                                                StratRules
    Stratocracy  PrivateDependencyModuleNames   (none)
    Stratocracy  PublicIncludePaths             Stratocracy,Stratocracy/Variant_Strategy,...

SIXTEEN rows, five modules, three fields each plus the conditional
`StratBridge PrivateDependencyModuleNames StratRules`. No module is missing from the output, so
the extraction did not silently fail. Every row matches the expected graph. `StratUI` has gained
no `Stratocracy` dependency; no cycle.

The sixteenth row is confirmed GUARDED by reading the file rather than the extraction, which
cannot show it -- `Source/StratBridge/StratBridge.Build.cs:133-138`:

    bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
    PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
    if (!bCompileVendoredRulesHere)
    {
        PrivateDependencyModuleNames.Add("StratRules");
    }

Monolithic-only, from the one expression that also decides the compile-in. Not unconditional, not
in `PublicDependencyModuleNames`. Correct and deliberate; not a finding.

Field census, the dumb instrument, unmodified:

    BRIDGE=
    PCHUsage =
    PrivateDefinitions.Add
    PrivateDependencyModuleNames.Add
    PrivateDependencyModuleNames.AddRange
    PublicDependencyModuleNames.AddRange
    PublicIncludePaths.AddRange
    ShadowVariableWarningLevel =
    bCompileVendoredRulesHere =
    bUseUnity =

TEN names, exactly the documented ten, `BRIDGE=` being the known string-literal tail of
`"STRAT_VENDORED_RULES_IN_BRIDGE="`. No eleventh name.

### 6 -- module registration

No new `Source/<Module>/` directory. `Stratocracy.uproject` is not in the diff.
`Source/StratRules/` remains absent from the `Modules` array, which is correct.

### 7 -- game module

    $ git diff --stat -- Source/Stratocracy
    (no output)

### 8 -- /Game/ literals

The only two in the changed file are `kShippedGameModeClassPath` and `kAiVsAiGameModeClassPath`
(lines 117 and 121), both UNCHANGED by this diff and both inside an automation-test fixture --
the standing exception. The new clause adds no path literal; it reaches the shipped class through
the existing `ResolveCdoOrFail(*this, kShippedGameModeClassPath)`.

### 10 -- lanes

`Source/StratPlay/Tests/StratShippedGameModeMatchConfig.cpp` is test code inside a `Tests/`
directory -- `strat-test-author`, in lane. `Tools/architect/state/tests.md` is that agent's own
record file. `Tools/architect/state/global.md` is the coordinator's own file. No production
code inside `Tests/`, no test code outside it, no asset touched, no `.agents/` change, no
existing gate report modified (`git status` over `Tools/architect/gate_reports/` returns empty).

### 12 -- staging

`git diff --cached --stat` is empty. All three paths are worktree-only modifications.

---

## The clause itself

The four questions the brief put, answered against the source rather than against the record.

**Does it establish what its NAME says?** Yes -- both halves, and the second half is asserted
rather than assumed, which is the part that usually goes missing.

*Inheritance* is the terminal assertion:

    TestTrue(..., DifficultyProp->Identical_InContainer(ShippedStruct, NativeStruct));

where `ShippedStruct` and `NativeStruct` are `MatchConfigProp->ContainerPtrToValuePtr<void>()`
over the shipped CDO and the super class CDO respectively. That is a question about DELTA, not
about value, so it answers what the two clauses beside it structurally cannot.

*FromCpp* is paid for separately, before anything is compared:

    UClass* const Super = Generated ? Generated->GetSuperClass() : nullptr;
    if (!TestTrue(... "PREMISE: BP_StratGameMode's parent class is the NATIVE AStratGameMode ..."
            Super == AStratGameMode::StaticClass()))
    {
        return false;
    }

An interposed `BP_StratGameModeBase` would leave "inherits" true and make "from C++" false; this
assertion is the one that stops the name overclaiming, and the header says so in those terms. It
composes with `ResolveCdoOrFail`'s own pre-existing assertion that the resolved class is NOT
native, so the pair under test is provably BP-derived-directly-from-C++. Both halves of the name
are earned. This is the axis the previous pass was re-gated on and it has been answered.

**Are the controls real -- would either pass on an inert instrument?** Yes, and no.

Control 1 brackets the comparator in BOTH directions ON `Difficulty` ITSELF, which is precisely
what `ShippedGameModeRunsAtEasy` cannot do (its liveness rides `AiSides`, a neighbouring field):

    FStratMatchConfig Copy = Shipped->MatchConfig;
    // 1a -- TestTrue: the faithful copy still compares IDENTICAL to the archetype
    DifficultyProp->Identical_InContainer(&Copy, NativeStruct)
    ...
    const EStratDifficulty OtherTier =
        (Shipped->MatchConfig.Difficulty == EStratDifficulty::Hard)
            ? EStratDifficulty::Easy
            : EStratDifficulty::Hard;
    Copy.Difficulty = OtherTier;
    // 1b -- TestFalse: move ONLY Difficulty and the SAME call must report a DIFFERENCE
    DifficultyProp->Identical_InContainer(&Copy, NativeStruct)

An `Identical_InContainer` stuck at true (wrong property, wrong offset, inert comparator) reddens
1b. One stuck at false reddens 1a and returns early. Neither degenerate instrument survives.
The comparison exercised is the same call, on the same property, against the same archetype
object as THE CLAIM -- not a rehearsal on a proxy. The moved-to tier is derived from the tier
read, so the control does not go stale if the shipped tier ever changes.

Control 2 kills the object-compared-to-itself case:

    TestFalse(..., AiSidesProp->Identical_InContainer(ShippedStruct, NativeStruct));

reinforced by an explicit `NativeDefaults != Shipped` guard earlier. Without it, "identical on
Difficulty" would be true of a self-comparison for the worst possible reason. Both controls are
live; neither passes on an inert instrument.

A third, quieter check ties the reflected handle to the C++ member the game actually reads --
`ExportTextItem_InContainer` on `DifficultyProp` compared against
`DescribeDifficulty(Shipped->MatchConfig.Difficulty)`. Without it a correct-looking comparison
could be running over some other property.

**Is per-member granularity pinned by an assertion, and is it sound?** Yes, and yes.

    TestFalse(
        TEXT("the WHOLE MatchConfig struct compares NON-identical between the shipped CDO and its "
             "archetype, because AiSides differs. ... a per-STRUCT comparison would conclude that "
             "the Blueprint authors the difficulty tier, and it would be wrong."),
        MatchConfigProp->Identical_InContainer(Shipped, NativeDefaults));

This is the assertion the brief asked about and it is sound: the struct-level comparison returns
the OPPOSITE answer to the member-level one, and that fact is now asserted rather than commented,
so a future simplification to a struct compare reddens here instead of silently inverting the
conclusion. Verified independently that all three members exist and are reflected --
`Source/StratPlay/StratMatchSubsystem.h:436` `EStratDifficulty Difficulty = EStratDifficulty::Easy;`
under `UPROPERTY(EditAnywhere, BlueprintReadWrite, ...)`, `:387` `TArray<int32> AiSides;` likewise,
and `Source/StratPlay/StratGameMode.h:103` `FStratMatchConfig MatchConfig;` under
`UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ...)`. Every `FindPropertyByName` result is
`TestNotNull`-guarded with an early return, so a rename fails rather than comparing nothing.

**Is anything asserted that the instrument cannot support -- specifically about a designer?**
No. This is the place an overclaim would have landed and it did not. The header's first
"DOES NOT PIN" bullet states the limit, and -- more importantly, because the failure message is
what a reader on a red actually sees -- THE CLAIM's own message carries it inline:

> "WHAT THIS CANNOT TELL YOU, and it is a limit of the data and not of the test: a property set
> in the editor to the value its parent already holds contributes no delta either, so this does
> not distinguish 'never touched' from 'set to the same tier'. It is a claim about WHERE THE
> RUNTIME VALUE COMES FROM, which is answered either way."

The name is `Inherits...FromCpp`, an ownership claim, not `DesignerLeftItAlone`. The name and the
assertion set agree, and the disclaimer travels with the name in the message rather than living
only in a header a citation-arriving reader never reaches.

**The two existing clauses -- PROSE ONLY, verified rather than accepted.** The claim was checked
mechanically, not read: the twelve assertion call sites present in the HEAD blob are identical
in call form and order to the first twelve in the worktree file, and both existing
`IMPLEMENT_SIMPLE_AUTOMATION_TEST` blocks retain their class names and their registered strings
(`...ShippedGameModeAuthorsOneAiSide`, `...ShippedGameModeRunsAtEasy`). The only substantive edit
inside an existing clause is the failure-message literal of the `ShippedGameModeRunsAtEasy`
`TestEqual`; the asserted pair `ShippedTier, EStratDifficulty::Easy` is untouched. Nothing was
removed, loosened or renamed. The superseded closing sentence in that clause header
("No arrangement of assertions can close that gap...") is STAMPED in place under
`<SUPERSEDED 2026-09-04>` and re-quoted rather than deleted, with the correction restated flat.

---

## The record

**tests.md -- no live N/N, no phase verdict.** Three N/N-shaped hits exist and all three are
judged non-claims: `200/200` (a `startingFame` value), `1757/635/107` (CR/LF byte counts), and
`69/69` in a historical entry reading "Suite was 69/69 at that pass". The sweep classifier agrees
-- it finds 62 suite claims tree-wide, 1 live and 61 stamped, the single live one being the
`global.md` banner. No verdict token is asserted in `tests.md`; every `VERDICT` occurrence there
is a quotation of a past gate or a statement that this file does not carry one.

**The global.md live banner, RE-MEASURED by this seat.** `Saved/AutomationReport/index.json` read
with `utf-8-sig`; the log was not used.

    reportCreatedOn = 2026.09.04-16.30.20
    succeeded = 389   failed = 0   notRun = 0   succeededWithWarnings = 0
    entries: 389      states: {'Success': 389}
    new clause present by name: True

All nine `T-FAME-02.*` clauses Success, including
`Stratocracy.StratPlay.T-FAME-02.ShippedGameModeInheritsDifficultyFromCpp`.

The 388 -> 389 delta was re-derived by set difference on the test MACRO, one collector run over
both sides -- the worktree and the HEAD blobs via `git show` -- rather than accepted:

    worktree macro names: 389
    HEAD     macro names: 388
    added  : ['Stratocracy.StratPlay.T-FAME-02.ShippedGameModeInheritsDifficultyFromCpp']
    removed: []
    report names: 389
    in tree not in report: []
    in report not in tree: []

+1 added, 0 removed, name sets equal in BOTH directions between tree and report. Every figure in
the banner -- 389/389, the `reportCreatedOn`, the delta, addition-not-rename -- reproduces.

**The three stamps on the previous banner.** All three land in place, re-quote what they retire
with a `STAMPED>` prefix on every wrapped line, and leave no live claim underneath: the retired
`388/388` sentence, the `reportCreatedOn 2026.09.04-04.47.20` citation, and the second citation of
that same artifact inside the timestamp-drift sentence. The now-discharged debt paragraph is
likewise stamped rather than deleted, and its closing parenthesis -- which terminates the whole
`_Last run 2026-09-04 (` parenthetical -- survives at the end of the stamped block, so the entry
is still balanced. The sweep confirms mechanically that the stamps land in its per-sentence
window:

    $ python Tools/architect/strat_banner_sweep.py
    Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
      automation report: 389 entries, all Success (2026.09.04-16.30.20)
      macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=389, COMPLEX/_CLASS=0
      report identity: mtime=1788539420, reportCreatedOn='2026.09.04-16.30.20'
      suite claims found: 62 (1 live, 61 stamped)
      tracked items declared: 2

    SWEEP CLEAN -- no self-contradiction found
    $ echo EXITCODE=$?
    EXITCODE=0

Measured with no pipe, so the code is the sweep own and not a tail status.

**Attribution.** The new `tests.md` entry opens "2026-09-04 (local), `strat-test-author` (ACTING
and WRITING; IN LANE ... base commit `15bd1b0`, UNCOMMITTED)" -- inline, both roles, base named.
The `global.md` banner states it was written by the coordinator, whose file it is, in lane and
under no exception clause, and that the lane was dispatched and wrote its own entry. No
transcription clause is invoked and none is needed; no in-session clause is invoked. The record
is consistent with the lane split observable in the diff: the `.cpp` and `tests.md` on one side,
`global.md` on the other.

**Housekeeping claimed and verified.** No `Saved/AutomationReportMutant` directory survives, no
`MUTANT` token remains in the `.cpp`, no untracked strays anywhere in the tree.

**Drift check on .agents/ue-project-context.md.** Read and compared against the derivation. Its
`StratBridge` row already carries the monolithic-only link edge with the guard expression named,
and its `Source/StratRules/**` row already defers to the manifest for the gated commit rather than
quoting a stale one. No drift found; no correction owed.

## Findings

None.

## Observations

Non-gating, recorded rather than acted on.

1. **A moment-scoped tree measurement inside the file it measures.** The new `tests.md` entry
   closes with "Working tree at the end of the pass: `git status --porcelain
   --untracked-files=all` lists exactly one path, `StratShippedGameModeMatchConfig.cpp`
   (modified)". A reader running that command from this checkout gets three paths, one of which
   is `tests.md` itself. The entry own header discloses the second file ("one file under
   `Source/StratPlay/Tests/`, plus this file"), and the load-bearing half of the sentence -- no
   untracked strays, nothing staged, probe and mutant report directory gone -- is TRUE right now
   and was re-verified above. So this is the known record-measures-itself shape rather than a
   false claim, and it is not a finding. Worth a line because the same sentence in a future entry
   may not have a true half to fall back on.

2. **The archetype is derived, not read.** The clause obtains its comparison target as
   `Super->GetDefaultObject<AStratGameMode>()` while the prose calls it "ITS ARCHETYPE". For a
   Blueprint CDO parented directly to a native class those are the same object, and the clause
   separately ASSERTS that parentage, so the derivation is sound and arguably stronger than
   `GetArchetype()` -- it fails loudly if the assumption stops holding instead of silently
   following a new archetype. Noted only so a later reader does not mistake the wording for a
   `GetArchetype()` call.

3. **TestEqual on FString is case-insensitive on this engine.** The reflected-versus-typed
   consistency check compares the `ExportTextItem_InContainer` output against
   `DescribeDifficulty(...)`. Both currently produce the enum short name and the check is a
   consistency guard rather than the claim, so nothing rests on it -- but it would not catch a
   case-only divergence. No action.

VERDICT: PASS
