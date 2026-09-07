// The gate on §2.11.6's UNDO -- the New Match route putting a player back at the beginning --
// under T-SAVE-06, GATE-TITLEMENU and T-UI-03.
//
// WHAT DEFECT THIS FILE EXISTS AGAINST, in the engineer's own measurement rather than restated
// as a hazard. `UStratMatchSubsystem::RecordMatchCompletionOnSave` was the ONLY writer of
// `UStratSaveGame::bHasCompletedAMatch` and nothing ever set it back, so on any machine that
// had finished one match §2.11.6's guided opening was over forever. The four parts that close
// it are `ClearMatchCompletionOnSave`, `RouteStartsAFreshMatch` /
// `CompletionClearSlotForRoute`, the transient `bPendingForcedGuidance` triple, and
// `AStratPlayerController::TryArmGuidedOpening`'s `!bForced && ...`.
//
// THE HAZARD THIS FILE IS SHAPED AROUND, AND IT IS NOT A STYLE PREFERENCE. Every clause below
// is about a bool that suppresses onboarding, read off a SAVE SLOT. The shipped slot is
// `StratocracyMatch`, the player's own; a clause that wrote there would clear or set a
// developer's onboarding state on every suite run, and one that DELETED there would destroy an
// in-progress game. So every clause below names its own slot, every fixture config sets
// `FStratMatchConfig::SaveSlotName` to one of them, and `FNamedSlotScope` deletes it on both
// ends. `StratocracyMatch` appears nowhere in this file, not even in a comparison.
//
// AND `bHasCompletedAMatch == false` IS ALSO WHAT AN ABSENT SLOT ANSWERS, which is why no
// clause here asserts "false after the clear" on its own. Each one either establishes the TRUE
// first -- through the module's own writer wherever a writer exists -- or asserts something an
// absent slot cannot satisfy, such as "the slot is still absent and no payload was authored".
// A clause that only measured the default would pass against a `ClearMatchCompletionOnSave`
// that had been deleted.
//
// WHERE THE EXPECTATIONS COME FROM.
//   - The §4.10 text a clause requires preserved is the text `SaveMatchToSlot` itself produced,
//     read off the slot before the clear and compared to the same slot after. No text, hash or
//     length is spelled here.
//   - "Which route starts fresh" is `UStratShellSubsystem::RouteStartsAFreshMatch`, and the set
//     of routes to ask it about is `StaticEnum<EStratShellRoute>()` -- neither is typed out.
//     `EStratShellRoute::NewMatch` is named in exactly one assertion, as the SPECIFICATION of
//     which route that is, and the sweep around it is what makes that one name checkable.
//   - The version a payload must be left at is the version read back off that payload before
//     the call, never `kCurrentSavedDataVersion` and never a literal. The module constant
//     appears only in the CONTROL that the planted version differs from it.
//   - Whether a completed match is on the slot is `UStratMatchSubsystem::HasCompletedAMatchOnSave`,
//     the same reader `TryArmGuidedOpening` asks.
//
// EVERY STRING COMPARISON IS CASE SENSITIVE. `FString::operator==`, `Contains` and
// `TestEqual(FString)` all ignore case in UE 5.8. The fixture slot names and configured-slot
// strings below are deliberately mixed-case so that a body which lower-cased or rebuilt what it
// was handed cannot read as a faithful passthrough.
//
// NO `strat::` FREE FUNCTION IS CALLED -- LNK2019 outside StratBridge and StratRules, measured
// 8 x. `FStratBridge` is an exported class; this file only asks the subsystem whether one is
// seeded.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/Class.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
#include "StratPlayerController.h"
#include "StratSaveGame.h"
#include "StratScoreboardHUD.h"
#include "StratShellSubsystem.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

// Included from a .cpp and never from a UHT-parsed header. Only `FStratBridge::IsSeeded` is
// called, which is inline in that header; no `strat::` free function is reached from here.
#include "StratBridge.h"

namespace StratNewMatchForcedGuidance
{
	static const int32 kUserIndex = 0;
	static const int32 kFirstSide = 0;

	/** One slot per clause, mixed case, and never the shipped `StratocracyMatch`. */
	static const TCHAR* kPreserveSlot = TEXT("StratocracyAutomation_ClearPreserve_MiXeD_4c1a");
	static const TCHAR* kAbsentSlot   = TEXT("StratocracyAutomation_ClearAbsent_MiXeD_4c1b");
	static const TCHAR* kVersionSlot  = TEXT("StratocracyAutomation_ClearVersion_MiXeD_4c1c");
	static const TCHAR* kForcedSlot   = TEXT("StratocracyAutomation_ForcedGuidance_MiXeD_4c1d");
	static const TCHAR* kControlSlot  = TEXT("StratocracyAutomation_UnforcedGuidance_MiXeD_4c1e");
	static const TCHAR* kSeedGuardSlot = TEXT("StratocracyAutomation_SeedGuard_MiXeD_4c1f");

	/** Two configured-slot strings for the passthrough clause. See its block: one string can be
	 *  matched by a constant; two different arguments each returned verbatim cannot. */
	static const TCHAR* kConfiguredA = TEXT("StratShellFixture_ClearSlot_AaA_7d20");
	static const TCHAR* kConfiguredB = TEXT("StratShellFixture_ClearSlot_bBb_7d21");

	/** See `StratSaveSlotClauses.cpp` on why these are supplied and never read off a manifest. */
	static const TCHAR* kRulesCommit = TEXT("automation-rules-commit");
	static const TCHAR* kDataHash    = TEXT("automation-data-hash");

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/**
	 * The shipped scenario, pointed at a slot this file owns.
	 *
	 * A `/Game/` LITERAL IN AN AUTOMATION FIXTURE IS THE PROJECT RULE'S OWN STANDING EXCEPTION.
	 */
	static bool MakeConfig(FStratMatchConfig& Out, const TCHAR* SlotName, FString& OutError)
	{
		Out.UnitTable    = LoadTable(TEXT("/Game/StratData/DT_Units.DT_Units"));
		Out.TerrainTable = LoadTable(TEXT("/Game/StratData/DT_Terrain.DT_Terrain"));
		if (Out.UnitTable == nullptr || Out.TerrainTable == nullptr)
		{
			OutError = TEXT("DT_Units and/or DT_Terrain did not load from /Game/StratData");
			return false;
		}

		Out.ScenarioFile    = TEXT("Data/ferrum_crossing.json");
		Out.FirstSide       = kFirstSide;
		Out.ViewingSide     = 0;
		Out.BoardActorClass = AStratBoardActor::StaticClass();
		Out.UnitActorClass  = AStratUnitActor::StaticClass();
		Out.RulesCommit     = kRulesCommit;
		Out.DataHash        = kDataHash;
		Out.SaveSlotName    = SlotName;
		return true;
	}

	/**
	 * Deletes a named slot on construction and again on destruction.
	 *
	 * BOTH ENDS, for `StratSaveSlotClauses.cpp`'s reason: a clause that failed halfway through
	 * last run must not poison the next one, and a developer's `Saved/SaveGames/` must not be
	 * littered by a suite run.
	 */
	struct FNamedSlotScope
	{
		const TCHAR* SlotName = nullptr;

		explicit FNamedSlotScope(const TCHAR* InSlotName)
			: SlotName(InSlotName) { Clear(SlotName); }
		~FNamedSlotScope() { Clear(SlotName); }

		static void Clear(const TCHAR* InSlotName)
		{
			if (UGameplayStatics::DoesSaveGameExist(InSlotName, kUserIndex))
			{
				UGameplayStatics::DeleteGameInSlot(InSlotName, kUserIndex);
			}
		}

