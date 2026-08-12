# Architect state

_Last run 2026-08-11 19:30 UTC._

## BUILT

- `Source/StratUI/StratScoreboardHUD.h` — applied (iteration 1, step `host_h`, live)
- `Source/StratUI/StratScoreboardHUD.cpp` — applied (iteration 1, step `host_cpp`, live)
- `FStratBridge::Reachable` — landed at `e0cc53d` with zero tests; its five clauses are now
  covered (`StratBridgeQueryParity.cpp`, T-UI-02, phase 1, 2026-08-12). Debt discharged.

## DECISIONS

- iteration 1: selected `scoreboard_host` at score 35; runner-up production_widget was blocked on buildlist_query

## NEXT

- `presentation_statelessness` — Presentation statelessness pass (rebuild widgets from the view model) (actionable)
- `production_widget` — Production menu widget (§2.11.5) (blocked on buildlist_query)
- `bridge_event_list` — Bridge ordered event list (§4.9 'command in / events out') (actionable)
- `buildlist_query` — Buildlist query on the Ui.h contract (actionable, excluded: shape unstated in the GDD by explicit decision, and the file is vendored certified bytes in another repo -- T-INT-01 hash-matches it)
- **Hot-seat milestone, out of scope**: production menu (§2.11.5), guided opening (§2.11.6), info panel, toasts, save-slot UI, AI opponent, move-undo.
- The **owned-bridge-path leak on `AStratScoreboardHUD` is not pinned.** Proving it needs an
  allocation counter behind `FStratBridge` — offered by `strat-gameplay-engineer` during phase 2
  and **declined as new production surface beyond phase 2's exit criterion**. The tests prove the
  *adopted* bridge is not freed (`T-UI-03.EndPlayLeavesAnAdoptedBridgeUsable`); they do not prove
  the owned one is. Stated honestly in `StratScoreboardHUDSeam.cpp:31-37`. Out of scope for the
  hot-seat milestone.

## Hot-seat milestone

### Phase 0 — row order proven, tests green

