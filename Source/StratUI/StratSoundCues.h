// The AUDIO milestone's world-free half: which of the seven cues a refresh should sound,
// decided from two readings of `FStratViewModel` and from nothing else.
//
// WHAT GAP THIS CLOSES. Nothing in this project makes a sound. The seven occasions the
// milestone names -- a button clicked, a turn ended, a unit moved, a unit attacked, a unit
// destroyed, a factory finishing a build, a match ending -- are five EDGES and two LATCHED
// MOMENTS. `FStratViewModel` is a complete statement of steady state and its own header
// forbids a "changed" flag, an event or a sequence number anywhere in it, so an edge has no
// home in the model and never will. This file is the same answer `StratTransientReceipts.h`
// already gave for GDD Sec 2.11.2's receipts, applied to audio: the previous reading lives in
// a plain value THE CALLER OWNS, the decision is a free function over two values, and what is
// left at the call site is one call and one re-mark.
//
// IT IS THAT FILE'S SIBLING AND DELIBERATELY NOT A NEW IDEA. Every hazard audio faces here --
// the first reconcile with no predecessor, a mark surviving a teardown into the next match, a
// pair-versus-half turn edge -- is one `StratTransientReceipts.h` states and solves, and its
// solutions are copied rather than re-derived. Read that header first; the paragraphs below
// say only what differs.
//
// WHY IT IS IN `StratUI` AND NOT BESIDE THE PLAYER. It reads `FStratViewModel` and touches no
// `UWorld`, no subsystem, no bridge and no `USoundBase`. `StratUI` is BELOW `StratPlay` and
// the arrow does not reverse, so the world-free half lives here and the half that needs a
// world lives there -- the split `Source/StratPlay/Tests/StratTransientReceiptCallSite.cpp`
// records for the receipts, in its own words. A clause about which cue fires needs no
// viewport and no PIE.
//
// AT MOST ONE EMISSION PER CUE KIND PER CALL, AND THAT LIMIT IS HERE RATHER THAN IN THE
// PLAYER ON PURPOSE. A whole AI hand-over is ONE `ApplyView`: ten units can move, three can
// die and two can be built between two readings. Ten overlapping copies of one move sound is
// not ten times as informative; it is a click. The obvious place to fix that is a cooldown in
// whatever plays the sound -- and a cooldown is a wall-clock fact that a headless `-nullrhi`
// fixture cannot drive and a clause cannot see. Collapsed HERE, "one move cue for a
// hand-over" is a property of a pure function over two structs, which a clause asserts in
// three lines with no world at all. The director still carries a per-cue cooldown for the
// cross-CALL case this cannot see -- six buttons clicked in one second -- and the two are
// different spam controls; neither substitutes for the other.
//
// `ButtonClick` AND `MatchEnded` ARE NEVER EMITTED FROM HERE, and their absence is a
// statement rather than an omission. A click is not a change in the model -- a refused button
// changes nothing at all and must still click -- so no diff over two view models can see one,
// and the six input verbs emit it directly. A match ending IS visible in the model, and it is
// still not taken here: `UStratMatchSubsystem::ConcludeMatchIfEnded` already owns a
// once-per-match latch (`bMatchConclusionAnnounced`) and sounds the cue inside it. A second
// latch in this file would be a second answer to "has this match already ended", and the two
// would drift the first time either moved.
//
// WHAT IS DELIBERATELY NOT IN IT.
//   - NO VOLUME, NO PITCH, NO DURATION, NO PRIORITY AND NO ASSET. Those are `Content/`
//     decisions on a `UStratSoundBank`; a constant here would be a mix decision no sound
//     designer can reach. This file answers WHAT HAPPENED and never HOW LOUD.
//   - NO ATTACKER IDENTITY. `UnitAttacked` names the unit whose HP FELL -- the defender --
//     because that is the only party a diff over two models can identify. The attacker is in
//     `FStratCombatOutcome`, which `StratBridge.h` states the bridge does not retain and whose
//     delivery it defers by name to a separate ruling (`bridge_event_list`). DISCHARGED WHEN
//     that ruling lands and the bridge retains outcomes.
//   - NO KILLING-BLOW CUE, AND THE GAP IS NAMED RATHER THAN PAPERED OVER. An attack that kills
//     outright produces NO HP fall, because the defender leaves the model entirely --
//     `ApplyView`'s destroy loop states that `FStratViewModel::Units` is every LIVING unit. So
//     a one-shot kill sounds `UnitDestroyed` and not `UnitAttacked`. That is deliberate: the
//     death cue subsumes the hit, and synthesising a "was attacked" for a unit that is gone
//     would mean guessing a cause this layer cannot observe.
//   - NO COUNTS. Two moves are one `UnitMoved`, per the one-per-kind rule, and this struct
//     carries no multiplicity to let a drawing layer pretend otherwise.
//   - NO STRINGS AND NO TIMES. Every field is a number or an enumerator.
#pragma once

