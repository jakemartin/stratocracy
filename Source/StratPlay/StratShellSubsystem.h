// GDD §2.11.5 / T-TURN-09 / GATE-TITLEMENU -- the shell. The object that gives the
// four-screen prototype a way to move BETWEEN its screens, and the first code in this
// project that opens a level.
//
// WHAT GAP THIS CLOSES. A packaged build boots into `Lvl_FerrumCrossing` and stays there:
// `grep -rn "OpenLevel" Source/` returned nothing before this file, and `GameDefaultMap` in
// `Config/DefaultEngine.ini` is the only thing that has ever chosen a level. So every
// capability the match already has -- `LoadMatchFromSlot`, a §2.8 result the result screen
// can draw, a scenario that re-seeds deterministically -- is reachable exactly once per
// process launch and never again. This file is the seam a title screen binds to.
//
// WHAT THE GDD ACTUALLY SAYS, MEASURED RATHER THAN PARAPHRASED, BECAUSE IT SAYS MUCH LESS
// THAN A READER WILL EXPECT. §2.11.5's last paragraph is the ONLY place the title screen
// appears in the whole document: "That is the complete screen list for the prototype:
// title/menu, briefing, match, result. No settings screen beyond volume + resolution is
// budgeted (Enhanced Input remap is a polish item)." Case-insensitive counts over
// `Stratocracy_Prototype_GDD.md`: `title/menu` 1, `title screen` 0, `main menu` 0, `Quit` 0,
// `Rematch` 0, `Restart` 0, `Play Again` 0, `Return to` 0. §2.11.8's must-have build ranking
// does not list this screen at all.
//   THE CONSEQUENCE, SAID PLAINLY: the route set below is OURS and is not the GDD's.
// Nothing here is derived from a design sentence, because there is no design sentence. The
// four routes are derived from CAPABILITIES THAT ALREADY EXIST IN THIS TREE and would
// otherwise stay unreachable -- level travel to a seeded match (`AStratGameMode::BeginPlay`),
// `UStratMatchSubsystem::LoadMatchFromSlot`, and process exit. This is the same species of
// declaration `StratMatchResultWidget.h` makes about its faction binding: a decision the
// design declined to make, made here so a screen can be drawn, and labelled so a reader can
// see whose decision it was. IF §2.11 EVER SPECIFIES THE MENU, THIS ENUM DEFERS TO IT.
//
// THE DECISION IS SEPARATED FROM THE TRAVEL, AND THAT SPLIT IS THE POINT OF THE FILE.
// No automation clause in this project has a viewport, a cursor or a real level travel, so a
// route implemented as a bare `UGameplayStatics::OpenLevel` inside a widget method would be
// untestable by construction -- and this project already carries "a correct mapping can be
// untestable" as a named failure. Everything that DECIDES is a `static` on this class and
// touches no world. THE DECLARATIONS BELOW ARE THAT LIST AND THIS PARAGRAPH DELIBERATELY NO
// LONGER COPIES IT: it named five statics, a sixth was added, and a second copy of a growing
// member list is a thing that goes stale without any diff showing it. A headless clause
// constructs an `FStratShellFacts` by hand and asserts on the answers.
//
// WHAT IS UNREACHABLE IS THE TRAVELLING PATH OF `ExecuteRoute` AND NOT `ExecuteRoute`. The
// sentence that stood here claimed the wider thing and is corrected rather than deleted,
// because a reader arriving by a citation must be able to see what was claimed:
//   RETRACTED>  "What is left over -- `ExecuteRoute` -- is a permission check it delegates,
//   RETRACTED>   then one engine call, and it is the only part no clause can reach."
// `strat-integration-reviewer` falsified the second half against this tree:
// `Stratocracy.StratPlay.GATE-TITLEMENU.ARefusedRouteArmsNothing` calls `ExecuteRoute` and
// genuinely executes its REFUSAL arm, pinning it with an expected-message declaration at
// exactly one occurrence -- so a refusal that stopped happening, and a second one that
// started, both fail that clause. A clause does reach this function. What no clause reaches
// is the arm on which permission was GRANTED and a level is opened, which is what the
// sentence should have named and now does.
//
// WHY A `UGameInstanceSubsystem`, WHICH `StratScoreboardHUD.h` AND `StratMatchSubsystem.h`
// BOTH REFUSED. Their reason was specific and it does not bind here: a GameInstance
// subsystem outlives the map, and `strat::GameState` is PER-MAP, so one would carry a
// seeded scenario into the next level. THIS OBJECT HOLDS NO RULES STATE AND CANNOT. It holds
// two soft level references, a save-slot name and one pending-load string; nothing it stores
// was produced by the rules module and it never names a `strat::` type. Outliving the map is
// the requirement rather than the hazard -- travel is by definition the operation that
// destroys one world and makes another, and a world subsystem asked to open a level would be
// scheduling its own destruction. The pending-load slot below is the one fact that must
// survive that destruction, which is the whole reason the class has this lifetime.
//
// IT HAS NO BLUEPRINT DEFAULTS AND IS CONFIGURED BY A GameMode, exactly as
// `UStratMatchSubsystem` is. A subsystem has no details panel, so the two level references
// arrive through `ConfigureMatchDestination` / `ConfigureTitleDestination` from a GameMode's
// `EditDefaultsOnly` properties on `BeginPlay`. That is `AStratGameMode`'s existing pattern
// with `FStratMatchConfig` and not a new one.
//
// THE TWO DESTINATIONS ARE HELD BY DIFFERENT GameModes ON PURPOSE, AND THE ALTERNATIVE IS
// NAMED BECAUSE IT LOOKS TIDIER AND IS WORSE. `AStratShellGameMode` (the title map) holds
// `MatchLevel`; `AStratGameMode` (the match map) holds `TitleLevel`. The shape that was
// rejected is one shared `FStratShellLevels` struct on both, which would put the SAME two
// asset references in TWO Blueprint defaults with nothing asserting they agree. Each
// GameMode now holds exactly the one destination it is able to route to, so there is nothing
// to disagree about.
//
// NO `/Game/` LITERAL, AND THE RULE IS SHARPEST HERE. A menu's whole job is to open levels,
// which makes this the single most likely file in the tree to break the rule. Both
// destinations are `TSoftObjectPtr<UWorld>` properties on a Blueprint default;
// `ResolveDestination` never loads one and never inspects its path, so it is decidable in a
// headless clause over a soft pointer built from any string at all.
//
// UNAVAILABLE OPTIONS ARE GREYED AND NAMED, NEVER HIDDEN, and that is §2.11.5's own rule
// applied to a second surface: "Unaffordable rows are greyed with the shortfall named ...
// never hidden -- the price list is also the strategy lesson." `FStratShellOption` therefore
// carries `bEnabled` AND `DisabledReason`, and `BuildMenuModel` returns all four routes in a
// fixed order in every state. A WBP that dropped a row would be contradicting that sentence.
//
// ZERO WIDGET-SIDE ARITHMETIC (T-UI-03's clause, third surface). Every string and every bool
// a menu WBP draws is a field on `FStratShellOption`. In particular `bEnabled` is a FIELD and
// not a `SlotExists AND LevelSet` node in a graph, and the label that reads `Restart Match`
// rather than `New Match` is chosen here rather than by a Select node on `bMatchIsLive`.
//
// NOT IN THIS ROUND, with reasons:
// - A DIFFICULTY-TIER CONTROL. Ruled out by the user on 2026-08-30. `EStratDifficulty` is
//   configured on the match GameMode's defaults and the menu neither reads nor moves it.
// - A SETTINGS SCREEN. §2.11.5 budgets "volume + resolution" and nothing else, and neither
//   has any engine-side surface in this tree to bind to yet. There is no `Settings` route,
//   deliberately, rather than a disabled one -- a greyed row promises a thing that exists.
// - THE BRIEFING SCREEN, which is the second name on §2.11.5's list. It is §2.11.6-A's
//   pre-match overlay, it lives inside the match level rather than beside it, and it is a
//   guidance surface rather than a route. No shell route opens it.
// - ANY WIDGET. This file draws nothing and includes no UMG header. The menu asset is the
//   editor lane's and binds to the reflected surface below, exactly as
//   `StratMatchResultWidget.h` says the result screen's buttons do.
// - LOCALIZATION. `FText::FromString` throughout, matching `StratGuidedOpening::
//   DirectiveTextFor` and the result screen's line chooser.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/SoftObjectPtr.h"

