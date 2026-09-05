// GDD Sec 2.11.5 -- the volume screen's owner, implementation side.
//
// WHAT IS HERE THAT COULD NOT BE IN THE HEADER. `UStratSoundDirector`, `UStratAudioSettings` and
// `UStratShellSubsystem` -- three `StratPlay` types this class talks to and names nowhere in its
// own declaration -- plus `Blueprint/UserWidget.h` for `AddToViewport`. The header's own block
// records the one include it was forced to carry and why.
//
// THE WHOLE OF THE FILE IS FOUR MOVES: reconcile, show, hide, forward. Nothing here decides
// whether the panel SHOULD be up -- `UStratShellSubsystem::IsOptionsPanelOpen` decides that and
// this class only makes the screen agree with it. Nothing here decides what a gain MEANS --
// `StratClampVolume` and `UStratSoundDirector::CommitVolumes` do that, and this class forwards.
//
// EVERY LOOKUP IS DONE AT THE MOMENT IT IS NEEDED AND NOTHING IS CACHED BUT THE WIDGET. The
// shell, the director and the settings object are all fetched per call. That is deliberate on
// `UStratShellSubsystem::GetMenuModel`'s stated reasoning about caches: a cache with no
// invalidation is a worse defect than a repeated lookup, and none of these three lookups touches
// the disk -- `GetAudioSettings` loads once and then holds, which is the director's own business
// and not a cost this class re-pays.

#include "StratOptionsPresenter.h"

#include "Blueprint/UserWidget.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

#include "StratAudioSettings.h"
#include "StratPlay.h"
#include "StratShellSubsystem.h"
#include "StratSoundCues.h"
#include "StratSoundDirector.h"

bool UStratOptionsPresenter::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	// Game and PIE only, mirroring `UStratSoundDirector::DoesSupportWorldType`. An editor
	// preview world has no viewport to add a panel to and no player to own one.
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UStratOptionsPresenter::ConfigureOptionsPanel(
	TSubclassOf<UStratOptionsWidget> InOptionsWidgetClass, const int32 InPanelZOrder)
{
	OptionsWidgetClass = InOptionsWidgetClass;
	PanelZOrder        = InPanelZOrder;

	// RECONCILED IMMEDIATELY, BECAUSE CONFIGURATION IS ONE OF THE TWO THINGS THAT CAN MAKE THE
	// ANSWER CHANGE. The other is the flag, which has a delegate. A GameMode that configures
	// this class in a world where the panel is ALREADY open -- which is what a travel with the
	// flag set looks like -- would otherwise leave the screen empty until the next request.
	ReconcileOptionsPanel();
}

void UStratOptionsPresenter::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	if (UStratShellSubsystem* const Shell = FindShell())
	{
		// `AddUniqueDynamic` AND NOT `AddDynamic`. The shell outlives the map and this object
		// does not, so a world reloaded in the same process binds a fresh presenter to a
		// long-lived delegate; the unique form makes a leaked binding from a presenter that
		// failed to deinitialize a duplicate rather than a second live handler. `Deinitialize`
		// removes it either way -- this is the belt, not the braces.
		Shell->OnOptionsPanelStateChanged.AddUniqueDynamic(
			this, &UStratOptionsPresenter::HandleOptionsPanelStateChanged);
	}

	// SEE THE HEADER. The flag survives level travel, so the opening state is a real state and
	// not always "closed".
	ReconcileOptionsPanel();
}

void UStratOptionsPresenter::Deinitialize()
{
	// THE UNBIND GOES FIRST, for `AStratShellHUD::EndPlay`'s reason about its timer: a live
	// binding into an object being torn down is the one way this class could outlive its world.
	if (UStratShellSubsystem* const Shell = FindShell())
	{
		Shell->OnOptionsPanelStateChanged.RemoveDynamic(
			this, &UStratOptionsPresenter::HandleOptionsPanelStateChanged);

		// AND THE FLAG IS CLOSED, WHICH IS THE DEBT `bOptionsPanelOpen`'S DECLARATION NAMES IN
		// TERMS. That member is on a `UGameInstanceSubsystem` and survives the destruction of
		// this world; leaving it true would hand the next world a panel nobody asked for, and
		// its own comment says the fix belongs "in that owner's `EndPlay`" rather than in a
		// lifetime change there. This is that call.
		//
		// IT IS CALLED EVEN WHEN NO PANEL WAS EVER SHOWN, deliberately: the flag can be true
		// with no panel on screen -- an unconfigured world is exactly that state -- and closing
		// only what we drew would leave precisely the case that has no other owner.
		Shell->CloseOptionsPanel();
	}

	HidePanel();

	Super::Deinitialize();
}

