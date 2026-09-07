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
	MatchEnded UMETA(DisplayName = "Match ended"),

	/**
	 * NOT A CUE. The number of cues, and the size of any array indexed by this enum.
	 *
	 * EVERY REAL CUE MUST BE DECLARED ABOVE THIS LINE. That is the whole contract, and it is
	 * stated as an imperative because no compiler can check it -- see the asserts below.
	 *
	 * WHY IT EXISTS. `StratDecideSoundCues` keeps a one-per-kind gate as a `bool[]` indexed by
	 * `static_cast<int32>(Cue)`. That array was sized `MatchEnded + 1`, which is correct only
	 * while `MatchEnded` is the last enumerator; an eighth cue appended after it would have
	 * been an out-of-bounds WRITE, silent, with no compiler diagnostic. Sizing from `Count`
	 * makes the array grow with the enum instead.
	 *
	 * `UMETA(Hidden)` AND NOT A BARE NAME, AND ITS PURPOSE IS THE COSMETIC ONE. It keeps a
	 * non-cue out of Blueprint dropdowns on a `BlueprintType` enum. That is load-bearing where
	 * it is load-bearing at all -- without it a designer would see `Count` in the key dropdown
	 * of `UStratSoundBank::MinSecondsBetween`, which is a `TMap<EStratSoundCue, float>` on a
	 * data asset a human edits -- and it is the whole of what the marker is for here.
	 *
	 * HOW A REFLECTION WALK SHOULD EXCLUDE THIS SENTINEL: BY VALUE, `[0, Count)`, AND NOT BY
	 * THIS MARKER. That is what both walks in the tree actually do --
	 * `StratShippedSoundBankParity.cpp`'s `AllCues()` and `StratSoundCueClauses.cpp`'s walk --
	 * and a real cue IS a value in `[0, Count)`, which is the same fact `StratDecideSoundCues`
	 * sizes `bEmitted` with. One rule drops `Count` and UHT's generated `_MAX` together and
	 * needs no build guard.
	 *
	 *   RETRACTED> "`Hidden` is a MACHINE-READABLE MARK: a reflection walk can ask
	 *   RETRACTED>  `UEnum::HasMetaData(TEXT("Hidden"), Index)` and tell a sentinel from a cue
	 *   RETRACTED>  without a hard-coded name list ... callers that want "every real cue"
	 *   RETRACTED>  should prefer that."
	 *   THAT RECOMMENDATION WAS REFUSED BY BOTH OF ITS ONLY TWO CALLERS, IN THE SAME PASS THAT
	 *   WROTE IT, ON THREE GROUNDS THEY MEASURED -- so it is retracted here rather than left to
	 *   cost a third author the same measurement. The grounds, in their words and in order of
	 *   weight:
	 *     (1) `Hidden` DOES NOT MEAN "SENTINEL". It means "keep out of Blueprint dropdowns". A
	 *         future REAL cue hidden for that cosmetic reason would vanish from both walks
	 *         SILENTLY -- under-coverage with a green run, which is the precise failure those
	 *         walks exist against and which a value bound cannot produce.
	 *     (2) IT WOULD NOT REMOVE THE `_MAX` NAME RULE ANYWAY, so it buys nothing structurally.
	 *         `_MAX` is not in the generated enumerator table and carries no metadata pair at
	 *         all; the generated `StratSoundCues.gen.cpp` carries `{ "Count.Hidden", "" }` and
	 *         nothing for `_MAX`. A metadata route would have had to keep a name rule beside it.
	 *     (3) IT DOES NOT COMPILE IN A TARGET CI BUILDS. `UEnum::HasMetaData` is declared inside
	 *         `#if WITH_METADATA` -- verified in UE 5.8 at
	 *         `Engine/Source/Runtime/CoreUObject/Public/UObject/Class.h:3224`, in the block
	 *         opening at 3215 and closing at 3255 -- and this tree measured
	 *         `error C2039: 'HasMetaData': is not a member of 'UEnum'` in a Win64 Development
	 *         **Game** build on 2026-08-31. The value bound needs no `#if WITH_EDITOR` guard.
	 *   THE MARKER ITSELF IS NOT RETRACTED AND MUST NOT BE REMOVED: ground (1) is an argument
	 *   against reading it as a sentinel discriminator, not against its real job above.
	 *   Corrected 2026-09-06 over base commit `f7da9ca`, reported as Finding 4; ground (3)
	 *   re-measured against the engine header here rather than taken from the report.
	 */
	Count UMETA(Hidden)
};

/**
 * WHAT `Count` GUARANTEES, PINNED AT COMPILE TIME -- AND, BELOW, WHAT IT CANNOT.
 *
 * A `Count` sentinel is not self-justifying. It is a correct array bound only while every
 * enumerator's value is less than it, and there are exactly three ways that can stop being
 * true. Two of them are visible to a `static_assert` and are pinned here. The third is not
 * visible to any construct in this language, and is named rather than left to be discovered.
 */

