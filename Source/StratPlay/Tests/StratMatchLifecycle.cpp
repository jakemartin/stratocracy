// The gate for UStratMatchSubsystem's lifetime and ownership -- GDD §4.1 / §4.9 / §2.11,
// under T-INT-05 and T-UI-03.
//
// THREE CLAUSES, AND THE FIRST IS THE SHARPEST THING IN PHASE 3.
//
//   1. `SubsystemSeedsBeforeHandover`. `StartMatch` must seed the bridge and THEN hand it
//      to the scoreboard, in that order, because `AStratScoreboardHUD::AdoptBridge` REFUSES
//      an unseeded bridge. Get the order wrong and nothing errors: the adoption returns
//      false, the subsystem logs a refusal, and the HUD carries on drawing from the bridge
//      IT seeded for itself -- a second `strat::GameState` for the same map that agrees
//      with the first exactly until the first command. That is the failure mode. It is
//      invisible to every other clause in this project, it produces no crash and no wrong
//      pixel until a move is made, and no compiler diagnoses it.
//
//      SO THE WITNESS IS A COMMAND, NOT A POINTER. Pointer equality between the HUD's
//      bridge and the subsystem's is asserted -- it is necessary -- but on its own it only
//      says the two agree about an address at one instant. The clause therefore ALSO
//      submits an EndTurn through the subsystem's bridge and requires the HUD's bridge to
//      report the moved canonical state hash and the longer §4.10 log. Two GameStates
//      cannot do that. One can, and only one.
//
//      AND THE HUD SEEDS FIRST, DELIBERATELY. The clause configures the HUD's own tables
//      and scenario and runs its `BeginPlay` BEFORE `StartMatch`, which is the real
//      ordering in a running map and the only ordering in which the HUD ever allocates.
//      `AdoptBridge` must then destroy that bridge and take the subsystem's, and the
//      recorded "before" pointer must no longer be what the HUD reads. That pointer is
//      compared as a VALUE and never dereferenced after the handover -- the object it named
//      is gone, which is the point.
//
//   2. `StartMatchRefusalLeavesNoBridge`. Four ways to misconfigure a match, and after
//      every one of them `GetBridge()` is null and `IsMatchLive()` is false. The one with
//      teeth is the missing scenario file: `LoadDefinitions` SUCCEEDED, so a half-loaded
//      bridge genuinely existed inside `StartMatch` and had to be reset on the way out.
//      `StratMatchSubsystem.h` states why that matters -- a reachable half-loaded bridge
//      "invites a caller to treat 'not seeded' as 'seeded and empty', which is the exact
//      confusion `FStratBridge::MakeUiSnapshot` refuses to permit."
//
//   3. `SetViewingSideMutatesNoState`. A view of a match is not a move in it. The hot-seat
//      hand-over happens dozens of times a match; if it could touch the rules module's
//      state, looking at the board would be playing on it and no amount of careful play
//      could tell the difference. The witnesses are `FStratBridge::StateHash()` --
//      `strat::canonicalStateHash` over the canonical state bytes, the rules module's own
//      answer to "is this the same match" -- and `RecordedLog().size()`, which is
//      append-only and so cannot be equal across a command that undid itself.
//      `AStratScoreboardHUD::SetViewingSide` is pinned for the same property by
//      `Stratocracy.StratUI.T-UI-03.SetViewingSideSwapsColumnsWithoutMutatingState`; this
//      clause pins the subsystem, which is the object a turn loop will actually call.
//
// WHERE THE EXPECTATIONS COME FROM. Every one of them is read from a module:
//   - the state hash and the log length, from `FStratBridge`;
//   - the number of sides, from `FStratViewModel::Sides`, never a literal 2;
//   - the viewing side, from the config that was passed and from `GetViewingSide()`;
//   - "the bridge is seeded", from `FStratBridge::IsSeeded()`.
// No number in this file was computed by this file.
//
// NO `strat::` FREE FUNCTION IS CALLED. `SubmitEndTurn`, `StateHash` and `RecordedLog` are
// all methods on the exported `FStratBridge`; a direct call is LNK2019, measured 8x.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Misc/Paths.h"
#include "UObject/UnrealType.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
#include "StratScoreboardHUD.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

// Included from a .cpp and never from a UHT-parsed header.
#include "StratBridge.h"

