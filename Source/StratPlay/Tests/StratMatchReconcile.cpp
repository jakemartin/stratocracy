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
// AND SINCE 2026-09-02 THIS FILE ALSO CARRIES THE FIVE ROUTE-TWEEN CLAUSES, at the bottom,
// under their own banner. They are here rather than in a file of their own for one reason:
// `FTestWorldScope` and `MakeConfig` above are this translation unit's, a second copy of a
// world fixture is a second thing to keep true, and the clauses need exactly this fixture. The
// two clauses above are untouched by them -- no shared mutable state, a fresh world per clause,
// and the route clauses configure a DIFFERENT `UnitActorClass` of their own after calling
// `MakeConfig`, so nothing above changes shape. Read that banner for what they pin and, more
// importantly, for the list of things they do NOT.
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
// TEST-ONLY, and required rather than convenient -- see its header: the shipped C++ defaults
// leave `MoveTweenSeconds` at zero, and against them no tween is ever armed.
#include "Tests/StratRouteTweenUnitDouble.h"
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

// ===========================================================================
// THE ROUTE TWEEN -- five clauses, T-INT-05, added 2026-09-02.
//
// WHAT THE FEATURE CLAIMS. A unit ordered to move slides along the hexes the rules module
// actually routed it through, not along the straight chord between the ends; an AI move
// snaps. The mechanism is stated at `UStratMatchSubsystem::NotePendingMoveRoute` and at
// `AStratUnitActor::ApplyUnitView`: a player command notes an ordered axial route, `ApplyView`
// converts it through `AStratBoardActor::WorldLocationOfHex` and hands the world points to
// `ApplyUnitView`, which builds a `Body`-relative polyline; nothing else notes a route, so an
// empty one is the ordinary case and means SNAP.
//
// WHAT THESE FIVE CLAUSES PIN.
//   1. `TweenWaypointsFollowTheNotedRoute` -- the armed polyline has one waypoint per noted
//      hex and each sits over that hex. Asserted on the INTERMEDIATE waypoints, because a
//      chord agrees with a route at both ends and an endpoint-only assertion is passed by the
//      very implementation this feature replaced. The route is chosen to BEND and the bend is
//      itself asserted, so the clause cannot quietly degenerate into a straight-line case.
//   2. `TweenWaypointsSitAtTheDrawnUnitsHeight` -- the Z half, separately, because it is a
//      separate defect: differencing the route against the ACTOR's location instead of against
//      the route's own last point sinks every intermediate waypoint by `BodyZOffset` and is
//      invisible in XY.
//   3. `NoNotedRouteArmsNoTween` -- an apply that MOVES a unit with nothing noted arms
//      nothing. This is the structural form of "AI moves snap": there is no AI detection in
//      the feature and none may be added, so the map's emptiness is the whole signal.
//   4. `SecondApplyWithIdenticalPositionsLeavesTheTweenAlone` -- a player move produces two
//      applies with identical positions; the second must not collapse the running route. The
//      second apply CARRIES A ROUTE OF ITS OWN, and a shorter one, for the reason that clause's
//      banner gives at length: with nothing noted on the second apply, the guard under test is
//      never reached and the clause pins nothing.
//   5. `ASpawnedUnitDoesNotSlideEvenWithARouteNoted` -- a unit that left the model and came
//      back is respawned, snapped and standing still, even with a route noted that fits its
//      arrival exactly. **IT REPLACED `PendingRouteIsEmptiedByEveryApply` ON 2026-09-02, WHICH
//      WAS RETIRED AS AN EQUIVALENT-MUTANT CLAUSE.** See its own banner: nothing in this file
//      pins `UStratMatchSubsystem::PendingMoveRoutes`' unconditional `Reset()`, and the reason
//      is that the hazard that line guards against is foreclosed twice over without it.
//
// WHAT THEY DO NOT PIN, AND THIS LIST IS AS LOAD-BEARING AS THE ONE ABOVE. `FTestWorldScope`
// never begins play and never ticks, so `AStratUnitActor::Tick` NEVER RUNS in any clause here.
// Therefore NOTHING below observes, and nothing below could observe:
//   - the eased position at any moment between the ends -- no midpoint, no sample, nothing;
//   - the trapezoidal velocity profile, its plateau, or `MoveTweenEaseFraction` at all;
//   - the wall-clock duration, or that it is `MoveTweenSeconds` per hex step;
//   - the zero-velocity ends that make a tween replaced mid-flight seamless;
//   - the retirement of a completed tween, which only `Tick` performs.
// A green suite says the POLYLINE IS THE RIGHT SHAPE AND IS ARMED ON THE RIGHT OCCASIONS. It
// says nothing whatever about the motion along it. `Tick` is protected and stays so; no
// test-only advance entry point was asked for or added, and a future clause that wants the
// curve needs a PIE fixture rather than a seam.
//
// WHERE THE EXPECTATIONS COME FROM -- the rule this file already states, applied one layer
// further in. Every expected world point below is `AStratBoardActor::WorldLocationOfHex`, read
// off the board the subsystem spawned. The axial -> world formula appears nowhere here, the
// hex spacing is never written down (it is measured off the board as the distance to a
// nearest neighbour), and `BodyZOffset` is never named as a number: the lift is read back as
// `GetActorLocation().Z - WorldLocationOfHex(hex).Z` and only asserted NON-ZERO, which is what
// makes clause 2 able to fail without making it depend on the fixture's tuning.
//
// THE ROUTE ITSELF IS FIXTURE INPUT AND NOT AN EXPECTATION, and it is not a hand-typed list of
// coordinates. It is DISCOVERED from the board: neighbours are the hexes at the minimum
// positive world distance from a given hex, so no adjacency rule is restated here and none
// could be -- `strat::` hex math is unreachable from this module by the LNK2019 rule, which is
// exactly why the production code asks `FStratBridge::MovePathToHex` instead of walking
// neighbours itself. A four-hex simple path is then chosen to MAXIMISE how far its two
// intermediate hexes sit off the chord, and the clause asserts that distance is a real
// fraction of a tile before it asserts anything else.
//
// FALSIFIABILITY. ALL FIVE ARE NOW DISCHARGED BY MEASUREMENT, AND HOW TWO OF THEM GOT THERE IS
// THE REASON THIS PARAGRAPH IS WORTH READING.
//
// [RESTATED FLAT ON 2026-09-02, THE SAME DAY, AFTER THE MUTANTS WERE ACTUALLY BUILT. NOT
// APPENDED TO, BECAUSE A CORRECTION THAT ONLY ADDS LEAVES BOTH CLAIMS LIVE.] It said:
// RETRACTED> "NO MUTANT WAS BUILT FOR ANY OF THESE FIVE. Building one means editing
// RETRACTED>  `StratUnitActor.cpp` or `StratMatchSubsystem.cpp`, and this lane may not edit a
// RETRACTED>  file outside `Tests/` even temporarily. So each clause below names ... the EXACT
// RETRACTED>  one-line change ... Those are UNDISCHARGED falsifiability debts."
// That was true when it was written and is now false. `strat-gameplay-engineer` -- the lane
// that owns those two files, which is why it could and this one could not -- built all five
// named mutants IN PLACE, rebuilt and re-ran each with the exported report deleted first, and
// restored both files byte-identically afterwards.
//
// THREE WENT RED IMMEDIATELY. TWO STAYED GREEN, which was the finding:
//   - `SecondApplyWithIdenticalPositionsLeavesTheTweenAlone` WAS INERT AS FIRST WRITTEN. It
//     applied the same model twice with nothing noted the second time, so `ApplyView`'s "was a
//     route noted" check refused before the guard under test was ever reached. It passed for a
//     reason unrelated to its own subject. **IT WAS REWRITTEN -- the second apply now carries a
//     shorter route -- AND THE IDENTICAL MUTATION NOW REDDENS IT.** Its banner has the message
//     and the stamp.
//   - `PendingRouteIsEmptiedByEveryApply` WAS INERT AND UNFIXABLE and is RETIRED, replaced by
//     clause 5 below, which is discharged on two independent mutations.
// The moral is written here rather than left in a report: a clause that names a mutation is a
// PROMISE about that mutation, and two of these five were promising something they could not
// deliver. The vacuity assertions each clause carries -- the tween armed at all, the route
// bends, the lift is non-zero, the actor really moved -- were all present and all green in both
// inert clauses, and caught neither. NON-VACUITY IS NOT FALSIFIABILITY, and a clause is worth
// what its mutant run says it is worth and not what its banner argues.
//
// AND ONE LINE IN THE PRODUCTION CODE IS PINNED BY NOTHING HERE, WHICH IS A FINDING AND NOT A
// GAP TO BE CLOSED. `UStratMatchSubsystem::PendingMoveRoutes`' unconditional `Reset()` cannot be
// pinned by any clause: the hazard it guards is foreclosed twice over without it -- see clause
// 5's banner -- so every mutation of that line is an EQUIVALENT MUTANT and a suite that deletes
// it is green. `strat-gameplay-engineer` is keeping the line as correct, direct defence in depth
// behind `bActorMoved`, and has recorded in `engine.md` that deleting it yields a green suite.
// **DO NOT WRITE A CLAUSE FOR IT, and do not cite anything here as covering it.**
//
// THE UNIT ACTOR CLASS IS `AStratRouteTweenUnitDouble` AND THAT IS NOT A CONVENIENCE. Against
// the shipped C++ defaults `MoveTweenSeconds` is 0 and `ApplyUnitView` takes the documented
// no-animation path, so every clause here would assert over an empty polyline and pass over
// any implementation at all. See that class's header.
// ===========================================================================

namespace StratRouteTween
{
	using StratMatchReconcile::FTestWorldScope;

	/** World-space slop. The same 0.01 the clauses above compare positions with. */
	static const double kTolerance = 0.01;

	/**
	 * The board hexes exactly one tile-step from `Of`, MEASURED OFF THE BOARD.
	 *
	 * NO ADJACENCY RULE IS RESTATED. The neighbours of a hex are the hexes at the minimum
	 * positive world distance from it, which the board's own `WorldLocationOfHex` answers.
	 * Writing the axial neighbour offsets here would be a second copy of `strat::` hex math --
	 * unreachable from this module by the LNK2019 rule and, more to the point, a copy that
	 * could agree with itself while disagreeing with the rules module.
	 */
	static TArray<FIntPoint> NeighboursOf(const AStratBoardActor& Board,
		const TArray<FStratHexView>& Hexes, const FIntPoint Of)
	{
		const FVector Origin = Board.WorldLocationOfHex(Of);

		double Nearest = TNumericLimits<double>::Max();
		for (const FStratHexView& Hex : Hexes)
		{
			if (Hex.Hex == Of)
			{
				continue;
			}
			const double Distance = FVector::Dist2D(Origin, Board.WorldLocationOfHex(Hex.Hex));
			if (Distance > kTolerance && Distance < Nearest)
			{
				Nearest = Distance;
			}
		}

		TArray<FIntPoint> Out;
		if (Nearest == TNumericLimits<double>::Max())
		{
			return Out;
		}
		for (const FStratHexView& Hex : Hexes)
		{
			if (Hex.Hex == Of)
			{
				continue;
			}
			const double Distance = FVector::Dist2D(Origin, Board.WorldLocationOfHex(Hex.Hex));
			if (Distance > kTolerance && Distance <= Nearest * 1.05)
			{
				Out.Add(Hex.Hex);
			}
		}
		return Out;
	}

	/** The distance between two adjacent tile centres, off the board. Never written down. */
	static double TileSpacing(const AStratBoardActor& Board, const TArray<FStratHexView>& Hexes,
		const FIntPoint Of)
	{
		const TArray<FIntPoint> Neighbours = NeighboursOf(Board, Hexes, Of);
		if (Neighbours.Num() == 0)
		{
			return 0.0;
		}
		return FVector::Dist2D(Board.WorldLocationOfHex(Of), Board.WorldLocationOfHex(Neighbours[0]));
	}

