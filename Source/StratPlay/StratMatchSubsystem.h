// GDD §4.1 / §4.9 / §4.10 / T-INT-05 -- the match owner. The object that finally makes
// §4.1's "never own rules" structurally true of the actors instead of aspirationally true.
//
// WHAT GAP THIS CLOSES, and it is a debt written down in advance rather than discovered.
// `StratScoreboardHUD.h` says, of itself: "this HUD OWNS the authoritative
// `strat::GameState` today because nothing else in the running game does. That is the
// wrong long-term owner -- §4.1's 'never own rules' is about actors not holding rules
// state, and a HUD is an actor. The moment a proper owner exists ... this class MUST drop
// to BORROWING a bridge it did not create." This is that owner. The seam it hands the
// bridge across -- `AdoptBridge` -- was built in phase 2 for this class specifically, so
// the handover costs one call rather than a rewrite.
//
// WHY A `UWorldSubsystem` AND NOT THE THREE ALTERNATIVES.
//   1. NOT A GameInstance SUBSYSTEM. `StratScoreboardHUD.h` already rejected that for the
//      bridge and the reason still holds: a GameInstance subsystem outlives the map, and
//      the seeded state is PER-MAP. It would carry one scenario's GameState into the next
//      level and have to remember to tear it down. A world subsystem is created with the
//      world and destroyed with it, which is the lifetime `strat::GameState` actually has.
//   2. NOT THE GameMode ITSELF, which is where a GameState-shaped thing conventionally
//      lives. A GameMode is an ACTOR, and putting the rules state back on an actor is the
//      exact thing §4.1 forbids and the exact debt the HUD's header was opened to discharge.
//      The GameMode configures this subsystem and does not hold what it configures.
//   3. NOT THE HUD, still. It is client-local and per-player -- correct for a scoreboard,
//      wrong for the authoritative state of the match, which there is exactly one of.
//
// SEEDING HAPPENS BEFORE THE HANDOVER, AND THAT ORDER IS A CONTRACT RATHER THAN A HABIT.
// `AStratScoreboardHUD::AdoptBridge` REFUSES an unseeded bridge, in its own words, so that
// "nothing is loaded" can never be drawn as "a match in which nothing has happened".
// `StartMatch` therefore runs `LoadDefinitions` then `LoadScenarioFromFile` -- after which
// `FStratBridge::IsSeeded()` is true -- and only then calls `AdoptBridge`. Reversed, the
// adoption returns false, this class logs a refusal, and the scoreboard silently keeps
// drawing from its own separately-seeded bridge, which is a SECOND `strat::GameState` for
// the same map that would agree with the first exactly until the first command. That is
// the failure this ordering exists to make impossible, and it is why the sequence in
// `StartMatch` is written as one function with no early handover.
//
// PRESENTATION IS RECONCILED, NOT EVENTED. `ApplyView` spawns, moves and destroys actors
// to match `FStratViewModel` on every refresh, and there is no incremental path beside it.
// `StratViewModel.h` states the property this rests on: the model "is a COMPLETE
// DESCRIPTION OF WHAT SHOULD BE ON SCREEN AND NEVER A DELTA. There is no 'changed' flag,
// no event, no dirty set and no sequence number anywhere below, and adding one would be
// the first step towards a screen that is only correct if every previous frame was also
// correct." Reconciling is therefore cheap to do UNCONDITIONALLY, and doing it
// unconditionally is what makes T-INT-05 -- rebuild the screen from the view model alone
// -- a fact about the code rather than a claim about it.
//
// `ApplyView` IS PUBLIC AND TAKES THE MODEL BY ARGUMENT, and that is phase 4's seam.
// `StratViewModel.h` records the obligation: the selection machine "writes these two bits
// [`bDone`, `bLockedThisTurn`] onto the built model and does NOT keep its own copy in an
// actor -- the moment it does, T-INT-05 is false and no compiler says so." So the phase-4
// path is build -> decorate -> `ApplyView`, with the decorated model being the one and
// only description of the screen. `RefreshPresentation` below is the undecorated path and
// is deliberately written in terms of the same two calls, so the two cannot drift.
//
// IT DECIDES NOTHING AND COMPUTES NOTHING. Every rules answer comes from a method on
// `FStratBridge`; this class holds the bridge, orders the calls and reconciles actors. It
// contains no reachability, no legality check, no damage number and no hex arithmetic
// beyond asking the board actor where a hex is.
//
// IT NAMES NO `strat::` TYPE. Hexes are `FIntPoint` with X = q and Y = r. `StratBridge.h`
// is included from the .cpp ONLY -- this header declares a UCLASS, so UHT parses it, and
// the vendored headers must never get in front of the header tool. `FStratBridge` is
// forward declared and held as `TPimplPtr`, never `TUniquePtr`: measured as C4150,
// "deletion of pointer to incomplete type 'FStratBridge'", reported from UniquePtr.h and
// compiled from a `.gen.cpp` that no definition in a .cpp file can reach
// (`StratScoreboardHUD.h`).
//
// NOT IN THIS ROUND, with reasons:
// - Submitting commands -- STILL TRUE OF FOUR OF THE FIVE, AND NO LONGER TRUE OF BUILD.
//   This bullet used to read, whole:
//   RETRACTED> "Submitting commands. The five `FStratBridge::Submit*` methods exist and
//   RETRACTED>  this class deliberately does not wrap them yet: phase 4 owns the
//   RETRACTED>  PlayerController, the selection machine and the `STRAT-CMD accepted` log
//   RETRACTED>  line, and a wrapper landed early is a wrapper written without its caller."
//   Its own condition has now been met for Build and for Build alone: Sec 2.11.5's
//   production menu is that caller, and `SubmitProductionChoice` below is the wrapper.
//   Move, Attack and EndTurn are still reached through `FStratSelectionMachine` and
//   `StratSubmitSelectionCommand` from `AStratPlayerController`, which is where a CLICK ON
//   THE BOARD belongs; a menu pick has no board click and no selection state to advance,
//   which is why it does not travel that path.
//   AND THE CAPTURE HALF IS NOW RETRACTED TOO, ON EVIDENCE RATHER THAN ON A SCHEDULE. It
//   used to read "Capture still has no engine caller at all", carried as owed work. It has
//   no engine caller and IS OWED NOTHING; three measurements against the tree say so and
//   the finding replaces the deferral:
//     (a) NO AFFORDANCE IS OWED, BY DESIGN. §2.11 states it: "Capture and build need no
//         extra verbs. Capture is by presence (§2.7: an Infantry that ends its move on a
//         capturable tile begins capturing -- a progress pip appears, no button)."
//         `strat::AiCommandKind` is `{Build, Move, Attack, EndTurn}` for the same reason.
//         The GDD is split -- §4.7 Stub 4 and §4.9/§4.10 list `Capture{unit}` among the five
//         submittable commands -- and the rules module already resolved the split toward
//         §2.11. So this is not an unanswered affordance question; it is an answered one.
//     (b) THE METHOD IS SAVE-FORMAT TRANSPORT AND IS KEPT FOR THAT. `strat::SaveCommandKind`
//         is pinned at §4.10's five, and `Source/StratBridge/Tests/StratBridgeRestoreParity.
//         cpp` and `StratBridgeSaveRecording.cpp` both dispatch `SubmitCapture` in log
//         replay. Deleting it would break replay of any log carrying a `Capture` entry.
//     (c) `strat::captureTick` IS NOT IDEMPOTENT, and this is the reason never to wire one.
//         Its progress arm is `prog->turnsHeld += 1` per call, and `applyCommand`'s Capture
//         arm calls it with the whole board's occupants -- never reading `c.unitId` at all.
//         N mid-turn submissions therefore advance a capture N turns' worth inside one
//         turn. On the shipped scenario (`captureTurns = 1`) that is masked. An affordance
//         bound to it would ship a cheat.
//   RETRACTED IN PLACE rather than deleted, because a reader who remembers the old claim
//   needs to see WHICH HALF of it was withdrawn -- and both halves now have been, for two
//   different reasons.
// - Turn hand-over. §2.11's hot-seat swap changes the viewing side, and `SetViewingSide`
//   below does exactly that and nothing more -- it submits no EndTurn, because whose screen
//   this is and whose turn it is are different questions (`StratScoreboardHUD.h` on the same
//   pair).
// - NOT ANY MORE: SAVE AND LOAD. This bullet used to read:
//   RETRACTED> "`SerializeRecordedSave` is on the bridge and has no engine-side caller;
//   RETRACTED>  where a save LIVES is the save-slot UI's question."
//   Wave B1 gave it one. `SaveMatchToSlot` and `LoadMatchFromSlot` below are
//   that caller, `UStratSaveGame` is the slot payload, and `FStratBridge::
//   RestoreFromSaveText` is the load half the bridge did not have. The bullet is
//   RETRACTED IN PLACE rather than deleted, because a reader who remembers the old claim
//   needs to see it withdrawn and not silently absent.
// - Any `/Game/` path. The tables, the scenario file and the two actor classes arrive in
//   `FStratMatchConfig` from a Blueprint default on the GameMode.
#pragma once

#include "CoreMinimal.h"
#include "Engine/TimerHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "Templates/PimplPtr.h"
#include "Templates/SubclassOf.h"

#include "StratAiPlayback.h"
#include "StratSoundCues.h"
#include "StratTransientReceipts.h"
#include "StratViewModel.h"

#include "StratMatchSubsystem.generated.h"

// Forward declaration only. See the header block: this may never become an include in
// this file.
class FStratBridge;

class AStratBoardActor;
class AStratScoreboardHUD;
class AStratUnitActor;
class UDataTable;
class UStratSaveGame;
class UStratSoundBank;
class UStratSoundDirector;

/**
 * The seam that completes a freshly built model before anything is drawn from it.
 *
 * WHAT GAP THIS CLOSES, AND IT IS A MEASURED DEFECT RATHER THAN A TIDY-UP. Until this
 * existed there were THREE paths on which a model reached `ApplyView` and only ONE of them
 * decorated: `AStratPlayerController::RefreshFromMachine` built, observed, decorated and
 * applied, while `StartMatchInternal`'s first reconciliation and `RefreshPresentation`
 * built and applied a model whose `Guidance` block was still default-constructed. Driven
 * and watched in PIE on 2026-08-21: a decorated refresh put a live directive on GDD
 * Sec 2.11.6's strip, and the very next `SetViewingSide` -- which goes through
 * `RefreshPresentation` -- left the widget reading `bActive=False, Beat=None,
 * DirectiveText=""` one second later. The same clearing was inferred, not observed, for the
 * first AI turn, which reaches `RefreshPresentation` through `RunAiTurnsNow`.
 *
 * THE PUSH WAS NOT THE DEFECT AND HAS NOT BEEN CHANGED. `ApplyView`'s guidance block is
 * emphatic that the push is "UNCONDITIONAL, WITH NO BRANCH ON `bActive`", because a strip
 * whose contents depended on which pushes were skipped would depend on the history of calls
 * instead of on the current model. That ruling stands untouched. What was wrong is that
 * models MISSING THEIR GUIDANCE SECTION reached `ApplyView` at all -- so the fix is at the
 * build, where the section is filled in, and not at the push, where a branch would have
 * bought a stale directive instead.
 *
 * A DELEGATE AND NOT A DEPENDENCY ON THE CONTROLLER. `FStratGuidedOpening` and
 * `FStratSelectionMachine` are held BY VALUE on `AStratPlayerController` -- both headers
 * record why they are plain C++ and not `UObject`s -- so this subsystem cannot own the
 * decoration without owning those two machines, which would move a per-seat input machine
 * onto a per-world object. Nor may it reach out and find the controller by type: that would
 * make "who decorates" a search rather than a registration, and a search answers
 * differently, silently, in a test world with no controller in it.
 *
 * BOUND WITH `CreateUObject`, SO IT IS WEAK. A controller destroyed without unbinding stops
 * decorating rather than crashing, and `ExecuteIfBound` reports that as "nobody decorates",
 * which is the same state a world with no controller has.
 * `AStratPlayerController::EndPlay` still releases it explicitly, because a seam taken
 * should be given back where it was taken.
 *
 * AT MOST ONE, AND THE LAST WRITER WINS. Sec 2.11's hot seat is ONE controller holding two
 * seats -- `FStratGuidedOpening::Begin` takes the guided side by argument precisely so the
 * viewing side can move without the guided seat moving -- so there is no configuration in
 * this milestone with two decorators to arbitrate between. A multicast delegate would have
 * invited two authors to write the same fields of one model, and which of them won would be
 * bind order.
 *
 * NOT REFLECTED, AND IT MAY NOT BECOME SO. A `DYNAMIC` delegate would let a Blueprint graph
 * register itself as the author of the guidance block, which is exactly the second-author
 * failure `UStratGuidanceWidget::PushGuidance` declines reflection to prevent.
 */
DECLARE_DELEGATE_OneParam(FStratViewDecorator, FStratViewModel& /*Model*/);

/**
 * Sec 2.9's difficulty tier, which this project implements as a starting-Fame handicap
 * and as NOTHING ELSE.
 *
 * THE GDD IS EXPLICIT THAT THIS IS NOT A SMARTER AI: "the baseline routine is identical
 * at every tier; only the economy shifts". So this enum reaches exactly one number --
 * `StratDifficultyFameDelta` below -- and no branch of `FStratAiTurnRunner` may ever read
 * it. A tier that also changed the AI's behaviour would make Sec 2.9's stated property
 * ("deterministic and trivially tunable, with no AI-quality risk") false, and would do so
 * in a way no economy assertion could catch.
 *
 * REFLECTED, BECAUSE A DESIGNER SETS IT. It is a `FStratMatchConfig` field like every
 * other field there, shown in the `AStratGameMode` Blueprint's details panel; `uint8` is
 * what `UENUM(BlueprintType)` requires.
 *
 * THREE VALUES AND NO `Custom`. Sec 2.9 names three tiers and gives three numbers; a
 * fourth value carrying a designer-typed delta would be a second way to say the same
 * thing, and the two could then disagree about what "Hard" means.
 */
UENUM(BlueprintType)
enum class EStratDifficulty : uint8
{
	/** Sec 2.9: the player opens +150 above the scenario's configured value. */
	Easy   UMETA(DisplayName = "Easy"),

	/** Sec 2.9: even. The scenario's configured value stands unchanged for both sides. */
	Normal UMETA(DisplayName = "Normal"),

	/** Sec 2.9: the player opens 100 below the scenario's configured value. */
	Hard   UMETA(DisplayName = "Hard")
};

/**
 * The opening-Fame delta Sec 2.9 gives each tier: +150 / 0 / -100, as a number to ADD to
 * whatever the scenario configured for the player's side.
 *
 * A DELTA AND NOT AN ABSOLUTE, and this is the whole reason the function exists rather
 * than three constants read at the call site. Sec 2.7 says the 200 is "a baseline, not a
 * constant, for the player", and T-FAME-02 says a gate must assert "each side's configured
 * value and never a literal 200". Returning 350 here would make this file a second author
 * of `Data/ferrum_crossing.json`'s `startingFame`, and the two would silently disagree the
 * first time a scenario opened on anything else.
 *
 * THE CHECKABLE FORM OF THAT CLAIM IS ABOUT WHAT IS ASSIGNED AND NOT ABOUT WHAT A TEXT
 * SEARCH FINDS, and it is written that way because the other way was written first and was
 * false. THIS FUNCTION AUTHORS NO OPENING-FAME VALUE: it returns three deltas, and the only
 * statement in `StratBridge`, `StratPlay` or `StratUI` that writes a side's `fameTotal` is
 * the one in `FStratBridge::ApplyStartingFameHandicap`, which ADDS a delta to a number this
 * code did not choose. `Data/ferrum_crossing.json` and `strat::initSide` own the absolute;
 * nothing here does.
 *
 * CITED BY SYMBOL AND NOT BY A GREP PATTERN, DELIBERATELY. A pattern quoted in a comment is
 * matched by its own search, so a census phrased that way begins returning the sentence that
 * made it -- which is exactly how the sentence this paragraph replaced went wrong. It read
 * "350 and 100 appear NOWHERE in `Source/`", a claim about TEXT IN A DIRECTORY standing in
 * for a claim about VALUES IN CODE, and a search for those numerals returns it along with
 * every other line of prose here that discusses the rule. The numerals DO occur in this file
 * and in `StratBridge.h`, as prose. What none of them is, is a value assigned to a purse.
 *
 * FREE AND `STRATPLAY_API`, on the precedent of `StratMatchIsConcluded` below: the tier ->
 * delta mapping is a pure function of its argument, a clause has to be able to call it
 * without standing up a world, and a method on the subsystem would have made it reachable
 * only from one.
 *
 * TOTAL OVER THE ENUM AND WITH NO `default:` LABEL, so that adding a tier is a compiler
 * warning here instead of a silent 0 at runtime.
 */
STRATPLAY_API int32 StratDifficultyFameDelta(EStratDifficulty Difficulty);

/**
 * Everything `StartMatch` needs and nothing it can derive.
 *
 * A STRUCT AND NOT SEVEN ARGUMENTS, because the caller is a Blueprint default -- one
 * `EditDefaultsOnly` property on `AStratGameMode` -- and a struct is what a details panel
 * can show as a group. It also means adding a field later does not change the signature
 * every caller passes.
 *
 * NOT ONE `/Game/` PATH. Two `UDataTable*`, two `TSubclassOf`, a project-relative file
 * name and two integers. `.agents/ue-project-context.md` records the rule and its two
 * standing exceptions (automation fixtures, the import commandlet); neither is this.
 */
USTRUCT(BlueprintType)
struct FStratMatchConfig
{
	GENERATED_BODY()

