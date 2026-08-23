// GDD §2.11.1 / §4.9 -- the player controller, implementation.
//
// THE WHOLE OF THE INPUT PATH IS BELOW AND IT IS SHORT ON PURPOSE. Four handlers, one
// cursor trace, one refresh. Every decision belongs to `FStratSelectionMachine`, every
// rules answer to `FStratBridge`, every reconciliation to `UStratMatchSubsystem`, and every
// coordinate to `AStratBoardActor`. If this file grows a branch on a rule, the branch is in
// the wrong file.
//
// `StratBridge.h` IS INCLUDED HERE AND NOT FROM THE HEADER. `StratPlayerController.h`
// declares a `UCLASS`, so UHT parses it, and the vendored `strat` headers must never get in
// front of the header tool. Nothing below names a `strat::` type: every bridge call is one
// of the engine-typed façade methods (`Turn`, `SideToMove`, `ReachableHexes`,
// `AttackTargetHexes`, `SubmitMoveToHex`, `SubmitAttackAtHex`, `SubmitEndTurn`,
// `StateHash`), which were added for this caller.

#include "StratPlayerController.h"

#include "StratBoardActor.h"
#include "StratBridge.h"
#include "StratMatchSubsystem.h"
#include "StratPlay.h"
#include "StratScoreboardHUD.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"

// `ULocalPlayer` is forward declared by `PlayerController.h` and `GetLocalPlayer()` returns
// one, but `ULocalPlayer::GetSubsystem<>` needs the definition. Measured as C2027 "use of
// undefined type 'ULocalPlayer'" plus a cascading C2275 and C2737 on the same line -- the
// last of which blamed the `const` on the variable and would have sent a reader to the wrong
// place entirely.
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"

AStratPlayerController::AStratPlayerController()
{
	// A TURN-BASED GAME IS PLAYED WITH A CURSOR. Set here rather than in `BeginPlay` so a
	// Blueprint subclass can still override it; nothing about hot-seat wants a hidden
	// cursor, and the alternative -- discovering it is hidden in PIE -- costs a build.
	bShowMouseCursor = true;

	// THIS CONTROLLER MUST TICK, AND NOT BECAUSE IT POLLS ANYTHING. It polls nothing: every
	// path below runs from an input trigger. It ticks because THE TRIGGERS ARE EVALUATED
	// DURING THE TICK. `APlayerController::TickActor` calls `TickPlayerInput`, which calls
	// `PlayerInput->ProcessInputStack(...)`, and that is the one place Enhanced Input walks
	// its trigger state machines and fires the delegates `SetupInputComponent` bound. A
	// controller with `bCanEverTick = false` still RECEIVES every key -- `InputKey` runs, the
	// key is buffered -- and then nothing ever drains the stack, so every action sits at
	// `None - 0.000s (false)` forever and not one bound delegate is called.
	//
	// MEASURED IN PHASE 6, at the cost of a long investigation. `bCanEverTick = false` stood
	// here from phase 4 with the reasoning "this controller polls nothing", which is true and
	// which does not survive the fact that undoes it. The symptom was maximally misleading:
	// standalone PIE showed `STRAT-PROBE ... InputKey key=... event=...` for every key,
	// `IgnoreInput=FALSE`, a valid `LocalPlayer_0` at `ControllerId=0`, all four `BindAction`
	// calls proven to have run (zero `is unset` warnings) -- and still zero `STRAT-CMD` lines.
	// The console kept working throughout, which read as "input is fine", but the console is
	// handled at the viewport-client layer UPSTREAM of player-input processing and needs no
	// controller tick at all. Do not re-derive "polls nothing, so need not tick" from this
	// file; that derivation is what cost phase 6.
	//
	// SET EXPLICITLY TO `true` RATHER THAN BY DELETING THE LINE. `APlayerController`'s own
	// constructor already enables ticking, so removing the assignment would work -- and would
	// leave nothing here for the next reader to hit before writing `false` again.
	// `bTickEvenWhenPaused` is deliberately not touched: `APlayerController` sets it, and
	// input during pause is its concern rather than this class's.
	PrimaryActorTick.bCanEverTick = true;
}

void AStratPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// THE MAPPING CONTEXT, IF THERE IS ONE. Null until phase 5 authors the asset, and that
	// is a supported configuration rather than a tolerated one -- see the header block. The
	// warning is emitted once from here; `SetupInputComponent` says the rest.
	if (SelectionMappingContext != nullptr)
	{
		if (UEnhancedInputLocalPlayerSubsystem* const InputSubsystem =
			    ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			InputSubsystem->AddMappingContext(SelectionMappingContext, MappingPriority);
		}
		else
		{
			// Reachable on a controller with no local player -- a remote or a spectator
			// controller. Not a fault of this class's configuration, so it is said plainly
			// rather than blamed on the asset.
			UE_LOG(LogStratPlay, Warning,
				TEXT("%s has a mapping context but no local player to add it to."), *GetName());
		}
	}
	else if (!bReportedMissingInputAssets)
	{
		bReportedMissingInputAssets = true;
		UE_LOG(LogStratPlay, Warning,
			TEXT("%s has no SelectionMappingContext; input is inert until a Blueprint default sets one."),
			*GetName());
	}

	// THE VIEW-DECORATOR SEAM, TAKEN BEFORE THE FIRST REFRESH BELOW. From here on, every
	// model this world draws from -- this controller's refreshes, the subsystem's own
	// `RefreshPresentation`, and `StartMatchInternal`'s first reconciliation -- passes
	// through `DecorateForPresentation` on its way to `ApplyView`. See `FStratViewDecorator`
	// for the defect this closes and for why the binding is single, weak and unreflected.
	//
	// NO ORDER IS ASSUMED. The subsystem exists with the world, so this binding lands whether
	// or not the GameMode has started the match; if the match started first, its
	// reconciliation ran undecorated and the refresh below immediately applies a decorated
	// model over it.
	if (UStratMatchSubsystem* const Match = GetMatch())
	{
		Match->SetViewDecorator(
			FStratViewDecorator::CreateUObject(this, &AStratPlayerController::DecorateForPresentation));
	}
	else
	{
		// A world with no match subsystem draws no match at all, which `RefreshFromMachine`
		// reports below in its own words. Said at Verbose here for the same reason that
		// refusal is: it is the ordinary state of a world this game is not running in.
		UE_LOG(LogStratPlay, Verbose,
			TEXT("%s found no UStratMatchSubsystem to register a view decorator with."), *GetName());
	}

	// THE FIRST FRAME'S OVERLAYS. Nothing is selected, so both come back empty and the
	// board's two overlay components clear -- which is the correct screen and not a no-op:
	// `AStratBoardActor` spawns with whatever instance counts its Blueprint default left.
	//
	// A REFUSAL HERE IS ORDINARY. `AStratGameMode::BeginPlay` starts the match, and actor
	// BeginPlay order is not guaranteed, so this can legitimately run before there is a
	// seeded bridge to build a model from. Logged at Verbose for that reason: the first
	// input event refreshes again and the transient case would otherwise be a warning that
	// means nothing.
	FString RefreshReason;
	if (!RefreshFromMachine(RefreshReason))
	{
		UE_LOG(LogStratPlay, Verbose,
			TEXT("%s could not paint an initial screen (this is ordinary before the match starts): %s"),
			*GetName(), *RefreshReason);
	}
}

void AStratPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// GIVEN BACK WHERE IT WAS TAKEN. Unconditional and not guarded by "did I bind", because
	// `ClearViewDecorator` is safe with nothing bound.
	//
	// IT CLEARS BY POSITION AND NOT BY IDENTITY, and the limitation is written down rather
	// than argued away: in a world where a SECOND controller had since registered, this would
	// drop that controller's binding instead of its own. There is no such world in this
	// milestone -- Sec 2.11's hot seat is one controller holding two seats, and
	// `FStratViewDecorator` records that as the reason the seam is single rather than
	// multicast. What discharges it is the day a second controller can exist: the seam grows
	// an owner argument, or the delegate grows an identity check, and this call passes `this`.
	if (UStratMatchSubsystem* const Match = GetMatch())
	{
		Match->ClearViewDecorator();
	}

	Super::EndPlay(EndPlayReason);
}

void AStratPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* const EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInput == nullptr)
	{
		// A PROJECT-SETTINGS FAULT AND NOT A CONFIGURATION ONE. `DefaultPlayerInputClass`
		// and `DefaultInputComponentClass` are project settings; no property on this class
		// or its Blueprint can fix this, which is why it is an Error where a null action
		// asset is a Warning.
		UE_LOG(LogStratPlay, Error,
			TEXT("%s: InputComponent is not a UEnhancedInputComponent; no Stratocracy input is bound."),
			*GetName());
		return;
	}

	// EACH BINDING IS GUARDED SEPARATELY, so that authoring three of the four assets binds
	// three actions rather than none. `BindAction` with a null action is a check() in the
	// engine, so the guard is required and not defensive.
	//
	// `ETriggerEvent::Started` ON ALL FOUR. Each of these is a discrete decision -- a click,
	// a cancel, a wait, an end-turn -- and `Triggered` would repeat them for the length of a
	// held key. A repeated Move is refused by the rules module the second time (the unit has
	// moved), so the visible symptom would be a log full of refusals rather than a wrong
	// board; it is still the wrong event.
	// AND EACH NULL BRANCH NOW SAYS SO BY NAME. THIS PART IS PERMANENT. It outlived the
	// temporary phase 6 diagnostics on purpose and is not to be stripped with them. Until
	// phase 6 these four guards were silent on the null side, so a log in which no input ever
	// arrived was consistent with four bindings AND with zero bindings, and the difference
	// between those two is the difference between an asset gap and an engine one. Phase 6
	// spent a standalone PIE session, a `showdebug enhancedinput` overlay and a
	// `GetAll EnhancedPlayerInput` probe establishing by other means what these four lines
	// state directly. "The defaults are set" is now an observation rather than an assertion.
	//
	// WARNING AND NOT ERROR, because a null action asset is a Blueprint-default gap that a
	// property on this class fixes -- unlike the non-Enhanced `InputComponent` above, which no
	// configuration here can fix and which is therefore an Error. The property NAME is in each
	// message because that is the thing the reader must go and set.
	if (SelectAction != nullptr)
	{
		EnhancedInput->BindAction(SelectAction, ETriggerEvent::Started, this,
			&AStratPlayerController::OnSelect);
	}
	else
	{
		UE_LOG(LogStratPlay, Warning,
			TEXT("%s: SelectAction is unset; no primary-click binding exists."), *GetName());
	}
	if (CancelAction != nullptr)
	{
		EnhancedInput->BindAction(CancelAction, ETriggerEvent::Started, this,
			&AStratPlayerController::OnCancel);
	}
	else
	{
		UE_LOG(LogStratPlay, Warning,
			TEXT("%s: CancelAction is unset; no cancel binding exists."), *GetName());
	}
	if (WaitAction != nullptr)
	{
		EnhancedInput->BindAction(WaitAction, ETriggerEvent::Started, this,
			&AStratPlayerController::OnWait);
	}
	else
	{
		UE_LOG(LogStratPlay, Warning,
			TEXT("%s: WaitAction is unset; no wait binding exists."), *GetName());
	}
	if (EndTurnAction != nullptr)
	{
		EnhancedInput->BindAction(EndTurnAction, ETriggerEvent::Started, this,
			&AStratPlayerController::OnEndTurn);
	}
	else
	{
		UE_LOG(LogStratPlay, Warning,
			TEXT("%s: EndTurnAction is unset; no end-turn binding exists."), *GetName());
	}

	// THE FIFTH BINDING, AND IT IS `Started` FOR A REASON THE OTHER FOUR DO NOT HAVE. This
	// one is a TOGGLE, so `Triggered` on a held key would open and close the menu once per
	// frame -- a symptom that reads as a flickering panel rather than as a wrong trigger
	// event, and would send the next reader to the widget.
	if (OpenProductionMenuAction != nullptr)
	{
		EnhancedInput->BindAction(OpenProductionMenuAction, ETriggerEvent::Started, this,
			&AStratPlayerController::OnToggleProductionMenu);
	}
	else
	{
		UE_LOG(LogStratPlay, Warning,
			TEXT("%s: OpenProductionMenuAction is unset; no production-menu binding exists."),
			*GetName());
	}
}

// ---------------------------------------------------------------------------
// Where things are.
// ---------------------------------------------------------------------------

UStratMatchSubsystem* AStratPlayerController::GetMatch() const
{
	const UWorld* const World = GetWorld();
	return World != nullptr ? World->GetSubsystem<UStratMatchSubsystem>() : nullptr;
}

AStratBoardActor* AStratPlayerController::GetBoard() const
{
	// THROUGH THE SUBSYSTEM AND NEVER A `TActorIterator`. The subsystem spawned the board
	// and destroys it; a second way to find one would find a board this controller is not
	// playing on the day a map contains two.
	const UStratMatchSubsystem* const Match = GetMatch();
	return Match != nullptr ? Match->GetBoard() : nullptr;
}

bool AStratPlayerController::HexUnderCursor(FIntPoint& OutHex)
{
	OutHex = FIntPoint::ZeroValue;

	const AStratBoardActor* const Board = GetBoard();
	if (Board == nullptr)
	{
		return false;
	}

	FHitResult Hit;
	if (!GetHitResultUnderCursor(ECC_Visibility, /*bTraceComplex=*/false, Hit))
	{
		return false;
	}

	// THE COMPONENT AND THE INSTANCE INDEX, HANDED OVER UNINTERPRETED. `FHitResult::Item`
	// carries the instance index for an instanced-static-mesh hit, and `HexAtInstance` is
	// the board's own lookup into the list it built. This class performs no conversion at
	// all -- see the header block on why no inverse of `WorldLocationOfHex` exists to call.
	//
	// A HIT ON SOMETHING THAT IS NOT A BOARD TILE RETURNS FALSE, and so does a hit on a
	// component the board does not own. Both mean "the cursor is not on the board", which is
	// an ordinary answer and not a fault.
	return Board->HexAtInstance(Hit.GetComponent(), Hit.Item, OutHex);
}

