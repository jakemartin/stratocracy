// The gate for the ownership seam on AStratScoreboardHUD -- GDD §4.9 part 2 / §4.1,
// under T-UI-03, plus the hot-seat viewing-side hand-over the same class owns.
//
// WHY THIS IS THE RISKIEST THING IN PHASE 2 AND THEREFORE HAS ITS OWN FILE.
// `AStratScoreboardHUD` represents ownership of the authoritative `strat::GameState` by
// WHICH OF TWO MEMBERS IS NON-NULL -- `TPimplPtr<FStratBridge> OwnedBridge`, which this
// HUD frees, versus `FStratBridge* AdoptedBridge`, which it must never free. There is no
// `bOwns` flag to read and no compiler diagnostic for getting it wrong. The two ways to
// get it wrong are a leak (the owned bridge kept alive underneath an adopted one) and a
// double free (the adopted bridge destroyed by a HUD that did not allocate it), and the
// second of those does not fail a test -- it takes the process down, or, worse, does not,
// until it does.
//
// WHAT IS PINNED HERE, AND WHAT IS DELIBERATELY NOT. The OBSERVABLE half of the seam is
// pinned: after `AdoptBridge(B)` the HUD reads through `B`; a HUD that adopted and then
// ended play leaves `B` still answering; and `AdoptBridge` refuses a bridge that has not
// been seeded, which makes "seed before handing over" a contract phase 3's subsystem must
// honour rather than a sentence in a header.
//
// THE STORED VIEWING SIDE IS NOW PINNED TOO, and this paragraph replaces a recorded gap
// rather than adding a claim. Until `AStratScoreboardHUD::GetViewingSide()` existed, the
// hot-seat clauses below could assert what `SetViewingSide` RETURNED but not its defining
// consequence -- that a refusal leaves the stored side exactly as it was -- because
// `ViewingSide` is protected and the only other window onto it is a widget that cannot be
// created headless (`UStratScoreboardWidget` is `Abstract`). That reader exists now, it
// clamps nothing on the way out, and three clauses below use it: a refusal leaves the
// stored side where it was, an accepted call moves it (without which the first clause
// would hold of a getter returning a constant), and the reader itself is pure. Every
// "before" value is READ THROUGH `GetViewingSide()` and never assumed to be 0.
//
// THE LEAK ON THE OWNED PATH IS NOT PINNED, and that is a ruling rather than an
// oversight. Proving it would need an allocation counter behind `FStratBridge` -- new
// production surface beyond phase 2's exit criterion, offered and declined, recorded as
// deferred in Tools/architect/state.md. Nothing here approximates it with a heap-size
// reading or a destructor hook, because both flake and a flaky gate is worse than a
// stated gap. The gap is stated: this file does not prove that the owned bridge is freed.
// It proves that the adopted one is not.
//
// WHY THIS FILE LIVES IN StratUI/Tests. `AStratScoreboardHUD` and `StratBuildViewModel`
// are both declared in StratUI, and StratBridge sits BELOW it -- the arrow does not
// reverse, so a StratBridge test could not name either. The rules-side values these
// clauses compare against (`StateHash`, `RecordedLog`, `IsSeeded`, `MakeUiSnapshot`) all
// arrive as METHODS on the exported `FStratBridge`; no `strat::` free function is called
// anywhere below, which is the LNK2019 measured 8x when the bridge first tried and once
// more from this very directory.
//
// TWO WAYS OF STANDING UP A HUD, and the choice is per clause rather than uniform.
//   - `NewObject` into the transient package, for clauses that only call `AdoptBridge`,
//     `SetViewingSide` and `GetBridge`. None of those touches a world, a widget or a
//     player controller, and a clause that needs no world should not create one -- a gate
//     that runs headless on every build is worth more than one that needs a level.
//   - A transient `UWorld`, for the one clause that needs `EndPlay`, because `EndPlay` is
//     protected and the supported way to reach it is to destroy a spawned actor.
//
// NO WIDGET IS CREATED ANYWHERE BELOW, and none can be: `UStratScoreboardWidget` is
// `Abstract` because a WBP_ asset must supply its layout. Every clause therefore
// exercises the no-widget path, which `SetViewingSide` and `BeginPlay` both treat as a
// legitimate configuration ("seed a bridge, put no panel on screen") rather than an
// error, so nothing below logs at Error level in passing.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Misc/Paths.h"
#include "UObject/GCObjectScopeGuard.h"
#include "UObject/UObjectGlobals.h"

#include "StratScoreboardHUD.h"
#include "StratViewModel.h"

// The bridge, included from a .cpp and never from a UHT-parsed header -- the same
// constraint StratScoreboardHUD.h records about itself.
#include "StratBridge.h"

#include "Ui.h"

