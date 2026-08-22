// GDD §4.9 part 2 -- the bridge. See StratBridge.h for what it owns and decides.
#include "StratBridge.h"

#include "Engine/DataTable.h"
#include "Misc/FileHelper.h"
#include "Modules/ModuleManager.h"

#include "StratData/StratDataRows.h"

// A REAL module object, unlike Source/StratRules/. That directory holds vendored
// C++ with no IMPLEMENT_MODULE, and listing it in Stratocracy.uproject once made
// the editor abort at startup with "The game module 'StratRules' could not be
// successfully initialized" -- recorded in §3. This module is our own code, so it
// carries the module object the loader requires and can be listed safely.
IMPLEMENT_MODULE(FDefaultModuleImpl, StratBridge)

// The bridge's first log category. Declared in StratCombatLog.h, which StratBridge.h
// includes; defined here, in the module .cpp, exactly as StratPlay.cpp:16 defines
// LogStratPlay. Before this line the module contained zero UE_LOG calls -- measured,
// `grep -rn UE_LOG Source/StratBridge/` was empty at 538cfe0.
DEFINE_LOG_CATEGORY(LogStratBridge)

namespace
{
	// EUnitType mirrors strat::UnitType, and the mirror is pinned by
	// static_assert in StratDataParity.cpp and asserted at runtime by T-DATA-05.
	// This cast rides on that guarantee rather than restating it as a switch a
	// fifth enumerator could silently fall through.
	strat::UnitType ToStratUnitType(EUnitType In)
	{
		return static_cast<strat::UnitType>(static_cast<uint8>(In));
	}

	std::string ToStd(const FString& In)
	{
		return std::string(TCHAR_TO_UTF8(*In));
	}

	FString FromStd(const std::string& In)
	{
		return FString(UTF8_TO_TCHAR(In.c_str()));
	}
}

FStratResult FStratBridge::LoadDefinitions(const UDataTable* UnitTable, const UDataTable* TerrainTable)
{
	if (UnitTable == nullptr || TerrainTable == nullptr)
	{
		return FStratResult::Fail(TEXT("a definition table was null"));
	}

	std::vector<strat::UnitDef>    NextUnits;
	std::vector<strat::TerrainDef> NextTerrain;

	// GetRowNames() returns rows in the table's own order, which for a
	// CSV-imported table is the CSV's order. That equality is not assumed here:
	// the editor pass asserts this vector against strat::loadUnits over the same
	// vendored bytes, order included, because defIndex depends on it.
	for (const FName& RowName : UnitTable->GetRowNames())
	{
		const FUnitRow* Row = UnitTable->FindRow<FUnitRow>(RowName, TEXT("FStratBridge::LoadDefinitions"));
		if (Row == nullptr)
		{
			return FStratResult::Fail(FString::Printf(
				TEXT("unit row '%s' is not an FUnitRow"), *RowName.ToString()));
		}

		strat::UnitDef Def;
		// The row NAME is the id: FUnitRow carries no Id field, because the
		// table's key already is one and a second copy could disagree with it.
		Def.id         = ToStd(RowName.ToString());
		Def.hpMax      = Row->HP;
		Def.move       = Row->Move;
		Def.atk        = Row->Atk;
		Def.def        = Row->Def;
		Def.rangeMin   = Row->RangeMin;
		Def.rangeMax   = Row->RangeMax;
		Def.costFame   = Row->CostFame;
		Def.type       = ToStratUnitType(Row->Type);
		Def.canCapture = Row->CanCapture;
		NextUnits.push_back(Def);
	}

	for (const FName& RowName : TerrainTable->GetRowNames())
	{
		const FTerrainRow* Row = TerrainTable->FindRow<FTerrainRow>(RowName, TEXT("FStratBridge::LoadDefinitions"));
		if (Row == nullptr)
		{
			return FStratResult::Fail(FString::Printf(
				TEXT("terrain row '%s' is not an FTerrainRow"), *RowName.ToString()));
		}

		strat::TerrainDef Def;
		Def.id            = ToStd(RowName.ToString());
		Def.moveCost      = Row->MoveCost;
		Def.defensePct    = Row->DefensePct;
		Def.passLand      = Row->PassLand;
		Def.passAir       = Row->PassAir;
		Def.passSea       = Row->PassSea;
		Def.capturable    = Row->Capturable;
		Def.incomeFame    = Row->IncomeFame;
		Def.isSpawnPoint  = Row->IsSpawnPoint;
		Def.isRepairPoint = Row->IsRepairPoint;
		NextTerrain.push_back(Def);
	}

	Units   = std::move(NextUnits);
	Terrain = std::move(NextTerrain);
	bDefinitionsLoaded = true;
	// Definitions changed underneath it, so any prior seed is stale rather than
	// merely old: its defIndex values point into the vector that just moved.
	bSeeded = false;
	// And the retained scenario goes with it, for the same reason and one more:
	// it was validated against the OLD tables, so its guided seats and its unit
	// ids are claims about a table that no longer exists. Dropping it makes
	// every unit unmarked until a scenario is loaded again, which is absence and
	// not a wrong answer.
	LoadedScenario = strat::Scenario();
	// And the recorded log with it. Every Build entry in it carries a defIndex
	// into the vector that just moved, so the log is not merely old -- it names
	// different unit types than it did a line ago.
	Recorded.clear();
	// And §2.9's buildlist, for exactly that reason: its entries ARE defIndexes
	// into the vector that just moved. Carried across a reload it would silently
	// tell the AI to build a different unit type than the caller named.
	Buildlist.clear();
	return FStratResult::Ok();
}

strat::RulesTables FStratBridge::Tables() const
{
	strat::RulesTables T;
	T.units   = &Units;
	T.terrain = &Terrain;
	return T;
}

FStratResult FStratBridge::LoadScenarioFromFile(const FString& ScenarioFilePath, int32 FirstSide)
{
	if (!bDefinitionsLoaded)
	{
		return FStratResult::Fail(TEXT("definitions are not loaded"));
	}

	FString Text;
	if (!FFileHelper::LoadFileToString(Text, *ScenarioFilePath))
	{
		return FStratResult::Fail(FString::Printf(
			TEXT("scenario file unreadable at %s"), *ScenarioFilePath));
	}

	// The module parses and validates; the bridge supplies bytes and a path to
	// name in the error, and reads no field of the file itself.
	strat::Scenario Parsed;
	const strat::ScenarioLoadResult ParseResult =
		strat::parseScenario(ToStd(Text), ToStd(ScenarioFilePath), Parsed);
	if (!ParseResult.ok)
	{
		return FStratResult::Fail(FromStd(ParseResult.reason), FromStd(ParseResult.failedId));
	}

	const strat::ScenarioLoadResult ValidResult =
		strat::validateScenario(Parsed, Units, Terrain);
	if (!ValidResult.ok)
	{
		return FStratResult::Fail(FromStd(ValidResult.reason), FromStd(ValidResult.failedId));
	}

	std::string Err;
	if (!strat::seedFromScenario(GameState, Parsed, Tables(), FirstSide, Err))
	{
		return FStratResult::Fail(FromStd(Err));
	}

	// Retained only on the success path, and only after the state it describes
	// exists. A scenario kept alongside a state it failed to seed would be a
	// guided-seat list for units that are not there.
	LoadedScenario = std::move(Parsed);

	// A new seed, so the old log describes a match that no longer exists. Cleared
	// on the success path only: a failed load leaves the previous match intact and
	// its log with it.
	Recorded.clear();

	bSeeded = true;
	return FStratResult::Ok();
}

