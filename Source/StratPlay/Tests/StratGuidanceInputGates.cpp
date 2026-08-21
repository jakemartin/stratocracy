// The gate on §2.11.6-B's Q27 input gates -- `AStratPlayerController::HandleSelectionEvent`'s
// three guidance arms, under T-UI-02.
//
// ON THE ACCEPTANCE ID. `T-UI-02` is used here on `StratPlayerControllerTick.cpp`'s precedent
// and for its reason: it is the ID this tree already uses for the player-input surface in
// `StratPlay`, every ID used in this directory is a real GDD ID, and no test in this tree
// mints one. `strat-data-steward` should confirm or reassign. The GDD calls beat 1a's End Turn
// gate "the only guided-opening constraint that gates a player INPUT rather than a selection,
// adopted under Q27, ruled".
//
// WHAT IS PINNED, AND WHY EACH ASSERTION IS THE ONE IT IS.
//   - End Turn: `FStratBridge::RecordedCommandCount()` is UNCHANGED. Not "the call returned
//     false" -- a gate that submitted the EndTurn and then reported a failure would satisfy
//     that and would end the player's turn anyway.
//   - Wait: `FStratSelectionMachine::IsDone(marked)` stays FALSE. Not merely "nothing was
//     submitted" -- Wait never submits anything, so a Wait gate that let the machine see the
//     event would pass a submission-only clause while RETIRING the unit, and a retired marked
//     Infantry with End Turn still inert is §2.11.6's deadlock exactly: no move left to
//     satisfy the gate and no way to end the turn.
//   - The silence clause carries its own control. See its block.
//
// WHERE THE EXPECTATIONS COME FROM. Every count is `RecordedCommandCount()`; every DONE bit is
// the machine's `IsDone`; the marked unit is `FStratUnitView::bIsGuidedMarked`; every
// destination is `FStratBridge::ReachableHexes`, so no coordinate is spelled in this file and
// a scenario edit does not need an edit here. The one literal is the gate's player-facing
// sentence, and it is compared against `FStratGuidedOpening::EndTurnGateHoverText()` rather
// than against a copy of the GDD string -- so a typo in that string is the engineer's review
// question and the gate reporting the WRONG sentence is this file's.
//
// THIS FIXTURE HAS NO TILE MESHES AND NO INPUT ASSETS, and the two behave DIFFERENTLY in the
// log. The board's missing tile meshes DO warn, once per terrain kind, and every clause
// declares that warning expected. The unset input assets do NOT warn, because
// `SetupInputComponent` never runs on an unpossessed controller in a world that has not begun
// play -- so there is nothing to declare, and declaring it anyway is itself a failure
// (`Occurrences = 0` means "at least once"). `DeclareFixtureWarnings` carries that measurement
// at the one place it is acted on.
//
// THAT SENTENCE USED TO SAY "both produce warnings that are declared expected in every clause",
// which was written before the measurement and was false in the second half. It is corrected
// rather than marked, because it was never a quotation of a claim worth preserving -- it was
// simply wrong about the log.
//
// NEITHER MATTERS TO WHAT IS PINNED: `HandleSelectionEvent` is a plain method and this file
// calls it directly rather than through Enhanced Input. What that costs is stated rather than
// hidden -- see the "WHAT THIS FILE DOES NOT PIN" list below.
//
// THIS FILE USES THE `RETRACTED> ` MARKER CONVENTION declared in
// `Source/StratPlay/Tests/StratSelectionMachineParity.cpp`. Every comment line that QUOTES a
// claim this file no longer makes begins, after `//`, with that exact token; no line carrying
// it asserts anything, so a stale-phrase sweep subtracts the withdrawn quotations with
// `grep -n '<phrase>' <file> | grep -v 'RETRACTED> '`. TWO passages in this file said the
// target overlay was unobservable -- the list below and `AttackIsClosedForTheMarkedInfantry`'s
// own block -- and both are marked. That there were two is the point: the previous sweep of
// exactly this kind, on `StratSelectionMachineParity.cpp`, found three passages and one of
// them was nearly missed, so this one was done by grep over the whole file rather than by
// reading the clause that seemed most likely to hold it.
//
// WHAT THIS FILE DOES NOT PIN, recorded because a half-pinned clause closes a phase as
// silently as an untested method:
//   - THE BINDING FROM A KEY TO `HandleSelectionEvent`. `SetupInputComponent` runs only on a
//     possessed controller in a begun-play world; `StratPlayerControllerTick.cpp` pins the one
//     actor flag that silently disables the whole pipeline, and nothing pins the bindings
//     themselves. Unchanged by this file.
//   - THE ATTACK ARM AT THE CONTROLLER. See `AttackIsClosedForTheMarkedInfantry` below, which
//     states exactly why it stops where it does and what would let it go further. Its item 1 is
//     no longer an inference: `T-UI-02.TheMarkedInfantryHasNoAttackTargetAtDeployment` measures
//     the marked Infantry's empty target set through `FStratBridge::AttackTargetHexes`, with a
//     control, and the arm stays unreachable for a reason the rules module states rather than a
//     reason this file read off the map.
//   - THE TARGET OVERLAY -- WITHDRAWN 2026-08-21. This list used to carry, in this file's own
//     voice:
//
// RETRACTED>   "THE TARGET OVERLAY BEING CLEARED. `AStratBoardActor::TargetOverlay` is
// RETRACTED>    protected and the class exposes no count for it -- `GetDrawnHexCount` is the
// RETRACTED>    TILE count."
//
//     `AStratBoardActor::GetTargetOverlayCount()` landed, `BlueprintPure`, reading the
//     component rather than a cached number. `AttackIsClosedForTheMarkedInfantry` now measures
//     the overlay through it, WITH a control, and states in its own block the one thing that
//     measurement still cannot attribute.
//
// NO `strat::` FREE FUNCTION IS CALLED -- LNK2019 outside StratBridge and StratRules.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "HAL/CriticalSection.h"
#include "Kismet/GameplayStatics.h"
#include "Math/IntPoint.h"
#include "Misc/OutputDevice.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/UnrealType.h"

#include "StratBoardActor.h"
#include "StratGuidedOpening.h"
#include "StratMatchSubsystem.h"
#include "StratPlayerController.h"
#include "StratSelectionMachine.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

// Included from a .cpp and never from a UHT-parsed header.
#include "StratBridge.h"

namespace StratGuidanceInputGates
{
	static const int32 kFirstSide   = 0;
	static const int32 kViewingSide = 0;