	/**
	 * A four-hex simple path out of `Start` whose two intermediate hexes sit as far off the
	 * chord as the board allows, avoiding hexes other units stand on.
	 *
	 * WHY MAXIMISE RATHER THAN TAKE THE FIRST BEND. The clause's subject is that the picture
	 * follows the route and not the chord, and the measurement is only as sharp as the
	 * difference between the two. `Deviation` is the MINIMUM of the two intermediates'
	 * distances from the chord, so a large value means BOTH are off it -- a route with one
	 * bend and one collinear step would score as the collinear one.
	 */
	struct FBendingRoute
	{
		TArray<FIntPoint> Hexes;
		double Deviation = 0.0;
	};

	static FBendingRoute FindBendingRoute(const AStratBoardActor& Board, const FStratViewModel& Model,
		const FIntPoint Start, const int32 MovingUnitId)
	{
		TSet<FIntPoint> Occupied;
		for (const FStratUnitView& Unit : Model.Units)
		{
			if (Unit.UnitId != MovingUnitId)
			{
				Occupied.Add(Unit.Hex);
			}
		}

		FBendingRoute Best;
		const FVector P0 = Board.WorldLocationOfHex(Start);

		for (const FIntPoint& A : NeighboursOf(Board, Model.Hexes, Start))
		{
			if (Occupied.Contains(A))
			{
				continue;
			}
			for (const FIntPoint& B : NeighboursOf(Board, Model.Hexes, A))
			{
				if (B == Start || B == A || Occupied.Contains(B))
				{
					continue;
				}
				for (const FIntPoint& C : NeighboursOf(Board, Model.Hexes, B))
				{
					if (C == Start || C == A || C == B || Occupied.Contains(C))
					{
						continue;
					}

					const FVector P3 = Board.WorldLocationOfHex(C);
					const double Deviation = FMath::Min(
						static_cast<double>(FMath::PointDistToSegment(Board.WorldLocationOfHex(A), P0, P3)),
						static_cast<double>(FMath::PointDistToSegment(Board.WorldLocationOfHex(B), P0, P3)));

					if (Deviation > Best.Deviation)
					{
						Best.Deviation = Deviation;
						Best.Hexes = { Start, A, B, C };
					}
				}
			}
		}
		return Best;
	}

	/** What every clause below needs before it can measure anything. */
	struct FRouteFixture
	{
		UStratMatchSubsystem* Subsystem = nullptr;
		AStratBoardActor* Board = nullptr;

		/** The model as the bridge built it. The moving unit stands on `Route[0]`. */
		FStratViewModel AtStart;
		/** The same model with the moving unit standing on `Route.Last()`. */
		FStratViewModel Moved;

		TArray<FIntPoint> Route;
		int32 MovingUnitId = INDEX_NONE;
		double Deviation = 0.0;
		double Spacing = 0.0;
	};

	/**
	 * Starts a live match with the tween double, finds a bending route for some unit, and
	 * applies the starting model so the actors exist and are STANDING STILL on it.
	 *
	 * IT ASSERTS ITS OWN PRECONDITIONS THROUGH `Test`, so a clause whose arrangement silently
	 * failed reddens where it failed rather than passing over an empty world.
	 */
	static bool Arrange(FAutomationTestBase& Test, UWorld* World, FRouteFixture& Out)
	{
		Out.Subsystem = World->GetSubsystem<UStratMatchSubsystem>();
		if (!Test.TestNotNull(TEXT("the world has a match subsystem"), Out.Subsystem))
		{
			return false;
		}

		FStratMatchConfig Config;
		FString Error;
		if (!Test.TestTrue(TEXT("the match config assembles"),
				StratMatchReconcile::MakeConfig(Config, Error)))
		{
			Test.AddError(Error);
			return false;
		}

		// THE ONE DEVIATION FROM `MakeConfig`, AND THE REASON IS IN THE DOUBLE'S HEADER: the
		// shipped C++ class has `MoveTweenSeconds == 0` and arms no tween ever.
		Config.UnitActorClass = AStratRouteTweenUnitDouble::StaticClass();

		FString StartReason;
		Out.Subsystem->StartMatch(Config, StartReason);
		if (!Test.TestTrue(TEXT("the match is live whatever StartMatch returned"),
				Out.Subsystem->IsMatchLive()))
		{
			Test.AddError(StartReason);
			return false;
		}

		Out.Board = Out.Subsystem->GetBoard();
		if (!Test.TestNotNull(TEXT("the subsystem spawned a board to ask for hex locations"), Out.Board))
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
		if (!Test.TestTrue(TEXT("the scenario ships units to move"), Out.AtStart.Units.Num() > 0) ||
			!Test.TestTrue(TEXT("the board has hexes to move across"), Out.AtStart.Hexes.Num() > 0))
		{
			return false;
		}

		// THE FIRST UNIT THAT HAS A BENDING ROUTE, AND THE CHOICE IS THE BOARD'S RATHER THAN
		// THIS FILE'S. Nothing about which unit it is matters to anything asserted below.
		for (const FStratUnitView& Unit : Out.AtStart.Units)
		{
			const double Spacing = TileSpacing(*Out.Board, Out.AtStart.Hexes, Unit.Hex);
			if (Spacing <= kTolerance)
			{
				continue;
			}
			const FBendingRoute Candidate =
				FindBendingRoute(*Out.Board, Out.AtStart, Unit.Hex, Unit.UnitId);
			if (Candidate.Hexes.Num() >= 3 && Candidate.Deviation > Spacing * 0.25)
			{
				Out.MovingUnitId = Unit.UnitId;
				Out.Route = Candidate.Hexes;
				Out.Deviation = Candidate.Deviation;
				Out.Spacing = Spacing;
				break;
			}
		}

		if (!Test.TestNotEqual(
				TEXT("some unit on this board has a route whose middle hexes leave the chord"),
				Out.MovingUnitId, static_cast<int32>(INDEX_NONE)))
		{
			return false;
		}

		// THE BEND, ASSERTED RATHER THAN ASSUMED. Without this the whole file could go quiet
		// the day the board or the scenario changes: a straight route makes every clause below
		// pass over a chord implementation.
		Test.TestTrue(*FString::Printf(
				TEXT("both middle hexes of the route sit well off the chord (%.1f uu off, tile spacing %.1f uu)"),
				Out.Deviation, Out.Spacing),
			Out.Deviation > Out.Spacing * 0.25);
		Test.AddInfo(FString::Printf(
			TEXT("unit %d routed (%d,%d) -> (%d,%d) -> (%d,%d) -> (%d,%d); off-chord %.1f uu of a %.1f uu tile"),
			Out.MovingUnitId,
			Out.Route[0].X, Out.Route[0].Y, Out.Route[1].X, Out.Route[1].Y,
			Out.Route[2].X, Out.Route[2].Y, Out.Route.Last().X, Out.Route.Last().Y,
			Out.Deviation, Out.Spacing));

		Out.Moved = Out.AtStart;
		for (FStratUnitView& Unit : Out.Moved.Units)
		{
			if (Unit.UnitId == Out.MovingUnitId)
			{
				Unit.Hex = Out.Route.Last();
			}
		}

		// THE UNITS ARE PUT ON SCREEN STANDING STILL. This apply SPAWNS them, which takes
		// `SnapToWorldLocation` and arms nothing whatever is noted -- see `ApplyView`'s spawn
		// branch. Every clause below therefore starts from an actor at rest on `Route[0]`.
		Out.Subsystem->ApplyView(Out.AtStart);
		return true;
	}

	/** Where a waypoint puts the picture, in world space: the actor's transform plus the
	 *  `Body`-relative offset, which is exactly what `Tick` lerps between. */
	static FVector WaypointWorld(const AStratUnitActor& Actor, const int32 Index)
	{
		return Actor.GetActorLocation() + Actor.GetTweenWaypoint(Index);
	}
}

// ---------------------------------------------------------------------------
// Clause 1. The armed polyline is the noted route and not the chord across it.
//
// THE ASSERTION IS ON THE INTERMEDIATE WAYPOINTS AND THAT IS THE POINT. A chord and a route
// agree at both ends, so a clause that checked only where the slide starts and stops would be
// passed by the implementation this feature exists to replace -- the two-point fallback
// `ApplyUnitView`'s own block records as DELETED. Every waypoint is compared against
// `WorldLocationOfHex` of the hex at the same index of the noted route, and then the polyline
// as drawn is required to leave the chord by a real fraction of a tile.
//
// COUNT IS ASSERTED SEPARATELY FROM POSITION, because they fail differently: a truncated
// polyline that happened to start right would satisfy an index-by-index loop over its own
// length.
//
// THE TERMINAL WAYPOINT IS ASSERTED EXACTLY ZERO -- `IsZero()`, not `IsNearlyZero()` --
// because `TweenOffsets`' own block states it is APPENDED rather than computed, precisely so
// that claim rests on construction rather than on two floating-point expressions agreeing.
// A tolerance here would let the construction be replaced by the arithmetic without notice.
//
// WHAT REDDENS IT -- DISCHARGED BY MEASUREMENT, 2026-09-02. [RESTATED FLAT, NOT APPENDED TO,
// BECAUSE A CORRECTION THAT ONLY ADDS LEAVES BOTH CLAIMS LIVE.] It said:
// RETRACTED> "WHAT WOULD REDDEN IT -- UNDISCHARGED, NO MUTANT BUILT, THIS LANE MAY NOT BUILD
// RETRACTED>  ONE."
// It was built, by `strat-gameplay-engineer` and in place, in
// `AStratUnitActor::ApplyUnitView` (`Source/StratPlay/StratUnitActor.cpp`): the loop appending
// `RouteWorldPoints[Index] - RouteEnd` for every index was replaced by the two-point chord it
// superseded. THIS CLAUSE WENT RED, with its own message -- "expected 4 waypoints, was 2" --
// and the file was restored byte-identically afterwards.
//
// THE STAMP THIS DISCHARGE IS CITED BY, AND WHAT THIS LANE CAN AND CANNOT ATTEST. The
// measurement is `strat-gameplay-engineer`'s, not this lane's: it owns the mutated file, built
// the mutant in place, rebuilt, re-ran with the report deleted first, and restored the file
// byte-identically. **THE PER-MUTANT `reportCreatedOn` FOR THIS FIRST MUTANT PASS WAS NOT
// REPORTED TO THIS LANE**, and `Saved/AutomationReport/index.json` is overwritten IN PLACE by
// every run, so those reports no longer exist to cite. The one artifact of that pass this lane
// observed directly was the restored run's exported report,
// `reportCreatedOn 2026.09.02-20.30.33`, which this lane recorded and then deleted before its
// own. Said rather than glossed: the failure message quoted above is another lane's reported
// observation, and only the restored stamp is one this lane took itself.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRouteTweenWaypointsFollowTheNotedRouteTest,
	"Stratocracy.StratPlay.T-INT-05.TweenWaypointsFollowTheNotedRoute",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRouteTweenWaypointsFollowTheNotedRouteTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRouteTween;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	FRouteFixture Fixture;
	if (!Arrange(*this, Scope.World, Fixture))
	{
		return false;
	}

	Fixture.Subsystem->NotePendingMoveRoute(Fixture.MovingUnitId, Fixture.Route);
	Fixture.Subsystem->ApplyView(Fixture.Moved);

	AStratUnitActor* const Actor = Fixture.Subsystem->FindUnitActor(Fixture.MovingUnitId);
	if (!TestNotNull(TEXT("the unit that moved still has an actor"), Actor))
	{
		return false;
	}

	// ONE WAYPOINT PER NOTED HEX. See `GetTweenWaypointCount`: it is a count of waypoints, and
	// the terminal one is appended rather than converted, so an N-hex route arms N waypoints.
	if (!TestEqual(TEXT("the armed polyline has one waypoint per hex of the noted route"),
			Actor->GetTweenWaypointCount(), Fixture.Route.Num()))
	{
		return false;
	}

	TestTrue(TEXT("the terminal waypoint is exactly zero, as it is constructed to be"),
		Actor->GetTweenWaypoint(Fixture.Route.Num() - 1).IsZero());

	// EVERY WAYPOINT OVER ITS OWN HEX, THE BOARD'S ANSWER ON THE EXPECTATION SIDE.
	for (int32 Index = 0; Index < Fixture.Route.Num(); ++Index)
	{
		const FVector Expected = Fixture.Board->WorldLocationOfHex(Fixture.Route[Index]);
		const FVector Actual   = WaypointWorld(*Actor, Index);

		TestTrue(*FString::Printf(
				TEXT("waypoint %d stands over WorldLocationOfHex(%d, %d) -- it draws at %s, the hex is at %s"),
				Index, Fixture.Route[Index].X, Fixture.Route[Index].Y,
				*Actual.ToString(), *Expected.ToString()),
			FMath::IsNearlyEqual(Actual.X, Expected.X, kTolerance) &&
			FMath::IsNearlyEqual(Actual.Y, Expected.Y, kTolerance));
	}

	// AND THE DRAWN POLYLINE LEAVES THE CHORD. This is the assertion a straight-line
	// implementation fails while passing every endpoint check above -- measured on the
	// waypoints as drawn, not on the hexes that produced them.
	{
		const FVector First = WaypointWorld(*Actor, 0);
		const FVector Last  = WaypointWorld(*Actor, Fixture.Route.Num() - 1);

		double Nearest = TNumericLimits<double>::Max();
		for (int32 Index = 1; Index < Fixture.Route.Num() - 1; ++Index)
		{
			Nearest = FMath::Min(Nearest,
				static_cast<double>(FMath::PointDistToSegment(WaypointWorld(*Actor, Index), First, Last)));
		}

		TestTrue(*FString::Printf(
				TEXT("every intermediate waypoint leaves the chord (nearest is %.1f uu off a %.1f uu tile) -- a chord would be 0"),
				Nearest, Fixture.Spacing),
			Nearest > Fixture.Spacing * 0.25);
	}

	return true;
}

