// The gate for UStratMatchSubsystem::ApplyView -- GDD §4.9 / §4.1, under T-INT-05.
//
//   T-INT-05  the screen is rebuildable FROM THE VIEW MODEL ALONE. No actor holds a bit
//             the model does not, and no frame is correct only because the previous one
//             was.
//
// WHAT THIS FILE MAKES OPERATIONAL, because an acceptance ID that is not made operational
// is decoration. `StratViewModel.h` states the property in as many words -- the model "is a
// COMPLETE DESCRIPTION OF WHAT SHOULD BE ON SCREEN AND NEVER A DELTA" -- and
// `StratMatchSubsystem.h` states the consequence: `ApplyView` reconciles unconditionally,
// which is "what makes T-INT-05 a fact about the code rather than a claim about it." Two
// clauses turn those sentences into measurements.
//
//   1. `ApplyViewSpawnsMovesAndDestroys`. Two models are applied in sequence and the
//      resulting WORLD is compared against the second one alone. Present only in the first
//      must be gone; present only in the second must exist; present in both must have
//      MOVED to the second model's hex. Then the pair is applied again in the same order
//      and the world must land in the identical state -- because "a function of the last
//      model alone" is a statement about sequence-independence, and applying once proves
//      only that one sequence works.
//
//   2. `NoActorHoldsPresentationBits`. `FStratUnitView::bDone` is a presentation bit no
//      rules value produces: `strat::buildUiSnapshot` does not emit it and
//      `StratBuildViewModel` leaves it at its default. It lives on the MODEL exactly so
//      phase 4's selection machine has somewhere to put it that is not an actor --
//      `StratViewModel.h`: "state in an actor makes T-INT-05 false and nothing reports it."
//      So the clause drives the bit true, then FALSE AGAIN, and requires the actor's cached
//      view to follow both ways. A mirror with a memory would pass the first half.
//
// THE ACTOR SET IS READ FROM THE WORLD, NOT FROM THE SUBSYSTEM'S MAP. `UnitActors` is
// private and `FindUnitActor` can only be probed with an id this file already has, which
// would make "the set is EXACTLY B's ids" unprovable from the inside -- a stale actor the
// map had dropped would be invisible to it. A `TActorIterator` over the world sees every
// `AStratUnitActor` in existence, so "destroyed" means destroyed and not merely
// unreferenced. That is a strictly stronger reading of the same claim.
//
// WHERE THE EXPECTATIONS COME FROM.
//   - The unit ids, hexes, hp and sides are the applied model's own fields. Nothing is
//     written down; the models are built by `BuildViewModel` from a live bridge and then
//     EDITED -- entries removed, hexes reassigned to other hexes OF THE SAME MODEL -- which
//     is fixture construction and not expectation computation.
//   - Every expected world location is `AStratBoardActor::WorldLocationOfHex`, read from
//     the board the subsystem spawned. The axial -> world formula does not appear in this
//     file and must not: it is the board's, once, and a copy here would be arithmetic this
//     gate was asserting against itself.
//   - The `bDone` flip is the one place a literal appears, and it is unavoidable: no
//     module-side value produces that bit today, which is precisely why it is a debt phase
//     4 inherits. The clause is written so the literal is the INPUT and the model's own
//     field is the expectation, and the undecorated model rebuilt from the bridge is used
//     as the third state so at least one of the three comes from the module.
//
// THIS FIXTURE HAS NO TILE MESHES, AND THAT IS ASSERTED RATHER THAN PAPERED OVER.
// `StartMatch` spawns the board and draws it in one call, so there is no moment in which a
// headless gate could hand the spawned board the Blueprint defaults phase 5 will author.
// The board therefore reports every terrain kind as unmeshed and `StartMatch` returns FALSE
// -- while `IsMatchLive()` returns true, because `StratMatchSubsystem.h` states the split in
// as many words: all-or-nothing on the rules side, deliberately NOT all-or-nothing on the
// presentation side, since "a configuration gap in phase 5's assets must not be able to
// prevent a seeded, correct match from existing." Every clause below asserts that split
// explicitly and declares the board's warning expected, which turns a nuisance into a
// pinned property no other test covers. Nothing this file measures needs a mesh:
// `WorldLocationOfHex` is arithmetic over a hex, and a unit actor with no mesh is invisible
// and fully reconciled.
//
// NO `strat::` FREE FUNCTION IS CALLED ANYWHERE BELOW. That is LNK2019 outside StratBridge
// and StratRules, measured 8x.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Misc/Paths.h"
#include "UObject/UnrealType.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

