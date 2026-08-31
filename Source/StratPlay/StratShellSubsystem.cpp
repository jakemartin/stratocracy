#include "StratShellSubsystem.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "StratMatchSubsystem.h"
#include "StratPlay.h"

// THE PLAYER-FACING COPY IS C++ AND NEVER A STRING TYPED INTO A `.uasset`, following
// `StratGuidedOpening::DirectiveTextFor`, which is the precedent this project set for
// authored copy. A label chosen in a WBP would be a second author of the same sentence and
// would make the model below advisory rather than authoritative.
namespace
{
	FText NewMatchLabel(const bool bMatchIsLive)
	{
		// ONE ROUTE, TWO WORDS FOR IT. See `EStratShellRoute::NewMatch`: restarting IS
		// starting, from a world that already has a match in it.
		return bMatchIsLive ? FText::FromString(TEXT("Restart Match"))
		                    : FText::FromString(TEXT("New Match"));
	}
} // namespace

bool UStratShellSubsystem::RouteTravels(const EStratShellRoute Route)
{
	return Route != EStratShellRoute::QuitGame;
}

bool UStratShellSubsystem::RouteLoadsSaveSlot(const EStratShellRoute Route)
{
	return Route == EStratShellRoute::ContinueMatch;
}

FString UStratShellSubsystem::PendingSlotForRoute(const EStratShellRoute Route,
                                                 const FString&         ConfiguredSlot)
{
	// EXPRESSED THROUGH `RouteLoadsSaveSlot` AND NOT AS A SECOND COMPARISON AGAINST
	// `ContinueMatch`. Two sites naming the same enumerator would be two answers to "which
	// route restores", agreeing until one of them was edited -- and this function exists at
	// all because a pairing written in exactly one unreachable place went unmeasured. One
	// authority, asked twice.
	return RouteLoadsSaveSlot(Route) ? ConfiguredSlot : FString();
}

bool UStratShellSubsystem::IsRoutePermitted(const EStratShellRoute  Route,
                                            const FStratShellFacts& Facts,
                                            FText&                  OutRefusalReason)
{
	OutRefusalReason = FText::GetEmpty();

	switch (Route)
	{
	case EStratShellRoute::NewMatch:
		// PERMITTED WHETHER OR NOT A MATCH IS LIVE, and that is the restart case rather than
		// an oversight. The only thing that can stop it is having nowhere to go.
		if (!Facts.bMatchLevelConfigured)
		{
			OutRefusalReason = FText::FromString(TEXT("No match level is configured."));
			return false;
		}
		return true;

	case EStratShellRoute::ContinueMatch:
		// THE DESTINATION IS CHECKED BEFORE THE SLOT, AND THE ORDER IS THE MESSAGE. An
		// unconfigured level is a project setup fault and sends a reader to a Blueprint
		// default; a missing save is the ordinary state of a first-time player and sends
		// them nowhere. Reporting the second when the first is also true would hide a
		// misconfiguration behind a sentence that reads as normal.
		if (!Facts.bMatchLevelConfigured)
		{
			OutRefusalReason = FText::FromString(TEXT("No match level is configured."));
			return false;
		}
		// THE PLAYER-FACING SENTENCE IS UNCHANGED BY THE RESTORABILITY FIX, DELIBERATELY. From
		// where the player stands, a slot that carries only a completion flag IS no saved
		// match -- they never saved one. Naming the internal distinction here would explain a
		// writer they have never heard of.
		if (!Facts.bSaveSlotIsRestorable)
		{
			OutRefusalReason = FText::FromString(TEXT("No saved match."));
			return false;
		}
		return true;

	case EStratShellRoute::ReturnToTitle:
		// LIVENESS IS CHECKED BEFORE THE DESTINATION HERE, WHICH IS THE OPPOSITE ORDER TO
		// `ContinueMatch` ABOVE AND IS DELIBERATE. On the title screen itself there is no
		// match and no title level configured, and "there is no match to leave" is the fact
		// that actually describes that player's situation. Naming the missing level there
		// would report a setup fault to someone who is already where the route would send
		// them.
		if (!Facts.bMatchIsLive)
		{
			OutRefusalReason = FText::FromString(TEXT("No match in progress."));
			return false;
		}
		if (!Facts.bTitleLevelConfigured)
		{
			OutRefusalReason = FText::FromString(TEXT("No title level is configured."));
			return false;
		}
		return true;

	case EStratShellRoute::QuitGame:
		// ALWAYS PERMITTED, AND IT IS THE ONE ROUTE WITH NO PRECONDITION. A menu whose exit
		// could be greyed out is the defect this whole file exists to fix.
		return true;
	}

	// UNREACHABLE FOR THE FOUR DECLARED ROUTES AND KEPT ANYWAY. A fifth enumerator added
	// without an arm above would otherwise fall out of the switch as permitted, which is the
	// one direction this function must never fail in.
	OutRefusalReason = FText::FromString(TEXT("Unknown route."));
	return false;
}

