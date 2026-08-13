// GDD §2.9 / §4.9 / T-AI-01 -- the AI turn runner: ask, submit, repeat, stop.
//
// WHAT GAP THIS CLOSES. Phase A put §2.9's opponent behind `FStratBridge::NextAiCommand`,
// which answers ONE command. Nothing in the engine turned a sequence of those answers into a
// turn. This file is that loop and nothing else: it decides no command, resolves no hex,
// picks no target and reads no rule. Every answer it acts on came from the bridge on the
// call immediately before it acted.
//
// IT IS A PLAIN STRUCT AND NOT A UCLASS, AND NOT ON AN ACTOR -- the shape
// `FStratSelectionMachine` takes, for the reason that file gives first: TESTABILITY. It is
// drivable with no actor, no world, no PIE and no Slate in existence, and against a scripted
// port rather than a seeded bridge. That last part is not a convenience. The three failures
// this file exists to prevent -- a refusal read as an EndTurn, a submission that is refused
// mid-turn, and an AI that never says EndTurn -- CANNOT BE PRODUCED BY THE SHIPPING AI. A
// runner reachable only through `FStratBridge` would therefore have all three of its fault
// arms permanently unexecuted, which is the state phase 4's second gate finding
// (`EStratSelectionCommand::Attack`, executed by no test) blocked on. `IStratAiTurnPort` is
// how a test makes the AI refuse.
//
// THE `bOk` HAZARD IS THE WHOLE REASON THE PORT RETURNS A `bool` SEPARATE FROM THE COMMAND.
// `FStratAiCommand`'s default-constructed value is `Kind = EndTurn`, and `StratBridge.h` says
// so on the field: "a refused `NextAiCommand` leaves this value here, and it is NOT
// distinguishable from a genuine EndTurn by inspection -- `FStratResult::bOk` is the only
// thing that tells them apart." A loop that switched on `Kind` first would read every refusal
// -- an unseeded bridge, an unknown side, an attack whose target is not on the board -- as
// "the AI is done", end the turn cleanly, log nothing alarming, and hand play back. That is a
// silent empty turn with no fault reported anywhere, and it is the single most likely way this
// phase fails. `RunTurn` therefore branches on the port's `bool` BEFORE it names `Kind`, in
// both places a `bool` arrives, and a refusal is a hard stop with a `STRAT-AI refused` line --
// never a turn end.
//
// IT HOLDS NO MIRROR OF THE RULES STATE, on `FStratSelectionMachine`'s line exactly. Turn,
// side and state hash are read off the port at the instant they are printed; the AI is asked
// again after every applied command rather than handed a plan to execute. The rejected shape
// -- ask once for a list of commands and submit them -- was killed by the same argument that
// killed optimistic selection state: the second command of such a list is decided against a
// board that the first command has already changed, and nothing would ever exercise the path
// where they disagree.
//
// THE TERMINATION BOUND IS NOT "UNTIL ENDTURN". `strat::nextCommand` is a pure function of
// state, so an AI that answers a command which applies without changing the state it read
// would loop forever -- taking PIE and the automation suite with it. `MaxCommandsPerTurn`
// bounds it, and EXCEEDING THE BOUND IS A REPORTED FAULT AND NOT A QUIET STOP: it returns
// `bOk = false` with a reason naming the count, and logs `STRAT-AI refused phase=bound`. A
// bound that stopped silently would present as an AI that mysteriously stops halfway through
// its turn, which is indistinguishable from an AI that decided to.
//
// EVERY COMMAND GOES OUT THROUGH A RECORDING ENTRY POINT. `FStratBridgeAiTurnPort::Submit`
// calls `SubmitBuildAtHex` / `SubmitMoveToHex` / `SubmitAttackAtHex` / `SubmitEndTurn`, which
// are the same `{turn, side}`-stamping, `RecordedLog()`-appending methods a player's command
// takes -- `StratBridge.h` states the obligation on `SubmitBuildAtHex` itself: "a second,
// non-recording apply path is the precise defect `ReplayRecordedLogOnto` was added to make
// visible; this method must never become one." So an AI-vs-AI game serializes and replays
// like any other, and phase 4's gate finding does not reopen through this door.
//
// IT NAMES NO `strat::` TYPE AND CALLS NO `strat::` FUNCTION. `FStratAiCommand` and
// `EStratAiCommandKind` are plain, non-reflected engine-typed declarations in `StratBridge.h`;
// both are FORWARD DECLARED here and the include lives in the .cpp, so nothing that includes
// this header pulls the vendored rules headers into its translation unit. `StratBridge.h`
// declares no reflected type, so this is a discipline about compile environments rather than
// about UHT -- but the discipline is the module's and this file keeps it.
//
// NOT IN THIS ROUND, with reasons:
// - CAPTURE. `Ai.h:62-66` omits it deliberately: `openTurn` runs `captureTick` from the
//   EndTurn arm (`Replay.good.cpp:264-270`, `:529-535`), so the AI captures without ever
//   emitting Capture. A Capture arm here would be an engine-side command the AI cannot ask
//   for. `EStratAiCommandKind` has four enumerators and this switch has four arms.
// - WAIT. There is no such rules command (phase 1's correction) and the AI has no vocabulary
//   for one.
// - PACING. This runner runs a whole turn synchronously. Delay between turns is
//   `UStratMatchSubsystem`'s, because a delay needs a world and this struct deliberately does
//   not have one. A per-command delay would make this an incremental machine with resumable
//   state, and resumable state is a mirror of the rules state by another name.
// - CHOOSING WHICH SIDE IS AI. That is configuration, and it arrives on
//   `FStratMatchConfig::AiSides` from a Blueprint default.
#pragma once

