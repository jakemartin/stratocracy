// THE STANDING CLAIM ABOUT THIS FILE: `EmitCue` CONTAINS NO `return` BEFORE ITS LAST LINE.
// Not one. Every state that could tempt an early return -- no world, no bank, no asset, a
// cooldown -- is a NAMED DISPOSITION written into the record and then a decision not to play.
// The header block says why at length; the reason it is restated at the top of the .cpp is
// that a future reader tidying this function will be looking at the code and not at the
// header, and adding a guard clause here is the single change that would make the whole
// feature unobservable in its shipped configuration.
//
// THE SAME CLAIM NOW HOLDS FOR `ApplyVolumes`, AND IT IS STATED AS A SECOND CLAIM RATHER THAN
// BY WIDENING THE FIRST. The two functions are separately editable and a reader tidying one is
// not looking at the other: `ApplyVolumes` CONTAINS NO `return` AT ALL. A null settings pointer
// is `EStratVolumeDisposition::NoSettings` and unity gains that ARE STILL APPLIED, which is the
// arm every fresh checkout lands on, so a guard clause there costs the whole of the volume
// feature's observability rather than one branch of it.

#include "StratSoundDirector.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"

#include "StratAudioSettings.h"
#include "StratPlay.h"
#include "StratSoundBank.h"
#include "StratSoundCues.h"

bool UStratSoundDirector::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	// Game and PIE only, mirroring `UStratMatchSubsystem::DoesSupportWorldType`. See the
	// declaration on why an editor preview world must not get one.
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UStratSoundDirector::AdoptSoundBank(UStratSoundBank* const Bank)
{
	// ONE ASSIGNMENT. No log, no refusal, no clearing of the record -- see the declaration,
	// which states each of those omissions and why.
	SoundBank = Bank;

	// AND ONE CONSEQUENCE, WHICH IS NOT THE SAME THING AS A SECOND JOB. The mix and the three
	// sound classes arrive ON the bank, so before this line there was nothing to write a volume
	// override into and after it there is. Re-applying here is what turns "the player's saved
	// volume" from a value nobody reads on the title map into the gain the first button click is
	// played at. It is also why the shipped path takes TWO records per world and why the first
	// is expected to read `NoBank` -- see `ApplyCurrentVolumes`.
	//
	// UNCONDITIONAL, INCLUDING ON A NULL BANK. Adopting null is how a title map says "no audio",
	// and the record it produces (`NoBank`, zero channels) is the honest statement of that. A
	// guard here would make the two adoptions behave differently for no reason a reader could
	// reconstruct.
	ApplyCurrentVolumes();
}

void UStratSoundDirector::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// THE FIRST OF THE TWO APPLICATIONS, AND IT IS EXPECTED TO MOVE NOTHING. A world subsystem
	// begins play before any actor, so no GameMode has adopted a bank yet. What this buys is a
	// record proving the seam is alive -- the same question `NoteApplyViewObserved` answers for
	// the emission half, and one an empty list cannot answer for either.
	ApplyCurrentVolumes();
}

void UStratSoundDirector::NoteApplyViewObserved()
{
	++ApplyViewObservationCount;
}