	/**
	 * A slot name that does not exist, so `HasCompletedAMatchOnSave` answers false and the
	 * guided opening ARMS.
	 *
	 * NOT THE CONFIGURED DEFAULT, and the reason is a real hazard rather than tidiness: the
	 * shipped `SaveSlotName` is `StratocracyMatch`, the player's slot, and a developer who had
	 * finished a match would have `bHasCompletedAMatch` true in it. Every clause in this file
	 * would then arm no guidance, every gate would be open, and every clause would PASS -- a
	 * whole file going quietly vacuous depending on whose machine ran it.
	 */
	static const TCHAR* kAbsentSlotName = TEXT("StratocracyAutomation_NoSuchSlot_InputGates");

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
		Out.SaveSlotName    = kAbsentSlotName;
		return true;
	}

	/** The transient world. See `StratMatchReconcile.cpp` on why the context and the
	 *  destruction order are not decoration. */
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
	 * A live match, a spawned controller, and the marked Infantry located.
	 *
	 * THE CONTROLLER IS SPAWNED AND NOT POSSESSED, and the world has not begun play, so
	 * `BeginPlay` and `SetupInputComponent` never run. That is deliberate:
	 * `HandleSelectionEvent` is the subject and it reads only the world's subsystem, its own
	 * two members, and the model it builds. What is NOT covered by this arrangement is stated
	 * in the file's header block rather than left to be discovered.
	 */
	struct FGateHarness
	{
		FTestWorldScope         Scope;
		UStratMatchSubsystem*   Match      = nullptr;
		AStratPlayerController* Controller = nullptr;
		FStratBridge*           Bridge     = nullptr;
		int32                   MarkedId   = INDEX_NONE;
		FIntPoint               MarkedHex  = FIntPoint::ZeroValue;

		bool Build(FString& OutError)
		{
			if (Scope.World == nullptr)
			{
				OutError = TEXT("the transient world was not created");
				return false;
			}

			Match = Scope.World->GetSubsystem<UStratMatchSubsystem>();
			if (Match == nullptr)
			{
				OutError = TEXT("the world has no UStratMatchSubsystem");
				return false;
			}

			FStratMatchConfig Config;
			if (!MakeConfig(Config, OutError))
			{
				return false;
			}

			// The return is false in a fixture with no tile meshes; `IsMatchLive()` is the
			// question -- `StratMatchSubsystem.h` splits all-or-nothing on the rules side from
			// not-all-or-nothing on the presentation side.
			Match->StartMatch(Config, OutError);
			if (!Match->IsMatchLive())
			{
				return false;
			}

			Bridge = Match->GetBridge();
			if (Bridge == nullptr)
			{
				OutError = TEXT("a live match has no bridge");
				return false;
			}

			Controller = Scope.World->SpawnActor<AStratPlayerController>();
			if (Controller == nullptr)
			{
				OutError = TEXT("AStratPlayerController did not spawn");
				return false;
			}

			FStratViewModel Model;
			if (!Match->BuildViewModel(Model, OutError))
			{
				return false;
			}
			for (const FStratUnitView& U : Model.Units)
			{
				if (U.Side == Match->GetViewingSide() && U.bIsGuidedMarked)
				{
					MarkedId  = U.UnitId;
					MarkedHex = U.Hex;
					break;
				}
			}
			if (MarkedId == INDEX_NONE)
			{
				OutError = TEXT("the shipped scenario marks no Infantry for the viewing seat");
				return false;
			}
			return true;
		}

		/** One event through the production path. Returns what the controller returned. */
		bool Send(EStratSelectionEvent Event, FIntPoint Hex, FString& OutReason)
		{
			return Controller->HandleSelectionEvent(Event, Hex, OutReason);
		}

		/**
		 * Beat 1a's own move, through the controller: select the marked Infantry, then click a
		 * hex the RULES MODULE says it can reach.
		 *
		 * IT CANCELS FIRST, AND THAT IS A MEASUREMENT RATHER THAN HYGIENE. Without it this
		 * helper failed three clauses in this file. `EStratSelectionEvent::HexPrimary` on the
		 * hex of the ALREADY-SELECTED unit does not re-select it -- it drops the selection --
		 * so a caller that had already selected the marked Infantry (which the Wait clause and
		 * the silence clause both had, because selecting it is how they reach the Wait gate)
		 * ended up with nothing selected, every subsequent click a no-op, and a helper that
		 * reported success. A `Cancel` makes the starting state the same however the caller
		 * arrived, which is what a shared helper owes its callers.
		 *
		 * SUCCESS IS MEASURED AND NOT READ OFF THE RETURN VALUE, for the reason that failure
		 * exposed. `HandleSelectionEvent` returns TRUE for a SELECTION as readily as for a
		 * submitted move -- `EStratSelectionCommand::None` is an ordinary outcome and not a
		 * failure -- so the bool cannot tell "moved" from "selected something" from "clicked
		 * empty ground". `FStratBridge::RecordedCommandCount()` can, and it is the same
		 * instrument the clauses themselves assert on.
		 *
		 * THE DESTINATION IS `ReachableHexes`'s. Entries equal to the unit's own hex are
		 * skipped -- `reachable` always includes the null move at cost 0, and moving there
		 * would not set `bHasMoved`, so the gate would never open and every control below
		 * would be measuring nothing.
		 */
		bool MoveTheMarkedInfantry(FString& OutError)
		{
			FString Reason;
			Send(EStratSelectionEvent::Cancel, FIntPoint::ZeroValue, Reason);

			TArray<FIntPoint> Reach;
			TArray<int32>     Costs;
			const FStratResult Asked = Bridge->ReachableHexes(MarkedId, Reach, Costs);
			if (!Asked.bOk)
			{
				OutError = FString::Printf(TEXT("ReachableHexes refused: %s"), *Asked.Reason);
				return false;
			}

			const int32 Before = Bridge->RecordedCommandCount();

			for (const FIntPoint& Hex : Reach)
			{
				if (Hex == MarkedHex)
				{
					continue;
				}

				// Re-selected before every attempt: a click that was not a legal move may have
				// dropped the selection, and the next click would then select rather than move.
				Send(EStratSelectionEvent::Cancel, FIntPoint::ZeroValue, Reason);
				Send(EStratSelectionEvent::HexPrimary, MarkedHex, Reason);
				if (Controller->GetSelectionMachine().GetSelectedUnitId() != MarkedId)
				{
					OutError = FString::Printf(
						TEXT("the marked Infantry did not select at %s: %s"),
						*MarkedHex.ToString(), *Reason);
					return false;
				}

				Send(EStratSelectionEvent::HexPrimary, Hex, Reason);

				if (Bridge->RecordedCommandCount() > Before)
				{
					MarkedHex = Hex;
					return true;
				}
			}

			OutError = TEXT("no non-null destination in the marked Infantry's reach produced a "
			                "recorded command");
			return false;
		}
	};

	/**
	 * Captures every `STRAT-CMD` line, however it is cased.
	 *
	 * THE FILTER IS LOOSE AND THE ASSERTION IS STRICT, deliberately and on a measurement this
	 * project has already paid for: a case-SENSITIVE filter makes a wrongly-cased defect
	 * vanish before the claim can see it, and the clause then reports silence that is the
	 * instrument's rather than the code's. So this admits `strat-cmd` in any casing and the
	 * clause below compares the exact bytes.
	 *
	 * `CanBeUsedOnMultipleThreads` IS NOT OPTIONAL. Measured 2026-08-14: a clause failed 1 run
	 * in 4 on byte-identical code without it, because `FOutputDeviceRedirector` drains its
	 * buffered queue to whichever devices are registered AT DRAIN TIME. The override routes
	 * this device to `UnbufferedOutputDevices`, fed only by the synchronous broadcast inside
	 * the emitting `UE_LOG`, so the window it sees equals this object's lifetime by
	 * construction rather than by timing. Removing this line reintroduces the flake.
	 */
	struct FStratCmdCapture final : public FOutputDevice
	{
		TArray<FString> Lines;

		FStratCmdCapture()
		{
			if (GLog != nullptr)
			{
				GLog->AddOutputDevice(this);
			}
		}

		virtual ~FStratCmdCapture()
		{
			if (GLog != nullptr)
			{
				GLog->RemoveOutputDevice(this);
			}
		}

		virtual bool CanBeUsedOnMultipleThreads() const override { return true; }

		virtual void Serialize(const TCHAR* Message, ELogVerbosity::Type /*Verbosity*/,
		                       const FName& /*Category*/) override
		{
			const FString Line(Message);
			// Case-INSENSITIVE on purpose. See the block above.
			if (Line.Contains(TEXT("STRAT-CMD"), ESearchCase::IgnoreCase))
			{
				FScopeLock Lock(&Mutex);
				Lines.Add(Line);
			}
		}

		void Settle()
		{
			if (GLog != nullptr)
			{
				GLog->Flush();
			}
		}

		/** Case-SENSITIVE. The claim is strict even though the instrument is not. */
		int32 CountExact(const TCHAR* Phrase) const
		{
			int32 Count = 0;
			for (const FString& Line : Lines)
			{
				if (Line.Contains(FString(Phrase), ESearchCase::CaseSensitive))
				{
					++Count;
				}
			}
			return Count;
		}

	private:
		FCriticalSection Mutex;
	};

	/**
	 * Gives the subsystem-spawned board an overlay mesh, so `GetTargetOverlayCount()` can be
	 * non-zero at all.
	 *
	 * WITHOUT THIS THE ACCESSOR IS MUTE, AND MUTE READS AS PASSING. `AStratBoardActor::
	 * FillOverlay` returns early when the overlay component has no static mesh -- "Nothing to
	 * instance", silent by design -- so on a fixture with no Blueprint defaults
	 * `GetTargetOverlayCount()` is 0 for every input, forever. A clause asserting "the overlay
	 * is empty" against that instrument would be unfalsifiable, which is the precise failure
	 * the control in `AttackIsClosedForTheMarkedInfantry` exists to make impossible.
	 *
	 * `OverlayMesh` IS APPLIED AT `BeginPlay` AND NOT IN THE CONSTRUCTOR -- the board's own
	 * block records why (the property is a Blueprint default and a constructor runs on the
	 * CDO). The subsystem spawns the board into a world that has not begun play, so the
	 * property is set here and `DispatchBeginPlay` is the supported route, exactly as
	 * `StratBoardPicking.cpp` does it. Guarded on `HasActorBegunPlay` so a fixture that ever
	 * does begin play does not dispatch twice.
	 *
	 * SET ON THE INSTANCE AND NEVER ON THE CDO. A `GetDefaultObject()` write would leak this
	 * mesh into every other clause in the suite that spawns a board.
	 */
	static bool GiveTheBoardAnOverlayMesh(AStratBoardActor* Board, FString& OutError)
	{
		if (Board == nullptr)
		{
			OutError = TEXT("the match spawned no board to give an overlay mesh to");
			return false;
		}

		UStaticMesh* const Mesh =
			LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
		if (Mesh == nullptr)
		{
			OutError = TEXT("/Engine/BasicShapes/Cube.Cube did not load");
			return false;
		}

		FObjectPropertyBase* const Property =
			FindFProperty<FObjectPropertyBase>(Board->GetClass(), FName(TEXT("OverlayMesh")));
		if (Property == nullptr)
		{
			OutError = TEXT("no reflected object property 'OverlayMesh' on AStratBoardActor");
			return false;
		}
		Property->SetObjectPropertyValue(Property->ContainerPtrToValuePtr<void>(Board), Mesh);

		if (!Board->HasActorBegunPlay())
		{
			Board->DispatchBeginPlay();
		}
		return true;
	}

	/** The warnings this fixture legitimately produces. Declared, never suppressed. */
	static void DeclareFixtureWarnings(FAutomationTestBase& Test)
	{
		Test.AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
			EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
		// NO DECLARATION FOR THE FIVE "<Action> is unset" WARNINGS, and the absence is a
		// measurement rather than an omission. One was written here defensively, on the theory
		// that a declaration for a line that does not fire is free. It is not:
		// `Occurrences = 0` means "any number, but AT LEAST ONE", so the declaration failed
		// three clauses in this file with "Expected suppressed ('Warning') level log message or
		// higher matching 'is unset' did not occur." `SetupInputComponent` never runs on an
		// unpossessed controller in a world that has not begun play, so the warnings never fire
		// and there is nothing to declare. A fixture that DID possess the controller would add
		// the declaration then, and would be right to.
	}

	// -----------------------------------------------------------------------
	// The control for `AttackTargetHexes`' empty answer, and the reason it is this elaborate.
	//
	// AN EMPTY RESULT PROVES NOTHING ON ITS OWN. `FStratBridge::AttackTargetHexes` returning
	// no hexes for the marked Infantry is a fact about the BOARD only if the same method can
	// be shown returning hexes for some unit somewhere. A method that returned empty for
	// everything -- a broken enumeration, a `Forecast` asked about the wrong side, an
	// `AttackerId` never resolved -- answers the marked Infantry in exactly the same way.
	//
	// THE SHIPPED SCENARIO HAS NO CONTACT AT DEPLOYMENT, so the control cannot be taken on the
	// opening position: `Data/ferrum_crossing.json` puts the two sides at opposite ends of the
	// map, which is the same authored fact §2.11.6 prices the guided lane against. Contact has
	// to be PLAYED INTO EXISTENCE, and the only mover this suite has is the rules module's own
	// AI.
	//
	// SO: A SECOND, PRIVATE MATCH WITH BOTH SIDES AI, ADVANCED IN SMALL BATCHES AND SCANNED
	// AFTER EACH. `AiMaxConsecutiveTurns` is set low so `RunAiTurnsNow` hands control back
	// often enough for a scan to land while two units are still in contact -- one unbounded
	// call plays the whole game and returns at the finished position, where nothing is legal
	// any more.
	//
	// IT RUNS IN ITS OWN WORLD. The measurement is taken on the guidance fixture's board and
	// nothing in here touches it; a control that mutated its own subject would not be one.
	//
	// EVERY BATCH THAT ENDS ON THE BOUND LOGS `STRAT-AI refused phase=handover`, so a clause
	// using this MUST declare that warning expected. That is a measured consequence of the
	// small bound rather than a defect: `RunAiTurnsNow` reports reaching its outer bound
	// instead of stopping silently, which is exactly what it should do.
	// -----------------------------------------------------------------------

	/** Two AI turns per call, so a scan lands between turns rather than after the game. */
	static const int32 kControlTurnsPerBatch = 2;

	/** Batches before the search gives up. `T-INT-05.BothSidesAiReachesAResultWithinTheBound`
	 *  measures a whole AI-vs-AI game as fitting inside 64 turns; this is that with room. */
	static const int32 kControlMaxBatches = 40;

	/**
	 * The control's own save slot, kept away from `kAbsentSlotName`.
	 *
	 * The control plays a real AI-vs-AI match, and a match that reaches a result now WRITES
	 * §2.11.6's completion flag through `ApplyView`'s hook. Written to `kAbsentSlotName` that
	 * would disarm the guided opening for every other clause in this file, on the next run and
	 * every run after -- a whole file going vacuous because of a control.
	 */
	static const TCHAR* kControlSlotName = TEXT("StratocracyAutomation_AttackControl_InputGates");
	static const int32  kControlUserIndex = 0;

	/** Deletes the control's slot on both ends. `StratSaveSlotClauses.cpp` owns the reasoning
	 *  for doing it twice rather than once. */
	struct FControlSlotScope
	{
		FControlSlotScope()  { Clear(); }
		~FControlSlotScope() { Clear(); }

		static void Clear()
		{
			if (UGameplayStatics::DoesSaveGameExist(kControlSlotName, kControlUserIndex))
			{
				UGameplayStatics::DeleteGameInSlot(kControlSlotName, kControlUserIndex);
			}
		}

		FControlSlotScope(const FControlSlotScope&) = delete;
		FControlSlotScope& operator=(const FControlSlotScope&) = delete;
	};

	static bool FindABoardPositionWithALegalAttack(FString& OutDescription, FString& OutError)
	{
		FControlSlotScope SlotScope;
		FTestWorldScope   Scope;
		if (Scope.World == nullptr)
		{
			OutError = TEXT("the control's transient world was not created");
			return false;
		}

		UStratMatchSubsystem* const Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
		if (Subsystem == nullptr)
		{
			OutError = TEXT("the control's world has no UStratMatchSubsystem");
			return false;
		}

		FStratMatchConfig Base;
		if (!MakeConfig(Base, OutError))
		{
			return false;
		}

		// Which sides exist is the PROJECTION's answer, exactly as `StratAiMatchClauses.cpp`
		// asks it. A literal two here would be this file deciding the scenario's side count.
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
			for (const FStratUnitView& U : Scout.Units)
			{
				Sides.AddUnique(U.Side);
			}
			Sides.Sort();
		}
		if (Sides.Num() < 2)
		{
			OutError = FString::Printf(
				TEXT("the seeded scenario deploys units for %d side(s); the control needs two"),
				Sides.Num());
			return false;
		}

		FStratMatchConfig BothAi  = Base;
		BothAi.AiSides            = Sides;
		BothAi.AiMaxConsecutiveTurns = kControlTurnsPerBatch;

		// ITS OWN SLOT, AND THIS IS NOT TIDINESS. If the control's game ever DOES reach a
		// result, `ApplyView`'s §2.11.6 hook writes `bHasCompletedAMatch` onto whatever
		// `SaveSlotName` resolves to -- and `MakeConfig` above resolves to
		// `kAbsentSlotName`, the name every OTHER clause in this file depends on NOT existing
		// so the guided opening arms. Sharing it would let this control silently disarm the
		// whole file. The scope below deletes this one on both ends.
		BothAi.SaveSlotName = kControlSlotName;

		if (BothAi.UnitTable != nullptr)
		{
			// §4.8's ids are `DT_Units`' own row names -- the row NAME is the id.
			BothAi.AiBuildlistUnitIds = BothAi.UnitTable->GetRowNames();
		}

		FString StartReason;
		Subsystem->StartMatch(BothAi, StartReason);
		if (!Subsystem->IsMatchLive())
		{
			OutError = StartReason;
			return false;
		}

		FStratBridge* const Bridge = Subsystem->GetBridge();
		if (Bridge == nullptr)
		{
			OutError = TEXT("the control's live match has no bridge");
			return false;
		}

		for (int32 Batch = 0; Batch < kControlMaxBatches; ++Batch)
		{
			FString RunReason;
			Subsystem->RunAiTurnsNow(RunReason);

			FStratViewModel Model;
			FString         ModelReason;
			if (!Subsystem->BuildViewModel(Model, ModelReason))
			{
				OutError = ModelReason;
				return false;
			}

			for (const FStratUnitView& U : Model.Units)
			{
				TArray<FIntPoint>  Targets;
				const FStratResult Asked = Bridge->AttackTargetHexes(U.UnitId, Targets);
				if (Asked.bOk && Targets.Num() > 0)
				{
					OutDescription = FString::Printf(
						TEXT("unit %d of side %d at %s has %d legal attack target(s), the first "
						     "at %s, after %d AI batch(es) of %d turn(s)"),
						U.UnitId, U.Side, *U.Hex.ToString(), Targets.Num(),
						*Targets[0].ToString(), Batch + 1, kControlTurnsPerBatch);
					return true;
				}
			}

			if (Model.Match.bHasResult)
			{
				OutError = FString::Printf(
					TEXT("the control's AI-vs-AI game reached a result after %d batch(es) without "
					     "any scan finding a unit with a legal attack target"),
					Batch + 1);
				return false;
			}
		}

		OutError = FString::Printf(
			TEXT("%d AI batches of %d turns produced no board position in which any unit had a "
			     "legal attack target"),
			kControlMaxBatches, kControlTurnsPerBatch);
		return false;
	}
}