#include "CoreMinimal.h"

#include "StratSoundCues.generated.h"

struct FStratViewModel;

/**
 * Which of the milestone's seven cues this is.
 *
 * EVERY ARM NAMES AN OCCASION AND NOT A SOUND. `UnitAttacked` is "a unit's HP fell between two
 * refreshes", not "a rifle fired"; which wave that maps to is a `UStratSoundBank` slot and is
 * content's call. An arm renamed after the asset that currently fills it would make this enum
 * a description of one sound bank rather than of the game.
 *
 * TWO ARMS ARE NEVER DECIDED BY `StratDecideSoundCues` -- see the header block. `ButtonClick`
 * comes from the six input verbs; `MatchEnded` comes from `ConcludeMatchIfEnded`'s latch.
 */
UENUM(BlueprintType)
enum class EStratSoundCue : uint8
{
	/** A player-facing control was activated. EMITTED AT THE VERB'S ENTRY, unconditionally,
	 *  BEFORE any legality check and REGARDLESS of the return -- a refused button that makes
	 *  no sound reads as a dead button, which is the defect this placement exists against. */
	ButtonClick UMETA(DisplayName = "Button click"),

	/** The pair (`FStratMatchView::Turn`, `FStratMatchView::SideToMove`) changed. Carries the
	 *  turn and side that ENDED, not the ones beginning. */
	TurnEnded UMETA(DisplayName = "Turn ended"),

	/** A unit present in both readings is on a different hex. */
	UnitMoved UMETA(DisplayName = "Unit moved"),

	/** A unit present in both readings has LOWER HP. Never a rise: HP rises under Sec 2.7
	 *  repair and arrives fresh at `hpMax` on a Sec 2.7 spawn, so only a FALL has exactly one
	 *  cause. `StratTransientReceipts.h` is the authority for that three-cause argument. */
	UnitAttacked UMETA(DisplayName = "Unit attacked"),

	/** A unit in the previous reading is absent from the current one. */
	UnitDestroyed UMETA(DisplayName = "Unit destroyed"),

	/** A unit in the current reading is absent from the previous one. NAMED FOR THE FACTORY
	 *  AND NOT FOR THE UNIT because Sec 2.7's Build is the only thing in this project that
	 *  creates one. */
	FactoryBuiltUnit UMETA(DisplayName = "Factory built unit"),

	/** The match reached a result. Emitted by `ConcludeMatchIfEnded` inside its own latch and
	 *  never by the decider. */
	MatchEnded UMETA(DisplayName = "Match ended")
};

