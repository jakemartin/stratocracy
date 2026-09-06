// T-INT-05 -- THE DAMAGE FLASH'S ARMING AND ITS RETIREMENT, AND THE ARBITRATION BETWEEN THE
// TWO CLOCKS THAT NOW SHARE ONE TICK FLAG.
//
// WHAT GAP THESE CLAUSES CLOSE. `AStratUnitActor` grew a second clock on 2026-09-06:
// `DamageFlashElapsed`, armed by `PlayDamageFlash`, retired by `FinishDamageFlash`, and driven
// between the two by `TickDamageFlash`. It also grew a component -- `Shake`, between `Body` and
// the three markers -- and one arbiter, `UpdateTickEnabled`, which is now the only caller of
// `SetActorTickEnabled(false)` in the class. None of that was executed by any clause.
//
// EVERY CLAUSE BELOW IS ABOUT ARMING OR RETIREMENT AND NOTHING ELSE, AND THAT IS A CONSEQUENCE
// OF A REFUSAL THIS PROJECT ALREADY MADE RATHER THAN A CHOICE MADE HERE.
// `AStratUnitActor::Tick` NEVER RUNS IN A HEADLESS FIXTURE -- `FTestWorldScope` never begins
// play and never ticks -- and `StratMatchReconcile.cpp`'s own banner records that a test-only
// advance seam was asked about for the move tween and REFUSED: *"`Tick` is protected and stays
// so; no test-only advance entry point was asked for or added, and a future clause that wants
// the curve needs a PIE fixture rather than a seam."* That refusal is inherited here verbatim
// and none was asked for. What makes these clauses possible at all is that `CancelDamageFlash`
// IS `FinishDamageFlash` made public -- the SAME four writes the running game reaches on
// completion -- so retirement is reachable without advancing anything.
//
// WHAT THESE CLAUSES DO NOT PIN, WRITTEN OUT IN `StratMatchReconcile.cpp`'S IDIOM BECAUSE A
// GREEN SUITE HERE WILL BE READ AS "THE DAMAGE ALERT WORKS" AND IT DOES NOT SAY THAT. Nothing
// below observes, and nothing below could observe:
//   - THE SHAKE'S MOTION. Not its amplitude, not its frequency, not the ratio of X to Y, not
//     the linear decay, not the continuity of the derivative at a re-arm, not one sampled
//     position between arming and retirement. `TickDamageFlash` is the only writer of a
//     non-zero `Shake` transform and it never runs here, so `Shake` is at exact relative zero
//     on every line of every clause below. `DamageShakeAmplitude` and `DamageShakeFrequency`
//     are unobservable in this project by construction.
//   - THE COLOUR. `BodyOverlayMaterial()` reports WHICH OBJECT is assigned to the overlay
//     channel and nothing else. The suite runs `-nullrhi`; no clause in this project will ever
//     assert that a unit looked red, that the material compiles as an overlay pass, or that it
//     has a shading model an overlay accepts. `DamageFlashMaterial`'s own declaration says
//     whoever authors that asset must confirm it, and a human at the keyboard is the only
//     instrument -- exactly as for `GuidedMarkerZOffset`.
//   - THE WALL-CLOCK DURATION, or that a flash lasts `DamageFlashSeconds`.
//   - THE TWO TRIGGER SITES' TIMING. That a flash fires at the right MOMENT of a real match is
//     `ATourGatedRefreshFlashesNothing`'s subject only for the gate; the tour-step
//     `Current.TargetId` flash in `AdvanceAiPlaybackOneStep` is NOT pinned by anything here,
//     and its own comment records that it is defender-only and misses counter-damage.
//   - WHAT `IsDamageFlashActive()` CANNOT SAY, inherited from its declaration rather than
//     re-learned: it reports a CLOCK. It answers true for a unit flashing with no material
//     assigned, which draws nothing. A clause asserting it is asserting that a clock started.
//
// WHERE THE EXPECTATIONS COME FROM, WHICH IS THE ONLY QUESTION THAT MATTERS. There is not one
// unit id, hex, HP figure, waypoint count or duration written in this file.
//   - THE ROSTER, THE BOARD AND THE ROUTE ARE ALL THE SUBSYSTEM'S OWN. Units come off
//     `BuildViewModel`, hexes come off the same model's `Hexes` array, and the expected
//     waypoint count is `Route.Num()` -- the length of the route THIS FILE HANDED TO
//     `NotePendingMoveRoute`, compared against what the actor armed from it.
//   - THE OVERLAY EXPECTATION IS THE VERY POINTER THE FIXTURE HANDED OVER, not "not null". A
//     `SetOverlayMaterial` writing some other object would be red.
//   - THE "UNCHANGED" EXPECTATIONS ARE READINGS TAKEN OFF THE ACTOR ITSELF A FEW LINES EARLIER,
//     compared with `==` and never with a tolerance. `ADamageFlashDoesNotDisturbTheMoveTween`
//     has no notion of where `Body` ought to be; it only knows the flash must not move it.
//   - THE SHIPPED DEFAULT IS READ BACK, NEVER ASSUMED. `DamageFlashSecondsForFixture()` is
//     asserted `> 0` before every clause that arms and `<= 0` in the clause about the shipped
//     class, so neither direction can go vacuous if the fixture or the C++ default is retuned.
//
// THE CONTROLS ARE LOAD-BEARING AND EVERY ONE IS AN EARLY RETURN. `PlayDamageFlash` REFUSES on
// a null `Shake` and on `DamageFlashSeconds <= 0`, and a refusal is indistinguishable from a
// correct no-op in every assertion these clauses make. So each arming clause first requires
// `HasShake()`, requires a positive duration, and requires the actor NOT already flashing --
// otherwise "it is flashing afterwards" would be a statement about the arrangement.
//
// NO `strat::` FREE FUNCTION IS CALLED ANYWHERE BELOW. That is `LNK2019` outside `StratBridge`
// and `StratRules`, measured 8x. Seeding goes through `FStratBridge` inside the subsystem.
//
// THE WORLD-FREE HALF OF THIS FEATURE IS NOT HERE. `StratDecideDamagedUnits` is a pure function
// over two `FStratViewModel`s and is asserted in `Source/StratUI/Tests/StratDamagedUnitsClauses.cpp`,
// the same split `StratTransientReceiptClauses.cpp` / `StratTransientReceiptCallSite.cpp` draws
// and the same one `StratSoundCueClauses.cpp` was written as a sibling of.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Materials/Material.h"
#include "Materials/MaterialInterface.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
// TEST-ONLY, and required rather than convenient -- see its header: at the shipped C++ defaults
// `PlayDamageFlash` returns having written nothing, and `Body`, `Shake` and the four flash
// properties are all protected.
#include "Tests/StratRouteTweenUnitDouble.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

namespace StratDamageFlash
{
	/** The same harness pin every parity file in this tree carries. No rule decides which side
	 *  moves first, so this is a harness constant and not a rule. */
	static const int32 kFirstSide = 0;