// ---------------------------------------------------------------------------
// Q27, beat 1a: End Turn is INERT until the marked Infantry has moved.
//
// THE ASSERTION IS THE RECORDED COMMAND COUNT AND NOT THE RETURN VALUE. §2.11.6's gate is
// "inert", which is a claim about the rules module: nothing was asked. A gate that submitted
// the EndTurn and then returned false would end the player's turn and report a failure, and a
// clause that only read the return would call that a pass.
//
// THE CONTROL IS IN THE SAME CLAUSE AND IT IS NOT OPTIONAL. An End Turn that never worked at
// all -- a broken fixture, an unbuilt model, a controller with no subsystem -- also leaves the
// count unchanged. So the marked Infantry is moved through the same controller and the same
// method, and End Turn is then required to be ACCEPTED and the count required to rise. The
// silence is only evidence once the instrument has been shown able to speak.
//
// THE REFUSAL SENTENCE IS THE GDD'S, compared against `FStratGuidedOpening::
// EndTurnGateHoverText()` rather than against a copy -- so the strip, the hover and the log
// line cannot say three different things, which is the property the controller's own comment
// claims.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGateEndTurnInertTest,
	"Stratocracy.StratPlay.T-UI-02.EndTurnIsInertUntilTheMarkedInfantryMoves",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGateEndTurnInertTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceInputGates;

	DeclareFixtureWarnings(*this);

	FGateHarness H;
	FString Error;
	if (!TestTrue(TEXT("a live match, a controller and a marked Infantry"), H.Build(Error)))
	{
		AddError(Error);
		return false;
	}

	const int32 CountBefore = H.Bridge->RecordedCommandCount();

	// ARMED THROUGH THE PRODUCTION PATH AND NOT BY HAND. `TryArmGuidedOpening` runs at the top
	// of `HandleSelectionEvent`, so one event -- a Cancel, which "drops the selection; never
	// submits anything" -- is what arms it. Reaching into `GetGuidedOpening()` and calling
	// `Begin` would arm a machine the controller had not armed, and the clause would then be
	// about a machine rather than about the game.
	FString Warm;
	H.Send(EStratSelectionEvent::Cancel, FIntPoint::ZeroValue, Warm);

	// The gate must actually be closed, or everything below is vacuous.
	if (!TestTrue(TEXT("guidance armed and beat 1a is outstanding, so the gate is closed"),
			H.Controller->GetGuidedOpening().IsEndTurnGated()))
	{
		return false;
	}
	TestEqual(TEXT("and a Cancel submitted nothing on the way in"),
		H.Bridge->RecordedCommandCount(), CountBefore);

	// ---- the gate ------------------------------------------------------------
	FString Reason;
	const bool bAccepted = H.Send(EStratSelectionEvent::EndTurn, FIntPoint::ZeroValue, Reason);

	TestFalse(TEXT("Q27: End Turn is refused while beat 1a is outstanding"), bAccepted);
	TestEqual(
		TEXT("Q27: and INERT -- FStratBridge::RecordedCommandCount() did not move, so the rules "
		     "module was never asked. \"An inert input is an input that was never asked about, "
		     "not a command the rules module refused.\""),
		H.Bridge->RecordedCommandCount(), CountBefore);
	TestEqual(
		TEXT("Q27: the player-facing sentence is FStratGuidedOpening::EndTurnGateHoverText(), so "
		     "the strip, the hover and the log cannot say three different things"),
		Reason, FStratGuidedOpening::EndTurnGateHoverText().ToString());

	// ---- the control: the instrument can speak -------------------------------
	if (!TestTrue(TEXT("CONTROL: the marked Infantry moves through the same controller"),
			H.MoveTheMarkedInfantry(Error)))
	{
		AddError(Error);
		return false;
	}
	TestFalse(TEXT("beat 1a retired on that move"),
		H.Controller->GetGuidedOpening().IsBeatOutstanding(EStratGuidanceBeat::Beat1a));
	TestFalse(TEXT("so the End Turn gate is open"),
		H.Controller->GetGuidedOpening().IsEndTurnGated());

	const int32 CountAfterMove = H.Bridge->RecordedCommandCount();
	TestTrue(TEXT("CONTROL: the move itself was recorded, so the count is a live instrument"),
		CountAfterMove > CountBefore);

	FString EndReason;
	const bool bEndAccepted =
		H.Send(EStratSelectionEvent::EndTurn, FIntPoint::ZeroValue, EndReason);

	TestTrue(
		*FString::Printf(
			TEXT("CONTROL: End Turn is ACCEPTED once the marked Infantry has moved -- without "
			     "this half the clause above would pass on an End Turn that never worked at all. "
			     "Reason if refused: %s"),
			*EndReason),
		bEndAccepted);
	TestTrue(TEXT("CONTROL: and it reached the rules module"),
		H.Bridge->RecordedCommandCount() > CountAfterMove);

	return true;
}

