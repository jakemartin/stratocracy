// The gate on §2.11.5's BUILD AFFORDANCE -- GATE-BUILDMENU. GDD §2.11.5, §2.11.2.
//
// WHAT THIS FILE'S SUBJECT IS. The user ruled that a dedicated input action opens §2.11.5's
// production menu, and `AStratPlayerController` grew the three things that ruling needs: a
// latched target hex with a second channel saying whether anything is latched
// (`GetProductionTargetHex`), a toggle that opens or closes (`ToggleProductionMenu`), and a
// handler and action asset behind it. This file pins what that toggle does on the paths a
// headless run can reach, and it is explicit about which paths those are.
//
// THE HEX IS LATCHED AT THE KEYPRESS AND NEVER RE-READ, AND THAT IS WHY THE LATCH HAS TWO
// CHANNELS. `WBP_ProductionMenu`'s `Construct` runs INSIDE `AddToViewport`, so a live cursor
// read would answer a different hex the moment the mouse moved off the factory between the
// keypress and the construct -- a menu built for a factory the player never pointed at. The
// second channel exists because `FIntPoint(0, 0)` is a real hex on this board and cannot signal
// its own absence, the same trap `UStratMatchSubsystem::IsProductionMenuOpen` records about
// `ProductionMenuHex`.
//
// ================================================================================
// WHAT THIS FILE CANNOT PIN, AND IT IS A THIRD OF THE AFFORDANCE. READ THIS BEFORE
// TREATING THE SUBJECT AS COVERED.
// ================================================================================
// `AStratPlayerController::HexUnderCursor` calls `APlayerController::GetHitResultUnderCursor`,
// which needs a `ULocalPlayer` with a viewport client to read a mouse position from. A headless
// `-nullrhi` automation run has neither, and a transient `UWorld` has no local player at all.
// So `HexUnderCursor` returns false in EVERY state reachable below, and therefore:
//
//   - `bHasProductionTargetHex` is never SET headlessly. `GetProductionTargetHex` answers false
//     in every reachable state, so there is NO POSITIVE CONTROL for it in this file. What is
//     pinned is that nothing latches a hex it was not given; what is NOT pinned is that the
//     accessor can ever answer true. A future PIE clause owes that half.
//   - The unwind branch -- `ToggleProductionMenu` clearing the latch when
//     `OpenProductionMenuWidget` refuses -- is UNREACHABLE. The toggle returns at "the cursor
//     is not on the board" before the latch is set and before the HUD is asked for a widget. A
//     clause asserting the unwind would assert `false == false` over a branch that never
//     executed, which is worse than no clause; it is deliberately absent, and the engineer's own
//     judgement that this is "the one most likely to rot" stands with no gate under it.
//   - The CLOSE ARM of the toggle is unreachable for the same family of reasons: it is guarded
//     by `AStratScoreboardHUD::IsProductionMenuWidgetOpen()`, which is false headlessly because
//     `AddToViewport` is a no-op with no game viewport (measured; see
//     `Source/StratUI/Tests/StratProductionMenuHostClauses.cpp`'s block). What IS pinned below
//     is the DIVISION OF LABOUR that arm depends on -- that the HUD's close does not clear the
//     subsystem's rows -- because that is reachable by calling the two objects directly.
//   - `OpenProductionMenuAction`'s BINDING at `ETriggerEvent::Started` is not pinned. The
//     property is `protected`, the binding happens in `SetupInputComponent` against an
//     `UEnhancedInputComponent` that a transient world does not build, and the action asset
//     itself is the editor lane's. That is an asset-and-PIE gate, not a headless one.
//
// WHAT IS GATED ELSEWHERE, and is deliberately not restated here:
//  - The HUD's widget-host half -- the viewport reader, the unconditional close, the unset
//    class on both its channels -- is `Source/StratUI/Tests/StratProductionMenuHostClauses.cpp`.
//  - The subsystem's rows -- refresh, submit, close, reseed, AI turn -- are the ten
//    `GATE-BUILDMENU` clauses in `StratProductionMenuSeam.cpp` in this directory.
//
// ONE CLAUSE THE ENGINEER PROPOSED IS REFUSED HERE AND NAMED RATHER THAN DROPPED.
// "`FStratSelectionMachine` has no BUILD arm" is the negative that makes the two retracted
// deferral bullets honest, and it is TRUE -- verified by reading the tree: the working-tree diff
// to `StratSelectionMachine.h` is comment text only, `EStratSelectionCommand` still enumerates
// `None, Move, Attack, EndTurn`, and `StratSubmitSelectionCommand` gained no arm. It is not
// WRITABLE as a clause. `EStratSelectionCommand` is a plain `enum class` and not a `UENUM`, so
// there is no reflected enumerator list to read and compare; the only assertions available are
// over the enumerators' integer VALUES, which a `Build` appended after `EndTurn` would not
// disturb. A clause named for the absence while measuring only the ordering would carry a
// subject broader than what it pins, which is the failure mode this project has paid for
// repeatedly. The code change that would make it writable is stated in
// `Tools/architect/state/tests.md`; this lane does not make it.
//
// WHERE EVERY EXPECTATION COMES FROM. No hex, price, reason sentence or row count is written
// down below. The board hexes come off `FStratViewModel`; the refusal sentences are read out of
// the controller and asserted on SUBSTANCE and on being DIFFERENT FROM ONE ANOTHER, never
// against literal text; the menu-open answers are `UStratMatchSubsystem::IsProductionMenuOpen()`
// and `AStratScoreboardHUD::IsProductionMenuWidgetOpen()` read directly.
//
// THE CASE-INSENSITIVITY TRAP. `FString::operator==`, `FString::Contains` and
// `FAutomationTestBase::TestEqual` are ALL case-insensitive in this engine, measured on this
// project. Nothing below turns on a casing distinction, and the two refusal clauses pin that the
// two sentences DIFFER, which no casing accident could satisfy.
//
// NO `strat::` FREE FUNCTION IS CALLED ANYWHERE BELOW. That is `LNK2019` outside StratBridge and
// StratRules, measured 8x. This file does not include `StratBridge.h` at all.
//
// SLOT HYGIENE. One clause starts a real match, and this suite's standing rule is that any
// fixture that plays a match is a potential writer to whatever slot its config names. The config
// below names a slot of this file's own, and `bRecordCompletionOnMatchEnd` is left at its C++
// default, which is false.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
#include "StratPlayerController.h"
#include "StratScoreboardHUD.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

