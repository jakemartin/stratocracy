// GDD §2.11.4 / §2.8 -- the live standings scoreboard, subject of T-UI-03.
//
// WHAT THIS FILE IS FOR: §2.11.4 opens by saying the scoreboard exists because of
// revision §1.5-#1 -- the tiebreak must never be a hidden win condition -- and that
// its rows are ordered top-to-bottom in exact tiebreak order, so that "the layout IS
// the rule". A widget that recomputed any of those numbers would be a second opinion
// about the rule sitting on top of the rule, which is the failure T-UI-03 names:
// "binds 1:1 to snapshot fields ... with no widget-side arithmetic".
//
// THE BINDING RULE, stated once so the rest of the file can refer to it:
//   Every number this widget draws is EQUAL TO ONE FIELD of `strat::UiSnapshot`.
//   Not a sum of two, not a difference, not a percentage, not a count of a vector.
//   If a display needs a number that is not already one field, that number is
//   decided outside the widget and handed in as a field of the row struct below.
// The raw `int32` value members exist precisely so this is checkable: a test in
// Source/StratUI/Tests/ can hold a snapshot beside a built model and assert equality
// field for field, which it could not do against pre-formatted text alone.
//
// WHY THIS HEADER DOES NOT INCLUDE StratBridge.h, and must never start to.
// `FStratBridge` is forward declared and included ONLY from the .cpp. StratBridge.h
// includes the vendored `strat` headers (Data.h, Ui.h, ...) and its own comment
// records why that is safe there: it "declares no reflected types on purpose, so the
// vendored `strat` headers it includes are never put in front of UHT". This header
// DOES declare reflected types, so UHT parses it; including the bridge here would put
// the vendored headers in front of the header tool and destroy the property that file
// was written to hold. StratUI.Build.cs records the same constraint from the module
// side. That is also why `Refresh` reports failure through `bool` + `FString` rather
// than returning the bridge's own `FStratResult`: naming that type in this header
// requires the include, and the include is the thing being refused.
//
// WHY A MODEL STRUCT INSTEAD OF meta=(BindWidget) TEXT BLOCKS. The rejected shape was
// a `UTextBlock*` member per cell of §2.11.4's mock -- turn line, six value cells, the
// chevron. Two reasons against. First, it freezes the mock's exact cell topology into
// C++, so a layout change becomes a C++ change even though the acceptance ID is about
// the BINDING being 1:1 and says nothing about which Slate primitive holds which
// number. Second, and the one that decided it: the T-UI-03 test would then have to
// construct a live widget tree to see any value, whereas it can compare a
// `FStratScoreboardModel` against a `strat::UiSnapshot` with no Slate at all. The WBP_
// asset does layout and reads `Model`; this class decides nothing about pixels.
//
// WHY THE CHEVRON IS A FIELD AND NOT A WIDGET-SIDE `if`. §2.11.4's chevron marks the
// attrition-tiebreak leader "evaluated in criteria order" -- that is §2.8's rule, not
// a drawing decision, and the widget having its own copy of it is exactly the second
// opinion this file is written to prevent. It is a comparison rather than arithmetic,
// which is why it can be answered at all outside the rules module; it is still
// answered outside the widget, by `StratBuildScoreboardModel` below.
//
// NOT ADDED TO strat::UiSnapshot, deliberately. The obvious alternative was a
// `chevronSide` field on `UiSideView`. T-UI-05 clause (c) fails a snapshot field with
// no contract entry, and `strat::UiSnapshot` is vendored besides -- so the comparison
// lives in this module's builder, above the widget and below the snapshot.
//
// OPEN, AND RECORDED RATHER THAN QUIETLY OWNED: if the rules module later exposes
// §2.8's criteria-order comparison as a callable (the same one behind `ResultTier`),
// `StratBuildScoreboardModel` MUST delegate to it. Two implementations of one
// tiebreak that agree today is the drift §4.9 exists to prevent, and this file is
// currently the second one.
//
// NOT IN THIS ROUND, with reasons:
// - §2.11.4's cap-approach banners. Their trigger is turn == cap-5, which is
//   arithmetic over two snapshot fields, and there is no field that carries the
//   answer: the snapshot has `turn` and `turnCap` and nothing between them, and
//   inventing `turnsRemaining` runs into the T-UI-05 clause (c) note above. The
//   banner needs an owner ruled first; it does not get one by default here.
// - §2.11.4's end-of-match screen. It is a different surface with a different
//   lifetime, and its faction-voiced result line is the one place faction voice
//   appears (kb/setting.md's pipeline note). Mixing it into the persistent
//   scoreboard's class would give that text a lifetime it must not have.
// - The spendable Fame pool (top-right). §2.11.4 is explicit that the Destroyed row
//   "deliberately does not equal" it; they are separate widgets so that nothing can
//   accidentally source one from the other.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "StratScoreboardWidget.generated.h"

