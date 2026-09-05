// The title track's lifecycle on `AStratShellGameMode` -- spawned in `BeginPlay`, stopped and
// released in `EndPlay`. The local ID `GATE-TITLEMENU` is authorized for this subject by the
// 2026-09-05 ruling in `Tools/architect/state/global.md`: title music played and stopped by this
// class is *"the SAME class and the SAME lifecycle (`BeginPlay`/`EndPlay`) `GATE-TITLEMENU`
// already reasons about for the screen itself"*. It mints no acceptance ID and claims none.
//
// ---------------------------------------------------------------------------------------
// THE HALF THAT IS ACTUALLY REQUIRED IS `EndPlay`, AND `BeginPlay` IS THE EASY HALF.
// `SpawnSound2D(..., bAutoDestroy=false)` removes exactly ONE release and only that one: the
// ENGINE'S OWN, by which a finished component destroys itself and nulls the member. THAT IS NOT
// THE SAME CLAIM AS "nothing else could free the component", and this paragraph asserted the
// second one until 2026-09-05 -- the block directly below carries the retraction and the
// measurement that falsified it, and a reader who takes nothing else from here should take that
// the actor's own teardown WOULD reach this component. What `EndPlay` owes is therefore a stop
// and an EXPLICIT release of a handle that will not release itself on finishing; it is not sole
// custody of a component nothing else can touch.
// A track left playing outlives the map it belonged to: a player who left the title screen for a
// match would hear the menu music under the battle, with nothing in the match world able to
// explain where it was coming from.
//
// AND THE REASON THIS PARAGRAPH USED TO GIVE FOR THAT WAS FALSE, RETRACTED IN PLACE RATHER THAN
// QUIETLY REWORDED, BECAUSE IT TRAVELLED -- into `StratShellGameMode.h`, into this file's own
// assertion below, into a dispatch brief, AND INTO THE PARAGRAPH DIRECTLY ABOVE THIS ONE -- the
// one opening "THE HALF THAT IS ACTUALLY REQUIRED IS `EndPlay`". That is FOUR destinations, and
// this list named only the first three until 2026-09-05. The omitted fourth survived three gate
// rounds and was charged in the fourth, and the under-count is part of why: a reader auditing
// the spread against this list would tick off three sites and stop. An enumeration of where a
// false premise travelled is itself a claim, and this one was wrong in the direction that hides
// work. It read:
//   RETRACTED>  "`SpawnSound2D(..., bAutoDestroy=false)` hands this actor a component the AUDIO
//   RETRACTED>   DEVICE owns rather than this actor's component list: it is not in
//   RETRACTED>   `OwnedComponents`, so actor teardown does not touch it"
// FALSIFIED BY THIS FILE'S OWN CLAUSE, at the assertion near the bottom that measures it:
//
//   DIAG outer=StratShellGameMode_1 owner=StratShellGameMode_1 registered=1 ownedCount=1
//
// The component IS outered and owned to the spawning actor and IS in its component list, so
// actor teardown WOULD reach it. `StratShellGameMode.h` carries its own `RETRACTED>` block for
// the same sentence. WHAT IS UNCHANGED IS THE REQUIREMENT AND WHAT THIS CLAUSE OBSERVES:
// `bAutoDestroy=false` is still load-bearing on its own -- with the engine default the component
// would free itself and null the member, which reads identically to a correct release and is not
// one -- and `EndPlay` still owes the stop and the release rather than inheriting them from a
// teardown order it does not control.
//
// WHAT THIS CLAUSE OBSERVES, AND A MEASUREMENT THAT COST IT TWO SHAPES AND IS RECORDED SO THE
// NEXT FIXTURE IN THIS DIRECTORY DOES NOT REPEAT EITHER.
//
// SHAPE ONE SPAWNED THE ACTOR WITH `SpawnActorDeferred` + `FinishSpawningActor` AND ASSUMED
// `BeginPlay` HAD RUN. It went red with its own premise message on run `2026.09.05-15.08.52`
// ("BeginPlay spawned no audio component for a configured TitleMusic"), and the diagnosis
// written into shape two was WRONG: it blamed a missing audio device. Shape two therefore
// planted a handle by hand and asserted only the release. It ALSO went red -- and the diagnostic
// it carried is the finding:
//
//   DIAG UseSound=1 allowAudio=1 worldBegunPlay=0 actorBegunPlay=0 audioDevice=1
//
// THE AUDIO DEVICE WAS THERE ALL ALONG. `FinishSpawningActor` dispatches `BeginPlay` only when
// the WORLD has begun play, and a world built by `UWorld::CreateWorld` +
// `InitializeActorsForPlay` has not. So neither `BeginPlay` NOR `EndPlay` ever ran, in either
// shape: no `BeginPlay` meant no `SpawnSound2D`, and no begun-play actor meant `Destroy()`
// dispatched no `EndPlay` either. The fix is `AActor::DispatchBeginPlay()`, which
// `StratBuildPulseDrawClauses.cpp` was already using on its board actor.
//
// AND THE CONTROL THAT SHOULD HAVE CAUGHT SHAPE ONE DID NOT, WHICH IS THE PART WORTH CARRYING
// AWAY. Shape one inferred "BeginPlay ran" from an `AddExpectedMessagePlain(...,
// Occurrences = 1)` for the refusal `BeginPlay` logs -- reasoning that an unfulfilled
// expectation would have failed the clause. It did not: the run reported exactly the three
// assertion failures and no unfulfilled-expectation error, with the declaration never matched.
//
// THE CONCLUSION FIRST DRAWN FROM THAT WAS AN OVERCLAIM AND IS RETRACTED IN PLACE, because it
// travelled -- it was cited against a CORRECT sentence in `StratAudioVolumeClauses.cpp` and
// nearly cost that clause its explanation. It read:
//   RETRACTED>  "**An `AddExpectedMessagePlain` is not a positive control that the line was
//   RETRACTED>   emitted** in this engine, whatever its `Occurrences` argument says, and any
//   RETRACTED>   clause in this tree using one as evidence that code RAN is resting on nothing"
// THAT GENERALISED FROM ONE RUN AND TWO OTHER CLAUSES IN THIS DIRECTORY HAD ALREADY MEASURED
// THE OPPOSITE: `StratGuidanceRouteClauses.cpp` records a clause turned RED by an unfulfilled
// declaration ("Expected suppressed ('Warning') level log message or higher matching 'no tile
// mesh for terrain' did not occur"), and `StratHotSeatReplayParity.cpp` records the same for
// `STRAT-CMD refused`. Both are true, and so is shape one's silence, because the mechanism is
// CONDITIONAL rather than absent. Read out of UE 5.8's
// `Runtime/Core/Private/Misc/AutomationTest.cpp` rather than taken on report:
//
//   FAutomationTestFramework::InternalStopTest --
//     bTestSuccessful = bTestSuccessful && !CurrentTest->HasAnyErrors()
//                                       && CurrentTest->HasMetExpectedMessages();
//
// `HasMetExpectedMessages` is the ONLY thing that emits the unfulfilled-expectation error, and
// `&&` SHORT-CIRCUITS. So on a clause that is already failing -- which shape one was, with three
// assertion failures -- the check is never called and an unfulfilled declaration is silently
// dropped. On an otherwise-passing clause it is called, and
// `ExpectedNumberOfOccurrences > 0 && Expected != Actual` raises an error that reddens the run.
//
// THE RULE AT THE WIDTH IT WAS MEASURED AT, therefore: **an `AddExpectedMessagePlain` with
// `Occurrences >= 1` IS an assertion that the line fired -- on a clause that would otherwise
// pass. It is NOT a diagnostic you can read on a run that is already red**, and that is the
// trap shape one fell into: it used the declaration to explain a FAILING run, which is the one
// state in which the engine does not check it. A clause using one as its positive control is
// resting on something real; a shape-one-style inference from a red run is not. This clause
// asserts `HasActorBegunPlay()` regardless, which is the engine's own answer, needs no
// otherwise-passing precondition, and cannot be satisfied by silence.
//
// WHAT IS PINNED, THEREFORE: the whole handle lifecycle, for real. `BeginPlay` spawns nothing
// when no track is configured; spawns a non-self-destroying handle playing the configured track
// when one is; and `EndPlay` -- reached by destroying the actor -- stops it, destroys it and
// nulls the member. Nothing audible is asserted and nothing could be: under `-nullrhi` no sound
// is rendered, and the component's existence is the whole observation.
//
// AND ONE OF THE CLASS HEADER'S OWN PREMISES IS FALSIFIED IN PASSING, which is recorded at the
// assertion that measures it rather than only here: the spawned component IS owned by the
// spawning actor. See "A MEASURED CONTRADICTION" below.
//
// THE ACTOR IS SPAWNED DEFERRED, WHICH IS NOT STYLE. `TitleMusic` is an `EditDefaultsOnly`
// property the shipped Blueprint sets, and `BeginPlay` reads it, so the fixture must be able to
// assign it between construction and begin-play in order to choose which path it exercises.
//
// THE ERROR THIS FIXTURE PROVOKES IS DECLARED, AND DECLARING IT IS ALL THAT DOES. A world
// created by `UWorld::CreateWorld` has no `UGameInstance`, so `BeginPlay`'s first refusal fires
// and logs `No shell this session: this GameMode has no game instance` -- twice, once per shell
// below. The declaration keeps the clause from being reported as SucceededWithWarnings, AND --
// per the corrected reading above, not the retracted one -- with `Occurrences = 2` it does also
// assert that both refusals fired, since this clause is expected to pass. What it cannot do is
// speak on a run that is ALREADY red, which is exactly when shape one asked it to. The control
// that does not care about that is `HasActorBegunPlay()`.
//
// WHERE THE EXPECTATIONS COME FROM. There is no duration, no gain and no asset name asserted.
// The track handed to the fixture is a shipped asset by path -- the sanctioned test-fixture
// exception to the no-`/Game/`-literals rule -- and the only thing asserted about it is that the
// same object the fixture set is the one the component is playing, read off the component.
//
// WHAT IT DOES NOT PIN.
//   - THAT THE SHIPPED `BP_StratShellGameMode` SETS `TitleMusic`. That is a Blueprint default
//     and a content fact; a shipped-asset clause for it belongs with the other
//     `StratShippedSoundBankParity.cpp` parities, after the asset pass sets it.
//   - THAT THE TRACK'S SOUND CLASS IS THE BANK'S `MusicSoundClass`. `AStratShellGameMode`'s own
//     header records that nothing in C++ checks that pairing and names the clause that
//     discharges it, once both assets carry it. Not this file's, and not written here.
//   - THAT THE MUSIC DOES NOT SURVIVE A LEVEL TRANSITION. `bPersistAcrossLevelTransition` is
//     false and a headless fixture does not travel. What is pinned -- that `EndPlay` releases
//     the handle -- is the mechanism that would make it moot either way.
// ---------------------------------------------------------------------------------------