#include "Tests/StratProductionMenuHostProbe.h"

namespace StratProductionMenuAffordance
{
	/** This file's own slot, shared with nothing. See the header block on slot hygiene. */
	static const TCHAR* const kSlotName = TEXT("StratocracyAutomation_ProductionMenuAffordance");

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** The match configuration a live match needs. Same shape and same stated reasons as
	 *  `StratProductionMenuSeam.cpp`'s: side 0 is both `FirstSide` and `ViewingSide`, because
	 *  §2.11.5's menu is asked for the side whose screen it is. */
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
	 *  destruction order as `StratProductionMenuSeam.cpp`'s, for the reason recorded there. */
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

		FTestWorldScope(const FTestWorldScope&)            = delete;
		FTestWorldScope& operator=(const FTestWorldScope&) = delete;
	};

	/**
	 * A controller and a HUD in a transient world, and nothing that starts a match.
	 *
	 * `BeginPlay` IS NOT DISPATCHED ON EITHER ACTOR, on `StratGuidanceRouteClauses.cpp`'s stated
	 * precedent: `InitializeActorsForPlay` initialises actors without beginning play, and none of
	 * the clauses here is about who instantiated anything. In particular the controller's input
	 * setup never runs, which is why `OpenProductionMenuAction`'s binding is out of scope --
	 * stated in this file's header block rather than discovered later.
	 */
	struct FAffordanceHarness
	{
		FTestWorldScope         Scope;
		AStratPlayerController* Controller = nullptr;
		AStratScoreboardHUD*    Hud        = nullptr;
		UStratMatchSubsystem*   Match      = nullptr;

		/** @param bWithHud wires `MyHUD` to a spawned `AStratScoreboardHUD`. Left false to reach
		 *         the toggle's first refusal, which is a different sentence about a different
		 *         thing and must not be confused with the cursor one. */
		bool Build(bool bWithHud, FString& OutError)
		{
			if (Scope.World == nullptr)
			{
				OutError = TEXT("no transient world was created");
				return false;
			}

			Controller = Scope.World->SpawnActor<AStratPlayerController>();
			if (Controller == nullptr)
			{
				OutError = TEXT("AStratPlayerController did not spawn");
				return false;
			}

			if (bWithHud)
			{
				Hud = Scope.World->SpawnActor<AStratScoreboardHUD>();
				if (Hud == nullptr)
				{
					OutError = TEXT("AStratScoreboardHUD did not spawn");
					return false;
				}
				Controller->MyHUD = Hud;
			}

			Match = Scope.World->GetSubsystem<UStratMatchSubsystem>();
			if (Match == nullptr)
			{
				OutError = TEXT("the world has no match subsystem");
				return false;
			}
			return true;
		}

		/**
		 * Starts the shipped scenario.
		 *
		 * `StartMatch` RETURNING FALSE IS EXPECTED AND IS NOT AN ERROR. This fixture has no tile
		 * meshes, so the presentation half fails while the rules half succeeds -- the split
		 * `StratMatchSubsystem.h` states and `StratMatchReconcile.cpp` pins. `IsMatchLive()` is
		 * what is asserted instead.
		 */
		bool StartTheMatch(FString& OutError)
		{
			FStratMatchConfig Config;
			if (!MakeConfig(Config, OutError))
			{
				return false;
			}

			FString StartReason;
			Match->StartMatch(Config, StartReason);
			if (!Match->IsMatchLive())
			{
				OutError = FString::Printf(TEXT("no live match: %s"), *StartReason);
				return false;
			}
			return true;
		}
	};

	/** The board's own sentence about a fixture carrying no Blueprint defaults. Declared so the
	 *  line stays in the log for a human. NOTE: `Occurrences 0` means "at least one", not "zero
	 *  or more" -- measured on this suite, so only ever declare a line that fires. */
	static void ExpectTheTileMeshWarning(FAutomationTestBase& Test)
	{
		Test.AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
			EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
	}

	/** The first hex the view model knows about. ENUMERATED AND NEVER CHOSEN: a clause that
	 *  named `(0, 0)` would go quiet the day the scenario's origin moved. */
	static bool FirstBoardHex(const FStratViewModel& Model, FIntPoint& OutHex)
	{
		if (Model.Hexes.Num() == 0)
		{
			return false;
		}
		OutHex = Model.Hexes[0].Hex;
		return true;
	}
}

