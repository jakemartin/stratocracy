// GDD §2.11.4 / §2.8 -- the live standings scoreboard, subject of T-UI-03.
//
// THE WHOLE OF THIS FILE'S JOB is stated in its header and repeated here because this
// is where it is possible to break it: every number that reaches `FStratScoreboardModel`
// is EQUAL TO ONE FIELD of `strat::UiSnapshot`, copied across with no transformation
// beyond a width cast. There is no `+`, no `-`, no `/`, no `.size()` over a snapshot
// vector anywhere below. If you are about to add one, the number you want belongs on
// the bridge side or in the rules module, not here -- see the header's note on why it
// may not simply become a new field of the vendored snapshot either (T-UI-05 clause
// (c)).
//
// THE ONE JUDGEMENT MADE HERE, and it is a comparison rather than arithmetic: which
// column the §2.11.4 chevron sits beside. §2.8 evaluates the tiebreak in criteria
// order and awards it to the higher value, so the answer is a walk down the rows
// asking `>`. The header records the standing debt: when the rules module exposes that
// same criteria-order comparison as a callable, `StratBuildScoreboardModel` MUST
// delegate to it, because two implementations of one tiebreak that agree today is
// exactly the drift §4.9 exists to prevent.
//
// WHY THE BUILDER WRITES INTO A LOCAL AND ASSIGNS LAST. `Refresh` promises that a
// failed refresh leaves a correct scoreboard on screen untouched -- a refresh
// attempted before seeding must not blank standings that were right a moment ago. If
// the builder wrote directly into the caller's model it would only hold that promise
// as long as every failure path happened to come before the first write, which is a
// property nobody can check by reading. Building into a local and moving it across
// makes all-or-nothing structural instead of careful.

#include "StratScoreboardWidget.h"

// The forbidden-in-the-header include, which is the reason this file exists as a
// separate translation unit at all: StratBridge.h pulls in the vendored `strat`
// headers, and StratScoreboardWidget.h declares reflected types that UHT parses. The
// two must never meet. See the header block in StratScoreboardWidget.h and the module
// note in StratUI.Build.cs.
#include "StratBridge.h"

#define LOCTEXT_NAMESPACE "StratScoreboard"

namespace
{
	// Presentation only. Grouping separators are switched OFF because §2.11.4's mock is
	// a fixed-width panel drawn top-left all match ("450", "4/8", "TURN 12 / 20"), and a
	// separator appearing the first time a side crosses a thousand combat Fame would
	// change the cell's width mid-match on a surface whose whole point is being read
	// passively without being looked at. This is formatting of one already-decided
	// integer; it is not a second opinion about its value.
	//
	// Built per call rather than held in a file-scope static: the options object is
	// trivial, and a mutable static shared by a UI path is a cache whose only possible
	// contribution is a stale one.
	FText ScoreNumber(int32 Value)
	{
		FNumberFormattingOptions Options;
		Options.SetUseGrouping(false);
		return FText::AsNumber(Value, &Options);
	}

