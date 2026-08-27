// Wave 3 -- §2.11.5's HEADER PURSE, at the view-model layer. GDD §2.11.5, §2.4, §4.7 Stub 2.
//
// WHAT THIS FILE'S SUBJECT IS, AND IT IS NARROW ON PURPOSE. `StratBuildProductionMenu`'s
// six-argument overload is new in this wave and adds exactly one output: `OutPurseFame`. The
// two clauses below pin the two properties that overload was built for, and nothing else.
// The menu's ROUTING -- that every row is the bridge's row, in the bridge's order, with the
// bridge's bytes -- is `StratProductionMenuRouting.cpp`'s GATE-BUILDMENU set and is not
// restated here.
//
// WHY `T-UI-04` AND NOT SOME OTHER ROW. §2.11.5's header number IS the Fame purse, and
// T-UI-04's own GDD sentence is "the production menu binds to the buildlist derived from the
// four Stub-2 unit rows plus CURRENT FAMETOTAL". `OutPurseFame` is that `fameTotal`, handed to
// the menu's binding by the same call that built the rows. No acceptance ID is invented here;
// both clauses ride the one T-UI-04 already carries.
//
// WHERE EVERY EXPECTATION COMES FROM, WHICH IS THE ONLY QUESTION THAT MATTERS IN THIS LANE.
//  - Clause 1's purse expectation is `strat::UiSnapshot::side[Side].fameTotal`, read from the
//    SAME bridge through `FStratBridge::MakeUiSnapshot` with no command submitted between the
//    two calls. That is the module's own number, not one computed here. The shortfall side of
//    the same clause is compared against `Row.CostFame - OutPurseFame`, and BOTH of those come
//    off the returned row and the returned purse -- the point of the clause is precisely that
//    those two values are consistent, so there is nothing else it could be written against.
//  - Clause 2's expectation is the six-argument call's own output array, on the same bridge in
//    the same frame. A module-side value read, not a written-down menu.
// No price, no purse figure, no unit id and no refusal sentence is typed anywhere below.
//
// THE CASE-INSENSITIVITY TRAP. UE's `FString` `==`, `Contains`, `FName` comparison and
// `TestEqual` are ALL case-insensitive here. Clause 2 compares `Id` and `Reason` through
// `FString::Equals(..., ESearchCase::CaseSensitive)` so that a forward which lower-cased a
// reason sentence would be caught rather than waved through.
//
// THE FACTORY HEXES ARE ENUMERATED OFF THE VIEW MODEL AND NEVER WRITTEN DOWN, the same
// construction `StratProductionMenuRouting.cpp` uses and for the same reason: a defect that
// bites one factory must not be able to hide behind a case this file happened not to pick.
//
// NOT ASSERTED HERE, with reasons:
//  - That `UStratMatchSubsystem::ProductionMenuFameTotal` lands this value. That is a StratPlay
//    seam and lives in `Source/StratPlay/Tests/StratProductionPurseSeamClauses.cpp`, because
//    this module cannot see the subsystem.
//  - That any widget draws it. Wave 3 is model-side; no Slate is constructed below.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Internationalization/Text.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratViewModel.h"

// The bridge, included from a .cpp and never from a UHT-parsed header -- the same constraint
// StratUI.Build.cs and StratViewModel.h both record.
#include "StratBridge.h"

// NAMING `strat::UiSnapshot` here is legal and CALLING a `strat::` free function from this
// module is not; every rules answer below arrives through an `FStratBridge` method.
#include "Ui.h"

namespace StratProductionPurseClauses
{
	/** The harness pin the rest of the engine-side suite carries: no rule decides who is first. */
	static const int32 kFirstSide = 0;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static FString Describe(const FIntPoint& P)
	{
		return FString::Printf(TEXT("hex(q=%d,r=%d)"), P.X, P.Y);
	}

	/** The seeded opening: shipped tables mapped in, shipped scenario seeded. */
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

