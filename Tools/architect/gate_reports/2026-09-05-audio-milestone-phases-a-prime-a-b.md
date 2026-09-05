# Gate: audio milestone, phases A-prime / A / B — strat-integration-reviewer

## Scope

Working tree of `E:\MultiAgent\Stratocracy`, branch `master`, over base `089c79c`. Nothing is
committed and nothing is staged; this gate reads the working tree, not `HEAD`.

`git status --porcelain` — 18 paths, 10 modified and 8 untracked, every one with a space in
column 1 (unstaged):

```
 M Source/StratPlay/StratMatchSubsystem.cpp
 M Source/StratPlay/StratMatchSubsystem.h
 M Source/StratPlay/StratPlay.Build.cs
 M Source/StratPlay/StratPlayerController.cpp
 M Source/StratPlay/StratShellGameMode.cpp
 M Source/StratPlay/StratShellGameMode.h
 M Source/StratPlay/StratShellSubsystem.cpp
 M Tools/architect/state/engine.md
 M Tools/architect/state/global.md
 M Tools/architect/state/tests.md
?? Source/StratPlay/StratSoundBank.cpp
?? Source/StratPlay/StratSoundBank.h
?? Source/StratPlay/StratSoundDirector.cpp
?? Source/StratPlay/StratSoundDirector.h
?? Source/StratPlay/Tests/StratSoundDirectorCallSite.cpp
?? Source/StratUI/StratSoundCues.cpp
?? Source/StratUI/StratSoundCues.h
?? Source/StratUI/Tests/StratSoundCueClauses.cpp
```

`git diff --stat -- Source/` on the tracked half is purely additive — 448 insertions, 0
deletions, across 7 files.

I did not run the build and I did not run the suite; that is `strat-test-author`'s report. I
re-derived the suite figure from the exported artifact and re-ran the sweep, both inlined below.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass |
| 2 | No `strat::` call outside `StratBridge/` and `StratRules/` | pass |
| 3 | No vendored header in front of UHT | pass |
| 4 | `TPimplPtr`, not `TUniquePtr`, for any `FStratBridge` member | pass |
| 5 | Module arrows unchanged and correct | pass |
| 6 | New modules registered — unless they have no module object | pass (no new module) |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` path literal in C++ | pass |
| 9 | No widget-side arithmetic | pass |
| 10 | Lane discipline | pass |
| 11 | Prose blocks present | pass |
| 12 | Nothing staged or committed | pass |
| — | **Record integrity (`global.md` stamp chronology)** | **BLOCK** — see Findings |

### 1. Vendored bytes untouched

```
$ git diff --stat -- Source/StratRules Data
(no output)
$ git status --porcelain -- Source/StratRules Data
(no output)
```

Both empty — no tracked edit and no untracked file. Authority derived from the manifests rather
than from prose, as required:

```
$ grep -o '"rulesCommit"[^,]*' Source/StratRules/StratRules.manifest.json
"rulesCommit": "96d93ea9e490c8cc33261f5d9671389a9c76ecfd"
$ grep -o '"dataCommit"[^,]*' Data/StratData.manifest.json
"dataCommit": "c2f58608c77c60c44e6c0fc87988bd3b372beaf5"
```

### 2. No `strat::` call outside the two permitted directories

```
$ grep -rn "strat::[a-z][A-Za-z0-9_]*[[:space:]]*(" Source/ --include=*.cpp --include=*.h \
    | grep -v "^Source/StratBridge/" | grep -v "^Source/StratRules/"
