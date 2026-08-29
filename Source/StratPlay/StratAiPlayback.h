// GDD §2.11.2 / T-TURN-09 / GATE-AITURN -- the recorded AI action list and its cursor.
//
// WHAT GAP THIS CLOSES. §2.11.2 asks for one thing this tree could not do: "the headless AI
// resolves instantly; the presentation layer replays its action list at a watchable fixed pace
// (~0.5 s per action, camera stepping to each) ... this is presentation pacing only, no rules
// change. Any click or Esc skips to the end state." `FStratAiTurnRunner` resolved the turn
// instantly and threw the sequence away -- every command it submitted was visible only as a
// `STRAT-AI applied` log line, and a log line is not something a camera can step to. There was
// no action list in this process to replay. This file is that list, and a cursor over it.
//
// IT IS A RECORDING AND NOT AN INFERENCE, WHICH IS THE DISTINCTION THAT MAKES IT LEGAL HERE.
// `StratUnitActor.h` records that the ordered event list §4.9 part 2 names is NOT built in this
// tree, and that reconstructing an event by diffing two states is the conflation
// `StratBridge.h` warns about. Verified still true on this tree before this file was written.
// Nothing here diffs anything: `FStratAiTurnRunner::RunTurn` appends a step at the instant the
// rules module ACCEPTED that exact command, so every entry is a command that was submitted
// rather than a command someone worked out must have been. If the §4.9 event list ever lands,
// this reel is a candidate to be replaced by it and not a competitor to it.
//
// THE EXECUTION IS NOT SPLIT AND MUST NOT BE. `StratAiTurnRunner.h` refuses a per-command
// delay in the runner, with its reason: "A per-command delay would make this an incremental
// machine with resumable state, and resumable state is a mirror of the rules state by another
// name." That ruling is untouched. The runner still plays the whole turn synchronously, in one
// call, before anything here is looked at; the reel is filled as a by-product and read only
// afterwards. `Cursor` advances over a list of things that have ALREADY HAPPENED, so it can
// never be resumed into, cannot be behind the rules state, and has nothing to mirror.
//
// AND THAT IS WHY THE BOARD DOES NOT REWIND, which is the load-bearing consequence and is
// stated here rather than discovered at the keyboard. By the time playback starts, the board
// actors have been reconciled to the FINAL view model -- units stand where the AI left them.
// So "camera stepping to each" is what §2.11.2 gets: a tour of the hexes the AI acted on, in
// the order it acted on them, over the finished board. THE ALTERNATIVE WAS A PER-STEP BOARD --
// show the units where they stood at step 3 -- AND IT WAS REJECTED, because the only way to
// draw that board is to hold a mid-turn copy of the rules state and reconcile against it, and
// that copy is exactly the mirror the two headers above forbid. §2.11.2's own words license
// this reading: it says "presentation pacing only, no rules change", and it names the camera as
// the thing that steps.
//
// A CONSEQUENCE OF THAT WORTH SAYING OUT LOUD: "any click or Esc skips to the end state" is
// SATISFIED TRIVIALLY HERE, because the end state was never absent. Skipping abandons the tour;
// it does not fast-forward a simulation. A clause under `T-TURN-09` that asserts the three
// dispositions (never played, watched to the end, skipped part-way) reach an identical state is
// therefore cheap BY CONSTRUCTION rather than by careful bookkeeping, and that is the property
// worth pinning: this file holds no `FStratBridge`, submits nothing, and has no route to.
//
// PLAIN STRUCTS, NOT `UCLASS`ES, AND NOT ON AN ACTOR -- `FStratAiTurnRunner`'s shape and
// `FStratSelectionMachine`'s, for their reason: TESTABILITY. A reel is fillable and steppable
// with no world, no timer, no PIE and no Slate, so the ordering, the cursor bounds and the skip
// are all assertable headlessly. The subsystem supplies the clock; this file supplies nothing
// but the list and the position in it.
//
// IT IS DELIBERATELY NOT IN THE VIEW MODEL, and that omission was ruled on before this lane
// started. `T-INT-05`'s subject is "every member of the view-model", so a playback cursor placed
// in `FStratViewModel` would enter that ID's subject by its own words and owe a clause under it.
// It is not placed there. The reel lives on `UStratMatchSubsystem` beside the timer that drives
// it, `FStratViewModel` gains no member, and `T-INT-05` stays refused for this wave. The other
// shape -- a `PlaybackCursor` field on the presentation block, so a widget could bind to it --
// would have bought a progress readout and cost a new obligation under an ID whose claim is
// about rebuilding the screen from the model; there is no widget asking for it today.
//
// NOT IN THIS ROUND, with reasons:
// - A HIGHLIGHT ON THE STEPPED HEX. `AStratBoardActor` has three overlay components and each
//   is spoken for (`ShowReach`, `ShowTargets`, `ShowObjective`); `ShowObjective`'s own comment
//   rules that a new use gets a NEW COMPONENT and never a third use of an existing one. A
//   fourth component needs a material, and `Content/` is not touched from a lane tree. §2.11.2
//   asks for the camera and this delivers the camera.
// - ANIMATING THE CAMERA BETWEEN STEPS. `AStratCameraPawn::FocusWorldLocation` snaps, and
//   whether it should ease is that pawn's question and not this list's.
// - RECORDING THE HUMAN'S COMMANDS. `FStratBridge::RecordedLog()` already holds every command
//   from both sides and is the §4.10 artifact; this reel is a presentation scratchpad for one
//   AI hand-over and is cleared at the start of the next one.
#pragma once

