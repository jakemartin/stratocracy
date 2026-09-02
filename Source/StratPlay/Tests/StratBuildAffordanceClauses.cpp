// The gate on §2.11.5's FOCUSED-FACTORY LATCH and the two controller verbs a BUILD button and
// an EXIT button call. GDD §2.11.5, §2.11.2, §2.8. Acceptance ID T-UI-03.
//
// WHY THESE CLAUSES RIDE T-UI-03, ruled by the user for this pass. Each states its own stretch
// in its own name, on `T-SCN-07`'s precedent for a shared ID.
//
// WHAT THIS FILE'S SUBJECT IS. `FStratBuildAffordance` (`Source/StratPlay/StratBuildAffordance.h`)
// -- a plain struct holding a hex, a bool and two observation fields -- and the three
// `AStratPlayerController` verbs that grew around it: `OpenProductionMenuAtFocusedFactory`,
// `CloseProductionMenu`, and the shared `OpenProductionMenuAtHex` they and
// `ToggleProductionMenu` all run through. The OUTPUT side -- what
// `StratDecorateCommandBar` writes onto `FStratCommandBarView` -- is
// `Source/StratUI/Tests/StratCommandBarClauses.cpp`. Only the hand-over clause below reaches
// both, and it lives here because StratPlay is the module that can see the latch at all.
//
// ================================================================================
// THIS FILE CLOSES A GAP `StratProductionMenuAffordance.cpp` DECLARES IN ITS OWN HEADER.
// ================================================================================
// That file states, in terms, that `AStratPlayerController::HexUnderCursor` "returns false in
// EVERY state reachable below" because `GetHitResultUnderCursor` needs a `ULocalPlayer` a
// headless `-nullrhi` run does not have, and lists three consequences: no positive control for
// the latch, an UNREACHABLE unwind branch, and an unreachable close arm. Two of those three are
// reachable through the verbs added on 2026-09-02, and they are reachable BECAUSE those verbs
// take no cursor:
//   - `OpenProductionMenuAtFocusedFactory` resolves its hex from the latch, so the open path
//     runs headlessly right up to the HUD. `NeedsNoCursor` below measures exactly that, by
//     contrast with `ToggleProductionMenu` refused at the cursor in the same frame.
//   - The UNWIND is therefore reachable too, and is asserted there. What is pinned is that a
//     refused open leaves no latch behind; what is still NOT pinned is that the latch was ever
//     SET, because nothing observable distinguishes "unwound" from "never written". That half
//     is still owed by a PIE clause and is recorded in `Tools/architect/state/tests.md`.
// The third -- `ToggleProductionMenu`'s own close arm, gated by `IsProductionMenuWidgetOpen()`
// which is false headlessly -- is STILL unreachable and no clause below claims it. What is
// pinned instead is `CloseProductionMenu`, the verb that arm now forwards to, called directly.
//
// WHERE EVERY EXPECTATION COMES FROM. No hex, side index, price or row count is written down.
// The board hexes and factories are enumerated out of `FStratViewModel` built by the live
// subsystem; the refusal sentences are asserted on SUBSTANCE (which noun they name) and on being
// DIFFERENT FROM ONE ANOTHER, never against literal text; the menu-open answers are
// `UStratMatchSubsystem::IsProductionMenuOpen()` and the probe's read of the HUD's own member.
// The hand-built models in the clear-points clause are INPUTS: the expectation side of every
// assertion over them is the behaviour `StratBuildAffordance.h`'s own clear-point table states,
// which is this pass's specification.
//
// THE CASE-INSENSITIVITY TRAP. `FString::operator==`, `Contains` and `TestEqual` are ALL
// case-insensitive in this engine, measured on this project. Nothing below turns on a casing
// distinction, and every refusal pair is asserted to DIFFER, which no casing accident satisfies.
//
// NO `strat::` FREE FUNCTION IS CALLED ANYWHERE BELOW -- `LNK2019` outside StratBridge and
// StratRules, measured 8x. This file does not include `StratBridge.h`.
//
// SLOT HYGIENE. Two clauses start a real match, and this suite's standing rule is that any
// fixture that plays a match is a potential writer to whatever slot its config names. The config
// below names a slot of this file's own; an UNSET `FStratMatchConfig::SaveSlotName` resolves to
// the PLAYER'S REAL SAVE SLOT, which is an open finding in the record.
//
// NOT ASSERTED HERE, WITH REASONS:
// - THE NEUTRAL-FACTORY HAZARD. `NoteHexPrimary`'s declaration records that
//   `Owner == Model.ViewingSide` latches a NEUTRAL factory on a model whose `ViewingSide` is
//   ALSO `INDEX_NONE`, and that `StratBuildViewModel` refuses to produce one. Pinning that
//   would pin a HAZARD rather than a REQUIREMENT and would go RED on its own fix. The
//   requirement -- a neutral factory does not latch on a model with a real viewing side -- is
//   a leg of the clear-points clause below.
// - `OpenProductionMenuAction`'s BINDING, and the WBP that hosts the button. Asset and PIE
//   gates, not headless ones.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Templates/Function.h"
#include "Templates/SharedPointer.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/WeakObjectPtr.h"

#include "StratBoardActor.h"
#include "StratBuildAffordance.h"
#include "StratMatchSubsystem.h"
#include "StratPlayerController.h"
#include "StratScoreboardHUD.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

