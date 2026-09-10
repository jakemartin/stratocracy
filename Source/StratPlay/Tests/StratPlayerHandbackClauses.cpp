// GATE-HANDBACK -- the moment play comes back to a human seat, and the camera that follows it.
//
// The local ID is minted for this surface by the 2026-09-07 ruling in
// `Tools/architect/state/global.md`, which mints no GDD acceptance ID and claims none. Its
// subject is: the predicate `StratHandsBackToPlayer`; the latch `bPlayerHandbackPending` and
// the verb `NotePlayerTurnBeganIfDue` firing once per hand-back and never on a match-ending
// transition; the pure `StratCentroidHexOfSide`; `UStratMatchSubsystem::
// RecenterCameraOnViewingSide`'s call into `AStratBoardActor::WorldLocationOfHex` and
// `AStratCameraPawn::FocusWorldLocation`, including that the recentre preserves the viewer's
// zoom; and `FStratMatchConfig::bRecenterCameraOnPlayerTurn`'s default and its opt-out.
//
// **THE `PlayerTurnBegan` CUE ITSELF IS NOT THIS FILE'S.** It rides `GATE-AUDIO`, in the three
// files that name already pins -- `Source/StratUI/Tests/StratSoundCueClauses.cpp` for the
// never-decided property, `Source/StratPlay/Tests/StratSoundDirectorCallSite.cpp` for the
// once-per-hand-back firing and the payload, and
// `Source/StratPlay/Tests/StratShippedSoundBankParity.cpp` for the bank slot. Two different
// technical facts share one trigger, which is the split the ruling draws in terms. This file
// USES the cue as an instrument -- it is the only observable the latch has -- and asserts
// nothing about what it carries.
//
// ---------------------------------------------------------------------------------------
// WHY THIS FILE IS IN `StratPlay/Tests` AND NOT NEXT DOOR. Every symbol above is `StratPlay`'s.
// `StratUI` is BELOW `StratPlay` and the arrow does not reverse, so a clause there could name
// none of them. No `strat::` free function is called anywhere below; that would be `LNK2019`,
// measured 8x.
//
// THE FILE SPLITS IN TWO AND THE SPLIT IS THE POINT.
//
//   THE PURE HALF -- clauses 1 to 6 -- takes no world, no PIE, no timer, no camera and no
//   subsystem. `StratHandsBackToPlayer` and `StratCentroidHexOfSide` were extracted as free
//   functions precisely so that every arm of the hand-back condition and every arm of the
//   centroid is drivable headlessly, and their declarations say so. A clause that runs on every
//   build is worth more than one that needs a world, so as much as can be pushed down here is.
//
//   THE LIVE HALF -- clauses 7 to 9 -- is the residue that structurally cannot be. The teardown
//   ordering is a fact about two functions' statement order; the camera is a fact about a
//   possessed pawn. Neither has a world-free spelling.
//
// **CLAUSES 8 AND 9 ARE THE FIRST IN THIS PROJECT TO POSSESS AN `AStratCameraPawn`.** Every
// other fixture in this tree has none, which is why `FocusPlaybackStep` -- the older of the two
// callers of `FindCameraPawn` -- has never been pinned by anything, and why
// `FStratMatchConfig::bRecenterCameraOnPlayerTurn`'s own declaration can truthfully say the
// new flag changes no existing fixture. `FindCameraPawn`'s chain is world -> first
// PlayerController -> possessed pawn -> cast, so the fixture below owes all four links: the
// world must have had `InitializeActorsForPlay` run (without it a spawned controller never
// enters `UWorld::PlayerControllerList` and `GetFirstPlayerController` returns null -- measured
// in `StratMatchLifecycle.cpp` and again in `StratCommandBarOptionsBindClauses.cpp`), the
// controller must exist, it must POSSESS the pawn, and the pawn must be an `AStratCameraPawn`.
//
// WHERE THE EXPECTATIONS COME FROM.
//   - The hand-back condition is never spelled out in this file. Every place a clause needs to
//     know whether a moment is a hand-back it asks `StratHandsBackToPlayer`, which is the
//     module's own single statement of it.
//   - The camera's expected XY is `AStratBoardActor::WorldLocationOfHex` -- the project's only
//     axial -> world conversion -- applied to the hex `StratCentroidHexOfSide` returns for the
//     model the subsystem applied. There is not one world coordinate, hex, side index or unit
//     count written anywhere below.
//   - The camera's expected Z and arm length are READ OFF THE PAWN immediately before the
//     hand-back and compared to themselves afterwards. Nothing assumes what a camera's height
//     or zoom is.
//   - The one place a figure IS written is the two hand-built centroid fixtures, and it is
//     written because the INPUT is the thing being controlled: no scenario in this project puts
//     an army on a mean that lands on a half. The EXPECTATION there is still not written -- it
//     is `FMath::RoundToInt` over the mean, the same engine function `StratCentroidHexOfSide`
//     names in its own declaration, so the clause pins "it rounds" rather than "0.5 goes up".
//
// THE PACING CONFIGURATION IS NAMED IN EVERY LIVE CLAUSE, on `StratAiPlaybackClauses.cpp`'s
// standing practice. `AiPlaybackStepSeconds` ships at `0.0f`, so no tour runs in a headless
// fixture unless one is asked for -- and the hand-back lands in TWO different places depending
// on that one value, so a clause that did not say which it was driving would not know which
// half of the feature it had measured.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Math/IntPoint.h"
#include "Math/UnrealMathUtility.h"
#include "Math/Vector.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratCameraPawn.h"
#include "StratMatchSubsystem.h"
#include "StratPlayerController.h"
#include "StratSoundCues.h"
#include "StratSoundDirector.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

// THE BRIDGE, INCLUDED FROM A `.cpp` AND FROM NOWHERE ELSE. This file declares no
// reflected type, so the vendored `strat` headers it drags in are never put in front of
// UHT -- the constraint `StratBridge.h` states in terms. Nothing below CALLS a `strat::`
// free function; the pure clauses use `FStratBridge` only to seed a real projection.
#include "StratBridge.h"

namespace StratPlayerHandbackClauses
{
	/** The same harness pin every parity file in this tree carries. No rule in this project
	 *  decides which side moves first, so this is a harness constant and not a rule. */
	static const int32 kFirstSide = 0;

	/**
	 * A HARNESS INTERVAL AND NOT A PACE, on `StratTourExistenceHoldClauses.cpp`'s reasoning
	 * verbatim. These worlds are never ticked, so nothing advances a tour except a clause that
	 * asks it to. Sec 2.11.2's 0.5 is not written here, on `AiPlaybackStepSeconds`' own recorded
	 * ruling that the pace has one home and a test is not it. What this value selects is only
	 * that a tour is ARMED, which is the condition the teardown clause needs.
	 */
	static const float kHarnessPlaybackInterval = 600.0f;

