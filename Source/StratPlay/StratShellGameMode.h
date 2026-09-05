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
// THE TITLE MAP NOW OWNS A LOOPING SOUND, WHICH IS THE FIRST ONE IN THE PROJECT, AND THE
// LIFETIME IT NEEDS IS WHY IT LANDED HERE. `UStratSoundBank`'s block refuses music in as many
// words -- "NO MUSIC, NO AMBIENCE, NO LOOPS. Seven one-shot cues is the milestone. A looping bed
// needs a component whose lifetime someone owns, which is a different design and not a slot on
// this asset." That sentence names the requirement and this class is the answer to it: a
// GameMode is destroyed exactly when its map is, which is exactly when the title music must
// stop. Nothing about that bullet is loosened -- the bank still carries no music slot, and
// `UStratSoundDirector` still plays only fire-and-forget cues.
//
// A HELD `UAudioComponent` AND NOT `UGameplayStatics::PlaySound2D`, AND THE REASON IS THE
// REQUIREMENT RATHER THAN A PREFERENCE. `PlaySound2D` returns nothing, so a title track started
// that way plays on over the match map after a `NewMatch` route travels, with no handle in
// existence that could stop it. The component is the handle.
//
// IT IS SPAWNED BY `SpawnSound2D` AND NOT `CreateDefaultSubobject`, WHICH WAS THE OTHER SHAPE
// AND WAS REJECTED FOR A CONCRETE REASON. `AGameModeBase` derives `AInfo`, which HAS NO
// `RootComponent`; a `UAudioComponent` is a `USceneComponent` and would have had to become the
// root of an actor that has no position and draws nothing, purely so a 2D sound could be
// attached to something. `SpawnSound2D(..., bAutoDestroy=false)` returns the same class of
// object with none of that, and it is the same 2D, unattenuated playback `UStratSoundDirector`
// already argues for at length. WHAT IT COSTS, said rather than discovered: `bAutoDestroy=false`
// removes THE ENGINE'S OWN release -- the self-free that would otherwise happen when the sound
// finishes -- so the component's lifetime becomes something this class states rather than
// inherits, and `EndPlay` is where it states it: stop, destroy, null. THAT IS NOT THE SAME AS
// SAYING NOTHING ELSE COULD FREE IT, and the paragraph below is the measurement of what else
// does and of exactly how much it changes.
//
// AND THE REASON THIS PARAGRAPH USED TO GIVE FOR THAT WAS FALSE, RETRACTED IN PLACE RATHER THAN
// QUIETLY REWORDED. It read:
//   RETRACTED>  "the component is owned by the audio device rather than by this actor's
//   RETRACTED>   component list, so `EndPlay` must stop AND destroy it explicitly ... a
//   RETRACTED>   non-owned one that nobody destroyed would outlive the map"
// MEASURED 2026-09-05 by `Stratocracy.StratPlay.GATE-TITLEMENU.TheTitleTrackIsReleasedWhenThe
// ShellMapEnds`, which asserts the measurement rather than the claim:
//
//   DIAG outer=StratShellGameMode_1 owner=StratShellGameMode_1 registered=1 ownedCount=1
//
// `UGameplayStatics::SpawnSound2D` gives the component an OUTER and an OWNER of the spawning
// actor, registers it, and it is that actor's only owned component. So actor teardown WOULD
// reach it, and `EndPlay`'s `Stop()` and `DestroyComponent()` are BELT-AND-BRACES -- a stated
// intent and a guaranteed ordering -- not the only thing standing between a menu track and a
// match map. **WHAT IS UNCHANGED IS THE REQUIREMENT AND THE CODE.** Relying on teardown order
// for a guarantee this class is the one making is the thing `EndPlay` refuses to do, and one
// half of the old sentence is still load-bearing on its own: with `bAutoDestroy` at the engine
// default the component would free itself and null this member, which reads identically to a
// correct release and is not one. The day `SpawnSound2D` stops owning what it spawns, the old
// argument becomes true again and this block is where a reader will be told.
//
// NOT IN THIS ROUND, with reasons:
// - MATCH MUSIC. `AStratGameMode` gets no equivalent property this pass, and the omission is a
//   scope decision taken by the user rather than an oversight: the milestone is title/menu music.
//   The day it lands it is this property and this `EndPlay`, copied to that class, or -- better,
//   if a track must survive the travel -- a `bPersistAcrossLevelTransition` spawn owned by
//   something with a game-instance lifetime, which is a different design and not this one.
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
#include "Templates/SubclassOf.h"
#include "UObject/SoftObjectPtr.h"

