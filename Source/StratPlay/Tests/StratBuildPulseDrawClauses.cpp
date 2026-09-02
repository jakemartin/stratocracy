// The draw hook for §2.11.5's BUILD pulse -- `AStratBoardActor::ShowBuildPulses` /
// `ClearBuildPulses` / `GetBuildPulseOverlayCount`, driven from
// `UStratMatchSubsystem::ApplyView` off `FStratFactoryView::bBuildPulse`.
//
// THE ID IS `T-UI-02` AND IT WAS TAKEN FROM THE CORPUS RATHER THAN CHOSEN. §4.11 states
// T-UI-02 as "the reachable-hex highlight displays exactly the T-MOVE-01 set -- the UI queries
// the module and never recomputes movement", and this repository has already stretched it to
// every board overlay drawn from a module-composed set: `ObjectiveRingLightsExactlyTheGuidanceHex`,
// `ReachOverlayIsNotComputedHere` and the path-preview decorator clauses all ride it. THE
// SECOND HALF OF THAT SENTENCE IS WHAT THIS FILE IS ABOUT -- the pulse is drawn where the
// module said and the drawing layer forms no second opinion -- so the fit is the stretched
// reading's core rather than its edge.
//
// AND IT IS THE NAME THE CODE ALREADY ASKED FOR. `AStratBoardActor::GetBuildPulseOverlayCount`
// says in terms that it exists for a clause and names it:
// `Stratocracy.StratPlay.T-UI-02.BuildPulseOverlayIsDrawnFromTheModelAlone`. That accessor has
// no other caller in the tree, exactly as `GetTargetOverlayCount` and
// `GetObjectiveOverlayCount` have none outside `Tests/`, so this file discharges a declared
// debt rather than inventing a use for a method.
//
// `GATE-BOARDCHURN` REFUSED T-UI-02 FOR ITS OWN CLAUSES AND THAT REFUSAL DOES NOT REACH HERE.
// Its stated reason is that T-UI-02 is a claim about WHAT IS DRAWN while its subject is
// whether the RENDERER WAS TOUCHED with the drawing identical. Every clause below has what is
// drawn as its subject.
//
// THREE FACTS ABOUT THIS FEATURE THAT A CLAUSE WRITTEN WITHOUT THEM WOULD GET WRONG.
//
//   1. A DARK BOARD IS CORRECT ON HALF OF ALL TURNS. `FStratFactoryBuildPulse::bShouldPulse`
//      inherits `side == activeSide` from `strat::canBuildAt`, so on the opponent's hot-seat
//      turn EVERY factory's pulse is false board-wide. A clause that lit pulses on the wrong
//      turn would read as a code defect and would not be one. `TheBuildPulseIsDarkOnThe
//      OpponentsTurn` below asserts the darkness as a REQUIREMENT, driven through the rules
//      module rather than through a planted flag.
//
//   2. `BuildPulseMaterial` SHIPS UNSET, so a lit factory currently draws in `OverlayMesh`'s
//      own material. NOTHING BELOW ASSERTS A MATERIAL OR AN APPEARANCE. Every clause reads
//      counts and instance positions, both of which are unaffected by which material a slot
//      holds, and the fixture assigns `OverlayMesh` only because `FillOverlay` adds no
//      instances at all without a static mesh -- the "mute reads as passing" hazard
//      `StratGuidedOpeningVisuals.cpp` records.
//
//   3. THE OVERLAY COMPONENT IS NOT REACHABLE BY A SCAN. `AStratBoardActor.h` states that with
//      more than one overlay a `GetComponents` search for "the one that is not the reach
//      overlay" returns whichever the iteration order reached first. `BuildPulseOverlay` is
//      `protected`, so reflection by name is the only door and is the same door the guidance
//      clauses use.
//
// WHERE EVERY EXPECTATION COMES FROM.
//   - THE LIT SET IS THE MODEL'S OWN. Every clause counts `FStratFactoryView::bBuildPulse` on
//     the model it just applied and requires the overlay to match that count. Not one factory
//     hex is written down in this file, and the model comes from
//     `UStratMatchSubsystem::BuildViewModel` over a bridge seeded from the shipped tables and
//     the shipped scenario.
//   - EVERY DRAWN POSITION IS `AStratBoardActor::WorldLocationOfHex`, asked of the board for
//     the hex the MODEL named. The axial->world formula is read, never repeated: a clause that
//     re-derived it would assert its own arithmetic.
//   - THE DARK CASE IS PRODUCED BY THE RULES MODULE, not by editing a flag: an end of turn is
//     submitted through the bridge and the model is rebuilt, so `bBuildPulse` goes false
//     because §2.7 says the viewing side may not build, in the same way it would in a hot-seat
//     game.
//
// NOTHING BELOW CALLS A `strat::` FREE FUNCTION. `LNK2019` outside StratBridge and
// StratRules, measured 8x; every rules-side value here arrives through `FStratBridge` or
// through the view model.