// ---------------------------------------------------------------------------------------------
// The latch, before anything has happened to it.
// ---------------------------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionAffordanceFreshControllerLatchesNothing,
	"Stratocracy.StratPlay.GATE-BUILDMENU.AFreshControllerLatchesNoTargetHex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * A controller that has never opened a menu answers `false`, and hands back a hex that carries
 * no meaning.
 *
 * WHAT THIS CATCHES. `bHasProductionTargetHex` defaulting to true, or a latch taken during
 * construction or `BeginPlay` -- either of which would have `WBP_ProductionMenu` build a menu
 * for a hex the player never pointed at, on the first press of every session.
 *
 * WHAT IT DOES NOT CATCH, STATED BECAUSE THE NAME COULD BE READ AS MORE. There is no headless
 * positive control for this accessor: `HexUnderCursor` cannot succeed without a `ULocalPlayer`,
 * so `GetProductionTargetHex` answers false in every state this file can reach and an
 * implementation that ALWAYS answered false would pass. The affirmative half is owed by a PIE
 * clause. See this file's header block.
 *
 * THE `OutHex` HALF IS ASSERTED FOR A NARROW REASON. `FIntPoint(0, 0)` is a real hex on this
 * board, so the value is meaningless while the first channel is false; what is pinned is that
 * the accessor does not hand back whatever happened to be in the member, which is what makes
 * "read the bool first" a contract instead of a convention. `OutHex` is seeded with a value the
 * accessor must overwrite, so a function that wrote nothing at all fails here.
 */
