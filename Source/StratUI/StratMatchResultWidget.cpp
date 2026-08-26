// GDD §2.11.4 -- the end-of-match screen's model builder and its authored copy.
//
// THE WHOLE OF THIS FILE'S JOB, repeated here because this is where it is possible to
// break it: every number that reaches `FStratMatchResultModel` is copied from one field of
// one of TWO projections -- `StratBuildMatchResult`'s and `StratBuildScoreboardModel`'s --
// and nothing below sums, counts, differences or scales anything. The three judgements
// made here are all COMPARISONS or LOOKUPS over already-decided values: which faction a
// side is, which of six authored lines a result selects, and whether the viewer is the
// winner. Each is stated in the header as belonging to the screen; none is a rules answer.
//
// THIS FILE DOES NOT ASK THE BRIDGE ANYTHING ITSELF. It calls two `STRATUI_API` builders
// that do, and it names no `strat::` symbol -- so the vendored include below exists only to
// satisfy the by-reference `FStratBridge` parameter it forwards.
//
// WHY THE BUILDER WRITES INTO A LOCAL AND ASSIGNS LAST, on `StratBuildScoreboardModel`'s
// stated reasoning: a result screen half-filled by a transient refusal would state a tier
// beside somebody else's rows, and building into a local makes all-or-nothing structural
// rather than careful.

#include "StratMatchResultWidget.h"

// The forbidden-in-the-header include. `StratMatchResultWidget.h` declares reflected types,
// so UHT parses it and `StratBridge.h` may not appear there; it appears here, where UHT
// never looks. See that header's block and `StratScoreboardWidget.cpp`, which is arranged
// the same way for the same reason.
#include "StratBridge.h"
#include "StratUI.h"

EStratFaction SideFaction(int32 Side)
{
	// THE HARNESS PIN. Side 0 -> Directorate, side 1 -> Vanguard, matching `kb/setting.md`'s
	// "Faction A" / "Faction B" ordering. THE GDD DOES NOT MAKE THIS BINDING and neither
	// does the rules module -- see the header block. A `switch` rather than an array index
	// so that the total is visible and an out-of-range side cannot read past anything.
	switch (Side)
	{
	case 0:
		return EStratFaction::Directorate;
	case 1:
		return EStratFaction::Vanguard;
	default:
		// `INDEX_NONE` (which `strat::SIDE_NONE` equals -- checked in `StratViewModel.cpp`
		// rather than assumed) and anything else. A draw's winner arrives here, and
		// `None` is the right answer for it rather than a fallback.
		return EStratFaction::None;
	}
}

FText StratResultLineFor(EStratResultTier Tier, EStratResultCause Cause, int32 Winner)
{
	// THE SIX STRINGS BELOW ARE §2.11.4's OWN BULLETS, character for character. They are
	// content and are quoted, never composed -- see the header for the three `kb/setting.md`
	// constraints they satisfy and the count of the longest.
	switch (Tier)
	{
	case EStratResultTier::Draw:
		// THE CAUSE IS READ HERE AND NOWHERE ELSE IN THIS FUNCTION. §2.11.4 lists two draw
		// samples and `EStratResultCause` supplies the discriminator: `PassivityGuard` IS
		// §2.8's mutual-passivity draw, which is the one the scoreboard's
		// `bNoEngagements` row has been warning about all match. Every other cause that can
		// reach a Draw tier -- `AllKeysTied` in practice -- is the neutral attrition draw.
		if (Cause == EStratResultCause::PassivityGuard)
		{
			return FText::FromString(
				TEXT("Turn cap reached. Neither side engaged. Recorded as a draw."));
		}
		return FText::FromString(
			TEXT("Turn cap reached. Attrition equal. Recorded as a draw."));

	case EStratResultTier::Decisive:
		// CHOSEN BY THE WINNER'S FACTION AND NEVER BY THE VIEWER'S. See the header: the two
		// agree in single-seat play and disagree on exactly the hot-seat turn where the
		// loser is looking at the screen.
		switch (SideFaction(Winner))
		{
		case EStratFaction::Directorate:
			return FText::FromString(
				TEXT("Command directive fulfilled. The enemy flag is struck from the record. Order is restored."));
		case EStratFaction::Vanguard:
			return FText::FromString(
				TEXT("Their flag is down. We hold the ground. That's the whole report."));
		default:
			// A DECIDED TIER WITH NO WINNING SIDE. Unreachable through the rules module,
			// representable in these engine types; mapped rather than asserted on, and
			// empty rather than given words that would be a lie about who won.
			return FText::GetEmpty();
		}

	case EStratResultTier::Marginal:
		switch (SideFaction(Winner))
		{
		case EStratFaction::Directorate:
			return FText::FromString(
				TEXT("The cap is reached. The ledger favors the Directorate. The record stands."));
		case EStratFaction::Vanguard:
			return FText::FromString(
				TEXT("Cap hit. We did the damage; they held the rear. The ground says we win."));
		default:
			return FText::GetEmpty();
		}

	case EStratResultTier::InProgress:
	default:
		// `InProgress` HAS NO LINE, AND AN EMPTY ONE IS THE HONEST ANSWER rather than a
		// placeholder a widget might render -- `DirectiveTextFor`'s own ruling about
		// `None`, made here for the same reason. `UStratMatchSubsystem::GetMatchResult` is
		// explicit that it does not require the match to have ended, so this value is a
		// caller error and not an impossible state.
		return FText::GetEmpty();
	}
}