#include "StratShellSubsystem.generated.h"

class UWorld;

/**
 * The routes a menu can drive.
 *
 * FOUR, AND THE GDD NAMES NONE OF THEM -- see the file header for the measurement. Each one
 * exists because a capability already in this tree is otherwise unreachable after boot.
 */
UENUM(BlueprintType)
enum class EStratShellRoute : uint8
{
	/**
	 * Open the match level fresh. The scenario re-seeds from `FStratMatchConfig` and no save
	 * slot is read.
	 *
	 * THIS IS ALSO "RESTART", AND THE SAMENESS IS THE LOAD-BEARING PART. Restarting a live
	 * match is not a second route with a second destination -- it is this one, taken from a
	 * world where `bMatchIsLive` is true. Only the LABEL differs. Two routes here would be
	 * two answers to "what does a fresh match seed from", which is exactly the drift
	 * `T-TURN-09` (same scenario -> identical state) exists to forbid.
	 */
	NewMatch UMETA(DisplayName = "New Match"),

	/**
	 * Open the match level and then restore the configured save slot.
	 *
	 * TRAVEL AND LOAD ARE TWO STEPS AND CANNOT BE ONE. `UStratMatchSubsystem::
	 * LoadMatchFromSlot` needs a seeded match to restore INTO, and that match is seeded by
	 * `AStratGameMode::BeginPlay` in the destination world, which does not exist yet at the
	 * moment this route is chosen. `PendingLoadSlot` is what carries the request across the
	 * travel, and it is why this class outlives the map.
	 */
	ContinueMatch UMETA(DisplayName = "Continue"),

