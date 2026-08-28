// GDD §2.11.2 -- the Blueprint-reachable surface for the hover info panel's projection.
//
// WHAT GAP THIS CLOSES, and it is the SAME gap `StratGuidanceWidget.h` opens by recording:
// a projection that is built, correct, and reachable by nothing. `FStratInfoPanelView` is
// filled every reconcile by `StratDecorateInfoPanel`, is pinned by clauses in
// `Source/StratPlay/Tests/StratInfoPanelRouting.cpp`, and until this file existed NO Widget
// Blueprint could read one field of it. A WBP authored against it would have been INERT,
// which is exactly what happened to the three assets authored against `FStratGuidanceView`
// before `UStratGuidanceWidget` existed.
//
// MEASURED, on the tree at `63433e7`, and the absence was controlled rather than asserted.
// Two greps -- for a Blueprint-accessible `UPROPERTY` of a view type, and for a
// `BlueprintImplementableEvent` delivering one -- return TWO hits for `FStratGuidanceView`
// (`UStratGuidanceWidget::Guidance` and `UStratGuidanceWidget::OnGuidanceRefreshed`) and,
// for `FStratInfoPanelView`, only `FStratViewModel::InfoPanel` itself. The instrument was
// therefore shown able to speak before its silence was read as an absence. The three routes
// that might have carried it and deliberately do not:
//   - `UStratMatchSubsystem::GetViewModel()` is not a `UFUNCTION`, by a ruling recorded on
//     its own declaration.
//   - `UStratMatchSubsystem::AppliedModel` is `UPROPERTY(Transient)` with no Blueprint
//     access, and is a RECORD of what was applied rather than an input to anything.
//   - `StratViewModelLibrary`'s `BlueprintPure` selectors TAKE a model and cannot produce
//     one.
// None of those three wanted changing. What was missing was a place for the value to
// ARRIVE, which is what this is.
//
// ---------------------------------------------------------------------------
// THE ONE DECISION THIS FILE MAKES: THE VIEWER-RELATIVE OWNERSHIP OF THE HEX.
// ---------------------------------------------------------------------------
//
// §2.11.2 renders the capturable clause as `· yours (+100/turn)` or `· neutral` or
// `· enemy`. `FStratInfoPanelView::HexOwner` is a SIDE, and `FStratInfoPanelView`'s own
// "NOT IN THIS ROUND" block refuses to bake the boolean there, in terms: it carries
// `HexOwner` "beside `FStratViewModel::ViewingSide` for the same reason
// `FStratForecastView::RiskedFlagSide` carries a SIDE rather than an own/enemy boolean ...
// the comparison rests on the viewer, and a boolean baked here would carry the premise that
// the viewer is fixed through a hot-seat hand-over."
//
// THAT REFUSAL IS ABOUT THE VIEW STRUCT AND NOT ABOUT THE COMPARISON. The comparison still
// has to be made somewhere, and the precedent for where is `FStratMatchResultModel`, which
// carries BOTH `ViewingSide` and a precomputed `bViewerWon` and says of the latter that it
// is "computed once here so no graph computes it". `FStratInfoPanelModel` below is that
// shape for §2.11.2: the view whole, the viewing side beside it, and the comparison already
// made. The alternative was leaving the widget to wire `HexOwner == ViewingSide` in its
// graph. What killed it is the second half of the same "NOT IN THIS ROUND" block, which
// distinguishes the two kinds of comparison a widget might make and rules on this one
// directly: `Owner == ViewingSide` "compares two fields of this model", so it belongs one
// layer below the graph for `bTerrainImpassable`'s and `FStratBuildOptionView::Shortfall`'s
// stated reason -- T-UI-03 forbids a widget deciding anything about a number it was handed.
//
// THE WORDS STAY THE WIDGET'S, AND THAT LINE HAS NOT MOVED. `yours`, `neutral`, `enemy`,
// the `·` separators, the `%` glyph, the `/` of `12/20`, `ready`/`done`, the flag's red
// edge and its sentence -- none of them is spelled anywhere in this file. What moved into
// C++ is the RESOLUTION, not the rendering: which of three states a hex is in for this
// viewer. A WBP switches on `EStratHexOwnership` and supplies the copy.
//
// WHY A NEW ENUM AND NOT `EStratScoreColumn`, WHICH IS THE NEAREST EXISTING SHAPE.
// FIRST, A CORRECTION TO THE BRIEF THIS FILE WAS WRITTEN FROM: it named
// `EStratScoreboardOwner { None, You, Enemy }` in `StratScoreboardWidget.h` as an existing
// type to consider reusing. THERE WAS NO SUCH TYPE ANYWHERE IN `Source/`. MEASURED, on the
// tree at `63433e7` -- which is the tree BEFORE this file was added -- `git grep
// "EStratScoreboardOwner" 63433e7 -- Source/` returns NOTHING, and the control that the
// grep works is that `git grep "enum class EStrat" 63433e7 -- Source/` returns eleven other
// enums. BOTH FIGURES ARE BOUND TO THAT COMMIT ON PURPOSE, because this file falsified them
// by shipping. Re-run either search against the CURRENT tree and it returns more: the first
// now returns 2, and both hits are the two sentences you are reading -- the comment quotes
// its own search token. The second now returns 13, the eleven plus this header's own
// `EStratHexOwnership` declaration plus the line above that quotes the token. A reader who
// greps the working tree, gets 2, and concludes the type exists has
// counted this paragraph. Reproduce the absence at the commit or not at all. The real
// neighbour is `EStratScoreColumn { None, You, Enemy }` in `StratScoreboardWidget.h`, whose
// shape is close and whose MEANING is not:
//   - Its `None` means "neither column", the chevron's absence. §2.11.2's `neutral` is the
//     opposite kind of value -- a real, held ownership state of a real capturable hex, and
//     one the panel prints a word for. Folding them onto one enumerator would let a widget
//     draw `neutral` for the chevron's "no engagements" case if the type were ever shared.
//   - It needs a FOURTH state this panel cannot do without. `bHexCapturable` false is a hex
//     with no ownership clause AT ALL -- a Plains hex -- and a NEUTRAL Factory carries the
//     identical `HexOwner == INDEX_NONE`. `StratDecorateInfoPanel` records that trap in its
//     own comment. A three-valued enum would force the widget to read `bHexCapturable`
//     alongside the enumerator and combine the two, which is the combination-shaped defect
//     this project has recorded as the one no clause covers.
//   - Reusing it would put `StratScoreboardWidget.h` above this header for a type that has
//     nothing to do with §2.8's rows, and would tie two vocabularies that must be free to
//     move apart -- `StratResultTierTextFor`'s reason for not reading `EStratResultTier`'s
//     UMETA DisplayNames as shipped copy, one layer up.
//
// WHY THIS HEADER DOES NOT INCLUDE StratBridge.h, and must never start to. It declares a
// `UCLASS` and two reflected types, so UHT parses it; `StratBridge.h` includes the vendored
// `strat` headers and is safe only where no reflected type is declared. Nothing here needs
// the bridge in any case -- `StratComposeInfoPanelModel` takes VALUES, reaches no bridge, no
// snapshot and no table, and is total and infallible for that reason. That is deliberately
// one step further than `StratBuildMatchResultModel`, which does take a bridge: everything
// §2.11.2 shows was already selected onto `FStratInfoPanelView` by `StratDecorateInfoPanel`,
// so there is no second source left to consult and no build here that could be refused.
//
// WHAT IS DELIBERATELY NOT IN THIS FILE.
// - NO ARITHMETIC AND NOTHING TO DO IT WITH. No `+`, no `-`, no `/`. The HP pair crosses as
//   two integers, the range pair as two integers, and `TerrainDefensePct` keeps its sign --
//   the Bridge row is `-10` and nothing here clamps it. A health-bar fraction is a rendering
//   decision a widget makes from `UnitHp` and `UnitHpMax`; it is not a number about the
//   match and it does not belong on this model.
// - NO SECOND STATEMENT OF ANY PANEL FIELD. `FStratInfoPanelView` is embedded WHOLE rather
//   than unpacked field by field, on `FStratMatchResultModel::Scoreboard`'s stated reason:
//   an unpacked copy would be two statements of one panel that agree today. Every one of the
//   view's fields reaches the widget unchanged and none is re-derived, so `UnitStatRangeMin`
//   AND `UnitStatRangeMax` both arrive (only Artillery is `2..3`; a widget fed one of them
//   is right on three unit rows and wrong on the one whose range matters), and `bUnitDone`
//   arrives as the machine's DONE bit rather than as a pair of turn flags.
// - NO VISIBILITY DECISION. `Panel.bHasHex` false is §2.11.2's "Empty when nothing is
//   hovered", and it covers "nothing hovered" and "hovered hex not on the board" on purpose
//   -- the field's own block rules on that. Nothing here shows, hides, or removes a widget;
//   a WBP binds visibility to that bool, exactly as the directive strip binds to `bActive`.
// - NO HOVER INPUT AND NO WAY TO MOVE ONE. The hover is written by a decorator on the
//   controller's refresh path. A widget deriving from this can draw the panel and cannot
//   change what it describes.
// - NO `TSubclassOf` AND NO `/Game/` PATH. This class is a parent, not a spawner. The asset
//   reference that instantiates it lives on `AStratScoreboardHUD::InfoPanelWidgetClass` as
//   an `EditDefaultsOnly` property set on a Blueprint default.
//
// IRREVERSIBLE ONCE AN ASSET DERIVES FROM THIS. A Widget Blueprint reparented onto this
// class bakes `/Script/StratUI.StratInfoPanelWidget` into its own bytes, and a WBP variable
// of `FStratInfoPanelModel` bakes that struct's name too. The module, the class name and the
// struct name are therefore expensive to change after the first asset exists -- the same
// trap `StratUI.Build.cs` records for `UStratScoreboardWidget`. StratUI and not StratPlay is
// correct for the reason it was correct there: `FStratInfoPanelView` lives in StratUI,
// StratPlay depends on StratUI, and UMG is already a dependency of this module and of no
// other.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "StratViewModel.h"

