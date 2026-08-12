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
#include "Data.h"
#include "Replay.h"
#include "Save.h"
#include "Scenario.h"
#include "Ui.h"

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
	FStratResult Submit(const strat::SaveCommand& Command);

	// ---- Typed commands (§4.9's five, and no others) ----------------------
	// One method per `strat::SaveCommandKind`, checked against Save.h:54: Move,
	// Attack, Build, Capture, EndTurn. There is no Wait -- §2.11.1's "wait" is the
	// selection machine spending a unit's turn and it reaches no rules module, so
	// a sixth method here would be inventing a command the format cannot carry.
	//
	// THEY EXIST TO STAMP `{turn, side}`, which is the whole content of the
	// façade. §4.10 tags every log entry with them and `applyCommand` REFUSES an
	// entry whose tag disagrees with the live turn (Replay.good.cpp:379-386), so
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
	// `SubmitBuild` TAKES A defIndex AND SAYS SO. Save.h:64 carries it in a field
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
	// InProgress-is-null mapping stays Ui.good.cpp:228's and the spelling stays
	// `strat::tierName`'s. The bytes are `strat::serializeSave`'s.
	//
	// REFUSES ON AN UNSEEDED BRIDGE rather than emitting a save of an empty match,
	// on the same line MakeUiSnapshot holds: a caller must not be able to mistake
	// "nothing is loaded" for "a match in which nothing has happened".
	FStratResult SerializeRecordedSave(const FStratSaveIdentity& Identity, FString& OutText) const;

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

	// ---- The engine-typed façade -----------------------------------------
	// The six methods below say exactly what the five above say, in `int32` and
	// `FIntPoint`, and they exist for one reason: `StratPlay` NAMES NO `strat::`
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
	// THEY ADD NO POLICY. Each one forwards to the typed method beside it and
	// converts the container; every refusal is the typed method's, in its words.
	// The one exception is documented on `AttackTargetHexes`, and it is an
	// enumeration rather than a rule.
	//
	// OUT OF LINE, DELIBERATELY, all six. An inline body here would force the
	// caller's translation unit to instantiate over `strat::Hex` and defeat the
	// whole point; it would also hide them from the link line, which is how the
	// 4 x LNK2019 measured on `StratPlay` stayed invisible until a real caller
	// existed (`StratPlay.Build.cs`).

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

	// Assembles the combat stat block for one unit exactly as the driver's
	// `combatUnit` does: every stat LOOKED UP from the UnitDef at `defIndex`,
	// current hp from the unit. Nothing is stored -- a cached copy of a stat is
	// a copy that can outlive a table reload.
	strat::Unit CombatUnitOf(const strat::GameUnit& U) const;

	bool bDefinitionsLoaded = false;
	bool bSeeded = false;
};
