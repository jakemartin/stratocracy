---
name: stratocracy
description: Work in Source/Stratocracy — the game module, holding the UE template code and StratData/ (the DataTable row structs and ImportStratDataCommandlet). Covers why StratBridge depends on this module, which DataTable's row order is load-bearing and which is not, and the commandlet's standing /Game/ exception. Use when editing a row struct, the import commandlet, or reasoning about DT_Units / DT_Terrain order.
---

# Source/Stratocracy — the game module and StratData

## Purpose

The UE template code plus `StratData/`: the `USTRUCT` row types in `StratDataRows.h` and
`ImportStratDataCommandlet`, which authors the `/Game/StratData/` DataTables from the vendored CSVs
under `Data/` (read-only, hash-gated — never edit them).

## Why `StratBridge → Stratocracy` exists

`FUnitRow` / `FTerrainRow` bake `/Script/Stratocracy.UnitRow` into `DT_Units` and `DT_Terrain`, so
the row structs cannot move without asset surgery. The arrow is deliberate. An audit that flags it
as a layering violation has bad ground truth.

## `DT_Units` row order is load-bearing. `DT_Terrain` row order is not. Do not conflate them.

**Units.** A §4.10 Build command carries a `defIndex` in its `unitId` field, and `applyCommand` uses
it as a **raw, bounds-checked-only index** into the definitions vector — no name lookup
(`strat::applyCommand`'s `SaveCommandKind::Build` arm, which bounds-checks `c.unitId` against
`t.units->size()` and indexes with it). So `DT_Units` in a different order than the headless
loader's resolves the same replay log to a **different unit type, silently**. Row order is taken
from the table and then *asserted* equal to `strat::loadUnits` over the same vendored CSV by
`GATE-BRIDGE-DEFS` — never assumed.

The fixture carries 22 Build commands, each with a raw `defIndex`, so a mis-ordered `DT_Units`
**would** move `T-INT-02`'s canonical state hash. `GATE-BRIDGE-DEFS` is therefore not the only net
under that failure, but it remains the sharper one: it names the offending row directly instead of
surfacing as an opaque hash mismatch at the end of the replay log. Keep the test. Its rationale
comment is in `Source/StratBridge/Tests/StratBridgeParity.cpp`.

**Terrain.** Ruled not load-bearing, on evidence, phase 0 / 2026-08-12. No `SaveCommand` field
carries a terrain index (the `SaveCommand` struct in `Save.h` — its fields are
`{turn, side, kind, unitId, hex, hasHex, hasUnit}` and no terrain among them); seeding resolves
every hex's terrain **by name** (`strat::seedFromScenario`'s terrain-id resolution loop, which
matches `sc.terrainId[i]` against each loaded row's `id`); and `canonicalStateBytes` — what
`FStratBridge::StateHash()` hashes via `strat::canonicalStateHash` — emits no terrain field at all,
not even the `terrainIndex` an `Objective` carries. Terrain indices are live *within* a build and
never externalised, so any order is self-consistent.
`Stratocracy.StratData.T-DATA-05.TerrainTableMatchesCsv`'s name-keyed field parity is sufficient
and no positional terrain test is needed.

**Do not reach for `stateHash` in `Driver.h` to reason about this.** That is the debug driver's own
digest over `Session`; it *does* fold raw terrain indices (`strat::stateHash`'s accumulator appends
`s.terrain` element by element), and it is a different function over a different type. `Save.h`'s
file-header note *"THE CANONICAL STATE HASH IS NOT DEFINED HERE"* disclaims the conflation
explicitly, naming `Driver.h`'s `stateHash` as "the debug driver's own digest (GATE-DRV-06) and a
different thing". Mistaking the two makes terrain order look transitively proven when it is not
proven at all; it simply does not need to be.

**What would flip the terrain ruling:** a save format that serialised board state, or a snapshot
cached across builds. Either externalises a terrain index, and terrain would then need a
`GATE-BRIDGE-DEFS`-shaped positional test of its own.

## Gotchas

- **`ImportStratDataCommandlet` is a standing exception to the no-`/Game/`-literal rule.** It
  *authors* the `/Game/StratData/` packages, so it is the producer of the path rather than a
  consumer of it. The rule itself (`CLAUDE.md` non-negotiable 4) still binds everything else here.
- **Moving a row struct is asset surgery**, not a refactor — see the baked `/Script/` path above.
