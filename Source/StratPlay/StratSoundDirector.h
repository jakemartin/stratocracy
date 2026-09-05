// The AUDIO milestone's world half: the one object that actually plays anything, and the one
// seam a headless clause can assert audio against.
//
// WHAT GAP THIS CLOSES, AND IT IS A TESTABILITY GAP BEFORE IT IS A GAMEPLAY ONE. The suite
// runs `-nullrhi`, unattended, with no audio device: NOBODY CAN HEAR ANYTHING, and no clause
// will ever assert that a sound was audible. So a design in which "the cue fired" and "a wave
// played" are the same event is a design in which the entire feature is unobservable, and this
// project's record already carries two measured defects of that species -- a shipped zero
// default that made every clause vacuous, and a real default that could not signal unset. This
// class splits the two apart: `EmitCue` RECORDS FIRST AND PLAYS SECOND, on every path, and the
// record carries a NAMED DISPOSITION saying what happened to the request. A clause asserts one
// record, of the right cue, at the right moment, with `Disposition == NoBank` -- and that
// assertion is exactly as sharp with no bank configured as it would be with one.
//
// THE RULE THAT MAKES THAT TRUE, AND BREAKING IT IS THE WHOLE OF HOW THIS FEATURE GOES DARK:
// `EmitCue` HAS NO EARLY RETURN. Not for a null bank, not for an unset slot, not for a
// cooldown, not for a missing world. Each of those produces a record with its own disposition
// and then declines to play. An `if (Bank == nullptr) { return; }` at the top would be one line
// that reads like defensive hygiene and would make EVERY clause in the phase-B suite vacuous at
// once, because the shipped state of this project has no bank -- a green suite over a feature
// that records nothing. The four dispositions ARE the error handling; there is no other.
//
// A `UWorldSubsystem` AND NOT A MEMBER OF `UStratMatchSubsystem`, WHICH WAS THE OTHER SHAPE.
// The title map has no match and never will -- `AStratShellGameMode` starts no match, and
// `UStratMatchSubsystem` is created there but never seeded -- and its buttons still need to
// click. Hanging audio off the match owner would make `WBP_TitleMenu` silent by construction,
// and the fix would be a second player for the shell, which is two answers to one question. A
// world subsystem exists on both maps, is created before any `BeginPlay`, and dies with the
// world, so it needs no lifetime argument of its own.
//
// GAME AND PIE ONLY, MIRRORING `UStratMatchSubsystem::DoesSupportWorldType` LINE FOR LINE. An
// editor preview world -- the little viewport in an asset editor -- is a `UWorld` and would
// otherwise acquire one of these; that world is not a session and nothing in it should be able
// to make a noise. The mirror is deliberate rather than incidental: the two subsystems answer
// the same question about the same worlds, and one of them answering differently would be a
// difference nobody chose.
//
// PLAYBACK IS 2D AND NOT SPATIALIZED, AND THAT IS A DECISION WITH TWO MEASUREMENTS BEHIND IT
// RATHER THAN A SHORTCUT.
//   - `AStratCameraPawn` IS A SPRING-ARM CAMERA. Attenuation is distance-based, so a
//     spatialized cue would get quieter purely because the player zoomed out. The same event,
//     at the same hex, would be a different loudness depending on a camera control that has
//     nothing to do with the match. That is a mix that misinforms.
//   - `UStratMatchSubsystem::FocusPlaybackStep` SNAPS THE CAMERA PER TOUR STEP. A spatialized
//     cue armed before a snap is HEARD AFTER IT, from a listener position that has since
//     jumped across the board -- so the sound would arrive panned toward wherever the camera
//     was going rather than where the thing happened.
// THE AFFORDANCE IS KEPT RATHER THAN CLOSED OFF, WHICH IS THE HALF WORTH WRITING DOWN.
// `FStratSoundEmission::UnitId` is carried through to `FStratSoundEmissionRecord::UnitId`, and
// `UStratMatchSubsystem::FindUnitActor(UnitId)` resolves it to an actor. So the day this
// becomes positional, the change is ONE LINE INSIDE `EmitCue` -- `PlaySoundAtLocation` with
// that actor's location instead of `PlaySound2D` -- plus an attenuation slot on the bank.
// Nothing above this class has to move, and no clause written against the record changes.
//
// `NoteApplyViewObserved` IS NOT DECORATION AND IS THE ANSWER TO A QUESTION THE RECORD ALONE
// CANNOT ANSWER. An empty emission list has two causes that look identical: every cue was
// suppressed, or the seam in `ApplyView` is dead -- deleted, mis-ordered, or gated off. This
// project has a named defect class for exactly that ("a reflected verb with no caller reads as
// built": check the ROUTE, not the mechanism). The observation counter makes the two separable
// in one assertion: a fixture that refreshes twice and sees the counter at zero is looking at a
// dead seam, whatever the emission list says.
//
// WHAT IS DELIBERATELY NOT IN IT.
//   - NO DECIDING. This class never compares two view models, never reads a bridge, and never
//     asks whether a cue was warranted. `StratDecideSoundCues` owns that and is world-free so
//     it can be tested without this object at all.
//   - NO QUEUE, NO DUCKING, NO PRIORITY AND NO MIXER. `USoundConcurrency` on the bank is the
//     voice budget and it is content's to author. A priority ordering compiled in here would
//     be a mix decision no sound designer can reach.
//   - NO BOUND ON `Emissions`, AND THAT IS A NAMED COST. The record grows for the life of the
//     world; a long session accumulates one small struct per cue. It is unbounded because a
//     ring buffer would silently drop the early records a clause asserting a first-reconcile
//     property depends on, and "the fixture calls `ResetEmissions`" is a cheaper contract than
//     "the clause must run before the buffer wraps". DISCHARGED WHEN a shipping build measures
//     this as costing something -- at which point the cap belongs behind a `#if !UE_BUILD_SHIPPING`
//     around the recording, not behind a smaller buffer.
//   - NO CONSOLE VARIABLE AND NO PER-CUE GAIN. `UStratSoundBank` refuses to carry a volume
//     multiplier and this class does not add one; what a cue sounds like is the asset's.
//
// THE BULLET ABOVE USED TO READ WIDER THAN THAT AND IS CORRECTED RATHER THAN DELETED, because
// a reader arriving by a citation must be able to see what was claimed and what replaced it:
//   RETRACTED>  "NO SAVE, NO CONFIG AND NO CONSOLE VARIABLE. Volume lives on the assets; a
//   RETRACTED>   master mute is a `USoundMix` and is content's, not a bool here."
// Both halves of that sentence were right about the MECHANISM and were then read as a refusal
// of the FEATURE. A master mute IS a `USoundMix` -- and the object that applies an override to
// one has to be an object with a world, a bank and a lifetime, which in this project is this
// one and nothing else. `UStratSoundBank` now carries the mix and the three sound classes for
// the reason it carries the seven cue assets (CLAUDE.md #4: no `/Game/` literal), and this
// class carries the SAVE for a reason the retracted sentence did not consider: Sec 2.11.5
// budgets volume as a PLAYER control, and a player control that forgets itself between launches
// is not one. `UStratAudioSettings` is the payload and `ApplyVolumes` is the applier.
//
// `ApplyVolumes` IS BUILT ON `EmitCue`'S RULE AND NOT ON A NEW ONE. It records first, applies
// second, and HAS NO EARLY RETURN ON ANY PATH -- no bank, no mix, no settings object and no
// world each produce a named `EStratVolumeDisposition` and then a decision. The shipped state of
// this project has no settings slot, so an `if (Settings == nullptr) { return; }` would make
// every clause over this half of the feature vacuous in exactly the configuration a fresh
// checkout runs in. `FStratVolumeApplicationRecord` additionally carries the three gains THAT
// WERE USED and a count of the channels actually written, so "applied unity because nobody has
// ever moved a slider" and "applied nothing because the bank names no mix" are two readable
// answers rather than one silence.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "StratSoundCues.h"

