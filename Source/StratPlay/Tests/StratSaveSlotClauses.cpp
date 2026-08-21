// The gate on the save-slot half of §4.10 -- `UStratMatchSubsystem::SaveMatchToSlot`,
// `LoadMatchFromSlot`, `DoesSaveSlotExist` and `HasCompletedAMatchOnSave`, plus
// `UStratSaveGame` -- under T-SAVE-04, T-SAVE-06 and T-INT-05.
//
// WHAT THIS FILE COVERS THAT `StratBridgeRestoreParity.cpp` DOES NOT. That file pins the
// §4.10 TEXT going into and out of a bridge. This one pins where a save LIVES: the slot, the
// three fields §4.10 cannot carry (`ScenarioFile`, `FirstSide`, `ViewingSide`), the payload
// version gate, and the two §2.11.6 onboarding fields that must survive being saved over.
// `UStratSaveGame`'s header block says it is "deliberately the only file in the tree that
// knows" where a save lives; this is the only test file that knows it either.
//
// WHERE EVERY EXPECTATION COMES FROM.
//   - Every state hash is `FStratBridge::StateHash()`, captured off the LIVE bridge before a
//     save and compared against the live bridge after a load. Nothing is written down.
//   - Every command count is `FStratBridge::RecordedCommandCount()`.
//   - Every move submitted is chosen by `FStratBridge::ReachableHexes` -- the rules module
//     picks the destination, so a scenario edit does not need an edit here and no coordinate
//     is spelled.
//   - The version numbers in the version-gate clause are
//     `UStratSaveGame::kCurrentSavedDataVersion` and that value plus one. The constant is the
//     module's; the `+ 1` is the clause's way of saying "a version this build does not read"
//     without naming a number that would go stale on the first bump.
//   - The two onboarding values in the save-over clause are PLANTED by this file, because
//     nothing in the tree writes them yet -- `UStratSaveGame`'s header records that debt and
//     names the condition that discharges it. Planted input, module-side output: what is
//     asserted is that `SaveMatchToSlot` CARRIED THEM FORWARD, and the control for that is
//     that the same call demonstrably overwrote every other field.
//
// THE SLOT IS THIS FILE'S OWN AND IT CLEANS UP AFTER ITSELF. `FStratMatchConfig::SaveSlotName`
// defaults to `StratocracyMatch`, which is the player's slot; a gate that wrote there would
// destroy a developer's in-progress game on every suite run. Every clause below uses
// `kTestSlotName` and deletes it on entry AND on exit, so a clause that failed halfway
// through last run does not poison the next one.
//
// THIS FIXTURE HAS NO TILE MESHES, and every clause declares the board's warning expected --
// the same standing `StratMatchReconcile.cpp` records.
//
// AND `Occurrences = 0` MEANS "ANY NUMBER, BUT AT LEAST ONE" -- MEASURED, not read off the
// signature. It does NOT mean "zero or more". A declaration for a line that never fires is
// itself a test failure: "Expected suppressed ('Warning') level log message or higher matching
// 'X' did not occur." Two declarations in this file were removed on that measurement, for
// `LoadMatchFromSlot`'s "Load of slot ... reported:" line -- which fires only when the load
// returns FALSE, and in this fixture the SECOND `StartMatchInternal` returns true. So an
// expected-message declaration is a claim that the line DOES appear, and must be treated as
// one; a spare one added defensively turns a green clause red.
//
// A CONSEQUENCE WORTH KNOWING BEFORE READING ANY ASSERTION BELOW. `StartMatch` returns FALSE in
// this fixture while `IsMatchLive()` returns TRUE, because `StratMatchSubsystem.h` splits
// all-or-nothing on the rules side from not-all-or-nothing on the presentation side. So the
// return value is NOT how these clauses ask whether a match or a load worked; the bridge's hash
// is, on both sides of every round trip.
//
// NO `strat::` FREE FUNCTION IS CALLED ANYWHERE BELOW -- LNK2019 outside StratBridge and
// StratRules, measured 8 x.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Math/IntPoint.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "StratMatchSubsystem.h"
#include "StratSaveGame.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

// Included from a .cpp and never from a UHT-parsed header.
#include "StratBridge.h"

