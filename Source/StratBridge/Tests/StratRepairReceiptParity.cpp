// The measurement half of §2.11.6's repair receipt -- GDD §2.7's start-of-turn repair, as
// `FStratBridge::RepairsAtTurnOpen` reports it.
//
// WHY THE ID IS `T-TURN-08` AND NOT `T-UI-02`, `T-UI-03` OR `T-TURN-09`. Two lists of
// proposed names reached this lane and they disagreed with each other, so neither was taken
// as authority and §4.11's own text decided it instead. §4.11 states T-TURN-08 as: "repair
// fires at the start of the unit's turn exactly when the verified repairAmount says so
// (owned Town/Factory, no adjacent enemy, +25% max HP floored, min 1, capped -> T-REPAIR-01..07
// @ 5ffa8d6); this gate asserts the turn loop calls it at the right moment with the right
// board facts, nothing more." THAT SENTENCE IS THIS FILE. The alternatives are each a
// different subject: T-UI-02 is "the reachable-hex highlight displays exactly the T-MOVE-01
// set", T-UI-03 is the scoreboard's 1:1 binding with no widget-side arithmetic, and T-TURN-09
// is determinism -- "the same command sequence from the same scenario -> identical result tier
// and identical state at every step". Nothing below asserts determinism, a highlight or a
// scoreboard. NO ACCEPTANCE ID IS MINTED HERE; T-TURN-08 is reassigned to the clauses that
// actually carry it, on `T-SCN-07`'s precedent, and this is its first use in this repository.
//
// (`strat::applyStartOfTurnRepair`'s own doc line cites BOTH T-TURN-08 and T-TURN-09 for the
// ascending-id order, so `FStratBridge::RepairsAtTurnOpen`'s "(T-TURN-09)" is half of a real
// citation and not an error. The ordering clause below files under T-TURN-08 because the order
// is the only thing it reads and the whole file already sits there.)
//
// WHERE EVERY EXPECTATION COMES FROM, which is the only column that matters.
//
//   - THE ORACLE IS `strat::repairAmount`, CALLED HERE. This module is one of the two
//     directories in the tree where a `strat::` free function may be called at all, which is
//     the whole reason this file is in `Source/StratBridge/Tests/` and not beside the receipt
//     clauses in `Source/StratUI/Tests/`. Putting it there would be `LNK2019`, not a test.
//     The bridge measures an HP RISE across one `applyCommand`; this file asks the rules
//     module what §2.7 owed that unit and requires the two to be the same number. Without
//     that comparison the bridge's `Amount` is only "some HP moved", which is exactly what
//     `FStratRepairApplication`'s own block says the bracket is supposed to rule out -- and a
//     clause that checked `Amount == HpAfter - HpBefore` would be checking the struct against
//     itself.
//
//   - THE TWO BOARD FACTS ARE READ OFF THE MODULE'S OWN PROJECTION, never off this file's
//     idea of the board. `onOwnedObjective` is `strat::UiHexView::owner == unit.side` at the
//     unit's hex -- `UiHexView::owner` mirrors the objective's owner and reads
//     `OWNER_NEUTRAL` on every hex that is not capturable, so that one comparison is exactly
//     `openTurn`'s `(o != nullptr && o->owner == u.side)` with no second opinion about which
//     tiles are objectives. `enemyAdjacent` is `strat::hexDistance(...) == 1` against every
//     other unit of another side, which is `Replay.good.cpp`'s `enemyAdjacent` with its
//     in-bounds filter dropped -- a filter that can change no answer, because every unit ON
//     the board is in bounds. Both are read from the snapshot taken BEFORE the submit, and
//     that is sound rather than convenient: inside `applyCommand`'s `EndTurn` arm nothing
//     between the previous projection and the repair loop moves a unit or an objective's
//     owner (`endTurn` and `beginTurn` move `result`, `phase`, the two flag sets and
//     `builtThisTurn`; `accrueIncome` and `captureTick` both run AFTER the repair loop).
//
//   - `strat::Unit`'S OTHER FIELDS ARE NOT SOURCED AND DO NOT NEED TO BE, stated rather than
//     left to be discovered. `repairAmount` reads `u.hp` and `u.hpMax` and no other member
//     (`Combat.good.cpp`, the whole body of the function). Both of those come off
//     `strat::UiUnitView`. Every remaining field is left at its declared default and a
//     re-vendor that made one of them load-bearing would break this file loudly, at the
//     oracle, which is the right place for it to break.
//
//   - THE MOMENT IS SEARCHED FOR AND REACHED, NOT WRITTEN DOWN.
//     `DriveToARepairingTurnOpen` replays `Data/parity_fixture.save` one command at a time
//     through `FStratBridge::Submit` -- which is what produces the DAMAGE -- and then walks a
//     damaged unit onto an objective its own side holds, using the rules module's own
//     `Reachable` for the destination set and the projection's own fields for the two board
//     facts. A hardcoded command index and a hand-picked hex would both be this file choosing
//     the position it then asserts on; a re-emitted fixture would silently move the first and
//     invalidate the second. THE FIXTURE'S OWN SCRIPT REACHES NO REPAIR AT ALL -- measured,
//     12 openings, none -- so this is not a convenience; that whole argument is on the
//     function. WHEN NO SUCH MOMENT CAN BE REACHED THE CLAUSE FAILS AND SAYS SO; it never
//     passes on an empty answer.
//
//   - `ReplayLog` IS DELIBERATELY NOT THE REPLAY VEHICLE HERE, which is the opposite of that
//     file's choice and for the reason `FStratBridge::ReplayLog`'s own block gives: it
//     crosses every turn boundary inside one `strat::replayLog` call and CLEARS the repair
//     record rather than filling it. A prefix replayed that way would leave nothing to
//     measure. One `Submit` per command is the only route that observes.
//
// FALSIFIABILITY, MEASURED AND NOT ASSUMED -- see this file's entry in
// `Tools/architect/state/tests.md` for the mutants that were built and what each reddened.
//
// WHAT THIS FILE DOES NOT PIN, stated so nobody reads it as covering them.
//   - A BLOCKED REPAIR. `FStratRepairApplication` records that the record is positive-case
//     only: an absent entry cannot be told apart from full HP, from a unit off an owned
//     objective, and from §2.7's anti-fortress lock firing. No clause below asserts anything
//     about a unit with no entry, and one that did would be asserting a discrimination the
//     code deliberately does not have.
//   - THE FIRST TURN OF A MATCH. `strat::seedFromScenario` opens it, not `Submit`, so there
//     is no bracket -- the method's own block says so. Nothing here claims otherwise.
//   - WHETHER `repairAmount`'S TABLE IS §2.7'S. That is T-REPAIR-01..07's, upstream, at the
//     `rulesCommit` `Source/StratRules/StratRules.manifest.json` records. This file asserts
//     that the bridge reports what that function returns, not that the function is right.
//   - THE RECEIPT, THE VIEW MIRROR AND THE DRAW. Those are
//     `Source/StratUI/Tests/StratTransientReceiptClauses.cpp` and
//     `Source/StratPlay/Tests/StratBuildPulseDrawClauses.cpp`.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include <string>