#include "CoreMinimal.h"

// Forward declarations only -- see the header block. `EStratAiCommandKind` is declared in
// `StratBridge.h` with an explicit `uint8` underlying type, which is what makes this legal;
// if that ever changes, this line stops compiling rather than silently disagreeing.
enum class EStratAiCommandKind : uint8;
struct FStratAiCommand;
class FStratBridge;

/**
 * The rules side of one AI turn, and nothing else.
 *
 * FIVE METHODS, EACH OF WHICH IS SOMEBODY ELSE'S ANSWER. There is deliberately no
 * `IsMatchOver()` and no `IsSideAi()`: whether this side should be playing at all is decided
 * before `RunTurn` is called, by the subsystem that has the view model. A runner that could
 * ask would be a runner that could decide.
 *
 * WHY AN INTERFACE RATHER THAN AN `FStratBridge&` -- the same two reasons
 * `IStratRulesQuery` gives, plus one that is specific to this file. First, the production
 * adapter (`FStratBridgeAiTurnPort`, below) becomes a named, separately assertable thing
 * rather than a call site buried in a loop. Second, the struct is drivable with no seeded
 * bridge. Third and decisively: the shipping AI never refuses, never loops and never fails a
 * submission, so the fault arms of `RunTurn` are unreachable through a real bridge and would
 * ship unexecuted. This is not a hook for an alternative shipping AI -- there is exactly one
 * production implementation and a second one would be the substitution T-UI-02 exists to
 * catch, applied to §2.9.
 */
struct STRATPLAY_API IStratAiTurnPort
{
	virtual ~IStratAiTurnPort() = default;

	/** §4.10's turn number, live. Read before each submission, never after -- see the .cpp. */
	virtual int32 Turn() const = 0;

	/** Whose turn the rules module says it is, live. */
	virtual int32 SideToMove() const = 0;

	/**
	 * §2.9's next command for `Side`.
	 *
	 * THE `bool` IS THE ANSWER AND `OutCommand` IS ONLY MEANINGFUL WHEN IT IS TRUE. See the
	 * header block: a default-constructed `FStratAiCommand` reads as a genuine EndTurn, so an
	 * implementation that returned `false` and left a plausible-looking command behind would
	 * be indistinguishable from one that ended the turn. Callers must not read `OutCommand`
	 * on a `false` return and `FStratAiTurnRunner` does not.
	 */
	virtual bool NextCommand(int32 Side, FStratAiCommand& OutCommand,
	                         FString& OutFailureReason) const = 0;

	/**
	 * Applies one AI command through a RECORDING entry point.
	 *
	 * "Recording" is part of the contract and not an implementation detail: the phase-4 gate
	 * blocked on a submission path whose recording could not be pinned, and an AI that applied
	 * commands outside `RecordedLog()` would produce a match whose save replays to a different
	 * state with nothing to point at.
	 */
	virtual bool Submit(const FStratAiCommand& Command, FString& OutFailureReason) = 0;

	/** §4.10's canonical state hash, for the log line. Never compared here. */
	virtual FString StateHash() const = 0;
};

/**
 * The one production implementation: five forwards onto `FStratBridge`.
 *
 * BORROWS AND NEVER OWNS, exactly as `FStratBridgeRulesQuery` does and for that struct's
 * reason: the bridge is the match subsystem's, and outliving it is the caller's problem to
 * avoid rather than this object's to solve with a copy of a `strat::GameState`.
 *
 * NON-CONST BRIDGE, unlike `FStratBridgeRulesQuery`. This one submits.
 */
struct STRATPLAY_API FStratBridgeAiTurnPort final : public IStratAiTurnPort
{
	explicit FStratBridgeAiTurnPort(FStratBridge* InBridge)
		: Bridge(InBridge)
	{
	}

	virtual int32   Turn() const override;
	virtual int32   SideToMove() const override;
	virtual bool    NextCommand(int32 Side, FStratAiCommand& OutCommand,
	                            FString& OutFailureReason) const override;
	virtual bool    Submit(const FStratAiCommand& Command, FString& OutFailureReason) override;
	virtual FString StateHash() const override;

private:
	FStratBridge* Bridge = nullptr;
};

/**
 * What one AI turn came to. A value, complete in itself.
 *
 * `bOk` AND `bEndedTurn` ARE TWO FACTS AND NOT ONE. A turn can end without being ok only if
 * something between the last command and the EndTurn refused, and it can be ok without
 * having ended only if -- nothing, today; the runner returns `bOk` true exactly when it saw
 * an applied EndTurn. They are separate fields anyway, because collapsing them would leave a
 * caller unable to tell "the AI finished" from "the loop stopped" without reading the
 * sentence.
 */