(no output)
```

Zero candidates, so nothing to judge. The only `strat::` tokens anywhere in the eight new/changed
non-record files are inside comments, and three of them are the lanes stating the rule to
themselves — `StratSoundCueClauses.cpp:51`, "NO `strat::` FREE FUNCTION IS CALLED ANYWHERE BELOW
... a `strat::` call here is `LNK2019`, measured 8x"; the same at
`StratSoundDirectorCallSite.cpp:51`; and a reference to `strat::applyCommand`'s indexing at
`StratSoundDirectorCallSite.cpp:1066`. Comments, not calls, and not types either. No finding.

### 3. No vendored header in front of UHT

The three new headers all declare reflected types — `EStratSoundCue`, `FStratUnitSoundMark`,
`FStratSoundMark`, `FStratSoundEmission` (`StratSoundCues.h`), `UStratSoundBank`
(`StratSoundBank.h`), `EStratSoundDisposition`, `FStratSoundEmissionRecord`,
`UStratSoundDirector` (`StratSoundDirector.h`) — and none includes `StratBridge.h` or any of
`Ui.h`, `Data.h`, `Save.h`, `Scenario.h`, `Replay.h`. Their full include lists:

```
StratSoundCues.h    : CoreMinimal.h, StratSoundCues.generated.h   (+ forward decl `struct FStratViewModel;`)
StratSoundBank.h    : CoreMinimal.h, Engine/DataAsset.h, StratSoundCues.h, StratSoundBank.generated.h
StratSoundDirector.h: CoreMinimal.h, Subsystems/WorldSubsystem.h, StratSoundCues.h, StratSoundDirector.generated.h
```

`StratSoundCues.h` forward-declares `FStratViewModel` rather than including `StratViewModel.h`;
the include is `.cpp`-only (`StratSoundCues.cpp:18`). The two modified headers,
`StratMatchSubsystem.h` and `StratShellGameMode.h`, gained only `#include "StratSoundCues.h"` and
the forward declarations `class UStratSoundBank;` / `class UStratSoundDirector;`. Correct shape
throughout.

### 4. `TPimplPtr` for any `FStratBridge` member

No new `FStratBridge` member was added. The only holder in the diff's blast radius is unchanged:

```
Source/StratPlay/StratMatchSubsystem.h:2538:	TPimplPtr<FStratBridge> Bridge;
```

No `TUniquePtr` appears in any new or modified file.

### 5. Module arrows — DERIVED, not trusted

Extracted from the checklist and executed against this tree. It printed **16 rows**, which is the
expected count, and every row matches the block I carry:

```
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
Stratocracy  PublicDependencyModuleNames    Core,CoreUObject,Engine,InputCore,EnhancedInput,AIModule,
                                            NavigationSystem,StateTreeModule,GameplayStateTreeModule,
                                            Niagara,UMG,Slate,Json,StratRules
Stratocracy  PrivateDependencyModuleNames   (none)
Stratocracy  PublicIncludePaths             Stratocracy,Stratocracy/Variant_Strategy,...
```

All five modules are present, so the extraction did not fail silently. Row 3 is the conditional
sixteenth; I read the branch rather than treating it as unconditional, and it is inside the
guard:

```
Source/StratBridge/StratBridge.Build.cs:133:  bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
                                     :134:  PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
                                     :135:  if (!bCompileVendoredRulesHere)
                                     :137:      PrivateDependencyModuleNames.Add("StratRules");
```

`StratRules` appears in no `PublicDependencyModuleNames` and in no *unconditional*
`PrivateDependencyModuleNames`. Not a finding, per the corrected 2026-09-01 rule.

Field census, the second instrument:

```
BRIDGE=                                PrivateDefinitions.Add
PCHUsage =                             PublicDependencyModuleNames.AddRange
PrivateDependencyModuleNames.Add       PublicIncludePaths.AddRange
PrivateDependencyModuleNames.AddRange  bCompileVendoredRulesHere =
ShadowVariableWarningLevel =           bUseUnity =
```

**Ten names.** No eleventh. `BRIDGE=` is the known false positive — the tail of the string literal
`"STRAT_VENDORED_RULES_IN_BRIDGE="` at `StratBridge.Build.cs:134`, quoted above; not chased, and
not filtered away.