	/**
	 * The §4.8 definition tables, handed straight to `FStratBridge::LoadDefinitions`.
	 *
	 * ROW ORDER IS LOAD-BEARING FOR UNITS AND IS NOT THIS MODULE'S BUSINESS. A §4.10 Build
	 * command carries a `defIndex` that indexes the vector the bridge builds from
	 * `UnitTable`, and `GATE-BRIDGE-DEFS` asserts that order against `strat::loadUnits`
	 * positionally. Pointing this at a different asset than the one that gate checks is a
	 * way to make a log resolve to different units, silently -- which is why the assertion
	 * lives in a gate and not in a comment here.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stratocracy|Data")
	TObjectPtr<UDataTable> UnitTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stratocracy|Data")
	TObjectPtr<UDataTable> TerrainTable;

	/**
	 * The scenario file, as a path RELATIVE TO THE PROJECT DIRECTORY
	 * (e.g. `Data/ferrum_crossing.json`), resolved against `FPaths::ProjectDir()` in the
	 * .cpp and used as-is if absolute.
	 *
	 * The subdirectory is part of the configured string rather than pasted on in C++, for
	 * the reason `StratScoreboardHUD.h` gives about its own copy of this property: half a
	 * path in C++ is the kind of hardcoded path that survives review by being too small to
	 * notice. This file resolves it the same way that class does, so the game and the gate
	 * open the same file when configured to.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stratocracy|Data")
	FString ScenarioFile;

	/**
	 * Which side moves first, passed straight to `LoadScenarioFromFile`.
	 *
	 * CONFIGURED RATHER THAN ASSUMED, because `Replay.h` states that no rule in this
	 * project decides it. Pinning it in C++ would make this file a silent second author of
	 * something the rules module deliberately declined to decide. NOT range-checked here:
	 * an out-of-range value comes back as the bridge's own refusal, in its words.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stratocracy|Data")
	int32 FirstSide = 0;

	/**
	 * Which `strat` side the screen is drawn FOR at the start of the match.
	 *
	 * NOT `sideToMove`, and the two differ every other turn in hot-seat. This is the
	 * initial value of the subsystem's viewing side; `SetViewingSide` moves it at the
	 * hand-over.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stratocracy|View")
	int32 ViewingSide = 0;

	/**
	 * The board and unit classes to spawn.
	 *
	 * `TSubclassOf` AND NEVER A ConstructorHelpers PATH, so the Blueprints that carry the
	 * meshes can be moved or renamed without a compile break. Unset `BoardActorClass` means
	 * no board is spawned and `StartMatch` says so; unset `UnitActorClass` means the match
	 * runs with no visible units, which is a legitimate configuration for a scoreboard-only
	 * map and is reported once rather than per unit.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stratocracy|Presentation")
	TSubclassOf<AStratBoardActor> BoardActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stratocracy|Presentation")
	TSubclassOf<AStratUnitActor> UnitActorClass;

	/**
	 * The AUDIO milestone's cue assets for this match. Null means the match runs silent, which
	 * is the shipped state until a GameMode Blueprint points at a bank.
	 *
	 * A POINTER SET ON A BLUEPRINT DEFAULT AND NEVER A `/Game/` LITERAL, per CLAUDE.md #4 and
	 * the pattern `StratGameMode.h` states: this struct is the object that HAS the map's asset
	 * references, and a renamed bank must not be a compile break.
	 *
	 * ADOPTED IN `StartMatchInternal` BEFORE THE FIRST `ApplyView`, so the director is
	 * configured by the time anything could ask it for a sound. The first reconcile emits
	 * nothing anyway -- `FStratSoundMark` is unseeded -- but ordering it after the first apply
	 * would make that a coincidence rather than a guarantee.
	 *
	 * THE DRIFT IT CREATES IS NAMED HERE RATHER THAN LEFT TO BE FOUND. `AStratShellGameMode`
	 * carries its own `SoundBank` for the title map, so TWO Blueprints hold an answer to "which
	 * bank". Unlike `SaveSlotName` -- whose two copies are reconciled in C++, because
	 * `AStratShellGameMode`'s constructor derives it from `FStratMatchConfig()`'s default -- an
	 * ASSET POINTER HAS NO C++ AUTHOR to derive from. Nothing in this tree can make the two
	 * agree by construction, so nothing here pretends to. DISCHARGED BY a clause comparing the
	 * two CDOs, which is phase D's and is not this file's to write.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stratocracy|Presentation")
	TObjectPtr<UStratSoundBank> SoundBank;

	// ---- §2.9's opponent -------------------------------------------------
	// EVERY C++ FIELD DEFAULT BELOW PRESERVES HOT-SEAT EXACTLY -- AND THE SHIPPED GAME DOES
	// NOT RUN ON THOSE DEFAULTS. An empty `AiSides` means no side is played by the AI; that
	// is what this struct initialises to, and it is what the game ran on through phases 0-6.
	// Nothing in this block runs until a Blueprint default says otherwise. That was
	// deliberate: a milestone that turned the AI on by defaulting it on would change the
	// meaning of every existing test and every existing PIE session in the same pass that
	// introduced it.
	//
	// A BLUEPRINT DEFAULT NOW SAYS OTHERWISE, AND IT IS THE SHIPPED STATE.
	// `BP_StratGameMode` authors `MatchConfig.AiSides=(1)`: side 0 (Directorate) is the
	// human, side 1 (Vanguard) is the AI. THE SHIPPED GAME IS HUMAN-VERSUS-AI AND IS NOT A
	// HOT SEAT. USER RULING, 2026-09-03: that authored value is INTENDED and not Blueprint
	// drift; the asset is correct as authored and no logic changes on account of it.
	//
	// THE MEASUREMENT BEHIND THAT RULING IS NOT THIS FILE'S AND IS NOT RESTATED AS ITS OWN.
	// It was taken off the live editor by the `coordinator` on 2026-09-03, with controls:
	// `BP_StratGameMode_AiVsAi` reads `AiSides=(0,1)`, `struct_properties("StratMatchConfig")`
	// gives the C++ default as empty, and `BP_StratShellGameMode` carries no `MatchConfig` at
	// all. `ViewingSide` 0 and `Difficulty` `Easy` are absent from both the native and the BP
	// export, which is what a zero-valued field sitting at its C++ default looks like.
	//
	// SO A COMMENT IN THIS FILE THAT DESCRIBES AN EMPTY `AiSides` MUST SAY "THE C++ DEFAULT"
	// AND MUST NOT SAY "THE SHIPPED CONFIGURATION". They are different states. Conflating
	// them is the defect this block, the `Difficulty` block below, and `StratHandicappedSide`
	// at both its declaration and its definition were repaired for on 2026-09-03.

	/**
	 * Which `strat` sides are played by §2.9's AI rather than by a human.
	 *
	 * A LIST AND NOT TWO BOOLS, because "is side N the AI" is the only question anyone asks
	 * of it and a list answers that for any side count without this file naming 2. §2.8 is
	 * two-sided today; a bool pair would be this module deciding that, which is a scenario
	 * fact (`Data/ferrum_crossing.json`) and not an engine one.
	 *
	 * BOTH SIDES IS A LEGAL VALUE and is what phase D's AI-vs-AI gate configures. It is
	 * bounded by `AiMaxConsecutiveTurns` below rather than by a rule forbidding it.
	 *
	 * NOT RANGE-CHECKED HERE. A side that does not exist simply never matches
	 * `sideToMove`, so the AI never runs for it -- the same outcome as leaving it out, and
	 * without this file inventing an upper bound on a number the scenario owns.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stratocracy|AI")
	TArray<int32> AiSides;

	/**
	 * Sec 2.9's difficulty tier, applied as a starting-Fame handicap to the player's side
	 * once the scenario has seeded.
	 *
	 * PLACED HERE, INSIDE THE AI BLOCK, BECAUSE `AiSides` IS WHAT ARMS IT. Sec 2.7 calls
	 * this "Single-player difficulty" and says it moves "the player's side only", so with
	 * `AiSides` empty there is no player as distinct from an opponent and the handicap has
	 * no side to move. `StratHandicappedSide` below is that reading in code, and it is the
	 * only place this field is consulted.
	 *
	 * THE INERTNESS IS A `coordinator` RULING FROM A TWO-WORD READING OF Sec 2.7 -- the
	 * words "Single-player difficulty" -- AND IS NOT A GDD QUOTATION. Said plainly here so
	 * a later reader can find it and overturn it rather than having to reconstruct why the
	 * hot seat is exempt. What the ruling avoids is concrete: applying the handicap to
	 * every human side would move a hot-seat opening from 200/200 to 350/350 and change the
	 * meaning of every existing clause in the same pass that introduced the field -- which
	 * is, in those words, what this block's own preamble above says it refused to do for
	 * the AI. If the GDD is later read as handicapping a hot seat's first seat, the change
	 * is to `StratHandicappedSide` alone.
	 *
	 * DEFAULTS TO `Easy`, AND THAT DEFAULT IS INERT ON THE C++ DEFAULTS AND LIVE ON THE
	 * SHIPPED CONFIGURATION. Sec 2.11.6 says "the first match runs on the one shipped
	 * scenario at Easy by default", so `Easy` is the honest C++ default. With `AiSides` at
	 * ITS C++ default -- empty -- the tier reaches `StratHandicappedSide` and comes back
	 * `INDEX_NONE`, and that remains exactly true of this struct's own defaults and of every
	 * fixture that builds it without touching `AiSides`.
	 *
	 * [CORRECTED 2026-09-03. This paragraph read, in the words on the next three lines:
	 * RETRACTED> "THAT DEFAULT IS INERT ON THE SHIPPED CONFIGURATION ... `AiSides` defaults
	 * RETRACTED> empty, so on the shipped hot seat the default reaches `StratHandicappedSide`
	 * RETRACTED> and comes back `INDEX_NONE`".
	 * Its antecedent was the C++ default and its consequence was asserted of the SHIPPED
	 * game, which is the conflation the block above now forbids in terms. `BP_StratGameMode` authors
	 * `AiSides=(1)`, so on the shipped configuration ARM ONE fails (`Num()` is 1) and ARM TWO
	 * fails (`AiSides` holds 1, `ViewingSide` is 0): `StratHandicappedSide` returns
	 * `ViewingSide` = 0, and `Easy`'s +150 lands on the HUMAN's side. USER RULING, 2026-09-03:
	 * THAT IS INTENDED -- Easy is meant to help the player, so the handicap landing on the
	 * human is correct behaviour and not a defect. Nothing in `StratHandicappedSide` changes;
	 * only this prose was wrong.]
	 *
	 * `Easy` RATHER THAN `Normal` STILL FOR THE REASON GIVEN, WHICH SURVIVES THE CORRECTION
	 * INTACT. `Normal`'s delta is 0, so choosing it would have made the handicap unobservable
	 * by ARITHMETIC rather than by configuration, and would have hidden the "single-player
	 * only" ruling above instead of leaving it checkable. That argument never depended on
	 * which configuration ships, which is why it needed no repair when this one did.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stratocracy|AI")
	EStratDifficulty Difficulty = EStratDifficulty::Easy;

	/**
	 * §2.9's buildlist, BY UNIT ID, handed to `FStratBridge::SetBuildlistByIds` after
	 * seeding.
	 *
	 * BY NAME AND NEVER BY INDEX, and the bridge's own header explains why at length: a
	 * defIndex is the raw, bounds-checked-only index a §4.10 Build command carries, and a
	 * config full of hand-written 0s and 1s is exactly how phase 0's hazard gets
	 * reintroduced. Resolution happens once, inside the module that can see `UnitDefs()`.
	 *
	 * DUPLICATES ARE THE POINT. §2.9 describes "mostly Infantry, an occasional Tank" and
	 * gives no ratio, so repetition in this list IS the ratio. The bridge preserves them.
	 *
	 * EMPTY CONFIGURES AN AI THAT NEVER BUILDS, which is an ordinary configuration and not
	 * a fault -- and it is the default, so phase C authoring this list is what turns
	 * production on. It is distinguishable from a misspelled id, because an unresolvable id
	 * refuses by name.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stratocracy|AI")
	TArray<FName> AiBuildlistUnitIds;

	/**
	 * The AI turn runner's termination bound. See `FStratAiTurnRunner::MaxCommandsPerTurn`
	 * for why 256 and why zero is a refusal rather than "unbounded".
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stratocracy|AI")
	int32 AiMaxCommandsPerTurn = 256;

	/**
	 * How many AI turns may run back to back before this subsystem calls it a fault.
	 *
	 * IT IS A SECOND, OUTER BOUND AND IT GUARDS A DIFFERENT FAILURE. `AiMaxCommandsPerTurn`
	 * bounds one turn; this bounds the handover loop that runs when BOTH sides are AI and
	 * the match never reaches a §2.8 result. Ferrum Crossing's `turnCap` is 20, so 64
	 * clears a whole AI-vs-AI game with room for a longer scenario; it exists so that a
	 * scenario whose cap never fires cannot spin forever inside one call.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stratocracy|AI")
	int32 AiMaxConsecutiveTurns = 64;

	/**
	 * §2.11's pacing: seconds to wait before an AI turn starts.
	 *
	 * ZERO RUNS SYNCHRONOUSLY AND IS THE DEFAULT, which is what keeps the AI path drivable
	 * from a test with no ticking world. A positive value schedules a timer instead, so a
	 * human can see the board they just handed over before it changes under them.
	 *
	 * BEFORE THE TURN AND NOT BETWEEN COMMANDS. A per-command delay would make
	 * `FStratAiTurnRunner` an incremental machine holding resumable state between ticks, and
	 * resumable state part-way through a turn is a mirror of the rules state by another
	 * name -- the shape `FStratSelectionMachine` refused for the same reason.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stratocracy|AI")
	float AiTurnDelaySeconds = 0.0f;

	/**
	 * §2.11.2's playback: seconds the camera rests on each of the AI's actions afterwards.
	 *
	 * THE OTHER HALF OF THE FIELD ABOVE AND NOT A SECOND SPELLING OF IT. `AiTurnDelaySeconds`
	 * waits BEFORE the turn, so a human sees the board they handed over; this one steps the
	 * camera through what the AI did AFTER the turn has entirely resolved. Both may be set,
	 * neither implies the other, and §2.11.2 asks for this one: "the headless AI resolves
	 * instantly; the presentation layer replays its action list at a watchable fixed pace
	 * (~0.5 s per action, camera stepping to each)".
	 *
	 * ZERO PLAYS NOTHING AND IS THE DEFAULT, for `AiTurnDelaySeconds`' reason exactly: the
	 * synchronous path is the tested one and an automation test has no ticking world to step a
	 * camera in. So this ships inert and a Blueprint default turns it on.
	 *
	 * §2.11.2's 0.5 IS NOT WRITTEN HERE, AND THE OMISSION IS DELIBERATE RATHER THAN AN
	 * OVERSIGHT. A C++ default of 0.5 would be a second place the pace is stated, and the
	 * designer-facing one on the GameMode Blueprint would be the one nobody could find when
	 * they disagreed. It would also change the path every existing test runs down, which is
	 * the change this field exists to avoid making. The GDD's figure belongs on the Blueprint
	 * default; this is the switch and not the setting.
	 *
	 * IT PACES PRESENTATION AND NOTHING ELSE. No command is submitted while it runs, no
	 * `FStratBridge` method is called, and the board already shows the final state -- see
	 * `StratAiPlayback.h`, which is where that reading of §2.11.2 is argued.
	 *
	 * THERE IS A THIRD PACING NUMBER AND IT DOES NOT LIVE ON THIS CLASS, WHICH IS WHY IT IS
	 * NAMED HERE. `AStratUnitActor::MoveTweenSeconds` is how long a unit's visual slide takes
	 * after it is moved; it sits on the UNIT ACTOR because it is per-actor presentation and
	 * this subsystem never reads it.
	 *
	 * **"THIS SUBSYSTEM NEVER READS IT" IS STILL TRUE AFTER 2026-09-02, AND IT IS RESTATED HERE
	 * BECAUSE THAT DAY'S CHANGE MADE READING IT THE OBVIOUS THING TO DO.** §2.11.2's tour now
	 * waits for each AI move's slide before showing the next action, so the interval is this
	 * field PLUS a slide duration -- and the tempting implementation multiplies
	 * `MoveTweenSeconds` by a route length right here. It does not.
	 * `AStratUnitActor::PlayRouteSlide` RETURNS the seconds it armed and `LastArmedSlideSeconds`
	 * holds the answer; this class adds what the actor TOLD it rather than predicting what the
	 * actor will do. Two computations of one duration is a clock that desynchronises from the
	 * picture the day a Blueprint changes that property, silently and with a green build.
	 *
	 * It is listed beside these two so that a reader tuning the
	 * game's pace finds all three from one place instead of two. It ships at zero for this
	 * field's own argument, restated locally in its block: the switch is in C++, the setting
	 * goes on `BP_StratUnit`.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stratocracy|AI")
	float AiPlaybackStepSeconds = 0.0f;

	// ---- §4.10 save identity ---------------------------------------------
	// The two header fields `FStratSaveIdentity` says are SUPPLIED and never recomputed --
	// "a bridge that went and read a manifest to fill these in would be asserting agreement
	// it is in no position to assert". They arrive here for exactly that reason: they are
	// build provenance, the GameMode's Blueprint default is the one place this project puts
	// build provenance a designer can see, and neither is a rules value.
	//
	// EMPTY IS A LEGAL AND SELF-CONSISTENT DEFAULT, and it is worth saying why rather than
	// leaving it to be discovered. A save written with both empty carries both empty, and a
	// load of that slot builds the same expectation from the slot's own copy, so
	// `strat::checkHeader` agrees. What is lost is only the cross-BUILD refusal: two builds
	// that both leave these empty cannot tell each other's slots apart on these two fields.
	// `scenarioHash` and `formatVersion` still refuse, and the replayed-`stateHash` check in
	// `FStratBridge::RestoreFromSaveText` refuses anything the definitions moved under.

	/** The crew commit the vendored `strat::` sources were taken at. See above on empty. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stratocracy|Save")
	FString RulesCommit;

	/** The digest over the §4.8 data set. See above on empty. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stratocracy|Save")
	FString DataHash;

	/**
	 * The slot `SaveMatchToSlot` / `LoadMatchFromSlot` use when passed an empty name.
	 *
	 * A SLOT NAME IS NOT AN ASSET PATH, which is why this is a defaulted string and not an
	 * `EditDefaultsOnly TObjectPtr`. `UGameplayStatics::SaveGameToSlot` writes
	 * `Saved/SaveGames/<name>.sav`; nothing in `Content/` is named, nothing is cooked, and a
	 * rename breaks no reference. The `/Game/` rule binds asset references and this is not
	 * one.
	 *
	 * ONE SLOT, BECAUSE THE GDD SCOPES ONE -- §4.4's IN list reads "minimal single-slot
	 * save/load". The methods still TAKE a name so a clause can use its own slot without
	 * clobbering a designer's, and empty means this.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stratocracy|Save")
	FString SaveSlotName = TEXT("StratocracyMatch");

	/**
	 * Whether the UNPROMPTED §2.11.6 writer may touch the disk -- `NoteMatchResultIfEnded`,
	 * and nothing else in this class.
	 *
	 * WHAT GAP THIS CLOSES, AND IT IS A DEFECT THAT ALREADY DID ITS DAMAGE ONCE.
	 * `NoteMatchResultIfEnded` used to gate on `ResolveSaveSlotName(FString()).IsEmpty()`, and
	 * its own comment said that arm existed for "an automation test that never configured a
	 * slot". `SaveSlotName` above defaults to `StratocracyMatch` -- THE PLAYER'S SLOT -- so a
	 * caller that never configured a slot resolved to the shipped one, and the guard was empty
	 * in exactly the case it was written for. Measured: the suite run in this worktree at
	 * 14:04 UTC on 2026-08-21 left `Saved/SaveGames/StratocracyMatch.sav`, 2096 bytes, written
	 * by an AI-vs-AI clause that never names a slot; the same suite in the integration tree at
	 * 13:38 UTC, without the writer, left that directory with zero files. A slot recording a
	 * completed match nobody played suppresses the guided opening PERMANENTLY -- the exact
	 * inverse of what the writer landed to fix.
	 *
	 * EMPTINESS IS THE WRONG PREDICATE FOR "NOBODY CHOSE THIS", and that is why this is a new
	 * field rather than a better-worded check. A slot name answers WHERE; it can never answer
	 * WHETHER, because a non-empty default cannot distinguish `unset` from `chosen`. Two
	 * questions, two fields.
	 *
	 * OFF IN C++, ON ONLY BY DEFAULT-OBJECT AUTHORSHIP. `AStratGameMode::MatchConfig` is an
	 * `EditDefaultsOnly` property whose Blueprint default is where shipping turns this true;
	 * every caller that builds an `FStratMatchConfig` in C++ -- which is every automation
	 * fixture, present and FUTURE -- inherits false and cannot reach a player's disk by
	 * forgetting something. The default points the safe way rather than the convenient way, so
	 * a fixture nobody has written yet is already covered. That is the property the
	 * empty-string guard never had: it protected only callers that remembered to opt OUT.
	 *
	 * A `TOptional<FString>` SLOT NAME WAS THE OTHER SHAPE AND IT CANNOT BE BUILT HERE.
	 * `TOptional` is not a reflectable `UPROPERTY` type, and this struct is `BlueprintType` and
	 * reaches a designer through a details panel -- so moving the unset/set distinction onto
	 * the string itself would cost the group its editability, which is the one thing this
	 * struct's own header block says it exists for.
	 *
	 * IT GATES ONLY THE UNPROMPTED WRITE. `RecordMatchCompletionOnSave` and `SaveMatchToSlot`
	 * remain unconditional, because a caller that named a slot has already chosen. This field
	 * answers a question only the hook has to ask: may I write somewhere nobody asked me to.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stratocracy|Save")
	bool bRecordCompletionOnMatchEnd = false;
};

// ---------------------------------------------------------------------------
// SEC 2.8's END OF MATCH, AS A PREDICATE OVER A MODEL AND NOTHING ELSE.
//
// WHAT GAP THESE TWO FUNCTIONS CLOSE. Measured 2026-08-23 in a human-driven PIE session and
// recorded in `Tools/architect/state/global.md`: the AI's turn-7 command killed side 0's flag,
// the rules module correctly declared the match over, this class correctly PERSISTED the
// completion three milliseconds later -- and then nothing left play. The AI's `EndTurn` was
// refused by the rules module (`[T-SAVE-05] no match is running`), so the turn number and the
// side to move stayed where they were, and the human went on issuing commands that the rules
// module ACCEPTED, as the AI's side. The engine detected the result and never acted on it.
//
// WHY A FREE FUNCTION OVER A MODEL AND NOT A METHOD ON THE SUBSYSTEM. The controller has
// already built the model it is about to act on before it can gate anything -- a gate written
// as `Match->IsMatchConcluded()` would build a SECOND model, and two models built at two
// instants are two answers to one question. Handed the caller's own model, the gate is the
// same value the screen was drawn from, by construction. It is also the shape a clause can
// drive: no world, no bridge, no PIE, and a planted `FStratViewModel` is enough to make it
// answer both ways.
//
// IT MIRRORS NOTHING AND LATCHES NOTHING. `FStratMatchView::bHasResult` is the rules module's
// own answer, copied by `StratBuildViewModel`; these functions read it and add no state of
// their own. That is deliberate and it is this class's standing rule -- see the note at the
// bottom of the class on why there is no `bSeeded` bool. A `bMatchOver` member beside the
// model could disagree with it, and the disagreement would present as exactly the defect
// above, wearing the fix's colours.
//
// NOT IN THESE, with reasons:
// - WHO WON -- RETIRED 2026-08-25, AND STAMPED IN PLACE RATHER THAN DELETED so that a reader
//   who remembers the debt can see which half was withdrawn. It used to read, whole:
//   RETRACTED> "WHO WON. `strat::UiMatchView` carries `turn`, `turnCap`, `sideToMove`,
//   RETRACTED>  `hasResult` and `resultTier` and NO winning side; `strat::MatchResult::
//   RETRACTED>  winner` lives on `TurnState` and reaches no projection. A victory SCREEN
//   RETRACTED>  needs it and this pass does not supply it -- see `engine.md` for the debt
//   RETRACTED>  and the two routes that could discharge it."
//   Neither of those two routes was taken. A THIRD one was, and it is the one the project
//   had already paid for once: upstream added `strat::uiMatchResult` as a FOURTH `ui*` query
//   beside `uiReachable`, `uiForecast` and `uiBuildOptions`, so no snapshot field moved, no
//   field-count constant moved, no `uiFieldContract()` row moved, and no T-UI-05 consumer
//   carries anything new. `FStratBridge::MatchResult` routes it, `FStratMatchResultView`
//   reflects it, and `GetMatchResult` below is this class's reader.
//   THE HALF THAT SURVIVES IS ABOUT THESE TWO FREE FUNCTIONS AND ONLY THEM: `UiMatchView`
//   still carries no winning side, so `StratMatchIsConcluded` and `StratMatchAcceptsCommands`
//   still cannot name one and still must not try. They answer WHETHER, off the model the
//   screen was drawn from; WHO is a query against the bridge and takes a different route on
//   purpose -- see `FStratMatchResultView`'s block on why it is not a view-model field.
// - THE TRANSITION'S SIDE EFFECTS. Clearing the pacing timer and logging the conclusion once
//   are `UStratMatchSubsystem::ConcludeMatchIfEnded`'s, because they are acts on an object.
//   A predicate that also did something would be unusable from the two callers that only want
//   to ask.

/**
 * Whether the model describes a match that has reached a Sec 2.8 result.
 *
 * ONE FIELD, READ, NEVER INFERRED FROM `ResultTier`. `FStratMatchView` says on the field
 * itself that the two are read separately so they cannot disagree, and a predicate that
 * asked `ResultTier != InProgress` instead would be a second author of the same fact.
 */
STRATPLAY_API bool StratMatchIsConcluded(const FStratViewModel& Model);

/**
 * Which side Sec 2.9's handicap moves for this configuration, or `INDEX_NONE` when it
 * moves none.
 *
 * THE WHOLE OF THE "SINGLE-PLAYER ONLY" RULING LIVES IN THIS FUNCTION, deliberately, so
 * that overturning it is one edit and so that a clause can assert the ruling directly
 * instead of inferring it from an opening purse. Three arms, and each is a different
 * configuration rather than three ways of saying one thing:
 *
 *   - `AiSides` EMPTY -> `INDEX_NONE`. A hot seat has no player-versus-opponent asymmetry
 *     for a handicap to express. This is `FStratMatchConfig`'s C++ default, and the
 *     configuration every fixture that leaves `AiSides` alone builds. IT IS NOT THE SHIPPED
 *     CONFIGURATION. [CORRECTED 2026-09-03; this line read, in the words on the next line:
 *     RETRACTED> "This is the shipped configuration."
 *     `BP_StratGameMode` authors `AiSides=(1)`, so the shipped game never reaches this arm
 *     -- see the third.]
 *   - `AiSides` CONTAINS `ViewingSide` -> `INDEX_NONE`. The seat the screen opens on is
 *     itself the AI, so either both sides are AI (phase D's AI-vs-AI gate, which must not
 *     move) or the configuration is inverted. Both fail inert rather than handicapping a
 *     computer.
 *   - OTHERWISE -> `ViewingSide`. THIS IS THE ARM THE SHIPPED GAME TAKES, and saying so is
 *     the whole of the 2026-09-03 repair: `BP_StratGameMode` authors `AiSides=(1)` with
 *     `ViewingSide` at its C++ default of 0, so both arms above fall through and Sec 2.9's
 *     handicap moves side 0 -- the human's. USER RULING, 2026-09-03: intended, because Easy
 *     is meant to help the player. See `FStratMatchConfig::Difficulty`'s own block for the
 *     ruling, and the AI block's preamble for whose measurement the authored value is.
 *
 * `ViewingSide` IS THE PLAYER'S SEAT AND NOT `FirstSide`. Its own declaration says it is
 * "which `strat` side the screen is drawn FOR at the start of the match"; in a
 * single-player match that is the human, whoever moves first. The alternative shape was to
 * take the complement of `AiSides` over the side count -- which is the same answer on two
 * sides and needs this module to learn a side count it deliberately does not name (see
 * `AiSides`' own "A LIST AND NOT TWO BOOLS"). That is what killed it.
 *
 * NOT RANGE-CHECKED, on `AiSides`' stated posture. A `ViewingSide` that names no real side
 * comes back from `FStratBridge::ApplyStartingFameHandicap` as that method's own refusal,
 * in its words, rather than as an upper bound invented here.
 */
STRATPLAY_API int32 StratHandicappedSide(const FStratMatchConfig& Config);

/**
 * Whether a human command may still be offered to the rules module for this model.
 *
 * FALSE MEANS INERT, NOT REFUSED. The distinction is the one Sec 2.11.6-B's input gates
 * already draw in `AStratPlayerController::HandleSelectionEvent`: nothing reaches
 * `FStratBridge`, no selection state moves, and `OutRefusalReason` is a sentence for a
 * human rather than a rules module's verdict.
 *
 * IT EXISTS BECAUSE THE RULES MODULE DOES NOT REFUSE THESE COMMANDS. Measured in the session
 * above: after the flag fell, `STRAT-CMD accepted kind=Move unit=14 ... side=1` and
 * `STRAT-CMD accepted kind=Attack unit=14 ... side=1` both landed. Only `EndTurn` carries the
 * `no match is running` check. So "the rules will stop it" was false, and this is the gate
 * that does.
 *
 * @param OutRefusalReason  reset on entry; written only when the answer is false.
 */
STRATPLAY_API bool StratMatchAcceptsPlayerCommands(const FStratViewModel& Model,
                                                   FString&               OutRefusalReason);

/**
 * The one sentence a concluded match refuses a command with.
 *
 * IT IS A FUNCTION AND NOT A LITERAL AT EACH SITE because there are three sites --
 * `StratMatchAcceptsPlayerCommands`, `AStratPlayerController::ToggleProductionMenu`'s open
 * path, and whatever a victory surface eventually says -- and a second spelling of the same
 * refusal is a second thing to keep in step. This project has already paid for that once,
 * with the `STRAT-AI` format string that drifted within a single diff.
 *
 * `FString` AND NOT `FText`, unlike `FStratGuidedOpening::EndTurnGateHoverText`. That one is
 * quoted verbatim from the GDD and reaches a widget; this one is engine prose that reaches a
 * log and a failure-reason channel, and §2.11.6's no-`LOCTEXT` ruling applies to neither.
 */
STRATPLAY_API FString StratMatchConcludedRefusalText();

/**
 * Owns the authoritative `strat::GameState` for one world, and reconciles the board, the
 * units and the scoreboard against it.
 *
 * ONE PER WORLD, created by the engine. Nothing spawns it and nothing may hold it past the
 * world's lifetime -- see the header block on why a GameInstance subsystem was refused.
 */
UCLASS()
class STRATPLAY_API UStratMatchSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UStratMatchSubsystem();

