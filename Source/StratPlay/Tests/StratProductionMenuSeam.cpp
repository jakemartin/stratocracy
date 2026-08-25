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
#include "HAL/PlatformFileManager.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
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
// rather than chosen -- see this file's header block. A hex is not a refusal channel at all:
// `FStratBridge::BuildOptions` does not pre-check the factory hex, so a non-build-point hex
// comes back as a FULL MENU with every row unavailable. The side is the only lever.
//
// **[THE LEVER CHANGED ON 2026-08-25 AND THE OLD ONE IS RECORDED HERE RATHER THAN DELETED,
// because a reader who remembers it needs to see why it stopped working.** It read:
//
//   RETRACTED> "`SetViewingSide` is deliberately not range-checked and deliberately does not
//   RETRACTED>  roll back, both stated in its own declaration, so it is the one lever that
//   RETRACTED>  makes `FStratBridge::BuildOptions` refuse on a live match."
//
// That was true and is not true now. `UStratMatchSubsystem::SetViewingSide` range-checks
// BEFORE it assigns, so an out-of-range hand-over is refused with the member untouched and
// the next refresh SUCCEEDS -- which is the fix, and which left this clause red because its
// lever, not its subject, had been removed. THE SUBJECT IS UNCHANGED AND NOTHING BELOW IS
// RELAXED: the same two members, the same builder-produced rows, the same all-or-nothing
// assertion.
//
// THE REPLACEMENT LEVER IS THE ONE PATH THAT STILL PUTS AN OUT-OF-RANGE SIDE INTO A LIVE
// MATCH, and it is a real one rather than a contrivance: `FStratMatchConfig::ViewingSide` is
// assigned by `StartMatch` WITHOUT a range check, deliberately -- the class's own comment
// says the check belongs to `StratBuildViewModel`, "nearer the data it indexes than any
// constant this file could name" -- so a Blueprint default can still produce this state. The
// clause therefore builds its menu on a correctly configured match, RESTARTS on a config
// whose `ViewingSide` is `Model.Sides.Num()`, and puts the rows it built back where the
// reseed cleared them.
//
// THE PLANTED ROWS ARE THE BUILDER'S OWN, taken from `RefreshProductionMenu` on the first
// match and never authored here. What is planted is only WHERE they sit, because a reseed
// clears the two members by design (`GATE-BUILDMENU.AMenuDoesNotSurviveAReseed` pins that)
// and this clause is about what a REFUSAL does to them, not about what a reseed does.
//
// `SetViewingSide` IS NO LONGER USED AS THE LEVER, so nothing here reports its return
// value.]**
//
// THE OUT-OF-RANGE SIDE IS `Model.Sides.Num()`, read off the module; no side count is written
// down in this file.
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

	FIntPoint OtherHex = FactoryHex;
	for (const FStratFactoryView& Factory : Model.Factories)
	{
		if (Factory.Hex != FactoryHex)
		{
			OtherHex = Factory.Hex;
			break;
		}
	}

	// ---- The lever: a live match whose viewing side is outside the board ----
	// `StartMatch` assigns `Config.ViewingSide` without a range check, deliberately, so this
	// is the state a Blueprint default can still produce. The side comes from the model's own
	// side list; no count is written down.
	const int32 OutOfRangeSide = Model.Sides.Num();
	{
		FStratMatchConfig Skewed = Config;
		Skewed.ViewingSide = OutOfRangeSide;

		// THE SKEWED MATCH LOGS ITS OWN REASON AT WARNING LEVEL, and that line is DECLARED
		// rather than tolerated: `StartMatch` seeds correctly and then reports that the
		// presentation could not be drawn for side N, which is exactly the documented
		// presentation split and exactly the state this clause is arranging. Undeclared it
		// leaves the clause reporting SuccessWithWarnings, which reads as a defect nobody
		// looked at.
		AddExpectedMessagePlain(TEXT("is outside the snapshot's"), ELogVerbosity::Warning,
			EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

		// NO SECOND `ExpectTheTileMeshWarning` HERE, and the omission is measured rather than
		// an oversight: the declaration at the top of this clause is `Contains` with
		// `Occurrences 0`, which means "at least one" and matches every occurrence in the run.
		// A second declaration asks for its own separate occurrence and fails the clause with
		// "Expected suppressed log message ... did not occur" -- measured 2026-08-25.
		FString RestartReason;
		H.Subsystem->StartMatch(Skewed, RestartReason);
		if (!TestTrue(TEXT("a match with an out-of-range viewing side is still LIVE"),
				H.Subsystem->IsMatchLive()))
		{
			AddError(RestartReason);
			return false;
		}
		if (!TestEqual(
				*FString::Printf(TEXT("and it is holding the out-of-range side %d"),
					OutOfRangeSide),
				H.Subsystem->GetViewingSide(), OutOfRangeSide))
		{
			return false;
		}

		// The reseed cleared both members by design. The rows put back are the ones the
		// builder produced above; only their PLACE is planted, because what this clause
		// measures is what a refusal does to them.
		H.Subsystem->ProductionMenu    = RowsBefore;
		H.Subsystem->ProductionMenuHex = HexBefore;
	}

	FString RefusedReason;
	const bool bRefreshed = H.Subsystem->RefreshProductionMenu(OtherHex, RefusedReason);

	if (!TestFalse(*FString::Printf(
				TEXT("a refresh for the out-of-range side %d is refused"), OutOfRangeSide),
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

// ===========================================================================
// §2.11.5's BOXED-IN FOOTER FACT ON THE SEAM -- `UStratMatchSubsystem::
// IsOpenMenuFactorySpawnBlocked` and the private bool `RefreshProductionMenu` publishes
// beside the rows.
//
// WHY `GATE-BUILDMENU` AND NOT THE `T-UI-03` THE DISPATCH PROPOSED. The four clauses below
// are about a member of `UStratMatchSubsystem` that is written in the SAME STATEMENT GROUP as
// `ProductionMenu` and `ProductionMenuHex` and cleared beside them, and those two are this
// file's subject under `GATE-BUILDMENU` in ten existing clauses. Filing the third member of
// one statement group under a different id would split one seam across two rows. The file's
// own header block already argues the rest of it: `T-UI-04` is an IN-EDITOR claim about a
// widget BINDING, nothing here constructs a widget or touches Slate, and `GATE-BUILDMENU` is
// the id this project already carries for §2.11.5 TRANSPORT. That reasoning applies to the
// footer exactly as it applies to the rows. `GATE-BUILDMENU` pre-exists; nothing is minted.
//
// THE REFUSAL ARM OF `FStratBridge::FactorySpawnBlockedAt` IS NOT HERE, and the split is the
// same one the file header already draws. Telling a refusal apart from a fall-through to raw
// occupancy needs `strat::spawnHexesBlocked`, a `strat::` free function that is `LNK2019` in
// this module. That clause is
// `Stratocracy.StratBridge.T-UI-04.FactorySpawnBlockedRefusesANonFactoryHex`, in
// `Source/StratBridge/Tests/StratSpawnBlockedClauses.cpp`.
//
// THE DESIGN THESE CLAUSES EXIST TO HOLD IN PLACE. The footer fact is re-queried on the ROWS'
// OWN CLOCK -- same bridge, same call, same frame -- rather than read from
// `AppliedModel.Factories`, which is a record of what was last DRAWN and carries no entry at
// all for a menu opened on a non-build-point hex. A future pass re-sourcing the footer from
// `FStratFactoryView::bSpawnBlocked` is exactly what these clauses must catch, and
// `SubmitProductionChoice` is where they can: it calls `RefreshProductionMenu` BEFORE
// `RefreshPresentation`, so at the moment the footer is published the applied model still
// describes the PRE-BUILD board. `SpawnBlockedRidesTheRowsClockAcrossABuild` is built on that
// ordering and says so where it uses it.
//
// WHERE EVERY EXPECTATION COMES FROM. The subject is always
// `IsOpenMenuFactorySpawnBlocked()`. The expectation is always
// `FStratBridge::FactorySpawnBlockedAt` asked of the SAME bridge in the SAME frame -- the
// exact call `RefreshProductionMenu` makes -- and never a bool written down here. Which hexes
// are factories, which side holds them, and which hex is not a factory at all are read off
// `FStratViewModel`. No hex is named and no neighbour is walked anywhere below.
//
// THE BOARDS ARE ARRANGED, NOT SELECTED. `Data/ferrum_crossing.json` leaves side 0's factory
// hex free, so on the shipped board the footer fact is false and every clause below would be
// measuring `false == false`. Each clause therefore writes a variant of the shipped scenario
// under `Saved/StratTests/` -- never under `Data/`, which is vendored and hash-gated -- moving
// placements and nothing else, and deletes it on the way out. The placement strings and the
// two scenario-validator rules that constrain them are `StratSpawnBlockedClauses.cpp`'s
// measurement; they are restated here because that file is in another module and a shared
// header for two test files would be a third thing to keep true.
// ===========================================================================
namespace StratProductionMenuSeam
{
	static FString ShippedScenarioPath()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"), TEXT("ferrum_crossing.json"));
	}

	/** One placement line of the shipped scenario, rewritten to a different hex. Located
	 *  CASE-SENSITIVELY and checked for before it is made, so a re-authored scenario fails
	 *  loudly instead of silently producing a variant identical to the shipped file -- which
	 *  would leave every clause below green and measuring nothing. */
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

	/** The shipped scenario with its declared `scenarioHash` dropped, ready to be edited. The
	 *  hash is optional and DERIVED, so dropping the line says "recompute from content" rather
	 *  than authoring a number for edited bytes. */
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

	// The placement edits, named once so all four clauses share one board vocabulary.
	//
	//   THE FACTORY-HEX EDIT puts one of side 0's Infantry ON its own factory hex, so the
	//   factory hex itself is occupied. The shipped scenario already surrounds that factory
	//   with five of side 0's own units, so this single edit leaves exactly TWO free hexes in
	//   the factory's own neighbourhood.
	//   THE TWO BOXING EDITS bring two of side 1's units onto those two, so that the factory
	//   hex and EVERY in-bounds neighbour are occupied.
	//
	// NEITHER SIDE'S GUIDED-OPENING INFANTRY IS MOVED. T-SCN-07 requires one `guidedOpening`
	// entry per side naming a hex that holds that side's Infantry, so moving a named unit
	// makes the variant unloadable for a reason that has nothing to do with spawning.
	//
	// AND NO SIDE-1 INFANTRY IS MOVED AT ALL, WHICH IS A MEASUREMENT AND NOT A PREFERENCE --
	// `StratSpawnBlockedClauses.cpp`'s, made when the scenario validator refused a side-1
	// Infantry near the middle of the board under T-SCN-11 ("side 0's guided lane is
	// contested"). §2.11.6-B's lane rule is about INFANTRY routes, so the boxing units are
	// side 1's Artillery and Recon, which are invisible to it.
	static const TCHAR* const kSide0InfantryAtOne3 =
		TEXT("{\"side\": 0, \"unitId\": \"Infantry\", \"hex\": [1, 3], \"isFlag\": false}");
	static const TCHAR* const kSide0InfantryOnFactory =
		TEXT("{\"side\": 0, \"unitId\": \"Infantry\", \"hex\": [1, 4], \"isFlag\": false}");

	static const TCHAR* const kSide1ArtilleryHome =
		TEXT("{\"side\": 1, \"unitId\": \"Artillery\", \"hex\": [10, 5], \"isFlag\": false}");
	static const TCHAR* const kSide1ArtilleryBoxing =
		TEXT("{\"side\": 1, \"unitId\": \"Artillery\", \"hex\": [2, 4], \"isFlag\": false}");

	static const TCHAR* const kSide1ReconHome =
		TEXT("{\"side\": 1, \"unitId\": \"Recon\", \"hex\": [10, 3], \"isFlag\": false}");
	static const TCHAR* const kSide1ReconBoxing =
		TEXT("{\"side\": 1, \"unitId\": \"Recon\", \"hex\": [1, 3], \"isFlag\": false}");

	/**
	 * The board on which side 0's factory is BOXED IN: its hex and every in-bounds neighbour
	 * occupied.
	 *
	 * NOTHING HERE ASSERTS THAT IT WORKED, and that is deliberate: every clause that uses this
	 * board asks the BRIDGE whether the factory is boxed in, fatally, before it measures
	 * anything else. A helper that decided the question would be the second author of it.
	 */
	static bool WriteBoxedInVariant(const TCHAR* FileName, FString& OutPath, FString& OutError)
	{
		FString Text;
		if (!LoadScenarioTextWithoutHash(Text, OutError))
		{
			return false;
		}
		if (!MovePlacement(Text, kSide0InfantryAtOne3,  kSide0InfantryOnFactory, OutError) ||
			!MovePlacement(Text, kSide1ArtilleryHome,   kSide1ArtilleryBoxing,   OutError) ||
			!MovePlacement(Text, kSide1ReconHome,       kSide1ReconBoxing,       OutError))
		{
			return false;
		}
		return WriteVariant(Text, FileName, OutPath, OutError);
	}

	/**
	 * The board on which side 0's factory has EXACTLY ONE free hex left in its neighbourhood:
	 * the factory-hex edit and ONE of the two boxing edits, so the Artillery's hex stays free.
	 *
	 * WHICH FREE HEX IT IS IS NEVER NAMED. What the clause using this board needs is only that
	 * the factory reads NOT blocked before a build and blocked after one, and both readings
	 * come from the bridge.
	 */
	static bool WriteNearlyBoxedInVariant(const TCHAR* FileName, FString& OutPath, FString& OutError)
	{
		FString Text;
		if (!LoadScenarioTextWithoutHash(Text, OutError))
		{
			return false;
		}
		if (!MovePlacement(Text, kSide0InfantryAtOne3, kSide0InfantryOnFactory, OutError) ||
			!MovePlacement(Text, kSide1ReconHome,      kSide1ReconBoxing,       OutError))
		{
			return false;
		}
		return WriteVariant(Text, FileName, OutPath, OutError);
	}

	/** `MakeConfig`, pointed at a variant scenario. `ResolveScenarioPath` passes an absolute
	 *  path through unchanged, which is what lets a clause seed from `Saved/`. */
	static bool MakeConfigOn(const FString& AbsoluteScenarioPath, FStratMatchConfig& Out,
	                         FString& OutError)
	{
		if (!MakeConfig(Out, OutError))
		{
			return false;
		}
		Out.ScenarioFile = AbsoluteScenarioPath;
		return true;
	}

	/** A factory the given side does NOT hold. Enumerated from the model; the shipped scenario
	 *  has several, and which one is returned is not this clause's business. */
	static bool FindFactoryNotOwnedBy(const FStratViewModel& Model, int32 Side, FIntPoint& OutHex)
	{
		for (const FStratFactoryView& Factory : Model.Factories)
		{
			if (Factory.Owner != Side)
			{
				OutHex = Factory.Hex;
				return true;
			}
		}
		return false;
	}

	/** `FStratFactoryView::bSpawnBlocked` as the APPLIED MODEL carries it -- the route the
	 *  design rejected, read here so a clause can measure that the footer did not come from
	 *  it. Returns false when the applied model has no entry for that hex, which is itself one
	 *  of the three findings that rejected the route. */
	static bool AppliedModelSaysSpawnBlocked(const FStratViewModel& Applied, FIntPoint Hex,
	                                         bool& bOutFound)
	{
		bOutFound = false;
		for (const FStratFactoryView& Factory : Applied.Factories)
		{
			if (Factory.Hex == Hex)
			{
				bOutFound = true;
				return Factory.bSpawnBlocked;
			}
		}
		return false;
	}
}

// ---------------------------------------------------------------------------
// The open menu's footer fact equals `FStratBridge::FactorySpawnBlockedAt` asked freshly for
// `ProductionMenuHex` in the same frame. IT COPIES; IT DOES NOT DERIVE.
//
// TWO FACTORIES, TWO ANSWERS, ONE FIXTURE, AND THE PAIR IS THE CLAUSE. A menu opened at side
// 0's boxed-in factory must report TRUE and a menu opened at a factory side 0 does not hold --
// which is still a factory, so the bridge still ANSWERS about it -- must report FALSE, both
// against the bridge's own answer for the hex the subsystem says the menu is for. Either half
// alone is satisfied by an accessor that returns a constant; the pair is not.
//
// THE HEX HANDED TO THE EXPECTATION IS `ProductionMenuHex` AND NOT THE HEX THE CLAUSE ASKED
// FOR. That is the brief's wording and it is worth more than the alternative: it makes the
// comparison say "the published footer describes the published hex", so a refresh that
// published one factory's rows beside another factory's footer reddens here rather than
// surviving as two separately-correct members.
//
// A MENU AT A FACTORY THE VIEWING SIDE DOES NOT HOLD IS NOT A REFUSAL, and that is the code's
// own answer rather than this clause's assumption: `FStratBridge::BuildOptions` sends "this
// side does not hold it" out on `available`, which
// `GATE-BUILDMENU.ANonFactoryHexOpensAnUnavailableMenuAndIsNotRefused` already pins one hex
// class over. The footer query is a different question and is not side-filtered at all --
// `MakeUiSnapshot`'s `factories` vector holds every factory on the board.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratOpenMenuSpawnBlockedMatchesTheBridgeTest,
	"Stratocracy.StratPlay.GATE-BUILDMENU.OpenMenuSpawnBlockedMatchesTheBridge",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratOpenMenuSpawnBlockedMatchesTheBridgeTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuSeam;

	ExpectTheTileMeshWarning(*this);

	FString VariantPath;
	FString Error;
	if (!TestTrue(TEXT("the boxed-in variant is written under Saved/"),
			WriteBoxedInVariant(TEXT("menu_footer_boxed_in.json"), VariantPath, Error)))
	{
		AddError(Error);
		RemoveVariant(VariantPath);
		return false;
	}

	{
		FStratMatchConfig Config;
		if (!TestTrue(TEXT("the match config assembles on the variant"),
				MakeConfigOn(VariantPath, Config, Error)))
		{
			AddError(Error);
			RemoveVariant(VariantPath);
			return false;
		}

		FSeamHarness H;
		if (!H.Arm(*this, Config))
		{
			RemoveVariant(VariantPath);
			return false;
		}

		FStratViewModel Model;
		FString         BuildError;
		if (!TestTrue(TEXT("the view model builds on the variant"),
				H.Subsystem->BuildViewModel(Model, BuildError)))
		{
			AddError(BuildError);
			RemoveVariant(VariantPath);
			return false;
		}

		FIntPoint HeldHex;
		if (!TestTrue(TEXT("the variant still gives the viewing side a factory to shop at"),
				FindHeldFactory(Model, Model.ViewingSide, HeldHex)))
		{
			AddError(TEXT("no factory in the view model is owned by the viewing side; this "
			              "clause has no case to measure and must not pass"));
			RemoveVariant(VariantPath);
			return false;
		}

		FIntPoint OtherHex;
		if (!TestTrue(TEXT("the variant carries a factory the viewing side does NOT hold"),
				FindFactoryNotOwnedBy(Model, Model.ViewingSide, OtherHex)))
		{
			AddError(TEXT("every factory in the view model is the viewing side's; this clause "
			              "cannot measure two different answers and must not pass"));
			RemoveVariant(VariantPath);
			return false;
		}

		// ---- The premise: the arrangement really boxed the held factory in --------
		// FATAL, AND ASKED OF THE BRIDGE. Without it every assertion below is
		// `false == false` and the clause is green on an accessor that returns a constant.
		{
			bool               bArranged = false;
			const FStratResult Asked     = H.Bridge->FactorySpawnBlockedAt(HeldHex, bArranged);
			if (!TestTrue(*FString::Printf(
						TEXT("PREMISE: the bridge answers about the held factory (%d, %d): '%s'"),
						HeldHex.X, HeldHex.Y, *Asked.Reason),
					Asked.bOk))
			{
				RemoveVariant(VariantPath);
				return false;
			}
			if (!TestTrue(*FString::Printf(
						TEXT("PREMISE: the variant boxed the held factory (%d, %d) in, so the ")
						TEXT("clause below is not measuring false against false"),
						HeldHex.X, HeldHex.Y),
					bArranged))
			{
				AddError(TEXT("the boxing arrangement did not box the factory in"));
				RemoveVariant(VariantPath);
				return false;
			}
		}

		// ---- Subject A: the menu is open at the boxed-in factory -------------------
		bool bAccessorAtHeld = false;
		{
			FString RefreshReason;
			if (!TestTrue(*FString::Printf(TEXT("the menu opens at the held factory (%d, %d)"),
						HeldHex.X, HeldHex.Y),
					H.Subsystem->RefreshProductionMenu(HeldHex, RefreshReason)))
			{
				AddError(RefreshReason);
				RemoveVariant(VariantPath);
				return false;
			}

			bool               bFresh = false;
			const FStratResult Fresh  =
				H.Bridge->FactorySpawnBlockedAt(H.Subsystem->ProductionMenuHex, bFresh);
			TestTrue(*FString::Printf(
					TEXT("the bridge answers about the PUBLISHED hex (%d, %d): '%s'"),
					H.Subsystem->ProductionMenuHex.X, H.Subsystem->ProductionMenuHex.Y,
					*Fresh.Reason),
				Fresh.bOk);

			bAccessorAtHeld = H.Subsystem->IsOpenMenuFactorySpawnBlocked();
			TestTrue(
				*FString::Printf(
					TEXT("the open menu's footer fact equals the bridge asked freshly for the ")
					TEXT("published hex in the same frame (accessor %d, bridge %d)"),
					bAccessorAtHeld ? 1 : 0, bFresh ? 1 : 0),
				bAccessorAtHeld == bFresh);
			TestTrue(TEXT("and on this board that shared answer is TRUE"), bAccessorAtHeld);
		}

		// ---- Subject B: the menu is open at a factory this side does not hold ------
		bool bAccessorAtOther = true;
		{
			FString RefreshReason;
			if (!TestTrue(*FString::Printf(
						TEXT("the menu opens at the unheld factory (%d, %d) rather than being ")
						TEXT("refused"),
						OtherHex.X, OtherHex.Y),
					H.Subsystem->RefreshProductionMenu(OtherHex, RefreshReason)))
			{
				AddError(RefreshReason);
				RemoveVariant(VariantPath);
				return false;
			}

			bool               bFresh = false;
			const FStratResult Fresh  =
				H.Bridge->FactorySpawnBlockedAt(H.Subsystem->ProductionMenuHex, bFresh);
			TestTrue(*FString::Printf(
					TEXT("the bridge answers about the unheld factory too, unfiltered by side ")
					TEXT("(%d, %d): '%s'"),
					H.Subsystem->ProductionMenuHex.X, H.Subsystem->ProductionMenuHex.Y,
					*Fresh.Reason),
				Fresh.bOk);

			bAccessorAtOther = H.Subsystem->IsOpenMenuFactorySpawnBlocked();
			TestTrue(
				*FString::Printf(
					TEXT("the footer for the unheld factory equals the bridge's own answer ")
					TEXT("(accessor %d, bridge %d)"),
					bAccessorAtOther ? 1 : 0, bFresh ? 1 : 0),
				bAccessorAtOther == bFresh);
			TestFalse(TEXT("and on this board that shared answer is FALSE"), bAccessorAtOther);
		}

		// ---- What kills a constant ------------------------------------------------
		TestTrue(
			TEXT("the two menus in this clause reported DIFFERENT footer facts, so no accessor "
				"returning a constant can satisfy it"),
			bAccessorAtHeld != bAccessorAtOther);
	}

	RemoveVariant(VariantPath);
	return true;
}

// ---------------------------------------------------------------------------
// With no menu open the footer fact is FALSE -- before any refresh, after a close, and after
// a reseed.
//
// TWO ARRANGEMENTS, BECAUSE THERE ARE TWO ROUTES TO "CLOSED" AND THEY RUN DIFFERENT CODE.
// `CloseProductionMenu` is one; `TearDownPresentation`, which a second `StartMatch` runs, is
// the other. The second one exists in this file already as
// `GATE-BUILDMENU.AMenuDoesNotSurviveAReseed` for the rows, and the footer must not be the one
// member of that statement group a reseed forgets.
//
// AND THE HALF THIS CLAUSE HONESTLY CANNOT SEPARATE, STATED RATHER THAN IMPLIED. The shipped
// answer is false TWICE OVER: `CloseProductionMenu` resets the published bool, and
// `IsOpenMenuFactorySpawnBlocked` ANDs `IsProductionMenuOpen()` in on top of it. The dispatch
// asked for arrangements that tell which half is load-bearing, and MEASURED HERE, NO SUCH
// ARRANGEMENT EXISTS FROM `Tests/`: both routes to "closed" run the reset, so there is no
// reachable state in which the bool is stale-true with the rows empty. Deleting the conjunct
// therefore reddens nothing below, and deleting the reset reddens nothing below either --
// which is exactly the redundancy the code's own comment claims ("the closed-panel answer is
// false twice over -- deliberately, because that accessor's guarantee must survive a future
// writer that clears the rows and forgets this line"). What this clause pins is the guarantee;
// it does not pin which half delivers it, and a reader must not read it as though it did.
// Separating them would need a seam this lane may not add -- see the note filed in
// `Tools/architect/state/tests.md`.
//
// EVERY "FALSE" HERE IS PRECEDED BY A TRUE ON THE SAME SUBSYSTEM. A clause that only ever
// observed false would be satisfied by an accessor that never reports anything, and this
// suite has been bitten by a mute instrument before. The boxed-in board makes the true
// reachable, and it is asserted fatally each time before the close that must undo it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSpawnBlockedFalseWhileNoMenuIsOpenTest,
	"Stratocracy.StratPlay.GATE-BUILDMENU.SpawnBlockedFalseWhileNoMenuIsOpen",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSpawnBlockedFalseWhileNoMenuIsOpenTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuSeam;

	ExpectTheTileMeshWarning(*this);

	FString VariantPath;
	FString Error;
	if (!TestTrue(TEXT("the boxed-in variant is written under Saved/"),
			WriteBoxedInVariant(TEXT("menu_footer_closed.json"), VariantPath, Error)))
	{
		AddError(Error);
		RemoveVariant(VariantPath);
		return false;
	}

	{
		FStratMatchConfig Config;
		if (!TestTrue(TEXT("the match config assembles on the variant"),
				MakeConfigOn(VariantPath, Config, Error)))
		{
			AddError(Error);
			RemoveVariant(VariantPath);
			return false;
		}

		FSeamHarness H;
		if (!H.Arm(*this, Config))
		{
			RemoveVariant(VariantPath);
			return false;
		}

		// ---- Arrangement 0: a fresh match, before any refresh ---------------------
		TestFalse(TEXT("a fresh match has no production menu open"),
			H.Subsystem->IsProductionMenuOpen());
		TestFalse(
			TEXT("and before any refresh the footer fact claims nothing about any factory"),
			H.Subsystem->IsOpenMenuFactorySpawnBlocked());

		FStratViewModel Model;
		FString         BuildError;
		if (!TestTrue(TEXT("the view model builds on the variant"),
				H.Subsystem->BuildViewModel(Model, BuildError)))
		{
			AddError(BuildError);
			RemoveVariant(VariantPath);
			return false;
		}

		FIntPoint HeldHex;
		if (!TestTrue(TEXT("the variant still gives the viewing side a factory to shop at"),
				FindHeldFactory(Model, Model.ViewingSide, HeldHex)))
		{
			RemoveVariant(VariantPath);
			return false;
		}

		// ---- Arrangement 1: opened at a boxed-in factory, then CLOSED --------------
		{
			FString RefreshReason;
			if (!TestTrue(TEXT("the menu opens at the boxed-in factory"),
					H.Subsystem->RefreshProductionMenu(HeldHex, RefreshReason)))
			{
				AddError(RefreshReason);
				RemoveVariant(VariantPath);
				return false;
			}
			if (!TestTrue(
					TEXT("PREMISE: the open menu's footer fact is TRUE, so the false below is "
						"something being undone rather than something never set"),
					H.Subsystem->IsOpenMenuFactorySpawnBlocked()))
			{
				AddError(TEXT("the boxing arrangement did not box the factory in"));
				RemoveVariant(VariantPath);
				return false;
			}

			H.Subsystem->CloseProductionMenu();

			TestFalse(TEXT("CloseProductionMenu takes the menu down"),
				H.Subsystem->IsProductionMenuOpen());
			TestFalse(TEXT("and the footer fact goes with it"),
				H.Subsystem->IsOpenMenuFactorySpawnBlocked());
		}

		// ---- Arrangement 2: opened again, then the match is RESEEDED ---------------
		// A DIFFERENT ROUTE TO CLOSED. `StartMatch` runs `TearDownPresentation`, which clears
		// the menu; a reseed that took the rows down and left the footer standing would be
		// invisible to arrangement 1 and is exactly what this half is for.
		{
			FString RefreshReason;
			if (!TestTrue(TEXT("the menu opens at the boxed-in factory a second time"),
					H.Subsystem->RefreshProductionMenu(HeldHex, RefreshReason)))
			{
				AddError(RefreshReason);
				RemoveVariant(VariantPath);
				return false;
			}
			if (!TestTrue(
					TEXT("PREMISE: the footer fact is TRUE again before the reseed"),
					H.Subsystem->IsOpenMenuFactorySpawnBlocked()))
			{
				RemoveVariant(VariantPath);
				return false;
			}

			FString RestartReason;
			H.Subsystem->StartMatch(Config, RestartReason);
			if (!TestTrue(TEXT("the reseeded match is live whatever StartMatch returned"),
					H.Subsystem->IsMatchLive()))
			{
				AddError(RestartReason);
				RemoveVariant(VariantPath);
				return false;
			}

			TestFalse(TEXT("a reseed leaves no menu open"), H.Subsystem->IsProductionMenuOpen());
			TestFalse(
				TEXT("and no footer fact standing either, so the third member of that statement "
					"group is not the one the reseed forgets"),
				H.Subsystem->IsOpenMenuFactorySpawnBlocked());
		}
	}

	RemoveVariant(VariantPath);
	return true;
}

