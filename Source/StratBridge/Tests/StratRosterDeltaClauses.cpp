// GDD §4.9 / §2.11.2 / T-INT-05 -- `FStratBridge::RosterDeltaOfLastCommand`.
//
// WHAT GAP THIS CLOSES. §2.11.2's existence hold needs to know which unit ids APPEARED and
// which DEPARTED across one accepted command, and `StratRosterObservation` is bracketed around
// the single `strat::applyCommand` call in `FStratBridge::Submit` to answer it. That
// observation is filled on the accepted path and CLEARED on the refused one, and the clearing
// is the half with no natural witness: a caller that ignores the `FStratResult` and reads the
// delta anyway would, without it, attribute the PREVIOUS command's appearances and departures
// to the command the rules module had just rejected. §4.9 says a refused command changes
// nothing, so its delta is empty by definition -- and "empty by definition" is exactly the kind
// of claim that survives its own implementation being deleted unless something asserts it.
//
// WHERE THE EXPECTATIONS COME FROM. There is not one unit id, command index or list length
// written in this file.
//   - THE ACCEPTED COMMAND THAT MOVES A ROSTER IS FOUND BY ASKING, NOT BY BEING NAMED. The
//     fixture's own §4.10 log is replayed one command at a time through the typed `Submit`
//     methods, and after each accepted one the bridge is asked for its delta. The walk stops at
//     the FIRST command the bridge itself reports as having changed the roster. Which command
//     that is, and which ids it names, are the module's answers; a hard-coded index would be
//     this file deciding what `applyCommand` does, and it would go stale on the next re-vendor.
//   - THE REFUSAL IS THE RULES MODULE'S OWN, produced by an `EndTurn` stamped with a turn
//     number the module says is not live -- `StratRepairReceiptParity.cpp`'s device, reused
//     because it refuses inside `applyCommand` on a tag check rather than being screened out by
//     the bridge before the observation is reached. A refusal the bridge caught early would not
//     exercise the `!R.ok` path this clause is about.
//   - THE STANDING DELTA IS COMPARED AGAINST ITSELF ACROSS THE REFUSAL, not against a list this
//     file composed: what is read before the refusal is remembered and required to be
//     non-empty, and what is read after it is required to be empty. Nothing in between is
//     computed here.
//
// THE CONTROL IS THE FIRST HALF AND IT IS NOT DECORATION. The assertion after the refusal is an
// ABSENCE, and an absence read on a bridge whose delta was already empty is free -- it would
// pass forever with both `clear()` calls deleted. So the clause refuses to proceed until it has
// a bridge standing on a genuinely NON-EMPTY delta, and reports what it found.
//
// WHAT THIS DOES NOT COVER, stated rather than left to be found.
//   - WHETHER THE DELTA IS RIGHT ON THE ACCEPTED PATH -- that the ids it names are the ids
//     `applyCommand` actually spawned and erased. This clause reads the accepted delta only to
//     establish that one exists. §2.11.2's four `T-TURN-09` clauses in
//     `Source/StratPlay/Tests/StratTourExistenceHoldClauses.cpp` are what pin the identities,
//     end to end, against a roster difference derived from the view model.
//   - `ReplayLog`, WHICH REACHES `strat::replayLog` DIRECTLY AND NEVER PASSES THROUGH `Submit`.
//     A log loaded from disk in a fresh process therefore leaves the delta untouched. That is
//     the SAME gap `Submit`'s own block already records for the `STRAT-COMBAT` family and for
//     `RepairsAtTurnOpen`, with the same fix available; it is a real gap, not a deliberate
//     exclusion, and it is named here so a reader does not infer coverage from this file's
//     existence.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include <string>

#include "StratBridge.h"

// The vendored headers, by short name through StratRules' PublicIncludePaths. Named explicitly
// because this file reads `strat::SaveCommand` and `strat::Save` directly.
#include "Save.h"

namespace StratRosterDeltaClauses
{
	/** Must equal the other bridge parity files' own `kFirstSide`. The fixture's log was
	 *  emitted against side 0; a different value here replays a different game. */
	static const int32 kFirstSide = 0;

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

	/** Brings a bridge to the seeded opening. Duplicated from the other bridge parity files
	 *  rather than shared, which is this project's standing posture for this helper -- sharing
	 *  would put a header inside a `Tests/` directory. */
	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		UDataTable* const Units   = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		UDataTable* const Terrain = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
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

