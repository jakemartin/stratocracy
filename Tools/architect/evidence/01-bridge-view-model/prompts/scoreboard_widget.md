# SYSTEM

You are a code-generation step inside a goal-oriented agent working on Stratocracy, a
turn-based hex strategy game in Unreal Engine 5.8 and C++17.

You write source files. You do not choose what to build -- a priority scorer already
did that -- and you do not decide whether your output is accepted; a human reviews
every file before it lands.

HOUSE STYLE, matched from the files you are shown rather than invented:

- Comments explain WHY, not what. This project's headers carry long reasoning blocks
  naming the alternative that was rejected and the reason. Match that register.
- Never claim something was "measured" or "verified" unless the fact is stated in the
  source you were given. An invented measurement is worse than no comment.
- Tabs for indentation in .cpp/.h under Source/, matching the existing files.
- No hardcoded gameplay values. Anything the scenario or the data tables supply is
  READ from them. This is a graded property of the work, not a preference.
- Unreal conventions: `U`/`F`/`E` prefixes, `UPROPERTY`/`UFUNCTION` where reflection is
  needed, `TEXT()` around literals, `FString`/`TArray` over std types at engine-facing
  boundaries.

OUTPUT FORMAT -- exactly this, once per file, and nothing outside the blocks except a
short plain-text rationale before the first block:

=== FILE: Source/Some/Path.h ===
<the complete file contents>
=== END FILE ===

Emit COMPLETE files. Never abbreviate with "// ... rest unchanged ...".


# USER

GOAL: Live standings scoreboard widget (§2.11.4)

WHY THIS ONE: the priority scorer selected it. §4.11 row 8: what the row's flip waits on are 'the real Stratocracy widgets those IDs assert against, which are measured absent at a13626f'. T-UI-03 names the live standings scoreboard as its subject.

WHERE IT LANDS: Source/StratUI/ + Source/StratUI/Tests/