#include "StratSoundDirector.generated.h"

class UStratAudioSettings;
class UStratSoundBank;
class USoundClass;

/**
 * What happened to one `EmitCue` request.
 *
 * EVERY ARM IS A DIFFERENT CAUSE OF SILENCE EXCEPT THE FIRST, AND THAT IS THE ENTIRE REASON
 * THIS ENUM EXISTS. "No sound came out" is not a finding a clause can use; "no sound came out
 * BECAUSE no bank was adopted" is. A single `bool bPlayed` would have collapsed four causes
 * into one, and the three that mean "correctly configured, deliberately silent" would be
 * indistinguishable from the one that means "nobody wired this up".
 */
UENUM(BlueprintType)
enum class EStratSoundDisposition : uint8
{
	/** `UGameplayStatics::PlaySound2D` was called. NOT a claim that anything was audible --
	 *  under `-nullrhi` there is no device and the call still returns. This arm means the
	 *  request reached the engine, which is the strongest claim this class is entitled to. */
	Played UMETA(DisplayName = "Played"),

	/** No `UStratSoundBank` has been adopted. THE SHIPPED STATE UNTIL A GAMEMODE BLUEPRINT
	 *  POINTS AT ONE, and therefore the disposition every phase-B clause will assert against.
	 *  A director in this state is fully functional as a recorder. */
	NoBank UMETA(DisplayName = "No bank"),

