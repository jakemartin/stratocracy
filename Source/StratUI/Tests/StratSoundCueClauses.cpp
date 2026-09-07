// The world-free half of GATE-AUDIO -- which of the seven cues `StratDecideSoundCues` sounds,
// decided from two readings of `FStratViewModel` and from nothing else. The local ID is
// authorized for exactly this file, `Source/StratPlay/Tests/StratSoundDirectorCallSite.cpp`
// and `Source/StratPlay/Tests/StratShippedSoundBankParity.cpp` by the 2026-09-04 ruling in
// `Tools/architect/state/global.md`, which mints no acceptance ID and claims none.
//
// WHY THE ID IS `GATE-AUDIO` AND NOT A `T-` ID, restated in one sentence because a clause that
// borrows a stronger acceptance ID than it earns is a false clearance of that ID: the ruling
// measured the GDD for `audio|sound|music|SFX|volume` against every `T-[A-Z]+-\d+` and found no
// acceptance ID whose own defining text owns this surface. NO NEW ACCEPTANCE ID IS MINTED HERE
// -- a proposed identifier greps exactly like a defined one, and this project has been bitten
// by that.
//
// WHY THIS FILE IS IN `StratUI/Tests` AND NOT BESIDE THE PLAYER. `StratSoundCues.h` is a pure
// function over two structs: no `UWorld`, no subsystem, no bridge, no `USoundBase`. `StratUI`
// is BELOW `StratPlay` and the arrow does not reverse, so the world-free half is asserted here
// and only the residue -- the ORDER of two statements inside `ApplyView`, the lifetime of the
// mark, the recording player itself -- lives next door. That is the same split
// `StratTransientReceiptClauses.cpp` / `StratTransientReceiptCallSite.cpp` already draw, and
// the audio decider was written as that pair's sibling on purpose.
//
// WHERE EVERY EXPECTATION COMES FROM, WHICH IS THE WHOLE DISCIPLINE OF THIS DIRECTORY.
//
//   - THE BASE FIXTURE OF EVERY CLAUSE IS A REAL PROJECTION. Each clause seeds an
//     `FStratBridge` from the shipped tables and the shipped scenario and builds an
//     `FStratViewModel` from it. There is not one unit id, side index, hex, HP figure, turn
//     number or unit count written in this file.
//
//   - EVERY EXPECTATION IS A FIELD READ BACK OFF THAT MODEL OR OFF THE MARK THE MODULE'S OWN
//     `StratSoundMarkFromView` produced. `UnitMoved` is asserted to name
//     `FStratUnitView::UnitId` and `FStratUnitView::Side`; `TurnEnded` is asserted to carry
//     `FStratSoundMark::Turn` and `FStratSoundMark::SideToMove` -- the MARK's pair, not the
//     model's, which is the one place the two disagree and therefore the only place the
//     assertion means anything.
//
//   - EVERY PLANT IS A MODULE VALUE TOO, ON `StratMatchReconcile.cpp`'S PRECEDENT. A "moved"
//     unit is given ANOTHER UNIT'S `Hex` off the same model rather than a hex this file
//     invented; a "built" unit is a real unit removed from a copy of the model; a "destroyed"
//     unit is a real unit removed from the copy the mark was taken from. Two hexes on the
//     board are distinct by construction, and each clause asserts that distinctness as a
//     control before it relies on it.
//
//   - THE ONE ARITHMETIC OPERATION ON A PLANT IS THE HP FALL, AND IT IS STATED RATHER THAN
//     QUIET. Nothing in this tree publishes "an HP value strictly below this one", and
//     `AnHpRiseIsNotAnAttack` needs an ORDERED pair of readings to drive both directions of a
//     `<`. So one plant subtracts one from `FStratUnitView::Hp`, the clause asserts the
//     resulting order before using it, and the SAME two readings drive both directions -- the
//     fall and the rise -- so no third number exists to disagree with. It is a plant and never
//     an expectation: no clause below asserts an HP figure.
//
// NO `strat::` FREE FUNCTION IS CALLED ANYWHERE BELOW. Seeding goes through `FStratBridge`,
// which is what this module is permitted; a `strat::` call here is `LNK2019`, measured 8x.
//
// ONE CLAUSE BELOW IS NOT ABOUT WHICH CUE FIRES, AND IT IS NAMED HERE SO THIS BANNER STAYS
// TRUE OF ITS OWN CONTENTS. `NoSoundCueIsDeclaredAfterTheCountSentinel` reads no view model at
// all. It is about the SHAPE `StratDecideSoundCues`'S ONE-PER-KIND GATE DEPENDS ON -- the fixed
// array `bEmitted`, whose bound is written as `(int32)EStratSoundCue::Count` and which
// `Emit` then indexes with `(int32)Cue`. That is still the decider's own logic, which is the
// first of the three facets `GATE-AUDIO`'s authorizing ruling names, and this is one of the
// three files that ruling authorizes; it is written HERE rather than in a new file for exactly
// that reason, since a fourth file would need an authorization this lane cannot mint.
//
// WHAT THESE CLAUSES DO NOT PIN, NAMED SO IT IS NOT READ AS COVERED. They say nothing about
// whether a cue was AUDIBLE -- the suite runs `-nullrhi` with no audio device and no clause in
// this project will ever assert a wave played. They say nothing about WHICH asset a cue maps
// to, which is `UStratSoundBank`'s and phase D's. And they say nothing about the call sites:
// that a cue decided here is actually asked for, at the right moment, is
// `StratSoundDirectorCallSite.cpp`'s subject and cannot be seen from this module at all.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Misc/Paths.h"
#include "UObject/Class.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "UObject/UObjectGlobals.h"

#include "StratSoundCues.h"
#include "StratViewModel.h"

// The bridge, included from a .cpp and never from a UHT-parsed header -- the same constraint
// `StratViewModel.h` records about itself. A test file is a .cpp, so it may say this.
#include "StratBridge.h"

