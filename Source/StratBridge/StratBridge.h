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
//
// THE RECORDED LOG BELOW IS NOT THAT EVENT LIST, and the two are easy to conflate
// because both are ordered and both come out of `Submit`. The log is §4.10's
// `commandLog` -- what was ASKED, in the format's own type, replayable -- and the
// event list is what HAPPENED, which is the thing a presentation layer animates.
// A save cannot be written from the second and a hit-flash cannot be driven from
// the first. Landing the log does not close `bridge_event_list`.
#pragma once

#include "CoreMinimal.h"

// Vendored rules headers, by short name via StratRules' PublicIncludePaths.
#include "Ai.h"
#include "Data.h"
#include "Replay.h"
#include "Save.h"
#include "Scenario.h"
#include "Ui.h"

// Engine-typed only, and includes nothing vendored. It is separate from this header so
// that a future UHT-parsed header can include it without inheriting this one's `strat`
// includes -- see its own prose block.
#include "StratCombatLog.h"

class UDataTable;

// The two §4.10 header fields this object CANNOT know, supplied by the caller.
//
// `rulesCommit` is the crew commit the vendored sources were taken at and
// `dataHash` is the digest over the §4.8 data set; both live in manifests this
// module does not read, and Save.h states the posture in as many words -- "every
// field is SUPPLIED, never recomputed here", which is what keeps save part (a)'s
// dependency set empty. A bridge that went and read a manifest to fill these in
// would be asserting agreement it is in no position to assert, and `checkHeader`
// would then compare a file against the file's own source of truth.
//
// The other three header fields are deliberately NOT here: `scenarioId` and
// `scenarioHash` come off the scenario this bridge actually loaded, and
// `stateHash` off the state it actually holds. Taking those as arguments too
// would let a caller write a save that describes a match this object is not in.
struct FStratSaveIdentity
{
	FString RulesCommit;
	FString DataHash;
};

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

// ---------------------------------------------------------------------------
// §2.9's opponent AI, in engine types.
//
// PLAIN AND NOT REFLECTED, exactly as `FStratResult` above is, and for that
// struct's reason: this header must declare no `UCLASS`/`USTRUCT`/`UENUM`, because
// the moment it declares one UHT parses it and the vendored `strat` headers it
// includes go in front of the header tool. A `UENUM` here would cost that, and no
// widget binds to an AI command -- the consumer is `StratPlay`'s C++, which can
// name a plain struct perfectly well.
//
// FOUR ENUMERATORS, NOT FIVE. `strat::AiCommandKind` declares `Build, Move, Attack, EndTurn` and
// this mirrors it exactly. There is deliberately no `None`: a fifth value would be
// a command the rules module cannot produce, and inventing one would let a caller
// believe the AI said something it has no vocabulary for. Capture is likewise
// absent, and that is the rules module's ruling rather than an omission here --
// §2.9's capture behaviour IS the Move onto the objective, and `openTurn`'s
// `captureTick` completes it at the turn boundary the EndTurn arm reaches.
enum class EStratAiCommandKind : uint8
{
	Build,
	Move,
	Attack,
	EndTurn,
};

// One `strat::AiCommand`, in `int32` and `FIntPoint`. X = q, Y = r, the same
// encoding `FStratUnitView::Hex` and the engine-typed façade already carry.
//
// `Hex` IS ALREADY RESOLVED FOR AN ATTACK, and that is the one field where this
// struct is not a field-for-field copy of the module's. `AiCommand::targetId` names
// a UNIT and `SubmitAttackAtHex` takes a HEX, so somebody has to look the unit up
// on the authoritative board -- and `StratPlay` cannot, because the board it can
// see is a view model rebuilt on its own schedule rather than the state the command
// was decided against. `NextAiCommand` does that lookup at the instant it asks, so
// `Hex` is submittable as it stands for Move, Attack and Build alike. `TargetId` is
// kept beside it for logging, and is NOT the thing to submit.
struct FStratAiCommand
{
	// Defaults mirror `strat::AiCommand`'s own, EndTurn included. A refused
	// `NextAiCommand` leaves this value here, and it is NOT distinguishable from a
	// genuine EndTurn by inspection -- `FStratResult::bOk` is the only thing that
	// tells them apart, which is why that method refuses rather than answering.
	EStratAiCommandKind Kind     = EStratAiCommandKind::EndTurn;
	int32               UnitId   = -1;    // Move, Attack
	int32               TargetId = -1;    // Attack -- for logging, never for submission
	FIntPoint           Hex      = FIntPoint(0, 0);   // Move dest, Build factory, Attack target hex
	int32               DefIndex = -1;    // Build -- an index into UnitDefs()
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
	//
	// IT IS ALSO THE ONE PLACE A COMMAND IS RECORDED, and that is why the five
	// typed methods below funnel through it rather than each appending for
	// themselves. `RecordedLog` claims to be every command this bridge applied;
	// recording per-caller makes that claim true only of the callers that
	// remembered, and the failure mode is a save that replays to a different
	// state with nothing to point at. Appended ONLY on success, so a rejected
	// command leaves the log exactly as §4.9 leaves the state.
	//
	// IT IS ALSO THE ONE PLACE A `STRAT-COMBAT` LINE IS EMITTED, for exactly the
	// reason it is the one place a command is recorded. `SubmitAttackAtHex` calls
	// `SubmitAttack` calls `SubmitStamped` calls this, and the two raw-command
	// callers in the tree call this directly, so this is the single point every
	// Attack passes through once -- a `…Logged` sibling of `SubmitAttack` would be
	// a second path a caller could take that skips the line, which is the failure
	// this placement exists to make unreachable. See StratCombatLog.h for the
	// record's shape and StratBridge.cpp's `StratCombatObservation` block for what
	// is captured on each side of `applyCommand`.
	//
	// TWO KINDS OF ATTACK REACH NO LINE AT ALL, and both matter to anyone counting.
	//
	// FIRST, an attack refused by the `!bSeeded` guard at the top of this method.
	// That return sits ABOVE the capture, so a command submitted to an unseeded
	// bridge produces neither `resolved` nor `refused`. Defensible -- there is no
	// state to project and nothing to measure, and a line of all -1 would say less
	// than no line -- but the consequence is load-bearing for a gate:
	// `resolved + refused` IS NOT THE COUNT OF ATTACKS SUBMITTED. A 1:1 pairing
	// gate that assumes it is would be silently off by however many commands
	// arrived before a scenario did.
	//
	// SECOND, `ReplayLog` DOES NOT PASS THROUGH HERE. Measured: it calls
	// `strat::replayLog` on the whole vector and appends to `Recorded` itself --
	// see `FStratBridge::ReplayLog` in the .cpp, cited by name because a line
	// number here has already gone stale twice in one diff.
	//
	// THE REASON FOR THAT SECOND EXCLUSION COVERS ONE SHAPE AND NOT THE OTHER, and
	// it is narrowed here deliberately rather than left to sound general. It holds
	// for the IN-PROCESS ROUND TRIP -- `ReplayRecordedLogOnto` hands a log to
	// `Fresh.ReplayLog`, and this process already emitted a line for every one of
	// those attacks when they first resolved, so observing them again would make
	// `grep -c "STRAT-COMBAT resolved"` count one match twice. It DOES NOT hold for
	// a log loaded from disk in a FRESH PROCESS: those attacks were never observed
	// by anyone, and today they replay silently. That case is a real gap, not a
	// deliberate exclusion, and nothing in the tree needs it closed yet.
	//
	// If a later phase needs replayed attacks observed, the fix is to route
	// `ReplayLog` through this method command by command AND to accept that it
	// stops being all-or-nothing at the module -- not to add a second emitter.
	FStratResult Submit(const strat::SaveCommand& Command);