	/**
	 * A HARNESS INTERVAL AND NOT A PACE, on `StratTourExistenceHoldClauses.cpp`'s own reasoning
	 * and copied from it: these worlds are never ticked, so nothing advances a tour except a
	 * clause that asks it to. Sec 2.11.2's 0.5 is not written here.
	 */
	static const float kHarnessPlaybackInterval = 600.0f;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** See `StratMatchReconcile.cpp` on why the world is destroyed before its context.
	 *  Duplicated from the other clause files in this directory rather than shared, which is
	 *  this project's standing posture for a test helper -- sharing would put a header inside a
	 *  `Tests/` directory. */
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

		// THE DOUBLE AND NOT THE SHIPPED CLASS, AND IT IS REQUIRED RATHER THAN CONVENIENT. See
		// its header: `MoveTweenSeconds` and `DamageFlashSeconds` are both `0.0f` in C++, so
		// against `AStratUnitActor::StaticClass()` no tween arms and no flash arms, and every
		// clause below would assert over an actor that refused everything.
		Out.UnitActorClass  = AStratRouteTweenUnitDouble::StaticClass();
		return true;
	}

	/** A transient material, made here so a clause can assert the overlay holds THE VERY
	 *  POINTER it handed over. Never a `/Game/` asset -- nothing renders it and there is no
	 *  reason to spend the fixture exception on it. */
	static UMaterialInterface* MakeFixtureFlashMaterial()
	{
		// THE DEFAULT OUTER IS THE TRANSIENT PACKAGE. `GetTransientPackage()` returns a
		// `UPackage*`, which UE 5.8's `NewObject` overload set no longer accepts as a bare
		// `UObject*` -- measured as `C2672` on 2026-09-06. The no-argument form defaults the
		// outer to `GetTransientPackageAsObject()`, which is the same package.
		return NewObject<UMaterial>();
	}

	/** Every live unit actor in the world as the double, so the four accessors are reachable.
	 *  Read from the world and not from the subsystem's map, for `LiveUnitIds`' reason: only an
	 *  iterator can see an actor the map forgot. */
	static TArray<AStratRouteTweenUnitDouble*> LiveDoubles(UWorld* const World)
	{
		TArray<AStratRouteTweenUnitDouble*> Out;
		if (World == nullptr)
		{
			return Out;
		}
		for (TActorIterator<AStratRouteTweenUnitDouble> It(World); It; ++It)
		{
			AStratRouteTweenUnitDouble* const Actor = *It;
			if (IsValid(Actor) && !Actor->IsActorBeingDestroyed())
			{
				Out.Add(Actor);
			}
		}
		return Out;
	}

	/** How many live unit actors report a flash in flight. The answer both directions of the
	 *  tour-gate clause are about. */
	static int32 CountFlashing(UWorld* const World)
	{
		int32 Count = 0;
		for (const AStratRouteTweenUnitDouble* const Actor : LiveDoubles(World))
		{
			if (Actor->IsDamageFlashActive())
			{
				++Count;
			}
		}
		return Count;
	}

	/** Retires every flash in the world, so a clause's own arming is the only thing its
	 *  assertions can be about. `CancelDamageFlash` is a documented no-op when nothing is
	 *  running, which is what makes this safe to call unconditionally. */
	static void CancelEveryFlash(UWorld* const World)
	{
		for (AStratRouteTweenUnitDouble* const Actor : LiveDoubles(World))
		{
			Actor->CancelDamageFlash();
		}
	}

	/** What every clause below needs before it can measure anything. */
	struct FFlashFixture
	{
		UStratMatchSubsystem* Subsystem = nullptr;
		AStratBoardActor*     Board     = nullptr;

		/** The model as the subsystem applied it. The moving unit stands on `Route[0]`. */
		FStratViewModel AtStart;
		/** The same model with the moving unit standing on `Route.Last()`. */
		FStratViewModel Moved;

		TArray<FIntPoint> Route;
		int32             MovingUnitId = INDEX_NONE;
	};

	/**
	 * Starts a live match with the double, applies the opening so the actors exist and stand
	 * still, and composes a three-hex route for some unit off the board's own hex list.
	 *
	 * IT ASSERTS ITS OWN PRECONDITIONS THROUGH `Test`, so a clause whose arrangement silently
	 * failed reddens where it failed rather than passing over an empty world.
	 *
	 * THE ROUTE'S HEXES ARE THE MODEL'S OWN AND ARE NEVER WRITTEN HERE. Two hexes off
	 * `Model.Hexes` that differ from the unit's own are taken in the order the board reports
	 * them, and their world locations are asserted PAIRWISE DISTINCT before use -- because
	 * `ApplyUnitView` refuses to arm unless the actor genuinely moved and refuses again if the
	 * polyline has no length, and both refusals are silent.
	 */
	static bool Arrange(FAutomationTestBase& Test, UWorld* const World, FFlashFixture& Out)
	{
		Out.Subsystem = World->GetSubsystem<UStratMatchSubsystem>();
		if (!Test.TestNotNull(TEXT("the world has a match subsystem"), Out.Subsystem))
		{
			return false;
		}

		FStratMatchConfig Config;
		FString Error;
		if (!Test.TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
		{
			Test.AddError(Error);
			return false;
		}

		FString StartReason;
		Out.Subsystem->StartMatch(Config, StartReason);
		if (!Test.TestTrue(TEXT("the match is live whatever StartMatch returned"),
				Out.Subsystem->IsMatchLive()))
		{
			Test.AddError(StartReason);
			return false;
		}

		Out.Board = Out.Subsystem->GetBoard();
		if (!Test.TestNotNull(TEXT("the subsystem spawned a board to ask for hex locations"),
				Out.Board))
		{
			return false;
		}

		FString BuildError;
		if (!Test.TestTrue(TEXT("the view model builds"),
				Out.Subsystem->BuildViewModel(Out.AtStart, BuildError)))
		{
			Test.AddError(BuildError);
			return false;
		}
		if (!Test.TestTrue(TEXT("CONTROL: the scenario deploys units"), Out.AtStart.Units.Num() > 0) ||
			!Test.TestTrue(TEXT("CONTROL: the board has hexes"), Out.AtStart.Hexes.Num() > 0))
		{
			return false;
		}

		// THE UNITS ARE PUT ON SCREEN STANDING STILL. This apply SPAWNS them, which takes
		// `SnapToWorldLocation` and arms nothing -- see `ApplyView`'s spawn branch -- and it
		// also SEEDS the subsystem's sound mark, which every clause that plants an HP fall
		// depends on and which `FStratSoundMark`'s first-observation rule makes load-bearing.
		Out.Subsystem->ApplyView(Out.AtStart);

		const FStratUnitView& Mover = Out.AtStart.Units[0];
		Out.MovingUnitId = Mover.UnitId;
		Out.Route.Add(Mover.Hex);
		for (const FStratHexView& Hex : Out.AtStart.Hexes)
		{
			if (Out.Route.Num() >= 3)
			{
				break;
			}
			if (!Out.Route.Contains(Hex.Hex))
			{
				Out.Route.Add(Hex.Hex);
			}
		}
		if (!Test.TestEqual(TEXT("CONTROL: three distinct hexes were found for a route"),
				Out.Route.Num(), 3))
		{
			return false;
		}

		// THE DISTINCTNESS IS ASSERTED IN WORLD SPACE AND NOT IN AXIAL SPACE, because that is
		// the space `ApplyUnitView` decides in: it arms only when the ACTOR moved and only when
		// the arc length is positive, and both are measured off
		// `AStratBoardActor::WorldLocationOfHex`. Two distinct axial pairs mapping to one world
		// point would make every waypoint clause below silently vacuous.
		const FVector P0 = Out.Board->WorldLocationOfHex(Out.Route[0]);
		const FVector P1 = Out.Board->WorldLocationOfHex(Out.Route[1]);
		const FVector P2 = Out.Board->WorldLocationOfHex(Out.Route[2]);
		if (!Test.TestTrue(*FString::Printf(
					TEXT("CONTROL: the three route hexes are three distinct world points -- %s, %s, %s"),
					*P0.ToString(), *P1.ToString(), *P2.ToString()),
				!P0.Equals(P1) && !P1.Equals(P2) && !P0.Equals(P2)))
		{
			return false;
		}

		Out.Moved = Out.AtStart;
		for (FStratUnitView& Unit : Out.Moved.Units)
		{
			if (Unit.UnitId == Out.MovingUnitId)
			{
				Unit.Hex = Out.Route.Last();
			}
		}

		Test.AddInfo(FString::Printf(
			TEXT("unit %d routed (%d,%d) -> (%d,%d) -> (%d,%d)"),
			Out.MovingUnitId,
			Out.Route[0].X, Out.Route[0].Y, Out.Route[1].X, Out.Route[1].Y,
			Out.Route[2].X, Out.Route[2].Y));
		return true;
	}

	/**
	 * Arms the move tween for the fixture's unit and hands back its actor with the polyline
	 * already in flight.
	 *
	 * THE TWEEN IS ARMED THROUGH THE SHIPPING PATH -- `NotePendingMoveRoute` then `ApplyView`
	 * -- and never by writing a member. That is the same route a player command takes, so a
	 * clause that finds the tween disturbed is reporting about the code the game runs.
	 */
	static AStratRouteTweenUnitDouble* ArmTheTween(FAutomationTestBase& Test,
	                                               FFlashFixture&       Fixture)
	{
		Fixture.Subsystem->NotePendingMoveRoute(Fixture.MovingUnitId, Fixture.Route);
		Fixture.Subsystem->ApplyView(Fixture.Moved);

		AStratRouteTweenUnitDouble* const Actor =
			Cast<AStratRouteTweenUnitDouble>(Fixture.Subsystem->FindUnitActor(Fixture.MovingUnitId));
		if (!Test.TestNotNull(TEXT("the unit that moved still has an actor, and it is the double"),
				Actor))
		{
			return nullptr;
		}

		// THE FOUR CONTROLS WITHOUT WHICH EVERY "UNDISTURBED" ASSERTION BELOW IS A TAUTOLOGY.
		// A tween that never armed has no waypoints and a `Body` at zero, and "unchanged
		// across a flash" is then true of nothing.
		if (!Test.TestEqual(TEXT("CONTROL: the polyline armed, one waypoint per noted hex"),
				Actor->GetTweenWaypointCount(), Fixture.Route.Num()))
		{
			return nullptr;
		}
		if (!Test.TestTrue(TEXT("CONTROL: the actor has a Body to be displaced"), Actor->HasBody()))
		{
			return nullptr;
		}
		if (!Test.TestTrue(*FString::Printf(
					TEXT("CONTROL: the picture is genuinely displaced mid-slide -- Body sits at %s"),
					*Actor->BodyRelativeLocation().ToString()),
				!Actor->BodyRelativeLocation().IsZero()))
		{
			return nullptr;
		}
		if (!Test.TestTrue(TEXT("CONTROL: a tween in flight has the actor ticking"),
				Actor->IsActorTickEnabled()))
		{
			return nullptr;
		}
		return Actor;
	}

	/**
	 * The four preconditions of any clause that arms a flash, asserted off the ACTOR rather
	 * than trusted from the double's constructor.
	 *
	 * WITHOUT THESE EVERY ARMING CLAUSE CAN GO VACUOUS SILENTLY. `PlayDamageFlash` refuses on a
	 * null `Shake` and on `DamageFlashSeconds <= 0` and says nothing when it does, and an actor
	 * ALREADY flashing would make "it is flashing afterwards" a statement about the
	 * arrangement rather than about the verb.
	 */
	static bool ReadyToArm(FAutomationTestBase& Test, AStratRouteTweenUnitDouble& Actor)
	{
		if (!Test.TestTrue(TEXT("CONTROL: the actor has the Shake component PlayDamageFlash requires"),
				Actor.HasShake()))
		{
			return false;
		}
		if (!Test.TestTrue(*FString::Printf(
					TEXT("CONTROL: the fixture's DamageFlashSeconds is POSITIVE (%.4f) -- at the "
					     "shipped 0.0f PlayDamageFlash returns having written nothing"),
					Actor.DamageFlashSecondsForFixture()),
				Actor.DamageFlashSecondsForFixture() > 0.0f))
		{
			return false;
		}
		if (!Test.TestFalse(TEXT("CONTROL: no flash is in flight before this clause arms one"),
				Actor.IsDamageFlashActive()))
		{
			return false;
		}
		if (!Test.TestNull(TEXT("CONTROL: the overlay channel is empty before this clause arms"),
				Actor.BodyOverlayMaterial()))
		{
			return false;
		}
		return true;
	}
}