namespace StratSaveSlotClauses
{
	static const int32 kFirstSide   = 0;
	static const int32 kViewingSide = 0;

	/**
	 * This file's slot, and never the configured default.
	 *
	 * `FStratMatchConfig::SaveSlotName` is `StratocracyMatch` -- the player's slot. A gate
	 * that wrote there would delete a developer's game every time the suite ran, and the
	 * failure would look like a save-system bug rather than a test.
	 */
	static const TCHAR* kTestSlotName = TEXT("StratocracyAutomation_SaveSlotClauses");

	/** The one hardcoded user index the subsystem uses. §2.11 is hot-seat: one logged-in user. */
	static const int32 kUserIndex = 0;

	/**
	 * The two identity fields, supplied by configuration on both the write and the read.
	 *
	 * THEY DO NOT NEED TO MATCH A MANIFEST AND MUST NOT BE READ FROM ONE. `FStratSaveIdentity`'s
	 * block says every field of it is SUPPLIED and never recomputed, and `UStratSaveGame` stores
	 * the two values the save was WRITTEN with so the load builds the same expectation. The
	 * round trip is therefore self-consistent by construction, which is the property under test;
	 * a clause that fed it the vendored manifest's values would be pinning `T-INT-01` by
	 * accident and would go red on a re-vendor for no reason of its own.
	 */
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
		return true;
	}

	/** The transient world, with the engine context teardown wants. `StratMatchReconcile.cpp`
	 *  records why the context is not decoration and why the order below is not free. */
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

		// The world first and the context second -- measured: the other order makes
		// `UWorld::DestroyActor` warn "World has no context!" once per actor.
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
	 * Deletes this file's slot on construction and again on destruction.
	 *
	 * BOTH ENDS, DELIBERATELY. Deleting only on the way out leaves a clause that failed
	 * halfway through poisoning the next run with a slot it did not expect; deleting only on
	 * the way in leaves the developer's `Saved/SaveGames/` littered.
	 */
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
	 * Spends one unit's move, through the rules module's own command path.
	 *
	 * THE DESTINATION IS `ReachableHexes`'s AND NOT THIS FILE'S. The first entry that is not
	 * the unit's own hex is taken -- `reachable` always includes the null move at cost 0, and
	 * moving there would not change the state hash, which would make every clause below
	 * vacuous while looking correct.
	 */
	static bool SpendOneMove(FStratBridge& Bridge, const FStratViewModel& Model, FString& OutError)
	{
		for (const FStratUnitView& U : Model.Units)
		{
			if (U.Side != Model.Match.SideToMove || U.bHasMoved)
			{
				continue;
			}

			TArray<FIntPoint> Reach;
			TArray<int32>     Costs;
			if (!Bridge.ReachableHexes(U.UnitId, Reach, Costs).bOk)
			{
				continue;
			}
			for (const FIntPoint& Hex : Reach)
			{
				if (Hex == U.Hex)
				{
					continue;
				}
				if (Bridge.SubmitMoveToHex(U.UnitId, Hex).bOk)
				{
					return true;
				}
			}
		}
		OutError = TEXT("no unit of the side to move had a legal non-null move on the opening");
		return false;
	}

	/** Every `AStratUnitActor` alive in the world, by the id it says it stands for. Read from
	 *  the WORLD and not from the subsystem's private map -- `StratMatchReconcile.cpp`'s
	 *  reasoning: only an iterator can see an actor the map forgot about. */
	static TSet<int32> LiveUnitIds(UWorld* World)
	{
		TSet<int32> Ids;
		if (World == nullptr)
		{
			return Ids;
		}
		for (TActorIterator<AStratUnitActor> It(World); It; ++It)
		{
			AStratUnitActor* const Actor = *It;
			if (IsValid(Actor) && !Actor->IsActorBeingDestroyed())
			{
				Ids.Add(Actor->GetUnitId());
			}
		}
		return Ids;
	}

	static int32 LiveBoardCount(UWorld* World)
	{
		int32 Count = 0;
		if (World == nullptr)
		{
			return Count;
		}
		for (TActorIterator<AStratBoardActor> It(World); It; ++It)
		{
			AStratBoardActor* const Actor = *It;
			if (IsValid(Actor) && !Actor->IsActorBeingDestroyed())
			{
				++Count;
			}
		}
		return Count;
	}

	static FString Describe(const TSet<int32>& Ids)
	{
		TArray<int32> Sorted = Ids.Array();
		Sorted.Sort();
		TArray<FString> Parts;
		for (const int32 Id : Sorted)
		{
			Parts.Add(FString::FromInt(Id));
		}
		return FString::Join(Parts, TEXT(", "));
	}
}

