# SYSTEM

You are a code-generation step inside a goal-oriented agent working on Stratocracy, a
turn-based hex strategy game in Unreal Engine 5.8 and C++17.

You write source files. You do not choose what to build -- a priority scorer already
did that -- and you do not decide whether your output is accepted; a human reviews
every file before it lands.

HOUSE STYLE, matched from the files you are shown rather than invented:

- Comments explain WHY, not what. This project's headers carry long reasoning blocks
  naming the alternative that was rejected and the reason. Match that register.
- Never claim something was "measured" or "verified" unless the fact is stated in the
  source you were given. An invented measurement is worse than no comment.
- Tabs for indentation in .cpp/.h under Source/, matching the existing files.
- No hardcoded gameplay values. Anything the scenario or the data tables supply is
  READ from them. This is a graded property of the work, not a preference.
- Unreal conventions: `U`/`F`/`E` prefixes, `UPROPERTY`/`UFUNCTION` where reflection is
  needed, `TEXT()` around literals, `FString`/`TArray` over std types at engine-facing
  boundaries.

OUTPUT FORMAT -- exactly this, once per file, and nothing outside the blocks except a
short plain-text rationale before the first block:

=== FILE: Source/Some/Path.h ===
<the complete file contents>
=== END FILE ===

Emit COMPLETE files. Never abbreviate with "// ... rest unchanged ...".


# USER

GOAL: Runtime host: own a bridge, seed the scenario, put the scoreboard on screen

WHY THIS ONE: the priority scorer selected it. §4.9 names the bridge as the only code that knows both worlds, and §4.7 Stub 8 has widgets bind to the view model it projects. Nothing outside the Automation tests constructs an FStratBridge -- StratBridge.Build.cs says so in as many words ('Nothing in the game module calls the bridge yet') -- so no widget can be fed at runtime, only in a test.

WHERE IT LANDS: Source/StratUI/ (HUD or subsystem)

THE PROBLEM. Nothing outside the Automation tests ever constructs an
`FStratBridge`. `StratBridge.Build.cs` says so itself -- 'Nothing in the
game module calls the bridge yet.' So the scoreboard is proven by a test
and cannot appear in a running game: there is no live bridge, no seeded
scenario, and nothing that creates the widget or refreshes it.

Build the host. An `AHUD` subclass is the natural shape: the map's GameMode
can point at it without the game module ever depending on this one, which
matters because the dependency runs StratBridge -> Stratocracy and cannot
be reversed.

WHAT TO PRODUCE: exactly ONE file, Source/StratUI/StratScoreboardHUD.h.

IT MUST, on BeginPlay: load the two DataTables, seed the bridge from the
shipped scenario, create the widget, add it to the viewport, and refresh
it. Expose a BlueprintCallable refresh so the game can call it again after
a command.

CONSTRAINTS THAT ARE NOT NEGOTIABLE:

1. THE HEADER MUST NOT INCLUDE StratBridge.h. It declares a UCLASS, so UHT
   parses it, and StratBridge.h pulls in the vendored `strat` headers --
   the thing StratBridge.h's own comment exists to prevent. Forward declare
   `class FStratBridge;` and hold it as `TUniquePtr<FStratBridge>`. That
   means an incomplete type at the member declaration, so DECLARE the
   destructor in the header and DEFINE it in the .cpp -- TUniquePtr cannot
   instantiate its deleter against an incomplete type at an implicit
   destructor. Include StratBridge.h only from the .cpp.

2. NO CROSS-MODULE `strat::` CALLS. The vendored sources carry no _API
   macro, so UnrealEditor-StratBridge.dll exports FStratBridge and nothing
   beside it; a direct `strat::` call from this module fails to LINK with
   LNK2019. This is measured, twice. Everything goes through methods on
   FStratBridge.

