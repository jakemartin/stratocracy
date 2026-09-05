// GATE-AUDIO -- what the SHIPPED ASSETS say about sound. The third and last file of the
// acceptance ID authorized in `Tools/architect/state/global.md`.
//
// WHY THIS FILE EXISTS, AND IT IS A GAP THE SUITE ITSELF MEASURED. Phase C authored seven
// `MetaSoundSource` assets, a `UStratSoundBank` pointing at all seven, a sound class tree and a
// concurrency, and set the bank on all three shipped GameMode Blueprints. The suite did not
// move by a single test. That is not a coincidence and it is not a comfort: every clause in
// `Source/StratPlay/Tests/StratSoundDirectorCallSite.cpp` builds its own `FStratMatchConfig` in
// C++ with no `SoundBank`, so every emission record in this tree lands on
// `EStratSoundDisposition::NoBank`, and NOT ONE CLAUSE HAD EVER READ A SHIPPED SOUND ASSET.
// Three of `EmitCue`'s five arms -- `Played`, `NoSoundConfigured` and `SuppressedByCooldown` --
// appear nowhere in `Source/` outside a display-only `switch`. A content pass could clear all
// seven slots, repoint one Blueprint at a different bank, or delete the bank outright, and the
// entire suite would stay green while the game went silent. The clauses below are the ones that
// would go red.
//
// THE TWO DEBTS THIS DISCHARGES, both recorded by `strat-gameplay-engineer` in phase A and both
// naming this phase as their creditor:
//
//   - `Source/StratPlay/StratMatchSubsystem.h`, on `FStratMatchConfig::SoundBank`: *"an ASSET
//     POINTER HAS NO C++ AUTHOR to derive from. Nothing in this tree can make the two agree by
//     construction, so nothing here pretends to. DISCHARGED BY a clause comparing the two CDOs,
//     which is phase D's and is not this file's to write."*
//   - `Source/StratPlay/StratShellGameMode.h`, on `AStratShellGameMode::SoundBank`: *"DISCHARGED
//     BY a clause reading both Blueprint CDOs and asserting the two pointers equal, exactly as
//     `SaveSlotName`'s discharge is stated one property up. That clause is phase D's and is not
//     this file's to write."*
//
// Both headers say TWO, and they are right about CLASSES and understate the ASSETS. There are
// two C++ classes carrying a `SoundBank` -- `FStratMatchConfig`'s, reached through
// `AStratGameMode`, and `AStratShellGameMode`'s own -- and THREE shipped Blueprints that set
// one, because `BP_StratGameMode_AiVsAi` is a second asset of the first class. The first clause
// below therefore compares three pointers and is named for three. A clause named `Both...` that
// asserted over three would be an overclaiming name of exactly the kind this project already
// renamed a clause to fix (`T-FAME-02.ShippedGameModeRunsAtEasy`, 2026-09-04), and the name is
// the one artifact that travels without its header.
//
// WHERE THE EXPECTATIONS COME FROM, clause by clause, because that is the column that matters:
//
//   - EveryShippedGameModeNamesTheSameSoundBank -- NO EXPECTATION AT ALL. Three module-side
//     pointers read off three CDOs, compared against each other. There is no literal to go
//     stale, and re-authoring the bank to a different asset keeps this clause green as long as
//     all three move together, which is precisely the property both headers asked for.
//   - ShippedGameModesAuthorTheirSoundBank -- NO EXPECTATION. Both sides of every comparison are
//     objects the engine produced: a Blueprint CDO and its native archetype, compared through
//     the reflected `FProperty`. The technique is `StratShippedGameModeMatchConfig.cpp`'s
//     `T-FAME-02.ShippedGameModeInheritsDifficultyFromCpp`, and the claim here is its MIRROR --
//     that clause asserts IDENTICAL and concludes inheritance, this one asserts NON-IDENTICAL
//     and concludes authorship.
//   - EveryShippedCueSoundCarriesTheSfxClass -- ONE ASSET-PATH LITERAL, `SCL_Strat_SFX`, and it
//     is the legitimate shape: the module is not the author of which sound class a cue belongs
//     to, `UStratSoundBank` carries no sound-class property to read, and the mix tree is content.
//     The clause loads the class ASSET and compares object identity, so a renamed class is a
//     red here and not a silently passing string compare.
//   - EveryCueInTheShippedBankHasASound -- NO EXPECTATION. `UStratSoundBank::SoundFor` is asked
//     for every enumerator `StaticEnum<EStratSoundCue>()` reports, and each answer must be
//     non-null. The cue list is read off the reflected enum rather than written here, so an
//     eighth cue is covered the day it is added.
//   - EveryCueInTheShippedBankReachesTheEngine -- NO EXPECTATION. The shipped bank is adopted by
//     a real `UStratSoundDirector` and every cue is emitted once; the disposition each records
//     is compared against `EStratSoundDisposition::Played`, a module-side enumerator.
//
// WHY THE `/Game/` LITERALS ARE HERE AND ARE NOT A VIOLATION. `CLAUDE.md` rule 4 binds GAMEPLAY
// C++ and names automation-test fixtures as one of its two standing exceptions. Every clause
// here has a NAMED ASSET as its subject; there is no Blueprint default that could hold a
// property pointing at the thing under test, and a soft reference would make a renamed asset
// resolve to null and report as "not configured" rather than "not found". Naming the path IS the
// test. Note what is NOT spelled as a literal: the sound bank itself is never loaded by path. It
// is read off the shipped GameMode's CDO, so these clauses are about THE BANK THE GAME USES and
// not about an asset that merely shares its name.
//
// EVERY FAILURE PATH IS RED AND NONE OF THEM SKIPS, on `StratShippedGameModeMatchConfig.cpp`'s
// stated posture. A clause about shipped audio that cannot find the asset has observed nothing,
// and reporting "could not tell" as "fine" would leave it green while pinning nothing -- strictly
// worse than not existing, because it also occupies the space where a real gate would go.
//
// WHAT THIS FILE DOES **NOT** PIN, said plainly so it is not inferred:
//
//   - IT DOES NOT PIN THAT ANYTHING IS AUDIBLE. `EStratSoundDisposition::Played` means
//     `UGameplayStatics::PlaySound2D` was called and returned; under `-nullrhi` there is no
//     audio device and the call is a no-op. That the MetaSound graphs produce a waveform a human
//     would recognise is not observable by any instrument in this tree and needs a human at the
//     keyboard.
//   - IT DOES NOT PIN THE MIX TREE ABOVE `SCL_Strat_SFX`. That `SCL_Strat_SFX` parents to
//     `SCL_Strat_Master`, that either has `SBM_Strat_SFX` as its default submix, or that a future
//     volume control reaches them, is a separate claim and would need its own assertion.
//   - IT DOES NOT PIN THE CONCURRENCY'S BEHAVIOUR. That `SC_StratCues` limits to four voices with
//     StopOldest is a property of that asset; nothing here reads it, and the fourth clause's
//     `Played` answers are per-cue and single so no limit is reached.
//   - IT DOES NOT PIN `MinSecondsBetween` OR THE `SuppressedByCooldown` ARM. Each cue is emitted
//     ONCE below, which is the only shape whose answer does not depend on a wall clock; these
//     worlds are never ticked, so `UWorld::GetTimeSeconds` does not advance and a second emission
//     would measure the fixture rather than the bank. That arm remains unpinned and is named in
//     `Tools/architect/state/tests.md` as such.
//   - IT DOES NOT PIN THAT ANY OF THESE BLUEPRINTS IS ITS MAP'S GAMEMODE. `Lvl_FerrumCrossing`'s
//     binding is a single unguarded `GlobalDefaultGameMode` line in `Config/DefaultEngine.ini`,
//     and that is a different subject.
// ---------------------------------------------------------------------------

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundClass.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"
#include "UObject/UObjectGlobals.h"

