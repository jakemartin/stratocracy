// The gate on §2.11.6's WRITER -- `UStratMatchSubsystem::RecordMatchCompletionOnSave` and the
// `ApplyView` hook (`NoteMatchResultIfEnded`) that calls it -- under T-UI-03 and T-SAVE-06.
//
// WHY THIS FILE EXISTS, AND IT IS A GAP RATHER THAN A DUPLICATE. Two clauses already touch
// `UStratSaveGame::bHasCompletedAMatch`:
//   - `Stratocracy.StratPlay.T-UI-03.GuidanceIsSuppressedByACompletedMatchOnTheSave` PLANTS the
//     field by hand and pins the READER's consequence.
//   - `Stratocracy.StratPlay.T-SAVE-06.SaveOverASlotPreservesOnboardingState` PLANTS it too, and
//     pins that `SaveMatchToSlot` carries a planted value forward.
// Both pass identically whether the writer exists, is deleted, or no-ops -- their input is the
// test's own. NOTHING IN THIS FILE PLANTS `bHasCompletedAMatch`. Every clause below that asserts
// it TRUE reaches that state only through the subsystem's own writer, and the one clause that
// calls the writer directly does so as a CONTROL for an absence, never as a set-up for an
// assertion about the writer's caller.
//
// THE ONE TRAP THIS FILE IS SHAPED AROUND, AND IT IS THE REASON THE FIRST ASSERTION IN EVERY
// CLAUSE IS ABOUT A STRING. `NoteMatchResultIfEnded` returns EARLY AND SILENTLY when
// `ResolveSaveSlotName(FString())` is empty -- deliberately, and its own comment says why ("a
// warning per frame for a condition that is not a fault is how a real refusal gets buried"). A
// clause that forgot to configure `FStratMatchConfig::SaveSlotName` would then be asserting
// against a subsystem that never attempted the write at all, and would report the writer's
// absence as the writer's silence. So every clause asks the subsystem what slot an empty name
// resolves to, requires it non-empty, and requires it to be THIS FILE'S slot -- as its own
// check, before anything about the effect is asked. If that check fails the clause stops; it
// does not go on to interpret a false.
//
// WHERE THE EXPECTATIONS COME FROM.
//   - "The match reached a result" is `FStratViewModel::Match::bHasResult`, which
//     `UStratMatchSubsystem::BuildViewModel` fills from `strat::buildUiSnapshot`. This file
//     never decides that a match is over, never counts turns, and never sets that field.
//   - The result is reached BY THE RULES MODULE'S OWN AI playing both sides, exactly as
//     `Stratocracy.StratPlay.T-INT-05.BothSidesAiReachesAResultWithinTheBound` does. No model is
//     hand-built and no `bHasResult` is forced, so the hook is observed on the same value a
//     player's screen would have been drawn from.
//   - The sides that get an AI are the distinct `FStratUnitView::Side` values the projection
//     reports; the AI's buildlist ids are `DT_Units`' own row names. Neither is written down.
//   - The §4.10 text a clause requires preserved is the text `SaveMatchToSlot` itself produced,
//     read back off the slot before the writer runs and compared to the same slot after. No
//     text, hash, hex or count is spelled in this file.
//   - The payload version is `UStratSaveGame::kCurrentSavedDataVersion`, the module's constant.
//
// EVERY STRING COMPARISON IS CASE SENSITIVE. `FString::operator==`, `Contains` and
// `TestEqual(FString)` all ignore case in UE 5.8; this project has already shipped one clause
// that could not fail because of it. Every string comparison below goes through
// `FAutomationTestBase::TestEqualSensitive` -- the ENGINE's own, not a local copy; a local one
// was written first and was shadowed by the member inside `RunTest`, which is a compile error
// rather than a silent weakening, but the engine's is the right call either way.
//
// `Occurrences = 0` MEANS "ANY NUMBER, BUT AT LEAST ONE". A declaration for a line that never
// fires is itself a failure. Two are declared here and both were measured to fire:
//   - "no tile mesh for terrain" -- this fixture has no Blueprint defaults, the standing warning
//     `StratMatchReconcile.cpp` records.
//   - "STRAT-AI refused" -- only in the clauses that RUN the AI to a result.
//     `Stratocracy.StratPlay.T-INT-05.BothSidesAiReachesAResultWithinTheBound` records the
//     measurement: a completed AI-vs-AI game's closing `EndTurn` is refused by the rules module
//     with "no match is running", so the line fires once per completed game. The two clauses
//     that do NOT run the AI do not declare it.
//
// AND `RunAiTurnsNow`'s RETURN VALUE IS RECORDED, NEVER ASSERTED, for the reason that same
// clause states: a completed AI-vs-AI game currently returns FALSE because of that refused
// closing `EndTurn`. Asserting true would fail on a correct game; asserting false would DEMAND
// the defect. What this file needs from that call is only that the match reached a result, and
// `bHasResult` is what says so.
//
// **[THE SENTENCE ABOVE BEGINNING "a completed AI-vs-AI game currently returns FALSE" IS
// FALSIFIED AS OF 2026-08-23. IT WAS TRUE WHEN WRITTEN AND IS KEPT, STAMPED, RATHER THAN
// DELETED.** Sec 2.8's end-of-match transition landed:
// `UStratMatchSubsystem::RunAiTurnsNow` now rebuilds the model after a refused turn and, when
// `StratMatchIsConcluded` answers yes, breaks out with its `StopReason` EMPTY -- so it falls
// past both the `StopReason` arm and the refresh arm and returns TRUE with an empty reason for
// a game that finished. "Asserting true would fail on a correct game" is no longer so.
//
// THE CROSS-REFERENCE IS THE PART THAT MATTERED, AND IT IS WHY THIS WAS A GATE FINDING RATHER
// THAN AN OBSERVATION. "the reason that same clause states" points at
// `T-INT-05.BothSidesAiReachesAResultWithinTheBound` in `StratAiMatchClauses.cpp`, whose
// paragraph WAS corrected in the same pass that landed the transition. This local copy was
// missed, so the pointer aimed at corrected text while restating the uncorrected version
// nearer to the reader -- and a reader trusts the nearer sentence. Two copies of one claim;
// the retraction reached the one where the evidence sits and not the one where the work is.
//
// WHAT IS STILL EXACTLY TRUE, AND IT IS WHY NOT ONE ASSERTION IN THIS FILE MOVES. The rules
// module still refuses the winning side's closing `EndTurn` with `[T-SAVE-05] no match is
// running`, and `FStratAiTurnRunner::RunTurn` still reports `bOk = false` for it; what changed
// is the CLASSIFICATION one layer above the runner. And this paragraph's operative sentence --
// "what this file needs from that call is only that the match reached a result, and
// `bHasResult` is what says so" -- was never about the return value and is unaffected.
// Recording the return rather than asserting it is now a CHOICE and not a necessity: the
// return value has clauses of its own,
// `Stratocracy.StratPlay.T-AI-01.AMatchWonMidTurnIsNotReportedAsAnAiFault` and
// `.AGenuineAiRefusalIsStillAFault` in `StratMatchConclusion.cpp`, and two files asserting one
// value is one file too many.]**
//
// THE SLOT IS THIS FILE'S OWN AND IT IS DELETED ON BOTH ENDS OF EVERY CLAUSE. The shipped
// `SaveSlotName` is `StratocracyMatch`, the player's slot; a gate that wrote there would destroy
// a developer's in-progress game on every suite run, and -- worse for a file about a bool that
// suppresses onboarding -- would leave `bHasCompletedAMatch` true on that machine forever.
//
// NO `strat::` FREE FUNCTION IS CALLED -- LNK2019 outside StratBridge and StratRules, measured
// 8 x. `FStratBridge` is an exported class and its methods are callable from here.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "Templates/Function.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
#include "StratSaveGame.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

