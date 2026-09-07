// `UStratOptionsPresenter`'s EXIT ROUTE -- `SeedExitAvailability` and
// `HandleReturnToTitleRequested`, added 2026-09-07 -- under `GATE-TITLEMENU`.
//
// ---------------------------------------------------------------------------------------
// THE ACCEPTANCE ID, AND IT IS THE ONE THE EXISTING SPLIT ALREADY ASSIGNS. `global.md`'s third
// acceptance-ID ruling of 2026-09-05 splits this class BY FACET: panel-visibility-tracks-
// route-state rides `GATE-TITLEMENU`, and settings-seeded-without-commit rides
// `GATE-AUDIO-SETTINGS`. The exit route is neither of those two facts, but the test the ruling
// itself applies decides it in one line -- *"the fact that decides it -- which route it reaches
// -- has nothing to do with which widget class asks"*. This file is entirely about an
// `EStratShellRoute` arm and its permission, touches no volume, no persistence and no mix, and
// so rides `GATE-TITLEMENU` on the same ground the ruling used for `OptionsButton`. NO NAME IS
// MINTED HERE.
//
// ---------------------------------------------------------------------------------------
// WHAT THIS FILE IS FOR, AND WHY THE SECOND CLAUSE IS THE ONE THAT MATTERS.
//
// `SeedExitAvailability` does NOT test `Facts.bMatchIsLive`. It calls
// `UStratShellSubsystem::IsRoutePermitted(EStratShellRoute::ReturnToTitle, Shell->GatherFacts(),
// Refusal)` -- the same static that greys the title menu's five rows -- and its own comment says
// why: *"Writing `Facts.bMatchIsLive` here instead would compile, would be right today, and
// would be a fourth copy of a rule that lives in one place; the `ReturnToTitle` arm also refuses
// on `bTitleLevelConfigured`, which a hand-written condition would have dropped."*
//
// A CLAUSE THAT ONLY EVER VARIED `bMatchIsLive` WOULD PASS IDENTICALLY AGAINST THE HAND-WRITTEN
// CONDITION, AND WOULD THEREFORE PIN NOTHING THE DESIGN CHOSE. So clause (2) sweeps THREE
// states, and the middle one is the whole point:
//   S1  no live match, no title level   -> refused, "No match in progress."
//   S2  live match, NO title level      -> refused, "No title level is configured."   <-- here
//   S3  live match, title level set     -> permitted, empty reason
// A presenter that wrote `ExitModel.bReturnToTitleEnabled = Facts.bMatchIsLive` is GREEN on S1
// and S3 and RED on S2, with the exit armed on a map that has nowhere to send the player.
//
// WHERE THE EXPECTATIONS COME FROM.
//   - The enabled bit and the refusal sentence are read from
//     `UStratShellSubsystem::IsRoutePermitted` over `Shell->GatherFacts()` -- the module-side
//     decider, asked by the clause, on the same facts the presenter saw. NO SENTENCE IN THIS
//     FILE IS COMPARED AGAINST A STRING LITERAL. The two literals that do appear ("No match in
//     progress.", "No title level is configured.") are in CONTROL assertions whose only job is
//     to show that S1's and S2's oracles are two DIFFERENT sentences, so the comparison above
//     them is discriminating; they are never the expectation side of a clause assertion, and
//     `Stratocracy.StratPlay.GATE-TITLEMENU.ReturnToTitleNamesTheAbsentMatchAheadOfTheAbsentLevel`
//     is the clause that owns those two strings.
//   - `LastFailureReason` after a refusal is compared against the same oracle's `.ToString()`,
//     which is exactly what `ExecuteRoute` assigns on its refusal path.
//   - The route count and the panel's liveness are the presenter's own readings.
//
// WHY THE REFUSED PATH IS THE ONLY EXIT PATH A CLAUSE CAN DRIVE, SAID RATHER THAN DISCOVERED.
// A PERMITTED `ExecuteRoute(ReturnToTitle)` ends in `UGameplayStatics::OpenLevelBySoftObjectPtr`,
// which no `-nullrhi` automation run survives. `GetReturnToTitleRoutesTakenCount`'s own
// declaration names this and is the instrument built for it. **Clause (2) therefore reaches S3
// -- the PERMITTED state -- through `SeedExitAvailability` only, and never broadcasts
// `OnReturnToTitleRequested` while it stands.** That is a deliberate boundary and not an
// oversight; crossing it would travel the automation process's world.
//
// WHAT THESE CLAUSES DO **NOT** PIN.
//   - THE PERMITTED ROUTE ACTUALLY TRAVELLING. Unreachable headlessly, per the paragraph above.
//     What is pinned is that the presenter asks, that a refusal costs nothing, and that the
//     availability the screen shows is the same decider's answer.
//   - THAT ANY PIXEL APPEARS. `AddToViewport` in `-nullrhi` puts nothing on a screen.
//   - THE WIDGET'S OWN DRAWING of the pushed model. That is
//     `Source/StratUI/Tests/StratOptionsExitClauses.cpp`'s subject, against a planted double
//     one module over; these clauses read `UStratOptionsWidget::ExitModel` and stop there.
//   - THAT THE SHIPPED `WBP_Options` HAS AN EXIT CONTROL AT ALL. It does not, today [THE SECOND
//     SENTENCE IS FALSE SINCE THE 2026-09-07 ASSET PASS; SEE THE STAMP BELOW. The FIRST is not:
//     these clauses still pin nothing about any asset, because they load none].
//     STAMPED 2026-09-07 BY `strat-test-author` OVER BASE `46321a6`. `Content/UI/WBP_Options.uasset`
//     acquired `ReturnToTitleButton` in the editor lane's pass, in this same working tree:
//     `grep -a -c ReturnToTitleButton Content/UI/WBP_Options.uasset` returns `1`, positive
//     control `MasterSlider` returns `1`, negative control `ZZZNope` returns `0`, against the
//     working file's `sha256`
//     `7e32ef67836b2d9241ecd70e760905d32e16def2ec7df9e2accdc4a346098adb` (42696 bytes) and the
//     committed LFS oid `367f21e199a50bd66e671607e95c1c58baddad986afdcc94b6cd5075bc953f44`
//     (39771 bytes). The asset fact is now pinned one module over, by
//     `Stratocracy.StratUI.GATE-TITLEMENU.ShippedOptionsWidgetCarriesReturnToTitleButton`
//     (`Source/StratUI/Tests/StratShippedOptionsExitControlParity.cpp`), which reads the compiled
//     widget tree rather than the bytes. **It is in `StratUI` and not here by choice rather than
//     by necessity** -- `StratPlay` depends on `StratUI` publicly and could have named the class
//     -- because the subject is a `UStratOptionsWidget` member's own binding contract, and
//     `StratGuidanceStripClauses.cpp` already puts the shipped-widget-asset clauses in the module
//     that owns the widget.
//   - A QUIRK WORTH KNOWING BEFORE READING A FAILURE HERE, AND IT IS A FIXTURE FACT RATHER THAN
//     A CODE ONE. `GatherFacts` asks `GetGameInstance()->GetWorld()` for match liveness, and
//     `UGameInstance::InitializeStandalone` sets that to a DUMMY world of its own making --
//     NOT the world this fixture creates and hangs the presenter off. In a shipped session
//     there is one world and the distinction does not exist. So `MakeMatchLive` below starts
//     its match on `Instance->GetWorld()`, taken from the shell's own route to it, rather than
//     on `Scope.World`. A fixture that started the match on the obvious world would leave
//     `bMatchIsLive` false, S2 and S3 would collapse onto S1, and clause (2) would go GREEN
//     while measuring nothing -- which is why this is written down here.
// ---------------------------------------------------------------------------------------

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Internationalization/Text.h"
#include "UObject/SoftObjectPath.h"
#include "UObject/SoftObjectPtr.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
#include "StratOptionsPanelDoubles.h"
#include "StratOptionsPresenter.h"
#include "StratOptionsWidget.h"
#include "StratShellSubsystem.h"
#include "StratSoundDirector.h"
#include "StratUnitActor.h"

