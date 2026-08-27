// Wave 3 -- §2.11.2's HUD selectors over `FStratViewModel`. GDD §2.11.2, §2.11.4, §4.9.
//
// WHAT THIS FILE'S SUBJECT IS. `UStratViewModelLibrary::GetSideView` and
// `GetViewingSideView`, and specifically the two properties that make them safe to bind a
// persistent HUD to: that the row they hand back is the model's own row for the side asked
// for, and that the side asked for by `GetViewingSideView` is `FStratViewModel::ViewingSide`
// and never `FStratMatchView::SideToMove`.
//
// WHY `T-UI-05`, WHICH IS SNAPSHOT FIDELITY. `FStratSideView` is mirrored one-for-one from
// `strat::UiSideView`, and the question "does the snapshot's side row reach a reader
// truthfully" is exactly T-UI-05's. The `+X/turn` income line §2.11.2 draws is
// `FStratSideView::IncomePerTurn`, a snapshot field, and this selector is now its only route
// to a widget: `IncomePerTurn` had ZERO readers before this wave outside the parity clauses.
// No acceptance ID is invented here.
//
// WHAT IS PINNED ELSEWHERE AND NOT RESTATED. That `FStratSideView` mirrors
// `strat::UiSideView` field for field is `StratViewModelParity.cpp`'s job. This file takes the
// built model as given and pins only the SELECTION over it.
//
// WHERE EVERY EXPECTATION COMES FROM. The right-hand side of every comparison below is a live
// `FStratViewModel` -- `Model.Sides[i]` read directly, or a freshly default-constructed
// `FStratSideView()`. Not one Fame figure, income figure or side index is typed as a literal
// expectation anywhere in this file. The two side indices that ARE written down
// (`Model.Sides.Num()` and `-1`) are ARGUMENTS -- the malformed questions the selector is
// required to refuse -- and not expectations.
//
// THE OUT-PARAMETER IS PRE-POISONED before every refusing call. `GetSideView` resets `OutSide`
// on entry, so without a poison an assertion that a refusal leaves it default-constructed
// would pass identically on a function that never touched it and on one that cleared it. The
// poison makes "cleared" the only reading that survives.
//
// NOT ASSERTED HERE, and named rather than left silent:
//  - `CountUnitsAbleToAct` and `CountViewingSideUnitsAbleToAct`. Their subject is `bDone` and
//    `bLockedThisTurn`, and `StratViewModel.h`'s PRESENTATION BLOCK rules those OUT of
//    T-UI-05's subject in terms -- they have no module-side counterpart for a fidelity clause
//    to compare against. No existing acceptance ID covers the idle count, and this lane does
//    not mint one. Recorded in `Tools/architect/state/tests.md` as an unpinned surface.
//  - Any widget. Nothing below constructs Slate.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratViewModel.h"
#include "StratViewModelLibrary.h"

// The bridge, included from a .cpp and never from a UHT-parsed header.
#include "StratBridge.h"

namespace StratViewingSideSelectorClauses
{
	/** The harness pin the rest of the engine-side suite carries. */
	static const int32 kFirstSide = 0;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
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

		const FString ScenarioPath = FPaths::Combine(DataDir(), TEXT("ferrum_crossing.json"));
		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath, kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	/** A `FStratSideView` compared field by field, with the failing field named. */
	static void CompareSideRows(FAutomationTestBase& Test, const FString& What,
	                            const FStratSideView& Actual, const FStratSideView& Expected)
	{
		Test.TestEqual(*FString::Printf(TEXT("%s: FameTotal"), *What),
			Actual.FameTotal, Expected.FameTotal);
		Test.TestEqual(*FString::Printf(TEXT("%s: FameCombat"), *What),
			Actual.FameCombat, Expected.FameCombat);
		Test.TestEqual(*FString::Printf(TEXT("%s: ObjectivesHeld"), *What),
			Actual.ObjectivesHeld, Expected.ObjectivesHeld);
		Test.TestEqual(*FString::Printf(TEXT("%s: SurvivingHp"), *What),
			Actual.SurvivingHp, Expected.SurvivingHp);
		Test.TestEqual(*FString::Printf(TEXT("%s: IncomePerTurn"), *What),
			Actual.IncomePerTurn, Expected.IncomePerTurn);
	}