#include "StratGameMode.h"
#include "StratMatchSubsystem.h"
#include "StratShellGameMode.h"
#include "StratSoundBank.h"
#include "StratSoundCues.h"
#include "StratSoundDirector.h"

namespace StratShippedSoundBankParity
{
	/**
	 * The match map's GameMode Blueprint's generated class, by path.
	 *
	 * The `_C` suffix is the generated CLASS inside the package, as against the `UBlueprint`
	 * asset object carrying the editor-only graph. It is the class a GameMode spawn resolves
	 * and the one whose CDO carries the class defaults, so it is the only object that can
	 * answer what the shipped game is configured to play.
	 */
	const TCHAR* const kMatchGameModeClassPath =
		TEXT("/Game/StratPlay/BP_StratGameMode.BP_StratGameMode_C");

	/** The AI-versus-AI GameMode Blueprint. A THIRD SHIPPED ASSET AND NOT A CONTROL: it starts
	 *  real matches on the same map and its cues are as audible as the shipped mode's, so a
	 *  bank that drifted here would be a silent demo nobody files. See the header on why this
	 *  makes the first clause's name say "Every" and not "Both". */
	const TCHAR* const kAiVsAiGameModeClassPath =
		TEXT("/Game/StratPlay/BP_StratGameMode_AiVsAi.BP_StratGameMode_AiVsAi_C");

	/** The title map's GameMode Blueprint. Carries `AStratShellGameMode::SoundBank` -- a
	 *  DIFFERENT C++ property on a different class, which is the whole reason the two can drift
	 *  and the whole reason the first clause exists. */
	const TCHAR* const kShellGameModeClassPath =
		TEXT("/Game/StratPlay/BP_StratShellGameMode.BP_StratShellGameMode_C");

	/** The sound class every cue is expected to carry. A CONTENT FACT WITH NO C++ AUTHOR: see
	 *  the header on why this one literal is the legitimate shape. */
	const TCHAR* const kSfxSoundClassPath =
		TEXT("/Game/StratAudio/SCL_Strat_SFX.SCL_Strat_SFX");

	/** The CONTROL sound class. Exists, is a `USoundClass`, and is NOT the one cues carry --
	 *  so an identity comparison that had gone inert is visible. */
	const TCHAR* const kMusicSoundClassPath =
		TEXT("/Game/StratAudio/SCL_Strat_Music.SCL_Strat_Music");

	/** An object's path name, or `<null>`. Display only; nothing is asserted about the string. */
	FString Describe(const UObject* const Object)
	{
		return Object != nullptr ? Object->GetPathName() : FString(TEXT("<null>"));
	}

	/**
	 * A cue's reflected name.
	 *
	 * ASKED OF THE `UENUM` RATHER THAN SWITCHED OVER IN THIS FILE, on
	 * `StratShippedGameModeMatchConfig.cpp`'s `DescribeDifficulty` precedent: a hand-written
	 * switch would be a second author of the cue names and would print a stale one the day a
	 * cue is renamed. Display only.
	 */
	FString CueWord(const EStratSoundCue Cue)
	{
		if (const UEnum* const Reflected = StaticEnum<EStratSoundCue>())
		{
			return Reflected->GetNameStringByValue(static_cast<int64>(Cue));
		}
		return FString::Printf(TEXT("<unreflected cue %d>"), static_cast<int32>(Cue));
	}

	/** A disposition's reflected name, for the same reason and with the same null guard. */
	FString DispositionWord(const EStratSoundDisposition Disposition)
	{
		if (const UEnum* const Reflected = StaticEnum<EStratSoundDisposition>())
		{
			return Reflected->GetNameStringByValue(static_cast<int64>(Disposition));
		}
		return FString::Printf(TEXT("<unreflected disposition %d>"),
		                       static_cast<int32>(Disposition));
	}

	/**
	 * Every cue the game has, READ OFF THE REFLECTED ENUM AND NOT WRITTEN HERE.
	 *
	 * A hand-written list of seven would be this file deciding what the cue set is, and would
	 * quietly stop covering an eighth cue on the day one is added -- the exact shape of the gap
	 * this whole file exists to close, reintroduced one level down. `UHT` appends a hidden
	 * `_MAX` sentinel to every `UENUM`; it is skipped by name, and `UEnum::HasMetaData` is not
	 * used because a sentinel carries no metadata to test.
	 *
	 * Returns empty when the enum is unreflected, and every caller treats that as red rather
	 * than as "no cues to check" -- an empty loop passes every assertion inside it.
	 */
	TArray<EStratSoundCue> AllCues()
	{
		TArray<EStratSoundCue> Out;

		const UEnum* const Reflected = StaticEnum<EStratSoundCue>();
		if (Reflected == nullptr)
		{
			return Out;
		}

		for (int32 Index = 0; Index < Reflected->NumEnums(); ++Index)
		{
			const FString Name = Reflected->GetNameStringByIndex(Index);
			if (Name.EndsWith(TEXT("_MAX"), ESearchCase::CaseSensitive))
			{
				continue;
			}
			Out.Add(static_cast<EStratSoundCue>(Reflected->GetValueByIndex(Index)));
		}

		return Out;
	}

	/**
	 * A GameMode Blueprint's generated class, or `nullptr` WITH THE TEST ALREADY FAILED.
	 *
	 * Never returns a "could not tell" a caller might read as benign: every early exit has
	 * already recorded a failure, so a null here means the clause is red and the caller's only
	 * correct move is to stop.
	 *
	 * THE NATIVE-CLASS CHECK IS THE LOAD-BEARING ONE. A CDO read is an EFFECTIVE read: it
	 * reports the asset's override if there is one and the C++ default if there is not, and the
	 * two are indistinguishable at the call site. If a later edit pointed one of these paths at
	 * the native class, every `SoundBank` below would read the C++ default -- null -- and the
	 * clauses would report "not configured" rather than "not found". That is the one direction
	 * this must never fail in, so it is asserted rather than assumed.
	 */
	UClass* ResolveGeneratedClassOrFail(FAutomationTestBase& Test, UClass* const BaseClass,
	                                    const TCHAR* const Path)
	{
		UClass* const Generated = StaticLoadClass(BaseClass, nullptr, Path);

		if (!Test.TestNotNull(
				FString::Printf(
					TEXT("GATE-AUDIO: the GameMode Blueprint's generated class loads from '%s'. "
					     "If this failed the asset is missing, renamed or moved -- and this "
					     "clause CANNOT conclude anything about what it configures, so it FAILS "
					     "rather than passing. Fix the asset or fix this path; do not soften "
					     "this into a skip."),
					Path),
				Generated))
		{
			return nullptr;
		}

		if (!Test.TestTrue(
				FString::Printf(
					TEXT("GATE-AUDIO: '%s' resolved to a Blueprint-generated class and not to "
					     "native C++ (read: '%s'). This clause's subject is an ASSET's class "
					     "default; a native class here means the path no longer points at the "
					     "asset and every SoundBank below would be the C++ default -- null -- "
					     "wearing the asset's name."),
					Path, *Generated->GetName()),
				!Generated->HasAnyClassFlags(CLASS_Native)))
		{
			return nullptr;
		}

		if (!Test.TestTrue(
				FString::Printf(
					TEXT("GATE-AUDIO: '%s' derives from '%s' (read: '%s'), so its CDO actually "
					     "carries the SoundBank property this clause reads"),
					Path, *BaseClass->GetName(), *Generated->GetName()),
				Generated->IsChildOf(BaseClass)))
		{
			return nullptr;
		}

		return Generated;
	}