	/** Leave a live match and open the title level. The match world is destroyed with it. */
	ReturnToTitle UMETA(DisplayName = "Return to Title"),

	/** Exit the process. The only route that does not travel. */
	QuitGame UMETA(DisplayName = "Quit")
};

/**
 * Everything a route decision is allowed to depend on.
 *
 * A STRUCT AND NOT FOUR ARGUMENTS, so that a headless clause states an entire world in one
 * initializer and so that adding a fifth fact is a compile error at the deciders rather than
 * a silently ignored parameter. Nothing here is a rules fact: no side, no turn, no Fame. The
 * shell does not know what a match contains, only whether one is running.
 */
USTRUCT(BlueprintType)
struct FStratShellFacts
{
	GENERATED_BODY()

	/** True when a `TSoftObjectPtr<UWorld>` for the match level has been configured. */
	UPROPERTY(BlueprintReadWrite, Category = "Stratocracy|Shell")
	bool bMatchLevelConfigured = false;

	/** True when a `TSoftObjectPtr<UWorld>` for the title level has been configured. */
	UPROPERTY(BlueprintReadWrite, Category = "Stratocracy|Shell")
	bool bTitleLevelConfigured = false;

	/**
	 * True when the configured save slot holds a match that can actually be RESTORED.
	 *
	 * IT WAS CALLED `bSaveSlotExists`, AND THE RENAME IS RECORDED HERE RATHER THAN DONE
	 * QUIETLY. The field stated the weaker fact -- a file is on disk -- while every consumer
	 * read it as the stronger one, that a match can be restored. Continue drew ENABLED for a
	 * player who had never saved, and gave them turn 1.
	 *
	 * WHY THAT HAPPENED IS `UStratMatchSubsystem::IsPayloadRestorable`'s BLOCK AND IS NOT
	 * RETOLD HERE. It is a two-part cause -- one of them a warning already standing in
	 * `StratSaveGame.h` that nobody read -- and this branch has been blocked twice for
	 * carrying a causal story at a second site, once for getting that second copy wrong. This
	 * field records what it is now called and why; that function records what went wrong.
	 *
	 * THE DECISIONS WERE NEVER WRONG -- THE FACT WAS. That is why no clause over
	 * `IsRoutePermitted` or `BuildMenuModel` could have caught it, and why the fix is here and
	 * not there: `GatherFacts` now asks
	 * `UStratMatchSubsystem::DoesSlotHoldARestorableMatch`, which derives from the loader's
	 * own refusal instead of restating it.
	 *
	 * AN EMPTY SLOT NAME STILL MAKES THIS FALSE, and it is still a distinct state from "no
	 * save yet". It is why `IsRoutePermitted` refuses `ContinueMatch` on the destination first
	 * and the slot second: the two refusals send a reader to two different files.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Stratocracy|Shell")
	bool bSaveSlotIsRestorable = false;

	/**
	 * True when a match is running in the current world.
	 *
	 * IT CHANGES LABELS AND ONE PERMISSION, NOTHING ELSE. `NewMatch` reads `Restart Match`
	 * when this is true and is permitted either way; `ReturnToTitle` is permitted only when
	 * it is true, because leaving a match that is not running is not a thing to offer.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Stratocracy|Shell")
	bool bMatchIsLive = false;
};

/** One row of the menu. Every field is drawn as-is; nothing is recomputed by the WBP. */
USTRUCT(BlueprintType)
struct FStratShellOption
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Shell")
	EStratShellRoute Route = EStratShellRoute::NewMatch;

