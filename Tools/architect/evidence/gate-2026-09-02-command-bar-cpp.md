# Gate: W9 command bar, C++ half - strat-integration-reviewer

Run 2026-09-02 against the UNCOMMITTED working tree at `E:/MultiAgent/Stratocracy`,
branch `master`, HEAD `fcf64d3`. Read-only; nothing in this tree was modified by this
gate except this report file.

## Scope

Working-tree diff against `fcf64d3`: 12 modified tracked files (1525 insertions,
38 deletions) plus 8 untracked new files, 20 paths total. Nothing staged, nothing
committed. No `.Build.cs` and no asset changed. The asset half of the feature is
deferred and out of scope for this gate.

Modified: `Source/StratPlay/StratMatchSubsystem.cpp`, `StratPlayerController.{h,cpp}`;
`Source/StratUI/StratScoreboardHUD.{h,cpp}`, `StratViewModel.{h,cpp}`,
`Tests/StratProductionMenuHostProbe.{h,cpp}`; `Tools/architect/state/{engine,tests,global}.md`.
New: `Source/StratPlay/StratBuildAffordance.{h,cpp}`,
`Source/StratPlay/Tests/StratBuildAffordanceClauses.cpp`,
`Source/StratUI/StratCommandBarWidget.{h,cpp}`,
`Source/StratUI/Tests/StratCommandBarClauses.cpp`,
`Source/StratUI/Tests/StratProductionMenuOrderDouble.{h,cpp}`.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched (`Source/StratRules`, `Data`) | pass |
| 2 | No `strat::` call outside `StratBridge/` and `StratRules/` | pass |
| 3 | No vendored header in front of UHT | pass |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass |
| 5 | Module arrows unchanged and correct | pass |
| 6 | New modules registered (none added) | pass |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` path literal in gameplay C++ | pass |
| 9 | No widget-side arithmetic | pass |
| 10 | Lane discipline | pass |
| 11 | Prose blocks present | pass |
| 12 | Nothing staged or committed | pass |

### Evidence, per row

1. `git diff --stat -- Source/StratRules Data` printed nothing, and
   `git ls-files --others --exclude-standard` lists no path under either. Compared through
   git's own index rather than a worktree text diff, so the CRLF/LF false positive cannot
   arise. `Source/StratRules/StratRules.manifest.json` and `Data/StratData.manifest.json`
   are not in the change set, so the recorded rulesCommit / dataCommit are unmoved.
2. The lowercase-initial call grep over `Source/`, excluding `Source/StratBridge/` and
   `Source/StratRules/`, returned ZERO lines. The unrestricted `strat::` scan over the same
   exclusion returns only prose comments plus the pre-existing
   `static_cast<int32>(strat::UnitType::...)` TYPE references in
   `Source/Stratocracy/StratData/Tests/StratDataParity.cpp` - types, not calls, and
   unchanged by this pass. The new `Source/StratPlay/StratBuildAffordance.cpp` names
   `strat::buildUiSnapshot` in a comment only.
3. The vendored-header include grep over all `*.h` in `Source/` hits only
   `Source/StratBridge/StratBridge.h` and files inside `Source/StratRules/`. Counted
   individually, each of the six UHT-parsed headers in or touched by this change set
   returns 0: `StratCommandBarWidget.h` (new UCLASS `UStratCommandBarWidget`),
   `StratViewModel.h` (new USTRUCT `FStratCommandBarView`), `StratScoreboardHUD.h`,
   `StratPlayerController.h`, `Tests/StratProductionMenuOrderDouble.h`,
   `Tests/StratProductionMenuHostProbe.h`. `StratViewModel.h` includes only
   `CoreMinimal.h` and its own generated header. `StratScoreboardHUD.h` forward declares
   `class UStratCommandBarWidget;` and the include of `StratCommandBarWidget.h` is in the
   `.cpp`. `StratPlayerController.h` includes the new `StratBuildAffordance.h`, which
   itself includes only `CoreMinimal.h` and forward declares `struct FStratViewModel`, so
   no vendored header reaches UHT through it.
4. No new `FStratBridge` member landed. The two existing owners,
   `UStratMatchSubsystem::Bridge` and `AStratScoreboardHUD::OwnedBridge`, are both
   `TPimplPtr<FStratBridge>` and neither declaration is in the diff.
5. The row extraction over every `Source/*/*.Build.cs` printed 16 rows across all five
   modules - none missing - including the conditional
   `StratBridge PrivateDependencyModuleNames StratRules` row, and matches the expected
   block exactly. `StratRules` to Core. `Stratocracy` to Core, CoreUObject, Engine,
   InputCore, EnhancedInput, AIModule, NavigationSystem, StateTreeModule,
   GameplayStateTreeModule, Niagara, UMG, Slate, Json, StratRules. `StratBridge` to Core,
   CoreUObject, Engine, Stratocracy, with PublicIncludePaths StratBridge + StratRules and
   the guarded private StratRules link edge. `StratUI` to Core, CoreUObject, Engine,
   StratBridge plus private UMG, Slate, SlateCore. `StratPlay` to Core, CoreUObject,
   Engine, StratUI plus private StratBridge, UMG, EnhancedInput. The field census printed
   TEN names: the three graph fields, PCHUsage, bUseUnity, ShadowVariableWarningLevel,
   PrivateDependencyModuleNames.Add, PrivateDefinitions.Add, bCompileVendoredRulesHere, and
   the known BRIDGE= false positive from inside the STRAT_VENDORED_RULES_IN_BRIDGE= string
   literal. No eleventh name. The `.Build.cs` diffstat printed nothing: no build file
   changed, so the engineer's claim that no new dependency was needed holds. No new cycle -
   the include scan over `Source/StratUI/` finds no include of any `StratPlay` header, and
   `StratDecorateCommandBar` takes a bool and an FIntPoint rather than an
   `FStratBuildAffordance` precisely to keep that arrow from reversing.
6. No new `Source/<Module>/` directory. The four new production files sit inside the
   already-registered `StratPlay` and `StratUI` modules.
7. The `Source/Stratocracy` diffstat printed nothing, and no untracked path is under it.
8. The `/Game/` grep over `Source/` outside `/Tests/` returns only prose comments plus the
   pre-existing `ImportStratDataCommandlet.cpp` package-name construction, which is the
   standing commandlet exception. The new HUD slot is
   `UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|CommandBar")` on
   `TSubclassOf<UStratCommandBarWidget> CommandBarWidgetClass` with no initializer and no
   path literal; `AStratScoreboardHUD::CreateCommandBarWidget` logs and returns false when
   it is unset rather than falling back to a lookup.
9. `UStratCommandBarWidget` has exactly one non-inline body,
   `UStratCommandBarWidget::PushCommandBar`, which assigns `Model` and fires the
   BlueprintImplementableEvent `OnCommandBarRefreshed`. No arithmetic, no branch, no
   comparison. Every field a widget would draw is a field of `FStratCommandBarView`. On the
   C++ side `StratDecorateCommandBar` folds bEndTurnSuggested as NOT
   `Model.Match.bHasResult` AND NOT `Model.Guidance.bEndTurnGated` AND
   `UStratViewModelLibrary::CountViewingSideUnitsAbleToAct(Model)` equal to zero - a single
   call to the existing selector, verified present as a declaration in
   `StratViewModelLibrary.h` and a definition in `StratViewModelLibrary.cpp`. There is no
   second walk of `Model.Units` anywhere in the function: its only container traversal is
   the `Model.Factories.FindByPredicate` hex lookup for the BUILD control. No count is
   stored on the model; only the boolean survives.
10. Production C++ landed in `Source/StratPlay/` and `Source/StratUI/` outside `Tests/`;
    test code is confined to `Source/StratPlay/Tests/` and `Source/StratUI/Tests/`,
    including the new `StratProductionMenuOrderDouble` test double. No `Content/` change in
    a source-only pass. Record writes: `engine.md` carries a strat-gameplay-engineer
    (ACTING and WRITING; IN LANE, base commit fcf64d3, UNCOMMITTED) entry, `tests.md` a
    strat-test-author (ACTING and WRITING; IN LANE) entry, `global.md` is the
    coordinator's. Each lane wrote only its own file.
11. Every new file opens with a why-first block citing GDD sections 2.11.2, 2.11.5 and
    2.11.6-B and acceptance IDs T-UI-03 and T-INT-05, recording measurements as
    measurements - including `FStratBuildAffordance::Reset`, whose declaration states its
    own no-shipping-caller debt against the tree at fcf64d3 rather than implying a caller.
12. `git status --porcelain` shows 12 modified and 8 untracked entries and no index-side
    letter; `git diff --cached --stat` printed nothing; HEAD is fcf64d3.

### The two dispatch-named constraints, checked directly

T-INT-05, the screen rebuildable from the view model alone: HELD.
`FStratBuildAffordance` holds `FocusedHex`, `bHasFocusedHex`, `LastObservedTurn` and
`LastObservedViewingSide` - an input intent and two observation clocks, no drawable bit.
`FStratBuildAffordance::DecorateViewModel` passes only `bHasFocusedHex` and `FocusedHex`
to `StratDecorateCommandBar`, which resolves the factory against `Model.Factories` and
`Model.ViewingSide` itself and writes all five fields of `Model.CommandBar`
unconditionally. `UStratMatchSubsystem::ApplyView` feeds `Model.CommandBar` to
`AStratScoreboardHUD::PushCommandBar`, and the widget reads nothing else. No bit a widget
draws is readable only off the actor.

Ordering: inside `AStratPlayerController`, `GuidedOpening.DecorateViewModel(Model)` runs
first, then `BuildAffordance.Observe(Model)`, then
`BuildAffordance.DecorateViewModel(Model)` - so the guidance block exists before
`StratDecorateCommandBar` reads `Model.Guidance.bEndTurnGated` and
`Model.Guidance.EndTurnGateHover`, and the turn/seat/concluded clears are applied before
the bar is drawn. Cited by symbol; the surrounding line numbers are incidental and will
move.

## Findings

None.

## Observations

Non-gating.

- `FStratBuildAffordance::Reset()` has no caller anywhere in `Source/`, including
  `Tests/`. Its own declaration says so and names the condition that discharges it - a
  load or reseed path that resets it alongside `FStratSelectionMachine::Reset()`, which is
  itself uncalled. The debt is declared rather than hidden, so this is a record and not a
  finding. Owner when it comes due: strat-gameplay-engineer.
- The BUILD control and the END TURN highlight are correct in C++ and invisible on screen
  until the content lane authors a WBP deriving from `UStratCommandBarWidget` and sets
  `CommandBarWidgetClass` on the HUD Blueprint default. Until then
  `AStratScoreboardHUD::BeginPlay` logs that no command bar was requested and the feature
  is absent from the screen while every C++ clause is green. That is the same gap this
  project already recorded at fcf64d3 - every instrument reporting correctly assigned and
  not one of them able to see a pixel. The asset half is deferred by design here, but no
  gate in this tree can close it.
- The build, suite and guard figures in the dispatch brief were not re-derived by this
  gate, which does not build and does not run the suite. Those numbers stand or fall on
  strat-test-author's own report, not on this verdict.

VERDICT: PASS