- **Completed:** 2026-08-12
- **Exit criterion:** "row order proven; 18/18 tests green; git status clean"
- **Met.** Units and terrain close the criterion by different mechanisms, and
  that asymmetry is deliberate, not a gap:
  - **Units — proven directly.**
    `Stratocracy.StratBridge.GATE-BRIDGE-DEFS.MappedDefsMatchLoaderOrder`
    (`Source/StratBridge/Tests/StratBridgeParity.cpp:166-231`) compares
    `FStratBridge::UnitDefs()[i]` (built from the live `.uasset` via
    `UDataTable::GetRowNames()`, `StratBridge.cpp:53-76`) against
    `strat::loadUnits` over the same vendored `units.csv`, positionally, field
    by field. This exists *because* `SaveCommandKind::Build` carries a raw
    `defIndex` in its `unitId` field (`Save.h:64`) that `applyCommand` uses as
    a direct array index with no name lookup (`Replay.good.cpp:483-487`,
    `t.units->size()` bounds check) — an order divergence there resolves the
    same log to a different unit type, silently. Passed 18/18 at HEAD
    `0a7296e` (report `Saved/AutomationReport/index.json`, reportCreatedOn
    2026.08.12-13.55.04).
  - **Terrain — ruled not load-bearing, on evidence, not a gap.** I checked a
    proposed transitive proof through T-INT-02 (Bridge.StateHash() vs the
    fixture's stateHash) and it does not hold. `Bridge.StateHash()` calls
    `strat::canonicalStateHash(GameState)`, defined in `Replay.good.cpp:224`
    over `canonicalStateBytes` (`Replay.good.cpp:149-222`) — a different
    function from `Driver.good.cpp:485`'s `stateHash(Session)`, which
    `Save.h:12-16` explicitly disclaims: *"It is NOT the `stateHash` in
    Driver.h, which is the debug driver's own digest (GATE-DRV-06) and a
    different thing."* `canonicalStateBytes` folds in turn/side, per-side fame,
    objective hex+owner, per-unit id/side/hex/hp/flags, capture progress,
    `builtThisTurn`, and pending-build hex/side/defIndex — **no terrain field
    or terrain index appears anywhere in it.** Nor is there a transitive path:
    scenario seeding resolves each hex's terrain by NAME, scanning
    `t.terrain` for `.id` match (`Replay.good.cpp:299-308`, mirrored in
    `Driver.good.cpp:336-343`), so the semantic properties attached to a hex
    (moveCost, defensePct, capturable, …) are correct regardless of the
    table's row order in that build — unlike units, no `SaveCommand` field
    carries a raw terrain-table index the way Build's `unitId` carries
    `defIndex` (`Save.h:59-68` lists all five command kinds' fields: `unitId`,
    `hex`, nothing terrain-shaped). So a `DT_Terrain` row-order divergence
    changes no semantic value, reaches no hash byte, and is not merely
    untested — it is untestable via T-INT-02 by construction, because the
    format never externalizes a terrain index the way it externalizes
    `defIndex`. `T-DATA-05.TerrainTableMatchesCsv`'s name-keyed `FindRow`
    comparison is therefore the correct and sufficient check for terrain; no
    GATE-BRIDGE-DEFS-shaped terrain test is needed. Suite is 18/18 and git
    status is clean apart from this steward's own edits.

### Phase 1

- **Completed:** 2026-08-12
- **Exit criterion:** "`FStratBridge` gains `Forecast`, the five `Submit*` façade
  methods, `RecordedLog`, `SerializeRecordedSave`; build green; parity tests
  green for every one of those methods including `Reachable`".
- **Met.** Evidence:
  - Engineer added, additions only (349 insertions, 0 deletions, confirmed by
    the gate against `HEAD` `b0a55c5`), to `Source/StratBridge/StratBridge.h`/
    `.cpp`: `FStratSaveIdentity`, `SubmitMove`, `SubmitAttack`, `SubmitBuild`,
    `SubmitCapture`, `SubmitEndTurn`, `RecordedLog`, `SerializeRecordedSave`,
    `Forecast`, private `SubmitStamped` + `Recorded`.
  - The five command kinds are `Move, Attack, Build, Capture, EndTurn`
    (`Source/StratRules/Save.h:54`) — **there is no `Wait`**. The orchestration
    brief said `Wait` and was wrong; recorded here so the next reader does not
    re-introduce it. `Wait` is a UI-level concept the save format cannot
    carry.
  - Suite **31/31** (was 18; +13), report
    `Saved/AutomationReport/index.json`, `reportCreatedOn
    2026.08.12-16.31.54`, `succeeded 31 / failed 0 / notRun 0`.
  - `Reachable`'s five clauses are now green in
    `Source/StratBridge/Tests/StratBridgeQueryParity.cpp`, closing the
    zero-coverage debt from `e0cc53d`: `T-UI-02.MatchesModuleQuery`
    (`:245-311`), `.NotHexDistance` (`:336-474`), `.RefusesUnknownUnit`
    (`:490-551`), `.SuccessIsNeverEmpty` (`:564-621`), `.RefusesUnseeded`
    (`:637-704`). `NotHexDistance` measured **122 divergent hexes across 10 of
    10 units**, from three causes: occupancy (a friendly unit blocks a
    distance-1 hex), impassable Water at moveCost 0, and weighted
    Woods=2/Mountains=3 cost.
  - Plus `T-UI-01.*` (3 clauses) and `T-SAVE-06.*` (5 clauses) in
    `StratBridgeQueryParity.cpp` / `StratBridgeSaveRecording.cpp`.

### Phase 2

- **Completed:** 2026-08-12
- **Exit criterion:** "`StratViewModel.h/.cpp` in `StratUI`; `AdoptBridge` +
  `SetViewingSide` on `AStratScoreboardHUD`; build green; all existing tests
  still green".
- **Met.** Evidence, all verified by the gate in source rather than taken
  from builder reports:
  - `Source/StratUI/StratViewModel.h`/`.cpp` — a fully reflected view model
    that **names no `strat::` type at all**: `FIntPoint` hexes (X=q, Y=r),
    `FName` terrain/unit ids, `EStratResultTier`. `StratViewModel.h` includes
    only `CoreMinimal.h` and its own `.generated.h`, and forward-declares
    `class FStratBridge`. Built by one `STRATUI_API` free function
    `StratBuildViewModel(const FStratBridge&, int32 ViewingSide,
    FStratViewModel&, FString&)`, deliberately a free function so it is
    assertable with no actor and no Slate in existence.
  - `AdoptBridge`, `SetViewingSide`, and `GetViewingSide` on
    `AStratScoreboardHUD`. **Ownership is represented by which of two members
    is non-null, with no `bOwns` bool**: `TPimplPtr<FStratBridge> OwnedBridge`
    (freed in `EndPlay`) vs raw `FStratBridge* AdoptedBridge` (**never freed
    here**; `EndPlay` nulls it and destroys nothing). `AdoptBridge` does
    `OwnedBridge.Reset(); AdoptedBridge = &InBridge;` in that order, so no
    interval exists with both set. `BeginPlay` still calls `SeedBridge` when
    `AdoptedBridge == nullptr`, keeping the owned path alive for the
    pre-existing tests.
  - Suite **44/44** (was 31; +13), report `Saved/AutomationReport/index.json`,
    `reportCreatedOn 2026.08.12-18.06.16`, `succeeded 44 / failed 0 / notRun
    0`, every entry `Success`. All 31 pre-existing tests present and passing.
  - New clauses: six `T-INT-05.*` on the builder
    (`Source/StratUI/Tests/StratViewModelParity.cpp`) and seven `T-UI-03.*` on
    the HUD seam (`Source/StratUI/Tests/StratScoreboardHUDSeam.cpp`).
- **Two contracts phase 3 inherits — recorded here explicitly, they are the
  reason the phase was worth gating:**
  - **`AdoptBridge` refuses an unseeded bridge.** So
    `UStratMatchSubsystem` **must seed before handing over**. That ordering
    is now a contract, not a convention.
  - **`FStratUnitView::bDone` and `bLockedThisTurn` are declared and left
    false, with no producer.** `Ui.h:186-197` puts the presentation block in
    the view model precisely so T-INT-05 can rebuild from the model alone.
    Phase 3's selection machine must write them **onto the built model, never
    keep its own copy in an actor** — if it does, T-INT-05 silently becomes
    false with no compiler diagnostic. `T-INT-05.PresentationBitsAreDefaulted`
    pins the negative; nothing can pin the positive obligation until the
    owner exists.
