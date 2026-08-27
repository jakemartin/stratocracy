// GDD §4.7 Stub 8 / §4.9 -- the reflected view model. T-INT-05's subject.
//
// WHAT GAP THIS CLOSES. `StratBridge.h:29-34` pre-authorises this file in as many
// words: `MakeUiSnapshot` hands back a `strat::UiSnapshot` by reference parameter
// rather than a USTRUCT because "a reflected mirror of the view model would have to be
// declared in a header UHT reads, and that header could then never include Ui.h", and
// it names the fix -- "the UMG-facing wrapper that copies fields out of the snapshot
// into reflected types is a later, separate file". This is that file. Until it existed,
// the only reflected projection in the tree was `FStratScoreboardModel`, which is three
// rows of §2.8 and carries no hex, no unit and no factory: nothing on screen could be
// described in engine types, so nothing could be rebuilt from a description.
//
// WHAT IT IS FOR, stated as the invariant it has to make structurally possible.
// §4.9's T-INT-05 requires the screen to be rebuildable FROM THE VIEW MODEL ALONE.
// Phase 3's `ApplyView` will spawn, move and destroy actors to match this value on
// every refresh -- so this is a COMPLETE DESCRIPTION OF WHAT SHOULD BE ON SCREEN AND
// NEVER A DELTA. There is no "changed" flag, no event, no dirty set and no sequence
// number anywhere below, and adding one would be the first step towards a screen that
// is only correct if every previous frame was also correct.
//
// IT LIVES IN StratUI, and that is a placement decision rather than a filing one. It is
// the lowest module both the widgets (here, today) and phase 3's `StratPlay` actors can
// see -- `StratPlay -> StratUI -> StratBridge`, per `.agents/ue-project-context.md`.
// Putting it in `StratBridge` would have meant a reflected header beside that module's
// vendored includes, which is the property `StratBridge.h` was written to hold; putting
// it in `StratPlay` would have put it above the widgets that need it.
//
// IT NAMES NO `strat::` TYPE, and that is the point rather than a coincidence. Every
// field below is `int32`, `bool`, `FName`, `FIntPoint` or one of the reflected types
// declared here. `StratPlay` will never name a `strat::` type, so anything left as a
// vendored type here would be a translation phase 3 had to perform, in a module that is
// not allowed to perform it. `strat::Hex{q, r}` arrives as `FIntPoint{X = q, Y = r}`;
// `strat::ResultTier` arrives as `EStratResultTier`, whose enumerator ORDER is the
// vendored enum's and is checked against it in the .cpp rather than trusted.
//
// WHY THIS HEADER MUST NEVER INCLUDE StratBridge.h. It declares USTRUCTs and a UENUM,
// so UHT parses it, and `StratBridge.h` includes the vendored `strat` headers (Data.h,
// Ui.h, ...). Its own note records why that is safe THERE -- it "declares no reflected
// types on purpose, so the vendored `strat` headers it includes are never put in front
// of UHT" -- and including it here would destroy exactly that property. This is the
// sharpest instance of the constraint in the tree, because this file is a MIRROR of
// those headers and the include looks helpful. `FStratBridge` is forward declared and
// included only from the .cpp. `StratScoreboardHUD.h` and `StratScoreboardWidget.h`
// refuse the same include for the same reason.
//
// There is deliberately no TPimplPtr and no owning member anywhere below: nothing here
// holds a bridge, so the `C4150` measured on `TUniquePtr<FStratBridge>` -- recorded in
// `StratScoreboardHUD.h`'s "WHY TPimplPtr AND NOT TUniquePtr" block -- has nothing to
// bite. The builder takes the bridge by const reference for the duration of one call and
// keeps nothing. (Cited by BLOCK NAME rather than line number on purpose: this comment
// first said `StratScoreboardHUD.h:48-56`, which by the time it was read pointed at that
// file's "AT MOST ONE IS EVER NON-NULL" ownership block instead -- a paragraph that
// mentions neither `C4150` nor `TUniquePtr`.)
//
// NO ARITHMETIC, ANYWHERE, and one lookup that is not arithmetic. NARROWED 2026-08-25 AND
// NOT DELETED, because the claim is still true of the thing it is about and a reader who
// remembers the absolute needs to see which half moved: it holds for `FStratViewModel` and
// every struct reachable from it, and there is now EXACTLY ONE exception, in a struct that
// is not part of the model at all. `FStratBuildOptionView::Shortfall` is
// `CostFame - FameTotal`, computed in `StratBuildProductionMenu`, and its own block states
// why §2.11.5's `need N` had to land one layer below the widget and what discharges it.
// The narrowing touches NOTHING ELSE in this pair: the three `static_assert`s -- one of
// which (`strat::SIDE_NONE == INDEX_NONE`) was added by the same pass -- and the two
// exhaustive mapping switches are unaffected, and the .cpp's own census block is the
// authority on both counts. Every number below
// is
// EQUAL TO ONE FIELD of `strat::UiSnapshot`, copied across with no transformation
// beyond a width cast -- the same binding rule `StratScoreboardWidget.h` states, held
// one layer lower. The single addition is `TerrainId` / `DefId`: the definition row's
// `id`, looked up by the index the snapshot carries. That is a table read, not a
// derivation, and the reason it is here is recorded on the fields themselves.
//
// THE PRESENTATION BLOCK'S DEBT IS DISCHARGED -- read the paragraph below as the reason
// the fields are shaped this way, not as an open item. `FStratSelectionMachine::Decorate-
// ViewModel` (`Source/StratPlay/StratSelectionMachine.h`) landed as the producer, called
// between `StratBuildViewModel` and `ApplyView`; it writes the two bits from the machine's
// own `TSet`s and no actor holds either, which is what `T-INT-05.NoActorHoldsPresentation-
// Bits` still pins. `StratBuildViewModel` itself is unchanged and still leaves both false
// -- that part of the paragraph below is current. Two details the original text got ahead
// of: the producer landed a phase later than "phase 3", and this, which the file used to
// assert in the present tense:
// RETRACTED> "`bLockedThisTurn` has a writer (`SetLockedThisTurn`) but no shipping caller,
// RETRACTED>  since §2.11.6's guidance layer does not exist, so that bit is false in every
// RETRACTED>  running path."
//
// THAT SECOND DETAIL IS NOW WITHDRAWN, and it is withdrawn in place rather than deleted
// so a reader who remembers it sees it retracted. Wave B2 landed the guidance layer:
// `FStratGuidedOpening` (`Source/StratPlay/StratGuidedOpening.h`) calls
// `SetLockedThisTurn` while beat 1a is outstanding and clears every lock the instant it
// retires, so `bLockedThisTurn` is true in a running path for the first time. What has
// NOT changed is the ownership: the bit is still written between `StratBuildViewModel`
// and `ApplyView`, still by a producer that is not an actor and not a widget, and
// `StratBuildViewModel` still leaves it false. `FStratGuidanceView` below is the same
// arrangement one level up -- the strip's own state, on the model, so T-INT-05 keeps
// meaning what it says.
//
// THE PRESENTATION BLOCK IS PRESENT AND NOTHING FILLS IT YET, recorded here rather than
// discovered later. `Ui.h`'s presentation-block header defines the view model as snapshot PLUS presentation
// and states why: the block "is in the view-model rather than inside a widget precisely
// so that T-INT-05 can rebuild the screen from the view-model alone -- state in the
// block satisfies that invariant, state in a widget does not". Its two members name
// owners that do not exist in this engine yet (§2.11.1's selection machine for `bDone`,
// the guidance layer for `bLockedThisTurn`), and `strat::buildUiSnapshot` explicitly
// does not produce them. They are therefore declared on `FStratUnitView`, defaulted
// false, and `StratBuildViewModel` leaves them at their defaults. THE DEBT: when phase
// 3's selection machine lands, it writes these two bits onto the built model and does
// NOT keep its own copy in an actor -- the moment it does, T-INT-05 is false and no
// compiler says so. Omitting the fields instead would have guaranteed that outcome by
// leaving the selection machine nowhere else to put them.
//
// NOT IN THIS ROUND, with reasons:
// - Any consumer. Phase 3 owns `ApplyView`; this phase owns the value it applies. The
//   builder is a free function for the same reason `StratBuildScoreboardModel` is: it
//   can be asserted against a bridge with no actor and no Slate in existence.
// - The §4.9 ordered event list. It is a different thing from a view model -- the model
//   says what IS, the event list says what HAPPENED -- and `StratBridge.h:41-46` records
//   the conflation as the trap. A hit-flash cannot be driven from this struct and is not
//   meant to be.
// - Anything the snapshot does not carry. §2.11.4's cap-approach banner, §2.11.5's
//   buildlist and §2.11.1's DONE bit are all in that set; the first two have no field
//   and the third is the presentation block's, above.
#pragma once

#include "CoreMinimal.h"

#include "StratViewModel.generated.h"

// Forward declaration only. See the header block above for why this may never become an
// include in this file.
class FStratBridge;