#include "StratBridge.h"

// The vendored headers, by short name through StratRules' PublicIncludePaths. Named
// explicitly because this file calls `strat::repairAmount` and `strat::hexDistance` and reads
// `strat::UiHexView` directly; a reader should not have to find that through a transitive
// include.
#include "Combat.h"
#include "Hex.h"
#include "Save.h"
#include "Ui.h"

namespace StratRepairReceiptParity
{
	/** Must equal `StratBridgeSaveRecording.cpp`'s and `StratCombatOutcomeParity.cpp`'s own
	 *  `kFirstSide`. The fixture's log was emitted against side 0; a different value here
	 *  replays a different game. `Replay.h` states no rule in this project decides it. */
	static const int32 kFirstSide = 0;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static FString ScenarioPath()
	{
		return FPaths::Combine(DataDir(), TEXT("ferrum_crossing.json"));
	}

	static std::string ToStd(const FString& In)
	{
		return std::string(TCHAR_TO_UTF8(*In));
	}

	static FString FromStd(const std::string& In)
	{
		return FString(UTF8_TO_TCHAR(In.c_str()));
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** Brings a bridge to the seeded opening. Duplicated from the other bridge parity files
	 *  rather than shared, which is this project's standing posture for this helper -- sharing
	 *  would put a header inside a `Tests/` directory. */
	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		UDataTable* Units   = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
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

		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath(), kFirstSide);
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

	/**
	 * A turn opening at which at least one unit healed, and everything needed to check it.
	 *
	 * `Before` is the projection taken immediately BEFORE the `EndTurn` that opened the turn
	 * -- the board facts §2.7's repair was decided against. `After` is the projection taken
	 * immediately after it. `CommandIndex` is where in the fixture's log the `EndTurn` sat,
	 * carried only so a red run can name the position rather than as an expectation.
	 */
	struct FRepairMoment
	{
		bool                            bFound = false;
		int32                           CommandIndex = INDEX_NONE;
		/** What was done to reach the position, in words, so a red run names it rather than
		 *  only failing. Carries no expectation. */
		FString                         Description;
		strat::UiSnapshot               Before;
		strat::UiSnapshot               After;
		TArray<FStratRepairApplication> Repairs;
	};

	/** `strat::UiUnitView` for an id, off a snapshot, or null. */
	static const strat::UiUnitView* UnitIn(const strat::UiSnapshot& Snapshot, int32 UnitId)
	{
		for (const strat::UiUnitView& U : Snapshot.units)
		{
			if (static_cast<int32>(U.id) == UnitId)
			{
				return &U;
			}
		}
		return nullptr;
	}

	/**
	 * `openTurn`'s `onOwnedObjective`, read off the projection.
	 *
	 * `strat::UiHexView::owner` MIRRORS the objective's owner and is `OWNER_NEUTRAL` on every
	 * hex that carries no objective, so "the owner equals this unit's side" is true exactly
	 * where `findObjective` returns a record owned by that side. No list of which tiles are
	 * capturable is written in this file, which is the point.
	 */
	static bool OnOwnedObjective(const strat::UiSnapshot& Snapshot, const strat::UiUnitView& Unit)
	{
		for (const strat::UiHexView& H : Snapshot.hexes)
		{
			if (strat::hexEqual(H.hex, Unit.hex))
			{
				return H.owner == Unit.side;
			}
		}
		return false;
	}

	/** `Replay.good.cpp`'s `enemyAdjacent`, over the projection's own unit list. The in-bounds
	 *  filter that function applies to its neighbour ring is dropped because it can change no
	 *  answer: every unit in the snapshot is on the board. */
	static bool EnemyAdjacent(const strat::UiSnapshot& Snapshot, const strat::UiUnitView& Unit)
	{
		for (const strat::UiUnitView& Other : Snapshot.units)
		{
			if (Other.side != Unit.side && strat::hexDistance(Other.hex, Unit.hex) == 1)
			{
				return true;
			}
		}
		return false;
	}