#include "Misc/AutomationTest.h"

#include "Components/AudioComponent.h"
#include "Containers/UnrealString.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "UObject/UObjectGlobals.h"

#include "StratAudioSettings.h"
#include "StratShellGameMode.h"
#include "StratSoundDirector.h"

namespace StratTitleMusicFixture
{
	/** A shipped sound asset, AS FIXTURE INPUT ONLY. A `/Game/` literal in a test is one of the
	 *  two sanctioned exceptions; nothing below asserts anything about this asset beyond its
	 *  identity with what the component reports playing. */
	const TCHAR* const kTrackPath =
		TEXT("/Game/StratAudio/MS_Strat_ButtonClick.MS_Strat_ButtonClick");

	/** See `StratSoundDirectorCallSite.cpp` on why `InitializeActorsForPlay` is not decoration
	 *  and why the world is destroyed before its context. Duplicated rather than shared: a
	 *  shared helper would put a header inside a `Tests/` directory. */
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratTitleMusicStopsOnEndPlayTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.TheTitleTrackIsReleasedWhenTheShellMapEnds",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratTitleMusicStopsOnEndPlayTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratTitleMusicFixture;

	// DECLARED, AND THE DECLARATION IS PART OF THE CONTROL THAT `BeginPlay` RAN -- twice, once
	// per shell below. See the header on why it is only PART of it: it bites only while this
	// clause is otherwise passing, and is short-circuited away the moment anything else fails.
	AddExpectedMessagePlain(TEXT("No shell this session"), ELogVerbosity::Error,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 2);