#include "Tests/StratProductionMenuHostProbe.h"

namespace StratBuildAffordanceClauses
{
	/** This file's own slot, shared with nothing. See the header block on slot hygiene. */
	static const TCHAR* const kSlotName = TEXT("StratocracyAutomation_BuildAffordance");

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** The match configuration a live match needs. Same shape and same stated reasons as
	 *  `StratProductionMenuAffordance.cpp`'s: side 0 is both `FirstSide` and `ViewingSide`,
	 *  because §2.11.5's menu is asked for the side whose screen it is. */
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
	 *  destruction order as `StratProductionMenuAffordance.cpp`'s. */
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
	 * A controller with a HUD wired to it, and optionally a live match.
	 *
	 * `BeginPlay` IS NOT DISPATCHED ON THE CONTROLLER OR THE HUD, on
	 * `StratProductionMenuAffordance.cpp`'s stated precedent -- none of the clauses here is
	 * about who instantiated anything, and the controller's input setup is out of scope for a
	 * headless run in any case.
	 *
	 * `StartMatch` RETURNING FALSE IS EXPECTED AND IS NOT AN ERROR. This fixture has no tile
	 * meshes, so the presentation half fails while the rules half succeeds -- the split
	 * `StratMatchSubsystem.h` states and `StratMatchReconcile.cpp` pins. `IsMatchLive()` is
	 * asserted instead.
	 */
	struct FHarness
	{
		FTestWorldScope         Scope;
		AStratPlayerController* Controller = nullptr;
		AStratScoreboardHUD*    Hud        = nullptr;
		UStratMatchSubsystem*   Match      = nullptr;

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

	/** The board's own sentence about a fixture carrying no Blueprint defaults. NOTE:
	 *  `Occurrences 0` means "at least one", not "zero or more" -- measured on this suite. */
	static void ExpectTheTileMeshWarning(FAutomationTestBase& Test)
	{
		Test.AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
			EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
	}

	/** The hex of a factory `Side` holds. ENUMERATED FROM THE MODEL AND NEVER CHOSEN, on
	 *  `StratProductionMenuSeam.cpp`'s rule. */
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

	// -----------------------------------------------------------------------------------------
	// The hand-built models the clear-point clause drives.
	//
	// HAND-BUILT ON PURPOSE, AND THE STRUCT WAS DESIGNED FOR IT. `StratBuildAffordance.h`'s own
	// block states that taking the hex and the model as ARGUMENTS is what makes the latch
	// "drivable with no actor, no world, no PIE and no cursor". These models are INPUTS; the
	// expectation side of every assertion is the clear-point table in that header.
	//
	// SIDE INDICES ARE THIS HARNESS'S OWN AND NAME NO RULE. Two sides and one factory is the
	// smallest model on which "mine", "theirs" and "nobody's" are three different answers.
	// -----------------------------------------------------------------------------------------

	static const int32 kSeat      = 0;
	static const int32 kOtherSeat = 1;

	/** A model with one factory at `FactoryHex` owned by `FactoryOwner`. */
	static FStratViewModel MakeModel(
		int32     ViewingSide,
		int32     Turn,
		bool      bHasResult,
		FIntPoint FactoryHex,
		int32     FactoryOwner)
	{
		FStratViewModel Model;
		Model.ViewingSide      = ViewingSide;
		Model.Match.Turn       = Turn;
		Model.Match.SideToMove = ViewingSide;
		Model.Match.bHasResult = bHasResult;

		FStratFactoryView Factory;
		Factory.Hex   = FactoryHex;
		Factory.Owner = FactoryOwner;
		Model.Factories.Add(Factory);

		return Model;
	}

	/** The factory hex these models use, and a hex no factory claims. Neither is a coordinate
	 *  of the shipped scenario and neither is compared against one -- they are two distinct
	 *  values of the harness's own, which is all the latch needs. */
	static const FIntPoint kFactoryHex(3, 4);
	static const FIntPoint kPlainHex(6, 1);
}

// ---------------------------------------------------------------------------------------------
// Every clear point `StratBuildAffordance.h` states, one leg each -- INCLUDING the one that is
// stated as NOT a clear point.
//
// THE HEADER'S TABLE IS THE SPECIFICATION. It names seven clears (a click elsewhere, a cancel, a
// turn change, a seat change, a concluded match, a reset, and the else arm of `NoteHexPrimary`
// itself) and one deliberate NON-clear (the production menu closing), and it says why the
// non-clear is "a decision rather than an omission": the exit button returns the player to the
// factory they were focused on, and clearing there would make exit behave as a cancel, which is
// a different gesture the player already has.
//
// A FRESH `FStratBuildAffordance` PER LEG. Sequencing them through one instance would make each
// leg's precondition depend on the leg before it, and a failure in the middle would then
// misattribute every leg after it.
//
// THE HEADER ALSO SAYS THESE ARE BELT AND BRACES OVER THE RECOMPUTE AND NOT LOAD-BEARING FOR
// CORRECTNESS, and that is why this clause is worth having anyway: it pins the STATE, so
// `HasFocus()` is not quietly true about a factory the player lost three turns ago. What the
// SCREEN does under a stale latch is the hand-over clause below and
// `StratCommandBarClauses.cpp`'s ownership clause; they are different subjects.
// ---------------------------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBuildAffordanceClearPointsTest,
	"Stratocracy.StratPlay.T-UI-03.BuildAffordanceClearsOnEveryStatedClearPoint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBuildAffordanceClearPointsTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBuildAffordanceClauses;

