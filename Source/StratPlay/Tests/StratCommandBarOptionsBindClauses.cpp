// `UStratMatchSubsystem::EnsureCommandBarOptionsBinding` -- the in-match entry point onto the
// `Options` route, under `GATE-TITLEMENU`.
//
// THE ID IS RULED AND NOT CHOSEN HERE. `Tools/architect/state/global.md`'s third acceptance-ID
// ruling of 2026-09-05: *"The button's whole job, by its own header, is reaching the `Options`
// route from a second surface -- a match, rather than the title screen -- and `GATE-TITLEMENU`'s
// subject is the ROUTE, not the screen that requests it ... It does not sit in a seam between
// two IDs; it sits inside one of them, because the fact that decides it -- which route it
// reaches -- has nothing to do with which widget class asks."* The engineer's proposed
// `T-UI-05.CommandBarOptions*` is refused there on evidence: `T-UI-05`'s subject is the
// `+X/turn` income line (`decisions.md:849`), and a route-request button is not an income
// projection. No name is minted.
//
// ---------------------------------------------------------------------------------------
// THE DEFECT THIS FILE EXISTS FOR, AND IT IS A SILENT ONE.
//
// `AddDynamic` ON A DYNAMIC MULTICAST DOES NOT DEDUPLICATE. `EnsureCommandBarOptionsBinding` is
// called from `ApplyView`, which runs on EVERY refresh of the match, so an unguarded
// `AddDynamic` accumulates one binding per refresh -- and a single click then runs
// `HandleCommandBarOptionsRequested` once per elapsed refresh. That subsystem's own declaration
// names it: *"the handler would run once per elapsed refresh on a single click."* Nothing about
// the screen would look wrong; the symptom is an options route opened dozens of times, a click
// cue emitted dozens of times, and a request counter running away.
//
// AND THE MIRROR DEFECT IS AS BAD AND LOOKS LIKE THE FIX. A bind placed in `Initialize` or in
// `OnWorldBeginPlay` would run exactly once and would MISS THE BAR ENTIRELY, because the HUD
// creates the command bar after the subsystem exists. The declaration argues that too: a bind
// on `ApplyView` is *"RECONCILED rather than evented -- it repairs itself the frame after a new
// widget appears"*. So this file's two clauses are the two halves of that: bound at most once
// however many refreshes elapse, AND bound at all when the bar appears late.
//
// HOW IT IS MEASURED, AND WHY THE INSTRUMENT IS THE SHELL'S COUNTER AND NOT A LOG LINE.
// `EnsureCommandBarOptionsBinding` is `private` and `IsAlreadyBound` is not reachable from a
// clause, so the binding is observed through what a single click DOES:
// `OnOptionsRequested.Broadcast()` -> `HandleCommandBarOptionsRequested` ->
// `AStratPlayerController::RequestOptionsScreen` -> `UStratShellSubsystem::ExecuteRoute(Options)`
// -> `RequestOptionsPanel`, which moves `GetOptionsPanelRequestCount()`. THAT IS A MODULE-SIDE
// COUNTER READ BEFORE AND AFTER, and every number below is a delta on it. N bindings produce N
// on that delta; one binding produces one.
//
// THE BAR IS PLANTED ON `AStratScoreboardHUD::CommandBar` DIRECTLY, which is public, rather than
// created through `CreateCommandBarWidget` from a `CommandBarWidgetClass` -- that property is
// `protected` and creating the widget would drag Slate and a viewport into a clause whose
// subject is a delegate binding. `FindScoreboardHUD` reads the controller's `MyHUD`, so the HUD
// is attached there the way `APlayerController::SpawnDefaultHUD` would have attached it.
//
// WHAT THESE CLAUSES DO **NOT** PIN.
//   - THAT A CLICK ON THE SHIPPED BUTTON BROADCASTS. That is
//     `Stratocracy.StratUI.GATE-TITLEMENU.TheCommandBarOptionsButtonBroadcastsExactlyOncePerClick`,
//     in StratUI, which is the module that can see the button. These clauses broadcast
//     `OnOptionsRequested` directly and so are indifferent to whether any button exists.
//   - THAT `WBP_CommandBar` CARRIES AN `OptionsButton`, or that `AStratScoreboardHUD` is
//     configured with a `CommandBarWidgetClass` on any shipped map. Asset facts, other
//     instruments.
//   - ANY VIEW CONTENT. The models applied are empty; `ApplyView`'s reconciliation of units,
//     the ring and the receipts is `StratMatchReconcile.cpp`'s and is untouched here.
// ---------------------------------------------------------------------------------------

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "StratMatchSubsystem.h"
#include "StratOptionsPanelDoubles.h"
#include "StratPlayerController.h"
#include "StratScoreboardHUD.h"
#include "StratShellSubsystem.h"
#include "StratViewModel.h"

