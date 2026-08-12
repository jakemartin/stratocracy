// GDD §4.1 / §4.9 -- the GameMode, implementation side.
//
// TWENTY LINES OF BEHAVIOUR, AND THAT IS THE MEASURE OF THIS CLASS. It sets a pawn class,
// finds the subsystem, and hands it a struct. Everything else about the match belongs to
// something that is not an actor.
//
// NO `StratBridge.h` INCLUDE AND NONE NEEDED. This file names no `strat::` type and asks
// no rules question; `FStratMatchConfig` is reflected engine types throughout.

#include "StratGameMode.h"

#include "StratCameraPawn.h"
#include "StratPlay.h"
#include "StratPlayerController.h"

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
		}
		return;
	}

	UE_LOG(LogStratPlay, Log, TEXT("Match started by %s."), *GetName());
}
