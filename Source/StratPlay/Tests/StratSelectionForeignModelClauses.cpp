// THE CLAUSE THE MACHINE ASKED FOR BY NAME. GDD §2.11.1, §2.6, §4.11 row 10.
//
//   T-INT-05  the presentation block is the MACHINE's. This clause is about the one place
//             where the machine's engine-side answer to "has this unit acted" and the rules
//             module's answer can DISAGREE, and about what the machine does when they do.
//
// THE ARM UNDER TEST is the `Selected->bHasActed` test in the ENEMY-CLICK ARM of
// `FStratSelectionMachine::HandleEvent` -- the `if (Clicked != nullptr)` branch reached with a
// unit already selected and a hex occupied by a unit of another side. It is cited by function
// and branch and never by line number, deliberately: four `file:NNN` citations in this
// milestone were invalidated by the very diff that wrote them.
//
// WHY NO EXISTING CLAUSE COVERS IT, IN THE MACHINE'S OWN WORDS. The comment block sitting on
// that guard says it is "UNREACHABLE BY ANY CLICK SEQUENCE" -- `NotifyCommandApplied`'s Attack
// arm adds the attacker to `DoneUnits` and clears the selection on the same event, and the
// selection gates refuse to re-select a unit in `DoneUnits`, so by the time `bHasActed` is
// true this machine holds no selection naming that unit. The same block states what the guard
// is actually for (a loaded save, a replayed log, or "a machine constructed against a match
// already in progress" produces the rules-side flag without the engine-side set) and hands the
// job of pinning it here in as many words: "Pinning it needs a machine seeded against a model
// it did not watch accumulate -- which is `strat-test-author`'s call". This file is that call.
// The comment is the gameplay engineer's and is not touched by it.
//
// HOW THE UNREACHABLE STATE IS REACHED. Not by a click sequence -- there is none -- but by
// CONSTRUCTION, which is the real-world shape the guard exists for:
//   1. One machine advances the sides into contact, using only hexes `ReachableHexes` itself
//      listed, until `AttackTargetHexes` reports a real target for some unit U.
//   2. A FRESH machine -- `DoneUnits` empty by construction -- selects U and clicks that
//      target. It yields `Attack`. THIS IS THE POSITIVE CONTROL: the identical click, on the
//      identical machine shape, is shown able to produce an attack BEFORE any silence or
//      refusal below is read as meaning anything.
//   3. That attack is submitted to the rules module, so U's rules-side `bHasActed` becomes
//      true. `NotifyCommandApplied` is deliberately NOT called on any machine that survives
//      into step 4 -- that call is precisely the engine-side half this clause must be missing.
//   4. A SECOND fresh machine is built against the rebuilt model. Rules side: U has acted.
//      Engine side: `DoneUnits` is empty. That is the foreign-model condition, and it is
//      ASSERTED rather than assumed, both halves, before anything is clicked.
//
// WHERE EVERY EXPECTATION COMES FROM, which is the only question that matters.
//   - `bHasActed` is read off a model `StratBuildViewModel` built; this file writes no model
//     field and computes no flag.
//   - The attack target is `FStratBridgeRulesQuery::AttackTargetHexes`'s own first element.
//     There is not one hex literal in this file.
//   - The second, far enemy hex is a hex some enemy unit on the MODEL occupies -- read, not
//     chosen by geometry. `FarthestHexDistance` decides only WHICH enemy, is a fixture device,
//     and no assertion compares anything against it.
//   - The rules state moving, or not moving, is `FStratBridge::StateHash()` against its own
//     earlier reading. Never a predicted hash.
//   - The two refusal SENTENCES are compared against EACH OTHER and against a third the same
//     machine produces on the same run. This file hardcodes no refusal text; see the note on
//     the clause itself.
//
// CASE SENSITIVITY, BOTH DIRECTIONS, ON PURPOSE. `FString::operator==`, `Contains`,
// `StartsWith` and `TestEqual` are all `ESearchCase::IgnoreCase` in UE 5.8, and this project
// has already shipped a clause that could not fail because of that default. So: the EQUALITY
// of the two refusals is asserted STRICTLY, through `FString::Equals(..., CaseSensitive)`,
// because a loose comparator would let two genuinely different sentences count as the same
// one. The INEQUALITIES are left LOOSE (plain `TestNotEqual`), because for a not-equal claim
// the loose comparator is the STRICTER of the two -- two strings differing only in case would
// be called equal and the assertion would go red. Strict where strictness is the harder claim,
// loose where looseness is.
//
// NO `strat::` TYPE IS NAMED AND NO `strat::` FREE FUNCTION IS CALLED, per
// `StratPlay.Build.cs`. Every rules answer arrives engine-typed.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Math/IntPoint.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratSelectionMachine.h"
#include "StratViewModel.h"

