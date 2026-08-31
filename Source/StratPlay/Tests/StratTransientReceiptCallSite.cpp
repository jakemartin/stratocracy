// The LIVE half of GATE-TRANSIENT -- the two properties of §2.11.2's transient layer that a
// world-free clause structurally cannot reach. The local ID is authorized for W8 items (5)
// and (6) by the 2026-08-29 ruling in `Tools/architect/state/global.md`, and no acceptance
// ID is minted or borrowed here; see the header block of
// `Source/StratUI/Tests/StratTransientReceiptClauses.cpp` for why `T-FAME-02`, `T-FAME-07`
// and `T-TURN-01` are refused for this work.
//
// WHY THIS FILE IS IN `StratPlay/Tests` AND NOT BESIDE THE DECIDERS. Everything asserted
// below is a property of `UStratMatchSubsystem` -- the ORDER of two statements inside
// `ApplyView`, and the pairing of the mark's lifetime with `AppliedModel`'s. `StratUI` is
// BELOW `StratPlay` and the arrow does not reverse, so a clause there could not name the
// subsystem at all. Everything that can be asserted without a world already is, next door;
// this file deliberately holds only the residue.
//
// THE TWO PROPERTIES, AND WHY EACH FAILS SILENTLY IF NOBODY PINS IT.
//
//   1. `ApplyView` DECIDES BEFORE IT RE-MARKS. The decider needs the PREVIOUS reading. Swap
//      the two lines and every refresh compares the model against itself, so the receipt list
//      is empty forever -- and an always-empty receipt list is indistinguishable from a quiet
//      match. Nothing goes red, nothing logs, and the feature is simply absent. The clause
//      makes the two orders separable by planting a rise between two applies and requiring
//      the receipt to carry the PRE-APPLY mark's reading as its `DurableBefore`.
//
//   2. THE MARK DIES WITH `AppliedModel`. A mark surviving a match teardown has a live
//      successor to be compared against: the next match's first model reads turn 1 and side
//      0, so the (Turn, SideToMove) pair differs from wherever the old match stopped and an
//      income receipt fires for a turn that never began -- a receipt about no match at all.
//      The kill arm stays quiet on that comparison because a fall emits nothing, so the
//      income arm is the whole of the observable and the clause drives exactly it.
//
// WHERE THE EXPECTATIONS COME FROM. Every reading compared below is taken off the subsystem's
// own published surface -- `GetViewModel`, `GetReceiptMark`, `GetTransientReceipts` -- or off
// the model that was handed to `ApplyView`. No fame figure, income figure, turn number or
// side count is written in this file. The fame RISE is planted, and the planted value is
// `FStratSideView::FameTotal` off the live model rather than a literal.
//
//   STRUCK> ", exactly as the StratUI clauses plant theirs and for the same second benefit:
//   STRUCK>  a call site watching the wrong field would see no rise at all."
//
// WITHDRAWN ON BOTH HALVES. The cross-reference went stale when
// `KillReceiptCarriesTheFameCombatRise` was rebuilt to plant `IncomePerTurn` as its `Before`
// reading; that clause and this one no longer plant the same thing. The mechanism half was
// never measured, and when it was, it proved wrong for this file too: a decider comparing
// `FameTotal` while the mark still writes `FameCombat` makes `ApplyViewRemarksAfterDeciding`
// red with TWO receipts, not none -- both sides rise from a mark of 0 to a `FameTotal` of
// 200. Only with the field swapped in the mark as well does the list come back empty, and
// then the clause reddens first on the mark-reads-the-applied-model assertions above it.
// NOTHING IN THIS FILE ASSERTS `Amount`, which is why neither variant reaches it here: the
// subject of these two clauses is the ORDER and the LIFETIME, and the arithmetic is
// `Source/StratUI/Tests/StratTransientReceiptClauses.cpp`'s.
//
// NO `strat::` FREE FUNCTION IS CALLED ANYWHERE BELOW. That is LNK2019 outside StratBridge
// and StratRules, measured 8x.
//
// THIS FIXTURE HAS NO TILE MESHES, and says so rather than papering over it. `StartMatch`
// spawns and draws the board in one call, so a headless gate has no moment in which to hand
// it the Blueprint defaults the content lane authors. The call therefore returns FALSE while
// `IsMatchLive()` returns true -- the split `StratMatchSubsystem.h` states in terms -- and
// the board's warning is declared expected, on `StratMatchReconcile.cpp`'s precedent.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
#include "StratTransientReceipts.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

