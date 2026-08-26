// GDD §2.11.4 (end-of-match screen) / §2.8 / kb/setting.md -- the verdict surface.
//
// WHAT GAP THIS CLOSES. `StratScoreboardWidget.h`'s "NOT IN THIS ROUND" block named this
// file in advance and said why it could not be part of that class: "§2.11.4's end-of-match
// screen. It is a different surface with a different lifetime, and its faction-voiced
// result line is the one place faction voice appears (kb/setting.md's pipeline note).
// Mixing it into the persistent scoreboard's class would give that text a lifetime it must
// not have." This is that separate class. Until it existed, `FStratMatchResultView` was
// projected by `StratBuildMatchResult`, routed by `UStratMatchSubsystem::GetMatchResult`,
// and read by nothing: §2.8 had a winner and no screen could say who it was.
//
// THE ORDER OF THE SCREEN IS THE GDD'S AND IS NOT A LAYOUT PREFERENCE. §2.11.4: "the tier
// first (§2.8 -- Decisive / Marginal / Draw), then the same three rows in the same order,
// so the verdict is always a restatement of what was on screen all match. Beneath the tier,
// one faction-voiced result line." `FStratMatchResultModel` below carries the tier, then
// the rows, then the line, and a WBP that reordered them would be contradicting the
// sentence rather than restyling it.
//
// THE THREE ROWS ARE THE SCOREBOARD'S OWN MODEL, EMBEDDED WHOLE, and that is the single
// most load-bearing decision in this file. `FStratScoreboardModel Scoreboard` is filled by
// `StratBuildScoreboardModel` -- the same function, on the same bridge, that the live panel
// uses -- rather than by three rows composed here. The alternative was a private
// `FStratResultRow` array with its own labels and its own tiebreak ordering, and what
// killed it is that "the same three rows in the same order" would then be TWO statements of
// one order that agree today. §2.11.4 exists because revision §1.5-#1 forbids the tiebreak
// being a hidden win condition; a verdict screen that spelled the tiebreak a second way
// would be the hiding place reopening one layer up.
//
// ZERO WIDGET-SIDE ARITHMETIC, WHICH IS T-UI-03's CLAUSE APPLIED TO A SECOND SURFACE.
// Every number and every string the WBP shows is already a field below: the tier's display
// text, the ≤30-word result line, which criterion decided a capped match, and whether the
// viewer won. Notably `bViewerWon` is a FIELD and not a `Winner == ViewingSide` node in a
// graph -- that comparison is the one §2.11.4 gets wrong most cheaply, and
// `FStratMatchResultView::Winner`'s own block says the comparison "belongs to the screen
// that draws it", which means this file and not the asset.
//
// WHY THE LINE CHOOSER LIVES HERE AND NOT BESIDE `StratGuidedOpening::DirectiveTextFor`.
// That function is the precedent -- authored player-facing copy is a C++ static returning
// `FText`, never a string typed into a `.uasset` -- and this file follows it exactly.
// It could not follow it into the same FILE: `DirectiveTextFor` is in `StratPlay`, the
// arrow is `StratPlay -> StratUI`, and the model built below needs the line, so a chooser
// in `StratPlay` would be a call this module cannot make. The copy moves to where the
// surface is; the shape does not change.
//
// THE FACTION BINDING IS OURS AND IS NOT THE GDD's. Nothing in §2.11.4, in `kb/setting.md`,
// in the scenario JSON or in the vendored rules binds a `strat` side index to a faction --
// the rules module has sides, the setting has factions, and no file joins them. `SideFaction`
// below pins side 0 to the Directorate and side 1 to the Vanguard, matching `kb/setting.md`'s
// "Faction A -- The Directorate" / "Faction B -- The Vanguard" ordering. It is a HARNESS PIN
// on `AStratScoreboardHUD::FirstSide`'s line exactly: that property is configured rather than
// constant because `Replay.h` states no rule in this project decides who moves first, and this
// is the same species of thing -- a decision the content and the rules both declined to make,
// made here so a screen can be drawn, and labelled so a reader can see whose decision it was.
// IF THE GDD EVER BINDS SIDES TO FACTIONS, THIS FUNCTION MUST DELEGATE and not merely agree.
//
// THE VOICE IS CHOSEN BY THE WINNER AND NEVER BY THE VIEWER. `SideFaction(Result.Winner)`,
// never `SideFaction(ViewingSide)`. The two agree in single-seat play and disagree on exactly
// the hot-seat turn where the loser is looking at the screen -- and a Vanguard-voiced line
// shown because the Directorate happened to be the camera is the defect this sentence exists
// to make greppable. `ViewingSide` is still read, once, for the YOU/ENEMY column assignment
// on the three rows, which is a different use and is `StratBuildScoreboardModel`'s anyway.
//
// NOT IN THIS ROUND, with reasons:
// - Any copy for `DecidedByKey`. The screen carries the KEY as a criterion tag
//   (`DecidedByCriterion`) so the WBP can mark the deciding row, and no sentence naming it,
//   because the GDD has not written one. `FStratMatchResultView::DecidedByKey`'s own block
//   refuses to name the keys for the same reason; inventing words here would give §2.8's
//   criteria a second vocabulary.
// - A rematch or quit button. Those are commands, and this class holds a model and draws
//   nothing. The WBP wires its own buttons to `UStratMatchSubsystem`'s reflected entry
//   points, which exist already.
// - Localization. `FText::FromString` throughout, matching `DirectiveTextFor`; see
//   `FStratGuidanceView`'s block on why localization is deliberately not started.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "StratScoreboardWidget.h"
#include "StratViewModel.h"

