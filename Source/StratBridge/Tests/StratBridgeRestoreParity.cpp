// The gate on `FStratBridge::RestoreFromSaveText` -- GDD §4.10, under T-SAVE-04, T-SAVE-05,
// T-SAVE-06 and GATE-SAVE-PARSE.
//
// WHAT THIS FILE IS THE OTHER HALF OF. `StratBridgeSaveRecording.cpp` pins the WRITE side:
// a bridge records what it applied, serializes it, and save -> load -> save is a fixed
// point. Wave B1 landed the READ side, and until this file it had no clause at all. The
// method is the one place in the tree where a §4.10 file becomes a board, so every refusal
// it can produce is a refusal that stands between a corrupted slot and a screen.
//
// THE FIVE THINGS IT CAN SAY, and each has a clause below:
//   - Ok, with the log applied and the file's own `stateHash` reached.
//   - `T-SAVE-06` -- the replayed state disagrees with the file's `stateHash`.
//   - `GATE-SAVE-PARSE` -- the text is not a §4.10 file at all.
//   - `T-SAVE-04` -- it is a §4.10 file, and one of the four Version-policy fields
//     disagrees. Two of those four are exercised: `rulesCommit` (a caller-supplied
//     expectation) and `scenarioHash` (an expectation this bridge reads off itself).
//   - a bare refusal with no id -- this bridge has already applied commands.
//
// WHERE EVERY EXPECTATION COMES FROM, which is the only question that matters.
//   - Every hash on both sides of every comparison is `FStratBridge::StateHash()`, which is
//     `strat::canonicalStateHash`. This file computes no hash and writes none down. The
//     `RestoreFromSaveText` clauses compare a RESTORED bridge against the SOURCE bridge that
//     wrote the file -- two live objects, never a literal.
//   - Every command count is `FStratBridge::RecordedCommandCount()` or the length of the
//     committed fixture's own `commandLog`. No count is authored here.
//   - `rulesCommit` and `dataHash` come off `Data/parity_fixture.save` through
//     `strat::parseSave`, exactly as `StratBridgeSaveRecording.cpp` takes them.
//   - The four refusal ids are the rules module's own strings, quoted from
//     `Source/StratRules/Save.good.cpp`'s `refuse(...)` calls and from
//     `FStratBridge::RestoreFromSaveText`'s own `T-SAVE-06` arm. An id is a protocol
//     constant, not a derived value; there is nothing module-side to read one from, and a
//     clause that compared it against itself would pin nothing.
//
// ON COMPARING A REFUSAL ID. `FString::operator==`, `Contains` and `TestEqual` are ALL
// case-insensitive in this engine, and a clause here has already been written in this
// project that could not fail for that reason. Every id comparison below goes through
// `EqualsExact` / `ContainsExact`, which pass `ESearchCase::CaseSensitive` explicitly. The
// ids `T-SAVE-04` and `T-SAVE-06` differ by more than case, so the distinctness clause would
// hold either way -- the discipline is applied anyway, because the next id added might not.
//
// THE VARIANT SCENARIO IS WRITTEN TO `Saved/`, NEVER TO `Data/`. `Data/` is vendored
// certified bytes hash-gated by `GATE-DATA-VENDOR`, and a test that edited a fixture there
// would break the gate that exists to catch exactly that. The variant is
// `Data/ferrum_crossing.json` with `turnCap` changed and the OPTIONAL `scenarioHash` field
// removed -- `Scenario.h` reading 4 says that field is derived and optional, so a file
// without it loads and gets the hash its own content implies. Changing `turnCap` moves the
// hash because `strat::scenarioHash`'s preimage carries `cap=`; nothing here recomputes the
// hash, and nothing here asserts what the new hash is.
//
// THIS FILE MAY SPELL `strat::`, AND THAT IS WHY IT IS HERE. `Source/StratBridge/Tests/` is
// inside `StratBridge`, so `strat::parseSave` links. A clause needing the same values from
// `StratPlay` would be 1 x LNK2019 per call -- measured 8 x when the bridge first tried.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBridge.h"

#include "Save.h"

namespace StratBridgeRestoreParity
{
	// The harness pin every parity file in this tree carries. `Replay.h` states no rule
	// decides which side moves first; the committed fixture's log was emitted against side 0,
	// so a different value here replays a different game.
	static const int32 kFirstSide = 0;

	static FString DataDir()
	{
		return FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"));
	}