#include "Misc/AutomationTest.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"
#include "Misc/Paths.h"
#include "UObject/UnrealType.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

// A .cpp may say this; a UHT-parsed header may not. The same constraint
// `StratMatchSubsystem.cpp` records about itself.
#include "StratBridge.h"

namespace StratBuildPulseDraw
{
	/** The harness pin for which side moves first -- `Replay.h` states no rule in this project
	 *  decides it, and the neighbouring board files pin the same value for the same reason. */
	static const int32 kFirstSide = 0;

	/**
	 * A slot name no match has ever written.
	 *
	 * HYGIENE ON `StratGuidedOpeningVisuals.cpp`'S PRECEDENT AND FOR ITS REASON.
	 * `FStratMatchConfig::SaveSlotName` defaults to `StratocracyMatch`, THE PLAYER'S SLOT, so
	 * a suite run against the default would read -- and on some paths write -- a developer's
	 * in-progress game. Nothing below asserts anything about a slot.
	 */
	static const TCHAR* const kAbsentSlotName = TEXT("StratBuildPulseDrawNoSuchSlot");

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

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
		Out.SaveSlotName    = kAbsentSlotName;
		Out.FirstSide       = kFirstSide;
		Out.ViewingSide     = 0;
		Out.BoardActorClass = AStratBoardActor::StaticClass();
		Out.UnitActorClass  = AStratUnitActor::StaticClass();
		return true;
	}

	/** The transient world, with the engine context teardown wants. The world goes first and
	 *  the context second -- the order is measured, not chosen; the other order leaves
	 *  "World has no context!" once per destroyed actor. */
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

	/**
	 * Gives the subsystem-spawned board an overlay mesh.
	 *
	 * WITHOUT THIS EVERY OVERLAY ACCESSOR IS MUTE, AND MUTE READS AS PASSING --
	 * `AStratBoardActor::FillOverlay` adds no instances when the component has no static mesh,
	 * and it is silent about it by design. `/Engine/` and not `/Game/`, so this does not touch
	 * the project content the no-path-literal rule is about; automation fixtures are one of
	 * that rule's two named exceptions in any case. Set on the INSTANCE and never on the CDO,
	 * so the mesh cannot leak into another clause's board.
	 */
	static bool GiveTheBoardAnOverlayMesh(AStratBoardActor* Board, FString& OutError)
	{
		if (Board == nullptr)
		{
			OutError = TEXT("the match spawned no board to give an overlay mesh to");
			return false;
		}

		UStaticMesh* const Mesh =
			LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
		if (Mesh == nullptr)
		{
			OutError = TEXT("/Engine/BasicShapes/Cube.Cube did not load");
			return false;
		}

		FObjectPropertyBase* const Property =
			FindFProperty<FObjectPropertyBase>(Board->GetClass(), FName(TEXT("OverlayMesh")));
		if (Property == nullptr)
		{
			OutError = TEXT("no reflected object property 'OverlayMesh' on AStratBoardActor");
			return false;
		}
		Property->SetObjectPropertyValue(Property->ContainerPtrToValuePtr<void>(Board), Mesh);

		if (!Board->HasActorBegunPlay())
		{
			Board->DispatchBeginPlay();
		}
		return true;
	}

	/** A named overlay component, read through reflection. BY NAME AND NEVER BY A SCAN -- see
	 *  the file header's third fact. */
	static UHierarchicalInstancedStaticMeshComponent* OverlayNamed(
		AStratBoardActor* Board, const TCHAR* PropertyName, FString& OutError)
	{
		if (Board == nullptr)
		{
			OutError = TEXT("no board to read an overlay from");
			return nullptr;
		}

		FObjectPropertyBase* const Property =
			FindFProperty<FObjectPropertyBase>(Board->GetClass(), FName(PropertyName));
		if (Property == nullptr)
		{
			OutError = FString::Printf(
				TEXT("no reflected object property '%s' on AStratBoardActor"), PropertyName);
			return nullptr;
		}

		UObject* const Value =
			Property->GetObjectPropertyValue(Property->ContainerPtrToValuePtr<void>(Board));
		UHierarchicalInstancedStaticMeshComponent* const Component =
			Cast<UHierarchicalInstancedStaticMeshComponent>(Value);
		if (Component == nullptr)
		{
			OutError = FString::Printf(
				TEXT("'%s' is not a hierarchical instanced mesh component"), PropertyName);
			return nullptr;
		}
		return Component;
	}

	/** A live match, its subsystem, its bridge and its board -- with an overlay mesh already
	 *  assigned, because every clause here needs one. */
	struct FLiveMatch
	{
		FTestWorldScope       Scope;
		UStratMatchSubsystem* Subsystem = nullptr;
		FStratBridge*         Bridge    = nullptr;
		AStratBoardActor*     Board     = nullptr;

		bool Start(FString& OutError)
		{
			if (Scope.World == nullptr)
			{
				OutError = TEXT("no transient world was created");
				return false;
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

			FString StartReason;
			Subsystem->StartMatch(Config, StartReason);
			if (!Subsystem->IsMatchLive())
			{
				OutError = StartReason;
				return false;
			}

			Bridge = Subsystem->GetBridge();
			if (Bridge == nullptr)
			{
				OutError = TEXT("a live match owns no bridge");
				return false;
			}

			Board = Subsystem->GetBoard();
			if (Board == nullptr)
			{
				OutError = TEXT("the subsystem spawned no board");
				return false;
			}
			return GiveTheBoardAnOverlayMesh(Board, OutError);
		}
	};

	/** How many factories the model says should pulse. THE EXPECTATION, read off the model and
	 *  never written down. */
	static int32 PulsingFactoryCount(const FStratViewModel& Model)
	{
		int32 Count = 0;
		for (const FStratFactoryView& Factory : Model.Factories)
		{
			if (Factory.bBuildPulse)
			{
				++Count;
			}
		}
		return Count;
	}

	/** The pulsing factories' hexes, in the model's own order. */
	static void PulsingFactoryHexes(const FStratViewModel& Model, TArray<FIntPoint>& Out)
	{
		Out.Reset();
		for (const FStratFactoryView& Factory : Model.Factories)
		{
			if (Factory.bBuildPulse)
			{
				Out.Add(Factory.Hex);
			}
		}
	}

	static FString DescribeFactories(const FStratViewModel& Model)
	{
		if (Model.Factories.Num() == 0)
		{
			return TEXT("<no factories>");
		}

		TArray<FString> Parts;
		for (const FStratFactoryView& Factory : Model.Factories)
		{
			Parts.Add(FString::Printf(TEXT("%s owner=%d pulse=%d"),
				*Factory.Hex.ToString(), Factory.Owner, Factory.bBuildPulse ? 1 : 0));
		}
		return FString::Join(Parts, TEXT(", "));
	}

	static FString DescribeHexes(const TArray<FIntPoint>& Hexes)
	{
		if (Hexes.Num() == 0)
		{
			return TEXT("none");
		}

		TArray<FString> Parts;
		for (const FIntPoint& Hex : Hexes)
		{
			Parts.Add(Hex.ToString());
		}
		return FString::Join(Parts, TEXT(", "));
	}
}

