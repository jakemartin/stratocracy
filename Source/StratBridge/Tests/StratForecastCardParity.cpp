// The in-engine gate on `FStratBridge::AttackForecast` -- GDD §2.11.3's card at the
// module end, T-UI-01, plus T-FAME-01's kill award seen through it.
//
// WHAT WAS UNGATED BEFORE THIS FILE. `FStratAttackForecast` carries seventeen fields.
// Twelve of them are `strat::uiForecast`'s own answers copied across, and until this
// file nothing compared even those against the call they were copied from. The other
// five were produced INSIDE the bridge -- the two HP-after clamps, the counter-kill
// boolean, the defender's terrain row, and `strat::killAward`'s figure -- and the two
// clamps in particular were the only numbers on §2.11.3's card that no test compared
// against anything at all. `FStratAttackForecast`'s own block names that debt and names
// its discharge: "a test in `Source/StratBridge/Tests/` CAN call the oracle and compare
// it against these fields."
//
// WHERE EVERY EXPECTATION IN THIS FILE COMES FROM, and it is never "computed here":
//
//   - `strat::uiResolveForGate` is THE ORACLE for the two clamps, and it is called for
//     exactly the reason `StratCombatOutcomeParity.cpp`'s header block sets out. That
//     block states the construction: the function "has zero production callers in this
//     tree BY CONSTRUCTION", and "a `uiResolveForGate` invoked by the bridge would make
//     every comparison below a comparison of the bridge with itself". A TEST MAY CALL
//     IT; PRODUCTION MAY NOT. `FStratBridge::AttackForecast` writes the subtraction and
//     the clamp-at-zero out by hand precisely so that this file's comparison is between
//     two independent computations of the same fact rather than between one and itself.
//     Nothing below is asserted with a `-` on the expectation side.
//
//   - `FStratBridge::Forecast` -- which is `strat::uiForecast` and two channels --
//     supplies the damage, the distance, the defender's death and the counter. The card
//     is asserted to REPEAT that call rather than to agree with a second derivation.
//
//   - `strat::killAward`, handed the victim's own `UnitDef` off `FStratBridge::UnitDefs()`,
//     supplies the fame figure. Not half a cost computed here and not the literals 150
//     or 500 -- `Economy.h` owns all three of that rule's clauses.
//
//   - The terrain percentage comes off the SAME loaded table the bridge read, reached
//     through `MakeUiWorld().terrain` and the board's own `terrainAt`. There is no `-10`
//     and no terrain name written in this file.
//
//   - `Data/parity_fixture.save` supplies every position. No board is hand-authored
//     here; a hand-built mid-match state would be this file choosing the numbers it then
//     asserts. The hand-built inputs §2.11.3 genuinely needs -- a zero-damage counter, a
//     flag falling on either side -- belong to the PRESENTATION shapings, and those are
//     pinned in `Source/StratUI/Tests/StratForecastViewClauses.cpp` against
//     `StratComposeForecastView`, which takes the plain struct by value for that reason.
//
// WHAT THIS FILE DOES NOT PIN, stated rather than papered over:
//   - Nothing about `FStratForecastView`, `EStratCounterReason` or any of §2.11.3's
//     three presentation shapings. Those are `StratUI`'s and this module is BELOW it;
//     a clause here could not call `StratComposeForecastView` at all.
//   - Nothing about the decorator, the hover, or the act gate. Those are `StratPlay`'s
//     and are pinned in `StratForecastDecoratorClauses.cpp`.
//   - The `bAttackerDies` field's counter-kill spelling is pinned only where a
//     counter-kill is reachable from the fixture; see `kCounterKillPrefix` below, which
//     is `StratCombatOutcomeParity.cpp`'s measured prefix and not a new guess.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Math/IntPoint.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBridge.h"

#include "Economy.h"
#include "Save.h"
#include "Ui.h"

namespace StratForecastCardParity
{
	/** Must equal `StratBridgeSaveRecording.cpp`'s `kFirstSide`, as every fixture-driven
	 *  file in this directory carries. A different value replays a different game. */
	static const int32 kFirstSide = 0;

	/** The fixture attack whose forecast has a firing counter AND a surviving defender,
	 *  so BOTH clamps have something to clamp. `StratCombatOutcomeParity.cpp`'s
	 *  `kAttackWithCounter`, re-checked at run time against the fixture's own entry. */
	static const int32 kAttackWithCounter = 13;

	/** The fixture attack that kills, so `KillAwardFame` has a kill to reward.
	 *  `StratCombatOutcomeParity.cpp`'s `kAttackWithKill`. */
	static const int32 kAttackWithKill = 33;

	/** The shortest prefix from which the module itself forecasts a counter-KILL. Not a
	 *  fixture command -- the fixture never plays one. `StratCombatOutcomeParity.cpp`
	 *  measured this prefix; it is reused rather than re-guessed. */
	static const int32 kCounterKillPrefix = 20;

