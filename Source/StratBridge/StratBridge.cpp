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
