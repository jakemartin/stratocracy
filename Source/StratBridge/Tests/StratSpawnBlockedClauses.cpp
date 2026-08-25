// GDD §2.7 / §2.11.5 -- what `spawnBlocked` reports, and what it deliberately does not.
// T-UI-04 (the buildlist) and T-UI-05 (snapshot fidelity), with Q31 ruled 2026-08-22.
//
// THE ASYMMETRY THESE TWO CLAUSES EXIST TO PIN IN THE TREE, so that no later pass
// re-derives it and no later widget is built against the wrong belief:
//
//   `strat::spawnHexesBlocked` IS OCCUPANCY-ONLY AND IS TRUE ONLY WHEN THE FACTORY HEX AND
//   EVERY IN-BOUNDS NEIGHBOUR ARE OCCUPIED. Under DISPLACEMENT -- factory hex occupied, a
//   neighbour free, the built unit placed on the neighbour -- it is FALSE.
//
// That matters because the reported symptom that put a footer on §2.11.5's list was
// displacement: a unit appearing somewhere the player did not expect, with nothing said. A
// widget bound to `bSpawnBlocked` would stay silent on exactly that case. Displacement
// feedback appears in NO GDD section and `FStratFactoryView` is a complete, faithful mirror
// of §4.7 Stub 8's per-factory block, so there is no field to read: the question is filed
// for a ruling in `Tools/architect/state/decisions.md` and nothing here builds it. What
// these clauses do is make the asymmetry a measurement.
//
// WHAT THE EXISTING COVERAGE COULD NOT SEE, and this is why two more clauses rather than an
// edit to one. `FStratProductionMenuBuildReconcilesTheBoardTest::RunTest`
// (`Source/StratPlay/Tests/StratProductionMenuSeam.cpp`) picks its row through
// `FindABuildableRow` and its factory through `FindHeldFactory` -- the first owned factory,
// with no occupancy check -- and then asserts actor-id SET EQUALITY plus one. A neighbour
// spawn satisfies that exactly as well as a factory-hex spawn, so displacement is invisible
// to it. It does NOT pass silently on a boxed-in build -- its COVERAGE guard bails out --
// which is a different and better failure, and is why that clause is left alone.
//
// BOTH CLAUSES SEED THE BOARD DELIBERATELY RATHER THAN SELECTING A ROW. Each writes a
// variant of the shipped scenario under `Saved/StratTests/` -- never under `Data/`, which is
// vendored and hash-gated -- moving one or three placements and nothing else. The variant is
// deleted at the end of the clause that wrote it. Same helper shape and same reasoning as
// `StratBridgeRestoreParity.cpp`'s `turnCap` variant.
//
// WHERE THE EXPECTATIONS COME FROM, AND WHY THIS FILE IS IN StratBridge/Tests RATHER THAN
// BESIDE THE SEAM CLAUSES IN StratPlay. "The unit landed on a NEIGHBOUR" needs an adjacency
// answer, and this suite's rule is that a test never computes what a module can be asked.
// `strat::neighbors(factoryHex, bounds, out)` IS that answer -- the same function
// `spawnHexesBlocked` and `resolveBuilds` both walk -- and it is a `strat::` free function,
// which in any module but this one is `LNK2019` rather than a test. Writing the clause in
// StratPlay would have forced a hex-arithmetic expectation into the test body, which is the
// one thing this suite refuses. Everything else here is read from the bridge: the factory
// hex and its `spawnBlocked` from `MakeUiSnapshot`, the offered rows from
// `FStratBridge::BuildOptions`, the unit set before and after from the same snapshot.
//
// NO ROW IS CHOSEN BY NAME OR INDEX. `FirstBuildableRow` takes the first row the module says
// is both affordable and available, and both clauses fail loudly if there is none rather
// than passing vacuously.
//
// EVERY STRING COMPARISON IS CASE SENSITIVE.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBridge.h"

#include "Hex.h"
#include "Ui.h"

#include <string>
#include <vector>

