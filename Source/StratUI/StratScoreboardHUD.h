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
// THAT DEBT IS NOW HALF PAID, and the half that is paid is the seam rather than the
// move. `AdoptBridge` lets a foreign owner -- phase 3's `UStratMatchSubsystem`, per
// `Tools/architect/state.md` -- hand this HUD a bridge it did not create, and the
// ownership question is answered by WHICH MEMBER IS SET rather than by a flag anyone
// has to maintain:
//
//   `OwnedBridge`   a TPimplPtr, non-null exactly when this HUD allocated the bridge.
//                   EndPlay resets it, which frees it.
//   `AdoptedBridge` a raw observing pointer, non-null exactly when the bridge belongs to
//                   somebody else. EndPlay clears the pointer and frees NOTHING.
//
// AT MOST ONE IS EVER NON-NULL. `AdoptBridge` releases the owned one before taking the
// borrowed one, so there is no state in which this class both owns and borrows, and
// therefore no state in which "who frees it" has two answers. `GetBridge()` prefers the
// adopted pointer and is the ONLY reader of either member, so every path through this
// class -- refresh, liveness, teardown -- sees the same bridge without knowing which
// kind it is. Two members rather than one pointer plus a `bOwns` bool because a bool can
// disagree with the pointer beside it and a null member cannot disagree with itself; the
// cost is one pointer on an actor that exists once per player.
//
// THE OWNED PATH IS NOT REPLACED AND MUST NOT BE. A map whose GameMode names this HUD
// class and nothing else still seeds its own bridge on BeginPlay and still frees it, and
// the Automation pass in Source/StratUI/Tests/ still constructs a bare `FStratBridge`
// and hands it to the builder. Adoption is a second path beside the first, so the debt
// above is discharged the day the subsystem lands rather than the day this HUD stops
// compiling.
//
// WHY THIS HEADER MUST NEVER INCLUDE StratBridge.h. It declares a UCLASS, so UHT
// parses it. `StratBridge.h` includes the vendored `strat` headers (Data.h, Ui.h, ...)
// and its own comment records why that is safe THERE: it "declares no reflected types
// on purpose, so the vendored `strat` headers it includes are never put in front of
// UHT". Including it here would put those headers in front of the header tool and
// destroy the property that file was written to hold. StratScoreboardWidget.h refuses
// the same include for the same reason, and StratUI.Build.cs and StratUI.h both record
// the constraint from the module side. `FStratBridge` is therefore forward declared,
// held as `TPimplPtr<FStratBridge>`, and included ONLY from the .cpp.
//
// WHY TPimplPtr AND NOT TUniquePtr, which is what this class held first and which does
// not work here. `TUniquePtr` instantiates its deleter wherever the owning destructor
// is instantiated -- and declaring this class's own constructor and destructor
// out-of-line is NOT sufficient, because UHT emits a vtable-helper constructor
// (`DEFINE_VTABLE_PTR_HELPER_CTOR`) into `Module.StratUI.gen.cpp`, a translation unit
// that includes this header and never includes StratBridge.h. That generated file is
// where the deleter got instantiated against the incomplete type. MEASURED: error
// C4150, "deletion of pointer to incomplete type 'FStratBridge'", reported from
// UniquePtr.h and compiled from the .gen.cpp, not from anything written by hand.
//
// `TPimplPtr` is UE's own answer to exactly this: its header says the deleter is
// "determined at binding time and type-erased, allowing the object to be deleted
// without access to the definition of the type". The type erasure happens at the
// `MakePimpl` call site in the .cpp, where the definition IS available, so no
// translation unit that merely sees this header ever needs it.
//
// The constructor and destructor remain declared here and defined in the .cpp. They are
// no longer load-bearing for the deleter, and they are kept because the constructor has
// a body worth reading and a matching pair is easier to keep honest than a lone one.
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
//
// ---------------------------------------------------------------------------
// A DEBT TAKEN ON KNOWINGLY: THIS CLASS NOW HOSTS A SURFACE ITS NAME DOES NOT COVER.
//
// It also creates and owns GDD Sec 2.11.6's guided-opening strip -- `GuidanceWidgetClass`,
// `GuidanceStrip`, `CreateGuidanceWidget`, `PushGuidance`, `DeliverLatestGuidance`,
// `LastPushedGuidance` and `bGuidanceEverPushed` below. That is a second surface on a class
// called `AStratScoreboardHUD`, and the name is now narrower than the job.
//
// WHY IT IS HERE ANYWAY, and the alternative that was measured against.
// The only other candidate owner was `UStratMatchSubsystem`, which is where the push
// comes FROM and which already holds `TSubclassOf` properties for the board and unit
// actors. It was rejected on a module arrow: creating a widget means `CreateWidget` and
// `AddToViewport`, so `StratPlay.Build.cs` would have had to name `UMG`, `Slate` and
// `SlateCore` -- three new dependencies on a module whose own block records that every
// existing arrow is load-bearing and measured. StratUI already declares all three
// (privately) because it is the module that names `UUserWidget`. Widening this class is a
// prose cost; widening that dependency list is a structural one, and the structural cost
// is the one this project's constraints exist to avoid.
// A HUD is also the only object in reach that has both an owning player controller to
// parent a widget to and a lifetime that ends with the map -- the same three reasons this
// class is an `AHUD` at all, restated for a second widget. And a player controller has
// exactly one HUD, so "a separate guidance HUD" is not an available shape.
//
// THE CONDITION THAT DISCHARGES IT, stated so it is a debt and not a design: when a
// Sec 2.11 UI-layer owner exists -- an `AStratHudBase` that both panels hang off, or a UI
// subsystem that owns viewport widgets -- the SEVEN guidance members move there UNCHANGED
// and this class goes back to being about the scoreboard. (Four when that sentence was
// written; `DeliverLatestGuidance`, `LastPushedGuidance` and `bGuidanceEverPushed` joined
// them, and the count is restated rather than left to be recounted by whoever performs the
// move.) `PushGuidance` was given a signature that survives that move: it takes a reflected
// struct by const reference and touches no member of this class other than the guidance
// ones, so relocating it is a cut and paste plus one call-site edit in
// `UStratMatchSubsystem::ApplyView`. This is the same shape as the bridge-ownership debt
// recorded above, and it is written down here for the same reason: a debt nobody wrote down
// is indistinguishable from a decision.
//
// AND A THIRD SURFACE HAS NOW JOINED THEM: GDD Sec 2.11.4's END-OF-MATCH SCREEN.
// `MatchResultWidgetClass`, `MatchResultZOrder`, `MatchResultScreen`, `ShowMatchResult`,
// `HideMatchResult`, `IsMatchResultWidgetOpen` and `CreateMatchResultWidget` below. It is
// here for the reason the guidance strip and the production menu are -- the module arrow,
// measured once and restated rather than re-argued: `StratPlay.Build.cs` would have to name
// `UMG`, `Slate` and `SlateCore` for `UStratMatchSubsystem` to create a widget, and StratUI
// already declares all three. THE SAME CONDITION DISCHARGES IT: when a Sec 2.11 UI-layer
// owner exists, these seven move there unchanged alongside the guidance seven, and
// `ShowMatchResult` was given a signature that survives the move -- it touches no member of
// this class except `GetBridge()`, `ViewingSide` and its own six, so relocating it is a cut
// and paste plus one call-site edit in `UStratMatchSubsystem::ConcludeMatchIfEnded`.
//
// ITS LIFETIME IS THE ONE THING THAT IS NOT THE GUIDANCE STRIP'S. The strip is created at
// `BeginPlay` and lives all match; this screen is created ON DEMAND when Sec 2.8 says the
// match is over, on `CreateProductionMenuWidget`'s pattern rather than
// `CreateGuidanceWidget`'s. `StratScoreboardWidget.h` named that difference in advance when
// it refused to host this surface: the end-of-match screen "is a different surface with a
// different lifetime, and its faction-voiced result line is the one place faction voice
// appears". A screen created at `BeginPlay` and merely hidden would be a widget holding a
// verdict for a match that had not produced one.
//
// AND ONE OF THOSE SEVEN IS A CACHE, WHICH THIS CLASS HAD DECLINED TO HOLD. Recorded here
// because the refusal was explicit and the reversal must be too. `CreateGuidanceWidget` used
// to end:
// RETRACTED> "The strip draws its own defaults until the first `ApplyView`."
// That sentence assumed a LATER `ApplyView`, and in five fresh PIE sessions on 2026-08-21
// there was not one: `AStratPlayerController::BeginPlay` reached `ApplyView` with the only
// decorated model of the session before this class's `BeginPlay` had created a strip to
// receive it, every time, and nothing reconciles again without player input. So
// `PushGuidance`'s null check dropped the session's only directive and the strip was never
// seen to draw.
//
// WHAT IS CACHED AND WHAT IS STILL REFUSED. `LastPushedGuidance` holds the last value this
// class WAS HANDED, and `DeliverLatestGuidance` gives it to a strip that appeared after it
// arrived. That is not the refusal the header block above states: this class still never
// ASKS for guidance, still touches no bridge and no view model, and still cannot become a
// second thing that runs matches -- the cache has exactly one writer, `PushGuidance`, and
// its content is decided entirely elsewhere. It is the same act `ApplyView` performs for a
// unit actor it has just spawned: a surface that appears mid-flight is brought up to the
// latest model rather than left blank until something else happens to move.
//
// IT IS NOT A SECOND SOURCE OF TRUTH AND CANNOT DRIFT INTO ONE. Every subsequent push
// overwrites it, so the strip's contents remain a function of the LAST model applied, which
// is the property `UStratMatchSubsystem::ApplyView`'s unconditional push exists to hold. The
// cache changes WHEN a value is delivered, never WHICH value.
//
// WHAT THE TWO SURFACES DO NOT SHARE, deliberately. The scoreboard builds its own model
// FROM THE BRIDGE; the strip is HANDED a value that came from the view model. They have
// different sources on purpose and there is no path below on which one can become the
// other's input -- `PushGuidance` never touches `GetBridge()`.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Templates/SubclassOf.h"
#include "Templates/PimplPtr.h"

