// GDD §2.11.1 / §4.9 -- the player controller. The input path, and the one object that
// wires the selection machine to a bridge, a board and a screen.
//
// WHAT GAP THIS CLOSES. `AStratGameMode.h` recorded the absence in advance: "there is no
// Strat player controller yet. Phase 4 builds it, with Enhanced Input and the selection
// state machine, and sets it here in the same pass." Until this class existed, the match
// subsystem could seed a board and reconcile actors onto it and no human could touch it --
// `UStratMatchSubsystem.h` states in its own "NOT IN THIS ROUND" list that it deliberately
// does not wrap the five `Submit*` methods, "because phase 4 owns the PlayerController, the
// selection machine and the `STRAT-CMD accepted` log line, and a wrapper landed early is a
// wrapper written without its caller." This is the caller.
//
// IT DECIDES NOTHING, AND THE MEASURE OF THAT IS HOW LITTLE IS IN THE .cpp. It turns an
// Enhanced Input trigger into an `EStratSelectionEvent`, turns a cursor position into a
// hex by asking the board, hands both to `FStratSelectionMachine`, and applies whatever
// comes back. There is no legality check here, no reachability, no damage number and no hex
// arithmetic -- not one coordinate is added to another anywhere in this class.
//
// THE SELECTION MACHINE IS A PLAIN MEMBER, BY VALUE, AND IT IS NOT A `UPROPERTY`. It cannot
// be one: `FStratSelectionMachine` is not a reflected type, on purpose, for the two reasons
// its own header gives -- it must be drivable in a test with no world, and a reflected copy
// of `bDone` on an actor is precisely what `T-INT-05.NoActorHoldsPresentationBits` forbids.
// So this controller HOLDS the machine and reads nothing out of it into a field of its own.
// The presentation bits reach the screen only through `DecorateViewModel`, on the model, on
// the way to `ApplyView`.
//
// PICKING GOES THROUGH THE INSTANCE INDEX AND NEVER THROUGH AN INVERSE. `AStratBoardActor`
// deliberately has no `HexAtWorldLocation`, and phase 3 recorded why: "a rounding inverse of
// `WorldLocationOfHex` would disagree with the forward map at tile edges, and disagreeing
// about which hex the cursor is on is how a player attacks the wrong unit." This class
// therefore traces under the cursor and hands the hit's COMPONENT and ITEM INDEX to
// `AStratBoardActor::HexAtInstance`, which is a lookup into the instance list the board
// itself built. A trace that hits something that is not a board tile is "the cursor is not
// on the board", which is an ordinary answer.
//
// THE MAPPING CONTEXT AND THE FOUR ACTIONS ARE NULL IN C++ AND SET ON A BLUEPRINT, AND THE
// NULL CASE IS SUPPORTED RATHER THAN TOLERATED. They are `EditDefaultsOnly` `TObjectPtr`
// properties -- never a `/Game/` literal, which this project forbids in gameplay C++
// outright. With all five null this controller adds no context, binds no action, logs one
// line saying so, and the match is still seeded, drawn and reconciled. A controller that
// asserted on a null context would have made asset authoring a prerequisite for this class's
// build being green.
//
// THE ASSETS NOW EXIST -- an earlier spelling of this paragraph read "ARE NULL UNTIL PHASE
// 5", which describes a world that ended when the hot-seat milestone's phase 5 authored
// `IMC_Selection` and the four `IA_*` actions and set them on the `BP_` subclass a GameMode
// Blueprint points at. What is still true, and is the load-bearing half, is that the C++
// DEFAULTS are null and every use site below stays null-safe: this class must remain
// constructible and drivable with no asset in existence, because that is what lets it be
// built and reasoned about without the content pass.
//
// WHY THE VIEW MODEL IS REBUILT ON EVERY EVENT rather than read from
// `UStratMatchSubsystem::GetViewModel()`. That accessor is documented as "A RECORD, NOT A
// SOURCE ... nothing in this class consults it", and driving the selection machine from it
// would make what a click means depend on when someone last called `ApplyView`. Building
// fresh costs one snapshot projection per click and makes the machine a function of the
// rules state alone.
//
// IT NAMES NO `strat::` TYPE. `FStratBridge` is forward declared; `StratBridge.h` is
// included from the .cpp only, because this header declares a `UCLASS` and UHT must never
// see the vendored headers. Every bridge call the .cpp makes is one of the engine-typed
// façade methods added for exactly this caller.
//
// NOT IN THIS ROUND, with reasons:
// - CAMERA INPUT. `AStratCameraPawn` is the pawn and its own movement is its business; a
//   controller that bound camera axes would put the camera's input two objects away from
//   the camera. Phase 3 shipped the pawn without Enhanced Input and nothing in phase 4's
//   exit criterion moves it.
// - THE HOT-SEAT HAND-OVER SCREEN. `UStratMatchSubsystem::SetViewingSide` exists and is
//   deliberately NOT bound to a key here. §2.11's hand-over is a screen with a confirmation
//   on it -- who is looking matters -- and a key that silently flipped the viewing side
//   would let either player see the other's board at any time. The affordance is UI work
//   that no phase of this milestone owns.
// - CAPTURE and BUILD. See `FStratSelectionMachine`'s own deferral list; neither is on the
//   phase list and neither has an answered affordance question.
// - SAVE / LOAD, AND THIS BULLET IS RETRACTED IN PLACE. It used to read:
//   RETRACTED> "`SerializeRecordedSave` still has no engine-side caller and the save-slot
//   RETRACTED>  UI is out of the milestone."
//   Wave B1 gave it one, and the caller is NOT this class:
//   `UStratMatchSubsystem::SaveMatchToSlot` / `LoadMatchFromSlot` own the slot, because a
//   PlayerController is per-player and client-local while there is exactly one match to
//   save. What is still not here is a KEY BINDING for either, and that is deliberate --
//   binding save to a key without a confirmation surface is how a player overwrites a slot
//   by leaning on the keyboard. The condition that discharges it: a save-slot UI to bind to.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "StratGuidedOpening.h"
#include "StratSelectionMachine.h"

