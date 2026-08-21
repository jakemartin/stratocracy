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
// - Submitting commands. The five `FStratBridge::Submit*` methods exist and this class
//   deliberately does not wrap them yet: phase 4 owns the PlayerController, the selection
//   machine and the `STRAT-CMD accepted` log line, and a wrapper landed early is a wrapper
//   written without its caller.
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
	 */
	void ApplyView(const FStratViewModel& Model);

	/**
	 * Build, apply, and refresh the scoreboard. The undecorated path, and the one a caller
	 * with nothing to add uses.
	 *
	 * WRITTEN IN TERMS OF `BuildViewModel` AND `ApplyView` so it cannot drift from the
	 * phase-4 path that calls those two directly with a decoration in between. There is no
	 * third reconciliation implementation in this class and there must not be.
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
	 * IT DOES NOT SET `bHasCompletedAMatch`, AND NOTHING ELSE DOES EITHER. Whether the match
	 * is over is `strat::buildUiSnapshot`'s answer and reaches the §4.10 text through
	 * `result`; §2.11.6's onboarding bool is a different fact -- the SLOT'S HISTORY -- and this
	 * method deliberately does not infer one from the other, because a save taken on the
	 * final turn of a match the player then abandons is not a completed match.
	 *
	 * THE CONDITION THAT DISCHARGES THAT, rather than the wave that was once expected to:
	 * a caller that sets the field when a match reaches a result. This clause used to read:
	 * RETRACTED> "wave B2 owns writing it"
	 * B2 landed §2.11.6's guided opening and did not, so the
	 * sentence is replaced by the condition -- a wave label stops being checkable the moment
	 * the wave closes. `UStratMatchSubsystem::HasCompletedAMatchOnSave` is the reader and
	 * carries the same statement.
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
	 * NOTHING WRITES THE FIELD YET, and that is a debt rather than an oversight -- see
	 * `SaveMatchToSlot`, which carries the value forward and does not set it. Until a caller
	 * sets it on a match ending, this answers false for every slot and guidance runs every
	 * match. That is the safe direction, and it is written here so the first reader of a
	 * "why does the strip keep coming back" report finds the answer in one hop.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Guidance")
	bool HasCompletedAMatchOnSave(const FString& SlotName) const;

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

	// THERE IS NO `bSeeded` MIRROR HERE, deliberately. `IsMatchLive()` asks
	// `Bridge->IsSeeded()` from the .cpp, where the definition is available. A bool beside
	// the pointer could disagree with it -- the same reason `AStratScoreboardHUD` refused a
	// `bOwns` flag in favour of "which member is null" -- and the disagreement would present
	// as a match that reports itself live while projecting nothing.
};