	USoundBase* const Track = LoadObject<USoundBase>(nullptr, kTrackPath);
	if (Track == nullptr)
	{
		AddError(FString::Printf(TEXT("fixture: the track '%s' did not load; nothing was "
			"observed."), kTrackPath));
		return false;
	}

	FTestWorldScope Scope;
	if (Scope.World == nullptr)
	{
		AddError(TEXT("could not create a test world; nothing was observed."));
		return false;
	}

	// THE SOUND DIRECTOR IS POINTED AT A FIXTURE SLOT BEFORE ANY GAMEMODE EXISTS.
	// `AStratShellGameMode::BeginPlay` adopts a bank, and adopting applies the current volumes,
	// which resolves an unset slot name to the REAL `StratocracyAudio` file. This clause has no
	// business reading the developer's own volume settings.
	if (UStratSoundDirector* const Director = Scope.World->GetSubsystem<UStratSoundDirector>())
	{
		Director->UseAudioSettingsSlot(TEXT("StratAudio_Fixture_TitleMusic"));
	}

	// ---- THE UNCONFIGURED SHELL: NO TRACK, NO HANDLE, NO FAULT --------------
	AStratShellGameMode* const Silent = Scope.World->SpawnActorDeferred<AStratShellGameMode>(
		AStratShellGameMode::StaticClass(), FTransform::Identity);
	if (Silent == nullptr)
	{
		AddError(TEXT("could not spawn an AStratShellGameMode; nothing was observed."));
		return false;
	}

