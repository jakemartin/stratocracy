// `UStratAudioSettings` and `UStratSoundDirector::ApplyVolumes` -- the volume state, persisted
// and applied. The local ID `GATE-AUDIO-SETTINGS` is authorized for this subject by the
// 2026-09-05 ruling in `Tools/architect/state/global.md`, which mints no acceptance ID and
// claims none: the settings slot, the mix application and the options widget are *"three facets
// of one domain -- volume state, PERSISTED, APPLIED through the mix, and PRESENTED on a
// widget"*. It is deliberately NOT `GATE-AUDIO`, whose own authorizing text scopes it to
// cue-reaction and pins it to three named files, none of them this one.
//
// ---------------------------------------------------------------------------------------
// THE ONE PROPERTY THE WHOLE FILE RESTS ON, ASSERTED FIRST FOR THAT REASON.
//
// `ApplyVolumes` HAS NO `return` ON ANY PATH AND FIVE NAMED DISPOSITIONS. That is the same
// discipline `EmitCue` was written under and it is what makes the shipped, unconfigured state
// observable rather than silent: a single `if (SoundBank == nullptr) { return; }` at the top
// would make every other clause here vacuous at once, and would read like defensive hygiene.
// `ApplyVolumesRecordsEveryDisposition` is written first, placed first, and is the clause to
// read first, because it is the one that keeps the others non-vacuous. This project's record
// already carries two measured defects of that exact species -- a shipped zero default that made
// every clause vacuous, and a real default that could not signal unset.
//
// ---------------------------------------------------------------------------------------
// NO CLAUSE HERE MAY TOUCH THE DEVELOPER'S OWN VOLUME SETTINGS, AND THAT IS AN ACTIVE RULE
// RATHER THAN A HOPE. `GetAudioSettings` resolves an empty slot name to
// `UStratAudioSettings::DefaultSlotName()` -- `StratocracyAudio`, the real file a real player
// and a real developer share -- so EVERY director in this file is handed
// `UseAudioSettingsSlot(<fixture name>)` BEFORE anything can call `GetAudioSettings`, and every
// slot a clause writes is deleted in the same clause. The fixture names carry a `_Fixture_`
// infix and a clause-specific suffix so a stray file left behind by a crashed run is
// identifiable and belongs to nobody's shipped configuration.
//
// A WORLD SUBSYSTEM THAT BEGINS PLAY WOULD READ THE REAL SLOT BEFORE ANY CLAUSE COULD SPEAK --
// `OnWorldBeginPlay` calls `ApplyCurrentVolumes` -- so these fixtures use
// `InitializeActorsForPlay` and never `UWorld::BeginPlay`, exactly as every other clause file in
// this directory does. That read would be harmless (it loads, it never writes) but it would make
// `bSettingsCameFromSlot` depend on whether the developer running the suite has ever moved a
// slider, which is a fixture that passes on one machine and fails on another.
//
// ---------------------------------------------------------------------------------------
// WHERE THE EXPECTATIONS COME FROM, FIELD BY FIELD.
//   - UNITY. Never the literal `1.0` where it means "the shipped default": it is read off a
//     freshly constructed `UStratAudioSettings`, which is the module-side authority for what
//     every field defaults to. A default changed to 0.8 must move these clauses' subject, not
//     redden them for the wrong reason.
//   - CLAMPING. Compared against `StratClampVolume`, the module's own clamp, called on the same
//     input. No interval is written here; `StratSoundCues.h` owns it.
//   - ROUND-TRIPPED GAINS. Compared against the FIRST director's own settings object, read back
//     through `GetAudioSettings()`. Nothing in the round-trip clause states what 0.25 becomes.
//   - `ChannelsApplied`. The one place a literal count IS written, and the reason is stated at
//     the clause: no module-side value publishes "how many channels a bank has". It is
//     bracketed by fixtures with three, one and zero classes set, so an implementation that
//     returned a constant is red whatever constant it chose.
//
// ---------------------------------------------------------------------------------------
// WHAT THIS FILE DOES NOT PIN, NAMED SO IT IS NOT READ AS COVERED.
//   - THAT ANYTHING IS AUDIBLE. `SetSoundMixClassOverride` is called with a real world, a real
//     `USoundMix` and real `USoundClass` assets, and under `-nullrhi` nothing can observe a
//     resulting gain. What is pinned is that the call site is REACHED for each configured
//     channel, counted by the director's own record.
//   - THE MIX TREE ITSELF. Whether `SCL_Strat_SFX` and `SCL_Strat_Music` are children of
//     `SCL_Strat_Master`, and whether `SMX_Strat_Base` is the default base mix, are asset facts
//     with no C++ author; the 2026-09-05 ruling names that subject and explicitly does not
//     quantify over it. No clause here reads a parent class.
//   - THE SHIPPED `DA_StratSoundBank`'s MIX PROPERTIES. `BaseMix`, `MasterSoundClass`,
//     `SfxSoundClass` and `MusicSoundClass` are unset on the shipped asset as this file is
//     written, so a shipped-asset parity clause for them would be red today and would be red for
//     a content reason rather than a code one. It belongs after the asset pass, and this
//     sentence is the debt. THE BANKS BELOW ARE FIXTURES BUILT IN THIS FILE, so nothing here
//     asserts anything about what ships.
// ---------------------------------------------------------------------------------------

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/OutputDevice.h"
#include "Misc/OutputDeviceRedirector.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "UObject/Class.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "StratAudioSettings.h"
#include "StratSoundBank.h"
#include "StratSoundCues.h"
#include "StratSoundDirector.h"