// ---------------------------------------------------------------------------
// The divergence rule. DECLARED in StratCombatLog.h -- see that file for why it is a
// free function over an engine-typed struct rather than a member or a file-local
// helper, and why it carries no `_API` macro. Defined here rather than inline in the
// header so the emitter below and the in-module clauses link against the same object
// code, and so a drift between the two shows up as a link error rather than as two
// silently-different inlinings.
//
// NOTHING IN EITHER FUNCTION NAMES A `strat::` TYPE. That is what makes them callable
// from a clause that never seeds a board.
// ---------------------------------------------------------------------------
bool StratLossAgrees(int32 HpBefore, int32 HpAfter, bool bDied, int32 ExpectedDamage, bool bExpectDeath)
{
	if (bDied != bExpectDeath)
	{
		return false;
	}
	if (bDied)
	{
		return ExpectedDamage >= HpBefore;
	}
	if (HpBefore == INDEX_NONE || HpAfter == INDEX_NONE)
	{
		return false;
	}
	return (HpBefore - HpAfter) == ExpectedDamage;
}

int32 StratDivergenceMaskOf(const FStratCombatOutcome& Outcome)
{
	int32 Mask = EStratCombatDivergence::None;

	if (!Outcome.bForecastLegal)
	{
		// The rules applied an attack the forecast called illegal. That is the two
		// halves of §2.6 disagreeing about whether the act was even available, and it
		// is a sharper fault than any damage mismatch.
		Mask |= EStratCombatDivergence::LegalityDisagrees;
	}

	if (!StratLossAgrees(Outcome.DefenderHpBefore, Outcome.DefenderHpAfter, Outcome.bDefenderDied,
	                     Outcome.ForecastDamage, Outcome.bForecastDefenderDies))
	{
		Mask |= EStratCombatDivergence::DefenderLoss;
	}

	// A counter that does not fire predicts a loss of ZERO, which is not the same as
	// predicting nothing -- and `counterFires` true with `counterDamage` 0 predicts
	// zero too. Folding both through the same expected-damage comparison is what
	// keeps a 0-damage counter from reading as a divergence.
	const int32 ExpectedCounter    = Outcome.bForecastCounterFires ? Outcome.ForecastCounterDamage : 0;
	const bool  bExpectAttackerDie = Outcome.bForecastCounterFires
	                                 && Outcome.ForecastCounterDamage >= Outcome.AttackerHpBefore;
	if (!StratLossAgrees(Outcome.AttackerHpBefore, Outcome.AttackerHpAfter, Outcome.bAttackerDied,
	                     ExpectedCounter, bExpectAttackerDie))
	{
		Mask |= EStratCombatDivergence::CounterLoss;
	}

	return Mask;
}

// ---------------------------------------------------------------------------
// StratCombatObservation -- the `STRAT-COMBAT` family, and the only place in the
// project that compares §2.6's forecast against what §2.6 actually did.
//
// WHY IT HAS TO BE ASSEMBLED AT ALL. `applyCommand`'s Attack case returns
// `okResult(1)` and nothing else (`strat::applyCommand`'s `Attack` arm). No damage, no death,
// no counter, no fame comes back through the return channel, so an observer has
// exactly two places to look: `strat::uiForecast` BEFORE the submit, and the
// difference between two `strat::UiSnapshot`s taken either side of it. This block
// reads both and DERIVES NEITHER -- every predicted number is `uiForecast`'s and
// every measured number is one snapshot field minus the same snapshot field.
//
// THE ONE PIECE OF ARITHMETIC THIS BLOCK REACHES IS `StratDivergenceMaskOf`
// (declared in StratCombatLog.h, defined just above), AND IT DECIDES NOTHING.
// It is the divergence detector; it compares two answers that already exist and
// its result reaches a log line and nothing else. No caller branches on it, no
// state depends on it, and if it were deleted the game would play identically.
// That is the line between a diagnostic and the rules answer §4.1 forbids this
// module to compute, and it is why `strat::uiResolveForGate` (returning `strat::UiResolution`) is
// NOT called from here: that function is the gate's independent oracle, and a
// production caller would make phase 2's clauses check the bridge against a value
// the bridge itself had just asked for.
//
// WHY IT IS FILE-LOCAL FREE FUNCTIONS AND NOT `FStratBridge` METHODS. Every one
// of them needs only the bridge's PUBLIC surface -- `State()`, `MakeUiSnapshot`,
// `Forecast` -- so making them members would widen the exported class for no
// caller. They are not `static` members for the same reason.
// ---------------------------------------------------------------------------
namespace StratCombatObservation
{
	/** The pre-submit projection, kept alive across `applyCommand` so the fame and HP
	 *  fields have a "before" to be measured against. */
	struct FBefore
	{
		bool              bValid = false;
		strat::UiSnapshot Snapshot;
	};

	const strat::UiUnitView* FindUnitById(const strat::UiSnapshot& S, int32 Id)
	{
		return (Id == INDEX_NONE) ? nullptr : strat::findUiUnitView(S, Id);
	}

	/** The unit standing on a hex, by walking the snapshot's own unit list. Deliberately
	 *  NOT a board lookup into `GameState`: the measured side of this record must come
	 *  off the same projection the screen reads, or a divergence here could be a
	 *  projection bug wearing a combat bug's clothes. */
	const strat::UiUnitView* FindUnitOnHex(const strat::UiSnapshot& S, const strat::Hex& H)
	{
		for (const strat::UiUnitView& U : S.units)
		{
			if (U.hex.q == H.q && U.hex.r == H.r)
			{
				return &U;
			}
		}
		return nullptr;
	}

	// `LossAgrees` AND THE MASK RULE USED TO LIVE HERE. They are now `StratLossAgrees`
	// and `StratDivergenceMaskOf` in StratCombatLog.h, promoted in the combat-outcome
	// milestone because their non-agreement arms were unreachable from any clause: a
	// forecast-illegal attack that applies is not constructible through the bridge, the
	// two rules entry points sharing `resolveDamage` / `defenderCanCounter` (74 Attacks
	// measured, zero divergences). The functions moved; the reasoning that shaped them
	// moved with them and is not restated here. `CaptureAfter` below calls them.
	void CaptureBefore(const FStratBridge&        Bridge,
	                   const strat::SaveCommand&  Command,
	                   FStratCombatOutcome&       Out,
	                   FBefore&                   Before)
	{
		Out.AttackerId = Command.unitId;
		Out.TargetHex  = FIntPoint(Command.hex.q, Command.hex.r);
		// READ BEFORE THE SUBMIT, for the reason `StratSubmitSelectionCommand` reads them
		// before: the command that closes turn N is tagged N, and a post-submit read of a
		// turn-ending resolution would describe a turn nobody played in.
		Out.Turn       = Bridge.State().turn.turnNumber;
		Out.ActiveSide = Bridge.State().turn.activeSide;

		Before.bValid = Bridge.MakeUiSnapshot(Before.Snapshot).bOk;
		if (Before.bValid)
		{
			if (const strat::UiUnitView* Attacker = FindUnitById(Before.Snapshot, Out.AttackerId))
			{
				Out.AttackerSide     = Attacker->side;
				Out.AttackerHpBefore = Attacker->hp;
				Out.AttackerHpMax    = Attacker->hpMax;
			}
			if (const strat::UiUnitView* Defender = FindUnitOnHex(Before.Snapshot, Command.hex))
			{
				Out.DefenderId       = Defender->id;
				Out.DefenderHpBefore = Defender->hp;
				Out.DefenderHpMax    = Defender->hpMax;
			}
			if (Out.AttackerSide >= 0 && Out.AttackerSide < strat::SIDE_COUNT)
			{
				const strat::UiSideView& S = Before.Snapshot.side[Out.AttackerSide];
				Out.AttackerFameTotalBefore  = S.fameTotal;
				Out.AttackerFameCombatBefore = S.fameCombat;
			}
		}

		// THE FORECAST, and the two channels are kept apart exactly as
		// `FStratBridge::Forecast`'s "TWO CHANNELS" block says to: `bOk` is whether the
		// question could be ASKED, `legal` is what the rules ANSWERED. An out-of-range
		// attack is Ok() with legal=false. (Cited by function name -- this comment first
		// said `StratBridge.h:392-399`, and the same diff that wrote it pushed the target
		// 24 lines down and left it pointing at `Reachable`.)
		strat::UiForecast F;
		Out.bForecastQueried = Bridge.Forecast(Out.AttackerId, Command.hex, F).bOk;
		if (Out.bForecastQueried)
		{
			Out.bForecastLegal          = F.legal;
			Out.ForecastDistance        = F.distance;
			Out.ForecastDamage          = F.damage;
			Out.bForecastDefenderDies   = F.defenderDies;
			Out.bForecastCounterFires   = F.counterFires;
			Out.ForecastCounterDamage   = F.counterDamage;
		}
	}

