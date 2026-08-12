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

	// `HUDClass` and `PlayerControllerClass` are deliberately left alone. The header block
	// gives both reasons: a bare C++ scoreboard HUD would refuse every refresh and look
	// like a bridge bug, and there is no Strat player controller until phase 4 builds one.

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