		const FString ScenarioPath = FPaths::Combine(DataDir(), TEXT("ferrum_crossing.json"));
		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath, kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	/** One question the menu can be asked. */
	struct FCase
	{
		FIntPoint Hex  = FIntPoint::ZeroValue;
		int32     Side = 0;
		FString   What;
	};

	/**
	 * Every question this board can put to the menu, read off the model rather than picked.
	 *
	 * Three shapes of question: a factory hex per side, one non-factory hex per side, and one
	 * side outside the match. The third is what exercises clause 2's REFUSAL arm -- see the
	 * block on it below, which records why the second does not.
	 */
	static bool BuildCases(const FStratViewModel& Model, TArray<FCase>& OutCases, FString& OutError)
	{
		OutCases.Reset();

		if (Model.Factories.Num() == 0)
		{
			OutError = TEXT("the seeded scenario carries no factories, so §2.11.5 has nothing to price");
			return false;
		}

		for (const FStratFactoryView& Factory : Model.Factories)
		{
			for (int32 Side = 0; Side < Model.Sides.Num(); ++Side)
			{
				FCase Entry;
				Entry.Hex  = Factory.Hex;
				Entry.Side = Side;
				Entry.What = FString::Printf(TEXT("side %d at the factory %s (owner %d)"),
					Side, *Describe(Factory.Hex), Factory.Owner);
				OutCases.Add(Entry);
			}
		}

		for (const FStratHexView& HexView : Model.Hexes)
		{
			bool bIsFactory = false;
			for (const FStratFactoryView& Factory : Model.Factories)
			{
				if (Factory.Hex == HexView.Hex)
				{
					bIsFactory = true;
					break;
				}
			}
			if (bIsFactory)
			{
				continue;
			}

			for (int32 Side = 0; Side < Model.Sides.Num(); ++Side)
			{
				FCase Entry;
				Entry.Hex  = HexView.Hex;
				Entry.Side = Side;
				Entry.What = FString::Printf(TEXT("side %d at %s, which is not a factory"),
					Side, *Describe(HexView.Hex));
				OutCases.Add(Entry);
			}
			break;
		}

		// THE ONE QUESTION THIS FUNCTION ACTUALLY REFUSES, and it is not the one a reader
		// expects. Asking at a hex that is no factory is an ANSWERED NO -- rows come back with
		// `bAvailable` false -- and `StratProductionMenuRouting.cpp`'s
		// `AnAnsweredNoIsNotARefusal` clause pins exactly that. The refusal is reserved for a
		// MALFORMED QUESTION: a side outside the match. MEASURED, not assumed -- clause 2's
		// refusal arm was empty and red on the first run with only the non-factory case in this
		// list, which is what identified the distinction.
		//
		// `Model.Sides.Num()` IS AN ARGUMENT AND NOT AN EXPECTATION -- one past the end of the
		// live model's own side array, so this case moves with the data rather than pinning
		// `strat::SIDE_COUNT` as a literal here.
		{
			FCase Entry;
			Entry.Hex  = OutCases[0].Hex;
			Entry.Side = Model.Sides.Num();
			Entry.What = FString::Printf(
				TEXT("side %d at %s, which is a side outside the match"),
				Entry.Side, *Describe(Entry.Hex));
			OutCases.Add(Entry);
		}

		return true;
	}

	/** Seeds, builds the view model, and enumerates the case list. */
	static bool Prepare(FStratBridge& Bridge, TArray<FCase>& OutCases, FString& OutError)
	{
		if (!SeedBridge(Bridge, OutError))
		{
			return false;
		}

		FStratViewModel Model;
		if (!StratBuildViewModel(Bridge, kFirstSide, Model, OutError))
		{
			return false;
		}

		return BuildCases(Model, OutCases, OutError);
	}
}