namespace StratSoundCueClauses
{
	// MUST match the side the rest of the engine-side harness seeds with. `Replay.h` states
	// that no rule in this project decides which side moves first, so this is a harness pin
	// and not a rule -- it exists so the projected opening is the same one every run.
	// `StratTransientReceiptClauses.cpp`, `StratScoreboardParity.cpp` and
	// `StratViewModelParity.cpp` pin the same value for the same stated reason.
	static const int32 kFirstSide = 0;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/**
	 * Brings a bridge to the seeded opening and projects it.
	 *
	 * Returns false and fills `OutError` rather than asserting, on
	 * `StratTransientReceiptClauses.cpp`'s precedent: a missing asset becomes a named failure
	 * inside the caller's own clause list instead of a crash in a helper.
	 *
	 * NOTHING IS HARDCODED PAST THE ASSET PATHS -- automation fixtures are one of the two
	 * places `.agents/ue-project-context.md` licenses a `/Game/` literal.
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

		const FString ScenarioPath = FPaths::Combine(DataDir(), TEXT("ferrum_crossing.json"));
		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath, kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}

		return StratBuildViewModel(Bridge, /*ViewingSide*/ kFirstSide, OutModel, OutError);
	}

	static const TCHAR* CueWord(const EStratSoundCue Cue)
	{
		switch (Cue)
		{
		case EStratSoundCue::ButtonClick:      return TEXT("ButtonClick");
		case EStratSoundCue::TurnEnded:        return TEXT("TurnEnded");
		// ADDED 2026-09-07 WITH THE ENUMERATOR. DISPLAY ONLY -- nothing branches on this
		// string. A missing arm here compiles silently under MSVC and prints `<unknown>`, so
		// the cost of forgetting it is a red run whose message does not say which cue; that is
		// the whole reason it is added in the same pass as the clauses below and not later.
		case EStratSoundCue::PlayerTurnBegan:  return TEXT("PlayerTurnBegan");
		case EStratSoundCue::UnitMoved:        return TEXT("UnitMoved");
		case EStratSoundCue::UnitAttacked:     return TEXT("UnitAttacked");
		case EStratSoundCue::UnitDestroyed:    return TEXT("UnitDestroyed");
		case EStratSoundCue::FactoryBuiltUnit: return TEXT("FactoryBuiltUnit");
		case EStratSoundCue::MatchEnded:       return TEXT("MatchEnded");
		}
		return TEXT("<unknown>");
	}

	/** Every emission, in the order the decider produced them, so a red run prints the answer
	 *  rather than only a count. */
	static FString Describe(const TArray<FStratSoundEmission>& Emissions)
	{
		TArray<FString> Parts;
		for (const FStratSoundEmission& Emission : Emissions)
		{
			Parts.Add(FString::Printf(TEXT("{cue=%s side=%d unit=%d turn=%d}"),
				CueWord(Emission.Cue), Emission.Side, Emission.UnitId, Emission.Turn));
		}
		return Parts.Num() == 0 ? FString(TEXT("<empty>")) : FString::Join(Parts, TEXT(", "));
	}

	static int32 CountOfCue(const TArray<FStratSoundEmission>& Emissions, const EStratSoundCue Cue)
	{
		int32 Count = 0;
		for (const FStratSoundEmission& Emission : Emissions)
		{
			if (Emission.Cue == Cue)
			{
				++Count;
			}
		}
		return Count;
	}

	static const FStratSoundEmission* FirstOfCue(const TArray<FStratSoundEmission>& Emissions,
	                                             const EStratSoundCue               Cue)
	{
		for (const FStratSoundEmission& Emission : Emissions)
		{
			if (Emission.Cue == Cue)
			{
				return &Emission;
			}
		}
		return nullptr;
	}

	/** `Model` with the unit at `Index` removed. The removal is the plant; the unit itself is
	 *  a real one off the shipped scenario and this file names none of its fields. */
	static FStratViewModel WithoutUnitAt(const FStratViewModel& Model, const int32 Index)
	{
		FStratViewModel Copy = Model;
		Copy.Units.RemoveAt(Index);
		return Copy;
	}
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- AN UNSEEDED MARK EMITS NOTHING.
//
// THIS IS THE FIRST CLAUSE IN THE FILE BECAUSE IT IS THE ONE WITHOUT WHICH THE FEATURE IS
// WRONG ON THE FIRST FRAME, and the shape of that wrongness is the reason it must be pinned
// rather than trusted. The first model a match ever applies has no predecessor, so EVERY unit
// on the board is "an id in the model, absent from the mark" -- which is `FactoryBuiltUnit`.
// Ferrum Crossing seeds ten units. Without `bSeeded` the match opens with the build cue firing
// on the opening refresh, and the one-per-kind rule reduces that to a SINGLE wrong sound rather
// than ten, which is WORSE rather than better: one build cue at the opening sounds exactly like
// a correct cue and nobody would file it.
//
// THE MODEL IS THE REAL TEN-UNIT OPENING AND THE CONTROL SAYS SO. A clause run against an empty
// roster would be green over a deleted rule, so the unit count is asserted non-empty and
// REPORTED before the silence is asserted.
//
// THE MARK IS DEFAULT-CONSTRUCTED AND NEVER HANDED TO `StratSoundMarkFromView`. That is the
// whole of the precondition: `FStratSoundMark::bSeeded` records THE WRITE and never the
// contents, because every field it carries has a legal value a real match also holds -- an
// empty `Units` array is a board with no units, and turn 0 and side 0 are both real. This
// project already carries a measured defect of exactly that species, a real default that could
// not signal unset.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundUnseededMarkTest,
	"Stratocracy.StratUI.GATE-AUDIO.AnUnseededMarkEmitsNothing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundUnseededMarkTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSoundCueClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	// THE CONTROL. Zero units here would satisfy every assertion below forever.
	AddInfo(FString::Printf(TEXT("the shipped scenario projects %d units"), Model.Units.Num()));
	if (!TestTrue(
			TEXT("CONTROL: the opening model carries units, so an unseeded mark WOULD have "
			     "reported them as newly built"),
			Model.Units.Num() > 0))
	{
		return false;
	}

	const FStratSoundMark Unseeded;
	if (!TestFalse(TEXT("CONTROL: the mark was never written by StratSoundMarkFromView"),
			Unseeded.bSeeded))
	{
		return false;
	}

	// A NON-EMPTY OUT PARAMETER, DELIBERATELY. The declaration says the decider empties `Out`
	// on EVERY path INCLUDING the unseeded one, so a caller handed the same array twice must
	// not find the first answer still in it. Seeding it with a real emission is what makes
	// that half assertable at all -- and a decider that returned before emptying would leave
	// this entry behind and go red here rather than in some later match.
	TArray<FStratSoundEmission> Out;
	FStratSoundEmission&        Stale = Out.AddDefaulted_GetRef();
	Stale.Cue = EStratSoundCue::MatchEnded;

	StratDecideSoundCues(Unseeded, Model, Out);

	TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: an unseeded mark has no predecessor and therefore no edges, so a "
			     "match opening on %d units is SILENT -- and the stale entry the caller's array "
			     "arrived with is gone: %s"),
			Model.Units.Num(), *Describe(Out)),
		Out.Num(), 0);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- THE TURN EDGE IS THE PAIR (Turn, SideToMove) AND NEVER EITHER HALF.
//
// BOTH DIRECTIONS ARE PINNED IN ONE CLAUSE BECAUSE EITHER ALONE IS SATISFIED BY A WRONG
// DECIDER, and this project has a measured defect class for exactly that -- an invariant
// covering only the half it was stated on.
//   - `Turn !=` ALONE goes silent on hot-seat's within-turn hand-over: side 0 plays, side 1
//     plays, the turn number does not move, and the most common turn edge in the game makes no
//     sound at all.
//   - `SideToMove !=` ALONE goes silent on a one-sided scenario, where the turn rolls with the
//     side unchanged.
// So this clause drives THREE models off one mark: side moved with the turn held, turn moved
// with the side held, and neither moved. A decider watching one half is red on one of the first
// two; a decider that always fires is red on the third.
//
// AND IT ASSERTS WHICH PAIR THE CUE CARRIES, which is the one place the mark and the model
// disagree and therefore the only place the assertion has content. `TurnEnded` names the turn
// and the side that ENDED -- the MARK's readings. The model's pair describes the turn that is
// BEGINNING, and a cue named `TurnEnded` carrying the incoming turn would be a field that reads
// correct and means the opposite.
//
// THE MOVED PAIR IS DERIVED FROM THE MODEL AND NOT WRITTEN HERE. The side is flipped through
// `Sides.Num() - 1 - SideToMove` -- the same expression `StratTransientReceiptCallSite.cpp`
// uses -- and the turn is the model's own turn advanced by one. Each is asserted to have
// actually moved before anything is read into the answer.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundTurnPairEdgeTest,
	"Stratocracy.StratUI.GATE-AUDIO.TheTurnEdgeIsThePairAndNotEitherHalf",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundTurnPairEdgeTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSoundCueClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: the scenario projects at least two sides, so a side CAN move"),
			Model.Sides.Num() >= 2))
	{
		return false;
	}

	const FStratSoundMark Mark = StratSoundMarkFromView(Model);
	if (!TestTrue(TEXT("CONTROL: StratSoundMarkFromView seeds what it writes"), Mark.bSeeded))
	{
		return false;
	}

	TArray<FStratSoundEmission> Out;

	// ---- 1. the side moved, the turn held --------------------------------------------
	FStratViewModel SideMoved = Model;
	SideMoved.Match.SideToMove = Model.Sides.Num() - 1 - Model.Match.SideToMove;
	if (!TestTrue(TEXT("CONTROL: the flipped side really differs from the mark's"),
			SideMoved.Match.SideToMove != Mark.SideToMove))
	{
		return false;
	}
	TestEqual(TEXT("CONTROL: and the turn did NOT move, so only the side half is in play"),
		SideMoved.Match.Turn, Mark.Turn);

	StratDecideSoundCues(Mark, SideMoved, Out);
	if (!TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: the side moving within one turn IS a turn edge -- hot-seat's "
			     "hand-over, and the case a `Turn !=` decider would silence: %s"),
			*Describe(Out)),
			CountOfCue(Out, EStratSoundCue::TurnEnded), 1))
	{
		return false;
	}

	const FStratSoundEmission* const Ended = FirstOfCue(Out, EStratSoundCue::TurnEnded);
	if (!TestNotNull(TEXT("the TurnEnded emission is readable"), Ended))
	{
		return false;
	}
	TestEqual(TEXT("GATE-AUDIO: and it names the side whose turn ENDED -- the MARK's, not the "
	               "model's incoming one"),
		Ended->Side, Mark.SideToMove);
	TestEqual(TEXT("GATE-AUDIO: and the turn that ENDED, which is the mark's"),
		Ended->Turn, Mark.Turn);
	TestEqual(TEXT("GATE-AUDIO: a turn edge is about the match and names no unit"),
		Ended->UnitId, static_cast<int32>(INDEX_NONE));

	// ---- 2. the turn rolled, the side held -------------------------------------------
	FStratViewModel TurnRolled = Model;
	TurnRolled.Match.Turn = Model.Match.Turn + 1;
	if (!TestTrue(TEXT("CONTROL: the rolled turn really differs from the mark's"),
			TurnRolled.Match.Turn != Mark.Turn))
	{
		return false;
	}
	TestEqual(TEXT("CONTROL: and the side did NOT move, so only the turn half is in play"),
		TurnRolled.Match.SideToMove, Mark.SideToMove);

	StratDecideSoundCues(Mark, TurnRolled, Out);
	TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: the turn rolling with the side unchanged IS a turn edge -- the "
			     "one-sided case a `SideToMove !=` decider would silence: %s"),
			*Describe(Out)),
		CountOfCue(Out, EStratSoundCue::TurnEnded), 1);

	// ---- 3. neither moved ------------------------------------------------------------
	StratDecideSoundCues(Mark, Model, Out);
	TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: and a refresh across which the pair did not move is NOT a turn "
			     "edge, so a decider that always fires is red here: %s"),
			*Describe(Out)),
		CountOfCue(Out, EStratSoundCue::TurnEnded), 0);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- ONLY A STRICTLY LOWER HP IS AN ATTACK.