	/** How far into the fixture the sweeping clauses walk, and the stride. The sweep
	 *  exists to reach terrain and range positions the opening board does not offer;
	 *  every prefix it visits is the fixture's own, so no state here is authored. */
	static const int32 kSweepStride   = 7;
	static const int32 kSweepMaxSteps = 24;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static std::string ToStd(const FString& In)
	{
		return std::string(TCHAR_TO_UTF8(*In));
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

	static bool LoadFixture(strat::Save& Out, FString& OutError)
	{
		const FString Path = FPaths::Combine(DataDir(), TEXT("parity_fixture.save"));
		FString Text;
		if (!FFileHelper::LoadFileToString(Text, *Path))
		{
			OutError = FString::Printf(TEXT("parity_fixture.save unreadable at %s"), *Path);
			return false;
		}
		const strat::SaveLoadResult Parsed =
			strat::parseSave(ToStd(Text), "parity_fixture.save", Out);
		if (!Parsed.ok)
		{
			OutError = FString::Printf(TEXT("%s: %s"),
				*FromStd(Parsed.failedId), *FromStd(Parsed.reason));
			return false;
		}
		return true;
	}

	/** Seed, then replay the fixture's first `PrefixLength` commands. `ReplayLog` and not
	 *  a loop of `Submit`, for `StratCombatOutcomeParity.cpp`'s reason. */
	static bool SeedAndReplayPrefix(FStratBridge& Bridge, const strat::Save& Fixture,
	                                int32 PrefixLength, FString& OutError)
	{
		if (!SeedBridge(Bridge, OutError))
		{
			return false;
		}
		if (PrefixLength < 0 || PrefixLength > static_cast<int32>(Fixture.commandLog.size()))
		{
			OutError = FString::Printf(TEXT("prefix %d is outside the fixture's %d commands"),
				PrefixLength, static_cast<int32>(Fixture.commandLog.size()));
			return false;
		}

		TArray<strat::SaveCommand> Prefix;
		Prefix.Reserve(PrefixLength);
		for (int32 i = 0; i < PrefixLength; ++i)
		{
			Prefix.Add(Fixture.commandLog[static_cast<size_t>(i)]);
		}

		const FStratResult Replayed = Bridge.ReplayLog(Prefix);
		if (!Replayed.bOk)
		{
			OutError = FString::Printf(TEXT("replaying the fixture's first %d commands refused: %s"),
				PrefixLength, *Replayed.Reason);
			return false;
		}
		return true;
	}

	/** The unit standing on a hex, off a projection this file did not build. A plain loop
	 *  rather than a helper, because `strat::findUiUnit` answers by id and not by hex. */
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

	static FIntPoint AsPoint(const strat::Hex& Hex)
	{
		return FIntPoint(Hex.q, Hex.r);
	}
}

// ---------------------------------------------------------------------------
// T-UI-01 -- the card's two HP-after clamps are `strat::uiResolveForGate`'s two numbers.
//
// THE DEBT THIS DISCHARGES, named on `FStratAttackForecast` itself: the subtraction and
// the clamp-at-zero are written out in `FStratBridge::AttackForecast` because the one
// module function that would return them may not be called from production. What makes
// that arrangement safe rather than merely tidy is this clause -- the bridge computing
// independently and the oracle computing independently, compared. Delete this clause and
// the two most prominent numbers on §2.11.3's card go back to being unwitnessed.
//
// THREE POSITIONS, AND THE THIRD IS THE POINT. A surviving defender with a firing
// counter moves both numbers; a kill drives the defender's clamp to its floor; and a
// COUNTER-KILL drives the ATTACKER's clamp to its floor, which is the arm
// `StratCombatOutcomeParity.cpp` measured had never executed in this project's history
// until it went looking for it. Each position asserts its positive control before the
// equality that depends on it, because "predicted == measured" is trivially true of an
// attack in which nothing moves.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratForecastCardHpAfterIsTheOraclesTest,
	"Stratocracy.StratBridge.T-UI-01.ForecastCardHpAfterIsTheOracles",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratForecastCardHpAfterIsTheOraclesTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratForecastCardParity;

	strat::Save Fixture;
	FString Error;
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"), LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- position 1 and 2: two fixture attacks, one with a counter, one a kill ----
	const int32 Indices[2]  = { kAttackWithCounter, kAttackWithKill };
	bool        bSawCounter = false;
	bool        bSawKill    = false;