	// Fills one §2.8 row. Takes the two values ALREADY READ from the snapshot rather
	// than taking the snapshot and a criterion, so that the field-selection decision
	// (fameCombat and not fameTotal, and so on) stays visible at the one call site
	// where a reviewer is looking for it instead of being buried in a switch here.
	//
	// `bHasTotal` carries the "X of N" shape. The rejected alternative was to treat
	// `Total == 0` as "no total", which fails on the legitimate scenario that supplies
	// no capturable objectives at all: 0/0 is a true statement about such a match and
	// a sentinel would have made it unprintable.
	FStratScoreboardRow MakeRow(
		EStratScoreCriterion Criterion,
		const FText&         Label,
		const FText&         Tooltip,
		int32                YouValue,
		int32                EnemyValue,
		bool                 bHasTotal,
		int32                Total)
	{
		FStratScoreboardRow Row;
		Row.Criterion  = Criterion;
		Row.Label      = Label;
		Row.Tooltip    = Tooltip;
		Row.YouValue   = YouValue;
		Row.EnemyValue = EnemyValue;
		Row.bHasTotal  = bHasTotal;
		Row.Total      = bHasTotal ? Total : 0;

		if (bHasTotal)
		{
			// "4/8" -- §2.11.4's mock, no spaces. Deliberately a different form from the
			// turn line's "12 / 20", which the same mock spaces out; both are copied from
			// the GDD's own rendering rather than normalised to one house form, because
			// the mock is the specification of this panel's appearance.
			const FText Format = LOCTEXT("ValueOverTotal", "{0}/{1}");
			Row.YouText   = FText::Format(Format, ScoreNumber(Row.YouValue),   ScoreNumber(Row.Total));
			Row.EnemyText = FText::Format(Format, ScoreNumber(Row.EnemyValue), ScoreNumber(Row.Total));
		}
		else
		{
			Row.YouText   = ScoreNumber(Row.YouValue);
			Row.EnemyText = ScoreNumber(Row.EnemyValue);
		}

		// Set by the criteria-order walk after all rows exist, never here: a row cannot
		// know whether an earlier criterion already decided the lead.
		Row.Chevron = EStratScoreColumn::None;
		return Row;
	}
}