	const FStratViewModel Mine =
		MakeModel(kSeat, /*Turn*/ 3, /*bHasResult*/ false, kFactoryHex, /*FactoryOwner*/ kSeat);

	// A latch armed the way the running game arms one: `Observe` first (the refresh path calls
	// it every frame), then the click.
	const auto Arm = [this, &Mine](FStratBuildAffordance& A) -> bool
	{
		A.Observe(Mine);
		A.NoteHexPrimary(kFactoryHex, Mine);
		return TestTrue(TEXT("precondition: a factory this seat owns is focused"), A.HasFocus());
	};

	// ---- the else arm: a click on a hex holding no factory ---------------------------------
	{
		FStratBuildAffordance A;
		if (!Arm(A)) { return false; }
		const bool bChanged = A.NoteHexPrimary(kPlainHex, Mine);
		TestFalse(TEXT("T-UI-03: a primary click on a hex with no factory clears the focus -- "
		               "one arm, both directions, so 'the player clicked somewhere else' is a "
		               "clear with no clear-point code"), A.HasFocus());
		TestTrue(TEXT("T-UI-03: and reports that it changed the state"), bChanged);
	}

	// ---- the else arm: a click on the enemy's factory --------------------------------------
	{
		const FStratViewModel Theirs =
			MakeModel(kSeat, 3, false, kFactoryHex, /*FactoryOwner*/ kOtherSeat);
		FStratBuildAffordance A;
		if (!Arm(A)) { return false; }
		TestTrue(TEXT("T-UI-03: a click on a factory the OTHER seat owns clears the focus"),
			A.NoteHexPrimary(kFactoryHex, Theirs));
		TestFalse(TEXT("T-UI-03: and leaves nothing focused"), A.HasFocus());
	}

	// ---- the else arm: a click on a NEUTRAL factory ----------------------------------------
	//
	// THE REQUIREMENT AND NOT THE HAZARD. `ViewingSide` here is a real side, which is the only
	// model `StratBuildViewModel` can produce. See this file's header block on why the
	// `INDEX_NONE == INDEX_NONE` case is deliberately absent.
	{
		const FStratViewModel Neutral =
			MakeModel(kSeat, 3, false, kFactoryHex, /*FactoryOwner*/ INDEX_NONE);
		FStratBuildAffordance A;
		if (!Arm(A)) { return false; }
		A.NoteHexPrimary(kFactoryHex, Neutral);
		TestFalse(TEXT("T-UI-03: a NEUTRAL factory does not latch on a model with a real "
		               "viewing side -- `Owner == ViewingSide`, not `Owner != INDEX_NONE`"),
			A.HasFocus());
	}

	// ---- §2.8: a concluded match refuses to latch and clears what was latched ---------------
	{
		const FStratViewModel Finished =
			MakeModel(kSeat, 3, /*bHasResult*/ true, kFactoryHex, kSeat);
		FStratBuildAffordance A;
		if (!Arm(A)) { return false; }
		A.NoteHexPrimary(kFactoryHex, Finished);
		TestFalse(TEXT("T-UI-03: §2.8's finished match refuses to latch even on a factory this "
		               "seat owns -- a BUILD control on a final board would advertise a control "
		               "guaranteed to refuse"), A.HasFocus());

		FStratBuildAffordance Fresh;
		Fresh.Observe(Finished);
		Fresh.NoteHexPrimary(kFactoryHex, Finished);
		TestFalse(TEXT("T-UI-03: and refuses from a standing start too"), Fresh.HasFocus());
	}

	// ---- §2.11.1's cancel -------------------------------------------------------------------
	{
		FStratBuildAffordance A;
		if (!Arm(A)) { return false; }
		TestTrue(TEXT("T-UI-03: ClearFocus reports that it changed the state"), A.ClearFocus());
		TestFalse(TEXT("T-UI-03: a cancel drops the focus"), A.HasFocus());

		// The coordinate goes with it, so a stale hex cannot survive behind a false flag.
		FIntPoint Read(-7, -7);
		TestFalse(TEXT("T-UI-03: and the accessor says so"), A.GetFocusedHex(Read));
		TestEqual(TEXT("T-UI-03: handing back the documented meaningless value rather than "
		               "whatever the caller passed in"), Read, FIntPoint::ZeroValue);
		TestFalse(TEXT("T-UI-03: a second cancel changes nothing and says so"), A.ClearFocus());
	}

	// ---- a turn change, which is EVERY AI turn as well as the player's ----------------------
	{
		const FStratViewModel NextTurn = MakeModel(kSeat, /*Turn*/ 4, false, kFactoryHex, kSeat);
		FStratBuildAffordance A;
		if (!Arm(A)) { return false; }
		TestTrue(TEXT("T-UI-03: Observe reports the clear it made"), A.Observe(NextTurn));
		TestFalse(TEXT("T-UI-03: a turn change clears the focus"), A.HasFocus());
	}

