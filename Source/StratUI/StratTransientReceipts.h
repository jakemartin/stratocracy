// GDD §2.11.2 / GATE-TRANSIENT -- the transient layer's engine side: the income and kill
// receipts, and the YOUR TURN / ENEMY TURN banner.
//
// WHAT GAP THIS CLOSES. `FStratViewModel` is a COMPLETE STATEMENT OF STEADY STATE and its
// own header forbids the alternative in terms -- "there is no 'changed' flag, no event, no
// dirty set and no sequence number anywhere below, and adding one would be the first step
// towards a screen that is only correct if every previous frame was also correct." That
// property is T-INT-05's and nothing here weakens it. But §2.11.2's transient facts are
// EDGES: "you were just paid", "you just destroyed something", "it is your turn now". A
// value that carries no history cannot express an edge, so until this file existed there
// was no engine-side place for one, and the only remaining place was the widget -- which
// would have had to keep its own previous reading and subtract, i.e. widget-side arithmetic
// over a remembered frame, which is T-UI-03's clause and the reconciliation rule failing at
// the same time.
//
// THE CONTRACT THAT KEEPS T-INT-05 TRUE, and it is the load-bearing sentence in this file:
// THE SCREEN MUST BE FULLY CORRECT WITH ZERO RECEIPTS DRAWN. `FStratTransientReceipts` is
// not a field of `FStratViewModel`, is not an input to `ApplyView`, and carries nothing the
// view model does not already carry as steady state. Every number in a receipt is a reading
// of a DURABLE HOME -- §2.11.2's own word -- that is on screen anyway:
//
//     income  -> `FStratSideView::IncomePerTurn`, the Fame widget's "+X/turn"
//     kill    -> `FStratSideView::FameCombat`, the scoreboard's "Destroyed" row
//     repair  -> `FStratUnitView::Hp`, the unit's own HP readout   [added 2026-09-01]
//     banner  -> `FStratMatchView::SideToMove`
//
// So a receipt is a SECOND, LOUDER TELLING of a fact the durable surface is already
// showing, and dropping every receipt on the floor loses emphasis and loses no information.
// If that ever stops being true -- if a receipt becomes the only place a fact appears --
// this file has become an event list and T-INT-05 has a hole in it.
//
// THE DESTROYED ROW IS COMBAT FAME AND NOT A COUNT OF UNITS, WHICH IS WORTH READING BEFORE
// THE KILL RECEIPT BELOW MAKES SENSE. `EStratScoreCriterion::CombatFame` carries the
// `DisplayName` "Destroyed" and `FStratScoreboardRow`'s own comment maps that row to
// `UiSideView::fameCombat`; §2.11.4 gives it a disambiguating tooltip precisely because the
// label reads like a tally and the number is a currency. There is NO published field
// anywhere in this tree carrying "how many units this side has destroyed" -- measured
// across `StratViewModel.h`, `StratBridge.h`, `StratScoreboardWidget.h` and the vendored
// `Ui.h`. So the kill receipt's number is Fame, in Fame's units, and a drawing layer that
// prints it as a body count is printing a different quantity.
//
// WHY A DELTA IS NOT A RECOMPUTATION, stated because the two look alike and only one of
// them is allowed. `strat::killAward` decided the award; the rules module folded it into
// `fameCombat`; this file subtracts one reading of THAT FIELD from an earlier reading of
// THAT SAME FIELD. No cost table is read, no half is taken, no flag bonus is special-cased
// -- the three clauses `StratBridge.h` says a deriver would have to restate are all absent
// here, and a change to any of them moves this number automatically. Contrast the thing
// that IS forbidden and that this file does not do: computing an award from a victim's §2.4
// cost. `FStratCombatOutcome` sets the precedent from the other side -- it carries
// `AttackerFameCombatBefore` and `AttackerFameCombatAfter` "rather than a delta, because a
// delta is arithmetic". THIS FILE CARRIES ALL THREE for that reason: the two readings are
// the mirrors, `Amount` is the one subtraction, and a clause can check it against its own
// inputs rather than trusting it.
//
// THE INCOME RECEIPT CARRIES A RATE AND SAYS SO IN THE NAME OF ITS OWN ARM, and this is the
// sharpest thing in the file. `Ui.h` is explicit that `incomePerTurn` "is the STANDING rate,
// and that is what it reads ON TURN 1: what those holdings will pay at the start of that
// side's turn 2, NOT the 0 that Q8(a) pays on turn 1", and it is "deliberately NOT
// `accrueIncome`'s return value". `accrueIncome` returns the amount actually added and that
// return value is published NOWHERE -- not in `UiSnapshot`, not through `FStratBridge`,
// measured on this tree. So:
//
//   - The ruling names the rate as this receipt's durable home and the rate is what is
//     carried. Nothing is derived and nothing is guessed.
//   - The arm is named `IncomeRate` and not `Income` so that no drawing layer can read the
//     number as "you were just paid this" without contradicting the enumerator in front of
//     it. Copy off this arm must be rate-voiced ("+8 / turn"), which is TRUE on turn 1.
//   - Q8(a)'s turn-1 zero is NOT transcribed into this file, and that refusal is deliberate
//     rather than an oversight. Suppressing the receipt on `Turn <= 1` would put a rules
//     fact in the presentation layer, where nothing rechecks it against §2.7 and where it
//     would silently outlive any change to the rule.
//
//   THE DEBT, written down rather than owned quietly -- DISCHARGED WHEN a paid-this-turn
//   figure is published, either as an upstream `UiSideView` field carrying `accrueIncome`'s
//   return or as an `FStratBridge` method routing it. At that point this arm becomes
//   `IncomePaid`, `Amount` becomes that field verbatim, and the rate-voiced copy obligation
//   above is DELETED rather than moved. Until then a receipt-voiced income toast is not
//   buildable from this tree and this file does not pretend otherwise.
//
// THE REPAIR ARM ARRIVES PRE-MEASURED AND THIS FILE DOES NO HP ARITHMETIC, ADDED 2026-09-01,
// AND THE REASON IT IS SHAPED THAT WAY IS THE MOST USEFUL THING IN THIS BLOCK.
// `StratTransientReceipts.cpp` opens with a standing claim that there is exactly ONE
// subtraction in it -- the `KillFame` amount -- and that "the header's argument for why the
// one subtraction is allowed does not extend to a second one." AN HP DELTA IS THAT SECOND
// SUBTRACTION AND THE CLAIM IS RIGHT TO FORBID IT HERE. The `KillFame` argument turns on
// `fameCombat` being a MONOTONE ACCUMULATOR with one cause: it only ever rises, and only
// under §2.7's kill award, so a rise between two refreshes has exactly one meaning. HP HAS
// NONE OF THOSE PROPERTIES. It falls under §2.6 combat, it rises under §2.7 repair, and it
// arrives fresh at `hpMax` on a §2.7 spawn -- so a rise measured between two PRESENTATION
// REFRESHES is a number with three possible causes and this layer cannot say which it saw.
// A widget drawing "+3 HP — repaired" off it would be asserting a rules fact nobody
// measured. The forbidding claim was therefore obeyed rather than amended, and it is still
// true of the .cpp: the repair arm's `Amount` is COPIED, character for character with the
// income arm's verbatim assignment.
//
// SO THE ARITHMETIC WENT DOWN A LAYER TO WHERE IT IS SOUND, NOT SIDEWAYS TO WHERE IT IS
// QUIET. `FStratBridge::RepairsAtTurnOpen` measures each unit's HP across the SINGLE
// `strat::applyCommand` call that opens a turn, and inside that bracket the only statement
// in the rules module that writes `GameUnit::hp` is `strat::openTurn`'s repair loop. That
// makes the rise a §2.7 repair BY CONSTRUCTION rather than by inference -- the property this
// layer cannot have, at the only layer that can have it. `FStratRepairApplication` carries
// the whole argument and the verification behind it; nothing of it is restated on this side,
// because a copy of an argument is a copy that can go stale.
//
// AND IT IS THE SAME MOVE `FStratFactoryView::bBuildPulse` ALREADY MADE, one struct along:
// an answer THE BRIDGE COMPOSED, copied whole across the boundary, with the derivation
// declared at its origin instead of disguised as a mirror at its destination.
//
// WHY IT IS AN INPUT PARAMETER AND NOT A FIELD OF `FStratViewModel`. The view model is a
// COMPLETE STATEMENT OF STEADY STATE and its own header forbids a "changed" flag, an event
// or a dirty set anywhere in it. A list of what just healed is an EDGE wearing a value's
// clothes, and the alternative shape -- `FStratViewModel::TurnRepairs` -- would have put the
// first event list in the model this whole file exists to keep out of it. It comes in
// alongside the model instead, into the struct that is already declared to be where edges
// live.
//
// THE POSITIVE CASE ONLY, AND A ZERO IS NOT EVIDENCE OF ANYTHING. No entry means "no rise
// was measured", which is indistinguishable from a unit at full HP, a unit off an owned
// objective, and a unit whose repair §2.7 BLOCKED because an enemy stood adjacent. §2.11.6's
// `enemy adjacent` one-shot needs precisely that third reading and this file does not
// provide it and must not be read as providing it -- it is blocked on the same missing
// module-side predicate that got the repair-eligibility PIP cut, and the pip's cutting is
// why this receipt exists at all.
//
// WORLD-FREE DECIDERS, ON `AStratShellHUD::DecideMenuTiming`'S PRECEDENT AND FOR ITS REASON.
// Everything that decides is a free function over plain reflected values -- no subsystem
// pointer, no `UWorld`, no bridge, no viewport. A clause plants an `FStratReceiptMark` and
// an `FStratViewModel` and asserts on the answer; nothing it needs is behind PIE. What is
// left over at the call site is one call and one re-mark, in
// `UStratMatchSubsystem::ApplyView`.
//
// WHY THE MARK IS A VALUE AND NOT A CACHE INSIDE THIS MODULE. The previous reading has to
// live somewhere, and putting it in a file-static or a singleton would make the answer
// depend on process history -- two clauses in one suite would contaminate each other and a
// second match in one session would inherit the first one's fame. The mark is a plain struct
// the CALLER owns, so its lifetime is the caller's match and a test's mark is the test's
// alone.
//
// WHAT IS DELIBERATELY NOT IN IT.
//   - NO VICTIM IDENTITY. §2.11.3's "Destroys Tank" needs the unit that died, and the only
//     place that exists is `FStratCombatOutcome`, which `StratBridge.h` states the bridge
//     "does not retain" and whose delivery mechanism it defers by name to a separate ruling
//     (`bridge_event_list`). Reaching for it here would be this lane opening that ruling.
//     DISCHARGED WHEN that ruling lands and the bridge retains outcomes; the kill arm then
//     grows a `VictimUnitId` and this paragraph goes.
//   - NO PER-KILL GRANULARITY, and it is a consequence of the above rather than a separate
//     choice. Two kills between two marks are ONE receipt carrying their summed Fame,
//     because a durable counter cannot say how many steps it took. The AI turn is where this
//     bites -- playback steps commands one at a time, so a refresh between steps splits them
//     and a refresh only at the end does not.
//   - NO TIMING, NO DURATION, NO ANIMATION AND NO QUEUE. How long a toast lingers and how
//     two toasts stack are `Content/` decisions about a widget, and a duration in seconds
//     compiled into this module would be a presentation constant no artist can reach.
//   - NO STRINGS. Every field is a number or an enumerator. §2.11.2 states no copy for these,
//     and `FStratBuildOptionView::Reason` records what inventing UI copy in a projection
//     costs.
#pragma once

