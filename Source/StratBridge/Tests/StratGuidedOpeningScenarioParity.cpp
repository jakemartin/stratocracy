// The gate on `FStratBridge::GuidedOpeningHexes` -- GDD §2.11.6-B / §2.13.1, under T-SCN-07.
//
// ON THE ACCEPTANCE ID, because it was reassigned and the reassignment should be checked.
// This clause was proposed as `T-SCN-02`. `T-SCN-02` is "structural validity: every hex
// reference is in bounds", which is a property of the FILE and is already the scenario
// loader's. The subject here is the two authored fields `guidedOpening.infantry` and
// `guidedOpening.objective`, and the GDD's acceptance ID for those two fields is
// **`T-SCN-07` -- opening-capture naming**; `Source/StratRules/Scenario.good.cpp` refuses
// every guided-opening violation under that id and no other. No ID is minted here.
//
// WHAT THIS CLAUSE IS ACTUALLY FOR, and it is not the axial arithmetic. §2.11.6 forbids one
// implementation by name: "Nothing is measured at runtime and no 'nearest objective'
// heuristic is used -- the lane is authored, machine-validated, and recorded as a number by
// `validate_scenario`". `FStratBridge::GuidedOpeningHexes` is the only door that answer comes
// through, and a future pass that made it search for the nearest neutral Factory would
// produce plausible hexes, a green build, and a guided opening that could disagree with the
// map. This clause is the thing that would go red.
//
// SO THE EXPECTATION IS THE FILE'S OWN BYTES, and that is the whole design of it. The method
// returns AXIAL (`strat::ScenarioGuided` holds `Hex`, converted at parse time -- T-SCN-05);
// the file authors ODD-R `[col, row]`. The clause converts the method's answer BACK through
// `strat::axialToOffset` -- the module's own conversion, never a formula written here --
// reassembles the JSON entry those numbers imply, and requires `Data/ferrum_crossing.json`
// to contain it. Nothing is written down: if the method invented a hex, the reassembled
// entry is not in the file and the clause fails.
//
// WHITESPACE IS STRIPPED FROM BOTH SIDES BEFORE THE COMPARISON, so the clause pins the
// authored NUMBERS and not the file's formatting. Key ORDER still matters and that is
// deliberate -- `side`, `infantry`, `objective` is the order §4.7 Stub 7 gives and the order
// the vendored file uses; a reformat that reordered them would be a `Data/` change, which is
// hash-gated by `GATE-DATA-VENDOR` and is somebody's decision rather than an accident.
//
// AND THE TWO SEATS MUST DIFFER. A method that returned one constant pair would satisfy the
// substring test for whichever seat it happened to match. Both seats are checked, and their
// objectives are required to be different hexes -- which the scenario validator independently
// requires (`T-SCN-07`: "both seats name ... " is a refusal), so this is a module-side fact
// and not an assumption about the shipped map.
//
// THE REFUSAL IS PART OF THE CLAUSE. `StratBridge.h` says the method refuses rather than
// handing back (0,0) for a side the scenario does not name, "because a caller asking WHERE
// the objective is has asked a question with no answer, and (0,0) is a real hex on this
// board." So a third side is asked, the refusal is required, and BOTH out-parameters are
// required to have been zeroed -- a caller that ignored `bOk` must not read a live hex.
//
// THIS FILE MAY SPELL `strat::` because `Source/StratBridge/Tests/` is inside `StratBridge`.
// The same clause in `StratPlay` would be `LNK2019` on `strat::axialToOffset`.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Math/IntPoint.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBridge.h"

#include "Hex.h"

namespace StratGuidedOpeningScenarioParity
{
	static const int32 kFirstSide = 0;

	/** The two seats the shipped scenario names. */
	static const int32 kSeatA = 0;
	static const int32 kSeatB = 1;

	/**
	 * A side no scenario in this project names.
	 *
	 * `SIDE_COUNT` is 2 and `strat::loadScenario` refuses a `guidedOpening` entry whose side
	 * is not 0 or 1, so 2 is unnameable by construction rather than merely absent from this
	 * file -- which is what makes the refusal below a property and not a coincidence.
	 */
	static const int32 kSeatTheFileCannotName = 2;

	static FString ScenarioPath()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"), TEXT("ferrum_crossing.json"));
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
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
		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath(), kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	/** Every space, tab, CR and LF removed. Formatting is not the subject; the numbers are. */
	static FString StripWhitespace(const FString& In)
	{
		FString Out = In;
		Out = Out.Replace(TEXT(" "),  TEXT(""), ESearchCase::CaseSensitive);
		Out = Out.Replace(TEXT("\t"), TEXT(""), ESearchCase::CaseSensitive);
		Out = Out.Replace(TEXT("\r"), TEXT(""), ESearchCase::CaseSensitive);
		Out = Out.Replace(TEXT("\n"), TEXT(""), ESearchCase::CaseSensitive);
		return Out;
	}

	/**
	 * The `guidedOpening` entry that a given answer IMPLIES, in the file's own authored frame.
	 *
	 * THE CONVERSION IS THE MODULE'S. `strat::axialToOffset` is the inverse of the one
	 * `strat::wantHex` applied at parse time; writing the odd-r formula here would be
	 * arithmetic over a rules answer, and this gate would then be asserting against itself.
	 */
	static FString EntryImpliedBy(int32 Side, FIntPoint InfantryAxial, FIntPoint ObjectiveAxial)
	{
		strat::Hex Infantry;
		Infantry.q = InfantryAxial.X;
		Infantry.r = InfantryAxial.Y;

		strat::Hex Objective;
		Objective.q = ObjectiveAxial.X;
		Objective.r = ObjectiveAxial.Y;

		int InfCol = 0, InfRow = 0, ObjCol = 0, ObjRow = 0;
		strat::axialToOffset(Infantry, InfCol, InfRow);
		strat::axialToOffset(Objective, ObjCol, ObjRow);

		// Whitespace-free, to be compared against a whitespace-free file.
		return FString::Printf(
			TEXT("{\"side\":%d,\"infantry\":[%d,%d],\"objective\":[%d,%d]}"),
			Side, InfCol, InfRow, ObjCol, ObjRow);
	}