void UStratSoundDirector::EmitCue(const EStratSoundCue Cue,
                                  const int32 Side,
                                  const int32 UnitId,
                                  const int32 Turn)
{
	++EmitCallCount;

	// THE RECORD IS APPENDED BEFORE ANYTHING IS DECIDED, so that no branch below can be taken
	// without leaving evidence. `Disposition` is filled in place afterwards; the entry exists
	// from this line on.
	FStratSoundEmissionRecord& Record = Emissions.AddDefaulted_GetRef();
	Record.Cue    = Cue;
	Record.Side   = Side;
	Record.UnitId = UnitId;
	Record.Turn   = Turn;

	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		Record.Disposition = EStratSoundDisposition::NoWorld;
	}
	else if (SoundBank == nullptr)
	{
		// THE SHIPPED STATE UNTIL A GAMEMODE BLUEPRINT POINTS AT A BANK, and therefore the arm
		// almost every phase-B clause will land on. It is a full, named answer rather than an
		// absence, which is the whole design.
		Record.Disposition = EStratSoundDisposition::NoBank;
	}
	else
	{
		USoundBase* const Sound = SoundBank->SoundFor(Cue);
		const float MinGap = SoundBank->MinSecondsBetweenFor(Cue);
		const float Now = World->GetTimeSeconds();
		const float* const LastAt = LastPlayedAtSeconds.Find(Cue);

		// THE THREE ARMS IN THE ORDER A READER WOULD ASK THEM. An unset slot is checked before
		// the cooldown because "there is no sound for this cue" is the more specific fact: a cue
		// with no asset and a configured minimum would otherwise report as suppressed, which
		// would send someone looking at the wrong property.
		if (Sound == nullptr)
		{
			Record.Disposition = EStratSoundDisposition::NoSoundConfigured;
		}
		else if (MinGap > 0.0f && LastAt != nullptr && (Now - *LastAt) < MinGap)
		{
			// THE ONE SUBTRACTION IN THIS FILE, AND IT IS OVER A WALL CLOCK RATHER THAN OVER
			// ANYTHING THE RULES MODULE OWNS. `StratSoundCues.cpp` states that it performs no
			// arithmetic at all; this is the other side of that boundary, where seconds are the
			// only quantity and no rules answer is being re-derived.
			Record.Disposition = EStratSoundDisposition::SuppressedByCooldown;
		}
		else
		{
			// 2D AND NOT SPATIALIZED. The header block carries the two measurements behind that
			// -- the spring-arm zoom and the per-step camera snap -- and the affordance that
			// makes the reversal one line: `Record.UnitId` plus
			// `UStratMatchSubsystem::FindUnitActor` resolves to an actor, and this call becomes
			// `PlaySoundAtLocation`.
			//
			// `bIsUISound` IS TRUE FOR `ButtonClick` ALONE, AND THE SPLIT IS NOT COSMETIC. A UI
			// sound is exempted from the engine's time dilation and from pause-state culling,
			// which is right for a control the player operated and wrong for a diegetic event:
			// a unit's death should slow with the game and stop with it, and a button's click
			// should not, because the button still works.
			const bool bIsUiSound = (Cue == EStratSoundCue::ButtonClick);

			// VolumeMultiplier 1, PitchMultiplier 1, StartTime 0 -- the engine defaults, written
			// out because `Concurrency` is the sixth parameter and there is no way to name it
			// without them. They are NOT mix decisions taken here; see `UStratSoundBank`, which
			// refuses to carry multipliers for that reason.
			UGameplayStatics::PlaySound2D(World, Sound,
			                              /*VolumeMultiplier=*/1.0f,
			                              /*PitchMultiplier=*/1.0f,
			                              /*StartTime=*/0.0f,
			                              SoundBank->Concurrency,
			                              /*OwningActor=*/nullptr,
			                              bIsUiSound);

			// STAMPED ONLY HERE, ON THE PLAYED PATH. A suppressed request must not push the
			// window forward -- see the declaration.
			LastPlayedAtSeconds.Add(Cue, Now);

			Record.Disposition = EStratSoundDisposition::Played;
		}
	}
}

void UStratSoundDirector::EmitCues(const TArray<FStratSoundEmission>& InEmissions)
{
	for (const FStratSoundEmission& Emission : InEmissions)
	{
		EmitCue(Emission.Cue, Emission.Side, Emission.UnitId, Emission.Turn);
	}
}

void UStratSoundDirector::ResetEmissions()
{
	Emissions.Reset();
	EmitCallCount = 0;
	ApplyViewObservationCount = 0;

	// `LastPlayedAtSeconds` IS DELIBERATELY NOT CLEARED. See the declaration: it is a statement
	// about the world's clock and not part of the record, and clearing it would let a fixture
	// manufacture an elapsed window no wall clock produced.
}

// =========================================================================================
// THE VOLUME HALF.
//
// THE STANDING CLAIM ABOUT `ApplyVolumes`, RESTATED WHERE THE CODE IS: IT CONTAINS NO `return`.
// Every state that could tempt one -- no world, no bank, no mix, no settings object -- is a
// named `EStratVolumeDisposition` written into `VolumeApplications` and then a decision about
// how many `SetSoundMixClassOverride` calls to make. The file header says why.
// =========================================================================================

