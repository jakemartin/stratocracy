// T-DATA-05 -- the in-editor Unreal Automation parity pass. GDD §4.8, §4.7 Stub 2.
//
// "Every imported DataTable row equals the CSV field-for-field, and EUnitType
// mirrors strat::UnitType exactly."
//
// Two things this file is deliberately NOT allowed to do:
//
// 1. It does not parse the CSV with FCsvParser. That is what UDataTableFactory
//    uses to import, so a comparison built on it would re-run the importer's own
//    code and agree with itself by construction. The parser below is a plain
//    split on ',' and '\n' -- the canonical files carry no quoted fields, no
//    embedded commas and no escapes, which the shape check in ParseCsv asserts
//    rather than assumes. An independent reading is the whole point of a parity
//    gate.
//
// 2. It does not read the row values from any shared constant. The expected
//    numbers come from the vendored CSV bytes; the actual numbers come from the
//    imported asset. Nothing in between is authored twice.
//
// GATE-DATA-VENDOR is an unnumbered check on the GATE-AI-SMOKE / GATE-CAP-PARTIAL
// precedent: it mints no acceptance ID. It asserts the CSVs this gate compares
// against are the ones the manifest records at the crew commit, so a parity PASS
// cannot be bought by editing the vendored copy to match a drifted asset.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/UObjectGlobals.h"

#include "StratData/StratDataRows.h"

// The vendored headless rules module. Including it is what makes the enum mirror
// a real comparison rather than a restatement of a literal: `strat::UnitType` is
// the thing EUnitType claims to mirror, so the check has to see it.
#include "Combat.h"

// ---------------------------------------------------------------------------
// The enum mirror, at compile time.
//
// T-DATA-05 asserts the mirror is exact. These static_asserts make a reorder of
// EITHER enum stop the build, rather than compiling cleanly and waiting for
// someone to run the gate. The runtime half below still checks the reflection
// data, because a UENUM can also drift in ways a cast cannot see -- a renamed
// enumerator, or a fifth one appended.
// ---------------------------------------------------------------------------
static_assert(static_cast<int32>(EUnitType::Infantry)
	== static_cast<int32>(strat::UnitType::Infantry), "EUnitType::Infantry must mirror strat::UnitType::Infantry");
static_assert(static_cast<int32>(EUnitType::Tank)
	== static_cast<int32>(strat::UnitType::Tank), "EUnitType::Tank must mirror strat::UnitType::Tank");
static_assert(static_cast<int32>(EUnitType::Artillery)
	== static_cast<int32>(strat::UnitType::Artillery), "EUnitType::Artillery must mirror strat::UnitType::Artillery");
static_assert(static_cast<int32>(EUnitType::Recon)
	== static_cast<int32>(strat::UnitType::Recon), "EUnitType::Recon must mirror strat::UnitType::Recon");

namespace StratDataParity
{
	/** One parsed CSV: the header row plus the data rows, split on ',' only. */
	struct FCsvTable
	{
		TArray<FString> Header;
		TArray<TArray<FString>> Rows;
	};

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	/**
	 * Deliberately minimal. Returns false and fills OutError rather than
	 * asserting, so a malformed canonical file is a named gate failure instead
	 * of a crash in the harness.
	 */
	static bool ParseCsv(const FString& AbsPath, FCsvTable& Out, FString& OutError)
	{
		FString Raw;
		if (!FFileHelper::LoadFileToString(Raw, *AbsPath))
		{
			OutError = FString::Printf(TEXT("could not read %s"), *AbsPath);
			return false;
		}

		// The independence claim above rests on these files carrying no quoting.
		// Check it rather than assume it: if a quote ever appears, this parser is
		// the wrong tool and should say so instead of silently mis-splitting.
		if (Raw.Contains(TEXT("\"")))
		{
			OutError = FString::Printf(
				TEXT("%s contains a quote character; this parser does not handle "
				     "quoted fields and must not guess"), *AbsPath);
			return false;
		}

		TArray<FString> Lines;
		Raw.ParseIntoArrayLines(Lines, /*bCullEmpty=*/true);
		if (Lines.Num() < 2)
		{
			OutError = FString::Printf(TEXT("%s has %d line(s); expected a header and at least one row"),
				*AbsPath, Lines.Num());
			return false;
		}

		Lines[0].ParseIntoArray(Out.Header, TEXT(","), /*InCullEmpty=*/false);
		for (int32 i = 1; i < Lines.Num(); ++i)
		{
			TArray<FString> Cells;
			Lines[i].ParseIntoArray(Cells, TEXT(","), /*InCullEmpty=*/false);
			if (Cells.Num() != Out.Header.Num())
			{
				OutError = FString::Printf(TEXT("%s line %d has %d cells, header has %d"),
					*AbsPath, i + 1, Cells.Num(), Out.Header.Num());
				return false;
			}
			for (FString& Cell : Cells)
			{
				Cell.TrimStartAndEndInline();
			}
			Out.Rows.Add(MoveTemp(Cells));
		}
		return true;
	}

