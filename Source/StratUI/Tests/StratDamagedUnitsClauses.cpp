// T-INT-05 -- THE WORLD-FREE HALF OF THE DAMAGE ALERT: which units `StratDecideDamagedUnits`
// names, decided from two readings of `FStratViewModel` and from nothing else.
//
// WHY THE ID IS `T-INT-05` AND WHY THESE CLAUSES ARE NOT IN `StratSoundCueClauses.cpp`, WHICH
// IS THE FIRST QUESTION A READER OF THIS FILE WILL HAVE, BECAUSE THE FUNCTION UNDER TEST LIVES
// IN `StratSoundCues.cpp` AND EVERY OTHER CLAUSE OVER THAT FILE RIDES `GATE-AUDIO`. It is not
// `GATE-AUDIO`, and the reason is that name's OWN authorizing text: the 2026-09-05 ruling in
// `Tools/architect/state/global.md` records that `GATE-AUDIO` *"scopes itself to 'audio
// presentation reacting to the view model' -- cue selection and playback -- and was pinned to
// three named files precisely so it would not be read as a grant over the wider audio domain."*
// `StratDecideDamagedUnits` decides nothing audible. It answers which units a BOARD-SIDE VISUAL
// is applied to, and it shares a file with the audio decider only because it is a second reader
// of the same mark. Riding `GATE-AUDIO` would be a false clearance of an audio gate by a visual
// feature, which is the failure that ruling exists to prevent. NO ACCEPTANCE ID IS MINTED HERE
// AND NONE IS CLAIMED -- minting is `strat-data-steward`'s and a proposed identifier greps
// exactly like a defined one.
//
// `T-INT-05` IS THE ID THE OTHER HALF OF THIS SAME FEATURE ALREADY RIDES, and it already spans
// both modules: `Stratocracy.StratUI.T-INT-05.MirrorsSnapshotFieldForField`,
// `.OrdersArePreserved` and `.NamesResolveThroughTheTables` are all clauses over projections of
// `FStratViewModel` in this very directory, which is exactly what this function is a pure
// function OF. `AStratUnitActor`'s own header block was widened under `T-INT-05` for
// `DamageFlashElapsed` on 2026-09-06, and
// `Source/StratPlay/Tests/StratDamageFlashClauses.cpp` carries the actor-side half under the
// same name. One feature, one ID, two modules -- the shape `GATE-AUDIO` and `GATE-BUILDMENU`
// already take for spanning `StratPlay` and `StratUI`.
//
// THE FILE IS SEPARATE FROM `StratSoundCueClauses.cpp` FOR THE SAME REASON. That file's banner
// declares its own subject and its own ID in its first paragraph; appending a differently-named
// clause to it would make that declaration false.
//
// WHY THIS FUNCTION EXISTS AT ALL, WHICH IS WHAT `DamagedUnitsReportEveryFallenUnitAndNotOne`
// IS ABOUT. `StratDecideSoundCues` collapses to AT MOST ONE EMISSION PER CUE KIND -- its own
// `bool bEmitted[7]` -- so a hand-over damaging three units yields ONE `UnitAttacked`. For
// audio that is the specification and the anti-cacophony rule argues it at length. For a FLASH
// it would mean two of three damaged units silently do not flash, with a green build. The
// obvious simplification a reviewer would wave through -- "just reuse the `Cues` array" --
// therefore goes red at 1 vs 3, and that clause asserts BOTH numbers off BOTH deciders over the
// SAME two models so the contrast is the measurement rather than a claim about it.
//
// WHERE EVERY EXPECTATION COMES FROM. The base fixture is a real projection: an `FStratBridge`
// seeded from the shipped tables and the shipped scenario, projected through
// `StratBuildViewModel`. There is not one unit id, side, hex, HP figure or unit count written
// in this file. Every expected id is `FStratUnitView::UnitId` read off that model, and every
// mark is the module's own `StratSoundMarkFromView` over it.
//
// THE HP FALLS ARE PLANTS AND NEVER EXPECTATIONS, ON `StratSoundCueClauses.cpp`'S STATED
// PRECEDENT AND FOR ITS REASON. Nothing in this tree publishes "an HP value strictly below this
// one", so a fall must be constructed; each plant subtracts one from the model's own
// `FStratUnitView::Hp`, the resulting ORDER is asserted before it is relied on, and no clause
// below asserts an HP figure. `ARisingHpIsNotDamage` drives both directions off ONE ordered
// pair of readings, so no third number exists to disagree with.
//
// WHAT THESE CLAUSES DO NOT PIN. They say nothing about whether anything flashed: that needs an
// actor and lives in `Source/StratPlay/Tests/StratDamageFlashClauses.cpp`. They say nothing
// about the ORDER of the two statements in `UStratMatchSubsystem::ApplyView` -- the decide
// must precede the re-mark, and swapping them kills the feature silently; that is a call-site
// property and cannot be seen from this module at all. They say nothing about the tour's
// `Current.TargetId` flash. And they say nothing about WHY a unit's HP fell: the function names
// no attacker and labels no cause, by its own declaration, and neither do these.
//
// NO `strat::` FREE FUNCTION IS CALLED ANYWHERE BELOW. Seeding goes through `FStratBridge`,
// which is what this module is permitted; a `strat::` call here is `LNK2019`, measured 8x.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratSoundCues.h"
#include "StratViewModel.h"

