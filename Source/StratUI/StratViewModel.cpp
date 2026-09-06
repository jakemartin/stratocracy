// GDD §4.7 Stub 8 / §4.9 -- the reflected view model, implementation side.
//
// WHAT IS HERE THAT COULD NOT BE IN THE HEADER, which is this file's whole reason for
// being a separate translation unit:
//
//   1. `#include "StratBridge.h"`. StratViewModel.h declares USTRUCTs and a UENUM, so
//      UHT parses it, and StratBridge.h pulls in the vendored `strat` headers -- exactly
//      what StratBridge.h's own note ("declares no reflected types on purpose") exists
//      to keep out of the header tool's way. A .cpp is not parsed by UHT, so this is the
//      one place in this pair where the include is legal. It must not migrate upward.
//
//   2. The `strat::` type names. `strat::UiSnapshot`, `strat::RulesTables` and
//      `strat::ResultTier` are NAMED below and no `strat::` free function is CALLED.
//      That distinction is the linker's, not a stylistic one: naming a vendored type is
//      free, and calling a vendored function from this module is `LNK2019` because the
//      vendored sources carry no `_API` macro. Every entry point below is a method on
//      `FStratBridge`, which is the only thing UnrealEditor-StratBridge.dll exports.
//
//   3. The static_asserts -- THREE. THE CHECK THAT CONFIRMS IT IS ANCHORED TO START OF
//      LINE PLUS ANY RUN OF WHITESPACE, and both halves of that anchor are load-bearing:
//
//          grep -c '^[[:space:]]*static_assert' Source/StratUI/StratViewModel.cpp   -> 3
//
//      THE ANCHOR EXCLUDES PROSE: a comment line begins with `/`, which is not whitespace,
//      so no sentence in this block can match however often it names the token. That is
//      what keeps this census out of the set it is counting.
//      THE `*` MAKES IT DEPTH-BLIND, and it is not decoration. An earlier form of this
//      check used `^[[:space:]]` with no `*`, matching EXACTLY ONE whitespace character.
//      It returned 3 only because all three statements happen to sit at namespace scope at
//      one tab; one added inside a function body, a nested namespace or a class would sit
//      deeper and would NOT have matched, and the census would have UNDER-reported in
//      silence. That is the same failure class as (a) below, just rarer -- so the check is
//      now indifferent to indentation as well as to prose.
//
//      AN UNANCHORED SEARCH IS NOT A SUBSTITUTE AND ITS ANSWER IS NOT EVEN STABLE, because
//      the prose below NAMES the token several times. Measured on this file, not predicted:
//      an unanchored search for the bare token returns EIGHT today, and it returns whatever
//      the surrounding prose happens to spell tomorrow. The anchored form returns THREE and
//      moves only when a statement is added or removed, which is the property a census
//      needs.
//
//      THIS BLOCK HAS NOW BEEN WRONG ABOUT ITS OWN NUMBER TWICE, BY TWO DIFFERENT
//      MECHANISMS, and both are recorded because a reader who has seen one will not
//      recognise the other:
//        (a) It said FOUR, having counted PINNED SUBJECTS rather than statements -- a
//            count invalidated by the change that wrote it. Corrected 2026-08-25.
//        (b) The correction said THREE and prescribed an unanchored search for the token
//            it had just quoted, so the census sentence became a member of its own
//            subject and the prescribed check returned FOUR against a stated THREE.
//            A CENSUS THAT QUOTES ITS OWN SEARCH TOKEN JOINS THE SET IT IS COUNTING.
//            Corrected the same day. The neighbouring failure in this repo is the one
//            where a `BP_` prefix nested inside `WBP_` inflated a census by 19.
//
//      Two mechanisms pin the five subjects below, and they are separated because only
//      one of them is an assert statement:
//        - PINNED BY `static_assert`, three of them: `strat::OWNER_NEUTRAL == INDEX_NONE`,
//          `sizeof(UiSnapshot::side)/sizeof(UiSideView) == strat::SIDE_COUNT`, and
//          `strat::SIDE_NONE == INDEX_NONE` (added 2026-08-25 with
//          `FStratMatchResultView::Winner`).
//        - PINNED BY AN EXHAUSTIVE SWITCH WITH NO `default:` LABEL, two of them: the
//          `strat::ResultTier` enumerators in `ResultTierOf` and the `strat::ResultCause`
//          enumerators in `ResultCauseOf`. An enum's MEMBERSHIP cannot be asserted at
//          compile time the way a constant's VALUE can, which is why the mechanism differs
//          -- see "WHY THE MAPPINGS ARE SWITCHES AND ASSERTS RATHER THAN CASTS" below.
//      All five subjects are compile-time facts about vendored declarations, reachable only
//      here. Reading one is not the cross-module call the constraint above forbids --
//      nothing is emitted and no symbol has to be exported for it to link.
//
// THE BINDING RULE, restated here because this is the file where it can be broken:
// every number written into `FStratViewModel` is EQUAL TO ONE FIELD of
// `strat::UiSnapshot`, copied with no transformation beyond a width cast. There is no
// `+`, no `-`, no `/` and no `.size()` over a snapshot vector in `StratBuildViewModel`.
//
// THAT SENTENCE USED TO END "below" AND IT IS CORRECTED RATHER THAN DROPPED. As of
// 2026-08-25 there is ONE subtraction in this file and it is not in the view model's
// builder: `StratBuildProductionMenu` computes `FStratBuildOptionView::Shortfall` as
// `CostFame - FameTotal`, because §2.11.5 requires an unaffordable row to print
// `need N` and T-UI-03 forbids the widget doing the subtraction. It is DECLARED derived
// on the field, it is clamped at zero, and it is never consulted to decide whether a row
// is affordable -- `UiBuildOption::affordable` remains the only authority on that, so the
// screen's greying still turns on the module's comparison and not on this file's. The one thing that
// is not a copy is the pair of `FName` lookups, which read a definition row's `id` at an
// index the snapshot supplies; that is a table read and it is justified on the fields
// themselves in the header.
//
// AND THE COUNT DID NOT MOVE ON 2026-08-27, which is worth saying because that day added a
// function. `StratDecorateInfoPanel` fills §2.11.2's panel and contains no `+`, `-`, `*`
// or `/` at all: every field it writes is assigned from a field of the same
// `FStratViewModel` it was handed. It has no bridge, no snapshot and no table, so there is
// nothing for it to derive a number FROM. The one comparison in it -- `TerrainMoveCost ==
// 0` -- reads `Data.h`'s §4.8 impassability sentinel and is declared on the field it
// writes; a comparison against a meaning the rules module states in its own header is the
// same class of thing as the exhaustive switches below, not the same class as `Shortfall`.
// So this file still has exactly ONE subtraction and it is still
// `StratBuildProductionMenu`'s.
//
// AND THE COUNT DID NOT MOVE ON 2026-09-02 EITHER, which is worth saying for the same
// reason: that day added `StratDecorateCommandBar`. That function contains no `+`, `-`, `*`
// or `/`. It performs ONE lookup (`Factories.FindByPredicate` by hex, the same shape
// `StratDecorateInfoPanel` uses to find the hovered hex's unit), TWO copies out of
// `Model.Guidance`, and TWO comparisons -- `Owner == ViewingSide` and `<count> == 0` -- both
// of which produce a BOOLEAN that is stored, never a number. The count itself is
// `UStratViewModelLibrary::CountViewingSideUnitsAbleToAct`'s, a `BlueprintPure` selector over
// this same model whose own header argues at length why computing it OVER the model is not
// arithmetic INSIDE it. So this file still has exactly ONE subtraction and it is still
// `StratBuildProductionMenu`'s.
//
// WHY THE MAPPINGS ARE SWITCHES AND ASSERTS RATHER THAN CASTS. `EStratResultTier` and
// `strat::ResultTier` agree today enumerator for enumerator, and a `static_cast` between
// them would keep agreeing right up until someone adds a tier upstream, at which point
// the screen would name the wrong outcome of a finished match with nothing failing. The
// exhaustive switch turns that into a compile error in this file, which is a file
// anybody is allowed to fix -- unlike the vendored enum.
//
// NOTHING BELOW IS A GAMEPLAY VALUE, AN ASSET PATH OR A CONSTANT OF THIS MODULE'S OWN.
// The board size, the turn cap, the objective total, the side count and every unit and
// terrain name arrive through the bridge from the scenario and the §4.8 tables.

