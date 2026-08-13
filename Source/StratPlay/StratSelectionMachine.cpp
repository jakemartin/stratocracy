// GDD §2.11.1 / §4.9 / T-INT-05 / T-UI-02 -- the selection state machine, implementation.
//
// THIS FILE INCLUDES `StratBridge.h`, and that is legal here for the reason the whole
// module's discipline turns on: this is a .cpp, and the header beside it declares no
// reflected type, so UHT parses neither. `StratSelectionMachine.h` forward declares
// `FStratBridge` and that is all any consumer needs.
//
// IT NAMES NO `strat::` TYPE EVEN SO. Every bridge call below is one of the engine-typed
// façade methods -- `ReachableHexes`, `AttackTargetHexes`, `SubmitMoveToHex`,
// `SubmitAttackAtHex`, `SubmitEndTurn`, `Turn`, `SideToMove`, `StateHash` -- which exist
// precisely so this file can ask a rules question in `int32` and `FIntPoint`.
//
// THERE IS NO HEX ARITHMETIC IN THIS FILE. Search it: no distance, no neighbour, no
// addition of two coordinates. Membership in a set the rules module produced is the only
// spatial predicate, and `FIntPoint::operator==` is the only spatial operator.

#include "StratSelectionMachine.h"

#include "StratBridge.h"
#include "StratPlay.h"

// ---------------------------------------------------------------------------
// The production query adapter.
// ---------------------------------------------------------------------------

bool FStratBridgeRulesQuery::ReachableHexes(int32              UnitId,
                                            TArray<FIntPoint>& OutHexes,
                                            FString&           OutFailureReason) const
{
	OutHexes.Reset();
	OutFailureReason.Reset();

	if (Bridge == nullptr)
	{
		OutFailureReason = TEXT("no bridge");
		return false;
	}

	// The costs are asked for and dropped. `ReachableHexes` hands back both because a
	// §2.11 cost readout will want them; this interface is about the overlay, which does
	// not. Dropping them here rather than adding a second bridge method keeps one query.
	TArray<int32> Costs;
	const FStratResult Asked = Bridge->ReachableHexes(UnitId, OutHexes, Costs);
	if (!Asked.bOk)
	{
		OutHexes.Reset();
		OutFailureReason = Asked.Reason;
		return false;
	}

	return true;
}

bool FStratBridgeRulesQuery::AttackTargetHexes(int32              UnitId,
                                                TArray<FIntPoint>& OutHexes,
                                                FString&           OutFailureReason) const
{
	OutHexes.Reset();
	OutFailureReason.Reset();

	if (Bridge == nullptr)
	{
		OutFailureReason = TEXT("no bridge");
		return false;
	}

	const FStratResult Asked = Bridge->AttackTargetHexes(UnitId, OutHexes);
	if (!Asked.bOk)
	{
		OutHexes.Reset();
		OutFailureReason = Asked.Reason;
		return false;
	}

	return true;
}

// ---------------------------------------------------------------------------
// Lookups. Both static, both over the model, neither cached.
// ---------------------------------------------------------------------------

const FStratUnitView* FStratSelectionMachine::FindUnit(const FStratViewModel& Model, int32 UnitId)
{
	if (UnitId == INDEX_NONE)
	{
		return nullptr;
	}

	for (const FStratUnitView& U : Model.Units)
	{
		if (U.UnitId == UnitId)
		{
			return &U;
		}
	}
	return nullptr;
}

const FStratUnitView* FStratSelectionMachine::FindUnitAtHex(const FStratViewModel& Model,
                                                            FIntPoint              Hex)
{
	// A LINEAR SCAN OVER TEN UNITS, and deliberately not a hex -> unit map. A map would be
	// a second description of where the units are, kept beside the model's own, and the
	// only way it can be wrong is the way that matters: silently, one refresh behind.
	for (const FStratUnitView& U : Model.Units)
	{
		if (U.Hex == Hex)
		{
			return &U;
		}
	}
	return nullptr;
}