namespace StratAudioVolumeFixture
{
	/** The shipped mix and the three shipped sound classes, BY PATH AND AS FIXTURE INPUT ONLY.
	 *  A `/Game/` literal in a test is one of the two sanctioned exceptions to the project's
	 *  no-path-literals rule; these objects are handed to a bank this file constructs, and no
	 *  clause below asserts anything about the shipped `DA_StratSoundBank`. */
	const TCHAR* const kBaseMixPath      = TEXT("/Game/StratAudio/SMX_Strat_Base.SMX_Strat_Base");
	const TCHAR* const kMasterClassPath  = TEXT("/Game/StratAudio/SCL_Strat_Master.SCL_Strat_Master");
	const TCHAR* const kSfxClassPath     = TEXT("/Game/StratAudio/SCL_Strat_SFX.SCL_Strat_SFX");
	const TCHAR* const kMusicClassPath   = TEXT("/Game/StratAudio/SCL_Strat_Music.SCL_Strat_Music");

	/** A disposition's reflected name, for failure messages. Asked of the `UENUM` rather than
	 *  switched over here, on `StratShippedSoundBankParity.cpp`'s precedent. Display only. */
	FString DispositionWord(const EStratVolumeDisposition Disposition)
	{
		if (const UEnum* const Reflected = StaticEnum<EStratVolumeDisposition>())
		{
			return Reflected->GetNameStringByValue(static_cast<int64>(Disposition));
		}
		return FString::Printf(TEXT("<unreflected disposition %d>"),
		                       static_cast<int32>(Disposition));
	}

	/** See `StratSoundDirectorCallSite.cpp` on why `InitializeActorsForPlay` is not decoration
	 *  and why the world is destroyed before its context. Duplicated rather than shared, which
	 *  is this project's standing posture for a test helper -- sharing would put a header inside
	 *  a `Tests/` directory. */
	struct FTestWorldScope
	{
		UWorld* World = nullptr;

		FTestWorldScope()
		{
			World = UWorld::CreateWorld(EWorldType::Game, /*bInformEngineOfWorld*/ false);
			if (World != nullptr && GEngine != nullptr)
			{
				FWorldContext& Context = GEngine->CreateNewWorldContext(EWorldType::Game);
				Context.SetCurrentWorld(World);
				World->InitializeActorsForPlay(FURL());
			}
		}

		~FTestWorldScope()
		{
			if (World != nullptr)
			{
				World->DestroyWorld(false);
				if (GEngine != nullptr)
				{
					GEngine->DestroyWorldContext(World);
				}
				World = nullptr;
			}
		}

		FTestWorldScope(const FTestWorldScope&) = delete;
		FTestWorldScope& operator=(const FTestWorldScope&) = delete;
	};

	/** A bank with the mix and all three sound classes set. Null members are reported by the
	 *  caller as a fixture failure and never as a clause result: a bank whose assets failed to
	 *  load would silently turn an `Applied` clause into a `NoMix` one. */
	UStratSoundBank* MakeFullBank(FString& OutError)
	{
		UStratSoundBank* const Bank = NewObject<UStratSoundBank>();
		if (Bank == nullptr)
		{
			OutError = TEXT("could not construct a UStratSoundBank");
			return nullptr;
		}

		Bank->BaseMix           = LoadObject<USoundMix>(nullptr, kBaseMixPath);
		Bank->MasterSoundClass  = LoadObject<USoundClass>(nullptr, kMasterClassPath);
		Bank->SfxSoundClass     = LoadObject<USoundClass>(nullptr, kSfxClassPath);
		Bank->MusicSoundClass   = LoadObject<USoundClass>(nullptr, kMusicClassPath);

		if (Bank->BaseMix == nullptr || Bank->MasterSoundClass == nullptr
			|| Bank->SfxSoundClass == nullptr || Bank->MusicSoundClass == nullptr)
		{
			OutError = TEXT("the shipped mix and/or sound class assets did not load from "
				"/Game/StratAudio");
			return nullptr;
		}

		return Bank;
	}
}