// ---------------------------------------------------------------------------
// T-INT-05 -- ARMING A DAMAGE FLASH STARTS A CLOCK, TURNS THE TICK ON, AND WRITES THE OVERLAY.
//
// THE THREE EFFECTS ARE ASSERTED SEPARATELY BECAUSE THEY FAIL SEPARATELY, and two of the three
// failures are invisible.
//   - NO TICK: `PlayDamageFlash` without `SetActorTickEnabled(true)` arms a clock nothing ever
//     advances. The unit never shakes, and -- worse -- never RETIRES, so it stays red for the
//     rest of the match with `IsDamageFlashActive()` reporting true forever. A build is green.
//   - NO OVERLAY: the shake happens and nothing turns red, which is the exact state
//     `DamageFlashMaterial`'s block describes as the shipped content gap. So an implementation
//     that never calls `SetOverlayMaterial` at all is INDISTINGUISHABLE from a correct one
//     against the shipped Blueprint, and only a fixture that assigns a material can tell them
//     apart. That is why `SetDamageFlashMaterialForFixture` exists.
//   - NO CLOCK: `IsDamageFlashActive()` is `DamageFlashElapsed > 0.0f`, so a flash seeded at
//     exactly `0.0f` would report itself inactive at the instant it is armed. The engineer
//     seeds `KINDA_SMALL_NUMBER` for precisely that reason, and this clause asserts the
//     OBSERVABLE consequence -- active, elapsed strictly positive -- rather than the constant.
//     IT DOES NOT ASSERT `GetDamageFlashElapsed() == 0.0f`, WHICH WOULD BE FALSE.
//
// THE OVERLAY EXPECTATION IS THE POINTER THE FIXTURE MADE, not "not null". A `SetOverlayMaterial`
// handed some other object -- `SideMaterials[Side]`, `FallbackMesh`'s material, anything --
// is red on the same line.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratDamageFlashArmsTest,
	"Stratocracy.StratPlay.T-INT-05.DamageFlashArmingSetsTheClockTickAndOverlay",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratDamageFlashArmsTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDamageFlash;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	FFlashFixture Fixture;
	if (!Arrange(*this, Scope.World, Fixture))
	{
		return false;
	}

	AStratRouteTweenUnitDouble* const Actor =
		Cast<AStratRouteTweenUnitDouble>(Fixture.Subsystem->FindUnitActor(Fixture.MovingUnitId));
	if (!TestNotNull(TEXT("the unit has an actor, and it is the double"), Actor))
	{
		return false;
	}
	if (!ReadyToArm(*this, *Actor))
	{
		return false;
	}

	// AND THE ONE CONTROL `ReadyToArm` CANNOT MAKE: a unit standing still is not ticking, so
	// "the tick is on afterwards" is about the verb rather than about the arrangement.
	if (!TestFalse(TEXT("CONTROL: a unit standing still with no tween is not ticking"),
			Actor->IsActorTickEnabled()))
	{
		return false;
	}

	UMaterialInterface* const Material = MakeFixtureFlashMaterial();
	if (!TestNotNull(TEXT("CONTROL: the fixture made a transient flash material"), Material))
	{
		return false;
	}
	Actor->SetDamageFlashMaterialForFixture(Material);

	Actor->PlayDamageFlash();

	TestTrue(TEXT("T-INT-05: arming reports a flash in flight"), Actor->IsDamageFlashActive());

	// STRICTLY POSITIVE AND NOT EQUAL TO ANY NUMBER. `PlayDamageFlash` seeds
	// `KINDA_SMALL_NUMBER` rather than `0.0f` so `IsDamageFlashActive()` cannot lie on the
	// first frame; the SEED VALUE is an implementation choice this clause does not pin, and the
	// property it exists for -- the clock is running -- is exactly `> 0`.
	TestTrue(*FString::Printf(
			TEXT("T-INT-05: the clock is strictly positive at the instant it is armed (%.8f), "
			     "which is what stops IsDamageFlashActive() from reporting false on the frame "
			     "the flash starts"),
			Actor->GetDamageFlashElapsed()),
		Actor->GetDamageFlashElapsed() > 0.0f);

	TestTrue(TEXT("T-INT-05: arming enables the actor tick -- without this the clock never "
	              "advances, the unit never shakes AND never retires, and it stays red forever"),
		Actor->IsActorTickEnabled());

	TestEqual(TEXT("T-INT-05: Body's OVERLAY channel holds the very material the fixture "
	               "assigned -- the overlay and not slot 0, which ApplyUnitView rewrites on "
	               "every hover refresh"),
		Actor->BodyOverlayMaterial(), Material);

	// THE SHAKE HAS NOT MOVED AND CANNOT HAVE. `TickDamageFlash` is its only non-zero writer
	// and never runs here. Asserted rather than left out, because it is the precondition of
	// `DamageFlashRetiresToExactlyZero` and a reader is entitled to know it was checked.
	TestTrue(TEXT("T-INT-05: arming writes no transform -- Shake is still at exact relative zero"),
		Actor->ShakeRelativeLocation().IsZero());

	return true;
}