// ---------------------------------------------------------------------------
// Clause 1 -- the purse returned is the purse the rows were priced against.
//
// WHAT THIS PINS, IN ONE SENTENCE. On a SINGLE call, every unaffordable row's `Shortfall`
// equals `Row.CostFame - OutPurseFame` -- so the header a widget draws from `OutPurseFame` and
// the `need N` a widget draws from `Shortfall` cannot describe two different amounts of Fame.
// That is a STRUCTURAL guarantee rather than an asserted one: the two numbers come out of the
// same call, off the same local. `StratViewModel.h`'s own block names the defect this forbids
// -- a header printing `Fame: 250` above a 275-cost row reading `need 50` -- and it is exactly
// what a caller reading the purse off a separately-refreshed model would eventually print.
//
// THE AFFORDABLE ARM IS ASSERTED TOO, and it is not decoration. `Shortfall` is clamped at zero
// and set to zero outright when the module says affordable, so an implementation that returned
// a purse from some other instant could still satisfy the unaffordable arm alone if that purse
// happened to be low. Requiring `CostFame <= OutPurseFame` on every row the module called
// affordable closes that door from the other side.
//
// AND THE PURSE IS PINNED TO THE MODULE, NOT ONLY TO ITSELF. `OutPurseFame` is compared
// against `strat::UiSnapshot::side[Side].fameTotal` read off the same bridge in the same frame.
// Without that comparison the clause would be satisfied by any self-consistent pair of
// numbers, including two zeroes.
//
// `bAffordable` IS THE DISCRIMINATOR AND NEVER `Shortfall > 0`, per that field's own block:
// asking the shortfall which rows are unaffordable would make this clause depend on the
// subtraction it is checking.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionPurseIsTheRowsOwnPurseTest,
	"Stratocracy.StratUI.T-UI-04.ReturnedPurseIsTheOneEveryShortfallWasComputedAgainst",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratProductionPurseIsTheRowsOwnPurseTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionPurseClauses;

	FStratBridge  Bridge;
	TArray<FCase> Cases;
	FString       Error;
	const bool    bReady = Prepare(Bridge, Cases, Error);
	if (!TestTrue(TEXT("the bridge seeds and the board offers questions to ask"), bReady))
	{
		AddError(Error);
		return false;
	}

	// THE MODULE'S OWN PURSE, read once for the whole clause. No command is submitted below,
	// so the authoritative state behind this snapshot is the state behind every menu call.
	strat::UiSnapshot Snapshot;
	const FStratResult Snapped = Bridge.MakeUiSnapshot(Snapshot);
	if (!TestTrue(TEXT("the seeded bridge answers with a snapshot"), Snapped.bOk))
	{
		AddError(Snapped.Reason);
		return false;
	}

	int32 RowsChecked        = 0;
	int32 UnaffordableChecked = 0;

	for (const FCase& Ask : Cases)
	{
		TArray<FStratBuildOptionView> Menu;
		int32                         Purse = INDEX_NONE;
		FString                       Refusal;
		const bool bBuilt = StratBuildProductionMenu(
			Bridge, Ask.Side, Ask.Hex, Menu, Purse, Refusal);
		if (!bBuilt)
		{
			// A refusal is not this clause's subject; clause 2 covers it.
			continue;
		}

		// THE MODULE-SIDE EXPECTATION. The case list carries one deliberately out-of-range
		// side; the call above refuses it, so control never reaches here with it. Guarded
		// anyway, because an index derived from a case list is not a proof.
		if (!TestTrue(*FString::Printf(TEXT("%s: an ANSWERED menu was asked for a real side"), *Ask.What),
				Ask.Side >= 0 && Ask.Side < static_cast<int32>(strat::SIDE_COUNT)))
		{
			continue;
		}
		const int32 ModulePurse = static_cast<int32>(Snapshot.side[Ask.Side].fameTotal);
		TestEqual(*FString::Printf(
				TEXT("%s: the returned purse is the module's own fameTotal for that side"), *Ask.What),
			Purse, ModulePurse);

		for (int32 i = 0; i < Menu.Num(); ++i)
		{
			const FStratBuildOptionView& Row = Menu[i];
			const FString Slot = FString::Printf(TEXT("%s: row %d ('%s', cost %d, purse %d)"),
				*Ask.What, i, *Row.Id.ToString(), Row.CostFame, Purse);
			++RowsChecked;

			if (Row.bAffordable)
			{
				TestEqual(*FString::Printf(
						TEXT("%s: an affordable row names no shortfall"), *Slot),
					Row.Shortfall, 0);
				TestTrue(*FString::Printf(
						TEXT("%s: the returned purse covers a row the module called affordable"), *Slot),
					Row.CostFame <= Purse);
				continue;
			}

			++UnaffordableChecked;

			// THE TEETH. Established first, so the equality below cannot be satisfied by the
			// `FMath::Max(0, ...)` clamp standing in for a purse read at another instant.
			TestTrue(*FString::Printf(
					TEXT("%s: the returned purse is genuinely short of the row the module called unaffordable"), *Slot),
				Row.CostFame > Purse);

			TestEqual(*FString::Printf(
					TEXT("%s: the shortfall is the price against THIS purse, from THIS call"), *Slot),
				Row.Shortfall, Row.CostFame - Purse);
		}
	}

	// Vacuity guards. A board that offered nothing, or nothing unaffordable, would pass every
	// assertion above without exercising the subject.
	TestTrue(TEXT("the seeded board offered at least one priced row to check"), RowsChecked > 0);
	TestTrue(
		TEXT("the seeded board offered at least one UNAFFORDABLE row, so the shortfall arm was exercised"),
		UnaffordableChecked > 0);

	return true;
}

