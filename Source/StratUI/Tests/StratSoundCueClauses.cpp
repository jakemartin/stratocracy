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
// TRUE OF ITS OWN CONTENTS. `MatchEndedIsTheLastSoundCue` reads no view model at all. It is
// about the SHAPE `StratDecideSoundCues`'S ONE-PER-KIND GATE DEPENDS ON -- the fixed array
// `bEmitted`, whose bound is written as `(int32)EStratSoundCue::MatchEnded + 1` and which
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
// GATE-AUDIO -- THE DECIDER NEVER EMITS `ButtonClick` OR `MatchEnded`.
//
// BOTH ARMS ARE OWNED ELSEWHERE, EACH BY A LATCH, AND THE ABSENCE IS A STATEMENT RATHER THAN AN
// OMISSION.
//   - A CLICK IS NOT A CHANGE IN THE MODEL. A refused button changes nothing at all and must
//     still click, so no diff over two view models can ever see one. The six input verbs emit
//     it directly.
//   - A MATCH ENDING **IS** VISIBLE IN THE MODEL, AND IS STILL NOT TAKEN HERE. That is the arm
//     this clause exists for. `ConcludeMatchIfEnded` already owns a once-per-match latch and
//     sounds the cue inside it; a `bHasResult` arm in the decider would be a SECOND answer to
//     "has this match already ended", and a concluded match refreshes many times -- so the
//     wrong answer is not one extra sound, it is a victory sting on every mouse move.
//
// THE FIXTURE DRIVES A CONCLUDED MODEL SPECIFICALLY, because a decider with a `bHasResult` arm
// is invisible to every other clause in this file: none of them ever sets that flag. The flag
// is planted and the plant is checked through the module's own predicate rather than by reading
// the bool back.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundNeverClickOrMatchEndedTest,
	"Stratocracy.StratUI.GATE-AUDIO.TheDeciderNeverEmitsButtonClickOrMatchEnded",
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

	TArray<FStratViewModel> Diffs;
	Diffs.Add(Model);
	Diffs.Add(Concluded);

	TArray<FStratSoundEmission> Out;
	for (const FStratViewModel& Diff : Diffs)
	{
		StratDecideSoundCues(Mark, Diff, Out);

		AddInfo(FString::Printf(TEXT("a diff carrying bHasResult=%s produced: %s"),
			Diff.Match.bHasResult ? TEXT("true") : TEXT("false"), *Describe(Out)));

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
	}

	// THE CONTROL FOR THE WHOLE CLAUSE. The concluded diff must have produced SOMETHING, or the
	// two assertions above were satisfied by a decider that emitted nothing at all.
	if (!TestTrue(*FString::Printf(
			TEXT("CONTROL: the concluded diff really did move -- it emitted %s -- so the two "
			     "counts above are zero because those arms are absent, not because the decider "
			     "was quiet"),
			*Describe(Out)),
			Out.Num() > 0))
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
// GATE-AUDIO -- NO ENUMERATOR OF `EStratSoundCue` SITS OUTSIDE THE ONE-PER-KIND GATE'S ARRAY.
//
// THE DEFECT THIS EXISTS AGAINST IS A MEMORY WRITE AND NOT A STALE COMMENT.
// `StratDecideSoundCues` opens its one-per-kind gate as
//
//     bool bEmitted[static_cast<int32>(EStratSoundCue::MatchEnded) + 1] = {};
//
// and its `Emit` lambda then indexes that array with `static_cast<int32>(Cue)` -- unchecked,
// because the bound is meant to cover the enum by construction. It covers the enum only while
// `MatchEnded` holds the LARGEST value any enumerator has. APPEND ONE CUE AFTER `MatchEnded`
// AND THE FIRST TIME THE DECIDER EMITS IT, THE WRITE LANDS ONE PAST THE END OF THAT STACK
// ARRAY. Nothing else in this tree observes that: the build is green, every other clause in
// this file is green, and the corruption is silent and stack-local.
//
// THIS CLAUSE PINS A RELATIONSHIP AND DELIBERATELY NOT A COUNT, WHICH IS THE WHOLE OF ITS
// DESIGN. A clause asserting `NumEnums()` equals some number would go RED for the CORRECT edit
// -- somebody adding a legitimate cue in the middle, where the array grows with it and nothing
// is unsafe -- and would say nothing about the dangerous one, since the counts move
// identically. So the assertion is the exact safety condition of the indexing above and nothing
// more: EVERY declared enumerator's value lies within `[0, (int32)MatchEnded]`. Insert
// `Retreated` between `UnitMoved` and `UnitAttacked` and this clause stays green, correctly.
// Append it after `MatchEnded` and this clause is the only thing in the project that goes red.
//
// IT ALSO COVERS THE OTHER HALF OF THE SAME HAZARD, which "MatchEnded is last" understates: an
// enumerator given an EXPLICIT value -- `Ambient = 64` anywhere in the list, or a negative one
// -- indexes outside the array without ever being written after `MatchEnded` in source order.
// The bound check is over VALUES and therefore catches that too. The clause is named for the
// shape a reader will look for and asserts the shape the code actually needs.
//
// WHERE THE EXPECTATION COMES FROM. There is no number written in this clause. The bound is
// read from the module's own `EStratSoundCue::MatchEnded`, the enumerators are read from the
// module's own reflected `UEnum`, and the comparison is between the two.
//
// THE THREE CONTROLS, because a reflection walk that visits nothing passes every bound check
// ever written.
//   (1) THE INSTRUMENT SPEAKS. `StaticEnum<EStratSoundCue>()` resolves, and the enumerator this
//       clause takes its bound FROM is found in it by value and reports the name `MatchEnded`.
//       A reflection lookup that silently returned an empty enum fails here.
//   (2) THE WALK VISITED THE ENUM. The number of entries the walk actually bound-checked is
//       asserted against `NumEnums()` minus the entries it excluded, and asserted to be greater
//       than one, so a skip rule that filtered everything out cannot pass. UHT appends one
//       `<EnumName>_MAX` sentinel whose value is deliberately one past the last real
//       enumerator; it is EXCLUDED, and the fact that exactly one entry was excluded is itself
//       asserted rather than assumed -- if a future UHT stops emitting it, this control reddens
//       and names what it saw instead of the clause quietly mis-scoping itself.
//   (3) THE PREDICATE DISCRIMINATES. The same comparison the loop uses is exercised on a
//       synthetic value one past the bound and asserted to FAIL it. Without this, a predicate
//       accidentally written to accept everything would be green over the very append this
//       clause exists for, and no arrangement of real enumerators could tell.
//
// WHAT THIS CLAUSE IS NOT, STATED PLAINLY BECAUSE IT IS THE WEAKER OF TWO AVAILABLE FORMS.
// The strongest form of this pin is not a clause at all: a `static_assert` in `StratSoundCues.h`
// against a `Count` sentinel would make the dangerous append a COMPILE ERROR, at the site, with
// no suite run required, and would be impossible to skip. That is production code and this lane
// does not write it. This runtime clause is the best thing available from `Tests/`: it runs
// after the fact, in a suite somebody has to remember to run, and it reports the defect rather
// than preventing it. Recorded as a gap, not offered as an equal.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSoundCueMatchEndedIsLastTest,
	"Stratocracy.StratUI.GATE-AUDIO.MatchEndedIsTheLastSoundCue",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSoundCueMatchEndedIsLastTest::RunTest(const FString& /*Parameters*/)
{
	const UEnum* const CueEnum = StaticEnum<EStratSoundCue>();
	if (!TestNotNull(TEXT("CONTROL 1a: EStratSoundCue is reflected and StaticEnum resolves it"),
			CueEnum))
	{
		return false;
	}

	// THE BOUND, READ FROM THE MODULE. This is the identical expression `StratSoundCues.cpp`
	// sizes `bEmitted` with, minus the `+ 1` -- so it is the largest index the array can hold.
	const int64 Bound = static_cast<int64>(EStratSoundCue::MatchEnded);

	// CONTROL 1b. The bound names a real enumerator, and it is the one this clause is about.
	// A `MatchEnded` that had been renamed or removed would surface here rather than as a
	// vacuous comparison below.
	const int32 BoundIndex = CueEnum->GetIndexByValue(Bound);
	if (!TestTrue(*FString::Printf(
				TEXT("CONTROL 1b: the value this clause takes as its bound (%lld) is a declared "
				     "enumerator of EStratSoundCue"), Bound),
			BoundIndex != INDEX_NONE))
	{
		return false;
	}
	if (!TestEqual(TEXT("CONTROL 1c: and that enumerator is MatchEnded -- the one whose value "
	                    "StratSoundCues.cpp sizes bEmitted from"),
			CueEnum->GetNameStringByIndex(BoundIndex), FString(TEXT("MatchEnded"))))
	{
		return false;
	}

	// ---- the walk -------------------------------------------------------------------
	const int32 EntryCount = CueEnum->NumEnums();

	int32   Checked  = 0;
	int32   Excluded = 0;
	FString Offenders;
	FString ExcludedNames;

	for (int32 Index = 0; Index < EntryCount; ++Index)
	{
		const FString Name = CueEnum->GetNameStringByIndex(Index);

		// UHT'S OWN SENTINEL, AND ONLY IT. Matched case-sensitively on purpose: FString's
		// comparisons ignore case by default in this engine, and a real enumerator ending
		// `_max` must not be waved through by the rule that skips `_MAX`.
		if (Name.EndsWith(TEXT("_MAX"), ESearchCase::CaseSensitive))
		{
			++Excluded;
			ExcludedNames += (ExcludedNames.IsEmpty() ? TEXT("") : TEXT(", "));
			ExcludedNames += Name;
			continue;
		}

		++Checked;

		const int64 Value = CueEnum->GetValueByIndex(Index);
		if (Value < 0 || Value > Bound)
		{
			Offenders += (Offenders.IsEmpty() ? TEXT("") : TEXT(", "));
			Offenders += FString::Printf(TEXT("%s=%lld"), *Name, Value);
		}
	}

	// CONTROL 2. The walk saw the enum, and it excluded exactly the one sentinel.
	TestEqual(*FString::Printf(
			TEXT("CONTROL 2a: exactly one entry was excluded as UHT's generated sentinel "
			     "(excluded: '%s'). If this is 0, UHT stopped emitting it and the bound check "
			     "below silently changed scope; if it is more than 1, the skip rule is eating "
			     "real enumerators"),
			*ExcludedNames),
		Excluded, 1);

	if (!TestEqual(TEXT("CONTROL 2b: every entry that was not the sentinel was bound-checked"),
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

	// CONTROL 3. The predicate the loop applied can actually fail. Exercised on a synthetic
	// value one past the bound -- the exact value an appended cue would take.
	{
		const int64 OnePastTheBound = Bound + 1;
		if (!TestTrue(*FString::Printf(
					TEXT("CONTROL 3: the bound predicate REJECTS %lld, the value an enumerator "
					     "appended after MatchEnded would carry and the first index outside "
					     "bEmitted. Without this the check below could be vacuously true"),
					OnePastTheBound),
				!(OnePastTheBound >= 0 && OnePastTheBound <= Bound)))
		{
			return false;
		}
	}

	// ---- the claim ------------------------------------------------------------------
	TestTrue(*FString::Printf(
			TEXT("GATE-AUDIO: every declared EStratSoundCue enumerator has a value within "
			     "[0, %lld] -- the range StratDecideSoundCues' one-per-kind array bEmitted "
			     "covers, since it is sized (int32)MatchEnded + 1 and Emit indexes it with "
			     "(int32)Cue unchecked. Outside that range: %s. A cue APPENDED AFTER MatchEnded "
			     "makes Emit write one past the end of a stack array, silently, in a green "
			     "build. Adding a cue is fine -- add it BEFORE MatchEnded, or have the engineer "
			     "size the array off a Count sentinel, which is the stronger fix this clause "
			     "cannot make from Tests/"),
			Bound, Offenders.IsEmpty() ? TEXT("none") : *Offenders),
		Offenders.IsEmpty());

	return true;
}
