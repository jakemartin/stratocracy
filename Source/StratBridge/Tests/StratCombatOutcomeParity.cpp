// The in-engine gate on the `STRAT-COMBAT` family -- T-UI-01's resolution end, and
// T-FAME-01's kill award seen through it. GDD §2.6, §2.11, §4.9, §4.11 row T-UI-01.
//
// WHAT WAS UNGATED BEFORE THIS FILE. `FStratBridge::Submit` emits a `STRAT-COMBAT`
// line for every Attack that reaches it (StratBridge.cpp's `StratCombatObservation`
// block). Nothing read one. The emitter's own agreement field, `agree=`, was
// therefore a number this project produced 483 times in a suite run and never once
// checked against anything outside the emitter that produced it -- which is the exact
// shape of a test that tests itself, moved one level down into production code.
//
// WHERE EVERY EXPECTATION IN THIS FILE COMES FROM, and the answer is never "computed
// here":
//
//   - `strat::uiResolveForGate` (Ui.h:355-361, Ui.good.cpp:290) is THE ORACLE. It
//     calls `uiForecast` and applies its numbers, "adding no arithmetic of its own",
//     and it has zero production callers in this tree BY CONSTRUCTION -- the phase
//     that wrote the emitter deliberately did not call it, and
//     StratBridge.cpp's block says so in as many words, so that it would still be an
//     independent second opinion when a gate arrived. It must stay uncalled from
//     production. A `uiResolveForGate` invoked by the bridge would make every
//     comparison below a comparison of the bridge with itself.
//
//   - `strat::killAward` (Economy.h:108) supplies the fame figure. Not half a cost
//     computed here; the module's own function, handed the victim's own `UnitDef`.
//
//   - `Data/parity_fixture.save` supplies every command, and the positions they are
//     submitted from. A hand-authored mid-match board would be this file choosing the
//     numbers it then asserts on.
//
// THE COUNTER-KILL CLAUSE IS THE REASON THIS FILE IS WORTH ITS LENGTH. Measured
// 2026-08-13 across one full suite run: `adied=1` occurred ZERO times in 483
// resolutions, so the attacker-death arm of `CaptureAfter` -- the `bAttackerDied` /
// `AttackerHpAfter == -1` branch -- had never executed in the history of this project.
// It is not reachable from the fixture's own script either: all 74 fixture Attacks
// forecast `counterDamage < attacker hp` (probe, same date). It IS reachable OFF the
// script: replaying the fixture's first 20 commands and then scanning every legal
// attack available to the active side finds 108 counter-kill opportunities across the
// match, the first at prefix 20. That is what `CounterKillLeavesTheAttackerOffTheRoster`
// submits, and the position it submits from is the fixture's, not this file's.
//
// WHAT THIS FILE DOES NOT AND CANNOT PIN -- stated here rather than papered over.
// `agree=0` / `STRAT-COMBAT divergence` is UNREACHABLE through `Submit`, and it is
// unreachable for a good reason: `uiForecast` and `applyCommand`'s Attack case call
// the SAME `Combat.h::resolveDamage` and `Combat.h::defenderCanCounter` over the same
// state within one call, so a disagreement is not a state a caller can drive the
// module into. The `LegalityDisagrees` arm is unreachable for the mirror reason --
// every `uiForecast` illegality (unknown unit, empty hex, self, same side, out of
// range) is refused by `applyCommand` too, so a forecast-illegal attack never applies.
// `agree=-1` on a `resolved` line is likewise unreachable: it needs `!bForecastQueried`
// (attacker not on the board -- `applyCommand` refuses), `DefenderId == INDEX_NONE`
// (empty target hex -- `applyCommand` refuses, Replay.good.cpp:421), or a snapshot
// projection that fails after a successful apply. NO CLAUSE BELOW CLAIMS THOSE ARMS.
// A clause that appeared to cover them would report green forever.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "HAL/CriticalSection.h"
#include "Misc/FileHelper.h"
#include "Misc/OutputDevice.h"
#include "Misc/Paths.h"
#include "Misc/ScopeLock.h"
#include "UObject/UObjectGlobals.h"

#include "StratBridge.h"

#include "Economy.h"
#include "Save.h"
#include "Ui.h"

namespace StratCombatOutcomeParity
{
	// Must equal StratBridgeSaveRecording.cpp's `kFirstSide`. The fixture's log was
	// emitted against side 0; a different value here replays a different game.
	static const int32 kFirstSide = 0;

	// ---- the fixture indices this file drives, and why each ------------------
	// Measured by probe, 2026-08-13, against `Data/parity_fixture.save` (169 commands,
	// 74 of them Attacks). Every one is re-checked at run time against the fixture's own
	// entry, so a re-emitted fixture fails LOUDLY on the check rather than quietly
	// testing whatever ended up at the index.

	/** Attack at 13: unit 10 -> hex (3,4). Forecast damage 3, defender survives, counter
	 *  FIRES for 1. Both halves of the comparison are non-zero, which is what stops
	 *  "measured == predicted" from being satisfied by nothing happening. */
	static const int32 kAttackWithCounter = 13;

	/** Attack at 33: unit 10 -> hex (3,4). Forecast damage 2, defender DIES. The fame
	 *  clause needs a kill and this is the fixture's third one. */
	static const int32 kAttackWithKill = 33;

	/** The shortest prefix after which a counter-KILL is available to the active side.
	 *  Not a fixture command -- the fixture never plays one. See the header block. */
	static const int32 kCounterKillPrefix = 20;

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

	static FString ScenarioPath()
	{
		return FPaths::Combine(DataDir(), TEXT("ferrum_crossing.json"));
	}

