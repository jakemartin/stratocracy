// The rules-side gate on §2.9's difficulty handicap -- `FStratBridge::ApplyStartingFameHandicap`
// and its two readback methods. GDD §2.9, §2.7, §2.13, under T-FAME-02 and T-FAME-01.
//
// WHY THESE CLAUSES RIDE EXISTING IDS AND MINT NOTHING. §2.9 has no acceptance ID of its own;
// the user ruling recorded 2026-08-28 in `Tools/architect/state/global.md`'s `## NEXT` splits
// W7's clauses across three EXISTING upstream ids by SUBJECT. This file carries two of the
// three:
//
//   - `T-FAME-02` -- the arithmetic. Its own stub text closes "so the gate asserts each side's
//     CONFIGURED VALUE and never a literal 200", which is an instruction addressed to a gate,
//     and the first clause below is that gate. `T-FAME-02`'s HEADLINE subject is income
//     accrual (factory +100/turn, town +25/turn); no clause here asserts any accrual, and none
//     is written as though it did.
//   - `T-FAME-01` -- the separate-counter rule: combat awards accrue `fameCombat` as well, and
//     passive income never touches it. The second clause asserts that a NEW non-combat mutation
//     never touches it either. It asserts nothing about kill awards; that half of the ID is
//     already `StratCombatOutcomeParity.cpp`'s.
//
// `T-AI-07` IS NOT AN ID AND IS NOT REACHED FOR HERE. It appears in the GDD and §4.11's row-6
// note records that `spec/ai_spec.md` declines to mint it; the self-play run rides
// `GATE-AI-SMOKE`. A proposed identifier greps exactly like a defined one.
//
// WHERE EVERY EXPECTATION COMES FROM, which is the only question that matters.
//   - THE OPENING FAME IS `FStratBridge::ScenarioData().startingFame[Side]` -- the scenario
//     struct `strat::parseScenario` filled from `Data/ferrum_crossing.json`. NO CLAUSE BELOW
//     WRITES 200, 350 OR 100. A clause that hardcoded the shipped scenario's value would pass
//     today and would be wrong in the exact way the ID it rides forbids, and it would go on
//     passing after somebody re-authored the scenario.
//   - EVERY "AFTER" VALUE IS `FStratBridge::SideFameTotal` / `SideFameCombat`, read from the
//     live `strat::EconomyState`. The deltas handed IN are this file's inputs, not its
//     expectations.
//   - THE DELTAS ARE DELIBERATELY NOT §2.9's THREE NUMBERS. +150 / 0 / -100 are `StratPlay`'s,
//     they are unreachable from this module without 1 x LNK2019 per call, and pinning them here
//     would make this file a second author of the tier table. `StratDifficultyHandicapMatchClauses.cpp`
//     pins the mapping where it lives. What is pinned here is that the method adds WHATEVER it
//     is handed to WHATEVER the scenario configured.
//   - THE OUT-OF-RANGE SIDE IS `strat::SIDE_COUNT`, the module's own count, on
//     `StratAiBridgeParity.cpp`'s precedent. A literal 2 here would be a second author of it.
//   - THE `fameCombat` FIXTURE POSITION IS `Data/parity_fixture.save`, replayed through the
//     typed submit methods until the module itself reports a non-zero counter. The prefix is
//     DISCOVERED, never authored, and the clause fails loudly if no prefix reaches one --
//     see the second clause's own block on why a zero counter cannot pin non-movement.
//
// ON COMPARING A REFUSAL'S PROSE. `FString::operator==`, `Contains` and `TestEqual` are ALL
// case-insensitive in this engine, and this project has already shipped a clause that could not
// fail for that reason. Every string comparison below goes through `ContainsExact` /
// `EqualsExact`. The refusal clause pins what the two refusals DISTINGUISH -- they must differ
// from each other, and the out-of-range one must name the side it was handed -- rather than
// quoting a sentence that a rewording would break without changing any behaviour.
//
// THIS FILE MAY SPELL `strat::`, AND THAT IS WHY IT IS HERE. `Source/StratBridge/Tests/` is
// inside `StratBridge`, so `strat::parseSave` and `strat::SIDE_COUNT` link. The same clause
// under `Source/StratPlay/Tests/` would be LNK2019 -- measured 8 x when the bridge first tried.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBridge.h"

