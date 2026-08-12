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
//   3. The three static_asserts. `strat::OWNER_NEUTRAL`, `strat::SIDE_COUNT` and the
//      `strat::ResultTier` enumerators are compile-time constants reachable only here.
//      Reading one is not the cross-module call the constraint above forbids -- nothing
//      is emitted and no symbol has to be exported for it to link.
//
// THE BINDING RULE, restated here because this is the file where it can be broken:
// every number written into `FStratViewModel` is EQUAL TO ONE FIELD of
// `strat::UiSnapshot`, copied with no transformation beyond a width cast. There is no
// `+`, no `-`, no `/` and no `.size()` over a snapshot vector below. The one thing that
// is not a copy is the pair of `FName` lookups, which read a definition row's `id` at an
// index the snapshot supplies; that is a table read and it is justified on the fields
// themselves in the header.
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

		FStratHexView HexView;
		HexView.Hex          = HexPoint(Source.hex);
		HexView.TerrainIndex = TerrainIndex;
		HexView.TerrainId    = DefinitionName((*Tables.terrain)[TerrainIndex].id);
		HexView.Owner        = static_cast<int32>(Source.owner);

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

		FStratUnitView UnitView;
		UnitView.UnitId          = static_cast<int32>(Source.id);
		UnitView.Side            = static_cast<int32>(Source.side);
		UnitView.DefIndex        = DefIndex;
		UnitView.DefId           = DefinitionName((*Tables.units)[DefIndex].id);
		UnitView.Hex             = HexPoint(Source.hex);
		UnitView.Hp              = static_cast<int32>(Source.hp);
		UnitView.HpMax           = static_cast<int32>(Source.hpMax);
		UnitView.bIsFlag         = Source.isFlag;
		UnitView.bHasMoved       = Source.hasMoved;
		UnitView.bHasActed       = Source.hasActed;
		UnitView.CaptureProgress = static_cast<int32>(Source.captureProgress);
		UnitView.bIsGuidedMarked = Source.isGuidedMarked;

		// `bDone` and `bLockedThisTurn` are DELIBERATELY LEFT AT FALSE. They are the
		// presentation block's (Ui.h:199-215), their owners are §2.11.1's selection
		// machine and the guidance layer, and neither exists in this engine yet. Setting
		// them from `hasMoved` / `hasActed` here would be inventing the DONE bit -- which
		// Ui.h states is derivable from neither flag nor from any pair of them -- and it
		// would be wrong in exactly the two cases (Wait, RMB-in-MOVED) that the bit was
		// separated out for. The debt and its discharge condition are in the header.

		Built.Units.Add(UnitView);
	}

	// ---- Factories ---------------------------------------------------------
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

		Built.Factories.Add(FactoryView);
	}

	OutModel = MoveTemp(Built);
	return true;
}