#include "CoreMinimal.h"

#include "StratTransientReceipts.generated.h"

struct FStratViewModel;

/**
 * Which §2.11.2 / §2.11.6 receipt this is.
 *
 * EVERY ARM NAMES ITS QUANTITY AND NOT ITS OCCASION. `IncomeRate` is a rate per the header
 * block above; `KillFame` is Fame and not a body count, per the "Destroyed row" paragraph;
 * `UnitRepair` is HP restored to ONE named unit and not a side-wide total. A reader who
 * takes any of them at its everyday-English meaning draws the wrong number, which is why
 * none is spelled that way.
 *
 * TWO ARMS ARE ABOUT A SIDE AND ONE IS ABOUT A UNIT, which is the split
 * `FStratReceiptView::UnitId` exists to record. A drawing layer that branched on `Side`
 * alone would put a repair floater on a scoreboard.
 */
UENUM(BlueprintType)
enum class EStratReceiptKind : uint8
{
	/** §2.7's STANDING income rate for the side whose turn just began. Equal to
	 *  `FStratSideView::IncomePerTurn` verbatim -- never a subtraction, never an accrual. */
	IncomeRate UMETA(DisplayName = "Income rate"),

	/** The rise in `FStratSideView::FameCombat` -- the scoreboard's "Destroyed" row -- since
	 *  the mark. Fame, in Fame's units. */
	KillFame UMETA(DisplayName = "Kill fame"),