// Included from a .cpp and never from a UHT-parsed header.
#include "StratBridge.h"

namespace StratMatchReconcile
{
	static const int32 kFirstSide = 0;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/**
	 * The match configuration a live match needs.
	 *
	 * THE SCENARIO PATH IS PROJECT-RELATIVE, exactly as `FStratMatchConfig::ScenarioFile`
	 * documents and exactly as `AStratScoreboardHUD` resolves its own copy, so this gate and
	 * the running game open the same file when configured the same way.
	 *
	 * THE ACTOR CLASSES ARE THE C++ CLASSES THEMSELVES. Neither is `Abstract`, so both
	 * spawn; phase 5's Blueprints subclass them to carry meshes, and this file supplies the
	 * meshes through the class defaults instead.
	 */
	static bool MakeConfig(FStratMatchConfig& Out, FString& OutError)
	{
		Out.UnitTable    = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		Out.TerrainTable = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
		if (Out.UnitTable == nullptr || Out.TerrainTable == nullptr)
		{
			OutError = TEXT("DT_Units and/or DT_Terrain did not load from /Game/StratData");
			return false;
		}

		Out.ScenarioFile     = TEXT("Data/ferrum_crossing.json");
		Out.FirstSide        = kFirstSide;
		Out.ViewingSide      = 0;
		Out.BoardActorClass  = AStratBoardActor::StaticClass();
		Out.UnitActorClass   = AStratUnitActor::StaticClass();
		return true;
	}

	/** The transient world, with the engine context teardown wants. See
	 *  StratScoreboardHUDSeam.cpp on why the context is not decoration. */
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

		// THE WORLD GOES FIRST AND THE CONTEXT SECOND. Measured: with the context destroyed
		// first, `UStratMatchSubsystem::Deinitialize` -- which destroys the board and every
		// unit actor from inside `DestroyWorld` -- makes `UWorld::DestroyActor` warn "World
		// has no context!" once per actor. A gate that leaves warnings in a green run trains
		// the next reader to skim warnings.
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

	/**
	 * Every `AStratUnitActor` actually alive in the world, by the id it says it stands for.
	 *
	 * READ FROM THE WORLD AND NOT FROM THE SUBSYSTEM. See the header block: the claim is
	 * "the actor set equals B's ids exactly", and only an iterator over the world can see an
	 * actor the reconciliation map forgot about.
	 */
	static TSet<int32> LiveUnitIds(UWorld* World)
	{
		TSet<int32> Ids;
		if (World == nullptr)
		{
			return Ids;
		}
		for (TActorIterator<AStratUnitActor> It(World); It; ++It)
		{
			AStratUnitActor* const Actor = *It;
			if (IsValid(Actor) && !Actor->IsActorBeingDestroyed())
			{
				Ids.Add(Actor->GetUnitId());
			}
		}
		return Ids;
	}

	static FString Describe(const TSet<int32>& Ids)
	{
		TArray<int32> Sorted = Ids.Array();
		Sorted.Sort();
		TArray<FString> Parts;
		for (const int32 Id : Sorted)
		{
			Parts.Add(FString::FromInt(Id));
		}
		return FString::Join(Parts, TEXT(", "));
	}
}