// The bridge, included from a .cpp and never from a UHT-parsed header -- the same constraint
// `StratViewModel.h` records about itself. A test file is a .cpp, so it may say this.
#include "StratBridge.h"

namespace StratDamagedUnitsClauses
{
	/** The same harness pin every parity file in this tree carries, for the reason
	 *  `StratSoundCueClauses.cpp` states: no rule in this project decides which side moves
	 *  first, so this is a harness constant and not a rule. */
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
	 * `StratSoundCueClauses.cpp`'s precedent: a missing asset becomes a named failure inside
	 * the caller's own clause list instead of a crash in a helper.
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

	/** Every reported id, in the order the decider produced them, so a red run prints the
	 *  answer rather than only a count. */
	static FString Describe(const TArray<int32>& Ids)
	{
		if (Ids.Num() == 0)
		{
			return TEXT("<empty>");
		}
		TArray<FString> Parts;
		for (const int32 Id : Ids)
		{
			Parts.Add(FString::Printf(TEXT("%d"), Id));
		}
		return FString::Join(Parts, TEXT(", "));
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
}

// ---------------------------------------------------------------------------
// T-INT-05 -- EVERY DAMAGED UNIT IS REPORTED, NOT THE FIRST. THE MOST IMPORTANT CLAUSE IN THIS
// FILE.
//
// THE MUTANT IS A SIMPLIFICATION AND NOT A BUG, WHICH IS EXACTLY WHY IT NEEDS A CLAUSE. "Just
// filter the `Cues` array `StratDecideSoundCues` already produces for `UnitAttacked`" is
// shorter, deletes a whole function, and is the change a reviewer would most plausibly wave
// through as tidying. It is wrong for one reason and the reason is invisible at the call site:
// `Emit`'s `bool bEmitted[7]` collapses to at most one emission per cue KIND, so a hand-over
// damaging three units yields ONE `UnitAttacked`. Two of three damaged units would then
// silently not flash -- and a unit that does not flash is indistinguishable from a unit that
// was not hit, which is what most refreshes look like.
//
// BOTH NUMBERS ARE MEASURED OFF BOTH DECIDERS OVER THE SAME TWO MODELS. The clause does not
// merely assert 3; it asserts that the AUDIO decider gives exactly 1 over the identical inputs.
// That contrast is the whole subject -- it pins that the two share a PREDICATE and not an
// OUTPUT, which is the design `StratDecideDamagedUnits`' declaration argues for, and it means
// the clause reddens if either half ever drifts toward the other.
//
// THREE UNITS AND NOT TWO, because two is the smallest number that distinguishes "every" from
// "one" and three is the smallest that distinguishes "every" from "one" AND from "a pair". The
// three are the model's own first three units and the clause asserts their ids are distinct
// before relying on them; a scenario with fewer than three deployed units returns early rather
// than asserting over a shorter list.
//
// THE REPORTED IDS ARE COMPARED AS A SET AND ALSO COUNTED. A decider returning the right count
// of the wrong ids -- three copies of one id, or three ids off the mark instead of the model --
// is red on the membership assertions and not merely on the count.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratDamagedUnitsReportsEveryFallenUnitTest,
	"Stratocracy.StratUI.T-INT-05.DamagedUnitsReportEveryFallenUnitAndNotOne",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratDamagedUnitsReportsEveryFallenUnitTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDamagedUnitsClauses;