UStratAudioSettings* UStratSoundDirector::GetAudioSettings()
{
	if (AudioSettings != nullptr)
	{
		return AudioSettings;
	}

	// EMPTY MEANS THE DEFAULT, RESOLVED HERE AND NOWHERE ELSE. See the member's declaration: the
	// name is not initialised at construction because it comes from a function, so exactly one
	// place has to turn "unset" into "the default" and this is it.
	if (AudioSettingsSlotName.IsEmpty())
	{
		AudioSettingsSlotName = UStratAudioSettings::DefaultSlotName();
	}

	// THE THREE FAILURES COLLAPSE TO ONE ANSWER AND THAT IS DELIBERATE. A slot that is absent, a
	// slot that fails to deserialize and a slot holding some other `USaveGame` subclass all mean
	// the same thing to a player -- "you have never chosen a volume" -- and all three produce the
	// audio this project shipped before the screen existed. What is NOT collapsed is whether a
	// load happened at all: `bSettingsCameFromSlot` records it, so "the player chose unity" and
	// "the player has chosen nothing" stay two readable states.
	//
	// ABSENCE IS ASKED ABOUT BEFORE IT IS ATTEMPTED, AND THE REASON IS NOISE RATHER THAN COST.
	// `UGameplayStatics::LoadGameFromSlot` reaches `FFileHelper::LoadFileToArray`, which emits
	// `LogStreaming: Warning: Failed to read file '<...>/StratocracyAudio.sav' error.` when the
	// file is not there. First run IS the absent case -- every fresh checkout, every CI job and
	// every player's first launch -- so that warning fires on the expected path, not an error
	// one. MEASURED: the 2026-09-05 headless suite reported `succeeded 304,
	// succeededWithWarnings 109` with all 109 carrying that one line and every per-test `state`
	// still `Success`; the entry count never moved, which is exactly why reading the count alone
	// missed it. A project whose banner states zero warnings cannot afford 109 expected ones,
	// because the 110th would be real and would look identical.
	//
	// `DoesSaveGameExist` IS THE RIGHT INSTRUMENT AND WAS CHECKED, NOT ASSUMED. It reaches
	// `IFileManager::FileSize` and returns a number; it opens nothing and logs nothing, so the
	// question can be asked on a machine with no slot without producing the very line this guard
	// removes. Verified by re-running the suite: the 109 warnings went to 0.
	//
	// A SLOT THAT EXISTS STAYS EXACTLY AS LOUD AS IT WAS, AND THAT IS TRUE BY CONSTRUCTION
	// RATHER THAN BY MEASUREMENT. When `DoesSaveGameExist` answers yes this is the same call
	// with the same arguments the old code made unconditionally, so no diagnostic that fired
	// before this guard can be quieter after it. The guard's whole reach is the absent case.
	// That is the intended split: absence is the expected first-run state and says nothing, an
	// I/O failure on a file that is really there is a genuine error -- something wrote bytes and
	// the disk will not give them back -- and it is the one audio-settings failure a player
	// could not diagnose from the sliders, so it keeps `LoadFileToArray`'s `LogStreaming` line.
	//
	// AND ONE THING THE FIRST DRAFT OF THIS BLOCK CLAIMED IS FALSE, RECORDED HERE RATHER THAN
	// CORRECTED SILENTLY. It said a present file that "fails to deserialize" still logs. It does
	// not. MEASURED 2026-09-05: a planted 14-byte `StratocracyAudio.sav` reading `not a
	// savegame` produced `succeeded 413, succeededWithWarnings 0` -- the file READS fine, so
	// `LoadFileToArray` is happy and the failure is a header-parse inside
	// `LoadGameFromMemory`, which returns null without a word. So the loud case is an
	// UNREADABLE FILE, not UNPARSEABLE BYTES. That silence predates this guard and is untouched
	// by it; see this function's entry in `Tools/architect/state/engine.md` for the debt.
	//
	// WHAT NONE OF THIS CHANGES: the three failures still collapse to one DISPOSITION, exactly
	// as the paragraph above says, and `bSettingsCameFromSlot` still separates "chose unity"
	// from "never chose". Only the logging moved, and only towards saying less about the case
	// that is not a failure.
	USaveGame* const Loaded =
		UGameplayStatics::DoesSaveGameExist(AudioSettingsSlotName, UStratAudioSettings::kUserIndex)
			? UGameplayStatics::LoadGameFromSlot(AudioSettingsSlotName,
			                                     UStratAudioSettings::kUserIndex)
			: nullptr;

	AudioSettings = Cast<UStratAudioSettings>(Loaded);
	bSettingsCameFromSlot = (AudioSettings != nullptr);

	if (AudioSettings == nullptr)
	{
		AudioSettings = Cast<UStratAudioSettings>(
			UGameplayStatics::CreateSaveGameObject(UStratAudioSettings::StaticClass()));
	}

	if (AudioSettings != nullptr)
	{
		// A SLOT IS A FILE A PLAYER CAN EDIT, so what came back is what was WRITTEN and not what
		// is legal. `Sanitize` is the only thing between that file and a gain of 40 reaching the
		// engine, and it is asked here rather than at the call sites so no later reader has to
		// remember. It is a no-op on a freshly constructed object, which is the point.
		AudioSettings->Sanitize();
	}

	return AudioSettings;
}

