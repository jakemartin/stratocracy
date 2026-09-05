// GDD Sec 2.11.5 ("No settings screen beyond volume + resolution is budgeted") -- the volume
// half of that sentence, as a slot payload. Three floats and nothing else.
//
// WHAT GAP THIS CLOSES. `UStratSoundDirector` plays every cue at `VolumeMultiplier 1.0f` and
// `UStratSoundBank` refuses to carry a multiplier at all -- its own block says "a sound designer
// changes the asset". That is right for a MIX decision and wrong for a PLAYER preference: a
// player who wants the music quieter cannot edit `MS_Strat_*`, and nothing in this project
// remembered a preference of any kind across a process. This class is the remembering, and
// `UStratSoundDirector::ApplyVolumes` is what makes it audible.
//
// A SECOND `USaveGame` AND NOT THREE FIELDS ON `UStratSaveGame`, AND THE THREE REASONS ARE
// SEPARATE ONES RATHER THAN ONE REASON SAID THREE WAYS.
//   1. VOLUME IS NOT MATCH STATE. `StratSaveGame.h` opens by saying what its payload IS -- the
//      Sec 4.10 text plus the four things the text cannot carry -- and every field it holds is
//      something a RESTORE needs. A gain is needed by nothing a restore does.
//   2. IT WOULD BECOME PER-SLOT. `UStratMatchSubsystem::SaveMatchToSlot` writes one named slot
//      and `AStratShellGameMode::SaveSlotName` names it, so a volume living there would be a
//      different volume per save. A player who loaded an older match would have the sliders
//      move under them, which is a defect with no plausible cause a player could guess at.
//   3. IT WOULD MOVE A VERSIONED SHAPE. `UStratSaveGame::kCurrentSavedDataVersion` is the
//      contract that any change to the MEANING of a field bumps the version and grows an arm in
//      `LoadMatchFromSlot`. Adding a field with a safe default does not bump it -- so the cost
//      here is not a forced version bump; it is that a purely presentational preference would
//      have joined a payload whose whole discipline exists to protect a deterministic replay,
//      and every later reader of that file would have to be told which fields are load-bearing
//      and which are decoration.
//   THE BRIEF THAT ORDERED THIS FILE GAVE A FOURTH REASON THAT DOES NOT HOLD, AND IT IS
// RECORDED RATHER THAN QUIETLY DROPPED: that a volume field "would move a field the save-parity
// clauses hash". It would not. `FStratBridge::StateHash` hashes `strat::canonicalStateBytes`,
// which is produced by the rules module from the board and knows nothing about this `USaveGame`
// at all; `UStratSaveGame` is engine-side bookkeeping AROUND that text and no clause hashes its
// members. The first three reasons are sufficient and are the ones this file stands on.
//
// WHAT IS DELIBERATELY NOT IN IT.
//   - NO RESOLUTION, NO WINDOW MODE, NO FRAME CAP. Sec 2.11.5 budgets "volume + resolution" and
//     this is half of that. Resolution is `UGameUserSettings`, which the engine already owns,
//     already persists in `GameUserSettings.ini`, and which a second copy here would fight. The
//     day the options screen grows a resolution row it binds to that object, not to this one.
//   - NO MUTE FLAG. A mute is `MasterVolume == 0` and a second representation of it would be
//     two answers to "is this silent" -- the `bool bPlayed` collapse `EStratSoundDisposition`
//     was written to avoid, one layer up. A screen that wants a mute toggle drives the float.
//   - NO PER-CUE VOLUME. Three channels, matching the three `USoundClass` assets the mix
//     carries. A per-cue gain would be a mix control an author cannot reach and would
//     reintroduce exactly the multiplier `UStratSoundBank` refuses.
//   - NO VERSION FIELD, AND THE ABSENCE IS ARGUED RATHER THAN COPIED FROM NOWHERE.
//     `UStratSaveGame` carries one because an unreadable match slot silently returns
//     default-constructed fields and a replay then refuses for the wrong reason. The failure
//     mode here is that three floats read as `1.0`, which is the shipped default, is audible
//     immediately, and is repaired by moving a slider. A version on a payload whose every
//     wrong value is self-announcing and self-repairing would be ceremony. IT BECOMES OWED the
//     moment a field here is added whose default is not also its safe value.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "StratAudioSettings.generated.h"

