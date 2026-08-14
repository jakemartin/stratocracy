// The falsification gate on the divergence rule itself -- T-UI-01's agreement end,
// taken directly rather than through a submit. GDD §2.6, §4.11 row T-UI-01.
//
// WHAT WAS UNGATED BEFORE THIS FILE, AND WHY IT COULD NOT BE GATED ANY OTHER WAY.
// `StratCombatOutcomeParity.cpp`'s own header block states the hole in as many words:
// `agree=0`, `STRAT-COMBAT divergence`, and every non-zero arm of `DivergenceMask` are
// UNREACHABLE through `FStratBridge::Submit`. That is not a gap in the fixture. Phase 2's
// gate measured it against the vendored sources -- `strat::uiForecast` and
// `strat::applyCommand`'s Attack case reach the SAME `Combat.h::resolveDamage` and
// `Combat.h::defenderCanCounter` over the same stat blocks inside one call, so a
// forecast/resolution disagreement is not a state a caller can drive the module into.
// 74 fixture Attacks, and zero divergences over every resolution the suite has ever run.
//
// THE RESOLUTION COUNT IS RUN-SPECIFIC AND IS NOT A LIVE FIGURE. One measurement, kept
// only because it is the one this file was written against: 483 `STRAT-COMBAT resolved`
// lines in the combat-outcome milestone's PHASE 1 full-suite run. A later phase measured
// 493 in its own session log. The two do not conflict -- the total grows with the suite,
// so any number here describes the run it was taken from and nothing else. Do NOT read
// it as a current count, and do NOT gate on it; the invariant this file rests on is the
// ZERO, not the denominator. What the zero is worth is settled below.
//
// That zero is the correct reading, and it is also no evidence whatsoever that the
// detector works.
//
// A DETECTOR NOBODY HAS EVER SEEN FIRE IS INDISTINGUISHABLE FROM A DETECTOR THAT CANNOT
// FIRE. Every clause below exists because `DivergenceMask` is read by a gate -- a phase
// that greps `STRAT-COMBAT divergence` and finds none is entitled to conclude the rules
// agreed with themselves ONLY IF the line would have appeared had they not. This file is
// what buys that entitlement.
//
// WHY THIS IS NOT A FOIL. The obvious move -- a fake rules module that disagrees with
// itself, driven through the bridge -- was ruled WRONG and must not be attempted: it
// would pin the fake's behaviour and not the rule's. Instead `StratLossAgrees` and
// `StratDivergenceMaskOf` were promoted out of `StratBridge.cpp` into
// `StratCombatLog.h` (combat-outcome phase 3, by the gameplay engineer), and
// `CaptureAfter` now CALLS them rather than keeping a copy -- `StratBridge.cpp`'s
// "ONE CALL, AND NO SECOND COPY OF THE RULE" block. So the bytes exercised below are
// the same bytes the emitter runs. A clause here going red is the emitter going red.
//
// WHERE THE EXPECTATIONS COME FROM, since this file admittedly hand-builds its inputs:
//
//   - Every EXPECTED MASK is an `EStratCombatDivergence` enumerator read off the module
//     -- `EStratCombatDivergence::DefenderLoss`, not the literal `1`. If the bit values
//     are ever renumbered, these clauses follow rather than break, and a clause that
//     spelled `1` would have been asserting this file's memory of the header.
//   - Every INPUT is a hand-built `FStratCombatOutcome`, and this is unavoidable rather
//     than convenient: the whole finding above is that NO module-side value can produce
//     a divergent outcome. There is nothing to read. What the clauses assert is not the
//     numbers -- it is the RELATION the rule draws between them, and each input is
//     constructed to differ from an agreeing one along exactly one named axis.
//   - The MEASURABILITY PRECONDITION is `StratDivergenceMaskOf`'s own, quoted from its
//     doc comment: `bForecastQueried` true, `DefenderId` not `INDEX_NONE`,
//     `AttackerHpBefore` not `INDEX_NONE`. `CaptureAfter` checks it at
//     `StratBridge.cpp`'s "The agreement" block and returns early, leaving
//     `ForecastAgrees` at -1, rather than calling the mask function. Every outcome built
//     here satisfies it, and NO CLAUSE BELOW ASSERTS ON A RETURN OUTSIDE IT. The
//     function's doc says the value is "meaningless rather than wrong" there; a clause
//     pinning a meaningless value would freeze an accident into a requirement.
//
// THE CONTROL IS NOT OPTIONAL AND IT IS NOT DECORATION. `MaskIsNoneWhenEveryClauseHolds`
// is the positive control for an instrument whose normal reading is silence -- the
// project spent six rounds in phase 6 measuring an absence with nothing to prove the
// instrument could speak, and the standing rule since is that a silence clause ships
// beside a clause that makes the same instrument sound.
//
// TWO NOTES ON HOUSE TRAPS, both deliberately sidestepped rather than handled:
//   - NO `FString` COMPARISON ANYWHERE IN THIS FILE. `TestEqual` / `==` / `Contains` on
//     `FString` are CASE-INSENSITIVE in UE 5.8, and a clause whose teeth are spelling
//     needs `ESearchCase::CaseSensitive`. Every comparison here is `int32` or `bool`, so
//     the trap is absent by construction, not by discipline.
//   - NO `GLog` CAPTURE. A capture must override `CanBeUsedOnMultipleThreads()` or it
//     inherits the redirector's buffered queue tail (measured, 1-in-4 flake). These
//     clauses read a return value, so there is no log to capture and no flake to inherit.
//     This file seeds no board, loads no `UDataTable`, names no `strat::` type and does
//     no I/O -- it is the cheapest gate in the tree and the one most likely to still run
//     on a machine where the fixture assets are missing.