namespace StratScoreboardHUDSeam
{
	// The harness pin for which side moves first. Replay.h states that no rule in this
	// project decides it; StratScoreboardParity.cpp and StratBridgeParity.cpp pin the same
	// value for the same stated reason.
	static const int32 kFirstSide = 0;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** Brings a bridge to the seeded opening. See StratViewModelParity.cpp for the same
	 *  helper and the reason automation fixtures may name a `/Game/` path at all. */
	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		UDataTable* Units = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		UDataTable* Terrain = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
		if (Units == nullptr || Terrain == nullptr)
		{
			OutError = TEXT("DT_Units and/or DT_Terrain did not load from /Game/StratData");
			return false;
		}

		const FStratResult Loaded = Bridge.LoadDefinitions(Units, Terrain);
		if (!Loaded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadDefinitions refused: %s"), *Loaded.Reason);
			return false;
		}

		const FString ScenarioPath = FPaths::Combine(DataDir(), TEXT("ferrum_crossing.json"));
		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath, kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	/** How many sides this build has, read off the projection rather than written down. */
	static int32 SideCountOf(const strat::UiSnapshot& Snapshot)
	{
		return static_cast<int32>(UE_ARRAY_COUNT(Snapshot.side));
	}

	/** The recorded §4.10 command log's length, as an int32 for comparison. */
	static int32 RecordedCount(const FStratBridge& Bridge)
	{
		return static_cast<int32>(Bridge.RecordedLog().size());
	}

	/**
	 * A HUD with no world, for clauses that need none.
	 *
	 * `AActor` is a `UObject` first; `AdoptBridge`, `SetViewingSide` and `GetBridge` read
	 * and write members of this object and reach neither the world nor the viewport. The
	 * caller keeps it alive with an `FGCObjectScopeGuard`, because an object created into
	 * the transient package with no reference is exactly what a collection during the run
	 * would take.
	 */
	static AStratScoreboardHUD* MakeDetachedHud()
	{
		return NewObject<AStratScoreboardHUD>();
	}
}

// ---------------------------------------------------------------------------
// After AdoptBridge(B), the HUD reads through B.
//
// `GetBridge()` is the ONLY reader of either ownership member, which is what makes this
// one assertion cover every path through the class -- refresh, liveness, teardown. If it
// returns the adopted pointer then all three are reading the subsystem's bridge and none
// of them had to know which kind it had.
//
// The before-adoption case is asserted too, because "GetBridge() returns B" would also
// hold of a class that returned B for reasons unconnected to adoption. A freshly
// constructed HUD has neither bridge and must say so.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratScoreboardHUDAdoptedBridgeIsWhatItReadsTest,
	"Stratocracy.StratUI.T-UI-03.AdoptedBridgeIsWhatTheHudReads",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratScoreboardHUDAdoptedBridgeIsWhatItReadsTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratScoreboardHUDSeam;

	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	AStratScoreboardHUD* const Hud = MakeDetachedHud();
	if (!TestNotNull(TEXT("a HUD was constructed"), Hud))
	{
		return false;
	}
	FGCObjectScopeGuard KeepAlive(Hud);

	// Setup has not run and nothing has been handed over, so there is no bridge at all.
	// A half-set HUD reachable through GetBridge() is the state BeginPlay's failure path
	// resets OwnedBridge to avoid.
	TestNull(TEXT("a HUD that has neither seeded nor adopted holds no bridge"), Hud->GetBridge());

	if (!TestTrue(TEXT("the seeded bridge is adopted"), Hud->AdoptBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}
	TestTrue(TEXT("a successful adoption reports no reason"), Error.IsEmpty());

	TestSamePtr(TEXT("GetBridge() is the bridge that was handed over"), Hud->GetBridge(), &Bridge);

	// Adopting the same bridge twice is a no-op reported as SUCCESS, not an error. A
	// subsystem that hands the same bridge to the same HUD again -- on a retry, on a
	// re-entered BeginPlay -- has done nothing wrong, and the alternative is a refusal
	// every caller would have to special-case.
	TestTrue(TEXT("re-adopting the same bridge is a success"), Hud->AdoptBridge(Bridge, Error));
	TestSamePtr(TEXT("re-adoption leaves the same bridge in place"), Hud->GetBridge(), &Bridge);

	// Adoption changes the SOURCE and not the STATE. The rules module's own digest over
	// the canonical state is the sharpest available statement of that, and the recorded
	// §4.10 log is the sharpest statement that no command was submitted along the way.
	TestTrue(TEXT("the adopted bridge is still seeded"), Bridge.IsSeeded());
	TestEqual(TEXT("adoption submitted no command"), RecordedCount(Bridge), 0);

	return true;
}