	// ---- Subsystem lifetime ----------------------------------------------

	/**
	 * Game and PIE worlds only.
	 *
	 * WITHOUT THIS, one of these exists for every editor preview world, every thumbnail
	 * render and every asset-editor viewport -- each holding a `TPimplPtr<FStratBridge>`
	 * that will never be seeded. They would be harmless and they would also be a fleet of
	 * objects claiming to own a match that does not exist, which is the kind of thing that
	 * later gets found by a search and treated as evidence.
	 */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	/** Destroys the bridge, the board and every unit actor. See the .cpp on why the HUD is
	 *  deliberately not told. */
	virtual void Deinitialize() override;

	// ---- The match -------------------------------------------------------

	/**
	 * Seeds a match and puts it on screen. The one entry point.
	 *
	 * THE SEQUENCE IS THE CONTRACT, and the header block records what each step buys:
	 * construct the bridge, map the two tables, seed the scenario -- ONLY NOW is the bridge
	 * seeded -- spawn the board, build the view model, reconcile, and only then hand the
	 * bridge to the scoreboard, which refuses an unseeded one.
	 *
	 * ALL-OR-NOTHING ON THE RULES SIDE. A failure before the scenario is seeded leaves this
	 * subsystem with NO bridge rather than a half-loaded one, because a half-loaded bridge
	 * reachable through `GetBridge()` invites a caller to read "not seeded" as "seeded and
	 * empty" -- the exact confusion `FStratBridge::MakeUiSnapshot` refuses to permit.
	 *
	 * NOT ALL-OR-NOTHING ON THE PRESENTATION SIDE, deliberately. A missing tile mesh or an
	 * unset unit class is reported in `OutFailureReason` and the match still runs: a
	 * configuration gap in phase 5's assets must not be able to prevent a seeded, correct
	 * match from existing, and the alternative is a refusal that reads exactly like a
	 * seeding failure and sends the next reader to the wrong file.
	 *
	 * IDEMPOTENCE IS NOT CLAIMED. Calling it twice reseeds -- a second
	 * `LoadScenarioFromFile` on the same bridge, which clears the recorded log by the
	 * bridge's own documented behaviour. There is one caller (`AStratGameMode::BeginPlay`)
	 * and it calls once.
	 *
	 * @param Config            from the GameMode's Blueprint defaults.
	 * @param OutFailureReason  the refusing layer's own words, forwarded unchanged. Empty
	 *                          on a clean start. A NON-EMPTY REASON WITH A `true` RETURN IS
	 *                          NOT POSSIBLE -- presentation gaps return false with the match
	 *                          live, and `IsMatchLive()` is how a caller tells the two apart.
	 */
	bool StartMatch(const FStratMatchConfig& Config, FString& OutFailureReason);

	/** True when there is a seeded bridge. Distinct from "the board drew" -- a match with a
	 *  missing tile mesh is live and incompletely drawn, and those have different fixes. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Match")
	bool IsMatchLive() const;

	/**
	 * The bridge this subsystem owns, or null before `StartMatch` succeeds.
	 *
	 * NOT A UFUNCTION AND IT CANNOT BE: `FStratBridge` is not a reflected type, and making
	 * it one would mean putting the vendored headers in front of UHT. This is the C++ seam
	 * phase 4's command loop reaches the five `Submit*` methods through.
	 *
	 * BORROWED, NEVER STORED. It dies with this subsystem, which dies with the world.
	 */
	FStratBridge* GetBridge() const;

	// ---- Presentation ----------------------------------------------------

	/**
	 * Builds the view model for the current viewing side, WITHOUT applying it.
	 *
	 * SPLIT FROM `ApplyView` FOR PHASE 4. The selection machine's path is build -> write
	 * `bDone` / `bLockedThisTurn` onto the built model -> apply, and it needs a model in its
	 * hand between the first and the third. `StratViewModel.h` records why those bits must
	 * go on the model and not into an actor: an actor's copy makes T-INT-05 false with no
	 * compiler diagnostic.
	 *
	 * It calls `StratBuildViewModel` and adds nothing. Refuses, with the builder's own
	 * words, rather than handing back an empty model -- an empty model is a legitimate value
	 * (a board with no hexes) and reconciling against one would destroy every actor on
	 * screen.
	 */
	bool BuildViewModel(FStratViewModel& OutModel, FString& OutFailureReason) const;

	/**
	 * Builds the view model AND runs the registered decorator over it. The only build this
	 * class applies from.
	 *
	 * THE INVARIANT IT EXISTS TO HOLD, stated as the thing a reader can check: no model this
	 * class builds reaches `ApplyView` without passing through here first. There are exactly
	 * two internal build-and-apply sites -- `StartMatchInternal`'s first reconciliation and
	 * `RefreshPresentation` -- and both call this. `BuildViewModel` above remains public and
	 * undecorated because two callers legitimately want a model they will NOT draw:
	 * `IsAiTurnDue` and `RunAiTurnsNow`'s loop read `sideToMove` and `bHasResult` off it, and
	 * decorating there would run `FStratGuidedOpening::Observe` -- documented as "THE ONLY
	 * THING THAT ADVANCES A BEAT" -- for a question nobody is drawing an answer to.
	 *
	 * IT IS BUILD-THEN-DECORATE AND NEVER DECORATE-THEN-BUILD. The decorator writes onto a
	 * model the rules module has just produced; a decoration that outlived a build would be
	 * the delta-shaped thinking `FStratViewModel` was written to exclude.
	 *
	 * WITH NO DECORATOR REGISTERED IT IS `BuildViewModel` EXACTLY, and that is a supported
	 * state rather than a degraded one: a world with no player controller -- every automation
	 * fixture that drives this subsystem directly -- has nobody to decorate, and the models it
	 * builds are correct and simply carry no guidance and no selection bits.
	 *
	 * PUBLIC SO THE INVARIANT IS PINNABLE. A clause can register a decorator, call this, and
	 * assert the section is present; with `RefreshPresentation` alone it could only observe
	 * the consequence on a widget.
	 */
	bool BuildViewModelForPresentation(FStratViewModel& OutModel, FString& OutFailureReason);

	/**
	 * Registers the one object allowed to complete a built model before it is drawn. See
	 * `FStratViewDecorator` for why it is a single, weak, non-reflected delegate.
	 *
	 * REPLACES WHATEVER WAS THERE, without complaint. A second controller taking over a world
	 * is a hand-over and not a conflict, and refusing the second binding would leave the
	 * screen decorated by an object that is on its way out.
	 */
	void SetViewDecorator(FStratViewDecorator InDecorator);

	/** Drops the registered decorator. Called from `AStratPlayerController::EndPlay`; safe
	 *  when nothing is bound. */
	void ClearViewDecorator();

	/**
	 * Whether anybody is registered to complete a model.
	 *
	 * FOR DIAGNOSIS AND FOR CLAUSES, never as a branch inside this class: every path here
	 * runs the decorator through `ExecuteIfBound` and behaves identically whether or not one
	 * is present, which is what keeps "no decorator" a configuration rather than a mode.
	 */
	bool HasViewDecorator() const;

	/**
	 * Makes the world look like this model. Spawn what is here and absent, move what is
	 * here and misplaced, destroy what is absent and present.
	 *
	 * PUBLIC ON PURPOSE -- it is phase 4's second half. See the header block.
	 *
	 * IT TAKES THE MODEL RATHER THAN READING A MEMBER, so that what is on screen is a
	 * function of the argument alone. A version that reconciled against a stored model would
	 * make "what should be drawn" depend on when someone last called a setter, which is the
	 * delta-shaped thinking `FStratViewModel` was written to exclude.
	 *
	 * THE MODEL IS CACHED AFTER APPLYING, as `GetViewModel()`, and that cache is a RECORD OF
	 * WHAT WAS APPLIED and never an input to anything. Nothing in this class reads it to
	 * decide what to draw.
	 *
	 * DOES NOT TOUCH THE SCOREBOARD. The HUD projects from the bridge itself and has its own
	 * refresh; driving it from here would give the panel two sources.
	 *
	 * IT DOES PUSH GDD Sec 2.11.6'S GUIDANCE STRIP, and the asymmetry with the scoreboard
	 * above is the reason rather than an inconsistency. `Model.Guidance` is a FIELD OF THIS
	 * ARGUMENT -- written by `FStratGuidedOpening::DecorateViewModel` on the decorated path,
	 * default-constructed and inactive on every other -- so pushing it here gives the strip
	 * ONE source, the same one the board and the units were drawn from. The scoreboard has
	 * its own projection off the bridge and would have gained a second; the strip has none
	 * and would otherwise have no route at all.
	 *
	 * IT IS HERE AND NOT IN `RefreshPresentation`, which is the load-bearing half of that
	 * choice: the push belongs with the reconciliation, so that every surface this class
	 * draws is drawn from one argument in one call.
	 *
	 * THE REASON GIVEN FOR THAT PLACEMENT HAS BEEN WITHDRAWN, and it is withdrawn in place
	 * because a reader who remembers the old claim needs to see it retracted and not
	 * silently absent. It used to read:
	 * RETRACTED> "`RefreshPresentation` is the UNDECORATED path; the decorated one --
	 * RETRACTED>  `AStratPlayerController`, build -> `Observe` -> decorate -> `ApplyView` --
	 * RETRACTED>  calls this function directly, and that is the only path on which guidance
	 * RETRACTED>  is ever non-default. A push in `RefreshPresentation` would therefore have
	 * RETRACTED>  been a push that never carried a live directive."
	 * That described the tree accurately and described a DEFECT. `RefreshPresentation` and
	 * `StartMatchInternal` reached this function with an undecorated model, so every
	 * subsystem-side reconcile cleared a live directive -- measured through `SetViewingSide`
	 * in PIE on 2026-08-21, see `FStratViewDecorator`. Both now build through
	 * `BuildViewModelForPresentation`, so there is no longer an undecorated path into this
	 * function from inside this class, and the premise the retracted sentence rested on is
	 * gone rather than merely inconvenient.
	 *
	 * WHAT DID NOT CHANGE, AND MUST NOT: an undecorated model applied FROM OUTSIDE -- which
	 * is what a clause driving `ApplyView` directly does -- still pushes an INACTIVE view and
	 * still clears the strip. The strip is a function of the argument, whatever the argument
	 * says. Reconciled, not evented -- the same property the unit set difference above has.
	 */
	void ApplyView(const FStratViewModel& Model);

	/**
	 * Build, decorate, apply, and refresh the scoreboard. The path a caller with nothing of
	 * its own to add uses.
	 *
	 * IT USED TO BE THE UNDECORATED PATH AND THAT WAS THE DEFECT, so the old first line is
	 * retracted in place rather than reworded away:
	 * RETRACTED> "Build, apply, and refresh the scoreboard. The undecorated path, and the
	 * RETRACTED>  one a caller with nothing to add uses."
	 * "Nothing to ADD" was read as "nothing to COMPLETE", and the two are not the same: this
	 * function's callers -- `SetViewingSide` and `RunAiTurnsNow` -- have no selection or beat
	 * of their own to contribute, but the model they draw from still has a guidance section
	 * that somebody has to fill in. It is filled in by the registered decorator, through
	 * `BuildViewModelForPresentation`.
	 *
	 * WRITTEN IN TERMS OF `BuildViewModelForPresentation` AND `ApplyView` so it cannot drift
	 * from `AStratPlayerController::RefreshFromMachine`, which calls the same two around its
	 * own overlay work. There is no third reconciliation implementation in this class and
	 * there must not be.
	 */
	bool RefreshPresentation(FString& OutFailureReason);

	/**
	 * The last model `ApplyView` was given.
	 *
	 * A RECORD, NOT A SOURCE. It is what is on screen, so a test or a debug overlay can ask
	 * without rebuilding; nothing in this class consults it.
	 *
	 * NOT A UFUNCTION, and by reference rather than by value for that reason: UHT does not
	 * accept a reference return, and returning a copy of a 99-hex model to satisfy
	 * reflection would make the cheap read expensive to discourage the only misuse it has.
	 * C++ callers -- phase 4, and a gate -- read it here.
	 */
	const FStratViewModel& GetViewModel() const { return AppliedModel; }

	/**
	 * §2.11.2's transient layer as of the last `ApplyView` -- GATE-TRANSIENT's surface.
	 *
	 * WHAT IT IS AND WHAT IT IS NOT. `StratTransientReceipts.h` carries the whole argument;
	 * the sentence that binds a caller here is that THE SCREEN MUST BE FULLY CORRECT WITH
	 * ZERO RECEIPTS DRAWN. Every number in it is a second reading of a durable home the view
	 * model already carries, so a widget that ignores this getter loses emphasis and loses no
	 * information. `Banner` is the exception in shape but not in principle: it is steady state
	 * derived from `FStratMatchView::SideToMove`, which is on screen anyway.
	 *
	 * IT IS THIS REFRESH'S, NOT THE MATCH'S. `ApplyView` overwrites it whole on every call,
	 * INCLUDING EMPTYING IT, so a consumer that polls slower than the refresh cadence will
	 * miss receipts. That is the correct trade for a layer whose whole content is emphasis --
	 * a queue that guaranteed delivery would be the event list `FStratViewModel` was written
	 * to exclude, and would make the screen depend on every previous frame.
	 *
	 * NOT A `UFUNCTION`, for `GetViewModel`'s stated reason -- UHT does not accept a reference
	 * return. This is the C++ and gate-facing read; `CopyTransientReceipts` below is the same
	 * value for Blueprint, and the two must stay one expression apart so they cannot drift.
	 */
	const FStratTransientReceipts& GetTransientReceipts() const { return LastReceipts; }

	/**
	 * The same value, by copy, for the widgets.
	 *
	 * BY VALUE AND CHEAPLY SO, which is why this exists where `GetViewModel` refuses the
	 * equivalent: that function returns a reference because copying a 99-hex model would make
	 * the cheap read expensive. This struct is a banner, two integers and a list that is empty
	 * on most refreshes, so the copy costs nothing and the reflection is worth having.
	 *
	 * IT IS WHAT KEEPS THE TOAST AND BANNER WIDGETS FREE OF ARITHMETIC. Every number a widget
	 * draws off this is a field, already decided; there is nothing left for it to compute,
	 * which is the state T-UI-03's no-widget-side-arithmetic clause describes.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Transient",
		meta = (DisplayName = "Get Transient Receipts"))
	FStratTransientReceipts CopyTransientReceipts() const { return LastReceipts; }

	/**
	 * The durable readings `ApplyView` will compare the next model against.
	 *
	 * EXPOSED SO A CLAUSE CAN SEE THE EDGE DETECTOR'S STATE, and for no other reason -- it is
	 * the input to a decision, not a description of the screen. `StratMarkFromView` is the
	 * only writer of a seeded value and it is a free function, so a clause needs no access to
	 * this to drive the decider; what it needs this for is asserting that the LIVE path
	 * re-marks, which is the half a world-free clause cannot reach.
	 */
	const FStratReceiptMark& GetReceiptMark() const { return ReceiptMark; }