// Forward declaration only -- see the header block above for why this may not become
// an include.
class FStratBridge;

// Which column the chevron sits beside. Expressed in the VIEWER's terms (you/enemy)
// rather than as a `strat` side index, because the widget must not perform the
// side-index -> column mapping itself; the builder does it once, from the viewing
// side it was handed.
//
// `None` is a real state and not a null: §2.11.4 replaces the chevron with
// "-- no engagements --" when both Destroyed values are zero, and a row can also be
// tied at a criterion that is not the deciding one.
UENUM(BlueprintType)
enum class EStratScoreColumn : uint8
{
	None  UMETA(DisplayName = "Neither"),
	You   UMETA(DisplayName = "You"),
	Enemy UMETA(DisplayName = "Enemy")
};

// The three §2.8 criteria, tagged so a test can assert the ROW ORDER rather than
// trusting it. §2.11.4: the rows are ordered in exact tiebreak order and the layout
// is the rule -- so the order of `FStratScoreboardModel::Rows` is load-bearing, and a
// widget that sorted or filtered it would be editing the rule. The tag exists to
// check the order, never to restore it.
//
// The numeric values are the criteria positions §2.8 assigns and are not free to
// renumber; `SurvivingHp` is last because §2.11.4 says it "is listed last because it
// IS last".
UENUM(BlueprintType)
enum class EStratScoreCriterion : uint8
{
	CombatFame  = 0 UMETA(DisplayName = "Destroyed"),
	Objectives  = 1 UMETA(DisplayName = "Objectives"),
	SurvivingHp = 2 UMETA(DisplayName = "Unit HP")
};

// One scoreboard row: a label, the two sides' values, and whether the chevron lands
// here.
//
// VALUES ARE CARRIED TWICE, ON PURPOSE, and this is the one duplication in the file.
// `YouValue` / `EnemyValue` are the raw integers copied out of the snapshot with no
// transformation -- they are the audit surface T-UI-03 asserts against. `YouText` /
// `EnemyText` are what the label shows, already composed (the "4/8" of §2.11.4's
// mock). Both are filled by the SAME builder call from the SAME snapshot read, so
// they cannot disagree; the alternative -- text only -- left the test nothing to
// compare against a snapshot field, and the alternative -- ints only -- pushed
// `FText::Format` into the widget, which is where composing a display out of two
// fields starts looking like permission to compute one.
USTRUCT(BlueprintType)
struct FStratScoreboardRow
{
	GENERATED_BODY()

	// Which §2.8 criterion this row is. See the enum's note on order.
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Scoreboard")
	EStratScoreCriterion Criterion = EStratScoreCriterion::CombatFame;

	// Row label ("Destroyed", "Objectives", "Unit HP"). UI copy, not game data --
	// nothing about the scenario or the tables can change it.
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Scoreboard")
	FText Label;