	void CaptureAfter(const FStratBridge& Bridge, const FBefore& Before, FStratCombatOutcome& Out)
	{
		Out.bApplied = true;

		strat::UiSnapshot After;
		const bool bAfterValid = Bridge.MakeUiSnapshot(After).bOk;
		if (!bAfterValid || !Before.bValid)
		{
			// Left at -1 / unmeasurable rather than guessed. See `ForecastAgrees`.
			return;
		}

		// ABSENCE FROM THE ROSTER IS THE DEATH TEST, and it is the only one available:
		// the snapshot carries no "is dead" field because a dead unit is not projected.
		if (const strat::UiUnitView* Attacker = FindUnitById(After, Out.AttackerId))
		{
			Out.AttackerHpAfter = Attacker->hp;
		}
		else
		{
			Out.bAttackerDied = (Out.AttackerHpBefore != INDEX_NONE);
		}

		if (const strat::UiUnitView* Defender = FindUnitById(After, Out.DefenderId))
		{
			Out.DefenderHpAfter = Defender->hp;
		}
		else
		{
			Out.bDefenderDied = (Out.DefenderId != INDEX_NONE);
		}

		if (Out.AttackerSide >= 0 && Out.AttackerSide < strat::SIDE_COUNT)
		{
			const strat::UiSideView& S = After.side[Out.AttackerSide];
			Out.AttackerFameTotalAfter  = S.fameTotal;
			Out.AttackerFameCombatAfter = S.fameCombat;
		}

		// ---- The agreement, the reason this record exists at all ----------
		if (!Out.bForecastQueried || Out.DefenderId == INDEX_NONE || Out.AttackerHpBefore == INDEX_NONE)
		{
			return;   // ForecastAgrees stays -1
		}

		// ONE CALL, AND NO SECOND COPY OF THE RULE. Everything above this line is
		// measurement -- reading two snapshots and a forecast. The clause comparison
		// itself is `StratDivergenceMaskOf`, and the reason it is not written out here
		// is that a clause hand-building an `FStratCombatOutcome` must exercise the same
		// bytes this emitter does, not a sibling of them.
		const int32 Mask = StratDivergenceMaskOf(Out);

		Out.DivergenceMask  = Mask;
		Out.ForecastAgrees  = (Mask == EStratCombatDivergence::None) ? 1 : 0;
	}

	/** Every field numeric, fixed order, -1 where a field has no value -- phase 4's
	 *  `STRAT-CMD` discipline, so the line's SHAPE never depends on its content. */
	FString DescribeCommonFields(const FStratCombatOutcome& O)
	{
		return FString::Printf(
			TEXT("attacker=%d defender=%d hex=%d,%d turn=%d side=%d attackerSide=%d ")
			TEXT("fqueried=%d flegal=%d fdist=%d fdmg=%d fdies=%d fcounter=%d fcdmg=%d"),
			O.AttackerId, O.DefenderId, O.TargetHex.X, O.TargetHex.Y,
			O.Turn, O.ActiveSide, O.AttackerSide,
			O.bForecastQueried ? 1 : 0, O.bForecastLegal ? 1 : 0,
			O.ForecastDistance, O.ForecastDamage,
			O.bForecastDefenderDies ? 1 : 0, O.bForecastCounterFires ? 1 : 0,
			O.ForecastCounterDamage);
	}

	/**
	 * THE EMITTER. One call site, in `FStratBridge::Submit`, on the Attack kind only.
	 *
	 * `refused` IS A DIFFERENT PHRASE FROM `resolved`, and it does not contain the word
	 * `resolved`, for the reason `STRAT-CMD refused` does not contain `accepted`
	 * (StratSelectionMachine.cpp:571-575): `grep -c "STRAT-COMBAT resolved"` must count
	 * attacks that ACTUALLY APPLIED, and a refusal that shared the phrase would inflate
	 * it silently. `STRAT-COMBAT divergence` is a third phrase for the same reason -- it
	 * is greppable on its own, and it is emitted BESIDE the resolved line rather than
	 * instead of it, so the resolved count stays a count of resolutions.
	 */
	void EmitResolved(const FStratCombatOutcome& O)
	{
		UE_LOG(LogStratBridge, Log,
			TEXT("STRAT-COMBAT resolved %s ahpBefore=%d ahpAfter=%d ahpMax=%d adied=%d ")
			TEXT("dhpBefore=%d dhpAfter=%d dhpMax=%d ddied=%d ")
			TEXT("fameBefore=%d fameAfter=%d fameCombatBefore=%d fameCombatAfter=%d ")
			TEXT("agree=%d diverge=%d"),
			*DescribeCommonFields(O),
			O.AttackerHpBefore, O.AttackerHpAfter, O.AttackerHpMax, O.bAttackerDied ? 1 : 0,
			O.DefenderHpBefore, O.DefenderHpAfter, O.DefenderHpMax, O.bDefenderDied ? 1 : 0,
			O.AttackerFameTotalBefore, O.AttackerFameTotalAfter,
			O.AttackerFameCombatBefore, O.AttackerFameCombatAfter,
			O.ForecastAgrees, O.DivergenceMask);

		if (O.ForecastAgrees == 0)
		{
			// The defect the `defenderCanCounter` note above `strat::uiForecast` says the forecast's
			// construction exists to catch, caught
			// on a real submit rather than in a gate fixture. Error level because there is
			// no benign reading of it: either the screen lied to the player or the rules
			// module has two answers for one question.
			UE_LOG(LogStratBridge, Error,
				TEXT("STRAT-COMBAT divergence %s diverge=%d"),
				*DescribeCommonFields(O), O.DivergenceMask);
		}
	}

	/**
	 * `Log` AND NOT `Warning`, AND THAT LEVEL IS MEASURED RATHER THAN CHOSEN BY TASTE.
	 *
	 * A refused attack is the rules module correctly saying no -- "which is the interface
	 * working", in StratPlayerController.cpp:267's words. It is what a player gets for
	 * clicking an out-of-range hex, and it is not a fault.
	 *
	 * The first cut emitted it at `Warning`, matching `STRAT-CMD refused`. MEASURED: the
	 * automation framework captures warnings into whichever test provoked them, and this
	 * one line downgraded two existing tests that deliberately submit illegal attacks --
	 * `T-SAVE-06.RejectedCommandIsNotRecorded` and
	 * `T-UI-01.SubmitAttackAtHexIsNotTransposed` -- taking the suite from
	 * 87/0/0/0 to 85 succeeded / 0 failed / 0 notRun / 2 succeededWithWarnings, with no
	 * test failing and nothing in the tree actually broken. `STRAT-CMD refused` gets away
	 * with `Warning` because it is emitted from `StratPlay` on a path no bridge test
	 * takes; this one sits under every test in the project that submits a command.
	 *
	 * `STRAT-COMBAT divergence` KEEPS ITS `Error` LEVEL, and the asymmetry is the point:
	 * automation treats a logged error as a test FAILURE, and a forecast that disagrees
	 * with its own resolution should fail whatever suite observes it. That is an
	 * obligation on any future test that provokes a divergence deliberately -- it must
	 * declare the error with `AddExpectedError`, and it must not lower this level.
	 */
	void EmitRefused(const FStratCombatOutcome& O, const FString& Reason)
	{
		UE_LOG(LogStratBridge, Log,
			TEXT("STRAT-COMBAT refused %s reason=%s"),
			*DescribeCommonFields(O), *Reason);
	}
}

