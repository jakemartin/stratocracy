// GDD §2.11.6-B / Q27 (§4.7) — the guided opening's beat machine.
//
// WHAT GAP THIS CLOSES. `FStratSelectionMachine::SetLockedThisTurn` has existed since
// phase 2 with exactly one caller in the tree and that caller is a test. Its own block
// said so, in wording that block has since withdrawn:
// RETRACTED> "THE GUIDANCE LAYER'S WRITER, AND THERE IS NO GUIDANCE LAYER YET … the set is
// RETRACTED>  empty in every shipping path today."
// `StratViewModel.h` said the same of
// `bLockedThisTurn` and `StratPlayerController.h` said it of `GetSelectionMachine`'s
// non-constness. This is that layer. All three of those blocks are retracted in the same
// change, because a header that still says a thing has no caller after you become its
// caller is the defect this project has paid for repeatedly.
//
// IT DECIDES NOTHING ABOUT RULES, and the discipline is stricter here than anywhere else
// in `StratPlay` because the temptation is worse. Every fact this machine branches on is
// a field it read off `FStratViewModel` or an answer `FStratBridge` gave it:
//
//   - which unit is marked  -> `FStratUnitView::bIsGuidedMarked`, which the rules module
//     DECLARES DERIVED off `placement` and not `hex`. Never recomputed here, and never
//     inferred from a hex comparison against `guidedOpening.infantry` — that comparison
//     is exactly the bug the `placement` derivation exists to prevent, because beat 1a's
//     own move would then unmark the unit the beat is about.
//   - whether it has moved  -> `FStratUnitView::bHasMoved`.
//   - the turn              -> `FStratMatchView::Turn`, which is a ROUND (`strat::TurnState`
//     counts `sidesEnded` within one `turnNumber`), so the GDD's "turn 1" is both seats'
//     turn 1 and the schedule table's rows line up with this field one for one.
//   - the capture pip       -> `FStratUnitView::CaptureProgress`.
//   - who holds the ring    -> `FStratHexView::Owner` at the objective hex, which mirrors
//     `UiHexView::owner` and therefore `strat::Objective::owner`. Added 2026-08-23; see
//     `IsRingedObjectiveHeldByGuidedSide` for the defect that made the pip alone
//     insufficient.
//   - the ringed factory    -> `FStratBridge::GuidedOpeningHexes`, a LOOKUP of the
//     authored `guidedOpening.objective`. §2.11.6 forbids the alternative by name:
//     "no 'nearest objective' heuristic is used".
//
// There is no arithmetic in this file over a rules answer, no distance, no reachability
// and no legality test. Where a fact it wanted was not available it takes the broader
// observable and says so in the open — see `HasAnyUnitSpawned`, which is the one place
// that still does. (`HasCapturePipLanded` used to be the other; a user ruling of
// 2026-08-21 narrowed it to the ringed objective and it now joins the list above,
// reading `guidedOpening.objective` through `FStratBridge::GuidedOpeningHexes`.)
//
// WHAT IT DOES OWN, and owning it is the point: which beats are outstanding, which have
// held the line, which holds it now, and the two input gates. None of those is a mirror of
// rules state — the rules module has no notion of a beat — so this is not the "state in an
// actor" shape §4.1 forbids, and it is not held in an actor either: the controller holds it
// by value beside `FStratSelectionMachine`, exactly as that machine is held.
//
// PLAIN C++ AND NOT A `UCLASS`. It needs no reflection: nothing spawns it, nothing finds it
// by class, and its whole visible surface reaches the screen through
// `FStratGuidanceView` on the view model. Making it a `UObject` would give it a lifetime
// separate from the controller's for no caller's benefit — the reasoning
// `FStratSelectionMachine` records for itself, and this file is its sibling in every
// respect. It may therefore be held by value in a `UCLASS` header, which it is.
//
// IT NAMES NO `strat::` TYPE and includes nothing vendored, so this header is safe to
// include from `StratPlayerController.h` even though that file declares a `UCLASS`.
//
// ---------------------------------------------------------------------------
// THE BEAT MODEL, AND HOW IT WAS DERIVED
// ---------------------------------------------------------------------------
//
// §2.11.6-B gives four beats in a fixed four-turn window, two line-assignment rules, a
// yield rule, and four retirement triggers. Modelled here as: each beat is OUTSTANDING
// until its own event retires it; separately, at most one outstanding beat HOLDS THE LINE
// and is the one the strip shows.
//
// RULE 1 — the lowest-numbered outstanding beat that has not yet HELD THE LINE ON AN
// EARLIER TURN. RULE 2 — if every outstanding beat has already had its turn on the line,
// the lowest-numbered outstanding beat: a last call.
//
// THE ONE READING THIS FILE HAD TO CHOOSE, and it is recorded because the two readings
// disagree on a cell of the schedule table. When a beat retires mid-turn the line passes
// immediately to the next beat (§2.11.6: "there is no live-but-blank strip in this system.
// Rule 2 has no exit"). The question is whether that INHERITOR is thereby marked as having
// held the line. It is NOT, here — a beat is marked only when rules 1–2 select it at the
// START of a turn. The deciding text is rule 2's own wording: "if every outstanding beat
// has already had ITS TURN on the line". A beat that picks up the tail of another beat's
// turn has not had its turn; it has had the remainder of someone else's. The alternative
// reading — mark on inheritance too — makes the common case's turn 3 a rule-2 last call
// where the schedule table labels it rule 1. Both readings put the same beat on the strip
// with the same text and the same (absent) tag, so nothing on screen distinguishes them;
// this one additionally reproduces the table's labels, and the labels are the only place
// the difference is visible at all.
//
// THE TAG IS A FACT ABOUT THE WINDOW. §2.11.6: "The tag states a fact about the *window*,
// not about rule 2, so it renders on turn 4 only", and the fast lane's turn-3 rule-2 last
// call is shown UNTAGGED for that reason. So `bShowsWindowEndTag` is `Turn == 4` and is not
// conditioned on the rule. That happens to coincide with "turn 4 is always a rule-2 last
// call", which is provable rather than assumed: beats 1a and 1b always take turn 1 and
// cannot outlive it; beat 2, if outstanding at the start of turn 2, is rule 1's pick there;
// beat 3, if outstanding at the start of turn 3, is rule 1's pick there. So no beat can
// still be un-held at the start of turn 4. The tag is implemented on the window because
// that is what the GDD says it means, not because the coincidence holds.
//
// THE FOUR RETIREMENT TRIGGERS, and where each is read:
//   1a — the marked Infantry's move completes -> `bHasMoved` on the marked unit.
//   1b — the enemy turn ends -> the round advances past the one 1b took the line on.
//   2  — THE RINGED OBJECTIVE IS TAKEN BY THE GUIDED SEAT, on whatever turn. TWO
//        OBSERVABLES, OR'd, because on the shipped scenario only the second one ever
//        becomes visible:
//          (a) `CaptureProgress > 0` on a guided-seat unit standing on
//              `guidedOpening.objective` — `HasCapturePipLanded`; and
//          (b) `FStratHexView::Owner == GuidedSide` at that same hex —
//              `IsRingedObjectiveHeldByGuidedSide`.
//        Narrowed to the ringed objective by user ruling, 2026-08-21, from what this line
//        used to say:
//        RETRACTED> "a capture pip appears, on whatever turn -> `CaptureProgress > 0`."
//        THAT RULING IS INTACT AND IS NOT WHAT CHANGED ON 2026-08-23. What changed is that
//        the pip was measured UNOBSERVABLE at the shipped scenario's `captureTurns = 1`, so
//        arm (a) alone could never fire and beat 2 could never retire. Arm (b) is the fix.
//        `HasCapturePipLanded` carries why §2.11.6 does not settle the SUBJECT on its own;
//        `IsRingedObjectiveHeldByGuidedSide` carries the measurement behind the OR.
//   3  — a unit spawns, on whatever turn including turn 1 -> a friendly unit id this
//        machine has not seen before.
//
// ONE OF THOSE FOUR IS BOOKKEEPING AND IT IS FLAGGED RATHER THAN HIDDEN. Beat 3's "a unit
// spawns" has no snapshot field. `FStratFactoryView::bHasBuiltThisTurn` is the ORDER
// (T-TURN-10's allowance) and `bBuildWaiting` is the queue holding the slot — neither is
// the spawn, and §2.7's build spawns a turn later than the order, so using either would
// retire beat 3 before the unit the §2.11.6-D ledger wants on the board exists. So this
// machine remembers the friendly unit ids it has observed and treats a new one as a spawn.
// That is memory about what THIS OBJECT HAS SEEN, not a second copy of rules state: it
// cannot disagree with the board about who is alive, because it never answers that
// question. A `spawnedThisTurn` field on `UiSnapshot` would replace it and is the fix if
// one ever lands upstream.
//
// ---------------------------------------------------------------------------
// WHAT IS DELIBERATELY NOT IN HERE
// ---------------------------------------------------------------------------
//
// - NO WIDGET, AND NO `/Game/` PATH. The directive strip, and the MESHES AND MATERIALS of
//   the ring and the turn-1a marker, are `strat-editor-builder`'s lane. This file's whole
//   output is `FStratGuidanceView` on the model plus the lock set on the selection machine,
//   which is what that widget will bind to.
//   [AMENDED 2026-08-23: this said "the ring mesh and the turn-1a marker are
//   `strat-editor-builder`'s lane", which became imprecise the day the seams landed. The
//   ring's COMPONENT and its show/clear are `AStratBoardActor::ShowObjective` /
//   `ClearObjective`, and the marker's COMPONENT and its visibility are
//   `AStratUnitActor::GuidedMarker` / `ApplyUnitView` -- both C++, both in this lane. What
//   is still the content lane's is the asset half: the meshes, the material instances, and
//   their assignment on `BP_StratBoardActor` / `BP_StratUnitActor`. NOTHING ABOUT THIS FILE
//   CHANGED -- `FStratGuidedOpening` still draws nothing and still names no component; the
//   sentence was describing a boundary that moved underneath it.]
// - NO §2.11.6-A PRE-MATCH BRIEFING. Three anchored callouts over a dimmed board with a
//   click-through are a widget and a camera state, not a beat; nothing in A is scheduled
//   by rules 1–2 and nothing in B depends on A having run. Its three strings are not
//   declared here because declaring content for a surface that does not exist is how a
//   string drifts from the GDD unnoticed.
// - NO §2.11.6 ONE-SHOT TIPS. `UStratSaveGame::FiredOneShotTipIds` is where they will
//   persist and wave B1 declared it for that reason, but the tips are teacher 3 and fire on
//   concept relevance across the whole match — a different clock from this four-turn
//   window, and folding them in would make this class the owner of two schedules.
// - NO CAP-APPROACH BANNERS. Same reason: §2.11's turn-cap banners are not beats.
// - NO LOCALIZATION. See `FStratGuidanceView`'s block.
#pragma once