	// ---- a hot-seat hand-over ---------------------------------------------------------------
	{
		const FStratViewModel OtherSeatModel =
			MakeModel(/*ViewingSide*/ kOtherSeat, /*Turn*/ 3, false, kFactoryHex, kSeat);
		FStratBuildAffordance A;
		if (!Arm(A)) { return false; }
		TestTrue(TEXT("T-UI-03: Observe reports the clear it made"), A.Observe(OtherSeatModel));
		TestFalse(TEXT("T-UI-03: a viewing-side change clears the focus, on the SAME turn -- "
		               "the seat arm is not redundant with the turn arm"), A.HasFocus());
	}

	// ---- a match that concluded under the latch ---------------------------------------------
	{
		const FStratViewModel Finished =
			MakeModel(kSeat, /*Turn*/ 3, /*bHasResult*/ true, kFactoryHex, kSeat);
		FStratBuildAffordance A;
		if (!Arm(A)) { return false; }
		TestTrue(TEXT("T-UI-03: Observe reports the clear it made"), A.Observe(Finished));
		TestFalse(TEXT("T-UI-03: a concluded match clears the focus on the same turn and the "
		               "same seat -- neither clock moved"), A.HasFocus());
	}

	// ---- the row that is NOT a clear point: a gated or refused End Turn ---------------------
	//
	// Nothing advanced. `Observe` keys on the turn the MODEL reports, which is what makes a
	// gated end turn -- which advances no turn -- correctly not clear.
	{
		FStratBuildAffordance A;
		if (!Arm(A)) { return false; }
		TestFalse(TEXT("T-UI-03: Observe on an unchanged model reports no change"),
			A.Observe(Mine));
		TestTrue(TEXT("T-UI-03: a gated or refused End Turn advances no turn and therefore does "
		              "NOT clear the focus"), A.HasFocus());
	}

	// ---- the first observation records and clears nothing ------------------------------------
	{
		FStratBuildAffordance A;
		TestFalse(TEXT("T-UI-03: the FIRST Observe clears nothing -- there is nothing latched on "
		              "the frame before anything has been clicked, and treating it as a change "
		              "would be a clear with no cause"), A.Observe(Mine));
		A.NoteHexPrimary(kFactoryHex, Mine);
		TestFalse(TEXT("T-UI-03: and a second Observe of the same model still clears nothing"),
			A.Observe(Mine));
		TestTrue(TEXT("T-UI-03: so the focus taken after the first observation survives it"),
			A.HasFocus());
	}

	// ---- Reset clears the focus AND the observation fields -----------------------------------
	//
	// THE OBSERVATION HALF IS WHAT DISTINGUISHES `Reset` FROM `ClearFocus`, and it is measured
	// rather than asserted about: after a reset, an `Observe` of a model on a DIFFERENT turn
	// must behave as a FIRST observation and clear nothing. Under a `Reset` that had left the
	// clocks alone it would read as a turn change instead.
	{
		FStratBuildAffordance A;
		if (!Arm(A)) { return false; }
		A.Reset();
		TestFalse(TEXT("T-UI-03: Reset drops the focus"), A.HasFocus());

		const FStratViewModel Reseeded =
			MakeModel(kSeat, /*Turn*/ 1, false, kFactoryHex, kSeat);
		TestFalse(TEXT("T-UI-03: and drops the recorded turn and seat with it, so the first "
		              "Observe of the reseeded match sees no spurious change"),
			A.Observe(Reseeded));
		A.NoteHexPrimary(kFactoryHex, Reseeded);
		TestTrue(TEXT("T-UI-03: leaving the new match's first focus standing"), A.HasFocus());
	}

	// ---- the stated NON-clear: closing the production menu -----------------------------------
	//
	// THIS LEG NEEDS THE CONTROLLER, because the affordance struct has no close path at all --
	// which is itself the point. `CloseProductionMenu` is the only close verb, and what is
	// pinned is that it does not reach the latch.
	{
		FHarness H;
		FString  Error;
		if (!TestTrue(TEXT("a controller with a scoreboard HUD wired to it"),
				H.Build(/*bWithHud*/ true, Error)))
		{
			AddError(Error);
			return false;
		}

		H.Controller->GetBuildAffordance().Observe(Mine);
		H.Controller->GetBuildAffordance().NoteHexPrimary(kFactoryHex, Mine);
		if (!TestTrue(TEXT("precondition: the controller's latch holds a factory"),
				H.Controller->GetBuildAffordance().HasFocus()))
		{
			return false;
		}

		FString CloseReason = TEXT("<the controller wrote nothing>");
		TestTrue(TEXT("closing a menu that is not open succeeds -- an exit button pressed twice "
		              "is not an error"), H.Controller->CloseProductionMenu(CloseReason));

		TestTrue(TEXT("T-UI-03: the production menu closing does NOT clear the focus. This is a "
		              "DECISION and not an omission: exit returns the player to the factory they "
		              "were focused on with BUILD still on screen, and clearing here would make "
		              "exit behave as a cancel -- a different gesture the player already has"),
			H.Controller->GetBuildAffordance().HasFocus());

		FIntPoint Still(-7, -7);
		TestTrue(TEXT("T-UI-03: and the hex survives with it"),
			H.Controller->GetBuildAffordance().GetFocusedHex(Still));
		TestEqual(TEXT("T-UI-03: unchanged"), Still, kFactoryHex);
	}