namespace StratSpawnBlockedClauses
{
	/** The harness pin every bridge parity file in this suite carries. */
	static const int32 kFirstSide = 0;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static FString ShippedScenarioPath()
	{
		return FPaths::Combine(DataDir(), TEXT("ferrum_crossing.json"));
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	static FString FromStd(const std::string& In)
	{
		return FString(UTF8_TO_TCHAR(In.c_str()));
	}

	/** Both axial and odd-r offset, because the scenario file authors the second and the two
	 *  print identically on even rows -- a difference that has cost this project a diagnosis
	 *  before. `StratBuildOptionRouting.cpp` carries the same helper for the same reason. */
	static FString Describe(const strat::Hex& H)
	{
		int Col = 0;
		int Row = 0;
		strat::axialToOffset(H, Col, Row);
		return FString::Printf(TEXT("axial(%d,%d)=offset[%d,%d]"), H.q, H.r, Col, Row);
	}

	static FIntPoint ToIntPoint(const strat::Hex& H)
	{
		return FIntPoint(H.q, H.r);
	}

	static bool SeedBridgeFrom(FStratBridge& Bridge, const FString& ScenarioPath, FString& OutError)
	{
		UDataTable* Units   = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
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

		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath, kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	/**
	 * One placement line of the shipped scenario, rewritten to a different hex.
	 *
	 * LOCATED CASE-SENSITIVELY AND CHECKED FOR BEFORE IT IS MADE, so a re-authored scenario
	 * fails the helper loudly instead of silently producing a variant identical to the
	 * shipped file -- which would leave both clauses below green and measuring nothing.
	 */
	static bool MovePlacement(FString& Text, const TCHAR* From, const TCHAR* To, FString& OutError)
	{
		if (Text.Find(From, ESearchCase::CaseSensitive) == INDEX_NONE)
		{
			OutError = FString::Printf(
				TEXT("ferrum_crossing.json no longer carries the placement '%s'; this file's "
					"board arrangement is stale and the clause would measure the shipped board"),
				From);
			return false;
		}
		Text = Text.Replace(From, To, ESearchCase::CaseSensitive);
		return true;
	}

	/**
	 * The shipped scenario with its declared `scenarioHash` dropped, ready to be edited.
	 *
	 * The hash is optional and DERIVED (`Scenario.h`, reading 4), so dropping the line is the
	 * correct way to say "recompute from content" rather than authoring a number for edited
	 * bytes.
	 */
	static bool LoadScenarioTextWithoutHash(FString& OutText, FString& OutError)
	{
		if (!FFileHelper::LoadFileToString(OutText, *ShippedScenarioPath()))
		{
			OutError = FString::Printf(TEXT("ferrum_crossing.json unreadable at %s"),
				*ShippedScenarioPath());
			return false;
		}

		const int32 KeyAt = OutText.Find(TEXT("\"scenarioHash\""), ESearchCase::CaseSensitive);
		if (KeyAt == INDEX_NONE)
		{
			OutError = TEXT("ferrum_crossing.json does not carry a 'scenarioHash' member to drop");
			return false;
		}
		const int32 LineEnd = OutText.Find(TEXT("\n"), ESearchCase::CaseSensitive,
			ESearchDir::FromStart, KeyAt);
		if (LineEnd == INDEX_NONE)
		{
			OutError = TEXT("the 'scenarioHash' member is on the file's last line; this helper "
			                "expects it to be followed by a newline");
			return false;
		}
		OutText.RemoveAt(KeyAt, (LineEnd + 1) - KeyAt);
		return true;
	}

	static bool WriteVariant(const FString& Text, const TCHAR* FileName,
	                         FString& OutPath, FString& OutError)
	{
		OutPath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("StratTests"), FileName);

		// `SaveStringToFile` does not build a missing tree, and a first run on a clean
		// checkout has no `Saved/StratTests/`.
		IPlatformFile& Files = FPlatformFileManager::Get().GetPlatformFile();
		Files.CreateDirectoryTree(*FPaths::GetPath(OutPath));

		if (!FFileHelper::SaveStringToFile(Text, *OutPath))
		{
			OutError = FString::Printf(TEXT("could not write the variant scenario to %s"), *OutPath);
			return false;
		}
		return true;
	}

	static void RemoveVariant(const FString& Path)
	{
		IPlatformFile& Files = FPlatformFileManager::Get().GetPlatformFile();
		if (!Path.IsEmpty() && Files.FileExists(*Path))
		{
			Files.DeleteFile(*Path);
		}
	}

	// The three placement edits, named once so both clauses share the same board vocabulary.
	//
	//   THE DISPLACEMENT EDIT puts one of side 0's Infantry ON its own factory hex -- offset
	//   [1, 4], the hex `ownership` gives side 0 -- so the factory hex is occupied while
	//   in-bounds neighbours remain free. §2.7's spawn rule then places a build on the
	//   canonically smallest free neighbour, which is displacement exactly.
	//
	//   THE TWO BOXING EDITS bring two of side 1's units onto the remaining free neighbours,
	//   [2, 4] and [1, 3], so that the factory hex and EVERY in-bounds neighbour are occupied
	//   and `spawnHexesBlocked` reports true.
	//
	// NEITHER SIDE'S GUIDED-OPENING INFANTRY IS MOVED. T-SCN-07 requires one `guidedOpening`
	// entry per side naming a hex that holds that side's Infantry, so moving the named unit
	// would make the variant unloadable for a reason that has nothing to do with spawning.
	// Side 0's entry names [1, 5] and side 1's names [9, 3]; the edits below avoid both.
	//
	// AND NO SIDE-1 INFANTRY IS MOVED AT ALL, WHICH IS A MEASUREMENT AND NOT A PREFERENCE.
	// The first version of the boxing arrangement brought a side-1 INFANTRY to [2, 4] and the
	// scenario validator refused the variant outright: "side 0's guided lane (1,5) -> (5,7) is
	// contested: 5 against 5. The opposing seat's cheapest Infantry route to that objective
	// must cost strictly more" (T-SCN-11). §2.11.6-B's lane rule is about INFANTRY routes, so
	// the boxing units are side 1's Artillery and Recon, which are invisible to it.
	static const TCHAR* const kSide0InfantryAt13 =
		TEXT("{\"side\": 0, \"unitId\": \"Infantry\", \"hex\": [1, 3], \"isFlag\": false}");
	static const TCHAR* const kSide0InfantryOnFactory =
		TEXT("{\"side\": 0, \"unitId\": \"Infantry\", \"hex\": [1, 4], \"isFlag\": false}");

	static const TCHAR* const kSide1ArtilleryAt105 =
		TEXT("{\"side\": 1, \"unitId\": \"Artillery\", \"hex\": [10, 5], \"isFlag\": false}");
	static const TCHAR* const kSide1ArtilleryAt24 =
		TEXT("{\"side\": 1, \"unitId\": \"Artillery\", \"hex\": [2, 4], \"isFlag\": false}");

	static const TCHAR* const kSide1ReconAt103 =
		TEXT("{\"side\": 1, \"unitId\": \"Recon\", \"hex\": [10, 3], \"isFlag\": false}");
	static const TCHAR* const kSide1ReconAt13 =
		TEXT("{\"side\": 1, \"unitId\": \"Recon\", \"hex\": [1, 3], \"isFlag\": false}");

	/** The offset hex the two variants build at: side 0's own factory. Named here so the
	 *  clauses can ASSERT that this is what the projection says side 0 owns, rather than
	 *  trusting the literal. */
	static const int32 kFactoryCol = 1;
	static const int32 kFactoryRow = 4;

	/** The factory the clause is about, found in the projection by the hex the variant was
	 *  built around -- and required to be owned by the building side. */
	static bool FindTheFactory(const strat::UiSnapshot& S, int32 Side,
	                           strat::UiFactoryView& Out, FString& OutError)
	{
		const strat::Hex Wanted = strat::offsetToAxial(kFactoryCol, kFactoryRow);
		for (const strat::UiFactoryView& F : S.factories)
		{
			if (strat::hexEqual(F.hex, Wanted))
			{
				if (F.owner != Side)
				{
					OutError = FString::Printf(
						TEXT("the factory at %s is owned by %d, not by side %d; this file's "
							"variant builds at a factory the building side does not hold"),
						*Describe(Wanted), F.owner, Side);
					return false;
				}
				Out = F;
				return true;
			}
		}
		OutError = FString::Printf(TEXT("the projection reports no factory at %s"),
			*Describe(Wanted));
		return false;
	}

	/** The first row the module says this side can both pay for and place. Nothing here
	 *  decides either; both booleans are `uiBuildOptions`' own. */
	static bool FirstBuildableRow(const std::vector<strat::UiBuildOption>& Rows, int32& OutDefIndex)
	{
		for (const strat::UiBuildOption& Row : Rows)
		{
			if (Row.affordable && Row.available)
			{
				OutDefIndex = Row.defIndex;
				return true;
			}
		}
		return false;
	}

	static TSet<int32> UnitIdsOf(const strat::UiSnapshot& S)
	{
		TSet<int32> Ids;
		for (const strat::UiUnitView& U : S.units)
		{
			Ids.Add(U.id);
		}
		return Ids;
	}

	static bool FindUnit(const strat::UiSnapshot& S, int32 Id, strat::UiUnitView& Out)
	{
		for (const strat::UiUnitView& U : S.units)
		{
			if (U.id == Id)
			{
				Out = U;
				return true;
			}
		}
		return false;
	}
}

// ---------------------------------------------------------------------------
// A boxed-in factory reports `spawnBlocked` -- and STILL takes the Build.
//
// Q31, RULED 2026-08-22: a player MAY queue into a boxed-in factory, and `buildWaiting` holds
// the build until a spawn hex frees. Folding `spawnBlocked` into `available` would refuse a
// Build the rules accept, and the row's own declaration in `Ui.h` says so in as many words.
// This clause is what makes that ruling falsifiable rather than a comment: it arranges the
// one board on which the two could be confused and requires them to stay apart.
//
// THE ARRANGEMENT IS THE BOARD, NOT A ROW. Three placements move so that side 0's factory hex
// and every one of its six in-bounds neighbours carry a unit. Nothing selects for a boxed-in
// factory afterwards -- the board IS boxed in, and the clause asserts that off the projection
// before it does anything else, so a variant that failed to box it in fails here instead of
// quietly measuring an ordinary build.
//
// THREE PROPERTIES, AND THE SECOND IS THE ONE WITH TEETH.
//   1. `spawnBlocked` is TRUE for that factory.
//   2. Every offered row is still `available` -- `spawnBlocked` is NOT folded in -- and the
//      Build is ACCEPTED by the rules module.
//   3. Nothing spawned. `buildWaiting` is true, the unit set is unchanged, and the two facts
//      are read from two places, because §4.7 Stub 8 declares them two fields for the reason
//      that `buildWaiting` alone cannot express a boxed-in factory with nothing queued.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBoxedInFactoryStillAcceptsTheBuildTest,
	"Stratocracy.StratBridge.T-UI-04.ABoxedInFactoryReportsSpawnBlockedAndStillAcceptsTheBuild",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBoxedInFactoryStillAcceptsTheBuildTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSpawnBlockedClauses;