	/**
	 * The tolerance on a world-space comparison, and it is here because an EXACT one would be a
	 * claim about float storage rather than about the camera.
	 *
	 * `AStratCameraPawn::FocusWorldLocation` calls `SetActorLocation`, which stores a WORLD
	 * location by round-tripping it through the actor's transform. This project has already
	 * measured that such a round trip cannot be relied on to store an exact value. A hundredth
	 * of an Unreal unit is four orders of magnitude below `AStratBoardActor`'s hex size, so a
	 * camera one hex out is still red by an enormous margin.
	 */
	static const double kWorldTolerance = 0.01;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	static FString ScenarioPath()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"), TEXT("ferrum_crossing.json"));
	}

	/**
	 * A real, projected view model with no world anywhere near it.
	 *
	 * THE PURE CLAUSES USE THIS RATHER THAN A HAND-BUILT STRUCT wherever the fields they drive
	 * are ones the rules module owns. `bHasResult` and `SideToMove` are two of those: a
	 * hand-built model would let this file decide what "a live match with a human seat" looks
	 * like, and the positive arm of `StratHandsBackToPlayer` would then be asserting against
	 * this file's own idea of an opening position.
	 */
	static bool SeedAndProject(FStratBridge& Bridge, FStratViewModel& OutModel, FString& OutError)
	{
		UDataTable* const Units   = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		UDataTable* const Terrain = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
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

		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath(), kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}

		return StratBuildViewModel(Bridge, /*ViewingSide*/ kFirstSide, OutModel, OutError);
	}

	/** See `StratMatchConclusion.cpp` on why `InitializeActorsForPlay` is not decoration and
	 *  why the world is destroyed before its context. Duplicated from the other clause files in
	 *  this directory rather than shared, which is this project's standing posture for a test
	 *  helper -- sharing would put a header inside a `Tests/` directory. */
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

		FTestWorldScope(const FTestWorldScope&)            = delete;
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
		Out.UnitActorClass  = AStratUnitActor::StaticClass();

		// NO `SoundBank`, WHICH IS THE SHIPPED CONFIGURATION. Every emission this file observes
		// therefore records `EStratSoundDisposition::NoBank`, which is a full named answer --
		// `UStratSoundDirector::EmitCue` records first and plays second, with no early return
		// on any path, and `StratSoundDirectorCallSite.cpp`'s first clause is what keeps that
		// true. This file counts cues and never asks whether one was audible.
		return true;
	}

	/**
	 * ONE AI SEAT AND ONE HUMAN SEAT, WITH THE AI MOVING FIRST -- the only shape in which a
	 * hand-back can be observed at all.
	 *
	 * AN AI-VS-AI CONFIG CANNOT STAND IN FOR IT, AND THAT IS STRUCTURAL RATHER THAN A MATTER OF
	 * DEGREE. `StratHandsBackToPlayer` is `!bHasResult && !AiSides.Contains(SideToMove)`, so
	 * with every side in `AiSides` the second term refuses at every possible moment and no
	 * fixture built on that shape could tell the feature from a deleted one.
	 *
	 * THE SIDES ARE READ OFF THE SCENARIO'S OWN VIEW MODEL, on `StratSoundDirectorCallSite.cpp`
	 * and `StratTourExistenceHoldClauses.cpp`'s shared pattern and for their reason: it starts
	 * a PROBE match, reads which sides the scenario actually deploys units for, and hands back
	 * a config the caller starts for real. A hand-written `{1}` would be this file deciding the
	 * scenario's roster. The LAST side plays AI and moves first; the FIRST side is the human
	 * seat and is what the screen is drawn for, so `RunAiTurnsNow` runs one AI turn and exits
	 * its loop on `!IsSideAi(SideToMove)` -- which is the hand-over.
	 *
	 * EVERY CALLER STILL VERIFIES THE HAND-BACK HAPPENED, through `StratHandsBackToPlayer`
	 * itself rather than by trusting this arrangement. A scenario arranged differently would
	 * otherwise give a green clause that observed nothing.
	 */
	static bool MakeOneAiSideConfig(UStratMatchSubsystem& Match, const float PlaybackStepSeconds,
	                                FStratMatchConfig& Out, FString& OutError)
	{
		if (!MakeConfig(Out, OutError))
		{
			return false;
		}

		FStratMatchConfig Probe = Out;
		Match.StartMatch(Probe, OutError);
		if (!Match.IsMatchLive())
		{
			return false;
		}

		FStratViewModel Model;
		if (!Match.BuildViewModel(Model, OutError))
		{
			return false;
		}

		TArray<int32> Sides;
		for (const FStratUnitView& Unit : Model.Units)
		{
			Sides.AddUnique(Unit.Side);
		}
		Sides.Sort();
		if (Sides.Num() < 2)
		{
			OutError = TEXT("the shipped scenario deploys units for fewer than two sides");
			return false;
		}

		UDataTable* const Units = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));

		Out.AiSides               = { Sides.Last() };
		Out.FirstSide             = Sides.Last();
		Out.ViewingSide           = Sides[0];
		Out.AiBuildlistUnitIds    = Units != nullptr ? Units->GetRowNames() : TArray<FName>();
		Out.AiPlaybackStepSeconds = PlaybackStepSeconds;
		return true;
	}

	static int32 CountOfCue(const TArray<FStratSoundEmissionRecord>& Records,
	                        const EStratSoundCue                     Cue)
	{
		int32 Count = 0;
		for (const FStratSoundEmissionRecord& Record : Records)
		{
			if (Record.Cue == Cue)
			{
				++Count;
			}
		}
		return Count;
	}

}

// ---------------------------------------------------------------------------
// GATE-HANDBACK (1) -- A CONCLUDED MATCH IS NOT A HAND-BACK, ON THAT TERM ALONE.
//
// THE SEAT TERM IS HELD OPEN SO THAT ONLY ONE TERM CAN BE WHAT REFUSED. `AiSides` is EMPTY,
// which by the function's own declaration makes every moment a hand-back -- "an empty list is a
// hot seat, in which every turn IS a human's". So a refusal here can only be `bHasResult`, and
// a mutant that dropped that term is red rather than merely differently green.
//
// WHY THE TERM MATTERS AT ALL: the rules module leaves `sideToMove` naming a seat after a
// Sec 2.8 result. Without `!bHasResult` a match that ended on the AI's killing blow would
// announce the player's turn beginning, and move their camera, half a second before the verdict
// screen appeared.
//
// THE POSITIVE ARM IS IN CLAUSE 3 AND IS NOT OPTIONAL -- without it this clause and clause 2
// both pass over a body of `return false;`.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratHandbackRefusesConcludedTest,
	"Stratocracy.StratPlay.GATE-HANDBACK.HandsBackToPlayerRefusesAConcludedMatch",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratHandbackRefusesConcludedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPlayerHandbackClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds, with no world anywhere"),
			SeedAndProject(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	// THE SEAT TERM, HELD OPEN. Empty by this function's own contract means "hot seat", so it
	// can never be the term that refuses below.
	const TArray<int32> NoAiSeats;

	if (!TestFalse(TEXT("CONTROL: the opening is not concluded, through the module's own "
	                    "predicate rather than by reading the bool back"),
			StratMatchIsConcluded(Model)))
	{
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: and with no AI seat the opening IS a hand-back -- so the seat "
	                   "term is open and only the result term can close it"),
			StratHandsBackToPlayer(Model, NoAiSeats)))
	{
		return false;
	}

	FStratViewModel Concluded  = Model;
	Concluded.Match.bHasResult = true;

	if (!TestTrue(TEXT("CONTROL: and the planted model IS concluded by that same predicate"),
			StratMatchIsConcluded(Concluded)))
	{
		return false;
	}
	if (!TestEqual(TEXT("CONTROL: the plant moved nothing else -- the side to move is "
	                    "unchanged, so the seat term is still open"),
			Concluded.Match.SideToMove, Model.Match.SideToMove))
	{
		return false;
	}

	TestFalse(*FString::Printf(
			TEXT("GATE-HANDBACK: a concluded match is not a hand-back even though `sideToMove` "
			     "still names seat %d. Sec 2.8 leaves that field set, so without the "
			     "`!bHasResult` term the player's turn would be announced on top of the "
			     "verdict"),
			Concluded.Match.SideToMove),
		StratHandsBackToPlayer(Concluded, NoAiSeats));

	return true;
}