// ---------------------------------------------------------------------------
// The input handlers. Four one-line calls.
// ---------------------------------------------------------------------------

void AStratPlayerController::OnSelect()
{
	FIntPoint Hex;
	if (!HexUnderCursor(Hex))
	{
		// The cursor is not on the board. NOT forwarded as a Cancel: a click on the sky is
		// not a statement about the selection, and dropping a selection on a stray click is
		// the interaction players complain about.
		return;
	}

	FString Reason;
	if (!HandleSelectionEvent(EStratSelectionEvent::HexPrimary, Hex, Reason))
	{
		// AT LOG AND NOT WARNING. Most of what arrives here is a player clicking somewhere
		// the rules do not allow, which is the interface working. `STRAT-CMD refused` is
		// the line that carries an actual rules refusal.
		UE_LOG(LogStratPlay, Log, TEXT("Selection: %s"), *Reason);
	}
}

void AStratPlayerController::OnCancel()
{
	FString Reason;
	if (!HandleSelectionEvent(EStratSelectionEvent::Cancel, FIntPoint::ZeroValue, Reason))
	{
		UE_LOG(LogStratPlay, Log, TEXT("Cancel: %s"), *Reason);
	}
}

void AStratPlayerController::OnWait()
{
	FString Reason;
	if (!HandleSelectionEvent(EStratSelectionEvent::Wait, FIntPoint::ZeroValue, Reason))
	{
		UE_LOG(LogStratPlay, Log, TEXT("Wait: %s"), *Reason);
	}
}

void AStratPlayerController::OnEndTurn()
{
	FString Reason;
	if (!HandleSelectionEvent(EStratSelectionEvent::EndTurn, FIntPoint::ZeroValue, Reason))
	{
		UE_LOG(LogStratPlay, Log, TEXT("End turn: %s"), *Reason);
	}
}

void AStratPlayerController::OnToggleProductionMenu()
{
	FString Reason;
	if (!ToggleProductionMenu(Reason))
	{
		// AT LOG AND NOT WARNING, on `OnSelect`'s rule: most of what arrives here is a
		// player pressing the key with the cursor off the board, which is the interface
		// working.
		UE_LOG(LogStratPlay, Log, TEXT("Production menu: %s"), *Reason);
	}
}

// ---------------------------------------------------------------------------
// §2.11.5's production menu. Which hex, and nothing else.
// ---------------------------------------------------------------------------

bool AStratPlayerController::GetProductionTargetHex(FIntPoint& OutHex) const
{
	OutHex = bHasProductionTargetHex ? ProductionTargetHex : FIntPoint::ZeroValue;
	return bHasProductionTargetHex;
}

bool AStratPlayerController::ToggleProductionMenu(FString& OutFailureReason)
{
	OutFailureReason.Reset();

	AStratScoreboardHUD* const Hud = Cast<AStratScoreboardHUD>(GetHUD());
	if (Hud == nullptr)
	{
		// THE HUD IS WHERE WIDGET CREATION LIVES, and `StratScoreboardHUD.h` records why:
		// `CreateWidget` and `AddToViewport` mean `UMG`, `Slate` and `SlateCore`, and
		// `StratPlay.Build.cs` naming those three is a structural cost this project has
		// already declined to pay once. So no HUD of that class means no menu, and it is
		// reported rather than worked around.
		OutFailureReason = TEXT("this controller has no AStratScoreboardHUD to host a production menu");
		return false;
	}

	UStratMatchSubsystem* const Match = GetMatch();

	// --- Already open: this press closes it. -------------------------------
	if (Hud->IsProductionMenuWidgetOpen())
	{
		Hud->CloseProductionMenuWidget();

		// TWO ACTS ON TWO OBJECTS, IN THIS ORDER, AND THE ORDER IS THE CONTRACT. The panel
		// comes down first, then the rows are cleared. The reverse would leave a live panel
		// bound to an empty `ProductionMenu` array for however long the two lines are apart,
		// which is a screen showing a menu the subsystem says is not open.
		//
		// A NULL SUBSYSTEM IS NOT A FAILURE HERE. The panel is down either way, which is
		// what the player asked for; there is simply nothing left holding rows.
		if (Match != nullptr)
		{
			Match->CloseProductionMenu();
		}

		// THE LATCH IS CLEARED WITH THE PANEL AND NOT WHEN THE NEXT ONE OPENS. Leaving it
		// set would have `GetProductionTargetHex` answer true with no menu on screen, and
		// the accessor's whole first channel is "is there one".
		bHasProductionTargetHex = false;
		ProductionTargetHex     = FIntPoint::ZeroValue;

		return true;
	}

	// --- Not open: this press opens one on the hex under the cursor. -------

	// §2.8: A FINISHED MATCH DOES NOT OPEN A MENU. Only the OPEN path is gated -- the close
	// branch above runs whatever the match's state, because taking a panel down is never a
	// command and a player left unable to dismiss a menu on a finished match would be worse
	// than the defect this gate closes.
	//
	// THROUGH THE SUBSYSTEM RATHER THAN A MODEL, unlike `HandleSelectionEvent`'s gate: this
	// function builds no model of its own, and building one here purely to ask would be the
	// second model that gate exists to avoid.
	if (Match != nullptr && Match->IsMatchConcluded())
	{
		OutFailureReason = StratMatchConcludedRefusalText();
		return false;
	}

	FIntPoint Hex;
	if (!HexUnderCursor(Hex))
	{
		// The same ordinary answer `OnSelect` gets. Not a fault, and nothing is latched --
		// see `GetProductionTargetHex` on why a hex cannot signal its own absence.
		OutFailureReason = TEXT("the cursor is not on the board");
		return false;
	}

	// LATCHED BEFORE THE WIDGET IS CREATED, WHICH IS THE ONLY ORDER THAT WORKS. The asset's
	// `Construct` runs inside `AddToViewport`, inside `OpenProductionMenuWidget` below, and
	// `Construct` is where it reads this value. Latching afterwards would hand the first
	// menu of every session a `false`.
	ProductionTargetHex     = Hex;
	bHasProductionTargetHex = true;

	if (!Hud->OpenProductionMenuWidget(OutFailureReason))
	{
		// UNWOUND, so that a refused open leaves no latch behind claiming a menu is about a
		// hex. The refusal reason is the HUD's own, forwarded unchanged.
		bHasProductionTargetHex = false;
		ProductionTargetHex     = FIntPoint::ZeroValue;
		return false;
	}

	// NO `RefreshProductionMenu` CALL, AND NO `RefreshMenu` CALL. The widget asks the
	// subsystem for its own rows from `Construct`, using the hex latched three lines above.
	// A refresh started here would be a second author of when the menu's contents are
	// decided, and this class would then have to know what a menu is made of -- which is the
	// whole of what it is arranged not to know.
	return true;
}

