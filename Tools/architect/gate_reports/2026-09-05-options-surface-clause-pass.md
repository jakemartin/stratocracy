# Gate: options-surface clause pass — strat-integration-reviewer

## Scope

Working-tree diff of `E:\MultiAgent\Stratocracy` on `master` against base `374398a`
(`The volume screen exists and is reachable from the title menu…`), which is `HEAD`. Nothing is
committed on top of it and nothing is staged.

**15 files.** 6 modified — `Source/StratUI/StratOptionsWidget.cpp`,
`Source/StratUI/StratOptionsWidget.h`, `Source/StratUI/Tests/StratOptionsWidgetDouble.h`,
`Tools/architect/state/engine.md`, `Tools/architect/state/global.md`,
`Tools/architect/state/tests.md` (564 insertions, 16 deletions). 9 untracked — 7 clause `.cpp`
files and 2 test-double headers under `Source/StratPlay/Tests/` and `Source/StratUI/Tests/`.

**`Content/` is not in this diff at all.** The `WBP_Options` build, the `WBP_TitleMenu` reparent,
the `WBP_CommandBar` button and the three `OptionsWidgetClass` defaults are all inside `374398a`
and were cleared by the previous gate. The asset confirmations the dispatch asked for are
recorded under Observations as re-derivations, not as gated subjects.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing |
| 2 | No `strat::` call outside `StratBridge`/`StratRules` | pass — grep for the lowercase-initial call pattern across `Source/` returned zero hits outside the two permitted directories; no hits to judge |
| 3 | No vendored header in front of UHT | pass — the only header including `Ui.h`/`Data.h`/`Save.h`/`Scenario.h`/`Replay.h` is `Source/StratBridge/StratBridge.h:53-58`, which declares no `UCLASS`/`USTRUCT`/`UENUM` at all. The nine new/changed headers declare reflected types and include none of the six |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` | pass — the single `TUniquePtr<FStratBridge>` hit in the tree is prose at `Source/StratUI/StratViewModel.h:47` recording the `C4150` measurement. No new class in this diff holds a bridge |
| 5 | Module arrows unchanged and correct | pass — derived, not trusted. The extraction printed **16 rows**, including the conditional `StratBridge PrivateDependencyModuleNames StratRules`; `StratBridge.Build.cs:133-137` confirms it sits inside `if (!bCompileVendoredRulesHere)` from `Target.LinkType != TargetLinkType.Monolithic`, so it is the monolithic-only link edge and is correct. Field census printed **10** names, matching the recorded set exactly (`BRIDGE=` present as the known string-literal false positive). No `.Build.cs` is in this diff |
| 6 | New modules registered | pass, vacuously — `Source/` holds the same five directories; `Stratocracy.uproject` lists the same four; `StratRules` correctly absent (no `IMPLEMENT_MODULE`) |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` literal in gameplay C++ | pass — every `"/Game/` hit in `Source/` is inside a `Tests/` directory or the import commandlet. The three new asset paths in `Source/StratPlay/Tests/StratShippedOptionsWidgetClassParity.cpp` are the standing fixture exception |
| 9 | No widget-side arithmetic | pass — every value drawn in `SyncBoundWidgetsToModel` (`StratOptionsWidget.cpp:225-253`) is exactly one `Model` field. The one arithmetic expression, at `:46`, is inside the free model builder, not a draw. See Observations |
| 10 | Lane discipline | pass — all 9 new files are under `Source/<Module>/Tests/`; both new headers are doubles, not production code. The two `Source/StratUI/` modifications are the engineer's lane and are comment-only. Record files carry in-lane attribution |
| 11 | Prose blocks present | pass — all 7 new clause files and both doubles open with a why-first block naming the acceptance ID, citing the steward's ruling by quote, and stating what the file does *not* pin. No new `.Build.cs` entries |
| 12 | Nothing staged or committed | pass — `git status --porcelain` shows no staged column; `git diff --cached --stat` empty; `HEAD` is `374398a`. The `-SCCProvider=None` runs left nothing auto-added |
| — | **Record accuracy (`global.md`)** | **BLOCK** — three findings below |

### Measurements re-derived rather than accepted

- `Saved/AutomationReport/index.json` read with `utf-8-sig`: `succeeded 435`, `failed 0`,
  `notRun 0`, `succeededWithWarnings 0`; **435** entries, every state `Success`;
  `reportCreatedOn 2026.09.05-23.18.02`. Prefix counts: `GATE-TITLEMENU` **42**,
  `GATE-AUDIO-SETTINGS` **10**, `GATE-AUDIO.` **24**. Every figure in the dispatch confirmed.