// ---------------------------------------------------------------------------
// AdoptBridge refuses an unseeded bridge -- which is what makes "seed it, THEN hand it
// over" a contract instead of a sentence.
//
// WHY THE REFUSAL IS THE RIGHT ANSWER AND NOT A DEFERRAL. `FStratBridge::MakeUiSnapshot`
// already refuses a bridge with nothing to project, so an unseeded bridge adopted here
// would not draw anything wrong -- it would produce a refusal on EVERY refresh, forever
// after, with nothing naming the moment the mistake was made. Phase 3's subsystem is the
// caller that can still fix it, and it is the caller this refusal reaches.
//
// NOTHING CHANGES ON A REFUSAL, which is the other half and the one with teeth: a
// refusal that had already reset `OwnedBridge` would leave the HUD with no bridge at all
// and no way back to the one it destroyed. The clause therefore refuses over a HUD that
// is already reading a good bridge, and asserts that it still is.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratScoreboardHUDAdoptRefusesUnseededTest,
	"Stratocracy.StratUI.T-UI-03.AdoptBridgeRefusesUnseededBridge",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratScoreboardHUDAdoptRefusesUnseededTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratScoreboardHUDSeam;

	AStratScoreboardHUD* const Hud = MakeDetachedHud();
	if (!TestNotNull(TEXT("a HUD was constructed"), Hud))
	{
		return false;
	}
	FGCObjectScopeGuard KeepAlive(Hud);

	// Never loaded, never seeded.
	FStratBridge Unseeded;
	TestFalse(TEXT("the offered bridge is genuinely unseeded"), Unseeded.IsSeeded());

	FString Reason;
	TestFalse(TEXT("an unseeded bridge is refused"), Hud->AdoptBridge(Unseeded, Reason));
	TestFalse(TEXT("the refusal names a reason"), Reason.IsEmpty());
	TestNull(TEXT("a refused adoption leaves the HUD with the bridge it had -- none"), Hud->GetBridge());

	// The same refusal over a HUD that is already reading a good bridge. This is the case
	// that would matter in phase 3: a subsystem retrying a hand-over with a bridge it had
	// not finished setting up must not cost the HUD the match it is already drawing.
	FStratBridge Good;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds"), SeedBridge(Good, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the seeded bridge is adopted"), Hud->AdoptBridge(Good, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratBridge SecondUnseeded;
	FString SecondReason;
	TestFalse(TEXT("a second unseeded bridge is refused too"),
		Hud->AdoptBridge(SecondUnseeded, SecondReason));
	TestFalse(TEXT("that refusal names a reason"), SecondReason.IsEmpty());
	TestSamePtr(TEXT("the refusal did not cost the HUD the bridge it was already reading"),
		Hud->GetBridge(), &Good);
	TestTrue(TEXT("and that bridge is still seeded"), Good.IsSeeded());

	return true;
}

