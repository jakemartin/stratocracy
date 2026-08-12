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
	// And the retained scenario goes with it, for the same reason and one more:
	// it was validated against the OLD tables, so its guided seats and its unit
	// ids are claims about a table that no longer exists. Dropping it makes
	// every unit unmarked until a scenario is loaded again, which is absence and
	// not a wrong answer.
	LoadedScenario = strat::Scenario();
	// And the recorded log with it. Every Build entry in it carries a defIndex
	// into the vector that just moved, so the log is not merely old -- it names
	// different unit types than it did a line ago.
	Recorded.clear();
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
	strat::Scenario Parsed;
	const strat::ScenarioLoadResult ParseResult =
		strat::parseScenario(ToStd(Text), ToStd(ScenarioFilePath), Parsed);
	if (!ParseResult.ok)
	{
		return FStratResult::Fail(FromStd(ParseResult.reason), FromStd(ParseResult.failedId));
	}

	const strat::ScenarioLoadResult ValidResult =
		strat::validateScenario(Parsed, Units, Terrain);
	if (!ValidResult.ok)
	{
		return FStratResult::Fail(FromStd(ValidResult.reason), FromStd(ValidResult.failedId));
	}

	std::string Err;
	if (!strat::seedFromScenario(GameState, Parsed, Tables(), FirstSide, Err))
	{
		return FStratResult::Fail(FromStd(Err));
	}

	// Retained only on the success path, and only after the state it describes
	// exists. A scenario kept alongside a state it failed to seed would be a
	// guided-seat list for units that are not there.
	LoadedScenario = std::move(Parsed);

	// A new seed, so the old log describes a match that no longer exists. Cleared
	// on the success path only: a failed load leaves the previous match intact and
	// its log with it.
	Recorded.clear();

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

	// AFTER the module accepted it, and only then. §4.9 says a rejected command
	// changes nothing; a log that recorded the attempt would be a change.
	Recorded.push_back(Command);
	return FStratResult::Ok();
}

FStratResult FStratBridge::SubmitStamped(strat::SaveCommand Command)
{
	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}

	// READ, not decided, and read BEFORE the command is applied. `applyCommand`
	// refuses an entry whose tag disagrees with the live turn, and an EndTurn read
	// back afterwards would carry the turn it opened rather than the one it closed.
	Command.turn = GameState.turn.turnNumber;
	Command.side = GameState.turn.activeSide;

	return Submit(Command);
}

FStratResult FStratBridge::SubmitMove(int32 UnitId, const strat::Hex& DestHex)
{
	strat::SaveCommand C;
	C.kind    = strat::SaveCommandKind::Move;
	C.unitId  = UnitId;
	C.hex     = DestHex;
	C.hasUnit = true;
	C.hasHex  = true;
	return SubmitStamped(C);
}

FStratResult FStratBridge::SubmitAttack(int32 UnitId, const strat::Hex& TargetHex)
{
	strat::SaveCommand C;
	C.kind    = strat::SaveCommandKind::Attack;
	C.unitId  = UnitId;
	C.hex     = TargetHex;
	C.hasUnit = true;
	C.hasHex  = true;
	return SubmitStamped(C);
}

FStratResult FStratBridge::SubmitBuild(const strat::Hex& FactoryHex, int32 DefIndex)
{
	strat::SaveCommand C;
	C.kind = strat::SaveCommandKind::Build;
	// The format's `unitId` field, which for a Build is the §2.4 ROW INDEX and not
	// a unit id. Save.h:64 names both meanings on one line; this is the second.
	C.unitId  = DefIndex;
	C.hex     = FactoryHex;
	C.hasUnit = true;
	C.hasHex  = true;
	return SubmitStamped(C);
}

FStratResult FStratBridge::SubmitCapture(int32 UnitId)
{
	strat::SaveCommand C;
	C.kind    = strat::SaveCommandKind::Capture;
	C.unitId  = UnitId;
	C.hasUnit = true;
	// No hex: Save.h:66 states Capture names none, and a hex set here would be a
	// field of another kind's command, which the parser REFUSES rather than
	// ignores.
	C.hasHex  = false;
	return SubmitStamped(C);
}