	/** The sound bank a `AStratGameMode` Blueprint configures, or null with the test failed. */
	UStratSoundBank* MatchModeBankOrFail(FAutomationTestBase& Test, const TCHAR* const Path)
	{
		UClass* const Generated =
			ResolveGeneratedClassOrFail(Test, AStratGameMode::StaticClass(), Path);
		if (Generated == nullptr)
		{
			return nullptr;
		}

		const AStratGameMode* const Cdo = Generated->GetDefaultObject<AStratGameMode>();
		if (!Test.TestNotNull(
				*FString::Printf(
					TEXT("GATE-AUDIO: '%s' has a class default object -- the object carrying the "
					     "FStratMatchConfig a spawned GameMode starts from"),
					Path),
				Cdo))
		{
			return nullptr;
		}

		return Cdo->MatchConfig.SoundBank.Get();
	}

	/**
	 * See `StratMatchConclusion.cpp` on why `InitializeActorsForPlay` is not decoration and why
	 * the world is destroyed before its context. Duplicated from the other clause files in this
	 * directory rather than shared, which is this project's standing posture for a test helper
	 * -- sharing would put a header inside a `Tests/` directory.
	 */
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
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- all three shipped GameMode Blueprints name ONE sound bank.
//
// THE CLAUSE TWO C++ HEADERS RECORDED AS OWED, and the only instrument in this project that can
// see the drift they describe. `FStratMatchConfig::SoundBank` and `AStratShellGameMode::SoundBank`
// are two different properties on two different classes, set on three different assets by hand.
// Unlike `SaveSlotName` -- whose two copies are reconciled in C++, because
// `AStratShellGameMode`'s constructor derives it from `FStratMatchConfig()`'s default -- AN ASSET
// POINTER HAS NO C++ AUTHOR. Nothing can make these agree by construction; only a clause can
// notice when they stop.
//
// WHAT A RED HERE COSTS IF NOBODY WRITES IT: the title screen's buttons and the match screen's
// buttons play different sounds, or one map is silent, and NOTHING IN THE GAME REPORTS IT. There
// is no error state -- a null bank is a legitimate configuration (`UStratSoundBank`'s header says
// so in terms) and a different-but-valid bank is legitimate too. The failure is two maps that
// sound different, which is not a bug anybody files.
//
// THE EXPECTATION IS NOTHING, WHICH IS THE POINT. This clause holds no path to a bank, no asset
// name and no literal. It reads three pointers off three CDOs and asserts they are one object.
// Re-author the bank tomorrow and this clause stays green as long as all three move together --
// which is exactly and only the property the two headers asked for.
//
// THE CONTROL. An equality over three nulls is also an equality, and null is the C++ default of
// all three properties, so "all equal" alone would pass on a tree where phase C had never run.
// Non-nullness is therefore asserted FIRST and fatally, per pointer, before any comparison.
//
// WHAT IT DOES NOT PIN. Not that the bank is the RIGHT bank -- there is no module-side value
// that could say which asset is right, and the remaining clauses in this file pin what is IN it
// instead. Not that any Blueprint AUTHORED the value rather than inheriting it; that is the next
// clause, and without it this one would be satisfied by three nulls if the C++ default ever
// stopped being null.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratEveryShippedGameModeNamesTheSameSoundBankTest,
	"Stratocracy.StratPlay.GATE-AUDIO.EveryShippedGameModeNamesTheSameSoundBank",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratEveryShippedGameModeNamesTheSameSoundBankTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShippedSoundBankParity;

	// ---- the match map ---------------------------------------------------------------------
	UStratSoundBank* const MatchBank = MatchModeBankOrFail(*this, kMatchGameModeClassPath);
	if (!TestNotNull(
			*FString::Printf(
				TEXT("GATE-AUDIO: '%s' configures a sound bank on MatchConfig.SoundBank. NULL IS "
				     "A LEGITIMATE CONFIGURATION AT RUNTIME -- the match simply runs silent and "
				     "every emission records NoBank -- which is exactly why it must be a failure "
				     "HERE: the shipped game is meant to have sound, and nothing else in this "
				     "tree distinguishes 'deliberately silent' from 'the pointer was lost'. This "
				     "is also the null control for the equality below, which three nulls would "
				     "otherwise satisfy. Fix: set MatchConfig.SoundBank on "
				     "Content/StratPlay/BP_StratGameMode.uasset (strat-editor-builder's lane)."),
				kMatchGameModeClassPath),
			MatchBank))
	{
		return false;
	}

	// ---- the AI-versus-AI mode -------------------------------------------------------------
	UStratSoundBank* const AiVsAiBank = MatchModeBankOrFail(*this, kAiVsAiGameModeClassPath);
	if (!TestNotNull(
			*FString::Printf(
				TEXT("GATE-AUDIO: '%s' configures a sound bank too. This asset is a THIRD SHIPPED "
				     "GAMEMODE and not a control -- it starts real matches whose cues are as "
				     "audible as the shipped mode's -- so a lost pointer here is a silent demo "
				     "and not a test-only concern."),
				kAiVsAiGameModeClassPath),
			AiVsAiBank))
	{
		return false;
	}

	// ---- the title map ---------------------------------------------------------------------
	UClass* const ShellClass =
		ResolveGeneratedClassOrFail(*this, AStratShellGameMode::StaticClass(),
		                            kShellGameModeClassPath);
	if (ShellClass == nullptr)
	{
		return false;
	}

	const AStratShellGameMode* const ShellCdo =
		ShellClass->GetDefaultObject<AStratShellGameMode>();
	if (!TestNotNull(
			TEXT("GATE-AUDIO: the title GameMode Blueprint's class default object exists -- the "
			     "object carrying the defaults a spawned shell GameMode starts from"),
			ShellCdo))
	{
		return false;
	}

	UStratSoundBank* const ShellBank = ShellCdo->SoundBank.Get();
	if (!TestNotNull(
			*FString::Printf(
				TEXT("GATE-AUDIO: '%s' configures a sound bank on its own SoundBank property. The "
				     "title map starts no match, so FStratMatchConfig::SoundBank cannot reach it "
				     "and this SEPARATE property is the only thing that can make WBP_TitleMenu's "
				     "buttons click -- StratShellGameMode.h states that in terms. Fix: set "
				     "SoundBank on Content/StratPlay/BP_StratShellGameMode.uasset."),
				kShellGameModeClassPath),
			ShellBank))
	{
		return false;
	}

	// ---- THE CLAIM: one bank, three assets -------------------------------------------------
	TestTrue(
		*FString::Printf(
			TEXT("GATE-AUDIO, THE CLAIM: the match map's GameMode and the TITLE map's GameMode "
			     "point at the SAME UStratSoundBank object -- match '%s' against shell '%s'. "
			     "These are two DIFFERENT C++ properties on two DIFFERENT classes "
			     "(FStratMatchConfig::SoundBank and AStratShellGameMode::SoundBank), set by hand "
			     "on two assets, and unlike SaveSlotName there is NO C++ author either could be "
			     "derived from -- so nothing but this line can notice when they diverge. A RED "
			     "HERE MEANS THE TITLE SCREEN AND THE MATCH SCREEN NOW SOUND DIFFERENT, and "
			     "neither is an error state the game will report: a bank that is merely "
			     "DIFFERENT is as valid at runtime as the right one. The fix is a content "
			     "decision about which bank is intended (strat-editor-builder's lane), not a "
			     "change to this test."),
			*Describe(MatchBank), *Describe(ShellBank)),
		MatchBank == ShellBank);