	/**
	 * Seeds, replays the fixture, and drives the match to a turn opening at which §2.7 repairs
	 * somebody -- leaving `Bridge` standing at exactly that moment.
	 *
	 * THE FIXTURE ALONE DOES NOT REACH ONE, AND THAT WAS MEASURED RATHER THAN ASSUMED.
	 * `Data/parity_fixture.save` replayed one `Submit` at a time crosses 12 turn openings
	 * across its 169 commands and REPAIRS NOBODY at any of them (measured 2026-09-01, by a
	 * first draft of this file that did exactly that walk). That is not a defect in the
	 * fixture: §2.7 needs a DAMAGED unit standing on an objective ITS OWN SIDE holds with NO
	 * enemy adjacent, and the fixture's damaged units are the ones in contact while its units
	 * on owned objectives are the ones capturing away from the fighting. So the position has to
	 * be reached, and this function reaches it.
	 *
	 * HOW, AND WHY EVERY STEP IS STILL THE MODULE'S ANSWER RATHER THAN THIS FILE'S.
	 *   1. The fixture is replayed through `Submit`, which is what produces the DAMAGE. Not one
	 *      attack is chosen here; every one is the fixture's.
	 *   2. At each turn opening the active side's units are asked, from the projection alone,
	 *      which of them is below `hpMax`. That is the module's `hp` and `hpMax`.
	 *   3. For each such unit the destination set is `FStratBridge::Reachable` -- the rules
	 *      module's own `strat::reachable`, T-UI-02's query, and emphatically not a distance
	 *      filter. It is filtered by the two facts §2.7 itself reads: `strat::UiHexView::owner
	 *      == side`, and no enemy at `hexDistance == 1`. The reach set includes the unit's own
	 *      hex at cost 0, so "it already stands somewhere that works" and "it can walk
	 *      somewhere that works" are one search rather than two.
	 *   4. Every such unit is moved, one at a time, with the projection and the reach set
	 *      recomputed between movers so that no plan is made against a board another mover has
	 *      already changed. A REFUSED MOVE IS FATAL, so a destination the module would not
	 *      accept cannot be silently skipped past.
	 *   5. Then two `SubmitEndTurn` calls: the opponent's turn opens and is passed WITHOUT A
	 *      SINGLE COMMAND, so no enemy moves and the adjacency read in step 3 is still the
	 *      adjacency §2.7 sees; then the movers' own turn opens, and THAT is the bracket the
	 *      clause measures.
	 *
	 * GOING OFF THE FIXTURE'S SCRIPT IS THIS DIRECTORY'S EXISTING POSTURE, not a new liberty:
	 * `StratCombatOutcomeParity.cpp`'s counter-kill clause does exactly this and says so, for
	 * the same reason -- the arm it needs is unreachable from the script, and the position it
	 * submits from is still the fixture's.
	 *
	 * EVERY MOVER IS PLANTED, NOT ONLY THE FIRST, so the answer can carry more than one entry
	 * and the ascending-by-unit-id clause has an order to read. WHEN NO PLAN EXISTS THIS
	 * RETURNS FALSE; no clause built on it can pass on an empty answer.
	 */
	static bool DriveToARepairingTurnOpen(FStratBridge&      Bridge,
	                                      const strat::Save& Fixture,
	                                      FRepairMoment&     Out,
	                                      FString&           OutError)
	{
		if (!SeedBridge(Bridge, OutError))
		{
			return false;
		}

		const int32 Count = static_cast<int32>(Fixture.commandLog.size());
		int32       Openings = 0;

		for (int32 Index = 0; Index <= Count; ++Index)
		{
			// A TURN OPENING, WHERE EVERY UNIT'S MOVE FLAG IS CLEAR. `beginTurn` clears both
			// flag sets at the start of the owner's turn (T-TURN-01(e)), so this is the one
			// moment at which any of the active side's units may still move. Index 0 is the
			// seeded opening; every later one is the state after a fixture `EndTurn`.
			const bool bIsOpening =
				(Index == 0)
				|| (Fixture.commandLog[static_cast<size_t>(Index - 1)].kind
				    == strat::SaveCommandKind::EndTurn);

			if (bIsOpening)
			{
				++Openings;

				strat::UiSnapshot Here;
				if (!Bridge.MakeUiSnapshot(Here).bOk)
				{
					OutError = FString::Printf(
						TEXT("the projection refused at fixture position %d"), Index);
					return false;
				}

				const int32 Side = static_cast<int32>(Here.match.sideToMove);

				// The damaged units of the side that is to move, by id, off the projection.
				TArray<int32> Damaged;
				for (const strat::UiUnitView& Unit : Here.units)
				{
					if (static_cast<int32>(Unit.side) == Side && Unit.hp < Unit.hpMax)
					{
						Damaged.Add(static_cast<int32>(Unit.id));
					}
				}
				if (Damaged.Num() == 0)
				{
					// Nothing to heal here; play the fixture on.
				}
				else
				{
					TArray<FString> Planted;

					for (const int32 UnitId : Damaged)
					{
						// RECOMPUTED PER MOVER. A plan made against the board as it was before
						// an earlier mover walked across it would be a plan against a state
						// that no longer exists.
						strat::UiSnapshot Now;
						if (!Bridge.MakeUiSnapshot(Now).bOk)
						{
							OutError = TEXT("the projection refused mid-plan");
							return false;
						}

						const strat::UiUnitView* const Unit = UnitIn(Now, UnitId);
						if (Unit == nullptr || Unit->hp >= Unit->hpMax)
						{
							continue;
						}

						std::vector<strat::ReachEntry> Reach;
						if (!Bridge.Reachable(UnitId, Reach).bOk)
						{
							continue;
						}

						for (const strat::ReachEntry& Entry : Reach)
						{
							strat::UiUnitView AsIfThere = *Unit;
							AsIfThere.hex = Entry.hex;
							if (!OnOwnedObjective(Now, AsIfThere) || EnemyAdjacent(Now, AsIfThere))
							{
								continue;
							}

							if (!strat::hexEqual(Entry.hex, Unit->hex))
							{
								const FStratResult Moved = Bridge.SubmitMove(UnitId, Entry.hex);
								if (!Moved.bOk)
								{
									// REFUSING IS FATAL RATHER THAN SKIPPED. The destination
									// came out of the module's own reach set computed against
									// this very board, so a refusal means two module answers
									// disagree -- a finding, not something to walk past.
									OutError = FString::Printf(
										TEXT("the module refused a move to (%d,%d), a hex its own "
										     "`reachable` had just offered unit %d: %s"),
										Entry.hex.q, Entry.hex.r, UnitId, *Moved.Reason);
									return false;
								}
							}

							Planted.Add(FString::Printf(
								TEXT("unit %d at hp %d/%d on (%d,%d)"),
								UnitId, static_cast<int32>(Unit->hp),
								static_cast<int32>(Unit->hpMax), Entry.hex.q, Entry.hex.r));
							break;
						}
					}

					if (Planted.Num() > 0)
					{
						const FString Description = FString::Printf(
							TEXT("fixture position %d (%d openings in), side %d: %s, then the "
							     "match is passed round to that side's next turn"),
							Index, Openings, Side, *FString::Join(Planted, TEXT("; ")));

						// The opponent's turn, opened and passed without a single command, so
						// nothing moves between the adjacency read above and the repair below.
						const FStratResult MineEnds = Bridge.SubmitEndTurn();
						if (!MineEnds.bOk)
						{
							OutError = FString::Printf(
								TEXT("ending the movers' turn refused: %s"), *MineEnds.Reason);
							return false;
						}

						// THE BRACKET. `Before` is read immediately ahead of the submit whose
						// `openTurn` performs the repair.
						strat::UiSnapshot Before;
						if (!Bridge.MakeUiSnapshot(Before).bOk)
						{
							OutError = TEXT("the projection refused before the measured opening");
							return false;
						}

						const FStratResult TheirsEnds = Bridge.SubmitEndTurn();
						if (!TheirsEnds.bOk)
						{
							OutError = FString::Printf(
								TEXT("ending the opponent's turn refused: %s"),
								*TheirsEnds.Reason);
							return false;
						}

						TArray<FStratRepairApplication> Repairs;
						const FStratResult Read = Bridge.RepairsAtTurnOpen(Repairs);
						if (!Read.bOk)
						{
							OutError = FString::Printf(TEXT("RepairsAtTurnOpen refused: %s"),
								*Read.Reason);
							return false;
						}
						if (Repairs.Num() == 0)
						{
							// The plan reached its own precondition and the record is empty.
							// That is a finding rather than a reason to keep looking: the
							// board facts were read off the module and the position is the
							// one §2.7 describes, so an empty answer here is the bridge
							// failing to observe a repair that happened.
							OutError = FString::Printf(
								TEXT("the position was reached and the record is EMPTY (%s)"),
								*Description);
							return false;
						}

						Out.bFound       = true;
						Out.CommandIndex = Index;
						Out.Description  = Description;
						Out.Before       = Before;
						Out.Repairs      = Repairs;
						if (!Bridge.MakeUiSnapshot(Out.After).bOk)
						{
							OutError = TEXT("the projection refused after the measured opening");
							return false;
						}
						return true;
					}
				}
			}

			if (Index == Count)
			{
				break;
			}

			const strat::SaveCommand& Command = Fixture.commandLog[static_cast<size_t>(Index)];
			const FStratResult Applied = Bridge.Submit(Command);
			if (!Applied.bOk)
			{
				OutError = FString::Printf(TEXT("fixture command %d refused: %s"),
					Index, *Applied.Reason);
				return false;
			}
		}

		OutError = FString::Printf(
			TEXT("no damaged unit at any of the fixture's %d turn openings could stand on an "
			     "objective its own side holds free of adjacent enemies -- this file's subject "
			     "is unreachable from this fixture and no clause below can measure anything"),
			Openings);
		return false;
	}