#include "CoreMinimal.h"

#include "StratViewModel.h"

class FStratBridge;
// `struct` AND NOT `class`, and the distinction is load-bearing on MSVC:
// `FStratSelectionMachine` is declared `struct` in StratSelectionMachine.h, and mismatching
// the key here was measured as 5 x C4099, "type name first seen using 'class' now seen using
// 'struct'", reported against that header's declaration line rather than against this one.
struct FStratSelectionMachine;

/**
 * §2.11.6-B's four beats, driven by observation of the view model.
 *
 * THE CALL ORDER IS A CONTRACT. `Begin` once per match, then `Observe` on every refresh
 * BEFORE `DecorateViewModel`, and `DecorateViewModel` before `ApplyView`. `Observe` is what
 * advances the machine; `DecorateViewModel` is a const projection and advances nothing, so
 * calling it twice draws the same thing and calling it without an `Observe` draws the
 * previous frame rather than a wrong one.
 */
class STRATPLAY_API FStratGuidedOpening
{
public:
	// ---- The window ---------------------------------------------------------
	// §2.11.6: "four scripted directives inside a fixed four-turn window — the first
	// appears on turn 1, the strip and every beat behind it are gone for good at the end of
	// turn 4". Named because the number appears in three places below and a literal 4 in
	// any of them is a number that can be changed in two.
	static constexpr int32 kFirstGuidedTurn = 1;
	static constexpr int32 kLastGuidedTurn  = 4;