	// ---- Typed commands (§4.9's five, and no others) ----------------------
	// One method per `strat::SaveCommandKind`, checked against that enum: Move,
	// Attack, Build, Capture, EndTurn. There is no Wait -- §2.11.1's "wait" is the
	// selection machine spending a unit's turn and it reaches no rules module, so
	// a sixth method here would be inventing a command the format cannot carry.
	//
	// THEY EXIST TO STAMP `{turn, side}`, which is the whole content of the
	// façade. §4.10 tags every log entry with them and `applyCommand` REFUSES an
	// entry whose tag disagrees with the live turn (`strat::applyCommand`'s turn-tag guard), so
	// a caller assembling a raw SaveCommand has to know the turn number and the
	// active side to get a command accepted at all -- two facts it would then be
	// holding a copy of. These read them off the authoritative TurnState at the
	// instant of submission and never from a caller argument. There is
	// deliberately no overload that lets a caller supply its own tag.
	//
	// STAMPED BEFORE APPLICATION, and that ordering is load-bearing for EndTurn
	// alone: the command that closes turn N is tagged N, and reading the tag back
	// off the state afterwards would tag it N+1 and make the log unreplayable at
	// the first turn boundary.
	//
	// `SubmitBuild` TAKES A defIndex AND SAYS SO. `SaveCommand::unitId` carries it in a field
	// spelled `unitId`, and `applyCommand` uses it as a raw bounds-checked index
	// into the definitions vector with no name lookup -- the reason `DT_Units`
	// row order is load-bearing (phase 0, `Tools/architect/state.md`). The
	// parameter is named for what the rules module does with it rather than for
	// what the format calls it, because the format's spelling is the trap.
	FStratResult SubmitMove(int32 UnitId, const strat::Hex& DestHex);
	FStratResult SubmitAttack(int32 UnitId, const strat::Hex& TargetHex);
	FStratResult SubmitBuild(const strat::Hex& FactoryHex, int32 DefIndex);
	FStratResult SubmitCapture(int32 UnitId);
	FStratResult SubmitEndTurn();

	// Replays a whole log, all-or-nothing (T-SAVE-05's property, the module's).
	//
	// RECORDS THE WHOLE LOG ON SUCCESS and nothing on failure, which follows from
	// what `replayLog` guarantees rather than being a second policy: it works on a
	// copy and assigns only after the last command succeeds, so "applied" and
	// "recorded" stay the same set. A caller that seeds and then replays a loaded
	// log therefore ends up with a recorded log equal to the one it loaded, which
	// is what makes save -> load -> save a fixed point.
	FStratResult ReplayLog(const TArray<strat::SaveCommand>& Log);

	// ---- Recorded log ----------------------------------------------------
	// Every command this bridge applied, in the order it applied them. §4.10's
	// `commandLog` field, and the input to `SerializeRecordedSave` below.
	//
	// NOT ON THE REFUSAL CHANNEL, deliberately, and the difference from
	// `Reachable` is the point rather than an inconsistency. An empty reach set is
	// never an answer -- `reachable` always includes the unit's own hex at cost 0 --
	// so there it is always a failure wearing one. An empty command log IS an
	// answer: it is what a freshly seeded match has, and a match that has taken no
	// command is an ordinary state and not a fault. There is nothing here for a
	// refusal to say.
	//
	// CLEARED BY A RESEED. `LoadDefinitions` and `LoadScenarioFromFile` both drop
	// it, because a log is only meaningful against the seed it was recorded from:
	// carried across a reseed it would serialize into a save whose `commandLog`
	// replays to a state its own `stateHash` disagrees with, and T-SAVE-06 would
	// then be catching the bridge rather than the format.
	const std::vector<strat::SaveCommand>& RecordedLog() const { return Recorded; }

	// The §4.10 file for the match this object is in: the recorded log above, the
	// header fields the caller supplies (see FStratSaveIdentity), the loaded
	// scenario's id and hash, and this state's canonical hash.
	//
	// SERIALIZES, AND DOES NOT WRITE. There is no path here, no FFileHelper call
	// and no slot: where a save lives is the save-slot UI's question and that is
	// out of this milestone. Handing back the text keeps the format testable
	// without a disk, which is the posture Save.h takes on `parseSave`.
	//
	// EVERY FIELD IS SOMEONE ELSE'S ANSWER. `formatVersion` is
	// `strat::kFormatVersion`; `scenarioHash` is `strat::scenarioHash` over the
	// retained scenario; `stateHash` is `strat::canonicalStateHash`; `seed` is 0
	// because Save.h says it MUST be and no RNG ships; and `result` is read off a
	// snapshot this bridge projected rather than compared here, so the
	// InProgress-is-null mapping stays `strat::buildUiSnapshot`'s and the spelling stays
	// `strat::tierName`'s. The bytes are `strat::serializeSave`'s.
	//
	// REFUSES ON AN UNSEEDED BRIDGE rather than emitting a save of an empty match,
	// on the same line MakeUiSnapshot holds: a caller must not be able to mistake
	// "nothing is loaded" for "a match in which nothing has happened".
	FStratResult SerializeRecordedSave(const FStratSaveIdentity& Identity, FString& OutText) const;