		const FStratResult Seeded = Bridge.LoadScenarioFromFile(
			FPaths::Combine(DataDir(), TEXT("ferrum_crossing.json")), kFirstSide);
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
		FString       Text;
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

	/** The §4.10 kinds, through the typed façade. `SaveCommand::unitId` spells the §2.4 row
	 *  index in a field named for a unit on the Build arm; `StratBridge.h` records that the
	 *  format's spelling is the trap. */
	static FStratResult SubmitTyped(FStratBridge& Bridge, const strat::SaveCommand& C)
	{
		switch (C.kind)
		{
		case strat::SaveCommandKind::Move:    return Bridge.SubmitMove(C.unitId, C.hex);
		case strat::SaveCommandKind::Attack:  return Bridge.SubmitAttack(C.unitId, C.hex);
		case strat::SaveCommandKind::Build:   return Bridge.SubmitBuild(C.hex, C.unitId);
		case strat::SaveCommandKind::Capture: return Bridge.SubmitCapture(C.unitId);
		case strat::SaveCommandKind::EndTurn: return Bridge.SubmitEndTurn();
		}
		return FStratResult::Fail(TEXT("a SaveCommandKind with no typed method"));
	}

	static FString Describe(const TArray<int32>& Ids)
	{
		if (Ids.Num() == 0)
		{
			return TEXT("(empty)");
		}
		FString Out;
		for (const int32 Id : Ids)
		{
			Out += Out.IsEmpty() ? FString::Printf(TEXT("%d"), Id)
			                     : FString::Printf(TEXT(", %d"), Id);
		}
		return Out;
	}
}