	// ---- The GDD's literal strings -----------------------------------------
	// §2.11.6-B, verbatim, including the full stops. They are here rather than in the .cpp
	// so that a reviewer diffing this file against the GDD sees the whole content surface at
	// once, and `static` free functions rather than members so nothing has to construct a
	// machine to read one. `FText` and not `FString`: see `FStratGuidanceView`'s block on
	// why localization is deliberately not started.
	//
	// ONE OF THE FOUR STRINGS CARRIES A NUMBER THAT LIVES SOMEWHERE ELSE, and it is recorded
	// here rather than left to be discovered. Beat 3 reads "Spend Fame at your Factory.
	// Infantry costs 100." -- and 100 is Infantry's cost in `DT_Units`, duplicated into a
	// display string with nothing pinning the two together. If that row ever changes, the
	// directive lies and no build, gate or clause notices.
	//
	// IT IS STILL CORRECT AS WRITTEN, and the alternative is worse. This is AUTHORED GDD
	// DIRECTIVE TEXT quoted verbatim from §2.11.6-B, not a rules answer this file computed;
	// reading the cost out of `DT_Units` to interpolate it would make the guidance layer a
	// consumer of the definition tables, which is the recomputation the module arrows forbid
	// and the exact shape `HasCapturePipLanded` was just narrowed to avoid. The number is
	// content, and content is allowed to be literal.
	//
	// WHAT DISCHARGES IT: a clause that reads Infantry's `Cost` off the same `DT_Units` the
	// match is configured with and asserts that this string contains it. That is a TEST
	// reading both sides, which is legal where this file reading one side is not -- the
	// asymmetry `StratBridgeParity`'s `GATE-BRIDGE-DEFS` already relies on. Until such a
	// clause exists the coupling is unpinned, and that is the honest state of it.
	static FText DirectiveTextFor(EStratGuidanceBeat Beat);
	static FText WindowEndTagText();      // `guidance ends this turn`
	static FText LockedUnitHoverText();   // `Locked this turn.`
	static FText EndTurnGateHoverText();  // `Move the marked Infantry first.`

