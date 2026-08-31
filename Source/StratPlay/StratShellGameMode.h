// GDD §2.11.5 / GATE-TITLEMENU -- the title map's GameMode. It configures the shell
// and it starts nothing.
//
// WHAT GAP THIS CLOSES. `UStratShellSubsystem` is a `UGameInstanceSubsystem` and therefore
// has no details panel, so it has no way to learn which level "New Match" opens or which
// save slot "Continue" restores. This class is the object that has them: two
// `EditDefaultsOnly` properties, set on a Blueprint default by the editor lane, handed over
// on `BeginPlay`. That is exactly `AStratGameMode`'s relationship to
// `UStratMatchSubsystem::StartMatch`, and it is copied on purpose rather than invented.
//
// IT IS A SIBLING OF `AStratGameMode` AND NOT A BASE OR A DERIVATIVE, and the alternative is
// named because it is the one a reader will reach for. Making `AStratGameMode` derive from
// this class would hand the match map a `MatchLevel` property it must never use -- the match
// map's "new match" is a restart, whose destination is the map it is already on, which is a
// self-reference nobody should have to reason about. Making this derive from `AStratGameMode`
// would start a match on the title screen. They share a parent (`AGameModeBase`) and one
// call, and nothing else.
//
// THE TITLE MAP GETS NO MATCH, AND THAT IS WHY THIS CLASS EXISTS RATHER THAN A FLAG ON
// `AStratGameMode`. A `bStartMatchOnBeginPlay = false` default would put the whole match
// configuration -- DataTables, scenario file, AI sides, difficulty -- on a details panel
// where every one of those fields is meaningless, and a designer who filled one in would get
// no feedback at all. An object that cannot express the wrong thing is better than one that
// ignores it.
//
// `DefaultPawnClass` IS SET TO `APawn` AND NOT TO `AStratCameraPawn`, which is the one place
// this class deliberately differs from `AStratGameMode`. There is no board to look at, and
// the camera pawn's spring arm and its Enhanced Input surface would be a live input path
// behind a menu. A bare `APawn` is the smallest thing the engine will possess. It is a
// `UClass*` from `StaticClass()` and not an asset path, on `AStratGameMode`'s own reasoning.
//
// NOT IN THIS ROUND, with reasons:
// - `HUDClass` AND ANY WIDGET. The menu asset is the editor lane's, and this module must not
//   name a `/Game/` path to reach it. The Blueprint subclass of this class sets `HUDClass`,
//   or the menu WBP is created by the player controller's own Blueprint -- either way it is
//   asset data. Until then this map boots to an empty screen, which is a configuration and
//   not a fault, exactly as `AStratGameMode` says of its own unset `HUDClass`.
// - MOVING `GameDefaultMap`. The INI is the steward's file and pointing it at a title level
//   that does not exist yet would break every suite run in the project. The shell is
//   asset-less by design this pass.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UObject/SoftObjectPtr.h"

#include "StratShellGameMode.generated.h"

/**
 * Runs §2.11.5's title/menu map.
 *
 * A map gets one by pointing its World Settings at a Blueprint subclass -- asset data, never
 * an `#include`, for the same reason `AStratGameMode` states: `StratBridge -> Stratocracy`
 * already exists and the reverse arrow would be a cycle.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Strat Shell Game Mode"))
class STRATPLAY_API AStratShellGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AStratShellGameMode();

	/**
	 * The level a `NewMatch` or `ContinueMatch` route opens.
	 *
	 * A SOFT REFERENCE AND NOT A NAME, so a renamed or moved map is a redirector's problem
	 * rather than a silent failure to travel, and so nothing in this module ever spells a
	 * `/Game/` path. It is never loaded by the shell: `UStratShellSubsystem::
	 * ResolveDestination` asks only whether it is null.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stratocracy|Shell")
	TSoftObjectPtr<UWorld> MatchLevel;

	/**
	 * The save slot `Continue` restores.
	 *
	 * IT MUST MATCH `FStratMatchConfig::SaveSlotName` ON THE MATCH MAP's GameMode, AND AS OF
	 * THE W6 REVIEWER PASS IT DOES SO BY CONSTRUCTION RATHER THAN BY A BLUEPRINT AUTHOR
	 * REMEMBERING. The constructor initialises this from a default-constructed
	 * `FStratMatchConfig`, so the two agree unless somebody deliberately changes one.
	 *
	 * DERIVED, NOT COPIED, AND THAT IS THE WHOLE POINT OF DOING IT THIS WAY. Writing
	 * `TEXT("StratocracyMatch")` here would have given the shipped slot name two authors --
	 * which is precisely what `UStratMatchSubsystem::ResolveSaveSlotName` refuses in its own
	 * words: "A slot name that fell back to a literal in this file would be a second author
	 * of the same string, and the designer's property would then be advisory." Two literals
	 * that agree today are a shape this project has already paid for. There is still one
	 * author of that string and this file is not it.
	 *
	 * WHAT THE DEFAULT COSTS, SAID RATHER THAN LEFT TO BE FOUND. Empty no longer means
	 * "unconfigured" on this property: a designer who wants Continue permanently unavailable
	 * must now clear the field deliberately rather than leave it alone. This project already
	 * carries a defect of that species -- a real default that could not signal unset -- so
	 * the trade is named rather than assumed. It is taken because the alternative was worse
	 * in the shipping direction: a title screen whose Continue row is greyed for every player
	 * who ever saves, until an asset is edited, with "No saved match." as the only
	 * explanation. Nothing here WRITES a save; the property is read by `DoesSaveGameExist`
	 * and handed to `LoadMatchFromSlot`, so a wrong value costs a wrong restore and never a
	 * clobbered slot.
	 *
	 * WHAT IT DOES NOT FIX. A designer who overrides EITHER property on EITHER Blueprint
	 * re-opens the drift, and no C++ can see it: a mismatch and a genuinely unwritten slot
	 * both land on "No saved match.", the ordinary first-run state, so the failure stays
	 * quiet. The discharge is unchanged -- a clause reading both Blueprint CDOs and asserting
	 * the two strings equal, once the editor lane has created them.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stratocracy|Shell")
	FString SaveSlotName;

	/** Why the shell is unconfigured, when it is. Empty on success. */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Shell")
	FString LastFailureReason;

protected:
	/**
	 * Hands the two properties above to `UStratShellSubsystem`.
	 *
	 * ON BeginPlay, matching `AStratGameMode`, though for a weaker reason than that class
	 * has: nothing here depends on the player controller existing. It is BeginPlay anyway so
	 * that both GameModes configure their subsystem at the same point in the world's life
	 * and a reader does not have to hold two orderings in mind.
	 */
	virtual void BeginPlay() override;
};