// ---------------------------------------------------------------------------
// The decision point.
// ---------------------------------------------------------------------------

FStratSelectionOutcome FStratSelectionMachine::HandleEvent(EStratSelectionEvent    Event,
                                                           FIntPoint               Hex,
                                                           const FStratViewModel&  Model,
                                                           const IStratRulesQuery& Query)
{
	FStratSelectionOutcome Outcome;

	// The active side, off the model's match view. NOT `Model.ViewingSide` -- see the
	// header block: letting the viewing side authorise a command lets a player move on
	// their opponent's turn by pressing the hand-over key.
	const int32 ActiveSide = Model.Match.SideToMove;

	switch (Event)
	{
	case EStratSelectionEvent::Cancel:
	{
		// Complete the moment it happens. No rules module is consulted and nothing can
		// refuse it, so it takes effect here rather than in `NotifyCommandApplied`.
		SelectedUnitId = INDEX_NONE;
		break;
	}

	case EStratSelectionEvent::EndTurn:
	{
		// THE ONE EVENT VALID WITH NOTHING SELECTED. It is a statement about the side and
		// not about a unit, which is why `UnitId` stays `INDEX_NONE` and the log line
		// prints -1 for it.
		//
		// NOT GUARDED ON "every unit is done". §2.1 does not require a side to spend every
		// unit, and a machine that refused an early EndTurn would be inventing a rule the
		// rules module does not have.
		Outcome.Command = EStratSelectionCommand::EndTurn;
		break;
	}

	case EStratSelectionEvent::Wait:
	{
		if (SelectedUnitId == INDEX_NONE)
		{
			Outcome.FailureReason = TEXT("nothing is selected");
			break;
		}

		// THE PUREST DEMONSTRATION OF WHY `bDone` IS THIS MACHINE'S. A wait spends the
		// unit's turn and reaches no rules module at all -- there is no `Wait` command in
		// `strat::SaveCommandKind` and phase 1 recorded that the orchestration brief was
		// wrong to say there was. So this is a presentation-block write with no command
		// beside it, and `bDone` is unreachable from `bHasMoved`/`bHasActed` in exactly
		// this case.
		DoneUnits.Add(SelectedUnitId);

		// THE WAIT'S ONLY WITNESS, AND IT IS DELIBERATELY NOT A `STRAT-CMD` LINE.
		//
		// Phase 6's playtest saw Move, Attack and EndTurn in the log and could not see a
		// Wait at all, because a wait submits nothing: it reaches no rules module, so
		// `StratSubmitSelectionCommand` returns early on `Command == None` and no
		// `STRAT-CMD` line is ever printed. The only trace left was that clicking the unit
		// afterwards said "unit N has finished this turn" -- and an accepted Attack adds to
		// `DoneUnits` too (`NotifyCommandApplied`, below), so that sentence cannot tell a
		// wait from an attack. In the captured session it did not.
		//
		// `STRAT-WAIT spent` IS A DIFFERENT PHRASE FROM `STRAT-CMD accepted` FOR THE SAME
		// REASON `STRAT-CMD refused` IS (phase 4): `grep "STRAT-CMD accepted"` is a reliable
		// count of commands that actually applied to `strat::GameState`, and a wait applied
		// none. It does not even share the `STRAT-CMD` prefix, because unlike a refusal there
		// was no command here to refuse -- `strat::SaveCommandKind` has no `Wait` (phase 1
		// recorded the orchestration brief as wrong about that), so a `kind=Wait` line would
		// be a claim about a save format that cannot carry it.
		//
		// `unit=`/`turn=`/`side=` ARE THE ACCEPTED LINE'S OWN SPELLING AND ORDER, so a gate
		// can read this beside those lines with no translation table. There is deliberately
		// no `hash=`: the machine holds no bridge here, and more to the point a wait moves no
		// rules state, so printing an unchanged hash would dress a presentation-only event as
		// a rules event. Its absence is the structural tell.
		//
		// IT LOGS WHERE THE WAIT IS APPLIED, NOT WHERE IT IS REQUESTED. The
		// nothing-is-selected arm above has already broken out, so a no-op wait cannot reach
		// this line and report a success.
		//
		// `side` is `Model.Match.SideToMove` -- the same quantity `ActiveSide` authorised the
		// wait with, and never `Model.ViewingSide`.
		UE_LOG(LogStratPlay, Log,
			TEXT("STRAT-WAIT spent unit=%d turn=%d side=%d (no rules command submitted; ")
			TEXT("the save format has no Wait kind)"),
			SelectedUnitId, Model.Match.Turn, ActiveSide);

		SelectedUnitId = INDEX_NONE;
		break;
	}

	case EStratSelectionEvent::HexPrimary:
	{
		const FStratUnitView* const Selected = FindUnit(Model, SelectedUnitId);
		const FStratUnitView* const Clicked  = FindUnitAtHex(Model, Hex);

		// --- Nothing selected: this is a selection attempt. --------------------
		if (Selected == nullptr)
		{
			// A stale selection -- the unit died, or the model was rebuilt from a
			// different match -- is dropped here rather than carried. The machine keeps an
			// id and the model is the authority on whether that id is still a unit.
			SelectedUnitId = INDEX_NONE;

			if (Clicked == nullptr)
			{
				// Empty ground with nothing selected. An ordinary click and not a failure;
				// no reason is set, because there is nothing to tell the player.
				break;
			}

			if (Clicked->Side != ActiveSide)
			{
				Outcome.FailureReason = FString::Printf(
					TEXT("unit %d belongs to side %d and side %d is to move"),
					Clicked->UnitId, Clicked->Side, ActiveSide);
				break;
			}

			if (LockedUnits.Contains(Clicked->UnitId))
			{
				// §2.11.6's lock. Unobservable today -- the set is empty in every shipping
				// path -- and written now because a guidance layer would otherwise have to
				// add a rule to a state machine it does not own.
				Outcome.FailureReason = FString::Printf(
					TEXT("unit %d is locked this turn"), Clicked->UnitId);
				break;
			}

			if (DoneUnits.Contains(Clicked->UnitId))
			{
				Outcome.FailureReason = FString::Printf(
					TEXT("unit %d has finished this turn"), Clicked->UnitId);
				break;
			}

			SelectedUnitId = Clicked->UnitId;
			break;
		}

		// --- A unit is selected. ---------------------------------------------

		// Clicking the selected unit again is a deselect, and is the mouse-only way to
		// reach the same place `Cancel` reaches.
		if (Clicked != nullptr && Clicked->UnitId == Selected->UnitId)
		{
			SelectedUnitId = INDEX_NONE;
			break;
		}

		// Clicking a DIFFERENT friendly unit re-selects rather than doing nothing, which
		// is what a player means by it. It goes through the same gates a fresh selection
		// does, so the lock and the done set cannot be bypassed by having something
		// selected first.
		if (Clicked != nullptr && Clicked->Side == ActiveSide)
		{
			if (LockedUnits.Contains(Clicked->UnitId))
			{
				Outcome.FailureReason = FString::Printf(
					TEXT("unit %d is locked this turn"), Clicked->UnitId);
				break;
			}
			if (DoneUnits.Contains(Clicked->UnitId))
			{
				Outcome.FailureReason = FString::Printf(
					TEXT("unit %d has finished this turn"), Clicked->UnitId);
				break;
			}

			SelectedUnitId = Clicked->UnitId;
			break;
		}

		// Clicking an ENEMY: an attack, if the rules module lists that hex as a target.
		if (Clicked != nullptr)
		{
			if (Selected->bHasActed)
			{
				Outcome.FailureReason = FString::Printf(
					TEXT("unit %d has already acted this turn"), Selected->UnitId);
				break;
			}

			TArray<FIntPoint> Targets;
			FString           QueryReason;
			if (!Query.AttackTargetHexes(Selected->UnitId, Targets, QueryReason))
			{
				Outcome.FailureReason = QueryReason;
				break;
			}

			// MEMBERSHIP IN THE MODULE'S SET IS THE WHOLE TEST. No range is compared here
			// and none may be: `Targets` is `strat::uiForecast`'s answer for every enemy
			// unit, and §2.6's range, terrain and side rules are all inside it.
			if (!Targets.Contains(Hex))
			{
				Outcome.FailureReason = FString::Printf(
					TEXT("unit %d cannot attack (%d,%d)"), Selected->UnitId, Hex.X, Hex.Y);
				break;
			}

			Outcome.Command = EStratSelectionCommand::Attack;
			Outcome.UnitId  = Selected->UnitId;
			Outcome.Hex     = Hex;
			break;
		}

		// Clicking EMPTY GROUND with a unit selected: a move, if it is in reach.
		if (Selected->bHasMoved)
		{
			Outcome.FailureReason = FString::Printf(
				TEXT("unit %d has already moved this turn"), Selected->UnitId);
			break;
		}

		{
			TArray<FIntPoint> Reach;
			FString           QueryReason;
			if (!Query.ReachableHexes(Selected->UnitId, Reach, QueryReason))
			{
				Outcome.FailureReason = QueryReason;
				break;
			}

			// The same membership test, over the movement set this time. T-UI-02 measured
			// 122 hexes across 10 of 10 units where `distance <= move` disagrees with this
			// set; a filter here would be wrong on most of Ferrum Crossing.
			if (!Reach.Contains(Hex))
			{
				Outcome.FailureReason = FString::Printf(
					TEXT("(%d,%d) is not in unit %d's reachable set"),
					Hex.X, Hex.Y, Selected->UnitId);
				break;
			}

			// The unit's own hex is IN the reachable set at cost 0 -- the null move, which
			// `Move.h` includes on purpose because "a unit is not blocked by itself". It is
			// refused here rather than submitted, because a null move spends the move flag
			// for nothing and a player who clicked where the unit already stands did not
			// ask for that.
			if (Hex == Selected->Hex)
			{
				Outcome.FailureReason = FString::Printf(
					TEXT("unit %d is already on (%d,%d)"), Selected->UnitId, Hex.X, Hex.Y);
				break;
			}

			Outcome.Command = EStratSelectionCommand::Move;
			Outcome.UnitId  = Selected->UnitId;
			Outcome.Hex     = Hex;
		}
		break;
	}
	}

	Outcome.SelectedUnitId = SelectedUnitId;
	return Outcome;
}

