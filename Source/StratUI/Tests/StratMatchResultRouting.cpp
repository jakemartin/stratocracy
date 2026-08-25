// GDD §2.8 / §2.11.4 / §2.11.5 -- the two projections `FStratViewModel` deliberately does
// not carry. T-UI-05 (the match result) and T-UI-03 (the production menu's shortfall).
//
// WHY THESE FOUR CLAUSES ARE ROUTING CLAUSES AND NOT PARITY CLAUSES, and it is a ruling
// rather than a gap. `StratViewModelParity.cpp` walks `FStratViewModel` field by field
// against a `strat::UiSnapshot` the same bridge projected. `FStratMatchResultView` is NOT a
// field of `FStratViewModel` -- upstream declared `uiMatchResult` a QUERY rather than a
// `UiSnapshot` field precisely so T-UI-05's field enumeration would not move, and the same
// decision was then made twice more down the stack: `FStratBridge::MatchResult` routes it as
// a query, and `StratBuildMatchResult` answers it on demand exactly as
// `StratBuildProductionMenu` does. So that walk will never reach this struct, there is no
// parity clause to write, and what needs pinning instead is the ROUTING: ask the bridge and
// ask the builder ON THE SAME BRIDGE IN THE SAME FRAME, and compare all four fields.
//
// WHAT WAS DARK BEFORE THEM. `strat::UiMatchView` -- the snapshot's match block, which
// `FStratMatchView` faithfully mirrors -- carries `resultTier` and DROPS `cause`, `winner`
// and `decidedByKey`. Every surface downstream of the projection could say *Decisive* and
// could not say FOR WHOM, while §2.11.4's end-of-match screen chooses its faction-voiced
// result line BY who won. `Source/StratBridge/Tests/StratMatchResultClauses.cpp` pins what
// the rules say about the winner under T-TURN-02/04/05; this file pins that the engine
// carries that answer across two layers without changing it.
//
// AND THE SHORTFALL IS THE SAME SHAPE OF THING FOR §2.11.5. `FStratBuildOptionView::Shortfall`
// is the one DECLARED-DERIVED field in the production menu's row that has no module-side
// counterpart at all: `strat::UiBuildOption` carries `costFame` and `affordable` and no
// shortfall. §2.11.5 requires an unaffordable row to stay visible, stay priced and NAME the
// shortfall (`need 50`), that sentence is arithmetic, and T-UI-03 forbids a widget performing
// it -- so `StratBuildProductionMenu` performs it, one layer below the screen, and these two
// clauses are what stop it drifting.
//
// WHERE THE EXPECTATIONS COME FROM.
//   - Every field of the match-result comparison has a module-side value on BOTH sides:
//     `FStratBridge::MatchResult` fills a `strat::UiMatchResult` and `StratBuildMatchResult`
//     fills an `FStratMatchResultView`, and the clause compares them. Nothing is written down.
//   - `strat::SIDE_NONE` and `INDEX_NONE` are ASSERTED equal rather than assumed, the same
//     treatment `strat::OWNER_NEUTRAL` gets on the owner fields, and for the same reason: they
//     are declared in two repositories, one vendored and one engine.
//   - The refusal sentence is `FStratBridge::MatchResult`'s own, taken from the bridge in the
//     same run and never transcribed.
//   - THE SHORTFALL IS THE ONE PLACE THIS FILE COMPUTES AN EXPECTATION, and it is stated
//     rather than hidden: `CostFame - FameTotal` has NO module-side value to read, which is
//     exactly why `FStratBuildOptionView::Shortfall` exists and is why its own declaration
//     carries the debt "DISCHARGED WHEN an upstream pass adds `shortfallFame` to
//     `UiBuildOption`". Both inputs are the module's -- `CostFame` mirrors
//     `UiBuildOption::costFame` and `FameTotal` mirrors `UiSideView::fameTotal` -- and the
//     subtraction is the only arithmetic in this file. When upstream discharges that debt,
//     the expectation here becomes an ordinary mirror and this paragraph goes with it.
//
// `bAffordable` IS THE AUTHORITY AND `Shortfall` IS NEVER READ TO DERIVE IT. Both clauses are
// keyed off `bAffordable`, never off `Shortfall > 0`; asking the shortfall instead would make
// the screen's greying depend on this project's subtraction rather than on the module's
// comparison, which is the substitution T-UI-03 exists to catch.
//
// EVERY STRING COMPARISON IS CASE SENSITIVE. `FString::operator==`, `Contains` and
// `TestEqual` all ignore case in UE 5.8, measured on this project.
//
// WHY THIS FILE LIVES IN StratUI/Tests. `StratBuildMatchResult`, `StratBuildProductionMenu`,
// `StratBuildViewModel`, `FStratMatchResultView` and `FStratBuildOptionView` are all declared
// in StratUI, and StratBridge sits BELOW it -- the arrow does not reverse, so a StratBridge
// test could not name any of them. The rules-side values arrive as METHODS on the exported
// `FStratBridge`; the only `strat::` names below are TYPES and CONSTANTS, which is legal from
// any module, and no `strat::` free function is called anywhere in this file.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratViewModel.h"

