// Wave 3 -- §2.11.5's header purse AT THE SUBSYSTEM SEAM. GDD §2.11.5, §2.11.2, §4.7 Stub 2.
//
// WHAT THIS FILE'S SUBJECT IS. `UStratMatchSubsystem::ProductionMenuFameTotal`, the member a
// §2.11.5 header binds to, and the one property that makes it safe to bind: it carries the
// purse the OPEN menu's rows were priced against, and it is zero exactly while there is no
// open menu.
//
// WHY IT MUST LIVE IN StratPlay. `UStratMatchSubsystem` is a StratPlay class and StratUI
// cannot see it. The view-model half of the same wave's purse work --
// `StratBuildProductionMenu`'s six-argument overload and its two clauses -- is in
// `Source/StratUI/Tests/StratProductionPurseClauses.cpp` and is not restated here.
//
// WHY `T-UI-04`. The number is §2.11.5's `Fame: 250` header and T-UI-04's own sentence names
// `current fameTotal` as half of what the production menu binds to. No ID is invented.
//
// WHERE THE EXPECTATION COMES FROM, AND IT IS NEVER A FIGURE TYPED HERE. Every non-zero
// expectation below is `StratBuildProductionMenu`'s six-argument `OutPurseFame`, asked of the
// subsystem's OWN live bridge for the subsystem's OWN viewing side at the subsystem's OWN menu
// hex, in the same frame, with no command submitted in between. That is the module's answer
// routed through the same function the subsystem itself called -- a value read, not computed.
// The only literal in an expectation position is the zero a CLOSED panel must publish, and
// that zero is the contract's own word: `CloseProductionMenu` sets it and
// `StratMatchSubsystem.h` says the member is "MEANINGLESS WHILE `IsProductionMenuOpen()` IS
// FALSE".
//
// THE BICONDITIONAL HAS TEETH ONLY IF THE OPEN PURSE IS NON-ZERO, so the clause establishes
// that first, off the live board, and fails loudly naming the fixture if the seeded side ever
// starts broke. It does not pass quietly on a board where zero and the purse coincide.
//
// THE BUILD LEG IS THE ONE THAT CATCHES A STALE PURSE. `SubmitProductionChoice` spends Fame
// and then refreshes the menu at the same hex, so the panel stays open and the header MUST
// have moved. A member assigned once and never re-read would satisfy the open leg and the
// closed leg and fail only here.
//
// NOT ASSERTED HERE, and named rather than left silent:
//  - `AStratPlayerController::RequestEndTurn`. §2.11.2's End Turn surface has no acceptance ID
//    covering it and this lane does not mint one; see `Tools/architect/state/tests.md`.
//  - Any widget binding. Nothing below constructs Slate.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

// Included from a .cpp and never from a UHT-parsed header.
#include "StratBridge.h"

namespace StratProductionPurseSeam
{
	/** This file's own slot, shared with nothing. */
	static const TCHAR* const kSlotName = TEXT("StratocracyAutomation_ProductionPurseSeam");

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

	/** Side 0 is both `FirstSide` and `ViewingSide`: §2.11.5's menu is asked for the side whose
	 *  screen it is, and this clause's whole subject is a build that side may make. */
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

	/** The transient world, same shape and destruction order as `StratProductionMenuSeam.cpp`'s. */
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

	/** A live match in a transient world. `StartMatch` returning false is expected here -- this
	 *  fixture has no tile meshes -- so `IsMatchLive()` is what is asserted. */
	struct FSeamHarness
	{
		FTestWorldScope       Scope;
		UStratMatchSubsystem* Subsystem = nullptr;
		FStratBridge*         Bridge    = nullptr;
		FString               StartReason;

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

	/** The board's own sentence about a fixture carrying no Blueprint defaults. */
	static void ExpectTheTileMeshWarning(FAutomationTestBase& Test)
	{
		Test.AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
			EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
	}

	/** The hex of a factory the viewing side holds, read off the model and never named. */
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