#include "StratInfoPanelWidget.generated.h"

/**
 * §2.11.2's capturable clause, resolved against the viewer.
 *
 * FOUR ENUMERATORS FOR A THREE-WORD LINE, and `NotCapturable` is the load-bearing one. It is
 * NOT a null and NOT an error: it is the ordinary case, every Plains, Woods, Mountains and
 * Water hex on the board, and §2.11.2 prints no ownership clause at all for it. Carrying it
 * as a real state is what lets a WBP write ONE total switch instead of reading
 * `Panel.bHexCapturable` and combining it with an enumerator -- and a neutral Factory and a
 * Plains hex both carry `HexOwner == INDEX_NONE`, so the combination is not optional if the
 * state is missing. `StratDecorateInfoPanel` records that same trap against `Ui.h`'s wording.
 *
 * `NotCapturable` IS ALSO WHAT AN UNHOVERED PANEL CARRIES. `bHasHex` false has no hex to own,
 * so there is nothing for this to say; see `StratComposeInfoPanelModel`, which is total over
 * every input including that one.
 *
 * IT IS NOT A `strat` CONCEPT AND MUST NOT BECOME ONE. The rules module has sides and knows
 * nothing about which of them is looking at the screen. `FStratViewModel::ViewingSide` is the
 * only join, its own declaration says it is "THE CALLER'S STATEMENT, NOT THE RULES MODULE'S",
 * and this enum is that statement's consequence rather than a rules answer.
 */
