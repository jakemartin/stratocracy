// `UStratOptionsPresenter`, SPLIT ACROSS TWO ACCEPTANCE IDs BY FACET.
//
// THE SPLIT IS RULED AND IT IS DELIBERATE. `Tools/architect/state/global.md`'s third
// acceptance-ID ruling of 2026-09-05 is explicit that this one class carries two different
// technical facts and is not forced onto one name:
//   (a) *"Showing and hiding the panel by reconciling against `IsOptionsPanelOpen`, and closing
//       that flag on `Deinitialize`, is screen-visibility-tracks-route-state ... This facet
//       rides `GATE-TITLEMENU`."*
//   (b) *"Seeding the widget without committing, and forwarding the widget's own commits
//       onward, is the push-model wiring `GATE-AUDIO-SETTINGS` already claims ... This facet
//       rides `GATE-AUDIO-SETTINGS`."*
// The ruling states why the usual "one class, one ID" shortcut is refused here:
// panel-visibility-tracks-shell-state and settings-model-seeded-without-commit *"are two
// different quantities that happen to be implemented in one class"*. No name is minted.
//
// ---------------------------------------------------------------------------------------
// WHAT THE PRESENTER IS FOR, AND WHY EACH CLAUSE BELOW IS SHAPED THE WAY IT IS.
//
// `StratOptionsPresenter.h` states the discipline in its own words: *"IT RECONCILES; IT DOES NOT
// REACT ... there is no ordering in which this class can be left showing a panel the flag says
// is closed, because nothing here acts on an event's CONTENT."* That is a property about what
// the class IGNORES, which is the hardest kind to observe: a presenter that acted on the
// event's boolean would behave identically in every ordinary session. THE INSTRUMENT IS
// `GetPanelsShownCount()`, which the header introduces for exactly this reason -- *"`IsPanelLive`
// cannot distinguish 'the panel was shown, hidden and shown again' from 'it has been up the
// whole time', and a clause proving the reconcile actually ran needs something only running can
// move."* Two requests against one already-open panel produce TWO broadcasts (pinned in
// `StratOptionsPanelDelegateClauses.cpp`) and must produce ONE show.
//
// WHERE THE EXPECTATIONS COME FROM.
//   - Panel visibility is compared against `UStratShellSubsystem::IsOptionsPanelOpen()`, the
//     shell's own reading, never against a local the clause kept.
//   - The seeded gains are compared against `UStratSoundDirector::GetAudioSettings()`, the
//     director's own persisted object, and the forwarded gains against the widget's own
//     `UStratOptionsWidget::Model`. This file never calls `StratBuildAudioOptionsModel` or
//     `StratClampVolume` to predict a value.
//   - The shown/forwarded COUNTS are the properties themselves ("one reconcile, one show") and
//     are asserted as deltas on a reading taken first.
//
// THE SAVE SLOT. Every clause here calls `UseAudioSettingsSlot(<fixture name>)` on the
// director BEFORE anything can reach `GetAudioSettings`, which is the rule
// `StratAudioVolumeClauses.cpp`'s header states in full: the default slot is `StratocracyAudio`,
// the real file a real player's volumes live in, and a clause that committed to it would
// overwrite the developer's own settings. `UseAudioSettingsSlot` clears the cache with the name,
// so the ordering is load-bearing rather than tidy.
//
// WHAT THESE CLAUSES DO **NOT** PIN, STATED RATHER THAN LEFT TO BE INFERRED.
//   - THAT ANY PIXEL APPEARS. `AddToViewport` in a `-nullrhi` run puts nothing on a screen.
//     What is pinned is that the presenter created, seeded, bound and retained a panel, and
//     that it let go of it again. A human at the keyboard remains the only instrument for
//     whether the screen is legible or operable, which `StratOptionsPresenter.h` already
//     records as an outstanding discharge for the in-match input mode.
//   - THAT THE SHIPPED `WBP_Options` IS THE CLASS CONFIGURED. That is
//     `StratShippedOptionsWidgetClassParity.cpp`'s subject; these clauses configure a double,
//     deliberately, so that a broken asset reddens one clause about the asset rather than
//     every clause about the presenter.
//   - THE CLAMP. Whether a gain is clamped to `[0, 1]` on the way through is
//     `StratAudioVolumeClauses.cpp`'s and `StratOptionsModelClauses.cpp`'s; both sides of the
//     comparison below go through the same clamp, so a clamp defect cancels out here and this
//     file must not be read as covering it.
// ---------------------------------------------------------------------------------------

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Misc/CoreMiscDefines.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "StratAudioSettings.h"
#include "StratOptionsPanelDoubles.h"
#include "StratOptionsPresenter.h"
#include "StratPlayerController.h"
#include "StratShellSubsystem.h"
#include "StratSoundDirector.h"

