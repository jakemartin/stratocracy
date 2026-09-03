// GDD §2.11.1 / §4.9 -- the player controller, implementation.
//
// THE WHOLE OF THE INPUT PATH IS BELOW AND IT IS SHORT ON PURPOSE. One handler per action,
// ONE cursor trace shared by all of them, one refresh. [AMENDED, wave 0: "Four handlers, one
// cursor trace, one refresh." --
// a count of a growing set, wrong since `OpenProductionMenuAction` landed. The invariant it
// was really about is the ONE cursor trace, which is unchanged: `HexUnderCursor` is still
// the only route from a cursor to a hex in this project, and the hover reuses it rather than
// adding a second.] Every decision belongs to `FStratSelectionMachine`, every
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
#include "StratForecastQuery.h"
#include "StratMatchSubsystem.h"
#include "StratPathPreviewQuery.h"
#include "StratPlay.h"
#include "StratScoreboardHUD.h"
// IWYU: this file now names `StratDecorateInfoPanel` directly. It arrived transitively
// through `StratForecastQuery.h` before, which is a dependency on somebody else's include
// list rather than on a header.
#include "StratViewModel.h"

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

// The input claim's two engine reads. `Engine/GameViewportClient.h` for the client whose
// `bIgnoreInput` the title screen leaves set, and `GameFramework/InputSettings.h` for the
// project's own capture and lock defaults -- read rather than copied, so this file holds no
// second opinion about what `Config/DefaultInput.ini` says. `Misc/App.h` carries
// `FApp::CanEverRender`, which is the same test `UGameViewportClient::Init` uses to decide
// whether those two modes apply at all.
#include "Engine/GameViewportClient.h"
#include "GameFramework/InputSettings.h"
#include "Misc/App.h"