#include "StratViewModel.h"

// IWYU: `StratDecorateCommandBar` names `UStratViewModelLibrary::CountViewingSideUnitsAbleToAct`
// directly. This is a header of THIS module and includes nothing vendored, so it carries none
// of the constraint the next include does; the two are adjacent only by position.
#include "StratViewModelLibrary.h"

// See item 1 of the block above. This line is legal here and nowhere else in this pair.
#include "StratBridge.h"

namespace
{
	/**
	 * `strat::Hex` -> `FIntPoint`, X = q and Y = r.
	 *
	 * THE ONLY PLACE THAT MAPPING IS WRITTEN DOWN. A second spelling of it elsewhere
	 * would be a second chance to transpose the axes, and a transposed axial coordinate
	 * is a board that looks plausible and is reflected about a diagonal -- the kind of
	 * defect that survives a glance at a screenshot.
	 */
	FIntPoint HexPoint(const strat::Hex& H)
	{
		return FIntPoint(static_cast<int32>(H.q), static_cast<int32>(H.r));
	}

	/**
	 * `std::string` -> `FName`, for the two definition-row `id` fields.
	 *
	 * The vendored ids are ASCII by construction (§4.8's tables ship "Plains", "Tank",
	 * ...), and the conversion goes through UTF8 anyway rather than assuming it: a
	 * narrowing assumption about someone else's bytes costs nothing to avoid here and is
	 * expensive to find later.
	 */
	FName DefinitionName(const std::string& Id)
	{
		return FName(UTF8_TO_TCHAR(Id.c_str()));
	}

	/**
	 * §2.8's tier, mapped exhaustively. See the header block on why this is not a cast.
	 *
	 * There is no `default:` label, deliberately: a `default` would make a new upstream
	 * tier compile silently into whatever this function returned for it, which is the
	 * exact failure the switch exists to prevent. The trailing return is unreachable and
	 * exists only for compilers that cannot see that.
	 */
	EStratResultTier ResultTierOf(strat::ResultTier Tier)
	{
		switch (Tier)
		{
		case strat::ResultTier::InProgress: return EStratResultTier::InProgress;
		case strat::ResultTier::Draw:       return EStratResultTier::Draw;
		case strat::ResultTier::Marginal:   return EStratResultTier::Marginal;
		case strat::ResultTier::Decisive:   return EStratResultTier::Decisive;
		}

		return EStratResultTier::InProgress;
	}

	/**
	 * §2.8's cause, mapped exhaustively, on `ResultTierOf`'s reasoning exactly.
	 *
	 * No `default:` label here either, and it matters more on this enum than on the tier:
	 * §2.8 has three win conditions and two draw conditions and upstream has already
	 * added one (`Domination`) since the tier enum was written. A `default` would compile
	 * the next one silently into whatever this returned for it.
	 */
	EStratResultCause ResultCauseOf(strat::ResultCause Cause)
	{
		switch (Cause)
		{
		case strat::ResultCause::None:           return EStratResultCause::None;
		case strat::ResultCause::FlagDestroyed:  return EStratResultCause::FlagDestroyed;
		case strat::ResultCause::Domination:     return EStratResultCause::Domination;
		case strat::ResultCause::AttritionLead:  return EStratResultCause::AttritionLead;
		case strat::ResultCause::PassivityGuard: return EStratResultCause::PassivityGuard;
		case strat::ResultCause::AllKeysTied:    return EStratResultCause::AllKeysTied;
		}

		return EStratResultCause::None;
	}

	// The mirror on `FStratHexView::Owner` / `FStratFactoryView::Owner` claims that
	// `strat::OWNER_NEUTRAL` and `INDEX_NONE` are the same number, so the copy is exact
	// rather than a re-encoding. The two constants are declared in different
	// repositories -- one vendored, one engine -- so the claim is checked instead of
	// trusted. If this ever fires, the fix is a mapping function beside `ResultTierOf`
	// and not a widened comment.
	static_assert(strat::OWNER_NEUTRAL == INDEX_NONE,
		"strat::OWNER_NEUTRAL must equal INDEX_NONE for the owner fields to be exact mirrors");

	// The view model carries one `FStratSideView` per side and indexes it by `strat` side
	// index. `UiSnapshot::side` is a fixed C array of `SIDE_COUNT`, and the loop below
	// walks its extent; this pins the two together so that a side count changing upstream
	// is a compile failure here rather than a silently short `Sides` array.
	static_assert(sizeof(strat::UiSnapshot::side) / sizeof(strat::UiSideView) == strat::SIDE_COUNT,
		"UiSnapshot::side must have SIDE_COUNT entries for Sides to be indexable by side");

	// `FStratMatchResultView::Winner` claims that `strat::SIDE_NONE` and `INDEX_NONE` are
	// the same number, so "nobody won" survives the copy without being re-encoded. Checked
	// rather than trusted, exactly as `OWNER_NEUTRAL` above is and for the same reason --
	// the two constants are declared in different repositories. If this fires, the fix is a
	// mapping beside `ResultCauseOf` and NOT a widened comment: a `Winner` that silently
	// meant -1 upstream and 0 here would name a real side as the victor.
	static_assert(strat::SIDE_NONE == INDEX_NONE,
		"strat::SIDE_NONE must equal INDEX_NONE for FStratMatchResultView::Winner to be an exact mirror");
}