//
// WHY `<` AND NOT `!=`, AND WHY THAT IS WORTH A CLAUSE. HP falls under §2.6 combat, RISES under
// §2.7 repair, and arrives fresh at `hpMax` on a §2.7 spawn -- so only the FALL has exactly one
// cause. A decider comparing `!=` would sound the combat cue every time a damaged unit sat on a
// town and healed, which is a §2.7 repair announcing itself as an attack: the player is told
// they are under fire while nothing is happening to them. `StratTransientReceipts.h` carries
// the same three-cause argument for the receipts and is the authority.
//
// THE TWO DIRECTIONS ARE DRIVEN OFF ONE ORDERED PAIR OF READINGS, which is what keeps the
// arithmetic out of the expectation side. One plant produces a value one below the model's own
// `FStratUnitView::Hp`; the clause asserts that order explicitly; then the FALL is
// (mark = high, model = low) and the RISE is (mark = low, model = high) over the very same two
// numbers. No HP figure is asserted anywhere below -- only which cue fired.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundHpRiseIsNotAnAttackTest,
	"Stratocracy.StratUI.GATE-AUDIO.AnHpRiseIsNotAnAttack",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundHpRiseIsNotAnAttackTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSoundCueClauses;

	FStratBridge    Bridge;
	FStratViewModel Healthy;
	FString         Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, Healthy, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: the scenario deploys at least one unit"), Healthy.Units.Num() > 0))
	{
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: that unit has a positive HP to fall from"),
			Healthy.Units[0].Hp > 0))
	{
		return false;
	}

	// THE ONE PLANT, AND THE ONLY ARITHMETIC IN THIS FILE. See the block above: it makes an
	// ORDERED pair out of a single module reading, and the order is asserted before use.
	FStratViewModel Hurt = Healthy;
	Hurt.Units[0].Hp = Healthy.Units[0].Hp - 1;
	if (!TestTrue(TEXT("CONTROL: the planted reading is STRICTLY BELOW the model's own"),
			Hurt.Units[0].Hp < Healthy.Units[0].Hp))
	{
		return false;
	}

	TArray<FStratSoundEmission> Out;

	// ---- the FALL: healthy at the mark, hurt in the model -----------------------------
	StratDecideSoundCues(StratSoundMarkFromView(Healthy), Hurt, Out);
	if (!TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: a unit whose HP FELL between two readings is an attack: %s"),
			*Describe(Out)),
			CountOfCue(Out, EStratSoundCue::UnitAttacked), 1))
	{
		return false;
	}

	const FStratSoundEmission* const Hit = FirstOfCue(Out, EStratSoundCue::UnitAttacked);
	if (!TestNotNull(TEXT("the UnitAttacked emission is readable"), Hit))
	{
		return false;
	}
	TestEqual(TEXT("GATE-AUDIO: naming the unit whose HP fell -- the DEFENDER, which is the only "
	               "party a diff over two models can identify"),
		Hit->UnitId, Healthy.Units[0].UnitId);
	TestEqual(TEXT("GATE-AUDIO: and that unit's own side off the model"),
		Hit->Side, Healthy.Units[0].Side);

	// ---- the RISE: the same two readings, the other way round -------------------------
	StratDecideSoundCues(StratSoundMarkFromView(Hurt), Healthy, Out);
	TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: and the SAME two readings in the other order are a §2.7 repair, "
			     "which is not combat and is silent -- the whole of why the comparison is `<` "
			     "and not `!=`: %s"),
			*Describe(Out)),
		CountOfCue(Out, EStratSoundCue::UnitAttacked), 0);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- A MOVED UNIT EMITS EXACTLY ONE MOVE CUE, NAMING ITSELF.
//
// THE PLANT IS ANOTHER UNIT'S HEX OFF THE SAME MODEL, never a hex this file invented. Two units
// cannot share a hex, so the destination is guaranteed to differ from the origin -- and the
// clause asserts that distinctness anyway, because "guaranteed by the scenario" is a property
// of a data file this lane may not edit and must not assume.
//
// THE WHOLE EMISSION LIST IS ASSERTED, NOT JUST THE MOVE COUNT. `Out.Num() == 1` is what makes
// a spurious second cue -- a turn edge fabricated out of an unchanged pair, an attack read off
// an unchanged HP -- red here rather than somewhere later. A decider that compared the UNIT ID
// instead of the HEX emits nothing at all and is red on the same line.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundOneMoveCueTest,
	"Stratocracy.StratUI.GATE-AUDIO.AMovedUnitEmitsExactlyOneMoveCue",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundOneMoveCueTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSoundCueClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: the scenario deploys at least two units, so one has somewhere "
	                   "to be moved TO that this file did not invent"),
			Model.Units.Num() >= 2))
	{
		return false;
	}

	const FStratSoundMark Mark = StratSoundMarkFromView(Model);

	FStratViewModel Moved = Model;
	Moved.Units[0].Hex = Model.Units[1].Hex;
	if (!TestTrue(TEXT("CONTROL: the destination really differs from the origin"),
			Moved.Units[0].Hex != Model.Units[0].Hex))
	{
		return false;
	}

	TArray<FStratSoundEmission> Out;
	StratDecideSoundCues(Mark, Moved, Out);

	if (!TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: one unit on a different hex is ONE cue and nothing else -- a "
			     "decider joining on the unit id rather than the hex emits none: %s"),
			*Describe(Out)),
			Out.Num(), 1))
	{
		return false;
	}
	TestEqual(TEXT("GATE-AUDIO: and it is the move cue"),
		static_cast<int32>(Out[0].Cue), static_cast<int32>(EStratSoundCue::UnitMoved));
	TestEqual(TEXT("GATE-AUDIO: naming the unit that moved"),
		Out[0].UnitId, Model.Units[0].UnitId);
	TestEqual(TEXT("GATE-AUDIO: and its own side off the model"),
		Out[0].Side, Model.Units[0].Side);
	TestEqual(TEXT("GATE-AUDIO: carrying the turn as read on THIS refresh"),
		Out[0].Turn, Moved.Match.Turn);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- A WHOLE BOARD MOVING IS ONE MOVE CUE, AND A UNIT THAT MOVED AND WAS HIT IS
