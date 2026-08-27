// The gate on `StratComposeForecastView` -- GDD §2.11.3's card at the PRESENTATION end.
// T-UI-01 throughout, by the ruling of 2026-08-26 that W1 rides that ID.
//
// WHY THIS FILE LIVES IN StratUI/Tests, decided before it was written rather than
// discovered as a link error. `StratComposeForecastView` is declared in `StratUI`, so a
// `StratBridge` test could not call it -- StratBridge is BELOW StratUI and the arrow does
// not reverse. And the counter-reason clause below needs a REAL seeded bridge beside the
// compose, because the only honest witness for its `OutOfRange` arm is the defender's own
// range band, which lives on the rules-side projection. This directory is the only one
// that can see both, exactly as `StratViewModelParity.cpp`'s own header block sets out
// for the same reason: NAMING a vendored type costs no symbol, CALLING a vendored free
// function is LNK2019. Nothing below calls one. `Board::terrainAt` is not called here
// either -- it is an out-of-line member of a vendored struct and would link no better
// than a free function.
//
// WHAT EACH ID CLAIMS, AND WHERE THIS FILE STRETCHES IT. T-UI-01's own sentence is about
// the forecast EQUALLING the resolution. Not one clause below asserts that equality --
// that is `StratForecastCardParity.cpp`'s, in `Source/StratBridge/Tests/`, against
// `strat::uiResolveForGate`. These clauses pin the three PRESENTATION SHAPINGS
// `StratComposeForecastView` performs over the card, which §2.11.3 requires and which
// T-UI-01's sentence does not claim. They ride T-UI-01 because the forecast card is that
// ID's subject. Each clause says so again where it is declared, so that a reader who
// arrives at one clause is not left to infer the scope from this block.
//
// WHERE THE EXPECTATIONS COME FROM.
//
//   - The counter-reason clause takes EVERY input from a seeded `FStratBridge` driven
//     forward by §2.9's OWN OPPONENT AI -- `FStratBridge::NextAiCommand`, submitted as it
//     stands -- and its witness for each arm is a fact the mapping did
//     not consult: the defender's own `[rangeMin, rangeMax]` band off the projection, and
//     the bridge's own `DefenderHpAfter` clamp. `EStratCounterReason`'s declaration asks
//     for exactly this -- "a gate can assert the enumerator against the inputs it was
//     chosen from, AND against the defender's own range". Asserting `DefenderDestroyed`
//     because `bDefenderDies` is true would be a pin asserting a copy of its own subject,
//     and this project has four fixtures from one day that did precisely that.
//
//     THE POSITIONS ARE THE AI'S AND NOT THIS FILE'S, and the driver is the AI rather than
//     `Data/parity_fixture.save` for a LINKER reason that is worth stating: `strat::parseSave`
//     is a vendored free function, so a StratUI translation unit cannot read the fixture at
//     all. MEASURED, this pass: 1 x LNK2019 on `UnrealEditor-StratUI.dll` naming
//     `?parseSave@strat@@...`. `NextAiCommand` and `SubmitMoveToHex` / `SubmitAttackAtHex` /
//     `SubmitBuildAtHex` / `SubmitEndTurn` are all engine-typed `FStratBridge` methods, so
//     self-play reaches mid-match boards from this module with no vendored symbol called and
//     with every command chosen by §2.9 rather than by this file.
//
//   - The shaping clauses take HAND-BUILT `FStratAttackForecast` values, and that is not
//     a licence to choose numbers. Half of §2.11.3's card is unreachable on this
//     project's one scenario -- a zero-damage counter, a flag falling to a counter, two
//     flags lethal to each other -- and `StratComposeForecastView`'s declaration takes
//     the plain struct rather than the bridge FOR that reason. What is asserted over a
//     hand-built input is never a NUMBER; it is which of two enumerators was chosen, or
//     which side was named, or that a field is at its default. The one number that
//     appears -- the terrain percentage -- is read off the bridge's own loaded terrain
//     table and is not typed here.
//
//   - Every "should be empty" is compared against a default-constructed
//     `FStratForecastView`, field by field, and never against a typed zero.
//
// WHAT THIS FILE DOES NOT PIN:
//   - Any number the bridge produced. Damage, distance, the two clamps, the award and the
//     terrain percentage are all `StratForecastCardParity.cpp`'s.
//   - The decorator, the hover ordering and the act gate -- `StratPlay`'s, in
//     `StratForecastDecoratorClauses.cpp`.
//   - The widget. Nothing here constructs one; the compose is a free function precisely
//     so that no Slate tree need exist to see a value.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Math/IntPoint.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratViewModel.h"