	/** The button's text, already chosen for the current facts. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Shell")
	FText Label;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Shell")
	bool bEnabled = false;

	/**
	 * Why the row is greyed, in the player's words. Empty exactly when `bEnabled` is true.
	 *
	 * §2.11.5's production menu names the shortfall on a greyed row rather than hiding it;
	 * this is that rule on this surface. The invariant "empty iff enabled" is stated so a
	 * clause can pin it in both directions rather than only on the disabled rows.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Shell")
	FText DisabledReason;
};

/**
 * The whole menu.
 *
 * ALL FOUR ROUTES, ALWAYS, IN DECLARATION ORDER. The model does not shrink with the facts --
 * only `bEnabled` and the strings move. That makes the option list assertable by INDEX,
 * which is what lets a clause pin ordering without matching on label text (and this project
 * has measured that `FString` comparison is case-insensitive, so text-keyed lookup is a
 * weaker instrument than it looks).
 */
USTRUCT(BlueprintType)
struct FStratShellMenuModel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Shell")
	TArray<FStratShellOption> Options;
};

/**
 * Decides and performs the moves between §2.11.5's four screens.
 *
 * A menu WBP gets one with the Blueprint node `Get Game Instance Subsystem`, by reflection,
 * and never by an `#include` -- the same route `StratMatchResultWidget.h` describes for the
 * result screen's buttons. `StratUI` does not and must not depend on `StratPlay`.
 */