void UStratOptionsPresenter::ReconcileOptionsPanel()
{
	const UStratShellSubsystem* const Shell = FindShell();

	// A MISSING SHELL READS AS CLOSED. See the declaration: with no game instance there is no
	// route that could have opened the panel, so this is an answer and not an error.
	const bool bShouldBeOpen = (Shell != nullptr) && Shell->IsOptionsPanelOpen();

	if (bShouldBeOpen == IsPanelLive())
	{
		// IDEMPOTENT, WHICH IS WHAT LETS THE SHELL'S DELEGATE FIRE UNFILTERED. A second
		// `RequestOptionsPanel` still increments the count that route's observability rests on,
		// and it must not cost the player a rebuilt screen with a reseeded thumb.
		return;
	}

	if (!bShouldBeOpen)
	{
		HidePanel();
		return;
	}

	FString Reason;
	if (!ShowPanel(Reason))
	{
		LastFailureReason = Reason;

		// AT `Log` AND NOT `Error` WHEN NO CLASS IS SET, on `AStratShellHUD::BeginPlay`'s rule:
		// unset is a configuration -- a map saying "no options panel here" -- and logging that
		// at Error trains a reader to ignore the channel a real failure arrives on. Everything
		// else here IS a real failure.
		if (OptionsWidgetClass == nullptr)
		{
			UE_LOG(LogStratPlay, Log, TEXT("No options panel on this map: %s"),
				*LastFailureReason);
		}
		else
		{
			UE_LOG(LogStratPlay, Error, TEXT("Options panel could not be shown: %s"),
				*LastFailureReason);
		}

		return;
	}

	LastFailureReason.Reset();
}

bool UStratOptionsPresenter::ShowPanel(FString& OutFailureReason)
{
	if (OptionsWidgetClass == nullptr)
	{
		OutFailureReason =
			TEXT("no OptionsWidgetClass was configured on this world's GameMode defaults");
		return false;
	}

	const UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		OutFailureReason = TEXT("this presenter has no world");
		return false;
	}

	// THE OWNING PLAYER IS THE WIDGET'S OUTER, matching `AStratShellHUD::CreateMenuWidget`
	// exactly: the panel belongs to this local player rather than to the world, which is what
	// gives its bindings a player context to read the game instance through.
	APlayerController* const OwningPlayer = UGameplayStatics::GetPlayerController(World, 0);
	if (OwningPlayer == nullptr)
	{
		OutFailureReason = TEXT("this world has no local player controller to parent a panel to");
		return false;
	}

	UStratOptionsWidget* const Created =
		CreateWidget<UStratOptionsWidget>(OwningPlayer, OptionsWidgetClass);
	if (Created == nullptr)
	{
		OutFailureReason = FString::Printf(
			TEXT("CreateWidget returned null for OptionsWidgetClass '%s'"),
			*GetNameSafe(OptionsWidgetClass));
		return false;
	}

	// ASSIGNED BEFORE THE SEED, because `SeedPanel` reads the member rather than a local. That is
	// not tidiness: it means the seed runs against exactly the object a later reconcile will
	// hide, so there is no window in which this class holds a panel it has not seeded.
	OptionsWidget = Created;

	Created->OnAudioOptionsCommitted.AddDynamic(
		this, &UStratOptionsPresenter::HandleAudioOptionsCommitted);
	Created->OnOptionsDismissed.AddDynamic(
		this, &UStratOptionsPresenter::HandleOptionsDismissed);

	// SEEDED BEFORE IT IS ON SCREEN, so the first frame a player sees already shows the gains
	// they chose last time rather than the designer's authored thumb positions followed by a
	// jump. `PushAudioOptions` and not the setters -- see `SeedPanel`.
	SeedPanel();

	Created->AddToViewport(PanelZOrder);

	++PanelsShownCount;

	return true;
}

void UStratOptionsPresenter::HidePanel()
{
	if (OptionsWidget == nullptr)
	{
		return;
	}

	OptionsWidget->OnAudioOptionsCommitted.RemoveDynamic(
		this, &UStratOptionsPresenter::HandleAudioOptionsCommitted);
	OptionsWidget->OnOptionsDismissed.RemoveDynamic(
		this, &UStratOptionsPresenter::HandleOptionsDismissed);

	// REMOVED RATHER THAN LEFT TO THE GARBAGE COLLECTOR, on `AStratShellHUD::EndPlay`'s measured
	// reasoning: a widget added to the viewport is referenced BY the viewport, so dropping the
	// pointer alone leaves a panel drawn over whatever comes next.
	OptionsWidget->RemoveFromParent();
	OptionsWidget = nullptr;
}