namespace StratOptionsExitRoute
{
	/** A slot name used nowhere else and never by a player. See `StratAudioVolumeClauses.cpp`'s
	 *  header: the default slot is the one a real player's volumes live in. */
	const TCHAR* const kFixtureAudioSlot = TEXT("StratOptionsExitRouteClauses_Audio");

	/** Likewise for the match save. `FStratMatchConfig::SaveSlotName` defaults to
	 *  `StratocracyMatch` -- THE PLAYER'S SLOT -- and a fixture that leaves it there reads and
	 *  can write a disk it never names. `StratGuidanceRouteClauses.cpp` carries the measurement. */
	const TCHAR* const kAbsentMatchSlot =
		TEXT("StratocracyAutomation_NoSuchSlot_OptionsExitRoute");

	/**
	 * A destination that need not resolve, and deliberately does not have to.
	 *
	 * `GatherFacts` computes `bTitleLevelConfigured = !TitleLevel.IsNull()` and nothing below
	 * loads it -- the only permitted path in this file stops at `SeedExitAvailability`, which
	 * never travels. A soft pointer to a path is therefore the whole fixture, and using a real
	 * map here would add a load that no assertion depends on. (A `/Game/` literal in an
	 * automation fixture is one of the two standing exceptions the project records.)
	 */
	TSoftObjectPtr<UWorld> FixtureTitleLevel()
	{
		return TSoftObjectPtr<UWorld>(
			FSoftObjectPath(TEXT("/Game/Maps/Lvl_StratOptionsExitRouteFixtureTitle."
			                     "Lvl_StratOptionsExitRouteFixtureTitle")));
	}