	/** The poison: a side row no seeded board could produce, so "untouched" is visible. */
	static FStratSideView PoisonRow()
	{
		FStratSideView Row;
		Row.FameTotal      = -987654;
		Row.FameCombat     = -987654;
		Row.ObjectivesHeld = -987654;
		Row.SurvivingHp    = -987654;
		Row.IncomePerTurn  = -987654;
		return Row;
	}

	/**
	 * Spends some of side 0's Fame, so that the two sides' rows stop being interchangeable.
	 *
	 * WHY THIS IS NEEDED AND IS NOT A CONVENIENCE. `Data/ferrum_crossing.json` seeds both sides
	 * with the same `startingFame` and a symmetric board, so at turn 1 `Sides[0]` and
	 * `Sides[1]` can carry identical numbers -- and a clause that asked "did the selector
	 * return the VIEWING side's row" against two identical rows could not fail whichever row
	 * it returned. A build by side 0 deducts that row's price from side 0's purse and nobody
	 * else's, which is the cheapest thing on this board that makes the two rows distinguishable.
	 * The clause that uses this asserts the divergence explicitly before relying on it.
	 *
	 * THE ROW BUILT IS FOUND, NEVER NAMED. The factory comes off `FStratViewModel::Factories`
	 * and the definition index off the menu the module answered with.
	 */
	static bool SpendSomeOfSideZerosFame(FStratBridge& Bridge, FString& OutError)
	{
		FStratViewModel Model;
		if (!StratBuildViewModel(Bridge, kFirstSide, Model, OutError))
		{
			return false;
		}

		for (const FStratFactoryView& Factory : Model.Factories)
		{
			if (Factory.Owner != kFirstSide)
			{
				continue;
			}

			TArray<FStratBuildOptionView> Menu;
			FString                       Refusal;
			if (!StratBuildProductionMenu(Bridge, kFirstSide, Factory.Hex, Menu, Refusal))
			{
				continue;
			}

			for (const FStratBuildOptionView& Row : Menu)
			{
				if (!Row.bAvailable || !Row.bAffordable || Row.CostFame <= 0)
				{
					continue;
				}

				const FStratResult Built = Bridge.SubmitBuildAtHex(Factory.Hex, Row.DefIndex);
				if (Built.bOk)
				{
					return true;
				}
				OutError = FString::Printf(TEXT("SubmitBuildAtHex refused: %s"), *Built.Reason);
			}
		}

		if (OutError.IsEmpty())
		{
			OutError = TEXT(
				"no available, affordable, non-free row was offered to side 0 at any factory it holds, "
				"so this fixture could not drive the two sides' rows apart");
		}
		return false;
	}
}