	FStratBridge    Bridge;
	FStratViewModel Healthy;
	FString         Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, Healthy, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: the scenario deploys at least three units"),
			Healthy.Units.Num() >= 3))
	{
		return false;
	}

	const int32 IdA = Healthy.Units[0].UnitId;
	const int32 IdB = Healthy.Units[1].UnitId;
	const int32 IdC = Healthy.Units[2].UnitId;
	if (!TestTrue(*FString::Printf(
				TEXT("CONTROL: the three units are three DISTINCT ids -- %d, %d, %d"),
				IdA, IdB, IdC),
			IdA != IdB && IdB != IdC && IdA != IdC))
	{
		return false;
	}

	// THE PLANT: THREE FALLS, ONE PER UNIT, EACH ONE BELOW THAT UNIT'S OWN READING. The order
	// is asserted before it is relied on, for `StratSoundCueClauses.cpp`'s reason: a unit
	// already at zero HP would make its own arm of this clause vacuous without saying so.
	FStratViewModel Hurt = Healthy;
	for (int32 Index = 0; Index < 3; ++Index)
	{
		Hurt.Units[Index].Hp = Healthy.Units[Index].Hp - 1;
		if (!TestTrue(*FString::Printf(
					TEXT("CONTROL: unit %d's planted reading is STRICTLY BELOW the model's own"),
					Healthy.Units[Index].UnitId),
				Hurt.Units[Index].Hp < Healthy.Units[Index].Hp))
		{
			return false;
		}
	}

	const FStratSoundMark Mark = StratSoundMarkFromView(Healthy);

	TArray<int32> Damaged;
	StratDecideDamagedUnits(Mark, Hurt, Damaged);

	if (!TestEqual(*FString::Printf(
				TEXT("T-INT-05: three units lost HP across the refresh and THREE are reported. "
				     "One is the answer a decider reusing the audio cue array gives, because "
				     "Emit collapses to one emission per cue KIND -- and two of three damaged "
				     "units would silently not flash. Reported: %s"),
				*Describe(Damaged)),
			Damaged.Num(), 3))
	{
		return false;
	}

	TestTrue(*FString::Printf(TEXT("T-INT-05: unit %d is among them (reported: %s)"),
			IdA, *Describe(Damaged)), Damaged.Contains(IdA));
	TestTrue(*FString::Printf(TEXT("T-INT-05: unit %d is among them (reported: %s)"),
			IdB, *Describe(Damaged)), Damaged.Contains(IdB));
	TestTrue(*FString::Printf(TEXT("T-INT-05: unit %d is among them (reported: %s)"),
			IdC, *Describe(Damaged)), Damaged.Contains(IdC));

	// THE CONTRAST, MEASURED RATHER THAN ASSERTED ABOUT. The same mark and the same model
	// through the AUDIO decider must give exactly ONE `UnitAttacked`. This is what makes the
	// three above a statement about a DESIGN DECISION -- the two deciders share a predicate and
	// not an output -- rather than a statement about a number.
	TArray<FStratSoundEmission> Cues;
	StratDecideSoundCues(Mark, Hurt, Cues);
	TestEqual(TEXT("T-INT-05: the AUDIO decider gives exactly ONE UnitAttacked over the very "
	               "same two models, which is its own anti-cacophony specification and is "
	               "precisely why the flash may not be derived from it"),
		CountOfCue(Cues, EStratSoundCue::UnitAttacked), 1);

	return true;
}