void UStratSoundDirector::UseAudioSettingsSlot(const FString& InSlotName)
{
	// THE CACHE GOES WITH THE NAME, NECESSARILY. A director left holding one slot's values while
	// reporting another slot's name would be the stale-read defect this project already records,
	// in a form no clause would think to look for.
	AudioSettingsSlotName = InSlotName;
	AudioSettings = nullptr;
	bSettingsCameFromSlot = false;
}

void UStratSoundDirector::ApplyCurrentVolumes()
{
	// NO BRANCH HERE, WHICH IS WHY `GetAudioSettings` PROMISES NEVER TO RETURN NULL AFTER ITS
	// FIRST CALL. Were it allowed to, the null would have to be handled twice -- once here and
	// once inside `ApplyVolumes`, which handles it anyway -- and the two handlers would be free
	// to disagree about what a missing slot means.
	ApplyVolumes(GetAudioSettings());
}

void UStratSoundDirector::ApplyVolumes(const UStratAudioSettings* const Settings)
{
	// APPENDED BEFORE ANYTHING IS DECIDED, on `EmitCue`'s rule: no branch below can be taken
	// without leaving evidence. Every field is filled in place afterwards.
	FStratVolumeApplicationRecord& Record = VolumeApplications.AddDefaulted_GetRef();
	Record.bSettingsCameFromSlot = bSettingsCameFromSlot;

	// THE GAINS ARE DECIDED FIRST AND UNCONDITIONALLY, BEFORE ANY QUESTION ABOUT WHETHER THEY
	// CAN BE DELIVERED. That ordering is what makes `NoBank` and `NoMix` records carry a real
	// number instead of a placeholder: a clause can assert that the player's saved 0.4 was the
	// value this director WOULD have applied, on a world where no bank was ever adopted.
	//
	// UNITY FOR A NULL SETTINGS OBJECT, AND THE CALLS ARE STILL MADE BELOW. See the declaration
	// and `EStratVolumeDisposition::NoSettings`.
	Record.MasterVolume = (Settings != nullptr) ? StratClampVolume(Settings->MasterVolume) : 1.0f;
	Record.SfxVolume    = (Settings != nullptr) ? StratClampVolume(Settings->SfxVolume)    : 1.0f;
	Record.MusicVolume  = (Settings != nullptr) ? StratClampVolume(Settings->MusicVolume)  : 1.0f;

	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		Record.Disposition = EStratVolumeDisposition::NoWorld;
	}
	else if (SoundBank == nullptr)
	{
		// THE SHIPPED STATE UNTIL A GAMEMODE BLUEPRINT POINTS AT A BANK, and the state
		// `OnWorldBeginPlay`'s own application lands in by construction.
		Record.Disposition = EStratVolumeDisposition::NoBank;
	}
	else if (SoundBank->BaseMix == nullptr)
	{
		// THE MIX IS CHECKED BEFORE THE CLASSES BECAUSE IT IS THE MORE SPECIFIC FACT, on
		// `EmitCue`'s ordering argument. An override needs somewhere to be written; with no mix
		// there is no such place and the state of the three class slots does not matter yet.
		Record.Disposition = EStratVolumeDisposition::NoMix;
	}
	else
	{
		// THE THREE CHANNELS, EACH SKIPPED INDEPENDENTLY WHEN ITS CLASS IS UNSET. A partially
		// authored bank applies the channels it can name and `ChannelsApplied` says how many --
		// see the record's declaration on why that is a count and not a bool.
		//
		// `FadeInTime` IS 0 AND THAT IS A DECISION. The engine default is 1 second, which is
		// right for a mix pushed by a gameplay event and wrong for a slider: a player dragging a
		// volume control is asking "what does this sound like NOW", and a one-second ramp makes
		// every drag report the previous position. `bApplyToChildren` is left at its default
		// true, which is what makes a master class override reach the two beneath it -- see
		// `UStratSoundBank::MasterSoundClass` on the authored tree that assumption rests on.
		const auto ApplyChannel = [&](USoundClass* const Class, const float Volume) -> int32
		{
			if (Class == nullptr)
			{
				return 0;
			}

			UGameplayStatics::SetSoundMixClassOverride(World, SoundBank->BaseMix, Class,
			                                           Volume,
			                                           /*Pitch=*/1.0f,
			                                           /*FadeInTime=*/0.0f,
			                                           /*bApplyToChildren=*/true);
			return 1;
		};

		Record.ChannelsApplied =
			  ApplyChannel(SoundBank->MasterSoundClass, Record.MasterVolume)
			+ ApplyChannel(SoundBank->SfxSoundClass,    Record.SfxVolume)
			+ ApplyChannel(SoundBank->MusicSoundClass,  Record.MusicVolume);

		// `NoSettings` IS REPORTED FROM THE ARM THAT APPLIED, NOT FROM AN ARM THAT REFUSED. The
		// calls above have already been made with unity gains; this line says WHY those were the
		// values, which is a different question from whether anything happened.
		Record.Disposition = (Settings != nullptr) ? EStratVolumeDisposition::Applied
		                                           : EStratVolumeDisposition::NoSettings;
	}
}