// ---------------------------------------------------------------------------
// EndPlay drops an adopted bridge and destroys NOTHING.
//
// THE CLAUSE THAT COULD NOT FAIL QUIETLY. Every other assertion in this file reports a
// wrong value; this one reports a wrong LIFETIME, and the failure mode of a HUD that
// freed a bridge it did not allocate is a call into freed memory -- which is a crash if
// the project is lucky and a wrong answer if it is not. `Bridge.StateHash()` is called
// AFTER the HUD has ended play precisely because it must reach into the bridge's own
// state to answer: a bridge that had been destroyed cannot produce the same digest it
// produced before, and cannot be relied upon to produce anything at all.
//
// THE EXPECTATION IS THE BRIDGE'S OWN DIGEST, taken from the same object before the HUD
// existed. `strat::canonicalStateHash` over the canonical state bytes is the rules
// module's own statement of "this is the same match"; nothing here recomputes it or
// compares it against a literal.
//
// WHY THIS ONE CLAUSE BUILDS A WORLD. `EndPlay` is protected, so the supported route to
// it is destroying a spawned actor, and spawning needs a world. The world is transient,
// created for this clause and torn down at the end of it, and no map is loaded. The HUD
// goes through a real `BeginPlay` on the adopted path, which is the path phase 3's
// subsystem will use: `AdoptedBridge` is non-null, so `SeedBridge` is skipped entirely
// and this HUD never allocates a bridge of its own. With no `ScoreboardWidgetClass` set,
// BeginPlay records that as a legitimate configuration and returns without error.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratScoreboardHUDEndPlayLeavesAdoptedBridgeAliveTest,
	"Stratocracy.StratUI.T-UI-03.EndPlayLeavesAnAdoptedBridgeUsable",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratScoreboardHUDEndPlayLeavesAdoptedBridgeAliveTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratScoreboardHUDSeam;

	// Declared in this frame and outliving everything below, which is the arrangement
	// phase 3's `UStratMatchSubsystem` has with the HUD it hands a bridge to.
	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	const FString HashBeforeTheHud = Bridge.StateHash();
	const int32   LogBeforeTheHud  = RecordedCount(Bridge);
	TestFalse(TEXT("the bridge produces a state hash before the HUD exists"), HashBeforeTheHud.IsEmpty());

	UWorld* const World = UWorld::CreateWorld(EWorldType::Game, /*bInformEngineOfWorld*/ false);
	if (!TestNotNull(TEXT("a transient world was created to spawn a HUD into"), World))
	{
		return false;
	}

	// A WORLD CONTEXT, REGISTERED AND TORN DOWN WITH THE WORLD. Not decoration:
	// `UWorld::DestroyActor` warns "World has no context!" for a world the engine has never
	// been told about, and a gate that leaves a warning in a green run trains the next
	// reader to skim warnings. Destroying the HUD is the whole point of this clause, so the
	// context exists for exactly as long as the actor that needs it.
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	WorldContext.SetCurrentWorld(World);

	{
		AStratScoreboardHUD* const Hud = World->SpawnActor<AStratScoreboardHUD>();
		if (!TestNotNull(TEXT("the HUD spawned"), Hud))
		{
			GEngine->DestroyWorldContext(World);
			World->DestroyWorld(false);
			return false;
		}

		// ADOPTED BEFORE BeginPlay -- the subsystem's path. BeginPlay then skips SeedBridge
		// entirely, so this HUD never allocates a bridge and there is nothing for it to
		// confuse with the borrowed one.
		if (!TestTrue(TEXT("the seeded bridge is adopted before play begins"),
				Hud->AdoptBridge(Bridge, Error)))
		{
			AddError(Error);
			GEngine->DestroyWorldContext(World);
			World->DestroyWorld(false);
			return false;
		}

		Hud->DispatchBeginPlay();

		TestSamePtr(TEXT("after BeginPlay the HUD is still reading the adopted bridge"),
			Hud->GetBridge(), &Bridge);
		TestEqual(TEXT("an adopted BeginPlay seeded nothing, so the state has not moved"),
			Bridge.StateHash(), HashBeforeTheHud);

		// There is no widget -- UStratScoreboardWidget is Abstract and needs a WBP asset --
		// so the panel is legitimately absent and the HUD says so rather than failing.
		TestFalse(TEXT("with no widget class set there is no live scoreboard"), Hud->IsScoreboardLive());

		// This is the EndPlay. `Destroy` routes it, because EndPlay itself is protected.
		Hud->Destroy();
	}

	// The HUD is gone. Everything below reaches into the bridge it was reading.
	TestTrue(TEXT("the adopted bridge is still seeded after the HUD ended play"), Bridge.IsSeeded());
	TestEqual(TEXT("the adopted bridge still answers with the same canonical state hash"),
		Bridge.StateHash(), HashBeforeTheHud);
	TestEqual(TEXT("the HUD's whole lifetime submitted no command"),
		RecordedCount(Bridge), LogBeforeTheHud);

	// And it is still USABLE, not merely still addressable: the projection the whole
	// engine side reads through still succeeds, and the view model still builds from it.
	strat::UiSnapshot Snapshot;
	const FStratResult Projected = Bridge.MakeUiSnapshot(Snapshot);
	if (TestTrue(TEXT("the adopted bridge still projects a snapshot"), Projected.bOk))
	{
		FStratViewModel Model;
		FString BuildError;
		if (!TestTrue(TEXT("the view model still builds from the adopted bridge"),
				StratBuildViewModel(Bridge, 0, Model, BuildError)))
		{
			AddError(BuildError);
		}
		else
		{
			TestEqual(TEXT("and it describes the same board it did before"),
				Model.Hexes.Num(), static_cast<int32>(Snapshot.hexes.size()));
		}
	}
	else
	{
		AddError(Projected.Reason);
	}

	GEngine->DestroyWorldContext(World);
	World->DestroyWorld(false);

	return true;
}