// ---------------------------------------------------------------------------
// The world is a function of the LAST model alone -- T-INT-05's reconciliation clause.
//
// THE TWO MODELS ARE MADE FROM ONE REAL ONE. `BuildViewModel` produces the shipped
// scenario's units; A drops the last of them and B drops the first, so exactly one unit is
// present only in A (it must be destroyed), exactly one is present only in B (it must be
// spawned), and everything else is present in both (it must be moved). Every unit in B is
// also relocated to a different hex OF THE SAME MODEL, so "moved" is measured against a
// coordinate the board actually has rather than against an invented one.
//
// AND THEN THE PAIR IS APPLIED AGAIN. `ApplyView(A); ApplyView(B);` a second time must land
// the world in the identical state. Without that, this clause would prove that ONE
// sequence reconciles correctly, which is what a delta-shaped implementation also does the
// first time. Sequence-independence is the actual property, and the second pass is what
// measures it.
//
// POSITIONS ARE COMPARED IN XY AND THE Z DELTA IS COMPARED AGAINST ITSELF. `BodyZOffset` is
// a phase-5 presentation property; the clause requires every unit to sit the same distance
// above the tile plane, which is what "standing on its hex" means, and never that the
// distance is zero.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratMatchApplyViewSpawnsMovesAndDestroysTest,
	"Stratocracy.StratPlay.T-INT-05.ApplyViewSpawnsMovesAndDestroys",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratMatchApplyViewSpawnsMovesAndDestroysTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchReconcile;

	// The board's own sentence about a fixture with no Blueprint defaults. Declared expected
	// rather than suppressed, so the line stays in the log for a human and the clause below
	// asserts what it means.
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

	// ALL-OR-NOTHING ON THE RULES SIDE, AND DELIBERATELY NOT ON THE PRESENTATION SIDE. The
	// declaration of `StartMatch` states exactly this and nothing else pins it: with no tile
	// meshes the call returns FALSE and the match is nonetheless LIVE, because a phase-5
	// asset gap must not be able to prevent a seeded, correct match from existing.
	FString StartReason;
	const bool bStarted = Subsystem->StartMatch(Config, StartReason);
	TestTrue(TEXT("the match is live whatever StartMatch returned"), Subsystem->IsMatchLive());
	if (!bStarted)
	{
		TestTrue(*FString::Printf(
				TEXT("the only complaint is the tile meshes this fixture has none of: '%s'"),
				*StartReason),
			StartReason.Contains(TEXT("no tile mesh for terrain")));
	}
	if (!TestNotNull(TEXT("a live match owns a bridge"), Subsystem->GetBridge()))
	{
		AddError(StartReason);
		return false;
	}

	AStratBoardActor* const Board = Subsystem->GetBoard();
	if (!TestNotNull(TEXT("the subsystem spawned a board to ask for hex locations"), Board))
	{
		return false;
	}

	FStratViewModel Full;
	FString BuildError;
	if (!TestTrue(TEXT("the view model builds"), Subsystem->BuildViewModel(Full, BuildError)))
	{
		AddError(BuildError);
		return false;
	}
	if (!TestTrue(TEXT("the scenario ships enough units to make a set difference meaningful"),
			Full.Units.Num() >= 3) ||
		!TestTrue(TEXT("the board has hexes to move units onto"), Full.Hexes.Num() > 0))
	{
		return false;
	}

	// ---- The two models ----------------------------------------------------
	const int32 OnlyInAId = Full.Units[0].UnitId;
	const int32 OnlyInBId = Full.Units.Last().UnitId;
	TestNotEqual(TEXT("the unit only in A and the unit only in B are different units"),
		OnlyInAId, OnlyInBId);

	FStratViewModel ModelA = Full;
	ModelA.Units.RemoveAt(ModelA.Units.Num() - 1);

	FStratViewModel ModelB = Full;
	ModelB.Units.RemoveAt(0);
	for (int32 Index = 0; Index < ModelB.Units.Num(); ++Index)
	{
		// A hex OF THIS MODEL, chosen by a stride that is coprime-ish with the board width
		// so consecutive units do not land in a row. Which hexes these are does not matter;
		// that they are the model's does, because the expectation below is
		// `WorldLocationOfHex` of exactly this value.
		ModelB.Units[Index].Hex = Full.Hexes[(Index * 13 + 5) % Full.Hexes.Num()].Hex;
	}

	TSet<int32> ExpectedIds;
	for (const FStratUnitView& View : ModelB.Units)
	{
		ExpectedIds.Add(View.UnitId);
	}
	TestEqual(TEXT("B names each unit once"), ExpectedIds.Num(), ModelB.Units.Num());

	// ---- Apply A, then B ---------------------------------------------------
	Subsystem->ApplyView(ModelA);

	TestNotNull(TEXT("after A, the unit present only in A is on screen"),
		Subsystem->FindUnitActor(OnlyInAId));
	TestNull(TEXT("after A, the unit present only in B is not"),
		Subsystem->FindUnitActor(OnlyInBId));

	Subsystem->ApplyView(ModelB);

	// THE CLAUSE. The set in the world, against B's ids, both directions.
	{
		const TSet<int32> Live = LiveUnitIds(Scope.World);
		TestEqual(TEXT("the world holds exactly as many unit actors as B has units"),
			Live.Num(), ExpectedIds.Num());
		TestTrue(*FString::Printf(
				TEXT("the actor set equals B's unit ids exactly (world: [%s]; B: [%s])"),
				*Describe(Live), *Describe(ExpectedIds)),
			Live.Includes(ExpectedIds) && ExpectedIds.Includes(Live));
	}

	TestNull(TEXT("present only in A: destroyed"), Subsystem->FindUnitActor(OnlyInAId));
	TestNotNull(TEXT("present only in B: spawned"), Subsystem->FindUnitActor(OnlyInBId));

	// ---- Present in both: at WorldLocationOfHex(B.Hex) ---------------------
	{
		TArray<double> ZDeltas;
		for (const FStratUnitView& View : ModelB.Units)
		{
			AStratUnitActor* const Actor = Subsystem->FindUnitActor(View.UnitId);
			if (Actor == nullptr)
			{
				AddError(FString::Printf(TEXT("unit %d is in B and has no actor"), View.UnitId));
				continue;
			}

			// The actor's own cache of what it was last handed. `StratUnitActor.h` calls it
			// a cache and not an authority; this asserts that the cache is what B said.
			TestTrue(*FString::Printf(TEXT("unit %d's last applied view carries B's hex"), View.UnitId),
				Actor->GetLastAppliedView().Hex == View.Hex);
			TestEqual(*FString::Printf(TEXT("unit %d's actor stands for unit %d"), View.UnitId, View.UnitId),
				Actor->GetUnitId(), View.UnitId);

			// THE EXPECTATION IS READ FROM THE BOARD. The axial -> world formula appears
			// nowhere in this file.
			const FVector Expected = Board->WorldLocationOfHex(View.Hex);
			const FVector Actual   = Actor->GetActorLocation();

			TestTrue(*FString::Printf(
					TEXT("unit %d stands over WorldLocationOfHex(%d, %d) -- it is at %s, the hex is at %s"),
					View.UnitId, View.Hex.X, View.Hex.Y, *Actual.ToString(), *Expected.ToString()),
				FMath::IsNearlyEqual(Actual.X, Expected.X, 0.01) &&
				FMath::IsNearlyEqual(Actual.Y, Expected.Y, 0.01));

			ZDeltas.Add(Actual.Z - Expected.Z);
		}

		// One height above the plane for every unit -- `BodyZOffset` is phase 5's and its
		// value is deliberately not named here.
		for (int32 Index = 1; Index < ZDeltas.Num(); ++Index)
		{
			TestTrue(TEXT("every unit sits the same height above the tile plane"),
				FMath::IsNearlyEqual(ZDeltas[Index], ZDeltas[0], 0.01));
		}
	}

	// The subsystem's record of what it drew agrees with what it was handed. A RECORD, per
	// `GetViewModel`'s declaration, never an input -- so this is a read-back and not a
	// second source.
	TestEqual(TEXT("the applied model is the one that was applied"),
		Subsystem->GetViewModel().Units.Num(), ModelB.Units.Num());
	TestEqual(TEXT("...and it is drawn for the configured side"),
		Subsystem->GetViewModel().ViewingSide, ModelB.ViewingSide);

	// ---- Sequence independence ---------------------------------------------
	// The same pair again. If the world after the second B differs from the world after the
	// first, then what is on screen depends on how it got there -- which is exactly the
	// delta-shaped failure the whole reconciliation posture exists to exclude.
	const TSet<int32> AfterFirstPass = LiveUnitIds(Scope.World);

	Subsystem->ApplyView(ModelA);
	Subsystem->ApplyView(ModelB);

	{
		const TSet<int32> AfterSecondPass = LiveUnitIds(Scope.World);
		TestEqual(TEXT("re-applying the same pair leaves the same number of actors"),
			AfterSecondPass.Num(), AfterFirstPass.Num());
		TestTrue(*FString::Printf(
				TEXT("the world is a function of the LAST model alone (first: [%s]; second: [%s])"),
				*Describe(AfterFirstPass), *Describe(AfterSecondPass)),
			AfterSecondPass.Includes(AfterFirstPass) && AfterFirstPass.Includes(AfterSecondPass));

		for (const FStratUnitView& View : ModelB.Units)
		{
			AStratUnitActor* const Actor = Subsystem->FindUnitActor(View.UnitId);
			if (Actor == nullptr)
			{
				AddError(FString::Printf(TEXT("unit %d vanished on the second pass"), View.UnitId));
				continue;
			}
			const FVector Expected = Board->WorldLocationOfHex(View.Hex);
			const FVector Actual   = Actor->GetActorLocation();
			TestTrue(*FString::Printf(
					TEXT("on the second pass unit %d is still over its hex (%d, %d)"),
					View.UnitId, View.Hex.X, View.Hex.Y),
				FMath::IsNearlyEqual(Actual.X, Expected.X, 0.01) &&
				FMath::IsNearlyEqual(Actual.Y, Expected.Y, 0.01));
		}
	}

	// ---- And an empty model empties the screen -----------------------------
	// The degenerate end of the same statement: a model with no units is a complete
	// description too, and a reconcile that treated "nothing to add" as "nothing to do"
	// would leave the last army standing.
	{
		FStratViewModel Empty = Full;
		Empty.Units.Reset();
		Subsystem->ApplyView(Empty);

		const TSet<int32> Live = LiveUnitIds(Scope.World);
		TestEqual(*FString::Printf(TEXT("a model with no units leaves no unit actors (found [%s])"),
				*Describe(Live)),
			Live.Num(), 0);
	}

	return true;
}