/**
 * §2.11.3's card as `FStratBridge::AttackForecast` answers it -- a PLAIN struct declared
 * in `StratBridge.h`, made of `int32`, `bool`, `FString` and `FName` and naming no
 * `strat::` type.
 *
 * FORWARD DECLARED FOR THE SAME REASON `FStratBridge` IS, and the reason is stronger than
 * it looks: the struct itself is harmless, but it is declared in a header that includes
 * `Ui.h` and `Data.h`, and this file declares USTRUCTs. `StratComposeForecastView` takes it
 * by const reference and only the .cpp needs its definition.
 */
struct FStratAttackForecast;

/**
 * §2.8's result tier, mirrored into a reflected enum.
 *
 * THE ORDER IS THE VENDORED ENUM'S AND IS NOT FREE. `strat::ResultTier`
 * (`strat::ResultTier`) is `{ InProgress, Draw, Marginal, Decisive }` and these enumerators are
 * pinned to it one for one. The mapping is written as an exhaustive switch in the .cpp
 * rather than as a cast, so that a tier added upstream is a compile failure in a file
 * that can be fixed instead of a silent renumber in a save-adjacent value.
 *
 * `InProgress` IS THE NULL, and that is Ui.h's ruling rather than this file's:
 * `UiMatchView` carries "the stub's `resultTier or null`. InProgress IS the null: §2.8's
 * tier is categorical and no numeric result exists to stand in for it." `bHasResult` on
 * `FStratMatchView` carries the same distinction, mirrored from `UiMatchView::hasResult`
 * rather than inferred from this enum, so the two cannot disagree.
 */
UENUM(BlueprintType)
enum class EStratResultTier : uint8
{
	InProgress UMETA(DisplayName = "In Progress"),
	Draw       UMETA(DisplayName = "Draw"),
	Marginal   UMETA(DisplayName = "Marginal"),
	Decisive   UMETA(DisplayName = "Decisive")
};

/**
 * WHY a match ended. Mirrors `strat::ResultCause`, enumerator for enumerator.
 *
 * PINNED BY ORDINAL TO THE VENDORED ENUM, exactly as `EStratResultTier` above is, and
 * mapped by an exhaustive switch in the .cpp rather than by a cast -- so a cause added
 * upstream is a compile failure in a file that can be fixed instead of a silent renumber.
 * `strat::ResultCause` is `{ None, FlagDestroyed, Domination, AttritionLead,
 * PassivityGuard, AllKeysTied }` and these six are it, in that order and with those names.
 *
 * `None` IS THE NULL AND IS NOT A SEVENTH STATE. Upstream spells it "still in progress",
 * and it is also what a capped match reports when no key was ever evaluated. A surface
 * must read the TIER to know whether a match ended; this says why, once one has.
 *
 * IT IS NOT A STRING AND MUST NOT BECOME ONE HERE. `strat::causeName` exists and is
 * deliberately not routed: §2.11.4's result line is FACTION-VOICED copy chosen by who
 * won, so the words belong to the screen and the cause belongs to the rules. A mapping to
 * player-facing text goes beside `ResultTierOf` when the GDD writes those strings, and
 * this enum is that mapping's INPUT rather than its output -- the same debt
 * `FStratBuildOptionView::Reason` carries and for the same reason.
 */
UENUM(BlueprintType)
enum class EStratResultCause : uint8
{
	None           UMETA(DisplayName = "None"),
	FlagDestroyed  UMETA(DisplayName = "Flag Destroyed"),
	Domination     UMETA(DisplayName = "Domination"),
	AttritionLead  UMETA(DisplayName = "Attrition Lead"),
	PassivityGuard UMETA(DisplayName = "Passivity Guard"),
	AllKeysTied    UMETA(DisplayName = "All Keys Tied")
};

/**
 * §2.11.6-B's four beats, plus the value for "no beat holds the line".
 *
 * FIVE ENUMERATORS FOR FOUR BEATS, and `None` is load-bearing rather than defensive: the
 * strip is GONE, not blank, once every beat has retired or the window closes, and a widget
 * needs a value that means that. §2.11.6 is explicit -- "there is no live-but-blank strip
 * in this system", so `None` always coincides with `FStratGuidanceView::bActive` being
 * false and never appears beside an active strip.
 *
 * `Beat1a` AND `Beat1b` RATHER THAN `One` AND `Two`, because the GDD numbers the beats
 * 1a and 1b and a renumbering here would make the beat table unreadable against the
 * code. They
 * are ordered so that the underlying value IS the beat's order index for rules 1-2, which
 * is what lets those rules be a scan in declaration order instead of a table.
 */
UENUM(BlueprintType)
enum class EStratGuidanceBeat : uint8
{
	None    UMETA(DisplayName = "None"),
	Beat1a  UMETA(DisplayName = "1a - Select and move the marked Infantry"),
	Beat1b  UMETA(DisplayName = "1b - End turn"),
	Beat2   UMETA(DisplayName = "2 - Capture the ringed Factory"),
	Beat3   UMETA(DisplayName = "3 - Spend Fame at your Factory")
};

/**
 * One board hex. Mirrors `strat::UiHexView`.
 *
 * HEXES ARE `FIntPoint`, X = q and Y = r, axial and pointy-top (§2.2, `strat::Hex`).
 * The conversion to odd-r offset -- and, beyond it, to a world location -- is phase 3's
 * and is deliberately absent: a hex is a board coordinate here, and the moment this
 * struct carried an `FVector` the view model would be describing a camera as well as a
 * match. `FIntPoint` and not a bespoke `FStratHex` because phase 3 has to do integer
 * math on these and an engine type it already has operators for is worth more than a
 * name that reads better.
 */
USTRUCT(BlueprintType)
struct FStratHexView
{
	GENERATED_BODY()

	/** Axial coordinate: X = q, Y = r. Mirrors `UiHexView::hex`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	FIntPoint Hex = FIntPoint::ZeroValue;

	/**
	 * Index into the loaded terrain table. Mirrors `UiHexView::terrainId`.
	 *
	 * LIVE WITHIN ONE BUILD ONLY, and that is a ruling rather than a caution.
	 * `DT_Terrain` row order was ruled NOT load-bearing on evidence (phase 0,
	 * `Tools/architect/state.md`): no `SaveCommand` field carries a terrain index,
	 * seeding resolves every hex's terrain BY NAME, and `canonicalStateBytes` emits no
	 * terrain field at all. So this number is self-consistent inside a build and means
	 * nothing across two. Key presentation off `TerrainId` below, never off this.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 TerrainIndex = 0;

	/**
	 * The terrain definition's own `id` ("Plains", "Bridge", ...), looked up through the
	 * bridge at the index above.
	 *
	 * THE ONE FIELD HERE THAT IS NOT A SNAPSHOT MIRROR, and the reason is the ruling
	 * quoted on `TerrainIndex`. Phase 3 has to choose a mesh per terrain, and the only
	 * two ways to write that down are an array indexed by row position or a map keyed by
	 * name. The first would bake a row order the project has explicitly declined to
	 * pin into a `.uasset` -- the exact "asset surgery" trap `.agents/ue-project-context.md`
	 * records for reflected class paths -- and it would fail silently, drawing Water as
	 * Woods. The name is what seeding itself resolves by (`strat::seedFromScenario`'s
	 * terrain-id resolution loop), so
	 * this field agrees with the rules module's own key.
	 *
	 * A LOOKUP, NOT A DERIVATION. It is read out of the definition table the bridge
	 * holds; nothing computes it, and if it is wrong the table is wrong.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	FName TerrainId;

	/**
	 * Owning side of a capturable hex, or `INDEX_NONE` where nothing is capturable.
	 * Mirrors `UiHexView::owner`.
	 *
	 * `strat::OWNER_NEUTRAL` is -1 and `INDEX_NONE` is -1, so this
	 * mirror is exact and not a re-encoding. The .cpp asserts that rather than assuming
	 * it, because the two constants are declared in different repositories.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 Owner = INDEX_NONE;
};

/**
 * One unit on the board. Mirrors `strat::UiUnitView`, plus the presentation block's two
 * per-unit bits (see the header block on why they are here and empty).
 */
USTRUCT(BlueprintType)
struct FStratUnitView
{
	GENERATED_BODY()

