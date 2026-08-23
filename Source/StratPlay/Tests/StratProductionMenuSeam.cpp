// The gate for §2.11.5's production-menu SEAM on `UStratMatchSubsystem` -- `GATE-BUILDMENU`.
//
// WHY `GATE-BUILDMENU` AND NOT AN ACCEPTANCE ROW. `T-UI-04` asserts that the production menu
// BINDS, which is an in-editor claim about a widget that does not exist yet; nothing below
// constructs a widget or touches Slate. `GATE-BUILDMENU` is the id this project already
// carries for §2.11.5 TRANSPORT -- `Source/StratBridge/Tests/StratBuildOptionRouting.cpp` and
// `Source/StratUI/Tests/StratProductionMenuRouting.cpp` are its first two files, and this is
// its third. `GATE-BRIDGE-DEFS` and `GATE-SAVE-PARSE` are the precedent for a gate with no
// acceptance row of its own.
//
// **[THE WIDGET NOW EXISTS, 2026-08-23: `Content/UI/WBP_ProductionMenu.uasset` is in the tree
// (untracked at the time of writing), so the words "that does not exist yet" above are stale.**
// EVERYTHING THE PARAGRAPH CONCLUDES IS UNCHANGED AND THE ID DOES NOT MOVE: `T-UI-04` is still
// an IN-EDITOR claim about BINDING, and this file still constructs no widget and touches no
// Slate, which is exactly what puts these clauses under `GATE-BUILDMENU` rather than under
// that row. What the asset's arrival changes is only what `T-UI-04` is now BLOCKED ON -- a
// human-driven playtest rather than a missing asset -- and `global.md` carries that. No clause
// in this file may be read as moving that row's ledger.]**
//
// WHAT THE THREE FILES DIVIDE BETWEEN THEM, and the division is forced by what each module
// can LINK against rather than chosen:
//   - StratBridge's file compares `FStratBridge::BuildOptions` against `strat::uiBuildOptions`.
//     It must CALL a vendored symbol, so it can only live inside StratBridge.
//   - StratUI's file compares `StratBuildProductionMenu` against `FStratBridge::BuildOptions`.
//   - THIS file is the only one that can see `UStratMatchSubsystem`, `AStratUnitActor` and
//     `FStratBridge` at once, which is what the seam's own properties need: a menu is state on
//     a subsystem, a submit is a rules command, and an accepted build must reach the board.
//     Written in either other module these clauses are `LNK2019`, not tests.
//
// WHERE EVERY EXPECTATION COMES FROM. Nothing below computes the answer side of a comparison
// that a module can be asked for:
//   - The rows are `StratBuildProductionMenu` called directly on the SAME bridge in the SAME
//     frame. Field-for-field and IN ORDER. The subsystem's copy is the subject; the free
//     function's answer is the expectation.
//   - Which hexes are factories, which side holds them, and whether each has built this turn
//     are `FStratViewModel::Factories`, read off `BuildViewModel` before and after.
//   - Which unit ids exist is `FStratViewModel::Units` after the build, compared against a
//     `TActorIterator` over the world.
//   - Whether a command was recorded is `FStratBridge::RecordedCommandCount()`.
//   - The side count used to construct a refusal is `FStratViewModel::Sides.Num()`.
// The only literals in this file are the two `/Game/StratData` fixture paths and the scenario
// path, all three of which are SUBJECTS and not expectations, under the standing
// automation-fixture exception to the no-`/Game/`-literals rule.
//
// A MEASURED CORRECTION TO THE SEAM'S OWN DOCUMENTATION, recorded here because a clause was
// asked for that the tree does not support -- AND SINCE ACTED ON AT THE SOURCE, so a reader
// following the citation now finds agreement rather than the contradiction this block was
// written to expose. Kept, in the past tense, because the false sentence is the reason
// `ANonFactoryHexOpensAnUnavailableMenuAndIsNotRefused` below exists at all.
//
// WHAT WAS MEASURED. `UStratMatchSubsystem::RefreshProductionMenu`'s declaration USED TO
// state "A hex that is not a factory is REFUSED, in the bridge's own words." THAT WAS FALSE,
// and it was false one layer down rather than here: `FStratBridge::BuildOptions` sends
// "whether it is an objective at all, whether this side holds it, whether it is a build
// point" out on `available` and `reason` and NEVER on the refusal channel -- its own comment
// says so, and `GATE-BUILDMENU.AnswersRideTheOkChannel` in StratBridge already pins it. So a
// non-factory hex OPENS a menu here, with every row unavailable, and `IsProductionMenuOpen()`
// goes true for a hex that is not a factory. That half is unchanged and is what the clause
// below pins; it is a fact about the CODE, and the code did not move.
//
// WHERE THE WITHDRAWAL NOW LIVES, so nobody re-derives this. `RefreshProductionMenu`'s block
// now says "SO A NON-FACTORY HEX OPENS A MENU RATHER THAN REFUSING ONE" and carries the old
// sentence beneath it under this project's `RETRACTED> ` convention, retracted in place and
// not deleted. `UStratMatchSubsystem::SubmitProductionChoice`'s `Bridge.Get() == nullptr` arm
// was corrected in the same pass. The claim's ORIGIN, `Tools/architect/state/global.md`, was
// withdrawn there. The declaration and the code AGREE as of 2026-08-22; before that date they
// did not, and this file is where the disagreement was filed rather than silently split.
//
// AND THE CONSEQUENCE FOR THE ALL-OR-NOTHING CLAUSE. Because of the above, a refused refresh
// cannot be produced by passing a bad HEX at all. The only refusal reachable on a live match
// is a viewing side outside the snapshot's sides, so that is what
// `ARefusedRefreshLeavesTheOpenMenuAndItsHexIntact` uses -- deliberately, and the side is
// taken from the model rather than written down.
//
// NO `strat::` FREE FUNCTION IS CALLED ANYWHERE BELOW. That is `LNK2019` outside StratBridge
// and StratRules, measured 8x. `StratBridge.h` is included from this `.cpp` and from no
// header.
//
// SLOT HYGIENE. One clause runs a real AI turn, and this suite's standing rule is that any
// fixture that plays a match is a potential writer to whatever slot its config names. The
// config below names a slot of this file's own and that clause deletes it on both ends;
// `FStratMatchConfig::bRecordCompletionOnMatchEnd` is left at its C++ default, which is
// false, and is deliberately not written down here.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

// Included from a .cpp and never from a UHT-parsed header.
#include "StratBridge.h"

namespace StratProductionMenuSeam
{
	/** This file's own slot, shared with nothing. See the header block on slot hygiene. */
	static const TCHAR* const kSlotName = TEXT("StratocracyAutomation_ProductionMenuSeam");

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	static void DeleteOurSlot()
	{
		if (UGameplayStatics::DoesSaveGameExist(kSlotName, /*UserIndex*/ 0))
		{
			UGameplayStatics::DeleteGameInSlot(kSlotName, /*UserIndex*/ 0);
		}
	}

	/**
	 * The match configuration a live match needs.
	 *
	 * SIDE 0 IS BOTH `FirstSide` AND `ViewingSide`, because §2.11.5's menu is asked for the
	 * side whose screen it is and this file's whole subject is a build that side may make.
	 * `Data/ferrum_crossing.json` gives side 0 exactly one factory it holds; nothing below
	 * names that hex, and every clause reads the factory set off the view model.
	 *
	 * `bRecordCompletionOnMatchEnd` IS NOT SET, so it is the C++ default. The one clause that
	 * plays a turn must not be able to record a completed match on a developer's machine.
	 */
	static bool MakeConfig(FStratMatchConfig& Out, FString& OutError)
	{
		Out.UnitTable    = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		Out.TerrainTable = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
		if (Out.UnitTable == nullptr || Out.TerrainTable == nullptr)
		{
			OutError = TEXT("DT_Units and/or DT_Terrain did not load from /Game/StratData");
			return false;
		}

		Out.ScenarioFile    = TEXT("Data/ferrum_crossing.json");
		Out.FirstSide       = 0;
		Out.ViewingSide     = 0;
		Out.BoardActorClass = AStratBoardActor::StaticClass();
		Out.UnitActorClass  = AStratUnitActor::StaticClass();
		Out.SaveSlotName    = kSlotName;
		return true;
	}