// ---------------------------------------------------------------------------
// T-SAVE-06 -- a match saved to a slot and loaded back is the same match.
//
// THE SUBJECT IS THE SLOT AND NOT THE FORMAT. `StratBridgeRestoreParity.cpp` already pins the
// §4.10 text; what this adds is everything between the text and the disk -- the payload, the
// three fields §4.10 cannot carry, and `StartMatchInternal`'s one ordered sequence being run
// a second time with a restore wedged into it.
//
// THE COMPARISON IS THE HASH ON EITHER SIDE OF THE ROUND TRIP, captured off the live bridge
// before the save and read off the NEW live bridge after the load. The two bridges are
// different objects -- `LoadMatchFromSlot` frees the first and builds the second -- so nothing
// is being compared against itself.
//
// AND THE PRE-SAVE STATE IS ASSERTED TO BE OFF THE OPENING FIRST, so the equality at the end
// measures the restore. A load that silently re-seeded and replayed nothing would reach the
// OPENING hash, and without this line that would be indistinguishable from success on a match
// where nothing had happened.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSlotRoundTripTest,
	"Stratocracy.StratPlay.T-SAVE-06.SlotRoundTripRestoresTheSameStateHash",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSlotRoundTripTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSaveSlotClauses;

	// The board's own sentence about a fixture with no Blueprint defaults, and the subsystem's
	// report of the presentation gap that follows from it. Declared rather than suppressed.
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

	// The return is false in a fixture with no tile meshes; `IsMatchLive()` is the question.
	Subsystem->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the match is live on the rules side despite the presentation gap"),
			Subsystem->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	FStratBridge* const Live = Subsystem->GetBridge();
	if (!TestNotNull(TEXT("a live match has a bridge"), Live))
	{
		return false;
	}

	const FString OpeningHash = Live->StateHash();

	// ---- take the match somewhere the opening is not -------------------------
	FStratViewModel Model;
	if (!TestTrue(TEXT("the view model builds"), Subsystem->BuildViewModel(Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("one unit's move is spent through the rules module"),
			SpendOneMove(*Live, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	const FString SavedHash  = Live->StateHash();
	const int32   SavedCount = Live->RecordedCommandCount();
	TestNotEqual(
		TEXT("the match has moved off the opening, so the equality at the end measures the "
		     "restore and not the seed"),
		SavedHash, OpeningHash);
	TestTrue(TEXT("and it has a recorded command to replay"), SavedCount > 0);

	// ---- save ----------------------------------------------------------------
	if (!TestTrue(TEXT("T-SAVE-06: a live match saves to a slot"),
			Subsystem->SaveMatchToSlot(FString(), Error)))
	{
		AddError(Error);
		return false;
	}
	TestTrue(TEXT("T-SAVE-06: and the slot now exists"),
		Subsystem->DoesSaveSlotExist(FString()));
	TestEqual(TEXT("an empty slot name resolves to the configured one"),
		Subsystem->ResolveSaveSlotName(FString()), FString(kTestSlotName));

	// ---- load ----------------------------------------------------------------
	// The return is false for the tile-mesh gap again; the bridge is what is asked.
	Subsystem->LoadMatchFromSlot(FString(), Error);
	if (!TestTrue(TEXT("the match is live after the load"), Subsystem->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	FStratBridge* const Reloaded = Subsystem->GetBridge();
	if (!TestNotNull(TEXT("the loaded match has a bridge"), Reloaded))
	{
		return false;
	}

	TestEqual(
		TEXT("T-SAVE-06: the loaded match reaches the canonical state hash the save was taken "
		     "at -- both sides are strat::canonicalStateHash off two different bridges"),
		Reloaded->StateHash(), SavedHash);
	TestEqual(TEXT("T-SAVE-06: and it carries the same recorded log length"),
		Reloaded->RecordedCommandCount(), SavedCount);
	TestEqual(TEXT("T-SAVE-06: the viewing side survived the round trip"),
		Subsystem->GetViewingSide(), kViewingSide);

	return true;
}

// ---------------------------------------------------------------------------
// T-INT-05 -- a LOADED match is drawn from the view model alone, exactly as a fresh one is.
//
// `StratMatchSubsystem.h` states the property and its mechanism: "PRESENTATION IS RECONCILED
// THROUGH THE ONE REFRESH PATH ... T-INT-05's 'rebuild the screen from the view model alone'
// therefore covers a loaded match for free, which it would not if a load had its own drawing
// path." For free is exactly the kind of claim that stops being true silently, so this clause
// measures it.
//
// THE BOARD COUNT IS THE HALF THAT CATCHES THE REGRESSION THIS CLAUSE EXISTS FOR.
// `LoadMatchFromSlot` runs `StartMatchInternal` a SECOND time in one world, and the only thing
// standing between that and two boards -- two overlays, two sets of tiles, one of them
// orphaned and never updated again -- is the unconditional `TearDownPresentation` at the top.
// A guard added to that call, or a reordering that put it after the spawn, produces a world
// with two `AStratBoardActor`s and a screen that looks almost right. So: exactly one.
//
// THE UNIT SET IS READ FROM THE WORLD, not from the subsystem's private map, for
// `StratMatchReconcile.cpp`'s reason: an actor the map had dropped would be invisible to a
// map-side reading, and "destroyed" has to mean destroyed.
//
// AND THE CONTROL IS THE PRE-LOAD WORLD. The board count and the unit set are taken before the
// load as well as after, so "exactly one board" is not being read off a world that never
// managed to spawn one.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratLoadedMatchDrawnFromModelTest,
	"Stratocracy.StratPlay.T-INT-05.LoadedMatchIsDrawnFromTheViewModelAlone",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratLoadedMatchDrawnFromModelTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSaveSlotClauses;

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

	Subsystem->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the match is live"), Subsystem->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	// ---- the control: one board and a populated world BEFORE the load --------
	if (!TestEqual(
			TEXT("CONTROL: a fresh match spawns exactly one board, so the count after the load is "
			     "comparable to something"),
			LiveBoardCount(Scope.World), 1))
	{
		return false;
	}
	const TSet<int32> IdsBefore = LiveUnitIds(Scope.World);
	if (!TestTrue(TEXT("CONTROL: and it spawns unit actors"), IdsBefore.Num() > 0))
	{
		return false;
	}

	FStratBridge* const Live = Subsystem->GetBridge();
	if (!TestNotNull(TEXT("a live match has a bridge"), Live))
	{
		return false;
	}

	FStratViewModel Model;
	if (!TestTrue(TEXT("the view model builds"), Subsystem->BuildViewModel(Model, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("one unit's move is spent, so the load has something to replay"),
			SpendOneMove(*Live, Model, Error)))
	{
		AddError(Error);
		return false;
	}

	if (!TestTrue(TEXT("the match saves"), Subsystem->SaveMatchToSlot(FString(), Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- the load, in the same world -----------------------------------------
	Subsystem->LoadMatchFromSlot(FString(), Error);
	if (!TestTrue(TEXT("the match is live after the load"), Subsystem->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Loaded;
	if (!TestTrue(TEXT("the loaded match's view model builds"),
			Subsystem->BuildViewModel(Loaded, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- exactly one board -----------------------------------------------------
	TestEqual(
		TEXT("T-INT-05: EXACTLY ONE AStratBoardActor exists after a load in the same world. Two "
		     "means TearDownPresentation did not run before the second StartMatchInternal, and "
		     "the orphan is a board nothing updates again"),
		LiveBoardCount(Scope.World), 1);

	// ---- the actor set is exactly the model's ----------------------------------
	TSet<int32> Expected;
	for (const FStratUnitView& U : Loaded.Units)
	{
		Expected.Add(U.UnitId);
	}
	const TSet<int32> Actual = LiveUnitIds(Scope.World);

	TestEqual(
		*FString::Printf(
			TEXT("T-INT-05: the loaded world holds exactly as many unit actors as the model has "
			     "units. Model: [%s]. World: [%s]"),
			*Describe(Expected), *Describe(Actual)),
		Actual.Num(), Expected.Num());

	for (const int32 Id : Expected)
	{
		TestTrue(
			*FString::Printf(TEXT("T-INT-05: the model's unit %d is drawn by an actor"), Id),
			Actual.Contains(Id));
		TestNotNull(
			*FString::Printf(TEXT("T-INT-05: and the subsystem's map agrees about unit %d"), Id),
			Subsystem->FindUnitActor(Id));
	}
	for (const int32 Id : Actual)
	{
		TestTrue(
			*FString::Printf(
				TEXT("T-INT-05: actor for unit %d corresponds to a unit the model names -- a "
				     "survivor of the previous match would land here"),
				Id),
			Expected.Contains(Id));
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-04 -- a slot written at a `SavedDataVersion` this build does not read is REFUSED,
// by name, and the match already on screen survives the refusal.
//
// `UStratSaveGame`'s header calls this the irreversible part: "A `USaveGame` whose shape
// changes without a version is the classic way a slot becomes unreadable while reporting
// success: `LoadGameFromSlot` happily returns an object with default-constructed new fields
// and nothing says so."
//
// THE SECOND HALF DIVERGES FROM WHAT WAS PROPOSED, AND THE CODE IS WHY. This clause was
// specified as "`IsMatchLive()` false". It is not, and it should not be: the version gate in
// `LoadMatchFromSlot` returns BEFORE `StartMatchInternal`, so `TearDownPresentation` never
// runs and the match that was on screen is untouched. That is a strictly better property than
// the one proposed -- `StratMatchSubsystem.h` records "ON A REFUSAL THE PREVIOUS MATCH IS
// GONE" as a LIMITATION of the refusals that happen INSIDE the sequence, and this refusal is
// one of the ones that does not. So the clause pins survival: live, same bridge state, same
// recorded log. A future refactor that moved the version check inside the sequence would turn
// a harmless refusal into a destructive one, and this is what would say so.
//
// THE VERSION IS `kCurrentSavedDataVersion + 1` AND NOT A LITERAL, so the clause keeps meaning
// "a version this build does not read" after the constant is bumped.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratLoadRefusesWrongSavedDataVersionTest,
	"Stratocracy.StratPlay.T-SAVE-04.LoadRefusesAWrongSavedDataVersion",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratLoadRefusesWrongSavedDataVersionTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSaveSlotClauses;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
	AddExpectedMessagePlain(TEXT("Load refused"), ELogVerbosity::Warning,
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

	Subsystem->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the match is live"), Subsystem->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	// A real save first, so the slot is otherwise valid and the version is the ONLY thing
	// wrong with it. A hand-built payload with an empty `SaveText` would be refused by a
	// different arm and this clause would pass for the wrong reason.
	if (!TestTrue(TEXT("a real save is written first"),
			Subsystem->SaveMatchToSlot(FString(), Error)))
	{
		AddError(Error);
		return false;
	}

	UStratSaveGame* const Payload =
		Cast<UStratSaveGame>(UGameplayStatics::LoadGameFromSlot(kTestSlotName, kUserIndex));
	if (!TestNotNull(TEXT("the slot holds a UStratSaveGame"), Payload))
	{
		return false;
	}
	if (!TestTrue(TEXT("and it carries §4.10 text, so the version is the only defect"),
			!Payload->SaveText.IsEmpty()))
	{
		return false;
	}

	const int32 FutureVersion = UStratSaveGame::kCurrentSavedDataVersion + 1;
	Payload->SavedDataVersion = FutureVersion;
	if (!TestTrue(TEXT("the retagged payload writes back"),
			UGameplayStatics::SaveGameToSlot(Payload, kTestSlotName, kUserIndex)))
	{
		return false;
	}

	FStratBridge* const Before = Subsystem->GetBridge();
	if (!TestNotNull(TEXT("there is a live bridge to protect"), Before))
	{
		return false;
	}
	const FString BeforeHash  = Before->StateHash();
	const int32   BeforeCount = Before->RecordedCommandCount();

	// ---- the refusal -----------------------------------------------------------
	FString Reason;
	TestFalse(TEXT("T-SAVE-04: a slot at an unreadable SavedDataVersion is refused"),
		Subsystem->LoadMatchFromSlot(FString(), Reason));

	TestTrue(
		*FString::Printf(
			TEXT("T-SAVE-04: the reason names the version the slot was WRITTEN at (%d); it was: %s"),
			FutureVersion, *Reason),
		Reason.Contains(FString::FromInt(FutureVersion), ESearchCase::CaseSensitive));
	TestTrue(
		*FString::Printf(
			TEXT("T-SAVE-04: and the version this build READS (%d), so a reader need not look "
			     "either number up"),
			UStratSaveGame::kCurrentSavedDataVersion),
		Reason.Contains(FString::FromInt(UStratSaveGame::kCurrentSavedDataVersion),
			ESearchCase::CaseSensitive));

	// ---- and the match on screen is untouched ----------------------------------
	TestTrue(
		TEXT("T-SAVE-04: the version gate refuses BEFORE StartMatchInternal, so the match already "
		     "on screen survives -- this refusal is not one of the destructive ones "
		     "StratMatchSubsystem.h warns about"),
		Subsystem->IsMatchLive());

	FStratBridge* const After = Subsystem->GetBridge();
	if (TestNotNull(TEXT("and the bridge is still there"), After))
	{
		TestEqual(TEXT("T-SAVE-04: at the same canonical state"), After->StateHash(), BeforeHash);
		TestEqual(TEXT("T-SAVE-04: with the same recorded log"),
			After->RecordedCommandCount(), BeforeCount);
	}

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-06 -- saving over a slot preserves §2.11.6's onboarding state.
//
// `SaveMatchToSlot` reads the existing slot before writing, and its own comment says why:
// "A fresh `UStratSaveGame` every time would silently reset `bHasCompletedAMatch` and re-arm
// every one-shot tip on the first mid-match save -- a guidance bug with a save-system cause."
// That is the clause. It is what makes the guidance suppression path safe, because the
// suppression reads exactly the field a careless save would clear.
//
// THE TWO VALUES ARE PLANTED, AND THAT IS STATED RATHER THAN HIDDEN. Nothing in the tree
// writes either field yet -- `UStratSaveGame`'s header records the debt for both and names
// what discharges each. So the clause writes a slot by hand with both set, and then asserts
// that the SUBSYSTEM's save carried them across. The planted values are the INPUT; the output
// asserted is a property of `SaveMatchToSlot`.
//
// THE CONTROL IS THAT EVERYTHING ELSE WAS OVERWRITTEN. Without it, a `SaveMatchToSlot` that
// silently did nothing at all -- returned true and never wrote -- would pass every assertion
// about preservation. So the planted payload carries a recognisably WRONG `ScenarioFile` and
// `SaveText`, and both are required to have been replaced by the live match's.
//
// AND `HasCompletedAMatchOnSave` IS ASKED AFTERWARDS, because that is the reader the guidance
// layer actually uses; a field preserved in the payload but not visible through the accessor
// would preserve nothing that matters.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratSaveOverPreservesOnboardingTest,
	"Stratocracy.StratPlay.T-SAVE-06.SaveOverASlotPreservesOnboardingState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratSaveOverPreservesOnboardingTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSaveSlotClauses;

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

	// ---- plant a slot carrying onboarding state and nothing else that is right ----
	static const FName kPlantedTipId = TEXT("automation.planted.tip");
	static const TCHAR* kWrongScenario = TEXT("Data/a_scenario_this_match_is_not_using.json");
	static const TCHAR* kWrongText     = TEXT("{ \"this\": \"is not the live match's save text\" }");

	{
		UStratSaveGame* const Planted = Cast<UStratSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UStratSaveGame::StaticClass()));
		if (!TestNotNull(TEXT("a UStratSaveGame can be created"), Planted))
		{
			return false;
		}

		Planted->bHasCompletedAMatch = true;
		Planted->FiredOneShotTipIds.Add(kPlantedTipId);
		Planted->ScenarioFile = kWrongScenario;
		Planted->SaveText     = kWrongText;

		if (!TestTrue(TEXT("the planted slot writes"),
				UGameplayStatics::SaveGameToSlot(Planted, kTestSlotName, kUserIndex)))
		{
			return false;
		}
	}

	// ---- a live match, saved over that slot ----------------------------------
	FStratMatchConfig Config;
	FString Error;
	if (!TestTrue(TEXT("the match config assembles"), MakeConfig(Config, Error)))
	{
		AddError(Error);
		return false;
	}

	Subsystem->StartMatch(Config, Error);
	if (!TestTrue(TEXT("the match is live"), Subsystem->IsMatchLive()))
	{
		AddError(Error);
		return false;
	}

	// The reader the guidance layer uses, asked BEFORE the save-over as well, so the answer
	// after it is a preservation rather than a coincidence.
	if (!TestTrue(
			TEXT("CONTROL: the planted slot already reports a completed match through the "
			     "accessor FStratGuidedOpening's suppression is driven from"),
			Subsystem->HasCompletedAMatchOnSave(FString())))
	{
		return false;
	}

	if (!TestTrue(TEXT("the live match saves over the planted slot"),
			Subsystem->SaveMatchToSlot(FString(), Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- what the slot holds now ----------------------------------------------
	UStratSaveGame* const After =
		Cast<UStratSaveGame>(UGameplayStatics::LoadGameFromSlot(kTestSlotName, kUserIndex));
	if (!TestNotNull(TEXT("the slot still holds a UStratSaveGame"), After))
	{
		return false;
	}

	// The control first: the save really did write, so "preserved" is not "untouched".
	TestEqual(
		TEXT("CONTROL: ScenarioFile was OVERWRITTEN with the live match's -- so SaveMatchToSlot "
		     "demonstrably wrote, and the preservation below is a carry-forward and not a no-op"),
		After->ScenarioFile, Config.ScenarioFile);
	TestFalse(TEXT("CONTROL: and the planted §4.10 text is gone"),
		After->SaveText.Equals(FString(kWrongText), ESearchCase::CaseSensitive));
	TestTrue(TEXT("CONTROL: replaced by text a real serializer produced"),
		!After->SaveText.IsEmpty());
	TestEqual(TEXT("CONTROL: and the payload is stamped at this build's shape version"),
		After->SavedDataVersion, UStratSaveGame::kCurrentSavedDataVersion);

	// ---- the two onboarding fields survived ------------------------------------
	TestTrue(
		TEXT("T-SAVE-06: §2.11.6's bHasCompletedAMatch survives a save-over -- the field the "
		     "guided opening's suppression reads"),
		After->bHasCompletedAMatch);
	TestTrue(
		*FString::Printf(
			TEXT("T-SAVE-06: §2.11.6's fired one-shot tip id '%s' survives a save-over, so a tip "
			     "does not re-arm on the first mid-match save"),
			*kPlantedTipId.ToString()),
		After->FiredOneShotTipIds.Contains(kPlantedTipId));
	TestEqual(TEXT("T-SAVE-06: and nothing else crept into the tip set"),
		After->FiredOneShotTipIds.Num(), 1);

	TestTrue(
		TEXT("T-SAVE-06: and the accessor still reports it, which is what the guidance layer "
		     "actually asks"),
		Subsystem->HasCompletedAMatchOnSave(FString()));

	return true;
}

// ---------------------------------------------------------------------------
// T-SAVE-04 -- an unconfigured subsystem refuses a load by name, and does not invent a
// configuration to load with.
//
// `StratMatchSubsystem.h`: "SO IT REQUIRES A CONFIGURED SUBSYSTEM. `StartMatch` must have run
// at least once -- which `AStratGameMode::BeginPlay` does -- or there are no tables to seed
// from, and this refuses by name rather than seeding from a default-constructed config."
//
// THE FAILURE THIS PREVENTS IS NOT A CRASH. A `LoadMatchFromSlot` that fell through to
// `StartMatchInternal` with a default `FStratMatchConfig` would be refused there too -- by the
// "definition tables are not assigned" arm -- but it would have run `TearDownPresentation`
// first and constructed a bridge, and the reason a caller got back would name the GameMode's
// defaults rather than the missing `StartMatch`. Those have different fixes, which is the
// whole reason the two checks are separate.
//
// SO THE CLAUSE ASSERTS THE WORDS. The refusal must name the precondition -- that `StartMatch`
// has never run -- and not merely the tables, and it must do so on a subsystem that has NO
// bridge and NO live match, before and after.
//
// AND A REAL SLOT EXISTS WHILE IT IS ASKED, planted from a second world that DID configure.
// Without it the refusal could be the "no save exists in slot" arm, and the clause would be
// pinning a different sentence.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratLoadRefusesUnconfiguredSubsystemTest,
	"Stratocracy.StratPlay.T-SAVE-04.LoadRefusesAnUnconfiguredSubsystem",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratLoadRefusesUnconfiguredSubsystemTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratSaveSlotClauses;

	AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
	AddExpectedMessagePlain(TEXT("Load refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FSlotScope Slot;

	// ---- a real slot, written from a world that DID configure ----------------
	{
		FTestWorldScope Donor;
		if (!TestNotNull(TEXT("a donor world was created"), Donor.World))
		{
			return false;
		}
		UStratMatchSubsystem* const DonorSubsystem =
			Donor.World->GetSubsystem<UStratMatchSubsystem>();
		if (!TestNotNull(TEXT("the donor world has a match subsystem"), DonorSubsystem))
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
		DonorSubsystem->StartMatch(Config, Error);
		if (!TestTrue(TEXT("the donor match is live"), DonorSubsystem->IsMatchLive()))
		{
			AddError(Error);
			return false;
		}
		if (!TestTrue(TEXT("the donor writes a real slot"),
				DonorSubsystem->SaveMatchToSlot(FString(), Error)))
		{
			AddError(Error);
			return false;
		}
	}

	// ---- the subject: a world whose subsystem has never been configured -------
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
	if (!TestFalse(TEXT("and StartMatch has never run on it"), Subsystem->IsMatchLive()))
	{
		return false;
	}
	if (!TestNull(TEXT("so it holds no bridge"), Subsystem->GetBridge()))
	{
		return false;
	}

	// The slot is named explicitly, because an EMPTY name would resolve through
	// `ActiveConfig.SaveSlotName`, which on an unconfigured subsystem is the C++ default
	// rather than this file's slot -- and the clause would then be pinning "no save exists".
	if (!TestTrue(TEXT("CONTROL: a real slot exists to be refused"),
			UGameplayStatics::DoesSaveGameExist(kTestSlotName, kUserIndex)))
	{
		return false;
	}

	FString Reason;
	TestFalse(TEXT("T-SAVE-04: an unconfigured subsystem refuses a load"),
		Subsystem->LoadMatchFromSlot(FString(kTestSlotName), Reason));

	TestTrue(
		*FString::Printf(
			TEXT("T-SAVE-04: and the reason names the PRECONDITION -- that this subsystem has "
			     "never been configured -- rather than blaming the GameMode's table properties, "
			     "which is a different fix. It was: %s"),
			*Reason),
		Reason.Contains(TEXT("never been configured"), ESearchCase::CaseSensitive));
	TestTrue(TEXT("T-SAVE-04: and it says what is missing, in the subsystem's own words"),
		Reason.Contains(TEXT("definition tables"), ESearchCase::CaseSensitive));

	// ---- and it did not seed from a default-constructed config ----------------
	TestFalse(TEXT("T-SAVE-04: no match came into existence"), Subsystem->IsMatchLive());
	TestNull(TEXT("T-SAVE-04: and no bridge was constructed on the way to the refusal"),
		Subsystem->GetBridge());
	TestEqual(TEXT("T-INT-05: nothing was drawn either"), LiveBoardCount(Scope.World), 0);
	TestEqual(TEXT("T-INT-05: and no unit actor was spawned"),
		LiveUnitIds(Scope.World).Num(), 0);

	return true;
}