namespace StratTransientReceiptCallSite
{
	static const int32 kFirstSide = 0;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** Everything `StartMatch` needs, on `StratMatchReconcile::MakeConfig`'s pattern. */
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

	/** The transient world, with the engine context teardown wants. See
	 *  `StratMatchReconcile.cpp` on why the context is not decoration and on the ORDER of
	 *  the two destructions. */
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

	static FString DescribeReceipts(const FStratTransientReceipts& Receipts)
	{
		TArray<FString> Parts;
		for (const FStratReceiptView& Receipt : Receipts.Receipts)
		{
			Parts.Add(FString::Printf(TEXT("{kind=%d side=%d turn=%d amount=%d before=%d after=%d}"),
				static_cast<int32>(Receipt.Kind), Receipt.Side, Receipt.Turn,
				Receipt.Amount, Receipt.DurableBefore, Receipt.DurableAfter));
		}
		return Parts.Num() == 0 ? FString(TEXT("<empty>")) : FString::Join(Parts, TEXT(", "));
	}
}

// ---------------------------------------------------------------------------
// `ApplyView` DECIDES AGAINST THE PREVIOUS MARK, THEN RE-MARKS.
//
// THE THREE APPLIES ARE WHAT MAKES THE ORDER SEPARABLE, and no shorter fixture does it.
//
//   - `StartMatch` runs its own `ApplyView` and leaves the mark SEEDED at the opening. The
//     clause asserts that first, against the subsystem's own applied model, because
//     everything after depends on there being a previous reading at all.
//   - A model carrying a planted fame rise is applied. The receipt must fire AND must carry
//     the OPENING's reading as `DurableBefore`. Re-marking first would have compared the
//     model with itself: no receipt, an empty list, and a green suite.
//   - The mark must then have MOVED to the applied model's readings -- so the same model
//     applied a third time is quiet. Without that half the clause would pass over a call
//     site that decided correctly and never re-marked at all, which would make every
//     subsequent refresh report the same kill again for the rest of the match.
//
// THE `DurableBefore` ASSERTION IS THE LOAD-BEARING ONE and it is taken from a copy of
// `GetReceiptMark()` made BEFORE the apply. A copy rather than a reference: `ApplyView`
// rewrites the member, and a reference would read the new value and assert that the receipt
// equals the state it produced.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratTransientApplyViewOrderTest,
	"Stratocracy.StratPlay.GATE-TRANSIENT.ApplyViewRemarksAfterDeciding",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratTransientApplyViewOrderTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratTransientReceiptCallSite;

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
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	// ALL-OR-NOTHING ON THE RULES SIDE AND DELIBERATELY NOT ON THE PRESENTATION SIDE.
	FString StartReason;
	Subsystem->StartMatch(Config, StartReason);
	if (!TestTrue(TEXT("the match is live whatever StartMatch returned"), Subsystem->IsMatchLive()))
	{
		AddError(StartReason);
		return false;
	}

	// ---- 1. StartMatch's own ApplyView left a seeded mark ----------------------------
	const FStratViewModel Opening = Subsystem->GetViewModel();
	if (!TestTrue(TEXT("the opening model carries at least one side"), Opening.Sides.Num() > 0))
	{
		return false;
	}
	if (!TestTrue(TEXT("GATE-TRANSIENT: StartMatch's own reconciliation seeded the mark"),
			Subsystem->GetReceiptMark().bSeeded))
	{
		return false;
	}

	const FStratReceiptMark MarkBefore = Subsystem->GetReceiptMark();
	if (!TestTrue(TEXT("the mark carries one fame reading per side of the applied model"),
			MarkBefore.FameCombat.Num() == Opening.Sides.Num()))
	{
		return false;
	}
	for (int32 Side = 0; Side < Opening.Sides.Num(); ++Side)
	{
		TestEqual(*FString::Printf(
				TEXT("GATE-TRANSIENT: the mark reads the applied model's FameCombat for side %d"),
				Side),
			MarkBefore.FameCombat[Side], Opening.Sides[Side].FameCombat);
	}
	TestEqual(TEXT("GATE-TRANSIENT: and the applied model's own turn"),
		MarkBefore.Turn, Opening.Match.Turn);

	// ---- 2. a rise, applied through the live path ------------------------------------
	if (!TestTrue(TEXT("the opening's FameTotal is a usable plant above its FameCombat"),
			Opening.Sides[0].FameTotal > Opening.Sides[0].FameCombat))
	{
		return false;
	}

	FStratViewModel Risen = Opening;
	Risen.Sides[0].FameCombat = Opening.Sides[0].FameTotal;

	Subsystem->ApplyView(Risen);

	const FStratTransientReceipts& Answer = Subsystem->GetTransientReceipts();
	if (!TestEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: ApplyView decided against the PREVIOUS mark, so the rise fired: %s"),
			*DescribeReceipts(Answer)),
			Answer.Receipts.Num(), 1))
	{
		return false;
	}

	TestEqual(TEXT("GATE-TRANSIENT: it is a KillFame receipt for the side that moved"),
		static_cast<int32>(Answer.Receipts[0].Kind),
		static_cast<int32>(EStratReceiptKind::KillFame));
	TestEqual(TEXT("GATE-TRANSIENT: naming side 0"), Answer.Receipts[0].Side, 0);

	// THE LOAD-BEARING ASSERTION. `DurableBefore` is the mark AS IT STOOD BEFORE THIS CALL --
	// which is only true if the decision was taken before the re-mark.
	TestEqual(TEXT("GATE-TRANSIENT: DurableBefore is the mark as it stood BEFORE this ApplyView"),
		Answer.Receipts[0].DurableBefore, MarkBefore.FameCombat[0]);
	TestEqual(TEXT("GATE-TRANSIENT: DurableAfter is the model that was just applied"),
		Answer.Receipts[0].DurableAfter, Risen.Sides[0].FameCombat);

	// ---- 3. and the mark moved afterwards --------------------------------------------
	TestEqual(TEXT("GATE-TRANSIENT: the mark then advanced to the applied model's reading"),
		Subsystem->GetReceiptMark().FameCombat[0], Risen.Sides[0].FameCombat);

	Subsystem->ApplyView(Risen);
	TestEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: so re-applying the same model is quiet, and the receipt is not "
			     "redrawn for the rest of the match: %s"),
			*DescribeReceipts(Subsystem->GetTransientReceipts())),
		Subsystem->GetTransientReceipts().Receipts.Num(), 0);

	return true;
}