bool StratBuildViewModel(
	const FStratBridge& Bridge,
	int32               ViewingSide,
	FStratViewModel&    OutModel,
	FString&            OutFailureReason)
{
	// Cleared up front so a success cannot leave a previous call's refusal sitting in the
	// caller's string.
	OutFailureReason.Reset();

	// The ONE read of the rules-side view model, and it is the bridge's answer whole. Its
	// header records why it refuses rather than handing back an empty snapshot when there
	// is nothing to project, and that reason bites harder here than anywhere: phase 3
	// reconciles actors against this value, so an empty model produced by an unseeded
	// bridge would not draw an empty board -- it would DESTROY the board that is on
	// screen.
	strat::UiSnapshot Snapshot;
	const FStratResult SnapshotResult = Bridge.MakeUiSnapshot(Snapshot);
	if (!SnapshotResult.bOk)
	{
		// The bridge's own sentence, forwarded unchanged. The failing-invariant `Id` is
		// deliberately not concatenated in: there is one string channel here, and
		// "T-SAVE-05: <reason>" is no longer the owning module's own words.
		OutFailureReason = SnapshotResult.Reason;
		return false;
	}

	// The definition tables, borrowed for the two `FName` lookups. `Tables()` is a method
	// on the exported bridge class; the vectors behind it are the ones the bridge mapped
	// from DT_Units and DT_Terrain, so a name read here is the same byte the rules module
	// resolves scenarios by.
	//
	// CHECKED RATHER THAN ASSUMED even though `MakeUiSnapshot` just succeeded on a seeded
	// bridge. The alternative is a null dereference reachable only through a future
	// refactor of the bridge's load order, which is precisely the kind of failure nobody
	// finds by reading.
	const strat::RulesTables Tables = Bridge.Tables();
	if (Tables.units == nullptr || Tables.terrain == nullptr)
	{
		OutFailureReason = TEXT("the bridge projected a snapshot but holds no definition tables to name its rows from");
		return false;
	}

	const int32 SideCount = static_cast<int32>(UE_ARRAY_COUNT(Snapshot.side));

	// NOT `sideToMove`. The snapshot carries whose turn it is; this is whose screen it is,
	// and on every other turn of a hot-seat match those differ. The caller supplies it and
	// this function only checks that it names a side that exists.
	//
	// A REFUSAL AND NOT A CLAMP. A clamped viewing side renders a coherent screen for the
	// wrong player, which is the worst available outcome in a hot-seat game.
	if (ViewingSide < 0 || ViewingSide >= SideCount)
	{
		OutFailureReason = FString::Printf(
			TEXT("Viewing side %d is outside the snapshot's %d sides."),
			ViewingSide, SideCount);
		return false;
	}

	// Built aside and moved across on the last line. See the header's all-or-nothing note.
	FStratViewModel Built;

	Built.ViewingSide    = ViewingSide;
	Built.ObjectiveTotal = static_cast<int32>(Snapshot.objectiveTotal);

	Built.Match.Turn       = static_cast<int32>(Snapshot.match.turn);
	Built.Match.TurnCap    = static_cast<int32>(Snapshot.match.turnCap);
	Built.Match.SideToMove = static_cast<int32>(Snapshot.match.sideToMove);
	Built.Match.bHasResult = Snapshot.match.hasResult;
	// READ, NEVER INFERRED FROM `bHasResult`, and vice versa. Ui.h rules that InProgress
	// IS the null, so the two fields agree today by construction -- deriving either from
	// the other would make this file the author of that ruling instead of its mirror.
	Built.Match.ResultTier = ResultTierOf(Snapshot.match.resultTier);

	// ---- Sides -------------------------------------------------------------
	// Every side, in side-index order, so `Sides[i]` is side `i`. The two-column YOU/ENEMY
	// reduction is §2.11.4's panel and lives in `StratBuildScoreboardModel`; the view
	// model describes the board, and the board has however many sides it has.
	Built.Sides.Reserve(SideCount);
	for (int32 SideIndex = 0; SideIndex < SideCount; ++SideIndex)
	{
		const strat::UiSideView& Source = Snapshot.side[SideIndex];

		FStratSideView Side;
		Side.FameTotal      = static_cast<int32>(Source.fameTotal);
		Side.FameCombat     = static_cast<int32>(Source.fameCombat);
		Side.ObjectivesHeld = static_cast<int32>(Source.objectivesHeld);
		Side.SurvivingHp    = static_cast<int32>(Source.survivingHp);
		Side.IncomePerTurn  = static_cast<int32>(Source.incomePerTurn);

		Built.Sides.Add(Side);
	}

	// ---- Hexes -------------------------------------------------------------
	// Appended in the order the snapshot emits them, which is canonical hex order, and
	// nothing sorts them afterwards. The determinism is the rules module's and is
	// preserved by not touching it.
	Built.Hexes.Reserve(static_cast<int32>(Snapshot.hexes.size()));
	for (const strat::UiHexView& Source : Snapshot.hexes)
	{
		const int32 TerrainIndex = static_cast<int32>(Source.terrainId);

		// A snapshot hex naming a terrain row the table does not have is a fault in the
		// load, not a hex to draw as something else. It refuses with the offending index
		// rather than falling back to NAME_None, because a nameless hex would reconcile
		// to whatever phase 3's map returns for an unknown key -- silently, and 99 times.
		if (TerrainIndex < 0 || TerrainIndex >= static_cast<int32>(Tables.terrain->size()))
		{
			OutFailureReason = FString::Printf(
				TEXT("Hex (%d, %d) names terrain row %d, which is outside the loaded table's %d rows."),
				static_cast<int32>(Source.hex.q), static_cast<int32>(Source.hex.r),
				TerrainIndex, static_cast<int32>(Tables.terrain->size()));
			return false;
		}

		// ONE ROW, BOUND ONCE, THREE FIELDS OFF IT. The index was range-checked just above
		// and the reference is taken here rather than subscripting three times, so the
		// three terrain facts on this hex cannot come from three different rows -- which
		// is the property `FStratHexView::TerrainMoveCost`'s block is about.
		const strat::TerrainDef& TerrainRow = (*Tables.terrain)[TerrainIndex];

		FStratHexView HexView;
		HexView.Hex               = HexPoint(Source.hex);
		HexView.TerrainIndex      = TerrainIndex;
		HexView.TerrainId         = DefinitionName(TerrainRow.id);
		// PASSED THROUGH, INCLUDING THE §4.8 SENTINEL AND INCLUDING THE SIGN. `moveCost`
		// 0 means impassable and is not clamped, defaulted or re-encoded here, and
		// `defensePct` is not made positive. Both are the table's own values widened to
		// `int32`; `Data.h` is the authority on what each means.
		HexView.TerrainMoveCost   = static_cast<int32>(TerrainRow.moveCost);
		HexView.TerrainDefensePct = static_cast<int32>(TerrainRow.defensePct);
		// §2.11.2's "status if capturable". `capturable` is READ rather than inferred from
		// `owner`, because a neutral Factory and a Plains hex carry the same owner -- the
		// trap is recorded on `FStratHexView::bTerrainCapturable`.
		HexView.bTerrainCapturable = TerrainRow.capturable;
		HexView.TerrainIncomeFame  = static_cast<int32>(TerrainRow.incomeFame);
		HexView.Owner             = static_cast<int32>(Source.owner);

		Built.Hexes.Add(HexView);
	}

	// ---- Units -------------------------------------------------------------
	// Ascending unit id, the snapshot's order, preserved.
	Built.Units.Reserve(static_cast<int32>(Snapshot.units.size()));
	for (const strat::UiUnitView& Source : Snapshot.units)
	{
		// `UiUnitView::unitId` is the §2.4 ROW INDEX and not an instance id, whatever its
		// spelling suggests -- see the note on `FStratUnitView::DefIndex`.
		const int32 DefIndex = static_cast<int32>(Source.unitId);

		if (DefIndex < 0 || DefIndex >= static_cast<int32>(Tables.units->size()))
		{
			OutFailureReason = FString::Printf(
				TEXT("Unit %d names definition row %d, which is outside the loaded table's %d rows."),
				static_cast<int32>(Source.id), DefIndex, static_cast<int32>(Tables.units->size()));
			return false;
		}

		// ONE ROW, BOUND ONCE, SIX FIELDS OFF IT -- the id and the five §2.4 stats. Bound
		// for `FStratHexView::TerrainMoveCost`'s reason on the terrain side: fields taken
		// off one read cannot disagree about which row was read.
		const strat::UnitDef& UnitRow = (*Tables.units)[DefIndex];

		FStratUnitView UnitView;
		UnitView.UnitId          = static_cast<int32>(Source.id);
		UnitView.Side            = static_cast<int32>(Source.side);
		UnitView.DefIndex        = DefIndex;
		UnitView.DefId           = DefinitionName(UnitRow.id);
		// §2.11.2's `Atk/Def/Move/Range`, the table's own values widened to `int32`.
		// `UnitRow.hpMax` is DELIBERATELY NOT READ -- `HpMax` below is the snapshot's
		// per-instance number and a second author of it is what the field's block refuses.
		UnitView.StatAtk         = static_cast<int32>(UnitRow.atk);
		UnitView.StatDef         = static_cast<int32>(UnitRow.def);
		UnitView.StatMove        = static_cast<int32>(UnitRow.move);
		UnitView.StatRangeMin    = static_cast<int32>(UnitRow.rangeMin);
		UnitView.StatRangeMax    = static_cast<int32>(UnitRow.rangeMax);
		UnitView.Hex             = HexPoint(Source.hex);
		UnitView.Hp              = static_cast<int32>(Source.hp);
		UnitView.HpMax           = static_cast<int32>(Source.hpMax);
		UnitView.bIsFlag         = Source.isFlag;
		UnitView.bHasMoved       = Source.hasMoved;
		UnitView.bHasActed       = Source.hasActed;
		UnitView.CaptureProgress = static_cast<int32>(Source.captureProgress);
		UnitView.bIsGuidedMarked = Source.isGuidedMarked;

		// `bDone` and `bLockedThisTurn` are DELIBERATELY LEFT AT FALSE. They are the
		// presentation block's (`strat::UiPresentationUnit`), their owners are §2.11.1's selection
		// machine and the guidance layer.
		//
		//     RETRACTED>  "and neither exists in this engine yet."
		//
		// BOTH OWNERS EXIST AND BOTH WRITE THESE TWO FIELDS TODAY, and the retraction is the
		// only change here -- every other sentence in this block is unchanged and still
		// load-bearing, including the decision this function makes. AMENDED 2026-09-06.
		//
		// THE SENTENCE WAS TRUE WHEN WRITTEN AND AGED INTO FALSITY; it was not a
		// misstatement. Ordered by ancestry rather than by date, because the first two
		// commits share a day and dates cannot order those: this block landed at `f918e30`,
		// which `git merge-base --is-ancestor` places before `ed27d5a` (`FStratSelectionMachine`,
		// same day, about three and a half hours later) and before `1d6f758`
		// (`FStratGuidedOpening`). A linear `git rev-list --count` walk agrees: 29, 31, 77. So the
		// claim was accurate for two commits and has been false since.
		//
		// WHERE THEY ARE WRITTEN, AND IT IS A LIVE ROUTE RATHER THAN AN UNCALLED VERB:
		// `FStratSelectionMachine::DecorateViewModel` assigns both -- `U.bDone` from
		// `DoneUnits` and `U.bLockedThisTurn` from `LockedUnits` -- and
		// `AStratPlayerController::DecorateForPresentation` calls it, then
		// `FStratGuidedOpening::DecorateViewModel`, on the decoration seam that every
		// refresh runs through.
		//
		// NONE OF WHICH MOVES THIS FUNCTION'S BEHAVIOUR, which is the point. The fields are
		// filled by an overlay on that seam AFTERWARDS, so leaving them false here is still
		// correct and is not a debt waiting on an owner. Setting
		// them from `hasMoved` / `hasActed` here would be inventing the DONE bit -- which
		// Ui.h states is derivable from neither flag nor from any pair of them -- and it
		// would be wrong in exactly the two cases (Wait, RMB-in-MOVED) that the bit was
		// separated out for.
		//
		//     RETRACTED>  "The debt and its discharge condition are in the header."
		//
		// THE HEADER STATES AN OWNER AND A DESIGN, NOT A DEBT, and states no discharge
		// condition at all -- read it and check. `FStratUnitView::bDone` and
		// `::bLockedThisTurn` each name their owner ("OWNER: the selection machine",
		// "OWNER: the guidance layer") and the group comment above them says this builder
		// leaves both false because `strat::buildUiSnapshot` does not produce them. That is
		// the arrangement, permanently, so a pointer promising a debt sends a reader looking
		// for something that is not there. It may once have been a debt; it is not one now.

		Built.Units.Add(UnitView);
	}

	// ---- Factories ---------------------------------------------------------
	// §2.11.5's BUILD pulse, ONE ASK FOR THE WHOLE BOARD, before the loop. The bridge
	// composes it -- see `FStratFactoryBuildPulse::bShouldPulse` for the condition and for
	// why availability stands where the GDD sentence names the build allowance. Nothing
	// below recomputes any part of it; the loop copies one bool.
	//
	// A REFUSAL FAILS THE WHOLE MODEL rather than yielding an unpulsed board, on
	// `MakeUiSnapshot`'s reasoning above: phase 3 RECONCILES against this value, so a model
	// that quietly dropped the pulse would not draw a board without pulses -- it would turn
	// off every pulse on screen and report success. The reachable refusals are all
	// whole-board ones (no unit table, or a viewing side outside the match, which the check
	// above has already refused in its own words), so there is no case where a partial
	// answer would have been the honest one.
	TArray<FStratFactoryBuildPulse> Pulses;
	{
		const FStratResult PulseResult = Bridge.FactoryBuildPulses(ViewingSide, Pulses);
		if (!PulseResult.bOk)
		{
			// The bridge's own sentence, forwarded unchanged, exactly as the snapshot
			// refusal above forwards its own.
			OutFailureReason = PulseResult.Reason;
			return false;
		}
	}

	// Canonical hex order, the snapshot's, preserved.
	Built.Factories.Reserve(static_cast<int32>(Snapshot.factories.size()));
	for (const strat::UiFactoryView& Source : Snapshot.factories)
	{
		FStratFactoryView FactoryView;
		FactoryView.Hex               = HexPoint(Source.hex);
		FactoryView.Owner             = static_cast<int32>(Source.owner);
		FactoryView.bHasBuiltThisTurn = Source.hasBuiltThisTurn;
		FactoryView.bBuildWaiting     = Source.buildWaiting;
		FactoryView.bSpawnBlocked     = Source.spawnBlocked;

		// MATCHED BY HEX, NOT BY INDEX. Both sequences come from the same
		// `Snapshot.factories` in the same canonical order and a positional read would
		// agree today -- which is exactly why it is refused: it would agree silently until
		// the day one of the two orders moved, and a pulse drawn on the wrong factory is a
		// defect nothing on screen distinguishes from a correct one. The set is the
		// shipped scenario's four factories, so the cost of the match is a constant this
		// module does not need to think about.
		for (const FStratFactoryBuildPulse& Pulse : Pulses)
		{
			if (Pulse.Hex == FactoryView.Hex)
			{
				FactoryView.bBuildPulse = Pulse.bShouldPulse;
				break;
			}
		}

		Built.Factories.Add(FactoryView);
	}

	OutModel = MoveTemp(Built);
	return true;
}

