# Stratocracy — project context

Canonical architecture statement for this repository. Every agent reads this first, and the
`ue-*` skills pick it up automatically to tailor their advice to *these* module arrows rather
than to generic Unreal practice.

**Trust order: when this file disagrees with the working tree, the tree wins.** Fix this file.

---

## The project

Turn-based hex strategy, descended from *Conflict* (NES, 1989). Unreal Engine **5.8**, C++17,
Win64. `BuildSettingsVersion.V7`, `EngineIncludeOrderVersion.Unreal5_8`.

Shipped scenario is **Ferrum Crossing** — an 11×9 board, 99 hexes, 7 terrain kinds
(Plains, Woods, Mountains, Water, Town, Bridge, Factory), 10 units (5 per side), 8 capturables,
`turnCap 20`, `startingFame 200/200`.

The rules are *deterministic and headless*. The engine never decides an outcome; it asks.

---

## Modules and the arrows between them

```
StratRules ──┐  (vendored; ALSO compiled into StratBridge as Vendored/*.strat.cpp)
             │
Stratocracy ─┴──▶ StratBridge ──▶ StratUI ──▶ StratPlay
                        └────────────────────────┘
                     (StratPlay also depends on StratBridge directly —
                      measured, see "Hard constraints" below)
```

| Module | Depends on | Holds |
|---|---|---|
| `StratRules` | `Core` only | Vendored pure C++17 `namespace strat`. Zero engine deps. |
| `Stratocracy` | Core, CoreUObject, Engine, InputCore, EnhancedInput, …, `StratRules` | Game module: UE template code + `StratData/` row structs and the import commandlet. |
| `StratBridge` | Core, CoreUObject, Engine, **`Stratocracy`** | `FStratBridge` — the only code that knows both worlds. Owns the authoritative `strat::GameState`. |
| `StratUI` | Core, CoreUObject, Engine, `StratBridge`; private UMG/Slate/SlateCore | The UMG surface and the reflected view model. |
| `StratPlay` | …, `StratBridge`, `StratUI`, `EnhancedInput` | Gameplay actors, camera, input, the match subsystem. |

Each arrow is load-bearing and each has a recorded reason. Read the `.Build.cs` header block
before you change one.

**`StratBridge → Stratocracy` is deliberate, not a mistake.** `FUnitRow` / `FTerrainRow` bake
`/Script/Stratocracy.UnitRow` into `DT_Units` and `DT_Terrain`, so the row structs cannot move
without asset surgery. An audit that flags this arrow as a layering violation has bad ground
truth — it is correct as drawn.

**`StratUI` deliberately does NOT depend on `Stratocracy`.** It binds to the projected view
model, not to DataTable rows. `StratUI.Build.cs` records this under "NOTE ON WHAT IS NOT HERE".

**New modules must be registered in `Stratocracy.uproject`'s `Modules` array.** A module that
no C++ depends on — one reached only from asset data, like a GameMode Blueprint — will not be
built by UBT otherwise, and the failure presents as *"my classes don't appear in the editor"*
alongside a **green** build.

**With one exception, and it is not optional: a directory with no `IMPLEMENT_MODULE` must NOT be
listed.** `Source/StratRules/` is deliberately absent from that array — it holds vendored C++ that
declares no module object, and listing it once **made the editor abort at startup** with *"The game
module 'StratRules' could not be successfully initialized"*. Recorded at `StratBridge.cpp:10-14`.
Before adding a directory to `Modules`, confirm it carries `IMPLEMENT_MODULE`. An audit that flags
`StratRules` as an unregistered module has this backwards.

---

## Hard constraints, with their measurements

### Never call a `strat::` function outside `Source/StratBridge/` or `Source/StratRules/`

An editor target is a modular build: each module is a DLL, and Unreal exports only symbols
carrying an `_API` macro. The vendored `strat::` sources carry none — GDD §4.9 forbids them
engine headers — so no other module can link a call to them.

**Measured: 8 × `LNK2019` the first time the bridge tried** (recorded in `StratBridge.h`). The
rules sources are therefore compiled *into* `StratBridge` via `Vendored/*.strat.cpp`, and the
exported surface is `FStratBridge`, which was written to be exported.

*Naming* a `strat::` type in a `.cpp` is legal — `StratScoreboardParity.cpp` does it. *Calling*
a `strat::` free function outside those two directories is a link error. When you need a rules
answer elsewhere, add a method to `FStratBridge` and route through it.

### Never let UHT see the vendored headers

`StratBridge.h` declares no reflected types **on purpose**, so the vendored `strat` headers it
includes are never put in front of the header tool. Any header that declares a `UCLASS` /
`USTRUCT` / `UENUM` must therefore **never** `#include "StratBridge.h"`. Forward declare
`FStratBridge` and include it only from the `.cpp`.

