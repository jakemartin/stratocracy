// The gate on the one persistent flag that switches off every click in the game while leaving
// the hover working. GDD Sec 2.11 / Sec 4.9, under T-UI-02 -- the same acceptance ID as
// `StratPlayerControllerTick.cpp`, and for the same reason it gives: T-UI-02 is the
// click-to-hex-to-reach chain, and this is the link before its first one. The controller's
// tick is what dispatches a click that arrived; this is what lets one arrive at all.
//
// WHY THIS FILE EXISTS. On 2026-08-31 a player could hover the board and could not select
// anything on it. Nothing in the selection path was wrong:
//
//   - `AStratShellHUD::ApplyMenuInputMode` sets `FInputModeUIOnly` for the title menu.
//   - `FInputModeUIOnly::ApplyInputMode` calls `GameViewportClient.SetIgnoreInput(true)`.
//   - The `UGameViewportClient` is owned by the game instance and SURVIVES `OpenLevel`. In the
//     engine source `bIgnoreInput` is written in exactly four places -- the two viewport-client
//     constructors, which default it false, and the three `FInputMode*::ApplyInputMode`
//     overrides. Level travel is not one of them.
//
// So the title screen's input mode followed the player into the match and every key and button
// was dropped at the viewport, upstream of `UPlayerInput` and therefore upstream of everything
// `StratPlayerControllerTick.cpp` protects. The hover survived it because the hover is not an
// input event: `Tick` polls `UpdateHoverFromCursor`, which reads the cursor off the viewport
// directly. `Saved/Logs/Stratocracy.log` for that session records the shape -- PIE on
// `Lvl_Title`, travel to `Lvl_FerrumCrossing`, a seeded 99-hex match, and ZERO `STRAT-CMD`
// lines, with no `is unset` warning and no missing-context warning to blame an asset for it.
//
// THE THREE CLAUSES BELOW ARE INDEPENDENT AND NONE OF THEM RE-ASSERTS ANOTHER'S SUBJECT:
//
//   1. `TheInputClaimClearsAViewportsIgnoreInput` pins the repair itself -- the field the
//      defect is about, cleared unconditionally.
//   2. `TheInputClaimRestoresTheProjectsOwnMouseModes` pins that the restore reads
//      `UInputSettings` rather than imposing an input mode's opinion, and that it mirrors
//      `UGameViewportClient::Init`'s headless rule instead of inventing one.
//   3. `BeginPlayMakesTheInputClaim` pins the ROUTE. This project has a recorded incident of
//      the mirror-image defect -- a correct mechanism with no caller, which reads as built --
//      and a static repair function nobody calls would be exactly that.
//
// WHY (3) NEEDS `EStratInputClaim` AT ALL, since a reader will ask. A headless automation world
// has no `UGameViewportClient`, so the claim can only take its no-viewport arm here and leaves
// no mark on any object a clause could otherwise reach. `NotAttempted` and `NoViewport` are
// therefore different values on purpose: deleting the `ClaimGameInput()` line out of
// `BeginPlay` leaves the first, and the clause reads the difference. Nothing on screen reads
// that member and it is not reflected, so it is not the presentation bit T-INT-05 forbids.
//
// WHERE THE EXPECTATIONS COME FROM. Clause (1)'s `false` is a literal and is the shape in which
// a literal is correct: no Stratocracy value produces it, the requirement is imposed by the
// engine's input pipeline. Clause (2) takes BOTH of its expectations from
// `GetDefault<UInputSettings>()` -- the same object `UGameViewportClient::Init` reads -- rather
// than retyping `Config/DefaultInput.ini`, so a project that changes its capture mode moves
// this clause with it and does not redden it.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Misc/App.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/Package.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "GameFramework/InputSettings.h"

#include "StratPlayerController.h"

namespace StratInputClaim
{
	/**
	 * A bare viewport client to plant a state on.
	 *
	 * NOT `Init`ialised, DELIBERATELY. `UGameViewportClient::Init` is the thing the restore
	 * mirrors, so standing one up here would hand the clause its expectation from the very
	 * code path it exists to check. Each clause plants the state it cares about explicitly.
	 */
	UGameViewportClient* NewBareViewportClient()
	{
		return NewObject<UGameViewportClient>(
			GEngine != nullptr ? static_cast<UObject*>(GEngine)
				               : static_cast<UObject*>(GetTransientPackage()),
			UGameViewportClient::StaticClass());
	}

	/**
	 * A world with a standalone game instance, so an `AStratPlayerController` can be spawned
	 * and dispatched.
	 *
	 * THE TEARDOWN ORDER IS THE PROJECT'S MEASURED ONE: world first, world context second.
	 * `StratBoardPicking.cpp` records why -- tearing the context down first leaves
	 * `UWorld::DestroyActor` warning "World has no context!" once per actor, and a gate that
	 * leaves warnings in a green run trains the next reader to skim warnings.
	 */
	struct FControllerScope
	{
		UWorld*                  World      = nullptr;
		UGameInstance*           Instance   = nullptr;
		AStratPlayerController*  Controller = nullptr;