// ---------------------------------------------------------------------------
// T-INT-05 -- AN UNSEEDED MARK REPORTS NO DAMAGE.
//
// THE FIRST MODEL A MATCH EVER APPLIES HAS NO PREDECESSOR, so no unit's HP can be lower than it
// was. Without `bSeeded` the comparison runs against a default-constructed mark whose `Units`
// array is empty -- and a unit absent from the mark takes the `continue` arm, so the naive
// failure here is silence rather than noise. THAT IS WHY THIS CLAUSE ASSERTS A CONTROL FIRST:
// the same model against a SEEDED mark of a hurt roster reports damage, so "nothing reported"
// is demonstrably the seeding rule and not a decider that reports nothing ever.
//
// THE MARK IS DEFAULT-CONSTRUCTED AND NEVER HANDED TO `StratSoundMarkFromView`. That is the
// whole of the precondition: `FStratSoundMark::bSeeded` records THE WRITE and never the
// contents, because every field it carries has a legal value a real match also holds. This
// project already carries a measured defect of exactly that species -- a real default that
// could not signal unset.
//
// THE OUTPUT ARRAY IS PRE-FILLED WITH A VALUE THIS CLAUSE PUTS THERE, which is the other half.
// `StratDecideDamagedUnits` documents itself as emptying `OutUnitIds` on EVERY path including
// the unseeded one, and a caller handing the same array twice must not find the first call's
// answer still in it. A decider that returned early WITHOUT the `Reset()` would leave that
// value behind and is red here; against a freshly-constructed array it would be green.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratDamagedUnitsUnseededMarkTest,
	"Stratocracy.StratUI.T-INT-05.AnUnseededMarkReportsNoDamage",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratDamagedUnitsUnseededMarkTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDamagedUnitsClauses;

	FStratBridge    Bridge;
	FStratViewModel Healthy;
	FString         Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, Healthy, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: the scenario deploys units"), Healthy.Units.Num() > 0))
	{
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: the first unit has positive HP to fall from"),
			Healthy.Units[0].Hp > 0))
	{
		return false;
	}

	FStratViewModel Hurt = Healthy;
	Hurt.Units[0].Hp = Healthy.Units[0].Hp - 1;
	if (!TestTrue(TEXT("CONTROL: the planted reading is STRICTLY BELOW the model's own"),
			Hurt.Units[0].Hp < Healthy.Units[0].Hp))
	{
		return false;
	}

	// ---- the control: a SEEDED mark over the same two models does report ------------
	TArray<int32> Damaged;
	StratDecideDamagedUnits(StratSoundMarkFromView(Healthy), Hurt, Damaged);
	if (!TestEqual(*FString::Printf(
				TEXT("CONTROL: with a SEEDED mark this exact HP fall IS reported (%s) -- "
				     "without this, the silence below is the answer a deleted decider gives"),
				*Describe(Damaged)),
			Damaged.Num(), 1))
	{
		return false;
	}

	// ---- the property: an UNSEEDED mark, the same hurt model ------------------------
	// THE ARRAY STILL CARRIES THE CONTROL'S ANSWER, DELIBERATELY. See the block above: this is
	// what makes the `Reset()` on the unseeded path observable at all.
	if (!TestTrue(TEXT("CONTROL: the out-array is NOT empty going in"), Damaged.Num() > 0))
	{
		return false;
	}

	const FStratSoundMark Unseeded;
	if (!TestFalse(TEXT("CONTROL: a default-constructed mark reports itself unseeded"),
			Unseeded.bSeeded))
	{
		return false;
	}

	StratDecideDamagedUnits(Unseeded, Hurt, Damaged);

	TestEqual(*FString::Printf(
			TEXT("T-INT-05: the first model a match applies has no predecessor, so nothing can "
			     "have lost HP and nothing is reported -- and the out-array is EMPTIED on that "
			     "path rather than left carrying the previous refresh's answer. It holds: %s"),
			*Describe(Damaged)),
		Damaged.Num(), 0);

	return true;
}