// ---------------------------------------------------------------------------
// GATE-HANDBACK (2) -- AN AI SEAT IS NOT A HAND-BACK, ON THAT TERM ALONE.
//
// THE MIRROR OF CLAUSE 1, AND THE OTHER TERM IS HELD OPEN THE SAME WAY: the model is the live
// opening, so `bHasResult` is false and cannot be what refuses. The AI seat is the model's own
// `SideToMove` -- read, not written -- so this file does not decide which seat the scenario
// opens on.
//
// THE SECOND HALF IS THE ONE THAT CATCHES `Contains` USED BACKWARDS. A list containing SOME
// OTHER side must still hand back; an implementation that refused whenever `AiSides` was
// non-empty would pass the first assertion and fail this one.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratHandbackRefusesAiSeatTest,
	"Stratocracy.StratPlay.GATE-HANDBACK.HandsBackToPlayerRefusesAnAiSeat",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratHandbackRefusesAiSeatTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPlayerHandbackClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds, with no world anywhere"),
			SeedAndProject(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	if (!TestFalse(TEXT("CONTROL: the opening is not concluded, so the result term is open and "
	                    "only the seat term can close it"),
			StratMatchIsConcluded(Model)))
	{
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: the projection deploys at least two seats, so there is a side "
	                   "that is NOT the one to move"),
			Model.Sides.Num() >= 2))
	{
		return false;
	}

	// READ OFF THE MODEL, NOT WRITTEN. Which seat the shipped scenario opens on is the
	// scenario's business.
	const int32 SeatToMove = Model.Match.SideToMove;

	int32 SomeOtherSeat = INDEX_NONE;
	for (int32 Index = 0; Index < Model.Sides.Num(); ++Index)
	{
		if (Index != SeatToMove)
		{
			SomeOtherSeat = Index;
			break;
		}
	}
	if (!TestTrue(TEXT("CONTROL: and that other seat was found"), SomeOtherSeat != INDEX_NONE))
	{
		return false;
	}

	TestFalse(*FString::Printf(
			TEXT("GATE-HANDBACK: a moment whose side to move (%d) is an AI seat is not a "
			     "hand-back -- the AI is about to play, and announcing the player's turn here "
			     "would be exactly wrong"),
			SeatToMove),
		StratHandsBackToPlayer(Model, { SeatToMove }));

	TestTrue(*FString::Printf(
			TEXT("GATE-HANDBACK: and an `AiSides` list naming some OTHER seat (%d) does not "
			     "refuse -- this is the assertion a `Contains` used backwards, or a "
			     "`!AiSides.IsEmpty()` standing in for it, fails"),
			SomeOtherSeat),
		StratHandsBackToPlayer(Model, { SomeOtherSeat }));

	return true;
}

// ---------------------------------------------------------------------------
// GATE-HANDBACK (3) -- AND A LIVE MATCH WITH A HUMAN SEAT TO MOVE **IS** ONE.
//
// **THIS CLAUSE IS NOT OPTIONAL AND IS THE REASON THE OTHER TWO MEAN ANYTHING.** Clauses 1 and
// 2 both assert a FALSE, and a body of `return false;` satisfies both of them completely. This
// is the only clause in the set that a dead predicate fails.
//
// BOTH TERMS ARE MEASURED AS OPEN FIRST, each through the module's own reader -- the result
// term through `StratMatchIsConcluded`, the seat term through an `AiSides` list built by
// excluding the model's own `SideToMove`. So a green here is not an accident of a scenario that
// happened to satisfy one of them.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratHandbackAcceptsHumanSeatTest,
	"Stratocracy.StratPlay.GATE-HANDBACK.HandsBackToPlayerAcceptsALiveHumanSeat",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratHandbackAcceptsHumanSeatTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPlayerHandbackClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds, with no world anywhere"),
			SeedAndProject(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	if (!TestFalse(TEXT("CONTROL: the result term is open -- the opening is not concluded"),
			StratMatchIsConcluded(Model)))
	{
		return false;
	}

	// EVERY SEAT EXCEPT THE ONE TO MOVE. A NON-EMPTY LIST DELIBERATELY: an empty one would make
	// the seat term trivially open by the function's hot-seat rule, and this clause would then
	// be green over an implementation that ignored `AiSides` entirely.
	TArray<int32> AiSeats;
	for (int32 Index = 0; Index < Model.Sides.Num(); ++Index)
	{
		if (Index != Model.Match.SideToMove)
		{
			AiSeats.Add(Index);
		}
	}
	if (!TestTrue(TEXT("CONTROL: the AI seat list is NOT empty, so the seat term is being "
	                   "genuinely evaluated rather than passed the hot-seat shortcut"),
			AiSeats.Num() > 0))
	{
		return false;
	}

	TestTrue(*FString::Printf(
			TEXT("GATE-HANDBACK: a live match whose side to move (%d) is not among the %d AI "
			     "seat(s) IS a hand-back. Without this arm the two refusal clauses are both "
			     "satisfied by `return false;` and the whole predicate could be deleted"),
			Model.Match.SideToMove, AiSeats.Num()),
		StratHandsBackToPlayer(Model, AiSeats));

	return true;
}

// ---------------------------------------------------------------------------
// GATE-HANDBACK (4) -- A SIDE WITH NO UNITS REFUSES **AND LEAVES `OutHex` UNTOUCHED**.
//
// THE OUT-PARAMETER IS THE CLAUSE, NOT THE RETURN VALUE. `StratCentroidHexOfSide`'s declaration
// is explicit that there is no `(0,0)` fallback and that the false path does not write, because
// `(0,0)` is a REAL hex -- the board origin -- and a caller cannot tell it from a genuine
// centroid there. A version that wrote a zero before returning false would satisfy every
// assertion about the return value and would fling the camera to the corner of the board on the
// one configuration the caller has no way to distinguish from success.
//
// SO THE HEX IS SEEDED WITH A SENTINEL AND THE SENTINEL IS ASSERTED TO HAVE SURVIVED. The
// sentinel is deliberately not `(0,0)`: seeding with the value the mutant would write makes the
// mutant invisible.
//
// THE MODEL IS HAND-BUILT HERE, WHICH THE PREDICATE CLAUSES ABOVE ARE NOT, AND THE ASYMMETRY IS
// DELIBERATE. Those drive fields the rules module owns and must not be authored by a test. This
// one drives a UNIT ROSTER, and the shipped scenario has no side with zero units on it -- the
// input is what is being controlled, and there is no expectation on the output side at all
// beyond "unchanged".
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCentroidEmptySideTest,
	"Stratocracy.StratPlay.GATE-HANDBACK.CentroidOfASideWithNoUnitsRefusesAndLeavesTheHexUntouched",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCentroidEmptySideTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPlayerHandbackClauses;

	FStratViewModel Model;

	FStratUnitView Present;
	Present.UnitId = 1;
	Present.Side   = 0;
	Present.Hex    = FIntPoint(4, 6);
	Model.Units.Add(Present);

	// NOT `(0, 0)`. That is the value the mutant writes, and seeding with it would make the
	// mutant pass. It is also outside anything a real board would produce, so a red says
	// "written" rather than "coincidence".
	const FIntPoint kSentinel(-9999, 12345);

	FIntPoint Hex = kSentinel;

	const bool bFound = StratCentroidHexOfSide(Model, /*Side*/ 1, Hex);

	if (!TestFalse(TEXT("GATE-HANDBACK: a side with no unit in the model has no centroid"),
			bFound))
	{
		return false;
	}

	TestTrue(*FString::Printf(
			TEXT("GATE-HANDBACK: and `OutHex` is NOT WRITTEN on that path -- the sentinel "
			     "(%d, %d) survived, and what came back was (%d, %d). A `(0,0)` written here "
			     "is a real hex, indistinguishable from a centroid at the board origin, and "
			     "would send the camera there on the one path the caller cannot tell from "
			     "success"),
			kSentinel.X, kSentinel.Y, Hex.X, Hex.Y),
		Hex == kSentinel);

	// THE CONTROL. Without it the assertions above are satisfied by a function that returns
	// false for every side, which would make the camera unreachable rather than robust.
	FIntPoint Occupied = kSentinel;
	if (!TestTrue(TEXT("CONTROL: the SAME model and the SAME function DO find a centroid for "
	                   "the side that has a unit, so the refusal above is about the roster and "
	                   "not about the function being dead"),
			StratCentroidHexOfSide(Model, /*Side*/ 0, Occupied)))
	{
		return false;
	}
	TestTrue(TEXT("CONTROL: and that call DID write the out-parameter, so 'untouched' above is "
	              "a property of the false path specifically"),
		Occupied != kSentinel);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-HANDBACK (5) -- THE CENTROID AVERAGES **ONLY ITS OWN SIDE'S** UNITS.