FStratResult FStratBridge::Submit(const strat::SaveCommand& Command)
{
	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}

	// THE ONE OBSERVATION POINT, AND IT SITS BELOW THE `!bSeeded` GUARD ABOVE. An attack
	// submitted to an unseeded bridge returns there and reaches neither emitter -- no
	// `resolved`, no `refused`, no line of any kind. That is the right answer (there is
	// no state to project, and a line of all -1 would say less than no line) but it is
	// NOT a free choice, because it makes `resolved + refused` something other than the
	// count of attacks submitted. A pairing gate that assumes those numbers reconcile
	// would be silently off by however many commands arrived before a scenario did.
	//
	// `SubmitAttackAtHex` -> `SubmitAttack` -> `SubmitStamped` -> here, and the only
	// other callers of this method in the tree (StratBridgeParity.cpp:340,
	// StratBridgeSaveRecording.cpp:466) hand it a raw SaveCommand and also arrive here.
	// Both of those submit a DELIBERATELY ILLEGAL command that refuses, so no attack in
	// the tree today APPLIES without passing a typed arm -- observing in `SubmitAttack`
	// would not currently miss anything. It is still the wrong place: the guarantee this
	// method gives is that a future raw Attack which does apply cannot escape, and
	// observing in both places would double-count every attack instead.
	const bool bIsAttack = (Command.kind == strat::SaveCommandKind::Attack);

	FStratCombatOutcome              Outcome;
	StratCombatObservation::FBefore  Before;
	if (bIsAttack)
	{
		StratCombatObservation::CaptureBefore(*this, Command, Outcome, Before);
	}

	const strat::ReplayResult R = strat::applyCommand(GameState, Command, Tables());
	if (!R.ok)
	{
		if (bIsAttack)
		{
			StratCombatObservation::EmitRefused(Outcome, FromStd(R.reason));
		}
		return FStratResult::Fail(FromStd(R.reason), FromStd(R.failedId));
	}

	// AFTER the module accepted it, and only then. §4.9 says a rejected command
	// changes nothing; a log that recorded the attempt would be a change.
	Recorded.push_back(Command);

	if (bIsAttack)
	{
		StratCombatObservation::CaptureAfter(*this, Before, Outcome);
		StratCombatObservation::EmitResolved(Outcome);
	}

	return FStratResult::Ok();
}

FStratResult FStratBridge::SubmitStamped(strat::SaveCommand Command)
{
	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}

	// READ, not decided, and read BEFORE the command is applied. `applyCommand`
	// refuses an entry whose tag disagrees with the live turn, and an EndTurn read
	// back afterwards would carry the turn it opened rather than the one it closed.
	Command.turn = GameState.turn.turnNumber;
	Command.side = GameState.turn.activeSide;

	return Submit(Command);
}

FStratResult FStratBridge::SubmitMove(int32 UnitId, const strat::Hex& DestHex)
{
	strat::SaveCommand C;
	C.kind    = strat::SaveCommandKind::Move;
	C.unitId  = UnitId;
	C.hex     = DestHex;
	C.hasUnit = true;
	C.hasHex  = true;
	return SubmitStamped(C);
}

FStratResult FStratBridge::SubmitAttack(int32 UnitId, const strat::Hex& TargetHex)
{
	strat::SaveCommand C;
	C.kind    = strat::SaveCommandKind::Attack;
	C.unitId  = UnitId;
	C.hex     = TargetHex;
	C.hasUnit = true;
	C.hasHex  = true;
	return SubmitStamped(C);
}

FStratResult FStratBridge::SubmitBuild(const strat::Hex& FactoryHex, int32 DefIndex)
{
	strat::SaveCommand C;
	C.kind = strat::SaveCommandKind::Build;
	// The format's `unitId` field, which for a Build is the §2.4 ROW INDEX and not
	// a unit id. `SaveCommand::unitId` names both meanings on one line; this is the second.
	C.unitId  = DefIndex;
	C.hex     = FactoryHex;
	C.hasUnit = true;
	C.hasHex  = true;
	return SubmitStamped(C);
}

FStratResult FStratBridge::SubmitCapture(int32 UnitId)
{
	strat::SaveCommand C;
	C.kind    = strat::SaveCommandKind::Capture;
	C.unitId  = UnitId;
	C.hasUnit = true;
	// No hex: `SaveCommand::hasHex` states Capture names none, and a hex set here would be a
	// field of another kind's command, which the parser REFUSES rather than
	// ignores.
	C.hasHex  = false;
	return SubmitStamped(C);
}

FStratResult FStratBridge::SubmitEndTurn()
{
	strat::SaveCommand C;
	C.kind = strat::SaveCommandKind::EndTurn;
	// The one kind that names neither a unit nor a hex (`SaveCommand::hasHex` / `hasUnit`).
	C.hasUnit = false;
	C.hasHex  = false;
	return SubmitStamped(C);
}

FStratResult FStratBridge::ReplayLog(const TArray<strat::SaveCommand>& Log)
{
	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}

	std::vector<strat::SaveCommand> AsVector;
	AsVector.reserve(static_cast<size_t>(Log.Num()));
	for (const strat::SaveCommand& C : Log)
	{
		AsVector.push_back(C);
	}

	const strat::ReplayResult R = strat::replayLog(GameState, AsVector, Tables());
	if (!R.ok)
	{
		return FStratResult::Fail(
			FString::Printf(TEXT("%s (at index %d)"), *FromStd(R.reason), R.failedIndex),
			FromStd(R.failedId));
	}

	// All-or-nothing at the module means all-or-nothing here: `replayLog` assigned
	// only after the last command succeeded, so appending the whole log keeps
	// "recorded" equal to "applied" without this file re-deciding either.
	Recorded.insert(Recorded.end(), AsVector.begin(), AsVector.end());
	return FStratResult::Ok();
}

FString FStratBridge::StateHash() const
{
	return FromStd(strat::canonicalStateHash(GameState));
}

// ---------------------------------------------------------------------------
// View model (§4.7 Stub 8).
// ---------------------------------------------------------------------------

strat::Unit FStratBridge::CombatUnitOf(const strat::GameUnit& U) const
{
	// Field for field with the driver's `combatUnit`, deliberately: the stat
	// block combat reads is the UnitDef's, and only `hp` comes from the unit.
	// Every value is LOOKED UP on each call. Caching one would be a second copy
	// of a §2.4 row that a table reload cannot reach.
	const strat::UnitDef& D = Units[static_cast<size_t>(U.defIndex)];

	strat::Unit C;
	C.atk      = D.atk;
	C.def      = D.def;
	C.hp       = U.hp;
	C.hpMax    = D.hpMax;
	C.rangeMin = D.rangeMin;
	C.rangeMax = D.rangeMax;
	C.type     = D.type;
	return C;
}

strat::UiWorld FStratBridge::MakeUiWorld() const
{
	strat::UiWorld W;

	// The board is DERIVED by the module from the state we own -- `boardOf` is
	// the same one-source-for-occupancy rule Replay.h states. The driver had to
	// build its own because its roster is DriverUnits; ours is GameUnits, so the
	// module's own derivation is reachable and hand-rolling a second one here
	// would be the drift this module exists to prevent.
	W.board    = strat::boardOf(GameState);
	W.unitDefs = &Units;
	W.terrain  = &Terrain;
	W.economy  = &GameState.economy;
	W.turn     = &GameState.turn;

	// The guided seats come from the loaded file and from nowhere else, exactly
	// as in `uiWorldOf`. An unseeded bridge holds a default Scenario, so this is
	// null and every unit is unmarked -- absence, not a default. It is null
	// rather than "the empty list of the scenario we happen to be holding"
	// because those are different claims and `isGuidedMarked` should not be
	// answering on behalf of a scenario that was never loaded.
	W.guided   = bSeeded ? &LoadedScenario.guided : nullptr;

	for (const strat::GameUnit& U : GameState.units)
	{
		// Unreachable on a seeded bridge: validateScenario pins every defIndex
		// against the same table `Units` was built from, and applyCommand takes
		// its defIndex through the same check. It is guarded anyway because the
		// alternative to skipping is reading past the end of the table, and the
		// alternative to BOTH -- emitting the unit with zeroed stats -- would
		// put a made-up §2.4 row in front of the player. Dropping invents
		// nothing; MakeUiSnapshot refuses before it can happen.
		if (U.defIndex < 0 || static_cast<size_t>(U.defIndex) >= Units.size())
		{
			ensureMsgf(false, TEXT("unit %d carries defIndex %d, outside the loaded table"),
				U.id, U.defIndex);
			continue;
		}

		strat::UiUnit V;
		V.id        = U.id;
		V.side      = U.side;
		V.defIndex  = U.defIndex;
		V.hex       = U.hex;
		// The DEPLOYMENT hex, not the current one. Ui.h is explicit that
		// `isGuidedMarked` is a property of the placement and does not move when
		// the unit does; passing `hex` here would unmark the marked Infantry the
		// instant beat 1a moved it.
		V.placement = U.placement;
		V.unit      = CombatUnitOf(U);
		// The module's own derivation from the per-side designation. Read
		// through `isFlagUnit` rather than by comparing `flagUnit[side]`
		// in-line, because a dead flag is absent from `units` and the rule for
		// what that means belongs to the module that owns the designation.
		V.isFlag    = strat::isFlagUnit(GameState, U);
		W.units.push_back(V);
	}

	return W;
}