	/**
	 * This unit's instance id. Mirrors `UiUnitView::id`.
	 *
	 * IT IS WHAT `FStratBridge::Reachable` AND `FStratBridge::Forecast` TAKE, which is
	 * why it is spelled `UnitId` here and not `Id`: a phase 3 actor holding one of these
	 * asks the bridge a question with it, and the parameter it passes it to is named the
	 * same thing.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 UnitId = 0;

	/** Which side owns it. Mirrors `UiUnitView::side`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 Side = 0;

	/**
	 * The §2.4 definition row this unit is. Mirrors `UiUnitView::unitId`.
	 *
	 * RENAMED ON PURPOSE. `SaveCommand::unitId` spells
	 * the same quantity `unitId` in a Build command, where `applyCommand` uses it as a
	 * raw bounds-checked index into the definitions vector with no name lookup
	 * (`strat::applyCommand`'s `Build` arm bounds-checks it against the unit-definition
	 * table). That spelling is the trap -- it reads like an instance
	 * id and is a row index -- and `FStratBridge::SubmitBuild` already renames it to
	 * `DefIndex` "for what the rules module does with it rather than for what the format
	 * calls it". This field keeps that name so the two agree.
	 *
	 * UNLIKE TERRAIN, THIS ORDER IS LOAD-BEARING and is pinned:
	 * `Stratocracy.StratBridge.GATE-BRIDGE-DEFS.MappedDefsMatchLoaderOrder` asserts
	 * `DT_Units` against `strat::loadUnits` positionally, field by field. Indexing an
	 * array by this is therefore legitimate in a way indexing by `TerrainIndex` is not.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 DefIndex = 0;

	/**
	 * The unit definition's own `id` ("Infantry", "Tank", ...), looked up at `DefIndex`.
	 *
	 * Carried even though `DefIndex` is pinned, because a presentation mapping keyed by
	 * name survives a table edit that inserts a row and an array indexed by position does
	 * not -- and the gate that pins the order exists to catch a DATA divergence, not to
	 * license `.uasset` arrays that would break loudly the day a fifth unit ships. The
	 * index remains available for anything that genuinely needs the position.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	FName DefId;

	/** Where it is now, axial. Mirrors `UiUnitView::hex`. NOT its placement. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	FIntPoint Hex = FIntPoint::ZeroValue;

	/** Mirrors `UiUnitView::hp`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 Hp = 0;

	/** Mirrors `UiUnitView::hpMax`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 HpMax = 0;

	/** §2.4's designated Tank. Mirrors `UiUnitView::isFlag`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	bool bIsFlag = false;

	/**
	 * TWO INDEPENDENT FLAGS, NOT ONE (T-TURN-01), and they are mirrored separately for
	 * the reason `UiUnitView`'s two-independent-flags note gives: "one field cannot express a unit that has spent
	 * exactly one of them, which is the drift this row's GDD half repaired". Neither is
	 * §2.11.1's DONE bit -- that is `bDone` below and it is a different owner's.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	bool bHasMoved = false;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	bool bHasActed = false;

	/** Turns this unit has held its capture. Mirrors `UiUnitView::captureProgress`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 CaptureProgress = 0;

	/**
	 * §2.11.6 beat 1a's marked unit. Mirrors `UiUnitView::isGuidedMarked`, which the
	 * rules module DECLARES DERIVED and computes -- read off the unit's `placement` and
	 * never its `hex`, so beat 1a's own move cannot unmark the unit the beat is about.
	 * Copied here, never recomputed: the derivation is Ui.cpp's and T-UI-05 is its gate.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	bool bIsGuidedMarked = false;

	// ---- The presentation block (`strat::UiPresentationUnit`). NOT the rules module's. ------
	// `StratBuildViewModel` leaves both at false; `strat::buildUiSnapshot` does not
	// produce them and the bridge has nothing to project them from. See the header
	// block: they are declared here so that phase 3's selection machine has somewhere to
	// put them that is not an actor, because state in an actor makes T-INT-05 false and
	// nothing reports it.

	/**
	 * §2.11.1's DONE bit. OWNER: the selection machine, which is a state machine and not
	 * a widget. This unit takes no further command this turn.
	 *
	 * DERIVABLE FROM NEITHER TURN FLAG NOR FROM ANY PAIR OF THEM -- Ui.h says so, and
	 * names the cases: Wait and RMB-in-MOVED both reach DONE without spending the act
	 * flag. Anything that sets this from `bHasMoved` / `bHasActed` is wrong.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	bool bDone = false;

	/**
	 * §2.11.6 turn 1a's `Locked this turn.` OWNER: the guidance layer, which is neither
	 * the rules module nor a widget.
	 *
	 * ITS LIFECYCLE IS NOT `bDone`'s. It clears when beat 1a RETIRES -- when the marked
	 * Infantry's move completes, inside turn 1 -- and not at the turn boundary. A unit
	 * can be un-locked and not-done in the same turn.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	bool bLockedThisTurn = false;
};

/**
 * One factory. Mirrors `strat::UiFactoryView`.
 *
 * A SEPARATE GROUP AND NOT A FLAG ON `FStratHexView`, because that is the shape Ui.h
 * settled on: §2.11.5's BUILD pulse and its boxed-in footer "need facts no other group
 * carries", and folding them into the hex view would put three fields on 99 hexes that
 * are meaningful on four.
 */
USTRUCT(BlueprintType)
struct FStratFactoryView
{
	GENERATED_BODY()

	/** Axial coordinate of the factory. Mirrors `UiFactoryView::hex`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	FIntPoint Hex = FIntPoint::ZeroValue;

	/** Owning side, or `INDEX_NONE` when neutral. Mirrors `UiFactoryView::owner`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 Owner = INDEX_NONE;

	/** T-TURN-10's per-factory build allowance. Mirrors `UiFactoryView::hasBuiltThisTurn`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	bool bHasBuiltThisTurn = false;

	/** A §2.7 build holding this factory's slot until it spawns (T-FAME-04). Mirrors
	 *  `UiFactoryView::buildWaiting`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	bool bBuildWaiting = false;

	/**
	 * No hex at or adjacent to the factory is free. Mirrors
	 * `UiFactoryView::spawnBlocked`, which the rules module DECLARES DERIVED from board
	 * geometry plus §2.7's spawn rule.
	 *
	 * DISTINCT FROM `bBuildWaiting`, and the difference is the case §2.11.5 must
	 * display: a boxed-in factory with nothing queued has this true and that false,
	 * which that field alone cannot express.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	bool bSpawnBlocked = false;
};

/**
 * One row of §2.11.5's production menu. Mirrors `strat::UiBuildOption` field for field.
 *
 * IT IS NOT A FIELD OF `FStratFactoryView`, and that is the same decision the rules
 * module already made one layer down rather than a fresh one. `strat::uiBuildOptions` is
 * a QUERY and not a `UiSnapshot` field because every snapshot field is pinned by
 * T-UI-05's enumeration; hanging the buildlist off the factory view here would put four
 * rows on each of four factories inside a value that is rebuilt on every refresh, to be
 * read by the one surface that opens a production menu. `StratBuildProductionMenu`
 * answers it on demand instead, and `FStratViewModel` does not grow.
 */
USTRUCT(BlueprintType)
struct FStratBuildOptionView
{
	GENERATED_BODY()

