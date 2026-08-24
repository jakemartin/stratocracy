// GDD §2.11.6-B — the guided opening's beat machine. See StratGuidedOpening.h for the
// beat model, the reading it had to choose, and what is deliberately not here.
#include "StratGuidedOpening.h"

#include "StratPlay.h"
#include "StratSelectionMachine.h"

// The bridge, from the .cpp only. This file names `FStratBridge` and calls one engine-typed
// façade method on it; it names no `strat::` type and calls no `strat::` free function.
#include "StratBridge.h"

namespace
{
	/** `EStratGuidanceBeat` -> a 0..3 slot. `None` has none and must never be passed. */
	int32 SlotOf(EStratGuidanceBeat Beat)
	{
		check(Beat != EStratGuidanceBeat::None);
		return static_cast<int32>(Beat) - 1;
	}

	/**
	 * The four beats in order-index order — which is declaration order, which is the
	 * underlying value's order. Written out rather than looped over a cast so that adding a
	 * beat is a change here and a compile error nowhere, and so a reader can see that 1a
	 * precedes 1b precedes 2 precedes 3.
	 */
	const EStratGuidanceBeat kBeatsInOrder[4] = {
		EStratGuidanceBeat::Beat1a,
		EStratGuidanceBeat::Beat1b,
		EStratGuidanceBeat::Beat2,
		EStratGuidanceBeat::Beat3
	};
}

// ---------------------------------------------------------------------------
// §2.11.6-B's literal strings. Verbatim, full stops included.
// ---------------------------------------------------------------------------

FText FStratGuidedOpening::DirectiveTextFor(EStratGuidanceBeat Beat)
{
	switch (Beat)
	{
	case EStratGuidanceBeat::Beat1a:
		return FText::FromString(
			TEXT("Select the marked Infantry. Lit hexes are its true reach. Click one to move."));
	case EStratGuidanceBeat::Beat1b:
		return FText::FromString(TEXT("End turn. The enemy moves; then you."));
	case EStratGuidanceBeat::Beat2:
		return FText::FromString(
			TEXT("Move the Infantry onto the ringed Factory. Only Infantry captures."));
	case EStratGuidanceBeat::Beat3:
		return FText::FromString(TEXT("Spend Fame at your Factory. Infantry costs 100."));
	default:
		// `None` has no line, and an empty one is the honest answer rather than a
		// placeholder a widget might render.
		return FText::GetEmpty();
	}
}

FText FStratGuidedOpening::WindowEndTagText()
{
	return FText::FromString(TEXT("guidance ends this turn"));
}

FText FStratGuidedOpening::LockedUnitHoverText()
{
	return FText::FromString(TEXT("Locked this turn."));
}