	TestTrue(
		*FString::Printf(
			TEXT("GATE-AUDIO, THE SAME CLAIM ACROSS THE THIRD ASSET: BP_StratGameMode_AiVsAi "
			     "points at that same bank -- match '%s' against AI-vs-AI '%s'. Asserted "
			     "separately from the shell so a failure names WHICH of the three drifted."),
			*Describe(MatchBank), *Describe(AiVsAiBank)),
		MatchBank == AiVsAiBank);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- both shipped GameMode CLASSES author their sound bank in the asset, and do not
// inherit it from C++.
//
// A POSITIVE CLAIM ABOUT OWNERSHIP, and it is the MIRROR of
// `T-FAME-02.ShippedGameModeInheritsDifficultyFromCpp` rather than a copy of it. That clause
// asserts a Blueprint CDO compares IDENTICAL to its native archetype on a property and concludes
// the value is inherited; this one asserts NON-IDENTICAL and concludes the value is authored.
// Same instrument, opposite answer, and the opposite answer is the one this subject needs.
//
// WHY THE CLAUSE ABOVE IS NOT ENOUGH, AND THIS IS THE WHOLE REASON THIS EXISTS. A CDO read is an
// EFFECTIVE read. `TObjectPtr<UStratSoundBank>` initialises to null in C++ on both classes, so
// TODAY the clause above's non-null assertions do rule out a pure C++ answer. But that is a
// coincidence of the current initialisers, not a property of the instrument: give either property
// a non-null C++ default tomorrow -- a `ConstructorHelpers` lookup, a `CreateDefaultSubobject`,
// anything -- and "all three equal and non-null" would be satisfied by three objects NO ASSET
// EVER SET, while the Blueprints had quietly been cleared. This project has a name for that
// failure, "a default cannot signal unset", and a memory file about it. This clause is the one
// that cannot be fooled that way, because it does not read a value at all: it asks the engine's
// own comparator whether the asset carries a DELTA against its parent.
//
// THE ARCHETYPE IS THE NATIVE CLASS DEFAULT, AND THAT IS ASSERTED RATHER THAN ASSUMED. Each
// generated class's super must be the native class itself. If a content pass ever interposed an
// intermediate Blueprint, "authored" would still be true and "against the C++ archetype" would
// have quietly stopped being, and this clause would be measuring a delta against another asset.
//
// THE CONTROLS, and the first runs on the property being pinned:
//
//   1. THE COMPARATOR CAN SAY "IDENTICAL" ON THIS PROPERTY, AGAINST THIS ARCHETYPE. The clause
//      takes a copy of the shipped container, asserts the copy still reports a difference exactly
//      as the original does, then clears ONLY the bank pointer on the copy and asserts the same
//      comparison now reports IDENTICAL. So the exact call whose "different" answer carries this
//      claim is demonstrated, on this property and against this archetype, to be capable of
//      answering "identical". An `Identical_InContainer` that had gone inert -- always false,
//      wrong property, wrong offset -- fails here. Note the direction: this control clears the
//      copy to the archetype's OWN value, read off the archetype rather than written as `nullptr`,
//      so it stays valid if the C++ default ever stops being null.
//   2. THE TWO OBJECTS ARE NOT THE SAME OBJECT. A CDO compared against itself reports "identical"
//      to everything, which would make control 1 fail loudly -- but it is asserted anyway,
//      because the failure it would otherwise produce would read as a content problem.
//
// WHAT IT DOES NOT PIN. Not WHICH bank -- that is the clause above and the three below. Not that
// a designer set the property in the editor as opposed to a commandlet; a delta is a delta. And
// note the ASYMMETRY with the difficulty clause it mirrors: that one cannot distinguish "never
// touched" from "set to the parent's value", a limit of asserting IDENTICAL. This one asserts
// NON-IDENTICAL and has no such gap -- a delta cannot exist without something having written it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShippedGameModesAuthorTheirSoundBankTest,
	"Stratocracy.StratPlay.GATE-AUDIO.ShippedGameModesAuthorTheirSoundBank",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShippedGameModesAuthorTheirSoundBankTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShippedSoundBankParity;

