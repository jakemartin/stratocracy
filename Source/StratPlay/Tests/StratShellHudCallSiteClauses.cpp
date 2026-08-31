// The gate on §2.11.5's shell HUD CALL SITE -- `AStratShellHUD::ResolveMenuTiming` -- under the
// local gate name `GATE-TITLEMENU`.
//
// WHAT THIS FILE CLOSES, AND IT IS A GAP THIS PROJECT WROTE DOWN IN THREE PLACES FIRST.
// `StratShellMenuTimingClauses.cpp` pins `DecideMenuTiming`, the world-free decider, and its own
// header says in terms what it does not reach: *"Nothing pins that
// `AStratShellHUD::ResolveMenuTiming` ACTUALLY CONSULTS `DecideMenuTiming`. These clauses pin the
// decider. They do not pin the CALL SITE. A `ResolveMenuTiming` rewritten to create the menu
// unconditionally would leave every clause below GREEN."* `tests.md`, `global.md` and the commit
// message that carried them all repeat it. This file spawns the HUD and reddens on exactly that.
//
// WHY IT WAS THOUGHT UNREACHABLE, AND WHY THAT WAS WRONG. The gap was recorded as needing "a
// PIE-shaped fixture and not a headless one". That claim was never measured -- it was inferred
// from the fact that the arm ends in `CreateWidget` and `AddToViewport`. A probe measured it
// instead, and the inference was half wrong: THE DECISION IS REACHABLE HEADLESSLY AND ONLY THE
// DRAWING IS NOT. The record has been corrected rather than left standing.
//
// WHAT THE PROBE MEASURED, because every line of the fixture below rests on it:
//   - `UWorld::CreateWorld(EWorldType::Game)` plus an engine world context: fine, and it is the
//     same `FTestWorldScope` shape `StratBoardPicking.cpp` and `StratScoreboardHUDSeam.cpp`
//     already use.
//   - `UGameInstance::InitializeStandalone` gives an instance whose SUBSYSTEM COLLECTION IS
//     LIVE. `GetSubsystem<UStratShellSubsystem>()` returns null before that call and a real
//     object after it, and `UWorld::SetGameInstance` then makes it reachable from the spawned
//     HUD through `AActor::GetGameInstance`. This is the piece the "PIE-shaped" claim assumed
//     was impossible.
//   - `AActor::DispatchBeginPlay` on the spawned HUD reaches `ResolveMenuTiming` for real. No
//     `InitializeActorsForPlay`, no `UWorld::BeginPlay`, no PIE.
//   - `CreateWidget` RETURNS NULL for every class this fixture can legally reach, because the
//     spawned controller is not a LOCAL player controller. MEASURED WITH A CONTROL RATHER THAN
//     ASSUMED OF ONE CLASS: a native `UUserWidget` and the shipped `WBP_TitleMenu_C` -- which
//     loads from its package perfectly well, so the failure is not a missing asset -- fail
//     identically. **The consequence is that this fixture needs no `/Game/` path at all.**
//     `Tests/` is one of CLAUDE.md's two standing exceptions to the no-literal rule and this
//     file declines to use it, because the exception would buy nothing here.
//
// SO WHAT IS ASSERTED IS THE ARM TAKEN, NOT THE MENU DRAWN, and the discriminator is
// `ConfigurationTicksWaited` -- 1 when the wait arm ran, 0 when the create arm ran. That single
// integer is what separates the two paths through the call site, and it is why the property
// exists on the class at all.
//
// THE CREATE ARM COSTS TWO EXPECTED-MESSAGE DECLARATIONS, AND THEY ARE NOT THE SAME KIND OF
// THING. The distinction is written out because a reader who treats them alike will either
// weaken the clause or be baffled by it.
//   - MINE, AT EXACTLY ONE OCCURRENCE, IS AN ASSERTION. `CreateMenuWidget` cannot succeed in a
//     viewport-less run, so it logs one Error that this module owns. Declaring it at exactly one
//     occurrence makes it positive evidence in both directions: zero occurrences means the call
//     site never entered the create arm and the clause reddens for a MISSING match; two means it
//     created twice and the clause reddens for a surplus. `UStratShellSubsystem` already uses
//     this shape for its refusal clause, so it is the house idiom rather than a new one.
//   - THE ENGINE'S IS A SUPPRESSION, AND IT IS LABELLED AS ONE. `CreateWidget` refuses a
//     controller that is not a LOCAL player controller, and says so at Error. That is a fact
//     about a headless fixture and not about this project's code, so it is declared with an
//     any-count occurrence rather than pinned. Pinning an engine message would make this clause
//     fail on an engine upgrade that reworded it, which would be a finding about nothing.
//
// WHY THE FIXTURE DOES NOT SIMPLY MAKE THE CREATE SUCCEED, since that would be stronger and was
// tried. Three routes were measured and all three were rejected on evidence:
//   - A LOCAL PLAYER. `UGameInstance::CreateInitialPlayer` does produce one -- and trips a
//     HANDLED ENSURE on `IsDedicatedServerInstance()`, which arrives as a callstack full of
//     Errors. A fixture that trips an engine ensure is not a fixture worth having, whatever it
//     buys. (With that local player, `CreateWidget` on the shipped `WBP_TitleMenu_C` DOES
//     succeed -- so the create arm is reachable in principle, and the ensure is the only thing
//     standing in the way.)
//   - A CONCRETE NATIVE WIDGET CLASS declared in this file, to dodge the engine's
//     "Abstract, Deprecated or Replaced classes are not allowed to be used to construct a user
//     widget" refusal of `UUserWidget` itself. UHT does not parse `.cpp` files, so a `UCLASS`
//     here does not compile -- measured as `fatal error C1083` on a `.generated.h` that cannot
//     exist. It would need a header under `Tests/`, and it would still not make the controller
//     local.
//   - THE SHIPPED `WBP_TitleMenu_C`, which loads from its package perfectly well. Without a
//     local player it fails exactly as the native class does, so it buys nothing and would spend
//     a `/Game/` literal and a dependency on an asset another lane owns.
// **The consequence, stated plainly: this file pins the ARM TAKEN and never the menu drawn.**
//
// WHAT THIS FILE STILL DOES NOT REACH, and the list is now short and honest:
//   - THE MENU EVER APPEARING. No viewport, so no widget, so `IsMenuLive()` is false on every
//     path here and no clause below asserts otherwise. Only a real PIE session can see a menu,
//     and a screenshot is not a test.
//   - THE RE-ARMING TIMER FIRING. The wait arm arms a next-tick timer on a world that never
//     ticks. That the timer was ARMED is not observable; that the arm was TAKEN is, and that is
//     what is asserted.
//   - `ApplyMenuInputMode`. It runs only after a successful create, which cannot happen here.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#include "StratShellHUD.h"
#include "StratShellSubsystem.h"