	// §2.11.4 gives the Destroyed row a disambiguating tooltip, because it is the one
	// place the single-currency design (§2.7) needs a sentence: the row is combat
	// Fame and excludes passive income, exactly as the tiebreak does. Empty on rows
	// that do not need one.
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Scoreboard")
	FText Tooltip;

	// EACH EQUAL TO ONE SNAPSHOT FIELD, for the viewing side and the other side
	// respectively:
	//   CombatFame  -> UiSideView::fameCombat     (NOT fameTotal -- §2.11.4 excludes
	//                                              passive income, and fameTotal is
	//                                              the spendable pool)
	//   Objectives  -> UiSideView::objectivesHeld
	//   SurvivingHp -> UiSideView::survivingHp
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Scoreboard")
	int32 YouValue = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Scoreboard")
	int32 EnemyValue = 0;

	// The N of "objectives held X of N", equal to `UiSnapshot::objectiveTotal`, which
	// the scenario supplies (§2.13). One field serves both columns because the
	// snapshot carries one total. `bHasTotal` is false on rows that show a bare
	// number, and `Total` is then not meaningful -- a sentinel would have to be a
	// number the display could accidentally print.
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Scoreboard")
	bool bHasTotal = false;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Scoreboard")
	int32 Total = 0;

	// Composed display forms of the two values above, and nothing else.
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Scoreboard")
	FText YouText;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Scoreboard")
	FText EnemyText;

	// The §2.11.4 chevron. Non-`None` on AT MOST ONE row in a model -- the first
	// criterion in order at which the sides differ -- and that decision is the
	// builder's, never the widget's.
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Scoreboard")
	EStratScoreColumn Chevron = EStratScoreColumn::None;
};

// Everything the scoreboard draws in one refresh.
//
// A SNAPSHOT OF A SNAPSHOT, and held by value on the widget for that reason: the
// bridge's `strat::UiSnapshot` is produced per call and the bridge's own header warns
// that borrowed world data must not outlive it, so the widget keeps no reference into
// rules memory. It keeps this copy, made of engine types, and the copy is the only
// thing Blueprint ever sees.
USTRUCT(BlueprintType)
struct FStratScoreboardModel
{
	GENERATED_BODY()

	// §2.11.4's "TURN 12 / 20". Both read from the snapshot -- `UiMatchView::turn` and
	// `UiMatchView::turnCap` -- because the cap is per-scenario data (§2.13.2) and
	// "the widget reads turnCap from the scenario rather than hardcoding a number" is
	// the GDD's own sentence about this line. `TurnText` is the composed form; the two
	// integers are what a test compares.
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Scoreboard")
	int32 Turn = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Scoreboard")
	int32 TurnCap = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Scoreboard")
	FText TurnText;

	// The side this scoreboard is drawn FOR -- the "YOU" column. A `strat` side index,
	// passed in by the caller rather than guessed: the snapshot carries `sideToMove`,
	// which is whose turn it is and NOT whose screen this is, and the two differ every
	// other turn. Range is validated in the .cpp, where SIDE_COUNT is reachable; this
	// header cannot name it without the forbidden include.
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Scoreboard")
	int32 ViewingSide = 0;

	// The three §2.8 rows, IN TIEBREAK ORDER. See EStratScoreCriterion.
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Scoreboard")
	TArray<FStratScoreboardRow> Rows;

	// True exactly when both sides' combat Fame is zero. §2.11.4 replaces the chevron
	// with "-- no engagements --" spanning the row in that case: the mutual-passivity
	// draw made visible before it bites. A flag rather than a magic value in
	// `Leader`, because "nobody leads yet" and "the match is heading for the passivity
	// draw" are different statements and only the second gets that text.
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Scoreboard")
	bool bNoEngagements = false;

	// The same decision as the row-level `Chevron`, expressed once for drawing (on the
	// row) and once for change detection (here). §2.11.4 requires the chevron to "flip
	// visibly when the lead changes", so a caller needs to compare this refresh's
	// leader against the last one's without walking rows. Both are set by one
	// comparison in one builder call.
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Scoreboard")
	EStratScoreColumn Leader = EStratScoreColumn::None;