	/**
	 * The §2.4 row this offer is, and the value `SubmitBuildAtHex` takes. Mirrors
	 * `UiBuildOption::defIndex`.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Production")
	int32 DefIndex = INDEX_NONE;

	/** The unit's id, e.g. `Infantry`. Mirrors `UiBuildOption::id`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Production")
	FName Id;

	/** The §2.4 price. Mirrors `UiBuildOption::costFame`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Production")
	int32 CostFame = 0;

	/**
	 * This side can pay for it right now. Mirrors `UiBuildOption::affordable`, which the
	 * rules module computes as `costFame <= fameTotal`.
	 *
	 * THE WIDGET MUST NOT DERIVE THIS, and it is carried precisely so that it need not:
	 * T-UI-03 forbids widget-side arithmetic, and comparing a price to a purse inside a
	 * binding is exactly that. Nothing between here and the module recomputes it.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Production")
	bool bAffordable = false;

	/**
	 * How much more Fame this side needs before it can pay for this row. Zero whenever
	 * `bAffordable` is true.
	 *
	 * THE ONE DERIVED NUMBER IN THIS PAIR, AND IT IS DECLARED DERIVED RATHER THAN
	 * DISGUISED AS A MIRROR. Every other field on every other struct in this header is
	 * EQUAL TO ONE `strat::UiSnapshot` or `strat::UiBuildOption` field, copied across with
	 * no transformation beyond a width cast; this one is `CostFame - FameTotal`, computed
	 * in `StratBuildProductionMenu`. The header block's "NO ARITHMETIC, ANYWHERE" claim is
	 * therefore NARROWED HERE rather than left standing falsely: it holds for
	 * `FStratViewModel` and everything reachable from it, and this struct -- a query
	 * answered on demand, never a field of the model -- now carries exactly one exception.
	 *
	 * WHY IT IS HERE AND NOT IN THE WIDGET. §2.11.5 requires an unaffordable row to stay
	 * visible, stay priced, and NAME THE SHORTFALL (`need 50`). That sentence is arithmetic
	 * and T-UI-03 forbids a widget performing it. Somebody one layer below the screen has
	 * to, and this is the lowest layer that can see both the price and the purse.
	 *
	 * WHY IT IS NOT UPSTREAM, WHICH IS WHERE `bAffordable` LIVES. `UiBuildOption` carries
	 * `costFame` and `affordable` and no shortfall, and `affordable` is module-side
	 * PRECISELY so that T-UI-03 has something to bind to. By that precedent the shortfall
	 * belongs beside it. It is here instead because it is PRESENTATION COPY'S INPUT and not
	 * a rules fact: no rule reads it, no §4.10 command carries it, and no §2.8 outcome
	 * turns on it. A DEBT RIDES WITH THAT CALL and is written down rather than owned
	 * quietly -- **DISCHARGED WHEN** an upstream pass adds `shortfallFame` to
	 * `UiBuildOption`, at which point this field becomes an ordinary mirror and the
	 * subtraction in `StratBuildProductionMenu` is DELETED rather than moved.
	 *
	 * `bAffordable` IS THE AUTHORITY AND THIS IS NEVER READ TO DERIVE IT. A widget greys a
	 * row off `bAffordable` and prints `need N` off this; asking `Shortfall > 0` instead
	 * would make the screen's greying depend on this file's subtraction rather than on the
	 * module's comparison, which is the substitution T-UI-03 exists to catch.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Production")
	int32 Shortfall = 0;

	/**
	 * This factory will take a build right now. Mirrors `UiBuildOption::available`.
	 *
	 * A SEPARATE FIELD FROM `bAffordable`, AND NOT AN AND OF IT. "This factory has
	 * already built this turn" and "you are still saving for this" are different answers
	 * and §2.11.5 shows them differently; a menu handed one boolean could not tell the
	 * player which one it was looking at.
	 *
	 * IT DOES NOT VARY BY ROW. Every gate behind it is a property of the FACTORY and the
	 * SIDE, never of the unit type -- the per-type population cap is AI policy and the
	 * player is bound by no cap. Rows that disagreed here would be that cap leaking into
	 * a path a player command passes through.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Production")
	bool bAvailable = false;

	/**
	 * Why not, when `bAvailable` is false; empty when it is true. Mirrors
	 * `UiBuildOption::reason`.
	 *
	 * THE MODULE'S OWN WORDS AND NOT AUTHORED UI COPY -- a debt rather than a decision.
	 * §2.11.5 states no strings for these refusals, and inventing them here would be the
	 * widget layer deciding what the rules mean. When the GDD writes them, the mapping
	 * belongs beside `ResultTierOf` and this field becomes its input rather than its
	 * output.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Production")
	FText Reason;
};

/** One side's standings. Mirrors `strat::UiSideView` field for field. */
USTRUCT(BlueprintType)
struct FStratSideView
{
	GENERATED_BODY()

	/**
	 * The spendable Fame pool. Mirrors `UiSideView::fameTotal`.
	 *
	 * NOT THE TIEBREAK NUMBER. §2.11.4 is explicit that the Destroyed row "deliberately
	 * does not equal" this; the tiebreak reads `FameCombat` below, which excludes passive
	 * income. Both are carried because they are different quantities and a surface that
	 * had to pick one would be picking a rule.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 FameTotal = 0;

	/** Fame from kills -- §2.8's first tiebreak key. Mirrors `UiSideView::fameCombat`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 FameCombat = 0;

	/** The X of "objectives held X of N". Mirrors `UiSideView::objectivesHeld`; the N is
	 *  `FStratViewModel::ObjectiveTotal`, which is one number for the whole board. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 ObjectivesHeld = 0;

	/** §2.8's third key. Mirrors `UiSideView::survivingHp`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 SurvivingHp = 0;

	/**
	 * §2.7's STANDING income rate over this side's held factories and towns. Mirrors
	 * `UiSideView::incomePerTurn`, which the rules module DECLARES DERIVED and computes
	 * from the terrain table's `incomeFame` "so no surface sums it widget-side".
	 *
	 * IT IS THE RATE THE HOLDINGS CARRY AND NEVER THIS TURN'S ACCRUAL. Ui.h is explicit
	 * that this reads non-zero on turn 1 even though Q8(a) pays 0 then.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 IncomePerTurn = 0;
};

/** Where the match is. Mirrors `strat::UiMatchView`. */
USTRUCT(BlueprintType)
struct FStratMatchView
{
	GENERATED_BODY()

	/** Mirrors `UiMatchView::turn`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 Turn = 0;

	/** Per-scenario data (§2.13.2), never a constant. Mirrors `UiMatchView::turnCap`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 TurnCap = 0;

	/**
	 * Whose turn it is. Mirrors `UiMatchView::sideToMove`.
	 *
	 * NOT `FStratViewModel::ViewingSide`. This is the rules module's answer about the
	 * match; that is the caller's statement about whose screen this is, and in hot-seat
	 * the two differ every other turn. Conflating them is the bug
	 * `AStratScoreboardHUD::SetViewingSide` exists so that nobody has to commit.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 SideToMove = 0;

	/** False while the match is in progress. Mirrors `UiMatchView::hasResult` -- read,
	 *  never inferred from `ResultTier`, so the two cannot disagree. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	bool bHasResult = false;

	/** §2.8's tier. Mirrors `UiMatchView::resultTier`; `InProgress` is the null. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	EStratResultTier ResultTier = EStratResultTier::InProgress;
};

/**
 * §2.8's result WHOLE -- the three fields `FStratMatchView` drops. Mirrors
 * `strat::UiMatchResult` field for field.
 *
 * IT IS NOT A FIELD OF `FStratViewModel`, AND THAT IS THE SAME DECISION MADE THREE TIMES
 * DOWN THE STACK rather than a fresh one here. Upstream declared `uiMatchResult` a QUERY
 * and not a `UiSnapshot` field, because every snapshot field is pinned by T-UI-05's
 * enumeration; `FStratBridge::MatchResult` routes it as a query for the same reason; and
 * hanging it off the view model here would put four values only §2.11.4's end-of-match
 * screen reads inside a value that is rebuilt on every refresh, for every consumer.
 * `StratBuildMatchResult` answers it on demand instead, exactly as
 * `StratBuildProductionMenu` does, and `FStratViewModel` does not grow.
 *
 * SO IT NEEDS NO PARITY CLAUSE, and that absence is load-bearing rather than an
 * oversight. `StratViewModelParity.cpp` walks `FStratViewModel` against a snapshot the
 * same bridge projected; this struct is outside that walk because it mirrors nothing IN
 * the snapshot. What pins it instead is the ROUTING -- a clause asks the bridge and asks
 * this, on the same bridge in the same frame, and compares.
 *
 * IT DOES NOT CARRY `bHasResult`. `FStratMatchView` does, read from the module rather
 * than inferred, and a second copy here could disagree with it. `Tier == InProgress` is
 * upstream's own null and is what this struct offers; a surface that needs the boolean
 * reads the view model's, which is the one the screen was drawn from.
 */
USTRUCT(BlueprintType)
struct FStratMatchResultView
{
	GENERATED_BODY()

	/** §2.8's tier. Mirrors `UiMatchResult::tier` -- the same value
	 *  `FStratMatchView::ResultTier` carries, through the same `ResultTierOf` mapping and
	 *  no other, so the two cannot spell one tier two ways. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Result")
	EStratResultTier Tier = EStratResultTier::InProgress;

	/** Why the match ended. Mirrors `UiMatchResult::cause`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Result")
	EStratResultCause Cause = EStratResultCause::None;

	/**
	 * The winning `strat` side, or `INDEX_NONE` on a draw AND while in progress. Mirrors
	 * `UiMatchResult::winner`, convention included.
	 *
	 * `INDEX_NONE` AND NOT A SEPARATE FLAG, because `strat::SIDE_NONE` IS -1 -- and the
	 * two constants are CHECKED equal in the .cpp rather than assumed, the same treatment
	 * `strat::OWNER_NEUTRAL` gets on the owner fields and for the same reason: they are
	 * declared in two repositories, one vendored and one engine.
	 *
	 * IT IS NOT `FStratMatchView::SideToMove` AND MUST NEVER BE DERIVED FROM IT. The two
	 * agree on a flag kill only because the killer happened to be the side to move, and
	 * disagree at the turn cap, where the match ends at a boundary. This field exists
	 * because that derivation is right in the common case and silently wrong in exactly
	 * the case a tiebreak decides -- see `FStratBridge::MatchResult`.
	 *
	 * IT IS NOT `FStratViewModel::ViewingSide` EITHER. Who won is the rules module's
	 * answer about the match; whose screen this is is the caller's statement. §2.11.4's
	 * faction-voiced result line is chosen by comparing them, and that comparison belongs
	 * to the screen that draws it.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Result")
	int32 Winner = INDEX_NONE;

	/**
	 * Which §2.8 tiebreak key decided a capped match: 1, 2 or 3; 0 when no tiebreak was
	 * evaluated. Mirrors `UiMatchResult::decidedByKey`.
	 *
	 * A NUMBER AND NOT AN ENUM, deliberately. §2.8 numbers the keys and T-TURN-04 grades
	 * "the criterion that differed" by that number; naming them here would be this file
	 * authoring a vocabulary the GDD has not written, and the mapping to Fame / objectives
	 * / surviving HP would then have two spellings.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Result")
	int32 DecidedByKey = 0;
};

/**
 * §2.11.6-B's directive strip and the two input gates behind it, as one value.
 *
 * WHAT THIS IS NOT: it is not the beat machine. `FStratGuidedOpening`
 * (`Source/StratPlay/StratGuidedOpening.h`) owns which beat is outstanding, which has held
 * the line, and when each retires; this struct is the projection of that machine for one
 * frame, the way `FStratMatchView` is a projection of the rules module's turn state. A
 * widget branching on this can draw the strip; it cannot advance a beat, and there is
 * deliberately nothing here for it to advance one with.
 *
 * EVERY FIELD IS SOMETHING A WIDGET DRAWS OR DIMS, and nothing here is a fact a widget
 * would have to combine with another to use -- the same no-widget-side-arithmetic line
 * `T-UI-03` draws over the scoreboard. `bShowsWindowEndTag` in particular is a field
 * rather than `Turn == 4` computed in Blueprint, because §2.11.6 states the tag "states a
 * fact about the WINDOW, not about rule 2", and a widget recomputing it would be a second
 * author of the window's length.
 *
 * NO LOCALIZATION. `DirectiveText`, `WindowEndTagText` and the two hover strings are
 * `FText`
 * built from the GDD's literal ASCII with `FText::FromString`, NOT `LOCTEXT`. That is a
 * deliberate stop rather than a half-build: this project ships one language, a `LOCTEXT`
 * namespace here would create string-table entries nothing translates, and the strings are
 * quoted verbatim from §2.11.6-B so a translator's first job is to find them in the GDD
 * rather than in a `.po`. `FText` and not `FString` so that the day localization is real,
 * the widget bindings do not change shape.
 */
USTRUCT(BlueprintType)
struct FStratGuidanceView
{
	GENERATED_BODY()