// ---------------------------------------------------------------------------
// Clause 1 -- the selector hands back the model's own row, and refuses rather than defaulting.
//
// TWO HALVES, AND THE SECOND IS THE ONE WITH TEETH. That `GetSideView(Model, i, Out)` yields
// `Model.Sides[i]` is the easy half. The half that matters is the refusal: a HUD asked about a
// side outside the match must not be told `Fame: 0`, because zero is a number no rules state
// produced and it reads on screen exactly like a bankrupt player. `GetSideView`'s own block
// states that in terms -- "answering `Fame: 0` would put a number on screen that no rules
// state produced" -- and the return value is the only discriminator.
//
// `GetViewingSideView` IS PINNED HERE AS THE PAIRING and nothing more: it must agree with
// `GetSideView(Model, Model.ViewingSide, ...)` on the answer AND on the refusal. Which side it
// pairs to is clause 2's subject, because that question needs two sides that differ.
//
// THE OUT-OF-RANGE ARGUMENTS COME OFF THE LIVE MODEL (`Model.Sides.Num()`), not from
// `strat::SIDE_COUNT` typed here, so a scenario that ever carried a different number of sides
// would still be asked a question one past its own end.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSideViewSelectorRefusesRatherThanDefaultsTest,
	"Stratocracy.StratUI.T-UI-05.SideSelectorReturnsTheModelsRowAndRefusesOutOfRange",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSideViewSelectorRefusesRatherThanDefaultsTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratViewingSideSelectorClauses;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Model;
	if (!TestTrue(TEXT("the view model builds for the first side"),
			StratBuildViewModel(Bridge, kFirstSide, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	if (!TestTrue(TEXT("the built model carries at least one side row to select"),
			Model.Sides.Num() > 0))
	{
		return false;
	}

	// ---- the answering half, over every side the model actually carries ----
	for (int32 Side = 0; Side < Model.Sides.Num(); ++Side)
	{
		FStratSideView Out = PoisonRow();
		const bool bGot = UStratViewModelLibrary::GetSideView(Model, Side, Out);
		if (!TestTrue(*FString::Printf(TEXT("side %d is in range and is answered"), Side), bGot))
		{
			continue;
		}
		CompareSideRows(*this, FString::Printf(TEXT("side %d is Model.Sides[%d]"), Side, Side),
			Out, Model.Sides[Side]);
	}

	// ---- the pairing, on the answering path ----
	{
		FStratSideView ViaViewing = PoisonRow();
		FStratSideView ViaIndex   = PoisonRow();
		const bool bViewing = UStratViewModelLibrary::GetViewingSideView(Model, ViaViewing);
		const bool bIndex   = UStratViewModelLibrary::GetSideView(Model, Model.ViewingSide, ViaIndex);
		TestEqual(TEXT("the viewing-side selector answers exactly when the indexed one does"),
			bViewing, bIndex);
		if (bViewing && bIndex)
		{
			CompareSideRows(*this,
				FString::Printf(TEXT("GetViewingSideView is GetSideView(ViewingSide=%d)"), Model.ViewingSide),
				ViaViewing, ViaIndex);
		}
	}

	// ---- the refusing half ----
	// A FRESHLY DEFAULT-CONSTRUCTED ROW IS THE EXPECTATION, not five zeroes typed out. If
	// `FStratSideView` ever grows a member with a non-zero default, this clause moves with it.
	const FStratSideView DefaultRow;

	const int32 OnePastTheEnd = Model.Sides.Num();
	{
		FStratSideView Out = PoisonRow();
		const bool bGot = UStratViewModelLibrary::GetSideView(Model, OnePastTheEnd, Out);
		TestFalse(*FString::Printf(TEXT("side %d is outside the match and is refused"), OnePastTheEnd), bGot);
		CompareSideRows(*this,
			FString::Printf(TEXT("a refusal at side %d leaves the caller's row default-constructed"), OnePastTheEnd),
			Out, DefaultRow);
	}
	{
		FStratSideView Out = PoisonRow();
		const bool bGot = UStratViewModelLibrary::GetSideView(Model, -1, Out);
		TestFalse(TEXT("a negative side is refused"), bGot);
		CompareSideRows(*this,
			TEXT("a refusal at side -1 leaves the caller's row default-constructed"),
			Out, DefaultRow);
	}

	// ---- the pairing, on the refusing path ----
	// `ViewingSide` IS DRIVEN OUT OF RANGE ON A COPY. `FStratViewModel` is a value the caller
	// owns; nothing under test is edited to produce this state.
	{
		FStratViewModel Broken = Model;
		Broken.ViewingSide = OnePastTheEnd;

		FStratSideView Out = PoisonRow();
		const bool bGot = UStratViewModelLibrary::GetViewingSideView(Broken, Out);
		TestFalse(
			TEXT("a model whose ViewingSide is outside its own Sides array is refused, not defaulted"),
			bGot);
		CompareSideRows(*this,
			TEXT("that refusal leaves the caller's row default-constructed"),
			Out, DefaultRow);
	}

	// ---- an empty model refuses too ----
	{
		const FStratViewModel Empty;
		FStratSideView Out = PoisonRow();
		const bool bGot = UStratViewModelLibrary::GetViewingSideView(Empty, Out);
		TestFalse(TEXT("a model with no side rows at all is refused"), bGot);
		CompareSideRows(*this,
			TEXT("an empty model's refusal leaves the caller's row default-constructed"),
			Out, DefaultRow);
	}

	return true;
}

// ---------------------------------------------------------------------------
// Clause 2 -- the HUD's side is `ViewingSide`, and never `Match.SideToMove`.
//
// THE DEFECT THIS FORBIDS, IN ONE SENTENCE. In hot-seat the two differ on every other turn,
// and a HUD wired to `SideToMove` shows the OPPONENT'S purse and the opponent's `+X/turn` on
// every screen drawn between the turn flipping and the hand-over completing. Both fields exist
// on the same built model and both are `int32`, so the two spellings compile identically and
// differ only in what a player sees.
//
// THE TWO SIDES ARE DRIVEN APART FIRST, AND THE DIVERGENCE IS ASSERTED BEFORE IT IS RELIED ON.
// The shipped scenario is symmetric at turn 1 -- same starting Fame both sides -- so on an
// untouched board this clause could not fail whichever row the selector returned. A build by
// side 0 spends side 0's Fame and nobody else's; the clause then requires
// `Sides[SideToMove].FameTotal != Sides[ViewingSide].FameTotal` and fails loudly, naming the
// fixture, if that is ever untrue. It does NOT quietly pass.
//
// THE MODEL IS BUILT FOR SIDE 1 WHILE SIDE 0 IS TO MOVE, which is the hot-seat hand-over
// moment written as a value. `StratBuildViewModel`'s `ViewingSide` parameter is the caller's
// statement about whose screen this is; `FStratMatchView::SideToMove` is the rules module's
// answer about the match. Both come off the same call, so neither is written down here.
//
// AND IT IS ASSERTED IN BOTH DIRECTIONS. Equality with the viewing side's row would be
// satisfied by an implementation that returned the right row by luck on a board where the two
// happened to agree; the extra inequality against the side-to-move's row is what makes the
// clause name the defect rather than merely miss it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratViewingSideSelectorTracksTheViewerTest,
	"Stratocracy.StratUI.T-UI-05.ViewingSideSelectorTracksTheViewerAndNotTheSideToMove",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratViewingSideSelectorTracksTheViewerTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratViewingSideSelectorClauses;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	if (!TestTrue(TEXT("side 0 spends some Fame, so the two sides' rows stop being interchangeable"),
			SpendSomeOfSideZerosFame(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	// THE HAND-OVER MOMENT: the model is rendered for side 1 while the rules module still says
	// side 0 is to move.
	const int32 TheViewer = 1;
	FStratViewModel Model;
	if (!TestTrue(TEXT("the view model builds for the viewing side"),
			StratBuildViewModel(Bridge, TheViewer, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	if (!TestTrue(TEXT("the model carries a row for both the viewer and the side to move"),
			Model.Sides.IsValidIndex(Model.ViewingSide) && Model.Sides.IsValidIndex(Model.Match.SideToMove)))
	{
		return false;
	}

	// THE TWO ARE ACTUALLY APART. Asserted, not assumed -- this is what stops the clause from
	// being unfalsifiable on a symmetric board.
	if (!TestNotEqual(
			*FString::Printf(TEXT("the viewing side (%d) is not the side to move (%d), so the two spellings differ"),
				Model.ViewingSide, Model.Match.SideToMove),
			Model.ViewingSide, Model.Match.SideToMove))
	{
		return false;
	}

	const FStratSideView& ViewerRow     = Model.Sides[Model.ViewingSide];
	const FStratSideView& SideToMoveRow = Model.Sides[Model.Match.SideToMove];

	if (!TestNotEqual(
			*FString::Printf(
				TEXT("the two rows carry different purses (viewer %d, side to move %d), so returning the wrong one is visible"),
				ViewerRow.FameTotal, SideToMoveRow.FameTotal),
			ViewerRow.FameTotal, SideToMoveRow.FameTotal))
	{
		return false;
	}

	FStratSideView Out = PoisonRow();
	if (!TestTrue(TEXT("the viewing-side selector answers on a hand-over model"),
			UStratViewModelLibrary::GetViewingSideView(Model, Out)))
	{
		return false;
	}

	CompareSideRows(*this,
		FString::Printf(TEXT("the selector returned Model.Sides[ViewingSide=%d]"), Model.ViewingSide),
		Out, ViewerRow);

	TestNotEqual(
		*FString::Printf(
			TEXT("the selector did NOT return Model.Sides[SideToMove=%d]'s purse, which is the hand-over defect"),
			Model.Match.SideToMove),
		Out.FameTotal, SideToMoveRow.FameTotal);

	// THE INCOME LINE SPECIFICALLY, because it is what §2.11.2 draws as `+X/turn` and because
	// this selector is its only route to a widget. Compared against the model's own row, never
	// against a figure written here.
	TestEqual(
		*FString::Printf(TEXT("the `+X/turn` income line is the viewer's IncomePerTurn (%d)"),
			ViewerRow.IncomePerTurn),
		Out.IncomePerTurn, ViewerRow.IncomePerTurn);

	return true;
}
