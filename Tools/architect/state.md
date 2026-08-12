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
  and **declined as new production surface beyond phase 2's exit criterion**; offered again in
  phase 3 and **declined a second time**. The tests prove the *adopted* bridge is not freed
  (`T-UI-03.EndPlayLeavesAnAdoptedBridgeUsable`); `T-INT-05.SubsystemSeedsBeforeHandover` proves
  the *consequence* that the HUD reads the subsystem's bridge and not one it allocated, but
  neither proves the owned path itself. Stated honestly in `StratScoreboardHUDSeam.cpp:31-37`.
  Out of scope for the hot-seat milestone.
- **`HexSize` on `AStratBoardActor`** is centre-to-centre spacing for a pointy-top layout, the
  only axial→world constant in the project; must be matched to whatever tile mesh phase 5 picks.
- **`EnhancedInput` is absent from `StratPlay.Build.cs` by design**; phase 4 adds it. Do not
  "fix" it early. — **Discharged in phase 4**: `EnhancedInput` is now Private in
  `StratPlay.Build.cs`; `StratBridge` moved Public → Private there in the same change.
- **A `bDone`/`bLockedThisTurn` producer does not exist yet.** Phase 4's selection machine must
  write those onto the built view model via `ApplyView(model)`, never into an actor.
  `T-INT-05.NoActorHoldsPresentationBits` pins the negative only. — **Discharged in phase 4**:
  `FStratSelectionMachine::DecorateViewModel`, called between `BuildViewModel` and `ApplyView`,
  writes both bits from the machine's own `TSet`s; no actor holds either.
- **`SubmitCapture` has no affordance and the machine never emits `Capture`.** The phase-4
  command list is literally move → attack, wait, end turn; which hex offers a capture and how
  the player is told is an unanswered UI question. The applier's `switch` will need one new arm.
  (Phase 4 deferral, carried forward.)
- **The hot-seat hand-over key is deliberately unbound.** `SetViewingSide` on a keypress would
  let either player see the other's board at any time; the confirmation screen is UI work no
  phase owns. (Phase 4 deferral, carried forward.)
- **`SetLockedThisTurn` has a writer and no shipping caller** (§2.11.6 guided opening is out of
  milestone), so `bLockedThisTurn` is false in every running path. Its clause calls the setter
  itself and discloses in four places that it pins machine behaviour, not that any shipping path
  produces a lock. **When §2.11.6's producer lands it needs a clause of its own; this one will
  not cover it.** (Phase 4 deferral, carried forward.)
- **The attack branch's "already acted" refusal
  (`StratSelectionMachine.cpp:258-263`) is unreachable by any click sequence**, because
  `NotifyCommandApplied` marks an attacker DONE on the same event. Ruled an acceptable
  defensive guard, not dead code — it guards a disagreement between the model's `bHasActed`
  (rules-side) and the machine's `DoneUnits` (engine-side, per-session) that a loaded save, a
  replayed log, or phase 6's PIE can produce even though no scripted sequence in this suite can.
  It carries no comment saying it is unexercised and why; that debt is currently owned quietly
  in `strat-gameplay-engineer`'s lane, deferred here rather than built, per the scope fence.
- **`ReplayRecordedLogOnto` does no save round trip** — no serialize, no parse, no
  `FStratSaveIdentity`. `SerializeRecordedSave` + `T-SAVE-06.SaveRoundTripsToEqualHash` already
  cover that path; a second entry point there would be a second policy over the same bytes.
- **`ETriggerEvent::Started` on all four input actions is asserted, not measured** — no input
  asset exists yet to test against. Discharged the first time the controller runs in PIE with
  real assets, i.e. no earlier than phase 5.
- **`Saved/AutomationReport/index.json` is UTF-8-with-BOM, not UTF-16.** First bytes are
  `EF BB BF`; all three UTF-16 codecs fail to parse it and `utf-8-sig` succeeds. Two separate
  gate passes reported UTF-16 for this file and were both wrong. **A phase-6 gate that
  hardcodes UTF-16 to parse the report will read zero tests and may not say so** — the same
  failure shape as the bare-`Stratocracy.uproject` test command that exits in ~1s having run
  nothing. Recorded here so a phase-6 reader hits it before writing that parser.