3. NO HARDCODED ASSET PATHS OR GAMEPLAY VALUES. The two DataTables, the
   scenario file name and the viewing side are `UPROPERTY(EditDefaultsOnly)`
   so they are set on a Blueprint subclass, not baked into C++. The widget
   class is `UPROPERTY(EditDefaultsOnly) TSubclassOf<UStratScoreboardWidget>`
   -- never a ConstructorHelpers path literal.

4. FAIL LOUDLY AND SURVIVE. A missing table or an unreadable scenario logs
   through `LogStratUI` with the bridge's own refusal reason and leaves the
   HUD without a scoreboard. It must not crash and must not display a panel
   of zeroes -- the widget already refuses rather than showing zeroes, and
   this must not defeat that.

----- Source/StratUI/StratScoreboardWidget.h -- what you spawn and refresh (READ ONLY) -----
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


----- Source/StratBridge/StratBridge.h -- the bridge API, the ONLY way to reach the rules (READ ONLY) -----
// GDD §4.9 part 2 -- the bridge: the only code that knows both worlds.
//
// It lives in its own `StratBridge` module and NOT in Source/StratRules/, which
// T-INT-01 requires to contain nothing unaccounted for against the recorded crew
// commit. Nothing here is vendored; nothing here is a rule.
//
// WHY ITS OWN MODULE RATHER THAN THE GAME MODULE, which is where this file first
// sat: an editor target is a MODULAR build, so each module is a DLL, and Unreal
// exports only symbols carrying an `_API` macro. The vendored `strat::` sources
// carry none -- §4.9 forbids them engine headers -- so no other module can link a
// call to them, measured as 8 x LNK2019 the first time this bridge tried. The
// rules sources are therefore compiled INTO this module (see Vendored/), and the
// exported surface is `FStratBridge`, which was written to be exported. See
// StratBridge.Build.cs.
//
// WHAT THIS OWNS: the authoritative `strat::GameState`. §4.1's "never own rules" is
// structural here rather than aspirational -- actors and UMG hold no rules state,
// because the state is in this object and they have no copy of it to drift from.
//
// WHAT THIS DECIDES: nothing. Every refusal below is a refusal the rules module
// returned, forwarded with that module's own reason. The bridge maps engine types
// to module types and carries answers back; where it would have to choose, it
// refuses instead and says why. The view-model surface added below is held to the
// same line: it GATHERS a `UiWorld` and hands it to `strat::buildUiSnapshot`, and
// computes no field of the result itself.
//
// NOT A UOBJECT, and deliberately. UHT parses headers that declare reflected types,
// and this one declares none, so the vendored `strat` headers it includes are never
// put in front of the header tool. That constraint is why `MakeUiSnapshot` hands
// back a `strat::UiSnapshot` by reference parameter rather than a USTRUCT: a
// reflected mirror of the view model would have to be declared in a header UHT
// reads, and that header could then never include Ui.h. The UMG-facing wrapper that
// copies fields out of the snapshot into reflected types is a later, separate file;
// the state does not move out of here when it arrives.
//
// NOT IN THIS ROUND: the ordered event list (§4.9 part 2's "command in / events
// out") and the widgets. T-INT-02, T-INT-03 and T-SAVE-06 assert over the state
// hash and the rejection channel, which is what this file provides. The event list
// is ruled to live headless, and no acceptance ID names it as its subject.
#pragma once

#include "CoreMinimal.h"

// Vendored rules headers, by short name via StratRules' PublicIncludePaths.
#include "Data.h"
#include "Replay.h"
#include "Save.h"
#include "Scenario.h"
#include "Ui.h"

class UDataTable;

// A refusal carries the reason the owning module gave. `Id` is that module's own
// failing-invariant tag where it supplied one (e.g. "T-SAVE-05"), empty otherwise.
struct FStratResult
{
	bool    bOk = false;
	FString Id;
	FString Reason;

	static FStratResult Ok() { FStratResult R; R.bOk = true; return R; }
	static FStratResult Fail(const FString& InReason, const FString& InId = FString())
	{
		FStratResult R;
		R.bOk = false;
		R.Id = InId;
		R.Reason = InReason;
		return R;
	}
};

