// GDD §2.11.1 / §4.9 / T-INT-05 / T-UI-02 -- the selection state machine, and the producer
// of `bDone` and `bLockedThisTurn`.
//
// WHAT GAP THIS CLOSES, and it has been owed since phase 2 in as many words.
// `StratViewModel.h` declares `FStratUnitView::bDone` and `bLockedThisTurn`, leaves both
// false, and records the debt on the fields themselves: "when phase 3's selection machine
// lands, it writes these two bits onto the built model and does NOT keep its own copy in an
// actor -- the moment it does, T-INT-05 is false and no compiler says so." Nothing produced
// them until this file. `T-INT-05.NoActorHoldsPresentationBits` pins the negative -- no
// actor holds them -- and could pin nothing else, because there was no positive obligation
// to pin. `DecorateViewModel` below is the positive one.
//
// IT IS A PLAIN STRUCT AND NOT A UCLASS, AND NOT ON AN ACTOR. Two independent reasons, and
// either alone would be sufficient:
//   1. TESTABILITY. It is drivable with no actor, no PIE, no world and no Slate in
//      existence -- the same shape `StratBuildViewModel` was deliberately given, for the
//      same stated reason ("it can be asserted against a bridge with no actor and no Slate
//      in existence"). A `UCLASS` would need a `UObject` outer and therefore a world.
//   2. THE T-INT-05 OBLIGATION ABOVE. A reflected member on an actor is exactly the copy
//      the view model's header forbids. Being unable to be an actor's `UPROPERTY` is a
//      property worth having.
//
// THE AUTHORITATIVE HALF OF ITS STATE IS NOT ITS OWN, and this is the load-bearing design
// call in the file. It holds a selected unit id and two id sets, and NOTHING ELSE. Whether
// the selected unit has already moved or acted is read from `FStratViewModel` -- from
// `bHasMoved` and `bHasActed`, which are the rules module's own turn flags -- and never
// cached here. So a rejected command cannot desynchronise the machine: there is no mirror
// of the rules state to go stale, because the mirror was never made. The alternative shape
// -- an explicit `Moved` / `Acted` state advanced optimistically on submission and rolled
// back on refusal -- was rejected because the rollback path is the one no test ever
// exercises and the one a rules change silently invalidates.
//
// WHAT IT MAY NOT DERIVE, stated because the temptation is one line away. `bDone` is NOT
// `bHasMoved && bHasActed`. `Ui.h` says so and names the cases: Wait and RMB-in-MOVED both
// reach DONE without spending the act flag. This machine therefore SETS `bDone` at the two
// moments §2.11.1 says it becomes true -- a Wait, and an accepted Attack -- and never
// computes it from a flag pair. Anything that reintroduces the derivation is wrong even
// while it agrees.
//
// EVERY RULES ANSWER COMES THROUGH `IStratRulesQuery`, AND IT COMPUTES NONE OF THEM. There
// is no hex arithmetic in this file at all -- no distance, no neighbour, no range -- and
// that absence is the point. `FStratBridge::ReachableHexes` is the movement set;
// `AttackTargetHexes` is the attack set. T-UI-02 measured 122 divergent hexes across 10 of
// 10 units between the real reachability query and `distance <= move`, from three separate
// causes (occupancy, impassable Water, weighted Woods/Mountains), so a filter that looked
// right on an empty plain would be wrong on most of Ferrum Crossing.
//
// WHY THE QUERY IS AN INTERFACE RATHER THAN AN `FStratBridge*`. So that this struct can be
// driven in a test against a scripted set without a seeded bridge, AND so that the
// production adapter -- `FStratBridgeRulesQuery`, below -- is a named, separately assertable
// thing rather than an implicit call site. It is deliberately NOT a hook for a "cheap"
// alternative implementation: the only production implementation is the bridge-backed one
// and a second one in shipping code would be the substitution T-UI-02 exists to catch.
//
// THE THREE ENTRY POINTS ARE SPLIT ALONG WHAT CAN FAIL. `HandleEvent` decides and mutates
// only what acceptance cannot change (which unit is selected); `NotifyCommandApplied`
// mutates what only an ACCEPTED command may change (`bDone`, deselection, the turn reset);
// `BuildOverlays` is const and derives the two highlight sets from the model and the query.
// A single fused entry point would have had to either submit commands itself -- putting a
// bridge inside the state machine -- or advance its state before knowing whether the rules
// module agreed.
//
// IT NAMES NO `strat::` TYPE. Hexes are `FIntPoint`, X = q and Y = r, straight out of
// `FStratViewModel`. `FStratBridge` is forward declared here and included only from the
// .cpp -- the module-wide discipline, and it costs nothing here because this header
// declares no reflected type either, so UHT does not parse it at all.
//
// NOT IN THIS ROUND, with reasons:
// - CAPTURE. `FStratBridge::SubmitCapture` exists and this machine never emits it. The
//   phase list is literally "click-to-select -> move -> attack, wait, end turn", and a
//   capture affordance is a §2.7 interaction with its own affordance question (which hex
//   offers it, and how the player is told) that no phase has answered. Recorded as a
//   deferral rather than guessed at; the applier's `switch` will need one new arm.
// - BUILD, AND THIS BULLET IS RETRACTED IN PLACE. It used to read:
//   RETRACTED> "BUILD. §2.11.5's production menu is explicitly out of the hot-seat
//   RETRACTED>  milestone."
//   The menu exists and has an affordance as of 2026-08-22, and NOTHING IN THIS STRUCT
//   CHANGED FOR IT -- which is the substance of the retraction rather than an aside. The
//   user ruled that §2.11.5's menu opens on a DEDICATED INPUT ACTION
//   (`AStratPlayerController::OpenProductionMenuAction`), specifically so that
//   `EStratSelectionEvent::HexPrimary` keeps meaning select / move / attack and this machine
//   needs no BUILD arm. There is still no `EStratSelectionCommand::Build`, this machine
//   still never emits one, and `StratSubmitSelectionCommand`'s `switch` gained no arm:
//   `UStratMatchSubsystem::SubmitProductionChoice` is the submit path and it does not pass
//   through here at all.
//   THE ONE THING A READER MUST NOT CONCLUDE from the menu existing is that this machine now
//   knows about hexes that hold no unit. It does not. `SelectedUnitId` is still the whole of
//   its selection state, a click on empty ground is still "an ordinary click and not a
//   failure", and the production menu's hex therefore comes from
//   `AStratPlayerController::HexUnderCursor` and never from here -- recorded in that class's
//   header block, because the ruling was phrased "the currently selected hex" and this
//   project has no such thing.
// - MOVE-UNDO. Out of the milestone. Note that this machine could not offer one anyway:
//   the rules module applies a command directly and `FStratBridge::Submit` records it, so
//   an undo is a rules-module question and not a selection-machine one.
// - ANY LOGGING. `STRAT-CMD accepted` belongs to the applier below, which is where a
//   command is actually accepted. A machine that logged its intent would produce a line
//   for a command the rules module then refused.
#pragma once