namespace StratOptionsPresenterClauses
{
	/** A slot name used nowhere else and never by a player. See the file header. */
	const TCHAR* const kFixtureSlot = TEXT("StratOptionsPresenterClauses_Fixture");

	/**
	 * A Game world with a live game instance, its shell, its presenter and its director.
	 *
	 * THE TEARDOWN ORDER IS THE PROJECT'S MEASURED ONE: world first, world context second.
	 * `StratBoardPicking.cpp` records why -- tearing the context down first leaves
	 * `UWorld::DestroyActor` warning "World has no context!" once per actor, and a gate that
	 * leaves warnings in a green run trains the next reader to skim warnings.
	 *
	 * THE INSTANCE IS HELD SEPARATELY AND OUTLIVES `TearDownWorld`, WHICH IS NOT A CONVENIENCE.
	 * `bOptionsPanelOpen` lives on the game INSTANCE subsystem and survives the world -- its own
	 * declaration says so -- and the whole subject of the `Deinitialize` clause is what the
	 * presenter did to that flag on its way out. A fixture that dropped the instance with the
	 * world could not ask the question at all.
	 *
	 * `InitializeStandalone` is what stands the subsystem collection up; a bare
	 * `NewObject<UGameInstance>` has none, measured and recorded in
	 * `StratShellHudCallSiteClauses.cpp`.
	 */
	struct FPresenterScope
	{
		UWorld*                          World = nullptr;
		TStrongObjectPtr<UGameInstance>  Instance;

		/** Empty when the fixture stood up; otherwise the first thing that went wrong. */
		FString Failure;

		explicit FPresenterScope(const TCHAR* const InContextName)
		{
			World = UWorld::CreateWorld(EWorldType::Game, /*bInformEngineOfWorld*/ false);
			if (World == nullptr)
			{
				Failure = TEXT("UWorld::CreateWorld returned null");
				return;
			}

			if (GEngine != nullptr)
			{
				FWorldContext& Context = GEngine->CreateNewWorldContext(EWorldType::Game);
				Context.SetCurrentWorld(World);
			}

			Instance = TStrongObjectPtr<UGameInstance>(NewObject<UGameInstance>(GEngine));
			if (!Instance.IsValid())
			{
				Failure = TEXT("could not allocate a UGameInstance");
				return;
			}
			Instance->InitializeStandalone(InContextName, nullptr);
			World->SetGameInstance(Instance.Get());

			World->InitializeActorsForPlay(FURL());

			// BEFORE ANYTHING CAN CALL `GetAudioSettings`, and that includes
			// `UStratSoundDirector::OnWorldBeginPlay`, which applies the current volumes. See
			// the file header.
			if (UStratSoundDirector* const SoundDirector =
					World->GetSubsystem<UStratSoundDirector>())
			{
				SoundDirector->UseAudioSettingsSlot(FString(kFixtureSlot));
			}
			else
			{
				Failure = TEXT("this Game world has no UStratSoundDirector, so the settings "
					"slot could not be redirected away from the player's real one");
				return;
			}
		}

		~FPresenterScope()
		{
			TearDownWorld();
		}

		/** Destroys the world -- and with it every world subsystem, which is what runs
		 *  `UStratOptionsPresenter::Deinitialize`. The instance and its shell survive. */
		void TearDownWorld()
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

		UStratShellSubsystem* Shell() const
		{
			return Instance.IsValid()
				? Instance->GetSubsystem<UStratShellSubsystem>() : nullptr;
		}

		UStratOptionsPresenter* Presenter() const
		{
			return (World != nullptr) ? World->GetSubsystem<UStratOptionsPresenter>() : nullptr;
		}