//
// THE OTHER SIDE IS PUT A LONG WAY AWAY, WHICH IS WHAT MAKES THE MUTANT LOUD. A version that
// dropped the `Unit.Side != Side` filter -- or inverted it -- would produce a hex hundreds of
// tiles from the answer, so the failure names itself instead of arriving as an off-by-one.
//
// THE EXPECTED HEX IS THE SAME MODEL WITH THE OTHER SIDE'S UNITS REMOVED, ASKED OF THE SAME
// FUNCTION. That is the whole design of this clause: the expectation is not arithmetic written
// here, it is `StratCentroidHexOfSide`'s own answer over a roster from which the units that
// must not count are ABSENT. So the clause says "the filter and the removal agree", which is
// the property, and it stays correct if the rounding, the accumulator width or the axial
// convention ever change.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCentroidIgnoresOtherSidesTest,
	"Stratocracy.StratPlay.GATE-HANDBACK.CentroidAveragesOnlyItsOwnSidesUnits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCentroidIgnoresOtherSidesTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPlayerHandbackClauses;

	const int32 kMine   = 0;
	const int32 kTheirs = 1;

	FStratViewModel Mixed;

	FStratUnitView A;
	A.UnitId = 1;
	A.Side   = kMine;
	A.Hex    = FIntPoint(2, 3);
	Mixed.Units.Add(A);

	FStratUnitView B;
	B.UnitId = 2;
	B.Side   = kMine;
	B.Hex    = FIntPoint(4, 7);
	Mixed.Units.Add(B);

	// FAR AWAY, AND ON BOTH AXES. A contaminating average would land nowhere near the answer.
	FStratUnitView Far1;
	Far1.UnitId = 3;
	Far1.Side   = kTheirs;
	Far1.Hex    = FIntPoint(900, -800);
	Mixed.Units.Add(Far1);

	FStratUnitView Far2;
	Far2.UnitId = 4;
	Far2.Side   = kTheirs;
	Far2.Hex    = FIntPoint(-700, 600);
	Mixed.Units.Add(Far2);

	// THE EXPECTATION SIDE: THE SAME FUNCTION OVER A ROSTER THAT SIMPLY HAS NO OTHER SIDE IN
	// IT. Nothing is computed here.
	FStratViewModel MineOnly;
	MineOnly.Units.Add(A);
	MineOnly.Units.Add(B);

	FIntPoint Expected = FIntPoint::ZeroValue;
	if (!TestTrue(TEXT("CONTROL: the expectation roster -- this side's units alone -- has a "
	                   "centroid at all"),
			StratCentroidHexOfSide(MineOnly, kMine, Expected)))
	{
		return false;
	}

	FIntPoint Got = FIntPoint::ZeroValue;
	if (!TestTrue(TEXT("CONTROL: and so does the mixed roster"),
			StratCentroidHexOfSide(Mixed, kMine, Got)))
	{
		return false;
	}

	TestTrue(*FString::Printf(
			TEXT("GATE-HANDBACK: the other side's units do not move the answer -- the mixed "
			     "roster gives (%d, %d) and the same function over this side's units alone "
			     "gives (%d, %d). The two enemy units sit at (900, -800) and (-700, 600), so a "
			     "dropped or inverted side filter is hundreds of tiles wrong rather than "
			     "subtly wrong"),
			Got.X, Got.Y, Expected.X, Expected.Y),
		Got == Expected);

	// AND THE OTHER SIDE HAS ITS OWN, DIFFERENT CENTROID -- so `Side` is genuinely a parameter
	// and not ignored in favour of "every unit" or "the first side seen".
	FIntPoint Theirs = FIntPoint::ZeroValue;
	if (!TestTrue(TEXT("CONTROL: the other side also has a centroid in the mixed roster"),
			StratCentroidHexOfSide(Mixed, kTheirs, Theirs)))
	{
		return false;
	}
	TestTrue(*FString::Printf(
			TEXT("GATE-HANDBACK: and it is a DIFFERENT hex ((%d, %d) against (%d, %d)), so the "
			     "`Side` argument is read rather than ignored"),
			Theirs.X, Theirs.Y, Got.X, Got.Y),
		Theirs != Got);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-HANDBACK (6) -- A MEAN THAT LANDS ON A HALF IS **ROUNDED**, NOT TRUNCATED.
