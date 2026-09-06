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
// THE TABLE-READ CLAUSE ABOVE WIDENED ON 2026-08-27 AND IS AMENDED RATHER THAN REWRITTEN,
// because "the single addition is `TerrainId` / `DefId`" was true of the fields when it
// was written and a reader who remembers the sentence needs to see which half moved. Wave
// 2 added `FStratHexView::TerrainMoveCost` and `::TerrainDefensePct`, which are the SAME
// row read at the SAME index in the SAME loop as `TerrainId` -- so what changed is the
// number of fields taken off that read, not the number of reads and not the KIND of thing
// the model may contain. The arithmetic count did not move: there is still exactly one
// arithmetic exception in this pair (`FStratBuildOptionView::Shortfall`) and it is still
// outside the model. No count of table reads is stated here on purpose -- a census written
// inside a growing thing is invalidated by the next thing that grows it, which this repo
// has recorded happening to this very file's `static_assert` count twice.
//
// AND ONE LAYER THAT IS NEITHER A MIRROR NOR A TABLE READ: `FStratInfoPanelView`, filled
// by `StratDecorateInfoPanel`, is a SELECTION over fields of this same struct. It reads
// `Hexes`, `Units` and `Hover` and reaches nothing else -- no bridge, no snapshot, no
// table -- so it can only restate numbers the census above already governs. That is why
// it does not need a clause of its own, and why its function takes the model rather than
// a bridge; see both blocks for the reasoning stated where it binds.
//
// AND ONE FIELD THAT IS NEITHER A MIRROR, A TABLE READ, NOR A SELECTION, ADDED 2026-09-01:
// `FStratFactoryView::bBuildPulse`. It is `FStratFactoryBuildPulse::bShouldPulse`, copied
// whole off `FStratBridge::FactoryBuildPulses` -- an answer the BRIDGE composed, in the one
// module where `strat::` symbols link, out of two facts that reach this file through
// different doors. So the census gains a FOURTH KIND of field, named here rather than left
// for a reader to classify as the mirror it is not: a value copied from a bridge QUERY
// rather than from the snapshot.
//
// THE ARITHMETIC COUNT DID NOT MOVE, and the narrowed no-arithmetic claim above is untouched
// by this. There is no `+`, no `-`, no `/` and no `.size()` written for this field anywhere
// in this pair; the only operation behind it is an OR over booleans `strat::uiBuildOptions`
// already decided, and it is taken in `StratBridge.cpp` where that block declares it.
// `FStratBuildOptionView::Shortfall` is still the one arithmetic exception and is still
// outside the model.
//
// WHY IT IS A FIELD AND NOT LEFT TO THE TILE TO COMPOSE. §2.11.5's pulse is
// `available && any(affordable)`, and only the first of those has a home on this model --
// affordability lives per row on `FStratBuildOptionView`, behind a query. A tile that
// composed it for itself would have to open a production menu per factory, which
// `UStratMatchSubsystem::RefreshProductionMenu` may not be asked to do (it is ALL-OR-NOTHING
// against one menu slot, per its own block), and the composition would land in the layer
// T-UI-03 governs. Carrying the answer keeps T-INT-05 true in the shape it is written:
// the pulse is rebuildable from the view model alone.
//
// AND ONE BLOCK THAT IS A FIFTH KIND AGAIN, ADDED 2026-09-02: `FStratCommandBarView`,
// filled by `StratDecorateCommandBar`. Two of its five fields are DECLARED COPIES of
// `FStratGuidanceView`'s, which is the selection kind above. The other three are a FOLD --
// booleans the decorator forms out of fields of this same model, plus one named selector
// over that same model (`UStratViewModelLibrary::CountViewingSideUnitsAbleToAct`). Named
// here rather than left for a reader to classify, on the same rule the fourth kind was
// named under.
//
// THE ARITHMETIC COUNT DID NOT MOVE, AND THE DISTINCTION THAT KEEPS IT STILL IS WORTH ITS
// LINE. No number is stored: the count is produced by a `BlueprintPure` selector, compared
// against zero inside the decorator, and only the BOOLEAN survives onto the model.
// `StratViewModelLibrary.h`'s own block already ruled that a count FIELD would be "the first
// piece of ARITHMETIC INSIDE THE MODEL" and that computing it over the model on demand is
// not; this block is the shape that ruling permits. There is still exactly one arithmetic
// exception in this pair (`FStratBuildOptionView::Shortfall`) and it is still outside the
// model.
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
	 * The same terrain row's `moveCost` and `defensePct` -- §2.11.2's info panel reads
	 * `Factory · move 1 · def +15%` off exactly these two.
	 *
	 * THE SAME TABLE READ `TerrainId` ALREADY IS, at the same index, in the same loop, off
	 * the same borrowed `strat::RulesTables`. That is the whole reason they are HERE rather
	 * than in a struct of their own: the terrain row a hex names is read once, and three
	 * fields taken off one read cannot disagree about which row was read. A parallel
	 * "hex info" struct filled from a second lookup would be a second chance to read a
	 * different row -- the failure `FStratAttackForecast`'s terrain block already refuses
	 * from the other direction ("so the bonus shown IS the bonus `resolveDamage` was
	 * handed").
	 *
	 * THEY ARE ON EVERY HEX AND NOT ONLY ON THE HOVERED ONE, deliberately. §2.5's path
	 * preview wants "the terrain-cost tick per hex" (§2.11.2's own hover row says so), and
	 * a per-hover-only field would send that consumer back to the table for a number the
	 * model already had.
	 *
	 * `TerrainMoveCost` CARRIES `Data.h`'s §4.8 SENTINEL UNCHANGED: 0 means impassable and
	 * is not a cheap hex. It is passed through rather than re-encoded, because re-encoding
	 * it here would make this file the author of a rules constant. The one place that
	 * sentinel is turned into a boolean a widget can bind to is
	 * `FStratInfoPanelView::bTerrainImpassable`, and its own block says why it is there and
	 * not here.
	 *
	 * `TerrainDefensePct` IS SIGNED AND THE SIGN IS LOAD-BEARING. `Data.h` says so on the
	 * field itself -- "SIGNED -- Bridge is -10 (§2.3)" -- so a panel that prints `+N%`
	 * unconditionally lies on a Bridge hex. `FStratForecastView::DefenderTerrainDefensePct`
	 * carries the same number for the defender's hex and records the same warning; the two
	 * are the same table field read for two different hexes and neither derives the other.
	 *
	 * A LOOKUP, NOT A DERIVATION, on `TerrainId`'s terms exactly. Nothing computes either
	 * number, and if one is wrong the table is wrong.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 TerrainMoveCost = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 TerrainDefensePct = 0;

	/**
	 * The same row's `capturable` and `incomeFame` -- §2.11.2's fourth readout, "status if
	 * capturable", which renders as `· yours (+100/turn)` or `· neutral` or `· enemy`.
	 *
	 * `bTerrainCapturable` IS NOT INFERRABLE FROM `Owner` AND THAT IS THE WHOLE REASON IT
	 * IS HERE. `Ui.h` says `UiHexView::owner` is `OWNER_NEUTRAL` on "capturable hexes only;
	 * OWNER_NEUTRAL elsewhere" -- so a NEUTRAL FACTORY and a PLAINS HEX both project
	 * `Owner == INDEX_NONE` and are indistinguishable by that field. A panel that inferred
	 * capturability from `Owner` would print `· neutral` beside every Plains hex on the
	 * board, which is the kind of defect that looks like a formatting choice.
	 *
	 * `TerrainIncomeFame` IS §2.7'S RATE FROM THE TABLE, and it is the hex's own -- Factory
	 * 100, Town 25, else 0, per `Data.h`'s comment on the field. It is NOT
	 * `FStratSideView::IncomePerTurn`, which is a SIDE's total across every objective it
	 * holds; the two are a row and a sum and confusing them would print a side's whole
	 * economy beside one tile.
	 *
	 * CARRIED ON EVERY HEX, INCLUDING THE ONES WHERE IT IS ZERO, and not suppressed on
	 * non-capturable rows. The table says 0 for those and 0 is the true rate; suppressing
	 * it would mean this file deciding when income is meaningful, which is
	 * `bTerrainCapturable`'s job to say and the panel's job to render.
	 *
	 * BOTH ARE THE SAME ROW READ, on `TerrainMoveCost`'s terms exactly.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	bool bTerrainCapturable = false;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 TerrainIncomeFame = 0;

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

	/**
	 * This unit's §2.4 row stats: attack, defence, move allowance and the range band.
	 * §2.11.2's info panel prints them as `Atk/Def/Move/Range`.
	 *
	 * A TABLE READ, EXACTLY AS `DefId` IS -- same `strat::UnitDef` row, same `DefIndex`,
	 * same bounds check, same loop, one binding. Nothing computes them and if one is wrong
	 * the table is wrong. They are here rather than in the info panel's own struct for
	 * `FStratHexView::TerrainMoveCost`'s reason: the row a unit names is read once, and
	 * fields taken off one read cannot disagree about which row was read.
	 *
	 * THE `Stat` PREFIX IS DISAMBIGUATION AND NOT DECORATION. `DefIndex` and `DefId` on
	 * this same struct mean DEFINITION, and a bare `Def` beside them would read as a third
	 * spelling of that rather than as §2.4's defence stat. The prefix costs five characters
	 * and removes a misreading that a reviewer would have to catch by attention.
	 *
	 * `StatHpMax` IS ABSENT ON PURPOSE, though the row carries `hpMax`. `HpMax` above is
	 * the SNAPSHOT's, per-instance, and is the number §2.11.2's `12/20` is drawn from. A
	 * second copy read off the definition row would be a second author of the same fact,
	 * and the two would part company the day a rule scales a unit's maximum.
	 *
	 * THE RANGE IS TWO NUMBERS BECAUSE `UnitDef` HAS TWO. §2.11.2 writes one word,
	 * `Range`, and rendering `1` where min equals max and `2-3` where it does not is a
	 * formatting decision the widget makes from these; collapsing them here would throw
	 * away the artillery minimum, which is a rule (§2.6) and not a display detail.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 StatAtk = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 StatDef = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 StatMove = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 StatRangeMin = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	int32 StatRangeMax = 0;

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
	//
	// THAT MACHINE IS NO LONGER PHASE 3'S FUTURE, AND THIS BLOCK IS A DESIGN STATEMENT
	// RATHER THAN A DEBT. Stamped 2026-09-06 because `StratViewModel.cpp` pointed here for
	// "the debt and its discharge condition" and there is neither to find, which sent a
	// reader hunting; that pointer is retracted at the site. `FStratSelectionMachine` and
	// `FStratGuidedOpening` both exist and both write these two fields on the decoration
	// seam, through `AStratPlayerController::DecorateForPresentation`. NOTHING ABOVE
	// CHANGES AND NOTHING IS DISCHARGED: the arrangement described -- built false here,
	// overlaid there -- is the shipped one and is permanent, so the sentence stating why
	// these fields live on the model instead of in an actor is as load-bearing as the day
	// it was written.

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

	/**
	 * §2.11.5's BUILD pulse for this factory, from the model's `ViewingSide`. Copied whole
	 * off `FStratFactoryBuildPulse::bShouldPulse`, which `FStratBridge::FactoryBuildPulses`
	 * composes.
	 *
	 * THE ONLY FIELD ON THIS STRUCT THAT IS NOT A SNAPSHOT MIRROR, and the header block
	 * above states what that costs the census. It is here rather than one layer up because
	 * its second half -- whether any §2.4 row is affordable to this side -- has no home on
	 * this model at all: affordability is per row on `FStratBuildOptionView`, behind
	 * `StratBuildProductionMenu`, which opens a menu.
	 *
	 * IT IS PER VIEWING SIDE AND NOT PER OWNER, which is the same distinction
	 * `StratBuildViewModel`'s `ViewingSide` block already draws and matters here for the
	 * same hot-seat reason: the pulse is a nudge to the player looking at the screen, and on
	 * every other turn the viewing side and `sideToMove` differ.
	 *
	 * SO THIS FIELD IS FALSE FOR EVERY FACTORY ON HALF OF ALL TURNS, AND A DARK BOARD IS NOT
	 * A DEFECT. [ADDED 2026-09-01, when `AStratBoardActor::ShowBuildPulses` became the first
	 * reader.] The argument -- `bShouldPulse` inheriting `side == activeSide` from
	 * `strat::canBuildAt` -- lives at `FStratFactoryBuildPulse::bShouldPulse` in
	 * `Source/StratBridge/StratBridge.h` and is deliberately not copied here.
	 *
	 * IT IS NOT `!bHasBuiltThisTurn && <affordable>`, AND THE DIFFERENCE IS DELIBERATE AND
	 * ARGUED WHERE IT IS DECIDED. `FStratFactoryBuildPulse::bShouldPulse`'s own block
	 * resolves the two spellings against the rules module, proves that availability CONTAINS
	 * the build allowance, and names the falsifier that separates them -- an enemy-held
	 * factory that has not built this turn. Nothing between that expression and this field
	 * recomputes any part of it.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	bool bBuildPulse = false;
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
 * §2.11.2's info panel -- the bottom-left, hover-driven, never-modal three lines.
 *
 * WHAT GAP THIS CLOSES. §2.11.2 names two rows -- the hovered HEX ("terrain name, move
 * cost, defense bonus, and status if capturable") and the hovered UNIT ("name, HP as
 * `12/20`, Atk/Def/Move/Range, and `ready` or `done`", plus the flag's own line. NO COUNT
 * OF THOSE READOUTS IS STATED HERE, on this file's own rule about censuses written inside
 * a growing thing; read the spec, which is the GDD's `#### 2.11.2` subsection.
 *
 * TWO DISTINCT ABSENCES ARE CLOSED AND THEY ARE NOT THE SAME KIND. MOST of the readouts
 * had NO SOURCE IN THE MODEL AT ALL -- move cost, defense bonus, capturability, the §2.7
 * income rate and all four §2.4 stats were on rows the bridge holds and on no view-model
 * field, so no widget could have shown them however it was written. `FStratUnitView::Hp`
 * and `::HpMax` are the other kind: projected since the view model's first day, correct
 * the whole time, and READ BY NOTHING OUTSIDE `Tests/`. A correct number with no route to
 * a screen is the shape this project has recorded as reading like a built feature when it
 * is not, and it is the harder of the two to notice.
 *
 * EVERY FIELD BELOW IS A SELECTION FROM `FStratViewModel` AND FROM NOTHING ELSE, and that
 * is the property the whole design is arranged around rather than a happy accident.
 * `StratDecorateInfoPanel` takes the model and no bridge, no snapshot and no table -- so
 * every number it can possibly write is already a field of the model that the header
 * block's census governs, and this struct structurally cannot introduce a number the
 * screen could not otherwise have shown. It is the same reasoning `StratComposeForecast-
 * View` states for taking the bridge's plain struct rather than the bridge, arrived at
 * from one layer further in.
 *
 * IT IS ON THE MODEL AND NOT ON A WIDGET, for `FStratViewModel::Guidance`'s reason word
 * for word: T-INT-05 requires the screen to be rebuildable from the view model alone, and
 * a panel whose content is computed inside the widget that draws it makes that false with
 * nothing failing. The panel is a visible element, so what it says is model state.
 *
 * IT IS A STATEMENT AND NOT AN EVENT. Nothing here records that the panel APPEARED or
 * that the hover MOVED. `bHasHex` false is "the panel is gone", which is §2.11.2's own
 * "Empty when nothing is hovered" -- there is no live-but-blank panel, exactly as
 * `EStratGuidanceBeat::None` records for the directive strip.
 *
 * TWO INDEPENDENT PRESENCE BITS, NOT ONE. A hovered hex with nobody on it is the common
 * case and draws one row; `bHasUnit` can never be true while `bHasHex` is false, because
 * the unit is found BY the hex. One bit could not express the ordinary case.
 *
 * THIS STRUCT WAS BUILT TWICE ON 2026-08-27 AND THE FIRST SHAPE IS RECORDED HERE RATHER
 * THAN ERASED, because the reason it was narrow is worth more than the fact that it was.
 * It first carried terrain name / cost / bonus and an HP pair, on a brief that summarised
 * §2.11.2 instead of quoting it; the deferrals were declared on this block with their
 * discharge conditions, and the corrected spec turned out to be exactly those conditions.
 * Both were then taken in the same pass and by the route the deferral named -- the terrain
 * row's `incomeFame` / `capturable` onto `FStratHexView`, the §2.4 row's stats onto
 * `FStratUnitView`, and this struct selecting both and changing shape not at all. A
 * DECLARED DEBT NAMING ITS OWN DISCHARGE IS WHY THE SECOND PASS WAS AN EXTENSION AND NOT A
 * REWRITE, which is the case this file's house style is making.
 *
 * NOT IN THIS ROUND, with reasons -- each is a real part of §2.11.2's rendered line and
 * each is named here rather than discovered missing:
 * - The WORDS `yours` / `neutral` / `enemy`, and the `·` separators, and the `+15%` sign
 *   glyph, and `12/20`'s slash. Those are the widget's. This struct carries `HexOwner`
 *   beside `FStratViewModel::ViewingSide` for the same reason
 *   `FStratForecastView::RiskedFlagSide` carries a SIDE rather than an own/enemy boolean,
 *   and that block's reasoning is cited rather than re-argued: the comparison rests on the
 *   viewer, and a boolean baked here would carry the premise that the viewer is fixed
 *   through a hot-seat hand-over.
 *   THAT IS A DIFFERENT KIND OF COMPARISON FROM `bTerrainImpassable`, and the difference
 *   is why one is here and the other is not. `Owner == ViewingSide` compares two fields of
 *   this model and means only what it says. `MoveCost == 0` compares against a MEANING the
 *   rules module declares in its own header, and a widget performing it would be a widget
 *   holding a rules constant.
 * - The flag's red edge and its `FLAG -- its loss ends the match.` sentence. `bUnitIsFlag`
 *   below is the whole of the model side; the edge colour and the sentence are the
 *   widget's, and a colour is not a view-model field.
 * - `ready` / `done` as TEXT. `bUnitDone` is the bit; which word it prints is §2.11.2's
 *   rendering and this struct does not spell either of them.
 */
USTRUCT(BlueprintType)
struct FStratInfoPanelView
{
	GENERATED_BODY()

	/**
	 * Whether the panel is on screen at all. False is §2.11.2's "empty when nothing is
	 * hovered", and it is also what a hovered hex that is not on the board gets.
	 *
	 * FALSE COVERS TWO CAUSES ON PURPOSE. `FStratHoverView::bHasHoveredHex` false, and a
	 * hovered hex absent from `FStratViewModel::Hexes`. The panel says the same thing about
	 * both because there is the same amount to say, and a "hovered a hex that does not
	 * exist" state a widget could render would be a diagnostic wearing a player-facing
	 * surface. The second cause is not a fault to refuse over either: the hover is written
	 * by a decorator that traces against a board the model was built from a moment earlier,
	 * so a hex can legitimately fall off between the two on a reseed.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	bool bHasHex = false;

	/** The hex being described. X = q, Y = r. Meaningless unless `bHasHex`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	FIntPoint Hex = FIntPoint::ZeroValue;

	/**
	 * The three terrain facts, selected from that hex's `FStratHexView` and never looked up
	 * again. See `FStratHexView::TerrainMoveCost`'s block for why the table is read once,
	 * there, and for the sign warning on `TerrainDefensePct`.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	FName TerrainId;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	int32 TerrainMoveCost = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	int32 TerrainDefensePct = 0;

	/**
	 * `TerrainMoveCost == 0`, which `Data.h` declares to mean impassable -- "0 == impassable
	 * (§4.8 sentinel)", on the field itself.
	 *
	 * A SENTINEL READ, NOT A RULE THIS FILE INVENTED. The comparison is against a meaning
	 * the rules module states in its own header, and nothing here decides what impassable
	 * costs, who may enter, or what may cross it -- `Move.h` owns all three and is not
	 * consulted, because none of those questions is on this panel.
	 *
	 * IT IS ONE LAYER BELOW THE WIDGET FOR `FStratBuildOptionView::Shortfall`'S REASON, and
	 * that precedent is cited rather than re-argued: the alternative is the widget writing
	 * `MoveCost == 0`, and T-UI-03's binding rule is what forbids a widget deciding
	 * anything about a number it was handed. Unlike `Shortfall` this is not arithmetic --
	 * it adds, subtracts and divides nothing -- so the header block's single declared
	 * arithmetic exception does not move.
	 *
	 * THE RAW COST IS STILL CARRIED ABOVE. A panel that showed only this boolean would have
	 * thrown the number away, and §2.11.2 asks for `move 1` on every passable hex.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	bool bTerrainImpassable = false;

	/**
	 * §2.11.2's fourth readout -- "status if capturable", rendered `· yours (+100/turn)` or
	 * `· neutral` or `· enemy`. All three selected from the hovered `FStratHexView`.
	 *
	 * `bHexCapturable` IS THE ONE THAT SAYS WHETHER THE CLAUSE APPEARS AT ALL, and it is a
	 * separate field rather than `HexOwner != INDEX_NONE` because those are not the same
	 * question: a NEUTRAL Factory and a Plains hex both carry `INDEX_NONE`. See
	 * `FStratHexView::bTerrainCapturable`, where the trap is recorded against `Ui.h`'s own
	 * wording.
	 *
	 * `HexOwner` IS A SIDE AND NOT AN OWN/ENEMY BOOLEAN. See this struct's "NOT IN THIS
	 * ROUND" note and `FStratForecastView::RiskedFlagSide` before adding one.
	 *
	 * `HexIncomeFame` IS THIS HEX'S §2.7 RATE AND NOT A SIDE'S TOTAL. `FStratSideView::
	 * IncomePerTurn` is the sum over everything a side holds; printing that beside one tile
	 * would be the panel reporting the economy instead of the hex.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	bool bHexCapturable = false;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	int32 HexOwner = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	int32 HexIncomeFame = 0;

	/**
	 * Whether a unit stands on `Hex`. §2.11.2's second row.
	 *
	 * FOUND BY HEX AND NOT BY A HOVERED UNIT ID, and that is the same refusal
	 * `FStratHoverView`'s block already makes: the hover states a HEX, the model states
	 * where every unit is, and a second author of "who is on that hex" is how a panel and
	 * a board come to disagree on the frame a unit moves.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	bool bHasUnit = false;

	/** The unit's instance id, side, and §2.4 row name. Selected from its `FStratUnitView`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	int32 UnitId = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	int32 UnitSide = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	FName UnitDefId;

	/**
	 * §2.11.2's `HP as 12/20`, AS TWO NUMBERS AND NEVER AS A STRING.
	 *
	 * THE SLASH IS THE WIDGET'S AND THE NUMBERS ARE THE MODEL'S. Formatting a `12/20` here
	 * would put a player-facing string in a struct a test compares field for field, and
	 * would make the panel's text unassertable against the two integers it came from.
	 *
	 * NO RATIO, NO PERCENTAGE, NO BAR FRACTION. `Hp / HpMax` is arithmetic and the header
	 * block's census forbids it; a health bar that wants a fraction computes it in the
	 * widget FROM these two, which is a rendering decision rather than a number about the
	 * match. This is the pair's first reader outside `Tests/`.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	int32 UnitHp = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	int32 UnitHpMax = 0;

	/**
	 * §2.11.2's `Atk/Def/Move/Range`, selected from the unit's `FStratUnitView`.
	 *
	 * SELECTED AND NOT LOOKED UP. The §2.4 row was read once, in `StratBuildViewModel`, at
	 * the `DefIndex` that unit carries; this struct reaches no table. So the stats shown
	 * beside a unit are the stats the model already says that unit has, and the panel
	 * cannot name a different row than the board was built from.
	 *
	 * TWO RANGE FIELDS, ONE WORD ON SCREEN. `FStratUnitView::StatRangeMin` records why the
	 * pair is not collapsed here: `2-3` and `1` are the same field rendered two ways, and
	 * the minimum is §2.6's rule rather than a display detail.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	int32 UnitStatAtk = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	int32 UnitStatDef = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	int32 UnitStatMove = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	int32 UnitStatRangeMin = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	int32 UnitStatRangeMax = 0;

	/**
	 * §2.11.2's `ready` or `done`, as the DONE bit and not as a turn flag.
	 *
	 * `FStratUnitView::bDone` AND NOT `bHasMoved && bHasActed`, and §2.11.2 rules on
	 * exactly this in as many words: it is "the machine's DONE bit (§2.11.1), read from the
	 * view-model's presentation block and not from a snapshot flag: a waited unit reads
	 * `done` while its act flag is unspent". So this is a selection of the presentation
	 * block, the two snapshot flags are deliberately NOT selected, and a panel built from
	 * them would be wrong on every waited unit while agreeing with the rules module.
	 *
	 * THIS IS WHAT PUTS AN ORDERING CONSTRAINT ON `StratDecorateInfoPanel` -- see its own
	 * declaration. Nothing else on this struct reads a field a decorator writes.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	bool bUnitDone = false;

	/**
	 * `FStratUnitView::bIsFlag`, mirrored so the panel can red-edge itself and append
	 * §2.11.2's `FLAG -- its loss ends the match.` without asking a second source which
	 * unit is the flag. Stub 7's placement field, at two removes and never inferred.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Info")
	bool bUnitIsFlag = false;
};

/**
 * §2.11.1's path preview with cost ticks: the route the selected unit would take to the
 * hovered hex, and what each hex on it costs to have arrived at. T-UI-02's second clause,
 * wave 8.
 *
 * WHAT GAP THIS CLOSES. `FStratHoverView` says where the cursor is and the movement
 * overlay says which hexes are reachable; between them there was no statement of the ROUTE,
 * and no field anywhere on this model carried one. The engine side had no source for it
 * either -- `strat::findPath` was routed through no bridge method at all, measured as zero
 * occurrences of that name in `Source/` outside `Source/StratRules/` -- so a preview built
 * before `FStratBridge::MovePathToHex` existed could only have been a walk of the reach set
 * performed in this engine. That is `Move.h`'s tie-break rule restated by a layer that may
 * not hold it, and it is the substitution T-UI-02's own words forbid.
 *
 * EVERY NUMBER HERE IS `FStratBridge::MovePathToHex`'s, COPIED. `RouteCosts[i]` is
 * `strat::reachable`'s cost for `RouteHexes[i]` -- the same number the movement overlay was
 * built from for that same hex -- and `TotalCost` is `strat::findPath`'s own `outCost`.
 * The bridge requires those two to agree and refuses when they do not; this struct does not
 * re-check, re-sum or re-derive either. THE HEADER BLOCK'S NO-ARITHMETIC CENSUS IS
 * UNMOVED BY THIS STRUCT: there is still exactly one arithmetic exception in this pair
 * (`FStratBuildOptionView::Shortfall`) and it is still outside the model.
 *
 * `TotalCost` IS NOT THE SUM OF `RouteCosts`, AND A WIDGET MUST NOT MAKE IT ONE. It is the
 * last tick -- the cost of arriving at the goal -- as `findPath` reported it, and it is
 * carried separately rather than left for a binding to compute, precisely because a
 * `for` loop in a widget adding up the ticks is T-UI-03's forbidden arithmetic wearing a
 * total's clothes. Draw `RouteCosts[i]` on `RouteHexes[i]`; draw `TotalCost` where §2.11.1
 * wants the number; add nothing.
 *
 * TWO PARALLEL ARRAYS AND NOT AN `FStratRouteStep`, on `FStratBridge::ReachableHexes`'
 * precedent, which hands its hexes and costs across the same way for the same reason. They
 * are ALWAYS THE SAME LENGTH -- the bridge fills them in one loop and empties both on every
 * refusal -- so a consumer that has checked `bHasPath` may index either by the other's
 * `Num()`.
 *
 * `RouteHexes[0]` IS THE UNIT'S OWN HEX AND `.Last()` IS THE GOAL, because `Move.h`'s
 * `outPath` includes both endpoints and neither the bridge nor this struct trims one. The
 * first tick is therefore 0, which is the null move's honest cost and not a placeholder.
 * There is deliberately no second array with the start removed: that would be a second
 * statement of one route, able to disagree with the first.
 *
 * `bHasPath` IS READ FIRST BY EVERY CONSUMER, on `FStratHoverView::bHasHoveredHex`'s rule.
 * False covers five causes and none of them is a fault: nothing is selected, the cursor is
 * over no hex, the selected unit has already moved this turn, the goal is out of the unit's
 * §2.4 allowance or blocked, and the bridge refused (the ordinary state before the match is
 * seeded). Those are not distinguished here on purpose -- the screen draws no preview in
 * all five, and a reason code would be a field nothing renders.
 *
 * IT IS A STATEMENT AND NOT AN EVENT, exactly as `FStratForecastView` is. Nothing records
 * that a route APPEARED or that the cursor moved; the preview is on screen on exactly the
 * frames this says it should be, and is rebuilt from this value like everything else.
 *
 * NOT IN THIS ROUND, with reasons:
 * - NO TEXT. Not `3 / 5 MP`, not a terrain name under a tick. Those are formatted from
 *   these fields, and putting the sentence here would make this struct the place a
 *   localisation change lands -- `FStratForecastView`'s own exclusion, held here.
 * - NO REMAINING-ALLOWANCE FIELD. `Move - TotalCost` is a subtraction, it has no snapshot
 *   field behind it, and computing it here would be the first arithmetic ever admitted to
 *   this model. If §2.11.1 turns out to require it, it is a `FStratBridge` method that
 *   asks the module, not a line in this file.
 * - NO ARROWHEAD, DASH PHASE, OR ANIMATION STATE. Drawing is the overlay's and the widget's.
 */
USTRUCT(BlueprintType)
struct FStratPathPreviewView
{
	GENERATED_BODY()

	/**
	 * Whether there is a route to draw. READ THIS FIRST -- the two arrays are empty and
	 * `TotalCost` is 0 when it is false, and 0 is also the honest cost of a route to the
	 * unit's own hex, so the number can never stand in for this flag.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Path")
	bool bHasPath = false;

	/**
	 * The route, start hex first and goal hex last, X = q and Y = r. `strat::findPath`'s
	 * `outPath` in its own order: §2.5's tie-break among equal-cost routes is the
	 * lexicographically smallest under canonical hex order and is decided in `Move.h`.
	 * Nothing in this tree reorders or trims it, which is what makes this the route
	 * `FStratBridge::SubmitMoveToHex` would actually take.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Path")
	TArray<FIntPoint> RouteHexes;

	/**
	 * The cost tick for each hex of `RouteHexes`, same index, same length.
	 * `strat::reachable`'s cost for that hex -- the CUMULATIVE cost of having arrived
	 * there, not the cost of the single step onto it, and not a difference this layer
	 * formed. `RouteCosts[0]` is 0 for the unit's own hex.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Path")
	TArray<int32> RouteCosts;

	/**
	 * `strat::findPath`'s `outCost` for the whole route, carried rather than summed. Equal
	 * to `RouteCosts.Last()` by the bridge's own cross-check, which REFUSES when the
	 * module's two answers disagree instead of picking one.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Path")
	int32 TotalCost = 0;

	/**
	 * The unit the route belongs to, or `INDEX_NONE`. Carried so a consumer never has to
	 * ask the selection machine which unit a drawn route is about -- the same reason
	 * `FStratForecastView::AttackerUnitId` is carried beside its numbers rather than looked
	 * up, and the same guarantee: this id and these hexes were produced by one call about
	 * one board.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Path")
	int32 UnitId = INDEX_NONE;
};

/**
 * GDD Sec 2.11.2 / Sec 2.11.5 -- the on-screen command bar: the BUILD control and the END
 * TURN control. Riding T-UI-03.
 *
 * WHAT GAP THIS CLOSES, AND IT IS THE PLAYER'S OWN REPORT. Every command in this prototype
 * is a keyboard binding -- `B` toggles Sec 2.11.5's menu, End Turn is `Enter`/`Space` -- and
 * nothing on screen says either exists. This block is what a persistent HUD binds those two
 * controls to.
 *
 * THE LATCH IS AN INPUT INTENT AND THIS BLOCK IS RECOMPUTED EVERY REFRESH, WHICH IS THE ONE
 * IDEA IT CARRIES. `FStratBuildAffordance` (`Source/StratPlay/StratBuildAffordance.h`) holds
 * a hex and a bool and NOT "is the button visible"; `StratDecorateCommandBar` resolves that
 * hex against `Factories` and `ViewingSide` on every decorate. So a factory captured out from
 * under the latch, a hot-seat hand-over, or a match that concluded darkens the control on the
 * next refresh with no clear-point code involved. Staleness is structurally impossible rather
 * than dependent on remembering every clear point -- the posture `FStratHoverView` already
 * takes, and the one both `UStratMatchSubsystem::IsProductionMenuOpen` and
 * `AStratScoreboardHUD::IsProductionMenuWidgetOpen` take in their own headers.
 *
 * AND THAT IS WHAT SATISFIES T-INT-05 RATHER THAN A CONVENTION.
 * `AStratPlayerController::GetProductionTargetHex` exempts itself from T-INT-05 on an
 * explicit condition -- it "appears in no view model, and nothing on screen is drawn from
 * it". A BUILD control drawn from a focus latch does not inherit that exemption, so what it
 * draws is a field HERE and not a read off an actor.
 *
 * WHAT IS DELIBERATELY ABSENT, AND WHY EACH ABSENCE IS A DECISION.
 *
 * - NO IDLE COUNT. `StratViewModelLibrary.h` argues against exactly this field at length: a
 *   count on the model would be "the first piece of ARITHMETIC INSIDE THE MODEL", and its
 *   only possible parity assertion would be against a `strat::UiSnapshot` field that does not
 *   exist, because the count is over a bit the rules module has ruled it does not own. A
 *   widget that draws the number calls `UStratViewModelLibrary::CountViewingSideUnitsAbleToAct`
 *   -- which is what `bEndTurnSuggested` below is folded over, so the number the HUD prints
 *   and the highlight it draws cannot disagree.
 * - NO `bProductionMenuOpen`. It is the mirror two headers already refuse. The BUILD control
 *   simply STAYS VISIBLE under the menu -- the menu's ZOrder is 20 and the bar's is 5 -- and
 *   `AStratScoreboardHUD::OpenProductionMenuWidget` already refuses an already-open menu and
 *   says why, so a second press is inert. Leaving the field out removes a model mirror and a
 *   widget-side conjunction at once.
 * - NO VERB AND NO ASSET REFERENCE. This is a description of what should be on screen. What
 *   the controls DO lives on `AStratPlayerController`, and what they look like lives in a
 *   WBP.
 */
USTRUCT(BlueprintType)
struct FStratCommandBarView
{
	GENERATED_BODY()

	/**
	 * Whether Sec 2.11.5's BUILD control is on screen.
	 *
	 * A WBP BINDS VISIBILITY TO THIS ONE BOOL AND MAKES NO CONJUNCTION, exactly as the
	 * directive strip binds to `FStratGuidanceView::bActive` and the info panel to
	 * `FStratInfoPanelView::bHasHex`. That is T-UI-03's requirement and it is the reason the
	 * ownership test was resolved one layer down rather than left as `Owner == ViewingSide`
	 * in a graph -- `FStratInfoPanelView`'s own "NOT IN THIS ROUND" block rules on that exact
	 * comparison.
	 *
	 * TRUE MEANS: the player has focused a hex, that hex holds a factory in `Factories`, and
	 * that factory's `Owner` equals `ViewingSide`. All three are re-asked on every decorate.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|CommandBar")
	bool bShowBuildButton = false;

	/**
	 * The focused factory's hex, X = q and Y = r. MEANINGLESS UNLESS `bShowBuildButton`.
	 *
	 * `FIntPoint(0, 0)` IS A REAL HEX ON THIS BOARD and cannot signal its own absence -- the
	 * same trap `AStratPlayerController::GetProductionTargetHex`,
	 * `UStratMatchSubsystem::IsProductionMenuOpen` and `FStratGuidanceView::ObjectiveHex` all
	 * record, closed here the same way: read the bool first.
	 *
	 * IT IS CARRIED FOR THE READER AND NOT FOR THE CLICK. The BUILD control's `OnClicked`
	 * calls `AStratPlayerController::OpenProductionMenuAtFocusedFactory`, which takes no hex
	 * -- the controller already holds the latch and passing this value back in would let a
	 * graph open a menu about a factory the latch is not on. This field exists so a clause,
	 * a log line or a WBP label can say WHICH factory without going to an actor for it.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|CommandBar")
	FIntPoint BuildFactoryHex = FIntPoint::ZeroValue;

	/**
	 * Sec 2.11.2's END TURN highlight: every unit this seat can command has finished.
	 *
	 * A HIGHLIGHT AND NEVER A GATE. It does not make End Turn legal, does not make it
	 * illegal, and promises the player nothing about whether a legal command still exists --
	 * `CountUnitsAbleToAct`'s own block draws that line in terms ("IT IS A NUDGE AND NEVER A
	 * LEGALITY CLAIM"). It answers "have you forgotten someone?" with "no".
	 *
	 * FOLDED IN C++ BECAUSE T-UI-03 FORBIDS THE GRAPH FORM. The derivation is
	 * `!Match.bHasResult && !Guidance.bEndTurnGated && CountViewingSideUnitsAbleToAct == 0`,
	 * written out at `StratDecorateCommandBar` where it is performed. A graph spelling
	 * `count == 0` would be a comparison and a graph spelling `suggested AND NOT gated` would
	 * be a conjunction; both are what that clause forbids a widget doing to a number it was
	 * handed.
	 *
	 * `bEndTurnGated` IS ANDED IN ON PURPOSE AND MUST NOT BE DROPPED AS REDUNDANT. It
	 * resolves dim-beats-highlight in one place so the widget never has to. On the SHIPPED
	 * scenario the pair is unreachable -- Sec 2.11.6-B beat 1a locks every unit but the marked
	 * Infantry, and locked units do not count, so a gated turn always has at least one unit
	 * able to act -- but that is a fact about `FStratGuidedOpening::PublishLocks` and not a
	 * structural guarantee. The `&&` is what makes it one.
	 *
	 * ZERO UNITS IS VACUOUSLY "ALL DONE", AND THE ONE INPUT ON WHICH THAT SHOWS IS NAMED. A
	 * model carrying no units at all reads as suggested. `StratBuildViewModel` cannot produce
	 * one -- it "REFUSES RATHER THAN PRODUCING AN EMPTY MODEL", its own block says so -- and
	 * a live match always carries at least the flag unit, whose death concludes the match and
	 * sets `bHasResult`. So the vacuous reading is reachable only by a hand-built model, which
	 * is exactly the thing a clause builds; recorded here rather than guarded against, because
	 * a units-count term would be this block forming an opinion about a model it was handed.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|CommandBar")
	bool bEndTurnSuggested = false;

	/**
	 * Sec 2.11.6-B's End Turn gate. A DECLARED COPY of `FStratGuidanceView::bEndTurnGated`.
	 *
	 * COPIED SO THAT ONE CONTROL HAS ONE SOURCE. The alternative was leaving the widget to
	 * read the highlight off this block and the dim off `Model.Guidance` -- two blocks, two
	 * bindings, one button -- which is the shape that lets a screen show a control both lit
	 * and dimmed on the frame the two are written apart. Precedent for a copy declared AS a
	 * copy: `FStratFactoryView::bBuildPulse`.
	 *
	 * IT RESTATES AND DOES NOT RE-DERIVE. `StratDecorateCommandBar` assigns it from
	 * `Model.Guidance.bEndTurnGated` and nothing else, which is why the decorator must run
	 * after `FStratGuidedOpening::DecorateViewModel`.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|CommandBar")
	bool bEndTurnDimmed = false;

	/**
	 * `Move the marked Infantry first.` -- Sec 2.11.6-B beat 1a. A DECLARED COPY of
	 * `FStratGuidanceView::EndTurnGateHover`, for `bEndTurnDimmed`'s reason exactly: the dim
	 * and its explanation are one control's two halves and must not come from two blocks.
	 *
	 * EMPTY WHEN NOT GATED, forwarding the guidance field's own contract unchanged.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|CommandBar")
	FText EndTurnHoverText;
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

	/**
	 * §2.11.2's info panel. Wave 2.
	 *
	 * WRITTEN BY `StratDecorateInfoPanel`, on the decoration seam, exactly as `Guidance`,
	 * `Hover` and `Forecast` are. `StratBuildViewModel` leaves it default-constructed,
	 * which is "no panel", and that default is load-bearing for the hover field's reason:
	 * every model built for a hand-over, a gate, an AI turn or a reconcile nobody hovered
	 * during says "no panel", which is the truth for all of them.
	 *
	 * IT IS THE ONLY DECORATED FIELD WHOSE INPUTS ARE ALL ON THIS SAME STRUCT. `Guidance`,
	 * `Hover` and `Forecast` each reach outside the model for something -- a scenario, a
	 * cursor trace, a bridge. This one reaches nowhere: it is a selection over `Hexes`,
	 * `Units` and `Hover`, so it is a RESTATEMENT of the model for one surface's
	 * convenience and cannot say anything the model was not already saying.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	FStratInfoPanelView InfoPanel;

	/**
	 * §2.11.1's path preview with cost ticks. Wave 8.
	 *
	 * WRITTEN BY `StratDecoratePathPreview` (`Source/StratPlay/StratPathPreviewQuery.h`),
	 * on the decoration seam, and AFTER `FStratHoverState::DecorateViewModel` -- it reads
	 * `Hover`, the same one-directional constraint `Forecast` carries and for the same
	 * reason: run before the hover, it composes this frame's selection against last frame's
	 * hex and the route is silently one mouse-move stale. `StratBuildViewModel` leaves it
	 * default-constructed, which is "no route", and that default is load-bearing for the
	 * hover field's reason: every model built for a hand-over, a gate, an AI turn or a
	 * reconcile nobody hovered during says "no route", which is the truth for all of them.
	 *
	 * IT IS THE SECOND DECORATED FIELD THAT REACHES THE BRIDGE, after `Forecast`, and it
	 * asks a DIFFERENT question of it -- `MovePathToHex` rather than `AttackForecast` -- so
	 * the two cannot disagree by construction rather than by discipline. Neither reads the
	 * other's field and neither is ordered against the other.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	FStratPathPreviewView PathPreview;

	/**
	 * Sec 2.11.2's on-screen command bar: the BUILD control and the END TURN control.
	 *
	 * WRITTEN BY `StratDecorateCommandBar`, on the decoration seam, and AFTER
	 * `FStratGuidedOpening::DecorateViewModel` -- it copies `Guidance.bEndTurnGated` and
	 * `Guidance.EndTurnGateHover` and ANDs the first into `bEndTurnSuggested`, so run before
	 * the guidance layer it would draw this frame's button against last frame's gate.
	 * `FStratBuildAffordance::DecorateViewModel` is the caller, which is why the ordering is
	 * stated on both.
	 *
	 * `StratBuildViewModel` LEAVES IT DEFAULT-CONSTRUCTED, and that default is load-bearing
	 * in the same way `PathPreview`'s is: no BUILD control, no highlight, no dim. Every model
	 * built for a hand-over, a gate, an AI turn or a reconcile nobody clicked during says
	 * exactly that, which is the truth for all of them -- so an undecorated rebuild CLEARS the
	 * bar rather than leaving the last click's affordance standing.
	 *
	 * IT REACHES NO BRIDGE, unlike `Forecast` and `PathPreview`. Its decorator takes the model
	 * and nothing else, on `StratDecorateInfoPanel`'s stated rule: a decorator holding a
	 * bridge could write a control the model does not describe, and then the command bar
	 * would be a second source of truth about a board T-INT-05 says the model alone
	 * describes.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|View")
	FStratCommandBarView CommandBar;
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
 * The same menu, and §2.11.5's OWN HEADER NUMBER beside it: `FACTORY -- BUILD   Fame: 250`.
 *
 * WHAT GAP THIS CLOSES. The five-argument form above already READ the purse -- it has to, in
 * order to compute `FStratBuildOptionView::Shortfall` -- and then threw it away, so §2.11.5's
 * header had no source at all. A caller that wanted it had to go back to
 * `FStratViewModel::Sides[...].FameTotal`, which is the SAME number read at a DIFFERENT
 * INSTANT.
 *
 * AND THAT SECOND INSTANT IS THE WHOLE REASON THIS OVERLOAD EXISTS, rather than convenience.
 * `Shortfall` is `CostFame - FameTotal` against the purse as it stood when the rows were
 * built. A header drawn from a separately-refreshed model could print `Fame: 250` above a row
 * reading `need 50` on a 275-cost unit, and the two would be inconsistent on screen while each
 * was individually correct. One read, one instant, one number -- the same rule
 * `UStratMatchSubsystem::IsOpenMenuFactorySpawnBlocked`'s block states for §2.11.5's footer
 * fact, arrived at here from the same direction.
 *
 * IT IS THE PURSE OF `Side`, NOT OF THE VIEWING SIDE AND NOT OF THE SIDE TO MOVE. This
 * function does not know which of the three `Side` is; the caller stated it, and the header
 * this fills belongs to the menu that caller opened.
 *
 * THE FIVE-ARGUMENT FORM IS THE ONE-LINE FORWARD AND NOT THE OTHER WAY AROUND, so that there
 * is exactly one implementation and no second place a purse can be read from. Its existing
 * callers are unchanged and its contract is unchanged.
 *
 * ALL-OR-NOTHING COVERS THE PURSE TOO. `OutPurseFame` is written on the same last lines as
 * `OutOptions` and is untouched by a refusal, so a caller keeps the header it was drawing
 * beside the rows it was drawing.
 */
STRATUI_API bool StratBuildProductionMenu(
	const FStratBridge&            Bridge,
	int32                          Side,
	FIntPoint                      FactoryHex,
	TArray<FStratBuildOptionView>& OutOptions,
	int32&                         OutPurseFame,
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

/**
 * Fills §2.11.2's info panel from the model it is a panel ON. Wave 2.
 *
 * IT TAKES THE MODEL AND NOTHING ELSE -- no bridge, no snapshot, no table, no query
 * interface. That is the design, not an economy: a decorator holding a bridge could write
 * a number the model does not contain, and then §2.11.2's panel would be a second source
 * of truth about a board T-INT-05 says the model alone describes. With this signature the
 * property is structural rather than reviewed. `FStratInfoPanelView`'s own block states
 * the same thing from the struct's side.
 *
 * A DECORATOR AND NOT A COMPOSE, which is why it is spelled like `StratDecorateForecast`
 * and not like `StratComposeForecastView`. It writes one field of a model it also reads,
 * so a compose taking `const FStratViewModel&` and an out-parameter would invite the
 * caller to alias the model against its own field. This form has no aliasing hazard to
 * reason about at all.
 *
 * IT MUST RUN AFTER `FStratHoverState::DecorateViewModel` AND AFTER
 * `FStratSelectionMachine::DecorateViewModel`, and BOTH constraints are real -- unlike
 * `StratDecorateForecast`, whose selection arm is incidental. It reads `Model.Hover` for
 * which hex, and `FStratUnitView::bDone` for §2.11.2's `ready` / `done`, and that bit is
 * written by the machine. Run early, the panel is one mouse-move stale and reports last
 * frame's DONE -- a failure that reads as latency and is sequencing.
 *
 * NO FAILURE CHANNEL, on purpose. There is no input it can be handed that has no answer:
 * not hovering is `bHasHex` false, hovering a hex the model does not carry is `bHasHex`
 * false, and an empty hex is `bHasUnit` false. A `bool` return here would be a value no
 * caller could act on, and the one thing this function cannot do is fail to describe a
 * model it was given.
 *
 * UNCONDITIONAL, on `StratDecorateForecast`'s rule and for its reason: it writes
 * `Model.InfoPanel` on every call, including the calls where the answer is "no panel". A
 * decorator that returned early on the empty case would leave the previous hover's panel
 * on screen after the cursor left the board.
 *
 * ALL-OR-NOTHING, as every builder in this file is: it fills a local and assigns on the
 * last line.
 */
STRATUI_API void StratDecorateInfoPanel(FStratViewModel& Model);

/**
 * Fills Sec 2.11.2's command bar from the model it is a bar ON, plus one input intent.
 *
 * IT TAKES THE MODEL AND A LATCH, AND NOTHING ELSE -- no bridge, no snapshot, no table, no
 * controller, no query interface. That is `StratDecorateInfoPanel`'s signature discipline
 * with one addition, and the addition is the whole reason this function has parameters at
 * all: WHICH FACTORY the player focused is an input intent that no field of the model
 * carries, and cannot be, because nothing in this project selects a hex
 * (`FStratSelectionMachine` tracks only `SelectedUnitId`, and its own header says so).
 *
 * SO THE HEX ARRIVES AS AN ARGUMENT AND THE ANSWER IS RESOLVED HERE, WHICH IS THE DESIGN.
 * The caller -- `FStratBuildAffordance::DecorateViewModel` -- hands over a hex and a bool and
 * decides nothing. This function looks the hex up in `Model.Factories` and compares the
 * owner against `Model.ViewingSide` on EVERY call, so a latch that has gone stale draws
 * nothing without anything having to notice. A `bShowBuildButton` computed by the caller and
 * merely copied here would be exactly the staleness this arrangement is built to make
 * unrepresentable.
 *
 * TWO ARGUMENTS AND NOT AN `FStratBuildAffordance`, deliberately. This module does not depend
 * on `StratPlay` and never may -- the arrow runs the other way -- so the parameter list is
 * two plain values. It also means a clause drives every arm of this function with no
 * affordance, no controller and no world in existence.
 *
 * A DECORATOR AND NOT A COMPOSE, spelled like `StratDecorateInfoPanel` and not like
 * `StratComposeInfoPanelModel`, for that function's stated reason: it writes one field of a
 * model it also reads, so a compose taking `const FStratViewModel&` and an out-parameter
 * would invite the caller to alias the model against its own field.
 *
 * IT MUST RUN AFTER `FStratGuidedOpening::DecorateViewModel`, and that constraint is REAL and
 * runs one way. It reads `Model.Guidance.bEndTurnGated` and `Model.Guidance.EndTurnGateHover`
 * and writes neither. Run early, the END TURN control is drawn against last frame's gate --
 * a failure that reads as latency and is sequencing, which is the same shape
 * `StratDecorateInfoPanel` records about the hover.
 *
 * IT IS UNORDERED AGAINST THE HOVER, THE FORECAST, THE PATH PREVIEW AND THE INFO PANEL. It
 * shares no field with any of them: it reads `Factories`, `ViewingSide`, `Match`, `Units` and
 * `Guidance`, and writes `CommandBar`, which nothing else reads.
 *
 * NO FAILURE CHANNEL, on `StratDecorateInfoPanel`'s rule. There is no input it can be handed
 * that has no answer: no focus is `bShowBuildButton` false, a focus on a hex the model does
 * not carry is `bShowBuildButton` false, and a focus on an enemy factory is the same. A
 * `bool` return would be a value no caller could act on.
 *
 * UNCONDITIONAL. It writes ALL FIVE FIELDS on every call, including the calls where the
 * answer is "no bar affordances at all". A decorator that returned early on the empty case
 * would leave the previous click's BUILD control on screen after the player clicked away.
 *
 * ALL-OR-NOTHING, as every builder in this file is: it fills a local and assigns on the last
 * line.
 *
 * @param Model                the model to decorate. Read and written.
 * @param bHasFocusedFactory   whether the player has focused a hex at all. Read FIRST --
 *                             `FIntPoint(0, 0)` is a real hex and cannot signal its absence.
 * @param FocusedFactoryHex    that hex, X = q and Y = r. Meaningless unless the bool. NOT
 *                             checked against the board -- if it names no factory the answer
 *                             is simply "no BUILD control", which is correct.
 */
STRATUI_API void StratDecorateCommandBar(
	FStratViewModel& Model,
	bool             bHasFocusedFactory,
	FIntPoint        FocusedFactoryHex);
