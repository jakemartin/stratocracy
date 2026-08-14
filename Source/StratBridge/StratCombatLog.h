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
//     ruling -- `StratBridge.h:35-45` already disclaims the event list and says landing
//     the command log does not close it. This struct is the payload such a mechanism
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
		/** The forecast said the attack was ILLEGAL and the rules applied it anyway, or the
		 *  forecast query was refused for an attack that then applied. Either way the two
		 *  halves of §2.6 disagree about whether the act was even available. */
		LegalityDisagrees = 1 << 2,
	};
}