FStratResult FStratBridge::MakeUiSnapshot(strat::UiSnapshot& OutSnapshot) const
{
	if (!bDefinitionsLoaded)
	{
		return FStratResult::Fail(TEXT("definitions are not loaded"));
	}
	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}

	for (const strat::GameUnit& U : GameState.units)
	{
		if (U.defIndex < 0 || static_cast<size_t>(U.defIndex) >= Units.size())
		{
			return FStratResult::Fail(FString::Printf(
				TEXT("unit %d carries defIndex %d, outside the loaded unit table"),
				U.id, U.defIndex));
		}
	}

	// The world does not escape this function, which is the whole reason to
	// prefer this entry point: every pointer in it borrows from `this`, and a
	// caller holding one past a LoadDefinitions would be reading a table that
	// moved.
	const strat::UiWorld World = MakeUiWorld();

	// The one line this method exists for. Nothing above it computed a snapshot
	// field and nothing below it adjusts one -- §4.7 Stub 8 names this the only
	// projection, and T-UI-05 asserts over its output, so a correction applied
	// here would be a correction the gate cannot see.
	OutSnapshot = strat::buildUiSnapshot(World);
	return FStratResult::Ok();
}

FStratResult FStratBridge::CheckSnapshotFidelity(
	const strat::UiSnapshot& Snapshot,
	TArray<FString>&         OutFailures) const
{
	OutFailures.Reset();

	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}

	// Rebuilt here rather than taken as an argument. The check compares a snapshot
	// against the state it claims to project, so the world it is measured against
	// must be this bridge's own -- a caller-supplied one could be a second assembly
	// that agrees with the snapshot for reasons the state does not.
	const strat::UiWorld World = MakeUiWorld();

	const strat::UiFidelityResult Result =
		strat::uiCheckSnapshotFidelity(World, Snapshot);

	for (const strat::UiFidelityFailure& Failure : Result.failures)
	{
		OutFailures.Add(FString::Printf(TEXT("clause (%s) %s: %s"),
			*FromStd(Failure.clause), *FromStd(Failure.field), *FromStd(Failure.detail)));
	}

	// The module's own verdict, forwarded. See the header for why `ok` is not
	// re-derived from whether the list came back empty.
	return Result.ok
		? FStratResult::Ok()
		: FStratResult::Fail(
			FString::Printf(TEXT("snapshot fidelity failed on %d field(s)"),
				OutFailures.Num()),
			TEXT("T-UI-05"));
}

FStratResult FStratBridge::Reachable(int32 UnitId, std::vector<strat::ReachEntry>& OutReach) const
{
	// Cleared up front so a refusal cannot leave the caller holding a previous
	// call's set and read it as this call's answer.
	OutReach.clear();

	if (!bDefinitionsLoaded)
	{
		return FStratResult::Fail(TEXT("definitions are not loaded"));
	}
	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}

	// MakeUiSnapshot's refusal, made here for its reason AND one more.
	// MakeUiWorld SKIPS a unit whose defIndex is outside the loaded table, so
	// without this loop such a unit would reach `findUiUnit` below as "no unit
	// with id N" -- a refusal blaming the caller's argument for a fault in the
	// table. Validating first keeps those two answers from being spelled the
	// same way.
	for (const strat::GameUnit& U : GameState.units)
	{
		if (U.defIndex < 0 || static_cast<size_t>(U.defIndex) >= Units.size())
		{
			return FStratResult::Fail(FString::Printf(
				TEXT("unit %d carries defIndex %d, outside the loaded unit table"),
				U.id, U.defIndex));
		}
	}

	// Borrowed for the length of this call and never escaping it, exactly as in
	// MakeUiSnapshot: every pointer in the world points into `this`.
	const strat::UiWorld World = MakeUiWorld();

	// The module's own finder, over the world the query will resolve against --
	// not a scan of `GameState.units`. Asking a different list would let this
	// check pass where `uiReachable` then finds nothing, which is the one
	// disagreement the check exists to prevent.
	if (strat::findUiUnit(World, UnitId) == nullptr)
	{
		return FStratResult::Fail(FString::Printf(TEXT("no unit with id %d"), UnitId));
	}

	// The one line this method exists for. Nothing above it narrowed the set and
	// nothing below it reorders, re-costs or trims one -- §2.5's "the UI queries
	// the module and never recomputes movement" is structural here only for as
	// long as that stays true.
	OutReach = strat::uiReachable(World, UnitId);
	return FStratResult::Ok();
}

FStratResult FStratBridge::Forecast(int32 AttackerId, const strat::Hex& DefenderHex,
                                    strat::UiForecast& OutForecast) const
{
	// Reset up front, exactly as Reachable clears its vector: a refusal must not
	// leave the caller holding a previous forecast's damage number and read it as
	// this one's.
	OutForecast = strat::UiForecast();

	if (!bDefinitionsLoaded)
	{
		return FStratResult::Fail(TEXT("definitions are not loaded"));
	}
	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}

	// Reachable's reason, unchanged: MakeUiWorld SKIPS a unit whose defIndex is
	// outside the loaded table, so without this the attacker would arrive at
	// `findUiUnit` as "no unit with id N" -- blaming the caller's argument for a
	// fault in the table.
	for (const strat::GameUnit& U : GameState.units)
	{
		if (U.defIndex < 0 || static_cast<size_t>(U.defIndex) >= Units.size())
		{
			return FStratResult::Fail(FString::Printf(
				TEXT("unit %d carries defIndex %d, outside the loaded unit table"),
				U.id, U.defIndex));
		}
	}

	const strat::UiWorld World = MakeUiWorld();

	// The malformed-question case, refused here rather than left to the module.
	// See the header: `uiForecast` spells "no such unit" the same way it spells
	// "out of range", and those are not the same kind of thing.
	if (strat::findUiUnit(World, AttackerId) == nullptr)
	{
		return FStratResult::Fail(FString::Printf(TEXT("no unit with id %d"), AttackerId));
	}

	// The one line this method exists for. Nothing above it computed a distance
	// and nothing below it adjusts a damage number -- T-UI-01's "the forecast is
	// exactly what resolves" is structural only for as long as that holds. The
	// DEFENDER is deliberately not pre-checked: an empty hex, a friendly unit and
	// a target out of range are answers this returns with the module's own reason.
	OutForecast = strat::uiForecast(World, AttackerId, DefenderHex);
	return FStratResult::Ok();
}