#include "Misc/AutomationTest.h"

#include "StratCombatLog.h"

namespace StratDivergenceRuleClauses
{
	/**
	 * An outcome on which EVERY clause of the rule holds, and which satisfies
	 * `StratDivergenceMaskOf`'s stated precondition.
	 *
	 * BOTH HALVES OF THE COMPARISON ARE NON-ZERO ON PURPOSE -- the defender loses 3 and
	 * the counter fires for 1. An "agreeing" baseline where nothing happened would be
	 * satisfied by a rule that returned `None` for every input, which is the degenerate
	 * pass the control exists to exclude.
	 *
	 * Each clause below copies this and moves exactly ONE axis, so the mask it asserts is
	 * attributable to that axis and to nothing else.
	 */
	static FStratCombatOutcome AgreeingOutcome()
	{
		FStratCombatOutcome O;

		// ---- precondition: measurable (see the file header) ----
		O.bForecastQueried = true;         // "a forecast was asked for and answered"
		O.DefenderId       = 4;            // "a unit stood on the target hex"
		O.AttackerHpBefore = 10;           // "the attacker's HP resolved off the snapshot"

		O.AttackerId       = 10;
		O.bForecastLegal   = true;

		// Defender: predicted to lose 3 and survive; measured 8 -> 5.
		O.ForecastDamage         = 3;
		O.bForecastDefenderDies  = false;
		O.DefenderHpBefore       = 8;
		O.DefenderHpAfter        = 5;
		O.bDefenderDied          = false;

		// Attacker: counter predicted to fire for 1; measured 10 -> 9.
		O.bForecastCounterFires  = true;
		O.ForecastCounterDamage  = 1;
		O.AttackerHpAfter        = 9;
		O.bAttackerDied          = false;

		return O;
	}
}

// ---------------------------------------------------------------------------
// The control: the instrument can be silent.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratDivergenceMaskIsNoneWhenEveryClauseHoldsTest,
	"Stratocracy.StratBridge.T-UI-01.DivergenceMaskIsNoneWhenEveryClauseHolds",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratDivergenceMaskIsNoneWhenEveryClauseHoldsTest::RunTest(const FString&)
{
	using namespace StratDivergenceRuleClauses;

	const FStratCombatOutcome Outcome = AgreeingOutcome();

	TestEqual(
		TEXT("An outcome where the defender lost exactly the forecast damage and the "
		     "attacker lost exactly the forecast counter damage carries no divergence bits"),
		StratDivergenceMaskOf(Outcome),
		static_cast<int32>(EStratCombatDivergence::None));

	// The emitter's own reading of that mask, asserted here rather than left implied:
	// `CaptureAfter` sets `ForecastAgrees` to 1 exactly when the mask is `None`
	// (StratBridge.cpp, "The agreement" block). Stated as the equivalence rather than
	// re-derived, so a reader knows which log field this clause is standing behind.
	TestTrue(
		TEXT("`None` is zero, which is what makes `agree=1` and `mask==None` the same "
		     "statement in the emitted line"),
		static_cast<int32>(EStratCombatDivergence::None) == 0);

	return true;
}