	/** The transient world, with the engine context teardown wants. Same shape and same
	 *  destruction order as `StratMatchReconcile.cpp`'s, for the reason recorded there. */
	struct FTestWorldScope
	{
		UWorld* World = nullptr;

		FTestWorldScope()
		{
			World = UWorld::CreateWorld(EWorldType::Game, /*bInformEngineOfWorld*/ false);
			if (World != nullptr && GEngine != nullptr)
			{
				FWorldContext& Context = GEngine->CreateNewWorldContext(EWorldType::Game);
				Context.SetCurrentWorld(World);
			}
		}

		~FTestWorldScope()
		{
			if (World != nullptr)
			{
				World->DestroyWorld(false);
				if (GEngine != nullptr)
				{
					GEngine->DestroyWorldContext(World);
				}
				World = nullptr;
			}
		}

		FTestWorldScope(const FTestWorldScope&) = delete;
		FTestWorldScope& operator=(const FTestWorldScope&) = delete;
	};

	/**
	 * A live match in a transient world.
	 *
	 * `StartMatch` RETURNING FALSE IS EXPECTED AND IS NOT AN ERROR HERE. This fixture has no
	 * tile meshes, so the board complains and the presentation half fails while the rules
	 * half succeeds -- the split `StratMatchSubsystem.h` states in as many words and
	 * `StratMatchReconcile.cpp` pins. Every clause below asserts `IsMatchLive()` instead,
	 * and declares the board's warning expected.
	 */
	struct FSeamHarness
	{
		FTestWorldScope        Scope;
		UStratMatchSubsystem*  Subsystem = nullptr;
		FStratBridge*          Bridge    = nullptr;
		FString                StartReason;

		bool Arm(FAutomationTestBase& Test, const FStratMatchConfig& Config)
		{
			if (!Test.TestNotNull(TEXT("a transient world was created"), Scope.World))
			{
				return false;
			}
			Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
			if (!Test.TestNotNull(TEXT("the world has a match subsystem"), Subsystem))
			{
				return false;
			}
			Subsystem->StartMatch(Config, StartReason);
			if (!Test.TestTrue(TEXT("the match is live whatever StartMatch returned"),
					Subsystem->IsMatchLive()))
			{
				Test.AddError(StartReason);
				return false;
			}
			Bridge = Subsystem->GetBridge();
			if (!Test.TestNotNull(TEXT("a live match owns a bridge"), Bridge))
			{
				Test.AddError(StartReason);
				return false;
			}
			return true;
		}
	};

	/** The board's own sentence about a fixture carrying no Blueprint defaults. Declared so
	 *  the line stays in the log for a human. NOTE: `Occurrences 0` means "at least one",
	 *  not "zero or more" -- measured on this suite, so only ever declare a line that fires. */
	static void ExpectTheTileMeshWarning(FAutomationTestBase& Test)
	{
		Test.AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
			EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
	}

	/**
	 * The hex of the one factory this side holds, plus every other factory hex.
	 *
	 * ENUMERATED FROM THE MODEL AND NEVER CHOSEN. A clause that named `(1, 4)` would go quiet
	 * the day the scenario moved it; a clause that reads `Factories` moves with the data. If
	 * the shipped scenario ever stops giving the viewing side a factory, `bFound` is false
	 * and the caller fails loudly naming the missing case rather than passing vacuously.
	 */
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

	/** The first hex on the board that no factory claims. Same derivation principle. */
	static bool FindNonFactoryHex(const FStratViewModel& Model, FIntPoint& OutHex)
	{
		for (const FStratHexView& Hex : Model.Hexes)
		{
			bool bIsFactory = false;
			for (const FStratFactoryView& Factory : Model.Factories)
			{
				if (Factory.Hex == Hex.Hex)
				{
					bIsFactory = true;
					break;
				}
			}
			if (!bIsFactory)
			{
				OutHex = Hex.Hex;
				return true;
			}
		}
		return false;
	}

	/**
	 * Field-for-field, in order.
	 *
	 * THE REASON STRING IS COMPARED CASE-SENSITIVELY. `FString::operator==`, `Contains` and
	 * `FAutomationTestBase::TestEqual` are all case-INSENSITIVE in this engine, measured on
	 * this project, and a parity clause written the obvious way therefore cannot fail on a
	 * casing defect. Both sides here come from the module, so the strict comparison costs
	 * nothing and closes the hole.
	 */
	static bool RowsMatch(const TArray<FStratBuildOptionView>& A,
	                      const TArray<FStratBuildOptionView>& B,
	                      FString&                             OutWhy)
	{
		if (A.Num() != B.Num())
		{
			OutWhy = FString::Printf(TEXT("row counts differ: %d vs %d"), A.Num(), B.Num());
			return false;
		}
		for (int32 Index = 0; Index < A.Num(); ++Index)
		{
			const FStratBuildOptionView& L = A[Index];
			const FStratBuildOptionView& R = B[Index];
			if (L.DefIndex != R.DefIndex || L.Id != R.Id || L.CostFame != R.CostFame ||
				L.bAffordable != R.bAffordable || L.bAvailable != R.bAvailable ||
				!L.Reason.ToString().Equals(R.Reason.ToString(), ESearchCase::CaseSensitive))
			{
				OutWhy = FString::Printf(
					TEXT("row %d differs: (%d, %s, %d, aff %d, avail %d, '%s') vs ")
					TEXT("(%d, %s, %d, aff %d, avail %d, '%s')"),
					Index, L.DefIndex, *L.Id.ToString(), L.CostFame, L.bAffordable ? 1 : 0,
					L.bAvailable ? 1 : 0, *L.Reason.ToString(),
					R.DefIndex, *R.Id.ToString(), R.CostFame, R.bAffordable ? 1 : 0,
					R.bAvailable ? 1 : 0, *R.Reason.ToString());
				return false;
			}
		}
		return true;
	}

	/** A `DefIndex` no row of the menu carries, derived from the menu rather than invented.
	 *  One past the largest offered index is outside `t.units->size()` too, so if it ever
	 *  DID reach the rules module the module would refuse it as well -- which is why the
	 *  clause that uses it measures `RecordedCommandCount()` and not merely a `false`. */
	static int32 AnUnofferedDefIndex(const TArray<FStratBuildOptionView>& Rows)
	{
		int32 Highest = INDEX_NONE;
		for (const FStratBuildOptionView& Row : Rows)
		{
			Highest = FMath::Max(Highest, Row.DefIndex);
		}
		return Highest + 1;
	}

	/** The first row the module says this side can both pay for and place. */
	static bool FindABuildableRow(const TArray<FStratBuildOptionView>& Rows, int32& OutDefIndex)
	{
		for (const FStratBuildOptionView& Row : Rows)
		{
			if (Row.bAffordable && Row.bAvailable)
			{
				OutDefIndex = Row.DefIndex;
				return true;
			}
		}
		return false;
	}

	/** Every `AStratUnitActor` actually alive in the world, by the id it stands for. Read from
	 *  the WORLD and not from the subsystem's map, for `StratMatchReconcile.cpp`'s reason: an
	 *  actor the map forgot about is invisible from the inside. */
	static TSet<int32> LiveUnitIds(UWorld* World)
	{
		TSet<int32> Ids;
		if (World == nullptr)
		{
			return Ids;
		}
		for (TActorIterator<AStratUnitActor> It(World); It; ++It)
		{
			AStratUnitActor* const Actor = *It;
			if (IsValid(Actor) && !Actor->IsActorBeingDestroyed())
			{
				Ids.Add(Actor->GetUnitId());
			}
		}
		return Ids;
	}