	/**
	 * THE EXPECTATION SIDE: the purse the module hands the very function the subsystem calls.
	 *
	 * Asked of the subsystem's own live bridge, for the subsystem's own viewing side, at the
	 * subsystem's own open hex -- so any difference from `ProductionMenuFameTotal` is the
	 * seam's doing and nothing else's.
	 */
	static bool AskTheModelForThePurse(const UStratMatchSubsystem& Subsystem, const FStratBridge& Bridge,
	                                   FIntPoint Hex, int32& OutPurse, FString& OutError)
	{
		TArray<FStratBuildOptionView> Rows;
		return StratBuildProductionMenu(
			Bridge, Subsystem.GetViewingSide(), Hex, Rows, OutPurse, OutError);
	}

	/** A row this factory will actually take, found off the open menu and never written down. */
	static bool FindABuildableRow(const TArray<FStratBuildOptionView>& Rows, int32& OutDefIndex)
	{
		for (const FStratBuildOptionView& Row : Rows)
		{
			if (Row.bAvailable && Row.bAffordable && Row.CostFame > 0)
			{
				OutDefIndex = Row.DefIndex;
				return true;
			}
		}
		return false;
	}
}

// ---------------------------------------------------------------------------
// The clause -- the published header purse is the open menu's purse, and zero exactly when
// there is no open menu.
//
// FOUR STATIONS OF ONE CYCLE, and the member is checked at every one of them:
//   1. BEFORE any menu -- closed, so zero, while the board's real purse is not zero.
//   2. OPEN at a held factory -- the module's purse for that menu, non-zero.
//   3. AFTER A BUILD -- the menu is rebuilt at the same hex and stays open, so the header must
//      have MOVED to the new purse. A member assigned once and left stale passes stations 1, 2
//      and 4 and fails only here.
//   4. CLOSED again -- zero, while the board's real purse is still not zero. That last half is
//      what makes the zero a CLEAR rather than a coincidence.
//
// `IsProductionMenuOpen()` IS THE DISCRIMINATOR AT EVERY STATION, asked of the object rather
// than assumed from the call that preceded it. `TestTrue` is a claim, not a question, and a
// clause that assumed the panel's state from its own last call would report the wrong subject
// when a refusal it did not expect intervened.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuFameTotalRidesTheMenuTest,
	"Stratocracy.StratPlay.T-UI-04.PublishedHeaderPurseIsTheOpenMenusPurseAndZeroWhenClosed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratProductionMenuFameTotalRidesTheMenuTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionPurseSeam;

