// The gate on §2.11.2's ON-SCREEN COMMAND BAR -- `FStratCommandBarView` and its one producer,
// `StratDecorateCommandBar`. GDD §2.11.2, §2.11.5, §2.11.6-B. Acceptance ID T-UI-03.
//
// WHY THESE CLAUSES RIDE T-UI-03. The user ruled the acceptance ID for this pass. It is also the
// ID the subject belongs under on its own merits: T-UI-03 is "every number a widget draws equals
// exactly one `strat::UiSnapshot` field, no sums, no ratios, no derived values", and the whole
// reason `FStratCommandBarView` exists is that the two controls' conditions were folded in C++
// so that no widget graph spells `count == 0` or `suggested AND NOT gated`. Each clause below
// states its own stretch of that ID in its own name.
//
// WHAT THIS FILE'S SUBJECT IS, AND WHAT IT IS NOT. `StratDecorateCommandBar` is a free function
// over `FStratViewModel`. Nothing below constructs a widget, an actor or a world. The INPUT
// side of the decorator -- which hex the player focused, and when that focus is dropped -- is
// `FStratBuildAffordance`, which lives in StratPlay and is gated by
// `Source/StratPlay/Tests/StratBuildAffordanceClauses.cpp`. A clause about the latch put here
// would be `LNK2019`, not a test: StratUI is BELOW StratPlay and the arrow does not reverse.
//
// WHERE EVERY EXPECTATION COMES FROM. No hex, side index, owner, unit count or turn number is
// written down below. The board comes from `StratBuildViewModel` over the shipped scenario
// through a seeded `FStratBridge`; the factory under test is ENUMERATED out of `Model.Factories`
// and never named; the side indices come from `Model.Sides.Num()` and `Model.ViewingSide`; and
// the END TURN clauses compare against
// `UStratViewModelLibrary::CountViewingSideUnitsAbleToAct` -- the same selector the decorator
// itself calls, and the number §2.11.2's HUD already draws. The one thing asserted as a literal
// is a BOOLEAN answer, which is the contract stated on `FStratCommandBarView::bShowBuildButton`
// ("TRUE MEANS: the player has focused a hex, that hex holds a factory in `Factories`, and that
// factory's `Owner` equals `ViewingSide`") made checkable.
//
// THE SUGGESTION CLAUSE COMPARES AGAINST THE SELECTOR ON PURPOSE, AND THAT IS NOT CIRCULAR.
// `StratDecorateCommandBar` calls `CountViewingSideUnitsAbleToAct` and this file calls it too,
// so a defect INSIDE the selector moves both sides together and is invisible here -- stated
// rather than left to be discovered. That is the correct division: the selector's own four-clause
// derivation (side, `bDone`, `bLockedThisTurn`, `!(bHasMoved && bHasActed)`) is gated where it is
// authored, and what is pinned here is SINGLE AUTHORING -- that the bar did not grow a second
// walk of `Model.Units` that could drift from the number printed beside it. Re-deriving the count
// in this file is exactly what would destroy that property, and it is why the corpus below
// deliberately contains models on which the four clauses disagree.
//
// THE CASE-INSENSITIVITY TRAP. `FString::operator==`, `FString::Contains` and
// `FAutomationTestBase::TestEqual` are ALL case-insensitive in this engine, measured on this
// project. The one string this file compares is `FText EndTurnHoverText` against
// `FStratGuidanceView::EndTurnGateHover`, and it is compared with
// `FString::Equals(..., ESearchCase::CaseSensitive)` for that reason -- both sides come from the
// module, so the strict comparison costs nothing and closes the hole.
//
// NO `strat::` FREE FUNCTION IS CALLED BELOW. Naming `strat::UiSnapshot` costs no symbol;
// calling a vendored free function outside StratBridge / StratRules is `LNK2019`, measured 8x.
// Every rules-side value arrives through a method on `FStratBridge`.
//
// NOT ASSERTED HERE, WITH REASONS:
// - THE NEUTRAL-FACTORY HAZARD IS DELIBERATELY NOT PINNED. `StratDecorateCommandBar` records
//   that `Owner == ViewingSide` latches a NEUTRAL factory (`Owner == INDEX_NONE`) on a model
//   whose `ViewingSide` is ALSO `INDEX_NONE`, and that `StratBuildViewModel` range-checks
//   `ViewingSide` and refuses rather than producing one. A clause asserting that behaviour would
//   pin a HAZARD rather than a REQUIREMENT: it would go RED on its own fix the day someone
//   tightens the test, which is the wrong thing for a gate to do. What IS pinned below is the
//   requirement -- a neutral factory does NOT show the button on a model with a real viewing
//   side -- which is the case the game can actually reach.
// - THE VACUOUS ZERO-UNIT READING IS PINNED AS A CASE OF THE AGREEMENT CLAUSE AND NOT AS A
//   CLAUSE OF ITS OWN, for the same reason. `bEndTurnSuggested`'s own block names it: a model
//   carrying no units reads as suggested, and that is a consequence of `count == 0` rather than
//   a separate rule. It appears in the corpus below so the agreement is measured over it; naming
//   a clause after it would advertise a subject the decorator does not have.
// - PIXELS AND BINDINGS. Whether `WBP_CommandBar` binds visibility to one bool and makes no
//   conjunction is an asset property, and the editor lane's.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Internationalization/Text.h"
#include "Misc/Paths.h"
#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"