class STRATBRIDGE_API FStratBridge
{
public:
	// ---- Load ------------------------------------------------------------
	// §4.9 part 2's Load bullet: FUnitRow / FTerrainRow -> strat::UnitDef /
	// strat::TerrainDef. The DataTables are the source, per that bullet.
	//
	// ORDER IS LOAD-BEARING AND IS NOT DECORATION. `defIndex` indexes these
	// vectors, and a §4.10 Build command carries a defIndex in its `unitId`
	// field. A vector built in a different order than the headless loader's
	// would resolve the same log to a different unit type and diverge silently
	// -- so the row order is taken from the table and then ASSERTED equal to
	// strat::loadUnits over the same vendored CSV by the editor pass, rather
	// than assumed here.
	FStratResult LoadDefinitions(const UDataTable* UnitTable, const UDataTable* TerrainTable);

	// Loads the shipped scenario file's text and seeds the authoritative state
	// through strat::seedFromScenario -- the single scenario -> GameState
	// mapping, shared with the headless side so neither can drift from it.
	//
	// The parsed scenario is RETAINED past this call, which it was not before.
	// See `LoadedScenario` below for why the view model cannot be built without
	// it.
	FStratResult LoadScenarioFromFile(const FString& ScenarioFilePath, int32 FirstSide);

	// ---- Command in ------------------------------------------------------
	// Applies one command through the rules module. An invalid command returns
	// a rejection reason and changes nothing (§4.9).
	//
	// APPLIED DIRECTLY, NOT TO A COPY. Copying here and assigning on success
	// would make T-INT-03's "no partial application" clause true of the bridge
	// while hiding whether it is true of the rules module -- the gate would
	// then assert a property of this file rather than of the thing under test.
	FStratResult Submit(const strat::SaveCommand& Command);

	// Replays a whole log, all-or-nothing (T-SAVE-05's property, the module's).
	FStratResult ReplayLog(const TArray<strat::SaveCommand>& Log);

	// ---- Queries ---------------------------------------------------------
	// §4.10's canonical state hash, computed by the rules module.
	FString StateHash() const;

	const strat::GameState& State() const { return GameState; }
	bool IsLoaded() const { return bDefinitionsLoaded; }
	bool IsSeeded() const { return bSeeded; }

	const std::vector<strat::UnitDef>& UnitDefs() const { return Units; }

	// Borrowed and const, the way every rules module takes what it does not own.
	strat::RulesTables Tables() const;

	// The scenario this bridge last seeded from. Meaningful only while
	// IsSeeded(); a bridge that never seeded, or whose definitions were reloaded
	// underneath it, holds a default-constructed one.
	const strat::Scenario& ScenarioData() const { return LoadedScenario; }

	// ---- View model ------------------------------------------------------
	// §4.7 Stub 8's projection input, GATHERED and not decided -- the same three
	// module-side sources §4.9 names, and nothing else: the authoritative
	// `GameState`, the §4.8 tables, and the loaded scenario file.
	//
	// WHY THIS EXISTS AT ALL: `strat::buildUiSnapshot` takes a `strat::UiWorld`,
	// and the only other function in the tree that produces one is the headless
	// driver's `uiWorldOf(const Session&)`. A Session is the driver's own type
	// and this object does not have one, so before this method no engine-side
	// caller could reach the view model. This mirrors `uiWorldOf` field for
	// field so the two projections cannot disagree; where the driver had to
	// hand-roll a derivation because `DriverUnit` is not a `GameUnit` (the flag
	// designation, the board), this calls the module function instead, because
	// we DO hold a `GameState` and a second copy of a derivation is exactly the
	// drift §4.9 is trying to prevent.
	//
	// BORROWS, DOES NOT OWN. The returned world's `unitDefs`, `terrain`,
	// `economy`, `turn` and `guided` all point into this object. It is a
	// short-lived argument for the projection call below, not something to
	// store: outliving the bridge, or surviving a LoadDefinitions, dangles it.
	// Prefer MakeUiSnapshot, which never lets the world escape.
	//
	// Valid only when IsSeeded(). On an unseeded bridge it faithfully projects
	// an empty state -- no hexes, no units -- rather than inventing one.
	strat::UiWorld MakeUiWorld() const;

