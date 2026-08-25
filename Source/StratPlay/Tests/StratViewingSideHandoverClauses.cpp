// GDD §2.11 -- the hot-seat viewing-side hand-over, across the TWO members that hold it.
// T-UI-03, on `StratScoreboardHUDSeam.cpp`'s precedent for the same acceptance ID.
//
// THE DEFECT THESE CLAUSES CLOSE, AND WHY ONE CLAUSE COULD NOT HAVE CLOSED IT. Two objects
// hold a viewing side, and until 2026-08-25 they desynced by construction:
//
//   - `AStratScoreboardHUD::SetViewingSide` range-checks BEFORE assigning, so a refused call
//     changes nothing there. That half was always right.
//   - `UStratMatchSubsystem::SetViewingSide` assigned `ViewingSide = InViewingSide` FIRST,
//     then forwarded to the HUD, and returned false on the HUD's refusal without reaching
//     `RefreshPresentation`.
//
// So a refused out-of-range hand-over left the SUBSYSTEM holding a side the HUD had rejected,
// and every later refresh then failed inside `StratBuildViewModel` naming the builder rather
// than the hand-over. The in-place comments described the no-rollback as deliberate -- and it
// is, but its stated reason is about a FAILED REBUILD, not a FAILED RANGE CHECK, and the code
// applied it to both.
//
// `Stratocracy.StratUI.T-UI-03.SetViewingSideRefusesOutOfRange` reads the HUD's member alone,
// and the HUD's half was never wrong -- so that clause PASSED THROUGHOUT THE LIFE OF THE BUG.
// The first clause below is what it could not be: it reads BOTH members, before and after
// every refusal, and requires both to be exactly where they were. A clause that read only one
// of them would have caught nothing, which is the whole point of writing this one.
//
// THE SECOND CLAUSE COVERS BEHAVIOUR THAT PREVIOUSLY HAD NO AUTHORITY AT ALL. With no HUD in
// the world, NOTHING range-checked the side -- and that was true before the fix too, silently:
// a HUD-less subsystem is a legitimate configuration (every fixture with no actors in its
// world is one), and it used to adopt any `int32` handed to it and discover the problem on the
// next refresh, or never. The fix gave that path an authority -- a TRIAL BUILD through
// `StratBuildViewModel` at the CANDIDATE side, discarded and never applied -- and nothing
// checked it. This clause is that check, in both directions: an out-of-range side is refused
// with the subsystem untouched, and an in-range one is still accepted, without which "refuses"
// is satisfied by a method that refuses everything.
//
// WHERE THE EXPECTATIONS COME FROM.
//   - Every "before" value is READ, through `GetViewingSide()` on whichever object is being
//     asserted about, immediately before the call being measured. Nothing assumes 0 and
//     nothing restates what an earlier accepted call was passed. Both readers are pure and
//     clamp nothing on the way out, so what they return IS what is stored.
//   - The side count is `FStratViewModel::Sides.Num()`, the projection's own -- the same
//     authority `StratBuildViewModel` indexes against. A literal 2 here would be this file
//     deciding how many sides the game has.
//   - The in-range side used to prove the refusals are not blanket refusals is
//     `SideCount - 1`, derived from that same count.
//   - No refusal sentence is transcribed. The reasons are asserted non-empty and required to
//     NAME the refused side, which is the one thing a hand-over refusal must carry for the
//     next reader; the wording belongs to the classes that own it.
//
// BOTH ENDS OF THE RANGE, `SideCount` and `-1`, because an off-by-one that guarded only the
// top would leave the bottom assigning, and they run against a stored side that is a REAL,
// NON-DEFAULT side -- so a refusal that reset a member to 0 is caught rather than
// coincidentally matching.
//
// A REFUSED HAND-OVER IS NOT A MOVE EITHER. The canonical state hash and the recorded command
// count are taken before and after: a view of a match must never become part of the match.
//
// NO WIDGET IS CREATED ANYWHERE BELOW and none can be -- `UStratScoreboardWidget` is
// `Abstract` because a WBP_ asset must supply its layout -- so every clause exercises the
// no-widget path, which `SetViewingSide` and `BeginPlay` both treat as a legitimate
// configuration rather than an error.
//
// WHY StratPlay. `UStratMatchSubsystem` is this module's, `AStratScoreboardHUD` is reachable
// from here through the public `StratUI` arrow, and `FStratBridge` is reachable privately. No
// `strat::` free function is called anywhere below.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
#include "StratScoreboardHUD.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