	UDataTable* LoadTable(const TCHAR* const ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/**
	 * A Game world with a live game instance, its shell, its presenter and its director.
	 *
	 * MODELLED ON `StratOptionsPresenterClauses.cpp`'s `FPresenterScope` AND DELIBERATELY A
	 * SECOND COPY. That struct lives in another translation unit's namespace and is not
	 * reachable from here; this one also has to redirect a SECOND sound director -- the facts
	 * world's -- which that one has no reason to. The teardown order (world, then context) is
	 * the project's measured one; `StratBoardPicking.cpp` records why.
	 */
	struct FExitScope
	{
		UWorld*                         World = nullptr;
		TStrongObjectPtr<UGameInstance> Instance;

		/** Empty when the fixture stood up; otherwise the first thing that went wrong. */
		FString Failure;

		explicit FExitScope(const TCHAR* const InContextName)
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

			// BOTH DIRECTORS, AND THE SECOND ONE IS THE POINT. `StartMatch` runs on the FACTS
			// world (see the file header), whose own `UStratSoundDirector` would otherwise read
			// and apply the developer's real volume slot.
			if (!RedirectAudioSlot(World))
			{
				Failure = TEXT("this Game world has no UStratSoundDirector, so its settings slot "
					"could not be redirected away from the player's real one");
				return;
			}
			if (!RedirectAudioSlot(FactsWorld()))
			{
				Failure = TEXT("the game instance's own world has no UStratSoundDirector, so the "
					"slot the match start would read could not be redirected");
				return;
			}
		}

		~FExitScope()
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

		static bool RedirectAudioSlot(UWorld* const InWorld)
		{
			if (InWorld == nullptr)
			{
				return false;
			}
			UStratSoundDirector* const Director = InWorld->GetSubsystem<UStratSoundDirector>();
			if (Director == nullptr)
			{
				return false;
			}
			Director->UseAudioSettingsSlot(FString(kFixtureAudioSlot));
			return true;
		}

		/** The world `UStratShellSubsystem::GatherFacts` asks about match liveness. NOT
		 *  `World`. See the file header's last bullet. */
		UWorld* FactsWorld() const
		{
			return Instance.IsValid() ? Instance->GetWorld() : nullptr;
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

		/**
		 * Spawns a player controller in THIS world and gives it a `ULocalPlayer`.
		 *
		 * `CreateWidget` refuses a controller that is not a LOCAL player controller, and
		 * `AController::IsLocalController` decides that on `Player` being set --
		 * `StratOptionsPresenterClauses.cpp` records the measurement and the reason
		 * `UGameInstance::CreateLocalPlayer` is the wrong verb here (it spawns into the facts
		 * world, not this one).
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
			ULocalPlayer* const LocalPlayer = NewObject<ULocalPlayer>(GEngine, LocalPlayerClass);
			if (LocalPlayer == nullptr)
			{
				return nullptr;
			}

			Controller->Player            = LocalPlayer;
			LocalPlayer->PlayerController = Controller;

			return Controller;
		}

		FExitScope(const FExitScope&)            = delete;
		FExitScope& operator=(const FExitScope&) = delete;
	};

	/**
	 * Makes `GatherFacts().bMatchIsLive` true, by seeding a real match on the FACTS world.
	 *
	 * `IsMatchLive()` is `Bridge != nullptr && Bridge->IsSeeded()` -- there is no seam and no
	 * shortcut, and there must not be one from a test. `StartMatch`'s RETURN is deliberately
	 * ignored: `StratMatchSubsystem.h` records the documented presentation split, where a
	 * fixture with no tile meshes seeds the bridge and then reports a false from the drawing
	 * half. `IsMatchLive()` is the reading every clause in this suite uses instead.
	 *
	 * Returns an empty string on success, or the reason it could not.
	 */
	FString MakeMatchLive(UWorld* const FactsWorld)
	{
		if (FactsWorld == nullptr)
		{
			return TEXT("the game instance has no world to start a match on");
		}

		UStratMatchSubsystem* const Match = FactsWorld->GetSubsystem<UStratMatchSubsystem>();
		if (Match == nullptr)
		{
			return TEXT("the facts world has no UStratMatchSubsystem");
		}

		FStratMatchConfig Config;
		Config.UnitTable    = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		Config.TerrainTable = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
		if (Config.UnitTable == nullptr || Config.TerrainTable == nullptr)
		{
			return TEXT("DT_Units and/or DT_Terrain did not load from /Game/StratData");
		}
		Config.ScenarioFile    = TEXT("Data/ferrum_crossing.json");
		Config.FirstSide       = 0;
		Config.ViewingSide     = 0;
		Config.BoardActorClass = AStratBoardActor::StaticClass();
		Config.UnitActorClass  = AStratUnitActor::StaticClass();
		Config.SaveSlotName    = FString(kAbsentMatchSlot);

		FString StartReason;
		Match->StartMatch(Config, StartReason);

		if (!Match->IsMatchLive())
		{
			return FString::Printf(
				TEXT("the fixture match did not come up live, so bMatchIsLive would have "
				     "stayed false and this clause would have measured nothing: %s"),
				*StartReason);
		}
		return FString();
	}

	/** `IsRoutePermitted`'s answer for `ReturnToTitle` over the shell's OWN current facts.
	 *  THE ORACLE. Every expectation in this file comes through this one function. */
	bool ExitOracle(const UStratShellSubsystem& Shell, FText& OutRefusal)
	{
		return UStratShellSubsystem::IsRoutePermitted(
			EStratShellRoute::ReturnToTitle, Shell.GatherFacts(), OutRefusal);
	}
}

// ---------------------------------------------------------------------------------------
// (1) With no match live, the options screen's exit is greyed and says why.
//
// THE STATE A PLAYER REACHES THIS IN: the options screen opened from the TITLE MENU.
// `UStratOptionsPresenter` is a `UWorldSubsystem` and exists in the title world too, and
// `UStratShellMenuWidget::HandleOptionsClicked` takes `EStratShellRoute::Options` from there --
// so this is not an edge case, it is half of the screen's traffic.
//
// THE EXPECTATION IS READ FROM `IsRoutePermitted` AND NOT RESTATED. The clause asks the same
// static over the same facts and compares both halves of the answer. A presenter that composed
// its own sentence -- "You can't leave yet" -- would be red on the reason while looking
// perfectly reasonable on screen, and the player would see a sentence the title menu's own
// greyed row does not use.
//
// MUTANTS:
//   - `ExitModel.bReturnToTitleEnabled = true` unconditionally -- red on the bit.
//   - `ExitModel.ReturnToTitleReason = FText::FromString(TEXT("Not available."))` -- red on
//     the sentence, green on the bit.
//   - the `OptionsWidget->PushExitOptions(ExitModel)` call deleted -- red on the bit only if
//     the default were true, so the reason comparison is what carries this one: an unpushed
//     model has an EMPTY reason and the oracle's is not empty, which the control asserts.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratOptionsExitDisabledWithNoMatchTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.TheOptionsExitIsDisabledWithNoMatchLive",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratOptionsExitDisabledWithNoMatchTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratOptionsExitRoute;

	FExitScope Scope(TEXT("StratOptionsExitNoMatch"));
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
	if (!TestNotNull(TEXT("FIXTURE: this world has a local player controller for the panel to "
			"be parented to. Without one ShowPanel refuses and nothing below is measured"),
			Scope.MakeLocalPlayerController()))
	{
		return false;
	}

	// Declared rather than tolerated -- see `StratOptionsPresenterClauses.cpp`'s note. The panel
	// really did reach the viewport subsystem, which is the last step of `ShowPanel` a headless
	// clause can observe at all.
	AddExpectedMessage(TEXT("No game viewport was found"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	Presenter->ConfigureOptionsPanel(UStratOptionsPanelWidgetDouble::StaticClass(), 100);

	// ---- THE ORACLE, ASKED BEFORE THE PANEL EXISTS -----------------------------------------
	FText Refusal;
	const bool bOraclePermits = ExitOracle(*Shell, Refusal);

	if (!TestFalse(TEXT("PREMISE: with no match started and no title level configured, "
			"IsRoutePermitted refuses ReturnToTitle. If this is ever true the fixture stopped "
			"describing the title-screen case and nothing below is the clause it claims"),
			bOraclePermits))
	{
		return false;
	}

	Shell->RequestOptionsPanel();
	Presenter->ReconcileOptionsPanel();

	if (!TestTrue(*FString::Printf(TEXT("FIXTURE: the panel came up (LastFailureReason: '%s')"),
			*Presenter->LastFailureReason), Presenter->IsPanelLive()))
	{
		return false;
	}
	UStratOptionsWidget* const Panel = Presenter->OptionsWidget;
	if (!TestNotNull(TEXT("FIXTURE: the live panel is readable"), Panel))
	{
		return false;
	}

	// ---- THE CLAUSE ------------------------------------------------------------------------
	TestEqual(TEXT("the exit the screen was seeded with is the one IsRoutePermitted gave. A "
		"true here is an armed control on a map with no match to leave"),
		Panel->ExitModel.bReturnToTitleEnabled, bOraclePermits);

	TestTrue(*FString::Printf(TEXT("and the sentence is the DECIDER'S, not one the presenter "
		"composed (the screen says '%s'; IsRoutePermitted says '%s')"),
		*Panel->ExitModel.ReturnToTitleReason.ToString(), *Refusal.ToString()),
		Panel->ExitModel.ReturnToTitleReason.EqualTo(Refusal));

	// CONTROL: the oracle's sentence is not empty, so the equality above is not two empty
	// strings agreeing -- which is exactly what a deleted `PushExitOptions` would produce.
	TestFalse(TEXT("CONTROL: the refusal sentence is non-empty, so the comparison above is "
		"discriminating rather than two blanks"), Refusal.IsEmpty());

	// ---- AND NOTHING ROUTED ----------------------------------------------------------------
	TestEqual(TEXT("seeding an availability takes no route"),
		Presenter->GetReturnToTitleRoutesTakenCount(), 0);

	return true;
}

// ---------------------------------------------------------------------------------------
// (2) The exit tracks `IsRoutePermitted`'s BOTH arms, not only match liveness.
//
// THIS IS THE CLAUSE THE ROUTING DECISION EXISTS FOR -- see the file header for the three-state
// table and for why a `bMatchIsLive`-only clause would pin nothing.
//
// THE PERMITTED STATE IS REACHED THROUGH `SeedExitAvailability` ONLY AND IS NEVER ROUTED. A
// permitted `ExecuteRoute(ReturnToTitle)` calls `OpenLevelBySoftObjectPtr` and no `-nullrhi`
// run survives it. This clause therefore never broadcasts `OnReturnToTitleRequested`; clause (3)
// owns that path and does it in the REFUSED state.
//
// THE RE-SEED IS A CLOSE AND RE-SHOW, BECAUSE `SeedExitAvailability` IS PRIVATE AND IS CALLED
// FROM `ShowPanel`. That is the shipped path -- a player opens the options screen and the
// availability is decided then -- and driving it any other way would measure a seam rather than
// the screen. Each show makes a NEW widget, so each state is read off `OptionsWidget` afresh.
//
// MUTANTS:
//   - `ExitModel.bReturnToTitleEnabled = Shell->GatherFacts().bMatchIsLive` -- the exact
//     shortcut the code refuses. GREEN on S1 and S3, RED on S2 (both the bit and the sentence).
//   - `IsRoutePermitted(EStratShellRoute::Options, ...)` in place of `ReturnToTitle` -- that arm
//     is always permitted, so red on S1 and S2.
//   - the ternary `bEnabled ? FText::GetEmpty() : Refusal` replaced by a bare `Refusal` -- green
//     here, because `IsRoutePermitted` clears `OutRefusalReason` at entry. Said so a reader
//     does not credit this clause with pinning that line; `ADisabledReasonIsEmptyExactlyWhenThe
//     RowIsEnabled` is where that shape is pinned, one class over.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratOptionsExitTracksBothArmsTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.TheOptionsExitTracksIsRoutePermittedAndNotOnlyMatchLiveness",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratOptionsExitTracksBothArmsTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratOptionsExitRoute;

	FExitScope Scope(TEXT("StratOptionsExitBothArms"));
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
	if (!TestNotNull(TEXT("FIXTURE: this world has a local player controller"),
			Scope.MakeLocalPlayerController()))
	{
		return false;
	}

	AddExpectedMessage(TEXT("No game viewport was found"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	Presenter->ConfigureOptionsPanel(UStratOptionsPanelWidgetDouble::StaticClass(), 100);

	// A local that reopens the panel and returns the freshly seeded exit model, or reports a
	// fixture failure. Each open is one `SeedExitAvailability`, which is the shipped path.
	auto ReopenAndRead =
		[&](const TCHAR* const What, FStratOptionsExitModel& OutModel) -> bool
	{
		Shell->CloseOptionsPanel();
		Presenter->ReconcileOptionsPanel();
		Shell->RequestOptionsPanel();
		Presenter->ReconcileOptionsPanel();

		if (!TestTrue(*FString::Printf(TEXT("FIXTURE: the panel came up for %s "
				"(LastFailureReason: '%s')"), What, *Presenter->LastFailureReason),
				Presenter->IsPanelLive()))
		{
			return false;
		}
		UStratOptionsWidget* const Panel = Presenter->OptionsWidget;
		if (!TestNotNull(*FString::Printf(TEXT("FIXTURE: the live panel for %s is readable"),
				What), Panel))
		{
			return false;
		}
		OutModel = Panel->ExitModel;
		return true;
	};

	// ---- S1: NO MATCH, NO TITLE LEVEL ------------------------------------------------------
	FText S1Refusal;
	const bool bS1Permits = ExitOracle(*Shell, S1Refusal);

	FStratOptionsExitModel S1;
	if (!TestFalse(TEXT("PREMISE (S1): with no match, the route is refused"), bS1Permits)
		|| !ReopenAndRead(TEXT("S1 (no match, no title level)"), S1))
	{
		return false;
	}

	TestEqual(TEXT("S1: the seeded bit is IsRoutePermitted's"),
		S1.bReturnToTitleEnabled, bS1Permits);
	TestTrue(*FString::Printf(TEXT("S1: the seeded sentence is IsRoutePermitted's ('%s' vs "
		"'%s')"), *S1.ReturnToTitleReason.ToString(), *S1Refusal.ToString()),
		S1.ReturnToTitleReason.EqualTo(S1Refusal));

	// ---- S2: A LIVE MATCH, STILL NO TITLE LEVEL. THE STATE THE CLAUSE EXISTS FOR. ----------
	const FString MatchFailure = MakeMatchLive(Scope.FactsWorld());
	if (!MatchFailure.IsEmpty())
	{
		AddError(MatchFailure);
		return false;
	}

	FText S2Refusal;
	const bool bS2Permits = ExitOracle(*Shell, S2Refusal);

	if (!TestFalse(TEXT("PREMISE (S2): a live match with NO title level configured is STILL "
			"refused -- this is the arm a hand-written bMatchIsLive test would have dropped, "
			"and if it ever permits here the whole clause has stopped discriminating"),
			bS2Permits))
	{
		return false;
	}
	// CONTROL: S1's and S2's refusals are two DIFFERENT sentences. Without this the equality
	// below is satisfied by a presenter that latched S1's answer and never re-decided.
	if (!TestFalse(*FString::Printf(TEXT("CONTROL: S1 and S2 refuse for different reasons "
			"('%s' vs '%s'), so the comparison below can tell them apart"),
			*S1Refusal.ToString(), *S2Refusal.ToString()),
			S1Refusal.EqualTo(S2Refusal)))
	{
		return false;
	}

	FStratOptionsExitModel S2;
	if (!ReopenAndRead(TEXT("S2 (live match, no title level)"), S2))
	{
		return false;
	}

	TestEqual(TEXT("S2: the exit is STILL GREYED in a live match with nowhere to send the "
		"player. A true here means the presenter is reading match liveness and not asking "
		"IsRoutePermitted, and the player gets a control that would refuse"),
		S2.bReturnToTitleEnabled, bS2Permits);
	TestTrue(*FString::Printf(TEXT("S2: and it says the SECOND arm's sentence, not the first "
		"('%s'; IsRoutePermitted says '%s')"), *S2.ReturnToTitleReason.ToString(),
		*S2Refusal.ToString()), S2.ReturnToTitleReason.EqualTo(S2Refusal));

	// ---- S3: A LIVE MATCH AND A TITLE LEVEL -- PERMITTED, AND NEVER ROUTED -----------------
	Shell->ConfigureTitleDestination(FixtureTitleLevel());

	FText S3Refusal;
	const bool bS3Permits = ExitOracle(*Shell, S3Refusal);

	if (!TestTrue(TEXT("PREMISE (S3): a live match with a title level configured IS permitted"),
			bS3Permits))
	{
		return false;
	}

	FStratOptionsExitModel S3;
	if (!ReopenAndRead(TEXT("S3 (live match, title level configured)"), S3))
	{
		return false;
	}

	TestEqual(TEXT("S3: the exit is ARMED. A false here is an options screen a player in a "
		"live match cannot leave from, which is the requirement this whole change is"),
		S3.bReturnToTitleEnabled, bS3Permits);
	TestTrue(TEXT("S3: and it carries no explanation, because there is nothing to explain"),
		S3.ReturnToTitleReason.EqualTo(S3Refusal));
	TestTrue(TEXT("S3: which is to say the sentence is empty"),
		S3.ReturnToTitleReason.IsEmpty());

	// ---- AND NOT ONE OF THE THREE SEEDS TOOK A ROUTE ---------------------------------------
	// Including S3, deliberately: `SeedExitAvailability` decides an availability and must never
	// act on it. A non-zero here in a `-nullrhi` run would mean this clause had travelled the
	// automation process's world.
	TestEqual(TEXT("no seed, in any of the three states, took the route"),
		Presenter->GetReturnToTitleRoutesTakenCount(), 0);

	return true;
}

// ---------------------------------------------------------------------------------------
// (3) A refused return-to-title takes no route and leaves the panel up.
//
// THE ONLY EXIT PATH A `-nullrhi` CLAUSE SURVIVES, and the presenter's own header says so.
// `HandleReturnToTitleRequested` asks `ExecuteRoute`, which re-asks `IsRoutePermitted` itself
// and returns false BEFORE travelling -- so the whole handler runs and nothing opens a level.
//
// THE THREE THINGS IT PINS, AND WHY EACH IS ITS OWN FAILURE:
//   - THE COUNT STAYS 0. `++ReturnToTitleRoutesTakenCount` sits after the refusal's early
//     return, deliberately: the counter answers *"did this class leave the match"* and not the
//     weaker *"was this class asked"*, which the widget's own broadcast already answers. A
//     presenter that counted the ask would report a route it never took.
//   - THE PANEL STAYS UP. A refusal means the player is still where they were; taking their
//     screen away would be a second consequence of a request that had none. A handler that
//     closed the flag first and discovered the refusal afterwards would leave the player back
//     in the match with the screen they were reading gone for nothing -- which is the exact
//     ordering `HandleReturnToTitleRequested` calls its load-bearing choice.
//   - THE REASON IS THE DECIDER'S. `LastFailureReason` is compared against
//     `IsRoutePermitted`'s own `FText`, `.ToString()`d -- which is what `ExecuteRoute` assigns.
//
// IT DRIVES THE WIDGET'S DELEGATE AND NOT THE HANDLER, so the binding made in `ShowPanel` is
// part of the subject. A presenter that never bound reads 0 on the count for the wrong reason,
// which is why `LastFailureReason` is asserted too: an unbound panel leaves it EMPTY.
//
// MUTANTS:
//   - `++ReturnToTitleRoutesTakenCount` moved above the refusal's early return -- red on the
//     count.
//   - `Shell->CloseOptionsPanel()` moved ahead of the `ExecuteRoute` -- red on IsPanelLive.
//   - the `OnReturnToTitleRequested.AddDynamic` in `ShowPanel` deleted -- red on
//     LastFailureReason, which stays empty.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratOptionsRefusedExitTakesNoRouteTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.ARefusedReturnToTitleTakesNoRouteAndLeavesThePanelUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratOptionsRefusedExitTakesNoRouteTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratOptionsExitRoute;

	FExitScope Scope(TEXT("StratOptionsExitRefused"));
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
	if (!TestNotNull(TEXT("FIXTURE: this world has a local player controller"),
			Scope.MakeLocalPlayerController()))
	{
		return false;
	}

	AddExpectedMessage(TEXT("No game viewport was found"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
	// THE REFUSAL IS THE SUBJECT, so both of its warnings are declared rather than tolerated --
	// one from `ExecuteRoute`, one from the presenter forwarding it.
	AddExpectedMessage(TEXT("Shell route refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
	AddExpectedMessage(TEXT("return to title refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	Presenter->ConfigureOptionsPanel(UStratOptionsPanelWidgetDouble::StaticClass(), 100);

	FText Refusal;
	const bool bOraclePermits = ExitOracle(*Shell, Refusal);
	if (!TestFalse(TEXT("PREMISE: the route is refused, which is the only state a -nullrhi "
			"clause may drive this path in. A permitted one would travel the level"),
			bOraclePermits))
	{
		return false;
	}

	Shell->RequestOptionsPanel();
	Presenter->ReconcileOptionsPanel();

	if (!TestTrue(*FString::Printf(TEXT("FIXTURE: the panel came up (LastFailureReason: '%s')"),
			*Presenter->LastFailureReason), Presenter->IsPanelLive()))
	{
		return false;
	}
	UStratOptionsWidget* const Panel = Presenter->OptionsWidget;
	if (!TestNotNull(TEXT("FIXTURE: the live panel is readable"), Panel))
	{
		return false;
	}

	TestEqual(TEXT("premise: no route has been taken"),
		Presenter->GetReturnToTitleRoutesTakenCount(), 0);

	// ---- THE ASK, THROUGH THE WIDGET'S OWN DELEGATE ----------------------------------------
	Panel->OnReturnToTitleRequested.Broadcast();

	TestEqual(TEXT("a REFUSED return to title takes no route. A 1 here means the presenter "
		"counted being asked rather than having left, and the counter would report a match "
		"exit that never happened"), Presenter->GetReturnToTitleRoutesTakenCount(), 0);

	TestTrue(TEXT("and the panel is still up -- a refusal leaves the player exactly where they "
		"were, and taking their screen away would be a second consequence of a request that "
		"had none"), Presenter->IsPanelLive());
	TestTrue(TEXT("and the shell's own flag agrees the panel is open"),
		Shell->IsOptionsPanelOpen());

	TestEqual(*FString::Printf(TEXT("and the recorded reason is the DECIDER'S sentence (the "
		"presenter says '%s'; IsRoutePermitted says '%s'). An empty one here means the panel's "
		"delegate was never bound and the ask reached nothing"),
		*Presenter->LastFailureReason, *Refusal.ToString()),
		Presenter->LastFailureReason, Refusal.ToString());

	// A second ask is a second refusal and not a latch -- the player is still on the screen and
	// may press it again.
	Panel->OnReturnToTitleRequested.Broadcast();
	TestEqual(TEXT("a second refused ask still takes no route"),
		Presenter->GetReturnToTitleRoutesTakenCount(), 0);
	TestTrue(TEXT("and the panel is still up"), Presenter->IsPanelLive());

	return true;
}