FText FStratGuidedOpening::EndTurnGateHoverText()
{
	return FText::FromString(TEXT("Move the marked Infantry first."));
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void FStratGuidedOpening::Reset()
{
	*this = FStratGuidedOpening();
}

void FStratGuidedOpening::Begin(const FStratBridge& Bridge, int32 InGuidedSide, bool bInSuppressed)
{
	Reset();

	GuidedSide  = InGuidedSide;
	bSuppressed = bInSuppressed;

	if (bSuppressed)
	{
		// §2.11.6: "Any completed match on the save skips all guidance automatically." Not
		// an error and not logged at Warning — it is the second match, working.
		UE_LOG(LogStratPlay, Log,
			TEXT("Guided opening suppressed for side %d: this save has a completed match."),
			GuidedSide);
		return;
	}

	// The ringed factory. A LOOKUP OF AN AUTHORED FIELD — see the bridge method's block on
	// why the alternative (finding the nearest neutral factory) is forbidden by name.
	FIntPoint InfantryDeployHex = FIntPoint::ZeroValue;
	FIntPoint ObjectiveFromScenario = FIntPoint::ZeroValue;
	const FStratResult Guided =
		Bridge.GuidedOpeningHexes(GuidedSide, InfantryDeployHex, ObjectiveFromScenario);

	if (!Guided.bOk)
	{
		// A scenario with no `guidedOpening` for this seat runs with no guided opening. That
		// is a configuration, not a fault: `Ui.h` says a missing block "marks nobody rather
		// than being an error", and with nobody marked beat 1a has no subject.
		UE_LOG(LogStratPlay, Log,
			TEXT("No guided opening for side %d: %s"), GuidedSide, *Guided.Reason);
		return;
	}

	bHasObjective = true;
	Objective     = ObjectiveFromScenario;
	bActive       = true;

	// `InfantryDeployHex` is READ AND NOT RETAINED, deliberately. The marked unit is found
	// through `bIsGuidedMarked`, which the rules module derives off `placement`; keeping the
	// deployment hex here would offer a future reader a hex comparison to reach for, and
	// that comparison stops being true the moment beat 1a's own move lands — which is the
	// exact bug the `placement` derivation exists to prevent.
	UE_LOG(LogStratPlay, Log,
		TEXT("Guided opening armed for side %d: objective hex (%d, %d), window turns %d-%d."),
		GuidedSide, Objective.X, Objective.Y, kFirstGuidedTurn, kLastGuidedTurn);
}

void FStratGuidedOpening::SkipGuidance()
{
	if (!bActive)
	{
		return;
	}

	bActive  = false;
	LineBeat = EStratGuidanceBeat::None;
	LineRule = 0;

	// THE BOARD STATE GOES WITH THE STRIP. §2.11.6: the ring "and the turn-1a unit marker
	// clear in the same frame as the strip". The ring clears because `bHasObjective` is what
	// `DecorateViewModel` publishes it from; the marker clears because that same call
	// publishes `bActive` false and the marker is drawn only while guidance runs.
	//
	// [STAMPED 2026-08-24. THE SECOND CLAUSE ABOVE WAS FALSE FOR A DAY AND IS TRUE NOW.]
	// Nothing implemented "the marker is drawn only while guidance runs" when that sentence
	// was written: `AStratUnitActor::ApplyUnitView` set the marker from
	// `View.bIsGuidedMarked && View.Side == ViewingSide`, both of them match-constant, so the
	// marker LATCHED ON and survived every route out of the window including this one. Found
	// in a human playtest on 2026-08-24 -- the ring cleared, the marker did not -- and fixed
	// by giving that writer `FStratViewModel::Guidance.bActive`, which is the very bit this
	// function drops. The sentence is left standing rather than rewritten because it is now
	// an accurate description of the tree, and because a reader should see that it described
	// an intended mechanism before it described an implemented one. **A SENTENCE IN THIS FILE
	// ASSERTING WHAT ANOTHER FILE DOES IS A CLAIM, NOT A SPECIFICATION**, and this one went
	// unmeasured for a day because it read like the latter.
	bHasObjective = false;

	// THE LOCKS ARE NOT CLEARED HERE. `Observe` is the single writer of the lock set and it
	// runs on the next refresh — which the controller performs in this same call chain, so
	// "the same frame" holds. A clear here would be a second writer of one set.
	UE_LOG(LogStratPlay, Log, TEXT("Guided opening skipped by the player; it does not return."));
}

// ---------------------------------------------------------------------------
// Observation — the only thing that advances a beat
// ---------------------------------------------------------------------------

void FStratGuidedOpening::Observe(const FStratViewModel& Model, FStratSelectionMachine& Machine)
{
	if (!bActive)
	{
		// STILL PUBLISHES LOCKS, and that is what makes a skip take effect. `PublishLocks`
		// clears every lock when guidance is inactive, so the frame after `SkipGuidance` has
		// no dimmed unit — rather than leaving the previous frame's locks standing because
		// the early return skipped the only writer.
		PublishLocks(Model, Machine);
		return;
	}

	const int32 Turn = Model.Match.Turn;

	// ---- The window closes at the END of turn 4, unconditionally ------------
	// §2.11.6: "The strip disappears for good once all four beats have retired, and
	// unconditionally at the end of turn 4; every beat, the objective ring, and the turn-1a
	// marker expire with it."
	//
	// OBSERVED AS "THE ROUND IS PAST 4" AND NOT AS A TURN-END EVENT, because this machine has
	// no turn-end event to hook: `Observe` sees rounds, and the first observation with
	// `Turn > 4` is the earliest moment "the end of turn 4" has demonstrably passed. The
	// difference is invisible — nothing renders between the last refresh of turn 4 and the
	// first of turn 5.
	if (Turn > kLastGuidedTurn)
	{
		bActive       = false;
		bHasObjective = false;
		LineBeat      = EStratGuidanceBeat::None;
		LineRule      = 0;
		PublishLocks(Model, Machine);
		UE_LOG(LogStratPlay, Log,
			TEXT("Guided opening window closed at the end of turn %d."), kLastGuidedTurn);
		return;
	}

	// ---- Retirement, before anything else -----------------------------------
	// FIRST, because a beat that retired on the board must not be able to hold the line for
	// one more frame, and because 1a's hand-off to 1b inside turn 1 depends on the line being
	// reassigned in the same observation that sees the move complete.
	RetireWhatTheModelRetires(Model);

	// ---- The turn clock -----------------------------------------------------
	const bool bNewTurn = (LastObservedTurn != Turn);
	LastObservedTurn = Turn;

	if (bNewTurn)
	{
		// Rules 1 and 2, at the start of a turn. This is the ONE place a beat is marked as
		// having had its turn on the line.
		AssignLineAtTurnStart();
	}
	else if (LineBeat == EStratGuidanceBeat::None || !IsBeatOutstanding(LineBeat))
	{
		// Mid-turn succession. §2.11.6: "there is no live-but-blank strip in this system.
		// Rule 2 has no exit: while any beat is outstanding, some beat holds the line."
		InheritLineAfterRetirement();
	}

	// ---- All four retired: the strip is gone, permanently -------------------
	if (LineBeat == EStratGuidanceBeat::None)
	{
		bActive       = false;
		bHasObjective = false;
		LineRule      = 0;
		UE_LOG(LogStratPlay, Log,
			TEXT("Guided opening complete on turn %d: every beat retired."), Turn);
	}

	PublishLocks(Model, Machine);
}

bool FStratGuidedOpening::RetireWhatTheModelRetires(const FStratViewModel& Model)
{
	bool bAnything = false;

	// ---- 1a: the marked Infantry's move completes ---------------------------
	if (bOutstanding[SlotOf(EStratGuidanceBeat::Beat1a)])
	{
		const int32 MarkedId = FindMarkedUnitId(Model);
		for (const FStratUnitView& U : Model.Units)
		{
			if (U.UnitId == MarkedId && U.bHasMoved)
			{
				bOutstanding[SlotOf(EStratGuidanceBeat::Beat1a)] = false;
				bAnything = true;
				UE_LOG(LogStratPlay, Log,
					TEXT("Guided beat 1a retired on turn %d: the marked Infantry moved."),
					Model.Match.Turn);
				break;
			}
		}
	}

	// ---- 1b: the enemy turn ends --------------------------------------------
	// READ AS "THE ROUND ADVANCED PAST THE ONE 1b TOOK THE LINE ON". `strat::TurnState`
	// counts a round, so a round boundary is precisely both seats having ended their turn --
	// which, for a beat that took the line on the player's turn 1, is exactly the enemy turn
	// ending. `Turn1bTookTheLine` is `INDEX_NONE` until 1b actually holds the line, so a beat
	// that never got there cannot retire on a boundary it never saw.
	if (bOutstanding[SlotOf(EStratGuidanceBeat::Beat1b)]
		&& Turn1bTookTheLine != INDEX_NONE
		&& Model.Match.Turn > Turn1bTookTheLine)
	{
		bOutstanding[SlotOf(EStratGuidanceBeat::Beat1b)] = false;
		bAnything = true;
		UE_LOG(LogStratPlay, Log,
			TEXT("Guided beat 1b retired on turn %d: the enemy turn ended."), Model.Match.Turn);
	}

	// ---- 2: the ringed objective is taken by the guided seat ----------------
	// TWO OBSERVABLES, OR'd, AND THE SECOND IS THE ONLY ONE THE SHIPPED SCENARIO CAN SHOW.
	// `IsRingedObjectiveHeldByGuidedSide` carries the measurement: at `captureTurns = 1` the
	// pip is created and erased inside one `strat::captureTick`, so `CaptureProgress` reads 0
	// in every snapshot and `HasCapturePipLanded` cannot fire. The pip arm is KEPT rather
	// than replaced because it is §2.11.6's own named trigger and it is the earlier of the
	// two at `captureTurns >= 2` — the arrival receipt, not the deed.
	//
	// WHICH ARM FIRED IS LOGGED, and that is not decoration. The two arms mean different
	// things to anyone reading a session log, and a single line would have made this defect
	// look like a fix that worked for the reason nobody checked.
	if (bOutstanding[SlotOf(EStratGuidanceBeat::Beat2)])
	{
		const bool bPip  = HasCapturePipLanded(Model);
		const bool bHeld = IsRingedObjectiveHeldByGuidedSide(Model);
		if (bPip || bHeld)
		{
			bOutstanding[SlotOf(EStratGuidanceBeat::Beat2)] = false;
			bAnything = true;
			UE_LOG(LogStratPlay, Log,
				TEXT("Guided beat 2 retired on turn %d: the ringed objective (%d, %d) was "
				     "taken by side %d (pip=%s, held=%s)."),
				Model.Match.Turn, Objective.X, Objective.Y, GuidedSide,
				bPip ? TEXT("true") : TEXT("false"),
				bHeld ? TEXT("true") : TEXT("false"));
		}
	}

	// ---- 3: a unit spawns, on whatever turn including turn 1 ----------------
	// EVALUATED BEFORE THE SEEN-SET IS UPDATED, and the order is the whole mechanism.
	if (bOutstanding[SlotOf(EStratGuidanceBeat::Beat3)] && HasAnyUnitSpawned(Model))
	{
		bOutstanding[SlotOf(EStratGuidanceBeat::Beat3)] = false;
		bAnything = true;
		UE_LOG(LogStratPlay, Log,
			TEXT("Guided beat 3 retired on turn %d: a unit spawned."), Model.Match.Turn);
	}

	// The seen-set is refreshed AFTER the spawn test, every observation, so the first
	// observation of a match establishes the baseline rather than reporting ten spawns.
	for (const FStratUnitView& U : Model.Units)
	{
		if (U.Side == GuidedSide)
		{
			SeenFriendlyUnitIds.Add(U.UnitId);
		}
	}

	return bAnything;
}

// ---------------------------------------------------------------------------
// Rules 1 and 2
// ---------------------------------------------------------------------------

void FStratGuidedOpening::AssignLineAtTurnStart()
{
	LineBeat = EStratGuidanceBeat::None;
	LineRule = 0;

	// RULE 1 — the lowest-numbered outstanding beat that has not yet held the line on an
	// earlier turn. `kBeatsInOrder` is order-index order, so the first hit is the lowest.
	for (const EStratGuidanceBeat Beat : kBeatsInOrder)
	{
		if (bOutstanding[SlotOf(Beat)] && TurnHeldTheLine[SlotOf(Beat)] == INDEX_NONE)
		{
			LineBeat = Beat;
			LineRule = 1;
			break;
		}
	}

	// RULE 2 — every outstanding beat has already had its turn on the line, so the
	// lowest-numbered outstanding beat gets a last call.
	if (LineBeat == EStratGuidanceBeat::None)
	{
		for (const EStratGuidanceBeat Beat : kBeatsInOrder)
		{
			if (bOutstanding[SlotOf(Beat)])
			{
				LineBeat = Beat;
				LineRule = 2;
				break;
			}
		}
	}

	if (LineBeat == EStratGuidanceBeat::None)
	{
		return;
	}

	// THE ONE PLACE A BEAT IS MARKED AS HAVING HAD ITS TURN. See the header block: an
	// inheritor that picks up the tail of another beat's turn is deliberately not marked,
	// because rule 2's own wording is "has already had ITS TURN on the line".
	//
	// STAMPED ON EVERY SELECTION AND NOT ONLY THE FIRST, which changes nothing rule 1 tests
	// (it asks only whether the value is `INDEX_NONE`) and keeps the field readable as "the
	// most recent turn this beat was selected at a turn start" for a log line.
	TurnHeldTheLine[SlotOf(LineBeat)] = LastObservedTurn;

	// Beat 1b's retire condition is a round boundary measured from here, so the round it
	// took the line on is recorded at the moment it takes it and nowhere else.
	if (LineBeat == EStratGuidanceBeat::Beat1b && Turn1bTookTheLine == INDEX_NONE)
	{
		Turn1bTookTheLine = LastObservedTurn;
	}
}

void FStratGuidedOpening::InheritLineAfterRetirement()
{
	LineBeat = EStratGuidanceBeat::None;
	LineRule = 0;

	// The same two rules, in the same order, WITHOUT the mark. Rule 1 first so that 1a's
	// hand-off inside turn 1 reaches 1b rather than falling through to a last call.
	for (const EStratGuidanceBeat Beat : kBeatsInOrder)
	{
		if (bOutstanding[SlotOf(Beat)] && TurnHeldTheLine[SlotOf(Beat)] == INDEX_NONE)
		{
			LineBeat = Beat;
			LineRule = 1;
			break;
		}
	}

	if (LineBeat == EStratGuidanceBeat::None)
	{
		for (const EStratGuidanceBeat Beat : kBeatsInOrder)
		{
			if (bOutstanding[SlotOf(Beat)])
			{
				LineBeat = Beat;
				LineRule = 2;
				break;
			}
		}
	}

	// 1b INHERITING THE LINE STILL STARTS ITS CLOCK, and it must: in every branch of the
	// schedule table 1b reaches the line by inheritance from 1a inside turn 1, never by a
	// turn-start assignment, so a clock started only in `AssignLineAtTurnStart` would never
	// start at all and 1b would never retire.
	if (LineBeat == EStratGuidanceBeat::Beat1b && Turn1bTookTheLine == INDEX_NONE)
	{
		Turn1bTookTheLine = LastObservedTurn;
	}
}

// ---------------------------------------------------------------------------
// The lock set
// ---------------------------------------------------------------------------

void FStratGuidedOpening::PublishLocks(const FStratViewModel& Model, FStratSelectionMachine& Machine)
{
	// §2.11.6-B beat 1a: "Only one marked Infantry selectable; others dimmed".
	//
	// KEYED ON 1a BEING OUTSTANDING, NOT ON ITS HOLDING THE LINE, and `StratViewModel.h`
	// states the lifecycle this satisfies: the lock "clears when beat 1a RETIRES — when the
	// marked Infantry's move completes, inside turn 1 — and not at the turn boundary. A unit
	// can be un-locked and not-done in the same turn."
	const bool bLockOn = bActive && bOutstanding[SlotOf(EStratGuidanceBeat::Beat1a)];

	// FOUND EVERY OBSERVATION AND NOT ONLY WHEN THE LOCK IS ON, because `IsUnitInputGated`
	// reads the cache and the controller consults it on clicks that happen while beat 1a is
	// outstanding but nothing is locked yet — the first click of the match is one.
	const int32 MarkedId = FindMarkedUnitId(Model);
	LastKnownMarkedUnitId = MarkedId;

	// WRITTEN FOR EVERY FRIENDLY UNIT EVERY OBSERVATION, both true and false, rather than
	// set once and cleared once. The set is small, and a writer that only ever sets is a
	// writer whose clear can be missed on one path — which for this bit means a permanently
	// undimmable unit and a player who cannot move.
	for (const FStratUnitView& U : Model.Units)
	{
		if (U.Side != GuidedSide)
		{
			continue;
		}

		const bool bLocked = bLockOn && (U.UnitId != MarkedId);
		Machine.SetLockedThisTurn(U.UnitId, bLocked);
	}
}

// ---------------------------------------------------------------------------
// Projection
// ---------------------------------------------------------------------------

void FStratGuidedOpening::DecorateViewModel(FStratViewModel& Model) const
{
	FStratGuidanceView View;

	View.bActive = bActive;

	if (bActive)
	{
		View.Beat          = LineBeat;
		View.DirectiveText = DirectiveTextFor(LineBeat);

		// THE TAG IS A FACT ABOUT THE WINDOW. §2.11.6: "The tag states a fact about the
		// *window*, not about rule 2, so it renders on turn 4 only" — the fast lane's turn-3
		// rule-2 last call is shown untagged for that reason. So this is the turn and not
		// `LineRule == 2`.
		View.bShowsWindowEndTag = (Model.Match.Turn == kLastGuidedTurn);
		View.WindowEndTagText   = View.bShowsWindowEndTag ? WindowEndTagText() : FText::GetEmpty();

		View.bHasObjectiveRing = bHasObjective;
		View.ObjectiveHex      = Objective;

		View.bEndTurnGated    = IsEndTurnGated();
		View.EndTurnGateHover = View.bEndTurnGated ? EndTurnGateHoverText() : FText::GetEmpty();

		// Non-empty whenever anything can be locked, so a widget rendering a hover need not
		// scan the unit array to decide whether the string is meaningful.
		View.LockedUnitHover = bOutstanding[SlotOf(EStratGuidanceBeat::Beat1a)]
			? LockedUnitHoverText()
			: FText::GetEmpty();
	}

	// ASSIGNED WHOLE, so an inactive guided opening publishes a default-constructed block
	// rather than last frame's fields with one bool flipped. That is what makes "the ring
	// and the marker clear in the same frame as the strip" structural.
	Model.Guidance = MoveTemp(View);
}

// ---------------------------------------------------------------------------
// The Q27 gates (§4.7, ruled) and the small readers
// ---------------------------------------------------------------------------

bool FStratGuidedOpening::IsEndTurnGated() const
{
	return bActive && bOutstanding[SlotOf(EStratGuidanceBeat::Beat1a)];
}

bool FStratGuidedOpening::IsUnitInputGated(int32 UnitId) const
{
	if (UnitId == INDEX_NONE || !IsEndTurnGated())
	{
		return false;
	}

	// THE GATE APPLIES TO THE MARKED UNIT ALONE. Every other friendly unit is unselectable
	// through the lock, so it can never be the subject of a Wait or an attack in the first
	// place, and gating those here as well would be a second mechanism for one restriction.
	//
	// AGAINST THE ID `Observe` LAST SAW MARKED. See the member's declaration on why this is a
	// cache rather than a model parameter, and why its staleness window is one refresh.
	return UnitId == LastKnownMarkedUnitId;
}

bool FStratGuidedOpening::IsBeatOutstanding(EStratGuidanceBeat Beat) const
{
	return Beat != EStratGuidanceBeat::None && bOutstanding[SlotOf(Beat)];
}

int32 FStratGuidedOpening::FindMarkedUnitId(const FStratViewModel& Model) const
{
	// §2.11.6 beat 1a's marked unit, READ AND NEVER DERIVED. `bIsGuidedMarked` is the rules
	// module's own derivation off `placement`; a hex comparison here against
	// `guidedOpening.infantry` would unmark the unit the instant beat 1a's move landed.
	for (const FStratUnitView& U : Model.Units)
	{
		if (U.Side == GuidedSide && U.bIsGuidedMarked)
		{
			return U.UnitId;
		}
	}
	return INDEX_NONE;
}

bool FStratGuidedOpening::HasCapturePipLanded(const FStratViewModel& Model) const
{
	// NARROWED TO THE RINGED OBJECTIVE BY USER RULING, 2026-08-21. See the declaration for
	// why §2.11.6 does not settle this on its own, what the narrowing costs, and which
	// clause is now load-bearing for it.
	//
	// NO OBJECTIVE MEANS NO RETIREMENT, and that arm is unreachable rather than defensive:
	// `Begin` leaves the machine inactive when `GuidedOpeningHexes` refuses, so beat 2 is
	// never evaluated without a ring. It is written out because the alternative -- falling
	// back to the unqualified reading when there is no hex to compare against -- would be a
	// second, quieter policy for exactly the case the ruling was about.
	if (!bHasObjective)
	{
		return false;
	}

	for (const FStratUnitView& U : Model.Units)
	{
		if (U.Side == GuidedSide && U.CaptureProgress > 0 && U.Hex == Objective)
		{
			return true;
		}
	}
	return false;
}

bool FStratGuidedOpening::IsRingedObjectiveHeldByGuidedSide(const FStratViewModel& Model) const
{
	// See the declaration for the measurement that made this arm necessary, for why the
	// 2026-08-21 ruling is untouched by it, and for why a scenario that seeds the objective
	// already-held retiring beat 2 at once is the correct answer.
	//
	// NO OBJECTIVE MEANS NO RETIREMENT, for `HasCapturePipLanded`'s reason and not a
	// different one: `Begin` leaves the machine inactive when `GuidedOpeningHexes` refuses.
	if (!bHasObjective)
	{
		return false;
	}

	// THE HEX VIEW AND NOT THE FACTORY VIEW, and the choice is load-bearing rather than
	// incidental. `guidedOpening.objective` is an OBJECTIVE, and `strat::Objective` is not
	// obliged to sit on Factory terrain — §2.11.6's directive says "ringed Factory" because
	// that is what Ferrum Crossing authored, not because the beat is about factories.
	// `FStratHexView::Owner` mirrors `UiHexView::owner` for every capturable hex, so this
	// arm keeps working on a scenario whose ringed objective is a Town.
	//
	// A HEX THAT IS NOT CAPTURABLE READS `INDEX_NONE`, which is `strat::OWNER_NEUTRAL` and
	// can never equal a side, so a mis-authored objective fails CLOSED — beat 2 stays
	// outstanding — rather than retiring on a hex nobody can own.
	for (const FStratHexView& H : Model.Hexes)
	{
		if (H.Hex == Objective)
		{
			return H.Owner == GuidedSide;
		}
	}
	return false;
}

bool FStratGuidedOpening::HasAnyUnitSpawned(const FStratViewModel& Model) const
{
	// THE FIRST OBSERVATION IS A BASELINE AND NOT TEN SPAWNS. `SeenFriendlyUnitIds` is empty
	// only before the first `Observe`, and `Begin` does not observe — so this guard is what
	// keeps a seeded board's own units from retiring beat 3 before the player has spent a
	// single point of Fame.
	if (SeenFriendlyUnitIds.Num() == 0)
	{
		return false;
	}

	for (const FStratUnitView& U : Model.Units)
	{
		if (U.Side == GuidedSide && !SeenFriendlyUnitIds.Contains(U.UnitId))
		{
			return true;
		}
	}
	return false;
}
