// GDD §4.1 / §4.9 -- the GameMode, implementation side.
//
// TWENTY LINES OF BEHAVIOUR, AND THAT IS THE MEASURE OF THIS CLASS. It sets a pawn class,
// finds the subsystem, and hands it a struct. Everything else about the match belongs to
// something that is not an actor.
//
// **W6 ADDED A SECOND SUBSYSTEM AND NOT A SECOND KIND OF WORK**, said here because the line
// above is a claim about this file's size and W6 moved it. It now finds `UStratShellSubsystem`
// too, hands it `TitleLevel`, and consumes any pending `Continue` slot. All three are the
// same shape as the paragraph above describes: find something that is not an actor, give it a
// value this actor was configured with. No rules question is asked and no §2.8 outcome is
// decided in either half.
//
// NO `StratBridge.h` INCLUDE AND NONE NEEDED. This file names no `strat::` type and asks
// no rules question; `FStratMatchConfig` is reflected engine types throughout.

#include "StratGameMode.h"

#include "StratCameraPawn.h"
#include "StratPlay.h"
#include "StratPlayerController.h"
#include "StratShellSubsystem.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"

AStratGameMode::AStratGameMode()
{
	// A CLASS REFERENCE, NOT AN ASSET PATH -- resolved by the linker, so there is no
	// `/Game/` literal and a renamed asset cannot break a compile. See the header block on
	// why it is set at all: `ADefaultPawn` would otherwise arrive with a flying-spectator
	// movement component and the engine's default bindings already attached, which is a
	// second input path beside the one phase 4 is about to build.
	//
	// A Blueprint subclass of this GameMode may override it with a BP_ subclass of the
	// camera pawn; this is the floor, not the ceiling.
	DefaultPawnClass = AStratCameraPawn::StaticClass();

	// PHASE 4 SET THIS, AS THE HEADER SAID IT WOULD. The same shape as `DefaultPawnClass`
	// above -- a `UClass*` from `StaticClass()`, resolved by the linker, with no `/Game/`
	// literal. It is set here rather than left to a Blueprint because the alternative
	// default is `APlayerController`, which binds nothing: the result would be a seeded,
	// correctly drawn match that no click can touch, and that failure reads as "the input
	// assets are wrong" when in fact no Stratocracy input path was installed at all.
	//
	// `AStratPlayerController` IS SAFE AS A BARE C++ DEFAULT IN A WAY `AStratScoreboardHUD`
	// IS NOT, which is the whole difference from the line below. It runs inert with null
	// input assets and says so once; a bare scoreboard HUD would refuse every refresh in its
	// own words and read as a bridge bug. Phase 5's GameMode Blueprint overrides this with a
	// BP_ subclass carrying the mapping context and the four actions; this is the floor.
	PlayerControllerClass = AStratPlayerController::StaticClass();

	// `HUDClass` is deliberately left alone. The header block gives the reason: a bare C++
	// scoreboard HUD would refuse every refresh and look like a bridge bug.

	PrimaryActorTick.bCanEverTick = false;
}