namespace StratCommandBarOptionsBind
{
	/**
	 * A Game world with a live game instance, this project's player controller, a scoreboard
	 * HUD attached to it, and a plantable command bar.
	 *
	 * THE CONTROLLER MUST BE `AStratPlayerController` AND NOT A BARE ONE, because
	 * `HandleCommandBarOptionsRequested` casts and reports a Warning on the null. A bare
	 * controller would make this fixture emit a log line per click and would measure nothing.
	 *
	 * TEARDOWN ORDER IS THE PROJECT'S MEASURED ONE -- world first, world context second; see
	 * `StratBoardPicking.cpp`.
	 */
	struct FBarScope
	{
		UWorld*                         World      = nullptr;
		TStrongObjectPtr<UGameInstance> Instance;
		AStratPlayerController*         Controller = nullptr;
		AStratScoreboardHUD*            Hud        = nullptr;

		/** Empty when the fixture stood up; otherwise the first thing that went wrong. */
		FString Failure;

		FBarScope()
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

			// `InitializeStandalone` is what stands the subsystem collection up; a bare
			// `NewObject<UGameInstance>` has none. Measured in `StratShellHudCallSiteClauses.cpp`,
			// and it is what makes `UStratShellSubsystem` reachable at the end of the chain.
			Instance = TStrongObjectPtr<UGameInstance>(NewObject<UGameInstance>(GEngine));
			if (!Instance.IsValid())
			{
				Failure = TEXT("could not allocate a UGameInstance");
				return;
			}
			Instance->InitializeStandalone(TEXT("StratCommandBarOptionsBind"), nullptr);
			World->SetGameInstance(Instance.Get());

			// MEASURED, NOT DECORATION, AND THE MEASUREMENT WAS THIS FIXTURE'S OWN. Without
			// this line a controller spawned below is never added to the world's
			// `PlayerControllerList`, so `UWorld::GetFirstPlayerController` -- the first step of
			// `UStratMatchSubsystem::FindScoreboardHUD` -- returns null and the binding under
			// test never runs. The failure presented as "the bind never happened", which is
			// exactly the clause's own defect message, and only the fixture premise below told
			// the two apart.
			World->InitializeActorsForPlay(FURL());

			Controller = World->SpawnActor<AStratPlayerController>();
			if (Controller == nullptr)
			{
				Failure = TEXT("could not spawn an AStratPlayerController");
				return;
			}

			Hud = World->SpawnActor<AStratScoreboardHUD>();
			if (Hud == nullptr)
			{
				Failure = TEXT("could not spawn an AStratScoreboardHUD");
				return;
			}

			// `AHUD::PlayerOwner` is what `GetOwningPlayerController` reads, and
			// `APlayerController::MyHUD` is what `FindScoreboardHUD` reads. Set directly
			// because nothing here runs `APlayerController::SpawnDefaultHUD`.
			Hud->PlayerOwner   = Controller;
			Controller->MyHUD  = Hud;
		}

		~FBarScope()
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

		UStratMatchSubsystem* Match() const
		{
			return (World != nullptr) ? World->GetSubsystem<UStratMatchSubsystem>() : nullptr;
		}

		UStratShellSubsystem* Shell() const
		{
			return Instance.IsValid()
				? Instance->GetSubsystem<UStratShellSubsystem>() : nullptr;
		}

		FBarScope(const FBarScope&)            = delete;
		FBarScope& operator=(const FBarScope&) = delete;
	};
}

