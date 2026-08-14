// GDD §2.6 / §4.9 -- the combat outcome record and the bridge's log category.
//
// WHAT GAP THIS CLOSES. `applyCommand`'s Attack case returns `okResult(1)` and nothing
// else (`Source/StratRules/Replay.good.cpp:413-470`): no damage, no death, no counter, no
// fame. So after an attack applies, the only in-engine witness that anything happened at
// all was the `STRAT-CMD accepted ... hash=` line, which says a command landed and says
// nothing about what it did. This header declares the record that carries what it did.
//
// TWO SOURCES, AND THE RECORD CARRIES BOTH ON PURPOSE. `strat::uiForecast` (`Ui.h:335-350`)
// is what the rules PREDICT, captured before the submit; the HP / roster / fame movement
// read off two `strat::UiSnapshot`s is what the rules DID. `Ui.h:346` states that the
// forecast is produced by `Combat.h::resolveDamage` and `defenderCanCounter` and by
// nothing else, "so 'the forecast is exactly what resolves' is structural rather than
// asserted" -- and until this record existed nothing in the engine ever compared the two
// across a real submit. `bForecastAgrees` / `DivergenceMask` are that comparison.
//
// WHY IT IS A PLAIN STRUCT AND NOT A `USTRUCT`. Every member is an engine type, and the
// struct names no `strat::` type anywhere -- so this header is safe to include from a
// header that declares a `UCLASS` / `USTRUCT` / `UENUM`, which `StratBridge.h` is
// permanently not (it includes the vendored rules headers, and UHT must never parse
// those). A later phase can therefore hand an `FStratCombatOutcome` to `StratPlay` -- to
// an actor, to a widget-facing view model -- without that phase first having to solve the
// UHT problem. Making it reflected NOW would have bought nothing: nothing draws it yet,
// and a `USTRUCT` here would need a `.generated.h`, which would make this header
// UHT-parsed and re-open the very question it was shaped to avoid.
//
// WHAT IS IRREVERSIBLE. The field NAMES in the emitted line are the interface, not this
// struct. Once a gate greps `STRAT-COMBAT resolved` or reads `agree=0`, renaming those
// tokens breaks it silently -- the same standing obligation `STRAT-CMD accepted` carries.
// Add fields at the end of the line; do not re-spell the ones already there.
//
// WHAT IS DELIBERATELY NOT IN IT.
//   - No accessor on `FStratBridge`. The bridge assembles one of these per attack and
//     emits it; it does not retain the last one. A caller that needs to READ an outcome
//     needs a delivery mechanism (an event list, a return channel) that is a separate
//     ruling -- `StratBridge.h`'s "NOT IN THIS ROUND" / "THE RECORDED LOG BELOW IS NOT
//     THAT EVENT LIST" blocks already disclaim the event list and say landing the command
//     log does not close `bridge_event_list`. (Cited by block name rather than by the
//     `:35-45` this comment first carried, for the reason the `Forecast` citation two
//     screens down records: a line range written during a diff is invalidated by that
//     same diff.) This struct is the payload such a mechanism
//     would carry, landed early because assembling it is the hard part.
//   - No `strat::UiForecast` by value. The forecast's seven fields are flattened into
//     engine scalars here precisely so this header stays free of the vendored headers.
//   - No damage FORMULA. Every predicted number is `uiForecast`'s and every measured
//     number is a snapshot difference. The only arithmetic in the emitter is the
//     agreement comparison itself, which is a diagnostic and decides no outcome.
#pragma once

#include "CoreMinimal.h"

/** Bridge-layer log category. Separate from `LogStratPlay` and `LogStratUI` for the reason
 *  those two are separate from each other: a forecast/resolution divergence is a RULES-facing
 *  fault with a rules-facing fix, and reading it in the gameplay module's channel would put
 *  it beside actor reconciliation failures that have a different owner entirely.
 *
 *  IT IS `StratBridge`-ONLY, AND THAT IS A TRAP THIS HEADER OTHERWISE INVITES. Everything
 *  else in this file was shaped to CROSS a module boundary -- `FStratCombatOutcome` names no
 *  `strat::` type precisely so a later phase can hand one to `StratPlay`. The category is the
 *  one thing here that does not travel. `DECLARE_LOG_CATEGORY_EXTERN` with no `_API` macro
 *  emits a category object with no dllexport, and an editor target is a modular build, so a
 *  `UE_LOG(LogStratBridge, ...)` written in `StratPlay` or `StratUI` COMPILES CLEANLY AND
 *  THEN FAILS TO LINK -- the `LNK2019` shape this project has measured 8x and records at the
 *  top of `StratBridge.h`.
 *
 *  DELIBERATELY NOT FIXED WITH AN EXPORT MACRO. No such call site exists today, and widening
 *  the exported surface for a caller nobody has written is how a module boundary stops meaning
 *  anything. If a later phase genuinely needs to log into this channel from another module,
 *  the choice is `STRATBRIDGE_API` on this line -- made then, with the caller in hand, and
 *  written down -- and not now on speculation. Until then: the struct crosses, the category
 *  does not. */