UENUM(BlueprintType)
enum class EStratHexOwnership : uint8
{
	NotCapturable UMETA(DisplayName = "Not Capturable"),
	Neutral       UMETA(DisplayName = "Neutral"),
	Yours         UMETA(DisplayName = "Yours"),
	Enemy         UMETA(DisplayName = "Enemy")
};

/**
 * Everything §2.11.2's panel draws, in one value.
 *
 * THE VIEW WHOLE, PLUS THE ONE COMPARISON IT REFUSES TO MAKE. `Panel` is embedded rather
 * than unpacked, so this struct restates no field of it and cannot drift from it; the two
 * members beside it are exactly what `FStratInfoPanelView`'s "NOT IN THIS ROUND" block sends
 * the reader elsewhere for.
 *
 * A SNAPSHOT OF A SNAPSHOT, held by value, for `FStratScoreboardModel`'s stated reason:
 * nothing here is a reference into rules memory, and `FStratInfoPanelView` was itself already
 * a selection over `FStratViewModel` and never a borrow.
 */
USTRUCT(BlueprintType)
struct FStratInfoPanelModel
{
	GENERATED_BODY()

	/**
	 * §2.11.2's panel as `StratDecorateInfoPanel` built it, unchanged and complete.
	 *
	 * EMBEDDED WHOLE AND NOT UNPACKED. See the header block: an unpacked copy would be a
	 * second statement of one panel, and the fields that matter most are the ones a partial
	 * copy would be most tempted to drop -- `UnitStatRangeMax` beside `UnitStatRangeMin`,
	 * `bUnitDone` rather than a pair of turn flags, and `TerrainDefensePct` with its sign.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	FStratInfoPanelView Panel;

	/**
	 * Which side this panel is drawn FOR. Copied from `FStratViewModel::ViewingSide`.
	 *
	 * NOT `Match.SideToMove` AND NOT `Panel.UnitSide`. `FStratSelectionMachine` records the
	 * first distinction and `FStratForecastView::RiskedFlagSide` the shape of the second.
	 * Carried raw beside the resolved answer for `FStratMatchResultModel::ViewingSide`'s
	 * reason: the number is what a clause compares against the model it came from, the
	 * enumerator is what the screen uses, and both are set from one read of one field.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	int32 ViewingSide = 0;

	/**
	 * `Panel.HexOwner` resolved against `ViewingSide`, computed once here so no graph
	 * computes it. See `EStratHexOwnership` and this file's header block.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	EStratHexOwnership HexOwnership = EStratHexOwnership::NotCapturable;
};

/**
 * The composition step, over VALUES. One copy and one comparison.
 *
 * A FREE FUNCTION AND NOT A METHOD ON THE WIDGET, for `StratComposeMatchResultModel`'s stated
 * reason: it draws nothing, holds no state, and can be driven with no Slate in existence --
 * which is the whole of what makes every arm of the mapping reachable by a clause. Both
 * inputs are plain values a test can hand-author, so unlike `StratScoreCriterionForKey`'s
 * predecessor there is no arm of this switch that only a player can reach.
 *
 * TOTAL AND INFALLIBLE -- no `bool`, no failure reason. There is nothing here to ask and
 * nothing to decline: `Panel` was already produced by a decorator that cannot fail, and every
 * `int32` including `INDEX_NONE` and every out-of-range side maps to a real enumerator. It
 * still assigns `OutModel` wholesale on its last line, so a caller's model is never
 * half-filled.
 *
 * THE ORDER OF THE THREE QUESTIONS IS LOAD-BEARING and is stated in the .cpp.
 *
 * @param Panel        §2.11.2's projection whole. Hand-authorable, which is the point.
 * @param ViewingSide  which `strat` side is looking. NOT `sideToMove`, NOT the unit's side.
 *                     Deliberately NOT range-checked -- see the .cpp.
 */
STRATUI_API void StratComposeInfoPanelModel(
	const FStratInfoPanelView& Panel,
	int32                      ViewingSide,
	FStratInfoPanelModel&      OutModel);

/**
 * §2.11.2's bottom-left, hover-driven, never-modal info panel.
 *
 * Holds a composed model and pushes it at Blueprint. It reads no `strat` type, chooses no
 * words, makes no comparison of its own and decides no visibility -- see this file's header
 * block for why each of those is the acceptance criterion rather than a style preference.
 *
 * Abstract and Blueprintable because the WBP_ asset derives from it and does the layout.
 * This class draws nothing: it holds one value and announces that the value changed.
 */
UCLASS(Abstract, Blueprintable, meta = (DisplayName = "Strat Info Panel"))
class STRATUI_API UStratInfoPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Replaces `Model` and fires `OnInfoPanelRefreshed`.
	 *
	 * NOT A UFUNCTION, and the reasoning is `UStratGuidanceWidget::PushGuidance`'s rather
	 * than `UStratScoreboardWidget::Refresh`'s -- cited rather than re-argued, because it is
	 * the same argument. Every argument here is reflectable, so nothing FORCES this to be
	 * plain C++. It is plain C++ so that Blueprint has no way to write a panel value at all.
	 * Reflecting it would let a graph push a fabricated hex -- a terrain it is not, a unit
	 * that is not there, a `ready` on a unit that has acted -- into the one surface the
	 * player uses to decide a move, and the panel must be readable off
	 * `FStratViewModel::InfoPanel` alone or "rebuild the screen from the view model" is a
	 * claim rather than a fact.
	 *
	 * VOID, AND IT CANNOT REFUSE. It is handed a finished value; there was no build here to
	 * decline, because `StratComposeInfoPanelModel` is total.
	 *
	 * ASSIGNS UNCONDITIONALLY, including when `Panel.bHasHex` is false. That is §2.11.2's
	 * "Empty when nothing is hovered" and it is the state the panel spends most of its life
	 * in; skipping the assignment on it would leave the last hovered hex's readout on screen
	 * after the cursor left the board, which is the one outcome §2.11.2 cannot produce. No
	 * equality test against the previous model either -- the delta-shaped thinking
	 * `UStratMatchSubsystem::ApplyView` refuses at the other end of this same call.
	 */
	void PushInfoPanel(const FStratInfoPanelModel& InModel);