	// The inverse of `SerializeRecordedSave`: §4.10 text back into this bridge's
	// state. It is the load half the save milestone shipped without, and it is
	// here rather than in the caller because `strat::loadSave` and
	// `strat::replayLog` are `strat::` free functions -- measured 8 x LNK2019 the
	// first time a call to one crossed a module boundary -- and `ReplayLog` takes
	// a `TArray<strat::SaveCommand>` no engine module may spell. Engine-typed in,
	// engine-typed out, exactly the shape `SubmitMoveToHex` and `ReachableHexes`
	// took for the same reason.
	//
	// IT RESTORES ONTO AN ALREADY-SEEDED BRIDGE, and that is not a convenience.
	// A §4.10 file carries COMMANDS AND NOT A BOARD, so there is nothing here to
	// seed FROM; and `LoadDefinitions` / `LoadScenarioFromFile` both CLEAR the
	// recorded log, so a method that reseeded internally would have to reach past
	// its own inputs to do it. The caller runs `StartMatch`'s ordered sequence --
	// LoadDefinitions, then LoadScenarioFromFile -- and then calls this.
	//
	// THE EXPECTATION IS ASSEMBLED, NOT TAKEN WHOLE, and each field's source is
	// the one `SerializeRecordedSave` writes from:
	//   - `expectedVersion` is `strat::kFormatVersion`, the version this build
	//     writes and accepts.
	//   - `rulesCommit` and `dataHash` come off `Identity`, because
	//     `FStratSaveIdentity`'s own block says every field of it is SUPPLIED and
	//     never recomputed here -- a bridge that read a manifest to fill these in
	//     would be comparing a file against the file's own source of truth.
	//   - `scenarioHash` is `strat::scenarioHash` over the scenario THIS BRIDGE
	//     ACTUALLY LOADED, and is deliberately not a caller argument. Taking it
	//     from the caller would let a load succeed against a scenario this object
	//     is not seeded from -- the mirror of the reason `scenarioHash` is absent
	//     from `FStratSaveIdentity` on the write side.
	//   - `scenarioId` is NOT compared, and that is `strat::checkHeader`'s ruling
	//     rather than an omission here: Save.h's reading 2 states the refusal set
	//     is exactly §4.10's four Version-policy fields and `scenarioId` is not
	//     one of them. The hash is the bytes; the id is a label.
	//
	// THE FILE'S `stateHash` IS VERIFIED AND NOT TRUSTED. After the log replays,
	// `strat::canonicalStateHash` over the resulting state is compared against the
	// `stateHash` the file carried, and a disagreement REFUSES. That is the same
	// fixed point `T-SAVE-06` pins for save -> load -> save, and the load path is
	// the last place it should be assumed: a file whose log and whose hash disagree
	// describes two different matches, and silently preferring the log would put a
	// board on screen that no save ever recorded.
	//
	// ALL-OR-NOTHING ON THIS OBJECT'S OWN STATE, achieved by replaying onto a COPY
	// of `GameState` and assigning only after the hash agrees. That DEPARTS from
	// `Submit`, which applies directly on purpose so that T-INT-03's "no partial
	// application" clause measures the rules module rather than this file -- and
	// the departure is deliberate, because here the last guard is the hash check
	// and `replayLog`'s own rollback cannot see it. `replayLog` still owns
	// all-or-nothing WITHIN the log; the copy exists solely so a hash refusal
	// leaves the seeded board untouched. `Recorded` is assigned in the same step,
	// so "applied" and "recorded" stay the same set the way `ReplayLog` keeps them.
	//
	// REFUSES A BRIDGE THAT HAS ALREADY APPLIED COMMANDS, on
	// `ReplayRecordedLogOnto`'s arm 4 exactly: replaying a loaded log onto a played
	// match produces a state whose hash means nothing while looking like it means
	// something. An EMPTY `commandLog` is NOT refused, and that is where this
	// method departs from `ReplayRecordedLogOnto`'s arm 5 -- a save taken before
	// anything was submitted is an ordinary file, and its hash check still has
	// content because it pins the seed.
	//
	// A GAP, RECORDED AND NOT FIXED: §4.10 CARRIES NO `firstSide`. The header is
	// `{formatVersion, rulesCommit, dataHash, scenarioId, scenarioHash, seed,
	// commandLog, stateHash, result}` and none of those is the side that moved
	// first -- yet `LoadScenarioFromFile` takes one and it changes the seeded
	// state. A caller that re-seeds with the WRONG side therefore reaches the hash
	// check and is REFUSED, which is safe (no wrong board reaches the screen) but
	// is NOT RECOVERABLE: this method cannot tell "wrong first side" from
	// "corrupted log", because the file gives it nothing to tell them apart with.
	// The fix is not to change the format -- Source/StratRules/ and Data/ are
	// vendored certified bytes, hash-gated by T-INT-01. The fix is engine-side and
	// lives in `UStratSaveGame` (StratPlay), whose payload carries the scenario
	// file and the `FirstSide` the match was seeded with alongside this text, so a
	// slot load re-seeds with the side it saved with. This method deliberately does
	// not know about that payload; it takes the text.
	//
	// `OutCommandCount` IS THE LOG LENGTH APPLIED, so a caller can log it and a
	// clause can pin it against `RecordedCommandCount()` without spelling
	// `strat::SaveCommand`. Written only on success; zeroed first either way.
	FStratResult RestoreFromSaveText(const FString&            SaveText,
	                                 const FStratSaveIdentity& Identity,
	                                 int32&                    OutCommandCount);

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

	// §2.11.6's two authored hexes for one seat: the deployment hex of that seat's marked
	// Infantry, and the neutral Factory hex it walks to. `strat::ScenarioGuided`'s two
	// members, in `FIntPoint` with X = q and Y = r.
	//
	// A LOOKUP AND NOT A MEASUREMENT, and that is the whole reason it is safe to expose.
	// §2.11.6 states it in as many words: "Nothing is measured at runtime and no 'nearest
	// objective' heuristic is used -- the lane is authored, machine-validated, and recorded
	// as a number by `validate_scenario`". This method reads `guidedOpening.objective` off
	// the retained scenario and returns it. It ranks nothing, searches nothing, and would
	// be the wrong place to start if it did.
	//
	// IT EXISTS BECAUSE THE SNAPSHOT CARRIES THE UNIT AND NOT THE FACTORY. `UiUnitView::
	// isGuidedMarked` projects `guidedOpening.infantry` per unit, so beat 1a's marked unit
	// reaches the view model already. `guidedOpening.objective` is projected NOWHERE --
	// `UiSnapshot` has no field for it and `UiHexView` / `UiFactoryView` carry no guided
	// flag -- so the ring §2.11.6-B requires "from turn 1" has no source at all without
	// this. The alternative was for the guidance layer to find the objective itself, by
	// distance or by "the nearest neutral factory": that is precisely the derived duplicate
	// of an authored answer this project's T-UI-02 exists to catch, and §2.11.6 forbids the
	// heuristic by name.
	//
	// ENGINE-TYPED BECAUSE THE CALLER IS StratPlay, which may not spell `strat::Hex` or
	// call `strat::` anything -- measured 8 x LNK2019. Same shape as `ReachableHexes`.
	//
	// REFUSES WHEN THE SCENARIO NAMES NO GUIDED OPENING FOR THAT SIDE, rather than handing
	// back (0,0). Ui.h says a scenario with no `guided` entries "marks nobody rather than
	// being an error", and that stays true of `isGuidedMarked`; but a caller asking WHERE
	// the objective is has asked a question with no answer, and (0,0) is a real hex on this
	// board. A refusal is how the guidance layer learns to run no guided opening at all.
	//
	// THE INFANTRY HEX IS THE DEPLOYMENT HEX AND NOT THE UNIT'S CURRENT HEX. `ScenarioGuided`
	// stores a placement reference, which is exactly why `isGuidedMarked` is derived from
	// `placement` and never from `hex` -- so that beat 1a's own move cannot unmark the unit
	// the beat is about. A caller that used this to locate the marked unit AFTER it moved
	// would be reintroducing that bug; use `FStratUnitView::bIsGuidedMarked` for the unit.
	FStratResult GuidedOpeningHexes(int32       Side,
	                                FIntPoint&  OutInfantryDeployHex,
	                                FIntPoint&  OutObjectiveHex) const;

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