// ---------------------------------------------------------------------------------------
// One record per call, on every one of the five dispositions.
//
// WHAT IT PINS. `ApplyVolumes` appends exactly one `FStratVolumeApplicationRecord` per call and
// never returns early, so the five states the feature can be in are five NAMED answers rather
// than four silences and one success. The gains on the record are the values that were used --
// clamped, or unity when there were no settings -- and `ChannelsApplied` counts the
// `SetSoundMixClassOverride` calls that were actually reached.
//
// THE FIVE FIXTURES, AND EACH ONE IS A DIFFERENT DELETION OF THE CONFIGURATION:
//   NoWorld    -- a director whose outer is not a world. The first branch, and the only one
//                 reached before the bank is consulted.
//   NoBank     -- a world director that adopted nothing. THE SHIPPED STATE of every fixture in
//                 this directory that predates the audio milestone.
//   NoMix      -- a bank with no `BaseMix`. THE SHIPPED STATE of `DA_StratSoundBank` today.
//   NoSettings -- a full bank, `ApplyVolumes(nullptr)`. The distinction that matters most: it
//                 still applies, at unity, and says so.
//   Applied    -- a full bank and real settings.
//
// THE RECORD COUNT IS ASSERTED AS A DELTA AFTER EVERY CALL, not once at the end. A body that
// recorded twice on one path and not at all on another would still total five.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratApplyVolumesRecordsEveryDispositionTest,
	"Stratocracy.StratPlay.GATE-AUDIO-SETTINGS.ApplyVolumesRecordsEveryDisposition",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratApplyVolumesRecordsEveryDispositionTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAudioVolumeFixture;

	// UNITY, READ OFF THE CLASS RATHER THAN WRITTEN HERE. This object is never saved and never
	// handed to a slot; it exists to answer "what does this class default to".
	const TStrongObjectPtr<UStratAudioSettings> Defaults(NewObject<UStratAudioSettings>());
	if (!Defaults.IsValid())
	{
		AddError(TEXT("could not construct a UStratAudioSettings to read the shipped defaults "
			"from; nothing was observed."));
		return false;
	}

	// ---- NoWorld ------------------------------------------------------------
	// A `UWorldSubsystem` resolves its world from its outer, so a director outered to the
	// transient package has none. This is the one disposition that cannot be produced inside a
	// world at all.
	{
		const TStrongObjectPtr<UStratSoundDirector> Worldless(NewObject<UStratSoundDirector>());
		if (!Worldless.IsValid())
		{
			AddError(TEXT("could not construct a worldless UStratSoundDirector."));
			return false;
		}

		Worldless->UseAudioSettingsSlot(TEXT("StratAudio_Fixture_Dispositions_NoWorld"));
		Worldless->ResetVolumeApplications();

		Worldless->ApplyVolumes(Defaults.Get());

		if (Worldless->GetVolumeApplications().Num() != 1)
		{
			AddError(FString::Printf(
				TEXT("a worldless ApplyVolumes recorded %d entries, not 1; nothing further was "
				     "observed on this director."),
				Worldless->GetVolumeApplications().Num()));
			return false;
		}

		const FStratVolumeApplicationRecord& Record = Worldless->GetVolumeApplications().Last();
		TestEqual(*FString::Printf(TEXT("a director with no world records NoWorld (it recorded "
			"%s)"), *DispositionWord(Record.Disposition)),
			Record.Disposition, EStratVolumeDisposition::NoWorld);
		TestEqual(TEXT("and applies no channel"), Record.ChannelsApplied, 0);
	}

	FTestWorldScope Scope;
	if (Scope.World == nullptr)
	{
		AddError(TEXT("could not create a test world; nothing was observed."));
		return false;
	}

	UStratSoundDirector* const Director = Scope.World->GetSubsystem<UStratSoundDirector>();
	if (Director == nullptr)
	{
		AddError(TEXT("the test world has no UStratSoundDirector; nothing was observed."));
		return false;
	}

	// BEFORE ANYTHING CAN CALL `GetAudioSettings`. See the file header.
	Director->UseAudioSettingsSlot(TEXT("StratAudio_Fixture_Dispositions"));
	Director->ResetVolumeApplications();

	// ---- NoBank -------------------------------------------------------------
	Director->ApplyVolumes(Defaults.Get());

	if (Director->GetVolumeApplications().Num() != 1)
	{
		AddError(FString::Printf(TEXT("the first ApplyVolumes recorded %d entries, not 1."),
			Director->GetVolumeApplications().Num()));
		return false;
	}

	{
		const FStratVolumeApplicationRecord& Record = Director->GetVolumeApplications().Last();
		TestEqual(*FString::Printf(TEXT("a director with no bank records NoBank (it recorded "
			"%s)"), *DispositionWord(Record.Disposition)),
			Record.Disposition, EStratVolumeDisposition::NoBank);
		TestEqual(TEXT("and applies no channel"), Record.ChannelsApplied, 0);
		TestEqual(TEXT("and still reports the gains it was handed"),
			Record.MasterVolume, Defaults->MasterVolume);
	}

	// ---- NoMix --------------------------------------------------------------
	// A bank with nothing on it: the shipped `DA_StratSoundBank`'s mix configuration as this
	// clause is written.
	{
		UStratSoundBank* const MixlessBank = NewObject<UStratSoundBank>();
		if (MixlessBank == nullptr)
		{
			AddError(TEXT("could not construct a mixless UStratSoundBank."));
			return false;
		}

		// `AdoptSoundBank` applies as a consequence of adopting -- that is its documented
		// second half -- so the record this clause reads is that call's, and the count moves by
		// one for it.
		const int32 Before = Director->GetVolumeApplications().Num();
		Director->AdoptSoundBank(MixlessBank);

		TestEqual(TEXT("adopting a bank applies the volumes exactly once"),
			Director->GetVolumeApplications().Num() - Before, 1);

		const FStratVolumeApplicationRecord& Record = Director->GetVolumeApplications().Last();
		TestEqual(*FString::Printf(TEXT("a bank with no base mix records NoMix (it recorded "
			"%s)"), *DispositionWord(Record.Disposition)),
			Record.Disposition, EStratVolumeDisposition::NoMix);
		TestEqual(TEXT("and applies no channel"), Record.ChannelsApplied, 0);
	}

	// ---- NoSettings and Applied ---------------------------------------------
	FString FixtureError;
	UStratSoundBank* const FullBank = MakeFullBank(FixtureError);
	if (FullBank == nullptr)
	{
		AddError(FString::Printf(TEXT("fixture: %s; the applying dispositions were not "
			"observed."), *FixtureError));
		return false;
	}

	Director->AdoptSoundBank(FullBank);

	{
		const int32 Before = Director->GetVolumeApplications().Num();
		Director->ApplyVolumes(nullptr);

		TestEqual(TEXT("ApplyVolumes(nullptr) records exactly once"),
			Director->GetVolumeApplications().Num() - Before, 1);

		const FStratVolumeApplicationRecord& Record = Director->GetVolumeApplications().Last();
		TestEqual(*FString::Printf(TEXT("a full bank with no settings records NoSettings (it "
			"recorded %s)"), *DispositionWord(Record.Disposition)),
			Record.Disposition, EStratVolumeDisposition::NoSettings);

		// THE HALF THAT MAKES `NoSettings` DIFFERENT FROM THE THREE REFUSALS ABOVE: it applied.
		// A literal 3 and the reason for it -- `UStratSoundBank` declares three channel
		// properties and publishes no count of them, so there is no module-side value to read
		// here. The zero and the one below are what stop this from being a constant.
		TestEqual(TEXT("and it still applied all three channels"), Record.ChannelsApplied, 3);
		TestEqual(TEXT("at the class's own unity default, on master"),
			Record.MasterVolume, Defaults->MasterVolume);
		TestEqual(TEXT("on sfx"), Record.SfxVolume, Defaults->SfxVolume);
		TestEqual(TEXT("and on music"), Record.MusicVolume, Defaults->MusicVolume);
	}

	{
		const TStrongObjectPtr<UStratAudioSettings> Settings(NewObject<UStratAudioSettings>());
		if (!Settings.IsValid())
		{
			AddError(TEXT("could not construct a UStratAudioSettings for the Applied case."));
			return false;
		}

		// Three DIFFERENT values, so a body that wrote one gain into all three fields is red.
		Settings->MasterVolume = 0.25f;
		Settings->SfxVolume    = 0.50f;
		Settings->MusicVolume  = 0.75f;

		const int32 Before = Director->GetVolumeApplications().Num();
		Director->ApplyVolumes(Settings.Get());

		TestEqual(TEXT("ApplyVolumes with settings records exactly once"),
			Director->GetVolumeApplications().Num() - Before, 1);

		const FStratVolumeApplicationRecord& Record = Director->GetVolumeApplications().Last();
		TestEqual(*FString::Printf(TEXT("a full bank and real settings record Applied (it "
			"recorded %s)"), *DispositionWord(Record.Disposition)),
			Record.Disposition, EStratVolumeDisposition::Applied);
		TestEqual(TEXT("all three channels are applied"), Record.ChannelsApplied, 3);

		// THE GAINS COME FROM THE SETTINGS OBJECT, THROUGH THE MODULE'S OWN CLAMP. Nothing here
		// states what 0.25 becomes.
		TestEqual(TEXT("the master gain is the settings' own, clamped"),
			Record.MasterVolume, StratClampVolume(Settings->MasterVolume));
		TestEqual(TEXT("the sfx gain is the settings' own, clamped"),
			Record.SfxVolume, StratClampVolume(Settings->SfxVolume));
		TestEqual(TEXT("the music gain is the settings' own, clamped"),
			Record.MusicVolume, StratClampVolume(Settings->MusicVolume));

		// AND THE THREE ARE NOT THE SAME NUMBER, which is what makes the three assertions above
		// three assertions rather than one repeated.
		TestNotEqual(TEXT("control: the three recorded gains are distinct, so a body that "
			"wrote one field into all three would be visible"),
			Record.MasterVolume, Record.MusicVolume);
	}

	// ---- `ChannelsApplied` IS A COUNT AND NOT A CONSTANT ---------------------
	// A bank with the mix and ONE class. The two brackets around the 3 above.
	{
		UStratSoundBank* const OneChannelBank = NewObject<UStratSoundBank>();
		if (OneChannelBank == nullptr)
		{
			AddError(TEXT("could not construct a single-channel UStratSoundBank."));
			return false;
		}

		OneChannelBank->BaseMix          = FullBank->BaseMix;
		OneChannelBank->MasterSoundClass = FullBank->MasterSoundClass;

		Director->AdoptSoundBank(OneChannelBank);

		const FStratVolumeApplicationRecord& Record = Director->GetVolumeApplications().Last();
		TestEqual(*FString::Printf(TEXT("a bank with one sound class applies one channel (it "
			"recorded disposition %s)"), *DispositionWord(Record.Disposition)),
			Record.ChannelsApplied, 1);
	}

	return true;
}

