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
// [AMENDED 2026-09-02 BY THE EXISTENCE HOLD. THE REJECTION ABOVE IS NOT OVERTURNED AND THE
// PRECISION MATTERS, SO THIS IS WRITTEN AS AN AMENDMENT AND NOT AS A RETRACTION.] The per-step
// board was rejected because DRAWING one requires A MID-TURN COPY OF THE RULES STATE to
// reconcile against, and that copy is the mirror two headers forbid. **Every word of that
// stands. What now rewinds is EXISTENCE, and it needs no such copy** -- it is driven from a
// per-step list of unit IDS RECORDED AT THE INSTANT OF ACCEPTANCE, which is what this file's
// own opening already declares the reel to be: "a list of things that have ALREADY HAPPENED".
// See `FStratAiPlaybackStep::AppearedUnitIds`. **POSITIONS STILL DO NOT REWIND**: a retained
// corpse stands where the last reconcile put it and a revealed unit appears at its final hex,
// because no mid-turn hex is recorded and none may be. The line is exactly where it was -- a
// recording of what happened may drive presentation; a reconstruction of the board may not.
//
// A CONSEQUENCE OF THAT WORTH SAYING OUT LOUD: "any click or Esc skips to the end state" is
// SATISFIED TRIVIALLY HERE, because the end state was never absent. Skipping abandons the tour;
// it does not fast-forward a simulation. A clause under `T-TURN-09` that asserts the three
// dispositions (never played, watched to the end, skipped part-way) reach an identical state is
// therefore cheap BY CONSTRUCTION rather than by careful bookkeeping, and that is the property
// worth pinning: this file holds no `FStratBridge`, submits nothing, and has no route to.
//
// [WEAKENED 2026-09-02 BY THE EXISTENCE HOLD, AND GENUINELY WEAKENED RATHER THAN RE-EXPLAINED.
// THE PARAGRAPH ABOVE IS LEFT STANDING BECAUSE ITS LAST SENTENCE IS STILL TRUE AND IS STILL THE
// REASON THE PROPERTY IS CHEAP.] **"SATISFIED TRIVIALLY, BECAUSE THE END STATE WAS NEVER ABSENT"
// IS NO LONGER TRUE.** With the hold in force the end state IS absent while a tour runs: units
// killed during the hand-over are still standing and units built during it are hidden. A skip
// now ACTIVELY DESTROYS the retained actors and ACTIVELY UNHIDES the concealed ones -- see
// `UStratMatchSubsystem::EndAiPlaybackTour`, which does both -- so the three dispositions reach
// an identical state because a verb MAKES them, not because nothing ever diverged.
// **WHAT IS STILL TRUE, AND IT IS WHY THIS IS SURVIVABLE:** none of it touches the RULES state.
// This file still holds no `FStratBridge` and submits nothing; the divergence and its repair are
// both entirely in the presentation layer, which is why `T-TURN-09`'s state-equality half is
// unaffected while its screen-equality half now rests on a function rather than on an absence.
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

	/**
	 * The ordered hexes an accepted Move actually walked. `[0]` is the from-hex and `.Last()`
	 * equals `Hex`. EMPTY on every other kind, and empty when the path query refused.
	 *
	 * A ROUTE AND NOT A FROM-HEX, WHICH IS THE WHOLE DECISION, and both reasons are already
	 * recorded elsewhere in this tree rather than invented here.
	 *
	 * FIRST, A FROM-HEX RECONSTRUCTS A CHORD AND NOTHING ELSE. `StratUnitActor.cpp` records,
	 * at the arming condition in `AStratUnitActor::ApplyUnitView`, that the degenerate
	 * two-point fallback was DELETED rather than kept -- "a chord is the artefact this whole
	 * change exists to remove, and keeping it as a fallback would have made the commonest AI
	 * move slide along the very line the player was told units do not take." A field that can
	 * only ever produce that chord would hand the deleted artefact straight back through a
	 * different door.
	 *
	 * SECOND, A FROM-HEX WOULD NEED A COMPANION `bHasFromHex`, FOR `bHasHex`'S OWN RECORDED
	 * REASON, WHICH IS STATED ON `bHasHex` ITSELF, JUST ABOVE: `(0,0)` is a real hex, and a
	 * value that is legal in the domain cannot signal absence. AN EMPTY `TArray` SIGNALS ITS OWN
	 * ABSENCE -- there is no legal one-element or zero-element route to be confused with a
	 * default, so this field needs no companion flag and cannot acquire the drift of one.
	 *
	 * `FStratAiPlaybackReel::Record` COPIES IT VERBATIM AND MAKES NO SECOND JUDGEMENT ABOUT
	 * IT. In particular there is deliberately no `RouteHexes.Last() == Hex` check in `Record`:
	 * `UStratMatchSubsystem::ApplyView` already performs exactly that equality on the player's
	 * route before it converts one, and one checking rule living in one place and being reused
	 * is the property. A second copy of the check here would be a second thing to keep in step
	 * -- the same argument `StratAiTurnRunner.h` makes about `StratLogAiTurnRefusal`, where a
	 * second spelling of one rule existed for the length of one diff and had already drifted.
	 * THE CONSUMER CHECKS, THE RECORD RECORDS.
	 *
	 * [AMENDED 2026-09-02, SAME DAY, WHEN THIS FIELD ACQUIRED ITS FIRST READER. WRITTEN FLAT
	 * BECAUSE "THE CONSUMER CHECKS" NAMES A SINGULAR AND THERE ARE NOW TWO.] The reader is
	 * `UStratMatchSubsystem::PlayMoveSlideForStep`, added when the user reversed the decision
	 * that AI moves must not slide. **IT DOES NOT PERFORM THAT EQUALITY, AND THE REASON IS THAT
	 * THERE IS NO WINDOW FOR IT TO CATCH ANYTHING.** `ApplyView`'s check exists because a
	 * PLAYER's route is noted before a submit and read after the AI has replied, so the board
	 * can have moved underneath it. This field is filled from the very command whose `Hex` it
	 * would be compared against, in the same call, with nothing in between -- so the equality
	 * holds by construction and a runtime test of it would assert a tautology. What the reader
	 * checks instead is what it actually depends on: at least two hexes, a board, a live actor,
	 * and an entry in the applied model. THE RECORD STILL RECORDS AND MAKES NO JUDGEMENT.
	 */
	TArray<FIntPoint> RouteHexes;

	/**
	 * Unit ids that APPEARED on the roster when this command was accepted, and ids that
	 * DEPARTED it. Both empty on every command that changed neither.
	 *
	 * WHAT GAP THESE CLOSE. A tour steps over a board that is already FINAL, so a unit built
	 * during the hand-over is on screen from the moment it resolves and a unit killed during it
	 * is gone from that same moment -- both visibly out of step with the tour that is about to
	 * show them being built and killed. `UStratMatchSubsystem` holds the existence of those
	 * units against the cursor, and these two fields are the only record of WHICH ids and AT
	 * WHICH STEP.
	 *
	 * A ROSTER DELTA AND NOT A COMBAT OUTCOME OR A BUILD OUTCOME, WHICH IS THE DECISION AND IS
	 * FORCED BY TWO MEASUREMENTS RATHER THAN CHOSEN FOR ELEGANCE. (1) A Build command spawns
	 * ZERO OR MORE units, not one: `applyCommand`'s Build arm calls `resolveBuilds` and pushes
	 * every spawn it returns, so a boxed-in factory spawns nothing and a previously blocked
	 * build can spawn on a LATER command. (2) `TargetId` is not a death identity: the Attack arm
	 * erases the DEFENDER when `defHpAfter <= 0` and otherwise erases the ATTACKER when the
	 * counter kills it, so a death can be either unit -- and 61 of 68 attacks in a measured
	 * hand-over killed nobody at all. A per-kind field would need a different shape for each
	 * arm and would be wrong in both; "which ids appeared, which departed" is one question with
	 * one answer, needs no `Kind` switch, and cannot drift the way two parallel mechanisms would.
	 *
	 * SETS, NOT SCALARS, EVEN THOUGH THE DEPARTED SET IS PROVABLY 0-OR-1. `applyCommand`
	 * contains exactly two unit erasures, both in the Attack arm, and they are MUTUALLY
	 * EXCLUSIVE -- the defender-death branch returns before the counter block. So a departure
	 * count above one is not reachable today. It is a `TArray` anyway because the bound is a
	 * fact about the vendored rules and this field would be the last thing to be corrected on
	 * the day it changed; a set costs nothing and cannot be wrong.
	 *
	 * `FStratAiPlaybackReel::Record` COPIES BOTH VERBATIM AND JUDGES NEITHER, on `RouteHexes`'
	 * own discipline exactly: no de-duplication, no ordering promise beyond the order the
	 * observation produced, and no cross-check against `Kind`. THE CONSUMER CHECKS, THE RECORD
	 * RECORDS.
	 */
	TArray<int32> AppearedUnitIds;

	/** See `AppearedUnitIds`. Ids that left the roster when this command was accepted. */
	TArray<int32> DepartedUnitIds;
};