ACCEPTANCE IDs THIS MUST SATISFY (the GDD's own words):

T-UI-03: the live standings scoreboard (§2.11, §2.8) binds 1:1 to snapshot fields — enemy strength destroyed, objectives held X/N, surviving units/HP, turn vs cap — with no widget-side arithmetic

WHAT TO PRODUCE: three complete files.
  1. Source/StratUI/StratScoreboardWidget.h
  2. Source/StratUI/StratScoreboardWidget.cpp
  3. Source/StratUI/Tests/StratScoreboardParity.cpp -- the Automation test

THE BINDING RULE, which is the whole of T-UI-03. The widget displays values
it was HANDED. It performs no arithmetic over snapshot fields: no summing,
no differencing, no percentage. If a displayed number is not equal to one
snapshot field, it is computed on the bridge side and handed over as a
field of the row struct -- never in the widget.

The chevron is a COMPARISON, not arithmetic, and it is still computed
outside the widget: expose it as a field on the row struct. Do NOT add any
field to `strat::UiSnapshot` -- T-UI-05 clause (c) fails a snapshot field
that has no contract entry, and that struct is vendored anyway.

THE TEST must follow the existing Automation pattern exactly:
`IMPLEMENT_SIMPLE_AUTOMATION_TEST`, flags `EditorContext | EngineFilter`,
names of the form `Stratocracy.StratUI.T-UI-03.<Clause>`, loading
`/Game/StratData/DT_Units`, `/Game/StratData/DT_Terrain` and
`Data/ferrum_crossing.json`. It must assert, clause by clause, that every
displayed value equals the snapshot field it names -- read from the
snapshot, never recomputed in the test.

It must ALSO run `strat::uiCheckSnapshotFidelity` over the bridge's own
projection and fail on any reported failure. That is what stops this widget
faithfully displaying a projection that lies.

----- GDD §2.11.4 -- the scoreboard's rules, verbatim -----
#### 2.11.4 The live Fame scoreboard

The scoreboard exists because of revision §1.5-#1: the tiebreak must never be a hidden win condition. It is persistent (top-left), compact, and its **rows are ordered top-to-bottom in exact tiebreak order (§2.8)** — the layout *is* the rule, read passively all match:

```
+---------------------------+
| TURN 12 / 20              |
|---------------------------|
|            YOU      ENEMY |
| Destroyed  450      600 ◀ |
| Objectives 4/8      3/8   |
| Unit HP    47       55    |
+---------------------------+
```

- **Turn counter** against the cap, always. (`/ 20` is *Ferrum Crossing*'s cap, §2.13.2; the cap is per-scenario data, so the widget reads `turnCap` from the scenario rather than hardcoding a number.)
- **Destroyed** = combat Fame earned (kills and the flag bonus, §2.7; there is no undamaged-strike bonus — Q6, §4.7) — **passive income is excluded**, exactly as the tiebreak excludes it. Hover tooltip: `Fame from kills. Factory income does not count at the cap.` This row deliberately does *not* equal the spendable Fame pool (top-right widget), and the tooltip on each names the difference — the one place the single-currency design (§2.7) needs a disambiguating sentence.
- **Objectives** as *X of N* over all factories + capturable towns (§2.8 criterion 2), N supplied by the scenario (§2.13) — **N = 8** on *Ferrum Crossing* (4 factories + 4 towns), as the mock shows.
- **Unit HP** = surviving strength (criterion 3), listed last because it *is* last.
- A **chevron (◀)** marks the current attrition-tiebreak leader, evaluated in criteria order, and flips visibly when the lead changes. It is drawn beside the leading side's value — in the mock above the enemy leads at criterion 1, 600 combat Fame to 450 (§2.8: higher wins), so the chevron sits on the enemy column. If both Destroyed values are zero, the chevron is replaced by `— no engagements —` spanning the row: the mutual-passivity draw (§2.8) made visible before it bites.
- **Cap-approach banners** (transient, once each): at cap−5, `5 turns to the cap. The scoreboard decides a capped match.`; additionally, if both sides are still at zero combat Fame, `No engagements. A capped match with no combat is a draw.`

**End-of-match screen.** The result is the tier first (§2.8 — Decisive / Marginal / Draw), then the same three rows in the same order, so the verdict is always a restatement of what was on screen all match. Beneath the tier, one **faction-voiced result line**, written to the setting and voice guide (`kb/setting.md`), which supplies all three constraints on it: faction voice appears only in result-screen text, a result line is **≤ 30 words**, and the register is field-manual plain — terse tactical briefing, substance over drama, no melodrama or fantasy filler. The same file's two faction blocks supply the voices the samples below are written in — the **Directorate** cold, doctrinal, bureaucratic-military, framing every outcome as a matter of record; the **Vanguard** terse, pragmatic, defiant, measuring everything by ground held — and its pipeline note retrieves a faction block *only* for this screen, which is why faction voice appears nowhere else in the UI. Samples, one per case, generated content to follow these:

- Directorate, decisive: `Command directive fulfilled. The enemy flag is struck from the record. Order is restored.`
- Directorate, marginal: `The cap is reached. The ledger favors the Directorate. The record stands.`
- Vanguard, decisive: `Their flag is down. We hold the ground. That's the whole report.`
- Vanguard, marginal: `Cap hit. We did the damage; they held the rear. The ground says we win.`
- Draw, neutral system voice: `Turn cap reached. Attrition equal. Recorded as a draw.` / mutual passivity: `Turn cap reached. Neither side engaged. Recorded as a draw.`

---

#### 2.11.5 Production menu & match-flow surfaces

**Production menu** — opens on LMB on an own factory (or `B`), anchored beside it:

```
+--------------------------------------+
| FACTORY — BUILD          Fame: 250   |
|--------------------------------------|
|  Infantry   100   [Build]            |
|  Recon      150   [Build]            |
|  Artille


----- Source/StratBridge/StratBridge.h -- the view-model surface you bind to (READ ONLY, current state of the tree) -----
// GDD §4.9 part 2 -- the bridge: the only code that knows both worlds.
//
// It lives in its own `StratBridge` module and NOT in Source/StratRules/, which
// T-INT-01 requires to contain nothing unaccounted for against the recorded crew
// commit. Nothing here is vendored; nothing here is a rule.
//
// WHY ITS OWN MODULE RATHER THAN THE GAME MODULE, which is where this file first
// sat: an editor target is a MODULAR build, so each module is a DLL, and Unreal
// exports only symbols carrying an `_API` macro. The vendored `strat::` sources
// carry none -- §4.9 forbids them engine headers -- so no other module can link a
// call to them, measured as 8 x LNK2019 the first time this bridge tried. The
// rules sources are therefore compiled INTO this module (see Vendored/), and the
// exported surface is `FStratBridge`, which was written to be exported. See
// StratBridge.Build.cs.
//
// WHAT THIS OWNS: the authoritative `strat::GameState`. §4.1's "never own rules" is
// structural here rather than aspirational -- actors and UMG hold no rules state,
// because the state is in this object and they have no copy of it to drift from.
//
// WHAT THIS DECIDES: nothing. Every refusal below is a refusal the rules module
// returned, forwarded with that module's own reason. The bridge maps engine types
// to module types and carries answers back; where it would have to choose, it
// refuses instead and says why. The view-model surface added below is held to the
// same line: it GATHERS a `UiWorld` and hands it to `strat::buildUiSnapshot`, and
// computes no field of the result itself.
//
// NOT A UOBJECT, and deliberately. UHT parses headers that declare reflected types,
// and this one declares none, so the vendored `strat` headers it includes are never
// put in front of the header tool. That constraint is why `MakeUiSnapshot` hands
// back a `strat::UiSnapshot` by reference parameter rather than a USTRUCT: a
// reflected mirror of the view model would have to be declared in a header UHT
// reads, and that header could then never include Ui.h. The UMG-facing wrapper that
// copies fields out of the snapshot into reflected types is a later, separate file;
// the state does not move out of here when it arrives.
//
// NOT IN THIS ROUND: the ordered event list (§4.9 part 2's "command in / events
// out") and the widgets. T-INT-02, T-INT-03 and T-SAVE-06 assert over the state
// hash and the rejection channel, which is what this file provides. The event list
// is ruled to live headless, and no acceptance ID names it as its subject.
#pragma once

#include "CoreMinimal.h"

// Vendored rules headers, by short name via StratRules' PublicIncludePaths.
#include "Data.h"
#include "Replay.h"
#include "Save.h"
#include "Scenario.h"
#include "Ui.h"

class UDataTable;

// A refusal carries the reason the owning module gave. `Id` is that module's own
// failing-invariant tag where it supplied one (e.g. "T-SAVE-05"), empty otherwise.
struct FStratResult
{
	bool    bOk = false;
	FString Id;
	FString Reason;

	static FStratResult Ok() { FStratResult R; R.bOk = true; return R; }
	static FStratResult Fail(const FString& InReason, const FString& InId = FString())
	{
		FStratResult R;
		R.bOk = false;
		R.Id = InId;
		R.Reason = InReason;
		return R;
	}
};

class STRATBRIDGE_API FStratBridge
{
public:
	// ---- Load ------------------------------------------------------------
	// §4.9 part 2's Load bullet: FUnitRow / FTerrainRow -> strat::UnitDef /
	// strat::TerrainDef. The DataTables are the source, per that bullet.
	//
	// ORDER IS LOAD-BEARING AND IS NOT DECORATION. `defIndex` indexes these
	// vectors, and a §4.10 Build command carries a defIndex in its `unitId`
	// field. A vector built in a different order than the headless loader's
	// would resolve the same log to a different unit type and diverge silently
	// -- so the row order is taken from the table and then ASSERTED equal to
	// strat::loadUnits over the same vendored CSV by the editor pass, rather
	// than assumed here.
	FStratResult LoadDefinitions(const UDataTable* UnitTable, const UDataTable* TerrainTable);

	// Loads the shipped scenario file's text and seeds the authoritative state
	// through strat::seedFromScenario -- the single scenario -> GameState
	// mapping, shared with the headless side so neither can drift from it.
	//
	// The parsed scenario is RETAINED past this call, which it was not before.
	// See `LoadedScenario` below for why the view model cannot be built without
	// it.
	FStratResult LoadScenarioFromFile(const FString& ScenarioFilePath, int32 FirstSide);

	// ---- Command in ------------------------------------------------------
	// Applies one command through the rules module. An invalid command returns
	// a rejection reason and changes nothing (§4.9).
	//
	// APPLIED DIRECTLY, NOT TO A COPY. Copying here and assigning on success
	// would make T-INT-03's "no partial application" clause true of the bridge
	// while hiding whether it is true of the rules module -- the gate would
	// then assert a property of this file rather than of the thing under test.
	FStratResult Submit(const strat::SaveCommand& Command);

	// Replays a whole log, all-or-nothing (T-SAVE-05's property, the module's).
	FStratResult ReplayLog(const TArray<strat::SaveCommand>& Log);

	// ---- Queries ---------------------------------------------------------
	// §4.10's canonical state hash, computed by the rules module.
	FString StateHash() const;

	const strat::GameState& State() const { return GameState; }
	bool IsLoaded() const { return bDefinitionsLoaded; }
	bool IsSeeded() const { return bSeeded; }

	const std::vector<strat::UnitDef>& UnitDefs() const { return Units; }

	// Borrowed and const, the way every rules module takes what it does not own.
	strat::RulesTables Tables() const;

	// The scenario this bridge last seeded from. Meaningful only while
	// IsSeeded(); a bridge that never seeded, or whose definitions were reloaded
	// underneath it, holds a default-constructed one.
	const strat::Scenario& ScenarioData() const { return LoadedScenario; }

	// ---- View model ------------------------------------------------------
	// §4.7 Stub 8's projection input, GATHERED and not decided -- the same three
	// module-side sources §4.9 names, and nothing else: the authoritative
	// `GameState`, the §4.8 tables, and the loaded scenario file.
	//
	// WHY THIS EXISTS AT ALL: `strat::buildUiSnapshot` takes a `strat::UiWorld`,
	// and the only other function in the tree that produces one is the headless
	// driver's `uiWorldOf(const Session&)`. A Session is the driver's own type
	// and this object does not have one, so before this method no engine-side
	// caller could reach the view model. This mirrors `uiWorldOf` field for
	// field so the two projections cannot disagree; where the driver had to
	// hand-roll a derivation because `DriverUnit` is not a `GameUnit` (the flag
	// designation, the board), this calls the module function instead, because
	// we DO hold a `GameState` and a second copy of a derivation is exactly the
	// drift §4.9 is trying to prevent.
	//
	// BORROWS, DOES NOT OWN. The returned world's `unitDefs`, `terrain`,
	// `economy`, `turn` and `guided` all point into this object. It is a
	// short-lived argument for the projection call below, not something to
	// store: outliving the bridge, or surviving a LoadDefinitions, dangles it.
	// Prefer MakeUiSnapshot, which never lets the world escape.
	//
	// Valid only when IsSeeded(). On an unseeded bridge it faithfully projects
	// an empty state -- no hexes, no units -- rather than inventing one.
	strat::UiWorld MakeUiWorld() const;

	// The ONLY projection (§4.7 Stub 8). It delegates to the rules module and
	// performs NO arithmetic of its own -- not a sum, not a count, not a
	// derived flag. Every field of the result is the module's answer; if a
	// number here is wrong, it is wrong in Ui.cpp and T-UI-05 is the gate that
	// says so, which stops being true the moment this file computes anything.
	//
	// Returns a refusal rather than an empty snapshot when there is nothing to
	// project, so a caller cannot mistake "not loaded yet" for "a match with no
	// units in it".
	FStratResult MakeUiSnapshot(strat::UiSnapshot& OutSnapshot) const;

private:
	// Owned by value: this object IS the authoritative state.
	strat::GameState GameState;

	// The §4.8 tables, mapped once and then borrowed by every call.
	std::vector<strat::UnitDef>    Units;
	std::vector<strat::TerrainDef> Terrain;

	// RETAINED, where it used to be a local that died at the end of
	// LoadScenarioFromFile. `UiWorld::guided` has no other source in the tree:
	// the guided seats are scenario-file input, `seedFromScenario` does not copy
	// them into `GameState` (they are not match state -- `scenarioHash` pins
	// them), and `isGuidedMarked` is silently false for every unit when the
	// pointer is null. Dropping the scenario therefore did not fail loudly, it
	// unmarked the one unit beat 1a is about. Held whole rather than as a
	// copied-out guided list because the file is the source and a second
	// extraction is a second thing that can disagree with it.
	strat::Scenario LoadedScenario;

	// Assembles the combat stat block for one unit exactly as the driver's
	// `combatUnit` does: every stat LOOKED UP from the UnitDef at `defIndex`,
	// current hp from the unit. Nothing is stored -- a cached copy of a stat is
	// a copy that can outlive a table reload.
	strat::Unit CombatUnitOf(const strat::GameUnit& U) const;

	bool bDefinitionsLoaded = false;
	bool bSeeded = false;
};


----- Source/StratRules/Ui.h -- the snapshot structs (READ ONLY) -----
struct UiHexView {
    Hex hex;
    int terrainId = 0;         // index into the loaded TerrainDef table
    int owner     = OWNER_NEUTRAL;   // capturable hexes only; OWNER_NEUTRAL elsewhere
};

struct UiUnitView {
    int  id       = 0;
    int  side     = 0;
    int  unitId   = 0;         // the stub's `unitId`: the §2.4 row index
    Hex  hex;
    int  hp       = 0;
    int  hpMax    = 0;
    bool isFlag   = false;
    // TWO INDEPENDENT FLAGS, not one (T-TURN-01). Read from TurnState's two sets and
    // never from each other: one field cannot express a unit that has spent exactly
    // one of them, which is the drift this row's GDD half repaired. NEITHER IS
    // §2.11.1's DONE bit -- that bit is the selection machine's own, every §2.11
    // surface reading "has not acted" binds to it, and it is deliberately absent
    // here because where per-unit presentation state lives is unruled.
    bool hasMoved = false;
    bool hasActed = false;
    // Progress is held by the TILE and names the unit that accumulated it (Q4,
    // T-FAME-05), so exactly one unit can carry a non-zero value and this per-unit
    // field expresses the tile's state without loss. A MIRROR: the lookup picks the
    // record, and what this reports equals that record's `turnsHeld` unchanged. It is
    // recomputed on every projection rather than stored twice, which is a different
    // property from DECLARED DERIVED and does not make it one.
    int  captureProgress = 0;
    // DECLARED DERIVED (ruled 2026-08-04). True exactly on the placement that the
    // scenario file's `guidedOpening.infantry` names for this unit's SEAT -- side and
    // deployment hex both -- and false on every other unit. A hex identifies one
    // placement because T-SCN-02 forbids two placements sharing one. Computed here
    // and never widget-side, and read off `placement`, not `hex`, so beat 1a's own
    // move cannot unmark the unit the beat is about.
    bool isGuidedMarked = false;
};

// per-factory (ruled 2026-08-04). §2.11.5's BUILD pulse and its boxed-in footer both
// need facts no other group carries.
struct UiFactoryView {
    Hex  hex;                        // mirrors the scenario file's factory placement
    int  owner = OWNER_NEUTRAL;      // mirrors the objective's owner
    // T-TURN-10's per-factory build allowance, and the §2.7 build that holds the
    // factory's slot until it spawns (T-FAME-04). Both MIRROR state the module
    // already holds -- this exposes it rather than adding it.
    bool hasBuiltThisTurn = false;
    bool buildWaiting     = false;
    // DECLARED DERIVED. True exactly when no hex at or adjacent to the factory is
    // free -- board geometry plus §2.7's spawn rule, which places at the factory hex
    // if free and otherwise at the canonically smallest free neighbour.
    //
    // DISTINCT from `buildWaiting`, and the difference is the case §2.11.5 must
    // display: a boxed-in factory with NOTHING QUEUED has `spawnBlocked` true and
    // `buildWaiting` false, which `buildWaiting` alone cannot express. Q31 asks
    // whether a player may queue into a boxed-in factory; `buildWaiting` is the field
    // such a ruling would bind to, and nothing here rules it -- today the waiting
    // build is an AI-only path and no gate asserts a player-queued one.
    bool spawnBlocked = false;
};

struct UiSideView {
    int fameTotal      = 0;
    int fameCombat     = 0;
    int objectivesHeld = 0;    // the X of "objectives held X of N"
    int survivingHp    = 0;
    // DECLARED DERIVED (ruled 2026-08-04). §2.7's rate over that side's held
    // factories (+100 each) and towns (+25 each), read from the terrain table's
    // `incomeFame` so no surface sums it widget-side (T-UI-03's no-arithmetic clause).
    //
    // It is the STANDING rate, and that is what it reads ON TURN 1: what those
    // holdings will pay at the start of that side's turn 2, NOT the 0 that Q8(a) pays
    // on turn 1. So it is deliberately NOT `accrueIncome`'s return value, whose
    // turn-1 guard would make this field read 0 for a side holding four factories.
    // The field is the rate the holdings carry and never the accrual of this turn.
    int incomePerTurn  = 0;
};

struct UiMatchView {
    int  turn       = 0;
    int  turnCap    = 0;
    int  sideToMove = 0;
    // The stub's `resultTier or null`. InProgress IS the null: §2.8's tier is
    // categorical and no numeric result exists to stand in for it.
    bool       hasResult  = false;
    ResultTier resultTier = ResultTier::InProgress;
};

struct UiSnapshot {
    std::vector<UiHexView>     hexes;      // canonical hex order
    std::vector<UiUnitView>    units;      // ascending unit id
    std::vector<UiFactoryView> factories;  // canonical hex order
    UiSideView side[SIDE_COUNT];
    int        objectiveTotal = 0;      // the N of "objectives held X of N"
    UiMatchView match;
};


----- Source/StratUI/StratUI.Build.cs -- your module (READ ONLY) -----
// GDD §4.7 Stub 8 / §2.11 -- the widget module. It holds the UMG surface that binds
// to the view model and NOTHING ELSE: no rules, no state, no arithmetic over what the
// snapshot already carries (T-UI-03's no-widget-side-arithmetic clause).
//
// WHY THIS IS ITS OWN MODULE RATHER THAN LIVING IN `StratBridge`, which is the only
// other module that could host it today. Two reasons, and the second is the one that
// cannot be undone later.
//
// FIRST: `StratBridge.h` states that it declares no reflected types on purpose, so the
// vendored `strat` headers it includes are never put in front of UHT. Adding a UCLASS
// to that module would put a reflected header beside those includes and start eroding
// the property that file was written to hold. Widgets live outside it instead.
//
// SECOND, AND IRREVERSIBLE: a Blueprint that derives from a UMG class bakes that
// class's script path into the asset -- `/Script/StratUI.StratScoreboardWidget`. This
// is the same trap `StratBridge.Build.cs` records for FUnitRow / FTerrainRow, whose
// `/Script/Stratocracy.UnitRow` paths are baked into DT_Units and DT_Terrain and which
// is why the bridge's dependency arrow points at the game module rather than the other
// way. Deciding the widget's module AFTER a WBP_ asset derives from it costs asset
// surgery; deciding it now costs this file.
//
// THE DEPENDENCY RUNS StratUI -> StratBridge AND NEVER BACK. The bridge owns the
// authoritative `strat::GameState`; this module owns pixels. A reverse dependency
// would let the bridge reach a widget, which is the arrangement §4.1's "never own
// rules" exists to prevent -- and it would also reintroduce the UHT problem above.
//
// NOTE ON WHAT IS NOT HERE: the game module `Stratocracy` is deliberately not a
// dependency. Nothing in this module needs FUnitRow or FTerrainRow -- it binds to the
// projected view model, not to the DataTable rows the bridge maps in -- and reaching
// for the game module would couple the widget layer to the row structs for no gain.

using UnrealBuildTool;

public class StratUI : ModuleRules
{
	public StratUI(ReadOnlyTargetRules Target) : base(Target)
	{
		// The same pair `StratBridge.Build.cs` and `StratRules.Build.cs` carry, for the
		// same underlying reason rather than by imitation: this module's .cpp files
		// include `StratBridge.h`, which includes the vendored `strat` headers, so the
		// compile environment those headers land in is this module's. Sharing the
		// engine PCH would put them in an environment whose warning settings this file
		// does not control. Kept conservative here; if a later pass measures that a
		// shared PCH is safe for this module, the comment should record the
		// measurement rather than the setting being dropped quietly.
		PCHUsage = PCHUsageMode.NoPCHs;
		bUseUnity = false;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			// The view-model source. `StratBridge` re-exports the vendored headers by
			// short name through its own PublicIncludePaths, so this module reaches
			// `Ui.h` and `Replay.h` through that dependency and vendors nothing itself.
			"StratBridge"
		});

		// UMG pulls Slate and SlateCore behind it, but a module that names UUserWidget
		// in its own headers declares all three rather than relying on that.
		PrivateDependencyModuleNames.AddRange(new string[] {
			"UMG",
			"Slate",
			"SlateCore"
		});

		PublicIncludePaths.AddRange(new string[] {
			"StratUI"
		});
	}
}


----- Source/StratBridge/Tests/StratBridgeParity.cpp -- the Automation pattern to copy (READ ONLY, first 200 lines) -----
// The editor pass for §4.9 part 2's bridge -- T-INT-02, T-INT-03, and T-SAVE-06's
// in-engine half. GDD §4.9, §4.10, §4.11 rows 9 and 10.
//
//   T-INT-02  replay parity: the same command log replayed headless and in-engine
//             (Automation test) produces the same final canonical state hash.
//   T-INT-03  rejection safety: an illegal command leaves the state hash unchanged
//             and returns a reason; no partial application.
//   T-SAVE-06 stateHash stability across the headless and in-engine builds. Marked †
//             in §4.11 and asserted JOINTLY with T-INT-02 -- it is the same
//             comparison read from the other end, so it closes here or nowhere.
//
// WHERE THE HEADLESS SIDE OF THE COMPARISON COMES FROM, and this is the whole design
// of the gate. `Data/parity_fixture.save` was emitted by the CREW repo's build, from
// the shipped tables and scenario, and carries the canonical state hash its log
// reaches THERE. This file replays that same log HERE and compares. Neither side
// recomputes the other's expectation, and nothing in this file chooses a number.
//
// A fixture re-emitted on this side would compare the engine against itself and
// would pass under exactly the divergence T-INT-02 exists to catch -- an agent that
// "ports" rather than vendors the module, or a compiler that rounds differently.
// That is why the fixture is a vendored artifact under GATE-DATA-VENDOR rather than
// something this test produces. It is kept fresh at its source by the crew's
// GATE-REPLAY-FIXTURE, whose clause (g) re-emits and compares bytes; it is kept
// equal to that source here by GATE-DATA-VENDOR's sha256. Between those two, a
// stale fixture cannot reach this comparison quietly.
//
// GATE-BRIDGE-DEFS is an unnumbered check on the GATE-DATA-VENDOR / GATE-AI-SMOKE
// precedent: it mints no acceptance ID. It exists because FStratBridge.h states that
// the DataTable row order is ASSERTED equal to `strat::loadUnits` over the same
// vendored bytes by the editor pass, rather than assumed -- and an unrun assertion
// in a header comment is not an assertion. See its own header block below.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBridge.h"

// The vendored rules module, by short name via StratRules' PublicIncludePaths. The
// test reads the fixture through `strat::parseSave` for the same reason the bridge
// loads the scenario through `strat::parseScenario`: the §4.10 format is the rules
// module's, and a second reader here could disagree with it.
#include "Data.h"
#include "Save.h"

namespace StratBridgeParity
{
	// MUST BE 0, and must be the value the crew emitter used. Replay.h states that no
	// rule in this project decides which side moves first, and that a disagreement
	// between the two callers shows up as a hash divergence -- which is T-INT-02
	// reporting a difference in the harnesses rather than in the code under test. The
	// crew side pins it in `kParityFirstSide`; this is the other pin.
	static const int32 kParityFirstSide = 0;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static std::string ToStd(const FString& In)
	{
		return std::string(TCHAR_TO_UTF8(*In));
	}

	static FString FromStd(const std::string& In)
	{
		return FString(UTF8_TO_TCHAR(In.c_str()));
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/**
	 * Brings a bridge to the state the fixture's log starts from: the shipped tables
	 * mapped in, the shipped scenario seeded through `strat::seedFromScenario`.
	 *
	 * Returns false and fills OutError rather than asserting, so a missing asset is a
	 * named failure in the caller's own clause list instead of a crash in a helper --
	 * and so every caller reports the same setup failure the same way.
	 */
	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		UDataTable* Units = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		UDataTable* Terrain = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
		if (Units == nullptr || Terrain == nullptr)
		{
			OutError = TEXT("DT_Units and/or DT_Terrain did not load from /Game/StratData");
			return false;
		}

		const FStratResult Loaded = Bridge.LoadDefinitions(Units, Terrain);
		if (!Loaded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadDefinitions refused: %s"), *Loaded.Reason);
			return false;
		}

		const FString ScenarioPath = FPaths::Combine(DataDir(), TEXT("ferrum_crossing.json"));
		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath, kParityFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	/** Reads the committed §4.10 fixture and parses it through the rules module. */
	static bool LoadFixture(strat::Save& Out, FString& OutError)
	{
		const FString Path = FPaths::Combine(DataDir(), TEXT("parity_fixture.save"));
		FString Text;
		if (!FFileHelper::LoadFileToString(Text, *Path))
		{
			OutError = FString::Printf(TEXT("parity_fixture.save unreadable at %s"), *Path);
			return false;
		}

		const strat::SaveLoadResult Parsed =
			strat::parseSave(ToStd(Text), "parity_fixture.save", Out);
		if (!Parsed.ok)
		{
			OutError = FString::Printf(TEXT("%s: %s"),
				*FromStd(Parsed.failedId), *FromStd(Parsed.reason));
			return false;
		}
		return true;
	}

	static TArray<strat::SaveCommand> ToArray(const std::vector<strat::SaveCommand>& In)
	{
		TArray<strat::SaveCommand> Out;
		Out.Reserve(static_cast<int32>(In.size()));
		for (const strat::SaveCommand& C : In)
		{
			Out.Add(C);
		}
		return Out;
	}
}

// ---------------------------------------------------------------------------
// GATE-BRIDGE-DEFS -- the mapped unit vector is the loader's vector, ORDER INCLUDED.
//
// WHY ORDER IS THE SUBJECT AND NOT AN INCIDENTAL. `defIndex` indexes this vector,
// and a §4.10 Build command carries a defIndex in its `unitId` field. A vector built
// in a different order than `strat::loadUnits` would resolve the same command log to
// a different unit type -- and it would do so SILENTLY, because both worlds would
// still produce a self-consistent game.
//
// This is not covered by T-INT-02 today, and the gap is stated rather than left to
// be discovered: the parity fixture carries no Build command, because the AI that
// produced it emits none on the shipped scenario. So the divergence this check
// exists to catch is exactly the one the replay cannot currently see, and a
// GATE-DATA-VENDOR pass does not close it either -- that gate compares BYTES on
// disk and says nothing about the order the importer produced from them.
//
// It mints no acceptance ID, on the GATE-DATA-VENDOR / GATE-AI-SMOKE precedent.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeDefsOrderTest,
	"Stratocracy.StratBridge.GATE-BRIDGE-DEFS.MappedDefsMatchLoaderOrder",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeDefsOrderTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeParity;

	UDataTable* Units = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
	UDataTable* Terrain = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
	if (!TestNotNull(TEXT("DT_Units loads"), Units) ||
		!TestNotNull(TEXT("DT_Terrain loads"), Terrain))
	{
		return false;
	}

	FStratBridge Bridge;
	if (!TestTrue(TEXT("LoadDefinitions accepts the shipped tables"),
		Bridge.LoadDefinitions(Units, Terrain).bOk))
	{
		return false;
	}

	// The independent reading: the same bytes, through the headless loader.
	std::vector<strat::UnitDef> Expected;
	std::string Err;
	const FString CsvPath = FPaths::Combine(DataDir(), TEXT("units.csv"));
	if (!TestTrue(FString::Printf(TEXT("strat::loadUnits reads %s"), *CsvPath),
		strat::loadUnits(ToStd(CsvPath), Expected, Err)))
	{
		AddError(FString::Printf(TEXT("loadUnits: %s"), *FromStd(Err)));
		return false;
	}