// ---------------------------------------------------------------------------
// THE BUILD PULSE OVERLAY LIGHTS EXACTLY THE FACTORIES THE MODEL SAYS PULSE, AND NOTHING ELSE.
//
// THE CLAUSE THE ACCESSOR WAS ADDED FOR, by the name its own doc block names.
// `FStratFactoryView::bBuildPulse` had been composed by the bridge, mirrored onto the view
// model and published since W8 and READ BY NOTHING; `UStratMatchSubsystem::ApplyView`'s loop
// and `AStratBoardActor::BuildPulseOverlay` are the two halves that finally read it, and this
// is the gate over the pair.
//
// THE EXPECTATION IS THE MODEL'S OWN COUNT AND THE MODEL'S OWN HEXES. A clause that asserted
// "four factories, one lit" would be transcribing Ferrum Crossing into the suite; the count
// comes off `Model.Factories` on the same object that was applied, so a scenario change moves
// both sides of the comparison together and the clause keeps meaning the same thing.
//
// AND THE POSITIONS ARE CHECKED, NOT ONLY THE COUNT. A subsystem that collected the right
// NUMBER of hexes off the wrong predicate -- `Owner == ViewingSide`, say, or
// `!bHasBuiltThisTurn` -- can land on the same count on this board. Every drawn instance is
// therefore matched to a hex the model marked, through the board's own
// `WorldLocationOfHex`, and every marked hex must be found among the drawn ones.
//
// THE THREE OTHER OVERLAYS ARE READ IN THE SAME BREATH, because "the pulse was drawn" and
// "something was drawn on the reach overlay instead" are different facts and only the second
// reading tells them apart.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBuildPulseDrawnFromTheModelTest,
	"Stratocracy.StratPlay.T-UI-02.BuildPulseOverlayIsDrawnFromTheModelAlone",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBuildPulseDrawnFromTheModelTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBuildPulseDraw;

	// THE FIXTURE HAS NO BLUEPRINT DEFAULTS, so the board reports every terrain kind as
	// unmeshed. Declared expected rather than suppressed, on `StratAiMatchClauses.cpp`'s
	// precedent: a gate that leaves warnings in a green run trains the next reader to skim
	// warnings. It says nothing about the overlays, which is what this file reads --
	// `OverlayMesh` is assigned by the fixture and is a different property from
	// `TerrainMeshes`.
	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FLiveMatch Match;
	FString    Error;
	if (!TestTrue(TEXT("a live match starts and the board takes an overlay mesh"),
			Match.Start(Error)))
	{
		AddError(Error);
		return false;
	}

	UHierarchicalInstancedStaticMeshComponent* const Pulse =
		OverlayNamed(Match.Board, TEXT("BuildPulseOverlay"), Error);
	if (!TestNotNull(TEXT("the board declares a BuildPulseOverlay component"), Pulse))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Model;
	FString         BuildError;
	if (!TestTrue(TEXT("the view model builds"), Match.Subsystem->BuildViewModel(Model, BuildError)))
	{
		AddError(BuildError);
		return false;
	}

	// THE FIXTURE PREMISE. The shipped opening must actually pulse something, or every
	// assertion below is an agreement between two zeros.
	const int32 Expected = PulsingFactoryCount(Model);
	if (!TestTrue(*FString::Printf(
			TEXT("the shipped opening pulses at least one factory for the seat to move, without "
			     "which this clause could not fail (%s; sideToMove=%d viewingSide=%d)"),
			*DescribeFactories(Model), Model.Match.SideToMove, Model.ViewingSide),
			Expected > 0))
	{
		return false;
	}

	TArray<FIntPoint> ExpectedHexes;
	PulsingFactoryHexes(Model, ExpectedHexes);

	Match.Subsystem->ApplyView(Model);

	TestEqual(*FString::Printf(
			TEXT("T-UI-02: the BUILD pulse lights exactly the factories the model marked (%s)"),
			*DescribeFactories(Model)),
		Match.Board->GetBuildPulseOverlayCount(), Expected);

	// The accessor's own claim -- "off the component, not a cached number" -- turned into a
	// measurement rather than left as a sentence.
	TestEqual(TEXT("T-UI-02: GetBuildPulseOverlayCount agrees with the component it says it reads"),
		Match.Board->GetBuildPulseOverlayCount(), Pulse->GetInstanceCount());

	// NOT ON SOMEBODY ELSE'S OVERLAY. Nothing in this model asks for a reach set, an attack
	// set or a guidance ring, so all three must be empty -- and a pulse drawn on one of them
	// would otherwise be invisible to the count above.
	UHierarchicalInstancedStaticMeshComponent* const Reach =
		OverlayNamed(Match.Board, TEXT("ReachOverlay"), Error);
	if (TestNotNull(TEXT("the board declares a ReachOverlay component"), Reach))
	{
		TestEqual(TEXT("T-UI-02: and nothing was drawn on the reach overlay"),
			Reach->GetInstanceCount(), 0);
	}
	TestEqual(TEXT("T-UI-02: nor on the attack overlay"),
		Match.Board->GetTargetOverlayCount(), 0);
	TestEqual(TEXT("T-UI-02: nor on the objective ring"),
		Match.Board->GetObjectiveOverlayCount(), 0);

	// ---- and every instance sits on a hex the model marked ----------------------------
	TArray<FIntPoint> Unmatched = ExpectedHexes;
	for (int32 Index = 0; Index < Pulse->GetInstanceCount(); ++Index)
	{
		FTransform Drawn;
		if (!TestTrue(*FString::Printf(TEXT("pulse instance %d has a world transform"), Index),
				Pulse->GetInstanceTransform(Index, Drawn, /*bWorldSpace*/ true)))
		{
			continue;
		}
		const FVector Actual = Drawn.GetLocation();

		int32 Found = INDEX_NONE;
		for (int32 H = 0; H < ExpectedHexes.Num(); ++H)
		{
			// XY ONLY. `OverlayZOffset` is a presentation property and this clause is about
			// WHICH HEX, not how far above the tile plane the pulse floats.
			const FVector Candidate = Match.Board->WorldLocationOfHex(ExpectedHexes[H]);
			if (FMath::IsNearlyEqual(Candidate.X, Actual.X, 0.5)
				&& FMath::IsNearlyEqual(Candidate.Y, Actual.Y, 0.5))
			{
				Found = H;
				break;
			}
		}

		if (TestTrue(*FString::Printf(
				TEXT("T-UI-02: pulse instance %d at (%.1f, %.1f) sits on one of the factories the "
				     "model marked (%s)"),
				Index, Actual.X, Actual.Y, *DescribeHexes(ExpectedHexes)),
				Found != INDEX_NONE))
		{
			Unmatched.Remove(ExpectedHexes[Found]);
		}
	}

	TestEqual(*FString::Printf(
			TEXT("T-UI-02: and every marked factory got an instance -- unlit: %s"),
			*DescribeHexes(Unmatched)),
		Unmatched.Num(), 0);

	return true;
}