#include "StratViewModel.h"
#include "StratViewModelLibrary.h"

// The bridge, included from a .cpp and never from a UHT-parsed header -- the same constraint
// `StratViewModel.h` records about itself. A test file is a .cpp, so it may say this.
#include "StratBridge.h"

namespace StratCommandBarClauses
{
	/** MUST match the side the rest of the engine-side harness seeds with. A harness pin and
	 *  not a rule -- `Replay.h` states that no rule in this project decides who moves first,
	 *  and `StratViewModelParity.cpp` pins the same value for the same stated reason. */
	static const int32 kFirstSide = 0;

	/** The viewing side these clauses build for. NOT `sideToMove`. */
	static const int32 kViewingSide = 0;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/**
	 * Seeds a bridge and builds the shipped scenario's view model for `kViewingSide`.
	 *
	 * Returns false and fills `OutError` rather than asserting, on `StratScoreboardParity`'s
	 * precedent: a missing asset becomes a named failure inside the caller's own clause list
	 * instead of a crash in a helper. Nothing is hardcoded past the asset paths, which is one
	 * of the two places `.agents/ue-project-context.md` licenses a `/Game/` literal.
	 */
	static bool BuildShippedModel(FStratBridge& Bridge, FStratViewModel& OutModel, FString& OutError)
	{
		UDataTable* const Units   = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		UDataTable* const Terrain = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
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
		const FStratResult Seeded  = Bridge.LoadScenarioFromFile(ScenarioPath, kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}

		if (!StratBuildViewModel(Bridge, kViewingSide, OutModel, OutError))
		{
			OutError = FString::Printf(TEXT("StratBuildViewModel refused: %s"), *OutError);
			return false;
		}
		return true;
	}

	/**
	 * The index in `Model.Factories` of a factory `Side` holds.
	 *
	 * ENUMERATED FROM THE MODEL AND NEVER CHOSEN, on `StratProductionMenuSeam.cpp`'s rule: a
	 * clause that named a coordinate would go quiet the day the scenario moved it.
	 */
	static int32 IndexOfFactoryHeldBy(const FStratViewModel& Model, int32 Side)
	{
		for (int32 Index = 0; Index < Model.Factories.Num(); ++Index)
		{
			if (Model.Factories[Index].Owner == Side)
			{
				return Index;
			}
		}
		return INDEX_NONE;
	}

	/** A side index that exists on this model and is not `Model.ViewingSide`. Read off
	 *  `Model.Sides`, which `StratBuildViewModel` fills one entry per side. */
	static int32 SomeOtherSide(const FStratViewModel& Model)
	{
		for (int32 Side = 0; Side < Model.Sides.Num(); ++Side)
		{
			if (Side != Model.ViewingSide)
			{
				return Side;
			}
		}
		return INDEX_NONE;
	}

	/** `FText` equality that a casing defect can fail. See the header block. */
	static bool TextsAreIdentical(const FText& A, const FText& B)
	{
		return A.ToString().Equals(B.ToString(), ESearchCase::CaseSensitive);
	}
}

