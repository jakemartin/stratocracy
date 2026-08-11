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

GOAL: Bridge view-model surface: GameState -> UiWorld -> UiSnapshot on FStratBridge

WHY THIS ONE: the priority scorer selected it. §4.9: the bridge owns the authoritative `strat::GameState`. §4.7 Stub 8: `buildUiSnapshot(UiWorld)` is the only projection, and the widgets bind to the view model it produces. The tree supplies `uiWorldOf(Session)` only, which takes the driver's type and not the bridge's.

WHERE IT LANDS: Source/StratBridge/ (projection + retained scenario)

THE PROBLEM, stated precisely. `strat::buildUiSnapshot` takes a
`strat::UiWorld`. The only function in the tree that builds one is the
headless driver's `uiWorldOf(const Session&)` -- and `FStratBridge` owns a
`strat::GameState`, not a `Session`. So no engine-side caller can reach the
view model at all. Close that gap.

WHAT TO PRODUCE: complete replacement files for StratBridge.h and
StratBridge.cpp that add:
  1. A `UiWorld` built from the bridge's own `GameState` + tables + the
     scenario it loaded. Mirror `uiWorldOf` field for field.
  2. A `UiSnapshot` query that delegates to `strat::buildUiSnapshot` and
     adds NO arithmetic of its own.
  3. Retention of the parsed `strat::Scenario`, because `UiWorld::guided`
     has no other source and `isGuidedMarked` is wrong without it.

CONSTRAINTS THAT ARE NOT NEGOTIABLE:
- `StratBridge.h` declares NO reflected type (no UCLASS/USTRUCT/UENUM). Its
  header comment explains why: UHT must never be shown the vendored `strat`
  headers. Keep that true.
- Do not modify anything under Source/StratRules/. Those are vendored
  certified bytes and T-INT-01 hash-matches them against a crew commit.
- The `Unit` a `UiUnit` carries is built from the loaded `UnitDef` plus
  current HP, exactly as `combatUnit` does. Look up every stat; store none.

----- Source/StratBridge/StratBridge.h (replace this file) -----
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
// refuses instead and says why.
//
// NOT A UOBJECT, and deliberately. UHT parses headers that declare reflected types,
// and this one declares none, so the vendored `strat` headers it includes are never
// put in front of the header tool. A UMG-facing wrapper can come later without
// moving the state out of here.
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

private:
	// Owned by value: this object IS the authoritative state.
	strat::GameState GameState;

	// The §4.8 tables, mapped once and then borrowed by every call.
	std::vector<strat::UnitDef>    Units;
	std::vector<strat::TerrainDef> Terrain;

	bool bDefinitionsLoaded = false;
	bool bSeeded = false;
};


----- Source/StratBridge/StratBridge.cpp (replace this file) -----
// GDD §4.9 part 2 -- the bridge. See StratBridge.h for what it owns and decides.
#include "StratBridge.h"

#include "Engine/DataTable.h"
#include "Misc/FileHelper.h"
#include "Modules/ModuleManager.h"

#include "StratData/StratDataRows.h"

// A REAL module object, unlike Source/StratRules/. That directory holds vendored
// C++ with no IMPLEMENT_MODULE, and listing it in Stratocracy.uproject once made
// the editor abort at startup with "The game module 'StratRules' could not be
// successfully initialized" -- recorded in §3. This module is our own code, so it
// carries the module object the loader requires and can be listed safely.
IMPLEMENT_MODULE(FDefaultModuleImpl, StratBridge)

namespace
{
	// EUnitType mirrors strat::UnitType, and the mirror is pinned by
	// static_assert in StratDataParity.cpp and asserted at runtime by T-DATA-05.
	// This cast rides on that guarantee rather than restating it as a switch a
	// fifth enumerator could silently fall through.
	strat::UnitType ToStratUnitType(EUnitType In)
	{
		return static_cast<strat::UnitType>(static_cast<uint8>(In));
	}

	std::string ToStd(const FString& In)
	{
		return std::string(TCHAR_TO_UTF8(*In));
	}

	FString FromStd(const std::string& In)
	{
		return FString(UTF8_TO_TCHAR(In.c_str()));
	}
}