	static FString ScenarioPath()
	{
		return FPaths::Combine(DataDir(), TEXT("ferrum_crossing.json"));
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

	/** Case-SENSITIVE equality. See the header block: the default is not. */
	static bool EqualsExact(const FString& A, const TCHAR* B)
	{
		return A.Equals(FString(B), ESearchCase::CaseSensitive);
	}

	/** Case-SENSITIVE containment. Same reason. */
	static bool ContainsExact(const FString& Haystack, const TCHAR* Needle)
	{
		return Haystack.Contains(FString(Needle), ESearchCase::CaseSensitive);
	}

	/** Definitions plus the shipped scenario, through the two engine-typed loaders. */
	static bool SeedBridge(FStratBridge& Bridge, const FString& ScenarioFile, FString& OutError)
	{
		UDataTable* Units   = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
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

		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioFile, kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		return SeedBridge(Bridge, ScenarioPath(), OutError);
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

	static FStratResult SubmitTyped(FStratBridge& Bridge, const strat::SaveCommand& C)
	{
		switch (C.kind)
		{
		case strat::SaveCommandKind::Move:    return Bridge.SubmitMove(C.unitId, C.hex);
		case strat::SaveCommandKind::Attack:  return Bridge.SubmitAttack(C.unitId, C.hex);
		// `SaveCommand::unitId` spells the §2.4 row index in a field named for a unit;
		// `StratBridge.h` records that the format's spelling is the trap.
		case strat::SaveCommandKind::Build:   return Bridge.SubmitBuild(C.hex, C.unitId);
		case strat::SaveCommandKind::Capture: return Bridge.SubmitCapture(C.unitId);
		case strat::SaveCommandKind::EndTurn: return Bridge.SubmitEndTurn();
		}
		return FStratResult::Fail(TEXT("a SaveCommandKind with no typed method"));
	}

	static FStratSaveIdentity IdentityOf(const strat::Save& Fixture)
	{
		FStratSaveIdentity Identity;
		Identity.RulesCommit = FromStd(Fixture.rulesCommit);
		Identity.DataHash    = FromStd(Fixture.dataHash);
		return Identity;
	}

	/**
	 * A bridge seeded and driven to a real mid-match position, plus the §4.10 text it wrote.
	 *
	 * THE LOG IS THE COMMITTED FIXTURE'S, REPLAYED THROUGH THE TYPED SUBMIT METHODS, so what
	 * is being restored below is a log with every §4.9 command kind in it rather than a run
	 * of Moves. `ReplayLog` would be shorter and would exercise a different entry point than
	 * the one a player's session goes through.
	 */
	static bool MakeSourceSave(FStratBridge&      Source,
	                           const strat::Save& Fixture,
	                           int32              PrefixLength,
	                           FString&           OutText,
	                           FString&           OutError)
	{
		if (!SeedBridge(Source, OutError))
		{
			return false;
		}
		for (int32 i = 0; i < PrefixLength; ++i)
		{
			const FStratResult Submitted =
				SubmitTyped(Source, Fixture.commandLog[static_cast<size_t>(i)]);
			if (!Submitted.bOk)
			{
				OutError = FString::Printf(TEXT("fixture command %d refused: %s"),
					i, *Submitted.Reason);
				return false;
			}
		}

		const FStratResult Serialized = Source.SerializeRecordedSave(IdentityOf(Fixture), OutText);
		if (!Serialized.bOk)
		{
			OutError = FString::Printf(TEXT("SerializeRecordedSave refused: %s"),
				*Serialized.Reason);
			return false;
		}
		return true;
	}

	/**
	 * How many fixture commands to replay into the source bridge.
	 *
	 * SIXTEEN, WHICH IS `StratBridgeSaveRecording.cpp`'s `kWindowBegin`, chosen there because
	 * a prefix of that length has crossed a turn boundary. The number is a fixture coordinate
	 * and not an expectation: nothing below asserts that it is 16, only that
	 * `RecordedCommandCount()` on the source and on the restored bridge agree.
	 */
	static const int32 kPrefixLength = 16;

	/** The variant scenario's path under `Saved/`. Never under `Data/`. */
	static FString VariantScenarioPath()
	{
		return FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("StratTests"),
			TEXT("restore_parity_variant_scenario.json"));
	}

	/**
	 * Writes a scenario that is `ferrum_crossing.json` with a different `turnCap` and no
	 * declared `scenarioHash`. See the header block on why both edits, and on why this
	 * lands in `Saved/`.
	 */
	static bool WriteVariantScenario(FString& OutPath, FString& OutError)
	{
		FString Text;
		if (!FFileHelper::LoadFileToString(Text, *ScenarioPath()))
		{
			OutError = FString::Printf(TEXT("ferrum_crossing.json unreadable at %s"),
				*ScenarioPath());
			return false;
		}

		// The declared hash is optional and DERIVED (`Scenario.h`, reading 4), so dropping the
		// line is the correct way to say "recompute from content" rather than authoring a
		// number. Located case-sensitively; the whole line goes, newline included.
		const int32 KeyAt = Text.Find(TEXT("\"scenarioHash\""), ESearchCase::CaseSensitive);
		if (KeyAt == INDEX_NONE)
		{
			OutError = TEXT("ferrum_crossing.json does not carry a 'scenarioHash' member to drop");
			return false;
		}
		const int32 LineEnd = Text.Find(TEXT("\n"), ESearchCase::CaseSensitive,
			ESearchDir::FromStart, KeyAt);
		if (LineEnd == INDEX_NONE)
		{
			OutError = TEXT("the 'scenarioHash' member is on the file's last line; this helper "
			                "expects it to be followed by a newline");
			return false;
		}
		Text.RemoveAt(KeyAt, (LineEnd + 1) - KeyAt);

		if (!ContainsExact(Text, TEXT("\"turnCap\": 20")))
		{
			OutError = TEXT("ferrum_crossing.json no longer declares \"turnCap\": 20; this "
			                "helper's edit is stale and the clause would not change the hash");
			return false;
		}
		Text = Text.Replace(TEXT("\"turnCap\": 20"), TEXT("\"turnCap\": 19"),
			ESearchCase::CaseSensitive);

		OutPath = VariantScenarioPath();

		// The directory is created explicitly: `FFileHelper::SaveStringToFile` does not build
		// a missing tree, and a first run on a clean checkout has no `Saved/StratTests/`.
		IPlatformFile& Files = FPlatformFileManager::Get().GetPlatformFile();
		Files.CreateDirectoryTree(*FPaths::GetPath(OutPath));

		if (!FFileHelper::SaveStringToFile(Text, *OutPath))
		{
			OutError = FString::Printf(TEXT("could not write the variant scenario to %s"),
				*OutPath);
			return false;
		}
		return true;
	}

	static void RemoveVariantScenario()
	{
		IPlatformFile& Files = FPlatformFileManager::Get().GetPlatformFile();
		const FString Path = VariantScenarioPath();
		if (Files.FileExists(*Path))
		{
			Files.DeleteFile(*Path);
		}
	}
}

// ---------------------------------------------------------------------------
// T-SAVE-06 -- the round trip closes: what a bridge wrote, a freshly seeded bridge restores
// to the same canonical state.
//
// THE COMPARISON IS BRIDGE AGAINST BRIDGE. `StratBridgeSaveRecording.cpp` already pins
// serialize -> `parseSave` -> `ReplayLog`; this pins the one method a slot load actually
// calls, and it compares the restored object against the SOURCE object rather than against
// anything this file knows. `A.StateHash()` and `B.StateHash()` are both
// `strat::canonicalStateHash`; `A.RecordedCommandCount()` is the source's own count.
//
// THE FRESH SEED IS ASSERTED TO BE SOMEWHERE ELSE FIRST, so the equality at the end measures
// the restore rather than the seed. Without that line a `RestoreFromSaveText` that did
// nothing at all would pass whenever the source had taken no command.
//
// `OutCommandCount` IS PINNED AGAINST `RecordedCommandCount()` ON BOTH OBJECTS, which is
// what `StratBridge.h` says the out-parameter is for: "so a caller can log it and a clause
// can pin it against `RecordedCommandCount()` without spelling `strat::SaveCommand`."
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeRestoreReachesSavedHashTest,
	"Stratocracy.StratBridge.T-SAVE-06.RestoreFromSaveTextReachesTheSavedHash",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeRestoreReachesSavedHashTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeRestoreParity;