	DeleteOurSlot();
	ON_SCOPE_EXIT { DeleteOurSlot(); };

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("the shipped tables load"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	ExpectTheTileMeshWarning(*this);

	FSeamHarness H;
	if (!H.Arm(*this, Config))
	{
		return false;
	}

	// ---- station 1: no menu has ever been opened ----
	TestFalse(TEXT("a fresh match has no open production menu"), H.Subsystem->IsProductionMenuOpen());
	TestEqual(TEXT("a closed panel publishes a zero header purse"),
		H.Subsystem->ProductionMenuFameTotal, 0);

	FStratViewModel Model;
	if (!TestTrue(TEXT("the subsystem builds a view model"), H.Subsystem->BuildViewModel(Model, Error)))
	{
		AddError(Error);
		return false;
	}

	FIntPoint FactoryHex = FIntPoint::ZeroValue;
	if (!TestTrue(TEXT("the seeded board gives the viewing side a factory to open a menu at"),
			FindHeldFactory(Model, H.Subsystem->GetViewingSide(), FactoryHex)))
	{
		return false;
	}

	// THE TEETH FOR STATION 1's ZERO: the board's real purse is not zero, so the published zero
	// is a statement about the closed panel and not a copy of the truth.
	{
		int32   PurseWhileClosed = 0;
		FString PurseError;
		if (!TestTrue(TEXT("the module prices a menu at that factory"),
				AskTheModelForThePurse(*H.Subsystem, *H.Bridge, FactoryHex, PurseWhileClosed, PurseError)))
		{
			AddError(PurseError);
			return false;
		}
		if (!TestTrue(
				*FString::Printf(
					TEXT("the seeded viewing side starts with a NON-ZERO purse (%d), so zero-when-closed can fail"),
					PurseWhileClosed),
				PurseWhileClosed > 0))
		{
			return false;
		}
	}

	// ---- station 2: the menu is open ----
	if (!TestTrue(TEXT("the menu opens at the held factory"),
			H.Subsystem->RefreshProductionMenu(FactoryHex, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the panel reports itself open"), H.Subsystem->IsProductionMenuOpen()))
	{
		return false;
	}

	int32   PurseWhileOpen = 0;
	FString OpenPurseError;
	if (!TestTrue(TEXT("the module prices the open menu"),
			AskTheModelForThePurse(*H.Subsystem, *H.Bridge, FactoryHex, PurseWhileOpen, OpenPurseError)))
	{
		AddError(OpenPurseError);
		return false;
	}
	TestEqual(TEXT("an open panel publishes the purse its own rows were priced against"),
		H.Subsystem->ProductionMenuFameTotal, PurseWhileOpen);
	TestTrue(
		*FString::Printf(TEXT("that published purse is non-zero (%d), so it is distinguishable from a closed panel's"),
			H.Subsystem->ProductionMenuFameTotal),
		H.Subsystem->ProductionMenuFameTotal != 0);

	// ---- station 3: a build spends Fame and the panel stays open ----
	const int32 PurseBeforeTheBuild = H.Subsystem->ProductionMenuFameTotal;

	int32 DefIndex = INDEX_NONE;
	if (!TestTrue(TEXT("the open menu offers a row this factory will take"),
			FindABuildableRow(H.Subsystem->ProductionMenu, DefIndex)))
	{
		return false;
	}

	if (!TestTrue(TEXT("the build is accepted and the menu rebuilds"),
			H.Subsystem->SubmitProductionChoice(DefIndex, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the panel is still open after a build"), H.Subsystem->IsProductionMenuOpen()))
	{
		return false;
	}

	int32   PurseAfterTheBuild = 0;
	FString RebuiltPurseError;
	if (!TestTrue(TEXT("the module prices the rebuilt menu"),
			AskTheModelForThePurse(*H.Subsystem, *H.Bridge, FactoryHex, PurseAfterTheBuild, RebuiltPurseError)))
	{
		AddError(RebuiltPurseError);
		return false;
	}

	// A STALE MEMBER DIES HERE. Both sides of this comparison are measured; neither is
	// arithmetic performed by this file.
	TestNotEqual(
		*FString::Printf(TEXT("spending Fame moved the module's purse (%d then %d), so a stale header is visible"),
			PurseBeforeTheBuild, PurseAfterTheBuild),
		PurseAfterTheBuild, PurseBeforeTheBuild);
	TestEqual(TEXT("the published header purse followed the rebuilt rows rather than staying stale"),
		H.Subsystem->ProductionMenuFameTotal, PurseAfterTheBuild);

	// ---- station 4: closed again ----
	H.Subsystem->CloseProductionMenu();
	TestFalse(TEXT("the panel reports itself closed"), H.Subsystem->IsProductionMenuOpen());
	TestEqual(TEXT("closing clears the header purse to zero"),
		H.Subsystem->ProductionMenuFameTotal, 0);

	// AND THE ZERO IS STILL A CLEAR AND NOT THE TRUTH: the side is not broke.
	{
		int32   PurseAfterClose = 0;
		FString ClosedPurseError;
		if (!TestTrue(TEXT("the module still prices a menu at that factory after the panel closed"),
				AskTheModelForThePurse(*H.Subsystem, *H.Bridge, FactoryHex, PurseAfterClose, ClosedPurseError)))
		{
			AddError(ClosedPurseError);
			return false;
		}
		TestTrue(
			*FString::Printf(
				TEXT("the side still holds Fame (%d) while the closed panel publishes zero"), PurseAfterClose),
			PurseAfterClose > 0);
	}

	return true;
}