	// =========================================================================================
	// PART ONE -- the match map's GameMode, where the property lives INSIDE FStratMatchConfig.
	// =========================================================================================
	{
		UClass* const Generated =
			ResolveGeneratedClassOrFail(*this, AStratGameMode::StaticClass(),
			                            kMatchGameModeClassPath);
		if (Generated == nullptr)
		{
			return false;
		}

		UClass* const Super = Generated->GetSuperClass();
		if (!TestTrue(
				*FString::Printf(
					TEXT("GATE-AUDIO PREMISE: BP_StratGameMode's parent class is the NATIVE "
					     "AStratGameMode (read: '%s'). This clause claims the bank is authored "
					     "AGAINST THE C++ ARCHETYPE, so the archetype it compares against has to "
					     "BE C++. An interposed Blueprint base would make this a measurement "
					     "against another asset, and the claim would be wearing a name it no "
					     "longer earns."),
					Super != nullptr ? *Super->GetName() : TEXT("<null>")),
				Super == AStratGameMode::StaticClass()))
		{
			return false;
		}

		const AStratGameMode* const Shipped = Generated->GetDefaultObject<AStratGameMode>();
		const AStratGameMode* const Native = Super->GetDefaultObject<AStratGameMode>();
		if (!TestNotNull(TEXT("GATE-AUDIO: BP_StratGameMode's class default object"), Shipped) ||
			!TestNotNull(
				TEXT("GATE-AUDIO: the native AStratGameMode class default object -- the archetype "
				     "the shipped Blueprint's class default is a delta against"),
				Native))
		{
			return false;
		}

		// ---- CONTROL 2: two different objects ---------------------------------------------
		if (!TestTrue(
				TEXT("GATE-AUDIO CONTROL: the shipped CDO and the native CDO are two DIFFERENT "
				     "objects. If they were one object every comparison below is a value against "
				     "itself and would report 'identical' regardless of what the asset says."),
				Shipped != Native))
		{
			return false;
		}

		// ---- the reflected handles ---------------------------------------------------------
		FProperty* const MatchConfigProp =
			AStratGameMode::StaticClass()->FindPropertyByName(TEXT("MatchConfig"));
		if (!TestNotNull(
				TEXT("GATE-AUDIO: AStratGameMode reflects a property named 'MatchConfig'. This "
				     "clause reaches the field through reflection because that is what can answer "
				     "'does this carry a delta'; a renamed property must fail here rather than "
				     "silently comparing nothing."),
				MatchConfigProp))
		{
			return false;
		}

		FProperty* const BankProp =
			FStratMatchConfig::StaticStruct()->FindPropertyByName(TEXT("SoundBank"));
		if (!TestNotNull(
				TEXT("GATE-AUDIO: FStratMatchConfig reflects a member named 'SoundBank' -- the "
				     "property whose authorship is this clause's entire subject"),
				BankProp))
		{
			return false;
		}

		const void* const ShippedStruct = MatchConfigProp->ContainerPtrToValuePtr<void>(Shipped);
		const void* const NativeStruct = MatchConfigProp->ContainerPtrToValuePtr<void>(Native);

		// ---- the reflected property IS the C++ member the game reads -----------------------
		//
		// Without this, a correct-looking comparison could be running over some other property
		// and every answer below would be about a field nobody uses.
		TestTrue(
			*FString::Printf(
				TEXT("GATE-AUDIO: the reflected 'SoundBank' property this clause compares "
				     "OCCUPIES THE SAME STORAGE as the typed C++ member "
				     "Shipped->MatchConfig.SoundBank (typed value: '%s'). Asserted by address "
				     "rather than by an exported string, because an address comparison cannot be "
				     "satisfied by two fields that happen to print alike. If this fails the "
				     "comparisons below are being run over a different field than the one "
				     "StartMatchInternal hands to the director."),
				*Describe(Shipped->MatchConfig.SoundBank.Get())),
			BankProp->ContainerPtrToValuePtr<void>(ShippedStruct) ==
				static_cast<const void*>(&Shipped->MatchConfig.SoundBank));

		// ---- CONTROL 1: the comparator can say "identical" ON THIS PROPERTY ----------------
		{
			FStratMatchConfig Copy = Shipped->MatchConfig;

			// DELIBERATELY NOT FATAL, and the reason was measured rather than reasoned.
			// Written as `if (!TestFalse(...)) { return false; }` this line fires FIRST on the
			// mutant that matters -- clearing MatchConfig.SoundBank on BP_StratGameMode, which
			// is the real defect this whole clause exists against -- and the clause then stops
			// with "the copy is not faithful", which is a false diagnosis of a true failure.
			// Non-fatal, the same input leaves control 1b GREEN (a copy cleared to the
			// archetype's own null does compare identical) and reddens THE CLAIM, whose message
			// names the actual fault. A control whose failure message misdiagnoses the most
			// likely cause is worse than no message, so this one says both.
			TestFalse(
				TEXT("GATE-AUDIO CONTROL 1a: a copy of the shipped CDO's MatchConfig reports a "
				     "DIFFERENCE against the archetype on SoundBank, exactly as the original "
				     "does. TWO THINGS MAKE THIS RED AND THEY ARE NOT THE SAME FAULT: either the "
				     "copy is unfaithful, in which case control 1b below proves nothing -- or the "
				     "shipped asset has STOPPED AUTHORING A BANK, which is this clause's whole "
				     "subject. THE CLAIM below is what tells you which; read its message first "
				     "and treat this line as an echo if the claim is also red."),
				BankProp->Identical_InContainer(&Copy, NativeStruct));

			// The archetype's OWN value, read rather than written as `nullptr`: this control
			// stays valid on the day the C++ default stops being null, which is precisely the
			// day the clause above stops being able to see authorship on its own.
			Copy.SoundBank = Native->MatchConfig.SoundBank;

			TestTrue(
				*FString::Printf(
					TEXT("GATE-AUDIO CONTROL 1b, AND IT IS THE ONE THAT MAKES THIS CLAUSE MORE "
					     "THAN AN EQUALITY: set ONLY SoundBank on that copy back to the "
					     "archetype's own value ('%s') and the same comparison against the same "
					     "archetype must now report IDENTICAL. This proves the instrument is live "
					     "ON THE FIELD BEING PINNED. A red here means Identical_InContainer is "
					     "answering 'different' regardless of the data -- wrong property, wrong "
					     "offset, or an inert comparator -- and the claim below would have passed "
					     "for the same reason it always would."),
					*Describe(Native->MatchConfig.SoundBank.Get())),
				BankProp->Identical_InContainer(&Copy, NativeStruct));
		}

		// ---- THE CLAIM -------------------------------------------------------------------
		TestFalse(
			*FString::Printf(
				TEXT("GATE-AUDIO, THE CLAIM (match map): BP_StratGameMode CONTRIBUTES a value for "
				     "MatchConfig.SoundBank -- its class default carries a DELTA against the "
				     "native AStratGameMode archetype on this property (shipped '%s' against "
				     "archetype '%s'). So the bank the shipped match plays comes from the ASSET "
				     "and not from a C++ initialiser, which is the fact "
				     "EveryShippedGameModeNamesTheSameSoundBank's non-null assertions can only "
				     "infer while the C++ default happens to be null. A RED HERE MEANS THE "
				     "BLUEPRINT NO LONGER AUTHORS A BANK: either the property was cleared in the "
				     "asset (strat-editor-builder's lane) or a C++ default was introduced that "
				     "matches it (strat-gameplay-engineer's). It is not a change to this test."),
				*Describe(Shipped->MatchConfig.SoundBank.Get()),
				*Describe(Native->MatchConfig.SoundBank.Get())),
			BankProp->Identical_InContainer(ShippedStruct, NativeStruct));
	}