bool UStratShellSubsystem::ResolveDestination(const EStratShellRoute        Route,
                                              const TSoftObjectPtr<UWorld>& InMatchLevel,
                                              const TSoftObjectPtr<UWorld>& InTitleLevel,
                                              TSoftObjectPtr<UWorld>&       OutLevel,
                                              FText&                        OutRefusalReason)
{
	OutLevel = nullptr;
	OutRefusalReason = FText::GetEmpty();

	if (!RouteTravels(Route))
	{
		OutRefusalReason = FText::FromString(TEXT("This route does not open a level."));
		return false;
	}

	// `IsNull()` AND NOT `IsValid()`. `IsValid()` on a `TSoftObjectPtr` asks whether the
	// object is LOADED, which is false for every unloaded level and would refuse every
	// correctly configured destination in the tree. The question here is whether a designer
	// filled the property in, and that is `IsNull()`. No load happens in this function.
	const TSoftObjectPtr<UWorld>& Chosen =
		(Route == EStratShellRoute::ReturnToTitle) ? InTitleLevel : InMatchLevel;

	if (Chosen.IsNull())
	{
		OutRefusalReason = (Route == EStratShellRoute::ReturnToTitle)
			? FText::FromString(TEXT("No title level is configured."))
			: FText::FromString(TEXT("No match level is configured."));
		return false;
	}

	OutLevel = Chosen;
	return true;
}

FStratShellMenuModel UStratShellSubsystem::BuildMenuModel(const FStratShellFacts& Facts)
{
	// THE ORDER IS THE DECLARATION ORDER OF THE ENUM AND IS FIXED IN EVERY STATE. A model
	// that reordered or dropped rows with the facts would make an index-keyed clause
	// meaningless, and `FString`/`FText` comparison in this engine is case-insensitive --
	// measured in this project -- so a label-keyed clause is the weaker instrument that
	// would have to replace it.
	static const EStratShellRoute AllRoutes[] = {
		EStratShellRoute::NewMatch,
		EStratShellRoute::ContinueMatch,
		EStratShellRoute::ReturnToTitle,
		EStratShellRoute::QuitGame
	};

	FStratShellMenuModel Model;
	Model.Options.Reserve(UE_ARRAY_COUNT(AllRoutes));

	for (const EStratShellRoute Route : AllRoutes)
	{
		FStratShellOption Option;
		Option.Route = Route;

		switch (Route)
		{
		case EStratShellRoute::NewMatch:
			Option.Label = NewMatchLabel(Facts.bMatchIsLive);
			break;
		case EStratShellRoute::ContinueMatch:
			Option.Label = FText::FromString(TEXT("Continue"));
			break;
		case EStratShellRoute::ReturnToTitle:
			Option.Label = FText::FromString(TEXT("Return to Title"));
			break;
		case EStratShellRoute::QuitGame:
			Option.Label = FText::FromString(TEXT("Quit"));
			break;
		}

		// ONE CALL, NOT A COPY OF THE CONDITIONS. `IsRoutePermitted` is the only place a
		// route's preconditions are written; a greyed row and a refused `ExecuteRoute` say
		// the same sentence because they are the same sentence.
		FText Refusal;
		Option.bEnabled = IsRoutePermitted(Route, Facts, Refusal);
		Option.DisabledReason = Option.bEnabled ? FText::GetEmpty() : Refusal;

		Model.Options.Add(MoveTemp(Option));
	}

	return Model;
}

void UStratShellSubsystem::ConfigureMatchDestination(TSoftObjectPtr<UWorld> InMatchLevel,
                                                     const FString&         InSaveSlotName)
{
	MatchLevel   = MoveTemp(InMatchLevel);
	SaveSlotName = InSaveSlotName;

	// SET LAST AND NEVER CLEARED. See the header: this records THE CALL, so that an
	// unconfigured shell and a shell configured with a null level stay two different
	// states. `AStratShellHUD` waits on it before it asks the shell for a menu model.
	bMatchDestinationConfigured = true;
}

void UStratShellSubsystem::ConfigureTitleDestination(TSoftObjectPtr<UWorld> InTitleLevel)
{
	TitleLevel = MoveTemp(InTitleLevel);
}

