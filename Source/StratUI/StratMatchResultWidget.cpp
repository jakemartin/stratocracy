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

bool StratScoreCriterionForKey(int32 Key, EStratScoreCriterion& OutCriterion)
{
	switch (Key)
	{
	case 1:
		OutCriterion = EStratScoreCriterion::CombatFame;
		return true;
	case 2:
		OutCriterion = EStratScoreCriterion::Objectives;
		return true;
	case 3:
		OutCriterion = EStratScoreCriterion::SurvivingHp;
		return true;
	default:
		// 0 IS "NO TIEBREAK RAN" AND IS THE COMMON CASE, not an error: a flag kill ends a match
		// without evaluating a key at all. Anything else is a key §2.8 does not have. Both
		// arms leave `OutCriterion` untouched -- see the header on why the caller's default
		// beats a sentinel, every enumerator of `EStratScoreCriterion` naming a real row a
		// display could accidentally mark.
		return false;
	}
}

void StratComposeMatchResultModel(const FStratMatchResultView& Result,
                                  const FStratScoreboardModel& Scoreboard,
                                  int32                        ViewingSide,
                                  FStratMatchResultModel&      OutModel)
{
	FStratMatchResultModel Built;

	// ---- §2.11.4's three rows, TAKEN WHOLE ------------------------------------
	// NOT COMPOSED HERE and not composed by the caller either -- see the header: "the same
	// three rows in the same order" is expressed as identity with the scoreboard's model,
	// not as agreement with it, so §2.8's tiebreak order has exactly one implementation on
	// screen. This function only carries it across.
	Built.Scoreboard = Scoreboard;

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

	// §2.8's key as a row tag. THE ONE INDEX SHIFT ON THIS SCREEN, and it is neither here
	// nor in the WBP -- it is `StratScoreCriterionForKey`, a module-side seam a clause can call
	// with all four classes of input. §2.8 numbers its keys 1, 2, 3 and `EStratScoreCriterion`
	// numbers the same three criteria 0, 1, 2; a graph doing that subtraction is the widget-side
	// arithmetic T-UI-03 forbids, and the switch that used to sit inline in the bridge-taking
	// builder was the same mapping with two of its three arms unreachable from any test.
	//
	// FALSE LEAVES `DecidedByCriterion` AT ITS DEFAULT, which is the same disposition the inline
	// switch had for key 0: `bHasDecidedBy` is what says not to read the tag.
	Built.bHasDecidedBy = StratScoreCriterionForKey(Result.DecidedByKey, Built.DecidedByCriterion);

	// ---- The three comparisons the model exists to have already made ----------
	// `bIsDraw` IS READ OFF THE TIER AND NOT OFF `Winner == INDEX_NONE`. The two agree, and
	// they agree for a reason that would stop being true if `InProgress` were ever drawn:
	// an in-progress result also carries `INDEX_NONE`, and calling that a draw would be the
	// screen announcing an outcome the match has not reached.
	Built.bIsDraw     = (Result.Tier == EStratResultTier::Draw);
	Built.bViewerWon  = (Result.Winner != INDEX_NONE && Result.Winner == ViewingSide);
	Built.bViewerLost = (Result.Winner != INDEX_NONE && Result.Winner != ViewingSide);

	// ALL-OR-NOTHING, ON THE LAST LINE. See the header block. It matters as much here as in
	// the bridge-taking builder even though nothing above can fail: the caller hands us the
	// model it is going to show, and a half-filled one states a tier beside somebody else's
	// rows.
	OutModel = MoveTemp(Built);
}

bool StratBuildMatchResultModel(const FStratBridge&     Bridge,
                                int32                   ViewingSide,
                                FStratMatchResultModel& OutModel,
                                FString&                OutFailureReason)
{
	OutFailureReason.Reset();

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
	FStratScoreboardModel Board;
	if (!StratBuildScoreboardModel(Bridge, ViewingSide, Board, OutFailureReason))
	{
		return false;
	}

	// ---- EVERYTHING ELSE IS `StratComposeMatchResultModel`'s, AND THAT IS THE POINT ----
	// This function is now exactly "ask the bridge twice, then compose", and the composition
	// -- every copy, every lookup, and the key-to-criterion tag -- lives in a function that
	// takes VALUES. A clause can therefore hand-author an `FStratMatchResultView` carrying
	// `DecidedByKey = 2` and drive the real composition with it, which no bridge this suite
	// can build will ever produce: keys 2 and 3 need a capped match in which both sides
	// fought to an EQUAL combat Fame. THAT IS THE WHOLE REASON THE SPLIT EXISTS. Before it,
	// the tag for two of §2.8's three keys was reachable by a player and by no test.
	//
	// ALL-OR-NOTHING SURVIVES THE SPLIT AND IS NOW MORE LEGIBLY SO -- NOT STRONGER. The
	// pre-image composed into a LOCAL too, so a refusing build never touched `OutModel` then
	// either; the guarantee is unchanged. What improved is that a reader no longer has to
	// notice that `Built` was a local: the composer's signature makes the wholesale assignment
	// structural. Corrected on `strat-integration-reviewer`'s ruling 1 after this comment first
	// claimed the contract had got stronger.
	StratComposeMatchResultModel(Result, Board, ViewingSide, OutModel);
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