// ---------------------------------------------------------------------------
// A menu opened on a hex that is not a build point reports the footer fact FALSE -- and the
// menu OPENS. A REFUSAL IS NOT A YES.
//
// THE CODE PATH THIS IS ABOUT. `RefreshProductionMenu` does not pre-check the hex, so a
// non-factory hex comes back as a full menu with every row `bAvailable` false --
// `GATE-BUILDMENU.ANonFactoryHexOpensAnUnavailableMenuAndIsNotRefused` pins that half already
// and this clause asserts it again only as its own premise. What is NEW is what the footer
// does on that path: `FStratBridge::FactorySpawnBlockedAt` REFUSES a hex that is not a build
// point, `RefreshProductionMenu` reads only `.bOk` off that refusal, and the published bool
// must therefore be false rather than the refusal's out-parameter being trusted or the
// previous factory's answer being left standing.
//
// THE ORDER IS THE TEETH. The menu is opened at the BOXED-IN factory first, so the published
// bool is TRUE, and only then moved to the non-factory hex. A refresh that forgot to publish
// the footer at all -- or that published it only on the success branch of the query -- leaves
// the previous factory's TRUE behind and reddens here. Asked cold, on a fresh match, this
// clause would read false for free and could not fail.
//
// AND THE REFUSAL IS ASSERTED, NOT ASSUMED. `FactorySpawnBlockedAt` is asked about the same
// hex in the same frame and is required to refuse, so a tree in which a non-factory hex became
// answerable turns this clause red at the premise rather than leaving it silently measuring a
// different code path than the one its prose describes.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSpawnBlockedFalseOnANonBuildPointMenuTest,
	"Stratocracy.StratPlay.GATE-BUILDMENU.SpawnBlockedFalseOnANonBuildPointMenu",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSpawnBlockedFalseOnANonBuildPointMenuTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuSeam;

	ExpectTheTileMeshWarning(*this);

	FString VariantPath;
	FString Error;
	if (!TestTrue(TEXT("the boxed-in variant is written under Saved/"),
			WriteBoxedInVariant(TEXT("menu_footer_non_build_point.json"), VariantPath, Error)))
	{
		AddError(Error);
		RemoveVariant(VariantPath);
		return false;
	}

	{
		FStratMatchConfig Config;
		if (!TestTrue(TEXT("the match config assembles on the variant"),
				MakeConfigOn(VariantPath, Config, Error)))
		{
			AddError(Error);
			RemoveVariant(VariantPath);
			return false;
		}

		FSeamHarness H;
		if (!H.Arm(*this, Config))
		{
			RemoveVariant(VariantPath);
			return false;
		}

		FStratViewModel Model;
		FString         BuildError;
		if (!TestTrue(TEXT("the view model builds on the variant"),
				H.Subsystem->BuildViewModel(Model, BuildError)))
		{
			AddError(BuildError);
			RemoveVariant(VariantPath);
			return false;
		}

		FIntPoint HeldHex;
		FIntPoint NonFactoryHex;
		if (!TestTrue(TEXT("the variant still gives the viewing side a factory to shop at"),
				FindHeldFactory(Model, Model.ViewingSide, HeldHex)) ||
			!TestTrue(TEXT("the board carries a hex that no factory claims"),
				FindNonFactoryHex(Model, NonFactoryHex)))
		{
			RemoveVariant(VariantPath);
			return false;
		}

		// ---- The premise: publish a TRUE that the move must undo -------------------
		{
			FString RefreshReason;
			if (!TestTrue(TEXT("the menu opens at the boxed-in factory first"),
					H.Subsystem->RefreshProductionMenu(HeldHex, RefreshReason)))
			{
				AddError(RefreshReason);
				RemoveVariant(VariantPath);
				return false;
			}
			if (!TestTrue(
					TEXT("PREMISE: the footer fact is TRUE at the boxed-in factory, so the "
						"false below is a value being replaced rather than never written"),
					H.Subsystem->IsOpenMenuFactorySpawnBlocked()))
			{
				AddError(TEXT("the boxing arrangement did not box the factory in"));
				RemoveVariant(VariantPath);
				return false;
			}
		}

		// ---- The premise the clause is written around: the bridge REFUSES here ------
		{
			bool               bOut  = false;
			const FStratResult Asked = H.Bridge->FactorySpawnBlockedAt(NonFactoryHex, bOut);
			TestFalse(*FString::Printf(
					TEXT("PREMISE: the bridge refuses the non-build-point hex (%d, %d): '%s'"),
					NonFactoryHex.X, NonFactoryHex.Y, *Asked.Reason),
				Asked.bOk);
		}

		// ---- The menu moves to the non-build-point hex ------------------------------
		FString RefreshReason;
		if (!TestTrue(*FString::Printf(
					TEXT("a refresh at the non-build-point hex (%d, %d) SUCCEEDS: '%s'"),
					NonFactoryHex.X, NonFactoryHex.Y, *RefreshReason),
				H.Subsystem->RefreshProductionMenu(NonFactoryHex, RefreshReason)))
		{
			AddError(RefreshReason);
			RemoveVariant(VariantPath);
			return false;
		}

		TestTrue(TEXT("and the menu is OPEN on it"), H.Subsystem->IsProductionMenuOpen());
		TestEqual(TEXT("published at the hex that was asked for"),
			H.Subsystem->ProductionMenuHex, NonFactoryHex);
		TestTrue(TEXT("with rows to show"), H.Subsystem->ProductionMenu.Num() > 0);
		for (const FStratBuildOptionView& Row : H.Subsystem->ProductionMenu)
		{
			TestFalse(*FString::Printf(
					TEXT("row '%s' of a non-build-point menu is unavailable, in the module's ")
					TEXT("own words ('%s')"),
					*Row.Id.ToString(), *Row.Reason.ToString()),
				Row.bAvailable);
		}

		// ---- The clause -------------------------------------------------------------
		TestFalse(
			TEXT("and the footer fact is FALSE: the bridge REFUSED the question, and a refusal "
				"is not a yes -- nor is the boxed-in factory's TRUE left standing behind it"),
			H.Subsystem->IsOpenMenuFactorySpawnBlocked());
	}

	RemoveVariant(VariantPath);
	return true;
}