UCLASS()
class STRATPLAY_API UStratShellSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// ---- THE DECISION. Static, world-free, and the whole of what a clause can call. ----

	/**
	 * The menu, for a stated world.
	 *
	 * STATIC AND TAKING ITS FACTS RATHER THAN GATHERING THEM, which is the single most
	 * load-bearing choice in this class. `GatherFacts` needs a `UGameInstance`, a `UWorld`
	 * and a save file; this needs nothing, so a clause can assert all sixteen combinations
	 * of `FStratShellFacts` in a `-nullrhi` run with no map loaded.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Shell")
	static FStratShellMenuModel BuildMenuModel(const FStratShellFacts& Facts);

	/**
	 * Whether a route may be taken, and why not when it may not.
	 *
	 * THE SINGLE AUTHORITY. `BuildMenuModel` calls this for every row rather than restating
	 * the conditions, and `ExecuteRoute` calls it before travelling rather than trusting
	 * that the WBP disabled the button. A greyed row and a refused command are therefore the
	 * same sentence from the same function, and a menu asset that wired an enabled button to
	 * an impermissible route fails loudly instead of travelling.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Shell")
	static bool IsRoutePermitted(EStratShellRoute Route, const FStratShellFacts& Facts,
	                             FText& OutRefusalReason);

	/**
	 * Which level a route opens.
	 *
	 * IT NEVER LOADS AND NEVER READS A PATH, so a clause may pass soft pointers built from
	 * any string. Returns false for `QuitGame`, which has no destination, and for a
	 * destination that was never configured.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Shell")
	static bool ResolveDestination(EStratShellRoute Route,
	                               const TSoftObjectPtr<UWorld>& InMatchLevel,
	                               const TSoftObjectPtr<UWorld>& InTitleLevel,
	                               TSoftObjectPtr<UWorld>&       OutLevel,
	                               FText&                        OutRefusalReason);

	/** False for `QuitGame` alone. Stated as a function so no caller re-derives it. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Shell")
	static bool RouteTravels(EStratShellRoute Route);

	/**
	 * True for `ContinueMatch` alone.
	 *
	 * THIS IS WHAT SEPARATES A RESTART FROM A RESTORE, and it is the assertable half of the
	 * `T-TURN-09` question. `NewMatch` returns false here whether or not a match is live and
	 * whether or not a save slot exists, so a restart carries no pending load and the
	 * destination world seeds from the scenario exactly as a fresh boot does.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Shell")
	static bool RouteLoadsSaveSlot(EStratShellRoute Route);

	/**
	 * WHICH slot a route arms, given the configured one. `ContinueMatch` gets it; every
	 * other route gets an empty string.
	 *
	 * IT EXISTS BECAUSE `strat-test-author` MEASURED THAT ITS ABSENCE WAS UNFALSIFIABLE, and
	 * that measurement is worth more than the function. The pairing of route to slot lived
	 * inside `ExecuteRoute` as `ArmPendingLoadSlot(SaveSlotName)` -- one line, in the one
	 * member no clause THEN reached -- so an `ExecuteRoute` that armed a literal, an empty
	 * string or the wrong member would have left every clause over this class GREEN. The
	 * five deciders above were extracted on exactly this reasoning and this one was missed;
	 * it is here now on the same footing.
	 *
	 * THAT PARAGRAPH IS ABOUT THE TREE AS IT WAS, AND ITS VERBS SAY SO DELIBERATELY. It read
	 * "no clause CAN reach" until the W6 reviewer pass, which is a claim about the tree as it
	 * IS and was false there: `GATE-TITLEMENU.ARefusedRouteArmsNothing` reaches `ExecuteRoute`
	 * and executes its refusal arm. Past tense is weaker and true. A later reader tempted to
	 * "fix" the tense back is being asked not to -- the sentence describes why this function
	 * was extracted, and that history does not become false when the coverage improves.
	 *
	 * NOT FOLDED INTO `RouteLoadsSaveSlot`, WHICH IS THE OBVIOUS SIMPLIFICATION. That
	 * function answers a question about the ROUTE ALONE, and it is what a menu asset asks to
	 * decide whether a row means "restore"; this one answers a question about the route AND
	 * the configuration, and returns a string. Collapsing them would make the cheap,
	 * configuration-free question unaskable, and that is the one `T-TURN-09`'s clause reads.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Shell")
	static FString PendingSlotForRoute(EStratShellRoute Route, const FString& ConfiguredSlot);

	// ---- CONFIGURATION, from a GameMode's Blueprint defaults on BeginPlay. ----

	/** Called by `AStratShellGameMode`, which is the only object that knows the match level. */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Shell")
	void ConfigureMatchDestination(TSoftObjectPtr<UWorld> InMatchLevel,
	                               const FString&         InSaveSlotName);

	/** Called by `AStratGameMode`, which is the only object that knows the title level. */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Shell")
	void ConfigureTitleDestination(TSoftObjectPtr<UWorld> InTitleLevel);

	// ---- THE LIVE SURFACE. Needs a world; a clause reads the statics above instead. ----

	/**
	 * The current world's facts.
	 *
	 * THE ONE PLACE THE SHELL TOUCHES ANYTHING ELSE. `bMatchIsLive` comes from
	 * `UStratMatchSubsystem::IsMatchLive` and is never inferred from the level name; a
	 * missing match subsystem -- which is every non-Game, non-PIE world -- reads as "no
	 * match", not as a failure, because on the title map that is the correct answer.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Shell")
	FStratShellFacts GatherFacts() const;

	/**
	 * `BuildMenuModel(GatherFacts())`. The node a menu WBP binds to.
	 *
	 * IT TOUCHES THE DISK ON EVERY CALL, AND A UMG PROPERTY BINDING RUNS EVERY FRAME. Said
	 * here, on the node an asset author actually binds, because that is the only place the
	 * cost is visible to the person paying it. `GatherFacts` asks
	 * `UStratMatchSubsystem::DoesSlotHoldARestorableMatch`, which does a full
	 * `LoadGameFromSlot` deserialize -- not the directory-entry check this used to be, back
	 * when the fact was merely "a file exists". `HasCompletedAMatchOnSave` sets the precedent
	 * for a per-call load, so this is not novel; it is worse, because a menu binds per frame
	 * and a guidance check does not.
	 *
	 * NO CACHE, AND THE ABSENCE IS A DECISION RATHER THAN AN OVERSIGHT -- which is the whole
	 * reason this paragraph exists, since the two are indistinguishable from the outside. A
	 * cache with no invalidation is a worse defect than a slow read: it would show Continue
	 * greyed to a player who had just saved, and this project already records a stale-read
	 * defect of exactly that shape. The correct invalidation points are not knowable until
	 * the menu asset exists and its refresh cadence is a fact rather than a guess.
	 *
	 * DISCHARGED BY EITHER, and the first is preferred: the WBP refreshing this on demand --
	 * on construction, and on the events that can change the answer -- rather than binding it
	 * per frame; or this class growing a cache invalidated on `ConfigureMatchDestination`,
	 * `ArmPendingLoadSlot` and `ConsumePendingLoadSlot`, which are the writers that can move
	 * the answer from inside. Whichever lands, the other stops being owed.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Shell")
	FStratShellMenuModel GetMenuModel() const;

	/**
	 * Take a route.
	 *
	 * THE SENTENCE THAT STOOD HERE WAS WRONG AND IS CORRECTED RATHER THAN DELETED, because a
	 * reader arriving at this comment by a citation must learn that it was wrong and what
	 * replaced it.
	 *   RETRACTED>  "THREE LINES OF ENGINE CALL BEHIND A DECISION IT DOES NOT MAKE. It asks
	 *   RETRACTED>   `IsRoutePermitted`, then `ResolveDestination`, then calls
	 *   RETRACTED>   `OpenLevelBySoftObjectPtr` or `QuitGame`."
	 * `strat-test-author` measured the defect that claim concealed: the pairing of a route to
	 * the slot it arms was `ArmPendingLoadSlot(SaveSlotName)`, written HERE and nowhere else,
	 * so an `ExecuteRoute` that armed a literal, an empty string or the wrong member left
	 * every clause over this class green. (That sentence carried the clause COUNT until the
	 * reviewer pass; a numeral restated beside a suite that grows is a second site that rots,
	 * and the quantifier says what the count was there to say.) The claim was directionally
	 * right about the shape and understated what was unreachable, which is the more dangerous
	 * of the two errors.
	 *
	 * WHAT IT CONTAINS NOW, ENUMERATED RATHER THAN COUNTED, IN THE ORDER THE BODY CALLS THEM.
	 * A number here would be a numeral restated at a second site and would drift the next time
	 * a line moves; the list is what a reader can check. It gathers facts (`GatherFacts`),
	 * asks permission (`IsRoutePermitted`), asks whether the route travels at all
	 * (`RouteTravels`) and leaves by the quit arm if it does not, asks for a destination
	 * (`ResolveDestination`), asks which slot to arm (`PendingSlotForRoute`), arms it
	 * (`ArmPendingLoadSlot`), and opens the level. EVERY ITEM ON THAT LIST IS A CALL TO
	 * SOMETHING A CLAUSE CAN REACH ON ITS OWN, EXCEPT THE ENGINE CALLS THEMSELVES --
	 * `QuitGame` and `OpenLevelBySoftObjectPtr`, of which exactly one runs.
	 *
	 * `RouteTravels` WAS MISSING FROM THAT LIST FOR ONE PASS, AND THE OMISSION IS RECORDED
	 * RATHER THAN QUIETLY REPAIRED, because of what had already been built on top of it. The
	 * class block above had just replaced a STALE COPY of the statics with a pointer to their
	 * declarations, on the reasoning that a second copy of a growing list rots invisibly; the
	 * clause file then pointed HERE for this enumeration instead of copying it, on the same
	 * reasoning. Both moves were right. But A POINTER DISCIPLINE IS ONLY AS GOOD AS THE
	 * AUTHORITY IT POINTS AT: adopting it does not make the authority correct, it CONCENTRATES
	 * the cost of the authority being wrong -- and this list was incomplete on the same day it
	 * acquired a dependent. It asserted no false COVERAGE, since `RouteTravels` is public and
	 * clauses pin it; what it got wrong is what this function does, in the one document that
	 * says so.
	 *
	 * THE SENTENCE THAT CLOSED THIS BLOCK WAS ALSO WRONG, AND IT WAS WRITTEN AS PART OF THE
	 * CORRECTION ABOVE -- WHICH IS THE PART OF THIS COMMENT WORTH READING TWICE. Twice now, a
	 * sentence added here to retract an over-claim has been an over-claim of its own about
	 * what the suite covers. A RETRACTION IS NOT SELF-CERTIFYING: it needs the same
	 * measurement the claim it replaces needed, and neither of these two got one before being
	 * written.
	 *   RETRACTED>  "What stays untestable is the engine call and the ORDER of the sequence
	 *   RETRACTED>   -- and the order is now the only thing about this function that could be
	 *   RETRACTED>   wrong without a clause noticing."
	 * `strat-integration-reviewer` falsified it in BOTH directions. HALF THE ORDER IS ALREADY
	 * PINNED: `GATE-TITLEMENU.ARefusedRouteArmsNothing` proves permission is asked BEFORE the
	 * arming, because a refused route leaves nothing armed. AND A SECOND THING THAT IS NOT AN
	 * ORDERING FACT IS UNPINNED: whether the `ArmPendingLoadSlot(...)` call is still
	 * UNCONDITIONAL -- so naming the order as "the only thing" was an affirmative coverage
	 * claim over the conditionality, and a false one.
	 *
	 * THE CONCLUSION ABOVE IS UNCHANGED; ITS EVIDENCE IS REPLACED, AND THE SWAP IS THE POINT.
	 * This sentence cited a mutant: "the `if` this function used to carry was put back and the
	 * suite stayed green." That citation is withdrawn. The instrument behind it was afterwards
	 * shown able to report `Result: Succeeded` for a build that RELINKED NOTHING -- a mutant
	 * tree whose cached build graph still resolved the original sources -- and a GREEN mutant
	 * is precisely the result that failure mode manufactures. A red still proves a relink
	 * happened, absent a flake, and this project records that a red can be a one-in-four
	 * flake; but nothing rescues a green. So the claim was true and the reason given for it
	 * could not have established it.
	 *
	 * WHAT STANDS IN ITS PLACE IS A DERIVATION ANY CHECKOUT CAN REDO. `ExecuteRoute` has
	 * exactly one call site in `Source/` -- inside `FStratShellRefusedRouteArmsNothingTest`,
	 * on a fixture whose own assertion is that the route is REFUSED, so the call returns at
	 * the permission arm and never reaches the arming line at all. Nothing executes that line;
	 * therefore restoring the `if` around it cannot change any observable behaviour, and no
	 * clause can redden. That is stronger than the mutant would have been even had it been
	 * sound, because it says WHY nothing catches the change rather than reporting that nothing
	 * did.
	 *
	 * AND IT IS FALSIFIABLE BY SOMEONE WHO WAS NOT HERE, WHICH THE MUTANT NEVER WAS. A grep
	 * over the tree is re-runnable by any reader at any commit; a mutant is a tree somebody
	 * built once and threw away, and a citation to it is a claim about bytes that no longer
	 * exist. Cite the derivation. It stops being true the moment a second caller appears --
	 * which is the correct trigger, and is exactly when this paragraph must be rewritten.
	 *
	 * WHAT IS ACTUALLY UNPINNED, THEN, AND NOTHING WIDER: the engine call on the permitted
	 * arm; the CONDITIONALITY of the arming call, which the unconditional-call comment in the
	 * body argues for and no clause enforces; and whatever ordering the refusal clause does
	 * not reach. A stronger sentence than that one needs a clause standing behind it before it
	 * is written here.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Shell")
	bool ExecuteRoute(EStratShellRoute Route, FString& OutFailureReason);

	// ---- THE CROSS-LEVEL HANDOFF. ----

	/** The slot a `ContinueMatch` route asked the next world to restore. Empty when none. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Shell")
	FString PeekPendingLoadSlot() const { return PendingLoadSlot; }

	/**
	 * Returns the pending slot and clears it.
	 *
	 * CONSUMING RATHER THAN READING, AND IT IS NOT TIDINESS. `AStratGameMode::BeginPlay`
	 * calls this in the destination world; if it merely read, the request would survive into
	 * every later travel and a player who chose New Match after continuing once would be
	 * silently handed their old save. Once is the contract, and it is a function rather than
	 * a convention so a clause can pin the second call returning empty.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Shell")
	FString ConsumePendingLoadSlot();

	/**
	 * Arms the pending load without travelling.
	 *
	 * IT EXISTS FOR ONE REASON AND THE REASON IS TESTABILITY, SAID SO RATHER THAN DISGUISED.
	 * `ExecuteRoute(ContinueMatch)` sets the slot and then opens a level, and no headless
	 * clause can survive the second half. Splitting the arming out means the handoff --
	 * arm, consume, consume again -- is assertable end to end without a travel, and
	 * `ExecuteRoute` calls THIS rather than writing the member itself, so the tested path
	 * and the shipped path are the same line.
	 *
	 * THAT ARGUMENT WAS ONLY HALF-APPLIED WHEN IT WAS FIRST WRITTEN, and the other half is
	 * now `PendingSlotForRoute`. Extracting the WRITE while leaving the choice of WHAT to
	 * write inside `ExecuteRoute` made the shipped path and the tested path the same line
	 * for the storing and a different line for the deciding -- and the deciding is the half
	 * that could be wrong.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Shell")
	void ArmPendingLoadSlot(const FString& InSlotName);

	/** The slot name configured on the title GameMode. Empty until one configures it. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Shell")
	FString GetSaveSlotName() const { return SaveSlotName; }

	/**
	 * Whether `ConfigureMatchDestination` has been called on this subsystem yet.
	 *
	 * IT RECORDS THE CALL AND NOT ITS ARGUMENTS, WHICH IS THE ONLY REASON IT IS A SEPARATE
	 * FLAG. Every value the call carries has a legal default a configured shell can also hold:
	 * `MatchLevel` is null on a genuinely unconfigured Blueprint, and `SaveSlotName` arrives
	 * non-empty by construction from `AStratShellGameMode`'s constructor, so neither can
	 * distinguish "nobody has called" from "somebody called with these values". This project
	 * already carries a defect of exactly that species -- a real default that could not signal
	 * unset, which wrote the player's save every run -- so the distinction is made explicit
	 * here rather than left to be inferred by every reader.
	 *
	 * WHAT IT IS FOR. `AStratShellHUD` creates the title menu only once this is true, which is
	 * what turns "the menu happens to be built a tick after `Super::BeginPlay()`" into "the
	 * shell is configured before the menu is asked for its model". IT SAYS NOTHING ABOUT
	 * WHETHER THE CONFIGURATION WAS ANY GOOD -- a configured shell whose `MatchLevel` is null
	 * reads true here and false on `FStratShellFacts::bMatchLevelConfigured`, and the two
	 * questions must not be confused.
	 *
	 * IT IS NEVER CLEARED. There is no `Unconfigure`, and a second call overwrites the values
	 * while leaving this true, because the question it answers is "has a GameMode ever handed
	 * this subsystem its destination in this process" -- and the game-instance lifetime this
	 * class holds for its own stated reason is exactly the span over which that question has
	 * one answer.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Shell")
	bool HasMatchDestinationBeenConfigured() const { return bMatchDestinationConfigured; }

private:
	/** Set by `AStratShellGameMode`. Never a `/Game/` literal in this file. */
	UPROPERTY(Transient)
	TSoftObjectPtr<UWorld> MatchLevel;

	/** Set by `AStratGameMode`. */
	UPROPERTY(Transient)
	TSoftObjectPtr<UWorld> TitleLevel;

	/** Set by `AStratShellGameMode`. Empty means unconfigured, and no literal stands in. */
	UPROPERTY(Transient)
	FString SaveSlotName;

	/**
	 * Set by `ConfigureMatchDestination`, read by `HasMatchDestinationBeenConfigured`.
	 *
	 * `Transient` LIKE EVERY OTHER MEMBER HERE. Nothing about the shell survives a process,
	 * and a saved "already configured" would be a lie on the first frame of the next launch.
	 */
	UPROPERTY(Transient)
	bool bMatchDestinationConfigured = false;

	/** Written by `ExecuteRoute(ContinueMatch)`, read once by the next world's GameMode. */
	UPROPERTY(Transient)
	FString PendingLoadSlot;
};