// ---------------------------------------------------------------------------
// Clause 2 -- the five-argument form is the six-argument form, minus the purse.
//
// WHY THIS IS A CLAUSE AT ALL. The five-argument overload has existing callers and an
// unchanged contract, and it is now a one-line forward into the new implementation. A forward
// is the cheapest thing in the file to get subtly wrong -- an argument transposed, a refusal
// swallowed, a `MoveTemp` that empties what it forwards -- and every existing GATE-BUILDMENU
// clause calls the five-argument form, so a defect in the SIX-argument body would be invisible
// to all of them while a defect in the FORWARD would redden them without naming the cause.
// This clause names it.
//
// BOTH PATHS ARE COMPARED, ANSWER AND REFUSAL. The case list includes a hex that is not a
// factory, so the two forms are required to agree about refusing, about the sentence they
// refuse with, and about leaving the caller's array alone.
//
// THE CALLER'S ARRAY IS PRE-POISONED before each call, so "left intact on a refusal" is
// distinguishable from "cleared and happened to be empty".
//
// `Id` AND `Reason` ARE COMPARED CASE-SENSITIVELY. `TestEqual` on an `FString`, `FName`
// comparison and `FText::EqualTo`'s underlying comparison would all pass on `INFANTRY`.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuOverloadsAgreeTest,
	"Stratocracy.StratUI.T-UI-04.TheFiveArgumentFormIsTheSixArgumentFormMinusThePurse",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratProductionMenuOverloadsAgreeTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionPurseClauses;

	FStratBridge  Bridge;
	TArray<FCase> Cases;
	FString       Error;
	const bool    bReady = Prepare(Bridge, Cases, Error);
	if (!TestTrue(TEXT("the bridge seeds and the board offers questions to ask"), bReady))
	{
		AddError(Error);
		return false;
	}

	// A row that belongs to neither call, used to poison both callers' arrays so that an
	// untouched array is distinguishable from an emptied one.
	FStratBuildOptionView Sentinel;
	Sentinel.DefIndex = 4242;
	Sentinel.Id       = FName(TEXT("StratProductionPurseClauses_Sentinel"));

	int32 AnswersCompared  = 0;
	int32 RefusalsCompared = 0;

	for (const FCase& Ask : Cases)
	{
		TArray<FStratBuildOptionView> FromSix;
		FromSix.Add(Sentinel);
		int32   Purse = INDEX_NONE;
		FString SixRefusal;
		const bool bSix = StratBuildProductionMenu(
			Bridge, Ask.Side, Ask.Hex, FromSix, Purse, SixRefusal);

		TArray<FStratBuildOptionView> FromFive;
		FromFive.Add(Sentinel);
		FString FiveRefusal;
		const bool bFive = StratBuildProductionMenu(
			Bridge, Ask.Side, Ask.Hex, FromFive, FiveRefusal);

		if (!TestEqual(*FString::Printf(TEXT("%s: the two forms agree about answering at all"), *Ask.What),
				bFive, bSix))
		{
			continue;
		}

		if (!bSix)
		{
			++RefusalsCompared;
			TestTrue(*FString::Printf(
					TEXT("%s: the two forms refuse in the same words, byte for byte ('%s' vs '%s')"),
					*Ask.What, *FiveRefusal, *SixRefusal),
				FiveRefusal.Equals(SixRefusal, ESearchCase::CaseSensitive));
			TestTrue(*FString::Printf(
					TEXT("%s: a refusal from the five-argument form leaves the caller's rows as it found them"),
					*Ask.What),
				FromFive.Num() == 1 && FromFive[0].DefIndex == Sentinel.DefIndex);
			TestTrue(*FString::Printf(
					TEXT("%s: a refusal from the six-argument form leaves the caller's rows as it found them"),
					*Ask.What),
				FromSix.Num() == 1 && FromSix[0].DefIndex == Sentinel.DefIndex);
			continue;
		}

		++AnswersCompared;

		TestTrue(*FString::Printf(TEXT("%s: a success clears both callers' failure strings"), *Ask.What),
			FiveRefusal.IsEmpty() && SixRefusal.IsEmpty());

		if (!TestEqual(*FString::Printf(TEXT("%s: the two forms return the same number of rows"), *Ask.What),
				FromFive.Num(), FromSix.Num()))
		{
			continue;
		}

		for (int32 i = 0; i < FromSix.Num(); ++i)
		{
			// THE EXPECTATION SIDE IS THE SIX-ARGUMENT CALL'S OWN ROW, on the same bridge in
			// the same frame -- not a menu written down in this file.
			const FStratBuildOptionView& Expected = FromSix[i];
			const FStratBuildOptionView& Actual   = FromFive[i];
			const FString Slot = FString::Printf(TEXT("%s: row %d"), *Ask.What, i);

			TestEqual(*FString::Printf(TEXT("%s: DefIndex, and in the same order"), *Slot),
				Actual.DefIndex, Expected.DefIndex);

			const FString ActualId   = Actual.Id.ToString();
			const FString ExpectedId = Expected.Id.ToString();
			TestTrue(*FString::Printf(TEXT("%s: Id byte for byte ('%s' vs '%s')"),
					*Slot, *ActualId, *ExpectedId),
				ActualId.Equals(ExpectedId, ESearchCase::CaseSensitive));

			TestEqual(*FString::Printf(TEXT("%s: CostFame"), *Slot),
				Actual.CostFame, Expected.CostFame);
			TestEqual(*FString::Printf(TEXT("%s: bAffordable"), *Slot),
				Actual.bAffordable, Expected.bAffordable);
			TestEqual(*FString::Printf(TEXT("%s: Shortfall"), *Slot),
				Actual.Shortfall, Expected.Shortfall);
			TestEqual(*FString::Printf(TEXT("%s: bAvailable"), *Slot),
				Actual.bAvailable, Expected.bAvailable);

			const FString ActualReason   = Actual.Reason.ToString();
			const FString ExpectedReason = Expected.Reason.ToString();
			TestTrue(*FString::Printf(TEXT("%s: Reason byte for byte ('%s' vs '%s')"),
					*Slot, *ActualReason, *ExpectedReason),
				ActualReason.Equals(ExpectedReason, ESearchCase::CaseSensitive));
		}
	}

	// Vacuity guards on BOTH arms -- a case list that only ever refused, or only ever
	// answered, would leave half this clause untested while it reported green.
	TestTrue(TEXT("at least one answered menu was compared across the two forms"),
		AnswersCompared > 0);
	TestTrue(TEXT("at least one refusal was compared across the two forms"),
		RefusalsCompared > 0);

	return true;
}