// STILL BOTH KINDS.
//
// WHY THE COLLAPSE IS THE FEATURE AND NOT AN OPTIMISATION. A whole AI hand-over is ONE
// `ApplyView`: every unit on the board can move between two readings. Ten overlapping copies of
// one move sound is not ten times as informative; it is a click. The collapse lives in the
// world-free decider rather than in the player because at THIS layer it is a property of a pure
// function over two structs -- a clause asserts it in three lines with no world at all -- while
// a cooldown in the player is a wall-clock fact that a headless `-nullrhi` fixture cannot drive.
//
// AND THE SECOND HALF IS WHAT STOPS THE COLLAPSE FROM BEING A MUTE BUTTON. "At most one per
// KIND" is not "at most one": a unit that moved and was hit between two readings emits BOTH,
// which is the ordinary human path -- move, attack, one refresh. A decider that emitted one cue
// per REFRESH would pass the first half of this clause and fail the second.
//
// EVERY UNIT IS MOVED ONTO THE NEXT UNIT'S HEX, CYCLICALLY. Every destination is a module value
// off the same model, and the clause asserts that every single unit's hex actually changed
// before it asserts the collapse -- otherwise a scenario whose units happened to land back on
// themselves would report the right answer for the wrong reason.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundManyMovesAreOneCueTest,
	"Stratocracy.StratUI.GATE-AUDIO.TenMovesAreOneCue",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundManyMovesAreOneCueTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSoundCueClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: the scenario deploys at least two units"), Model.Units.Num() >= 2))
	{
		return false;
	}

	const FStratSoundMark Mark = StratSoundMarkFromView(Model);

	// EVERY UNIT ONTO THE NEXT ONE'S HEX. Read from `Model` and written into `Shuffled`, so no
	// entry is overwritten before it is read.
	FStratViewModel Shuffled = Model;
	for (int32 Index = 0; Index < Model.Units.Num(); ++Index)
	{
		Shuffled.Units[Index].Hex = Model.Units[(Index + 1) % Model.Units.Num()].Hex;
	}
	for (int32 Index = 0; Index < Model.Units.Num(); ++Index)
	{
		if (!TestTrue(*FString::Printf(
				TEXT("CONTROL: unit %d really moved, so all %d of them are genuine move edges"),
				Model.Units[Index].UnitId, Model.Units.Num()),
				Shuffled.Units[Index].Hex != Model.Units[Index].Hex))
		{
			return false;
		}
	}

	TArray<FStratSoundEmission> Out;
	StratDecideSoundCues(Mark, Shuffled, Out);

	AddInfo(FString::Printf(TEXT("%d units moved across one refresh and produced: %s"),
		Model.Units.Num(), *Describe(Out)));

	TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: %d units moving across ONE refresh is ONE move cue -- a per-unit "
			     "emission would be %d overlapping copies of the same sound"),
			Model.Units.Num(), Model.Units.Num()),
		CountOfCue(Out, EStratSoundCue::UnitMoved), 1);
	TestEqual(TEXT("GATE-AUDIO: and nothing else fired, so the collapse did not swallow the "
	               "refresh whole"),
		Out.Num(), 1);

	// ---- and the collapse is PER KIND, not per refresh --------------------------------
	if (!TestTrue(TEXT("CONTROL: the moved unit has a positive HP to fall from"),
			Model.Units[0].Hp > 0))
	{
		return false;
	}
	FStratViewModel MovedAndHit = Shuffled;
	MovedAndHit.Units[0].Hp = Model.Units[0].Hp - 1;
	if (!TestTrue(TEXT("CONTROL: and the planted reading is strictly below the marked one"),
			MovedAndHit.Units[0].Hp < Mark.Units[0].Hp))
	{
		return false;
	}

	StratDecideSoundCues(Mark, MovedAndHit, Out);

	TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: a unit that MOVED and was HIT between two readings emits BOTH "
			     "kinds -- the collapse is per KIND and never per refresh: %s"),
			*Describe(Out)),
		Out.Num(), 2);
	TestEqual(TEXT("GATE-AUDIO: still exactly one move cue"),
		CountOfCue(Out, EStratSoundCue::UnitMoved), 1);
	TestEqual(TEXT("GATE-AUDIO: and exactly one attack cue beside it"),
		CountOfCue(Out, EStratSoundCue::UnitAttacked), 1);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- A UNIT IN THE MARK AND ABSENT FROM THE MODEL IS DESTROYED.
//
// READ THIS CLAUSE AND `AUnitAbsentFromTheMarkEmitsBuilt` TOGETHER, AND DO NOT DELETE EITHER AS
// A DUPLICATE OF THE OTHER. They are the two set differences, and THEY REDDEN EACH OTHER'S
// MUTANT: a decider with the two differences SWAPPED -- built for a departure, destroyed for an
// arrival -- passes neither, while EITHER clause alone is green over a decider that emits the
// same cue for both directions. The inversion is not hypothetical: the two passes are eight
// lines apart in `StratSoundCues.cpp`, over the same `TMap` and the same `TSet`, and the only
// thing distinguishing them is which container is iterated.
//
// THE INFERENCE THIS PINS IS `ApplyView`'S OWN. `FStratViewModel::Units` is every LIVING unit --
// the destroy loop's stated contract -- so a unit that left the model died. That is the same
// inference the actor reconciliation already makes, which is what keeps a cue and an actor
// agreeing about which unit they mean.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundAbsentFromModelIsDestroyedTest,
	"Stratocracy.StratUI.GATE-AUDIO.AUnitAbsentFromTheModelEmitsDestroyed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundAbsentFromModelIsDestroyedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSoundCueClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: the scenario deploys at least two units, so one can leave and "
	                   "a board still remain"),
			Model.Units.Num() >= 2))
	{
		return false;
	}

	const FStratSoundMark      Mark    = StratSoundMarkFromView(Model);
	const FStratUnitView       Departed = Model.Units[0];
	const FStratViewModel      Bereaved = WithoutUnitAt(Model, 0);

	TArray<FStratSoundEmission> Out;
	StratDecideSoundCues(Mark, Bereaved, Out);

	if (!TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: a unit in the mark and absent from the model died, and that is the "
			     "ONLY cue this refresh produces: %s"),
			*Describe(Out)),
			Out.Num(), 1))
	{
		return false;
	}
	TestEqual(TEXT("GATE-AUDIO: it is the death cue and NOT the build cue -- the two set "
	               "differences are not interchangeable"),
		static_cast<int32>(Out[0].Cue), static_cast<int32>(EStratSoundCue::UnitDestroyed));
	TestEqual(TEXT("GATE-AUDIO: naming the unit that left the model"),
		Out[0].UnitId, Departed.UnitId);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- A UNIT IN THE MODEL AND ABSENT FROM THE MARK WAS BUILT.
//
// THE OTHER SET DIFFERENCE. See `AUnitAbsentFromTheModelEmitsDestroyed`'s block for why these
// two clauses are each other's mutant and why neither is redundant; it is not restated here.
//
// THE CUE IS NAMED FOR THE FACTORY AND NOT FOR THE UNIT because §2.7's Build is the only thing
// in this project that creates one. And it is NOT emitted at `SubmitProductionChoice`: §2.7 can
// HOLD a build, so a sound at submit time would announce a unit that has not spawned and may
// not spawn this turn. That placement is the call site's and is
// `StratSoundDirectorCallSite.cpp`'s to observe; what is pinned HERE is that the arrival of an
// id in the model is what decides it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundAbsentFromMarkIsBuiltTest,
	"Stratocracy.StratUI.GATE-AUDIO.AUnitAbsentFromTheMarkEmitsBuilt",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundAbsentFromMarkIsBuiltTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSoundCueClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: the scenario deploys at least two units"), Model.Units.Num() >= 2))
	{
		return false;
	}

	// THE MARK IS TAKEN FROM A BOARD THAT IS SHORT ONE UNIT, so the full model is that board
	// with an arrival on it. The arriving unit is a real one off the scenario.
	const FStratUnitView  Arrived = Model.Units[0];
	const FStratSoundMark Mark    = StratSoundMarkFromView(WithoutUnitAt(Model, 0));
	if (!TestTrue(TEXT("CONTROL: the mark is seeded, so this is not the unseeded rule firing"),
			Mark.bSeeded))
	{
		return false;
	}

	TArray<FStratSoundEmission> Out;
	StratDecideSoundCues(Mark, Model, Out);

	if (!TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: an id in the model and absent from the mark is a factory "
			     "delivering a unit, and that is the ONLY cue this refresh produces: %s"),
			*Describe(Out)),
			Out.Num(), 1))
	{
		return false;
	}
	TestEqual(TEXT("GATE-AUDIO: it is the build cue and NOT the death cue"),
		static_cast<int32>(Out[0].Cue), static_cast<int32>(EStratSoundCue::FactoryBuiltUnit));
	TestEqual(TEXT("GATE-AUDIO: naming the unit that arrived"), Out[0].UnitId, Arrived.UnitId);
	TestEqual(TEXT("GATE-AUDIO: and its own side off the model, which still carries it"),
		Out[0].Side, Arrived.Side);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- A DESTROYED UNIT'S CUE NAMES THE SIDE THAT LOST IT.