	/**
	 * Arms the guided opening for one match, or declines to.
	 *
	 * @param Bridge         the seeded bridge, asked once for `GuidedOpeningHexes`. Not retained.
	 * @param GuidedSide     the seat the guidance is for. §2.11.6 is a first-session
	 *                       onboarding against §2.9's Easy AI, so this is the human's seat
	 *                       and in practice the subsystem's viewing side. It is an argument
	 *                       rather than read off the model because the viewing side MOVES in
	 *                       a hot-seat hand-over and the guided seat does not.
	 * @param bSuppressed    true skips guidance outright — §2.11.6's "any completed match on
	 *                       the save skips all guidance automatically". Passed in rather
	 *                       than read here, because the answer lives in a save slot and this
	 *                       class owns no disk.
	 *
	 * DECLINES QUIETLY WHEN THE SCENARIO NAMES NO GUIDED OPENING. `GuidedOpeningHexes`
	 * refuses, and a scenario without one is an ordinary scenario — `Ui.h` says a missing
	 * `guided` block "marks nobody rather than being an error". The machine ends up inactive
	 * and every gate below answers false, which is the same state `SkipGuidance` produces.
	 */
	void Begin(const FStratBridge& Bridge, int32 GuidedSide, bool bSuppressed);

	/**
	 * One observation of the board. THE ONLY THING THAT ADVANCES A BEAT.
	 *
	 * In order: retire whatever the model says has retired; advance the turn clock and
	 * reassign the line if the round moved; reassign again if the line-holder just retired;
	 * then write the lock set through `Machine`.
	 *
	 * RETIREMENT IS EVALUATED CONTINUOUSLY AND NOT AT THE TURN BOUNDARY, which is what makes
	 * 1a hand off to 1b inside turn 1 — the case `StratViewModel.h` singles out when it says
	 * a lock's "lifecycle is not `bDone`'s … A unit can be un-locked and not-done in the same
	 * turn."
	 *
	 * IT TAKES THE MACHINE NON-CONST BECAUSE IT WRITES THE LOCKS, and that is the whole
	 * reason `AStratPlayerController::GetSelectionMachine` has a non-const overload. The
	 * locks are written here rather than in `DecorateViewModel` because
	 * `FStratSelectionMachine::HandleEvent` enforces them on SELECTION, which happens before
	 * any decoration — a lock published only at draw time would be one event late.
	 */
	void Observe(const FStratViewModel& Model, FStratSelectionMachine& Machine);