// ---------------------------------------------------------------------------
// The footer fact RIDES THE ROWS' CLOCK: a build that fills the factory's last free hex moves
// it inside the SAME `SubmitProductionChoice` that rebuilt the rows.
//
// THIS IS THE CLAUSE THAT REDDENS IF THE FOOTER IS EVER RE-SOURCED FROM `AppliedModel`, and
// the reason it can be is an ordering rather than an argument. `SubmitProductionChoice` calls
// `RefreshProductionMenu` FIRST and `RefreshPresentation` SECOND, so at the instant the footer
// is published the applied model still describes the PRE-BUILD board -- where this factory is
// NOT boxed in. An implementation reading `FStratFactoryView::bSpawnBlocked` out of
// `AppliedModel.Factories` there publishes false and fails the terminal assertion below.
//
// THE PRE-BUILD APPLIED-MODEL VALUE IS CAPTURED AND PRINTED, so the failure names the defect
// instead of merely reporting a bool. It is captured as an OBSERVATION and is never used as an
// expectation: the expectation on both sides of the build is `FStratBridge::
// FactorySpawnBlockedAt` asked freshly of the same bridge in the same frame.
//
// THE BOARD IS ARRANGED SO THAT THE BUILD ITSELF IS WHAT FILLS THE LAST HEX. Side 0's factory
// hex is occupied and all but one of its in-bounds neighbours are, so §2.7 places the built
// unit on the one that is left. Both readings -- not blocked before, blocked after -- come
// from the bridge, and the FIRST is fatal: a board that was already boxed in would make the
// "after" reading true for free and the clause would be measuring nothing.
//
// WHAT IT DELIBERATELY DOES NOT PIN. Which hex the unit landed on. That needs an adjacency
// answer, `strat::neighbors` is `LNK2019` in this module, and
// `T-UI-05.ADisplacedSpawnLandsOnANeighbourAndSpawnBlockedStaysFalse` in StratBridge owns it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSpawnBlockedRidesTheRowsClockTest,
	"Stratocracy.StratPlay.GATE-BUILDMENU.SpawnBlockedRidesTheRowsClockAcrossABuild",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSpawnBlockedRidesTheRowsClockTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuSeam;

	ExpectTheTileMeshWarning(*this);

	FString VariantPath;
	FString Error;
	if (!TestTrue(TEXT("the nearly-boxed-in variant is written under Saved/"),
			WriteNearlyBoxedInVariant(TEXT("menu_footer_last_hex.json"), VariantPath, Error)))
	{
		AddError(Error);
		RemoveVariant(VariantPath);
		return false;
	}

	{
		FStratMatchConfig Config;
		if (!TestTrue(TEXT("the match config assembles on the variant"),
				MakeConfigOn(VariantPath, Config, Error)))
		{
			AddError(Error);
			RemoveVariant(VariantPath);
			return false;
		}

		FSeamHarness H;
		if (!H.Arm(*this, Config))
		{
			RemoveVariant(VariantPath);
			return false;
		}

		FStratViewModel Model;
		FString         BuildError;
		if (!TestTrue(TEXT("the view model builds on the variant"),
				H.Subsystem->BuildViewModel(Model, BuildError)))
		{
			AddError(BuildError);
			RemoveVariant(VariantPath);
			return false;
		}

		FIntPoint HeldHex;
		if (!TestTrue(TEXT("the variant still gives the viewing side a factory to shop at"),
				FindHeldFactory(Model, Model.ViewingSide, HeldHex)))
		{
			RemoveVariant(VariantPath);
			return false;
		}

		// ---- Before: the menu opens and the factory is NOT boxed in ----------------
		FString RefreshReason;
		if (!TestTrue(TEXT("the menu opens at the held factory"),
				H.Subsystem->RefreshProductionMenu(HeldHex, RefreshReason)))
		{
			AddError(RefreshReason);
			RemoveVariant(VariantPath);
			return false;
		}

		{
			bool               bFreshBefore = false;
			const FStratResult Fresh =
				H.Bridge->FactorySpawnBlockedAt(H.Subsystem->ProductionMenuHex, bFreshBefore);
			if (!TestTrue(*FString::Printf(TEXT("the bridge answers before the build: '%s'"),
						*Fresh.Reason),
					Fresh.bOk))
			{
				RemoveVariant(VariantPath);
				return false;
			}
			const bool bAccessorBefore = H.Subsystem->IsOpenMenuFactorySpawnBlocked();
			TestTrue(*FString::Printf(
					TEXT("before the build the footer equals the bridge (accessor %d, bridge %d)"),
					bAccessorBefore ? 1 : 0, bFreshBefore ? 1 : 0),
				bAccessorBefore == bFreshBefore);
			if (!TestFalse(
					TEXT("PREMISE: the factory is NOT boxed in yet -- one free hex is left, and "
						"the build below is what takes it. A board already boxed in would make "
						"the assertion after the build true for free"),
					bAccessorBefore))
			{
				AddError(TEXT("the nearly-boxed arrangement left no free hex, or left more than "
				              "one; either way this clause measures nothing"));
				RemoveVariant(VariantPath);
				return false;
			}
		}

		// ---- The observation the terminal assertion is contrasted against -----------
		bool       bFoundInApplied = false;
		const bool bAppliedBefore  =
			AppliedModelSaysSpawnBlocked(H.Subsystem->GetViewModel(), HeldHex, bFoundInApplied);
		AddInfo(FString::Printf(
			TEXT("the PRE-BUILD applied model says spawnBlocked=%d for (%d, %d) (entry found: ")
			TEXT("%d). This is the value a footer re-sourced from AppliedModel would publish ")
			TEXT("inside SubmitProductionChoice, because RefreshProductionMenu runs before ")
			TEXT("RefreshPresentation."),
			bAppliedBefore ? 1 : 0, HeldHex.X, HeldHex.Y, bFoundInApplied ? 1 : 0));

		// ---- The build --------------------------------------------------------------
		int32 DefIndex = INDEX_NONE;
		if (!TestTrue(TEXT("the module offers a row that is both affordable and available"),
				FindABuildableRow(H.Subsystem->ProductionMenu, DefIndex)))
		{
			RemoveVariant(VariantPath);
			return false;
		}

		FString SubmitReason;
		if (!TestTrue(*FString::Printf(TEXT("the build is accepted: '%s'"), *SubmitReason),
				H.Subsystem->SubmitProductionChoice(DefIndex, SubmitReason)))
		{
			AddError(SubmitReason);
		}

		// ---- After: the SAME call moved it ------------------------------------------
		bool               bFreshAfter = false;
		const FStratResult FreshAfter  =
			H.Bridge->FactorySpawnBlockedAt(H.Subsystem->ProductionMenuHex, bFreshAfter);
		TestTrue(*FString::Printf(TEXT("the bridge answers after the build: '%s'"),
				*FreshAfter.Reason),
			FreshAfter.bOk);

		const bool bAccessorAfter = H.Subsystem->IsOpenMenuFactorySpawnBlocked();

		TestTrue(TEXT("the menu is still open on the same factory after the rebuild"),
			H.Subsystem->IsProductionMenuOpen() &&
				H.Subsystem->ProductionMenuHex == HeldHex);

		TestTrue(*FString::Printf(
				TEXT("after the build the footer still equals the bridge asked freshly ")
				TEXT("(accessor %d, bridge %d)"),
				bAccessorAfter ? 1 : 0, bFreshAfter ? 1 : 0),
			bAccessorAfter == bFreshAfter);

		// THE TERMINAL ASSERTION. The build consumed the factory's last free hex, so the
		// factory is boxed in NOW -- and the rebuild that `SubmitProductionChoice` performed
		// is the only thing that could have said so.
		TestTrue(
			*FString::Printf(
				TEXT("the footer fact moved false -> true inside the SAME ")
				TEXT("SubmitProductionChoice that rebuilt the rows. A footer re-sourced from ")
				TEXT("AppliedModel.Factories would have published the pre-build value (%d) ")
				TEXT("here, because RefreshProductionMenu runs before RefreshPresentation"),
				bAppliedBefore ? 1 : 0),
			bAccessorAfter);

		DeleteOurSlot();
		TestFalse(TEXT("this file's save slot is left behind on neither end"),
			UGameplayStatics::DoesSaveGameExist(kSlotName, /*UserIndex*/ 0));
	}

	RemoveVariant(VariantPath);
	return true;
}