/**
 * (1) THE ENUM IS ZERO-BASED, so `Count` is the NUMBER of cues and not merely one past the
 * largest, and slots `0 .. Count-1` are all real. This is what licenses reading the array's
 * length as a cue count anywhere it is convenient to.
 */
static_assert(static_cast<int32>(EStratSoundCue::ButtonClick) == 0,
	"EStratSoundCue must be zero-based: ButtonClick is the first cue and must have value 0. "
	"If you gave an enumerator an explicit value, Count is no longer the number of cues.");

/**
 * (2) `Count` SITS IMMEDIATELY AFTER THE LAST CUE. This is the assert that earns its place: it
 * is the one that fires on an EXPLICIT VALUE, which is the failure mode a sentinel invites and
 * does not prevent. Write `Count = 7` by hand, then insert a cue anywhere above it, and the
 * array is one slot too small while every enumerator still looks reasonable at a glance --
 * that case reddens here, at compile time, and cannot reach a run. It also fires when a cue is
 * inserted BETWEEN `MatchEnded` and `Count`, which is harmless to the array but means the
 * paragraph above has stopped describing the file, and a reader should be sent here to see so.
 */
static_assert(static_cast<int32>(EStratSoundCue::MatchEnded) + 1
		== static_cast<int32>(EStratSoundCue::Count),
	"Count must be the value immediately after MatchEnded, the last cue. Either an enumerator "
	"was given an explicit value, or something was declared between MatchEnded and Count. "
	"Declare new cues ABOVE MatchEnded, or move MatchEnded's doc comment to the new last cue.");