void FStratSelectionMachine::NotifyCommandApplied(const FStratSelectionOutcome& Outcome)
{
	switch (Outcome.Command)
	{
	case EStratSelectionCommand::Move:
		// The unit STAYS SELECTED. §2.11.1's move-then-attack needs it, and the overlays
		// rebuild around the model's now-true `bHasMoved` with no state change here.
		break;

	case EStratSelectionCommand::Attack:
		// §2.11.1: an attack ends the unit's turn. Set explicitly and never derived from
		// `bHasActed` -- `Ui.h` is explicit that `bDone` is not a function of the turn
		// flags, and the Wait path above is the case that proves it.
		DoneUnits.Add(Outcome.UnitId);
		SelectedUnitId = INDEX_NONE;
		break;

	case EStratSelectionCommand::EndTurn:
		// A PER-TURN FACT, CLEARED AT THE BOUNDARY. Carrying it would grey out a unit that
		// has a fresh turn. `LockedUnits` is deliberately NOT cleared here: a §2.11.6 lock
		// clears when beat 1a retires, INSIDE turn 1, and the two lifecycles are different
		// (`StratViewModel.h` on `bLockedThisTurn`).
		DoneUnits.Reset();
		SelectedUnitId = INDEX_NONE;
		break;

	case EStratSelectionCommand::None:
		break;
	}
}