#include "CoreMinimal.h"

// Forward declaration only, exactly as `StratAiTurnRunner.h` does it and for its reason: an
// opaque enum declaration with an explicit underlying type is a complete type, so one may be
// held BY VALUE below without `StratBridge.h` entering any translation unit that includes this
// header. If `EStratAiCommandKind` ever loses its `: uint8`, this line stops compiling rather
// than silently disagreeing.
enum class EStratAiCommandKind : uint8;
struct FStratAiCommand;

/**
 * One thing the AI did, as it was submitted and accepted.
 *
 * A VALUE, COMPLETE IN ITSELF, and copied out of `FStratAiCommand` rather than aliasing it.
 * The two are near-identical today and are still separate types, because the command is an
 * INSTRUCTION with a lifetime of one submission while this is a RECORD with a lifetime of one
 * hand-over -- and because `FStratAiCommand`'s own comment says `TargetId` is "for logging,
 * never for submission", a distinction that stops meaning anything in a struct nobody submits.
 */
struct STRATPLAY_API FStratAiPlaybackStep
{
	/**
	 * Which of §4.10's four kinds.
	 *
	 * INITIALISED BY CAST AND NOT BY ENUMERATOR NAME, which is ugly and is the price of the
	 * forward declaration above: an opaque enum is a complete type but has no visible
	 * enumerators. `Record` overwrites this on every step it writes, so the initialiser is
	 * only ever seen by a default-constructed step that nothing reads. Naming
	 * `EStratAiCommandKind::EndTurn` here would need `StratBridge.h` in this header, which is
	 * the include this module does not do.
	 */
	EStratAiCommandKind Kind = static_cast<EStratAiCommandKind>(0);

	/** The side that acted, as the runner captured it once at turn entry. */
	int32 Side = INDEX_NONE;

	/** §4.10's turn number, live at the moment this command was accepted. */
	int32 Turn = INDEX_NONE;

	/** The acting unit for Move and Attack. `-1` for Build and EndTurn. */
	int32 UnitId = -1;

	/** The defender for Attack, for a caption. `-1` otherwise. Never resubmitted. */
	int32 TargetId = -1;

	/** Build's `defIndex`. `-1` otherwise. Carried so a caption can name the unit type. */
	int32 DefIndex = -1;

	/** Move destination, Build site, Attack target hex. Meaningless unless `bHasHex`. */
	FIntPoint Hex = FIntPoint(0, 0);