// ---------------------------------------------------------------------------
// §2.11.5 — the production menu, per factory.
// ---------------------------------------------------------------------------

// THE FIVE-ARGUMENT FORM IS A FORWARD AND HOLDS NO LOGIC OF ITS OWN. Every existing caller
// uses it and none of them wants §2.11.5's header number; the purse is discarded into a local
// rather than the two bodies being kept in step by hand, which is the arrangement that cannot
// drift. See the header for why the purse had to become an output at all.
bool StratBuildProductionMenu(
	const FStratBridge&            Bridge,
	int32                          Side,
	FIntPoint                      FactoryHex,
	TArray<FStratBuildOptionView>& OutOptions,
	FString&                       OutFailureReason)
{
	int32 UnusedPurse = 0;
	return StratBuildProductionMenu(
		Bridge, Side, FactoryHex, OutOptions, UnusedPurse, OutFailureReason);
}

bool StratBuildProductionMenu(
	const FStratBridge&            Bridge,
	int32                          Side,
	FIntPoint                      FactoryHex,
	TArray<FStratBuildOptionView>& OutOptions,
	int32&                         OutPurseFame,
	FString&                       OutFailureReason)
{
	// Cleared up front so a success cannot leave a previous call's refusal sitting in
	// the caller's string.
	OutFailureReason.Reset();

	// X = q, Y = r, the encoding the whole façade uses. Written out rather than routed
	// through `HexPoint`, which converts the other way.
	strat::Hex Factory;
	Factory.q = static_cast<int>(FactoryHex.X);
	Factory.r = static_cast<int>(FactoryHex.Y);

	// The ONE read, and it is the bridge's answer whole. NAMING `strat::UiBuildOption`
	// is legal in this .cpp and never in the header beside it; CALLING
	// `strat::uiBuildOptions` from this module is not legal at all -- that is the single
	// `LNK2019` this pair's header block records from the StratUI Automation test.
	std::vector<strat::UiBuildOption> Options;
	const FStratResult Result = Bridge.BuildOptions(Side, Factory, Options);
	if (!Result.bOk)
	{
		// The bridge's own words. An empty menu is one of the things it refuses over,
		// so there is nothing to fall back to and nothing to soften.
		OutFailureReason = Result.Reason;
		return false;
	}

	// THE SECOND READ, AND IT EXISTS FOR ONE FIELD. §2.11.5 requires an unaffordable row
	// to name its shortfall (`need 50`), `UiBuildOption` carries no such field, and T-UI-03
	// forbids the widget subtracting. So the purse is read here, from the SAME bridge in
	// the SAME frame as the prices above -- the two calls are separated by nothing that can
	// mutate a const bridge, which is what makes `Shortfall` and `bAffordable` answers about
	// one board state rather than two.
	//
	// THE SNAPSHOT AND NOT `FStratViewModel`. Taking a built model as a parameter would
	// have let a CALLER supply the purse, and a caller that can supply the purse can supply
	// the wrong one; the failure would present as a menu priced for the other player, which
	// in a hot-seat game is the leak this file is otherwise careful about. The signature is
	// unchanged for that reason as much as for its callers.
	strat::UiSnapshot Snapshot;
	const FStratResult SnapshotResult = Bridge.MakeUiSnapshot(Snapshot);
	if (!SnapshotResult.bOk)
	{
		// Unreachable behind a successful `BuildOptions`, which refuses on the same two
		// preconditions. Refused rather than defaulted anyway: a zero purse here would
		// print `need <full price>` on every row of a menu that was otherwise correct.
		OutFailureReason = SnapshotResult.Reason;
		return false;
	}

	// Range-checked by `FStratBridge::BuildOptions` against `strat::SIDE_COUNT` before it
	// answered, and the array is SIDE_COUNT long by the static_assert at the top of this
	// file. Re-checked anyway, because the alternative to a redundant branch is an
	// out-of-bounds read reachable only through a future change to either.
	const int32 SideCount = static_cast<int32>(UE_ARRAY_COUNT(Snapshot.side));
	if (Side < 0 || Side >= SideCount)
	{
		OutFailureReason = FString::Printf(
			TEXT("Side %d is outside the snapshot's %d sides."), Side, SideCount);
		return false;
	}
	const int32 FameTotal = static_cast<int32>(Snapshot.side[Side].fameTotal);

	// ALL-OR-NOTHING: built into a local and moved across on the last line, so a
	// refusal above leaves the caller's menu exactly as it found it.
	TArray<FStratBuildOptionView> Built;
	Built.Reserve(static_cast<int32>(Options.size()));
	for (const strat::UiBuildOption& Source : Options)
	{
		FStratBuildOptionView OptionView;
		OptionView.DefIndex    = static_cast<int32>(Source.defIndex);
		OptionView.Id          = DefinitionName(Source.id);
		OptionView.CostFame    = static_cast<int32>(Source.costFame);
		// COPIED, NEVER RECOMPUTED. `affordable` is the module's comparison of price
		// against purse; redoing it here would be T-UI-03's forbidden arithmetic moved
		// one module rather than removed, and it would be able to disagree.
		OptionView.bAffordable = Source.affordable;
		OptionView.bAvailable  = Source.available;
		// THE ONE PIECE OF ARITHMETIC IN THIS FILE, and the field's own block records why it
		// is here rather than upstream beside `affordable`, and what discharges it.
		//
		// `affordable` DECIDES, THE SUBTRACTION ONLY DESCRIBES. Zero whenever the module
		// says the row is affordable -- never `Max(0, Cost - Fame)` alone -- so the two
		// values cannot disagree about WHETHER the row can be bought even if they were ever
		// to disagree about by how much. The clamp on the other branch is the same
		// discipline pointed the other way: a negative shortfall is not a thing {S}2.11.5 can
		// draw, and printing `need -25` would be this file inventing a state.
		OptionView.Shortfall   = Source.affordable
			? 0
			: FMath::Max(0, OptionView.CostFame - FameTotal);
		// Copied whole and never composed: nothing here appends a price, a hint or a
		// unit name to the sentence the module wrote.
		OptionView.Reason      = Source.reason.empty()
			? FText::GetEmpty()
			: FText::FromString(UTF8_TO_TCHAR(Source.reason.c_str()));

		Built.Add(MoveTemp(OptionView));
	}

	// ASSIGNED TOGETHER, ON THE LAST TWO LINES, AND THE PAIRING IS THE POINT. `OutPurseFame`
	// is the very `FameTotal` every `Shortfall` above was computed against -- the same local,
	// not a second read -- so §2.11.5's header and its `need N` rows cannot describe two
	// different purses. Written here rather than beside the read, so that a refusal between
	// the two leaves both of the caller's values as it found them.
	OutOptions   = MoveTemp(Built);
	OutPurseFame = FameTotal;
	return true;
}