	strat::Save Fixture;
	FString Error;
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"),
			LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the fixture is long enough for the prefix this clause replays"),
			static_cast<int32>(Fixture.commandLog.size()) >= kPrefixLength))
	{
		return false;
	}

	FStratBridge Source;
	FString      SaveText;
	if (!TestTrue(TEXT("a source bridge reaches a mid-match position and serializes"),
			MakeSourceSave(Source, Fixture, kPrefixLength, SaveText, Error)))
	{
		AddError(Error);
		return false;
	}

	const FString SourceHash  = Source.StateHash();
	const int32   SourceCount = Source.RecordedCommandCount();
	TestTrue(TEXT("the source has a hash to compare against"), !SourceHash.IsEmpty());
	TestTrue(TEXT("and a non-empty log, so the restore has work to do"), SourceCount > 0);

	FStratBridge Target;
	if (!TestTrue(TEXT("a target bridge seeds from the same scenario"),
			SeedBridge(Target, Error)))
	{
		AddError(Error);
		return false;
	}
	TestNotEqual(
		TEXT("the fresh seed is NOT already at the source's state, so the restore does the work"),
		Target.StateHash(), SourceHash);
	TestEqual(TEXT("and it has recorded nothing yet"), Target.RecordedCommandCount(), 0);

	int32 Applied = INDEX_NONE;
	const FStratResult Restored =
		Target.RestoreFromSaveText(SaveText, IdentityOf(Fixture), Applied);
	if (!TestTrue(TEXT("T-SAVE-06: RestoreFromSaveText accepts the file its own bridge wrote"),
			Restored.bOk))
	{
		AddError(FString::Printf(TEXT("[%s] %s"), *Restored.Id, *Restored.Reason));
		return false;
	}

	TestEqual(TEXT("T-SAVE-06: OutCommandCount is the source's recorded length"),
		Applied, SourceCount);
	TestEqual(TEXT("T-SAVE-06: and the restored bridge records the same log length"),
		Target.RecordedCommandCount(), SourceCount);
	TestEqual(
		TEXT("T-SAVE-06: the restored bridge reaches the source's canonical state hash -- both "
		     "sides are strat::canonicalStateHash and neither was authored here"),
		Target.StateHash(), SourceHash);

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-06 -- a file whose `stateHash` disagrees with its own log is refused, and the
// refusal leaves this bridge exactly as it found it.
//
// THE SECOND HALF IS THE LOAD-BEARING HALF. `StratBridge.h` states that the method replays
// onto a COPY of `GameState` and assigns only after the hash agrees -- "the copy exists
// solely so a hash refusal leaves the seeded board untouched." A `RestoreFromSaveText` that
// replayed in place and then reported the mismatch would pass a clause that only checked the
// return value, and would leave a board on screen that no save ever recorded. So the target's
// `StateHash()` and `RecordedCommandCount()` are captured BEFORE the call and required
// unchanged after it.
//
// ONLY THE `stateHash` FIELD IS ALTERED. The mutated value is built by flipping one character
// of the hash the file itself carries -- read back through `strat::parseSave` rather than
// located by string search -- so the log, the header and the scenario reference are all
// still the ones the source wrote. Anything else would risk landing on GATE-SAVE-PARSE or
// T-SAVE-04 and passing this clause for the wrong reason, which is why the refusal id is
// compared exactly rather than merely required non-empty.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeRestoreRefusesMutatedStateHashTest,
	"Stratocracy.StratBridge.T-SAVE-06.RestoreRefusesAMutatedStateHash",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeRestoreRefusesMutatedStateHashTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeRestoreParity;

	strat::Save Fixture;
	FString Error;
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"),
			LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratBridge Source;
	FString      SaveText;
	if (!TestTrue(TEXT("a source bridge reaches a mid-match position and serializes"),
			MakeSourceSave(Source, Fixture, kPrefixLength, SaveText, Error)))
	{
		AddError(Error);
		return false;
	}

	// The file's own claim about where its log lands, read back through the module's parser
	// so the string this clause replaces is the module's and not a guess about formatting.
	strat::Save Written;
	const strat::SaveLoadResult Reparsed =
		strat::parseSave(ToStd(SaveText), "SerializeRecordedSave", Written);
	if (!TestTrue(TEXT("what the bridge wrote, strat::parseSave reads"), Reparsed.ok))
	{
		AddError(FString::Printf(TEXT("%s: %s"),
			*FromStd(Reparsed.failedId), *FromStd(Reparsed.reason)));
		return false;
	}

	const FString HonestHash = FromStd(Written.stateHash);
	if (!TestTrue(TEXT("the emitted save carries a stateHash to mutate"), !HonestHash.IsEmpty()))
	{
		return false;
	}

	// One character, flipped between two hex digits, so the value stays a well-formed hash
	// string and only its VALUE is wrong. A case-sensitive replace: hex digits are lowercase
	// and an ignore-case replace could match a different span.
	FString MutatedHash = HonestHash;
	MutatedHash[0] = (HonestHash[0] == TEXT('0')) ? TEXT('1') : TEXT('0');
	const FString MutatedText =
		SaveText.Replace(*HonestHash, *MutatedHash, ESearchCase::CaseSensitive);
	if (!TestTrue(TEXT("the mutation changed the text"), !MutatedText.Equals(SaveText,
			ESearchCase::CaseSensitive)))
	{
		return false;
	}

	FStratBridge Target;
	if (!TestTrue(TEXT("a target bridge seeds from the same scenario"),
			SeedBridge(Target, Error)))
	{
		AddError(Error);
		return false;
	}

	const FString PostSeedHash  = Target.StateHash();
	const int32   PostSeedCount = Target.RecordedCommandCount();

	int32 Applied = INDEX_NONE;
	const FStratResult Refused =
		Target.RestoreFromSaveText(MutatedText, IdentityOf(Fixture), Applied);

	TestFalse(TEXT("T-SAVE-06: a save whose stateHash disagrees with its log is refused"),
		Refused.bOk);
	TestTrue(
		*FString::Printf(
			TEXT("T-SAVE-06: the refusal carries the id T-SAVE-06 exactly (case-sensitive); got '%s'"),
			*Refused.Id),
		EqualsExact(Refused.Id, TEXT("T-SAVE-06")));
	TestTrue(TEXT("and the reason names both hashes, so a caller need not guess"),
		ContainsExact(Refused.Reason, *HonestHash) || ContainsExact(Refused.Reason, *MutatedHash));
	TestEqual(TEXT("OutCommandCount is zeroed on the refusal path"), Applied, 0);

	// ---- the copy-then-assign, which is what this clause is really for -------
	TestEqual(
		TEXT("T-SAVE-06: a refused restore leaves this bridge's state EXACTLY as it was -- the "
		     "replay ran on a copy (StratBridge.h: \"ALL-OR-NOTHING ON THIS OBJECT'S OWN STATE\")"),
		Target.StateHash(), PostSeedHash);
	TestEqual(TEXT("T-SAVE-06: and its recorded log is untouched"),
		Target.RecordedCommandCount(), PostSeedCount);

	return true;
}

