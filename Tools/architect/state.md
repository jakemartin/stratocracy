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
- **Hot-seat milestone is COMPLETE (phase 6 closed 2026-08-13); see "Hot-seat milestone —
  COMPLETE" below.** Out of scope, unchanged: production menu (§2.11.5), guided opening
  (§2.11.6), info panel, toasts, save-slot UI, AI opponent, move-undo.
- Two clauses owed to the next test-author phase: `T-INT-05.WaitIsDistinguishableFromAttack`,
  `T-INT-05.WaitWithNothingSelectedIsANoOp` (writable since phase 6's `STRAT-WAIT spent` line
  landed; see Phase 6 — CLOSED for detail).
- Standing debt, ruled out of any future phase's scope by the phase-6 reviewer: NeoStack input
  injection reaches `UGameViewportClient::InputKey` but never `UPlayerInput`, so no playtest is
  machine-repeatable. This is a NeoStack plugin issue outside this repository.
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
- **Phase 6 risk, and the most consequential open item: the `STRAT-CMD` click-to-command gate
  is unclosed.** No `playtest_key` or `playtest_click` produced any `LogStratPlay` output.
  **Correction to this record, made in phase 6:** the original claim here — "total input
  silence, reproduced on Epic's own shipped TopDown template with known-good assets" — had a
  confounded control and is **not established**. The "TopDown template" the control actually
  drove was `Lvl_FerrumCrossing` launched under the TopDown template's GameMode (a consequence
  of the stale-config bug found in phase 6 — see below), and `Lvl_FerrumCrossing`'s `Floor`
  actor had been deleted in phase 5 (recorded above, "The template's `Floor` actor was deleted
  from `Lvl_FerrumCrossing`"). A click on that map hits nothing under the cursor, so
  `CharMoveComp.Velocity` legitimately stayed zero for a reason having nothing to do with
  whether input reaches the game. `Escape` still ending PIE is real and unaffected by this
  correction. The underlying question — does simulated input reach `StratPlayerController` at
  all — is **still open**, restated accurately with new, still-confounded-a-different-way
  measurements under Phase 6 below. Phase 4's standing debt — "`ETriggerEvent::Started` on all
  four input actions is asserted, not measured" — remains open, and phase 6's
  `assert_log_contains` gate still depends on solving it. **Do not action the phase-6 escalation
  that asks to repoint `Lvl_FerrumCrossing`'s GameMode or `GlobalDefaultGameMode` again** — see
  Phase 6 below: the running editor was serving a stale, pre-flip config, and the on-disk
  `Config/DefaultEngine.ini` needs no change.
- **`AWorldSettings` is unreachable through the NeoStack Lua API**, so no level's GameMode
  override can be authored that way. Exact failures: `configure("actor","WorldSettings")`,
  `select_actor`, `get_actor_properties`, `open_asset` on the sub-object path, and
  `invoke({actor_label="WorldSettings"})` all failed; `configure` accepts only
  `actor|landscape`. (Worked around this phase for the `Config` question by reading the two
  levels' binary `.umap` bytes directly instead — see Phase 5 below.)
- **`array_count("DefaultKeyMappings.Mappings")` → `property not found`** — dot-path into a
  struct's array is unsupported; write the whole struct instead.
- **`FKey` `ImportText` takes the bare name.** `Key=(KeyName="X")` silently produced `Key=()`;
  `Key=LeftMouseButton` succeeded. A silent empty-key write is exactly the kind of failure that
  reads as an input bug later.
- Three findings for `strat-gameplay-engineer`, none blocking: (1) `AStratCameraPawn` sets
  `Arm->bInheritYaw = false` at `StratCameraPawn.cpp:59`, pinning the camera to world yaw 0 so
  spawn rotation cannot turn the view — measured, `PlayerStart` yaw `-90` and `0` give
  pixel-identical framing; Ferrum Crossing's long axis (2800×1385, long axis on world X) is
  stuck on the screen's short axis. An `ArmYaw` property or `bInheritYaw = true` fixes it; the
  builder compensated with `DefaultArmLength = 3600`. (2) `AStratBoardActor` has
  `TerrainMeshes` but no `TerrainMaterials` map, which is the sole reason there are seven
  near-identical `SM_HexTile_*` assets instead of one mesh and seven materials. (3)
  `MI_Terrain_Water.Color` reads back `(0.02, 0.09, 0.30)` yet renders pale blue, and darkening
  every terrain colour ~55% produced no visible change — the harvested `M_Mono` does not
  respond linearly to `Color`; a purpose-built opaque `M_StratTile` would remove the guesswork.
- **Scope note:** `BP_StratCamera` is a sixth Blueprint beyond the four the brief named. The
  builder judged it inside "the Blueprints" because `AStratCameraPawn`'s C++ default
  `DefaultArmLength = 1200` shows about a third of the board. It holds no logic — four float
  defaults only. Recorded so a gate can evaluate the judgement rather than rediscover it.
- **The map→GameMode binding is a single unguarded line.**
  `Config/DefaultEngine.ini:4` (`GlobalDefaultGameMode`) is now the *only*
  thing binding `Lvl_FerrumCrossing` to `BP_StratGameMode_C`, because
  `AWorldSettings` is unreachable through the NeoStack Lua API and the level
  carries no World Settings override — unlike `Lvl_TopDown`, which does (see
  Phase 5's `Config` flip evidence above). This fact is not recorded in
  `.agents/ue-project-context.md`, which is where a future config edit would
  most plausibly be checked against. Owed to a future doc pass. **No crew
  agent owns `.agents/ue-project-context.md`** — this is a flag, not a task
  the steward takes.

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

**Phases 0-5 are closed**, including the `Config` map-defaults flip (see Phase
5 above). Phase 6 is next; its `assert_log_contains` gate depends on closing
the unresolved click-to-command input gate recorded under NEXT — that item is
this milestone's one standing blocker. Out-of-scope list unchanged:
production menu (§2.11.5), guided opening (§2.11.6), info panel, toasts,
save-slot UI, AI opponent, move-undo.

Note for phase 5's builder, from the engineer: the five input assets must be
authored and set on a `BP_` subclass of `AStratPlayerController`, all
`UInputAction`s Digital/bool; the GameMode Blueprint's `PlayerControllerClass`
must point at that `BP_` subclass (the C++ default is the floor, not the
answer); and **tile meshes must be traceable on `ECC_Visibility` or picking
returns nothing.**

### Phase 5

- **Completed:** 2026-08-12
- **Exit criterion:** "hex mesh + terrain material instances, Input Mapping
  Context + Input Actions, the Blueprints and Widget Blueprints,
  `Lvl_FerrumCrossing`, with `Config` map defaults flipped last."
- **Met**, with one item carried forward unresolved (the click-to-command
  input gate — see NEXT) that is explicitly outside this exit criterion's
  wording. Evidence:
  - Assets: 13 material instances under `/Game/StratArt/Materials/`, 9 hex
    meshes under `/Game/StratArt/Meshes/`, 5 Enhanced Input assets under
    `/Game/StratInput/`, 5 Blueprints under `/Game/StratPlay/`, and
    `/Game/StratMaps/Lvl_FerrumCrossing`.
  - `HexSize = 200.0`, derived from `SM_Tile_Hex`'s across-flats width along X
    (2 × 100.0). The collision hulls read out as an exact regular hexagonal
    prism — vertices `(±99.9998, 0)`, `(0, ±115.4694)`,
    `(±99.9998, ±57.7347)` — not a bounding approximation. Neighbours abut
    with zero overlap.
  - `SM_Hex` was rejected for tiles because its collision is a single box of
    half-extent `(87.68, 101.24)` which at a 151.9 row pitch overlaps
    neighbours by ~50 units, producing coplanar ambiguous hits and wrong-hex
    picking near row edges. Used only as `OverlayMesh`, where the C++
    disables collision.
  - The template's `Floor` actor was deleted from `Lvl_FerrumCrossing`. It
    sits at `Z=0`, coplanar with the tile tops, and would have contested
    every `GetHitResultUnderCursor(ECC_Visibility)` — the one asset-side way
    picking could have silently failed.
  - Terrain keys `Plains, Woods, Mountains, Water, Town, Bridge, Factory`
    read from the `Id` column of `Data/terrain.csv`; unit keys
    `Infantry, Tank, Artillery, Recon` from `Data/units.csv`.
  - PIE confirms the live match: `LogStratPlay: Match live: seeded from
    '.../Data/ferrum_crossing.json' (first side 0), drawn for side 0, 99
    hexes and 10 units on screen.` `BP_StratPlayerController_C_0`
    possessing, mapping context added, no missing-context warning.
  - `IMC_Selection.DefaultKeyMappings`: `IA_Select`→`LeftMouseButton`;
    `IA_Cancel`→`RightMouseButton`, `Escape`; `IA_Wait`→`W`;
    `IA_EndTurn`→`Enter`, `SpaceBar`. Storage location verified against all
    11 pre-existing IMCs in the project rather than guessed.
  - `Lvl_TopDown`, `BP_TopDownGameMode` untouched by the builder.

  **The `Config` flip — measured before flipping, not taken from the builder
  or the brief.** The builder's escalation argued `GlobalDefaultGameMode` is
  load-bearing for `Lvl_TopDown`'s existing scoreboard evidence, because it
  could not author a World Settings GameMode override on `Lvl_FerrumCrossing`
  (`AWorldSettings` unreachable through NeoStack — see NEXT) and inferred
  `Lvl_TopDown` must therefore also rely on the global default, making a flip
  unsafe. That inference rested on an untested premise, which I checked by
  measurement rather than accepting or overriding on argument alone:
  - **`Lvl_TopDown` already carries its own World Settings GameMode
    Override, as a hard package reference.** Extracted printable ASCII
    strings directly from the binary `Content/TopDown/Lvl_TopDown.umap`:
    it contains `/Game/TopDown/Blueprints/BP_TopDownGameMode`,
    `BP_TopDownGameMode_C`, and the literal property name
    `DefaultGameMode`. `DefaultGameMode` is `AWorldSettings`'s own override
    property — confirmed against the engine header,
    `WorldSettings.h:631-634`: `UPROPERTY(EditAnywhere, BlueprintReadOnly,
    Category=GameMode, meta=(DisplayName="GameMode Override"))
    TSubclassOf<class AGameModeBase> DefaultGameMode;`. The same extraction
    against `Content/StratMaps/Lvl_FerrumCrossing.umap` finds `WorldSettings`
    / `WorldSettings1` (the actor exists) but **no GameMode string of any
    kind** — consistent with the builder's report that it could not write
    one. A level's own World Settings override always takes precedence over
    the config global, so flipping `GlobalDefaultGameMode` cannot strip
    `Lvl_TopDown`'s GameMode; the dilemma the builder raised does not apply
    to it.
  - **The existing scoreboard/match PIE evidence was never produced via the
    global default in the first place.** `Saved/Logs/Stratocracy.log:4911`:
    `LogGlobalStatus: UEngine::Browse Started Browse:
    "/Game/StratMaps/Lvl_FerrumCrossing?game=/Game/StratPlay/BP_StratGameMode.BP_StratGameMode_C"`
    — an explicit `?game=` override, followed at `:4926` by `LogLoad: Game
    class is 'BP_StratGameMode_C'` and at `:4930` by `LogStratPlay: Match
    live: seeded from '.../ferrum_crossing.json' ...`. The reproduction path
    names its map and GameMode explicitly and does not read
    `GlobalDefaultGameMode` at all.
  - Both checks say the flip is safe on evidence, not inference, so **all
    three lines were flipped together** — see Files changed below. No
    outstanding `Config` item remains from this job.

  Decisions worth preserving because they foreclose alternatives:
  - **`GameDefaultMap`/`EditorStartupMap`/`GlobalDefaultGameMode` were
    treated as one decision, not three**, once both measurements confirmed
    no conflict — the task brief allowed treating the GameMode line
    independently if a real conflict existed; none did, so splitting them
    would have been unmotivated caution.
  - **A binary `.umap` cannot be diffed for row order or struct content**,
    but a targeted ASCII-string extraction over its raw bytes is a legitimate
    measurement for "does this package reference symbol X at all" — it does
    not require the editor. Used here in place of the escalation the brief
    anticipated; recorded so a future steward reaches for it before assuming
    "binary, therefore unknowable" for this narrower class of question.

  **Committed as `d310aa1`** ("Phase 5: the global default was never
  load-bearing, and the floor would have eaten every click"), 35 files
  changed, pushed to `origin/master`. Parent `ed27d5a` (phase 4).

  **It took two gate passes — the block is worth recording, same reasoning
  as phase 4's.**
  - **First gate: `VERDICT: BLOCK`, one finding.** All eleven substantive
    checks passed; the block was the index. 28 asset files were staged
    (`A`/`AM`) while `Config/DefaultEngine.ini` was unstaged and all five
    `Content/StratPlay/` Blueprints were **untracked** (`??`, no ignore rule —
    simply never added). A commit from that index would have landed
    `Lvl_FerrumCrossing` referencing five Blueprints absent from the
    repository, and without the `GlobalDefaultGameMode` line that is the
    level's only map→GameMode binding. Owner was **the user**, not a crew
    agent, per the "agents do not commit" non-negotiable.
  - **A second, narrower trap found while clearing it, worth recording as a
    measurement:** most assets showed `AM` — staged, then modified again by
    the editor afterwards. Committing in that state captures **stale asset
    bytes** while the working tree holds newer ones: a commit that is a
    snapshot of a state nobody ever ran. Resolved by re-running `git add -A`
    to collapse every `AM` → `A` before committing. Likely cause is Unreal's
    Git revision-control plugin auto-staging assets on save — meaning **the
    index goes stale again the moment the editor touches a file.** Expect
    this on every asset phase, i.e. phase 6.
  - **Re-gate: `VERDICT: PASS`, zero findings**, audited against `d310aa1`
    rather than the working tree. The stale-bytes risk was confirmed clean:
    `git diff HEAD` empty, `git lfs fsck` OK, and four asset blobs
    (`BP_StratGameMode.uasset`, `Lvl_FerrumCrossing.umap`,
    `SM_HexTile_Plains.uasset`, `IMC_Selection.uasset`) hashing identically on
    both sides. The committed snapshot is the state measured working in PIE.
  - The gate also confirmed `.agents/ue-project-context.md` shows **no**
    drift from the tree at HEAD — module table (lines 37-41), the
    `StratRules`-deliberately-absent note (lines 60-64), and the vendored-path
    row (line 173, `rulesCommit cb8e12b`) all match measurement. Nothing in
    `d310aa1` invalidated it. Recorded so the existing staleness flag for that
    file (phase 4, `:195`) is not over-read as still-live everywhere.

### Phase 6 — in progress

- **Status:** in progress, not closed. No gate has run. This is a checkpoint
  so a fresh session can resume without re-measuring.
- **Exit criterion (unchanged, not yet met):** the `assert_log_contains`
  clause over `STRAT-CMD` closing the click-to-command input gate, plus
  on-disk evidence in `Tools/architect/evidence/`. Neither exists yet — see
  "Not yet produced" below.

  **Finding 1 — the running editor was serving a stale config; the on-disk
  `Config/DefaultEngine.ini` needed no change.** `Config/DefaultEngine.ini:4`
  reads `GlobalDefaultGameMode=/Game/StratPlay/BP_StratGameMode.BP_StratGameMode_C`,
  flipped in `d310aa1` (phase 5). The running `UnrealEditor` process
  (PID 34576) started **2026-08-12 18:11:07**; `DefaultEngine.ini` was last
  written **2026-08-12 19:01:37**. The editor read its config at startup and
  held the **pre-flip** `GlobalDefaultGameMode` in memory for the rest of that
  process's life. Consequence, measured: every `playtest_start({map=
  '/Game/StratMaps/Lvl_FerrumCrossing'})` in that process launched the map
  under the **TopDown template GameMode** — the PIE world contained
  `BP_TopDownController_C_0`/`BP_TopDownCharacter_C_1`, no
  `AStratPlayerController`, no `AStratCameraPawn`, no `AStratBoardActor`, and
  `playtest_observe` showed pure sky. **`Lvl_FerrumCrossing` does not need a
  World Settings GameMode Override and `GlobalDefaultGameMode` does not need
  repointing** — the phase-6 builder's gating escalation asked for one of
  those two, and the ask rested on the premise that the on-disk config is
  wrong. Measurement refutes that premise. **The fix is to restart the
  editor**, not to touch `Config/`. Generalised: **a long-lived editor
  silently serves the config it started with**, so any `Config/` change
  measured only on disk is unproven until a process that started after the
  write reads it. Phase 5's own PIE evidence never exercised the global
  default in the first place — `Saved/Logs/Stratocracy.log:4911` used an
  explicit `?game=` override — which is why the staleness went unseen until
  now.

  **Finding 2 — correction owed to phase 5's own record.** See the corrected
  NEXT bullet above ("Phase 6 risk, and the most consequential open item").
  Phase 5's "total input silence, reproduced on Epic's own shipped TopDown
  template with known-good assets" claim had a confounded control: the
  "TopDown template" it actually drove was `Lvl_FerrumCrossing` under the
  TopDown GameMode (a consequence of Finding 1's stale-config bug), and that
  map's `Floor` actor had already been deleted in phase 5, so a click hit
  nothing and `CharMoveComp.Velocity` legitimately stayed zero. That specific
  claim is retracted; the underlying input question is not — see Finding 3.

  **Finding 3 — superseded below.** The original text here reported the
  input question as "still open, and itself confounded," pending a clean-PIE
  re-test after an editor restart. That re-test has since run (full editor
  restart, standalone-window PIE via a human-pressed Play button) and the
  question is **not** resolved — it is narrower and more precisely located.
  See "The symptom, stated precisely" and "Ruled out" below, which correct
  this finding in place rather than being appended beside it.

  **Ruled out, each with its measurement:**
  - **Stale editor config** — fixed (Finding 1 stands). Cold PIE now comes up
    on `BP_StratGameMode_C` with the match seeded (`99 hexes and 10 units on
    screen`). Not the input cause.
  - **`WBP_Scoreboard` blocking mouse input** — root `VerticalBox_35` and all
    five panels are `SelfHitTestInvisible`; only ten small top-left
    TextBlocks are `Visible`. `DesiredFocusWidget = none`.
  - **Editor-process state / input-device→local-player mapping** — a **full
    editor restart** was tried (fresh process, fresh `LocalPlayer`, PIE via
    the Play button, standalone window) and a real user click on a cyan unit
    still produced no feedback and no `STRAT-CMD`.
  - **UE 5.8 input-mode filtering** — ruled out **by measurement**.
    `UEnhancedInputDeveloperSettings`: `bEnableInputModeFiltering=True`,
    `DefaultInputMode=(GameplayTags=((TagName="EnhancedInput.Modes.Default")))`,
    `DefaultMappingContextInputModeQuery` with
    `QueryTokenStream=(0,1,2,1,0)` decoding to **`AllTagsMatch{EnhancedInput.
    Modes.Default}`** — which matches the player's mode exactly.
    `ShouldFilterMappingByInputMode()`/`GetInputModeQuery()` have exactly
    **one** call site in the whole plugin,
    `EnhancedInputSubsystemInterface.cpp:978-986`, inside
    `RebuildControlMappings` — **not** at evaluation time. A PIE restart with
    `LogEnhancedInput` verified at `VeryVerbose` produced **zero**
    `"Not applying mappings from IMC"` lines while other
    `LogEnhancedInput: Verbose:` lines from the same startup printed
    normally.
  - **`UGameViewportClient::IgnoreInput()`** — ruled out by enumerating all
    four `SetIgnoreInput` call sites engine-wide; only
    `FInputModeUIOnly::ApplyInputMode` (`PlayerController.cpp:6384`) and an
    editor-module SceneState file set `true`, and `SetInputMode` appears
    **zero** times in `Source/` with zero matching nodes in any of the seven
    Strat Blueprint/Widget graphs.
  - **Input-gating calls in the project** — grep across all of `Source/` for
    `SetInputMode`, `FInputModeUIOnly/GameAndUI/GameOnly`, `DisableInput`,
    `EnableInput`, `bBlockInput`, `SetIgnoreMoveInput/LookInput`,
    `Push/PopInputComponent`, `SetPause`: **zero hits**.

  **Established correct, and should not be re-measured:**
  - All six key→action mappings are live on the player:
    `GetAll EnhancedPlayerInput EnhancedActionMappings` shows
    `BP_StratPlayerController_C_0.EnhancedPlayerInput_0.EnhancedActionMappings`
    = `IA_Select`←`LeftMouseButton`, `IA_Cancel`←`RightMouseButton`/`Escape`,
    `IA_Wait`←`W`, `IA_EndTurn`←`Enter`/`SpaceBar`.
  - `IMC_Selection` applied at `Priority: 0`; `Input Mode:
    EnhancedInput.Modes.Default`; `STATE Playing`; controller possessing
    `BP_StratCamera_C_0`; `DefaultPlayerInputClass`/
    `DefaultInputComponentClass` are `EnhancedPlayerInput`/
    `EnhancedInputComponent` in config **and** live on the actor.
  - The four `IA_*` assets are `ValueType=Boolean`, empty
    `Triggers`/`Modifiers`, `bConsumeInput=True`.

  **The symptom, stated precisely.** In a **standalone PIE window** (verified
  1286×760, title `Stratocracy Preview [NetMode: Standalone 0]`, not the
  2538×1352 editor frame), two independent devices were held for ~50 s each —
  `W` and the left mouse button — and **every** action stayed
  `None - 0.000s (false)` on the `showdebug enhancedinput` overlay, with
  `LogInput`, `LogEnhancedInput` and `LogStratPlay` silent at verified
  `VeryVerbose`. `STRAT-CMD` has **never** appeared in any log file. The
  break is upstream of `UPlayerInput`, in `UGameViewportClient::InputKey`'s
  dispatch to the local player.

  **Premises retired — recorded as corrections, they cost this phase hours:**
  1. **"The console opens, therefore keys reach the viewport and the
     game."** False as an exclusion. `GameViewportClient.cpp:767-770`: when
     `IgnoreInput()` is true the console still receives every key while the
     PlayerController receives none. The observation never had exclusionary
     power.
  2. **"`consumed=true`/`consumed=false` indicates delivery."** It does not.
     `consumed` means some Slate widget claimed the event; it was measured
     unstable between sessions for the same key.
  3. **"Simulated input bypasses Slate focus."** False for keys — Slate
     routes keys to the focused widget, so `playtest_key` returns
     `consumed=false` in an unfocused viewport and does not escape the focus
     variable.
  4. **This section's own earlier "total input silence reproduced on
     known-good assets"** — already retracted in Finding 2; its control was
     confounded.
  5. **The `showdebug enhancedinput` overlay proves the mapping context is
     applied, NOT that any handler is bound.** It is populated by the
     subsystem, independent of `BindAction`.

  **New observations, recorded as deferrals — not this milestone:**
  - **`AStratPlayerController::SetupInputComponent`
    (`StratPlayerController.cpp:123-142`) guards each `BindAction` with
    `if (Action != nullptr)` and logs nothing on the null branch.** A null
    action binds silently; only the mapping context has a warning. So "the
    four action defaults are correct" is **un-witnessed** — the log cannot
    distinguish four bindings from zero. Same half-pinned shape as the
    phase-4 findings. Owner: `strat-gameplay-engineer`.
  - **`AStratPlayerController::OnSelect` returns silently on a trace miss**
    (`StratPlayerController.cpp:198-204`), making "input never arrived" and
    "picking failed" indistinguishable in the log. This unobservability cost
    six sessions of ambiguity. Owner: `strat-gameplay-engineer`.
  - **`LogUIActionRouter: Error: Using CommonUI without a
    CommonGameViewportClient derived game viewport client. CommonUI Input
    routing will not function correctly.`** appears in every session, with
    `Found 0 derived classes`. CommonUI is a template leftover — no
    `CommonUI`/`UCommon`/`CommonActivatableWidget` match anywhere in
    `Source/`, and the three `CommonUI.*` cvars in `DefaultGame.ini:10-12`
    are leftovers too. Probably inert, but it is an `Error` on the input
    path that nobody has investigated.
  - **`LogStratPlay: Verbose: BP_StratPlayerController_C_0 could not paint an
    initial screen …: scoreboard refresh refused: there is no scoreboard
    widget to refresh`** alongside `LogStratUI: Scoreboard live on an
    adopted bridge` — the HUD has a scoreboard widget, the PlayerController
    does not. This is the resolution of the phase-6 "Finding 5" unchased
    observation below: not a screenshot-compositing artifact but a real
    widget-ownership mismatch, still unchased for cause.
  - **The screenshot pipeline excludes the UI layer**: `playtest_observe`
    and plain `HighResShot` do not composite the canvas layer; only
    `Shot showui` does. **No playtest may use a screenshot to prove a widget
    drew.** `Shot showui` captures the whole editor window in docked
    sessions and needs cropping for evidence.
  - **`playtest_start` overrides the editor's Play dropdown** and forces
    in-viewport PIE, silently ignoring `PlayMode_InEditorFloating`. Only a
    human-pressed Play button produces a standalone window. This is why the
    focus confound could not be removed by any agent-initiated session.
  - **`Escape` is the PIE-stop key** in the preview window — sending it via
    `playtest_key` ends the session.
  - **NeoStack `execute_script` discards Lua `return` values**; `print()` is
    the only output channel. `read_log('output', …)` reports the
    live-locked `Stratocracy.log` as empty while `Grep`/`Read` read it fine.
    `playtest_key` rejects `BackQuote`; `Tilde` is accepted.

  **Not yet produced:** no `assert_log_contains` clause, no on-disk evidence
  in `Tools/architect/evidence/`, and no `STRAT-CMD` line. The blocker is
  unresolved and is the milestone's one standing item.

  **Status line.** The decided next step is a **C++ diagnostic probe** —
  `UE_LOG` in an `InputKey` override plus `GetGameViewport()->IgnoreInput()`
  logged from `BeginPlay` — which is `strat-gameplay-engineer`'s lane and
  requires the editor **closed** for a build, reversing phase 6's
  editor-open invariant. That decision is the user's and has not been taken.
  Keep the scope fence: none of the deferrals above enters this milestone.

  **Update — the probe ran, found the cause, and has been stripped.**

  **The cause and fix, verified against source.**
  `AStratPlayerController` had `PrimaryActorTick.bCanEverTick = false`.
  `APlayerController::TickActor` → `TickPlayerInput` →
  `UPlayerInput::ProcessInputStack` is the *only* evaluator of Enhanced Input
  trigger state machines — the one place bound `BindAction` delegates fire.
  With the tick off, every key was still received and buffered at
  `InputKey`, every `BindAction` call still ran and logged no warning, the
  mapping context was still applied, and the console — handled upstream at
  the viewport-client layer, never through `ProcessInputStack` — still
  worked. Nothing observable pointed at the tick flag. Now `true`, set
  explicitly in the constructor rather than left to
  `APlayerController`'s own default, with the reasoning written in place
  (`Source/StratPlay/StratPlayerController.cpp:37-70`) specifically so a
  future reader who re-derives "this controller polls nothing, so it need
  not tick" — the exact reasoning that produced the bug in phase 4 — hits
  the correction before re-introducing it. **The general mechanism, not just
  the line: a `UCLASS` on the input path that disables its own tick still
  receives input and dispatches none of it, and the failure presents as a
  green build with every binding correctly configured.**
  Confirmed by reading the constructor and `SetupInputComponent` directly
  (`StratPlayerController.cpp:37-200`) rather than taken from a report.

  **The diagnostic probe is fully stripped.** `grep "STRAT-PROBE"` across
  `Source/` returns one hit, and it is inside the constructor's explanatory
  comment, not code; no `InputKey` override exists anywhere under
  `Source/StratPlay/`. The four `else` Warning branches on
  `SetupInputComponent`'s null `BindAction` guards (`StratPlayerController.cpp:165-199`)
  are confirmed **permanent** in the source's own comment block
  (`:147-154`, "THIS PART IS PERMANENT... not to be stripped with them") and
  stay.

  **Acceptance ID — confirmed, not reassigned.**
  `strat-test-author`'s `Stratocracy.StratPlay.T-UI-02.ControllerTicksSoInputDispatches`
  (`Source/StratPlay/Tests/StratPlayerControllerTick.cpp`) is **confirmed**
  over `strat-gameplay-engineer`'s proposed `T-PLAY-01`, on the same two
  grounds the test's own header states, independently re-verified here:
  `grep -c "T-PLAY"` against the GDD returns **0** (own measurement, not
  copied), and the GDD's actual `T-UI-02`
  (`Stratocracy_Prototype_GDD.md:2505`, "the reachable-hex highlight
  displays exactly the T-MOVE-01 set") is the acceptance ID StratPlay's
  existing click-to-hex-to-reach chain already uses
  (`BoardHexRoundTrip`, `ReachOverlayIsNotComputedHere`,
  `SelectionMachineUsesTheQueryNotDistance`) — the controller's tick is the
  first link in that same chain, since no click reaches the highlight
  without it. No test in `Source/StratPlay/Tests/` mints an ID, and minting
  one is not this steward's lane either; `T-UI-02` is the correct reuse.

  **Suite count, read from the report, not asserted.**
  `Saved/AutomationReport/index.json`: `reportCreatedOn 2026.08.13-15.05.30`,
  `succeeded 66 / failed 0 / notRun 0`. `ControllerTicksSoInputDispatches`
  does **not** appear in that report's test list — confirmed by direct
  search of the JSON — so the suite stands at **66/66 with the 67th clause
  compiled but not yet run**, matching the phase-6 status exactly.
  `strat-test-author` re-runs it after this steward.

  **Two further clauses owed, not built** (per the engineer's proposal,
  recorded here as owed rather than authored — writing tests is
  `strat-test-author`'s lane, not this one's):
  - `T-INT-05.WaitIsDistinguishableFromAttack` — pin that a wait yields
    `Command == None` while an accepted attack yields `Command == Attack`
    with unit and hex, read off the outcome struct (no `UE_LOG` can satisfy
    this; it is a struct-field expectation).
  - `T-INT-05.WaitWithNothingSelectedIsANoOp`.

  **Harness facts, recorded for the next agent to hit them before
  re-discovering them:**
  - `playtest_log_marker`'s `since=` filter is off by one — the marker's own
    line is excluded from what it returns.
  - The in-game console cycles **small → full → closed**, and a full-screen
    console silently swallows every keystroke.
  - `playtest_start` forces in-viewport PIE and ignores the Play dropdown;
    only a human-pressed Play button produces a standalone window.
  - `Escape` is the PIE-stop key in the preview window.

  Phase 6 evidence assembled under
  `Tools/architect/evidence/06-hotseat-playtest/` — see that directory's
  own `blackboard.md` for the full account, including a correction this
  steward made to the task's own framing (measured **13** side flips across
  the 27 `STRAT-CMD accepted` lines, not the stated 12).

### Phase 6 — CLOSED

- **Completed:** 2026-08-13. `strat-integration-reviewer` returned
  `VERDICT: PASS`, zero findings, all twelve standing constraints clean, and
  the probe strip verified complete (no `InputKey` override, no `BeginPlay`
  dump, no live `STRAT-PROBE` emitter; the single occurrence is inside an
  explanatory comment at `StratPlayerController.cpp:56`). The tick fix and
  all four `else` Warnings survived the strip. Nothing staged or committed;
  `HEAD` stayed `d310aa1` throughout — staging/committing is the user's call.
- **Exit criterion met:** "PIE playtest screenshots plus every
  `assert_log_contains` passing; full suite green; evidence assembled under
  `Tools/architect/evidence/`."
  - Suite **67/67**, `succeeded 67 / succeededWithWarnings 0 / failed 0 /
    notRun 0`, `reportCreatedOn 2026.08.13-15.29.04`, with
    `Stratocracy.StratPlay.T-UI-02.ControllerTicksSoInputDispatches` present
    **by name** and `Success` (it had compiled but not yet run as of the
    2026.08.13-15.05.30 report cited above).
  - Eight assertions pass, re-derived independently by the reviewer from the
    preserved log (`md5 baa918eade6b1061e9346c47a3b82d54`,
    `grep -c "STRAT-CMD accepted"` = 27, `refused` = 0, `Selection:` = 6).
  - Evidence under `Tools/architect/evidence/06-hotseat-playtest/`.
- **A false positive struck, not carried forward — the reviewer's reasoning
  is the keeper, not this steward's original flag.** This record previously
  flagged `ScreenShot00041.png`'s `Destroyed` row rendering `50` for side 1
  as a probable scoreboard defect, reasoning from a false premise: that the
  row is a unit count capped at 5. **It is not a count.** `Destroyed` binds
  `strat::UiSideView::fameCombat` — Fame from kills, GDD §2.11.4 — per
  `Source/StratUI/StratScoreboardWidget.cpp:214` (tooltip: *"Fame from
  kills. Factory income does not count at the cap."*), pinned by
  `Stratocracy.StratUI.T-UI-03.DestroyedBindsCombatFame`
  (`Source/StratUI/Tests/StratScoreboardParity.cpp:406-424`, including
  `TestFalse(… "Destroyed shows a bare value, not X of N")`). So
  `Destroyed 0 / 50` means side 1 held 50 combat Fame and side 0 held none —
  **independent corroboration of the user's otherwise-unlogged "a red unit
  won the encounter,"** not a contradiction of it. The misreading itself is
  the lesson: reading a bare-value row as a bounded count is what produced
  the phantom defect. Both this steward and the reviewer made that error
  once; the pinning test is what settled it. `blackboard.md` under the
  evidence directory has been corrected in place rather than superseded.
- **Cause and fix, generalised for reuse:** a `UCLASS` on the input path
  that disables its own tick still receives input and dispatches none of
  it, and the failure presents as a green build with every binding
  correctly configured. `AStratPlayerController::PrimaryActorTick.bCanEverTick`
  `false` → `true`, set explicitly in the constructor with the reasoning
  written in place (`StratPlayerController.cpp:37-70`).
- **Standing debt, left open by the reviewer's judgement — do not spend
  another phase on it.** NeoStack input injection reaches
  `UGameViewportClient::InputKey` (the in-game console provably toggles
  under simulated input) but never reaches `UPlayerInput`, so the playtest
  is **not machine-repeatable**: the eight assertions document one stored
  artifact rather than defend a repeatable gate. This is judged a NeoStack
  plugin issue outside this repository. The phase's actual regression — the
  tick flag — *is* netted repeatably, by
  `T-UI-02.ControllerTicksSoInputDispatches`.
- **Combat-outcome gap, unresolved and not this phase's job to close:** no
  destruction or encounter-result line exists in `LogStratPlay`, so the
  encounter narrative stays visual. If a later phase wants a log-backed
  outcome, it is a `FStratBridge`-routed line, not a widget one.
- **Two clauses now writable, owed to the next test-author phase** — they
  were not writable before phase 6 because the `STRAT-WAIT spent` line did
  not yet exist: `T-INT-05.WaitIsDistinguishableFromAttack` and
  `T-INT-05.WaitWithNothingSelectedIsANoOp`.
- **A wait leaves no trace in `RecordedLog()` and cannot** — the vendored
  format has no `Wait` kind; a replay reproduces state hashes but not the
  player experience.
- **`.agents/ue-project-context.md:195` drift is already fixed** — it now
  reads 67/67, hot-seat phase 6, 2026-08-13 (confirmed above, this
  steward's own edit, dated 2026-08-13). No action owed from a future
  reader; recorded here so it is not re-flagged.

## Hot-seat milestone — COMPLETE

With phase 6 closed, the hot-seat milestone is complete. Ferrum Crossing
renders, units spawn from the scenario, click-to-select → move → attack
with the deterministic forecast, wait, end turn advances sides, and the
scoreboard follows the active side.

Out-of-scope list, unchanged, stays under NEXT: production menu (§2.11.5),
guided opening (§2.11.6), info panel, toasts, save-slot UI, AI opponent,
move-undo.