#include "CoreMinimal.h"
#include "Containers/Set.h"

#include "StratViewModel.h"

// Forward declaration only, per the header block.
class FStratBridge;

/**
 * What the player did. NOT what should happen -- that is `FStratSelectionOutcome`.
 *
 * A PLAIN `enum class` AND NOT A `UENUM`, like everything else in this file. Enhanced
 * Input's actions map onto these in `AStratPlayerController`; nothing in a Blueprint needs
 * to name one, and making it reflected would put this header in front of UHT for no gain.
 */
enum class EStratSelectionEvent : uint8
{
	/** Primary click on a board hex. Select, move, or attack, depending on the state. */
	HexPrimary,

	/** Secondary click, or Escape. Drops the selection; never submits anything. */
	Cancel,

	/**
	 * §2.11.1's wait. Spends the selected unit's turn WITHOUT a rules command.
	 *
	 * THERE IS NO `Wait` COMMAND AND THERE CANNOT BE ONE. `strat::SaveCommandKind` is
	 * `{Move, Attack, Build, Capture, EndTurn}` (`strat::SaveCommandKind`) and phase 1 recorded the
	 * correction explicitly: "`Wait` is a UI-level concept the save format cannot carry."
	 * So this event produces `EStratSelectionCommand::None` and sets `bDone`, which is the
	 * clearest single demonstration of why `bDone` is this machine's and not the rules
	 * module's.
	 */
	Wait,

	/** End the active side's turn. The one event that is valid with nothing selected. */
	EndTurn
};