		FNamedSlotScope(const FNamedSlotScope&) = delete;
		FNamedSlotScope& operator=(const FNamedSlotScope&) = delete;
	};

	/** See `StratMatchReconcile.cpp` on why the context is not decoration and why the world is
	 *  destroyed before its context. */
	struct FTestWorldScope
	{
		UWorld* World = nullptr;

		FTestWorldScope()
		{
			World = UWorld::CreateWorld(EWorldType::Game, /*bInformEngineOfWorld*/ false);
			if (World != nullptr && GEngine != nullptr)
			{
				FWorldContext& Context = GEngine->CreateNewWorldContext(EWorldType::Game);
				Context.SetCurrentWorld(World);
			}
		}

		~FTestWorldScope()
		{
			if (World != nullptr)
			{
				World->DestroyWorld(false);
				if (GEngine != nullptr)
				{
					GEngine->DestroyWorldContext(World);
				}
				World = nullptr;
			}
		}

		FTestWorldScope(const FTestWorldScope&) = delete;
		FTestWorldScope& operator=(const FTestWorldScope&) = delete;
	};

	/**
	 * A Game world with a live game instance -- so `UStratShellSubsystem` exists -- this
	 * project's own player controller, and a scoreboard HUD attached to it.
	 *
	 * THE RECIPE IS `StratCommandBarOptionsBindClauses.cpp`'s AND THE THREE LINES IT CALLS
	 * MEASURED ARE KEPT. `InitializeStandalone` is what stands the subsystem collection up (a
	 * bare `NewObject<UGameInstance>` has none, so `GetSubsystem<UStratShellSubsystem>()` would
	 * return null and every clause below would observe an unreached shell);
	 * `SetGameInstance` is what makes `AActor::GetGameInstance()` find it from the controller,
	 * which is the exact hop `TryArmGuidedOpening` takes; and `InitializeActorsForPlay` is what
	 * puts a spawned controller into the world's `PlayerControllerList`.
	 *
	 * THE CONTROLLER'S `BeginPlay` DOES NOT RUN, and that is required rather than tolerated.
	 * `InitializeActorsForPlay` initialises actors without beginning play, so nothing arms the
	 * guided opening behind the clause's back and every arming below happens on a
	 * `DecorateForPresentation` call this file made.
	 */
	struct FShellWorldScope
	{
		FTestWorldScope                 Scope;
		TStrongObjectPtr<UGameInstance> Instance;
		AStratPlayerController*         Controller = nullptr;
		AStratScoreboardHUD*            Hud        = nullptr;
		UStratMatchSubsystem*           Match      = nullptr;
		UStratShellSubsystem*           Shell      = nullptr;

		/** Empty when the fixture stood up; otherwise the first thing that went wrong. */
		FString Failure;

		explicit FShellWorldScope(const TCHAR* InstanceName)
		{
			if (Scope.World == nullptr)
			{
				Failure = TEXT("UWorld::CreateWorld returned null");
				return;
			}

			Instance = TStrongObjectPtr<UGameInstance>(NewObject<UGameInstance>(GEngine));
			if (!Instance.IsValid())
			{
				Failure = TEXT("could not allocate a UGameInstance");
				return;
			}
			Instance->InitializeStandalone(InstanceName, nullptr);
			Scope.World->SetGameInstance(Instance.Get());
			Scope.World->InitializeActorsForPlay(FURL());

			Controller = Scope.World->SpawnActor<AStratPlayerController>();
			if (Controller == nullptr)
			{
				Failure = TEXT("could not spawn an AStratPlayerController");
				return;
			}

			Hud = Scope.World->SpawnActor<AStratScoreboardHUD>();
			if (Hud == nullptr)
			{
				Failure = TEXT("could not spawn an AStratScoreboardHUD");
				return;
			}
			Hud->PlayerOwner  = Controller;
			Controller->MyHUD = Hud;

			Match = Scope.World->GetSubsystem<UStratMatchSubsystem>();
			if (Match == nullptr)
			{
				Failure = TEXT("the world has no UStratMatchSubsystem");
				return;
			}

			Shell = Instance->GetSubsystem<UStratShellSubsystem>();
			if (Shell == nullptr)
			{
				Failure = TEXT("the game instance has no UStratShellSubsystem, so nothing below "
				               "could observe a forced arm at all");
				return;
			}
		}

		FShellWorldScope(const FShellWorldScope&) = delete;
		FShellWorldScope& operator=(const FShellWorldScope&) = delete;
	};

	/** A match subsystem in a plain transient world -- everything the save-slot clauses need and
	 *  nothing they do not. */
	struct FMatchOnlyScope
	{
		FTestWorldScope       Scope;
		UStratMatchSubsystem* Match = nullptr;
		FString               Failure;

		FMatchOnlyScope()
		{
			if (Scope.World == nullptr)
			{
				Failure = TEXT("UWorld::CreateWorld returned null");
				return;
			}
			Match = Scope.World->GetSubsystem<UStratMatchSubsystem>();
			if (Match == nullptr)
			{
				Failure = TEXT("the world has no UStratMatchSubsystem");
			}
		}

		FMatchOnlyScope(const FMatchOnlyScope&) = delete;
		FMatchOnlyScope& operator=(const FMatchOnlyScope&) = delete;
	};
} // namespace StratNewMatchForcedGuidance