// The bridge, included from a .cpp and never from a UHT-parsed header -- the same constraint
// StratScoreboardHUD.h records about itself.
#include "StratBridge.h"

#include "Turn.h"
#include "Ui.h"

namespace StratMatchResultRouting
{
	/** The harness pin every parity file in this suite carries, and it must stay equal to
	 *  theirs. `Replay.h` states that no rule in this project decides which side moves first. */
	static const int32 kFirstSide = 0;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** Definitions only -- the bridge state between the two refusal reasons. */
	static bool LoadDefsOnly(FStratBridge& Bridge, FString& OutError)
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
		return true;
	}

	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		if (!LoadDefsOnly(Bridge, OutError))
		{
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

	/** The hex of the first factory the named side holds, read off the model. A clause that
	 *  named a hex would go quiet the day the scenario moved it. */
	static bool FindHeldFactory(const FStratViewModel& Model, int32 Side, FIntPoint& OutHex)
	{
		for (const FStratFactoryView& Factory : Model.Factories)
		{
			if (Factory.Owner == Side)
			{
				OutHex = Factory.Hex;
				return true;
			}
		}
		return false;
	}
}

// ---------------------------------------------------------------------------
// T-UI-05 -- `FStratMatchResultView` is exactly what the bridge said, in engine types.
//
// ONE BRIDGE, ONE FRAME, FOUR FIELDS. The comparison is `FStratBridge::MatchResult`'s
// `strat::UiMatchResult` against `StratBuildMatchResult`'s `FStratMatchResultView`, asked of
// the same object with nothing in between. Both sides of every assertion are module-side
// values; this file supplies neither.
//
// THE TWO ENUMS ARE COMPARED BY ORDINAL, WHICH IS THE CONTRACT ITSELF. `EStratResultTier` and
// `EStratResultCause` are ORDINAL-PINNED to `strat::ResultTier` and `strat::ResultCause` --
// same enumerators, same order, same names -- and a reordering upstream would show up here as
// a mismatched cast rather than as a screen that says *Domination* when the flag went down.
//
// AND `INDEX_NONE == strat::SIDE_NONE` IS ASSERTED, NOT ASSUMED. The two constants are
// declared in two repositories. If they ever diverge, every "no winner" reading in this
// engine flips meaning silently, and this line is the only place that would say so.
//
// THE PROJECTION IS NOT A MOVE. Both witnesses -- the canonical state hash and the recorded
// command count -- are taken before and after, because a query that submitted anything would
// make a result screen part of the match it reports on.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratMatchResultViewRoutesTheBridgeAnswerTest,
	"Stratocracy.StratUI.T-UI-05.MatchResultViewRoutesTheBridgeAnswer",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratMatchResultViewRoutesTheBridgeAnswerTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchResultRouting;

	// The two repositories' spellings of "nobody". Asserted first, because every winner
	// assertion below and every reading of `FStratMatchResultView::Winner` anywhere in this
	// engine depends on it.
	TestEqual(TEXT("strat::SIDE_NONE and INDEX_NONE are the same constant"),
		static_cast<int32>(strat::SIDE_NONE), static_cast<int32>(INDEX_NONE));

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	const FString HashBefore = Bridge.StateHash();
	const int32   LogBefore  = Bridge.RecordedCommandCount();

	// ---- The two answers, same bridge, same frame ---------------------------
	strat::UiMatchResult FromBridge;
	const FStratResult Asked = Bridge.MatchResult(FromBridge);
	if (!TestTrue(TEXT("FStratBridge::MatchResult answers a seeded bridge"), Asked.bOk))
	{
		AddError(Asked.Reason);
		return false;
	}

	FStratMatchResultView FromBuilder;
	FString               BuildReason;
	if (!TestTrue(TEXT("StratBuildMatchResult answers the same bridge"),
			StratBuildMatchResult(Bridge, FromBuilder, BuildReason)))
	{
		AddError(BuildReason);
		return false;
	}
	TestTrue(TEXT("a successful build names no failure reason"), BuildReason.IsEmpty());

	// ---- All four fields ----------------------------------------------------
	TestEqual(TEXT("Tier == UiMatchResult::tier, by ordinal"),
		static_cast<int32>(FromBuilder.Tier), static_cast<int32>(FromBridge.tier));
	TestEqual(TEXT("Cause == UiMatchResult::cause, by ordinal"),
		static_cast<int32>(FromBuilder.Cause), static_cast<int32>(FromBridge.cause));
	TestEqual(TEXT("Winner == UiMatchResult::winner, convention included"),
		FromBuilder.Winner, static_cast<int32>(FromBridge.winner));
	TestEqual(TEXT("DecidedByKey == UiMatchResult::decidedByKey"),
		FromBuilder.DecidedByKey, static_cast<int32>(FromBridge.decidedByKey));

	// ---- And it agrees with the model the screen is drawn from ---------------
	// `FStratMatchResultView` deliberately carries no `bHasResult`: a second copy could
	// disagree with `FStratMatchView`'s, which is read from the module rather than inferred.
	// `Tier == InProgress` is upstream's own null, and this asserts the two spell the SAME
	// state -- which is the whole reason the field was left off.
	{
		FStratViewModel Model;
		FString         ModelReason;
		if (TestTrue(TEXT("the view model builds off the same bridge"),
				StratBuildViewModel(Bridge, kFirstSide, Model, ModelReason)))
		{
			TestEqual(TEXT("the view model's tier is the same tier"),
				static_cast<int32>(Model.Match.ResultTier), static_cast<int32>(FromBuilder.Tier));
			TestTrue(
				TEXT("and `Tier == InProgress` says exactly what `bHasResult == false` says"),
				(FromBuilder.Tier == EStratResultTier::InProgress) == (!Model.Match.bHasResult));
		}
		else
		{
			AddError(ModelReason);
		}
	}

	// A freshly seeded match has not ended, and the fields say so in upstream's own
	// vocabulary. This is not an independent expectation -- it is the null both sides above
	// agreed on, named once so a reader can see which state the comparison ran over.
	TestEqual(TEXT("a seeded opening is InProgress"),
		static_cast<int32>(FromBuilder.Tier),
		static_cast<int32>(EStratResultTier::InProgress));
	TestEqual(TEXT("with no cause"),
		static_cast<int32>(FromBuilder.Cause), static_cast<int32>(EStratResultCause::None));
	TestEqual(TEXT("no winner"), FromBuilder.Winner, static_cast<int32>(INDEX_NONE));
	TestEqual(TEXT("and no §2.8 key evaluated"), FromBuilder.DecidedByKey, 0);

	// ---- Reading a match is not playing in it -------------------------------
	TestEqual(TEXT("asking twice did not move the canonical state hash"),
		Bridge.StateHash(), HashBefore);
	TestEqual(TEXT("asking twice submitted no command"),
		Bridge.RecordedCommandCount(), LogBefore);

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-05 -- an unseeded bridge is REFUSED, and the caller's value is left exactly as it was.
//
// WHY REFUSING MATTERS MORE HERE THAN ANYWHERE ELSE IN THAT FILE. `strat::uiMatchResult`
// answers a world with no turn state `InProgress` / `SIDE_NONE`, which is INDISTINGUISHABLE
// from a live match that has not ended -- and upstream says so, and says the two are
// deliberately spelled the same way because the only safe thing a caller can do with either
// is the same thing. `FStratBridge::MatchResult` is the one place that CAN tell them apart,
// because `IsSeeded()` lives there, and it refuses rather than answering InProgress.
// `StratBuildMatchResult` forwards that refusal in the bridge's own words.
//
// ALL-OR-NOTHING IS THE HALF WITH TEETH, and it matters more for this struct than for any
// other in the file: a result screen blanked by a transient refusal would announce a match
// still in progress. So the caller's `FStratMatchResultView` is pre-loaded with values no
// successful build could produce, and every one of the four is required to survive the
// refusal untouched. A builder that cleared its out-parameter before failing passes every
// other assertion in this body.
//
// THE REFUSAL SENTENCE IS THE BRIDGE'S OWN, taken from the same bridge in the same run and
// compared CASE-SENSITIVELY. Transcribing it would be this file authoring a second spelling
// of a message the bridge owns.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratMatchResultRefusesAnUnseededBridgeTest,
	"Stratocracy.StratUI.T-UI-05.MatchResultRefusesAnUnseededBridge",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratMatchResultRefusesAnUnseededBridgeTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchResultRouting;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("definitions load"), LoadDefsOnly(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestFalse(TEXT("and the bridge is deliberately NOT seeded"), Bridge.IsSeeded()))
	{
		return false;
	}

	// SENTINELS NO SUCCESSFUL BUILD COULD WRITE. `Decisive` with cause `None` and a winner of
	// -2 is not a state the rules module can produce, so "untouched" below is a real
	// measurement rather than a coincidence with a default.
	FStratMatchResultView Untouched;
	Untouched.Tier         = EStratResultTier::Decisive;
	Untouched.Cause        = EStratResultCause::None;
	Untouched.Winner       = INDEX_NONE - 1;
	Untouched.DecidedByKey = INDEX_NONE;

	const FStratMatchResultView Before = Untouched;

	FString Reason;
	TestFalse(TEXT("StratBuildMatchResult refuses an unseeded bridge"),
		StratBuildMatchResult(Bridge, Untouched, Reason));
	TestFalse(TEXT("and the refusal names a reason"), Reason.IsEmpty());

	// THE ALL-OR-NOTHING CLAUSE. Four fields, all four required to be exactly what the caller
	// held.
	TestEqual(TEXT("a refusal left Tier untouched"),
		static_cast<int32>(Untouched.Tier), static_cast<int32>(Before.Tier));
	TestEqual(TEXT("a refusal left Cause untouched"),
		static_cast<int32>(Untouched.Cause), static_cast<int32>(Before.Cause));
	TestEqual(TEXT("a refusal left Winner untouched"), Untouched.Winner, Before.Winner);
	TestEqual(TEXT("a refusal left DecidedByKey untouched"),
		Untouched.DecidedByKey, Before.DecidedByKey);

	// THE REASON IS THE BRIDGE'S, NEVER TRANSCRIBED. Asked of the same bridge in the same run;
	// `StratBuildMatchResult`'s declaration records that it forwards these words.
	{
		strat::UiMatchResult Ignored;
		const FStratResult FromBridge = Bridge.MatchResult(Ignored);
		TestFalse(TEXT("the bridge itself refuses the same call"), FromBridge.bOk);
		TestTrue(
			*FString::Printf(TEXT("the builder's refusal carries the bridge's own words "
				"('%s' contains '%s')"), *Reason, *FromBridge.Reason),
			Reason.Contains(FromBridge.Reason, ESearchCase::CaseSensitive));
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-03 -- an affordable row names NO shortfall.
//
// §2.11.5 shows a shortfall on the rows the side cannot pay for and nothing on the ones it
// can. `bAffordable` is the module's comparison -- `costFame <= this side's fameTotal`,
// computed in `uiBuildOptions` PRECISELY so T-UI-03 has something to bind to -- and this
// clause is one half of keeping the derived number in step with it: wherever the module says
// affordable, the derived field is ZERO and not a negative number, not a leftover from
// another row, and not the cost.
//
// IT IS KEYED OFF `bAffordable` AND NEVER OFF `Shortfall`. Asking `Shortfall == 0` to decide
// which rows to check would make this clause agree with itself by construction.
//
// A COVERAGE GUARD, BECAUSE AN EMPTY LOOP IS A GREEN CLAUSE. The shipped scenario opens both
// sides at 200 Fame against a four-row §2.4 table, so affordable rows exist -- but if a
// re-priced table ever removed them, this clause would pass over nothing at all and say
// nothing about it. It fails loudly instead.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShortfallIsZeroWhenAffordableTest,
	"Stratocracy.StratUI.T-UI-03.ShortfallIsZeroWheneverTheModuleSaysAffordable",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShortfallIsZeroWhenAffordableTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchResultRouting;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Model;
	FString         ModelReason;
	if (!TestTrue(TEXT("the view model builds"),
			StratBuildViewModel(Bridge, kFirstSide, Model, ModelReason)))
	{
		AddError(ModelReason);
		return false;
	}

	FIntPoint FactoryHex = FIntPoint::ZeroValue;
	if (!TestTrue(TEXT("the viewing side holds a factory to open a menu at"),
			FindHeldFactory(Model, kFirstSide, FactoryHex)))
	{
		return false;
	}

	TArray<FStratBuildOptionView> Rows;
	FString                       MenuReason;
	if (!TestTrue(TEXT("the production menu builds for that factory"),
			StratBuildProductionMenu(Bridge, kFirstSide, FactoryHex, Rows, MenuReason)))
	{
		AddError(MenuReason);
		return false;
	}

	int32 Affordable = 0;
	for (const FStratBuildOptionView& Row : Rows)
	{
		if (!Row.bAffordable)
		{
			continue;
		}
		++Affordable;

		TestEqual(
			*FString::Printf(TEXT("row '%s' costs %d and the module calls it affordable, so it "
				"names no shortfall"), *Row.Id.ToString(), Row.CostFame),
			Row.Shortfall, 0);
	}

	// COVERAGE. Without this the loop above is green over an empty set.
	if (!TestTrue(
			*FString::Printf(TEXT("the module calls at least one of the %d offered rows "
				"affordable, so the loop above measured something"), Rows.Num()),
			Affordable > 0))
	{
		return false;
	}

	AddInfo(FString::Printf(TEXT("%d of %d rows affordable at %d Fame"),
		Affordable, Rows.Num(),
		Model.Sides.IsValidIndex(kFirstSide) ? Model.Sides[kFirstSide].FameTotal : INDEX_NONE));

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-03 -- an unaffordable row names the EXACT shortfall.
//
// §2.11.5: unaffordable rows stay visible, stay priced, and say `need 50`. That sentence is
// arithmetic and T-UI-03 forbids a widget performing it, so the lowest layer that can see
// both the price and the purse performs it instead. This clause is what pins the number.
//
// THIS IS THE ONE CLAUSE IN THIS FILE THAT COMPUTES ITS OWN EXPECTATION, AND THE REASON IS
// STATED RATHER THAN IMPLIED: no module-side value exists to read. `strat::UiBuildOption`
// carries `costFame` and `affordable` and no shortfall -- that absence is exactly why
// `FStratBuildOptionView::Shortfall` was added, and its declaration records the debt as
// DISCHARGED WHEN an upstream pass adds `shortfallFame`, at which point this expectation
// becomes an ordinary mirror and the subtraction below is deleted rather than moved. BOTH
// INPUTS ARE STILL THE MODULE'S: `CostFame` mirrors `UiBuildOption::costFame` and `FameTotal`
// mirrors `UiSideView::fameTotal`, both read off the same bridge in the same frame as the
// row. What this file supplies is one minus sign.
//
// TWO PROPERTIES, NOT ONE. The shortfall equals the difference AND is strictly positive --
// because "equals the difference" is also satisfied by a zero on a row the module already
// said was unaffordable, if the difference were ever computed the wrong way round. §2.11.5
// prints `need N`, and an N of 0 or below is not a sentence that can be shown.
//
// AND THE ROW IS STILL PRICED AND STILL OFFERED. An unaffordable row is greyed, never hidden,
// so `CostFame` must survive and the row must still be in the list -- which is asserted by
// the row being walked at all, and by `CostFame` being the number the subtraction used.
//
// COVERAGE. The shipped scenario opens at 200 Fame against a §2.4 table whose top row costs
// more, so unaffordable rows exist. A re-priced table that made everything affordable would
// leave this clause passing over nothing; it fails loudly instead.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratUnaffordableRowNamesTheExactShortfallTest,
	"Stratocracy.StratUI.T-UI-03.UnaffordableRowNamesTheExactShortfall",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratUnaffordableRowNamesTheExactShortfallTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchResultRouting;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Model;
	FString         ModelReason;
	if (!TestTrue(TEXT("the view model builds"),
			StratBuildViewModel(Bridge, kFirstSide, Model, ModelReason)))
	{
		AddError(ModelReason);
		return false;
	}

	if (!TestTrue(TEXT("the model carries a side view for the viewing side"),
			Model.Sides.IsValidIndex(kFirstSide)))
	{
		return false;
	}

	// THE PURSE, READ FROM THE MODULE. `FStratSideView::FameTotal` mirrors
	// `strat::UiSideView::fameTotal`, which is the same number `uiBuildOptions` compared
	// against when it set `affordable`.
	const int32 FameTotal = Model.Sides[kFirstSide].FameTotal;

	FIntPoint FactoryHex = FIntPoint::ZeroValue;
	if (!TestTrue(TEXT("the viewing side holds a factory to open a menu at"),
			FindHeldFactory(Model, kFirstSide, FactoryHex)))
	{
		return false;
	}

	TArray<FStratBuildOptionView> Rows;
	FString                       MenuReason;
	if (!TestTrue(TEXT("the production menu builds for that factory"),
			StratBuildProductionMenu(Bridge, kFirstSide, FactoryHex, Rows, MenuReason)))
	{
		AddError(MenuReason);
		return false;
	}

	int32 Unaffordable = 0;
	for (const FStratBuildOptionView& Row : Rows)
	{
		// KEYED OFF THE MODULE'S COMPARISON, never off the derived field.
		if (Row.bAffordable)
		{
			continue;
		}
		++Unaffordable;

		// The one subtraction in this file. Both operands are the module's.
		const int32 Expected = Row.CostFame - FameTotal;

		TestEqual(
			*FString::Printf(TEXT("row '%s' costs %d against %d Fame, so §2.11.5's 'need N' is "
				"%d"), *Row.Id.ToString(), Row.CostFame, FameTotal, Expected),
			Row.Shortfall, Expected);

		// A shortfall that is not a positive number is not a sentence §2.11.5 can print, and
		// this is the assertion the equality above cannot make on its own.
		TestTrue(
			*FString::Printf(TEXT("and row '%s''s shortfall is strictly positive (%d)"),
				*Row.Id.ToString(), Row.Shortfall),
			Row.Shortfall > 0);

		// Greyed, never hidden: the row is still here and still carries its price.
		TestTrue(
			*FString::Printf(TEXT("and row '%s' is still priced (%d), not hidden"),
				*Row.Id.ToString(), Row.CostFame),
			Row.CostFame > 0);
	}

	if (!TestTrue(
			*FString::Printf(TEXT("the module calls at least one of the %d offered rows "
				"unaffordable at %d Fame, so the loop above measured something"),
				Rows.Num(), FameTotal),
			Unaffordable > 0))
	{
		return false;
	}

	AddInfo(FString::Printf(TEXT("%d of %d rows unaffordable at %d Fame"),
		Unaffordable, Rows.Num(), FameTotal));

	return true;
}
