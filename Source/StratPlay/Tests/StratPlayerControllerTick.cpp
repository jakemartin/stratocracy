// The gate on the one actor flag that silently switches off every player input in the game.
// GDD §2.11 / §4.9, under T-UI-02 -- see "ON THE ACCEPTANCE ID" at the bottom of this block.
//
// WHY THIS FILE EXISTS, AND DO NOT TIDY IT INTO "a test that a bool is true".
//
// `APlayerController::TickActor` calls `TickPlayerInput`, which calls
// `PlayerInput->ProcessInputStack(...)`. That call is the ONLY place Enhanced Input walks its
// trigger state machines and fires the delegates `SetupInputComponent` bound. A controller
// with `PrimaryActorTick.bCanEverTick == false` still RECEIVES every key -- `InputKey` runs and
// the key is buffered -- and then nothing ever drains the stack, so every action sits at
// `None - 0.000s (false)` forever and not one bound delegate is called.
//
// [AMENDED 2026-08-27 -- THIS CLAUSE PROTECTS TWO MECHANISMS AND UNTIL NOW NAMED ONLY ONE. The
// assertion has not changed and was correct throughout; what was incomplete is the rationale,
// which is the half a future reader would use to decide whether the flag still matters.
//
// THE SECOND MECHANISM: THE HOVER IS NOW POLLED ON `Tick`. On 2026-08-27 the §2.11.3 / §2.11.2
// hover moved off Enhanced Input entirely -- an `Axis2D` action bound `ETriggerEvent::Triggered`
// was measured DEAD on a human playtest (the handler ran once in three and a half minutes, on a
// focus transition) and was removed, assets and all. `AStratPlayerController::Tick` now calls
// `UpdateHoverFromCursor` once a frame, and that call is the ONLY route from the cursor to the
// hovered hex. `bCanEverTick == false` therefore does not merely disable Enhanced Input any
// more; it stops the hover dead as well, and it does so with the same total silence -- the
// build green, the poll present and correct, `UpdateHoverFromCursor` never called by anything.
//
// THE TWO FAILURES ARE INDEPENDENT AND THE FLAG IS THE SINGLE POINT UNDER BOTH. Enhanced Input
// dies because `ProcessInputStack` never runs; the hover dies because `Tick`'s own body never
// runs. Neither implies the other and this clause is the only net under either.
//
// WHAT PINS THE OTHER END OF THE HOVER CHAIN. This clause pins that the engine WILL call
// `Tick`; `Stratocracy.StratPlay.T-UI-01.TickDrivesTheHoverPath` pins that `Tick` moves the
// hover when it is called. Neither alone is the route, and neither re-asserts the other's
// subject -- that clause calls the override directly and reads no tick flag at all.]
//
// So `false` here disables ALL player input -- and, since 2026-08-27, the hover with it -- while
// leaving:
//   - the build green,
//   - every `BindAction` call present, run, and warning-free,
//   - the mapping context added and the subsystem valid,
//   - the console still working (it does not go through `ProcessInputStack`),
//   - and EVERY OTHER TEST IN THIS SUITE passing, because `FStratSelectionMachine` is a
//     plain non-`UObject` struct deliberately tested with no actor, no world and no PIE.
//     Nothing else in the suite observes an actor tick flag at all -- including
//     `T-UI-01.TickDrivesTheHoverPath`, which calls the override directly and so is green
//     with the flag off. That is deliberate on both sides: this clause is the only observer.
//     [AMENDED 2026-08-27: this line read "all 66 other tests in this suite passing". A count
//     of a growing set, written inside the thing it counts, and stale for many phases. The
//     figure was never load-bearing -- the point is "every one of them", not a number -- and
//     `global.md` is the only file in this project that may state a live suite count anyway.]
//
// That combination is what made the phase 6 investigation long: every diagnostic a reader
// would reach for reports "input is fine". This clause is the cheap net under a regression
// that has already happened once, standing from phase 4 to phase 6 under the reasoning "this
// controller polls nothing" -- true, and not the reason the tick is needed.
// `StratPlayerController.cpp`'s constructor carries the same measurement in full.
//
// WHY THE C++ CLASS DEFAULT OBJECT AND NOT A SPAWNED ACTOR OR `BP_StratPlayerController`.
// This is the load-bearing part of the clause and it is deliberate, not a convenience.
//   - Not a spawned instance: a CDO read needs no world, no PIE and no `SpawnActor`, so this
//     runs headless in microseconds on every build. An instance would add a world fixture and
//     would still be reading the same value off the same class defaults.
//   - Not the Blueprint: a Blueprint subclass may legitimately override "Start with Tick
//     Enabled" in its class defaults. A test that read `BP_StratPlayerController` would be
//     pinning an ASSET-STEWARD property under a GAMEPLAY-ENGINEER acceptance ID, and would
//     fail for an asset edit that this clause has no opinion about. The subject here is the
//     C++ constructor in `Source/StratPlay/StratPlayerController.cpp` and nothing else.
// The class identity is therefore ASSERTED, not assumed: the CDO's class must be exactly
// `AStratPlayerController::StaticClass()` and that class must be native. If a later refactor
// ever routed this read through a Blueprint-generated class, the clause says so instead of
// quietly changing what it pins.
//
// WHERE THE EXPECTATION COMES FROM. `true` is a literal here, and this is the one shape in
// which a literal is correct: there is no module-side value that can produce it. The
// requirement is imposed by the engine's input pipeline -- `TickActor` -> `TickPlayerInput`
// -> `ProcessInputStack` -- not by any Stratocracy rules value, and no `FStratBridge`,
// `FStratViewModel` or `strat::` API knows anything about actor ticking. The base class's own
// CDO (`APlayerController` enables ticking in its constructor) is deliberately NOT used as the
// expectation: parity against it would pass if an engine upgrade ever flipped the base default,
// which is precisely the case in which player input breaks. The property is absolute.
//
// ON THE ACCEPTANCE ID. `strat-gameplay-engineer` proposed `T-PLAY-01.PlayerControllerTicks`
// in one report and `T-UI-02.ControllerTicks` in another, and stated that neither was written
// down. `T-UI-02` is used here, on two grounds, and `strat-data-steward` should confirm or
// reassign it:
//   1. `T-PLAY-01` appears ZERO times in the GDD (measured: `grep -c "T-PLAY"` -> 0). Every
//      acceptance ID already used in `Source/StratPlay/Tests/` -- T-UI-01, T-UI-02, T-UI-03,
//      T-INT-05, T-SAVE-05, T-TURN-01 -- is a real GDD ID. No test in this tree mints one, and
//      minting IDs is not the test author's lane.
//   2. `T-UI-02` is "the reachable-hex highlight displays exactly the T-MOVE-01 set" (GDD
//      §4.11), and StratPlay's existing T-UI-02 clauses are the click-to-hex-to-reach chain:
//      `BoardHexRoundTrip`, `ReachOverlayIsNotComputedHere`,
//      `SelectionMachineUsesTheQueryNotDistance`. The controller's tick is the FIRST link in
//      that same chain -- without it the click that produces the highlight never arrives.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "GameFramework/PlayerController.h"
#include "UObject/Class.h"
#include "UObject/ObjectMacros.h"

