#include "StratData/ImportStratDataCommandlet.h"

#include "StratData/StratDataRows.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/DataTable.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

DEFINE_LOG_CATEGORY_STATIC(LogImportStratData, Log, All);

UImportStratDataCommandlet::UImportStratDataCommandlet()
{
	IsClient = false;
	IsServer = false;
	IsEditor = true;
	LogToConsole = true;
}

namespace
{
	struct FTableSpec
	{
		const TCHAR* CsvName;
		const TCHAR* AssetName;
		UScriptStruct* RowStruct;
	};

#if WITH_EDITOR
	/**
	 * Imports one CSV into /Game/StratData/<AssetName> and saves the package.
	 *
	 * The CSV is parsed here by UDataTable's own importer, which is the correct
	 * side to use it on: §4.8 says the editor IMPORTS the same file, and this is
	 * that import. The parity gate (T-DATA-05) deliberately re-reads the CSV with
	 * an independent parser, so the two sides never share a reader.
	 */
	bool ImportOne(const FTableSpec& Spec, const FString& DataDir)
	{
		const FString CsvPath = FPaths::Combine(DataDir, Spec.CsvName);

		FString CsvText;
		if (!FFileHelper::LoadFileToString(CsvText, *CsvPath))
		{
			UE_LOG(LogImportStratData, Error,
				TEXT("missing vendored CSV %s -- run sync_stratdata.py in the crew repo first"),
				*CsvPath);
			return false;
		}

		const FString PackageName = FString::Printf(TEXT("/Game/StratData/%s"), Spec.AssetName);
		UPackage* Package = CreatePackage(*PackageName);
		if (!Package)
		{
			UE_LOG(LogImportStratData, Error, TEXT("could not create package %s"), *PackageName);
			return false;
		}
		Package->FullyLoad();

		UDataTable* Table = NewObject<UDataTable>(
			Package, UDataTable::StaticClass(), FName(Spec.AssetName),
			RF_Public | RF_Standalone);
		Table->RowStruct = Spec.RowStruct;

		// CreateTableFromCSVString returns the problems it found rather than
		// throwing. An import that half-succeeded would leave a table this gate
		// then compares and reports as a data defect, so treat any problem as a
		// hard failure of the import step itself and say which row it was.
		const TArray<FString> Problems = Table->CreateTableFromCSVString(CsvText);
		if (Problems.Num() > 0)
		{
			for (const FString& Problem : Problems)
			{
				UE_LOG(LogImportStratData, Error, TEXT("%s: %s"), Spec.CsvName, *Problem);
			}
			return false;
		}

		FAssetRegistryModule::AssetCreated(Table);
		Package->MarkPackageDirty();

		const FString FileName = FPackageName::LongPackageNameToFilename(
			PackageName, FPackageName::GetAssetPackageExtension());

		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_NoError;

		if (!UPackage::SavePackage(Package, Table, *FileName, SaveArgs))
		{
			UE_LOG(LogImportStratData, Error, TEXT("failed to save %s"), *FileName);
			return false;
		}

		UE_LOG(LogImportStratData, Display, TEXT("IMPORTED %s <- %s : %d rows -> %s"),
			*PackageName, Spec.CsvName, Table->GetRowMap().Num(), *FileName);
		return true;
	}
#endif // WITH_EDITOR
}

int32 UImportStratDataCommandlet::Main(const FString& /*Params*/)
{
#if WITH_EDITOR
	const FString DataDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	UE_LOG(LogImportStratData, Display, TEXT("=== ImportStratData from %s ==="), *DataDir);

	const FTableSpec Specs[] = {
		{ TEXT("units.csv"),         TEXT("DT_Units"),         FUnitRow::StaticStruct() },
		{ TEXT("terrain.csv"),       TEXT("DT_Terrain"),       FTerrainRow::StaticStruct() },
		{ TEXT("effectiveness.csv"), TEXT("DT_Effectiveness"), FEffectivenessRow::StaticStruct() },
	};

	int32 Failures = 0;
	for (const FTableSpec& Spec : Specs)
	{
		if (!ImportOne(Spec, DataDir))
		{
			++Failures;
		}
	}

	if (Failures > 0)
	{
		UE_LOG(LogImportStratData, Error, TEXT("=== ImportStratData FAILED: %d of %d ==="),
			Failures, UE_ARRAY_COUNT(Specs));
		return 1;
	}

	UE_LOG(LogImportStratData, Display, TEXT("=== ImportStratData OK: %d tables ==="),
		UE_ARRAY_COUNT(Specs));
	return 0;
#else
	UE_LOG(LogImportStratData, Error, TEXT("ImportStratData is editor-only."));
	return 1;
#endif
}