/**
 * What the caller should submit, if anything.
 *
 * `None` IS AN ORDINARY VALUE AND NOT A FAILURE. A Wait, a selection, a cancel and a click
 * on empty ground all produce it, and only `FStratSelectionOutcome::FailureReason`
 * distinguishes "nothing was asked for" from "something was asked for and refused".
 */
enum class EStratSelectionCommand : uint8
{
	None,
	Move,
	Attack,
	EndTurn
};

/**
 * The rules answers this machine needs, and no others.
 *
 * TWO METHODS, BOTH SET-VALUED, BOTH THE MODULE'S. There is deliberately no
 * `IsLegalMove(unit, hex)` here: the machine decides legality by membership in the reach
 * set it was given, so there is exactly one place a movement rule can enter the engine and
 * it is `FStratBridge::ReachableHexes`. A per-hex predicate would be a second entry point
 * that a later "fast path" could answer locally.
 */
struct STRATPLAY_API IStratRulesQuery
{
	virtual ~IStratRulesQuery() = default;

	/** §2.5's reachable set for one unit, hex for hex. Never a distance filter. */
	virtual bool ReachableHexes(int32 UnitId, TArray<FIntPoint>& OutHexes,
	                            FString& OutFailureReason) const = 0;

	/** §2.6's legal attack targets for one unit. Empty is an answer here. */
	virtual bool AttackTargetHexes(int32 UnitId, TArray<FIntPoint>& OutHexes,
	                               FString& OutFailureReason) const = 0;
};

/**
 * The one production implementation: both sets, straight off `FStratBridge`.
 *
 * A NAMED TYPE RATHER THAN A LAMBDA AT THE CALL SITE, so that a gate can construct one
 * against a seeded bridge and assert that what it hands back IS `FStratBridge::Reachable`'s
 * answer -- which is the T-UI-02 clause that matters, and which is unassertable if the
 * forwarding only exists inside a controller method.
 *
 * BORROWS AND NEVER OWNS. It holds a raw `const FStratBridge*` for the same reason
 * `AStratScoreboardHUD` holds one after adoption: the bridge is the match subsystem's and
 * outliving it is the caller's problem to avoid, not this object's to solve with a copy of
 * a `strat::GameState`.
 */
struct STRATPLAY_API FStratBridgeRulesQuery final : public IStratRulesQuery
{
	explicit FStratBridgeRulesQuery(const FStratBridge* InBridge)
		: Bridge(InBridge)
	{
	}

	virtual bool ReachableHexes(int32 UnitId, TArray<FIntPoint>& OutHexes,
	                            FString& OutFailureReason) const override;

	virtual bool AttackTargetHexes(int32 UnitId, TArray<FIntPoint>& OutHexes,
	                               FString& OutFailureReason) const override;

private:
	const FStratBridge* Bridge = nullptr;
};

/**
 * What one input event came to. A value, complete in itself.
 *
 * IT CARRIES THE COMMAND AND NOT THE RESULT OF SUBMITTING IT. The machine does not hold a
 * bridge and cannot know whether the rules module will accept -- see
 * `StratSubmitSelectionCommand`, which is where acceptance happens and where the
 * `STRAT-CMD accepted` line is emitted.
 */
struct STRATPLAY_API FStratSelectionOutcome
{
	/** What to submit. `None` means nothing is to be submitted. */
	EStratSelectionCommand Command = EStratSelectionCommand::None;

	/** The unit the command is about. `INDEX_NONE` for `EndTurn` and for `None`. */
	int32 UnitId = INDEX_NONE;

	/**
	 * The destination or target hex, X = q and Y = r.
	 *
	 * MEANINGLESS UNLESS `Command` IS `Move` OR `Attack`, and left at (0,0) otherwise --
	 * which is a real hex on this board, so a consumer must switch on `Command` and never
	 * sniff this field.
	 */
	FIntPoint Hex = FIntPoint::ZeroValue;

	/** Which unit is selected AFTER the event, or `INDEX_NONE`. A report, not an input. */
	int32 SelectedUnitId = INDEX_NONE;