`StratScoreboardHUD.h` and `StratScoreboardWidget.h` both refuse that include for this reason.

### Hold `FStratBridge` as `TPimplPtr`, never `TUniquePtr`

`TUniquePtr` instantiates its deleter wherever the owning destructor is instantiated — and
declaring the owner's constructor and destructor out-of-line is **not** sufficient, because UHT
emits a vtable-helper constructor into `Module.<Name>.gen.cpp`, a translation unit that includes
the header and never includes `StratBridge.h`.

**Measured: error `C4150`, "deletion of pointer to incomplete type 'FStratBridge'"**, reported
from `UniquePtr.h` and compiled from the `.gen.cpp`. `TPimplPtr` type-erases the deleter at
binding time, which is exactly the fix.

### A Blueprint deriving from a C++ class bakes `/Script/Module.Class` irreversibly

Deciding a widget's or actor's module *after* an asset derives from it costs asset surgery.
**Before creating any asset from a C++ parent, confirm which module declares that parent.**

### `PCHUsage = NoPCHs` and `bUseUnity = false` on every module that reaches the vendored headers

`StratRules`, `StratBridge`, `StratUI` — and any new module whose `.cpp` files include
`StratBridge.h`. Sharing the engine PCH would put the vendored headers in a compile environment
whose warning settings the module does not control. If a later pass measures that a shared PCH
is safe, record the measurement — do not drop the setting quietly.

### No widget-side arithmetic

Every number a widget draws equals **exactly one** `strat::UiSnapshot` field. No sums, no
ratios, no derived values. This is `T-UI-03`'s defining clause, and it is what keeps the screen
and the rules from drifting.

### `DT_Units` row order is load-bearing. `DT_Terrain` row order is not. Do not conflate them.

**Units.** A §4.10 Build command carries a `defIndex` in its `unitId` field, and `applyCommand`
uses it as a **raw, bounds-checked-only index** into the definitions vector — no name lookup
(`Replay.good.cpp:486-487`). So `DT_Units` in a different order than the headless loader's
resolves the same replay log to a **different unit type, silently**. Row order is taken from the
table and then *asserted* equal to `strat::loadUnits` over the same vendored CSV by
`GATE-BRIDGE-DEFS` — never assumed.

The fixture carries 22 Build commands, each with a raw `defIndex`, so a mis-ordered `DT_Units`
**would** move `T-INT-02`'s canonical state hash. `GATE-BRIDGE-DEFS` is therefore no longer the
only net under that failure — but it remains the sharper one, because it names the offending row
directly instead of surfacing as an opaque hash mismatch three hundred commands later. Keep the
test; fix the rationale. (`StratBridgeParity.cpp:158-162`.)

**Terrain.** Ruled not load-bearing, on evidence, phase 0 / 2026-08-12. No `SaveCommand` field
carries a terrain index (`Save.h:59-68`); seeding resolves every hex's terrain **by name**
(`Replay.good.cpp:299-308`); and `canonicalStateBytes` — what `FStratBridge::StateHash()` actually
hashes via `strat::canonicalStateHash` — emits no terrain field at all, not even the
`terrainIndex` an `Objective` carries. Terrain indices are live *within* a build and never
externalized, so any order is self-consistent. `T-DATA-05.TerrainTableMatchesCsv`'s name-keyed
field parity is sufficient and no positional terrain test is needed.

**Do not reach for `stateHash` in `Driver.h` to reason about this.** That is the debug driver's
own digest over `Session`, it *does* fold raw terrain indices (`Driver.good.cpp:494`), and it is
a different function over a different type — `Save.h:12-16` disclaims the conflation explicitly.
Mistaking the two makes terrain order look transitively proven when it is not proven at all; it
simply does not need to be.

**What would flip the terrain ruling:** a save format that serialized board state, or a snapshot
cached across builds. Either externalizes a terrain index, and terrain would then need a
`GATE-BRIDGE-DEFS`-shaped positional test of its own.

### Never write a `/Game/` path literal into *gameplay* C++

Asset references are `EditDefaultsOnly` `TObjectPtr` properties set on a Blueprint default, not
`ConstructorHelpers` string lookups. A renamed asset must not be a compile-time break.

Two places legitimately carry the literal, because neither has a Blueprint default to hold a
property and both fail loudly rather than silently: **automation tests** loading a fixture
(`LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"))`, as the existing parity tests do) and
**`ImportStratDataCommandlet`**, which *authors* those packages and is the producer of the path
rather than a consumer of it. The rule binds gameplay and widget code. There are zero
`ConstructorHelpers` asset lookups in this tree; keep it that way.

---

## Read-only territory

