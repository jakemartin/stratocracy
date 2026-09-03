# Gate: unit movement + existence hold, SECOND RE-GATE — strat-integration-reviewer

Tree: `E:/MultiAgent/Stratocracy` (confirmed by `git -C E:/MultiAgent/Stratocracy
rev-parse --show-toplevel`). Branch `master`, HEAD `283d711`, working tree DIRTY.
No worktree was used.

**NOTHING IS INHERITED FROM EITHER PRIOR GATE, INCLUDING THEIR CLEAN CODE PASSES.** The
twelve-item checklist, the module graph, the field census, the `.uasset` name-table
measurement, the sweep probes and the report figures were all re-derived from scratch in
this pass. Where a prior report's conclusion is restated here it is because I re-measured
it, not because it was carried forward.

## Scope

The whole uncommitted working tree against `283d711`: **26 entries** from
`git status --porcelain` — 20 modified, 6 untracked — 8,606 insertions / 126 deletions
across 20 tracked files. Nothing staged (`git diff --cached --stat` empty). HEAD is
unmoved.

Since the last gate, `Tools/architect/state/engine.md` is the only file that changed
(F4 + twin repairs, the 76-file claim-shape sweep, the ~21-site debt declaration, and the
struck-and-replaced deferral reason). **No `Source/` byte moved**, which I confirmed
independently rather than accepted: the newest file under `Source/` by mtime is
`Source/StratPlay/Tests/StratAiPlaybackClauses.cpp` at 2026-09-03 15:45:58, and the
exported report was written at 15:47:45.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass |
| 2 | No `strat::` call outside `StratBridge/` and `StratRules/` | pass |
| 3 | No vendored header in front of UHT | pass |
| 4 | `TPimplPtr`, not `TUniquePtr`, for any `FStratBridge` member | pass |
| 5 | Module arrows unchanged and correct | pass |
| 6 | New modules registered (or correctly absent) | pass |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` path literal in C++ | pass |
| 9 | No widget-side arithmetic | pass |
| 10 | Lane discipline | pass |
| 11 | Prose blocks present | pass |
| 12 | Nothing staged or committed | pass |

### Code, assets and module graph — re-derived from scratch

Reported as its own section per the user's standing gate-priority ruling.

**1. Vendored.** `git diff --stat -- Source/StratRules Data` prints nothing;
`git status --porcelain -- Source/StratRules Data` prints nothing, so there are no
untracked additions either. The manifests are the authority and were read rather than
quoted from the checklist: `rulesCommit` `96d93ea9e490…`, `dataCommit` `c2f58608c77c…`.

**2. `strat::` calls.** The lowercase-initial call pattern across `Source/`, minus the two
permitted directories, returns **zero**. I then re-ran unfiltered for `strat::` of any case
outside those directories and judged every hit: they are prose comments plus
`strat::UnitType` enumerator reads in
`Source/Stratocracy/StratData/Tests/StratDataParity.cpp` — **type names, not calls, and
legal**. Reported here as judged, not forwarded.

**3. UHT.** `Source/StratBridge/StratBridge.h` includes `Data.h`, `Replay.h`, `Save.h`,
`Scenario.h`, `Ui.h`, and my first grep flagged it because the file *mentions*
`UCLASS`/`USTRUCT`/`UENUM`. Judged: every occurrence (lines 30, 119, 262, 264, 323, 486)
is inside a prose block explaining why this header declares none; there is no macro at the
start of a line and no `.generated.h`. **Not a finding.** No other header in the tree
includes a vendored header.

**4. Pimpl.** `TPimplPtr<FStratBridge>` at `StratMatchSubsystem.h:2448` and
`StratScoreboardHUD.h:1235`. Every `TUniquePtr` occurrence in the tree is prose recording
the `C4150` measurement. No `TUniquePtr<FStratBridge>` member exists.

**5. Module graph.** The extraction command was extracted from the checklist and executed,
not retyped. It prints **16 rows**, all five modules present:

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
Stratocracy  PublicDependencyModuleNames    Core,CoreUObject,Engine,InputCore,EnhancedInput,
                                            AIModule,NavigationSystem,StateTreeModule,
                                            GameplayStateTreeModule,Niagara,UMG,Slate,Json,StratRules
Stratocracy  PrivateDependencyModuleNames   (none)
Stratocracy  PublicIncludePaths             Stratocracy,Stratocracy/Variant_Strategy,...
```