// ---------------------------------------------------------------------------------------------
// The BUILD control is RE-RESOLVED on every decorate, so a latch cannot go stale.
//
// THIS IS THE DESIGN'S LOAD-BEARING PROPERTY, and `StratBuildAffordance.h` spends its opening
// block on it: the struct holds a hex and a bool and NOT "is the button visible", so a factory
// captured out from under the latch, a hot-seat hand-over or a concluded match darkens the
// button on the next refresh "with no clear-point code involved, and staleness is structurally
// impossible rather than dependent on remembering every clear point".
//
// HOW IT IS MEASURED. ONE latch -- the same two arguments, unchanged, on every call below -- and
// the MODEL is mutated between decorates. If the decorator carried an answer in, or if a caller
// were expected to recompute the bool and pass it, the answers could not follow the mutations.
// Every mutation is of a field the decorator's own contract names (`Factories[i].Owner`,
// `Factories` membership, `ViewingSide`), and the model is restored between cases so each answer
// is attributable to one change.
//
// THE NEUTRAL CASE IS THE REQUIREMENT AND NOT THE HAZARD. `Owner = INDEX_NONE` on a model whose
// `ViewingSide` is a real side must not show the button. The hazard the decorator records --
// `ViewingSide` ALSO `INDEX_NONE` -- is deliberately not asserted; see this file's header block.
// ---------------------------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCommandBarBuildFollowsOwnershipTest,
	"Stratocracy.StratUI.T-UI-03.CommandBarBuildButtonFollowsFactoryOwnershipOnEveryDecorate",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCommandBarBuildFollowsOwnershipTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratCommandBarClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the shipped scenario seeds and builds a view model"),
			BuildShippedModel(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	const int32 OwnedIndex = IndexOfFactoryHeldBy(Model, Model.ViewingSide);
	if (!TestTrue(
			TEXT("the shipped scenario gives the viewing side a factory, so the positive case "
			     "below is real data rather than a fabricated one"),
			OwnedIndex != INDEX_NONE))
	{
		AddError(TEXT("no factory in the built model is owned by the viewing side; this clause ")
			TEXT("has no positive case to measure and must not pass"));
		return false;
	}

	const int32 OtherSide = SomeOtherSide(Model);
	if (!TestTrue(TEXT("the model carries a second side to hand the factory to"),
			OtherSide != INDEX_NONE))
	{
		return false;
	}

	// THE ONE LATCH. Neither of these two values changes anywhere below. Everything that moves
	// is a field of the model.
	const FIntPoint FocusedHex = Model.Factories[OwnedIndex].Hex;
	const int32     TrueOwner  = Model.Factories[OwnedIndex].Owner;

	AddInfo(FString::Printf(
		TEXT("latched factory: hex (%d, %d), owner %d, viewing side %d, %d sides on the model"),
		FocusedHex.X, FocusedHex.Y, TrueOwner, Model.ViewingSide, Model.Sides.Num()));

	// ---- (a) the factory this seat holds ---------------------------------------------------
	StratDecorateCommandBar(Model, /*bHasFocusedFactory*/ true, FocusedHex);
	TestTrue(TEXT("T-UI-03: a focus on a factory this seat owns shows the BUILD control"),
		Model.CommandBar.bShowBuildButton);
	TestEqual(TEXT("T-UI-03: and names the hex it was focused on"),
		Model.CommandBar.BuildFactoryHex, FocusedHex);

	// ---- (b) the same hex, captured by the enemy -------------------------------------------
	Model.Factories[OwnedIndex].Owner = OtherSide;
	StratDecorateCommandBar(Model, /*bHasFocusedFactory*/ true, FocusedHex);
	TestFalse(
		TEXT("T-UI-03: the SAME latch darkens the BUILD control once the factory changes hands "
		     "-- the decorator re-resolves ownership and does not carry an answer in"),
		Model.CommandBar.bShowBuildButton);
	TestEqual(
		TEXT("T-UI-03: and the hex field is written back to its meaningless value rather than "
		     "left holding the last factory the button was about"),
		Model.CommandBar.BuildFactoryHex, FIntPoint::ZeroValue);

	// ---- (c) the same hex, neutral ---------------------------------------------------------
	Model.Factories[OwnedIndex].Owner = INDEX_NONE;
	StratDecorateCommandBar(Model, /*bHasFocusedFactory*/ true, FocusedHex);
	TestFalse(
		TEXT("T-UI-03: a NEUTRAL factory shows no BUILD control on a model with a real viewing "
		     "side -- `Owner == ViewingSide` and not `Owner != INDEX_NONE`"),
		Model.CommandBar.bShowBuildButton);

	// ---- (d) the seat moves, the factory does not ------------------------------------------
	Model.Factories[OwnedIndex].Owner = TrueOwner;
	Model.ViewingSide                 = OtherSide;
	StratDecorateCommandBar(Model, /*bHasFocusedFactory*/ true, FocusedHex);
	TestFalse(
		TEXT("T-UI-03: the SAME latch darkens the BUILD control on the other seat's screen -- "
		     "the answer follows ViewingSide, which is what makes a hot-seat hand-over safe "
		     "without any clear-point code"),
		Model.CommandBar.bShowBuildButton);

	// ---- (e) the factory leaves the model entirely -----------------------------------------
	Model.ViewingSide = kViewingSide;
	const FStratFactoryView Removed = Model.Factories[OwnedIndex];
	Model.Factories.RemoveAt(OwnedIndex);
	StratDecorateCommandBar(Model, /*bHasFocusedFactory*/ true, FocusedHex);
	TestFalse(
		TEXT("T-UI-03: a focus on a hex the model no longer carries a factory for shows no "
		     "BUILD control, and is an answer rather than a refusal"),
		Model.CommandBar.bShowBuildButton);

	// ---- (f) restored, so (b)-(e) were the mutations and not a one-way latch ----------------
	Model.Factories.Insert(Removed, OwnedIndex);
	StratDecorateCommandBar(Model, /*bHasFocusedFactory*/ true, FocusedHex);
	TestTrue(
		TEXT("T-UI-03: restoring the model restores the BUILD control from the unchanged latch "
		     "-- so every answer above was the decorator re-resolving and not a latch that had "
		     "been spent"),
		Model.CommandBar.bShowBuildButton);

	// ---- (g) no focus at all ---------------------------------------------------------------
	//
	// THE HEX PASSED HERE IS THE REAL FACTORY'S. The bool must be read first, so a decorator
	// that resolved the hex before consulting the bool would show the button on a frame where
	// nothing is focused -- which is the trap `FIntPoint(0, 0)` is a real hex creates.
	StratDecorateCommandBar(Model, /*bHasFocusedFactory*/ false, FocusedHex);
	TestFalse(
		TEXT("T-UI-03: nothing focused shows no BUILD control, even when the hex handed in "
		     "names a factory this seat owns -- the bool is read first"),
		Model.CommandBar.bShowBuildButton);
	TestEqual(TEXT("T-UI-03: and the hex field carries the documented meaningless value"),
		Model.CommandBar.BuildFactoryHex, FIntPoint::ZeroValue);

	return true;
}