		UStratSoundDirector* Director() const
		{
			return (World != nullptr) ? World->GetSubsystem<UStratSoundDirector>() : nullptr;
		}

		/**
		 * Spawns a player controller in THIS world and gives it a `ULocalPlayer`.
		 *
		 * WHY NOT `UGameInstance::CreateLocalPlayer`, WHICH IS THE OBVIOUS CALL. That verb
		 * spawns into `Instance->GetWorld()`, and `InitializeStandalone` sets that to a DUMMY
		 * world of its own making -- so the controller would land in a world this fixture never
		 * looks at, `ShowPanel` would find none in ours, and the clause would read "the
		 * presenter never showed a panel", which is a true sentence about the wrong subject.
		 *
		 * THE LOCAL PLAYER IS THE POINT AND NOT DECORATION. `CreateWidget` refuses a controller
		 * that is not a LOCAL player controller, and `AController::IsLocalController` decides
		 * that on `Player` being set. A bare `SpawnActor<APlayerController>` -- which is what
		 * `StratShellHudCallSiteClauses.cpp` needs and uses -- has none, and the refusal
		 * presents as `CreateWidget` returning null rather than as anything naming a player.
		 *
		 * Returns null on any failure; every caller treats that as a FIXTURE failure.
		 */
		APlayerController* MakeLocalPlayerController()
		{
			if (World == nullptr || GEngine == nullptr)
			{
				return nullptr;
			}

			APlayerController* const Controller = World->SpawnActor<APlayerController>();
			if (Controller == nullptr)
			{
				return nullptr;
			}

			UClass* const LocalPlayerClass = (GEngine->LocalPlayerClass != nullptr)
				? GEngine->LocalPlayerClass.Get()
				: ULocalPlayer::StaticClass();
			ULocalPlayer* const LocalPlayer =
				NewObject<ULocalPlayer>(GEngine, LocalPlayerClass);
			if (LocalPlayer == nullptr)
			{
				return nullptr;
			}

			Controller->Player = LocalPlayer;
			LocalPlayer->PlayerController = Controller;

			return Controller;
		}

		FPresenterScope(const FPresenterScope&)            = delete;
		FPresenterScope& operator=(const FPresenterScope&) = delete;
	};
}