namespace StratShellHudCallSite
{
	/**
	 * A world, a game instance with a live subsystem collection, a player controller and a HUD.
	 *
	 * THE TEARDOWN ORDER IS THE PROJECT'S MEASURED ONE: world first, world context second.
	 * `StratBoardPicking.cpp` records why -- tearing the context down first leaves
	 * `UWorld::DestroyActor` warning "World has no context!" once per actor, and a gate that
	 * leaves warnings in a green run trains the next reader to skim warnings.
	 */
	struct FShellHudScope
	{
		UWorld*             World    = nullptr;
		UGameInstance*      Instance = nullptr;
		APlayerController*  Player   = nullptr;
		AStratShellHUD*     Hud      = nullptr;

		/** Empty when the fixture stood up; otherwise the first thing that went wrong. */
		FString Failure;

		FShellHudScope()
		{
			World = UWorld::CreateWorld(EWorldType::Game, /*bInformEngineOfWorld*/ false);
			if (World == nullptr)
			{
				Failure = TEXT("UWorld::CreateWorld returned null");
				return;
			}

			if (GEngine != nullptr)
			{
				FWorldContext& Context = GEngine->CreateNewWorldContext(EWorldType::Game);
				Context.SetCurrentWorld(World);
			}

			// THE ONE NON-OBVIOUS LINE IN THIS FIXTURE. A bare `NewObject<UGameInstance>` has no
			// subsystem collection -- measured: `GetSubsystem<UStratShellSubsystem>()` returns
			// null on one. `InitializeStandalone` is what stands the collection up, and it is
			// the reason the call site is reachable without PIE at all.
			Instance = NewObject<UGameInstance>(GEngine);
			if (Instance == nullptr)
			{
				Failure = TEXT("could not allocate a UGameInstance");
				return;
			}
			Instance->InitializeStandalone(TEXT("StratShellHudCallSite"), nullptr);
			World->SetGameInstance(Instance);

			Player = World->SpawnActor<APlayerController>();
			if (Player == nullptr)
			{
				Failure = TEXT("could not spawn an APlayerController to own the HUD");
				return;
			}

			Hud = World->SpawnActor<AStratShellHUD>();
			if (Hud == nullptr)
			{
				Failure = TEXT("could not spawn an AStratShellHUD");
				return;
			}

			// `AHUD::PlayerOwner` is what `GetOwningPlayerController` reads. Set directly
			// because nothing here runs `APlayerController::SpawnDefaultHUD`.
			Hud->PlayerOwner = Player;
		}