	/**
	 * Whether the strip is on screen at all.
	 *
	 * FALSE HAS THREE CAUSES AND THE STRIP CANNOT TELL THEM APART, deliberately: guidance
	 * was suppressed (a completed match on the save), the player pressed `Skip guidance`,
	 * or every beat retired / the window closed at the end of turn 4. All three are "gone
	 * for good" by §2.11.6, none is recoverable, and a widget that branched on which would
	 * be drawing a distinction the player is never shown.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Guidance")
	bool bActive = false;

	/** Which beat holds the line. `None` exactly when `bActive` is false. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Guidance")
	EStratGuidanceBeat Beat = EStratGuidanceBeat::None;

	/** That beat's one line, verbatim from §2.11.6-B's Directive column. Empty when inactive. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Guidance")
	FText DirectiveText;

	/**
	 * The dim right-hand tag on a turn-4 line: `guidance ends this turn`.
	 *
	 * A SEPARATE FIELD FROM `DirectiveText` AND NOT APPENDED TO IT, because §2.11.6's own
	 * mock-up renders it right-aligned on its own row in a dim style. Concatenating would
	 * make the strip a single string and hand the widget a formatting decision the GDD
	 * already made.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Guidance")
	bool bShowsWindowEndTag = false;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Guidance")
	FText WindowEndTagText;

	/**
	 * The ringed neutral Factory (`guidedOpening.objective`), from turn 1 until the strip
	 * disappears. `bHasObjectiveRing` false means there is nothing to ring.
	 *
	 * IT IS NOT ON `FStratHexView` OR `FStratFactoryView`, and that is a decision worth its
	 * line. A `bIsGuidedObjective` flag on 99 hexes -- or on four factories -- would be the
	 * shape `FStratFactoryView`'s own block rejects for the build fields, and it would
	 * survive the strip's disappearance unless something remembered to clear it on 99
	 * elements. One hex in the guidance block clears when the block does.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Guidance")
	bool bHasObjectiveRing = false;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Guidance")
	FIntPoint ObjectiveHex = FIntPoint::ZeroValue;

	// ---- The two input gates (Q27, §4.7 -- ruled) --------------------------
	// §2.11.6-B calls beat 1a's End Turn gate "the only guided-opening constraint that
	// gates a player INPUT rather than a selection, adopted under Q27, ruled". These two
	// fields are that gate's screen half. THE GATE ITSELF IS IN
	// `AStratPlayerController::HandleSelectionEvent`, and it is a UI restriction and
	// nothing more: no line behind these asks `FStratBridge` to refuse a command the rules
	// module would accept, which is the same footing §2.11.1's machine-narrower-than-the-
	// rule note already puts the SELECTED -> attack case on.

	/** End Turn is inert. Draw it dimmed with `EndTurnGateHover` on hover. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Guidance")
	bool bEndTurnGated = false;

	/** `Move the marked Infantry first.` -- §2.11.6-B, beat 1a. Empty when not gated. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Guidance")
	FText EndTurnGateHover;

	/**
	 * `Locked this turn.` -- §2.11.6-B, beat 1a. The hover for any unit whose
	 * `FStratUnitView::bLockedThisTurn` is true.
	 *
	 * ONE STRING HERE RATHER THAN A COPY PER UNIT. The lock itself is per-unit and lives on
	 * `FStratUnitView` where it belongs; the sentence is the same for every locked unit, and
	 * putting it on each would be the 99-hexes shape again. Non-empty whenever any unit is
	 * locked, so a widget need not know whether guidance is running to render a hover.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Guidance")
	FText LockedUnitHover;
};

/**
 * Which hex the cursor is over, as one value. §2.11.3 / §2.11.2, wave 0.
 *
 * WHAT GAP THIS CLOSES. §2.11.3's attack forecast card and §2.11.2's info panel are both
 * specified as HOVER-driven, and until this struct existed nothing on this side of the
 * project carried a hovered hex at all -- at `69e75bb`, `AStratPlayerController` bound no
 * hover action of any kind. The hover STRINGS on `FStratGuidanceView` above are tooltip
 * TEXT for a widget that already knows what it is drawing; they are not an input and they
 * answer no question about where the cursor is. This struct is the input half, and it is
 * deliberately the whole of wave 0's model surface: neither the forecast struct nor the info
 * panel struct is here, because each is its own wave and a field landed ahead of its reader
 * is a field written without its caller.
 *
 * PART OF THE MODEL AND NOT OF THE CONTROLLER, for `FStratGuidanceView`'s reason exactly and
 * with the same trap one line away: a `BlueprintPure` on `AStratPlayerController` returning
 * the hovered hex would work on the first day and would make `T-INT-05` false, because
 * "rebuild the screen from the view model alone" stops holding the moment one visible element
 * reads from somewhere else. A card that appears because of a hover IS a visible element.
 *
 * A FLAG AND A HEX RATHER THAN A SENTINEL, because there is no hex value that cannot be a
 * real hex. `FIntPoint::ZeroValue` is (0,0), which on Ferrum Crossing is an ordinary
 * board corner -- a model that encoded "not hovering" as (0,0) would draw a forecast for the
 * top-left tile every time the cursor left the board. `bHasHoveredHex` is therefore read
 * FIRST by every consumer, and `HoveredHex` means nothing when it is false.
 *
 * NO UNIT ID, NO TERRAIN, NO FORECAST. A widget that wants the unit under the cursor looks it
 * up in `Units` by hex; that is a lookup and not arithmetic, and it is the same lookup every
 * other consumer of this model already performs. Adding a `HoveredUnitId` here would be a
 * second author of a fact `Units` already states, and the two would drift the day a unit
 * moves between the build and the decoration.
 *
 * WRITTEN BY `FStratHoverState::DecorateViewModel` (`Source/StratPlay/StratHoverState.h`),
 * between `StratBuildViewModel` and `ApplyView`, on the same seam
 * `FStratSelectionMachine::DecorateViewModel` and `FStratGuidedOpening::DecorateViewModel`
 * use. `StratBuildViewModel` leaves it default-constructed -- not hovering -- exactly as it
 * leaves the guidance block inactive.
 */
USTRUCT(BlueprintType)
struct FStratHoverView
{
	GENERATED_BODY()