	static int32 ColumnIndex(const FCsvTable& Table, const TCHAR* Name)
	{
		return Table.Header.IndexOfByPredicate(
			[Name](const FString& H) { return H.Equals(Name, ESearchCase::CaseSensitive); });
	}

	static bool AsBool(const FString& Cell)
	{
		return Cell.Equals(TEXT("true"), ESearchCase::IgnoreCase);
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	// -----------------------------------------------------------------------
	// SHA-256, implemented here.
	//
	// NOT a preference: FPlatformMisc::GetSHA256Signature is unimplemented on
	// Windows and asserts at runtime with "No SHA256 Platform implementation",
	// which crashed this gate before this replaced it. Core's FSHA1/FMD5 are
	// available but would mean recording a second digest in the manifest beside
	// the sha256 the crew side already writes with hashlib.
	//
	// Being independent of BOTH the writer (Python hashlib) and the platform
	// layer is a property, not a cost: the comparison shares no code with the
	// thing it checks. The function is proven against the NIST vector for "abc"
	// as the first assertion of the gate that uses it, so a broken digest fails
	// loudly instead of agreeing with itself.
	// -----------------------------------------------------------------------
	static FString Sha256Hex(const uint8* Data, int64 Len)
	{
		static const uint32 K[64] = {
			0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
			0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
			0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
			0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
			0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
			0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
			0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
			0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2 };

		uint32 H[8] = { 0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
						0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19 };

		auto Ror = [](uint32 X, uint32 N) { return (X >> N) | (X << (32 - N)); };

		// Message + 0x80 + zero pad to 56 mod 64 + 8-byte big-endian bit length.
		TArray<uint8> Msg;
		Msg.Append(Data, Len);
		Msg.Add(0x80);
		while (Msg.Num() % 64 != 56)
		{
			Msg.Add(0x00);
		}
		const uint64 BitLen = static_cast<uint64>(Len) * 8;
		for (int32 i = 7; i >= 0; --i)
		{
			Msg.Add(static_cast<uint8>((BitLen >> (i * 8)) & 0xff));
		}

		for (int32 Off = 0; Off < Msg.Num(); Off += 64)
		{
			uint32 W[64];
			for (int32 i = 0; i < 16; ++i)
			{
				W[i] = (static_cast<uint32>(Msg[Off + i * 4 + 0]) << 24)
					 | (static_cast<uint32>(Msg[Off + i * 4 + 1]) << 16)
					 | (static_cast<uint32>(Msg[Off + i * 4 + 2]) << 8)
					 | (static_cast<uint32>(Msg[Off + i * 4 + 3]));
			}
			for (int32 i = 16; i < 64; ++i)
			{
				const uint32 S0 = Ror(W[i - 15], 7) ^ Ror(W[i - 15], 18) ^ (W[i - 15] >> 3);
				const uint32 S1 = Ror(W[i - 2], 17) ^ Ror(W[i - 2], 19) ^ (W[i - 2] >> 10);
				W[i] = W[i - 16] + S0 + W[i - 7] + S1;
			}

			uint32 a = H[0], b = H[1], c = H[2], d = H[3];
			uint32 e = H[4], f = H[5], g = H[6], h = H[7];
			for (int32 i = 0; i < 64; ++i)
			{
				const uint32 S1 = Ror(e, 6) ^ Ror(e, 11) ^ Ror(e, 25);
				const uint32 Ch = (e & f) ^ (~e & g);
				const uint32 T1 = h + S1 + Ch + K[i] + W[i];
				const uint32 S0 = Ror(a, 2) ^ Ror(a, 13) ^ Ror(a, 22);
				const uint32 Maj = (a & b) ^ (a & c) ^ (b & c);
				const uint32 T2 = S0 + Maj;
				h = g; g = f; f = e; e = d + T1;
				d = c; c = b; b = a; a = T1 + T2;
			}
			H[0] += a; H[1] += b; H[2] += c; H[3] += d;
			H[4] += e; H[5] += f; H[6] += g; H[7] += h;
		}

		FString Out;
		for (int32 i = 0; i < 8; ++i)
		{
			Out += FString::Printf(TEXT("%08x"), H[i]);
		}
		return Out;
	}
}

// ---------------------------------------------------------------------------
// GATE-DATA-VENDOR -- the compared bytes are the recorded ones.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratDataVendorManifestTest,
	"Stratocracy.StratData.GATE-DATA-VENDOR.VendoredCsvMatchesManifest",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratDataVendorManifestTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDataParity;