DECLARE_LOG_CATEGORY_EXTERN(LogStratBridge, Log, All);

/**
 * The outcome of one attack that reached `strat::applyCommand`, forecast and measured.
 *
 * SENTINEL DISCIPLINE, and it is the same one `STRAT-CMD` set in phase 4: every field is
 * numeric, and `INDEX_NONE` (-1) means "this field has no value here" rather than zero.
 * Zero is a real damage number and a real fame delta; -1 is not. The consequence is that
 * the emitted line's SHAPE never depends on its content, so a parser splits on a fixed
 * field count and never on which fields happened to apply.
 *
 * WHICH FIELDS CAN BE -1 AND WHY:
 *   - every `Defender*` field, when no unit stood on the target hex before the submit
 *     (a refusal path -- there is nothing to measure and no id to name);
 *   - `*HpAfter`, when that unit left the roster (there is no "after" HP for a dead unit;
 *     `bDefenderDied` / `bAttackerDied` is where that fact lives);
 *   - every `Forecast*` field, when the forecast query itself was refused -- which per
 *     `FStratBridge::Forecast`'s "TWO CHANNELS" block means the bridge was handed an
 *     attacker id that is not on the board, NOT that the attack was illegal. An illegal
 *     attack is `bForecastLegal` false with the other fields populated. (Cited by
 *     FUNCTION NAME rather than line number on purpose: the first spelling of this
 *     comment said `StratBridge.h:392-399`, and the same diff that wrote it inserted 24
 *     lines above the target and left the citation pointing at `Reachable` instead.)
 */
struct FStratCombatOutcome
{
	// ---- Identity -------------------------------------------------------
	/** The attacking unit id, as submitted. */
	int32 AttackerId = INDEX_NONE;
	/** The unit standing on the target hex BEFORE the submit, or -1 if none did. Resolved
	 *  from the pre-submit snapshot and never from the post-submit one, because the whole
	 *  point of an attack is that the second may not contain it. */
	int32 DefenderId = INDEX_NONE;
	/** The submitted target hex. X = q, Y = r, the project's one encoding. */
	FIntPoint TargetHex = FIntPoint(INDEX_NONE, INDEX_NONE);
	/** The live turn number and active side, READ BEFORE the submit -- the same reason
	 *  `StratSubmitSelectionCommand` reads them before rather than after. */
	int32 Turn = INDEX_NONE;
	int32 ActiveSide = INDEX_NONE;
	/** The attacker's OWN side, off the pre-submit snapshot. Normally equal to
	 *  `ActiveSide`; kept separate because the fame fields below are that side's and a
	 *  reader should not have to assume the two agree to know whose fame moved. */
	int32 AttackerSide = INDEX_NONE;

	// ---- Forecast: what the rules predicted, before the submit -----------
	/** True when `FStratBridge::Forecast` answered at all (see the class doc above for the
	 *  difference between "could not ask" and "the rules said no"). */
	bool  bForecastQueried = false;
	/** `strat::UiForecast::legal` -- the rules' answer, not the query's success. */
	bool  bForecastLegal = false;
	int32 ForecastDistance = INDEX_NONE;
	int32 ForecastDamage = INDEX_NONE;
	bool  bForecastDefenderDies = false;
	bool  bForecastCounterFires = false;
	int32 ForecastCounterDamage = INDEX_NONE;

	// ---- Measured: what the state actually did ---------------------------
	bool  bApplied = false;
	int32 AttackerHpBefore = INDEX_NONE;
	int32 AttackerHpAfter = INDEX_NONE;
	int32 AttackerHpMax = INDEX_NONE;
	bool  bAttackerDied = false;
	int32 DefenderHpBefore = INDEX_NONE;
	int32 DefenderHpAfter = INDEX_NONE;
	int32 DefenderHpMax = INDEX_NONE;
	bool  bDefenderDied = false;
	/** The ATTACKER side's fame, before and after. Two fields each rather than a delta,
	 *  because a delta is arithmetic and these are mirrors of `strat::UiSideView`. */
	int32 AttackerFameTotalBefore = INDEX_NONE;
	int32 AttackerFameTotalAfter = INDEX_NONE;
	int32 AttackerFameCombatBefore = INDEX_NONE;
	int32 AttackerFameCombatAfter = INDEX_NONE;