//
// WHY THIS NEEDS ITS OWN CLAUSE. Clause 5 compares one call of the function against another
// call of the same function, so every arithmetic property it has cancels out -- including a
// truncation. Integer division, `(int32)(Sum / Count)`, or a `FloorToInt` would all pass clause
// 5 unchanged and would put the camera up to a full tile off the army, consistently, in the
// direction of the board origin.
//
// THE INPUT IS WRITTEN HERE BECAUSE NO SHIPPED SCENARIO PUTS AN ARMY ON A HALF, and a clause
// that waited for one would be pinning nothing. Two units at `(0, 0)` and `(1, 3)` give a mean
// of `(0.5, 1.5)` -- a half on BOTH axes, and DIFFERENT halves, so a q/r transposition is red
// too.
//
// **THE EXPECTATION IS STILL NOT WRITTEN.** It is `FMath::RoundToInt` over those means -- the
// same engine function `StratCentroidHexOfSide`'s own declaration names, and the reason that
// declaration spends a paragraph explaining why component-wise rounding is acceptable for a
// camera and wrong for picking. So this clause pins "it rounds", which is the property; it does
// NOT pin which way a tie goes, which is the engine's business and not this feature's. A
// literal `(1, 2)` here would have been this file adopting a tie-breaking rule nobody chose.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCentroidRoundsAHalfTest,
	"Stratocracy.StratPlay.GATE-HANDBACK.CentroidRoundsAMeanThatLandsOnAHalf",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCentroidRoundsAHalfTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPlayerHandbackClauses;

	const int32 kSide = 0;

	FStratViewModel Model;

	FStratUnitView A;
	A.UnitId = 1;
	A.Side   = kSide;
	A.Hex    = FIntPoint(0, 0);
	Model.Units.Add(A);

	FStratUnitView B;
	B.UnitId = 2;
	B.Side   = kSide;
	B.Hex    = FIntPoint(1, 3);
	Model.Units.Add(B);

	// THE TWO MEANS, AND THEY ARE DIFFERENT HALVES ON PURPOSE -- 0.5 on q and 1.5 on r -- so a
	// transposed pair cannot pass.
	const double MeanQ = 0.5;
	const double MeanR = 1.5;

	const FIntPoint Expected(FMath::RoundToInt(MeanQ), FMath::RoundToInt(MeanR));

	// THE CONTROL THAT MAKES THE CLAUSE MEAN "ROUNDS" RATHER THAN "AGREES WITH RoundToInt". If
	// rounding and truncation gave the same answer on this input there would be nothing to
	// catch, so the two are shown to DIFFER before the claim is made.
	// PLAIN C++ TRUNCATION AND NOT `FMath::TruncToInt`, because the mutant this control models
	// IS a plain `static_cast` or an integer division -- writing it the way the defect would be
	// written keeps the control honest about what it is comparing against.
	const FIntPoint Truncated(static_cast<int32>(MeanQ), static_cast<int32>(MeanR));
	if (!TestTrue(*FString::Printf(
			TEXT("CONTROL: on this input rounding (%d, %d) and truncation (%d, %d) disagree on "
			     "both axes, so the assertion below can actually see the difference"),
			Expected.X, Expected.Y, Truncated.X, Truncated.Y),
			Expected.X != Truncated.X && Expected.Y != Truncated.Y))
	{
		return false;
	}

	FIntPoint Got = FIntPoint::ZeroValue;
	if (!TestTrue(TEXT("CONTROL: the two-unit roster has a centroid"),
			StratCentroidHexOfSide(Model, kSide, Got)))
	{
		return false;
	}

	TestTrue(*FString::Printf(
			TEXT("GATE-HANDBACK: units at (0, 0) and (1, 3) give a mean of (0.5, 1.5), and the "
			     "centroid is `FMath::RoundToInt` of it -- (%d, %d), read: (%d, %d). Integer "
			     "division or a `FloorToInt` gives (%d, %d) and would park the camera up to a "
			     "full tile toward the board origin, every time"),
			Expected.X, Expected.Y, Got.X, Got.Y, Truncated.X, Truncated.Y),
		Got == Expected);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-HANDBACK (7) -- **A TEARDOWN WHILE A TOUR IS RUNNING DOES NOT SPEND THE HAND-BACK.**
//
// THIS IS THE SHARPEST CLAUSE IN THE FILE AND THE ONE THE FEATURE'S DESIGN TURNS ON.
//
// `bPlayerHandbackPending` is lowered on the line ABOVE each of `Deinitialize`'s and
// `TearDownPresentation`'s `EndAiPlaybackTour()` call, and `EndAiPlaybackTour` is a CALLER of
// `NotePlayerTurnBeganIfDue`. Put the reset where it looks like it belongs -- beside `SoundMark`
// and `ReceiptMark`, where each function otherwise gathers everything a match boundary
// invalidates -- and it lands FORTY LINES BELOW that call in `Deinitialize` and SIXTY below it
// in `TearDownPresentation`.
//
// **AND EVERY GUARD WOULD PASS.** That is what makes this worth a clause rather than a comment.
// At the moment `EndAiPlaybackTour` runs inside `TearDownPresentation`, `IsMatchLive()` is still
// true (the bridge is reset sixty lines further down) and `AppliedModel` is still the OUTGOING
// match's (cleared on the same later line), so `StratHandsBackToPlayer` answers about the match
// the player just asked to LEAVE -- and answers yes. The player clicks New Match and hears the
// old match's hand-back while their camera jumps to the old match's army.
//
// HOW THE FIXTURE REACHES THAT STATE. A PACED config, so `RunAiTurnsNow` leaves the flag UP and
// a tour RUNNING -- the beat deferred, not spent. Then a second `StartMatch` on the same valid
// config, which passes `StartMatchInternal`'s two configuration-refusal arms and so reaches
// `TearDownPresentation` with no further guard. Both preconditions are asserted rather than
// assumed: a fixture whose tour had already ended would have nothing to defer and would be
// green over the mutant.
//
// WHY THE ONE AI SEAT MATTERS HERE TOO. With BOTH sides AI, `StratHandsBackToPlayer` refuses on
// the seat term at teardown and the mutant emits nothing -- the clause would be green over the
// defect. It has to be a match that really is handing back to a human.
//
// **MEASURED AGAINST THE MUTANT, 2026-09-07.** The reset was moved back below each
// `EndAiPlaybackTour()` call and this clause went red, reporting one `PlayerTurnBegan` where it
// requires zero; it was restored and the clause went green again. A clause that would pass over
// the defect is worth nothing here, and this one does not.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratHandbackNotSpentByTeardownTest,
	"Stratocracy.StratPlay.GATE-HANDBACK.ATeardownMidTourDoesNotSpendTheHandback",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratHandbackNotSpentByTeardownTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPlayerHandbackClauses;

	// ONLY THE TILE-MESH LINE IS DECLARED, AND `STRAT-AI refused` IS DELIBERATELY NOT.
	// Every AI-VS-AI fixture in this directory declares both, correctly: a both-sides-AI
	// stretch runs to a Sec 2.8 result and the rules module then refuses the winning side's
	// own EndTurn. **THIS FIXTURE RUNS ONE AI TURN AND HANDS BACK, SO THAT LINE NEVER
	// FIRES.** Measured 2026-09-07 rather than reasoned: declaring it made this clause RED
	// with "Expected suppressed ('Warning') level log message or higher matching
	// 'STRAT-AI refused' did not occur", because `AddExpectedMessagePlain` with
	// `Occurrences 0` is itself an ASSERTION -- the point `StratAiPlaybackClauses.cpp`,
	// `StratTourExistenceHoldClauses.cpp` and `MatchEndedFiresOnceAndFromTheLatch` all
	// record about these same lines on this same fixture shape.
	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Match    = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	UStratSoundDirector* const  Director = Scope.World->GetSubsystem<UStratSoundDirector>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Match) ||
	    !TestNotNull(TEXT("the world has a sound director"), Director))
	{
		return false;
	}

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("a one-AI-seat, PACED config assembles"),
			MakeOneAiSideConfig(*Match, kHarnessPlaybackInterval, Config, Error)))
	{
		AddError(Error);
		return false;
	}

	// THE CONFIGURATION UNDER TEST, NAMED. This is the PACED half -- the shipped default is
	// zero, and at zero the beat is spent inside `RunAiTurnsNow` and there is no deferred flag
	// for a teardown to find.
	if (!TestTrue(*FString::Printf(
			TEXT("CONTROL: the configuration under test is PACED (AiPlaybackStepSeconds %.1f), "
			     "which is what leaves a hand-back DEFERRED across the teardown. The shipped "
			     "default of %.1f could not construct this state at all"),
			Config.AiPlaybackStepSeconds, FStratMatchConfig().AiPlaybackStepSeconds),
			Config.AiPlaybackStepSeconds > 0.0f))
	{
		return false;
	}

	Match->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the match is live whatever StartMatch returned"), Match->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	Director->ResetEmissions();

	FString RunReason;
	Match->RunAiTurnsNow(RunReason);
	AddInfo(FString::Printf(TEXT("RunAiTurnsNow reason '%s'; %d step(s) recorded"),
		*RunReason, Match->GetAiPlaybackStepCount()));

	// ---- THE THREE PRECONDITIONS, ALL MEASURED --------------------------------------------
	if (!TestTrue(TEXT("CONTROL (1): a tour is RUNNING, so the hand-back beat is deferred and "
	                   "the flag is still up going into the teardown. A fixture whose tour had "
	                   "already ended would have nothing for the mutant to spend"),
			Match->IsAiPlaybackRunning()))
	{
		return false;
	}

	if (!TestEqual(*FString::Printf(
			TEXT("CONTROL (2): and nothing has sounded yet -- %d hand-back cue(s) so far -- so "
			     "any count after the teardown was produced BY the teardown"),
			CountOfCue(Director->GetEmissions(), EStratSoundCue::PlayerTurnBegan)),
			CountOfCue(Director->GetEmissions(), EStratSoundCue::PlayerTurnBegan), 0))
	{
		return false;
	}

	const FStratViewModel& Outgoing = Match->GetViewModel();
	if (!TestTrue(*FString::Printf(
			TEXT("CONTROL (3): and the outgoing match IS a hand-back by the module's own "
			     "predicate (turn %d, side to move %d) -- so a flag spent during the teardown "
			     "WOULD emit. Without this the zero below is satisfied by a match that could "
			     "not have sounded anyway"),
			Outgoing.Match.Turn, Outgoing.Match.SideToMove),
			StratHandsBackToPlayer(Outgoing, Config.AiSides)))
	{
		return false;
	}

	// ---- THE TEARDOWN --------------------------------------------------------------------
	// A SECOND `StartMatch` ON THIS VALID CONFIG REACHES `TearDownPresentation` -- unguarded once
	// `StartMatchInternal`'s two configuration-refusal arms have passed, which this config does --
	// above everything that would clear the bridge or the applied model. This is the reseed a
	// player performs by clicking New Match.
	FString SecondError;
	Match->StartMatch(Config, SecondError);

	if (!TestFalse(TEXT("CONTROL: the teardown really did stop the tour"),
			Match->IsAiPlaybackRunning()))
	{
		return false;
	}

	TestEqual(*FString::Printf(
			TEXT("GATE-HANDBACK: tearing a match down while a tour is running does NOT spend "
			     "the pending hand-back. `bPlayerHandbackPending` comes down on the line ABOVE "
			     "each `EndAiPlaybackTour()` call; placed with `SoundMark` forty to sixty lines "
			     "below it, the flag would be spent on the very next line -- sounding the "
			     "OUTGOING match's hand-back and moving the camera to its army, with every "
			     "guard passing because the bridge is still seeded and `AppliedModel` is still "
			     "the old match's. Read %d cue(s)"),
			CountOfCue(Director->GetEmissions(), EStratSoundCue::PlayerTurnBegan)),
		CountOfCue(Director->GetEmissions(), EStratSoundCue::PlayerTurnBegan), 0);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-HANDBACK (8) -- THE RECENTRE LANDS ON THE VIEWING SIDE'S CENTROID AND KEEPS THE ZOOM.