// ---------------------------------------------------------------------------
// Output.
// ---------------------------------------------------------------------------

void FStratSelectionMachine::DecorateViewModel(FStratViewModel& Model) const
{
	// TWO FIELDS, AND NOTHING ELSE IS TOUCHED. Every other field on every other unit is
	// `StratBuildViewModel`'s, copied from a `strat::UiSnapshot` field with no
	// transformation; adjusting one here would be widget-side arithmetic in a new place.
	for (FStratUnitView& U : Model.Units)
	{
		U.bDone           = DoneUnits.Contains(U.UnitId);
		U.bLockedThisTurn = LockedUnits.Contains(U.UnitId);
	}
}

void FStratSelectionMachine::BuildOverlays(const FStratViewModel&  Model,
                                           const IStratRulesQuery& Query,
                                           TArray<FIntPoint>&      OutReachHexes,
                                           TArray<FIntPoint>&      OutTargetHexes,
                                           FString&                OutFailureReason) const
{
	OutReachHexes.Reset();
	OutTargetHexes.Reset();
	OutFailureReason.Reset();

	const FStratUnitView* const Selected = FindUnit(Model, SelectedUnitId);
	if (Selected == nullptr)
	{
		// Nothing selected -- or a selection the model no longer has. Both overlays clear,
		// which is the correct screen for both.
		return;
	}

	// A unit that has already moved gets NO movement overlay, and that decision is made
	// here rather than by the query returning less: `ReachableHexes` always answers with at
	// least the unit's own hex, because a null move is a move. Drawing that one hex after
	// the unit has moved would be an overlay saying something §2.1 does not.
	if (!Selected->bHasMoved)
	{
		FString ReachReason;
		if (!Query.ReachableHexes(Selected->UnitId, OutReachHexes, ReachReason))
		{
			OutReachHexes.Reset();
			OutFailureReason = ReachReason;
		}
	}

	if (!Selected->bHasActed)
	{
		FString TargetReason;
		if (!Query.AttackTargetHexes(Selected->UnitId, OutTargetHexes, TargetReason))
		{
			OutTargetHexes.Reset();
			if (OutFailureReason.IsEmpty())
			{
				OutFailureReason = TargetReason;
			}
		}
	}
}