bool FStratProductionAffordanceFreshControllerLatchesNothing::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuAffordance;

	FAffordanceHarness H;
	FString            Error;
	if (!TestTrue(TEXT("a controller in a transient world"), H.Build(/*bWithHud*/ false, Error)))
	{
		AddError(Error);
		return false;
	}

	// A value the accessor is required to overwrite. Not a legal board hex on this scenario and
	// not the documented "no meaning" value either, so neither a silent no-op nor a coincidence
	// can produce the expected answer.
	FIntPoint Hex(-7, -7);

	TestFalse(TEXT("a controller that has opened no menu has no production target hex"),
		H.Controller->GetProductionTargetHex(Hex));
	TestEqual(TEXT("and the hex it hands back is the documented meaningless value rather than "
	               "whatever the caller passed in"),
		Hex, FIntPoint::ZeroValue);

	return true;
}

// ---------------------------------------------------------------------------------------------
// The two refusals, and the fact that they are two.
// ---------------------------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionAffordanceNoHudRefusal,
	"Stratocracy.StratPlay.GATE-BUILDMENU.AToggleWithNoScoreboardHudRefusesNamingTheHost",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * With no `AStratScoreboardHUD` to host a widget, the toggle refuses, says so in terms of the
 * HUD, and latches nothing.
 *
 * THIS IS THE FIRST GUARD IN `ToggleProductionMenu` AND IT IS NOT DECORATION. `CreateWidget` and
 * `AddToViewport` mean `UMG`, `Slate` and `SlateCore`, which `StratPlay.Build.cs` deliberately
 * does not name -- so widget creation lives on the HUD and no HUD of that class means no menu.
 * Reporting it rather than working around it is what keeps that arrow from being quietly
 * redrawn.
 *
 * THE ORDER MATTERS AND IS PINNED BY THE PAIR. This clause and the cursor clause below assert
 * that the two sentences DIFFER, which is what stops a future implementation from collapsing two
 * distinct configurations into one message the player cannot act on.
 */
bool FStratProductionAffordanceNoHudRefusal::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuAffordance;

	FAffordanceHarness H;
	FString            Error;
	if (!TestTrue(TEXT("a controller with no scoreboard HUD wired to it"),
			H.Build(/*bWithHud*/ false, Error)))
	{
		AddError(Error);
		return false;
	}

	FString Reason = TEXT("<the controller wrote nothing>");
	TestFalse(TEXT("a toggle with no AStratScoreboardHUD is refused"),
		H.Controller->ToggleProductionMenu(Reason));

	TestTrue(TEXT("the refusal names the HUD that would have hosted the menu"),
		Reason.Contains(TEXT("HUD"), ESearchCase::IgnoreCase));
	TestFalse(TEXT("and does not blame the cursor, which is a different configuration"),
		Reason.Contains(TEXT("cursor"), ESearchCase::IgnoreCase));

	FIntPoint Hex(-7, -7);
	TestFalse(TEXT("a refused toggle latched no production target hex"),
		H.Controller->GetProductionTargetHex(Hex));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionAffordanceCursorRefusal,
	"Stratocracy.StratPlay.GATE-BUILDMENU.AToggleWithABoardButNoCursorRefusesAndLatchesNothing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * With a HUD wired and a real board spawned, a toggle that cannot read a cursor is refused in
 * terms of the cursor, and latches nothing.
 *
 * THE BOARD IS THE POSITIVE CONTROL AND IT IS THE WHOLE REASON THIS CLAUSE STARTS A MATCH.
 * `HexUnderCursor` returns false for two different reasons -- no board, or no cursor hit on one
 * -- and both produce this same sentence. A fixture with no match would take the first, which
 * says nothing about the cursor arm the affordance actually lives on. So the match is started,
 * `GetBoard()` is asserted non-null, and only then is the toggle pressed: the refusal that comes
 * back is the cursor one by elimination.
 *
 * THE ONE THING THE TOGGLE REFUSES IS A CURSOR OFF THE BOARD, AND THAT IS ALSO PINNED HERE BY
 * WHAT IS ABSENT. It does not test whether the hex is a factory, whether this side holds it, or
 * whether the side can pay -- all three are rows on the menu with the rules module's own reasons
 * attached, and a pre-check here would replace the module's reason with this class's silence.
 * The refusal is therefore asserted NOT to mention a factory or affordability.
 *
 * WHAT IT DOES NOT PIN: the unwind. See this file's header block -- the toggle returns here,
 * before the latch is set and before the HUD is asked for a widget, so the false below is a
 * "never set" and not a "set and then cleared". Those are two different guarantees and only the
 * first is measured.
 */