	/**
	 * Whether the cursor is over a board hex at all.
	 *
	 * FALSE IS THE ORDINARY STATE AND NOT A FAULT -- the cursor is off the board, over a
	 * unit's own mesh, over a widget, or the game is not being played with a mouse.
	 * `AStratPlayerController::HexUnderCursor` already treats all of those as one answer and
	 * this field is that answer, projected.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	bool bHasHoveredHex = false;

	/**
	 * The hex, X = q and Y = r, in the same coordinates every other hex in this model uses.
	 *
	 * MEANINGLESS WHEN `bHasHoveredHex` IS FALSE, and left at (0,0) then rather than at some
	 * out-of-band value, precisely so that a consumer which forgets to check the flag draws
	 * something obviously wrong at a corner rather than something plausible in the middle.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	FIntPoint HoveredHex = FIntPoint::ZeroValue;
};

/**
 * The counter line's stated reason -- §2.11.3's "a number, `out of range`, or
 * `defender destroyed`", as one value.
 *
 * A PRESENTATION MAPPING AND NOT A COMBAT RULE, and this is the one place on the card
 * where that distinction is load-bearing enough to spell out. Every other field on
 * `FStratForecastView` is a module answer copied across; this enumerator is CHOSEN, by
 * `StratComposeForecastView`, out of two module booleans and in a fixed order:
 *
 *     `bDefenderDies`  -> `DefenderDestroyed`
 *     `bCounterFires`  -> `Number`
 *     otherwise        -> `OutOfRange`
 *
 * WHY THE ORDER IS SAFE TODAY. `strat::uiForecast` computes the counter inside
 * `if (!f.defenderDies)`, so `counterFires` is false whenever `defenderDies` is true and
 * the first two arms cannot both match. `Combat.h::defenderCanCounter` is documented
 * "True only when distance is inside the defender's [rangeMin, rangeMax] band", so range
 * is the ONLY thing that can make `counterFires` false on a surviving defender, and the
 * third arm's words are exact.
 *
 * THIS IS THE ONE PLACE A LATER RULES CHANGE COULD MAKE THE CARD LIE, and it would do so
 * QUIETLY -- no compiler and no existing clause would report it. Two changes upstream do
 * it. (1) If `defenderCanCounter` ever refuses for a reason that is not range -- a
 * suppression, an ammunition state, a type rule -- the card says `out of range` about a
 * defender that is in range, and the mapping is still total, still exhaustive and still
 * wrong. (2) If `uiForecast` ever lets a dying defender counter, the first arm silently
 * swallows a counter that fires. Neither is arithmetic and neither moves a number, which
 * is why a numeric parity gate cannot see either one.
 *
 * `Distance`, `bCounterFires` AND `bDefenderDies` ARE ALL CARRIED BESIDE THIS FIELD for
 * exactly that reason: a gate can assert the enumerator against the inputs it was chosen
 * from, and against the defender's own range, without this file being the only witness.
 *
 * NO `None`. There is no card without a forecast -- `bHasForecast` is the flag for that --
 * and a fourth enumerator would be a counter line §2.11.3 says is "never omitted".
 */
UENUM(BlueprintType)
enum class EStratCounterReason : uint8
{
	/** The counter fires. `CounterDamage` is the number the line shows. */
	Number            UMETA(DisplayName = "A number"),

	/** The defender survives and cannot reach back. `Counter 0 -- out of range`. */
	OutOfRange        UMETA(DisplayName = "Out of range"),

	/** The defender does not survive to counter. `Counter 0 -- defender destroyed`. */
	DefenderDestroyed UMETA(DisplayName = "Defender destroyed")
};

/**
 * §2.11.3's attack forecast card, as the widget needs to see it. THE CENTREPIECE DISPLAY'S
 * WHOLE MODEL.
 *
 * WHAT GAP THIS CLOSES. `FStratHoverView` above landed wave 0's half -- where the cursor is
 * -- and its own block says in as many words that "neither the forecast struct nor the info
 * panel struct is here, because each is its own wave". This is the forecast wave. Before it,
 * the six readouts §2.11.3 requires had no reflected home at all, and three of them had no
 * source anywhere on the engine side: `strat::UiForecast` carries seven fields and none of
 * them is an HP-after, a terrain bonus or a kill award.
 *
 * PART OF THE MODEL AND NOT OF A WIDGET, for `FStratGuidanceView`'s reason and
 * `FStratHoverView`'s: a card that appears because of a hover is a visible element, and
 * T-INT-05's "rebuild the screen from the view model alone" stops holding the moment one
 * visible element reads from somewhere else. A `BlueprintPure` on the controller returning a
 * forecast would work on the first day and would make that invariant false with nothing
 * reporting it.
 *
 * EVERY NUMBER BELOW IS `FStratBridge::AttackForecast`'s, COPIED. That method's own block
 * accounts for each one -- which are `strat::uiForecast`'s, which is
 * `strat::killAward`'s, which is a terrain-table read, and which two are the clamps it
 * writes out because `strat::uiResolveForGate` may not be called from production. NOTHING
 * IS COMPUTED IN THIS FILE except the three presentation shapings named on their own
 * fields: `CounterReason`, `bFlagAtRisk` / `RiskedFlagSide`, and `bShowAttackerHp`. Each is
 * a CHOICE OVER BOOLEANS, not a number, and each says so where it is declared. The header
 * block's no-arithmetic census stands: no field here is an addition, a subtraction or a
 * comparison of two quantities.
 *
 * THE WIDGET SHAPES NOTHING FURTHER, which is T-UI-03's standing rule and the reason
 * `bShowAttackerHp` exists at all rather than being left as `CounterDamage > 0` in a
 * binding. The card's remaining work is formatting: a number into a string, an enumerator
 * into a sentence, a side into a colour.
 *
 * WRITTEN BY `StratDecorateForecast` (`Source/StratPlay/StratForecastQuery.h`), on the
 * decoration seam `FStratSelectionMachine`, `FStratGuidedOpening` and `FStratHoverState`
 * already use, and AFTER the hover decorator because it reads `FStratViewModel::Hover`. That is the first ordering constraint any decorator but
 * `Observe` has carried, and it is recorded on the producer as well as here.
 * `StratBuildViewModel` leaves this default-constructed -- no card -- exactly as it leaves
 * the hover not-hovering and the guidance block inactive.
 *
 * NOT IN THIS ROUND, with reasons:
 * - NO TEXT. Not the terrain phrase, not `Destroys Tank · +150 Fame`, not `FLAG AT RISK`.
 *   Those are formatted from these fields, and putting the sentence here would make this
 *   struct the place a localisation change lands.
 * - NO ATTACK RING AND NO RANGE-1 HOLE. §2.11.3's dead-zone drawing is the overlay's, off
 *   `FStratSelectionMachine::BuildOverlays`, and it is about the SELECTION rather than
 *   about one hovered target.
 * - NO §2.11.6 ONE-SHOT REASSURANCE LINE. That is the guidance layer's beat and
 *   `FStratGuidanceView` is where it belongs.
 */
USTRUCT(BlueprintType)
struct FStratForecastView
{
	GENERATED_BODY()