void FStratSelectionMachine::SetLockedThisTurn(int32 UnitId, bool bLocked)
{
	if (bLocked)
	{
		LockedUnits.Add(UnitId);
	}
	else
	{
		LockedUnits.Remove(UnitId);
	}
}

void FStratSelectionMachine::Reset()
{
	SelectedUnitId = INDEX_NONE;
	DoneUnits.Reset();
	LockedUnits.Reset();
}

// ---------------------------------------------------------------------------
// Submission, and the one `STRAT-CMD accepted` line in the project.
// ---------------------------------------------------------------------------

namespace
{
	/**
	 * The SAVE FORMAT's spelling of a command kind, not this module's enum names.
	 *
	 * `Save.good.cpp:294-300` spells them `Move` / `Attack` / `Build` / `Capture` /
	 * `EndTurn`, and `kindFromName` parses those same words back. Matching it means a phase
	 * 6 gate can compare a `STRAT-CMD accepted` line against a `commandLog` entry without a
	 * translation table between them.
	 *
	 * THE FIVE LITERALS ARE UNAVOIDABLE *HERE*, and the reason is the module boundary and
	 * not the function's linkage. `strat::saveCommandName` is declared at
	 * `Source/StratRules/Save.h:135` and defined at `Save.good.cpp:292` at namespace scope
	 * with no `static` -- it has external linkage. What it does not carry is an `_API`
	 * macro, and an editor target is a modular build, so `StratPlay` cannot link a call to
	 * it (8 x LNK2019, `StratBridge.h`'s opening measurement). A routing fix would mean an
	 * `FStratBridge` method, and there is nothing for one to route: this switch is over
	 * `EStratSelectionCommand`, a StratPlay enum the bridge has no reason to know.
	 *
	 * SO IT IS A DELIBERATE SECOND SPELLING, and it is pinnable -- just not from this
	 * module. `Source/StratBridge/Tests/` CAN call `strat::saveCommandName` directly and
	 * assert these five words against the format's own spelling; that is the sharper clause
	 * `.agents/ue-project-context.md:189-191` asks for, and nothing in the tree forbids it.
	 */
	const TCHAR* CommandKindName(EStratSelectionCommand Command)
	{
		switch (Command)
		{
		case EStratSelectionCommand::Move:    return TEXT("Move");
		case EStratSelectionCommand::Attack:  return TEXT("Attack");
		case EStratSelectionCommand::EndTurn: return TEXT("EndTurn");
		case EStratSelectionCommand::None:    break;
		}
		return TEXT("None");
	}
}