/**
 * The player's three gains, persisted.
 *
 * EVERY FIELD DEFAULTS TO UNITY, AND THAT IS THE PROPERTY THE WHOLE FEATURE RESTS ON. A player
 * with no slot, a slot that failed to deserialize, and a slot written before this class existed
 * all produce the same object: the audio this project shipped before there was a volume screen.
 * `UStratSoundDirector::ApplyVolumes` is written so that this state still APPLIES rather than
 * returning early, for the reason `EmitCue` states about its own null-bank arm -- an early
 * return on the shipped default makes every clause over the feature vacuous at once.
 *
 * THE VALUES ARE `[0, 1]` AND `Sanitize` IS THE ONLY THING THAT GUARANTEES IT. A `USaveGame` is
 * deserialized from a file a player can edit, so the members below are what was READ and not
 * what is legal; nothing in `UGameplayStatics::SetSoundMixClassOverride` refuses a gain of 40.
 */
UCLASS(BlueprintType)
class STRATPLAY_API UStratAudioSettings : public USaveGame
{
	GENERATED_BODY()

public:
	/**
	 * The slot these live in. NOT `FStratMatchConfig::SaveSlotName` and never derived from it.
	 *
	 * A LITERAL HERE IS NOT THE `SaveSlotName` DEFECT REPEATED, AND THE DIFFERENCE IS WORTH
	 * STATING BECAUSE THE TWO LOOK IDENTICAL. `AStratShellGameMode::SaveSlotName` derives its
	 * value from `FStratMatchConfig()` precisely because that string has ANOTHER author -- a
	 * designer property on a second GameMode -- and two authors of one string drift. This string
	 * has no other author: no Blueprint property names it, no designer can move it, and this
	 * line is the only place it exists in the tree. A literal with exactly one author is a
	 * constant.
	 *
	 * IT IS A PROCESS-WIDE PREFERENCE AND SO IT IS NOT A `UPROPERTY`. Nothing serializes it;
	 * it is the KEY the payload is stored under, and `StratSaveGame.h`'s "NO SLOT NAME AND NO
	 * USER INDEX" block gives the reason a payload must not record its own address.
	 */
	static const TCHAR* DefaultSlotName();

	/** The user index every `UGameplayStatics` slot call in this project passes. One machine,
	 *  one player; stated once so no call site invents a second. */
	static constexpr int32 kUserIndex = 0;

	/** Sec 2.11.5's master gain. `[0, 1]`, unity by default. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stratocracy|Audio")
	float MasterVolume = 1.0f;

	/** The seven `UStratSoundBank` cues' gain. `[0, 1]`, unity by default. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stratocracy|Audio")
	float SfxVolume = 1.0f;

	/** `AStratShellGameMode::TitleMusic`'s gain. `[0, 1]`, unity by default. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stratocracy|Audio")
	float MusicVolume = 1.0f;

	/**
	 * Force all three into `[0, 1]`, in place.
	 *
	 * CALLED ON EVERY LOAD AND ON EVERY WRITE, so no path exists by which an out-of-range gain
	 * reaches the engine. The rule itself is `StratClampVolume` in `StratUI` and is NOT restated
	 * here -- that function's own block records why one authority has to sit below both modules,
	 * and a second `FMath::Clamp` in this file would be the drift it exists to prevent.
	 *
	 * MUTATING RATHER THAN RETURNING A COPY, because the caller is always the owner of a freshly
	 * deserialized object and a version that returned a value would leave the unclamped original
	 * in scope for someone to pass on by accident.
	 */
	void Sanitize();
};