//
// THIS IS THE CLAUSE THAT MAKES `FStratUnitSoundMark::Side` A FEATURE RATHER THAN A DEAD FIELD,
// AND IT IS WHY THE FIELD SHOULD STAY. The header describes it as stored and never COMPARED,
// which is true and is not the same as never READ: `UnitDestroyed` is the one cue whose subject
// is absent from the current model, so `Model` cannot say whose unit died and the mark is the
// last place that fact exists. Without the field the cue would carry `INDEX_NONE` and a
// per-side mix would be unbuildable for exactly the cue most likely to want one -- and nothing
// downstream could recover it, because the unit is gone.
//
// THE CONTROL IS THE HALF THAT MAKES THE ASSERTION MEAN SOMETHING. `Emission.Side` equalling
// the departed unit's side would be satisfiable by a decider reading the MODEL if any surviving
// unit shared that side, so this clause removes EVERY unit of one side and asserts that the
// side is absent from the model before it reads the answer. A decider that sourced the side
// from the model then has nowhere to get it from, and `INDEX_NONE` is red on the next line.
//
// THE SIDE IS NOT WRITTEN HERE. It is `FStratUnitView::Side` off the model's first unit,
// whatever that is, and the population removed is every unit sharing it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundDestroyedCarriesSideTest,
	"Stratocracy.StratUI.GATE-AUDIO.ADestroyedUnitCarriesItsSide",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundDestroyedCarriesSideTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSoundCueClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: the scenario deploys units"), Model.Units.Num() > 0))
	{
		return false;
	}

	const FStratSoundMark Mark = StratSoundMarkFromView(Model);

	// WIPE ONE WHOLE SIDE. The side is read off the model; this file names no side index.
	const int32 WipedSide = Model.Units[0].Side;
	if (!TestTrue(TEXT("CONTROL: the model's own side reading is a real index"),
			WipedSide != INDEX_NONE))
	{
		return false;
	}

	FStratViewModel Survivors = Model;
	Survivors.Units.RemoveAll([WipedSide](const FStratUnitView& Unit)
	{
		return Unit.Side == WipedSide;
	});

	if (!TestTrue(TEXT("CONTROL: at least one unit actually left the model"),
			Survivors.Units.Num() < Model.Units.Num()))
	{
		return false;
	}
	for (const FStratUnitView& Unit : Survivors.Units)
	{
		if (!TestTrue(*FString::Printf(
				TEXT("CONTROL: no surviving unit carries side %d, so the MODEL cannot supply it "
				     "and only the mark can"), WipedSide),
				Unit.Side != WipedSide))
		{
			return false;
		}
	}

	TArray<FStratSoundEmission> Out;
	StratDecideSoundCues(Mark, Survivors, Out);

	const FStratSoundEmission* const Death = FirstOfCue(Out, EStratSoundCue::UnitDestroyed);
	if (!TestNotNull(*FString::Printf(
			TEXT("GATE-AUDIO: wiping a side produces a death cue: %s"), *Describe(Out)),
			Death))
	{
		return false;
	}

	TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: and it names side %d -- the side that LOST the unit, recoverable "
			     "from `FStratUnitSoundMark::Side` alone because the model no longer carries "
			     "that side at all"), WipedSide),
		Death->Side, WipedSide);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- THE DECIDER NEVER EMITS `ButtonClick`, `MatchEnded` OR `PlayerTurnBegan`.
//
// ALL THREE ARMS ARE OWNED ELSEWHERE, EACH BY A LATCH, AND THE ABSENCE IS A STATEMENT RATHER
// THAN AN OMISSION.
//   - A CLICK IS NOT A CHANGE IN THE MODEL. A refused button changes nothing at all and must
//     still click, so no diff over two view models can ever see one. The six input verbs emit
//     it directly.
//   - A MATCH ENDING **IS** VISIBLE IN THE MODEL, AND IS STILL NOT TAKEN HERE. That is the arm
//     this clause exists for. `ConcludeMatchIfEnded` already owns a once-per-match latch and
//     sounds the cue inside it; a `bHasResult` arm in the decider would be a SECOND answer to
//     "has this match already ended", and a concluded match refreshes many times -- so the
//     wrong answer is not one extra sound, it is a victory sting on every mouse move.
//   - A HAND-BACK IS **NOT DECIDABLE FROM TWO VIEW MODELS AT ALL**, and that is the third arm,
//     added 2026-09-07 with `EStratSoundCue::PlayerTurnBegan`. `StratSoundCues.h` gives four
//     separate reasons; the one a clause can drive is the first: the condition needs
//     `FStratMatchConfig::AiSides`, which is a CONFIGURATION and is not a field of
//     `FStratViewModel`, so the decider's own declared inputs cannot answer it. What a decider
//     arm COULD see is the `(Turn, SideToMove)` change `TurnEnded` already keys on -- which is
//     exactly the wrong test, because it fires on a hot-seat human -> human hand-over too. So
//     the third diff below is a plain non-concluded turn hand-over: the shape a decider arm
//     would most plausibly be written on, and the one this clause has to be red over.
//
// **THE NAME WAS `TheDeciderNeverEmitsButtonClickOrMatchEnded` UNTIL 2026-09-07 AND WAS RENAMED
// RATHER THAN LEFT, WHICH IS NOT COSMETIC.** A test name is read without its header, and a
// clause asserting three absences while naming two is this project's recorded overclaiming-name
// defect pointed the other way -- it would have UNDER-claimed, and a reader auditing "what pins
// the hand-back cue's provenance" would have grepped past it. The old name appears nowhere else
// in this tree.
//
// THE FIXTURE DRIVES A CONCLUDED MODEL SPECIFICALLY, because a decider with a `bHasResult` arm
// is invisible to every other clause in this file: none of them ever sets that flag. The flag
// is planted and the plant is checked through the module's own predicate rather than by reading
// the bool back.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundNeverClickOrMatchEndedTest,
	"Stratocracy.StratUI.GATE-AUDIO.TheDeciderNeverEmitsButtonClickMatchEndedOrPlayerTurnBegan",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundNeverClickOrMatchEndedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSoundCueClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: the scenario deploys at least two units and two sides"),
			Model.Units.Num() >= 2 && Model.Sides.Num() >= 2))
	{
		return false;
	}
	if (!TestFalse(TEXT("CONTROL: the opening model carries no result yet"),
			Model.Match.bHasResult))
	{
		return false;
	}

	const FStratSoundMark Mark = StratSoundMarkFromView(Model);

	// FIVE DIFFS ACROSS ONE MARK, one per arm the decider does own, plus the concluded model
	// that is the whole point of the clause.
	FStratViewModel Concluded = Model;
	Concluded.Match.bHasResult = true;
	Concluded.Match.Turn       = Model.Match.Turn + 1;
	Concluded.Units.RemoveAt(0);
	Concluded.Units[0].Hex = Model.Units[0].Hex;

	// THE THIRD DIFF, ADDED 2026-09-07: A PLAIN, LIVE TURN HAND-OVER.
	//
	// IT IS THE ONE THE `PlayerTurnBegan` ASSERTION NEEDS, AND `Concluded` ABOVE CANNOT STAND IN
	// FOR IT. A decider arm written for the hand-back would key on the `(Turn, SideToMove)`
	// change -- the same change `TurnEnded` keys on -- and a hypothetical arm that ALSO refused
	// on `bHasResult`, which is the shape someone copying `MatchEnded`'s reasoning would write,
	// would be silent on `Concluded` and loud here. So the mutant this file most has to fear is
	// invisible to the other two diffs by construction.
	//
	// THE NEW SIDE IS TAKEN FROM THE MODEL'S OWN ROSTER and is not written here: `Sides` is
	// the projected list of seats and the mark carries the opening's `SideToMove`, so
	// "somebody else's turn" is `the first entry that is not the mark's`, read off the module.
	FStratViewModel HandedBack = Model;
	HandedBack.Match.Turn      = Model.Match.Turn + 1;
	for (int32 Index = 0; Index < HandedBack.Sides.Num(); ++Index)
	{
		if (Index != Model.Match.SideToMove)
		{
			HandedBack.Match.SideToMove = Index;
			break;
		}
	}
	if (!TestTrue(TEXT("CONTROL: the hand-over diff really does change the side to move, so a "
	                   "decider arm keyed on that change would have something to fire on"),
			HandedBack.Match.SideToMove != Model.Match.SideToMove))
	{
		return false;
	}
	if (!TestFalse(TEXT("CONTROL: and it is NOT concluded -- otherwise a hand-back arm that "
	                    "copied `MatchEnded`'s `bHasResult` refusal would be silent here for "
	                    "the wrong reason and this diff would prove nothing"),
			HandedBack.Match.bHasResult))
	{
		return false;
	}

	TArray<FStratViewModel> Diffs;
	Diffs.Add(Model);
	Diffs.Add(Concluded);
	Diffs.Add(HandedBack);

	TArray<FStratSoundEmission> Out;
	for (const FStratViewModel& Diff : Diffs)
	{
		StratDecideSoundCues(Mark, Diff, Out);

		AddInfo(FString::Printf(
			TEXT("a diff carrying bHasResult=%s turn=%d sideToMove=%d produced: %s"),
			Diff.Match.bHasResult ? TEXT("true") : TEXT("false"), Diff.Match.Turn,
			Diff.Match.SideToMove, *Describe(Out)));

		TestEqual(*FString::Printf(
				TEXT("GATE-AUDIO: no diff over two view models can see a CLICK, so the decider "
				     "never emits one: %s"),
				*Describe(Out)),
			CountOfCue(Out, EStratSoundCue::ButtonClick), 0);

		TestEqual(*FString::Printf(
				TEXT("GATE-AUDIO: and a match ending is `ConcludeMatchIfEnded`'s latch and not a "
				     "decider arm, so a CONCLUDED model is still silent on it -- a second latch "
				     "here would sound a victory sting on every refresh of a finished match: %s"),
				*Describe(Out)),
			CountOfCue(Out, EStratSoundCue::MatchEnded), 0);

		TestEqual(*FString::Printf(
				TEXT("GATE-AUDIO: and a hand-back is not decidable from two view models at all "
				     "-- its condition needs `FStratMatchConfig::AiSides`, which this "
				     "function's own declaration excludes -- so `PlayerTurnBegan` belongs to "
				     "`NotePlayerTurnBeganIfDue` and is never a decider arm. A `(Turn, "
				     "SideToMove)` arm here would also fire on a hot-seat human -> human "
				     "swap, which is a different occasion: %s"),
				*Describe(Out)),
			CountOfCue(Out, EStratSoundCue::PlayerTurnBegan), 0);
	}

	// THE CONTROL FOR THE WHOLE CLAUSE. The last diff must have produced SOMETHING, or the three
	// assertions above were satisfied by a decider that emitted nothing at all.
	if (!TestTrue(*FString::Printf(
			TEXT("CONTROL: the last diff really did move -- it emitted %s -- so the three "
			     "counts above are zero because those arms are absent, not because the decider "
			     "was quiet"),
			*Describe(Out)),
			Out.Num() > 0))
	{
		return false;
	}

	// AND THE SHARPER HALF OF THAT CONTROL, ADDED 2026-09-07 WITH THE THIRD ASSERTION. The final
	// diff is the live turn hand-over, which is the diff a `PlayerTurnBegan` arm would most
	// plausibly be written on -- so the zero above only means something if the decider was
	// AWAKE on it. `TurnEnded` is the arm it does own for that exact change, and asserting it
	// fired is what separates "the hand-back arm is absent" from "the decider went silent on
	// turn changes altogether", which the `Out.Num() > 0` above cannot tell apart because a
	// unit's move would satisfy it.
	if (!TestEqual(*FString::Printf(
			TEXT("CONTROL: the live turn hand-over sounded `TurnEnded` exactly once, so the "
			     "`PlayerTurnBegan` zero was measured on a diff the decider reacted to: %s"),
			*Describe(Out)),
		CountOfCue(Out, EStratSoundCue::TurnEnded), 1))
	{
		return false;
	}

	return true;
}