	// =========================================================================================
	// PART TWO -- the title map's GameMode, where `SoundBank` is a property ON THE CLASS.
	//
	// A SEPARATE MEASUREMENT AND NOT A REPETITION: this is a different `FProperty` on a
	// different `UClass`, and the only thing the two have in common is the type they point at.
	// The drift both C++ headers describe is exactly the gap between these two parts.
	// =========================================================================================
	{
		UClass* const Generated =
			ResolveGeneratedClassOrFail(*this, AStratShellGameMode::StaticClass(),
			                            kShellGameModeClassPath);
		if (Generated == nullptr)
		{
			return false;
		}

		UClass* const Super = Generated->GetSuperClass();
		if (!TestTrue(
				*FString::Printf(
					TEXT("GATE-AUDIO PREMISE: BP_StratShellGameMode's parent class is the NATIVE "
					     "AStratShellGameMode (read: '%s'), for the same reason part one gives."),
					Super != nullptr ? *Super->GetName() : TEXT("<null>")),
				Super == AStratShellGameMode::StaticClass()))
		{
			return false;
		}

		const AStratShellGameMode* const Shipped =
			Generated->GetDefaultObject<AStratShellGameMode>();
		const AStratShellGameMode* const Native = Super->GetDefaultObject<AStratShellGameMode>();
		if (!TestNotNull(TEXT("GATE-AUDIO: BP_StratShellGameMode's class default object"),
				Shipped) ||
			!TestNotNull(TEXT("GATE-AUDIO: the native AStratShellGameMode class default object"),
				Native))
		{
			return false;
		}

		if (!TestTrue(
				TEXT("GATE-AUDIO CONTROL: the shipped shell CDO and the native shell CDO are two "
				     "DIFFERENT objects"),
				Shipped != Native))
		{
			return false;
		}

		FProperty* const BankProp =
			AStratShellGameMode::StaticClass()->FindPropertyByName(TEXT("SoundBank"));
		if (!TestNotNull(
				TEXT("GATE-AUDIO: AStratShellGameMode reflects a property named 'SoundBank' -- "
				     "the TITLE MAP's own property, which is a different FProperty on a different "
				     "UClass from FStratMatchConfig's and is the reason the two can drift at all"),
				BankProp))
		{
			return false;
		}

		// ---- the reflected property IS the C++ member --------------------------------------
		TestTrue(
			*FString::Printf(
				TEXT("GATE-AUDIO: the reflected 'SoundBank' property names the same object as the "
				     "typed C++ member ShellCdo->SoundBank ('%s'). If these disagree the "
				     "comparison below runs over a field BeginPlay never reads."),
				*Describe(Shipped->SoundBank.Get())),
			BankProp->ContainerPtrToValuePtr<void>(Shipped) ==
				static_cast<const void*>(&Shipped->SoundBank));

		// ---- CONTROL 1: the comparator can say "identical" ---------------------------------
		//
		// The shell's property is on the CLASS, so there is no struct copy to move. The control
		// is run by comparing the NATIVE CDO against ITSELF on this property -- a case whose
		// answer must be "identical" by construction, and which therefore proves the comparator
		// is capable of that answer on this exact FProperty.
		if (!TestTrue(
				TEXT("GATE-AUDIO CONTROL 1: the same Identical_InContainer call on the same "
				     "FProperty reports IDENTICAL when handed the native CDO on both sides. This "
				     "proves the comparator is capable of answering 'identical' here, so the "
				     "'different' answer the claim below rests on is a fact about the data rather "
				     "than an inert comparator that always says 'different'."),
				BankProp->Identical_InContainer(Native, Native)))
		{
			return false;
		}

		// ---- THE CLAIM -------------------------------------------------------------------
		TestFalse(
			*FString::Printf(
				TEXT("GATE-AUDIO, THE CLAIM (title map): BP_StratShellGameMode CONTRIBUTES a "
				     "value for its own SoundBank property -- a DELTA against the native "
				     "AStratShellGameMode archetype (shipped '%s' against archetype '%s'). This "
				     "is a SEPARATE fact from part one's, on a separate property of a separate "
				     "class, and it is the half StratShellGameMode.h calls 'strictly worse' than "
				     "SaveSlotName's drift: there is no C++ author to derive from and the failure "
				     "is not even a wrong restore, it is a title screen whose buttons do not "
				     "click."),
				*Describe(Shipped->SoundBank.Get()), *Describe(Native->SoundBank.Get())),
			BankProp->Identical_InContainer(Shipped, Native));
	}

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- every cue in the shipped bank has a sound.
//
// THE CHEAPEST CLAUSE IN THIS FILE AND THE ONE THAT NAMES THE FAULT MOST PRECISELY. A bank with
// one empty slot makes exactly one cue silent. `UStratSoundBank`'s header says that is a
// legitimate authoring state -- *"EVERY SLOT MAY BE NULL AND A NULL SLOT IS A CONFIGURATION,
// NEVER A FAULT"* -- and it is right about the CLASS. It is not right about the SHIPPED bank,
// which is meant to be finished, and nothing in this tree could tell the difference until now:
// the director records `NoSoundConfigured`, logs nothing, refuses nothing, and the game plays on.
//
// THE CUE LIST IS READ OFF THE REFLECTED `UENUM` AND NOT WRITTEN HERE. A hand-written list of
// seven would be this file deciding what the cue set is, and would stop covering an eighth cue
// the day one is added -- the same shape of gap this whole file exists to close. `SoundFor` is
// the module-side accessor and is what the director itself calls, so this clause asks the exact
// question `EmitCue` asks.
//
// THE BANK IS READ OFF THE SHIPPED GAMEMODE'S CDO AND NEVER LOADED BY PATH. So this clause is
// about THE BANK THE GAME USES, not about an asset that merely shares its name.
//
// THE CONTROLS. The cue list must be non-empty -- an empty loop passes every assertion inside it
// and would report a bank with no sounds at all as clean. And the enum's own count is asserted
// to exceed one, so a reflected enum that had collapsed to a single sentinel is visible.
//
// WHY IT IS NOT REDUNDANT WITH `EveryCueInTheShippedBankReachesTheEngine` BELOW, which would also
// go red on a null slot. This one runs with no world, no subsystem and no engine call, so it
// stays green-or-red on the DATA alone; the one below can be reddened by a fixture problem that
// has nothing to do with the bank. When both fail, this is the one that says what to fix.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratEveryCueInTheShippedBankHasASoundTest,
	"Stratocracy.StratPlay.GATE-AUDIO.EveryCueInTheShippedBankHasASound",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratEveryCueInTheShippedBankHasASoundTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShippedSoundBankParity;

	UStratSoundBank* const Bank = MatchModeBankOrFail(*this, kMatchGameModeClassPath);
	if (!TestNotNull(
			TEXT("GATE-AUDIO: the shipped match GameMode configures a sound bank. This clause's "
			     "subject is what is INSIDE that bank, so it cannot conclude anything without "
			     "one -- EveryShippedGameModeNamesTheSameSoundBank is the clause that pins the "
			     "pointer itself."),
			Bank))
	{
		return false;
	}

	const TArray<EStratSoundCue> Cues = AllCues();

	if (!TestTrue(
			*FString::Printf(
				TEXT("GATE-AUDIO CONTROL: StaticEnum<EStratSoundCue>() reports at least one cue "
				     "(read: %d). The loop below asserts once per cue, so an EMPTY list would "
				     "pass this clause while checking nothing -- a bank with every slot cleared "
				     "would read as clean. This is the assertion that stops that."),
				Cues.Num()),
			Cues.Num() > 1))
	{
		return false;
	}

	for (const EStratSoundCue Cue : Cues)
	{
		TestNotNull(
			*FString::Printf(
				TEXT("GATE-AUDIO: the shipped bank '%s' has a sound for cue '%s'. A NULL SLOT IS "
				     "NOT AN ERROR ANYWHERE ELSE IN THIS PROJECT -- UStratSoundBank refuses "
				     "nothing, UStratSoundDirector records NoSoundConfigured and logs nothing, "
				     "and the game plays on -- so this line is the only thing between a cleared "
				     "slot and exactly one permanently silent cue. Asked through "
				     "UStratSoundBank::SoundFor, which is the same accessor EmitCue calls, so a "
				     "green here is a statement about the code path the game runs. Fix: fill the "
				     "slot on Content/StratAudio/DA_StratSoundBank.uasset "
				     "(strat-editor-builder's lane)."),
				*Describe(Bank), *CueWord(Cue)),
			Bank->SoundFor(Cue));
	}

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- every cue in the shipped bank carries the SFX sound class.
//
// WHAT A RED HERE COSTS. A `USoundBase` whose `SoundClassObject` is null or is some other class
// sits OUTSIDE the mix tree `SCL_Strat_Master` roots. It routes to the engine's default submix
// rather than `SBM_Strat_SFX`, and the volume control the audio milestone still owes -- which
// will hang on a sound class, because that is what a sound class is for -- will not move it. The
// cue stays audible, at the wrong level, forever, and every instrument in this tree reports the
// bank as complete because the slot is filled. That is the exact failure this clause exists for,
// and it is invisible to all four of the others.
//
// WHERE THE EXPECTATION COMES FROM, AND IT IS THE ONE ASSET-PATH LITERAL IN THIS FILE.
// `UStratSoundBank` carries no sound-class property; nothing in `Source/` names a sound class at
// all, by design, because the mix tree is content. So there is NO module-side value to read for
// "which class", and the expectation is the named asset itself -- loaded as an object and
// compared by IDENTITY, not by string, so a renamed or duplicated class is red rather than a
// compare that quietly passes. This is `CLAUDE.md` rule 4's automation-fixture exception and the
// same posture `StratShippedGameModeMatchConfig.cpp` states for its own literals.
//
// TWO ASSERTIONS, AND THE FIRST NEEDS NO LITERAL AT ALL. Before comparing anything to
// `SCL_Strat_SFX`, the clause asserts all seven cues carry ONE AND THE SAME class as each other,
// read off the first cue. That half is pure parity with no expectation and would survive a
// wholesale rename of the mix tree; the literal half is what stops all seven drifting together.
// Neither is sufficient alone and the file header records why.
//
// THE CONTROL, and it is a set-difference control rather than a liveness remark: the SAME reader
// is run over `SCL_Strat_Music`, which exists and is a `USoundClass` and is NOT what cues carry.
// A comparison that had gone inert -- comparing a null against a null, or an object against
// itself -- would report the music class as a match too, and does not.
//
// WHAT IT DOES NOT PIN. Not the tree ABOVE `SCL_Strat_SFX`: that it parents to
// `SCL_Strat_Master`, that either names `SBM_Strat_SFX` as its default submix, or that anything
// routes to `SMX_Strat_Base`. Each is a separate claim about a separate asset property and none
// is asserted here. Not any volume, and not that a future volume control will reach these cues --
// only that they are all in one place a control COULD reach.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratEveryShippedCueSoundCarriesTheSfxClassTest,
	"Stratocracy.StratPlay.GATE-AUDIO.EveryShippedCueSoundCarriesTheSfxClass",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratEveryShippedCueSoundCarriesTheSfxClassTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShippedSoundBankParity;