	static FString Describe(const TSet<int32>& Ids)
	{
		TArray<int32> Sorted = Ids.Array();
		Sorted.Sort();
		TArray<FString> Parts;
		for (const int32 Id : Sorted)
		{
			Parts.Add(FString::FromInt(Id));
		}
		return FString::Join(Parts, TEXT(", "));
	}

	/** `bHasBuiltThisTurn` per factory hex, off the model. The expectation side of the
	 *  "which factory did the build land at" comparison, and it is the rules module's. */
	static TMap<FIntPoint, bool> BuiltFlagsByHex(const FStratViewModel& Model)
	{
		TMap<FIntPoint, bool> Out;
		for (const FStratFactoryView& Factory : Model.Factories)
		{
			Out.Add(Factory.Hex, Factory.bHasBuiltThisTurn);
		}
		return Out;
	}
}

// ---------------------------------------------------------------------------
// A refresh publishes the bridge's rows, in the bridge's order, at the hex it was asked for.
//
// THE EXPECTATION IS `StratBuildProductionMenu` ON THE SAME BRIDGE IN THE SAME FRAME -- the
// exact call `RefreshProductionMenu` makes. This clause is therefore about TRANSPORT and
// about nothing else: that the subsystem publishes the builder's answer whole, unsorted,
// unfiltered, and pairs it with the hex the caller named.
//
// ORDER IS PART OF THE CLAIM AND IS NOT DECORATION. `DefIndex` is a raw, bounds-checked-only
// index into the definitions vector, so a menu re-ordered between the builder and the widget
// would submit a DIFFERENT UNIT TYPE with a green build -- the same property that makes
// `DT_Units` row order load-bearing. `RowsMatch` walks the two arrays positionally.
//
// THE "NOT OPEN YET" ASSERTION IS AN ASSERTION AND NOT A PROBE. It is taken after `StartMatch`
// on a subsystem this clause created, where `IsProductionMenuOpen()` false is the required
// post-condition of a fresh match and not a state the clause would repair if it found it
// otherwise.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuRefreshPublishesTheBridgesRowsTest,
	"Stratocracy.StratPlay.GATE-BUILDMENU.RefreshPublishesTheBridgesRowsInOrderAtTheHexAsked",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratProductionMenuRefreshPublishesTheBridgesRowsTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuSeam;

	ExpectTheTileMeshWarning(*this);

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	FSeamHarness H;
	if (!H.Arm(*this, Config))
	{
		return false;
	}

	TestFalse(TEXT("a fresh match has no production menu open"), H.Subsystem->IsProductionMenuOpen());
	TestEqual(TEXT("a fresh match publishes no rows"), H.Subsystem->ProductionMenu.Num(), 0);

	FStratViewModel Model;
	FString         BuildError;
	if (!TestTrue(TEXT("the view model builds"), H.Subsystem->BuildViewModel(Model, BuildError)))
	{
		AddError(BuildError);
		return false;
	}

	FIntPoint FactoryHex;
	if (!TestTrue(TEXT("the shipped scenario gives the viewing side a factory to shop at"),
			FindHeldFactory(Model, Model.ViewingSide, FactoryHex)))
	{
		AddError(TEXT("no factory in the view model is owned by the viewing side; this clause ")
			TEXT("has no case to measure and must not pass"));
		return false;
	}

	FString RefreshReason;
	if (!TestTrue(*FString::Printf(TEXT("the menu opens at the held factory (%d, %d)"),
				FactoryHex.X, FactoryHex.Y),
			H.Subsystem->RefreshProductionMenu(FactoryHex, RefreshReason)))
	{
		AddError(RefreshReason);
		return false;
	}

	TestTrue(TEXT("the menu reports itself open"), H.Subsystem->IsProductionMenuOpen());
	TestEqual(TEXT("the published hex is the hex that was asked for"),
		H.Subsystem->ProductionMenuHex, FactoryHex);
	TestTrue(TEXT("a successful refresh clears the failure reason"), RefreshReason.IsEmpty());

	// The expectation, asked of the module directly.
	TArray<FStratBuildOptionView> Expected;
	FString                       ExpectedReason;
	if (!TestTrue(TEXT("the builder answers the same question directly"),
			StratBuildProductionMenu(*H.Bridge, Model.ViewingSide, FactoryHex, Expected,
				ExpectedReason)))
	{
		AddError(ExpectedReason);
		return false;
	}

	// A DEFAULT CANNOT SIGNAL "COPIED". `FStratBuildOptionView::DefIndex` initialises to
	// `INDEX_NONE` on both sides, so a bare parity check could not tell "copied" from "both
	// untouched". Establish that the module produced real §2.4 row indices first.
	if (!TestTrue(TEXT("the builder returned at least one row"), Expected.Num() > 0))
	{
		return false;
	}
	for (int32 Index = 0; Index < Expected.Num(); ++Index)
	{
		TestTrue(*FString::Printf(TEXT("expected row %d carries a real definition index"), Index),
			Expected[Index].DefIndex >= 0);
	}

	FString Why;
	if (!TestTrue(TEXT("the published menu is the builder's rows, field for field and in order"),
			RowsMatch(H.Subsystem->ProductionMenu, Expected, Why)))
	{
		AddError(Why);
	}

	return true;
}