		~FShellHudScope()
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

		UStratShellSubsystem* Shell() const
		{
			return (Instance != nullptr)
				? Instance->GetSubsystem<UStratShellSubsystem>() : nullptr;
		}

		FShellHudScope(const FShellHudScope&) = delete;
		FShellHudScope& operator=(const FShellHudScope&) = delete;
	};

	/**
	 * The message `CreateMenuWidget` logs when the create arm is entered in a viewport-less run.
	 *
	 * DERIVED FROM THE CLASS UNDER TEST RATHER THAN RETYPED WHERE IT CAN BE: the widget class
	 * name comes from the same `UClass` the fixture assigns, so a renamed class moves the
	 * expectation with it. The stem is the one literal, and it is the smallest one that can
	 * identify the arm.
	 */
	FString CreateFailureMessageFor(const UClass* MenuClass)
	{
		return FString::Printf(TEXT("CreateWidget returned null for MenuWidgetClass '%s'"),
			*GetNameSafe(MenuClass));
	}
}

// ---------------------------------------------------------------------------------------
// THE CLAUSE THE GAP WAS ABOUT.
//
// An unconfigured shell must make the spawned HUD WAIT. Three observations together say the
// wait arm ran and the create arm did not: the tick counter advanced to 1, no menu exists, and
// `LastFailureReason` is EMPTY -- the last being the load-bearing one, because a
// `ResolveMenuTiming` that created unconditionally would have entered `CreateMenuWidget`,
// failed there for want of a viewport, and left a reason behind.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellHudWaitsForAnUnconfiguredShellTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.AnUnconfiguredShellMakesTheSpawnedHudWaitInsteadOfDrawing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellHudWaitsForAnUnconfiguredShellTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellHudCallSite;

	FShellHudScope Scope;
	if (!Scope.Failure.IsEmpty())
	{
		AddError(Scope.Failure + TEXT("; this clause has observed nothing and will not report "
			"that as agreement."));
		return false;
	}

	// THE PREMISE, ASSERTED RATHER THAN ARRANGED AND ASSUMED. If the shell were already
	// configured -- or absent -- the clause below would be measuring a different world than the
	// one it claims to.
	UStratShellSubsystem* const Shell = Scope.Shell();
	if (!TestNotNull(TEXT("premise: the standalone game instance carries a live shell subsystem"),
			Shell))
	{
		return false;
	}
	if (!TestFalse(TEXT("premise: nothing has configured the shell in this world"),
			Shell->HasMatchDestinationBeenConfigured()))
	{
		return false;
	}

	// A NON-NULL MENU CLASS, OR `BeginPlay` RETURNS BEFORE THE DECIDER IS EVER CONSULTED. The
	// class is the native one on purpose: the probe measured that no class can be instantiated
	// without a viewport, so reaching for the shipped asset would buy nothing and would spend a
	// `/Game/` literal this file does not need.
	Scope.Hud->MenuWidgetClass = UUserWidget::StaticClass();

	Scope.Hud->DispatchBeginPlay();

	TestEqual(TEXT("the HUD took the wait arm exactly once -- ConfigurationTicksWaited is the "
		"discriminator between the two paths through ResolveMenuTiming"),
		Scope.Hud->ConfigurationTicksWaited, 1);

	TestFalse(TEXT("no menu was created for an unconfigured shell"),
		Scope.Hud->IsMenuLive());

	TestFalse(TEXT("the HUD does not claim it drew on a configured shell"),
		Scope.Hud->bMenuDrewOnAConfiguredShell);

	// THE ONE THAT KILLS THE 'CREATES UNCONDITIONALLY' REWRITE. Entering the create arm in this
	// fixture cannot succeed, so it always leaves a reason behind. An empty reason is therefore
	// positive evidence that the arm was never entered.
	TestTrue(FString::Printf(
		TEXT("CreateMenuWidget was never called, so no failure reason was recorded -- read: "
			"'%s'"), *Scope.Hud->LastFailureReason),
		Scope.Hud->LastFailureReason.IsEmpty());

	return true;
}