- **`.agents/ue-project-context.md:195` reads "51/51, hot-seat phase 3"** against a measured
  66/66 as of phase 4, and its line-41 module-arrow row for `StratPlay` does not mention
  `EnhancedInput`, added to that module's `Build.cs` this phase. That file is outside this
  steward's lane (`.agents/` is not `Config/` or `Tools/architect/`) — recorded here as a
  handoff rather than edited.

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

### Phase 3

- **Completed:** 2026-08-12
- **Exit criterion:** "`StratPlay` module created and registered in
  `Stratocracy.uproject`; board actor, unit actor, match subsystem, camera
  pawn, game mode; build green; hex-layout round-trip and view-reconcile
  tests green."
- **Met.** Gate passed with **zero findings**. Evidence:
  - New module `StratPlay`, registered in `Stratocracy.uproject` (+5 lines) as
    `Runtime`/`Default`, **and** carrying `IMPLEMENT_MODULE(FDefaultModuleImpl,
    StratPlay)` at `StratPlay.cpp:14`. The gate verified both, specifically
    because `Source/StratRules/` is the local precedent for a module with a
    `Build.cs` and no `.uproject` entry — copying that shape would have
    produced a green build and a module that never loads.
  - Classes: `AStratBoardActor : AActor`, `AStratUnitActor : AActor`,
    `UStratMatchSubsystem : UWorldSubsystem`, `AStratCameraPawn : APawn`,
    `AStratGameMode : AGameModeBase`. Reflected structs `FStratTerrainLayer`,
    `FStratMatchConfig`.
  - Suite **51/51** (was 44; +7), `Saved/AutomationReport/index.json`,
    `reportCreatedOn 2026.08.12-19.11.44`, `succeeded 51 / failed 0 / notRun
    0 / succeededWithWarnings 0`. All 44 baseline tests still plain
    `Success`, none downgraded. Read from the report by the gate, not taken
    from a builder.
  - The seven new clauses, by full name:
    `Stratocracy.StratPlay.T-UI-02.BoardHexRoundTrip`,
    `.T-UI-02.ReachOverlayIsNotComputedHere` (both
    `Source/StratPlay/Tests/StratBoardPicking.cpp`),
    `.T-INT-05.ApplyViewSpawnsMovesAndDestroys`,
    `.T-INT-05.NoActorHoldsPresentationBits` (both
    `Tests/StratMatchReconcile.cpp`), `.T-INT-05.SubsystemSeedsBeforeHandover`,
    `.T-INT-05.StartMatchRefusalLeavesNoBridge`,
    `.T-UI-03.SetViewingSideMutatesNoState` (all
    `Tests/StratMatchLifecycle.cpp`).

  Decisions worth preserving because they foreclose alternatives:
  - **Tile HISM components are created from data, keyed by `FName`
    `TerrainId`, not seven `CreateDefaultSubobject` calls.** The
    declared-in-C++ alternative was rejected because it makes
    `StratBoardActor` a second author of the §4.8 terrain table, and because
    phase 0 ruled `DT_Terrain` row order *not* load-bearing — so a positional
    array would draw Water as Woods after a table edit, silently. Cost: tiles
    do not appear individually in the Blueprint component tree; phase 5
    styles them through the keyed `TerrainMeshes` map.
  - **There is deliberately no `HexAtWorldLocation`.** Picking is an
    instance-index lookup only. A rounding inverse of `WorldLocationOfHex`
    would disagree with the forward map at tile edges, and disagreeing about
    which hex the cursor is on is how a player attacks the wrong unit.
  - **`UWorldSubsystem`, not GameInstance and not the GameMode.** GameInstance
    outlives the map while the seeded state is per-map; the GameMode is an
    actor, and putting `strat::GameState` on an actor reinstates the §4.1
    debt this phase discharges. The GameMode holds configuration only.
  - **`AGameModeBase`, not `AGameMode`** — `AGameMode`'s `MatchState` machine
    would be a second, engine-side answer to "is the match over" beside
    `strat::UiMatchView`.
  - **`StartMatch` is all-or-nothing on the rules side, deliberately not on
    the presentation side.** A failed load resets the bridge to null so
    `GetBridge()` can never hand out a half-loaded one; a missing tile mesh
    returns `false` with the match live. `T-INT-05.StartMatchRefusalLeavesNoBridge`
    pins the first across four refusal paths — including the one where
    `LoadDefinitions` succeeded and a half-loaded bridge genuinely existed.

  **The measured link fact — the phase's most transferable finding:**
  `StratPlay.Build.cs` must name `StratBridge` **directly**; relying on
  `StratUI`'s public `StratBridge` dependency produced 4 × `LNK2019`. The
  missing symbols were exactly the out-of-line `STRATBRIDGE_API` ones —
  `FStratBridge::LoadDefinitions`, `LoadScenarioFromFile`, the constructor,
  and the destructor via `TPimplPtr`'s `DeleterFunc<FStratBridge>` — while
  `IsSeeded()` and `GetBridge()`, being header-inline, never appeared. **A
  smaller caller would not have surfaced this.** A transitive public
  dependency propagates include paths but **not the import library**. This is
  categorically **not** the `strat::`-across-a-module error measured 8× on
  this project: those symbols arrive *unadorned* in `namespace strat`,
  whereas these arrived `__declspec(dllimport)`, proving the header and the
  `_API` macro both resolved.

  Deferred, recorded because a positive obligation is now owed to phase 4:
  - **`HexSize` on `AStratBoardActor`** is centre-to-centre spacing for a
    pointy-top layout and is the only axial→world constant in the project.
    It must be matched to whatever tile mesh phase 5 picks.
  - **`EnhancedInput` is absent from `StratPlay.Build.cs` by design**; phase 4
    adds it. Anyone finding it missing should not "fix" it early.
  - **A `bDone`/`bLockedThisTurn` producer does not exist yet.** Phase 4's
    selection machine must write those onto the built view model via the
    `ApplyView(model)` seam, **never** into an actor. Nothing can pin that
    positive obligation until the owner exists;
    `T-INT-05.NoActorHoldsPresentationBits` currently pins the negative.
  - **Deferred a second time, and say so explicitly:** proving
    `AStratScoreboardHUD` does not *also* allocate an owned bridge needs a
    production-side reader (`bool OwnsItsBridge() const`) or the allocation
    counter offered and declined in phase 2. `T-INT-05.SubsystemSeedsBeforeHandover`
    proves the *consequence* — the HUD reads the subsystem's bridge, that
    bridge is not the one it allocated, and an `EndTurn` on the subsystem's
    bridge moves the HUD's `StateHash` and lengthens its log — but not the
    member. This is the second phase running in which the direct check was
    offered and declined as production surface beyond the exit criterion.
    Record that it has now been deferred twice.

  Two non-gating observations from the gate, for a later phase and not this
  milestone:
  - `StratPlay.Build.cs:94` lists `StratBridge` under
    `PublicDependencyModuleNames`; nothing in the module's headers names
    `FStratBridge` beyond a forward declaration, so `Private` would be the
    tighter statement and would stop the arrow being re-exported to phase 4's
    dependents.
  - `AStratBoardActor` contains **three** copies of the axial→world
    expressions (`WorldLocationOfHex`, `ApplyHexes` at
    `StratBoardActor.cpp:205-210`, `FillOverlay` at `:295-300`), differing
    only in Z, while its header block claims to be "the only axial → world
    conversion in the project". They cannot disagree today because they read
    the same `HexSize`, but the prose is stronger than the code. Follow-up
    for `strat-gameplay-engineer`.