// The bridge, included from a .cpp and never from a UHT-parsed header -- the same constraint
// StratScoreboardHUD.h records about itself. `StratViewModel.h` only forward declares it, and
// the two witnesses below (`StateHash`, `RecordedCommandCount`) are methods on the complete
// type. No `strat::` free function is called anywhere in this file.
#include "StratBridge.h"

namespace StratViewingSideHandoverClauses
{
	static const int32 kFirstSide = 0;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** See `StratAiMatchClauses.cpp` for the same helper and the reason it is duplicated.
	 *  `bRecordCompletionOnMatchEnd` is deliberately left at its C++ default, so no path here
	 *  can create a save slot and `Saved/SaveGames/` is untouched by this file. */
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
		Out.ViewingSide     = kFirstSide;
		Out.BoardActorClass = AStratBoardActor::StaticClass();
		Out.UnitActorClass  = AStratUnitActor::StaticClass();
		return true;
	}

	/**
	 * A transient world whose actors are INITIALISED FOR PLAY.
	 *
	 * Copied in shape from `StratGuidanceRouteClauses.cpp`, whose block records the
	 * measurement: without `InitializeActorsForPlay`, `AController::PostInitializeComponents`
	 * never runs, a spawned `APlayerController` is not in `UWorld::PlayerControllerList`, and
	 * `UStratMatchSubsystem::FindScoreboardHUD` -- which asks `GetFirstPlayerController()` --
	 * finds nothing. The HUD clause below would then silently become the NO-HUD clause and
	 * both would measure the same path.
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

	/** The board's own sentence about a fixture carrying no Blueprint defaults. NOTE:
	 *  `Occurrences 0` means "at least one", not "zero or more" -- measured on this suite, so
	 *  only ever declare a line that fires. */
	static void ExpectTheTileMeshWarning(FAutomationTestBase& Test)
	{
		Test.AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
			EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
	}

	/**
	 * A live match in a transient world, optionally with a findable scoreboard HUD.
	 *
	 * THE HUD'S OWN `BeginPlay` IS NOT DISPATCHED. `BeginPlay` is where the HUD would create
	 * its panel from `ScoreboardWidgetClass`, and that ends in `AddToViewport`, which a
	 * headless `-nullrhi` run has no viewport for. Neither clause below is about who
	 * instantiated a panel; both are about a stored int32 on two objects.
	 */
	struct FHandoverHarness
	{
		FTestWorldScope       Scope;
		UStratMatchSubsystem* Subsystem  = nullptr;
		AStratScoreboardHUD*  Hud        = nullptr;
		APlayerController*    Controller = nullptr;

		bool Arm(FAutomationTestBase& Test, bool bWithHud, FString& OutError)
		{
			if (Scope.World == nullptr)
			{
				OutError = TEXT("no transient world was created");
				return false;
			}

			if (bWithHud)
			{
				Controller = Scope.World->SpawnActor<APlayerController>();
				if (Controller == nullptr)
				{
					OutError = TEXT("no player controller spawned");
					return false;
				}

				Hud = Scope.World->SpawnActor<AStratScoreboardHUD>();
				if (Hud == nullptr)
				{
					OutError = TEXT("no scoreboard HUD spawned");
					return false;
				}
				Controller->MyHUD = Hud;

				if (Scope.World->GetFirstPlayerController() == nullptr)
				{
					OutError = TEXT("the world reports no first local player controller, so "
					                "FindScoreboardHUD would find nothing and the HUD clause "
					                "would silently measure the no-HUD path instead");
					return false;
				}
			}

			Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
			if (Subsystem == nullptr)
			{
				OutError = TEXT("the world has no match subsystem");
				return false;
			}

			FStratMatchConfig Config;
			if (!MakeConfig(Config, OutError))
			{
				return false;
			}

			ExpectTheTileMeshWarning(Test);

			FString StartReason;
			Subsystem->StartMatch(Config, StartReason);
			if (!Subsystem->IsMatchLive() || Subsystem->GetBridge() == nullptr)
			{
				OutError = FString::Printf(TEXT("no live match: %s"), *StartReason);
				return false;
			}

			return true;
		}
	};

	/** How many sides the projection reports. The authority `StratBuildViewModel` itself
	 *  indexes against; a literal here would be this file deciding the side count. */
	static bool SideCountOf(UStratMatchSubsystem* Subsystem, int32& OutCount, FString& OutError)
	{
		FStratViewModel Model;
		if (!Subsystem->BuildViewModel(Model, OutError))
		{
			return false;
		}
		OutCount = Model.Sides.Num();
		return true;
	}
}