// ---------------------------------------------------------------------------
// GATE-AUDIO -- A MODEL DECIDED AGAINST ITS OWN MARK IS QUIET.
//
// THE STEADY-STATE CLAUSE, AND IT IS THE ONE THAT CATCHES AN ALWAYS-TRUE COMPARISON. Every
// other clause in this file plants a change and asserts a cue fires; all of them stay green
// over a decider whose every comparison is inverted or vacuous. This one is the other
// direction: a refresh across which NOTHING moved must sound nothing, and the shipped game
// takes that path on every mouse move, every hover, every selection change and every camera
// nudge. A decider that fired on an unchanged model would make the game a continuous noise.
//
// IT IS THE STRONGEST FORM OF THAT ASSERTION AVAILABLE HERE -- the mark is taken from the very
// model it is then compared against, so every field on both sides is identical by
// construction and there is no reading a comparison could legitimately find a difference in.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundRemarkedModelIsQuietTest,
	"Stratocracy.StratUI.GATE-AUDIO.ARemarkedModelIsQuiet",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundRemarkedModelIsQuietTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSoundCueClauses;

	FStratBridge    Bridge;
	FStratViewModel Model;
	FString         Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	// THE CONTROL. A model with no units and no sides would be quiet under any decider at all.
	if (!TestTrue(*FString::Printf(
			TEXT("CONTROL: the model carries %d units and %d sides, so there is something for a "
			     "broken comparison to fire on"),
			Model.Units.Num(), Model.Sides.Num()),
			Model.Units.Num() > 0 && Model.Sides.Num() > 0))
	{
		return false;
	}

	const FStratSoundMark Mark = StratSoundMarkFromView(Model);
	if (!TestEqual(TEXT("CONTROL: the mark read every unit the model carries"),
			Mark.Units.Num(), Model.Units.Num()))
	{
		return false;
	}

	TArray<FStratSoundEmission> Out;
	StratDecideSoundCues(Mark, Model, Out);

	TestEqual(*FString::Printf(
			TEXT("GATE-AUDIO: a refresh across which nothing moved is SILENT -- the path the "
			     "shipped game takes on every mouse move: %s"),
			*Describe(Out)),
		Out.Num(), 0);

	return true;
}
// ---------------------------------------------------------------------------
// GATE-AUDIO -- NO ENUMERATOR OF `EStratSoundCue` IS DECLARED AT OR ABOVE THE `Count` SENTINEL.
//
// RENAMED FROM `MatchEndedIsTheLastSoundCue` ON 2026-09-06, over base commit `f7da9ca`, in the
// pass that made the old name untrue OF WHAT THE CLAUSE ASSERTS. `MatchEnded` is still the last
// cue -- `StratSoundCues.h`'s second `static_assert` now pins exactly that, at compile time --
// so the old name was not false about the world; it was false about THIS TEST, which no longer
// asks anything about `MatchEnded` and takes its bound from `Count` instead. A name that
// describes a property the clause has stopped asserting is the same defect as an overclaiming
// one, arrived at from the other side. THE COST OF THE RENAME, STATED: the old full name
// appears in `Tools/architect/state/global.md` and in two `Tools/architect/gate_reports/`
// files, and a search for it against a future report will find nothing.
//
// THE DEFECT THIS EXISTS AGAINST IS A MEMORY WRITE AND NOT A STALE COMMENT.
// `StratDecideSoundCues` opens its one-per-kind gate as
//
//     bool bEmitted[static_cast<int32>(EStratSoundCue::Count)] = {};
//
// and its `Emit` lambda then indexes that array with `static_cast<int32>(Cue)`. That bound
// covers the enum only while every enumerator's value is strictly below `Count`. DECLARE ONE
// CUE AFTER `Count` AND THE FIRST TIME THE DECIDER EMITS IT, THE WRITE LANDS PAST THE END OF
// THAT STACK ARRAY.
//
// WHAT THIS CLAUSE PINS THAT THE `static_assert`S DO NOT, WHICH IS THE ONLY REASON IT STILL
// EXISTS AND IS STATED FIRST RATHER THAN ASSUMED.
//
// `StratSoundCues.h` now carries two `static_assert`s. The first pins `ButtonClick == 0`, so
// `Count` is the NUMBER of cues. The second pins `MatchEnded + 1 == Count`, so nothing was
// given an explicit value and nothing was inserted between the last cue and the sentinel. THOSE
// TWO CLOSE MOST OF WHAT THIS CLAUSE USED TO CLOSE, at compile time, without a suite run, which
// is strictly better. Read plainly: the "explicit value" half of this clause's old banner --
// `Ambient = 64` in the middle of the list -- is now the compiler's, not this file's, and this
// clause is NOT the net under it any more.
//
// THE RESIDUE IS ONE CASE AND IT IS NOT SMALL. A cue declared AFTER `Count` takes a value
// greater than `Count`, changes no other enumerator's value, and is therefore invisible to
// every `static_assert` that can be written -- an assert cannot name a symbol whose name it
// does not know, and C++17 does not enumerate an enum. The header says so itself, at length,
// under "(3) AND THE ONE NEITHER ASSERT CAN SEE". Two instruments see that case: a `check` in
// `Emit`, which needs the cue to actually FIRE and is compiled out of Shipping, and this
// clause, which sees it on any suite run whether the cue ever fires or not. So this is not a
// clause kept for appearances; it is the only thing in the tree that reddens on the residue
// WITHOUT requiring the defect to be executed.
//
// THIS CLAUSE PINS A RELATIONSHIP AND DELIBERATELY NOT A COUNT, WHICH IS STILL THE WHOLE OF ITS
// DESIGN. A clause asserting `NumEnums()` equals some number would go RED for the CORRECT edit
// -- somebody adding a legitimate cue above `MatchEnded`, where the array grows with it and
// nothing is unsafe. So the assertion is the exact safety condition of the indexing above and
// nothing more: EVERY declared enumerator's value lies within `[0, Count)`. Insert `Retreated`
// between `UnitMoved` and `UnitAttacked` and this clause stays green, correctly. Declare it
// after `Count` and this clause is the only thing in the project that goes red on a suite run.
//
// WHERE THE EXPECTATION COMES FROM. There is no number written in this clause. The bound is
// read from the module's own `EStratSoundCue::Count` -- the identical expression
// `StratSoundCues.cpp` sizes `bEmitted` with -- the enumerators are read from the module's own
// reflected `UEnum`, and the comparison is between the two.
//
// THE TWO ENTRIES THAT ARE EXCLUDED, AND WHY NEITHER IS A BLACKLIST THAT GROWS.
//   - `Count` ITSELF, excluded BY VALUE and never by name: the walk drops the entry whose value
//     equals the bound it read from `EStratSoundCue::Count`. Nothing in the filter matches the
//     string "Count", so a sentinel renamed tomorrow needs no edit here. Its name IS asserted,
//     as CONTROL 1c, so a bound that had come to mean something else surfaces.
//   - UHT'S OWN `<EnumName>_MAX`, excluded by name suffix. This one MUST be a name rule: the
//     engine synthesises it at `UEnum::SetEnums` time with a value one past the largest
//     declared enumerator -- 8 here, against `Count` at 7 -- so it is outside the bound by
//     construction and a value rule would report it as the offender. It is matched
//     case-sensitively on purpose: `FString`'s comparisons ignore case by default in this
//     engine, and a real enumerator ending `_max` must not be waved through.
//
// `UEnum::HasMetaData(TEXT("Hidden"), Index)` WAS PROPOSED FOR THIS AND IS DELIBERATELY NOT
// USED. Three measured reasons, in order of weight.
//   (1) `Hidden` DOES NOT MEAN "SENTINEL", it means "keep this out of Blueprint dropdowns", and
//       a future REAL cue marked `Hidden` for that cosmetic reason would be silently dropped
//       from this walk and from `StratShippedSoundBankParity.cpp`'s `AllCues()`. Silent
//       under-coverage is the exact failure shape both files exist against.
//   (2) IT WOULD NOT REMOVE THE `_MAX` NAME RULE ANYWAY. The generated
//       `Intermediate/.../StratSoundCues.gen.cpp` carries `{ "Count.Hidden", "" }` and carries
//       NO metadata pair for `_MAX` at all, because `_MAX` is not in the generated enumerator
//       table -- the engine appends it later. `StratMatchResultModelClauses.cpp` already pairs
//       `HasMetaData` with an explicit `_MAX` name test for that reason.
//   (3) IT IS EDITOR-ONLY AND COSTS A WHOLE-FILE GUARD. `UEnum::HasMetaData` is declared under
//       `#if WITH_METADATA`, and this tree has the measurement: a Win64 Development **Game**
//       build on 2026-08-31 emitted `error C2039: 'HasMetaData': is not a member of 'UEnum'`
//       twice, and `StratMatchResultModelClauses.cpp` carries a whole-file `#if WITH_EDITOR`
//       guard as the fix, whose own banner names the spreading cost. The value bound below
//       needs no guard and compiles in every target CI builds.
//
// THE FOUR CONTROLS, because a reflection walk that visits nothing passes every bound check
// ever written.
//   (1) THE INSTRUMENT SPEAKS. `StaticEnum<EStratSoundCue>()` resolves, and the enumerator this
//       clause takes its bound FROM is found in it by value and reports the name `Count`.
//   (2) THE WALK VISITED THE ENUM. The number of entries bound-checked is asserted against
//       `NumEnums()` minus the entries excluded; the exclusions are asserted to be EXACTLY TWO
//       and are named in the message; and the checked count is asserted equal to `Count`
//       itself, which is the module's own statement of how many real cues there are. That last
//       one is a second, independent detector of the same mutant: a cue declared after `Count`
//       makes the walk check `Count + 1` entries.
//   (3) THE PREDICATE DISCRIMINATES. The comparison the loop uses is exercised on a synthetic
//       value one past the bound and asserted to FAIL it.
//   (4) THE WHOLE WALK IS RUN OVER A MUTANT AND REDDENS. This is the control that answers "did
//       this clause stop asking anything". The exclusion-and-offender logic is a free function
//       over a list of (name, value) pairs; the clause feeds it a synthetic list shaped like
//       today's enum and asserts NO offenders, then feeds it the same list with one extra
//       enumerator declared after the sentinel -- the exact mutant this clause exists for --
//       and asserts EXACTLY ONE offender, named. Neither synthetic list touches the real enum.
//       A future edit that softens the predicate into a tautology reddens HERE, in the same
//       run, without anybody having to remember to mutate `StratSoundCues.h`.
// ---------------------------------------------------------------------------