Row by row against the checklist block: every row matches. **The sixteenth row is the
conditional one and I read the branch rather than treating the row as unconditional** —
`Source/StratBridge/StratBridge.Build.cs:133-138`:

```
bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
if (!bCompileVendoredRulesHere)
{
    PrivateDependencyModuleNames.Add("StratRules");
}
```

So `StratRules` is in `PrivateDependencyModuleNames` **inside** the guard, never
unconditionally, and never in `PublicDependencyModuleNames`. **Not a finding, per the
2026-09-01 correction.** The modular branch still compiles the vendored sources: all
twelve `Source/StratBridge/Vendored/*.strat.cpp` are present and each is bracketed by
`#if STRAT_VENDORED_RULES_IN_BRIDGE`, with an `#error` on the not-defined path. `StratUI`
has gained no `Stratocracy` dependency. No cycle. No `.Build.cs` changed in this diff.

**Field census.** Printed **10** names, matching the checklist exactly: `BRIDGE=` (the
known false positive — the tail of the string literal
`"STRAT_VENDORED_RULES_IN_BRIDGE="`, deliberately not narrowed away), `PCHUsage =`,
`PrivateDefinitions.Add`, `PrivateDependencyModuleNames.Add`,
`PrivateDependencyModuleNames.AddRange`, `PublicDependencyModuleNames.AddRange`,
`PublicIncludePaths.AddRange`, `ShadowVariableWarningLevel =`,
`bCompileVendoredRulesHere =`, `bUseUnity =`. **No eleventh name.**

**6.** No new `Source/<Module>/` directory. `IMPLEMENT_MODULE` exists in exactly
`StratBridge.cpp:23`, `StratPlay.cpp:14` and `StratUI.cpp:10`, and all three are in
`Stratocracy.uproject`'s `Modules` array alongside `Stratocracy`. `Source/StratRules/`
carries no `IMPLEMENT_MODULE` and is correctly absent from that array.

**7.** `git diff --stat -- Source/Stratocracy` prints nothing.

**8.** 289 raw `/Game/` hits. Judged: every one outside `Source/**/Tests/` and
`ImportStratDataCommandlet.cpp` — the two standing exceptions — is **prose in a comment
explaining the rule**. Filtering those two out leaves 38 hits and I read all 38; not one
is a path literal in gameplay code. No `ConstructorHelpers` lookup exists outside
`Source/Stratocracy/StratocracyCharacter.cpp` (template code, untouched).

**9.** `Source/StratUI/` has **no working-tree change at all** in this wave. Scanned
anyway: the only formatting site in a non-test `StratUI` file that touches two values is
`StratScoreboardWidget.cpp:91`, `LOCTEXT("ValueOverTotal", "{0}/{1}")` — two view-model
fields placed side by side, not a computed number. No sums, ratios, percentages or
conditionals compute a displayed value.

**10. Lane discipline.** `Source/` production changes are all `strat-gameplay-engineer`'s
lane; the four new files under `Source/**/Tests/` are `strat-test-author`'s and contain no
production code (`StratRouteTweenUnitDouble.{h,cpp}` is a test-only `AStratUnitActor`
subclass, declared as such in its own first line). No test code sits outside `Tests/`.
`Content/StratPlay/BP_StratUnit.uasset` is the content lane's and `content.md` records
`strat-editor-builder` as ACTING and WRITING in its own lane under no exception. Each
record file matches its declared writer.