	// ---- Agreement -------------------------------------------------------
	/** 1 when every clause held, 0 when at least one did not, -1 when the comparison could
	 *  not be made at all -- the attack did not apply, or a snapshot projection was refused
	 *  underneath the measurement. THREE STATES AND NOT TWO, deliberately: an unmeasurable
	 *  attack reported as `agree=0` would put a fault in the log that nobody can reproduce,
	 *  and reported as `agree=1` would claim a comparison that never ran. */
	int32 ForecastAgrees = INDEX_NONE;
	/** Bitmask of the clauses that did NOT hold. Zero on agreement, and zero also when
	 *  `ForecastAgrees` is -1 -- read the two together, never the mask alone. */
	int32 DivergenceMask = 0;
};

/** The clauses `FStratCombatOutcome::DivergenceMask` reports.
 *
 *  Deliberately a bitmask of NAMED clauses rather than one boolean: "the forecast was
 *  wrong" is not actionable, "the counter's damage was wrong but the defender's was right"
 *  points at `defenderCanCounter` and not at `resolveDamage`. */
namespace EStratCombatDivergence
{
	enum Type : int32
	{
		None = 0,
		/** The defender's measured loss or death did not match `damage` / `defenderDies`. */
		DefenderLoss = 1 << 0,
		/** The attacker's measured loss or death did not match `counterFires` /
		 *  `counterDamage`. */
		CounterLoss = 1 << 1,
		/** The forecast said the attack was ILLEGAL and the rules applied it anyway: the two
		 *  halves of §2.6 disagreeing about whether the act was even available.
		 *
		 *  ONE ARM, NOT TWO. `StratDivergenceMaskOf` sets this bit on `!bForecastLegal`
		 *  alone; it makes no `bForecastQueried` test. An earlier spelling of this comment
		 *  claimed a second arm -- "or the forecast query was refused for an attack that
		 *  then applied" -- that no code implements and that no code CAN reach through the
		 *  emitter: a refused query is precisely one of the three conditions on which
		 *  `CaptureAfter` returns early, leaving `ForecastAgrees` at -1 without calling the
		 *  mask function at all. That is the same fact `StratDivergenceMaskOf`'s stated
		 *  PRECONDITION expresses from the other side.
		 *
		 *  UNREACHABLE THROUGH `Submit`, AND NOT BY ACCIDENT. `strat::uiForecast` has SIX
		 *  refusal arms, and they do not all close for the same reason -- an earlier spelling
		 *  of this comment listed five of them under the word "every", which is the kind of
		 *  false universal a doc pass exists to remove. Counted off `uiForecast` itself, but
		 *  DELIBERATELY REORDERED -- grouped by why each arm closes, which is not the order
		 *  they appear in, so that the five that close together are read together:
		 *    - "no such unit" (1st in source), "no unit on that hex" (3rd), "a unit cannot
		 *      attack itself" (4th), "same side" (5th), "out of range" (6th) -- FIVE
		 *      ILLEGALITIES OF THE ACT, each refused by `applyCommand` too, so a
		 *      forecast-illegal attack never applies;
		 *    - "no tables" (`w.unitDefs == nullptr || w.terrain == nullptr`) -- SECOND in
		 *      source, listed last here because it is the odd one out: not an illegality of
		 *      the act at all, but a malformed `UiWorld`, and it has no `applyCommand`
		 *      counterpart to be mirrored by. It closes on a different and stronger fact:
		 *      `FStratBridge::MakeUiWorld` assigns `W.unitDefs = &Units` and
		 *      `W.terrain = &Terrain`, the addresses of two by-value members, which cannot
		 *      be null. No bridge-side state produces this arm.
		 *  So the bit never rises on a `STRAT-COMBAT resolved` line. It is reachable only by
		 *  handing a hand-built `FStratCombatOutcome` to `StratDivergenceMaskOf` directly,
		 *  which is exactly the seam the block below this enum exists to provide. Read this
		 *  as the detector's own coverage, not as a fault the game is expected to produce. */
		LegalityDisagrees = 1 << 2,
	};
}

