// The ROUTE gate for §2.11.2's info panel: `StratDecorateInfoPanel` is reached from the
// shipped decoration path, and it is reached AFTER the hover.
//
// `GATE-INFOPANEL`, and the ID question is settled in
// `Source/StratUI/Tests/StratInfoPanelSourceParity.cpp`'s header rather than re-argued here.
//
// WHY A ROUTE CLAUSE EXISTS AT ALL WHEN THE DECORATOR ALREADY HAS SIX. Everything in
// `Source/StratUI/Tests/StratInfoPanelClauses.cpp` drives `StratDecorateInfoPanel` directly.
// All six stay green over a `DecorateForPresentation` that never calls it -- the function
// would be correct, clause-pinned and unreachable, which is a shape this project has recorded
// reading exactly like a built feature. The lesson recorded with it was to check the ROUTE
// and not the mechanism. This is that check, and it is here because `StratPlay` is the only
// module that can see `AStratPlayerController`.
//
// AND IT PINS THE ORDERING, which is the half a "does it get called" clause would miss.
// `StratDecorateInfoPanel` selects the hovered hex out of `Model.Hexes`, so it MUST run after
// `Hover.DecorateViewModel`. Each observation below hands the controller a model with NO
// hover on it and lets the controller's own decorator put one there. A panel decorated before
// the hover would see `bHasHoveredHex` false and write an empty panel on every arm -- so the
// two filled arms below are red under that reordering, not merely stale.
//
// WHERE THE EXPECTATIONS COME FROM. The hexes are the shipped scenario's, taken off a model
// this clause builds from a bridge THE CONTROLLER NEVER SEES, and each panel field is
// compared against that model's own hex or unit row. Not one hex coordinate, terrain name,
// stat or move cost is written in this file.
//
// HOW THE HOVER IS DRIVEN, PER CLAUSE, because the two clauses below differ and the difference
// is deliberate rather than an oversight. The FIRST clause drives the hover entirely through
// the controller's own public `SetHoveredHex` / `ClearHoveredHex` seam and never writes
// `Model.Hover`. The SECOND clause drives its SHIPPED arm the same way, and writes
// `Model.Hover` directly in its CONTROL arm alone -- the leg that stands in for a reordered
// controller, which this lane may not produce by editing the controller. That write is
// disclosed again at the line that makes it, and its reason is given there.
//
// NARROWED> "The hover is driven through the controller's own public `SetHoveredHex` /
// NARROWED>  `ClearHoveredHex` seam, never by writing `Model.Hover`."
//
// THAT SENTENCE STOOD AT FILE SCOPE AND UNQUALIFIED UNTIL 2026-08-28. It was true of this file
// while the file held one clause; the second clause made it false and nothing re-read it. It is
// narrowed here rather than deleted because the SPECIES is this wave's own recurring defect --
// a claim true when written, invalidated by later work in the same file -- and the block below
// carries a struck instance of exactly that shape.
//
// AN OFFSET PAIR AND AN AXIAL PAIR PRINT IDENTICALLY on even rows, so no coordinate is typed
// here at all -- every hex below is carried out of the projection as an `FIntPoint` and never
// reconstructed.
//
// `BeginPlay` IS NOT DISPATCHED, on `StratHoverInputClauses.cpp`'s stated precedent, so
// `SetupInputComponent` never runs and no input asset is needed.
//
// WHAT THIS FILE PINS ABOUT THE SECOND ORDERING CONSTRAINT, AND THE FALSE CLAIM THAT STOOD
// HERE UNTIL 2026-08-28. This block used to say the constraint was untestable. It said:
//
//   STRUCK> "THAT THE PANEL RUNS AFTER `FStratSelectionMachine::DecorateViewModel`. That is
//   STRUCK>  the controller's second ordering constraint and it is real -- `bUnitDone` comes
//   STRUCK>  from the machine -- but it is not observable from here. The machine in a fixture
//   STRUCK>  with no live match writes `bDone` false on every unit, so a panel decorated
//   STRUCK>  BEFORE it and one decorated after it produce identical output, and an assertion
//   STRUCK>  over that pair would be green under both orders. Making it observable needs a
//   STRUCK>  live match through `UStratMatchSubsystem` with a unit actually marked done,
//   STRUCK>  which is a seam this wave does not have."
//
// THAT IS WITHDRAWN IN FULL AND IS NOT A LIVE STATEMENT ABOUT THIS TREE. The constraint IS
// pinned, by the second clause below, with no live match and no new seam. Read the paragraph
// above as history and the two below as what holds.
//
// WHY THE OLD ARGUMENT WAS WRONG, stated as the measurement that refutes it rather than as an
// apology. `FStratSelectionMachine::DecorateViewModel` writes `U.bDone =
// DoneUnits.Contains(U.UnitId)` in a plain loop over every unit with NO GUARD -- an
// unconditional, destructive assignment. So the two orders agree only when the INCOMING
// model's units already carry `bDone` false, and a fixture that owns its model by value
// controls exactly that. Seeding `bDone` TRUE on the hovered unit before handing the model to
// `DecorateForPresentation` separates the orders: the shipped order has the machine overwrite
// the seed and the panel select FALSE, while a panel run before the machine would read the
// seeded TRUE. "The machine writes false in this fixture" was true and was mistaken for "the
// machine writes nothing observable", which is a different sentence.
//
// WHAT REMAINS TRUE FROM THE OLD BLOCK AND IS NOT WITHDRAWN WITH IT: the SELECTION half --
// that the panel reads `bDone` and not a turn flag -- is pinned headlessly by
// `Stratocracy.StratUI.GATE-INFOPANEL.DoneIsThePresentationBitAndNotATurnFlag` and not here.
// This file pins the two ORDERINGS; that clause pins which bit is read.
//
// WHAT THIS FILE DOES NOT PIN, stated rather than left to be discovered:
//   - That a mouse move produces a hover. Nothing in this suite drives `UPlayerInput`.
//   - What `bDone` MEANS -- which units the machine puts in `DoneUnits`, and when. That is
//     `StratSelectionMachineParity.cpp`'s and `StratSelectionWaitClauses.cpp`'s subject. The
//     ordering clause below asserts only that the machine's write LANDED BEFORE the panel's
//     read, and it takes the machine's answer as given rather than restating it.
//   - Any widget. The panel has a model side and no asset in this wave.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Math/IntPoint.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratPlayerController.h"
#include "StratViewModel.h"