// Included from a .cpp and never from a UHT-parsed header -- the module-wide discipline.
#include "StratBridge.h"

namespace StratSelectionForeignModelClauses
{
	// The same harness pin every parity file in this tree carries. `Replay.h` states that no
	// rule decides which side moves first, so this is a harness constant and not a rule.
	static const int32 kFirstSide = 0;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	static FString ScenarioPath()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"), TEXT("ferrum_crossing.json"));
	}

	static FString Describe(FIntPoint Hex)
	{
		return FString::Printf(TEXT("axial(%d,%d)"), Hex.X, Hex.Y);
	}

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

	/** Build -> decorate, the order `UStratMatchSubsystem` runs and the running game uses. */
	static bool Refresh(const FStratBridge& Bridge, const FStratSelectionMachine& Machine,
	                    FStratViewModel& OutModel, FString& OutError)
	{
		if (!StratBuildViewModel(Bridge, Bridge.SideToMove(), OutModel, OutError))
		{
			return false;
		}
		Machine.DecorateViewModel(OutModel);
		return true;
	}

	static const FStratUnitView* FindUnit(const FStratViewModel& Model, int32 UnitId)
	{
		for (const FStratUnitView& U : Model.Units)
		{
			if (U.UnitId == UnitId)
			{
				return &U;
			}
		}
		return nullptr;
	}

	static const FStratUnitView* FindUnitAtHex(const FStratViewModel& Model, FIntPoint Hex)
	{
		for (const FStratUnitView& U : Model.Units)
		{
			if (U.Hex == Hex)
			{
				return &U;
			}
		}
		return nullptr;
	}

	/**
	 * Axial hex distance. A FIXTURE DEVICE AND NEVER AN EXPECTATION.
	 *
	 * Two jobs, both of them "which element of a module-supplied set do we click": it steers
	 * the advance toward contact, and it picks WHICH of the model's enemy-occupied hexes plays
	 * the part of the far hex. No clause compares anything against it; a wrong answer here
	 * makes the advance wander or picks a nearer enemy, and makes no assertion easier to pass.
	 * The same standing `StratSelectionWaitClauses.cpp` records for its `AdvanceHexDistance`
	 * and `StratHotSeatReplayParity.cpp` for its own, and deliberately not shared with them --
	 * a shared "distance" helper is exactly the general-circulation arithmetic the machine's
	 * header says this module must not have.
	 */
	static int32 FixtureHexDistance(FIntPoint A, FIntPoint B)
	{
		const int32 dq = A.X - B.X;
		const int32 dr = A.Y - B.Y;
		return (FMath::Abs(dq) + FMath::Abs(dq + dr) + FMath::Abs(dr)) / 2;
	}

	static int32 NearestEnemyDistance(const FStratViewModel& Model, FIntPoint From, int32 Side)
	{
		int32 Best = MAX_int32;
		for (const FStratUnitView& U : Model.Units)
		{
			if (U.Side == Side)
			{
				continue;
			}
			Best = FMath::Min(Best, FixtureHexDistance(From, U.Hex));
		}
		return Best;
	}

	/**
	 * The element of `ReachableHexes`'s OWN ANSWER standing closest to an enemy.
	 *
	 * THE SET IS THE MODULE'S; ONLY THE CHOICE WITHIN IT IS THIS FILE'S. Ferrum Crossing
	 * deploys the sides on opposite edges with nothing in range on turn 1, so taking "the
	 * first" reachable hex never brings the sides into contact and no attack would ever exist
	 * to be pinned.
	 */
	static bool PickAdvance(const IStratRulesQuery& Query, const FStratViewModel& Model,
	                        int32 UnitId, FIntPoint Origin, int32 Side, FIntPoint& OutHex,
	                        FString& OutError)
	{
		TArray<FIntPoint> Reach;
		if (!Query.ReachableHexes(UnitId, Reach, OutError))
		{
			return false;
		}

		bool      bFound       = false;
		int32     BestDistance = MAX_int32;
		FIntPoint Best         = Origin;
		for (const FIntPoint& H : Reach)
		{
			if (H == Origin || FindUnitAtHex(Model, H) != nullptr)
			{
				continue;
			}
			const int32 D = NearestEnemyDistance(Model, H, Side);
			if (!bFound || D < BestDistance)
			{
				bFound       = true;
				BestDistance = D;
				Best         = H;
			}
		}

		if (!bFound)
		{
			OutError = FString::Printf(
				TEXT("unit %d has no unoccupied reachable hex other than its own"), UnitId);
			return false;
		}
		OutHex = Best;
		return true;
	}
}