**Zero new module dependencies, confirmed.** `git diff -- Source/StratPlay/StratPlay.Build.cs` is
18 added lines and all 18 are comment, inside the existing NOTE ON WHAT IS NOT HERE list. The
argued absence is `MetasoundEngine`, and the file marks its own epistemic status rather than
overclaiming: "THIS IS AN ARGUED ABSENCE AND NOT A MEASURED ONE, WHICH IS A WEAKER THING AND IS
SAID SO. The `InputCore` note below is backed by 2 x LNK2019 with the mangled names quoted; this
one is backed by a green build in which the symbol was never referenced." That is the right
shape. No `.Build.cs` field value changed in this pass, in any module.

**On the placement of `StratSoundCues` (adversarial point 2).** It is consistent with the arrows
and introduces no reverse edge. `StratSoundCues.h/.cpp` sit in `StratUI` and name exactly one
thing outside `Core`: `FStratViewModel`, which is `StratUI`'s own (`StratViewModel.h`), forward
declared in the header and included only in the `.cpp`. The consumers are in `StratPlay`, which
already holds `StratUI` in `PublicDependencyModuleNames` — an existing arrow, used in its existing
direction. The two free functions carry `STRATUI_API` (`StratSoundCues.h:267`, `:304`), which is
what makes the cross-module call link rather than repeat the `_API`-less failure the vendored
sources have. Nothing in `StratUI` gained a `Stratocracy` dependency; nothing in `StratUI` names
anything from `StratPlay`. No cycle.

### 6. New modules registered

No new `Source/<Module>/` directory exists. `ls Source/` is unchanged at `StratBridge`,
`StratPlay`, `StratRules`, `StratUI`, `Stratocracy` plus the two `.Target.cs` files, and
`Stratocracy.uproject`'s `Modules` array is untouched (the file is not in `git status`). The
`StratRules` absence from that array is intact and correct. Nothing to register.

### 7. `Source/Stratocracy/` untouched

```
$ git diff --stat -- Source/Stratocracy
(no output)
```

Also absent from `git status --porcelain`, so no untracked file was added under it either.

### 8. No `/Game/` path literal in C++ (adversarial point 1)

Swept every new and modified file. Six literal hits, and **all six are inside `Source/*/Tests/`**,
which is one of the two standing exceptions:

```
Source/StratUI/Tests/StratSoundCueClauses.cpp:106:  LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"))
Source/StratUI/Tests/StratSoundCueClauses.cpp:107:  LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"))
Source/StratUI/Tests/StratSoundCueClauses.cpp:110:  OutError = TEXT("DT_Units and/or DT_Terrain did not load from /Game/StratData");
Source/StratPlay/Tests/StratSoundDirectorCallSite.cpp:136: Out.UnitTable    = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
Source/StratPlay/Tests/StratSoundDirectorCallSite.cpp:137: Out.TerrainTable = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
Source/StratPlay/Tests/StratSoundDirectorCallSite.cpp:196: UDataTable* const Units = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
```

They are the exact fixture form `.agents/ue-project-context.md` licenses by name
(`LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"))`), and `StratSoundCueClauses.cpp:102`
cites the licence in place. **Legal, and inside the exception.** Reported here because the task
asked to be told either way.

Outside `Tests/`, the only `/Game/` occurrence in any new or changed gameplay file is prose:
`Source/StratPlay/StratSoundBank.h:6`, "...because CLAUDE.md #4 forbids a `/Game/` literal in
gameplay C++...". The bank reaches C++ only through `FStratMatchConfig::SoundBank`
(`StratMatchSubsystem.h`, `UPROPERTY(EditAnywhere, BlueprintReadWrite)`, `TObjectPtr<UStratSoundBank>`)
and `AStratShellGameMode::SoundBank` (`StratShellGameMode.h`,
`UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)`), both with no initializer. No
`ConstructorHelpers` lookup was added anywhere; the tree's only `ConstructorHelpers` include is
the pre-existing UE-template `StratocracyCharacter.cpp:4`, outside this diff.