FStratShellFacts UStratShellSubsystem::GatherFacts() const
{
	FStratShellFacts Facts;

	Facts.bMatchLevelConfigured = !MatchLevel.IsNull();
	Facts.bTitleLevelConfigured = !TitleLevel.IsNull();

	// RESTORABLE, NOT MERELY PRESENT, AND THE DIFFERENCE IS A DEFECT THIS FIELD ONCE SHIPPED.
	// `DoesSaveGameExist` answers "a file is there", and a file is there after any completed
	// match because `ApplyView` records §2.11.6's completion flag into the same slot with an
	// empty `SaveText`. Asking the weaker question offered Continue to a player who had never
	// saved and then gave them turn 1.
	//
	// THE QUESTION IS ASKED OF THE LOADER'S OWN AUTHORITY AND NOT RE-DERIVED HERE.
	// `DoesSlotHoldARestorableMatch` is built from `IsPayloadRestorable`, which is what
	// `LoadMatchFromSlot` itself refuses on -- so this menu cannot come to a different
	// conclusion from the function that will do the work. It also handles the empty-name case,
	// which is why no `IsEmpty()` guard stands in front of it: a second guard here would be a
	// second statement of that rule.
	FText Unrestorable;
	Facts.bSaveSlotIsRestorable =
		UStratMatchSubsystem::DoesSlotHoldARestorableMatch(SaveSlotName, Unrestorable);

	// ASKED OF THE MATCH SUBSYSTEM AND NEVER INFERRED FROM THE LEVEL NAME. A world with no
	// `UStratMatchSubsystem` -- every world that is not Game or PIE, and that is what
	// `DoesSupportWorldType` refuses -- has no match, which is the true answer and not a
	// failure to report.
	if (const UGameInstance* const Instance = GetGameInstance())
	{
		if (const UWorld* const World = Instance->GetWorld())
		{
			if (const UStratMatchSubsystem* const Match =
					World->GetSubsystem<UStratMatchSubsystem>())
			{
				Facts.bMatchIsLive = Match->IsMatchLive();
			}
		}
	}

	return Facts;
}

FStratShellMenuModel UStratShellSubsystem::GetMenuModel() const
{
	return BuildMenuModel(GatherFacts());
}

void UStratShellSubsystem::ArmPendingLoadSlot(const FString& InSlotName)
{
	PendingLoadSlot = InSlotName;
}

FString UStratShellSubsystem::ConsumePendingLoadSlot()
{
	FString Taken = PendingLoadSlot;
	PendingLoadSlot.Reset();
	return Taken;
}

bool UStratShellSubsystem::ExecuteRoute(const EStratShellRoute Route, FString& OutFailureReason)
{
	OutFailureReason.Reset();

	// THE PERMISSION IS RE-ASKED HERE AND NOT TRUSTED FROM THE WIDGET. A menu asset that
	// wired an enabled button to an impermissible route is a content defect; travelling
	// anyway would turn it into an unexplainable one two levels later.
	const FStratShellFacts Facts = GatherFacts();

	FText Refusal;
	if (!IsRoutePermitted(Route, Facts, Refusal))
	{
		OutFailureReason = Refusal.ToString();
		UE_LOG(LogStratPlay, Warning, TEXT("Shell route refused: %s"), *OutFailureReason);
		return false;
	}

	if (!RouteTravels(Route))
	{
		// The only non-travelling route. `nullptr` for the controller is the documented
		// "any local player will do" argument, and `bIgnorePlatformRestrictions` is false so
		// a platform that forbids self-exit is obeyed rather than overridden.
		UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
		return true;
	}

	TSoftObjectPtr<UWorld> Destination;
	if (!ResolveDestination(Route, MatchLevel, TitleLevel, Destination, Refusal))
	{
		// REACHABLE ONLY BY DISAGREEMENT BETWEEN THE TWO DECIDERS, and it is reported rather
		// than asserted away: `IsRoutePermitted` reads the FACTS and this reads the
		// POINTERS, and `GatherFacts` is what joins them. If they ever disagree the failure
		// text says which function refused.
		OutFailureReason = Refusal.ToString();
		UE_LOG(LogStratPlay, Warning, TEXT("Shell route has no destination: %s"),
			*OutFailureReason);
		return false;
	}

	// ARMED BEFORE THE TRAVEL, NECESSARILY. `OpenLevelBySoftObjectPtr` may not return before
	// the world is torn down, so anything this route must carry across has to be written
	// first -- and both the VALUE and the WRITE now go through public entry points a clause
	// exercises, which is the correction `strat-test-author` measured the need for.
	//
	// UNCONDITIONAL, AND THE VANISHED `if` IS THE POINT. `PendingSlotForRoute` returns an
	// empty string for every route that does not restore, so a branch here would have been a
	// third statement of "which route restores" -- in the one member no clause can read.
	ArmPendingLoadSlot(PendingSlotForRoute(Route, SaveSlotName));

	UE_LOG(LogStratPlay, Log, TEXT("Shell route %d opening %s."),
		static_cast<int32>(Route), *Destination.ToSoftObjectPath().ToString());

	UGameplayStatics::OpenLevelBySoftObjectPtr(this, Destination);
	return true;
}
