// GDD §2.9 / §4.9 -- the AI turn runner, implementation, and the one producer of `STRAT-AI`.
//
// `StratBridge.h` IS INCLUDED HERE AND NOT FROM THE HEADER, the module-wide discipline. This
// file names `FStratAiCommand` and `EStratAiCommandKind` -- both plain, non-reflected engine
// types -- and calls the engine-typed façade methods. IT CALLS NO `strat::` FUNCTION: the
// vendored sources carry no `_API` macro, measured as 8 x LNK2019 (`StratBridge.h`'s opening
// measurement), so `NextAiCommand` on the bridge is the only door §2.9 can be reached
// through from this module at all.

#include "StratAiTurnRunner.h"

#include "StratBridge.h"
#include "StratPlay.h"

namespace
{
	/**
	 * The SAVE FORMAT's spelling of an AI command kind, not this project's enum names.
	 *
	 * IDENTICAL IN INTENT TO `CommandKindName` IN `StratSelectionMachine.cpp`, and
	 * deliberately not shared with it. That one maps `EStratSelectionCommand` (a UI concept
	 * with a `None` and no `Build`); this one maps `EStratAiCommandKind` (four rules-module
	 * kinds, no `None`). They agree on the four words they both spell BECAUSE both are
	 * copying `strat::saveCommandName`'s switch, which is the single source neither of them is. Fusing
	 * them into one helper over a merged enum would create exactly the merged enum this
	 * project does not have.
	 *
	 * THE LITERALS ARE UNAVOIDABLE HERE. `strat::saveCommandName` has external linkage and no
	 * `_API` macro, so it is reachable from `StratBridge` and from nowhere else; spelling the
	 * four words in this module is the module boundary's cost, not a preference.
	 *
	 * `None` IS FOR THE REFUSAL LINE ONLY. `EStratAiCommandKind` has no such enumerator --
	 * `StratBridge.h` records why there are four and not five -- so this string appears only
	 * where there IS no command: a `phase=decide` refusal, where the AI answered nothing at
	 * all. It must never appear on an `applied` line.
	 */
	const TCHAR* AiCommandKindName(EStratAiCommandKind Kind)
	{
		switch (Kind)
		{
		case EStratAiCommandKind::Build:   return TEXT("Build");
		case EStratAiCommandKind::Move:    return TEXT("Move");
		case EStratAiCommandKind::Attack:  return TEXT("Attack");
		case EStratAiCommandKind::EndTurn: return TEXT("EndTurn");
		}

		// A REFUSING DEFAULT AND NOT A CAST, on the precedent phase A set for the kind mapping
		// inside the bridge: `Ai.h` lives in another repository, and a re-vendored enumerator
		// would otherwise print as whichever word happened to be nearest.
		return TEXT("Unknown");
	}

	/**
	 * `STRAT-AI refused`, in one place so its field order cannot drift from the accepted line.
	 *
	 * A DELIBERATELY DIFFERENT PHRASE FROM `applied`, inheriting phase 4's decision for
	 * `STRAT-CMD refused` verbatim: `grep "STRAT-AI applied"` must count commands that
	 * actually reached `strat::GameState`, so a refusal may not contain that phrase.
	 *
	 * `phase=` IS THIS LINE'S ONE ADDITION OVER `STRAT-CMD refused`, and it is there because
	 * an AI turn has four distinct ways to fail and they send a reader to four different
	 * places. `decide` -- `NextAiCommand` refused, which is the bridge or the vendored AI.
	 * `apply` -- the AI named a command the rules module then rejected, which is a genuine
	 * disagreement between §2.9 and §2.5/§2.6 and the most interesting of the four.
	 * `bound` -- the AI never said EndTurn. `handover` -- the AI turns never gave the board
	 * back; that one is `UStratMatchSubsystem::RunAiTurnsNow`'s, reaching this function
	 * through `StratLogAiTurnRefusal` at the bottom of this file. Without the field all four
	 * read as "the AI broke".
	 */
	void LogAiRefusal(const TCHAR*              Phase,
	                  const EStratAiCommandKind Kind,
	                  const bool                bHasCommand,
	                  const FStratAiCommand&    Command,
	                  const int32               Turn,
	                  const int32               Side,
	                  const FString&            Reason)
	{
		UE_LOG(LogStratPlay, Warning,
			TEXT("STRAT-AI refused phase=%s kind=%s unit=%d hex=%d,%d def=%d target=%d ")
			TEXT("turn=%d side=%d reason=%s"),
			Phase,
			bHasCommand ? AiCommandKindName(Kind) : TEXT("None"),
			bHasCommand ? Command.UnitId : INDEX_NONE,
			bHasCommand ? Command.Hex.X : INDEX_NONE,
			bHasCommand ? Command.Hex.Y : INDEX_NONE,
			bHasCommand ? Command.DefIndex : INDEX_NONE,
			bHasCommand ? Command.TargetId : INDEX_NONE,
			Turn, Side, *Reason);
	}
}

