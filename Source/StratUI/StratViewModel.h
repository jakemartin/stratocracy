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
// NO ARITHMETIC, ANYWHERE, and one lookup that is not arithmetic. Every number below is
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
// of: the producer landed a phase later than "phase 3", and `bLockedThisTurn` has a writer
// (`SetLockedThisTurn`) but no shipping caller, since §2.11.6's guidance layer does not
// exist, so that bit is false in every running path.
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
	 * Woods. The name is what seeding itself resolves by (`Replay.good.cpp:299-308`), so
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
	 * RENAMED ON PURPOSE. The vendored field is spelled `unitId`, and `SaveCommand::unitId` spells
	 * the same quantity `unitId` in a Build command, where `applyCommand` uses it as a
	 * raw bounds-checked index into the definitions vector with no name lookup
	 * (`Replay.good.cpp:486-487`). That spelling is the trap -- it reads like an instance
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