FStratResult FStratBridge::SubmitEndTurn()
{
	strat::SaveCommand C;
	C.kind = strat::SaveCommandKind::EndTurn;
	// The one kind that names neither a unit nor a hex (Save.h:66-67).
	C.hasUnit = false;
	C.hasHex  = false;
	return SubmitStamped(C);
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

	// All-or-nothing at the module means all-or-nothing here: `replayLog` assigned
	// only after the last command succeeded, so appending the whole log keeps
	// "recorded" equal to "applied" without this file re-deciding either.
	Recorded.insert(Recorded.end(), AsVector.begin(), AsVector.end());
	return FStratResult::Ok();
}

FString FStratBridge::StateHash() const
{
	return FromStd(strat::canonicalStateHash(GameState));
}

// ---------------------------------------------------------------------------
// View model (§4.7 Stub 8).
// ---------------------------------------------------------------------------

strat::Unit FStratBridge::CombatUnitOf(const strat::GameUnit& U) const
{
	// Field for field with the driver's `combatUnit`, deliberately: the stat
	// block combat reads is the UnitDef's, and only `hp` comes from the unit.
	// Every value is LOOKED UP on each call. Caching one would be a second copy
	// of a §2.4 row that a table reload cannot reach.
	const strat::UnitDef& D = Units[static_cast<size_t>(U.defIndex)];

	strat::Unit C;
	C.atk      = D.atk;
	C.def      = D.def;
	C.hp       = U.hp;
	C.hpMax    = D.hpMax;
	C.rangeMin = D.rangeMin;
	C.rangeMax = D.rangeMax;
	C.type     = D.type;
	return C;
}

strat::UiWorld FStratBridge::MakeUiWorld() const
{
	strat::UiWorld W;

	// The board is DERIVED by the module from the state we own -- `boardOf` is
	// the same one-source-for-occupancy rule Replay.h states. The driver had to
	// build its own because its roster is DriverUnits; ours is GameUnits, so the
	// module's own derivation is reachable and hand-rolling a second one here
	// would be the drift this module exists to prevent.
	W.board    = strat::boardOf(GameState);
	W.unitDefs = &Units;
	W.terrain  = &Terrain;
	W.economy  = &GameState.economy;
	W.turn     = &GameState.turn;

	// The guided seats come from the loaded file and from nowhere else, exactly
	// as in `uiWorldOf`. An unseeded bridge holds a default Scenario, so this is
	// null and every unit is unmarked -- absence, not a default. It is null
	// rather than "the empty list of the scenario we happen to be holding"
	// because those are different claims and `isGuidedMarked` should not be
	// answering on behalf of a scenario that was never loaded.
	W.guided   = bSeeded ? &LoadedScenario.guided : nullptr;

	for (const strat::GameUnit& U : GameState.units)
	{
		// Unreachable on a seeded bridge: validateScenario pins every defIndex
		// against the same table `Units` was built from, and applyCommand takes
		// its defIndex through the same check. It is guarded anyway because the
		// alternative to skipping is reading past the end of the table, and the
		// alternative to BOTH -- emitting the unit with zeroed stats -- would
		// put a made-up §2.4 row in front of the player. Dropping invents
		// nothing; MakeUiSnapshot refuses before it can happen.
		if (U.defIndex < 0 || static_cast<size_t>(U.defIndex) >= Units.size())
		{
			ensureMsgf(false, TEXT("unit %d carries defIndex %d, outside the loaded table"),
				U.id, U.defIndex);
			continue;
		}

		strat::UiUnit V;
		V.id        = U.id;
		V.side      = U.side;
		V.defIndex  = U.defIndex;
		V.hex       = U.hex;
		// The DEPLOYMENT hex, not the current one. Ui.h is explicit that
		// `isGuidedMarked` is a property of the placement and does not move when
		// the unit does; passing `hex` here would unmark the marked Infantry the
		// instant beat 1a moved it.
		V.placement = U.placement;
		V.unit      = CombatUnitOf(U);
		// The module's own derivation from the per-side designation. Read
		// through `isFlagUnit` rather than by comparing `flagUnit[side]`
		// in-line, because a dead flag is absent from `units` and the rule for
		// what that means belongs to the module that owns the designation.
		V.isFlag    = strat::isFlagUnit(GameState, U);
		W.units.push_back(V);
	}

	return W;
}