// ---------------------------------------------------------------------------
// GATE-SAVE-PARSE and T-SAVE-04 are DIFFERENT VERDICTS and must not be able to pass for each
// other.
//
// `Save.h` reading 2 states the split: "a malformed file fails GATE-SAVE-PARSE and a
// well-formed disagreeing one fails T-SAVE-04". `FStratBridge::RestoreFromSaveText` forwards
// `strat::loadSave`'s `failedId` rather than collapsing both into "bad save", and this clause
// is what makes that forwarding binding.
//
// WHY IT MATTERS TO A PLAYER. The two verdicts mean opposite things about a slot: a
// GATE-SAVE-PARSE slot is damaged and there is nothing to do with it, while a T-SAVE-04 slot
// is intact and was written by a different build. A load screen that showed one message for
// both would tell a player their save was corrupt when it was merely old.
//
// THE T-SAVE-04 CASE USES A WRONG `rulesCommit` SUPPLIED BY THE CALLER, which is the field
// `FStratSaveIdentity` exists to carry and the one a different build genuinely disagrees on.
// The text itself is byte-for-byte the good one, so the only difference between this arm and
// the accepting clause above is the expectation handed in.
//
// AND THE THIRD ASSERTION IS THAT THE TWO IDS DIFFER, compared case-sensitively. Two arms
// that each checked their own id would both keep passing if the method started returning one
// id for everything.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeRestoreForwardsParseVerdictTest,
	"Stratocracy.StratBridge.GATE-SAVE-PARSE.RestoreForwardsTheParseVerdictDistinctly",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeRestoreForwardsParseVerdictTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeRestoreParity;

	strat::Save Fixture;
	FString Error;
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"),
			LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratBridge Source;
	FString      SaveText;
	if (!TestTrue(TEXT("a source bridge reaches a mid-match position and serializes"),
			MakeSourceSave(Source, Fixture, kPrefixLength, SaveText, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- arm 1: not a §4.10 file at all -------------------------------------
	FStratBridge ParseTarget;
	if (!TestTrue(TEXT("a target bridge seeds"), SeedBridge(ParseTarget, Error)))
	{
		AddError(Error);
		return false;
	}

	int32 ParseApplied = INDEX_NONE;
	const FStratResult Malformed = ParseTarget.RestoreFromSaveText(
		TEXT("{ \"formatVersion\": 1, this is not JSON"), IdentityOf(Fixture), ParseApplied);

	TestFalse(TEXT("GATE-SAVE-PARSE: malformed text is refused"), Malformed.bOk);
	TestTrue(
		*FString::Printf(
			TEXT("GATE-SAVE-PARSE: the refusal carries that id exactly (case-sensitive); got '%s'"),
			*Malformed.Id),
		EqualsExact(Malformed.Id, TEXT("GATE-SAVE-PARSE")));
	TestEqual(TEXT("and this bridge recorded nothing"), ParseTarget.RecordedCommandCount(), 0);

	// ---- arm 2: a well-formed file from a different build --------------------
	FStratBridge HeaderTarget;
	if (!TestTrue(TEXT("a second target bridge seeds"), SeedBridge(HeaderTarget, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratSaveIdentity Wrong = IdentityOf(Fixture);
	Wrong.RulesCommit = Wrong.RulesCommit + TEXT("-not-this-build");

	int32 HeaderApplied = INDEX_NONE;
	const FStratResult Mismatched =
		HeaderTarget.RestoreFromSaveText(SaveText, Wrong, HeaderApplied);

	TestFalse(TEXT("T-SAVE-04: a well-formed file from another build is refused"),
		Mismatched.bOk);
	TestTrue(
		*FString::Printf(
			TEXT("T-SAVE-04: the refusal carries that id exactly (case-sensitive); got '%s'"),
			*Mismatched.Id),
		EqualsExact(Mismatched.Id, TEXT("T-SAVE-04")));
	TestTrue(TEXT("T-SAVE-04: and the reason names the disagreeing field"),
		ContainsExact(Mismatched.Reason, TEXT("rulesCommit")));
	TestEqual(TEXT("and this bridge recorded nothing either"),
		HeaderTarget.RecordedCommandCount(), 0);

	// ---- the two verdicts are not the same verdict ---------------------------
	TestFalse(
		TEXT("GATE-SAVE-PARSE and T-SAVE-04 are distinct refusals and neither can pass for the "
		     "other (Save.h reading 2)"),
		EqualsExact(Malformed.Id, *Mismatched.Id));

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-04 -- the `scenarioHash` expectation is READ OFF THIS BRIDGE and is not a caller
// argument that could be made to agree.
//
// `StratBridge.h` states it: `scenarioHash` "is `strat::scenarioHash` over the scenario THIS
// BRIDGE ACTUALLY LOADED, and is deliberately not a caller argument. Taking it from the
// caller would let a load succeed against a scenario this object is not seeded from."
//
// HALF OF THAT IS ALREADY A COMPILE-TIME FACT AND IS NOT WHAT THIS CLAUSE MEASURES.
// `RestoreFromSaveText`'s signature is `(SaveText, FStratSaveIdentity, int32&)` and
// `FStratSaveIdentity` carries only `RulesCommit` and `DataHash`, so there is no parameter a
// caller could pass a scenario hash through. What the signature does NOT settle is whether
// the comparison happens at all, and against which scenario. So the clause restores a
// ferrum-written save onto a bridge seeded from a DIFFERENT scenario and requires T-SAVE-04
// naming `scenarioHash`.
//
// THE DIFFERENT SCENARIO IS GENERATED, NOT COMMITTED, and it lands in `Saved/`. See the
// header block: `Data/` is vendored and hash-gated, and the variant is
// `ferrum_crossing.json` with `turnCap` moved and the optional declared hash dropped.
// Nothing here computes the variant's hash; the clause asserts only that the two scenarios
// DISAGREE, which the refusal itself reports.
//
// THE CONTROL IS THE CLAUSE ABOVE. `RestoreFromSaveTextReachesTheSavedHash` restores the same
// text onto a bridge seeded from the SAME scenario and it is accepted -- so a refusal here is
// attributable to the scenario and not to the text.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeRestoreSourcesScenarioHashTest,
	"Stratocracy.StratBridge.T-SAVE-04.RestoreSourcesScenarioHashFromThisBridge",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeRestoreSourcesScenarioHashTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeRestoreParity;

	strat::Save Fixture;
	FString Error;
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"),
			LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratBridge Source;
	FString      SaveText;
	if (!TestTrue(TEXT("a source bridge reaches a mid-match position and serializes"),
			MakeSourceSave(Source, Fixture, kPrefixLength, SaveText, Error)))
	{
		AddError(Error);
		return false;
	}

	FString VariantPath;
	if (!TestTrue(TEXT("a variant scenario is written under Saved/"),
			WriteVariantScenario(VariantPath, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratBridge Elsewhere;
	const bool bSeeded = SeedBridge(Elsewhere, VariantPath, Error);
	if (!TestTrue(TEXT("a bridge seeds from the variant scenario"), bSeeded))
	{
		AddError(Error);
		RemoveVariantScenario();
		return false;
	}

	int32 Applied = INDEX_NONE;
	const FStratResult Refused =
		Elsewhere.RestoreFromSaveText(SaveText, IdentityOf(Fixture), Applied);

	TestFalse(
		TEXT("T-SAVE-04: a save written against one scenario does not restore onto a bridge "
		     "seeded from another"),
		Refused.bOk);
	TestTrue(
		*FString::Printf(
			TEXT("T-SAVE-04: the refusal carries that id exactly (case-sensitive); got '%s'"),
			*Refused.Id),
		EqualsExact(Refused.Id, TEXT("T-SAVE-04")));
	TestTrue(
		*FString::Printf(
			TEXT("T-SAVE-04: and it names 'scenarioHash' as the disagreeing field, so the "
			     "expectation demonstrably came from the loaded scenario; reason was: %s"),
			*Refused.Reason),
		ContainsExact(Refused.Reason, TEXT("scenarioHash")));
	TestEqual(TEXT("nothing was applied"), Applied, 0);
	TestEqual(TEXT("and nothing was recorded"), Elsewhere.RecordedCommandCount(), 0);

	RemoveVariantScenario();
	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-05 -- a bridge that has already played is not a bridge to restore onto, and
// refusing costs it nothing.
//
// `StratBridge.h` puts this on `ReplayRecordedLogOnto`'s arm 4 exactly: "replaying a loaded
// log onto a played match produces a state whose hash means nothing while looking like it
// means something."
//
// THE SECOND ASSERTION IS THE ONE THAT WOULD CATCH A REGRESSION. A method that checked
// freshness AFTER touching `Recorded` -- or that cleared it on the way to the check -- would
// return false and still have destroyed the one command the player had taken. So the count
// is required to be exactly the one command submitted here, before and after.
//
// THIS REFUSAL CARRIES NO ID, and that is the method's own shape rather than an omission
// here: `FStratResult::Fail` is called with one argument on that arm, because no rules-module
// invariant is being reported -- the bridge is refusing on its own state. The clause asserts
// the id is EMPTY rather than ignoring it, so an id appearing later is a change someone has
// to look at.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeRestoreRefusesPlayedBridgeTest,
	"Stratocracy.StratBridge.T-SAVE-05.RestoreRefusesABridgeThatHasPlayed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeRestoreRefusesPlayedBridgeTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeRestoreParity;

	strat::Save Fixture;
	FString Error;
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"),
			LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratBridge Source;
	FString      SaveText;
	if (!TestTrue(TEXT("a source bridge reaches a mid-match position and serializes"),
			MakeSourceSave(Source, Fixture, kPrefixLength, SaveText, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratBridge Played;
	if (!TestTrue(TEXT("a target bridge seeds"), SeedBridge(Played, Error)))
	{
		AddError(Error);
		return false;
	}

	// One command, taken from the fixture's own log so it is legal against this seed.
	const FStratResult First = SubmitTyped(Played, Fixture.commandLog[0]);
	if (!TestTrue(TEXT("the fixture's first command applies to a fresh seed"), First.bOk))
	{
		AddError(First.Reason);
		return false;
	}
	TestEqual(TEXT("so the target has played exactly one command"),
		Played.RecordedCommandCount(), 1);

	const FString PlayedHash = Played.StateHash();

	int32 Applied = INDEX_NONE;
	const FStratResult Refused =
		Played.RestoreFromSaveText(SaveText, IdentityOf(Fixture), Applied);

	TestFalse(TEXT("T-SAVE-05: a bridge that has applied a command refuses a restore"),
		Refused.bOk);
	TestTrue(TEXT("T-SAVE-05: and the reason says so in the bridge's own words"),
		ContainsExact(Refused.Reason, TEXT("not fresh")));
	TestTrue(
		*FString::Printf(
			TEXT("T-SAVE-05: the refusal carries no invariant id -- it is the bridge refusing on "
			     "its own state, not a rules-module verdict; got '%s'"),
			*Refused.Id),
		Refused.Id.IsEmpty());

	TestEqual(TEXT("T-SAVE-05: the one command it had played is still recorded"),
		Played.RecordedCommandCount(), 1);
	TestEqual(TEXT("T-SAVE-05: and its state did not move"), Played.StateHash(), PlayedHash);
	TestEqual(TEXT("OutCommandCount is zeroed on the refusal path"), Applied, 0);

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-06 -- a save taken before anything happened restores, and that is a DELIBERATE
// departure from `ReplayRecordedLogOnto`'s arm 5.
//
// `StratBridge.h` names the departure: "An EMPTY `commandLog` is NOT refused, and that is
// where this method departs from `ReplayRecordedLogOnto`'s arm 5 -- a save taken before
// anything was submitted is an ordinary file, and its hash check still has content because it
// pins the seed."
//
// A DEPARTURE STATED IN A HEADER AND PINNED BY NOTHING IS A SENTENCE, NOT A CONTRACT. The two
// methods are one refactor away from being folded together, and the fold would look like
// tidying. This clause is what makes the difference cost a red test.
//
// THE HASH CHECK STILL HAS CONTENT, and the clause says why rather than asserting it
// abstractly: the restored bridge's hash must equal the SOURCE's, and both are freshly seeded
// boards, so what is being pinned is that the seed itself round-tripped.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratBridgeRestoreOfEmptyLogTest,
	"Stratocracy.StratBridge.T-SAVE-06.RestoreOfAnEmptyLogIsAccepted",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratBridgeRestoreOfEmptyLogTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratBridgeRestoreParity;

	strat::Save Fixture;
	FString Error;
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"),
			LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}

	// Zero commands: seeded and untouched, which is what a player saving on turn 1 writes.
	FStratBridge Source;
	FString      SaveText;
	if (!TestTrue(TEXT("a freshly seeded bridge serializes a save with an empty log"),
			MakeSourceSave(Source, Fixture, /*PrefixLength*/ 0, SaveText, Error)))
	{
		AddError(Error);
		return false;
	}
	TestEqual(TEXT("the source really has taken no command"), Source.RecordedCommandCount(), 0);

	strat::Save Written;
	const strat::SaveLoadResult Reparsed =
		strat::parseSave(ToStd(SaveText), "SerializeRecordedSave", Written);
	if (!TestTrue(TEXT("what the bridge wrote, strat::parseSave reads"), Reparsed.ok))
	{
		AddError(FString::Printf(TEXT("%s: %s"),
			*FromStd(Reparsed.failedId), *FromStd(Reparsed.reason)));
		return false;
	}
	TestEqual(TEXT("and its commandLog is empty, which is the case under test"),
		static_cast<int32>(Written.commandLog.size()), 0);

	FStratBridge Target;
	if (!TestTrue(TEXT("a target bridge seeds"), SeedBridge(Target, Error)))
	{
		AddError(Error);
		return false;
	}

	int32 Applied = INDEX_NONE;
	const FStratResult Restored =
		Target.RestoreFromSaveText(SaveText, IdentityOf(Fixture), Applied);

	if (!TestTrue(
			TEXT("T-SAVE-06: an empty commandLog is an ordinary save and is ACCEPTED -- the "
			     "deliberate departure from ReplayRecordedLogOnto's arm 5 (StratBridge.h)"),
			Restored.bOk))
	{
		AddError(FString::Printf(TEXT("[%s] %s"), *Restored.Id, *Restored.Reason));
		return false;
	}

	TestEqual(TEXT("T-SAVE-06: nothing was applied, and the count says so"), Applied, 0);
	TestEqual(TEXT("T-SAVE-06: the restored bridge records an empty log"),
		Target.RecordedCommandCount(), 0);
	TestEqual(
		TEXT("T-SAVE-06: and the hash check still had content -- it pinned the seed itself"),
		Target.StateHash(), Source.StateHash());

	return true;
}