FStratResult FStratBridge::BuildOptions(int32 Side, const strat::Hex& FactoryHex,
                                        std::vector<strat::UiBuildOption>& OutOptions) const
{
	// Cleared up front, exactly as Forecast resets its out-parameter: a refusal must
	// not leave the caller holding the previous factory's menu and pricing it as this
	// one's.
	OutOptions.clear();

	if (!bDefinitionsLoaded)
	{
		return FStratResult::Fail(TEXT("definitions are not loaded"));
	}
	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}

	// The malformed-question case, refused here rather than left to the module. See
	// the header: `uiBuildOptions` spells "invalid side" the same way it spells "not a
	// build point", and those are not the same kind of thing.
	if (Side < 0 || Side >= strat::SIDE_COUNT)
	{
		return FStratResult::Fail(FString::Printf(
			TEXT("side %d is outside the %d sides this match has"), Side, strat::SIDE_COUNT));
	}

	// The one line this method exists for. The factory hex goes through untouched:
	// whether it is an objective at all, whether this side holds it, whether it is a
	// build point and whether it has already built are the module's answers, and they
	// arrive on `available` and `reason` rather than on the refusal channel.
	OutOptions = strat::uiBuildOptions(MakeUiWorld(), Side, FactoryHex);

	// An empty vector is `uiBuildOptions`'s missing-table result and never an answer.
	// A caller handed an empty menu cannot tell it from a board state that offers
	// nothing -- and there is no such board state, which is the whole point.
	if (OutOptions.empty())
	{
		return FStratResult::Fail(TEXT("the loaded unit table has no rows to offer"));
	}

	return FStratResult::Ok();
}

// ---------------------------------------------------------------------------
// Recorded log -> §4.10 save (row 10 part (a)).
// ---------------------------------------------------------------------------

FStratResult FStratBridge::SerializeRecordedSave(const FStratSaveIdentity& Identity,
                                                 FString&                  OutText) const
{
	OutText.Reset();

	if (!bDefinitionsLoaded)
	{
		return FStratResult::Fail(TEXT("definitions are not loaded"));
	}
	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}

	// `result` is read off a projection rather than compared here. `strat::buildUiSnapshot`
	// owns the InProgress-is-null mapping and `tierName` owns the spelling; a
	// `tier != InProgress` written in this file would be a second copy of the first
	// and a second spelling of the second, and the save would then be able to
	// disagree with the scoreboard about whether the match is over.
	strat::UiSnapshot Snapshot;
	const FStratResult Projected = MakeUiSnapshot(Snapshot);
	if (!Projected.bOk)
	{
		return Projected;
	}

	strat::Save S;
	S.formatVersion = strat::kFormatVersion;
	S.rulesCommit   = ToStd(Identity.RulesCommit);
	S.dataHash      = ToStd(Identity.DataHash);
	// The loaded scenario's own id and the module's digest over it -- not a
	// caller's claim about which scenario this is.
	S.scenarioId    = LoadedScenario.scenarioId;
	S.scenarioHash  = strat::scenarioHash(LoadedScenario);
	// `Save::seed`: reserved, MUST be 0. No RNG ships, so there is nothing for a
	// non-zero value to mean.
	S.seed          = 0;
	S.commandLog    = Recorded;
	S.stateHash     = strat::canonicalStateHash(GameState);
	S.hasResult     = Snapshot.match.hasResult;
	S.result        = S.hasResult ? std::string(strat::tierName(Snapshot.match.resultTier))
	                              : std::string();

	OutText = FromStd(strat::serializeSave(S));
	return FStratResult::Ok();
}

FStratResult FStratBridge::GuidedOpeningHexes(int32      Side,
                                              FIntPoint& OutInfantryDeployHex,
                                              FIntPoint& OutObjectiveHex) const
{
	OutInfantryDeployHex = FIntPoint::ZeroValue;
	OutObjectiveHex      = FIntPoint::ZeroValue;

	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}

	for (const strat::ScenarioGuided& G : LoadedScenario.guided)
	{
		if (G.side == Side)
		{
			// X = q, Y = r, the same axial convention every engine-typed hex in this
			// façade uses. No conversion and no odd-r round trip: the scenario parser
			// already converted at parse time (Scenario.h's T-SCN-05 posture).
			OutInfantryDeployHex = FIntPoint(G.infantry.q, G.infantry.r);
			OutObjectiveHex      = FIntPoint(G.objective.q, G.objective.r);
			return FStratResult::Ok();
		}
	}

	return FStratResult::Fail(FString::Printf(
		TEXT("scenario '%s' names no guidedOpening for side %d"),
		*FromStd(LoadedScenario.scenarioId), Side));
}

FStratResult FStratBridge::RestoreFromSaveText(const FString&            SaveText,
                                               const FStratSaveIdentity& Identity,
                                               int32&                    OutCommandCount)
{
	OutCommandCount = 0;

	if (!bDefinitionsLoaded)
	{
		return FStratResult::Fail(TEXT("definitions are not loaded"));
	}
	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}
	if (!Recorded.empty())
	{
		// `ReplayRecordedLogOnto`'s arm 4, on this object rather than on a target.
		return FStratResult::Fail(FString::Printf(
			TEXT("this bridge is not fresh: it has already applied %d command(s)"),
			static_cast<int32>(Recorded.size())));
	}

	// Every field of the expectation has exactly one source, and the two this
	// object owns are read off this object -- see the declaration.
	strat::SaveHeaderExpectation Expect;
	Expect.expectedVersion = strat::kFormatVersion;
	Expect.rulesCommit     = ToStd(Identity.RulesCommit);
	Expect.dataHash        = ToStd(Identity.DataHash);
	Expect.scenarioHash    = strat::scenarioHash(LoadedScenario);

	// `loadSave` keeps GATE-SAVE-PARSE and T-SAVE-04 distinct and leaves `Parsed`
	// untouched on either, so the verdict is forwarded with the module's own id
	// rather than collapsed into one "bad save".
	strat::Save Parsed;
	const strat::SaveLoadResult Load =
		strat::loadSave(ToStd(SaveText), std::string("save slot"), Expect, Parsed);
	if (!Load.ok)
	{
		return FStratResult::Fail(FromStd(Load.reason), FromStd(Load.failedId));
	}

	// ONTO A COPY. `replayLog` is all-or-nothing within the log, but the hash check
	// below is downstream of it and `replayLog` cannot roll back for a reason it
	// never sees. Assigning only after both guards pass is what makes this method
	// all-or-nothing on THIS object.
	strat::GameState Candidate = GameState;
	const strat::ReplayResult R = strat::replayLog(Candidate, Parsed.commandLog, Tables());
	if (!R.ok)
	{
		return FStratResult::Fail(
			FString::Printf(TEXT("%s (at index %d)"), *FromStd(R.reason), R.failedIndex),
			FromStd(R.failedId));
	}

	// §4.10's fixed point, verified rather than assumed. The reason names both
	// hashes because the failure a caller will actually hit -- re-seeding with the
	// wrong `firstSide`, which the format cannot carry -- is indistinguishable from
	// a corrupted log here, and saying so is cheaper than a caller guessing.
	const std::string Reached = strat::canonicalStateHash(Candidate);
	if (Reached != Parsed.stateHash)
	{
		return FStratResult::Fail(
			FString::Printf(
				TEXT("replayed state hash disagrees with the save's (file %s, replayed %s); ")
				TEXT("the log, the definitions or the seeding side is not the one this save was written from"),
				*FromStd(Parsed.stateHash), *FromStd(Reached)),
			TEXT("T-SAVE-06"));
	}

	// One assignment step. "Applied" and "recorded" become the same set here for
	// `ReplayLog`'s reason: a bridge with state but no log serializes a save whose
	// `commandLog` cannot reproduce its own `stateHash`.
	GameState       = MoveTemp(Candidate);
	Recorded        = Parsed.commandLog;
	OutCommandCount = static_cast<int32>(Recorded.size());

	return FStratResult::Ok();
}

// ---------------------------------------------------------------------------
// The engine-typed façade. See the header block on these: they exist so that
// `StratPlay` can ask a rules question without naming a `strat::` type, and they
// add no policy of their own. Every refusal below is forwarded from the typed
// method beside it, unaltered, so a gameplay-side failure reads in the words of
// the layer that actually refused.
// ---------------------------------------------------------------------------