	/**
	 * Writes `FStratViewModel::Guidance`. Const, and adds nothing `Observe` did not decide.
	 *
	 * IT DOES NOT WRITE `bLockedThisTurn`. That bit is the selection machine's to publish —
	 * `FStratSelectionMachine::DecorateViewModel` writes it from `LockedUnits`, which this
	 * class filled in `Observe`. Writing it from here too would be a second producer of one
	 * field, which is the drift `StratViewModel.h` opens by refusing.
	 */
	void DecorateViewModel(FStratViewModel& Model) const;

	// ---- The Q27 input gates (§4.7, ruled) ---------------------------------
	// §2.11.6-B, beat 1a: End Turn is inert until the marked Infantry has moved, and that
	// Infantry's two SELECTED -> DONE routes that skip MOVED are closed — its attack targets
	// are not lit, and Space (Wait) is inert for it. "Those are the machine's only two routes
	// from SELECTED to DONE that do not pass through MOVED, so both are closed for that one
	// unit and nothing the player can do leaves End Turn inert with no move left to satisfy
	// it — this is what makes 1a retire inside turn 1 in every branch."
	//
	// THESE ARE PREDICATES AND NOT ENFORCEMENT. `AStratPlayerController::HandleSelectionEvent`
	// is the enforcer, and it enforces by NOT SUBMITTING and NOT ADVANCING THE MACHINE. No
	// line behind these asks `FStratBridge` to refuse a command the rules module would
	// accept — §2.11.1's machine-narrower-than-the-rule note already puts the SELECTED ->
	// attack case on exactly that footing, and this is the same restriction one beat wide.
	//
	// THEY ARE KEYED ON BEAT 1a BEING OUTSTANDING AND NOT ON ITS HOLDING THE LINE. The GDD
	// says "while 1a is outstanding that Infantry cannot retire itself without moving", and
	// 1a hands the line to 1b the instant it retires — so a gate keyed on the line would be
	// open in exactly the window it is meant to close.

	/** End Turn is inert. True only while beat 1a is outstanding. */
	bool IsEndTurnGated() const;

	/**
	 * This unit's Wait and attack routes are closed. True only for the marked Infantry, and
	 * only while beat 1a is outstanding.
	 *
	 * `INDEX_NONE` ANSWERS FALSE, so a caller with nothing selected need not special-case it.
	 */
	bool IsUnitInputGated(int32 UnitId) const;

	/**
	 * §2.11.6's `Skip guidance` control: "kills it instantly for anyone, and kills the guided
	 * opening's board state with it — the objective ring and the turn-1a unit marker clear in
	 * the same frame as the strip."
	 *
	 * IN THE SAME FRAME IS STRUCTURAL HERE AND NOT A PROMISE. The ring, the strip and the
	 * beat all read one `bActive`, and the marker reads `bIsGuidedMarked` on a model the very
	 * next `DecorateViewModel` writes with `bActive` false. There is no second frame in which
	 * one could survive the other.
	 *
	 * IT DOES NOT CLEAR THE LOCKS BY ITSELF. `Observe` does, on the next refresh, from the
	 * same single place that sets them; a second clear here would be a second writer of the
	 * lock set. The controller calls `Observe` immediately after, which is why the frame
	 * holds.
	 *
	 * PERMANENT. §2.11.6 says "gone for good"; there is deliberately no un-skip.
	 */
	void SkipGuidance();