	TestNull(TEXT("premise: nothing is playing before BeginPlay"),
		Silent->TitleMusicComponent.Get());

	UGameplayStatics::FinishSpawningActor(Silent, FTransform::Identity);
	Silent->DispatchBeginPlay();

	// THE CONTROL THAT `BeginPlay` ACTUALLY RAN, AND IT IS ASSERTED RATHER THAN INFERRED. This
	// clause's first shape inferred it from an expected-message declaration and WAS WRONG: on an
	// ALREADY-FAILING run the engine short-circuits `HasMetExpectedMessages()` away entirely
	// (`InternalStopTest`, see the header), so a `BeginPlay` that never ran read exactly like one
	// that ran and refused. `HasActorBegunPlay()` is the engine's own answer, holds on a red run
	// as well as a green one, and cannot be satisfied by silence.
	TestTrue(TEXT("control: BeginPlay ran on the unconfigured shell"),
		Silent->HasActorBegunPlay());
	TestNull(TEXT("a shell with no title track spawns no handle"),
		Silent->TitleMusicComponent.Get());

	// ---- THE CONFIGURED SHELL ----------------------------------------------
	AStratShellGameMode* const Shell = Scope.World->SpawnActorDeferred<AStratShellGameMode>(
		AStratShellGameMode::StaticClass(), FTransform::Identity);
	if (Shell == nullptr)
	{
		AddError(TEXT("could not spawn the configured AStratShellGameMode."));
		return false;
	}

	// The one thing the shipped Blueprint default supplies, supplied before BeginPlay reads it
	// -- which is the whole reason the spawn is deferred.
	Shell->TitleMusic = Track;

	UGameplayStatics::FinishSpawningActor(Shell, FTransform::Identity);
	Shell->DispatchBeginPlay();

	TestTrue(TEXT("control: BeginPlay ran on the configured shell"),
		Shell->HasActorBegunPlay());

	UAudioComponent* const Component = Shell->TitleMusicComponent;
	if (Component == nullptr)
	{
		AddError(TEXT("BeginPlay spawned no audio component for a configured TitleMusic, so "
			"neither half of this clause was observed. This is a red result and not a skip: if "
			"a future run has no audio device, this clause must be reported as blocked rather "
			"than weakened until it passes."));
		return false;
	}