// ---------------------------------------------------------------------------
// A REFRESH THAT MARKS NOTHING TAKES THE PULSE DOWN -- AND THE OPPONENT'S TURN IS THAT CASE.
//
// THE UNCONDITIONAL CALL IS THE SUBJECT. `ApplyView` calls `ShowBuildPulses` on every refresh,
// with an empty array when nothing pulses, and its own block says there is deliberately no
// `if (Pulses.Num() > 0)` guard -- because that is the delta-shaped thinking that would leave
// last turn's pulses standing on a turn that lit none. NOTHING BUT A LIT-THEN-DARK PAIR CAN
// SEE THAT GUARD: with the guard in place the lit half is identical and only the second
// refresh differs.
//
// THE DARK MODEL IS PRODUCED BY THE RULES MODULE AND NOT BY EDITING A FLAG, which is what
// makes this clause also the record of fact 1 in the file header. An end of turn is submitted
// through the bridge; the viewing side does not follow it, so the model is rebuilt for a seat
// that is no longer to move -- and `bBuildPulse`, which inherits `side == activeSide` from
// `strat::canBuildAt`, goes false for every factory on the board. THAT DARKNESS IS ASSERTED AS
// A REQUIREMENT: a future change that lit an opponent's factories would redden here, and a
// reader who meets a dark board in the editor has this clause to tell them it is correct.
//
// THE LIT HALF IS THIS CLAUSE'S OWN POSITIVE CONTROL. On a board with no overlay mesh both
// halves read zero, and the fixture would look green while measuring nothing.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBuildPulseDarkOnTheOpponentsTurnTest,
	"Stratocracy.StratPlay.T-UI-02.BuildPulseClearsAndIsDarkOnTheOpponentsTurn",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBuildPulseDarkOnTheOpponentsTurnTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBuildPulseDraw;

	// THE FIXTURE HAS NO BLUEPRINT DEFAULTS, so the board reports every terrain kind as
	// unmeshed. Declared expected rather than suppressed, on `StratAiMatchClauses.cpp`'s
	// precedent: a gate that leaves warnings in a green run trains the next reader to skim
	// warnings. It says nothing about the overlays, which is what this file reads --
	// `OverlayMesh` is assigned by the fixture and is a different property from
	// `TerrainMeshes`.
	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FLiveMatch Match;
	FString    Error;
	if (!TestTrue(TEXT("a live match starts and the board takes an overlay mesh"),
			Match.Start(Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- lit: the seat to move is the seat being viewed --------------------------------
	FStratViewModel Lit;
	FString         BuildError;
	if (!TestTrue(TEXT("the opening view model builds"),
			Match.Subsystem->BuildViewModel(Lit, BuildError)))
	{
		AddError(BuildError);
		return false;
	}

	const int32 LitCount = PulsingFactoryCount(Lit);
	if (!TestTrue(*FString::Printf(
			TEXT("the opening pulses at least one factory, without which the dark half below "
			     "measures nothing (%s)"),
			*DescribeFactories(Lit)),
			LitCount > 0))
	{
		return false;
	}

	Match.Subsystem->ApplyView(Lit);
	if (!TestEqual(TEXT("T-UI-02, THE POSITIVE CONTROL: the pulse is lit before it is asked to "
	                    "go dark"),
			Match.Board->GetBuildPulseOverlayCount(), LitCount))
	{
		return false;
	}

	// ---- dark, through the rules module ------------------------------------------------
	const FStratResult Ended = Match.Bridge->SubmitEndTurn();
	if (!TestTrue(*FString::Printf(TEXT("the rules module accepts an end of turn: %s"),
			*Ended.Reason), Ended.bOk))
	{
		return false;
	}

	FStratViewModel Dark;
	if (!TestTrue(TEXT("the model rebuilds from the advanced state"),
			Match.Subsystem->BuildViewModel(Dark, BuildError)))
	{
		AddError(BuildError);
		return false;
	}
	if (!TestNotEqual(TEXT("the seat being viewed is no longer the seat to move, which is the "
	                       "whole premise of the hot-seat case"),
			Dark.ViewingSide, Dark.Match.SideToMove))
	{
		return false;
	}

	// THE REQUIREMENT, NOT AN OBSERVATION. §2.11.5's pulse is a nudge to the player looking at
	// the screen; on the opponent's turn that player may not build and no factory pulses.
	TestEqual(*FString::Printf(
			TEXT("T-UI-02: on the opponent's turn the model marks no factory at all -- "
			     "`bBuildPulse` inherits `side == activeSide` (%s)"),
			*DescribeFactories(Dark)),
		PulsingFactoryCount(Dark), 0);

	// ---- ONE `ApplyView` between the two readings ---------------------------------------
	Match.Subsystem->ApplyView(Dark);

	TestEqual(*FString::Printf(
			TEXT("T-UI-02: and one refresh that marks nothing takes the whole pulse down -- it "
			     "was lighting %d"),
			LitCount),
		Match.Board->GetBuildPulseOverlayCount(), 0);

	return true;
}

// ---------------------------------------------------------------------------
// THE BOARD'S OWN PULSE API: `ShowBuildPulses` DRAWS THE SET IT IS HANDED, `ClearBuildPulses`
// TAKES IT DOWN, AND NEITHER TOUCHES ANOTHER OVERLAY.
//
// BELOW THE SUBSYSTEM AND ON PURPOSE. The two clauses above drive the whole path and cannot
// tell "the subsystem collected the wrong hexes" from "the board drew them wrongly"; this one
// hands the board a set directly, so a failure names the board. The set is the LIVE MODEL's
// factory hexes -- every one of them, pulsing or not -- so no coordinate is written here and
// the count is a number the scenario produced.
//
// THE SET IS PASSED STRAIGHT THROUGH: NO SORT, NO DEDUPE, NO FILTER. `ShowBuildPulses` says so
// and gives the reason -- the hexes arrive in `FStratViewModel::Factories` order, which
// `StratViewModel.h` declares load-bearing, and re-ordering them here would make the board a
// second author of that order. A count alone cannot see a re-order, so the instances are read
// back positionally, in sequence.
//
// AND THE FOURTH OVERLAY IS NOT COLLIDABLE. `AStratBoardActor`'s constructor loop turns
// collision off on every overlay, and the pulse is the case its own comment calls sharpest: it
// lights the very factory tiles a player clicks to open a production menu, so a pulse with
// collision would make the pulse itself the reason the build could not be started. That is a
// one-line property with a real consequence and nothing else in the tree reads it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBuildPulseBoardApiTest,
	"Stratocracy.StratPlay.T-UI-02.ShowBuildPulsesDrawsTheSetItIsHandedInOrder",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBuildPulseBoardApiTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBuildPulseDraw;

	// THE FIXTURE HAS NO BLUEPRINT DEFAULTS, so the board reports every terrain kind as
	// unmeshed. Declared expected rather than suppressed, on `StratAiMatchClauses.cpp`'s
	// precedent: a gate that leaves warnings in a green run trains the next reader to skim
	// warnings. It says nothing about the overlays, which is what this file reads --
	// `OverlayMesh` is assigned by the fixture and is a different property from
	// `TerrainMeshes`.
	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FLiveMatch Match;
	FString    Error;
	if (!TestTrue(TEXT("a live match starts and the board takes an overlay mesh"),
			Match.Start(Error)))
	{
		AddError(Error);
		return false;
	}

	UHierarchicalInstancedStaticMeshComponent* const Pulse =
		OverlayNamed(Match.Board, TEXT("BuildPulseOverlay"), Error);
	if (!TestNotNull(TEXT("the board declares a BuildPulseOverlay component"), Pulse))
	{
		AddError(Error);
		return false;
	}

	// NO COLLISION. Read off the component the board actually created.
	TestEqual(TEXT("T-UI-02: the BUILD pulse overlay blocks no trace, so a lit factory stays "
	               "clickable"),
		static_cast<int32>(Pulse->GetCollisionEnabled()),
		static_cast<int32>(ECollisionEnabled::NoCollision));

	FStratViewModel Model;
	FString         BuildError;
	if (!TestTrue(TEXT("the view model builds"),
			Match.Subsystem->BuildViewModel(Model, BuildError)))
	{
		AddError(BuildError);
		return false;
	}

	// EVERY factory hex, in the model's order -- more than the opening pulses, so the board is
	// being asked to draw a set it would never receive from `ApplyView` today. That is the
	// point: this clause is about the board's obedience, not about the subsystem's predicate.
	TArray<FIntPoint> Hexes;
	for (const FStratFactoryView& Factory : Model.Factories)
	{
		Hexes.Add(Factory.Hex);
	}
	if (!TestTrue(TEXT("the shipped scenario projects at least two factories, so an order is "
	                   "visible at all"),
			Hexes.Num() >= 2))
	{
		return false;
	}

	Match.Board->ShowBuildPulses(Hexes);

	if (!TestEqual(*FString::Printf(
			TEXT("T-UI-02: the board draws one instance per hex it was handed (%s)"),
			*DescribeHexes(Hexes)),
			Match.Board->GetBuildPulseOverlayCount(), Hexes.Num()))
	{
		return false;
	}

	// IN SEQUENCE. Instance i must be hex i -- no sort, no dedupe, no filter.
	for (int32 Index = 0; Index < Hexes.Num(); ++Index)
	{
		FTransform Drawn;
		if (!TestTrue(*FString::Printf(TEXT("pulse instance %d has a world transform"), Index),
				Pulse->GetInstanceTransform(Index, Drawn, /*bWorldSpace*/ true)))
		{
			continue;
		}

		const FVector Expected = Match.Board->WorldLocationOfHex(Hexes[Index]);
		const FVector Actual   = Drawn.GetLocation();
		TestTrue(*FString::Printf(
				TEXT("T-UI-02: instance %d is drawn at the hex that was %dth in the set -- "
				     "WorldLocationOfHex(axial %s) is (%.1f, %.1f), the instance is at "
				     "(%.1f, %.1f)"),
				Index, Index, *Hexes[Index].ToString(),
				Expected.X, Expected.Y, Actual.X, Actual.Y),
			FMath::IsNearlyEqual(Expected.X, Actual.X, 0.5)
			&& FMath::IsNearlyEqual(Expected.Y, Actual.Y, 0.5));
	}

	// The other overlays are untouched by a pulse.
	TestEqual(TEXT("T-UI-02: ShowBuildPulses draws nothing on the attack overlay"),
		Match.Board->GetTargetOverlayCount(), 0);
	TestEqual(TEXT("T-UI-02: nor on the objective ring"),
		Match.Board->GetObjectiveOverlayCount(), 0);

	// ---- and the clear ----------------------------------------------------------------
	Match.Board->ClearBuildPulses();
	TestEqual(TEXT("T-UI-02: ClearBuildPulses empties the overlay"),
		Match.Board->GetBuildPulseOverlayCount(), 0);
	TestEqual(TEXT("T-UI-02: and the accessor still agrees with the component"),
		Match.Board->GetBuildPulseOverlayCount(), Pulse->GetInstanceCount());

	return true;
}