	/**
	 * §2.11.6's REPAIR RECEIPT: HP restored to ONE unit by §2.7's start-of-turn repair.
	 * `UnitId` names it and is never `INDEX_NONE` on this arm.
	 *
	 * COPY ON THIS ARM IS THE GDD'S, VERBATIM: `+[N] HP — repaired`, where `[N]` is
	 * `FStratReceiptView::Amount`. Amount-voiced copy is correct here -- unlike
	 * `IncomeRate`, this number is HP that was actually restored, on the turn the receipt
	 * carries. See the `Amount` tooltip, which states the obligation where a UMG author
	 * binding the pin will actually meet it.
	 *
	 * IT SAYS NOTHING ABOUT A UNIT THAT DID NOT HEAL. No receipt for a unit means no rise
	 * was measured, and that does NOT distinguish full HP from a blocked repair. See the
	 * header block's "positive case only" paragraph before drawing anything off an absence.
	 */
	UnitRepair UMETA(DisplayName = "Unit repair")
};

/**
 * One unit's §2.7 start-of-turn repair, as the bridge measured it. THE REFLECTED MIRROR OF
 * `FStratRepairApplication`, field for field, and an INPUT to this file rather than an
 * output of it.
 *
 * IT IS A MIRROR AND NOT A SECOND MEASUREMENT, on `FStratFactoryView::bBuildPulse`'s
 * precedent: the bridge composed the answer, this struct carries it across the module
 * boundary, and nothing on this side re-asks. `FStratRepairApplication` in
 * `Source/StratBridge/StratBridge.h` is the authority for every field below -- WHY the
 * amount may be subtracted there, why the join is by unit id, and what a missing entry does
 * and does not mean. That argument is deliberately not copied here; a copy of an argument is
 * a copy that can go stale while the original moves.
 *
 * REFLECTED HERE RATHER THAN IN THE BRIDGE because `StratBridge.h` must declare no
 * `USTRUCT` -- UHT would then parse the vendored `strat` headers it includes. The plain
 * original and this mirror are the same arrangement `FStratFactoryBuildPulse` /
 * `FStratFactoryView` already uses.
 */