// ---------------------------------------------------------------------------
// Q27, beat 1a: Wait is INERT for the marked Infantry, and the unit is NOT retired.
//
// `IsDone` IS THE ASSERTION AND "NOTHING WAS SUBMITTED" IS NOT ENOUGH. Wait never submits
// anything -- `EStratSelectionEvent::Wait` produces `EStratSelectionCommand::None` by
// construction, and the machine's own header says why: "`Wait` is a UI-level concept the save
// format cannot carry." So a Wait gate that let the machine see the event would leave the
// recorded log untouched AND retire the marked unit, and a submission-only clause would call
// that a pass. It is §2.11.6's deadlock exactly: End Turn still inert, and the one unit that
// could satisfy it retired without moving.
//
// THE CONTROLLER'S OWN COMMENT NAMES THE MECHANISM: the gate "returns before `HandleEvent`,
// which is the whole mechanism". That is what `IsDone` false measures.
//
// THE CONTROL IS THE SAME UNIT, LATER. After beat 1a retires the same Wait on the same unit
// must retire it -- so the false above is the gate and not a Wait that never worked.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGateWaitInertTest,
	"Stratocracy.StratPlay.T-UI-02.WaitIsInertForTheMarkedInfantry",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGateWaitInertTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceInputGates;

	DeclareFixtureWarnings(*this);

	FGateHarness H;
	FString Error;
	if (!TestTrue(TEXT("a live match, a controller and a marked Infantry"), H.Build(Error)))
	{
		AddError(Error);
		return false;
	}

	const int32 CountBefore = H.Bridge->RecordedCommandCount();

	// Select the marked Infantry. It is the one friendly unit the lock exempts, so this is
	// also a check that the exemption works.
	FString SelectReason;
	if (!TestTrue(
			*FString::Printf(
				TEXT("the marked Infantry selects -- the lock exempts it, and every other "
				     "friendly unit is unselectable. Reason if refused: %s"),
				*SelectReason),
			H.Send(EStratSelectionEvent::HexPrimary, H.MarkedHex, SelectReason)))
	{
		return false;
	}
	if (!TestEqual(TEXT("and it is the selected unit"),
			H.Controller->GetSelectionMachine().GetSelectedUnitId(), H.MarkedId))
	{
		return false;
	}
	if (!TestTrue(TEXT("the input gate really is closed for it"),
			H.Controller->GetGuidedOpening().IsUnitInputGated(H.MarkedId)))
	{
		return false;
	}

	// ---- the gate ------------------------------------------------------------
	FString Reason;
	const bool bAccepted = H.Send(EStratSelectionEvent::Wait, FIntPoint::ZeroValue, Reason);

	TestFalse(TEXT("Q27: Wait is refused for the marked Infantry while beat 1a is outstanding"),
		bAccepted);
	TestFalse(
		TEXT("Q27: and the unit is NOT retired -- IsDone stays false. A gate that let the machine "
		     "see the Wait would submit nothing and still retire it, which is §2.11.6's deadlock: "
		     "End Turn inert with no move left to satisfy it"),
		H.Controller->GetSelectionMachine().IsDone(H.MarkedId));
	TestEqual(TEXT("Q27: and nothing reached the rules module"),
		H.Bridge->RecordedCommandCount(), CountBefore);
	TestEqual(TEXT("Q27: the refusal carries the GDD's own sentence"),
		Reason, FStratGuidedOpening::EndTurnGateHoverText().ToString());

	// ---- the control: the same Wait, on the same unit, after 1a retires ------
	if (!TestTrue(TEXT("CONTROL: the marked Infantry moves"), H.MoveTheMarkedInfantry(Error)))
	{
		AddError(Error);
		return false;
	}
	TestFalse(TEXT("the input gate is open for it now"),
		H.Controller->GetGuidedOpening().IsUnitInputGated(H.MarkedId));

	// RE-SELECTED THROUGH A CANCEL FIRST, on the same measurement the move helper records:
	// a `HexPrimary` on the hex of the already-selected unit DROPS the selection rather than
	// re-selecting it, and the Wait that followed then refused with "nothing is selected" --
	// which would have looked like the gate still being closed.
	FString Reselect;
	H.Send(EStratSelectionEvent::Cancel, FIntPoint::ZeroValue, Reselect);
	H.Send(EStratSelectionEvent::HexPrimary, H.MarkedHex, Reselect);
	if (!TestEqual(
			*FString::Printf(
				TEXT("CONTROL: the marked Infantry re-selects at its new hex %s, so the Wait below "
				     "has a subject. Reason if refused: %s"),
				*H.MarkedHex.ToString(), *Reselect),
			H.Controller->GetSelectionMachine().GetSelectedUnitId(), H.MarkedId))
	{
		return false;
	}

	FString WaitReason;
	H.Send(EStratSelectionEvent::Wait, FIntPoint::ZeroValue, WaitReason);

	TestTrue(
		*FString::Printf(
			TEXT("CONTROL: the same Wait on the same unit DOES retire it once beat 1a has retired "
			     "-- without this half, IsDone false above would pass on a Wait that never worked. "
			     "Reason if refused: %s"),
			*WaitReason),
		H.Controller->GetSelectionMachine().IsDone(H.MarkedId));

	return true;
}

