#include "StratShellGameMode.h"

#include "Engine/GameInstance.h"
#include "GameFramework/Pawn.h"

#include "StratMatchSubsystem.h"
#include "StratPlay.h"
#include "StratShellSubsystem.h"

AStratShellGameMode::AStratShellGameMode()
{
	// A BARE `APawn`, RESOLVED BY THE LINKER AND NOT BY A PACKAGE LOOKUP. See the header:
	// `ADefaultPawn` would bring a flying-spectator movement component and the engine's own
	// bindings onto a menu screen, and `AStratCameraPawn` would bring the match's input
	// surface. Neither belongs in front of a title card.
	DefaultPawnClass = APawn::StaticClass();

	// THE SHIPPED SLOT NAME, DERIVED FROM ITS ONE AUTHOR AND NOT COPIED FROM IT. See the
	// header on `SaveSlotName`: a literal here would make `FStratMatchConfig`'s designer
	// property advisory, which that struct's own resolver refuses in terms. This line is
	// what makes the title map and the match map agree about the slot without a Blueprint
	// author remembering to.
	//
	// IT IS WHAT THE `StratMatchSubsystem.h` INCLUDE ABOVE IS FOR, in a class that
	// deliberately holds no match configuration -- an include is a smaller price than a
	// second copy of a string, and it buys agreement by construction rather than by
	// convention.
	SaveSlotName = FStratMatchConfig().SaveSlotName;
}

void AStratShellGameMode::BeginPlay()
{
	Super::BeginPlay();

	// CLEARED FIRST, on `AStratGameMode::BeginPlay`'s stated reason: a stale sentence must
	// never explain a new success.
	LastFailureReason.Reset();

	UGameInstance* const Instance = GetGameInstance();
	if (Instance == nullptr)
	{
		LastFailureReason = TEXT("this GameMode has no game instance");
		UE_LOG(LogStratPlay, Error, TEXT("No shell this session: %s"), *LastFailureReason);
		return;
	}

	UStratShellSubsystem* const Shell = Instance->GetSubsystem<UStratShellSubsystem>();
	if (Shell == nullptr)
	{
		LastFailureReason = TEXT("no UStratShellSubsystem on this game instance");
		UE_LOG(LogStratPlay, Error, TEXT("No shell this session: %s"), *LastFailureReason);
		return;
	}

	// NOT GUARDED ON `MatchLevel` BEING SET, DELIBERATELY. An unconfigured destination is a
	// state the model already describes -- `New Match` and `Continue` come back greyed with
	// "No match level is configured." -- and refusing to configure at all would replace that
	// readable menu with an empty one and a log line nobody is looking at.
	Shell->ConfigureMatchDestination(MatchLevel, SaveSlotName);

	if (MatchLevel.IsNull())
	{
		LastFailureReason = TEXT("MatchLevel is unset on this GameMode's defaults");
		UE_LOG(LogStratPlay, Warning, TEXT("Shell is incompletely configured: %s"),
			*LastFailureReason);
	}
}