USTRUCT(BlueprintType)
struct FStratUnitRepairView
{
	GENERATED_BODY()

	/** `FStratRepairApplication::UnitId`. Keys into `FStratViewModel::Units` by
	 *  `FStratUnitView::Id` -- the same identity the unit actors are reconciled on. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	int32 UnitId = INDEX_NONE;

	/** `FStratRepairApplication::Side`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	int32 Side = INDEX_NONE;

	/** `FStratRepairApplication::HpBefore`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	int32 HpBefore = 0;

	/** `FStratRepairApplication::HpAfter`. The unit's durable HP, which is on screen anyway. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	int32 HpAfter = 0;

	/** `FStratRepairApplication::HpMax`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	int32 HpMax = 0;

	/** `FStratRepairApplication::Amount` -- `HpAfter - HpBefore`, subtracted BY THE BRIDGE
	 *  and copied through unchanged. Strictly positive: an entry exists only where HP rose. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	int32 Amount = 0;
};

/**
 * §2.11.2's turn banner, in the VIEWER's terms.
 *
 * VIEWER-RELATIVE AND RESOLVED HERE, never in the widget, on `EStratScoreColumn`'s stated
 * reason: the side-index -> you/enemy mapping is done once, by the code that was handed the
 * seat, so no surface can perform it differently. `FStratMatchView::SideToMove` is the rules
 * module's answer about the match and `FStratViewModel::ViewingSide` is the caller's
 * statement about whose screen this is; in hot-seat the two differ every other turn, and
 * `FStratMatchView`'s own comment names conflating them as the bug.
 *
 * `None` IS A REAL STATE AND NOT A NULL. A concluded match has no turn to announce, and
 * §2.11.4's end-of-match screen is the surface that speaks then. A banner left reading
 * "ENEMY TURN" over a finished match would be asserting a turn that no longer exists.
 */
UENUM(BlueprintType)
enum class EStratTurnBanner : uint8
{
	/** No turn to announce: the match has a result, or nothing has been observed yet. */
	None UMETA(DisplayName = "None"),