// ---------------------------------------------------------------------------------------
// THE OTHER SIDE OF THE SAME LINKAGE.
//
// A configured shell must make the HUD ask for the menu at once: no wait, and `CreateMenuWidget`
// entered EXACTLY ONCE. The second half is asserted by the expected-message declaration, which
// is an assertion in both directions -- a missing message and a duplicated one both redden.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellHudDrawsFromAConfiguredShellTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.AConfiguredShellMakesTheSpawnedHudAskForItsMenuAtOnce",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellHudDrawsFromAConfiguredShellTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellHudCallSite;

	FShellHudScope Scope;
	if (!Scope.Failure.IsEmpty())
	{
		AddError(Scope.Failure + TEXT("; this clause has observed nothing and will not report "
			"that as agreement."));
		return false;
	}

	UStratShellSubsystem* const Shell = Scope.Shell();
	if (!TestNotNull(TEXT("premise: the standalone game instance carries a live shell subsystem"),
			Shell))
	{
		return false;
	}

	// CONFIGURED WITH NOTHING, deliberately: a null level and an empty slot. The HUD's wait is
	// about whether the call HAPPENED, never about whether it carried anything, and configuring
	// with real values here would leave that untested at the call site too.
	Shell->ConfigureMatchDestination(TSoftObjectPtr<UWorld>(), FString());
	if (!TestTrue(TEXT("premise: the shell now reports configured"),
			Shell->HasMatchDestinationBeenConfigured()))
	{
		return false;
	}

	UClass* const MenuClass = UUserWidget::StaticClass();
	Scope.Hud->MenuWidgetClass = MenuClass;

	// THE ASSERTION. Declared BEFORE the act, at EXACTLY ONE occurrence: zero means
	// `CreateMenuWidget` was never reached and this clause reddens for a missing match; two means
	// the call site created twice and it reddens for a surplus. This message belongs to
	// `StratShellHUD.cpp`, which is why it can be pinned this tightly.
	AddExpectedError(CreateFailureMessageFor(MenuClass),
		EAutomationExpectedErrorFlags::Contains, /*Occurrences*/ 1);

	// THE SUPPRESSION, AND IT IS DELIBERATELY NOT AN ASSERTION. `CreateWidget` refuses a
	// controller that is not a LOCAL player controller and says so at Error. That is a fact about
	// a headless fixture, not about this project's code, so it is declared at ANY count rather
	// than pinned -- an engine upgrade that reworded it would otherwise redden this clause and
	// the finding would be about nothing. The header records the three routes to a genuinely
	// successful create that were measured and rejected.
	AddExpectedError(TEXT("Only Local Player Controllers can be assigned to widgets"),
		EAutomationExpectedErrorFlags::Contains, /*Occurrences*/ 0);

	Scope.Hud->DispatchBeginPlay();

	TestEqual(TEXT("the HUD did not wait at all for a shell that was already configured"),
		Scope.Hud->ConfigurationTicksWaited, 0);

	// THE CREATE ARM WAS ENTERED AND COULD NOT SUCCEED -- no viewport exists in a headless run,
	// measured with both a native widget class and the shipped Blueprint one. So this asserts
	// the ATTEMPT, which is the half that belongs to the call site, and says nothing about a
	// menu ever appearing, which is the half that does not.
	TestFalse(FString::Printf(
		TEXT("entering the create arm left a reason behind -- read: '%s'"),
		*Scope.Hud->LastFailureReason),
		Scope.Hud->LastFailureReason.IsEmpty());

	TestFalse(TEXT("no menu is live, because no viewport exists in a headless run -- this "
		"clause pins the arm taken, never the menu drawn"),
		Scope.Hud->IsMenuLive());

	return true;
}