### 9. No widget-side arithmetic

The only `StratUI` production code added is `StratSoundCues.cpp`, which is not a widget and draws
nothing. I checked its claim rather than accepting it: every decision in the file is an equality
or an order comparison between two readings of one field — `Mark.Turn != Model.Match.Turn ||
Mark.SideToMove != Model.Match.SideToMove` (`:94`), `Before.Hex != Unit.Hex` (`:131`),
`Unit.Hp < Before.Hp` (`:140`) — plus id-set membership (`Living.Contains`). No sum, no
difference, no ratio, no hex distance, no HP delta. The one-per-kind collapse is
`bool bEmitted[static_cast<int32>(EStratSoundCue::MatchEnded) + 1]` (`:67`), a compile-time array
bound and not a drawn value. No widget was modified in this pass at all.

The single subtraction in the whole feature is `(Now - *LastAt) < MinGap` at
`StratSoundDirector.cpp:79`, in `StratPlay`, over `UWorld::GetTimeSeconds()`. A wall clock is not
a rules answer and no view-model field is being re-derived. Not a violation.

### 10. Lane discipline

Clean, and the split is exact. `strat-gameplay-engineer`: seven `Source/` files outside `Tests/`
plus four new non-test files, plus its own `engine.md`. `strat-test-author`: exactly two files,
both under `Source/*/Tests/`, plus its own `tests.md`. `strat-data-steward`: the `GATE-AUDIO`
ruling in `global.md`. `coordinator`: the banner in `global.md`, which is its own file. No test
code outside `Tests/`; no production code inside `Tests/`; no asset touched. Both lane entries
attribute inline and in the required shape:

```
engine.md:18: - **2026-09-04, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on `master` in the main ...
tests.md:17:  - **2026-09-04 (local), `strat-test-author` (ACTING and WRITING; IN LANE -- two files under ...
```

No transcription clause, editor-driver clause or in-session clause was invoked, and none was
needed.

### 11. Prose blocks present

Every new file opens why-first, cites its GDD section and `GATE-AUDIO`, and records measurements
as measurements. `StratSoundCues.cpp` opens with a falsifiable standing claim about itself
("THERE IS NOT ONE ARITHMETIC OPERATION IN IT") which I checked above and which holds. The new
`.Build.cs` entry marks itself as argued rather than measured, quoted under check 5. Debts are
written down in advance rather than quietly owned — `FStratMatchConfig::SoundBank` and
`AStratShellGameMode::SoundBank` both name the two-Blueprint drift they create and both name the
clause that would discharge it.

### 12. Nothing staged or committed

`git status --porcelain`, quoted in full under Scope: all 18 entries have a space in column 1.
Nothing staged. `git log --oneline -1` is `089c79c`, the stated base — no commit was made.

### Re-derived measurements (not accepted from the brief)

Read directly from `Saved/AutomationReport/index.json` with `utf-8-sig`, this seat's own
collector:

```
entries 408
Counter({'Success': 408})
warnings 0
reportCreatedOn 2026.09.05-04.55.48
GATE-AUDIO count 19
```

Zero failed, zero notRun, zero succeededWithWarnings, zero entries carrying warnings. The 19
`GATE-AUDIO` names split 10 `Stratocracy.StratUI.GATE-AUDIO.*` / 9
`Stratocracy.StratPlay.GATE-AUDIO.*`, and every one takes the required
`Stratocracy.<Module>.GATE-AUDIO.<Clause>` shape.

Counted BY MACRO in the tree, per this project's own rule, and the two agree:

```
$ grep -c IMPLEMENT_SIMPLE_AUTOMATION_TEST Source/StratUI/Tests/StratSoundCueClauses.cpp
10
$ grep -c IMPLEMENT_SIMPLE_AUTOMATION_TEST Source/StratPlay/Tests/StratSoundDirectorCallSite.cpp
9
```

