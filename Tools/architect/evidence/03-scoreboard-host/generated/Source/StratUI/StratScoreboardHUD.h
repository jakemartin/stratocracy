// GDD §4.9 part 2 / §4.7 Stub 8 / §2.11.4 -- the runtime host for the scoreboard.
//
// WHAT THIS FILE IS FOR, stated as the gap it closes. `StratBridge.Build.cs` records
// that "nothing in the game module calls the bridge yet", and that was still true of
// the whole project: the only code that ever constructed an `FStratBridge` was the
// Automation pass in Source/StratUI/Tests/. So §2.11.4's scoreboard was proven by a
// test and could not appear in a running game -- there was no live bridge, nothing had
// seeded a scenario into one, and nothing created the widget. This class is the one
// object that does those four things in order: load the two §4.8 tables, seed the
// shipped scenario through the bridge, create the widget, refresh it.
//
// WHY AN AHUD AND NOT A UGameInstanceSubsystem, which was the first shape tried.
// Three reasons, in the order they decided it.
//   1. THE DEPENDENCY ARROW. StratUI -> StratBridge -> Stratocracy, and
//      StratUI.Build.cs plus StratBridge.Build.cs both record that it cannot be
//      reversed (the vendored sources carry no _API macro; the DataTable row structs
//      bake `/Script/Stratocracy.*` paths into DT_Units and DT_Terrain). A map's
//      GameMode points at a HUD class as ASSET DATA -- a Blueprint reference, not an
//      #include -- so the game module gets a live scoreboard without ever naming this
//      module in C++ or in its .Build.cs. A subsystem would have to be found and
//      poked by somebody, and the only somebody is the game module.
//   2. LIFETIME. A GameInstance subsystem outlives the map, and the seeded state is
//      per-map: a subsystem holding the bridge would carry one scenario's GameState
//      into the next level and have to remember to tear it down. A HUD is destroyed
//      with the world it was spawned into, which is the lifetime the state actually
//      has.
//   3. It is client-local and per-player, which is what a scoreboard is.
//
// AND THE PART THAT IS OWED, recorded rather than quietly owned: this HUD OWNS the
// authoritative `strat::GameState` today because nothing else in the running game does.
// That is the wrong long-term owner -- §4.1's "never own rules" is about actors not
// holding rules state, and a HUD is an actor. The moment a proper owner exists (a game
// state, a world subsystem, whatever the §4.10 command loop lands on), this class MUST
// drop to BORROWING a bridge it did not create, and `Bridge` becomes a raw observing
// pointer. The refresh path below is already written that way: `RefreshScoreboard`
// reads through `GetBridge()` and does not care who allocated it.
//
// WHY THIS HEADER MUST NEVER INCLUDE StratBridge.h. It declares a UCLASS, so UHT
// parses it. `StratBridge.h` includes the vendored `strat` headers (Data.h, Ui.h, ...)
// and its own comment records why that is safe THERE: it "declares no reflected types
// on purpose, so the vendored `strat` headers it includes are never put in front of
// UHT". Including it here would put those headers in front of the header tool and
// destroy the property that file was written to hold. StratScoreboardWidget.h refuses
// the same include for the same reason, and StratUI.Build.cs and StratUI.h both record
// the constraint from the module side. `FStratBridge` is therefore forward declared,
// held as `TUniquePtr<FStratBridge>`, and included ONLY from the .cpp.
//
// THE CONSEQUENCE OF THAT FORWARD DECLARATION, spelled out because it is easy to undo
// by accident: `TUniquePtr` instantiates its deleter where the destructor is defined,
// and a deleter cannot be instantiated against an incomplete type. So the constructor
// AND destructor are declared here and defined in the .cpp, after the include. Deleting
// either declaration, or writing `= default` on it here, breaks the build in a way
// whose error message points at TUniquePtr rather than at this decision.
//
// NO CROSS-MODULE `strat::` CALLS FROM THIS FILE OR ITS .cpp. The vendored sources
// carry no _API macro -- §4.9 forbids them engine headers -- so
// UnrealEditor-StratBridge.dll exports `FStratBridge` and nothing beside it. A direct
// `strat::` call from this module does not LINK: measured twice already, once as the
// 8 x LNK2019 that moved the rules sources into the bridge module (StratBridge.h), and
// once as the StratUI Automation test calling `strat::uiCheckSnapshotFidelity`
// directly. Everything this class needs arrives as a method on `FStratBridge`. That is
// also why `RefreshScoreboard` reports failure as `bool` + `FString` rather than
// returning `FStratResult`: naming that type here requires the forbidden include.
//
// NOTHING BELOW IS A GAMEPLAY VALUE. The two tables, the scenario file, the first side
// and the viewing side are all `EditDefaultsOnly`, set on a Blueprint subclass. The
// turn cap, the objective total N and both sides' standings arrive from the scenario
// and the tables through the projection and appear nowhere in this module -- the same
// property Source/StratUI/Tests/StratScoreboardParity.cpp keeps, and for the same
// reason: a scenario edit must not need a C++ edit. In particular the widget class is a
// `TSubclassOf` and never a `ConstructorHelpers::FClassFinder` path literal, which
// would put `/Game/...` back into C++ and make the asset unmovable.
//
// FAILING: a missing table, an unreadable scenario or an unset widget class logs
// through `LogStratUI` with the BRIDGE'S OWN refusal reason forwarded unchanged, and
// leaves the HUD with no scoreboard. It does not crash and it does not put an empty
// panel on screen. That last part is load-bearing rather than tidy: the widget already
// refuses to draw rather than showing zeroes, because §2.11.4 exists for revision
// §1.5-#1 and "both sides have destroyed nothing" is a real, banner-carrying state that
// must not be counterfeited by a failed load. A host that created the widget anyway and
// left `Model` at its defaults would defeat that refusal from above.
//
// NOT IN THIS ROUND, with reasons:
// - Submitting commands. §4.9 part 2's "command in / events out" has no engine-side
//   caller yet and no acceptance ID names this class as its subject. `RefreshScoreboard`
//   is public precisely so whoever builds that loop can call it after each command
//   without this class growing an opinion about commands.
// - Turn advancement, end-of-turn, the end-of-match screen. Different surfaces with
//   different lifetimes; StratScoreboardWidget.h records why the end-of-match line in
//   particular must not share a lifetime with the persistent panel.
// - Reacting to state changes automatically. There is no change notification to
//   subscribe to, and inventing a tick-driven poll would make the scoreboard's update
//   rate a frame-rate property instead of a turn property.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Templates/SubclassOf.h"
#include "Templates/UniquePtr.h"

