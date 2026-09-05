// The AUDIO milestone's asset carrier: seven cue slots, one concurrency, and a per-cue
// minimum spacing. The only file in this project that names a sound asset type.
//
// WHAT GAP THIS CLOSES. `StratSoundCues.h` decides WHAT HAPPENED and
// `UStratSoundDirector` decides WHETHER AND HOW to play it; neither may know which wave file
// a cue maps to, because CLAUDE.md #4 forbids a `/Game/` literal in gameplay C++ and because a
// renamed asset must not be a compile-time break. So the mapping lives in a `UDataAsset`
// authored in the editor, and every consumer takes it as a pointer it was handed.
//
// A `UDataAsset` AND NOT SEVEN PROPERTIES ON A GAMEMODE. There are two GameModes that need
// this -- `AStratGameMode` for the match map and `AStratShellGameMode` for the title map --
// and seven slots on each would be fourteen designer properties that must agree, with no
// instrument that could tell whether they do. One asset pointed at twice is one authored fact.
// The drift that remains is which ASSET each points at, and that is the debt
// `AStratShellGameMode::SoundBank` records against `SaveSlotName`'s precedent.
//
// THE SLOTS ARE TYPED `USoundBase` AND NOT `UMetaSoundSource`, AND THE REASON IS A LINK LINE.
// The assets phase C will author are MetaSounds, and `UMetaSoundSource` derives
// `USoundWaveProcedural` -> `USoundWave` -> `USoundBase`, so a MetaSound assigns into one of
// these slots with no cast and no adapter. Naming the plugin class here would force a
// `MetasoundEngine` dependency into `StratPlay.Build.cs` -- a module arrow bought for a type
// name that buys nothing, since `UGameplayStatics::PlaySound2D` takes `USoundBase*` anyway.
// `Engine` already carries `USoundBase`, `USoundConcurrency` and `UDataAsset`, so this file
// adds no dependency at all. That absence is recorded in `StratPlay.Build.cs` rather than left
// to be rediscovered.
//
// EVERY SLOT MAY BE NULL AND A NULL SLOT IS A CONFIGURATION, NEVER A FAULT. A bank with one
// sound in it is a legitimate authoring state on the way to seven, and this class refuses
// nothing and logs nothing. What a null costs is one named disposition in the director's
// record -- `EStratSoundDisposition::NoSoundConfigured` -- so that "silent because unset" and
// "silent because suppressed" are distinguishable by a clause. That is the entire error
// handling this file has, and it is deliberately on the other side of the boundary.
//
// WHAT IS DELIBERATELY NOT IN IT.
//   - NO VOLUME OR PITCH MULTIPLIERS. `USoundBase` carries its own volume, and a second
//     multiplier here would be a mix control in two places with no rule about which wins. A
//     sound designer changes the asset.
//   - NO MUSIC, NO AMBIENCE, NO LOOPS. Seven one-shot cues is the milestone. A looping bed
//     needs a component whose lifetime someone owns, which is a different design and not a
//     slot on this asset.
//   - NO ATTENUATION. Playback is 2D by decision, not by omission; `UStratSoundDirector`'s
//     header carries that argument and the two measurements behind it.
//   - NO PER-SIDE OR PER-UNIT VARIATION. `FStratSoundEmission` carries `Side` and `UnitId` and
//     this class ignores both. A MetaSound is where variation belongs, because it can vary
//     without a C++ change; a `TMap<int32, USoundBase*>` here would put a mix decision in a
//     shape only an engineer can extend.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "StratSoundCues.h"

#include "StratSoundBank.generated.h"

class USoundBase;
class USoundClass;
class USoundConcurrency;
class USoundMix;

/**
 * The seven cues' assets, as one authored object.
 *
 * SEVEN NAMED PROPERTIES AND NOT A `TMap<EStratSoundCue, USoundBase*>`, WHICH WAS THE OTHER
 * SHAPE AND WAS REJECTED. A map is one property in the details panel with an Add button, an
 * enum dropdown per entry, and no way for the editor to tell an author that `TurnEnded` is
 * unfilled -- an unset cue and an absent key look identical, which is this project's measured
 * "a default cannot signal unset" defect in a new costume. Seven named slots make the whole
 * surface visible at once and make an omission a blank field rather than a missing row. The
 * cost is that adding an eighth cue touches this file; that is one line and it is the right
 * place to notice.
 *
 * `MinSecondsBetween` IS A MAP FOR THE OPPOSITE REASON, and the asymmetry is the point: an
 * ABSENT key there means "no minimum", which is a real and common configuration rather than an
 * unfinished one. A missing entry says something true; a missing sound says nothing.
 */