| Path | Why | If you need a change |
|---|---|---|
| `Source/StratRules/**` | Vendored certified bytes from `E:\MultiAgent\stratocracy-crew`, hash-gated by `T-INT-01` at `rulesCommit cb8e12b` | Fix upstream in `stratocracy-crew`, re-vendor, update the manifest. File the request; do not hand-edit. |
| `Data/**` | Vendored, hash-gated by `GATE-DATA-VENDOR` at `dataCommit c2f5860` | Same escalation path. |

A hand-edit to either breaks a gate that exists precisely to catch it. `.claude/settings.json`
denies writes to both.

---

## Tests

Location `Source/<Module>/Tests/*.cpp`. Macro `IMPLEMENT_SIMPLE_AUTOMATION_TEST`. Flags
`EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter`.

Naming: **`Stratocracy.<Module>.<AcceptanceID>.<Clause>`** — e.g.
`Stratocracy.StratUI.T-UI-03.TurnAgainstCap`.

**Never compute the expectation side of a comparison when a module-side value exists to read.**
A test that derives what it then asserts is testing itself. `StratScoreboardParity.cpp` is the
reference shape.

The count moves every phase and goes stale the moment it is restated — read it from
`Saved/AutomationReport/index.json` (`succeeded` / `failed` / `notRun`) rather than trusting a
number here. Last observed here: 67/67, hot-seat phase 6, 2026-08-13.

**That report is UTF-8 with a BOM** — first bytes `EF BB BF`. Measured in phase 4, after two
agents reported it as UTF-16: all three UTF-16 codecs fail on it and `utf-8-sig` succeeds. A
reader that hardcodes the wrong codec sees zero tests and may not say so — the same silent-zero
shape as the bare `"Stratocracy.uproject"` test command below.

### Build

```
Build.bat StratocracyEditor Win64 Development -project="E:\MultiAgent\Stratocracy\Stratocracy.uproject" -waitmutex
```

### Headless test run

```
"C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "E:\MultiAgent\Stratocracy\Stratocracy.uproject" -ExecCmds="Automation RunTests Stratocracy;Quit" -unattended -nopause -nosplash -nullrhi -ReportExportPath="E:\MultiAgent\Stratocracy\Saved\AutomationReport"
```

**Both paths must be absolute, and this is measured.** A bare `"Stratocracy.uproject"` makes the
engine exit in about a second with *"Project file not found"* / *"Could not find a valid project
file, the engine will exit now"* — before automation starts, and without writing a log, because
it dies ahead of log init. `cd`-ing to the repo root does not help: the engine resolves a bare
`.uproject` against its own install and search paths, not the process working directory. For the
same reason a relative `-ReportExportPath` lands the report under
`C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\Saved\`, not in the repo.

A run that reports "Project file not found" has run **zero** tests. Do not read that as a pass.

### The editor holds a write lock on the module DLLs

`Build.bat` **fails while the editor is open** — a link error naming
`UnrealEditor-Strat*.dll` and "cannot open for writing" means *close the editor*, not *fix the
code*. Live Coding does not support new `UCLASS`es, so every new reflected class costs a full
close → build → reopen cycle. Batch new classes into one pass rather than adding them one at a
time.

---

## Documents and where they live

| What | Where | Note |
|---|---|---|
| The GDD | `E:\MultiAgent\stratocracy-content\Stratocracy_Prototype_GDD.md` | **446 KB with ~100 KB lines. Grep it or slice it — never `cat` it.** |
| GDD snapshot | `Tools/architect/gdd_snapshot/` | A copy with a `MANIFEST.md` sha256. Explicitly *not* hash-gated; can go stale silently. |
| Current build state | `Tools/architect/state.md` | BUILT / DECISIONS / NEXT. |
| Evidence | `Tools/architect/evidence/NN-slug/` | Each with a `blackboard.md`. |
| Session notes | `Tools/context/*.md` | Gitignored export of the desktop memory store. Flagged stale in places. |
| Headless rules source | `E:\MultiAgent\stratocracy-crew` | Canonical `cpp_reference/`, `spec/`, and the re-vendor scripts. |

---

## House style for code

Every `.Build.cs` and every non-trivial header opens with a prose block that explains **why**,
cites the GDD section and acceptance IDs it serves, and records measurements as measurements
("measured as 8 × LNK2019", not "this can cause link errors"). Decisions that are irreversible
say so. Debts that are being taken on deliberately are written down rather than quietly owned —
see `StratScoreboardHUD.h`, which states in advance that it must give up bridge ownership once a
proper owner exists.

Naming: modules `Strat*`; types `FStrat*` / `UStrat*` / `AStrat*` / `EStrat*`; vendored rules in
`namespace strat` with lowerCamelCase free functions; assets `DT_*`, `WBP_*`, `BP_*`, `SM_*`,
`Lvl_*`.

Git: LF enforced by `.gitattributes` under `core.autocrlf=true`. Diagnose phantom CRLF drift
with `git diff --ignore-cr-at-eol --numstat`. `*.uasset` / `*.umap` are LFS.