#include "StratScoreboardHUD.generated.h"

// Forward declarations only. See the header block above for why the first of these may
// never become an include in this file.
class FStratBridge;
class UDataTable;
class UStratScoreboardWidget;

/**
 * Owns a live `FStratBridge`, seeds it from the shipped scenario, and puts §2.11.4's
 * scoreboard on screen.
 *
 * NOT `Abstract`, unlike UStratScoreboardWidget, and the difference is deliberate. The
 * widget is abstract because a WBP_ asset must supply its layout and a bare C++ widget
 * would draw nothing. This class is spawned by GameMode's `HUDClass`, and an abstract
 * HUD class turns a misconfiguration into a spawn failure reported by engine code, in
 * engine words, about a class name -- which is strictly worse than the named
 * `LogStratUI` refusal this class produces when its properties are unset. It stays
 * concrete so that misconfiguring it fails LOUDLY IN OUR OWN VOICE rather than early in
 * someone else's.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Strat Scoreboard HUD"))
class STRATUI_API AStratScoreboardHUD : public AHUD
{
	GENERATED_BODY()

public:
	// Declared, defined in the .cpp. Not `= default` here, and not omitted: the
	// TUniquePtr member below holds an incomplete type, and its deleter is
	// instantiated wherever the destructor body lands. See the header block.
	AStratScoreboardHUD();
	virtual ~AStratScoreboardHUD() override;

	/**
	 * Rebuilds the scoreboard from the live bridge.
	 *
	 * BlueprintCallable because the §4.10 command loop, whenever it arrives, must be
	 * able to call this after every accepted command without needing a C++ dependency
	 * on this module. It is safe to call at any time and on any frame: it is a read of
	 * the authoritative state through the bridge's projection, and it mutates nothing
	 * but the widget's model.
	 *
	 * On failure it returns false with the bridge's own reason and LEAVES THE EXISTING
	 * MODEL ALONE -- the widget's `Refresh` is documented to do the same, so a transient
	 * refusal cannot blank a scoreboard that was already showing correct standings.
	 *
	 * @param OutFailureReason  the refusing layer's own words, forwarded unchanged.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Scoreboard")
	bool RefreshScoreboard(FString& OutFailureReason);

	/**
	 * True when there is a seeded bridge AND a widget on screen -- i.e. when a refresh
	 * has something to project and somewhere to put it.
	 *
	 * Exposed so a caller can branch on "no scoreboard this session" without having to
	 * treat a false return from RefreshScoreboard as if it were the same thing; a
	 * refused refresh over a live scoreboard and a HUD that never came up are different
	 * situations with different fixes.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Scoreboard")
	bool IsScoreboardLive() const;

	/**
	 * The bridge this HUD created, or null if setup refused.
	 *
	 * NOT A UFUNCTION and it cannot be: `FStratBridge` is not a reflected type, and
	 * making it one would mean putting the vendored headers in front of UHT. C++ within
	 * this module (and, later, whichever object takes ownership of the bridge away from
	 * this one) reaches the rules through here.
	 *
	 * BORROWED, NEVER STORED. `MakeUiWorld` hands out pointers into bridge-owned memory
	 * and the bridge's own header warns that world must not outlive it; the same applies
	 * one level up to this pointer, which dies with the HUD and therefore with the map.
	 */
	FStratBridge* GetBridge() const { return Bridge.Get(); }

	/** The scoreboard widget, or null when setup refused. Read-only on purpose: the HUD
	 *  creates and owns it, and a second creator is a second lifetime to reason about. */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Scoreboard")
	TObjectPtr<UStratScoreboardWidget> Scoreboard;

	/**
	 * Why there is no scoreboard, when there is none. Empty on success.
	 *
	 * Kept as state rather than only logged, because a log line is gone by the time
	 * anyone asks the question in-editor, and because a debug overlay that wants to say
	 * "scoreboard unavailable: <reason>" must not have to re-derive the reason by
	 * re-running the failing load.
	 */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Scoreboard")
	FString LastFailureReason;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// ---- Configuration: everything the scenario or the tables supply --------
	// All EditDefaultsOnly, so they live on a Blueprint subclass of this class and not
	// in C++. See the header block on why no asset path may be written here.

	/**
	 * The §4.8 definition tables, in the shape `FStratBridge::LoadDefinitions` takes.
	 *
	 * HARD REFERENCES, not TSoftObjectPtr, and the reason is that a soft pointer here
	 * would buy nothing. Both tables are needed unconditionally on BeginPlay, so the
	 * "defer the load" property a soft pointer exists for never applies; all it would
	 * add is a LoadSynchronous call and a second null case to handle.
	 *
	 * ROW ORDER IS LOAD-BEARING and is not this class's business: `defIndex` indexes the
	 * vectors the bridge builds from these tables, and a §4.10 Build command carries a
	 * defIndex. The bridge's header records that the order is taken from the table and
	 * then ASSERTED equal to the headless loader's by the editor pass. Pointing these
	 * properties at a different asset than the one that pass checks is therefore a way
	 * to make a log resolve to different units, silently -- which is exactly why the
	 * assertion lives in a gate and not in a comment.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Data")
	TObjectPtr<UDataTable> UnitTable;

	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Data")
	TObjectPtr<UDataTable> TerrainTable;

	/**
	 * The scenario file to seed from.
	 *
	 * A PATH RELATIVE TO THE PROJECT DIRECTORY (e.g. `Data/ferrum_crossing.json`);
	 * resolved in the .cpp against `FPaths::ProjectDir()`, and used as-is if absolute.
	 * The subdirectory is part of the configured string rather than being pasted on in
	 * C++, because "Data" is as much a layout decision as the file name is, and half a
	 * path in C++ is the version of a hardcoded path that is easy to miss.
	 *
	 * `FString` rather than `FFilePath` -- a file picker would be nicer in the details
	 * panel, but the shipped scenario lives outside Content/ and the picker's
	 * relative-to-game-dir handling is one more thing between what is typed and what is
	 * opened. When this grows into a scenario-selection surface it should become an
	 * asset, not a prettier string.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Data")
	FString ScenarioFile;

	/**
	 * Which side moves first, passed straight to `LoadScenarioFromFile`.
	 *
	 * CONFIGURED RATHER THAN ASSUMED because Replay.h states that no rule in this
	 * project decides it. It is a harness pin, and pinning it in C++ would make this
	 * file a silent second author of something the rules module deliberately declined to
	 * decide. Source/StratUI/Tests/ pins its own copy for the same stated reason; if the
	 * two disagree, the running game and the gate are looking at different openings,
	 * which is a thing a reader can now see rather than a thing baked into a constant.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Data")
	int32 FirstSide = 0;

	/**
	 * Which `strat` side is the scoreboard's "YOU" column.
	 *
	 * NOT `sideToMove`. The snapshot carries whose turn it is; this is whose screen this
	 * is, and StratScoreboardWidget.h is explicit that the two differ every other turn.
	 * Range is validated in the .cpp, where SIDE_COUNT is reachable -- this header
	 * cannot name it without the forbidden include, which is the same sentence the
	 * widget's model struct carries about the same field.
	 *
	 * Separate from `FirstSide` even though a hot-seat build will usually set them
	 * equal: one is a rule the rules module declined to make, the other is a camera.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Scoreboard")
	int32 ViewingSide = 0;

	/**
	 * The WBP_ asset deriving from UStratScoreboardWidget.
	 *
	 * A `TSubclassOf` set on a Blueprint, never a ConstructorHelpers path literal: the
	 * literal would put `/Game/...` back into C++ and make the asset unmovable, which is
	 * the trap StratUI.Build.cs already records for the reverse direction (the asset
	 * baking `/Script/StratUI.StratScoreboardWidget` into itself).
	 *
	 * Unset is a legitimate configuration and not an error to be asserted on -- a level
	 * that wants the bridge seeded but no panel on screen leaves it empty, and the .cpp
	 * says so in the log rather than refusing to finish setup.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Scoreboard")
	TSubclassOf<UStratScoreboardWidget> ScoreboardWidgetClass;

	/** Viewport Z-order for the scoreboard. Presentation, not game data; exposed only so
	 *  a later overlay can be layered without touching this class. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Scoreboard")
	int32 ScoreboardZOrder = 0;

	// ---- Setup steps, split so a failure names the step that refused --------
	// Each returns false with the refusing layer's own reason rather than logging and
	// swallowing it, so BeginPlay can record one reason in `LastFailureReason` and the
	// caller of RefreshScoreboard can be told the same thing later.

	/** Constructs the bridge, maps the two tables in, seeds the scenario. */
	bool SeedBridge(FString& OutFailureReason);

	/** Creates the configured widget and adds it to the viewport. Does NOT refresh it --
	 *  BeginPlay refreshes once afterwards, so that "created" and "has data" fail
	 *  separately and a reader of the log can tell which one went wrong. */
	bool CreateScoreboardWidget(FString& OutFailureReason);

private:
	/**
	 * The authoritative `strat::GameState`, one level of indirection down.
	 *
	 * NOT A UPROPERTY -- `FStratBridge` is not a reflected type and must not become one.
	 * Held by TUniquePtr rather than by value for exactly that reason: a by-value member
	 * would require the full definition here, and the full definition is the include
	 * this whole file is arranged to refuse.
	 *
	 * The widget holds no pointer to this. `Refresh` takes the bridge by reference per
	 * call and copies engine-typed values out of the snapshot, so teardown order between
	 * the two is not something anyone has to get right.
	 */
	TUniquePtr<FStratBridge> Bridge;
};