	/**
	 * Why nothing happened, when the player did something that could not be honoured --
	 * clicking an enemy that is out of range, selecting a unit that has already finished,
	 * ending a turn on the wrong side's screen. Empty otherwise.
	 *
	 * A SENTENCE FOR A HUMAN, NOT A CHANNEL. It exists so a log line or a later toast can
	 * say what was wrong; nothing branches on its content.
	 */
	FString FailureReason;
};

/**
 * §2.11.1's selection state, and the owner of the presentation block.
 *
 * DEFAULT-CONSTRUCTIBLE AND IMMEDIATELY USABLE. No `Initialize`, no bridge, no world.
 */
struct STRATPLAY_API FStratSelectionMachine
{
	// ---- Input -----------------------------------------------------------

	/**
	 * Decides what one input event means. THE ONLY DECISION POINT.
	 *
	 * MUTATES SELECTION AND NOTHING ELSE. Selecting and cancelling are complete the moment
	 * they happen -- no rules module is consulted and nothing can refuse them -- so they
	 * take effect here. Everything an acceptance could change is `NotifyCommandApplied`'s.
	 * The split is the reason this struct cannot go out of sync with the rules module: the
	 * state it advances eagerly is state the rules module has no opinion about.
	 *
	 * THE ACTIVE SIDE IS READ FROM THE MODEL (`Model.Match.SideToMove`) AND NOT FROM
	 * `ViewingSide`. In hot-seat the two differ every other turn, and a machine that let
	 * the viewing side authorise a command would let a player move on their opponent's turn
	 * by pressing the hand-over key. `StratScoreboardHUD.h` and `StratMatchSubsystem.h`
	 * both record the same distinction; this is the place where confusing them would be a
	 * rules violation rather than a cosmetic one.
	 *
	 * @param Event  what the player did.
	 * @param Hex    the clicked hex, X = q and Y = r. Ignored for every event but
	 *               `HexPrimary`.
	 * @param Model  the current view model. READ ONLY -- `DecorateViewModel` is where this
	 *               struct writes to a model, and it is a different call on purpose.
	 * @param Query  the rules answers. Consulted for `HexPrimary` only.
	 */
	FStratSelectionOutcome HandleEvent(EStratSelectionEvent    Event,
	                                   FIntPoint               Hex,
	                                   const FStratViewModel&  Model,
	                                   const IStratRulesQuery& Query);

	/**
	 * Records that a command this machine asked for was ACCEPTED by the rules module.
	 *
	 * CALL IT ONLY ON ACCEPTANCE. On a refusal, call nothing: the machine's state is
	 * already consistent with a world in which the command did not happen, which is the
	 * whole benefit of not mirroring the rules state.
	 *
	 * WHAT EACH ACCEPTANCE MEANS:
	 *   `Move`    -- the unit stays selected. §2.11.1's move-then-attack needs it, and the
	 *                overlays rebuild around it because `bHasMoved` is now true in the
	 *                model.
	 *   `Attack`  -- the unit is DONE and deselected. §2.11.1: an attack ends the unit's
	 *                turn.
	 *   `EndTurn` -- the done set is cleared and the selection dropped. The done set is a
	 *                per-turn fact and carrying it across a turn boundary would grey out a
	 *                unit that has a fresh turn.
	 *   `None`    -- nothing; there was no command to accept.
	 *
	 * @param Outcome  the outcome `HandleEvent` returned, unmodified.
	 */
	void NotifyCommandApplied(const FStratSelectionOutcome& Outcome);

	// ---- Output ----------------------------------------------------------

	/**
	 * Writes `bDone` and `bLockedThisTurn` onto the built model. THE SEAM PHASE 2 ASKED
	 * FOR, and the reason this file exists.
	 *
	 * `UStratMatchSubsystem` splits `BuildViewModel` from `ApplyView` specifically to leave
	 * room for this call between them -- its header says so: "the phase-4 path is build ->
	 * decorate -> `ApplyView`, with the decorated model being the one and only description
	 * of the screen." So the presentation bits reach the screen by being ON THE MODEL that
	 * is reconciled, and no actor is ever told about them directly. `ApplyView` never reads
	 * them and does not need to; whatever draws a DONE unit reads the model like everything
	 * else does.
	 *
	 * IT WRITES ONLY THOSE TWO FIELDS AND TOUCHES NOTHING ELSE ON THE MODEL. Every other
	 * field is `StratBuildViewModel`'s, sourced from a `strat::UiSnapshot` field with no
	 * transformation; a decorator that adjusted one of those would be widget-side
	 * arithmetic wearing a different hat.
	 *
	 * NON-CONST ON THE MODEL AND CONST ON THIS. The machine is the source of the two bits
	 * and the model is where they live; nothing here is learned from the model.
	 */
	void DecorateViewModel(FStratViewModel& Model) const;