// ---------------------------------------------------------------------------
// T-INT-05 -- RETIREMENT PUTS EVERYTHING BACK TO A WRITTEN ZERO, ON EVERY LIVE ACTOR.
//
// `IsZero()` AND NEVER `IsNearlyZero()`, AND THE TOLERANCE IS THE WHOLE SUBJECT.
// `FinishDamageFlash` writes `Shake` through `SetRelativeLocation_Direct` plus
// `UpdateComponentToWorld` rather than through `SetRelativeLocation`, and its own block records
// why: `SetRelativeLocation` is a WORLD-SPACE move that converts back through the parent's
// inverse, so it stores a ROUND TRIP. MEASURED 2026-09-02 on the tween's own retirement -- the
// same call asking for `FVector::ZeroVector` stored an exact zero for one unit and `Y=-0.000`
// for another, differing only in where their actors stood. The residue is ~1e-14 uu, invisible
// on a screen and NOT invisible to `IsZero()`. `IsNearlyZero()` here would be green over the
// defect this clause exists for.
//
// IT RUNS OVER EVERY LIVE UNIT ACTOR AND NOT OVER ONE, WHICH FOLLOWS DIRECTLY FROM THAT
// MEASUREMENT. The residue appeared for unit 13 and not for unit 4 in the same call on the same
// line -- it is a property of the PARENT'S TRANSFORM, so a clause sampling one actor samples
// one board position and could be green by luck. Ten actors on ten hexes is ten samples.
//
// WHAT THIS CLAUSE CANNOT DO, STATED PLAINLY BECAUSE IT IS A REAL WEAKNESS AND NOT A
// FORMALITY. The tween's equivalent clause asserts the picture is genuinely DISPLACED before it
// retires it. This one cannot: `TickDamageFlash` is the only thing that displaces `Shake` and
// never runs headless, so `Shake` is at exact zero both before and after. What the clause still
// measures is the SAME SHAPE the 2026-09-02 residue was measured on -- `CancelRouteSlide`
// writing a zero the component ALREADY HELD, where the round trip still ran and still left a
// residue. So a `SetRelativeLocation` mutant remains reachable here; it is NOT reachable with
// certainty on any given board, and this file does not claim otherwise. The OVERLAY and CLOCK
// halves below are unconditional and have no such caveat.
//
// THE OVERLAY HALF IS NOT VACUOUS AND THAT TAKES A FIXTURE MATERIAL. Every actor is handed one
// and asserted to be carrying it BEFORE the cancel, so `nullptr` afterwards is evidence about
// `FinishDamageFlash` rather than about a channel nothing ever wrote. It also kills the
// retirement that leaves the unit red forever, which is the failure a player would see and no
// build would report.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratDamageFlashRetiresToZeroTest,
	"Stratocracy.StratPlay.T-INT-05.DamageFlashRetiresToExactlyZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratDamageFlashRetiresToZeroTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDamageFlash;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	FFlashFixture Fixture;
	if (!Arrange(*this, Scope.World, Fixture))
	{
		return false;
	}

	const TArray<AStratRouteTweenUnitDouble*> Actors = LiveDoubles(Scope.World);
	if (!TestTrue(TEXT("CONTROL: the opening apply spawned unit actors to retire"),
			Actors.Num() > 0))
	{
		return false;
	}
	AddInfo(FString::Printf(TEXT("retiring a flash on each of %d live unit actors"), Actors.Num()));

	UMaterialInterface* const Material = MakeFixtureFlashMaterial();
	if (!TestNotNull(TEXT("CONTROL: the fixture made a transient flash material"), Material))
	{
		return false;
	}

	for (AStratRouteTweenUnitDouble* const Actor : Actors)
	{
		if (!ReadyToArm(*this, *Actor))
		{
			return false;
		}
		Actor->SetDamageFlashMaterialForFixture(Material);
		Actor->PlayDamageFlash();
	}

	// THE ARMING IS ASSERTED BEFORE ANYTHING IS RETIRED. Without this every assertion after the
	// cancel is satisfied by an actor that never armed -- which is the state a `PlayDamageFlash`
	// that silently refused would leave, and the state the shipped defaults leave.
	for (const AStratRouteTweenUnitDouble* const Actor : Actors)
	{
		if (!TestTrue(*FString::Printf(TEXT("CONTROL: unit %d is flashing before it is retired"),
					Actor->GetUnitId()),
				Actor->IsDamageFlashActive()) ||
			!TestEqual(*FString::Printf(TEXT("CONTROL: unit %d is carrying the overlay before it is retired"),
					Actor->GetUnitId()),
				Actor->BodyOverlayMaterial(), Material) ||
			!TestTrue(*FString::Printf(TEXT("CONTROL: unit %d is ticking before it is retired"),
					Actor->GetUnitId()),
				Actor->IsActorTickEnabled()))
		{
			return false;
		}
	}

	for (AStratRouteTweenUnitDouble* const Actor : Actors)
	{
		Actor->CancelDamageFlash();
	}

	for (const AStratRouteTweenUnitDouble* const Actor : Actors)
	{
		const int32 Id = Actor->GetUnitId();

		TestFalse(*FString::Printf(TEXT("T-INT-05: unit %d reports no flash in flight"), Id),
			Actor->IsDamageFlashActive());

		TestEqual(*FString::Printf(
				TEXT("T-INT-05: unit %d's clock is EXACTLY zero -- written by FinishDamageFlash, "
				     "not approached by a Tick that never ran"), Id),
			Actor->GetDamageFlashElapsed(), 0.0f);

		TestNull(*FString::Printf(
				TEXT("T-INT-05: unit %d's overlay channel is cleared -- a missed clear here "
				     "leaves the unit red for the rest of the match with a green build"), Id),
			Actor->BodyOverlayMaterial());

		TestTrue(*FString::Printf(TEXT("CONTROL: unit %d has a Shake component to be at zero"), Id),
			Actor->HasShake());

		TestTrue(*FString::Printf(
				TEXT("T-INT-05: unit %d's Shake is at EXACTLY relative zero -- IsZero(), no "
				     "tolerance, which is what SetRelativeLocation's ~1e-14 world round-trip "
				     "residue fails and IsNearlyZero() would hide. It sits at %s"),
				Id, *Actor->ShakeRelativeLocation().ToString()),
			Actor->ShakeRelativeLocation().IsZero());

		// THE TICK GOES OFF ONLY BECAUSE NO TWEEN WANTS IT. That qualifier is the whole subject
		// of `RetiringOneClockLeavesTheOtherTicking`; here every actor is standing still, so
		// `UpdateTickEnabled` must answer false and a retirement that forgot to ask is red.
		TestFalse(*FString::Printf(
				TEXT("T-INT-05: unit %d stops ticking, since neither clock wants it"), Id),
			Actor->IsActorTickEnabled());
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-INT-05 -- A DAMAGE FLASH DOES NOT DISTURB A MOVE TWEEN. THIS IS THE CLAUSE THAT PINS THE
// COMPONENT SPLIT.
//
// THE REJECTED DESIGN IS THE MUTANT. `StratUnitActor.h`'s `Shake` block records that summing
// the shake into `Body`'s relative location was considered and refused, because `FinishTween`
// writes `TweenRestOffset` VERBATIM and the exactly-zero-at-rest property would then depend on
// BOTH clocks having retired. An implementation that wrote the shake into `Body` compiles,
// builds green, and passes every existing tween clause -- because no existing clause ever arms
// a flash.
//
// THE KILLING HALF IS THE CANCEL AND NOT THE ARM, AND THE ORDER BELOW IS CHOSEN FOR THAT.
// `PlayDamageFlash` writes no transform at all, so arming alone would leave a `Body`-based
// implementation undetected. `FinishDamageFlash` writes `FVector::ZeroVector` into the shake's
// component -- so on the mutant it writes ZERO INTO `Body`, in the middle of a slide, and the
// unit teleports to its destination hex mid-tween. This clause reads `Body`'s relative location
// before the flash, after arming it, and after retiring it, and compares all three with `==`.
//
// THE WAYPOINT COUNT IS ASSERTED ALONGSIDE IT because the two failures are different: a mutant
// that RESET the polyline and one that merely MOVED the picture would be distinguishable only
// by which of the two assertions reddens, and a reader of a red run should not have to guess.
//
// EVERY EXPECTATION IS A READING TAKEN OFF THIS ACTOR A FEW LINES EARLIER. This clause has no
// opinion about where `Body` ought to be or how many waypoints there ought to be; it knows only
// that a damage flash may not change either.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratDamageFlashLeavesTheTweenAloneTest,
	"Stratocracy.StratPlay.T-INT-05.ADamageFlashDoesNotDisturbTheMoveTween",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratDamageFlashLeavesTheTweenAloneTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDamageFlash;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	FFlashFixture Fixture;
	if (!Arrange(*this, Scope.World, Fixture))
	{
		return false;
	}

	AStratRouteTweenUnitDouble* const Actor = ArmTheTween(*this, Fixture);
	if (Actor == nullptr)
	{
		return false;
	}
	if (!ReadyToArm(*this, *Actor))
	{
		return false;
	}

	const int32   WaypointsBefore = Actor->GetTweenWaypointCount();
	const FVector BodyBefore      = Actor->BodyRelativeLocation();

	UMaterialInterface* const Material = MakeFixtureFlashMaterial();
	Actor->SetDamageFlashMaterialForFixture(Material);

	// ---- 1. arming ---------------------------------------------------------
	Actor->PlayDamageFlash();

	if (!TestTrue(TEXT("CONTROL: the flash really armed, so what follows is about a live flash"),
			Actor->IsDamageFlashActive()))
	{
		return false;
	}

	TestEqual(TEXT("T-INT-05: arming a flash leaves the armed polyline untouched"),
		Actor->GetTweenWaypointCount(), WaypointsBefore);

	TestTrue(*FString::Printf(
			TEXT("T-INT-05: arming a flash does not move the picture -- Body was at %s and is "
			     "at %s"),
			*BodyBefore.ToString(), *Actor->BodyRelativeLocation().ToString()),
		Actor->BodyRelativeLocation() == BodyBefore);

	TestTrue(TEXT("T-INT-05: and the shake's own transform is where the flash's displacement "
	              "would go -- at exact zero, since Tick never runs here"),
		Actor->ShakeRelativeLocation().IsZero());

	// ---- 2. retirement, which is the half that kills the summed-offset design ----
	Actor->CancelDamageFlash();

	if (!TestFalse(TEXT("CONTROL: the flash really retired"), Actor->IsDamageFlashActive()))
	{
		return false;
	}

	TestEqual(TEXT("T-INT-05: retiring a flash leaves the armed polyline untouched"),
		Actor->GetTweenWaypointCount(), WaypointsBefore);

	TestTrue(*FString::Printf(
			TEXT("T-INT-05: retiring a flash does not move the picture EITHER -- Body was at %s "
			     "and is at %s. This is the assertion a shake summed into `Body` fails: "
			     "FinishDamageFlash would write ZeroVector into Body mid-slide and the unit "
			     "would teleport to its destination"),
			*BodyBefore.ToString(), *Actor->BodyRelativeLocation().ToString()),
		Actor->BodyRelativeLocation() == BodyBefore);

	return true;
}