// Included from a .cpp and never from a UHT-parsed header.
#include "StratBridge.h"

namespace StratMatchCompletionRecording
{
	static const int32 kFirstSide   = 0;
	static const int32 kViewingSide = 0;
	static const int32 kUserIndex   = 0;

	/** This file's slot, and never the configured default. See the header block. */
	static const TCHAR* kTestSlotName = TEXT("StratocracyAutomation_MatchCompletion");

	/** `TheCompletionHookIsSilentWithoutTheOptIn`'s OWN slot -- see that clause's block on why
	 *  it does not share the one above. Namespace scope rather than clause-local because the
	 *  config adjusters that name it are capture-less lambdas. */
	static const TCHAR* kOptInGateSlotName = TEXT("StratocracyAutomation_OptInGate");

	/** See `StratSaveSlotClauses.cpp` on why these are supplied and never read off a manifest. */
	static const TCHAR* kRulesCommit = TEXT("automation-rules-commit");
	static const TCHAR* kDataHash    = TEXT("automation-data-hash");

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	static bool MakeConfig(FStratMatchConfig& Out, FString& OutError)
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
		Out.ViewingSide     = kViewingSide;
		Out.BoardActorClass = AStratBoardActor::StaticClass();
		Out.UnitActorClass  = AStratUnitActor::StaticClass();
		Out.RulesCommit     = kRulesCommit;
		Out.DataHash        = kDataHash;
		Out.SaveSlotName    = kTestSlotName;