// ---------------------------------------------------------------------------
// The production port. Five forwards and one switch.
// ---------------------------------------------------------------------------

int32 FStratBridgeAiTurnPort::Turn() const
{
	return Bridge != nullptr ? Bridge->Turn() : INDEX_NONE;
}

int32 FStratBridgeAiTurnPort::SideToMove() const
{
	return Bridge != nullptr ? Bridge->SideToMove() : INDEX_NONE;
}

FString FStratBridgeAiTurnPort::StateHash() const
{
	return Bridge != nullptr ? Bridge->StateHash() : FString();
}

bool FStratBridgeAiTurnPort::NextCommand(int32            Side,
                                         FStratAiCommand& OutCommand,
                                         FString&         OutFailureReason) const
{
	OutFailureReason.Reset();

	if (Bridge == nullptr)
	{
		OutFailureReason = TEXT("this AI port has no bridge");
		return false;
	}

	// THE REFUSAL IS FORWARDED VERBATIM, on `UStratMatchSubsystem.cpp`'s line: every refusal
	// the bridge returns is a refusal the rules module returned, and rewording it here would
	// put this file's guess in front of the failing layer's own account.
	//
	// `OutCommand` IS LEFT EXACTLY AS THE BRIDGE LEFT IT ON A REFUSAL and is deliberately not
	// reset. Resetting would be this file inventing a second answer, and the runner is
	// documented not to read it -- see `IStratAiTurnPort::NextCommand`.
	const FStratResult Answered = Bridge->NextAiCommand(Side, OutCommand);
	if (!Answered.bOk)
	{
		OutFailureReason = Answered.Reason;
		return false;
	}

	return true;
}

bool FStratBridgeAiTurnPort::Submit(const FStratAiCommand& Command, FString& OutFailureReason)
{
	OutFailureReason.Reset();

	if (Bridge == nullptr)
	{
		OutFailureReason = TEXT("this AI port has no bridge");
		return false;
	}

	// FOUR ARMS, ALL RECORDING. Each of these stamps `{turn, side}` off the authoritative
	// TurnState and appends to `RecordedLog()` -- they are the same methods a player's command
	// takes. There is no fifth arm and there must not be one: adding a non-recording apply
	// here is the precise defect phase 4's gate blocked on, and it would be invisible in every
	// hash comparison that drove both sides through this same function.
	//
	// `Hex` IS SUBMITTED AS IT STANDS FOR ALL THREE HEX KINDS, including Attack.
	// `NextAiCommand` resolved `AiCommand::targetId` -> hex on the authoritative board at the
	// instant it asked, and `StratBridge.h` records why that resolution may not happen out
	// here: the view model this module can see is rebuilt on its own schedule and can describe
	// a board one command older than the one the AI decided against.
	FStratResult Applied;
	switch (Command.Kind)
	{
	case EStratAiCommandKind::Build:
		Applied = Bridge->SubmitBuildAtHex(Command.Hex, Command.DefIndex);
		break;

	case EStratAiCommandKind::Move:
		Applied = Bridge->SubmitMoveToHex(Command.UnitId, Command.Hex);
		break;

	case EStratAiCommandKind::Attack:
		Applied = Bridge->SubmitAttackAtHex(Command.UnitId, Command.Hex);
		break;

	case EStratAiCommandKind::EndTurn:
		Applied = Bridge->SubmitEndTurn();
		break;

	default:
		// UNREACHABLE THROUGH `NextAiCommand`, which maps the vendored enum with a refusing
		// switch of its own rather than a cast. It is written anyway because a re-vendored
		// `Ai.h` with a fifth kind would otherwise reach here and fall out of the switch
		// having submitted nothing, and "submitted nothing" would be reported as success.
		OutFailureReason = FString::Printf(
			TEXT("this AI port has no arm for command kind %d"), static_cast<int32>(Command.Kind));
		return false;
	}

	if (!Applied.bOk)
	{
		OutFailureReason = Applied.Id.IsEmpty()
			? Applied.Reason
			: FString::Printf(TEXT("[%s] %s"), *Applied.Id, *Applied.Reason);
		return false;
	}

	return true;
}