/**
 * What ONE accepted AI command did to the board, as an out-param carried from the port to the
 * reel.
 *
 * WHY A STRUCT AND NOT MORE OUT-PARAMS, AND IT IS THE SECOND WIDENING IN ONE DAY THAT DECIDED
 * IT. `IStratAiTurnPort::Submit` grew `TArray<FIntPoint>& OutMoveRoute` on 2026-09-02 and broke
 * three test doubles; the roster delta would have made it five parameters and broken the same
 * three again. **The doubles are broken ONCE MORE AND THEN NEVER AGAIN BY THIS AXIS** -- a
 * fourth fact about an accepted command is a field here and costs no implementation anything.
 * That is the whole of the trade and it is why the churn is being paid now rather than deferred.
 *
 * IT IS A TRANSPORT AND NOT A RECORD. `FStratAiPlaybackStep` keeps its own three flat fields;
 * this struct is what carries them one hop, from the port that observed them to the reel that
 * stores them. Nothing holds one of these.
 *
 * `Reset()` IS PART OF THE CONTRACT AND IS CALLED BY THE IMPLEMENTATION, not by the caller --
 * see `IStratAiTurnPort::Submit`, which states that obligation for all three members together
 * rather than once per member.
 */
struct STRATPLAY_API FStratAiCommandEffect
{
	/** The ordered hexes an accepted Move walked. See `FStratAiPlaybackStep::RouteHexes`. */
	TArray<FIntPoint> MoveRoute;