// ---------------------------------------------------------------------------------------
// A missing settings slot yields unity AND still applies -- "chose unity" versus "never chose".
//
// WHAT IT PINS, AND WHY IT IS NOT THE `NoSettings` CLAUSE ABOVE. That one hands `ApplyVolumes` a
// null pointer directly. This one goes through the shipped entry point, `ApplyCurrentVolumes` ->
// `GetAudioSettings`, on a slot that does not exist -- which is EVERY first-run player. The
// director constructs defaults, applies them through the mix, and records
// `bSettingsCameFromSlot = false`.
//
// THE `bSettingsCameFromSlot` FLAG IS THE POINT. Without it, a player who deliberately dragged
// every slider to unity and a player who has never opened the options screen produce byte-identical
// records, and no later feature ("restore defaults", "first-run audio prompt") can tell them
// apart. The round-trip clause below is this clause's other half: same fixture shape, flag true.
//
// A NON-ZERO `ChannelsApplied` IS ASSERTED HERE FOR THE SAME REASON. "Unity" and "did nothing"
// are the same three numbers on the record; only the channel count separates them.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratMissingAudioSlotAppliesUnityTest,
	"Stratocracy.StratPlay.GATE-AUDIO-SETTINGS.AMissingSettingsSlotAppliesUnityAndStillApplies",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratMissingAudioSlotAppliesUnityTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAudioVolumeFixture;

	const TCHAR* const kAbsentSlot = TEXT("StratAudio_Fixture_MissingSlot");

	const TStrongObjectPtr<UStratAudioSettings> Defaults(NewObject<UStratAudioSettings>());
	if (!Defaults.IsValid())
	{
		AddError(TEXT("could not construct a UStratAudioSettings to read the shipped defaults "
			"from; nothing was observed."));
		return false;
	}

	FTestWorldScope Scope;
	if (Scope.World == nullptr)
	{
		AddError(TEXT("could not create a test world; nothing was observed."));
		return false;
	}

	UStratSoundDirector* const Director = Scope.World->GetSubsystem<UStratSoundDirector>();
	if (Director == nullptr)
	{
		AddError(TEXT("the test world has no UStratSoundDirector; nothing was observed."));
		return false;
	}

	Director->UseAudioSettingsSlot(kAbsentSlot);
	Director->ResetVolumeApplications();

	// THE PREMISE, MEASURED AND NOT ASSUMED. A leftover file from a crashed earlier run would
	// turn this clause into the opposite of itself -- green on a slot that DID load -- so the
	// absence is asserted before it is relied on.
	if (UGameplayStatics::DoesSaveGameExist(kAbsentSlot, UStratAudioSettings::kUserIndex))
	{
		AddError(FString::Printf(TEXT("premise failed: the fixture slot '%s' already exists, so "
			"the missing-slot path was not the one exercised."), kAbsentSlot));
		return false;
	}

	FString FixtureError;
	UStratSoundBank* const Bank = MakeFullBank(FixtureError);
	if (Bank == nullptr)
	{
		AddError(FString::Printf(TEXT("fixture: %s; nothing was observed."), *FixtureError));
		return false;
	}

	// `AdoptSoundBank` applies as its documented second half, so this one call is the whole
	// shipped sequence a title map performs.
	Director->AdoptSoundBank(Bank);

	if (Director->GetVolumeApplications().Num() != 1)
	{
		AddError(FString::Printf(TEXT("adopting a bank recorded %d applications, not 1."),
			Director->GetVolumeApplications().Num()));
		return false;
	}

	const FStratVolumeApplicationRecord& Record = Director->GetVolumeApplications().Last();

	TestEqual(*FString::Printf(TEXT("a missing slot still applies (it recorded %s)"),
		*DispositionWord(Record.Disposition)),
		Record.Disposition, EStratVolumeDisposition::Applied);

	TestEqual(TEXT("at the class's own unity default, on master"),
		Record.MasterVolume, Defaults->MasterVolume);
	TestEqual(TEXT("on sfx"), Record.SfxVolume, Defaults->SfxVolume);
	TestEqual(TEXT("and on music"), Record.MusicVolume, Defaults->MusicVolume);

	// THE HALF THAT SEPARATES "APPLIED UNITY" FROM "DID NOTHING".
	TestTrue(*FString::Printf(TEXT("and it reached the mix: %d channels were applied"),
		Record.ChannelsApplied), Record.ChannelsApplied > 0);

	// THE HALF THAT SEPARATES "CHOSE UNITY" FROM "NEVER CHOSE".
	TestFalse(TEXT("the record says these gains did not come from a slot"),
		Record.bSettingsCameFromSlot);
	TestFalse(TEXT("and the director says so too"), Director->WasLoadedFromSlot());

	// AND NOTHING WAS WRITTEN. A read path that quietly created the file would make the next
	// run of this clause fail on its own premise -- which is a real failure, but one that would
	// be reported a day late and against the wrong subject.
	TestFalse(TEXT("reading an absent slot does not create it"),
		UGameplayStatics::DoesSaveGameExist(kAbsentSlot, UStratAudioSettings::kUserIndex));

	return true;
}