FText StratResultTierTextFor(EStratResultTier Tier)
{
	// §2.11.4's THREE WORDS AND NO OTHERS. Not `StaticEnum<>()->GetDisplayNameText()`, which
	// would promote the UMETA DisplayNames on `EStratResultTier` into shipped copy by
	// accident -- those belong to the details panel and must stay free to differ.
	switch (Tier)
	{
	case EStratResultTier::Decisive:
		return FText::FromString(TEXT("Decisive"));
	case EStratResultTier::Marginal:
		return FText::FromString(TEXT("Marginal"));
	case EStratResultTier::Draw:
		return FText::FromString(TEXT("Draw"));
	case EStratResultTier::InProgress:
	default:
		return FText::GetEmpty();
	}
}

bool StratBuildMatchResultModel(const FStratBridge&     Bridge,
                                int32                   ViewingSide,
                                FStratMatchResultModel& OutModel,
                                FString&                OutFailureReason)
{
	OutFailureReason.Reset();

	FStratMatchResultModel Built;

	// ---- §2.8's result whole -------------------------------------------------
	// ASKED FIRST so that a refusal names the RESULT rather than the rows. Both builders
	// refuse an unseeded bridge in the same words, so the order does not change what is
	// said -- it changes which layer a reader goes looking at, and "there is no verdict" is
	// the more specific of the two on this screen.
	FStratMatchResultView Result;
	if (!StratBuildMatchResult(Bridge, Result, OutFailureReason))
	{
		return false;
	}

	// ---- §2.11.4's three rows, from the LIVE PANEL'S OWN BUILDER --------------
	// NOT COMPOSED HERE. See the header: "the same three rows in the same order" is
	// expressed as identity with the scoreboard's model, not as agreement with it, so
	// §2.8's tiebreak order has exactly one implementation on screen.
	if (!StratBuildScoreboardModel(Bridge, ViewingSide, Built.Scoreboard, OutFailureReason))
	{
		return false;
	}

	// ---- Copies, with no transformation --------------------------------------
	Built.Tier         = Result.Tier;
	Built.Cause        = Result.Cause;
	Built.Winner       = Result.Winner;
	Built.DecidedByKey = Result.DecidedByKey;
	Built.ViewingSide  = ViewingSide;

	// ---- Lookups over already-decided values ---------------------------------
	Built.TierText      = StratResultTierTextFor(Result.Tier);
	Built.ResultLine    = StratResultLineFor(Result.Tier, Result.Cause, Result.Winner);
	Built.WinnerFaction = SideFaction(Result.Winner);

	// §2.8's key as a row tag. THE ONE INDEX SHIFT IN THIS FILE, and it is here rather than
	// in the WBP precisely because it is arithmetic: §2.8 numbers its keys 1, 2, 3 and
	// `EStratScoreCriterion` numbers the same three criteria 0, 1, 2. A graph doing this
	// subtraction is the widget-side arithmetic T-UI-03 forbids; a `switch` doing it here is
	// a mapping between two vocabularies that both already exist.
	switch (Result.DecidedByKey)
	{
	case 1:
		Built.bHasDecidedBy      = true;
		Built.DecidedByCriterion = EStratScoreCriterion::CombatFame;
		break;
	case 2:
		Built.bHasDecidedBy      = true;
		Built.DecidedByCriterion = EStratScoreCriterion::Objectives;
		break;
	case 3:
		Built.bHasDecidedBy      = true;
		Built.DecidedByCriterion = EStratScoreCriterion::SurvivingHp;
		break;
	default:
		// 0 IS "NO TIEBREAK RAN" AND IS THE COMMON CASE, not an error: a flag kill ends a
		// match without evaluating a key at all. `DecidedByCriterion` is left at its default
		// and `bHasDecidedBy` says not to read it.
		break;
	}

	// ---- The three comparisons the model exists to have already made ----------
	// `bIsDraw` IS READ OFF THE TIER AND NOT OFF `Winner == INDEX_NONE`. The two agree, and
	// they agree for a reason that would stop being true if `InProgress` were ever drawn:
	// an in-progress result also carries `INDEX_NONE`, and calling that a draw would be the
	// screen announcing an outcome the match has not reached.
	Built.bIsDraw     = (Result.Tier == EStratResultTier::Draw);
	Built.bViewerWon  = (Result.Winner != INDEX_NONE && Result.Winner == ViewingSide);
	Built.bViewerLost = (Result.Winner != INDEX_NONE && Result.Winner != ViewingSide);

	// ALL-OR-NOTHING, ON THE LAST LINE. See the header block.
	OutModel = MoveTemp(Built);
	return true;
}

void UStratMatchResultWidget::PushMatchResult(const FStratMatchResultModel& InModel)
{
	// UNCONDITIONAL, WITH NO EQUALITY TEST AND NO BRANCH ON THE VALUE. This function does
	// not read `Tier`, does not show or hide itself, and decides nothing about appearance --
	// `UStratGuidanceWidget::PushGuidance`'s discipline exactly. Visibility is the WBP's,
	// bound to the fields; a `RemoveFromParent` here would make this class a second opinion
	// about when a match is over.
	Model = InModel;

	OnMatchResultRefreshed(Model);
}