	/** For a reseed. A turn boundary is not this — `FStratSelectionMachine::Reset`'s line. */
	void Reset();

	// ---- Readable state, for the controller and for a clause ---------------

	/** Whether the strip is on screen. False after a skip, a suppression, or the window closing. */
	bool IsActive() const { return bActive; }

	/** Which beat holds the line right now, or `None`. */
	EStratGuidanceBeat BeatOnTheLine() const { return LineBeat; }

	/** Whether a given beat has yet to retire. False for a retired beat and for `None`. */
	bool IsBeatOutstanding(EStratGuidanceBeat Beat) const;

	/**
	 * Which rule put the current beat on the line: 1, 2, or 0 when nothing holds it.
	 *
	 * EXPOSED PURELY SO THE SCHEDULE TABLE CAN BE PINNED. Nothing renders it — §2.11.6
	 * surfaces rule 2 only through the turn-4 tag, which is keyed on the window instead. It
	 * is here because the table's three branches label every cell with a rule, and a clause
	 * that could only check the beat would pass on a model that reproduced the beats by
	 * coincidence.
	 */
	int32 RuleThatSelectedTheLine() const { return LineRule; }

	/** The ringed objective, valid only while `IsActive()`. */
	bool HasObjective() const { return bHasObjective; }
	FIntPoint ObjectiveHex() const { return Objective; }

private:
	/** Rules 1 and 2, run at the start of a turn. Marks the winner as having had its turn. */
	void AssignLineAtTurnStart();

	/** Rule-1/2 selection without marking — the mid-turn inheritance case. See the header block. */
	void InheritLineAfterRetirement();

	/** Retirement, from the model alone. Returns true if anything retired. */
	bool RetireWhatTheModelRetires(const FStratViewModel& Model);

	/** Every friendly unit that is not the marked one, locked while 1a is outstanding. */
	void PublishLocks(const FStratViewModel& Model, FStratSelectionMachine& Machine);

	/** The marked unit of the guided seat in this model, or `INDEX_NONE`. */
	int32 FindMarkedUnitId(const FStratViewModel& Model) const;

	/**
	 * Whether a capture pip is showing for the guided seat ON THE RINGED OBJECTIVE.
	 *
	 * RULED BY THE USER, 2026-08-21, AND NOT DECIDED BY THIS FILE. §2.11.6 is genuinely
	 * ambiguous here and stays ambiguous: the beat table's "Retires when" cell is
	 * unqualified — "A capture pip appears — on whatever turn that happens" — while the
	 * directive it retires reads "Move the Infantry onto the ringed Factory". Two honest
	 * readings, and the document does not choose between them. This code took the
	 * unqualified one when it landed, flagged it as the single open question in the wave,
	 * and the user ruled the other way. IT IS RECORDED AS A RULING RATHER THAN AS A READING
	 * so that the next person to hold §2.11.6 beside this function does not conclude the code
	 * overreached its source — it did not; it was told.
	 *
	 * WHAT THE NARROWING COSTS, because it is not free. Under the old reading beat 2's
	 * retirement depended on nothing but the model. It now depends on
	 * `FStratBridge::GuidedOpeningHexes` returning the right hex, and the failure mode of
	 * that dependency is the bad kind: if that lookup ever regressed into the
	 * nearest-neutral-factory heuristic §2.11.6 forbids by name, beat 2 would retire on the
	 * WRONG TILE rather than fail visibly — a strip that clears on a capture the player was
	 * never directed to make, with nothing on screen to say so.
	 *
	 * WHAT STANDS BETWEEN THIS AND THAT, NAMED: `Stratocracy.StratBridge.T-SCN-07.
	 * GuidedOpeningHexesMatchesTheScenarioFile`, which asserts the returned hexes against
	 * `Data/ferrum_crossing.json`'s own `guidedOpening` block for both seats. That clause is
	 * now load-bearing for a beat and not only for a ring, and deleting it would take this
	 * function's correctness with it.
	 *
	 * IT COMPARES THE UNIT'S HEX AND NOT ITS TARGET, because `FStratUnitView` carries no
	 * notion of what a unit is capturing — `CaptureProgress` is turns-held and the hex is
	 * where the unit stands. §2.11.6 line 771 makes that the right join anyway: the pip is
	 * "the arrival receipt", so a pip on a unit standing on the objective IS the arrival
	 * the directive asked for.
	 *
	 * AMENDED 2026-08-23. The sentence that stood here is retracted:
	 * RETRACTED> "and the ownership flip a turn later is a different event this beat
	 * RETRACTED>  deliberately does not wait for."
	 * It is retracted because on the shipped scenario there IS no "a turn later" — the flip
	 * happens inside the same `captureTick` that creates the pip, and the pip is erased
	 * before any snapshot can be taken. This function is UNCHANGED and still means exactly
	 * what it says; it is simply no longer beat 2's only retirement arm. See
	 * `IsRingedObjectiveHeldByGuidedSide`.
	 */
	bool HasCapturePipLanded(const FStratViewModel& Model) const;