// ---------------------------------------------------------------------------
// Clause 2. The waypoints sit at the height the unit is DRAWN at, not on the tile plane.
//
// WHY THIS IS A SEPARATE CLAUSE AND NOT A THIRD AXIS ON CLAUSE 1. The defect it catches is a
// one-token difference in `ApplyUnitView` -- differencing the route points against the ACTOR's
// location rather than against the route's own last point -- and it is invisible in XY,
// because the actor and the route agree there exactly. It shows up only in Z, as every
// intermediate waypoint sitting `BodyZOffset` below where the unit is drawn: the picture dips
// into the board and climbs back out. `Tools/architect/state/engine.md` records that the
// engineer caught it while writing the comment and refused it; this is the clause that would
// have.
//
// IT WOULD BE VACUOUS AT ZERO LIFT AND THAT IS ASSERTED BEFORE ANYTHING ELSE. The shipped C++
// `BodyZOffset` is 0.0f -- against it the tile plane and the drawn unit are the same height
// and every assertion below is a tautology. `AStratRouteTweenUnitDouble` carries a non-zero
// one for exactly this reason, and the clause reads the lift BACK off the drawn actor
// (`GetActorLocation().Z` minus the board's Z for the hex it stands on, which is
// `ApplyUnitView`'s own location expression read in reverse) rather than naming the number.
// So the measurement survives the fixture being retuned, and dies loudly if it is zeroed.
//
// WHAT REDDENS IT -- DISCHARGED BY MEASUREMENT, 2026-09-02, AND THIS IS THE SHARPEST OF THE
// FIVE. [RESTATED FLAT, NOT APPENDED TO.] It said:
// RETRACTED> "WHAT WOULD REDDEN IT -- UNDISCHARGED, NO MUTANT BUILT, THIS LANE MAY NOT BUILD
// RETRACTED>  ONE."
// `strat-gameplay-engineer` changed `const FVector RouteEnd = RouteWorldPoints.Last();` to
// `= Destination;` in `AStratUnitActor::ApplyUnitView` (`Source/StratPlay/StratUnitActor.cpp`).
// THIS CLAUSE WENT RED ALONE -- "waypoint 1 draws at the unit's own height over hex (-1, 4):
// Z 0.00, expected 37.00" -- AND CLAUSE 1 STAYED GREEN UNDER THE SAME MUTATION, which is
// exactly what the pair was written to do: the XY difference is identical, so the two are
// independent and the height defect has a net of its own. The file was restored
// byte-identically afterwards.
//
// THE STAMP THIS DISCHARGE IS CITED BY, AND WHAT THIS LANE CAN AND CANNOT ATTEST. The
// measurement is `strat-gameplay-engineer`'s, not this lane's: it owns the mutated file, built
// the mutant in place, rebuilt, re-ran with the report deleted first, and restored the file
// byte-identically. **THE PER-MUTANT `reportCreatedOn` FOR THIS FIRST MUTANT PASS WAS NOT
// REPORTED TO THIS LANE**, and `Saved/AutomationReport/index.json` is overwritten IN PLACE by
// every run, so those reports no longer exist to cite. The one artifact of that pass this lane
// observed directly was the restored run's exported report,
// `reportCreatedOn 2026.09.02-20.30.33`, which this lane recorded and then deleted before its
// own. Said rather than glossed: the failure message quoted above is another lane's reported
// observation, and only the restored stamp is one this lane took itself.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRouteTweenWaypointsSitAtTheDrawnUnitsHeightTest,
	"Stratocracy.StratPlay.T-INT-05.TweenWaypointsSitAtTheDrawnUnitsHeight",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRouteTweenWaypointsSitAtTheDrawnUnitsHeightTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRouteTween;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	FRouteFixture Fixture;
	if (!Arrange(*this, Scope.World, Fixture))
	{
		return false;
	}

	Fixture.Subsystem->NotePendingMoveRoute(Fixture.MovingUnitId, Fixture.Route);
	Fixture.Subsystem->ApplyView(Fixture.Moved);

	AStratUnitActor* const Actor = Fixture.Subsystem->FindUnitActor(Fixture.MovingUnitId);
	if (!TestNotNull(TEXT("the unit that moved still has an actor"), Actor))
	{
		return false;
	}
	if (!TestEqual(TEXT("a tween is in flight to measure the height of"),
			Actor->GetTweenWaypointCount(), Fixture.Route.Num()))
	{
		return false;
	}

	// THE LIFT, READ BACK OFF THE DRAWN ACTOR. `ApplyUnitView` writes
	// `WorldLocation + (0, 0, BodyZOffset)`; this is that expression inverted against the
	// board's own answer for the hex the unit now stands on. No number is named here.
	const FVector DestinationPlane = Fixture.Board->WorldLocationOfHex(Fixture.Route.Last());
	const double Lift = Actor->GetActorLocation().Z - DestinationPlane.Z;

	// NON-VACUITY FIRST. At zero lift every assertion below is a tautology and this clause
	// would report the defect it exists to catch as absent.
	if (!TestTrue(*FString::Printf(
			TEXT("the fixture draws the unit above the tile plane, so this clause can fail at all (lift %.1f uu)"),
			Lift),
		FMath::Abs(Lift) > 1.0))
	{
		return false;
	}
	AddInfo(FString::Printf(TEXT("the drawn unit sits %.1f uu above the tile plane"), Lift));

	for (int32 Index = 0; Index < Fixture.Route.Num(); ++Index)
	{
		const FVector Plane  = Fixture.Board->WorldLocationOfHex(Fixture.Route[Index]);
		const FVector Actual = WaypointWorld(*Actor, Index);

		TestTrue(*FString::Printf(
				TEXT("waypoint %d draws at the unit's own height over hex (%d, %d): Z %.2f, expected %.2f (plane %.2f + lift %.2f)"),
				Index, Fixture.Route[Index].X, Fixture.Route[Index].Y,
				Actual.Z, Plane.Z + Lift, Plane.Z, Lift),
			FMath::IsNearlyEqual(Actual.Z, Plane.Z + Lift, kTolerance));
	}

	return true;
}

// ---------------------------------------------------------------------------
// Clause 3. No noted route, no tween -- which is the whole of "AI moves snap".
//
// THE PROPERTY IS STRUCTURAL AND THE CLAUSE IS WRITTEN TO SAY SO. There is no AI detection
// anywhere in this feature and none may be added: `NotePendingMoveRoute`'s own declaration
// records that nothing but an accepted player command calls it, so the map's emptiness IS the
// signal. This clause moves a unit exactly as clause 1 does and notes NOTHING, and requires
// that no polyline is armed. A load, a reseed and a path query that refused all arrive here
// too, and snapping is the right answer to each.
//
// IT ASSERTS THE ACTOR REALLY MOVED, which is not decoration: `ApplyUnitView` also arms
// nothing when the actor did not move (clause 4's subject), so without this the clause could
// go green for the wrong reason and would keep going green if the route branch were deleted
// outright.
//
// WHAT REDDENS IT -- DISCHARGED BY MEASUREMENT, 2026-09-02. [RESTATED FLAT, NOT APPENDED TO.]
// It said:
// RETRACTED> "WHAT WOULD REDDEN IT -- UNDISCHARGED, NO MUTANT BUILT, THIS LANE MAY NOT BUILD
// RETRACTED>  ONE."
// `strat-gameplay-engineer` restored the deleted degenerate fallback in
// `AStratUnitActor::ApplyUnitView` (`Source/StratPlay/StratUnitActor.cpp`) -- on an empty
// `RouteWorldPoints`, arm the two-point chord `{ Drawn - Destination, FVector::ZeroVector }`.
// THIS CLAUSE WENT RED -- "expected 0, was 2" -- and the file was restored byte-identically
// afterwards. It is the only thing standing between the tree and that chord's return.
//
// THE STAMP THIS DISCHARGE IS CITED BY, AND WHAT THIS LANE CAN AND CANNOT ATTEST. The
// measurement is `strat-gameplay-engineer`'s, not this lane's: it owns the mutated file, built
// the mutant in place, rebuilt, re-ran with the report deleted first, and restored the file
// byte-identically. **THE PER-MUTANT `reportCreatedOn` FOR THIS FIRST MUTANT PASS WAS NOT
// REPORTED TO THIS LANE**, and `Saved/AutomationReport/index.json` is overwritten IN PLACE by
// every run, so those reports no longer exist to cite. The one artifact of that pass this lane
// observed directly was the restored run's exported report,
// `reportCreatedOn 2026.09.02-20.30.33`, which this lane recorded and then deleted before its
// own. Said rather than glossed: the failure message quoted above is another lane's reported
// observation, and only the restored stamp is one this lane took itself.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRouteTweenNoNotedRouteArmsNoTweenTest,
	"Stratocracy.StratPlay.T-INT-05.NoNotedRouteArmsNoTween",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRouteTweenNoNotedRouteArmsNoTweenTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRouteTween;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	FRouteFixture Fixture;
	if (!Arrange(*this, Scope.World, Fixture))
	{
		return false;
	}

	AStratUnitActor* const Actor = Fixture.Subsystem->FindUnitActor(Fixture.MovingUnitId);
	if (!TestNotNull(TEXT("the unit to be moved has an actor before the move"), Actor))
	{
		return false;
	}
	const FVector Before = Actor->GetActorLocation();

	// NOTHING IS NOTED. This is the AI's move, a load, a reseed and a refused path query, all
	// of which reach `ApplyUnitView` with an empty route.
	Fixture.Subsystem->ApplyView(Fixture.Moved);

	// THE MOVE REALLY HAPPENED. Without this the clause would also pass over a unit that
	// stood still, which is a different function's guarantee.
	TestTrue(*FString::Printf(TEXT("the unit really moved (%s -> %s)"),
			*Before.ToString(), *Actor->GetActorLocation().ToString()),
		!Before.Equals(Actor->GetActorLocation()));

	TestEqual(TEXT("a move with no noted route arms no tween at all"),
		Actor->GetTweenWaypointCount(), 0);

	return true;
}