/**
 * One unit's audible state at the mark: WHO IT WAS, WHERE IT WAS AND HOW HURT IT WAS.
 *
 * FOUR NAMED FIELDS AND NOT A COPY OF `FStratUnitView`. A whole unit view would make this mark
 * grow a field every time the projection does, and each new field would silently become an
 * input to a cue decision nobody argued for -- a re-selection moving `bSelected` would become
 * "a unit moved" the day someone diffed the struct instead of naming what is compared.
 *
 * `Side` IS STORED THOUGH NOTHING COMPARES IT, WHICH IS THE ONE FIELD HERE THAT IS NOT AN
 * INPUT TO A DECISION, AND IT IS DELIBERATE. `UnitDestroyed` is the only cue whose subject is
 * absent from the current model, so the model cannot say whose unit died; without this field
 * that cue's `Side` would be `INDEX_NONE` and a per-side mix would be unbuildable for exactly
 * the one cue most likely to want one. It is stored rather than derived because it CANNOT be
 * derived after the fact -- the unit is gone. The paragraph above still holds: this is a named
 * field with a stated purpose, not a struct copy.
 *
 * THE ID FIELD IS `FStratUnitView::UnitId` AND IS SPELLED `UnitId` HERE TO MATCH IT. That
 * struct deliberately does NOT spell it `Id`, and its own comment records why.
 */
USTRUCT(BlueprintType)
struct FStratUnitSoundMark
{
	GENERATED_BODY()

	/** `FStratUnitView::UnitId`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	int32 UnitId = INDEX_NONE;

	/** `FStratUnitView::Side` at the mark. NEVER COMPARED -- read only so `UnitDestroyed` can
	 *  name the side that lost the unit. See the struct doc. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	int32 Side = INDEX_NONE;

	/** `FStratUnitView::Hex` at the mark. A different hex now is `UnitMoved`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	FIntPoint Hex = FIntPoint::ZeroValue;

	/** `FStratUnitView::Hp` at the mark. A strictly LOWER value now is `UnitAttacked`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	int32 Hp = 0;
};

/**
 * The previous reading this file compares against. The caller's value.
 *
 * `bSeeded` IS THE WHOLE OF THE FIRST-OBSERVATION RULE, and it is a flag rather than an
 * inference from the contents for `FStratReceiptMark`'s stated reason: every field here has a
 * legal value a real match also holds. An empty `Units` array is a board with no units, which
 * a scenario may legitimately present; turn 0 and side 0 are both real. So the flag records
 * THE WRITE and never its contents. This project already carries a measured defect of exactly
 * that species -- a real default that could not signal unset.
 *
 * AN UNSEEDED MARK EMITS NOTHING, AND FOR AUDIO THAT IS NOT MERELY DEFENSIVE. The first model
 * a match ever applies has no predecessor, so EVERY unit on the board is "an id in the model,
 * absent from the mark" -- which is `FactoryBuiltUnit`. Ferrum Crossing seeds ten units, so
 * without this rule the match opens with the build cue firing on the first reconcile. The
 * one-per-kind rule would reduce that to a single wrong sound rather than ten, which is WORSE
 * rather than better: it would sound exactly like a correct cue.
 *
 * IT IS A VALUE AND NOT A CACHE INSIDE THIS MODULE, on `FStratReceiptMark`'s reasoning. A
 * file-static or a singleton would make the answer depend on process history: two clauses in
 * one suite would contaminate each other, and a second match in one session would inherit the
 * first one's board.
 *
 * IT MUST BE RESET WHEREVER THE APPLIED MODEL IS RESET. A mark surviving a teardown has a live
 * successor to be compared against -- the next match's first model -- and every one of that
 * match's units would read as newly built while every one of the old match's would read as
 * destroyed. `UStratMatchSubsystem` resets it beside `ReceiptMark`, at both of the two places,
 * and those two are the whole of the requirement.
 */
USTRUCT(BlueprintType)
struct FStratSoundMark
{
	GENERATED_BODY()