	/**
	 * Whether the ringed objective is held by the guided seat. Beat 2's durable observable.
	 *
	 * WHY THIS EXISTS, MEASURED RATHER THAN ARGUED. `HasCapturePipLanded` cannot fire on the
	 * shipped scenario, and the defect was confirmed by a human-driven PIE session on
	 * 2026-08-23 — the guided Infantry took the ringed objective, the factory became the
	 * player's, and no `Guided beat 2 retired` line was emitted while `1a` and `1b` both
	 * were. Read out of the vendored sources rather than inferred:
	 *
	 *   - `strat::EconomyState::captureTurns` is declared `= 1` and NOTHING sets it. The
	 *     shipped `Data/ferrum_crossing.json` carries no `captureTurns` key and
	 *     `strat::seedFromScenario` names the field nowhere, so the shipped match runs at 1.
	 *   - `strat::captureTick` pushes a `CaptureProgress` with `turnsHeld = 1`, tests
	 *     `turnsHeld >= captureTurns` in the same iteration, flips `Objective::owner`, and
	 *     calls `strat::clearProgress`, which ERASES the entry. All inside one call.
	 *   - `strat::captureTick` holds the only `push_back` into `EconomyState::captures` in
	 *     the whole vendored tree, so no other path can leave one standing.
	 *   - the projection's `progressForUnit` then finds nothing and returns 0, so
	 *     `UiUnitView::captureProgress` — and therefore `FStratUnitView::CaptureProgress` —
	 *     reads 0 in every snapshot that can ever be taken.
	 *
	 * A beat that cannot retire does not fall silent: rule 1 puts it back on the line every
	 * turn, and the player is instructed to capture a factory they already captured. That is
	 * the symptom this closes.
	 *
	 * THE SUBJECT IS UNCHANGED AND THE 2026-08-21 RULING IS UNTOUCHED. This reads the SAME
	 * hex `HasCapturePipLanded` reads — `guidedOpening.objective` through
	 * `FStratBridge::GuidedOpeningHexes` — so `Stratocracy.StratBridge.T-SCN-07.
	 * GuidedOpeningHexesMatchesTheScenarioFile` is load-bearing for this arm exactly as it is
	 * for that one. No "nearest objective" heuristic is introduced and no hex is derived.
	 *
	 * IT IS A READ AND NOT A DERIVATION. `FStratHexView::Owner` mirrors `UiHexView::owner`,
	 * which `strat::buildUiSnapshot` mirrors off `Objective::owner`, and the parity clause
	 * `Stratocracy.StratUI.T-INT-05.MirrorsSnapshotFieldForField` already pins that mirror
	 * (its `.Owner == UiHexView::owner` assertion). There is no arithmetic here,
	 * no distance and no legality test — one equality against a field the rules module
	 * published.
	 *
	 * IT IS A STATE AND NOT AN EVENT, AND THAT IS DELIBERATE. A scenario whose ringed
	 * objective is ALREADY the guided seat's at seed retires beat 2 on the first observation
	 * rather than never. That is the correct answer, not a leniency: `strat::captureTick`
	 * short-circuits on `o.owner == side`, so on such a scenario the directive "Move the
	 * Infantry onto the ringed Factory" is already satisfied and no pip and no flip could
	 * ever follow. Retiring at once is what stops rule 1 re-issuing an impossible
	 * instruction — the same defect this function was written for. Measured on the shipped
	 * data: both seats' `guidedOpening.objective` hexes appear in `ownership` with
	 * `owner: -1`, so nothing retires early on Ferrum Crossing.
	 *
	 * WHAT WOULD MAKE THIS ARM WRONG: a rules change that let a side hold an objective it
	 * did not take, or a §2.11.6 rewrite that made beat 2 about the ARRIVAL specifically
	 * rather than about the objective becoming yours. Neither exists today.
	 */
	bool IsRingedObjectiveHeldByGuidedSide(const FStratViewModel& Model) const;