// ---------------------------------------------------------------------------
// T-INT-05 -- RETIRING ONE CLOCK LEAVES THE OTHER TICKING. NO OTHER CLAUSE CAN SEE THIS.
//
// `SetActorTickEnabled` IS A SINGLE BOOLEAN AND TWO FEATURES NOW WANT IT. The concrete failure
// `UpdateTickEnabled` exists to prevent, in that function's own words: a damage flash retiring
// while a move tween is in flight would disable the tick and FREEZE A UNIT HALFWAY BETWEEN TWO
// HEXES, FOREVER, WITH A GREEN BUILD AND NO LOG. The mirror is equally real -- a tween retiring
// mid-flash leaves a unit permanently red and displaced.
//
// THE MUTANT IS ONE TOKEN IN EITHER RETIREMENT PATH: `SetActorTickEnabled(false)` written
// directly instead of `UpdateTickEnabled()`. `FinishTween` carried exactly that line until
// 2026-09-06 and it was correct until the day it was not. BOTH DIRECTIONS ARE ASSERTED, because
// they are two different lines and a fix to one does not fix the other.
//
// WHY NO OTHER CLAUSE CAN SEE IT. `DamageFlashRetiresToExactlyZero` asserts the tick goes OFF,
// and would be GREENER for the mutant, not redder. Every existing tween clause retires a tween
// with no flash running, where the arbiter and the direct write agree. The property only exists
// when both clocks are live at once, and this is the only clause that puts them there.
//
// THE CLOSING CONTROL IS NOT DECORATION. Three assertions here are "the tick is STILL on", and
// all three would be satisfied by an implementation that could never turn the tick off at all.
// So the clause ends by retiring BOTH clocks and requiring the tick to go off -- which is the
// only thing that makes the three preceding assertions mean anything.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratDamageFlashTickArbitrationTest,
	"Stratocracy.StratPlay.T-INT-05.RetiringOneClockLeavesTheOtherTicking",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratDamageFlashTickArbitrationTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDamageFlash;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	FFlashFixture Fixture;
	if (!Arrange(*this, Scope.World, Fixture))
	{
		return false;
	}

	AStratRouteTweenUnitDouble* const Actor = ArmTheTween(*this, Fixture);
	if (Actor == nullptr)
	{
		return false;
	}
	if (!ReadyToArm(*this, *Actor))
	{
		return false;
	}

	const int32 WaypointsArmed = Actor->GetTweenWaypointCount();

	// ---- direction 1: the flash retires while the tween is in flight ----------------
	Actor->PlayDamageFlash();
	if (!TestTrue(TEXT("CONTROL: both clocks are live -- the flash armed"),
			Actor->IsDamageFlashActive()) ||
		!TestTrue(TEXT("CONTROL: both clocks are live -- the polyline is still armed"),
			Actor->GetTweenWaypointCount() > 0) ||
		!TestTrue(TEXT("CONTROL: both clocks are live -- the actor is ticking"),
			Actor->IsActorTickEnabled()))
	{
		return false;
	}

	Actor->CancelDamageFlash();

	TestFalse(TEXT("CONTROL: the flash did retire"), Actor->IsDamageFlashActive());

	TestTrue(TEXT("T-INT-05: the actor is STILL ticking after the flash retired, because the "
	              "move tween still wants it. A FinishDamageFlash writing "
	              "SetActorTickEnabled(false) directly freezes this unit halfway between two "
	              "hexes, forever, with a green build and no log"),
		Actor->IsActorTickEnabled());

	TestEqual(TEXT("T-INT-05: and the tween itself is untouched by the flash's retirement"),
		Actor->GetTweenWaypointCount(), WaypointsArmed);

	// ---- direction 2: the tween retires while the flash is in flight ----------------
	Actor->PlayDamageFlash();
	if (!TestTrue(TEXT("CONTROL: both clocks are live again -- the flash re-armed"),
			Actor->IsDamageFlashActive()) ||
		!TestTrue(TEXT("CONTROL: both clocks are live again -- the actor is ticking"),
			Actor->IsActorTickEnabled()))
	{
		return false;
	}

	// THE SHIPPING VERB AND NOT A MEMBER WRITE. `CancelRouteSlide` is `FinishTween` with the
	// park cleared first, so this exercises the exact line that changed on 2026-09-06.
	Actor->CancelRouteSlide();

	if (!TestEqual(TEXT("CONTROL: the tween did retire -- the polyline is empty"),
			Actor->GetTweenWaypointCount(), 0))
	{
		return false;
	}

	TestTrue(TEXT("T-INT-05: the flash is still in flight after the tween retired"),
		Actor->IsDamageFlashActive());

	TestTrue(TEXT("T-INT-05: and the actor is STILL ticking, because the flash still wants it. "
	              "A FinishTween writing SetActorTickEnabled(false) directly -- which is what "
	              "that line said until 2026-09-06 -- leaves this unit red and displaced "
	              "permanently"),
		Actor->IsActorTickEnabled());

	// ---- the closing control: the arbiter CAN say no -------------------------------
	Actor->CancelDamageFlash();

	TestFalse(TEXT("CONTROL: with both clocks retired the tick finally goes off -- without "
	               "this, all three 'still ticking' assertions above would be satisfied by an "
	               "arbiter that never returns false"),
		Actor->IsActorTickEnabled());

	return true;
}