// ---------------------------------------------------------------------------
// A hex that is not a factory OPENS an unavailable menu. It is NOT refused.
//
// THIS CLAUSE PINS WHAT THE CODE DOES, AND IT ONCE CONTRADICTED WHAT THE DECLARATION SAID.
// `RefreshProductionMenu`'s block USED TO state "A hex that is not a factory is REFUSED, in
// the bridge's own words"; `FStratBridge::BuildOptions` sends exactly that question out on
// `available` instead, its own comment says so, and StratBridge's `AnswersRideTheOkChannel`
// already pins it one layer down. THE DECISION WAS TAKEN IN FAVOUR OF THE CODE on 2026-08-22:
// the declaration now says a non-factory hex opens a menu, and carries the withdrawn sentence
// beneath it under `RETRACTED> `. This clause did not change and did not need to -- it was
// written against the code and the code is what the documentation moved to meet. It remains
// what makes the answer visible: a change that starts refusing turns it red, on purpose, and
// the redness is a decision arriving rather than a regression.
//
// WHAT IT MEASURES, so a reader is not left with only the contradiction: the seam does not
// invent an empty menu for a hex the rules will not build at, and `IsProductionMenuOpen()`
// therefore answers TRUE for a hex that is not a factory. A widget that treats "the menu is
// open" as "this is a factory" is wrong today, and this file is where that is written down.
//
// THE COVERAGE ASSERTION IS THE POINT OF THE SECOND HALF. "Every row unavailable" would be
// satisfied by a board on which every question answered no. The clause therefore requires the
// SAME side and the SAME frame to produce an AVAILABLE menu at its held factory, so the
// unavailability measured here is attributable to the hex.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuNonFactoryHexTest,
	"Stratocracy.StratPlay.GATE-BUILDMENU.ANonFactoryHexOpensAnUnavailableMenuAndIsNotRefused",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratProductionMenuNonFactoryHexTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuSeam;

	ExpectTheTileMeshWarning(*this);

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	FSeamHarness H;
	if (!H.Arm(*this, Config))
	{
		return false;
	}

	FStratViewModel Model;
	FString         BuildError;
	if (!TestTrue(TEXT("the view model builds"), H.Subsystem->BuildViewModel(Model, BuildError)))
	{
		AddError(BuildError);
		return false;
	}

	FIntPoint PlainHex;
	if (!TestTrue(TEXT("the board has a hex no factory claims"), FindNonFactoryHex(Model, PlainHex)))
	{
		AddError(TEXT("every hex on the board is a factory; this clause has no case to measure"));
		return false;
	}

	FString Reason;
	if (!TestTrue(*FString::Printf(
				TEXT("a non-factory hex (%d, %d) is ANSWERED, not refused"), PlainHex.X, PlainHex.Y),
			H.Subsystem->RefreshProductionMenu(PlainHex, Reason)))
	{
		AddError(FString::Printf(
			TEXT("it was refused with '%s'. If that is now the intended contract, this clause is ")
			TEXT("the place the change is declared -- see this file's header block, and note that ")
			TEXT("StratBridge's AnswersRideTheOkChannel pins the opposite one layer down."),
			*Reason));
		return false;
	}

	TestTrue(TEXT("so 'the menu is open' does NOT mean 'this hex is a factory'"),
		H.Subsystem->IsProductionMenuOpen());
	TestEqual(TEXT("the non-factory hex is published beside the rows"),
		H.Subsystem->ProductionMenuHex, PlainHex);
	TestTrue(TEXT("the answer is a full menu and never an empty one"),
		H.Subsystem->ProductionMenu.Num() > 0);

	for (const FStratBuildOptionView& Row : H.Subsystem->ProductionMenu)
	{
		TestFalse(*FString::Printf(TEXT("row '%s' is unavailable at a non-factory hex"),
				*Row.Id.ToString()),
			Row.bAvailable);
	}

	// THE COVERAGE CONTROL, IN THIS CLAUSE AND NOT IN ANOTHER ONE. An absence needs its
	// control in the same fixture or the control is an assumption about a different clause's
	// health.
	FIntPoint FactoryHex;
	if (!TestTrue(TEXT("the viewing side holds a factory to take the control at"),
			FindHeldFactory(Model, Model.ViewingSide, FactoryHex)))
	{
		AddError(TEXT("no held factory: the unavailability above cannot be attributed to the hex"));
		return false;
	}

	FString ControlReason;
	if (!TestTrue(TEXT("the same side in the same frame opens a menu at its held factory"),
			H.Subsystem->RefreshProductionMenu(FactoryHex, ControlReason)))
	{
		AddError(ControlReason);
		return false;
	}

	bool bAnyAvailable = false;
	for (const FStratBuildOptionView& Row : H.Subsystem->ProductionMenu)
	{
		bAnyAvailable = bAnyAvailable || Row.bAvailable;
	}
	if (!TestTrue(TEXT("CONTROL: at the held factory at least one row IS available, so the "
			"unavailability above is a fact about the hex and not about the board"), bAnyAvailable))
	{
		AddError(TEXT("the board supplied no available row anywhere this frame; the measurement ")
			TEXT("above is vacuous and this clause is reporting that rather than passing"));
	}

	return true;
}

// ---------------------------------------------------------------------------
// A refused refresh leaves the open menu AND its hex exactly as it found them.
//
// ALL-OR-NOTHING ACROSS TWO MEMBERS, which is the part `StratBuildProductionMenu`'s own
// all-or-nothing contract does not cover: the builder owns the array, the SUBSYSTEM owns the
// hex beside it, and a hex written before the call could move on a call that produced no rows.
//
// THE REFUSAL IS MANUFACTURED FROM THE VIEWING SIDE AND NOT FROM THE HEX, and that is forced
// rather than chosen -- see this file's header block. `SetViewingSide` is deliberately not
// range-checked and deliberately does not roll back, both stated in its own declaration, so
// it is the one lever that makes `FStratBridge::BuildOptions` refuse on a live match. THE
// OUT-OF-RANGE SIDE IS `Model.Sides.Num()`, read off the module; no side count is written
// down in this file.
//
// `SetViewingSide` RETURNING FALSE IS EXPECTED here and is reported, never asserted: it fails
// at the presentation rebuild for exactly the reason this clause wants, and asserting its
// return would be asserting the rebuild.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuRefusedRefreshIsAllOrNothingTest,
	"Stratocracy.StratPlay.GATE-BUILDMENU.ARefusedRefreshLeavesTheOpenMenuAndItsHexIntact",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratProductionMenuRefusedRefreshIsAllOrNothingTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuSeam;

	ExpectTheTileMeshWarning(*this);

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	FSeamHarness H;
	if (!H.Arm(*this, Config))
	{
		return false;
	}

	FStratViewModel Model;
	FString         BuildError;
	if (!TestTrue(TEXT("the view model builds"), H.Subsystem->BuildViewModel(Model, BuildError)))
	{
		AddError(BuildError);
		return false;
	}

	FIntPoint FactoryHex;
	if (!TestTrue(TEXT("the viewing side holds a factory"),
			FindHeldFactory(Model, Model.ViewingSide, FactoryHex)))
	{
		AddError(TEXT("no held factory; this clause has no menu to protect"));
		return false;
	}

	FString OpenReason;
	if (!TestTrue(TEXT("a correct menu is open before anything is refused"),
			H.Subsystem->RefreshProductionMenu(FactoryHex, OpenReason)))
	{
		AddError(OpenReason);
		return false;
	}

	const TArray<FStratBuildOptionView> RowsBefore = H.Subsystem->ProductionMenu;
	const FIntPoint                     HexBefore  = H.Subsystem->ProductionMenuHex;
	if (!TestTrue(TEXT("the menu being protected is non-empty"), RowsBefore.Num() > 0))
	{
		return false;
	}

	// The one lever that makes the builder refuse on a live match. The side comes from the
	// model's own side list.
	const int32 OutOfRangeSide = Model.Sides.Num();
	FString     SideReason;
	const bool  bSideAccepted = H.Subsystem->SetViewingSide(OutOfRangeSide, SideReason);

	FIntPoint OtherHex = FactoryHex;
	for (const FStratFactoryView& Factory : Model.Factories)
	{
		if (Factory.Hex != FactoryHex)
		{
			OtherHex = Factory.Hex;
			break;
		}
	}

	FString RefusedReason;
	const bool bRefreshed = H.Subsystem->RefreshProductionMenu(OtherHex, RefusedReason);

	// Restore before asserting, so a failing assertion cannot leave the subsystem in a state
	// the teardown would complain about.
	FString RestoreReason;
	H.Subsystem->SetViewingSide(Model.ViewingSide, RestoreReason);

	if (!TestFalse(*FString::Printf(
				TEXT("a refresh for side %d is refused (SetViewingSide returned %d: '%s')"),
				OutOfRangeSide, bSideAccepted ? 1 : 0, *SideReason),
			bRefreshed))
	{
		AddError(TEXT("no refusal was produced, so the all-or-nothing property below was never ")
			TEXT("exercised and this clause measured nothing"));
		return false;
	}

	TestFalse(TEXT("the refusal carries the refusing layer's own words"), RefusedReason.IsEmpty());

	TestTrue(TEXT("the menu is still open after the refusal"), H.Subsystem->IsProductionMenuOpen());
	TestEqual(TEXT("the published hex did not move to the refused hex"),
		H.Subsystem->ProductionMenuHex, HexBefore);

	FString Why;
	if (!TestTrue(TEXT("every row is exactly as the refusal found it"),
			RowsMatch(H.Subsystem->ProductionMenu, RowsBefore, Why)))
	{
		AddError(Why);
	}

	return true;
}