// ---------------------------------------------------------------------------
// T-INT-05 -- THE ALREADY-ACTED GUARD FIRES ON A MODEL THE MACHINE DID NOT WATCH ACCUMULATE,
// AND IT FIRES BEFORE THE CLICKED HEX IS CONSULTED.
//
// WHAT IS ASSERTED, in this order of weight:
//   1. THE PRECONDITION, both halves and positively. On the rebuilt model unit U carries
//      `bHasActed == true` -- the RULES side -- while the fresh machine reports
//      `IsDone(U) == false` and the decorated model carries `bDone == false` -- the ENGINE
//      side. Without both halves stated, a green here could be the done-set arm reporting on
//      the wrong guard, which the machine's own comment names as the failure mode a careless
//      test would ship.
//   2. THE SELECTION SUCCEEDS. U is selectable on this machine, because `DoneUnits` is empty.
//      This is what proves the click below reaches the enemy-click arm at all, rather than
//      being turned away by the selection gates.
//   3. THE ENEMY CLICK IS REFUSED: `Command == None`, no unit named, a reason set, and the
//      selection left standing on U.
//   4. THE REFUSAL IS THE SAME SENTENCE FOR A HEX THE MODULE ITSELF LISTED AS A TARGET AND FOR
//      A FAR ENEMY HEX IT DID NOT. That equality is the shape of the guard: it fires on the
//      SELECTED unit's flag, before `AttackTargetHexes` is asked anything, so the clicked hex
//      cannot enter the answer. Delete the guard and the two clicks stop agreeing -- the
//      fall-through arm's refusal names the hex's own coordinates -- or one of them becomes an
//      Attack and (3) goes red.
//   5. AND IT IS NOT THE DONE-SET ARM'S SENTENCE. The same machine, later in the same run and
//      about the SAME unit id, is made to produce the already-finished refusal by spending U
//      on a Wait. The two are compared. A same-unit comparison is the point: an inequality
//      between sentences about different units would hold no matter which arm produced them.
//
// THE POSITIVE CONTROL COMES FIRST AND THE CLAUSE IS WORTHLESS WITHOUT IT. Before the attack
// is submitted, a fresh machine of the very same shape, on the very same model, selects U and
// clicks the very same hex, and gets `Attack` back with U's id and that hex on it. Only one
// thing differs between that click and the refused one: whether the rules module has recorded
// U as having acted. `Bridge.StateHash()` is read either side of the submission to show that
// the rules state really did move in between, so "the only thing that differs" is a
// measurement and not a claim.
//
// A SKIP IS A FAILURE. If contact never arises, nothing above was asserted, and the clause
// fails saying how close the two sides came and over how many moves.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAlreadyActedGuardFiresOnAForeignModelTest,
	"Stratocracy.StratPlay.T-INT-05.AlreadyActedGuardFiresOnAForeignModel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAlreadyActedGuardFiresOnAForeignModelTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSelectionForeignModelClauses;

	// NO `AddExpectedMessagePlain`, and that is measured rather than omitted: every command
	// submitted below is an element of a set the module itself enumerated, so no
	// `STRAT-CMD refused` line is emitted and declaring one would fail the clause on "expected
	// message did not occur". The refusals this clause is about are RETURNED VALUES --
	// `HandleEvent` logs nothing on any refusal path.
	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	// THE MACHINE THAT WATCHES. It drives the advance and is then thrown away; nothing it
	// accumulated may reach the machine under test, which is the whole point of the clause.
	FStratSelectionMachine Watcher;
	FStratBridgeRulesQuery Query(&Bridge);

	// The same ceiling the other contact-seeking clauses in this module use: room for the board
	// to be re-authored without flakiness, and a hard stop rather than an unbounded loop.
	const int32 kMaxTurns = 12;

	bool  bPinned        = false;
	int32 ClosestContact = MAX_int32;
	int32 MovesPlayed    = 0;
	int32 TurnsPlayed    = 0;

	for (int32 Leg = 0; Leg < kMaxTurns && !bPinned; ++Leg)
	{
		FStratViewModel Model;
		if (!TestTrue(*FString::Printf(TEXT("leg %d: the model builds"), Leg),
				Refresh(Bridge, Watcher, Model, Error)))
		{
			AddError(Error);
			return false;
		}
		const int32 ActiveSide = Model.Match.SideToMove;
		++TurnsPlayed;

		TArray<int32> ActiveUnitIds;
		for (const FStratUnitView& U : Model.Units)
		{
			if (U.Side == ActiveSide)
			{
				ActiveUnitIds.Add(U.UnitId);
			}
		}

		for (int32 UnitId : ActiveUnitIds)
		{
			if (bPinned)
			{
				break;
			}
			if (!Refresh(Bridge, Watcher, Model, Error))
			{
				AddError(Error);
				return false;
			}

			const FStratUnitView* Unit = FindUnit(Model, UnitId);
			if (Unit == nullptr || Unit->bDone || Unit->Side != Model.Match.SideToMove)
			{
				continue;
			}
			ClosestContact = FMath::Min(ClosestContact,
				NearestEnemyDistance(Model, Unit->Hex, Unit->Side));

			Watcher.HandleEvent(EStratSelectionEvent::HexPrimary, Unit->Hex, Model, Query);
			if (Watcher.GetSelectedUnitId() != UnitId)
			{
				continue;
			}

			for (int32 Attempt = 0; Attempt < 2 && !bPinned; ++Attempt)
			{
				TArray<FIntPoint> Targets;
				FString           TargetReason;
				if (!Query.AttackTargetHexes(UnitId, Targets, TargetReason))
				{
					AddError(TargetReason);
					return false;
				}

				if (Targets.Num() > 0)
				{
					// ================= THE CLAUSE =================================
					if (!Refresh(Bridge, Watcher, Model, Error))
					{
						AddError(Error);
						return false;
					}

					const FIntPoint Target = Targets[0];

					// ---- 0. THE POSITIVE CONTROL, ON A FRESH MACHINE -------------
					// A machine with an empty `DoneUnits`, on this model, selecting this unit
					// and clicking this hex, produces an ATTACK. Every refusal read below is
					// read only after this has been shown. `Watcher` is not used for it: the
					// control must be the same SHAPE of object as the machine under test, or
					// it controls for the wrong thing.
					FStratSelectionMachine Control;
					Control.HandleEvent(
						EStratSelectionEvent::HexPrimary, Unit->Hex, Model, Query);
					if (!TestEqual(TEXT("the control machine selects the attacker"),
							Control.GetSelectedUnitId(), UnitId))
					{
						return false;
					}

					const FStratSelectionOutcome Controlled = Control.HandleEvent(
						EStratSelectionEvent::HexPrimary, Target, Model, Query);
					if (!TestEqual(
							*FString::Printf(
								TEXT("CONTROL: a fresh machine with an EMPTY done set, on this "
								     "model, clicking %s with unit %d selected, produces an "
								     "ATTACK -- so the refusal measured below is a reading and "
								     "not a machine that refuses everything"),
								*Describe(Target), UnitId),
							static_cast<int32>(Controlled.Command),
							static_cast<int32>(EStratSelectionCommand::Attack)))
					{
						AddError(Controlled.FailureReason);
						return false;
					}
					TestEqual(TEXT("CONTROL: the attack names the attacking unit"),
						Controlled.UnitId, UnitId);
					TestEqual(TEXT("CONTROL: and carries the hex the module enumerated"),
						Controlled.Hex, Target);

					// ---- 1. make the rules module record the act -----------------
					const FString HashBeforeAttack = Bridge.StateHash();
					FString       AttackFailure;
					if (!TestTrue(
							TEXT("the rules module ACCEPTS the control attack, so what follows "
							     "is a real applied command and not a proposed one"),
							StratSubmitSelectionCommand(Bridge, Controlled, AttackFailure)))
					{
						AddError(AttackFailure);
						return false;
					}
					// `Control` is deliberately NOT told. Nothing that knows about this attack
					// on the engine side survives past this line -- `Control` goes out of scope
					// at the end of this block and is never consulted again, and the machine
					// under test below is constructed after it.
					TestNotEqual(
						TEXT("the accepted attack MOVED the rules state -- which is the one "
						     "thing that differs between the control click and the refused "
						     "click below"),
						Bridge.StateHash(), HashBeforeAttack);

					// ---- 2. THE FOREIGN MACHINE ----------------------------------
					// Constructed against a match already in progress. Its `DoneUnits` has
					// never had anything put in it, and no `NotifyCommandApplied` has ever been
					// called on it.
					FStratSelectionMachine Foreign;

					FStratViewModel ForeignModel;
					if (!TestTrue(TEXT("the model rebuilds for the foreign machine"),
							Refresh(Bridge, Foreign, ForeignModel, Error)))
					{
						AddError(Error);
						return false;
					}

					const FStratUnitView* const Acted = FindUnit(ForeignModel, UnitId);
					if (!TestNotNull(
							*FString::Printf(
								TEXT("unit %d survived its own attack and is still on the "
								     "board"), UnitId),
							Acted))
					{
						return false;
					}
					if (!TestEqual(
							TEXT("the side to move has not changed -- no turn was handed over "
							     "between the control click and the foreign one"),
							ForeignModel.Match.SideToMove, Acted->Side))
					{
						return false;
					}

					// ---- 3. THE PRECONDITION, BOTH HALVES, ASSERTED --------------
					if (!TestTrue(
							*FString::Printf(
								TEXT("T-INT-05: the RULES side says unit %d has acted -- "
								     "bHasActed is true on the rebuilt model"), UnitId),
							Acted->bHasActed))
					{
						return false;
					}
					if (!TestFalse(
							*FString::Printf(
								TEXT("T-INT-05: and the ENGINE side does not -- the foreign "
								     "machine's done set has never contained unit %d. This is "
								     "the disagreement the guard exists for, and no click "
								     "sequence can produce it"), UnitId),
							Foreign.IsDone(UnitId)))
					{
						return false;
					}
					TestFalse(
						*FString::Printf(
							TEXT("T-INT-05: so the decorated model draws unit %d as NOT done -- "
							     "bDone is the machine's bit and this machine never set it"),
							UnitId),
						Acted->bDone);

					const FIntPoint ActedHex = Acted->Hex;

					// ---- 4. the two enemy hexes, both read off the MODEL ---------
					// The near one is `AttackTargetHexes`'s own first element, if the unit
					// standing on it survived the attack. The far one is whichever enemy the
					// model carries that stands farthest away. Both are hexes the model says
					// an enemy occupies; neither is a literal.
					const FStratUnitView* const NearEnemy = FindUnitAtHex(ForeignModel, Target);
					const bool bTargetStillOccupied =
						NearEnemy != nullptr && NearEnemy->Side != ForeignModel.Match.SideToMove;

					FIntPoint FarEnemyHex   = FIntPoint::ZeroValue;
					int32     FarEnemyId    = INDEX_NONE;
					int32     FarthestSoFar = -1;
					for (const FStratUnitView& U : ForeignModel.Units)
					{
						if (U.Side == ForeignModel.Match.SideToMove)
						{
							continue;
						}
						if (bTargetStillOccupied && U.Hex == Target)
						{
							continue;
						}
						const int32 D = FixtureHexDistance(ActedHex, U.Hex);
						if (D > FarthestSoFar)
						{
							FarthestSoFar = D;
							FarEnemyHex   = U.Hex;
							FarEnemyId    = U.UnitId;
						}
					}

					// The near hex is preferred because it makes the before/after pair
					// IDENTICAL in every input. If the defender died to the control attack the
					// hex is empty and clicking it would reach the MOVE arm, not the enemy arm,
					// so the far enemy stands in for it -- and the clause says which it used.
					const FIntPoint FirstClick = bTargetStillOccupied ? Target : FarEnemyHex;
					if (!TestTrue(
							TEXT("the model carries a second enemy unit, so the guard can be "
							     "clicked at two different hexes"),
							FarEnemyId != INDEX_NONE && FarEnemyHex != FirstClick))
					{
						return false;
					}
					AddInfo(FString::Printf(
						TEXT("the enumerated target %s is %s after the control attack; the "
						     "clause clicks %s and then %s (enemy unit %d, %d hexes away)"),
						*Describe(Target),
						bTargetStillOccupied ? TEXT("still occupied") : TEXT("now empty"),
						*Describe(FirstClick), *Describe(FarEnemyHex), FarEnemyId,
						FarthestSoFar));

					// ---- 5. the selection succeeds ------------------------------
					const FStratSelectionOutcome Reselect = Foreign.HandleEvent(
						EStratSelectionEvent::HexPrimary, ActedHex, ForeignModel, Query);
					if (!TestEqual(
							*FString::Printf(
								TEXT("T-INT-05: the foreign machine SELECTS unit %d even though "
								     "it has acted -- the done-set gate cannot refuse what was "
								     "never put in the done set, which is what makes the "
								     "enemy-click arm reachable at all"), UnitId),
							Foreign.GetSelectedUnitId(), UnitId))
					{
						AddError(Reselect.FailureReason);
						return false;
					}
					TestTrue(TEXT("and selecting is not a refusal"),
						Reselect.FailureReason.IsEmpty());

					// What the rules module says about this unit's targets NOW. Recorded, and
					// used only to rule out the query-failure arm below; no expectation is
					// taken from it.
					TArray<FIntPoint> TargetsAfter;
					FString           QueryReasonAfter;
					const bool        bQueryOkAfter =
						Query.AttackTargetHexes(UnitId, TargetsAfter, QueryReasonAfter);
					AddInfo(FString::Printf(
						TEXT("after acting, AttackTargetHexes(%d) %s and enumerated %d hexes"),
						UnitId, bQueryOkAfter ? TEXT("answered") : TEXT("refused"),
						TargetsAfter.Num()));

					// ---- 6. THE REFUSAL ------------------------------------------
					const FString HashBeforeClicks = Bridge.StateHash();

					const FStratSelectionOutcome RefusedNear = Foreign.HandleEvent(
						EStratSelectionEvent::HexPrimary, FirstClick, ForeignModel, Query);

					TestEqual(
						*FString::Printf(
							TEXT("T-INT-05: clicking the enemy at %s with an ALREADY-ACTED unit "
							     "selected yields NO command -- the already-acted guard in "
							     "HandleEvent's enemy-click arm. The identical click produced "
							     "an Attack on the control machine above"),
							*Describe(FirstClick)),
						static_cast<int32>(RefusedNear.Command),
						static_cast<int32>(EStratSelectionCommand::None));
					TestEqual(TEXT("T-INT-05: and it commands no unit"),
						RefusedNear.UnitId, static_cast<int32>(INDEX_NONE));
					TestFalse(
						TEXT("T-INT-05: the machine refuses IN ITS OWN WORDS -- see this "
						     "clause's note on the refusal text; the sentence is not pinned"),
						RefusedNear.FailureReason.IsEmpty());
					TestEqual(
						TEXT("T-INT-05: and the selection is left standing -- a refused attack "
						     "does not deselect"),
						RefusedNear.SelectedUnitId, UnitId);
					TestEqual(TEXT("T-INT-05: and the rules state did not move"),
						Bridge.StateHash(), HashBeforeClicks);
					AddInfo(FString::Printf(TEXT("the guard's refusal: %s"),
						*RefusedNear.FailureReason));

					// ---- 7. THE SAME SENTENCE AT A HEX THE MODULE NEVER LISTED ---
					// This is the shape of the guard, and the assertion that survives a guard
					// deletion: the fall-through arm's refusal names the clicked hex's own
					// coordinates, so two different hexes could not produce one sentence.
					const FStratSelectionOutcome RefusedFar = Foreign.HandleEvent(
						EStratSelectionEvent::HexPrimary, FarEnemyHex, ForeignModel, Query);

					TestEqual(
						*FString::Printf(
							TEXT("T-INT-05: clicking the FAR enemy at %s is refused too"),
							*Describe(FarEnemyHex)),
						static_cast<int32>(RefusedFar.Command),
						static_cast<int32>(EStratSelectionCommand::None));
					TestTrue(
						*FString::Printf(
							TEXT("T-INT-05: and it is THE SAME REFUSAL, byte for byte and case "
							     "for case, as the one at %s -- the guard reads the SELECTED "
							     "unit's flag and breaks out before AttackTargetHexes is asked "
							     "anything, so the clicked hex cannot enter the answer. "
							     "near='%s' far='%s'"),
							*Describe(FirstClick), *RefusedNear.FailureReason,
							*RefusedFar.FailureReason),
						RefusedFar.FailureReason.Equals(
							RefusedNear.FailureReason, ESearchCase::CaseSensitive));

					if (!bQueryOkAfter)
					{
						// Only reachable if the rules module refuses to enumerate targets for
						// an acted unit. Then the query-failure arm would produce a
						// hex-independent sentence too, and the equality above would no longer
						// tell the two arms apart -- so it is ruled out explicitly, against the
						// module's own reason rather than against a literal.
						TestNotEqual(
							TEXT("T-INT-05: and the refusal is the GUARD's, not the "
							     "query-failure arm's -- it differs from what "
							     "AttackTargetHexes itself said"),
							RefusedNear.FailureReason, QueryReasonAfter);
					}

					// ---- 8. AND IT IS NOT THE DONE-SET ARM'S SENTENCE ------------
					// Both sides of this comparison are the machine's own words, about the SAME
					// unit id, on the same run. This file writes neither string. An inequality
					// between sentences about DIFFERENT units would have held whichever arm
					// produced them, and would have proved nothing.
					const FStratSelectionOutcome Spent = Foreign.HandleEvent(
						EStratSelectionEvent::Wait, FIntPoint::ZeroValue, ForeignModel, Query);
					if (!TestTrue(TEXT("the wait spends the still-selected unit"),
							Foreign.IsDone(UnitId)))
					{
						AddError(Spent.FailureReason);
						return false;
					}

					const FStratSelectionOutcome Finished = Foreign.HandleEvent(
						EStratSelectionEvent::HexPrimary, ActedHex, ForeignModel, Query);
					TestFalse(TEXT("clicking the now-finished unit is refused, with a reason"),
						Finished.FailureReason.IsEmpty());
					TestNotEqual(
						*FString::Printf(
							TEXT("T-INT-05: the already-acted refusal is a DIFFERENT statement "
							     "from the already-finished one, about the same unit %d -- the "
							     "guard has its own arm and this clause did not drive the done "
							     "set by accident. acted='%s' finished='%s'"),
							UnitId, *RefusedNear.FailureReason, *Finished.FailureReason),
						RefusedNear.FailureReason, Finished.FailureReason);

					AddInfo(FString::Printf(
						TEXT("T-INT-05: unit %d acted on turn %d and a machine that never saw "
						     "it act refused its enemy click, after %d advancing moves over %d "
						     "turns"),
						UnitId, Bridge.Turn(), MovesPlayed, TurnsPlayed));

					bPinned = true;
					break;
					// ==============================================================
				}

				// ---- no target: advance, then look again ---------------------
				if (Attempt > 0 || Unit->bHasMoved)
				{
					break;
				}

				FIntPoint Dest;
				if (!PickAdvance(Query, Model, UnitId, Unit->Hex, Unit->Side, Dest, Error))
				{
					break;
				}

				const FStratSelectionOutcome MoveOutcome =
					Watcher.HandleEvent(EStratSelectionEvent::HexPrimary, Dest, Model, Query);
				if (MoveOutcome.Command != EStratSelectionCommand::Move)
				{
					break;
				}

				FString MoveFailure;
				if (!TestTrue(
						*FString::Printf(
							TEXT("leg %d: the module's own listed move for unit %d is accepted"),
							Leg, UnitId),
						StratSubmitSelectionCommand(Bridge, MoveOutcome, MoveFailure)))
				{
					AddError(MoveFailure);
					return false;
				}
				Watcher.NotifyCommandApplied(MoveOutcome);
				++MovesPlayed;

				if (!Refresh(Bridge, Watcher, Model, Error))
				{
					AddError(Error);
					return false;
				}
				Unit = FindUnit(Model, UnitId);
				if (Unit == nullptr)
				{
					break;
				}
				ClosestContact = FMath::Min(ClosestContact,
					NearestEnemyDistance(Model, Unit->Hex, Unit->Side));
			}
		}

		if (bPinned)
		{
			break;
		}

		// ---- hand over --------------------------------------------------------
		FStratViewModel EndModel;
		if (!TestTrue(*FString::Printf(TEXT("leg %d: the model rebuilds before the hand-over"), Leg),
				Refresh(Bridge, Watcher, EndModel, Error)))
		{
			AddError(Error);
			return false;
		}
		const FStratSelectionOutcome EndOutcome = Watcher.HandleEvent(
			EStratSelectionEvent::EndTurn, FIntPoint::ZeroValue, EndModel, Query);
		FString EndFailure;
		if (!TestTrue(*FString::Printf(TEXT("leg %d: the end of turn is accepted"), Leg),
				StratSubmitSelectionCommand(Bridge, EndOutcome, EndFailure)))
		{
			AddError(EndFailure);
			return false;
		}
		Watcher.NotifyCommandApplied(EndOutcome);
	}

	// A SKIP IS A FAILURE. Without contact there is no acted unit and the clause asserted
	// nothing about the guard; a silent green would be the shape this milestone was blocked
	// for twice.
	TestTrue(
		*FString::Printf(
			TEXT("T-INT-05: a unit was brought into contact and made to act within %d turns -- "
			     "the closest the two sides came was %d hexes, over %d advancing moves"),
			kMaxTurns, ClosestContact, MovesPlayed),
		bPinned);

	return bPinned;
}
