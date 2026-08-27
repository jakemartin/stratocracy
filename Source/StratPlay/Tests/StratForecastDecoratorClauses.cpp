// The gate on `StratDecorateForecast` -- GDD §2.11.3's card at the DECORATION seam.
// T-UI-01 throughout, by the ruling of 2026-08-26 that W1 rides that ID, and the same ID
// `StratHoverInputClauses.cpp` in this directory already carries for wave 0's half.
//
// STRETCH DECLARED ONCE, HERE AND AGAIN ON EACH CLAUSE. T-UI-01's own sentence is about
// the forecast EQUALLING the resolution, and not one clause in this file asserts that
// equality -- `Source/StratBridge/Tests/StratForecastCardParity.cpp` does, against
// `strat::uiResolveForGate`. What is pinned here is the three things the decorator
// decides that are nobody else's: WHEN there is a card, WHICH hex it is about, and that
// the model's field is a complete statement on every path. Each is a property the card
// needs; none is a claim T-UI-01's sentence makes.
//
// NOTHING HERE CONSTRUCTS A BRIDGE, A WORLD OR AN ACTOR. `IStratForecastQuery` is an
// interface for exactly this reason -- its own declaration says a gate must be able to
// drive the decorator over positions the one shipped scenario does not contain. The
// double below is a RECORDER as much as an answerer: what the decorator ASKED is the
// subject of two of the three clauses, and a production query would swallow the question
// and hand back only an answer.
//
// WHERE THE EXPECTATIONS COME FROM. Every "should be empty" is a field-by-field
// comparison against a default-constructed `FStratForecastView`, never against typed
// zeroes. Every "should be about hex X" is compared against the hex handed to
// `FStratHoverState::SetHoveredHex` -- the production hover struct, driven, and not a
// value written straight into the model. And the act gate's clause compares against
// nothing at all on the numeric side: what it asserts is that the QUERY WAS NOT ASKED,
// which is the only way to tell the decorator's own gate from the rules module's answer.
//
// AN OFFSET PAIR AND AN AXIAL PAIR PRINT IDENTICALLY on even rows. The hexes below are
// chosen with ODD rows and with distinct X AND Y, so a coordinate confusion in either
// direction is visible in the failure text rather than hidden by a coincidence.
//
// WHAT THIS FILE DOES NOT PIN:
//   - Any number on the card. The decorator computes none; it composes.
//   - `AStratPlayerController::DecorateForPresentation`'s call site. The controller's own
//     ordering is asserted in `StratHoverInputClauses.cpp`'s controller-route clause for
//     the hover; what this file pins is that the ordering CONSTRAINT is real, which is
//     the fact that makes the call site's placement load-bearing rather than arbitrary.
//   - The widget, the asset and anything drawn.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Math/IntPoint.h"

#include "StratForecastQuery.h"
#include "StratHoverState.h"
#include "StratViewModel.h"

#include "StratBridge.h"

namespace StratForecastDecoratorClauses
{
	/** The selected attacker's id in every fixture below. Arbitrary and distinct from
	 *  every other id used here, so a decorator that picked the wrong unit is visible. */
	static const int32 kAttackerId = 7;

	/** A second unit the model lists and nothing selects. */
	static const int32 kBystanderId = 9;

	/** Two hexes with ODD rows and distinct components -- see the header block on why. */
	static const FIntPoint kHexA{2, 7};
	static const FIntPoint kHexB{4, 5};

	/**
	 * The query double: it ANSWERS, and it RECORDS what it was asked.
	 *
	 * `mutable` on the recording members because `ForecastAgainst` is `const` on the
	 * interface, and it is const for a good reason -- the production implementation
	 * borrows a bridge and caches nothing. A double that forced the interface to widen
	 * would be a test changing the code under test.
	 */
	struct FRecordingForecastQuery final : public IStratForecastQuery
	{
		/** What to hand back when `bAnswer`. */
		FStratAttackForecast Answer;

