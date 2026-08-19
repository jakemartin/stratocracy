// The editor pass for the engine-typed façade phase 4 added to `FStratBridge`, and for the
// half of the hot-seat replay-parity criterion that can only be measured in this module.
// GDD §2.5, §2.6, §4.9, §4.10, §4.11 rows 8 and 10.
//
//   T-UI-02   the movement set a gameplay caller receives IS `Reachable`'s set (§2.5).
//   T-UI-01   the attack-target enumeration adds no rule of its own to `uiForecast` (§2.6).
//   T-SAVE-06 `Turn()` / `SideToMove()` read before a submission equal the `{turn, side}`
//             the bridge then stamps onto the recorded entry (§4.10).
//   T-SAVE-05 a log this bridge recorded, replayed onto a fresh seed, reaches an equal
//             `stateHash`, and the recorded length equals the number of ACCEPTED commands.
//
// WHY THIS FILE IS IN StratBridge AND COULD NOT BE ANYWHERE ELSE, which is the same
// reason StratBridgeQueryParity.cpp opens with. Every clause below compares an
// engine-typed answer (`TArray<FIntPoint>`, `int32`, `FString`) against the `strat::`
// answer for the same question, so the file has to be able to CALL both, and the vendored
// sources carry no `_API` macro. A "façade equals the typed method" clause written in
// StratPlay is not a weaker test, it is `LNK2019`.
//
// WHERE EVERY EXPECTATION COMES FROM. `ReachableHexesMatchesReachable` takes its
// right-hand side from `FStratBridge::Reachable` -- the typed method the façade forwards --
// entry for entry. `AttackTargetsAreForecastLegal` takes its verdict from
// `FStratBridge::Forecast`, in BOTH directions. `TurnAndSideAreReadBeforeStamp` compares
// two module-side readings of the same fact against each other and computes neither.
// `RecordedLogReplaysToEqualHash` compares two `StateHash()` strings, both the rules
// module's. NOTHING BELOW RE-COSTS A PATH, RE-ROLLS A DAMAGE NUMBER, OR PREDICTS A HASH.
//
// THE ONE PLACE A SEQUENCE IS AUTHORED rather than read is the command script in the
// T-SAVE-05 clause, and every hex in it is chosen AT RUN TIME out of
// `ReachableHexes`'s own answer. There is no coordinate literal in this file. That is
// fixture construction -- the same standing that StratMatchReconcile.cpp's edited models
// have -- and it is not an expectation: the expectation is the second bridge's hash.
//
// WHAT THIS FILE DELIBERATELY DOES NOT DO is drive `FStratSelectionMachine`. That type
// lives in `StratPlay`, which depends on this module and not the other way round, so a
// clause here cannot see a click. `StratPlay/Tests/StratHotSeatReplayParity.cpp` drives
// the clicks and now reads the bridge's own recording through the engine-typed
// `RecordedCommandCount()` / `ReplayRecordedLogOnto()` pair; this file is what pins the
// typed side -- that `RecordedLog()` is exactly the accepted set, that replaying it lands
// on the same state, and that the engine-typed replay REFUSES the six inputs that would
// let the other clause pass vacuously.

#include "Misc/AutomationTest.h"

#include "Containers/Set.h"
#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBridge.h"

// The vendored rules module, by short name via StratRules' PublicIncludePaths. Named
// explicitly rather than left to the transitive include through StratBridge.h, because
// this file reads these types directly.
#include "Hex.h"
#include "Save.h"
#include "Scenario.h"
#include "Ui.h"

namespace StratSelectionFacadeParity
{
	// The same harness pin StratBridgeParity.cpp, StratBridgeQueryParity.cpp and
	// StratScoreboardParity.cpp all carry, and it must stay equal to theirs. Replay.h
	// states that no rule in this project decides which side moves first.
	static const int32 kFirstSide = 0;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static FString FromStd(const std::string& In)
	{
		return FString(UTF8_TO_TCHAR(In.c_str()));
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** Both axial and odd-r offset, because the scenario file authors the second. */
	static FString Describe(const strat::Hex& H)
	{
		int Col = 0;
		int Row = 0;
		strat::axialToOffset(H, Col, Row);
		return FString::Printf(TEXT("axial(%d,%d)=offset[%d,%d]"), H.q, H.r, Col, Row);
	}

	static FString DescribePoint(FIntPoint P)
	{
		return Describe(strat::Hex{ P.X, P.Y });
	}

	/**
	 * The seeded opening: shipped tables mapped in, shipped scenario seeded through the
	 * rules module's own `seedFromScenario`.
	 *
	 * Returns false and fills OutError rather than asserting, on StratBridgeParity's
	 * precedent -- a missing asset becomes a named failure inside the caller's own clause
	 * list rather than a crash in a helper.
	 */
	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		UDataTable* Units = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		UDataTable* Terrain = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
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
		return true;
	}

	static std::string ToStd(const FString& In)
	{
		return std::string(TCHAR_TO_UTF8(*In));
	}

	/** Reads the committed §4.10 fixture through the rules module's own parser. */
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

	/**
	 * Seeds and replays the fixture's first `PrefixLength` commands, leaving the bridge at
	 * the mid-match position the fixture's own log reached at that index.
	 *
	 * THE INDEX IS THE ONE StratBridgeQueryParity.cpp ALREADY DEPENDS ON, and for the same
	 * reason: entry 17 of the shipped fixture is an Attack, so replaying [0,17) reaches a
	 * position in which the emitting build itself found a legal attack. The attacker and
	 * the target therefore arrive from a vendored artifact rather than from a hex this file
	 * picked and would have to re-pick whenever the board moved.
	 */
	static const int32 kAttackIndex = 17;

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

	/**
	 * A destination for `UnitId` that the rules module itself listed: the first entry of
	 * `ReachableHexes` that is neither the unit's own hex nor occupied.
	 *
	 * READ, NOT COMPUTED, and that distinction survives even though this is fixture
	 * construction rather than an expectation. Nothing here decides whether a hex is
	 * reachable; the set is the module's and this picks an element of it. The occupancy
	 * test is the board's own `occupantAt`.
	 */
	static bool PickMoveDestination(const FStratBridge& Bridge, const strat::UiWorld& World,
	                                int32 UnitId, FIntPoint Origin, FIntPoint& OutHex,
	                                FString& OutError)
	{
		TArray<FIntPoint> Hexes;
		TArray<int32>     Costs;
		const FStratResult Asked = Bridge.ReachableHexes(UnitId, Hexes, Costs);
		if (!Asked.bOk)
		{
			OutError = FString::Printf(TEXT("ReachableHexes(%d) refused: %s"), UnitId, *Asked.Reason);
			return false;
		}

		for (const FIntPoint& H : Hexes)
		{
			if (H == Origin)
			{
				continue;
			}
			if (World.board.occupantAt(strat::Hex{ H.X, H.Y }) != strat::OCCUPANT_NONE)
			{
				continue;
			}
			OutHex = H;
			return true;
		}

		OutError = FString::Printf(
			TEXT("unit %d has no reachable hex other than its own that is unoccupied"), UnitId);
		return false;
	}