	// ---- Rules queries ---------------------------------------------------
	// T-UI-02's set for one unit, as `strat::uiReachable` returns it -- hex for
	// hex, cost for cost, in the module's canonical order.
	//
	// WHY IT IS A METHOD HERE RATHER THAN A CALL AT THE CALL SITE: the same
	// measurement this header opens with. `strat::uiReachable` carries no `_API`
	// macro, so a movement-highlight actor or a widget that called it directly
	// would not link -- the 8 x LNK2019 recorded above, and the single LNK2019
	// CheckSnapshotFidelity records from the StratUI test that tried it. Every
	// rules answer a presentation layer needs arrives as a method on this class;
	// this is the movement one, and it is the first of them a gameplay module
	// (rather than a test) will reach for.
	//
	// WHAT IT FORECLOSES, AND DELIBERATELY: a hex-distance filter standing in
	// for a real query. Move.h weights cost by terrain and blocks pathing on
	// occupancy, so `distance <= move` agrees with the rules on an empty plain
	// and lies everywhere else. T-UI-02 exists to catch that substitution, and
	// a gate can only prefer the routed query if there IS a routed query. This
	// is it.
	//
	// COMPUTES NOTHING, on the same line MakeUiSnapshot holds. It gathers the
	// same `UiWorld` and forwards the module's vector unchanged -- no filter, no
	// sort, no re-cost, no truncation. If a cost here is wrong it is wrong in
	// Move.cpp.
	//
	// REFUSES RATHER THAN HANDING BACK AN EMPTY SET. `uiReachable` returns the
	// empty vector for an unknown unit id and for missing tables, and those are
	// its ONLY empty results -- `reachable` always includes the unit's own hex at
	// cost 0, the null move -- so an empty vector is never an answer, it is
	// always a failure wearing one. The caller cannot tell which failure from the
	// vector; this method says. A successful call therefore always yields at
	// least one entry.
	FStratResult Reachable(int32 UnitId, std::vector<strat::ReachEntry>& OutReach) const;

	// T-UI-01's pre-commit forecast for one attack, as `strat::uiForecast` returns
	// it -- distance, damage, whether the defender dies, whether a counter fires
	// and for how much. Routed here for the same reason `Reachable` is: the symbol
	// carries no `_API` macro and a widget calling it directly is the LNK2019 this
	// header opens with.
	//
	// TWO CHANNELS, AND THEY ANSWER DIFFERENT QUESTIONS. `FStratResult` says
	// whether the query could be ASKED; `OutForecast.legal` says what the rules
	// ANSWERED. "Out of range", "same side", "no unit on that hex" and "a unit
	// cannot attack itself" are all answers -- the module states each with its own
	// reason and a UI shows them -- so they come back as Ok() with `legal` false,
	// and folding them into a refusal would make an ordinary hover look like a
	// fault. A refusal here means the bridge is not loaded, is not seeded, holds a
	// unit outside its own table, or was handed an attacker id that does not exist.
	//
	// THE UNKNOWN-ATTACKER CASE IS A REFUSAL, matching `Reachable` exactly and
	// deliberately not left to the module. `uiForecast` would answer it with
	// `legal` false and "no such unit", which is spelled the same way as "out of
	// range" and is not the same kind of thing: one is the rules declining an
	// attack, the other is the caller naming a unit that is not on the board. The
	// resulting rule is crisp and is what a gate can pin -- bOk false means the
	// question was malformed, bOk true means the rules answered it.
	//
	// COMPUTES NOTHING. Not the distance, not the damage, not the counter. If a
	// number here is wrong it is wrong in Combat.cpp, which is the property §2.6's
	// "the forecast is exactly what resolves" rests on.
	FStratResult Forecast(int32 AttackerId, const strat::Hex& DefenderHex,
	                      strat::UiForecast& OutForecast) const;

	// §2.11.5's production menu, as `strat::uiBuildOptions` returns it: one entry
	// per §2.4 unit-table row, each carrying its price, whether this side can pay
	// it, and whether this factory will take a build at all. Routed here for the
	// reason the two queries above are -- the symbol carries no `_API` macro.
	//
	// A QUERY AND NOT A SNAPSHOT FIELD, which is a ruling rather than a preference.
	// Every `UiSnapshot` field is pinned by T-UI-05's enumeration, so carrying the
	// buildlist there would move that invariant and make every consumer of the
	// snapshot hold a value only §2.11.5 reads. `MakeUiSnapshot` is unchanged by it.
	//
	// TWO CHANNELS, exactly as `Forecast` has them. `FStratResult` says whether the
	// query could be ASKED; `available` and `reason` on each entry say what the rules
	// ANSWERED. A factory this side does not hold, one that has already taken its
	// build this turn, one already holding a waiting build, and a hex that is not a
	// build point are all ANSWERS -- the menu shows each with the module's own reason
	// -- so they arrive as Ok() with `available` false. THE FACTORY HEX IS THEREFORE
	// NOT PRE-CHECKED HERE, deliberately, the way `Forecast` leaves the defender to
	// the module.
	//
	// THE SIDE IS THE ONE MALFORMED-QUESTION CASE and is refused rather than left to
	// the module, matching `Forecast`'s treatment of an unknown attacker id.
	// `uiBuildOptions` would answer a side outside the match with `available` false
	// and "invalid side" -- spelled the same way as "not a build point", and not the
	// same kind of thing. Refusing here keeps the rule crisp (bOk false means the
	// question was malformed, bOk true means the rules answered it) and makes the
	// module's own "invalid side" unreachable through this bridge.
	//
	// REFUSES RATHER THAN HANDING BACK AN EMPTY SET, on `Reachable`'s reasoning.
	// `uiBuildOptions` returns the empty vector only when the world carries no unit
	// table, so an empty result is never an answer: the menu has one row per §2.4
	// row whatever the board looks like. A successful call therefore always yields
	// exactly as many entries as the loaded unit table has rows.
	//
	// COMPUTES NOTHING -- not the affordability, not the availability, not the reason.
	// `affordable` is the MODULE's arithmetic precisely so that T-UI-03's
	// no-widget-side-arithmetic clause has something to bind to; recomputing it here
	// would put a second spelling of it one module closer to the screen.
	FStratResult BuildOptions(int32 Side, const strat::Hex& FactoryHex,
	                          std::vector<strat::UiBuildOption>& OutOptions) const;