// ---------------------------------------------------------------------------------------
// THE EARLY RETURN, which is the third path through `BeginPlay` and the cheapest to get wrong.
//
// A HUD whose Blueprint sets no menu class must consult nothing and arm nothing: unset is a
// CONFIGURATION and not an error, and a map that draws no menu must cost no timer callbacks.
// The shell is left configured here so that a `BeginPlay` which had lost its early return would
// take the create arm and be caught by the reason it left behind.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellHudWithNoMenuClassArmsNothingTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.AHudWithNoMenuClassConsultsTheShellNotAtAll",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellHudWithNoMenuClassArmsNothingTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShellHudCallSite;

	FShellHudScope Scope;
	if (!Scope.Failure.IsEmpty())
	{
		AddError(Scope.Failure + TEXT("; this clause has observed nothing and will not report "
			"that as agreement."));
		return false;
	}

	UStratShellSubsystem* const Shell = Scope.Shell();
	if (!TestNotNull(TEXT("premise: the standalone game instance carries a live shell subsystem"),
			Shell))
	{
		return false;
	}

	// CONFIGURED ON PURPOSE. With the shell unconfigured, a lost early return would land on the
	// WAIT arm and look almost like this one; with it configured, a lost early return lands on
	// the CREATE arm and leaves a reason, which the last assertion below catches.
	Shell->ConfigureMatchDestination(TSoftObjectPtr<UWorld>(), FString());

	// THE SUBJECT: no menu class at all.
	Scope.Hud->MenuWidgetClass = nullptr;

	Scope.Hud->DispatchBeginPlay();

	TestEqual(TEXT("a HUD with no menu class never entered the wait arm"),
		Scope.Hud->ConfigurationTicksWaited, 0);

	TestFalse(TEXT("a HUD with no menu class created no menu"), Scope.Hud->IsMenuLive());

	// THE REASON IS THE UNSET ONE AND NOT A CREATE FAILURE. Both are non-empty, so the
	// distinction has to be made on content: this asserts the arm by naming the property that
	// was unset, and a create failure -- which names `CreateWidget` -- would not match.
	TestTrue(FString::Printf(
		TEXT("the recorded reason is the unset-class configuration, not a create failure -- "
			"read: '%s'"), *Scope.Hud->LastFailureReason),
		Scope.Hud->LastFailureReason.Contains(TEXT("no MenuWidgetClass is set")));

	return true;
}