#include "StratShellGameMode.generated.h"

class UAudioComponent;
class USoundBase;
class UStratOptionsWidget;
class UStratSoundBank;

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

	/**
	 * The AUDIO milestone's cue assets for the TITLE MAP. Null means the shell runs silent.
	 *
	 * IT EXISTS BECAUSE THE TITLE MAP HAS NO MATCH. `FStratMatchConfig::SoundBank` is the
	 * match map's answer and this class starts no match, so without this property
	 * `WBP_TitleMenu` would be the one screen in the game whose buttons could not click. The
	 * two are not one property because they are set on two different GameModes on two
	 * different maps.
	 *
	 * THE DRIFT IS THE SAME SHAPE AS `SaveSlotName`'S AND IS STRICTLY WORSE, WHICH IS WORTH
	 * SAYING PLAINLY. That property's two copies are reconciled in C++ -- this class's
	 * constructor derives it from `FStratMatchConfig()`'s default, so agreement is by
	 * construction and an override is the only way to break it. AN ASSET POINTER HAS NO C++
	 * AUTHOR: there is no default `UStratSoundBank` to derive from, no literal is permitted
	 * (CLAUDE.md #4), and nothing in this tree can make the title map and the match map point
	 * at the same bank. So the two CAN silently differ from the day they are authored, and
	 * unlike `SaveSlotName` the failure is not even a wrong restore -- it is two maps with
	 * different-sounding buttons, which nobody will file as a bug.
	 *
	 * DISCHARGED BY a clause reading both Blueprint CDOs and asserting the two pointers equal,
	 * exactly as `SaveSlotName`'s discharge is stated one property up. That clause is phase D's
	 * and is not this file's to write.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stratocracy|Shell")
	TObjectPtr<UStratSoundBank> SoundBank;

	/**
	 * The looping track the title map plays. Null means the title screen is silent.
	 *
	 * `USoundBase` AND NOT `USoundWave`, on `UStratSoundBank`'s stated reasoning about its own
	 * seven slots: a MetaSound, a Sound Cue and a wave all derive `USoundBase`, so the asset
	 * type stays content's decision and this module names no plugin class and grows no
	 * dependency.
	 *
	 * LOOPING IS THE ASSET'S PROPERTY AND NOT THIS CLASS'S, WHICH IS WHY THERE IS NO `bLoop`
	 * BESIDE IT. A `USoundWave` carries `bLooping` and a MetaSound loops in its own graph;
	 * a bool here would be a second author of that fact with no rule about which wins -- the
	 * same refusal `UStratSoundBank` makes about volume multipliers. A track that does not loop
	 * simply ends, which is a legitimate configuration and not a fault this class reports.
	 *
	 * ITS GAIN IS `UStratAudioSettings::MusicVolume` AND THAT IS NOT WIRED HERE. The join is the
	 * asset's Sound Class -- set to `UStratSoundBank::MusicSoundClass` in the editor -- and
	 * `UStratSoundDirector::ApplyVolumes` writes the override. NOTHING IN C++ CHECKS THAT
	 * PAIRING, and a track authored into the SFX class or into none at all will play at unity
	 * however far the music slider is pulled down, with no error anywhere. DISCHARGED BY a
	 * clause reading this asset's `SoundClassObject` and asserting it equals the bank's
	 * `MusicSoundClass`, once both assets exist; that clause is content's phase and not this
	 * file's to write.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stratocracy|Shell")
	TObjectPtr<USoundBase> TitleMusic;

	/**
	 * The handle `BeginPlay` spawned and `EndPlay` stops. Null when `TitleMusic` is unset, and
	 * null again after `EndPlay`.
	 *
	 * `BlueprintReadOnly` AND NOT WRITABLE, so no graph can swap the handle out from under
	 * `EndPlay` and strand a playing component. `Transient` because it is a live object and not
	 * a configured one; nothing about it should survive a save or a duplicate of this actor.
	 */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Shell")
	TObjectPtr<UAudioComponent> TitleMusicComponent;

	/**
	 * Sec 2.11.5's volume screen, for `UStratOptionsPresenter` to create when the shell's
	 * `IsOptionsPanelOpen` says so. Null means the title map shows no options panel.
	 *
	 * IT IS ON A GameMode AND NOT ON THE HUD, AND THE REASON IS NOT LOCAL TO THIS FILE.
	 * `StratOptionsPresenter.h` carries it in full: the owner has to be a `UWorldSubsystem`
	 * because `AStratScoreboardHUD` -- the MATCH map's HUD -- is in `StratUI` and cannot name
	 * `UStratShellSubsystem` at all, and a subsystem has no details panel to hold this.
	 *
	 * THE SAME ASSET IS NAMED BY `AStratGameMode::OptionsWidgetClass` AND NOTHING ASSERTS THE
	 * TWO AGREE. That is the identical shape `SoundBank` records above and `SaveSlotName`
	 * before it, and it is a real cost rather than a formality: a match map pointed at an
	 * older options WBP would be a second, differently-behaved volume screen reachable only
	 * mid-match, which nobody will file as a bug. DISCHARGED BY a clause reading both
	 * Blueprint CDOs and asserting the two class pointers equal, on
	 * `StratShellBlueprintSlotParity.cpp`'s existing precedent. That clause is the test lane's
	 * and is not this file's to write.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stratocracy|Options")
	TSubclassOf<UStratOptionsWidget> OptionsWidgetClass;

	/** Where the options panel sits in the viewport stack. Above the title menu by default,
	 *  since a panel drawn under the menu it was opened from is indistinguishable from a panel
	 *  that never appeared -- which is the failure this whole route exists to fix. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stratocracy|Options")
	int32 OptionsPanelZOrder = 100;

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

	/**
	 * Stops and destroys `TitleMusicComponent`.
	 *
	 * IT IS A STATED RELEASE AND NOT THE ONLY ONE, AND THAT IS SAID HERE RATHER THAN ONLY IN THE
	 * FILE HEADER BECAUSE A MAINTAINER WHO JUMPS STRAIGHT TO THIS DECLARATION WOULD OTHERWISE
	 * READ THE OPPOSITE. Measured 2026-09-05 by `Stratocracy.StratPlay.GATE-TITLEMENU.TheTitle
	 * TrackIsReleasedWhenTheShellMapEnds`: `UGameplayStatics::SpawnSound2D` gives the component
	 * an OUTER and an OWNER of this actor and registers it --
	 * `outer=StratShellGameMode_1 owner=StratShellGameMode_1 registered=1 ownedCount=1` -- so
	 * actor teardown WOULD reach it. This override is BELT-AND-BRACES: a stated intent and a
	 * guaranteed ordering, not the only thing standing between a menu track and the match map.
	 * The file header carries the full retraction of the claim that used to say otherwise.
	 *
	 * IT IS KEPT ANYWAY, AND THE REASON IS NOT TIDINESS. Deleting it would make this class's
	 * guarantee depend on `SpawnSound2D` CONTINUING to own what it spawns -- which is precisely
	 * the premise that was measured and found wrong once already, in the other direction. A
	 * guarantee this class is the one making is not one to borrow from another module's teardown
	 * order. And `bAutoDestroy=false` is the half of the old argument that survived the
	 * measurement intact: with the engine default the component would free itself and null this
	 * member, which reads identically to a correct release and is not one.
	 *
	 * UNCONDITIONAL AND NOT GATED ON THE END-PLAY REASON. Every reason this actor stops -- travel,
	 * PIE ending, the world tearing down -- is a reason the title music should stop, and a
	 * `switch` here would be a list of the ways to get it wrong.
	 */
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
};