#include "StratPlayerController.h"

// ---------------------------------------------------------------------------
// The controller ticks, so Enhanced Input dispatches -- T-UI-02's first link.
//
// No world, no PIE, no spawned actor: the subject is the class default object of the C++
// class, which is the object the constructor in `StratPlayerController.cpp` wrote.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratPlayerControllerTicksSoInputDispatchesTest,
	"Stratocracy.StratPlay.T-UI-02.ControllerTicksSoInputDispatches",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratPlayerControllerTicksSoInputDispatchesTest::RunTest(const FString& /*Parameters*/)
{
	UClass* const ControllerClass = AStratPlayerController::StaticClass();
	if (!TestNotNull(TEXT("AStratPlayerController::StaticClass() resolves"), ControllerClass))
	{
		return false;
	}

	// ---- The subject is the NATIVE class, not a Blueprint-generated one -----------------
	//
	// Asserted rather than assumed. A Blueprint subclass may override "Start with Tick
	// Enabled" in its class defaults; this clause pins the C++ constructor and must never
	// silently start reading an asset's value instead. See the header block.
	if (!TestTrue(
			TEXT("the class under test is native C++, not a Blueprint-generated class -- this ")
			TEXT("clause pins StratPlayerController.cpp's constructor, never BP_StratPlayerController"),
			ControllerClass->HasAnyClassFlags(CLASS_Native)))
	{
		return false;
	}

	AStratPlayerController* const Cdo = ControllerClass->GetDefaultObject<AStratPlayerController>();
	if (!TestNotNull(TEXT("the C++ class default object exists"), Cdo))
	{
		return false;
	}

	if (!TestTrue(
			FString::Printf(
				TEXT("the CDO's class is exactly AStratPlayerController and not a subclass ")
				TEXT("(read: '%s')"),
				*Cdo->GetClass()->GetName()),
			Cdo->GetClass() == ControllerClass))
	{
		return false;
	}

	// ---- The clause ---------------------------------------------------------------------
	//
	// `false` here means APlayerController::TickActor never runs, so TickPlayerInput never
	// runs, so UPlayerInput::ProcessInputStack never runs -- and ProcessInputStack is the
	// only evaluator of Enhanced Input triggers. Every key would still be received and
	// buffered, every binding would still be present and correct, the build would still be
	// green, and not one bound delegate would ever fire. Measured in phase 6.
	//
	// AND, SINCE 2026-08-27, `false` ALSO STOPS THE HOVER: `AStratPlayerController::Tick`'s
	// own body never runs, so `UpdateHoverFromCursor` is never called, and the cursor-to-hex
	// route has no other caller. See the header block.
	TestTrue(
		TEXT("AStratPlayerController's C++ CDO has PrimaryActorTick.bCanEverTick == true. ")
		TEXT("If this failed, TWO mechanisms are dead at once. (1) PLAYER INPUT IS ENTIRELY ")
		TEXT("DEAD: APlayerController::TickActor -> TickPlayerInput -> ")
		TEXT("UPlayerInput::ProcessInputStack is the one place Enhanced Input evaluates ")
		TEXT("triggers and fires bound delegates; with the tick off, every action sits at ")
		TEXT("'None - 0.000s (false)' forever while the build stays green and every binding ")
		TEXT("stays configured. (2) THE HOVER IS DEAD: since 2026-08-27 the hover is POLLED, ")
		TEXT("and AStratPlayerController::Tick's call to UpdateHoverFromCursor is the only ")
		TEXT("route from the cursor to the hovered hex -- with the tick off, Tick's body never ")
		TEXT("runs and no hex is ever hovered. Restore the assignment in ")
		TEXT("Source/StratPlay/StratPlayerController.cpp's constructor -- do not delete it, ")
		TEXT("and do not 'simplify' this test away."),
		Cdo->PrimaryActorTick.bCanEverTick);

	return true;
}