	static bool SeedBridge(FStratBridge& Bridge, FString& OutError)
	{
		UDataTable* Units =
			LoadObject<UDataTable>(nullptr, TEXT("/Game/StratData/DT_Units.DT_Units"));
		UDataTable* Terrain =
			LoadObject<UDataTable>(nullptr, TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
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

	/**
	 * Seed, then replay the fixture's first `PrefixLength` commands.
	 *
	 * `ReplayLog` AND NOT A LOOP OF `Submit`, deliberately: `ReplayLog` does not pass
	 * through `Submit` (StratBridge.h says so, and the .cpp measures it), so the prefix
	 * emits NO `STRAT-COMBAT` line however many Attacks it contains. That is what lets a
	 * capture opened afterwards see exactly the lines this file's own submission caused.
	 */
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
	 * The windowed log capture.
	 *
	 * `CanBeUsedOnMultipleThreads() == true` IS LOAD-BEARING AND ITS ABSENCE IS A
	 * MEASURED 1-IN-4 FLAKE, not a style point. Without it the device lands in
	 * `FOutputDeviceRedirector::BufferedOutputDevices` and is handed whatever the
	 * redirector's queue drains next -- including lines emitted BEFORE this object
	 * existed. With it the device sits in `UnbufferedOutputDevices`, fed only by the
	 * synchronous broadcast inside the emitting `UE_LOG`, so the window is exactly this
	 * object's lifetime. StratHotSeatReplayParity.cpp:293-319 records the measurement in
	 * full; four devices in this tree carry the override for that reason and this is the
	 * fifth.
	 *
	 * The override tells the engine no external locking is needed and the engine takes
	 * it literally, so `Lines` is MUTATED under `Mutex`. Reading `Lines` is unlocked and
	 * game-thread-only; every read below is on the game thread.
	 *
	 * IT KEEPS EVERY `STRAT-` LINE AND NOT ONLY `STRAT-COMBAT`. The refusal clause needs
	 * to assert that no `STRAT-CMD` token appears beside a `STRAT-COMBAT` one, and a
	 * filter that dropped `STRAT-CMD` at the door would make that assertion unfalsifiable
	 * by construction.
	 */
	struct FStratCombatCapture final : public FOutputDevice
	{
		TArray<FString> Lines;

		FStratCombatCapture()
		{
			if (GLog != nullptr)
			{
				GLog->AddOutputDevice(this);
			}
		}

		virtual ~FStratCombatCapture()
		{
			if (GLog != nullptr)
			{
				GLog->RemoveOutputDevice(this);
			}
		}

		/** See the block above. Removing this line restores the 1-in-4 flake. */
		virtual bool CanBeUsedOnMultipleThreads() const override { return true; }

		virtual void Serialize(const TCHAR* Message, ELogVerbosity::Type /*Verbosity*/,
		                       const FName& /*Category*/) override
		{
			const FString Line(Message);
			// CASE-SENSITIVE ON PURPOSE. `FString::StartsWith` defaults to
			// IgnoreCase in UE 5.8, and a case-insensitive filter on this project
			// already produced a clause that could not fail.
			if (Line.StartsWith(TEXT("STRAT-"), ESearchCase::CaseSensitive))
			{
				FScopeLock Lock(&Mutex);
				Lines.Add(Line);
			}
		}

		int32 CountWithPhrase(const TCHAR* Phrase) const
		{
			int32 Count = 0;
			for (const FString& Line : Lines)
			{
				if (Line.StartsWith(Phrase, ESearchCase::CaseSensitive))
				{
					++Count;
				}
			}
			return Count;
		}

		/** The single line beginning with `Phrase`, or nullptr when there is not exactly
		 *  one. "Not exactly one" is deliberately indistinguishable from "none" here --
		 *  every caller asserts the count first. */
		const FString* SoleLineWithPhrase(const TCHAR* Phrase) const
		{
			const FString* Found = nullptr;
			for (const FString& Line : Lines)
			{
				if (Line.StartsWith(Phrase, ESearchCase::CaseSensitive))
				{
					if (Found != nullptr)
					{
						return nullptr;
					}
					Found = &Line;
				}
			}
			return Found;
		}

	private:
		FCriticalSection Mutex;
	};

	/**
	 * Reads one `key=value` field out of a `STRAT-COMBAT` line.
	 *
	 * THE FIELD NAMES ARE AN INTERFACE. StratCombatLog.h:27-30 says so: once a gate greps
	 * a token, re-spelling it breaks the gate silently. This helper is that grep, and a
	 * missing key returns false rather than a default, so a renamed field surfaces as a
	 * named failure instead of as a comparison against zero.
	 */
	static bool ReadIntField(const FString& Line, const TCHAR* Key, int32& OutValue)
	{
		TArray<FString> Tokens;
		Line.ParseIntoArray(Tokens, TEXT(" "), /*InCullEmpty*/ true);
		for (const FString& Token : Tokens)
		{
			FString K;
			FString V;
			if (Token.Split(TEXT("="), &K, &V) &&
				K.Equals(Key, ESearchCase::CaseSensitive))
			{
				OutValue = FCString::Atoi(*V);
				return true;
			}
		}
		return false;
	}

	static const strat::UiUnit* FindUnitOnHex(const strat::UiWorld& W, const strat::Hex& H)
	{
		for (const strat::UiUnit& U : W.units)
		{
			if (strat::hexEqual(U.hex, H))
			{
				return &U;
			}
		}
		return nullptr;
	}
}

// ---------------------------------------------------------------------------
// T-UI-01 -- what the forecast predicted is what the state did, across a real
// accepted attack, measured against an oracle the bridge never touches.
//
// THE COMPARISON. `strat::uiResolveForGate` is called on the PRE-SUBMIT `UiWorld` and
// its two numbers -- `attackerHpAfter`, `defenderHpAfter` -- are compared against the
// two HP fields of the post-submit `UiSnapshot`, and against the `ahpAfter=` /
// `dhpAfter=` fields of the line the bridge emitted. Three readings of the same fact
// from two independent producers; nothing here computes damage.
//
// THE POSITIVE CONTROL IS ASSERTED BEFORE THE COMPARISON, because
// "predicted == measured" is trivially true when the prediction is "nothing happens".
// The chosen fixture attack forecasts a non-zero defender loss AND a firing counter, so
// BOTH sides of the comparison have to move, and each is asserted to have moved before
// the equality that depends on it is read.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCombatOutcomeAgreesWithForecastTest,
	"Stratocracy.StratBridge.T-UI-01.CombatOutcomeAgreesWithForecast",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCombatOutcomeAgreesWithForecastTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratCombatOutcomeParity;

	strat::Save Fixture;
	FString Error;
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"), LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the fixture is long enough to reach the chosen attack"),
			static_cast<int32>(Fixture.commandLog.size()) > kAttackWithCounter))
	{
		return false;
	}

	const strat::SaveCommand& Attack = Fixture.commandLog[static_cast<size_t>(kAttackWithCounter)];
	if (!TestEqual(
			*FString::Printf(TEXT("fixture entry %d is still an Attack"), kAttackWithCounter),
			static_cast<int32>(Attack.kind), static_cast<int32>(strat::SaveCommandKind::Attack)))
	{
		AddError(TEXT("the fixture was re-emitted and its command order moved; re-pick the index "
		              "rather than submitting whatever now sits there"));
		return false;
	}

	FStratBridge Bridge;
	if (!TestTrue(TEXT("the bridge reaches the position the attack was made from"),
			SeedAndReplayPrefix(Bridge, Fixture, kAttackWithCounter, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- the oracle, on the pre-submit world --------------------------------
	const strat::UiWorld Before = Bridge.MakeUiWorld();
	const strat::UiResolution Oracle =
		strat::uiResolveForGate(Before, Attack.unitId, Attack.hex);

	if (!TestTrue(TEXT("uiResolveForGate applied, so there is an oracle to compare against"),
			Oracle.applied))
	{
		AddError(FString::Printf(TEXT("the oracle refused: %s"), *FromStd(Oracle.forecast.reason)));
		return false;
	}

	const strat::UiUnit* AttackerBefore = strat::findUiUnit(Before, Attack.unitId);
	const strat::UiUnit* DefenderBefore = FindUnitOnHex(Before, Attack.hex);
	if (!TestNotNull(TEXT("the attacker stands on the board before the submit"), AttackerBefore) ||
		!TestNotNull(TEXT("a defender stands on the target hex before the submit"), DefenderBefore))
	{
		return false;
	}

	// ---- the positive control, before anything is compared -------------------
	// Both halves must MOVE, or the equalities below are satisfiable by an attack that
	// did nothing at all.
	if (!TestTrue(TEXT("control: the forecast predicts a non-zero defender loss"),
			Oracle.forecast.damage > 0) ||
		!TestTrue(TEXT("control: and the defender survives it, so an 'after' HP exists to compare"),
			!Oracle.forecast.defenderDies) ||
		!TestTrue(TEXT("control: and the counter FIRES, so the attacker's HP must move too"),
			Oracle.forecast.counterFires && Oracle.forecast.counterDamage > 0))
	{
		AddError(FString::Printf(
			TEXT("fixture entry %d no longer forecasts a surviving defender and a firing counter "
			     "(damage %d, dies %d, counter %d/%d); re-pick the index -- a clause whose two "
			     "sides do not move cannot fail"),
			kAttackWithCounter, Oracle.forecast.damage, Oracle.forecast.defenderDies ? 1 : 0,
			Oracle.forecast.counterFires ? 1 : 0, Oracle.forecast.counterDamage));
		return false;
	}
	TestNotEqual(TEXT("control: the oracle's defender HP is not already the pre-submit one"),
		Oracle.defenderHpAfter, DefenderBefore->unit.hp);
	TestNotEqual(TEXT("control: nor the oracle's attacker HP"),
		Oracle.attackerHpAfter, AttackerBefore->unit.hp);

	// ---- submit, with the emitted line captured ------------------------------
	FString ResolvedLine;
	{
		FStratCombatCapture Capture;
		const FStratResult Submitted = Bridge.SubmitAttack(Attack.unitId, Attack.hex);
		if (!TestTrue(TEXT("the fixture's own attack is accepted at its own position"),
				Submitted.bOk))
		{
			AddError(Submitted.Reason);
			return false;
		}
		if (!TestEqual(TEXT("T-UI-01: one accepted attack emits exactly one STRAT-COMBAT resolved line"),
				Capture.CountWithPhrase(TEXT("STRAT-COMBAT resolved")), 1))
		{
			return false;
		}
		const FString* Line = Capture.SoleLineWithPhrase(TEXT("STRAT-COMBAT resolved"));
		if (!TestNotNull(TEXT("and that line is readable"), Line))
		{
			return false;
		}
		ResolvedLine = *Line;
	}
	AddInfo(ResolvedLine);

	// ---- the measured side, off the post-submit projection -------------------
	strat::UiSnapshot After;
	if (!TestTrue(TEXT("the post-submit snapshot projects"), Bridge.MakeUiSnapshot(After).bOk))
	{
		return false;
	}
	const strat::UiUnitView* AttackerAfter = strat::findUiUnitView(After, Attack.unitId);
	const strat::UiUnitView* DefenderAfter = strat::findUiUnitView(After, DefenderBefore->id);
	if (!TestNotNull(TEXT("the attacker is still on the roster, as forecast"), AttackerAfter) ||
		!TestNotNull(TEXT("the defender is still on the roster, as forecast"), DefenderAfter))
	{
		return false;
	}

	// THE CLAUSE. Left side measured, right side `uiResolveForGate`'s.
	TestEqual(TEXT("T-UI-01: the defender's measured HP is uiResolveForGate's defenderHpAfter"),
		DefenderAfter->hp, Oracle.defenderHpAfter);
	TestEqual(TEXT("T-UI-01: the attacker's measured HP is uiResolveForGate's attackerHpAfter"),
		AttackerAfter->hp, Oracle.attackerHpAfter);

	// ---- and the line the bridge emitted says the same thing -----------------
	// A third reading, from the emitter rather than from the snapshot. It is what a gate
	// greps, so it is checked against the oracle and not against the snapshot.
	int32 LineAttacker = INDEX_NONE;
	int32 LineDefender = INDEX_NONE;
	int32 LineAhpAfter = INDEX_NONE;
	int32 LineDhpAfter = INDEX_NONE;
	int32 LineFdmg     = INDEX_NONE;
	int32 LineFcdmg    = INDEX_NONE;
	int32 LineAgree    = INDEX_NONE;
	int32 LineDiverge  = INDEX_NONE;
	int32 LineAdied    = INDEX_NONE;
	int32 LineDdied    = INDEX_NONE;

	TestTrue(TEXT("the line carries an attacker= field"), ReadIntField(ResolvedLine, TEXT("attacker"), LineAttacker));
	TestTrue(TEXT("the line carries a defender= field"),  ReadIntField(ResolvedLine, TEXT("defender"), LineDefender));
	TestTrue(TEXT("the line carries an ahpAfter= field"), ReadIntField(ResolvedLine, TEXT("ahpAfter"), LineAhpAfter));
	TestTrue(TEXT("the line carries a dhpAfter= field"),  ReadIntField(ResolvedLine, TEXT("dhpAfter"), LineDhpAfter));
	TestTrue(TEXT("the line carries an fdmg= field"),     ReadIntField(ResolvedLine, TEXT("fdmg"), LineFdmg));
	TestTrue(TEXT("the line carries an fcdmg= field"),    ReadIntField(ResolvedLine, TEXT("fcdmg"), LineFcdmg));
	TestTrue(TEXT("the line carries an agree= field"),    ReadIntField(ResolvedLine, TEXT("agree"), LineAgree));
	TestTrue(TEXT("the line carries a diverge= field"),   ReadIntField(ResolvedLine, TEXT("diverge"), LineDiverge));
	TestTrue(TEXT("the line carries an adied= field"),    ReadIntField(ResolvedLine, TEXT("adied"), LineAdied));
	TestTrue(TEXT("the line carries a ddied= field"),     ReadIntField(ResolvedLine, TEXT("ddied"), LineDdied));

	TestEqual(TEXT("T-UI-01: the line names the unit that was submitted"), LineAttacker, Attack.unitId);
	TestEqual(TEXT("T-UI-01: and the unit that stood on the target hex"), LineDefender, DefenderBefore->id);
	TestEqual(TEXT("T-UI-01: the line's fdmg is uiForecast's damage, via the oracle"),
		LineFdmg, Oracle.forecast.damage);
	TestEqual(TEXT("T-UI-01: the line's fcdmg is uiForecast's counterDamage, via the oracle"),
		LineFcdmg, Oracle.forecast.counterDamage);
	TestEqual(TEXT("T-UI-01: the line's dhpAfter is uiResolveForGate's defenderHpAfter"),
		LineDhpAfter, Oracle.defenderHpAfter);
	TestEqual(TEXT("T-UI-01: the line's ahpAfter is uiResolveForGate's attackerHpAfter"),
		LineAhpAfter, Oracle.attackerHpAfter);
	TestEqual(TEXT("T-UI-01: neither combatant died, as the oracle predicted"), LineAdied, 0);
	TestEqual(TEXT("T-UI-01: nor the defender"), LineDdied, 0);
	TestEqual(TEXT("T-UI-01: the emitter reports agreement"), LineAgree, 1);
	TestEqual(TEXT("T-UI-01: with an empty divergence mask"), LineDiverge, 0);

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-01 -- a counter that kills the attacker takes it off the roster, and the
// record says so.
//
// THE HIGHEST-VALUE CLAUSE IN THIS FILE, because the arm it drives had never executed.
// Measured 2026-08-13: `adied=1` appeared 0 times in 483 resolutions in a full suite
// run, and all 74 of the fixture's own Attacks forecast a counter too small to kill.
// So `CaptureAfter`'s attacker-absent branch -- the one that sets `bAttackerDied` and
// leaves `AttackerHpAfter` at -1 -- was untested code that a refactor could have
// deleted with the suite staying green.
//
// THE POSITION IS THE FIXTURE'S; THE ATTACK IS NOT. There is no legal way to make a
// fixture Attack into a counter-kill, so this clause replays a fixture PREFIX and then
// searches the resulting board for a counter-kill the active side could legally make.
// The search asks `FStratBridge::Forecast` -- which is `strat::uiForecast` and nothing
// else -- and picks a candidate only when the module itself says the counter's damage
// meets the attacker's HP. This file chooses no damage number and no board.
//
// WHY IT TRIES CANDIDATES IN TURN. `uiForecast` does not model §4.9's turn flags: a
// unit that has already acted forecasts a perfectly legal attack and is then refused by
// `markActed`. A refusal changes nothing (§4.9, and T-SAVE-06 clause 2 gates it), so
// trying the next candidate is safe. If NO candidate is accepted the clause FAILS --
// it does not quietly pass having proved nothing, which is the failure mode a
// search-driven test invites.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCounterKillLeavesAttackerOffRosterTest,
	"Stratocracy.StratBridge.T-UI-01.CounterKillLeavesTheAttackerOffTheRoster",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCounterKillLeavesAttackerOffRosterTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratCombatOutcomeParity;

	strat::Save Fixture;
	FString Error;
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"), LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratBridge Bridge;
	if (!TestTrue(TEXT("the bridge reaches the fixture position the search runs from"),
			SeedAndReplayPrefix(Bridge, Fixture, kCounterKillPrefix, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- the search: whom does the MODULE say a counter would kill? ----------
	const strat::UiWorld Before = Bridge.MakeUiWorld();
	const int32 ActiveSide = Bridge.State().turn.activeSide;

	struct FCandidate
	{
		int32       AttackerId = INDEX_NONE;
		int32       DefenderId = INDEX_NONE;
		strat::Hex  TargetHex;
	};
	TArray<FCandidate> Candidates;

	for (const strat::UiUnit& A : Before.units)
	{
		if (A.side != ActiveSide)
		{
			continue;
		}
		for (const strat::UiUnit& D : Before.units)
		{
			if (D.side == ActiveSide)
			{
				continue;
			}
			strat::UiForecast F;
			if (!Bridge.Forecast(A.id, D.hex, F).bOk || !F.legal)
			{
				continue;
			}
			// THE MODULE'S OWN PREDICATE FOR "the counter kills the attacker", and it is
			// `applyCommand`'s: the defender survives, the counter fires, and the
			// counter's damage empties the attacker's pool
			// (Replay.good.cpp:468 -- `if (atkHp - counter <= 0)`).
			if (!F.defenderDies && F.counterFires && F.counterDamage >= A.unit.hp)
			{
				FCandidate C;
				C.AttackerId = A.id;
				C.DefenderId = D.id;
				C.TargetHex  = D.hex;
				Candidates.Add(C);
			}
		}
	}

	if (!TestTrue(
			*FString::Printf(
				TEXT("T-UI-01: the fixture position after %d commands offers at least one "
				     "counter-kill the module itself forecasts"), kCounterKillPrefix),
			Candidates.Num() > 0))
	{
		AddError(TEXT("no counter-kill is reachable from this prefix any more. The attacker-death "
		              "arm of CaptureAfter is then UNCOVERED -- re-pick the prefix against a fresh "
		              "probe rather than deleting this clause"));
		return false;
	}
	AddInfo(FString::Printf(TEXT("%d counter-kill candidates at prefix %d"),
		Candidates.Num(), kCounterKillPrefix));

	// ---- submit, taking the first the turn flags actually allow --------------
	FString ResolvedLine;
	FCandidate Chosen;
	strat::UiResolution Oracle;
	int32 AttackerHpBefore = INDEX_NONE;
	{
		FStratCombatCapture Capture;
		bool bAccepted = false;
		for (const FCandidate& C : Candidates)
		{
			// The oracle is read BEFORE the submit that might consume it, and re-read per
			// candidate because a refusal leaves the world where it was.
			const strat::UiWorld World = Bridge.MakeUiWorld();
			const strat::UiUnit* A = strat::findUiUnit(World, C.AttackerId);
			if (A == nullptr)
			{
				continue;
			}
			const strat::UiResolution R =
				strat::uiResolveForGate(World, C.AttackerId, C.TargetHex);
			if (!R.applied)
			{
				continue;
			}

			const FStratResult Submitted = Bridge.SubmitAttack(C.AttackerId, C.TargetHex);
			if (Submitted.bOk)
			{
				bAccepted = true;
				Chosen = C;
				Oracle = R;
				AttackerHpBefore = A->unit.hp;
				break;
			}
			AddInfo(FString::Printf(TEXT("candidate attacker %d refused: %s"),
				C.AttackerId, *Submitted.Reason));
		}

		if (!TestTrue(TEXT("T-UI-01: one of those counter-kills is accepted by applyCommand"),
				bAccepted))
		{
			AddError(TEXT("every forecast counter-kill at this prefix was refused -- most likely "
			              "each attacker had already acted. Re-pick the prefix; do not weaken the "
			              "clause"));
			return false;
		}

		if (!TestEqual(TEXT("exactly one attack resolved in the window"),
				Capture.CountWithPhrase(TEXT("STRAT-COMBAT resolved")), 1))
		{
			return false;
		}
		const FString* Line = Capture.SoleLineWithPhrase(TEXT("STRAT-COMBAT resolved"));
		if (!TestNotNull(TEXT("and that line is readable"), Line))
		{
			return false;
		}
		ResolvedLine = *Line;
	}
	AddInfo(ResolvedLine);

	// ---- the control: this was a KILL, and the oracle says so ----------------
	// `uiResolveForGate` clamps at zero, and zero HP is death in this rules module
	// (Replay.good.cpp reads `hp - damage <= 0`). An oracle attackerHpAfter of 0 is
	// therefore the module's own statement that the counter killed the attacker, and it
	// is what makes the roster assertion below a real one rather than a tautology.
	TestTrue(TEXT("control: the attacker had HP to lose"), AttackerHpBefore > 0);
	TestEqual(TEXT("control: uiResolveForGate empties the attacker's pool"),
		Oracle.attackerHpAfter, 0);
	TestTrue(TEXT("control: the counter fired"), Oracle.forecast.counterFires);

	// ---- THE CLAUSE ---------------------------------------------------------
	strat::UiSnapshot After;
	if (!TestTrue(TEXT("the post-submit snapshot projects"), Bridge.MakeUiSnapshot(After).bOk))
	{
		return false;
	}
	TestNull(TEXT("T-UI-01: a counter-killed attacker is no longer on the projected roster"),
		strat::findUiUnitView(After, Chosen.AttackerId));
	TestNotNull(TEXT("T-UI-01: and the defender that survived to counter still is"),
		strat::findUiUnitView(After, Chosen.DefenderId));

	int32 LineAttacker = INDEX_NONE;
	int32 LineAdied    = INDEX_NONE;
	int32 LineAhpAfter = INDEX_NONE;
	int32 LineDdied    = INDEX_NONE;
	int32 LineDhpAfter = INDEX_NONE;
	int32 LineAgree    = INDEX_NONE;
	TestTrue(TEXT("the line carries an attacker= field"), ReadIntField(ResolvedLine, TEXT("attacker"), LineAttacker));
	TestTrue(TEXT("the line carries an adied= field"),    ReadIntField(ResolvedLine, TEXT("adied"), LineAdied));
	TestTrue(TEXT("the line carries an ahpAfter= field"), ReadIntField(ResolvedLine, TEXT("ahpAfter"), LineAhpAfter));
	TestTrue(TEXT("the line carries a ddied= field"),     ReadIntField(ResolvedLine, TEXT("ddied"), LineDdied));
	TestTrue(TEXT("the line carries a dhpAfter= field"),  ReadIntField(ResolvedLine, TEXT("dhpAfter"), LineDhpAfter));
	TestTrue(TEXT("the line carries an agree= field"),    ReadIntField(ResolvedLine, TEXT("agree"), LineAgree));

	TestEqual(TEXT("T-UI-01: the line names the attacker that was submitted"),
		LineAttacker, Chosen.AttackerId);
	TestEqual(TEXT("T-UI-01: adied=1 -- the arm that had never executed before this clause"),
		LineAdied, 1);
	TestEqual(TEXT("T-UI-01: and a dead attacker has no 'after' HP, so ahpAfter is the -1 sentinel"),
		LineAhpAfter, INDEX_NONE);
	TestEqual(TEXT("T-UI-01: the defender lived, so ddied=0"), LineDdied, 0);
	TestEqual(TEXT("T-UI-01: and the defender's measured HP is uiResolveForGate's defenderHpAfter"),
		LineDhpAfter, Oracle.defenderHpAfter);
	TestEqual(TEXT("T-UI-01: the emitter still reports agreement on a counter-kill"), LineAgree, 1);

	return true;
}

// ---------------------------------------------------------------------------
// T-FAME-01 -- a kill moves the attacking side's fame, by the module's own award.
//
// §2.4 / Q5, through `Economy.h:108`: the award is half the victim's cost, or a flat
// 500 for a flag, and it lands on `fameTotal` AND `fameCombat`. THE EXPECTED NUMBER IS
// `strat::killAward`'s, handed the victim's own `UnitDef` off `FStratBridge::UnitDefs()`
// and the victim's own `isFlag` off the pre-submit `UiWorld`. Nothing here halves a
// cost; a test that did would agree with a broken `killAward` forever.
//
// THE CONTROL. `killAward` must return something non-zero, or "fame moved by the award"
// is satisfied by fame not moving at all. That is asserted before the comparison.
//
// WHY AN ATTACK AND NOT AN EndTurn. Passive income also moves `fameTotal` -- and
// deliberately never touches `fameCombat` (T-FAME-01, Economy.good.cpp:60). A single
// Attack command runs no income tick, so the whole of the observed movement is the
// award, and the `fameCombat` half is what tells the two sources apart.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratAttackerFameMovesOnAKillTest,
	"Stratocracy.StratBridge.T-FAME-01.AttackerFameMovesOnAKill",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratAttackerFameMovesOnAKillTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratCombatOutcomeParity;

	strat::Save Fixture;
	FString Error;
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"), LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the fixture is long enough to reach the chosen attack"),
			static_cast<int32>(Fixture.commandLog.size()) > kAttackWithKill))
	{
		return false;
	}

	const strat::SaveCommand& Attack = Fixture.commandLog[static_cast<size_t>(kAttackWithKill)];
	if (!TestEqual(*FString::Printf(TEXT("fixture entry %d is still an Attack"), kAttackWithKill),
			static_cast<int32>(Attack.kind), static_cast<int32>(strat::SaveCommandKind::Attack)))
	{
		AddError(TEXT("the fixture was re-emitted and its command order moved; re-pick the index"));
		return false;
	}

	FStratBridge Bridge;
	if (!TestTrue(TEXT("the bridge reaches the position the kill was made from"),
			SeedAndReplayPrefix(Bridge, Fixture, kAttackWithKill, Error)))
	{
		AddError(Error);
		return false;
	}

	const strat::UiWorld Before = Bridge.MakeUiWorld();
	const strat::UiResolution Oracle =
		strat::uiResolveForGate(Before, Attack.unitId, Attack.hex);
	if (!TestTrue(TEXT("the oracle applies, so the attack is legal here"), Oracle.applied))
	{
		AddError(FromStd(Oracle.forecast.reason));
		return false;
	}

	const strat::UiUnit* Attacker = strat::findUiUnit(Before, Attack.unitId);
	const strat::UiUnit* Victim   = FindUnitOnHex(Before, Attack.hex);
	if (!TestNotNull(TEXT("the attacker stands on the board"), Attacker) ||
		!TestNotNull(TEXT("a victim stands on the target hex"), Victim))
	{
		return false;
	}

	// ---- the control: this fixture entry still KILLS -------------------------
	if (!TestTrue(TEXT("control: the forecast says the defender dies, so an award is due"),
			Oracle.forecast.defenderDies))
	{
		AddError(FString::Printf(
			TEXT("fixture entry %d no longer forecasts a kill (damage %d against %d HP); re-pick "
			     "the index -- a fame clause on a survivor cannot fail"),
			kAttackWithKill, Oracle.forecast.damage, Victim->unit.hp));
		return false;
	}

	// ---- the expected number, from the module -------------------------------
	const std::vector<strat::UnitDef>& Defs = Bridge.UnitDefs();
	if (!TestTrue(TEXT("the victim's defIndex is inside the loaded definitions"),
			Victim->defIndex >= 0 && Victim->defIndex < static_cast<int32>(Defs.size())))
	{
		return false;
	}
	const int32 Award =
		strat::killAward(Defs[static_cast<size_t>(Victim->defIndex)], Victim->isFlag);
	if (!TestTrue(TEXT("control: strat::killAward returns a non-zero award, so fame HAS to move"),
			Award > 0))
	{
		return false;
	}
	AddInfo(FString::Printf(TEXT("strat::killAward for defIndex %d (isFlag %d) = %d"),
		Victim->defIndex, Victim->isFlag ? 1 : 0, Award));

	// ---- submit --------------------------------------------------------------
	FString ResolvedLine;
	{
		FStratCombatCapture Capture;
		const FStratResult Submitted = Bridge.SubmitAttack(Attack.unitId, Attack.hex);
		if (!TestTrue(TEXT("the fixture's own killing attack is accepted"), Submitted.bOk))
		{
			AddError(Submitted.Reason);
			return false;
		}
		if (!TestEqual(TEXT("exactly one attack resolved in the window"),
				Capture.CountWithPhrase(TEXT("STRAT-COMBAT resolved")), 1))
		{
			return false;
		}
		const FString* Line = Capture.SoleLineWithPhrase(TEXT("STRAT-COMBAT resolved"));
		if (!TestNotNull(TEXT("and that line is readable"), Line))
		{
			return false;
		}
		ResolvedLine = *Line;
	}
	AddInfo(ResolvedLine);

	// ---- THE CLAUSE, read off the emitted record -----------------------------
	// The four fame fields are `strat::UiSideView` mirrors either side of the submit --
	// StratCombatLog.h keeps them as two values rather than a delta precisely so the
	// record carries no arithmetic. The one subtraction is here, in the gate, and its
	// right-hand side is `strat::killAward`.
	int32 LineDdied            = INDEX_NONE;
	int32 LineDhpAfter         = INDEX_NONE;
	int32 LineFameBefore       = INDEX_NONE;
	int32 LineFameAfter        = INDEX_NONE;
	int32 LineFameCombatBefore = INDEX_NONE;
	int32 LineFameCombatAfter  = INDEX_NONE;
	int32 LineAttackerSide     = INDEX_NONE;
	int32 LineAgree            = INDEX_NONE;

	TestTrue(TEXT("the line carries a ddied= field"),            ReadIntField(ResolvedLine, TEXT("ddied"), LineDdied));
	TestTrue(TEXT("the line carries a dhpAfter= field"),         ReadIntField(ResolvedLine, TEXT("dhpAfter"), LineDhpAfter));
	TestTrue(TEXT("the line carries a fameBefore= field"),       ReadIntField(ResolvedLine, TEXT("fameBefore"), LineFameBefore));
	TestTrue(TEXT("the line carries a fameAfter= field"),        ReadIntField(ResolvedLine, TEXT("fameAfter"), LineFameAfter));
	TestTrue(TEXT("the line carries a fameCombatBefore= field"), ReadIntField(ResolvedLine, TEXT("fameCombatBefore"), LineFameCombatBefore));
	TestTrue(TEXT("the line carries a fameCombatAfter= field"),  ReadIntField(ResolvedLine, TEXT("fameCombatAfter"), LineFameCombatAfter));
	TestTrue(TEXT("the line carries an attackerSide= field"),    ReadIntField(ResolvedLine, TEXT("attackerSide"), LineAttackerSide));
	TestTrue(TEXT("the line carries an agree= field"),           ReadIntField(ResolvedLine, TEXT("agree"), LineAgree));

	TestEqual(TEXT("T-FAME-01: the record says the defender died"), LineDdied, 1);
	TestEqual(TEXT("T-FAME-01: and a dead defender has no 'after' HP, so dhpAfter is the -1 sentinel"),
		LineDhpAfter, INDEX_NONE);
	TestEqual(TEXT("T-FAME-01: the fame that moved is the ATTACKER's side"),
		LineAttackerSide, Attacker->side);
	TestEqual(TEXT("T-FAME-01: fameTotal moved by exactly strat::killAward's amount"),
		LineFameAfter - LineFameBefore, Award);
	TestEqual(TEXT("T-FAME-01: and fameCombat moved by the same amount -- a kill is not income"),
		LineFameCombatAfter - LineFameCombatBefore, Award);
	TestEqual(TEXT("T-UI-01: the emitter still reports agreement on a kill"), LineAgree, 1);

	// The same numbers off the post-submit projection, so the clause does not rest on
	// the emitter alone. `UiSideView` is the screen's source; the record mirrors it.
	strat::UiSnapshot After;
	if (TestTrue(TEXT("the post-submit snapshot projects"), Bridge.MakeUiSnapshot(After).bOk) &&
		TestTrue(TEXT("the attacker's side indexes the snapshot's side array"),
			Attacker->side >= 0 && Attacker->side < strat::SIDE_COUNT))
	{
		TestNull(TEXT("T-FAME-01: the victim is off the projected roster"),
			strat::findUiUnitView(After, Victim->id));
		TestEqual(TEXT("T-FAME-01: and the projection's fameTotal is the record's fameAfter"),
			After.side[Attacker->side].fameTotal, LineFameAfter);
		TestEqual(TEXT("T-FAME-01: and its fameCombat is the record's fameCombatAfter"),
			After.side[Attacker->side].fameCombat, LineFameCombatAfter);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-06 -- a RAW `strat::SaveCommand` of kind Attack, handed to `Submit`, is
// observed like any other.
//
// WHAT THIS CLAUSE DEFENDS, and it is a placement rather than a behaviour.
// StratBridge.h states that the emitter sits in `Submit` and not in `SubmitAttack`
// exactly so that a raw Attack cannot escape observation, and it admits that no raw
// Attack in the tree today APPLIES -- both existing raw-`Submit` sites
// (StratBridgeParity.cpp:340, StratBridgeSaveRecording.cpp:466) submit a deliberately
// illegal command and can only ever prove the refusal phrase. So the guarantee the
// placement exists to give was, until this clause, untested in the direction that
// matters: nothing would have failed if the emitter migrated onto `SubmitAttack`.
//
// THE COMMAND IS THE FIXTURE'S, TAG AND ALL. It is submitted at the position it was
// recorded from, so its `{turn, side}` stamp is already correct and this file supplies
// no field of it -- which also means the clause exercises the untyped path with a
// command no typed method touched.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRawAttackThroughSubmitIsObservedTest,
	"Stratocracy.StratBridge.T-SAVE-06.RawAttackThroughSubmitIsObserved",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRawAttackThroughSubmitIsObservedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratCombatOutcomeParity;

	strat::Save Fixture;
	FString Error;
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"), LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the fixture is long enough to reach the chosen attack"),
			static_cast<int32>(Fixture.commandLog.size()) > kAttackWithCounter))
	{
		return false;
	}

	const strat::SaveCommand Raw = Fixture.commandLog[static_cast<size_t>(kAttackWithCounter)];
	if (!TestEqual(*FString::Printf(TEXT("fixture entry %d is still an Attack"), kAttackWithCounter),
			static_cast<int32>(Raw.kind), static_cast<int32>(strat::SaveCommandKind::Attack)))
	{
		AddError(TEXT("the fixture was re-emitted and its command order moved; re-pick the index"));
		return false;
	}

	FStratBridge Bridge;
	if (!TestTrue(TEXT("the bridge reaches the position the attack was made from"),
			SeedAndReplayPrefix(Bridge, Fixture, kAttackWithCounter, Error)))
	{
		AddError(Error);
		return false;
	}

	// The tag must already match the live turn, or this clause would be testing the
	// refusal path again rather than the applying one.
	TestEqual(TEXT("the fixture's own tag matches the live turn at this position"),
		Raw.turn, Bridge.State().turn.turnNumber);
	TestEqual(TEXT("and the live active side"),
		Raw.side, Bridge.State().turn.activeSide);

	const int32 RecordedBefore = static_cast<int32>(Bridge.RecordedLog().size());

	FString ResolvedLine;
	{
		FStratCombatCapture Capture;

		// THE UNTYPED ENTRY POINT. No `SubmitAttack`, no `SubmitAttackAtHex`, no
		// `SubmitStamped` -- this is the path a loaded log or a future tool would take.
		const FStratResult Submitted = Bridge.Submit(Raw);
		if (!TestTrue(TEXT("T-SAVE-06: a raw Attack that is legal here APPLIES through Submit"),
				Submitted.bOk))
		{
			AddError(Submitted.Reason);
			return false;
		}

		if (!TestEqual(
				TEXT("T-SAVE-06: and is observed -- one STRAT-COMBAT resolved line, from the raw path"),
				Capture.CountWithPhrase(TEXT("STRAT-COMBAT resolved")), 1))
		{
			AddError(TEXT("a raw Attack applied without emitting a resolved line. The emitter has "
			              "moved off FStratBridge::Submit onto a typed method, and a raw Attack now "
			              "escapes observation -- which is the exact failure its placement in "
			              "Submit exists to make unreachable"));
			return false;
		}
		TestEqual(TEXT("T-SAVE-06: and no refusal phrase beside it"),
			Capture.CountWithPhrase(TEXT("STRAT-COMBAT refused")), 0);

		const FString* Line = Capture.SoleLineWithPhrase(TEXT("STRAT-COMBAT resolved"));
		if (!TestNotNull(TEXT("and that line is readable"), Line))
		{
			return false;
		}
		ResolvedLine = *Line;
	}
	AddInfo(ResolvedLine);

	TestEqual(TEXT("T-SAVE-06: the raw command was recorded, as any accepted command is"),
		static_cast<int32>(Bridge.RecordedLog().size()), RecordedBefore + 1);

	// The line describes THIS command and not some other -- every identity field is the
	// fixture's own, so nothing here was authored.
	int32 LineAttacker = INDEX_NONE;
	int32 LineTurn     = INDEX_NONE;
	int32 LineSide     = INDEX_NONE;
	int32 LineAgree    = INDEX_NONE;
	TestTrue(TEXT("the line carries an attacker= field"), ReadIntField(ResolvedLine, TEXT("attacker"), LineAttacker));
	TestTrue(TEXT("the line carries a turn= field"),      ReadIntField(ResolvedLine, TEXT("turn"), LineTurn));
	TestTrue(TEXT("the line carries a side= field"),      ReadIntField(ResolvedLine, TEXT("side"), LineSide));
	TestTrue(TEXT("the line carries an agree= field"),    ReadIntField(ResolvedLine, TEXT("agree"), LineAgree));

	TestEqual(TEXT("T-SAVE-06: the line names the raw command's unit"), LineAttacker, Raw.unitId);
	TestEqual(TEXT("T-SAVE-06: and the fixture's own turn tag"), LineTurn, Raw.turn);
	TestEqual(TEXT("T-SAVE-06: and the fixture's own side tag"), LineSide, Raw.side);
	TestEqual(TEXT("T-SAVE-06: and the comparison ran"), LineAgree, 1);

	// The hex is two numbers in one token (`hex=q,r`), so it is compared as text against
	// the fixture's own coordinates rather than parsed into an int.
	TestTrue(TEXT("T-SAVE-06: and carries the fixture's own target hex"),
		ResolvedLine.Contains(
			*FString::Printf(TEXT("hex=%d,%d"), Raw.hex.q, Raw.hex.r), ESearchCase::CaseSensitive));

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-01 -- a refused attack carries the refusal phrase and never the resolution one.
//
// WHY THE PHRASES MATTER. `grep -c "STRAT-COMBAT resolved"` is meant to count attacks
// that ACTUALLY APPLIED. StratBridge.cpp's emitter block says the refusal was given a
// distinct phrase for exactly that reason, on the same principle that `STRAT-CMD
// refused` does not contain `accepted`. A refusal that shared the word would inflate
// every such count silently, and the symptom would be a gate that reports more combat
// than the match contained.
//
// THE POSITIVE CONTROL COMES FIRST AND IS NOT OPTIONAL. This clause reads an ABSENCE --
// "no `resolved` in the refused line" -- and an absence proves nothing until the
// instrument has been shown able to speak. So a legal attack is submitted into the SAME
// capture first; its `resolved` line is asserted present before the refusal is read.
// Without that, a capture that was silently seeing nothing would pass this clause.
//
// THE `STRAT-CMD` HALF IS WEAKER THAN IT LOOKS, AND SAYING SO IS THE POINT. That token
// is emitted from `StratPlay`'s selection machine, and no `StratPlay` code runs in a
// `StratBridge` test -- so its absence here is close to structural. It is asserted
// anyway, because the capture keeps every `STRAT-` line and the assertion costs
// nothing, but a reader should not mistake it for evidence about the gameplay module.
// The clause's teeth are in the resolved/refused separation above it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRefusedAttackCarriesDistinctPhraseTest,
	"Stratocracy.StratBridge.T-UI-01.RefusedAttackCarriesTheDistinctPhrase",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRefusedAttackCarriesDistinctPhraseTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratCombatOutcomeParity;

	strat::Save Fixture;
	FString Error;
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"), LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the fixture is long enough to reach the chosen attack"),
			static_cast<int32>(Fixture.commandLog.size()) > kAttackWithCounter))
	{
		return false;
	}

	const strat::SaveCommand& Attack = Fixture.commandLog[static_cast<size_t>(kAttackWithCounter)];
	if (!TestEqual(*FString::Printf(TEXT("fixture entry %d is still an Attack"), kAttackWithCounter),
			static_cast<int32>(Attack.kind), static_cast<int32>(strat::SaveCommandKind::Attack)))
	{
		AddError(TEXT("the fixture was re-emitted and its command order moved; re-pick the index"));
		return false;
	}

	FStratBridge Bridge;
	if (!TestTrue(TEXT("the bridge reaches the position the attack was made from"),
			SeedAndReplayPrefix(Bridge, Fixture, kAttackWithCounter, Error)))
	{
		AddError(Error);
		return false;
	}

	FString RefusedLine;
	FString RefusalReason;
	{
		FStratCombatCapture Capture;

		// ---- the positive control: the instrument speaks ---------------------
		const FStratResult Accepted = Bridge.SubmitAttack(Attack.unitId, Attack.hex);
		if (!TestTrue(TEXT("control: a legal attack is accepted"), Accepted.bOk))
		{
			AddError(Accepted.Reason);
			return false;
		}
		if (!TestEqual(TEXT("control: and the capture SEES its resolved line"),
				Capture.CountWithPhrase(TEXT("STRAT-COMBAT resolved")), 1))
		{
			AddError(TEXT("the capture saw no resolved line for an accepted attack, so the absence "
			              "asserted below would prove nothing"));
			return false;
		}

		// ---- the refusal -----------------------------------------------------
		// The SAME unit attacking the SAME hex again. §4.9's `markActed` refuses it: a
		// unit acts once per turn. Chosen over an out-of-range hex because it needs no
		// second coordinate from this file, and over an unknown unit id because
		// StratBridgeSaveRecording.cpp:460 already drives that one.
		const FStratResult Refused = Bridge.SubmitAttack(Attack.unitId, Attack.hex);
		if (!TestFalse(TEXT("T-UI-01: a unit that has already acted is refused a second attack"),
				Refused.bOk))
		{
			AddError(TEXT("the second attack was ACCEPTED -- §4.9's once-per-turn action rule is "
			              "not holding, which is a rules-layer finding and not a logging one"));
			return false;
		}
		RefusalReason = Refused.Reason;

		if (!TestEqual(TEXT("T-UI-01: the refusal emits exactly one STRAT-COMBAT refused line"),
				Capture.CountWithPhrase(TEXT("STRAT-COMBAT refused")), 1))
		{
			return false;
		}
		if (!TestEqual(
				TEXT("T-UI-01: and does NOT add a second resolved line -- the refusal is not counted "
				     "as a resolution"),
				Capture.CountWithPhrase(TEXT("STRAT-COMBAT resolved")), 1))
		{
			return false;
		}

		// The gameplay module's token, asserted absent. See the header block for why this
		// half is weaker than it looks.
		TestEqual(TEXT("T-UI-01: and no STRAT-CMD line appears beside them"),
			Capture.CountWithPhrase(TEXT("STRAT-CMD")), 0);

		const FString* Line = Capture.SoleLineWithPhrase(TEXT("STRAT-COMBAT refused"));
		if (!TestNotNull(TEXT("the refused line is readable"), Line))
		{
			return false;
		}
		RefusedLine = *Line;
	}
	AddInfo(RefusedLine);
	AddInfo(FString::Printf(TEXT("refusal reason: %s"), *RefusalReason));

	// ---- THE CLAUSE ---------------------------------------------------------
	// CASE-SENSITIVE, because `FString::Contains` defaults to IgnoreCase in UE 5.8 and a
	// case-insensitive phrase test on this project has already produced a clause that
	// could not fail.
	TestFalse(TEXT("T-UI-01: the refused line does not contain the word 'resolved' anywhere"),
		RefusedLine.Contains(TEXT("resolved"), ESearchCase::CaseSensitive));
	TestFalse(TEXT("T-UI-01: nor the STRAT-CMD family's token"),
		RefusedLine.Contains(TEXT("STRAT-CMD"), ESearchCase::CaseSensitive));
	TestTrue(TEXT("T-UI-01: it carries a reason= field"),
		RefusedLine.Contains(TEXT("reason="), ESearchCase::CaseSensitive));
	TestTrue(TEXT("T-UI-01: and the reason is the one the rules module gave"),
		RefusedLine.Contains(*FString::Printf(TEXT("reason=%s"), *RefusalReason),
			ESearchCase::CaseSensitive));

	// A refused line carries the COMMON fields and stops -- no HP, no fame, no agreement.
	// That is the shape StratCombatLog.h's sentinel discipline promises, and a gate that
	// split a refused line on the resolved line's field count would otherwise read
	// garbage.
	int32 LineAttacker = INDEX_NONE;
	TestTrue(TEXT("T-UI-01: the refused line still names the attacker"),
		ReadIntField(RefusedLine, TEXT("attacker"), LineAttacker));
	TestEqual(TEXT("T-UI-01: and it is the unit that was refused"), LineAttacker, Attack.unitId);

	int32 Unused = INDEX_NONE;
	TestFalse(TEXT("T-UI-01: a refused line carries no agree= field -- nothing was measured"),
		ReadIntField(RefusedLine, TEXT("agree"), Unused));
	TestFalse(TEXT("T-UI-01: nor an ahpAfter= field"),
		ReadIntField(RefusedLine, TEXT("ahpAfter"), Unused));
	TestFalse(TEXT("T-UI-01: nor a fameAfter= field"),
		ReadIntField(RefusedLine, TEXT("fameAfter"), Unused));

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-01 -- an attack submitted before a scenario is observed by NOTHING, and
// `resolved + refused` is therefore not the count of attacks submitted.
//
// THIS IS A DOCUMENTED HOLE BEING PINNED AS A HOLE, not a defect being reported.
// `FStratBridge::Submit`'s `!bSeeded` guard returns ABOVE the `bIsAttack` capture, so
// an Attack that arrives before a scenario does reaches neither emitter -- no
// `resolved`, no `refused`, no line of any kind. StratBridge.h argues that is the right
// answer (there is no state to project, and a line of all -1 would say less than no
// line) and then states the consequence plainly: a 1:1 pairing gate that assumed those
// two counts reconciled with attacks submitted would be SILENTLY off by however many
// commands arrived before a scenario did.
//
// A CONSEQUENCE THAT IS ONLY WRITTEN DOWN IS NOT PINNED. Moving that capture above the
// guard -- a plausible tidy-up, and one that would look like a strict improvement in a
// diff -- changes the arithmetic every such gate depends on, and nothing in the tree
// would have noticed. This clause is what notices. If it ever fails, the fix is not to
// delete it: it is to decide deliberately that the counts now DO reconcile, and to
// correct StratBridge.h's block in the same commit.
//
// THE CONTROL IS DOUBLE, because the absence read here is an absence of BOTH phrases.
// A capture that had only ever been shown to see `resolved` would leave open that it
// simply cannot see `refused`, and the unseeded submit refuses. So the same window is
// first made to carry one of each, off a seeded bridge, before the unseeded bridge's
// silence is read.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratUnseededAttackIsObservedByNothingTest,
	"Stratocracy.StratBridge.T-UI-01.UnseededAttackIsObservedByNothing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratUnseededAttackIsObservedByNothingTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratCombatOutcomeParity;

	strat::Save Fixture;
	FString Error;
	if (!TestTrue(TEXT("the committed parity fixture loads and parses"), LoadFixture(Fixture, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the fixture is long enough to reach the chosen attack"),
			static_cast<int32>(Fixture.commandLog.size()) > kAttackWithCounter))
	{
		return false;
	}

	const strat::SaveCommand Attack = Fixture.commandLog[static_cast<size_t>(kAttackWithCounter)];
	if (!TestEqual(*FString::Printf(TEXT("fixture entry %d is still an Attack"), kAttackWithCounter),
			static_cast<int32>(Attack.kind), static_cast<int32>(strat::SaveCommandKind::Attack)))
	{
		AddError(TEXT("the fixture was re-emitted and its command order moved; re-pick the index"));
		return false;
	}

	FStratBridge Seeded;
	if (!TestTrue(TEXT("a seeded bridge reaches the attack's own position"),
			SeedAndReplayPrefix(Seeded, Fixture, kAttackWithCounter, Error)))
	{
		AddError(Error);
		return false;
	}

	// An UNSEEDED bridge: default-constructed, no tables, no scenario. Never seeded at
	// any point, so `bSeeded` is false for the reason the guard exists rather than
	// because this file arranged it.
	FStratBridge Unseeded;
	if (!TestFalse(TEXT("the second bridge is not seeded"), Unseeded.IsSeeded()))
	{
		return false;
	}

	{
		FStratCombatCapture Capture;

		// ---- control 1: the window can carry a `resolved` line ---------------
		const FStratResult Accepted = Seeded.SubmitAttack(Attack.unitId, Attack.hex);
		if (!TestTrue(TEXT("control: the seeded bridge accepts a legal attack"), Accepted.bOk))
		{
			AddError(Accepted.Reason);
			return false;
		}
		if (!TestEqual(TEXT("control: and the capture sees its resolved line"),
				Capture.CountWithPhrase(TEXT("STRAT-COMBAT resolved")), 1))
		{
			return false;
		}

		// ---- control 2: and a `refused` one ----------------------------------
		const FStratResult SeededRefusal = Seeded.SubmitAttack(Attack.unitId, Attack.hex);
		if (!TestFalse(TEXT("control: a second attack by the same unit is refused"),
				SeededRefusal.bOk))
		{
			return false;
		}
		if (!TestEqual(TEXT("control: and the capture sees its refused line too"),
				Capture.CountWithPhrase(TEXT("STRAT-COMBAT refused")), 1))
		{
			AddError(TEXT("the capture cannot see a refused line, so the silence asserted below "
			              "would prove nothing about the refused phrase"));
			return false;
		}

		// ---- THE CLAUSE: the unseeded submit is observed by nothing -----------
		const FStratResult Unobserved = Unseeded.Submit(Attack);
		TestFalse(TEXT("T-UI-01: an unseeded bridge refuses the attack"), Unobserved.bOk);
		TestEqual(TEXT("T-UI-01: with the guard's own sentence"),
			Unobserved.Reason, FString(TEXT("no scenario is loaded")));

		TestEqual(
			TEXT("T-UI-01: and emits NO resolved line -- the !bSeeded guard returns above the "
			     "capture, so `resolved` is not a count of attacks submitted"),
			Capture.CountWithPhrase(TEXT("STRAT-COMBAT resolved")), 1);
		TestEqual(
			TEXT("T-UI-01: and no refused line either -- nor is `resolved + refused`"),
			Capture.CountWithPhrase(TEXT("STRAT-COMBAT refused")), 1);
		TestEqual(TEXT("T-UI-01: no STRAT-COMBAT line of any phrase, in fact"),
			Capture.CountWithPhrase(TEXT("STRAT-COMBAT")), 2);
	}

	return true;
}