	// The ONLY projection (§4.7 Stub 8). It delegates to the rules module and
	// performs NO arithmetic of its own -- not a sum, not a count, not a
	// derived flag. Every field of the result is the module's answer; if a
	// number here is wrong, it is wrong in Ui.cpp and T-UI-05 is the gate that
	// says so, which stops being true the moment this file computes anything.
	//
	// Returns a refusal rather than an empty snapshot when there is nothing to
	// project, so a caller cannot mistake "not loaded yet" for "a match with no
	// units in it".
	FStratResult MakeUiSnapshot(strat::UiSnapshot& OutSnapshot) const;

	// T-UI-05's own check, run over a snapshot this bridge projected, with the
	// failures handed back as engine strings.
	//
	// IT IS EXPOSED HERE RATHER THAN CALLED DIRECTLY BY THE CALLER, and that is
	// this module's whole reason for existing rather than a convenience. The
	// vendored `strat` sources carry no `_API` macro -- §4.9 forbids them engine
	// headers -- so `UnrealEditor-StratBridge.dll` exports `FStratBridge` and
	// nothing else, and a call to `strat::uiCheckSnapshotFidelity` from any other
	// module does not link. MEASURED, not assumed: the StratUI Automation test
	// called it directly and the build failed with LNK2019 on exactly that symbol,
	// which is the same failure StratBridge.Build.cs records the first bridge
	// hitting. Every strat entry point a widget or its gate needs arrives through
	// a method on this class for that reason.
	//
	// RETURNS THE MODULE'S VERDICT AND NOT A RECOUNT OF IT. `Result.ok` is
	// forwarded rather than inferred from an empty failure list: Ui.h's clause (c)
	// fails a snapshot field carrying no contract entry, and treating "nothing
	// listed" as success would let this method disagree with the check it ran.
	FStratResult CheckSnapshotFidelity(const strat::UiSnapshot& Snapshot,
	                                   TArray<FString>& OutFailures) const;

private:
	// Owned by value: this object IS the authoritative state.
	strat::GameState GameState;

	// The §4.8 tables, mapped once and then borrowed by every call.
	std::vector<strat::UnitDef>    Units;
	std::vector<strat::TerrainDef> Terrain;

	// RETAINED, where it used to be a local that died at the end of
	// LoadScenarioFromFile. `UiWorld::guided` has no other source in the tree:
	// the guided seats are scenario-file input, `seedFromScenario` does not copy
	// them into `GameState` (they are not match state -- `scenarioHash` pins
	// them), and `isGuidedMarked` is silently false for every unit when the
	// pointer is null. Dropping the scenario therefore did not fail loudly, it
	// unmarked the one unit beat 1a is about. Held whole rather than as a
	// copied-out guided list because the file is the source and a second
	// extraction is a second thing that can disagree with it.
	strat::Scenario LoadedScenario;

	// Assembles the combat stat block for one unit exactly as the driver's
	// `combatUnit` does: every stat LOOKED UP from the UnitDef at `defIndex`,
	// current hp from the unit. Nothing is stored -- a cached copy of a stat is
	// a copy that can outlive a table reload.
	strat::Unit CombatUnitOf(const strat::GameUnit& U) const;

	bool bDefinitionsLoaded = false;
	bool bSeeded = false;
};