	static FString Describe(FIntPoint Hex)
	{
		return FString::Printf(TEXT("axial(%d,%d)"), Hex.X, Hex.Y);
	}
}

// ---------------------------------------------------------------------------
// T-SCN-07 -- the guided opening's two hexes are the ones the scenario file authors, for
// both seats, and a seat the file does not name is refused.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeGuidedOpeningMatchesScenarioFileTest,
	"Stratocracy.StratBridge.T-SCN-07.GuidedOpeningHexesMatchesTheScenarioFile",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeGuidedOpeningMatchesScenarioFileTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidedOpeningScenarioParity;

	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FString FileText;
	if (!TestTrue(TEXT("Data/ferrum_crossing.json is readable"),
			FFileHelper::LoadFileToString(FileText, *ScenarioPath())))
	{
		return false;
	}
	const FString Packed = StripWhitespace(FileText);
	if (!TestTrue(TEXT("the file declares a guidedOpening block at all"),
			Packed.Contains(TEXT("\"guidedOpening\":["), ESearchCase::CaseSensitive)))
	{
		return false;
	}

	FIntPoint SeatAInfantry = FIntPoint(-1, -1);
	FIntPoint SeatAObjective = FIntPoint(-1, -1);
	FIntPoint SeatBInfantry = FIntPoint(-1, -1);
	FIntPoint SeatBObjective = FIntPoint(-1, -1);

	// ---- both seats, each against the file's own bytes -----------------------
	const int32 Seats[2]      = { kSeatA, kSeatB };
	FIntPoint*  InfantryOut[2] = { &SeatAInfantry, &SeatBInfantry };
	FIntPoint*  ObjectiveOut[2] = { &SeatAObjective, &SeatBObjective };

	for (int32 i = 0; i < 2; ++i)
	{
		const int32 Side = Seats[i];

		FIntPoint Infantry = FIntPoint::ZeroValue;
		FIntPoint Objective = FIntPoint::ZeroValue;
		const FStratResult Answer = Bridge.GuidedOpeningHexes(Side, Infantry, Objective);

		if (!TestTrue(
				*FString::Printf(TEXT("T-SCN-07: the scenario names a guided opening for side %d"),
					Side),
				Answer.bOk))
		{
			AddError(Answer.Reason);
			return false;
		}

		const FString Implied = EntryImpliedBy(Side, Infantry, Objective);
		TestTrue(
			*FString::Printf(
				TEXT("T-SCN-07: side %d's answer -- infantry %s, objective %s -- is the entry the "
				     "scenario file authors. The bytes those hexes imply are %s; the file must "
				     "contain them, and nothing in this clause wrote a coordinate down."),
				Side, *Describe(Infantry), *Describe(Objective), *Implied),
			Packed.Contains(Implied, ESearchCase::CaseSensitive));

		*InfantryOut[i]  = Infantry;
		*ObjectiveOut[i] = Objective;
	}

	// ---- the two seats are not one constant ----------------------------------
	// `strat::validateScenario` refuses a scenario in which "both seats name" the same
	// objective under T-SCN-07, so this is the module's own requirement read back at the
	// façade rather than an assumption about the shipped map.
	TestFalse(
		*FString::Printf(
			TEXT("T-SCN-07: the two seats have DIFFERENT objectives (%s vs %s), so the method is "
			     "reading the entry for the side it was asked about"),
			*Describe(SeatAObjective), *Describe(SeatBObjective)),
		SeatAObjective == SeatBObjective);
	TestFalse(
		*FString::Printf(
			TEXT("T-SCN-07: and DIFFERENT marked-Infantry deployment hexes (%s vs %s)"),
			*Describe(SeatAInfantry), *Describe(SeatBInfantry)),
		SeatAInfantry == SeatBInfantry);

	// ---- a seat the file cannot name is refused, with both outputs zeroed ----
	FIntPoint StaleInfantry  = FIntPoint(7, 7);
	FIntPoint StaleObjective = FIntPoint(7, 7);
	const FStratResult Refused =
		Bridge.GuidedOpeningHexes(kSeatTheFileCannotName, StaleInfantry, StaleObjective);

	TestFalse(
		TEXT("T-SCN-07: a side the scenario names no guided opening for is REFUSED rather than "
		     "answered with (0,0), which is a real hex on this board (StratBridge.h)"),
		Refused.bOk);
	TestTrue(TEXT("T-SCN-07: and the refusal says which side and which scenario"),
		Refused.Reason.Contains(TEXT("guidedOpening"), ESearchCase::CaseSensitive));
	TestTrue(
		*FString::Printf(
			TEXT("T-SCN-07: the infantry out-parameter is zeroed on the refusal path, so a caller "
			     "that ignored bOk cannot read a live hex; it holds %s"),
			*Describe(StaleInfantry)),
		StaleInfantry == FIntPoint::ZeroValue);
	TestTrue(
		*FString::Printf(
			TEXT("T-SCN-07: and so is the objective out-parameter; it holds %s"),
			*Describe(StaleObjective)),
		StaleObjective == FIntPoint::ZeroValue);

	return true;
}