	UStratSoundBank* const Bank = MatchModeBankOrFail(*this, kMatchGameModeClassPath);
	if (!TestNotNull(
			TEXT("GATE-AUDIO: the shipped match GameMode configures a sound bank, without which "
			     "this clause has no sounds to read a class off"),
			Bank))
	{
		return false;
	}

	USoundClass* const SfxClass = LoadObject<USoundClass>(nullptr, kSfxSoundClassPath);
	if (!TestNotNull(
			*FString::Printf(
				TEXT("GATE-AUDIO: the SFX sound class asset loads from '%s'. If this failed the "
				     "class was renamed, moved or deleted -- and this clause CANNOT conclude "
				     "anything about which class the cues carry, so it FAILS rather than "
				     "passing. Fix the asset or fix this path; do not soften this into a skip."),
				kSfxSoundClassPath),
			SfxClass))
	{
		return false;
	}

	// ---- THE CONTROL: a second class exists and is NOT the one cues carry ------------------
	USoundClass* const MusicClass = LoadObject<USoundClass>(nullptr, kMusicSoundClassPath);
	if (!TestNotNull(
			*FString::Printf(
				TEXT("GATE-AUDIO CONTROL: the music sound class loads from '%s'. It is this "
				     "clause's set-difference control -- an identity comparison that had gone "
				     "inert would match it too -- so its absence makes the assertions below "
				     "unwarranted rather than merely uncontrolled."),
				kMusicSoundClassPath),
			MusicClass))
	{
		return false;
	}

	if (!TestTrue(
			TEXT("GATE-AUDIO CONTROL: the SFX class and the music class are two DIFFERENT "
			     "objects. If a rename had collapsed them the control below would agree with "
			     "the claim and prove nothing."),
			SfxClass != MusicClass))
	{
		return false;
	}

	const TArray<EStratSoundCue> Cues = AllCues();
	if (!TestTrue(
			*FString::Printf(
				TEXT("GATE-AUDIO CONTROL: StaticEnum<EStratSoundCue>() reports more than one cue "
				     "(read: %d), so the loop below is not vacuous"),
				Cues.Num()),
			Cues.Num() > 1))
	{
		return false;
	}

	// ---- PARITY, WITH NO EXPECTATION: all cues share ONE class ------------------------------
	//
	// Read off the first cue that has a sound, not written here. This half survives a wholesale
	// rename of the mix tree and would catch exactly one cue drifting out of it.
	USoundClass* FirstSeenClass = nullptr;
	EStratSoundCue FirstSeenCue = Cues[0];
	bool bFirstSeen = false;

	for (const EStratSoundCue Cue : Cues)
	{
		USoundBase* const Sound = Bank->SoundFor(Cue);
		if (!TestNotNull(
				*FString::Printf(
					TEXT("GATE-AUDIO: cue '%s' has a sound in the shipped bank, without which "
					     "this clause cannot read a sound class off it. "
					     "EveryCueInTheShippedBankHasASound is the clause that pins this on its "
					     "own; it is repeated here so a null slot does not surface as a "
					     "confusing null-class failure."),
					*CueWord(Cue)),
				Sound))
		{
			continue;
		}

		USoundClass* const Carried = Sound->SoundClassObject;

		// ---- THE CLAIM, half one: the named class ------------------------------------------
		TestTrue(
			*FString::Printf(
				TEXT("GATE-AUDIO, THE CLAIM: cue '%s' (sound '%s') carries SoundClassObject "
				     "'%s', which must be the SFX class '%s'. A cue OUTSIDE this class escapes "
				     "the mix tree SCL_Strat_Master roots: it routes to the engine's default "
				     "submix instead of SBM_Strat_SFX, and the volume control this milestone "
				     "still owes will not move it -- the cue stays audible at the wrong level "
				     "and every other clause in this file reports the bank as complete. A NULL "
				     "read here is the same fault and not a lesser one. Fix: set the Sound Class "
				     "on the MetaSoundSource asset (strat-editor-builder's lane)."),
				*CueWord(Cue), *Describe(Sound), *Describe(Carried), *Describe(SfxClass)),
			Carried == SfxClass);

		// ---- THE CONTROL, on this cue ------------------------------------------------------
		TestFalse(
			*FString::Printf(
				TEXT("GATE-AUDIO CONTROL, on cue '%s': the same read does NOT come back as the "
				     "music class '%s'. This is a set-difference control on the instrument -- a "
				     "comparison that matched everything, or a SoundClassObject read that had "
				     "gone inert and answered one fixed object, would agree with the claim above "
				     "AND with this line. Both cannot hold."),
				*CueWord(Cue), *Describe(MusicClass)),
			Carried == MusicClass);

		// ---- THE CLAIM, half two: parity, with no literal ----------------------------------
		if (!bFirstSeen)
		{
			FirstSeenClass = Carried;
			FirstSeenCue = Cue;
			bFirstSeen = true;
		}
		else
		{
			TestTrue(
				*FString::Printf(
					TEXT("GATE-AUDIO, PARITY WITH NO EXPECTATION: cue '%s' carries the same "
					     "sound class as cue '%s' -- '%s' against '%s'. This half of the claim "
					     "holds no asset path and would survive a wholesale rename of the mix "
					     "tree; what it catches is exactly ONE cue drifting out while the other "
					     "six stay put, which is the likeliest way this breaks."),
					*CueWord(Cue), *CueWord(FirstSeenCue), *Describe(Carried),
					*Describe(FirstSeenClass)),
				Carried == FirstSeenClass);
		}
	}