----- Source/StratUI/StratUI.Build.cs -- your module (READ ONLY) -----
// GDD §4.7 Stub 8 / §2.11 -- the widget module. It holds the UMG surface that binds
// to the view model and NOTHING ELSE: no rules, no state, no arithmetic over what the
// snapshot already carries (T-UI-03's no-widget-side-arithmetic clause).
//
// WHY THIS IS ITS OWN MODULE RATHER THAN LIVING IN `StratBridge`, which is the only
// other module that could host it today. Two reasons, and the second is the one that
// cannot be undone later.
//
// FIRST: `StratBridge.h` states that it declares no reflected types on purpose, so the
// vendored `strat` headers it includes are never put in front of UHT. Adding a UCLASS
// to that module would put a reflected header beside those includes and start eroding
// the property that file was written to hold. Widgets live outside it instead.
//
// SECOND, AND IRREVERSIBLE: a Blueprint that derives from a UMG class bakes that
// class's script path into the asset -- `/Script/StratUI.StratScoreboardWidget`. This
// is the same trap `StratBridge.Build.cs` records for FUnitRow / FTerrainRow, whose
// `/Script/Stratocracy.UnitRow` paths are baked into DT_Units and DT_Terrain and which
// is why the bridge's dependency arrow points at the game module rather than the other
// way. Deciding the widget's module AFTER a WBP_ asset derives from it costs asset
// surgery; deciding it now costs this file.
//
// THE DEPENDENCY RUNS StratUI -> StratBridge AND NEVER BACK. The bridge owns the
// authoritative `strat::GameState`; this module owns pixels. A reverse dependency
// would let the bridge reach a widget, which is the arrangement §4.1's "never own
// rules" exists to prevent -- and it would also reintroduce the UHT problem above.
//
// NOTE ON WHAT IS NOT HERE: the game module `Stratocracy` is deliberately not a
// dependency. Nothing in this module needs FUnitRow or FTerrainRow -- it binds to the
// projected view model, not to the DataTable rows the bridge maps in -- and reaching
// for the game module would couple the widget layer to the row structs for no gain.

using UnrealBuildTool;

public class StratUI : ModuleRules
{
	public StratUI(ReadOnlyTargetRules Target) : base(Target)
	{
		// The same pair `StratBridge.Build.cs` and `StratRules.Build.cs` carry, for the
		// same underlying reason rather than by imitation: this module's .cpp files
		// include `StratBridge.h`, which includes the vendored `strat` headers, so the
		// compile environment those headers land in is this module's. Sharing the
		// engine PCH would put them in an environment whose warning settings this file
		// does not control. Kept conservative here; if a later pass measures that a
		// shared PCH is safe for this module, the comment should record the
		// measurement rather than the setting being dropped quietly.
		PCHUsage = PCHUsageMode.NoPCHs;
		bUseUnity = false;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			// The view-model source. `StratBridge` re-exports the vendored headers by
			// short name through its own PublicIncludePaths, so this module reaches
			// `Ui.h` and `Replay.h` through that dependency and vendors nothing itself.
			"StratBridge"
		});

		// UMG pulls Slate and SlateCore behind it, but a module that names UUserWidget
		// in its own headers declares all three rather than relying on that.
		PrivateDependencyModuleNames.AddRange(new string[] {
			"UMG",
			"Slate",
			"SlateCore"
		});

		PublicIncludePaths.AddRange(new string[] {
			"StratUI"
		});
	}
}


----- Source/StratUI/StratUI.h -- the log category (READ ONLY) -----
// GDD §4.7 Stub 8 / §2.11 -- the widget module's own header. See StratUI.Build.cs for
// why this module exists separately from `StratBridge`.
//
// It declares a log category and nothing else. In particular it declares NO reflected
// type and includes no `strat` header: the widget headers in this module forward
// declare `FStratBridge` and include `StratBridge.h` only from their .cpp files, so
// UHT never parses the vendored rules headers. That is the same discipline
// `StratBridge.h` states for itself, carried one module outward.
#pragma once

#include "CoreMinimal.h"

/** Widget-layer log category. Separate from LogStratocracy so a binding failure in a
 *  widget is not read as a failure in the game module. */
DECLARE_LOG_CATEGORY_EXTERN(LogStratUI, Log, All);