	for (const int32 Index : Indices)
	{
		if (!TestTrue(TEXT("the fixture is long enough to reach the chosen attack"),
				static_cast<int32>(Fixture.commandLog.size()) > Index))
		{
			return false;
		}
		const strat::SaveCommand& Attack = Fixture.commandLog[static_cast<size_t>(Index)];
		if (!TestEqual(*FString::Printf(TEXT("fixture entry %d is still an Attack"), Index),
				static_cast<int32>(Attack.kind), static_cast<int32>(strat::SaveCommandKind::Attack)))
		{
			AddError(TEXT("the fixture was re-emitted and its command order moved; re-pick the "
			              "index rather than forecasting whatever now sits there"));
			return false;
		}

		FStratBridge Bridge;
		if (!TestTrue(TEXT("the bridge reaches the position the attack was made from"),
				SeedAndReplayPrefix(Bridge, Fixture, Index, Error)))
		{
			AddError(Error);
			return false;
		}

		// THE ORACLE, on the same world the card was answered off. Read before the card
		// so that no ordering between them can be the reason they agree.
		const strat::UiWorld      Before = Bridge.MakeUiWorld();
		const strat::UiResolution Oracle =
			strat::uiResolveForGate(Before, Attack.unitId, Attack.hex);

		if (!TestTrue(*FString::Printf(
				TEXT("uiResolveForGate applied at fixture index %d, so there is an oracle"), Index),
				Oracle.applied))
		{
			AddError(FString::Printf(TEXT("the oracle refused: %s"), *FromStd(Oracle.forecast.reason)));
			return false;
		}

		FStratAttackForecast Card;
		const FStratResult   Asked =
			Bridge.AttackForecast(Attack.unitId, AsPoint(Attack.hex), Card);
		if (!TestTrue(*FString::Printf(TEXT("AttackForecast answers at fixture index %d"), Index),
				Asked.bOk))
		{
			AddError(Asked.Reason);
			return false;
		}
		if (!TestTrue(TEXT("and the rules module calls that fixture attack legal"), Card.bLegal))
		{
			return false;
		}

		// ---- the positive control for THIS position ------------------------------
		TestTrue(*FString::Printf(TEXT("control: attack %d predicts a non-zero loss"), Index),
			Card.Damage > 0);
		bSawCounter = bSawCounter || Card.bCounterFires;
		bSawKill    = bSawKill    || Card.bDefenderDies;

		// ---- THE CLAUSE ----------------------------------------------------------
		// Left side the bridge's, right side the oracle's. No arithmetic on either.
		TestEqual(*FString::Printf(
			TEXT("T-UI-01: the card's DefenderHpAfter at fixture index %d is "
			     "uiResolveForGate's defenderHpAfter"), Index),
			Card.DefenderHpAfter, Oracle.defenderHpAfter);
		TestEqual(*FString::Printf(
			TEXT("T-UI-01: the card's AttackerHpAfter at fixture index %d is "
			     "uiResolveForGate's attackerHpAfter"), Index),
			Card.AttackerHpAfter, Oracle.attackerHpAfter);
	}

	TestTrue(TEXT("control: one of the two fixture positions fired a counter, so the "
	              "attacker's clamp had a subtrahend"), bSawCounter);
	TestTrue(TEXT("control: one of the two fixture positions was a kill, so the defender's "
	              "clamp reached its floor"), bSawKill);