	/** `SideToMove` equals the seat this model was rendered for. */
	YourTurn UMETA(DisplayName = "Your turn"),

	/** `SideToMove` is some other side. */
	EnemyTurn UMETA(DisplayName = "Enemy turn")
};

/**
 * One receipt that fired at one refresh.
 *
 * `Amount` IS THE ONLY NUMBER A WIDGET MAY DRAW, and `DurableBefore` / `DurableAfter` are
 * the audit surface beside it -- exactly the split `FStratScoreboardRow` makes between its
 * raw integers and its composed text, and for the same reason. A widget that subtracted the
 * two itself would be doing the arithmetic this struct exists to have already done.
 */
USTRUCT(BlueprintType)
struct FStratReceiptView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	EStratReceiptKind Kind = EStratReceiptKind::IncomeRate;

	/** The side this receipt belongs to -- an index into `FStratViewModel::Sides`, never a
	 *  you/enemy answer. The banner is where the viewer-relative mapping happens; a receipt
	 *  can be about the other side (they were paid too) and must stay able to say so. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	int32 Side = INDEX_NONE;

	/**
	 * The unit this receipt is about, or `INDEX_NONE` when it is about a side.
	 *
	 * `INDEX_NONE` ON `IncomeRate` AND `KillFame`, AND THAT IS A STATEMENT RATHER THAN A
	 * DEFAULT NOBODY SET. Neither has a unit: income is a side's rate, and the kill arm's own
	 * "NO VICTIM IDENTITY" paragraph records that the unit which died is not available to
	 * this file at all. A future kill arm that grows a `VictimUnitId` MUST NOT reuse this
	 * field for it -- this one names the unit the receipt is DRAWN ON, and a victim is not
	 * drawn on.
	 *
	 * NEVER `INDEX_NONE` ON `UnitRepair`. It keys into `FStratViewModel::Units` by
	 * `FStratUnitView::Id`, which is what lets a floater find the actor to sit above without
	 * this struct carrying a hex that would be a second copy of a fact that moves.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	int32 UnitId = INDEX_NONE;

	/** `FStratMatchView::Turn` as read on the refresh that fired this. Carried so a log line
	 *  or a clause can place the receipt without holding the model that produced it. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	int32 Turn = 0;

	/**
	 * The number to draw.
	 *
	 *   `IncomeRate` -- equal to `DurableAfter`, which is `FStratSideView::IncomePerTurn`.
	 *                   NOT a difference. `DurableBefore` is the rate at the mark and is
	 *                   present only so a clause can see that it was not subtracted.
	 *   `KillFame`   -- equal to `DurableAfter - DurableBefore` over
	 *                   `FStratSideView::FameCombat`, and strictly positive by construction:
	 *                   a receipt is only emitted when the field ROSE.
	 *   `UnitRepair` -- HP restored to the unit named by `UnitId`, equal to
	 *                   `FStratUnitRepairView::Amount` COPIED. `DurableBefore` /
	 *                   `DurableAfter` are that unit's HP either side of the turn opening,
	 *                   and `DurableAfter` is `FStratUnitView::Hp` -- the number the unit's
	 *                   own readout is already showing.
	 *
	 * THE COPY OBLIGATION RIDES ON THE `IncomeRate` ARM, AND IT IS REPEATED IN THIS TOOLTIP
	 * RATHER THAN LEFT IN THE HEADER BLOCK BECAUSE THIS IS WHERE A UMG AUTHOR LANDS. The
	 * content lane works through the editor and sees this pin, its type and this text -- not
	 * the prose at the top of the file -- so an obligation stated only there is one the
	 * author binding the pin will never read.
	 *
	 *   COPY DRAWN OFF AN `IncomeRate` RECEIPT MUST BE RATE-VOICED: "+8 / turn", NEVER
	 *   "+8 Fame".
	 *
	 *   COPY DRAWN OFF A `UnitRepair` RECEIPT IS THE GDD's, VERBATIM: `+[N] HP — repaired`,
	 *   with `[N]` this field. AN EM DASH, NOT A HYPHEN. Amount-voiced is CORRECT here and
	 *   the `IncomeRate` obligation above does not carry across: this number is HP that was
	 *   actually restored on the turn `Turn` names, so there is no turn on which "+3 HP" is
	 *   the wrong sentence.
	 *
	 *   AND IT SAYS NOTHING ABOUT UNITS WITH NO RECEIPT. Do not draw "no repair", "blocked",
	 *   or "enemy adjacent" off an absence -- an absence cannot tell those apart from a unit
	 *   already at full HP. §2.11.6's `enemy adjacent` one-shot is a separate, still-blocked
	 *   piece of work and this receipt is not it.
	 *
	 * WHY IT MATTERS, briefly. The number is §2.7's STANDING RATE, and this file deliberately
	 * does NOT suppress the receipt on turn 1 -- Q8(a) pays 0 then, and transcribing that
	 * rule into the presentation layer is precisely what the header block refuses to do. So
	 * amount-voiced copy is FALSE ON TURN 1, while rate-voiced copy is true on every turn
	 * including that one. The `KillFame` arm carries no such obligation: its number is Fame
	 * that was actually awarded, and amount-voiced copy is correct for it.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	int32 Amount = 0;

	/** The durable home's reading at the mark. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	int32 DurableBefore = 0;

	/** The durable home's reading on the refresh that fired this. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	int32 DurableAfter = 0;
};

/**
 * The previous reading of every durable home this file watches. The caller's value.
 *
 * `bSeeded` IS THE WHOLE OF THE FIRST-OBSERVATION RULE, and it is a flag rather than an
 * inference from the contents because every field here has a legal value that a real match
 * also holds -- fame 0 and income 0 are both reachable, and a side with neither is
 * indistinguishable from a mark nobody ever wrote. This project already carries a defect of
 * exactly that species, a real default that could not signal unset, so the flag records THE
 * WRITE rather than its contents.
 *
 * AN UNSEEDED MARK EMITS NOTHING, and that is not merely defensive. The first model a session
 * ever applies has no predecessor, so every durable home would read as having risen from zero
 * -- a match resuming from a save would open with a kill toast for the whole war's
 * accumulated combat Fame. There is no edge at the first observation because an edge is a
 * relation between two readings and only one exists.
 */
USTRUCT(BlueprintType)
struct FStratReceiptMark
{
	GENERATED_BODY()