	/** A bank is adopted and its slot for this cue is null. An authoring state, not a fault. */
	NoSoundConfigured UMETA(DisplayName = "No sound configured"),

	/** `UStratSoundBank::MinSecondsBetween` for this cue has not elapsed since the last PLAYED
	 *  emission of it. See `EmitCue` on what a world with no ticking clock does to this arm. */
	SuppressedByCooldown UMETA(DisplayName = "Suppressed by cooldown"),

	/** `GetWorld()` returned null, so there is no world-context object to play into. Should be
	 *  unreachable from a live subsystem and is recorded rather than asserted away, because the
	 *  alternative is a `check()` that turns a silent cue into a crash. */
	NoWorld UMETA(DisplayName = "No world")
};

/**
 * One `EmitCue` call, as it was received and as it was disposed of.
 *
 * IT RECORDS THE REQUEST AND NOT THE RESULT OF PLAYING. There is no handle, no
 * `UAudioComponent` and no duration here, because `PlaySound2D` is fire-and-forget and this
 * class holds nothing afterwards. A record is the statement "this cue was asked for, at this
 * turn, about this unit, and here is what happened to the request".
 */
USTRUCT(BlueprintType)
struct FStratSoundEmissionRecord
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	EStratSoundCue Cue = EStratSoundCue::ButtonClick;

	/** `FStratSoundEmission::Side`, or whatever a direct caller passed. Never interpreted. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	int32 Side = INDEX_NONE;

	/** `FStratSoundEmission::UnitId`. Ignored by today's 2D playback and carried anyway -- see
	 *  the header block's affordance paragraph. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	int32 UnitId = INDEX_NONE;

	/** `FStratSoundEmission::Turn`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	int32 Turn = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	EStratSoundDisposition Disposition = EStratSoundDisposition::NoBank;
};

/**
 * What happened to one `ApplyVolumes` request.
 *
 * IT IS `EStratSoundDisposition`'S SIBLING AND SHARES ITS WHOLE REASON: "no sound came out" is
 * not a finding a clause can use, and the four ways this call can fail to move a gain are four
 * different bugs in four different files. THE ARMS ARE NOT THE SAME ARMS, though, and the
 * differences are the interesting part -- `NoSettings` is a state in which the call still
 * APPLIES (unity gains), and there is no cooldown here at all because a volume is a level and
 * not an event.
 */
UENUM(BlueprintType)
enum class EStratVolumeDisposition : uint8
{
	/** A bank, a mix and a settings object were all present, and every non-null sound class was
	 *  written. NOT a claim that anything became audible -- see `FStratVolumeApplicationRecord::
	 *  ChannelsApplied` for how many of the three classes actually existed, and `UStratSoundBank::
	 *  BaseMix` for the unchecked agreement with `Config/DefaultEngine.ini` that stands behind
	 *  every one of these calls. */
	Applied UMETA(DisplayName = "Applied"),

	/**
	 * The mix and the classes were reachable, but no `UStratAudioSettings` was supplied, so
	 * UNITY GAINS WERE APPLIED AND THE CALLS WERE STILL MADE.
	 *
	 * THIS ARM IS NOT A FAILURE AND THAT IS THE WHOLE POINT OF IT EXISTING SEPARATELY. A player
	 * who has never opened the options screen has no slot; the correct behaviour is the audio
	 * this project shipped before the screen existed, and the correct RECORD is one that says so
	 * rather than one that reads `Applied` and hides which values were used. `ApplyVolumes` was
	 * required to do exactly this rather than return early, on `EmitCue`'s reasoning -- an early
	 * return here makes every clause over the shipped configuration vacuous.
	 */
	NoSettings UMETA(DisplayName = "No settings (unity applied)"),