FStratResult FStratBridge::LoadDefinitions(const UDataTable* UnitTable, const UDataTable* TerrainTable)
{
	if (UnitTable == nullptr || TerrainTable == nullptr)
	{
		return FStratResult::Fail(TEXT("a definition table was null"));
	}

	std::vector<strat::UnitDef>    NextUnits;
	std::vector<strat::TerrainDef> NextTerrain;

	// GetRowNames() returns rows in the table's own order, which for a
	// CSV-imported table is the CSV's order. That equality is not assumed here:
	// the editor pass asserts this vector against strat::loadUnits over the same
	// vendored bytes, order included, because defIndex depends on it.
	for (const FName& RowName : UnitTable->GetRowNames())
	{
		const FUnitRow* Row = UnitTable->FindRow<FUnitRow>(RowName, TEXT("FStratBridge::LoadDefinitions"));
		if (Row == nullptr)
		{
			return FStratResult::Fail(FString::Printf(
				TEXT("unit row '%s' is not an FUnitRow"), *RowName.ToString()));
		}

		strat::UnitDef Def;
		// The row NAME is the id: FUnitRow carries no Id field, because the
		// table's key already is one and a second copy could disagree with it.
		Def.id         = ToStd(RowName.ToString());
		Def.hpMax      = Row->HP;
		Def.move       = Row->Move;
		Def.atk        = Row->Atk;
		Def.def        = Row->Def;
		Def.rangeMin   = Row->RangeMin;
		Def.rangeMax   = Row->RangeMax;
		Def.costFame   = Row->CostFame;
		Def.type       = ToStratUnitType(Row->Type);
		Def.canCapture = Row->CanCapture;
		NextUnits.push_back(Def);
	}

	for (const FName& RowName : TerrainTable->GetRowNames())
	{
		const FTerrainRow* Row = TerrainTable->FindRow<FTerrainRow>(RowName, TEXT("FStratBridge::LoadDefinitions"));
		if (Row == nullptr)
		{
			return FStratResult::Fail(FString::Printf(
				TEXT("terrain row '%s' is not an FTerrainRow"), *RowName.ToString()));
		}

		strat::TerrainDef Def;
		Def.id            = ToStd(RowName.ToString());
		Def.moveCost      = Row->MoveCost;
		Def.defensePct    = Row->DefensePct;
		Def.passLand      = Row->PassLand;
		Def.passAir       = Row->PassAir;
		Def.passSea       = Row->PassSea;
		Def.capturable    = Row->Capturable;
		Def.incomeFame    = Row->IncomeFame;
		Def.isSpawnPoint  = Row->IsSpawnPoint;
		Def.isRepairPoint = Row->IsRepairPoint;
		NextTerrain.push_back(Def);
	}

	Units   = std::move(NextUnits);
	Terrain = std::move(NextTerrain);
	bDefinitionsLoaded = true;
	// Definitions changed underneath it, so any prior seed is stale rather than
	// merely old: its defIndex values point into the vector that just moved.
	bSeeded = false;
	return FStratResult::Ok();
}

strat::RulesTables FStratBridge::Tables() const
{
	strat::RulesTables T;
	T.units   = &Units;
	T.terrain = &Terrain;
	return T;
}

FStratResult FStratBridge::LoadScenarioFromFile(const FString& ScenarioFilePath, int32 FirstSide)
{
	if (!bDefinitionsLoaded)
	{
		return FStratResult::Fail(TEXT("definitions are not loaded"));
	}

	FString Text;
	if (!FFileHelper::LoadFileToString(Text, *ScenarioFilePath))
	{
		return FStratResult::Fail(FString::Printf(
			TEXT("scenario file unreadable at %s"), *ScenarioFilePath));
	}

	// The module parses and validates; the bridge supplies bytes and a path to
	// name in the error, and reads no field of the file itself.
	strat::Scenario Scenario;
	const strat::ScenarioLoadResult Parsed =
		strat::parseScenario(ToStd(Text), ToStd(ScenarioFilePath), Scenario);
	if (!Parsed.ok)
	{
		return FStratResult::Fail(FromStd(Parsed.reason), FromStd(Parsed.failedId));
	}

	const strat::ScenarioLoadResult Valid =
		strat::validateScenario(Scenario, Units, Terrain);
	if (!Valid.ok)
	{
		return FStratResult::Fail(FromStd(Valid.reason), FromStd(Valid.failedId));
	}

	std::string Err;
	if (!strat::seedFromScenario(GameState, Scenario, Tables(), FirstSide, Err))
	{
		return FStratResult::Fail(FromStd(Err));
	}

	bSeeded = true;
	return FStratResult::Ok();
}