// ---------------------------------------------------------------------------------------
// T-SAVE-06 -- clearing the completion bit leaves every other field exactly as it was.
//
// THE USER DECISION THIS PINS, 2026-09-06: New Match clears ONLY the completion flag and never
// the slot, so a player who saved mid-match and then started a new one can still Continue the
// old one. `UGameplayStatics::DeleteGameInSlot` would have been one line and would have
// silently destroyed that save; this clause is what makes the difference between the two
// observable, and it is the clause that reddens if the one-liner is ever substituted.
//
// NOTHING IS PLANTED. The §4.10 text comes from `SaveMatchToSlot` running on a live shipped
// match, and the completion bit comes from `RecordMatchCompletionOnSave`, the module's own and
// only writer of it. So a clause that reported preservation would be reporting it across a
// state the module can actually produce, not across one the test invented.
//
// THREE CONTROLS, EACH AGAINST A DIFFERENT WAY THIS COULD PASS WHILE BLIND.
//   - The text read before the clear must be NON-EMPTY, or the comparison at the end is two
//     empty strings and cannot fail.
//   - The bit read before the clear must be TRUE, because `false` is also what an absent slot
//     and a never-written field answer -- so without this the final `false` is satisfied by a
//     `ClearMatchCompletionOnSave` that was deleted.
//   - The call must return true with an EMPTY reason, so a refusal cannot be read as a clear.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratClearingCompletionPreservesTheSlotTest,
	"Stratocracy.StratPlay.T-SAVE-06.ClearingCompletionPreservesEverythingButTheBit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratClearingCompletionPreservesTheSlotTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratNewMatchForcedGuidance;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FNamedSlotScope Slot(kPreserveSlot);
	FMatchOnlyScope Fixture;
	if (!TestTrue(*FString::Printf(TEXT("the fixture stood up: %s"), *Fixture.Failure),
			Fixture.Failure.IsEmpty()))
	{
		return false;
	}

	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("the shipped scenario configures against this clause's own slot"),
			MakeConfig(Config, kPreserveSlot, Error)))
	{
		AddError(Error);
		return false;
	}

	if (!TestTrue(TEXT("the match starts"), Fixture.Match->StartMatch(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	// ANTI-VACUITY, AND IT IS THE SAME ONE `StratMatchCompletionRecording.cpp` runs first: the
	// writer returns early on an empty resolved slot, so a clause that had not configured one
	// would be asserting against a write never attempted -- and would then read this file's
	// whole subject as absent.
	const FString Resolved = Fixture.Match->ResolveSaveSlotName(FString());
	if (!TestEqualSensitive(
			TEXT("ANTI-VACUITY: an empty slot name resolves to THIS CLAUSE'S slot, so nothing "
			     "below reads or writes the player's game and no write is skipped silently"),
			Resolved, FString(kPreserveSlot)))
	{
		return false;
	}

	if (!TestTrue(TEXT("the live match saves to the slot"),
			Fixture.Match->SaveMatchToSlot(FString(), Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the module's own writer records the completion"),
			Fixture.Match->RecordMatchCompletionOnSave(FString(), Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- what the slot holds before the clear ---------------------------------
	FString SavedTextBefore;
	FString ScenarioFileBefore;
	FString RulesCommitBefore;
	FString DataHashBefore;
	int32   FirstSideBefore   = INDEX_NONE;
	int32   ViewingSideBefore = INDEX_NONE;
	int32   VersionBefore     = INDEX_NONE;
	{
		UStratSaveGame* const Before =
			Cast<UStratSaveGame>(UGameplayStatics::LoadGameFromSlot(kPreserveSlot, kUserIndex));
		if (!TestNotNull(TEXT("the slot holds a UStratSaveGame before the clear"), Before))
		{
			return false;
		}

		SavedTextBefore    = Before->SaveText;
		ScenarioFileBefore = Before->ScenarioFile;
		RulesCommitBefore  = Before->RulesCommit;
		DataHashBefore     = Before->DataHash;
		FirstSideBefore    = Before->FirstSide;
		ViewingSideBefore  = Before->ViewingSide;
		VersionBefore      = Before->SavedDataVersion;

		if (!TestFalse(
				TEXT("CONTROL: the slot really carries §4.10 text -- without this the comparison "
				     "at the end is two empty strings and cannot fail"),
				SavedTextBefore.IsEmpty()))
		{
			return false;
		}
		if (!TestTrue(
				TEXT("CONTROL: and the completion bit is TRUE going in -- `false` is also what an "
				     "absent slot answers, so without this the false at the end is satisfied by a "
				     "clear that was deleted"),
				Before->bHasCompletedAMatch))
		{
			return false;
		}
	}

	// ---- the clear -------------------------------------------------------------
	FString ClearFailure = TEXT("SENTINEL-NOT-OVERWRITTEN");
	const bool bCleared =
		UStratMatchSubsystem::ClearMatchCompletionOnSave(FString(kPreserveSlot), ClearFailure);

	if (!TestTrue(TEXT("§2.11.6: clearing a slot that carries the bit succeeds"), bCleared))
	{
		AddError(ClearFailure);
		return false;
	}
	TestTrue(TEXT("and it reports no failure reason -- the sentinel this clause planted was "
		"cleared by the call, so an empty reason here is the function's answer and not this "
		"clause's own string"), ClearFailure.IsEmpty());

	// ---- what the slot holds now -----------------------------------------------
	UStratSaveGame* const After =
		Cast<UStratSaveGame>(UGameplayStatics::LoadGameFromSlot(kPreserveSlot, kUserIndex));
	if (!TestNotNull(
			TEXT("§2.11.6: the slot STILL EXISTS and still holds a UStratSaveGame -- the user "
			     "decision was to clear one bit, never to delete the save"),
			After))
	{
		return false;
	}

	TestFalse(TEXT("§2.11.6: the completion bit is off, so the guided opening runs again"),
		After->bHasCompletedAMatch);

	TestEqualSensitive(
		TEXT("T-SAVE-06: the §4.10 text of the match in progress survives the clear byte for "
		     "byte -- Continue must still offer the match it offered before"),
		After->SaveText, SavedTextBefore);
	TestEqualSensitive(TEXT("T-SAVE-06: and so does ScenarioFile"),
		After->ScenarioFile, ScenarioFileBefore);
	TestEqualSensitive(TEXT("T-SAVE-06: and RulesCommit"),
		After->RulesCommit, RulesCommitBefore);
	TestEqualSensitive(TEXT("T-SAVE-06: and DataHash"),
		After->DataHash, DataHashBefore);
	TestEqual(TEXT("T-SAVE-06: and FirstSide"), After->FirstSide, FirstSideBefore);
	TestEqual(TEXT("T-SAVE-06: and ViewingSide"), After->ViewingSide, ViewingSideBefore);
	TestEqual(
		TEXT("T-SAVE-06: and SavedDataVersion is left at the value READ OFF THIS PAYLOAD, not "
		     "restamped -- the divergent-version case has its own clause below"),
		After->SavedDataVersion, VersionBefore);

	// AND THE READER AGREES, which is the half a player actually experiences. The field could
	// be false on disk while the subsystem's own answer came from somewhere else.
	TestFalse(
		TEXT("§2.11.6: and the reader `TryArmGuidedOpening` asks -- HasCompletedAMatchOnSave -- "
		     "now answers false for this slot"),
		Fixture.Match->HasCompletedAMatchOnSave(FString()));

	return true;
}

// ---------------------------------------------------------------------------------------
// T-SAVE-06 -- "nothing to clear" is success, and it authors nothing.
//
// WHY THE RETURN VALUE IS THE LESSER HALF OF THIS CLAUSE. The declaration's reason for the
// success is that `ExecuteRoute` would otherwise log a warning on the single most common path
// through the shipped game -- a first-time player choosing New Match with no save file at all.
// But a `return true` before any work is indistinguishable from a `return true` after the wrong
// work, so the assertion that carries the weight is the one AFTER: the slot must still not
// exist. A clear that default-constructed a payload and saved it would satisfy the return and
// fail here, and would have authored a save file for a player who has never saved.
//
// THE ABSENCE IS MEASURED WITH THE ENGINE'S OWN INSTRUMENT BEFORE THE CALL, not assumed from
// the scope's delete. `DoesSaveGameExist` is asked, and if it answers true the clause stops
// rather than interpreting a success against a slot that was there all along.
//
// THE FAILURE CHANNEL IS SHOWN LIVE IN THE NEXT CLAUSE, and the two are a pair: this one
// requires an empty reason on a success, and that one requires a populated reason on a refusal.
// Neither alone rules out a function that never writes the string at all.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratClearingAnAbsentSlotAuthorsNothingTest,
	"Stratocracy.StratPlay.T-SAVE-06.ClearingAnAbsentSlotSucceedsAndAuthorsNoPayload",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratClearingAnAbsentSlotAuthorsNothingTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratNewMatchForcedGuidance;

	FNamedSlotScope Slot(kAbsentSlot);

	if (!TestFalse(
			TEXT("CONTROL: the slot is genuinely absent before the call, measured with "
			     "DoesSaveGameExist and not assumed from the scope's delete"),
			UGameplayStatics::DoesSaveGameExist(kAbsentSlot, kUserIndex)))
	{
		return false;
	}

	FString ClearFailure = TEXT("SENTINEL-NOT-OVERWRITTEN");
	const bool bCleared =
		UStratMatchSubsystem::ClearMatchCompletionOnSave(FString(kAbsentSlot), ClearFailure);

	TestTrue(
		TEXT("§2.11.6: nothing to clear is SUCCESS -- this is the first-time player's path "
		     "through the shipped game and New Match must not warn on it"),
		bCleared);
	TestTrue(
		TEXT("and with an empty failure reason -- the sentinel this clause planted was cleared "
		     "by the call, so the emptiness is the function's answer"),
		ClearFailure.IsEmpty());

	// THE ASSERTION THAT CARRIES THE CLAUSE.
	TestFalse(
		TEXT("§2.11.6: and the slot is STILL ABSENT -- a clear that default-constructed a "
		     "payload and saved it would have authored a save file for a player who has never "
		     "saved, while returning exactly the true asserted above"),
		UGameplayStatics::DoesSaveGameExist(kAbsentSlot, kUserIndex));

	return true;
}

// ---------------------------------------------------------------------------------------
// T-SAVE-06 -- an empty slot name is REFUSED, with a reason, and resolves to nothing.
//
// WHAT IT PINS AND WHY IT MATTERS HERE MORE THAN IT WOULD ELSEWHERE. `ExecuteRoute` calls the
// clear UNCONDITIONALLY and hands it `CompletionClearSlotForRoute`'s result, which is an empty
// string on every route that does not start a fresh match. The refusal is therefore the whole
// mechanism by which Continue, Return to Title, Quit and Options do not clear anything. A
// version of this function that resolved an empty name to a configured default would clear the
// player's completion bit on EVERY route -- and would do it silently, since the call site logs
// only on a failure.
//
// AND IT IS `DoesSlotHoldARestorableMatch`'S SHAPE, deliberately: refused rather than resolved,
// because the function is `static` for a caller -- `UStratShellSubsystem` on the title world --
// that has no `ActiveConfig` to fall back to.
//
// THE SENTINEL IS THE CONTROL. The reason string is pre-loaded with a value this clause wrote;
// a non-empty reason at the end that still equalled the sentinel would mean the function never
// wrote one and the clause was reading its own input back.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratClearingAnEmptySlotNameIsRefusedTest,
	"Stratocracy.StratPlay.T-SAVE-06.ClearingAnEmptySlotNameIsRefusedWithItsOwnReason",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratClearingAnEmptySlotNameIsRefusedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratNewMatchForcedGuidance;

	static const TCHAR* const Sentinel = TEXT("SENTINEL-NOT-OVERWRITTEN");

	FString    Reason  = Sentinel;
	const bool bResult = UStratMatchSubsystem::ClearMatchCompletionOnSave(FString(), Reason);

	TestFalse(
		TEXT("§2.11.6: an empty slot name is REFUSED and never resolved to a default -- this "
		     "refusal is the whole mechanism by which every route but New Match clears nothing"),
		bResult);

	TestFalse(TEXT("and the refusal carries a reason"), Reason.IsEmpty());
	TestFalse(
		TEXT("CONTROL: and that reason is the function's own, not the sentinel this clause "
		     "planted -- a function that never wrote the string would otherwise read as one "
		     "that explained itself"),
		Reason.Equals(FString(Sentinel), ESearchCase::CaseSensitive));

	return true;
}

// ---------------------------------------------------------------------------------------
// T-SAVE-06 -- the clear does NOT stamp `SavedDataVersion`, and the divergence is deliberate.
//
// THIS IS THE CLAUSE THE ENGINEER FLAGGED AS MOST LIKELY TO BE "FIXED" INTO A STAMP LATER, and
// that is exactly why it is here. `RecordMatchCompletionOnSave` DOES stamp, because it may be
// authoring a payload from nothing and the shape going to disk is then this build's shape.
// This one never creates a payload; stamping would take a slot written at another version and
// make it CLAIM to be current -- which is the lie `IsPayloadRestorable`'s version arm exists to
// catch. A later pass that "made the two writers consistent" would break the reader.
//
// THE PAYLOAD IS PLANTED BY HAND, AND THE REASON IS STATED RATHER THAN ASSUMED. No module-side
// writer can produce a payload at a NON-CURRENT version -- `RecordMatchCompletionOnSave` and
// `SaveMatchToSlot` both stamp `kCurrentSavedDataVersion` -- so the divergent version is a
// PRECONDITION this clause must author, not an expectation it computes. The EXPECTATION is
// still module-side in the only sense that matters: it is the version read back off the payload
// immediately before the call, and no literal appears in the comparison.
//
// `kCurrentSavedDataVersion` APPEARS EXACTLY ONCE, IN A CONTROL. If the planted version equalled
// the current one, a body that stamped would leave the field unmoved and the clause would pass
// while measuring nothing. So the clause asserts they differ before it asserts anything else.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratClearingDoesNotStampTheVersionTest,
	"Stratocracy.StratPlay.T-SAVE-06.ClearingDoesNotStampTheSavedDataVersion",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratClearingDoesNotStampTheVersionTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratNewMatchForcedGuidance;

	FNamedSlotScope Slot(kVersionSlot);

	// ---- a payload at a version this build does not write ----------------------
	{
		UStratSaveGame* const Planted =
			Cast<UStratSaveGame>(UGameplayStatics::CreateSaveGameObject(
				UStratSaveGame::StaticClass()));
		if (!TestNotNull(TEXT("a UStratSaveGame was allocated to plant"), Planted))
		{
			return false;
		}

		Planted->SavedDataVersion    = UStratSaveGame::kCurrentSavedDataVersion + 1;
		Planted->bHasCompletedAMatch = true;
		Planted->SaveText            = TEXT("PlantedSaveText_MiXeDcAsE_9f31");

		if (!TestTrue(TEXT("the planted payload reaches the slot"),
				UGameplayStatics::SaveGameToSlot(Planted, kVersionSlot, kUserIndex)))
		{
			return false;
		}
	}

	int32   VersionBefore = INDEX_NONE;
	FString TextBefore;
	{
		UStratSaveGame* const Before =
			Cast<UStratSaveGame>(UGameplayStatics::LoadGameFromSlot(kVersionSlot, kUserIndex));
		if (!TestNotNull(TEXT("the planted payload reads back"), Before))
		{
			return false;
		}
		VersionBefore = Before->SavedDataVersion;
		TextBefore    = Before->SaveText;

		if (!TestFalse(
				TEXT("CONTROL: the planted version DIFFERS from this build's -- if it did not, a "
				     "body that stamped would leave the field unmoved and this clause would pass "
				     "while measuring nothing"),
				VersionBefore == UStratSaveGame::kCurrentSavedDataVersion))
		{
			return false;
		}
		if (!TestTrue(
				TEXT("CONTROL: and the bit is TRUE going in, so the clear below has work to do "
				     "and does not take its 'already false' early return"),
				Before->bHasCompletedAMatch))
		{
			return false;
		}
	}

	FString    ClearFailure;
	const bool bCleared =
		UStratMatchSubsystem::ClearMatchCompletionOnSave(FString(kVersionSlot), ClearFailure);
	if (!TestTrue(TEXT("§2.11.6: the clear succeeds on a slot from another shape of the struct -- "
			"there is no version gate here, matching HasCompletedAMatchOnSave"), bCleared))
	{
		AddError(ClearFailure);
		return false;
	}

	UStratSaveGame* const After =
		Cast<UStratSaveGame>(UGameplayStatics::LoadGameFromSlot(kVersionSlot, kUserIndex));
	if (!TestNotNull(TEXT("the slot still reads back after the clear"), After))
	{
		return false;
	}

	TestFalse(
		TEXT("CONTROL: the clear demonstrably WROTE -- the bit moved, so the unmoved version "
		     "below is a decision and not a call that did nothing at all"),
		After->bHasCompletedAMatch);

	TestEqual(
		TEXT("T-SAVE-06: SavedDataVersion is left exactly as it was read -- stamping would make a "
		     "payload written at another version CLAIM to be current, which is the lie "
		     "IsPayloadRestorable's version arm exists to catch"),
		After->SavedDataVersion, VersionBefore);

	TestEqualSensitive(TEXT("T-SAVE-06: and the text is untouched on this path too"),
		After->SaveText, TextBefore);

	return true;
}

// ---------------------------------------------------------------------------------------
// GATE-TITLEMENU -- exactly one route starts a match from the scenario, and it is New Match.
//
// THE SWEEP IS OVER `StaticEnum<EStratShellRoute>()` and not over a list typed here, on
// `OnlyTheRestoringRouteCarriesASlotToArm`'s reasoning: a sixth route added tomorrow is asked
// the question automatically, and a clause that enumerated five by hand would have kept
// answering about five.
//
// `EStratShellRoute::NewMatch` IS NAMED, ONCE, AND THAT IS THE SPECIFICATION RATHER THAN A
// COMPUTED EXPECTATION. Which route starts fresh is a design fact with no module-side second
// opinion to read -- `RouteStartsAFreshMatch` IS the authority, so asking it what it thinks
// would be the function agreeing with itself. What makes the single literal checkable is the
// sweep around it: every OTHER declared route must answer false, and the count of trues must be
// exactly one.
//
// IT DELIBERATELY DOES NOT ASSERT THE COMPLEMENT OF `RouteLoadsSaveSlot`, though today that
// complement holds. The declaration argues at length that the two must be free to diverge when
// a route arrives that neither restores nor starts fresh -- a tutorial, a skirmish setup screen
// -- and a clause demanding `bFresh == !bLoads` would redden on that correct change and would
// be read as evidence against it. What IS asserted is MUTUAL EXCLUSION, which is a real
// property and survives that route: no route may both restore a save and start from the
// scenario, because the destination world cannot do both.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellOnlyNewMatchStartsAFreshMatchTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.OnlyTheNewMatchRouteStartsAMatchFromTheScenario",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellOnlyNewMatchStartsAFreshMatchTest::RunTest(const FString& /*Parameters*/)
{
	const UEnum* const RouteEnum = StaticEnum<EStratShellRoute>();
	if (!TestNotNull(TEXT("EStratShellRoute resolved through reflection; this clause will not "
			"report an unobserved sweep as agreement"), RouteEnum))
	{
		return false;
	}

	int32 FreshRoutes    = 0;
	int32 NonFreshRoutes = 0;
	int32 RoutesAsked    = 0;

	for (int32 Index = 0; Index < RouteEnum->NumEnums(); ++Index)
	{
		// UHT appends a hidden `_MAX` enumerator, which is not a route. Dropped by name and
		// CASE-SENSITIVELY, so a future enumerator legitimately called `Max` still counts.
		if (RouteEnum->GetNameStringByIndex(Index).EndsWith(TEXT("_MAX"),
				ESearchCase::CaseSensitive))
		{
			continue;
		}

		const EStratShellRoute Route =
			static_cast<EStratShellRoute>(RouteEnum->GetValueByIndex(Index));
		++RoutesAsked;

		const bool bFresh = UStratShellSubsystem::RouteStartsAFreshMatch(Route);
		const bool bLoads = UStratShellSubsystem::RouteLoadsSaveSlot(Route);

		TestFalse(
			*FString::Printf(
				TEXT("route %d does not both restore a save AND start from the scenario -- the "
				     "destination world cannot do both"),
				static_cast<int32>(Route)),
			bFresh && bLoads);

		if (bFresh)
		{
			++FreshRoutes;
			TestTrue(
				*FString::Printf(
					TEXT("the route that starts a fresh match is NewMatch, and route %d is not"),
					static_cast<int32>(Route)),
				Route == EStratShellRoute::NewMatch);
		}
		else
		{
			++NonFreshRoutes;
		}
	}

	TestTrue(TEXT("some route was asked at all"), RoutesAsked > 0);
	TestEqual(
		TEXT("§2.11.6: EXACTLY ONE declared route starts a fresh match -- a predicate returning "
		     "true for everything would clear the completion bit and force the onboarding on "
		     "Continue, and one returning false for everything would restore the defect this "
		     "whole change exists to fix"),
		FreshRoutes, 1);
	TestTrue(
		TEXT("CONTROL: and some route answered false, so the single true above is a "
		     "discrimination rather than the only route there is"),
		NonFreshRoutes > 0);

	return true;
}

// ---------------------------------------------------------------------------------------
// GATE-TITLEMENU -- only the fresh route carries a slot to clear the completion bit off.
//
// THIS IS THE PROPERTY THAT MAKES `ExecuteRoute`'S UNCONDITIONAL CALL SAFE, and it is the only
// place that property can be measured. The call site is inside `ExecuteRoute`'s travelling
// tail, which nothing in `Source/` executes -- see that function's own declaration, which says
// so and derives it. So `CompletionClearSlotForRoute` was extracted precisely so a clause could
// ask the pairing directly, and this is the clause that takes it up: if it returned the
// configured slot for EVERY route, then Continue, Return to Title and Options would each clear
// the player's completion bit on the way past, and every other clause over this class would
// still be green.
//
// THE EXPECTATION IS READ FROM THE MODULE. Which route is fresh comes from
// `RouteStartsAFreshMatch` -- the same authority the body itself asks -- so this clause pins
// the PAIRING and not the membership, which the clause above pins separately. That split is
// deliberate: one function deciding wrongly is then one red clause, not two.
//
// THE POSITIVE CONTROLS ARE COUNTS IN BOTH DIRECTIONS, on `OnlyTheRestoringRouteCarriesASlotToArm`'s
// shape. An implementation returning empty for everything satisfies every "is empty" assertion
// on its own; an empty return means something only when the same call returned a value for a
// different route.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellOnlyFreshRouteCarriesAClearSlotTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.OnlyTheFreshRouteCarriesASlotToClearTheCompletionOff",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellOnlyFreshRouteCarriesAClearSlotTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratNewMatchForcedGuidance;

	const UEnum* const RouteEnum = StaticEnum<EStratShellRoute>();
	if (!TestNotNull(TEXT("EStratShellRoute resolved through reflection"), RouteEnum))
	{
		return false;
	}

	int32 RoutesCarryingASlot   = 0;
	int32 RoutesCarryingNothing = 0;

	for (int32 Index = 0; Index < RouteEnum->NumEnums(); ++Index)
	{
		if (RouteEnum->GetNameStringByIndex(Index).EndsWith(TEXT("_MAX"),
				ESearchCase::CaseSensitive))
		{
			continue;
		}

		const EStratShellRoute Route =
			static_cast<EStratShellRoute>(RouteEnum->GetValueByIndex(Index));

		const FString ToClear =
			UStratShellSubsystem::CompletionClearSlotForRoute(Route, FString(kConfiguredA));

		// THE EXPECTATION IS READ FROM THE MODULE, not from a list of which route starts fresh.
		const bool bFresh = UStratShellSubsystem::RouteStartsAFreshMatch(Route);

		TestTrue(
			*FString::Printf(
				TEXT("route %d carries a slot to clear exactly when it starts a fresh match "
				     "(fresh=%s, carries=%s)"),
				static_cast<int32>(Route),
				bFresh ? TEXT("yes") : TEXT("no"),
				ToClear.IsEmpty() ? TEXT("nothing") : TEXT("something")),
			bFresh != ToClear.IsEmpty());

		if (ToClear.IsEmpty()) { ++RoutesCarryingNothing; } else { ++RoutesCarryingASlot; }
	}

	TestTrue(TEXT("CONTROL: some route carried a slot -- without this the clause is satisfied by "
		"a function that returns empty for everything, which would mean New Match never clears"),
		RoutesCarryingASlot > 0);
	TestTrue(TEXT("CONTROL: some route carried nothing -- without this it is satisfied by a "
		"function that returns the configured slot for everything, which would clear the "
		"player's completion bit on Continue, Return to Title and Options alike"),
		RoutesCarryingNothing > 0);

	return true;
}

// ---------------------------------------------------------------------------------------
// GATE-TITLEMENU -- the slot cleared is the CONFIGURED one, byte for byte, and not a name baked
// into the body.
//
// THIS IS THE CLAUSE THAT STANDS BETWEEN THE SUITE AND THE PLAYER'S SAVE FILE. A
// `CompletionClearSlotForRoute` whose fresh arm returned `TEXT("StratocracyMatch")` rather than
// its argument would be indistinguishable from the correct one on the shipped configuration,
// and would clear the completion bit on a slot the caller never named the day anything
// configures a different one. That defect is invisible to the pairing clause above, which only
// asks whether the answer is empty.
//
// TWO FIXTURE STRINGS AND NOT ONE. One string can be matched by a constant -- unlikely, but
// unfalsifiable from the clause's side. Two different arguments producing two different
// results, each equal to its own input, cannot be satisfied by any constant at all.
//
// CASE-SENSITIVE THROUGHOUT, because `==`, `Contains` and `TestEqual` are not in UE 5.8, and a
// body returning a lower-cased or re-cased copy of its argument would otherwise read as a
// faithful passthrough. The two fixture names differ in more than capitalisation for the same
// reason.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellClearedSlotIsTheConfiguredOneTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.TheClearedSlotIsTheConfiguredOneAndNotABakedInName",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellClearedSlotIsTheConfiguredOneTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratNewMatchForcedGuidance;

	if (!TestFalse(TEXT("CONTROL: the two fixture slot names differ in more than capitalisation"),
			FString(kConfiguredA).Equals(FString(kConfiguredB), ESearchCase::IgnoreCase)))
	{
		return false;
	}

	const FString FirstAnswer = UStratShellSubsystem::CompletionClearSlotForRoute(
		EStratShellRoute::NewMatch, FString(kConfiguredA));
	const FString SecondAnswer = UStratShellSubsystem::CompletionClearSlotForRoute(
		EStratShellRoute::NewMatch, FString(kConfiguredB));

	TestTrue(
		TEXT("§2.11.6: the fresh route clears the slot it was HANDED, byte for byte"),
		FirstAnswer.Equals(FString(kConfiguredA), ESearchCase::CaseSensitive));
	TestTrue(
		TEXT("§2.11.6: and a different configured slot produces that different slot -- no "
		     "constant can satisfy both of these, which is what rules out a baked-in name"),
		SecondAnswer.Equals(FString(kConfiguredB), ESearchCase::CaseSensitive));

	// AN UNCONFIGURED SLOT CLEARS NOTHING, which is the other half of "not a baked-in name".
	// A body reading `ConfiguredSlot.IsEmpty() ? TEXT("SaveSlot") : ConfiguredSlot` would pass
	// both assertions above and would clear a literal on a project that configures none.
	TestTrue(
		TEXT("§2.11.6: an unconfigured slot carries nothing on the fresh route either, so a "
		     "project that configures no slot clears no file"),
		UStratShellSubsystem::CompletionClearSlotForRoute(
			EStratShellRoute::NewMatch, FString()).IsEmpty());

	return true;
}

// ---------------------------------------------------------------------------------------
// GATE-TITLEMENU -- the forced-guidance arm survives one peek and one consume only.
//
// THE SHAPE IS `ThePendingLoadSlotSurvivesOnePeekAndOneConsumeOnly`'S, and for its reason: a
// flag that was consumed and a flag that was never armed look identical from the outside, so
// the clause runs an ORDERED SEQUENCE on one instance and asserts a true between the falses --
// fresh (false) -> arm (true) -> peek again (still true, so peeking does not consume) ->
// consume (returns true) -> peek (false) -> consume (false) -> re-arm (true again).
//
// THE RE-ARM IS THE POSITIVE CONTROL and without it every false above is equally explained by a
// writer that never worked at all.
//
// WHY "ONCE" IS THE CONTRACT AND NOT A DETAIL. The declaration names the failure on the other
// side of it: a player who takes New Match, plays, and later travels again would be silently
// forced through the onboarding a second time by a flag that survived its first use. The
// clause exists so that a `ConsumePendingForcedGuidance` quietly rewritten as a plain getter
// reddens here rather than in a playtest.
//
// THE OUTER IS A `UGameInstance` AND NOT THE TRANSIENT PACKAGE. `UGameInstanceSubsystem`
// declares `ClassWithin = UGameInstance`, so the transient package is an illegal outer and
// `StaticAllocateObject` fires a handled ensure -- measured and recorded in
// `StratShellRouteClauses.cpp`. The instance below is never initialised and never gets a world;
// it is an outer and nothing else, which is enough because the member exercised here is a plain
// `bool`.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellForcedGuidanceConsumedOnceTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.TheForcedGuidanceArmSurvivesOnePeekAndOneConsumeOnly",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellForcedGuidanceConsumedOnceTest::RunTest(const FString& /*Parameters*/)
{
	const TStrongObjectPtr<UGameInstance> OwningInstance(NewObject<UGameInstance>());
	if (!OwningInstance.IsValid())
	{
		AddError(TEXT("could not construct a UGameInstance to own the shell; nothing was "
			"observed."));
		return false;
	}

	const TStrongObjectPtr<UStratShellSubsystem> Shell(
		NewObject<UStratShellSubsystem>(OwningInstance.Get()));
	if (!Shell.IsValid())
	{
		AddError(TEXT("could not construct a UStratShellSubsystem; nothing was observed."));
		return false;
	}

	TestFalse(TEXT("a fresh shell forces no guided opening"),
		Shell->PeekPendingForcedGuidance());

	Shell->ArmPendingForcedGuidance(true);

	TestTrue(TEXT("arming leaves something to be found"),
		Shell->PeekPendingForcedGuidance());
	TestTrue(TEXT("peeking twice does not consume"),
		Shell->PeekPendingForcedGuidance());

	TestTrue(TEXT("consuming returns the arm"), Shell->ConsumePendingForcedGuidance());

	TestFalse(TEXT("and clears it, so a later travel is handed nothing"),
		Shell->PeekPendingForcedGuidance());
	TestFalse(
		TEXT("§2.11.6: a second consume returns false -- a player who takes New Match, plays, "
		     "and travels again is not forced through the onboarding a second time"),
		Shell->ConsumePendingForcedGuidance());

	// THE POSITIVE CONTROL.
	Shell->ArmPendingForcedGuidance(true);
	TestTrue(TEXT("the shell can still hold an arm after a consume, so the falses above are a "
		"clearing rather than a broken writer"),
		Shell->PeekPendingForcedGuidance());

	return true;
}

// ---------------------------------------------------------------------------------------
// GATE-TITLEMENU -- arming FALSE clears an arm an earlier route left behind.
//
// WHY THIS IS A SEPARATE CLAUSE FROM THE CONSUME ONE. `ExecuteRoute` calls
// `ArmPendingForcedGuidance(RouteStartsAFreshMatch(Route))` UNCONDITIONALLY, with no `if`, so
// arming false is not a no-op it happens to perform -- it is the entire mechanism by which a
// route that does not start a fresh match fails to inherit an arm. An implementation reading
// `if (bInForced) { bPendingForcedGuidance = true; }` would satisfy every assertion in the
// consume clause above and would leave a New Match arm standing across a subsequent Continue,
// forcing the onboarding onto a restored match.
//
// THE ORDER IS THE SUBJECT: arm true, confirm, arm false, confirm cleared -- and then arm true
// again as the positive control that the object still works, so the false in the middle is a
// clearing and not a writer that had stopped writing.
//
// AND THE CLEARED STATE IS CHECKED THROUGH BOTH READERS, peek and consume, because a `Peek` that
// answered from a different field than `Consume` reads from would otherwise pass this.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShellArmingFalseClearsAPriorArmTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.ArmingFalseClearsAnArmAnEarlierRouteLeftBehind",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShellArmingFalseClearsAPriorArmTest::RunTest(const FString& /*Parameters*/)
{
	const TStrongObjectPtr<UGameInstance> OwningInstance(NewObject<UGameInstance>());
	if (!OwningInstance.IsValid())
	{
		AddError(TEXT("could not construct a UGameInstance to own the shell; nothing was "
			"observed."));
		return false;
	}

	const TStrongObjectPtr<UStratShellSubsystem> Shell(
		NewObject<UStratShellSubsystem>(OwningInstance.Get()));
	if (!Shell.IsValid())
	{
		AddError(TEXT("could not construct a UStratShellSubsystem; nothing was observed."));
		return false;
	}

	Shell->ArmPendingForcedGuidance(true);
	if (!TestTrue(
			TEXT("CONTROL: an arm is standing before the false one -- without this the clause is "
			     "satisfied by a shell that was never armed"),
			Shell->PeekPendingForcedGuidance()))
	{
		return false;
	}

	Shell->ArmPendingForcedGuidance(false);

	TestFalse(
		TEXT("§2.11.6: arming false CLEARS -- ExecuteRoute arms unconditionally, so this is the "
		     "whole mechanism by which Continue does not inherit a New Match arm and force the "
		     "onboarding onto a restored match"),
		Shell->PeekPendingForcedGuidance());
	TestFalse(
		TEXT("and the consuming reader agrees with the peek, so the two are not reading "
		     "different fields"),
		Shell->ConsumePendingForcedGuidance());

	// THE POSITIVE CONTROL.
	Shell->ArmPendingForcedGuidance(true);
	TestTrue(TEXT("the shell can still be armed afterwards, so the false above is a clearing "
		"rather than a writer that had stopped writing"),
		Shell->PeekPendingForcedGuidance());

	return true;
}

// ---------------------------------------------------------------------------------------
// T-UI-03 -- the forced arm is NOT consumed before the bridge is seeded.
//
// THIS CLOSES A DEBT THE ENGINEER DECLARED OPEN, and the fixture it doubted exists turned out to
// be reachable: `AStratPlayerController::DecorateForPresentation` is PUBLIC and calls
// `TryArmGuidedOpening` on every invocation, so a clause can drive the refresh path directly
// with the match subsystem present and UNSEEDED. `PeekPendingForcedGuidance` is the
// non-destructive reader that makes the state observable without disturbing it. No seam was
// needed and none was added.
//
// WHAT IS PINNED, EXACTLY. The consume sits BELOW the seeded guard. `TryArmGuidedOpening` runs
// on every refresh and returns at that guard until `StartMatch` has finished; a consume moved
// ABOVE it would burn the flag on a refresh that armed nothing, and the refresh that finally
// armed would read false and suppress. The bug that shape produces is the exact one this whole
// change exists to fix, and -- because it depends on how many refreshes precede the first seeded
// one -- it would be INTERMITTENT, which is the kind of defect a suite must catch by
// construction rather than by luck.
//
// THE TWO HALVES FAIL IN OPPOSITE DIRECTIONS, which is what makes the pair worth more than
// either alone.
//   - UNSEEDED: three refreshes, and the arm must SURVIVE all three. A consume moved above the
//     guard reddens here.
//   - SEEDED: one refresh, and the arm must be GONE. A fixture whose refreshes never reached
//     `TryArmGuidedOpening` at all -- a controller that could not find the match, a shell the
//     controller's game instance does not serve -- reddens here, so the survival above cannot
//     be explained by a call that never happened.
// There is no state of the world in which both halves pass for the wrong reason.
//
// THE UNSEEDED STATE IS MEASURED, NOT ASSUMED. `GetBridge()` is asked before the refreshes and
// the clause stops if it reports a seeded bridge, because a fixture that had somehow started a
// match would make the whole first half a statement about the wrong branch.
//
// AND THE CONFIGURED SLOT IS THIS CLAUSE'S OWN, ABSENT ONE. On the seeded refresh the forced arm
// short-circuits the save read, so the disk should not be consulted at all -- but "should not"
// is the thing under test, and a fixture that pointed at the shipped slot would read the
// player's file on exactly the run where the short-circuit was broken.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratForcedArmNotConsumedBeforeSeedingTest,
	"Stratocracy.StratPlay.T-UI-03.TheForcedGuidanceArmIsNotConsumedBeforeTheBridgeIsSeeded",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratForcedArmNotConsumedBeforeSeedingTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratNewMatchForcedGuidance;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FNamedSlotScope  Slot(kSeedGuardSlot);
	FShellWorldScope Fixture(TEXT("StratNewMatchForcedGuidance_SeedGuard"));
	if (!TestTrue(*FString::Printf(TEXT("the fixture stood up: %s"), *Fixture.Failure),
			Fixture.Failure.IsEmpty()))
	{
		return false;
	}

	// The exact hop `TryArmGuidedOpening` takes, asserted rather than assumed: if the controller
	// cannot reach this shell, the first half below measures nothing.
	if (!TestTrue(
			TEXT("CONTROL: the shell the controller's own game instance serves is the shell this "
			     "clause arms -- TryArmGuidedOpening reaches it by GetGameInstance()"),
			Fixture.Controller->GetGameInstance() == Fixture.Instance.Get()))
	{
		return false;
	}

	Fixture.Shell->ArmPendingForcedGuidance(true);

	// ---- the unseeded half -----------------------------------------------------
	if (!TestTrue(
			TEXT("CONTROL: the bridge is UNSEEDED before the refreshes below -- measured, "
			     "because a fixture that had somehow started a match would make this half a "
			     "statement about the wrong branch"),
			Fixture.Match->GetBridge() == nullptr || !Fixture.Match->GetBridge()->IsSeeded()))
	{
		return false;
	}

	for (int32 Refresh = 0; Refresh < 3; ++Refresh)
	{
		FStratViewModel Model;
		Fixture.Controller->DecorateForPresentation(Model);

		if (!TestTrue(
				*FString::Printf(
					TEXT("§2.11.6: refresh %d before seeding did NOT consume the forced arm -- a "
					     "consume above the seeded guard would burn it on a refresh that armed "
					     "nothing, and the arming refresh would then suppress"),
					Refresh),
				Fixture.Shell->PeekPendingForcedGuidance()))
		{
			return false;
		}
	}

	// ---- the seeded half, which is the positive control ------------------------
	FStratMatchConfig Config;
	FString           Error;
	if (!TestTrue(TEXT("the shipped scenario configures against this clause's own absent slot"),
			MakeConfig(Config, kSeedGuardSlot, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the match starts"), Fixture.Match->StartMatch(Config, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: and the bridge is now seeded, so the refresh below passes the "
			"guard the refreshes above stopped at"),
			Fixture.Match->GetBridge() != nullptr && Fixture.Match->GetBridge()->IsSeeded()))
	{
		return false;
	}

	FStratViewModel SeededModel;
	if (!TestTrue(TEXT("the view model builds on the seeded match"),
			Fixture.Match->BuildViewModel(SeededModel, Error)))
	{
		AddError(Error);
		return false;
	}
	Fixture.Controller->DecorateForPresentation(SeededModel);

	TestFalse(
		TEXT("§2.11.6: POSITIVE CONTROL -- the first SEEDED refresh consumes the arm, so the "
		     "three survivals above are the guard doing its job and not a refresh path that "
		     "never reached TryArmGuidedOpening at all"),
		Fixture.Shell->PeekPendingForcedGuidance());

	return true;
}