#include "StratBridge.h"

namespace StratInfoPanelRouting
{
	static const int32 kFirstSide   = 0;
	static const int32 kViewingSide = 0;

	static FString Describe(const FIntPoint& Hex)
	{
		return FString::Printf(TEXT("hex (q %d, r %d)"), Hex.X, Hex.Y);
	}

	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		UDataTable* const Units   = LoadObject<UDataTable>(nullptr, TEXT("/Game/StratData/DT_Units.DT_Units"));
		UDataTable* const Terrain = LoadObject<UDataTable>(nullptr, TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
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
		const FString ScenarioPath =
			FPaths::Combine(FPaths::Combine(FPaths::ProjectDir(), TEXT("Data")), TEXT("ferrum_crossing.json"));
		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath, kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	/** A transient game world, on `StratHoverInputClauses.cpp`'s pattern. */
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

		FTestWorldScope(const FTestWorldScope&)            = delete;
		FTestWorldScope& operator=(const FTestWorldScope&) = delete;
	};
}

// ---------------------------------------------------------------------------
// The panel reaches the model through the controller's own decorator, and follows the
// controller's own hover.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratInfoPanelReachesTheModelThroughTheControllerTest,
	"Stratocracy.StratPlay.GATE-INFOPANEL.PanelReachesTheModelThroughTheControllersDecorator",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratInfoPanelReachesTheModelThroughTheControllerTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratInfoPanelRouting;

	// The board, through a bridge the controller never sees. The hexes below are DATA.
	FStratBridge    Bridge;
	FString         Error;
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Board;
	if (!TestTrue(TEXT("StratBuildViewModel builds a model from the seeded bridge"),
			StratBuildViewModel(Bridge, kViewingSide, Board, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the board has hexes and units"), Board.Hexes.Num() > 0 && Board.Units.Num() > 0))
	{
		return false;
	}

	// A hex a unit stands on, and a hex of DIFFERENT terrain, both taken off the projection.
	const int32 UnitIndex = 0;
	const FStratUnitView& Unit = Board.Units[UnitIndex];

	const int32 OccupiedHexIndex = Board.Hexes.IndexOfByPredicate(
		[&Unit](const FStratHexView& Hex) { return Hex.Hex == Unit.Hex; });
	if (!TestTrue(TEXT("the unit stands on a hex the board lists"), OccupiedHexIndex != INDEX_NONE))
	{
		return false;
	}
	const FStratHexView& Occupied = Board.Hexes[OccupiedHexIndex];

	const int32 OtherHexIndex = Board.Hexes.IndexOfByPredicate(
		[&Occupied](const FStratHexView& Hex) { return Hex.TerrainId != Occupied.TerrainId; });
	if (!TestTrue(TEXT("the board carries a second hex of different terrain, so following the "
	                   "hover is observable through the controller"),
			OtherHexIndex != INDEX_NONE))
	{
		return false;
	}
	const FStratHexView& Other = Board.Hexes[OtherHexIndex];

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}
	AStratPlayerController* const Controller = Scope.World->SpawnActor<AStratPlayerController>();
	if (!TestNotNull(TEXT("AStratPlayerController spawned"), Controller))
	{
		return false;
	}

	// ---- Arm 1: the occupied hex, hovered through the controller's own seam ----
	// The model handed over carries NO hover. If `StratDecorateInfoPanel` ran before
	// `Hover.DecorateViewModel`, everything below this line would be an empty panel.
	Controller->SetHoveredHex(Occupied.Hex);

	FStratViewModel First = Board;
	Controller->DecorateForPresentation(First);

	TestTrue(
		*FString::Printf(TEXT("decorating for presentation while the controller hovers %s puts a "
		                      "panel on the model"), *Describe(Occupied.Hex)),
		First.InfoPanel.bHasHex);
	TestEqual(TEXT("and the panel is about that hex"), First.InfoPanel.Hex, Occupied.Hex);
	TestEqual(TEXT("and carries that hex's own terrain row"),
		First.InfoPanel.TerrainId, Occupied.TerrainId);
	TestEqual(TEXT("and that hex's own move cost"),
		First.InfoPanel.TerrainMoveCost, Occupied.TerrainMoveCost);
	TestEqual(TEXT("and that hex's own defence bonus"),
		First.InfoPanel.TerrainDefensePct, Occupied.TerrainDefensePct);

	// The unit half travels too -- the whole panel arrives, not just the hex line.
	TestTrue(TEXT("the panel reports the unit standing there"), First.InfoPanel.bHasUnit);
	TestEqual(TEXT("and it is that unit"), First.InfoPanel.UnitId, Unit.UnitId);
	TestEqual(TEXT("carrying that unit's own HP"), First.InfoPanel.UnitHp, Unit.Hp);
	TestEqual(TEXT("its own maximum"), First.InfoPanel.UnitHpMax, Unit.HpMax);
	TestEqual(TEXT("and its own attack stat"), First.InfoPanel.UnitStatAtk, Unit.StatAtk);

	// ---- Arm 2: move the controller's hover, and the panel moves with it ----
	// The identity leg at the controller's level: a route that wrote a constant, or that read
	// a hover captured once, would pass arm 1 and fail here.
	Controller->SetHoveredHex(Other.Hex);

	FStratViewModel Second = Board;
	Controller->DecorateForPresentation(Second);

	TestTrue(*FString::Printf(TEXT("the panel is still on screen while hovering %s"),
			*Describe(Other.Hex)),
		Second.InfoPanel.bHasHex);
	TestEqual(TEXT("and it has followed the controller's hover to that hex"),
		Second.InfoPanel.Hex, Other.Hex);
	TestEqual(TEXT("and now carries the OTHER hex's terrain row"),
		Second.InfoPanel.TerrainId, Other.TerrainId);
	TestNotEqual(TEXT("which is not the first hex's -- the panel is not a constant"),
		Second.InfoPanel.TerrainId, First.InfoPanel.TerrainId);

	// ---- Arm 3: clearing the controller's hover empties the panel ----
	// The decorator writes unconditionally; this is that write seen from the shipped path,
	// where a stale panel would freeze the last hex's description on screen.
	Controller->ClearHoveredHex();

	FStratViewModel Cleared = Board;
	Cleared.InfoPanel = Second.InfoPanel;   // a REAL stale panel, produced a few lines above
	Controller->DecorateForPresentation(Cleared);

	const FStratInfoPanelView Default;
	TestFalse(TEXT("a model decorated after the hover was cleared carries no panel"),
		Cleared.InfoPanel.bHasHex);
	TestEqual(TEXT("and its hex is the struct's own default, not the stale one"),
		Cleared.InfoPanel.Hex, Default.Hex);
	TestEqual(TEXT("and its terrain id is the default, not the stale one"),
		Cleared.InfoPanel.TerrainId, Default.TerrainId);
	TestFalse(TEXT("and it reports no unit"), Cleared.InfoPanel.bHasUnit);

	return true;
}

// ---------------------------------------------------------------------------
// The panel reads the DONE bit the selection machine wrote THIS frame -- the controller's
// second ordering constraint, made observable with no live match.
//
// THE CONSTRAINT. `StratDecorateInfoPanel` selects `FStratUnitView::bDone`, and
// `FStratSelectionMachine::DecorateViewModel` is what writes it. A panel decorated before the
// machine shows LAST frame's done bits: a unit that has just been marked done reads `ready`
// for one frame, and a unit whose done state was just cleared reads `done`. Every field on
// the panel stays plausible, so nothing else in this suite sees it.
//
// HOW IT IS MADE OBSERVABLE, and this is the whole of the trick. The machine's write is
// UNCONDITIONAL AND DESTRUCTIVE -- `U.bDone = DoneUnits.Contains(U.UnitId)` in a plain loop
// over every unit, with no guard (`FStratSelectionMachine::DecorateViewModel`,
// `Source/StratPlay/StratSelectionMachine.cpp`). So the two orders produce the same answer
// only when the INCOMING model already carries `bDone` false on the unit in question. This
// fixture owns its model by value and hands it over, so it controls that: seeding `bDone`
// TRUE on the hovered unit makes the orders disagree, in a fixture with no live match, no
// `UStratMatchSubsystem` and no new seam. The file's header block records that this file once
// claimed the opposite, and why the claim was wrong.
//
// THE CONTROL IS RUN, NOT ASSUMED. The clause computes what the REORDERED controller would
// have produced -- the same seeded model, hovered, handed straight to
// `StratDecorateInfoPanel` with no machine in between -- and asserts that it differs from
// what the shipped path produced. Without that leg the clause would be asserting `false` and
// could not distinguish "the machine ran first" from "nothing ever set the bit".
//
// WHERE THE EXPECTATION COMES FROM. The shipped arm compares the panel against the MACHINE'S
// OWN OUTPUT -- `Units[i].bDone` as it stands on the model after `DecorateForPresentation`
// returns -- and never against a literal. So THE PANEL ASSERTION takes no position on which
// units the machine considers done: if that answer ever changes, both sides of THAT comparison
// move together and only the ORDER is still asserted.
//
// THE OTHER TWO LEGS DO DEPEND ON THAT ANSWER, AND THE SCOPE IS STATED HERE RATHER THAN LEFT
// FOR A READER TO DERIVE. The seed-destruction leg and the differential both rest on the
// machine NOT reporting the subject unit as done, because both compare against the seed and the
// seed is `true`. If the machine ever began reporting this fixture's subject done, those two
// would redden while the panel assertion stayed green -- and the failure would be about THIS
// FIXTURE'S PREMISE rather than about the ordering.
//
// THAT PREMISE IS NOT SEPARATELY GUARDED, and saying so is the point of this paragraph. The
// `TestFalse` a few lines below guards a DIFFERENT premise -- that `StratBuildViewModel` leaves
// `bDone` false, so the seed is a real change -- and it would keep passing in the case above,
// because the builder runs before the machine and the machine never touches `Board`. So a
// reader who sees those two legs red and the panel assertion green should check what the
// machine now answers for this unit BEFORE concluding the ordering broke. Guarding it properly
// would mean asserting the machine's own `DoneUnits` membership, which is
// `StratSelectionMachineParity.cpp`'s subject and not this file's.
//
// The one typed value in this clause is the seed, which is the fixture's own input and is
// asserted to have been destroyed rather than expected.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratInfoPanelReadsThisFramesDoneBitTest,
	"Stratocracy.StratPlay.GATE-INFOPANEL.PanelReadsTheDoneBitTheSelectionMachineWroteThisFrame",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratInfoPanelReadsThisFramesDoneBitTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratInfoPanelRouting;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Board;
	if (!TestTrue(TEXT("StratBuildViewModel builds a model from the seeded bridge"),
			StratBuildViewModel(Bridge, kViewingSide, Board, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the board has units"), Board.Units.Num() > 0))
	{
		return false;
	}

	const int32     SubjectId  = Board.Units[0].UnitId;
	const FIntPoint SubjectHex = Board.Units[0].Hex;

	// One unit on that hex, or "the panel's unit" is ambiguous and so is everything below.
	int32 OnThatHex = 0;
	for (const FStratUnitView& Unit : Board.Units)
	{
		OnThatHex += (Unit.Hex == SubjectHex) ? 1 : 0;
	}
	if (!TestEqual(*FString::Printf(TEXT("exactly one unit stands on %s"), *Describe(SubjectHex)),
			OnThatHex, 1))
	{
		return false;
	}

	// The builder leaves the presentation bits at their defaults, so the seed below is a
	// change rather than a restatement. Asserted, because if it stopped being true the
	// separation this clause rests on would quietly vanish.
	if (!TestFalse(TEXT("StratBuildViewModel leaves the subject's bDone false, so seeding it "
	                    "true is a real change"),
			Board.Units[0].bDone))
	{
		return false;
	}

	// ---- The seeded model: LAST frame's done bit, still on the model -------
	FStratViewModel Seeded = Board;
	Seeded.Units[0].bDone = true;

	// ---- The control: what a panel decorated BEFORE the machine would say ----
	// The same seeded model, hovered, handed straight to the decorator. This is the
	// reordering, performed instrument-side, because this lane may not edit the controller.
	FStratViewModel Reordered = Seeded;
	Reordered.Hover.bHasHoveredHex = true;
	Reordered.Hover.HoveredHex     = SubjectHex;
	StratDecorateInfoPanel(Reordered);

	if (!TestTrue(TEXT("the control's panel found the seeded unit"), Reordered.InfoPanel.bHasUnit))
	{
		return false;
	}
	TestTrue(TEXT("a panel decorated BEFORE the machine reports the stale seeded bit -- this is "
	              "the answer the shipped order must NOT produce"),
		Reordered.InfoPanel.bUnitDone);

	// ---- The shipped path -------------------------------------------------
	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}
	AStratPlayerController* const Controller = Scope.World->SpawnActor<AStratPlayerController>();
	if (!TestNotNull(TEXT("AStratPlayerController spawned"), Controller))
	{
		return false;
	}
	Controller->SetHoveredHex(SubjectHex);

	FStratViewModel Shipped = Seeded;
	Controller->DecorateForPresentation(Shipped);

	const FStratUnitView* const AfterDecoration = Shipped.Units.FindByPredicate(
		[SubjectId](const FStratUnitView& Unit) { return Unit.UnitId == SubjectId; });
	if (!TestNotNull(TEXT("the subject unit survives decoration"), AfterDecoration))
	{
		return false;
	}
	if (!TestTrue(TEXT("the shipped path's panel found the subject unit"), Shipped.InfoPanel.bHasUnit))
	{
		return false;
	}
	TestEqual(TEXT("and it is the subject"), Shipped.InfoPanel.UnitId, SubjectId);

	// THE MACHINE RAN AND ITS WRITE LANDED: the seed was destroyed.
	TestNotEqual(TEXT("the selection machine overwrote the seeded bDone -- its write is "
	                  "unconditional, so this is how the frame's own answer arrives"),
		AfterDecoration->bDone, Seeded.Units[0].bDone);

	// THE ASSERTION: the panel carries the machine's own output, not the model's prior value.
	TestEqual(TEXT("the panel's bUnitDone is the bDone the machine left on the model this "
	               "frame, and not the value the model arrived carrying"),
		Shipped.InfoPanel.bUnitDone, AfterDecoration->bDone);

	// ---- The differential: the two orders genuinely disagree ---------------
	// If this ever stopped holding, everything above would still be green while the property
	// had become untestable -- which is precisely the mistake this clause was written to undo.
	TestNotEqual(TEXT("the shipped order and the reordered control disagree about the panel's "
	                  "done bit, so the assertion above is sensitive to the ORDER and not only "
	                  "to the bit"),
		Shipped.InfoPanel.bUnitDone, Reordered.InfoPanel.bUnitDone);

	return true;
}