	/**
	 * Changes which `strat` side the screen is drawn FOR, and rebuilds.
	 *
	 * THE HOT-SEAT HAND-OVER, and it MUTATES NO GAME STATE -- one `int32` here, one call to
	 * the HUD's own `SetViewingSide`, and a rebuild. It submits no command and does not
	 * touch the bridge. A view of a match is not a move in it.
	 *
	 * NOT `sideToMove`. Whose turn it is comes from the snapshot; whose screen this is comes
	 * from here, and `StratScoreboardHUD.h` is explicit that conflating them is the bug the
	 * split exists so nobody has to commit.
	 *
	 * TWO FAILURE MODES, AND THEY ARE DISTINGUISHED BY WHERE THE ASSIGNMENT SITS RATHER THAN
	 * BY A COMMENT. This paragraph used to state only the second of them, and the code
	 * conflated them; corrected 2026-08-25.
	 *
	 *   - A REFUSED SIDE CHANGES NOTHING, ANYWHERE. The range check runs BEFORE the
	 *     assignment, so an out-of-range hand-over leaves this class's `ViewingSide` and the
	 *     HUD's exactly as it found them. The old order assigned first and then forwarded, so
	 *     a side the HUD REJECTED was left held here -- and every later refresh then failed
	 *     inside `StratBuildViewModel` with a reason that named the builder rather than the
	 *     hand-over. That was the defect, and it was reachable from Blueprint and the console
	 *     the whole time.
	 *   - ON A FAILED REBUILD THE SIDE HAS STILL CHANGED, matching `AStratScoreboardHUD::
	 *     SetViewingSide` exactly and unchanged from the original claim: rolling back would
	 *     make a hand-over silently stay with the previous player, which is the one outcome a
	 *     hot-seat game must not produce quietly. The rebuild runs AFTER the assignment,
	 *     which is what makes that property structural instead of documented.
	 *
	 * STILL NO RANGE CHECK OF ITS OWN, and that is unchanged: a constant named here would be
	 * a third authority that can disagree with the two that already own the check. What moved
	 * is only WHEN the existing authority is consulted. With a HUD it is the HUD's check; with
	 * NO HUD -- a legitimate configuration, and one no check covered before -- it is
	 * `StratBuildViewModel`'s own, asked as a trial build at the candidate side before
	 * anything is committed. The .cpp states which branch is which.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Match")
	bool SetViewingSide(int32 InViewingSide, FString& OutFailureReason);

	/** Which `strat` side the screen is drawn for. A reader and nothing else. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Match")
	int32 GetViewingSide() const { return ViewingSide; }

	// ---- Sec 2.11.5's production menu --------------------------------------
	//
	// WHY THIS CLASS AND NOT THE HUD, which was the other candidate and is the closest
	// prior art in the tree. `AStratScoreboardHUD` has a bridge, already hosts a second
	// widget (the Sec 2.11.6 strip) and already exposes a refresh in exactly this shape, so
	// the READ half would have fitted there unchanged. The WRITE half decided it, twice
	// over:
	//   1. AN ACCEPTED BUILD PUTS A UNIT ON THE BOARD, and the only thing in this project
	//      that can make the board agree with that is `ApplyView`, which is here. A submit
	//      routed through the HUD could refresh the scoreboard panel and would leave the new
	//      unit unspawned until something else happened to reconcile -- a screen carrying a
	//      change the view model it was drawn from did not, which is the delta-shaped
	//      thinking `FStratViewModel` was written to exclude.
	//   2. THE HUD IS SHEDDING ITS BRIDGE, NOT GROWING ONE. `StratScoreboardHUD.h` states in
	//      advance that it must give up bridge ownership once a proper owner exists; this
	//      class IS that owner and `AdoptBridge` is the handover. Hanging a MUTATING rules
	//      call off the borrower would deepen the dependency that debt exists to discharge.
	//
	// AND NOT A `UBlueprintFunctionLibrary`, which was the third shape considered and is
	// structurally impossible rather than merely worse: every static there would need an
	// `FStratBridge&` parameter, `FStratBridge` is not a reflected type and must never
	// become one, so no Blueprint graph could ever supply the argument.
	//
	// THE WIDGET THEREFORE OWNS NO RULES AND HOLDS NO BRIDGE. A WBP reaches this with `Get
	// World Subsystem`, binds to `ProductionMenu`, and calls the two functions below. It
	// derives from nothing of ours -- which also means no `/Script/Module.Class` parent is
	// baked into an asset before anybody has decided who owns the menu widget, and that
	// choice is irreversible once an asset makes it.

	/**
	 * Rebuilds Sec 2.11.5's buildlist for ONE factory, for the side whose screen this is.
	 *
	 * THE SIDE IS `ViewingSide` AND IS DELIBERATELY NOT A PARAMETER. A side argument would
	 * let a widget ask what the OTHER player could buy, which in a hot-seat game sharing one
	 * screen is the whole of the information it must not leak. Whose TURN it is remains a
	 * different question and remains the rules module's: asked out of turn the rows come
	 * back with `bAvailable` false and the module's own reason, and this class asserts
	 * nothing about it either way.
	 *
	 * ALL-OR-NOTHING, inherited from `StratBuildProductionMenu` and re-stated here because
	 * the hex beside the rows is this class's and not the builder's: a refusal leaves BOTH
	 * `ProductionMenu` and `ProductionMenuHex` exactly as it found them, so a transient
	 * failure cannot blank a menu that was correct a moment ago.
	 *
	 * TWO CHANNELS, AND THIS FUNCTION ROUTES BOTH WITHOUT COLLAPSING EITHER. `FStratBridge::
	 * BuildOptions` states the split in its own words: `FStratResult` says whether the query
	 * could be ASKED, and `bAvailable` / `Reason` on each row say what the rules ANSWERED.
	 *   - ON THE REFUSAL CHANNEL, so `false` from here: a bridge with no definitions loaded or
	 *     no scenario seeded; a SIDE outside the match -- the one malformed-question case,
	 *     refused in the bridge rather than left to the module because `uiBuildOptions` spells
	 *     "invalid side" the same way it spells "not a build point"; and an EMPTY SET, which is
	 *     unreachable with a unit table loaded, since the menu carries one row per Sec 2.4 row.
	 *   - ON EACH ROW, so `true` from here with a full menu: a factory this side does not hold,
	 *     one that has already taken its build this turn, one already holding a waiting build,
	 *     and A HEX THAT IS NOT A BUILD POINT AT ALL. THE FACTORY HEX IS NOT PRE-CHECKED --
	 *     not by the bridge and not here -- the way `Forecast` leaves the defender to the module.
	 *
	 * SO A NON-FACTORY HEX OPENS A MENU RATHER THAN REFUSING ONE, and every row of it reads
	 * `bAvailable` false carrying the module's own reason. This paragraph claimed the opposite
	 * and is RETRACTED IN PLACE, because a reader who remembers the old claim needs to see it
	 * withdrawn and not silently absent:
	 * RETRACTED> "A REFUSAL IS NOT AN EMPTY MENU, and nothing on this path converts one into
	 * RETRACTED>  the other. A hex that is not a factory is REFUSED, in the bridge's own words."
	 * The half that survives is the half about EMPTINESS, and it survives unchanged: an empty
	 * array is never an answer to this question -- which is exactly what lets
	 * `IsProductionMenuOpen()` read the rows below. Nothing on this path converts a refusal into
	 * an empty menu, or an empty menu into a refusal.
	 *
	 * A CALLER ASKING "IS THIS HEX A FACTORY" MUST READ THE ROWS AND NOT THE RETURN VALUE, and
	 * Sec 2.11.5 shows those reasons to the player, so there is nothing here for this class to
	 * decide and no hex test for it to grow.
	 *
	 * ROW ORDER IS THE BRIDGE'S AND IS NEVER SORTED. `DefIndex` is a raw, bounds-checked-only
	 * index into the definitions vector (`FStratBridge::SubmitBuildAtHex` records why), so a
	 * reordering here would not be cosmetic -- it would be a different unit built.
	 *
	 * @param FactoryHex        X = q, Y = r -- the encoding `FStratFactoryView::Hex` already
	 *                          carries, so a widget passes the view model's own value back in.
	 * @param OutFailureReason  the refusing layer's own words, forwarded unchanged.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Production")
	bool RefreshProductionMenu(FIntPoint FactoryHex, FString& OutFailureReason);

	/**
	 * Submits the chosen row as a Sec 4.10 Build at the factory the OPEN MENU is for.
	 *
	 * IT TAKES NO HEX, AND THAT IS THE ONE GUARD THIS FUNCTION ADDS. `DefIndex` is meaningful
	 * only as a row of the menu it was picked from; pairing a caller-supplied hex with it
	 * would let a widget choose row 2 of factory A and build it at factory B, and the rules
	 * module would accept that silently because `applyCommand` uses the field as a raw index
	 * and never as a name. Taking the open menu's own hex removes the pairing rather than
	 * checking it.
	 *
	 * IT DECIDES NO LEGALITY. `bAffordable` and `bAvailable` are NOT consulted here and must
	 * not be: whether this side can pay, and whether this factory will take a build, are the
	 * rules module's answers and it gets to give them again below in its own words. The only
	 * thing refused locally is a `DefIndex` appearing in NO row of the open menu -- a value
	 * the module never OFFERED, which is a different question from a value it offered and
	 * would refuse.
	 *
	 * REFUSES DURING AN AI TURN, for `LoadMatchFromSlot`'s neighbouring reason: `RunAiTurnsNow`
	 * is submitting into this same bridge, and interleaving a player command into that loop
	 * would put two authors' commands into one recorded log.
	 *
	 * ON A REFUSAL NOTHING MOVES -- no command, no menu change, no redraw.
	 *
	 * ON ACCEPTANCE THE MENU AND THE SCREEN ARE BOTH REBUILT, in that order, because the build
	 * changed both: the factory has now built this turn and the fame has been spent, so every
	 * row is stale; and there is a unit in the rules with no actor. REBUILT AND NEVER PATCHED,
	 * and reconciled through `RefreshPresentation` rather than by spawning anything here.
	 *
	 * IT DOES NOT CLOSE THE MENU. Whether the panel stays up after a build is a Sec 2.11.5
	 * presentation question; `CloseProductionMenu` is the widget's answer to it, and closing
	 * here would decide it in C++ for every skin.
	 *
	 * A `false` RETURN AFTER AN ACCEPTED COMMAND IS POSSIBLE, and it is stated rather than
	 * designed away -- it is the same trade `SetViewingSide` documents about its own
	 * already-changed side. The command is recorded and irreversible by the time the rebuild
	 * runs, so a rollback is not on offer; what a caller gets is the rebuild's reason, prefixed
	 * with "build accepted". `FStratBridge::RecordedCommandCount` is the disambiguator for a
	 * caller that must tell "the build did not happen" from "the build happened and the redraw
	 * did not".
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Production")
	bool SubmitProductionChoice(int32 DefIndex, FString& OutFailureReason);

	/**
	 * Closes the menu: empties the rows and forgets the factory.
	 *
	 * VOID AND UNREFUSABLE. Closing a menu that is not open is the state the caller asked for,
	 * not a failure, and there is nothing a caller could do with the difference.
	 *
	 * IT SUBMITS NOTHING AND CANCELS NOTHING. There is no Sec 4.10 command for "changed my
	 * mind": a build either was submitted or was not, and closing the panel is neither.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Production")
	void CloseProductionMenu();

	/**
	 * Whether a menu is open -- i.e. whether `ProductionMenu` describes a factory.
	 *
	 * IT READS THE ROWS AND IS NOT A SECOND BOOL BESIDE THEM, on this class's standing rule
	 * against a mirror that can disagree with the thing it mirrors (see the absent `bSeeded`
	 * at the bottom of this file). What makes that sound is a guarantee one layer down and not
	 * a convention here: `StratBuildProductionMenu` REFUSES rather than producing an empty
	 * menu, so an empty array is not a value any successful refresh can produce and is
	 * therefore free to mean "none". `ProductionMenuHex` could NOT have carried the marker --
	 * `FIntPoint(0, 0)` is a real hex, and this project has already paid once for treating a
	 * real default as an unset marker.
	 *
	 * IT ANSWERS "A MENU IS OPEN" AND NEVER "THAT HEX IS A FACTORY", and the two come apart on
	 * a real input. A refresh aimed at a hex that is not a build point SUCCEEDS -- the factory
	 * hex is not pre-checked anywhere on this path, see `RefreshProductionMenu`'s two-channel
	 * block -- so this reads true over a full menu whose every row is `bAvailable` false with
	 * the module's own reason. That is the state Sec 2.11.5 draws, reasons and all;
	 * `SubmitProductionChoice` does not consult those bits, so a build attempted from such a
	 * menu is refused by the rules module in its own words and never by this class.
	 *
	 * WHAT WOULD BREAK IT: a rules change that made an empty buildlist a legal answer. The
	 * cost if it broke is a refused submit, not a wrong build.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Production")
	bool IsProductionMenuOpen() const { return !ProductionMenu.IsEmpty(); }

	/**
	 * §2.11.5's boxed-in footer, as ONE boolean about the OPEN menu's factory: is
	 * every hex at or adjacent to it occupied, so a build submitted here waits?
	 *
	 * TRUE IS NOT A REASON TO GREY A BUILD BUTTON, AND Q31 IS WHY. Ruled 2026-08-22:
	 * a player MAY queue into a boxed-in factory, and `buildWaiting` is the mechanism
	 * -- the same one the AI path already uses. `Ui.h` states at the field that this
	 * value is INFORMATIONAL and that `uiBuildOptions` must not fold it into
	 * availability. So the footer swaps and the Build buttons STAY ENABLED. A binding
	 * that ANDed this into a button's `bIsEnabled` would re-decide, widget-side, a
	 * question the rules module deliberately declined to fold in.
	 *
	 * IT IS ON THE ROWS' CLOCK BY CONSTRUCTION, AND THAT IS THE WHOLE OF THE DESIGN
	 * HERE. `RefreshProductionMenu` asks the bridge for this in the same call, from the
	 * same `FStratBridge`, and publishes it in the same all-or-nothing move that
	 * publishes `ProductionMenu` and `ProductionMenuHex`; `CloseProductionMenu` clears
	 * it beside them. So the footer and the rows above it are fresh together or stale
	 * together, and CANNOT DISAGREE -- which is the property a footer needs, because a
	 * footer contradicting the rows it sits under is a defect and not a cosmetic lag.
	 *
	 * IT IS DELIBERATELY NOT READ OFF `AppliedModel.Factories`, AND THE CLOCKS WERE
	 * MEASURED RATHER THAN ASSUMED. That array carries this exact field, and reading it
	 * would have cost nothing. It was rejected on three findings, in this tree:
	 *   - `AppliedModel` IS WRITTEN ONLY BY `ApplyView` and `RefreshProductionMenu`
	 *     writes nothing else, so the two are independent by construction rather than
	 *     merely usually in step.
	 *   - THE DIVERGENCE IS REACHABLE ON THE SEQUENCE THIS FOOTER IS ABOUT. In
	 *     `SubmitProductionChoice` an accepted build rebuilds the MENU first and the
	 *     SCREEN second, and a failed `RefreshPresentation` leaves the rows fresh with
	 *     `AppliedModel` untouched -- a case that function's own block states rather
	 *     than designs away ("A `false` RETURN AFTER AN ACCEPTED COMMAND IS
	 *     POSSIBLE"). `RunAiTurnsNow` has the same shape. The build that just filled
	 *     the factory's last free hex is precisely the build that flips this bit.
	 *   - `AppliedModel` IS "WHAT WAS DRAWN" AND NOT "WHAT THE RULES SAY". `ApplyView`
	 *     is public and takes the model as its argument on purpose, so a caller may
	 *     apply a hand-built one; `GetViewModel`'s own block calls the cache "a record,
	 *     never an input". A footer sourced there would be a function of the last
	 *     argument somebody passed.
	 * A fourth finding decided the shape even where the clocks agree: a menu opened on
	 * a hex that is NOT a build point succeeds (see `IsProductionMenuOpen`), and
	 * `AppliedModel.Factories` then holds no entry for `ProductionMenuHex` at all, so
	 * the applied model cannot answer that reachable state even in principle.
	 *
	 * FALSE WHEN NO MENU IS OPEN, AND THE OPENNESS TEST IS PART OF THE ANSWER RATHER
	 * THAN A GUARD IN FRONT OF IT -- see the .cpp. A `BlueprintPure` cannot refuse, and
	 * of the two values available `false` is the one that claims nothing: it means "do
	 * not show the boxed-in footer", which is what a closed panel wants. `true` would
	 * be a positive claim about a factory nobody is looking at.
	 *
	 * FALSE WHEN THE OPEN MENU'S HEX IS NOT A BUILD POINT, for the same reason one
	 * layer down. `FStratBridge::FactorySpawnBlockedAt` REFUSES that question rather
	 * than answering it, and a refusal is not a yes. What the player sees in that state
	 * is the rows, every one of them `bAvailable` false carrying the rules module's own
	 * reason, which is the sentence §2.11.5 wants there and is not this one.
	 *
	 * IT SEARCHES NOTHING AND COMPARES NO HEX. The widget is handed one boolean about
	 * the factory whose menu is open. Binding a footer to `Factories` and letting the
	 * widget find its own entry would put a hex comparison in a Blueprint, which is
	 * T-UI-03's forbidden widget-side derivation wearing a different hat.
	 *
	 * WHAT IT DOES NOT SAY: whether anything is QUEUED here. That is `bBuildWaiting`,
	 * a different field for a different sentence, and `strat::UiFactoryView`'s own
	 * block names the case that separates them -- a boxed-in factory with nothing
	 * queued has this true and that false. `bBuildWaiting` has no Blueprint route yet
	 * and this pass deliberately does not give it one.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Production")
	bool IsOpenMenuFactorySpawnBlocked() const;

	/**
	 * Sec 2.11.5's rows for the open factory, IN THE ORDER THE RULES MODULE RETURNED THEM.
	 *
	 * THE ONE THING A WIDGET BINDS TO. `BlueprintReadOnly` and never writable FROM A
	 * BLUEPRINT, which is the claim that carries the reason -- a menu a Blueprint could also
	 * write is a menu with two authors and no way to tell which one the player is looking at.
	 *
	 * THE SENTENCE USED TO SAY "there is exactly one writer, `RefreshProductionMenu`", AND
	 * THAT IS NOW FALSE OF THE TREE. Stamped rather than deleted, because the reason it was
	 * written for is untouched and only the unqualified quantifier went stale. What is
	 * actually true, measured 2026-08-25:
	 *   - IN SHIPPING CODE the count is right and the writers are two halves of one pair, both
	 *     in this class's .cpp: `RefreshProductionMenu` fills the rows and the hex together,
	 *     and `CloseProductionMenu` clears them together. Nothing else in `Source/` outside
	 *     `Tests/` assigns either member.
	 *   - IN AUTOMATION `Source/StratPlay/Tests/StratProductionMenuSeam.cpp` assigns both
	 *     members directly, to re-plant a menu a deliberate reseed had cleared so that a
	 *     clause can measure what a REFUSAL does to it. It is C++ in this module and
	 *     `BlueprintReadOnly` does not hold it out; the plant is declared at its own site and
	 *     is exactly the access this specifier was never meant to prevent.
	 * SO THE INVARIANT A WIDGET AUTHOR MAY RELY ON IS THE ONE ABOUT BLUEPRINTS, not the one
	 * about the total number of writers. A future reader who needs "one writer" to be true
	 * should read it as "one writer reachable from anything the player can touch".
	 *
	 * NOTHING IN A ROW MAY BE RECOMPUTED, COMBINED OR RE-DERIVED. `bAffordable` and
	 * `bAvailable` are separate answers deliberately -- "you are still saving for this" and
	 * "this factory has already built this turn" -- and Sec 2.11.5 shows them differently.
	 * ANDing them in a binding is T-UI-03's forbidden widget-side arithmetic and would
	 * collapse two states the player must be able to tell apart.
	 */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Production")
	TArray<FStratBuildOptionView> ProductionMenu;

	/**
	 * The factory `ProductionMenu` is for. X = q, Y = r.
	 *
	 * MEANINGLESS WHILE `IsProductionMenuOpen()` IS FALSE, and it is published anyway so a
	 * widget can label the panel with the factory it is showing without keeping a second copy
	 * of the hex it passed in. Read it only alongside the rows.
	 */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Production")
	FIntPoint ProductionMenuHex = FIntPoint(0, 0);

	/**
	 * Sec 2.11.5's header number: the purse the rows above were priced against.
	 * `FACTORY -- BUILD   Fame: 250`.
	 *
	 * IT RIDES THE ROWS' CLOCK, WHICH IS THE ONLY REASON IT IS HERE RATHER THAN READ OFF THE
	 * VIEW MODEL. `FStratViewModel::Sides[ViewingSide].FameTotal` is the same quantity, and a
	 * widget binding to it would be reading it at a different instant from the one every
	 * `FStratBuildOptionView::Shortfall` beside it was computed against. The two would then be
	 * able to disagree on screen -- a header saying `Fame: 250` above a 275-cost row saying
	 * `need 50` -- while each was individually correct. `StratBuildProductionMenu`'s
	 * six-argument overload returns the very local the shortfalls used, and this member is
	 * where it lands.
	 *
	 * SAME LIFETIME AND SAME STATEMENT GROUP AS THE ROWS. Assigned in `RefreshProductionMenu`
	 * with `ProductionMenu` and `ProductionMenuHex`, cleared in `CloseProductionMenu` with
	 * them, and MEANINGLESS WHILE `IsProductionMenuOpen()` IS FALSE for the reason the hex
	 * above gives: a value left behind an emptied container reads like live state to anyone
	 * who consults it without consulting the container first.
	 *
	 * IT IS THE PURSE OF `ViewingSide`, because that is the side `RefreshProductionMenu` opens
	 * every menu for. It is deliberately NOT re-derived from `Match.SideToMove`, which in
	 * hot-seat is a different side on every other screen.
	 */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Production")
	int32 ProductionMenuFameTotal = 0;

	// ---- §4.10 save slots ---------------------------------------------------
	// THIS CLASS OWNS WHERE A SAVE LIVES, and it is the only thing in the tree that does.
	// `FStratBridge::SerializeRecordedSave` says of itself "SERIALIZES, AND DOES NOT WRITE
	// ... where a save lives is the save-slot UI's question", and `Save.h` keeps save part
	// (a)'s dependency set empty by having "no I/O beyond reading the one file". The disk
	// is here, and it is here rather than in the HUD for the reason the header block gives
	// for the bridge itself: a HUD is client-local and per-player, and there is exactly one
	// match to save.
	//
	// `UGameplayStatics` AND NOT `FFileHelper`, deliberately. A raw text file beside the
	// project would be the shorter path and would give up the two things a slot needs: the
	// platform's own save location (`Saved/SaveGames/`, redirected per platform) and a
	// versioned payload object. `UStratSaveGame` carries six engine-side facts §4.10 has no
	// field for -- see that header -- and a bare text file could carry none of them.
	//
	// USER INDEX 0, HARDCODED, AND THAT IS A RULING RATHER THAN A SHORTCUT. §2.11 is
	// hot-seat: two players, one machine, one logged-in user. A per-side user index would
	// model two platform accounts this game has no notion of, and would silently split one
	// hot-seat match's saves across two directories at the hand-over. If split-screen or
	// online profiles ever arrive, this constant is where they land and it is one place.