// A REAL INCLUDE, WHERE A FORWARD DECLARATION USED TO DO. `LastPushedGuidance` is held BY
// VALUE, so this file now needs `FStratGuidanceView`'s size and UHT needs it too. Nothing
// about the constraint that governs this header is loosened by it: `StratViewModel.h` is
// this module's own reflected header and includes nothing vendored, which is exactly what
// `StratBridge.h` -- still forbidden here, and forever -- is not.
#include "StratViewModel.h"

#include "StratScoreboardHUD.generated.h"

// Forward declarations only. See the header block above for why the first of these may
// never become an include in this file.
class FStratBridge;
class UDataTable;
class UStratGuidanceWidget;
class UStratMatchResultWidget;
class UStratScoreboardWidget;
class UUserWidget;

// `FStratGuidanceView` used to be forward declared here, and the reason is retracted in
// place rather than deleted because it was correct for as long as it held:
// RETRACTED> "A reflected struct, forward declared because it appears only in a
// RETRACTED>  NON-reflected signature (`PushGuidance`). UHT never has to size it here, and
// RETRACTED>  this file stays free of an include it would otherwise carry only to pass a
// RETRACTED>  value straight through."
// It no longer passes straight through: `LastPushedGuidance` holds one. See the include
// above.

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
	// definition of FStratBridge is only available in the .cpp. See the header
	// block for why the member is a TPimplPtr and not a TUniquePtr.
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
	 * Takes a bridge this HUD did not create, and gives up the one it did.
	 *
	 * THE BORROWING SEAM the header block's debt names. Phase 3's `UStratMatchSubsystem`
	 * owns the authoritative `strat::GameState` and hands it here; this HUD then draws a
	 * match it does not own, which is what §4.1's "never own rules" asks of an actor.
	 * After this call `GetBridge()` returns `InBridge`, `EndPlay` frees NOTHING, and any
	 * bridge this HUD had allocated for itself is already destroyed.
	 *
	 * BY REFERENCE AND NOT BY POINTER, so there is no null case and therefore no
	 * "un-adopt" that would leave this HUD with neither bridge. Handing ownership back is
	 * not a supported transition and is not needed by any caller: the subsystem outlives
	 * the HUD in phase 3's arrangement, and a HUD that has adopted keeps drawing until the
	 * world goes away.
	 *
	 * CALL IT BEFORE OR AFTER BeginPlay, both work and they do different things. Called
	 * before (the subsystem's path), `SeedBridge` is skipped entirely and this HUD never
	 * allocates a bridge at all -- the adopted one is already seeded and seeding a second
	 * time would give the map two `strat::GameState`s to disagree over. Called after, the
	 * HUD's own bridge is destroyed here and the caller's takes its place.
	 *
	 * IT DOES NOT REFRESH, deliberately. Refreshes are requested by whoever changed the
	 * state -- the constructor comment on this class records why there is no automatic
	 * poll -- and adopting is a change of source, not of state. A caller that wants the
	 * panel to show the new bridge calls `RefreshScoreboard` next; until it does, the
	 * scoreboard keeps showing the last model it successfully built, which is the same
	 * promise `Refresh` makes about every other refusal.
	 *
	 * REFUSES AN UNSEEDED BRIDGE, in this class's own words rather than deferring to a
	 * refusal on every refresh forever after. `FStratBridge::MakeUiSnapshot` refuses a
	 * bridge with nothing to project precisely so that "not loaded yet" cannot be mistaken
	 * for "a match in which nothing has happened", and adopting one would turn that into a
	 * permanent, unattributed blank panel. Seed first, then hand it over.
	 *
	 * NOT A UFUNCTION and it cannot be: `FStratBridge` is not a reflected type, and making
	 * it one would mean putting the vendored headers in front of UHT. The handover is C++,
	 * between the subsystem and this class.
	 *
	 * @param OutFailureReason  why the bridge was not adopted; empty on success. On a
	 *                          refusal NOTHING changes -- the previous bridge, owned or
	 *                          adopted, is still in place and still whoever's it was.
	 */
	bool AdoptBridge(FStratBridge& InBridge, FString& OutFailureReason);

	/**
	 * Changes which `strat` side the scoreboard is drawn FOR.
	 *
	 * THIS IS WHAT MAKES HOT-SEAT WORK on the existing panel. §2.11.4's two columns are
	 * YOU and ENEMY, and which side is "YOU" is a property of whose screen this is, not of
	 * the match -- `StratScoreboardWidget.h` is explicit that `sideToMove` and the viewing
	 * side differ every other turn. The turn loop calls this at the hand-over and the same
	 * scoreboard reads for the other player.
	 *
	 * IT MUTATES NO GAME STATE, and that is a guarantee rather than an observation: it
	 * touches one `int32` on this actor and then asks the widget to rebuild. It submits no
	 * command, it does not touch the bridge, and it cannot change what the rules module
	 * holds. A view of a match is not a move in it.
	 *
	 * IT REFRESHES WHEN THERE IS A SCOREBOARD TO REFRESH, because a viewing side that has
	 * changed without the panel changing is the same value being wrong on screen -- and
	 * the reconciliation posture this project takes is that presentation is REBUILT from
	 * the model rather than patched towards it. When there is no widget up, the new side
	 * is stored and the next successful refresh uses it.
	 *
	 * ON A FAILED REFRESH THE SIDE HAS STILL CHANGED, and the return value says the
	 * refresh failed rather than that the set did. That ordering is deliberate: the widget
	 * leaves its `Model` untouched on refusal, so the panel still shows the OLD side's
	 * correct standings, and a caller that retried the refresh later would get the new
	 * side without having to re-set it. The alternative -- rolling the side back -- would
	 * mean a hot-seat hand-over silently staying with the previous player.
	 *
	 * @param InViewingSide     which `strat` side is the "YOU" column; NOT `sideToMove`.
	 *                          Range-checked in the .cpp, where SIDE_COUNT is reachable.
	 * @param OutFailureReason  the refusing layer's own words; empty on success.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Scoreboard")
	bool SetViewingSide(int32 InViewingSide, FString& OutFailureReason);

	/**
	 * Which `strat` side the scoreboard is currently drawn FOR.
	 *
	 * A READER AND NOTHING ELSE. It does not clamp, does not refresh, does not touch the
	 * bridge, and cannot fail. `SetViewingSide` remains the only writer, and this exists so
	 * that reading the stored side costs nothing and changes nothing -- a getter that
	 * repaired a value on the way out would make "what is stored" and "what is returned"
	 * two different questions, and the interesting question about this field is precisely
	 * whether a refused `SetViewingSide` left it alone.
	 *
	 * IT CLOSES A COVERAGE HOLE RATHER THAN ADDING A FEATURE. `ViewingSide` is protected,
	 * and the only other window onto it -- the widget's `Model` -- cannot be opened
	 * headless, because `UStratScoreboardWidget` is `Abstract` and needs a WBP asset to
	 * exist at all. So `T-UI-03.SetViewingSideRefusesOutOfRange` could pin the return value
	 * of a refusal but not its defining consequence, that THE STORED SIDE IS UNCHANGED.
	 * With this reader that clause is checkable from an Automation test with no assets.
	 *
	 * IT IS NOT A WINDOW ONTO THE PANEL. On a failed refresh inside `SetViewingSide` the
	 * stored side has already moved while the widget still shows the previous side's
	 * standings -- that divergence is deliberate and documented on `SetViewingSide`, and
	 * this function reports the STORED side, not the drawn one. A caller wanting to know
	 * what is on screen asks the widget's model, not this.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Scoreboard")
	int32 GetViewingSide() const;

	/**
	 * The bridge this HUD is reading, whether it created it or adopted it.
	 *
	 * NOT A UFUNCTION and it cannot be: `FStratBridge` is not a reflected type, and
	 * making it one would mean putting the vendored headers in front of UHT. C++ within
	 * this module (and, later, whichever object takes ownership of the bridge away from
	 * this one) reaches the rules through here.
	 *
	 * BORROWED, NEVER STORED. `MakeUiWorld` hands out pointers into bridge-owned memory
	 * and the bridge's own header warns that world must not outlive it; the same applies
	 * one level up to this pointer. An OWNED bridge dies with the HUD and therefore with
	 * the map; an ADOPTED one outlives it, and a caller holding this pointer past EndPlay
	 * is relying on somebody else's lifetime either way.
	 *
	 * THE ONLY READER OF EITHER MEMBER. Adoption is expressed here and nowhere else, so
	 * no path through this class has to know which kind of bridge it has.
	 */
	FStratBridge* GetBridge() const
	{
		return AdoptedBridge != nullptr ? AdoptedBridge : OwnedBridge.Get();
	}

	/** The scoreboard widget, or null when setup refused. Read-only on purpose: the HUD
	 *  creates and owns it, and a second creator is a second lifetime to reason about. */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Scoreboard")
	TObjectPtr<UStratScoreboardWidget> Scoreboard;

	/**
	 * Hands GDD Sec 2.11.6's guidance projection to the strip, if there is a strip.
	 *
	 * NOT A UFUNCTION, and for a different reason than the ones above it: every argument
	 * here IS reflectable. It is plain C++ so that Blueprint has no way to state a
	 * guidance value. The strip must be readable off `FStratViewModel::Guidance` alone --
	 * see `UStratGuidanceWidget`'s header on why a reflected setter would make the screen
	 * a thing two authors can write.
	 *
	 * VOID AND UNREFUSABLE, matching `UStratMatchSubsystem::ApplyView`, which is its only
	 * caller. Reconciliation is not a request that can be declined; it is the act of
	 * making the screen agree with a value already decided. There is nothing a caller
	 * could do with a failure here.
	 *
	 * NO STRIP IS A NO-OP AND NOT A REFUSAL, and this is the one judgement in the
	 * function. An unset `GuidanceWidgetClass` is a legitimate configuration -- exactly as
	 * `ScoreboardWidgetClass` is, and recorded once at BeginPlay in the same words. A
	 * `bool` return here would have been false on every reconcile of every session that
	 * ships without a strip, which is a warning that means "configured as intended" and
	 * therefore means nothing.
	 */
	void PushGuidance(const FStratGuidanceView& InGuidance);

	/**
	 * Brings the strip up to the last value `PushGuidance` was handed.
	 *
	 * WHAT GAP THIS CLOSES, AND IT IS THE HALF OF THE GUIDANCE PATH THE PUSH COULD NOT REACH.
	 * `PushGuidance` drops a value when there is no strip, and at match start there is not
	 * one yet: measured in five fresh PIE sessions on 2026-08-21, `AStratPlayerController::
	 * BeginPlay` reached `UStratMatchSubsystem::ApplyView` with the session's only decorated
	 * model BEFORE this HUD's `BeginPlay` had run at all, every time. Nothing reconciles again
	 * until the player gives input, so the first directive was never seen. This function is
	 * how a strip created after a push still ends up carrying it.
	 *
	 * NOT A CATCH-UP QUEUE. It replays the LATEST value and never a backlog, which is what
	 * keeps the strip a function of the last model applied rather than of the sequence of
	 * models applied -- the same property `UStratMatchSubsystem::ApplyView`'s unconditional
	 * push holds one level up.
	 *
	 * IDEMPOTENT, AND THAT IS WHY IT CLEARS NO FLAG. Calling it twice delivers the same value
	 * twice, and `UStratGuidanceWidget::PushGuidance` assigns unconditionally, so the second
	 * call changes nothing on screen. A one-shot latch was the alternative and was rejected:
	 * it would make this an event that can be consumed, and "has the pending push been taken"
	 * is exactly the history-shaped question this project keeps out of its presentation path.
	 *
	 * @return true when a value was delivered -- there is a strip AND something has been
	 *         pushed. False distinguishes "no strip" and "nothing pushed yet" from "delivered
	 *         an inactive view", which a caller cannot tell apart from the strip's contents
	 *         because a default-constructed `FStratGuidanceView` is a real, meaningful state.
	 */
	bool DeliverLatestGuidance();

	/** The guided-opening strip, or null when none was configured or setup refused.
	 *  Read-only for the same reason `Scoreboard` is: this HUD creates and owns it, and a
	 *  second creator is a second lifetime to reason about. */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Guidance")
	TObjectPtr<UStratGuidanceWidget> GuidanceStrip;

	/**
	 * The last value `PushGuidance` was handed, whether or not a strip existed to receive it.
	 *
	 * ONE WRITER, `PushGuidance`, AND ONE READER, `DeliverLatestGuidance`. Nothing else in
	 * this class consults it and nothing branches on it -- in particular `PushGuidance` does
	 * NOT compare against it to skip a forward, which would reintroduce the delta-shaped
	 * thinking `UStratMatchSubsystem::ApplyView` refuses at the other end of the same call.
	 *
	 * NOT A `UPROPERTY`, and deliberately not `BlueprintReadOnly`. `FStratGuidanceView` holds
	 * no object references, so there is nothing here for the garbage collector to keep alive;
	 * and publishing it would give a Widget Blueprint a SECOND place to read guidance from,
	 * one whose freshness depends on when it asked. `UStratGuidanceWidget::Guidance` is the
	 * one a WBP binds to and this is not a rival for it.
	 */
	FStratGuidanceView LastPushedGuidance;

	/**
	 * Whether `PushGuidance` has ever run. NOT whether guidance is active.
	 *
	 * A SEPARATE BOOL BECAUSE THE VALUE CANNOT SIGNAL ITS OWN ABSENCE. A default-constructed
	 * `FStratGuidanceView` is a REAL state -- `FStratGuidanceView`'s own block records three
	 * distinct causes of `bActive` false that the strip is not allowed to tell apart -- so
	 * "the cache equals the default" cannot mean "nothing has been cached". Without this,
	 * `DeliverLatestGuidance` on a fresh HUD would fire `OnGuidanceRefreshed` at a Blueprint
	 * to announce a refresh that never happened. This project has already paid once for
	 * treating a real default as an unset marker, in `FStratMatchConfig::SaveSlotName`.
	 *
	 * IT NEVER GOES BACK TO FALSE. There is no un-push, and clearing it on delivery would
	 * turn the cache into a queue -- see `DeliverLatestGuidance` on why it is not one.
	 */
	bool bGuidanceEverPushed = false;

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

	// ---- §2.11.5's production menu -----------------------------------------
	// A THIRD SURFACE ON A CLASS CALLED `AStratScoreboardHUD`, AND IT JOINS THE DEBT
	// RECORDED ABOVE RATHER THAN OPENING A NEW ONE. The guidance block already states why
	// widget creation lives here: `CreateWidget` and `AddToViewport` mean `UMG`, `Slate`
	// and `SlateCore`, and `StratPlay.Build.cs` would have to grow all three to create a
	// widget itself -- a structural cost where widening this class is a prose cost. That
	// argument is unchanged for this widget, so the SEVEN guidance members become ELEVEN,
	// and the same condition discharges all of them together: when a §2.11 UI-layer owner
	// exists, `ProductionMenuWidgetClass`, `ProductionMenuZOrder`, `ProductionMenu`,
	// `OpenProductionMenuWidget`, `CloseProductionMenuWidget`, `IsProductionMenuWidgetOpen`
	// and `CreateProductionMenuWidget` move there unchanged with the other seven.
	//
	// WHAT IS DIFFERENT FROM THE OTHER TWO WIDGETS, AND IT IS THE ONLY DIFFERENCE: this one
	// is created ON DEMAND and destroyed on close, where the scoreboard and the strip are
	// created once in `BeginPlay` and live for the map. §2.11.5's menu is modal in the
	// player's attention -- it is about ONE factory, chosen at the moment the player asks
	// for it -- and a permanently-resident panel would have to carry "no factory" as a
	// drawable state that nothing in the rules produces. `BeginPlay` still reports an unset
	// `ProductionMenuWidgetClass` once, exactly as it reports an unset `GuidanceWidgetClass`,
	// so "no menu was configured" and "the menu failed to open" stay distinguishable.
	//
	// THIS CLASS ASKS THE RULES MODULE NOTHING ABOUT THE MENU AND HOLDS NO ROW. The rows
	// live on `UStratMatchSubsystem::ProductionMenu`, written by `RefreshProductionMenu`
	// and by nothing else; this class creates a widget and takes it down again. In
	// particular nothing here calls the WBP's `RefreshMenu` -- that is a Blueprint custom
	// event, the widget refreshes itself from its own `Construct`, and a C++ caller would
	// be a second author of when the menu's contents are decided.

	/**
	 * Puts §2.11.5's production menu on screen, if a class is configured and one is not
	 * already up.
	 *
	 * IT DOES NOT SAY WHICH FACTORY, and that is deliberate rather than an omission. The
	 * hex is `AStratPlayerController`'s answer (`GetProductionTargetHex`) and the rows are
	 * `UStratMatchSubsystem`'s; a hex parameter here would make this class a third party to
	 * a pairing it has no way to check, and `UStratMatchSubsystem::SubmitProductionChoice`
	 * exists in the shape it does precisely to remove that pairing rather than check it.
	 *
	 * ALREADY-OPEN IS A REFUSAL AND NOT A NO-OP. Two menus over one board is a screen with
	 * two answers on it, and the caller -- a toggle -- needs to be able to tell that it
	 * asked for something that did not happen.
	 *
	 * @return true when a menu is on the viewport because of this call.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Production")
	bool OpenProductionMenuWidget(FString& OutFailureReason);

	/**
	 * Takes the menu back down. Safe to call with none up.
	 *
	 * VOID AND UNREFUSABLE, matching `PushGuidance`: there is nothing a caller could do
	 * with a failure to close something that is already closed.
	 *
	 * IT DOES NOT CLEAR `UStratMatchSubsystem::ProductionMenu`. Removing the panel and
	 * clearing the rows are two acts on two objects, and this class must not reach into the
	 * subsystem -- see the header block on why it never becomes a second thing that runs
	 * matches. The caller closes both, in that order; `AStratPlayerController` is the one
	 * caller and does exactly that.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Production")
	void CloseProductionMenuWidget();

	/**
	 * Whether a production menu is on the viewport.
	 *
	 * IT READS THE WIDGET AND IS NOT A BOOL BESIDE IT, on the same rule
	 * `UStratMatchSubsystem::IsProductionMenuOpen` states about its own rows: a mirror can
	 * disagree with the thing it mirrors. A WBP that removes itself -- a Cancel button in
	 * the asset, which is a shape the widget is free to have -- leaves `ProductionMenu`
	 * non-null and NOT in the viewport, and this answers false for it. A cached bool would
	 * have answered true and the toggle would have needed two presses.
	 *
	 * OUT OF LINE AND NOT INLINE, AND THAT IS A MODULE ARROW RATHER THAN A STYLE CHOICE.
	 * The body calls `UUserWidget::IsInViewport()`, which needs UMG's definition; inlining
	 * it here would push `UMG` onto every module that includes this header, and
	 * `StratPlay.Build.cs`'s dependency list is measured line by line. `UUserWidget` is
	 * forward declared in this file and stays that way.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Production")
	bool IsProductionMenuWidgetOpen() const;

	/** The production menu, or null when none is up. Read-only for the reason `Scoreboard`
	 *  and `GuidanceStrip` are: this HUD creates and owns it, and a second creator is a
	 *  second lifetime to reason about. */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Production")
	TObjectPtr<UUserWidget> ProductionMenu;

	/**
	 * GDD Sec 2.11.4: put the end-of-match screen up, built from the live bridge.
	 *
	 * ITS CALLER IS `UStratMatchSubsystem::ConcludeMatchIfEnded` AND ONCE PER MATCH IS THAT
	 * FUNCTION'S PROPERTY, NOT THIS ONE'S. That function latches on
	 * `bMatchConclusionAnnounced`, which `StartMatchInternal` clears, so a restart shows the
	 * next match's screen and the same match cannot show two. THIS function is idempotent
	 * rather than latched: called twice it replaces the widget and rebuilds the model, which
	 * is what a console-driven or gate-driven caller needs and what a latch here would have
	 * taken away. A second latch would also be a second authority on "has this match
	 * concluded", and the subsystem's block on `bMatchConclusionAnnounced` is explicit that
	 * every question about that is answered by asking the model.
	 *
	 * BLUEPRINTCALLABLE so the screen is reachable without a C++ dependency on this module --
	 * `RefreshScoreboard`'s reasoning. A verb with no caller reads as built and is not; this
	 * one has a C++ caller AND a reflected route.
	 *
	 * REFUSES RATHER THAN SHOWING AN EMPTY VERDICT, forwarding the refusing layer's own words
	 * unchanged: no bridge, no `MatchResultWidgetClass`, or a build the projection declined.
	 * IT DOES NOT CHECK WHETHER THE MATCH HAS ENDED and must not -- `StratBuildMatchResultModel`
	 * does not either, and this class asking would be a HUD forming an opinion about Sec 2.8.
	 * A caller that shows this screen mid-match gets a screen with an empty tier and an empty
	 * line, which is the honest rendering of the answer it asked for.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Result")
	bool ShowMatchResult(FString& OutFailureReason);

	/** Takes the end-of-match screen down. `CloseProductionMenuWidget`'s shape exactly,
	 *  including the absence of an `IsInViewport` guard and the silence towards
	 *  `UStratMatchSubsystem` -- a HUD that cleared somebody elses match state would be the
	 *  first line of this class becoming a thing that runs matches. A no-op when none is up. */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Result")
	void HideMatchResult();

	/** True when the end-of-match screen exists AND is in the viewport, for the reason
	 *  `IsProductionMenuWidgetOpen` gives about its own pair: a widget that removed itself
	 *  leaves the pointer non-null, and a cached bool could disagree with the screen. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Result")
	bool IsMatchResultWidgetOpen() const;

	/** The end-of-match screen, or null when none is up. Read-only for the reason
	 *  `Scoreboard`, `GuidanceStrip` and `ProductionMenu` are: this HUD creates and owns it,
	 *  and a second creator is a second lifetime to reason about. */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Result")
	TObjectPtr<UStratMatchResultWidget> MatchResultScreen;

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

	/**
	 * The WBP_ asset deriving from UStratGuidanceWidget -- `WBP_DirectiveStrip`.
	 *
	 * A `TSubclassOf` set on a Blueprint default, never a ConstructorHelpers path literal,
	 * for the reason the scoreboard's copy of this property gives.
	 *
	 * UNSET IS A LEGITIMATE CONFIGURATION AND NOT AN ERROR. A session that wants the match
	 * to run with no guided opening on screen leaves it empty; the .cpp says so in the log
	 * at Log level and finishes setup. Note what this does NOT do: leaving it unset does
	 * not stop `FStratGuidedOpening` advancing beats, because the beat machine is not a
	 * property of the screen. It only means nothing draws the directive.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Guidance")
	TSubclassOf<UStratGuidanceWidget> GuidanceWidgetClass;

	/** Viewport Z-order for the guidance strip. Defaults ABOVE `ScoreboardZOrder` because
	 *  Sec 2.11.6-B's strip is a foreground instruction and the scoreboard is persistent
	 *  chrome; both remain properties so the layering is a designer's call, not this
	 *  file's. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Guidance")
	int32 GuidanceZOrder = 10;

	/**
	 * The WBP_ asset for §2.11.5's production menu -- `WBP_ProductionMenu`.
	 *
	 * A `TSubclassOf` set on a Blueprint default, never a `ConstructorHelpers` path
	 * literal, for the reason the scoreboard's copy of this property gives.
	 *
	 * `UUserWidget` AND NOT A C++ BASE OF OURS, AND THAT IS THE IRREVERSIBLE PART OF THIS
	 * CHANGE HELD OPEN RATHER THAN SPENT. A Blueprint deriving from a C++ class bakes
	 * `/Script/Module.Class` into itself permanently, so typing this property as, say,
	 * `UStratProductionMenuWidget` would decide which module owns the menu widget before
	 * anybody has needed to decide it -- the same call `engine.md` records the reflected
	 * seam declining to make. Typed as the engine's own base, the asset commits to nothing
	 * of ours and this property narrows for free on the day a base class exists.
	 * THE CONDITION THAT DISCHARGES IT: somebody needing C++-side layout or a
	 * `BlueprintImplementableEvent` hook on the menu, which no caller needs today.
	 *
	 * THE COST OF THAT, STATED: this class cannot call anything on the widget beyond
	 * `UUserWidget`'s own surface, so it cannot refresh it. It does not want to -- see the
	 * block on `OpenProductionMenuWidget` -- but a future caller that does will have to
	 * pay for a base class first, and should read this paragraph before assuming it is free.
	 *
	 * UNSET IS A LEGITIMATE CONFIGURATION AND NOT AN ERROR, exactly as `GuidanceWidgetClass`
	 * is: a session that wants the match playable with no production surface leaves it
	 * empty, and `BeginPlay` says so once at Log rather than refusing to finish setup. What
	 * it does NOT mean is that production is unreachable -- `UStratMatchSubsystem`'s three
	 * `Stratocracy|Production` entry points are reflected and a console or a gate can drive
	 * them with no widget in existence.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Production")
	TSubclassOf<UUserWidget> ProductionMenuWidgetClass;

	/** Viewport Z-order for the production menu. Defaults ABOVE `GuidanceZOrder` because
	 *  §2.11.5's menu is the thing the player is looking at while it is up, and both the
	 *  scoreboard and the directive strip are chrome behind it. A property rather than a
	 *  literal so the layering stays a designer's call. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Production")
	int32 ProductionMenuZOrder = 20;

	/**
	 * The WBP_ asset deriving from `UStratMatchResultWidget` -- Sec 2.11.4's end-of-match
	 * screen.
	 *
	 * A `TSubclassOf` set on a Blueprint default, never a `ConstructorHelpers` path literal,
	 * for the reason the scoreboard's copy of this property gives.
	 *
	 * TYPED AS OUR OWN C++ BASE AND NOT AS `UUserWidget`, which is where it parts company with
	 * `ProductionMenuWidgetClass` above -- and the parting is deliberate rather than
	 * inconsistent. That property is a bare `UUserWidget` because nothing needs to call
	 * anything on the menu, so the irreversible `/Script/Module.Class` bake was held open. This
	 * one MUST be called: `ShowMatchResult` pushes a finished `FStratMatchResultModel` into it,
	 * and Sec 2.11.4's verdict is the one value in this project that must have exactly one
	 * author -- a widget Blueprint free to compose its own would be a second one. The bake is
	 * paid for here on purpose and `StratMatchResultWidget.h` records that it is irreversible.
	 *
	 * UNSET IS A LEGITIMATE CONFIGURATION AND NOT AN ERROR, exactly as `GuidanceWidgetClass`
	 * and `ProductionMenuWidgetClass` are: a session that wants the match playable with no
	 * verdict surface leaves it empty, and `BeginPlay` says so once at Log rather than refusing
	 * to finish setup. What it does NOT mean is that the result is unreachable --
	 * `UStratMatchSubsystem::GetMatchResult` is reflected and a console or a gate can read the
	 * winner with no widget in existence, and `ConcludeMatchIfEnded` still logs `STRAT-MATCH
	 * concluded` either way.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Result")
	TSubclassOf<UStratMatchResultWidget> MatchResultWidgetClass;

	/** Viewport Z-order for the end-of-match screen. Defaults ABOVE `ProductionMenuZOrder`
	 *  because Sec 2.11.4s verdict is the last thing drawn and everything else -- the panel,
	 *  the strip, any menu left open when the flag fell -- is behind it. A property rather than
	 *  a literal so the layering stays a designer's call. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Result")
	int32 MatchResultZOrder = 30;

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

	/** Creates the configured guidance strip and adds it to the viewport. Does NOT push a
	 *  value into it: there is no view model to push until a match is running, and this
	 *  HUD is not the thing that runs one. It stays showing whatever its own defaults draw
	 *  until `UStratMatchSubsystem::ApplyView` reconciles it for the first time. */
	bool CreateGuidanceWidget(FString& OutFailureReason);

	/** Creates the configured production menu and adds it to the viewport. Does NOT refresh
	 *  it and CANNOT: `RefreshMenu` is a Blueprint custom event on the WBP and this class
	 *  holds the widget as a bare `UUserWidget`. The asset refreshes itself from its own
	 *  `Construct`, reading `AStratPlayerController::GetProductionTargetHex`. */
	bool CreateProductionMenuWidget(FString& OutFailureReason);

	/** Creates the configured end-of-match screen and adds it to the viewport. Does NOT push a
	 *  model into it -- `ShowMatchResult` builds one and pushes it immediately afterwards, so
	 *  that "the screen could not be created" and "the verdict could not be projected" stay
	 *  separate refusals with separate sentences, which is `CreateScoreboardWidget`'s stated
	 *  reason for the same split. */
	bool CreateMatchResultWidget(FString& OutFailureReason);

private:
	/**
	 * The authoritative `strat::GameState`, one level of indirection down, WHEN THIS HUD
	 * IS THE OWNER. Null once a bridge has been adopted, and null before setup runs.
	 *
	 * NOT A UPROPERTY -- `FStratBridge` is not a reflected type and must not become one.
	 * Held by TPimplPtr rather than by value for exactly that reason: a by-value member
	 * would require the full definition here, and the full definition is the include
	 * this whole file is arranged to refuse. And by TPimplPtr rather than TUniquePtr for
	 * the C4150 the header block measures.
	 *
	 * THIS MEMBER IS WHAT FREES THE BRIDGE, and it is the only thing in the project that
	 * frees one this HUD allocated. `EndPlay` resets it; `AdoptBridge` resets it before
	 * taking a foreign bridge, because a HUD that kept its own alive underneath an adopted
	 * one would be leaking a whole `strat::GameState` per level load with nothing to point
	 * at.
	 *
	 * The widget holds no pointer to this. `Refresh` takes the bridge by reference per
	 * call and copies engine-typed values out of the snapshot, so teardown order between
	 * the two is not something anyone has to get right.
	 */
	TPimplPtr<FStratBridge> OwnedBridge;

	/**
	 * A bridge belonging to somebody else, or null. Set only by `AdoptBridge`.
	 *
	 * RAW, AND RAW IS THE POINT. Every smart pointer available here would claim some share
	 * of a lifetime this class does not have any share of. `EndPlay` sets it to null and
	 * DESTROYS NOTHING; if the owner outlives this HUD, which is the arrangement phase 3's
	 * `UStratMatchSubsystem` has, that is correct, and if it does not, the owner tore down
	 * a bridge while an actor was still drawing from it and the fix is in the owner.
	 *
	 * NOT A UPROPERTY -- `FStratBridge` is not a UObject, so there is nothing here for the
	 * garbage collector to keep alive and nothing it could null out. The lifetime is C++'s
	 * and is stated in `AdoptBridge`'s contract.
	 *
	 * NEVER NON-NULL AT THE SAME TIME AS `OwnedBridge`. See the header block; `GetBridge()`
	 * would still be well-defined if it were, but "who frees it" would not be.
	 */
	FStratBridge* AdoptedBridge = nullptr;
};