	/** See `FStratAiPlaybackStep::AppearedUnitIds`. */
	TArray<int32> AppearedUnitIds;

	/** See `FStratAiPlaybackStep::DepartedUnitIds`. */
	TArray<int32> DepartedUnitIds;

	/** Empties all three. ONE VERB, so an implementation cannot clear two of the three -- which
	 *  is exactly the failure the single-parameter shape made impossible and a five-parameter
	 *  shape would have reintroduced. */
	void Reset()
	{
		MoveRoute.Reset();
		AppearedUnitIds.Reset();
		DepartedUnitIds.Reset();
	}
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

	/**
	 * Appends one accepted command. The ONLY writer of `Steps`, and it only ever appends.
	 *
	 * @param Effect     what the accepted command did -- the Move route and the roster delta.
	 *                   [WIDENED 2026-09-02 FROM `const TArray<FIntPoint>& MoveRoute` TO THE
	 *                   WHOLE EFFECT. The paragraphs below were written about the route and
	 *                   bind ALL THREE MEMBERS unchanged; `FStratAiCommandEffect` records why
	 *                   one struct replaced a growing parameter list.] COPIED VERBATIM
	 *                   INTO `FStratAiPlaybackStep::RouteHexes` AND NOT INSPECTED -- see that
	 *                   field on why the `.Last() == Hex` equality belongs at the consumer and
	 *                   is deliberately not repeated here.
	 *
	 *                   REQUIRED AND NOT DEFAULTED, WHICH IS A CHOICE AND NOT AN OVERSIGHT.
	 *                   A default of `{}` would compile every existing and every future call
	 *                   site that forgot the route, and "forgot the route" is
	 *                   INDISTINGUISHABLE FROM "this kind has no route" once it is in the
	 *                   reel -- both are an empty array and both make the unit snap. There is
	 *                   exactly one caller in this tree (`FStratAiTurnRunner::RunTurn`), so
	 *                   the default buys nothing today and costs a silent failure mode
	 *                   tomorrow. A caller with no route passes an empty array and says so.
	 */
	void Record(const FStratAiCommand& Command, int32 Side, int32 Turn,
	            const FStratAiCommandEffect& Effect);

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

	/**
	 * Any step by index, or `nullptr` out of range. THE CURSOR IS NOT CONSULTED AND NOT MOVED.
	 *
	 * WHAT GAP THIS CLOSES, ADDED 2026-09-02. `Peek` answers about the cursor, which is the
	 * right question for stepping and the wrong one for a reader that must see the WHOLE reel
	 * before the tour starts. `UStratMatchSubsystem::PreParkPicturesForTour` is that reader: it
	 * finds each unit's FIRST recorded Move so a picture can be put back where that move began,
	 * and "first" is a question about the list and not about where a camera has got to.
	 *
	 * READ-ONLY AND CURSOR-BLIND ON PURPOSE. A second way to MOVE the cursor would be a second
	 * driver of the reel, which is exactly what `UStratMatchSubsystem::AdvanceAiPlaybackOneStep`
	 * was extracted to prevent; this cannot advance anything and a caller that wants to step
	 * still has exactly one way to.
	 *
	 * INVALIDATED BY `Record` AND BY `Reset`, on `Peek`'s own terms and for its reason.
	 */
	const FStratAiPlaybackStep* StepAt(int32 Index) const;

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