// The bridge, for the counter-reason clause's real board and for the loaded terrain
// table. `StratViewModelParity.cpp` in this same directory already does this and links.
#include "StratBridge.h"

#include "Ui.h"

namespace StratForecastViewClauses
{
	static const int32 kFirstSide = 0;

	/** A ceiling on the self-play driver, NOT an expectation about how long a match runs.
	 *  `StratAiBridgeParity.cpp` carries the same kind of ceiling for the same reason. */
	static const int32 kMaxCommands = 400;

	/** How often the driver stops and sweeps every attacker/defender pair on the board. */
	static const int32 kSampleEvery = 8;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static FString FromStd(const std::string& In)
	{
		return FString(UTF8_TO_TCHAR(In.c_str()));
	}

	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		UDataTable* Units =
			LoadObject<UDataTable>(nullptr, TEXT("/Game/StratData/DT_Units.DT_Units"));
		UDataTable* Terrain =
			LoadObject<UDataTable>(nullptr, TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
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
		const FStratResult Seeded = Bridge.LoadScenarioFromFile(
			FPaths::Combine(DataDir(), TEXT("ferrum_crossing.json")), kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	/**
	 * One §2.9 command, submitted EXACTLY as `NextAiCommand` handed it back.
	 *
	 * `StratAiBridgeParity.cpp`'s `SubmitAsIs` in the letter and for its reason: nothing
	 * here consults `State()`, resolves a `TargetId` or substitutes a hex of its own. The
	 * positions this file sweeps are therefore the opponent AI's, and not this file's.
	 */
	static FStratResult SubmitAsIs(FStratBridge& Bridge, const FStratAiCommand& C)
	{
		switch (C.Kind)
		{
		case EStratAiCommandKind::Build:   return Bridge.SubmitBuildAtHex(C.Hex, C.DefIndex);
		case EStratAiCommandKind::Move:    return Bridge.SubmitMoveToHex(C.UnitId, C.Hex);
		case EStratAiCommandKind::Attack:  return Bridge.SubmitAttackAtHex(C.UnitId, C.Hex);
		case EStratAiCommandKind::EndTurn: return Bridge.SubmitEndTurn();
		}
		return FStratResult::Fail(TEXT("this test has no submission arm for that command kind"));
	}

	/** The unit standing on a hex, off a projection this file did not build. A plain loop
	 *  and not `strat::findUiUnit`, which would be LNK2019 from this module. */
	static const strat::UiUnit* UnitOnHex(const strat::UiWorld& World, const strat::Hex& Hex)
	{
		for (const strat::UiUnit& U : World.units)
		{
			if (U.hex.q == Hex.q && U.hex.r == Hex.r)
			{
				return &U;
			}
		}
		return nullptr;
	}

	/**
	 * A card with every field moved OFF its default, so that "the compose emptied it" and
	 * "the compose never touched it" are distinguishable.
	 *
	 * NOT A BOARD AND NOT A SET OF NUMBERS ANYTHING IS ASSERTED AGAINST. Nothing below
	 * compares a value from this function against a value derived from it; it exists so
	 * that a default-valued OUTPUT is evidence rather than a coincidence.
	 */
	static FStratAttackForecast LoudCard()
	{
		FStratAttackForecast Card;
		Card.bLegal                    = true;
		Card.IllegalReason             = TEXT("");
		Card.Distance                  = 3;
		Card.Damage                    = 7;
		Card.bDefenderDies             = false;
		Card.bCounterFires             = true;
		Card.CounterDamage             = 2;
		Card.AttackerUnitId            = 11;
		Card.DefenderUnitId            = 22;
		Card.AttackerHpBefore          = 9;
		Card.DefenderHpBefore          = 8;
		Card.AttackerHpAfter           = 7;
		Card.DefenderHpAfter           = 1;
		Card.bAttackerDies             = false;
		Card.bAttackerIsFlag           = false;
		Card.bDefenderIsFlag           = false;
		Card.AttackerSide              = 0;
		Card.DefenderSide              = 1;
		Card.DefenderTerrainDefensePct = 20;
		Card.DefenderTerrainId         = FName(TEXT("some-row"));
		Card.KillAwardFame             = 150;
		return Card;
	}

	/** Every field of `FStratForecastView`, compared. Used to assert emptiness against a
	 *  default-constructed instance rather than against typed zeroes. */
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
// T-UI-01 -- `EStratCounterReason`'s three arms, each witnessed by something the mapping
// did not consult.
//
// STRETCH DECLARED. T-UI-01's sentence is the forecast/resolution equality. This clause
// asserts no equality of that kind at all; it asserts a MAPPING §2.11.3 requires -- "a
// number, `out of range`, or `defender destroyed`" -- and it rides T-UI-01 because the
// card is that ID's subject.
//
// THE CIRCULARITY THIS CLAUSE EXISTS TO AVOID. `StratComposeForecastView` chooses the
// enumerator out of exactly two booleans, `bDefenderDies` and `bCounterFires`. A clause
// that asserted `DefenderDestroyed` wherever `bDefenderDies` is true would be asserting a
// copy of its own subject: it would stay green over a mapping rewritten to any other
// arrangement of the same two bits, and it would stay green over both of the lie-vectors
// `EStratCounterReason`'s own declaration names. So each arm is witnessed by a DIFFERENT
// fact:
//
//   `DefenderDestroyed`  <->  the bridge's own `DefenderHpAfter` is 0.
//   `Number`             ->   the defender survives AND the distance is INSIDE the
//                             defender's own `[rangeMin, rangeMax]` band.
//   `OutOfRange`         ->   the defender survives AND the distance is OUTSIDE it.
//
// THE THIRD IS THE ONE THAT MATTERS AND IT IS THE ONE THE BRIEF ASKED FOR: `OutOfRange`
// is asserted only where the range really is the reason. `Combat.h::defenderCanCounter`
// is "distance >= d.rangeMin && distance <= d.rangeMax" and nothing else, so the band read
// off the projection is an independent statement of the same fact -- and if a later
// revision gives that function a second refusal, THIS clause reddens, which is precisely
// the lie-vector the declaration says no numeric parity gate can see.
//
// THE BAND IS THE WOUNDED DEFENDER'S, as `uiForecast` uses it: that function copies the
// defender, sets `hp` to the post-damage value and asks `defenderCanCounter` about the
// copy. Only `hp` moves, and the band does not, so the band read off the pre-damage
// projection is the same band -- stated because it is the one place this clause's witness
// could quietly stop being the module's.
//
// THE POSITIVE CONTROL REQUIRES ALL THREE ARMS. A board on which only one arm ever occurs
// would satisfy every implication above vacuously.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCounterReasonMatchesTheDefendersBandTest,
	"Stratocracy.StratUI.T-UI-01.CounterReasonMatchesTheDefendersBand",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCounterReasonMatchesTheDefendersBandTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratForecastViewClauses;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge loads its definitions and seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	int32 NumberSeen     = 0;
	int32 OutOfRangeSeen = 0;
	int32 DestroyedSeen  = 0;
	int32 Sampled        = 0;
	int32 CommandsPlayed = 0;

	// The board is walked forward by §2.9's own AI, sampled every `kSampleEvery`
	// commands. The ceiling is a driver ceiling and NOT an expectation about how long a
	// match runs; a refusal ends the drive rather than being retried, because a driver
	// that pushed past a refusal would be authoring the board it then swept.
	for (int32 Step = 0; Step < kMaxCommands; ++Step)
	{
		if (Step % kSampleEvery == 0)
		{
			++Sampled;

			const strat::UiWorld World = Bridge.MakeUiWorld();
			for (const strat::UiUnit& A : World.units)
			{
				for (const strat::UiUnit& D : World.units)
				{
					FStratAttackForecast Card;
					if (!Bridge.AttackForecast(A.id, FIntPoint(D.hex.q, D.hex.r), Card).bOk ||
						!Card.bLegal)
					{
						continue;
					}

					FStratForecastView View;
					StratComposeForecastView(Card, View);

					const strat::UiUnit* const Def = UnitOnHex(World, D.hex);
					if (Def == nullptr)
					{
						AddError(TEXT("a legal forecast whose defender is not on the projection"));
						return false;
					}

					// THE WITNESS. The defender's own band, off the projection; the
					// distance the card carries for exactly this purpose (see
					// `FStratForecastView::Distance`, which is on the struct and NOT on
					// the screen).
					//
					// THE BAND IS THE WOUNDED DEFENDER'S, as `uiForecast` uses it: that
					// function copies the defender, sets `hp` to the post-damage value and
					// asks `defenderCanCounter` about the copy. Only `hp` moves and the
					// band does not, so the band read off the pre-damage projection is the
					// same band. Stated because it is the one place this witness could
					// quietly stop being the module's.
					const bool bInBand = View.Distance >= Def->unit.rangeMin &&
					                     View.Distance <= Def->unit.rangeMax;
					const bool bSurvived = Card.DefenderHpAfter > 0;

					switch (View.CounterReason)
					{
					case EStratCounterReason::DefenderDestroyed:
						++DestroyedSeen;
						if (bSurvived)
						{
							AddError(FString::Printf(
								TEXT("T-UI-01: the card says 'defender destroyed' about unit %d, "
								     "whose HP after this attack is %d"),
								Def->id, Card.DefenderHpAfter));
							return false;
						}
						break;

					case EStratCounterReason::Number:
						++NumberSeen;
						if (!bSurvived)
						{
							AddError(FString::Printf(
								TEXT("T-UI-01: the card shows a counter number for unit %d, which "
								     "does not survive to fire it"), Def->id));
							return false;
						}
						if (!bInBand)
						{
							AddError(FString::Printf(
								TEXT("T-UI-01: the card shows a counter number at distance %d, "
								     "outside unit %d's own band [%d, %d]"),
								View.Distance, Def->id, Def->unit.rangeMin, Def->unit.rangeMax));
							return false;
						}
						break;

					case EStratCounterReason::OutOfRange:
						++OutOfRangeSeen;
						if (!bSurvived)
						{
							AddError(FString::Printf(
								TEXT("T-UI-01: the card says 'out of range' about unit %d, which "
								     "this attack destroys -- the range is not the reason"),
								Def->id));
							return false;
						}
						// THE CLAUSE THE BRIEF NAMED. `out of range` is asserted only where
						// the distance genuinely is outside the defender's own band.
						if (bInBand)
						{
							AddError(FString::Printf(
								TEXT("T-UI-01: the card says 'out of range' at distance %d, which "
								     "is INSIDE unit %d's own band [%d, %d]"),
								View.Distance, Def->id, Def->unit.rangeMin, Def->unit.rangeMax));
							return false;
						}
						break;

					default:
						AddError(TEXT("T-UI-01: the card carried a counter reason outside the "
						              "three §2.11.3 names"));
						return false;
					}

					// The dying defender in the OTHER direction, so a mapping that never
					// chose `DefenderDestroyed` at all could not pass by choosing one of
					// the other two for a corpse.
					if (!bSurvived && View.CounterReason != EStratCounterReason::DefenderDestroyed)
					{
						AddError(FString::Printf(
							TEXT("T-UI-01: unit %d does not survive and the card's counter line "
							     "does not say so"), Def->id));
						return false;
					}
				}
			}
		}

		FStratAiCommand Command;
		if (!Bridge.NextAiCommand(Bridge.State().turn.activeSide, Command).bOk)
		{
			break;
		}
		if (!SubmitAsIs(Bridge, Command).bOk)
		{
			break;
		}
		++CommandsPlayed;
	}

	AddInfo(FString::Printf(
		TEXT("%d AI commands played, %d boards sampled; counter reasons: %d Number, "
		     "%d OutOfRange, %d DefenderDestroyed"),
		CommandsPlayed, Sampled, NumberSeen, OutOfRangeSeen, DestroyedSeen));

	// ---- the positive controls: all three arms, or the implications above are vacuous --
	TestTrue(TEXT("control: the drive actually moved the board"),           CommandsPlayed > 0);
	TestTrue(TEXT("control: the sweep reached the Number arm"),             NumberSeen     > 0);
	TestTrue(TEXT("control: the sweep reached the OutOfRange arm"),         OutOfRangeSeen > 0);
	TestTrue(TEXT("control: the sweep reached the DefenderDestroyed arm"),  DestroyedSeen  > 0);

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-01 -- the attacker's HP row is hidden for a counter that fires for nothing.
//
// STRETCH DECLARED, as above: §2.11.3's "and for the attacker whenever the counter is
// nonzero" is a visibility rule, not the forecast/resolution equality T-UI-01 states.
//
// A HAND-BUILT INPUT AND NOT A BOARD, and the reason is that a firing counter for ZERO
// damage is the position this scenario does not offer. `FStratForecastView::
// bShowAttackerHp`'s own declaration is explicit that the field is NOT `bCounterFires`
// -- "a `20 -> 20` row would teach the player that the counter did something" -- and
// that distinction is invisible on any board where every firing counter hurts. This is
// the arm `a-correct-mapping-can-be-unreachable-by-test` is about.
//
// NO NUMBER IS ASSERTED. Every assertion below is on the boolean the compose chose.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAttackerHpRowFollowsTheCounterDamageTest,
	"Stratocracy.StratUI.T-UI-01.AttackerHpRowFollowsTheCounterDamage",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAttackerHpRowFollowsTheCounterDamageTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratForecastViewClauses;

	// ---- a counter that fires and hurts: the row is drawn ---------------------
	{
		FStratAttackForecast Card = LoudCard();
		Card.bCounterFires = true;
		Card.CounterDamage = 2;

		FStratForecastView View;
		StratComposeForecastView(Card, View);

		TestTrue(TEXT("control: this is a card at all"), View.bHasForecast);
		TestTrue(TEXT("T-UI-01: a counter that fires for a non-zero number draws the attacker's "
		              "HP row"), View.bShowAttackerHp);
	}

	// ---- THE CLAUSE: a counter that fires for zero: the row is hidden ---------
	{
		FStratAttackForecast Card = LoudCard();
		Card.bCounterFires = true;
		Card.CounterDamage = 0;

		FStratForecastView View;
		StratComposeForecastView(Card, View);

		TestTrue(TEXT("control: this is a card at all"), View.bHasForecast);
		TestTrue(TEXT("control: and the counter really does fire on it"), View.bCounterFires);
		TestFalse(TEXT("T-UI-01: a counter that fires for ZERO does not draw the attacker's HP "
		               "row, so no 20 -> 20 is shown"), View.bShowAttackerHp);
	}

	// ---- and no counter at all: hidden, for the other reason ------------------
	{
		FStratAttackForecast Card = LoudCard();
		Card.bCounterFires = false;
		Card.CounterDamage = 0;

		FStratForecastView View;
		StratComposeForecastView(Card, View);

		TestFalse(TEXT("T-UI-01: a card with no counter draws no attacker HP row"),
			View.bShowAttackerHp);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-01 -- the flag band raises for EITHER flag, and names the side whose flag it is.
//
// STRETCH DECLARED: §2.11.3's `FLAG AT RISK -- this attack ends the match` band is a
// requirement the card carries, not the forecast/resolution equality T-UI-01 states.
//
// HAND-BUILT, AND NECESSARILY. An attacker's own flag dying to a counter, and two flags
// each lethal to the other, are positions no fixture on this project's one scenario
// reaches -- `StratForecastCardParity.cpp`'s own award clause reports that not even a
// lethal attack ON a flag is reachable from the fixture. `StratComposeForecastView` takes
// the plain struct rather than the bridge so that these arms are assertable at all.
//
// WHAT IS ASSERTED IS A SIDE AND A BOOLEAN, never a number. `RiskedFlagSide` is compared
// against the side carried ON THE SAME CARD, so a compose that named the wrong
// participant fails even though both sides are ordinary small integers.
//
// THE BOTH-FALL ARM IS PINNED, NOT RULED ON. §2.11.3 describes one band and gives no rule
// for a flag attacking a flag with each lethal to the other. `StratComposeForecastView`
// chose the DEFENDER's side, and stated the choice where it made it. This clause records
// what was chosen so that a later change to it is visible; it takes no position on
// whether the choice is the right one, and if §2.11.3 is ever amended this clause is the
// thing to update rather than the evidence against the amendment.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratFlagBandRaisesForEitherFlagTest,
	"Stratocracy.StratUI.T-UI-01.FlagBandRaisesForEitherFlag",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratFlagBandRaisesForEitherFlagTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratForecastViewClauses;

	// ---- neither flag: no band -----------------------------------------------
	{
		FStratAttackForecast Card = LoudCard();
		Card.bDefenderIsFlag = false;
		Card.bAttackerIsFlag = false;
		Card.bDefenderDies   = true;
		Card.bAttackerDies   = true;

		FStratForecastView View;
		StratComposeForecastView(Card, View);

		TestTrue(TEXT("control: this is a card at all"), View.bHasForecast);
		TestFalse(TEXT("T-UI-01: an attack that kills two ordinary units raises no flag band"),
			View.bFlagAtRisk);
		TestEqual(TEXT("T-UI-01: and names no risked side"),
			View.RiskedFlagSide, FStratForecastView().RiskedFlagSide);
	}

	// ---- a flag that is not killed: no band ----------------------------------
	{
		FStratAttackForecast Card = LoudCard();
		Card.bDefenderIsFlag = true;
		Card.bDefenderDies   = false;
		Card.bAttackerIsFlag = true;
		Card.bAttackerDies   = false;

		FStratForecastView View;
		StratComposeForecastView(Card, View);

		TestFalse(TEXT("T-UI-01: a flag on the card that this attack does not kill raises no "
		               "band -- the band is about lethality, not about presence"), View.bFlagAtRisk);
	}

	// ---- THE ENEMY FLAG FALLS: the band names the DEFENDER's side -------------
	{
		FStratAttackForecast Card = LoudCard();
		Card.AttackerSide    = 0;
		Card.DefenderSide    = 1;
		Card.bDefenderIsFlag = true;
		Card.bDefenderDies   = true;
		Card.bAttackerIsFlag = false;
		Card.bAttackerDies   = false;

		FStratForecastView View;
		StratComposeForecastView(Card, View);

		TestTrue(TEXT("T-UI-01: a lethal attack on a flag raises the band"), View.bFlagAtRisk);
		TestEqual(TEXT("T-UI-01: and names the DEFENDER's own side, taken off the same card"),
			View.RiskedFlagSide, Card.DefenderSide);
	}

	// ---- THE PLAYER'S OWN FLAG FALLS TO THE COUNTER: the band names the ATTACKER's ----
	// The half that rests on `FStratAttackForecast::bAttackerDies`, which is the bridge's
	// field because `strat::UiForecast` has none. §2.11.3: "No player can end a match,
	// theirs or the enemy's, without having been told on the card they clicked."
	{
		FStratAttackForecast Card = LoudCard();
		Card.AttackerSide    = 0;
		Card.DefenderSide    = 1;
		Card.bDefenderIsFlag = false;
		Card.bDefenderDies   = false;
		Card.bAttackerIsFlag = true;
		Card.bAttackerDies   = true;

		FStratForecastView View;
		StratComposeForecastView(Card, View);

		TestTrue(TEXT("T-UI-01: an attack whose counter kills the attacking flag raises the band"),
			View.bFlagAtRisk);
		TestEqual(TEXT("T-UI-01: and names the ATTACKER's own side, taken off the same card"),
			View.RiskedFlagSide, Card.AttackerSide);
	}

	// ---- BOTH FALL: the engineer's stated choice, recorded ---------------------
	{
		FStratAttackForecast Card = LoudCard();
		Card.AttackerSide    = 0;
		Card.DefenderSide    = 1;
		Card.bDefenderIsFlag = true;
		Card.bDefenderDies   = true;
		Card.bAttackerIsFlag = true;
		Card.bAttackerDies   = true;

		FStratForecastView View;
		StratComposeForecastView(Card, View);

		TestTrue(TEXT("T-UI-01: both flags falling raises the band"), View.bFlagAtRisk);
		TestEqual(TEXT("T-UI-01: and the DEFENDER's side is the one named, which is "
		               "StratComposeForecastView's own stated choice where §2.11.3 gives no rule"),
			View.RiskedFlagSide, Card.DefenderSide);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-01 -- an illegal forecast composes an ENTIRELY default card.
//
// STRETCH DECLARED: this is §2.11.3's "the card appears over a lit target" seen from the
// unlit side, not the forecast/resolution equality.
//
// THE INPUT IS LOUD ON EVERY FIELD, which is what makes the emptiness evidence.
// `LoudCard()` moves all seventeen fields off their defaults; only `bLegal` is then
// cleared. A compose that copied any field before consulting `bLegal` -- or that returned
// early leaving the caller's previous card standing -- shows up as one field that is not
// its default. A test that handed in a default-constructed source could not tell an
// emptied card from an untouched one.
//
// AND THE OUTPUT IS PRE-POISONED TOO, for the same reason from the other end: `OutForecast`
// arrives already carrying a full card, so "the compose wrote nothing" is distinguishable
// from "the compose wrote an empty card". `StratComposeForecastView`'s declaration
// promises the latter.
//
// THE EXPECTATION IS A DEFAULT-CONSTRUCTED `FStratForecastView`, compared field by field.
// No zero is typed here, and `CounterReason`'s default is whatever the struct says it is
// rather than whatever this file guesses.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratIllegalForecastComposesADefaultCardTest,
	"Stratocracy.StratUI.T-UI-01.IllegalForecastComposesADefaultCard",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratIllegalForecastComposesADefaultCardTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratForecastViewClauses;

	FStratAttackForecast Card = LoudCard();
	Card.bLegal        = false;
	Card.IllegalReason = TEXT("out of range");

	// The output arrives already carrying a full card. See the block above.
	FStratForecastView View;
	StratComposeForecastView(LoudCard(), View);
	if (!TestTrue(TEXT("control: the pre-poisoned output really does carry a card, so an "
	                   "untouched output and an emptied one are distinguishable"),
			View.bHasForecast))
	{
		return false;
	}

	StratComposeForecastView(Card, View);

	const FStratForecastView Default;
	if (!TestTrue(TEXT("T-UI-01: an illegal forecast composes a card equal, field by field, to a "
	                   "default-constructed FStratForecastView"),
			IsFieldwiseEqual(View, Default)))
	{
		AddError(FString::Printf(
			TEXT("the composed card differs from the default: bHasForecast=%d attacker=%d "
			     "defender=%d damage=%d reason=%d flagAtRisk=%d riskedSide=%d award=%d"),
			View.bHasForecast ? 1 : 0, View.AttackerUnitId, View.DefenderUnitId, View.Damage,
			static_cast<int32>(View.CounterReason), View.bFlagAtRisk ? 1 : 0,
			View.RiskedFlagSide, View.KillAwardFame));
		return false;
	}

	// Named separately because it is the field every consumer reads first.
	TestFalse(TEXT("T-UI-01: and there is no card"), View.bHasForecast);

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-01 -- a terrain PENALTY reaches the card with its sign intact.
//
// STRETCH DECLARED: §2.11.3 requires the modifier "named inline every time"; that is the
// card's requirement and not T-UI-01's own sentence.
//
// WHY THIS EXISTS BESIDE THE BRIDGE-SIDE TERRAIN CLAUSE.
// `StratForecastCardParity.cpp` pins that the bridge reads the defender row's own
// `defensePct`, and its sign control requires a negative row among the positions it
// sweeps. This clause pins the OTHER half of the same journey -- the copy through
// `StratComposeForecastView` -- and it does so over the negative row unconditionally,
// because the row is fetched from the loaded terrain table rather than waited for on a
// board. A single `abs()` or an `uint8` anywhere on either half is caught by one of the
// two.
//
// THE NUMBER IS NOT TYPED. `-10` appears nowhere here. The expectation is the minimum
// `defensePct` in `FStratBridge::MakeUiWorld().terrain` -- the very table the bridge
// indexes -- and a control asserts that that minimum really is negative, so a table edit
// that removed every penalty reddens this clause instead of hollowing it out.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratTerrainPenaltyReachesTheCardSignedTest,
	"Stratocracy.StratUI.T-UI-01.TerrainPenaltyReachesTheCardSigned",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratTerrainPenaltyReachesTheCardSignedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratForecastViewClauses;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge loads its definitions and seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	const strat::UiWorld World = Bridge.MakeUiWorld();
	if (!TestNotNull(TEXT("the projection carries the loaded terrain table"), World.terrain))
	{
		return false;
	}

	// The most negative row the LOADED table actually holds. Nothing typed, and the row's
	// own id travels with it.
	const strat::TerrainDef* Worst = nullptr;
	for (const strat::TerrainDef& Row : *World.terrain)
	{
		if (Worst == nullptr || Row.defensePct < Worst->defensePct)
		{
			Worst = &Row;
		}
	}
	if (!TestNotNull(TEXT("the loaded terrain table has rows"), Worst))
	{
		return false;
	}

	AddInfo(FString::Printf(TEXT("the loaded table's lowest defensePct is %d, on row '%s'"),
		Worst->defensePct, *FromStd(Worst->id)));

	if (!TestTrue(TEXT("control: the loaded terrain table really does hold a NEGATIVE "
	                   "defensePct, so the sign below is exercised"),
			Worst->defensePct < 0))
	{
		AddError(TEXT("every loaded terrain row is non-negative. §2.3's Bridge penalty is then "
		              "exercised by nothing -- fix the table upstream rather than dropping this "
		              "control"));
		return false;
	}

	const FName WorstId(*FromStd(Worst->id));

	FStratAttackForecast Card         = LoudCard();
	Card.DefenderTerrainDefensePct    = Worst->defensePct;
	Card.DefenderTerrainId            = WorstId;

	FStratForecastView View;
	StratComposeForecastView(Card, View);

	// ---- THE CLAUSE ----------------------------------------------------------
	TestEqual(TEXT("T-UI-01: the card carries the loaded row's defensePct unchanged, sign and all"),
		View.DefenderTerrainDefensePct, Worst->defensePct);
	TestTrue(TEXT("T-UI-01: and it is still negative on the card, so nothing took its magnitude"),
		View.DefenderTerrainDefensePct < 0);
	// `TestTrue` on an `FName` comparison rather than `TestEqual` on two `FString`s:
	// `FString` comparison is case-INSENSITIVE on this project and has already produced a
	// clause here that could not fail. `FName` equality is the type's own, and it is the
	// comparison a consumer of this field would perform.
	TestTrue(TEXT("T-UI-01: and the row's own id travels with it"),
		View.DefenderTerrainId == WorstId);

	return true;
}
