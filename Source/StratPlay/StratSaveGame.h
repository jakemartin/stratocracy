// GDD §4.10 / §2.11.6 / §4.1 -- the slot payload. What actually goes in a save slot,
// as distinct from what §4.10 defines.
//
// WHAT GAP THIS CLOSES. `FStratBridge::SerializeRecordedSave` sat unused for a whole
// milestone -- nothing in the engine called it -- and `FStratBridge::RestoreFromSaveText`
// is its new
// inverse. Both trade in §4.10 TEXT and neither knows where a save LIVES -- Save.h and the
// bridge both say so in as many words ("SERIALIZES, AND DOES NOT WRITE"). This class is
// where it lives, and it is deliberately the only file in the tree that knows that.
//
// WHY IT IS MORE THAN A STRING. §4.10's header is exactly
// `{formatVersion, rulesCommit, dataHash, scenarioId, scenarioHash, seed, commandLog,
// stateHash, result}`. Three things a slot load needs are not in that list, and the format
// is vendored certified bytes hash-gated by T-INT-01, so they cannot be added to it:
//
//   1. `FirstSide`. `FStratBridge::LoadScenarioFromFile` takes one and it CHANGES the
//      seeded state, but §4.10 carries no such field. `RestoreFromSaveText`'s own block
//      records this as a gap and names this class as where it is closed: a caller that
//      re-seeded with the wrong side would reach the `stateHash` check and be refused --
//      safe, but unrecoverable, because the file gives nothing to tell "wrong first side"
//      from "corrupted log" with. Carrying the side in the SLOT rather than in the FILE
//      makes the ordinary load recover instead of refuse.
//   2. `ScenarioFile`. §4.10 carries a `scenarioId` and a `scenarioHash`, which identify a
//      scenario but do not locate one. Re-seeding needs a path, and this module's rule is
//      that a path arrives as configuration and never as a `/Game/` literal in C++ -- so
//      the string written here is the one `FStratMatchConfig::ScenarioFile` was configured
//      with, project-relative, resolved the way `StartMatch` resolves it.
//   3. `RulesCommit` / `DataHash`. `FStratSaveIdentity`'s block says every field of it is
//      SUPPLIED and never recomputed, which means a LOAD needs the same two values a SAVE
//      was written with in order to build the expectation `strat::checkHeader` compares.
//      Storing them beside the text keeps that to one source of truth. Note what this does
//      NOT do: it does not make the check vacuous in the case that matters, because the
//      four-field comparison still refuses a slot written by a DIFFERENT BUILD -- the
//      values travel with the slot, not with the running binary.
//
// ONBOARDING STATE, ONE FIELD OF IT NOW WRITTEN AND ONE STILL NOT. This heading previously
// read:
// RETRACTED> "ONBOARDING STATE, DECLARED HERE AND NOT YET WRITTEN BY ANYTHING."
// See the per-field state below. §2.11.6 puts the
// one-shot tip flags "in the save slot (§4.1)" verbatim, and states that "any completed
// match on the save skips all guidance automatically". Both are properties of a SLOT and of
// nothing else -- there is no rules-module field for either, and there must not be:
// guidance is a teaching surface and the rules are deterministic and headless. They are
// declared in the same pass that opened the payload so that a reader could be added
// without reopening this struct, which is the change that would strand every slot written
// before it.
//
// THE STATE OF THAT DEBT, STATED AS A CONDITION AND NOT AS A SCHEDULE. This block
// previously read:
// RETRACTED> "ONBOARDING STATE, DECLARED NOW AND CONSUMED IN WAVE B2."
// B2 has since landed the guided opening
// without discharging it -- a wave label goes stale the moment the wave closes, the same
// way a line citation goes stale the moment the diff lands, so it is replaced here by what
// is actually true and by what would change it. TODAY:
//   - `bHasCompletedAMatch` is WRITTEN AND READ. This entry previously read:
//     RETRACTED> "`bHasCompletedAMatch` has a READER and no WRITER ... DISCHARGED BY: a
//     RETRACTED>  caller that sets it when a match reaches a result."
//     That is done. `UStratMatchSubsystem::RecordMatchCompletionOnSave` is the writer and
//     `UStratMatchSubsystem::ApplyView` is its caller, on `FStratMatchView::bHasResult` --
//     the view model's own answer to "is this match over", observed at the one place every
//     model reaches the screen. `HasCompletedAMatchOnSave` remains the reader and
//     `FStratGuidedOpening` remains the consumer, so the loop is closed in both directions.
//     ONE CONSEQUENCE, WRITTEN DOWN RATHER THAN DISCOVERED: the writer CREATES a slot that
//     does not exist, so a player who finishes a match without ever saving now has a slot
//     carrying an empty `SaveText`. `LoadMatchFromSlot` refuses that by name; anything that
//     offers a "Continue" affordance must gate on a LOADABLE slot and not on
//     `DoesSaveSlotExist`, which now answers true for a slot with no match in it.
//   - `FiredOneShotTipIds` has NEITHER. §2.11.6's teacher 3 -- nine one-shot strings plus
//     the two cap-approach banners -- is not built. DISCHARGED BY: that tip layer, which
//     is a different clock from the guided opening's four-turn window and was kept out of
//     `FStratGuidedOpening` for that reason. This field is where it persists when it lands.
// Both survive a save-over: `SaveMatchToSlot` carries them forward rather than
// default-constructing a payload, so nothing here is lost while the debt stands.
//
// VERSIONED, AND THIS IS THE IRREVERSIBLE PART. `SavedDataVersion` is written on every
// save and read on every load. A `USaveGame` whose shape changes without a version is the
// classic way a slot becomes unreadable while reporting success: `LoadGameFromSlot`
// happily returns an object with default-constructed new fields and nothing says so. The
// contract this file takes on: ANY change to the meaning of a field below bumps
// `kCurrentSavedDataVersion` and `UStratMatchSubsystem::LoadMatchFromSlot` grows an arm
// for the old value or refuses it by name. Adding a field with a safe default does not.
//
// NOT IN HERE, WITH REASONS:
// - NO RULES STATE. Not a board, not a unit list, not a Fame total. §4.10's log plus the
//   scenario IS the state, and a second copy of it here is a second thing to drift.
//   `RestoreFromSaveText` verifies the reconstruction against the file's own `stateHash`
//   precisely so that this payload never has to be believed about the board.
// - NO `/Game/` PATH. `ScenarioFile` is a project-relative data path, which is what
//   `FStratMatchConfig::ScenarioFile` already is; the tables and the actor classes come
//   from the GameMode's Blueprint default and are NOT written into a slot, because a slot
//   that pinned an asset would make renaming that asset break old saves.
// - NO SLOT NAME AND NO USER INDEX. Those are the caller's arguments to
//   `UGameplayStatics::SaveGameToSlot`; a payload that recorded which slot it was in would
//   be wrong the first time anyone copied one.
// - NO `strat::` TYPE. This header declares a UCLASS, so UHT parses it, and the vendored
//   headers must never get in front of the header tool. Nothing here is a rules type: the
//   §4.10 payload is an `FString` on purpose.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "StratSaveGame.generated.h"