	// §2.8's result WHOLE, as `strat::uiMatchResult` returns it: the tier, the cause,
	// the WINNING SIDE and the §2.8 key a capped match was decided on. Routed here for
	// the reason the three queries above are -- the symbol carries no `_API` macro.
	//
	// WHAT GAP IT CLOSES, and it is a projection loss rather than a missing rule.
	// `MakeUiSnapshot`'s `UiMatchView` carries `resultTier` and drops `winner`, `cause`
	// and `decidedByKey`, so before this method existed every consumer downstream of the
	// projection could say *Decisive* and could not say FOR WHOM. T-TURN-02 grades a flag
	// kill "Decisive win for the KILLER" and T-TURN-04 decides a capped match on a NAMED
	// criterion; neither second half was assertable outside the rules module at all.
	//
	// `sideToMove` IS NOT A STAND-IN FOR `winner`, and this is the trap this method
	// exists to keep shut. Upstream states it at the field: the two agree on a flag kill
	// only because the killer happened to be the side to move, and they disagree at the
	// cap, where the match ends at a turn boundary. A caller deriving one from the other
	// is right in the common case and silently wrong in the case a tiebreak decides.
	//
	// A FOURTH `ui*` QUERY AND NOT A SNAPSHOT FIELD, which is upstream's ruling and not
	// this file's preference -- `Ui.h` states it at the declaration. Every `UiSnapshot`
	// field is pinned by T-UI-05's enumeration, so a `winner` there would move
	// `kUiSnapshotFieldCount`, `kUiMirrorFieldCount`, `kUiDerivedFieldCount`, the
	// transcribed `uiFieldContract()` table and `uiEnumerateSnapshot`, and every consumer
	// of the snapshot would carry that move to hold a value only §2.11.4's end-of-match
	// screen reads. `MakeUiSnapshot` is untouched by this method, and that is the point.
	//
	// ONE CHANNEL AND NOT TWO, which is the difference from `Forecast` and `BuildOptions`
	// and is stated because those two sit directly above it. There is no question here
	// the rules can DECLINE: `uiMatchResult` is a pure mirror of `TurnState::result` with
	// no legality anywhere in it, so `FStratResult` carries only whether the bridge could
	// be ASKED. Nothing on the returned struct is a refusal wearing an answer's clothes.
	//
	// AN UNSEEDED BRIDGE IS REFUSED, AND UPSTREAM ANSWERS THE SAME CASE. `Ui.h` rules
	// that a world with no `turn` reports the default -- InProgress, SIDE_NONE -- because
	// the only safe thing a caller can do with a missing input and an unfinished match is
	// the same thing. That ruling is right where it is made and wrong here: this class
	// HAS `IsSeeded()` and can tell the two apart, and a victory screen handed "no result
	// yet" by a bridge that was never loaded would wait forever on a match that does not
	// exist. The divergence is deliberate and is the only place this method is not a
	// straight pass-through.
	//
	// COMPUTES NOTHING. Not the tier, not the winner, not the tiebreak key, and it does
	// not consult `hasResult` to decide whether the other fields are meaningful -- that
	// is the caller's read of `Tier`, exactly as `FStratMatchView::bHasResult` is.
	FStratResult MatchResult(strat::UiMatchResult& OutResult) const;

	// ---- The engine-typed façade -----------------------------------------
	// EVERY METHOD IN THIS SECTION SAYS, IN `int32` AND `FIntPoint`, EXACTLY WHAT THE TYPED
	// METHOD OF THE SAME SUBJECT SAYS -- EXCEPT `Turn()` AND `SideToMove()`, WHICH MIRROR
	// NOTHING. Those two read `GameState.turn` directly (`return GameState.turn.turnNumber;`
	// and `return GameState.turn.activeSide;`) and have no typed counterpart anywhere in this
	// class. They live here because a caller of these methods needs them, not because
	// anything was translated for them.
	//
	// NO COUNT IS STATED FOR EITHER SECTION, AND THE ABSENCE IS THE CORRECTION RATHER THAN AN
	// OMISSION. This sentence has been wrong twice, and the second time was its own fix:
	// RETRACTED> "The six methods below say exactly what the five above say, in `int32` and
	// RETRACTED>  `FIntPoint`"
	// RETRACTED> "The six methods below say exactly what SIX OF THE SEVEN above say, in
	// RETRACTED>  `int32` and `FIntPoint`"   (2026-08-25, itself replacing the line above)
	// BOTH DEPENDED ON THE WORD "ABOVE", WHICH THIS HEADER NEVER DEFINES, and the ambiguity is
	// not academic -- three readings were derived on 2026-08-25 and they give different
	// answers: SIX (the two sections above this banner, less `MakeUiWorld`), SEVEN (those
	// sections, since `MakeUiWorld` returns `strat::UiWorld` and is public -- see "PRIVATE,
	// WHERE `MakeUiWorld` IS PUBLIC" further down this file) and SEVENTEEN (every method in
	// the class whose signature names a `strat::` type). TWO CAREFUL READERS HAD TO DISAGREE
	// BEFORE ANYONE COULD SEE THAT THE SCOPE, NOT THE NUMBER, WAS THE DEFECT. A cardinal in
	// prose goes stale silently and cannot be checked without first settling a convention
	// nobody wrote down; a NAMED exception is checkable by a reader standing in one place,
	// which is why the sentence above names `Turn()` and `SideToMove()` instead of counting.
	//
	// `MatchResult` HAS NO MIRROR HERE AND NEEDS NONE, AND THAT IS DELIBERATE RATHER THAN AN
	// OMISSION. The mirrors exist because `StratPlay` cannot spell a `strat::` type to
	// call the typed form -- but `MatchResult`'s consumer is `StratUI`, which MAY name one
	// (`StratViewModel.cpp` names `strat::UiSnapshot` and `strat::UiBuildOption` already),
	// and its projection `StratBuildMatchResult` hands `StratPlay` an `FStratMatchResultView`
	// that names nothing vendored. So the translation happens once, in `StratUI`, and a
	// mirror here would be a second one. If a gameplay-side caller ever needs the result
	// WITHOUT going through the view model, it gets a mirror and this paragraph gains a name.
	//
	// The mirrors exist for one reason: `StratPlay` NAMES NO `strat::`
	// TYPE. That is not a style rule it could bend -- `StratPlay.Build.cs` and
	// `StratMatchSubsystem.h` both state it, and the actor headers that would
	// otherwise carry such a type are parsed by UHT, which must never see the
	// vendored headers. A gameplay caller therefore cannot spell
	// `std::vector<strat::ReachEntry>` or `strat::Hex` to CALL the typed methods
	// above, even though calling them would link perfectly well.
	//
	// SO THE TRANSLATION HAPPENS HERE, IN THE ONE MODULE THAT IS ALLOWED TO SEE
	// BOTH WORLDS, and it happens exactly once. The alternative was a translation
	// helper in `StratPlay` that named `strat::Hex` in a .cpp only -- legal by the
	// letter of the linker rule, since naming is not calling -- and it was rejected
	// because it puts a second spelling of the axial coordinate in the module that
	// is supposed to have none, and because the next person to need one puts it in
	// an actor. `FIntPoint::X` is `q` and `FIntPoint::Y` is `r`, the same encoding
	// `FStratHexView::Hex` and `FStratUnitView::Hex` already carry, so a gameplay
	// caller passes the view model's own value straight back in.
	//
	// THEY ADD NO POLICY. Each one THAT HAS a typed method beside it forwards to it and
	// converts the container; every refusal is the typed method's, in its words.
	// The one exception to the no-policy rule is documented on `AttackTargetHexes`,
	// and it is an enumeration rather than a rule.
	//
	// `Turn()` AND `SideToMove()` ARE OUTSIDE THAT GUARANTEE, and the qualifier above exists
	// for them. They forward to nothing and convert nothing -- they read `GameState.turn`
	// directly -- so "every refusal is the typed method's" is VACUOUS for them rather than
	// true: they cannot refuse at all, and both return 0 on an unseeded bridge, which their
	// own declaration is explicit is not a sentinel.
	// THIS SENTENCE READ "Each one forwards to the typed method beside it" WITH NO QUALIFIER,
	// and it is corrected here as well as in the banner deliberately: it states a GUARANTEE
	// rather than a count, so a reader is likelier to rely on it, and a correction that
	// reached only the banner would have left the false claim standing exactly where it does
	// work.
	//
	// OUT OF LINE, DELIBERATELY, AND WITHOUT EXCEPTION IN THIS SECTION -- established by
	// inspection (no declaration here carries a body) rather than by a count. THE CARDINAL IS
	// GONE FROM THIS SENTENCE TOO: it read "all six" and this section declares seven, so it
	// was a THIRD instance of the same defect inside one block.
	// An inline body would force the caller's translation unit to instantiate over
	// `strat::Hex` and defeat the whole point; it would also hide the method from the link
	// line, which is how the 4 x LNK2019 measured on `StratPlay` stayed invisible until a
	// real caller existed (`StratPlay.Build.cs`).
	//
	// THAT REASON DOES NOT REACH `Turn()` AND `SideToMove()`, which name no vendored type and
	// could have been inline without instantiating anything. They are out of line anyway, and
	// nothing in this block depends on why -- stated so that a reader does not take the
	// instantiation argument as covering a case it does not.