bool UStratSoundDirector::CommitVolumes(const float InMasterVolume,
                                        const float InSfxVolume,
                                        const float InMusicVolume)
{
	UStratAudioSettings* const Settings = GetAudioSettings();

	// UNREACHABLE UNLESS `CreateSaveGameObject` ITSELF FAILED, which is an out-of-memory shape
	// rather than a configuration. It still applies -- through the null path, which is the whole
	// reason that path exists -- so a player whose settings object could not be built hears the
	// shipped audio rather than whatever the last override left behind.
	if (Settings == nullptr)
	{
		ApplyVolumes(nullptr);
		return false;
	}

	// CLAMPED ON THE WAY IN, THROUGH THE ONE AUTHORITY. `Sanitize` would reach the same answer;
	// it is called anyway rather than instead, because these three assignments are what a caller
	// can read back off `GetAudioSettings` and they must already be legal when it does.
	Settings->MasterVolume = StratClampVolume(InMasterVolume);
	Settings->SfxVolume    = StratClampVolume(InSfxVolume);
	Settings->MusicVolume  = StratClampVolume(InMusicVolume);
	Settings->Sanitize();

	if (AudioSettingsSlotName.IsEmpty())
	{
		AudioSettingsSlotName = UStratAudioSettings::DefaultSlotName();
	}

	const bool bSaved = UGameplayStatics::SaveGameToSlot(Settings, AudioSettingsSlotName,
	                                                     UStratAudioSettings::kUserIndex);
	if (bSaved)
	{
		// A WRITE MAKES THE NEXT LOAD A SLOT LOAD, and the flag has to say so or every record
		// taken after the player's first choice would claim the values were defaults.
		bSettingsCameFromSlot = true;
	}
	else
	{
		UE_LOG(LogStratPlay, Warning,
			TEXT("Audio settings could not be written to slot '%s'; the volumes are applied for "
			     "this session only."), *AudioSettingsSlotName);
	}

	// APPLIED WHETHER OR NOT THE WRITE SUCCEEDED. See the declaration: a disk that refused is a
	// reason to warn, never a reason to leave the audio where it was.
	ApplyVolumes(Settings);

	return bSaved;
}

void UStratSoundDirector::ResetVolumeApplications()
{
	// THE RECORD ALONE. Neither the cached settings nor the slot name is touched -- see the
	// declaration, and `ResetEmissions`, which refuses to clear its own cooldown stamps for the
	// same reason: configuration is not record.
	VolumeApplications.Reset();
}