	/**
	 * The last pushed model. Everything the WBP draws comes from here.
	 *
	 * READ-ONLY TO BLUEPRINT ON PURPOSE. `PushInfoPanel` is the only writer. A writable
	 * property would be the same second-author problem as a reflected setter, arrived at from
	 * a different direction.
	 *
	 * TRANSIENT because it is a projection of live match state and means nothing on disk; a
	 * saved copy would be a second, staler statement about a hex the cursor has left.
	 */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Info")
	FStratInfoPanelModel Model;

	/**
	 * Fired after `Model` is replaced.
	 *
	 * The model is passed as an argument as well as being readable off the widget, for the
	 * reason `OnScoreboardRefreshed`, `OnGuidanceRefreshed` and `OnMatchResultRefreshed` all
	 * give: so a graph has no motive to cache its own copy, a cached copy being a value that
	 * can go stale.
	 *
	 * FIRED ON EVERY PUSH, NOT ONLY ON A CHANGE, and here that matters more than it does for
	 * the strip: the hover moves every frame the mouse does, and a fire conditioned on a
	 * change would need this class to hold what the previous panel was. A graph that wants
	 * edge behaviour compares against what it drew, in the one place that already knows.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Stratocracy|Info")
	void OnInfoPanelRefreshed(const FStratInfoPanelModel& InModel);
};