**11.** All four new files carry a why-first block citing GDD sections and acceptance IDs
(§4.9 / §2.11.2 / T-INT-05, and §2.11.2 / T-TURN-09) and recording measurements as
measurements. No `.Build.cs` entry changed.

**12.** `git diff --cached --stat` is empty; HEAD is `283d711`, unmoved.

**Asset.** `Content/StratPlay/BP_StratUnit.uasset` is an LFS pointer change in git's view
(`oid e605800b… → b153ee66…`, `size 26623 → 26925`); the file on disk is 26,925 bytes, so
the working file IS the new asset and the pointer is not stale.

### The engineer's own measurements, re-run rather than accepted

**The deciding `.uasset` measurement reproduces exactly.** `grep -aoF` over raw bytes:

| property | `BP_StratUnit` | `BP_StratGameMode` | `BP_StratGameMode_AiVsAi` |
|---|---|---|---|
| `AiPlaybackStepSeconds` | 0 | 1 | 1 |
| `AiSides` | 0 | 1 | 1 |
| `MoveTweenSeconds` | 1 | 0 | 0 |
| a fabricated property name | 0 | 0 | 0 |

It discriminates per asset and per property in both directions, with the fabricated-name
control absent from all three. The entry's caveat — a name-table hit proves the property is
*serialised*, not what it *holds* — is correct and is honoured: nothing in the entry states
a value.

**F4 and its twin are correctly repaired.** `engine.md:371` and `engine.md:906` now read
"at the C++ FIELD default", with the false wording surviving only inside an inline
`**[CORRECTED 2026-09-03 …]**` stamp, per this file's stamp-don't-delete convention. Both
stamps attribute acting and writing inline. Four sites in total; a tree-wide grep for
"shipped default" in `Tools/architect/state/` returns only stamped, quoted, or
declared-open instances.

**The struck deferral reason: the replacement is sound, and I verified the retraction more
strongly than by re-running the touch probes.** Rather than mutate mtimes in a tree I am
read-only over, I proved it statically, which covers *all* production files instead of one:
`Tools/architect/strat_banner_sweep.py` contains exactly **one** `os.walk(source_dir)`
(in `read_macro_census`, line 698), and `newest_mtime` is updated only inside `if s or c:`
at line 710 — that is, only for a `.cpp` matching `IMPLEMENT_SIMPLE_AUTOMATION_TEST` or
`IMPLEMENT_COMPLEX_AUTOMATION_TEST(_CLASS)`. `Source/StratPlay/StratUnitActor.cpp`
contains **0** such macros; `Source/StratPlay/Tests/StratTourExistenceHoldClauses.cpp`
contains **4**. So a comment edit in any of the four debt files cannot trip REPORT IDENTITY,
and the coordinator's probe result stands. **The originally-struck reason was false and the
strike is correct.**

The three replacement grounds — per-site rulings, one site not the engineer's to rule, and
rebuild-then-run-then-record being a different pass — are each sound, and declaring the
debt with a named discharge condition rather than repairing it under a record-repair brief
is **acceptable**. The defect is in the *list* the discharge condition points at, not in
the decision to defer. See F6.

**The claim-shape sweep.** I reproduced its subject set and its shape independently: 75
non-`Tests/` `.h`/`.cpp`/`.cs` files across `StratPlay`, `StratBridge` and `StratUI`, plus
`engine.md` itself = **76**, reconciling with the entry's figure. Re-running the shape
(comment leaders stripped, all lines joined, {shipped, ships, the real, actual, in
practice} within 120 characters of "default") gives match sites in nine files:
`StratBoardActor.h` 1, `StratCameraPawn.cpp` 1, `StratMatchSubsystem.cpp` 9,
`StratMatchSubsystem.h` 12, `StratPlayerController.cpp` 2, `StratShellGameMode.cpp` 1,
`StratUnitActor.cpp` 6, `StratUnitActor.h` 6, `StratScoreboardHUD.cpp` 1 — 39 matches.