//
// **THE FIRST FIXTURE IN THIS PROJECT TO POSSESS AN `AStratCameraPawn`.** See the file header:
// `FindCameraPawn` walks world -> first PlayerController -> possessed pawn -> cast, and no
// other fixture in this tree has ever had the third or fourth link, which is why
// `FocusPlaybackStep` -- the older caller of the same chain -- has never been pinned by
// anything. All four links are built and each is checked, because a missing one produces a
// silent early return and therefore a camera that did not move -- which is exactly the mutant
// this clause exists to catch, arriving disguised as a fixture defect.
//
// THE EXPECTED LOCATION IS READ FROM TWO MODULE FUNCTIONS AND IS NOT A NUMBER.
// `StratCentroidHexOfSide` over `GetViewModel()` at `GetViewModel().ViewingSide` gives the hex;
// `AStratBoardActor::WorldLocationOfHex` -- the project's only axial -> world map, reached
// through `GetBoard()` -- gives the location. A world coordinate written here would be this
// file re-deriving the board's layout, which `AStratBoardActor.h` records having already paid
// for once.
//
// `ViewingSide` AND NOT `SideToMove`, WHICH IS THE ONE ARGUMENT IN THE VERB WORTH PINNING.
// `RecenterCameraOnViewingSide`'s declaration says a camera is a statement about whose screen
// this is; the CUE that fires on the same line deliberately carries `SideToMove` instead. On
// the shipped single-player configuration the two are EQUAL, so a later tidy-up that unified
// them would be invisible to any assertion that did not name its field -- and this fixture
// cannot make them differ, because the hand-back only fires when the side to move is the human
// seat and the screen is drawn for that seat. **SO THIS CLAUSE DOES NOT DISTINGUISH THE TWO
// FIELDS AND DOES NOT CLAIM TO.** It names `ViewingSide` in its expectation so that a reader
// knows which was intended; the distinction itself is a measured gap, recorded in
// `Tools/architect/state/tests.md`.
//
// **Z AND ARM LENGTH ARE PINNED EVEN THOUGH THEY SURVIVE "BY CONSTRUCTION".**
// `AStratCameraPawn::FocusWorldLocation` reads the actor's current Z back and re-writes it, and
// touches the spring arm not at all -- so the player's height and zoom cannot move. That is a
// property of four lines of code, and "by construction" is precisely what a tidy-up breaks: a
// `SetActorLocation(WorldLocation)` that forgot to preserve Z is a one-token change with no
// other symptom. Both are read off the pawn immediately before the hand-back and compared to
// themselves.
//
// THE ANTI-VACUITY GUARD IS THAT THE CAMERA MUST HAVE MOVED AT ALL. The pawn spawns at the
// origin; the clause asserts the expected XY is not where it started, so "landed on the
// centroid" cannot be satisfied by a camera that never moved and a centroid that happened to be
// at zero.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRecenterLandsOnCentroidTest,
	"Stratocracy.StratPlay.GATE-HANDBACK.RecenterLandsOnTheViewingSideCentroidAndKeepsTheZoom",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRecenterLandsOnCentroidTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPlayerHandbackClauses;

	// ONLY THE TILE-MESH LINE IS DECLARED, AND `STRAT-AI refused` IS DELIBERATELY NOT.
	// Every AI-VS-AI fixture in this directory declares both, correctly: a both-sides-AI
	// stretch runs to a Sec 2.8 result and the rules module then refuses the winning side's
	// own EndTurn. **THIS FIXTURE RUNS ONE AI TURN AND HANDS BACK, SO THAT LINE NEVER
	// FIRES.** Measured 2026-09-07 rather than reasoned: declaring it made this clause RED
	// with "Expected suppressed ('Warning') level log message or higher matching
	// 'STRAT-AI refused' did not occur", because `AddExpectedMessagePlain` with
	// `Occurrences 0` is itself an ASSERTION -- the point `StratAiPlaybackClauses.cpp`,
	// `StratTourExistenceHoldClauses.cpp` and `MatchEndedFiresOnceAndFromTheLatch` all
	// record about these same lines on this same fixture shape.
	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
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

	// ---- LINKS 2, 3 AND 4 OF `FindCameraPawn`'s CHAIN ------------------------------------
	AStratPlayerController* const Controller = Scope.World->SpawnActor<AStratPlayerController>();
	if (!TestNotNull(TEXT("a player controller was spawned"), Controller))
	{
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: and the world reports it as the FIRST local controller, which "
	                   "is the link `FindCameraPawn` actually asks for -- without "
	                   "`InitializeActorsForPlay` a spawned controller never enters "
	                   "`UWorld::PlayerControllerList` and this is null"),
			Scope.World->GetFirstPlayerController() == Controller))
	{
		return false;
	}

	AStratCameraPawn* const Camera = Scope.World->SpawnActor<AStratCameraPawn>();
	if (!TestNotNull(TEXT("an AStratCameraPawn was spawned"), Camera))
	{
		return false;
	}

	Controller->Possess(Camera);
	if (!TestTrue(TEXT("CONTROL: the controller POSSESSES the camera pawn -- the last link of "
	                   "`FindCameraPawn`'s chain, and the one no other fixture in this tree has "
	                   "ever had. Without it the verb takes a silent early return and this "
	                   "clause would report 'the camera did not move' for a fixture reason"),
			Controller->GetPawn() == Camera))
	{
		return false;
	}

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("a one-AI-seat config assembles"),
			MakeOneAiSideConfig(*Match, FStratMatchConfig().AiPlaybackStepSeconds, Config,
			                    Error)))
	{
		AddError(Error);
		return false;
	}

	if (!TestTrue(*FString::Printf(
			TEXT("CONTROL: the configuration under test is the SHIPPED, UNPACED one "
			     "(AiPlaybackStepSeconds %.3f), so the recentre happens inside `RunAiTurnsNow` "
			     "with no tour to defer it"),
			Config.AiPlaybackStepSeconds),
			Config.AiPlaybackStepSeconds <= 0.0f))
	{
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: and the recentre opt-in is ON, which is the shipped default "
	                   "and is asserted rather than set here"),
			Config.bRecenterCameraOnPlayerTurn))
	{
		return false;
	}

	Match->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the match is live whatever StartMatch returned"), Match->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}
	if (!TestNotNull(TEXT("CONTROL: and the match has a board, which owns the one axial -> "
	                      "world conversion this clause's expectation is read from"),
			Match->GetBoard()))
	{
		return false;
	}

	// ---- READ BEFORE ---------------------------------------------------------------------
	const FVector Before    = Camera->GetActorLocation();
	const float   ArmBefore = Camera->GetArmLength();
	AddInfo(FString::Printf(TEXT("camera before: (%.3f, %.3f, %.3f), arm %.3f"),
		Before.X, Before.Y, Before.Z, ArmBefore));

	FString RunReason;
	Match->RunAiTurnsNow(RunReason);

	const FStratViewModel& Applied = Match->GetViewModel();
	if (!TestTrue(*FString::Printf(
			TEXT("CONTROL: the hand-over really did give play back to a human seat (turn %d, "
			     "side to move %d), so the recentre was due"),
			Applied.Match.Turn, Applied.Match.SideToMove),
			StratHandsBackToPlayer(Applied, Config.AiSides)))
	{
		return false;
	}

	// ---- THE EXPECTATION, FROM TWO MODULE FUNCTIONS --------------------------------------
	FIntPoint Centre = FIntPoint::ZeroValue;
	if (!TestTrue(*FString::Printf(
			TEXT("CONTROL: the viewing side (%d) has a centroid, so the verb had somewhere to "
			     "go -- this is the same call `RecenterCameraOnViewingSide` makes"),
			Applied.ViewingSide),
			StratCentroidHexOfSide(Applied, Applied.ViewingSide, Centre)))
	{
		return false;
	}

	const FVector Expected = Match->GetBoard()->WorldLocationOfHex(Centre);
	const FVector After    = Camera->GetActorLocation();
	AddInfo(FString::Printf(
		TEXT("centroid hex (%d, %d); expected world (%.3f, %.3f); camera after: "
		     "(%.3f, %.3f, %.3f), arm %.3f"),
		Centre.X, Centre.Y, Expected.X, Expected.Y, After.X, After.Y, After.Z,
		Camera->GetArmLength()));

	// THE ANTI-VACUITY GUARD. If the expected location were where the pawn already was, a verb
	// that did nothing would pass.
	if (!TestTrue(*FString::Printf(
			TEXT("CONTROL: the expected location (%.3f, %.3f) is not where the pawn started "
			     "(%.3f, %.3f), so 'landed on the centroid' cannot be satisfied by a camera "
			     "that never moved"),
			Expected.X, Expected.Y, Before.X, Before.Y),
			!FMath::IsNearlyEqual(Expected.X, Before.X, kWorldTolerance) ||
			!FMath::IsNearlyEqual(Expected.Y, Before.Y, kWorldTolerance)))
	{
		return false;
	}

	TestTrue(*FString::Printf(
			TEXT("GATE-HANDBACK: the hand-back put the camera's X on the viewing side's "
			     "centroid -- `AStratBoardActor::WorldLocationOfHex` of "
			     "`StratCentroidHexOfSide`, both read from the module. Expected %.3f, read "
			     "%.3f"),
			Expected.X, After.X),
		FMath::IsNearlyEqual(After.X, Expected.X, kWorldTolerance));

	TestTrue(*FString::Printf(
			TEXT("GATE-HANDBACK: and its Y. Expected %.3f, read %.3f"), Expected.Y, After.Y),
		FMath::IsNearlyEqual(After.Y, Expected.Y, kWorldTolerance));

	TestTrue(*FString::Printf(
			TEXT("GATE-HANDBACK: and the player's HEIGHT is untouched -- `FocusWorldLocation` "
			     "reads the actor's current Z back and re-writes it. Before %.3f, after %.3f. "
			     "A `SetActorLocation(WorldLocation)` that forgot the Z is a one-token change "
			     "with no other symptom, which is why this is pinned rather than trusted to "
			     "construction"),
			Before.Z, After.Z),
		FMath::IsNearlyEqual(After.Z, Before.Z, kWorldTolerance));

	TestEqual(
		TEXT("GATE-HANDBACK: and the player's ZOOM is untouched -- `TargetArmLength` lives on "
		     "the spring arm, which this path never names. A recentre that reset the arm would "
		     "throw away a zoom the player set, once a turn, forever"),
		Camera->GetArmLength(), ArmBefore);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-HANDBACK (9) -- THE RECENTRE IS OPT-OUT, AND THE OPT-OUT SHIPS **ON**.