		// THE HOOK WRITES ONLY WHERE SOMETHING DELIBERATELY CHOSE, and naming a slot is not
		// that choosing. See `FStratMatchConfig::bRecordCompletionOnMatchEnd`: `SaveSlotName`
		// answers WHERE and cannot answer WHETHER, so this file's own slot is not on its own
		// consent for `NoteMatchResultIfEnded` to write to it. Every clause built on this
		// helper is about the hook, so every one of them opts in, in writing, here.
		//
		// `TheCompletionHookIsSilentWithoutTheOptIn` below is the clause that puts this line
		// back to the module's default and requires the silence -- so this line is a fixture
		// decision that has its own gate, not an assumption.
		Out.bRecordCompletionOnMatchEnd = true;
		return true;
	}

	/** §4.8's unit ids, read off the table the match is configured with -- the row NAME is the
	 *  id. See `StratAiMatchClauses.cpp`, which owns that reasoning. */
	static TArray<FName> UnitIdsFrom(const FStratMatchConfig& Config)
	{
		return Config.UnitTable != nullptr ? Config.UnitTable->GetRowNames() : TArray<FName>();
	}

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

	/** Deletes this file's slot on construction and again on destruction. Both ends, for
	 *  `StratSaveSlotClauses.cpp`'s reason: a clause that failed halfway through last run must
	 *  not poison the next one, and a developer's `Saved/SaveGames/` must not be littered. */
	struct FSlotScope
	{
		FSlotScope()  { Clear(); }
		~FSlotScope() { Clear(); }

		static void Clear()
		{
			if (UGameplayStatics::DoesSaveGameExist(kTestSlotName, kUserIndex))
			{
				UGameplayStatics::DeleteGameInSlot(kTestSlotName, kUserIndex);
			}
		}

		FSlotScope(const FSlotScope&) = delete;
		FSlotScope& operator=(const FSlotScope&) = delete;
	};

	/**
	 * `FSlotScope` for a slot this file names per-clause rather than per-file.
	 *
	 * WHY A SECOND SCOPE TYPE RATHER THAN A PARAMETER ON THE FIRST. Nothing forces the two
	 * apart, but the clause that uses this one deliberately reaches a §2.8 result with the
	 * hook's opt-in at its C++ DEFAULT, and every OTHER clause here reaches a result with the
	 * opt-in on. Sharing one slot between them would let a leak from either be read as the
	 * other's, which is the exact confusion the whole file is shaped against.
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

	/**
	 * The anti-vacuity check every clause runs FIRST, and the reason this file has a helper for
	 * three lines.
	 *
	 * `NoteMatchResultIfEnded` returns early and silently on an empty resolved slot name. A
	 * clause that skipped this would assert an effect against a subsystem that never attempted
	 * the write, and would call that a measurement of the writer.
	 */
	static bool SlotNameResolves(FAutomationTestBase& Test, UStratMatchSubsystem* Subsystem,
	                             const TCHAR* ExpectedSlot = kTestSlotName)
	{
		const FString Resolved = Subsystem->ResolveSaveSlotName(FString());

		if (!Test.TestFalse(
				TEXT("ANTI-VACUITY: an empty slot name resolves to a NON-EMPTY slot -- the hook "
				     "returns early and silently when it does not, and every assertion below "
				     "would then be about a write that was never attempted"),
				Resolved.IsEmpty()))
		{
			return false;
		}

		return Test.TestEqualSensitive(
			TEXT("ANTI-VACUITY: and it resolves to THIS CLAUSE'S slot rather than the shipped "
			     "default, so nothing below reads or writes the player's game"),
			Resolved, FString(ExpectedSlot));
	}

	/** The distinct sides the PROJECTION reports units for. Not a constant, and not two. */
	static TArray<int32> SidesIn(const FStratViewModel& Model)
	{
		TArray<int32> Sides;
		for (const FStratUnitView& Unit : Model.Units)
		{
			Sides.AddUnique(Unit.Side);
		}
		Sides.Sort();
		return Sides;
	}

	/**
	 * Starts an AI-vs-AI match on `Subsystem` and plays it, in one synchronous call, until the
	 * rules module gives it a result.
	 *
	 * IT IS THE ONLY WAY A CLAUSE IN THIS FILE REACHES `bHasResult`, and that is the point: the
	 * value the hook observes is `strat::buildUiSnapshot`'s, produced by the module's own AI
	 * playing the shipped scenario. A hand-built `FStratViewModel` with the bit forced would
	 * pin the hook against an input no production path can produce.
	 *
	 * TWO `StartMatch` CALLS, DELIBERATELY. The first is a scouting match, used only to ask the
	 * projection which sides exist; the second is the real one. `StratAiMatchClauses.cpp` does
	 * the same and for the same reason -- `AiSides` must name real sides, and this file does not
	 * get to decide what those are.
	 *
	 * NOTE FOR A READER OF A FAILING RUN: the scouting `StartMatch` and the real one BOTH end in
	 * `BuildViewModel` -> `ApplyView`, so the hook has already been offered two chances before a
	 * single AI turn runs -- with `bHasResult` FALSE both times. That is why a clause can assert
	 * "nothing was recorded" immediately after this returns and have it mean something.
	 */
	static bool PlayAnAiMatchToAResult(FAutomationTestBase& Test, UStratMatchSubsystem* Subsystem,
	                                   FString& OutError,
	                                   TFunctionRef<void(FStratMatchConfig&)> Adjust)
	{
		FStratMatchConfig Base;
		if (!MakeConfig(Base, OutError))
		{
			return false;
		}

		// THE ONE SEAM, AND IT IS APPLIED BEFORE THE SCOUTING MATCH so that both `StartMatch`
		// calls below run under the SAME config. A clause that adjusted only the second would
		// be measuring a subsystem whose `ActiveConfig` had disagreed with itself mid-clause.
		// The only clause that passes anything but a no-op is the opt-in's own gate.
		Adjust(Base);

		TArray<int32> Sides;
		{
			FString ScoutReason;
			Subsystem->StartMatch(Base, ScoutReason);
			if (!Subsystem->IsMatchLive())
			{
				OutError = ScoutReason;
				return false;
			}

			FStratViewModel Scout;
			if (!Subsystem->BuildViewModel(Scout, OutError))
			{
				return false;
			}
			Sides = SidesIn(Scout);
		}

		if (Sides.Num() < 2)
		{
			OutError = FString::Printf(
				TEXT("the seeded scenario deploys units for %d side(s); an AI-vs-AI game needs at least 2"),
				Sides.Num());
			return false;
		}

		FStratMatchConfig BothAi = Base;
		BothAi.AiSides            = Sides;
		BothAi.AiBuildlistUnitIds = UnitIdsFrom(BothAi);

		FString StartReason;
		Subsystem->StartMatch(BothAi, StartReason);
		if (!Subsystem->IsMatchLive())
		{
			OutError = StartReason;
			return false;
		}

		return true;
	}

	/** The same, with the config exactly as `MakeConfig` leaves it. */
	static bool PlayAnAiMatchToAResult(FAutomationTestBase& Test, UStratMatchSubsystem* Subsystem,
	                                   FString& OutError)
	{
		return PlayAnAiMatchToAResult(Test, Subsystem, OutError,
			[](FStratMatchConfig&) {});
	}
}