FStratResult FStratBridge::Submit(const strat::SaveCommand& Command)
{
	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}

	const strat::ReplayResult R = strat::applyCommand(GameState, Command, Tables());
	if (!R.ok)
	{
		return FStratResult::Fail(FromStd(R.reason), FromStd(R.failedId));
	}
	return FStratResult::Ok();
}

FStratResult FStratBridge::ReplayLog(const TArray<strat::SaveCommand>& Log)
{
	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}

	std::vector<strat::SaveCommand> AsVector;
	AsVector.reserve(static_cast<size_t>(Log.Num()));
	for (const strat::SaveCommand& C : Log)
	{
		AsVector.push_back(C);
	}

	const strat::ReplayResult R = strat::replayLog(GameState, AsVector, Tables());
	if (!R.ok)
	{
		return FStratResult::Fail(
			FString::Printf(TEXT("%s (at index %d)"), *FromStd(R.reason), R.failedIndex),
			FromStd(R.failedId));
	}
	return FStratResult::Ok();
}

FString FStratBridge::StateHash() const
{
	return FromStd(strat::canonicalStateHash(GameState));
}


----- Source/StratRules/Ui.h -- the contract you project into (READ ONLY) -----
struct UiUnit {
    int  id       = 0;
    int  side     = 0;
    int  defIndex = 0;         // into `unitDefs`; the §2.4 row this unit is
    Hex  hex;
    Unit unit;                 // current hp/hpMax and the stat block combat reads
    bool isFlag   = false;     // Stub 7's placement field (Q10 open on exactness)
    // The Stub-7 placement this unit was DEPLOYED at, which is not `hex` once it
    // moves. It is an input, borrowed from the scenario file exactly as `isFlag` is,
    // and it exists because `isGuidedMarked` is a property OF THE PLACEMENT: the stub
    // says in as many words that it "does not move when the unit does". Deriving that
    // field from `hex` instead would unmark the marked Infantry the moment beat 1a
    // asked it to move, which is the one thing beat 1a does.
    Hex  placement;
};

struct UiWorld {
    Board                       board;
    std::vector<UiUnit>         units;
    const std::vector<UnitDef>*    unitDefs = nullptr;
    const std::vector<TerrainDef>* terrain  = nullptr;
    const EconomyState*         economy = nullptr;
    const TurnState*            turn    = nullptr;
    // The scenario file's `guidedOpening` entries (Stub 7), one per side. §4.9 names
    // the loaded scenario file as one of the three module-side sources the snapshot
    // may draw on, alongside `GameState` and the §4.8 tables. Null on a fixture with
    // no guided opening, which marks nobody rather than being an error.
    const std::vector<ScenarioGuided>* guided = nullptr;
};

// ---------------------------------------------------------------------------
// The view model = this SNAPSHOT plus the PRESENTATION BLOCK declared below it.
// §4.7 Stub 8 states both field lists and they are followed exactly; NO FIELD IS
// ADDED, and T-UI-05 clause (c) is what enforces that rather than this comment.
//
// The three values this module once filed as unwritten change requests -- the
// per-factory "has built this turn" record §2.11.5's BUILD pulse needs, §2.11.2's
// income rate, and §2.11.1's DONE bit -- were RULED on 2026-08-04 and are written
// into the stub. The first two are snapshot fields below; the DONE bit is the
// presentation block's, because it is derivable from neither turn flag nor from any
// pair of them (Wait and RMB-in-MOVED both reach it without spending the act flag).
//
// TWO KINDS, and every snapshot field has exactly one. A MIRROR -- the unmarked
// default -- equals, unchanged, the module-side value it names. A field marked
// DECLARED DERIVED is computed from those values instead and states its derivation
// beside it. "Derived on every projection rather than stored twice" is NOT the same
// property: `captureProgress` is recomputed on each pass and is still a MIRROR,
// because what it reports equals a value the module holds. The distinction is
// load-bearing -- T-UI-05 checks the two kinds by different rules.
// ---------------------------------------------------------------------------
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

// Read-only projection of the world into the view model. Enumerates hexes and
// factories in canonical order and units by ascending id, so two runs on the same
// state produce the same bytes. Pure: it mutates nothing it is given.
//
// It produces the SNAPSHOT ONLY. The presentation block below is deliberately not an
// output of this function: its two members have owners that are not this module, and
// a rules module that filled them in would be asserting state it does not hold.
UiSnapshot buildUiSnapshot(const UiWorld& w);