// ---------------------------------------------------------------------------
// §2.8 -- the result, whole.
// ---------------------------------------------------------------------------

bool StratBuildMatchResult(
	const FStratBridge&    Bridge,
	FStratMatchResultView& OutResult,
	FString&               OutFailureReason)
{
	// Cleared up front so a success cannot leave a previous call's refusal sitting in the
	// caller's string.
	OutFailureReason.Reset();

	// The ONE read, and it is the bridge's answer whole. NAMING `strat::UiMatchResult` is
	// legal in this .cpp and never in the header beside it; CALLING `strat::uiMatchResult`
	// from this module is not legal at all -- the `LNK2019` this pair's header block
	// records.
	strat::UiMatchResult Result;
	const FStratResult Asked = Bridge.MatchResult(Result);
	if (!Asked.bOk)
	{
		// The bridge's own words. Its declaration records why an unseeded bridge is refused
		// here and answered InProgress upstream, and there is nothing to soften: a result
		// screen must not be told "no winner yet" by a bridge that holds no match.
		OutFailureReason = Asked.Reason;
		return false;
	}

	// ALL-OR-NOTHING: filled into a local and assigned on the last line.
	FStratMatchResultView Built;
	// THROUGH `ResultTierOf`, THE SAME MAPPING `StratBuildViewModel` USES. A second switch
	// over `strat::ResultTier` in this file would be a second author of §2.8's tier names
	// and could disagree with the one the scoreboard was drawn from.
	Built.Tier         = ResultTierOf(Result.tier);
	Built.Cause        = ResultCauseOf(Result.cause);
	// COPIED, NEVER DERIVED FROM `sideToMove`, and the static_assert above is what makes
	// the copy exact rather than a re-encoding of "nobody".
	Built.Winner       = static_cast<int32>(Result.winner);
	Built.DecidedByKey = static_cast<int32>(Result.decidedByKey);

	OutResult = Built;
	return true;
}