10 + 9 = 19 macros in the tree, 19 `GATE-AUDIO` names in the report. 389 + 19 = 408. The delta is
consistent in both directions.

`strat_banner_sweep.py`, run by this seat:

```
Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
  automation report: 408 entries, all Success (2026.09.05-04.55.48)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=408, COMPLEX/_CLASS=0
  report identity: mtime=1788584148, reportCreatedOn='2026.09.05-04.55.48'
  suite claims found: 63 (1 live, 62 stamped)
  tracked items declared: 2
SWEEP CLEAN -- no self-contradiction found
EXIT=0
```

**One live claim**, in `global.md`, which is the only file permitted to carry it. I checked the
other five record files independently: every `N/N` in `engine.md`, `tests.md`, `data.md`,
`decisions.md` and `content.md` is either a stamped historical figure, a build-step counter
(`[81/84] Compile [x64] StratUnitActor.cpp`), a rules figure (`200/200` starting fame) or an HP
glyph (`12/20`) — none is a live suite claim and none is a phase verdict. This pass added no
`N/N` of any kind to `engine.md` or `tests.md`.

### Adversarial points, answered

**Point 3 — `DoesSupportWorldType` mirrors the match subsystem's.** Verified by reading both
bodies. They are identical:

```
StratSoundDirector.cpp:17   bool UStratSoundDirector::DoesSupportWorldType(const EWorldType::Type WorldType) const
                     :21       return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;

StratMatchSubsystem.cpp:94  bool UStratMatchSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
                       :98      return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
```

Same operator, same two enumerators, same order, both `const`. The claim holds.

**Point 4 — disposition before play, no early return on any path.** I read
`UStratSoundDirector::EmitCue` end to end, `StratSoundDirector.cpp:36-121`. There is **no `return`
statement anywhere in the function**. The record is appended at `:46`, six lines in and before the
first branch:

```
	FStratSoundEmissionRecord& Record = Emissions.AddDefaulted_GetRef();
	Record.Cue    = Cue;    Record.Side  = Side;
	Record.UnitId = UnitId; Record.Turn  = Turn;
```

The reference is held, and `Disposition` is then filled in place on exactly one of five mutually
exclusive arms of one if / else-if / else tree, every one of which assigns:

| Arm | Line | Disposition |
|---|---|---|
| `World == nullptr` | `:55` | `NoWorld` |
| `SoundBank == nullptr` | `:62` | `NoBank` |
| `Sound == nullptr` | `:77` | `NoSoundConfigured` |
| `MinGap > 0 && LastAt && (Now - *LastAt) < MinGap` | `:85` | `SuppressedByCooldown` |
| else | `:118` | `Played` (after `PlaySound2D`, and `LastPlayedAtSeconds.Add` at `:116`) |

The chain has no uncovered fall-through: the outer tree's third branch is a bare `else`, and the
inner tree's third branch is a bare `else` too. `++EmitCallCount` at `:41` precedes everything. So
every call appends exactly one record and every record carries a named disposition — including the
null-bank, null-world and cooldown paths named in the task. **`EmitCallCount` and
`Emissions.Num()` are equal by construction**, which is what makes the 19 clauses falsifiable
under `-nullrhi` where nothing can be heard. `LastPlayedAtSeconds` is stamped only on the played
arm, so a suppressed request cannot push the window forward, and `ResetEmissions` (`:131-140`)
deliberately does not clear it — stated in place, with the reason.

The one non-obvious hazard I checked and did not find: `Emissions.AddDefaulted_GetRef()` returns a
reference into a `TArray`, and a reallocation during the branch would dangle it. Nothing between
`:46` and `:118` appends to `Emissions` — the only calls are `SoundBank->SoundFor`,
`MinSecondsBetweenFor`, `World->GetTimeSeconds`, `LastPlayedAtSeconds.Find/Add` and
`PlaySound2D`, none of which reach this object. `EmitCues` (`:123`) loops calling `EmitCue`, so
each iteration re-acquires its own reference. Safe.