// ---------------------------------------------------------------------------
// Clause 4. An apply that changes no position leaves the running tween completely alone.
//
// WHY THERE IS A SECOND APPLY AT ALL. `AStratPlayerController` documents the ordering as
// deliberate: a player move produces TWO `ApplyView` calls with identical positions.
// `ApplyUnitView`'s block records what that costs a POLYLINE and did not cost a chord --
// re-arming a zero-length chord was harmless, while re-arming a route would collapse it in the
// same frame it started. The guard is `&& bActorMoved` on the arming condition.
//
// THIS CLAUSE WAS INERT AS FIRST WRITTEN, AND THE REWRITE IS THE INTERESTING PART.
// [RESTATED FLAT ON 2026-09-02, THE SAME DAY IT WAS WRITTEN, AFTER THE MUTANT WAS BUILT AND
// THE CLAUSE STAYED GREEN. NOT APPENDED TO, BECAUSE A CORRECTION THAT ONLY ADDS LEAVES BOTH
// CLAIMS LIVE.] The first version said:
// RETRACTED> "WHAT WOULD REDDEN IT ... drop `&& bActorMoved` from the arming condition. The
// RETRACTED>  polyline is then rebuilt on the second apply ... and this clause sees the list
// RETRACTED>  change under it."
// IT DOES NOT. `strat-gameplay-engineer` dropped `&& bActorMoved` in place, rebuilt -- the
// immediate re-build reported "Target is up to date", so the mutation really was in the binary
// -- and the whole suite stayed green. The cause is visible in the old arrangement: it applied
// the same model twice AND NOTED A ROUTE ONLY ONCE, and `UStratMatchSubsystem::ApplyView`
// empties `PendingMoveRoutes` on every apply. So the second apply carried an EMPTY route,
// `ApplyUnitView` refused at `RouteWorldPoints.Num() > 0` -- a different condition, several
// tokens earlier -- and the guard under test was never reached. The clause passed for a reason
// that had nothing to do with its own subject.
//
// AND THE OBVIOUS REPAIR IS NOT ENOUGH EITHER, WHICH IS WHY THE ROUTE BELOW IS A DIFFERENT ONE
// AND NOT THE SAME ONE NOTED TWICE. Re-noting the SAME route leaves the mutation EQUIVALENT,
// which is derivable from the code and is stated here so that nobody has to re-derive it: on
// the second apply `Drawn` is `Body->GetComponentLocation()`, and `Body` is sitting at
// `TweenOffsets[0]`, so `Drawn - Destination` reproduces `TweenOffsets[0]` exactly; every other
// waypoint is `RouteWorldPoints[i] - RouteEnd` over the same route, so the rebuilt array is
// IDENTICAL ELEMENT BY ELEMENT. A mutant that rebuilds the same numbers is invisible to any
// assertion about the numbers. `TweenElapsed` restarting is the real cost of a re-arm and there
// is no accessor for it, nor should this file ask for one.
//
// SO THE SECOND APPLY CARRIES A SHORTER ROUTE: THE CHORD. Both of `ApplyUnitView`'s earlier
// conditions are then satisfied -- a route is present and its last hex is the unit's hex -- and
// `&& bActorMoved` is THE ONLY THING that can refuse the re-arm. Under the shipped code the
// four-waypoint route stays in flight untouched; without the guard the polyline collapses to
// the two-point chord, which is the failure `ApplyUnitView`'s own block names, verbatim. The
// clause therefore measures the guard rather than its neighbours.
//
// THE MEASUREMENT IS THE WAYPOINT LIST, ELEMENT BY ELEMENT AND EXACTLY. Exact equality rather
// than a tolerance, because the correct behaviour is that NOTHING WAS WRITTEN, not that
// something was rewritten to nearly the same value.
//
// WHAT REDDENS IT -- DISCHARGED BY MEASUREMENT, 2026-09-02, AND THE REWRITE ABOVE IS WHAT MADE
// THE DIFFERENCE. [RESTATED FLAT, NOT APPENDED TO.] This block said, one revision ago:
// RETRACTED> "UNDISCHARGED AGAIN AS OF THIS REWRITE ... NOBODY HAS RUN THAT MUTANT AGAINST
// RETRACTED>  THIS VERSION ... until somebody does this banner is an argument and not a
// RETRACTED>  measurement."
// Somebody did. `strat-gameplay-engineer` dropped `&& bActorMoved` from the arming condition in
// `AStratUnitActor::ApplyUnitView` (`Source/StratPlay/StratUnitActor.cpp`), rebuilt, and ran the
// suite -- `reportCreatedOn 2026.09.02-20.47.31`. THIS CLAUSE WENT RED:
//   "Expected 'the second apply leaves the waypoint count untouched -- 4, not the chord's 2'
//    to be 4, but it was 2."
// **THE IDENTICAL MUTATION REDDENED NOTHING AGAINST THE PREVIOUS VERSION OF THIS CLAUSE**, so
// the discharge is attributable to the rewrite and not to a lucky arrangement -- and the count
// fell to the CHORD'S TWO, which is the predicted MECHANISM and not merely the predicted
// direction. `bActorMoved` is pinned. The file was restored byte-identically afterwards, and
// the same mutation also reddened clause 5, which is the second net that clause claims.
// The measurement is that lane's, not this one's: this lane owns no file it could mutate.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRouteTweenSecondApplyLeavesTheTweenAloneTest,
	"Stratocracy.StratPlay.T-INT-05.SecondApplyWithIdenticalPositionsLeavesTheTweenAlone",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRouteTweenSecondApplyLeavesTheTweenAloneTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRouteTween;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	FRouteFixture Fixture;
	if (!Arrange(*this, Scope.World, Fixture))
	{
		return false;
	}

	Fixture.Subsystem->NotePendingMoveRoute(Fixture.MovingUnitId, Fixture.Route);
	Fixture.Subsystem->ApplyView(Fixture.Moved);

	AStratUnitActor* const Actor = Fixture.Subsystem->FindUnitActor(Fixture.MovingUnitId);
	if (!TestNotNull(TEXT("the unit that moved still has an actor"), Actor))
	{
		return false;
	}
	if (!TestEqual(TEXT("a tween is in flight for the second apply to disturb"),
			Actor->GetTweenWaypointCount(), Fixture.Route.Num()))
	{
		return false;
	}

	TArray<FVector> Armed;
	for (int32 Index = 0; Index < Actor->GetTweenWaypointCount(); ++Index)
	{
		Armed.Add(Actor->GetTweenWaypoint(Index));
	}
	const FVector ArmedActorLocation = Actor->GetActorLocation();

	// THE CHORD, NOTED FOR THE SECOND APPLY. Two points: where the unit started and where it
	// now is. See the banner -- with nothing noted the second apply is refused several tokens
	// earlier than the guard under test, and with the SAME route noted the re-arm reproduces
	// the identical array. A shorter route is what makes the guard's effect visible at all.
	const TArray<FIntPoint> Chord = { Fixture.Route[0], Fixture.Route.Last() };
	if (!TestTrue(TEXT("the second route is a different, shorter shape that ends on the same hex"),
			Chord.Num() < Fixture.Route.Num() && Chord.Last() == Fixture.Route.Last()))
	{
		return false;
	}
	Fixture.Subsystem->NotePendingMoveRoute(Fixture.MovingUnitId, Chord);

	// THE SECOND APPLY. The same model again, so every position is identical -- which is
	// precisely the sequence the controller produces for one player move.
	Fixture.Subsystem->ApplyView(Fixture.Moved);

	if (!TestEqual(*FString::Printf(
				TEXT("the second apply leaves the waypoint count untouched -- %d, not the chord's %d"),
				Armed.Num(), Chord.Num()),
			Actor->GetTweenWaypointCount(), Armed.Num()))
	{
		return false;
	}
	for (int32 Index = 0; Index < Armed.Num(); ++Index)
	{
		TestTrue(*FString::Printf(
				TEXT("waypoint %d is the same vector it was before the second apply (%s, now %s)"),
				Index, *Armed[Index].ToString(), *Actor->GetTweenWaypoint(Index).ToString()),
			Actor->GetTweenWaypoint(Index) == Armed[Index]);
	}
	TestTrue(TEXT("and the actor is still standing on its destination"),
		Actor->GetActorLocation().Equals(ArmedActorLocation));

	return true;
}