	/** False until `StratSoundMarkFromView` has written this. See the struct doc. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	bool bSeeded = false;

	/** `FStratMatchView::Turn` at the mark. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	int32 Turn = 0;

	/** `FStratMatchView::SideToMove` at the mark. The turn edge is a change in the PAIR
	 *  (Turn, SideToMove) and NEVER in either half: hot-seat moves the side within a turn, and
	 *  a one-sided scenario rolls the turn with the side unchanged.
	 *  `FStratReceiptMark::SideToMove` states that argument in full. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	int32 SideToMove = INDEX_NONE;

	/** Every unit on the board at the mark, in `FStratViewModel::Units` order. ORDER IS
	 *  PRESERVED AND NOT RELIED ON: the join is by `UnitId`, because a unit's position in that
	 *  array is a projection detail and one unit dying shifts every entry after it. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	TArray<FStratUnitSoundMark> Units;
};

/**
 * One cue that fired at one refresh, with enough identity for a later pass to place it.
 *
 * `UnitId` IS CARRIED EVEN THOUGH TODAY'S DIRECTOR IGNORES IT, AND THAT IS THE POINT. Playback
 * is 2D for the reasons `StratSoundDirector.h` argues; the day it becomes positional, the
 * change is `UStratMatchSubsystem::FindUnitActor(UnitId)` and one call swapped inside the
 * director. Dropping the field would make that a re-derivation rather than a lookup.
 */
USTRUCT(BlueprintType)
struct FStratSoundEmission
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	EStratSoundCue Cue = EStratSoundCue::ButtonClick;