// ---------------------------------------------------------------------------------------
// (1) The bind is made at most once, however many refreshes elapse.
//
// TEN `ApplyView`s AND NOT TWO. The number is arbitrary and is deliberately larger than any
// plausible off-by-one: an unguarded `AddDynamic` gives a delta of 10 here, which names the
// defect in the failure message rather than leaving a reader to work out why 2 appeared.
//
// THE SECOND CLICK IS THE POSITIVE CONTROL. A binding that was consumed by its first delivery
// would pass "exactly one" perfectly and would leave the button dead from the player's second
// press onward -- which in a match is every press after the first.
//
// MUTANTS:
//   - the `if (!Bar->OnOptionsRequested.IsAlreadyBound(...))` guard deleted -- red, printing 10.
//   - the `EnsureCommandBarOptionsBinding()` call removed from `ApplyView` -- red on the first
//     delta, printing 0.
//   - `HandleCommandBarOptionsRequested` calling `Shell->RequestOptionsPanel()` directly instead
//     of `PC->RequestOptionsScreen` -- NOT red here, and said rather than left to be found: both
//     routes move the same counter. That choice is argued on the click cue in the handler's own
//     declaration and is not pinned by any clause in this tree.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCommandBarOptionsBindIsIdempotentTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.TheCommandBarOptionsBindSurvivesManyRefreshesWithoutAccumulating",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCommandBarOptionsBindIsIdempotentTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratCommandBarOptionsBind;

	FBarScope Scope;
	if (!Scope.Failure.IsEmpty())
	{
		AddError(Scope.Failure);
		return false;
	}

	UStratMatchSubsystem* const Match = Scope.Match();
	UStratShellSubsystem* const Shell = Scope.Shell();

	if (!TestNotNull(TEXT("FIXTURE: this Game world has a UStratMatchSubsystem"), Match)
		|| !TestNotNull(TEXT("FIXTURE: this game instance has a UStratShellSubsystem -- the "
			"object whose request counter is this clause's whole instrument"), Shell))
	{
		return false;
	}

	// ---- FIXTURE PREMISES: the subsystem can REACH the bar the way it does in a match --------
	//
	// `FindScoreboardHUD` is private and takes exactly this route -- the world's first player
	// controller, then that controller's `MyHUD`. A fixture that failed either step would make
	// every delta below zero and the clause would read "the bind never happened", which is a
	// true sentence that names the wrong culprit. This is the "measure the premise" rule
	// applied to the fixture's own plumbing.
	if (!TestTrue(TEXT("FIXTURE: this world's first player controller is the fixture's own "
			"AStratPlayerController -- the object FindScoreboardHUD starts from"),
			Scope.World->GetFirstPlayerController() == Scope.Controller)
		|| !TestTrue(TEXT("FIXTURE: that controller's HUD is the fixture's AStratScoreboardHUD "
			"-- the object FindScoreboardHUD casts to"),
			Scope.Controller->GetHUD() == Scope.Hud))
	{
		return false;
	}

	const TStrongObjectPtr<UStratCommandBarPlayDouble> Bar(
		NewObject<UStratCommandBarPlayDouble>(Scope.World));
	if (!TestTrue(TEXT("FIXTURE: a command bar double was constructed"), Bar.IsValid()))
	{
		return false;
	}

	Scope.Hud->CommandBar = Bar.Get();

	// ---- PREMISE: the bar reaches nothing YET ----------------------------------------------
	// This is what makes the delta after the refreshes a measurement of the BINDING rather than
	// of anything the fixture did on its way up.
	Bar->OnOptionsRequested.Broadcast();
	if (!TestEqual(TEXT("premise: before any ApplyView the bar's OnOptionsRequested reaches "
			"nothing, so the shell's request counter has not moved. If this is non-zero the "
			"bind is happening somewhere this clause is not looking"),
			Shell->GetOptionsPanelRequestCount(), 0))
	{
		return false;
	}

	// ---- TEN REFRESHES ---------------------------------------------------------------------
	const FStratViewModel Empty;
	for (int32 Refresh = 0; Refresh < 10; ++Refresh)
	{
		Match->ApplyView(Empty);
	}

	// ---- ONE CLICK -------------------------------------------------------------------------
	const int32 Before = Shell->GetOptionsPanelRequestCount();
	Bar->OnOptionsRequested.Broadcast();

	TestEqual(TEXT("after TEN refreshes, one click on the command bar's options control "
		"requests the options panel EXACTLY ONCE. A larger number here is the accumulating "
		"AddDynamic this binding's guard exists to prevent -- one press opening the volume "
		"screen once per elapsed frame of the match"),
		Shell->GetOptionsPanelRequestCount() - Before, 1);

	// ---- AND A SECOND CLICK STILL WORKS ----------------------------------------------------
	const int32 Between = Shell->GetOptionsPanelRequestCount();
	Bar->OnOptionsRequested.Broadcast();

	TestEqual(TEXT("control: the second click requests once too, so the count above is one "
		"live binding and not a binding consumed by its first delivery"),
		Shell->GetOptionsPanelRequestCount() - Between, 1);

	return true;
}