// ---------------------------------------------------------------------------
// §2.11.3's card. The copy, and the three shapings.
//
// EVERY ASSIGNMENT BELOW IS `=` ON ONE FIELD, WITH THREE EXCEPTIONS THAT ARE CHOICES OVER
// BOOLEANS AND NOT OVER NUMBERS. There is no `+`, no `-`, no `*` and no comparison of two
// quantities anywhere in this function. The card's two subtractions live in
// `FStratBridge::AttackForecast`, whose block records why `strat::uiResolveForGate` -- the
// function that exists to do exactly them -- may not be called from production code.
// ---------------------------------------------------------------------------
void StratComposeForecastView(const FStratAttackForecast& Source,
                              FStratForecastView&         OutForecast)
{
	// ALL-OR-NOTHING, and the empty case is written rather than fallen into: an illegal
	// forecast produces a default-constructed card and never a partly-filled one.
	FStratForecastView Built;

	if (!Source.bLegal)
	{
		OutForecast = Built;
		return;
	}

	Built.bHasForecast    = true;
	Built.AttackerUnitId  = Source.AttackerUnitId;
	Built.DefenderUnitId  = Source.DefenderUnitId;

	Built.Damage           = Source.Damage;
	Built.Distance         = Source.Distance;
	Built.DefenderHpBefore = Source.DefenderHpBefore;
	Built.DefenderHpAfter  = Source.DefenderHpAfter;
	Built.AttackerHpBefore = Source.AttackerHpBefore;
	Built.AttackerHpAfter  = Source.AttackerHpAfter;

	Built.bCounterFires = Source.bCounterFires;
	Built.CounterDamage = Source.CounterDamage;
	Built.bDefenderDies = Source.bDefenderDies;
	Built.KillAwardFame = Source.KillAwardFame;

	Built.DefenderTerrainDefensePct = Source.DefenderTerrainDefensePct;   // SIGNED. See Data.h.
	Built.DefenderTerrainId         = Source.DefenderTerrainId;

	// SHAPING 1 -- §2.11.3's counter line, "a number, `out of range`, or `defender
	// destroyed`". THE ORDER IS THE MAPPING and `EStratCounterReason`'s block is the
	// authority on why it is exact today and on the two upstream changes that would make
	// it lie without moving a number. Written as an if/else ladder rather than as a switch
	// because the inputs are two independent booleans and there is nothing to switch on;
	// the dying-defender arm is FIRST because `uiForecast` computes the counter inside
	// `if (!f.defenderDies)`, so a dead defender's `counterFires` is false and the
	// `OutOfRange` arm would otherwise claim a range answer about a corpse.
	if (Source.bDefenderDies)
	{
		Built.CounterReason = EStratCounterReason::DefenderDestroyed;
	}
	else if (Source.bCounterFires)
	{
		Built.CounterReason = EStratCounterReason::Number;
	}
	else
	{
		Built.CounterReason = EStratCounterReason::OutOfRange;
	}

	// SHAPING 2 -- §2.11.3's "and for the attacker whenever the counter is nonzero". Here
	// rather than in a widget binding because of T-UI-03; `FStratForecastView::
	// bShowAttackerHp` records why "it is only a comparison" is not an exemption. NOT
	// `bCounterFires`: a counter that fires for nothing would draw a `20 -> 20` row that
	// teaches the player it did something.
	Built.bShowAttackerHp = Source.bCounterFires && Source.CounterDamage != 0;

	// SHAPING 3 -- §2.11.3's flag band, in BOTH directions. "No player can end a match,
	// theirs or the enemy's, without having been told on the card they clicked", so the
	// attacker's own flag dying to a counter raises it exactly as the enemy flag dying to
	// the attack does. The attacker half rests on `FStratAttackForecast::bAttackerDies`,
	// which is the bridge's field and not the rules module's -- `strat::UiForecast` has no
	// attacker-death field at all, and that asymmetry is recorded on the bridge struct.
	//
	// THE SIDE AND NOT AN OWN/ENEMY BOOLEAN. See `RiskedFlagSide`: the widget compares this
	// against `FStratViewModel::ViewingSide`, so the colour does not rest on the premise
	// that the attacker is always the viewer.
	const bool bDefenderFlagFalls = Source.bDefenderIsFlag && Source.bDefenderDies;
	const bool bAttackerFlagFalls = Source.bAttackerIsFlag && Source.bAttackerDies;

	Built.bFlagAtRisk = bDefenderFlagFalls || bAttackerFlagFalls;

	// THE DEFENDER'S FLAG IS NAMED FIRST WHEN BOTH FALL, and both CAN: a flag attacking a
	// flag, each lethal to the other, is a legal position on any board that gives two flags
	// overlapping range. §2.11.3 describes one band and gives no rule for the pair, so the
	// choice is made here and stated rather than left to whichever branch happened to be
	// written second -- the attack the player is about to commit is the defender's death,
	// and that is the one the card is about.
	Built.RiskedFlagSide = bDefenderFlagFalls ? Source.DefenderSide
	                     : bAttackerFlagFalls ? Source.AttackerSide
	                                          : INDEX_NONE;

	OutForecast = Built;
}