// ---------------------------------------------------------------------------
// A `DefIndex` the open menu never offered is refused HERE, without reaching the rules.
//
// `RecordedCommandCount()` IS THE CONTROL AND IT IS WHAT MAKES THIS A CLAUSE RATHER THAN AN
// OBSERVATION OF A `false`. One past the largest offered index is also outside the definitions
// vector, so the rules module would refuse it too -- a clause that only read the return value
// would pass identically whether the guard exists or the module caught it downstream. The
// count moving is the difference between "the seam declined to ask" and "the seam asked and
// was told no", and the seam's declaration claims the first.
//
// THE INDEX IS DERIVED FROM THE MENU, never written down, so a §2.4 table that grows a row
// moves it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuUnofferedIndexTest,
	"Stratocracy.StratPlay.GATE-BUILDMENU.AnUnofferedDefIndexIsRefusedWithoutReachingTheRules",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratProductionMenuUnofferedIndexTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuSeam;

	ExpectTheTileMeshWarning(*this);

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	FSeamHarness H;
	if (!H.Arm(*this, Config))
	{
		return false;
	}

	FStratViewModel Model;
	FString         BuildError;
	if (!TestTrue(TEXT("the view model builds"), H.Subsystem->BuildViewModel(Model, BuildError)))
	{
		AddError(BuildError);
		return false;
	}

	FIntPoint FactoryHex;
	if (!TestTrue(TEXT("the viewing side holds a factory"),
			FindHeldFactory(Model, Model.ViewingSide, FactoryHex)))
	{
		AddError(TEXT("no held factory; there is no menu to pick an unoffered row from"));
		return false;
	}

	FString OpenReason;
	if (!TestTrue(TEXT("a menu is open to be picked from"),
			H.Subsystem->RefreshProductionMenu(FactoryHex, OpenReason)))
	{
		AddError(OpenReason);
		return false;
	}

	const TArray<FStratBuildOptionView> RowsBefore  = H.Subsystem->ProductionMenu;
	const int32                         Unoffered   = AnUnofferedDefIndex(RowsBefore);
	const int32                         CountBefore = H.Bridge->RecordedCommandCount();

	// The premise: the index really is absent from the menu. Established, not assumed.
	for (const FStratBuildOptionView& Row : RowsBefore)
	{
		TestNotEqual(TEXT("the chosen index is absent from every offered row"),
			Row.DefIndex, Unoffered);
	}

	FString SubmitReason;
	TestFalse(*FString::Printf(TEXT("definition index %d is refused"), Unoffered),
		H.Subsystem->SubmitProductionChoice(Unoffered, SubmitReason));
	TestFalse(TEXT("the refusal says something"), SubmitReason.IsEmpty());

	// THE CONTROL.
	TestEqual(TEXT("no command reached the rules module"),
		H.Bridge->RecordedCommandCount(), CountBefore);

	// AND NOTHING MOVED ON SCREEN EITHER: the declaration says "no command, no menu change,
	// no redraw".
	TestTrue(TEXT("the menu is still open"), H.Subsystem->IsProductionMenuOpen());
	TestEqual(TEXT("the menu's hex is unchanged"), H.Subsystem->ProductionMenuHex, FactoryHex);
	FString Why;
	if (!TestTrue(TEXT("the rows are unchanged"),
			RowsMatch(H.Subsystem->ProductionMenu, RowsBefore, Why)))
	{
		AddError(Why);
	}

	return true;
}

// ---------------------------------------------------------------------------
// A submit with no menu open is refused, and the refusal is attributable to the CLOSED MENU.
//
// THE ROW SUBMITTED IS ONE THE MENU DID OFFER, captured while it was open and submitted after
// it was closed. Submitting an arbitrary index would let the clause above's guard produce the
// same `false`, and the clause would then pass with the "no menu open" arm deleted.
//
// `RecordedCommandCount()` IS AGAIN THE CONTROL, for the same reason.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuSubmitWithNoMenuTest,
	"Stratocracy.StratPlay.GATE-BUILDMENU.SubmitWithNoMenuOpenIsRefusedWithoutReachingTheRules",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratProductionMenuSubmitWithNoMenuTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuSeam;

	ExpectTheTileMeshWarning(*this);

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	FSeamHarness H;
	if (!H.Arm(*this, Config))
	{
		return false;
	}

	FStratViewModel Model;
	FString         BuildError;
	if (!TestTrue(TEXT("the view model builds"), H.Subsystem->BuildViewModel(Model, BuildError)))
	{
		AddError(BuildError);
		return false;
	}

	FIntPoint FactoryHex;
	if (!TestTrue(TEXT("the viewing side holds a factory"),
			FindHeldFactory(Model, Model.ViewingSide, FactoryHex)))
	{
		AddError(TEXT("no held factory; there is no offered row to capture"));
		return false;
	}

	FString OpenReason;
	if (!TestTrue(TEXT("a menu is opened so a REAL row can be captured from it"),
			H.Subsystem->RefreshProductionMenu(FactoryHex, OpenReason)))
	{
		AddError(OpenReason);
		return false;
	}

	int32 OfferedDefIndex = INDEX_NONE;
	if (!TestTrue(TEXT("the open menu offers a row this side can pay for and place"),
			FindABuildableRow(H.Subsystem->ProductionMenu, OfferedDefIndex)))
	{
		AddError(TEXT("the board supplied no affordable-and-available row, so the index below ")
			TEXT("would be refusable for a second reason and this clause would isolate nothing"));
		return false;
	}

	// ---- Close, then submit the row that WAS offered ------------------------
	H.Subsystem->CloseProductionMenu();
	if (!TestFalse(TEXT("the menu is closed before the submit"), H.Subsystem->IsProductionMenuOpen()))
	{
		return false;
	}

	const int32 CountBefore = H.Bridge->RecordedCommandCount();

	FString SubmitReason;
	TestFalse(*FString::Printf(
			TEXT("submitting the previously-offered index %d with no menu open is refused"),
			OfferedDefIndex),
		H.Subsystem->SubmitProductionChoice(OfferedDefIndex, SubmitReason));
	TestFalse(TEXT("the refusal says something"), SubmitReason.IsEmpty());

	// THE CONTROL.
	TestEqual(TEXT("no command reached the rules module"),
		H.Bridge->RecordedCommandCount(), CountBefore);
	TestFalse(TEXT("and no menu was invented by the attempt"), H.Subsystem->IsProductionMenuOpen());

	return true;
}