	// §4.10's `{turn, side}` stamp, readable before a command is submitted.
	//
	// READ BEFORE, NOT AFTER, AND THAT IS THE WHOLE REASON THESE EXIST. The five
	// `Submit*` methods stamp the command with the turn and side live at the
	// instant of submission -- "the command that closes turn N is tagged N" -- so a
	// log line that wants to report which turn a command belonged to must read
	// these first. Reading them afterwards tags an `EndTurn` with N+1 and describes
	// a command that was never submitted. `AStratPlayerController` does exactly
	// this for its `STRAT-CMD accepted` line.
	//
	// Both return 0 on an unseeded bridge, which is not a sentinel: an unseeded
	// bridge holds a default-constructed `TurnState` and this is faithfully what it
	// says. Ask `IsSeeded()` to tell that apart from a real turn 0.
	int32 Turn() const;
	int32 SideToMove() const;

	// `Reachable`, as parallel arrays. `OutHexes[i]` is reached at `OutCosts[i]`,
	// in the module's canonical order, and the two are always the same length.
	//
	// TWO ARRAYS RATHER THAN AN `FStratReachEntry` STRUCT, because the only
	// consumer that exists -- the movement overlay -- wants the hexes and not the
	// costs, and a reflected mirror of `ReachEntry` would be a third spelling of a
	// value `FStratViewModel` deliberately does not carry. When a §2.11 cost
	// readout needs one, `OutCosts` is already here to build it from.
	//
	// THE REFUSAL IS `Reachable`'s AND SO IS THE NON-EMPTINESS. A successful call
	// yields at least one entry -- the unit's own hex at cost 0 -- so an empty
	// result is always a failure wearing one, and this method never manufactures
	// the empty answer that a hex-distance filter would produce for a surrounded
	// unit. T-UI-02 measured 122 divergent hexes across 10 of 10 units between the
	// real query and `distance <= move`; this is the routing that makes the real
	// one the only one reachable from gameplay code.
	FStratResult ReachableHexes(int32 UnitId, TArray<FIntPoint>& OutHexes,
	                            TArray<int32>& OutCosts) const;

	// Every hex this unit may legally attack right now, ascending unit id.
	//
	// AN ENUMERATION, NOT A RANGE CHECK, and the distinction is the only reason
	// this is defensible in a file that computes nothing. It walks the live unit
	// list, and for each unit not on the attacker's side it ASKS `Forecast` --
	// which is `strat::uiForecast` and nothing else -- keeping the hex when the
	// module answers `legal`. No distance is compared here, no range is read off a
	// `UnitDef`, and no §2.6 rule is restated. If a target is missing it is missing
	// from `uiForecast`, and T-UI-01 is the gate that says so.
	//
	// WHY IT IS HERE AND NOT IN THE SELECTION MACHINE: `uiForecast` carries no
	// `_API` macro, so the loop can only run in this module (8 x LNK2019, this
	// header's opening measurement). A gameplay-side loop would have had to
	// approximate, and approximating §2.6 is the same substitution T-UI-02 catches
	// for movement.
	//
	// THE RULES MODULE OFFERS NO TARGET ENUMERATION OF ITS OWN. `Ui.h` declares
	// `uiReachable` for movement and has no counterpart for attack; this is that
	// counterpart, assembled from single-target answers rather than invented. If
	// one is ever vendored, this method's body becomes a forward to it and no
	// caller changes.
	//
	// AN EMPTY RESULT IS AN ANSWER HERE, unlike `ReachableHexes`, and for the
	// reason `RecordedLog` is off the refusal channel: a unit with nothing in
	// reach is an ordinary board position, and there is no null move for attack the
	// way there is for movement.
	FStratResult AttackTargetHexes(int32 AttackerId, TArray<FIntPoint>& OutHexes) const;

	// `SubmitMove` and `SubmitAttack`, in `FIntPoint`. X = q, Y = r. They stamp
	// `{turn, side}` and record exactly as their typed counterparts do, because
	// they ARE their typed counterparts with one conversion in front.
	FStratResult SubmitMoveToHex(int32 UnitId, FIntPoint DestHex);
	FStratResult SubmitAttackAtHex(int32 UnitId, FIntPoint TargetHex);

	// `SubmitBuild`, in `FIntPoint`. It closes the one hole in the façade above:
	// `SubmitBuild` names `strat::Hex` and had no engine-typed counterpart, so Build
	// was the single §4.9 command a module that cannot spell `strat::` could not
	// issue. §2.9's AI emits Build as one of its four kinds, so that hole was about
	// to become load-bearing.
	//
	// `DefIndex`, AND IT IS THE SAME LOAD-BEARING INDEX AS EVERYWHERE ELSE. It is
	// what `SaveCommandKind::Build` carries in the field the format spells `unitId`,
	// and `applyCommand` uses it as a raw bounds-checked index into the definitions
	// vector with no name lookup -- the reason `DT_Units` row order is load-bearing
	// (phase 0). This method does NOT resolve a name; `SetBuildlistByIds` below is
	// where names are resolved, exactly once, against `UnitDefs()`.
	//
	// ROUTED THROUGH `SubmitBuild`, so it stamps `{turn, side}` and records in
	// `RecordedLog()` on the same path as every other command. A second, non-
	// recording apply path is the precise defect `ReplayRecordedLogOnto` was added
	// to make visible; this method must never become one.
	FStratResult SubmitBuildAtHex(FIntPoint FactoryHex, int32 DefIndex);