### Phase 4

- **Completed:** 2026-08-12
- **Exit criterion:** "PlayerController with Enhanced Input, the selection
  state machine as a plain testable struct, the `STRAT-CMD` log line; build
  green; hot-seat replay-parity test green."
- **Met, but only after a `BLOCK` on the first gate.** The re-gate cause is
  the most transferable finding in this phase — record why it failed before
  what landed.

  **The first gate — `VERDICT: BLOCK`, three findings.**
  - **Finding 1 — a split clause left the recording joint unpinned.** The
    chain the criterion needs is *clicks → outcomes → submissions →
    `RecordedLog()` → replay → equal hash*. Two clauses covered the ends;
    **the joint "that `StratSubmitSelectionCommand` calls a recording entry
    point at all" was pinned by nothing.** The StratPlay clause was
    self-referential — it drove both bridges through the same submission
    function, so any path, recording or not, yielded equal hashes, and its
    count assertion counted `STRAT-CMD accepted` lines emitted by that same
    function, making it both subject and witness. Routing submission through
    a non-recording apply path would have left both clauses green with
    `RecordedLog()` empty after a full hot-seat session — exactly what
    phase 6's PIE gate leans on. Closed by two new engine-typed methods,
    `int32 RecordedCommandCount() const` and
    `FStratResult ReplayRecordedLogOnto(FStratBridge& Fresh) const`, and
    repointing the gating clause onto them. **The general lesson: a clause
    whose expectation is produced by the code under test pins nothing, and
    splitting one clause across two modules can drop the joint between them.**
  - **Finding 2 — `EStratSelectionCommand::Attack` was executed by no test.**
    All four references were inside `StratSelectionMachine.cpp`;
    `SubmitAttackAtHex` had zero test callers. A q/r transposition at
    `StratBridge.cpp:739-742` would have refused every attack in the game
    with the suite green at 62/62. Move was implicitly protected because its
    destination is read out of `ReachableHexes` inside a clause asserting
    acceptance; Attack had no such clause. The conversion turned out to be
    **correct** — `Hex.h:11-14` declares `q` then `r` — but correct-and-untested
    was still a `BLOCK`. Closed by three clauses; the transposition check
    takes its coordinate from the vendored fixture's own `strat::Hex{3,4}`
    (entry 17) rather than from `AttackTargetHexes`, **specifically so a
    double transposition cannot cancel**, asserts `q != r` first, and submits
    the transposed `{4,3}` as a foil observed refused.
  - **Finding 3** was prose stating `strat::saveCommandName` is "file-local"
    when it has external linkage and merely carries no `_API` macro — the
    conclusion held, the stated reason did not, and a wrong reason tells the
    next reader no pin is possible anywhere.

  **What landed, once the re-gate passed with zero findings:**
  - `Source/StratPlay/StratSelectionMachine.h`/`.cpp`,
    `StratPlayerController.h`/`.cpp` (new); `Source/StratBridge/StratBridge.h`/
    `.cpp`, `StratPlay.Build.cs`, `StratGameMode.h`/`.cpp`,
    `StratBoardActor.h`/`.cpp` (modified). No new module —
    `Stratocracy.uproject` unchanged, confirmed by the gate rather than
    assumed.
  - `FStratSelectionMachine` is a **plain non-reflected struct**, drivable
    with no actor, no world, no PIE, no Slate. `AStratPlayerController` holds
    it by value as a non-`UPROPERTY`.
  - Enhanced Input: five `EditDefaultsOnly TObjectPtr` properties
    (`SelectionMappingContext` + `SelectAction`, `CancelAction`,
    `WaitAction`, `EndTurnAction`), **null by default and null-safe at every
    use site**, because phase 5 authors the assets. `EnhancedInput` added to
    `StratPlay.Build.cs` (Private); `StratBridge` moved Public → Private
    there.
  - The `STRAT-CMD` line:
    `STRAT-CMD accepted kind=%s unit=%d hex=%d,%d turn=%d side=%d hash=%s`,
    sole call site `StratSubmitSelectionCommand`. `kind` spells
    `Move`/`Attack`/`EndTurn` to match the save format's own words so a gate
    can compare a line to a `commandLog` entry with no translation table.
    `unit`/`hex` are **always numeric**, carrying `-1`/`-1,-1` where the kind
    has no such field, so the line's shape never depends on its content.
    `turn`/`side` are read **before** submission, `hash` is `StateHash()`
    **after**. Refusals log `STRAT-CMD refused …` — a deliberately different
    phrase, so `grep "STRAT-CMD accepted"` counts only commands that applied.
  - Suite **66/66** (was 51; +15), `succeeded 66 / failed 0 / notRun 0`,
    `reportCreatedOn 2026.08.12-21.47.20`, read from the report by the gate.

  Decisions that foreclose alternatives — recorded with their reasons:
  - **`StratPlay` still names no `strat::` type; the translation went into
    the bridge.** Six new engine-typed `STRATBRIDGE_API` methods: `Turn`,
    `SideToMove`, `ReachableHexes`, `AttackTargetHexes`, `SubmitMoveToHex`,
    `SubmitAttackAtHex`. The rejected alternative was a `strat::Hex`-naming
    helper inside a `StratPlay` `.cpp` — legal by the letter of the linker
    rule, since naming is not calling — killed because it would put a second
    spelling of the axial coordinate in the one module that is supposed to
    have none.
  - **`AttackTargetHexes` is an enumeration, not a range check.** `Ui.h`
    declares no target-enumeration counterpart to `uiReachable`, so it walks
    the live unit list and asks `uiForecast` per enemy, keeping the module's
    `legal`. No distance is compared and no `UnitDef` range is read. If such
    a function is ever vendored, the body becomes a forward and no caller
    changes.
  - **The machine holds no mirror of the rules state.** `bHasMoved`/
    `bHasActed` are read off the view model every call. The rejected
    alternative — optimistic state advanced and rolled back on refusal — was
    killed because the rollback path is the one no test exercises. This is
    why a refused command cannot desynchronise the machine.
  - **The view model is rebuilt on every event, not read from
    `GetViewModel()`** ("a record, not a source"); otherwise what a click
    means would depend on when `ApplyView` last ran.
  - **`bDone`/`bLockedThisTurn` reach the model through
    `FStratSelectionMachine::DecorateViewModel`**, called between
    `BuildViewModel` and `ApplyView` — the seam phase 3 left owing. The bits
    live in the machine's `TSet`s; no actor holds them, neither is a
    `UPROPERTY` on an actor. `bDone` is set on a Wait and on an accepted
    Attack, **never** derived from `bHasMoved && bHasActed`.
  - **`GameMode` sets `PlayerControllerClass` but still not `HUDClass`** — a
    bare controller with null input assets is inert and says so once; a bare
    scoreboard HUD refuses every refresh and reads as a bridge bug.
  - **An empty recorded log is a refusal, not a successful no-op**
    (`ReplayRecordedLogOnto` arm 5). A deliberate departure from
    `RecordedLog()`'s "empty is an ordinary answer" posture, because as a
    *replay input* empty returns success and equal hashes and proves
    nothing. Scenario identity is compared by `strat::scenarioHash` over the
    seeded bytes, not by the `scenarioId` label a file chose.
  - Both phase 3 non-gating findings were taken and confirmed mechanical:
    `StratBridge` → `PrivateDependencyModuleNames`, and the three
    axial→world copies in `AStratBoardActor` collapsed into one private
    `LocalLocationOfHex`.

  **T-UI-02 foil weaker than phase 1's, by construction, and the gate
  verified the argument rather than waving it through:** 69 divergent hexes
  across 5 of 5 active units (occupancy, impassable Water, weighted
  Woods/Mountains) vs phase 1's 122 across 10 of 10, because `StratPlay`
  cannot see a unit's `move` and so the disc radius is the largest distance
  the real reachable set itself contains. Every reachable hex is reached in
  ≤ `move` steps and axial distance never exceeds step count, so the
  phase-4 disc is a strict subset of the classic one — conservative, the
  only direction a self-computed value may lean.

  Two corrections to carry forward as measurements — the numbered
  UTF-8-with-BOM and `.agents/ue-project-context.md:195` staleness items
  live under NEXT above, next to the other deferred debt.

**Phases 0-4 are closed.** Phase 5 is next and requires the editor OPEN: hex
mesh + terrain material instances, Input Mapping Context + Input Actions, the
Blueprints and Widget Blueprints, `Lvl_FerrumCrossing`, with `Config` map
defaults flipped last. Out-of-scope list unchanged: production menu
(§2.11.5), guided opening (§2.11.6), info panel, toasts, save-slot UI, AI
opponent, move-undo.

Note for phase 5's builder, from the engineer: the five input assets must be
authored and set on a `BP_` subclass of `AStratPlayerController`, all
`UInputAction`s Digital/bool; the GameMode Blueprint's `PlayerControllerClass`
must point at that `BP_` subclass (the C++ default is the floor, not the
answer); and **tile meshes must be traceable on `ECC_Visibility` or picking
returns nothing.**