// ---------------------------------------------------------------------------
// An accepted submit builds at the OPEN MENU'S factory, and at no other.
//
// THIS IS THE HIGHEST-VALUE CLAUSE IN THE FILE, and it is the one guarding the defect the
// seam's own declaration says would otherwise be accepted silently: `applyCommand` uses the
// Build command's `unitId` field as a raw, bounds-checked-only index and its `hex` field
// without pairing the two, so a caller-supplied hex would let a widget pick row 2 of factory
// A and build it at factory B with no refusal anywhere.
//
// WHAT IS OBSERVED, AND WHY IT IS THIS AND NOT THE NEW UNIT'S HEX. §2.7 spawns a built unit at
// the factory hex OR AN ADJACENT ONE, so "the unit is standing on the factory" is not a
// property that holds; measuring it would need an adjacency computation, which is arithmetic
// this gate would then be asserting against itself. `FStratFactoryView::bHasBuiltThisTurn` is
// the rules module's own per-factory record of T-TURN-10's allowance, keyed by hex, and it
// names the factory the build was charged to exactly. The clause requires that EXACTLY ONE
// factory's flag moved and that it is `ProductionMenuHex`; every other factory must be
// element-wise unchanged.
//
// THE HONEST LIMIT, STATED RATHER THAN LEFT TO BE INFERRED. On `ferrum_crossing.json` side 0
// holds exactly ONE factory, so this fixture cannot exhibit "built at the wrong factory the
// caller could legitimately have named" -- every other factory hex is one the rules would
// refuse this side outright. The clause is still falsifiable in every direction that matters:
// any change that submits at a hex other than `ProductionMenuHex` reddens it, but on THIS
// scenario it reddens because the rules refuse a factory this side does not hold, not because
// a unit appeared in the wrong place. Distinguishing those two would need a scenario giving
// one side two factories -- a `Data/` addition, vendored and hash-gated, writable from no lane
// in this repo.
//
// THE COVERAGE ASSERTION. "Exactly one flag moved" is satisfiable by a board where the flag
// was already true everywhere. The clause requires the target factory's flag to be FALSE
// before and TRUE after, so the movement is measured and not merely the end state.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuBuildLandsAtTheOpenFactoryTest,
	"Stratocracy.StratPlay.GATE-BUILDMENU.AnAcceptedBuildLandsAtTheOpenMenusFactoryAndNoOther",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratProductionMenuBuildLandsAtTheOpenFactoryTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuSeam;

	ExpectTheTileMeshWarning(*this);

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	FSeamHarness H;
	if (!H.Arm(*this, Config))
	{
		return false;
	}

	FStratViewModel Before;
	FString         BuildError;
	if (!TestTrue(TEXT("the view model builds"), H.Subsystem->BuildViewModel(Before, BuildError)))
	{
		AddError(BuildError);
		return false;
	}

	FIntPoint FactoryHex;
	if (!TestTrue(TEXT("the viewing side holds a factory"),
			FindHeldFactory(Before, Before.ViewingSide, FactoryHex)))
	{
		AddError(TEXT("no held factory; there is nowhere for an accepted build to land"));
		return false;
	}
	if (!TestTrue(TEXT("the board has more than one factory, so 'at no other' has other "
			"candidates to exclude"), Before.Factories.Num() > 1))
	{
		return false;
	}

	FString OpenReason;
	if (!TestTrue(TEXT("the menu opens at the held factory"),
			H.Subsystem->RefreshProductionMenu(FactoryHex, OpenReason)))
	{
		AddError(OpenReason);
		return false;
	}

	int32 DefIndex = INDEX_NONE;
	if (!TestTrue(TEXT("the open menu offers a row this side can pay for and place"),
			FindABuildableRow(H.Subsystem->ProductionMenu, DefIndex)))
	{
		AddError(TEXT("the board supplied no affordable-and-available row; this clause needs an ")
			TEXT("ACCEPTED build and has none, so it is reporting the missing case rather than ")
			TEXT("passing"));
		return false;
	}

	const TMap<FIntPoint, bool> FlagsBefore = BuiltFlagsByHex(Before);
	const bool* const           TargetBefore = FlagsBefore.Find(FactoryHex);
	if (!TestNotNull(TEXT("the target factory is in the model's factory list"), TargetBefore))
	{
		return false;
	}
	if (!TestFalse(TEXT("COVERAGE: the target factory has NOT built this turn before the submit"),
			*TargetBefore))
	{
		AddError(TEXT("the flag was already true, so its being true afterwards would measure ")
			TEXT("nothing"));
		return false;
	}

	const int32 CountBefore = H.Bridge->RecordedCommandCount();

	FString SubmitReason;
	if (!TestTrue(*FString::Printf(TEXT("definition index %d is accepted at the open factory"),
				DefIndex),
			H.Subsystem->SubmitProductionChoice(DefIndex, SubmitReason)))
	{
		AddError(SubmitReason);
		return false;
	}

	TestEqual(TEXT("exactly one command was recorded"),
		H.Bridge->RecordedCommandCount(), CountBefore + 1);

	FStratViewModel After;
	FString         AfterError;
	if (!TestTrue(TEXT("the view model rebuilds after the build"),
			H.Subsystem->BuildViewModel(After, AfterError)))
	{
		AddError(AfterError);
		return false;
	}

	const TMap<FIntPoint, bool> FlagsAfter = BuiltFlagsByHex(After);
	TestEqual(TEXT("the factory set did not change"), FlagsAfter.Num(), FlagsBefore.Num());

	int32     MovedCount = 0;
	FIntPoint MovedHex   = FIntPoint::ZeroValue;
	for (const TPair<FIntPoint, bool>& Pair : FlagsAfter)
	{
		const bool* const WasBuilt = FlagsBefore.Find(Pair.Key);
		if (!TestNotNull(*FString::Printf(TEXT("factory (%d, %d) existed before the build"),
					Pair.Key.X, Pair.Key.Y),
				WasBuilt))
		{
			continue;
		}
		if (*WasBuilt != Pair.Value)
		{
			++MovedCount;
			MovedHex = Pair.Key;
		}
	}

	TestEqual(TEXT("exactly one factory's build allowance moved"), MovedCount, 1);
	TestEqual(TEXT("and it is the factory the OPEN MENU was for"),
		MovedHex, H.Subsystem->ProductionMenuHex);
	TestEqual(TEXT("which is still the hex the menu was opened at"),
		H.Subsystem->ProductionMenuHex, FactoryHex);

	return true;
}

// ---------------------------------------------------------------------------
// An accepted build reaches the BOARD, with no manual refresh in between.
//
// THE CLAUSE CALLS NOTHING AFTER THE SUBMIT. No `ApplyView`, no `RefreshPresentation`, no
// second `RefreshProductionMenu`. If `SubmitProductionChoice` stopped reconciling, the rules
// would hold a unit that no actor stands for and the player would see a board that disagrees
// with the game -- and nothing else in the suite would notice, because every other clause that
// reconciles calls the reconciler itself.
//
// THE EXPECTATION IS THE MODEL, NOT A COUNT. The world's actor id set must equal the freshly
// built `FStratViewModel::Units` id set EXACTLY -- so a spawn that happened at the wrong id, a
// stale actor that survived, and a missing actor all fail, and none of them is expressible as
// "+1". The `+1` is asserted too, separately, as the coverage check that a unit really was
// added; without it a build that spawned nothing would satisfy set equality trivially.
//
// THE ACTOR SET IS READ FROM THE WORLD via `TActorIterator`, not from the subsystem's private
// map, for `StratMatchReconcile.cpp`'s reason.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuBuildReconcilesTheBoardTest,
	"Stratocracy.StratPlay.GATE-BUILDMENU.AnAcceptedBuildReachesTheBoardWithNoManualRefresh",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratProductionMenuBuildReconcilesTheBoardTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuSeam;

	ExpectTheTileMeshWarning(*this);

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	FSeamHarness H;
	if (!H.Arm(*this, Config))
	{
		return false;
	}

	FStratViewModel Before;
	FString         BuildError;
	if (!TestTrue(TEXT("the view model builds"), H.Subsystem->BuildViewModel(Before, BuildError)))
	{
		AddError(BuildError);
		return false;
	}

	FIntPoint FactoryHex;
	if (!TestTrue(TEXT("the viewing side holds a factory"),
			FindHeldFactory(Before, Before.ViewingSide, FactoryHex)))
	{
		AddError(TEXT("no held factory; no build can be accepted"));
		return false;
	}

	FString OpenReason;
	if (!TestTrue(TEXT("the menu opens at the held factory"),
			H.Subsystem->RefreshProductionMenu(FactoryHex, OpenReason)))
	{
		AddError(OpenReason);
		return false;
	}

	int32 DefIndex = INDEX_NONE;
	if (!TestTrue(TEXT("the open menu offers a row this side can pay for and place"),
			FindABuildableRow(H.Subsystem->ProductionMenu, DefIndex)))
	{
		AddError(TEXT("the board supplied no affordable-and-available row; this clause needs an ")
			TEXT("ACCEPTED build and has none"));
		return false;
	}

	const TSet<int32> ActorIdsBefore = LiveUnitIds(H.Scope.World);
	if (!TestTrue(TEXT("the board is already reconciled before the build"),
			ActorIdsBefore.Num() == Before.Units.Num()))
	{
		AddError(FString::Printf(
			TEXT("actors [%s] vs %d model units -- this clause's premise is that the board and ")
			TEXT("the model already agree"),
			*Describe(ActorIdsBefore), Before.Units.Num()));
		return false;
	}

	FString SubmitReason;
	if (!TestTrue(TEXT("the build is accepted"),
			H.Subsystem->SubmitProductionChoice(DefIndex, SubmitReason)))
	{
		AddError(SubmitReason);
		return false;
	}

	// NOTHING IS CALLED HERE. That absence is the subject.

	FStratViewModel After;
	FString         AfterError;
	if (!TestTrue(TEXT("the view model rebuilds so the expectation can be read off it"),
			H.Subsystem->BuildViewModel(After, AfterError)))
	{
		AddError(AfterError);
		return false;
	}

	TSet<int32> ExpectedIds;
	for (const FStratUnitView& Unit : After.Units)
	{
		ExpectedIds.Add(Unit.UnitId);
	}
	TestEqual(TEXT("the model names each unit once"), ExpectedIds.Num(), After.Units.Num());

	// COVERAGE: a build that spawned nothing would satisfy set equality for free. §2.7 holds a
	// build whose factory is boxed in, so this is a real possibility and not a formality.
	if (!TestEqual(TEXT("COVERAGE: the accepted build put exactly one new unit in the rules"),
			ExpectedIds.Num(), ActorIdsBefore.Num() + 1))
	{
		AddError(TEXT("the build queued without spawning (a boxed-in factory), so this clause ")
			TEXT("cannot tell a reconciled board from an unchanged one and is reporting that"));
		return false;
	}

	const TSet<int32> ActorIdsAfter = LiveUnitIds(H.Scope.World);
	if (!TestTrue(TEXT("the world's actors are EXACTLY the rebuilt model's units, with no "
			"manual refresh between the submit and this read"),
			ActorIdsAfter.Difference(ExpectedIds).Num() == 0 &&
			ExpectedIds.Difference(ActorIdsAfter).Num() == 0))
	{
		AddError(FString::Printf(TEXT("actors [%s] vs model [%s]"),
			*Describe(ActorIdsAfter), *Describe(ExpectedIds)));
	}

	// And the new unit has an actor by the id the model gave it, asked the other way round.
	const TSet<int32> NewIds = ExpectedIds.Difference(ActorIdsBefore);
	TestEqual(TEXT("exactly one id is new"), NewIds.Num(), 1);
	for (const int32 NewId : NewIds)
	{
		TestNotNull(*FString::Printf(TEXT("the new unit %d has an actor on the board"), NewId),
			H.Subsystem->FindUnitActor(NewId));
	}

	return true;
}