	/** False until `StratMarkFromView` has written this. See the struct doc. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	bool bSeeded = false;

	/** `FStratMatchView::Turn` at the mark. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	int32 Turn = 0;

	/** `FStratMatchView::SideToMove` at the mark. The turn-start edge is a change in the PAIR
	 *  (Turn, SideToMove), never in either alone: hot-seat moves the side within a turn, and
	 *  a turn rolls over with the side unchanged in a one-sided scenario. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	int32 SideToMove = INDEX_NONE;

	/**
	 * `FStratMatchView::bHasResult` and `FStratViewModel::ViewingSide` at the mark.
	 *
	 * THE MARK RECORDS ALL THREE OF THE BANNER'S INPUTS, NOT JUST THE ONE THE RECEIPTS NEED,
	 * and the first spelling of this struct carried only `SideToMove` -- which broke
	 * `bBannerJustChanged` in two directions at once. Recomputing the previous banner with
	 * the CURRENT result flag makes a concluded match report a change on every refresh
	 * forever, because the old side always maps to a turn arm and the new one always maps to
	 * `None`; and forcing the flag to false hides the one transition that matters, the
	 * refresh on which the match ends. The seat is the same shape -- `SetViewingSide` flips
	 * the banner with no rules field moving at all, and a mark that did not record it would
	 * call that no change.
	 *
	 * SO THE RULE IS: whatever `StratDecideTurnBanner` reads, this struct stores. A fourth
	 * input added to that function without a field here reintroduces the same class of bug.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	bool bMatchHadResult = false;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	int32 ViewingSide = INDEX_NONE;

	/** `FStratSideView::FameCombat` per side, indexed as `FStratViewModel::Sides` is. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	TArray<int32> FameCombat;

	/** `FStratSideView::IncomePerTurn` per side, indexed as `FStratViewModel::Sides` is. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	TArray<int32> IncomePerTurn;
};

/**
 * Everything §2.11.2's transient layer should be showing right now.
 *
 * THE BANNER IS STEADY STATE AND THE RECEIPTS ARE EDGES, IN ONE STRUCT ON PURPOSE. They have
 * different lifetimes -- the banner is a function of the current model alone and would be
 * identical if recomputed from scratch every frame, while `Receipts` is empty on any refresh
 * where nothing moved. Splitting them would give a drawing layer two things to fetch in one
 * pass for no benefit; keeping them together makes it structurally obvious that the banner
 * survives an empty receipt list, which is the failure mode of treating both as events.
 */
USTRUCT(BlueprintType)
struct FStratTransientReceipts
{
	GENERATED_BODY()