// ---------------------------------------------------------------------------
// Clause 5. A unit that comes back from absence is snapped, not slid -- even holding a route
// that fits its arrival exactly.
//
// THIS CLAUSE REPLACED `PendingRouteIsEmptiedByEveryApply` ON 2026-09-02, THE SAME DAY, AND THE
// RETIREMENT IS RECORDED HERE RATHER THAN DELETED WITH IT. [WRITTEN FLAT. THE OLD CLAUSE IS
// GONE FROM THE FILE; WHAT IT CLAIMED IS NOT, BECAUSE A READER ARRIVING BY A CITATION TO IT
// MUST LAND ON WHAT REPLACED IT.] It claimed:
// RETRACTED> "`PendingRouteIsEmptiedByEveryApply` -- a route noted for a unit ABSENT from the
// RETRACTED>  next model must not reach a later apply and animate an unrelated move ... WHAT
// RETRACTED>  WOULD REDDEN IT: replace the unconditional `PendingMoveRoutes.Reset()` after the
// RETRACTED>  destroy loop with a removal of only the ids the unit loop consumed."
// IT WAS AN EQUIVALENT MUTANT AND COULD NOT FAIL. `strat-gameplay-engineer` built that mutation
// in `UStratMatchSubsystem::ApplyView` and the clause stayed green, then built two more --
// dropping `&& bActorMoved`, and dropping the last-hex equality -- and it stayed green under
// both. The cause is in the retired clause's own arrangement: its middle step re-applied a
// model that CONTAINED the unit, which put the id back in the seen set, so the narrowed reset
// removed the route there anyway.
//
// AND THE DEEPER REASON IS WORTH MORE THAN THE CLAUSE WAS. The hazard is foreclosed twice over
// and neither foreclosure is the map: a unit absent from a model has its ACTOR DESTROYED, so on
// any later apply a stranded route could attach to, the actor is FRESHLY SPAWNED, snapped by
// `UStratMatchSubsystem::ApplyView`'s spawn branch, and `ApplyUnitView`'s actor-moved guard is
// therefore false. SO NOTHING IN THIS FILE PINS THAT `Reset()`, and this banner says so rather
// than implying otherwise. `strat-gameplay-engineer` is keeping the line -- "currently
// unobservable" is not a reason to delete a correct one -- and that is a decision about the
// code, not a coverage claim, and must not be cited as one.
//
// WHAT THIS CLAUSE PINS INSTEAD, AND IT IS THE MECHANISM THAT ACTUALLY DOES THE FORECLOSING.
// The unit leaves the model and its actor is destroyed; a route is then noted that ends
// EXACTLY where the unit is about to reappear, so `ApplyView`'s last-hex equality accepts it
// and the world points reach `ApplyUnitView`; the unit comes back. It must arrive STANDING
// STILL. `SnapToWorldLocation`'s own declaration states the guarantee -- "a unit that just
// appeared must not slide in from wherever it was standing" -- and `ApplyView`'s spawn branch
// states the failure it prevents: without it "every unit on a fresh board would ease in from
// the world origin". Nothing pinned either sentence before this clause.
//
// IT IS NON-VACUOUS BY CONSTRUCTION AND SAYS SO IN ITS OWN ASSERTIONS: the route really is
// non-empty, it really does end on the hex the model brings the unit back to, and the unit
// really was absent a moment earlier. Without those three a green result would be
// indistinguishable from a clause whose route was silently refused upstream -- which is exactly
// how the retired clause managed to pin nothing.
//
// WHAT REDDENS IT -- DISCHARGED BY MEASUREMENT, 2026-09-02, ON BOTH OF THE TWO INDEPENDENT
// MUTATIONS THIS BANNER NAMED. [RESTATED FLAT, NOT APPENDED TO.] It said:
// RETRACTED> "NEITHER HAS BEEN RUN AGAINST THIS CLAUSE, this lane may not build them, and
// RETRACTED>  until somebody does the two sentences above are arguments and not measurements."
// `strat-gameplay-engineer` ran both, in place, rebuilding and re-running each and restoring
// `Source/StratPlay/StratMatchSubsystem.cpp` and `Source/StratPlay/StratUnitActor.cpp`
// byte-identically afterwards.
//
// (1) DELETING `Spawned->SnapToWorldLocation(Where);` FROM `UStratMatchSubsystem::ApplyView`'S
// SPAWN BRANCH -- `reportCreatedOn 2026.09.02-20.49.51`. THIS CLAUSE WENT RED:
//   "Expected 'a freshly spawned actor arms no tween, though a 4-hex route ending on its own
//    hex was noted' to be 0, but it was 4."
// **AND IT WENT RED ALONE.** `T-INT-05.ApplyViewSpawnsMovesAndDestroys` STAYED GREEN under the
// same deletion, and the reason is worth more than the discharge: `ApplyUnitView`'s own
// `SetActorLocation` writes the destination microseconds later, so the spawn snap is
// UNOBSERVABLE IN THE FINAL POSITION and observable only in whether a tween ARMS. A clause that
// reads a position after `ApplyView` cannot see that line at all. This one can, and it is the
// only thing in the tree that does.
//
// (2) DROPPING `&& bActorMoved` IN `AStratUnitActor::ApplyUnitView` -- `reportCreatedOn
// 2026.09.02-20.47.31`. THIS CLAUSE WENT RED with the same message, alongside clause 4, which
// is the SECOND NET this banner claimed: the guard is reached here by a spawn rather than by a
// second apply.
// The measurement is that lane's, not this one's: this lane owns no file it could mutate.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRouteTweenSpawnedUnitDoesNotSlideTest,
	"Stratocracy.StratPlay.T-INT-05.ASpawnedUnitDoesNotSlideEvenWithARouteNoted",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRouteTweenSpawnedUnitDoesNotSlideTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRouteTween;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	FRouteFixture Fixture;
	if (!Arrange(*this, Scope.World, Fixture))
	{
		return false;
	}

	// ---- The unit leaves the model, which destroys its actor --------------
	FStratViewModel Without = Fixture.AtStart;
	Without.Units.RemoveAll([&Fixture](const FStratUnitView& Unit)
	{
		return Unit.UnitId == Fixture.MovingUnitId;
	});
	if (!TestEqual(TEXT("the unit really left the model"),
			Without.Units.Num(), Fixture.AtStart.Units.Num() - 1))
	{
		return false;
	}

	Fixture.Subsystem->ApplyView(Without);
	if (!TestNull(TEXT("with no model entry the unit has no actor at all"),
			Fixture.Subsystem->FindUnitActor(Fixture.MovingUnitId)))
	{
		return false;
	}

	// ---- A route is noted that ENDS where the unit is about to reappear ---
	// Reversed, so its last hex is `Route[0]` -- the hex `AtStart` puts this unit on. That is
	// what `ApplyView`'s last-hex equality requires, and satisfying it is what makes the result
	// below attributable to the spawn's snap rather than to a route that was refused upstream.
	TArray<FIntPoint> ReturnRoute;
	for (int32 Index = Fixture.Route.Num() - 1; Index >= 0; --Index)
	{
		ReturnRoute.Add(Fixture.Route[Index]);
	}
	if (!TestTrue(TEXT("the noted route is non-empty and ends on the hex the model brings the "
				"unit back to"),
			ReturnRoute.Num() >= 3 && ReturnRoute.Last() == Fixture.Route[0]))
	{
		return false;
	}
	Fixture.Subsystem->NotePendingMoveRoute(Fixture.MovingUnitId, ReturnRoute);

	// ---- And it comes back ------------------------------------------------
	Fixture.Subsystem->ApplyView(Fixture.AtStart);

	AStratUnitActor* const Actor = Fixture.Subsystem->FindUnitActor(Fixture.MovingUnitId);
	if (!TestNotNull(TEXT("the unit is back on screen"), Actor))
	{
		return false;
	}

	// It arrived where the model put it -- the board's answer, as everywhere else in this file.
	{
		const FVector Expected = Fixture.Board->WorldLocationOfHex(Fixture.Route[0]);
		const FVector Actual   = Actor->GetActorLocation();
		TestTrue(*FString::Printf(
				TEXT("the returning unit stands over WorldLocationOfHex(%d, %d) -- it is at %s, the hex is at %s"),
				Fixture.Route[0].X, Fixture.Route[0].Y, *Actual.ToString(), *Expected.ToString()),
			FMath::IsNearlyEqual(Actual.X, Expected.X, kTolerance) &&
			FMath::IsNearlyEqual(Actual.Y, Expected.Y, kTolerance));
	}

	// THE CLAUSE. A unit that just appeared does not slide in, whatever route it is holding.
	TestEqual(*FString::Printf(
			TEXT("a freshly spawned actor arms no tween, though a %d-hex route ending on its own hex was noted"),
			ReturnRoute.Num()),
		Actor->GetTweenWaypointCount(), 0);

	return true;
}

// ===========================================================================
// THE AI'S ROUTE SLIDE -- five more clauses, T-INT-05, added 2026-09-02.
//
// A DIFFERENT VERB WITH A DIFFERENT SHAPE, WHICH IS WHY THESE ARE NOT MORE ASSERTIONS INSIDE
// THE FIVE ABOVE. The block above is about `AStratUnitActor::ApplyUnitView`, which arms a tween
// that ENDS WHERE THE ACTOR ALREADY IS. `AStratUnitActor::PlayRouteSlide` is the AI tour's entry
// point and it PARKS: it takes an anchor from the caller, differences every route point against
// it, and comes to rest over the route's own LAST hex -- which during a hand-over of several
// moves is an intermediate hex the actor does not stand on. Every assumption the five clauses
// above are built on ("the terminal waypoint is exactly zero", "at rest the picture is home")
// is false on this path by construction, so a sixth assertion bolted into one of them would
// have had to weaken it.
//
// THE INVARIANT THAT SPANS BOTH PATHS, AND IT IS THE ENGINEER'S WORDING RATHER THAN THIS
// LANE'S. `AStratUnitActor::TweenOffsets`' own block was RE-SCOPED on 2026-09-02: it now reads
// "`Body` at relative `TweenOffsets.Last()` draws the unit over the route's own last hex", of
// which the terminal zero clause 1 above asserts is the special case where that hex is also the
// actor's. Clause 6 below is the sibling that pins the general form; clause 1 above still pins
// the special one, unweakened and unedited.
//
// WHAT THESE FIVE PIN.
//   6. `RouteSlideParksOverTheRoutesLastHex` -- the general invariant. A route whose last hex
//      is NOT the actor's arms a terminal waypoint that is NOT zero and that stands over that
//      hex. This is the clause that would redden if `PlayRouteSlide` were made to copy
//      `ApplyUnitView`'s construction.
//   7. `AFirstRouteSlideArmsANonDegeneratePolyline` -- the defect the engineer caught by
//      reading, which no clause above can see. See its own banner: the brief that produced this
//      feature specified an unconditional overwrite of waypoint 0, and at a tour's start that
//      expression is ZERO, so the commonest reel entry of all -- a one-hex move, two waypoints
//      -- collapses to {0, 0}, arc length zero, and NOTHING ANIMATES with a green build.
//   8. `RouteSlideWaypointsSitAtTheDrawnUnitsHeight` -- the anchor's lift cancellation on THIS
//      path, mirroring clause 2 above on the other one. `PlayRouteSlide`'s declaration forbids
//      deriving the anchor from the actor transform BY NAME; this is the clause that collects.
//   9. `AParkedPictureSurvivesAnApplyWithIdenticalPositions` -- the hover hazard, and it is the
//      half a naive clause omits. `AStratPlayerController::Tick` reaches `ApplyView` whenever
//      the cursor crosses a hex boundary, so `ApplyUnitView` fires DURING a tour; an
//      unconditional park clear would pop every parked unit on the next mouse move.
//  10. `AnActorThatMovedWithNoRouteClearsItsPark` -- the other half of the same branch: an
//      actor that GENUINELY moved with nothing armed has its park hard-cut. Clauses 9 and 10
//      are a matched pair over one `if`, and each is the other's control.
//
// WHAT THEY DO NOT PIN, AND THE LIST FROM THE BLOCK ABOVE APPLIES HERE WORD FOR WORD.
// `FTestWorldScope` never begins play and never ticks, so `AStratUnitActor::Tick` NEVER RUNS.
// Nothing here observes the eased position at any instant, the velocity profile, the wall-clock
// duration, or the retirement of a completed slide. IN PARTICULAR NOTHING HERE OBSERVES THAT A
// PARKED PICTURE EVER ARRIVES ANYWHERE: `PlayRouteSlide` leaves `Body` at `TweenOffsets[0]` and
// only `Tick` walks it to `TweenRestOffset`. These clauses say the polyline is the right shape,
// that it is armed on the right occasions, and that a park is created, survived and cleared on
// the right occasions. They say nothing whatever about the motion along it, and the seconds
// `PlayRouteSlide` RETURNS are asserted only to be positive -- never as a number, for
// `AiPlaybackStepSeconds`' own recorded reason that a pace has one home and this is not it.
//
// WHERE THE EXPECTATIONS COME FROM -- unchanged from the block above, one layer further in.
// Every expected point is `AStratBoardActor::WorldLocationOfHex` off the board the subsystem
// spawned. No axial -> world formula appears here, no hex spacing is written down, and
// `BodyZOffset` is never named as a number: clause 8 reads the lift BACK off the drawn actor.
// The route is `FRouteFixture`'s, discovered from the board by `StratRouteTween::Arrange`.
//
// THE ANCHOR IS THE PRODUCTION ANCHOR AND GETTING THAT WRONG WOULD HAVE MADE CLAUSE 7 INERT,
// WHICH IS WORTH STATING BECAUSE IT IS THE TRAP THIS WHOLE FILE KEEPS FALLING INTO.
// `UStratMatchSubsystem::PlayMoveSlideForStep` takes the anchor from `AppliedModel`'s entry for
// the unit -- and before a tour begins `RunAiTurnsNow` has ALREADY reconciled the board to the
// FINAL state, so the anchor is the unit's DESTINATION and the actor is already standing on it.
// Every clause below therefore applies `Moved` first and puts the actor at `Route.Last()`
// before arming anything. Anchoring on the actor's STARTING hex instead would have made
// waypoint 0 exactly zero on the shipped code, at which point clause 7's mutant produces the
// identical polyline and the clause pins nothing.
//
// FALSIFIABILITY -- UNDISCHARGED, AND SAID PLAINLY BECAUSE THE BLOCK ABOVE RECORDS WHAT
// HAPPENS WHEN IT IS NOT. Each clause below names the EXACT one-line mutation that must redden
// it, in the file and function it lives in. NONE WAS BUILT: every one of them is in
// `Source/StratPlay/StratUnitActor.cpp` or `Source/StratPlay/StratMatchSubsystem.cpp`, which
// this lane may not edit even temporarily. Two of the five clauses in the block above passed
// their own named mutation on the first pass, with every vacuity assertion present and green,
// so **A NAMED MUTATION IS A PROMISE AND NOT A MEASUREMENT** and nothing below may be cited as
// discharged until `strat-gameplay-engineer` has run it.
//
// THE UNIT ACTOR CLASS IS `AStratRouteTweenUnitDouble` FOR THE REASON THE BLOCK ABOVE GIVES,
// AND ON THIS PATH IT IS SHARPER STILL: `PlayRouteSlide`'s FIRST guard is
// `MoveTweenSeconds <= 0.0f`, at which it returns 0 HAVING WRITTEN NOTHING. Against the shipped
// C++ class every clause below would assert over an actor the function never touched, and would
// pass over a correct implementation, a wrong one and a deleted one alike.
// ===========================================================================

namespace StratRouteSlide
{
	using StratMatchReconcile::FTestWorldScope;
	using StratRouteTween::FRouteFixture;
	using StratRouteTween::kTolerance;