	// THE HANDLE IS THE TRACK'S. A component spawned for some other sound would satisfy a bare
	// non-null check; the identity is read off the component and compared against the object the
	// fixture set on the actor, so neither side is a literal.
	TestTrue(TEXT("the component plays the configured track"),
		Component->Sound.Get() == Shell->TitleMusic.Get());

	// `bAutoDestroy` FALSE IS WHAT MAKES THE HANDLE A HANDLE, and it is the property `EndPlay`
	// depends on: with the engine default the component would free itself and the member would
	// be nulled by the GC, which reads identically to a correct release and is not one.
	TestFalse(TEXT("the component is not self-destroying, so EndPlay is the only thing that "
		"can release it"), Component->bAutoDestroy);

	// AND IT IS SPAWNED RATHER THAN CREATED AS A SUBOBJECT because `AGameModeBase` derives
	// `AInfo` and has no `RootComponent` to attach a `USceneComponent` to. (This comment
	// previously went on to say the component "is not one of the actor's own components ... actor
	// teardown does not reach it", which is the same retracted claim as the file header's and is
	// falsified by the very assertion below.)
	// ---- A MEASURED CONTRADICTION OF A PREMISE SINCE RETRACTED AT SOURCE ----
	// `StratShellGameMode.h` USED TO argue for this whole shape by saying the spawned component
	// was "owned by the audio device rather than by this actor's component list, so `EndPlay`
	// must stop AND destroy it explicitly". THAT SENTENCE NO LONGER EXISTS AS WRITTEN: it is
	// quoted here only as the retracted claim this assertion falsified, and the header now
	// carries it inside its own `RETRACTED>` block -- so a reader chasing it into that file
	// should look for the retraction, not for the sentence. It was false in this engine,
	// measured here rather than asserted from either side:
	//
	//   DIAG outer=StratShellGameMode_1 owner=StratShellGameMode_1 registered=1 ownedCount=1
	//
	// `UGameplayStatics::SpawnSound2D` handed the component an OUTER and an OWNER of the
	// spawning actor, registered it, and it is that actor's only owned component. So actor
	// teardown WOULD reach it, and `EndPlay`'s `DestroyComponent()` is belt-and-braces rather
	// than the only thing standing between a menu track and a level transition.
	//
	// THE ASSERTION IS THEREFORE THE MEASUREMENT AND NOT THE HEADER'S CLAIM. It is asserted at
	// all -- rather than dropped -- because the ownership is what makes the rest of the
	// lifecycle legible, and because the day `SpawnSound2D` stops owning the component the
	// header's argument becomes true again and this line is where a reader will be told.
	// THE REQUIREMENT IS UNCHANGED EITHER WAY: `EndPlay` stops the track and releases the
	// handle, and the member going null is a thing ONLY `EndPlay` does.
	//
	// THE ASSERTION'S OWN MESSAGE BELOW STILL SAYS "the class header's stated premise", AND IS
	// LEFT AS IT IS DELIBERATELY. That premise was stated when this line was written and it is
	// what this line falsified; it is now retracted at source, in a `RETRACTED>` block this
	// assertion caused. The string is left untouched so that correcting prose changed no
	// compiled byte in this pass.
	TestTrue(TEXT("measured: the spawned handle IS in the actor's own component list, which "
		"contradicts the class header's stated premise and is recorded as a finding"),
		Shell->GetComponents().Contains(Component));

	// ---- THE MAP ENDS -------------------------------------------------------
	Shell->Destroy();

	TestNull(TEXT("EndPlay released the handle"), Shell->TitleMusicComponent.Get());
	TestTrue(TEXT("and the component itself is destroyed, not merely forgotten"),
		Component->IsBeingDestroyed() || !IsValid(Component));
	TestFalse(TEXT("and it is not still playing"), Component->IsPlaying());

	// THE SILENT SHELL IS TAKEN DOWN TOO, so the world scope's teardown has nothing left that
	// began play. Nothing is asserted about it here; its own null handle was asserted above.
	Silent->Destroy();

	return true;
}