	/** No `UStratSoundBank` has been adopted, so there is no mix and no class to name. Zero
	 *  calls made. THE SHIPPED STATE UNTIL A GAMEMODE BLUEPRINT POINTS AT A BANK, and the arm
	 *  `OnWorldBeginPlay`'s own first application lands on by construction -- a world subsystem
	 *  begins play BEFORE any actor, so the bank cannot yet be adopted at that moment. */
	NoBank UMETA(DisplayName = "No bank"),

	/** A bank is adopted and `UStratSoundBank::BaseMix` is null. Zero calls made: an override
	 *  needs a mix to be written into and there is nowhere to put it. An authoring state, not a
	 *  fault. */
	NoMix UMETA(DisplayName = "No mix configured"),

	/** `GetWorld()` returned null, so there is no world-context object to pass to
	 *  `UGameplayStatics::SetSoundMixClassOverride`. Recorded rather than asserted away, on
	 *  `EStratSoundDisposition::NoWorld`'s stated reasoning. */
	NoWorld UMETA(DisplayName = "No world")
};

/**
 * One `ApplyVolumes` call, as it was received and as it was disposed of.
 *
 * IT CARRIES THE GAINS THAT WERE USED AND NOT THE GAINS THAT WERE ASKED FOR, and that is the
 * field a clause should read hardest. `ApplyVolumes` clamps and it substitutes unity for a
 * missing settings object, so the request and the application can legitimately differ; recording
 * the request would make the record a restatement of the caller's own argument, which proves
 * nothing about this class. Recording what was USED means a clamp that stopped clamping, and a
 * `NoSettings` arm that started applying something other than unity, are both visible.
 *
 * `ChannelsApplied` IS A COUNT AND NOT A BOOL, FOR THE REASON `EStratSoundDisposition` IS AN
 * ENUM AND NOT A `bool bPlayed`. Zero, one, two and three are four different authoring states of
 * `UStratSoundBank`'s three class slots, and a bool would collapse the three partial ones into
 * the complete one or into the empty one -- either way naming a configuration that is not what
 * happened.
 */
USTRUCT(BlueprintType)
struct FStratVolumeApplicationRecord
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	EStratVolumeDisposition Disposition = EStratVolumeDisposition::NoBank;

	/** The master gain actually passed to `SetSoundMixClassOverride`, or the value that would
	 *  have been passed on an arm that made no call. Always in `[0, 1]`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	float MasterVolume = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	float SfxVolume = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	float MusicVolume = 1.0f;

	/** How many of the bank's three `USoundClass` slots were non-null and therefore written.
	 *  0 on every arm but `Applied` and `NoSettings`, and 0..3 on those two. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	int32 ChannelsApplied = 0;

	/** True when the settings object came from a slot on disk, false when it was constructed
	 *  fresh. DISTINCT FROM `Disposition == NoSettings`: a director that loaded a slot
	 *  successfully and one that fell back to defaults both reach `Applied`, and only this field
	 *  separates "the player chose unity" from "the player has never chosen anything". */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	bool bSettingsCameFromSlot = false;
};

/**
 * The one object in this project that plays a sound.
 *
 * A RECORDER FIRST AND A PLAYER SECOND. See the header block: the ordering inside `EmitCue` is
 * the feature, not an implementation detail, and it is the property `strat-test-author` should
 * pin hardest.
 */