	// ---- position 3: the counter-kill, where the ATTACKER's clamp floors ----------
	{
		FStratBridge Bridge;
		if (!TestTrue(TEXT("the bridge reaches the prefix the counter-kill search runs from"),
				SeedAndReplayPrefix(Bridge, Fixture, kCounterKillPrefix, Error)))
		{
			AddError(Error);
			return false;
		}

		const strat::UiWorld World      = Bridge.MakeUiWorld();
		const int32          ActiveSide = Bridge.State().turn.activeSide;

		// THE MODULE'S OWN PREDICATE for "the counter kills the attacker", asked of
		// `FStratBridge::Forecast` -- this file picks no damage number and no board.
		int32      ChosenAttacker = INDEX_NONE;
		strat::Hex ChosenHex;
		int32      Found = 0;
		for (const strat::UiUnit& A : World.units)
		{
			if (A.side != ActiveSide)
			{
				continue;
			}
			for (const strat::UiUnit& D : World.units)
			{
				if (D.side == ActiveSide)
				{
					continue;
				}
				strat::UiForecast F;
				if (!Bridge.Forecast(A.id, D.hex, F).bOk || !F.legal)
				{
					continue;
				}
				if (!F.defenderDies && F.counterFires && F.counterDamage >= A.unit.hp)
				{
					++Found;
					if (ChosenAttacker == INDEX_NONE)
					{
						ChosenAttacker = A.id;
						ChosenHex      = D.hex;
					}
				}
			}
		}

		if (!TestTrue(*FString::Printf(
				TEXT("T-UI-01: the fixture position after %d commands still offers a "
				     "counter-kill the module itself forecasts"), kCounterKillPrefix),
				ChosenAttacker != INDEX_NONE))
		{
			AddError(TEXT("no counter-kill is reachable from this prefix any more. The attacker "
			              "clamp's floor is then UNCOVERED -- re-pick the prefix against a fresh "
			              "probe rather than deleting this arm"));
			return false;
		}
		AddInfo(FString::Printf(TEXT("%d counter-kill candidates at prefix %d"),
			Found, kCounterKillPrefix));

		const strat::UiResolution Oracle =
			strat::uiResolveForGate(World, ChosenAttacker, ChosenHex);
		if (!TestTrue(TEXT("the oracle applies to the chosen counter-kill"), Oracle.applied))
		{
			return false;
		}

		FStratAttackForecast Card;
		const FStratResult   Asked = Bridge.AttackForecast(ChosenAttacker, AsPoint(ChosenHex), Card);
		if (!TestTrue(TEXT("AttackForecast answers for the chosen counter-kill"), Asked.bOk))
		{
			AddError(Asked.Reason);
			return false;
		}

		// ---- the control: this really is the floor, and the oracle says so --------
		TestEqual(TEXT("control: uiResolveForGate empties the attacker's pool"),
			Oracle.attackerHpAfter, 0);
		TestTrue(TEXT("control: the attacker had HP to lose"), Card.AttackerHpBefore > 0);

		// ---- THE CLAUSE ----------------------------------------------------------
		TestEqual(TEXT("T-UI-01: the card's AttackerHpAfter on a counter-kill is "
		               "uiResolveForGate's attackerHpAfter"),
			Card.AttackerHpAfter, Oracle.attackerHpAfter);
		TestEqual(TEXT("T-UI-01: and its DefenderHpAfter is uiResolveForGate's defenderHpAfter"),
			Card.DefenderHpAfter, Oracle.defenderHpAfter);

		// The bridge's own counter-kill boolean, beside the clamp it must agree with.
		// This is `bAttackerDies`' only witness in the tree.
		TestTrue(TEXT("T-UI-01: and the card says the attacker dies"), Card.bAttackerDies);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-01 -- the card REPEATS `strat::uiForecast` and does not re-derive it.
//
// STRETCH DECLARED. T-UI-01's own sentence is about the forecast equalling the
// RESOLUTION. This clause asserts no such equality -- it asserts that the twelve fields
// `FStratBridge::AttackForecast` says it copies are copies, over every legal and every
// illegal position the sweep reaches. It rides T-UI-01 because the forecast card is that
// ID's subject and by the ruling of 2026-08-26, not because the ID's sentence covers it.
//
// WHY IT IS WORTH A CLAUSE. `AttackForecast` calls `Forecast` and then writes fifteen
// assignments. Every one of them is a place a field can be dropped, transposed with its
// neighbour, or filled from the wrong participant -- `bAttackerIsFlag` beside
// `bDefenderIsFlag`, `AttackerSide` beside `DefenderSide`, `AttackerHpBefore` beside
// `DefenderHpBefore`. A transposition moves no number the compiler can see.
//
// THE SWEEP IS THE FIXTURE'S, NOT THIS FILE'S. It walks prefixes of
// `Data/parity_fixture.save` and asks the card about every attacker/hex pair the board
// offers at each. Both the legal and the illegal answers are asserted, because the
// illegal path's contract -- everything default but the reason and the echoed attacker
// -- is a claim `FStratBridge::AttackForecast`'s declaration makes in as many words.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratForecastCardRepeatsTheModulesForecastTest,
	"Stratocracy.StratBridge.T-UI-01.ForecastCardRepeatsTheModulesForecast",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratForecastCardRepeatsTheModulesForecastTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratForecastCardParity;

	strat::Save Fixture;
	FString Error;
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"), LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}

	int32 LegalSeen   = 0;
	int32 IllegalSeen = 0;
	int32 KillSeen    = 0;
	int32 CounterSeen = 0;

	for (int32 Step = 0; Step < kSweepMaxSteps; ++Step)
	{
		const int32 Prefix = Step * kSweepStride;
		if (Prefix > static_cast<int32>(Fixture.commandLog.size()))
		{
			break;
		}

		FStratBridge Bridge;
		if (!TestTrue(TEXT("the bridge reaches each swept fixture prefix"),
				SeedAndReplayPrefix(Bridge, Fixture, Prefix, Error)))
		{
			AddError(Error);
			return false;
		}

		const strat::UiWorld World = Bridge.MakeUiWorld();
		for (const strat::UiUnit& A : World.units)
		{
			for (const strat::UiUnit& D : World.units)
			{
				strat::UiForecast Module;
				const FStratResult Direct = Bridge.Forecast(A.id, D.hex, Module);

				FStratAttackForecast Card;
				const FStratResult   Asked = Bridge.AttackForecast(A.id, AsPoint(D.hex), Card);

				if (!Direct.bOk)
				{
					// Both channels must refuse together, or the card is answering a
					// question `Forecast` would not have taken.
					TestFalse(TEXT("T-UI-01: the card refuses wherever Forecast refuses"), Asked.bOk);
					continue;
				}
				if (!TestTrue(TEXT("T-UI-01: the card answers wherever Forecast answers"), Asked.bOk))
				{
					AddError(FString::Printf(TEXT("prefix %d, attacker %d -> (%d,%d): %s"),
						Prefix, A.id, D.hex.q, D.hex.r, *Asked.Reason));
					return false;
				}

				// The echoed attacker, on every path including the illegal one.
				if (Card.AttackerUnitId != A.id)
				{
					AddError(FString::Printf(
						TEXT("T-UI-01: the card echoed attacker %d for a question about %d"),
						Card.AttackerUnitId, A.id));
					return false;
				}

				if (Card.bLegal != Module.legal)
				{
					AddError(FString::Printf(
						TEXT("T-UI-01: the card called attacker %d -> (%d,%d) %s where uiForecast "
						     "called it %s"),
						A.id, D.hex.q, D.hex.r,
						Card.bLegal ? TEXT("legal") : TEXT("illegal"),
						Module.legal ? TEXT("legal") : TEXT("illegal")));
					return false;
				}

				if (!Module.legal)
				{
					++IllegalSeen;
					// The illegal contract, as the declaration states it: everything
					// default but the reason and the echoed attacker. Compared against a
					// default-constructed struct rather than against typed zeroes.
					const FStratAttackForecast Empty;
					if (Card.Damage        != Empty.Damage        ||
						Card.Distance      != Empty.Distance      ||
						Card.DefenderUnitId!= Empty.DefenderUnitId||
						Card.AttackerSide  != Empty.AttackerSide  ||
						Card.DefenderSide  != Empty.DefenderSide  ||
						Card.KillAwardFame != Empty.KillAwardFame ||
						Card.DefenderTerrainDefensePct != Empty.DefenderTerrainDefensePct ||
						Card.DefenderTerrainId != Empty.DefenderTerrainId ||
						Card.AttackerHpBefore != Empty.AttackerHpBefore ||
						Card.DefenderHpBefore != Empty.DefenderHpBefore ||
						Card.AttackerHpAfter  != Empty.AttackerHpAfter  ||
						Card.DefenderHpAfter  != Empty.DefenderHpAfter  ||
						Card.bDefenderDies != Empty.bDefenderDies ||
						Card.bCounterFires != Empty.bCounterFires ||
						Card.CounterDamage != Empty.CounterDamage ||
						Card.bAttackerDies != Empty.bAttackerDies ||
						Card.bAttackerIsFlag != Empty.bAttackerIsFlag ||
						Card.bDefenderIsFlag != Empty.bDefenderIsFlag)
					{
						AddError(FString::Printf(
							TEXT("T-UI-01: an illegal card for attacker %d -> (%d,%d) carried a "
							     "non-default field beside its reason"), A.id, D.hex.q, D.hex.r));
						return false;
					}
					// CASE-SENSITIVE ON PURPOSE. `FString::operator==` on this project is
					// case-INSENSITIVE, which has already produced a clause here that could
					// not fail; the reason is forwarded verbatim and is asserted verbatim.
					if (!Card.IllegalReason.Equals(FromStd(Module.reason), ESearchCase::CaseSensitive))
					{
						AddError(FString::Printf(
							TEXT("T-UI-01: the card's reason was '%s' where uiForecast said '%s'"),
							*Card.IllegalReason, *FromStd(Module.reason)));
						return false;
					}
					continue;
				}

				++LegalSeen;
				KillSeen    += Module.defenderDies ? 1 : 0;
				CounterSeen += Module.counterFires ? 1 : 0;

				// ---- THE CLAUSE: twelve copies, each against the call it came from ----
				const strat::UiUnit* const Def = UnitOnHex(World, D.hex);
				if (Def == nullptr)
				{
					AddError(TEXT("a legal forecast whose defender is not on the projection"));
					return false;
				}

				const bool bMatches =
					Card.Damage           == Module.damage        &&
					Card.Distance         == Module.distance      &&
					Card.bDefenderDies    == Module.defenderDies  &&
					Card.bCounterFires    == Module.counterFires  &&
					Card.CounterDamage    == Module.counterDamage &&
					Card.IllegalReason.IsEmpty()                  &&
					Card.DefenderUnitId   == Def->id              &&
					Card.AttackerSide     == A.side               &&
					Card.DefenderSide     == Def->side            &&
					Card.bAttackerIsFlag  == A.isFlag             &&
					Card.bDefenderIsFlag  == Def->isFlag          &&
					Card.AttackerHpBefore == A.unit.hp            &&
					Card.DefenderHpBefore == Def->unit.hp;
				if (!bMatches)
				{
					AddError(FString::Printf(
						TEXT("T-UI-01: the card for attacker %d -> (%d,%d) at prefix %d does not "
						     "repeat uiForecast and the projection it was read off"),
						A.id, D.hex.q, D.hex.r, Prefix));
					return false;
				}
			}
		}
	}

	AddInfo(FString::Printf(TEXT("%d legal and %d illegal forecasts swept; %d kills, %d counters"),
		LegalSeen, IllegalSeen, KillSeen, CounterSeen));

	// ---- the positive controls -----------------------------------------------
	// Without these the sweep above is satisfiable by a board on which nothing is ever
	// legal, which is the shape a sweeping clause fails silently in.
	TestTrue(TEXT("control: the sweep reached legal forecasts"),   LegalSeen   > 0);
	TestTrue(TEXT("control: the sweep reached illegal forecasts"), IllegalSeen > 0);
	TestTrue(TEXT("control: the sweep reached a lethal forecast"), KillSeen    > 0);
	TestTrue(TEXT("control: the sweep reached a firing counter"),  CounterSeen > 0);

	return true;
}

// ---------------------------------------------------------------------------
// T-FAME-01 -- the card's `+N Fame` is `strat::killAward`'s answer about THIS victim.
//
// THIS CLAUSE RIDES T-FAME-01 AND NOT T-UI-01, because the award itself is that ID's own
// subject; §2.7 / Q5 owns the "half the cost, or a flat 500 for a flag, REPLACING and
// never stacking" ruling, and what is asserted here is that the card quotes that
// function rather than restating any of its three clauses.
//
// THE FLAG'S 500 IS NEVER TYPED. The expectation on every arm is
// `strat::killAward(the victim's own UnitDef, the victim's own isFlag)`. The flat figure
// appears nowhere in this file; a change to it moves both sides of the comparison
// together, which is the point.
//
// AND THE isFlag ARGUMENT IS PROVED LOAD-BEARING. A card that ignored the flag bit
// entirely would satisfy the equality above on every ordinary victim, because every
// ordinary victim's award is the same either way only if `killAward` ignores the bit
// too. The control asserts that `killAward(def, true) != killAward(def, false)` for the
// tables actually loaded, so the equality below cannot be satisfied by dropping the
// argument.
//
// WHAT THIS DOES NOT PIN, and it is stated rather than left to be discovered: whether a
// FLAG can be killed from a position the fixture reaches. The sweep looks, and reports
// what it found; if it found none, the flat-500 path through `AttackForecast` is
// exercised by nothing on a real board and this file says so instead of implying
// otherwise.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratForecastKillAwardIsTheModulesAwardTest,
	"Stratocracy.StratBridge.T-FAME-01.ForecastKillAwardIsTheModulesAward",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratForecastKillAwardIsTheModulesAwardTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratForecastCardParity;