----- Source/StratUI/Tests/StratScoreboardParity.cpp -- how the test loads the tables and seeds the scenario; do the same thing (READ ONLY, first 190 lines) -----
// The editor pass for §2.11.4's live standings scoreboard -- T-UI-03. GDD §2.11, §2.8,
// §2.13.2, §4.11 row 8.
//
//   T-UI-03  the live standings scoreboard (§2.11, §2.8) binds 1:1 to snapshot fields
//            -- enemy strength destroyed, objectives held X/N, surviving units/HP,
//            turn vs cap -- with no widget-side arithmetic.
//
// WHAT "BINDS 1:1" IS MADE TO MEAN HERE, because an acceptance ID that is not made
// operational is decoration. Every clause below takes a `strat::UiSnapshot` from the
// bridge, takes an `FStratScoreboardModel` built from the SAME bridge, and asserts
// that each displayed value is EQUAL TO ONE NAMED FIELD of that snapshot. The
// expectation side of every comparison is a field read, never a derivation: this file
// contains no sum over `Snapshot.units`, no difference of two sides' fame, no
// percentage. If the test computed the expectation, a widget and a test that made the
// same arithmetic mistake would agree, and the gate would certify the mistake.
//
// WHY THE FIDELITY CHECK RUNS HERE TOO, and it is not padding. Binding 1:1 to a
// snapshot only means anything if the snapshot is true of the state it claims to
// project. A scoreboard can be perfectly bound and still lie all match if the
// projection under it lies -- and §2.11.4's whole reason for existing is revision
// §1.5-#1, the tiebreak must never be a hidden win condition. So the last clause runs
// `strat::uiCheckSnapshotFidelity` over the bridge's own projection and fails on any
// reported failure. T-UI-05 owns that check as its subject; this file consumes it,
// because a green T-UI-03 over a lying projection would be a false clearance of the
// thing §1.5-#1 asked for.
//
// WHY THE STATE UNDER TEST IS THE SEEDED OPENING AND NOT A MID-MATCH POSITION. The
// only mid-match position this tree can reach deterministically is the one
// `Data/parity_fixture.save` replays to, and reaching it here would couple this gate
// to the §4.10 log format for no gain in what it asserts: the clauses below check that
// the model AGREES WITH the snapshot, and that agreement is a property of the builder
// rather than of any particular board. It is stated rather than hidden, though: the
// chevron clause exercises whichever §2.8 criterion Ferrum Crossing's opening happens
// to decide on, and a variant driven to a position where an EARLIER criterion decides
// is a later pass, not something this file quietly claims to have covered.
//
// NOT ASSERTED HERE, with reasons:
// - §2.11.4's cap-approach banners. StratScoreboardWidget.h records that their trigger
//   is arithmetic over `turn` and `turnCap` with no field carrying the answer, and that
//   the banner needs an owner ruled first. There is nothing to bind, so there is
//   nothing to assert.
// - The end-of-match screen and its faction-voiced result line. A different surface
//   with a different lifetime; kb/setting.md's pipeline note is why it must not be
//   folded into the persistent panel, and folding it into this file would be the same
//   mistake one layer up.
// - Whether `survivingHp` is the correct sum over that side's units. That is the
//   projection's arithmetic, which is the fidelity check's subject and T-UI-05's ID.
//   Recomputing it here would give this gate a second opinion about a number it is
//   supposed to be checking the TRANSPORT of.
// - Pixels. The builder is a free function precisely so no Slate tree is needed to see
//   a value; nothing below constructs a widget.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Internationalization/Text.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratScoreboardWidget.h"

// The bridge, included from a .cpp and never from a UHT-parsed header -- the same
// constraint StratScoreboardWidget.h and StratUI.Build.cs both record. A test file is
// a .cpp, so it may say this.
#include "StratBridge.h"

// The vendored view-model header, by short name via StratRules' PublicIncludePaths.
// Reached through StratBridge.h already; named explicitly because this file reads
// `strat::UiSnapshot` fields directly and a reader should not have to discover that
// through a transitive include.
#include "Ui.h"