**Point 5 — record ownership, and the banner not narrating its own gating.** Ownership is clean
(check 10, check 12 and the sweep above). On the banner itself: `global.md:14-69` contains **no
verdict token, no PASS, no BLOCK, and no reference to a reviewer having run**. It states what was
built, what was measured, what is carried as debt, and what is explicitly not claimed ("NOT YET
AUDIBLE, AND NOTHING HERE CLAIMS OTHERWISE ... no human has heard anything"). It does not narrate
its own gating. Correct. The defect I did find in it is a different one and is below.

**Point 6 — the two recorded debts.** Both are honest limits, not missed seams.

(a) *Per-step kind mapping.* I derived the public surface of `UStratMatchSubsystem` rather than
taking the claim: `public:` runs from `:794` and `private:` begins at `:2023`, and
`FStratAiPlaybackReel AiPlaybackReel;` is declared at `:2748`, well inside the private section.
The public playback surface is `SkipAiPlayback` (`:1873`), `IsAiPlaybackRunning` (`:1904`),
`GetAiPlaybackStepCount` (`:1916`), `GetAiPlaybackCursor` (`:1920`), `AdvanceAiPlaybackOneStep`
(`:1963`) and `WillAiPlaybackRun` (`:2230`) — **counts and control, and not one accessor that
returns a step or its `EStratAiCommandKind`**. `PlayMoveSlideForStep` (`:2169`) and
`FocusPlaybackStep` (`:2375`), the two functions that do take a step, are both private. So a
clause can advance the tour and observe that cues were emitted, but cannot learn what kind the
step it advanced past was, and therefore cannot assert Move -> `UnitMoved`, Attack ->
`UnitAttacked`, Build -> `FactoryBuiltUnit`, EndTurn -> silence. The only alternative is for the
fixture to *compute* which kind step N ought to be, which is precisely the "test that derives what
it then asserts" this project forbids. The debt names the missing seam exactly
(`GetAiPlaybackStepAt(int32)`), which is the right way to carry it. **Honest limit.**

(b) *`Deinitialize`'s `SoundMark` reset.* `SoundMark` is a private `UPROPERTY(Transient)` on the
match subsystem with no getter, and the director that holds the observable record is a
`UWorldSubsystem` that dies with the same world. There is no moment at which a clause holds both a
live director and a post-`Deinitialize` subsystem. The reset is also belt-and-braces: the same two
lines run in `TearDownPresentation`, which **is** reachable between two matches in one session and
**is** the path a clause can pin. So the unpinnable half is the redundant half. This is the
identical gap `StratTransientReceiptCallSite.cpp` already records for `ReceiptMark`, and the two
marks are reset on the same lines in both places so they cannot drift. **Honest limit.**

## Findings

**1. `global.md` dates one pass two different ways, in two stamps nine lines apart, added in the
same working-tree edit by the same seat.** Constraint violated: the record must not make a claim
that is false of itself. `global.md` is the one file that carries the live suite figure, and these
stamps are the mechanism by which a superseded figure stays readable — a supersession chain whose
links disagree about their own order is the one thing that mechanism cannot survive.

```
Tools/architect/state/global.md:88:  [STAMPED 2026-09-05] The suite WAS **389/389**, every entry Success, zero failed, zero
Tools/architect/state/global.md:97:  THE LIVE FIGURE'S REPORT WAS `reportCreatedOn 2026.09.04-16.30.20`. [STAMPED 2026-09-04:
                                     THAT REPORT NO LONGER EXISTS AND A READER CANNOT OPEN IT.
```

Neither stamp exists at `089c79c` — `git show HEAD:Tools/architect/state/global.md` carries the
plain sentences "The suite is **389/389** ..." and "THE LIVE FIGURE'S REPORT IS `reportCreatedOn
2026.09.04-16.30.20`." with no bracket at all. Both were added by this pass, by the
`coordinator`, in one edit, to one file.

The `:97` stamp is additionally self-contradicting on its own terms. Its body reads "the audio
pass's phase-A suite run replaced it with `reportCreatedOn 2026.09.05-03.53.31`, itself since
superseded by the phase-B run **named in the banner above**" — and the banner above is
`global.md:14`, `_Last run 2026-09-05`. A stamp dated 2026-09-04 cannot cite, as already written,
a banner dated 2026-09-05 by the same hand in the same edit.

This is not the local-vs-UTC trap and I checked that first. Both lane entries and the steward's
ruling date this pass **2026-09-04 local** — `engine.md:18` ("2026-09-04,
`strat-gameplay-engineer`"), `tests.md:17` ("2026-09-04 (local), `strat-test-author`"),
`global.md:4376` ("2026-09-04, STRAT-DATA-STEWARD") — while the report's `2026.09.05-*` timestamps
are the known UTC-ahead form and cannot arbitrate between them. So the record's other four entries
agree on one date, and `global.md` alone says both. I am not ruling which date is right; that is
the owner's call. The file must not say both.

**Owner: the `coordinator`.** `global.md` is this seat's own file and the fix is in lane — make
the two stamps and the banner header agree on one local date, and if `_Last run 2026-09-05` is the
correct one, reconcile it with the four entries that say 2026-09-04.

## Observations

Non-gating.

- **The `GATE-AUDIO` ruling authorizes a file that does not exist yet.** `global.md:4404` names
  `Source/StratPlay/Tests/StratShippedSoundBankParity.cpp` as one of the three files the ID
  covers; `ls Source/StratPlay/Tests/` does not contain it. That is forward authorization for the
  phase-C asset-parity clauses and is legitimate — the ruling is explicitly per-subject and names
  its own limits — but a reader grepping for the third file today will not find it. Worth a stamp
  when phase C lands, at the steward's discretion.
- **The `:97` stamp asserts a figure from an artifact that no longer exists** — "that intermediate
  report also read 389 entries, all Success" about `reportCreatedOn 2026.09.05-03.53.31`, which has
  since been overwritten in place. The stamp says so itself in the same breath, which is the honest
  handling, and this gate cannot verify it either. Recorded, not charged.
- **Six `ButtonClick` sites, and the count checks out.** `SubmitProductionChoice`
  (`StratMatchSubsystem.cpp`, at function entry above every refusal), `RequestEndTurn`,
  `OpenProductionMenuAtFocusedFactory`, `CloseProductionMenu`, `SkipGuidance` (the four
  `StratSoundClick(this)` calls in `StratPlayerController.cpp`, whose file-local helper's own block
  says "called from four places") and `ExecuteRoute` (`StratShellSubsystem.cpp`). Six, as claimed,
  and `ToggleProductionMenu`'s deliberate exclusion is argued in place ("A KEY IS NOT A BUTTON").
- **The two-Blueprint `SoundBank` drift is real, named in advance, and undischarged.**
  `FStratMatchConfig::SoundBank` and `AStratShellGameMode::SoundBank` are two asset pointers with
  no C++ author to reconcile them, unlike `SaveSlotName` whose two copies agree by construction.
  Both headers state it plainly and both name the phase-D CDO-comparison clause that would
  discharge it. Nothing to fix now; this is the note that it is outstanding.
- The measurements in the brief were all re-derived here and all held: 16 rows, 10 fields, 408/408
  all Success, 19 `GATE-AUDIO` names, macro count 19, sweep exit 0. The one thing I did not and
  cannot verify is that the report's tree is byte-identical to the tree I audited; I did not build
  and did not run the suite, and the sweep's `report identity` line (`mtime=1788584148`) is the
  strongest binding available to me.

VERDICT: BLOCK