		/** Empty when the fixture stood up; otherwise the first thing that went wrong. */
		FString Failure;

		FControllerScope()
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

			// `InitializeStandalone` is what stands the subsystem collection up --
			// `StratShellHudCallSiteClauses.cpp` records the measurement that a bare
			// `NewObject<UGameInstance>` has none. The controller's `BeginPlay` looks for
			// `UStratMatchSubsystem` on the WORLD rather than the instance, and finding
			// none is an arm it handles at Verbose; the instance is here so the world is
			// the shape a spawned controller expects rather than for the subsystem.
			Instance = NewObject<UGameInstance>(GEngine);
			if (Instance == nullptr)
			{
				Failure = TEXT("could not allocate a UGameInstance");
				return;
			}
			Instance->InitializeStandalone(TEXT("StratInputClaim"), nullptr);
			World->SetGameInstance(Instance);

			Controller = World->SpawnActor<AStratPlayerController>();
			if (Controller == nullptr)
			{
				Failure = TEXT("could not spawn an AStratPlayerController");
				return;
			}
		}

		~FControllerScope()
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

		FControllerScope(const FControllerScope&) = delete;
		FControllerScope& operator=(const FControllerScope&) = delete;
	};
}

// ---------------------------------------------------------------------------
// (1) The repair itself. The field the defect is about, cleared unconditionally.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratInputClaimClearsIgnoreInputTest,
	"Stratocracy.StratPlay.T-UI-02.TheInputClaimClearsAViewportsIgnoreInput",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratInputClaimClearsIgnoreInputTest::RunTest(const FString& /*Parameters*/)
{
	UGameViewportClient* const ViewportClient = StratInputClaim::NewBareViewportClient();
	if (!TestNotNull(TEXT("premise: a UGameViewportClient could be allocated"), ViewportClient))
	{
		return false;
	}

	// THE PLANT IS THE DEFECT, REPRODUCED AT ITS ONLY MECHANISM. `FInputModeUIOnly` reaches
	// the viewport client through exactly this setter; planting the flag rather than applying
	// the input mode keeps the clause off Slate, which a headless run has no window for.
	ViewportClient->SetIgnoreInput(true);
	if (!TestTrue(TEXT("premise: the planted state is the shipped defect -- input ignored"),
			ViewportClient->IgnoreInput()))
	{
		return false;
	}

	AStratPlayerController::RestoreProjectInputState(*ViewportClient);

	TestFalse(
		TEXT("the input claim clears bIgnoreInput, so a click reaches UPlayerInput at all. ")
		TEXT("If this is red, a title-screen UI-only input mode survives the travel into the ")
		TEXT("match and the player can hover the board but not select anything on it."),
		ViewportClient->IgnoreInput());

	return true;
}