// ---------------------------------------------------------------------------
// Q27, beat 1a: the marked Infantry's SELECTED -> attack route is closed.
//
// READ THE LIMITS BEFORE READING THE ASSERTIONS. This clause pins the PREDICATE the
// controller's attack arm is written against, and it deliberately stops there. Two things it
// does not reach, both stated with what would let it:
//
//   1. THE CONTROLLER'S ATTACK ARM ITSELF IS UNREACHABLE ON THE SHIPPED SCENARIO, and that is
//      now a MEASUREMENT rather than a reading of the map. The arm fires only when
//      `HandleEvent` returns `EStratSelectionCommand::Attack` for the marked unit WHILE beat 1a
//      is outstanding -- and beat 1a retires on that unit's own move. So the arm is reachable
//      only from a position where the marked Infantry has a legal attack target AT ITS
//      DEPLOYMENT HEX, before it has moved. `T-UI-02.TheMarkedInfantryHasNoAttackTargetAt-
//      Deployment` below asks the rules module: `FStratBridge::AttackTargetHexes` ANSWERS for
//      the marked Infantry (`bOk` true, not a refusal) and returns ZERO hexes, and the same
//      run reports 0 of 10 answered units with a target anywhere on the deployment position.
//      The reason for it is unchanged and still authored: the guided lane is a walk to a
//      neutral Factory and the two sides deploy at opposite ends of the map. The sentence this
//      item used to end on was:
//
// RETRACTED>     "On `Data/ferrum_crossing.json` it does not: the guided lane is a
// RETRACTED>      five-movement-point walk to a neutral Factory."
//
//      -- withdrawn only because it argued from the lane's length rather than from the
//      module's answer, not because it was wrong. WHAT WOULD UNBLOCK THE ARM ITSELF is
//      unchanged: a scenario fixture whose `guidedOpening.infantry` deploys adjacent to an
//      enemy. That is a `Data/` addition -- vendored, hash-gated, and not writable from this
//      lane or the engineer's.
//
//   2. THE TARGET OVERLAY -- WITHDRAWN 2026-08-21. This item used to read:
//
// RETRACTED>     "THE TARGET OVERLAY IS NOT OBSERVABLE. `AStratBoardActor::TargetOverlay` is
// RETRACTED>      `protected` and the class exposes no count for it -- `GetDrawnHexCount` is
// RETRACTED>      the TILE count, not an overlay's. So \"the target overlay is empty\" cannot be
// RETRACTED>      asserted from a test today. WHAT WOULD UNBLOCK IT: a `BlueprintPure` accessor
// RETRACTED>      on `AStratBoardActor` returning the target overlay's instance count."
//
//      That accessor landed as `GetTargetOverlayCount()`, and the overlay half below measures
//      through it. WHAT THE MEASUREMENT STILL CANNOT ATTRIBUTE, stated so the assertion is not
//      read as more than it is: the marked Infantry is BELIEVED to have no legal attack target
//      at its deployment hex, in which case `FStratSelectionMachine::BuildOverlays` would hand
//      back an empty target set with or without the gate, and a zero after a gated selection
//      would be the right OBSERVABLE -- nothing is lit -- without being evidence that the gate
//      is what emptied it.
//
//      THAT BELIEF WAS AN INFERENCE AND IS NOW A MEASUREMENT -- 2026-08-21. This item used to
//      read, in this file's own voice:
//
// RETRACTED>     "THAT BELIEF IS AN INFERENCE AND IS MARKED AS ONE ... It was NOT measured:
// RETRACTED>      nothing in this file calls `FStratBridge::AttackTargetHexes` for the marked
// RETRACTED>      unit, so no run has ever asked the rules module the question. WHAT WOULD
// RETRACTED>      SETTLE IT: one call to that method on the seeded board, asserted empty."
//
//      That call is made, in `T-UI-02.TheMarkedInfantryHasNoAttackTargetAtDeployment` below,
//      WITH the control an empty result needs: the same method is shown returning a non-empty
//      set for a unit in contact on a privately-played AI-vs-AI board, so the zero here is a
//      fact about the deployment position and not about the method. What that does and does
//      NOT change for THIS clause: the marked Infantry's empty target set is now known rather
//      than believed, so `BuildOverlays` would hand back an empty target set with or without
//      the gate -- which means the zero below is still not evidence that the gate is what
//      emptied it. The attribution gap is unchanged; only the belief underneath it has been
//      settled, and it settled in the direction that KEEPS the gap. Item 1 above is the same
//      root cause and unblocking it unblocks this.
//      WHAT THE CLAUSE DOES GET FROM THE ACCESSOR, and it is not nothing: a non-zero overlay
//      is PLANTED first and required to survive being read, then required to be back to zero
//      after the gated refresh -- so the refresh demonstrably CLEARS the overlay rather than
//      merely never writing to it, and the zero is a measurement rather than a mute instrument.
//
// WHAT IS PINNED, AND IT IS THE HALF THAT DECIDES. `FStratGuidedOpening::IsUnitInputGated` is
// the single predicate BOTH the Wait arm, the attack arm and the overlay suppression consult
// -- three call sites, one answer. The clause pins that answer on all three of its axes: true
// for the marked unit while 1a is outstanding, false for every other unit, false for
// `INDEX_NONE`, and false for the marked unit once 1a retires. An attack arm keyed on a
// predicate that was wrong on any of those would be wrong wherever it fired.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGateAttackClosedTest,
	"Stratocracy.StratPlay.T-UI-02.AttackIsClosedForTheMarkedInfantry",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGateAttackClosedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceInputGates;

	DeclareFixtureWarnings(*this);

	FGateHarness H;
	FString Error;
	if (!TestTrue(TEXT("a live match, a controller and a marked Infantry"), H.Build(Error)))
	{
		AddError(Error);
		return false;
	}

	// Arm the guidance through the production path.
	FString Warm;
	H.Send(EStratSelectionEvent::Cancel, FIntPoint::ZeroValue, Warm);

	const FStratGuidedOpening& Guidance = H.Controller->GetGuidedOpening();
	if (!TestTrue(TEXT("beat 1a is outstanding, which is the gate's condition"),
			Guidance.IsBeatOutstanding(EStratGuidanceBeat::Beat1a)))
	{
		return false;
	}

	// ---- true for the marked unit --------------------------------------------
	TestTrue(
		TEXT("Q27: the marked Infantry's Wait and attack routes are closed while beat 1a is "
		     "outstanding -- one predicate, consulted by the Wait arm, the attack arm and the "
		     "overlay suppression alike"),
		Guidance.IsUnitInputGated(H.MarkedId));

	// ---- and false for everyone else -----------------------------------------
	FStratViewModel Model;
	if (!TestTrue(TEXT("the view model builds"), H.Match->BuildViewModel(Model, Error)))
	{
		AddError(Error);
		return false;
	}

	int32 OtherUnitsChecked = 0;
	for (const FStratUnitView& U : Model.Units)
	{
		if (U.UnitId == H.MarkedId)
		{
			continue;
		}
		TestFalse(
			*FString::Printf(
				TEXT("Q27: unit %d is NOT input-gated -- \"gating those here as well would be a "
				     "second mechanism for one restriction\"; every other friendly unit is "
				     "unselectable through the LOCK instead"),
				U.UnitId),
			Guidance.IsUnitInputGated(U.UnitId));
		++OtherUnitsChecked;
	}
	TestTrue(TEXT("the board really has other units, so the loop above had content"),
		OtherUnitsChecked > 0);

	// ---- and false for nothing-selected --------------------------------------
	TestFalse(
		TEXT("Q27: INDEX_NONE answers false, so a caller with nothing selected need not "
		     "special-case it"),
		Guidance.IsUnitInputGated(INDEX_NONE));

	// ---- and false for the marked unit once 1a retires -----------------------
	if (!TestTrue(TEXT("CONTROL: the marked Infantry moves"), H.MoveTheMarkedInfantry(Error)))
	{
		AddError(Error);
		return false;
	}
	TestFalse(
		TEXT("Q27: the gate opens for the marked Infantry the moment beat 1a retires -- without "
		     "this the clause could not tell a gate from a predicate that is always true"),
		Guidance.IsUnitInputGated(H.MarkedId));

	// -----------------------------------------------------------------------
	// THE NOT-LIT HALF. §2.11.6-B: beat 1a's marked Infantry's "attack targets are not lit,
	// so the SELECTED -> attack transition (§2.11.1) is closed to it". Submission and lighting
	// are TWO restrictions, and a clause pinning only the first looks complete while covering
	// half of what the GDD names.
	//
	// THE CONTROL COMES FIRST AND IT IS THE REASON THIS IS WRITABLE AT ALL. A non-zero target
	// overlay is planted directly on the board and read back; only once the instrument has
	// been shown able to report a non-zero is the zero after the gated refresh worth anything.
	// See item 2 of this clause's block for what the zero still cannot attribute -- and for
	// what the plant-then-clear transition DOES buy over a bare zero.
	// -----------------------------------------------------------------------
	AStratBoardActor* const Board = H.Match->GetBoard();
	if (!TestNotNull(TEXT("the match spawned a board"), Board))
	{
		return false;
	}
	if (!TestTrue(TEXT("the board is given an overlay mesh, or its overlay count is mute"),
			GiveTheBoardAnOverlayMesh(Board, Error)))
	{
		AddError(Error);
		return false;
	}

	// A second harness, because the one above has already moved the marked Infantry and the
	// gate is open on it. The overlay half needs beat 1a OUTSTANDING.
	FGateHarness G;
	if (!TestTrue(TEXT("a second fixture, with beat 1a still outstanding"), G.Build(Error)))
	{
		AddError(Error);
		return false;
	}
	AStratBoardActor* const GatedBoard = G.Match->GetBoard();
	if (!TestNotNull(TEXT("that fixture spawned a board too"), GatedBoard))
	{
		return false;
	}
	if (!TestTrue(TEXT("and it is given an overlay mesh"),
			GiveTheBoardAnOverlayMesh(GatedBoard, Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- CONTROL: the instrument can report a non-zero -----------------------
	// Two hexes the board actually has, taken off the model rather than invented.
	FStratViewModel GatedModel;
	if (!TestTrue(TEXT("the second fixture's view model builds"),
			G.Match->BuildViewModel(GatedModel, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestTrue(TEXT("the board has at least two hexes to light"), GatedModel.Hexes.Num() >= 2))
	{
		return false;
	}

	TArray<FIntPoint> Planted;
	Planted.Add(GatedModel.Hexes[0].Hex);
	Planted.Add(GatedModel.Hexes[1].Hex);
	GatedBoard->ShowTargets(Planted);

	if (!TestEqual(
			TEXT("CONTROL: AStratBoardActor::GetTargetOverlayCount() reports a PLANTED, non-zero "
			     "target overlay. Without this the zero below could be a mute instrument -- an "
			     "overlay with no mesh reports 0 for every input, silently, forever"),
			GatedBoard->GetTargetOverlayCount(), Planted.Num()))
	{
		return false;
	}

	// ---- and the gated selection leaves it empty -----------------------------
	// ARMED UNCONDITIONALLY BEFORE IT IS ASSERTED ON, and the order is a measurement rather
	// than a style choice. This was first written as `if (!TestTrue(<gate is closed>)) { arm; }`
	// -- a `TestTrue` used as a PROBE. Guidance arms lazily inside the first
	// `HandleSelectionEvent`, so before any event the gate is legitimately open, the probe
	// recorded a failure, and the clause went red while every assertion it actually cared
	// about passed. `TestTrue` is not a question; it is a claim. Anything that needs to be
	// true first gets DONE first, and only then asserted.
	FString Arm;
	G.Send(EStratSelectionEvent::Cancel, FIntPoint::ZeroValue, Arm);
	if (!TestTrue(TEXT("beat 1a is outstanding in the second fixture, so the gate is closed"),
			G.Controller->GetGuidedOpening().IsEndTurnGated()))
	{
		return false;
	}

	FString GatedSelect;
	G.Send(EStratSelectionEvent::HexPrimary, G.MarkedHex, GatedSelect);

	if (!TestEqual(TEXT("the marked Infantry is the selected unit in the second fixture"),
			G.Controller->GetSelectionMachine().GetSelectedUnitId(), G.MarkedId))
	{
		return false;
	}
	if (!TestTrue(TEXT("and it is input-gated at the moment the overlay is read"),
			G.Controller->GetGuidedOpening().IsUnitInputGated(G.MarkedId)))
	{
		return false;
	}

	TestEqual(
		*FString::Printf(
			TEXT("§2.11.6-B: the marked Infantry's attack targets are NOT LIT while beat 1a is "
			     "outstanding. The overlay held %d instance(s) a moment ago and the gated refresh "
			     "brought it to zero, so the refresh demonstrably CLEARED it rather than never "
			     "writing to it"),
			Planted.Num()),
		GatedBoard->GetTargetOverlayCount(), 0);

	return true;
}

// ---------------------------------------------------------------------------
// The gates never reach the rules module -- the negative, WITH its control.
//
// AN ABSENCE NEEDS A CONTROL, and this clause is shaped around that and nothing else. Silence
// in a log proves nothing until the instrument has been shown able to speak: a capture that
// registered too late, filtered too strictly, or watched the wrong category reports exactly
// the same zero as a working gate. This project has paid about eight rounds for that lesson
// once already.
//
// SO THE CLAUSE HAS TWO HALVES AND THE SECOND IS NOT OPTIONAL:
//   1. With beat 1a outstanding, a gated End Turn and a gated Wait produce ZERO `STRAT-CMD`
//      lines of any kind, and `RecordedCommandCount()` does not move.
//   2. Then the marked Infantry moves through the SAME controller, the SAME method and the
//      SAME live capture, and a `STRAT-CMD accepted` line MUST appear. That line is the
//      instrument speaking.
//
// THE INSTRUMENT IS LOOSE AND THE CLAIM IS STRICT. The capture admits `strat-cmd` in any
// casing -- so a wrongly-cased emitter is caught rather than filtered out before the claim can
// see it -- and the assertion compares the exact bytes `STRAT-CMD accepted`.
//
// NOT `AddExpectedMessagePlain`, and this is the reason rather than a preference:
// `FAutomationExpectedMessage::Matches` uses `FString::Contains` at its `IgnoreCase` default
// and both of its constructors hardcode `ERegexPatternFlags::CaseInsensitive`. The
// expected-message machinery cannot express a case-sensitive claim at all, so a clause that
// tried to close this with one would be asserting something weaker than it read.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGatesNeverReachRulesTest,
	"Stratocracy.StratPlay.T-UI-02.GatesNeverReachTheRulesModule",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGatesNeverReachRulesTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceInputGates;

	DeclareFixtureWarnings(*this);

	FGateHarness H;
	FString Error;
	if (!TestTrue(TEXT("a live match, a controller and a marked Infantry"), H.Build(Error)))
	{
		AddError(Error);
		return false;
	}

	// The capture is constructed AFTER the fixture, so the seeding's own log traffic is not in
	// its window and the counts below are attributable to the events this clause sends.
	FStratCmdCapture Capture;

	FString Warm;
	H.Send(EStratSelectionEvent::Cancel, FIntPoint::ZeroValue, Warm);
	if (!TestTrue(TEXT("beat 1a is outstanding, so both gates are closed"),
			H.Controller->GetGuidedOpening().IsEndTurnGated()))
	{
		return false;
	}

	const int32 CountBefore = H.Bridge->RecordedCommandCount();

	// ---- half one: the silence -----------------------------------------------
	FString EndReason;
	H.Send(EStratSelectionEvent::EndTurn, FIntPoint::ZeroValue, EndReason);

	FString SelectReason;
	H.Send(EStratSelectionEvent::HexPrimary, H.MarkedHex, SelectReason);

	FString WaitReason;
	H.Send(EStratSelectionEvent::Wait, FIntPoint::ZeroValue, WaitReason);

	Capture.Settle();

	TestEqual(
		TEXT("Q27: a gated End Turn and a gated Wait emit NO STRAT-CMD line of any kind -- not "
		     "`accepted`, not `refused`. An inert input is an input the rules module was never "
		     "asked about"),
		Capture.Lines.Num(), 0);
	TestEqual(TEXT("Q27: and the recorded command count agrees, independently of any log"),
		H.Bridge->RecordedCommandCount(), CountBefore);

	// ---- half two: the control, on the same instrument -----------------------
	if (!TestTrue(
			TEXT("CONTROL: the marked Infantry moves through the same controller and the same "
			     "method, with this capture still registered"),
			H.MoveTheMarkedInfantry(Error)))
	{
		AddError(Error);
		return false;
	}
	Capture.Settle();

	const int32 Accepted = Capture.CountExact(TEXT("STRAT-CMD accepted"));
	TestTrue(
		*FString::Printf(
			TEXT("CONTROL: a `STRAT-CMD accepted` line DOES appear once a real command lands -- "
			     "this is the half that turns the zero above into evidence rather than a broken "
			     "capture. Saw %d STRAT-CMD line(s) in total, %d of them `accepted`"),
			Capture.Lines.Num(), Accepted),
		Accepted > 0);
	TestTrue(TEXT("CONTROL: and the recorded command count moved with it"),
		H.Bridge->RecordedCommandCount() > CountBefore);

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-02 -- the marked Infantry has NO legal attack target at its deployment hex, MEASURED.
//
// WHAT THIS REPLACES. `AttackIsClosedForTheMarkedInfantry` above states, in its own block, that
// the marked Infantry is BELIEVED to have no attack target at deployment, marks the belief as an
// inference, and names what would settle it: "one call to `FStratBridge::AttackTargetHexes` on
// the seeded board, asserted empty". This clause is that call. It changes nothing about what the
// overlay clause asserts; it removes the word BELIEVED from the reasoning that clause's zero
// rests on, and it is the same measurement item 1 of that block needs -- the controller's attack
// arm is unreachable on the shipped scenario because THE RULES MODULE SAYS the marked unit has
// nothing to attack from where it stands, not because this file thinks the lane looks long.
//
// AN EMPTY RESULT PROVES NOTHING ON ITS OWN, so the control is in this clause and is not
// optional. `FindABoardPositionWithALegalAttack` plays a private AI-vs-AI match until the same
// method, on the same build, returns a NON-EMPTY set for some unit -- see its block for why the
// control cannot be taken on the opening position. Only then is the empty answer for the marked
// Infantry a fact about the board rather than a fact about the method.
//
// THE ORDER IS MEASUREMENT FIRST, CONTROL SECOND, AND THAT IS DELIBERATE. The control plays a
// game; the measurement must be taken on an untouched deployment. They are in different worlds
// besides, so neither could disturb the other, but the order says which is the subject.
//
// `bOk` IS ASSERTED SEPARATELY FROM EMPTINESS, and the distinction is the whole hazard here.
// `AttackTargetHexes` REFUSES for an unknown unit or an unseeded bridge, and an out-param left
// untouched by a refusal is also empty. A clause that read only `Targets.Num() == 0` would call
// "the bridge does not know this unit" a measurement of the board. `StratBridge.h`: "AN EMPTY
// RESULT IS AN ANSWER HERE, unlike `ReachableHexes`" -- so an empty set with `bOk` TRUE is the
// module saying there is nothing to attack, and that is the only reading this clause accepts.
//
// WHAT IS NOT ASSERTED, said plainly: that no OTHER unit has a target at deployment. Every unit
// is scanned and the scan is reported with `AddInfo`, because a scenario re-authored into
// contact should show up in a report rather than redden a clause about the marked Infantry.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratMarkedInfantryHasNoAttackTargetTest,
	"Stratocracy.StratPlay.T-UI-02.TheMarkedInfantryHasNoAttackTargetAtDeployment",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratMarkedInfantryHasNoAttackTargetTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceInputGates;

	DeclareFixtureWarnings(*this);

	// The control's bounded batches each end on `AiMaxConsecutiveTurns` and say so. See
	// `FindABoardPositionWithALegalAttack`'s block; `Occurrences = 0` means at least once, and
	// at least one batch always runs.
	AddExpectedMessagePlain(TEXT("STRAT-AI refused"), ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);

	FGateHarness H;
	FString Error;
	if (!TestTrue(TEXT("a live match, a controller and a marked Infantry"), H.Build(Error)))
	{
		AddError(Error);
		return false;
	}

	// ---- THE MEASUREMENT, on the untouched deployment -------------------------
	TArray<FIntPoint>  Targets;
	const FStratResult Asked = H.Bridge->AttackTargetHexes(H.MarkedId, Targets);

	if (!TestTrue(
			*FString::Printf(
				TEXT("the rules module ANSWERS the question for the marked Infantry rather than "
				     "refusing it -- a refusal leaves the out-param empty too, and would be read "
				     "as 'no targets' by a clause that only counted. It said: '%s'"),
				*Asked.Reason),
			Asked.bOk))
	{
		return false;
	}

	FString Listed;
	for (const FIntPoint& Hex : Targets)
	{
		Listed += (Listed.IsEmpty() ? TEXT("") : TEXT(", ")) + Hex.ToString();
	}

	TestEqual(
		*FString::Printf(
			TEXT("§2.11.6-B / T-UI-02: the marked Infantry has NO legal attack target at its "
			     "deployment hex %s -- measured through FStratBridge::AttackTargetHexes, which "
			     "asks strat::uiForecast per enemy and compares no distances. Targets returned: "
			     "[%s]"),
			*H.MarkedHex.ToString(), *Listed),
		Targets.Num(), 0);

	// ---- what every other unit says, reported and not asserted ----------------
	FStratViewModel Model;
	if (TestTrue(TEXT("the view model builds"), H.Match->BuildViewModel(Model, Error)))
	{
		int32 UnitsWithTargets = 0;
		int32 UnitsAsked       = 0;
		for (const FStratUnitView& U : Model.Units)
		{
			TArray<FIntPoint>  Theirs;
			const FStratResult TheirAnswer = H.Bridge->AttackTargetHexes(U.UnitId, Theirs);
			if (TheirAnswer.bOk)
			{
				++UnitsAsked;
				if (Theirs.Num() > 0)
				{
					++UnitsWithTargets;
				}
			}
		}
		AddInfo(FString::Printf(
			TEXT("At deployment: %d of %d answered units have a legal attack target "
			     "(%d unit(s) in the model)"),
			UnitsWithTargets, UnitsAsked, Model.Units.Num()));
	}

	// ---- THE CONTROL: the same method, on this build, can return non-empty ----
	FString Where;
	FString ControlError;
	if (!TestTrue(
			*FString::Printf(
				TEXT("CONTROL: FStratBridge::AttackTargetHexes returns a NON-EMPTY set for some "
				     "unit at some board position -- without this half the zero above is a fact "
				     "about the method rather than about the board. %s"),
				*ControlError),
			FindABoardPositionWithALegalAttack(Where, ControlError)))
	{
		AddError(ControlError);
		return false;
	}

	AddInfo(FString::Printf(TEXT("CONTROL: %s"), *Where));

	return true;
}