#include "StratPlayerController.generated.h"

// Forward declarations only, per the header block.
class FStratBridge;

class AStratBoardActor;
class UInputAction;
class UInputMappingContext;
class UStratMatchSubsystem;

/**
 * Turns clicks and keys into §4.9 commands, through `FStratSelectionMachine` and
 * `FStratBridge` and through nothing else.
 *
 * A map gets one by pointing a GameMode Blueprint at a subclass of this class. Phase 5
 * authors that Blueprint and the five input assets it carries; until it does, this class
 * runs inert and says so once.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Strat Player Controller"))
class STRATPLAY_API AStratPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AStratPlayerController();

	/**
	 * Rebuilds the model, decorates it with the presentation bits, reconciles, and repaints
	 * the two overlays. THE ONE REFRESH PATH IN THIS CLASS.
	 *
	 * PUBLIC because the hand-over and the debug console both want it, and because a single
	 * named refresh is what stops a future caller from writing a second, subtly different
	 * sequence. The sequence is build -> `DecorateViewModel` -> `ApplyView` ->
	 * `BuildOverlays` -> `ShowReach` / `ShowTargets`, and every step of it is somebody
	 * else's method.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Input")
	bool RefreshFromMachine(FString& OutFailureReason);

	/**
	 * Feeds one event to the selection machine, submits whatever it asks for, and refreshes.
	 *
	 * PUBLIC AND NOT PROTECTED, so that a hand-over screen, a debug command or a gate can
	 * drive the same path a key does. The Enhanced Input handlers below are four one-line
	 * calls to this.
	 *
	 * THE ORDER IS THE CONTRACT: decide, submit, and only then tell the machine the command
	 * applied. `FStratSelectionMachine::NotifyCommandApplied` is documented "call it only on
	 * acceptance", and calling it before the bridge answered would advance the machine past
	 * a command the rules module refused.
	 *
	 * NOT A `UFUNCTION`, and the reason is worth recording because the fix looks trivial:
	 * `EStratSelectionEvent` is a plain `enum class` and UHT will not accept one, so a
	 * Blueprint-callable version would have to take a `uint8` and cast. That is an untyped
	 * entry point into a state machine, and the only thing it buys is a node nothing in this
	 * milestone needs. `RefreshFromMachine` above IS reflected, because it takes no enum.
	 */
	bool HandleSelectionEvent(EStratSelectionEvent Event, FIntPoint Hex, FString& OutFailureReason);

	/**
	 * Which hex the cursor is over, if any.
	 *
	 * AN INSTANCE-INDEX LOOKUP, NEVER AN INVERSE OF `WorldLocationOfHex`. See the header
	 * block, and `AStratBoardActor`'s, on why no inverse exists to call.
	 *
	 * FALSE IS AN ORDINARY ANSWER -- the cursor is off the board, over a unit's own mesh, or
	 * over nothing. It is not a fault and is not logged.
	 *
	 * NON-CONST DESPITE BEING A QUERY, because `APlayerController::GetHitResultUnderCursor`
	 * is non-const. Written honestly rather than with a `const_cast` behind a const
	 * signature: the cast would be a claim about the engine's method that this file is in no
	 * position to make.
	 */
	bool HexUnderCursor(FIntPoint& OutHex);

	/**
	 * The machine, so a hand-over or a gate can read what is selected.
	 *
	 * NON-CONST BECAUSE THE GUIDANCE LAYER WRITES THE LOCKS, and that clause used to read:
	 * RETRACTED> "because §2.11.6's guidance layer will need `SetLockedThisTurn` when it
	 * RETRACTED>  exists."
	 * It exists: `FStratGuidedOpening::Observe` takes this reference and calls
	 * `SetLockedThisTurn` on it once per refresh. Retracted in place rather than deleted,
	 * because the forward-looking version is the one a reader will remember.
	 */
	FStratSelectionMachine& GetSelectionMachine() { return SelectionMachine; }
	const FStratSelectionMachine& GetSelectionMachine() const { return SelectionMachine; }

	// ---- §2.11.6's guided opening -----------------------------------------
	// THIS CLASS DRIVES IT AND DOES NOT IMPLEMENT IT, exactly as it drives the selection
	// machine. `FStratGuidedOpening` owns every beat decision; what lives here is the three
	// input gates, because §2.11.6-B calls beat 1a's End Turn constraint "the only
	// guided-opening constraint that gates a player INPUT rather than a selection, adopted
	// under Q27 (§4.7), ruled" — and input is this class's subject.
	//
	// THE GATES REFUSE BY NOT ASKING, AND NEVER BY ASKING FOR A REFUSAL. No gate calls
	// `FStratBridge`; a gated event returns before `FStratSelectionMachine::HandleEvent`
	// (so no machine state moves) or before `StratSubmitSelectionCommand` (so no command is
	// submitted). Nothing in this path can make the rules module refuse a command it would
	// otherwise accept, which is the same footing §2.11.1's machine-narrower-than-the-rule
	// note puts the SELECTED → attack restriction on.

	/** The layer, so a clause can drive it and a hand-over can read it. */
	FStratGuidedOpening& GetGuidedOpening() { return GuidedOpening; }
	const FStratGuidedOpening& GetGuidedOpening() const { return GuidedOpening; }

	/**
	 * §2.11.6's `Skip guidance` control, for the button that will eventually call it.
	 *
	 * A `UFUNCTION` WHERE `GetGuidedOpening` IS NOT, because this is the one member of the
	 * surface a widget needs and `FStratGuidedOpening` is not a reflected type. Exposing the
	 * object to Blueprint would mean making it one; exposing the single verb costs nothing.
	 *
	 * IT REFRESHES. §2.11.6 requires the ring, the marker and the strip to clear "in the
	 * same frame as the strip", and the model is only rewritten by a refresh — so the skip
	 * that did not refresh would be a skip the player sees next click.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Guidance")
	void SkipGuidance();

	/** Whether the strip is on screen. False after a skip, a suppression, or the window closing. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Guidance")
	bool IsGuidanceActive() const;

protected:
	// ---- Enhanced Input assets. Null as C++ DEFAULTS; set on the `BP_` subclass. ------
	// EVERY ONE IS AN `EditDefaultsOnly` `TObjectPtr` AND NOT A PATH. The project forbids a
	// `/Game/` literal in gameplay C++, with two standing exceptions (automation fixtures
	// and the import commandlet) that this file is neither of. A null asset is handled
	// everywhere below: no context is added, no action is bound, and the controller logs
	// once rather than per frame.

	/**
	 * The §2.11.1 mapping context, added on `BeginPlay` at `MappingPriority`.
	 *
	 * NULL IS SUPPORTED. `BeginPlay` skips the add and says so at Warning -- once, because a
	 * missing input asset is a configuration gap in whatever Blueprint the map's GameMode
	 * names, and not a fault that repeats. (The shipping `BP_` subclass does set it; this
	 * arm is what keeps a map that forgot to from being a crash instead of a warning.)
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Input")
	TObjectPtr<UInputMappingContext> SelectionMappingContext;

	/**
	 * Priority for the context above.
	 *
	 * A PROPERTY RATHER THAN A LITERAL because this game will eventually stack a hand-over
	 * screen's context over this one, and the ordering between them is a design question
	 * that must not be settled by a magic number in a C++ file.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Input")
	int32 MappingPriority = 0;

	/** Primary click on a hex: select, move or attack. `EStratSelectionEvent::HexPrimary`. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Input")
	TObjectPtr<UInputAction> SelectAction;

	/** Secondary click or Escape: drop the selection. `EStratSelectionEvent::Cancel`. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Input")
	TObjectPtr<UInputAction> CancelAction;

	/**
	 * §2.11.1's wait. `EStratSelectionEvent::Wait`.
	 *
	 * IT SUBMITS NO COMMAND, and that is not an omission. `strat::SaveCommandKind` has no
	 * `Wait` -- phase 1 recorded the correction -- so a wait is the selection machine
	 * marking a unit DONE and nothing reaching the rules module at all.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Input")
	TObjectPtr<UInputAction> WaitAction;

	/** End the active side's turn. `EStratSelectionEvent::EndTurn`, and the one event that
	 *  is valid with nothing selected. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Input")
	TObjectPtr<UInputAction> EndTurnAction;

	// ---- Lifetime ---------------------------------------------------------

	/** Adds the mapping context, shows the cursor, and paints the first frame's overlays. */
	virtual void BeginPlay() override;

	/**
	 * Arms §2.11.6's guided opening once, the first time a seeded match is reachable.
	 *
	 * ONCE, AND LAZILY. `BeginPlay` ordering between a PlayerController and a GameMode's
	 * `StartMatch` is not something this file may assume — so rather than asserting an
	 * order, this is called from the refresh path and returns immediately once armed. The
	 * cost is one bool test per refresh; the alternative is a guided opening that silently
	 * does not run because two `BeginPlay`s landed the other way round on someone's machine.
	 *
	 * THE SUPPRESSION BOOL COMES FROM THE SAVE SLOT, through
	 * `UStratMatchSubsystem::HasCompletedAMatchOnSave`. §2.11.6: "Any completed match on the
	 * save skips all guidance automatically." Read here rather than inside
	 * `FStratGuidedOpening`, which owns no disk and must stay drivable with no slot present.
	 */
	void TryArmGuidedOpening();

	/**
	 * Binds the four actions on the Enhanced Input component.
	 *
	 * IT REFUSES QUIETLY AND COMPLETELY IF THE COMPONENT IS NOT AN ENHANCED ONE, which
	 * happens when the project's default input component class has been changed out from
	 * under this module. Logged at Error, because unlike a null action asset that is a
	 * project-settings fault with no configuration that fixes it here.
	 */
	virtual void SetupInputComponent() override;

