#include "StratAiPlayback.h"

// THE ONE PLACE `StratBridge.h` ENTERS FOR THIS PAIR OF TYPES, and it is a .cpp -- the header
// forward declares `EStratAiCommandKind` and `FStratAiCommand` and this file supplies them, on
// `StratAiTurnRunner.cpp`'s pattern exactly. NOTHING HERE CALLS A `strat::` FUNCTION and
// nothing here names a `strat::` TYPE: `FStratAiCommand` is a plain engine-typed struct that
// happens to live in that header. See `StratBridge.h`'s own block on why the distinction
// between naming a vendored type and calling a vendored function is the one that matters.
#include "StratBridge.h"

void FStratAiPlaybackReel::Reset()
{
	// `Reset()` AND NOT `Empty()`: the allocation is kept. A hand-over is a dozen-odd steps and
	// this runs once per hand-over, so the saving is nothing; it is written this way because
	// the array is refilled immediately and shrinking it to refill it is noise in a profile.
	Steps.Reset();
	Cursor = 0;
}

void FStratAiPlaybackReel::Record(const FStratAiCommand& Command, int32 Side, int32 Turn)
{
	FStratAiPlaybackStep& Step = Steps.AddDefaulted_GetRef();

	Step.Kind     = Command.Kind;
	Step.Side     = Side;
	Step.Turn     = Turn;
	Step.UnitId   = Command.UnitId;
	Step.TargetId = Command.TargetId;
	Step.DefIndex = Command.DefIndex;
	Step.Hex      = Command.Hex;

	// THE ONE JUDGEMENT THIS FILE MAKES, AND IT IS ABOUT A FIELD'S MEANING RATHER THAN ABOUT
	// THE GAME. Build, Move and Attack each carry a hex the command was ABOUT --
	// `FStratAiCommand::Hex` says so on itself, "Move dest, Build factory, Attack target hex".
	// EndTurn carries the default `(0,0)` and means nothing by it. See `bHasHex` in the header
	// on why that default cannot be left to speak for itself.
	//
	// WRITTEN AS "NOT EndTurn" AND NOT AS A THREE-WAY OR, so that a fifth kind added to
	// `EStratAiCommandKind` arrives here as "has a hex" rather than as "silently invisible to
	// the camera". Of the two ways to be wrong about a kind nobody has written yet, panning to
	// a hex that turns out not to matter is recoverable by looking at it; never panning at all
	// is a feature that appears not to exist.
	Step.bHasHex = (Command.Kind != EStratAiCommandKind::EndTurn);
}

const FStratAiPlaybackStep* FStratAiPlaybackReel::Peek() const
{
	return Steps.IsValidIndex(Cursor) ? &Steps[Cursor] : nullptr;
}

bool FStratAiPlaybackReel::Advance()
{
	if (Cursor >= Steps.Num())
	{
		// ALREADY FINISHED. Reported rather than clamped silently, because the driving timer
		// uses this return to decide when to stop rearming itself; a call that quietly did
		// nothing would leave that timer running forever over an exhausted reel.
		return false;
	}

	++Cursor;
	return true;
}

bool FStratAiPlaybackReel::SkipToEnd()
{
	if (Cursor >= Steps.Num())
	{
		// NOT PLAYING. The caller uses this `false` to leave the input alone -- see the header:
		// a click at a quiet moment must reach the selection machine like any other, and a
		// skip that claimed to have consumed one would make the board unclickable.
		return false;
	}

	Cursor = Steps.Num();
	return true;
}