// ---------------------------------------------------------------------------------------------
// The §2.11.6-B gate beats the §2.11.2 highlight when both hold, and the `&&` is what makes it
// so.
//
// WHY THIS CLAUSE EXISTS AND MUST NOT BE DROPPED AS REDUNDANT. `FStratCommandBarView::
// bEndTurnSuggested` and `StratDecorateCommandBar` both record, in terms, that
// `!Guidance.bEndTurnGated` is anded in "ON PURPOSE AND MUST NOT BE DROPPED AS REDUNDANT", and
// both record why the redundancy argument is tempting and wrong: on the SHIPPED scenario the
// pair is unreachable, because beat 1a locks every unit but the marked Infantry and locked units
// do not count, so a gated turn always has one unit able to act. That is a fact about
// `FStratGuidedOpening::PublishLocks` and not a structural guarantee. This clause is what makes
// it structural -- it hand-builds the combination the shipped guidance cannot produce and pins
// the answer.
//
// THE POSITIVE CONTROL IS THE HALF THAT STOPS THIS BEING A TAUTOLOGY. The same model, ungated,
// must SUGGEST. Without it a decorator that never suggested anything would pass.
//
// THE COUNT IS ASSERTED ZERO ON BOTH RUNS, from the module's own selector, so the suppression
// is attributable to the gate rather than to a corpus that quietly stopped meeting the
// precondition.
// ---------------------------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCommandBarGateBeatsSuggestionTest,
	"Stratocracy.StratUI.T-UI-03.CommandBarGateBeatsSuggestionWhenBothHold",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCommandBarGateBeatsSuggestionTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratCommandBarClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the shipped scenario seeds and builds a view model"),
			BuildShippedModel(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	// EVERY UNIT OF THIS SEAT RETIRED. `bDone` is §2.11.1's retirement bit and is the first of
	// the selector's four clauses; the model built above leaves it false for every unit, which
	// is why the count is non-zero before this loop and zero after it.
	if (!TestTrue(TEXT("the built model carries units to retire"), Model.Units.Num() > 0))
	{
		return false;
	}
	const int32 CountBefore = UStratViewModelLibrary::CountViewingSideUnitsAbleToAct(Model);
	if (!TestTrue(
			TEXT("the seat starts with units able to act, so the zero below is this clause's "
			     "doing and not the scenario's"),
			CountBefore > 0))
	{
		return false;
	}
	for (FStratUnitView& Unit : Model.Units)
	{
		if (Unit.Side == Model.ViewingSide)
		{
			Unit.bDone = true;
		}
	}
	if (!TestEqual(
			TEXT("the module's own selector now counts nobody able to act for this seat"),
			UStratViewModelLibrary::CountViewingSideUnitsAbleToAct(Model), 0))
	{
		return false;
	}

	// ---- the positive control: not gated, so the highlight is on --------------------------
	Model.Guidance.bEndTurnGated = false;
	Model.Guidance.EndTurnGateHover = FText::GetEmpty();
	StratDecorateCommandBar(Model, /*bHasFocusedFactory*/ false, FIntPoint::ZeroValue);
	if (!TestTrue(
			TEXT("T-UI-03: with everyone finished and no gate, END TURN is suggested -- the "
			     "control this clause needs in order for its suppression half to mean anything"),
			Model.CommandBar.bEndTurnSuggested))
	{
		return false;
	}
	TestFalse(TEXT("T-UI-03: and is not dimmed"), Model.CommandBar.bEndTurnDimmed);

	// ---- the subject: gated AND everybody finished ----------------------------------------
	//
	// THE HOVER IS SET TO A SENTENCE OF THE GUIDANCE LAYER'S OWN, so the copy assertion below
	// compares two module values rather than this file's literal against a field.
	// THE PLANTED HOVER IS AN INPUT AND NOT AN EXPECTATION, and the distinction is the one
	// `StratRepairReceiptParity.cpp` draws about its planted row. The guidance layer that
	// authors this sentence in the running game is `FStratGuidedOpening::EndTurnGateHoverText`,
	// which lives in StratPlay and is unreachable from this module -- so the field is planted
	// here to give the copy something non-empty and distinctive to carry. The EXPECTATION side
	// of the assertion below is `Model.Guidance.EndTurnGateHover` read back off the model, never
	// this string, so a decorator that invented its own sentence fails whatever this says.
	Model.Guidance.bEndTurnGated    = true;
	Model.Guidance.EndTurnGateHover =
		FText::FromString(TEXT("Move the marked Infantry first."));

	TestEqual(
		TEXT("the count is still zero, so what changes below is the gate and nothing else"),
		UStratViewModelLibrary::CountViewingSideUnitsAbleToAct(Model), 0);

	StratDecorateCommandBar(Model, /*bHasFocusedFactory*/ false, FIntPoint::ZeroValue);

	TestFalse(
		TEXT("T-UI-03: §2.11.6-B's gate beats §2.11.2's highlight -- END TURN is NOT suggested "
		     "while it is gated, even with every unit finished. This is the `&&` that resolves "
		     "dim-beats-highlight in one place so no widget spells `suggested AND NOT gated`"),
		Model.CommandBar.bEndTurnSuggested);
	TestTrue(TEXT("T-UI-03: and it is dimmed"), Model.CommandBar.bEndTurnDimmed);
	TestTrue(
		TEXT("T-UI-03: the dim's explanation is a DECLARED COPY of "
		     "FStratGuidanceView::EndTurnGateHover, compared case-sensitively"),
		TextsAreIdentical(Model.CommandBar.EndTurnHoverText, Model.Guidance.EndTurnGateHover));

	// ---- and the two copies track the guidance block in both directions -------------------
	Model.Guidance.bEndTurnGated    = false;
	Model.Guidance.EndTurnGateHover = FText::GetEmpty();
	StratDecorateCommandBar(Model, /*bHasFocusedFactory*/ false, FIntPoint::ZeroValue);
	TestFalse(TEXT("T-UI-03: lifting the gate lifts the dim"), Model.CommandBar.bEndTurnDimmed);
	TestTrue(TEXT("T-UI-03: and empties the hover with it"),
		Model.CommandBar.EndTurnHoverText.IsEmpty());
	TestTrue(TEXT("T-UI-03: and the highlight comes back"),
		Model.CommandBar.bEndTurnSuggested);

	return true;
}