	/**
	 * The two highlight sets for the current selection.
	 *
	 * BOTH FROM THE QUERY, NEITHER COMPUTED. With nothing selected both come back empty,
	 * which is how the overlays clear -- `AStratBoardActor::ShowReach({})` and
	 * `ClearReach()` do the same thing and the board's header says so.
	 *
	 * A UNIT THAT HAS ALREADY MOVED GETS AN EMPTY REACH SET, and that comes from this
	 * machine reading `bHasMoved` off the model rather than from the query returning less:
	 * `FStratBridge::ReachableHexes` always answers with at least the unit's own hex,
	 * because a null move is a move. Drawing that single hex as "you may move here" after
	 * the unit has moved would be an overlay saying something §2.1 does not.
	 *
	 * A QUERY REFUSAL LEAVES BOTH SETS EMPTY AND IS REPORTED. It is not a fault of the
	 * selection -- the usual cause is an unseeded bridge -- and drawing a stale overlay
	 * would be worse than drawing none.
	 */
	void BuildOverlays(const FStratViewModel&  Model,
	                   const IStratRulesQuery& Query,
	                   TArray<FIntPoint>&      OutReachHexes,
	                   TArray<FIntPoint>&      OutTargetHexes,
	                   FString&                OutFailureReason) const;

	// ---- State, readable ---------------------------------------------------

	/** The selected unit, or `INDEX_NONE`. The whole of this machine's own selection state. */
	int32 GetSelectedUnitId() const { return SelectedUnitId; }

	/** Whether this unit has been marked finished this turn. `bDone`'s source. */
	bool IsDone(int32 UnitId) const { return DoneUnits.Contains(UnitId); }

	/**
	 * §2.11.6's per-turn lock. THE GUIDANCE LAYER'S WRITER, AND THE GUIDANCE LAYER NOW
	 * EXISTS. This line used to end:
	 * RETRACTED> "AND THERE IS NO GUIDANCE LAYER YET."
	 * Wave B2 landed `FStratGuidedOpening` (`StratGuidedOpening.h`), which calls this once
	 * per
	 * refresh from `Observe`. Retracted in place: the old sentence is the one a reader
	 * will remember.
	 *
	 * HERE RATHER THAN NOWHERE, deliberately. `bLockedThisTurn` had no producer at all
	 * before this file, and the failure mode phase 2 recorded is that its owner puts it in
	 * an actor for want of anywhere else. This is somewhere else.
	 *
	 * THE NEXT CLAIM IS RETRACTED, and it read:
	 * RETRACTED> "Nothing in the hot-seat milestone calls it -- §2.11.6's guided opening is
	 * RETRACTED>  explicitly out of scope -- so the set is empty in every shipping path today
	 * RETRACTED>  and `DecorateViewModel` writes false for every unit."
	 * True then, false now.
	 * `FStratGuidedOpening::Observe` locks every friendly unit that is not the marked
	 * Infantry while beat 1a is outstanding and clears them all the instant it retires.
	 * `StratBuildViewModel` does still write false for every unit, and that part is
	 * unchanged and correct -- the bit is produced between that call and `ApplyView`,
	 * never inside it.
	 *
	 * A LOCKED UNIT CANNOT BE SELECTED, which is what "locked" means and is enforced in
	 * `HandleEvent`. That enforcement was unobservable for want of a caller; it is
	 * observable now, and the foresight paid off exactly as recorded -- the guidance layer
	 * added no rule to this state machine, it only filled a set this machine already knew
	 * how to honour.
	 *
	 * ITS LIFECYCLE IS NOT `bDone`'s and this struct does not manage it: `StratViewModel.h`
	 * records that a lock clears when beat 1a RETIRES, inside turn 1, and not at the turn
	 * boundary. `NotifyCommandApplied` therefore clears `DoneUnits` on an `EndTurn` and
	 * deliberately does NOT clear this.
	 */
	void SetLockedThisTurn(int32 UnitId, bool bLocked);