namespace StratScoreboardParity
{
	// MUST match the side the rest of the engine-side harness seeds with. Replay.h
	// states that no rule in this project decides which side moves first, so this is a
	// harness pin and not a rule: it exists so the projected opening position is the
	// same one every run, and so a divergence shows up as a failed clause rather than
	// as a scoreboard that reads differently on alternate runs. StratBridgeParity.cpp
	// pins the same value for the same reason.
	static const int32 kFirstSide = 0;

	// The viewing side these clauses build for by default. NOT `sideToMove`: the
	// snapshot carries whose turn it is, this is whose screen it is, and
	// StratScoreboardWidget.h is explicit that the two differ every other turn. The
	// ColumnsFollowViewingSide clause below builds for both.
	static const int32 kViewingSide = 0;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static FString FromStd(const std::string& In)
	{
		return FString(UTF8_TO_TCHAR(In.c_str()));
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/**
	 * Brings a bridge to the seeded opening: the shipped tables mapped in, the shipped
	 * scenario seeded through the rules module's own `seedFromScenario`.
	 *
	 * Returns false and fills OutError rather than asserting, on StratBridgeParity's
	 * precedent: a missing asset becomes a named failure inside the caller's own clause
	 * list instead of a crash in a helper, and every clause reports setup failure the
	 * same way.
	 *
	 * NOTHING IS HARDCODED PAST THE ASSET PATHS. The turn cap, the objective total and
	 * both sides' standings all arrive from `ferrum_crossing.json` and the two tables
	 * through the projection; §2.11.4's "/ 20" and "N = 8" appear nowhere in this file,
	 * and a scenario edit that changed either must not need an edit here.
	 */
	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		UDataTable* Units = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		UDataTable* Terrain = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
		if (Units == nullptr || Terrain == nullptr)
		{
			OutError = TEXT("DT_Units and/or DT_Terrain did not load from /Game/StratData");
			return false;
		}

		const FStratResult Loaded = Bridge.LoadDefinitions(Units, Terrain);
		if (!Loaded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadDefinitions refused: %s"), *Loaded.Reason);
			return false;
		}

		const FString ScenarioPath = FPaths::Combine(DataDir(), TEXT("ferrum_crossing.json"));
		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath, kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	/**
	 * Seeds a bridge, projects a snapshot, and builds the model for `ViewingSide`.
	 *
	 * TWO PROJECTIONS OF ONE STATE, and that is safe here for a stated reason rather
	 * than by hope: no command is submitted between them, so the authoritative
	 * `GameState` is byte-identical across both calls, and `MakeUiSnapshot` is the
	 * rules module's function over that state. The alternative -- having the builder
	 * hand its snapshot back out -- would have widened the production surface purely to
	 * suit a test, which is the tail wagging the dog.
	 */
	static bool Project(
		FStratBridge&           Bridge,
		int32                   ViewingSide,
		strat::UiSnapshot&      OutSnapshot,
		FStratScoreboardModel&  OutModel,
		FString&                OutError)
	{
		if (!SeedBridge(Bridge, OutError))
		{
			return false;
		}

		const FStratResult Projected = Bridge.MakeUiSnapshot(OutSnapshot);
		if (!Projected.bOk)
		{
			OutError = FString::Printf(TEXT("MakeUiSnapshot refused: %s"), *Projected.Reason);
			return false;
		}

		if (!StratBuildScoreboardModel(Bridge, ViewingSide, OutModel, OutError))
		{
			OutError = FString::Printf(TEXT("StratBuildScoreboardModel refused: %s"), *OutError);
			return false;
		}
		return true;
	}

	/**
	 * The column that is not the viewer's, found by scanning rather than by
	 * `1 - ViewingSide` -- the same reasoning StratScoreboardWidget.cpp gives for the
	 * same choice. The subtraction is correct for two sides and is the kind of correct
	 * that stops being correct without failing to compile; the scan says "the side that
	 * is not the viewer", and the two-side check at each call site is what makes the
	 * answer unique.
	 */