// ---------------------------------------------------------------------------------------
// (1) GATE-TITLEMENU -- the presenter reconciles to the flag, not to the event.
//
// `ReconcileOptionsPanel` is `BlueprintCallable` precisely so a clause can force a pass without
// owning the delegate, which is what this clause does: no `UWorld::BeginPlay` is run, the flag
// is driven directly, and the reconcile is asked for by hand. That keeps the subject to the
// reconcile itself and off the binding, which clause (2) covers separately.
//
// THE THREE THINGS IT ASKS, IN ORDER OF WHAT THEY RULE OUT:
//   - A closed flag with nothing shown reconciles to nothing. (A presenter that showed a panel
//     unconditionally is red here, and would otherwise pass every count below.)
//   - An open flag shows exactly one panel, and a SECOND reconcile against the same open flag
//     shows no more. This is the idempotence the header calls "Idempotent" and it is the half a
//     reactive implementation loses.
//   - A closed flag takes the panel back down, and re-opening shows a SECOND panel -- so the
//     count is not a one-shot latch and `IsPanelLive` genuinely tracks the flag in both
//     directions.
//
// MUTANTS:
//   - `ReconcileOptionsPanel`'s early `if (bShouldBeOpen == IsPanelLive()) return;` deleted --
//     red on "a second reconcile shows no second panel", with the count printing 2.
//   - `HidePanel` bodied as `{}` -- red on `IsPanelLive` after the close.
//   - the reconcile reading the handler's `bIsOpen` argument instead of `IsOptionsPanelOpen()`
//     -- not reachable by this clause, which calls the reconcile directly; it is clause (2)'s.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratOptionsPresenterReconcilesToTheFlagTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.ThePresenterShowsOnePanelPerOpenFlagHoweverOftenItReconciles",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratOptionsPresenterReconcilesToTheFlagTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratOptionsPresenterClauses;

	FPresenterScope Scope(TEXT("StratOptionsPresenterReconcile"));
	if (!Scope.Failure.IsEmpty())
	{
		AddError(Scope.Failure);
		return false;
	}

	UStratOptionsPresenter* const Presenter = Scope.Presenter();
	UStratShellSubsystem* const   Shell     = Scope.Shell();

	if (!TestNotNull(TEXT("FIXTURE: this Game world has a UStratOptionsPresenter"), Presenter)
		|| !TestNotNull(TEXT("FIXTURE: this game instance has a UStratShellSubsystem"), Shell))
	{
		return false;
	}

	// A LOCAL PLAYER, because `ShowPanel` parents the panel to one and reports a failure without
	// it. Reported as a FIXTURE failure and never as a clause result: a world with no player
	// would make every count below zero and the clause would read as "the presenter never
	// showed anything", which is a true sentence about the wrong subject.
	if (!TestNotNull(TEXT("FIXTURE: this world has a local player controller for the panel to "
			"be parented to. Without one ShowPanel refuses and nothing below is measured"),
			Scope.MakeLocalPlayerController()))
	{
		return false;
	}

	// `AddToViewport` IN A `-nullrhi` RUN REACHES NO GAME VIEWPORT AND SAYS SO, AT WARNING.
	// Declared rather than tolerated: a warning left in a green run trains the next reader to
	// skim the channel a real failure arrives on, which is `StratBoardPicking.cpp`'s stated
	// rule for this suite. Declaring it also makes it evidence -- the panel really did reach
	// the engine's viewport subsystem, which is the last step of `ShowPanel` this clause can
	// observe at all.
	AddExpectedMessage(TEXT("No game viewport was found"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	Presenter->ConfigureOptionsPanel(UStratOptionsPanelWidgetDouble::StaticClass(), 100);

	// ---- 1. A CLOSED FLAG SHOWS NOTHING ----------------------------------------------------
	TestFalse(TEXT("premise: the shell says the panel is closed"), Shell->IsOptionsPanelOpen());

	Presenter->ReconcileOptionsPanel();

	TestFalse(TEXT("reconciling against a closed flag leaves no panel live"),
		Presenter->IsPanelLive());
	TestEqual(TEXT("and shows nothing"), Presenter->GetPanelsShownCount(), 0);

	// ---- 2. AN OPEN FLAG SHOWS ONE PANEL ---------------------------------------------------
	Shell->RequestOptionsPanel();
	Presenter->ReconcileOptionsPanel();

	if (!TestEqual(*FString::Printf(
			TEXT("reconciling against an open flag shows exactly one panel (LastFailureReason: "
			     "'%s')"), *Presenter->LastFailureReason),
			Presenter->GetPanelsShownCount(), 1))
	{
		return false;
	}
	TestTrue(TEXT("and the panel is live"), Presenter->IsPanelLive());
	TestTrue(TEXT("and the panel's liveness agrees with the shell's own flag"),
		Presenter->IsPanelLive() == Shell->IsOptionsPanelOpen());

	// ---- 3. A SECOND RECONCILE AGAINST THE SAME OPEN FLAG SHOWS NO SECOND PANEL -------------
	// THIS IS THE IDEMPOTENCE, AND IT IS THE HALF A REACTIVE PRESENTER LOSES. Two requests
	// against one already-open panel produce TWO broadcasts -- pinned in
	// `StratOptionsPanelDelegateClauses.cpp` -- and must produce ONE show.
	Shell->RequestOptionsPanel();
	Presenter->ReconcileOptionsPanel();
	Presenter->ReconcileOptionsPanel();

	TestEqual(TEXT("three reconciles against an open flag have shown exactly one panel. More "
		"than one here means the presenter is acting on the call rather than on the state, and "
		"a player asking for the options screen twice would be stacking panels"),
		Presenter->GetPanelsShownCount(), 1);

	// ---- 4. CLOSING TAKES IT DOWN, AND RE-OPENING SHOWS A SECOND --------------------------
	Shell->CloseOptionsPanel();
	Presenter->ReconcileOptionsPanel();

	TestFalse(TEXT("closing the flag takes the panel down"), Presenter->IsPanelLive());
	TestEqual(TEXT("and hiding is not a show"), Presenter->GetPanelsShownCount(), 1);

	Shell->RequestOptionsPanel();
	Presenter->ReconcileOptionsPanel();

	TestEqual(TEXT("control: re-opening shows a SECOND panel, so the count above was an "
		"idempotent reconcile and not a one-shot latch"),
		Presenter->GetPanelsShownCount(), 2);
	TestTrue(TEXT("and the panel is live again"), Presenter->IsPanelLive());

	return true;
}

// ---------------------------------------------------------------------------------------
// (2) GATE-TITLEMENU -- Deinitialize closes a flag it did not open, with no panel configured.
//
// THE DEBT THIS DISCHARGES, IN THE WORDS IT WAS RECORDED IN. `bOptionsPanelOpen` is a
// `UGameInstanceSubsystem` member and SURVIVES LEVEL TRAVEL; its own declaration names the fix
// as *"a `CloseOptionsPanel` in that owner's `EndPlay`"*, and `UStratOptionsPresenter::
// Deinitialize` is where that landed. Without it, a player who opened the options screen and
// then travelled would arrive in the next map with the flag still true, and the next world's
// presenter would draw a panel nobody asked for.
//
// NO `OptionsWidgetClass` IS CONFIGURED, DELIBERATELY, AND IT IS THE POINT RATHER THAN A
// SIMPLIFICATION. The close must be unconditional -- the presenter's own header says so:
// *"`UStratOptionsPresenter::Deinitialize` calls `CloseOptionsPanel` unconditionally"*. A
// presenter that only closed the flag it had itself opened a panel for would leave exactly the
// case this clause runs: a map with no options screen at all, inheriting a true flag from the
// map before it. It also keeps the clause off `CreateWidget`, so it measures the close and
// nothing else.
//
// THE INSTANCE OUTLIVES THE WORLD, WHICH IS WHAT MAKES THE QUESTION ASKABLE. See
// `FPresenterScope`.
//
// MUTANTS:
//   - `Deinitialize`'s `Shell->CloseOptionsPanel()` deleted -- red, with the flag still true.
//   - it wrapped in `if (IsPanelLive())` -- red on this exact fixture, and green on every
//     fixture that configured a widget class, which is why this clause configures none.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratOptionsPresenterClosesTheFlagOnTeardownTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.ThePresenterClosesTheShellFlagOnTeardownWithNoPanelConfigured",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratOptionsPresenterClosesTheFlagOnTeardownTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratOptionsPresenterClauses;

	FPresenterScope Scope(TEXT("StratOptionsPresenterTeardown"));
	if (!Scope.Failure.IsEmpty())
	{
		AddError(Scope.Failure);
		return false;
	}

	UStratShellSubsystem* const Shell = Scope.Shell();
	if (!TestNotNull(TEXT("FIXTURE: this game instance has a UStratShellSubsystem"), Shell)
		|| !TestNotNull(TEXT("FIXTURE: this Game world has a UStratOptionsPresenter -- the "
			"object whose Deinitialize is the subject"), Scope.Presenter()))
	{
		return false;
	}

	// NOTHING IS CONFIGURED. This world has no options panel, which is a legitimate map
	// configuration and is the case the close must still handle.
	TestFalse(TEXT("premise: the presenter has no panel live"), Scope.Presenter()->IsPanelLive());

	// THE FLAG IS OPENED BY THE SHELL AND NOT BY THE PRESENTER -- "a flag it did not open".
	Shell->RequestOptionsPanel();
	if (!TestTrue(TEXT("premise: the shell's flag is open, and the presenter had nothing to do "
			"with opening it"), Shell->IsOptionsPanelOpen()))
	{
		return false;
	}

	const int32 RequestCountBefore = Shell->GetOptionsPanelRequestCount();

	// ---- THE WORLD ENDS --------------------------------------------------------------------
	Scope.TearDownWorld();

	TestFalse(TEXT("tearing the world down closed the shell's options flag. If this is red, a "
		"player who opens the options screen and then travels arrives in the next map with a "
		"panel nobody asked for -- the defect bOptionsPanelOpen's own declaration names"),
		Shell->IsOptionsPanelOpen());

	// AND IT CLOSED RATHER THAN REQUESTED. A `Deinitialize` that called `RequestOptionsPanel`
	// by mistake would leave the flag true and would also move the counter; this separates the
	// two failures rather than reporting one for both.
	TestEqual(TEXT("and it did not request anything on the way out -- the request history is "
		"untouched"), Shell->GetOptionsPanelRequestCount(), RequestCountBefore);

	return true;
}