	FString Text;
	FString Error;
	if (!TestTrue(TEXT("the shipped scenario is readable"),
			LoadScenarioTextWithoutHash(Text, Error)))
	{
		AddError(Error);
		return false;
	}

	const bool bEdited =
		MovePlacement(Text, kSide0InfantryAt13, kSide0InfantryOnFactory, Error) &&
		MovePlacement(Text, kSide1ArtilleryAt105, kSide1ArtilleryAt24,   Error) &&
		MovePlacement(Text, kSide1ReconAt103,   kSide1ReconAt13,         Error);
	if (!TestTrue(TEXT("the three boxing placements are applied"), bEdited))
	{
		AddError(Error);
		return false;
	}

	FString VariantPath;
	if (!TestTrue(TEXT("the boxed-in variant is written under Saved/"),
			WriteVariant(Text, TEXT("spawn_blocked_boxed_in.json"), VariantPath, Error)))
	{
		AddError(Error);
		RemoveVariant(VariantPath);
		return false;
	}

	{
		FStratBridge Bridge;
		if (!TestTrue(TEXT("the bridge seeds from the boxed-in variant"),
				SeedBridgeFrom(Bridge, VariantPath, Error)))
		{
			AddError(Error);
			RemoveVariant(VariantPath);
			return false;
		}

		strat::UiSnapshot Before;
		const FStratResult Projected = Bridge.MakeUiSnapshot(Before);
		if (!TestTrue(TEXT("MakeUiSnapshot succeeds on the seeded variant"), Projected.bOk))
		{
			AddError(Projected.Reason);
			RemoveVariant(VariantPath);
			return false;
		}

		strat::UiFactoryView Factory;
		if (!TestTrue(TEXT("the variant's factory is in the projection and side 0 holds it"),
				FindTheFactory(Before, kFirstSide, Factory, Error)))
		{
			AddError(Error);
			RemoveVariant(VariantPath);
			return false;
		}

		// ---- 1. The board really is boxed in ------------------------------------
		// ASSERTED BEFORE ANYTHING ELSE HAPPENS. If the arrangement failed, every assertion
		// below would still pass -- against an ordinary build -- and this clause would be
		// green while measuring nothing it claims to.
		if (!TestTrue(
				*FString::Printf(TEXT("the factory at %s reports spawnBlocked"),
					*Describe(Factory.hex)),
				Factory.spawnBlocked))
		{
			AddError(TEXT("the boxing arrangement did not box the factory in; the clause below "
			              "would measure an ordinary build"));
			RemoveVariant(VariantPath);
			return false;
		}
		TestFalse(
			*FString::Printf(TEXT("and nothing is queued there yet, which is the case "
				"buildWaiting alone cannot express")),
			Factory.buildWaiting);

		// ---- 2. Availability does not fold it in --------------------------------
		std::vector<strat::UiBuildOption> Rows;
		const FStratResult Offered = Bridge.BuildOptions(kFirstSide, Factory.hex, Rows);
		if (!TestTrue(TEXT("BuildOptions answers for the boxed-in factory"), Offered.bOk))
		{
			AddError(Offered.Reason);
			RemoveVariant(VariantPath);
			return false;
		}
		if (!TestTrue(TEXT("the boxed-in factory is still offered rows"), !Rows.empty()))
		{
			RemoveVariant(VariantPath);
			return false;
		}

		for (const strat::UiBuildOption& Row : Rows)
		{
			// Q31 IN ONE LINE. Every row of a boxed-in factory stays available; a
			// `uiBuildOptions` that folded `spawnBlocked` into `available` fails here and
			// nowhere else in this suite.
			TestTrue(*FString::Printf(
					TEXT("row '%s' of a boxed-in factory is still available (reason '%s')"),
					*FromStd(Row.id), *FromStd(Row.reason)),
				Row.available);
		}

		int32 DefIndex = INDEX_NONE;
		if (!TestTrue(TEXT("the module offers a row that is both affordable and available"),
				FirstBuildableRow(Rows, DefIndex)))
		{
			RemoveVariant(VariantPath);
			return false;
		}

		const TSet<int32> IdsBefore = UnitIdsOf(Before);

		const FStratResult Built = Bridge.SubmitBuildAtHex(ToIntPoint(Factory.hex), DefIndex);
		if (!TestTrue(
				*FString::Printf(TEXT("the rules module ACCEPTS a Build into the boxed-in "
					"factory (def %d)"), DefIndex),
				Built.bOk))
		{
			AddError(Built.Reason);
			RemoveVariant(VariantPath);
			return false;
		}

		// ---- 3. Nothing spawned; the build waits --------------------------------
		strat::UiSnapshot After;
		const FStratResult Reprojected = Bridge.MakeUiSnapshot(After);
		if (!TestTrue(TEXT("MakeUiSnapshot succeeds after the build"), Reprojected.bOk))
		{
			AddError(Reprojected.Reason);
			RemoveVariant(VariantPath);
			return false;
		}

		strat::UiFactoryView FactoryAfter;
		if (TestTrue(TEXT("the factory is still in the projection after the build"),
				FindTheFactory(After, kFirstSide, FactoryAfter, Error)))
		{
			TestTrue(TEXT("the accepted build is WAITING at the boxed-in factory"),
				FactoryAfter.buildWaiting);
			TestTrue(TEXT("and the factory is still boxed in"), FactoryAfter.spawnBlocked);
			TestTrue(TEXT("and its build allowance for this turn is spent"),
				FactoryAfter.hasBuiltThisTurn);
		}
		else
		{
			AddError(Error);
		}

		const TSet<int32> IdsAfter = UnitIdsOf(After);
		TestEqual(
			*FString::Printf(TEXT("no unit spawned: %d units before, %d after"),
				IdsBefore.Num(), IdsAfter.Num()),
			IdsAfter.Num(), IdsBefore.Num());
		TestTrue(TEXT("and the unit set is the same set, not merely the same size"),
			IdsAfter.Difference(IdsBefore).Num() == 0 &&
			IdsBefore.Difference(IdsAfter).Num() == 0);

		AddInfo(FString::Printf(
			TEXT("boxed-in build at %s: accepted, waiting=%d, spawnBlocked=%d, units %d -> %d"),
			*Describe(Factory.hex), FactoryAfter.buildWaiting ? 1 : 0,
			FactoryAfter.spawnBlocked ? 1 : 0, IdsBefore.Num(), IdsAfter.Num()));
	}