**I audited the rulings, not merely the sweep's existence.** The "correct as written" list
is correctly filed: `StratBoardActor.h`'s `ObjectiveMaterial`/`TargetMaterial` block names
a Blueprint default *as* the shipping state ("`BP_StratBoard`'s default carries
`MI_Overlay_Objective` here"); `StratScoreboardHUD.cpp`'s hit distinguishes "a Blueprint
camera default" from the side the model was rendered for; and `StratPlayerController.cpp`'s
two hits are both inside the `[CORRECTED 2026-08-29]` block that must keep the wording it
quotes. **Nothing in that list is misfiled.** All ~21 debt sites are cited by symbol and
every one of the sixteen symbols I probed resolves in the named file.

### Verification of the coordinator's own writing in `global.md`

Re-run rather than cited from my prior report. Every probe ran against **copies** in the
session scratchpad; the repository was not modified.

- **Control** (unmodified copy of all six `state/` files): `SWEEP CLEAN`, **exit 0**.
- **Mutant B — stale live citation.** `THE LIVE FIGURE'S REPORT IS reportCreatedOn
  2026.09.03-19.47.45` changed to `2026.09.02-23.16.13`: **exit 1**, one
  `[**REPORT PROVENANCE**] global.md:25` finding naming both stamps. **The guard is armed.**
- **Mutant C — wrong live count.** `386/386` → `387/387` at `global.md:19`: **exit 1**, two
  `[**SUITE COUNT AGREEMENT**]` findings, one against the macro census and one against the
  report's own `succeeded`. **The guard is armed.**
- **Mutant D — the inertness property, re-confirmed.** Mutant B's stale citation *plus* an
  ordinary `**[STAMPED 2026-09-03 …]**` bracket inserted one line below it: **exit 0,
  `SWEEP CLEAN`, zero REPORT PROVENANCE findings.** The subtle property the coordinator
  introduced is real and reproduces on demand: a stamped bracket inside
  `_PROVENANCE_CITATION_WINDOW` (read from the source at line 313 as 400) silently returns
  check 5(a) to inert on the banner's own certifying citation.
- **The current layout keeps it armed with margin.** Measured on the real file: **1,086
  characters** from the end of the live citation to the next `[STAMPED` marker, and there is
  no `[STAMPED` marker anywhere above it. 1,086 > 400.

The banner's placement note at `global.md:34-41` states this property accurately and its
reasoning is correct. **No finding in `global.md`.**

### Brief facts, all re-measured

| claim | measured |
|---|---|
| report 386 / 0 / 0 at `reportCreatedOn 2026.09.03-19.47.45` | confirmed; `tests` array length 386 |
| BOM — plain `json.load` raises | confirmed: `JSONDecodeError: Expecting value: line 1 column 1` |
| macro census 386 | confirmed, counted independently |
| no `Source/` file newer than the report | confirmed (15:45:58 vs 15:47:45) |
| sweep exits 0, `SWEEP CLEAN` | confirmed, **run without a pipe**, `EXIT=0` |
| 26 `git status --porcelain` entries, nothing staged | confirmed |

### Standing open items

- **The `AiSides` value the content lane owes** — still declared open at `engine.md:96-105`
  and cross-referenced from `tests.md:76-83`. **Still open, not quietly closed** — but see
  F5 and F6, which are about *how* it is declared.
- **`Source/StratPlay/Tests/StratAiMatchClauses.cpp:380`** — `// ---- THE GUARDED PATH: the
  shipped defaults ----`. Unchanged, still declared open by `tests.md`, still ruled
  acceptable here: it is a comment, it reaches no report, and the file is outside this
  wave's diff.
- **The `const` accessor discharge condition for
  `AUnitBuiltDuringTheHandoverIsHiddenUntilItsStep`** — declared at `engine.md:341`,
  `tests.md:137` and `global.md:57`, consistently and with the same closing condition at all
  three. Correctly declared.
- **The two undischarged mutants on
  `GATE-AITURN.ReelRecordsEveryAcceptedCommandInOrder`** — `tests.md:282-296` still reads
  *"Neither was built and neither may be cited as discharged; the engineer runs them."* I
  grepped `engine.md` and `global.md` for any claim to the contrary and found none. **Still
  correctly declared open.** The distinct mutants A and B in the earlier route-arm entry
  (`tests.md:510-519`) *are* discharged, built and run; the record does not confuse the two
  sets.

## Findings

**F5 — a citation naming a symbol that does not exist, on the one bullet whose stated
purpose is to be greppable. Owner: `strat-gameplay-engineer`.**

`Tools/architect/state/engine.md:98`:

> **One of them is explicitly OPEN and is not mine to rule** -- `StratMatchSubsystem.h`'s
> `StratAiDifficulty` block, whose consequence depends on the authored `AiSides` value only
> the content lane can read

and again at `engine.md:109`:

> `StratMatchSubsystem.h`'s `StratAiDifficulty` block and this record's own 2026-08-30 copy
> of it say ...

**`StratAiDifficulty` does not exist anywhere in the tree.** Grepping it across `Source/`,
`Config/` and `Content/` returns **zero**; the only two occurrences in the repository are
the two citations above. The block actually meant is the doc comment on the
`EStratDifficulty Difficulty` property at `Source/StratPlay/StratMatchSubsystem.h:389-395`,
whose text the entry quotes correctly — `StratMatchSubsystem.h:391` reads:

> `is the honest C++ default;` `AiSides` `defaults empty, so on the shipped hot seat the`

This is load-bearing rather than cosmetic. That bullet is the entry's hand-off to the
content lane and it closes with *"Recorded here so it is greppable rather than lost."* It
is **not** greppable: a reader who greps the cited symbol in `Source/` gets zero hits, and
a fabricated name greps exactly like a defined one until you look. The neighbouring
`Difficulty` property is real, so the reader cannot infer the miss from context either.

**F6 — the sweep's shape misses two production sites of the very claim the entry declares
open, and the debt list's symbol enumeration walks past both, so the DISCHARGED WHEN
condition can be satisfied while they survive. Owner: `strat-gameplay-engineer`.**

The sweep's shape requires the literal word "default" within 120 characters of a {shipped,
ships, the real, actual, in practice} token. The same claim phrased without that word is
invisible to it. I ran a widened probe over the identical 75-file set — assertions about
the shipped or live state with **no** "default" within 120 characters — and two of its
sixteen hits are exactly this defect class, both on `AiSides`, the property the entry
itself declares unmeasured:

`Source/StratPlay/StratMatchSubsystem.h:652-653`:

>  ` *   - ` `AiSides` ` EMPTY -> ` `INDEX_NONE` `. A hot seat has no player-versus-opponent asymmetry`
>  ` *     for a handicap to express. This is the shipped configuration.`

`Source/StratPlay/StratMatchSubsystem.cpp:2750`:

>  `// ARM ONE -- the shipped hot seat. See the declaration: Sec 2.7 calls this`

Both assert that the shipped configuration has `AiSides` empty. `AiSides` is serialised in
**both** GameMode Blueprints — my own name-table measurement above — so both sentences make
the identical unmeasured claim as `StratMatchSubsystem.h:391`, and neither appears in the
debt bullet's enumeration. That bullet cites the `.h` sites as `AdvanceAiPlaybackOneStep`,
`BeginAiPlayback` x2, `PreParkPicturesForTour`, `EndAiPlaybackTour`, `LastArmedSlideSeconds`
and `AiPlaybackStepSeconds`, and the `.cpp` sites as `ApplyView`, `RunAiTurnsNow`,
`WillAiPlaybackRun`, `BeginAiPlayback`, `ArmNextPlaybackStep`, `AdvanceAiPlaybackOneStep` x2
and `EndAiPlaybackTour`. **`StratHandicappedSide` is in neither list**, and it is the symbol
both surviving sites sit in or beside.