#include "Economy.h"
#include "Save.h"
#include "Scenario.h"

namespace StratDifficultyHandicapClauses
{
	/** The harness pin every parity file in this tree carries. The committed fixture's log was
	 *  emitted against side 0; a different value replays a different game. */
	static const int32 kFirstSide = 0;

	/**
	 * Two deltas that are NOT §2.9's tier numbers, and one that is 0 for every tier table.
	 *
	 * See the header block: the tier -> delta mapping belongs to `StratPlay` and is pinned
	 * there. These are inputs chosen so that a clause below cannot accidentally pass because
	 * the method hardcoded a tier's number, and so that the positive and negative arms are
	 * different magnitudes -- a `FMath::Abs` slipped into the arithmetic would survive a
	 * symmetric pair.
	 */
	static const int32 kUpDelta   = 37;
	static const int32 kDownDelta = -23;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static FString ScenarioPath()
	{
		return FPaths::Combine(DataDir(), TEXT("ferrum_crossing.json"));
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

	/** Case-SENSITIVE containment. See the header block: the default is not. */
	static bool ContainsExact(const FString& Haystack, const FString& Needle)
	{
		return Haystack.Contains(Needle, ESearchCase::CaseSensitive);
	}

	/** Case-SENSITIVE equality. Same reason. */
	static bool EqualsExact(const FString& A, const FString& B)
	{
		return A.Equals(B, ESearchCase::CaseSensitive);
	}

	/** Definitions plus the shipped scenario, through the two engine-typed loaders.
	 *  See `StratBridgeRestoreParity.cpp` for the same helper and the reason it is duplicated. */
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

	/** See `StratBridgeRestoreParity.cpp`: the format spells a §2.4 row index in a field
	 *  named for a unit, and `StratBridge.h` records that the spelling is the trap. */
	static FStratResult SubmitTyped(FStratBridge& Bridge, const strat::SaveCommand& C)
	{
		switch (C.kind)
		{
		case strat::SaveCommandKind::Move:    return Bridge.SubmitMove(C.unitId, C.hex);
		case strat::SaveCommandKind::Attack:  return Bridge.SubmitAttack(C.unitId, C.hex);
		case strat::SaveCommandKind::Build:   return Bridge.SubmitBuild(C.hex, C.unitId);
		case strat::SaveCommandKind::Capture: return Bridge.SubmitCapture(C.unitId);
		case strat::SaveCommandKind::EndTurn: return Bridge.SubmitEndTurn();
		}
		return FStratResult::Fail(TEXT("a SaveCommandKind with no typed method"));
	}
}

// ---------------------------------------------------------------------------
// T-FAME-02 -- the handicap moves ONE side's fame by the delta, on top of the value the
// SCENARIO configured, and moves nothing else.
//
// THE OPENING IS ASSERTED AGAINST `ScenarioData().startingFame` FIRST, AND THAT HALF IS THE
// ID's OWN INSTRUCTION. `T-FAME-02`'s stub closes "so the gate asserts each side's configured
// value and never a literal 200". Without that line the delta arithmetic below would be true of
// a bridge that seeded every side to zero, and this file would have pinned the addition while
// leaving the thing added to unpinned.
//
// THE ZERO ARM IS NOT DECORATION. `StratBridge.h` says zero is a no-op BY ARITHMETIC and not by
// an early return, because Normal tier passes it. The clause therefore requires
// `strat::canonicalStateHash` -- which carries `fT` for both sides, `Replay.good.cpp`'s
// `canonicalStateBytes` -- to be UNCHANGED across the zero call and CHANGED across a non-zero
// one. The pair is what stops the whole clause from being satisfied by a method that returns Ok
// and does nothing: the non-zero arm proves the hash is a live witness, the zero arm then means
// something.
//
// AND THE RECORDED LOG IS PINNED UNCHANGED THROUGHOUT. The header states the handicap is NOT a
// §4.10 command and could not be one -- that is precisely why a cross-tier load lands on
// T-SAVE-06. A handicap that quietly appended a command would move every recorded length in the
// project and would make that refusal unreachable.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratHandicapMovesConfiguredFameByTheDeltaTest,
	"Stratocracy.StratBridge.T-FAME-02.HandicapMovesTheScenariosConfiguredFameByTheDelta",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratHandicapMovesConfiguredFameByTheDeltaTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDifficultyHandicapClauses;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("a bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- the configured value, and never a literal --------------------------
	const strat::Scenario& Configured = Bridge.ScenarioData();
	TArray<int32> Opening;
	for (int32 Side = 0; Side < strat::SIDE_COUNT; ++Side)
	{
		const int32 FromScenario = Configured.startingFame[Side];
		TestEqual(
			*FString::Printf(
				TEXT("T-FAME-02: side %d's turn-1 buying power is the scenario's OWN configured ")
				TEXT("startingFame -- read from strat::Scenario, never written down here"),
				Side),
			Bridge.SideFameTotal(Side), FromScenario);
		Opening.Add(FromScenario);
	}

	// The clause needs a scenario that actually configured something to add to; a scenario
	// opening on 0 would make the clamp arm and this arm indistinguishable.
	if (!TestTrue(TEXT("the shipped scenario opens both sides above zero, so a delta composes "
	                   "with something"),
			Opening[0] > 0 && Opening[1] > 0))
	{
		return false;
	}

	const FString HashAtSeed  = Bridge.StateHash();
	const int32   CountAtSeed = Bridge.RecordedCommandCount();

	// ---- a positive delta on side 0 -----------------------------------------
	{
		int32 After = INDEX_NONE;
		const FStratResult Applied = Bridge.ApplyStartingFameHandicap(0, kUpDelta, After);

		TestTrue(*FString::Printf(TEXT("T-FAME-02: a %+d handicap on side 0 is accepted; got '%s'"),
			kUpDelta, *Applied.Reason), Applied.bOk);
		TestEqual(TEXT("T-FAME-02: the reported total is the CONFIGURED value plus the delta"),
			After, Opening[0] + kUpDelta);
		TestEqual(TEXT("T-FAME-02: and the live purse agrees with what was reported"),
			Bridge.SideFameTotal(0), Opening[0] + kUpDelta);
		TestEqual(
			TEXT("T-FAME-02: the OTHER side is untouched -- §2.9 moves the player's side only"),
			Bridge.SideFameTotal(1), Opening[1]);
		TestNotEqual(
			TEXT("T-FAME-02: the canonical state hash MOVED, so the hash is a live witness for "
			     "the zero arm below (canonicalStateBytes carries fT per side)"),
			Bridge.StateHash(), HashAtSeed);
		TestEqual(
			TEXT("T-FAME-02: and NOTHING was recorded -- the handicap is not a §4.10 command, "
			     "which is why a cross-tier load has to land on T-SAVE-06"),
			Bridge.RecordedCommandCount(), CountAtSeed);
	}

	// ---- a negative delta on side 1, of a different magnitude ---------------
	{
		const FString HashBefore = Bridge.StateHash();

		int32 After = INDEX_NONE;
		const FStratResult Applied = Bridge.ApplyStartingFameHandicap(1, kDownDelta, After);

		TestTrue(*FString::Printf(TEXT("T-FAME-02: a %+d handicap on side 1 is accepted; got '%s'"),
			kDownDelta, *Applied.Reason), Applied.bOk);
		TestEqual(TEXT("T-FAME-02: a negative delta SUBTRACTS -- it is not an absolute and not "
		               "a magnitude"),
			After, Opening[1] + kDownDelta);
		TestEqual(TEXT("T-FAME-02: and the live purse agrees"),
			Bridge.SideFameTotal(1), Opening[1] + kDownDelta);
		TestEqual(TEXT("T-FAME-02: side 0 keeps the total the previous call gave it"),
			Bridge.SideFameTotal(0), Opening[0] + kUpDelta);
		TestNotEqual(TEXT("T-FAME-02: the state hash moved again"),
			Bridge.StateHash(), HashBefore);
	}

	// ---- zero: Ok, and inert BY ARITHMETIC ----------------------------------
	{
		const FString HashBefore   = Bridge.StateHash();
		const int32   Side0Before  = Bridge.SideFameTotal(0);
		const int32   Side1Before  = Bridge.SideFameTotal(1);

		int32 After = INDEX_NONE;
		const FStratResult Applied = Bridge.ApplyStartingFameHandicap(0, 0, After);

		TestTrue(*FString::Printf(
			TEXT("T-FAME-02: a zero delta is a LEGAL delta and is accepted (Normal tier passes "
			     "it and the caller is not asked to know that Normal is special); got '%s'"),
			*Applied.Reason), Applied.bOk);
		TestEqual(TEXT("T-FAME-02: it reports the unchanged total, not a sentinel"),
			After, Side0Before);
		TestEqual(TEXT("T-FAME-02: side 0 is where it was"), Bridge.SideFameTotal(0), Side0Before);
		TestEqual(TEXT("T-FAME-02: side 1 is where it was"), Bridge.SideFameTotal(1), Side1Before);
		TestEqual(
			TEXT("T-FAME-02: and the canonical state hash is unmoved -- a zero handicap is not "
			     "merely reported as inert, it IS inert"),
			Bridge.StateHash(), HashBefore);
		TestEqual(TEXT("T-FAME-02: still nothing recorded after three calls"),
			Bridge.RecordedCommandCount(), CountAtSeed);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-FAME-01 -- the handicap does not touch `fameCombat`, ON A STATE WHERE `fameCombat` IS NOT
// ZERO.
//
// THIS IS THE CORRECTNESS CLAUSE OF THE WAVE AND THE NON-ZERO SEED IS THE WHOLE OF IT.
// `fameCombat` is §2.8's tiebreak criterion-1 sort key and `T-TURN-05`'s mutual-passivity guard
// reads it (`Turn.good.cpp`: `if (b.side[i].fameCombat != 0) anyoneFought = true`). At the
// seeded opening both counters are 0, and 0 is also what an uninitialised read, a wrong-member
// read and a zeroing bug all look like -- a value that was already 0 cannot tell you the
// handicap left it alone. So the clause replays `Data/parity_fixture.save` through the typed
// submit methods until the MODULE reports a non-zero counter, and only then applies the
// handicap.
//
// THE PREFIX IS DISCOVERED AND NEVER AUTHORED. `StratCombatOutcomeParity.cpp` records that the
// fixture's third kill sits at index 33, and this clause does not depend on that: it walks the
// log and stops at the first prefix that moves either counter. A re-emitted fixture moves the
// index and not the clause. If NO prefix reaches a non-zero counter the clause FAILS with that
// as its message rather than quietly asserting 0 == 0.
//
// AND `fameTotal` IS REQUIRED TO MOVE IN THE SAME CALL. Three of the assertions here are
// assertions that nothing happened, and every one of them would pass on an
// `ApplyStartingFameHandicap` that had been deleted. The control is that the same call moved
// the counter it is supposed to move, by the delta it was handed.
//
// THE ID IS T-FAME-01 AND THE STRETCH IS STATED. `T-FAME-01` is the separate-counter rule --
// combat awards accrue `fameCombat` too, and passive income never touches it. This clause
// asserts that a THIRD writer of `fameTotal`, which is neither a combat award nor income, also
// leaves it alone. It asserts nothing about kill awards; `StratCombatOutcomeParity.cpp`'s
// `T-FAME-01.AttackerFameMovesOnAKill` is that half.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratHandicapLeavesFameCombatUnmovedTest,
	"Stratocracy.StratBridge.T-FAME-01.HandicapLeavesANonZeroFameCombatUnmoved",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratHandicapLeavesFameCombatUnmovedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDifficultyHandicapClauses;

	strat::Save Fixture;
	FString     Error;
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"),
			LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratBridge Bridge;
	if (!TestTrue(TEXT("a bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	// The opening reading, which is the reading this clause exists BECAUSE it cannot use.
	TestEqual(TEXT("the seeded opening has fameCombat 0 on side 0 -- which is why the clause "
	               "does not stop here"), Bridge.SideFameCombat(0), 0);
	TestEqual(TEXT("and 0 on side 1"), Bridge.SideFameCombat(1), 0);

	// ---- walk the fixture until the MODULE reports a kill award -------------
	int32 Prefix = 0;
	for (size_t I = 0; I < Fixture.commandLog.size(); ++I)
	{
		const FStratResult Submitted = SubmitTyped(Bridge, Fixture.commandLog[I]);
		if (!Submitted.bOk)
		{
			// The fixture is a recorded game and every command should apply. If one does not,
			// say which and stop -- a silent break would leave the clause asserting whatever
			// state the refusal left behind.
			AddError(FString::Printf(TEXT("fixture command %d refused: %s"),
				static_cast<int32>(I), *Submitted.Reason));
			break;
		}
		Prefix = static_cast<int32>(I) + 1;
		if (Bridge.SideFameCombat(0) != 0 || Bridge.SideFameCombat(1) != 0)
		{
			break;
		}
	}

	const int32 CombatBefore0 = Bridge.SideFameCombat(0);
	const int32 CombatBefore1 = Bridge.SideFameCombat(1);

	if (!TestTrue(
			*FString::Printf(
				TEXT("T-FAME-01: the fixture reached a NON-ZERO fameCombat within %d command(s) ")
				TEXT("(side 0: %d, side 1: %d) -- without one, 'unmoved' would be 0 == 0 and the ")
				TEXT("clause could not fail"),
				Prefix, CombatBefore0, CombatBefore1),
			CombatBefore0 != 0 || CombatBefore1 != 0))
	{
		return false;
	}

	// ---- the handicap, applied to each side in turn -------------------------
	for (int32 Side = 0; Side < strat::SIDE_COUNT; ++Side)
	{
		const int32 TotalBefore = Bridge.SideFameTotal(Side);

		int32 After = INDEX_NONE;
		const FStratResult Applied = Bridge.ApplyStartingFameHandicap(Side, kUpDelta, After);

		if (!TestTrue(*FString::Printf(
				TEXT("the handicap applies to side %d mid-match; got '%s'"), Side, *Applied.Reason),
				Applied.bOk))
		{
			return false;
		}

		// THE CONTROL. Without this the three assertions below would all hold of a method
		// that did nothing at all.
		TestEqual(
			*FString::Printf(
				TEXT("CONTROL: the same call DID move side %d's fameTotal by the delta"), Side),
			After, TotalBefore + kUpDelta);

		TestEqual(
			*FString::Printf(
				TEXT("T-FAME-01: side 0's fameCombat is unmoved across the handicap on side %d ")
				TEXT("-- §2.8's tiebreak key and T-TURN-05's passivity guard both read it"), Side),
			Bridge.SideFameCombat(0), CombatBefore0);
		TestEqual(
			*FString::Printf(
				TEXT("T-FAME-01: side 1's fameCombat is unmoved across the handicap on side %d"),
				Side),
			Bridge.SideFameCombat(1), CombatBefore1);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-FAME-02 -- a delta below zero clamps AT zero and REPORTS zero, which is a real purse and
// not the sentinel.
//
// `StratBridge.h` states the clamp is a decision and not an oversight: a negative purse is a
// state `strat::initSide` cannot produce and no rule reads as debt, and it is REPORTED through
// `OutFameTotalAfter` rather than refused, because a designer configuring Hard against a
// 50-Fame scenario has authored a mismatch and not an unplayable match.
//
// SO THE TWO HALVES ARE `bOk == true` AND `OutFameTotalAfter == 0`. `INDEX_NONE` is what the
// refusal path writes; if the clamp path ever reported it instead, a caller could not tell a
// clamped purse from a rejected call, and the "reported rather than refused" sentence above
// would be false while every arithmetic clause stayed green.
//
// THE OVERSHOOT IS DERIVED FROM THE LIVE PURSE -- `-(SideFameTotal + 1)` -- so the clause
// clamps by exactly one Fame no matter what the scenario configured, and the assertion that the
// purse was above zero to begin with is made against the module rather than assumed.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratHandicapClampsAtZeroTest,
	"Stratocracy.StratBridge.T-FAME-02.HandicapClampsAtZeroAndReportsZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratHandicapClampsAtZeroTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDifficultyHandicapClauses;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("a bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	const int32 Purse = Bridge.SideFameTotal(0);
	if (!TestTrue(TEXT("the seeded purse is above zero, so a clamp is reachable at all"),
			Purse > 0))
	{
		return false;
	}

	// One Fame past the floor. Derived from the live purse: nothing here knows what the
	// scenario configured and nothing here needs to.
	const int32 Overshoot = -(Purse + 1);

	int32 After = INDEX_NONE;
	const FStratResult Applied = Bridge.ApplyStartingFameHandicap(0, Overshoot, After);

	TestTrue(*FString::Printf(
		TEXT("T-FAME-02: an overshooting delta is ACCEPTED and not refused -- the clamp is ")
		TEXT("reported, per StratBridge.h; got '%s'"), *Applied.Reason), Applied.bOk);
	TestEqual(TEXT("T-FAME-02: the purse clamps at zero rather than going negative"),
		Bridge.SideFameTotal(0), 0);
	TestEqual(
		TEXT("T-FAME-02: and the report is 0 -- a real purse -- and NOT INDEX_NONE, which is "
		     "what the refusal path writes and would make a clamp indistinguishable from a "
		     "rejection"),
		After, 0);
	TestEqual(TEXT("T-FAME-02: the other side is untouched by a clamped call too"),
		Bridge.SideFameTotal(1), Bridge.ScenarioData().startingFame[1]);

	// The floor is a floor and not a one-shot: a second overshoot stays at zero and still
	// reports it.
	int32 Again = INDEX_NONE;
	const FStratResult Twice = Bridge.ApplyStartingFameHandicap(0, Overshoot, Again);
	TestTrue(TEXT("T-FAME-02: a second overshoot is accepted"), Twice.bOk);
	TestEqual(TEXT("T-FAME-02: and still reports zero"), Again, 0);

	return true;
}

// ---------------------------------------------------------------------------
// T-FAME-02 -- the two refusals, and the sentinel they leave in the out-parameter.
//
// WHAT IS ACTUALLY AT RISK. `StratBridge.h` says `OutFameTotalAfter` is set BEFORE either
// guard "so a refused call leaves the caller holding a sentinel rather than a previous call's
// answer". The failure that sentence prevents is silent: a caller that logged the out-parameter
// after a refused call would print the last successful total and nobody would know. So the
// out-parameter is PRE-LOADED here with a value neither the method nor `INDEX_NONE` would
// produce, and the clause requires it to have been overwritten.
//
// AND THE STATE IS PINNED UNCHANGED ACROSS BOTH REFUSALS. A refusal that had already moved the
// purse before deciding to refuse would pass a clause that only read the return value.
//
// ON THE PROSE. The two reasons are compared for DISTINCTNESS -- an unseeded bridge and a bad
// side have different fixes and a caller must be able to tell them apart -- and the
// out-of-range one is required to name the side it was handed, a number this clause supplied.
// Neither assertion quotes a sentence, because a reworded message with identical behaviour is
// not a regression. Both comparisons are case-SENSITIVE; see the header block.
//
// THE READBACKS ARE PINNED HERE TOO, and for the reason `StratBridge.h` gives: `SideFameTotal`
// and `SideFameCombat` return `INDEX_NONE` and never 0 for a side that does not exist, because
// zero is a real purse and a sentinel that collides with a real value cannot report absence.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratHandicapRefusalsTest,
	"Stratocracy.StratBridge.T-FAME-02.HandicapRefusesAnUnseededBridgeAndAnOutOfRangeSide",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratHandicapRefusalsTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDifficultyHandicapClauses;

	// A value the method can never legitimately produce: it is not INDEX_NONE and it is not a
	// purse this scenario can reach from any delta this clause hands over.
	const int32 kPreloaded = -987654;

	// ---- an unseeded bridge -------------------------------------------------
	FString UnseededReason;
	{
		FStratBridge Unseeded;
		TestFalse(TEXT("the control: this bridge really is unseeded"), Unseeded.IsSeeded());

		int32 After = kPreloaded;
		const FStratResult Refused = Unseeded.ApplyStartingFameHandicap(0, kUpDelta, After);

		TestFalse(TEXT("T-FAME-02: an unseeded bridge has no purse to move, and says so"),
			Refused.bOk);
		TestEqual(
			TEXT("T-FAME-02: the out-parameter was OVERWRITTEN with INDEX_NONE, so a refused "
			     "call cannot leave a caller holding a stale answer"),
			After, static_cast<int32>(INDEX_NONE));
		TestTrue(TEXT("and the refusal carries a reason"), !Refused.Reason.IsEmpty());
		UnseededReason = Refused.Reason;

		TestEqual(TEXT("T-FAME-02: SideFameTotal on an unseeded bridge is INDEX_NONE, never 0 -- "
		               "zero is a real purse"),
			Unseeded.SideFameTotal(0), static_cast<int32>(INDEX_NONE));
		TestEqual(TEXT("T-FAME-02: and SideFameCombat likewise"),
			Unseeded.SideFameCombat(0), static_cast<int32>(INDEX_NONE));
	}

	// ---- a seeded bridge and a side that does not exist ---------------------
	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("a bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	const FString HashBefore  = Bridge.StateHash();
	const int32   Side0Before = Bridge.SideFameTotal(0);
	const int32   Side1Before = Bridge.SideFameTotal(1);

	// `strat::SIDE_COUNT` is the module's own count; a literal 2 here would be a second author
	// of it. `StratAiBridgeParity.cpp` takes the out-of-range side the same way.
	const int32 NoSuchSide = strat::SIDE_COUNT;

	int32 AfterHigh = kPreloaded;
	const FStratResult High = Bridge.ApplyStartingFameHandicap(NoSuchSide, kUpDelta, AfterHigh);

	TestFalse(TEXT("T-FAME-02: a side at the count is out of range and is refused"), High.bOk);
	TestEqual(TEXT("T-FAME-02: and the out-parameter is the sentinel"),
		AfterHigh, static_cast<int32>(INDEX_NONE));
	TestTrue(
		*FString::Printf(
			TEXT("T-FAME-02: the refusal NAMES the side it was handed (%d), so a caller need "
			     "not guess which argument was wrong; got '%s'"),
			NoSuchSide, *High.Reason),
		ContainsExact(High.Reason, FString::FromInt(NoSuchSide)));

	int32 AfterLow = kPreloaded;
	const FStratResult Low = Bridge.ApplyStartingFameHandicap(-1, kUpDelta, AfterLow);
	TestFalse(TEXT("T-FAME-02: a negative side is refused too"), Low.bOk);
	TestEqual(TEXT("T-FAME-02: with the same sentinel"),
		AfterLow, static_cast<int32>(INDEX_NONE));

	// ---- the two refusals must be distinguishable ---------------------------
	TestFalse(
		TEXT("T-FAME-02: 'no scenario is loaded' and 'that side does not exist' are DIFFERENT "
		     "refusals with different fixes, and a caller can tell them apart (case-sensitive)"),
		EqualsExact(UnseededReason, High.Reason));

	// ---- nothing moved ------------------------------------------------------
	TestEqual(TEXT("T-FAME-02: a refused handicap leaves the canonical state exactly as it was"),
		Bridge.StateHash(), HashBefore);
	TestEqual(TEXT("T-FAME-02: side 0's purse is untouched"),
		Bridge.SideFameTotal(0), Side0Before);
	TestEqual(TEXT("T-FAME-02: side 1's purse is untouched"),
		Bridge.SideFameTotal(1), Side1Before);
	TestEqual(TEXT("T-FAME-02: SideFameTotal for a side that does not exist is INDEX_NONE"),
		Bridge.SideFameTotal(NoSuchSide), static_cast<int32>(INDEX_NONE));
	TestEqual(TEXT("T-FAME-02: SideFameCombat likewise"),
		Bridge.SideFameCombat(NoSuchSide), static_cast<int32>(INDEX_NONE));

	return true;
}