// ---------------------------------------------------------------------------
// Closing empties the rows, forgets the factory, and submits nothing.
//
// THE HEX GOES WITH THE ROWS. A hex left behind an emptied container reads like live state to
// anyone who consults it without consulting the container first, which is the trap
// `TearDownPresentation` clears `AppliedModel` beside `UnitActors` to avoid.
//
// THE HEX IS ASSERTED AGAINST THE HEX IT WAS OPENED AT, not against `FIntPoint(0, 0)`. The
// zero is a real hex on this board and writing it down here would make the clause a statement
// about a literal; "it is no longer the factory it was showing" is the property and it needs
// no literal. WHAT THIS THEREFORE DOES NOT PIN: that the forgotten value is specifically the
// origin. A close that moved the hex to some other non-opened value passes this clause.
//
// `RecordedCommandCount()` IS ASSERTED UNCHANGED because the declaration makes a claim that
// would otherwise be untested: closing "SUBMITS NOTHING AND CANCELS NOTHING".
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuCloseForgetsTheFactoryTest,
	"Stratocracy.StratPlay.GATE-BUILDMENU.CloseEmptiesTheRowsForgetsTheFactoryAndSubmitsNothing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratProductionMenuCloseForgetsTheFactoryTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuSeam;

	ExpectTheTileMeshWarning(*this);

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	FSeamHarness H;
	if (!H.Arm(*this, Config))
	{
		return false;
	}

	FStratViewModel Model;
	FString         BuildError;
	if (!TestTrue(TEXT("the view model builds"), H.Subsystem->BuildViewModel(Model, BuildError)))
	{
		AddError(BuildError);
		return false;
	}

	FIntPoint FactoryHex;
	if (!TestTrue(TEXT("the viewing side holds a factory"),
			FindHeldFactory(Model, Model.ViewingSide, FactoryHex)))
	{
		AddError(TEXT("no held factory; there is no menu to close"));
		return false;
	}

	FString OpenReason;
	if (!TestTrue(TEXT("a menu is open before it is closed"),
			H.Subsystem->RefreshProductionMenu(FactoryHex, OpenReason)))
	{
		AddError(OpenReason);
		return false;
	}
	if (!TestTrue(TEXT("the open menu has rows to lose"), H.Subsystem->ProductionMenu.Num() > 0))
	{
		return false;
	}

	const int32 CountBefore = H.Bridge->RecordedCommandCount();

	H.Subsystem->CloseProductionMenu();

	TestFalse(TEXT("the menu reports itself closed"), H.Subsystem->IsProductionMenuOpen());
	TestEqual(TEXT("the rows are gone"), H.Subsystem->ProductionMenu.Num(), 0);
	TestFalse(TEXT("the factory it was showing is forgotten"),
		H.Subsystem->ProductionMenuHex == FactoryHex);
	TestEqual(TEXT("closing submitted nothing and cancelled nothing"),
		H.Bridge->RecordedCommandCount(), CountBefore);

	// UNREFUSABLE AND IDEMPOTENT: closing a closed menu is the state the caller asked for.
	H.Subsystem->CloseProductionMenu();
	TestFalse(TEXT("closing a closed menu leaves it closed"), H.Subsystem->IsProductionMenuOpen());
	TestEqual(TEXT("and still records nothing"),
		H.Bridge->RecordedCommandCount(), CountBefore);

	return true;
}

// ---------------------------------------------------------------------------
// A menu does not survive a re-seed.
//
// A BUILDLIST DESCRIBES ONE FACTORY IN ONE `strat::GameState`. After a re-seed the rows price
// a purse that no longer exists and the hex names a factory in a board that was rebuilt, so a
// surviving menu is a menu whose submit would apply a §4.10 Build into a match nobody was
// looking at. `TearDownPresentation` clears it beside `AppliedModel` for exactly this reason.
//
// THE RE-SEED IS A SECOND `StartMatch` with the same config, which is the production path
// `LoadMatchFromSlot` also takes; nothing here reaches into teardown directly.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuDoesNotSurviveAReseedTest,
	"Stratocracy.StratPlay.GATE-BUILDMENU.AMenuDoesNotSurviveAReseed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratProductionMenuDoesNotSurviveAReseedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuSeam;

	ExpectTheTileMeshWarning(*this);

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	FSeamHarness H;
	if (!H.Arm(*this, Config))
	{
		return false;
	}

	FStratViewModel Model;
	FString         BuildError;
	if (!TestTrue(TEXT("the view model builds"), H.Subsystem->BuildViewModel(Model, BuildError)))
	{
		AddError(BuildError);
		return false;
	}

	FIntPoint FactoryHex;
	if (!TestTrue(TEXT("the viewing side holds a factory"),
			FindHeldFactory(Model, Model.ViewingSide, FactoryHex)))
	{
		AddError(TEXT("no held factory; there is no menu to carry across the re-seed"));
		return false;
	}

	FString OpenReason;
	if (!TestTrue(TEXT("a menu is open before the re-seed"),
			H.Subsystem->RefreshProductionMenu(FactoryHex, OpenReason)))
	{
		AddError(OpenReason);
		return false;
	}
	if (!TestTrue(TEXT("the menu that must not survive has rows"),
			H.Subsystem->ProductionMenu.Num() > 0))
	{
		return false;
	}

	FString RestartReason;
	H.Subsystem->StartMatch(Config, RestartReason);
	if (!TestTrue(TEXT("the re-seeded match is live"), H.Subsystem->IsMatchLive()))
	{
		AddError(RestartReason);
		return false;
	}

	TestFalse(TEXT("the menu did not survive the re-seed"), H.Subsystem->IsProductionMenuOpen());
	TestEqual(TEXT("and its rows are gone"), H.Subsystem->ProductionMenu.Num(), 0);
	TestFalse(TEXT("and the factory it named is forgotten"),
		H.Subsystem->ProductionMenuHex == FactoryHex);

	return true;
}