FStratResult FStratBridge::MakeUiSnapshot(strat::UiSnapshot& OutSnapshot) const
{
	if (!bDefinitionsLoaded)
	{
		return FStratResult::Fail(TEXT("definitions are not loaded"));
	}
	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}

	for (const strat::GameUnit& U : GameState.units)
	{
		if (U.defIndex < 0 || static_cast<size_t>(U.defIndex) >= Units.size())
		{
			return FStratResult::Fail(FString::Printf(
				TEXT("unit %d carries defIndex %d, outside the loaded unit table"),
				U.id, U.defIndex));
		}
	}

	// The world does not escape this function, which is the whole reason to
	// prefer this entry point: every pointer in it borrows from `this`, and a
	// caller holding one past a LoadDefinitions would be reading a table that
	// moved.
	const strat::UiWorld World = MakeUiWorld();

	// The one line this method exists for. Nothing above it computed a snapshot
	// field and nothing below it adjusts one -- §4.7 Stub 8 names this the only
	// projection, and T-UI-05 asserts over its output, so a correction applied
	// here would be a correction the gate cannot see.
	OutSnapshot = strat::buildUiSnapshot(World);
	return FStratResult::Ok();
}

FStratResult FStratBridge::CheckSnapshotFidelity(
	const strat::UiSnapshot& Snapshot,
	TArray<FString>&         OutFailures) const
{
	OutFailures.Reset();

	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}

	// Rebuilt here rather than taken as an argument. The check compares a snapshot
	// against the state it claims to project, so the world it is measured against
	// must be this bridge's own -- a caller-supplied one could be a second assembly
	// that agrees with the snapshot for reasons the state does not.
	const strat::UiWorld World = MakeUiWorld();

	const strat::UiFidelityResult Result =
		strat::uiCheckSnapshotFidelity(World, Snapshot);

	for (const strat::UiFidelityFailure& Failure : Result.failures)
	{
		OutFailures.Add(FString::Printf(TEXT("clause (%s) %s: %s"),
			*FromStd(Failure.clause), *FromStd(Failure.field), *FromStd(Failure.detail)));
	}

	// The module's own verdict, forwarded. See the header for why `ok` is not
	// re-derived from whether the list came back empty.
	return Result.ok
		? FStratResult::Ok()
		: FStratResult::Fail(
			FString::Printf(TEXT("snapshot fidelity failed on %d field(s)"),
				OutFailures.Num()),
			TEXT("T-UI-05"));
}

FStratResult FStratBridge::Reachable(int32 UnitId, std::vector<strat::ReachEntry>& OutReach) const
{
	// Cleared up front so a refusal cannot leave the caller holding a previous
	// call's set and read it as this call's answer.
	OutReach.clear();

	if (!bDefinitionsLoaded)
	{
		return FStratResult::Fail(TEXT("definitions are not loaded"));
	}
	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}

	// MakeUiSnapshot's refusal, made here for its reason AND one more.
	// MakeUiWorld SKIPS a unit whose defIndex is outside the loaded table, so
	// without this loop such a unit would reach `findUiUnit` below as "no unit
	// with id N" -- a refusal blaming the caller's argument for a fault in the
	// table. Validating first keeps those two answers from being spelled the
	// same way.
	for (const strat::GameUnit& U : GameState.units)
	{
		if (U.defIndex < 0 || static_cast<size_t>(U.defIndex) >= Units.size())
		{
			return FStratResult::Fail(FString::Printf(
				TEXT("unit %d carries defIndex %d, outside the loaded unit table"),
				U.id, U.defIndex));
		}
	}

	// Borrowed for the length of this call and never escaping it, exactly as in
	// MakeUiSnapshot: every pointer in the world points into `this`.
	const strat::UiWorld World = MakeUiWorld();

	// The module's own finder, over the world the query will resolve against --
	// not a scan of `GameState.units`. Asking a different list would let this
	// check pass where `uiReachable` then finds nothing, which is the one
	// disagreement the check exists to prevent.
	if (strat::findUiUnit(World, UnitId) == nullptr)
	{
		return FStratResult::Fail(FString::Printf(TEXT("no unit with id %d"), UnitId));
	}

	// The one line this method exists for. Nothing above it narrowed the set and
	// nothing below it reorders, re-costs or trims one -- §2.5's "the UI queries
	// the module and never recomputes movement" is structural here only for as
	// long as that stays true.
	OutReach = strat::uiReachable(World, UnitId);
	return FStratResult::Ok();
}

