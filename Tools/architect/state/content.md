# Stratocracy state — content

> **Sole writer: `strat-editor-builder`.** Split out of `Tools/architect/state.md` on 2026-08-20, which is now
> FROZEN as the historical record of closed milestones. This directory is the live record.
>
> **Holds:** Assets, Blueprint defaults, PIE evidence, and the measured limits of the NeoStack Lua API.
>
> **One file owns each fact class.** Other files may *link* to a fact recorded here; they may not
> restate it. Only `global.md` may carry a live suite count or a phase verdict — a live `N/N`
> in any other file is a finding, enforced by `strat_banner_sweep.py`'s RECORD OWNERSHIP check.
> Everything under `## NEXT` is swept as live; stamp an entry that has become history rather
> than deleting it, exactly as `state.md` did.

## NEXT

- **`AWorldSettings` is unreachable through the NeoStack Lua API**, so no level's GameMode
  override can be authored that way. Exact failures: `configure("actor","WorldSettings")`,
  `select_actor`, `get_actor_properties`, `open_asset` on the sub-object path, and
  `invoke({actor_label="WorldSettings"})` all failed; `configure` accepts only
  `actor|landscape`. (Worked around this phase for the `Config` question by reading the two
  levels' binary `.umap` bytes directly instead — see Phase 5 below.)
- **`array_count("DefaultKeyMappings.Mappings")` → `property not found`** — dot-path into a
  struct's array is unsupported; write the whole struct instead. **Generalises to struct members,
  not just arrays** — phase C (AI-opponent milestone) hit the same failure on
  `get`/`set("self", "MatchConfig.AiSides")`, a plain (non-array) struct field, and had to read
  and rewrite the whole `MatchConfig` struct. Whole-struct read/write is the standing workaround
  for both cases.
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