void AStratGameMode::BeginPlay()
{
	Super::BeginPlay();

	// A fresh attempt owns a fresh verdict. Carrying a previous reason forward would let a
	// stale sentence explain a new success.
	LastFailureReason.Reset();

	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		LastFailureReason = TEXT("this GameMode has no world");
		UE_LOG(LogStratPlay, Error, TEXT("No match this session: %s"), *LastFailureReason);
		return;
	}

	UStratMatchSubsystem* const Match = World->GetSubsystem<UStratMatchSubsystem>();
	if (Match == nullptr)
	{
		// Reachable in exactly one way: this world's type is not Game or PIE, which
		// `UStratMatchSubsystem::DoesSupportWorldType` refuses. Said plainly rather than
		// null-guarded silently, because "the subsystem is missing" is otherwise an
		// unexplainable absence in a log.
		LastFailureReason = TEXT(
			"no UStratMatchSubsystem in this world -- it is created for Game and PIE worlds only");
		UE_LOG(LogStratPlay, Error, TEXT("No match this session: %s"), *LastFailureReason);
		return;
	}

	// THE ONE CALL. Everything about ordering -- seed before handover, reconcile before the
	// panel refreshes -- is inside `StartMatch`, deliberately, so that there is one place
	// the sequence is written and no caller can get it half right.
	//
	// A `false` RETURN DOES NOT MEAN THERE IS NO MATCH. `StartMatch` is all-or-nothing on
	// the rules side and not on the presentation side: an unassigned tile mesh refuses with
	// a live, correctly seeded match behind it. `IsMatchLive()` is the question that
	// separates them, which is why the log line below asks it rather than inferring from
	// the return value.
	FString FailureReason;
	if (!Match->StartMatch(MatchConfig, FailureReason))
	{
		LastFailureReason = FailureReason;

		if (Match->IsMatchLive())
		{
			UE_LOG(LogStratPlay, Warning,
				TEXT("Match is live but incompletely presented: %s"), *LastFailureReason);
		}
		else
		{
			UE_LOG(LogStratPlay, Error, TEXT("No match this session: %s"), *LastFailureReason);
			return;
		}
	}
	else
	{
		UE_LOG(LogStratPlay, Log, TEXT("Match started by %s."), *GetName());
	}

	// ---- §2.11.5's shell -----------------------------------------------------
	// AFTER `StartMatch` AND BEFORE THE AI, AND BOTH HALVES OF THAT POSITION ARE LOAD-BEARING.
	// After, because a `ContinueMatch` route restores INTO a seeded match and there is nothing
	// to restore into until `StartMatch` has run. Before the AI, because restoring a save
	// replaces whose turn it is: running the opening AI turn first would move a match the
	// player is about to have overwritten, and on the AI-first side it would move it twice.
	if (UStratShellSubsystem* const Shell = GetGameInstance()
			? GetGameInstance()->GetSubsystem<UStratShellSubsystem>()
			: nullptr)
	{
		// The title map's return destination. Unset is a configuration and not a fault --
		// `Return to Title` simply comes back greyed with its own reason.
		Shell->ConfigureTitleDestination(TitleLevel);

		// CONSUMED, NOT READ. `ConsumePendingLoadSlot` clears as it answers, so a player who
		// continues once and then restarts is not silently handed the old save on the second
		// travel. The empty case is every ordinary boot and is not logged.
		const FString PendingSlot = Shell->ConsumePendingLoadSlot();
		if (!PendingSlot.IsEmpty() && Match->IsMatchLive())
		{
			FString LoadReason;
			if (!Match->LoadMatchFromSlot(PendingSlot, LoadReason))
			{
				// NOT FOLDED INTO `LastFailureReason`, on this file's existing rule: that
				// property answers "why is there no match, or why is it incompletely drawn",
				// and a refused restore leaves a perfectly good freshly seeded match behind
				// it. The player gets turn 1 instead of their save, which is a worse outcome
				// than they asked for and a better one than an empty screen.
				UE_LOG(LogStratPlay, Warning,
					TEXT("Continue could not restore slot '%s': %s -- the freshly seeded match stands."),
					*PendingSlot, *LoadReason);
			}
			else
			{
				UE_LOG(LogStratPlay, Log, TEXT("Continue restored slot '%s'."), *PendingSlot);
			}
		}
	}

	// ---- §2.9's opponent, if it moves first ---------------------------------
	// REACHED ON BOTH SURVIVING PATHS ABOVE, AND THAT IS WHY THE EARLY `return` MOVED INSIDE
	// THE `else`. A missing tile mesh returns `false` from `StartMatch` with a live, correctly
	// seeded match behind it; if the AI holds the first side in that match, returning on the
	// warning path would leave the game sitting on the AI's turn forever behind a scoreboard
	// that looks fine. Only the no-match path returns without asking, because there is nothing
	// to ask about.
	//
	// IT DOES NOT CHECK WHOSE TURN IT IS. `RunAiTurnsIfDue` does, once, and a second copy of
	// that check here would be a second authority on it. In a hot-seat match with no
	// `AiSides` configured -- which is the default and is every match before phase C -- this
	// call builds one view model, finds a human's turn and returns true.
	if (Match->IsMatchLive())
	{
		FString AiReason;
		if (!Match->RunAiTurnsIfDue(AiReason))
		{
			// NOT FOLDED INTO `LastFailureReason`. That property answers "why is there no
			// match, or why is it incompletely drawn"; an AI turn that refused is a different
			// question about a match that exists, and `FStratAiTurnRunner` has already logged
			// the `STRAT-AI refused` line naming the phase that failed.
			UE_LOG(LogStratPlay, Warning, TEXT("The opening AI turn refused: %s"), *AiReason);
		}
	}
}