// ---------------------------------------------------------------------------
// A submit during an AI turn is refused, and no command interleaves into the AI's log.
//
// THE ONLY PROPERTY OF THIS SEAM THAT NEEDS A RUNNING AI TURN TO OBSERVE, and the only place
// in this file where a real turn is played. `bAiTurnRunning` is a private reentrancy guard set
// by `TGuardValue` inside `RunAiTurnsNow`; there is no setter, so the submit has to be made
// FROM INSIDE that scope.
//
// HOW IT GETS INSIDE, and it is a production seam rather than a back door.
// `RunAiTurnsNow` ends with `RefreshPresentation()` STILL INSIDE the guard's scope;
// `RefreshPresentation` builds through `BuildViewModelForPresentation`, which runs the
// registered `FStratViewDecorator`. So a decorator registered by this clause is called at a
// moment when `bAiTurnRunning` is true, and it is the only such moment reachable from outside
// the class. The decorator submits and records what it saw; nothing is asserted from inside
// it, because an assertion inside a delegate that failed to fire is an assertion that never
// happened.
//
// THE FIRING OF THE DECORATOR IS ITSELF ASSERTED, for that reason. Without it a build in which
// the decorator seam was removed would leave this clause green having measured nothing.
//
// ATTRIBUTION. The menu is OPEN and the index submitted is one the menu OFFERS, so of
// `SubmitProductionChoice`'s guards only the AI arm can account for the refusal. The control
// is `RecordedCommandCount()` read on both sides of the call inside the decorator: the AI is
// recording into this same bridge, so a count taken outside would move for the AI's own
// reasons and prove nothing.
//
// `AiMaxConsecutiveTurns` IS 1 so the loop stops at its own bound and the match cannot reach a
// §2.8 result -- which is also why the `STRAT-AI refused phase=handover` line is declared
// expected below: that is the bound reporting itself, not a defect. The slot this file's
// config names is deleted on both ends regardless.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuSubmitDuringAnAiTurnTest,
	"Stratocracy.StratPlay.GATE-BUILDMENU.ASubmitDuringAnAiTurnIsRefusedAndRecordsNothing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratProductionMenuSubmitDuringAnAiTurnTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuSeam;

	ExpectTheTileMeshWarning(*this);
	AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
	AddExpectedMessagePlain(TEXT("Build refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	DeleteOurSlot();

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		DeleteOurSlot();
		return false;
	}
	// The side whose screen this is plays itself, so one AI turn is due immediately.
	Config.AiSides.Add(0);
	Config.AiMaxConsecutiveTurns = 1;

	FSeamHarness H;
	if (!H.Arm(*this, Config))
	{
		DeleteOurSlot();
		return false;
	}

	FStratViewModel Model;
	FString         BuildError;
	if (!TestTrue(TEXT("the view model builds"), H.Subsystem->BuildViewModel(Model, BuildError)))
	{
		AddError(BuildError);
		DeleteOurSlot();
		return false;
	}

	FIntPoint FactoryHex;
	if (!TestTrue(TEXT("the viewing side holds a factory"),
			FindHeldFactory(Model, Model.ViewingSide, FactoryHex)))
	{
		AddError(TEXT("no held factory; there is no offered row to submit"));
		DeleteOurSlot();
		return false;
	}

	FString OpenReason;
	if (!TestTrue(TEXT("a menu is open before the AI turn starts"),
			H.Subsystem->RefreshProductionMenu(FactoryHex, OpenReason)))
	{
		AddError(OpenReason);
		DeleteOurSlot();
		return false;
	}

	int32 DefIndex = INDEX_NONE;
	if (!TestTrue(TEXT("the open menu offers a row that is affordable and available, so only "
			"the AI arm can account for the refusal below"),
			FindABuildableRow(H.Subsystem->ProductionMenu, DefIndex)))
	{
		AddError(TEXT("no affordable-and-available row; the refusal would be over-determined"));
		DeleteOurSlot();
		return false;
	}

	// Recorded by the decorator, asserted afterwards. Nothing is asserted from inside a
	// delegate that might never fire.
	struct FSeen
	{
		bool    bFired         = false;
		bool    bSubmitOk      = true;
		FString Reason;
		int32   CountBefore    = INDEX_NONE;
		int32   CountAfter     = INDEX_NONE;
		bool    bMenuStillOpen = false;
	};
	FSeen Seen;

	UStratMatchSubsystem* const Subsystem = H.Subsystem;
	FStratBridge* const         Live      = H.Bridge;
	const int32                 Chosen    = DefIndex;

	Subsystem->SetViewDecorator(FStratViewDecorator::CreateLambda(
		[Subsystem, Live, Chosen, &Seen](FStratViewModel& /*InOut*/)
		{
			if (Seen.bFired)
			{
				return;   // the first call inside the guard is the one that matters
			}
			Seen.bFired      = true;
			Seen.CountBefore = Live->RecordedCommandCount();
			Seen.bSubmitOk   = Subsystem->SubmitProductionChoice(Chosen, Seen.Reason);
			Seen.CountAfter  = Live->RecordedCommandCount();
			Seen.bMenuStillOpen = Subsystem->IsProductionMenuOpen();
		}));

	const int32 CountAtStart = H.Bridge->RecordedCommandCount();

	FString AiReason;
	H.Subsystem->RunAiTurnsNow(AiReason);

	H.Subsystem->ClearViewDecorator();

	if (!TestTrue(TEXT("the decorator fired inside the AI turn, so this clause observed the "
			"moment it exists for"), Seen.bFired))
	{
		AddError(FString::Printf(
			TEXT("RunAiTurnsNow reported '%s' and never reached its presentation rebuild; the ")
			TEXT("seam this clause rides on may have moved, which is a finding and not a pass"),
			*AiReason));
		DeleteOurSlot();
		return false;
	}

	TestFalse(*FString::Printf(TEXT("a submit during a running AI turn is refused: '%s'"),
			*Seen.Reason),
		Seen.bSubmitOk);
	TestFalse(TEXT("the refusal says something"), Seen.Reason.IsEmpty());
	TestTrue(TEXT("CONTROL: no command interleaved into the AI's log"),
		Seen.CountBefore == Seen.CountAfter && Seen.CountBefore != INDEX_NONE);
	TestTrue(TEXT("and the menu is untouched by the refusal"), Seen.bMenuStillOpen);
	// THE INSTRUMENT'S OWN CONTROL. "The count did not move across the submit" is satisfied
	// for free by a bridge that records nothing at all -- a runner that refused its first
	// phase, a match that was already over. The AI turn as a whole MUST have moved the count,
	// or the frozen count above is a fact about the instrument and not about the guard.
	TestTrue(*FString::Printf(
			TEXT("CONTROL: the AI turn itself recorded commands (%d -> %d), so a frozen count ")
			TEXT("across the submit is the guard and not a mute bridge. AI reported: '%s'"),
			CountAtStart, Live->RecordedCommandCount(), *AiReason),
		Live->RecordedCommandCount() > CountAtStart);

	DeleteOurSlot();
	TestFalse(TEXT("this file's save slot is left behind on neither end"),
		UGameplayStatics::DoesSaveGameExist(kSlotName, /*UserIndex*/ 0));

	return true;
}