	return true;
}

// ---------------------------------------------------------------------------------------------
// A BUILD button opens the menu with no cursor, in the very frame the key cannot.
//
// WHAT THIS CLOSES, AND IT IS NAMED RATHER THAN IMPLIED.
// `Source/StratPlay/Tests/StratProductionMenuAffordance.cpp`'s header declares a third of its
// subject untestable because `AStratPlayerController::HexUnderCursor` "returns false in EVERY
// state reachable" headlessly -- `GetHitResultUnderCursor` needs a `ULocalPlayer` with a
// viewport client, and a transient world has neither. That is exactly why
// `FStratBuildAffordance` exists: a button click puts the cursor on the button and off the
// board, so a second, cursor-free source of "which factory" had to exist. This clause measures
// that the second source works, and it measures it BY CONTRAST IN ONE FRAME -- the same
// controller, the same live match, the same board -- so the difference is attributable to the
// hex source and to nothing about the fixture.
//
// THE THREE REFUSALS ARE ASSERTED TO BE THREE. `no factory is focused`, `the cursor is not on
// the board` and the HUD's `no ProductionMenuWidgetClass...` are different sentences about
// different configurations, and each is asserted on the noun it names rather than on its text.
// A future implementation that collapsed any two into one message the player cannot act on
// fails here.
//
// THE MATCH IS STARTED, AND THAT IS THE POSITIVE CONTROL FOR THE CURSOR HALF. `HexUnderCursor`
// answers false for two different reasons -- no board, or no cursor hit on one -- and both
// produce the same sentence. With `GetBoard()` asserted non-null, the refusal that comes back
// is the cursor one by elimination. `StratProductionMenuAffordance.cpp`'s cursor clause takes
// the same precaution for the same reason.
//
// WHAT IT DOES NOT PIN, STATED BECAUSE THE NAME COULD BE READ AS MORE. The open still cannot
// SUCCEED headlessly: `AStratScoreboardHUD::CreateProductionMenuWidget` calls `CreateWidget`,
// and `UUserWidget::CreateWidgetInstance` refuses a controller that is not a LOCAL player
// controller -- measured and recorded in `StratProductionMenuHostProbe.cpp`. So the furthest
// this reaches is the HUD's own refusal, and what that proves is that hex resolution SUCCEEDED
// with no cursor in existence. That the latched hex is the FOCUSED one, rather than some other
// hex, is not observable here: the refusal unwinds the latch, so `GetProductionTargetHex` is
// false either way. That half is owed by a PIE clause.
// ---------------------------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBuildAffordanceOpenNeedsNoCursorTest,
	"Stratocracy.StratPlay.T-UI-03.OpenProductionMenuAtFocusedFactoryNeedsNoCursor",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBuildAffordanceOpenNeedsNoCursorTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBuildAffordanceClauses;

	ExpectTheTileMeshWarning(*this);

	FHarness H;
	FString  Error;
	if (!TestTrue(TEXT("a controller with a scoreboard HUD wired to it"),
			H.Build(/*bWithHud*/ true, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the shipped scenario starts"), H.StartTheMatch(Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestNotNull(TEXT("the live match spawned a board, so a cursor refusal below is about "
	                      "the CURSOR and not about a missing board"), H.Match->GetBoard()))
	{
		return false;
	}

	// ---- nothing focused: the button's own refusal ------------------------------------------
	FString NoFocus = TEXT("<the controller wrote nothing>");
	TestFalse(TEXT("T-UI-03: with no factory focused, the BUILD verb refuses"),
		H.Controller->OpenProductionMenuAtFocusedFactory(NoFocus));
	TestTrue(TEXT("T-UI-03: and names the FOCUS as the thing that is missing"),
		NoFocus.Contains(TEXT("focus"), ESearchCase::IgnoreCase));
	TestFalse(TEXT("T-UI-03: not the cursor, which this verb never reads"),
		NoFocus.Contains(TEXT("cursor"), ESearchCase::IgnoreCase));
	TestFalse(TEXT("T-UI-03: and not the HUD, which it has not reached yet"),
		NoFocus.Contains(TEXT("HUD"), ESearchCase::IgnoreCase));

	// ---- the key path, in the same frame, is stopped by the cursor --------------------------
	FString KeyPath = TEXT("<the controller wrote nothing>");
	TestFalse(TEXT("the key path refuses in this same frame"),
		H.Controller->ToggleProductionMenu(KeyPath));
	if (!TestTrue(TEXT("T-UI-03: and it refuses AT THE CURSOR -- which is the whole reason a "
	                   "cursor-free hex source had to exist for a button"),
			KeyPath.Contains(TEXT("cursor"), ESearchCase::IgnoreCase)))
	{
		AddError(FString::Printf(TEXT("the key path said: %s"), *KeyPath));
		return false;
	}

	// ---- latch a factory the way a primary click does ---------------------------------------
	FStratViewModel Model;
	FString         BuildError;
	if (!TestTrue(TEXT("the live match builds a view model"),
			H.Match->BuildViewModel(Model, BuildError)))
	{
		AddError(BuildError);
		return false;
	}

	FIntPoint FactoryHex;
	if (!TestTrue(TEXT("the shipped scenario gives the viewing side a factory to focus"),
			FindHeldFactory(Model, Model.ViewingSide, FactoryHex)))
	{
		AddError(TEXT("no factory in the view model is owned by the viewing side; this clause ")
			TEXT("has no case to measure and must not pass"));
		return false;
	}

	H.Controller->GetBuildAffordance().Observe(Model);
	H.Controller->GetBuildAffordance().NoteHexPrimary(FactoryHex, Model);
	if (!TestTrue(TEXT("precondition: the click latched the factory"),
			H.Controller->GetBuildAffordance().HasFocus()))
	{
		return false;
	}
	AddInfo(FString::Printf(TEXT("focused factory: hex (%d, %d) on the live board"),
		FactoryHex.X, FactoryHex.Y));

	// ---- the button path now gets PAST hex resolution, with no cursor in existence -----------
	FString Focused = TEXT("<the controller wrote nothing>");
	TestFalse(TEXT("the open is still refused headlessly, because CreateWidget needs a LOCAL "
	              "player controller -- see this clause's block"),
		H.Controller->OpenProductionMenuAtFocusedFactory(Focused));
	TestTrue(TEXT("T-UI-03: with a factory focused, the BUILD verb resolves its hex WITHOUT A "
	              "CURSOR and reaches the HUD -- the refusal is now the HUD's own, about a "
	              "Blueprint default, and no longer about a focus or a cursor"),
		Focused.Contains(TEXT("ProductionMenuWidgetClass"), ESearchCase::IgnoreCase));
	TestFalse(TEXT("T-UI-03: it does not blame the cursor -- this verb reads none"),
		Focused.Contains(TEXT("cursor"), ESearchCase::IgnoreCase));
	TestFalse(TEXT("T-UI-03: nor the focus, which it has"),
		Focused.Contains(TEXT("focus"), ESearchCase::IgnoreCase));

	// ---- the three sentences are three ------------------------------------------------------
	TestFalse(TEXT("T-UI-03: the no-focus refusal and the cursor refusal are different sentences"),
		NoFocus.Equals(KeyPath, ESearchCase::CaseSensitive));
	TestFalse(TEXT("T-UI-03: and so are the cursor refusal and the HUD's"),
		KeyPath.Equals(Focused, ESearchCase::CaseSensitive));
	TestFalse(TEXT("T-UI-03: and the no-focus refusal and the HUD's"),
		NoFocus.Equals(Focused, ESearchCase::CaseSensitive));

	// ---- the unwind, which the key path could not reach --------------------------------------
	FIntPoint Latched(-7, -7);
	TestFalse(TEXT("T-UI-03: a refused open leaves NO production target hex behind claiming a "
	              "menu is about a factory. This is the unwind branch "
	              "StratProductionMenuAffordance.cpp's header calls UNREACHABLE from the key "
	              "path -- it is reachable from the focus path because that path resolves a hex"),
		H.Controller->GetProductionTargetHex(Latched));
	TestEqual(TEXT("T-UI-03: and hands back the documented meaningless value"),
		Latched, FIntPoint::ZeroValue);

	// ---- and the focus itself is untouched by a refused open ---------------------------------
	TestTrue(TEXT("T-UI-03: a refused open does not clear the focus -- the player is still "
	              "pointed at the factory they chose"),
		H.Controller->GetBuildAffordance().HasFocus());

	return true;
}

