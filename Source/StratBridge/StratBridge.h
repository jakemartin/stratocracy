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

	// T-UI-05's own check, run over a snapshot this bridge projected, with the
	// failures handed back as engine strings.
	//
	// IT IS EXPOSED HERE RATHER THAN CALLED DIRECTLY BY THE CALLER, and that is
	// this module's whole reason for existing rather than a convenience. The
	// vendored `strat` sources carry no `_API` macro -- §4.9 forbids them engine
	// headers -- so `UnrealEditor-StratBridge.dll` exports `FStratBridge` and
	// nothing else, and a call to `strat::uiCheckSnapshotFidelity` from any other
	// module does not link. MEASURED, not assumed: the StratUI Automation test
	// called it directly and the build failed with LNK2019 on exactly that symbol,
	// which is the same failure StratBridge.Build.cs records the first bridge
	// hitting. Every strat entry point a widget or its gate needs arrives through
	// a method on this class for that reason.
	//
	// RETURNS THE MODULE'S VERDICT AND NOT A RECOUNT OF IT. `Result.ok` is
	// forwarded rather than inferred from an empty failure list: Ui.h's clause (c)
	// fails a snapshot field carrying no contract entry, and treating "nothing
	// listed" as success would let this method disagree with the check it ran.
	FStratResult CheckSnapshotFidelity(const strat::UiSnapshot& Snapshot,
	                                   TArray<FString>& OutFailures) const;

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