void UStratOptionsPresenter::SeedPanel()
{
	if (OptionsWidget == nullptr)
	{
		return;
	}

	const UWorld* const World = GetWorld();
	UStratSoundDirector* const Director =
		(World != nullptr) ? World->GetSubsystem<UStratSoundDirector>() : nullptr;

	// NO DIRECTOR MEANS NO SEED AND NOT A DEFAULT SEED. Pushing three unity gains here would
	// draw a screen claiming the player is at full volume when nothing had been asked; the
	// widget's own `Model` defaults are unity anyway, so the difference is only whether this
	// class ASSERTS it. It does not.
	if (Director == nullptr)
	{
		return;
	}

	const UStratAudioSettings* const Settings = Director->GetAudioSettings();
	if (Settings == nullptr)
	{
		return;
	}

	// BUILT THROUGH `StratBuildAudioOptionsModel` RATHER THAN FIELD-BY-FIELD, so the three
	// percent strings come from the one function entitled to produce them. `PushAudioOptions`
	// rebuilds from the floats again on the other side, which is that function's own invariant
	// and not a duplication this call has to work around.
	OptionsWidget->PushAudioOptions(StratBuildAudioOptionsModel(
		Settings->MasterVolume, Settings->SfxVolume, Settings->MusicVolume));
}

UStratShellSubsystem* UStratOptionsPresenter::FindShell() const
{
	const UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return nullptr;
	}

	UGameInstance* const Instance = World->GetGameInstance();
	if (Instance == nullptr)
	{
		return nullptr;
	}

	return Instance->GetSubsystem<UStratShellSubsystem>();
}

void UStratOptionsPresenter::HandleOptionsPanelStateChanged(const bool bIsOpen)
{
	// THE ARGUMENT IS IGNORED ON PURPOSE AND THAT IS THE FILE HEADER'S WHOLE POINT. Acting on
	// the payload would make this class's screen a function of the EVENT; reading the flag makes
	// it a function of the STATE, and only the second is impossible to leave stale.
	(void)bIsOpen;

	ReconcileOptionsPanel();
}

void UStratOptionsPresenter::HandleAudioOptionsCommitted(const FStratAudioOptionsModel& InModel)
{
	const UWorld* const World = GetWorld();
	UStratSoundDirector* const Director =
		(World != nullptr) ? World->GetSubsystem<UStratSoundDirector>() : nullptr;

	if (Director == nullptr)
	{
		// NOT COUNTED. `VolumeCommitsForwardedCount` answers "did this class hand a gain to the
		// director", and incrementing on a world that had none would make the counter answer a
		// different, weaker question -- "was this class told" -- which the widget's own
		// broadcast already answers.
		LastFailureReason =
			TEXT("a volume was committed but this world has no UStratSoundDirector");
		UE_LOG(LogStratPlay, Warning, TEXT("Options panel: %s"), *LastFailureReason);
		return;
	}

	// THE MODEL'S FLOATS AND NOT ITS STRINGS. The percent text is a LABEL and explicitly not an
	// inverse of the gain -- `StratBuildAudioOptionsModel`'s block measures that 0.996 reads
	// "100%" -- so anything that parsed the string back would be committing a different number
	// than the player chose.
	Director->CommitVolumes(InModel.MasterVolume, InModel.SfxVolume, InModel.MusicVolume);

	++VolumeCommitsForwardedCount;
}

void UStratOptionsPresenter::HandleOptionsDismissed()
{
	// THE CLICK CUE THE WIDGET COULD NOT EMIT. `UStratOptionsWidget::HandleBackClicked` records
	// why: `StratUI` cannot reach `UStratSoundDirector`. AT ENTRY AND BEFORE ANY CHECK, which is
	// `AStratPlayerController::RequestEndTurn`'s stated rule -- the cue acknowledges the INPUT
	// and never the outcome, so a control that refuses still sounds alive.
	if (const UWorld* const World = GetWorld())
	{
		if (UStratSoundDirector* const Director = World->GetSubsystem<UStratSoundDirector>())
		{
			Director->EmitCue(EStratSoundCue::ButtonClick, INDEX_NONE, INDEX_NONE, 0);
		}
	}

	// THROUGH THE SHELL AND NOT BY CALLING `HidePanel` DIRECTLY, WHICH IS THE LOAD-BEARING LINE
	// IN THIS FUNCTION. Hiding the panel here would leave `bOptionsPanelOpen` true with nothing
	// on screen -- the exact state `StratShellSubsystem.h` calls "one step worse than a greyed
	// row" -- and the next reconcile from any source would put the panel straight back. Closing
	// the flag makes the delegate come back round and take the panel down through the one route
	// that exists, so the shipped path and the path a clause drives are the same line.
	if (UStratShellSubsystem* const Shell = FindShell())
	{
		Shell->CloseOptionsPanel();
		return;
	}

	// NO SHELL AND A PANEL ON SCREEN IS A STATE NOTHING SHOULD BE ABLE TO REACH -- this class
	// only shows a panel when the shell said to. It is handled rather than asserted away,
	// because the alternative is a back button that does nothing on a world that lost its game
	// instance, and a dead exit is what the whole options route exists to avoid.
	LastFailureReason = TEXT("the options panel was dismissed with no UStratShellSubsystem to "
	                         "close; it was removed directly");
	UE_LOG(LogStratPlay, Warning, TEXT("Options panel: %s"), *LastFailureReason);
	HidePanel();
}