	strat::Save Fixture;
	FString Error;
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"), LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}

	int32 KillsSeen     = 0;
	int32 SurvivalsSeen = 0;
	int32 FlagKillsSeen = 0;
	bool  bFlagBitMoves = false;

	for (int32 Step = 0; Step < kSweepMaxSteps; ++Step)
	{
		const int32 Prefix = Step * kSweepStride;
		if (Prefix > static_cast<int32>(Fixture.commandLog.size()))
		{
			break;
		}

		FStratBridge Bridge;
		if (!TestTrue(TEXT("the bridge reaches each swept fixture prefix"),
				SeedAndReplayPrefix(Bridge, Fixture, Prefix, Error)))
		{
			AddError(Error);
			return false;
		}

		const strat::UiWorld               World = Bridge.MakeUiWorld();
		const std::vector<strat::UnitDef>& Defs  = Bridge.UnitDefs();

		for (const strat::UiUnit& A : World.units)
		{
			for (const strat::UiUnit& D : World.units)
			{
				FStratAttackForecast Card;
				if (!Bridge.AttackForecast(A.id, AsPoint(D.hex), Card).bOk || !Card.bLegal)
				{
					continue;
				}

				const strat::UiUnit* const Def = UnitOnHex(World, D.hex);
				if (Def == nullptr ||
					Def->defIndex < 0 ||
					static_cast<size_t>(Def->defIndex) >= Defs.size())
				{
					AddError(TEXT("a legal forecast whose defender has no loaded definition"));
					return false;
				}
				const strat::UnitDef& Victim = Defs[static_cast<size_t>(Def->defIndex)];

				// The control, evaluated on the victim actually in front of us: the flag
				// bit changes the module's answer, so the equality below is not
				// satisfiable by a bridge that drops it.
				bFlagBitMoves = bFlagBitMoves ||
					(strat::killAward(Victim, true) != strat::killAward(Victim, false));

				// ---- THE CLAUSE ------------------------------------------------
				const int32 Expected = Card.bDefenderDies
					? strat::killAward(Victim, Def->isFlag)
					: 0;

				if (Card.KillAwardFame != Expected)
				{
					AddError(FString::Printf(
						TEXT("T-FAME-01: the card awarded %d for %s victim %d where killAward "
						     "says %d"),
						Card.KillAwardFame,
						Card.bDefenderDies ? TEXT("dying") : TEXT("surviving"),
						Def->id, Expected));
					return false;
				}

				KillsSeen     += Card.bDefenderDies ? 1 : 0;
				SurvivalsSeen += Card.bDefenderDies ? 0 : 1;
				FlagKillsSeen += (Card.bDefenderDies && Def->isFlag) ? 1 : 0;
			}
		}
	}