// ---------------------------------------------------------------------------------------------
// The END TURN highlight agrees with `CountViewingSideUnitsAbleToAct`, over a corpus the four
// clauses of that selector disagree on.
//
// WHAT IS PINNED: SINGLE AUTHORING. `StratDecorateCommandBar` records why it folds over the
// existing selector rather than walking `Model.Units` again -- "Re-deriving it here would be a
// second author of the same fact, and the highlight could then disagree with the number printed
// beside it." A second walk that got any of the four clauses wrong -- side, `bDone`,
// `bLockedThisTurn`, `!(bHasMoved && bHasActed)` -- would disagree with the selector on at least
// one model below.
//
// WHAT IS NOT PINNED, AND IT IS STATED RATHER THAN LEFT TO BE ASSUMED: whether the SELECTOR is
// right. Both sides of the comparison call it, so a defect inside it moves both together. Its
// own derivation is gated where it is authored. See this file's header block.
//
// THE CORPUS IS ASSERTED NON-VACUOUS. At least one model must suggest and at least one must not,
// or the whole clause could be satisfied by a decorator that answered one way always.
//
// THE ZERO-UNIT MODEL IS IN THE CORPUS BY NAME. `bEndTurnSuggested`'s own block records that a
// model carrying no units reads as vacuously suggested and that `StratBuildViewModel` cannot
// produce one. That is a consequence of `count == 0` rather than a rule of its own, so it is
// measured here as one case of the agreement and does not get a clause named after it.
// ---------------------------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCommandBarSuggestionAgreesWithCountTest,
	"Stratocracy.StratUI.T-UI-03.CommandBarSuggestionAgreesWithCountViewingSideUnitsAbleToAct",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCommandBarSuggestionAgreesWithCountTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratCommandBarClauses;

	FStratBridge    Bridge;
	FStratViewModel Base;
	FString         Error;
	if (!TestTrue(TEXT("the shipped scenario seeds and builds a view model"),
			BuildShippedModel(Bridge, Base, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the built model carries units"), Base.Units.Num() > 0))
	{
		return false;
	}

	// Six models, each a copy of the shipped one with one of the selector's four clauses driven
	// a different way. None of these is an expectation: each is an INPUT, and the expectation is
	// whatever the module's selector says about it.
	struct FCase
	{
		FString         Name;
		FStratViewModel Model;
	};

	TArray<FCase> Cases;

	{
		FCase Untouched;
		Untouched.Name  = TEXT("the shipped opening, nobody retired");
		Untouched.Model = Base;
		Cases.Add(MoveTemp(Untouched));
	}
	{
		FCase AllDone;
		AllDone.Name  = TEXT("every unit of this seat retired (bDone)");
		AllDone.Model = Base;
		for (FStratUnitView& Unit : AllDone.Model.Units)
		{
			if (Unit.Side == AllDone.Model.ViewingSide)
			{
				Unit.bDone = true;
			}
		}
		Cases.Add(MoveTemp(AllDone));
	}
	{
		FCase BothSpent;
		BothSpent.Name  = TEXT("every unit of this seat has both moved and acted");
		BothSpent.Model = Base;
		for (FStratUnitView& Unit : BothSpent.Model.Units)
		{
			if (Unit.Side == BothSpent.Model.ViewingSide)
			{
				Unit.bHasMoved = true;
				Unit.bHasActed = true;
			}
		}
		Cases.Add(MoveTemp(BothSpent));
	}
	{
		FCase HalfSpent;
		HalfSpent.Name  = TEXT("every unit of this seat has moved but NOT acted -- one allowance "
		                       "left, so the selector still counts them");
		HalfSpent.Model = Base;
		for (FStratUnitView& Unit : HalfSpent.Model.Units)
		{
			if (Unit.Side == HalfSpent.Model.ViewingSide)
			{
				Unit.bHasMoved = true;
				Unit.bHasActed = false;
			}
		}
		Cases.Add(MoveTemp(HalfSpent));
	}
	{
		FCase Locked;
		Locked.Name  = TEXT("every unit of this seat locked by §2.11.6-B beat 1a");
		Locked.Model = Base;
		for (FStratUnitView& Unit : Locked.Model.Units)
		{
			if (Unit.Side == Locked.Model.ViewingSide)
			{
				Unit.bLockedThisTurn = true;
			}
		}
		Cases.Add(MoveTemp(Locked));
	}
	{
		FCase Empty;
		Empty.Name  = TEXT("no units at all -- the vacuous reading bEndTurnSuggested's own block "
		                   "names, reachable only by a hand-built model");
		Empty.Model = Base;
		Empty.Model.Units.Reset();
		Cases.Add(MoveTemp(Empty));
	}
	{
		FCase OtherSeat;
		OtherSeat.Name  = TEXT("every unit of the OTHER seat retired, this seat's untouched -- "
		                       "the selector's side clause");
		OtherSeat.Model = Base;
		for (FStratUnitView& Unit : OtherSeat.Model.Units)
		{
			if (Unit.Side != OtherSeat.Model.ViewingSide)
			{
				Unit.bDone = true;
			}
		}
		Cases.Add(MoveTemp(OtherSeat));
	}

	int32 SuggestedCount = 0;
	int32 QuietCount     = 0;

	for (FCase& Case : Cases)
	{
		// Neither term the decorator ANDs in front of the count is in play on any case, so the
		// agreement below is about the count alone. Both are set explicitly rather than assumed
		// off the build, so a builder change cannot quietly make this corpus measure something
		// else.
		Case.Model.Match.bHasResult      = false;
		Case.Model.Guidance.bEndTurnGated = false;

		const int32 Count = UStratViewModelLibrary::CountViewingSideUnitsAbleToAct(Case.Model);

		StratDecorateCommandBar(Case.Model, /*bHasFocusedFactory*/ false, FIntPoint::ZeroValue);

		const bool bSuggested = Case.Model.CommandBar.bEndTurnSuggested;
		AddInfo(FString::Printf(TEXT("%s -> selector says %d able to act, bar says suggested %d"),
			*Case.Name, Count, bSuggested ? 1 : 0));

		TestEqual(
			*FString::Printf(
				TEXT("T-UI-03: the END TURN highlight agrees with "
				     "UStratViewModelLibrary::CountViewingSideUnitsAbleToAct on '%s' -- one "
				     "author for the count and the highlight beside it"),
				*Case.Name),
			bSuggested, Count == 0);

		if (bSuggested)
		{
			++SuggestedCount;
		}
		else
		{
			++QuietCount;
		}
	}

	// THE CORPUS IS NOT ONE-SIDED. Without this a decorator that always answered the same way
	// would satisfy every assertion above on a corpus that had drifted to one answer.
	TestTrue(TEXT("the corpus contains at least one model on which END TURN IS suggested"),
		SuggestedCount > 0);
	TestTrue(TEXT("and at least one on which it is NOT"), QuietCount > 0);

	// ---- and the concluded-match term, which breaks the agreement on purpose --------------
	//
	// §2.8: a finished match has nothing left to end. This is the one input on which the
	// highlight and the count deliberately DISAGREE, so it is measured outside the loop rather
	// than folded into it.
	FStratViewModel Concluded = Base;
	for (FStratUnitView& Unit : Concluded.Units)
	{
		if (Unit.Side == Concluded.ViewingSide)
		{
			Unit.bDone = true;
		}
	}
	Concluded.Guidance.bEndTurnGated = false;
	Concluded.Match.bHasResult       = true;
	TestEqual(TEXT("the concluded model still counts nobody able to act"),
		UStratViewModelLibrary::CountViewingSideUnitsAbleToAct(Concluded), 0);
	StratDecorateCommandBar(Concluded, /*bHasFocusedFactory*/ false, FIntPoint::ZeroValue);
	TestFalse(
		TEXT("T-UI-03: §2.8's concluded match suppresses the END TURN highlight even with the "
		     "count at zero -- nudging a player to end a turn on a final board would advertise "
		     "a control the §2.8 gate is guaranteed to refuse"),
		Concluded.CommandBar.bEndTurnSuggested);

	return true;
}

// ---------------------------------------------------------------------------------------------
// `StratBuildViewModel` leaves the command bar default-constructed.
//
// SIBLING TO `T-UI-03.BuildViewModelLeavesGuidanceDefaulted` in
// `Source/StratUI/Tests/StratViewModelParity.cpp`, and it exists for that clause's reason
// applied to a newer block: `FStratCommandBarView` is the block a future pass is most likely to
// try to fill in at build time, because the builder holds a seeded bridge and `Factories` is
// right there. Filling it there would give the block two producers, and the decorator's
// re-resolution -- the property the first clause in this file pins -- would then be racing a
// value written one layer down.
//
// THE EXPECTATION IS A DEFAULT-CONSTRUCTED `FStratCommandBarView`, COMPARED THROUGH THE
// REFLECTION SYSTEM. `UScriptStruct::CompareScriptStruct` walks every `UPROPERTY`, so a field
// ADDED to the block tomorrow is covered the day it lands without an edit here.
//
// AND THE NEGATIVE CONTROL IS THE HALF THAT MAKES THE COMPARISON MEAN SOMETHING. If
// `CompareScriptStruct` reported everything identical, the assertion above it would pass over a
// builder that filled the block completely. So a decorated model is compared against the same
// default and must DIFFER.
// ---------------------------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCommandBarBuilderLeavesItDefaultedTest,
	"Stratocracy.StratUI.T-UI-03.BuildViewModelLeavesCommandBarDefaulted",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCommandBarBuilderLeavesItDefaultedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratCommandBarClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the shipped scenario seeds and builds a view model"),
			BuildShippedModel(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	// The board is really there, so the emptiness below is the command bar's and not the
	// model's -- `BuildViewModelLeavesGuidanceDefaulted`'s own precaution.
	if (!TestTrue(TEXT("the built model carries the scenario's hexes"), Model.Hexes.Num() > 0))
	{
		return false;
	}
	if (!TestTrue(TEXT("and its factories, which is the data a builder would be tempted by"),
			Model.Factories.Num() > 0))
	{
		return false;
	}

	const UScriptStruct* const Struct = FStratCommandBarView::StaticStruct();
	if (!TestNotNull(TEXT("FStratCommandBarView has a reflected struct"), Struct))
	{
		return false;
	}

	const FStratCommandBarView Default;
	TestTrue(
		TEXT("T-UI-03: StratBuildViewModel leaves FStratCommandBarView default-constructed, "
		     "field for field, including fields this clause never names. Its producer is "
		     "StratDecorateCommandBar and there must be exactly one"),
		Struct->CompareScriptStruct(&Model.CommandBar, &Default, PPF_None));

	// ---- the three a reader will look for first, for the failure message ------------------
	TestFalse(TEXT("T-UI-03: the builder invents no BUILD control"),
		Model.CommandBar.bShowBuildButton);
	TestFalse(TEXT("T-UI-03: nor an END TURN highlight"), Model.CommandBar.bEndTurnSuggested);
	TestFalse(TEXT("T-UI-03: nor a dim"), Model.CommandBar.bEndTurnDimmed);

	// ---- the negative control for the comparison itself -----------------------------------
	const int32 OwnedIndex = IndexOfFactoryHeldBy(Model, Model.ViewingSide);
	if (!TestTrue(TEXT("the shipped scenario gives the viewing side a factory to decorate with"),
			OwnedIndex != INDEX_NONE))
	{
		return false;
	}
	FStratViewModel Decorated = Model;
	StratDecorateCommandBar(Decorated, /*bHasFocusedFactory*/ true,
		Decorated.Factories[OwnedIndex].Hex);
	TestFalse(
		TEXT("negative control: a DECORATED model's command bar differs from the default, so "
		     "the assertion above is not being satisfied by a comparison that cannot fail"),
		Struct->CompareScriptStruct(&Decorated.CommandBar, &Default, PPF_None));

	return true;
}