// ---------------------------------------------------------------------------------------
// (2) A bar that appears AFTER the first refresh is bound on the next one.
//
// THIS IS THE HALF A ONE-SHOT BIND LOSES, AND IT IS THE ORDINARY CASE RATHER THAN AN EDGE ONE.
// `UStratMatchSubsystem` exists from the moment the world does; `AStratScoreboardHUD` creates
// the command bar during its own `BeginPlay`. So on every real match the first refreshes run
// with no bar at all, and a bind placed anywhere that runs once would have already missed it.
//
// THE FIRST REFRESH WITH NO BAR IS NOT MERELY SETUP -- it is the state the reconcile has to
// survive silently. `EnsureCommandBarOptionsBinding` returns early on a null HUD and a null bar
// without logging, deliberately, *"on `PushGuidance`'s stated reasoning"*, so this clause also
// stands as evidence that the no-bar path is quiet: an implementation that warned would put one
// line per refresh into every headless clause in this module.
//
// MUTANT: move `EnsureCommandBarOptionsBinding()` from `ApplyView` into `Initialize` or
// `OnWorldBeginPlay` -- green on nothing here, red on the delta, printing 0.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCommandBarOptionsBindCatchesALateBarTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.ACommandBarCreatedAfterTheFirstRefreshIsBoundOnTheNext",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCommandBarOptionsBindCatchesALateBarTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratCommandBarOptionsBind;

	FBarScope Scope;
	if (!Scope.Failure.IsEmpty())
	{
		AddError(Scope.Failure);
		return false;
	}

	UStratMatchSubsystem* const Match = Scope.Match();
	UStratShellSubsystem* const Shell = Scope.Shell();

	if (!TestNotNull(TEXT("FIXTURE: a UStratMatchSubsystem"), Match)
		|| !TestNotNull(TEXT("FIXTURE: a UStratShellSubsystem"), Shell))
	{
		return false;
	}

	// ---- FIXTURE PREMISES: the subsystem can REACH the bar the way it does in a match --------
	//
	// `FindScoreboardHUD` is private and takes exactly this route -- the world's first player
	// controller, then that controller's `MyHUD`. A fixture that failed either step would make
	// every delta below zero and the clause would read "the bind never happened", which is a
	// true sentence that names the wrong culprit. This is the "measure the premise" rule
	// applied to the fixture's own plumbing.
	if (!TestTrue(TEXT("FIXTURE: this world's first player controller is the fixture's own "
			"AStratPlayerController -- the object FindScoreboardHUD starts from"),
			Scope.World->GetFirstPlayerController() == Scope.Controller)
		|| !TestTrue(TEXT("FIXTURE: that controller's HUD is the fixture's AStratScoreboardHUD "
			"-- the object FindScoreboardHUD casts to"),
			Scope.Controller->GetHUD() == Scope.Hud))
	{
		return false;
	}

	const FStratViewModel Empty;

	// ---- REFRESHES WITH NO BAR -------------------------------------------------------------
	if (!TestNull(TEXT("premise: the HUD has no command bar yet, which is the state of every "
			"real match until AStratScoreboardHUD's BeginPlay has created one"),
			Scope.Hud->CommandBar.Get()))
	{
		return false;
	}

	Match->ApplyView(Empty);
	Match->ApplyView(Empty);
	Match->ApplyView(Empty);

	// ---- THE BAR APPEARS -------------------------------------------------------------------
	const TStrongObjectPtr<UStratCommandBarPlayDouble> Bar(
		NewObject<UStratCommandBarPlayDouble>(Scope.World));
	if (!TestTrue(TEXT("FIXTURE: a command bar double was constructed"), Bar.IsValid()))
	{
		return false;
	}

	Scope.Hud->CommandBar = Bar.Get();

	// PREMISE: the refreshes that ran before it existed did not somehow bind it.
	Bar->OnOptionsRequested.Broadcast();
	if (!TestEqual(TEXT("premise: a bar that has never seen a refresh reaches nothing"),
			Shell->GetOptionsPanelRequestCount(), 0))
	{
		return false;
	}

	// ---- ONE MORE REFRESH ------------------------------------------------------------------
	Match->ApplyView(Empty);

	const int32 Before = Shell->GetOptionsPanelRequestCount();
	Bar->OnOptionsRequested.Broadcast();

	TestEqual(TEXT("the very next refresh after the bar appeared bound it, so one click now "
		"requests the options panel exactly once. Zero here means the bind runs once at "
		"startup and the in-match Options control is dead on every real match, because the HUD "
		"creates the bar after this subsystem exists"),
		Shell->GetOptionsPanelRequestCount() - Before, 1);

	return true;
}
