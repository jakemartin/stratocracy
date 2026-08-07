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

	const std::vector<strat::UnitDef>& Actual = Bridge.UnitDefs();
	if (!TestEqual(TEXT("the mapped vector has the loader's row count"),
		static_cast<int32>(Actual.size()), static_cast<int32>(Expected.size())))
	{
		return false;
	}

	for (size_t i = 0; i < Expected.size(); ++i)
	{
		const strat::UnitDef& E = Expected[i];
		const strat::UnitDef& A = Actual[i];
		const FString At = FString::Printf(TEXT("defIndex %d"), static_cast<int32>(i));

		// The id at this index is the order claim. Every other field is checked too,
		// because a table whose rows are in the right order but carry wrong numbers
		// diverges just as silently.
		TestEqual(*FString::Printf(TEXT("%s: id"), *At), FromStd(A.id), FromStd(E.id));
		TestEqual(*FString::Printf(TEXT("%s: hpMax"), *At), A.hpMax, E.hpMax);
		TestEqual(*FString::Printf(TEXT("%s: move"), *At), A.move, E.move);
		TestEqual(*FString::Printf(TEXT("%s: atk"), *At), A.atk, E.atk);
		TestEqual(*FString::Printf(TEXT("%s: def"), *At), A.def, E.def);
		TestEqual(*FString::Printf(TEXT("%s: rangeMin"), *At), A.rangeMin, E.rangeMin);
		TestEqual(*FString::Printf(TEXT("%s: rangeMax"), *At), A.rangeMax, E.rangeMax);
		TestEqual(*FString::Printf(TEXT("%s: costFame"), *At), A.costFame, E.costFame);
		TestEqual(*FString::Printf(TEXT("%s: type"), *At),
			static_cast<int32>(A.type), static_cast<int32>(E.type));
		TestEqual(*FString::Printf(TEXT("%s: canCapture"), *At), A.canCapture, E.canCapture);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-INT-02 -- replay parity, and with it T-SAVE-06's in-engine half.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeReplayParityTest,
	"Stratocracy.StratBridge.T-INT-02.ReplayParityWithHeadless",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeReplayParityTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeParity;

	FStratBridge Bridge;
	FString Error;
	// SeedBridge runs on its own line and not inside the TestTrue call: it WRITES
	// Error, and a call that both fills the string and formats it has unspecified
	// argument evaluation order, so the message could report the empty string.
	const bool bSeeded = SeedBridge(Bridge, Error);
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), bSeeded))
	{
		AddError(Error);
		return false;
	}

	strat::Save Fixture;
	const bool bFixtureLoaded = LoadFixture(Fixture, Error);   // fills Error; see above
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"), bFixtureLoaded))
	{
		AddError(Error);
		return false;
	}

	// The fixture describes the game this scenario produces, not some other one. If
	// this differs, the hash comparison below would be comparing two different games
	// and its verdict would mean nothing -- so it is checked BEFORE the hash rather
	// than left to be inferred from a failure.
	TestEqual(TEXT("the fixture names the scenario the bridge seeded from"),
		FromStd(Fixture.scenarioId), FString(TEXT("ferrum_crossing")));

	AddInfo(FString::Printf(TEXT("fixture: %d commands, result %s"),
		static_cast<int32>(Fixture.commandLog.size()),
		Fixture.hasResult ? *FromStd(Fixture.result) : TEXT("null")));

	const FStratResult Replayed = Bridge.ReplayLog(ToArray(Fixture.commandLog));
	if (!TestTrue(TEXT("the fixture's command log replays clean in-engine"), Replayed.bOk))
	{
		AddError(FString::Printf(TEXT("%s: %s"), *Replayed.Id, *Replayed.Reason));
		return false;
	}

	// THE COMPARISON. Left side computed here, right side carried from the crew
	// build. Nothing in this file authored either number.
	TestEqual(
		TEXT("T-INT-02/T-SAVE-06: the in-engine canonical state hash equals the headless one"),
		Bridge.StateHash(), FromStd(Fixture.stateHash));

	return true;
}