namespace StratMatchLifecycle
{
	static const int32 kFirstSide = 0;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** See StratMatchReconcile.cpp for the same helper and the reason it is duplicated. */
	static bool MakeConfig(FStratMatchConfig& Out, FString& OutError)
	{
		Out.UnitTable    = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		Out.TerrainTable = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
		if (Out.UnitTable == nullptr || Out.TerrainTable == nullptr)
		{
			OutError = TEXT("DT_Units and/or DT_Terrain did not load from /Game/StratData");
			return false;
		}

		Out.ScenarioFile    = TEXT("Data/ferrum_crossing.json");
		Out.FirstSide       = kFirstSide;
		Out.ViewingSide     = 0;
		Out.BoardActorClass = AStratBoardActor::StaticClass();
		Out.UnitActorClass  = AStratUnitActor::StaticClass();
		return true;
	}

	static bool SetObjectPropertyOn(UObject* Object, const TCHAR* PropertyName, UObject* Value, FString& OutError)
	{
		if (Object == nullptr)
		{
			OutError = FString::Printf(TEXT("cannot set '%s' on a null object"), PropertyName);
			return false;
		}
		FObjectPropertyBase* const Property =
			FindFProperty<FObjectPropertyBase>(Object->GetClass(), FName(PropertyName));
		if (Property == nullptr)
		{
			OutError = FString::Printf(TEXT("no reflected object property '%s' on '%s'"),
				PropertyName, *Object->GetClass()->GetName());
			return false;
		}
		Property->SetObjectPropertyValue(Property->ContainerPtrToValuePtr<void>(Object), Value);
		return true;
	}

	static bool SetStringPropertyOn(UObject* Object, const TCHAR* PropertyName, const FString& Value, FString& OutError)
	{
		if (Object == nullptr)
		{
			OutError = FString::Printf(TEXT("cannot set '%s' on a null object"), PropertyName);
			return false;
		}
		FStrProperty* const Property = FindFProperty<FStrProperty>(Object->GetClass(), FName(PropertyName));
		if (Property == nullptr)
		{
			OutError = FString::Printf(TEXT("no reflected string property '%s' on '%s'"),
				PropertyName, *Object->GetClass()->GetName());
			return false;
		}
		Property->SetPropertyValue_InContainer(Object, Value);
		return true;
	}