	bool IsLockedThisTurn(int32 UnitId) const { return LockedUnits.Contains(UnitId); }

	/** Drops the selection and both sets. For a reseed; a turn boundary is not this. */
	void Reset();

private:
	/** The model's entry for a unit, or null. The machine's only lookup. */
	static const FStratUnitView* FindUnit(const FStratViewModel& Model, int32 UnitId);

	/** The unit standing on a hex, or null. */
	static const FStratUnitView* FindUnitAtHex(const FStratViewModel& Model, FIntPoint Hex);

	/**
	 * The selected unit, or `INDEX_NONE`. NOT a state enum: whether that unit has moved or
	 * acted is the model's answer, read fresh every time. See the header block.
	 */
	int32 SelectedUnitId = INDEX_NONE;

	/** §2.11.1's DONE units this turn. Cleared by an accepted `EndTurn` and by nothing else. */
	TSet<int32> DoneUnits;

	/** §2.11.6's locked units. Written only by `SetLockedThisTurn`, whose one shipping
	 *  caller is `FStratGuidedOpening::Observe` (wave B2). Non-empty while beat 1a is
	 *  outstanding; empty at every other moment of a match. */
	TSet<int32> LockedUnits;
};

/**
 * Submits one outcome's command and emits the `STRAT-CMD accepted` line. THE ONLY PLACE
 * EITHER HAPPENS.
 *
 * A FREE FUNCTION TAKING A BRIDGE, so that a replay-parity gate can drive the machine and
 * apply its commands with no controller, no world and no PIE -- the same reason
 * `StratBuildViewModel` is a free function. The controller calls this; a test calls this;
 * there is no third path to a submitted command in this module.
 *
 * THE LOG LINE'S FORMAT IS LOAD-BEARING, and this is the only producer of it:
 *
 *     STRAT-CMD accepted kind=%s unit=%d hex=%d,%d turn=%d side=%d hash=%s
 *
 * It is what turns a PIE playtest into a machine-checkable gate in phase 6, so it is
 * written as one `UE_LOG` with a fixed field order and no conditional fields. Notes on each
 * choice, because a later reader will otherwise "tidy" one of them:
 *   - `kind` is spelled `Move` / `Attack` / `EndTurn`, matching the SAVE FORMAT's own
 *     spelling (`strat::saveCommandName`'s switch) rather than this module's enum names, so a gate
 *     can compare a log line against a `commandLog` entry without a translation table.
 *   - `unit` and `hex` are ALWAYS NUMERIC, and carry `-1` (and `-1,-1`) where the command
 *     kind has no such field -- `EndTurn` names no unit and no hex. A word like `none`
 *     would have made the line's shape depend on its content.
 *   - `turn` and `side` are read from the bridge BEFORE submission, because
 *     `FStratBridge`'s five typed methods stamp the command with the values live at that
 *     instant: "the command that closes turn N is tagged N". Reading them afterwards would
 *     print N+1 for every `EndTurn` and describe a command nobody submitted.
 *   - `hash` is `FStratBridge::StateHash()` AFTER the command applies, which is the point
 *     of having it: two runs that accept the same commands in the same order must print the
 *     same hashes, and the first line where they differ names the command that diverged.
 *
 * EMITTED ONLY ON ACCEPTANCE, which is what the word `accepted` in the line claims. A
 * refused command returns false with the bridge's own reason and logs a Warning in a
 * different shape, so a grep for `STRAT-CMD accepted` counts commands that actually applied.
 *
 * IT DOES NOT CALL `NotifyCommandApplied`. Deliberately: this function takes a bridge and
 * the machine does not, and fusing them would put a bridge pointer inside the state
 * machine's reach. The caller does both, in that order, and `AStratPlayerController` is
 * the one production caller.
 *
 * @param Bridge   the seeded bridge. Not checked for seeding here -- the bridge refuses in
 *                 its own words, which are better than any this function could invent.
 * @param Outcome  what `HandleEvent` decided. `Command == None` returns true having done
 *                 nothing, because "no command" is not a failure.
 */
STRATPLAY_API bool StratSubmitSelectionCommand(FStratBridge&                 Bridge,
                                               const FStratSelectionOutcome& Outcome,
                                               FString&                      OutFailureReason);