	TestTrue(
		TEXT("GATE-AUDIO CONTROL: at least one cue in the shipped bank had a sound, so the loop "
		     "above asserted something. Without this a bank with every slot cleared would reach "
		     "the end of this clause having made no claim at all."),
		bFirstSeen);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- every cue in the shipped bank reaches the engine.
//
// THE CLAUSE THAT EXERCISES THE PATH NOTHING IN THIS TREE HAD EVER RUN. Before this file,
// `EStratSoundDisposition::Played` appeared in `Source/` exactly once outside its own
// declaration -- in a display-only `switch` in a test helper. Every emission any clause had ever
// observed recorded `NoBank`, because every fixture built its own `FStratMatchConfig` with no
// bank. So `UStratSoundDirector::EmitCue`'s entire play arm -- the `SoundFor` lookup, the
// cooldown comparison, the `PlaySound2D` call, the `LastPlayedAtSeconds` write and the `Played`
// record -- was dead code as far as the suite was concerned, and would have stayed green if it
// had been deleted. This clause runs it, with the bank the shipped game actually uses.
//
// WHAT `Played` MEANS AND WHAT IT DOES NOT. It means `UGameplayStatics::PlaySound2D` was called
// and returned. `EStratSoundDisposition`'s own declaration says it in terms: *"NOT a claim that
// anything was audible -- under `-nullrhi` there is no device and the call still returns."* This
// clause therefore pins that the REQUEST REACHES THE ENGINE for every cue, which is the strongest
// claim any headless instrument in this project is entitled to make about sound. Whether a human
// hears seven distinct sounds is not observable here and is not claimed anywhere.
//
// WHY IT IS THE RIGHT SHAPE ANYWAY. `Played` is the ONLY disposition that is reachable solely by
// passing every gate in `EmitCue` in order: a world, a bank, a non-null slot, and no live
// cooldown. Each of the other four arms is a distinct named failure, so asserting `Played`
// asserts the conjunction and a failure message that names the arm reached says immediately which
// of the four is the fault.
//
// EACH CUE IS EMITTED EXACTLY ONCE, AND THAT IS DELIBERATE. `SuppressedByCooldown` compares
// against `UWorld::GetTimeSeconds`; this world is created with `bInformEngineOfWorld = false` and
// is never ticked, so the clock does not advance and a second emission of the same cue would
// measure the FIXTURE rather than the bank. `EmitCue`'s own comment records that. One emission
// per cue has an answer that does not depend on a clock -- `LastPlayedAtSeconds` has no entry, so
// the cooldown arm is not taken regardless of what `MinSecondsBetween` says. The cooldown arm
// stays unpinned by this file and is named as such in `Tools/architect/state/tests.md`.
//
// THE CONTROLS. The director must start with NO bank and an EMPTY record -- so the `Played`
// answers below are the result of the adoption this clause performs and not of a director that
// arrived configured. The adopted bank is asserted to be the object read off the CDO, so a
// silently-ignored `AdoptSoundBank` is visible. And the record count must equal the cue count, so
// an `EmitCue` that returned early before appending cannot leave a shorter list that the loop
// below never notices.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratEveryCueInTheShippedBankReachesTheEngineTest,
	"Stratocracy.StratPlay.GATE-AUDIO.EveryCueInTheShippedBankReachesTheEngine",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratEveryCueInTheShippedBankReachesTheEngineTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratShippedSoundBankParity;

	UStratSoundBank* const Bank = MatchModeBankOrFail(*this, kMatchGameModeClassPath);
	if (!TestNotNull(
			TEXT("GATE-AUDIO: the shipped match GameMode configures a sound bank -- the object "
			     "this clause adopts into a director. This is read off the CDO and never loaded "
			     "by path, so what runs below is the bank the game uses."),
			Bank))
	{
		return false;
	}

	const TArray<EStratSoundCue> Cues = AllCues();
	if (!TestTrue(
			*FString::Printf(
				TEXT("GATE-AUDIO CONTROL: StaticEnum<EStratSoundCue>() reports more than one cue "
				     "(read: %d), so the emissions below are not vacuous"),
				Cues.Num()),
			Cues.Num() > 1))
	{
		return false;
	}

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("GATE-AUDIO: a transient Game world was created"), Scope.World))
	{
		return false;
	}

	UStratSoundDirector* const Director = Scope.World->GetSubsystem<UStratSoundDirector>();
	if (!TestNotNull(
			TEXT("GATE-AUDIO: a Game world acquires a UStratSoundDirector"),
			Director))
	{
		return false;
	}

	// ---- CONTROLS: the director arrives unconfigured and empty ------------------------------
	if (!TestNull(
			TEXT("GATE-AUDIO CONTROL: the director starts with NO bank. Every Played answer "
			     "below must be the result of the adoption this clause performs; a director that "
			     "arrived already configured would make this clause a statement about the "
			     "subsystem's construction rather than about the shipped asset."),
			Director->GetSoundBank()))
	{
		return false;
	}

	if (!TestEqual(
			TEXT("GATE-AUDIO CONTROL: the director starts with an empty record, so the records "
			     "read below are the ones this clause caused"),
			Director->GetEmissions().Num(), 0))
	{
		return false;
	}

	Director->AdoptSoundBank(Bank);

	if (!TestTrue(
			*FString::Printf(
				TEXT("GATE-AUDIO CONTROL: AdoptSoundBank actually took the shipped bank (read: "
				     "'%s', expected '%s'). An adoption that was silently ignored would leave "
				     "every emission on NoBank, and the failures below would read as a content "
				     "problem when they were a code one."),
				*Describe(Director->GetSoundBank()), *Describe(Bank)),
			Director->GetSoundBank() == Bank))
	{
		return false;
	}

	// ---- ONE emission per cue, for the reason the header block gives ------------------------
	for (const EStratSoundCue Cue : Cues)
	{
		Director->EmitCue(Cue, /*Side*/ INDEX_NONE, /*UnitId*/ INDEX_NONE, /*Turn*/ 0);
	}

	if (!TestEqual(
			*FString::Printf(
				TEXT("GATE-AUDIO CONTROL: the director recorded one emission per cue (%d cues, "
				     "%d records). EmitCue RECORDS FIRST AND PLAYS SECOND, so a shorter list "
				     "means an early return before the append -- and the loop below would then "
				     "read fewer dispositions than it was asked about without noticing."),
				Cues.Num(), Director->GetEmissions().Num()),
			Director->GetEmissions().Num(), Cues.Num()))
	{
		return false;
	}

	// ---- THE CLAIM ---------------------------------------------------------------------------
	for (int32 Index = 0; Index < Cues.Num(); ++Index)
	{
		const FStratSoundEmissionRecord& Record = Director->GetEmissions()[Index];

		TestEqual(
			*FString::Printf(
				TEXT("GATE-AUDIO CONTROL: record %d is about the cue it was asked for ('%s')"),
				Index, *CueWord(Cues[Index])),
			static_cast<int32>(Record.Cue), static_cast<int32>(Cues[Index]));

		TestEqual(
			*FString::Printf(
				TEXT("GATE-AUDIO, THE CLAIM: cue '%s' emitted through a director holding the "
				     "SHIPPED bank records the disposition `Played` (read: '%s'). This is the "
				     "arm no clause in this project had ever executed -- before this file every "
				     "emission anywhere recorded NoBank, so EmitCue's whole play path was dead "
				     "code to the suite. WHAT Played MEANS: PlaySound2D was called and returned. "
				     "It is NOT a claim that anything was audible; under -nullrhi there is no "
				     "device. WHAT A RED MEANS, by the arm it names: `NoBank` -- the adoption "
				     "control above should have caught it, so suspect EmitCue; "
				     "`NoSoundConfigured` -- that cue's slot in DA_StratSoundBank is empty, and "
				     "EveryCueInTheShippedBankHasASound says which; `SuppressedByCooldown` -- "
				     "impossible on a first emission and means the cooldown arm's condition "
				     "changed; `NoWorld` -- the fixture, not the asset."),
				*CueWord(Cues[Index]),
				*DispositionWord(Record.Disposition)),
			static_cast<int32>(Record.Disposition),
			static_cast<int32>(EStratSoundDisposition::Played));
	}

	return true;
}