	/** What fired at this refresh. EMPTY IS THE COMMON CASE and is not an error. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	TArray<FStratReceiptView> Receipts;

	/** §2.11.2's banner, viewer-relative. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	EStratTurnBanner Banner = EStratTurnBanner::None;

	/**
	 * `FStratMatchView::SideToMove` as the banner was decided from, or `INDEX_NONE` on
	 * `None`. Carried so a clause can assert WHICH side the viewer-relative answer was
	 * resolved against instead of re-deriving it from a model it would then be testing
	 * against itself.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	int32 BannerSide = INDEX_NONE;

	/**
	 * True when the banner changed at this refresh -- the turn-start edge, in the banner's
	 * own terms.
	 *
	 * IT IS NOT A DIRTY BIT AND THE BANNER IS DRAWABLE WITHOUT IT. `Banner` is complete on
	 * its own; this says only whether an ENTRANCE animation is due. A widget that hid the
	 * banner when this is false would be turning steady state into an event, which is the one
	 * thing the header block asks this file not to enable.
	 *
	 * FALSE ON AN UNSEEDED MARK, because "changed" is a relation between two readings and the
	 * first observation has only one -- the same rule the receipts run on.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Transient")
	bool bBannerJustChanged = false;
};

// ---------------------------------------------------------------------------
// THE DECIDERS. World-free, so a clause can reach every arm without a viewport.
// ---------------------------------------------------------------------------

/**
 * Reads every durable home this file watches out of `Model`, and returns a seeded mark.
 *
 * TOTAL AND NEVER REFUSING: a model with no sides yields a seeded mark with empty arrays,
 * which is the correct statement that nothing was observable, not a failure. A refusal would
 * have to be reported to a caller that has no other course than to store it anyway.
 */
STRATUI_API FStratReceiptMark StratMarkFromView(const FStratViewModel& Model);

/**
 * §2.11.2's banner from the two fields it follows, plus the result flag that silences it.
 *
 * TAKES THREE SCALARS RATHER THAN THE MODEL so that a clause can drive the `None` arm without
 * constructing a concluded match, and so that the mapping is testable at values
 * `strat::buildUiSnapshot` cannot currently produce. `bMatchHasResult` is
 * `FStratMatchView::bHasResult`, which that field's own comment says is READ from the module
 * and never inferred from `ResultTier` -- so this function must not infer it either.
 *
 * A NEGATIVE `SideToMove` IS `None`. `FStratMatchView` defaults it to 0, which is a real
 * side, so the sentinel a caller can hand in is the negative one; the seeded-mark rule above
 * is what actually keeps an unobserved match quiet.
 */
STRATUI_API EStratTurnBanner StratDecideTurnBanner(int32 SideToMove,
                                                   int32 ViewingSide,
                                                   bool bMatchHasResult);

/**
 * Compares `Model`'s durable homes against `Mark` and fills `OutReceipts`.
 *
 * OVERWRITES `OutReceipts` ENTIRELY, INCLUDING EMPTYING IT. The output describes THIS refresh
 * and never accumulates -- a caller that wants a history keeps one, and this function is not
 * it. That is the same "complete statement, never a delta" discipline `ApplyView` runs on,
 * applied one layer up.
 *
 * THE RULES IT IMPLEMENTS, in the order they are applied:
 *   1. The banner is decided from `Model` alone and is filled on EVERY call, seeded mark or
 *      not. `bBannerJustChanged` needs the mark and is false when there is none.
 *   2. An unseeded mark emits NO receipts. See `FStratReceiptMark`'s doc.
 *   3. Per side, in `Sides` order: a `KillFame` receipt when `FameCombat` ROSE. Sides present
 *      in the model but absent from the mark are skipped -- a side that appeared between two
 *      marks has no previous reading, which is rule 2 again at per-side granularity.
 *   4. One `IncomeRate` receipt for the side now to move, and only when the pair
 *      (Turn, SideToMove) changed. It is emitted AFTER the kill receipts so that a refresh
 *      carrying both reads in the order the turn actually ran: the kills happened on the turn
 *      that is ending, the income belongs to the one beginning.
 *   5. One `UnitRepair` receipt per entry in `TurnRepairs`, on THE SAME (Turn, SideToMove)
 *      EDGE as rule 4 and after it, because §2.7 repairs the turn that is beginning.
 *
 * WHY THE REPAIR ARM IS GATED ON THE TURN EDGE AND NOT ON THE MARK'S CONTENTS, which is the
 * one place this arm's shape differs from the other two and is worth reading before changing
 * it. `TurnRepairs` is not a durable home and has no reading at the mark: it is a RECORD the
 * bridge replaces at each turn opening and serves unchanged for the whole turn
 * (`FStratBridge::RepairsAtTurnOpen` says so in terms). So a mid-turn refresh would re-emit
 * the identical list on every mouse move if this arm fired unconditionally. The edge is what
 * makes it fire once, and it is the SAME edge rather than a second one so that the income
 * receipt and the repair receipts can never disagree about whether a turn began.
 *
 * `TurnRepairs` IS COPIED, NEVER RECOMPUTED, AND THIS FILE PERFORMS NO HP ARITHMETIC. See
 * the header block's own paragraph on the arm, and the .cpp's file block, which still states
 * truthfully that there is exactly one subtraction in it.
 *
 * A VIEWER-RELATIVE FILTER IS DELIBERATELY NOT APPLIED, and hot-seat is the case that
 * settles it. §2.7 repairs the ACTIVE side alone, so on the opponent's turn every entry is
 * theirs; filtering to `ViewingSide` here would make the arm silent on exactly half of all
 * turns. That is not this file's call to make: `FStratReceiptView::Side` is declared to be
 * "an index into `FStratViewModel::Sides`, never a you/enemy answer", precisely so a receipt
 * CAN be about the other side and stay able to say so, and `EStratTurnBanner` is the one
 * place the viewer-relative mapping happens. `IncomeRate` already behaves this way -- it
 * fires for whichever side is now to move, viewer or not. Whether a hot-seat screen SHOWS
 * the opponent's repair floaters is a drawing decision with both facts in hand; suppressing
 * them here would take that decision away and hide it in a decider.
 *
 * A FALL IN `FameCombat` EMITS NOTHING AND IS NOT REPORTED AS A FAULT. The field only accrues
 * under §2.7, so a fall means the mark and the model describe different matches -- a reseed,
 * or a load -- and the caller resets the mark on both of those paths. Treating it as an error
 * here would put a diagnosis in a decider that cannot see which of the two happened.
 */
STRATUI_API void StratDecideTransientReceipts(const FStratReceiptMark& Mark,
                                              const FStratViewModel& Model,
                                              const TArray<FStratUnitRepairView>& TurnRepairs,
                                              FStratTransientReceipts& OutReceipts);

/**
 * The three-argument call: the same decision with NOTHING HAVING BEEN REPAIRED.
 *
 * IT EXISTS SO THAT "NO REPAIRS" IS SPELLABLE WITHOUT CONSTRUCTING AN EMPTY ARRAY AT EVERY
 * CALL SITE, and it is an overload rather than a default argument because a default would
 * let a caller that SHOULD be passing repairs silently pass none -- which is the shape of
 * failure this whole file is written against, an always-empty list being indistinguishable
 * from a quiet turn. Naming the short form makes the omission deliberate and greppable.
 *
 * `UStratMatchSubsystem::ApplyView` DOES NOT USE IT. It passes the bridge's answer. This form
 * is for callers that have no bridge -- a clause about the banner or about the two
 * side-scoped arms, which is what every existing one is.
 */
STRATUI_API void StratDecideTransientReceipts(const FStratReceiptMark& Mark,
                                              const FStratViewModel& Model,
                                              FStratTransientReceipts& OutReceipts);