// ---------------------------------------------------------------------------
// Decide, submit, refresh.
// ---------------------------------------------------------------------------

bool AStratPlayerController::HandleSelectionEvent(EStratSelectionEvent Event,
                                                  FIntPoint            Hex,
                                                  FString&             OutFailureReason)
{
	OutFailureReason.Reset();

	UStratMatchSubsystem* const Match = GetMatch();
	if (Match == nullptr)
	{
		OutFailureReason = TEXT("no UStratMatchSubsystem in this world");
		return false;
	}

	FStratBridge* const Bridge = Match->GetBridge();
	if (Bridge == nullptr)
	{
		OutFailureReason = TEXT("no match is live");
		return false;
	}

	// BUILT FRESH, NOT READ FROM `GetViewModel()`. That accessor is documented as a record
	// of what was drawn and not a source; driving the machine from it would make what a
	// click means depend on when someone last called `ApplyView`.
	FStratViewModel Model;
	if (!Match->BuildViewModel(Model, OutFailureReason))
	{
		return false;
	}

	// ---- §2.8's end of match, before anything else looks at the event ------
	// FIRST, AHEAD OF THE GUIDED OPENING AND AHEAD OF THE MACHINE. A finished match has no
	// beat to observe and no selection to advance, and running either would move state on a
	// match that is over. This is the gate whose absence let a human inherit the AI's side
	// and go on playing it -- measured 2026-08-23, recorded in `global.md`.
	//
	// ON THE MODEL BUILT SIX LINES ABOVE, WHICH IS THE POINT OF THE FREE FUNCTION. Asking
	// the subsystem would build a second model at a second instant; this gate is the same
	// value the rest of this function acts on, by construction.
	//
	// INERT, NOT REFUSED, AND NO REFRESH. Nothing reaches `FStratBridge`, no machine state
	// moves, and the screen already shows the final board -- `ApplyView` drew it on the
	// refresh that concluded the match. Refreshing here would repaint an unchanged screen
	// once per click for the rest of the session.
	//
	// IT DOES NOT CHECK WHOSE TURN IT IS and must not: that is the rules module's answer and
	// the selection machine already asks for it. This gate is about the MATCH.
	{
		FString ConcludedReason;
		if (!StratMatchAcceptsPlayerCommands(Model, ConcludedReason))
		{
			OutFailureReason = ConcludedReason;
			return false;
		}
	}

	// The one production query adapter. Constructed per event and holding nothing: it is a
	// borrowed pointer and a pair of forwards.
	const FStratBridgeRulesQuery Query(Bridge);

	// ---- §2.11.6's guided opening, before the machine sees the event --------
	// ARMED HERE RATHER THAN IN `BeginPlay`, and observed BEFORE the gates below so that a
	// gate never answers from a stale beat — see `TryArmGuidedOpening`'s declaration.
	TryArmGuidedOpening();
	GuidedOpening.Observe(Model, SelectionMachine);

	// ---- The Q27 input gates (§4.7, ruled) ----------------------------------
	// §2.11.6-B beat 1a: "End Turn is inert until that Infantry has moved … and **Space** is
	// inert for it on the same footing as End Turn and for the same reason."
	//
	// THEY RETURN BEFORE `HandleEvent`, WHICH IS THE WHOLE MECHANISM. `Wait` is the event
	// that sets `bDone` without a rules command, so intercepting it AFTER the machine had
	// seen it would leave the unit retired with beat 1a still outstanding — End Turn inert
	// and no move left to satisfy it, which is exactly the deadlock §2.11.6 says these two
	// closures exist to make unreachable. Nothing here touches `FStratBridge`: an inert
	// input is an input that was never asked about, not a command the rules module refused.
	{
		const bool bEndTurnBlocked = (Event == EStratSelectionEvent::EndTurn)
			&& GuidedOpening.IsEndTurnGated();

		const bool bWaitBlocked = (Event == EStratSelectionEvent::Wait)
			&& GuidedOpening.IsUnitInputGated(SelectionMachine.GetSelectedUnitId());

		if (bEndTurnBlocked || bWaitBlocked)
		{
			// The player-facing sentence is the GDD's hover string, so the log line, the
			// hover and the strip cannot say three different things.
			OutFailureReason = FStratGuidedOpening::EndTurnGateHoverText().ToString();

			// REFRESHED ANYWAY, on the same reasoning the machine's own refusal path gives:
			// nothing moved, but the strip and the dimming must be current for the player to
			// see WHY nothing moved.
			FString RefreshReason;
			RefreshFromMachine(RefreshReason);
			return false;
		}
	}

	const FStratSelectionOutcome Outcome = SelectionMachine.HandleEvent(Event, Hex, Model, Query);

	// SUBMIT FIRST, NOTIFY SECOND, AND ONLY ON ACCEPTANCE. `NotifyCommandApplied` is
	// documented "call it only on acceptance"; calling it before the bridge answered would
	// advance the machine past a command the rules module refused. `StratSubmitSelectionCommand`
	// is also where `STRAT-CMD accepted` is emitted, and it emits it for the same reason --
	// the word `accepted` is a claim about the rules module and not about the click.
	// §2.11.6-B beat 1a's other closed route: "its attack targets are not lit, so the
	// SELECTED → attack transition (§2.11.1) is closed to it".
	//
	// CHECKED AFTER `HandleEvent` AND BEFORE THE SUBMIT, which is the only place it can be:
	// a primary click on an enemy hex is `HexPrimary` and nothing before the machine has run
	// can tell it apart from a move. `NotifyCommandApplied` is not reached, so the machine's
	// DONE set does not move either — the attack is inert in exactly the way Space is.
	//
	// NOT LIT AND NOT LEGAL ARE BOTH COVERED. `RefreshFromMachine` clears the target overlay
	// for this unit, so the click is one the UI never invited; this arm is what makes the
	// invitation's absence binding rather than cosmetic.
	if (Outcome.Command == EStratSelectionCommand::Attack
		&& GuidedOpening.IsUnitInputGated(Outcome.UnitId))
	{
		OutFailureReason = FStratGuidedOpening::EndTurnGateHoverText().ToString();

		FString GatedRefreshReason;
		RefreshFromMachine(GatedRefreshReason);
		return false;
	}

	if (Outcome.Command != EStratSelectionCommand::None)
	{
		FString SubmitReason;
		if (!StratSubmitSelectionCommand(*Bridge, Outcome, SubmitReason))
		{
			OutFailureReason = SubmitReason;

			// REFRESHED ANYWAY. A refusal changes no rules state, but the machine's
			// selection may have moved, and leaving the overlays describing the previous
			// selection is a screen that disagrees with the machine.
			FString RefreshReason;
			RefreshFromMachine(RefreshReason);
			return false;
		}

		SelectionMachine.NotifyCommandApplied(Outcome);

		// ---- §2.9's opponent, if the command just handed play to it -----------
		// AFTER `NotifyCommandApplied` AND BEFORE THE REFRESH BELOW, and both halves of that
		// placement are load-bearing. After, because the machine's DONE set must be cleared by
		// the human's own accepted EndTurn before the AI's commands change the board -- calling
		// it first would leave the machine advancing across a turn boundary it had not been
		// told about. Before, because the refresh at the end of this function is the DECORATED
		// one, and it is the screen the player is left looking at: refreshing first would paint
		// the pre-AI board and then let the subsystem's own undecorated refresh overwrite it,
		// so the last thing drawn would be the one without `bDone` on it.
		//
		// ASKED AFTER EVERY ACCEPTED COMMAND AND NOT ONLY AFTER AN EndTurn. `RunAiTurnsIfDue`
		// reads `sideToMove` off the rules module; this file inferring "an EndTurn means it is
		// now the other side's turn" would be a turn rule restated in the input path, and it
		// would be wrong the day a scenario gives a side two turns in a row.
		//
		// ITS REFUSAL DOES NOT FAIL THE PLAYER'S COMMAND. The human's command applied -- that
		// is what the accepted `STRAT-CMD` line above claims and it remains true. The AI's
		// failure is reported on its own line, in `FStratAiTurnRunner`'s words, so the two are
		// not conflated in a log.
		if (UStratMatchSubsystem* const AiMatch = GetMatch())
		{
			FString AiReason;
			if (!AiMatch->RunAiTurnsIfDue(AiReason))
			{
				UE_LOG(LogStratPlay, Warning, TEXT("The AI turn refused: %s"), *AiReason);
			}
		}
	}

	// The machine's own refusal -- an illegal click, a finished unit, a locked unit --
	// carried out even though the refresh below still runs. It is a sentence for a human and
	// nothing branches on it.
	if (!Outcome.FailureReason.IsEmpty())
	{
		OutFailureReason = Outcome.FailureReason;
	}

	FString RefreshReason;
	if (!RefreshFromMachine(RefreshReason))
	{
		if (OutFailureReason.IsEmpty())
		{
			OutFailureReason = RefreshReason;
		}
		return false;
	}

	return OutFailureReason.IsEmpty();
}