AStratPlayerController::AStratPlayerController()
{
	// A TURN-BASED GAME IS PLAYED WITH A CURSOR. Set here rather than in `BeginPlay` so a
	// Blueprint subclass can still override it; nothing about hot-seat wants a hidden
	// cursor, and the alternative -- discovering it is hidden in PIE -- costs a build.
	bShowMouseCursor = true;

	// THIS CONTROLLER MUST TICK, AND THERE ARE NOW TWO INDEPENDENT REASONS. The original one
	// is below and is unchanged. [AMENDED 2026-08-27: this paragraph opened "AND NOT BECAUSE
	// IT POLLS ANYTHING. It polls nothing: every path below runs from an input trigger." That
	// was true when written and is now false in exactly one place -- `Tick` polls the cursor
	// for the hover, because the Enhanced Input route for it was measured never to fire; that
	// method's block carries the measurement. The correction is recorded here rather than in
	// the new code alone, because a reader arriving at `bCanEverTick` with a performance
	// question would read the old sentence and conclude the tick body was empty. NOTE WHAT IS
	// NOT AMENDED: the reason below is not weakened by the second consumer, and turning the
	// tick off would now break input twice over rather than once.]
	//
	// IT TICKS BECAUSE THE TRIGGERS ARE EVALUATED
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
	// `IgnoreInput=FALSE`, a valid `LocalPlayer_0` at `ControllerId=0`, EVERY `BindAction` call
	// THAT EXISTED THEN proven to have run (zero `is unset` warnings) -- and still zero
	// `STRAT-CMD` lines. [AMENDED, wave 0: "all four `BindAction` calls". A measurement, and
	// the measurement is unchanged -- what is removed is only the implication that four is
	// still the number, which a reader of this paragraph had no way to sort from the live
	// counts that used to sit sixty lines below it.]
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


	// THE INPUT THIS CONTROLLER NEEDS, CLAIMED BEFORE ANYTHING IS BOUND TO IT. Not a
	// reordering hazard and not an ordering assumption: the claim touches only the viewport
	// client and the mapping-context add below touches only the local player's Enhanced Input
	// subsystem, so neither can see the other. It goes first because a reader looking for
	// "why does no key work" should meet it before three screens of asset warnings.
	ClaimGameInput();

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

// ---------------------------------------------------------------------------
// The game-input claim. `RestoreProjectInputState` carries the measurement and the reasoning
// in the header; these two are the whole of the code.
// ---------------------------------------------------------------------------

void AStratPlayerController::ClaimGameInput()
{
	// SET TO THE FAILING VALUE FIRST AND RAISED ONLY ON SUCCESS, so there is no arm through
	// this function that leaves `NotAttempted` behind. `NotAttempted` means BeginPlay did not
	// call this, and it must keep meaning only that -- it is the value the call-site clause
	// reads to tell a live call from a deleted one.
	LastInputClaim = EStratInputClaim::NoViewport;

	const UWorld* const World = GetWorld();
	UGameViewportClient* const ViewportClient =
		World != nullptr ? World->GetGameViewport() : nullptr;
	if (ViewportClient == nullptr)
	{
		// ORDINARY AND SILENT. A headless automation world and the class default object both
		// arrive here, and neither is a fault; a log line would fire on every suite run.
		return;
	}

	RestoreProjectInputState(*ViewportClient);
	LastInputClaim = EStratInputClaim::Claimed;
}

void AStratPlayerController::RestoreProjectInputState(UGameViewportClient& ViewportClient)
{
	// THE DEFECT, AND IT IS UNCONDITIONAL. Everything below is about how a click FEELS;
	// this line is about whether one arrives at all.
	ViewportClient.SetIgnoreInput(false);

	// `UGameViewportClient::Init` forces both modes off when the process cannot render, so a
	// restore that ignored that test would leave a headless run in a state the engine itself
	// never puts it in.
	if (!FApp::CanEverRender())
	{
		return;
	}

	const UInputSettings* const Settings = GetDefault<UInputSettings>();
	if (Settings == nullptr)
	{
		return;
	}

	// THE SAME TWO FIELDS `Init` READS, FROM THE SAME OBJECT. If a later pass changes
	// `Config/DefaultInput.ini`, this follows it with no edit here -- which is the whole
	// reason they are read rather than named.
	ViewportClient.SetMouseCaptureMode(Settings->DefaultViewportMouseCaptureMode);
	ViewportClient.SetMouseLockMode(Settings->DefaultViewportMouseLockMode);
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

	// EACH BINDING IS GUARDED SEPARATELY, so that authoring SOME of the assets binds those
	// actions rather than none. `BindAction` with a null action is a check() in the engine, so
	// the guard is required and not defensive.
	//
	// `ETriggerEvent::Started` ON EVERY SELECTION BINDING BELOW. Each of these is a discrete
	// decision -- a click, a cancel, a wait, an end-turn -- and `Triggered` would repeat them
	// for the length of a held key. A repeated Move is refused by the rules module the second
	// time (the unit has moved), so the visible symptom would be a log full of refusals rather
	// than a wrong board; it is still the wrong event. [AMENDED 2026-08-27: this sentence used
	// to continue "NOT A STATEMENT ABOUT EVERY BINDING IN THIS FUNCTION: the hover binding at
	// the bottom is `Triggered` on purpose and says why." There is no longer a hover binding
	// in this function -- see the block at the end of it -- so `Started` is now true of every
	// binding here without exception. The carve-out is struck rather than deleted because a
	// reader who remembers it should find out where it went.]
	// AND EACH NULL BRANCH NOW SAYS SO BY NAME. THIS PART IS PERMANENT. It outlived the
	// temporary phase 6 diagnostics on purpose and is not to be stripped with them. Until
	// phase 6 THE GUARDS THAT EXISTED THEN were silent on the null side, so a log in which no
	// input ever arrived was consistent with every action bound AND with none bound, and the
	// difference between those two is the difference between an asset gap and an engine one.
	// Phase 6 spent a standalone PIE session, a `showdebug enhancedinput` overlay and a
	// `GetAll EnhancedPlayerInput` probe establishing by other means what these guards state
	// directly. "The defaults are set" is now an observation rather than an assertion.
	//
	// [AMENDED, wave 0: this block said "three of the four assets", "`ETriggerEvent::Started`
	// ON ALL FOUR", "these four guards", "consistent with four bindings" and "what these four
	// lines state directly" -- and, on the same line as the first of them, "three actions
	// rather than none", now "those actions rather than none" -- six counts of the same
	// growing set inside one function, every
	// one already stale when `OpenProductionMenuAction` landed, and now contradicted outright
	// by wave 0's
	// own hover binding at the bottom of this same function. The phase 6 sentences are kept as
	// HISTORY and scoped to the guards of that day rather than deleted: what phase 6 measured
	// is unchanged, and only the implication that the set is still that size is removed.]
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

	// THE PRODUCTION-MENU BINDING, AND IT IS `Started` FOR A REASON THE SELECTION BINDINGS
	// ABOVE DO NOT HAVE. [AMENDED, wave 0: "THE FIFTH BINDING ... THE OTHER FOUR".] This
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

	// THERE IS NO HOVER BINDING HERE, AND ITS ABSENCE IS THE POINT. Wave 0 bound a
	// `HoverAction` on `ETriggerEvent::Triggered` and it was MEASURED NEVER TO FIRE -- see
	// `Tick` below for the measurement and the route that replaced it. Do not re-add one
	// without reading that block: an axis binding here would be a second author of the hover,
	// racing the tick poll for the same `FStratHoverState`.
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
// The input handlers. One line each. [AMENDED, wave 0: "The input handlers. Four one-line
// calls." -- a count of a growing set. Wave 0's first draft changed this line SILENTLY while
// its own record entry claimed every such site had been quoted and stamped, so that entry was
// false against this very line. Stamped on the gate's finding, which is the shape the
// amendment pass exists to catch and did not catch in itself.]
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
	// THROUGH `RequestEndTurn` AND NOT DIRECTLY TO `HandleSelectionEvent`, so the key and
	// §2.11.2's button spell the event and its meaningless hex ONCE between them. The three
	// handlers above still call `HandleSelectionEvent` directly because no reflected verb
	// exists for their events; if one ever does, this is the shape it takes.
	FString Reason;
	if (!RequestEndTurn(Reason))
	{
		UE_LOG(LogStratPlay, Log, TEXT("End turn: %s"), *Reason);
	}
}

// §2.11.2's End Turn control. THE SAME PATH THE KEY TAKES, named so a widget can take it too.
//
// IT IS `OnEndTurn` WITH THE REFUSAL RETURNED INSTEAD OF LOGGED, and the difference is the
// whole of it. A key press has nobody to tell, so `OnEndTurn` logs at `Log` and returns void; a
// button press has a caller, and swallowing "move the marked Infantry first" into the output
// log would leave the player pressing a control that does nothing and says nothing.
bool AStratPlayerController::RequestEndTurn(FString& OutFailureReason)
{
	// NO GATE, NO CHECK AND NO LOG OF ITS OWN. Everything this function could usefully decide
	// is already decided inside `HandleSelectionEvent`, in an order that block calls the
	// contract; anything added here would be a copy of one of those decisions that can drift
	// from it.
	return HandleSelectionEvent(
		EStratSelectionEvent::EndTurn, FIntPoint::ZeroValue, OutFailureReason);
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
// §2.11.3 / §2.11.2's hover: WHY IT IS POLLED ON TICK AND NOT DRIVEN BY AN ACTION.
//
// THIS REPLACES A MEASURED-DEAD ROUTE, AND THE MEASUREMENT IS THE WHOLE JUSTIFICATION.
// Wave 0 wired the hover as a sixth Enhanced Input action -- `IA_Hover`, an `Axis2D` over
// `Mouse2D`, bound here on `ETriggerEvent::Triggered` -- and its asset and mapping-context
// row were authored and byte-verified. It never fired. Instrumented on 2026-08-27 with a
// per-instance counter on the handler and a console-forcible control on the resolve path,
// a human playtest measured, over three and a half minutes on a seeded 99-hex board:
//
//     OnHover ran EXACTLY ONCE, seven seconds after the match seeded, on a focus
//     transition with the cursor off the board. Between two forced console reads of the
//     counter -- `ke StratPlayerController UpdateHoverFromCursor`, which prints the
//     handler's count out of the running game -- the human swept the cursor across the
//     board for fifteen seconds and THE COUNT DID NOT MOVE.
//
// The control in that pair is what makes the absence a measurement rather than a silence:
// the forced call printed, which proved the log channel worked and that the editor had
// loaded the rebuilt binary, in the same line that read the handler's zero. An independent
// earlier observation agrees -- a planted hover cleared at the moment the human moved the
// cursor OUT of the window, which is another focus transition and not a mouse move.
//
// WHAT THAT PROVES AND WHAT IT DOES NOT. It proves mouse movement produces no `Triggered`
// event for a mouse-axis action on this controller, as configured. IT DOES NOT PROVE WHY.
// The standing hypothesis is that a visible, uncaptured cursor feeds no axis to
// `UPlayerInput` -- this class sets `bShowMouseCursor = true` and calls `SetInputMode`
// nowhere -- but that mechanism was never isolated, and this file does not assert it.
// Nothing below depends on the hypothesis being right; it depends only on the measurement,
// which is about the OBSERVED behaviour of the route and not about its cause.
//
// WHY POLLING AND NOT AN INPUT MODE. The obvious repair is `SetInputMode` with capture, and
// it was rejected on a property this game cannot trade: `bShowMouseCursor = true` is
// load-bearing -- a mouse-driven hex strategy whose player cannot see the cursor is not the
// game -- and every input mode that reliably feeds mouse axis does so by capturing, which
// changes how the cursor behaves at the viewport edge and how a click lands. A route that
// fixes the hover and makes selection feel different is worse than the bug it fixes. It is
// also unfalsifiable at our cost: no automation clause in this project reaches
// `UPlayerInput`, so an input-mode fix could only ever be re-confirmed by another human
// playtest, whereas the line below is reachable by a clause. If a later pass isolates the
// capture mechanism and finds a mode that feeds axis with a free visible cursor, this
// becomes a live alternative again -- and it would still have to beat the testability.
//
// THE COST IS ONE TRACE PER FRAME AND IT IS BOUNDED BY THINGS THAT ALREADY EXIST.
// `HexUnderCursor` returns before tracing when there is no board, so a controller in a
// world with no seeded match pays a null check. When there is a board it is one
// `GetHitResultUnderCursor` -- the same single trace the old `Triggered` binding ran per
// mouse-move frame, and mouse-move frames are most frames a hand is moving. The EXPENSIVE
// half, the model rebuild, is not paid per frame: `FStratHoverState`'s setters return
// whether anything actually moved and `ApplyHoverChange` refreshes only then. That
// de-duplication is not new and is not taken on trust here -- it is the shape those setters
// were written in (`StratHoverState.cpp`, both setters return `bChanged`) and the playtest
// exercised it 30 samples out of 30 while tracking the cursor across two hexes.
//
// IT POLLS, AND THE CONSTRUCTOR'S TICK COMMENT HAS BEEN CORRECTED FOR IT. That block said
// "It polls nothing: every path below runs from an input trigger", which was true when it
// was written and is now false in exactly one place -- this one. The reason the controller
// must tick is UNCHANGED and is still the phase-6 measurement about Enhanced Input trigger
// evaluation; this is a second, independent consumer of the same tick.
// ---------------------------------------------------------------------------

void AStratPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// THE RETURN VALUE IS DISCARDED HERE AND NOWHERE ELSE. A hover that did not move is not
	// news to anybody -- there is no caller waiting on it and nothing to log. It is returned
	// for `UpdateHoverFromCursor`'s other callers, which are a clause and a hand-over, and
	// both want the answer.
	UpdateHoverFromCursor();
}

// ---------------------------------------------------------------------------
// §2.11.3 / §2.11.2's hover, wave 0. Resolve, record, refresh if it moved.
//
// EVERY FUNCTION BELOW ENDS AT `ApplyHoverChange`, which is the one place a hover decides to
// refresh. However many entry points there come to be, there is ONE refresh decision, rather
// than a copy of it per entry point -- the shape `RefreshFromMachine`'s own block gives as the
// reason it is a single named method: a second, subtly different sequence is what a future
// caller writes when there is no first one to call. [AMENDED, wave 0, on the second W0 gate:
// "Three entry points and one refresh decision, rather than three copies of the same two
// lines." A census of a growing set, in the same class as the gate's own R2 finding and found
// by the same sweep after the blind spot that hid R2 was closed. The invariant -- one refresh
// decision, not one per caller -- is what the sentence was for and needs no count at all.]
// ---------------------------------------------------------------------------

bool AStratPlayerController::UpdateHoverFromCursor()
{
	// THE SAME CURSOR-TO-HEX ROUTE `OnSelect` USES, AND NOT A SECOND ONE. `HexUnderCursor` is
	// an instance-index lookup into the board's own list; there is no inverse of
	// `WorldLocationOfHex` in this project and this path does not invent one.
	FIntPoint Hex;
	if (!HexUnderCursor(Hex))
	{
		// OFF THE BOARD IS A HOVER ANSWER AND NOT A NON-ANSWER, which is the one place this
		// path differs in posture from `OnSelect`. A click on the sky is deliberately NOT
		// forwarded as a Cancel, because it is not a statement about the selection; a MOUSE
		// MOVE onto the sky is a complete statement about the cursor, and dropping it would
		// leave a forecast card standing over a hex the cursor has left.
		return ApplyHoverChange(Hover.ClearHoveredHex());
	}

	return ApplyHoverChange(Hover.SetHoveredHex(Hex));
}

bool AStratPlayerController::SetHoveredHex(FIntPoint Hex)
{
	return ApplyHoverChange(Hover.SetHoveredHex(Hex));
}

bool AStratPlayerController::ClearHoveredHex()
{
	return ApplyHoverChange(Hover.ClearHoveredHex());
}

bool AStratPlayerController::GetHoveredHex(FIntPoint& OutHex) const
{
	return Hover.GetHoveredHex(OutHex);
}

bool AStratPlayerController::ApplyHoverChange(bool bChanged)
{
	if (!bChanged)
	{
		return false;
	}

	// THE REFRESH IS HOW THE HOVER REACHES THE SCREEN, and it is the whole of the mechanism.
	// `DecorateForPresentation` writes `FStratViewModel::Hover` on the way through, so there
	// is no hover-specific push, no event and no widget notification -- the screen is rebuilt
	// from the model exactly as it is after a move or an end-turn.
	FString RefreshReason;
	if (!RefreshFromMachine(RefreshReason))
	{
		// AT LOG AND NOT WARNING, on `OnSelect`'s rule. The commonest cause by far is a cursor
		// moving over the board before `StartMatch` has seeded anything, which is the
		// interface working; and this fires at mouse-move rate, so a Warning here would bury
		// the log for a condition that is not a fault.
		UE_LOG(LogStratPlay, Log, TEXT("Hover: %s"), *RefreshReason);
	}

	// TRUE EVEN WHEN THE REFRESH REFUSED. The caller asked whether the HOVER moved; it did.
	// See `UpdateHoverFromCursor`'s declaration on why conflating the two would make one
	// return value mean two things.
	return true;
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

	// THE SUBSYSTEM IS NO LONGER LOOKED UP HERE. Both arms below reach it through the verb
	// that needs it -- `CloseProductionMenu` for the rows, `OpenProductionMenuAtHex` for the
	// §2.8 gate -- so this function now holds no pointer it does not use on its own path.

	// --- Already open: this press closes it. -------------------------------
	//
	// FORWARDED TO `CloseProductionMenu` RATHER THAN SPELLED OUT, AS OF 2026-09-02. The two
	// acts and their order -- panel down, then rows cleared -- used to be written here and
	// nowhere else, which is what made an EXIT button wiring them itself one drag of a wire
	// away from a live panel bound to an empty row array. That verb now owns the pair; this
	// arm is one call to it and the key's behaviour is unchanged.
	if (Hud->IsProductionMenuWidgetOpen())
	{
		return CloseProductionMenu(OutFailureReason);
	}

	// --- Not open: this press opens one on the hex under the cursor. -------
	//
	// THE CURSOR IS RESOLVED HERE AND THE SEQUENCE IS BELOW. `HexUnderCursor` is THE key's
	// hex source and cannot be a button's -- a click puts the cursor on the button -- so hex
	// RESOLUTION stayed here and the OPEN SEQUENCE moved into `OpenProductionMenuAtHex`.
	//
	// WHAT MOVED IS STATED BEFORE WHAT DID NOT, AND THAT ORDER IS DELIBERATE. A summary that
	// claims no change and then retracts itself further down the same block leaves BOTH claims
	// live for a reader who stops at the first sentence, and a nested correction is not read
	// as a correction. So this block is flat: no sentence below is retracted by any sentence
	// after it.
	//
	// TWO THINGS MOVED ON 2026-09-02. (1) THE OPEN SEQUENCE -- the Sec 2.8 gate, the latch,
	// the HUD open and the unwind -- is no longer written in this function. It is in
	// `OpenProductionMenuAtHex`, so the button and the key run ONE sequence rather than two
	// subtly different ones. (2) THE Sec 2.8 CONCLUDED-MATCH GATE THEREFORE RUNS AFTER THE
	// CURSOR READ INSTEAD OF BEFORE IT, and the refusal sentence reported on one input
	// changed with it. That second one is argued in full at the refusal it changed -- the
	// `[ORDER CHANGED 2026-09-02]` note on the `HexUnderCursor` refusal below, where a reader hits it
	// -- and is deliberately not re-argued here, because a copy of an argument is a copy that
	// can go stale.
	//
	// WHAT DID NOT MOVE: the toggle, the cursor as this arm's hex source, the `B` key
	// binding, and the WORDING of every refusal taken individually. No refusal sentence was
	// rewritten; one of them is now reached on an input that used to reach a different one.
	//
	// THE CLOSE ARM ABOVE IS DELIBERATELY NOT SHARED. See `OpenProductionMenuAtHex`'s
	// declaration: the shared inner is an OPEN and never a toggle, so a button cannot inherit
	// a second exit behaviour the menu's own EXIT control does not have. The close arm above
	// forwards to `CloseProductionMenu` for the ordering reason that verb exists to own.

	FIntPoint Hex;
	if (!HexUnderCursor(Hex))
	{
		// The same ordinary answer `OnSelect` gets. Not a fault, and nothing is latched --
		// see `GetProductionTargetHex` on why a hex cannot signal its own absence.
		//
		// AHEAD OF THE CONCLUDED-MATCH GATE, WHICH IS WHERE IT ALREADY WAS RELATIVE TO THE
		// CURSOR READ AND IS NOT WHERE IT IS RELATIVE TO THE GATE. [ORDER CHANGED
		// 2026-09-02: the §2.8 gate used to run BEFORE this read and now runs after it,
		// inside `OpenProductionMenuAtHex`. On a concluded match with the cursor off the
		// board the reported reason therefore changed from the concluded-match sentence to
		// `the cursor is not on the board`. Both are true; the second is the more local
		// answer, and no caller branches on either string -- they are sentences for a human.
		// Recorded because a clause asserting the first would go red on a correct change.]
		OutFailureReason = TEXT("the cursor is not on the board");
		return false;
	}

	return OpenProductionMenuAtHex(Hex, OutFailureReason);
}

bool AStratPlayerController::OpenProductionMenuAtFocusedFactory(FString& OutFailureReason)
{
	OutFailureReason.Reset();

	// THE LATCH IS THE ONLY HEX SOURCE, AND NO CURSOR IS READ. See the declaration: this is
	// the verb the retraction on `ToggleProductionMenu` points at, and reading `HexUnderCursor`
	// here would reintroduce the exact defect -- a button click has the cursor on the button.
	FIntPoint Hex;
	if (!BuildAffordance.GetFocusedHex(Hex))
	{
		// A SENTENCE AND NOT A CRASH, and it should be unreachable from the button: the BUILD
		// control is only on screen when `FStratViewModel::CommandBar::bShowBuildButton` is
		// true, and that field is written from this same latch. It is reachable from a console
		// command, from a gate, and from a graph that binds visibility to something else --
		// which is precisely why the answer is a reason rather than an assumption.
		OutFailureReason = TEXT("no factory is focused: click one you own first");
		return false;
	}

	return OpenProductionMenuAtHex(Hex, OutFailureReason);
}

bool AStratPlayerController::CloseProductionMenu(FString& OutFailureReason)
{
	OutFailureReason.Reset();

	AStratScoreboardHUD* const Hud = Cast<AStratScoreboardHUD>(GetHUD());
	if (Hud == nullptr)
	{
		OutFailureReason = TEXT("this controller has no AStratScoreboardHUD hosting a production menu");
		return false;
	}

	// TWO ACTS ON TWO OBJECTS, IN THIS ORDER, AND THE ORDER IS THE CONTRACT -- the sentence
	// this verb exists to hold. The panel comes down first, then the rows are cleared. The
	// reverse would leave a live panel bound to an empty `ProductionMenu` array for however
	// long the two lines are apart, which is a screen showing a menu the subsystem says is
	// not open. An EXIT button wiring the two underlying calls itself would have been one
	// drag of a wire away from that; see the declaration.
	Hud->CloseProductionMenuWidget();

	// A NULL SUBSYSTEM IS NOT A FAILURE HERE. The panel is down either way, which is what the
	// player asked for; there is simply nothing left holding rows.
	if (UStratMatchSubsystem* const Match = GetMatch())
	{
		Match->CloseProductionMenu();
	}

	// THE LATCH IS CLEARED WITH THE PANEL AND NOT WHEN THE NEXT ONE OPENS. Leaving it set
	// would have `GetProductionTargetHex` answer true with no menu on screen, and the
	// accessor's whole first channel is "is there one".
	bHasProductionTargetHex = false;
	ProductionTargetHex     = FIntPoint::ZeroValue;

	// THE FOCUS IS DELIBERATELY NOT CLEARED. `BuildAffordance` is untouched: exit returns the
	// player to the factory they were focused on with the BUILD control still on screen.
	// Clearing here would make exit behave as a cancel, which is a different gesture the
	// player already has on the secondary click and on Escape.

	// IDEMPOTENT, AND "NO MENU WAS OPEN" IS SUCCESS. `CloseProductionMenuWidget` is written
	// to be safe with none up -- `AStratScoreboardHUD::EndPlay` calls it unguarded for that
	// reason -- and `UStratMatchSubsystem::CloseProductionMenu` likewise. An exit button
	// pressed twice is not an error.
	return true;
}

bool AStratPlayerController::OpenProductionMenuAtHex(FIntPoint Hex, FString& OutFailureReason)
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

	// §2.8: A FINISHED MATCH DOES NOT OPEN A MENU. Only the OPEN path is gated -- closing runs
	// whatever the match's state, because taking a panel down is never a command and a player
	// left unable to dismiss a menu on a finished match would be worse than the defect this
	// gate closes. That is why the gate lives HERE, in the shared open, and not in
	// `CloseProductionMenu`.
	//
	// THROUGH THE SUBSYSTEM RATHER THAN A MODEL, unlike `HandleSelectionEvent`'s gate: this
	// function builds no model of its own, and building one here purely to ask would be the
	// second model that gate exists to avoid.
	UStratMatchSubsystem* const Match = GetMatch();
	if (Match != nullptr && Match->IsMatchConcluded())
	{
		OutFailureReason = StratMatchConcludedRefusalText();
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
		// hex. The refusal reason is the HUD's own, forwarded unchanged. THIS COVERS THE
		// ALREADY-OPEN CASE, which is the one a BUILD button reaches by being pressed twice:
		// the HUD refuses, this unwinds, and the menu that IS up keeps its own latch --
		// because the unwind restores nothing, it clears, and the open menu's hex was the
		// same hex in the only way a second press can happen from a focus that has not moved.
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

	// ---- §2.11.2's skip, ahead of every other gate in this function ------
	// "Any click or Esc skips to the end state." Both arrive here: `SelectAction` becomes
	// `HexPrimary` and `CancelAction` -- secondary click OR Escape -- becomes `Cancel`, so
	// gating the FUNCTION rather than two named events is what makes the GDD's "any" true
	// without enumerating an input set that has already grown twice.
	//
	// BEFORE THE BRIDGE LOOKUP AND BEFORE `StratMatchAcceptsPlayerCommands`, AND THAT ORDERING
	// IS THE POINT RATHER THAN TIDINESS. The AI's own command is how §2.8's primary win most
	// often lands -- measured 2026-08-23, the AI's turn-7 ninth command killed side 0's flag --
	// so the single most interesting hand-over to watch is the one after which the match is
	// OVER. The concluded-match gate below returns `false` for every event once that happens.
	// Placed after it, this skip would be unreachable in exactly the case it matters most and
	// the player would be stuck watching a tour they could not stop.
	//
	// IT NEEDS NO BRIDGE AND NO VIEW MODEL, which is what makes that placement legal rather
	// than merely desirable. `UStratMatchSubsystem::SkipAiPlayback` reads a cursor over a list
	// of things that already happened; it cannot be asked at a moment when its answer is not
	// yet knowable.
	//
	// `true` AND NOT `false`, UNLIKE THE Q27 GATES BELOW. Those refuse an input that was inert
	// and owe the player a sentence saying why nothing happened. This is not a refusal:
	// something DID happen -- the tour stopped -- and there is nothing to explain. Returning
	// `false` would put a failure string in front of a click that did what the player asked.
	//
	// AND IT CONSUMES THE INPUT, WHICH IS A DECISION AND NOT A CONSEQUENCE. The click that
	// stops the tour does not also select whatever is under the cursor, and the Escape that
	// stops it does not also drop the selection. §2.11.2 gives that input one job here.
	// `UStratMatchSubsystem::SkipAiPlayback` returns false when nothing was playing, so at any
	// quiet moment this block is invisible and every event falls through untouched.
	//
	// [CORRECTED 2026-08-29. THE SENTENCE THAT STOOD HERE ADDED "-- which is every moment in
	// the shipped default configuration, where `AiPlaybackStepSeconds` is zero", AND THAT WAS
	// FALSE ON THE TREE IT WAS WRITTEN ON. It is struck and not merely amended, because the
	// two claims cannot both stand: at the shipped default this block was invisible at NO
	// moment after an AI hand-over. The reel was filled on every hand-over while only the
	// timer was gated, so the cursor sat at 0 over a non-empty list, `SkipAiPlayback`
	// succeeded, and this block consumed the first click or Esc after every AI turn. The
	// comment asserted the exact property the code did not have, which is the worst way for a
	// prose block to be wrong -- it reads as the reason not to go and check.
	//
	// WHAT MAKES THE REMAINING SENTENCE TRUE IS A CHANGE IN `UStratMatchSubsystem` AND NOT A
	// REWORDING HERE. `BeginAiPlayback` now retires the reel on every path where it declines
	// to arm a timer, so the cursor is at the end unless a tour is genuinely under way, and
	// `SkipAiPlayback` reports false in the default configuration because there is nothing to
	// skip. Nothing in THIS file changed with that fix; this call site was always correct
	// about what it does and was wrong only about when it would fire.]
	if (Match->SkipAiPlayback())
	{
		return true;
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

	// ---- §2.11.5's focused-factory latch -----------------------------------
	// AFTER THE §2.8 GATE AND AFTER THE Q27 GATES, AND BEFORE `HandleEvent`. After, because
	// an event those gates refuse never happened as far as the player is concerned, and a
	// focus moved by a refused click would be an affordance appearing in response to an input
	// that was told it did nothing. Before, only because the model is already built ten lines
	// up and this is where the event is still in hand; nothing here depends on the machine.
	//
	// NO REFRESH CALL. The refresh at the bottom of this function runs on every path that
	// reaches here, so a second one would repaint the same frame twice.
	//
	// IT IS BOTH, ALWAYS, AND THAT IS A RULING RATHER THAN A CONVENIENCE. A primary click on
	// your own factory latches the focus AND still goes to `SelectionMachine.HandleEvent`
	// below -- so a click that is simultaneously a move order onto that factory's hex and a
	// focus gesture does both, and neither is swallowed. Nothing here consumes the event.
	//
	// THE CONTROLLER FORWARDS A HEX AND A MODEL AND DECIDES NOTHING, which is what keeps this
	// file's opening claim ("IT DECIDES NOTHING") true rather than nearly true. Whether that
	// hex is a factory, whether this seat holds it and whether the match is over are all
	// answered inside `FStratBuildAffordance::NoteHexPrimary` against the model built above.
	if (Event == EStratSelectionEvent::HexPrimary)
	{
		BuildAffordance.NoteHexPrimary(Hex, Model);
	}
	else if (Event == EStratSelectionEvent::Cancel)
	{
		// §2.11.1's cancel -- secondary click or Escape -- drops the selection, and the focus
		// goes with it. A cancel that left a BUILD control on screen would leave the player
		// with one affordance still lit after an input whose whole meaning is "never mind".
		BuildAffordance.ClearFocus();
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
		// ---- The route the player is about to be shown travelling ------------
		// ASKED BEFORE THE SUBMIT, AND THAT IS THE ONLY CORRECT POSITION RATHER THAN A
		// PREFERENCE. `FStratBridge::MovePathToHex` answers about the board AS IT IS: after the
		// submit the unit is standing on `Outcome.Hex`, so the same question answers `{goal}` at
		// cost 0 -- a one-point route, structurally valid, silently wrong, and indistinguishable
		// from a real answer by anything downstream. There is no later moment at which the
		// question can be asked and no flag that would make one.
		//
		// HELD IN A LOCAL AND HANDED OVER ONLY AFTER THE BRIDGE ACCEPTS. A refused move must
		// leave no route behind: `UStratMatchSubsystem::ApplyView` empties the map on every
		// apply, but the refusal path below refreshes too, and a route noted for a command that
		// never happened would animate a unit along a path it was told it could not take.
		//
		// IT REUSES THE §2.11.1 SEAM AND ADDS NO BRIDGE METHOD, NO QUERY TYPE AND NO HEX MATH.
		// `FStratBridgePathQuery` already exists for the path preview -- `RefreshFromMachine`
		// constructs one every refresh -- and it is the same forward to the same bridge method,
		// which is what makes the ANIMATED route and the PREVIEWED route the same route by
		// construction. The alternative, walking hex neighbours here, is `Move.h`'s §2.5
		// tie-break restated in the input path and is the substitution `T-UI-02` exists to
		// catch; the only neighbour math in this project is vendored `strat::` and unreachable
		// from this module anyway.
		//
		// MOVE ONLY. An Attack does not relocate the attacker and an EndTurn has no unit, so
		// neither has a route to walk. The switch is on `Outcome.Command` and never on a hex
		// being non-default -- `FStratSelectionOutcome::Hex`'s own block forbids sniffing that
		// field, because (0,0) is a real hex on this board.
		//
		// A REFUSAL IS SILENT AND DEGRADES TO A SNAP. The two channels are the query's own: a
		// `false` means the question could not be asked, an empty array means the module
		// answered "no route". `NotePendingMoveRoute` treats both as "note nothing", and the
		// actor snaps on an empty route -- which is the same thing the player saw before this
		// feature existed. Not logged at Warning for `StratDecoratePathPreview`'s reason: the
		// commonest cause is a bridge state the player cannot see and did nothing to cause.
		TArray<FIntPoint> MoveRouteHexes;
		if (Outcome.Command == EStratSelectionCommand::Move)
		{
			const FStratBridgePathQuery PathQuery(Bridge);

			TArray<int32> RouteCosts;
			int32         RouteTotalCost = 0;
			FString       RouteFailureReason;

			if (!PathQuery.PathTo(Outcome.UnitId, Outcome.Hex, MoveRouteHexes, RouteCosts,
			                      RouteTotalCost, RouteFailureReason))
			{
				MoveRouteHexes.Reset();

				UE_LOG(LogStratPlay, Verbose,
					TEXT("%s could not read the route for unit %d's move (the move still applies; it will snap): %s"),
					*GetName(), Outcome.UnitId, *RouteFailureReason);
			}
		}

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

		// ---- The route, handed over now that the move is a fact ---------------
		// AFTER THE ACCEPTANCE AND BEFORE `RunAiTurnsIfDue`, AND BOTH HALVES ARE LOAD-BEARING.
		// After, because the route describes a move that happened, and the refusal path above
		// returned without reaching this line -- so a refused move leaves nothing behind, which
		// is what asking BEFORE the submit and handing over AFTER it buys.
		//
		// BEFORE, because `RunAiTurnsIfDue` is what reaches `RefreshPresentation` and therefore
		// `UStratMatchSubsystem::ApplyView`, which is the one consumer and empties the map on
		// its way out. Handed over after that call, the route would miss its own apply and would
		// then be emptied by the NEXT one having animated nothing.
		//
		// ONE CALL AND ONE UNIT. Nothing here notes a route for anything the AI does.
		//
		// [CORRECTED 2026-09-02, SAME DAY, BECAUSE THE USER REVERSED THE DECISION THIS BLOCK
		// CITED. WRITTEN FLAT, AND THE SPLIT MATTERS: THE SENTENCE ABOVE IS STILL TRUE AND ONLY
		// THE CLAUSE AFTER ITS COMMA IS RETRACTED.] It said:
		// RETRACTED> "... and that absence is the entire implementation of the user's decision
		// RETRACTED>  that AI moves must not slide -- see
		// RETRACTED>  `UStratMatchSubsystem::NotePendingMoveRoute`, which states why no
		// RETRACTED>  detection exists or may be added."
		// **AI MOVES NOW SLIDE**, during §2.11.2's playback tour, by the user's reversal. The
		// ABSENCE here is intact and is unchanged in every particular -- this file notes one
		// route for one unit on one accepted player command, and it still contains no detection
		// of an AI turn. What it is no longer the implementation OF is a decision that no longer
		// stands. The AI's slide is `UStratMatchSubsystem::PlayMoveSlideForStep`, reading the
		// reel this file never touches; the "no detection exists or may be added" rule survives
		// it, because that function is reached from the TOUR and asks nothing about whose turn
		// it was.
		if (Match != nullptr)
		{
			Match->NotePendingMoveRoute(Outcome.UnitId, MoveRouteHexes);
		}

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

	// The reach and target overlays, from the rules module through the machine.
	// `BuildOverlays` computes neither set; with nothing selected both are empty and both
	// clear. NAMED RATHER THAN COUNTED as of 2026-08-23 -- this said "The two overlays", and
	// the board now carries a third that this block has nothing to do with: §2.11.6-B's ring
	// was already repainted by the `Match->ApplyView(Model)` call above.
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

	// THE HOVER STILL HAS NO ORDERING CONSTRAINT OF ITS OWN. It writes
	// `FStratViewModel::Hover` and no other field, reads nothing off the model, and consults
	// neither the machine nor the guidance layer. What HAS changed is that something now
	// depends on it, so it is no longer last -- see the forecast below.
	Hover.DecorateViewModel(Model);

	// §2.11.3'S CARD, AND IT MUST RUN AFTER THE HOVER. `StratDecorateForecast` reads
	// `Model.Hover` -- that is the whole of the ordering constraint and it runs one way:
	// the forecast reads the hover and writes nothing the hover reads. Placed before it,
	// it would compose this frame's selection against last frame's hex and the card would
	// be silently one mouse-move stale, which is the failure mode that looks like a
	// latency bug and is a sequencing one.
	//
	// IT ALSO READS THE MACHINE'S SELECTION, so it sits after
	// `SelectionMachine.DecorateViewModel` as well -- though only incidentally: it takes
	// the selection through `GetSelectedUnitId` rather than off the model, so that arm has
	// no ordering requirement. The hover one is real.
	//
	// A REFUSAL HERE IS ORDINARY AND IS NOT LOGGED AT WARNING. The commonest cause by far
	// is an unseeded bridge, which is the state every frame before `StartMatch` finishes,
	// and this function runs on every reconcile. The card is cleared unconditionally inside
	// the decorator, so a refusal leaves no stale forecast to report.
	{
		const UStratMatchSubsystem* const Match  = GetMatch();
		const FStratBridge* const         Bridge = (Match != nullptr) ? Match->GetBridge() : nullptr;

		const FStratBridgeForecastQuery ForecastQuery(Bridge);

		FString ForecastFailureReason;
		if (!StratDecorateForecast(Model, SelectionMachine.GetSelectedUnitId(),
		                           ForecastQuery, ForecastFailureReason)
			&& !ForecastFailureReason.IsEmpty())
		{
			UE_LOG(LogStratPlay, Verbose,
				TEXT("%s could not forecast for the hovered hex (this is ordinary before the match starts): %s"),
				*GetName(), *ForecastFailureReason);
		}
	}

	// §2.11.1'S PATH PREVIEW, AND IT MUST RUN AFTER THE HOVER FOR THE FORECAST'S REASON,
	// UNCHANGED. `StratDecoratePathPreview` reads `Model.Hover` and writes nothing the hover
	// reads; placed before it, the route is composed against last frame's hex and the
	// preview is silently one mouse-move stale.
	//
	// IT IS UNORDERED AGAINST THE FORECAST ABOVE. The two touch no common field -- one
	// writes `Forecast`, the other `PathPreview`, and neither reads the other's -- and they
	// ask the bridge different questions (`AttackForecast` and `MovePathToHex`). It sits
	// after only because the hover constraint puts both here and reading them in §2.11
	// order is easier.
	//
	// A SECOND `FStratBridge` LOOKUP RATHER THAN A WIDENED SCOPE ABOVE, deliberately: the
	// forecast block scopes its bridge pointer, and hoisting it to serve two decorators
	// would make the next one's lifetime assumption implicit. Both reads are of the same
	// subsystem inside the same call and cannot disagree.
	//
	// A REFUSAL HERE IS ORDINARY AND IS NOT LOGGED AT WARNING, on the forecast's rule: the
	// commonest cause by far is an unseeded bridge, which is the state every frame before
	// `StartMatch` finishes. The preview is cleared unconditionally inside the decorator, so
	// a refusal leaves no stale route to report. AN UNREACHABLE GOAL DOES NOT REACH THIS
	// BRANCH AT ALL -- it is an answer, not a refusal, and carries no reason to log.
	{
		const UStratMatchSubsystem* const Match  = GetMatch();
		const FStratBridge* const         Bridge = (Match != nullptr) ? Match->GetBridge() : nullptr;

		const FStratBridgePathQuery PathQuery(Bridge);

		FString PathFailureReason;
		if (!StratDecoratePathPreview(Model, SelectionMachine.GetSelectedUnitId(),
		                              PathQuery, PathFailureReason)
			&& !PathFailureReason.IsEmpty())
		{
			UE_LOG(LogStratPlay, Verbose,
				TEXT("%s could not preview a path to the hovered hex (this is ordinary before the match starts): %s"),
				*GetName(), *PathFailureReason);
		}
	}

	// §2.11.2'S COMMAND BAR, AND IT MUST RUN AFTER `GuidedOpening.DecorateViewModel`.
	// `StratDecorateCommandBar` reads `Model.Guidance.bEndTurnGated` and `::EndTurnGateHover`
	// and writes neither -- the constraint runs one way, exactly as the hover's does for the
	// forecast. Run before the guidance layer, the END TURN control would be drawn against
	// last frame's gate.
	//
	// TWO CALLS AND NOT ONE, ON `FStratGuidedOpening`'S SPLIT. `Observe` MUTATES -- it is the
	// turn clock, the seat clock and the concluded-match clear -- and `DecorateViewModel` is
	// `const`. Collapsing them would put a clock advance inside a decorator, which is the
	// thing this file's ordering block spends its length keeping separable.
	//
	// IT IS UNORDERED AGAINST THE HOVER, THE FORECAST, THE PATH PREVIEW AND THE INFO PANEL.
	// It reads `Factories`, `ViewingSide`, `Match`, `Units` and `Guidance` and writes
	// `CommandBar`, which none of them reads. It sits here rather than last only because the
	// info panel below is the decorator that restates the most and belongs last.
	//
	// NO FAILURE CHANNEL AND NOTHING TO LOG, on the info panel's rule: this decorator reaches
	// no bridge, so there is no unseeded state for it to refuse over. Nothing focused, a
	// focus on a hex the model does not carry and a focus on an enemy factory are all answers
	// it writes unconditionally.
	BuildAffordance.Observe(Model);
	BuildAffordance.DecorateViewModel(Model);

	// §2.11.2'S INFO PANEL, AND IT IS LAST BECAUSE IT READS THE MOST. It selects the
	// hovered hex out of `Model.Hexes` (so it must follow `Hover.DecorateViewModel`) and
	// that hex's unit's `bDone` bit (so it must follow
	// `SelectionMachine.DecorateViewModel`). Both constraints run one way: it writes
	// `Model.InfoPanel` and nothing else, and no decorator above reads that field.
	//
	// IT IS ORDERED AFTER THE FORECAST BY CHOICE RATHER THAN BY NEED -- the two touch no
	// common field -- and last is where a decorator that only restates the model belongs,
	// so that whatever it restates is final.
	//
	// NO FAILURE CHANNEL AND NOTHING TO LOG. Unlike the forecast, this decorator has no
	// bridge to be unseeded and no query to refuse; not hovering, hovering off the board
	// and hovering an empty hex are all answers it writes unconditionally. See its
	// declaration.
	StratDecorateInfoPanel(Model);
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