// ---------------------------------------------------------------------------
// T-INT-05 -- AT THE SHIPPED C++ DEFAULT NOTHING ARMS. THIS IS THE CLAUSE THE WHOLE
// "BIT-IDENTICAL AT DEFAULT" ARGUMENT RESTS ON.
//
// `DamageFlashSeconds` SHIPS AT `0.0f` AND IS A SWITCH RATHER THAN A SETTING, and its own block
// gives the load-bearing half of why: a non-zero default would change the path every existing
// automation fixture runs down. `PlayDamageFlash` is called unconditionally from
// `UStratMatchSubsystem::ApplyView` for every damaged unit and from the tour's Attack arm, so an
// arming path that ignored the `<= 0` guard would START TICKING UNIT ACTORS IN FIXTURES ACROSS
// THIS DIRECTORY -- and the failure would surface as unrelated clauses moving, not as this
// feature breaking.
//
// THE FIXTURE IS A SUBCLASS THAT WRITES NOTHING, AND THE DISTINCTION MATTERS.
// `AStratShippedDefaultUnitDouble` declares no constructor at all, so there is nowhere a
// property could be set; it adds five accessors and that is the whole of it. The clause
// nonetheless READS `DamageFlashSecondsForFixture()` BACK and requires it `<= 0` -- because
// "the default is zero" is the precondition of everything below, and a clause that assumed it
// would go silently vacuous the day somebody gave the property a non-zero initializer.
//
// A RAW `AStratUnitActor` IS SPAWNED BESIDE IT AS THE CONTROL ON THE FIXTURE ITSELF. The two
// assertions the shipped class can answer without an accessor -- inactive, not ticking -- are
// made against the real class, so the clause is not resting entirely on a subclass's claim to
// be equivalent to it.
//
// THE OVERLAY ASSERTION NEEDS A MATERIAL AND WOULD OTHERWISE BE THE THING IT IS TESTING FOR.
// `PlayDamageFlash` writes the overlay only when `DamageFlashMaterial != nullptr`, so against
// an unconfigured actor the overlay stays null WHETHER OR NOT the guard is honoured -- green
// over the exact mutant. So a material is assigned first, the duration is left at the shipped
// default, and `nullptr` afterwards is evidence about THE GUARD.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratNoDamageFlashAtShippedDefaultTest,
	"Stratocracy.StratPlay.T-INT-05.NoDamageFlashAtTheShippedDefault",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratNoDamageFlashAtShippedDefaultTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDamageFlash;

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	// ---- the real shipped class, on the two questions it can answer ----------------
	AStratUnitActor* const Shipped =
		Scope.World->SpawnActor<AStratUnitActor>(AStratUnitActor::StaticClass(),
			FTransform::Identity);
	if (!TestNotNull(TEXT("a raw AStratUnitActor spawned"), Shipped))
	{
		return false;
	}
	if (!TestFalse(TEXT("CONTROL: a freshly spawned unit actor is not ticking"),
			Shipped->IsActorTickEnabled()))
	{
		return false;
	}

	Shipped->PlayDamageFlash();

	TestFalse(TEXT("T-INT-05: PlayDamageFlash on the SHIPPED class arms no flash"),
		Shipped->IsDamageFlashActive());
	TestEqual(TEXT("T-INT-05: and leaves the clock at exactly zero"),
		Shipped->GetDamageFlashElapsed(), 0.0f);
	TestFalse(TEXT("T-INT-05: and enables no tick -- which is what keeps every existing fixture "
	               "in this directory on the path it was written against"),
		Shipped->IsActorTickEnabled());

	// ---- and the accessor subclass, for the overlay and the Shake ------------------
	AStratShippedDefaultUnitDouble* const Probe =
		Scope.World->SpawnActor<AStratShippedDefaultUnitDouble>(
			AStratShippedDefaultUnitDouble::StaticClass(), FTransform::Identity);
	if (!TestNotNull(TEXT("the shipped-default accessor double spawned"), Probe))
	{
		return false;
	}

	if (!TestTrue(*FString::Printf(
				TEXT("CONTROL: DamageFlashSeconds really is the OFF value on this actor (%.4f) "
				     "-- everything below is vacuous if it is not"),
				Probe->DamageFlashSecondsForFixture()),
			Probe->DamageFlashSecondsForFixture() <= 0.0f))
	{
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: it HAS a Shake component, so a refusal below is about the "
	                   "duration guard and not about a missing component"),
			Probe->HasShake()))
	{
		return false;
	}

	UMaterialInterface* const Material = MakeFixtureFlashMaterial();
	if (!TestNotNull(TEXT("CONTROL: the fixture made a transient flash material"), Material))
	{
		return false;
	}
	Probe->SetDamageFlashMaterialForFixture(Material);

	if (!TestNull(TEXT("CONTROL: the overlay channel is empty before the refused call"),
			Probe->BodyOverlayMaterial()))
	{
		return false;
	}

	Probe->PlayDamageFlash();

	TestFalse(TEXT("T-INT-05: no flash is armed at the shipped default"),
		Probe->IsDamageFlashActive());
	TestEqual(TEXT("T-INT-05: the clock stays at exactly zero"),
		Probe->GetDamageFlashElapsed(), 0.0f);
	TestFalse(TEXT("T-INT-05: no tick is enabled at the shipped default"),
		Probe->IsActorTickEnabled());
	TestNull(TEXT("T-INT-05: and the overlay channel is NOT written, even though a material IS "
	              "assigned -- which is the half that is evidence about the `<= 0` guard rather "
	              "than about an unconfigured actor"),
		Probe->BodyOverlayMaterial());
	TestTrue(TEXT("T-INT-05: and Shake keeps the exact zero the constructor gave it"),
		Probe->ShakeRelativeLocation().IsZero());

	return true;
}