// ---------------------------------------------------------------------------
// The loop, and the `STRAT-AI` lines.
//
// THE THREE FORMAT STRINGS, WRITTEN OUT SO A GATE CAN BE BUILT ON THEM. Phase D builds a
// machine-repeatable AI-vs-AI gate over these, so they are fixed-field, unconditional, and
// modelled field for field on phase 4's `STRAT-CMD` line:
//
//   STRAT-AI applied kind=%s unit=%d hex=%d,%d def=%d target=%d turn=%d side=%d hash=%s
//   STRAT-AI refused phase=%s kind=%s unit=%d hex=%d,%d def=%d target=%d turn=%d side=%d reason=%s
//   STRAT-AI turn-ended side=%d turn=%d commands=%d hash=%s
//
// THREE FORMAT STRINGS, FOUR `phase` VALUES, AND EXACTLY ONE TRANSLATION UNIT SPELLING ANY OF
// THEM. `phase` is `decide` | `apply` | `bound` | `handover`. The first three are `RunTurn`'s
// own and are emitted below. `handover` is `UStratMatchSubsystem::RunAiTurnsNow`'s -- it bounds
// CONSECUTIVE AI TURNS, a fault a runner that plays one turn cannot see -- and it is emitted by
// calling `StratLogAiTurnRefusal` at the bottom of this file, so it goes out through the format
// string above rather than through a second one. That is not tidiness: the second one existed
// for the length of one diff, carried two fields instead of nine, and would have tripped the
// phase-D field parser on the one line whose whole purpose is to say the gate's run did not
// finish.
//
// The inherited decisions, each of which a later reader will otherwise "tidy":
//   - `kind` SPELLS THE SAVE FORMAT'S OWN WORDS (`Build`/`Move`/`Attack`/`EndTurn`), so a
//     line can be compared against a `commandLog` entry with no translation table.
//   - `unit`, `hex`, `def` AND `target` ARE ALWAYS NUMERIC, carrying `-1` and `-1,-1` where
//     the kind has no such field. The line's shape never depends on its content. `def` and
//     `target` are this line's two additions over `STRAT-CMD`, and they exist because Build
//     and Attack carry information the player's line has no equivalent of: a `defIndex` is
//     the raw index phase 0 established as load-bearing, and `TargetId` names the unit the AI
//     chose, which is the one thing the resolved hex does not tell you.
//   - `turn` AND `side` ARE READ BEFORE SUBMISSION and `hash` AFTER. `FStratBridge`'s typed
//     methods stamp the command with the values live at that instant -- "the command that
//     closes turn N is tagged N" -- so reading afterwards would print N+1 for every EndTurn
//     and describe a command nobody submitted.
//   - `refused` IS A DIFFERENT PHRASE FROM `applied`, so `grep "STRAT-AI applied"` counts
//     only commands that reached `strat::GameState`.
//   - `turn-ended` IS A THIRD PHRASE AND NOT A FIELD ON THE APPLIED LINE, so `grep -c
//     "STRAT-AI turn-ended"` counts AI turns while `grep -c "STRAT-AI applied"` counts AI
//     commands. Its `turn` is the turn the AI PLAYED, captured at entry, not the turn the
//     applied EndTurn advanced into.
//
// AI COMMANDS PRODUCE NO `STRAT-CMD` LINE, and that is deliberate rather than an oversight.
// `StratSubmitSelectionCommand` takes an `FStratSelectionOutcome`, which has no Build kind and
// describes a click; routing AI commands through it would mean inventing a click. The
// consequence is worth stating because a gate depends on it: `grep -c "STRAT-CMD accepted"`
// counts HUMAN commands and `grep -c "STRAT-AI applied"` counts AI ones, with no overlap and
// no double counting, and `FStratBridge::RecordedCommandCount()` is the sum.
// ---------------------------------------------------------------------------