	// ---- §2.9's opponent AI ----------------------------------------------
	// The AI's next command for `Side`, as `strat::nextCommand` returns it.
	//
	// ROUTED HERE FOR THIS HEADER'S OPENING MEASUREMENT. `strat::nextCommand`
	// carries no `_API` macro, so a subsystem or controller that called it directly
	// would not link -- the 8 x LNK2019 above. `Ai.good.cpp` is compiled INTO this
	// module (`Vendored/Ai.strat.cpp`), so this is the only place in the tree the
	// call can be made from at all.
	//
	// IT DECIDES NOTHING, on the same line `MakeUiSnapshot` and `Reachable` hold.
	// Every arm of §2.9 -- which unit, which target, whether an exchange trades
	// down, what to build -- is `Ai.h`'s. This method GATHERS an `AiState` and
	// forwards, and the only transformation it performs on the answer is the target
	// hex resolution documented on `FStratAiCommand::Hex`, which is a lookup on the
	// authoritative board rather than a rule.
	//
	// THE ATTACK ARM RESOLVES `targetId` -> `Hex` HERE, DELIBERATELY. The
	// alternative was handing `TargetId` to `StratPlay` and letting it find the hex
	// on the view model. That was rejected twice over: the view model is rebuilt on
	// the caller's own schedule, so it can describe a board one command older than
	// the one the AI decided against; and it would put a "which unit is on which
	// hex" derivation in the module that is supposed to hold no rules answers. If
	// the named target is not on the board this REFUSES rather than submitting an
	// attack at (0,0) -- `AiCommand::hex` is default-constructed for an Attack, and
	// (0,0) is a real hex on this board.
	//
	// REFUSES ON AN UNLOADED OR UNSEEDED BRIDGE, and `Reachable`'s
	// `RefusesUnseeded` clause is the shape. An unseeded bridge holds a default
	// `GameState` with no units, and `nextCommand` over it answers EndTurn --
	// perfectly reasonably, and indistinguishably from a turn in which a live AI
	// genuinely has nothing left to do. A caller that drove an AI turn loop against
	// an unseeded bridge would therefore see a clean, instant, empty turn and no
	// fault of any kind. That is the failure this refusal exists to make loud.
	//
	// AN UNKNOWN `Side` IS A REFUSAL for the reason `Forecast`'s unknown attacker
	// is: the rules module would answer it (`nextCommand` finds no units for side
	// 7 and says EndTurn), and "the AI has finished" and "you asked about a side
	// that does not exist" are not the same kind of thing.
	FStratResult NextAiCommand(int32 Side, FStratAiCommand& Out) const;

	// §2.9's default buildlist, set BY UNIT ID and resolved here against
	// `UnitDefs()`.
	//
	// BY NAME AND NOT BY INDEX, AND THAT IS THE WHOLE POINT OF THE METHOD.
	// `AiState::buildlist` is a vector of defIndexes, and a defIndex is the same
	// raw, bounds-checked-only index a §4.10 Build command carries -- phase 0's
	// hazard, the one that resolves the same log to a different unit type in
	// silence. Only this module can see `UnitDefs()`, so only this module can turn
	// "Infantry" into that index; a defIndex-only setter would push the resolution
	// out to a caller that would end up hand-writing 0 and 1 into a config, which
	// is precisely how the hazard gets reintroduced.
	//
	// REFUSES ON ANY UNRESOLVABLE ID AND NAMES IT, never substituting a default
	// (§4.8's posture). All-or-nothing: the stored list is replaced only after every
	// entry resolved, so a partially-resolved buildlist cannot exist -- the same
	// discipline `loadUnits` uses when it leaves `out` untouched on any defect. A
	// silently-dropped entry would change §2.9's build MIX, which is data with no
	// other witness in the tree.
	//
	// DUPLICATES ARE LEGAL AND ARE PRESERVED, AND THIS METHOD IS THE ONLY LAYER
	// THAT CARES. §2.9 describes "mostly Infantry, an occasional Tank" and gives no
	// ratio, so `AiState::buildlist`'s own comment makes the list caller-supplied DATA and this method
	// stores it verbatim: no dedupe, order kept, repetition kept.
	//
	// BUT REPETITION DOES NOT EXPRESS A RATIO AT THE RULES LAYER, and an earlier
	// spelling of this comment said it did. `strat::chooseBuild` (`Ai.good.cpp`)
	// collects every AFFORDABLE entry and then returns the single
	// `buildPriorityLess` winner among them -- so a list of {Infantry, Infantry,
	// Tank} and a list of {Infantry, Tank} choose identically, every time, and with
	// Infantry at 100 Fame and Tank at 300 the Tank entry is unreachable at any
	// repetition count. §2.9's "an occasional Tank" is not an observable outcome of
	// this setter. The property this method actually holds is that the bytes handed
	// to `AiState::buildlist` are the caller's own; what consumes them is a
	// vendored-behaviour question for `E:\MultiAgent\stratocracy-crew` and not a
	// defect in this file.
	//
	// DEDUPLICATING HERE WOULD STILL BE WRONG, for a smaller reason than the one
	// this comment used to give: it would make the stored list differ from the
	// caller's, so `BuildlistDefIndexes()` would stop being a readback, and it
	// would foreclose a future `chooseBuild` that DOES weight by repetition.
	//
	// AN EMPTY LIST IS ACCEPTED and configures an AI that never builds --
	// `chooseBuild` returns -1 with nothing affordable to choose. It is an ordinary
	// configuration, on `RecordedLog`'s line, and it is distinguishable from the
	// unresolvable-id case because that one refuses.
	//
	// COMPARED AS EXACT UTF-8 BYTES, not by `FName`'s own operator==. `FName`
	// compares case-insensitively, so `SetBuildlistByIds({"infantry"})` would
	// resolve under that comparison while `Data/units.csv` says `Infantry` -- a
	// widening of the §4.8 id space decided here rather than by the data. The
	// argument is `FName` because that is what the view model and a `DataTable` row
	// name are; the MATCH is on the bytes.
	//
	// CLEARED BY `LoadDefinitions`, for `RecordedLog`'s reason exactly: the stored
	// values are indexes into the vector that just moved, so after a reload they do
	// not merely go stale, they name different unit types.
	FStratResult SetBuildlistByIds(const TArray<FName>& UnitIds);

	// The resolved buildlist, as defIndexes into `UnitDefs()`, in the order it was
	// set. Empty on a bridge whose buildlist was never set or whose definitions were
	// reloaded underneath it.
	//
	// THE DEFINDEXES AND NOT THE NAMES, because the defIndexes are what is stored
	// and what `AiState::buildlist` receives. A names reader would have to project
	// back through `UnitDefs()`, which is a second derivation that can disagree with
	// the first -- and the value a caller would then be checking is not the value the
	// AI is handed.
	TArray<int32> BuildlistDefIndexes() const;