bool StratBuildScoreboardModel(
	const FStratBridge&      Bridge,
	int32                    ViewingSide,
	FStratScoreboardModel&   OutModel,
	FString&                 OutFailureReason)
{
	// Cleared up front so a success cannot leave a previous call's refusal sitting in
	// the caller's string, which is a stale value of exactly the kind this whole file
	// is written to avoid.
	OutFailureReason.Reset();

	// The ONE read of the view model, and it is the bridge's answer whole. The bridge's
	// own header records why it refuses rather than handing back an empty snapshot when
	// there is nothing to project, and that reason applies with full force here: "not
	// seeded yet" and "a match where both sides have destroyed nothing" would render as
	// the same three zero rows, and only the second of them gets §2.11.4's
	// "-- no engagements --" text and the mutual-passivity draw behind it.
	strat::UiSnapshot Snapshot;
	const FStratResult SnapshotResult = Bridge.MakeUiSnapshot(Snapshot);
	if (!SnapshotResult.bOk)
	{
		// The bridge's own sentence, forwarded unchanged, as the header promises. The
		// failing-invariant `Id` is deliberately NOT concatenated in: there is one string
		// channel here, and "T-SAVE-05: <reason>" is no longer the owning module's own
		// words. A caller that wants the tag asks the bridge for it.
		OutFailureReason = SnapshotResult.Reason;
		return false;
	}

	// SIDE COUNT READ OFF THE ARRAY BEING INDEXED, not off a separately named constant.
	// `strat::SIDE_COUNT` is reachable from this file and would say the same thing
	// today; the array's own extent is used instead because it is the thing the bounds
	// check actually protects, and it cannot drift from itself.
	const int32 SideCount = static_cast<int32>(UE_ARRAY_COUNT(Snapshot.side));

	// §2.11.4's panel is TWO COLUMNS, YOU and ENEMY, and that is a statement of the
	// layout rather than of this code. With more than two sides there is no single
	// "ENEMY" column and any choice this function made would be an invention, so it
	// refuses instead. A silent `side[1]` here would have shipped a scoreboard that
	// quietly omitted a belligerent.
	if (SideCount != 2)
	{
		OutFailureReason = FString::Printf(
			TEXT("Scoreboard is a two-column YOU/ENEMY panel (GDD 2.11.4); snapshot carries %d sides."),
			SideCount);
		return false;
	}

	// NOT `sideToMove`. The snapshot carries whose turn it is; this is whose screen it
	// is, and on every other turn those differ. The caller supplies it and this function
	// only checks that it can be indexed.
	if (ViewingSide < 0 || ViewingSide >= SideCount)
	{
		OutFailureReason = FString::Printf(
			TEXT("Viewing side %d is outside the snapshot's %d sides."),
			ViewingSide, SideCount);
		return false;
	}

	// The other column, found by scanning rather than by `1 - ViewingSide`. The
	// subtraction is correct for two sides and is the kind of correct that stops being
	// correct without anything failing to compile; the scan states the intent -- "the
	// side that is not the viewer" -- and the SideCount check above is what guarantees
	// the answer is unique.
	int32 EnemySide = INDEX_NONE;
	for (int32 Index = 0; Index < SideCount; ++Index)
	{
		if (Index != ViewingSide)
		{
			EnemySide = Index;
			break;
		}
	}

	const strat::UiSideView& You   = Snapshot.side[ViewingSide];
	const strat::UiSideView& Enemy = Snapshot.side[EnemySide];

	FStratScoreboardModel Built;

	Built.ViewingSide = ViewingSide;

	// §2.11.4: "the cap is per-scenario data, so the widget reads turnCap from the
	// scenario rather than hardcoding a number". It reaches the widget through the
	// snapshot, which is the projection of the scenario the bridge seeded from -- the
	// "/ 20" of the mock is Ferrum Crossing's cap and appears nowhere in this file.
	Built.Turn    = static_cast<int32>(Snapshot.match.turn);
	Built.TurnCap = static_cast<int32>(Snapshot.match.turnCap);
	Built.TurnText = FText::Format(
		LOCTEXT("TurnLine", "TURN {0} / {1}"),
		ScoreNumber(Built.Turn),
		ScoreNumber(Built.TurnCap));

	// ---- The three §2.8 rows, appended IN TIEBREAK ORDER ---------------------
	// §2.11.4: "rows are ordered top-to-bottom in exact tiebreak order (§2.8)" and "the
	// layout IS the rule". The order of these three Add calls is therefore load-bearing
	// game design and not a listing convenience; a sort or a filter downstream of here
	// would be editing §2.8.

	// Criterion 1 -- combat Fame. `fameCombat` AND NOT `fameTotal`: §2.11.4 excludes
	// passive income "exactly as the tiebreak excludes it", and `fameTotal` is the
	// spendable pool the separate top-right widget shows. The two are deliberately
	// different numbers and the tooltip below is the one sentence the single-currency
	// design (§2.7) needs in order to say so.
	Built.Rows.Add(MakeRow(
		EStratScoreCriterion::CombatFame,
		LOCTEXT("RowDestroyed", "Destroyed"),
		LOCTEXT("RowDestroyedTooltip", "Fame from kills. Factory income does not count at the cap."),
		static_cast<int32>(You.fameCombat),
		static_cast<int32>(Enemy.fameCombat),
		/*bHasTotal=*/false,
		/*Total=*/0));

	// Criterion 2 -- objectives held X of N. N is `objectiveTotal`, which the scenario
	// supplies (§2.13); the mock's 8 is Ferrum Crossing's four factories plus four
	// towns and is not written down here. ONE total serves both columns because the
	// snapshot carries one: the same board is being divided, and a per-side total would
	// be a second number that could disagree with the board.
	Built.Rows.Add(MakeRow(
		EStratScoreCriterion::Objectives,
		LOCTEXT("RowObjectives", "Objectives"),
		FText::GetEmpty(),
		static_cast<int32>(You.objectivesHeld),
		static_cast<int32>(Enemy.objectivesHeld),
		/*bHasTotal=*/true,
		static_cast<int32>(Snapshot.objectiveTotal)));

	// Criterion 3 -- surviving strength. §2.11.4: "listed last because it IS last".
	Built.Rows.Add(MakeRow(
		EStratScoreCriterion::SurvivingHp,
		LOCTEXT("RowUnitHp", "Unit HP"),
		FText::GetEmpty(),
		static_cast<int32>(You.survivingHp),
		static_cast<int32>(Enemy.survivingHp),
		/*bHasTotal=*/false,
		/*Total=*/0));

	// ---- §2.11.4's chevron: the criteria-order comparison --------------------
	// Walked in row order, which IS criteria order by the construction above, stopping
	// at the first criterion where the two sides differ. §2.8 awards each criterion to
	// the higher value, so the test is a bare `>`; nothing is summed, weighted or
	// normalised, and no criterion is consulted once an earlier one has decided.
	//
	// AT MOST ONE ROW carries a chevron, which falls out of the `break` rather than
	// being enforced afterwards.
	Built.Leader           = EStratScoreColumn::None;
	Built.LeadingCriterion = INDEX_NONE;

	for (int32 RowIndex = 0; RowIndex < Built.Rows.Num(); ++RowIndex)
	{
		FStratScoreboardRow& Row = Built.Rows[RowIndex];

		// `LeadingCriterion` names the row carrying the chevron, and the enum's numeric
		// values are §2.8's criteria positions -- so position and ordinal coincide, by
		// construction, in the block just above. Checked rather than assumed because the
		// two are written down in different places and a future row inserted in the wrong
		// order would otherwise mislabel the deciding criterion without misdrawing it.
		checkf(static_cast<int32>(Row.Criterion) == RowIndex,
			TEXT("Scoreboard rows must be built in GDD 2.8 tiebreak order; row %d carries criterion %d."),
			RowIndex, static_cast<int32>(Row.Criterion));

		if (Row.YouValue == Row.EnemyValue)
		{
			continue; // Tied at this criterion; §2.8 moves on to the next one.
		}

		Row.Chevron = (Row.YouValue > Row.EnemyValue)
			? EStratScoreColumn::You
			: EStratScoreColumn::Enemy;

		// Recorded twice on purpose: on the row for drawing, on the model for the
		// change detection §2.11.4 asks for when it requires the chevron to "flip
		// visibly when the lead changes". Both are written from this one comparison,
		// so they cannot disagree.
		Built.Leader           = Row.Chevron;
		Built.LeadingCriterion = RowIndex;
		break;
	}

	// §2.11.4's mutual-passivity case: both Destroyed values zero, where the chevron is
	// replaced by "-- no engagements --" spanning that row. Read straight off the two
	// snapshot fields the Destroyed row binds to.
	//
	// NOTE THAT THIS SUPPRESSES NOTHING. It is a statement about the Destroyed row's
	// display, not about the tiebreak: two zeroes are tied, so the walk above has
	// already passed that row by without leaving a chevron on it, and if the sides
	// differ on objectives the chevron correctly sits there. That is the reading the
	// model's own header takes -- "'nobody leads yet' and 'the match is heading for the
	// passivity draw' are different statements and only the second gets that text" --
	// and it is also the reading that keeps §1.5-#1 true, since a lead on criterion 2
	// is still a lead and the scoreboard exists so that no such lead is hidden.
	Built.bNoEngagements = (You.fameCombat == 0) && (Enemy.fameCombat == 0);

	OutModel = MoveTemp(Built);
	return true;
}

bool UStratScoreboardWidget::Refresh(const FStratBridge& Bridge, int32 ViewingSide, FString& OutFailureReason)
{
	// Built aside, then moved in. See this file's header block: a failed refresh must
	// leave the last correct standings on screen, and doing that by construction is
	// worth one stack temporary per refresh on a widget that updates at most once per
	// turn transition.
	FStratScoreboardModel Built;
	if (!StratBuildScoreboardModel(Bridge, ViewingSide, Built, OutFailureReason))
	{
		return false;
	}

	Model = MoveTemp(Built);

	// Fired AFTER the assignment, and handed the model as well, so that a graph reading
	// `Model` off the widget and a graph reading the event argument are looking at the
	// same values in the same frame. The argument exists so no Blueprint has a reason
	// to cache a copy of a number -- a cached copy is a value that can go stale, which
	// is the same failure as a recomputed one wearing different clothes.
	OnScoreboardRefreshed(Model);
	return true;
}

#undef LOCTEXT_NAMESPACE