// ---------------------------------------------------------------------------------------
// Volumes round-trip through a slot, and come back marked as having come from one.
//
// WHAT IT PINS. `CommitVolumes` clamps, saves and applies; a SECOND, INDEPENDENT director
// pointed at the same slot loads those three gains, reports `WasLoadedFromSlot()` true, and
// applies them through the mix. That is the whole feature end to end -- the only thing between
// the two directors is the file.
//
// THE SECOND DIRECTOR IS THE WHOLE POINT AND IS NOT A CONVENIENCE. `GetAudioSettings` CACHES,
// so re-reading the first director would answer out of memory and would be green against a
// `SaveGameToSlot` that never wrote a byte. The second director has no cache to answer from.
//
// WHERE THE EXPECTATIONS COME FROM. The second director's gains are compared against the FIRST
// director's own settings object, read back through its `GetAudioSettings()`. Nothing here
// states what 0.25 becomes; the inputs exist only to be three distinct non-default numbers, and
// the clamp control asks the module's own `StratClampVolume` what an out-of-range input becomes.
//
// THE SLOT IS DELETED AT EVERY EXIT. A fixture slot left on disk would make the missing-slot
// clause above fail on its own premise on the next run, and would do it from a different file.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCommittedVolumesRoundTripTest,
	"Stratocracy.StratPlay.GATE-AUDIO-SETTINGS.CommittedVolumesRoundTripThroughASlot",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCommittedVolumesRoundTripTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAudioVolumeFixture;

	const FString RoundTripSlot(TEXT("StratAudio_Fixture_RoundTrip"));

	// A slot left by a crashed earlier run is deleted rather than trusted, and the delete
	// happens before the premise is measured rather than only at the end.
	UGameplayStatics::DeleteGameInSlot(RoundTripSlot, UStratAudioSettings::kUserIndex);

	FTestWorldScope Scope;
	if (Scope.World == nullptr)
	{
		AddError(TEXT("could not create a test world; nothing was observed."));
		return false;
	}

	UStratSoundDirector* const Writer = Scope.World->GetSubsystem<UStratSoundDirector>();
	if (Writer == nullptr)
	{
		AddError(TEXT("the test world has no UStratSoundDirector; nothing was observed."));
		return false;
	}

	Writer->UseAudioSettingsSlot(RoundTripSlot);
	Writer->ResetVolumeApplications();

	FString FixtureError;
	UStratSoundBank* const Bank = MakeFullBank(FixtureError);
	if (Bank == nullptr)
	{
		AddError(FString::Printf(TEXT("fixture: %s; nothing was observed."), *FixtureError));
		return false;
	}
	Writer->AdoptSoundBank(Bank);

	TestFalse(TEXT("premise: the writing director has not loaded a slot"),
		Writer->WasLoadedFromSlot());

	// AN OUT-OF-RANGE MASTER, so the clamp is exercised by the shipped entry point rather than
	// only by `ApplyVolumes`.
	const float RawMaster = 1.4f;
	const bool bCommitted = Writer->CommitVolumes(RawMaster, 0.5f, 0.25f);

	TestTrue(TEXT("committing volumes writes the slot"), bCommitted);
	TestTrue(TEXT("and the slot now exists"),
		UGameplayStatics::DoesSaveGameExist(RoundTripSlot, UStratAudioSettings::kUserIndex));
	TestTrue(TEXT("and the writing director now says its settings came from a slot"),
		Writer->WasLoadedFromSlot());

	const UStratAudioSettings* const Written = Writer->GetAudioSettings();
	if (Written == nullptr)
	{
		AddError(TEXT("the writing director has no settings object after committing; nothing "
			"was compared."));
		UGameplayStatics::DeleteGameInSlot(RoundTripSlot, UStratAudioSettings::kUserIndex);
		return false;
	}

	// THE CLAMP, ASKED OF THE MODULE. No interval is written in this file.
	TestEqual(TEXT("an out-of-range master is stored clamped"),
		Written->MasterVolume, StratClampVolume(RawMaster));

	// ---- THE READER. A second director, and only the file between them. -----
	UStratSoundDirector* const Reader = NewObject<UStratSoundDirector>();
	if (Reader == nullptr)
	{
		AddError(TEXT("could not construct a second UStratSoundDirector."));
		UGameplayStatics::DeleteGameInSlot(RoundTripSlot, UStratAudioSettings::kUserIndex);
		return false;
	}
	const TStrongObjectPtr<UStratSoundDirector> ReaderHandle(Reader);

	Reader->UseAudioSettingsSlot(RoundTripSlot);

	const UStratAudioSettings* const Loaded = Reader->GetAudioSettings();
	if (Loaded == nullptr)
	{
		AddError(TEXT("the second director loaded no settings at all; nothing was compared."));
		UGameplayStatics::DeleteGameInSlot(RoundTripSlot, UStratAudioSettings::kUserIndex);
		return false;
	}

	TestTrue(TEXT("the second director's settings came from the slot"),
		Reader->WasLoadedFromSlot());

	// EVERY EXPECTATION IS THE WRITER'S OWN OBJECT. Three fields, compared one by one, because
	// a body that round-tripped one field and defaulted the other two would otherwise pass on
	// whichever field was checked.
	TestEqual(TEXT("master round-trips"), Loaded->MasterVolume, Written->MasterVolume);
	TestEqual(TEXT("sfx round-trips"),    Loaded->SfxVolume,    Written->SfxVolume);
	TestEqual(TEXT("music round-trips"),  Loaded->MusicVolume,  Written->MusicVolume);

	// AND THE THREE ARE DISTINCT, so the three assertions above are three facts. Without this,
	// a loader that returned defaults would pass whenever the committed values happened to be
	// the defaults.
	TestNotEqual(TEXT("control: the round-tripped gains are not all the same number"),
		Loaded->SfxVolume, Loaded->MusicVolume);
	TestNotEqual(TEXT("control: and they are not the shipped unity default"),
		Loaded->MusicVolume, 1.0f);

	// AND THEY REACH THE MIX ON THE READER TOO, which is the half a pure field comparison
	// cannot see: a director that loaded correctly and applied nothing is silent.
	Reader->AdoptSoundBank(Bank);
	if (Reader->GetVolumeApplications().Num() == 0)
	{
		AddError(TEXT("the second director recorded no volume application at all."));
		UGameplayStatics::DeleteGameInSlot(RoundTripSlot, UStratAudioSettings::kUserIndex);
		return false;
	}

	{
		const FStratVolumeApplicationRecord& Record = Reader->GetVolumeApplications().Last();

		// THE READER IS WORLDLESS, so its disposition is `NoWorld` and its channel count is
		// zero. THAT IS DELIBERATE AND IS NOT A WEAKER ASSERTION: the fields this clause reads
		// -- the three gains and `bSettingsCameFromSlot` -- are written BEFORE the world branch
		// in `ApplyVolumes`, which is the property that makes them readable at all on a
		// director that has no world. The applying half is pinned by
		// `ApplyVolumesRecordsEveryDisposition` and by the missing-slot clause, both of which
		// use a world director; repeating it here would need a second world for no new fact.
		TestEqual(TEXT("the loaded master gain reaches the record"),
			Record.MasterVolume, Written->MasterVolume);
		TestEqual(TEXT("the loaded sfx gain reaches the record"),
			Record.SfxVolume, Written->SfxVolume);
		TestEqual(TEXT("the loaded music gain reaches the record"),
			Record.MusicVolume, Written->MusicVolume);
		TestTrue(TEXT("and the record says they came from a slot"),
			Record.bSettingsCameFromSlot);
	}

	UGameplayStatics::DeleteGameInSlot(RoundTripSlot, UStratAudioSettings::kUserIndex);
	TestFalse(TEXT("the fixture slot is deleted, so no later run inherits it"),
		UGameplayStatics::DoesSaveGameExist(RoundTripSlot, UStratAudioSettings::kUserIndex));

	return true;
}