	// ---- The recording joint, in engine types -----------------------------
	// `RecordedLog().size()` and "replay it onto another bridge", reachable from a
	// module that may not name `strat::SaveCommand`.
	//
	// THEY EXIST BECAUSE THE JOINT WAS UNPINNABLE, and that is the whole reason,
	// so it is written down rather than left to be inferred. Phase 4 shipped two
	// green clauses either side of it: `T-SAVE-05.HotSeatReplayParity` pins
	// clicks -> outcomes -> submissions, and `T-SAVE-05.RecordedLogReplaysToEqualHash`
	// pins `SubmitMoveToHex`/`SubmitEndTurn` -> `RecordedLog()` -> replay -> equal
	// hash. Neither pins that `StratSubmitSelectionCommand` calls a *recording*
	// entry point at all. The StratPlay clause could not: it drove both bridges
	// through the same submission function, so any path -- recording or not --
	// yielded equal hashes, and its command count came from log lines that same
	// function emitted. Subject and witness were one object. Route a fourth arm of
	// `StratSelectionMachine.cpp`'s switch through a non-recording apply and both
	// clauses stay green while `RecordedLog()` comes back empty after a full
	// hot-seat session -- exactly the property §4.10 needs and exactly what phase
	// 6's PIE gate leans on when it compares a `STRAT-CMD accepted` line against a
	// `commandLog` entry.
	//
	// `StratPlay` CANNOT NAME `strat::SaveCommand`, so it could not read
	// `RecordedLog()` and could not build the `TArray<strat::SaveCommand>` that
	// `ReplayLog` takes. These two are the same shape as the six engine-typed
	// façade methods above and for the same reason: a module-side value to read
	// instead of a proxy the code under test produced
	// (`.agents/ue-project-context.md:189-191`).
	//
	// `RecordedCommandCount` IS OFF THE REFUSAL CHANNEL, exactly as `RecordedLog`
	// is: zero is an answer, it is what a freshly seeded match has, and there is
	// nothing here for a refusal to say.
	//
	// `ReplayRecordedLogOnto` IS ON IT, and refuses six things rather than
	// no-opping, because a no-op and a full replay are indistinguishable at the
	// call site and the whole point of this method is to be distinguishable.
	// Six here means six guard arms in this method's own body, in this order;
	// `ReplayLog`'s own refusals -- the per-command index, the all-or-nothing
	// rollback -- are downstream of arm 6 and are deliberately not counted:
	//
	//   1. `&Fresh == this` -- replaying a log onto its own recorder would apply
	//      every command a second time and append it a second time.
	//   2. this bridge unseeded -- there is no match here to replay.
	//   3. `Fresh` unseeded -- phase 1's `RefusesUnseeded` is the local precedent.
	//   4. `Fresh` already carries commands -- the target is meant to be fresh, and
	//      replaying onto a played match produces a hash comparison that means
	//      nothing while looking like it means something.
	//   5. an EMPTY recorded log here. This is the one that DEPARTS from
	//      `RecordedLog`'s "empty is an ordinary answer" posture, deliberately: as
	//      a *query* an empty log is a fact about a match, but as the *input to a
	//      replay* it is a no-op that returns Ok() and equal hashes and proves
	//      nothing -- which is the precise failure this method was added to make
	//      visible. Refusing it means a caller that forgot to assert the count
	//      still fails instead of passing vacuously.
	//   6. `Fresh` seeded from a different scenario, compared by
	//      `strat::scenarioHash` over the two retained scenarios rather than by
	//      scenario id, because the id is a label and the hash is the bytes. A
	//      mismatched target would refuse partway through `replayLog` with an
	//      index, or -- worse -- accept and hash differently; naming the scenario
	//      up front says which of the two happened.
	//
	// ON SUCCESS `Fresh` ENDS UP RECORDING WHAT IT REPLAYED, because `ReplayLog`
	// appends on success. That is not incidental: it is what makes
	// seed -> replay -> serialize a fixed point, and it means a caller may chain
	// this without the copy silently becoming a bridge with state but no log.
	//
	// NOT A SAVE ROUND TRIP. Nothing here serializes, parses, or touches
	// `FStratSaveIdentity` -- `SerializeRecordedSave` owns that and
	// `T-SAVE-06.SaveRoundTripsToEqualHash` already pins it. This is the shorter
	// path that exists so a module that cannot spell the format can still ask
	// whether the commands were recorded.
	int32        RecordedCommandCount() const;
	FStratResult ReplayRecordedLogOnto(FStratBridge& Fresh) const;

private:
	// The five typed methods' shared tail: stamps `{turn, side}` off the live
	// TurnState and hands the command to `Submit`. Private because the stamp is
	// the guarantee -- a public entry point taking a half-filled command would let
	// a caller opt out of it.
	FStratResult SubmitStamped(strat::SaveCommand Command);

	// §2.9's input, GATHERED and not decided -- `MakeUiWorld`'s sibling, and it
	// exists for exactly `MakeUiWorld`'s reason. `strat::nextCommand` takes a
	// `strat::AiState`, and the only other function in the tree that produces one is
	// the headless driver's `strat::aiStateOf(const Session&)`. A Session
	// is the driver's own type and this object does not have one, so before this
	// method no engine-side caller could reach the AI at all. This mirrors
	// `aiStateOf` member for member so the two compositions cannot disagree.
	//
	// PRIVATE, WHERE `MakeUiWorld` IS PUBLIC, and the asymmetry is deliberate. A
	// `UiWorld` is a bundle of BORROWED pointers that a test legitimately wants to
	// inspect beside the snapshot built from it; an `AiState` is a by-value COPY of
	// the board with nothing to inspect that `State()` and `UnitDefs()` do not
	// already say, and exposing it would offer a caller a second, staler spelling of
	// the state this object is authoritative for. `NextAiCommand` is the surface.
	//
	// EVERY MEMBER OF `AiState` IS COMPOSED, AND COMPLETENESS IS THE POINT rather
	// than a tidiness goal. A silently-empty `builtThisTurn` is the sharpest case:
	// §2.7's "one build per factory per turn" would then read as unspent at every
	// factory, the AI would queue a second build per factory per turn, and every one
	// of them would be REFUSED by `markBuilt` downstream -- a green build, a running
	// match, and an opponent that stalls on its own refused command. Nine members,
	// against `strat::AiState`.
	//
	// COPIES UNITS FAITHFULLY, INCLUDING A UNIT WHOSE `defIndex` IS OUT OF RANGE,
	// where `MakeUiWorld` skips it. The two are answering different questions:
	// dropping a unit from a projection hides it from a display, while dropping one
	// from the AI's board hides an enemy from the opponent's reasoning and is a rule
	// change wearing a guard's clothes. `NextAiCommand` validates the whole roster
	// up front and refuses, so this method is never reached with such a unit.
	strat::AiState MakeAiState() const;

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

	// §4.10's `commandLog`, accumulated as the match is played. Held as the
	// module's own `SaveCommand` rather than as an engine mirror of it, because
	// the only consumers are `serializeSave` and `replayLog` and a mirror would be
	// a second spelling of the format that could disagree with the first.
	std::vector<strat::SaveCommand> Recorded;

	// §2.9's buildlist, as defIndexes, held in the module's own type because that is
	// what `AiState::buildlist` is. It is CONFIGURATION and not match state, which is
	// why `LoadScenarioFromFile` leaves it alone while `LoadDefinitions` clears it:
	// a reseed does not move the table these index into, and a table reload does.
	std::vector<int> Buildlist;

	// Assembles the combat stat block for one unit exactly as the driver's
	// `combatUnit` does: every stat LOOKED UP from the UnitDef at `defIndex`,
	// current hp from the unit. Nothing is stored -- a cached copy of a stat is
	// a copy that can outlive a table reload.
	strat::Unit CombatUnitOf(const strat::GameUnit& U) const;

	bool bDefinitionsLoaded = false;
	bool bSeeded = false;
};