	/**
	 * Whether `Hex` names a hex the camera should step to.
	 *
	 * IT EXISTS BECAUSE `FStratAiCommand::Hex` DEFAULTS TO `(0,0)`, WHICH IS A REAL HEX. An
	 * EndTurn carries that default and means nothing by it, so a camera that trusted `Hex`
	 * unconditionally would pan to the board's origin corner once per AI turn -- a motion the
	 * player would read as the AI having done something there. A sentinel hex was the other
	 * shape and was rejected for the same reason `FStratAiCommand` gives about its own
	 * defaults: a value that is legal in the domain cannot signal absence.
	 */
	bool bHasHex = false;
};

/**
 * The recorded list for one hand-over, and the position in it.
 *
 * ONE REEL PER HAND-OVER AND NOT PER TURN. `UStratMatchSubsystem::RunAiTurnsNow` may play
 * several consecutive AI turns in one call -- an AI-vs-AI game plays to a §2.8 result inside
 * one -- and the player watches that whole block as one uninterrupted stretch. `Turn` and
 * `Side` ride on every step, so a caption can still say which turn it is watching.
 *
 * DEFAULT-CONSTRUCTIBLE AND IMMEDIATELY USABLE, on `FStratAiTurnRunner`'s line. An empty reel
 * is not playing, which is the ordinary state.
 */
struct STRATPLAY_API FStratAiPlaybackReel
{
	/** Empties the list and rewinds. Called at the START of a hand-over, never at the end --
	 *  see `SkipToEnd` on why a finished reel keeps its contents. */
	void Reset();

	/** Appends one accepted command. The ONLY writer of `Steps`, and it only ever appends. */
	void Record(const FStratAiCommand& Command, int32 Side, int32 Turn);

	/** How many commands the AI got through this hand-over. */
	int32 Num() const { return Steps.Num(); }

	/** Where the tour has got to. `Num()` means finished. */
	int32 GetCursor() const { return Cursor; }

	/** True while there is a step left to show. */
	bool IsPlaying() const { return Cursor < Steps.Num(); }

	/**
	 * The step at the cursor, or `nullptr` when finished.
	 *
	 * RETURNS A POINTER AND NOT A BOOL-PLUS-OUT-PARAM, unlike `IStratAiTurnPort::NextCommand`
	 * next door, and the difference is not carelessness: that method has a REFUSAL to report
	 * and a default-constructed command that reads as a genuine answer, so it needs the
	 * separate `bool`. This one has neither. There is a step or the tour is over, and a null
	 * pointer cannot be mistaken for a plausible step.
	 *
	 * INVALIDATED BY `Record` AND BY `Reset`, as any pointer into a `TArray` is. Every caller
	 * in this tree uses it and drops it inside one timer callback, and none of them records.
	 */
	const FStratAiPlaybackStep* Peek() const;

	/** Moves past the current step. Returns false when there was nothing to move past. */
	bool Advance();

	/**
	 * Ends the tour now: §2.11.2's "any click or Esc skips to the end state".
	 *
	 * IT MOVES THE CURSOR AND CLEARS NOTHING. `Steps` survives so that "how many actions did
	 * that hand-over take" is still answerable after a skip -- which is the difference between
	 * a skipped tour and one that never had a list, and a clause asserting the skip landed
	 * needs to be able to tell them apart. Returns true only if it was actually playing, so a
	 * caller can tell "this click skipped playback" from "this click arrived at a quiet
	 * moment" and consume the input in exactly the first case.
	 */
	bool SkipToEnd();

private:
	TArray<FStratAiPlaybackStep> Steps;

	/**
	 * How many steps have been shown. In `[0, Steps.Num()]`.
	 *
	 * AN INDEX INTO A FINISHED LIST AND NOT A POSITION IN THE MATCH. Nothing downstream may
	 * read this to decide what the board looks like; the board looks like the view model, on
	 * every refresh, exactly as it did before this file existed.
	 */
	int32 Cursor = 0;
};