	/** The board's answer for each hex of a route, in order. No conversion is written here. */
	static TArray<FVector> WorldPointsOf(const AStratBoardActor& Board, const TArray<FIntPoint>& Hexes)
	{
		TArray<FVector> Out;
		Out.Reserve(Hexes.Num());
		for (const FIntPoint& Hex : Hexes)
		{
			Out.Add(Board.WorldLocationOfHex(Hex));
		}
		return Out;
	}

	/** The fixture's route with its last hex dropped -- a leg that stops SHORT of where the
	 *  actor stands, which is what every step but a unit's last one looks like in a real tour. */
	static TArray<FIntPoint> LegShortOfTheAnchor(const FRouteFixture& Fixture)
	{
		TArray<FIntPoint> Leg;
		for (int32 Index = 0; Index + 1 < Fixture.Route.Num(); ++Index)
		{
			Leg.Add(Fixture.Route[Index]);
		}
		return Leg;
	}

	/**
	 * The moving unit's actor, standing STILL on the hex its route ENDS at -- which is the state
	 * `UStratMatchSubsystem::PlayMoveSlideForStep` is always called in.
	 *
	 * THE APPLY IS THE ARRANGEMENT AND NOT AN ASSERTION. `StratRouteTween::Arrange` leaves the
	 * actors on `AtStart`; this moves the one unit to `Route.Last()` with NOTHING noted, so it
	 * snaps, arms no tween and leaves `Body` at exact relative zero. That is the board a tour
	 * looks at: `RunAiTurnsNow` reconciles to the final model BEFORE `BeginAiPlayback`.
	 *
	 * IT ASSERTS ITS OWN PRECONDITIONS THROUGH `Test`, INCLUDING THE TWO THAT ARE EASIEST TO
	 * LOSE SILENTLY -- that the spawned actor really is the double (against the shipped class
	 * `PlayRouteSlide` writes nothing at all), and that the picture starts at exact relative
	 * zero (clause 7's whole subject is what happens to waypoint 0 from THAT state).
	 */
	static AStratRouteTweenUnitDouble* ArrangeAtDestination(FAutomationTestBase& Test,
		FRouteFixture& Fixture)
	{
		Fixture.Subsystem->ApplyView(Fixture.Moved);

		AStratRouteTweenUnitDouble* const Double =
			Cast<AStratRouteTweenUnitDouble>(Fixture.Subsystem->FindUnitActor(Fixture.MovingUnitId));
		if (!Test.TestNotNull(
				TEXT("the moving unit's actor is the tween double the config asked for -- against "
				     "the shipped class PlayRouteSlide returns 0 having written nothing"),
				Double))
		{
			return nullptr;
		}
		if (!Test.TestTrue(TEXT("the double has a body, without which 'the picture is home' is "
				"indistinguishable from 'there is no picture'"),
				Double->HasBody()))
		{
			return nullptr;
		}
		if (!Test.TestEqual(TEXT("the actor is at rest before anything is armed"),
				Double->GetTweenWaypointCount(), 0))
		{
			return nullptr;
		}
		if (!Test.TestTrue(*FString::Printf(
					TEXT("and its picture sits at exact relative zero (it is at %s)"),
					*Double->BodyRelativeLocation().ToString()),
				Double->BodyRelativeLocation().IsZero()))
		{
			return nullptr;
		}

		// AND IT STANDS ON THE ROUTE'S LAST HEX -- the board's answer, as everywhere else here.
		const FVector Expected = Fixture.Board->WorldLocationOfHex(Fixture.Route.Last());
		const FVector Actual   = Double->GetActorLocation();
		if (!Test.TestTrue(*FString::Printf(
					TEXT("the unit stands over WorldLocationOfHex(%d, %d), which is the anchor a "
					     "tour would use -- it is at %s, the hex is at %s"),
					Fixture.Route.Last().X, Fixture.Route.Last().Y,
					*Actual.ToString(), *Expected.ToString()),
				FMath::IsNearlyEqual(Actual.X, Expected.X, kTolerance) &&
				FMath::IsNearlyEqual(Actual.Y, Expected.Y, kTolerance)))
		{
			return nullptr;
		}

		return Double;
	}

	/** Where a waypoint puts the picture in world space. `StratRouteTween::WaypointWorld`'s
	 *  expression, restated on this path for the same reason: the actor transform plus the
	 *  `Body`-relative offset is exactly what `Tick` lerps between. */
	static FVector WaypointWorld(const AStratUnitActor& Actor, const int32 Index)
	{
		return Actor.GetActorLocation() + Actor.GetTweenWaypoint(Index);
	}
}

// ---------------------------------------------------------------------------
// Clause 6. A slide parks over the route's own last hex, and that is NOT the actor's hex.
//
// WHAT MAKES THIS A CLAUSE AND NOT A RESTATEMENT OF CLAUSE 1. Clause 1 asserts the terminal
// waypoint is EXACTLY zero, with no tolerance, because `ApplyUnitView` APPENDS
// `FVector::ZeroVector` and the claim rests on construction. `PlayRouteSlide` COMPUTES its
// terminal, and it is zero only in the special case where the route ends on the hex the actor
// stands on -- a unit's final move of a hand-over. This clause takes the OTHER case, which is
// every intermediate move of every multi-move unit in a tour, and requires the terminal to be
// non-zero and to stand over the route's last hex.
//
// THE ROUTE IS A PREFIX OF THE FIXTURE'S AND THE ANCHOR IS THE PRODUCTION ANCHOR. The actor
// stands on `Route.Last()` (see `ArrangeAtDestination`) and the slide is armed over the leg
// that stops one hex short of it, so the park is over a hex the actor is not on. That is a real
// tour's shape: `PlayMoveSlideForStep` anchors on `AppliedModel`, which is already the FINAL
// state, so every step but a unit's last parks somewhere the actor is not.
//
// NON-ZERO IS ASSERTED WITH A MAGNITUDE AND NOT WITH `!IsZero()` ALONE, because `!IsZero()` is
// satisfied by a float wobble and would go green over an implementation that got the park
// nearly right by accident. The magnitude is compared against the TILE SPACING measured off the
// board, so no distance is written down here either.
//
// IT IS DELIBERATELY BLIND IN Z, which is what keeps it independent of clause 8. The XY
// comparison is unaffected by the anchor being derived from the actor transform -- the actor
// and the anchor agree in XY exactly -- so clause 8's mutation must redden clause 8 ALONE, the
// same division clauses 1 and 2 above are built on.
//
// WHAT WOULD REDDEN IT -- UNDISCHARGED, NO MUTANT BUILT, THIS LANE MAY NOT BUILD ONE. In
// `AStratUnitActor::PlayRouteSlide` (`Source/StratPlay/StratUnitActor.cpp`), change the offset
// loop's `TweenOffsets.Add(Point - AnchorWorldPoint);` to
// `TweenOffsets.Add(Point - RouteWorldPoints.Last());` -- which is `ApplyUnitView`'s
// construction, and is the single most plausible way for this function to be "tidied" into
// agreement with the one above it. The terminal waypoint becomes exactly zero, the park is
// destroyed, and every intermediate move of a tour would draw the unit as though it had already
// arrived. THIS CLAUSE IS THE ONLY THING IN THE TREE THAT WOULD NOTICE.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRouteSlideParksOverTheRoutesLastHexTest,
	"Stratocracy.StratPlay.T-INT-05.RouteSlideParksOverTheRoutesLastHex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRouteSlideParksOverTheRoutesLastHexTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRouteSlide;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	FRouteFixture Fixture;
	if (!StratRouteTween::Arrange(*this, Scope.World, Fixture))
	{
		return false;
	}

	AStratRouteTweenUnitDouble* const Actor = ArrangeAtDestination(*this, Fixture);
	if (Actor == nullptr)
	{
		return false;
	}

	// ---- A route that stops SHORT of where the actor stands -------------------
	const TArray<FIntPoint> Leg = LegShortOfTheAnchor(Fixture);
	if (!TestTrue(TEXT("the leg is a real route of at least two hexes"), Leg.Num() >= 2))
	{
		return false;
	}
	if (!TestTrue(TEXT("and it ends on a hex the actor is NOT standing on, without which the "
				"terminal waypoint would legitimately be zero and this clause would pin nothing"),
			Leg.Last() != Fixture.Route.Last()))
	{
		return false;
	}

	const FVector Anchor  = Fixture.Board->WorldLocationOfHex(Fixture.Route.Last());
	const float   Seconds = Actor->PlayRouteSlide(WorldPointsOf(*Fixture.Board, Leg), Anchor);

	// ---- The slide armed at all -----------------------------------------------
	if (!TestTrue(*FString::Printf(
				TEXT("the slide armed and reported a positive duration (%.3f s) -- a zero here "
				     "means PlayRouteSlide refused and nothing below would be measuring it"),
				Seconds),
			Seconds > 0.0f))
	{
		return false;
	}
	if (!TestEqual(TEXT("the armed polyline has one waypoint per hex of the route it was handed"),
			Actor->GetTweenWaypointCount(), Leg.Num()))
	{
		return false;
	}

	// ---- THE CLAUSE: the park is real, and it is over the route's last hex ------
	const FVector Terminal = Actor->GetTweenWaypoint(Leg.Num() - 1);
	const FVector Expected = Fixture.Board->WorldLocationOfHex(Leg.Last()) - Anchor;

	TestTrue(*FString::Printf(
			TEXT("the terminal waypoint is NOT zero -- it is %s, a park of %.1f uu on a %.1f uu tile"),
			*Terminal.ToString(), Terminal.Size2D(), Fixture.Spacing),
		!Terminal.IsZero() && Terminal.Size2D() > Fixture.Spacing * 0.5);

	TestTrue(*FString::Printf(
			TEXT("and it is WorldLocationOfHex(%d, %d) minus the anchor hex (%d, %d): it is %s, "
			     "that difference is %s"),
			Leg.Last().X, Leg.Last().Y, Fixture.Route.Last().X, Fixture.Route.Last().Y,
			*Terminal.ToString(), *Expected.ToString()),
		FMath::IsNearlyEqual(Terminal.X, Expected.X, kTolerance) &&
		FMath::IsNearlyEqual(Terminal.Y, Expected.Y, kTolerance));

	// AND THE PICTURE THE PARK DESCRIBES STANDS OVER THAT HEX, which is the re-scoped invariant
	// in `AStratUnitActor::TweenOffsets`' own words rather than in this file's arithmetic.
	{
		const FVector Drawn     = WaypointWorld(*Actor, Leg.Num() - 1);
		const FVector HexAtRest = Fixture.Board->WorldLocationOfHex(Leg.Last());
		TestTrue(*FString::Printf(
				TEXT("Body at the terminal waypoint draws the unit over hex (%d, %d) -- it draws "
				     "at %s, the hex is at %s"),
				Leg.Last().X, Leg.Last().Y, *Drawn.ToString(), *HexAtRest.ToString()),
			FMath::IsNearlyEqual(Drawn.X, HexAtRest.X, kTolerance) &&
			FMath::IsNearlyEqual(Drawn.Y, HexAtRest.Y, kTolerance));
	}

	AddInfo(FString::Printf(
		TEXT("unit %d slid over %d hexes anchored on (%d,%d) and parked %.1f uu away, on a %.1f uu tile"),
		Fixture.MovingUnitId, Leg.Num(), Fixture.Route.Last().X, Fixture.Route.Last().Y,
		Terminal.Size2D(), Fixture.Spacing));

	return true;
}