// ---------------------------------------------------------------------------
// The defender arm, isolated.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratDefenderHpOffByOneSetsOnlyDefenderLossTest,
	"Stratocracy.StratBridge.T-UI-01.DefenderHpOffByOneSetsOnlyDefenderLoss",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratDefenderHpOffByOneSetsOnlyDefenderLossTest::RunTest(const FString&)
{
	using namespace StratDivergenceRuleClauses;

	// ONE AXIS: the defender ends on 4 rather than 5, so it lost 4 against a forecast
	// of 3. Off by one and not by ten, because a large error would also be caught by a
	// rule that only checked the SIGN of the difference.
	FStratCombatOutcome Outcome = AgreeingOutcome();
	Outcome.DefenderHpAfter = 4;

	// Exact equality, not a bit test. `mask & DefenderLoss` would pass on a rule that
	// set every bit for every fault, which is the failure mode the bitmask exists to
	// avoid -- "the forecast was wrong" is not actionable.
	TestEqual(
		TEXT("A defender that lost one more HP than forecast sets DefenderLoss and no "
		     "other bit"),
		StratDivergenceMaskOf(Outcome),
		static_cast<int32>(EStratCombatDivergence::DefenderLoss));

	return true;
}

// ---------------------------------------------------------------------------
// The counter arm, isolated -- and its mirror, which is the clause that stops the
// detector from crying wolf on every attack that draws no counter.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAttackerLossWithNoForecastCounterSetsOnlyCounterLossTest,
	"Stratocracy.StratBridge.T-UI-01.AttackerLossWithNoForecastCounterSetsOnlyCounterLoss",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAttackerLossWithNoForecastCounterSetsOnlyCounterLossTest::RunTest(const FString&)
{
	using namespace StratDivergenceRuleClauses;

	// ONE AXIS: the forecast says no counter fires, and `ForecastCounterDamage` takes the
	// header's sentinel for "this field has no value here". The attacker nonetheless
	// arrives at 9 from 10. A counter that does not fire predicts a loss of ZERO -- not
	// "predicts nothing" -- so a real loss of 1 is a divergence.
	FStratCombatOutcome Outcome = AgreeingOutcome();
	Outcome.bForecastCounterFires = false;
	Outcome.ForecastCounterDamage = INDEX_NONE;

	TestEqual(
		TEXT("An attacker that lost HP when the forecast predicted no counter sets "
		     "CounterLoss and no other bit"),
		StratDivergenceMaskOf(Outcome),
		static_cast<int32>(EStratCombatDivergence::CounterLoss));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratZeroDamageCounterAgainstUnharmedAttackerIsNotDivergenceTest,
	"Stratocracy.StratBridge.T-UI-01.ZeroDamageCounterAgainstAnUnharmedAttackerIsNotDivergence",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratZeroDamageCounterAgainstUnharmedAttackerIsNotDivergenceTest::RunTest(const FString&)
{
	using namespace StratDivergenceRuleClauses;

	// THE MIRROR OF THE CLAUSE ABOVE, and the reason the rule folds both cases through
	// one expected-damage comparison instead of branching on `counterFires`. A counter
	// that FIRES for 0 and a counter that does not fire predict the same thing: the
	// attacker loses nothing. A rule that treated `counterFires == true` as "expect the
	// attacker to be hurt" would report a divergence on every such attack -- a false
	// positive in a channel whose whole value is that it is normally empty.
	FStratCombatOutcome Outcome = AgreeingOutcome();
	Outcome.bForecastCounterFires = true;
	Outcome.ForecastCounterDamage = 0;
	Outcome.AttackerHpAfter       = Outcome.AttackerHpBefore;

	TestEqual(
		TEXT("A counter that fires for zero damage against an attacker that lost no HP "
		     "is agreement, not divergence"),
		StratDivergenceMaskOf(Outcome),
		static_cast<int32>(EStratCombatDivergence::None));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCounterKillMatchingTheThresholdIsNotDivergenceTest,
	"Stratocracy.StratBridge.T-UI-01.CounterKillMatchingTheThresholdIsNotDivergence",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCounterKillMatchingTheThresholdIsNotDivergenceTest::RunTest(const FString&)
{
	using namespace StratDivergenceRuleClauses;

	// THE ARM WHERE NAIVE ARITHMETIC WOULD FIRE FALSELY, and the one place the rule
	// DERIVES an expectation the forecast does not carry. `strat::UiForecast` has a
	// `defenderDies` field and no attacker-dies field, so `StratDivergenceMaskOf` must
	// infer the attacker's death from `counterDamage >= AttackerHpBefore`. Meanwhile a
	// dead attacker has `AttackerHpAfter == INDEX_NONE` -- the header's sentinel, and
	// `CaptureAfter` sets it that way because a unit off the roster has no "after" HP.
	//
	// A rule that reached for `HpBefore - HpAfter` here would compute 10 - (-1) = 11
	// against an expected 10 and report a divergence on a perfectly correct counter-kill.
	// This is the arm `StratCombatOutcomeParity.cpp` reaches through a real submit at
	// prefix 20; asserted here directly so the two agree by construction.
	FStratCombatOutcome Outcome = AgreeingOutcome();
	Outcome.bForecastCounterFires = true;
	Outcome.ForecastCounterDamage = 10;          // exactly the attacker's HP: the threshold
	Outcome.AttackerHpAfter       = INDEX_NONE;  // no "after" HP for a unit off the roster
	Outcome.bAttackerDied         = true;

	TestEqual(
		TEXT("A counter whose damage exactly empties the attacker's HP pool, against an "
		     "attacker that died, is agreement -- the rule must not do arithmetic on the "
		     "-1 sentinel"),
		StratDivergenceMaskOf(Outcome),
		static_cast<int32>(EStratCombatDivergence::None));

	return true;
}

// ---------------------------------------------------------------------------
// The legality arm, isolated.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratForecastIllegalAloneSetsOnlyLegalityDisagreesTest,
	"Stratocracy.StratBridge.T-UI-01.ForecastIllegalAloneSetsOnlyLegalityDisagrees",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratForecastIllegalAloneSetsOnlyLegalityDisagreesTest::RunTest(const FString&)
{
	using namespace StratDivergenceRuleClauses;

	// ONE AXIS: every measured number still matches its forecast exactly. Only
	// `bForecastLegal` moves. That isolation is the property -- legality is INDEPENDENT
	// of damage, so an attack the forecast called illegal and that then applied cleanly
	// is still a fault, and it is still a fault a reader can attribute to legality alone.
	//
	// Note this is `bForecastLegal`, "the rules' answer", and NOT `bForecastQueried`,
	// "the query succeeded" -- the header keeps them separate and so does this clause.
	// `bForecastQueried` stays true here because it is part of the measurability
	// precondition; an outcome with it false is one `CaptureAfter` never hands to this
	// function at all.
	FStratCombatOutcome Outcome = AgreeingOutcome();
	Outcome.bForecastLegal = false;

	TestEqual(
		TEXT("A forecast-illegal attack whose damage numbers all matched sets "
		     "LegalityDisagrees and no other bit"),
		StratDivergenceMaskOf(Outcome),
		static_cast<int32>(EStratCombatDivergence::LegalityDisagrees));

	return true;
}

// ---------------------------------------------------------------------------
// The bitmask doing work a boolean could not.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratTwoSimultaneousFaultsSetBothBitsTest,
	"Stratocracy.StratBridge.T-UI-01.TwoSimultaneousFaultsSetBothBits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratTwoSimultaneousFaultsSetBothBitsTest::RunTest(const FString&)
{
	using namespace StratDivergenceRuleClauses;

	// TWO AXES AT ONCE, and that is the point. The header's rationale for a bitmask over
	// a boolean is that "the forecast was wrong" is not actionable while "the counter's
	// damage was wrong but the defender's was right" points at `defenderCanCounter` and
	// not at `resolveDamage`. That rationale is only true if the bits ACCUMULATE -- a
	// rule that returned on its first fault would report the defender fault and hide the
	// counter fault, and every reader would then chase the wrong function.
	FStratCombatOutcome Outcome = AgreeingOutcome();
	Outcome.DefenderHpAfter       = 4;           // lost 4 against a forecast 3
	Outcome.bForecastCounterFires = false;       // ... and no counter was predicted ...
	Outcome.ForecastCounterDamage = INDEX_NONE;
	// ... yet the attacker still arrives at 9 from 10 (inherited from the baseline).

	const int32 Mask = StratDivergenceMaskOf(Outcome);

	TestEqual(
		TEXT("An outcome faulting on both the defender and the counter carries both bits "
		     "and only those two"),
		Mask,
		static_cast<int32>(EStratCombatDivergence::DefenderLoss)
			| static_cast<int32>(EStratCombatDivergence::CounterLoss));

	// Spelled out separately so a failure MESSAGE names which half went missing, rather
	// than printing two integers and leaving the reader to decode the bits.
	TestTrue(TEXT("DefenderLoss survived the presence of a second fault"),
	         (Mask & static_cast<int32>(EStratCombatDivergence::DefenderLoss)) != 0);
	TestTrue(TEXT("CounterLoss survived the presence of a first fault"),
	         (Mask & static_cast<int32>(EStratCombatDivergence::CounterLoss)) != 0);

	return true;
}

// ---------------------------------------------------------------------------
// `StratLossAgrees` taken directly. The three clauses below are the ones the mask
// function delegates to twice, and each pins a decision the mask cannot express.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratLossAgreesOnDeathComparesThresholdNotExcessTest,
	"Stratocracy.StratBridge.T-UI-01.LossAgreesOnDeathComparesThresholdNotExcess",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratLossAgreesOnDeathComparesThresholdNotExcessTest::RunTest(const FString&)
{
	// A DEAD UNIT HAS NO "AFTER" HP, so death and damage cannot be checked by the same
	// comparison. The function's doc states the ruling: when both sides agree the unit
	// died, the only checkable thing is that the predicted blow was at least large enough
	// to empty the pool. The excess is not observable ANYWHERE -- the rules do not
	// record overkill and no snapshot carries it -- so asserting on it would be inventing
	// a clause and would turn every high-damage kill into a false divergence.

	TestTrue(
		TEXT("A predicted blow far larger than the pool still agrees with a death -- the "
		     "excess is unobservable and is not a fault"),
		StratLossAgrees(/*HpBefore*/ 4, /*HpAfter*/ INDEX_NONE, /*bDied*/ true,
		                /*ExpectedDamage*/ 99, /*bExpectDeath*/ true));

	TestTrue(
		TEXT("A predicted blow exactly equal to the pool agrees -- the comparison is >=, "
		     "and an exclusive > would make every exact lethal hit a divergence"),
		StratLossAgrees(4, INDEX_NONE, true, 4, true));

	TestFalse(
		TEXT("A predicted blow SMALLER than the pool disagrees with a death -- this is "
		     "the direction that carries the fault, and it is the only one"),
		StratLossAgrees(4, INDEX_NONE, true, 3, true));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratLossAgreesRejectsADeathFlagMismatchTest,
	"Stratocracy.StratBridge.T-UI-01.LossAgreesRejectsADeathFlagMismatch",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratLossAgreesRejectsADeathFlagMismatchTest::RunTest(const FString&)
{
	// THE FLAGS ARE COMPARED BEFORE ANY NUMBER IS, and both inputs below are chosen so
	// that the numeric path would have said "agrees". If the flag comparison were dropped
	// or reordered after the arithmetic, both of these would go green -- which is exactly
	// how "the forecast predicted a kill and the unit lived" would become invisible.

	TestFalse(
		TEXT("A survivor whose HP moved by exactly the predicted damage still disagrees "
		     "when the forecast predicted a death"),
		StratLossAgrees(/*HpBefore*/ 10, /*HpAfter*/ 7, /*bDied*/ false,
		                /*ExpectedDamage*/ 3, /*bExpectDeath*/ true));

	TestFalse(
		TEXT("A unit that died disagrees when the forecast predicted it would survive, "
		     "even though the predicted damage exceeded its pool"),
		StratLossAgrees(/*HpBefore*/ 2, /*HpAfter*/ INDEX_NONE, /*bDied*/ true,
		                /*ExpectedDamage*/ 3, /*bExpectDeath*/ false));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratLossAgreesRefusesArithmeticOnAnUnmeasurableSurvivorTest,
	"Stratocracy.StratBridge.T-UI-01.LossAgreesRefusesArithmeticOnAnUnmeasurableSurvivor",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratLossAgreesRefusesArithmeticOnAnUnmeasurableSurvivorTest::RunTest(const FString&)
{
	// `INDEX_NONE` MEANS "NO VALUE HERE", NOT "-1 HP", and this clause is built so that
	// the sentinel guard is the ONLY thing standing between it and a green.
	//
	// Both inputs are chosen adversarially: each expected-damage figure is precisely the
	// value the unguarded subtraction would produce. Delete the
	// `HpBefore == INDEX_NONE || HpAfter == INDEX_NONE` check and the arithmetic returns
	// TRUE for both -- a surviving unit whose HP could not be resolved would be silently
	// reported as agreeing with the forecast. A clause using round numbers here would
	// have passed with the guard deleted and pinned nothing.

	TestFalse(
		TEXT("A survivor with an unresolvable AFTER HP disagrees rather than computing "
		     "10 - (-1) == 11"),
		StratLossAgrees(/*HpBefore*/ 10, /*HpAfter*/ INDEX_NONE, /*bDied*/ false,
		                /*ExpectedDamage*/ 11, /*bExpectDeath*/ false));

	TestFalse(
		TEXT("A survivor with an unresolvable BEFORE HP disagrees rather than computing "
		     "(-1) - 9 == -10"),
		StratLossAgrees(/*HpBefore*/ INDEX_NONE, /*HpAfter*/ 9, /*bDied*/ false,
		                /*ExpectedDamage*/ -10, /*bExpectDeath*/ false));

	return true;
}
