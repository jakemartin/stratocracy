// The editor pass for the bridge's §4.10 recording surface -- T-SAVE-06's second half.
// GDD §4.9 part 2, §4.10, §4.11 row 10 part (a).
//
//   T-SAVE-06  stateHash stability across the headless and in-engine builds. Marked †
//              in §4.11. StratBridgeParity.cpp closes the REPLAY direction of it --
//              a vendored log replayed here reaches the hash the crew build recorded.
//              This file closes the WRITE direction: a log this bridge recorded,
//              serialized through `strat::serializeSave`, parsed back through
//              `strat::parseSave` and replayed onto a fresh seed reaches the same hash.
//
// WHY BOTH DIRECTIONS ARE NEEDED AND NEITHER IMPLIES THE OTHER. The replay direction
// proves this build agrees with the crew build about what a given log means. It says
// nothing about whether this build can WRITE a log -- a bridge that recorded the wrong
// commands, or stamped them with the wrong turn, or dropped one, passes T-INT-02
// untouched, because T-INT-02 never reads `RecordedLog()`. The failure mode is a save
// file that loads without complaint into a different match, and the player's only
// symptom is that their game is not the one they left.
//
// WHERE EVERY EXPECTATION COMES FROM. The five typed `Submit*` methods are driven with
// commands taken OUT OF `Data/parity_fixture.save`, and the recorded entries are
// compared BACK AGAINST those same fixture entries -- including the `{turn, side}` tag,
// which the bridge stamps and this file therefore never computes. The fixture is a
// vendored artifact under GATE-DATA-VENDOR, emitted by the crew build; using it as the
// script has three consequences worth stating:
//   - every command is known-legal at the index it sits at, so a five-kind sequence
//     needs no hand-authored mid-match position;
//   - the turn tag this file asserts is the crew build's, not one recomputed here, so
//     "SubmitEndTurn is tagged the turn it CLOSED" is checked against an outside
//     record rather than against a rule this file restated;
//   - a re-emitted fixture whose command order moved fails LOUDLY on a kind check,
//     rather than quietly testing whatever ended up at the index.
//
// THE WINDOW IS INDICES 16..20 and it is chosen, not arbitrary: it is the shortest run
// in the shipped fixture containing all five §4.9 kinds, and it happens to STRADDLE A
// TURN BOUNDARY -- EndTurn at 19 tagged {turn 1, side 1}, Build at 20 tagged
// {turn 2, side 0}. That boundary is the whole reason StratBridge.h stamps before
// application rather than after: a tag read back afterwards would number the EndTurn
// N+1 and the log would stop replaying at the first turn change. One window exercises
// both the five kinds and the ordering.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBridge.h"

#include "Save.h"
#include "Ui.h"

namespace StratBridgeSaveRecording
{
	// Must equal StratBridgeParity.cpp's `kParityFirstSide` and StratScoreboardParity's
	// `kFirstSide`. Replay.h states no rule decides which side moves first; the fixture's
	// log was emitted against side 0, so a different value here replays a different game.
	static const int32 kFirstSide = 0;

	// A unit id that no table or scenario edit can bring into existence -- the same
	// choice and the same reason as StratBridgeParity.cpp's illegal Move.
	static const int32 kNoSuchUnitId = 999999;

	// The five-kind window. See this file's header block for why these two numbers and
	// not others. Half-open at the top in the usual way: [kWindowBegin, kWindowEnd).
	static const int32 kWindowBegin = 16;
	static const int32 kWindowEnd   = 21;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static std::string ToStd(const FString& In)
	{
		return std::string(TCHAR_TO_UTF8(*In));
	}

	static FString FromStd(const std::string& In)
	{
		return FString(UTF8_TO_TCHAR(In.c_str()));
	}

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	static FString ScenarioPath()
	{
		return FPaths::Combine(DataDir(), TEXT("ferrum_crossing.json"));
	}

	static FString DescribeCommand(const strat::SaveCommand& C)
	{
		int Col = 0;
		int Row = 0;
		strat::axialToOffset(C.hex, Col, Row);
		const FString Kind = FromStd(std::string(strat::saveCommandName(C.kind)));
		return FString::Printf(TEXT("{turn %d, side %d} %s unit/def %d%s"),
			C.turn, C.side, *Kind, C.unitId,
			C.hasHex ? *FString::Printf(TEXT(" hex offset[%d,%d]"), Col, Row) : TEXT(""));
	}