		/** false makes the query REFUSE -- the "no bridge" channel, not the illegal one. */
		bool    bAnswer = true;
		FString Refusal = TEXT("the double refused");

		mutable int32             Calls = 0;
		mutable TArray<int32>     AskedAttackers;
		mutable TArray<FIntPoint> AskedHexes;

		virtual bool ForecastAgainst(int32 AttackerId, FIntPoint DefenderHex,
		                             FStratAttackForecast& OutForecast,
		                             FString&              OutFailureReason) const override
		{
			++Calls;
			AskedAttackers.Add(AttackerId);
			AskedHexes.Add(DefenderHex);

			if (!bAnswer)
			{
				OutFailureReason = Refusal;
				return false;
			}
			OutForecast = Answer;
			return true;
		}
	};

	/** A legal card the double can hand back. No number here is asserted anywhere. */
	static FStratAttackForecast LegalAnswer()
	{
		FStratAttackForecast Card;
		Card.bLegal            = true;
		Card.Distance          = 1;
		Card.Damage            = 4;
		Card.bDefenderDies     = false;
		Card.bCounterFires     = true;
		Card.CounterDamage     = 2;
		Card.AttackerUnitId    = kAttackerId;
		Card.DefenderUnitId    = 31;
		Card.AttackerHpBefore  = 10;
		Card.DefenderHpBefore  = 10;
		Card.AttackerHpAfter   = 8;
		Card.DefenderHpAfter   = 6;
		Card.AttackerSide      = 0;
		Card.DefenderSide      = 1;
		Card.DefenderTerrainId = FName(TEXT("some-row"));
		return Card;
	}

	/** A model with two units on it, the attacker unspent. */
	static FStratViewModel ModelWithUnits()
	{
		FStratViewModel Model;

		FStratUnitView Attacker;
		Attacker.UnitId    = kAttackerId;
		Attacker.Side      = 0;
		Attacker.Hp        = 10;
		Attacker.HpMax     = 10;
		Attacker.bHasActed = false;
		Model.Units.Add(Attacker);

		FStratUnitView Bystander;
		Bystander.UnitId    = kBystanderId;
		Bystander.Side      = 1;
		Bystander.Hp        = 10;
		Bystander.HpMax     = 10;
		Bystander.bHasActed = false;
		Model.Units.Add(Bystander);

		return Model;
	}

	/**
	 * A card on the MODEL that is loud on every field.
	 *
	 * THIS IS THE WHOLE INSTRUMENT for the unconditional-write clause, and the reason is
	 * `a-default-read-cannot-tell-silence-from-an-empty-call`: an unconditional writer
	 * called with a default payload reads EXACTLY like a writer that was never called, if
	 * the field it writes started at its default. Starting it loud makes the two
	 * distinguishable -- a decorator that returned early without writing leaves this
	 * standing, and every field of it is different from the default it must be replaced by.
	 */
	static FStratForecastView LoudStandingCard()
	{
		FStratForecastView View;
		View.bHasForecast              = true;
		View.AttackerUnitId            = 4242;
		View.DefenderUnitId            = 4343;
		View.Damage                    = 99;
		View.DefenderHpBefore          = 98;
		View.DefenderHpAfter           = 97;
		View.bCounterFires             = true;
		View.CounterDamage             = 96;
		View.CounterReason             = EStratCounterReason::DefenderDestroyed;
		View.AttackerHpBefore          = 95;
		View.AttackerHpAfter           = 94;
		View.bShowAttackerHp           = true;
		View.DefenderTerrainDefensePct = 93;
		View.DefenderTerrainId         = FName(TEXT("a-row-no-table-holds"));
		View.bDefenderDies             = true;
		View.KillAwardFame             = 92;
		View.bFlagAtRisk               = true;
		View.RiskedFlagSide            = 91;
		View.Distance                  = 90;
		return View;
	}