	/**
	 * Walks both halves of the target-enumeration clause over ONE position, and reports
	 * what it exercised so a vacuous half cannot hide inside a green run.
	 *
	 * Factored out because the clause runs over TWO positions and the walk must be the
	 * same walk on both -- a second copy would be a second, weaker opinion about what the
	 * enumeration promises.
	 */
	static void WalkTargetEnumeration(FAutomationTestBase& Test, const FStratBridge& Bridge,
	                                  const TCHAR* Position, int32& OutIncluded, int32& OutOmitted)
	{
		const strat::UiWorld World = Bridge.MakeUiWorld();

		for (const strat::UiUnit& Attacker : World.units)
		{
			const FString At = FString::Printf(TEXT("%s: attacker %d (side %d) at %s"),
				Position, Attacker.id, Attacker.side, *Describe(Attacker.hex));

			TArray<FIntPoint> Targets;
			const FStratResult Asked = Bridge.AttackTargetHexes(Attacker.id, Targets);
			if (!Test.TestTrue(*FString::Printf(TEXT("%s: AttackTargetHexes answers"), *At), Asked.bOk))
			{
				Test.AddError(FString::Printf(TEXT("%s: %s"), *At, *Asked.Reason));
				continue;
			}

			// ---- INCLUSION ---------------------------------------------------
			for (const FIntPoint& Hex : Targets)
			{
				strat::UiForecast Out;
				const FStratResult Forecast =
					Bridge.Forecast(Attacker.id, strat::Hex{ Hex.X, Hex.Y }, Out);
				if (!Test.TestTrue(*FString::Printf(TEXT("%s: the enumerated hex %s can be forecast"),
						*At, *DescribePoint(Hex)), Forecast.bOk))
				{
					Test.AddError(Forecast.Reason);
					continue;
				}
				Test.TestTrue(*FString::Printf(
						TEXT("T-UI-01: %s: enumerated target %s forecasts legal -- reason '%s'"),
						*At, *DescribePoint(Hex), *FromStd(Out.reason)),
					Out.legal);
				++OutIncluded;
			}

			// ---- OMISSION ----------------------------------------------------
			for (const strat::UiUnit& Defender : World.units)
			{
				if (Defender.side == Attacker.side)
				{
					continue;
				}

				const FIntPoint DefenderHex(Defender.hex.q, Defender.hex.r);
				if (Targets.Contains(DefenderHex))
				{
					continue;
				}

				strat::UiForecast Out;
				const FStratResult Forecast = Bridge.Forecast(Attacker.id, Defender.hex, Out);
				if (!Test.TestTrue(*FString::Printf(TEXT("%s: the omitted enemy hex %s can be forecast"),
						*At, *Describe(Defender.hex)), Forecast.bOk))
				{
					Test.AddError(Forecast.Reason);
					continue;
				}

				Test.TestFalse(*FString::Printf(
						TEXT("T-UI-01: %s: enemy at %s is omitted from the enumeration, so uiForecast "
						     "must call it illegal -- module reason '%s'"),
						*At, *Describe(Defender.hex), *FromStd(Out.reason)),
					Out.legal);
				++OutOmitted;
			}
		}
	}
}