//
// A MATCHED PAIR DIFFERING IN ONE FIELD, WITH OPPOSITE EXPECTATIONS. Two fixtures identical in
// every other respect: with `bRecenterCameraOnPlayerTurn` true the camera moves, with it false
// the camera is exactly where it was. Neither half is worth anything alone -- the ON half
// duplicates clause 8, and the OFF half is satisfied by a deleted feature.
//
// **AND THE THIRD ASSERTION IS THE ANTI-VACUITY GUARD, WHICH IS THE ONE THIS PROJECT HAS BEEN
// BITTEN BY.** A shipped default of `false` would make every clause about this feature vacuous:
// every fixture would run the OFF path and the ON path would never execute. That is a measured
// defect in this tree's own history, not a hypothetical, and `bRecenterCameraOnPlayerTurn`'s
// declaration argues at length that `AiPlaybackStepSeconds`' reasons for shipping inert do not
// transfer to a synchronous verb. So the C++ default is asserted directly, off
// `FStratMatchConfig()`, and a flip to `false` reddens this clause immediately rather than
// silently emptying the other eight.
//
// THE CUE IS ASSERTED TO STILL FIRE ON THE OFF ARM, WHICH IS WHAT KEEPS THE OPT-OUT NARROW. The
// flag's declaration scopes it to the CAMERA; a `return` placed at the top of
// `NotePlayerTurnBeganIfDue` instead of inside `RecenterCameraOnViewingSide` would silence the
// hand-back cue as well, and every camera assertion here would still pass.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRecenterOptOutTest,
	"Stratocracy.StratPlay.GATE-HANDBACK.RecenterIsSuppressedByItsOptOutWhichShipsOn",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRecenterOptOutTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratPlayerHandbackClauses;

	// ONLY THE TILE-MESH LINE IS DECLARED, AND `STRAT-AI refused` IS DELIBERATELY NOT.
	// Every AI-VS-AI fixture in this directory declares both, correctly: a both-sides-AI
	// stretch runs to a Sec 2.8 result and the rules module then refuses the winning side's
	// own EndTurn. **THIS FIXTURE RUNS ONE AI TURN AND HANDS BACK, SO THAT LINE NEVER
	// FIRES.** Measured 2026-09-07 rather than reasoned: declaring it made this clause RED
	// with "Expected suppressed ('Warning') level log message or higher matching
	// 'STRAT-AI refused' did not occur", because `AddExpectedMessagePlain` with
	// `Occurrences 0` is itself an ASSERTION -- the point `StratAiPlaybackClauses.cpp`,
	// `StratTourExistenceHoldClauses.cpp` and `MatchEndedFiresOnceAndFromTheLatch` all
	// record about these same lines on this same fixture shape.
	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	// ---- THE ANTI-VACUITY GUARD, FIRST -----------------------------------------------------
	// READ OFF `FStratMatchConfig()`, WHICH IS THE C++ DEFAULT EVERY MATCH STARTS FROM.
	if (!TestTrue(TEXT("GATE-HANDBACK: `bRecenterCameraOnPlayerTurn` ships TRUE. A `false` "
	                   "default would make every clause about this feature vacuous -- every "
	                   "fixture would take the OFF path and the ON path would never execute, "
	                   "which is a defect this project has already shipped once"),
			FStratMatchConfig().bRecenterCameraOnPlayerTurn))
	{
		return false;
	}

	const bool bArms[] = { true, false };

	for (const bool bRecenter : bArms)
	{
		FTestWorldScope Scope;
		if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
		{
			return false;
		}

		UStratMatchSubsystem* const Match    = Scope.World->GetSubsystem<UStratMatchSubsystem>();
		UStratSoundDirector* const  Director = Scope.World->GetSubsystem<UStratSoundDirector>();
		if (!TestNotNull(TEXT("the world has a match subsystem"), Match) ||
		    !TestNotNull(TEXT("the world has a sound director"), Director))
		{
			return false;
		}

		AStratPlayerController* const Controller =
			Scope.World->SpawnActor<AStratPlayerController>();
		AStratCameraPawn* const Camera = Scope.World->SpawnActor<AStratCameraPawn>();
		if (!TestNotNull(TEXT("a player controller was spawned"), Controller) ||
		    !TestNotNull(TEXT("an AStratCameraPawn was spawned"), Camera))
		{
			return false;
		}
		Controller->Possess(Camera);
		if (!TestTrue(TEXT("CONTROL: the whole `FindCameraPawn` chain stands -- first "
		                   "controller, possessed pawn, right class -- on BOTH arms, so the "
		                   "OFF arm's stillness is the flag and not a broken fixture"),
				Scope.World->GetFirstPlayerController() == Controller &&
				Controller->GetPawn() == Camera))
		{
			return false;
		}

		FStratMatchConfig Config;
		FString           Error;
		if (!TestTrue(TEXT("a one-AI-seat config assembles"),
				MakeOneAiSideConfig(*Match, FStratMatchConfig().AiPlaybackStepSeconds, Config,
				                    Error)))
		{
			AddError(Error);
			return false;
		}

		// THE ONE FIELD THAT DIFFERS BETWEEN THE TWO ARMS.
		Config.bRecenterCameraOnPlayerTurn = bRecenter;

		Match->StartMatch(Config, Error);
		if (!TestTrue(TEXT("the match is live whatever StartMatch returned"),
				Match->IsMatchLive()))
		{
			AddError(Error);
			return false;
		}

		Director->ResetEmissions();

		const FVector Before    = Camera->GetActorLocation();
		const float   ArmBefore = Camera->GetArmLength();

		FString RunReason;
		Match->RunAiTurnsNow(RunReason);

		const FStratViewModel& Applied = Match->GetViewModel();
		if (!TestTrue(TEXT("CONTROL: the hand-over really did give play back to a human seat, "
		                   "so the recentre was due on the ON arm and declined on the OFF arm"),
				StratHandsBackToPlayer(Applied, Config.AiSides)))
		{
			return false;
		}

		FIntPoint Centre = FIntPoint::ZeroValue;
		if (!TestTrue(TEXT("CONTROL: the viewing side has a centroid, so a side with no units "
		                   "is not what kept the camera still"),
				StratCentroidHexOfSide(Applied, Applied.ViewingSide, Centre)))
		{
			return false;
		}
		if (!TestNotNull(TEXT("CONTROL: and the match has a board"), Match->GetBoard()))
		{
			return false;
		}

		const FVector Expected = Match->GetBoard()->WorldLocationOfHex(Centre);
		const FVector After    = Camera->GetActorLocation();

		AddInfo(FString::Printf(
			TEXT("bRecenterCameraOnPlayerTurn=%s: before (%.3f, %.3f), expected centroid "
			     "(%.3f, %.3f), after (%.3f, %.3f)"),
			bRecenter ? TEXT("true") : TEXT("false"), Before.X, Before.Y, Expected.X, Expected.Y,
			After.X, After.Y));

		if (!TestTrue(TEXT("CONTROL: the two locations differ, so 'moved' and 'did not move' "
		                   "are distinguishable on this fixture"),
				!FMath::IsNearlyEqual(Expected.X, Before.X, kWorldTolerance) ||
				!FMath::IsNearlyEqual(Expected.Y, Before.Y, kWorldTolerance)))
		{
			return false;
		}

		if (bRecenter)
		{
			TestTrue(TEXT("GATE-HANDBACK, THE ON ARM: with the opt-in set the camera lands on "
			              "the centroid. Without this half the OFF arm below is satisfied by a "
			              "deleted feature"),
				FMath::IsNearlyEqual(After.X, Expected.X, kWorldTolerance) &&
				FMath::IsNearlyEqual(After.Y, Expected.Y, kWorldTolerance));
		}
		else
		{
			TestTrue(*FString::Printf(
					TEXT("GATE-HANDBACK, THE OFF ARM: with `bRecenterCameraOnPlayerTurn` false "
					     "the camera is exactly where the player left it -- (%.3f, %.3f), was "
					     "(%.3f, %.3f). The flag is asked FIRST in the verb, of `ActiveConfig`, "
					     "before the board and the pawn are even looked at"),
					After.X, After.Y, Before.X, Before.Y),
				FMath::IsNearlyEqual(After.X, Before.X, kWorldTolerance) &&
				FMath::IsNearlyEqual(After.Y, Before.Y, kWorldTolerance));

			// AND THE OPT-OUT REACHES THE CAMERA ONLY. A `return` at the top of
			// `NotePlayerTurnBeganIfDue` would satisfy every camera assertion above while
			// silencing a cue the flag says nothing about.
			TestEqual(
				TEXT("GATE-HANDBACK: and the hand-back CUE still sounds -- this flag is scoped "
				     "to the camera by its own declaration, and a refusal moved up into "
				     "`NotePlayerTurnBeganIfDue` would pass every assertion above while "
				     "silencing audio the flag has no opinion about"),
				CountOfCue(Director->GetEmissions(), EStratSoundCue::PlayerTurnBegan), 1);
		}

		TestEqual(
			TEXT("GATE-HANDBACK: and the zoom survives on both arms"),
			Camera->GetArmLength(), ArmBefore);
	}

	return true;
}