- `IMPLEMENT_SIMPLE_AUTOMATION_TEST` across the 7 new clause files: **13**. 422 + 13 = 435.
- `strat_banner_sweep.py`: `SWEEP CLEAN`, `EXIT=0`, macro census 435, report identity matched.
- The three reported findings were checked against the tree, not accepted:
  - **(1) is stated correctly.** `Source/StratPlay/StratShellSubsystem.cpp:240` reads
    `Option.Label = FText::FromString(TEXT("Options"));`, the same literal the clause compares.
  - **(2) is stated correctly.** `IsRoutePermitted`'s `case EStratShellRoute::Options:` arm
    returns `true` with no branch on any `FStratShellFacts` field, so the enabled-copy assertion
    compares `true` against `true`. Both the clause header and `global.md` say so in terms.
  - **(3) is stated correctly** in `tests.md` as an instrument caveat, with the separating check
    (compare the report's own `reportCreatedOn` against the run) named rather than implied.
- **The retracted premise is retracted correctly, and the replacement is right.** UE 5.8
  `Engine/Source/Runtime/UMG/Private/Components/Slider.cpp` was opened directly: `USlider::SetValue`
  calls `MySlider->SetValue` inside `if (MySlider.IsValid())`, then calls `HandleOnValueChanged`
  from a separate `if (Value != InValue)` block OUTSIDE it — exactly as `StratOptionsWidget.h`
  now states, line citation included and correct. `tests.md:76-86` carries the old claim under an
  explicit `[SUPERSEDED 2026-09-05 …]` stamp rather than deleted, which is this record's own
  convention — and is the contrast that makes BLOCK 1 below a defect rather than a convention.
- **The `.h`/`.cpp` change really is prose-only.** `git diff -U0` on both files, stripped of
  added and removed lines that are comments or blank, returned **zero** lines.

## Findings

### BLOCK 1 — the banner states a superseded suite measurement in the present tense, in the one file that may state a suite count

`Tools/architect/state/global.md:30-32`:

> This seat read 422 entries directly out of `Saved/AutomationReport/index.json` with `utf-8-sig`
> and counted 24 `GATE-AUDIO.` names and 6 `GATE-AUDIO-SETTINGS.` names, confirmed every entry
> `Success` and zero with warnings, rather than accepting any lane's figure. 389 + 19 + 5 + 9 = 422.

Three false claims, measured against the file that sentence names:

- **`422` entries** — that path holds **435**.
- **`6` `GATE-AUDIO-SETTINGS.` names** — measured **10**.
- **`389 + 19 + 5 + 9 = 422`** — contradicted nine lines above by the same edit, which now reads
  `THE COUNT MOVED 389 -> 435: +46 added … in four steps` at `:23`.

The paragraph carries no `[STAMPED …]`, no `CORRECTION,` and no past-tense marker, and it sits
between the live figure at `:21` and the live report identity at `:33` — both of which this same
edit updated. It is not history left deliberately; it is the un-updated remainder of the edit that
moved everything around it. `global.md` is the only file permitted to state a suite count, so a
reader has nowhere else to check it against.

**`SWEEP CLEAN` does not cover this and cannot.** `strat_banner_sweep.py` collects `N/N`-shaped
suite claims — it reported `suite claims found: 63 (1 live, 62 stamped)` — and every false figure
above is a bare integer. The sweep is blind to them by construction; its exit 0 is true and
insufficient, which is the same shape as the targeted-check finding this milestone already carries.

**Owner: the `coordinator`**, sole writer of `global.md` with the steward.

### BLOCK 2 — the paragraph written to replace a repeatedly-wrong claim is itself now wrong, at its own next suite movement

`Tools/architect/state/global.md:52-57`:

> What is durable is stated instead: the count reached 422 from 389 in
> three additions, +19 then +5 then +9, and those additions are the ONLY movement in the
> figure across the whole milestone.

Both halves are now false: the count reached **435** in **four** additions, and the `+13` this
same edit adds at `:23` is movement the sentence says does not exist. The milestone is not closed,
so "across the whole milestone" is not a closed-scope claim either.

This is the repair-is-the-next-finding pattern, and the paragraph diagnoses itself two lines
earlier: it exists because *"An earlier draft of this banner did keep that list, and it had to be
corrected on FOUR CONSECUTIVE GATE ROUNDS as it grew — a running history of identities is a claim
that acquires a new way to be wrong every time a suite runs."* The replacement was chosen for
being durable and went stale at the first suite movement after it was written, for the identical
reason. What is durable here is the SHAPE — additions only, no removals, no state changes — not a
recomputed total.

**Owner: the `coordinator`.**

### BLOCK 3 — the banner narrates its own gating, in a sentence this record has already struck twice

`Tools/architect/state/global.md:227`:

> WHAT IS STILL OWED. NO GATE HAS RUN ON THE OPTIONS SURFACE.

True when written and false the moment this verdict lands — so the commit carrying it would ship a
false sentence about itself. This is not novel in this file: `global.md:2131` and `global.md:2191`
already stamp earlier instances, `STRUCK: NO GATE HAS RUN ON THIS TREE` and `STRUCK: NO GATE HAS
RUN ON THIS TREE SINCE THE MERGE`. The immediately preceding commit `369513f` records the same
class in its own message: *"the gate falsified a sentence in the commit it was gating — 'no gate
has returned PASS on this tree' was true when written and untrue the moment the verdict landed."*

Note that the edit introducing it was itself a repair. The prior text read *"NO CLAUSE COVERS ANY
OF THE OPTIONS SURFACE and NO GATE HAS RUN ON IT."* The first half was correctly discharged and
deleted; the second was reworded and kept. The remedy is deletion, not a fourth rewording — a
record cannot truthfully describe its own gating state from inside the artifact being gated. The
genuinely owed items in that same block are durable and should stay: no human has seen the screen,
a commandlet has no pixels, the in-match panel takes no input mode, and the `ApplyOptionsRow` seam
is recorded as needed and not requested.

**Owner: the `coordinator`.**

## Observations

Non-gating.

- **The three items flagged for adversarial attention all hold.**
  - **The reparent is intact, confirmed at the byte level.** `Content/UI/WBP_TitleMenu.uasset`
    contains `StratShellMenuWidget` (6 occurrences), `StratPlay` (5), `StratUI` (**0**), and
    `Btn_0`, `Btn_1`, `Btn_2`, `Btn_3` (4 each), plus `OptionsButton` and `OptionsLabel`. The four
    original buttons survived and the baked module in `/Script/StratPlay.StratShellMenuWidget` is
    right: `Source/StratPlay/StratShellMenuWidget.h` and `.cpp` exist and `StratPlay` is a
    registered module in `Stratocracy.uproject`.
  - **A widget parent in `StratPlay` is not merely permitted, it is forced.** The derivation, not
    the citation: `StratPlay` publicly depends on `StratUI`, and `StratUI` publicly depends on
    `StratBridge` and names no `StratPlay` module in any of its three fields. A `StratUI` class
    naming `UStratShellSubsystem` would need a `StratUI -> StratPlay` edge against the existing
    `StratPlay -> StratUI` edge — a cycle. `StratCommandBarWidget.h:122-124` states the converse
    case for its own class in the same terms, and every `StratPlay` mention inside
    `Source/StratUI/` production code is in a comment.
  - **On "does any asset still ensure": the clean suite is sufficient for two of the three widgets
    and not for the third, and the difference is worth knowing.** `WBP_Options` is reached
    transitively — `BP_StratShellGameMode.uasset` and `BP_StratGameMode.uasset` each carry 2
    `WBP_Options` references, and the new parity clause loads all three GameMode CDOs, which
    compiles the referenced widget Blueprints; an ensure there is an automation error and would
    have reddened the run. `WBP_TitleMenu_C` is loaded directly by
    `StratShellHudCallSiteClauses.cpp`. **`WBP_CommandBar` is loaded by no clause** — no GameMode
    default names it (0 occurrences in both packages) and
    `StratCommandBarOptionsBindClauses.cpp:52` disclaims it explicitly. So 435/435 says nothing
    about that asset either way. A declared limit, not a defect, recorded so the green is not read
    as broader than it is.
- **The one arithmetic expression in the options surface is placed correctly, and the boundary is
  worth naming once.** `StratBuildAudioOptionsModel` lives in `Source/StratUI/` and does
  `FMath::RoundToInt(Volume * 100.0f)` at `StratOptionsWidget.cpp:46`. Check 9 governs what a
  widget *draws*, and every drawn value is exactly one `Model` field, so this passes: it is a free
  function producing the model, not a widget consuming it, and the file header at `:3-8` argues the
  placement (keeping the expression out of a Blueprint graph). Recorded so a future gate grepping
  for `* 100` in `StratUI` finds the judgement already made rather than making it again.
- **The clause prose in this pass is unusually good at declaring its own limits**, and two
  instances are the reason findings (1) and (2) in the dispatch were checkable at all:
  `StratTitleMenuOptionsRowClauses.cpp`'s mutant block corrects two contradicted predictions in
  place instead of deleting them, and marks the third `(SURVIVES BY CONSTRUCTION — DERIVED FROM
  THE SOURCE, NOT RUN IN THE BATTERY)` so no reader takes it for a live measurement. Nothing here
  needs changing; it is the standard the record findings above are held to.
- **Line endings.** `git diff` warns `LF will be replaced by CRLF` on five files. Expected under
  `core.autocrlf=true` with `.gitattributes`. Not a finding.
- **`.agents/ue-project-context.md` was read and needed no correction.** Its module table, the
  conditional `StratBridge -> StratRules` reasoning, and the `StratRules`-absent-from-`Modules`
  note all match the tree as derived above.

VERDICT: BLOCK