/**
 * (3) AND THE ONE NEITHER ASSERT CAN SEE, SAID PLAINLY BECAUSE A GUARD WHOSE HOLE IS UNSTATED
 * READS AS TOTAL. A cue declared AFTER `Count` gets a value greater than `Count`, changes no
 * other enumerator's value, and is therefore invisible to every comparison that can be written
 * over the enumerators that exist -- an assert cannot name a symbol whose name it does not
 * know. There is no C++17 construct that closes this; it is not that a better assert was not
 * found, it is that the language does not enumerate an enum.
 *
 * IT IS CLOSED AT RUNTIME INSTEAD, AND BY TWO INSTRUMENTS RATHER THAN ONE.
 *   FIRST, a `check` on the index in `StratDecideSoundCues`' `Emit`. That one is strictly
 * weaker -- it needs a run, it needs the offending cue to actually FIRE, and a `check` is
 * compiled out of Shipping -- and what it buys is position: it lands one instruction before the
 * out-of-bounds write rather than after it. No automation clause can exercise THAT LINE without
 * editing this enum, so it will read as dead code to a coverage tool; that is correct and it is
 * the point.
 *   SECOND, AND IT IS THE STRONGER OF THE TWO,
 * `Stratocracy.StratUI.GATE-AUDIO.NoSoundCueIsDeclaredAfterTheCountSentinel`. It walks the
 * reflected `UEnum` for this type and asserts every declared enumerator's value lies in
 * `[0, Count)` -- which is exactly the residue, since an enumerator declared after `Count` is
 * one the compiler will not name but reflection WILL. It sees the defect on any suite run,
 * WITHOUT this enum being edited and whether the offending cue ever fires or not, and its
 * CONTROL 4 runs the same decision over a synthetic mutant every run so the detector itself
 * cannot go quietly blind.
 *
 *   (This paragraph named only the `check` and said "No automation clause can exercise it
 *   without editing this enum" of the WHOLE hole, until 2026-09-06 over base commit `f7da9ca`.
 *   That understated this hole's coverage by half and pointed the reader at the weaker net;
 *   the clause above already existed and already saw the case. The sentence survives, narrowed
 *   to the `check` line it is true of. Reported as the second half of Finding 4.)
 */

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
 * Fills `OutUnitIds` with EVERY unit whose HP is strictly lower in `Model` than it was at
 * `Mark`. The board-side damage alert's world-free half.
 *
 * IT IS A SECOND READER OF THE EXISTING MARK AND ADDS NO SECOND MARK, WHICH IS THE WHOLE
 * DESIGN. A damage diff carrying a private mark of its own would have to re-derive the seeding
 * rule, would have to be reset at both of the two places `FStratSoundMark` names, and would have
 * to be re-marked in the same order at the same call site -- and the first time any of those
 * three drifted, the flash and the `UnitAttacked` cue would disagree about which units were hit,
 * silently. Reading the mark the audio decider already maintains, this function INHERITS all
 * three and is structurally incapable of that disagreement: same mark, same predicate, same
 * refresh.
 *
 * IT REPORTS EVERY DAMAGED UNIT AND THAT IS WHY IT IS NOT `StratDecideSoundCues` WITH A FILTER.
 * That function collapses to AT MOST ONE EMISSION PER CUE KIND -- the header block argues at
 * length why ten overlapping copies of one hit sound is a click rather than information -- so a
 * hand-over damaging three units yields one `UnitAttacked`. For AUDIO that is the specification.
 * For a FLASH it would mean two of three damaged units silently do not flash, with a green
 * build, which is the anti-cacophony rule applied to a surface it was never argued for. The two
 * therefore share a predicate and not an output.
 *
 * AN UNSEEDED MARK REPORTS NOTHING, on `FStratSoundMark`'s own first-observation rule and not on
 * a weaker restatement of it. The first model a match applies has no predecessor, so no unit's
 * HP can be lower than it was.
 *
 * THE OUTPUT IS AN ID AND THERE IS NO SUBTRACTION ANYWHERE, WHICH IS SAID EXPLICITLY SO A LATER
 * READER DOES NOT THINK THE FILE'S STANDING CLAIM WAS QUIETLY DROPPED. `StratSoundCues.cpp`
 * opens by claiming IT COMPUTES NO MAGNITUDE ABOUT GAME STATE, and this function is written to
 * keep that true: it compares `Hp < Hp` and appends an id. (This sentence quoted that claim in
 * its earlier wording, "THERE IS NOT ONE ARITHMETIC OPERATION IN IT", until 2026-09-06. That
 * wording was false OF THE FILE AS IT STOOD THAT MORNING -- the `.cpp` SIZED its one-per-kind
 * `bool[]` with a `+ 1` over `MatchEnded`, which was the enum's last member at the time.
 *   CORRECTED 2026-09-06 AT THE WORDS BEFORE THIS STAMP, over base commit `f7da9ca`: THAT
 * `+ 1` NO LONGER EXISTS ANYWHERE IN THE `.cpp`, and the sentence above stood in the PRESENT
 * TENSE describing a tree that had already changed. The same day's later pass -- the `Count`
 * sentinel, whose reasoning is in `Count`'s own block above -- replaced the bound with
 * `bool bEmitted[static_cast<int32>(EStratSoundCue::Count)]`, a cast and no operator at all.
 * A reader who grepped the `.cpp` for `+ 1` on the strength of the un-stamped sentence would
 * find nothing and would reasonably conclude this block had gone stale unnoticed. The
 * retraction, and the ONE exemption that survives it -- which is now the `- 1` inside
 * `UE_ARRAY_COUNT` and is NOT the `+ 1` this sentence names -- are stated in that file's own
 * opening block, which is the authority for both.
 *   THIS WAS THE THIRD SITE OF ONE FALSE DESCRIPTION AND THE FIRST OUTSIDE THE `.cpp`, which is
 * the part worth carrying forward rather than the correction itself. The sweep that found and
 * fixed the other two grepped the `.cpp`; it was bounded by the FILE it searched, not by the
 * claim it searched for, and this site sat in the header the whole time. The transferable fact
 * is the boundary, not the count.
 * Nothing about THIS function changed, and no executable byte moved for either correction.)
 * An HP DELTA -- which is what a damage NUMBER on screen would need -- is exactly the second
 * subtraction
 * `StratTransientReceipts.cpp`'s own claim already forbids, and it is not here and is not owed.
 * A caller wanting an amount wants `FStratBridge::RepairsAtTurnOpen`'s shape, from the bridge.
 *
 * IT NAMES NO ATTACKER AND LABELS NO CAUSE, inheriting `EStratSoundCue::UnitAttacked`'s stated
 * limit rather than re-learning it: the attacker lives in `FStratCombatOutcome`, which
 * `StratBridge.h` says the bridge does not retain. A unit whose HP fell for any other reason is
 * reported identically, and whatever draws this must not claim otherwise.
 *
 * A KILLED UNIT IS NEVER REPORTED, AND THAT IS CORRECT RATHER THAN A GAP. A unit that died is
 * absent from `Model.Units` entirely -- `ApplyView`'s destroy loop is the authority for that
 * array being every LIVING unit -- so it has no HP to be lower and its actor has already been
 * destroyed. There is nothing left to flash.
 *
 * OVERWRITES `OutUnitIds` ENTIRELY, INCLUDING EMPTYING IT, on `StratDecideSoundCues`' discipline:
 * the output describes THIS refresh and never accumulates.
 *
 * IT ASKS THE RULES MODULE NOTHING. No `FStratBridge` in the signature or the includes; this is
 * a statement about two projections.
 */
STRATUI_API void StratDecideDamagedUnits(const FStratSoundMark& Mark,
                                         const FStratViewModel& Model,
                                         TArray<int32>& OutUnitIds);

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