struct STRATPLAY_API FStratAiTurnOutcome
{
	/** True only when an EndTurn was APPLIED. Every refusal and the bound leave this false. */
	bool bOk = false;

	/** True when the applied final command was EndTurn. See above on why this is not `bOk`. */
	bool bEndedTurn = false;

	/** The side the AI was asked about, captured once at entry. `INDEX_NONE` before entry. */
	int32 Side = INDEX_NONE;

	/** The turn number live when the runner started. Not re-read after the EndTurn applies. */
	int32 Turn = INDEX_NONE;

	/** How many commands the rules module ACCEPTED, EndTurn included. */
	int32 CommandsApplied = 0;

	/** Why the turn stopped short, in the refusing layer's own words. Empty when `bOk`. */
	FString FailureReason;
};

/**
 * §2.9's turn: ask the AI, submit what it said, repeat until it says EndTurn.
 *
 * DEFAULT-CONSTRUCTIBLE AND IMMEDIATELY USABLE. No `Initialize`, no bridge, no world.
 */
struct STRATPLAY_API FStratAiTurnRunner
{
	/**
	 * THE TERMINATION BOUND, and the reason it is a field rather than a constant is that
	 * `FStratMatchConfig` carries it from a Blueprint default so a scenario with a larger
	 * roster can raise it without a code change.
	 *
	 * 256 IS A CEILING AND NOT AN ESTIMATE. Phase A measured a whole self-play GAME at 156
	 * commands across 11 turns -- about 14 per turn -- so this is more than an order of
	 * magnitude above anything §2.9 has been observed to do in one turn, which is the
	 * property a runaway guard wants. A bound near the observed value would fire on a
	 * scenario with more units and report a defect that is not one.
	 *
	 * A VALUE OF ZERO OR LESS REFUSES IMMEDIATELY rather than meaning "unbounded". An
	 * unbounded option would be the one configuration in which a looping AI hangs the suite,
	 * and it would be reachable from a Blueprint default with no compiler diagnostic.
	 */
	int32 MaxCommandsPerTurn = 256;

	/**
	 * Plays one side's whole turn. The one entry point.
	 *
	 * @param Port  the rules side. `SideToMove()` is read ONCE, at entry, and that value is
	 *              what the AI is asked about for the rest of the turn -- so a side that
	 *              changes underneath the loop ends it rather than silently continuing to
	 *              play for whoever is up now.
	 *
	 * EMITS EVERY `STRAT-AI` LINE EXCEPT `phase=handover`. The three formats and the reason
	 * each field is shaped the way it is are in the .cpp. `handover` is the fourth phase, it
	 * belongs to `UStratMatchSubsystem::RunAiTurnsNow` because only that function has the
	 * outer loop it bounds, and it reaches this file's format string through
	 * `StratLogAiTurnRefusal` below rather than writing a second one.
	 */
	FStratAiTurnOutcome RunTurn(IStratAiTurnPort& Port);
};

/**
 * `STRAT-AI refused` for a refusal that has NO COMMAND, emitted from outside this file.
 *
 * WHY THIS EXISTS AT ALL. `UStratMatchSubsystem::RunAiTurnsNow` bounds the number of
 * CONSECUTIVE AI turns, which is a fault only that function can observe -- the runner plays one
 * turn and has no idea it is the ninth in a row. So there is a fourth `phase` value,
 * `handover`, that this file cannot emit and that function must. The gate finding this closes
 * was that it emitted its own two-field line instead, which the format's own design rule
 * ("`unit`/`hex` are always numeric, carrying `-1`/`-1,-1` where the kind has no such field, so
 * the line's shape never depends on its content") exists to forbid: phase D splits every
 * `STRAT-AI` line the same way, and one line shaped differently defeats the rule rather than
 * being excused by it.
 *
 * THE FORMAT STRING STAYS IN ONE TRANSLATION UNIT, which is the whole point of routing the
 * call here rather than exporting the format. A second `UE_LOG` spelling these fields is a
 * second thing to keep in step, and the first one drifted within a single diff.
 *
 * IT TAKES NO COMMAND BECAUSE THERE IS NEVER ONE. Every caller of this overload is refusing
 * before or after a command rather than about one, so the line carries `kind=None unit=-1
 * hex=-1,-1 def=-1 target=-1` unconditionally. A variant that took an `FStratAiCommand` would
 * need `StratBridge.h` in this header, which is the include this module does not do.
 *
 * @param Phase  the `phase=` value. `handover` is the only one used from outside this file;
 *               `decide`, `apply` and `bound` are the runner's own and stay private to it.
 * @param Turn   §4.10's turn number, read live at the refusal.
 * @param Side   the side whose turn it was, read live at the refusal.
 */
STRATPLAY_API void StratLogAiTurnRefusal(const TCHAR* Phase, int32 Turn, int32 Side,
                                         const FString& Reason);