	/**
	 * Writes the live match to a slot. `SlotName` empty means `FStratMatchConfig::SaveSlotName`.
	 *
	 * REFUSES RATHER THAN WRITING A DEGENERATE SLOT. No live match, an unseeded bridge, or a
	 * serializer refusal all return false with the bridge's own reason -- a slot that loads
	 * back into an empty board is worse than no slot, because it looks like a save.
	 *
	 * IT STILL DOES NOT SET `bHasCompletedAMatch`, AND THAT IS NOW A DIVISION OF LABOUR
	 * RATHER THAN A GAP. Whether the match is over is `strat::buildUiSnapshot`'s answer and
	 * reaches the §4.10 text through `result`; §2.11.6's onboarding bool is a different fact
	 * -- the SLOT'S HISTORY -- and this method deliberately does not infer one from the
	 * other, because a save taken on the final turn of a match the player then abandons is
	 * not a completed match. Two paragraphs of this block used to read:
	 * RETRACTED> "IT DOES NOT SET `bHasCompletedAMatch`, AND NOTHING ELSE DOES EITHER."
	 * RETRACTED> "THE CONDITION THAT DISCHARGES THAT ... a caller that sets the field when a
	 * RETRACTED>  match reaches a result."
	 * That condition is met: `RecordMatchCompletionOnSave` is the writer and
	 * `ApplyView` is the caller, on the model's own `FStratMatchView::bHasResult`.
	 * The half of the sentence that survives is the half about THIS method -- the inference
	 * is still refused here, and the write still happens at the moment the match reaches a
	 * result rather than at the moment a save is taken.
	 *
	 * SO THE TWO WRITERS OF ONE SLOT DO NOT COLLIDE. This method writes every field EXCEPT
	 * the onboarding pair; `RecordMatchCompletionOnSave` writes ONE onboarding field and no
	 * other. Each read-modify-writes the existing payload, so whichever runs second keeps
	 * what the first put there.
	 *
	 * IT CARRIES FORWARD whatever an existing slot already held for both onboarding fields,
	 * so a save over a slot does not erase guidance state -- which is what keeps the debt
	 * above a missing writer rather than a lossy one.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Save")
	bool SaveMatchToSlot(const FString& SlotName, FString& OutFailureReason);

	/**
	 * Restores a slot into this subsystem: re-seeds, replays, and reconciles the screen.
	 *
	 * IT RUNS `StartMatch`'s SEQUENCE AND NOT A SECOND ONE. Both go through the private
	 * `StartMatchInternal`, which is that function's body with one extra step: after
	 * `LoadScenarioFromFile` and before the hand-over, an optional
	 * `FStratBridge::RestoreFromSaveText`. There is exactly one ordering of LoadDefinitions
	 * -> LoadScenarioFromFile -> (restore) -> AdoptBridge in this file, which is what the
	 * header block means by calling that ordering a contract rather than a habit. A second
	 * sequence beside it is the shape that produces two `strat::GameState`s for one map.
	 *
	 * PRESENTATION IS RECONCILED THROUGH THE ONE REFRESH PATH. `StartMatchInternal` ends in
	 * `BuildViewModel` -> `ApplyView` and then `HandBridgeToScoreboard`, exactly as a fresh
	 * match does; nothing here draws anything of its own. T-INT-05's "rebuild the screen
	 * from the view model alone" therefore covers a loaded match for free, which it would
	 * not if a load had its own drawing path.
	 *
	 * THE TABLES AND ACTOR CLASSES COME FROM `ActiveConfig`, NOT FROM THE SLOT, and the slot
	 * overrides exactly three fields: `ScenarioFile`, `FirstSide` and `ViewingSide` -- the
	 * three §4.10 cannot carry (`UStratSaveGame`'s header block, items 1 and 2, plus the
	 * hot-seat viewing side). A slot that pinned `DT_Units` would break the first time that
	 * asset was renamed, and the definitions are already checked, harder, by the
	 * replayed-`stateHash` comparison inside `RestoreFromSaveText`.
	 *
	 * SO IT REQUIRES A CONFIGURED SUBSYSTEM. `StartMatch` must have run at least once --
	 * which `AStratGameMode::BeginPlay` does -- or there are no tables to seed from, and
	 * this refuses by name rather than seeding from a default-constructed config.
	 *
	 * REFUSES DURING AN AI TURN. `RunAiTurnsNow` submits into the bridge this call frees.
	 *
	 * ON A REFUSAL THE PREVIOUS MATCH IS GONE, and that is a limitation stated rather than a
	 * property designed. The sequence tears down and rebuilds, so a slot that fails its
	 * header or its hash check leaves NO match rather than the one that was on screen.
	 * Validating first would need the load checked against a bridge that does not exist yet
	 * -- a second seeded bridge, which is the thing this class exists to make impossible.
	 * `IsMatchLive()` is false afterwards and the caller must start a new match. The
	 * condition that discharges this: a `FStratBridge` that can be constructed, seeded and
	 * restored detached and then SWAPPED IN whole. `ReplayRecordedLogOnto` is already that
	 * shape on the write side, so the seam exists; nothing needs it yet.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Save")
	bool LoadMatchFromSlot(const FString& SlotName, FString& OutFailureReason);

	/**
	 * Whether a slot EXISTS. `SlotName` empty means `FStratMatchConfig::SaveSlotName`.
	 *
	 * A MENU MUST NOT ASK THIS TO DECIDE WHETHER TO OFFER CONTINUE. A file exists after any
	 * completed match, saved or not, because `ApplyView` records §2.11.6's completion flag
	 * into the same slot with an empty `SaveText`. The question a menu is actually asking is
	 * `DoesSlotHoldARestorableMatch`.
	 *
	 * THE PARAGRAPH ABOVE PREVIOUSLY BLAMED THIS FUNCTION'S OWN DOC FOR THE DEFECT AND WAS
	 * WRONG AT BOTH ENDS. It is corrected rather than deleted, because it was briefly the
	 * tree's account of why a player-visible defect shipped and a reader who followed it was
	 * sent to the wrong declaration.
	 *   RETRACTED>  "this sentence is here because the doc that stood in its place invited
	 *   RETRACTED>   exactly that -- it read "Exposed so a menu can label the button", and
	 *   RETRACTED>   §2.11.5's menu took the invitation and shipped a defect."
	 * (1) THAT SENTENCE WAS NEVER ON THIS FUNCTION. At `347c722` this doc read, in full:
	 * "Whether a slot exists. `SlotName` empty means `FStratMatchConfig::SaveSlotName`." --
	 * neutral, and it invited nothing. The quoted sentence is `ResolveSaveSlotName`'s, one
	 * declaration below, and it is still there and still fine: labelling a button with a slot
	 * NAME is not gating an affordance on a slot's CONTENTS.
	 * (2) THE MENU NEVER CALLED THIS FUNCTION. `DoesSaveSlotExist` has exactly one
	 * non-declaring call site in the tree and it is a clause. What the shell asked was
	 * `UGameplayStatics::DoesSaveGameExist` directly -- the same weaker question, one layer
	 * down, reached without passing this declaration at all. (That prior shell state is
	 * attested by the agents who read it and is NOT re-derivable from a checkout: the file
	 * was untracked when it held that call. Said here rather than asserted as though the
	 * repository could show it.)
	 * THE REAL ACCOUNT LIVES ON `IsPayloadRestorable` BELOW and is not restated here.
	 *
	 * IT IS KEPT, NOT NARROWED, because "does a file occupy this slot" is a real question with
	 * other askers -- an overwrite prompt is the obvious one -- and folding it into the
	 * stronger question would leave no way to ask the weaker one.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Save")
	bool DoesSaveSlotExist(const FString& SlotName) const;

	/** The slot a name of `""` resolves to. Exposed so a menu can label the button. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Save")
	FString ResolveSaveSlotName(const FString& Requested) const;

	/**
	 * Whether a loaded payload carries a match THIS BUILD CAN RESTORE.
	 *
	 * WHY IT WAS EXTRACTED, AND IT IS A PLAYER-VISIBLE DEFECT AND NOT A TIDY-UP. `DoesSaveSlotExist`
	 * answers "a file is there". §2.11.5's menu read that as "a match can be restored" and
	 * the two came apart the moment anything else wrote to the slot -- which
	 * `ApplyView` does, recording §2.11.6's completion flag through
	 * `RecordMatchCompletionOnSave` on a match the player never saved. That writer creates a
	 * payload whose `SaveText` is EMPTY and stamps `SavedDataVersion` with the current
	 * constant, so the file exists, the version gate cannot catch it -- `UStratSaveGame`
	 * initialises that member TO the current version, so a freshly created payload is always
	 * current -- and `LoadMatchFromSlot` refuses one line later on `SaveText.IsEmpty()`. A
	 * player who finished a match without saving was OFFERED Continue and silently given
	 * turn 1. Nothing was lost or overwritten; the offer was simply false.
	 *
	 * AND THE TREE HAD ALREADY WRITTEN THAT DOWN, IN CURRENT PROSE, BEFORE W6 EXISTED. This is
	 * the half of the cause that matters and it is stated second only because it is worse.
	 * `StratSaveGame.h` -- unmodified by this branch, so this is HEAD -- carries it under the
	 * heading ONE CONSEQUENCE, WRITTEN DOWN RATHER THAN DISCOVERED:
	 *     "the writer CREATES a slot that does not exist, so a player who finishes a match
	 *      without ever saving now has a slot carrying an empty `SaveText`. `LoadMatchFromSlot`
	 *      refuses that by name; anything that offers a "Continue" affordance must gate on a
	 *      LOADABLE slot and not on `DoesSaveSlotExist`, which now answers true for a slot
	 *      with no match in it."
	 * IT NAMES THE AFFORDANCE, THE WRONG FUNCTION AND THE RIGHT RULE. It was not read. So the
	 * cause has two halves -- an arm no clause pinned, AND a live in-tree warning that the
	 * agent who needed it never saw -- and an account giving only the first is the same
	 * species of miss.
	 *
	 * THE DURABLE LESSON IS NOT "READ MORE CAREFULLY", WHICH IS UNACTIONABLE AND WOULD BE THE
	 * THIRD TIME THIS PROJECT WROTE IT. A warning placed in the header of the TYPE IT CONCERNS
	 * is invisible to someone working from the CONSUMING end: nothing about writing a menu
	 * sends you to a save-game payload's file comment, and grep only finds it if you already
	 * suspect the thing it warns about. That is why this project already records "read the
	 * lane record first" and "a fix instruction encodes a diagnosis -- measure the premise".
	 * WHAT WOULD ACTUALLY HAVE CAUGHT IT IS THE CLAUSE THE ARM NEVER HAD -- a prose warning
	 * cannot fail a build and an executable one can, which is the whole argument for this
	 * function being `static` and reachable rather than a sentence somebody should have read.
	 *
	 * THE THREE REFUSALS BELOW USED TO BE WRITTEN INLINE IN `LoadMatchFromSlot` AND ARE NOW
	 * WRITTEN ONCE. That function CALLS this rather than keeping a copy, which is the whole
	 * point: a menu that asked "is this restorable" against a second statement of the
	 * conditions would agree with the loader today and drift from it silently. One authority,
	 * asked by both. The wording of each refusal is unchanged, and each is a PHRASE that
	 * completes "slot 'X' ..." so the loader's messages read exactly as they did.
	 *
	 * NOT THE SUBSYSTEM-STATE REFUSALS, WHICH STAY IN `LoadMatchFromSlot` AND MUST. A running
	 * AI turn and an unconfigured subsystem are facts about THIS OBJECT AT THIS MOMENT, not
	 * about the slot; folding them in here would make a title screen -- which has no match
	 * subsystem configured at all -- report every slot in the world as unrestorable.
	 *
	 * STATIC, AND THAT IS WHAT MAKES IT PINNABLE. It needs no world, no subsystem and no
	 * disk: a clause constructs a `UStratSaveGame` with `NewObject`, sets the fields it wants
	 * to test, and asserts the answer. `null` is a legitimate argument and means "the slot did
	 * not hold one of these".
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Save")
	static bool IsPayloadRestorable(const UStratSaveGame* Payload, FText& OutRefusalReason);

	/**
	 * Whether a named slot holds a restorable match. The disk-touching half of the question
	 * above.
	 *
	 * IT RESOLVES NOTHING AND THAT IS DELIBERATE: `ResolveSaveSlotName` is an INSTANCE method
	 * because the fallback lives in `ActiveConfig`, and this is static so that a caller with no
	 * match subsystem -- the title screen, which is the caller this exists for -- can ask at
	 * all. Pass the name you mean. An empty name is refused rather than resolved.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Save")
	static bool DoesSlotHoldARestorableMatch(const FString& SlotName, FText& OutRefusalReason);

	/**
	 * §2.11.6: "any completed match on the save skips all guidance automatically."
	 *
	 * READ FROM THE SLOT AND NOT FROM THIS SESSION. The question is about the SAVE's
	 * history, not about the match on screen -- a player who finished a match last week and
	 * started a new one today is not a first-session player, and a session-local bool would
	 * say they were on every launch. `UStratSaveGame::bHasCompletedAMatch` is the field, and
	 * wave B1 declared it in the payload for this call.
	 *
	 * A MISSING SLOT ANSWERS FALSE, and that is the correct answer rather than a fallback: a
	 * save with no history has no completed match on it, so guidance runs. Same for a slot
	 * this build cannot read -- an unreadable slot proves nothing about what the player has
	 * finished, and the cost of a wrong answer is asymmetric. Guidance shown to a veteran is
	 * a strip they dismiss with `Skip guidance`; guidance withheld from a first-time player
	 * is §2.11.6's whole purpose silently not happening.
	 *
	 * WHAT WRITES THE FIELD. This paragraph used to read:
	 * RETRACTED> "NOTHING WRITES THE FIELD YET, and that is a debt rather than an oversight
	 * RETRACTED>  ... Until a caller sets it on a match ending, this answers false for every
	 * RETRACTED>  slot and guidance runs every match."
	 * `RecordMatchCompletionOnSave` writes it, and `ApplyView` calls that whenever the model
	 * it is reconciling against carries `FStratMatchView::bHasResult`. So the first reader of
	 * a "why does the strip keep coming back" report is pointed at THREE things that can make
	 * this answer false on a veteran's machine, in the order worth checking.
	 *
	 * FIRST, `FStratMatchConfig::bRecordCompletionOnMatchEnd` is false, in which case nothing
	 * was ever written. This is the one to check first because it is the one a build can get
	 * wrong silently: it is false in C++ and true only on the GameMode Blueprint's default, so
	 * a GameMode that lost that default plays correctly and remembers nothing.
	 * RETRACTED> This paragraph used to name "the slot name is empty
	 * RETRACTED>  (`FStratMatchConfig::SaveSlotName` unset on the GameMode's defaults, in which
	 * RETRACTED>  case nothing was ever written)" as the first thing to check. That branch is
	 * RETRACTED>  unreachable without a deliberate clear -- `SaveSlotName` is declared
	 * RETRACTED>  `= TEXT("StratocracyMatch")` and is never unset -- so it sent the first
	 * RETRACTED>  reader of that report down a path that cannot occur.
	 *
	 * SECOND, the match never reached a result on screen: `ApplyView` observes the model, so a
	 * match abandoned before its result is correctly not a completed one. THIRD, the write
	 * itself was refused, which `RecordMatchCompletionOnSave` logs as a warning and never
	 * swallows.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Guidance")
	bool HasCompletedAMatchOnSave(const FString& SlotName) const;

	/**
	 * §2.11.6's WRITER: records on the slot that a match reached a result. `SlotName` empty
	 * means `FStratMatchConfig::SaveSlotName`, exactly as everywhere else in this section.
	 *
	 * WHAT GAP THIS CLOSES. `UStratSaveGame::bHasCompletedAMatch` had a reader
	 * (`HasCompletedAMatchOnSave`, consumed by `AStratPlayerController::TryArmGuidedOpening`'s
	 * `bSuppressed` argument) and no writer, so §2.11.6's "any completed match on the save
	 * skips all guidance automatically" was structurally unreachable: the guided opening
	 * re-armed on every match and the onboarding never stopped teaching. This is the writer.
	 *
	 * IT WRITES ONE FIELD AND READS THE SLOT FIRST. A fresh `UStratSaveGame` here would
	 * destroy the §4.10 text of a match in progress -- turning a guidance bug into a
	 * save-erasing one -- so the existing payload is loaded and modified, which is the same
	 * posture `SaveMatchToSlot` takes from the other side. Between them, one method owns
	 * every field but the onboarding pair and this one owns exactly one of that pair.
	 *
	 * IT CREATES A SLOT THAT DOES NOT EXIST, and that is the load-bearing call in here. The
	 * alternative -- write only into an existing slot -- would mean a first-time player who
	 * finished a match WITHOUT EVER SAVING is still a first-time player forever, which is the
	 * most likely path through a first session and therefore the path the defect would have
	 * survived on. §2.11.6's condition is about the player's history, and a history that only
	 * accrues when someone presses Save is not the history the GDD names. The payload this
	 * creates carries no `SaveText`, which `LoadMatchFromSlot` already refuses BY NAME
	 * ("carries no §4.10 text") rather than restoring an empty board.
	 *
	 * IT IS IDEMPOTENT AND SAYS SO BY RETURNING TRUE. A slot that already holds the bit is
	 * not rewritten -- there is no second thing to record, `ApplyView` may observe a finished
	 * match on every refresh until the player leaves it, and a disk write per frame for a
	 * value that cannot change is the shape that turns an end-of-match screen into a stutter.
	 *
	 * IT REFUSES AN EMPTY SLOT NAME rather than inventing one, matching `SaveMatchToSlot`:
	 * a literal here would be a second author of a string the designer's property owns.
	 *
	 * IT IS NOT A MATCH-ENDED EVENT AND MUST NOT BECOME ONE. It records a fact that is
	 * already true of a model; it ends nothing, awards nothing and tells no widget. If an
	 * end-of-match screen ever needs to fire once, it hangs off the same observation in
	 * `ApplyView` and not off this method's return, because this method returns true for
	 * "already recorded" and an event that fired on that would fire on every load of a
	 * finished slot.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Guidance")
	bool RecordMatchCompletionOnSave(const FString& SlotName, FString& OutFailureReason);

	/** The board actor, or null when none was spawned. Phase 4's selection machine drives
	 *  the reach and target overlays through it, and `ApplyView` drives §2.11.6-B's objective
	 *  ring through it directly.
	 *  [AMENDED 2026-08-23: this said "the two overlays", which was TRUE OF THE SELECTION
	 *  MACHINE and is still true of it -- the machine owns exactly two highlight sets. It is
	 *  amended anyway because a reader arriving here by grepping the count would generalise
	 *  it to the board, which has three components and two different drivers.] */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Match")
	AStratBoardActor* GetBoard() const { return Board; }

	/** The actor drawing a given unit, or null. The reconciliation map, read-only. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Match")
	AStratUnitActor* FindUnitActor(int32 UnitId) const;

	// ---- §2.8's end of match ---------------------------------------------

	/**
	 * Whether the match this object is running has reached a §2.8 result.
	 *
	 * THE CONVENIENCE FORM OF `StratMatchIsConcluded`, for a caller that has no model in
	 * hand -- a widget, a GameMode, a clause. It builds one and asks; a caller that already
	 * built a model must use the free function on THAT model instead, because two models
	 * built at two instants are two answers.
	 *
	 * FALSE WHEN IT CANNOT TELL. No bridge, no seed, or a model that refused to build all
	 * answer false, and that is deliberate: this is "the match is over", and an object that
	 * cannot read the state has not established that it is. `IsMatchLive()` is the question
	 * that separates "no match" from "a match in progress".
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Match")
	bool IsMatchConcluded() const;

	/**
	 * The §2.8 result as the screen last saw it, or a default-constructed view.
	 *
	 * VALID ONLY WHEN `IsMatchConcluded()`, and it is a projection of `AppliedModel.Match`
	 * rather than a fresh query, so it answers "what was drawn" the way `GetViewModel` does.
	 * A victory surface reads `ResultTier` from here.
	 *
	 * IT DOES NOT CARRY WHO WON, AND THAT IS NOW A DESIGN RATHER THAN A GAP. The sentence
	 * used to end "...and that is a gap rather than a design: `strat::UiMatchView` has no
	 * winning side to mirror", and it is STAMPED rather than deleted because the first half
	 * is still true and only the verdict on it moved. `UiMatchView` still has no winning side
	 * and this struct still mirrors it faithfully; what changed on 2026-08-25 is that
	 * `GetMatchResult` below now answers WHO, through a separate upstream query, for the
	 * reason `FStratMatchResultView` records -- putting the winner on the view model would
	 * have moved T-UI-05's field enumeration to hold a value one screen reads.
	 *
	 * SO THE TWO READERS ANSWER DIFFERENT QUESTIONS AND A CALLER MUST NOT SUBSTITUTE ONE.
	 * This one is a projection of what was DRAWN and cannot fail; `GetMatchResult` is a live
	 * query against the bridge and can be refused.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Match")
	FStratMatchView GetConcludedMatchView() const;

	/**
	 * §2.8's result WHOLE -- tier, cause, THE WINNING SIDE, and the tiebreak key a capped
	 * match was decided on. What §2.11.4's end-of-match screen reads.
	 *
	 * A LIVE QUERY AND NOT A PROJECTION OF `AppliedModel`, which is the one way it differs
	 * from `GetConcludedMatchView` above and the reason it has a refusal channel at all. The
	 * winner is not on the view model and deliberately never will be -- see
	 * `FStratMatchResultView` -- so there is nothing cached here to read, and this asks the
	 * bridge. `StratBuildMatchResult` is the whole of the implementation; this class adds no
	 * policy, no caching and no interpretation.
	 *
	 * IT DOES NOT REQUIRE THE MATCH TO HAVE ENDED and refuses over no such thing. An
	 * in-progress match answers `true` with tier `InProgress` and `Winner == INDEX_NONE`,
	 * which is upstream's own convention. A caller that wants "is it over" asks
	 * `IsMatchConcluded()`, which reads the model the screen was drawn from; asking this and
	 * testing `Tier` instead would be a SECOND answer to that question, built at a second
	 * instant, and this class's standing rule is against exactly that.
	 *
	 * ALL-OR-NOTHING: a refusal leaves `OutResult` as the caller brought it. `false` means
	 * there is no bridge, no definitions or no seed -- never "nobody has won yet".
	 *
	 * THE WINNER IS A `strat` SIDE INDEX AND NOT A FACTION. §2.11.4's result line is
	 * faction-voiced, and choosing the voice means comparing `Winner` against
	 * `GetViewingSide()`. That comparison belongs to the screen that draws the line; this
	 * class supplies both numbers and draws no conclusion from them.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Match")
	bool GetMatchResult(FStratMatchResultView& OutResult, FString& OutFailureReason);

	// ---- §2.9's opponent -------------------------------------------------
	// THIS CLASS DRIVES THE AI AND DOES NOT IMPLEMENT IT. Every command comes from
	// `FStratAiTurnRunner`, which gets every command from `FStratBridge::NextAiCommand`,
	// which is `strat::nextCommand`. What is decided here is only WHEN a turn runs and
	// WHETHER it is this side's, and both of those are read from configuration and the view
	// model rather than derived.

	/** Whether `FStratMatchConfig::AiSides` names this side. Configuration, not a rule. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|AI")
	bool IsSideAi(int32 Side) const;

	/**
	 * True when a live, unfinished match is waiting on a side the config calls AI.
	 *
	 * IT ASKS THE VIEW MODEL AND CACHES NOTHING. `sideToMove` and `hasResult` are the rules
	 * module's answers, read fresh; a `bAiTurnPending` flag beside them could disagree, and
	 * the disagreement would present as an AI that plays after the match is over.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|AI")
	bool IsAiTurnDue() const;

	/**
	 * Runs the AI's turn if one is due -- immediately, or after `AiTurnDelaySeconds`.
	 *
	 * THE ENTRY POINT CALLERS USE. `AStratGameMode::BeginPlay` calls it once the match is
	 * live (so an AI that moves first does), and `AStratPlayerController` calls it after a
	 * command the human's side applied (so an AI that moves second does). Neither of them
	 * checks whose turn it is -- that check lives here, once.
	 *
	 * A `true` RETURN WITH A DELAY CONFIGURED MEANS "SCHEDULED", NOT "PLAYED", and that
	 * ambiguity is the price of pacing. `RunAiTurnsNow` is what a caller that needs the turn
	 * to have happened by the time it returns calls -- and it is what every test calls,
	 * because the default delay is zero and the delayed path needs a ticking world.
	 *
	 * NOTHING DUE IS `true` AND NOT A REFUSAL. "It is the human's turn" is the ordinary
	 * state of a hot-seat game and is not an error to report.
	 */
	bool RunAiTurnsIfDue(FString& OutFailureReason);