bool FStratProductionAffordanceCursorRefusal::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuAffordance;

	ExpectTheTileMeshWarning(*this);

	FAffordanceHarness H;
	FString            Error;
	if (!TestTrue(TEXT("a controller with a scoreboard HUD wired to it"),
			H.Build(/*bWithHud*/ true, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the shipped scenario is live"), H.StartTheMatch(Error)))
	{
		AddError(Error);
		return false;
	}

	// --- THE CONTROL. Without a board, the refusal below would be the no-board one. ---
	if (!TestNotNull(TEXT("the match spawned a board, so a refusal cannot be the no-board one"),
			H.Match->GetBoard()))
	{
		return false;
	}
	// And the HUD is genuinely reachable, so the refusal cannot be the no-HUD one either.
	if (!TestNotNull(TEXT("the controller can still see its scoreboard HUD"), H.Hud))
	{
		return false;
	}

	FString Reason = TEXT("<the controller wrote nothing>");
	TestFalse(TEXT("a toggle with a board but no readable cursor is refused"),
		H.Controller->ToggleProductionMenu(Reason));

	TestTrue(TEXT("the refusal is about the cursor and the board"),
		Reason.Contains(TEXT("cursor"), ESearchCase::IgnoreCase));
	TestFalse(TEXT("and is not the no-HUD refusal, which the fixture has ruled out"),
		Reason.Contains(TEXT("HUD"), ESearchCase::IgnoreCase));

	// The three rules answers this class must never pre-empt.
	TestFalse(TEXT("the toggle does not refuse on whether the hex is a factory"),
		Reason.Contains(TEXT("factory"), ESearchCase::IgnoreCase));
	TestFalse(TEXT("nor on whether the side can pay"),
		Reason.Contains(TEXT("fame"), ESearchCase::IgnoreCase) ||
		Reason.Contains(TEXT("afford"), ESearchCase::IgnoreCase));

	FIntPoint Hex(-7, -7);
	TestFalse(TEXT("a refused toggle latched no production target hex"),
		H.Controller->GetProductionTargetHex(Hex));

	// Nothing was opened on either object.
	TestFalse(TEXT("no menu widget went up"), H.Hud->IsProductionMenuWidgetOpen());
	TestFalse(TEXT("and no rows were published"), H.Match->IsProductionMenuOpen());

	return true;
}

// ---------------------------------------------------------------------------------------------
// The division of labour a close depends on.
// ---------------------------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionAffordanceCloseIsTwoActsOnTwoObjects,
	"Stratocracy.StratPlay.GATE-BUILDMENU.TheHudCloseTakesThePanelDownAndLeavesTheRowsToItsCaller",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * `AStratScoreboardHUD::CloseProductionMenuWidget()` takes the panel down and does NOT clear
 * `UStratMatchSubsystem::ProductionMenu`. The caller's second act does.
 *
 * WHY THE NEGATIVE IS THE LOAD-BEARING HALF. Closing is two acts on two objects in a fixed order
 * -- panel down, then rows cleared -- and `AStratScoreboardHUD` deliberately does not perform the
 * second itself, because a HUD that reached into the match subsystem to clear a menu would be the
 * first line of that class becoming a second thing that runs matches, which its own header block
 * forbids in those words. A "helpful" HUD that cleared the rows would pass every other clause in
 * this file and in `StratProductionMenuSeam.cpp`; it fails here.
 *
 * THIS IS AS CLOSE AS A HEADLESS RUN GETS TO THE TOGGLE'S CLOSE ARM, and it is deliberately not
 * named as if it were that arm. The arm itself is guarded by `IsProductionMenuWidgetOpen()`,
 * which is false headlessly, so the ORDER of the two acts inside `ToggleProductionMenu` is not
 * measured here -- only that the two acts belong to two different objects, which is the property
 * the order is about. A PIE clause owes the ordering.
 *
 * EVERY "BEFORE" VALUE IS READ AND NONE IS ASSUMED. The rows are shown open before the close and
 * the panel is shown up before the close, so neither `false` afterwards can mean "there was
 * nothing there".
 */
bool FStratProductionAffordanceCloseIsTwoActsOnTwoObjects::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuAffordance;

	ExpectTheTileMeshWarning(*this);

	FAffordanceHarness H;
	FString            Error;
	if (!TestTrue(TEXT("a controller with a scoreboard HUD wired to it"),
			H.Build(/*bWithHud*/ true, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the shipped scenario is live"), H.StartTheMatch(Error)))
	{
		AddError(Error);
		return false;
	}

	// A hex off the model rather than a written-down one. It need not be a factory: a refresh at
	// a hex that is not a build point SUCCEEDS and publishes a full menu of unavailable rows --
	// that is `GATE-BUILDMENU.ANonFactoryHexOpensAnUnavailableMenuAndIsNotRefused`'s subject, and
	// this clause only needs the rows to be non-empty.
	FStratViewModel Model;
	FString         BuildError;
	if (!TestTrue(TEXT("the view model builds"), H.Match->BuildViewModel(Model, BuildError)))
	{
		AddError(BuildError);
		return false;
	}
	FIntPoint Hex;
	if (!TestTrue(TEXT("the board has at least one hex to aim a menu at"),
			FirstBoardHex(Model, Hex)))
	{
		return false;
	}

	FString RefreshReason;
	if (!TestTrue(TEXT("a refresh publishes rows for that hex"),
			H.Match->RefreshProductionMenu(Hex, RefreshReason)))
	{
		AddError(RefreshReason);
		return false;
	}

	// The panel, placed on the member the HUD's own create path assigns.
	if (!TestTrue(TEXT("a concrete menu widget is on the HUD's ProductionMenu member"),
			StratTestInstallProductionMenuDouble(H.Hud)))
	{
		return false;
	}

	// --- THE CONTROLS. Both "before" states are read, not assumed. ---
	if (!TestTrue(TEXT("the subsystem reports a menu open before the close"),
			H.Match->IsProductionMenuOpen()))
	{
		return false;
	}
	if (!TestTrue(TEXT("and the HUD is holding a panel before the close"),
			StratTestHasProductionMenuWidget(H.Hud)))
	{
		return false;
	}

	// --- ACT ONE, on the HUD alone. ---
	H.Hud->CloseProductionMenuWidget();

	TestFalse(TEXT("the HUD dropped its panel"), StratTestHasProductionMenuWidget(H.Hud));
	TestFalse(TEXT("and reports no menu widget open"), H.Hud->IsProductionMenuWidgetOpen());

	// --- THE ASSERTION THIS CLAUSE EXISTS FOR. ---
	TestTrue(TEXT("the HUD's close did NOT clear the subsystem's rows -- that is its caller's "
	              "second act on its second object, and a HUD that did it itself would be a HUD "
	              "that runs matches"),
		H.Match->IsProductionMenuOpen());

	// --- ACT TWO, on the subsystem, which is what `AStratPlayerController::ToggleProductionMenu`
	//     performs after the panel is down. ---
	H.Match->CloseProductionMenu();

	TestFalse(TEXT("with both acts performed, the subsystem reports no menu open"),
		H.Match->IsProductionMenuOpen());
	TestFalse(TEXT("and the HUD still reports no menu widget open"),
		H.Hud->IsProductionMenuWidgetOpen());

	return true;
}