// ---------------------------------------------------------------------------
// The unit actor is a mirror with no memory -- T-INT-05's presentation-bit clause.
//
// WHY THE FLIP BACK IS THE WHOLE TEST. Setting `bDone` true and seeing the actor report
// true proves only that the value reached it. An actor that OWNED the bit -- latched it,
// remembered it across a turn, kept it while the model said otherwise -- would pass that.
// It fails the moment the model says false again and the actor is asked. `StratUnitActor.h`
// is explicit that its cached view is "a cache of the last applied view and never a source"
// and that the moment an actor owns a bit the model does not, "`T-INT-05.
// PresentationBitsAreDefaulted` is quietly false with no compiler diagnostic." This is the
// diagnostic.
//
// FIVE STATES, AND THE FIRST AND LAST COME FROM THE MODULE. The undecorated model built by
// `StratBuildViewModel` carries `bDone == false` for every unit because the builder leaves
// it at its default -- that is the module-side value, and it opens and closes the sequence.
// The three in between are decorations, which is what phase 4's selection machine will
// write, and the literal `true` is unavoidable: no rules value produces this bit, which is
// exactly why it is a debt rather than a projection. The expectation on every assertion is
// the APPLIED MODEL'S OWN FIELD, never the literal that was written into it.
//
// `bLockedThisTurn` IS DRIVEN INDEPENDENTLY, because `StratViewModel.h` records that its
// lifecycle is not `bDone`'s -- it clears when §2.11.6 beat 1a retires, inside a turn, and
// "a unit can be un-locked and not-done in the same turn." An actor that collapsed the two
// into one flag would pass every clause that moved them together.
//
// THE REST OF THE VIEW IS CHECKED TOO, and briefly, because `ApplyUnitView` is documented
// TOTAL: every field written on every call. A partial apply would leave a fragment of an
// older model on the actor, which is the same failure one field over.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratMatchNoActorHoldsPresentationBitsTest,
	"Stratocracy.StratPlay.T-INT-05.NoActorHoldsPresentationBits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratMatchNoActorHoldsPresentationBitsTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchReconcile;

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

	// See the reconciliation clause above on why a false return here is the documented
	// presentation split and not a failed match.
	FString StartReason;
	Subsystem->StartMatch(Config, StartReason);
	if (!TestTrue(TEXT("the match is live"), Subsystem->IsMatchLive()))
	{
		AddError(StartReason);
		return false;
	}

	FStratViewModel Undecorated;
	FString BuildError;
	if (!TestTrue(TEXT("the view model builds"), Subsystem->BuildViewModel(Undecorated, BuildError)))
	{
		AddError(BuildError);
		return false;
	}
	if (!TestTrue(TEXT("the scenario ships units to decorate"), Undecorated.Units.Num() > 0))
	{
		return false;
	}

	// THE MODULE-SIDE STARTING POINT. The builder leaves both presentation bits at their
	// defaults, and this asserts that rather than assuming it -- if it ever stopped being
	// true, the sequence below would be measuring something else entirely.
	for (const FStratUnitView& View : Undecorated.Units)
	{
		TestFalse(*FString::Printf(
				TEXT("the builder leaves unit %d's bDone at its default"), View.UnitId),
			View.bDone);
		TestFalse(*FString::Printf(
				TEXT("the builder leaves unit %d's bLockedThisTurn at its default"), View.UnitId),
			View.bLockedThisTurn);
	}

	// Five decorations. `Pass 0` and `Pass 4` are the builder's own model, unmodified.
	enum class EDecoration : uint8
	{
		AsBuilt,
		AllDone,
		NoneDone,
		Alternating,
		AsBuiltAgain
	};

	const EDecoration Passes[] =
	{
		EDecoration::AsBuilt,
		EDecoration::AllDone,
		EDecoration::NoneDone,       // <- the flip back. The clause with teeth.
		EDecoration::Alternating,
		EDecoration::AsBuiltAgain
	};

	const TCHAR* const PassNames[] =
	{
		TEXT("as built"),
		TEXT("every unit done"),
		TEXT("no unit done (flipped back)"),
		TEXT("alternating"),
		TEXT("as built again")
	};

	const int32 PassCount = static_cast<int32>(UE_ARRAY_COUNT(Passes));
	for (int32 PassIndex = 0; PassIndex < PassCount; ++PassIndex)
	{
		FStratViewModel Decorated = Undecorated;

		for (int32 Index = 0; Index < Decorated.Units.Num(); ++Index)
		{
			switch (Passes[PassIndex])
			{
			case EDecoration::AsBuilt:
			case EDecoration::AsBuiltAgain:
				// Left exactly as the builder produced it.
				break;

			case EDecoration::AllDone:
				Decorated.Units[Index].bDone           = true;
				Decorated.Units[Index].bLockedThisTurn = true;
				break;

			case EDecoration::NoneDone:
				Decorated.Units[Index].bDone           = false;
				Decorated.Units[Index].bLockedThisTurn = false;
				break;

			case EDecoration::Alternating:
				// THE TWO BITS DRIVEN APART, because their lifecycles are not the same one.
				Decorated.Units[Index].bDone           = (Index % 2) == 0;
				Decorated.Units[Index].bLockedThisTurn = (Index % 2) != 0;
				break;
			}
		}

		Subsystem->ApplyView(Decorated);

		for (const FStratUnitView& View : Decorated.Units)
		{
			AStratUnitActor* const Actor = Subsystem->FindUnitActor(View.UnitId);
			if (Actor == nullptr)
			{
				AddError(FString::Printf(TEXT("[%s] unit %d has no actor"), PassNames[PassIndex], View.UnitId));
				continue;
			}

			const FStratUnitView& Cached = Actor->GetLastAppliedView();

			// THE EXPECTATION IS THE APPLIED MODEL'S FIELD, not the literal written into it.
			TestEqual(*FString::Printf(
					TEXT("[%s] unit %d: the actor's bDone equals the model's"),
					PassNames[PassIndex], View.UnitId),
				Cached.bDone, View.bDone);
			TestEqual(*FString::Printf(
					TEXT("[%s] unit %d: the actor's bLockedThisTurn equals the model's"),
					PassNames[PassIndex], View.UnitId),
				Cached.bLockedThisTurn, View.bLockedThisTurn);

			// TOTAL, per `ApplyUnitView`'s declaration: nothing of a previous view survives.
			TestEqual(*FString::Printf(TEXT("[%s] unit %d: the cached id is this unit's"),
					PassNames[PassIndex], View.UnitId),
				Cached.UnitId, View.UnitId);
			TestEqual(*FString::Printf(TEXT("[%s] unit %d: the cached side is the model's"),
					PassNames[PassIndex], View.UnitId),
				Cached.Side, View.Side);
			TestEqual(*FString::Printf(TEXT("[%s] unit %d: the cached hp is the model's"),
					PassNames[PassIndex], View.UnitId),
				Cached.Hp, View.Hp);
			TestEqual(*FString::Printf(TEXT("[%s] unit %d: the cached turn flags are the model's"),
					PassNames[PassIndex], View.UnitId),
				Cached.bHasMoved, View.bHasMoved);
			TestEqual(*FString::Printf(TEXT("[%s] unit %d: the cached act flag is the model's"),
					PassNames[PassIndex], View.UnitId),
				Cached.bHasActed, View.bHasActed);
		}
	}

	// AND THE LAST WORD IS THE MODULE'S. After being driven true, false and alternating, the
	// screen is rebuilt from a model the BRIDGE produced -- and every actor reports the
	// builder's default again. An actor that had latched anything of its own would still be
	// carrying it here, and no compiler would have said so.
	{
		FStratViewModel Rebuilt;
		FString RebuildError;
		if (TestTrue(TEXT("the model rebuilds from the bridge"),
				Subsystem->BuildViewModel(Rebuilt, RebuildError)))
		{
			Subsystem->ApplyView(Rebuilt);

			for (const FStratUnitView& View : Rebuilt.Units)
			{
				AStratUnitActor* const Actor = Subsystem->FindUnitActor(View.UnitId);
				if (Actor == nullptr)
				{
					AddError(FString::Printf(TEXT("unit %d has no actor after the rebuild"), View.UnitId));
					continue;
				}
				TestEqual(*FString::Printf(
						TEXT("unit %d's bDone is whatever the rebuilt model says, and nothing it remembered"),
						View.UnitId),
					Actor->GetLastAppliedView().bDone, View.bDone);
				TestEqual(*FString::Printf(
						TEXT("unit %d's bLockedThisTurn likewise"), View.UnitId),
					Actor->GetLastAppliedView().bLockedThisTurn, View.bLockedThisTurn);
			}
		}
		else
		{
			AddError(RebuildError);
		}
	}

	return true;
}