// ---------------------------------------------------------------------------------------
// An absent audio slot is resolved without reading the file.
//
// THE REGRESSION THIS HOLDS, AND IT HAD NOTHING HOLDING IT. Before `GetAudioSettings` gated its
// load on `UGameplayStatics::DoesSaveGameExist`, every clause in the suite that reached a sound
// director logged `LogStreaming: Failed to read file 'StratocracyAudio.sav'` -- 109 of them --
// and the suite reported them as SucceededWithWarnings rather than as failures. A count read
// from `succeeded` alone was unmoved. THE PROPERTY IS THEREFORE PINNED HERE RATHER THAN LEFT TO
// A HUMAN NOTICING A WARNING COLUMN.
//
// IT PINS THE REQUIREMENT AND NOT THE HAZARD. The requirement is that resolving an absent slot
// touches no file; the hazard was one particular engine warning. So the assertion is over the
// SLOT NAME appearing in ANY captured line, not over that sentence -- an engine that reworded
// the warning, or logged it from a different category, is still caught.
//
// THE POSITIVE CONTROL IS AN UNGUARDED LOAD OF THE SAME ABSENT SLOT, which is exactly the code
// the guard replaced. Without it, a capture that was simply deaf would report the same silence
// as a working guard -- the failure mode this project has a named lesson about. The control is
// declared through `AddExpectedMessagePlain` with `Occurrences = 1`, which stops the control's
// own warning from downgrading this clause to SucceededWithWarnings.
//
// THAT DECLARATION IS ALSO AN ASSERTION THAT THE CONTROL FIRED, WITH ONE PRECONDITION THIS BLOCK
// USED TO LEAVE UNSTATED, AND STATING IT IS NOT A HEDGE. Read out of UE 5.8's
// `Runtime/Core/Private/Misc/AutomationTest.cpp`:
// `FAutomationTestFramework::InternalStopTest` computes `bTestSuccessful = bTestSuccessful &&
// !CurrentTest->HasAnyErrors() && CurrentTest->HasMetExpectedMessages();`, and
// `HasMetExpectedMessages` is the only site that raises the unfulfilled-expectation error
// (`ExpectedNumberOfOccurrences > 0 && Expected != Actual`). Because `&&` SHORT-CIRCUITS, the
// check runs only on a clause that is OTHERWISE PASSING; on an already-red clause it is never
// called and an unfulfilled declaration is silently dropped. `StratTitleMusicLifecycle.cpp`
// measured that silence on a red run and its header first generalised it into "an
// `AddExpectedMessagePlain` is not a positive control at all" -- an overclaim now retracted in
// place there, and the reason this paragraph is worth its length.
//
// SO THIS CLAUSE'S CONTROL IS DOUBLE-BOUND ON PURPOSE, and neither binding depends on the other.
// The declaration bites while this clause passes; and `ControlHits > 0` is asserted directly off
// the capture, which holds on a red run too. If the unguarded load ever stops speaking, the
// capture-count assertion fails on its own -- measured, since that is the assertion mutant 7
// reddened, not the declaration.
//
// THE CAPTURE OVERRIDES `CanBeUsedOnMultipleThreads()` TO RETURN TRUE. Measured 2026-08-14: a
// capture without it fails 1 run in 4 on byte-identical code, because
// `FOutputDeviceRedirector` drains its buffered queue to whichever devices are registered AT
// DRAIN TIME. The override routes this device to the unbuffered list, fed only by the
// synchronous broadcast inside the emitting `UE_LOG`, so the window it sees equals its own
// lifetime by construction rather than by timing.
// ---------------------------------------------------------------------------------------
namespace StratAudioVolumeFixture
{
	/** Captures every line `GLog` broadcasts for the object's lifetime. See the clause header
	 *  on why `CanBeUsedOnMultipleThreads` is not optional. */
	class FStratLogCapture : public FOutputDevice
	{
	public:
		FStratLogCapture()
		{
			if (GLog != nullptr)
			{
				GLog->AddOutputDevice(this);
			}
		}