	/**
	 * Plays every consecutive AI turn that is due, synchronously, and reconciles after.
	 *
	 * THE TESTABLE ENTRY POINT, and the one the delayed path eventually calls. It loops
	 * while the side to move is an AI side and the match has no result, bounded by
	 * `AiMaxConsecutiveTurns` -- so a one-AI hot-seat game plays exactly one turn here and
	 * phase D's AI-vs-AI game plays to a §2.8 result in one call.
	 *
	 * A REFUSED AI TURN STOPS THE LOOP AND IS REPORTED. Commands already applied stand; the
	 * runner cannot undo one and neither can this. The alternative -- swallow it and hand
	 * play back -- is the silent-empty-turn failure the whole phase is shaped around.
	 *
	 * REFRESHES PRESENTATION ON THE WAY OUT, including on the refusal path. The board moved
	 * whether or not the turn finished, and a screen that still shows the pre-AI board is a
	 * screen that disagrees with the rules module.
	 */
	bool RunAiTurnsNow(FString& OutFailureReason);

	// ---- §2.11.2's playback ----------------------------------------------
	// THE WHOLE PUBLIC SURFACE IS THREE READS AND ONE VERB, AND NONE OF THEM CAN MOVE THE
	// MATCH. Nothing below submits a command, touches `FStratBridge` or writes
	// `FStratViewModel`; the most any of them does is stop a camera. That is the property a
	// `T-TURN-09` clause about the three dispositions pins, and it is structural rather than
	// a promise -- see `StratAiPlayback.h`.

	/**
	 * §2.11.2's "any click or Esc skips to the end state". True only if it was playing.
	 *
	 * THE RETURN IS WHAT LETS THE INPUT BE CONSUMED EXACTLY ONCE.
	 * `AStratPlayerController::HandleSelectionEvent` calls this first and returns early on a
	 * `true`, so the click that stopped the tour does not ALSO select a unit -- and a click
	 * arriving at a quiet moment gets the `false` and falls through to the selection machine
	 * untouched. A `void` skip was the other shape and would have forced that caller to ask
	 * `IsAiPlaybackRunning()` first, which is the same question answered a call earlier and
	 * therefore a second thing that can disagree with it.
	 *
	 * "THE END STATE" IS ALREADY ON SCREEN WHEN THIS IS CALLED. It stops a camera tour; it
	 * fast-forwards nothing. `StratAiPlayback.h` argues why that is what §2.11.2 asks of a
	 * presentation layer that is reconciled rather than evented.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|AI")
	bool SkipAiPlayback();

	/**
	 * True while the camera still has recorded actions to step to.
	 *
	 * IT ANSWERS "IS A TOUR RUNNING" AND NOT "DID THE AI DO ANYTHING", AND FOR ONE DIFF IT
	 * ANSWERED THE SECOND. [CORRECTED 2026-08-29, and the correction is in the code rather
	 * than only here -- see `BeginAiPlayback`.] This is a cursor-versus-count read, and the
	 * reel is filled on EVERY hand-over whatever the configuration, so at the shipped
	 * `AiPlaybackStepSeconds` of zero it returned true for a tour that was never armed and
	 * never would be. `SkipAiPlayback` then succeeded and `HandleSelectionEvent` consumed the
	 * first click or Esc after every AI turn in the default configuration. Measured by the
	 * test author and confirmed at the source before the fix.
	 *
	 * WHAT MAKES THE CURSOR SUFFICIENT IS AN INVARIANT AND NOT A SECOND QUESTION ASKED HERE,
	 * AND IT HAS TWO OWNERS RATHER THAN ONE. `BeginAiPlayback` arms the timer and retires the
	 * reel on every path where it declines to; `EndAiPlaybackTour` is the only thing that
	 * stops the timer and it retires the reel every time, so a tour cannot be disarmed and
	 * leave a live cursor behind it. [AMENDED 2026-08-29, second fix: this paragraph named
	 * `BeginAiPlayback` alone, which covered the arming half only -- see that function on the
	 * reseed defect that gap allowed.] So `IsPlaying()` is false unless a tour is genuinely
	 * under way, and this stays a one-line read of one fact.
	 *
	 * ASKING `FTimerManager::IsTimerActive` INSTEAD WAS THE OTHER SHAPE AND IS STILL REJECTED,
	 * for the reason it always was: the timer stays armed for one interval after the final
	 * action is shown, so it would answer true after the tour visibly ended. Gating this call
	 * on `ActiveConfig.AiPlaybackStepSeconds` was the third shape and was rejected because it
	 * fixes only the configured case -- a positive interval with no world to time against
	 * leaves the same stuck cursor, and that path is how a headless caller reaches it.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|AI")
	bool IsAiPlaybackRunning() const;

	/**
	 * How many actions the last hand-over recorded. Survives the tour and survives a skip.
	 *
	 * EXPOSED BECAUSE THE ALTERNATIVE IS UNOBSERVABLE FROM OUTSIDE, which is the same hole
	 * `AStratBoardActor::GetTargetOverlayCount` was added to close: with only
	 * `IsAiPlaybackRunning()`, a reel that recorded nothing and a reel that was skipped read
	 * identically, so no clause could tell "the AI's actions were listed and the tour was cut
	 * short" from "nothing was ever listed".
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|AI")
	int32 GetAiPlaybackStepCount() const;

	/** How far the tour got. Equals `GetAiPlaybackStepCount()` once finished or skipped. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|AI")
	int32 GetAiPlaybackCursor() const;

	/**
	 * Shows the action at the cursor and moves past it. Returns false when there was none.
	 *
	 * WHAT GAP THIS CLOSES. `OnAiPlaybackTimer` is reachable only through `FTimerManager`, so
	 * the whole camera-stepping half of §2.11.2 -- the focus, the advance, the stop on the
	 * last step -- could be executed only by a test that owned a ticking world and waited out
	 * a real interval. In practice that meant executed by nothing. This is the same body
	 * behind a name a caller can reach, and the timer callback is now one line calling it.
	 *
	 * IT IS NOT A SECOND DRIVER OF THE REEL, WHICH IS THE THING IT HAD TO AVOID BEING. There
	 * is one implementation of stepping and the timer is a caller of it, not a peer -- so a
	 * clause asserting what a step does is asserting what the timer does, rather than
	 * asserting a copy that could drift from it. A separate test-only stepping path would
	 * have been the substitution `T-UI-02` exists to catch, applied to §2.11.2.
	 *
	 * IT DOES NOT ARM, RE-ARM OR REQUIRE A TIMER, and it does stop one that is running when it
	 * takes the last step -- so hand-driving a tour to its end leaves exactly the state that
	 * letting the clock do it leaves.
	 *
	 * **THAT SENTENCE IS STILL TRUE AFTER THE 2026-09-02 SLIDE RESHAPE, AND IT IS RESTATED HERE
	 * RATHER THAN LEFT TO BE RE-DERIVED, BECAUSE THE OBVIOUS IMPLEMENTATION FALSIFIED IT.** The
	 * tour's interval is now variable -- `AiPlaybackStepSeconds` plus however long the slide this
	 * step armed will take -- and the natural place to compute it is right here, where the step
	 * is taken. It is NOT here. `OnAiPlaybackTimer` calls this and then calls
	 * `ArmNextPlaybackStep`, so the clock's logic sits on the other side of the timer callback
	 * and everything this function's block claims survives verbatim: no world is required, no
	 * handle is touched, and a clause may drive a whole tour by hand at
	 * `AiPlaybackStepSeconds = 600` on a world that is never ticked, exactly as
	 * `StratAiPlaybackClauses.cpp` does today. It DOES arm a unit's SLIDE -- see
	 * `PlayMoveSlideForStep` -- which is a `Body` offset and a tick flag on one actor, not a
	 * timer and not a tour.
	 *
	 * It also does not check `AiPlaybackStepSeconds`: the
	 * cursor is retired at the shipped default (see `BeginAiPlayback`), so at that default
	 * this returns false because there is nothing at the cursor, and not because it asked.
	 *
	 * SAFE TO CALL AT ANY TIME. Nothing here submits a command, touches `FStratBridge` or
	 * writes `FStratViewModel`; the most it does is point a camera. Called with no tour under
	 * way it returns false and does nothing.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|AI")
	bool AdvanceAiPlaybackOneStep();

	/**
	 * Hands over the route one unit is about to be seen travelling, for the NEXT `ApplyView`
	 * and for that one only.
	 *
	 * WHAT GAP THIS CLOSES. `AStratUnitActor` can ease its picture along a polyline, and the
	 * only producer of a real one is `FStratBridge::MovePathToHex` -- the same query §2.5's
	 * commit itself takes, so the picture walks the hexes the rules module actually moved the
	 * unit through rather than the straight chord between the ends. Nothing carried that route
	 * from the input path, where it is asked for, to `ApplyView`, where the actor is reconciled.
	 * This is that carrier and it is the whole of it.
	 *
	 * A PRESENTATION SCRATCHPAD ON THIS OBJECT, CLEARED PER HAND-OVER, AND NOT IN THE MODEL --
	 * `AiPlaybackReel`'s shape exactly, and the ruling that put the reel here transfers without
	 * amendment. That member's own block states it: "`T-INT-05`'s subject is every member of the
	 * view model, so a playback cursor placed there would enter that ID's subject by its own
	 * words and owe a clause under it." A route field on `FStratViewModel` would owe a clause
	 * under an ID whose whole claim is that the screen is a function of the model alone, and the
	 * route is precisely the one thing that is NOT -- it is where the unit CAME FROM, which no
	 * statement of what IS can carry.
	 *
	 * AND `FStratPathPreviewView` IS STRUCTURALLY THE WRONG CARRIER, WHICH IS THE SECOND REASON
	 * AND IS NOT THE SAME AS THE FIRST. `StratDecoratePathPreview` CLEARS the preview for a unit
	 * whose `bHasMoved` is set -- its own block lists that among the five paths that produce no
	 * route -- and `bHasMoved` is set one microsecond after a move applies. A carrier that is
	 * empty exactly when the animation needs it is not a carrier.
	 *
	 * IT IS CONSUMED ONCE AND THE WHOLE MAP IS EMPTIED, INCLUDING ENTRIES NOBODY READ. See
	 * `ApplyView`: the reset is unconditional and after the destroy loop, so a route noted for a
	 * unit that died in the AI's reply -- and therefore has no actor and no model entry left --
	 * cannot survive to be applied to a later, unrelated move.
	 *
	 * NOTHING ELSE CALLS IT, AND THAT IS STILL TRUE: `AStratPlayerController::HandleSelectionEvent`
	 * is the only caller, no AI path notes a route, and `AStratUnitActor::ApplyUnitView` snaps on
	 * an empty route -- so this map's own contents ARE the signal for THAT function and there is
	 * no second one to keep in agreement with it.
	 *
	 * [CORRECTED 2026-09-02, SAME DAY, BECAUSE THE USER REVERSED THE DECISION THIS BLOCK CITED.
	 * WRITTEN FLAT. WHAT IS RETRACTED IS THE DECISION AND ITS CONSEQUENCE, NOT THE MECHANISM
	 * ABOVE.] It said:
	 * RETRACTED> "AND THAT ABSENCE IS THE FEATURE. AI moves must not slide (user decision), and
	 * RETRACTED>  the mechanism is that no AI path notes a route -- not a detection of one."
	 * **AI MOVES NOW SLIDE.** They do it during §2.11.2's playback tour, one unit at a time, and
	 * they still do not reach this map: `UStratMatchSubsystem::PlayMoveSlideForStep` reads
	 * `FStratAiPlaybackStep::RouteHexes` off the reel and calls
	 * `AStratUnitActor::PlayRouteSlide`, a different verb on a different object with a different
	 * resting behaviour. So the ABSENCE this block described is intact and only what it was for
	 * has changed -- there is still no detection of an AI turn anywhere in the tween path, and a
	 * second producer for this map would still be a defect.
	 *
	 * @param UnitId     the unit the route belongs to, keyed as `FStratUnitView::UnitId` is.
	 * @param RouteHexes `FStratBridge::MovePathToHex`'s ordered route, `[0]` the unit's own hex
	 *                   and `.Last()` the goal. AXIAL HEXES AND NOT WORLD POINTS: the conversion
	 *                   is `AStratBoardActor::WorldLocationOfHex` and happens in `ApplyView`,
	 *                   which is the one place in this file allowed to make it. An empty array
	 *                   notes nothing and clears nothing.
	 */
	void NotePendingMoveRoute(int32 UnitId, const TArray<FIntPoint>& RouteHexes);

private:
	/** `RunAiTurnsNow`, reached from the pacing timer. Its refusal is logged, not returned. */
	void OnAiTurnTimer();

	/**
	 * Starts the camera tour over whatever `AiPlaybackReel` holds, if one is configured.
	 *
	 * CALLED ON THE WAY OUT OF `RunAiTurnsNow`, AFTER THE REFRESH AND ON THE REFUSAL PATH TOO.
	 * The AI moved whether or not its turn finished, and the actions it got through are worth
	 * watching either way; a tour that only ran on the happy path would go missing in exactly
	 * the situation a player most wants to see what happened.
	 *
	 * IT OWNS THE ARMING HALF OF THE INVARIANT THE REST OF THIS SECTION READS: **the reel's
	 * cursor is at the end unless a tour is actually under way.** It is the only function that
	 * arms `AiPlaybackTimer`, and on every path where it decides NOT to arm one it retires the
	 * reel -- `FStratAiPlaybackReel::SkipToEnd` -- so that `IsAiPlaybackRunning()` and
	 * `SkipAiPlayback()` cannot report a tour that will never run.
	 *
	 * [CORRECTED 2026-08-29, SECOND FIX. THE SENTENCE HERE READ "IT IS THE ONLY FUNCTION THAT
	 * ARMS `AiPlaybackTimer`, SO IT IS THE ONLY ONE IN A POSITION TO KNOW", AND WENT ON TO
	 * CLAIM THIS FUNCTION OWNED THE INVARIANT. That was true of ARMING and false of
	 * DISARMING, and the invariant constrains both. The clock was stopped from five other
	 * places, and two of them -- `Deinitialize` and `TearDownPresentation` -- stopped it
	 * WITHOUT retiring. So a match played at a positive `AiPlaybackStepSeconds` with a tour
	 * mid-reel, followed by a new match or a slot load, left the cursor stranded: on a
	 * brand-new match `IsAiPlaybackRunning()` reported true, `SkipAiPlayback()` returned true,
	 * and the skip gate swallowed the player's first click or Esc -- carrying the PREVIOUS
	 * match's step list. That is the same swallow this function was written to fix, reinstated
	 * through a door the prose said did not exist. Found by the W5 gate.
	 *
	 * THE FIX IS THAT NO FUNCTION STOPS THE CLOCK WITHOUT RETIRING, because there is no longer
	 * one that can: `StopAiPlaybackTimer` is now `EndAiPlaybackTour` and does both, atomically,
	 * at all six call sites. So the disarming half is closed the way the arming half is --
	 * in one place, covering call sites nobody has written yet -- and this block no longer
	 * claims a sole ownership the code does not give it. **The invariant has two owners and
	 * they are named: this function arms, `EndAiPlaybackTour` disarms.**]
	 *
	 * [ADDED 2026-08-29 AS A DEFECT FIX AND NOT AS TIDYING. Before it, the reel was filled on
	 * every hand-over while only the TIMER was gated, so at the shipped `AiPlaybackStepSeconds`
	 * of zero the cursor sat at 0 with a non-empty reel: `IsAiPlaybackRunning()` was true,
	 * `SkipAiPlayback()` succeeded, and `AStratPlayerController::HandleSelectionEvent`
	 * consumed the first click or Esc after EVERY AI hand-over in the default configuration.
	 * Gating `SkipAiPlayback` on the config was the alternative and was rejected: it fixes the
	 * zero case and leaves the no-world case, where a positive interval still arms nothing.
	 * Establishing the invariant where the arming decision is made fixes both, because there
	 * is one decision and it is here.]
	 *
	 * RETIRING IS NOT CLEARING, AND THE DIFFERENCE IS LOAD-BEARING. `SkipToEnd` moves the
	 * cursor and keeps `Steps`, so `GetAiPlaybackStepCount()` still reports what the AI did
	 * even at the shipped default -- which is the one discriminator between "a tour was cut
	 * short" and "a reel was never filled", and the thing a clause asserting the skip landed
	 * needs in order to tell them apart.
	 *
	 * DOES NOTHING WITH AN EMPTY REEL, WITH A NON-POSITIVE `AiPlaybackStepSeconds`, OR WITH NO
	 * WORLD. Each is an ordinary state and none is reported: the first is an AI that did
	 * nothing, the second is the shipped default, and the third is a headless test. All three
	 * now leave the reel retired rather than merely un-toured.
	 */
	void BeginAiPlayback();

	/**
	 * One tick of the tour, driven by `AiPlaybackTimer`. Takes a step and, if there was one,
	 * arms the clock for the next.
	 *
	 * [CORRECTED 2026-09-02, SAME DAY, BY THE VARIABLE-INTERVAL RESHAPE. WRITTEN FLAT: THE
	 * HEADING AND THE SENTENCE BELOW BOTH BECAME FALSE AND A READER WHO GREPS EITHER MUST LAND
	 * ON THE CURRENT SHAPE.] It said:
	 * RETRACTED> "A ONE-LINE CALL TO `AdvanceAiPlaybackOneStep` AND NOTHING ELSE. IT HOLDS NO
	 * RETRACTED>  LOGIC OF ITS OWN ON PURPOSE."
	 * **IT HOLDS THE CLOCK'S LOGIC AND NONE OF THE STEP'S, WHICH IS THE SPLIT AND NOT A
	 * WEAKENING OF THE OLD RULE.** §2.11.2's tour now waits for each AI move's slide to finish
	 * before showing the next action, so the interval is no longer a constant and the timer can
	 * no longer loop. That logic has to live somewhere; it lives HERE, in the function only a
	 * `FTimerManager` ever calls, rather than in `AdvanceAiPlaybackOneStep` -- because that
	 * function's own block promises "IT DOES NOT ARM, RE-ARM OR REQUIRE A TIMER" and a whole
	 * headless clause file drives it on worlds that are never ticked. Putting the re-arm inside
	 * it would falsify that block and put a timer in front of every one of those clauses.
	 *
	 * WHAT SURVIVES VERBATIM: there is ONE driver of the REEL and this is a caller of it, not a
	 * second copy. The step-and-stop behaviour is still `AdvanceAiPlaybackOneStep`'s alone, and
	 * this function decides nothing about what a step does -- only whether another one is worth
	 * waiting for.
	 */
	void OnAiPlaybackTimer();

	/**
	 * Arms `AiPlaybackTimer` for the next step, ONE-SHOT, at this step's own interval.
	 *
	 * WHY ONE-SHOT AND NOT A LOOP. The interval is `ActiveConfig.AiPlaybackStepSeconds` plus
	 * `LastArmedSlideSeconds` -- the seconds the slide just armed will take -- so it differs
	 * from step to step and a looping handle has one rate for the whole tour. A tour that did
	 * not wait would show the next action while the previous unit was still visibly walking,
	 * which is the specific thing the user asked for the opposite of: one at a time.
	 *
	 * TWO INDEPENDENT REASONS NOT TO ARM, AND BOTH ARE CHECKED. The reel not playing means the
	 * tour is over -- `AdvanceAiPlaybackOneStep` has already called `EndAiPlaybackTour` in that
	 * case, and arming here would put a handle back after the verb that exists to remove it. No
	 * world means nothing to time against, which is `BeginAiPlayback`'s third exit and is the
	 * headless case. Neither is reported: both are ordinary.
	 *
	 * IT DOES NOT RETIRE ANYTHING ON EITHER REFUSAL, WHICH IS THE DIFFERENCE FROM
	 * `BeginAiPlayback` AND IS DELIBERATE. That function retires the reel whenever it declines
	 * to arm, because it is deciding whether a tour happens at all. This one runs only after a
	 * step was taken, so the cursor is already correct and the only two refusals it has are
	 * states some other verb has already handled.
	 */
	void ArmNextPlaybackStep();

	/**
	 * Plays one reel step's move as a slide on the unit that made it. Returns the seconds
	 * armed, or 0.
	 *
	 * WHAT GAP THIS CLOSES, AND IT IS A USER DECISION REVERSED ON 2026-09-02. AI moves were
	 * required not to slide and the mechanism was that nothing noted a route for them; the user
	 * reversed that, so §2.11.2's tour now animates each AI move along the hexes the rules
	 * module actually walked. `FStratAiPlaybackStep::RouteHexes` has carried those hexes since
	 * earlier the same day and had no reader; this is the reader.
	 *
	 * **IT IS NOT AN `ApplyView`, AND THE DISTINCTION IS THE WHOLE REASON IT MAY EXIST.** It
	 * touches ONE actor, writes no transform, spawns and destroys nothing, reconciles nothing,
	 * and consults no model but the one already applied. The board it runs over already shows
	 * the FINAL state -- `RunAiTurnsNow` reconciled to it before the tour began -- so there is
	 * nothing here that could disagree with the model about where a unit is. What it moves is a
	 * picture, on `AStratUnitActor::PlayRouteSlide`'s stated terms.
	 *
	 * SIX REFUSALS AND ALL SIX ARE ORDINARY, WHICH IS WHY NONE IS LOGGED: the step is not a
	 * Move, the route is shorter than two hexes (the path query refused, or the AI moved a unit
	 * nowhere), there is no `Board`, no actor answers for the unit id, the applied model has no
	 * entry for it, or the actor declines. **THE ACTOR-IS-NULL CASE IS THE ONE WORTH NAMING: a
	 * unit that moved early in the hand-over can be DEAD by the time the tour reaches its step**
	 * -- the reel is a list of things that already happened and the board is the aftermath --
	 * and the right answer is to show nothing and move on, not to report a fault.
	 *
	 * THE ANCHOR COMES FROM THE APPLIED MODEL AND IS CONVERTED BY THE BOARD, WHICH IS BOTH OF
	 * THIS FUNCTION'S OBLIGATIONS TO THE ACTOR IT CALLS. `AStratUnitActor::PlayRouteSlide`
	 * measures every offset against a TILE-PLANE point and its declaration forbids deriving one
	 * from the actor transform; the hex the last applied model puts this unit on, through
	 * `AStratBoardActor::WorldLocationOfHex`, is that point and comes from the same expression
	 * the route does. `AppliedModel` and not a fresh `BuildViewModel`: the tour is looking at
	 * the board that was drawn, and a second model built at a second instant is a second answer.
	 *
	 * @param Step the reel entry being shown. Taken by const reference and not mutated; its
	 *             `RouteHexes` are converted, never inspected for legality.
	 * @return the seconds the slide will take, straight from the actor -- see
	 *         `AiPlaybackStepSeconds`, which records why this class may not compute it.
	 */
	float PlayMoveSlideForStep(const FStratAiPlaybackStep& Step);

