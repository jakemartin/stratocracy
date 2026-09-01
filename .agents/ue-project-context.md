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
StratRules ──┐  (vendored; ALSO compiled into StratBridge as Vendored/*.strat.cpp,
             │   IN MODULAR BUILDS ONLY — see "Hard constraints" below)
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
| `StratBridge` | Core, CoreUObject, Engine, **`Stratocracy`**; `StratRules` as a `PublicIncludePaths` entry in EVERY target — an include edge — **PLUS a `PrivateDependencyModuleNames` link edge ON THE MONOLITHIC BRANCH ONLY**, added by `StratBridge.Build.cs` from `Target.LinkType != TargetLinkType.Monolithic`. Both branches are reasoned under "Hard constraints" below; do not derive this row for one link type and report the other as a violation | `FStratBridge` — the only code that knows both worlds. Owns the authoritative `strat::GameState`. |
| `StratUI` | Core, CoreUObject, Engine, `StratBridge`; private UMG/Slate/SlateCore | The UMG surface and the reflected view model. |
| `StratPlay` | Core, CoreUObject, Engine, `StratUI` public; **`StratBridge`, `UMG`, `EnhancedInput` private** | Gameplay actors, camera, input, the match subsystem. |

Each arrow is load-bearing and each has a recorded reason. Read the `.Build.cs` header block
before you change one.

**This table flattens dependency STRENGTH, and on `StratPlay` the strength is itself load-bearing** —
verified against `StratPlay.Build.cs` on 2026-08-14, not restated from an earlier reading.
`StratBridge` is `Private` there *deliberately*: it was moved Public → Private closing a phase-3
gate finding, because nothing in this module's headers names `FStratBridge` beyond a forward
declaration, so `Public` re-exported the arrow to every dependent for no caller's benefit. It is
named at all — rather than arriving transitively through `StratUI`, which declares it `Public` —
because of a measurement recorded in that file: **4 × `LNK2019`** on `UnrealEditor-StratPlay.dll`,
naming `__imp_?LoadDefinitions@FStratBridge@@`, `__imp_?LoadScenarioFromFile@FStratBridge@@`, and
the imported constructor and destructor. The transitive arrow carried the include paths; it did
not carry `UnrealEditor-StratBridge.lib` onto the link line. **A module that CALLS a symbol
declares the module that exports it.** Note which methods did *not* appear in that error —
`IsSeeded()` and `GetBridge()` are inline in the header and linked fine, which is exactly how the
omission would survive a smaller caller. `EnhancedInput` is likewise `Private`: `AStratPlayerController`
is the only file including an Enhanced Input header, and the six asset properties are
forward-declared `TObjectPtr`s.

**`InputCore` is absent from `StratPlay` deliberately, and one `FKey` call brings it back.**
`FKey::ToString` and `FKey::IsGamepadKey` are `INPUTCORE_API` rather than inline; omitting the
module was measured as 2 × `LNK2019` then `LNK1120`. The declarations resolve fine —
`InputCoreTypes.h` arrives transitively through `Engine` — so the failure is a *link* failure and
reads as a missing function rather than a missing module.

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

**AND THAT COMPILE-IN IS CONDITIONAL, WHICH IS NOT A DETAIL.** The reason above is a statement
about a DLL boundary, so it stops holding when there is no boundary. A **Game target is
monolithic**: every module's objects enter one executable, and the `StratRules` module's copy
of the rules is already there because `Source/Stratocracy/Stratocracy.Build.cs` depends on it.
Compiling them a second time into `StratBridge` then puts every `strat::` symbol in the binary
twice. **Measured 2026-08-31, the first Game-target build attempted during packaging: 110 ×
`LNK2005` then `LNK1169`** — while the editor build was green and the suite was 347/347, which
is exactly why no clause could see it.

So each `Vendored/*.strat.cpp` guards its single `#include` on `STRAT_VENDORED_RULES_IN_BRIDGE`,
and `StratBridge.Build.cs` defines that from `Target.LinkType != TargetLinkType.Monolithic` —
the *same* expression that adds `StratRules` to the private dependencies on the monolithic
branch, so the two halves cannot drift apart. One copy of the rules reaches the binary either
way. An **absent** definition is a `#error`, not a silent 0: without that, a dropped define
would resurface as the very `LNK2019` the shims exist to prevent, which is the worst available
disguise for their own misconfiguration.

The suite cannot cover this. It runs inside a modular editor build and cannot observe a
monolithic link, so the net is CI building the Game target — the `build the Stratocracy Game
target` step in `.github/workflows/build-and-suite.yml`.

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
(`strat::applyCommand`'s `SaveCommandKind::Build` arm, which bounds-checks `c.unitId` against
`t.units->size()` and indexes with it). So `DT_Units` in a different order than the headless loader's
resolves the same replay log to a **different unit type, silently**. Row order is taken from the
table and then *asserted* equal to `strat::loadUnits` over the same vendored CSV by
`GATE-BRIDGE-DEFS` — never assumed.

The fixture carries 22 Build commands, each with a raw `defIndex`, so a mis-ordered `DT_Units`
**would** move `T-INT-02`'s canonical state hash. `GATE-BRIDGE-DEFS` is therefore no longer the
only net under that failure — but it remains the sharper one, because it names the offending row
directly instead of surfacing as an opaque hash mismatch three hundred commands later. Keep the
test; fix the rationale. (`StratBridgeParity.cpp:158-162`.)

**Terrain.** Ruled not load-bearing, on evidence, phase 0 / 2026-08-12. No `SaveCommand` field
carries a terrain index (the `SaveCommand` struct in `Save.h` — its fields are
`{turn, side, kind, unitId, hex, hasHex, hasUnit}` and no terrain among them); seeding resolves
every hex's terrain **by name** (`strat::seedFromScenario`'s terrain-id resolution loop, which
matches `sc.terrainId[i]` against each loaded row's `id`); and `canonicalStateBytes` — what `FStratBridge::StateHash()` actually
hashes via `strat::canonicalStateHash` — emits no terrain field at all, not even the
`terrainIndex` an `Objective` carries. Terrain indices are live *within* a build and never
externalized, so any order is self-consistent. `T-DATA-05.TerrainTableMatchesCsv`'s name-keyed
field parity is sufficient and no positional terrain test is needed.

**Do not reach for `stateHash` in `Driver.h` to reason about this.** That is the debug driver's
own digest over `Session`, it *does* fold raw terrain indices (`strat::stateHash`'s accumulator
appends `s.terrain` element by element), and it is a different function over a different type —
`Save.h`'s file-header note *"THE CANONICAL STATE HASH IS NOT DEFINED HERE"* disclaims the
conflation explicitly, naming `Driver.h`'s `stateHash` as "the debug driver's own digest
(GATE-DRV-06) and a different thing".
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

### The map → GameMode binding is a single unguarded line in `Config/`

`GlobalDefaultGameMode` in `Config/DefaultEngine.ini` is the **only** thing binding
`Lvl_FerrumCrossing` to `BP_StratGameMode_C`. The level carries no World Settings GameMode
override of its own — unlike `Lvl_TopDown`, which does. So an edit to that one INI line silently
repoints the whole game, and nothing in `Content/` contradicts it.

Two consequences worth knowing before you touch it:

- **`AWorldSettings` is unreachable through the NeoStack Lua API**, so the override cannot be
  authored that way as a belt-and-braces second binding. Measured failures: `configure("actor",
  "WorldSettings")`, `select_actor`, `get_actor_properties`, `open_asset` on the sub-object path,
  and `invoke({actor_label="WorldSettings"})` all fail; `configure` accepts only `actor|landscape`.
  The workaround used previously was reading the two levels' binary `.umap` bytes directly.
- **A long-lived editor serves the config it started with.** A `Config/` edit made while the
  editor is open does not take effect, and the symptom reads as a content bug rather than a stale
  read. Restart the editor after changing this file.

This was flagged as unrecorded across two milestones — "not recorded in
`.agents/ue-project-context.md`, which is where a future config edit would most plausibly be
checked against." Recorded here on 2026-08-14, verified against `Config/DefaultEngine.ini`.

---

## Read-only territory

| Path | Why | If you need a change |
|---|---|---|
| `Source/StratRules/**` | Vendored certified bytes from `E:\MultiAgent\stratocracy-crew`, hash-gated by `T-INT-01` at the `rulesCommit` recorded in `Source/StratRules/StratRules.manifest.json`, WHICH IS THE AUTHORITY -- `96d93ea` as of 2026-08-25, and this line named `cb8e12b` through two re-vendors before that. A commit written into prose goes stale on the next re-vendor and nothing recomputes it, so read the manifest rather than this sentence | Fix upstream in `stratocracy-crew`, re-vendor, update the manifest. File the request; do not hand-edit. |
| `Data/**` | Vendored, hash-gated by `GATE-DATA-VENDOR` at the `dataCommit` recorded in `Data/StratData.manifest.json`, WHICH IS THE AUTHORITY -- `c2f5860` as of 2026-08-25. That figure is CURRENT, and it is rewritten anyway: the row above went stale through two re-vendors while reading exactly as confidently as this one, so a quoted commit that happens to be right is the same defect not yet triggered | Same escalation path. |

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
number here. Last observed here: 140/140, 2026-08-21, after
`T-INT-05.BothSidesAiReachesAResultOnDifferentContent`, the content-independence corpus (103 at
combat-outcome phase 3 → 104 at the `accepted` tightening → 106 at the two `STRAT-CMD refused`
clauses → 107 at the foreign-model guard → 108 → 133 at Lane B's save-slot I/O and the
§2.11.6 guided opening, +25 clauses at `1d6f758`).

**That report is UTF-8 with a BOM** — first bytes `EF BB BF`. Measured in the hot-seat milestone's
phase 4 (not the combat-outcome milestone's, which is the AI-vs-AI PIE run), after two
agents reported it as UTF-16: all three UTF-16 codecs fail on it and `utf-8-sig` succeeds. A
reader that hardcodes the wrong codec sees zero tests and may not say so — the same silent-zero
shape as the bare `"Stratocracy.uproject"` test command below.

**Any `FOutputDevice` that captures `GLog` inside a clause MUST override
`CanBeUsedOnMultipleThreads()` to return `true`.** Measured 2026-08-14, after a clause failed
1 run in 4 on byte-identical code while passing in isolation. `FOutputDeviceRedirector` queues
lines it cannot broadcast on the primary-thread fast path, and `FlushBufferedItems` drains that
queue to whichever devices sit in `BufferedOutputDevices` **at drain time**
(`OutputDeviceRedirector.cpp:553`) — so a capture registering mid-stream inherits the queue's
tail, and a `GLog->Flush()` in the capture's own settle step is itself enough to trigger the
drain. The override routes the device to `UnbufferedOutputDevices` (`:440-447`), fed only by the
synchronous broadcast inside the emitting `UE_LOG` (`:905`), so the window it sees equals the
object's lifetime by construction rather than by timing. Engine precedent:
`AutomationTest.h:1345`, `:1396`. All four captures in `Source/StratPlay/Tests/` carry it; a
fifth without it reintroduces the flake. Note the lock guards the append only — every read of the
line array is unlocked and game-thread-only, which is safe solely because every `STRAT-*` emitter
is on the game thread, and **nothing pins that**.

### Build

```
"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" StratocracyEditor Win64 Development -project="E:\MultiAgent\Stratocracy\Stratocracy.uproject" -waitmutex
```

**`Build.bat` is NOT on `PATH`, and this is measured** (2026-08-14, `command -v Build.bat` →
nothing; the file exists only at the Engine path above). This block previously recorded the bare
`Build.bat …` form as if it were runnable, and every agent handed that form has silently worked
around it — one by `cd`-ing to `Engine\Build\BatchFiles`, another by reporting
`The term 'Build.bat' is not recognized as the name of a cmdlet, function, script file, or
operable program` and then recovering. The cost was invisible because the workaround always
succeeded, so nothing was ever written down. Same failure class as the bare `"Stratocracy.uproject"`
trap below: a command recorded as verbatim that does not run as written. From PowerShell the full
path needs the call operator (`& "C:\…\Build.bat" …`); from the Bash tool it does not.

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

**`-NoHotReloadFromIDE` IS NOT AN EXEMPTION FROM THIS, and it looks like one.** That flag defeats
the Live Coding **mutex**, which is engine-keyed and machine-wide and is what blocks a build in a
*linked worktree* (a worktree has its own `Binaries/`, so the mutex is the only thing in its way).
It does NOT defeat the **write lock** here: the running `UnrealEditor.exe` has this tree's
`Binaries\Win64\*.dll` mapped and no UBT switch unmaps them. Passing it on THIS tree makes the
failure later and stranger rather than removing it -- measured 2026-08-22: every compile action
succeeds, including both `.gen.cpp` files, so UHT has parsed everything and a reflection error is
NOT what you are looking at; then the links fail, first as
`UbaSessionServer - ERROR opening file ... for write after retrying for 20 seconds` and then, on
the non-UBA retry, as `LINK : fatal error LNK1104: cannot open file ...UnrealEditor-<Module>.dll`.
The fix is still *close the editor*.

---

## Documents and where they live

| What | Where | Note |
|---|---|---|
| The GDD | `E:\MultiAgent\stratocracy-content\Stratocracy_Prototype_GDD.md` | **446 KB with ~100 KB lines. Grep it or slice it — never `cat` it.** |
| GDD snapshot | `Tools/architect/gdd_snapshot/` | A copy with a `MANIFEST.md` sha256. Explicitly *not* hash-gated; can go stale silently. |
| Current build state | `Tools/architect/state/` | The live record, one file per owning agent: `global.md` (banner, milestone status, and the only file that may state a suite count), `decisions.md`, `engine.md`, `tests.md`, `content.md`, `data.md`. |
| Closed-phase history | `Tools/architect/state.md` | FROZEN 2026-08-20. The recorded reasoning behind fourteen closed phases. Not swept, not edited. |
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