	RemoveVariant(VariantPath);
	return true;
}

// ---------------------------------------------------------------------------
// A displaced spawn lands on a NEIGHBOUR -- and `spawnBlocked` stays FALSE throughout.
//
// THIS IS THE CLAUSE THAT PINS THE ASYMMETRY, and it is the reason both clauses were written
// rather than only the boxed-in one. The reported symptom that put a boxed-in footer on
// §2.11.5's list was DISPLACEMENT, and on displacement `spawnHexesBlocked` returns FALSE --
// so a widget bound to `bSpawnBlocked` would say nothing at all on the case that produced the
// complaint. That is not a defect in the rules module: `spawnHexesBlocked` mirrors
// `resolveBuilds` exactly, and `resolveBuilds` is occupancy-only by design. It is a gap
// between what §2.11.5 specifies and what the reported symptom needs, and it is filed for a
// ruling rather than built.
//
// WHAT IS ASSERTED, AND WHERE EACH EXPECTATION COMES FROM.
//   - `spawnBlocked` is FALSE both before and after the build. Read from the projection.
//   - The build spawns EXACTLY ONE unit. The id is the one member of the after-set that the
//     before-set does not contain -- a set difference over the module's own ids, never a
//     guess at what the next id will be.
//   - That unit is NOT on the factory hex. Compared with `strat::hexEqual`.
//   - That unit IS on one of the factory's in-bounds neighbours -- asked of
//     `strat::neighbors`, the module's own adjacency over the board's own bounds. No hex
//     arithmetic appears in this file, and that is exactly why the clause is here and not in
//     StratPlay, where `strat::neighbors` is `LNK2019`.
//
// THE ARRANGEMENT IS ONE PLACEMENT. Side 0's Infantry moves onto side 0's own factory hex;
// every other unit stays where the shipped scenario puts it, so free neighbours remain and
// the factory is NOT boxed in. The clause asserts that too, before it builds.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratDisplacedSpawnLandsOnANeighbourTest,
	"Stratocracy.StratBridge.T-UI-05.ADisplacedSpawnLandsOnANeighbourAndSpawnBlockedStaysFalse",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratDisplacedSpawnLandsOnANeighbourTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSpawnBlockedClauses;

	FString Text;
	FString Error;
	if (!TestTrue(TEXT("the shipped scenario is readable"),
			LoadScenarioTextWithoutHash(Text, Error)))
	{
		AddError(Error);
		return false;
	}

	if (!TestTrue(TEXT("the displacement placement is applied"),
			MovePlacement(Text, kSide0InfantryAt13, kSide0InfantryOnFactory, Error)))
	{
		AddError(Error);
		return false;
	}

	FString VariantPath;
	if (!TestTrue(TEXT("the displacement variant is written under Saved/"),
			WriteVariant(Text, TEXT("spawn_blocked_displaced.json"), VariantPath, Error)))
	{
		AddError(Error);
		RemoveVariant(VariantPath);
		return false;
	}

	{
		FStratBridge Bridge;
		if (!TestTrue(TEXT("the bridge seeds from the displacement variant"),
				SeedBridgeFrom(Bridge, VariantPath, Error)))
		{
			AddError(Error);
			RemoveVariant(VariantPath);
			return false;
		}

		strat::UiSnapshot Before;
		const FStratResult Projected = Bridge.MakeUiSnapshot(Before);
		if (!TestTrue(TEXT("MakeUiSnapshot succeeds on the seeded variant"), Projected.bOk))
		{
			AddError(Projected.Reason);
			RemoveVariant(VariantPath);
			return false;
		}

		strat::UiFactoryView Factory;
		if (!TestTrue(TEXT("the variant's factory is in the projection and side 0 holds it"),
				FindTheFactory(Before, kFirstSide, Factory, Error)))
		{
			AddError(Error);
			RemoveVariant(VariantPath);
			return false;
		}

		// ---- The arrangement, asserted -----------------------------------------
		// The factory hex is OCCUPIED -- otherwise the build would land on it and there would
		// be no displacement to observe.
		{
			bool bSomethingOnTheFactoryHex = false;
			for (const strat::UiUnitView& U : Before.units)
			{
				if (strat::hexEqual(U.hex, Factory.hex))
				{
					bSomethingOnTheFactoryHex = true;
					break;
				}
			}
			if (!TestTrue(
					*FString::Printf(TEXT("a unit stands on the factory hex %s, so a build must "
						"be displaced"), *Describe(Factory.hex)),
					bSomethingOnTheFactoryHex))
			{
				RemoveVariant(VariantPath);
				return false;
			}
		}

		// THE HALF OF THE ASYMMETRY THAT SURPRISES. Occupied factory hex, and `spawnBlocked`
		// is FALSE -- because a neighbour is free. A widget bound to this field says nothing
		// here.
		if (!TestFalse(
				*FString::Printf(TEXT("spawnBlocked is FALSE at the occupied factory %s, "
					"because a neighbour is free"), *Describe(Factory.hex)),
				Factory.spawnBlocked))
		{
			AddError(TEXT("the variant boxed the factory in rather than displacing; this clause "
			              "measures the displacement case and cannot run on a blocked one"));
			RemoveVariant(VariantPath);
			return false;
		}

		// ---- Build --------------------------------------------------------------
		std::vector<strat::UiBuildOption> Rows;
		const FStratResult Offered = Bridge.BuildOptions(kFirstSide, Factory.hex, Rows);
		if (!TestTrue(TEXT("BuildOptions answers for the factory"), Offered.bOk))
		{
			AddError(Offered.Reason);
			RemoveVariant(VariantPath);
			return false;
		}

		int32 DefIndex = INDEX_NONE;
		if (!TestTrue(TEXT("the module offers a row that is both affordable and available"),
				FirstBuildableRow(Rows, DefIndex)))
		{
			RemoveVariant(VariantPath);
			return false;
		}

		const TSet<int32> IdsBefore = UnitIdsOf(Before);

		const FStratResult Built = Bridge.SubmitBuildAtHex(ToIntPoint(Factory.hex), DefIndex);
		if (!TestTrue(*FString::Printf(TEXT("the Build is accepted (def %d)"), DefIndex),
				Built.bOk))
		{
			AddError(Built.Reason);
			RemoveVariant(VariantPath);
			return false;
		}

		strat::UiSnapshot After;
		const FStratResult Reprojected = Bridge.MakeUiSnapshot(After);
		if (!TestTrue(TEXT("MakeUiSnapshot succeeds after the build"), Reprojected.bOk))
		{
			AddError(Reprojected.Reason);
			RemoveVariant(VariantPath);
			return false;
		}

		// ---- Exactly one unit appeared, and it is not on the factory hex --------
		const TSet<int32> IdsAfter = UnitIdsOf(After);
		const TSet<int32> Appeared = IdsAfter.Difference(IdsBefore);

		TestEqual(TEXT("no unit disappeared"), IdsBefore.Difference(IdsAfter).Num(), 0);
		if (!TestEqual(
				*FString::Printf(TEXT("exactly one unit appeared (%d before, %d after)"),
					IdsBefore.Num(), IdsAfter.Num()),
				Appeared.Num(), 1))
		{
			RemoveVariant(VariantPath);
			return false;
		}

		const TArray<int32> AppearedIds = Appeared.Array();
		strat::UiUnitView Spawned;
		if (!TestTrue(TEXT("the new unit is in the projection"),
				FindUnit(After, AppearedIds[0], Spawned)))
		{
			RemoveVariant(VariantPath);
			return false;
		}

		TestFalse(
			*FString::Printf(TEXT("the spawn did NOT land on the factory hex %s -- it landed at %s"),
				*Describe(Factory.hex), *Describe(Spawned.hex)),
			strat::hexEqual(Spawned.hex, Factory.hex));

		// THE ADJACENCY IS THE MODULE'S, NOT THIS FILE'S. `strat::neighbors` over the board's
		// own bounds is the same walk `spawnHexesBlocked` and `resolveBuilds` both do; a hex
		// formula written here would be arithmetic the gate was asserting against itself.
		{
			const strat::UiWorld Live = Bridge.MakeUiWorld();
			strat::Hex Adjacent[strat::HEX_DIRECTIONS];
			const int  Count = strat::neighbors(Factory.hex, Live.board.bounds, Adjacent);

			bool bOnANeighbour = false;
			FString Listed;
			for (int Index = 0; Index < Count; ++Index)
			{
				Listed += (Index == 0 ? TEXT("") : TEXT(", "));
				Listed += Describe(Adjacent[Index]);
				if (strat::hexEqual(Spawned.hex, Adjacent[Index]))
				{
					bOnANeighbour = true;
				}
			}

			TestTrue(
				*FString::Printf(TEXT("the spawn landed on an in-bounds NEIGHBOUR of the "
					"factory -- at %s, among {%s}"), *Describe(Spawned.hex), *Listed),
				bOnANeighbour);
		}

		// ---- And the field a widget would bind to never moved -------------------
		strat::UiFactoryView FactoryAfter;
		if (TestTrue(TEXT("the factory is still in the projection after the build"),
				FindTheFactory(After, kFirstSide, FactoryAfter, Error)))
		{
			// THE ASYMMETRY, STATED AS A MEASUREMENT. A unit was displaced onto a neighbour
			// and `spawnBlocked` is still false -- so the §2.11.5 footer this field will drive
			// stays silent through the whole of the reported symptom.
			TestFalse(
				TEXT("spawnBlocked is STILL false after a displaced spawn -- the field a "
					"§2.11.5 footer binds to says nothing about displacement"),
				FactoryAfter.spawnBlocked);
			TestFalse(TEXT("and nothing is waiting: the build spawned rather than queued"),
				FactoryAfter.buildWaiting);
			TestTrue(TEXT("and the factory's build allowance for this turn is spent"),
				FactoryAfter.hasBuiltThisTurn);
		}
		else
		{
			AddError(Error);
		}

		AddInfo(FString::Printf(
			TEXT("displaced build at %s: spawned unit %d at %s, spawnBlocked stayed %d"),
			*Describe(Factory.hex), Spawned.id, *Describe(Spawned.hex),
			FactoryAfter.spawnBlocked ? 1 : 0));
	}

	RemoveVariant(VariantPath);
	return true;
}