// ---------------------------------------------------------------------------
// T-INT-05 -- A REFUSED COMMAND LEAVES NO ROSTER DELTA.
//
// §4.9: a rejected command changes nothing. `FStratBridge::Submit` therefore empties both
// halves of the roster observation on its `!R.ok` return, and the two `clear()` calls that do
// it are the entire subject of this clause. Their absence is invisible to every other test in
// this suite and to every caller that checks the result before reading -- which is why it needs
// a clause of its own rather than being covered incidentally.
//
// THE SEQUENCE, AND EVERY STEP OF IT IS THE MODULE'S ANSWER.
//   1. Seed, then replay the fixture's own §4.10 log one command at a time, asking the bridge
//      after each accepted one whether that command moved the roster. Stop at the first that
//      did. WHICH COMMAND THAT IS IS NOT WRITTEN HERE.
//   2. CONTROL: require that delta to be non-empty and report it. Everything after this is an
//      absence, and an absence over an already-empty delta is free.
//   3. Submit an `EndTurn` stamped with a turn the module says is not live, and require the
//      module to refuse it. A refusal that did not happen would make step 4 an assertion about
//      an accepted command.
//   4. Read the delta again. Both halves must be empty -- the previous command's appearances
//      and departures may not be readable as the refused command's.
//   5. And the read must still SUCCEED, which is what distinguishes "cleared" from "the bridge
//      stopped answering". `RosterDeltaOfLastCommand` refuses only on an unseeded bridge, and
//      empties its out-params before that refusal, so a clause that only checked emptiness
//      would pass against a bridge that had lost its seed.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRefusedCommandLeavesNoRosterDeltaTest,
	"Stratocracy.StratBridge.T-INT-05.ARefusedCommandLeavesNoRosterDelta",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRefusedCommandLeavesNoRosterDeltaTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratRosterDeltaClauses;

	strat::Save Fixture;
	FString     Error;
	if (!TestTrue(TEXT("the parity fixture parses"), LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratBridge Bridge;
	if (!TestTrue(TEXT("the bridge seeds"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- 1. Walk the fixture until the BRIDGE says a command moved the roster ----
	const int32   Count = static_cast<int32>(Fixture.commandLog.size());
	int32         FoundAt = INDEX_NONE;
	TArray<int32> StandingAppeared;
	TArray<int32> StandingDeparted;

	for (int32 Index = 0; Index < Count; ++Index)
	{
		const strat::SaveCommand& Command = Fixture.commandLog[static_cast<size_t>(Index)];

		// A REFUSED FIXTURE COMMAND IS FATAL RATHER THAN SKIPPED. The log is the module's own
		// record of a game it played; a refusal here means the replay has diverged from it, and
		// walking past one would leave this clause standing at a position nobody can describe.
		const FStratResult Submitted = SubmitTyped(Bridge, Command);
		if (!TestTrue(*FString::Printf(TEXT("fixture command %d applies: %s"),
				Index, *Submitted.Reason), Submitted.bOk))
		{
			return false;
		}

		TArray<int32> Appeared;
		TArray<int32> Departed;
		if (!TestTrue(TEXT("the roster delta reads on a seeded bridge"),
				Bridge.RosterDeltaOfLastCommand(Appeared, Departed).bOk))
		{
			return false;
		}

		if (Appeared.Num() > 0 || Departed.Num() > 0)
		{
			FoundAt          = Index;
			StandingAppeared = Appeared;
			StandingDeparted = Departed;
			break;
		}
	}

	// ---- 2. THE CONTROL. Without a standing delta nothing below can fail --------
	if (!TestTrue(
			*FString::Printf(
				TEXT("CONTROL: replaying the fixture reaches an accepted command that MOVED the "
				     "roster -- every assertion below is an absence, and an absence read off an "
				     "already-empty delta would pass with both `clear()` calls deleted "
				     "(searched %d fixture commands)"), Count),
			FoundAt != INDEX_NONE))
	{
		return false;
	}

	AddInfo(FString::Printf(
		TEXT("fixture command %d of %d is the first the bridge reports as moving the roster: "
		     "appeared %s, departed %s"),
		FoundAt, Count, *Describe(StandingAppeared), *Describe(StandingDeparted)));

	if (!TestTrue(TEXT("CONTROL: and the standing delta is genuinely non-empty"),
			StandingAppeared.Num() > 0 || StandingDeparted.Num() > 0))
	{
		return false;
	}

	// ---- 3. A refusal the RULES MODULE produces, inside `applyCommand` -----------
	// AN `EndTurn` TAGGED WITH A TURN THE MODULE SAYS IS NOT LIVE. Chosen because it is refused
	// by `applyCommand`'s own tag check rather than screened out by the bridge before the
	// observation is reached -- a refusal the bridge caught earlier would never execute the
	// `!R.ok` path this clause exists for.
	strat::SaveCommand Mistagged;
	Mistagged.kind    = strat::SaveCommandKind::EndTurn;
	Mistagged.turn    = Bridge.Turn() + 1;
	Mistagged.side    = Bridge.SideToMove();
	Mistagged.hasHex  = false;
	Mistagged.hasUnit = false;

	const FStratResult Refused = Bridge.Submit(Mistagged);
	if (!TestFalse(
			TEXT("CONTROL: the rules module refuses an EndTurn tagged with the wrong turn -- an "
			     "accepted command here would make everything below an assertion about the wrong "
			     "path"),
			Refused.bOk))
	{
		return false;
	}
	AddInfo(FString::Printf(TEXT("the refusal's reason was '%s'"), *Refused.Reason));

	// ---- 4 and 5. The delta is EMPTY, and the read still succeeds ----------------
	TArray<int32> AfterAppeared;
	TArray<int32> AfterDeparted;
	const FStratResult Read = Bridge.RosterDeltaOfLastCommand(AfterAppeared, AfterDeparted);

	TestTrue(
		TEXT("the delta still READS after a refusal -- which is what tells 'cleared' from 'the "
		     "bridge lost its seed', since the unseeded refusal empties the out-params too"),
		Read.bOk);

	TestEqual(*FString::Printf(
		TEXT("T-INT-05: a refused command leaves NO appearances -- the previous command "
		     "appeared %s and a reader that ignored the result would otherwise attribute those "
		     "ids to the command §4.9 says changed nothing (it now reports %s)"),
		*Describe(StandingAppeared), *Describe(AfterAppeared)),
		AfterAppeared.Num(), 0);

	TestEqual(*FString::Printf(
		TEXT("T-INT-05: and NO departures -- the previous command departed %s (it now reports "
		     "%s)"),
		*Describe(StandingDeparted), *Describe(AfterDeparted)),
		AfterDeparted.Num(), 0);

	return true;
}