	// Which criterion decided `Leader`, or INDEX_NONE when nothing did (all three tied,
	// or the no-engagements case). Names the row carrying the chevron.
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Scoreboard")
	int32 LeadingCriterion = INDEX_NONE;
};

// Builds the model from the bridge's view model. THE ONLY PLACE the snapshot is read.
//
// A FREE FUNCTION AND NOT A METHOD ON THE WIDGET, which is the whole point: T-UI-03's
// clause is about the WIDGET performing arithmetic, and the §2.8 chevron comparison
// has to happen somewhere between the vendored snapshot (which may not grow a field
// for it -- T-UI-05 clause (c)) and the pixels. It happens here, in a function that
// draws nothing, holds no state, and can be tested without a widget existing. That
// separation is also what lets the test in Source/StratUI/Tests/ assert the binding
// against a snapshot directly.
//
// It calls `FStratBridge::MakeUiSnapshot` and copies fields. It does not sum, count,
// difference or scale anything; the ONE judgement it makes is the criteria-order
// comparison for the chevron, and the header block above records that this must move
// to the rules module the moment that module exposes it.
//
// Fails rather than producing an empty scoreboard when the bridge has nothing to
// project, mirroring `MakeUiSnapshot`'s own reason for doing so: a caller must not be
// able to mistake "not seeded yet" for "a match where both sides have destroyed
// nothing", because the second of those has a banner and a draw attached to it.
// `OutFailureReason` carries the bridge's own words forward unchanged.
STRATUI_API bool StratBuildScoreboardModel(
	const FStratBridge& Bridge,
	int32 ViewingSide,
	FStratScoreboardModel& OutModel,
	FString& OutFailureReason);

/**
 * §2.11.4's persistent top-left scoreboard.
 *
 * Holds a built model and pushes it at Blueprint. It reads no `strat` type, indexes no
 * `side[]` array, and composes no number -- see this file's header block for why that
 * is the acceptance criterion rather than a style preference.
 *
 * Abstract and Blueprintable because the WBP_ asset derives from it and does the
 * layout. StratUI.Build.cs records the irreversible part of that arrangement: the
 * derived asset bakes `/Script/StratUI.StratScoreboardWidget` into itself, so this
 * class's module and name are expensive to change after the first asset exists.
 */
UCLASS(Abstract, Blueprintable, meta = (DisplayName = "Strat Scoreboard"))
class STRATUI_API UStratScoreboardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Rebuilds `Model` from the bridge and fires OnScoreboardRefreshed.
	 *
	 * NOT A UFUNCTION, and cannot be: `FStratBridge` is not a reflected type, and
	 * making it one would mean including the vendored headers in a UHT-parsed header.
	 * Blueprint drives the refresh through whatever owns the bridge, not by reaching
	 * for the bridge itself.
	 *
	 * NOT AN FStratResult return, for the same include reason -- see the header block.
	 * On failure `Model` is left untouched rather than cleared, so a transient refusal
	 * (a refresh attempted before seeding) does not blank a scoreboard that was
	 * already showing correct standings.
	 *
	 * @param ViewingSide  which `strat` side is the "YOU" column; NOT `sideToMove`.
	 */
	bool Refresh(const FStratBridge& Bridge, int32 ViewingSide, FString& OutFailureReason);

	/** The last successfully built model. Everything the WBP draws comes from here. */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Scoreboard")
	FStratScoreboardModel Model;

	/**
	 * Fired after `Model` is replaced. The WBP updates its text blocks here.
	 *
	 * The model is passed as an argument as well as being readable off the widget so
	 * that the Blueprint graph has no reason to cache its own copy of a value -- a
	 * cached copy is a value that can be stale, which is the same class of problem as
	 * a recomputed one.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Stratocracy|Scoreboard")
	void OnScoreboardRefreshed(const FStratScoreboardModel& InModel);
};