// ---------------------------------------------------------------------------
// T-INT-05 -- A REFRESH ARRIVING WHILE THE TOUR HOLDS EXISTENCE FLASHES NOTHING.
//
// WHY THE GATE IS THERE, IN `ApplyView`'S OWN WORDS: the damage diff spans the WHOLE hand-over
// while the tour is about to show it step by step, so an ungated flash would fire every damaged
// unit at hand-over and then again per step. The mutant is the `for (DamagedId : Damaged)` loop
// placed OUTSIDE the `if (!bTourExistenceHeld)` block -- one indentation level, no compiler
// complaint, and a double flash that a green suite would never mention.
//
// THE GATE IS PRIVATE AND IS OBSERVED THROUGH `IsAiPlaybackRunning()`, WHICH IS NOT A GUESS.
// `bTourExistenceHeld` is raised in `BeginAiPlayback` and lowered in `EndAiPlaybackTour`, which
// is the same pair `IsAiPlaybackRunning()` brackets; `StratTourExistenceHoldClauses.cpp` runs
// on the identical correspondence. The world is never ticked and the harness interval is ten
// minutes, so nothing ends the tour except a clause asking it to.
//
// THE CONTROL IS THE WHOLE CLAUSE, AND IT RUNS FIRST. "No unit flashed" is the answer a
// fixture that CANNOT flash gives -- a wrong `UnitActorClass`, a mark that was never seeded, an
// HP plant that did not take, a decider that reports nothing. So the identical operation is
// performed BEFORE the tour, with the gate down, and is required to flash. Only then is the
// same operation performed with the gate up and required to flash nothing. The two halves
// differ in ONE fact: whether a tour is running.
//
// THE PLANT IS ONE SUBTRACTION AND IT IS A PLANT, NEVER AN EXPECTATION, on
// `StratSoundCueClauses.cpp`'s stated precedent: nothing in this tree publishes "an HP value
// strictly below this one", so a fall must be constructed. The order is asserted before use and
// no HP figure is asserted anywhere.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratTourGatedRefreshFlashesNothingTest,
	"Stratocracy.StratPlay.T-INT-05.ATourGatedRefreshFlashesNothing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratTourGatedRefreshFlashesNothingTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDamageFlash;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	// THE CLOSING EndTurn OF THE TURN THAT WON THE GAME, declared for the reason
	// `StratTourExistenceHoldClauses.cpp` records: a result lands mid-turn and the AI's own
	// EndTurn is then refused. Declared from that file's measured run on the same fixture
	// shape, not speculatively -- `Occurrences 0` FAILS a clause where the message never fires.
	AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Match = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Match))
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

	// BOTH SIDES AI, SO A WHOLE Sec 2.8 GAME RESOLVES INSIDE ONE SYNCHRONOUS `RunAiTurnsNow` --
	// which is the hand-over that raises the gate. THE SIDES ARE READ OFF THE SCENARIO'S OWN
	// MODEL through a probe match and are not written here, exactly as
	// `StratTourExistenceHoldClauses.cpp` does it: a hand-written `{0, 1}` would be this file
	// deciding the scenario's roster.
	{
		FStratMatchConfig Probe = Config;
		Match->StartMatch(Probe, Error);
		if (!TestTrue(TEXT("the probe match is live"), Match->IsMatchLive()))
		{
			AddError(Error);
			return false;
		}
		FStratViewModel ProbeModel;
		if (!TestTrue(TEXT("the probe model builds"), Match->BuildViewModel(ProbeModel, Error)))
		{
			AddError(Error);
			return false;
		}
		TArray<int32> Sides;
		for (const FStratUnitView& Unit : ProbeModel.Units)
		{
			Sides.AddUnique(Unit.Side);
		}
		Sides.Sort();
		if (!TestTrue(TEXT("CONTROL: the shipped scenario deploys units for two sides"),
				Sides.Num() >= 2))
		{
			return false;
		}
		UDataTable* const Units = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		Config.AiSides               = Sides;
		Config.AiBuildlistUnitIds    = Units != nullptr ? Units->GetRowNames() : TArray<FName>();
		Config.AiPlaybackStepSeconds = kHarnessPlaybackInterval;
	}

	FString StartReason;
	Match->StartMatch(Config, StartReason);
	if (!TestTrue(TEXT("the AI-vs-AI match is live"), Match->IsMatchLive()))
	{
		AddError(StartReason);
		return false;
	}

	// ---- 1. THE CONTROL: the gate is DOWN and the same operation MUST flash ---------
	if (!TestFalse(TEXT("CONTROL: no tour is running yet, so the gate is down"),
			Match->IsAiPlaybackRunning()))
	{
		return false;
	}

	CancelEveryFlash(Scope.World);
	if (!TestEqual(TEXT("CONTROL: nothing is flashing before the control refresh"),
			CountFlashing(Scope.World), 0))
	{
		return false;
	}

	const FStratViewModel Opening = Match->GetViewModel();
	if (!TestTrue(TEXT("CONTROL: the applied opening carries units"), Opening.Units.Num() > 0))
	{
		return false;
	}

	FStratViewModel Hurt = Opening;
	Hurt.Units[0].Hp = Opening.Units[0].Hp - 1;
	if (!TestTrue(TEXT("CONTROL: the planted reading is STRICTLY BELOW the model's own"),
			Hurt.Units[0].Hp < Opening.Units[0].Hp))
	{
		return false;
	}

	Match->ApplyView(Hurt);

	AStratRouteTweenUnitDouble* const HurtActor =
		Cast<AStratRouteTweenUnitDouble>(Match->FindUnitActor(Opening.Units[0].UnitId));
	if (!TestNotNull(TEXT("CONTROL: the damaged unit has an actor"), HurtActor))
	{
		return false;
	}
	if (!TestTrue(*FString::Printf(
				TEXT("CONTROL: WITH THE GATE DOWN the identical refresh DOES flash unit %d. "
				     "Without this, everything below is the answer a fixture that cannot flash "
				     "at all would give"),
				Opening.Units[0].UnitId),
			HurtActor->IsDamageFlashActive()))
	{
		return false;
	}

	// ---- 2. raise the gate ----------------------------------------------------------
	CancelEveryFlash(Scope.World);

	FString AiReason;
	Match->RunAiTurnsNow(AiReason);

	if (!TestTrue(*FString::Printf(
				TEXT("CONTROL: the hand-over armed a tour, which is what raises the existence "
				     "hold -- RunAiTurnsNow said '%s'"), *AiReason),
			Match->IsAiPlaybackRunning()))
	{
		return false;
	}

	// THE HAND-OVER'S OWN FLASHES ARE RETIRED FIRST. `AdvanceAiPlaybackOneStep` flashes the
	// defender of an Attack step and `BeginAiPlayback` reaches it once, so a residue from the
	// tour itself would be attributed to the refresh below.
	CancelEveryFlash(Scope.World);
	if (!TestEqual(TEXT("CONTROL: nothing is flashing before the gated refresh"),
			CountFlashing(Scope.World), 0))
	{
		return false;
	}

	// ---- 3. the gated refresh -------------------------------------------------------
	const FStratViewModel Toured = Match->GetViewModel();
	if (!TestTrue(TEXT("CONTROL: the post-hand-over applied model still carries units"),
			Toured.Units.Num() > 0))
	{
		return false;
	}

	FStratViewModel HurtAgain = Toured;
	HurtAgain.Units[0].Hp = Toured.Units[0].Hp - 1;
	if (!TestTrue(TEXT("CONTROL: the second planted reading is STRICTLY BELOW its own model's"),
			HurtAgain.Units[0].Hp < Toured.Units[0].Hp))
	{
		return false;
	}

	Match->ApplyView(HurtAgain);

	TestTrue(TEXT("CONTROL: the tour is still running, so the gate was up for that refresh"),
		Match->IsAiPlaybackRunning());

	TestEqual(*FString::Printf(
			TEXT("T-INT-05: a refresh arriving while the tour holds existence flashes NOTHING "
			     "-- %d unit actors report a flash in flight. The mutant is the damage loop "
			     "placed outside `if (!bTourExistenceHeld)`, which double-flashes every damaged "
			     "unit: once at hand-over and again per step"),
			CountFlashing(Scope.World)),
		CountFlashing(Scope.World), 0);

	return true;
}