#include "StratMatchResultWidget.generated.h"

// Forward declaration only. This header declares reflected types, so UHT parses it, and
// `StratBridge.h` may never appear above -- see `StratScoreboardWidget.h`'s block on the
// same constraint, which this file inherits unchanged.
class FStratBridge;

/**
 * Which faction's voice a result line is written in.
 *
 * THREE ENUMERATORS FOR TWO FACTIONS, and `None` is load-bearing rather than defensive:
 * §2.11.4's draw lines are "neutral system voice" in the GDD's own words, so a draw has no
 * faction and the value that says so must be a real state. It is also what an out-of-range
 * side maps to, so a caller never has to treat a faction as possibly-wrong.
 *
 * IT IS NOT A `strat` CONCEPT AND MUST NOT BECOME ONE. The rules module has sides and knows
 * nothing about factions; `kb/setting.md` has factions and knows nothing about side indices.
 * `SideFaction` is the only join, and this file's header block records that the join is ours.
 */
UENUM(BlueprintType)
enum class EStratFaction : uint8
{
	None        UMETA(DisplayName = "Neutral"),
	Directorate UMETA(DisplayName = "The Directorate"),
	Vanguard    UMETA(DisplayName = "The Vanguard")
};

/**
 * §2.11.4's harness pin: which faction speaks for a `strat` side.
 *
 * SIDE 0 IS THE DIRECTORATE AND SIDE 1 IS THE VANGUARD, matching `kb/setting.md`'s
 * Faction A / Faction B ordering. THIS BINDING IS THIS PROJECT'S AND IS NOT A RULE -- see
 * the header block. Anything outside those two indices, `INDEX_NONE` included, is `None`.
 *
 * A FUNCTION AND NOT A `TMap`, so that the total is visible at the call site and an unmapped
 * index cannot be a lookup that quietly returns a default-constructed value.
 */
STRATUI_API EStratFaction SideFaction(int32 Side);

/**
 * §2.11.4's faction-voiced result line, quoted verbatim from the GDD's six samples.
 *
 * THE SIX STRINGS ARE CONTENT AND ARE COPIED, NOT COMPOSED. They are §2.11.4's own bullets
 * character for character, including the full stops and the apostrophe in "That's".
 * `kb/setting.md` supplies all three constraints on them -- faction voice appears only in
 * result-screen text, a result line is ≤ 30 words, and the register is field-manual plain
 * with *destiny, glory, honor, legend, forever, epic, heroic, sacred, doom* named as the
 * banned register. The longest of the six is 15 words, so the budget is met with room, and
 * none of the nine banned words appears in any of them.
 *
 * TOTAL OVER EVERY REACHABLE `(Tier, Cause, Winner)`, and the totality is the point rather
 * than a courtesy. The mapping is:
 *   Draw + PassivityGuard              -> "Neither side engaged"  (the mutual-passivity draw)
 *   Draw + any other cause             -> "Attrition equal"       (the neutral system voice)
 *   Decisive / Marginal + Directorate  -> that faction's line for that tier
 *   Decisive / Marginal + Vanguard     -> that faction's line for that tier
 *   InProgress                         -> empty
 *   Decisive / Marginal + no faction   -> empty
 * THE LAST TWO ROWS ARE WHY THIS RETURNS AN `FText` AND NOT A GUARANTEE. `InProgress` is a
 * value `UStratMatchSubsystem::GetMatchResult` legitimately returns -- its declaration is
 * explicit that it is a LIVE query and does not require the match to have ended -- so a
 * screen built from an in-progress result is a caller error and not an impossible state,
 * and an empty line is the honest answer rather than a placeholder a widget might render.
 * That is `DirectiveTextFor`'s own ruling about `None`, made here for the same reason.
 * The final row is unreachable through the rules module, which never reports a decided tier
 * with `SIDE_NONE`; it is representable in these engine types, so it is mapped rather than
 * asserted on.
 *
 * THE CAUSE IS READ ONLY TO SPLIT THE TWO DRAWS. `EStratResultCause`'s own block says it is
 * "this mapping's INPUT rather than its output" and that `strat::causeName` is deliberately
 * not routed, because the words belong to the screen. This is that mapping, and it uses the
 * cause for exactly one discrimination -- the one §2.11.4 lists two draw samples for.
 */
