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
//   which is why it does not travel that path. Capture still has no engine caller at all.
//   RETRACTED IN PLACE rather than deleted, because a reader who remembers the old claim
//   needs to see WHICH HALF of it was withdrawn.
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

	// ---- §2.9's opponent -------------------------------------------------
	// EVERY FIELD BELOW HAS A DEFAULT THAT PRESERVES HOT-SEAT EXACTLY. An empty `AiSides`
	// means no side is played by the AI, which is the game phases 0-6 shipped; nothing in
	// this block runs until a Blueprint default says otherwise. That is deliberate: a
	// milestone that turned the AI on by defaulting it on would change the meaning of every
	// existing test and every existing PIE session in the same pass that introduced it.

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
	 * ON A FAILED REBUILD THE SIDE HAS STILL CHANGED, matching `AStratScoreboardHUD::
	 * SetViewingSide` exactly: rolling back would make a hand-over silently stay with the
	 * previous player, which is the one outcome a hot-seat game must not produce quietly.
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
	 * Sec 2.11.5's rows for the open factory, IN THE ORDER THE RULES MODULE RETURNED THEM.
	 *
	 * THE ONE THING A WIDGET BINDS TO. `BlueprintReadOnly` and never writable: there is
	 * exactly one writer, `RefreshProductionMenu`, and a menu a Blueprint could also write is
	 * a menu with two authors and no way to tell which one the player is looking at.
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

	/** Whether a slot exists. `SlotName` empty means `FStratMatchConfig::SaveSlotName`. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Save")
	bool DoesSaveSlotExist(const FString& SlotName) const;

	/** The slot a name of `""` resolves to. Exposed so a menu can label the button. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Save")
	FString ResolveSaveSlotName(const FString& Requested) const;

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
	 *  the two overlays through it. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Match")
	AStratBoardActor* GetBoard() const { return Board; }

	/** The actor drawing a given unit, or null. The reconciliation map, read-only. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Match")
	AStratUnitActor* FindUnitActor(int32 UnitId) const;

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

private:
	/** `RunAiTurnsNow`, reached from the pacing timer. Its refusal is logged, not returned. */
	void OnAiTurnTimer();

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

	/**
	 * §2.11.6's match-ended hook. Called from `ApplyView` with the model being applied; if
	 * that model says the match has a result, records it on the slot.
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

	/** What `ApplyView` last drew. See `GetViewModel`: a record, never an input. */
	UPROPERTY(Transient)
	FStratViewModel AppliedModel;

	/** Retained from `StartMatch` so `ApplyView` knows which class to spawn. */
	UPROPERTY(Transient)
	FStratMatchConfig ActiveConfig;

	/** Which `strat` side the screen is drawn for. `SetViewingSide` is the only writer
	 *  after `StartMatch`. */
	UPROPERTY(Transient)
	int32 ViewingSide = 0;

	/** The pacing timer, when `AiTurnDelaySeconds` is positive. Cleared in `Deinitialize`. */
	FTimerHandle AiTurnTimer;

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

	// THERE IS NO `bSeeded` MIRROR HERE, deliberately. `IsMatchLive()` asks
	// `Bridge->IsSeeded()` from the .cpp, where the definition is available. A bool beside
	// the pointer could disagree with it -- the same reason `AStratScoreboardHUD` refused a
	// `bOwns` flag in favour of "which member is null" -- and the disagreement would present
	// as a match that reports itself live while projecting nothing.
};