// ---------------------------------------------------------------------------
// T-INT-03 -- rejection safety: "an illegal command leaves the state hash unchanged
// and returns a reason; no partial application".
//
// Three clauses, and the third is the one with teeth. A single illegal command
// changing nothing is satisfied by an implementation that validates everything up
// front; "no partial application" is about a log whose FIRST commands are legal and
// whose last is not, which is where an apply-then-validate implementation leaks.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeRejectionSafetyTest,
	"Stratocracy.StratBridge.T-INT-03.RejectionSafety",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeRejectionSafetyTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeParity;

	FStratBridge Bridge;
	FString Error;
	const bool bSeeded = SeedBridge(Bridge, Error);   // fills Error; see T-INT-02 above
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), bSeeded))
	{
		AddError(Error);
		return false;
	}

	const FString SeededHash = Bridge.StateHash();
	TestTrue(TEXT("the seeded state has a hash to compare against"), !SeededHash.IsEmpty());

	// An acting unit that does not exist. Chosen because it cannot become legal under
	// any rule or table change -- a hex or a range would have to be re-picked every
	// time the scenario moved, and a check that needs re-picking is a check that will
	// one day be quietly relaxed instead.
	strat::SaveCommand Illegal;
	Illegal.turn = Bridge.State().turn.turnNumber;
	Illegal.side = Bridge.State().turn.activeSide;
	Illegal.kind = strat::SaveCommandKind::Move;
	Illegal.unitId = 999999;
	Illegal.hasUnit = true;
	Illegal.hex = strat::Hex{ 0, 0 };
	Illegal.hasHex = true;

	// ---- clause 1 + 2: refused, WITH A REASON ------------------------------
	const FStratResult Refused = Bridge.Submit(Illegal);
	TestFalse(TEXT("T-INT-03: an illegal command is refused"), Refused.bOk);
	TestTrue(TEXT("T-INT-03: the refusal carries a reason"), !Refused.Reason.IsEmpty());
	AddInfo(FString::Printf(TEXT("refusal reason: %s%s"),
		Refused.Id.IsEmpty() ? TEXT("") : *FString::Printf(TEXT("[%s] "), *Refused.Id),
		*Refused.Reason));

	// ---- clause 3: the state hash is unchanged -----------------------------
	TestEqual(TEXT("T-INT-03: the refused command left the state hash unchanged"),
		Bridge.StateHash(), SeededHash);

	// ---- clause 3, the form that has teeth: no partial application ----------
	// A log of legal commands with one illegal command appended. An implementation
	// that applied as it went would leave the legal prefix applied and this hash
	// would differ; the whole log must be refused as one.
	strat::Save Fixture;
	const bool bFixtureLoaded = LoadFixture(Fixture, Error);   // fills Error; see above
	if (TestTrue(TEXT("the committed parity fixture loads, for the prefix"), bFixtureLoaded))
	{
		TArray<strat::SaveCommand> Poisoned = ToArray(Fixture.commandLog);
		const int32 LegalPrefix = Poisoned.Num();
		TestTrue(TEXT("the fixture supplies a non-empty legal prefix"), LegalPrefix > 0);
		Poisoned.Add(Illegal);

		const FStratResult Rejected = Bridge.ReplayLog(Poisoned);
		TestFalse(TEXT("T-INT-03: a log ending in an illegal command is refused"),
			Rejected.bOk);
		TestTrue(TEXT("T-INT-03: that refusal carries a reason too"),
			!Rejected.Reason.IsEmpty());
		TestEqual(
			FString::Printf(TEXT("T-INT-03: no partial application -- %d legal commands left nothing behind"),
				LegalPrefix),
			Bridge.StateHash(), SeededHash);
	}
	else
	{
		AddError(Error);
	}

	return true;
}