// ---------------------------------------------------------------------------------------------
// EXIT takes the panel down BEFORE it clears the rows, and is idempotent.
//
// AN ORDER IS NOT VISIBLE IN THE STATE THAT FOLLOWS IT, which is why this clause needs a moment
// rather than an aftermath. `AStratPlayerController::CloseProductionMenu`'s declaration calls
// the order the contract: the panel comes down and THEN the rows are cleared, because the
// reverse "leaves a live panel bound to an empty `ProductionMenu` array for however long the two
// lines are apart, which is a screen showing a menu the subsystem says is not open". Both orders
// leave the identical final state, so a clause that only inspected the aftermath would pass over
// the defective one -- and, named for the order, would carry a subject broader than what it
// pinned.
//
// HOW THE MOMENT IS OBTAINED. `UStratProductionMenuOrderDouble`
// (`Source/StratUI/Tests/`) overrides `UWidget::RemoveFromParent`, which is the one virtual
// `AStratScoreboardHUD::CloseProductionMenuWidget` calls, and fires a hook this clause supplies.
// The hook reads `UStratMatchSubsystem::IsProductionMenuOpen()` -- THE SUBSYSTEM'S OWN ANSWER,
// not a recording made by the double -- at that instant. If the rows were cleared first, the
// hook sees them already gone and this clause reddens.
//
// THE DOUBLE SUPPLIES A MOMENT AND NEVER AN EXPECTATION, which is what keeps
// `StratProductionMenuHostDouble.h`'s "a double that kept its own bookkeeping" hazard out of
// reach: every value compared is the subsystem's or the HUD's.
//
// WHY THE DOUBLE IS INSTALLED RATHER THAN OPENED. `CreateWidget` refuses a controller that is
// not a LOCAL player controller, measured and recorded in `StratProductionMenuHostProbe.cpp`, so
// no headless clause can reach the HUD's own success path. The probe writes the same member
// `CreateProductionMenuWidget` writes; what differs is only the `AddToViewport` a headless run
// has no viewport for, and nothing below reads `IsInViewport()`.
// ---------------------------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBuildAffordanceCloseOrderTest,
	"Stratocracy.StratPlay.T-UI-03.CloseProductionMenuTakesThePanelDownBeforeClearingTheRows",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBuildAffordanceCloseOrderTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBuildAffordanceClauses;

	ExpectTheTileMeshWarning(*this);

	FHarness H;
	FString  Error;
	if (!TestTrue(TEXT("a controller with a scoreboard HUD wired to it"),
			H.Build(/*bWithHud*/ true, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the shipped scenario starts"), H.StartTheMatch(Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- a menu with real rows in it ---------------------------------------------------------
	FStratViewModel Model;
	FString         BuildError;
	if (!TestTrue(TEXT("the live match builds a view model"),
			H.Match->BuildViewModel(Model, BuildError)))
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
	if (!TestTrue(TEXT("the subsystem publishes rows for that factory"),
			H.Match->RefreshProductionMenu(FactoryHex, RefreshReason)))
	{
		AddError(RefreshReason);
		return false;
	}
	if (!TestTrue(TEXT("precondition: the subsystem reports the menu open"),
			H.Match->IsProductionMenuOpen()))
	{
		return false;
	}

	// ---- the moment ---------------------------------------------------------------------------
	//
	// CAPTURED WEAKLY AND BY SHARED POINTER, so that a hook surviving into fixture teardown --
	// `AStratScoreboardHUD::EndPlay` calls `CloseProductionMenuWidget` unguarded -- cannot touch
	// a destroyed subsystem or a dead stack frame. The hook is cleared explicitly below in any
	// case; this is belt and braces over that, not a substitute for it.
	struct FObservation
	{
		int32 Fired                   = 0;
		bool  bRowsPresentAtPanelDown = false;
		int32 RowCountAtPanelDown     = 0;
	};
	const TSharedRef<FObservation> Seen = MakeShared<FObservation>();

	const TWeakObjectPtr<UStratMatchSubsystem> WeakMatch(H.Match);

	if (!TestTrue(TEXT("the order-observing double installs on the HUD"),
			StratTestInstallProductionMenuOrderDouble(H.Hud,
				[Seen, WeakMatch]()
				{
					++Seen->Fired;
					if (const UStratMatchSubsystem* const Live = WeakMatch.Get())
					{
						Seen->bRowsPresentAtPanelDown = Live->IsProductionMenuOpen();
						Seen->RowCountAtPanelDown     = Live->ProductionMenu.Num();
					}
				})))
	{
		return false;
	}
	if (!TestTrue(TEXT("precondition: the HUD holds a menu widget"),
			StratTestHasProductionMenuWidget(H.Hud)))
	{
		return false;
	}

	// ---- exit --------------------------------------------------------------------------------
	FString CloseReason = TEXT("<the controller wrote nothing>");
	TestTrue(TEXT("the exit verb succeeds"), H.Controller->CloseProductionMenu(CloseReason));

	AddInfo(FString::Printf(
		TEXT("panel-down fired %d time(s); at that instant the subsystem reported the menu open "
		     "= %d, with %d row(s)"),
		Seen->Fired, Seen->bRowsPresentAtPanelDown ? 1 : 0, Seen->RowCountAtPanelDown));

	if (!TestEqual(TEXT("the panel came down exactly once"), Seen->Fired, 1))
	{
		StratTestClearProductionMenuOrderHook(H.Hud);
		return false;
	}

	TestTrue(TEXT("T-UI-03: at the instant the PANEL came down, the subsystem still held its "
	              "ROWS -- so the panel goes down FIRST. The reverse order would leave a live "
	              "panel bound to an empty row array, which is a screen showing a menu the "
	              "subsystem says is not open"),
		Seen->bRowsPresentAtPanelDown);
	TestTrue(TEXT("T-UI-03: and the rows were really there, not an empty array reporting open"),
		Seen->RowCountAtPanelDown > 0);

	// ---- and both halves did land -------------------------------------------------------------
	TestFalse(TEXT("T-UI-03: the panel is down afterwards"),
		StratTestHasProductionMenuWidget(H.Hud));
	TestFalse(TEXT("T-UI-03: and the rows are cleared afterwards"),
		H.Match->IsProductionMenuOpen());
	TestEqual(TEXT("T-UI-03: with nothing left in the array"),
		H.Match->ProductionMenu.Num(), 0);

	// ---- idempotence: an exit button pressed twice is not an error ----------------------------
	FString SecondReason = TEXT("<the controller wrote nothing>");
	TestTrue(TEXT("T-UI-03: a second exit succeeds"),
		H.Controller->CloseProductionMenu(SecondReason));
	TestEqual(TEXT("T-UI-03: and takes no second panel down, because there was none"),
		Seen->Fired, 1);
	TestFalse(TEXT("T-UI-03: leaving the rows cleared"), H.Match->IsProductionMenuOpen());

	// The hook has told this clause everything it can. Dropped before teardown so that no
	// automation code runs on the HUD's own `EndPlay` path.
	StratTestClearProductionMenuOrderHook(H.Hud);

	return true;
}

// ---------------------------------------------------------------------------------------------
// A hot-seat hand-over leaves no BUILD control on the incoming player's screen -- twice over.
//
// TWICE OVER IS THE SUBJECT. `StratBuildAffordance.h` states that the clear points are "BELT AND
// BRACES OVER THE RECOMPUTE, NOT LOAD-BEARING FOR CORRECTNESS", and that saying which is which
// is the point. This clause measures both independently and in that order:
//   1. THE RECOMPUTE ALONE. A latch taken on seat 0's screen, decorated onto seat 1's model with
//      NO `Observe` in between -- the state is deliberately stale -- draws nothing, because
//      `StratDecorateCommandBar` re-resolves the hex against `Factories` and `ViewingSide` every
//      refresh. This is the half that makes staleness structurally impossible.
//   2. THE CLEAR. `Observe` on the same hand-over then drops the focus, so `HasFocus()` is not
//      quietly true about a factory on a screen that is no longer this player's.
// A clause that measured only the second would report the design's guarantee as resting on a
// clear-point list, which is precisely what the header says it does not rest on.
//
// THE FACTORY DOES NOT CHANGE HANDS ANYWHERE BELOW. The same factory, owned by seat 0
// throughout; only the SEAT looking at it moves. So the answers are attributable to
// `ViewingSide` and to nothing about ownership -- which is a different clause, in
// `Source/StratUI/Tests/StratCommandBarClauses.cpp`.
//
// IT LIVES IN THIS MODULE BECAUSE IT NEEDS BOTH SIDES. `FStratBuildAffordance` is StratPlay's
// and `StratDecorateCommandBar` is StratUI's; StratPlay depends on StratUI and not the reverse,
// so this is the only directory that can call both. In StratUI it would be `LNK2019`.
// ---------------------------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBuildAffordanceHandoverTest,
	"Stratocracy.StratPlay.T-UI-03.CommandBarSurvivesNoStaleFocusAcrossAHandover",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBuildAffordanceHandoverTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBuildAffordanceClauses;

	// Seat 0's screen, turn 3. The factory is seat 0's and stays seat 0's.
	FStratViewModel Seat0 = MakeModel(kSeat, /*Turn*/ 3, false, kFactoryHex, /*Owner*/ kSeat);

	FStratBuildAffordance Affordance;
	Affordance.Observe(Seat0);
	Affordance.NoteHexPrimary(kFactoryHex, Seat0);
	if (!TestTrue(TEXT("precondition: seat 0 focused its own factory"), Affordance.HasFocus()))
	{
		return false;
	}

	Affordance.DecorateViewModel(Seat0);
	if (!TestTrue(TEXT("precondition: seat 0's screen shows the BUILD control"),
			Seat0.CommandBar.bShowBuildButton))
	{
		return false;
	}
	TestEqual(TEXT("precondition: about the factory that was clicked"),
		Seat0.CommandBar.BuildFactoryHex, kFactoryHex);

	// ---- (1) the recompute alone, with the state deliberately left stale ---------------------
	//
	// NO `Observe` HERE. The latch still holds seat 0's factory and still says so -- asserted
	// below -- so what draws nothing is the decorator's re-resolution and nothing else.
	FStratViewModel Seat1 =
		MakeModel(/*ViewingSide*/ kOtherSeat, /*Turn*/ 4, false, kFactoryHex, /*Owner*/ kSeat);
	Affordance.DecorateViewModel(Seat1);

	TestTrue(TEXT("the latch is deliberately still set at this point, so the answer below is the "
	              "decorator's and not a clear's"), Affordance.HasFocus());
	TestFalse(TEXT("T-UI-03: a STALE focus draws no BUILD control on the incoming seat's screen. "
	              "The bit is recomputed against Factories and ViewingSide on every decorate, "
	              "which is what makes staleness structurally impossible rather than dependent "
	              "on remembering every clear point"),
		Seat1.CommandBar.bShowBuildButton);
	TestEqual(TEXT("T-UI-03: and the hex field carries the documented meaningless value"),
		Seat1.CommandBar.BuildFactoryHex, FIntPoint::ZeroValue);

	// ---- (2) the belt and braces: the hand-over clears the state too -------------------------
	TestTrue(TEXT("T-UI-03: Observe reports the clear the hand-over caused"),
		Affordance.Observe(Seat1));
	TestFalse(TEXT("T-UI-03: so HasFocus() is not quietly true about a factory on a screen that "
	              "is no longer this player's"), Affordance.HasFocus());

	// ---- and the seat coming back gets a clean bar, not the old one --------------------------
	FStratViewModel Seat0Again =
		MakeModel(kSeat, /*Turn*/ 5, false, kFactoryHex, /*Owner*/ kSeat);
	Affordance.Observe(Seat0Again);
	Affordance.DecorateViewModel(Seat0Again);
	TestFalse(TEXT("T-UI-03: seat 0 returning does not get its old BUILD control back -- the "
	              "focus is an input intent and there is no history, no stack and no previous "
	              "focus in this struct"),
		Seat0Again.CommandBar.bShowBuildButton);

	// ---- the positive control: the same seat can focus again ---------------------------------
	Affordance.NoteHexPrimary(kFactoryHex, Seat0Again);
	Affordance.DecorateViewModel(Seat0Again);
	TestTrue(TEXT("positive control: one click brings it back, so the three negatives above are "
	              "the hand-over's doing and not a latch that had been permanently spent"),
		Seat0Again.CommandBar.bShowBuildButton);

	return true;
}
