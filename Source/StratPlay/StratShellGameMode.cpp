#include "StratShellGameMode.h"

#include "Components/AudioComponent.h"
#include "Engine/GameInstance.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

#include "StratMatchSubsystem.h"
#include "StratPlay.h"
#include "StratShellSubsystem.h"
#include "StratSoundBank.h"
#include "StratSoundDirector.h"

#include "Engine/World.h"

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

	// ---- The AUDIO milestone's bank, FIRST AND OUTSIDE EVERY REFUSAL BELOW -
	// **BEFORE THE SHELL SUBSYSTEM LOOKUP AND BEFORE ANY OF THE THREE EARLY RETURNS**, which
	// is the whole of the placement. Two of those returns are reachable in a real session --
	// no game instance, no `UStratShellSubsystem` -- and a title screen that failed to
	// configure its shell STILL DRAWS ITS MENU, because `WBP_TitleMenu` is the HUD's and not
	// this class's. A silent menu on a misconfigured map would be a second symptom of the
	// first, and the log line those branches already write is the one that explains it.
	//
	// A NULL BANK IS ADOPTED AS READILY AS A REAL ONE, on `StartMatchInternal`'s reasoning: it
	// is how a title map with no audio says so, and a guarded call could not clear a stale
	// adoption. A NULL DIRECTOR IS NOT REPORTED -- see `UStratMatchSubsystem::FindSoundDirector`.
	if (UWorld* const World = GetWorld())
	{
		if (UStratSoundDirector* const Director = World->GetSubsystem<UStratSoundDirector>())
		{
			Director->AdoptSoundBank(SoundBank);
		}
	}

	// ---- The title track ---------------------------------------------------
	// AFTER THE BANK AND STILL AHEAD OF EVERY REFUSAL BELOW, for the same reason the bank is:
	// a title map whose shell failed to configure still draws its menu, and a silent one would
	// be a second symptom of the first. It is after the bank rather than before it because
	// `AdoptSoundBank` re-applies the saved volumes, so the music starts into a mix that has
	// already had `UStratAudioSettings::MusicVolume` written into it rather than a frame of
	// unity followed by a correction.
	//
	// `bAutoDestroy` FALSE IS WHAT MAKES THE HANDLE A HANDLE. With the engine default (true) the
	// component frees itself when the sound finishes and `TitleMusicComponent` becomes a dangling
	// `UPROPERTY` -- nulled by the GC, so not a crash, but also not something `EndPlay` can stop.
	// The cost is that this class now owns the object, which `EndPlay` discharges.
	//
	// `bPersistAcrossLevelTransition` IS LEFT FALSE, DELIBERATELY. Persisting is the one setting
	// that would make the music survive a `NewMatch` route -- which is precisely the behaviour
	// this whole shape exists to prevent. `EndPlay` would still stop it, so the two are not in
	// conflict; false means the engine agrees rather than being overruled at the last moment.
	if (TitleMusic != nullptr)
	{
		TitleMusicComponent = UGameplayStatics::SpawnSound2D(this, TitleMusic,
		                                                    /*VolumeMultiplier=*/1.0f,
		                                                    /*PitchMultiplier=*/1.0f,
		                                                    /*StartTime=*/0.0f,
		                                                    /*ConcurrencySettings=*/nullptr,
		                                                    /*bPersistAcrossLevelTransition=*/false,
		                                                    /*bAutoDestroy=*/false);
	}

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

void AStratShellGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// STOPPED AND THEN DESTROYED, IN THAT ORDER AND BOTH OF THEM. `DestroyComponent` alone would
	// be enough in practice, and relying on that would be relying on a teardown order for a
	// guarantee this class is the one making. `Stop()` is the statement of intent; the destroy
	// discharges the release `bAutoDestroy=false` took away from the engine in `BeginPlay`.
	//
	// AND MEASURED 2026-09-05, THIS WHOLE BLOCK IS BELT-AND-BRACES RATHER THAN THE ONLY NET --
	// `SpawnSound2D` outers AND owns the component to this actor (`ownedCount=1`), so actor
	// teardown would reach it. The class header carries the retraction and the diagnostic; the
	// code is unchanged, because "in practice" is not the guarantee this class is making.
	//
	// NULLED AFTERWARDS SO THE MEMBER CANNOT ANSWER A QUESTION ABOUT A DEAD OBJECT. Nothing in
	// this class reads it after here, but `TitleMusicComponent` is `BlueprintReadOnly` and a
	// graph on this actor can.
	if (TitleMusicComponent != nullptr)
	{
		TitleMusicComponent->Stop();
		TitleMusicComponent->DestroyComponent();
		TitleMusicComponent = nullptr;
	}

	// LAST, NOT FIRST. `AGameModeBase::EndPlay` is where the actor's own teardown runs, and the
	// component must be released while this object is still fully alive.
	Super::EndPlay(EndPlayReason);
}