/**
 * The slot payload. See the header block for what is in it and what is deliberately not.
 */
UCLASS(BlueprintType)
class STRATPLAY_API UStratSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	/**
	 * The version of THIS STRUCT'S SHAPE -- not `strat::kFormatVersion`, which versions the
	 * §4.10 text inside `SaveText` and is checked by `strat::checkHeader`. The two move
	 * independently: a new engine-side bookkeeping field bumps this and not that; a change
	 * to the §4.10 grammar bumps that and not this. Conflating them would make an engine
	 * refactor look like a rules-format change to anyone reading a refusal.
	 */
	static constexpr int32 kCurrentSavedDataVersion = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stratocracy|Save")
	int32 SavedDataVersion = kCurrentSavedDataVersion;

	/** The §4.10 text exactly as `FStratBridge::SerializeRecordedSave` emitted it. Never edited here. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stratocracy|Save")
	FString SaveText;

	/**
	 * The two `FStratSaveIdentity` fields the save was WRITTEN with, so the load can build
	 * the same expectation without a second source of truth. See the header block.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stratocracy|Save")
	FString RulesCommit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stratocracy|Save")
	FString DataHash;

	/** Project-relative, as configured on `FStratMatchConfig::ScenarioFile`. Not a `/Game/` path. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stratocracy|Save")
	FString ScenarioFile;

	/**
	 * The side `LoadScenarioFromFile` was given when this match was seeded. §4.10 has no
	 * field for it and this is the only record of it -- see the header block, item 1.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stratocracy|Save")
	int32 FirstSide = 0;

	/**
	 * Which side the screen was drawn FOR when the save was taken. Not `sideToMove`, which
	 * the replayed state answers for itself -- this is §2.11's hot-seat viewing side, an
	 * engine-side fact with no rules-module source, and restoring without it would put a
	 * loaded hot-seat match on the wrong player's screen.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stratocracy|Save")
	int32 ViewingSide = 0;

	// ---- §2.11.6 onboarding state ------------------------------------------
	// ONE FIELD IS NOW WRITTEN AND READ; THE OTHER IS STILL NEITHER. These lines previously
	// read:
	// RETRACTED> "Declared now, consumed in wave B2. See the header block."
	// RETRACTED> "DECLARED, READ IN ONE DIRECTION, AND NOT YET WRITTEN."
	// `bHasCompletedAMatch` has both ends wired --
	// `UStratMatchSubsystem::RecordMatchCompletionOnSave` writes it from `ApplyView` and
	// `HasCompletedAMatchOnSave` reads it. `FiredOneShotTipIds` still has neither. The header
	// block carries the current state and, for each field, what would change it.

	/**
	 * §2.11.6: "any completed match on the save skips all guidance automatically".
	 *
	 * A BOOL ON THE SLOT AND NOT A COUNT. The GDD's condition is "any completed match", so
	 * the second match is not distinguishable from the ninth and a count would be a number
	 * nothing reads -- the same posture §4.10 takes on `seed`, which MUST be 0 because
	 * nothing would mean anything else.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stratocracy|Onboarding")
	bool bHasCompletedAMatch = false;

	/**
	 * §2.11.6: the one-shot event tips, "fired the first time a concept becomes relevant,
	 * never repeated (boolean flags in the save slot, §4.1)".
	 *
	 * A SET OF IDS AND NOT NINE BOOLS. §2.11.6 names nine one-shot strings plus two
	 * cap-approach banners, and that list is content rather than engine structure -- a
	 * named bool per tip would make adding or cutting one a C++ change and an old-slot
	 * migration. A set answers "has this fired" for any id without this file naming any.
	 * The consequence, written down rather than discovered: a CUT tip leaves a dead id in
	 * old slots, which is inert and is the cheaper failure.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stratocracy|Onboarding")
	TSet<FName> FiredOneShotTipIds;
};