// ---------------------------------------------------------------------------
// §2.11.2's info panel, at the decoration seam.
// ---------------------------------------------------------------------------

void StratDecorateInfoPanel(FStratViewModel& Model)
{
	// BUILT ASIDE AND ASSIGNED ON THE LAST LINE, which here is not only the house
	// all-or-nothing rule: this function reads `Model` and writes into `Model`, so a
	// running write would let a half-filled panel be read by the very lookups that fill
	// the rest of it. A local removes the question rather than answering it.
	FStratInfoPanelView Built;

	// NOT HOVERING IS THE ANSWER "no panel", not a reason to return early. See the
	// declaration's UNCONDITIONAL note: the assignment below must happen on this path too,
	// or a cursor leaving the board leaves its last panel on screen.
	if (Model.Hover.bHasHoveredHex)
	{
		const FIntPoint Hovered = Model.Hover.HoveredHex;

		// FOUND IN THE MODEL'S OWN HEX LIST, AND THE ABSENCE IS AN ANSWER. `Hexes` is in
		// canonical order and this is a linear find rather than an index computed from
		// board dimensions -- a q/r-to-index formula would be this file deciding the
		// board's shape, which is `strat::buildUiSnapshot`'s to decide and would be wrong
		// silently on any board whose emission order changed.
		const FStratHexView* const HexView = Model.Hexes.FindByPredicate(
			[Hovered](const FStratHexView& Candidate) { return Candidate.Hex == Hovered; });

		if (HexView != nullptr)
		{
			Built.bHasHex = true;
			Built.Hex     = HexView->Hex;

			// SELECTED, NOT LOOKED UP. These three came off one terrain row in
			// `StratBuildViewModel`; nothing here reaches a table, and the panel therefore
			// cannot name a different row than the board is drawn from.
			Built.TerrainId         = HexView->TerrainId;
			Built.TerrainMoveCost   = HexView->TerrainMoveCost;
			Built.TerrainDefensePct = HexView->TerrainDefensePct;

			// `Data.h`'s §4.8 sentinel, read and not invented. See the field's own block
			// for why the comparison is here and not in the widget, and why it is not the
			// arithmetic the census forbids.
			Built.bTerrainImpassable = (HexView->TerrainMoveCost == 0);

			// §2.11.2's fourth readout. `bHexCapturable` is the table's own flag and is
			// NOT `HexOwner != INDEX_NONE`: a neutral Factory and a Plains hex carry the
			// same owner, so that inference would put `· neutral` beside every Plains hex
			// on the board. `HexOwner` stays a SIDE -- the yours/enemy reading is the widget's
			// against `Model.ViewingSide`, on `FStratForecastView::RiskedFlagSide`'s rule.
			Built.bHexCapturable = HexView->bTerrainCapturable;
			Built.HexOwner       = HexView->Owner;
			Built.HexIncomeFame  = HexView->TerrainIncomeFame;

			// THE UNIT IS FOUND BY THE HEX, which is what makes the two rows of §2.11.2
			// describe the same place by construction. `Units` is by ascending id and a
			// hex holds at most one unit, so the first match is the only match.
			const FStratUnitView* const UnitView = Model.Units.FindByPredicate(
				[Hovered](const FStratUnitView& Candidate) { return Candidate.Hex == Hovered; });

			if (UnitView != nullptr)
			{
				Built.bHasUnit  = true;
				Built.UnitId    = UnitView->UnitId;
				Built.UnitSide  = UnitView->Side;
				Built.UnitDefId = UnitView->DefId;

				// TWO NUMBERS AND NO RATIO. §2.11.2 asks for `12/20`; the slash is the
				// widget's. See the fields' own block.
				Built.UnitHp    = UnitView->Hp;
				Built.UnitHpMax = UnitView->HpMax;

				// §2.11.2's `Atk/Def/Move/Range`. Selected from the §2.4 row this unit
				// already carries -- no table is reached from here, so the panel's stats
				// are the model's stats by construction.
				Built.UnitStatAtk      = UnitView->StatAtk;
				Built.UnitStatDef      = UnitView->StatDef;
				Built.UnitStatMove     = UnitView->StatMove;
				Built.UnitStatRangeMin = UnitView->StatRangeMin;
				Built.UnitStatRangeMax = UnitView->StatRangeMax;

				// THE PRESENTATION BLOCK'S DONE BIT AND NOT `bHasMoved && bHasActed`.
				// §2.11.2 rules on this by name -- "a waited unit reads `done` while its
				// act flag is unspent" -- so the two snapshot flags are deliberately not
				// consulted, and this is the field the selection machine wrote a moment
				// earlier on this same seam.
				Built.bUnitDone   = UnitView->bDone;
				Built.bUnitIsFlag = UnitView->bIsFlag;
			}
		}
	}

	Model.InfoPanel = Built;
}