// ---------------------------------------------------------------------------
// Clause 7. A unit's FIRST slide of a tour arms a polyline that is not degenerate.
//
// THIS IS THE CLAUSE FOR A DEFECT THAT WAS CAUGHT BY READING AND NEVER SHIPPED, AND IT IS THE
// ONLY ONE HERE WHOSE SUBJECT IS AN `if` THAT MIGHT LOOK LIKE AN OVER-CAUTION. `engine.md`
// records that the brief for this feature required `TweenOffsets[0]` to be overwritten with the
// drawn position UNCONDITIONALLY, on `ApplyUnitView`'s real and recorded anti-pop reason: an
// order arriving mid-slide must restart from where the unit is DRAWN, not from a hex it has
// visibly left.
//
// THE ARGUMENT THAT MAKES IT WRONG, AND IT IS SHORT. Before a tour the board already holds the
// FINAL state, so every picture is at EXACT relative zero. `Body->GetComponentLocation() -
// GetActorLocation()` is therefore ZERO on a unit's first slide, while the route's own first
// point is a hex away. An unconditional overwrite sets waypoint 0 to zero; for a ONE-HEX MOVE,
// which is two waypoints and the commonest reel entry there is, the polyline becomes {0, 0},
// `TweenArcLengths.Last()` is 0, `FinishTween` runs, and the feature ANIMATES NOTHING -- with a
// green build, a green suite and no log line. The conditional is the fix, and this clause is
// what stops it being conditioned away again.
//
// THE ONE-HEX CASE IS CHOSEN DELIBERATELY AND IT IS NOT THE GENERAL CASE. On a longer route the
// unconditional overwrite leaves waypoints 1..N-1 intact, so the arc length stays positive, a
// slide still arms and only the first leg is wrong -- visible to a human as a unit that starts
// from the wrong hex, invisible to any count. TWO WAYPOINTS IS WHERE IT COLLAPSES COMPLETELY,
// so the fixture takes the last leg of the fixture's route and nothing else.
//
// THE PRECONDITION IS THE HALF THAT MAKES IT WORK. `ArrangeAtDestination` asserts the picture
// is at EXACT relative zero before anything is armed -- `IsZero()`, no tolerance. Without that
// this clause is measuring an actor in an unknown state, and the mutation it names would be
// reddened or not depending on where the picture happened to be sitting.
//
// THE TERMINAL WAYPOINT IS NOT ASSERTED HERE, AND THE OMISSION IS DELIBERATE RATHER THAN AN
// OVERSIGHT. On this fixture the leg ends on the actor's own hex, so the terminal is zero by
// arithmetic -- and asserting that would make clause 8's mutation redden this clause too,
// destroying the independence clauses 6, 7 and 8 are built to have. Clause 6 owns the terminal.
//
// WHAT WOULD REDDEN IT -- UNDISCHARGED, NO MUTANT BUILT, THIS LANE MAY NOT BUILD ONE. In
// `AStratUnitActor::PlayRouteSlide` (`Source/StratPlay/StratUnitActor.cpp`), delete the
// `if (bSlideInFlight)` guard so the `TweenOffsets[0] = Body->GetComponentLocation() -
// GetActorLocation();` line runs on every call -- which is the shape the brief specified. Both
// waypoints go to zero, the arc-length guard retires the tween, `PlayRouteSlide` returns 0, and
// this clause reddens on the duration and on the waypoint count. NO CLAUSE ABOVE CAN SEE IT:
// they are all about `ApplyUnitView`, which does not contain that line's condition.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRouteSlideFirstSlideIsNonDegenerateTest,
	"Stratocracy.StratPlay.T-INT-05.AFirstRouteSlideArmsANonDegeneratePolyline",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRouteSlideFirstSlideIsNonDegenerateTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRouteSlide;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	FRouteFixture Fixture;
	if (!StratRouteTween::Arrange(*this, Scope.World, Fixture))
	{
		return false;
	}

	AStratRouteTweenUnitDouble* const Actor = ArrangeAtDestination(*this, Fixture);
	if (Actor == nullptr)
	{
		return false;
	}

	// ---- A ONE-HEX MOVE: two waypoints, which is where the defect collapses ----
	TArray<FIntPoint> OneStep;
	OneStep.Add(Fixture.Route[Fixture.Route.Num() - 2]);
	OneStep.Add(Fixture.Route.Last());
	if (!TestTrue(TEXT("the one-hex move really is two distinct hexes"),
			OneStep[0] != OneStep[1]))
	{
		return false;
	}

	const FVector Anchor  = Fixture.Board->WorldLocationOfHex(Fixture.Route.Last());
	const float   Seconds = Actor->PlayRouteSlide(WorldPointsOf(*Fixture.Board, OneStep), Anchor);

	// ---- THE CLAUSE, AND IT IS THREE INDEPENDENT WAYS TO SEE THE SAME COLLAPSE --
	// A degenerate polyline does not merely animate wrongly: `PlayRouteSlide`'s arc-length
	// guard retires it outright, so the duration, the waypoint count and the polyline's own
	// length all report it. Each is asserted, because they fail at different points on the way
	// to the same defect and a future change might reach only one of them.
	TestTrue(*FString::Printf(
			TEXT("a one-hex move from a picture at rest arms a slide with a positive duration "
			     "(%.3f s) -- an unconditionally overwritten waypoint 0 makes this 0"),
			Seconds),
		Seconds > 0.0f);

	if (!TestEqual(TEXT("and the polyline has both of its waypoints -- a collapsed one is "
				"retired, leaving none"),
			Actor->GetTweenWaypointCount(), 2))
	{
		return false;
	}

	const FVector First  = Actor->GetTweenWaypoint(0);
	const FVector Second = Actor->GetTweenWaypoint(1);

	TestTrue(*FString::Printf(
			TEXT("waypoint 0 is the hex the move STARTS on and not the picture's own resting "
			     "place: it is %s, %.1f uu out on a %.1f uu tile"),
			*First.ToString(), First.Size2D(), Fixture.Spacing),
		!First.IsZero() && First.Size2D() > Fixture.Spacing * 0.5);

	{
		const FVector Expected = Fixture.Board->WorldLocationOfHex(OneStep[0]) - Anchor;
		TestTrue(*FString::Printf(
				TEXT("and it is WorldLocationOfHex(%d, %d) minus the anchor: it is %s, that "
				     "difference is %s"),
				OneStep[0].X, OneStep[0].Y, *First.ToString(), *Expected.ToString()),
			FMath::IsNearlyEqual(First.X, Expected.X, kTolerance) &&
			FMath::IsNearlyEqual(First.Y, Expected.Y, kTolerance));
	}

	TestTrue(*FString::Printf(
			TEXT("and the polyline has real length -- %.1f uu between its two waypoints on a "
			     "%.1f uu tile, where a collapsed {0, 0} is 0"),
			FVector::Dist2D(First, Second), Fixture.Spacing),
		FVector::Dist2D(First, Second) > Fixture.Spacing * 0.5);

	return true;
}

// ---------------------------------------------------------------------------
// Clause 8. The slide's waypoints sit at the height the unit is DRAWN at.
//
// THE SAME DEFECT AS CLAUSE 2 ON A DIFFERENT FUNCTION, AND THE DECLARATION FORBIDS IT BY NAME.
// `AStratUnitActor::PlayRouteSlide` takes its anchor as a parameter and its declaration records
// that the obvious derivation -- `GetActorLocation() - FVector(0, 0, BodyZOffset)` -- is
// FORBIDDEN, because it reintroduces the `+ BodyZOffset` expression whose absence is why every
// offset here is a difference of two TILE-PLANE points and the lift cancels exactly. This is
// the clause that collects on that prohibition. `UStratMatchSubsystem::PlayMoveSlideForStep`
// supplies the anchor through `AStratBoardActor::WorldLocationOfHex`, the same expression the
// route points go through, so on the shipped code the two cannot disagree by construction.
//
// IT IS VACUOUS AT ZERO LIFT AND THAT IS ASSERTED FIRST. The shipped C++ `BodyZOffset` is
// 0.0f, at which the tile plane and the drawn unit are the same height and every assertion
// below is a tautology; `AStratRouteTweenUnitDouble` carries 37.0f for exactly this reason. The
// lift is READ BACK off the drawn actor -- `GetActorLocation().Z` minus the board's Z for the
// hex it stands on, which is `ApplyUnitView`'s own location expression inverted -- and only
// asserted NON-ZERO, so no number is named and the measurement survives the fixture being
// retuned.
//
// IT IS BLIND IN XY, WHICH IS WHAT MAKES IT INDEPENDENT OF CLAUSES 6 AND 7. The actor and the
// anchor agree in XY exactly, so an anchor derived from the actor transform is invisible to
// every XY assertion in this file and visible only here -- the same division that let clause 2
// redden ALONE while clause 1 stayed green under one mutation.
//
// WHAT WOULD REDDEN IT -- UNDISCHARGED, NO MUTANT BUILT, THIS LANE MAY NOT BUILD ONE. In
// `AStratUnitActor::PlayRouteSlide` (`Source/StratPlay/StratUnitActor.cpp`), change the offset
// loop's `TweenOffsets.Add(Point - AnchorWorldPoint);` to
// `TweenOffsets.Add(Point - GetActorLocation());` -- the forbidden derivation, one token, and
// the anchor parameter then goes unused. Every waypoint sinks by `BodyZOffset` and the picture
// dips into the board and climbs back out. Clauses 6 and 7 must stay GREEN under it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRouteSlideWaypointsSitAtTheDrawnUnitsHeightTest,
	"Stratocracy.StratPlay.T-INT-05.RouteSlideWaypointsSitAtTheDrawnUnitsHeight",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRouteSlideWaypointsSitAtTheDrawnUnitsHeightTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRouteSlide;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	FRouteFixture Fixture;
	if (!StratRouteTween::Arrange(*this, Scope.World, Fixture))
	{
		return false;
	}

	AStratRouteTweenUnitDouble* const Actor = ArrangeAtDestination(*this, Fixture);
	if (Actor == nullptr)
	{
		return false;
	}

	const FVector Anchor  = Fixture.Board->WorldLocationOfHex(Fixture.Route.Last());
	const float   Seconds = Actor->PlayRouteSlide(
		WorldPointsOf(*Fixture.Board, Fixture.Route), Anchor);

	if (!TestTrue(*FString::Printf(
				TEXT("a slide armed to measure the height of (%.3f s)"), Seconds),
			Seconds > 0.0f))
	{
		return false;
	}
	if (!TestEqual(TEXT("with one waypoint per hex of the route"),
			Actor->GetTweenWaypointCount(), Fixture.Route.Num()))
	{
		return false;
	}

	// ---- The lift, read back off the drawn actor. No number is named here -----
	const double Lift = Actor->GetActorLocation().Z - Anchor.Z;
	if (!TestTrue(*FString::Printf(
				TEXT("the fixture draws the unit above the tile plane, so this clause can fail at "
				     "all (lift %.1f uu)"), Lift),
			FMath::Abs(Lift) > 1.0))
	{
		return false;
	}
	AddInfo(FString::Printf(TEXT("the drawn unit sits %.1f uu above the tile plane"), Lift));

	// ---- THE CLAUSE. Every waypoint draws the unit at its own height ----------
	for (int32 Index = 0; Index < Fixture.Route.Num(); ++Index)
	{
		const FVector Plane = Fixture.Board->WorldLocationOfHex(Fixture.Route[Index]);
		const FVector Drawn = WaypointWorld(*Actor, Index);

		TestTrue(*FString::Printf(
				TEXT("waypoint %d draws at the unit's own height over hex (%d, %d): Z %.2f, "
				     "expected %.2f (plane %.2f + lift %.2f)"),
				Index, Fixture.Route[Index].X, Fixture.Route[Index].Y,
				Drawn.Z, Plane.Z + Lift, Plane.Z, Lift),
			FMath::IsNearlyEqual(Drawn.Z, Plane.Z + Lift, kTolerance));
	}

	return true;
}