		virtual ~FStratLogCapture()
		{
			if (GLog != nullptr)
			{
				GLog->RemoveOutputDevice(this);
			}
		}

		virtual void Serialize(const TCHAR* const V, const ELogVerbosity::Type,
		                       const FName&) override
		{
			Lines.Add(FString(V));
		}

		virtual bool CanBeUsedOnMultipleThreads() const override { return true; }

		/** Every captured line containing `Needle`, case-insensitively -- `FString::Contains`
		 *  ignores case here by default and that is the intended looseness: the instrument is
		 *  loose so the claim can be strict. */
		TArray<FString> Matching(const TCHAR* const Needle) const
		{
			TArray<FString> Hits;
			for (const FString& Line : Lines)
			{
				if (Line.Contains(Needle))
				{
					Hits.Add(Line);
				}
			}
			return Hits;
		}

		TArray<FString> Lines;
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAbsentAudioSlotLoadsWithoutTouchingTheFileTest,
	"Stratocracy.StratPlay.GATE-AUDIO-SETTINGS.AbsentAudioSlotLoadsWithoutTouchingTheFile",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAbsentAudioSlotLoadsWithoutTouchingTheFileTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratAudioVolumeFixture;

	// The name is the observation: it is unique to this clause, so any line mentioning it was
	// caused by this clause and by nothing else in the run.
	const FString AbsentSlot(TEXT("StratAudio_Fixture_AbsentSlot"));