// ---------------------------------------------------------------------------
// T-UI-03 -- a match that reaches a §2.8 result records completion on the slot, with nothing
// having planted the field.
//
// THE SUBJECT IS THE WRITER AND ITS CALLER TOGETHER. `RecordMatchCompletionOnSave` is never
// called by this clause; the only thing called is the AI, and the only thing asserted is that
// the slot afterwards says a match was completed. So the clause goes red on all three of the
// ways the feature can be absent:
//   - the `NoteMatchResultIfEnded(Model)` line deleted from `ApplyView`;
//     **[CITATION CORRECTED 2026-08-23, AND THE ARGUMENT IS DELIBERATELY UNWEAKENED.
//     `ApplyView` NO LONGER CONTAINS THAT LINE**: it now calls
//     `UStratMatchSubsystem::ConcludeMatchIfEnded`, which calls `NoteMatchResultIfEnded`
//     itself, FIRST and unlatched, in the same order it always ran in. Read the bullet as
//     "the `NoteMatchResultIfEnded(Model)` call deleted from anywhere in the `ApplyView` ->
//     `ConcludeMatchIfEnded` -> `NoteMatchResultIfEnded` chain" -- deleting the link at
//     EITHER end still reddens this clause, which is the property the bullet always claimed,
//     so the reasoning survives the indirection intact. The old wording stays because it was
//     accurate for the tree it described; what a reader following it to `ApplyView` will not
//     find is the literal line, and that is the whole of the staleness. The new intermediate
//     link has a clause of its own,
//     `Stratocracy.StratPlay.T-INT-05.ConcludingLogsOnceAndOnlyOnce`.]**
//   - the hook moved onto a path this clause does not take (a command-submit hook would never
//     fire here -- the AI's commands go through `FStratAiTurnRunner` and the result is observed
//     by the reconciliation `RunAiTurnsNow` ends in);
//   - the writer no-opping, refusing, or writing a different field.
//
// AND THE SLOT DID NOT EXIST WHEN THE CLAUSE STARTED. `FSlotScope` deletes it, and that absence
// is asserted rather than assumed -- if the slot existed, "it says true afterwards" could be a
// leftover from another clause or from a developer's own game.
//
// WHAT THIS CLAUSE DOES NOT PIN, said plainly: that a HUMAN hot-seat match reaching a result
// records it. The path is the same `ApplyView` and the same model field, but the game that
// reaches the result here is played by the AI because that is the only way this suite can reach
// a §2.8 result inside one test. The observation the hook makes is of a MODEL, not of a mover.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratResultRecordsCompletionTest,
	"Stratocracy.StratPlay.T-UI-03.AMatchReachingAResultRecordsCompletionOnTheSlot",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratResultRecordsCompletionTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchCompletionRecording;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
	// Fires once when the completed game's closing EndTurn is refused. See the header block.
	AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FSlotScope Slot;
	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Subsystem))
	{
		return false;
	}

	if (!TestFalse(TEXT("the slot does not exist when the clause starts -- so anything it says "
	                    "afterwards was put there during this run"),
			UGameplayStatics::DoesSaveGameExist(kTestSlotName, kUserIndex)))
	{
		return false;
	}

	FString Error;
	if (!TestTrue(TEXT("an AI-vs-AI match is configured and live"),
			PlayAnAiMatchToAResult(*this, Subsystem, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- THE ANTI-VACUITY CHECK, ON ITS OWN, BEFORE ANY EFFECT IS ASKED ABOUT ----
	if (!SlotNameResolves(*this, Subsystem))
	{
		return false;
	}

	// ---- and nothing has been recorded yet, though the hook has already run twice ----
	// See `PlayAnAiMatchToAResult`: two `StartMatch` calls each ended in `ApplyView`.
	if (!TestFalse(
			TEXT("nothing is recorded before the match reaches a result, though ApplyView has "
			     "already run -- so the hook is observing bHasResult and not merely running"),
			Subsystem->HasCompletedAMatchOnSave(FString())))
	{
		return false;
	}
	if (!TestFalse(TEXT("and no slot has been created either"),
			UGameplayStatics::DoesSaveGameExist(kTestSlotName, kUserIndex)))
	{
		return false;
	}

	// ---- the rules module's own AI plays the game out -------------------------
	FString    RunReason;
	const bool bRan = Subsystem->RunAiTurnsNow(RunReason);
	AddInfo(FString::Printf(TEXT("RunAiTurnsNow returned %s; reason: '%s'"),
		bRan ? TEXT("true") : TEXT("false"), *RunReason));

	FStratViewModel After;
	if (!TestTrue(TEXT("the view model builds after the game"),
			Subsystem->BuildViewModel(After, Error)))
	{
		AddError(Error);
		return false;
	}

	// THE PRECONDITION OF THE WHOLE CLAUSE, AND IT IS THE MODULE'S ANSWER. If the game did not
	// finish there is nothing for the hook to have observed, and every assertion below would be
	// pinning the wrong thing. Stopped rather than reported as a writer failure.
	if (!TestTrue(
			TEXT("§2.8: the AI-vs-AI game reached a result under its own steam -- the model field "
			     "the hook reads, filled by strat::buildUiSnapshot and not by this file"),
			After.Match.bHasResult))
	{
		return false;
	}

	// ---- the assertion ---------------------------------------------------------
	TestTrue(
		TEXT("§2.11.6: the slot now exists -- created by the writer alone, with no SaveMatchToSlot "
		     "in this clause and nothing planted"),
		UGameplayStatics::DoesSaveGameExist(kTestSlotName, kUserIndex));

	TestTrue(
		TEXT("§2.11.6: a match that reached a result records completion on the save -- read "
		     "through the accessor AStratPlayerController::TryArmGuidedOpening's bSuppressed "
		     "argument is driven from"),
		Subsystem->HasCompletedAMatchOnSave(FString()));

	// And on the disk itself, not only through the accessor -- a reader that answered true from
	// a session-local bool would satisfy the line above and nothing else.
	UStratSaveGame* const Payload =
		Cast<UStratSaveGame>(UGameplayStatics::LoadGameFromSlot(kTestSlotName, kUserIndex));
	if (TestNotNull(TEXT("the slot holds a UStratSaveGame"), Payload))
	{
		TestTrue(TEXT("§2.11.6: and the field on the payload is the one that was set"),
			Payload->bHasCompletedAMatch);
		TestEqual(TEXT("and the payload is stamped at this build's shape version"),
			Payload->SavedDataVersion, UStratSaveGame::kCurrentSavedDataVersion);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-03 -- THE CONTROL: a match with no result records nothing, and the writer could have
// written if there had been something to record.
//
// WITHOUT THIS CLAUSE THE ONE ABOVE PROVES ONLY THAT SOMETHING WROTE A TRUE. A hook that ran
// unconditionally -- `NoteMatchResultIfEnded` with its `bHasResult` arm dropped -- would pass it
// exactly, and would suppress §2.11.6's onboarding for every player on the first frame of their
// first match. That is the defect this clause exists to catch, and it is a worse one than the
// missing writer was.
//
// THE ABSENCE CARRIES ITS OWN INSTRUMENT CHECK, IN THE SAME CLAUSE. "No slot was written" is
// satisfied by a subsystem that cannot write at all -- a wrong slot name, a read-only directory,
// a `CreateSaveGameObject` returning null. So after the absence is measured, the writer is
// called DIRECTLY on the same subsystem and the same slot and is required to succeed. The direct
// call is a control for the silence and never a set-up: nothing is asserted about the writer's
// CALLER after it runs.
//
// `ApplyView` IS RUN REPEATEDLY, through `RefreshPresentation`, so the silence is not the
// silence of a hook that simply never got a turn.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratNoResultRecordsNothingTest,
	"Stratocracy.StratPlay.T-UI-03.AMatchWithNoResultRecordsNothingOnTheSlot",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratNoResultRecordsNothingTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchCompletionRecording;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FSlotScope Slot;
	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Subsystem))
	{
		return false;
	}

	if (!TestFalse(TEXT("the slot does not exist when the clause starts"),
			UGameplayStatics::DoesSaveGameExist(kTestSlotName, kUserIndex)))
	{
		return false;
	}

	// A plain hot-seat match: no AI, so nothing plays it and it cannot reach a result.
	FStratMatchConfig Config;
	FString Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	FString StartReason;
	Subsystem->StartMatch(Config, StartReason);
	if (!TestTrue(TEXT("the match is live"), Subsystem->IsMatchLive()))
	{
		AddError(StartReason);
		return false;
	}

	// ---- THE ANTI-VACUITY CHECK, ON ITS OWN ------------------------------------
	if (!SlotNameResolves(*this, Subsystem))
	{
		return false;
	}

	// ---- the model really has no result, and that is the module's answer -------
	FStratViewModel Model;
	if (!TestTrue(TEXT("the view model builds"), Subsystem->BuildViewModel(Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestFalse(
			TEXT("the opening position carries no §2.8 result -- the field the hook reads, and "
			     "the only difference between this clause and the one above"),
			Model.Match.bHasResult))
	{
		return false;
	}

	// ---- the hook is offered several turns -------------------------------------
	for (int32 Refresh = 0; Refresh < 3; ++Refresh)
	{
		FString RefreshReason;
		Subsystem->RefreshPresentation(RefreshReason);
	}

	// ---- the absence -----------------------------------------------------------
	TestFalse(
		TEXT("§2.11.6: a match with no result records nothing -- the slot was never created"),
		UGameplayStatics::DoesSaveGameExist(kTestSlotName, kUserIndex));
	TestFalse(
		TEXT("§2.11.6: and the accessor the guidance layer asks still answers false, so a first "
		     "session's onboarding is not suppressed on its own first frame"),
		Subsystem->HasCompletedAMatchOnSave(FString()));

	// ---- CONTROL: the instrument can speak -------------------------------------
	// The writer is called directly, on the same subsystem and the same slot, and required to
	// succeed. Without this, every assertion above is satisfied by a subsystem that could not
	// have written whatever happened.
	FString WriteReason;
	if (!TestTrue(
			TEXT("CONTROL: the same subsystem CAN write this slot -- so the absence above is a "
			     "decision the hook made and not an inability"),
			Subsystem->RecordMatchCompletionOnSave(FString(), WriteReason)))
	{
		AddError(WriteReason);
		return false;
	}
	TestTrue(TEXT("CONTROL: and the slot exists once it has"),
		UGameplayStatics::DoesSaveGameExist(kTestSlotName, kUserIndex));
	TestTrue(TEXT("CONTROL: and the accessor reports it, so the false above was not the "
	              "accessor's own blindness"),
		Subsystem->HasCompletedAMatchOnSave(FString()));

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-06 -- recording a completion preserves the §4.10 text already in the slot.
//
// THE PROPERTY IS READ-MODIFY-WRITE, PINNED FROM THE WRITER'S SIDE. `SaveOverASlotPreservesOn-
// boardingState` pins the same slot's other direction -- a save carrying a PLANTED onboarding
// value forward. This is the direction that had no gate: a `RecordMatchCompletionOnSave` that
// default-constructed a `UStratSaveGame` would turn a guidance bug into a save-ERASING one, and
// it would do it at the exact moment a player finished a match with an in-progress save in the
// slot.
//
// THE EXPECTED TEXT IS THE SLOT'S OWN, CAPTURED BEFORE. `SaveMatchToSlot` writes a real §4.10
// serialization; this clause reads that string off the slot, plays the game out, and requires
// the same bytes back. Nothing about the format is asserted and no hash is written down --
// `StratBridgeRestoreParity.cpp` owns the format.
//
// AND THE CONTROLS ARE BOTH DIRECTIONS OF "IT WAS NOT ALREADY LIKE THAT":
//   - the captured text is required NON-EMPTY, or the comparison at the end is two empty strings;
//   - `bHasCompletedAMatch` is required FALSE on the slot before the game runs, or the true at
//     the end could be the value `SaveMatchToSlot` itself carried forward.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratRecordingPreservesSaveTextTest,
	"Stratocracy.StratPlay.T-SAVE-06.RecordingCompletionPreservesTheSavedMatchText",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratRecordingPreservesSaveTextTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchCompletionRecording;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
	AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FSlotScope Slot;
	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Subsystem))
	{
		return false;
	}

	FString Error;
	if (!TestTrue(TEXT("an AI-vs-AI match is configured and live"),
			PlayAnAiMatchToAResult(*this, Subsystem, Error)))
	{
		AddError(Error);
		return false;
	}

	if (!SlotNameResolves(*this, Subsystem))
	{
		return false;
	}

	// ---- a real save into the slot, mid-match ---------------------------------
	if (!TestTrue(TEXT("the live match saves to the slot"),
			Subsystem->SaveMatchToSlot(FString(), Error)))
	{
		AddError(Error);
		return false;
	}

	FString SavedTextBefore;
	FString ScenarioFileBefore;
	int32   FirstSideBefore   = INDEX_NONE;
	int32   ViewingSideBefore = INDEX_NONE;
	{
		UStratSaveGame* const Before =
			Cast<UStratSaveGame>(UGameplayStatics::LoadGameFromSlot(kTestSlotName, kUserIndex));
		if (!TestNotNull(TEXT("the slot holds a UStratSaveGame after the save"), Before))
		{
			return false;
		}

		SavedTextBefore    = Before->SaveText;
		ScenarioFileBefore = Before->ScenarioFile;
		FirstSideBefore    = Before->FirstSide;
		ViewingSideBefore  = Before->ViewingSide;

		if (!TestTrue(
				TEXT("CONTROL: the slot really carries §4.10 text -- without this the comparison "
				     "at the end is two empty strings and cannot fail"),
				!SavedTextBefore.IsEmpty()))
		{
			return false;
		}
		if (!TestFalse(
				TEXT("CONTROL: and the completion bit is NOT already set -- so the true at the end "
				     "is the writer's and not something SaveMatchToSlot carried forward"),
				Before->bHasCompletedAMatch))
		{
			return false;
		}
	}

	// ---- the game plays out and the hook fires --------------------------------
	FString    RunReason;
	const bool bRan = Subsystem->RunAiTurnsNow(RunReason);
	AddInfo(FString::Printf(TEXT("RunAiTurnsNow returned %s; reason: '%s'"),
		bRan ? TEXT("true") : TEXT("false"), *RunReason));

	FStratViewModel After;
	if (!TestTrue(TEXT("the view model builds after the game"),
			Subsystem->BuildViewModel(After, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("§2.8: the AI-vs-AI game reached a result under its own steam"),
			After.Match.bHasResult))
	{
		return false;
	}

	// ---- what the slot holds now ----------------------------------------------
	UStratSaveGame* const Now =
		Cast<UStratSaveGame>(UGameplayStatics::LoadGameFromSlot(kTestSlotName, kUserIndex));
	if (!TestNotNull(TEXT("the slot still holds a UStratSaveGame"), Now))
	{
		return false;
	}

	if (!TestTrue(
			TEXT("§2.11.6: the completion was recorded -- so the preservation below is measured "
			     "across a write that demonstrably happened"),
			Now->bHasCompletedAMatch))
	{
		return false;
	}

	TestEqualSensitive(
		TEXT("T-SAVE-06: the §4.10 text of the match in progress survives the completion write "
		     "byte for byte -- a writer that default-constructed its payload would have erased it"),
		Now->SaveText, SavedTextBefore);
	TestEqualSensitive(
		TEXT("T-SAVE-06: and so does the ScenarioFile, which §4.10 cannot carry"),
		Now->ScenarioFile, ScenarioFileBefore);
	TestEqual(TEXT("T-SAVE-06: and FirstSide"), Now->FirstSide, FirstSideBefore);
	TestEqual(TEXT("T-SAVE-06: and ViewingSide"), Now->ViewingSide, ViewingSideBefore);
	TestEqual(TEXT("and the payload is stamped at this build's shape version"),
		Now->SavedDataVersion, UStratSaveGame::kCurrentSavedDataVersion);

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-06 -- a completion the WRITER recorded survives a save over the slot.
//
// WHAT THIS ADDS TO `SaveOverASlotPreservesOnboardingState`, which already pins the carry-
// forward: that clause's input is a `bHasCompletedAMatch` the test itself planted, so it holds
// whether or not anything in the tree can produce that value. Here the value on the slot is the
// subsystem's own -- written by `RecordMatchCompletionOnSave` -- and what is pinned is that the
// two writers of one slot do not collide in the direction that matters to a player: finishing a
// match and then saving must not un-finish it.
//
// THE WRITER IS CALLED DIRECTLY HERE AND THAT IS DELIBERATE. The subject is `SaveMatchToSlot`'s
// treatment of a value the writer produced, not the hook that calls the writer -- the hook has
// its own clause above. Calling it directly also lets this clause run on a hot-seat match with
// no AI game in it.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSaveOverPreservesARecordedCompletionTest,
	"Stratocracy.StratPlay.T-SAVE-06.ASaveOverPreservesACompletionTheWriterRecorded",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSaveOverPreservesARecordedCompletionTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchCompletionRecording;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FSlotScope Slot;
	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Subsystem))
	{
		return false;
	}

	FStratMatchConfig Config;
	FString Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	FString StartReason;
	Subsystem->StartMatch(Config, StartReason);
	if (!TestTrue(TEXT("the match is live"), Subsystem->IsMatchLive()))
	{
		AddError(StartReason);
		return false;
	}

	if (!SlotNameResolves(*this, Subsystem))
	{
		return false;
	}

	// ---- the writer's own output, into a slot that did not exist --------------
	FString WriteReason;
	if (!TestTrue(TEXT("the writer records a completion on a slot that did not exist"),
			Subsystem->RecordMatchCompletionOnSave(FString(), WriteReason)))
	{
		AddError(WriteReason);
		return false;
	}

	{
		UStratSaveGame* const Fresh =
			Cast<UStratSaveGame>(UGameplayStatics::LoadGameFromSlot(kTestSlotName, kUserIndex));
		if (!TestNotNull(TEXT("the writer created a UStratSaveGame in the slot"), Fresh))
		{
			return false;
		}
		if (!TestTrue(TEXT("carrying the completion"), Fresh->bHasCompletedAMatch))
		{
			return false;
		}
		TestTrue(
			TEXT("and carrying NO §4.10 text -- the writer's declaration says a payload it creates "
			     "goes to disk with an empty SaveText, which LoadMatchFromSlot refuses by name"),
			Fresh->SaveText.IsEmpty());
	}

	// ---- the other writer of the same slot ------------------------------------
	if (!TestTrue(TEXT("the live match saves over it"),
			Subsystem->SaveMatchToSlot(FString(), Error)))
	{
		AddError(Error);
		return false;
	}

	UStratSaveGame* const After =
		Cast<UStratSaveGame>(UGameplayStatics::LoadGameFromSlot(kTestSlotName, kUserIndex));
	if (!TestNotNull(TEXT("the slot still holds a UStratSaveGame"), After))
	{
		return false;
	}

	// The control first: the save demonstrably wrote, so "preserved" is not "untouched".
	TestTrue(
		TEXT("CONTROL: the slot now carries §4.10 text it did not carry a moment ago -- so "
		     "SaveMatchToSlot really ran over this payload"),
		!After->SaveText.IsEmpty());
	TestEqualSensitive(
		TEXT("CONTROL: and the live match's ScenarioFile with it"),
		After->ScenarioFile, Config.ScenarioFile);

	TestTrue(
		TEXT("T-SAVE-06: and the completion the WRITER recorded survives -- a player who finishes "
		     "a match and then saves has still finished one"),
		After->bHasCompletedAMatch);
	TestTrue(
		TEXT("T-SAVE-06: read back through the accessor the guidance layer asks"),
		Subsystem->HasCompletedAMatchOnSave(FString()));

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-03 -- THE TRAP GATE: with `FStratMatchConfig::bRecordCompletionOnMatchEnd` at its C++
// default, a match that reaches a §2.8 result creates NO slot at all.
//
// THIS IS THE CLAUSE THAT KEEPS A FIXED DEFECT FIXED, and it is about fixtures nobody has
// written yet. The defect it gates was measured, not imagined: `SaveSlotName` defaults to the
// PLAYER'S slot (`StratocracyMatch`) and is never empty, so before the opt-in existed, ANY
// clause anywhere in this suite that played a match to a result wrote a 2096-byte
// `StratocracyMatch.sav` into a developer's `Saved/SaveGames/` -- and left §2.11.6's onboarding
// suppressed on that machine forever. `StratAiMatchClauses.cpp` did exactly that, and it is not
// a save-system file and had no reason to think about slots.
//
// SO THE THING BEING PINNED IS A NEGATIVE ABOUT CODE THAT DOES NOT EXIST YET: a future fixture
// that plays a match out and never thinks about saving must remain harmless. The only way to
// state that as a gate is to state it about the DEFAULT, which is what every such fixture will
// inherit. `TheOptInDefaultsOffInCpp` below pins the default itself; this pins its consequence
// on the live hook path.
//
// AND THE DEFAULT IS READ FROM THE MODULE, NEVER WRITTEN DOWN HERE. The adjust below assigns
// `FStratMatchConfig().bRecordCompletionOnMatchEnd`, so if a later "convenience" flip turned the
// default on, this clause does not quietly keep testing `false` -- phase A would opt in, the
// slot WOULD appear, and the clause goes red. That is the correct outcome: the trap re-armed.
//
// THE ABSENCE CARRIES ITS CONTROL IN THIS SAME CLAUSE, IN THE STRONGEST FORM AVAILABLE.
// "No slot was created" is satisfied for free by a fixture that never reached a result, by a
// scenario that failed to load, by an AI that refused its first turn. Phase A rules the first
// of those out by requiring `FStratMatchView::bHasResult` -- the module's own answer -- before
// it interprets anything. Phase B rules out the rest, and rules out the whole class of
// "this path cannot write here" at once: the SAME subsystem, the SAME slot name and the SAME
// scenario play a second AI game to a result with the opt-in ON, and the slot MUST appear. So
// the silence in phase A is a decision the hook made, measured against a demonstration that the
// hook could have spoken.
//
// (The opt-in clause at the top of this file is NOT a sufficient control on its own: it runs in
// a different world, on a different slot, at a different moment in the process. An absence and
// its control belong in one clause, or the control is an assumption about another clause's
// health.)
//
// TWO AI GAMES IN ONE CLAUSE IS THE COST, DELIBERATELY PAID. The alternative -- trusting another
// clause -- is what this file exists to refuse.
//
// THE SLOT IS THIS CLAUSE'S ALONE AND IS DELETED ON BOTH ENDS. Any fixture that reaches a result
// is now a potential writer, which is the whole point of the clause; a clause about that must
// not be the one that leaves a file behind.
//
// WHAT THIS DOES NOT PIN: that the SHIPPED game opts in. It does not, today --
// `BP_StratGameMode`'s `MatchConfig` has no such checkbox yet, so the packaged game's guided
// opening re-arms every match. That clause is owed and recorded in
// `Tools/architect/state/tests.md`; it belongs after the Content/ default lands.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratHookIsSilentWithoutTheOptInTest,
	"Stratocracy.StratPlay.T-UI-03.TheCompletionHookIsSilentWithoutTheOptIn",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratHookIsSilentWithoutTheOptInTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratMatchCompletionRecording;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
	// Fires when a completed game's closing EndTurn is refused; two games run here.
	AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FNamedSlotScope Slot(kOptInGateSlotName);
	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	UStratMatchSubsystem* const Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
	if (!TestNotNull(TEXT("the world has a match subsystem"), Subsystem))
	{
		return false;
	}

	if (!TestFalse(TEXT("this clause's slot does not exist when it starts -- so anything that "
	                    "appears was put there during this run"),
			UGameplayStatics::DoesSaveGameExist(kOptInGateSlotName, kUserIndex)))
	{
		return false;
	}

	// ================= PHASE A: the opt-in at the module's default ==============
	FString Error;
	if (!TestTrue(TEXT("an AI-vs-AI match is configured and live, with the completion opt-in "
	                   "left at whatever FStratMatchConfig declares it to be"),
			PlayAnAiMatchToAResult(*this, Subsystem, Error,
				[](FStratMatchConfig& Config)
				{
					Config.SaveSlotName = kOptInGateSlotName;
					// READ FROM THE MODULE, NOT WRITTEN DOWN. See the block above.
					Config.bRecordCompletionOnMatchEnd =
						FStratMatchConfig().bRecordCompletionOnMatchEnd;
				})))
	{
		AddError(Error);
		return false;
	}

	// The hook's OTHER silent arm must not be the one doing the work here: if the slot name
	// resolved empty, `NoteMatchResultIfEnded` would return before it ever consulted the opt-in
	// and phase A would be measuring the wrong refusal.
	if (!SlotNameResolves(*this, Subsystem, kOptInGateSlotName))
	{
		return false;
	}

	FString    RunReason;
	const bool bRan = Subsystem->RunAiTurnsNow(RunReason);
	AddInfo(FString::Printf(TEXT("PHASE A: RunAiTurnsNow returned %s; reason: '%s'"),
		bRan ? TEXT("true") : TEXT("false"), *RunReason));

	FStratViewModel After;
	if (!TestTrue(TEXT("the view model builds after the game"),
			Subsystem->BuildViewModel(After, Error)))
	{
		AddError(Error);
		return false;
	}

	// THE PRECONDITION, AND IT IS THE MODULE'S ANSWER. Without it the absence below is the
	// absence of a game that never finished.
	if (!TestTrue(
			TEXT("§2.8: the AI-vs-AI game reached a result under its own steam -- so the hook was "
			     "offered the exact model it acts on, and declined for the opt-in's sake alone"),
			After.Match.bHasResult))
	{
		return false;
	}

	TestFalse(
		TEXT("§2.11.6: with the opt-in at its C++ default, a match reaching a result creates NO "
		     "slot -- this is what keeps a fixture that merely plays a game from writing a save"),
		UGameplayStatics::DoesSaveGameExist(kOptInGateSlotName, kUserIndex));
	TestFalse(
		TEXT("§2.11.6: and the accessor still answers false, so nothing was recorded anywhere "
		     "the guidance layer reads"),
		Subsystem->HasCompletedAMatchOnSave(FString()));

	// ================= PHASE B: THE CONTROL, same everything but the opt-in =====
	// Without this, every line above is satisfied by a subsystem that could not have written
	// this slot on this path under any circumstances.
	if (!TestTrue(TEXT("CONTROL: a second AI-vs-AI match is configured and live, identical but "
	                   "for the opt-in"),
			PlayAnAiMatchToAResult(*this, Subsystem, Error,
				[](FStratMatchConfig& Config)
				{
					Config.SaveSlotName                = kOptInGateSlotName;
					Config.bRecordCompletionOnMatchEnd = true;
				})))
	{
		AddError(Error);
		return false;
	}

	if (!SlotNameResolves(*this, Subsystem, kOptInGateSlotName))
	{
		return false;
	}

	// StartMatch clears `bMatchResultRecorded`, so the hook is genuinely offered this game.
	if (!TestFalse(TEXT("CONTROL: and the slot is still absent going into the second game"),
			UGameplayStatics::DoesSaveGameExist(kOptInGateSlotName, kUserIndex)))
	{
		return false;
	}

	FString    ControlReason;
	const bool bControlRan = Subsystem->RunAiTurnsNow(ControlReason);
	AddInfo(FString::Printf(TEXT("PHASE B: RunAiTurnsNow returned %s; reason: '%s'"),
		bControlRan ? TEXT("true") : TEXT("false"), *ControlReason));

	FStratViewModel ControlAfter;
	if (!TestTrue(TEXT("CONTROL: the view model builds after the second game"),
			Subsystem->BuildViewModel(ControlAfter, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: §2.8: the second game reached a result too"),
			ControlAfter.Match.bHasResult))
	{
		return false;
	}

	TestTrue(
		TEXT("CONTROL: with the opt-in ON, this same subsystem writing this same slot from this "
		     "same path DOES create it -- so phase A's absence is the hook's decision and not "
		     "this path's inability"),
		UGameplayStatics::DoesSaveGameExist(kOptInGateSlotName, kUserIndex));
	TestTrue(TEXT("CONTROL: and the accessor reports it, so phase A's false was not the "
	              "accessor's own blindness"),
		Subsystem->HasCompletedAMatchOnSave(FString()));

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-06 -- the completion opt-in is OFF in C++.
//
// ONE LINE, AND IT IS WHAT STOPS A LATER CONVENIENCE FLIP FROM BEING INVISIBLE. The clause above
// reads this same default off the module rather than writing it down, which makes it honest but
// also leaves it silent about WHICH value the default is. This clause names it.
//
// WHERE THE EXPECTATION COMES FROM: the reviewer's ruling that produced the field. A slot name
// answers WHERE and cannot answer WHETHER, so the unprompted write must be off until something
// says yes -- and the only place C++ can say "off by default" is the member initialiser. There
// is no module-side value to compare against here; the default IS the subject, so the assertion
// is a direct read of it and the literal is the ruling.
//
// THIS IS NOT A DUPLICATE OF THE CLAUSE ABOVE. That one would go red if the default flipped, but
// it would go red saying "a slot appeared", which reads as a hook defect. This one says what
// actually changed.
//
// WHAT IT DOES NOT PIN: what the SHIPPED `BP_StratGameMode` sets. A Blueprint default is not a
// C++ default and this clause cannot see one. See the owed clause in
// `Tools/architect/state/tests.md`.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratOptInDefaultsOffInCppTest,
	"Stratocracy.StratPlay.T-SAVE-06.TheOptInDefaultsOffInCpp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratOptInDefaultsOffInCppTest::RunTest(const FString& /*Parameters*/)
{
	const FStratMatchConfig Defaults;

	TestFalse(
		TEXT("§2.11.6: FStratMatchConfig::bRecordCompletionOnMatchEnd is FALSE in C++ -- the "
		     "unprompted completion write is off until something deliberately says yes, and a "
		     "config assembled in code (every automation fixture there will ever be) says no"),
		Defaults.bRecordCompletionOnMatchEnd);

	return true;
}