	/**
	 * Puts every AI unit's PICTURE back to the start of its first recorded move, once, as a
	 * tour begins. Nothing is animated and no transform is touched.
	 *
	 * WHAT GAP THIS CLOSES, AND IT IS A DEFECT A HUMAN REPORTED FROM PIE ON 2026-09-02 THAT NO
	 * CLAUSE IN THIS TREE COULD SEE. `RunAiTurnsNow` reconciles BEFORE it calls
	 * `BeginAiPlayback` -- deliberately, so the tour steps a camera over a finished board, and
	 * that ordering is load-bearing and is NOT what changed. The cost nobody predicted is that
	 * every AI unit is DRAWN at its destination the moment the hand-over resolves, so
	 * `PlayMoveSlideForStep` then displaces each picture BACK to its leg's start and eases
	 * forward. The player sees **a mass snap to the destinations and then a rubber-band per
	 * unit**. This function is the missing first half: the pictures go back before anything is
	 * shown, and nothing is ever drawn at a hex it has not been seen to walk to.
	 *
	 * THE FIRST MOVE PER UNIT AND ONLY THE FIRST, WHICH IS THE WHOLE OF THE RULE. A unit that
	 * moves three times parks at leg 1's start and needs no further help: leg N+1's route
	 * begins on the hex leg N ended on, and `AStratUnitActor::PlayRouteSlide` measures waypoint
	 * 0 from that same hex, so consecutive legs are continuous by construction rather than by a
	 * second correction.
	 *
	 * CALLED WHERE `BeginAiPlayback` HAS ALREADY PASSED ALL THREE OF ITS GUARDS, WHICH IS THE
	 * PLACEMENT AND NOT AN ACCIDENT OF READING ORDER. A non-positive `AiPlaybackStepSeconds`
	 * -- the shipped default and every automation fixture -- an empty reel, and a missing world
	 * all return before this runs, so each of those paths parks nothing and stays bit-identical
	 * to the tour that shipped before slides existed. `AStratUnitActor::ParkPictureAt` refuses a
	 * second time on `MoveTweenSeconds <= 0`, so the inertness does not depend on this call
	 * site alone.
	 *
	 * THREE UNITS ARE DELIBERATELY NOT PARKED AND EACH IS ORDINARY. One with no Move step in
	 * the reel never appeared to travel, so there is nothing to put back. One whose actor is
	 * gone -- it died later in the same hand-over -- has no picture. And one whose first Move's
	 * path query REFUSED carries an empty route: that leg cannot animate at all, so this
	 * function skips it and parks the unit at its first move that has a usable route instead.
	 * **THE RESIDUAL IS NAMED RATHER THAN HIDDEN**: such a unit is drawn at the refused leg's
	 * destination from the tour's start, which is exactly what it did before this function
	 * existed and is no worse.
	 */
	void PreParkPicturesForTour();

	/**
	 * Whether a §2.11.2 tour will actually run for the reel as it now stands.
	 *
	 * WHAT GAP THIS CLOSES, AND IT IS A HAZARD REMOVED BY CONSTRUCTION RATHER THAN BY CARE.
	 * `BeginAiPlayback` held three separate reasons not to arm a tour, each with its own `return`
	 * and its own argument. The existence hold has to be built ABOVE `RefreshPresentation` --
	 * that reconcile is what destroys and spawns the actors it is about -- so the same question
	 * has to be answerable from two places. **A second copy of three conditions is exactly how
	 * the gate gets raised for a tour that then declines to run, and every held unit is stranded
	 * forever.** One function, two callers, and that state cannot be constructed.
	 *
	 * THE THREE REASONS ARE KEPT AS THREE BLOCKS AND THREE `return false`s, verbatim from where
	 * they were. Folding them into one boolean expression would have merged three arguments into
	 * one anonymous `false`.
	 *
	 * `const` AND WITH NO SIDE EFFECT, WHICH IS WHAT LETS IT BE ASKED TWICE. The RETIRE that
	 * used to ride on each of those exits stayed in `BeginAiPlayback`, because retiring a reel
	 * is a consequence of deciding not to tour and not part of deciding it -- and the hoisted
	 * caller must be able to ask without changing anything.
	 */
	bool WillAiPlaybackRun() const;

	/**
	 * Walks the reel once and records which units must be HIDDEN until their step and which must
	 * be RETAINED past it. Raises the existence gate.
	 *
	 * WHAT GAP THIS CLOSES, AND IT IS THE SECOND HALF OF A DEFECT A HUMAN REPORTED FROM PIE. The
	 * pre-park fixed POSITION -- units no longer snap to their destinations before the tour
	 * shows them moving. This fixes EXISTENCE: a unit built during the hand-over is on screen
	 * from the moment it resolves, and a unit killed during it is gone from that same moment,
	 * both visibly out of step with the tour about to show them being built and killed.
	 *
	 * **THE SCOPE IS NARROWER THAN IT LOOKS AND THE LIMIT IS STRUCTURAL, NOT AN OMISSION.** Two
	 * sets are held: units in the FINAL model that appeared during the hand-over, and units that
	 * departed but WERE in the pre-hand-over roster. **A unit BUILT AND KILLED inside one
	 * hand-over is in neither** -- it is absent from the final model, so `ApplyView` never spawns
	 * an actor for it, and absent from the pre-hand-over roster, so no actor already exists. It
	 * cannot be shown being built or dying without a spawn path the model never asked for, and
	 * this pass does not invent one. **This is the common case and not a corner: three of the
	 * seven deaths in a measured hand-over were units built during that same hand-over.**
	 * `AStratUnitActor` is spawned in exactly one place -- `ApplyView`'s spawn branch, verified
	 * by sweep -- so there is no other route by which such an actor could exist.
	 *
	 * CALLED FROM `RunAiTurnsNow` ABOVE `RefreshPresentation` AND FROM NOWHERE ELSE, because
	 * after that reconcile the evidence is gone: the killed unit's actor is destroyed and the
	 * built unit's actor is spawned, and `UnitActors` -- which this function reads as the
	 * pre-hand-over roster -- has already moved on.
	 */
	void BuildTourExistenceHolds();

	/**
	 * Sets every unit actor's visibility from the playback cursor. A pure function of that
	 * cursor.
	 *
	 * **IT RECOMPUTES AND NEVER APPLIES A DELTA, AND THAT IS THE DESIGN.** `bVisible =
	 * (Cursor > RevealAfter) && (Cursor <= HideAfter)`, with permissive defaults -- `-1` for a
	 * unit that was always there, `MAX_int32` for one that never leaves -- so a unit in both
	 * maps, in one, or in neither falls out of one expression with no special case. A delta
	 * would have to remember what it last did, and the two places that would be wrong are
	 * exactly the ones with no incremental answer: a skip moves the cursor by any amount, and a
	 * unit can be both revealed and hidden within one tour.
	 *
	 * IT HIDES AND NEVER DESTROYS. A unit already killed is hidden here and destroyed by
	 * `EndAiPlaybackTour`; destroying at the step would be irreversible against a cursor a skip
	 * can move, and this function promises to be a function of that cursor alone.
	 *
	 * TWO CALL SITES: `BeginAiPlayback`, at cursor zero and AFTER `RefreshPresentation` has
	 * spawned the actors this is about, and `AdvanceAiPlaybackOneStep` immediately after its
	 * `Advance()`. It is NOT called from `ApplyView` and does not need to be -- nothing there
	 * writes an actor's hidden flag, so a mid-tour refresh cannot undo it.
	 */
	void ApplyTourExistenceAtCursor();

	/**
	 * The hex the LAST APPLIED model puts a unit on. False when the model has no entry for it.
	 *
	 * ONE SCAN AND NOT TWO, WHICH IS A DRIFT RISK CLOSED RATHER THAN A TIDY-UP.
	 * `PlayMoveSlideForStep` and `PreParkPicturesForTour` both need the anchor
	 * `AStratUnitActor::PlayRouteSlide` and `ParkPictureAt` measure against, and it must be the
	 * SAME hex for both or a picture parks at one place and slides from another. Two hand-copied
	 * loops over `AppliedModel.Units` is two chances for that to stop being true.
	 *
	 * `AppliedModel` AND NOT A FRESH `BuildViewModel`, on this class's standing distinction: a
	 * tour is looking at the board that was DRAWN, and a model built at a second instant is a
	 * second answer.
	 */
	bool FindAppliedUnitHex(int32 UnitId, FIntPoint& OutHex) const;

	/**
	 * Ends a tour: clears `AiPlaybackTimer`, retires the reel, AND cancels every unit's route
	 * slide. The one place that does any of the three, and it always does all three.
	 *
	 * [WIDENED 2026-09-02 BY THE AI ROUTE SLIDE, BY EXACTLY THE ARGUMENT THAT CREATED THIS
	 * FUNCTION. The heading said "either ... both" and now says three; that is a restatement and
	 * not a retraction, and the two original halves are untouched.] `AStratUnitActor::PlayRouteSlide`
	 * PARKS a picture over an intermediate hex with no clock behind it -- that is what a tour's
	 * slide is -- so a tour that stops mid-slide strands that picture until something clears it.
	 * The clear went HERE and not in `SkipAiPlayback` for this function's own founding reason:
	 * **there must be no verb in this class that stops a tour without clearing what the tour
	 * left behind**, so that a SEVENTH call site cannot reopen the hazard by being written by
	 * somebody who wanted only to stop a clock. Without it, a skip mid-slide strands a unit over
	 * the hex its route happened to reach, forever, with the model and the actor transform both
	 * saying it is somewhere else.
	 *
	 * A NO-OP AT MOST OF THE SIX SITES, AND THAT IS THE POINT RATHER THAN A COST.
	 * `AStratUnitActor::CancelRouteSlide` writes a zero this actor already holds whenever no
	 * slide is running, and at the shipped `AStratUnitActor::MoveTweenSeconds <= 0` default no
	 * slide can ever run, so every automation fixture pays one loop over `UnitActors` and
	 * nothing else.
	 *
	 * ONE COST IS NAMED RATHER THAN DISCOVERED: it is unconditional over EVERY unit actor, so a
	 * PLAYER's slide still in flight when a tour ends is hard-cut too. That is accepted --
	 * distinguishing the two would need this class to remember which pictures it parked, which
	 * is presentation state on the subsystem to avoid presentation state on the actor.
	 *
	 * ORDERED AFTER THE CLOCK AND BEFORE NOTHING. `AdvanceAiPlaybackOneStep` calls this on its
	 * last step and THEN arms that step's slide, deliberately, so the final action of a tour
	 * still animates. See that function.
	 *
	 * IT WAS `StopAiPlaybackTimer` AND IT CLEARED ONLY THE CLOCK, WHICH IS WHAT MADE THE
	 * RESEED DEFECT POSSIBLE. `Deinitialize` and `TearDownPresentation` both called it to stop
	 * a tour and neither retired the reel, so a reseed mid-tour left the cursor stranded
	 * mid-reel with no timer to advance it -- and `IsAiPlaybackRunning()`, which reads the
	 * cursor, reported a running tour on a brand-new match. See `BeginAiPlayback` for the full
	 * account.
	 *
	 * THE RENAME IS THE FIX AND NOT DECORATION. Retiring inside the old function while it was
	 * still called `StopAiPlaybackTimer` would have left a name that promises less than the
	 * body does, which is how the SEVENTH call site gets added by someone who wanted only to
	 * stop a clock. There is now no verb in this class for stopping the clock alone, so the
	 * disarming half of `BeginAiPlayback`'s invariant cannot be reopened by a call site that
	 * does not exist yet -- which is the property that made putting the arming half on
	 * `BeginAiPlayback` right in the first place.
	 *
	 * CHECKED AT ALL SIX CALL SITES BEFORE THE BODY CHANGED, because "retiring is harmless
	 * here" is a claim about each of them and not about the function. Three were already at
	 * the end and are unaffected: `AdvanceAiPlaybackOneStep`'s null-`Peek` arm, its last-step
	 * arm, and `SkipAiPlayback` after its own `SkipToEnd`. One -- `RunAiTurnsNow` -- calls
	 * `FStratAiPlaybackReel::Reset` on the very next line, which subsumes it. The remaining
	 * two, `Deinitialize` and `TearDownPresentation`, are the defect and are the reason for
	 * the change.
	 *
	 * IT RETIRES AND DOES NOT `Reset()`, so `GetAiPlaybackStepCount()` still answers after a
	 * tour ends -- which `AdvanceAiPlaybackOneStep` and `SkipAiPlayback` both need, and which
	 * is the only discriminator between a tour cut short and a reel never filled. Clearing
	 * `Steps` at a MATCH boundary is a separate concern and is done explicitly in
	 * `TearDownPresentation`; see there for why that half is not folded in here.
	 */
	void EndAiPlaybackTour();

	/**
	 * Points the camera at one recorded action.
	 *
	 * ASKS THE BOARD FOR THE WORLD LOCATION AND COMPUTES NO LAYOUT OF ITS OWN.
	 * `AStratBoardActor::WorldLocationOfHex` is documented as the ONE hex -> world conversion
	 * in the project, "because two copies of a layout constant is a board whose units are half
	 * a tile off"; and `AStratCameraPawn` is documented as knowing nothing about hexes, which
	 * is why the conversion happens on this side of the call rather than behind a `FocusHex`.
	 *
	 * DEGRADES SILENTLY AND DOES NOT REFUSE. A step with no hex (the closing EndTurn), no
	 * board, or a possessed pawn that is not an `AStratCameraPawn` all leave the camera where
	 * it is and let the tour continue. None of those is a fault: the tour is decoration, and a
	 * decoration that reported into the match's failure channel would put presentation text in
	 * front of a player whose game is fine.
	 */
	void FocusPlaybackStep(const FStratAiPlaybackStep& Step) const;

	/**
	 * Hands the seeded bridge to the scoreboard HUD, if there is one.
	 *
	 * CALLED ONLY AFTER SEEDING. `AdoptBridge` refuses an unseeded bridge -- see the header
	 * block; this is the whole reason `StartMatch` is one ordered function.
	 *
	 * A MISSING HUD IS NOT A FAILURE. A map whose GameMode names no scoreboard runs the
	 * match without one, which is a configuration and not a fault. A HUD that REFUSES is a
	 * failure and is reported, because the only way it can refuse is that the bridge was not
	 * seeded, and that means this function was called out of order.
	 */
	bool HandBridgeToScoreboard(FString& OutFailureReason);

	/**
	 * `StartMatch`'s body, and `LoadMatchFromSlot`'s. THE ONE ORDERED SEQUENCE.
	 *
	 * `Restore` null is a fresh match; non-null replays that payload's §4.10 text onto the
	 * bridge after `LoadScenarioFromFile` and before `HandBridgeToScoreboard`. The step sits
	 * exactly there because `FStratBridge::RestoreFromSaveText` refuses an unseeded bridge
	 * and `AStratScoreboardHUD::AdoptBridge` refuses one too -- so the only legal window is
	 * between them, and putting it anywhere else is an ordering error that presents as a
	 * refusal from whichever end it fell off.
	 *
	 * A RESTORE FAILURE IS A RULES-SIDE FAILURE and tears the bridge down, joining
	 * `LoadDefinitions` and `LoadScenarioFromFile` above it rather than the presentation
	 * complaints below it. A half-restored match is exactly the "seeded and empty" a caller
	 * must not be able to mistake for a match -- `FStratBridge::MakeUiSnapshot`'s line.
	 */
	bool StartMatchInternal(const FStratMatchConfig& Config,
	                        const UStratSaveGame*    Restore,
	                        FString&                 OutFailureReason);

	/**
	 * Destroys the board and the unit actors and clears the pacing timer.
	 *
	 * A NO-OP ON A FIRST `StartMatch`, which is why it can sit unconditionally at the top of
	 * `StartMatchInternal`: nothing is spawned, the map is empty and the handle is invalid.
	 * It has content only on a RESTART -- a load, or a second `StartMatch` -- where without
	 * it the world would carry two boards and the timer would fire into a freed bridge.
	 * `Deinitialize` does the same three things plus the bridge; the duplication is
	 * deliberate, because teardown-at-world-death and teardown-before-reseed are different
	 * events, and collapsing them would make the shutdown path reseed-aware.
	 */
	void TearDownPresentation();

	/** The scoreboard HUD of the first local player, or null. */
	AStratScoreboardHUD* FindScoreboardHUD() const;

	// ---- §2.11.5's OPTIONS control on the in-match command bar ----------------------------
	// THIS MODULE IS THE BINDER BECAUSE IT IS THE ONLY ONE THAT CAN BE, AND THE ARROW IS THE
	// WHOLE ARGUMENT. `UStratCommandBarWidget::OnOptionsRequested` is broadcast in `StratUI`;
	// `AStratPlayerController::RequestOptionsScreen` is a verb in `StratPlay`; `StratPlay ->
	// StratUI` and never back, so the widget cannot call the verb and something on this side of
	// the arrow has to. `AStratScoreboardHUD` -- which creates and owns the widget, and would
	// otherwise be the obvious binder -- is in `StratUI` and fails on the same boundary, exactly
	// as `StratOptionsPresenter.h` records it failing to be the options screen's owner.
	//
	// THIS CLASS AND NOT `UStratOptionsPresenter`, WHICH WAS THE OTHER CANDIDATE AND IS THE ONE
	// SYMMETRY ARGUES FOR. The presenter already binds `OnOptionsDismissed` on the volume screen
	// and is the options lane's owner, so binding the request half there would put both halves
	// in one file. It was rejected on LIFETIME rather than on taste: the presenter's only
	// repeated hook is `OnOptionsPanelStateChanged`, which fires when the panel opens and closes
	// and therefore cannot be relied on to have run when the HUD creates the command bar; a
	// `OnWorldBeginPlay` bind would race the HUD's `BeginPlay` and would not survive a HUD
	// respawn. `ApplyView` runs on every refresh, so a bind placed there is RECONCILED rather
	// than evented -- it repairs itself the frame after a new widget appears, which is the same
	// discipline the units loop and the ring apply to actors.

	/**
	 * Binds `OnOptionsRequested` on this world's command bar if it is not already bound.
	 *
	 * IDEMPOTENT BY CONSTRUCTION AND CALLED ON EVERY `ApplyView`. `IsAlreadyBound` is what makes
	 * a per-refresh call cost a pointer comparison instead of accumulating a binding per frame;
	 * an unguarded `AddDynamic` on a dynamic multicast does not deduplicate, so the handler
	 * would run once per elapsed refresh on a single click.
	 *
	 * NO UNBIND ANYWHERE. The widget and this subsystem die with the same world, and
	 * `UStratCommandBarWidget::NativeDestruct` drops the button's side regardless. A teardown
	 * path here would be a second lifetime to reason about for no observable difference.
	 *
	 * SILENT WHEN THERE IS NO HUD AND WHEN THERE IS NO BAR. Both are legitimate configurations
	 * -- `AStratScoreboardHUD::PushCommandBar`'s own block says an unset `CommandBarWidgetClass`
	 * is one -- and neither is reported, on `PushGuidance`'s stated reasoning.
	 */
	void EnsureCommandBarOptionsBinding();

	/**
	 * Runs `AStratPlayerController::RequestOptionsScreen` for the first local player.
	 *
	 * A `UFUNCTION` BECAUSE `FStratCommandBarOptionsRequested` IS A DYNAMIC MULTICAST and can
	 * bind nothing else. It takes no parameter, so nothing of `StratUI`'s needs to be complete
	 * in `Module.StratPlay.gen.cpp` and this header includes no UMG header on its account --
	 * unlike `UStratOptionsPresenter::HandleAudioOptionsCommitted`, whose struct-by-reference
	 * parameter forced exactly that and is recorded in `StratPlay.Build.cs`.
	 *
	 * THROUGH THE CONTROLLER AND NOT STRAIGHT TO `UStratShellSubsystem::ExecuteRoute`, WHICH IS
	 * THE LOAD-BEARING CHOICE AND IS NOT THE SHORTEST ONE. Both compile from here. Going to the
	 * shell directly would skip the click cue that `RequestOptionsScreen` emits at entry, and
	 * would make the button the one control in the match that does not pass through the
	 * controller -- so the day the input path acquires anything the verb must do first, this
	 * caller would silently not do it. That verb's own declaration makes the mirror of this
	 * argument about `ExecuteRoute` versus `RequestOptionsPanel`; this is the same rule applied
	 * one layer out.
	 *
	 * A REFUSAL IS LOGGED AND NOT PROPAGATED. There is nothing a broadcast can return to and no
	 * caller to hand a reason to; `RequestOptionsScreen`'s own block says a `false` from it is a
	 * fault rather than the interface working, so it is `Warning` and not `Log`.
	 */
	UFUNCTION()
	void HandleCommandBarOptionsRequested();

	/**
	 * This world's `UStratSoundDirector`, or null.
	 *
	 * NULL IS AN ORDINARY ANSWER AND NOT A FAULT, AND EVERY CALLER TREATS IT AS ONE. A world
	 * that is neither Game nor PIE has no director by that class's own
	 * `DoesSupportWorldType`, and a subsystem hand-driven with no world at all has no route to
	 * one either. Audio is emphasis; a match with none is correct in every rules-visible
	 * respect, exactly as `StratTransientReceipts.h` says of a screen with no receipts drawn.
	 * So no caller below logs, refuses or propagates on a null return.
	 *
	 * A LOOKUP AND NOT A CACHED POINTER. Caching would be a second lifetime to reason about
	 * for a `GetSubsystem` call that is a pointer read off the world.
	 */
	UStratSoundDirector* FindSoundDirector() const;