// ---------------------------------------------------------------------------
// A refused hand-over leaves BOTH members exactly as they were.
//
// The existing `T-UI-03.SetViewingSideRefusesOutOfRange` reads the HUD's copy, and the HUD's
// copy was never the broken one. This clause reads the SUBSYSTEM's as well, on the same
// refusal, and that second read is the entire difference between a clause that would have
// caught the desync and one that passed through it.
//
// AN IN-RANGE HAND-OVER RUNS FIRST, for two reasons that are both load-bearing. It puts a
// REAL, NON-DEFAULT side into both members, so a refusal that reset either to 0 is caught
// rather than coincidentally matching. And it proves the two members MOVE TOGETHER on a call
// that was not refused -- without which "they are equal after a refusal" is satisfied by two
// ints that never change at all.
//
// AND IT IS THE OTHER FAILURE MODE, WHICH IS WHY ITS RETURN VALUE IS RECORDED RATHER THAN
// ASSERTED. This fixture has no scoreboard WIDGET and can have none, so the rebuild inside
// `RefreshPresentation` refuses and `SetViewingSide` returns FALSE for a side that is
// perfectly in range. On a FAILED REBUILD the side has still changed, deliberately -- rolling
// back would make a hand-over silently stay with the previous player. So both failure modes
// return false, and the ONLY observable that tells them apart is whether the members moved.
// That is the split the 2026-08-25 fix introduced and it is what this clause measures.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRefusedViewingSideLeavesBothMembersUnchangedTest,
	"Stratocracy.StratPlay.T-UI-03.ARefusedViewingSideLeavesBothMembersUnchanged",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRefusedViewingSideLeavesBothMembersUnchangedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratViewingSideHandoverClauses;

	FHandoverHarness Harness;
	FString          Error;
	if (!TestTrue(TEXT("a live match with a findable scoreboard HUD is armed"),
			Harness.Arm(*this, /*bWithHud*/ true, Error)))
	{
		AddError(Error);
		return false;
	}

	UStratMatchSubsystem* const Subsystem = Harness.Subsystem;
	AStratScoreboardHUD*  const Hud       = Harness.Hud;

	int32 SideCount = 0;
	if (!TestTrue(TEXT("the projection reports how many sides there are"),
			SideCountOf(Subsystem, SideCount, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(*FString::Printf(TEXT("the scenario has more than one side (it has %d)"),
				SideCount),
			SideCount >= 2))
	{
		return false;
	}

	const FStratBridge* const Bridge = Subsystem->GetBridge();
	if (!TestNotNull(TEXT("a live match owns a bridge"), Bridge))
	{
		return false;
	}
	const FString HashBefore = Bridge->StateHash();
	const int32   LogBefore  = Bridge->RecordedCommandCount();

	// ---- An IN-RANGE hand-over, and it is the other failure mode ------------
	//
	// THE RETURN VALUE IS RECORDED AND DELIBERATELY NOT ASSERTED TRUE, and the reason is the
	// distinction this clause exists to pin. This fixture has no scoreboard WIDGET and can
	// have none -- `UStratScoreboardWidget` is `Abstract` because a WBP_ asset must supply its
	// layout -- so `AStratScoreboardHUD::RefreshScoreboard` refuses, `RefreshPresentation`
	// carries that refusal out, and `SetViewingSide` returns FALSE. That is a FAILED REBUILD,
	// and on a failed rebuild the side HAS still changed, deliberately: rolling back would
	// make a hot-seat hand-over silently stay with the previous player, which is the one
	// outcome this game must not produce quietly.
	//
	// SO THE TWO FAILURE MODES BOTH RETURN FALSE, AND THE ONLY THING THAT TELLS THEM APART IS
	// WHETHER THE MEMBERS MOVED. That is exactly what is measured here and in the loop below:
	// an in-range call moves both, an out-of-range call moves neither. A clause that asserted
	// only the return value could not distinguish them at all.
	const int32 Target = SideCount - 1;
	{
		FString HandoverReason;
		const bool bReturned = Subsystem->SetViewingSide(Target, HandoverReason);
		AddInfo(FString::Printf(
			TEXT("an in-range hand-over to side %d returned %s ('%s') -- this fixture has no "
				"scoreboard widget, so the rebuild refuses"),
			Target, bReturned ? TEXT("true") : TEXT("false"), *HandoverReason));

		// Both members moved, and to the SAME side, whatever the rebuild said. Without this
		// the refusals below could be satisfied by two values that never change at all.
		TestEqual(TEXT("an in-range hand-over moves the subsystem's member"),
			Subsystem->GetViewingSide(), Target);
		TestEqual(TEXT("and the HUD's member"), Hud->GetViewingSide(), Target);
	}

	// ---- Both ends of the range, refused ------------------------------------
	const int32 OutOfRange[] = { SideCount, INDEX_NONE };
	for (const int32 Side : OutOfRange)
	{
		// READ, NOT ASSUMED, on BOTH objects, immediately before the call being measured.
		const int32 SubsystemBefore = Subsystem->GetViewingSide();
		const int32 HudBefore       = Hud->GetViewingSide();

		FString Reason;
		TestFalse(*FString::Printf(TEXT("a hand-over to side %d is refused"), Side),
			Subsystem->SetViewingSide(Side, Reason));
		TestFalse(*FString::Printf(TEXT("the refusal of %d names a reason"), Side),
			Reason.IsEmpty());
		TestTrue(
			*FString::Printf(TEXT("and the refusal of %d names the side it refused ('%s')"),
				Side, *Reason),
			Reason.Contains(FString::FromInt(Side), ESearchCase::CaseSensitive));

		// THE CLAUSE WITH TEETH, AND IT IS THE HALF THE EXISTING T-UI-03 CLAUSE CANNOT MAKE.
		// The HUD's member was never the broken one; the subsystem's was, and a
		// `SetViewingSide` that refused correctly and assigned anyway satisfies every other
		// assertion in this body.
		TestEqual(
			*FString::Printf(TEXT("refusing %d left the SUBSYSTEM's viewing side exactly as it "
				"was"), Side),
			Subsystem->GetViewingSide(), SubsystemBefore);
		TestEqual(
			*FString::Printf(TEXT("refusing %d left the HUD's viewing side exactly as it was"),
				Side),
			Hud->GetViewingSide(), HudBefore);

		// And they still agree with each other, which is the property the desync broke.
		TestEqual(
			*FString::Printf(TEXT("refusing %d left the two members AGREEING"), Side),
			Subsystem->GetViewingSide(), Hud->GetViewingSide());

		TestNotEqual(
			*FString::Printf(TEXT("and the refused value %d was stored by neither"), Side),
			Subsystem->GetViewingSide(), Side);

		// THE TWO FAILURE MODES, SIDE BY SIDE. Both returned false; the in-range call above
		// left both members on `Target` and this one left them where it found them. That
		// difference is the fix, and it is the only observable that distinguishes them.
		TestEqual(
			*FString::Printf(TEXT("refusing %d left both members on the side the in-range "
				"hand-over put them on (%d)"), Side, Target),
			Subsystem->GetViewingSide(), Target);
	}

	// A refused hand-over is not a move.
	TestEqual(TEXT("the refusals did not move the canonical state hash"),
		Bridge->StateHash(), HashBefore);
	TestEqual(TEXT("and submitted no command"),
		Bridge->RecordedCommandCount(), LogBefore);

	return true;
}

// ---------------------------------------------------------------------------
// With NO HUD, a refused hand-over leaves the subsystem unchanged -- and an accepted one
// still lands.
//
// THIS PATH HAD NO AUTHORITY BEFORE 2026-08-25 AND NO TEST EITHER. With no HUD there was
// nothing to range-check the side, so the subsystem adopted whatever `int32` it was handed
// and the problem surfaced -- if ever -- as a later refresh failing inside
// `StratBuildViewModel` for an unrelated-looking reason. The fix gave the path an authority
// without adding a constant: a TRIAL BUILD through `StratBuildViewModel` AT THE CANDIDATE
// SIDE, discarded and never applied, which gets the check without committing to the value.
//
// A HUD-LESS SUBSYSTEM IS A LEGITIMATE CONFIGURATION and not a broken fixture -- every
// fixture with no actors in its world is one -- so this is production behaviour being pinned
// rather than a corner being papered over. The harness ASSERTS the configuration it wanted
// (`FindScoreboardHUD` has no controller to ask) by spawning neither actor, and the clause
// above asserts the opposite configuration for itself, so the two cannot silently become the
// same test.
//
// BOTH DIRECTIONS. Refusal alone is satisfied by a method that refuses everything, and that
// would be a WORSE regression than the one this closes -- a hand-over that never worked. The
// accepted call runs first and is required to land.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRefusedViewingSideWithNoHudLeavesTheSubsystemUnchangedTest,
	"Stratocracy.StratPlay.T-UI-03.ARefusedViewingSideWithNoHudLeavesTheSubsystemUnchanged",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRefusedViewingSideWithNoHudLeavesTheSubsystemUnchangedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratViewingSideHandoverClauses;

	FHandoverHarness Harness;
	FString          Error;
	if (!TestTrue(TEXT("a live match with NO scoreboard HUD is armed"),
			Harness.Arm(*this, /*bWithHud*/ false, Error)))
	{
		AddError(Error);
		return false;
	}

	UStratMatchSubsystem* const Subsystem = Harness.Subsystem;

	// THE CONFIGURATION, ASSERTED. `FindScoreboardHUD` is private, so the fixture's own
	// premise is pinned through the thing it depends on: with no player controller in the
	// world there is no HUD to find, and the trial-build branch is the one that runs.
	if (!TestNull(TEXT("this world has no first local player controller, so there is no HUD "
			"to range-check the side"),
			Harness.Scope.World->GetFirstPlayerController()))
	{
		return false;
	}

	int32 SideCount = 0;
	if (!TestTrue(TEXT("the projection reports how many sides there are"),
			SideCountOf(Subsystem, SideCount, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(*FString::Printf(TEXT("the scenario has more than one side (it has %d)"),
				SideCount),
			SideCount >= 2))
	{
		return false;
	}

	const FStratBridge* const Bridge = Subsystem->GetBridge();
	if (!TestNotNull(TEXT("a live match owns a bridge"), Bridge))
	{
		return false;
	}
	const FString HashBefore = Bridge->StateHash();
	const int32   LogBefore  = Bridge->RecordedCommandCount();

	// ---- An in-range hand-over still lands, with no HUD in the world --------
	{
		const int32 Target = SideCount - 1;

		FString AcceptReason;
		if (!TestTrue(
				*FString::Printf(TEXT("with no HUD, a hand-over to the existing side %d is "
					"still ACCEPTED"), Target),
				Subsystem->SetViewingSide(Target, AcceptReason)))
		{
			AddError(AcceptReason);
			return false;
		}
		TestEqual(TEXT("and the subsystem moved to it"), Subsystem->GetViewingSide(), Target);
	}

	// ---- Both ends of the range, refused by the trial build -----------------
	const int32 OutOfRange[] = { SideCount, INDEX_NONE };
	for (const int32 Side : OutOfRange)
	{
		const int32 Before = Subsystem->GetViewingSide();

		FString Reason;
		TestFalse(
			*FString::Printf(TEXT("with no HUD, a hand-over to side %d is refused rather than "
				"silently adopted"), Side),
			Subsystem->SetViewingSide(Side, Reason));
		TestFalse(*FString::Printf(TEXT("the refusal of %d names a reason"), Side),
			Reason.IsEmpty());
		TestTrue(
			*FString::Printf(TEXT("and the refusal of %d names the side it refused ('%s')"),
				Side, *Reason),
			Reason.Contains(FString::FromInt(Side), ESearchCase::CaseSensitive));

		// THE CLAUSE WITH TEETH. Before the fix this member held whatever it was handed.
		TestEqual(
			*FString::Printf(TEXT("refusing %d left the subsystem's viewing side exactly as it "
				"was"), Side),
			Subsystem->GetViewingSide(), Before);
		TestNotEqual(
			*FString::Printf(TEXT("and the refused value %d was not stored"), Side),
			Subsystem->GetViewingSide(), Side);
	}

	// ---- The discarded trial build is discarded -----------------------------
	// The probe builds a model at the CANDIDATE side and throws it away; nothing it produced
	// may reach the rules state, and the model the caller can still read must be the one the
	// subsystem committed to.
	{
		FStratViewModel Model;
		FString         ModelReason;
		if (TestTrue(TEXT("the view model still builds after the refusals"),
				Subsystem->BuildViewModel(Model, ModelReason)))
		{
			TestEqual(TEXT("and it is built for the side the subsystem actually holds"),
				Model.ViewingSide, Subsystem->GetViewingSide());
		}
		else
		{
			AddError(ModelReason);
		}
	}

	TestEqual(TEXT("the refusals did not move the canonical state hash"),
		Bridge->StateHash(), HashBefore);
	TestEqual(TEXT("and submitted no command"),
		Bridge->RecordedCommandCount(), LogBefore);

	return true;
}