	/**
	 * The side this cue is about -- an index into `FStratViewModel::Sides`, NEVER a you/enemy
	 * answer, on `FStratReceiptView::Side`'s stated rule. A cue can be about the opponent
	 * (their unit moved) and must stay able to say so; a viewer-relative mix decision belongs
	 * to whatever plays it, with `FStratViewModel::ViewingSide` in hand.
	 *
	 * ON `TurnEnded` IT IS THE SIDE WHOSE TURN ENDED -- the mark's `SideToMove`, not the
	 * model's.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	int32 Side = INDEX_NONE;

	/** The unit this cue is about, or `INDEX_NONE` when it is about the match. Never
	 *  `INDEX_NONE` on `UnitMoved`, `UnitAttacked`, `UnitDestroyed` or `FactoryBuiltUnit`.
	 *  Always `INDEX_NONE` on `TurnEnded`. On `ButtonClick` and `MatchEnded` it is whatever the
	 *  emitting verb passed, which is `INDEX_NONE` at every site that exists today. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	int32 UnitId = INDEX_NONE;

	/** The turn this cue belongs to. `FStratMatchView::Turn` as read on this refresh, EXCEPT
	 *  on `TurnEnded`, which carries the MARK's turn -- the one that ended. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Sound")
	int32 Turn = 0;
};

// ---------------------------------------------------------------------------
// THE DECIDERS. World-free, so a clause can reach every arm without a viewport.
// ---------------------------------------------------------------------------

/**
 * Reads the audible state of `Model` and returns a SEEDED mark.
 *
 * TOTAL AND NEVER REFUSING, on `StratMarkFromView`'s precedent: a model with no units yields a
 * seeded mark with an empty array, which is the correct statement that nothing was on the
 * board -- not a failure. A refusal would have to be reported to a caller with no course other
 * than storing it anyway.
 */
STRATUI_API FStratSoundMark StratSoundMarkFromView(const FStratViewModel& Model);

/**
 * Compares `Model` against `Mark` and fills `Out` with the cues that should sound.
 *
 * OVERWRITES `Out` ENTIRELY, INCLUDING EMPTYING IT. The output describes THIS refresh and
 * never accumulates; a caller wanting a history keeps one. The same discipline `ApplyView`
 * runs on, one layer up.
 *
 * THE RULES IT IMPLEMENTS, in the order they are applied and the order the emissions land in:
 *   1. AN UNSEEDED MARK EMITS NOTHING AND RETURNS. See `FStratSoundMark`'s doc -- this is the
 *      whole of the first-reconcile hazard, and Ferrum Crossing's ten seeded units are what it
 *      costs to get wrong.
 *   2. `TurnEnded` when the PAIR (Turn, SideToMove) differs. Never either half alone.
 *   3. One pass over `Model.Units` in model order. For an id present in BOTH readings:
 *      `UnitMoved` if `Hex` differs, `UnitAttacked` if `Hp` is strictly LOWER. For an id
 *      present only in the model: `FactoryBuiltUnit`.
 *   4. One pass over `Mark.Units` in mark order: `UnitDestroyed` for an id absent from the
 *      model. Legal because `ApplyView`'s destroy loop states that `FStratViewModel::Units` is
 *      every LIVING unit, so a unit that left the model died.
 *   5. AT MOST ONE EMISSION PER CUE KIND. The first qualifying unit wins; later ones are
 *      dropped silently and are counted nowhere. See the header block for why this collapse is
 *      here and not in the director.
 *
 * A UNIT THAT BOTH MOVED AND WAS HIT BETWEEN TWO READINGS EMITS BOTH, and that does not
 * contradict rule 5 -- they are different kinds. It is the ordinary human path: move, attack,
 * one refresh.
 *
 * AN HP RISE EMITS NOTHING AND IS NOT REPORTED AS A FAULT. Sec 2.7 repair raises HP and a
 * Sec 2.7 spawn arrives at `hpMax`; neither is an attack, and this layer cannot tell a repair
 * from a reissued id. `StratTransientReceipts.h` carries the full three-cause argument and
 * `FStratBridge::RepairsAtTurnOpen` is where a repair is measured soundly.
 *
 * IT ASKS THE RULES MODULE NOTHING. No legality check, no reachability query, no cost table --
 * there is no `FStratBridge` in this file's signature or its includes, and a cue is a statement
 * about two projections and never about what was allowed.
 */
STRATUI_API void StratDecideSoundCues(const FStratSoundMark& Mark,
                                      const FStratViewModel& Model,
                                      TArray<FStratSoundEmission>& Out);

/**
 * The one authority for what a legal volume is: `[0, 1]`, and NaN reads as silence.
 *
 * WHY A VOLUME RULE LIVES IN THE CUE-VOCABULARY HEADER AND NOT BESIDE EITHER OF ITS TWO
 * CALLERS, WHICH IS THE ONLY QUESTION THIS FUNCTION RAISES. Two files clamp:
 * `UStratOptionsWidget`'s setters, so that the number the screen draws is the number that
 * was accepted, and `UStratAudioSettings::Sanitize`, so that a slot hand-edited on disk
 * cannot push a gain of 40 into `SetSoundMixClassOverride`. THOSE TWO FILES ARE IN DIFFERENT
 * MODULES -- the widget is `StratUI`, the save payload is `StratPlay` -- and the arrow runs
 * `StratPlay -> StratUI` and never back. So the rule has to live at or below `StratUI` or it
 * has two authors, and this project has measured what two authors of one string costs
 * (`AStratShellGameMode::SaveSlotName`'s block, which derives rather than copies for exactly
 * this reason).
 *   IT LANDS IN *THIS* HEADER RATHER THAN A NEW ONE BECAUSE `StratSoundBank.h` ALREADY
 * INCLUDES IT. `StratPlay`'s audio files reach this file today for `EStratSoundCue`, so the
 * clamp arrives on both sides of the boundary at the cost of zero new includes and zero new
 * translation units. A `StratAudioVolume.h` would have been a second header carrying one
 * function.
 *
 * NaN IS MAPPED TO 0 AND NOT TO 1, AND THE CHOICE IS ARGUED BECAUSE BOTH ARE DEFENSIBLE.
 * `FMath::Clamp` on a NaN returns the NaN -- comparison against a NaN is false in both
 * directions -- so a clamp alone is not a guarantee, and a NaN reaching
 * `UGameplayStatics::SetSoundMixClassOverride` is a gain nobody can predict. Silence is the
 * failure a player can diagnose ("I hear nothing") and unity is the one they cannot ("it
 * sounds normal, so the slider must be working"). The louder default would hide the fault.
 *
 * TOTAL AND INFALLIBLE. There is no refusal and no out-parameter: every float has an answer,
 * and a caller that wanted to know whether its input was already legal compares the result
 * against what it passed in.
 */
STRATUI_API float StratClampVolume(float Volume);