	static bool IsFieldwiseEqual(const FStratForecastView& A, const FStratForecastView& B)
	{
		return A.bHasForecast              == B.bHasForecast              &&
		       A.AttackerUnitId            == B.AttackerUnitId            &&
		       A.DefenderUnitId            == B.DefenderUnitId            &&
		       A.Damage                    == B.Damage                    &&
		       A.DefenderHpBefore          == B.DefenderHpBefore          &&
		       A.DefenderHpAfter           == B.DefenderHpAfter           &&
		       A.bCounterFires             == B.bCounterFires             &&
		       A.CounterDamage             == B.CounterDamage             &&
		       A.CounterReason             == B.CounterReason             &&
		       A.AttackerHpBefore          == B.AttackerHpBefore          &&
		       A.AttackerHpAfter           == B.AttackerHpAfter           &&
		       A.bShowAttackerHp           == B.bShowAttackerHp           &&
		       A.DefenderTerrainDefensePct == B.DefenderTerrainDefensePct &&
		       A.DefenderTerrainId         == B.DefenderTerrainId         &&
		       A.bDefenderDies             == B.bDefenderDies             &&
		       A.KillAwardFame             == B.KillAwardFame             &&
		       A.bFlagAtRisk               == B.bFlagAtRisk               &&
		       A.RiskedFlagSide            == B.RiskedFlagSide            &&
		       A.Distance                  == B.Distance;
	}
}