// ---------------------------------------------------------------------------
// T-INT-05 -- ONLY A STRICTLY LOWER HP IS DAMAGE.
//
// WHY `<` AND NOT `!=`. HP falls under Sec 2.6 combat, RISES under Sec 2.7 repair, and arrives
// fresh at `hpMax` on a Sec 2.7 spawn -- so only the FALL has exactly one cause. A decider
// comparing `!=` would flash every damaged unit sitting on a town while it HEALED: a repair
// announcing itself as an attack, on the board, in red. `StratDecideSoundCues` makes exactly
// this comparison for `UnitAttacked` and `AnHpRiseIsNotAnAttack` pins it there; this clause
// pins the same boundary on the second reader of the same mark, because the two are separate
// lines of code and a relaxation of one is not caught by the other.
//
// THE TWO DIRECTIONS ARE DRIVEN OFF ONE ORDERED PAIR OF READINGS, which is what keeps the
// arithmetic out of the expectation side. One plant produces a value one below the model's own
// `FStratUnitView::Hp`; the clause asserts that order explicitly; then the FALL is
// (mark = high, model = low) and the RISE is (mark = low, model = high) over the very same two
// numbers. No HP figure is asserted anywhere below -- only which ids come back.
//
// THE FRESH-SPAWN ARM IS ASSERTED TOO AND IS A DIFFERENT LINE OF CODE. A unit present in the
// model and absent from the mark takes the `continue` arm rather than the comparison, and it is
// the arm a freshly built unit hits: it has no previous HP, so it cannot have lost any, and a
// newly delivered unit flashing red would be the wrong alert at exactly the right moment. The
// plant is a real unit REMOVED FROM A COPY of the mark's model, never an id this file invented.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratDamagedUnitsRisingHpTest,
	"Stratocracy.StratUI.T-INT-05.ARisingHpIsNotDamage",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratDamagedUnitsRisingHpTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratDamagedUnitsClauses;

	FStratBridge    Bridge;
	FStratViewModel Healthy;
	FString         Error;
	if (!TestTrue(TEXT("the bridge seeds and the model builds"),
			SeedAndProject(Bridge, Healthy, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: the scenario deploys units"), Healthy.Units.Num() > 0))
	{
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: the first unit has positive HP to fall from"),
			Healthy.Units[0].Hp > 0))
	{
		return false;
	}

	const int32 SubjectId = Healthy.Units[0].UnitId;

	FStratViewModel Hurt = Healthy;
	Hurt.Units[0].Hp = Healthy.Units[0].Hp - 1;
	if (!TestTrue(TEXT("CONTROL: the planted reading is STRICTLY BELOW the model's own"),
			Hurt.Units[0].Hp < Healthy.Units[0].Hp))
	{
		return false;
	}

	TArray<int32> Damaged;

	// ---- the FALL: healthy at the mark, hurt in the model ---------------------------
	StratDecideDamagedUnits(StratSoundMarkFromView(Healthy), Hurt, Damaged);
	if (!TestEqual(*FString::Printf(
				TEXT("T-INT-05: a unit whose HP FELL between two readings is damaged -- "
				     "reported: %s"), *Describe(Damaged)),
			Damaged.Num(), 1))
	{
		return false;
	}
	TestEqual(TEXT("T-INT-05: and it is the unit whose HP fell, named by the MODEL's own id -- "
	               "which is the key `UStratMatchSubsystem::FindUnitActor` reconciles on"),
		Damaged[0], SubjectId);

	// ---- the RISE: the same two readings, the other way round -----------------------
	StratDecideDamagedUnits(StratSoundMarkFromView(Hurt), Healthy, Damaged);
	TestEqual(*FString::Printf(
			TEXT("T-INT-05: and the SAME two readings in the other order are a Sec 2.7 repair, "
			     "which is not damage and flashes nothing -- the whole of why the comparison is "
			     "`<` and not `!=`. Reported: %s"), *Describe(Damaged)),
		Damaged.Num(), 0);

	// ---- the fresh spawn: present in the model, absent from the mark -----------------
	// A REAL UNIT REMOVED FROM A COPY, never an id this file composed. That is the state a
	// Sec 2.7 build leaves, and it reaches a DIFFERENT arm of the decider than the comparison
	// above -- the `continue` for an id the mark does not carry.
	FStratViewModel WithoutSubject = Healthy;
	WithoutSubject.Units.RemoveAt(0);
	if (!TestTrue(TEXT("CONTROL: the removal really dropped the subject from the mark's model"),
			WithoutSubject.Units.Num() == Healthy.Units.Num() - 1))
	{
		return false;
	}

	StratDecideDamagedUnits(StratSoundMarkFromView(WithoutSubject), Hurt, Damaged);
	TestFalse(*FString::Printf(
			TEXT("T-INT-05: a unit that is in the model and NOT in the mark was just built. It "
			     "has no previous HP, so it cannot have lost any, and it is not reported even "
			     "though its HP reads lower than another unit's. Reported: %s"),
			*Describe(Damaged)),
		Damaged.Contains(SubjectId));

	return true;
}