	/** True once a friendly unit id appears that this object has not seen. See the header block. */
	bool HasAnyUnitSpawned(const FStratViewModel& Model) const;

	bool bActive     = false;
	bool bSuppressed = false;
	int32 GuidedSide = 0;

	bool      bHasObjective = false;
	FIntPoint Objective     = FIntPoint::ZeroValue;

	/** Outstanding until retired. Index by `EStratGuidanceBeat` minus one; `None` has no slot. */
	bool bOutstanding[4] = { true, true, true, true };

	/**
	 * The turn on which each beat was selected AT A TURN START, or `INDEX_NONE`.
	 *
	 * `INDEX_NONE` IS "HAS NOT HAD ITS TURN ON THE LINE", which is rule 1's test and rule 2's
	 * exhaustion condition. A beat that inherited the line mid-turn keeps `INDEX_NONE` — see
	 * the header block on the one reading this file had to choose.
	 */
	int32 TurnHeldTheLine[4] = { INDEX_NONE, INDEX_NONE, INDEX_NONE, INDEX_NONE };

	EStratGuidanceBeat LineBeat = EStratGuidanceBeat::None;
	int32              LineRule = 0;

	/** The last round observed, so a change is a turn boundary. `INDEX_NONE` before the first. */
	int32 LastObservedTurn = INDEX_NONE;

	/** The round beat 1b took the line on, so "the enemy turn ends" is the next round. */
	int32 Turn1bTookTheLine = INDEX_NONE;

	/**
	 * Friendly unit ids seen so far. Beat 3's spawn detector, and nothing else reads it.
	 *
	 * IT IS NOT A UNIT LIST. It is never asked who is alive, it is only ever asked whether an
	 * id is new, and a dead unit's id staying in it is correct rather than stale — a rebuilt
	 * id would not be a spawn the player paid for twice.
	 */
	TSet<int32> SeenFriendlyUnitIds;

	/**
	 * The marked unit id as of the last `Observe`, or `INDEX_NONE`.
	 *
	 * A CACHE, AND IT IS ONE ON PURPOSE RATHER THAN BY OVERSIGHT — so it is written down.
	 * `IsUnitInputGated` is asked by the controller in the middle of handling a click, where
	 * there is a model in hand but the gate must give the same answer the last refresh
	 * published; taking a model parameter would let the caller pass a DIFFERENT model and get
	 * a gate that disagrees with the dimming the player is looking at. The staleness window
	 * is one refresh, and the controller's own contract closes it: it calls `Observe` with a
	 * freshly built model before it consults any gate.
	 *
	 * IT IS NOT A SECOND COPY OF A RULES ANSWER. `bIsGuidedMarked` is derived off
	 * `placement`, so the mark does not change during a match at all — the cache can go stale
	 * only across a reseed, and `Reset` clears it.
	 */
	int32 LastKnownMarkedUnitId = INDEX_NONE;
};