// ---------------------------------------------------------------------------
// T-UI-01 -- the decorator writes the card on EVERY path, including every path that
// produces no card.
//
// STRETCH DECLARED: this is `StratDecorateForecast`'s own unconditional-write contract,
// which §2.11.3 needs and which T-UI-01's sentence does not state.
//
// WHAT WOULD GO WRONG WITHOUT IT, in the declaration's own words: a decorator that wrote
// only on the card path "would leave a previous frame's forecast standing in any model
// built by a caller that never hovered", and "the card would hang on screen over a hex
// the cursor left, quoting an attacker that is no longer selected".
//
// HOW THIS CLAUSE CAN TELL A WRITE FROM A SILENCE, which is the trap
// `a-default-read-cannot-tell-silence-from-an-empty-call` records: the model's
// `Forecast` is PRE-POPULATED with `LoudStandingCard()` before every call. A decorator
// that returned early without writing leaves that card standing, and every one of its
// nineteen fields differs from the default the contract requires. A fixture that started
// the field at its default would report green over a decorator that had been deleted.
//
// FIVE PATHS, WHICH IS THE DECLARATION'S FOUR PLUS THE REFUSAL. The declaration names
// four no-card paths; the query's refusal channel is a fifth, and the declaration
// promises the clearing covers it too ("the card is cleared unconditionally inside the
// decorator, so a refusal leaves no stale forecast to report").
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratForecastDecoratorWritesUnconditionallyTest,
	"Stratocracy.StratPlay.T-UI-01.ForecastDecoratorWritesUnconditionally",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratForecastDecoratorWritesUnconditionallyTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratForecastDecoratorClauses;

	const FStratForecastView Default;

	// The instrument's own control, before it is used as evidence: the standing card must
	// actually differ from the default, or every assertion below is satisfied by nothing.
	if (!TestFalse(TEXT("control: the pre-populated standing card differs from a default one, so "
	                    "'cleared' and 'never written' are distinguishable"),
			IsFieldwiseEqual(LoudStandingCard(), Default)))
	{
		return false;
	}

	struct FPath
	{
		const TCHAR* What;
		int32        Selection;
		bool         bHovering;
		bool         bHasActed;
		bool         bLegal;
		bool         bQueryAnswers;
	};

	const FPath Paths[] =
	{
		{ TEXT("nothing is selected"),                INDEX_NONE,  true,  false, true,  true  },
		{ TEXT("the cursor is over no hex"),          kAttackerId, false, false, true,  true  },
		{ TEXT("the selected unit has already acted"),kAttackerId, true,  true,  true,  true  },
		{ TEXT("the rules module calls it illegal"),  kAttackerId, true,  false, false, true  },
		{ TEXT("the query refuses outright"),         kAttackerId, true,  false, true,  false },
	};

	for (const FPath& Path : Paths)
	{
		FStratViewModel Model = ModelWithUnits();
		Model.Units[0].bHasActed = Path.bHasActed;

		if (Path.bHovering)
		{
			FStratHoverState Hover;
			Hover.SetHoveredHex(kHexA);
			Hover.DecorateViewModel(Model);
		}

		// PRE-POPULATED. See the block above.
		Model.Forecast = LoudStandingCard();

		FRecordingForecastQuery Query;
		Query.Answer        = LegalAnswer();
		Query.Answer.bLegal = Path.bLegal;
		Query.bAnswer       = Path.bQueryAnswers;

		FString Reason;
		const bool bCard = StratDecorateForecast(Model, Path.Selection, Query, Reason);

		if (!TestFalse(*FString::Printf(TEXT("no card is produced when %s"), Path.What), bCard))
		{
			return false;
		}

		// ---- THE CLAUSE -----------------------------------------------------
		if (!IsFieldwiseEqual(Model.Forecast, Default))
		{
			AddError(FString::Printf(
				TEXT("T-UI-01: when %s, the model's Forecast was not cleared to a default card "
				     "(bHasForecast=%d attacker=%d damage=%d riskedSide=%d)"),
				Path.What, Model.Forecast.bHasForecast ? 1 : 0, Model.Forecast.AttackerUnitId,
				Model.Forecast.Damage, Model.Forecast.RiskedFlagSide));
			return false;
		}
	}

	// ---- and the positive path, so the clause is not satisfied by a decorator that
	// ---- clears the field and never writes a card at all -----------------------
	{
		FStratViewModel Model = ModelWithUnits();
		FStratHoverState Hover;
		Hover.SetHoveredHex(kHexA);
		Hover.DecorateViewModel(Model);
		Model.Forecast = LoudStandingCard();

		FRecordingForecastQuery Query;
		Query.Answer = LegalAnswer();

		FString Reason;
		const bool bCard = StratDecorateForecast(Model, kAttackerId, Query, Reason);

		TestTrue(TEXT("control: a selected, unspent unit over a legal target DOES produce a card"),
			bCard);
		TestTrue(TEXT("control: and the model says so"), Model.Forecast.bHasForecast);
		TestFalse(TEXT("control: which is not the standing card it replaced"),
			IsFieldwiseEqual(Model.Forecast, LoudStandingCard()));
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-01 -- the decorator reads `Model.Hover`, so running it before the hover composes a
// STALE hex.
//
// STRETCH DECLARED: this is the ordering constraint `StratDecorateForecast`'s
// declaration states, and §2.11.3 needs it because the card must be about the hex the
// cursor is on THIS frame. T-UI-01's sentence claims nothing about ordering.
//
// THE FAILURE THIS PINS IS INVISIBLE TO EVERY OTHER KIND OF TEST. A decorator run before
// the hover produces a perfectly well-formed card about last frame's hex. Every number on
// it is correct, every field is filled, nothing refuses and nothing logs. It reads on
// screen as a latency bug and it is a sequencing one. `FStratHoverState::DecorateViewModel`'s
// declaration was amended on wave 0's second gate to say its unorderedness "would stop
// being the whole story the day W1 or W2 added a decorator"; this clause is what makes
// that day's constraint checkable rather than merely written down.
//
// THE HOVER IS DRIVEN THROUGH THE PRODUCTION STRUCT and not written into the model by
// hand, so the clause pins the real seam. Two hexes with odd rows and distinct components
// -- an offset/axial confusion would be visible either way.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratForecastComposesTheHoveredHexTest,
	"Stratocracy.StratPlay.T-UI-01.ForecastComposesTheHoveredHex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratForecastComposesTheHoveredHexTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratForecastDecoratorClauses;

	FStratViewModel  Model = ModelWithUnits();
	FStratHoverState Hover;

	// ---- frame one: the cursor is on A, and the model has been told ----------
	Hover.SetHoveredHex(kHexA);
	Hover.DecorateViewModel(Model);

	FRecordingForecastQuery Query;
	Query.Answer = LegalAnswer();

	FString Reason;
	TestTrue(TEXT("control: the first frame produces a card"),
		StratDecorateForecast(Model, kAttackerId, Query, Reason));

	if (!TestEqual(TEXT("control: the query was asked exactly once"), Query.Calls, 1))
	{
		return false;
	}
	// COMPONENTWISE, because `TestEqual` has no `FIntPoint` overload and because a failure
	// that prints both components is the only kind an offset/axial confusion is legible in.
	TestEqual(TEXT("T-UI-01: and it was asked about the hovered hex's X"),
		Query.AskedHexes[0].X, kHexA.X);
	TestEqual(TEXT("T-UI-01: and about the hovered hex's Y"),
		Query.AskedHexes[0].Y, kHexA.Y);
	TestEqual(TEXT("T-UI-01: and about the selected attacker"),
		Query.AskedAttackers[0], kAttackerId);

	// ---- frame two: the cursor MOVES to B, and the hover has not decorated yet ----
	Hover.SetHoveredHex(kHexB);

	TestTrue(TEXT("control: the second frame still produces a card"),
		StratDecorateForecast(Model, kAttackerId, Query, Reason));
	if (!TestEqual(TEXT("control: the query has now been asked twice"), Query.Calls, 2))
	{
		return false;
	}

	// ---- THE CLAUSE, first direction: run BEFORE the hover, the hex is STALE ----
	// This is not the desired behaviour; it is the CONSEQUENCE that makes the ordering
	// constraint real. A decorator that read the hover from somewhere other than the
	// model would answer about B here, and the constraint recorded on three declarations
	// would be describing a dependency that does not exist.
	TestEqual(TEXT("T-UI-01: a decorator run BEFORE the hover decorator composes LAST frame's "
	               "hex (X) -- which is why the ordering constraint is real"),
		Query.AskedHexes[1].X, kHexA.X);
	TestEqual(TEXT("T-UI-01: and LAST frame's hex (Y)"),
		Query.AskedHexes[1].Y, kHexA.Y);

	// ---- THE CLAUSE, second direction: run AFTER the hover, the hex is current ----
	Hover.DecorateViewModel(Model);

	TestTrue(TEXT("control: the third call still produces a card"),
		StratDecorateForecast(Model, kAttackerId, Query, Reason));
	if (!TestEqual(TEXT("control: the query has now been asked three times"), Query.Calls, 3))
	{
		return false;
	}
	TestEqual(TEXT("T-UI-01: run AFTER the hover decorator, it composes THIS frame's hex (X)"),
		Query.AskedHexes[2].X, kHexB.X);
	TestEqual(TEXT("T-UI-01: and THIS frame's hex (Y)"),
		Query.AskedHexes[2].Y, kHexB.Y);

	// The instrument's own control: the two hexes really are different, so the two
	// assertions above cannot both be satisfied by one value.
	TestTrue(TEXT("control: the two hexes differ in BOTH components, so neither assertion "
	              "above can be satisfied by the other's value"),
		kHexA.X != kHexB.X && kHexA.Y != kHexB.Y);

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-01 -- a unit that has already acted gets no card, even where the rules module
// calls the attack legal.
//
// STRETCH DECLARED: §2.11.3's card is a COMMIT affordance, and a commit card for a
// command `FStratSelectionMachine::HandleEvent` will refuse is worse than no card. That
// is the decorator declaration's own reasoning; T-UI-01's sentence says nothing about the
// turn.
//
// THE DOUBLE ANSWERS `legal`, AND THAT IS THE POINT. `strat::uiForecast` does not consult
// `hasActed` -- it answers about §2.6 and not about §4.9's turn flags -- so an
// already-spent unit really does forecast a legal attack. If the decorator's gate were
// missing, this fixture would produce a card. The clause is therefore about a gate that
// exists nowhere else on this path.
//
// WHAT IS ASSERTED IS THAT THE QUERY WAS NOT ASKED, and that is the only way to tell the
// decorator's OWN gate from the rules module's answer. A clause that merely observed
// "no card" would stay green if the gate were deleted and the query happened to refuse
// for its own reasons -- which is exactly what a `TestTrue` probe on a lazily-armed
// subject does, and this project has paid for that shape before.
//
// THE BIT READ IS `bHasActed` AND NOT `bDone`, and the control below proves the
// distinction is live: a unit that is `bDone` with its act unspent STILL gets a card,
// because `bDone` is the engine's presentation bit and would hide cards for attacks §2.6
// permits.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratForecastRefusesASpentUnitTest,
	"Stratocracy.StratPlay.T-UI-01.ForecastRefusesASpentUnit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratForecastRefusesASpentUnitTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratForecastDecoratorClauses;

	// ---- the control FIRST: the same fixture with the act unspent DOES produce a card,
	// ---- so the refusal below is caused by the bit and by nothing else in the setup ----
	{
		FStratViewModel  Model = ModelWithUnits();
		FStratHoverState Hover;
		Hover.SetHoveredHex(kHexA);
		Hover.DecorateViewModel(Model);
		Model.Units[0].bHasActed = false;

		FRecordingForecastQuery Query;
		Query.Answer = LegalAnswer();

		FString Reason;
		TestTrue(TEXT("control: with the act unspent this exact fixture produces a card"),
			StratDecorateForecast(Model, kAttackerId, Query, Reason));
		TestEqual(TEXT("control: and the query was consulted"), Query.Calls, 1);
	}

	// ---- THE CLAUSE ---------------------------------------------------------
	{
		FStratViewModel  Model = ModelWithUnits();
		FStratHoverState Hover;
		Hover.SetHoveredHex(kHexA);
		Hover.DecorateViewModel(Model);
		Model.Units[0].bHasActed = true;

		FRecordingForecastQuery Query;
		Query.Answer = LegalAnswer();   // the module WOULD call this attack legal

		FString Reason;
		const bool bCard = StratDecorateForecast(Model, kAttackerId, Query, Reason);

		TestFalse(TEXT("T-UI-01: a unit that has already acted gets no card, though the rules "
		               "module calls the attack legal"), bCard);
		TestFalse(TEXT("T-UI-01: and the model says there is no card"),
			Model.Forecast.bHasForecast);
		TestEqual(TEXT("T-UI-01: and the query was never asked, so the gate is the decorator's "
		               "own and not the rules module's answer"), Query.Calls, 0);
	}

	// ---- `bDone` is NOT the bit: a done-but-unspent unit still gets a card ----
	{
		FStratViewModel  Model = ModelWithUnits();
		FStratHoverState Hover;
		Hover.SetHoveredHex(kHexA);
		Hover.DecorateViewModel(Model);
		Model.Units[0].bHasActed = false;
		Model.Units[0].bDone     = true;

		FRecordingForecastQuery Query;
		Query.Answer = LegalAnswer();

		FString Reason;
		TestTrue(TEXT("T-UI-01: `bDone` with the act unspent does NOT hide the card -- the gate "
		              "reads the rules-side bit and not the presentation one"),
			StratDecorateForecast(Model, kAttackerId, Query, Reason));
	}

	// ---- a selection naming a unit the model does not list gets no card -------
	{
		FStratViewModel  Model = ModelWithUnits();
		FStratHoverState Hover;
		Hover.SetHoveredHex(kHexA);
		Hover.DecorateViewModel(Model);

		FRecordingForecastQuery Query;
		Query.Answer = LegalAnswer();

		FString Reason;
		const bool bCard = StratDecorateForecast(Model, kAttackerId + 1000, Query, Reason);

		TestFalse(TEXT("T-UI-01: a selection naming no unit in the model produces no card"), bCard);
		TestEqual(TEXT("T-UI-01: and asks nothing"), Query.Calls, 0);
	}

	return true;
}