STRATUI_API FText StratResultLineFor(EStratResultTier Tier, EStratResultCause Cause, int32 Winner);

/**
 * §2.8's tier as the word §2.11.4 puts at the top of the screen.
 *
 * "Decisive", "Marginal", "Draw" -- the GDD's three words and no others. `InProgress` is
 * empty, on `StratResultLineFor`'s reasoning: it is a state the screen must never be shown
 * in, and a fourth word would be this file inventing copy for it.
 *
 * NOT `StaticEnum<>()->GetDisplayNameText()`, which would make the UMETA DisplayNames on
 * `EStratResultTier` into shipped player-facing copy by accident. Those exist for the
 * details panel; these are §2.11.4's, and the two must be free to differ.
 */
STRATUI_API FText StratResultTierTextFor(EStratResultTier Tier);

/**
 * §2.8's key number as the criterion tag §2.11.4's screen marks a row with.
 *
 * THE SEAM EXISTS SO THE MAPPING CAN BE PINNED, and that is its whole justification. §2.8
 * numbers its tiebreak keys 1, 2, 3; `EStratScoreCriterion` numbers the same three criteria
 * 0, 1, 2. While this switch lived inline in `StratBuildMatchResultModel`, keys 2 and 3 were
 * unreachable from any clause the suite can write: they need a capped match in which both
 * sides fought to an EQUAL combat Fame, and every game §2.9's AI plays on the shipped
 * scenario ends `Decisive / FlagDestroyed` with `decidedByKey == 0`. A wrong mapping for
 * those two keys therefore shipped GREEN -- the screen would mark the wrong row and every
 * number on it would still be right. `Tools/architect/state/tests.md` named this function as
 * the discharge before it existed.
 *
 * A `switch` AND NOT `Key - 1`, for T-UI-03's reason one layer down: the shift is arithmetic
 * between two vocabularies that both already exist, and a cast off an int would turn every
 * key §2.8 does not have into a real row of the scoreboard.
 *
 * @param Key           `FStratMatchResultView::DecidedByKey`, in §2.8's own 1-based
 *                      numbering.
 * @param OutCriterion  written ONLY when the answer is true, so the caller's own default
 *                      survives a key of 0 -- which is the common case, a flag kill having
 *                      evaluated no key at all, and not an error.
 * @return true exactly when `Key` names one of §2.8's three keys.
 */
STRATUI_API bool StratScoreCriterionForKey(int32 Key, EStratScoreCriterion& OutCriterion);

/**
 * Everything the end-of-match screen draws, in one value.
 *
 * A SNAPSHOT OF A SNAPSHOT, held by value, for `FStratScoreboardModel`'s stated reason: the
 * bridge produces `strat::UiSnapshot` per call and warns that borrowed world data must not
 * outlive it, so nothing here is a reference into rules memory.
 */
USTRUCT(BlueprintType)
struct FStratMatchResultModel
{
	GENERATED_BODY()

	// ---- 1. The tier, first ------------------------------------------------