int32 FStratBridge::Turn() const
{
	// The live turn number, not a copy kept anywhere. See the header on why a
	// caller must read this BEFORE submitting.
	return GameState.turn.turnNumber;
}

int32 FStratBridge::SideToMove() const
{
	return GameState.turn.activeSide;
}

FStratResult FStratBridge::ReachableHexes(int32           UnitId,
                                          TArray<FIntPoint>& OutHexes,
                                          TArray<int32>&     OutCosts) const
{
	// Cleared up front for `Reachable`'s own reason: a refusal must not leave the
	// caller holding a previous call's set and read it as this call's answer.
	OutHexes.Reset();
	OutCosts.Reset();

	std::vector<strat::ReachEntry> Reach;
	const FStratResult Asked = Reachable(UnitId, Reach);
	if (!Asked.bOk)
	{
		return Asked;
	}

	// Copied in the order the module emitted, with nothing sorted, filtered or
	// re-costed. `Reachable`'s header states that property of itself and this
	// method would silently take it away.
	OutHexes.Reserve(static_cast<int32>(Reach.size()));
	OutCosts.Reserve(static_cast<int32>(Reach.size()));
	for (const strat::ReachEntry& E : Reach)
	{
		OutHexes.Add(FIntPoint(E.hex.q, E.hex.r));
		OutCosts.Add(E.cost);
	}

	return FStratResult::Ok();
}

FStratResult FStratBridge::AttackTargetHexes(int32 AttackerId, TArray<FIntPoint>& OutHexes) const
{
	OutHexes.Reset();

	if (!bDefinitionsLoaded)
	{
		return FStratResult::Fail(TEXT("definitions are not loaded"));
	}
	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}

	// The attacker's side, read off the authoritative unit list. A malformed
	// question -- an id that is not on the board -- is refused here in the same
	// words `Forecast` and `Reachable` use, rather than coming back as an empty
	// target list that a caller would draw as "nothing to attack".
	const strat::GameUnit* Attacker = nullptr;
	for (const strat::GameUnit& U : GameState.units)
	{
		if (U.id == AttackerId)
		{
			Attacker = &U;
			break;
		}
	}
	if (Attacker == nullptr)
	{
		return FStratResult::Fail(FString::Printf(TEXT("no unit with id %d"), AttackerId));
	}

	const int32 AttackerSide = Attacker->side;

	// ONE `Forecast` CALL PER CANDIDATE, and the module's `legal` is the whole of
	// the decision. Nothing here compares a distance or reads a range: the
	// candidates are "every unit not on the attacker's side" and the filter is the
	// rules module's answer about each one.
	//
	// The order is `GameState.units`' -- ascending unit id, the same order
	// `FStratViewModel::Units` carries -- so two calls on the same state produce
	// the same array, without this method sorting anything.
	for (const strat::GameUnit& U : GameState.units)
	{
		if (U.side == AttackerSide)
		{
			continue;
		}

		strat::UiForecast Forecasted;
		const FStratResult Asked = Forecast(AttackerId, U.hex, Forecasted);
		if (!Asked.bOk)
		{
			// A refusal here is about the bridge or the tables and not about this
			// candidate -- `Forecast` answers "out of range" and "same side" with
			// Ok() and `legal` false. Forwarded rather than skipped, because a
			// partial target list drawn as a complete one is how a player learns
			// the wrong thing about a board.
			OutHexes.Reset();
			return Asked;
		}

		if (Forecasted.legal)
		{
			OutHexes.Add(FIntPoint(U.hex.q, U.hex.r));
		}
	}

	return FStratResult::Ok();
}

FStratResult FStratBridge::SubmitMoveToHex(int32 UnitId, FIntPoint DestHex)
{
	return SubmitMove(UnitId, strat::Hex{DestHex.X, DestHex.Y});
}

FStratResult FStratBridge::SubmitAttackAtHex(int32 UnitId, FIntPoint TargetHex)
{
	return SubmitAttack(UnitId, strat::Hex{TargetHex.X, TargetHex.Y});
}

FStratResult FStratBridge::SubmitBuildAtHex(FIntPoint FactoryHex, int32 DefIndex)
{
	// The same one conversion in front of the same typed method the other two do.
	// Nothing is validated here that `SubmitBuild` -> `SubmitStamped` -> `Submit`
	// does not validate: the defIndex bound is `applyCommand`'s, the factory
	// ownership is Economy.h's, the once-per-turn allowance is `Turn.h::canBuildAt`'s,
	// and every refusal comes back in the words of whichever of them said no.
	return SubmitBuild(strat::Hex{FactoryHex.X, FactoryHex.Y}, DefIndex);
}

// ---------------------------------------------------------------------------
// §2.9's opponent AI. See the header for why the call can only be made here.
// ---------------------------------------------------------------------------

strat::AiState FStratBridge::MakeAiState() const
{
	strat::AiState A;

	// Member for member with the headless driver's `strat::aiStateOf`. Nine of them,
	// against `strat::AiState`; the order below is that declaration's order so a reader
	// can check completeness by walking the two side by side.
	A.bounds      = GameState.bounds;
	A.terrain     = GameState.terrain;
	// `A.units` is filled below.
	A.unitDefs    = Units;
	A.terrainDefs = Terrain;
	A.economy     = GameState.economy;
	A.turn        = GameState.turn;
	A.buildlist   = Buildlist;
	// SET EXPLICITLY EVEN THOUGH `A.turn` ALREADY CARRIES IT. `TurnState` owns the
	// per-factory record (`TurnState::builtThisTurn`, "row 5 owns it now, not the driver") and
	// `AiState` restates it as its own member, so `aiStateOf` assigns both -- and so
	// does this. Assigning only `turn` and trusting the AI to reach through it would
	// leave `builtThisTurn` empty for whichever of the two `Ai.good.cpp` actually
	// reads, and an empty one re-permits a second build per factory per turn: the AI
	// queues it, `markBuilt` refuses it, and the opponent stalls on its own command
	// with nothing on fire.
	A.builtThisTurn = GameState.turn.builtThisTurn;

	for (const strat::GameUnit& U : GameState.units)
	{
		strat::AiUnit AU;
		AU.id       = U.id;
		AU.side     = U.side;
		AU.defIndex = U.defIndex;
		AU.hex      = U.hex;
		AU.hp       = U.hp;
		// The module's own derivation from the per-side designation, read through
		// `isFlagUnit` rather than by comparing `flagUnit[side]` in-line, for
		// `MakeUiWorld`'s reason: a dead flag is absent from `units` and what that
		// means belongs to the module that owns the designation. `isFlag` is NOT a
		// field on `GameUnit` and there is nothing here to copy.
		AU.isFlag   = strat::isFlagUnit(GameState, U);
		A.units.push_back(AU);
	}

	// `placement` is deliberately not carried: `AiUnit` has no such field. §4.7
	// Stub 6 says the AI sees only what a player could read off the board, and where
	// a unit was deployed is scenario data the UI projection needs and §2.9 does not.
	return A;
}