// ---------------------------------------------------------------------------
// THE MARK IS RESET WHEREVER `AppliedModel` IS, SO A NEW MATCH IS A FIRST OBSERVATION.
//
// THE PATH THIS DRIVES IS `TearDownPresentation`, which is private and runs unconditionally
// at the top of `StartMatchInternal`. So a SECOND `StartMatch` in one session is the reachable
// route to it, and it is also the real one -- a restart and a load both go through there.
// `Deinitialize`'s copy of the same two lines is not driven here: it runs as the world dies,
// after which nothing is left to observe the mark with, and asserting it would need a probe
// this lane may not add. That gap is stated rather than left to be discovered.
//
// WHAT IS OBSERVABLE, GIVEN THAT THE RESTART RE-SEEDS THE MARK ON ITS WAY OUT.
// `StartMatchInternal` ends in `BuildViewModel` -> `ApplyView`, so `bSeeded` is true again by
// the time this clause can look -- and reading it would say nothing. The observable is the
// RECEIPT LIST after the restart. With the mark reset, the restart's first `ApplyView` is a
// first observation and emits nothing. With the mark surviving, the old match's (Turn,
// SideToMove) pair is compared against the new match's turn 1 and an income receipt fires for
// a turn that never began.
//
// SO THE FIXTURE MUST MOVE THAT PAIR FIRST, OR THE CLAUSE IS INERT. Two fresh matches both
// open at the same turn and the same side, so with no divergence the pair would match and the
// income arm would stay silent for the RIGHT answer for the WRONG reason. The clause therefore
// applies a model with a moved pair, captures the mark, and asserts the divergence explicitly
// before it asserts the silence.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratTransientMarkResetTest,
	"Stratocracy.StratPlay.GATE-TRANSIENT.MarkResetsWithTheAppliedModel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratTransientMarkResetTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratTransientReceiptCallSite;

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
	FString           Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	FString StartReason;
	Subsystem->StartMatch(Config, StartReason);
	if (!TestTrue(TEXT("the first match is live"), Subsystem->IsMatchLive()))
	{
		AddError(StartReason);
		return false;
	}

	// ---- drive the first match's mark away from a fresh opening ----------------------
	FStratViewModel Advanced = Subsystem->GetViewModel();
	if (!TestTrue(TEXT("the opening model carries at least two sides"), Advanced.Sides.Num() >= 2))
	{
		return false;
	}
	Advanced.Match.Turn       = Advanced.Match.Turn + 1;
	Advanced.Match.SideToMove = Advanced.Sides.Num() - 1 - Advanced.Match.SideToMove;

	Subsystem->ApplyView(Advanced);

	const FStratReceiptMark Stale = Subsystem->GetReceiptMark();
	if (!TestTrue(TEXT("the first match's mark is seeded before the restart"), Stale.bSeeded))
	{
		return false;
	}

	// ---- restart, which runs TearDownPresentation on its way in ----------------------
	FString RestartReason;
	Subsystem->StartMatch(Config, RestartReason);
	if (!TestTrue(TEXT("the second match is live"), Subsystem->IsMatchLive()))
	{
		AddError(RestartReason);
		return false;
	}

	const FStratViewModel& Fresh = Subsystem->GetViewModel();

	// THE FIXTURE-VALIDITY CONTROL. Without this the clause would be green over a surviving
	// mark whose pair happened to agree with the new match's.
	if (!TestTrue(*FString::Printf(
			TEXT("the stale mark's pair (turn %d, side %d) differs from the new match's "
			     "(turn %d, side %d), so a surviving mark WOULD have fired"),
			Stale.Turn, Stale.SideToMove, Fresh.Match.Turn, Fresh.Match.SideToMove),
			Stale.Turn != Fresh.Match.Turn || Stale.SideToMove != Fresh.Match.SideToMove))
	{
		return false;
	}

	// THE CLAUSE.
	TestEqual(*FString::Printf(
			TEXT("GATE-TRANSIENT: a restart is a first observation, so the new match's first "
			     "refresh emits nothing: %s"),
			*DescribeReceipts(Subsystem->GetTransientReceipts())),
		Subsystem->GetTransientReceipts().Receipts.Num(), 0);

	TestFalse(TEXT("GATE-TRANSIENT: and reports no banner change across the match boundary"),
		Subsystem->GetTransientReceipts().bBannerJustChanged);

	// The banner itself survives the reset -- it is steady state, not an edge, and a new
	// match must announce whose turn it is.
	TestEqual(TEXT("GATE-TRANSIENT: while the banner is still decided for the new match"),
		static_cast<int32>(Subsystem->GetTransientReceipts().Banner),
		static_cast<int32>(StratDecideTurnBanner(Fresh.Match.SideToMove,
		                                         Fresh.ViewingSide,
		                                         Fresh.Match.bHasResult)));

	return true;
}