	/** §2.8's tier, copied from `FStratMatchResultView::Tier` and not re-derived. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Result")
	EStratResultTier Tier = EStratResultTier::InProgress;

	/** The word the screen puts at the top. See `StratResultTierTextFor`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Result")
	FText TierText;

	// ---- 2. The same three rows, in the same order --------------------------

	/**
	 * §2.11.4's three §2.8 rows, built by `StratBuildScoreboardModel` -- the live panel's
	 * own builder and its own model type. See the header block: this is the whole of "the
	 * same three rows in the same order", expressed as identity rather than as agreement.
	 *
	 * ITS `ViewingSide` IS THE VIEWER'S AND ITS COLUMNS ARE YOU/ENEMY, which is the one
	 * place this screen legitimately reads the viewing side. The VOICE above and below is
	 * still the winner's.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Result")
	FStratScoreboardModel Scoreboard;

	/**
	 * Which §2.8 criterion decided a capped match, so the WBP can mark that row without
	 * mapping 1/2/3 onto 0/1/2 itself -- which is arithmetic, and the arithmetic
	 * `FStratMatchResultView::DecidedByKey` is careful to leave numbered.
	 *
	 * `bHasDecidedBy` false means no tiebreak ran (`DecidedByKey == 0`), and `DecidedByCriterion`
	 * is then not meaningful. A flag rather than a sentinel, because every value of
	 * `EStratScoreCriterion` names a real row a display could accidentally highlight.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Result")
	bool bHasDecidedBy = false;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Result")
	EStratScoreCriterion DecidedByCriterion = EStratScoreCriterion::CombatFame;

	/** Carried raw beside the tag above, for the reason `FStratScoreboardRow` carries its
	 *  values twice: the number is what a test compares against the projection, the tag is
	 *  what the screen uses. Both are set from one read of one field. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Result")
	int32 DecidedByKey = 0;

	// ---- 3. Beneath the tier, one faction-voiced result line -----------------

	/** §2.11.4's ≤30-word line. See `StratResultLineFor` for the six strings and the
	 *  totality of the mapping that picks between them. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Result")
	FText ResultLine;

	/** Whose voice `ResultLine` is in -- `None` on a draw and on an in-progress result.
	 *  Carried so the WBP can pick a colour or a sigil without re-deriving the faction from
	 *  `Winner`, which is the comparison the header block forbids the asset from making. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Result")
	EStratFaction WinnerFaction = EStratFaction::None;

	// ---- The comparisons, made once, here -----------------------------------

	/** The winning `strat` side, or `INDEX_NONE` on a draw and while in progress. Copied
	 *  from `FStratMatchResultView::Winner`, convention included. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Result")
	int32 Winner = INDEX_NONE;

	/** Which side this screen is drawn FOR. Not `SideToMove`; not the winner. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Result")
	int32 ViewingSide = 0;

	/**
	 * `Winner == ViewingSide`, computed once here so no graph computes it.
	 *
	 * THREE BOOLEANS AND NOT ONE ENUM, because a draw is not a third value of "did you win"
	 * -- it is the absence of a winner, and `bIsDraw` is true exactly when `Tier` is `Draw`.
	 * All three are false on an in-progress result, which is the correct reading of a screen
	 * that should not have been shown.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Result")
	bool bViewerWon = false;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Result")
	bool bViewerLost = false;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Result")
	bool bIsDraw = false;

	/** Why the match ended. Carried so a later surface can branch on it without a second
	 *  query; nothing in §2.11.4 renders it, and no copy exists for it -- see the header. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Result")
	EStratResultCause Cause = EStratResultCause::None;
};

/**
 * Builds the model from the bridge. THE ONLY PLACE the two projections are joined.
 *
 * A FREE FUNCTION AND NOT A METHOD ON THE WIDGET, for `StratBuildScoreboardModel`'s stated
 * reason: it draws nothing, holds no state, and can be tested with no Slate in existence.
 *
 * IT MAKES TWO CALLS AND COMPOSES NOTHING ELSE. `StratBuildMatchResult` answers §2.8's
 * result whole; `StratBuildScoreboardModel` answers §2.11.4's three rows. Everything else
 * below is a copy, a lookup in `SideFaction` / `StratResultLineFor`, or one of the three
 * equality comparisons the model exists to have already made.
 *
 * ALL-OR-NOTHING, matching both of the functions it calls: it fills a local and assigns on
 * its last line. That matters more here than anywhere: a result screen half-filled by a
 * transient refusal would state a tier beside somebody else's rows.
 *
 * REFUSES RATHER THAN PRODUCING AN EMPTY VERDICT, forwarding the refusing layer's own words
 * unchanged. `StratBuildMatchResult` refuses an unseeded bridge; it does NOT refuse an
 * in-progress match, and neither does this -- see `StratResultLineFor` on why `InProgress`
 * is mapped rather than asserted on.
 *
 * @param ViewingSide  which `strat` side is the "YOU" column. NOT the winner, and not
 *                     `sideToMove`; range checked by `StratBuildScoreboardModel`.
 */
STRATUI_API bool StratBuildMatchResultModel(
	const FStratBridge&     Bridge,
	int32                   ViewingSide,
	FStratMatchResultModel& OutModel,
	FString&                OutFailureReason);

/**
 * The composition step, over VALUES. Every copy, every lookup and §2.8's key-to-criterion tag.
 *
 * WHY IT IS SEPARATE FROM THE BUILDER ABOVE, AND IT IS A TESTABILITY SEAM RATHER THAN A DESIGN
 * PREFERENCE. `StratBuildMatchResultModel` reaches its `FStratMatchResultView` THROUGH the
 * bridge, and no bridge this suite can build reports `decidedByKey` 2 or 3 -- those need a capped
 * match in which both sides fought to an EQUAL combat Fame, where every game §2.9's AI plays on
 * the shipped scenario ends `Decisive / FlagDestroyed` at key 0. So while the composition lived
 * inside the builder, the tag for TWO OF §2.8's THREE KEYS was reachable by a player and by no
 * test. `FStratMatchResultView` is a plain `USTRUCT` with a public defaulted `DecidedByKey`, so a
 * clause can hand-author one and drive this function with it. THAT is what the split buys.
 *
 * IT TAKES THE SCOREBOARD RATHER THAN BUILDING IT, for the header block's reason: "the same three
 * rows in the same order" is an IDENTITY with the live panel's model, so this function carries
 * that model across and composes no row of its own.
 *
 * TOTAL AND INFALLIBLE -- no `bool`, no failure reason. Every input is already a value somebody
 * else refused or produced; there is nothing left here that can be asked and declined. It still
 * assigns `OutModel` wholesale on its last line, so a caller's model is never half-filled.
 *
 * @param Result       §2.8's result whole. Hand-authorable, which is the point.
 * @param Scoreboard   §2.11.4's three rows, already built by `StratBuildScoreboardModel`.
 * @param ViewingSide  which `strat` side is the "YOU" column -- NOT the winner. Copied onto the
 *                     model and used for the two viewer comparisons; the row columns were
 *                     already assigned when `Scoreboard` was built.
 */
STRATUI_API void StratComposeMatchResultModel(
	const FStratMatchResultView& Result,
	const FStratScoreboardModel& Scoreboard,
	int32                        ViewingSide,
	FStratMatchResultModel&      OutModel);

/**
 * §2.11.4's end-of-match screen.
 *
 * Holds a built model and pushes it at Blueprint. It reads no `strat` type, chooses no
 * words, and makes no comparison -- see this file's header block for why that is the
 * acceptance criterion and not a style preference.
 *
 * Abstract and Blueprintable because the WBP_ asset derives from it and does the layout.
 * `StratUI.Build.cs` records the irreversible part: the derived asset bakes
 * `/Script/StratUI.StratMatchResultWidget` into itself, so this class's module and name are
 * expensive to change once the first asset exists.
 */
UCLASS(Abstract, Blueprintable, meta = (DisplayName = "Strat Match Result"))
class STRATUI_API UStratMatchResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Replaces `Model` and fires `OnMatchResultRefreshed`.
	 *
	 * NOT A UFUNCTION, and for `UStratGuidanceWidget::PushGuidance`'s reason rather than
	 * `UStratScoreboardWidget::Refresh`'s: every argument here is reflectable, so nothing
	 * FORCES this to be plain C++. It is plain C++ so that Blueprint has no way to write a
	 * verdict at all. Reflecting it would let a graph push a fabricated winner onto the
	 * screen, and §2.8's result is the one value in this project that must have exactly one
	 * author.
	 *
	 * VOID, AND IT CANNOT REFUSE. It is handed a finished value; the build that could be
	 * refused already happened in `StratBuildMatchResultModel`.
	 *
	 * ASSIGNS UNCONDITIONALLY, with no equality test and no early-out on "unchanged" -- the
	 * delta-shaped thinking `UStratMatchSubsystem::ApplyView` refuses, arrived at from the
	 * widget's side.
	 */
	void PushMatchResult(const FStratMatchResultModel& InModel);

	/** The last pushed model. Everything the WBP draws comes from here.
	 *
	 *  READ-ONLY TO BLUEPRINT ON PURPOSE. `PushMatchResult` is the only writer, and a
	 *  writable property would be the same second-author problem as a reflected setter. */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Result")
	FStratMatchResultModel Model;

	/**
	 * Fired after `Model` is replaced.
	 *
	 * The model is passed as an argument as well as being readable off the widget, for the
	 * reason `OnScoreboardRefreshed` and `OnGuidanceRefreshed` both give: so a graph has no
	 * motive to cache its own copy, a cached copy being a value that can go stale.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Stratocracy|Result")
	void OnMatchResultRefreshed(const FStratMatchResultModel& InModel);
};