	/**
	 * A transient world whose actors are INITIALISED FOR PLAY.
	 *
	 * THE `InitializeActorsForPlay` CALL IS NOT DECORATION AND WAS MEASURED. `AActor::
	 * PostActorConstruction` only routes `PostInitializeComponents` once the world reports
	 * its actors initialised, and `AController::PostInitializeComponents` is what registers a
	 * controller into `UWorld::PlayerControllerList`. Without it, a spawned
	 * `APlayerController` exists and `GetFirstPlayerController()` still returns null -- so
	 * `UStratMatchSubsystem::FindScoreboardHUD`, which asks exactly that question, finds no
	 * HUD and the hand-over clause silently tests nothing. Measured as
	 * "Expected 'the world reports a first local player controller' to be not null."
	 *
	 * THE WORLD IS DESTROYED BEFORE THE CONTEXT, also measured: the other way round,
	 * `UStratMatchSubsystem::Deinitialize` destroys the board and every unit from inside
	 * `DestroyWorld` and each `UWorld::DestroyActor` warns "World has no context!".
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

	static int32 RecordedCount(const FStratBridge& Bridge)
	{
		return static_cast<int32>(Bridge.RecordedLog().size());
	}
}

// ---------------------------------------------------------------------------
// One strat::GameState per map -- T-INT-05's ownership clause, and phase 3's sharpest.
//
// SEE THIS FILE'S HEADER BLOCK for why pointer equality alone would not be enough and why
// the HUD is made to seed first. What follows is the order of events, which IS the clause:
//
//   1. A PlayerController and an AStratScoreboardHUD are spawned and wired, because
//      `UStratMatchSubsystem::FindScoreboardHUD` looks for the first local player's HUD and
//      a hand-over to a HUD that cannot be found proves nothing.
//   2. The HUD is configured with the same two tables and the same scenario, and its
//      BeginPlay is dispatched. It allocates a bridge OF ITS OWN. This is the state a real
//      map is in when the GameMode's BeginPlay runs.
//   3. `StartMatch`. It seeds, then adopts. The HUD's own bridge is destroyed by
//      `AdoptBridge` and the subsystem's takes its place.
//   4. A command is submitted through the subsystem's bridge and the HUD's bridge is asked
//      what state it is in. If the handover had silently no-opped, this is where the two
//      would disagree -- and nowhere else in the project would.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratMatchSubsystemSeedsBeforeHandoverTest,
	"Stratocracy.StratPlay.T-INT-05.SubsystemSeedsBeforeHandover",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratMatchSubsystemSeedsBeforeHandoverTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchLifecycle;

	// This fixture has no Blueprint defaults, so the board reports every terrain kind as
	// unmeshed. Declared expected rather than suppressed; StratMatchReconcile.cpp's header
	// block records why that is the documented presentation split and not a failed match.
	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	// ---- A findable scoreboard ---------------------------------------------
	APlayerController* const Controller = Scope.World->SpawnActor<APlayerController>();
	if (!TestNotNull(TEXT("a player controller spawned"), Controller))
	{
		return false;
	}

	AStratScoreboardHUD* const Hud = Scope.World->SpawnActor<AStratScoreboardHUD>();
	if (!TestNotNull(TEXT("a scoreboard HUD spawned"), Hud))
	{
		return false;
	}
	Controller->MyHUD = Hud;

	APlayerController* const FirstLocal = Scope.World->GetFirstPlayerController();
	if (!TestNotNull(TEXT("the world reports a first local player controller"), FirstLocal))
	{
		return false;
	}
	if (!TestSamePtr(TEXT("the world's first local player has that HUD"),
			FirstLocal->GetHUD(), static_cast<AHUD*>(Hud)))
	{
		return false;
	}

	// ---- The HUD seeds FIRST, which is the real ordering --------------------
	FString Error;
	UDataTable* const Units   = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
	UDataTable* const Terrain = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
	if (!TestNotNull(TEXT("DT_Units loaded"), Units) || !TestNotNull(TEXT("DT_Terrain loaded"), Terrain))
	{
		return false;
	}

	if (!TestTrue(TEXT("the HUD's own data configuration is set"),
			SetObjectPropertyOn(Hud, TEXT("UnitTable"), Units, Error) &&
			SetObjectPropertyOn(Hud, TEXT("TerrainTable"), Terrain, Error) &&
			SetStringPropertyOn(Hud, TEXT("ScenarioFile"), TEXT("Data/ferrum_crossing.json"), Error)))
	{
		AddError(Error);
		return false;
	}

	Hud->DispatchBeginPlay();

	// A VALUE, RECORDED AND NEVER DEREFERENCED AGAIN. The object it names is destroyed by
	// `AdoptBridge` below; the whole point of recording it is to show that the HUD stopped
	// reading it.
	const FStratBridge* const BridgeTheHudAllocated = Hud->GetBridge();
	if (!TestNotNull(TEXT("running the HUD's BeginPlay first made it seed a bridge of its own"),
			BridgeTheHudAllocated))
	{
		return false;
	}

	// ---- The subsystem starts the match ------------------------------------
	UStratMatchSubsystem* const Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Subsystem))
	{
		return false;
	}

	FStratMatchConfig Config;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}
	// A viewing side that is NOT the default, so "the HUD's side came from this config" is
	// distinguishable from "the HUD happened to already be there".
	Config.ViewingSide = 1;

	// A false return here is the documented presentation split -- see the expected-message
	// note at the top of this clause. What must be true is that the match is LIVE.
	FString StartReason;
	Subsystem->StartMatch(Config, StartReason);
	if (!TestTrue(TEXT("the match is live"), Subsystem->IsMatchLive()))
	{
		AddError(StartReason);
		return false;
	}

	FStratBridge* const Owner = Subsystem->GetBridge();
	if (!TestNotNull(TEXT("the subsystem owns a bridge"), Owner))
	{
		return false;
	}
	TestTrue(TEXT("the subsystem's bridge is seeded, which is what makes the handover legal"),
		Owner->IsSeeded());
	TestTrue(TEXT("the match is live"), Subsystem->IsMatchLive());

	// ---- The handover happened ---------------------------------------------
	TestSamePtr(TEXT("the HUD's bridge IS the subsystem's bridge"),
		Hud->GetBridge(), Owner);
	TestTrue(TEXT("the HUD is no longer reading the bridge its own path allocated"),
		Hud->GetBridge() != BridgeTheHudAllocated);

	TestEqual(TEXT("the HUD is drawn for the side the match was configured for"),
		Hud->GetViewingSide(), Config.ViewingSide);
	TestEqual(TEXT("and so is the subsystem"),
		Subsystem->GetViewingSide(), Config.ViewingSide);

	// ---- THE WITNESS: a command moves ONE state ----------------------------
	// This is what pointer equality cannot say. If the adoption had silently no-opped -- the
	// failure mode a reversed ordering produces, because `AdoptBridge` REFUSES rather than
	// errors -- the HUD would be reading a second, separately seeded `strat::GameState` that
	// agreed with this one right up until this line.
	const FString HashBefore = Owner->StateHash();
	const int32   LogBefore  = RecordedCount(*Owner);
	TestFalse(TEXT("the owner's bridge produces a canonical state hash"), HashBefore.IsEmpty());
	TestEqual(TEXT("the HUD's bridge agrees on the state before the command"),
		Hud->GetBridge()->StateHash(), HashBefore);

	const FStratResult Ended = Owner->SubmitEndTurn();
	if (TestTrue(TEXT("an EndTurn is accepted on the seeded bridge"), Ended.bOk))
	{
		TestNotEqual(TEXT("the command moved the owner's canonical state"),
			Owner->StateHash(), HashBefore);

		TestSamePtr(TEXT("the HUD still reads the same object after the command"),
			Hud->GetBridge(), Owner);
		TestEqual(
			TEXT("ONE strat::GameState per map: the HUD's bridge reports the moved state hash"),
			Hud->GetBridge()->StateHash(), Owner->StateHash());
		TestEqual(
			TEXT("...and the same lengthened command log"),
			RecordedCount(*Hud->GetBridge()), LogBefore + 1);
	}
	else
	{
		AddError(Ended.Reason);
	}

	// ---- Why the ordering is a contract and not a habit --------------------
	// `AdoptBridge` refuses an unseeded bridge, which is exactly why a handover performed
	// before seeding would NOT error -- it would return false and leave the HUD on whatever
	// it already had. Asserted here so the reason this clause exists is visible in the
	// clause, not only in a header.
	{
		FStratBridge Unseeded;
		FString Refusal;
		TestFalse(TEXT("an unseeded bridge is refused, so a reversed StartMatch would no-op"),
			Hud->AdoptBridge(Unseeded, Refusal));
		TestFalse(TEXT("that refusal names a reason"), Refusal.IsEmpty());
		TestSamePtr(TEXT("and a refused handover leaves the HUD on the bridge it already had"),
			Hud->GetBridge(), Owner);
	}

	return true;
}

// ---------------------------------------------------------------------------
// A refused StartMatch leaves NO bridge -- T-INT-05's all-or-nothing clause.
//
// FOUR MISCONFIGURATIONS, AND THEY ARE NOT INTERCHANGEABLE. The first three refuse before
// anything is allocated, which is easy. The fourth -- a scenario path that does not exist --
// refuses AFTER `LoadDefinitions` succeeded, so a half-loaded bridge genuinely existed
// inside `StartMatch` and had to be reset on the way out. That is the one this clause is
// really for; the other three are there so that a subsystem which simply never allocated
// could not pass it.
//
// AND A GOOD CONFIG AT THE END, so the whole clause is not satisfied by a `StartMatch` that
// refuses everything. Without it, "refusal leaves no bridge" holds vacuously of a method
// that never starts a match at all.
//
// THE ERROR LOGS ARE EXPECTED, NOT SUPPRESSED. `StartMatch` logs each refusal at Error
// because a match that did not start IS an error in a running game; declaring the message
// expected keeps the clause green while leaving the log line intact for a human reading it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratMatchStartMatchRefusalLeavesNoBridgeTest,
	"Stratocracy.StratPlay.T-INT-05.StartMatchRefusalLeavesNoBridge",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratMatchStartMatchRefusalLeavesNoBridgeTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchLifecycle;

	// The subsystem's own words on every refusal path. Any number of occurrences.
	AddExpectedErrorPlain(TEXT("No match this session"),
		EAutomationExpectedErrorFlags::Contains, /*Occurrences*/ 0);

	// The good config at the end starts a real match on a fixture with no Blueprint defaults.
	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Subsystem))
	{
		return false;
	}

	// Nothing has been asked of it yet, so there is nothing to hand out.
	TestNull(TEXT("a subsystem that has not started a match holds no bridge"), Subsystem->GetBridge());
	TestFalse(TEXT("...and is not live"), Subsystem->IsMatchLive());

	FStratMatchConfig Good;
	FString Error;
	if (!TestTrue(TEXT("the good match config assembles"), MakeConfig(Good, Error)))
	{
		AddError(Error);
		return false;
	}

	// A refusal, and the three things that must be true after every one of them.
	auto ExpectRefusal = [this, Subsystem](const TCHAR* What, const FStratMatchConfig& Config)
	{
		FString Reason;
		TestFalse(*FString::Printf(TEXT("%s is refused"), What), Subsystem->StartMatch(Config, Reason));
		TestFalse(*FString::Printf(TEXT("%s: the refusal names a reason"), What), Reason.IsEmpty());

		// NO HALF-LOADED BRIDGE IS EVER REACHABLE.
		TestNull(*FString::Printf(TEXT("%s: no bridge is reachable afterwards"), What),
			Subsystem->GetBridge());
		TestFalse(*FString::Printf(TEXT("%s: the match is not live"), What), Subsystem->IsMatchLive());

		// And nothing can be projected from a match that never seeded -- which is the reason
		// the bridge is withheld rather than handed out empty.
		FStratViewModel Model;
		FString BuildReason;
		TestFalse(*FString::Printf(TEXT("%s: nothing can be projected"), What),
			Subsystem->BuildViewModel(Model, BuildReason));
		TestFalse(*FString::Printf(TEXT("%s: that refusal names a reason too"), What),
			BuildReason.IsEmpty());
		TestEqual(*FString::Printf(TEXT("%s: no view model was produced"), What),
			Model.Hexes.Num(), 0);
	};

	// ---- 1 and 2: the tables, named individually ---------------------------
	{
		FStratMatchConfig NoUnits = Good;
		NoUnits.UnitTable = nullptr;
		ExpectRefusal(TEXT("an unset UnitTable"), NoUnits);
	}
	{
		FStratMatchConfig NoTerrain = Good;
		NoTerrain.TerrainTable = nullptr;
		ExpectRefusal(TEXT("an unset TerrainTable"), NoTerrain);
	}

	// ---- 3: an empty scenario name -----------------------------------------
	{
		FStratMatchConfig NoScenario = Good;
		NoScenario.ScenarioFile.Reset();
		ExpectRefusal(TEXT("an empty ScenarioFile"), NoScenario);
	}

	// ---- 4: THE ONE WITH TEETH ---------------------------------------------
	// The tables load, so `LoadDefinitions` succeeds and a bridge with definitions mapped and
	// no scenario seeded exists inside `StartMatch`. It must not survive the return.
	{
		FStratMatchConfig Missing = Good;
		Missing.ScenarioFile = TEXT("Data/no_such_scenario_this_is_a_gate.json");
		ExpectRefusal(TEXT("a scenario path that does not exist"), Missing);
	}

	// ---- And a good config still starts ------------------------------------
	// Without this, every assertion above would hold of a StartMatch that refused
	// unconditionally.
	{
		FString StartReason;
		Subsystem->StartMatch(Good, StartReason);

		// LIVE, not "returned true": with no tile meshes this fixture takes the documented
		// presentation-gap path, which returns false over a correctly seeded match.
		if (TestTrue(TEXT("a good config still starts a live match"), Subsystem->IsMatchLive()))
		{
			if (TestNotNull(TEXT("a started match has a bridge"), Subsystem->GetBridge()))
			{
				TestTrue(TEXT("...and its bridge is seeded"), Subsystem->GetBridge()->IsSeeded());
			}
		}
		else
		{
			AddError(StartReason);
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// A view of a match is not a move in it -- T-UI-03's hot-seat clause, at the subsystem.
//
// TWO MODULE-SIDE WITNESSES, both read from the bridge and neither computed here. See this
// file's header block. Both must be unchanged across every hand-over, and the stored side
// and the applied model's `ViewingSide` must have MOVED -- otherwise the clause would be
// satisfied by a `SetViewingSide` that did nothing at all.
//
// NO HUD IS SPAWNED, on purpose. `RefreshPresentation` asks a scoreboard HUD to refresh when
// one exists, and with no widget class set that refresh legitimately refuses -- which would
// make `SetViewingSide` return false for a reason that has nothing to do with this clause.
// The HUD's own copy of the same property is already pinned by
// `Stratocracy.StratUI.T-UI-03.SetViewingSideSwapsColumnsWithoutMutatingState`; this clause
// is about the subsystem, and it is written so the return value means what it says.
//
// THE REFUSED HAND-OVER IS MEASURED TOO, and it is the interesting half.
//
// **[CORRECTED 2026-08-25, AND THE SENTENCE IT REPLACES IS KEPT AT THE ASSERTION ITSELF.**
// This paragraph used to read that an out-of-range side is refused "and the stored side has
// STILL MOVED, which the class documents deliberately". The class's documented no-rollback is
// about a FAILED REBUILD; this block drives a FAILED RANGE CHECK, and the code used to treat
// the two the same way. It no longer does, so what is asserted here now is that a refused
// out-of-range hand-over leaves the stored side exactly where it was. The surviving half --
// a failed rebuild still hands over -- is pinned by
// `Stratocracy.StratPlay.T-UI-03.ARefusedViewingSideLeavesBothMembersUnchanged`, which can
// drive it because it spawns a HUD and this clause deliberately does not.]**
//
// What must not move, on the refused path exactly as on the accepted one, is the rules
// module's state.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratMatchSetViewingSideMutatesNoStateTest,
	"Stratocracy.StratPlay.T-UI-03.SetViewingSideMutatesNoState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratMatchSetViewingSideMutatesNoStateTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchLifecycle;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Subsystem))
	{
		return false;
	}

	FStratMatchConfig Config;
	FString Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	// Live, not "returned true" -- the documented presentation split, see the reconciliation
	// clause in StratMatchReconcile.cpp.
	FString StartReason;
	Subsystem->StartMatch(Config, StartReason);
	if (!TestTrue(TEXT("the match is live"), Subsystem->IsMatchLive()))
	{
		AddError(StartReason);
		return false;
	}

	FStratBridge* const Bridge = Subsystem->GetBridge();
	if (!TestNotNull(TEXT("the subsystem owns a bridge"), Bridge))
	{
		return false;
	}

	// THE NUMBER OF SIDES IS READ, NEVER WRITTEN. `FStratViewModel::Sides` is indexed by
	// strat side index and carries every side; a build with a different count would move
	// both ends of this loop together.
	const int32 SideCount = Subsystem->GetViewModel().Sides.Num();
	if (!TestTrue(TEXT("there are at least two sides to hand over between"), SideCount >= 2))
	{
		return false;
	}

	const FString HashBefore = Bridge->StateHash();
	const int32   LogBefore  = RecordedCount(*Bridge);
	TestFalse(TEXT("the bridge produces a canonical state hash"), HashBefore.IsEmpty());

	// ---- Every side in turn ------------------------------------------------
	TArray<int32> Observed;
	Observed.Reserve(SideCount);

	for (int32 Side = 0; Side < SideCount; ++Side)
	{
		FString Reason;
		if (!TestTrue(*FString::Printf(TEXT("the hand-over to side %d is accepted"), Side),
				Subsystem->SetViewingSide(Side, Reason)))
		{
			AddError(Reason);
			continue;
		}

		// THE CLAUSE.
		TestEqual(*FString::Printf(
				TEXT("handing over to side %d did not move the canonical state hash"), Side),
			Bridge->StateHash(), HashBefore);
		TestEqual(*FString::Printf(
				TEXT("handing over to side %d submitted no command"), Side),
			RecordedCount(*Bridge), LogBefore);
		TestSamePtr(*FString::Printf(
				TEXT("handing over to side %d did not change which bridge is owned"), Side),
			Subsystem->GetBridge(), Bridge);
		TestTrue(*FString::Printf(TEXT("the match is still live after handing over to side %d"), Side),
			Subsystem->IsMatchLive());

		// AND THE READING MOVED, or the clause above would hold of a method that did
		// nothing. Both the stored side and the model that was actually drawn.
		TestEqual(*FString::Printf(TEXT("the stored viewing side is %d"), Side),
			Subsystem->GetViewingSide(), Side);
		TestEqual(*FString::Printf(TEXT("the applied model is drawn for side %d"), Side),
			Subsystem->GetViewModel().ViewingSide, Side);

		Observed.Add(Subsystem->GetViewingSide());
	}

	if (TestEqual(TEXT("one reading was taken per side"), Observed.Num(), SideCount))
	{
		TestNotEqual(TEXT("the reading after side 0 differs from the reading after side 1 -- not a constant"),
			Observed[0], Observed[1]);
	}

	// ---- A refused hand-over is not a move either --------------------------
	{
		// READ, NOT ASSUMED. The loop above left the subsystem on the last side it handed over
		// to; the assertion below is that a REFUSAL leaves it exactly there, and restating
		// which side that is would make this clause assert its own arithmetic.
		const int32 StoredBefore = Subsystem->GetViewingSide();

		FString Reason;
		TestFalse(*FString::Printf(TEXT("a hand-over to side %d is refused"), SideCount),
			Subsystem->SetViewingSide(SideCount, Reason));
		TestFalse(TEXT("that refusal names a reason"), Reason.IsEmpty());

		TestEqual(TEXT("a refused hand-over did not move the canonical state hash"),
			Bridge->StateHash(), HashBefore);
		TestEqual(TEXT("a refused hand-over submitted no command"),
			RecordedCount(*Bridge), LogBefore);

		// THE PARAGRAPH THIS REPLACES IS KEPT RATHER THAN DELETED, because the conversation it
		// asked for happened. It read:
		//
		//   RETRACTED> "DOCUMENTED, AND ASSERTED AS DOCUMENTED. The class states that the side
		//   RETRACTED>  has still changed on a failed rebuild ... Pinned so that a later change
		//   RETRACTED>  to that decision is a conversation rather than a surprise."
		//   RETRACTED>  TestEqual(..., Subsystem->GetViewingSide(), SideCount);
		//
		// **[CORRECTED 2026-08-25. The sentence it quoted is still true and this clause was
		// applying it to the wrong failure.** `UStratMatchSubsystem::SetViewingSide`'s
		// no-rollback is about a FAILED REBUILD -- the presentation could not be rebuilt for a
		// side that is perfectly legal -- and this block drives a FAILED RANGE CHECK, which is
		// a different thing that the code used to treat the same way. It no longer does: every
		// refusal now runs BEFORE the assignment and the rebuild, the one failure that
		// deliberately does not roll back, runs after it. So a refused OUT-OF-RANGE hand-over
		// leaves this member exactly where it was, and the old expectation of `SideCount` was
		// pinning the defect.
		//
		// THE HALF THAT SURVIVES IS NOT DROPPED, IT MOVED. "A failed rebuild still hands over"
		// is pinned by `Stratocracy.StratPlay.T-UI-03.
		// ARefusedViewingSideLeavesBothMembersUnchanged` in
		// `StratViewingSideHandoverClauses.cpp`, which drives an IN-RANGE side into a fixture
		// with no scoreboard widget -- the rebuild refuses, `SetViewingSide` returns false, and
		// both members have still moved. That clause reads BOTH members, which is what this one
		// structurally could not do: it spawns no HUD on purpose.]**
		TestEqual(
			TEXT("a refused OUT-OF-RANGE hand-over left the stored side exactly as it was -- "
				"the no-rollback rule is about a failed REBUILD, not a failed range check"),
			Subsystem->GetViewingSide(), StoredBefore);
		TestNotEqual(
			*FString::Printf(TEXT("and the refused value %d was not stored"), SideCount),
			Subsystem->GetViewingSide(), SideCount);
	}

	// ---- And the board itself never moved ----------------------------------
	// The last accepted hand-over is restored, and the model it produces describes the same
	// board from the other seat. `SetViewingSide` selects; it does not mutate.
	{
		FString Reason;
		if (TestTrue(TEXT("the hand-over back to side 0 is accepted"),
				Subsystem->SetViewingSide(0, Reason)))
		{
			TestEqual(TEXT("the whole sequence of hand-overs left the state exactly where it was"),
				Bridge->StateHash(), HashBefore);
			TestEqual(TEXT("...and submitted no command at all"),
				RecordedCount(*Bridge), LogBefore);
		}
		else
		{
			AddError(Reason);
		}
	}

	return true;
}