namespace StratSoundCueClauses
{
	/** One reflected enumerator, reduced to the two facts the walk below decides on. */
	struct FCueEntry
	{
		FString Name;
		int64   Value = 0;
	};

	/**
	 * THE WALK, AS A FREE FUNCTION OVER ITS INPUT, so the clause can run it over a MUTANT.
	 *
	 * Extracted for exactly one reason: a clause that can only be exercised against the real
	 * enum can only be falsified by editing `StratSoundCues.h`, which is production code and
	 * not this lane's to touch even temporarily. With the decision separated from where the
	 * entries came from, CONTROL 4 hands it a hand-built enum with a cue after the sentinel and
	 * asserts it reddens -- a real mutation, of the input rather than of the tree.
	 *
	 * @param Entries           every entry the reflected enum reports, in index order.
	 * @param Bound             `(int64)EStratSoundCue::Count` when walking the real enum.
	 * @param OutChecked        how many entries were bound-checked rather than excluded.
	 * @param OutExcluded       how many were excluded, and
	 * @param OutExcludedNames  which -- both reported so a filter that ate a real cue is seen.
	 * @return the offenders, `Name=Value`, comma separated; empty when there are none.
	 */
	FString CollectCueBoundOffenders(const TArray<FCueEntry>& Entries,
	                                 const int64 Bound,
	                                 int32& OutChecked,
	                                 int32& OutExcluded,
	                                 FString& OutExcludedNames)
	{
		OutChecked  = 0;
		OutExcluded = 0;
		OutExcludedNames.Reset();

		FString Offenders;

		for (const FCueEntry& Entry : Entries)
		{
			// THE SENTINEL ITSELF, BY VALUE AND NOT BY NAME. Nothing here matches the string
			// "Count"; a renamed sentinel needs no edit in this file.
			const bool bIsTheSentinel = Entry.Value == Bound;

			// UHT'S GENERATED `_MAX`, WHICH MUST BE A NAME RULE -- its value is one past the
			// largest declared enumerator and so is outside the bound by construction.
			// Case-sensitive: `FString` comparisons ignore case by default in this engine.
			const bool bIsGeneratedMax =
				Entry.Name.EndsWith(TEXT("_MAX"), ESearchCase::CaseSensitive);

			if (bIsTheSentinel || bIsGeneratedMax)
			{
				++OutExcluded;
				OutExcludedNames += (OutExcludedNames.IsEmpty() ? TEXT("") : TEXT(", "));
				OutExcludedNames += Entry.Name;
				continue;
			}

			++OutChecked;

			if (Entry.Value < 0 || Entry.Value >= Bound)
			{
				Offenders += (Offenders.IsEmpty() ? TEXT("") : TEXT(", "));
				Offenders += FString::Printf(TEXT("%s=%lld"), *Entry.Name, Entry.Value);
			}
		}

		return Offenders;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundCueNoneAfterCountTest,
	"Stratocracy.StratUI.GATE-AUDIO.NoSoundCueIsDeclaredAfterTheCountSentinel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundCueNoneAfterCountTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSoundCueClauses;

	const UEnum* const CueEnum = StaticEnum<EStratSoundCue>();
	if (!TestNotNull(TEXT("CONTROL 1a: EStratSoundCue is reflected and StaticEnum resolves it"),
			CueEnum))
	{
		return false;
	}

	// THE BOUND, READ FROM THE MODULE. This is the identical expression `StratSoundCues.cpp`
	// sizes `bEmitted` with, so it is one past the largest index that array can hold.
	const int64 Bound = static_cast<int64>(EStratSoundCue::Count);

	// CONTROL 1b. The bound names a real enumerator, and it is the one this clause is about.
	// A `Count` that had been renamed or removed would surface here rather than as a vacuous
	// comparison below.
	const int32 BoundIndex = CueEnum->GetIndexByValue(Bound);
	if (!TestTrue(*FString::Printf(
				TEXT("CONTROL 1b: the value this clause takes as its bound (%lld) is a declared "
				     "enumerator of EStratSoundCue"), Bound),
			BoundIndex != INDEX_NONE))
	{
		return false;
	}
	if (!TestEqual(TEXT("CONTROL 1c: and that enumerator is Count -- the one whose value "
	                    "StratSoundCues.cpp sizes bEmitted from. This is the only place the "
	                    "sentinel's NAME appears, and it is ASSERTED rather than matched on"),
			CueEnum->GetNameStringByIndex(BoundIndex), FString(TEXT("Count"))))
	{
		return false;
	}

	// ---- the walk, over the real enum -----------------------------------------------
	const int32 EntryCount = CueEnum->NumEnums();

	TArray<FCueEntry> Entries;
	Entries.Reserve(EntryCount);
	for (int32 Index = 0; Index < EntryCount; ++Index)
	{
		Entries.Add(FCueEntry{ CueEnum->GetNameStringByIndex(Index),
		                       CueEnum->GetValueByIndex(Index) });
	}


	int32   Checked  = 0;
	int32   Excluded = 0;
	FString ExcludedNames;
	const FString Offenders =
		CollectCueBoundOffenders(Entries, Bound, Checked, Excluded, ExcludedNames);

	// CONTROL 2. The walk saw the enum, and it excluded exactly the two entries it should.
	TestEqual(*FString::Printf(
			TEXT("CONTROL 2a: exactly two entries were excluded -- the Count sentinel and UHT's "
			     "generated _MAX (excluded: '%s'). If this is 1, one of the two stopped being "
			     "emitted and the bound check below silently changed scope; if it is more than "
			     "2, the skip rules are eating real enumerators and this clause is covering "
			     "less than it reports"),
			*ExcludedNames),
		Excluded, 2);

	if (!TestEqual(TEXT("CONTROL 2b: every entry that was not excluded was bound-checked"),
			Checked, EntryCount - Excluded))
	{
		return false;
	}
	if (!TestTrue(*FString::Printf(
				TEXT("CONTROL 2c: the walk bound-checked more than one enumerator (%d), so a "
				     "green result below is not the answer an empty walk would give"),
				Checked),
			Checked > 1))
	{
		return false;
	}
	TestEqual(*FString::Printf(
			TEXT("CONTROL 2d: the walk bound-checked exactly Count (%lld) enumerators. That is "
			     "the module's own statement of how many real cues there are, and it is a "
			     "SECOND detector of the same mutant: a cue declared after Count makes this "
			     "read Count + 1 even before the claim below fires"),
			Bound),
		static_cast<int64>(Checked), Bound);

	// CONTROL 3. The predicate the loop applied can actually fail, exercised on the exact value
	// an enumerator declared after `Count` would take.
	{
		const int64 OnePastTheBound = Bound + 1;
		if (!TestTrue(*FString::Printf(
					TEXT("CONTROL 3: the bound predicate REJECTS %lld, the value an enumerator "
					     "declared after Count would carry and the first index outside "
					     "bEmitted. Without this the check below could be vacuously true"),
					OnePastTheBound),
				!(OnePastTheBound >= 0 && OnePastTheBound < Bound)))
		{
			return false;
		}
	}

	// CONTROL 4. THE WHOLE WALK, RUN OVER A MUTANT. Nothing below touches the real enum: both
	// lists are built here. The first is shaped like a healthy enum and must be clean; the
	// second is the same enum with one cue declared AFTER the sentinel -- the defect this
	// clause exists for -- and must name exactly that cue. This is what stops a future
	// softening of `CollectCueBoundOffenders` from passing quietly.
	{
		const int64 SyntheticBound = 3;

		int32   SyntheticChecked  = 0;
		int32   SyntheticExcluded = 0;
		FString SyntheticExcludedNames;

		const TArray<FCueEntry> Healthy = {
			FCueEntry{ TEXT("Alpha"),          0 },
			FCueEntry{ TEXT("Beta"),           1 },
			FCueEntry{ TEXT("Gamma"),          2 },
			FCueEntry{ TEXT("Count"),          SyntheticBound },
			FCueEntry{ TEXT("ESynthetic_MAX"), SyntheticBound + 1 },
		};

		const FString HealthyOffenders = CollectCueBoundOffenders(
			Healthy, SyntheticBound, SyntheticChecked, SyntheticExcluded,
			SyntheticExcludedNames);

		if (!TestEqual(*FString::Printf(
					TEXT("CONTROL 4a: over a synthetic enum shaped like a HEALTHY one, the walk "
					     "reports no offenders (read: '%s'). A predicate that rejected "
					     "everything would redden here, and its green below would mean nothing"),
					HealthyOffenders.IsEmpty() ? TEXT("none") : *HealthyOffenders),
				HealthyOffenders, FString()))
		{
			return false;
		}
		if (!TestEqual(TEXT("CONTROL 4b: and it excluded exactly the sentinel and the _MAX"),
				SyntheticExcluded, 2))
		{
			return false;
		}
		if (!TestEqual(TEXT("CONTROL 4c: and bound-checked the three real entries"),
				SyntheticChecked, 3))
		{
			return false;
		}

		const TArray<FCueEntry> Mutant = {
			FCueEntry{ TEXT("Alpha"),              0 },
			FCueEntry{ TEXT("Beta"),               1 },
			FCueEntry{ TEXT("Gamma"),              2 },
			FCueEntry{ TEXT("Count"),              SyntheticBound },
			FCueEntry{ TEXT("DeclaredAfterCount"), SyntheticBound + 1 },
			FCueEntry{ TEXT("ESynthetic_MAX"),     SyntheticBound + 2 },
		};

		const FString MutantOffenders = CollectCueBoundOffenders(
			Mutant, SyntheticBound, SyntheticChecked, SyntheticExcluded,
			SyntheticExcludedNames);

		if (!TestEqual(
				TEXT("CONTROL 4d: THE MUTANT. The same walk over the same enum with one cue "
				     "declared AFTER the sentinel names exactly that cue as an offender. If "
				     "this line ever reads 'none', this clause has stopped asking anything and "
				     "its green result below is worthless -- that, and not a red enum, is the "
				     "failure mode this control exists for"),
				MutantOffenders, FString(TEXT("DeclaredAfterCount=4"))))
		{
			return false;
		}
	}

	// ---- the claim ------------------------------------------------------------------
	TestTrue(*FString::Printf(
			TEXT("GATE-AUDIO: every declared EStratSoundCue enumerator has a value within "
			     "[0, %lld) -- the range StratDecideSoundCues' one-per-kind array bEmitted "
			     "covers, since it is sized (int32)EStratSoundCue::Count and Emit indexes it "
			     "with (int32)Cue. Outside that range: %s. A cue DECLARED AFTER Count makes "
			     "Emit write past the end of a stack array; the two static_asserts in "
			     "StratSoundCues.h cannot see that case, and this clause is the only thing that "
			     "sees it without the cue having to fire. Adding a cue is fine -- add it ABOVE "
			     "MatchEnded, which is where the header tells you to"),
			Bound, Offenders.IsEmpty() ? TEXT("none") : *Offenders),
		Offenders.IsEmpty());

	return true;
}