// ---------------------------------------------------------------------------
// Sec 2.11.2's command bar, at the decoration seam.
// ---------------------------------------------------------------------------

void StratDecorateCommandBar(
	FStratViewModel& Model,
	bool             bHasFocusedFactory,
	FIntPoint        FocusedFactoryHex)
{
	// BUILT ASIDE AND ASSIGNED ON THE LAST LINE, on `StratDecorateInfoPanel`'s reasoning
	// rather than only the house rule: this function reads `Model` and writes into `Model`,
	// so a running write would let a half-filled block be read by the lookups that fill the
	// rest of it.
	FStratCommandBarView Built;

	// ---- The BUILD control, RESOLVED HERE AND NEVER CARRIED IN ---------------
	//
	// THE CALLER HANDS OVER AN INTENT AND THIS FUNCTION ANSWERS THE QUESTION. That split is
	// the whole design: `FStratBuildAffordance` holds a hex and a bool, and every refresh
	// re-asks whether that hex is still a factory and still this seat's. A caller that
	// computed `bShowBuildButton` for itself would be caching an answer that goes stale on a
	// capture, a hand-over or a conclusion -- see the struct's own block.
	//
	// THE BOOL IS READ FIRST. `FIntPoint(0, 0)` is a real hex on this board and cannot signal
	// its own absence.
	if (bHasFocusedFactory)
	{
		// A LOOKUP AND NOT A DERIVATION. `Factories` is in canonical hex order and a hex
		// holds at most one factory, so the first match is the only match. Nothing here asks
		// the rules module anything: which hexes hold factories and who owns each were
		// answered by `strat::buildUiSnapshot` and copied across by `StratBuildViewModel`.
		const FStratFactoryView* const Factory = Model.Factories.FindByPredicate(
			[FocusedFactoryHex](const FStratFactoryView& Candidate)
			{
				return Candidate.Hex == FocusedFactoryHex;
			});

		// `Owner == ViewingSide` AND NOT `Owner != INDEX_NONE`. A NEUTRAL factory carries
		// `INDEX_NONE`, so on a model whose `ViewingSide` were also `INDEX_NONE` the two
		// spellings differ and this one latches the neutral factory. `StratBuildViewModel`
		// range-checks `ViewingSide` against the snapshot's own side count and refuses rather
		// than producing such a model, so the input is reachable only by hand -- recorded
		// because a clause is exactly the thing that builds one by hand.
		//
		// AND IT IS `ViewingSide` AND NOT `Match.SideToMove`. Sec 2.11.5's BUILD control is a
		// statement about whose SCREEN this is, and in hot-seat the two differ every other
		// turn -- the same distinction `UStratViewModelLibrary::GetViewingSideView` exists to
		// make in one place. A bar wired to `SideToMove` would offer the opponent's factories
		// on every screen drawn between the turn flipping and the hand-over completing.
		if (Factory != nullptr && Factory->Owner == Model.ViewingSide)
		{
			Built.bShowBuildButton = true;
			Built.BuildFactoryHex  = FocusedFactoryHex;
		}
	}

	// ---- The two guidance copies, declared AS copies -------------------------
	//
	// ASSIGNED AND NOT RE-DERIVED. `FStratGuidedOpening::DecorateViewModel` decided both, and
	// this function must run after it for exactly that reason. Copying rather than reading
	// the guidance block from the widget is what gives one control one source; see the
	// fields' own blocks.
	Built.bEndTurnDimmed   = Model.Guidance.bEndTurnGated;
	Built.EndTurnHoverText = Model.Guidance.EndTurnGateHover;

	// ---- The END TURN highlight, single-authored over the existing count ------
	//
	// THIS IS NOT T-UI-03'S FORBIDDEN ARITHMETIC, and the standing precedent for that reading
	// is `UStratMatchSubsystem::ApplyView`, in its "Sec 2.11.6-B's objective ring" block:
	// "THIS IS NOT T-UI-03'S FORBIDDEN ARITHMETIC: it is a visibility condition over two
	// booleans, not a number drawn on screen, and no widget renders the conjunction." The
	// same holds here with one term more. No number reaches the model: the count is compared
	// against zero and only the boolean is stored.
	//
	// OVER `CountViewingSideUnitsAbleToAct` AND NOT OVER A SECOND WALK OF `Model.Units`. That
	// selector is the number Sec 2.11.2's HUD draws, and its four-clause derivation --
	// side, `bDone`, `bLockedThisTurn`, and `!(bHasMoved && bHasActed)` -- is written out on
	// its own declaration so it can be pinned rather than inferred. Re-deriving it here would
	// be a second author of the same fact, and the highlight could then disagree with the
	// number printed beside it.
	//
	// `bHasResult` IS THE FIRST TERM. A concluded match has nothing left to end, and nudging
	// the player to end a turn on a final board would advertise a control Sec 2.8's gate is
	// guaranteed to refuse.
	//
	// `bEndTurnGated` IS ANDED IN ON PURPOSE AND MUST NOT BE DROPPED AS REDUNDANT. It resolves
	// dim-beats-highlight in one place so no widget spells `suggested AND NOT gated`. On the
	// shipped scenario the pair is unreachable -- beat 1a locks every unit but the marked
	// Infantry and locked units do not count, so a gated turn always has one unit able to act
	// -- but that is a fact about `FStratGuidedOpening::PublishLocks` and not a structural
	// guarantee. This `&&` is what makes it one.
	Built.bEndTurnSuggested =
		   !Model.Match.bHasResult
		&& !Model.Guidance.bEndTurnGated
		&& UStratViewModelLibrary::CountViewingSideUnitsAbleToAct(Model) == 0;

	Model.CommandBar = Built;
}