// ---------------------------------------------------------------------------
// The PRESENTATION BLOCK (§4.7 Stub 8). NOT produced by the rules module: two
// members, each with its owner named, and neither owner a widget.
//
// It is in the view-model rather than inside a widget precisely so that T-INT-05
// (§4.9) can rebuild the screen from the view-model alone -- state in the block
// satisfies that invariant, state in a widget does not.
//
// It is NOT in T-UI-05's subject. That invariant asks whether the snapshot tells the
// truth about th


----- Source/StratRules/Replay.h -- GameState (READ ONLY) -----
struct GameUnit {
    int id       = 0;
    int side     = 0;
    int defIndex = 0;
    Hex hex;
    int hp       = 0;
    // Where this unit was DEPLOYED, which stops being `hex` the moment it moves.
    // NOT hashed: it is scenario data pinned by `scenarioHash`, not match state.
    Hex placement;
};

// The authoritative composed state (§4.9). Every member is state a rules module owns
// and this struct borrows nothing mutable from anywhere else.
struct GameState {
    MapBounds             bounds;
    std::vector<int>      terrain;    // offset-indexed, into the TerrainDef table
    std::vector<GameUnit> units;
    EconomyState          economy;
    TurnState             turn;
    int                   nextUnitId = 1;
    // Which unit is the flag, per side, from Stub 7's `isFlag` placement field; -1
    // means this side designates none, so flag death is unreachable rather than
    // assumed (Q10 stays open on exactness). It is held HERE and not as a bool on
    // GameUnit because THE DESIGNATION MUST OUTLIVE THE UNIT: a dead flag is simply
    // absent from `units`, and a per-unit bool cannot then tell "the flag died" from
    // "this side never had one" -- which are opposite verdicts at §2.8.
    int                   flagUnit[SIDE_COUNT] = {-1, -1};
};

// Per-unit flag status, DERIVED from the designation above. §4.10 hashes it as a
// per-unit value, in the same way it hashes `hasMoved`/`hasActed` -- per-unit FACTS
// that row 5 holds as id sets rather than as fields on the unit.
bool isFlagUnit(const GameState& g, const GameUnit& u);

// The §4.8 tables, borrowed. Never owned, never copied, never recomputed here.
struct RulesTables {
    const std::vector<UnitDef>*    units   = nullptr;
    const std::vector<TerrainDef>* terrain = nullptr;
};

// Occupancy is DERIVED from `units` on every call rather than stored beside them --
// one source for where a unit is. Row 3 takes a Board as an argument for the same
// reason row 4 takes the turn number as one.
Board boardOf(const GameState& g);

const GameUnit* findGameUnit(const GameState& g, int id);

// ---------------------------------------------------------------------------


----- Source/StratRules/Driver.good.cpp -- uiWorldOf, the reference implementation to mirror (READ ONLY) -----
static Unit combatUnit(const Session& s, const DriverUnit& u) {
    const UnitDef& d = s.unitDefs[u.defIndex];
    Unit c;
    c.atk = d.atk; c.def = d.def; c.hp = u.hp; c.hpMax = d.hpMax;
    c.rangeMin = d.rangeMin; c.rangeMax = d.rangeMax; c.type = d.type;
    return c;
}

// Row 8's world, GATHERED and not decided: the board from the session's terrain, the
// units from its roster, ownership and Fame from Economy.h, the turn and both per-unit
// flags from Turn.h, the flag designation from the same field `snapshotOf` reads.
UiWorld uiWorldOf(const Session& s) {
    UiWorld w;
    w.board    = buildBoard(s);
    w.unitDefs = &s.unitDefs;
    w.terrain  = &s.terrainDefs;
    w.economy  = &s.economy;
    w.turn     = &s.match;
    // The guided seats come from the loaded file and from nowhere else. A built-in
    // fixture has no guided opening (`fixture` says so), so this stays null and every
    // unit is unmarked -- absence, not a default.
    w.guided   = s.scenarioLoaded ? &s.scenario.guided : nullptr;
    for (const DriverUnit& u : s.units) {
        UiUnit v;
        v.id       = u.id;
        v.side     = u.side;
        v.defIndex = u.defIndex;
        v.hex      = u.hex;
        v.placement = u.placement;
        v.unit     = combatUnit(s, u);
        v.isFlag   = (u.side >= 0 && u.side < SIDE_COUNT && s.flagUnit[u.side] == u.id);
        w.units.push_back(v);
    }
    return w;
}