	UGameplayStatics::DeleteGameInSlot(AbsentSlot, UStratAudioSettings::kUserIndex);
	if (UGameplayStatics::DoesSaveGameExist(AbsentSlot, UStratAudioSettings::kUserIndex))
	{
		AddError(FString::Printf(TEXT("premise failed: '%s' exists and could not be removed."),
			*AbsentSlot));
		return false;
	}

	// THE CONTROL'S OWN WARNING, DECLARED. `Occurrences = 1` is an assertion that the unguarded
	// load below did produce exactly one such line: a control that stopped firing would fail the
	// clause here rather than silently making the silence meaningless. THE PRECONDITION IS IN THE
	// HEADER -- that check is short-circuited away on an already-red clause -- which is why
	// `ControlHits > 0` is asserted separately below and is the binding that does not need it.
	AddExpectedMessagePlain(TEXT("Failed to read file"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 1);

	const TStrongObjectPtr<UStratSoundDirector> Director(NewObject<UStratSoundDirector>());
	if (!Director.IsValid())
	{
		AddError(TEXT("could not construct a UStratSoundDirector; nothing was observed."));
		return false;
	}

	Director->UseAudioSettingsSlot(AbsentSlot);

	int32 GuardedHits = 0;
	int32 ControlHits = 0;

	{
		FStratLogCapture Capture;

		// ---- THE SUBJECT. The shipped path, on an absent slot. ----
		const UStratAudioSettings* const Settings = Director->GetAudioSettings();

		// It must still ANSWER. A guard that returned null would produce the same silence.
		if (Settings == nullptr)
		{
			AddError(TEXT("resolving an absent slot produced no settings object at all, so the "
				"silence below is explained by the feature not working."));
			return false;
		}
		if (Director->WasLoadedFromSlot())
		{
			AddError(TEXT("the director reports it loaded from a slot that does not exist; the "
				"premise of this clause is broken."));
			return false;
		}

		GuardedHits = Capture.Matching(*AbsentSlot).Num();

		// ---- THE CONTROL. The same absent slot, loaded WITHOUT the guard: the exact code the
		// guard replaced. If this produces no line naming the slot, the capture is deaf and the
		// zero above means nothing.
		const USaveGame* const Unguarded =
			UGameplayStatics::LoadGameFromSlot(AbsentSlot, UStratAudioSettings::kUserIndex);

		ControlHits = Capture.Matching(*AbsentSlot).Num() - GuardedHits;

		TestNull(TEXT("premise: an unguarded load of an absent slot returns nothing"),
			Unguarded);
	}

	TestTrue(*FString::Printf(
		TEXT("control: an unguarded load of the absent slot is audible to the capture "
		     "(%d lines named it)"), ControlHits),
		ControlHits > 0);

	TestEqual(*FString::Printf(
		TEXT("resolving an absent slot through the director names the file nowhere in the log "
		     "(%d lines did)"), GuardedHits),
		GuardedHits, 0);

	return true;
}