// ---------------------------------------------------------------------------
// T-UI-02 -- the `FIntPoint` façade IS `Reachable`, hex for hex and cost for cost.
//
// WHY THIS CLAUSE IS NOT COVERED BY `T-UI-02.MatchesModuleQuery`. That clause pins
// `FStratBridge::Reachable` against `strat::uiReachable`. It says nothing about the
// engine-typed method beside it, and the engine-typed method is THE ONLY ONE A GAMEPLAY
// CALLER CAN REACH -- `StratPlay` may not spell `std::vector<strat::ReachEntry>`, which
// StratPlay.Build.cs states as a module property. So a translation layer that dropped an
// entry, re-sorted the set, or truncated it would leave every existing T-UI-02 clause
// green while every overlay and every click test in the game was wrong. This is the
// clause that closes that gap, and the expectation side of it is the typed method's own
// answer.
//
// ORDER IS COMPARED, not merely membership. `reachable` returns canonical hex order and
// the parallel `OutCosts` array is only meaningful positionally: a façade that returned
// the same hexes in another order would silently mis-label every cost the moment §2.11's
// move-cost readout is built on it.
//
// EVERY UNIT ON THE BOARD, because the four §2.4 rows have four different `move` values
// and stand on four different terrains, and a translation that mishandled exactly one of
// them is the interesting failure.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeReachableHexesMatchesReachableTest,
	"Stratocracy.StratBridge.T-UI-02.ReachableHexesMatchesReachable",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeReachableHexesMatchesReachableTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSelectionFacadeParity;

	FStratBridge Bridge;
	FString Error;
	const bool bSeeded = SeedBridge(Bridge, Error);
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), bSeeded))
	{
		AddError(Error);
		return false;
	}

	const strat::UiWorld World = Bridge.MakeUiWorld();
	if (!TestTrue(TEXT("the seeded opening puts units on the board"), !World.units.empty()))
	{
		return false;
	}

	int32 TotalEntries = 0;

	for (const strat::UiUnit& U : World.units)
	{
		const FString At = FString::Printf(TEXT("unit %d (side %d) at %s"),
			U.id, U.side, *Describe(U.hex));

		// THE EXPECTATION: the typed method the façade is supposed to be forwarding.
		std::vector<strat::ReachEntry> Typed;
		const FStratResult TypedAsked = Bridge.Reachable(U.id, Typed);
		if (!TestTrue(*FString::Printf(TEXT("%s: Reachable answers"), *At), TypedAsked.bOk))
		{
			AddError(FString::Printf(TEXT("%s: %s"), *At, *TypedAsked.Reason));
			continue;
		}

		TArray<FIntPoint> Hexes;
		TArray<int32>     Costs;
		const FStratResult FacadeAsked = Bridge.ReachableHexes(U.id, Hexes, Costs);
		if (!TestTrue(*FString::Printf(TEXT("%s: ReachableHexes answers"), *At), FacadeAsked.bOk))
		{
			AddError(FString::Printf(TEXT("%s: %s"), *At, *FacadeAsked.Reason));
			continue;
		}

		// The two out-params are parallel BY CONTRACT (StratBridge.h: "OutHexes[i] is
		// reached at OutCosts[i] ... the two are always the same length"), and a caller
		// indexing one by the other's index has no way to notice a violation.
		if (!TestEqual(*FString::Printf(TEXT("%s: the two façade arrays are parallel"), *At),
				Costs.Num(), Hexes.Num()))
		{
			continue;
		}

		if (!TestEqual(
				*FString::Printf(TEXT("%s: the façade forwards every entry, none added or dropped"), *At),
				Hexes.Num(), static_cast<int32>(Typed.size())))
		{
			continue;
		}

		for (int32 i = 0; i < Hexes.Num(); ++i)
		{
			const strat::ReachEntry& T = Typed[static_cast<size_t>(i)];
			const FString Slot = FString::Printf(TEXT("%s: entry %d"), *At, i);

			// X = q and Y = r, the encoding StratBridge.h states and FStratUnitView::Hex
			// already carries. Asserted field by field so a swapped pair -- which is
			// invisible on any symmetric position -- names itself.
			TestEqual(*FString::Printf(TEXT("%s: q survives the conversion (%s vs %s), order included"),
					*Slot, *DescribePoint(Hexes[i]), *Describe(T.hex)),
				Hexes[i].X, T.hex.q);
			TestEqual(*FString::Printf(TEXT("%s: r survives the conversion"), *Slot),
				Hexes[i].Y, T.hex.r);
			TestEqual(*FString::Printf(TEXT("%s: cost is the typed method's, not re-costed"), *Slot),
				Costs[i], T.cost);
		}

		TotalEntries += Hexes.Num();

		// Non-emptiness is `Reachable`'s own guarantee (the null move) and it is restated
		// on the façade because the façade is what a caller sees: an empty answer there is
		// the shape a hex-distance filter would produce for a surrounded unit.
		TestTrue(*FString::Printf(TEXT("%s: a successful façade call is never empty"), *At),
			Hexes.Num() > 0);
	}

	// So the loop above cannot pass by comparing ten empty pairs of arrays.
	TestTrue(TEXT("T-UI-02: the comparison ran over a non-trivial number of entries"),
		TotalEntries > 0);
	AddInfo(FString::Printf(TEXT("T-UI-02: %d reachable entries compared across %d units"),
		TotalEntries, static_cast<int32>(World.units.size())));

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-01 -- the target enumeration adds NO rule of its own, in both directions.
//
// StratBridge.h calls `AttackTargetHexes` "an enumeration, not a range check" and says
// the distinction "is the only reason this is defensible in a file that computes
// nothing": it walks the live unit list and asks `Forecast` about each enemy. That claim
// has two halves and only one of them is interesting on its own.
//
//   INCLUSION -- every hex returned forecasts `legal`. A method that returned extra hexes
//   would let the selection machine submit an attack the rules then refuse.
//
//   OMISSION -- every enemy-occupied hex NOT returned forecasts `legal` false. THIS IS
//   THE HALF THAT PINS THE CLAIM. Inclusion alone is satisfied by a method that returns
//   the empty set on every board forever, and an over-filtering enumeration -- one that
//   quietly dropped a legal target because of a range, a side or a terrain rule it
//   restated locally -- is exactly the substitution T-UI-02 catches for movement. It is
//   also the half that is silent in the game: the player simply never sees the target.
//
// TWO POSITIONS, AND THE SECOND ONE IS NOT OPTIONAL -- MEASURED. Run at the seeded
// opening alone, this clause reported `0 enumerated targets checked, 50 omitted enemy
// hexes checked`: the omission half was at its strongest and THE INCLUSION HALF WAS
// VACUOUS, because Ferrum Crossing deploys the two sides on opposite edges and nothing is
// in range on turn 1 (StratBridgeQueryParity.cpp records the same fact for the same
// reason). A vacuous half in a green run is a half-pinned clause, so the walk runs a
// SECOND time over the fixture's mid-match position -- `Data/parity_fixture.save` replayed
// to index 17, which is where the emitting build itself made an attack. Both counts are
// reported, and the clause FAILS if either half ever reaches zero across both positions.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeAttackTargetsAreForecastLegalTest,
	"Stratocracy.StratBridge.T-UI-01.AttackTargetsAreForecastLegal",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeAttackTargetsAreForecastLegalTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSelectionFacadeParity;

	int32 IncludedChecked = 0;
	int32 OmittedChecked  = 0;

	// ---- position 1: the seeded opening -------------------------------------
	// Every enemy is out of range here, so this position carries the OMISSION half and
	// contributes nothing to the inclusion half. That is measured, not assumed -- see the
	// clause header.
	{
		FStratBridge Bridge;
		FString Error;
		if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
		{
			AddError(Error);
			return false;
		}

		const strat::UiWorld World = Bridge.MakeUiWorld();
		if (!TestTrue(TEXT("the seeded opening puts units on the board"), !World.units.empty()))
		{
			return false;
		}

		int32 Included = 0;
		int32 Omitted  = 0;
		WalkTargetEnumeration(*this, Bridge, TEXT("opening"), Included, Omitted);
		AddInfo(FString::Printf(
			TEXT("T-UI-01 at the seeded opening: %d enumerated targets, %d omitted enemy hexes"),
			Included, Omitted));
		IncludedChecked += Included;
		OmittedChecked  += Omitted;
	}

	// ---- position 2: the fixture's own mid-match position --------------------
	// `Data/parity_fixture.save` replayed to index 17, where the emitting build itself made
	// an attack. The attacker and the target come from a vendored artifact under
	// GATE-DATA-VENDOR rather than from a hex chosen here, so this position moves only when
	// the fixture is re-emitted -- and if that ever stops being an attack, the clause below
	// says so in as many words instead of quietly forecasting something else.
	{
		strat::Save Fixture;
		FString Error;
		if (!TestTrue(TEXT("the committed parity fixture loads and parses"), LoadFixture(Fixture, Error)))
		{
			AddError(Error);
			return false;
		}
		if (!TestTrue(TEXT("the fixture is long enough to supply an attack"),
				static_cast<int32>(Fixture.commandLog.size()) > kAttackIndex))
		{
			return false;
		}

		const strat::SaveCommand& TheAttack = Fixture.commandLog[static_cast<size_t>(kAttackIndex)];
		if (!TestEqual(
				*FString::Printf(TEXT("fixture entry %d is an Attack, which is what makes this position "
				                      "worth replaying to"), kAttackIndex),
				static_cast<int32>(TheAttack.kind), static_cast<int32>(strat::SaveCommandKind::Attack)))
		{
			AddError(TEXT("the fixture was re-emitted and its command order moved; re-pick kAttackIndex "
			              "rather than enumerating targets in whatever position now sits there"));
			return false;
		}

		FStratBridge Bridge;
		if (!TestTrue(TEXT("the bridge reaches the position the fixture's attack was made from"),
				SeedAndReplayPrefix(Bridge, Fixture, kAttackIndex, Error)))
		{
			AddError(Error);
			return false;
		}

		int32 Included = 0;
		int32 Omitted  = 0;
		WalkTargetEnumeration(*this, Bridge, TEXT("fixture prefix"), Included, Omitted);
		AddInfo(FString::Printf(
			TEXT("T-UI-01 at the fixture's turn %d position: %d enumerated targets, %d omitted enemy hexes"),
			TheAttack.turn, Included, Omitted));
		IncludedChecked += Included;
		OmittedChecked  += Omitted;

		// The fixture's own attacker must find its own target in the enumeration. This is the
		// sharpest form of the inclusion half: a target the CREW BUILD committed a command for,
		// which this enumeration must therefore offer.
		TArray<FIntPoint> Targets;
		const FStratResult Asked = Bridge.AttackTargetHexes(TheAttack.unitId, Targets);
		if (TestTrue(TEXT("the fixture's attacker can be asked for its targets"), Asked.bOk))
		{
			TestTrue(
				*FString::Printf(
					TEXT("T-UI-01: the enumeration offers the very target the fixture's own log attacked "
					     "-- unit %d -> %s, %d target(s) offered"),
					TheAttack.unitId, *Describe(TheAttack.hex), Targets.Num()),
				Targets.Contains(FIntPoint(TheAttack.hex.q, TheAttack.hex.r)));
		}
	}

	// The exercise counts, stated rather than assumed. BOTH halves must have run: a green
	// clause with either count at zero is a clause asserting nothing in that direction.
	AddInfo(FString::Printf(
		TEXT("T-UI-01 total: %d enumerated targets checked for legality, %d omitted enemy hexes checked "
		     "for illegality"),
		IncludedChecked, OmittedChecked));
	TestTrue(
		TEXT("T-UI-01: the INCLUSION half ran over at least one enumerated target -- if this fails the "
		     "enumeration returned nothing anywhere and the legality half is vacuous"),
		IncludedChecked > 0);
	TestTrue(
		TEXT("T-UI-01: the OMISSION half ran over at least one omitted enemy hex -- a board that "
		     "offered none would leave that half vacuous, and that is a scenario finding rather than "
		     "a pass"),
		OmittedChecked > 0);

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-06 -- `Turn()` and `SideToMove()` read BEFORE a submission are the values the
// bridge stamps onto the recorded entry.
//
// WHY THIS IS THE CLAUSE THE `STRAT-CMD` LINE RESTS ON. `StratSubmitSelectionCommand`
// reads both before submitting, and StratSelectionMachine.cpp records why in as many
// words: "reading them afterwards would print N+1 for every EndTurn and describe a
// command nobody submitted". That reasoning is only sound if the pre-submission reading
// IS the stamp. Nothing checked that. `Turn()` and `SideToMove()` are new methods; a
// caller has no way to see the stamp at all, because the stamp lives in a `strat::`
// type this module is the only one that can read.
//
// ENDTURN IS THE CASE THAT MATTERS AND IT IS DONE LAST, on purpose. A `Move` is stamped
// with a turn that has not changed, so the clause would pass on a bridge that read the
// tag back afterwards. `EndTurn` is the one command whose before and after differ, and
// the clause asserts BOTH that the stamp equals the before-reading and that the
// after-reading has moved on -- otherwise "before" and "after" would be indistinguishable
// and the assertion would be about nothing.
//
// EVERY NUMBER IS A MODULE-SIDE READING. No turn is incremented here and no side is
// flipped; the "after" values are read from the same two methods.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeTurnAndSideAreReadBeforeStampTest,
	"Stratocracy.StratBridge.T-SAVE-06.TurnAndSideAreReadBeforeStamp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeTurnAndSideAreReadBeforeStampTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSelectionFacadeParity;

	FStratBridge Bridge;
	FString Error;
	const bool bSeeded = SeedBridge(Bridge, Error);
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), bSeeded))
	{
		AddError(Error);
		return false;
	}

	TestEqual(TEXT("a freshly seeded bridge has recorded nothing"),
		static_cast<int32>(Bridge.RecordedLog().size()), 0);

	// ---- a Move: the same turn either side of it ----------------------------
	{
		const strat::UiWorld World = Bridge.MakeUiWorld();
		const strat::UiUnit* Mover = nullptr;
		for (const strat::UiUnit& U : World.units)
		{
			if (U.side == Bridge.SideToMove())
			{
				Mover = &U;
				break;
			}
		}
		if (!TestNotNull(TEXT("the side to move has a unit on the board"), Mover))
		{
			return false;
		}

		FIntPoint Dest;
		if (!TestTrue(TEXT("that unit has somewhere the rules module says it may go"),
				PickMoveDestination(Bridge, World, Mover->id,
					FIntPoint(Mover->hex.q, Mover->hex.r), Dest, Error)))
		{
			AddError(Error);
			return false;
		}

		const int32 TurnBefore = Bridge.Turn();
		const int32 SideBefore = Bridge.SideToMove();

		const FStratResult Applied = Bridge.SubmitMoveToHex(Mover->id, Dest);
		if (!TestTrue(TEXT("the move the module listed is accepted"), Applied.bOk))
		{
			AddError(Applied.Reason);
			return false;
		}

		if (!TestEqual(TEXT("the accepted move was recorded"),
				static_cast<int32>(Bridge.RecordedLog().size()), 1))
		{
			return false;
		}
		const strat::SaveCommand& Entry = Bridge.RecordedLog().back();
		TestEqual(TEXT("T-SAVE-06: the Move entry carries the turn read before submission"),
			Entry.turn, TurnBefore);
		TestEqual(TEXT("T-SAVE-06: the Move entry carries the side read before submission"),
			Entry.side, SideBefore);
		TestEqual(TEXT("T-SAVE-06: it is the Move that was submitted"),
			static_cast<int32>(Entry.kind), static_cast<int32>(strat::SaveCommandKind::Move));
	}

	// ---- an EndTurn: the case where before and after differ -----------------
	{
		const int32 TurnBefore = Bridge.Turn();
		const int32 SideBefore = Bridge.SideToMove();

		const FStratResult Applied = Bridge.SubmitEndTurn();
		if (!TestTrue(TEXT("the end of turn is accepted"), Applied.bOk))
		{
			AddError(Applied.Reason);
			return false;
		}

		const int32 TurnAfter = Bridge.Turn();
		const int32 SideAfter = Bridge.SideToMove();

		// Without this, "before" and "after" could be the same reading and the clause
		// would hold against a bridge that stamped either.
		TestTrue(
			FString::Printf(TEXT("T-SAVE-06: an EndTurn moves the {turn, side} on: {%d, %d} -> {%d, %d}"),
				TurnBefore, SideBefore, TurnAfter, SideAfter),
			(TurnAfter != TurnBefore) || (SideAfter != SideBefore));

		if (!TestEqual(TEXT("the accepted end of turn was recorded"),
				static_cast<int32>(Bridge.RecordedLog().size()), 2))
		{
			return false;
		}
		const strat::SaveCommand& Entry = Bridge.RecordedLog().back();
		TestEqual(TEXT("T-SAVE-06: the command that CLOSES turn N is tagged N, not N+1"),
			Entry.turn, TurnBefore);
		TestEqual(TEXT("T-SAVE-06: and with the side that closed it"),
			Entry.side, SideBefore);
		TestEqual(TEXT("T-SAVE-06: it is the EndTurn that was submitted"),
			static_cast<int32>(Entry.kind), static_cast<int32>(strat::SaveCommandKind::EndTurn));

		AddInfo(FString::Printf(
			TEXT("T-SAVE-06: EndTurn read {turn %d, side %d} before submission, stamped {turn %d, side %d}, "
			     "and the bridge now reads {turn %d, side %d}"),
			TurnBefore, SideBefore, Entry.turn, Entry.side, TurnAfter, SideAfter));
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-05 -- the recorded log IS the accepted set, and replaying it reaches the same
// state. THE MODULE-SIDE HALF OF THE HOT-SEAT REPLAY-PARITY CRITERION.
//
// WHAT THIS HALF OWNS AND WHY IT IS SEPARATE FROM THE CLICK-DRIVEN ONE. The phase-4 exit
// criterion asks for a replay-parity test over a scripted hot-seat sequence. That
// sequence is made of clicks, and clicks are `FStratSelectionMachine`'s, which lives one
// module out. `RecordedLog()` and `ReplayLog` are `strat::`-typed and live HERE. No single
// module can see both today -- StratPlay is barred from naming a `strat::` type and this
// module cannot see StratPlay -- so the criterion is closed by two clauses that meet in
// the middle:
//
//   HERE: `SubmitMoveToHex` and `SubmitEndTurn` -- TWO of the three arms of
//   `StratSubmitSelectionCommand`'s switch, and the sentence says two because the body
//   below submits two -- are recorded exactly once each on acceptance and never on
//   refusal, and the recorded log replayed onto a fresh seed reaches an EQUAL
//   `StateHash()`. The THIRD arm, `SubmitAttackAtHex`, is pinned by
//   `T-UI-01.SubmitAttackAtHexIsNotTransposed` below (the conversion and the recorded
//   entry) and by `T-SAVE-06.RecordedLogMatchesSubmissions` in
//   `StratBridgeSaveRecording.cpp` (the recording); it is deliberately NOT submitted in
//   this clause, because the seeded opening puts nothing in range and reaching an
//   attack-legal position needs the fixture prefix that clause replays.
//
//   THERE (`StratPlay/Tests/StratHotSeatReplayParity.cpp`): a scripted CLICK sequence
//   drives the machine, its accepted commands are counted against
//   `RecordedCommandCount()`, and the recorded log is replayed onto a second bridge
//   through `ReplayRecordedLogOnto`.
//
// THE JOINT BETWEEN THE TWO HALVES -- that `StratSubmitSelectionCommand` calls a
// *recording* entry point at all -- is now that clause's, and it is held by those two
// engine-typed methods rather than by this file. `T-SAVE-05.ReplayRecordedLogOntoRefuses`
// below pins that they are distinguishable from a no-op, which is the only thing that
// makes the other clause's greenness worth anything.
//
// THE REFUSED COMMAND IS LOAD-BEARING. Without one, "the log length equals the accepted
// count" is satisfied by a bridge that recorded every SUBMISSION, and that bridge writes
// a save whose log contains a command the rules module rejected -- which replays to a
// different state or refuses outright on load. So the script contains a submission that
// is refused, and the clause asserts the log did not grow and the hash did not move.
//
// THE SEQUENCE CROSSES TWO TURN BOUNDARIES so both sides move, because a log that never
// changed side would not exercise the stamp that `applyCommand` checks on replay
// (`strat::applyCommand`'s turn-tag guard) and a one-move log proves nothing about ordering.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeRecordedLogReplaysToEqualHashTest,
	"Stratocracy.StratBridge.T-SAVE-05.RecordedLogReplaysToEqualHash",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeRecordedLogReplaysToEqualHashTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSelectionFacadeParity;

	FStratBridge Bridge;
	FString Error;
	const bool bSeeded = SeedBridge(Bridge, Error);
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), bSeeded))
	{
		AddError(Error);
		return false;
	}

	const FString SeedHash = Bridge.StateHash();
	int32 Accepted = 0;

	// Three moves and two ends of turn, all through the engine-typed façade -- the same
	// three methods `StratSubmitSelectionCommand` calls and no others.
	for (int32 Leg = 0; Leg < 3; ++Leg)
	{
		const strat::UiWorld World = Bridge.MakeUiWorld();
		const int32 ActiveSide = Bridge.SideToMove();

		const strat::UiUnit* Mover = nullptr;
		for (const strat::UiUnit& U : World.units)
		{
			if (U.side == ActiveSide)
			{
				Mover = &U;
				break;
			}
		}
		if (!TestNotNull(*FString::Printf(TEXT("leg %d: side %d has a unit"), Leg, ActiveSide), Mover))
		{
			return false;
		}

		FIntPoint Dest;
		if (!TestTrue(*FString::Printf(TEXT("leg %d: unit %d has a listed destination"), Leg, Mover->id),
				PickMoveDestination(Bridge, World, Mover->id,
					FIntPoint(Mover->hex.q, Mover->hex.r), Dest, Error)))
		{
			AddError(Error);
			return false;
		}

		const FStratResult Moved = Bridge.SubmitMoveToHex(Mover->id, Dest);
		if (!TestTrue(*FString::Printf(TEXT("leg %d: the listed move is accepted"), Leg), Moved.bOk))
		{
			AddError(Moved.Reason);
			return false;
		}
		++Accepted;
		AddInfo(FString::Printf(TEXT("leg %d: {turn %d, side %d} unit %d -> %s"),
			Leg, Bridge.Turn(), ActiveSide, Mover->id, *DescribePoint(Dest)));

		// ---- the refusal, on the first leg only ------------------------------
		// A move of a unit belonging to the side that is NOT to move. It is refused by
		// the rules module in the module's own words, and nothing here predicts those
		// words -- only that the submission is refused and leaves no trace.
		if (Leg == 0)
		{
			const strat::UiUnit* Enemy = nullptr;
			for (const strat::UiUnit& U : World.units)
			{
				if (U.side != ActiveSide)
				{
					Enemy = &U;
					break;
				}
			}
			if (TestNotNull(TEXT("the other side has a unit to refuse a command for"), Enemy))
			{
				const int32   LengthBefore = static_cast<int32>(Bridge.RecordedLog().size());
				const FString HashBefore   = Bridge.StateHash();

				const FStratResult Refused =
					Bridge.SubmitMoveToHex(Enemy->id, FIntPoint(Enemy->hex.q, Enemy->hex.r));

				TestFalse(TEXT("T-SAVE-05: a command for the side that is not to move is refused"),
					Refused.bOk);
				TestTrue(TEXT("T-SAVE-05: that refusal carries the rules module's own reason"),
					!Refused.Reason.IsEmpty());
				TestEqual(TEXT("T-SAVE-05: a refused command is NOT recorded"),
					static_cast<int32>(Bridge.RecordedLog().size()), LengthBefore);
				TestEqual(TEXT("T-SAVE-05: and changes nothing (§4.9's no-partial-application)"),
					Bridge.StateHash(), HashBefore);
				AddInfo(FString::Printf(TEXT("refusal: %s"), *Refused.Reason));
			}
		}

		if (Leg < 2)
		{
			const FStratResult Ended = Bridge.SubmitEndTurn();
			if (!TestTrue(*FString::Printf(TEXT("leg %d: the end of turn is accepted"), Leg), Ended.bOk))
			{
				AddError(Ended.Reason);
				return false;
			}
			++Accepted;
		}
	}

	// ---- the count ----------------------------------------------------------
	TestEqual(
		TEXT("T-SAVE-05: the recorded log holds exactly the ACCEPTED commands -- one entry per "
		     "acceptance, none for the refusal"),
		static_cast<int32>(Bridge.RecordedLog().size()), Accepted);

	const FString DrivenHash = Bridge.StateHash();
	TestNotEqual(
		TEXT("T-SAVE-05: the sequence actually moved the state, so an equal hash below is not the "
		     "seed's hash compared with itself"),
		DrivenHash, SeedHash);

	// ---- the replay ---------------------------------------------------------
	FStratBridge Replayed;
	if (!TestTrue(TEXT("a second bridge seeds from the same scenario"),
			SeedBridge(Replayed, Error)))
	{
		AddError(Error);
		return false;
	}
	TestEqual(TEXT("the two seeds agree before either is driven"),
		Replayed.StateHash(), SeedHash);

	TArray<strat::SaveCommand> Log;
	Log.Reserve(static_cast<int32>(Bridge.RecordedLog().size()));
	for (const strat::SaveCommand& C : Bridge.RecordedLog())
	{
		Log.Add(C);
	}

	const FStratResult ReplayResult = Replayed.ReplayLog(Log);
	if (!TestTrue(TEXT("T-SAVE-05: the recorded log replays onto a fresh seed"), ReplayResult.bOk))
	{
		AddError(ReplayResult.Reason);
		return false;
	}

	// THE CLAUSE. Two `strat::canonicalStateHash` readings, neither predicted here.
	TestEqual(
		TEXT("T-SAVE-05: replaying the recorded log reaches an EQUAL canonical state hash"),
		Replayed.StateHash(), DrivenHash);

	// And the replay is itself recorded as the same log, which is what makes
	// save -> load -> save a fixed point (StratBridge.h on ReplayLog).
	TestEqual(TEXT("T-SAVE-05: the replaying bridge records the same number of commands"),
		static_cast<int32>(Replayed.RecordedLog().size()), Accepted);

	AddInfo(FString::Printf(TEXT("T-SAVE-05: %d accepted commands, 1 refused, hash %s on both bridges"),
		Accepted, *DrivenHash));

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-01 -- `SubmitAttackAtHex` converts `FIntPoint` to `strat::Hex` as X = q and
// Y = r, AND THE CLAUSE CAN FAIL IF IT DID NOT.
//
// WHY THIS EXISTS. `SubmitAttackAtHex` had exactly one caller in the tree --
// `StratSelectionMachine.cpp`'s Attack arm -- and no test caller at all, so the one line
// of conversion in it was executed by nothing:
//
//     return SubmitAttack(UnitId, strat::Hex{TargetHex.X, TargetHex.Y});
//
// A transposed `{TargetHex.Y, TargetHex.X}` there ships GREEN: every attack a player
// clicked would be refused by the rules module, the selection machine would dutifully
// report the module's reason, and the whole suite would stay at its count. The Move
// counterpart is implicitly protected because its destination is read out of
// `ReachableHexes` inside a clause that asserts ACCEPTANCE. Attack had no such clause.
//
// WHERE THE COORDINATE COMES FROM, AND WHY IT IS NOT `AttackTargetHexes`'s. The obvious
// shape -- take a hex out of `AttackTargetHexes` and hand it straight back to
// `SubmitAttackAtHex` -- IS THE ONE SHAPE THAT CANNOT CATCH A SWAP, because
// `AttackTargetHexes` builds its `FIntPoint`s with the same X=q convention and a
// transposition in BOTH would cancel exactly. So the target here comes from the vendored
// fixture's own Attack command, as a `strat::Hex` this module reads directly:
// `Data/parity_fixture.save` entry `kAttackIndex`, whose `unit` and `targetHex` were
// chosen by the crew build and are under GATE-DATA-VENDOR.
//
// THE CLAUSE IS THEREFORE FOUR ASSERTIONS AND A FOIL:
//   1. `hex.q != hex.r` on the fixture's target -- WITHOUT THIS NOTHING BELOW COULD
//      DETECT A SWAP, and a fixture re-emission that made them equal would silently turn
//      this clause into decoration. It fails loudly instead.
//   2. the enumeration offers the fixture's target and NOT its transpose, which is the
//      cancellation argument closed from the query side.
//   3. `SubmitAttackAtHex(unit, {q, r})` is ACCEPTED, and the entry it recorded carries
//      `q` in q and `r` in r -- compared field by field against the fixture's own
//      `strat::Hex`, never against a value this file built.
//   4. THE FOIL: on a second bridge at the SAME position, `SubmitAttackAtHex(unit,
//      {r, q})` is REFUSED and records nothing. That is the transposed call spelled out,
//      and it is what makes assertion 3 a measurement rather than a hope.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeSubmitAttackAtHexIsNotTransposedTest,
	"Stratocracy.StratBridge.T-UI-01.SubmitAttackAtHexIsNotTransposed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeSubmitAttackAtHexIsNotTransposedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSelectionFacadeParity;

	strat::Save Fixture;
	FString Error;
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"), LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the fixture is long enough to supply an attack"),
			static_cast<int32>(Fixture.commandLog.size()) > kAttackIndex))
	{
		return false;
	}

	const strat::SaveCommand& TheAttack = Fixture.commandLog[static_cast<size_t>(kAttackIndex)];
	if (!TestEqual(
			*FString::Printf(TEXT("fixture entry %d is an Attack"), kAttackIndex),
			static_cast<int32>(TheAttack.kind), static_cast<int32>(strat::SaveCommandKind::Attack)))
	{
		AddError(TEXT("the fixture was re-emitted and its command order moved; re-pick kAttackIndex"));
		return false;
	}

	// ASSERTION 1. The whole clause rests on this: on a symmetric target a swap is
	// invisible, and a green run over one would be a green run over nothing.
	if (!TestNotEqual(
			*FString::Printf(
				TEXT("T-UI-01: the fixture's target %s has q != r, so a transposed conversion is "
				     "OBSERVABLE -- if this ever fails the clause below is decoration and the fixture "
				     "needs a different attack index"),
				*Describe(TheAttack.hex)),
			TheAttack.hex.q, TheAttack.hex.r))
	{
		return false;
	}

	const FIntPoint Target(TheAttack.hex.q, TheAttack.hex.r);
	const FIntPoint Transposed(TheAttack.hex.r, TheAttack.hex.q);

	// ---- the accepted call --------------------------------------------------
	FStratBridge Bridge;
	if (!TestTrue(TEXT("the bridge reaches the position the fixture's attack was made from"),
			SeedAndReplayPrefix(Bridge, Fixture, kAttackIndex, Error)))
	{
		AddError(Error);
		return false;
	}

	// ASSERTION 2. Both directions, from the query side, so the cancellation argument
	// cannot be made about this position either.
	{
		TArray<FIntPoint> Targets;
		const FStratResult Asked = Bridge.AttackTargetHexes(TheAttack.unitId, Targets);
		if (TestTrue(TEXT("the fixture's attacker can be asked for its targets"), Asked.bOk))
		{
			TestTrue(*FString::Printf(
					TEXT("T-UI-01: the enumeration offers the fixture's own target %s"), *Describe(TheAttack.hex)),
				Targets.Contains(Target));
			TestFalse(*FString::Printf(
					TEXT("T-UI-01: and does NOT offer its transpose %s -- so a swap on both sides of the "
					     "conversion could not cancel"), *DescribePoint(Transposed)),
				Targets.Contains(Transposed));
		}
	}

	const int32   CountBefore = Bridge.RecordedCommandCount();
	const FString HashBefore  = Bridge.StateHash();

	// ASSERTION 3.
	const FStratResult Attacked = Bridge.SubmitAttackAtHex(TheAttack.unitId, Target);
	if (!TestTrue(
			*FString::Printf(
				TEXT("T-UI-01: SubmitAttackAtHex(%d, %s) is ACCEPTED -- a transposed conversion would be "
				     "refused here and the whole suite would stay green without this clause"),
				TheAttack.unitId, *DescribePoint(Target)),
			Attacked.bOk))
	{
		AddError(Attacked.Reason);
		return false;
	}

	if (!TestEqual(TEXT("T-UI-01: the accepted attack was recorded, exactly once"),
			Bridge.RecordedCommandCount(), CountBefore + 1))
	{
		return false;
	}
	TestNotEqual(TEXT("T-UI-01: and it moved the state, so 'accepted' is not a no-op"),
		Bridge.StateHash(), HashBefore);

	const strat::SaveCommand& Entry = Bridge.RecordedLog().back();
	TestEqual(TEXT("T-UI-01: the recorded entry is an Attack"),
		static_cast<int32>(Entry.kind), static_cast<int32>(strat::SaveCommandKind::Attack));
	TestEqual(TEXT("T-UI-01: for the unit that was submitted"), Entry.unitId, TheAttack.unitId);
	TestEqual(
		*FString::Printf(TEXT("T-UI-01: FIntPoint::X arrived as q (%s recorded, fixture says %s)"),
			*Describe(Entry.hex), *Describe(TheAttack.hex)),
		Entry.hex.q, TheAttack.hex.q);
	TestEqual(TEXT("T-UI-01: and FIntPoint::Y arrived as r"), Entry.hex.r, TheAttack.hex.r);

	// ---- ASSERTION 4: THE FOIL ----------------------------------------------
	// The transposed call, spelled out, on a second bridge at the same position. This is
	// what `SubmitAttackAtHex` would DO if its one line were `{TargetHex.Y, TargetHex.X}`,
	// and the rules module must say no to it.
	{
		FStratBridge Foil;
		if (!TestTrue(TEXT("a second bridge reaches the same position"),
				SeedAndReplayPrefix(Foil, Fixture, kAttackIndex, Error)))
		{
			AddError(Error);
			return false;
		}

		const int32   FoilCountBefore = Foil.RecordedCommandCount();
		const FString FoilHashBefore  = Foil.StateHash();

		const FStratResult Refused = Foil.SubmitAttackAtHex(TheAttack.unitId, Transposed);
		TestFalse(
			*FString::Printf(
				TEXT("T-UI-01: the TRANSPOSED target %s is refused -- this is the failure a swapped "
				     "conversion would produce on every attack in the game, and it must be visible"),
				*DescribePoint(Transposed)),
			Refused.bOk);
		TestFalse(TEXT("T-UI-01: and the refusal carries the rules module's own reason"),
			Refused.Reason.IsEmpty());
		TestEqual(TEXT("T-UI-01: a refused attack is not recorded"),
			Foil.RecordedCommandCount(), FoilCountBefore);
		TestEqual(TEXT("T-UI-01: and changes nothing"), Foil.StateHash(), FoilHashBefore);
		AddInfo(FString::Printf(TEXT("T-UI-01 foil refusal: %s"), *Refused.Reason));
	}

	AddInfo(FString::Printf(
		TEXT("T-UI-01: unit %d attacked %s (accepted, recorded) and %s (refused)"),
		TheAttack.unitId, *DescribePoint(Target), *DescribePoint(Transposed)));

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-05 -- `ReplayRecordedLogOnto` REFUSES the six inputs that would otherwise let a
// caller pass vacuously, and each refusal is distinguishable from every other.
//
// WHY A REFUSAL CLAUSE IS THE LOAD-BEARING ONE HERE. This method exists so
// `StratPlay/Tests/StratHotSeatReplayParity.cpp` can hold both ends of the recording
// joint. That clause's value is entirely a function of this method being IMPOSSIBLE TO
// SATISFY TRIVIALLY: an empty log replays to `Ok()` and to an equal hash on any target
// seeded from the same scenario, so a version of this method that quietly no-opped would
// make the gating clause green in exactly the world it was written to catch -- one where
// `StratSubmitSelectionCommand` never recorded anything. Arm 5 below is that case, and it
// is asserted alongside the DEMONSTRATION that the hashes would have matched.
//
// THE REASONS ARE COMPARED FOR DISTINCTNESS, NOT TRANSCRIBED. This file does not restate
// the six wordings: a clause that copied the production strings would pass against any
// mistake in them and would break on a harmless rewording. What it asserts is the property
// that actually matters at a call site -- every refusal is non-empty, and no two of the six
// are the same string, so a caller that logs the reason can always tell which one it hit.
//
// ARM 6 NEEDS A SECOND SCENARIO AND THERE IS ONLY ONE IN `Data/`. It is built here, in
// `Saved/`, by taking the shipped scenario's own bytes and changing `scenarioId` while
// DROPPING the declared `scenarioHash` -- which `Scenario.h` calls "the one OPTIONAL field
// ... DERIVED from the others", and `loadScenarioFile` only checks when the file declares
// one. Nothing under `Data/` is touched, nothing is recomputed here, and the resulting
// scenario differs from the shipped one in a field `strat::scenarioHash` demonstrably
// hashes (`strat::scenarioHash`, which feeds `id=` into the digest). The clause asserts the
// two hashes differ before
// it asserts the refusal, so a variant that failed to be a variant is a failure and not a
// pass.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeReplayRecordedLogOntoRefusesTest,
	"Stratocracy.StratBridge.T-SAVE-05.ReplayRecordedLogOntoRefuses",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeReplayRecordedLogOntoRefusesTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSelectionFacadeParity;

	TArray<FString> Reasons;

	// A driven source: one accepted move, so its log is non-empty for the arms that are
	// not about emptiness.
	FStratBridge Source;
	FString Error;
	if (!TestTrue(TEXT("the source bridge seeds from the shipped scenario"), SeedBridge(Source, Error)))
	{
		AddError(Error);
		return false;
	}
	{
		const strat::UiWorld World = Source.MakeUiWorld();
		const strat::UiUnit* Mover = nullptr;
		for (const strat::UiUnit& U : World.units)
		{
			if (U.side == Source.SideToMove())
			{
				Mover = &U;
				break;
			}
		}
		if (!TestNotNull(TEXT("the side to move has a unit"), Mover))
		{
			return false;
		}
		FIntPoint Dest;
		if (!TestTrue(TEXT("that unit has a listed destination"),
				PickMoveDestination(Source, World, Mover->id,
					FIntPoint(Mover->hex.q, Mover->hex.r), Dest, Error)))
		{
			AddError(Error);
			return false;
		}
		const FStratResult Moved = Source.SubmitMoveToHex(Mover->id, Dest);
		if (!TestTrue(TEXT("the listed move is accepted"), Moved.bOk))
		{
			AddError(Moved.Reason);
			return false;
		}
	}
	if (!TestEqual(TEXT("so the source has exactly one recorded command"),
			Source.RecordedCommandCount(), 1))
	{
		return false;
	}

	// ---- arm 1: the target is this bridge -----------------------------------
	{
		const FStratResult R = Source.ReplayRecordedLogOnto(Source);
		TestFalse(TEXT("T-SAVE-05: replaying a log onto its own recorder is refused"), R.bOk);
		TestFalse(TEXT("T-SAVE-05: with a reason"), R.Reason.IsEmpty());
		TestEqual(TEXT("T-SAVE-05: and the source's log did not grow"), Source.RecordedCommandCount(), 1);
		Reasons.Add(R.Reason);
	}

	// ---- arm 2: the SOURCE is unseeded --------------------------------------
	{
		FStratBridge Unseeded;
		FStratBridge Target;
		if (!TestTrue(TEXT("a target seeds"), SeedBridge(Target, Error)))
		{
			AddError(Error);
			return false;
		}
		const FStratResult R = Unseeded.ReplayRecordedLogOnto(Target);
		TestFalse(TEXT("T-SAVE-05: an unseeded source has no match to replay, and says so"), R.bOk);
		TestFalse(TEXT("T-SAVE-05: with a reason"), R.Reason.IsEmpty());
		Reasons.Add(R.Reason);
	}

	// ---- arm 3: the TARGET is unseeded --------------------------------------
	{
		FStratBridge Unseeded;
		const FStratResult R = Source.ReplayRecordedLogOnto(Unseeded);
		TestFalse(TEXT("T-SAVE-05: an unseeded target is refused"), R.bOk);
		TestFalse(TEXT("T-SAVE-05: with a reason"), R.Reason.IsEmpty());
		Reasons.Add(R.Reason);
	}

	// ---- arm 4: the target is not fresh -------------------------------------
	// The sharp one. A target that has already played is a hash comparison that means
	// nothing while looking like it means something.
	{
		FStratBridge Played;
		if (!TestTrue(TEXT("a second bridge seeds"), SeedBridge(Played, Error)))
		{
			AddError(Error);
			return false;
		}
		const strat::UiWorld World = Played.MakeUiWorld();
		const strat::UiUnit* Mover = nullptr;
		for (const strat::UiUnit& U : World.units)
		{
			if (U.side == Played.SideToMove())
			{
				Mover = &U;
				break;
			}
		}
		if (!TestNotNull(TEXT("it has a unit to move"), Mover))
		{
			return false;
		}
		FIntPoint Dest;
		if (!TestTrue(TEXT("with a listed destination"),
				PickMoveDestination(Played, World, Mover->id,
					FIntPoint(Mover->hex.q, Mover->hex.r), Dest, Error)))
		{
			AddError(Error);
			return false;
		}
		const FStratResult Moved = Played.SubmitMoveToHex(Mover->id, Dest);
		if (!TestTrue(TEXT("which is accepted, so the target is no longer fresh"), Moved.bOk))
		{
			AddError(Moved.Reason);
			return false;
		}

		const FString HashBefore = Played.StateHash();
		const FStratResult R = Source.ReplayRecordedLogOnto(Played);
		TestFalse(TEXT("T-SAVE-05: a target that has already applied commands is refused"), R.bOk);
		TestFalse(TEXT("T-SAVE-05: with a reason"), R.Reason.IsEmpty());
		TestEqual(TEXT("T-SAVE-05: and nothing was applied to it -- the refusal is not partial"),
			Played.StateHash(), HashBefore);
		TestEqual(TEXT("T-SAVE-05: nor appended to its log"), Played.RecordedCommandCount(), 1);
		Reasons.Add(R.Reason);
	}

	// ---- arm 5: THE EMPTY LOG -----------------------------------------------
	// The arm the method exists for. The clause asserts the refusal AND demonstrates the
	// vacuous success it replaces: two seeded bridges already hash equal, so a silent
	// `Ok()` here would satisfy any hash comparison a caller then made.
	{
		FStratBridge EmptySource;
		FStratBridge Target;
		if (!TestTrue(TEXT("an undriven source seeds"), SeedBridge(EmptySource, Error)))
		{
			AddError(Error);
			return false;
		}
		if (!TestTrue(TEXT("and a fresh target seeds"), SeedBridge(Target, Error)))
		{
			AddError(Error);
			return false;
		}

		TestEqual(TEXT("T-SAVE-05: the undriven source has recorded nothing"),
			EmptySource.RecordedCommandCount(), 0);
		TestEqual(
			TEXT("T-SAVE-05: and the two bridges ALREADY hash equal -- which is exactly why an empty "
			     "log must not be replayed silently: the hash comparison a caller would then make is "
			     "true before the call and proves nothing about any recording"),
			Target.StateHash(), EmptySource.StateHash());

		const FStratResult R = EmptySource.ReplayRecordedLogOnto(Target);
		TestFalse(TEXT("T-SAVE-05: an EMPTY recorded log is REFUSED rather than replayed"), R.bOk);
		TestFalse(TEXT("T-SAVE-05: with a reason"), R.Reason.IsEmpty());
		TestEqual(TEXT("T-SAVE-05: and the target is untouched"), Target.RecordedCommandCount(), 0);
		Reasons.Add(R.Reason);
	}

	// ---- arm 6: a target seeded from a DIFFERENT scenario --------------------
	{
		const FString ShippedPath = FPaths::Combine(DataDir(), TEXT("ferrum_crossing.json"));
		FString Text;
		if (!TestTrue(TEXT("the shipped scenario's bytes are readable"),
				FFileHelper::LoadFileToString(Text, *ShippedPath)))
		{
			return false;
		}

		// The shipped id, and the declared hash line, both come out. `Data/` is not
		// touched: the variant is written under `Saved/`.
		TArray<FString> Lines;
		Text.ParseIntoArrayLines(Lines, /*InCullEmpty*/ false);
		TArray<FString> Kept;
		bool bDroppedHash = false;
		bool bChangedId   = false;
		for (const FString& Line : Lines)
		{
			if (Line.Contains(TEXT("\"scenarioHash\"")))
			{
				bDroppedHash = true;
				continue;
			}
			if (Line.Contains(TEXT("\"scenarioId\"")))
			{
				Kept.Add(TEXT("  \"scenarioId\": \"ferrum_crossing_variant\","));
				bChangedId = true;
				continue;
			}
			Kept.Add(Line);
		}
		if (!TestTrue(TEXT("the shipped scenario declares an id and a hash, both of which this "
		                   "fixture edits"), bDroppedHash && bChangedId))
		{
			return false;
		}

		const FString VariantPath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("StratTests"),
			TEXT("ferrum_crossing_variant.json"));
		if (!TestTrue(TEXT("the variant scenario is written under Saved/"),
				FFileHelper::SaveStringToFile(FString::Join(Kept, TEXT("\n")), *VariantPath)))
		{
			return false;
		}

		FStratBridge Variant;
		UDataTable* Units = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		UDataTable* Terrain = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
		if (!TestTrue(TEXT("the shipped tables load"), Units != nullptr && Terrain != nullptr))
		{
			return false;
		}
		const FStratResult Loaded = Variant.LoadDefinitions(Units, Terrain);
		if (!TestTrue(TEXT("definitions load onto the variant bridge"), Loaded.bOk))
		{
			AddError(Loaded.Reason);
			return false;
		}
		const FStratResult Seeded = Variant.LoadScenarioFromFile(VariantPath, kFirstSide);
		if (!TestTrue(TEXT("the variant scenario seeds"), Seeded.bOk))
		{
			AddError(Seeded.Reason);
			return false;
		}

		// The variant IS a variant -- asserted against the module's own digest, not assumed
		// from the edit.
		const std::string MineHash    = strat::scenarioHash(Source.ScenarioData());
		const std::string VariantHash = strat::scenarioHash(Variant.ScenarioData());
		if (!TestNotEqual(
				TEXT("T-SAVE-05: the variant hashes differently under strat::scenarioHash, so the arm "
				     "below is about a real mismatch"),
				FromStd(VariantHash), FromStd(MineHash)))
		{
			return false;
		}

		const FStratResult R = Source.ReplayRecordedLogOnto(Variant);
		TestFalse(TEXT("T-SAVE-05: a target seeded from a different scenario is refused"), R.bOk);
		TestFalse(TEXT("T-SAVE-05: with a reason"), R.Reason.IsEmpty());
		TestEqual(TEXT("T-SAVE-05: and nothing was applied to it"), Variant.RecordedCommandCount(), 0);
		TestEqual(
			TEXT("T-SAVE-05: the refusal carries the TARGET's scenario hash in Id, so a caller can say "
			     "which scenario it actually had"),
			R.Id, FromStd(VariantHash));
		Reasons.Add(R.Reason);
	}

	// ---- the refusals are told apart ----------------------------------------
	if (!TestEqual(TEXT("T-SAVE-05: all six refusal arms ran"), Reasons.Num(), 6))
	{
		return false;
	}
	for (int32 i = 0; i < Reasons.Num(); ++i)
	{
		AddInfo(FString::Printf(TEXT("T-SAVE-05 refusal %d: %s"), i + 1, *Reasons[i]));
		for (int32 j = i + 1; j < Reasons.Num(); ++j)
		{
			TestNotEqual(
				*FString::Printf(
					TEXT("T-SAVE-05: refusal %d and refusal %d are DIFFERENT strings, so a caller that "
					     "logs the reason can tell which guard it hit"),
					i + 1, j + 1),
				Reasons[i], Reasons[j]);
		}
	}

	// ---- and the successful path is still reachable --------------------------
	// Without this the clause could be satisfied by a method that refused everything.
	{
		FStratBridge Fresh;
		if (!TestTrue(TEXT("a fresh target seeds"), SeedBridge(Fresh, Error)))
		{
			AddError(Error);
			return false;
		}
		const FStratResult R = Source.ReplayRecordedLogOnto(Fresh);
		if (!TestTrue(TEXT("T-SAVE-05: a legitimate replay is ACCEPTED -- the six arms above are guards "
		                   "and not a refusal of everything"), R.bOk))
		{
			AddError(R.Reason);
			return false;
		}
		TestEqual(TEXT("T-SAVE-05: and it reaches an equal canonical state hash"),
			Fresh.StateHash(), Source.StateHash());
		TestEqual(TEXT("T-SAVE-05: leaving the target recording what it replayed"),
			Fresh.RecordedCommandCount(), Source.RecordedCommandCount());
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-05 -- the five command-kind words `StratSelectionMachine.cpp` writes into every
// `STRAT-CMD` line ARE the save format's own spellings, read from the format's own
// function.
//
// THE LAST LITERAL EXPECTATION IN THE `STRAT-CMD` CHAIN, AND THIS IS WHERE IT GOES AWAY.
// `StratPlay`'s `CommandKindName` is a second spelling of `strat::saveCommandName`, and it
// has to be: that function carries no `_API` macro, so `StratPlay` cannot link a call to it
// (8 x LNK2019, `StratBridge.h`'s opening measurement), and there is nothing for an
// `FStratBridge` method to route because the switch is over `EStratSelectionCommand`, a
// StratPlay enum the bridge has no reason to know. The function is NOT file-local, though
// -- it is declared as `strat::saveCommandName` in `Source/StratRules/Save.h` and defined in `Save.good.cpp` at
// namespace scope with no `static`, so it has external linkage and THIS module can call it
// directly. That is what this clause does.
//
// WHICH SIDE IS THE EXPECTATION. `strat::saveCommandName` is. The five string literals
// below are a TRANSCRIPTION of `StratSelectionMachine.cpp`'s `CommandKindName` -- the code
// under test -- not of the format, and the clause fails if the format's spelling and the
// transcription ever disagree. A reader changing one must change the other, which is the
// whole point: the phase-6 gate compares a `STRAT-CMD accepted` line against a `commandLog`
// entry with no translation table between them, and a drift in either spelling breaks that
// comparison silently from the far side of a milestone boundary.
//
// ALL FIVE, NOT THE THREE `StratSelectionMachine` EMITS. `Build` and `Capture` have no
// selection-machine arm today, and they are asserted anyway: the day one lands, the word it
// must use is already pinned, and the five-way distinctness assertion below is what makes
// "the format spells these five things differently" a property rather than an accident.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeCommandKindNamesMatchFormatTest,
	"Stratocracy.StratBridge.T-SAVE-05.CommandKindNamesMatchFormat",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeCommandKindNamesMatchFormatTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSelectionFacadeParity;

	struct FKindCase
	{
		strat::SaveCommandKind Kind;
		const TCHAR*           SpelledInStratPlay;
	};

	// The right-hand column is transcribed from `StratSelectionMachine.cpp`'s
	// `CommandKindName` and from `StratHotSeatReplayParity.cpp`'s two `kind=` assertions.
	// The left-hand column is the format's own enumerator; the expectation is what
	// `strat::saveCommandName` returns for it.
	const FKindCase Cases[] = {
		{ strat::SaveCommandKind::Move,    TEXT("Move")    },
		{ strat::SaveCommandKind::Attack,  TEXT("Attack")  },
		{ strat::SaveCommandKind::Build,   TEXT("Build")   },
		{ strat::SaveCommandKind::Capture, TEXT("Capture") },
		{ strat::SaveCommandKind::EndTurn, TEXT("EndTurn") },
	};

	TArray<FString> Names;
	for (const FKindCase& C : Cases)
	{
		// THE MODULE-SIDE VALUE, called directly. Legal here and nowhere else in the tree.
		const FString FromFormat = FString(UTF8_TO_TCHAR(strat::saveCommandName(C.Kind)));
		TestEqual(
			*FString::Printf(
				TEXT("T-SAVE-05: the save format spells this kind `%s`, which is the word "
				     "StratSelectionMachine.cpp's CommandKindName writes into a STRAT-CMD line"),
				C.SpelledInStratPlay),
			FromFormat, FString(C.SpelledInStratPlay));
		Names.Add(FromFormat);
	}

	// Five different words. A format that spelled two kinds the same would make a phase-6
	// gate's `kind=` field ambiguous, and the clause above would not notice.
	for (int32 i = 0; i < Names.Num(); ++i)
	{
		TestFalse(*FString::Printf(TEXT("T-SAVE-05: kind %d has a non-empty spelling"), i),
			Names[i].IsEmpty());
		for (int32 j = i + 1; j < Names.Num(); ++j)
		{
			TestNotEqual(
				*FString::Printf(TEXT("T-SAVE-05: kinds %d and %d are spelled differently"), i, j),
				Names[i], Names[j]);
		}
	}

	// And the fixture's own log uses these words, which is the chain's other end: the
	// parser that read `Data/parity_fixture.save` turned those spellings into these
	// enumerators.
	{
		strat::Save Fixture;
		FString Error;
		if (TestTrue(TEXT("the committed parity fixture loads and parses"), LoadFixture(Fixture, Error)))
		{
			TSet<FString> Seen;
			for (const strat::SaveCommand& C : Fixture.commandLog)
			{
				Seen.Add(FString(UTF8_TO_TCHAR(strat::saveCommandName(C.kind))));
			}
			TestTrue(TEXT("T-SAVE-05: the fixture's log exercises more than one kind"), Seen.Num() > 1);
			AddInfo(FString::Printf(TEXT("T-SAVE-05: fixture kinds present: %d of 5"), Seen.Num()));
		}
		else
		{
			AddError(Error);
		}
	}

	return true;
}