Why this gates rather than sits in Observations: the entry's discharge condition reads
*"DISCHARGED WHEN the next pass that compiles this module rewrites those blocks."* A pass
that faithfully walks the cited symbols satisfies that condition, stamps the debt closed,
and leaves two instances of the same claim standing unstamped in production prose — in the
very file the open question is about. This is the project's own recorded "a correction that
only adds leaves both claims live" and "an invariant stated on one half of a pair" shape,
arriving on the hand-off itself. Combined with F5: the moment the content lane answers what
`AiSides` holds, the record points at a symbol that does not exist and two of the three
affected sites are not findable from it.

The fix is small and entirely within one lane, and **no production byte need change to
clear this gate** — the debt may stay deferred. Correct the two citations to the
`EStratDifficulty Difficulty` block (or to any symbol that greps), and add
`StratHandicappedSide` — both its declaration comment at `StratMatchSubsystem.h:652-653`
and its definition comment at `StratMatchSubsystem.cpp:2750` — to the declared-open
`AiSides` set.

## Observations

Non-gating.

- **The sweep's shape is narrower than the defect class it was built for, and that is worth
  recording even after F6 is repaired.** It is pinned to the literal token "default". My
  widened probe returned 16 hits with no "default" nearby; 14 are about the shipped
  *scenario* or shipped *data*, a different and correct subject, and 2 are F6. A future
  sweep should either widen the shape or record in the entry that the shape cannot see a
  shipped-state claim phrased without that one word — the "guard goes inert on its own
  subject" shape, arriving here as a guard that was never quite wide enough rather than one
  that went quiet.
- **The "correct as written" list is incomplete relative to its own stated purpose.** It
  says it exists *"so the next sweep does not re-litigate them"*, but three files carrying
  sweep matches are named nowhere in the entry: `StratCameraPawn.cpp` (the token sits beside
  the identifier `bAddDefaultMovementBindings`), `StratShellGameMode.cpp` (beside
  `DefaultPawnClass = APawn::StaticClass();`) and `StratScoreboardHUD.cpp`. I read all three
  and **none is a defect** — the entry's claim that nothing else in the sweep is a defect is
  TRUE — but a later sweep will re-litigate them, which is the cost the list exists to avoid.
- **The dispatch brief's description of the untracked set was off while its count was
  right.** It said "the two new test files"; there are **four**:
  `Source/StratBridge/Tests/StratRosterDeltaClauses.cpp`,
  `Source/StratPlay/Tests/StratRouteTweenUnitDouble.cpp`,
  `Source/StratPlay/Tests/StratRouteTweenUnitDouble.h` and
  `Source/StratPlay/Tests/StratTourExistenceHoldClauses.cpp`. The total of 26 is correct
  because the other two fell under "the wave". Recorded because a count that is right for the
  wrong reason is the shape this project already tracks as "the brief is not ground truth";
  nothing else follows from it here.
- **The REPORT PROVENANCE margin is 1,086 characters and is maintained by convention alone.**
  Nothing mechanical stops the next writer appending a stamped block into that gap, and
  mutant D shows the guard goes *silent* — not noisy — when they do. The banner warns about
  this in prose at `global.md:34-41`, which is the best available answer, but it is a prose
  guard on a mechanical property. A check asserting "no `[STAMPED` marker within 400
  characters of the live citation" would close it.
- **`.agents/ue-project-context.md` shows no drift.** Its module table (lines 38-40) matches
  the graph I derived row for row, including the conditional monolithic link edge and its
  reasoning, and its vendored-bytes row correctly instructs the reader to read the manifest
  rather than the prose. No correction is owed to it this pass.
- **Three gate reports are now persisted and untracked** under
  `Tools/architect/gate_reports/`. Committing them is the user's call.

VERDICT: BLOCK