// ---------------------------------------------------------------------------
// Clause 9. A parked picture survives an apply that moves nothing -- THE HOVER HAZARD.
//
// THIS IS THE HALF A NAIVE CLAUSE OMITS, AND IT IS THE ONE THAT CATCHES THE GLITCH.
// `AStratUnitActor::ApplyUnitView` gained a branch on 2026-09-02: an actor that genuinely moved
// with nothing armed has its park hard-cut. Clause 10 below pins that. THIS clause pins the
// GUARD on it, and the guard is not defensive programming -- it is the whole difference between
// a feature and a bug report. `AStratPlayerController::Tick` calls `UpdateHoverFromCursor` every
// frame and a hover crossing into a new hex reaches `UStratMatchSubsystem::ApplyView`, so
// `ApplyUnitView` FIRES DURING A TOUR whenever the player moves the mouse. An unconditional
// clear would pop every parked unit to its final hex on the next mouse movement, and it would
// read as a random glitch rather than as a rule.
//
// THE FIXTURE IS THE HOVER, STRUCTURALLY. A hover applies the SAME model with the SAME
// positions, and that is exactly what this clause does: the identical `Moved` view, applied
// twice, with a park created in between. `bActorMoved` is therefore false and the branch must
// not be entered.
//
// THE COMPARISON IS ELEMENT BY ELEMENT, AND SO IS THE PICTURE'S POSITION. Two empty arrays
// compare equal element-wise, so the count is asserted against the leg's length FIRST and the
// picture's offset is required to be NON-ZERO before and after -- the state the mutation
// destroys is "there is a park", and an equality between two absences would report it as
// intact. That is the exact shape that made two clauses in the block above inert.
//
// WHY IT IS NOT `SecondApplyWithIdenticalPositionsLeavesTheTweenAlone` AGAIN. That clause is
// about the ARMING branch's `&& bActorMoved` and a tween created by `ApplyUnitView` itself;
// this one is about the PARK-CLEAR branch's `bActorMoved &&` and a park created by
// `PlayRouteSlide`. They are two different conditions in two different `if`s, and the mutation
// named below reddens this one and not that one.
//
// WHAT WOULD REDDEN IT -- UNDISCHARGED, NO MUTANT BUILT, THIS LANE MAY NOT BUILD ONE. In
// `AStratUnitActor::ApplyUnitView` (`Source/StratPlay/StratUnitActor.cpp`), change the
// park-clear branch's condition `else if (bActorMoved && (TweenOffsets.Num() > 0 ||
// !TweenRestOffset.IsZero()))` to `else if (TweenOffsets.Num() > 0 ||
// !TweenRestOffset.IsZero())` -- dropping `bActorMoved &&`, which is one term and reads like a
// simplification. `CancelRouteSlide` then runs on this clause's second apply, the waypoint count
// falls to 0 and the picture snaps home. Clause 10 must stay GREEN under it, since its own apply
// really does move the actor.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRouteSlideParkSurvivesAnIdenticalApplyTest,
	"Stratocracy.StratPlay.T-INT-05.AParkedPictureSurvivesAnApplyWithIdenticalPositions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRouteSlideParkSurvivesAnIdenticalApplyTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRouteSlide;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	FRouteFixture Fixture;
	if (!StratRouteTween::Arrange(*this, Scope.World, Fixture))
	{
		return false;
	}

	AStratRouteTweenUnitDouble* const Actor = ArrangeAtDestination(*this, Fixture);
	if (Actor == nullptr)
	{
		return false;
	}

	// ---- A REAL PARK: a leg that stops short of where the actor stands --------
	const TArray<FIntPoint> Leg = LegShortOfTheAnchor(Fixture);
	const FVector Anchor  = Fixture.Board->WorldLocationOfHex(Fixture.Route.Last());
	const float   Seconds = Actor->PlayRouteSlide(WorldPointsOf(*Fixture.Board, Leg), Anchor);

	if (!TestTrue(*FString::Printf(TEXT("a slide armed to be left alone (%.3f s)"), Seconds),
			Seconds > 0.0f))
	{
		return false;
	}
	if (!TestEqual(TEXT("the park exists before the apply -- one waypoint per hex of the leg"),
			Actor->GetTweenWaypointCount(), Leg.Num()))
	{
		return false;
	}

	// THE PICTURE IS DISPLACED, AND THIS IS THE ASSERTION WITHOUT WHICH THE WHOLE CLAUSE IS AN
	// EQUALITY BETWEEN TWO ABSENCES. `PlayRouteSlide` puts `Body` at `TweenOffsets[0]`, hexes
	// away from where the actor stands; a mutation that cancelled the slide would zero it.
	const FVector PictureBefore = Actor->BodyRelativeLocation();
	if (!TestTrue(*FString::Printf(
				TEXT("and the picture is genuinely displaced by it -- %s, %.1f uu out on a %.1f uu tile"),
				*PictureBefore.ToString(), PictureBefore.Size2D(), Fixture.Spacing),
			!PictureBefore.IsZero() && PictureBefore.Size2D() > Fixture.Spacing * 0.5))
	{
		return false;
	}

	TArray<FVector> WaypointsBefore;
	for (int32 Index = 0; Index < Actor->GetTweenWaypointCount(); ++Index)
	{
		WaypointsBefore.Add(Actor->GetTweenWaypoint(Index));
	}
	const FVector ActorLocationBefore = Actor->GetActorLocation();

	// ---- THE HOVER. The same model, the same positions, nothing noted ---------
	Fixture.Subsystem->ApplyView(Fixture.Moved);

	if (!TestTrue(TEXT("the apply moved the actor nowhere, which is what makes it a hover and "
				"not a move"),
			Actor->GetActorLocation().Equals(ActorLocationBefore)))
	{
		return false;
	}

	// ---- THE CLAUSE ----------------------------------------------------------
	if (!TestEqual(TEXT("the park survives an apply with identical positions -- the waypoint "
				"count is untouched"),
			Actor->GetTweenWaypointCount(), WaypointsBefore.Num()))
	{
		return false;
	}
	for (int32 Index = 0; Index < WaypointsBefore.Num(); ++Index)
	{
		TestTrue(*FString::Printf(
				TEXT("waypoint %d is untouched: it was %s and is %s"),
				Index, *WaypointsBefore[Index].ToString(),
				*Actor->GetTweenWaypoint(Index).ToString()),
			Actor->GetTweenWaypoint(Index).Equals(WaypointsBefore[Index], kTolerance));
	}
	TestTrue(*FString::Printf(
			TEXT("and the picture is still parked where the slide put it: it was %s and is %s"),
			*PictureBefore.ToString(), *Actor->BodyRelativeLocation().ToString()),
		Actor->BodyRelativeLocation().Equals(PictureBefore, kTolerance));
	TestTrue(TEXT("which is still a real displacement and not a picture that came home"),
		!Actor->BodyRelativeLocation().IsZero());

	return true;
}

// ---------------------------------------------------------------------------
// Clause 10. An actor that GENUINELY moved with nothing armed has its park hard-cut.
//
// THE OTHER HALF OF CLAUSE 9's `if`, AND EACH IS THE OTHER'S CONTROL. A park describes where a
// picture rests relative to a transform; the moment the model puts that unit somewhere else and
// nothing new is armed, the park stops describing anything and the picture must come home in
// the same call. `AStratUnitActor::ApplyUnitView`'s branch says so and the engineer's entry
// names it as a BEHAVIOUR CHANGE that owes a clause rather than a tidy-up. This is that clause.
//
// IT IS A HARD CUT AND THAT IS WHAT IS ASSERTED. `CancelRouteSlide` zeroes `TweenRestOffset`
// FIRST and then retires, so `Body` is at exact relative zero when the call returns -- not
// walking there over the following seconds. The alternative shape, zeroing the rest offset and
// letting a running slide finish, would walk the polyline all the way to the park and only THEN
// snap; that is the shape that pops, and asserting `IsZero()` with no tolerance immediately
// after the apply is what distinguishes them. `Tick` never runs here, so a picture that was
// merely SENT home would still be parked when this clause reads it.
//
// BOTH PRECONDITIONS ARE ASSERTED BECAUSE EITHER ONE FAILING MAKES THIS CLAUSE VACUOUS. That a
// park existed at all (else "the picture is at zero" is trivially true), and that the actor
// REALLY MOVED (else clause 9's branch is the one under test and this clause would go green
// while the hard cut was deleted).
//
// WHAT WOULD REDDEN IT -- UNDISCHARGED, NO MUTANT BUILT, THIS LANE MAY NOT BUILD ONE. In
// `AStratUnitActor::ApplyUnitView` (`Source/StratPlay/StratUnitActor.cpp`), delete the
// `CancelRouteSlide();` call that is the whole body of the park-clear `else if` -- one line, and
// the branch then does nothing at all. The waypoint count stays at the leg's length and the
// picture stays parked hexes from where the model now says the unit is. Clause 9 must stay GREEN
// under it, since its subject is the branch NOT being entered.
//
// A SECOND MUTATION THIS CLAUSE ALSO CATCHES, NAMED BECAUSE IT IS THE ONE A READER WOULD REACH
// FOR: replacing that call with `TweenRestOffset = FVector::ZeroVector;` -- the gentler-looking
// option the branch's own comment records as WRONG. The waypoint count survives, so the count
// assertion reddens before the position one does.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRouteSlideMovedActorClearsItsParkTest,
	"Stratocracy.StratPlay.T-INT-05.AnActorThatMovedWithNoRouteClearsItsPark",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRouteSlideMovedActorClearsItsParkTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRouteSlide;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	FRouteFixture Fixture;
	if (!StratRouteTween::Arrange(*this, Scope.World, Fixture))
	{
		return false;
	}

	AStratRouteTweenUnitDouble* const Actor = ArrangeAtDestination(*this, Fixture);
	if (Actor == nullptr)
	{
		return false;
	}

	// ---- A REAL PARK, exactly as clause 9 builds one --------------------------
	const TArray<FIntPoint> Leg = LegShortOfTheAnchor(Fixture);
	const FVector Anchor  = Fixture.Board->WorldLocationOfHex(Fixture.Route.Last());
	const float   Seconds = Actor->PlayRouteSlide(WorldPointsOf(*Fixture.Board, Leg), Anchor);

	if (!TestTrue(*FString::Printf(TEXT("a slide armed to be cut (%.3f s)"), Seconds),
			Seconds > 0.0f))
	{
		return false;
	}
	if (!TestEqual(TEXT("the park exists before the apply"),
			Actor->GetTweenWaypointCount(), Leg.Num()))
	{
		return false;
	}
	if (!TestTrue(*FString::Printf(
				TEXT("and the picture is genuinely displaced by it (%s, %.1f uu out) -- without "
				     "which 'the picture is home' below is trivially true"),
				*Actor->BodyRelativeLocation().ToString(),
				Actor->BodyRelativeLocation().Size2D()),
			!Actor->BodyRelativeLocation().IsZero()))
	{
		return false;
	}

	const FVector ActorLocationBefore = Actor->GetActorLocation();

	// ---- A MODEL THAT REALLY MOVES IT, WITH NOTHING NOTED ---------------------
	// `AtStart` puts this unit back on `Route[0]`; no route is noted, so nothing arms and the
	// park-clear branch is the only thing that can act.
	Fixture.Subsystem->ApplyView(Fixture.AtStart);

	if (!TestTrue(TEXT("the actor genuinely moved -- without this the branch under test is not "
				"even reached and this clause would pin nothing"),
			!Actor->GetActorLocation().Equals(ActorLocationBefore)))
	{
		return false;
	}
	{
		const FVector Expected = Fixture.Board->WorldLocationOfHex(Fixture.Route[0]);
		const FVector Actual   = Actor->GetActorLocation();
		TestTrue(*FString::Printf(
				TEXT("and it stands over WorldLocationOfHex(%d, %d) -- it is at %s, the hex is at %s"),
				Fixture.Route[0].X, Fixture.Route[0].Y, *Actual.ToString(), *Expected.ToString()),
			FMath::IsNearlyEqual(Actual.X, Expected.X, kTolerance) &&
			FMath::IsNearlyEqual(Actual.Y, Expected.Y, kTolerance));
	}

	// ---- THE CLAUSE ----------------------------------------------------------
	TestEqual(TEXT("the park is retired outright -- no polyline is left behind with no clock"),
		Actor->GetTweenWaypointCount(), 0);
	TestTrue(*FString::Printf(
			TEXT("and the picture is home in the same call, at EXACT relative zero and not on "
			     "its way there -- it is at %s"),
			*Actor->BodyRelativeLocation().ToString()),
		Actor->BodyRelativeLocation().IsZero());

	return true;
}