	static FString DescribeRepairs(const TArray<FStratRepairApplication>& Repairs)
	{
		if (Repairs.Num() == 0)
		{
			return TEXT("<empty>");
		}

		TArray<FString> Parts;
		for (const FStratRepairApplication& R : Repairs)
		{
			Parts.Add(FString::Printf(TEXT("{unit=%d side=%d hp %d->%d/%d amount=%d}"),
				R.UnitId, R.Side, R.HpBefore, R.HpAfter, R.HpMax, R.Amount));
		}
		return FString::Join(Parts, TEXT(", "));
	}
}

// ---------------------------------------------------------------------------
// THE AMOUNT IS `strat::repairAmount`'S, NOT MERELY AN HP RISE.
//
// THIS IS THE ORACLE CLAUSE AND EVERY OTHER CLAUSE IN THIS FILE IS BOOKKEEPING BESIDE IT.
// `FStratBridge` cannot read the module's own `RepairApplied::amount` -- `strat::openTurn`
// returns `void` and discards the vector -- so it measures HP across the single
// `applyCommand` that opens a turn and calls the difference a repair. THAT INFERENCE IS WHAT
// THIS CLAUSE TESTS. The bridge's own header argues it is sound because nothing else inside
// that arm writes `GameUnit::hp`; an argument in a comment is not a gate, and this is the
// gate. If a re-vendor ever put a second HP writer in the `EndTurn` arm, the measured amount
// would stop equalling `repairAmount` and this clause is what would say so.
//
// THE EXPECTATION IS COMPUTED IN THE TEST, AND THAT IS THE POINT RATHER THAN A LAPSE. There
// is no module-side repair figure to read anywhere in this tree -- `openTurn` throws it away
// and `Driver.good.cpp`'s `openActiveTurn`, which does keep it, is the debug driver over
// `Session` and the bridge never runs it. So the only independent oracle available is
// `strat::repairAmount` itself, asked the same question with the same board facts. Its two
// inputs are read off the module's own projection and its `hp`/`hpMax` off
// `strat::UiUnitView`; nothing here re-derives the +25% table, the min-1 floor or the
// never-overheal clamp, which are `repairAmount`'s and are gated upstream as T-REPAIR-01..07.
//
// AND THE CLAUSE ASKS THE ORACLE ABOUT EVERY UNIT, NOT ONLY THE REPAIRED ONES. That is what
// makes it a two-sided statement: a unit the oracle says was owed nothing must carry NO
// entry, and a unit the oracle says was owed something must carry an entry for exactly that
// amount. A one-sided version -- checking only the rows the bridge chose to report -- would
// be green over a bridge that reported one unit out of four.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRepairAmountIsTheModulesTest,
	"Stratocracy.StratBridge.T-TURN-08.RepairAmountIsTheModulesOwnRepairAmount",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRepairAmountIsTheModulesTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRepairReceiptParity;

	strat::Save Fixture;
	FString     Error;
	if (!TestTrue(TEXT("the parity fixture parses"), LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratBridge  Bridge;
	FRepairMoment Moment;
	if (!TestTrue(TEXT("the match reaches a turn opening at which somebody heals"),
			DriveToARepairingTurnOpen(Bridge, Fixture, Moment, Error)))
	{
		AddError(Error);
		return false;
	}
	AddInfo(Moment.Description);

	AddInfo(FString::Printf(TEXT("the measured answer is %s"),
		*DescribeRepairs(Moment.Repairs)));

	// ---- the oracle, asked about EVERY projected unit --------------------------------
	int32 UnitsTheOracleOwed = 0;
	for (const strat::UiUnitView& Unit : Moment.Before.units)
	{
		const int32 UnitId = static_cast<int32>(Unit.id);

		// The two board facts, off the module's own projection at the moment §2.7 read them.
		const bool bOnOwned  = OnOwnedObjective(Moment.Before, Unit);
		const bool bAdjacent = EnemyAdjacent(Moment.Before, Unit);

		// `repairAmount` reads `hp` and `hpMax` and no other member of `Unit`; both come off
		// the projection. See the file header.
		strat::Unit AsCombatUnit;
		AsCombatUnit.hp    = Unit.hp;
		AsCombatUnit.hpMax = Unit.hpMax;

		const int32 Owed =
			static_cast<int32>(strat::repairAmount(AsCombatUnit, bOnOwned, bAdjacent));

		// §2.7 repairs the ACTIVE side alone. `applyStartOfTurnRepair` filters on
		// `s.activeSide`, and the side that is active inside `openTurn` is the side the
		// snapshot AFTER the submit reports as to move -- read from the module, not assumed.
		const bool bIsTheRepairedSide =
			(static_cast<int32>(Unit.side) == static_cast<int32>(Moment.After.match.sideToMove));

		const FStratRepairApplication* Reported = nullptr;
		for (const FStratRepairApplication& R : Moment.Repairs)
		{
			if (R.UnitId == UnitId)
			{
				Reported = &R;
				break;
			}
		}

		if (Owed > 0 && bIsTheRepairedSide)
		{
			++UnitsTheOracleOwed;
			if (!TestNotNull(*FString::Printf(
					TEXT("T-TURN-08: strat::repairAmount owes unit %d %d HP at this opening, so "
					     "the bridge must report it: %s"),
					UnitId, Owed, *DescribeRepairs(Moment.Repairs)),
					Reported))
			{
				continue;
			}
			TestEqual(*FString::Printf(
					TEXT("T-TURN-08: unit %d's measured amount IS strat::repairAmount's answer"),
					UnitId),
				Reported->Amount, Owed);
			TestEqual(*FString::Printf(
					TEXT("T-TURN-08: unit %d's HpBefore is the projection's own reading"), UnitId),
				Reported->HpBefore, static_cast<int32>(Unit.hp));
			TestEqual(*FString::Printf(
					TEXT("T-TURN-08: unit %d's HpAfter is HpBefore plus the module's amount"),
					UnitId),
				Reported->HpAfter, static_cast<int32>(Unit.hp) + Owed);
		}
		else
		{
			TestNull(*FString::Printf(
					TEXT("T-TURN-08: strat::repairAmount owes unit %d nothing at this opening "
					     "(onOwnedObjective=%d enemyAdjacent=%d hp=%d/%d activeSide=%d), so the "
					     "bridge must report no entry for it"),
					UnitId, bOnOwned ? 1 : 0, bAdjacent ? 1 : 0,
					static_cast<int32>(Unit.hp), static_cast<int32>(Unit.hpMax),
					static_cast<int32>(Moment.After.match.sideToMove)),
				Reported);
		}
	}

	// The fixture precondition, asserted AFTER the loop so a red run has already named which
	// unit disagreed. Without it a bridge that reported nothing and an oracle that owed
	// nothing would agree vacuously.
	TestTrue(TEXT("T-TURN-08: the oracle owed at least one unit something, without which this "
	              "clause would be an agreement between two empty answers"),
		UnitsTheOracleOwed > 0);

	// And the two sides of the comparison have the same size, which the per-unit loop above
	// cannot see: a bridge reporting a unit the projection never carried is invisible to a
	// walk over the projection.
	TestEqual(*FString::Printf(
			TEXT("T-TURN-08: the bridge reports exactly the units the oracle owed: %s"),
			*DescribeRepairs(Moment.Repairs)),
		Moment.Repairs.Num(), UnitsTheOracleOwed);

	return true;
}

// ---------------------------------------------------------------------------
// EVERY ENTRY IS A STRICTLY POSITIVE RISE, WITHIN `hpMax`, ON THE SIDE NOW TO MOVE, AND THE
// LIST ASCENDS BY UNIT ID -- AND AN OPENING THAT HEALS NOBODY ANSWERS `Ok` AND EMPTY.
//
// TWO HALVES, AND THE SECOND IS THE ONE THAT IS EASY TO LEAVE OUT. `RepairsAtTurnOpen`'s own
// block says "EMPTY IS AN ANSWER meaning 'nothing healed', which is the common case and is
// `Ok()`" -- so the ordinary turn, on which §2.7 pays nobody, must be a SUCCESSFUL call with
// an empty array and never a refusal. The fixture supplies twelve of those openings for free,
// so the common case is pinned over twelve real turn boundaries rather than over none.
//
// THE STRUCTURAL RULES ARE APPLIED TO THE DRIVEN OPENING, which is the only non-empty answer
// this fixture can produce -- see `DriveToARepairingTurnOpen` for why the fixture reaches none
// on its own script.
//
// THE ORDER IS ASSERTED BECAUSE THE BRIDGE SORTS RATHER THAN INHERITS.
// `applyStartOfTurnRepair` guarantees ascending ids for the vector IT returns; the bridge
// never sees that vector and builds its list by walking `strat::UiSnapshot::units`, so the
// declared order is the `std::sort` in `StratRepairObservation::CaptureAfter` and nothing
// else. STRICT ascent, not merely non-descent, which also pins that no unit is reported twice.
//
// AND THE ASCENT IS ONLY OBSERVABLE WITH TWO ENTRIES, WHICH IS WHY `DriveToARepairingTurnOpen`
// PLANTS EVERY MOVER IT CAN RATHER THAN THE FIRST. Whether the fixture affords two is a
// property of the board and not of this file, so the clause reports the width it got via
// `AddInfo` instead of demanding one -- a demand it could not honestly make. WHAT THE ORDER
// CLAUSE PINS ON A ONE-ENTRY ANSWER IS THEREFORE NOTHING, and that is recorded in
// `Tools/architect/state/tests.md` rather than left for a reader to infer from a green run.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRepairEntriesAreWellFormedTest,
	"Stratocracy.StratBridge.T-TURN-08.EveryRepairEntryIsAPositiveRiseAscendingByUnitId",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRepairEntriesAreWellFormedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRepairReceiptParity;

	strat::Save Fixture;
	FString     Error;
	if (!TestTrue(TEXT("the parity fixture parses"), LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- HALF ONE: the ordinary opening answers Ok and empty ---------------------------
	{
		FStratBridge Plain;
		if (!TestTrue(TEXT("the bridge seeds"), SeedBridge(Plain, Error)))
		{
			AddError(Error);
			return false;
		}

		int32 Openings = 0;
		int32 Empty    = 0;

		const int32 Count = static_cast<int32>(Fixture.commandLog.size());
		for (int32 Index = 0; Index < Count; ++Index)
		{
			const strat::SaveCommand& Command = Fixture.commandLog[static_cast<size_t>(Index)];

			const FStratResult Applied = Plain.Submit(Command);
			if (!TestTrue(*FString::Printf(TEXT("fixture command %d applies: %s"),
					Index, *Applied.Reason), Applied.bOk))
			{
				return false;
			}
			if (Command.kind != strat::SaveCommandKind::EndTurn)
			{
				continue;
			}
			++Openings;

			TArray<FStratRepairApplication> Repairs;
			const FStratResult Read = Plain.RepairsAtTurnOpen(Repairs);
			if (!TestTrue(*FString::Printf(
					TEXT("T-TURN-08: a seeded bridge answers the repair record at every turn "
					     "opening -- command %d refused with '%s'"),
					Index, *Read.Reason),
					Read.bOk))
			{
				return false;
			}
			if (Repairs.Num() == 0)
			{
				++Empty;
			}
		}

		AddInfo(FString::Printf(
			TEXT("the fixture's own script crosses %d turn openings; %d of them healed nobody"),
			Openings, Empty));

		if (!TestTrue(TEXT("the fixture crosses turn openings at all"), Openings > 0))
		{
			return false;
		}
		TestEqual(TEXT("T-TURN-08: an opening that heals nobody is a SUCCESSFUL call with an "
		               "empty array -- empty is an answer, not a refusal"),
			Empty, Openings);
	}

	// ---- HALF TWO: the structural rules, over a real non-empty answer -------------------
	FStratBridge  Bridge;
	FRepairMoment Moment;
	if (!TestTrue(TEXT("the match reaches a turn opening at which somebody heals"),
			DriveToARepairingTurnOpen(Bridge, Fixture, Moment, Error)))
	{
		AddError(Error);
		return false;
	}
	AddInfo(Moment.Description);
	AddInfo(FString::Printf(TEXT("the measured answer carries %d entries: %s"),
		Moment.Repairs.Num(), *DescribeRepairs(Moment.Repairs)));

	int32 PreviousId = INDEX_NONE;
	for (const FStratRepairApplication& R : Moment.Repairs)
	{
		const FString Where = DescribeRepairs(Moment.Repairs);

		TestTrue(*FString::Printf(
				TEXT("T-TURN-08: the list ascends strictly by unit id (%s)"), *Where),
			R.UnitId > PreviousId);
		PreviousId = R.UnitId;

		TestTrue(*FString::Printf(
				TEXT("T-TURN-08: every entry is a strictly positive rise (%s)"), *Where),
			R.Amount > 0);
		TestEqual(*FString::Printf(
				TEXT("T-TURN-08: Amount is HpAfter - HpBefore and nothing else (%s)"), *Where),
			R.Amount, R.HpAfter - R.HpBefore);
		TestTrue(*FString::Printf(
				TEXT("T-TURN-08: the module never overheals past hpMax (%s)"), *Where),
			R.HpAfter <= R.HpMax);
		TestEqual(*FString::Printf(
				TEXT("T-TURN-08: start-of-turn repair covers the side that is now to move, and "
				     "only it (%s)"),
				*Where),
			R.Side, static_cast<int32>(Moment.After.match.sideToMove));

		// The reported HP is the unit's real HP on the projection taken after the open -- a
		// mirror, not a number the observation invented.
		const strat::UiUnitView* const Unit = UnitIn(Moment.After, R.UnitId);
		if (TestNotNull(*FString::Printf(
				TEXT("T-TURN-08: the repaired unit %d is on the board afterwards (%s)"),
				R.UnitId, *Where), Unit))
		{
			TestEqual(*FString::Printf(
					TEXT("T-TURN-08: HpAfter is the projection's own hp for unit %d (%s)"),
					R.UnitId, *Where),
				R.HpAfter, static_cast<int32>(Unit->hp));
			TestEqual(*FString::Printf(
					TEXT("T-TURN-08: HpMax is the projection's own hpMax for unit %d (%s)"),
					R.UnitId, *Where),
				R.HpMax, static_cast<int32>(Unit->hpMax));
		}

		// And the reading it started from is the projection's, one bracket earlier.
		const strat::UiUnitView* const Was = UnitIn(Moment.Before, R.UnitId);
		if (TestNotNull(*FString::Printf(
				TEXT("T-TURN-08: the repaired unit %d was on the board beforehand (%s)"),
				R.UnitId, *Where), Was))
		{
			TestEqual(*FString::Printf(
					TEXT("T-TURN-08: HpBefore is the projection's own hp for unit %d before the "
					     "submit (%s)"),
					R.UnitId, *Where),
				R.HpBefore, static_cast<int32>(Was->hp));
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// AN UNSEEDED BRIDGE REFUSES, AND EMPTIES THE CALLER'S ARRAY ON THE WAY OUT.
//
// THE SECOND HALF IS THE ONE WORTH HAVING. `RepairsAtTurnOpen` documents that it overwrites
// `OutRepairs` entirely "including emptying it", and that the reset happens BEFORE the
// `bSeeded` guard -- so a caller that reconciles floaters against a reused array cannot
// inherit last turn's numbers on a refusal. A clause that only checked `bOk` would be green
// over a refusal that left the array standing, which is the failure that actually puts a
// stale number on screen.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRepairUnseededRefusalTest,
	"Stratocracy.StratBridge.T-TURN-08.RepairsAtTurnOpenRefusesUnseededAndEmptiesTheArray",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRepairUnseededRefusalTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRepairReceiptParity;

	FStratBridge Unseeded;

	// A NON-EMPTY ARRAY GOING IN, so "emptied" is observable. The entry's contents are a
	// fixture and carry no expectation -- what is asserted is that it is gone.
	TArray<FStratRepairApplication> Answer;
	Answer.AddDefaulted();
	if (!TestEqual(TEXT("the fixture array really is non-empty before the call"),
			Answer.Num(), 1))
	{
		return false;
	}

	const FStratResult Refused = Unseeded.RepairsAtTurnOpen(Answer);

	TestFalse(TEXT("T-TURN-08: an unseeded bridge refuses the repair record"), Refused.bOk);
	TestTrue(TEXT("T-TURN-08: and says why"), !Refused.Reason.IsEmpty());
	TestEqual(TEXT("T-TURN-08: and the caller's array is emptied on the refusal path"),
		Answer.Num(), 0);

	return true;
}

// ---------------------------------------------------------------------------
// A REFUSED `EndTurn` LEAVES THE PREVIOUS RECORD STANDING; A NEW ONE REPLACES IT WHOLE.
//
// THE ASYMMETRY IS DELIBERATE AND IS ARGUED IN `Submit`: a refused `EndTurn` opened no turn,
// so the standing record still describes the turn the player is in, and clearing it there
// would delete a true answer on the strength of a command that changed nothing. The capture
// runs BEFORE `applyCommand` and the write runs below the `!R.ok` return, and only a clause
// that submits a REFUSED `EndTurn` can tell those two placements apart.
//
// THE REFUSAL IS THE RULES MODULE'S, NOT A FLAG THIS FILE SETS. The command is stamped with a
// turn the module says is not live, which `strat::applyCommand`'s own tag check refuses
// before it looks at the kind -- and a refused non-Attack emits no log line, so nothing here
// needs an expected-message registration.
//
// THE SECOND HALF -- that the NEXT accepted `EndTurn` replaces the record whole rather than
// appending to it -- is what makes "a record and not a history" observable. Without it a
// bridge that accumulated every turn's repairs would pass the standing half.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRepairRefusedEndTurnTest,
	"Stratocracy.StratBridge.T-TURN-08.ARefusedEndTurnLeavesThePreviousRecordStanding",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRepairRefusedEndTurnTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRepairReceiptParity;

	strat::Save Fixture;
	FString     Error;
	if (!TestTrue(TEXT("the parity fixture parses"), LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratBridge  Bridge;
	FRepairMoment Moment;
	if (!TestTrue(TEXT("the match reaches a turn opening at which somebody heals"),
			DriveToARepairingTurnOpen(Bridge, Fixture, Moment, Error)))
	{
		AddError(Error);
		return false;
	}
	AddInfo(Moment.Description);

	TArray<FStratRepairApplication> Standing;
	if (!TestTrue(TEXT("the record reads"), Bridge.RepairsAtTurnOpen(Standing).bOk))
	{
		return false;
	}
	if (!TestTrue(TEXT("the standing record is non-empty, without which nothing below can fail"),
			Standing.Num() > 0))
	{
		return false;
	}

	// ---- a REFUSED EndTurn, refused by the rules module's own tag check ---------------
	strat::SaveCommand Mistagged;
	Mistagged.kind    = strat::SaveCommandKind::EndTurn;
	Mistagged.turn    = Bridge.Turn() + 1;      // a turn the module says is not live
	Mistagged.side    = Bridge.SideToMove();
	Mistagged.hasHex  = false;
	Mistagged.hasUnit = false;

	const FStratResult Refused = Bridge.Submit(Mistagged);
	if (!TestFalse(TEXT("the rules module refuses an EndTurn tagged with the wrong turn"),
			Refused.bOk))
	{
		return false;
	}

	TArray<FStratRepairApplication> AfterRefusal;
	TestTrue(TEXT("the record still reads after a refusal"),
		Bridge.RepairsAtTurnOpen(AfterRefusal).bOk);

	if (TestEqual(*FString::Printf(
			TEXT("T-TURN-08: a refused EndTurn leaves the record's length alone (was %s, now %s)"),
			*DescribeRepairs(Standing), *DescribeRepairs(AfterRefusal)),
			AfterRefusal.Num(), Standing.Num()))
	{
		for (int32 i = 0; i < Standing.Num(); ++i)
		{
			TestEqual(TEXT("T-TURN-08: and every unit id in it"),
				AfterRefusal[i].UnitId, Standing[i].UnitId);
			TestEqual(TEXT("T-TURN-08: and every amount in it"),
				AfterRefusal[i].Amount, Standing[i].Amount);
		}
	}

	// ---- and the next ACCEPTED EndTurn replaces it whole ------------------------------
	// A RECORD, NOT A HISTORY. The very next turn opening is the opponent's, and §2.7 repairs
	// the active side alone, so the previous side's entries cannot legitimately survive it.
	const FStratResult Accepted = Bridge.SubmitEndTurn();
	if (!TestTrue(*FString::Printf(TEXT("a correctly stamped EndTurn applies: %s"),
			*Accepted.Reason), Accepted.bOk))
	{
		return false;
	}

	TArray<FStratRepairApplication> Replaced;
	TestTrue(TEXT("the record reads after the accepted end of turn"),
		Bridge.RepairsAtTurnOpen(Replaced).bOk);

	strat::UiSnapshot Now;
	if (TestTrue(TEXT("the projection answers"), Bridge.MakeUiSnapshot(Now).bOk))
	{
		for (const FStratRepairApplication& R : Replaced)
		{
			TestEqual(*FString::Printf(
					TEXT("T-TURN-08: the record was replaced whole -- every entry belongs to the "
					     "side that is now to move, not to the previous turn's (%s)"),
					*DescribeRepairs(Replaced)),
				R.Side, static_cast<int32>(Now.match.sideToMove));
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// A COMMAND THAT IS NOT AN `EndTurn` DOES NOT DISTURB THE RECORD.
//
// `Submit` gates the whole observation on `Command.kind == EndTurn`, so the record must read
// identically across a Move or a Capture -- the ordinary mid-turn traffic during which a
// reconciled surface refreshes over and over. The `RepairsAtTurnOpen` block calls this "a
// record and not an event" and rests the caller's whole re-emission rule on it: if a mid-turn
// command silently emptied or refilled the record, `StratDecideTransientReceipts`'s turn-edge
// gate would be sitting on a moving value.
//
// THE MOVES ARE THE MODULE'S OWN OFFERS. Every destination comes out of
// `FStratBridge::Reachable` for the unit being moved, so no hex is chosen in this file, and a
// destination the module then refuses is a failure rather than something to skip past. The
// fixture's own log cannot be resumed here: `DriveToARepairingTurnOpen` leaves the match off
// the script, and a fixture command tagged with another turn would simply be refused --
// which would make this clause measure a refusal instead of a Move.
//
// EVERY UNIT OF THE ACTIVE SIDE IS MOVED, so the clause is not one command wide. A unit with
// nowhere to go is skipped, and the count of what was actually submitted is asserted at the
// end -- without which a board on which nothing could move would run every assertion zero
// times and report green.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRepairNonEndTurnIsInertTest,
	"Stratocracy.StratBridge.T-TURN-08.ANonEndTurnCommandLeavesTheRecordStanding",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRepairNonEndTurnIsInertTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRepairReceiptParity;

	strat::Save Fixture;
	FString     Error;
	if (!TestTrue(TEXT("the parity fixture parses"), LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratBridge  Bridge;
	FRepairMoment Moment;
	if (!TestTrue(TEXT("the match reaches a turn opening at which somebody heals"),
			DriveToARepairingTurnOpen(Bridge, Fixture, Moment, Error)))
	{
		AddError(Error);
		return false;
	}
	AddInfo(Moment.Description);

	TArray<FStratRepairApplication> Standing;
	if (!TestTrue(TEXT("the record reads"), Bridge.RepairsAtTurnOpen(Standing).bOk))
	{
		return false;
	}
	if (!TestTrue(TEXT("the standing record is non-empty, without which nothing below can fail"),
			Standing.Num() > 0))
	{
		return false;
	}

	const int32 ActiveSide = Bridge.SideToMove();

	// The active side's unit ids, off the projection taken at the opening this clause is
	// standing in.
	TArray<int32> Movers;
	for (const strat::UiUnitView& Unit : Moment.After.units)
	{
		if (static_cast<int32>(Unit.side) == ActiveSide)
		{
			Movers.Add(static_cast<int32>(Unit.id));
		}
	}

	int32 Submitted = 0;
	for (const int32 UnitId : Movers)
	{
		strat::UiSnapshot Now;
		if (!TestTrue(TEXT("the projection answers"), Bridge.MakeUiSnapshot(Now).bOk))
		{
			return false;
		}
		const strat::UiUnitView* const Unit = UnitIn(Now, UnitId);
		if (Unit == nullptr)
		{
			continue;
		}

		std::vector<strat::ReachEntry> Reach;
		if (!Bridge.Reachable(UnitId, Reach).bOk)
		{
			continue;
		}

		// Any offered hex that is not the one it already stands on. `strat::reachable`
		// includes the start hex at cost 0, and a move to it would be a no-op.
		bool bMoved = false;
		for (const strat::ReachEntry& Entry : Reach)
		{
			if (strat::hexEqual(Entry.hex, Unit->hex))
			{
				continue;
			}

			const FStratResult Moved = Bridge.SubmitMove(UnitId, Entry.hex);
			if (!TestTrue(*FString::Printf(
					TEXT("the module accepts a move to (%d,%d), a hex its own `reachable` just "
					     "offered unit %d: %s"),
					Entry.hex.q, Entry.hex.r, UnitId, *Moved.Reason),
					Moved.bOk))
			{
				return false;
			}
			bMoved = true;
			break;
		}
		if (!bMoved)
		{
			continue;
		}
		++Submitted;

		TArray<FStratRepairApplication> After;
		TestTrue(TEXT("the record reads mid-turn"), Bridge.RepairsAtTurnOpen(After).bOk);

		if (TestEqual(*FString::Printf(
				TEXT("T-TURN-08: a Move is not an EndTurn and leaves the record's length alone "
				     "(was %s, now %s)"),
				*DescribeRepairs(Standing), *DescribeRepairs(After)),
				After.Num(), Standing.Num()))
		{
			for (int32 i = 0; i < Standing.Num(); ++i)
			{
				TestEqual(TEXT("T-TURN-08: and every unit id in it"),
					After[i].UnitId, Standing[i].UnitId);
				TestEqual(TEXT("T-TURN-08: and every amount in it"),
					After[i].Amount, Standing[i].Amount);
				TestEqual(TEXT("T-TURN-08: and every HP reading in it"),
					After[i].HpAfter, Standing[i].HpAfter);
			}
		}
	}

	// WITHOUT THIS THE LOOP MAY HAVE RUN ZERO TIMES.
	TestTrue(TEXT("T-TURN-08: at least one non-EndTurn command was actually submitted"),
		Submitted > 0);

	return true;
}

// ---------------------------------------------------------------------------
// `ReplayLog` AND A RESEED EACH LEAVE THE RECORD EMPTY, AND FOR DIFFERENT STATED REASONS.
//
// BOTH ARE DECLARED BEHAVIOURS AND NEITHER IS OBSERVABLE ANYWHERE ELSE.
//   - `ReplayLog` crosses every turn boundary inside one `strat::replayLog` call and hands
//     back nothing per command, so there is no bracket and no honest answer. What it must NOT
//     do is leave the previous record standing: those entries describe a turn boundary from
//     before the replay, over a state the replay has since moved, and drawing a floater off
//     them would attribute a repair to a different match's timeline.
//   - `LoadScenarioFromFile` and `LoadDefinitions` each clear it because the unit ids it
//     names belong to a seed that has just gone stale. A record carried across a reseed draws
//     the previous match's numbers over whatever now holds those ids.
//
// ONE CLAUSE FOR THREE ROUTES BECAUSE THEY SHARE ONE FIXTURE -- a bridge whose record is
// non-empty -- and building that fixture is the expensive half. Each route is asserted
// separately and names itself in its own message.
//
// AND ONE OF THE THREE IS ONLY HALF-PINNED, WHICH IS SAID HERE RATHER THAN LEFT TO A READER OF
// A GREEN RUN. `LoadDefinitions` sets `bSeeded = false` in the same function that clears the
// record, so the emptiness sits behind a refusal and a mutant deleting the clear stays green.
// The route's own block below states exactly what is and is not measured.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRepairRecordIsClearedTest,
	"Stratocracy.StratBridge.T-TURN-08.ReplayAndReseedEachLeaveTheRecordEmpty",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRepairRecordIsClearedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRepairReceiptParity;

	strat::Save Fixture;
	FString     Error;
	if (!TestTrue(TEXT("the parity fixture parses"), LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- route 1: a reseed through `LoadScenarioFromFile` ------------------------------
	{
		FStratBridge  Bridge;
		FRepairMoment Moment;
		if (!TestTrue(TEXT("the match reaches a repairing turn opening"),
				DriveToARepairingTurnOpen(Bridge, Fixture, Moment, Error)))
		{
			AddError(Error);
			return false;
		}

		TArray<FStratRepairApplication> Standing;
		TestTrue(TEXT("the record reads"), Bridge.RepairsAtTurnOpen(Standing).bOk);
		if (!TestTrue(TEXT("the record is non-empty before the reseed"), Standing.Num() > 0))
		{
			return false;
		}

		const FStratResult Reseeded = Bridge.LoadScenarioFromFile(ScenarioPath(), kFirstSide);
		if (TestTrue(*FString::Printf(TEXT("the bridge reseeds: %s"), *Reseeded.Reason),
				Reseeded.bOk))
		{
			TArray<FStratRepairApplication> After;
			TestTrue(TEXT("the record reads after the reseed"),
				Bridge.RepairsAtTurnOpen(After).bOk);
			TestEqual(*FString::Printf(
					TEXT("T-TURN-08: LoadScenarioFromFile clears the repair record (%s)"),
					*DescribeRepairs(After)),
				After.Num(), 0);
		}
	}

	// ---- route 2: a table reload through `LoadDefinitions` -----------------------------
	// AND THIS ROUTE IS PINNED ONLY AS FAR AS IT CAN BE, WHICH IS NOT ALL THE WAY.
	// `LoadDefinitions` sets `bSeeded = false` in the same function that clears the repair
	// record, so `RepairsAtTurnOpen` REFUSES afterwards and the emptiness is invisible behind
	// the refusal: deleting `RepairsAtLastTurnOpen.clear()` from `LoadDefinitions` leaves this
	// clause green. What IS observable -- and is what a caller actually experiences -- is that
	// a reload takes the answer away and empties the array it was handed, so no stale entry
	// can survive into a caller's list by either route. The unobservable half is recorded in
	// `Tools/architect/state/tests.md` rather than papered over here.
	{
		FStratBridge  Bridge;
		FRepairMoment Moment;
		if (!TestTrue(TEXT("the match reaches a repairing turn opening"),
				DriveToARepairingTurnOpen(Bridge, Fixture, Moment, Error)))
		{
			AddError(Error);
			return false;
		}

		TArray<FStratRepairApplication> Standing;
		TestTrue(TEXT("the record reads"), Bridge.RepairsAtTurnOpen(Standing).bOk);
		if (!TestTrue(TEXT("the record is non-empty before the table reload"), Standing.Num() > 0))
		{
			return false;
		}

		UDataTable* const Units   = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		UDataTable* const Terrain = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
		if (TestTrue(TEXT("the shipped tables load"), Units != nullptr && Terrain != nullptr))
		{
			const FStratResult Reloaded = Bridge.LoadDefinitions(Units, Terrain);
			if (TestTrue(*FString::Printf(TEXT("the tables reload: %s"), *Reloaded.Reason),
					Reloaded.bOk))
			{
				TArray<FStratRepairApplication> After = Standing;   // deliberately non-empty
				const FStratResult Read = Bridge.RepairsAtTurnOpen(After);
				TestFalse(TEXT("T-TURN-08: a table reload makes any prior seed stale, so the "
				               "repair record is no longer answerable"),
					Read.bOk);
				TestEqual(*FString::Printf(
						TEXT("T-TURN-08: and the caller's array is emptied even so, which is what "
						     "stops a stale entry surviving the reload (%s)"),
						*DescribeRepairs(After)),
					After.Num(), 0);
			}
		}
	}

	// ---- route 3: `ReplayLog` ----------------------------------------------------------
	{
		FStratBridge  Bridge;
		FRepairMoment Moment;
		if (!TestTrue(TEXT("the match reaches a repairing turn opening"),
				DriveToARepairingTurnOpen(Bridge, Fixture, Moment, Error)))
		{
			AddError(Error);
			return false;
		}

		TArray<FStratRepairApplication> Standing;
		TestTrue(TEXT("the record reads"), Bridge.RepairsAtTurnOpen(Standing).bOk);
		if (!TestTrue(TEXT("the record is non-empty before the replay"), Standing.Num() > 0))
		{
			return false;
		}

		// AN EMPTY LOG IS ENOUGH AND IS THE SHARPEST FIXTURE AVAILABLE. `ReplayLog` clears
		// the record unconditionally, so a log that changes NOTHING still empties it -- which
		// is precisely the "empty is the true statement: nothing was observed" rule and is
		// the case an implementation that only cleared on a non-empty log would fail.
		const TArray<strat::SaveCommand> Nothing;
		const FStratResult Replayed = Bridge.ReplayLog(Nothing);
		if (TestTrue(*FString::Printf(TEXT("an empty log replays: %s"), *Replayed.Reason),
				Replayed.bOk))
		{
			TArray<FStratRepairApplication> After;
			TestTrue(TEXT("the record reads after the replay"),
				Bridge.RepairsAtTurnOpen(After).bOk);
			TestEqual(*FString::Printf(
					TEXT("T-TURN-08: ReplayLog leaves the repair record empty (%s)"),
					*DescribeRepairs(After)),
				After.Num(), 0);
		}
	}

	return true;
}