// ---------------------------------------------------------------------------
// The divergence rule, promoted out of `StratBridge.cpp` so it can be FALSIFIED.
//
// WHAT GAP THIS CLOSES. `DivergenceMask`'s non-zero arms and `ForecastAgrees == 0`
// were unreachable from any test, and not for want of trying. Phase 2's gate measured
// it against the vendored sources: `strat::uiForecast` and `strat::applyCommand` reach
// the same `resolveDamage` / `defenderCanCounter` over the same stat blocks, so a
// forecast-illegal attack that nonetheless applies is not CONSTRUCTIBLE through the
// bridge -- 74 Attacks driven, zero divergences, which is the right number and also
// no evidence at all that the detector works. A clause that cannot fail pins nothing.
//
// THAT 74 IS A PROPERTY OF ONE CORPUS, NOT A CONSTANT. It is the Attack count in
// `Data/parity_fixture.save` (169 commands, 74 of them Attacks), probed 2026-08-13 and
// re-checked at run time by `Source/StratBridge/Tests/StratCombatOutcomeParity.cpp`
// against the fixture's own entries. A re-emitted fixture moves it; the "zero
// divergences" half is the load-bearing claim and does not depend on the count.
//
// The remedy is a seam, not a foil. Rather than inventing a fake rules module that
// disagrees with itself -- which would pin the fake and not the rule -- the mask
// computation is lifted to a free function over a hand-buildable `FStratCombatOutcome`.
// A test writes a deliberately wrong `DefenderHpAfter` into a struct it owns and reads
// the mask straight back. No submit, no snapshot, no `strat::` anything.
//
// THERE IS EXACTLY ONE COPY OF THE RULE, and that is the point of doing it this way
// rather than by giving the test its own reimplementation. `CaptureAfter` CALLS these;
// it does not keep a parallel copy. If these two functions and the emitter ever drift,
// the drift is a compile error or a behaviour change, never a silent second opinion.
//
// NO `_API` DECORATION, DELIBERATELY -- the same ruling the `LogStratBridge` block
// above makes for the same reason. Both call sites are in-module: the emitter in
// `Source/StratBridge/StratBridge.cpp` and the clauses in `Source/StratBridge/Tests/`,
// which UBT compiles into `UnrealEditor-StratBridge.dll` alongside it. An undecorated
// extern function links across translation units within one DLL; it is only a
// cross-MODULE call that becomes the `LNK2019` this project measured 8x. Adding
// `STRATBRIDGE_API` here would widen the exported surface for a caller nobody has
// written. If one is ever written -- an outcome handed to `StratPlay` that the gameplay
// module then wants to re-score -- the decoration is the fix, made then, with the caller
// in hand.
//
// WHAT IS DELIBERATELY NOT IN HERE. No measurability guard. The three-state
// `ForecastAgrees` (-1 for "the comparison could not be made") is a property of the
// CAPTURE -- it asks whether a forecast was queried and whether a defender and an
// attacker HP were resolvable off the snapshots -- and it stays with the capture in
// `CaptureAfter`. `StratDivergenceMaskOf` answers only "given these numbers, which
// clauses failed", and its precondition is stated on it. Folding the guard in here
// would make the mask function unable to distinguish "agreed" from "unmeasurable",
// which is precisely the two-state collapse `ForecastAgrees`' doc comment refuses.
// ---------------------------------------------------------------------------

/**
 * Did one combatant lose what it was predicted to lose?
 *
 * A DEAD UNIT HAS NO "AFTER" HP, so death and damage cannot be checked by the same
 * comparison. When both sides agree the unit died, the only thing left to check is
 * that the predicted blow was at least large enough to empty the pool -- the excess
 * is not observable anywhere and asserting on it would be inventing a clause.
 *
 * `HpBefore` / `HpAfter` carry `FStratCombatOutcome`'s sentinel discipline: `INDEX_NONE`
 * means "no value here", and a surviving unit with an unmeasurable HP disagrees by
 * construction rather than comparing -1 against a real number.
 */
bool StratLossAgrees(int32 HpBefore, int32 HpAfter, bool bDied, int32 ExpectedDamage, bool bExpectDeath);

/**
 * Which `EStratCombatDivergence` clauses did NOT hold for this outcome.
 *
 * Returns `EStratCombatDivergence::None` (0) when every clause held. PRECONDITION: the
 * outcome is measurable -- `bForecastQueried` is true, `DefenderId` is not `INDEX_NONE`,
 * and `AttackerHpBefore` is not `INDEX_NONE`. Called on an outcome that fails any of
 * those, the return value is meaningless rather than wrong; `CaptureAfter` is what
 * decides measurability, and it leaves `ForecastAgrees` at -1 instead of calling this.
 */
int32 StratDivergenceMaskOf(const FStratCombatOutcome& Outcome);