	/**
	 * Whether there is a card at all.
	 *
	 * READ FIRST BY EVERY CONSUMER, on `FStratHoverView::bHasHoveredHex`'s rule and for its
	 * reason: no value of the numbers below can mean "no forecast" -- zero damage against a
	 * zero-HP unit is a real forecast -- so the absence needs its own flag. True requires all
	 * of: a unit selected, a hovered hex, the bridge answering, and the rules module calling
	 * the attack LEGAL. That last conjunct is what makes this exactly §2.11.3's "over a lit
	 * target": `FStratBridge::AttackTargetHexes` lights a hex on the same `legal` answer, so
	 * the card and the highlight cannot disagree about which hexes are targets.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Forecast")
	bool bHasForecast = false;

	/**
	 * The two participants, by unit id.
	 *
	 * THE ATTACKER IS THE SELECTION AND IS ECHOED so a card can be proved to be about the
	 * unit the player selected rather than about whatever was selected when it was built.
	 * The defender is the bridge's answer to "who did I forecast against" -- see
	 * `FStratAttackForecast::DefenderUnitId` on why that is not a second author of the fact
	 * `Units` states by hex.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Forecast")
	int32 AttackerUnitId = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Forecast")
	int32 DefenderUnitId = INDEX_NONE;

	/** `You deal N dmg`. `strat::resolveDamage`'s answer, through the bridge. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Forecast")
	int32 Damage = 0;

	/**
	 * `Tank 20 -> 17`. §2.11.3's before-and-after for the defender.
	 *
	 * BOTH HALVES CARRIED, though `FStratUnitView::Hp` already states the "before". The pair
	 * is the readout, the bridge produced them from one read of one board, and splitting them
	 * across two authors is how a card comes to show a before from this frame beside an after
	 * from the last. That is the same reasoning `FStratAttackForecast::DefenderUnitId` gives,
	 * and it is the deliberate exception to the lookup-not-duplication preference
	 * `FStratHoverView` states.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Forecast")
	int32 DefenderHpBefore = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Forecast")
	int32 DefenderHpAfter = 0;

	/** Whether a counter fires at all. `strat::defenderCanCounter`'s answer, through the bridge. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Forecast")
	bool bCounterFires = false;

	/** The counter line's number. Zero and meaningless unless `bCounterFires`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Forecast")
	int32 CounterDamage = 0;

	/**
	 * Which of §2.11.3's three counter phrasings this line takes.
	 *
	 * THE ONE CHOSEN FIELD ON THIS CARD, and `EStratCounterReason`'s own block is the
	 * authority on how it is chosen, why the choice is exact today, and the two upstream
	 * changes that would make it lie without moving a number.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Forecast")
	EStratCounterReason CounterReason = EStratCounterReason::OutOfRange;

	/**
	 * The attacker's own before-and-after, for the counter.
	 *
	 * SHOWN ONLY WHEN `bShowAttackerHp`, which is §2.11.3's "and for the attacker whenever
	 * the counter is nonzero". Filled whenever there is a forecast, because the attacker's
	 * HP-before is true regardless and the after equals it when nothing fires.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Forecast")
	int32 AttackerHpBefore = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Forecast")
	int32 AttackerHpAfter = 0;

	/**
	 * Whether the attacker's HP row is drawn. §2.11.3's "whenever the counter is nonzero".
	 *
	 * A SHAPING, AND HERE RATHER THAN IN A BINDING BECAUSE OF T-UI-03. `CounterDamage > 0`
	 * in a widget graph is a comparison of two quantities inside the layer that is
	 * explicitly not allowed to shape numbers, and "it is only a comparison" is the argument
	 * that ends with a widget doing arithmetic. It is chosen here, once, and the widget binds
	 * visibility straight to it.
	 *
	 * IT IS NOT `bCounterFires`. A counter that fires for zero damage moves no HP, and a
	 * `20 -> 20` row would teach the player that the counter did something.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Forecast")
	bool bShowAttackerHp = false;

	/**
	 * `(Woods +20%)` -- the defender's terrain, named inline, every time.
	 *
	 * SIGNED, AND A WIDGET THAT WRITES `+` UNCONDITIONALLY IS WRONG. `strat::TerrainDef::
	 * defensePct` is signed and its own comment names the case: `Bridge is -10 (§2.3)`.
	 * §2.11.3 requires the modifier named so that terrain defense "must never read as hidden
	 * dice", and a bridge hex showing `+-10%` or `+10%` breaks that in opposite directions.
	 *
	 * IT IS THE DEFENDER'S HEX. The §3 invariant is that it always is; the card's placement
	 * of the modifier beside the defender is what teaches it, per §2.11.3. The attacker's
	 * hex has a percentage too -- the counter's -- and it is deliberately not on this card.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Forecast")
	int32 DefenderTerrainDefensePct = 0;

	/** That terrain row's own `id` ("Woods", "Bridge", ...). `FStratHexView::TerrainId`'s lookup, for the hex the damage was computed on. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Forecast")
	FName DefenderTerrainId;

	/** Whether this attack kills. `UiForecast::defenderDies`, the module's own field. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Forecast")
	bool bDefenderDies = false;

	/**
	 * The `+N Fame` of §2.11.3's `Destroys Tank · +150 Fame`, and of the enemy-flag band's
	 * `+500 · Decisive victory`. ZERO WHEN THE DEFENDER LIVES.
	 *
	 * `strat::killAward`'s ANSWER AND NOT HALF A COST. One function serves both lines,
	 * because Q5 makes the flat 500 a REPLACEMENT for the ordinary award rather than a
	 * second rule -- so a widget that special-cased the flag would be restating a ruling the
	 * rules module already made.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Forecast")
	int32 KillAwardFame = 0;

	/**
	 * §2.11.3's `FLAG AT RISK -- this attack ends the match` band.
	 *
	 * A SHAPING OVER FOUR BOOLEANS, chosen in `StratComposeForecastView`: true when the
	 * forecast is lethal to EITHER flag -- the defender is the flag and dies, or the attacker
	 * is the flag and the counter kills it. §2.11.3 requires both directions: "No player can
	 * end a match, theirs or the enemy's, without having been told on the card they clicked."
	 *
	 * THE ATTACKER-SIDE HALF RESTS ON `FStratAttackForecast::bAttackerDies`, which is the
	 * BRIDGE'S and not the module's -- `strat::UiForecast` has no attacker-death field. That
	 * asymmetry is recorded on the bridge struct; it is named here too because this band is
	 * the only thing on the card that depends on it.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Forecast")
	bool bFlagAtRisk = false;

	/**
	 * Which side's flag is at risk, or `INDEX_NONE`. Meaningless when `bFlagAtRisk` is false.
	 *
	 * A SIDE AND NOT A `bIsOwnFlag`. §2.11.3 colours the band red for the player's own flag
	 * and gold for the enemy's, and the shortcut -- "the attacker is the viewer, because you
	 * can only select your own units" -- is true today and is exactly the premise a hot-seat
	 * hand-over breaks without saying so. The widget compares this against
	 * `FStratViewModel::ViewingSide`, which is the same distinction
	 * `FStratSelectionMachine` records between the viewing side and the side to move.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Forecast")
	int32 RiskedFlagSide = INDEX_NONE;

	/**
	 * `UiForecast::distance` -- `Hex.h`'s answer.
	 *
	 * NOT ON THE CARD, AND CARRIED ANYWAY. §2.11.3 shows no distance readout. It is here
	 * because `CounterReason`'s `OutOfRange` arm is an inference ABOUT this number, and a
	 * clause that cannot see the distance can only assert the mapping against the same two
	 * booleans the mapping was chosen from -- which is a test agreeing with itself. See
	 * `EStratCounterReason`.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Forecast")
	int32 Distance = 0;
};

/**
 * The whole view model: everything that should be on screen, in engine types.
 *
 * A VALUE, REBUILT, NEVER PATCHED. Phase 3's `ApplyView` reconciles actors against this
 * on every refresh -- spawn what is here and absent, move what is here and misplaced,
 * destroy what is absent and present -- which is only sound because this is a complete
 * statement rather than a difference from a previous one. Nothing below records what
 * changed, and nothing should.
 *
 * ORDERS ARE THE MODULE'S AND ARE LOAD-BEARING. `Hexes` and `Factories` are in
 * canonical hex order (ascending r, then ascending q -- `strat::hexLess`) and `Units` is by
 * ascending unit id, because `strat::buildUiSnapshot` emits them that way so that two
 * runs on the same state produce the same bytes. This builder appends in the order it
 * reads and sorts nothing; a consumer that reorders them is discarding a determinism
 * property it did not pay for.
 *
 * IT CARRIES NO POINTER INTO RULES MEMORY. `FStratBridge::MakeUiWorld` hands out
 * borrowed pointers and its own header warns that the world must not outlive the bridge;
 * this struct is made of copies, so a view model can be held across a reseed, compared
 * against a later one, or sent to a Blueprint without anyone having to reason about the
 * bridge's lifetime.
 */
USTRUCT(BlueprintType)
struct FStratViewModel
{
	GENERATED_BODY()