FStratAiTurnOutcome FStratAiTurnRunner::RunTurn(IStratAiTurnPort& Port)
{
	FStratAiTurnOutcome Outcome;

	// READ ONCE, AT ENTRY. The side the AI is asked about for the whole turn, and the turn
	// number the `turn-ended` line will carry. See the header on why a side read per iteration
	// would let the loop keep playing for whoever is up after an EndTurn slipped through.
	Outcome.Side = Port.SideToMove();
	Outcome.Turn = Port.Turn();

	if (MaxCommandsPerTurn <= 0)
	{
		// NOT "UNBOUNDED". See the field's declaration: an unbounded configuration is the one
		// in which a looping AI hangs PIE and the automation suite, and it would be reachable
		// from a Blueprint default with no compiler diagnostic.
		Outcome.FailureReason = FString::Printf(
			TEXT("MaxCommandsPerTurn is %d; an AI turn must be allowed at least one command"),
			MaxCommandsPerTurn);
		LogAiRefusal(TEXT("bound"), EStratAiCommandKind::EndTurn, /*bHasCommand=*/false,
			FStratAiCommand(), Outcome.Turn, Outcome.Side, Outcome.FailureReason);
		return Outcome;
	}

	for (int32 Step = 0; Step < MaxCommandsPerTurn; ++Step)
	{
		// ---- Ask ------------------------------------------------------------
		FStratAiCommand Command;
		FString         DecideReason;

		// THE `bOk` BRANCH, AND IT COMES BEFORE ANYTHING READS `Command.Kind`. This ordering
		// is the whole hazard phase A recorded: a default-constructed `FStratAiCommand` reads
		// as a genuine EndTurn, so a loop that looked at the kind first would treat every
		// refusal as a clean turn end and report no fault at all.
		if (!Port.NextCommand(Outcome.Side, Command, DecideReason))
		{
			Outcome.FailureReason = FString::Printf(
				TEXT("the AI refused to answer for side %d after %d applied command(s): %s"),
				Outcome.Side, Outcome.CommandsApplied, *DecideReason);

			// `bHasCommand=false`: there is no command here, and printing the fields of a
			// value the port is documented not to have written would be printing a plausible
			// EndTurn that nobody decided.
			LogAiRefusal(TEXT("decide"), EStratAiCommandKind::EndTurn, /*bHasCommand=*/false,
				Command, Port.Turn(), Outcome.Side, DecideReason);
			return Outcome;
		}

		// ---- Submit ----------------------------------------------------------
		// Read BEFORE the submission, for the reason in the block above.
		const int32 TurnBefore = Port.Turn();
		const int32 SideBefore = Port.SideToMove();

		FString ApplyReason;
		if (!Port.Submit(Command, ApplyReason))
		{
			Outcome.FailureReason = FString::Printf(
				TEXT("the rules module refused the AI's %s for side %d after %d applied ")
				TEXT("command(s): %s"),
				AiCommandKindName(Command.Kind), Outcome.Side, Outcome.CommandsApplied,
				*ApplyReason);

			LogAiRefusal(TEXT("apply"), Command.Kind, /*bHasCommand=*/true, Command,
				TurnBefore, SideBefore, ApplyReason);
			return Outcome;
		}

		++Outcome.CommandsApplied;

		UE_LOG(LogStratPlay, Log,
			TEXT("STRAT-AI applied kind=%s unit=%d hex=%d,%d def=%d target=%d turn=%d ")
			TEXT("side=%d hash=%s"),
			AiCommandKindName(Command.Kind),
			Command.UnitId, Command.Hex.X, Command.Hex.Y, Command.DefIndex, Command.TargetId,
			TurnBefore, SideBefore, *Port.StateHash());

		// ---- Stop ------------------------------------------------------------
		// AN APPLIED EndTurn IS THE ONLY SUCCESSFUL EXIT. Not "the side changed" -- that would
		// be inferring the turn ended from a symptom of it, and it would also be true after a
		// refusal that happened to coincide with a hand-over.
		if (Command.Kind == EStratAiCommandKind::EndTurn)
		{
			Outcome.bEndedTurn = true;
			Outcome.bOk        = true;

			UE_LOG(LogStratPlay, Log,
				TEXT("STRAT-AI turn-ended side=%d turn=%d commands=%d hash=%s"),
				Outcome.Side, Outcome.Turn, Outcome.CommandsApplied, *Port.StateHash());

			return Outcome;
		}
	}

	// ---- The bound -------------------------------------------------------
	// A REPORTED FAULT AND NOT A QUIET STOP. See the header: a silent bound presents as an AI
	// that stops halfway through its turn, which is indistinguishable from an AI that chose
	// to. Everything applied so far STAYS applied -- the rules module accepted each of those
	// commands and this runner has no authority to undo one -- so the caller is told how many
	// there were.
	Outcome.FailureReason = FString::Printf(
		TEXT("the AI applied %d command(s) for side %d without ever ending its turn ")
		TEXT("(MaxCommandsPerTurn is %d); the turn was stopped and those commands stand"),
		Outcome.CommandsApplied, Outcome.Side, MaxCommandsPerTurn);

	LogAiRefusal(TEXT("bound"), EStratAiCommandKind::EndTurn, /*bHasCommand=*/false,
		FStratAiCommand(), Port.Turn(), Outcome.Side, Outcome.FailureReason);

	return Outcome;
}

// ---------------------------------------------------------------------------
// The one door into the format string from outside this file. See the header.
// ---------------------------------------------------------------------------

void StratLogAiTurnRefusal(const TCHAR* const Phase, const int32 Turn, const int32 Side,
                           const FString& Reason)
{
	// `EndTurn` IS A PLACEHOLDER THAT IS NEVER PRINTED. `bHasCommand=false` makes the formatter
	// spell `kind=None` and fill every command field with `-1`, exactly as the runner's own
	// `decide` and `bound` refusals do; the enumerator here only satisfies the parameter. A
	// caller cannot pass a command through this door and there is deliberately no overload that
	// would let it -- see the header on why `FStratAiCommand` may not appear in it.
	LogAiRefusal(Phase, EStratAiCommandKind::EndTurn, /*bHasCommand=*/false,
		FStratAiCommand(), Turn, Side, Reason);
}