	static bool LoadDefsOnly(FStratBridge& Bridge, FString& OutError)
	{
		UDataTable* Units = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		UDataTable* Terrain = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
		if (Units == nullptr || Terrain == nullptr)
		{
			OutError = TEXT("DT_Units and/or DT_Terrain did not load from /Game/StratData");
			return false;
		}

		const FStratResult Loaded = Bridge.LoadDefinitions(Units, Terrain);
		if (!Loaded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadDefinitions refused: %s"), *Loaded.Reason);
			return false;
		}
		return true;
	}

	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		if (!LoadDefsOnly(Bridge, OutError))
		{
			return false;
		}
		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath(), kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	static bool LoadFixture(strat::Save& Out, FString& OutError)
	{
		const FString Path = FPaths::Combine(DataDir(), TEXT("parity_fixture.save"));
		FString Text;
		if (!FFileHelper::LoadFileToString(Text, *Path))
		{
			OutError = FString::Printf(TEXT("parity_fixture.save unreadable at %s"), *Path);
			return false;
		}
		const strat::SaveLoadResult Parsed =
			strat::parseSave(ToStd(Text), "parity_fixture.save", Out);
		if (!Parsed.ok)
		{
			OutError = FString::Printf(TEXT("%s: %s"),
				*FromStd(Parsed.failedId), *FromStd(Parsed.reason));
			return false;
		}
		return true;
	}

	static bool SeedAndReplayPrefix(FStratBridge& Bridge, const strat::Save& Fixture,
	                                int32 PrefixLength, FString& OutError)
	{
		if (!SeedBridge(Bridge, OutError))
		{
			return false;
		}
		if (PrefixLength < 0 || PrefixLength > static_cast<int32>(Fixture.commandLog.size()))
		{
			OutError = FString::Printf(TEXT("prefix %d is outside the fixture's %d commands"),
				PrefixLength, static_cast<int32>(Fixture.commandLog.size()));
			return false;
		}

		TArray<strat::SaveCommand> Prefix;
		Prefix.Reserve(PrefixLength);
		for (int32 i = 0; i < PrefixLength; ++i)
		{
			Prefix.Add(Fixture.commandLog[static_cast<size_t>(i)]);
		}

		const FStratResult Replayed = Bridge.ReplayLog(Prefix);
		if (!Replayed.bOk)
		{
			OutError = FString::Printf(TEXT("replaying the fixture's first %d commands refused: %s"),
				PrefixLength, *Replayed.Reason);
			return false;
		}
		return true;
	}

	/**
	 * Dispatches one parsed command through the TYPED method for its kind.
	 *
	 * THE `{turn, side}` TAG IS DELIBERATELY NOT PASSED ON. That is the entire content
	 * of the façade StratBridge.h describes: the typed methods read the tag off the live
	 * `TurnState` and there is no overload that lets a caller supply one. This helper
	 * therefore hands over ONLY the fields §4.9 names for the kind, which is what makes
	 * the comparison afterwards meaningful -- if the tag came from here, the assertion
	 * would be that this file can copy a struct.
	 */
	static FStratResult SubmitTyped(FStratBridge& Bridge, const strat::SaveCommand& C)
	{
		switch (C.kind)
		{
		case strat::SaveCommandKind::Move:
			return Bridge.SubmitMove(C.unitId, C.hex);
		case strat::SaveCommandKind::Attack:
			return Bridge.SubmitAttack(C.unitId, C.hex);
		case strat::SaveCommandKind::Build:
			// Save.h:64 spells the §2.4 row index in a field called `unitId`. The bridge's
			// parameter is named for what the rules module does with it; the format's
			// spelling is the trap, and this line is the one place the two meet.
			return Bridge.SubmitBuild(C.hex, C.unitId);
		case strat::SaveCommandKind::Capture:
			return Bridge.SubmitCapture(C.unitId);
		case strat::SaveCommandKind::EndTurn:
			return Bridge.SubmitEndTurn();
		}
		return FStratResult::Fail(TEXT("a SaveCommandKind with no typed method"));
	}
}