// ---------------------------------------------------------------------------
// (2) It restores the PROJECT'S modes, and it mirrors `Init`'s headless rule.
//
// TWO ARMS AND BOTH ASSERT. Which one runs is decided by `FApp::CanEverRender()`, which is the
// same test `UGameViewportClient::Init` uses; the clause does not skip on either side, so it
// cannot go quietly inert on a runner configured differently from this box.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratInputClaimRestoresProjectMouseModesTest,
	"Stratocracy.StratPlay.T-UI-02.TheInputClaimRestoresTheProjectsOwnMouseModes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratInputClaimRestoresProjectMouseModesTest::RunTest(const FString& /*Parameters*/)
{
	const UInputSettings* const Settings = GetDefault<UInputSettings>();
	if (!TestNotNull(TEXT("premise: the project's UInputSettings are readable"), Settings))
	{
		return false;
	}

	UGameViewportClient* const ViewportClient = StratInputClaim::NewBareViewportClient();
	if (!TestNotNull(TEXT("premise: a UGameViewportClient could be allocated"), ViewportClient))
	{
		return false;
	}

	// THE PLANT IS DERIVED SO IT CANNOT ACCIDENTALLY BE THE ANSWER. `NoCapture` is what
	// `FInputModeUIOnly` leaves behind and is the state this repair exists to undo -- but if
	// the project ever adopts `NoCapture` as its own default, planting it would make the
	// restore arm untestable by coincidence. So the plant moves to `CapturePermanently` in
	// that case, and either way the planted value differs from the expected one.
	const EMouseCaptureMode PlantedCapture =
		Settings->DefaultViewportMouseCaptureMode == EMouseCaptureMode::NoCapture
			? EMouseCaptureMode::CapturePermanently
			: EMouseCaptureMode::NoCapture;
	const EMouseLockMode PlantedLock =
		Settings->DefaultViewportMouseLockMode == EMouseLockMode::DoNotLock
			? EMouseLockMode::LockAlways
			: EMouseLockMode::DoNotLock;

	ViewportClient->SetMouseCaptureMode(PlantedCapture);
	ViewportClient->SetMouseLockMode(PlantedLock);

	// PLANTED HERE TOO, AND IT WAS NOT AT FIRST -- WHICH THE MUTANT CAUGHT. The trailing
	// TestFalse below read a bare viewport client whose `bIgnoreInput` was already false
	// from its own constructor, so it passed no matter what the restore did: the
	// SetIgnoreInput mutant reddened clause (1) and left this clause green. An assertion
	// that cannot fail is worse than an absent one, because it reads as cover.
	ViewportClient->SetIgnoreInput(true);

	AStratPlayerController::RestoreProjectInputState(*ViewportClient);

	if (FApp::CanEverRender())
	{
		TestEqual(
			TEXT("the restored capture mode is the project's own DefaultViewportMouseCaptureMode, ")
			TEXT("read from UInputSettings and not imposed by an FInputMode*"),
			static_cast<int32>(ViewportClient->GetMouseCaptureMode()),
			static_cast<int32>(Settings->DefaultViewportMouseCaptureMode));

		TestEqual(
			TEXT("the restored lock mode is the project's own DefaultViewportMouseLockMode"),
			static_cast<int32>(ViewportClient->GetMouseLockMode()),
			static_cast<int32>(Settings->DefaultViewportMouseLockMode));
	}
	else
	{
		// `UGameViewportClient::Init` forces NoCapture / DoNotLock when the process cannot
		// render, so a restore that applied the settings here would put a headless run into a
		// state the engine itself never produces. Asserting the plant is UNCHANGED is what
		// makes that guard falsifiable: remove it and the settings values land instead, and
		// the plant above is chosen so those two can never coincide.
		TestEqual(
			TEXT("a run that cannot render has its capture mode left exactly as it was, ")
			TEXT("mirroring UGameViewportClient::Init rather than overriding it"),
			static_cast<int32>(ViewportClient->GetMouseCaptureMode()),
			static_cast<int32>(PlantedCapture));

		TestEqual(
			TEXT("a run that cannot render has its lock mode left exactly as it was"),
			static_cast<int32>(ViewportClient->GetMouseLockMode()),
			static_cast<int32>(PlantedLock));
	}

	// THE UNCONDITIONAL HALF, ASSERTED ON BOTH ARMS. Clause (1) is the primary subject of
	// this line; it is repeated here, over its own plant, so a reader of a red arm above can
	// tell a broken headless guard from a broken repair.
	TestFalse(TEXT("bIgnoreInput is cleared on either arm"), ViewportClient->IgnoreInput());

	return true;
}

// ---------------------------------------------------------------------------
// (3) The route. `BeginPlay` makes the claim, and a deleted call site says so.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBeginPlayMakesTheInputClaimTest,
	"Stratocracy.StratPlay.T-UI-02.BeginPlayMakesTheInputClaim",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBeginPlayMakesTheInputClaimTest::RunTest(const FString& /*Parameters*/)
{
	// A NATIVE CONTROLLER HAS NO INPUT ASSETS, so `BeginPlay` says so once, by name. Declared
	// expected rather than suppressed: the warning is the documented arm for a controller
	// whose Blueprint defaults are unset, and this fixture has no Blueprint at all.
	AddExpectedMessagePlain(TEXT("has no SelectionMappingContext"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	StratInputClaim::FControllerScope Scope;
	if (!TestTrue(FString::Printf(TEXT("premise: the fixture stood up (%s)"), *Scope.Failure),
			Scope.Failure.IsEmpty()))
	{
		return false;
	}

	if (!TestEqual(
			TEXT("premise: a spawned controller has made no claim before BeginPlay runs"),
			static_cast<int32>(Scope.Controller->GetLastInputClaim()),
			static_cast<int32>(EStratInputClaim::NotAttempted)))
	{
		return false;
	}

	Scope.Controller->DispatchBeginPlay();

	// `NoViewport` AND NOT `Claimed`, AND THE DIFFERENCE IS THE FIXTURE RATHER THAN THE CODE.
	// A headless automation world has no `UGameViewportClient`, so the claim reaches its
	// no-viewport arm and stops there. What this clause pins is that it RAN: `NotAttempted`
	// is the value a BeginPlay that no longer calls `ClaimGameInput` would leave behind.
	TestEqual(
		TEXT("BeginPlay called the input claim -- it reports NoViewport in a headless world, ")
		TEXT("not NotAttempted. If this is red, the claim's call site is gone and the repair ")
		TEXT("is a function nobody runs, which reads as built from every other instrument."),
		static_cast<int32>(Scope.Controller->GetLastInputClaim()),
		static_cast<int32>(EStratInputClaim::NoViewport));

	return true;
}