bool StratSubmitSelectionCommand(FStratBridge&                 Bridge,
                                 const FStratSelectionOutcome& Outcome,
                                 FString&                      OutFailureReason)
{
	OutFailureReason.Reset();

	if (Outcome.Command == EStratSelectionCommand::None)
	{
		// Not a failure. A Wait, a selection and a cancel all arrive here.
		return true;
	}

	// READ BEFORE SUBMISSION, AND THIS IS THE REASON `Turn()` AND `SideToMove()` EXIST.
	// The bridge's five typed methods stamp `{turn, side}` off the live `TurnState` at the
	// instant of submission -- "the command that closes turn N is tagged N" -- so reading
	// these afterwards would print N+1 for every EndTurn and describe a command nobody
	// submitted.
	const int32 Turn = Bridge.Turn();
	const int32 Side = Bridge.SideToMove();

	FStratResult Applied;
	switch (Outcome.Command)
	{
	case EStratSelectionCommand::Move:
		Applied = Bridge.SubmitMoveToHex(Outcome.UnitId, Outcome.Hex);
		break;
	case EStratSelectionCommand::Attack:
		Applied = Bridge.SubmitAttackAtHex(Outcome.UnitId, Outcome.Hex);
		break;
	case EStratSelectionCommand::EndTurn:
		Applied = Bridge.SubmitEndTurn();
		break;
	case EStratSelectionCommand::None:
		// Unreachable -- returned above. Present so the switch is exhaustive and a new
		// kind is a compiler warning here rather than a silently dropped command.
		return true;
	}

	if (!Applied.bOk)
	{
		OutFailureReason = Applied.Reason;
		// A DIFFERENT SHAPE FROM THE ACCEPTED LINE, on purpose. A grep for
		// `STRAT-CMD accepted` must count commands that actually applied, so a refusal must
		// not contain that phrase.
		UE_LOG(LogStratPlay, Warning,
			TEXT("STRAT-CMD refused kind=%s unit=%d hex=%d,%d turn=%d side=%d reason=%s"),
			CommandKindName(Outcome.Command),
			Outcome.Command == EStratSelectionCommand::EndTurn ? INDEX_NONE : Outcome.UnitId,
			Outcome.Command == EStratSelectionCommand::EndTurn ? INDEX_NONE : Outcome.Hex.X,
			Outcome.Command == EStratSelectionCommand::EndTurn ? INDEX_NONE : Outcome.Hex.Y,
			Turn, Side, *OutFailureReason);
		return false;
	}

	// AFTER APPLICATION, deliberately -- that is the whole point of carrying it. Two runs
	// that accept the same commands in the same order print the same hashes, and the first
	// line where they differ names the command that diverged.
	const FString Hash = Bridge.StateHash();

	// THE LINE. Fixed field order, no conditional fields, always numeric. `unit` and `hex`
	// carry -1 where the kind has no such field (EndTurn names neither). See the header
	// block for why each of these choices is not free.
	UE_LOG(LogStratPlay, Log,
		TEXT("STRAT-CMD accepted kind=%s unit=%d hex=%d,%d turn=%d side=%d hash=%s"),
		CommandKindName(Outcome.Command),
		Outcome.Command == EStratSelectionCommand::EndTurn ? INDEX_NONE : Outcome.UnitId,
		Outcome.Command == EStratSelectionCommand::EndTurn ? INDEX_NONE : Outcome.Hex.X,
		Outcome.Command == EStratSelectionCommand::EndTurn ? INDEX_NONE : Outcome.Hex.Y,
		Turn, Side, *Hash);

	return true;
}