bool AStratPlayerController::RefreshFromMachine(FString& OutFailureReason)
{
	OutFailureReason.Reset();

	UStratMatchSubsystem* const Match = GetMatch();
	if (Match == nullptr)
	{
		OutFailureReason = TEXT("no UStratMatchSubsystem in this world");
		return false;
	}

	FStratBridge* const Bridge = Match->GetBridge();
	if (Bridge == nullptr)
	{
		OutFailureReason = TEXT("no match is live");
		return false;
	}

	FStratViewModel Model;
	if (!Match->BuildViewModel(Model, OutFailureReason))
	{
		return false;
	}

	// THE SEAM, NOW NAMED. `bDone`, `bLockedThisTurn` and Sec 2.11.6's guidance block reach
	// the screen on the model on its way to `ApplyView`, and the sequence that writes them
	// lives in `DecorateForPresentation` so that the subsystem's own reconciles can run the
	// same one. `StratViewModel.h` records the obligation and `UStratMatchSubsystem.h` split
	// `BuildViewModel` from `ApplyView` to leave room for exactly this line: "the phase-4 path
	// is build -> decorate -> `ApplyView`, with the decorated model being the one and only
	// description of the screen."
	//
	// CALLED DIRECTLY AND NOT THROUGH `BuildViewModelForPresentation`, which would have been
	// the tidier-looking shape and is the wrong one here. That function runs the REGISTERED
	// decorator, and the registration is taken in `BeginPlay` -- so a controller driven
	// before or without `BeginPlay`, which is every fixture that spawns one into a world it
	// built itself, would silently stop decorating and this path's whole purpose would depend
	// on an actor lifecycle step. Calling the method directly makes this path decorate
	// because it is this controller, not because a registration happened to be in place.
	//
	// IT CANNOT DOUBLE-DECORATE. `BuildViewModel` above never runs the delegate; only
	// `BuildViewModelForPresentation` does, and this function does not call it. Each model is
	// decorated exactly once no matter which path built it.
	DecorateForPresentation(Model);

	Match->ApplyView(Model);

	// The two overlays, from the rules module through the machine. `BuildOverlays` computes
	// neither set; with nothing selected both are empty and the overlays clear.
	AStratBoardActor* const Board = Match->GetBoard();
	if (Board != nullptr)
	{
		TArray<FIntPoint> ReachHexes;
		TArray<FIntPoint> TargetHexes;
		FString           OverlayReason;
		SelectionMachine.BuildOverlays(Model, FStratBridgeRulesQuery(Bridge),
			ReachHexes, TargetHexes, OverlayReason);

		// §2.11.6-B beat 1a: "its attack targets are not lit". The REACH overlay is left
		// alone deliberately — beat 1a's own directive is "Lit hexes are its true reach", so
		// suppressing that would put out the light the beat is teaching by.
		//
		// CLEARED HERE RATHER THAN NOT COMPUTED. `BuildOverlays` is the selection machine's
		// and asking it to know about beats would put a guidance rule inside a state machine
		// that does not own one — `FStratSelectionMachine`'s own block warns against exactly
		// that. The set is discarded one line after it is produced, at the cost of one
		// `AttackTargetHexes` call the player will not see the answer to.
		if (GuidedOpening.IsUnitInputGated(SelectionMachine.GetSelectedUnitId()))
		{
			TargetHexes.Reset();
		}

		Board->ShowReach(ReachHexes);
		Board->ShowTargets(TargetHexes);

		if (!OverlayReason.IsEmpty())
		{
			// The board reconciled and the highlight did not. Reported rather than folded
			// into the return, because a match that is correctly drawn with no overlay is
			// not the same failure as a match that failed to draw.
			OutFailureReason = OverlayReason;
			return false;
		}
	}

	// THE SCOREBOARD REFRESHES ITSELF FROM THE BRIDGE, not from the model above --
	// `UStratMatchSubsystem::RefreshPresentation` holds the same line, for the same reason:
	// giving the panel a second source is two answers to what one panel shows. Asked last so
	// a refusal names the panel rather than the board.
	if (AStratScoreboardHUD* const HUD = Cast<AStratScoreboardHUD>(GetHUD()))
	{
		FString ScoreboardReason;
		if (!HUD->RefreshScoreboard(ScoreboardReason))
		{
			OutFailureReason = FString::Printf(TEXT("scoreboard refresh refused: %s"),
				*ScoreboardReason);
			return false;
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// §2.11.6's guided opening — arming, and the one control a widget calls.
// ---------------------------------------------------------------------------

void AStratPlayerController::DecorateForPresentation(FStratViewModel& Model)
{
	// ARMED FROM HERE, LAZILY, per `TryArmGuidedOpening`'s own declaration: actor `BeginPlay`
	// order between this class and `AStratGameMode` is not something this file may assume, so
	// arming is attempted on every decoration and returns immediately once it has taken.
	// Reaching this function from the subsystem's reconcile rather than from a refresh does
	// not change that -- it adds earlier opportunities to arm, which is the direction that
	// helps.
	TryArmGuidedOpening();

	// THE ORDER MATTERS IN ONE DIRECTION ONLY. `Observe` writes the lock set that
	// `SelectionMachine.DecorateViewModel` then publishes as `bLockedThisTurn`, so an
	// `Observe` after it would publish last frame's locks. `FStratGuidedOpening::
	// DecorateViewModel` writes the guidance block and touches no unit bit, which is why it
	// can sit on either side of the machine's call and sits after it for readability.
	GuidedOpening.Observe(Model, SelectionMachine);

	SelectionMachine.DecorateViewModel(Model);
	GuidedOpening.DecorateViewModel(Model);
}

void AStratPlayerController::TryArmGuidedOpening()
{
	if (bGuidanceArmed)
	{
		return;
	}

	UStratMatchSubsystem* const Match = GetMatch();
	if (Match == nullptr)
	{
		return;
	}

	const FStratBridge* const Bridge = Match->GetBridge();
	if (Bridge == nullptr || !Bridge->IsSeeded())
	{
		// Not yet. `StartMatch` has not finished, so there is no scenario to read
		// `guidedOpening` off and no board for beat 1a to be about. Silent: this is the
		// ordinary state for however many refreshes precede the first seeded one, and a log
		// line here would fire once per refresh for a condition that is not a fault.
		return;
	}

	// §2.11.6: "Any completed match on the save skips all guidance automatically."
	// An empty slot name means the configured default; a slot that does not exist answers
	// false, which is correct — a save with no history has no completed match on it.
	const bool bSuppressed = Match->HasCompletedAMatchOnSave(FString());

	// THE GUIDED SEAT IS THE VIEWING SIDE AT ARMING TIME AND IS FIXED FROM THEN ON. §2.11.6
	// is a first-session onboarding against §2.9's Easy AI, so the human holds one seat for
	// the match; `FStratGuidedOpening::Begin` takes the side by argument precisely so a
	// hot-seat hand-over — which MOVES the viewing side — cannot move the guided seat under
	// the beats. A both-human hot-seat match therefore guides the seat that was on screen
	// when the match started, which is the seat §2.11.6 means by "the player".
	GuidedOpening.Begin(*Bridge, Match->GetViewingSide(), bSuppressed);
	bGuidanceArmed = true;
}

void AStratPlayerController::SkipGuidance()
{
	GuidedOpening.SkipGuidance();

	// IN THE SAME FRAME. §2.11.6 requires the ring and the turn-1a marker to "clear in the
	// same frame as the strip", and the model is only rewritten by a refresh — so the refresh
	// is part of the control and not a courtesy. `Observe` inside it is also what clears the
	// locks, which `SkipGuidance` deliberately does not do for itself.
	FString RefreshReason;
	if (!RefreshFromMachine(RefreshReason))
	{
		UE_LOG(LogStratPlay, Warning,
			TEXT("Guidance was skipped but the refresh refused: %s"), *RefreshReason);
	}
}

bool AStratPlayerController::IsGuidanceActive() const
{
	return GuidedOpening.IsActive();
}