// ---------------------------------------------------------------------------------------
// (3) GATE-AUDIO-SETTINGS -- seeding the panel forwards no commit, and a real commit does.
//
// THE DEFECT THIS IS FOR, AND IT IS THE ONE `SeedPanel`'s own block calls the easiest thing in
// the file to get wrong: seeding through the three SETTERS instead of through
// `PushAudioOptions`. A setter fires `OnAudioOptionsCommitted`, so the presenter would re-save
// the settings slot every time a player merely LOOKED at the options screen -- and it would do
// it with whatever the presenter happened to seed, which on a machine with no slot yet is the
// defaults. OPENING AND CLOSING THE SCREEN WOULD SILENTLY RESET THE PLAYER'S VOLUMES, and
// nothing on the screen would look wrong.
//
// THE COUNTER IS THE PRESENTER'S OWN AND IT COUNTS THE FORWARD, NOT THE BROADCAST, which its
// declaration says is the distinction that makes it worth having: *"a binding that was never
// made -- the defect this whole file exists to fix -- reads zero here while reading non-zero"*
// on the widget's side. So the second half of this clause drives a real setter on the live
// panel and requires the counter to move, which a presenter that never bound the delegate fails.
//
// AND THE SEEDED VALUES ARE COMPARED AGAINST THE DIRECTOR'S OWN PERSISTED OBJECT, not against a
// literal: a seed that pushed zeros, or the wrong channel into the wrong row, is red on the
// value even though the count is right.
//
// MUTANTS:
//   - `SeedPanel` calling `SetMasterVolume`/`SetSfxVolume`/`SetMusicVolume` instead of
//     `PushAudioOptions` -- red on the seed's forwarded count, printing 3.
//   - the `OnAudioOptionsCommitted.AddDynamic` in `ShowPanel` deleted -- red on the second half.
//   - `HandleAudioOptionsCommitted` forwarding `InModel.MasterVolume` into `CommitVolumes`'s
//     sfx slot -- red on the value comparison.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratOptionsPresenterSeedsWithoutCommittingTest,
	"Stratocracy.StratPlay.GATE-AUDIO-SETTINGS.ThePresenterSeedsThePanelWithoutForwardingACommitAndForwardsARealOne",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratOptionsPresenterSeedsWithoutCommittingTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratOptionsPresenterClauses;

	FPresenterScope Scope(TEXT("StratOptionsPresenterSeed"));
	if (!Scope.Failure.IsEmpty())
	{
		AddError(Scope.Failure);
		return false;
	}

	UStratOptionsPresenter* const Presenter = Scope.Presenter();
	UStratShellSubsystem* const   Shell     = Scope.Shell();
	UStratSoundDirector* const    Director  = Scope.Director();

	if (!TestNotNull(TEXT("FIXTURE: a UStratOptionsPresenter"), Presenter)
		|| !TestNotNull(TEXT("FIXTURE: a UStratShellSubsystem"), Shell)
		|| !TestNotNull(TEXT("FIXTURE: a UStratSoundDirector"), Director))
	{
		return false;
	}

	if (!TestNotNull(TEXT("FIXTURE: this world has a local player controller for the panel to "
			"be parented to"), Scope.MakeLocalPlayerController()))
	{
		return false;
	}

	// `AddToViewport` IN A `-nullrhi` RUN REACHES NO GAME VIEWPORT AND SAYS SO, AT WARNING.
	// Declared rather than tolerated: a warning left in a green run trains the next reader to
	// skim the channel a real failure arrives on, which is `StratBoardPicking.cpp`'s stated
	// rule for this suite. Declaring it also makes it evidence -- the panel really did reach
	// the engine's viewport subsystem, which is the last step of `ShowPanel` this clause can
	// observe at all.
	AddExpectedMessage(TEXT("No game viewport was found"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	// A KNOWN STATE ON DISK, WRITTEN THROUGH THE DIRECTOR'S OWN VERB AND TO THE FIXTURE SLOT.
	// The three gains are pairwise distinct so a seed that put one channel in all three rows is
	// red on the value rather than passing on the count.
	if (!TestTrue(TEXT("FIXTURE: the director accepted a starting set of gains"),
			Director->CommitVolumes(0.8f, 0.4f, 0.1f)))
	{
		return false;
	}

	const UStratAudioSettings* const Persisted = Director->GetAudioSettings();
	if (!TestNotNull(TEXT("FIXTURE: the director has a settings object to seed from"), Persisted))
	{
		return false;
	}

	const int32 ForwardedBeforeSeed = Presenter->GetVolumeCommitsForwardedCount();

	// ---- THE SEED --------------------------------------------------------------------------
	Presenter->ConfigureOptionsPanel(UStratOptionsPanelWidgetDouble::StaticClass(), 100);
	Shell->RequestOptionsPanel();
	Presenter->ReconcileOptionsPanel();

	if (!TestNotNull(*FString::Printf(
			TEXT("FIXTURE: the presenter created a panel (LastFailureReason: '%s')"),
			*Presenter->LastFailureReason),
			Presenter->OptionsWidget.Get()))
	{
		return false;
	}

	UStratOptionsWidget* const Panel = Presenter->OptionsWidget;

	TestEqual(TEXT("SEEDING FORWARDED NOTHING. Opening the options screen must not write the "
		"settings slot -- a player who merely looks at the screen has chosen nothing"),
		Presenter->GetVolumeCommitsForwardedCount(), ForwardedBeforeSeed);

	// AND THE SEED CARRIED THE PERSISTED GAINS, read off the director's own object.
	TestEqual(TEXT("the panel was seeded with the director's persisted master gain"),
		Panel->Model.MasterVolume, Persisted->MasterVolume);
	TestEqual(TEXT("...its sfx gain"), Panel->Model.SfxVolume, Persisted->SfxVolume);
	TestEqual(TEXT("...and its music gain"), Panel->Model.MusicVolume, Persisted->MusicVolume);

	// ---- A REAL COMMIT ---------------------------------------------------------------------
	// THE OTHER DIRECTION, IN THE SAME CLAUSE. A presenter that forwarded nothing at all would
	// pass everything above perfectly and would leave the options screen a read-only display.
	const int32 ForwardedBeforeCommit = Presenter->GetVolumeCommitsForwardedCount();

	Panel->SetMusicVolume(0.625f);

	TestEqual(TEXT("a setter on the live panel forwards exactly one commit to the director"),
		Presenter->GetVolumeCommitsForwardedCount() - ForwardedBeforeCommit, 1);

	const UStratAudioSettings* const AfterCommit = Director->GetAudioSettings();
	if (TestNotNull(TEXT("the director still has a settings object"), AfterCommit))
	{
		// BOTH SIDES ARE MODULE-SIDE READS. The left is the director's persisted object, the
		// right is the widget's own model. Nothing here states what 0.625 becomes.
		TestEqual(TEXT("and the director's music gain is now the panel's own"),
			AfterCommit->MusicVolume, Panel->Model.MusicVolume);
		TestEqual(TEXT("the master gain was carried across unchanged, not defaulted"),
			AfterCommit->MasterVolume, Panel->Model.MasterVolume);
		TestEqual(TEXT("and the sfx gain likewise -- the forward carries the whole model and "
			"not one float"), AfterCommit->SfxVolume, Panel->Model.SfxVolume);
	}

	// ---- AND DISMISSING CLOSES THE FLAG RATHER THAN THE WIDGET -----------------------------
	// `HandleOptionsDismissed` closes the shell's flag, which comes back through the delegate
	// and takes the panel down -- "one route, not two", in that handler's own words. With no
	// delegate bind in this fixture (no `UWorld::BeginPlay` has run) the flag is the observable
	// half, and the panel staying live here is CORRECT rather than a defect: nothing has
	// reconciled yet.
	Panel->OnOptionsDismissed.Broadcast();

	TestFalse(TEXT("dismissing the panel closes the SHELL'S FLAG. A handler that removed the "
		"widget directly would leave the flag true and the next reconcile would put the panel "
		"straight back"), Shell->IsOptionsPanelOpen());

	TestEqual(TEXT("and dismissing forwarded no commit -- leaving the screen must not re-save"),
		Presenter->GetVolumeCommitsForwardedCount(), ForwardedBeforeCommit + 1);

	return true;
}