// ---------------------------------------------------------------------------
// The viewing side selects and never mutates -- T-UI-03's hot-seat clause.
//
// THIS IS THE CLAUSE THAT PINS "MUST NOT MUTATE GAME STATE" and it is written to be
// impossible to satisfy by accident. The hand-over between two players at a shared screen
// happens dozens of times a match; if changing whose screen it is could touch the rules
// module's state, then looking at the board would be a move in the game, and no amount of
// careful play could tell the difference.
//
// TWO MODULE-SIDE WITNESSES, both read from the bridge and neither computed here:
//   - `FStratBridge::StateHash()` -- `strat::canonicalStateHash` over the canonical state
//     bytes. The rules module's own answer to "is this the same match".
//   - `FStratBridge::RecordedLog().size()` -- every §4.10 command this bridge applied.
//     The hash could in principle be equal across a command that undid itself; the log
//     cannot, because it is append-only and appended on success.
// Both must be unchanged across the hand-over, and the model's `ViewingSide` must have
// moved, or the clause proves only that nothing happened at all.
//
// WHAT "SWAPS COLUMNS" IS MADE TO MEAN. `FStratViewModel::Sides` carries EVERY side and
// is indexed by strat side index -- the view model describes a board, and a board does
// not have a YOU column. So the swap is asserted as: the per-side standings are IDENTICAL
// between the two models, and only `ViewingSide` differs. That is exactly "the reading
// moved and the board did not", and a builder that had rearranged `Sides` to put the
// viewer first would fail it.
//
// THE HUD IS EXERCISED FOR THE SAME PROPERTY, over an adopted bridge, because
// `AStratScoreboardHUD::SetViewingSide` is what a turn loop will actually call. With no
// widget on screen it stores the side and returns -- a legitimate configuration the class
// documents -- and the two witnesses above must still be unchanged across the call.
//
// WHERE THE STORED SIDE IS ASSERTED. Not here -- `T-UI-03.SetViewingSideAcceptsInRange`
// owns that clause, over `GetViewingSide()`, so that this one stays about the property it
// is named for (the hand-over is not a move) and does not quietly become two tests in one
// body. This clause's own hand-over loop still checks nothing about the member, and that
// is a division of labour now rather than the coverage hole it used to be.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratScoreboardHUDSetViewingSideDoesNotMutateTest,
	"Stratocracy.StratUI.T-UI-03.SetViewingSideSwapsColumnsWithoutMutatingState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratScoreboardHUDSetViewingSideDoesNotMutateTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratScoreboardHUDSeam;

	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	strat::UiSnapshot Snapshot;
	const FStratResult Projected = Bridge.MakeUiSnapshot(Snapshot);
	if (!TestTrue(TEXT("MakeUiSnapshot succeeds on a seeded bridge"), Projected.bOk))
	{
		AddError(Projected.Reason);
		return false;
	}
	const int32 SideCount = SideCountOf(Snapshot);
	if (!TestTrue(TEXT("there are at least two sides to hand over between"), SideCount >= 2))
	{
		return false;
	}

	// The two witnesses, taken before anything looks at the board.
	const FString HashBefore = Bridge.StateHash();
	const int32   LogBefore  = RecordedCount(Bridge);
	TestFalse(TEXT("the bridge produces a state hash"), HashBefore.IsEmpty());

	AStratScoreboardHUD* const Hud = MakeDetachedHud();
	if (!TestNotNull(TEXT("a HUD was constructed"), Hud))
	{
		return false;
	}
	FGCObjectScopeGuard KeepAlive(Hud);

	if (!TestTrue(TEXT("the seeded bridge is adopted"), Hud->AdoptBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- The hand-over, every side in turn ---------------------------------
	for (int32 Side = 0; Side < SideCount; ++Side)
	{
		FString Reason;
		if (!TestTrue(FString::Printf(TEXT("the hand-over to side %d is accepted"), Side),
				Hud->SetViewingSide(Side, Reason)))
		{
			AddError(Reason);
			continue;
		}

		TestEqual(*FString::Printf(
				TEXT("handing over to side %d did not move the canonical state hash"), Side),
			Bridge.StateHash(), HashBefore);
		TestEqual(FString::Printf(
				TEXT("handing over to side %d submitted no command"), Side),
			RecordedCount(Bridge), LogBefore);
		TestSamePtr(*FString::Printf(
				TEXT("handing over to side %d did not change which bridge is read"), Side),
			Hud->GetBridge(), &Bridge);
	}

	// ---- What the hand-over is FOR: the same board, read from the other seat ----
	FStratViewModel FirstSeat;
	FStratViewModel SecondSeat;
	FString FirstError;
	FString SecondError;
	if (!TestTrue(TEXT("the model builds for side 0"), StratBuildViewModel(Bridge, 0, FirstSeat, FirstError)))
	{
		AddError(FirstError);
		return false;
	}
	if (!TestTrue(TEXT("the model builds for side 1"), StratBuildViewModel(Bridge, 1, SecondSeat, SecondError)))
	{
		AddError(SecondError);
		return false;
	}

	// The reading moved.
	TestEqual(TEXT("the first seat is side 0"), FirstSeat.ViewingSide, 0);
	TestEqual(TEXT("the second seat is side 1"), SecondSeat.ViewingSide, 1);
	TestNotEqual(TEXT("the two seats are different seats"), FirstSeat.ViewingSide, SecondSeat.ViewingSide);

	// The board did not. `Sides` carries every side indexed by strat side index, so a
	// builder that rearranged it to put the viewer first would fail here -- and a phase 3
	// consumer indexing `Sides` by side would then be reading the wrong belligerent.
	if (TestEqual(TEXT("both seats describe the same number of sides"),
			FirstSeat.Sides.Num(), SecondSeat.Sides.Num()))
	{
		for (int32 Side = 0; Side < FirstSeat.Sides.Num(); ++Side)
		{
			const FStratSideView& From0 = FirstSeat.Sides[Side];
			const FStratSideView& From1 = SecondSeat.Sides[Side];
			const FString Where = FString::Printf(TEXT("Sides[%d]"), Side);

			TestEqual(Where + TEXT(".FameTotal does not depend on who is looking"),
				From1.FameTotal, From0.FameTotal);
			TestEqual(Where + TEXT(".FameCombat does not depend on who is looking"),
				From1.FameCombat, From0.FameCombat);
			TestEqual(Where + TEXT(".ObjectivesHeld does not depend on who is looking"),
				From1.ObjectivesHeld, From0.ObjectivesHeld);
			TestEqual(Where + TEXT(".SurvivingHp does not depend on who is looking"),
				From1.SurvivingHp, From0.SurvivingHp);
			TestEqual(Where + TEXT(".IncomePerTurn does not depend on who is looking"),
				From1.IncomePerTurn, From0.IncomePerTurn);
		}
	}

	TestEqual(TEXT("both seats see the same board"), SecondSeat.Hexes.Num(), FirstSeat.Hexes.Num());
	TestEqual(TEXT("both seats see the same units"), SecondSeat.Units.Num(), FirstSeat.Units.Num());
	TestEqual(TEXT("both seats see the same factories"), SecondSeat.Factories.Num(), FirstSeat.Factories.Num());
	TestEqual(TEXT("both seats see the same objective total"),
		SecondSeat.ObjectiveTotal, FirstSeat.ObjectiveTotal);
	TestEqual(TEXT("whose turn it is does not depend on whose screen it is"),
		SecondSeat.Match.SideToMove, FirstSeat.Match.SideToMove);
	TestEqual(TEXT("the turn does not depend on whose screen it is"),
		SecondSeat.Match.Turn, FirstSeat.Match.Turn);

	// The witnesses again, after both projections. Reading the board is not playing on it.
	TestEqual(TEXT("building both seats' models did not move the canonical state hash"),
		Bridge.StateHash(), HashBefore);
	TestEqual(TEXT("building both seats' models submitted no command"),
		RecordedCount(Bridge), LogBefore);

	return true;
}

// ---------------------------------------------------------------------------
// An out-of-range hand-over is refused, not clamped.
//
// SAME REASON AS THE BUILDER'S OWN REFUSAL, one layer up: a clamped viewing side renders
// a coherent, correct-looking screen for the wrong player, and a hot-seat game gives
// nobody a way to notice. `AStratScoreboardHUD::SetViewingSide` range-checks BEFORE the
// assignment so that a refused call changes nothing at all.
//
// THE RANGE IS NOT WRITTEN HERE. The HUD checks against `strat::SIDE_COUNT` and the
// builder checks against the snapshot's own side array; this clause reads the second,
// which is the one the class's own comment names as the authority if the two ever
// disagree.
//
// THE DEFINING CONSEQUENCE IS ASSERTED, and it is the reason this clause was extended:
// after a refused call the STORED side is the value it was before that call. "Changes
// nothing at all" is a claim about the member, not about the return value, and a
// `SetViewingSide` that refused correctly and assigned anyway would pass every other
// assertion in this body.
//
// THE "BEFORE" VALUE IS READ, NOT WRITTEN DOWN. It comes from `GetViewingSide()` taken
// immediately before each refused call -- never from a literal 0 and never from the value
// the accepted call above was passed. The module holds that number; a test that restated
// it would be asserting its own arithmetic, and it would also stop being true the moment
// the HUD's default changed. `GetViewingSide()` does not clamp, so what it returns IS what
// is stored.
//
// BOTH ENDS OF THE RANGE, `-1` and `SideCount`, because an off-by-one that guarded only
// the top would leave the bottom assigning. They are exercised in that order against a HUD
// whose stored side is a real, non-zero side, so a refusal that reset the member to 0
// would be caught rather than coincidentally matching.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratScoreboardHUDSetViewingSideRefusesOutOfRangeTest,
	"Stratocracy.StratUI.T-UI-03.SetViewingSideRefusesOutOfRange",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratScoreboardHUDSetViewingSideRefusesOutOfRangeTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratScoreboardHUDSeam;

	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	strat::UiSnapshot Snapshot;
	const FStratResult Projected = Bridge.MakeUiSnapshot(Snapshot);
	if (!TestTrue(TEXT("MakeUiSnapshot succeeds on a seeded bridge"), Projected.bOk))
	{
		AddError(Projected.Reason);
		return false;
	}
	const int32 SideCount = SideCountOf(Snapshot);

	const FString HashBefore = Bridge.StateHash();
	const int32   LogBefore  = RecordedCount(Bridge);

	AStratScoreboardHUD* const Hud = MakeDetachedHud();
	if (!TestNotNull(TEXT("a HUD was constructed"), Hud))
	{
		return false;
	}
	FGCObjectScopeGuard KeepAlive(Hud);

	if (!TestTrue(TEXT("the seeded bridge is adopted"), Hud->AdoptBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	// A hand-over to a side that exists, first, so the refusals below cannot be satisfied
	// by a method that refuses everything -- and so that the stored side the refusals must
	// leave alone is a real side rather than whatever the default happened to be.
	FString Accepted;
	TestTrue(TEXT("a hand-over to an existing side is accepted"),
		Hud->SetViewingSide(SideCount - 1, Accepted));

	const int32 OutOfRange[] = { SideCount, -1 };
	for (const int32 Side : OutOfRange)
	{
		// READ, NOT ASSUMED. The HUD holds the stored side and is asked for it; nothing
		// here restates what the accepted call above was passed, and nothing assumes 0.
		const int32 StoredBefore = Hud->GetViewingSide();

		FString Reason;
		TestFalse(FString::Printf(TEXT("a hand-over to side %d is refused"), Side),
			Hud->SetViewingSide(Side, Reason));
		TestFalse(FString::Printf(TEXT("the refusal of %d names a reason"), Side), Reason.IsEmpty());

		// THE CLAUSE WITH TEETH. "Refused, not clamped" is a statement about the member,
		// and this is the only assertion in this body that measures it: a SetViewingSide
		// that returned false and assigned anyway would satisfy everything else here.
		TestEqual(*FString::Printf(
				TEXT("refusing %d left the stored viewing side exactly as it was"), Side),
			Hud->GetViewingSide(), StoredBefore);
		TestNotEqual(*FString::Printf(
				TEXT("and the refused value %d was not stored"), Side),
			Hud->GetViewingSide(), Side);

		// A refused hand-over is not a move either. Both witnesses, again.
		TestEqual(*FString::Printf(TEXT("refusing %d did not move the canonical state hash"), Side),
			Bridge.StateHash(), HashBefore);
		TestEqual(FString::Printf(TEXT("refusing %d submitted no command"), Side),
			RecordedCount(Bridge), LogBefore);
		TestSamePtr(*FString::Printf(TEXT("refusing %d did not change which bridge is read"), Side),
			Hud->GetBridge(), &Bridge);
	}

	// And the builder refuses the same values, which is where the range check that phase 3
	// actually depends on lives -- the HUD's copy exists to name the misconfiguration at
	// the moment it is made, not to be the authority on it.
	for (const int32 Side : OutOfRange)
	{
		FStratViewModel Model;
		FString Reason;
		TestFalse(FString::Printf(TEXT("the builder also refuses viewing side %d"), Side),
			StratBuildViewModel(Bridge, Side, Model, Reason));
		TestFalse(FString::Printf(TEXT("the builder's refusal of %d names a reason"), Side), Reason.IsEmpty());
	}

	return true;
}

// ---------------------------------------------------------------------------
// An accepted hand-over stores the side it was given.
//
// WHY THIS CLAUSE EXISTS AT ALL, given that the refusal clause looks like the interesting
// one. "A refusal leaves the stored side unchanged" is satisfied VACUOUSLY by a
// `GetViewingSide()` that returns a constant, and by a `SetViewingSide` that stores
// nothing on any path. This clause is what makes the refusal clause mean something: the
// reader tracks the writer on the accepted path, so when it does NOT move on the refused
// path that is a fact about the refusal rather than a fact about the reader.
//
// EVERY SIDE, AND THE READINGS MUST DIFFER. Setting each side in turn and asserting the
// reader agrees would still pass for a reader wired to the wrong member if that member
// happened to track. The distinctness assertion at the end -- the reading after side 0
// differs from the reading after side 1 -- is what rules out a constant, and it uses the
// values OBSERVED rather than the values passed.
//
// THE RANGE IS READ OFF THE SNAPSHOT, exactly as the refusal clause reads it: the number
// of sides this build has is `UiSnapshot::side`'s own extent, not a literal 2.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratScoreboardHUDSetViewingSideAcceptsInRangeTest,
	"Stratocracy.StratUI.T-UI-03.SetViewingSideAcceptsInRange",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratScoreboardHUDSetViewingSideAcceptsInRangeTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratScoreboardHUDSeam;

	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	strat::UiSnapshot Snapshot;
	const FStratResult Projected = Bridge.MakeUiSnapshot(Snapshot);
	if (!TestTrue(TEXT("MakeUiSnapshot succeeds on a seeded bridge"), Projected.bOk))
	{
		AddError(Projected.Reason);
		return false;
	}
	const int32 SideCount = SideCountOf(Snapshot);
	if (!TestTrue(TEXT("there are at least two sides to tell apart"), SideCount >= 2))
	{
		return false;
	}

	AStratScoreboardHUD* const Hud = MakeDetachedHud();
	if (!TestNotNull(TEXT("a HUD was constructed"), Hud))
	{
		return false;
	}
	FGCObjectScopeGuard KeepAlive(Hud);

	if (!TestTrue(TEXT("the seeded bridge is adopted"), Hud->AdoptBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	// Every side in turn, and what the HUD said after each one -- kept so the comparison
	// below is between two OBSERVED readings and not between two values this test chose.
	TArray<int32> Observed;
	Observed.Reserve(SideCount);

	for (int32 Side = 0; Side < SideCount; ++Side)
	{
		FString Reason;
		if (!TestTrue(FString::Printf(TEXT("the hand-over to side %d is accepted"), Side),
				Hud->SetViewingSide(Side, Reason)))
		{
			AddError(Reason);
			return false;
		}
		TestTrue(FString::Printf(TEXT("an accepted hand-over to %d reports no reason"), Side),
			Reason.IsEmpty());

		// The writer moved and the reader followed. With no widget up, `SetViewingSide`
		// stores and returns -- the legitimate no-panel configuration the class documents
		// -- so this is the assignment and nothing else.
		TestEqual(*FString::Printf(
				TEXT("after accepting %d the stored viewing side is %d"), Side, Side),
			Hud->GetViewingSide(), Side);

		Observed.Add(Hud->GetViewingSide());
	}

	// NOT A CONSTANT. Two different accepted hand-overs produced two different readings,
	// which is the property that stops the refusal clause passing vacuously.
	if (TestEqual(TEXT("one reading was taken per side"), Observed.Num(), SideCount))
	{
		TestNotEqual(TEXT("the reading after side 0 differs from the reading after side 1"),
			Observed[0], Observed[1]);
	}

	return true;
}

// ---------------------------------------------------------------------------
// GetViewingSide reads and does nothing else.
//
// WHAT A "PURE" READER HAS TO MEAN HERE for the two clauses above to be worth anything.
// `GetViewingSide` is the instrument both of them measure with, and an instrument that
// perturbed what it measured -- refreshed the panel, re-projected the bridge, overwrote
// the recorded failure reason -- would make those clauses statements about the reading
// rather than about the stored side. The class's own declaration asserts "does not clamp,
// does not refresh, does not touch the bridge, and cannot fail"; this pins the observable
// part of that.
//
// THREE WITNESSES, ALL READ FROM THE MODULE and none computed here:
//   - `GetViewingSide()` itself, called repeatedly with nothing in between. Same answer.
//   - `IsScoreboardLive()`, the HUD's own statement of whether there is a seeded bridge
//     AND a widget on screen. A reader that refreshed or re-created anything would be the
//     thing most likely to move it.
//   - `LastFailureReason`, the HUD's own record of why there is no scoreboard. A reader
//     that ran a refresh internally would overwrite or clear it.
// The bridge's two witnesses are checked as well, because "does not touch the bridge" is
// checkable against exactly the digest and log the rest of this file uses.
//
// ON A HUD WITH NO BRIDGE TOO. Purity must not depend on there being something to read
// through; a getter that dereferenced the bridge on the way out would crash or refuse on a
// freshly constructed HUD, and phase 3 will have one of those between construction and
// hand-over.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratScoreboardHUDGetViewingSideIsPureTest,
	"Stratocracy.StratUI.T-UI-03.GetViewingSideIsPure",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratScoreboardHUDGetViewingSideIsPureTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratScoreboardHUDSeam;

	// ---- A HUD with nothing handed to it yet -------------------------------
	{
		AStratScoreboardHUD* const Fresh = MakeDetachedHud();
		if (!TestNotNull(TEXT("a HUD was constructed"), Fresh))
		{
			return false;
		}
		FGCObjectScopeGuard KeepAlive(Fresh);

		const int32 FirstRead = Fresh->GetViewingSide();
		TestEqual(TEXT("reading a bridgeless HUD twice gives the same answer"),
			Fresh->GetViewingSide(), FirstRead);
		TestNull(TEXT("reading the viewing side did not conjure a bridge"), Fresh->GetBridge());
	}

	// ---- A HUD reading an adopted, seeded bridge ---------------------------
	FStratBridge Bridge;
	FString Error;
	if (!TestTrue(TEXT("bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	AStratScoreboardHUD* const Hud = MakeDetachedHud();
	if (!TestNotNull(TEXT("a HUD was constructed"), Hud))
	{
		return false;
	}
	FGCObjectScopeGuard KeepAlive(Hud);

	if (!TestTrue(TEXT("the seeded bridge is adopted"), Hud->AdoptBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	// A real, accepted hand-over first, so the value being read back is one the writer put
	// there rather than the class default.
	FString Accepted;
	if (!TestTrue(TEXT("a hand-over to an existing side is accepted"),
			Hud->SetViewingSide(0, Accepted)))
	{
		AddError(Accepted);
		return false;
	}

	// Everything the reader must not disturb, taken from the HUD and the bridge.
	const int32   SideBefore     = Hud->GetViewingSide();
	const bool    LiveBefore     = Hud->IsScoreboardLive();
	const FString FailureBefore  = Hud->LastFailureReason;
	const FString HashBefore     = Bridge.StateHash();
	const int32   LogBefore      = RecordedCount(Bridge);

	// Called repeatedly with no intervening SetViewingSide.
	for (int32 Call = 0; Call < 4; ++Call)
	{
		TestEqual(*FString::Printf(TEXT("read %d gives the same viewing side as the first"), Call),
			Hud->GetViewingSide(), SideBefore);
	}

	// It changed neither of the HUD's own two statements about itself...
	TestTrue(TEXT("reading the viewing side did not change whether the scoreboard is live"),
		Hud->IsScoreboardLive() == LiveBefore);
	TestEqual(TEXT("reading the viewing side did not touch LastFailureReason"),
		Hud->LastFailureReason, FailureBefore);

	// ...nor anything the rules module holds. Reading whose screen it is is not a move.
	TestEqual(TEXT("reading the viewing side did not move the canonical state hash"),
		Bridge.StateHash(), HashBefore);
	TestEqual(TEXT("reading the viewing side submitted no command"),
		RecordedCount(Bridge), LogBefore);
	TestSamePtr(TEXT("reading the viewing side did not change which bridge is read"),
		Hud->GetBridge(), &Bridge);

	return true;
}