// ---------------------------------------------------------------------------------------
// T-UI-03 -- the New Match arm forces the guided opening past a completed match on the save.
//
// THIS IS THE CLAUSE THE WHOLE CHANGE IS FOR, and it is the only one that measures the user's
// sentence -- "New Match always plays the guided opening" -- end to end through the shipped
// controller rather than through a helper. The measured defect it stands against is a live PIE
// session on this tree at `e36e78c`: `Shell route 0 opening ...` followed 164 ms later by
// `Guided opening suppressed for side 0: this save has a completed match.`
//
// THE FORM IS DIFFERENTIAL AND THE SINGLE VARIABLE IS THE ARM. Two identical fixtures, each
// with a completed match recorded on its own slot by the module's own writer, each seeded from
// the shipped scenario, each refreshed once through `DecorateForPresentation`. One is armed and
// one is not. `Model.Guidance.bActive` must differ, and the difference must fall the right way.
//
// WHY BOTH HALVES ARE REQUIRED RATHER THAN JUST THE FORCED ONE.
//   - Without the UNFORCED half, an `bActive` of true is equally explained by a suppression
//     that stopped working altogether -- which would be a different defect wearing this one's
//     green.
//   - Without the FORCED half, the clause says only what
//     `T-UI-03.GuidanceIsSuppressedByACompletedMatchOnTheSave` already says.
//
// THE COMPLETION IS NOT PLANTED. `RecordMatchCompletionOnSave` writes it -- the module's own and
// only writer -- and `HasCompletedAMatchOnSave`, the exact reader `TryArmGuidedOpening` asks, is
// then required to answer TRUE before either refresh. So the suppression pressure the arm has
// to overcome is real and is measured, not asserted.
//
// WHAT THIS DOES NOT CLAIM. It does not pin that `ExecuteRoute` arms anything -- that call sits
// in the travelling tail nothing in `Source/` executes, and it is recorded as an open debt in
// this file's report and in `Tools/architect/state/tests.md`. It pins what the destination
// world does with an arm, which is the half a clause can reach.
// ---------------------------------------------------------------------------------------
namespace StratNewMatchForcedGuidance
{
	/**
	 * Stands a seeded match up with a completed match recorded on `SlotName`, refreshes once
	 * through the shipped controller, and reports whether §2.11.6's strip came up.
	 *
	 * ONE HELPER FOR BOTH HALVES, so that `bArmForced` is demonstrably the only difference
	 * between them. Two copied bodies would have been two places for a difference to hide.
	 */
	static bool ObserveGuidanceUnderACompletedMatch(FAutomationTestBase& Test,
	                                                const TCHAR*         InstanceName,
	                                                const TCHAR*         SlotName,
	                                                const bool           bArmForced,
	                                                bool&                OutGuidanceActive,
	                                                FString&             OutError)
	{
		FShellWorldScope Fixture(InstanceName);
		if (!Fixture.Failure.IsEmpty())
		{
			OutError = Fixture.Failure;
			return false;
		}

		if (bArmForced)
		{
			Fixture.Shell->ArmPendingForcedGuidance(true);
		}

		FStratMatchConfig Config;
		if (!MakeConfig(Config, SlotName, OutError))
		{
			return false;
		}
		if (!Fixture.Match->StartMatch(Config, OutError))
		{
			return false;
		}

		// ANTI-VACUITY: the writer returns early and silently on an empty resolved slot.
		const FString Resolved = Fixture.Match->ResolveSaveSlotName(FString());
		if (!Resolved.Equals(FString(SlotName), ESearchCase::CaseSensitive))
		{
			OutError = FString::Printf(
				TEXT("an empty slot name resolved to '%s' rather than this half's own slot "
				     "'%s'; the completion below would have been written somewhere else"),
				*Resolved, SlotName);
			return false;
		}

		if (!Fixture.Match->RecordMatchCompletionOnSave(FString(), OutError))
		{
			return false;
		}

		// THE SUPPRESSION PRESSURE IS MEASURED WITH THE READER THE CONTROLLER ITSELF ASKS.
		if (!Fixture.Match->HasCompletedAMatchOnSave(FString()))
		{
			OutError = TEXT("the slot does not report a completed match after the module's own "
			                "writer recorded one, so neither half below would be measuring a "
			                "guided opening under suppression pressure");
			return false;
		}

		Test.TestTrue(
			TEXT("CONTROL: the arm is in the state this half asked for, immediately before the "
			     "refresh that reads it"),
			Fixture.Shell->PeekPendingForcedGuidance() == bArmForced);

		FStratViewModel Model;
		if (!Fixture.Match->BuildViewModel(Model, OutError))
		{
			return false;
		}

		Fixture.Controller->DecorateForPresentation(Model);
		OutGuidanceActive = Model.Guidance.bActive;
		return true;
	}
} // namespace StratNewMatchForcedGuidance

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratNewMatchArmForcesGuidanceTest,
	"Stratocracy.StratPlay.T-UI-03.ANewMatchArmForcesGuidancePastACompletedMatchOnTheSave",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratNewMatchArmForcesGuidanceTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratNewMatchForcedGuidance;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
	// THE SUPPRESSION LINE IS NOT DECLARED, DELIBERATELY. `FStratGuidedOpening::Begin` emits it
	// at `Log` verbosity, which is not an automation failure, and `AddExpectedMessagePlain` with
	// `Occurrences 0` means AT LEAST ONE -- so declaring it would make the ARMED half fail for
	// the correct reason that it never suppressed anything.

	// ---- the control half: the same fixture, unarmed, IS suppressed -------------
	bool bControlActive = true;
	{
		FNamedSlotScope Slot(kControlSlot);
		FString         Error;
		if (!TestTrue(TEXT("the unarmed half stood up"),
				ObserveGuidanceUnderACompletedMatch(
					*this, TEXT("StratNewMatchForcedGuidance_Control"), kControlSlot,
					/*bArmForced*/ false, bControlActive, Error)))
		{
			AddError(Error);
			return false;
		}

		if (!TestFalse(
				TEXT("CONTROL: with no arm, a completed match on the save DOES suppress §2.11.6 "
				     "-- this is the shipped behaviour the measured PIE session showed, and "
				     "without it the true below is equally explained by a suppression that "
				     "stopped working altogether"),
				bControlActive))
		{
			return false;
		}
	}

	// ---- the case: armed, and the guided opening runs anyway --------------------
	bool bForcedActive = false;
	{
		FNamedSlotScope Slot(kForcedSlot);
		FString         Error;
		if (!TestTrue(TEXT("the armed half stood up"),
				ObserveGuidanceUnderACompletedMatch(
					*this, TEXT("StratNewMatchForcedGuidance_Forced"), kForcedSlot,
					/*bArmForced*/ true, bForcedActive, Error)))
		{
			AddError(Error);
			return false;
		}
	}

	TestTrue(
		TEXT("§2.11.6: New Match's forced arm plays the guided opening even though the save "
		     "records a completed match -- the single difference between this and the "
		     "suppressed control above is ArmPendingForcedGuidance(true)"),
		bForcedActive);

	TestFalse(
		TEXT("and the two halves genuinely differ, which is the whole content of the "
		     "measurement"),
		bControlActive == bForcedActive);

	return true;
}