UCLASS()
class STRATPLAY_API UStratSoundDirector : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Game and PIE only. MIRRORS `UStratMatchSubsystem::DoesSupportWorldType` DELIBERATELY --
	 * an editor preview world is a `UWorld` and must not acquire one of these. See the header
	 * block.
	 */
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;

	/**
	 * Point this director at the bank a GameMode was authored with. Null is legal and means
	 * "back to recording only".
	 *
	 * ADOPT AND NOT OWN, on `UStratMatchSubsystem::AdoptBridge`'s naming: the asset belongs to
	 * the package, this holds a reference so the GC keeps it, and nothing here frees anything.
	 *
	 * IT IS CALLED TWICE PER SESSION ON THE SHIPPED PATH AND THAT IS FINE -- once by
	 * `AStratShellGameMode::BeginPlay` on the title map, and once from `StartMatchInternal` off
	 * `FStratMatchConfig::SoundBank` on the match map. They are different worlds and therefore
	 * different directors; the repetition is two objects each being configured once.
	 *
	 * IT DOES NOT CLEAR `Emissions`. A clause that adopts a bank mid-fixture is entitled to the
	 * records taken before it, and clearing here would silently delete the `NoBank` evidence
	 * that is the most useful thing this class produces.
	 */
	void AdoptSoundBank(UStratSoundBank* Bank);

	/** The adopted bank, or null. */
	UStratSoundBank* GetSoundBank() const { return SoundBank; }

	/**
	 * Ask for one cue.
	 *
	 * RECORDS FIRST, PLAYS SECOND, AND HAS NO EARLY RETURN ON ANY PATH. The header block states
	 * why at length; the short form is that an early return on a null bank makes every clause
	 * in this feature vacuous in the shipped configuration.
	 *
	 * THE COOLDOWN IS MEASURED AGAINST `UWorld::GetTimeSeconds`, AND WHAT THAT MEANS FOR A
	 * FIXTURE IS STATED HERE RATHER THAN DISCOVERED. An automation world that is never ticked
	 * has a clock that does not advance, so two emissions in one such fixture are AT THE SAME
	 * TIME and the second is suppressed if a minimum is configured. With no bank -- the shipped
	 * and the fixture default -- there is no minimum and the arm is unreachable, so this affects
	 * only a fixture that adopts a bank AND authors `MinSecondsBetween`. A clause wanting to
	 * drive `SuppressedByCooldown` must do both; a clause wanting to avoid it must do neither.
	 *
	 * THE COOLDOWN CLOCK IS STAMPED ONLY ON A `Played` EMISSION. A suppressed request does not
	 * push the window forward, so a burst of ten clicks in one second yields one sound and nine
	 * suppressions rather than one sound and then silence for ten times the interval.
	 */
	void EmitCue(EStratSoundCue Cue, int32 Side, int32 UnitId, int32 Turn);

	/**
	 * `EmitCue` for each entry, in order.
	 *
	 * A LOOP AND NOT A BATCH, DELIBERATELY -- there is no cross-emission decision here, and
	 * anything that looked like one would be the decider's job leaking downward. An empty array
	 * is the common case and does nothing, including not touching the observation counter,
	 * which `NoteApplyViewObserved` owns separately for that exact reason.
	 */
	void EmitCues(const TArray<FStratSoundEmission>& Emissions);

	/**
	 * Record that the `ApplyView` seam ran. Emits nothing.
	 *
	 * THE ONLY WAY TO TELL "SUPPRESSED" FROM "DEAD". See the header block. It is a separate call
	 * from `EmitCues` rather than a side effect of it, because the case worth detecting is
	 * precisely the one where `EmitCues` is handed nothing -- a counter incremented inside
	 * `EmitCues` would count the calls that already proved themselves and miss the ones that
	 * did not.
	 */
	void NoteApplyViewObserved();

	/** Every emission this director has been asked for, oldest first. Never trimmed -- see the
	 *  header block's named cost. */
	const TArray<FStratSoundEmissionRecord>& GetEmissions() const { return Emissions; }

	/** How many times `EmitCue` has been called, including via `EmitCues`. Equal to
	 *  `GetEmissions().Num()` today and kept separately so it stays true if the record ever
	 *  grows a cap. */
	int32 GetEmitCallCount() const { return EmitCallCount; }

	/** How many times `NoteApplyViewObserved` has been called. Zero after two refreshes means
	 *  the `ApplyView` seam is dead, whatever `GetEmissions` says. */
	int32 GetApplyViewObservationCount() const { return ApplyViewObservationCount; }

	// ---- THE VOLUME HALF. Sec 2.11.5's "volume", applied and remembered. ----

	/**
	 * Applied at `OnWorldBeginPlay` and again at the end of `AdoptSoundBank`.
	 *
	 * TWO CALLS PER WORLD, DELIBERATELY, AND THE FIRST ONE IS EXPECTED TO ACHIEVE NOTHING. A
	 * `UWorldSubsystem` begins play BEFORE any actor's `BeginPlay`, so at the first call the
	 * GameMode has not yet handed over a bank and the record reads `NoBank` with zero channels.
	 * That record is not waste: it is the evidence that this seam RAN, which is the question
	 * `NoteApplyViewObserved` exists to answer for the other half of this class and which an
	 * empty record cannot distinguish from a deleted call. The second call is the one that moves
	 * a gain.
	 *
	 * IT LOADS THE SLOT AT MOST ONCE PER DIRECTOR. `GetAudioSettings` caches, so the second call
	 * and every later one re-apply a value already in memory rather than touching the disk --
	 * which matters because `UStratShellSubsystem::GetMenuModel`'s own block records what a
	 * per-frame disk read costs when a UMG binding finds one.
	 */
	void ApplyCurrentVolumes();

	/**
	 * Record what will be applied, then apply it. NO EARLY RETURN ON ANY PATH.
	 *
	 * @param Settings  may be NULL, and null is a supported configuration rather than a caller
	 *                  error. See `EStratVolumeDisposition::NoSettings`: unity gains are applied
	 *                  and the calls are still made, so that the shipped state of this project
	 *                  -- no slot, nobody has opened the screen -- exercises the same code path
	 *                  a configured one does. An `if (Settings == nullptr) { return; }` here is
	 *                  the single edit that would make this feature unobservable, exactly as it
	 *                  would be in `EmitCue`.
	 *
	 * IT CLAMPS EVEN THOUGH `UStratAudioSettings::Sanitize` ALREADY DID, AND THE REDUNDANCY IS
	 * THE POINT RATHER THAN AN OVERSIGHT. This is `public` and takes a bare pointer, so a
	 * Blueprint, a fixture or a future caller can hand it an object that never went through the
	 * loader; the clamp is what makes the range a property of THIS function rather than of the
	 * path that usually reaches it. Both clamps call `StratClampVolume`, so there is one rule
	 * and two askings of it, never two rules.
	 *
	 * IT DOES NOT SAVE. Persisting is `CommitVolumes`, and the split exists so a screen can
	 * preview a drag without writing a file per frame.
	 */
	void ApplyVolumes(const UStratAudioSettings* Settings);

	/**
	 * Clamp, store, persist and apply -- the whole of what a slider release should do.
	 *
	 * `BlueprintCallable` BECAUSE THE CALLER IS AN ASSET, and that is the same reflection route
	 * `UStratShellSubsystem` is reached by: `UStratOptionsWidget` is in `StratUI`, which is BELOW
	 * this module and must never name it, so the join is a `Get World Subsystem` node in a WBP
	 * or a `StratPlay`-side owner binding `UStratOptionsWidget::OnAudioOptionsCommitted`. Either
	 * way no `#include` crosses the arrow.
	 *
	 * @return whether the slot was written. FALSE STILL APPLIED. A disk that refused the write is
	 *         a reason to tell the player their choice will not survive the session, and not a
	 *         reason to leave the audio where it was; separating the two answers is why this
	 *         returns a bool instead of void.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Sound")
	bool CommitVolumes(float InMasterVolume, float InSfxVolume, float InMusicVolume);

	/**
	 * The settings this director is applying, loaded from the slot on first ask.
	 *
	 * NEVER NULL AFTER THE FIRST CALL. A missing slot, a slot that failed to deserialize and a
	 * slot of the wrong class all produce a fresh `UStratAudioSettings` with three unity gains --
	 * which is the shipped audio -- and `WasLoadedFromSlot` is what tells those apart from a real
	 * load. Returning a default object rather than null is what lets `ApplyCurrentVolumes` have
	 * no branch of its own.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Sound")
	UStratAudioSettings* GetAudioSettings();

	/** Whether `GetAudioSettings` found a slot, as opposed to constructing defaults. False
	 *  before the first call. See `FStratVolumeApplicationRecord::bSettingsCameFromSlot`. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Sound")
	bool WasLoadedFromSlot() const { return bSettingsCameFromSlot; }

	/**
	 * Point this director at a different save slot, and drop the cache.
	 *
	 * IT EXISTS FOR FIXTURES AND THE HEADER SAYS SO RATHER THAN DISGUISING IT AS CONFIGURATION.
	 * `UStratAudioSettings::DefaultSlotName()` is a constant, so a clause that drove
	 * `CommitVolumes` without this would write THE DEVELOPER'S OWN AUDIO SETTINGS on every suite
	 * run -- this project has already measured that exact failure once, a guard that could not
	 * signal unset while fixtures read the real save. An empty name restores the default, so
	 * there is no state a caller can leave this in that a later caller cannot name.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Sound")
	void UseAudioSettingsSlot(const FString& InSlotName);

	/** The slot name in force. `UStratAudioSettings::DefaultSlotName()` unless overridden. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Sound")
	FString GetAudioSettingsSlotName() const { return AudioSettingsSlotName; }

	/** Every `ApplyVolumes` call, oldest first. Unbounded on `Emissions`' stated reasoning and
	 *  for the same named cost. */
	const TArray<FStratVolumeApplicationRecord>& GetVolumeApplications() const
	{
		return VolumeApplications;
	}

	/**
	 * Clear the volume record alone.
	 *
	 * SEPARATE FROM `ResetEmissions` AND NOT FOLDED INTO IT, because that function's contract is
	 * already written down -- "clear the record, the call count and the observation count" -- and
	 * a fixture that reset cues would otherwise silently lose the `NoBank` volume record taken at
	 * `OnWorldBeginPlay`, which is the one piece of evidence that the world-start seam is alive.
	 * It does not touch the cached settings or the slot name for the same reason `ResetEmissions`
	 * does not touch the cooldown stamps: those are configuration, not record.
	 */
	void ResetVolumeApplications();

	/**
	 * Clear the record, the call count and the observation count.
	 *
	 * IT DOES NOT CLEAR THE COOLDOWN STAMPS, AND THAT IS ON PURPOSE. Those are a statement about
	 * the world's clock, not about the record; a fixture that reset them would be able to
	 * construct a "cooldown expired" that no wall clock produced. A clause needing a fresh
	 * window advances the world instead.
	 *
	 * FOR FIXTURES AND FOR CONSOLE DEBUGGING. Nothing in the shipped path calls it.
	 */
	void ResetEmissions();