private:
	/** The match subsystem of this controller's world, or null outside a Game/PIE world. */
	UStratMatchSubsystem* GetMatch() const;

	/** The spawned board, or null. Read through the subsystem so there is one owner of it. */
	AStratBoardActor* GetBoard() const;

	// The four Enhanced Input handlers. Each is one call to `HandleSelectionEvent`; the
	// only one that reads the cursor is `OnSelect`, because the other three are statements
	// about the selection rather than about a hex.
	void OnSelect();
	void OnCancel();
	void OnWait();
	void OnEndTurn();

	/**
	 * §2.11.1's selection state and the producer of `bDone` / `bLockedThisTurn`.
	 *
	 * BY VALUE, NOT A `UPROPERTY`, AND THAT IS THE POINT. See the header block: a reflected
	 * copy of the presentation bits on an actor is exactly what T-INT-05 forbids, and this
	 * member cannot become one because `FStratSelectionMachine` is not a reflected type.
	 */
	FStratSelectionMachine SelectionMachine;

	/**
	 * §2.11.6's guided opening. BY VALUE AND BESIDE THE MACHINE, for the machine's reasons.
	 *
	 * ARMED IN `BeginPlay` AND NOT IN THE CONSTRUCTOR, because arming needs a seeded bridge
	 * (for `GuidedOpeningHexes`) and the save slot (for the suppression bool), and neither
	 * exists when a controller is constructed. A controller whose `BeginPlay` runs before
	 * `StartMatch` finds no bridge and leaves guidance inactive, which is the same
	 * inactive state a scenario with no `guidedOpening` produces — see `TryArmGuidedOpening`.
	 */
	FStratGuidedOpening GuidedOpening;

	/**
	 * Whether `TryArmGuidedOpening` has succeeded, so it is not retried every refresh.
	 *
	 * IT IS NOT A MIRROR OF `GuidedOpening.IsActive()` and the two differ in the case that
	 * matters: an armed-then-skipped layer is armed and inactive, and re-arming it would
	 * undo the skip §2.11.6 calls permanent. This records a fact about THIS OBJECT'S OWN
	 * call history, which is the same justification `bAiTurnRunning` carries on the
	 * subsystem.
	 */
	bool bGuidanceArmed = false;

	/** Whether the "no input assets" warning has been said. Said once, not per click. */
	bool bReportedMissingInputAssets = false;
};