	/**
	 * §2.11.6's match-ended hook: if the model says the match has a result, record it on the
	 * slot.
	 *
	 * ITS CALLER MOVED ON 2026-08-23 AND ITS BEHAVIOUR DID NOT. `ApplyView` used to call this
	 * directly and now calls `ConcludeMatchIfEnded`, which calls this FIRST and unlatched, in
	 * the same position in the same function. That ordering is deliberate: this function has
	 * its own latch, its own opt-in and its own retry-on-failure, and running it inside the
	 * conclusion latch would have made a transient write failure permanent.
	 *
	 * IT HANGS OFF `ApplyView` AND NOT OFF A COMMAND RESULT, because `ApplyView` is the one
	 * place EVERY model reaches the screen through -- `RefreshPresentation` calls it,
	 * `AStratPlayerController::RefreshFromMachine` calls it after decorating, and
	 * `StartMatchInternal` calls it on a load. A hook on the submit path would miss the AI's
	 * winning command, which is precisely how §2.9's Easy AI ends a first-session match.
	 *
	 * THE MODEL IS THE SOURCE AND THE BRIDGE IS NOT ASKED. `FStratMatchView::bHasResult` is
	 * `strat::UiSnapshot::match.hasResult` copied by `StratBuildViewModel` and is the view
	 * model's own answer to "is this match over" -- so this hook decides nothing, which is
	 * the rule this class is built on. Asking the bridge again here would be a second
	 * authority that can disagree with the screen.
	 *
	 * VOID, AND ITS REFUSALS ARE LOGGED RATHER THAN PROPAGATED, for `ApplyView`'s stated
	 * reason: reconciliation is not a request that can be declined, and a caller can do
	 * nothing useful with "the onboarding bool did not persist" except what the log line
	 * does.
	 *
	 * IT IS THE ONLY WRITER IN THIS CLASS NOBODY ASKED FOR, and that is why it is the only one
	 * `FStratMatchConfig::bRecordCompletionOnMatchEnd` gates. `SaveMatchToSlot` and
	 * `RecordMatchCompletionOnSave` run because a caller named a slot; this runs because a
	 * model happened to say a match ended, on a refresh nobody requested a write during. Read
	 * that field's block for the measurement that made the distinction load-bearing.
	 * RETRACTED> This method used to gate on `ResolveSaveSlotName(FString()).IsEmpty()` and to
	 * RETRACTED>  say the arm was for "an automation test that never configured a slot". It was
	 * RETRACTED>  not: `SaveSlotName` defaults to the PLAYER'S slot, so a caller that
	 * RETRACTED>  configured nothing resolved to it and wrote there.
	 */
	void NoteMatchResultIfEnded(const FStratViewModel& Model);

	/**
	 * §2.8's TRANSITION OUT OF PLAY. Called from `ApplyView` with the model being applied;
	 * if that model says the match has a result, this is where the match stops being played.
	 *
	 * WHAT IT IS FOR, IN ONE SENTENCE. Before it existed the engine DETECTED a result and
	 * persisted it and then did nothing else, so the AI's turn never terminated, the side to
	 * move never moved, and the human inherited the AI's side and kept issuing commands the
	 * rules module accepted. `NoteMatchResultIfEnded` -- which this function calls, and which
	 * is unchanged -- was the whole of what a result used to cause.
	 *
	 * IT IS THE SIDE-EFFECT HALF AND `StratMatchIsConcluded` IS THE PREDICATE HALF. The split
	 * is what lets the input gate be a pure function of a model: the controller asks and does
	 * not transition, this transitions and is not asked. A single function doing both would
	 * be unusable from the gate, which runs on every click of a finished match.
	 *
	 * ONCE PER MATCH, LATCHED ON `bMatchConclusionAnnounced`. `ApplyView` runs on every
	 * refresh, and a finished match refreshes as often as an unfinished one; the log line and
	 * the timer clear would otherwise repeat forever. THE LATCH IS NOT A MIRROR OF RULES
	 * STATE and nothing reads it as one -- exactly the distinction `bMatchResultRecorded`'s
	 * own block draws. Every question about whether the match is over is answered by asking
	 * the model.
	 *
	 * THE PACING TIMER IS CLEARED HERE and that is belt-and-braces rather than the mechanism:
	 * `RunAiTurnsNow`'s loop already exits on `bHasResult` before it asks the runner for
	 * anything. Clearing it means a queued turn does not fire into a finished match at all,
	 * rather than firing and finding nothing to do.
	 *
	 * IT DOES NOT LOCK INPUT AND CANNOT. Input arrives at `AStratPlayerController`, which
	 * gates itself on `StratMatchAcceptsPlayerCommands` over its own model. A lock latched
	 * here would be a second authority on whether play is open, and the two could disagree.
	 *
	 * VOID, FOR `ApplyView`'s STATED REASON: reconciliation is not a request that can be
	 * declined.
	 *
	 * IT ALSO PUTS GDD Sec 2.11.4'S END-OF-MATCH SCREEN UP, through
	 * `AStratScoreboardHUD::ShowMatchResult`, and that is the second thing the latch buys.
	 * The screen is a one-shot surface about an event, so its trigger has to be an EDGE, and
	 * this is the only edge in this class that a finished match crosses exactly once.
	 * `StartMatchInternal` clears the latch AND takes any prior screen down, so a restart in
	 * the same session gets its own verdict rather than inheriting the last one's.
	 *
	 * THE SCREEN'S REFUSALS DO NOT REACH THIS FUNCTION'S CALLER and are logged at Warning.
	 * An unset `MatchResultWidgetClass` is a configuration -- that property's own block says
	 * so -- and the `STRAT-MATCH concluded` line above is emitted either way, so a gate reading
	 * the transition never depends on an asset existing.
	 */
	void ConcludeMatchIfEnded(const FStratViewModel& Model);

	/**
	 * THE AUTHORITATIVE `strat::GameState`, one level of indirection down.
	 *
	 * NOT A UPROPERTY -- `FStratBridge` is not a reflected type and must not become one.
	 * `TPimplPtr` and never `TUniquePtr`: the latter instantiates its deleter into a
	 * `.gen.cpp` that never sees `StratBridge.h`, measured as C4150 on this project's HUD.
	 *
	 * THIS MEMBER IS WHAT FREES THE BRIDGE and it is now the only thing in a running game
	 * that owns one. `AStratScoreboardHUD` holds a raw observing pointer to it after
	 * adoption and frees nothing.
	 */
	TPimplPtr<FStratBridge> Bridge;

	/**
	 * Who completes a built model before it is drawn. Unbound until somebody registers.
	 *
	 * NOT A UPROPERTY, because a plain (non-`DYNAMIC`) delegate is not a reflected type and
	 * must not become one -- see `FStratViewDecorator`. It holds a WEAK reference to its
	 * bound object, so it is not a GC root and cannot keep a controller alive past the world.
	 */
	FStratViewDecorator ViewDecorator;

	/** The spawned board, or null. Destroyed in `Deinitialize`. */
	UPROPERTY(Transient)
	TObjectPtr<AStratBoardActor> Board;

	/**
	 * One actor per living unit, keyed by the model's `UnitId`.
	 *
	 * THE KEY IS THE MODEL'S AND NOT A HANDLE OF OUR OWN, which is what makes reconciliation
	 * a set difference rather than a search: a unit in the model with no entry here is
	 * spawned, an entry here with no model unit is destroyed, and everything else is moved.
	 */
	UPROPERTY(Transient)
	TMap<int32, TObjectPtr<AStratUnitActor>> UnitActors;

	/**
	 * §2.11.5's boxed-in fact for the factory `ProductionMenu` is for. Read through
	 * `IsOpenMenuFactorySpawnBlocked`, which is the only reader and the only Blueprint
	 * surface this value has.
	 *
	 * MEANINGLESS WHILE `IsProductionMenuOpen()` IS FALSE, exactly as `ProductionMenuHex`
	 * is, and written and cleared by the same pair of functions in the same statements --
	 * `RefreshProductionMenu` fills all three together, `CloseProductionMenu` clears all
	 * three together. That is what makes it a value ON the rows' clock rather than a
	 * mirror that can drift from them.
	 *
	 * NOT A `UPROPERTY`, AND NOT PUBLIC, WHICH IS THE DIFFERENCE FROM THE TWO MEMBERS IT
	 * RIDES WITH. `ProductionMenu` and `ProductionMenuHex` are `BlueprintReadOnly` so a
	 * widget can bind to them; this one is deliberately not, because a widget reading it
	 * WITHOUT the openness test would draw a boxed-in footer over a closed panel. The
	 * accessor ANDs `IsProductionMenuOpen()` in, so publishing the raw bool beside it
	 * would be two Blueprint surfaces for one fact and only one of them correct.
	 *
	 * IT IS NOT THE `bSeeded`-SHAPED MIRROR THIS CLASS REFUSES ELSEWHERE, and the test is
	 * the one that rule states: a mirror is refused when it CAN disagree with the thing it
	 * mirrors. There is no other source for this value in this object -- `AppliedModel`
	 * is a record of what was drawn and is explicitly not one, see the accessor -- and the
	 * three members move as one. This is `ProductionMenuHex`'s relationship to the rows,
	 * not `bSeeded`'s to the bridge.
	 */
	bool bProductionMenuSpawnBlocked = false;

	/** What `ApplyView` last drew. See `GetViewModel`: a record, never an input. */
	UPROPERTY(Transient)
	FStratViewModel AppliedModel;

	/**
	 * §2.11.2's transient layer for the refresh `AppliedModel` describes. See
	 * `GetTransientReceipts`.
	 *
	 * PRIVATE AND `Transient`, EXACTLY AS `AppliedModel` IS, AND BLUEPRINT REACHES IT THROUGH
	 * `CopyTransientReceipts` RATHER THAN THROUGH THIS FIELD. The first spelling marked it
	 * `BlueprintReadOnly` in place, which UHT refuses -- measured, one error, `Error:
	 * BlueprintReadOnly should not be used on private members`, reported at this file. The
	 * alternative was moving the member to the public section; it was not taken, because a
	 * public mutable member is a second writer of a value whose declaration two lines up
	 * insists `ApplyView` is the only one.
	 */
	UPROPERTY(Transient)
	FStratTransientReceipts LastReceipts;

	/**
	 * The previous refresh's durable readings. `ApplyView` is the only writer.
	 *
	 * IT IS RESET WHEREVER `AppliedModel` IS RESET, AND THAT PAIRING IS LOAD-BEARING RATHER
	 * THAN TIDY. There are exactly three writers of `AppliedModel` -- `ApplyView`,
	 * `Deinitialize` and the match teardown -- and a mark left standing across either
	 * teardown would be compared against the NEXT match's first model, whose `fameCombat` is
	 * zero and whose turn is 1. The kill arm would stay silent on that comparison (a fall
	 * emits nothing) but the income arm would fire on a turn edge between two different
	 * matches, which is a receipt about no match at all. Resetting to a default -- and so to
	 * `bSeeded == false` -- makes the first model of the next match a first observation
	 * again, which is what it is.
	 */
	UPROPERTY(Transient)
	FStratReceiptMark ReceiptMark;

	/**
	 * The AUDIO milestone's previous reading. `ApplyView` is the only writer.
	 *
	 * RESET WHEREVER `ReceiptMark` IS RESET, AND THE PAIRING IS THE SAME ONE AND FOR THE SAME
	 * REASON. `FStratSoundMark`'s own doc gives the audio-specific cost of getting it wrong: a
	 * mark carried across a teardown makes every unit of the OLD match read as destroyed and
	 * every unit of the NEW one read as newly built, on the next match's very first refresh.
	 * The two marks are separate structs rather than one because they watch different fields
	 * for different layers; they are reset on the same two lines so they cannot drift.
	 */
	UPROPERTY(Transient)
	FStratSoundMark SoundMark;

	/**
	 * Unit ids whose death cue has already sounded during the CURRENT playback tour.
	 *
	 * IT EXISTS BECAUSE `ApplyTourExistenceAtCursor` IS A PURE FUNCTION OF THE CURSOR AND RUNS
	 * EVERY STEP. That is its stated design and it is correct -- a skip can move the cursor by
	 * any amount, so recomputing beats tracking -- but it means the same unit is found hidden
	 * on every step after the one that killed it. Without this set a tour of 150 steps would
	 * sound one unit's death up to 149 times.
	 *
	 * A SET AND NOT A "LAST ANNOUNCED STEP" COUNTER, because the cursor can go BACKWARDS in
	 * principle and because two units can die on one step. Membership answers both with no
	 * ordering assumption.
	 *
	 * EMPTIED WHERE THE HOLD IS RAISED AND WHERE IT IS DROPPED, so a second hand-over starts
	 * clean. NOT A `UPROPERTY`, on `RevealAfterStep`'s line: a set of `int32` owns no `UObject`.
	 */
	TSet<int32> AnnouncedDeaths;

	/** Retained from `StartMatch` so `ApplyView` knows which class to spawn. */
	UPROPERTY(Transient)
	FStratMatchConfig ActiveConfig;

	/** Which `strat` side the screen is drawn for. `SetViewingSide` is the only writer
	 *  after `StartMatch`. */
	UPROPERTY(Transient)
	int32 ViewingSide = 0;

	/** The pacing timer, when `AiTurnDelaySeconds` is positive. Cleared in `Deinitialize`. */
	FTimerHandle AiTurnTimer;

	/** §2.11.2's playback timer, when `AiPlaybackStepSeconds` is positive. A SECOND HANDLE
	 *  and never a second use of `AiTurnTimer`: both can be pending at once in an AI-vs-AI
	 *  game -- the tour of turn 7 is still running when turn 8 is scheduled -- and one handle
	 *  would silently cancel whichever was armed first. Cleared in `Deinitialize` and in
	 *  `TearDownPresentation`, beside the other one. */
	FTimerHandle AiPlaybackTimer;

	/**
	 * How many seconds the slide armed by the LAST reel step will take. Read once, by
	 * `ArmNextPlaybackStep`, immediately afterwards.
	 *
	 * IT IS THE ACTOR'S ANSWER AND NEVER THIS CLASS'S. `AStratUnitActor::PlayRouteSlide` returns
	 * it and `PlayMoveSlideForStep` stores it verbatim. The alternative -- multiplying
	 * `AStratUnitActor::MoveTweenSeconds` by a route length here -- would put a second
	 * computation of one duration in a second class, and `AiPlaybackStepSeconds`' own block
	 * states that this subsystem never reads that property. This field is what keeps that
	 * sentence true.
	 *
	 * NOT A `UPROPERTY` AND NOT A MIRROR OF ANYTHING, on `AiPlaybackReel`'s line: a float
	 * describing how long a picture will take to finish moving. Zero at the shipped
	 * `MoveTweenSeconds` default and on every refusal, so the tour's interval collapses to
	 * `AiPlaybackStepSeconds` with no special case anywhere.
	 */
	float LastArmedSlideSeconds = 0.0f;

	/**
	 * Whether §2.11.2's existence hold is in force. Raised by `BuildTourExistenceHolds`, dropped
	 * by `EndAiPlaybackTour`, and by nothing else.
	 *
	 * IT IS ONE HALF OF A CONJUNCTION AND NEVER A DECISION ON ITS OWN. `ApplyView`'s destroy
	 * loop retains an actor only when this is true AND the id is in `HideAfterStep`; this alone
	 * cannot say which ids, and the map alone could strand an actor forever if it outlived its
	 * tour. See that loop, which states both failures.
	 *
	 * IT IS NOT A MIRROR OF RULES STATE, on `AiPlaybackReel`'s distinction: it records whether a
	 * camera tour is deferring some destructions, and nothing reads it to decide what is true of
	 * the match. A match played with it permanently false is identical in every rules-visible
	 * respect and differs only in that units pop in and vanish early.
	 */
	bool bTourExistenceHeld = false;

	/**
	 * Unit id -> the reel step index at which that unit APPEARED. Visible once
	 * `Cursor > value`.
	 *
	 * KEYED BY ID AND VALUED BY STEP INDEX, not by a precomputed threshold, so the comparison in
	 * `ApplyTourExistenceAtCursor` reads against `BuildTourExistenceHolds`' loop without either
	 * having to restate the cursor's meaning. `Cursor` counts steps SHOWN.
	 *
	 * NOT A `UPROPERTY`, on `PendingMoveRoutes`' line: a map of `int32` to `int32` owns no
	 * `UObject`, and leaving it unreflected keeps it off any future reflected walk over this
	 * class. Emptied by `EndAiPlaybackTour` beside the gate.
	 */
	TMap<int32, int32> RevealAfterStep;

	/** Unit id -> the reel step index at which that unit DEPARTED. Retained while
	 *  `Cursor <= value`, and destroyed by `EndAiPlaybackTour` rather than at the step -- see
	 *  `ApplyTourExistenceAtCursor` on why hiding and destroying are split. Only ids that
	 *  already had an actor when the hold was built are here; see `BuildTourExistenceHolds`,
	 *  which states the scope limit that puts a built-and-killed unit in neither map. */
	TMap<int32, int32> HideAfterStep;

	/**
	 * §2.11.2's recorded action list for the LAST hand-over, and the cursor over it.
	 *
	 * NOT A `UPROPERTY` AND NOT REFLECTED, on `AStratPlayerController::SelectionMachine`'s
	 * line: `FStratAiPlaybackReel` is a plain struct on purpose, for the testability reason
	 * its own header gives, and it holds no `UObject` for the collector to care about.
	 *
	 * AND IT IS NOT IN `FStratViewModel`, WHICH WAS RULED ON BEFORE THIS WAVE STARTED.
	 * `T-INT-05`'s subject is every member of the view model, so a playback cursor placed
	 * there would enter that ID's subject by its own words and owe a clause under it. It is
	 * here instead, beside the timer that drives it, and `FStratViewModel` gains no member --
	 * so `T-INT-05` stays refused for W5, and the screen is still rebuildable from the model
	 * alone because nothing drawn on the board depends on this.
	 *
	 * IT IS NOT A MIRROR OF RULES STATE, on `bAiTurnRunning`'s distinction exactly. It is a
	 * list of things that have ALREADY happened plus how far a camera has got through looking
	 * at them; nothing reads it to decide what is true of the match.
	 */
	FStratAiPlaybackReel AiPlaybackReel;

	/**
	 * The routes the NEXT `ApplyView` is to animate along, keyed by unit id. Written only by
	 * `NotePendingMoveRoute`; emptied by `ApplyView` and by `TearDownPresentation`.
	 *
	 * PLACED BESIDE `AiPlaybackReel` BECAUSE IT IS THE SAME KIND OF THING, and that member's
	 * block is the authority for why either may live here: a presentation scratchpad on the
	 * subsystem, cleared per hand-over, not in the model. `NotePendingMoveRoute`'s declaration
	 * states the two reasons `FStratViewModel` was refused and the one reason
	 * `FStratPathPreviewView` was refused; they are not restated here.
	 *
	 * NOT A `UPROPERTY`, ON `AiPlaybackReel`'S LINE. It owns no `UObject` -- a map of arrays of
	 * `FIntPoint` -- so there is nothing for the collector to care about, and leaving it
	 * unreflected keeps it off any future reflected walk over this class.
	 *
	 * IT IS NOT A MIRROR OF RULES STATE, on `AiPlaybackReel`'s own distinction exactly. It is a
	 * list of hexes a unit has ALREADY finished travelling through by the time anything reads
	 * it; nothing consults it to decide what is true of the match, and a match played with this
	 * map permanently empty is identical in every rules-visible respect.
	 */
	TMap<int32, TArray<FIntPoint>> PendingMoveRoutes;

	/**
	 * Guards against re-entering the AI loop.
	 *
	 * IT IS NOT A MIRROR OF RULES STATE, which is the distinction that makes it acceptable
	 * in a class whose header refuses a `bSeeded` bool three paragraphs down. It records a
	 * fact about THIS OBJECT'S CALL STACK -- an AI turn is on it -- and nothing else can
	 * answer that. The path it guards is real: `RunAiTurnsNow` reconciles, reconciliation
	 * can reach a HUD refresh, and a re-entrant call would interleave two AI turns'
	 * commands into one log.
	 */
	bool bAiTurnRunning = false;

	/**
	 * Whether THIS OBJECT has already recorded a completion for the match it is running.
	 *
	 * IT IS NOT A MIRROR OF THE SLOT AND NOT A MIRROR OF RULES STATE, which is what keeps it
	 * acceptable in a class that refuses a `bSeeded` bool. It is a "have I already done the
	 * disk write" latch and nothing reads it as an answer to any question about the match:
	 * `HasCompletedAMatchOnSave` still reads the SLOT, and `RecordMatchCompletionOnSave` is
	 * idempotent on its own without this. Dropping it would cost a `LoadGameFromSlot` per
	 * refresh on a finished match and change no outcome.
	 *
	 * CLEARED IN `StartMatchInternal`, beside `ActiveConfig`, so a second match in the same
	 * session records its own completion rather than inheriting the first one's latch.
	 */
	bool bMatchResultRecorded = false;

	/**
	 * Whether THIS OBJECT has already run §2.8's transition out of play for the match it is
	 * running. See `ConcludeMatchIfEnded`.
	 *
	 * IT IS A "HAVE I ALREADY DONE THE ONE-SHOT WORK" LATCH AND NOT AN ANSWER TO "IS THE
	 * MATCH OVER", on `bMatchResultRecorded`'s line exactly and for its reason. Nothing reads
	 * it as a fact about the match: `IsMatchConcluded`, `StratMatchIsConcluded` and the
	 * controller's input gate all ask the model. Setting it by hand would suppress a log line
	 * and a timer clear and would not open or close play by one command.
	 *
	 * CLEARED IN `StartMatchInternal`, beside `bMatchResultRecorded`, so a second match in
	 * the same session concludes on its own account rather than inheriting the first's latch.
	 */
	bool bMatchConclusionAnnounced = false;

	// THERE IS NO `bSeeded` MIRROR HERE, deliberately. `IsMatchLive()` asks
	// `Bridge->IsSeeded()` from the .cpp, where the definition is available. A bool beside
	// the pointer could disagree with it -- the same reason `AStratScoreboardHUD` refused a
	// `bOwns` flag in favour of "which member is null" -- and the disagreement would present
	// as a match that reports itself live while projecting nothing.
};