	// Prove the instrument before using it. If the digest is wrong, every
	// comparison below is meaningless, so this runs first and the gate says so
	// rather than reporting a data defect that is really a hash defect.
	// NIST FIPS 180-4 vector for the three-byte message "abc".
	{
		const uint8 Abc[] = { 'a', 'b', 'c' };
		TestEqual(TEXT("SHA-256 reproduces the NIST vector for \"abc\""),
			Sha256Hex(Abc, 3),
			FString(TEXT("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad")));
	}

	const FString ManifestPath = FPaths::Combine(DataDir(), TEXT("StratData.manifest.json"));
	FString ManifestRaw;
	if (!TestTrue(FString::Printf(TEXT("manifest readable at %s"), *ManifestPath),
		FFileHelper::LoadFileToString(ManifestRaw, *ManifestPath)))
	{
		return false;
	}

	TSharedPtr<FJsonObject> Root;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ManifestRaw);
	if (!TestTrue(TEXT("manifest parses as JSON"), FJsonSerializer::Deserialize(Reader, Root) && Root.IsValid()))
	{
		return false;
	}

	const TSharedPtr<FJsonObject>* Files = nullptr;
	if (!TestTrue(TEXT("manifest carries a files object"), Root->TryGetObjectField(TEXT("files"), Files)))
	{
		return false;
	}

	FString DataCommit;
	Root->TryGetStringField(TEXT("dataCommit"), DataCommit);
	AddInfo(FString::Printf(TEXT("manifest dataCommit = %s"), *DataCommit));

	// Every file the manifest names must be present and hash-equal. A file on
	// disk that the manifest does NOT name is also a failure: it would be a table
	// this gate never compares, which is the silent-drift case the manifest exists
	// to prevent.
	TSet<FString> Recorded;
	for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : (*Files)->Values)
	{
		const FString Name = Pair.Key;
		Recorded.Add(Name);

		const FString Expected = Pair.Value.IsValid() ? Pair.Value->AsString() : FString();
		const FString Path = FPaths::Combine(DataDir(), Name);

		TArray<uint8> Bytes;
		if (!TestTrue(FString::Printf(TEXT("%s present"), *Name),
			FFileHelper::LoadFileToArray(Bytes, *Path)))
		{
			continue;
		}

		// Sha256Hex renders lowercase, matching hashlib's hexdigest() on the
		// writing side; Expected is lowered anyway so neither side has to know
		// the other's casing.
		const FString Actual = Sha256Hex(Bytes.GetData(), Bytes.Num());

		TestEqual(*FString::Printf(TEXT("%s sha256 matches the manifest"), *Name),
			Actual, Expected.ToLower());
	}

	TArray<FString> OnDisk;
	IFileManager::Get().FindFiles(OnDisk, *(DataDir() / TEXT("*.csv")), true, false);
	for (const FString& Name : OnDisk)
	{
		TestTrue(FString::Printf(TEXT("%s is recorded in the manifest"), *Name), Recorded.Contains(Name));
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-DATA-05, part 1 -- EUnitType mirrors strat::UnitType exactly.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratDataEnumMirrorTest,
	"Stratocracy.StratData.T-DATA-05.EUnitTypeMirrorsStratUnitType",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratDataEnumMirrorTest::RunTest(const FString& /*Parameters*/)
{
	const UEnum* Enum = StaticEnum<EUnitType>();
	if (!TestNotNull(TEXT("EUnitType is reflected"), Enum))
	{
		return false;
	}

	// NumEnums() counts the implicit _MAX entry, so the declared count is one
	// fewer. Getting this wrong would make the gate pass a five-enumerator enum.
	const int32 Declared = Enum->NumEnums() - 1;
	TestEqual(TEXT("EUnitType declares exactly four enumerators"), Declared, 4);

	// The pinned order from §4.8. Paired with the static_asserts at the top of
	// this file, this catches a rename or an appended enumerator, which a value
	// cast alone cannot see.
	const TCHAR* const ExpectedNames[] = { TEXT("Infantry"), TEXT("Tank"), TEXT("Artillery"), TEXT("Recon") };
	const int64 ExpectedValues[] = {
		static_cast<int64>(strat::UnitType::Infantry),
		static_cast<int64>(strat::UnitType::Tank),
		static_cast<int64>(strat::UnitType::Artillery),
		static_cast<int64>(strat::UnitType::Recon),
	};

	for (int32 i = 0; i < 4 && i < Declared; ++i)
	{
		TestEqual(FString::Printf(TEXT("EUnitType enumerator %d is named %s"), i, ExpectedNames[i]),
			Enum->GetNameStringByIndex(i), FString(ExpectedNames[i]));
		TestEqual(FString::Printf(TEXT("EUnitType::%s equals strat::UnitType::%s"), ExpectedNames[i], ExpectedNames[i]),
			Enum->GetValueByIndex(i), ExpectedValues[i]);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-DATA-05, part 2 -- every imported unit row equals the CSV field-for-field.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratDataUnitParityTest,
	"Stratocracy.StratData.T-DATA-05.UnitTableMatchesCsv",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratDataUnitParityTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDataParity;

	UDataTable* Table = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
	if (!TestNotNull(TEXT("DT_Units imported and loadable"), Table))
	{
		return false;
	}
	TestTrue(TEXT("DT_Units row struct is FUnitRow"),
		Table->GetRowStruct() == FUnitRow::StaticStruct());

	FCsvTable Csv;
	FString Error;
	if (!TestTrue(TEXT("units.csv parses"), ParseCsv(FPaths::Combine(DataDir(), TEXT("units.csv")), Csv, Error)))
	{
		AddError(Error);
		return false;
	}

	// Row COUNT parity in both directions. A table with an extra row and a table
	// missing one are different defects and both must fail.
	TestEqual(TEXT("DT_Units row count equals units.csv row count"),
		Table->GetRowMap().Num(), Csv.Rows.Num());
	TestEqual(TEXT("units.csv has exactly four rows (§2.4)"), Csv.Rows.Num(), 4);

	const int32 IdCol = ColumnIndex(Csv, TEXT("Id"));
	if (!TestTrue(TEXT("units.csv has an Id column"), IdCol != INDEX_NONE))
	{
		return false;
	}

	const UEnum* Enum = StaticEnum<EUnitType>();

	for (const TArray<FString>& Row : Csv.Rows)
	{
		const FString Id = Row[IdCol];
		const FUnitRow* Actual = Table->FindRow<FUnitRow>(FName(*Id), TEXT("T-DATA-05"), /*bWarnIfMissing=*/false);
		if (!TestNotNull(*FString::Printf(TEXT("DT_Units has row %s"), *Id), Actual))
		{
			continue;
		}

		auto Cell = [&](const TCHAR* Name) -> FString
		{
			const int32 Col = ColumnIndex(Csv, Name);
			return Col == INDEX_NONE ? FString() : Row[Col];
		};
		auto CheckInt = [&](const TCHAR* Name, int32 ActualValue)
		{
			const int32 Col = ColumnIndex(Csv, Name);
			if (!TestTrue(*FString::Printf(TEXT("units.csv has column %s"), Name), Col != INDEX_NONE))
			{
				return;
			}
			TestEqual(*FString::Printf(TEXT("%s.%s"), *Id, Name), ActualValue, FCString::Atoi(*Row[Col]));
		};

		CheckInt(TEXT("HP"), Actual->HP);
		CheckInt(TEXT("Move"), Actual->Move);
		CheckInt(TEXT("Atk"), Actual->Atk);
		CheckInt(TEXT("Def"), Actual->Def);
		CheckInt(TEXT("RangeMin"), Actual->RangeMin);
		CheckInt(TEXT("RangeMax"), Actual->RangeMax);
		CheckInt(TEXT("CostFame"), Actual->CostFame);

		TestEqual(*FString::Printf(TEXT("%s.CanCapture"), *Id),
			Actual->CanCapture, AsBool(Cell(TEXT("CanCapture"))));

		// The Type cell is an enum STRING in the CSV; compare it by name rather
		// than by index, so a reordered enum fails here too and not only in the
		// mirror test.
		const FString TypeCell = Cell(TEXT("Type"));
		const FString ActualTypeName = Enum ? Enum->GetNameStringByValue(static_cast<int64>(Actual->Type)) : FString();
		TestEqual(*FString::Printf(TEXT("%s.Type"), *Id), ActualTypeName, TypeCell);
	}

	// §4.8 reserves MoveClass and blocks it on Q2. It must be in neither the
	// canonical file nor the row struct: a reserved column that quietly became a
	// real one would be a rules change arriving through the data layer.
	TestEqual(TEXT("units.csv does not carry the reserved MoveClass column (blocked on Q2)"),
		ColumnIndex(Csv, TEXT("MoveClass")), INDEX_NONE);
	TestNull(TEXT("FUnitRow declares no MoveClass property (blocked on Q2)"),
		FUnitRow::StaticStruct()->FindPropertyByName(TEXT("MoveClass")));

	return true;
}

// ---------------------------------------------------------------------------
// T-DATA-05, part 3 -- every imported terrain row equals the CSV field-for-field.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratDataTerrainParityTest,
	"Stratocracy.StratData.T-DATA-05.TerrainTableMatchesCsv",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratDataTerrainParityTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDataParity;

	UDataTable* Table = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
	if (!TestNotNull(TEXT("DT_Terrain imported and loadable"), Table))
	{
		return false;
	}
	TestTrue(TEXT("DT_Terrain row struct is FTerrainRow"),
		Table->GetRowStruct() == FTerrainRow::StaticStruct());

	FCsvTable Csv;
	FString Error;
	if (!TestTrue(TEXT("terrain.csv parses"), ParseCsv(FPaths::Combine(DataDir(), TEXT("terrain.csv")), Csv, Error)))
	{
		AddError(Error);
		return false;
	}

	TestEqual(TEXT("DT_Terrain row count equals terrain.csv row count"),
		Table->GetRowMap().Num(), Csv.Rows.Num());
	TestEqual(TEXT("terrain.csv has exactly seven rows (§2.3)"), Csv.Rows.Num(), 7);

	const int32 IdCol = ColumnIndex(Csv, TEXT("Id"));
	if (!TestTrue(TEXT("terrain.csv has an Id column"), IdCol != INDEX_NONE))
	{
		return false;
	}

	for (const TArray<FString>& Row : Csv.Rows)
	{
		const FString Id = Row[IdCol];
		const FTerrainRow* Actual = Table->FindRow<FTerrainRow>(FName(*Id), TEXT("T-DATA-05"), /*bWarnIfMissing=*/false);
		if (!TestNotNull(*FString::Printf(TEXT("DT_Terrain has row %s"), *Id), Actual))
		{
			continue;
		}

		auto Cell = [&](const TCHAR* Name) -> FString
		{
			const int32 Col = ColumnIndex(Csv, Name);
			return Col == INDEX_NONE ? FString() : Row[Col];
		};
		auto CheckInt = [&](const TCHAR* Name, int32 ActualValue)
		{
			const int32 Col = ColumnIndex(Csv, Name);
			if (!TestTrue(*FString::Printf(TEXT("terrain.csv has column %s"), Name), Col != INDEX_NONE))
			{
				return;
			}
			TestEqual(*FString::Printf(TEXT("%s.%s"), *Id, Name), ActualValue, FCString::Atoi(*Row[Col]));
		};
		auto CheckBool = [&](const TCHAR* Name, bool bActualValue)
		{
			TestEqual(*FString::Printf(TEXT("%s.%s"), *Id, Name), bActualValue, AsBool(Cell(Name)));
		};

		// DefensePct is signed and Bridge ships -10; Atoi handles the sign, and
		// the Bridge row is what would catch a struct that made this unsigned.
		CheckInt(TEXT("MoveCost"), Actual->MoveCost);
		CheckInt(TEXT("DefensePct"), Actual->DefensePct);
		CheckInt(TEXT("IncomeFame"), Actual->IncomeFame);

		CheckBool(TEXT("PassLand"), Actual->PassLand);
		CheckBool(TEXT("PassAir"), Actual->PassAir);
		CheckBool(TEXT("PassSea"), Actual->PassSea);
		CheckBool(TEXT("Capturable"), Actual->Capturable);
		CheckBool(TEXT("IsSpawnPoint"), Actual->IsSpawnPoint);
		CheckBool(TEXT("IsRepairPoint"), Actual->IsRepairPoint);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-DATA-05, part 4 -- the effectiveness matrix equals the CSV field-for-field.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratDataEffectivenessParityTest,
	"Stratocracy.StratData.T-DATA-05.EffectivenessTableMatchesCsv",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratDataEffectivenessParityTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDataParity;

	UDataTable* Table = LoadTable(TEXT("/Game/StratData/DT_Effectiveness.DT_Effectiveness"));
	if (!TestNotNull(TEXT("DT_Effectiveness imported and loadable"), Table))
	{
		return false;
	}
	TestTrue(TEXT("DT_Effectiveness row struct is FEffectivenessRow"),
		Table->GetRowStruct() == FEffectivenessRow::StaticStruct());

	FCsvTable Csv;
	FString Error;
	if (!TestTrue(TEXT("effectiveness.csv parses"),
		ParseCsv(FPaths::Combine(DataDir(), TEXT("effectiveness.csv")), Csv, Error)))
	{
		AddError(Error);
		return false;
	}

	// §4.8: a 4x4 matrix. Both dimensions are checked -- the header carries the
	// attacker key plus four defender columns.
	TestEqual(TEXT("effectiveness.csv has exactly four rows"), Csv.Rows.Num(), 4);
	TestEqual(TEXT("effectiveness.csv has four defender columns plus the attacker key"),
		Csv.Header.Num(), 5);
	TestEqual(TEXT("DT_Effectiveness row count equals effectiveness.csv row count"),
		Table->GetRowMap().Num(), Csv.Rows.Num());

	const int32 IdCol = ColumnIndex(Csv, TEXT("Attacker"));
	if (!TestTrue(TEXT("effectiveness.csv has an Attacker column"), IdCol != INDEX_NONE))
	{
		return false;
	}

	// The pinned type order, positionally. §4.8 says the matrix is indexed in
	// that order, so the gate asserts the file is laid out in it rather than
	// accepting any permutation that happens to contain the right names.
	const TCHAR* const Pinned[] = { TEXT("Infantry"), TEXT("Tank"), TEXT("Artillery"), TEXT("Recon") };
	for (int32 i = 0; i < 4; ++i)
	{
		TestEqual(*FString::Printf(TEXT("effectiveness.csv column %d is %s"), i + 1, Pinned[i]),
			Csv.Header.IsValidIndex(i + 1) ? Csv.Header[i + 1] : FString(), FString(Pinned[i]));
		TestEqual(*FString::Printf(TEXT("effectiveness.csv row %d is %s"), i, Pinned[i]),
			Csv.Rows.IsValidIndex(i) ? Csv.Rows[i][IdCol] : FString(), FString(Pinned[i]));
	}

	for (const TArray<FString>& Row : Csv.Rows)
	{
		const FString Id = Row[IdCol];
		const FEffectivenessRow* Actual =
			Table->FindRow<FEffectivenessRow>(FName(*Id), TEXT("T-DATA-05"), /*bWarnIfMissing=*/false);
		if (!TestNotNull(*FString::Printf(TEXT("DT_Effectiveness has row %s"), *Id), Actual))
		{
			continue;
		}

		const float ActualCells[] = { Actual->Infantry, Actual->Tank, Actual->Artillery, Actual->Recon };
		for (int32 i = 0; i < 4; ++i)
		{
			const int32 Col = ColumnIndex(Csv, Pinned[i]);
			if (!TestTrue(*FString::Printf(TEXT("effectiveness.csv has column %s"), Pinned[i]), Col != INDEX_NONE))
			{
				continue;
			}
			const float Expected = FCString::Atof(*Row[Col]);
			TestEqual(*FString::Printf(TEXT("effectiveness[%s][%s]"), *Id, Pinned[i]),
				ActualCells[i], Expected, /*Tolerance=*/KINDA_SMALL_NUMBER);

			// T-DATA-06's domain, re-asserted here against the IMPORTED value.
			// The headless gate asserts it against the CSV; this side is what
			// would catch an editor-side edit to the asset alone.
			const bool bInDomain = FMath::IsNearlyEqual(ActualCells[i], 0.5f)
				|| FMath::IsNearlyEqual(ActualCells[i], 1.0f)
				|| FMath::IsNearlyEqual(ActualCells[i], 1.5f);
			TestTrue(*FString::Printf(TEXT("effectiveness[%s][%s] is in {0.5, 1.0, 1.5}"), *Id, Pinned[i]),
				bInDomain);
		}
	}

	return true;
}