UCLASS(BlueprintType)
class STRATPLAY_API UStratSoundBank : public UDataAsset
{
	GENERATED_BODY()

public:
	/** `EStratSoundCue::ButtonClick`. The only cue a title map can fire. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Sound")
	TObjectPtr<USoundBase> ButtonClick;

	/** `EStratSoundCue::TurnEnded`. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Sound")
	TObjectPtr<USoundBase> TurnEnded;

	/** `EStratSoundCue::UnitMoved`. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Sound")
	TObjectPtr<USoundBase> UnitMoved;

	/** `EStratSoundCue::UnitAttacked`. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Sound")
	TObjectPtr<USoundBase> UnitAttacked;

	/** `EStratSoundCue::UnitDestroyed`. Fires for a killing blow INSTEAD of `UnitAttacked`,
	 *  never as well -- `StratSoundCues.h` records why a one-shot kill produces no HP fall. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Sound")
	TObjectPtr<USoundBase> UnitDestroyed;

	/** `EStratSoundCue::FactoryBuiltUnit`. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Sound")
	TObjectPtr<USoundBase> FactoryBuiltUnit;

	/** `EStratSoundCue::MatchEnded`. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Sound")
	TObjectPtr<USoundBase> MatchEnded;

	/**
	 * Passed to every `PlaySound2D` call this bank feeds, or null for none.
	 *
	 * ONE CONCURRENCY FOR ALL SEVEN, WHICH IS A DECISION AND NOT A SIMPLIFICATION. The thing
	 * worth limiting is the TOTAL number of overlapping Stratocracy cues -- an AI hand-over
	 * that fires a move, an attack, a death and a build in the same frame is four voices, and
	 * per-cue limits would each say "one of me is fine" and together say four. A single shared
	 * object is the only shape that can express the real budget. A cue that genuinely needs its
	 * own rule gets a per-slot concurrency the day one does, and that is a second property, not
	 * a rework.
	 *
	 * NULL IS FINE AND IS THE SHIPPED DEFAULT UNTIL CONTENT SETS ONE. `PlaySound2D` accepts a
	 * null concurrency and applies the engine's default behaviour.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Sound")
	TObjectPtr<USoundConcurrency> Concurrency;

	/**
	 * The minimum wall-clock seconds between two plays of the SAME cue. An absent key means no
	 * minimum, and so does a non-positive value.
	 *
	 * THIS IS THE CROSS-CALL SPAM CONTROL AND IT IS NOT THE ONLY ONE. `StratDecideSoundCues`
	 * already collapses a whole refresh to at most one emission per kind, which handles the ten
	 * units moving in one AI hand-over. What it structurally cannot see is six separate calls
	 * in one second -- a player clicking a button six times, or six refreshes each carrying one
	 * move. That is what this is for. The two live at different layers because they answer
	 * different questions, and the decider's one is where a headless clause can see it while
	 * this one, being wall-clock, is where a headless clause cannot.
	 *
	 * AUTHORED IN SECONDS AND COMPARED AGAINST `UWorld::GetTimeSeconds`, which is why an
	 * automation fixture on a world that is never ticked sees a clock that does not advance --
	 * see `UStratSoundDirector::EmitCue`, which states what that makes observable and what it
	 * does not.
	 *
	 * AN UNSET MAP IS THE SHIPPED DEFAULT AND MEANS EVERY CUE MAY REPEAT FREELY. That is the
	 * behaviour this project had before audio existed, which is the state a phase adding a
	 * feature should default to.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Sound")
	TMap<EStratSoundCue, float> MinSecondsBetween;

	// ---- THE MIX. Added by the volume screen; see `UStratSoundDirector::ApplyVolumes`. ----
	//
	// FOUR MORE ASSET SLOTS ON *THIS* ASSET, AND THE ALTERNATIVE THAT LOOKS RIGHT IS THE ONE
	// THAT BREAKS A PROJECT RULE. `UGameplayStatics::SetSoundMixClassOverride` needs a
	// `USoundMix*` and a `USoundClass*`, and CLAUDE.md #4 forbids either arriving as a `/Game/`
	// literal in gameplay C++. The obvious shapes were a second `UDataAsset` for the mix and a
	// `ConstructorHelpers` lookup in the director; the first is a second authored object that
	// must agree with this one about which project it belongs to, and the second is the rule.
	// This asset is ALREADY the object every GameMode Blueprint points at, so the mix arrives
	// by the route the cue sounds already arrive by and nothing new has to be wired.
	//
	// A BANK THAT NAMES A MIX IS DOING TWO JOBS, AND THAT IS SAID PLAINLY RATHER THAN DENIED.
	// This class's own header opens by calling itself "the AUDIO milestone's asset carrier",
	// and it is now the carrier for two different kinds of asset: WHAT to play, and WHAT TO
	// PLAY IT THROUGH. They are separable in principle -- one bank per campaign with one shared
	// mix is a real configuration -- and they are not separated here because separating them
	// would create the exact drift `AStratShellGameMode::SoundBank` already records as its
	// worst case: two asset pointers that nothing in this tree can force to agree.
	// DISCHARGED WHEN a second bank exists. At that point the mix moves to its own `UDataAsset`
	// and this class holds a pointer to it, which is one property change and no call-site change
	// because every consumer already reads these through the bank.
	//
	// EVERY ONE OF THE FOUR MAY BE NULL, on the cue slots' own rule: a null is a configuration
	// and never a fault. What a null costs is a named disposition in
	// `FStratVolumeApplicationRecord` -- `NoMix` for the mix, and a `ChannelsApplied` count
	// below three for a class -- so "silent because unrouted" stays distinguishable from
	// "silent because the player asked for silence".

	/**
	 * The `USoundMix` every class override is written into. Null means volume is inert.
	 *
	 * IT MUST BE THE MIX NAMED BY `[/Script/Engine.AudioSettings] DefaultBaseSoundMix` IN
	 * `Config/DefaultEngine.ini`, AND NOTHING IN THIS TREE CHECKS THAT. The override map a
	 * `SetSoundMixClassOverride` writes is read only by `ApplyClassAdjusters`, whose callers
	 * both require the mix to be ACTIVE or activating -- and no code in `Source/` calls
	 * `SetBaseSoundMix` or `PushSoundMixModifier`, so the project default is the only thing that
	 * ever makes this mix active. Point this at a different mix and every override lands in a
	 * map nobody reads: the calls succeed, the record says `Applied`, and the volume does
	 * nothing. That is the sharpest silent failure in this feature.
	 *   THE ALTERNATIVE WAS TO PUSH THE MIX EXPLICITLY from `UStratSoundDirector` and stop
	 * depending on the INI key at all, which would have removed the unchecked agreement. It is
	 * NOT taken here, and the reason is that a pushed mix is a modifier on a STACK with its own
	 * lifetime, priority and pop -- state this class would then own across a level travel -- to
	 * buy a guarantee the project default already provides for one line of configuration.
	 * DISCHARGED WHEN a second mix exists, or when a screen needs to duck one against another;
	 * at that point the stack is the right model and the INI key becomes the fallback rather
	 * than the mechanism.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Sound")
	TObjectPtr<USoundMix> BaseMix;

	/**
	 * `UStratAudioSettings::MasterVolume`'s class.
	 *
	 * THE THREE CLASSES ARE NAMED PROPERTIES AND NOT A `TMap`, ON THIS FILE'S OWN STANDING
	 * ARGUMENT about the seven cue slots: a map's absent key and unset value are
	 * indistinguishable in a details panel, and "a default cannot signal unset" is a defect this
	 * project has already shipped once. Three named slots make an omission a blank field.
	 *
	 * MASTER IS EXPECTED TO BE THE PARENT OF THE OTHER TWO IN THE AUTHORED ASSET, AND THIS FILE
	 * CANNOT ENFORCE IT. `SetSoundMixClassOverride`'s `bApplyToChildren` defaults true, so a
	 * master override applied to a parent class reaches its children -- which is why a master
	 * slider works at all. A tree authored flat would leave master overriding nothing that any
	 * sound belongs to, and the record would still say `Applied`.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Sound")
	TObjectPtr<USoundClass> MasterSoundClass;

	/** `UStratAudioSettings::SfxVolume`'s class -- the class the seven cue assets belong to. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Sound")
	TObjectPtr<USoundClass> SfxSoundClass;

	/** `UStratAudioSettings::MusicVolume`'s class -- the class `AStratShellGameMode::TitleMusic`
	 *  belongs to. It is the one channel with exactly one asset behind it today, and it is a
	 *  separate channel anyway because Sec 2.11.5's budget names volume as a player control and
	 *  a music slider that also moved the cues would not be one. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Sound")
	TObjectPtr<USoundClass> MusicSoundClass;

	/**
	 * The asset for one cue, or null when that slot is unset.
	 *
	 * A SWITCH AND NOT A LOOKUP TABLE, so that a cue added to `EStratSoundCue` without a slot
	 * here is a compiler warning about an unhandled enumerator rather than a silent null at
	 * runtime. That is the whole reason this function exists instead of the director reading
	 * the members directly.
	 *
	 * TOTAL AND NEVER REFUSING. There is no arm that can fail; an unset slot is a null return
	 * and the caller records `NoSoundConfigured`.
	 */
	USoundBase* SoundFor(EStratSoundCue Cue) const;

	/** `MinSecondsBetween` for one cue, or 0 when unset. Zero and negative both mean "no
	 *  minimum", and they are collapsed here so no caller has to decide. */
	float MinSecondsBetweenFor(EStratSoundCue Cue) const;
};