// ---------------------------------------------------------------------------
// T-SAVE-06 clause 1 -- what was submitted is what was recorded, tag included.
//
// FIVE KINDS AND A TURN BOUNDARY IN ONE WINDOW. Every field is compared against the
// fixture's own entry: the kind, the acting unit or def index, the hex and its presence
// flag, and the `{turn, side}` tag the bridge stamped. The tag is the interesting one --
// this file never computes a turn number, so a bridge that stamped from the wrong place
// disagrees with an outside record rather than with a rule restated here.
//
// THE ENDTURN CASE IS WHY THE ORDERING MATTERS. Entry 19 closes turn 1 for side 1 and
// entry 20 opens turn 2 for side 0. A bridge that read its tag back off the state AFTER
// application would number the EndTurn 2, the log would then be refused by
// `applyCommand`'s turn check on replay, and every save written after the first turn
// boundary would be unloadable. That is a one-line ordering mistake with a total
// failure, which is exactly the kind a gate is for.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeRecordedLogMatchesSubmissionsTest,
	"Stratocracy.StratBridge.T-SAVE-06.RecordedLogMatchesSubmissions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeRecordedLogMatchesSubmissionsTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeSaveRecording;

	strat::Save Fixture;
	FString Error;
	const bool bFixture = LoadFixture(Fixture, Error);
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"), bFixture))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the fixture is long enough to supply the five-kind window"),
		static_cast<int32>(Fixture.commandLog.size()) >= kWindowEnd))
	{
		return false;
	}

	// THE WINDOW MUST STILL BE THE FIVE-KIND ONE. A re-emitted fixture is allowed to
	// move; what is not allowed is this clause quietly submitting four Moves and calling
	// it coverage.
	{
		bool bSeen[5] = { false, false, false, false, false };
		for (int32 i = kWindowBegin; i < kWindowEnd; ++i)
		{
			bSeen[static_cast<int32>(Fixture.commandLog[static_cast<size_t>(i)].kind)] = true;
		}
		bool bAllFive = true;
		for (int32 k = 0; k < 5; ++k)
		{
			bAllFive = bAllFive && bSeen[k];
		}
		if (!TestTrue(
				*FString::Printf(TEXT("fixture entries [%d,%d) still cover all five SaveCommandKinds"),
					kWindowBegin, kWindowEnd),
				bAllFive))
		{
			AddError(TEXT("the fixture was re-emitted and its command order moved; re-pick the window "
			              "rather than submitting whatever now sits there"));
			return false;
		}
	}

	FStratBridge Bridge;
	const bool bReplayed = SeedAndReplayPrefix(Bridge, Fixture, kWindowBegin, Error);
	if (!TestTrue(TEXT("the bridge reaches the position the window's first command was made from"),
			bReplayed))
	{
		AddError(Error);
		return false;
	}

	const int32 LengthBefore = static_cast<int32>(Bridge.RecordedLog().size());
	TestEqual(TEXT("the replayed prefix is itself recorded, command for command"),
		LengthBefore, kWindowBegin);

	// ---- submit the window through the TYPED methods -------------------------
	for (int32 i = kWindowBegin; i < kWindowEnd; ++i)
	{
		const strat::SaveCommand& Wanted = Fixture.commandLog[static_cast<size_t>(i)];
		const FStratResult Submitted = SubmitTyped(Bridge, Wanted);
		if (!TestTrue(
				*FString::Printf(TEXT("fixture entry %d submits through its typed method: %s"),
					i, *DescribeCommand(Wanted)),
				Submitted.bOk))
		{
			AddError(FString::Printf(TEXT("entry %d refused: %s"), i, *Submitted.Reason));
			return false;
		}
	}

	const std::vector<strat::SaveCommand>& Recorded = Bridge.RecordedLog();
	if (!TestEqual(TEXT("T-SAVE-06: five submissions produced five further entries"),
			static_cast<int32>(Recorded.size()), kWindowEnd))
	{
		return false;
	}

	// ---- and every field is the one the fixture recorded ---------------------
	for (int32 i = kWindowBegin; i < kWindowEnd; ++i)
	{
		const strat::SaveCommand& Wanted = Fixture.commandLog[static_cast<size_t>(i)];
		const strat::SaveCommand& Got    = Recorded[static_cast<size_t>(i)];
		const FString KindName = FromStd(std::string(strat::saveCommandName(Wanted.kind)));
		const FString At = FString::Printf(TEXT("entry %d (%s)"), i, *KindName);

		TestEqual(*FString::Printf(TEXT("%s: kind"), *At),
			static_cast<int32>(Got.kind), static_cast<int32>(Wanted.kind));

		// THE STAMP. Neither number is computed here; both come off the fixture.
		TestEqual(*FString::Printf(TEXT("%s: turn tag is the one the crew build recorded"), *At),
			Got.turn, Wanted.turn);
		TestEqual(*FString::Printf(TEXT("%s: side tag is the one the crew build recorded"), *At),
			Got.side, Wanted.side);

		TestEqual(*FString::Printf(TEXT("%s: hasUnit"), *At), Got.hasUnit, Wanted.hasUnit);
		TestEqual(*FString::Printf(TEXT("%s: hasHex"), *At),  Got.hasHex,  Wanted.hasHex);
		if (Wanted.hasUnit)
		{
			TestEqual(*FString::Printf(TEXT("%s: unitId / defIndex"), *At),
				Got.unitId, Wanted.unitId);
		}
		if (Wanted.hasHex)
		{
			TestTrue(*FString::Printf(TEXT("%s: hex"), *At),
				strat::hexEqual(Got.hex, Wanted.hex));
		}
	}

	// ---- the turn-boundary property, called out by name ----------------------
	// Stated separately from the loop because it is the clause's own sentence rather
	// than one instance of a field comparison, and because a reader of a failing log
	// should see it named.
	{
		const strat::SaveCommand* EndTurn = nullptr;
		int32 EndTurnIndex = INDEX_NONE;
		for (int32 i = kWindowBegin; i < kWindowEnd; ++i)
		{
			if (Recorded[static_cast<size_t>(i)].kind == strat::SaveCommandKind::EndTurn)
			{
				EndTurn = &Recorded[static_cast<size_t>(i)];
				EndTurnIndex = i;
				break;
			}
		}
		if (TestNotNull(TEXT("the window contains an EndTurn"), EndTurn) &&
			EndTurnIndex + 1 < kWindowEnd)
		{
			const strat::SaveCommand& Next = Recorded[static_cast<size_t>(EndTurnIndex + 1)];
			TestEqual(
				TEXT("T-SAVE-06: SubmitEndTurn is tagged the turn it CLOSED, not the one it opened"),
				EndTurn->turn,
				Fixture.commandLog[static_cast<size_t>(EndTurnIndex)].turn);
			TestEqual(TEXT("T-SAVE-06: and the next command carries the turn it opened"),
				Next.turn, EndTurn->turn + 1);
			AddInfo(FString::Printf(
				TEXT("turn boundary: EndTurn tagged {turn %d, side %d}, next command {turn %d, side %d}"),
				EndTurn->turn, EndTurn->side, Next.turn, Next.side));
		}
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-06 clause 2 -- a rejected command is not recorded.
//
// §4.9: an invalid command changes nothing. The log is state, so "nothing" includes it,
// and StratBridge.h says the append happens AFTER the module accepted. A log that
// recorded the attempt would serialize into a save whose `commandLog` refuses to replay
// -- the file would be rejected at load with a reason pointing at a command the player
// never successfully gave.
//
// THE STATE HASH IS CHECKED ALONGSIDE, because "the log did not grow" and "the state did
// not move" are two different guarantees and a clause that checked one would leave the
// other to T-INT-03's good intentions.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeRejectedCommandNotRecordedTest,
	"Stratocracy.StratBridge.T-SAVE-06.RejectedCommandIsNotRecorded",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeRejectedCommandNotRecordedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeSaveRecording;

	FStratBridge Bridge;
	FString Error;
	const bool bSeeded = SeedBridge(Bridge, Error);
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), bSeeded))
	{
		AddError(Error);
		return false;
	}

	TestEqual(TEXT("a freshly seeded match has recorded nothing"),
		static_cast<int32>(Bridge.RecordedLog().size()), 0);

	// A legal command first, so the clause distinguishes "did not append" from "cannot
	// append". Its hex is the unit's own current one, which `reachable` always contains
	// at cost 0 (Move.h's null move) -- so this is legal on any board without this file
	// choosing a destination it would have to re-pick when the scenario moved.
	const strat::UiWorld World = Bridge.MakeUiWorld();
	if (!TestTrue(TEXT("the seeded opening puts units on the board"), !World.units.empty()))
	{
		return false;
	}
	const strat::UiUnit& Mover = World.units.front();

	const FStratResult Legal = Bridge.SubmitMove(Mover.id, Mover.hex);
	if (!TestTrue(TEXT("a null move is accepted, so the log CAN grow"), Legal.bOk))
	{
		AddError(Legal.Reason);
		return false;
	}
	const int32 LengthAfterLegal = static_cast<int32>(Bridge.RecordedLog().size());
	TestEqual(TEXT("the accepted command was recorded"), LengthAfterLegal, 1);

	const FString HashBefore = Bridge.StateHash();

	// ---- the illegal one ----------------------------------------------------
	const FStratResult Illegal = Bridge.SubmitMove(kNoSuchUnitId, Mover.hex);
	TestFalse(TEXT("T-SAVE-06: a move by a unit that does not exist is refused"), Illegal.bOk);
	TestTrue(TEXT("T-SAVE-06: the refusal carries a reason"), !Illegal.Reason.IsEmpty());
	AddInfo(FString::Printf(TEXT("refusal reason: %s"), *Illegal.Reason));

	TestEqual(TEXT("T-SAVE-06: the refused command left the recorded log the length it was"),
		static_cast<int32>(Bridge.RecordedLog().size()), LengthAfterLegal);
	TestEqual(TEXT("T-SAVE-06: and left the state hash unchanged"),
		Bridge.StateHash(), HashBefore);

	// The untyped entry point takes the same line -- `Submit` is where the append lives,
	// so a refusal there must not append either.
	strat::SaveCommand RawIllegal;
	RawIllegal.turn    = Bridge.State().turn.turnNumber;
	RawIllegal.side    = Bridge.State().turn.activeSide;
	RawIllegal.kind    = strat::SaveCommandKind::Attack;
	RawIllegal.unitId  = kNoSuchUnitId;
	RawIllegal.hasUnit = true;
	RawIllegal.hex     = Mover.hex;
	RawIllegal.hasHex  = true;

	const FStratResult RawRefused = Bridge.Submit(RawIllegal);
	TestFalse(TEXT("T-SAVE-06: the untyped Submit refuses it too"), RawRefused.bOk);
	TestEqual(TEXT("T-SAVE-06: and records nothing"),
		static_cast<int32>(Bridge.RecordedLog().size()), LengthAfterLegal);

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-06 clause 3 -- both reseed paths clear the log; a FAILED load does not.
//
// StratBridge.h's reason: a log is only meaningful against the seed it was recorded
// from. Carried across a reseed it would serialize into a save whose `commandLog`
// replays to a state its own `stateHash` disagrees with -- and T-SAVE-06 would then be
// catching this bridge rather than the format, which is the worst kind of green.
//
// THE ASYMMETRY IS THE INTERESTING HALF. `LoadDefinitions` clears ALWAYS, because the
// defIndex values in every Build entry point into a table that just moved.
// `LoadScenarioFromFile` clears ON SUCCESS ONLY, because a scenario that failed to load
// left the previous match intact and dropping its log would destroy a save the player
// could still have written. A single `Recorded.clear()` at the top of that function
// would satisfy "reseed clears" and silently take the failed path with it, so the third
// sub-case below is the one that has teeth.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeReseedClearsRecordedLogTest,
	"Stratocracy.StratBridge.T-SAVE-06.ReseedClearsRecordedLog",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeReseedClearsRecordedLogTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeSaveRecording;

	FStratBridge Bridge;
	FString Error;
	const bool bSeeded = SeedBridge(Bridge, Error);
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), bSeeded))
	{
		AddError(Error);
		return false;
	}

	// ---- path 1: LoadDefinitions clears, always -----------------------------
	if (!TestTrue(TEXT("an EndTurn is accepted, so there is a log to clear"),
			Bridge.SubmitEndTurn().bOk))
	{
		return false;
	}
	TestEqual(TEXT("the log holds the submitted command"),
		static_cast<int32>(Bridge.RecordedLog().size()), 1);

	const bool bReloaded = LoadDefsOnly(Bridge, Error);
	if (!TestTrue(TEXT("the tables map in again"), bReloaded))
	{
		AddError(Error);
		return false;
	}
	TestEqual(TEXT("T-SAVE-06: LoadDefinitions clears the recorded log"),
		static_cast<int32>(Bridge.RecordedLog().size()), 0);
	TestFalse(TEXT("and drops the seed with it, since every defIndex just moved"),
		Bridge.IsSeeded());

	// ---- path 2: a SUCCESSFUL LoadScenarioFromFile clears --------------------
	{
		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath(), kFirstSide);
		if (!TestTrue(TEXT("the scenario seeds again"), Seeded.bOk))
		{
			AddError(Seeded.Reason);
			return false;
		}
	}
	if (!TestTrue(TEXT("an EndTurn is accepted again"), Bridge.SubmitEndTurn().bOk))
	{
		return false;
	}
	TestEqual(TEXT("the log holds it"), static_cast<int32>(Bridge.RecordedLog().size()), 1);

	{
		const FStratResult Reseeded = Bridge.LoadScenarioFromFile(ScenarioPath(), kFirstSide);
		if (!TestTrue(TEXT("the scenario re-seeds"), Reseeded.bOk))
		{
			AddError(Reseeded.Reason);
			return false;
		}
	}
	TestEqual(TEXT("T-SAVE-06: a successful LoadScenarioFromFile clears the recorded log"),
		static_cast<int32>(Bridge.RecordedLog().size()), 0);

	// ---- path 3: a FAILED LoadScenarioFromFile does NOT clear ----------------
	if (!TestTrue(TEXT("an EndTurn is accepted once more"), Bridge.SubmitEndTurn().bOk))
	{
		return false;
	}
	const int32 LengthBefore = static_cast<int32>(Bridge.RecordedLog().size());
	const FString HashBefore = Bridge.StateHash();
	TestEqual(TEXT("there is a log for the failed load to leave alone"), LengthBefore, 1);

	const FString Missing = FPaths::Combine(DataDir(), TEXT("no_such_scenario_exists.json"));
	const FStratResult Failed = Bridge.LoadScenarioFromFile(Missing, kFirstSide);
	TestFalse(TEXT("T-SAVE-06: loading a scenario that is not there is refused"), Failed.bOk);
	TestTrue(TEXT("T-SAVE-06: with a reason"), !Failed.Reason.IsEmpty());
	AddInfo(FString::Printf(TEXT("failed-load reason: %s"), *Failed.Reason));

	TestEqual(TEXT("T-SAVE-06: a FAILED load leaves the previous match's log intact"),
		static_cast<int32>(Bridge.RecordedLog().size()), LengthBefore);
	TestTrue(TEXT("T-SAVE-06: and leaves the previous match seeded"), Bridge.IsSeeded());
	TestEqual(TEXT("T-SAVE-06: and leaves its state where it was"),
		Bridge.StateHash(), HashBefore);

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-06 clause 4 -- serialize, parse, replay, and land on the same hash.
//
// THE PROPERTY THAT MATTERS MOST, and it is a round trip rather than a string
// comparison on purpose. Comparing the emitted text against a stored blob would pin the
// BYTES, which is a fragile thing to gate and not what a player needs; what a player
// needs is that the file they saved reconstructs the match they were in. So this clause
// closes the loop end to end: this bridge's recorded log ->
// `strat::serializeSave` -> `strat::parseSave` -> `strat::replayLog` onto a FRESHLY
// SEEDED bridge -> `strat::canonicalStateHash`, compared against the hash of the bridge
// the log came off.
//
// FOUR SEPARATE FAILURES ARE COVERED BY ONE COMPARISON, which is why it is worth its
// name: a log recorded in the wrong order, a command stamped with the wrong turn, a
// field lost by the writer, and a field misread by the parser all land somewhere other
// than the same hash.
//
// THE SAVE'S OWN `stateHash` IS CHECKED TOO. `SerializeRecordedSave` writes it from
// `strat::canonicalStateHash` over the state at serialization time, so the file makes a
// CLAIM about where its log ends up. That claim is compared against where the log
// actually ended up, which is the check a loader would want to make and cannot make for
// itself at part (a) -- Save.h is explicit that `stateHash` is opaque there.
//
// THE TWO HEADER FIELDS THIS FILE SUPPLIES COME OFF THE FIXTURE. `rulesCommit` and
// `dataHash` are `FStratSaveIdentity`'s whole reason for existing -- the bridge cannot
// know them and must not go looking -- so the values are taken from the vendored save
// rather than invented, and the emitted file is a well-formed sibling of it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeSerializedSaveRoundTripsTest,
	"Stratocracy.StratBridge.T-SAVE-06.SerializedSaveRoundTrips",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeSerializedSaveRoundTripsTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeSaveRecording;

	strat::Save Fixture;
	FString Error;
	const bool bFixture = LoadFixture(Fixture, Error);
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"), bFixture))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the fixture is long enough to supply the five-kind window"),
		static_cast<int32>(Fixture.commandLog.size()) >= kWindowEnd))
	{
		return false;
	}

	// The source bridge: seeded, then driven to a mid-match position that has crossed a
	// turn boundary and taken all five kinds of command -- the same window clause 1
	// uses, so what is being round-tripped is a log with every kind in it rather than a
	// run of Moves.
	FStratBridge Source;
	const bool bReplayed = SeedAndReplayPrefix(Source, Fixture, kWindowBegin, Error);
	if (!TestTrue(TEXT("the source bridge reaches the window's start"), bReplayed))
	{
		AddError(Error);
		return false;
	}
	for (int32 i = kWindowBegin; i < kWindowEnd; ++i)
	{
		const FStratResult Submitted =
			SubmitTyped(Source, Fixture.commandLog[static_cast<size_t>(i)]);
		if (!TestTrue(*FString::Printf(TEXT("window entry %d submits"), i), Submitted.bOk))
		{
			AddError(Submitted.Reason);
			return false;
		}
	}

	const FString SourceHash = Source.StateHash();
	const int32 RecordedLength = static_cast<int32>(Source.RecordedLog().size());
	TestEqual(TEXT("the source's log is the prefix plus the window"), RecordedLength, kWindowEnd);
	TestTrue(TEXT("the source has a hash to compare against"), !SourceHash.IsEmpty());

	// ---- serialize ----------------------------------------------------------
	FStratSaveIdentity Identity;
	Identity.RulesCommit = FromStd(Fixture.rulesCommit);
	Identity.DataHash    = FromStd(Fixture.dataHash);

	FString Text;
	const FStratResult Serialized = Source.SerializeRecordedSave(Identity, Text);
	if (!TestTrue(TEXT("T-SAVE-06: a seeded bridge with a log serializes"), Serialized.bOk))
	{
		AddError(Serialized.Reason);
		return false;
	}
	TestTrue(TEXT("T-SAVE-06: the serialized save is not empty"), !Text.IsEmpty());

	// ---- parse back through the module's own parser --------------------------
	strat::Save Reparsed;
	const strat::SaveLoadResult Parsed =
		strat::parseSave(ToStd(Text), "SerializeRecordedSave", Reparsed);
	if (!TestTrue(TEXT("T-SAVE-06: what the bridge wrote, strat::parseSave reads"), Parsed.ok))
	{
		AddError(FString::Printf(TEXT("%s: %s"),
			*FromStd(Parsed.failedId), *FromStd(Parsed.reason)));
		return false;
	}

	TestEqual(TEXT("T-SAVE-06: the emitted save carries the whole recorded log"),
		static_cast<int32>(Reparsed.commandLog.size()), RecordedLength);
	TestEqual(TEXT("T-SAVE-06: it names the scenario the bridge actually loaded"),
		FromStd(Reparsed.scenarioId), FromStd(Fixture.scenarioId));
	TestEqual(TEXT("T-SAVE-06: it carries the scenario hash the fixture does, since it is the same file"),
		FromStd(Reparsed.scenarioHash), FromStd(Fixture.scenarioHash));
	TestEqual(TEXT("T-SAVE-06: formatVersion is this build's"),
		Reparsed.formatVersion, strat::kFormatVersion);
	TestEqual(TEXT("T-SAVE-06: seed is 0, which Save.h says it MUST be"), Reparsed.seed, 0);
	TestEqual(TEXT("T-SAVE-06: the caller-supplied rulesCommit is carried unchanged"),
		FromStd(Reparsed.rulesCommit), Identity.RulesCommit);
	TestEqual(TEXT("T-SAVE-06: the caller-supplied dataHash is carried unchanged"),
		FromStd(Reparsed.dataHash), Identity.DataHash);

	// The file's own claim about where its log ends up.
	TestEqual(TEXT("T-SAVE-06: the save's stateHash is the state the log was written from"),
		FromStd(Reparsed.stateHash), SourceHash);

	// ---- replay onto a freshly seeded bridge ---------------------------------
	FStratBridge Restored;
	const bool bRestoredSeed = SeedBridge(Restored, Error);
	if (!TestTrue(TEXT("a second bridge seeds from the same scenario"), bRestoredSeed))
	{
		AddError(Error);
		return false;
	}
	TestNotEqual(TEXT("the fresh seed is NOT already at the source's state, so the replay does the work"),
		Restored.StateHash(), SourceHash);

	TArray<strat::SaveCommand> Reloaded;
	Reloaded.Reserve(static_cast<int32>(Reparsed.commandLog.size()));
	for (const strat::SaveCommand& C : Reparsed.commandLog)
	{
		Reloaded.Add(C);
	}

	const FStratResult ReplayedBack = Restored.ReplayLog(Reloaded);
	if (!TestTrue(TEXT("T-SAVE-06: the parsed log replays clean onto a fresh seed"), ReplayedBack.bOk))
	{
		AddError(FString::Printf(TEXT("%s: %s"), *ReplayedBack.Id, *ReplayedBack.Reason));
		return false;
	}

	// THE COMPARISON. Both sides are `strat::canonicalStateHash`; nothing in this file
	// authored either.
	TestEqual(
		TEXT("T-SAVE-06: serialize -> parseSave -> replay onto a fresh seed reaches the same canonical state hash"),
		Restored.StateHash(), SourceHash);

	// SAVE -> LOAD -> SAVE IS A FIXED POINT, which follows from the bridge recording a
	// replayed log and is worth asserting rather than inferring: it is what makes a
	// player's second save of the same session the same file as their first.
	FString SecondText;
	const FStratResult Reserialized = Restored.SerializeRecordedSave(Identity, SecondText);
	if (TestTrue(TEXT("the restored bridge serializes too"), Reserialized.bOk))
	{
		TestEqual(TEXT("T-SAVE-06: save -> load -> save is a fixed point, byte for byte"),
			SecondText, Text);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-06 clause 5 -- "nothing loaded" and "a match in which nothing happened" are
// different, and the API says which.
//
// StratBridge.h holds `SerializeRecordedSave` to the line `MakeUiSnapshot` holds: refuse
// rather than emit a save of an empty match. The distinction has teeth here because an
// empty command log is a PERFECTLY GOOD save -- it is what a player saving on turn 1
// before touching anything writes, and it replays to the seeded opening. A bridge that
// emitted the same empty-logged file for "startup has not finished" would produce a save
// that loads into a match nobody started.
//
// SO THE POSITIVE CASE IS PART OF THE CLAUSE. The two refusals are only meaningful
// beside a success that looks superficially identical, and the third sub-case below is
// that success: seeded, log empty, and the file is real enough to parse.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeSerializeRefusesUnseededTest,
	"Stratocracy.StratBridge.T-SAVE-06.SerializeRefusesUnseeded",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeSerializeRefusesUnseededTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeSaveRecording;

	FStratSaveIdentity Identity;
	Identity.RulesCommit = TEXT("cb8e12b");
	Identity.DataHash    = TEXT("test-supplied");

	// Pre-filled so "cleared" is distinguishable from "never written" -- the same reason
	// the Reachable clauses pre-fill their vector.
	FString Text = TEXT("<a previous save's text>");

	// ---- nothing loaded at all ----------------------------------------------
	FStratBridge Empty;
	const FStratResult NoDefs = Empty.SerializeRecordedSave(Identity, Text);
	TestFalse(TEXT("T-SAVE-06: an unloaded bridge refuses to serialize"), NoDefs.bOk);
	TestTrue(TEXT("T-SAVE-06: with a reason"), !NoDefs.Reason.IsEmpty());
	TestTrue(TEXT("T-SAVE-06: and leaves no half-written text behind"), Text.IsEmpty());
	TestEqual(TEXT("T-SAVE-06: the unloaded reason names the definitions"),
		NoDefs.Reason, FString(TEXT("definitions are not loaded")));

	// ---- tables loaded, no scenario -----------------------------------------
	FStratBridge DefsOnly;
	FString Error;
	const bool bLoaded = LoadDefsOnly(DefsOnly, Error);
	if (!TestTrue(TEXT("the shipped tables map in"), bLoaded))
	{
		AddError(Error);
		return false;
	}

	Text = TEXT("<a previous save's text>");
	const FStratResult NoScenario = DefsOnly.SerializeRecordedSave(Identity, Text);
	TestFalse(TEXT("T-SAVE-06: a bridge with tables but no scenario still refuses"), NoScenario.bOk);
	TestTrue(TEXT("T-SAVE-06: with a reason"), !NoScenario.Reason.IsEmpty());
	TestTrue(TEXT("T-SAVE-06: and no text"), Text.IsEmpty());
	TestEqual(TEXT("T-SAVE-06: the unseeded reason names the scenario"),
		NoScenario.Reason, FString(TEXT("no scenario is loaded")));

	TestNotEqual(TEXT("T-SAVE-06: the two refusals are distinct sentences"),
		NoDefs.Reason, NoScenario.Reason);

	// The same two reasons the routed queries give, in the same order. One vocabulary
	// for one missing step, across the whole exported surface.
	std::vector<strat::ReachEntry> Reach;
	TestEqual(TEXT("Reachable gives the same unloaded reason"),
		FStratBridge().Reachable(0, Reach).Reason, NoDefs.Reason);
	TestEqual(TEXT("Reachable gives the same unseeded reason"),
		DefsOnly.Reachable(0, Reach).Reason, NoScenario.Reason);

	// ---- seeded, and nothing has happened: a REAL save ----------------------
	FStratBridge Seeded;
	const bool bSeeded = SeedBridge(Seeded, Error);
	if (!TestTrue(TEXT("the bridge seeds from the shipped scenario"), bSeeded))
	{
		AddError(Error);
		return false;
	}
	TestEqual(TEXT("this match has taken no command"),
		static_cast<int32>(Seeded.RecordedLog().size()), 0);

	FString OpeningText;
	const FStratResult Opening = Seeded.SerializeRecordedSave(Identity, OpeningText);
	TestTrue(TEXT("T-SAVE-06: a match in which nothing has happened DOES serialize"), Opening.bOk);
	TestTrue(TEXT("T-SAVE-06: and produces a file"), !OpeningText.IsEmpty());

	strat::Save Reparsed;
	const strat::SaveLoadResult Parsed =
		strat::parseSave(ToStd(OpeningText), "SerializeRecordedSave", Reparsed);
	TestTrue(TEXT("T-SAVE-06: the empty-log save is well formed"), Parsed.ok);
	if (Parsed.ok)
	{
		TestEqual(TEXT("T-SAVE-06: its command log is empty, which is an ANSWER and not a fault"),
			static_cast<int32>(Reparsed.commandLog.size()), 0);
		TestEqual(TEXT("T-SAVE-06: and its stateHash is the seeded opening's"),
			FromStd(Reparsed.stateHash), Seeded.StateHash());
	}
	else
	{
		AddError(FString::Printf(TEXT("%s: %s"),
			*FromStd(Parsed.failedId), *FromStd(Parsed.reason)));
	}

	return true;
}