FStratResult FStratBridge::Forecast(int32 AttackerId, const strat::Hex& DefenderHex,
                                    strat::UiForecast& OutForecast) const
{
	// Reset up front, exactly as Reachable clears its vector: a refusal must not
	// leave the caller holding a previous forecast's damage number and read it as
	// this one's.
	OutForecast = strat::UiForecast();

	if (!bDefinitionsLoaded)
	{
		return FStratResult::Fail(TEXT("definitions are not loaded"));
	}
	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}

	// Reachable's reason, unchanged: MakeUiWorld SKIPS a unit whose defIndex is
	// outside the loaded table, so without this the attacker would arrive at
	// `findUiUnit` as "no unit with id N" -- blaming the caller's argument for a
	// fault in the table.
	for (const strat::GameUnit& U : GameState.units)
	{
		if (U.defIndex < 0 || static_cast<size_t>(U.defIndex) >= Units.size())
		{
			return FStratResult::Fail(FString::Printf(
				TEXT("unit %d carries defIndex %d, outside the loaded unit table"),
				U.id, U.defIndex));
		}
	}

	const strat::UiWorld World = MakeUiWorld();

	// The malformed-question case, refused here rather than left to the module.
	// See the header: `uiForecast` spells "no such unit" the same way it spells
	// "out of range", and those are not the same kind of thing.
	if (strat::findUiUnit(World, AttackerId) == nullptr)
	{
		return FStratResult::Fail(FString::Printf(TEXT("no unit with id %d"), AttackerId));
	}

	// The one line this method exists for. Nothing above it computed a distance
	// and nothing below it adjusts a damage number -- T-UI-01's "the forecast is
	// exactly what resolves" is structural only for as long as that holds. The
	// DEFENDER is deliberately not pre-checked: an empty hex, a friendly unit and
	// a target out of range are answers this returns with the module's own reason.
	OutForecast = strat::uiForecast(World, AttackerId, DefenderHex);
	return FStratResult::Ok();
}

// ---------------------------------------------------------------------------
// Recorded log -> §4.10 save (row 10 part (a)).
// ---------------------------------------------------------------------------

FStratResult FStratBridge::SerializeRecordedSave(const FStratSaveIdentity& Identity,
                                                 FString&                  OutText) const
{
	OutText.Reset();

	if (!bDefinitionsLoaded)
	{
		return FStratResult::Fail(TEXT("definitions are not loaded"));
	}
	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}

	// `result` is read off a projection rather than compared here. Ui.good.cpp:228
	// owns the InProgress-is-null mapping and `tierName` owns the spelling; a
	// `tier != InProgress` written in this file would be a second copy of the first
	// and a second spelling of the second, and the save would then be able to
	// disagree with the scoreboard about whether the match is over.
	strat::UiSnapshot Snapshot;
	const FStratResult Projected = MakeUiSnapshot(Snapshot);
	if (!Projected.bOk)
	{
		return Projected;
	}

	strat::Save S;
	S.formatVersion = strat::kFormatVersion;
	S.rulesCommit   = ToStd(Identity.RulesCommit);
	S.dataHash      = ToStd(Identity.DataHash);
	// The loaded scenario's own id and the module's digest over it -- not a
	// caller's claim about which scenario this is.
	S.scenarioId    = LoadedScenario.scenarioId;
	S.scenarioHash  = strat::scenarioHash(LoadedScenario);
	// Save.h:78: reserved, MUST be 0. No RNG ships, so there is nothing for a
	// non-zero value to mean.
	S.seed          = 0;
	S.commandLog    = Recorded;
	S.stateHash     = strat::canonicalStateHash(GameState);
	S.hasResult     = Snapshot.match.hasResult;
	S.result        = S.hasResult ? std::string(strat::tierName(Snapshot.match.resultTier))
	                              : std::string();

	OutText = FromStd(strat::serializeSave(S));
	return FStratResult::Ok();
}