	/** Every hex of the board, canonical order. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	TArray<FStratHexView> Hexes;

	/** Every living unit, ascending unit id. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	TArray<FStratUnitView> Units;

	/** Every factory, canonical hex order. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	TArray<FStratFactoryView> Factories;

	/**
	 * One entry per side, indexed by `strat` side index.
	 *
	 * A `TArray` and not two named members, because `strat::SIDE_COUNT` is the rules
	 * module's number and writing `You`/`Enemy` here would bake §2.11.4's two-column
	 * panel into the general description of the board. `StratBuildScoreboardModel` is
	 * where the two-side refusal lives, and it lives there because that panel is what has
	 * two columns -- the view model does not.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	TArray<FStratSideView> Sides;

	/** The N of "objectives held X of N", supplied by the scenario (§2.13). Mirrors
	 *  `UiSnapshot::objectiveTotal`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 ObjectiveTotal = 0;

	/** Turn, cap, side to move, result. Mirrors `UiSnapshot::match`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	FStratMatchView Match;

	/**
	 * Which `strat` side this model is rendered FOR.
	 *
	 * THE CALLER'S STATEMENT, NOT THE RULES MODULE'S, and the only field in this struct
	 * that is not sourced from the snapshot. It is here rather than in the actor that
	 * draws it so that "what should be on screen" is a function of this value alone --
	 * a viewing side held beside the model is a second input, and T-INT-05 would then be
	 * about two things.
	 *
	 * IT SELECTS AND NEVER MUTATES. Nothing about the board or the match changes when it
	 * does; `Sides` still carries every side, and only the reading of it moves.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 ViewingSide = 0;

	/**
	 * §2.11.6's guided opening, as the strip and the ring need to see it.
	 *
	 * PART OF THE MODEL AND NOT OF THE CONTROLLER, for `bLockedThisTurn`'s reason exactly
	 * and stated here because the temptation is stronger: the directive strip is a widget
	 * with one line of text, and hanging that text off a `BlueprintPure` on
	 * `AStratPlayerController` would work on the first day and would make T-INT-05 false --
	 * "rebuild the screen from the view model alone" stops holding the moment one visible
	 * element reads from somewhere else. `Ui.h`'s presentation-block note is the precedent
	 * and it is quoted in this header's block above.
	 *
	 * WRITTEN BY `FStratGuidedOpening::DecorateViewModel`, between `StratBuildViewModel`
	 * and `ApplyView`, on the same seam `FStratSelectionMachine::DecorateViewModel` uses.
	 * `StratBuildViewModel` leaves it default-constructed -- inactive, no beat -- exactly as
	 * it leaves the two unit bits false.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	FStratGuidanceView Guidance;

	/**
	 * Which hex the cursor is over. §2.11.3 / §2.11.2, wave 0.
	 *
	 * WRITTEN BY `FStratHoverState::DecorateViewModel`, on the decoration seam, exactly as
	 * `Guidance` and the two unit bits are. `StratBuildViewModel` leaves it
	 * default-constructed, which is "not hovering" -- and that default is load-bearing rather
	 * than incidental: every model built for a hand-over, a gate, an AI turn or a reconcile
	 * this controller did not start says "not hovering", which is the truth for all of them.
	 *
	 * IT DOES NOT MOVE A UNIT, SELECT ONE, OR CHANGE ANY OTHER FIELD OF THIS STRUCT. A hover
	 * is a statement about the cursor and never about the board; `FStratSelectionMachine` is
	 * not consulted when it changes and holds no copy of it.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	FStratHoverView Hover;

	/**
	 * §2.11.3's attack forecast card. Wave 1.
	 *
	 * WRITTEN BY `StratDecorateForecast`, on the decoration seam, and
	 * AFTER `FStratHoverState::DecorateViewModel` -- it reads `Hover` and is the first
	 * decorator with an ordering constraint that is not `FStratGuidedOpening::Observe`'s.
	 * `StratBuildViewModel` leaves it default-constructed, which is "no card", and that
	 * default is load-bearing for the hover field's reason: every model built for a
	 * hand-over, a gate, an AI turn or a reconcile nobody hovered during says "no card",
	 * which is the truth for all of them.
	 *
	 * IT IS A STATEMENT AND NOT AN EVENT. Nothing here records that a card APPEARED; the
	 * card is on screen on exactly the frames this says it should be, and is rebuilt from
	 * this value like everything else. §4.9's ordered event list is a different thing, and
	 * `StratBridge.h` records the conflation as the trap.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	FStratForecastView Forecast;
};

/**
 * Builds the view model from the bridge. THE ONLY PLACE `strat::UiSnapshot` is turned
 * into reflected types.
 *
 * A FREE FUNCTION AND NOT A METHOD ON AN ACTOR, for the reason `StratBuildScoreboardModel`
 * is one: it draws nothing, holds no state, and can be asserted against a bridge with no
 * world, no actor and no Slate in existence. Phase 3's `ApplyView` calls it and then
 * reconciles; a test calls it and then compares field for field against a snapshot the
 * same bridge projected.
 *
 * IT CALLS EXACTLY TWO BRIDGE METHODS -- `MakeUiSnapshot` for every number, and `Tables`
 * for the two `FName` lookups -- and no `strat::` free function at all. A `strat::` call
 * from this module does not link: measured as 8 x `LNK2019` when the bridge first tried
 * (`StratBridge.h:11`), and again as a single `LNK2019` when the StratUI Automation test
 * called `strat::uiCheckSnapshotFidelity` directly.
 *
 * ALL-OR-NOTHING. It builds into a local and moves it across on the last line, so a
 * refusal leaves the caller's model exactly as it found it -- the same construction
 * `StratScoreboardWidget.cpp` uses, and for the same reason: a transient refusal must not
 * be able to blank a screen that was correct a moment ago.
 *
 * REFUSES RATHER THAN PRODUCING AN EMPTY MODEL when the bridge has nothing to project,
 * forwarding `MakeUiSnapshot`'s own words. An empty view model is a legitimate value --
 * it is what a board with no hexes looks like -- so a caller handed one cannot tell it
 * apart from "not seeded yet", and reconciling against it would destroy every actor on
 * screen.
 *
 * @param ViewingSide  which `strat` side this model is for; NOT `sideToMove`. Range
 *                     checked against the snapshot's own side count.
 */
STRATUI_API bool StratBuildViewModel(
	const FStratBridge& Bridge,
	int32               ViewingSide,
	FStratViewModel&    OutModel,
	FString&            OutFailureReason);

/**
 * §2.11.5's production menu for ONE factory, in engine types.
 *
 * A SEPARATE ENTRY POINT FROM `StratBuildViewModel` rather than a field it fills, for
 * the reason `FStratBuildOptionView`'s own block gives: the buildlist is a query one
 * surface asks, not state every consumer of the view model should carry.
 *
 * ALL-OR-NOTHING, exactly as `StratBuildViewModel` is. It fills a local and moves it
 * across on the last line, so a refusal leaves the caller's array as it found it and a
 * transient failure cannot blank a menu that was correct a moment ago.
 *
 * REFUSES RATHER THAN PRODUCING AN EMPTY MENU, forwarding `FStratBridge::BuildOptions`'s
 * own words. The menu carries one row per §2.4 row, so an empty array is never an
 * answer.
 *
 * @param Side        which `strat` side is shopping. Range checked by the bridge.
 * @param FactoryHex  X = q, Y = r -- the encoding `FStratFactoryView::Hex` already
 *                    carries, so a caller passes the view model's own value back in.
 */
STRATUI_API bool StratBuildProductionMenu(
	const FStratBridge&            Bridge,
	int32                          Side,
	FIntPoint                      FactoryHex,
	TArray<FStratBuildOptionView>& OutOptions,
	FString&                       OutFailureReason);

/**
 * §2.8's result WHOLE, in engine types. The projection §2.11.4's end-of-match screen
 * needs and the one `StratBuildViewModel` structurally cannot supply.
 *
 * A SEPARATE ENTRY POINT, for `StratBuildProductionMenu`'s reason exactly: it is a query
 * one surface asks and not state every consumer of the view model should carry. See
 * `FStratMatchResultView`'s own block for the three places that decision has now been
 * made in a row.
 *
 * ALL-OR-NOTHING, as both functions above are. It fills a local and assigns on the last
 * line, so a refusal leaves the caller's value exactly as it found it -- which matters
 * more here than anywhere else in this file: a result screen blanked by a transient
 * refusal would announce a match still in progress.
 *
 * REFUSES ON AN UNSEEDED BRIDGE rather than answering InProgress, forwarding
 * `FStratBridge::MatchResult`'s own words. That method's declaration records why it
 * diverges from upstream on exactly this case.
 *
 * IT DOES NOT CHECK WHETHER THE MATCH HAS ENDED and never refuses over it. An
 * in-progress match is a legitimate answer -- tier `InProgress`, winner `INDEX_NONE` --
 * and a caller that wants the boolean reads `FStratMatchView::bHasResult` off the model
 * the screen was drawn from.
 */
STRATUI_API bool StratBuildMatchResult(
	const FStratBridge&    Bridge,
	FStratMatchResultView& OutResult,
	FString&               OutFailureReason);

/**
 * Copies `FStratBridge::AttackForecast`'s answer into the reflected card, and performs
 * §2.11.3's three presentation shapings. THE LAST PLACE A NUMBER MAY BE SHAPED.
 *
 * A FREE FUNCTION IN StratUI AND NOT A METHOD ON THE DECORATOR THAT CALLS IT, which is a
 * placement decision and the reason is this header's own census. The block at the top of
 * this file states what every field of the view model may be -- a snapshot mirror, a table
 * lookup, and since 2026-08-25 exactly one arithmetic exception -- and that census can only
 * govern code it can see. A compose living in `StratPlay` would fill this model from outside
 * the file that says what filling it is allowed to mean. It is also `StratBuildViewModel`'s
 * own shape, for `StratBuildViewModel`'s own reason: it can be asserted with no actor, no
 * world, no controller and no Slate in existence.
 *
 * IT TAKES THE BRIDGE'S PLAIN STRUCT AND NOT THE BRIDGE. So a gate can hand it a
 * hand-built `FStratAttackForecast` -- a counter-kill on a flag, a negative terrain
 * percentage, a lethal forecast against a flag on the far side -- and pin the shapings over
 * positions no fixture on this project's one scenario reaches. Half of §2.11.3's card is
 * only exercisable that way, and `a-correct-mapping-can-be-unreachable-by-test` is what
 * happens when it is not.
 *
 * THE THREE SHAPINGS, ALL DECLARED ON THEIR OWN FIELDS AND ALL CHOICES OVER BOOLEANS:
 * `CounterReason` (see `EStratCounterReason` for the one lie-vector on this card),
 * `bFlagAtRisk` / `RiskedFlagSide`, and `bShowAttackerHp`. EVERY OTHER FIELD IS A COPY.
 * Nothing here adds, subtracts or divides; the two subtractions the card needs were done in
 * `FStratBridge::AttackForecast`, and that method's block says why they could not be asked
 * for instead.
 *
 * AN ILLEGAL FORECAST PRODUCES `bHasForecast` FALSE AND NOTHING ELSE. The bridge already
 * leaves its own struct empty on that path; this restates the emptiness rather than trusting
 * it, so a card cannot be assembled out of a refusal's leftovers.
 *
 * ALL-OR-NOTHING, as every builder in this file is: it fills a local and assigns on the last
 * line.
 */
STRATUI_API void StratComposeForecastView(
	const FStratAttackForecast& Source,
	FStratForecastView&         OutForecast);
