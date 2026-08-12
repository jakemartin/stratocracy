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

	// This controller polls nothing. Every path below runs from an input trigger.
	PrimaryActorTick.bCanEverTick = false;
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
	if (SelectAction != nullptr)
	{
		EnhancedInput->BindAction(SelectAction, ETriggerEvent::Started, this,
			&AStratPlayerController::OnSelect);
	}
	if (CancelAction != nullptr)
	{
		EnhancedInput->BindAction(CancelAction, ETriggerEvent::Started, this,
			&AStratPlayerController::OnCancel);
	}
	if (WaitAction != nullptr)
	{
		EnhancedInput->BindAction(WaitAction, ETriggerEvent::Started, this,
			&AStratPlayerController::OnWait);
	}
	if (EndTurnAction != nullptr)
	{
		EnhancedInput->BindAction(EndTurnAction, ETriggerEvent::Started, this,
			&AStratPlayerController::OnEndTurn);
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

	// The one production query adapter. Constructed per event and holding nothing: it is a
	// borrowed pointer and a pair of forwards.
	const FStratBridgeRulesQuery Query(Bridge);

	const FStratSelectionOutcome Outcome = SelectionMachine.HandleEvent(Event, Hex, Model, Query);

	// SUBMIT FIRST, NOTIFY SECOND, AND ONLY ON ACCEPTANCE. `NotifyCommandApplied` is
	// documented "call it only on acceptance"; calling it before the bridge answered would
	// advance the machine past a command the rules module refused. `StratSubmitSelectionCommand`
	// is also where `STRAT-CMD accepted` is emitted, and it emits it for the same reason --
	// the word `accepted` is a claim about the rules module and not about the click.
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

	// THE SEAM. `bDone` and `bLockedThisTurn` reach the screen HERE, on the model, on its
	// way to `ApplyView` -- and nowhere else. `StratViewModel.h` records the obligation and
	// `UStratMatchSubsystem.h` split `BuildViewModel` from `ApplyView` to leave room for
	// exactly this line: "the phase-4 path is build -> decorate -> `ApplyView`, with the
	// decorated model being the one and only description of the screen."
	SelectionMachine.DecorateViewModel(Model);

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