FStratResult FStratBridge::NextAiCommand(int32 Side, FStratAiCommand& Out) const
{
	// Reset up front so a refusal cannot leave the caller holding a previous call's
	// command and read it as this call's answer -- `Reachable`'s discipline. Note
	// that the reset value READS AS EndTurn, which is why every path below refuses
	// rather than falling through to it.
	Out = FStratAiCommand();

	if (!bDefinitionsLoaded)
	{
		return FStratResult::Fail(TEXT("definitions are not loaded"));
	}
	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}
	if (Side < 0 || Side >= strat::SIDE_COUNT)
	{
		return FStratResult::Fail(FString::Printf(
			TEXT("side %d is outside the %d sides this match has"), Side, strat::SIDE_COUNT));
	}

	// `Reachable`'s roster check, for its reason and one specific to this method:
	// `MakeAiState` copies every unit faithfully rather than skipping a broken one,
	// so a defIndex outside the loaded table would reach `Ai.good.cpp` and be used
	// to look up a stat block. Refusing here is the only place that can name the
	// unit and the index.
	for (const strat::GameUnit& U : GameState.units)
	{
		if (U.defIndex < 0 || static_cast<size_t>(U.defIndex) >= Units.size())
		{
			return FStratResult::Fail(FString::Printf(
				TEXT("unit %d carries defIndex %d, outside the loaded unit table"),
				U.id, U.defIndex));
		}
	}

	// The one line this method exists for. Nothing above it decided a command and
	// nothing below it adjusts one -- the hex resolution further down is a lookup on
	// the authoritative board, not an amendment to what §2.9 chose.
	const strat::AiCommand C = strat::nextCommand(MakeAiState(), Side);

	// A SWITCH AND NOT A CAST, unlike `ToStratUnitType` above. That cast rides on a
	// static_assert and a runtime parity test pinning `EUnitType` to
	// `strat::UnitType`; no such gate exists for `AiCommandKind`, and the two enums
	// live in different repositories. A re-vendored `Ai.h` that added a fifth
	// enumerator would make a cast produce a value `EStratAiCommandKind` does not
	// have, silently; this arm makes it a refusal that names the number.
	switch (C.kind)
	{
	case strat::AiCommandKind::Build:   Out.Kind = EStratAiCommandKind::Build;   break;
	case strat::AiCommandKind::Move:    Out.Kind = EStratAiCommandKind::Move;    break;
	case strat::AiCommandKind::Attack:  Out.Kind = EStratAiCommandKind::Attack;  break;
	case strat::AiCommandKind::EndTurn: Out.Kind = EStratAiCommandKind::EndTurn; break;
	default:
		Out = FStratAiCommand();
		return FStratResult::Fail(FString::Printf(
			TEXT("the rules module returned AiCommandKind %d, which this bridge has no name for"),
			static_cast<int32>(C.kind)));
	}

	Out.UnitId   = C.unitId;
	Out.TargetId = C.targetId;
	Out.DefIndex = C.defIndex;
	// X = q, Y = r, the encoding the rest of the façade already carries. For Move
	// this is the destination and for Build the factory; for Attack it is
	// default-constructed and is REPLACED below.
	Out.Hex      = FIntPoint(C.hex.q, C.hex.r);

	if (Out.Kind == EStratAiCommandKind::Attack)
	{
		// `AiCommand` names an Attack's victim by UNIT ID and leaves `hex` at its
		// default, while `SubmitAttackAtHex` takes a hex. Resolved HERE, against the
		// authoritative unit list, at the instant the command was decided -- see the
		// header for why the caller must not do this off a view model.
		const strat::GameUnit* Target = strat::findGameUnit(GameState, C.targetId);
		if (Target == nullptr)
		{
			// Refused rather than left at (0,0). (0,0) is a real hex on this board,
			// so an unresolved target would submit an attack somewhere plausible
			// instead of failing.
			Out = FStratAiCommand();
			return FStratResult::Fail(FString::Printf(
				TEXT("the AI named target unit %d, which is not on the board"), C.targetId));
		}
		Out.Hex = FIntPoint(Target->hex.q, Target->hex.r);
	}

	return FStratResult::Ok();
}

FStratResult FStratBridge::SetBuildlistByIds(const TArray<FName>& UnitIds)
{
	if (!bDefinitionsLoaded)
	{
		return FStratResult::Fail(TEXT("definitions are not loaded"));
	}

	// Built aside and assigned only after every entry resolved, so a refusal leaves
	// the previous buildlist byte-identical -- `loadUnits`' own "out is left
	// untouched on failure" discipline, and the reason a half-resolved §2.9 build
	// mix cannot exist.
	std::vector<int> Next;
	Next.reserve(static_cast<size_t>(UnitIds.Num()));

	for (int32 Index = 0; Index < UnitIds.Num(); ++Index)
	{
		// Exact UTF-8 bytes, NOT FName::operator==, which compares case-
		// insensitively and would widen the §4.8 id space by a decision taken here
		// rather than in the data. See the header.
		const std::string Wanted = ToStd(UnitIds[Index].ToString());

		int32 Resolved = INDEX_NONE;
		for (size_t I = 0; I < Units.size(); ++I)
		{
			if (Units[I].id == Wanted)
			{
				Resolved = static_cast<int32>(I);
				break;
			}
		}

		if (Resolved == INDEX_NONE)
		{
			// NAMED, AND NOT SUBSTITUTED. §4.8's posture: a caller that misspelled a
			// unit id must find out here, not by watching an opponent build the
			// wrong thing for twenty turns. The position is reported because
			// duplicates are legal, so the id alone does not say which entry failed.
			return FStratResult::Fail(FString::Printf(
				TEXT("buildlist entry %d names unit id '%s', which is not in the loaded unit table"),
				Index, *UnitIds[Index].ToString()));
		}

		// Appended, never deduplicated: repetition is how §2.9's build MIX is
		// expressed, and this list is data (`AiState::buildlist`'s own comment).
		Next.push_back(Resolved);
	}

	Buildlist = std::move(Next);
	return FStratResult::Ok();
}

TArray<int32> FStratBridge::BuildlistDefIndexes() const
{
	TArray<int32> Out;
	Out.Reserve(static_cast<int32>(Buildlist.size()));
	for (const int Def : Buildlist)
	{
		Out.Add(Def);
	}
	return Out;
}

// ---------------------------------------------------------------------------
// The recording joint, in engine types. See the header for why these exist at
// all -- they are the two values `StratPlay` needs and cannot spell.
// ---------------------------------------------------------------------------

int32 FStratBridge::RecordedCommandCount() const
{
	// The container's own count, not a parallel counter. A counter maintained
	// beside `Recorded` is a second thing that can disagree with it, and the
	// disagreement would show up as a test that trusts the counter while the log
	// it stands for is empty -- which is the exact defect this method exists to
	// expose.
	return static_cast<int32>(Recorded.size());
}

FStratResult FStratBridge::ReplayRecordedLogOnto(FStratBridge& Fresh) const
{
	if (&Fresh == this)
	{
		return FStratResult::Fail(TEXT("target bridge is this bridge"));
	}
	if (!bSeeded)
	{
		return FStratResult::Fail(TEXT("no scenario is loaded"));
	}
	if (!Fresh.IsSeeded())
	{
		return FStratResult::Fail(TEXT("target bridge has no scenario loaded"));
	}
	if (Fresh.RecordedCommandCount() != 0)
	{
		return FStratResult::Fail(FString::Printf(
			TEXT("target bridge is not fresh: it has already applied %d command(s)"),
			Fresh.RecordedCommandCount()));
	}
	if (Recorded.empty())
	{
		// Refused rather than replayed. An empty log replays to Ok() and to an
		// equal hash on any target seeded from the same scenario, so a silent
		// success here is indistinguishable from a full session's worth of
		// commands -- and telling those two apart is the only job this method has.
		return FStratResult::Fail(TEXT("no commands have been recorded"));
	}

	// The scenario's bytes, not its label. `scenarioId` is a name a file chose;
	// `scenarioHash` is the module's digest over the thing that was actually
	// seeded, and it is the same function `SerializeRecordedSave` writes into a
	// §4.10 header for the same reason.
	const std::string MineHash  = strat::scenarioHash(LoadedScenario);
	const std::string TheirHash = strat::scenarioHash(Fresh.ScenarioData());
	if (MineHash != TheirHash)
	{
		return FStratResult::Fail(
			FString::Printf(TEXT("target bridge seeded from a different scenario (%s vs %s)"),
				*FromStd(LoadedScenario.scenarioId), *FromStd(Fresh.ScenarioData().scenarioId)),
			FromStd(TheirHash));
	}

	// `ReplayLog` owns all-or-nothing, the refusal wording and the index, and the
	// append that leaves `Fresh` recording what it replayed. Nothing is re-decided
	// here; this method is the guard band in front of it and the type conversion.
	TArray<strat::SaveCommand> Log;
	Log.Reserve(static_cast<int32>(Recorded.size()));
	for (const strat::SaveCommand& C : Recorded)
	{
		Log.Add(C);
	}

	return Fresh.ReplayLog(Log);
}