private:
	/** Adopted, never owned. `UPROPERTY` so the GC keeps the asset alive while a world holds
	 *  this director. */
	UPROPERTY(Transient)
	TObjectPtr<UStratSoundBank> SoundBank;

	/** See `GetEmissions`. */
	TArray<FStratSoundEmissionRecord> Emissions;

	/** Per-cue `UWorld::GetTimeSeconds` at the last PLAYED emission. Absent means never
	 *  played. Not a `UPROPERTY`: a map of an enum to a float owns no `UObject`. */
	TMap<EStratSoundCue, float> LastPlayedAtSeconds;

	int32 EmitCallCount = 0;
	int32 ApplyViewObservationCount = 0;

	/** Loaded once by `GetAudioSettings` and then held. `UPROPERTY` so the GC keeps it while
	 *  this director lives; `Transient` because it is a copy of a slot and not a source. */
	UPROPERTY(Transient)
	TObjectPtr<UStratAudioSettings> AudioSettings;

	/** See `UseAudioSettingsSlot`. Left EMPTY here and resolved on first use, because
	 *  `UStratAudioSettings::DefaultSlotName()` is a function and this class must not depend on
	 *  the order a member initialiser would run it in. Empty therefore means "the default",
	 *  which is also what `UseAudioSettingsSlot("")` means -- one spelling, one meaning. */
	UPROPERTY(Transient)
	FString AudioSettingsSlotName;

	/** See `WasLoadedFromSlot`. */
	bool bSettingsCameFromSlot = false;

	/** See `GetVolumeApplications`. */
	TArray<FStratVolumeApplicationRecord> VolumeApplications;

protected:
	/**
	 * Applies the volumes once, as early as this class can.
	 *
	 * IT RUNS BEFORE ANY ACTOR'S `BeginPlay` AND THEREFORE BEFORE ANY BANK EXISTS -- see
	 * `ApplyCurrentVolumes`, which states why the record it takes is worth having anyway. The
	 * override is here rather than in `Initialize` because `Initialize` runs during world
	 * creation, where there is nothing yet to play into.
	 */
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
};