	AddInfo(FString::Printf(TEXT("%d lethal and %d survivable forecasts swept; %d of the kills "
	                             "were against a flag"), KillsSeen, SurvivalsSeen, FlagKillsSeen));

	TestTrue(TEXT("control: the sweep reached a lethal forecast, so the award path ran"),
		KillsSeen > 0);
	TestTrue(TEXT("control: the sweep reached a surviving defender, so the zero path ran"),
		SurvivalsSeen > 0);
	TestTrue(TEXT("control: killAward's flag bit changes its answer for a loaded victim, so the "
	              "clause above cannot be satisfied by a card that ignores isFlag"),
		bFlagBitMoves);

	if (FlagKillsSeen == 0)
	{
		// STATED AND NOT ASSERTED. No fixture position offers a lethal attack on a flag,
		// so `killAward`'s flat-500 REPLACEMENT is not exercised through
		// `FStratBridge::AttackForecast` by anything on a real board. The compose end of
		// the same number is exercised over a hand-built card in
		// `Source/StratUI/Tests/StratForecastViewClauses.cpp`. Reported rather than
		// turned into a failure, because the absence is the scenario's and not the code's.
		AddInfo(TEXT("no lethal attack on a flag is reachable from the fixture's positions; "
		             "the flat-500 arm of killAward is unexercised HERE by construction"));
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-01 -- the card's terrain percentage is the DEFENDER row's own, sign and all.
//
// STRETCH DECLARED. T-UI-01's sentence is the forecast/resolution equality; this clause
// asserts a table read, which the forecast card needs and which that sentence does not
// claim. It rides T-UI-01 for the card's sake by the 2026-08-26 ruling.
//
// THE EXPECTATION IS THE LOADED TABLE'S. `MakeUiWorld().terrain` is the very vector
// `FStratBridge::AttackForecast` indexed, and `Board::terrainAt` is the very lookup it
// performed -- but performed here from the DEFENDER the card names rather than from the
// hex the caller asked about, so a card that read the ATTACKER's hex fails this.
//
// THE SIGN IS THE REASON THIS CLAUSE EXISTS. `TerrainDef::defensePct` is signed and its
// own comment names the case (`Bridge is -10`); §2.11.3 requires the modifier named
// inline every time, which includes the times it is a penalty. A sweep that only ever
// met non-negative rows would pass over an `abs()` that nobody wrote. The control
// therefore requires a negative row IN THE SWEPT SET and fails loudly if the fixture
// stops offering one, rather than reporting green on a weaker board.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratForecastTerrainBonusIsTheDefenderRowsTest,
	"Stratocracy.StratBridge.T-UI-01.ForecastTerrainBonusIsTheDefenderRows",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratForecastTerrainBonusIsTheDefenderRowsTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratForecastCardParity;

	strat::Save Fixture;
	FString Error;
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"), LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}

	TSet<FName> RowsSeen;
	int32       NegativeSeen = 0;
	int32       PositiveSeen = 0;
	int32       Compared     = 0;

	for (int32 Step = 0; Step < kSweepMaxSteps; ++Step)
	{
		const int32 Prefix = Step * kSweepStride;
		if (Prefix > static_cast<int32>(Fixture.commandLog.size()))
		{
			break;
		}

		FStratBridge Bridge;
		if (!TestTrue(TEXT("the bridge reaches each swept fixture prefix"),
				SeedAndReplayPrefix(Bridge, Fixture, Prefix, Error)))
		{
			AddError(Error);
			return false;
		}

		const strat::UiWorld World = Bridge.MakeUiWorld();
		if (!TestNotNull(TEXT("the projection carries the loaded terrain table"), World.terrain))
		{
			return false;
		}

		for (const strat::UiUnit& A : World.units)
		{
			for (const strat::UiUnit& D : World.units)
			{
				FStratAttackForecast Card;
				if (!Bridge.AttackForecast(A.id, AsPoint(D.hex), Card).bOk || !Card.bLegal)
				{
					continue;
				}

				const strat::UiUnit* const Def = UnitOnHex(World, D.hex);
				if (Def == nullptr)
				{
					AddError(TEXT("a legal forecast whose defender is not on the projection"));
					return false;
				}

				// The expectation: the board's own index at the DEFENDER's hex, into the
				// bridge's own loaded table. Nothing typed.
				const int32 Index = World.board.terrainAt(Def->hex);
				if (Index < 0 || static_cast<size_t>(Index) >= World.terrain->size())
				{
					AddError(FString::Printf(TEXT("hex (%d,%d) carries terrain index %d"),
						Def->hex.q, Def->hex.r, Index));
					return false;
				}
				const strat::TerrainDef& Row = (*World.terrain)[static_cast<size_t>(Index)];

				// ---- THE CLAUSE ------------------------------------------------
				if (Card.DefenderTerrainDefensePct != Row.defensePct)
				{
					AddError(FString::Printf(
						TEXT("T-UI-01: the card said %d%% for the hex under defender %d where the "
						     "loaded row says %d%%"),
						Card.DefenderTerrainDefensePct, Def->id, Row.defensePct));
					return false;
				}
				const FName RowId(*FromStd(Row.id));
				if (Card.DefenderTerrainId != RowId)
				{
					AddError(FString::Printf(
						TEXT("T-UI-01: the card named terrain '%s' where the loaded row is '%s'"),
						*Card.DefenderTerrainId.ToString(), *RowId.ToString()));
					return false;
				}

				++Compared;
				RowsSeen.Add(RowId);
				NegativeSeen += (Row.defensePct <  0) ? 1 : 0;
				PositiveSeen += (Row.defensePct >  0) ? 1 : 0;
			}
		}
	}

	TArray<FName> Names = RowsSeen.Array();
	FString       Joined;
	for (const FName& N : Names)
	{
		Joined += (Joined.IsEmpty() ? TEXT("") : TEXT(", ")) + N.ToString();
	}
	AddInfo(FString::Printf(TEXT("%d forecasts compared over defender terrain rows: %s"),
		Compared, *Joined));

	TestTrue(TEXT("control: the sweep compared at least one forecast"), Compared > 0);
	TestTrue(TEXT("control: the sweep met a POSITIVE terrain bonus"), PositiveSeen > 0);

	// THE SIGN CONTROL, and it is a hard one on purpose. See the block above.
	if (!TestTrue(TEXT("control: the sweep met a NEGATIVE terrain bonus, so the sign is "
	                   "actually exercised"), NegativeSeen > 0))
	{
		AddError(TEXT("no defender in any swept fixture position stood on a hex whose row has a "
		              "negative defensePct. The signed half of this card is then covered by "
		              "nothing on a real board -- widen the sweep or re-pick the stride rather "
		              "than dropping this control"));
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-01 -- `bLegal` is exactly membership in the lit target set.
//
// STRETCH DECLARED, as above: this is not the forecast/resolution equality T-UI-01's own
// sentence states. It is §2.11.3's "the card appears over a LIT target", made checkable.
//
// WHY IT IS NOT CIRCULAR EVEN THOUGH BOTH SIDES CALL `uiForecast`.
// `FStratBridge::AttackForecast`'s declaration argues that `bLegal` and lit membership
// are the same predicate BECAUSE `AttackTargetHexes` keeps a hex on the same answer.
// That is an argument about today's implementation of a DIFFERENT method, and it is
// exactly the kind of premise that stops holding when somebody adds a filter to the
// target set -- a fog rule, an already-acted skip, a range-1 hole. The two are compared
// here so that the day they diverge is reported by a test rather than by a card
// appearing over an unlit hex.
//
// THE COMPARISON IS SET EQUALITY IN BOTH DIRECTIONS, over every hex on the board and not
// only over the hexes either side proposes. A one-directional check would pass a target
// set that had silently gained a hex.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratForecastLegalityIsTheLitTargetSetTest,
	"Stratocracy.StratBridge.T-UI-01.ForecastLegalityIsTheLitTargetSet",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratForecastLegalityIsTheLitTargetSetTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratForecastCardParity;

	strat::Save Fixture;
	FString Error;
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"), LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}

	int32 AttackersChecked = 0;
	int32 LitHexesSeen     = 0;

	for (int32 Step = 0; Step < kSweepMaxSteps; ++Step)
	{
		const int32 Prefix = Step * kSweepStride;
		if (Prefix > static_cast<int32>(Fixture.commandLog.size()))
		{
			break;
		}

		FStratBridge Bridge;
		if (!TestTrue(TEXT("the bridge reaches each swept fixture prefix"),
				SeedAndReplayPrefix(Bridge, Fixture, Prefix, Error)))
		{
			AddError(Error);
			return false;
		}

		const strat::UiWorld World = Bridge.MakeUiWorld();

		for (const strat::UiUnit& A : World.units)
		{
			TArray<FIntPoint> Lit;
			if (!Bridge.AttackTargetHexes(A.id, Lit).bOk)
			{
				continue;
			}
			++AttackersChecked;
			LitHexesSeen += Lit.Num();

			const TSet<FIntPoint> LitSet(Lit);

			// The card's own answer, over EVERY hex on the board.
			TSet<FIntPoint> LegalSet;
			for (int32 Q = 0; Q < World.board.bounds.cols; ++Q)
			{
				for (int32 R = 0; R < World.board.bounds.rows; ++R)
				{
					const FIntPoint      Hex(Q, R);
					FStratAttackForecast Card;
					if (Bridge.AttackForecast(A.id, Hex, Card).bOk && Card.bLegal)
					{
						LegalSet.Add(Hex);
					}
				}
			}

			// ---- THE CLAUSE, both directions -------------------------------------
			for (const FIntPoint& Hex : LegalSet)
			{
				if (!LitSet.Contains(Hex))
				{
					AddError(FString::Printf(
						TEXT("T-UI-01: attacker %d gets a legal card over (%d,%d), which "
						     "AttackTargetHexes does not light"), A.id, Hex.X, Hex.Y));
					return false;
				}
			}
			for (const FIntPoint& Hex : LitSet)
			{
				if (!LegalSet.Contains(Hex))
				{
					AddError(FString::Printf(
						TEXT("T-UI-01: AttackTargetHexes lights (%d,%d) for attacker %d, which "
						     "gets no legal card"), Hex.X, Hex.Y, A.id));
					return false;
				}
			}
		}
	}

	AddInfo(FString::Printf(TEXT("%d attackers compared against %d lit hexes in total"),
		AttackersChecked, LitHexesSeen));

	TestTrue(TEXT("control: attackers were compared"), AttackersChecked > 0);
	// Without this the set equality above is satisfiable by two empty sets everywhere.
	TestTrue(TEXT("control: at least one attacker actually had a lit target"), LitHexesSeen > 0);

	return true;
}
