// GDD §2.11.6 -- the route the guided-opening projection takes to the screen.
//
// T-INT-05, "rebuild the screen from the view model alone". Filed under that ID and not under
// T-UI-02 or T-UI-06 for the reason `StratViewModel.h` states about the presentation block:
// state in the block satisfies the rebuild invariant and state in a widget does not. Every
// clause below is a way of asking whether the directive strip is still a PROJECTION of
// `FStratViewModel::Guidance` or has quietly become a second place the screen is stated from.
// `T-INT-05.ApplyViewSpawnsMovesAndDestroys` (StratMatchReconcile.cpp) asks that question of
// units and `T-INT-05.LockArisesFromTheGuidanceLayer` asks it of locks; this file asks it of
// the strip.
//
// WHERE THE EXPECTATIONS COME FROM, since that is the only thing that makes these clauses
// worth their runtime. NOT ONE guidance field is typed out below. The expected value is always
// `Model.Guidance` -- the field of the very model handed to `ApplyView` -- and the comparison
// is `UScriptStruct::CompareScriptStruct`, which walks `FStratGuidanceView`'s own reflection
// data. Two consequences, both deliberate:
//   - The clauses cannot drift from the struct. A field added to `FStratGuidanceView`
//     tomorrow is compared tomorrow, with no edit here. A hand-written field-by-field
//     comparison would have gone silently incomplete on that same day, which is the exact
//     way a parity test stops testing.
//   - No `FString` or `FText` comparison appears anywhere in an assertion. `FString::operator==`,
//     `Contains` and `TestEqual` are all CASE-INSENSITIVE in this project and have already
//     produced a clause that could not fail. `CompareScriptStruct` uses `FProperty::Identical`
//     per field, which is not. The only place text is turned into a string here is
//     `Describe`, and that is a failure message, never a subject.
//
// AND THE INSTRUMENT IS CONTROLLED. `GuidanceInstrumentDistinguishes` below shows
// `CompareScriptStruct` returning FALSE for two views that differ before any clause relies on
// it returning TRUE. An absence proves nothing until the instrument is shown able to speak,
// and a comparator that answered "equal" unconditionally would make every clause in this file
// green and meaningless.
//
// WHAT THIS FILE DOES NOT PIN, stated so a later reader does not assume more than was
// measured. It does not pin the SOURCE SHAPE of `AStratScoreboardHUD::PushGuidance` -- an
// automation test cannot read a function body, and a clause that scanned the .cpp text would
// fail on a comment edit and pass on a branch written in an unexpected form. What is pinned
// instead is that method's observable consequence: every distinguishable shape of
// `FStratGuidanceView` handed to it arrives at the strip unchanged, which is what a branch on
// `bActive` (or on any other field) there would break.
#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Math/IntPoint.h"
#include "Misc/Paths.h"
#include "UObject/Class.h"
#include "UObject/GCObjectScopeGuard.h"
#include "UObject/UnrealType.h"
#include "UObject/UObjectGlobals.h"

#include "StratBoardActor.h"
#include "Tests/StratGuidanceRouteProbe.h"
#include "StratGuidedOpening.h"
#include "StratMatchSubsystem.h"
#include "StratPlayerController.h"
#include "StratScoreboardHUD.h"
#include "StratSelectionMachine.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

#include "StratBridge.h"

namespace StratGuidanceRoute
{
	static const int32 kFirstSide = 0;

	/**
	 * A slot name that does not exist, so `HasCompletedAMatchOnSave` answers false and
	 * `AStratPlayerController::TryArmGuidedOpening` ARMS the guided opening.
	 *
	 * WHY THIS IS HERE, AND IT IS A MEASUREMENT RATHER THAN TIDINESS.
	 * `FStratMatchConfig::SaveSlotName` defaults to `StratocracyMatch` -- THE PLAYER'S SLOT
	 * -- and `MakeConfig` below used to leave it there. `TryArmGuidedOpening` asks
	 * `HasCompletedAMatchOnSave(FString())`, which resolves to that default, so
	 * `RefreshFromMachineDecoratesWithNoRegistration` read whatever `.sav` happened to be on
	 * the machine running it. Measured 2026-09-01 over `cada741`, three runs of that clause
	 * alone: with `Saved/SaveGames/StratocracyMatch.sav` present and carrying
	 * `bHasCompletedAMatch`, it failed with an entirely unset strip; with that one file moved
	 * aside and nothing else changed, it passed; with the same bytes restored, it failed
	 * again. The controller was right every time -- §2.11.6 is "any completed match on the
	 * save skips all guidance automatically" -- and the clause was reading a disk it never
	 * named.
	 *
	 * `StratGuidanceInputGates.cpp` carries the same constant for the same cause and states
	 * the OPPOSITE symptom: there a suppressed opening opens every gate and the whole file
	 * passes vacuously. A fixture that does not name its slot fails on one machine and goes
	 * quiet on another, and neither is about the code under test.
	 */
	static const TCHAR* kAbsentSlotName = TEXT("StratocracyAutomation_NoSuchSlot_GuidanceRoute");

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/**
	 * Property-wise equality, taken from `FStratGuidanceView`'s OWN reflection data.
	 *
	 * This is the single reason no clause below retypes a guidance field. See the file header.
	 */
	static bool SameGuidance(const FStratGuidanceView& A, const FStratGuidanceView& B)
	{
		return FStratGuidanceView::StaticStruct()->CompareScriptStruct(&A, &B, /*PortFlags*/ 0);
	}

	/** Failure-message text only. Never a subject of an assertion. */
	static FString Describe(const FStratGuidanceView& V)
	{
		return FString::Printf(
			TEXT("bActive=%d beat=%d directive='%s' endTag=%d tagText='%s' ring=%d hex=(%d,%d) ")
			TEXT("gated=%d gateHover='%s' lockHover='%s'"),
			V.bActive ? 1 : 0,
			static_cast<int32>(V.Beat),
			*V.DirectiveText.ToString(),
			V.bShowsWindowEndTag ? 1 : 0,
			*V.WindowEndTagText.ToString(),
			V.bHasObjectiveRing ? 1 : 0,
			V.ObjectiveHex.X, V.ObjectiveHex.Y,
			V.bEndTurnGated ? 1 : 0,
			*V.EndTurnGateHover.ToString(),
			*V.LockedUnitHover.ToString());
	}

	/**
	 * A transient world whose actors are INITIALISED FOR PLAY.
	 *
	 * Copied in shape from StratMatchLifecycle.cpp, whose block records the measurement:
	 * without `InitializeActorsForPlay`, `AController::PostInitializeComponents` never runs, a
	 * spawned `APlayerController` is not in `UWorld::PlayerControllerList`, and
	 * `UStratMatchSubsystem::FindScoreboardHUD` -- which asks `GetFirstPlayerController()` --
	 * finds nothing. Every push clause here would then observe an unreached HUD and pass.
	 */
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
				World->InitializeActorsForPlay(FURL());
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
		Out.ViewingSide     = 0;
		Out.BoardActorClass = AStratBoardActor::StaticClass();
		Out.UnitActorClass  = AStratUnitActor::StaticClass();
		Out.SaveSlotName    = kAbsentSlotName;
		return true;
	}

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

	/**
	 * A live match in a transient world, with a findable HUD and (optionally) a strip on it.
	 *
	 * THE HUD'S OWN `BeginPlay` IS NOT DISPATCHED, unlike StratMatchLifecycle.cpp's fixture,
	 * and the omission is the point. `BeginPlay` is where the HUD would create its strip from
	 * `GuidanceWidgetClass`, and `CreateGuidanceWidget` ends in `AddToViewport`, which a
	 * headless `-nullrhi` run has no viewport for. The clauses here are about what the route
	 * DELIVERS, not about who instantiated the receiver, so the receiver is placed directly on
	 * `GuidanceStrip` -- the same member `CreateGuidanceWidget` assigns -- and the asset step
	 * is left to the editor lane. `ReconcileCompletesWithNoStripConfigured` is the clause that
	 * covers the other configuration, and it needs no asset either.
	 */
	struct FRouteHarness
	{
		FTestWorldScope       Scope;
		UStratMatchSubsystem* Subsystem  = nullptr;
		AStratScoreboardHUD*  Hud        = nullptr;
		APlayerController*    Controller = nullptr;

		/** Non-null only when `SpawnWorldAndActors` was asked for the game's own controller. */
		AStratPlayerController* StratController = nullptr;
		bool                  bHasStrip  = false;

		/**
		 * The world, the controller and the HUD -- everything that makes a match findable, and
		 * nothing that starts one.
		 *
		 * SPLIT OUT OF `Arm` RATHER THAN COPIED. The delivery clauses added at the end of this
		 * file need the three steps in orders `Arm` does not offer, and a second fixture would
		 * have been a second thing to keep true. THE ORDER IS ITSELF THE SUBJECT of two of them:
		 * a strip installed BEFORE the match starts receives `StartMatchInternal`'s push, and one
		 * installed after it does not -- which is the first delivery defect exactly.
		 *
		 * @param bStratController spawns `AStratPlayerController` rather than the plain engine
		 *        one. Its `BeginPlay` still does NOT run in this world -- `InitializeActorsForPlay`
		 *        initialises actors without beginning play -- so spawning it registers no view
		 *        decorator, and `RefreshFromMachineDecoratesWithNoRegistration` asserts that
		 *        through `HasViewDecorator()` rather than assuming it.
		 */
		bool SpawnWorldAndActors(bool bStratController, FString& OutError)
		{
			if (Scope.World == nullptr)
			{
				OutError = TEXT("no transient world was created");
				return false;
			}

			if (bStratController)
			{
				StratController = Scope.World->SpawnActor<AStratPlayerController>();
				Controller      = StratController;
			}
			else
			{
				Controller = Scope.World->SpawnActor<APlayerController>();
			}
			if (Controller == nullptr)
			{
				OutError = TEXT("no player controller spawned");
				return false;
			}

			Hud = Scope.World->SpawnActor<AStratScoreboardHUD>();
			if (Hud == nullptr)
			{
				OutError = TEXT("no scoreboard HUD spawned");
				return false;
			}
			Controller->MyHUD = Hud;

			if (Scope.World->GetFirstPlayerController() == nullptr)
			{
				OutError = TEXT("the world reports no first local player controller, so "
				                "FindScoreboardHUD would find nothing and every push clause "
				                "would observe an unreached HUD");
				return false;
			}

			Subsystem = Scope.World->GetSubsystem<UStratMatchSubsystem>();
			if (Subsystem == nullptr)
			{
				OutError = TEXT("the world has no match subsystem");
				return false;
			}

			return true;
		}

		/** Starts the shipped scenario on the subsystem `SpawnWorldAndActors` found. */
		bool StartTheMatch(FString& OutError)
		{
			if (Subsystem == nullptr)
			{
				OutError = TEXT("SpawnWorldAndActors has not run, so there is no subsystem to start");
				return false;
			}

			FStratMatchConfig Config;
			if (!MakeConfig(Config, OutError))
			{
				return false;
			}

			FString StartReason;
			Subsystem->StartMatch(Config, StartReason);
			if (!Subsystem->IsMatchLive() || Subsystem->GetBridge() == nullptr)
			{
				OutError = FString::Printf(TEXT("no live match: %s"), *StartReason);
				return false;
			}

			return true;
		}

		/** Puts the test-only concrete strip on the member `CreateGuidanceWidget` assigns. */
		bool InstallStrip(FString& OutError)
		{
			if (!StratTestInstallGuidanceStripDouble(Hud))
			{
				OutError = TEXT("the test-only concrete guidance widget did not install");
				return false;
			}
			bHasStrip = true;
			return true;
		}

		/**
		 * Spawn, start, then install -- the original sequence, kept identical in effect so the
		 * seven clauses written against it are driving the fixture they always drove.
		 */
		bool Arm(bool bWithStrip, FString& OutError)
		{
			if (!SpawnWorldAndActors(/*bStratController*/ false, OutError) || !StartTheMatch(OutError))
			{
				return false;
			}
			return !bWithStrip || InstallStrip(OutError);
		}

		/**
		 * What the strip is holding right now.
		 *
		 * ASSERTS NOTHING AND DEFAULTS NOTHING. A read that failed would otherwise hand a clause
		 * a default-constructed view, and a default view is a legitimate value here -- an
		 * inactive strip holds one. So the read's own success is returned and each caller
		 * checks it.
		 */
		bool ReadStrip(FStratGuidanceView& Out) const
		{
			return StratTestReadGuidanceStrip(Hud, Out);
		}

		/** The subsystem's own rebuild, undecorated: exactly what `RefreshPresentation` applies. */
		bool BuildUndecorated(FStratViewModel& Out, FString& OutError) const
		{
			return Subsystem->BuildViewModel(Out, OutError);
		}

		/**
		 * The same rebuild, run through §2.11.6's real decorator.
		 *
		 * `FStratGuidedOpening` is the module-side author of every guidance field. Nothing here
		 * writes one; if the shipped scenario names no guided opening this returns false and the
		 * clause bails rather than fabricating an active view to assert against.
		 */
		bool BuildDecorated(FStratViewModel& Out, FString& OutError) const
		{
			if (!Subsystem->BuildViewModel(Out, OutError))
			{
				return false;
			}

			FStratGuidedOpening    Opening;
			FStratSelectionMachine Machine;
			Opening.Begin(*Subsystem->GetBridge(), Out.ViewingSide, /*bSuppressed*/ false);
			if (!Opening.IsActive())
			{
				OutError = TEXT("the shipped scenario names no guided opening, so there is no "
				                "active directive for this clause to be about");
				return false;
			}

			Opening.Observe(Out, Machine);
			Opening.DecorateViewModel(Out);

			if (!Out.Guidance.bActive)
			{
				OutError = TEXT("DecorateViewModel left the strip inactive, so the 'active' side "
				                "of this clause has no subject");
				return false;
			}
			return true;
		}
	};

	/**
	 * The one struct kind `AlterOneField` is able to vary: EXACTLY `FIntPoint`.
	 *
	 * Factored out of the branch that uses it so that both of its directions can be exercised
	 * against real `FStructProperty` instances WITHOUT performing the write. That matters: the
	 * write goes through the property's own offset, so a probe that proved the refusal by
	 * calling the writing path on a foreign struct field would be relying on the very gate it
	 * is trying to test in order not to scribble on unrelated bytes. Asking the predicate is
	 * free of that circularity.
	 */
	static bool CanVaryStructField(const FStructProperty* AsStruct)
	{
		return AsStruct != nullptr && AsStruct->Struct == TBaseStructure<FIntPoint>::Get();
	}

	/**
	 * Moves exactly one field of a view, chosen through reflection rather than by name.
	 *
	 * WHY REFLECTION AND NOT A TYPED LIST. Two clauses need "a view that differs from this one
	 * in field N, for every N". A hand-written list of field names would go silently incomplete
	 * the day `FStratGuidanceView` grows an eleventh field -- and the clause it feeds would
	 * still pass, having quietly stopped covering the new one. Walking the struct's own
	 * property list cannot do that.
	 *
	 * REFUSES RATHER THAN SKIPS. A field of a kind this cannot alter returns false with a
	 * reason, so the caller reports the gap. Skipping it silently would be the same
	 * going-inert failure in a different place.
	 */
	static bool AlterOneField(FProperty* Property, FStratGuidanceView& InOut, FString& OutError)
	{
		if (Property == nullptr)
		{
			OutError = TEXT("a null property reached the field-varying helper");
			return false;
		}

		if (FBoolProperty* const AsBool = CastField<FBoolProperty>(Property))
		{
			AsBool->SetPropertyValue_InContainer(&InOut, !AsBool->GetPropertyValue_InContainer(&InOut));
			return true;
		}
		if (FTextProperty* const AsText = CastField<FTextProperty>(Property))
		{
			AsText->SetPropertyValue_InContainer(&InOut,
				FText::FromString(TEXT("a directive nobody authored")));
			return true;
		}
		if (FEnumProperty* const AsEnum = CastField<FEnumProperty>(Property))
		{
			void* const Value = AsEnum->ContainerPtrToValuePtr<void>(&InOut);
			FNumericProperty* const Underlying = AsEnum->GetUnderlyingProperty();
			Underlying->SetIntPropertyValue(Value, Underlying->GetSignedIntPropertyValue(Value) + 1);
			return true;
		}
		// A `UENUM` over `uint8` reaches reflection as an FByteProperty and not an FEnumProperty,
		// so both spellings are handled rather than one of them assumed.
		if (FNumericProperty* const AsNumber = CastField<FNumericProperty>(Property))
		{
			void* const Value = AsNumber->ContainerPtrToValuePtr<void>(&InOut);
			AsNumber->SetIntPropertyValue(Value, AsNumber->GetSignedIntPropertyValue(Value) + 1);
			return true;
		}
		if (FStructProperty* const AsStruct = CastField<FStructProperty>(Property))
		{
			// GATED ON THE STRUCT'S IDENTITY, not on "it is a struct at all".
			//
			// `FIntPoint` is the ONLY struct kind this helper knows how to move. Reinterpreting
			// some other struct field's bytes as an FIntPoint would let the control clause below
			// keep reporting green while proving nothing whatever about that field -- the exact
			// going-inert failure this property-walk design exists to prevent, relocated one
			// level down into the helper. Anything that is not exactly FIntPoint therefore falls
			// through to the refusal at the bottom of this function, which names the field, so
			// the day `FStratGuidanceView` grows a second struct field
			// `T-INT-05.GuidanceComparisonDistinguishesViews` goes RED with that field's name in
			// the message instead of passing on garbage.
			//
			// WHAT IS COVERED, honestly: FIntPoint and nothing else in the struct family.
			// `T-INT-05.GuidanceFieldVaryingHelperGatesOnStructIdentity` exercises BOTH
			// directions of this gate -- accept and refuse -- against real reflection data.
			if (CanVaryStructField(AsStruct))
			{
				FIntPoint* const Point = AsStruct->ContainerPtrToValuePtr<FIntPoint>(&InOut);
				Point->X += 3;
				Point->Y += 5;
				return true;
			}
		}

		OutError = FString::Printf(
			TEXT("field '%s' is of a kind this helper cannot vary, so the clauses that walk ")
			TEXT("FStratGuidanceView are unproven on it -- extend AlterOneField"),
			*Property->GetName());
		return false;
	}

	/** This fixture has no Blueprint defaults, so the board reports every terrain kind unmeshed. */
	static void ExpectTheUnmeshedFixtureWarning(FAutomationTestBase& Test)
	{
		Test.AddExpectedMessagePlain(TEXT("no tile mesh for terrain"), ELogVerbosity::Warning,
			EAutomationExpectedMessageFlags::Contains, /*Occurrences*/ 0);
	}

	// -----------------------------------------------------------------------
	// The view-decorator seam. Everything below serves the clauses added at the
	// end of this file, and nothing above it uses any of it.
	// -----------------------------------------------------------------------

	/**
	 * Sec 2.11.6's REAL author, registered as the subsystem's view decorator, recording what
	 * it produced.
	 *
	 * WHERE THE EXPECTATIONS COME FROM, which is the only thing that makes the decorator
	 * clauses worth their runtime. Not one guidance field is typed out below: this runs
	 * `FStratGuidedOpening` -- the module-side author of every guidance field, and the same
	 * object `AStratPlayerController` holds by value -- and records the block it wrote. A
	 * clause then compares the STRIP against that recording through `SameGuidance`. Neither
	 * side of that comparison is computed by the test.
	 *
	 * ARMED LAZILY, MIRRORING `AStratPlayerController::TryArmGuidedOpening`, and for the same
	 * reason: one clause registers this BEFORE `StartMatch`, and until the match exists there
	 * is no seeded bridge to call `Begin` with. Arming at registration time would have made
	 * that clause impossible to write, and it is the clause about the ordering the second
	 * defect lived in.
	 *
	 * IT IS NOT A DOUBLE AND RECORDS NOTHING THE PRODUCTION PATH DOES NOT DO. `Decorate` runs
	 * `Observe` then `DecorateViewModel`, in that order, which is `AStratPlayerController::
	 * DecorateForPresentation` minus the selection machine's own two bits -- those are pinned
	 * by `T-INT-05.LockArisesFromTheGuidanceLayer` and are not this file's subject.
	 */
	struct FRecordingDecorator
	{
		UStratMatchSubsystem*  Subsystem = nullptr;
		FStratGuidedOpening    Opening;
		FStratSelectionMachine Machine;

		/** How many times the subsystem has run this. The instrument for "did it decorate". */
		int32 Calls = 0;

		/** Whether `Begin` has taken. False forever in a world whose match never started. */
		bool bArmed = false;

		/** The guidance block this decorator LEFT ON the last model it was handed. */
		FStratGuidanceView LastProduced;

		void Decorate(FStratViewModel& Model)
		{
			++Calls;

			if (!bArmed && Subsystem != nullptr)
			{
				const FStratBridge* const Bridge = Subsystem->GetBridge();
				if (Bridge != nullptr && Bridge->IsSeeded())
				{
					Opening.Begin(*Bridge, Subsystem->GetViewingSide(), /*bSuppressed*/ false);
					bArmed = true;
				}
			}

			if (bArmed)
			{
				Opening.Observe(Model, Machine);
				Opening.DecorateViewModel(Model);
			}

			// READ BACK OFF THE MODEL AFTER THE WRITE, so this is what the decorator PRODUCED
			// and not what it was handed. When nothing armed, it is whatever the build left --
			// which is the honest record of a decorator that had nothing to say.
			LastProduced = Model.Guidance;
		}
	};

	/**
	 * Registers a fresh recorder on the harness's subsystem and hands it back.
	 *
	 * HELD ALIVE BY THE DELEGATE ITSELF, through a `TSharedRef` the lambda captures by value.
	 * Production binds with `CreateUObject`, which a plain struct cannot use; capturing the
	 * shared reference gives the same property that matters here -- the subsystem cannot call
	 * into a recorder that has gone away -- without the recorder having to be a `UObject`.
	 */
	static TSharedRef<FRecordingDecorator> RegisterRecorder(FRouteHarness& H)
	{
		TSharedRef<FRecordingDecorator> Recorder = MakeShared<FRecordingDecorator>();
		Recorder->Subsystem = H.Subsystem;
		H.Subsystem->SetViewDecorator(FStratViewDecorator::CreateLambda(
			[Recorder](FStratViewModel& Model) { Recorder->Decorate(Model); }));
		return Recorder;
	}

	/**
	 * `RefreshPresentation`, with its return value handed back rather than asserted on.
	 *
	 * IT RETURNS FALSE IN THIS FIXTURE AND THAT IS NOT A DEFECT. `FRouteHarness` deliberately
	 * does not dispatch the HUD's `BeginPlay`, so the HUD holds no scoreboard widget and
	 * `RefreshScoreboard` refuses -- and that refusal is the LAST step of
	 * `RefreshPresentation`, reached after `ApplyView` has already run and already pushed. A
	 * clause that asserted the return true would be asserting the presence of a scoreboard
	 * asset, which is the editor lane's subject and not this file's. The reason is carried
	 * into every failure message instead, so a refusal arriving from an EARLIER step -- a
	 * build that did not build -- is not mistaken for this known one.
	 */
	static bool RefreshAndReport(FRouteHarness& H, FString& OutReason)
	{
		return H.Subsystem->RefreshPresentation(OutReason);
	}
}

// ---------------------------------------------------------------------------
// The instrument, before anything relies on it.
//
// Every clause in this file reads "the value that arrived equals the value on the model" off
// `SameGuidance`. If that comparator could not answer NO, all of them would be green and none
// of them would be a gate. This shows it answering no, once per field the struct declares,
// driven off the struct's own property list rather than a typed list of field names.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceInstrumentDistinguishesTest,
	"Stratocracy.StratPlay.T-INT-05.GuidanceComparisonDistinguishesViews",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceInstrumentDistinguishesTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceRoute;

	const FStratGuidanceView Default;
	TestTrue(TEXT("the comparator calls a view equal to itself"), SameGuidance(Default, Default));

	UScriptStruct* const Struct = FStratGuidanceView::StaticStruct();
	if (!TestNotNull(TEXT("FStratGuidanceView publishes reflection data to compare through"), Struct))
	{
		return false;
	}

	int32 FieldsSeen = 0;
	for (TFieldIterator<FProperty> It(Struct); It; ++It)
	{
		FProperty* const Property = *It;
		++FieldsSeen;

		FStratGuidanceView Altered;
		FString AlterError;
		if (!AlterOneField(Property, Altered, AlterError))
		{
			AddError(AlterError);
			continue;
		}

		TestFalse(*FString::Printf(
				TEXT("a view differing only in '%s' is not called equal (default: %s; altered: %s)"),
				*Property->GetName(), *Describe(Default), *Describe(Altered)),
			SameGuidance(Default, Altered));
	}

	TestTrue(TEXT("the struct declared fields for this control to walk"), FieldsSeen > 0);
	return true;
}

// ---------------------------------------------------------------------------
// The control ON the control -- the field-varying helper gates on struct IDENTITY.
//
// `GuidanceComparisonDistinguishesViews` above is only a gate for as long as
// `AlterOneField` genuinely alters the field it was handed. Its struct branch used to
// reinterpret ANY struct field as an `FIntPoint`, which was harmless only by the accident
// that `FIntPoint` is the single struct kind `FStratGuidanceView` declares. The day a second
// struct field arrived, that branch would have written eight bytes of nonsense through a
// foreign offset, the comparator would have called the two views different for the wrong
// reason, and the control clause would have stayed GREEN while covering nothing about the new
// field. That is a going-inert failure one level below the one the property walk was built to
// prevent, so it gets its own clause rather than a comment.
//
// BOTH DIRECTIONS ARE EXERCISED, ON REAL REFLECTION DATA, AND NEITHER CAN PASS VACUOUSLY:
//   - ACCEPT: every `FStructProperty` `FStratGuidanceView` actually declares is accepted.
//     A predicate that refused everything reddens here. This is deliberately a walk and not a
//     lookup of `ObjectiveHex` by name, so a second struct field on the view is REQUIRED to be
//     handled rather than silently unmentioned.
//   - REFUSE: a real `FStructProperty` of a different struct kind -- `FStratViewModel::Guidance`,
//     which is an `FStratGuidanceView` -- is refused, and `AlterOneField` itself refuses it
//     with a message naming the field. A predicate that accepted everything reddens here.
//
// WHY THE PREDICATE AND NOT ONLY THE WRITER. The refuse direction is also asserted through
// `AlterOneField`, which is safe BECAUSE the gate holds. The accept direction is asked of
// `CanVaryStructField` rather than by writing through a foreign container, because a probe
// that had to survive the ungated write in order to report on it would be leaning on the very
// property under test.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceFieldVaryingHelperGatesOnStructIdentityTest,
	"Stratocracy.StratPlay.T-INT-05.GuidanceFieldVaryingHelperGatesOnStructIdentity",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceFieldVaryingHelperGatesOnStructIdentityTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceRoute;

	UScriptStruct* const ViewStruct = FStratGuidanceView::StaticStruct();
	UScriptStruct* const ModelStruct = FStratViewModel::StaticStruct();
	if (!TestNotNull(TEXT("FStratGuidanceView publishes reflection data"), ViewStruct) ||
		!TestNotNull(TEXT("FStratViewModel publishes reflection data"), ModelStruct))
	{
		return false;
	}

	// ---- PREMISE: the two struct kinds below are genuinely different ----------------------
	//
	// Checked first, because if `FStratViewModel::Guidance` were somehow an FIntPoint the
	// refuse direction would be asking nothing.
	if (!TestTrue(
			TEXT("PREMISE: FStratGuidanceView is not FIntPoint, so a property of it is a real "
			     "non-FIntPoint struct field for the refuse direction to be about"),
			ViewStruct != TBaseStructure<FIntPoint>::Get()))
	{
		return false;
	}

	// ---- ACCEPT: every struct field the view declares is one the helper can vary ----------
	int32 StructFieldsSeen = 0;
	for (TFieldIterator<FProperty> It(ViewStruct); It; ++It)
	{
		FStructProperty* const AsStruct = CastField<FStructProperty>(*It);
		if (AsStruct == nullptr)
		{
			continue;
		}
		++StructFieldsSeen;

		TestTrue(*FString::Printf(
				TEXT("the field-varying helper accepts FStratGuidanceView's struct field '%s' "
				     "(a '%s'). If this is red, the view has grown a struct field AlterOneField "
				     "cannot move -- extend AlterOneField and CanVaryStructField to cover it; do "
				     "NOT widen the gate to 'any struct', which is what made the helper inert."),
				*AsStruct->GetName(),
				AsStruct->Struct != nullptr ? *AsStruct->Struct->GetName() : TEXT("<null>")),
			CanVaryStructField(AsStruct));
	}
	TestTrue(TEXT("FStratGuidanceView declares at least one struct field for the accept "
	              "direction to be about"), StructFieldsSeen > 0);

	// ---- REFUSE: a struct field of any other kind is turned away, by name -----------------
	FStructProperty* const Foreign =
		CastField<FStructProperty>(FindFProperty<FProperty>(ModelStruct, FName(TEXT("Guidance"))));
	if (!TestNotNull(
			TEXT("FStratViewModel::Guidance is a reflected struct property, which is this "
			     "clause's stand-in for 'a struct field of a kind the helper does not know'"),
			Foreign))
	{
		return false;
	}

	TestFalse(*FString::Printf(
			TEXT("the field-varying helper REFUSES a non-FIntPoint struct field ('%s', a '%s'). "
			     "If this is red the gate accepts any struct again, and "
			     "T-INT-05.GuidanceComparisonDistinguishesViews is no longer proving anything "
			     "about a struct field it appears to cover."),
			*Foreign->GetName(),
			Foreign->Struct != nullptr ? *Foreign->Struct->GetName() : TEXT("<null>")),
		CanVaryStructField(Foreign));

	// And the writer built on the predicate refuses it too, with the field named in its reason.
	FStratGuidanceView Untouched;
	FStratGuidanceView Subject;
	FString AlterError;
	const bool bAltered = AlterOneField(Foreign, Subject, AlterError);

	TestFalse(TEXT("AlterOneField refuses a struct field it cannot vary rather than "
	               "reinterpreting its bytes"), bAltered);
	TestTrue(*FString::Printf(
			TEXT("the refusal names the field it turned away (reported: '%s')"), *AlterError),
		AlterError.Contains(Foreign->GetName(), ESearchCase::CaseSensitive));
	TestTrue(TEXT("and the refused call left the view untouched, so nothing was written "
	              "through the foreign field's offset"),
		SameGuidance(Untouched, Subject));

	return true;
}

// ---------------------------------------------------------------------------
// Clause 1 -- ApplyView pushes the guidance it was HANDED.
//
// `ApplyView` is the single reconcile choke point downstream of every presentation path,
// `RefreshPresentation` included (it calls `ApplyView`). What must be true of it is that the
// value reaching the strip is `Model.Guidance` and nothing else: not a value rebuilt from the
// bridge, not one re-queried from `FStratGuidedOpening`, not one the HUD assembled. The
// expectation is therefore the argument itself, read back off the model that was applied.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceApplyViewPushesTheModelsGuidanceTest,
	"Stratocracy.StratPlay.T-INT-05.ApplyViewPushesTheModelsGuidance",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceApplyViewPushesTheModelsGuidanceTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceRoute;
	ExpectTheUnmeshedFixtureWarning(*this);

	FRouteHarness H;
	FString Error;
	if (!TestTrue(TEXT("a live match with a findable HUD and a strip on it"), H.Arm(true, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Decorated;
	if (!TestTrue(TEXT("the decorator produces an active directive to route"),
			H.BuildDecorated(Decorated, Error)))
	{
		AddError(Error);
		return false;
	}

	// The strip starts on its own default, which the decorated view is not -- otherwise the
	// assertion below would hold whether or not anything was pushed.
	FStratGuidanceView Before;
	if (!TestTrue(TEXT("the installed strip is readable"), H.ReadStrip(Before)) ||
		!TestFalse(TEXT("and does not already hold the view about to be pushed"),
			SameGuidance(Before, Decorated.Guidance)))
	{
		return false;
	}

	H.Subsystem->ApplyView(Decorated);

	FStratGuidanceView Held;
	if (!TestTrue(TEXT("the strip is readable after the reconcile"), H.ReadStrip(Held)))
	{
		return false;
	}

	TestTrue(*FString::Printf(
			TEXT("the strip holds the applied model's own Guidance, field for field ")
			TEXT("(model: %s; strip: %s)"),
			*Describe(Decorated.Guidance), *Describe(Held)),
		SameGuidance(Held, Decorated.Guidance));

	// AND IT IS THE ARGUMENT, NOT A REBUILD. `BuildViewModel` run again undecorated is what a
	// re-query would produce; if the strip matched THAT it would be reading the world rather
	// than the value it was handed.
	FStratViewModel Rebuilt;
	if (TestTrue(TEXT("the subsystem can rebuild an undecorated model to contrast against"),
			H.BuildUndecorated(Rebuilt, Error)))
	{
		TestFalse(
			TEXT("the strip does not hold what an undecorated re-query would have produced -- so "
			     "the pushed value came from the argument, not from asking again"),
			SameGuidance(Held, Rebuilt.Guidance));
	}

	return true;
}

// ---------------------------------------------------------------------------
// Clause 2 -- an undecorated rebuild CLEARS the strip. The falsification clause.
//
// This is the one that matters. T-INT-05 says the screen is a function of the current view
// model alone; the way a projection stops being one is by becoming sticky -- the widget keeps
// the last thing it liked, and the strip goes on showing a directive after the window that
// authored it has closed. Sec 2.11.6 calls that the one outcome it cannot produce, and
// `UStratGuidanceWidget::PushGuidance` records "assigns unconditionally" as the reason it
// cannot.
//
// So: an ACTIVE view is applied, and a model rebuilt with NO decorate step is applied over it.
// The expectation for the second push is `Rebuilt.Guidance` -- read off the model, never
// typed -- and the property that must hold is that the strip agrees with the LAST model and
// not with the union of both.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceUndecoratedRebuildClearsTheStripTest,
	"Stratocracy.StratPlay.T-INT-05.UndecoratedRebuildClearsTheStrip",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceUndecoratedRebuildClearsTheStripTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceRoute;
	ExpectTheUnmeshedFixtureWarning(*this);

	FRouteHarness H;
	FString Error;
	if (!TestTrue(TEXT("a live match with a findable HUD and a strip on it"), H.Arm(true, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Decorated;
	FStratViewModel Rebuilt;
	if (!TestTrue(TEXT("the decorator produces an active directive to make stale"),
			H.BuildDecorated(Decorated, Error)) ||
		!TestTrue(TEXT("and the subsystem rebuilds a model with no decorate step"),
			H.BuildUndecorated(Rebuilt, Error)))
	{
		AddError(Error);
		return false;
	}

	// The subject has to be armed or the clause is about nothing: the undecorated rebuild must
	// genuinely carry no directive, and the decorated one must genuinely carry one.
	if (!TestFalse(
			TEXT("a rebuild with no decorate step carries no active directive -- otherwise this "
			     "clause is not the case the GDD is talking about"),
			Rebuilt.Guidance.bActive) ||
		!TestTrue(TEXT("and the decorated rebuild does carry one"), Decorated.Guidance.bActive))
	{
		return false;
	}

	FStratGuidanceView Held;

	H.Subsystem->ApplyView(Decorated);
	if (!TestTrue(TEXT("the strip is readable"), H.ReadStrip(Held)) ||
		!TestTrue(*FString::Printf(
				TEXT("the directive reached the strip before it is asked to survive (strip: %s)"),
				*Describe(Held)),
			SameGuidance(Held, Decorated.Guidance)))
	{
		return false;
	}

	H.Subsystem->ApplyView(Rebuilt);
	if (!TestTrue(TEXT("the strip is readable after the undecorated rebuild"), H.ReadStrip(Held)))
	{
		return false;
	}

	TestEqual(
		TEXT("a rebuild that no longer carries a directive leaves the strip inactive -- the "
		     "widget's copy is not an independent source of truth"),
		Held.bActive, Rebuilt.Guidance.bActive);

	TestTrue(*FString::Printf(
			TEXT("and the strip agrees with the LAST model in every field, not with the union of "
			     "the two (last model: %s; strip: %s)"),
			*Describe(Rebuilt.Guidance), *Describe(Held)),
		SameGuidance(Held, Rebuilt.Guidance));

	// Applying the pair again shows the strip is a function of the last model and not of the
	// order it has seen models in.
	H.Subsystem->ApplyView(Decorated);
	if (TestTrue(TEXT("the strip is readable"), H.ReadStrip(Held)))
	{
		TestTrue(TEXT("re-applying the decorated model restores the directive"),
			SameGuidance(Held, Decorated.Guidance));
	}
	H.Subsystem->ApplyView(Rebuilt);
	if (TestTrue(TEXT("the strip is readable"), H.ReadStrip(Held)))
	{
		TestTrue(TEXT("and applying the undecorated one clears it again"),
			SameGuidance(Held, Rebuilt.Guidance));
	}

	return true;
}

// ---------------------------------------------------------------------------
// Clause 3 -- no strip configured is a legitimate configuration, not a failure.
//
// `AStratScoreboardHUD`'s header records that an unset `GuidanceWidgetClass` is a session that
// wants no guided opening on screen, and `PushGuidance` is void because a `bool` return would
// be false on every reconcile of every such session. The property that must hold is that the
// rest of the reconcile is UNAFFECTED: the units still spawn, move and die by the model. If a
// guidance push could throw a session without a strip off the reconcile path, the strip would
// have become a precondition of the board.
//
// NEEDS NO ASSET, which is why it is a headless gate rather than an editor one.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceReconcileCompletesWithNoStripTest,
	"Stratocracy.StratPlay.T-INT-05.ReconcileCompletesWithNoStripConfigured",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceReconcileCompletesWithNoStripTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceRoute;
	ExpectTheUnmeshedFixtureWarning(*this);

	FRouteHarness H;
	FString Error;
	if (!TestTrue(TEXT("a live match with a findable HUD and NO strip on it"), H.Arm(false, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratGuidanceView Held;
	if (!TestFalse(TEXT("the HUD genuinely has no strip, so the no-op is the case under test"),
			H.ReadStrip(Held)))
	{
		return false;
	}

	FStratViewModel Decorated;
	if (!TestTrue(TEXT("the decorator produces a directive to offer a HUD with no strip"),
			H.BuildDecorated(Decorated, Error)))
	{
		AddError(Error);
		return false;
	}

	// The HUD's own entry point, called directly: it must complete and must not invent a strip.
	H.Hud->PushGuidance(Decorated.Guidance);
	TestFalse(TEXT("a push at a HUD with no strip creates none"), H.ReadStrip(Held));

	// And now the whole reconcile, which is the part that could have been derailed.
	FStratViewModel ModelA = Decorated;
	if (!TestTrue(TEXT("the scenario ships enough units for a set difference to mean something"),
			ModelA.Units.Num() >= 2))
	{
		return false;
	}

	FStratViewModel ModelB = ModelA;
	const int32 OnlyInAId = ModelB.Units[0].UnitId;
	ModelB.Units.RemoveAt(0);

	TSet<int32> ExpectedIds;
	for (const FStratUnitView& View : ModelB.Units)
	{
		ExpectedIds.Add(View.UnitId);
	}

	H.Subsystem->ApplyView(ModelA);
	TestNotNull(TEXT("with no strip, A's units still reach the board"),
		H.Subsystem->FindUnitActor(OnlyInAId));

	H.Subsystem->ApplyView(ModelB);
	TestNull(TEXT("with no strip, the unit dropped from B is still destroyed"),
		H.Subsystem->FindUnitActor(OnlyInAId));

	const TSet<int32> Live = LiveUnitIds(H.Scope.World);
	TestEqual(TEXT("with no strip, the world holds exactly as many unit actors as B has units"),
		Live.Num(), ExpectedIds.Num());
	TestTrue(TEXT("and the actor set equals B's unit ids exactly"),
		Live.Includes(ExpectedIds) && ExpectedIds.Includes(Live));

	TestEqual(TEXT("and the model the subsystem recorded is the one it was last given"),
		H.Subsystem->GetViewModel().Units.Num(), ModelB.Units.Num());

	TestFalse(TEXT("and no strip appeared over the course of a whole reconcile"),
		H.ReadStrip(Held));

	return true;
}

// ---------------------------------------------------------------------------
// Clause 4 -- the widget holds what it was pushed, INCLUDING an inactive push over an active
// one.
//
// The stale-directive failure, observed at `UStratGuidanceWidget::PushGuidance` itself rather
// than through the HUD, so that a regression names the class that caused it.
// `UStratGuidanceWidget::PushGuidance` records "assigns unconditionally, including when the
// incoming view is inactive"; this is that sentence made falsifiable. An
// `if (InGuidance.bActive)` added to that method tomorrow reddens exactly here.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceStripHoldsWhatItWasPushedTest,
	"Stratocracy.StratPlay.T-INT-05.StripHoldsWhatItWasPushed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceStripHoldsWhatItWasPushedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceRoute;
	ExpectTheUnmeshedFixtureWarning(*this);

	FRouteHarness H;
	FString Error;
	if (!TestTrue(TEXT("a live match with a findable HUD and a strip on it"), H.Arm(true, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratViewModel Decorated;
	FStratViewModel Rebuilt;
	if (!TestTrue(TEXT("the decorator produces an active directive to push"),
			H.BuildDecorated(Decorated, Error)) ||
		!TestTrue(TEXT("and the subsystem rebuilds an undecorated one to push after it"),
			H.BuildUndecorated(Rebuilt, Error)))
	{
		AddError(Error);
		return false;
	}

	if (!TestTrue(TEXT("the two views differ, so the second push below is a real transition"),
			!SameGuidance(Decorated.Guidance, Rebuilt.Guidance)))
	{
		return false;
	}

	FStratGuidanceView Held;

	if (!TestTrue(TEXT("the active view is pushed straight at the widget"),
			StratTestPushAtGuidanceStripDirectly(H.Hud, Decorated.Guidance)) ||
		!TestTrue(TEXT("the strip is readable"), H.ReadStrip(Held)))
	{
		return false;
	}
	TestTrue(*FString::Printf(
			TEXT("the widget holds the active view it was pushed (pushed: %s; held: %s)"),
			*Describe(Decorated.Guidance), *Describe(Held)),
		SameGuidance(Held, Decorated.Guidance));

	// The push a sticky widget would ignore.
	if (!TestTrue(TEXT("the inactive view is pushed over it"),
			StratTestPushAtGuidanceStripDirectly(H.Hud, Rebuilt.Guidance)) ||
		!TestTrue(TEXT("the strip is readable"), H.ReadStrip(Held)))
	{
		return false;
	}
	TestEqual(
		TEXT("an INACTIVE push over an active one lands -- the assignment is unconditional"),
		Held.bActive, Rebuilt.Guidance.bActive);
	TestTrue(*FString::Printf(
			TEXT("and it lands in every field (pushed: %s; held: %s)"),
			*Describe(Rebuilt.Guidance), *Describe(Held)),
		SameGuidance(Held, Rebuilt.Guidance));

	// Pushing the same value twice is not a special case either.
	StratTestPushAtGuidanceStripDirectly(H.Hud, Rebuilt.Guidance);
	if (TestTrue(TEXT("the strip is readable"), H.ReadStrip(Held)))
	{
		TestTrue(TEXT("a repeated push leaves the same value"),
			SameGuidance(Held, Rebuilt.Guidance));
	}

	return true;
}

// ---------------------------------------------------------------------------
// Clause 5 -- the HUD is a CONDUIT, not a second author of when the window closes.
//
// `AStratScoreboardHUD::PushGuidance` must branch on ONE thing -- whether there is a strip --
// and on nothing in the view it is carrying. An `if (InGuidance.bActive)` there would make the
// HUD the thing that decides when the guidance window ends, a decision `FStratGuidedOpening`
// already owns; a branch on `bShowsWindowEndTag` or `bEndTurnGated` would be the same defect
// wearing a different field.
//
// WHAT IS ACTUALLY MEASURED, AND IT IS NOT THE SOURCE SHAPE. An automation test cannot read a
// function body. What it can do is offer that method every distinguishable shape of
// `FStratGuidanceView` -- one per field the struct declares, generated from the struct's own
// property list -- and require each to arrive at the strip unaltered. Any branch that skips,
// substitutes or edits on the strength of a field breaks at least one of those pushes. A
// branch that reads a field and changes nothing survives, and is not a defect.
//
// See this file's header block for why a .cpp text scan was NOT written instead.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceHudForwardsEveryShapeTest,
	"Stratocracy.StratPlay.T-INT-05.HudForwardsEveryGuidanceShapeUnchanged",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceHudForwardsEveryShapeTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceRoute;
	ExpectTheUnmeshedFixtureWarning(*this);

	FRouteHarness H;
	FString Error;
	if (!TestTrue(TEXT("a live match with a findable HUD and a strip on it"), H.Arm(true, Error)))
	{
		AddError(Error);
		return false;
	}

	// The two ends of the range that matters most: a real decorated directive, and the state a
	// closed window leaves behind. Both come from the module; neither is typed here.
	FStratViewModel Decorated;
	FStratViewModel Rebuilt;
	if (!TestTrue(TEXT("the decorator produces an active directive"),
			H.BuildDecorated(Decorated, Error)) ||
		!TestTrue(TEXT("and the subsystem rebuilds an undecorated one"),
			H.BuildUndecorated(Rebuilt, Error)))
	{
		AddError(Error);
		return false;
	}

	FStratGuidanceView Held;

	H.Hud->PushGuidance(Decorated.Guidance);
	if (!TestTrue(TEXT("the strip is readable"), H.ReadStrip(Held)))
	{
		return false;
	}
	TestTrue(TEXT("the HUD forwards an ACTIVE view unchanged"),
		SameGuidance(Held, Decorated.Guidance));

	H.Hud->PushGuidance(Rebuilt.Guidance);
	if (!TestTrue(TEXT("the strip is readable"), H.ReadStrip(Held)))
	{
		return false;
	}
	TestEqual(
		TEXT("the HUD forwards an INACTIVE view too -- it does not decide when the window closes"),
		Held.bActive, Rebuilt.Guidance.bActive);
	TestTrue(TEXT("and it forwards it in every field"), SameGuidance(Held, Rebuilt.Guidance));

	// One shape per field, so no single field can be the one the HUD is quietly reading.
	UScriptStruct* const Struct = FStratGuidanceView::StaticStruct();
	int32 ShapesOffered = 0;
	for (TFieldIterator<FProperty> It(Struct); It; ++It)
	{
		FProperty* const Property = *It;

		// Started from the decorated view, so each shape is a real directive with one field
		// moved rather than a default struct with one bit set.
		FStratGuidanceView Shape = Decorated.Guidance;
		FString AlterError;
		if (!AlterOneField(Property, Shape, AlterError))
		{
			AddError(AlterError);
			continue;
		}
		++ShapesOffered;

		// Cleared to something else first, so "unchanged" cannot be satisfied by the value that
		// happened to be there already.
		H.Hud->PushGuidance(FStratGuidanceView());

		H.Hud->PushGuidance(Shape);
		if (!TestTrue(TEXT("the strip is readable"), H.ReadStrip(Held)))
		{
			return false;
		}
		TestTrue(*FString::Printf(
				TEXT("the HUD forwards a view varying in '%s' unchanged (offered: %s; strip: %s)"),
				*Property->GetName(), *Describe(Shape), *Describe(Held)),
			SameGuidance(Held, Shape));
	}

	TestTrue(TEXT("a shape was offered for every field the struct declares"), ShapesOffered > 0);
	return true;
}

// ---------------------------------------------------------------------------
// THE TWO DELIVERY DEFECTS, measured in PIE on 2026-08-21 and fixed in the pass these clauses
// were written for. Neither had a clause, which is why both shipped green.
//
// DEFECT 1 -- THE STRIP DID NOT EXIST YET. `AStratPlayerController::BeginPlay` reached
// `ApplyView` with the session's only decorated model before `AStratScoreboardHUD::BeginPlay`
// had created a strip; `PushGuidance`'s null check discarded it and nothing reconciled again.
// Pinned by `StripCreatedAfterAPushStillCarriesIt` and the three clauses after it.
//
// DEFECT 2 -- EVERY SUBSYSTEM-SIDE RECONCILE CLEARED THE STRIP. `SetViewingSide` ->
// `RefreshPresentation` -> undecorated `ApplyView` -> `PushGuidance(default)`. Pinned by
// `SetViewingSideDoesNotClearAnActiveDirective` and the four clauses around it.
//
// WHERE THE EXPECTATIONS COME FROM, without exception below: `FStratGuidedOpening`, run as
// the registered decorator by `FRecordingDecorator` and read back off the model it wrote. Not
// one guidance field is typed out in this section, and every comparison is `SameGuidance` --
// `CompareScriptStruct` over the struct's own reflection data, which is case-SENSITIVE where
// `TestEqual` on an `FString` in this project is not.
//
// WHAT THIS SECTION DOES NOT PIN. It does not pin `AStratPlayerController::BeginPlay`'s
// registration itself: `BeginPlay` is not dispatched in a transient world built this way, so
// no clause here observes the line that calls `SetViewDecorator`. What is pinned is the seam
// it registers with, from both ends -- that `BuildViewModelForPresentation` runs a registered
// decorator and `BuildViewModel` does not, and that `RefreshFromMachine` decorates whether or
// not any registration happened. The registration line itself is covered only by the PIE
// session it was measured in, and that is stated rather than left to be assumed.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// DEFECT 1. The production ordering: the match starts, pushes its only decorated model at a
// HUD with no strip, and the strip appears afterwards.
//
// THE PUSH IS NOT SYNTHESISED. `StartTheMatch` performs it, through `StartMatchInternal` ->
// `BuildViewModelForPresentation` -> `ApplyView` -> `PushGuidance`, with the recorder
// registered beforehand -- the same registration order production has when a controller's
// `BeginPlay` precedes the GameMode's. The expectation is the recorder's own output.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceStripCreatedAfterAPushTest,
	"Stratocracy.StratPlay.T-INT-05.StripCreatedAfterAPushStillCarriesIt",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceStripCreatedAfterAPushTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceRoute;
	ExpectTheUnmeshedFixtureWarning(*this);

	FRouteHarness H;
	FString Error;
	if (!TestTrue(TEXT("a world with a findable HUD and NO strip on it"),
			H.SpawnWorldAndActors(/*bStratController*/ false, Error)))
	{
		AddError(Error);
		return false;
	}

	TSharedRef<FRecordingDecorator> Recorder = RegisterRecorder(H);

	if (!TestTrue(TEXT("the match starts, and its own reconcile pushes at a HUD with no strip"),
			H.StartTheMatch(Error)))
	{
		AddError(Error);
		return false;
	}

	// THE SUBJECT HAS TO BE ARMED OR THE CLAUSE IS ABOUT NOTHING. If the start reconcile did
	// not carry a live directive there is nothing for a later strip to have missed.
	if (!TestTrue(TEXT("the start reconcile went through the registered decorator"),
			Recorder->Calls > 0) ||
		!TestTrue(TEXT("and that decorator produced an ACTIVE directive to be lost"),
			Recorder->LastProduced.bActive))
	{
		return false;
	}

	const FStratGuidanceView Dropped = Recorder->LastProduced;

	if (!TestTrue(TEXT("a strip is installed after the push"), H.InstallStrip(Error)))
	{
		AddError(Error);
		return false;
	}

	// THE CONTROL, MEASURED AND NOT ARGUED. A freshly installed strip must NOT already hold
	// the directive, or the assertion below would pass on a value that was never delivered.
	FStratGuidanceView Held;
	if (!TestTrue(TEXT("the new strip is readable"), H.ReadStrip(Held)))
	{
		return false;
	}
	if (!TestFalse(*FString::Printf(
				TEXT("a strip that appeared after the push does not already carry it (strip: %s)"),
				*Describe(Held)),
			SameGuidance(Held, Dropped)))
	{
		return false;
	}

	TestTrue(TEXT("delivery reports that it had both a strip and something to deliver"),
		H.Hud->DeliverLatestGuidance());

	if (!TestTrue(TEXT("the strip is readable after delivery"), H.ReadStrip(Held)))
	{
		return false;
	}

	TestTrue(*FString::Printf(
			TEXT("the strip carries the directive the match start pushed, in every field ")
			TEXT("(pushed: %s; strip: %s)"),
			*Describe(Dropped), *Describe(Held)),
		SameGuidance(Held, Dropped));

	return true;
}

// ---------------------------------------------------------------------------
// The other half of that member pair, and the reason `bGuidanceEverPushed` is a separate
// bool: a default-constructed `FStratGuidanceView` is a REAL state, so "the cache equals the
// default" cannot mean "nothing has been cached". This project has already paid once for
// treating a real default as an unset marker, in `FStratMatchConfig::SaveSlotName`.
//
// NO MATCH IS STARTED HERE, deliberately -- starting one is what performs a push.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceDeliverIsSilentBeforeAnyPushTest,
	"Stratocracy.StratPlay.T-INT-05.DeliverLatestGuidanceIsSilentBeforeAnyPush",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceDeliverIsSilentBeforeAnyPushTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceRoute;

	// NO `ExpectTheUnmeshedFixtureWarning` HERE, AND THAT IS MEASURED RATHER THAN TIDIED.
	// `AddExpectedMessagePlain` with `Occurrences 0` means AT LEAST ONE, not "any number":
	// this clause starts no match, so no board is spawned, no terrain is meshed and the
	// warning never fires -- and declaring it expected failed the clause with "Expected
	// suppressed ('Warning') level log message or higher matching 'no tile mesh for terrain'
	// did not occur." Every other clause in this file starts a match and does declare it.
	FRouteHarness H;
	FString Error;
	if (!TestTrue(TEXT("a world with a HUD that no match has ever pushed at"),
			H.SpawnWorldAndActors(/*bStratController*/ false, Error)) ||
		!TestTrue(TEXT("and a strip on it"), H.InstallStrip(Error)))
	{
		AddError(Error);
		return false;
	}

	FStratGuidanceView Before;
	if (!TestTrue(TEXT("the strip is readable"), H.ReadStrip(Before)))
	{
		return false;
	}

	TestFalse(TEXT("delivery on a HUD nothing has pushed at reports nothing delivered"),
		H.Hud->DeliverLatestGuidance());

	FStratGuidanceView After;
	if (!TestTrue(TEXT("the strip is still readable"), H.ReadStrip(After)))
	{
		return false;
	}

	// UNCHANGED, not "equal to the default". The two are the same value here and are not the
	// same claim, and it is the first that this function owes its caller.
	TestTrue(*FString::Printf(
			TEXT("and it left the strip untouched in every field (before: %s; after: %s)"),
			*Describe(Before), *Describe(After)),
		SameGuidance(Before, After));

	// THE OTHER FALSE, a different question with the same answer: no strip at all. A HUD
	// outside the harness, so nothing has reached it either.
	AStratScoreboardHUD* const Bare = H.Scope.World->SpawnActor<AStratScoreboardHUD>();
	if (!TestNotNull(TEXT("a second HUD, with neither a strip nor a push"), Bare))
	{
		return false;
	}
	TestFalse(TEXT("delivery on a HUD with no strip reports nothing delivered"),
		Bare->DeliverLatestGuidance());

	return true;
}

// ---------------------------------------------------------------------------
// A REPLAY, NOT A CONSUMABLE QUEUE. A one-shot latch was the rejected alternative, and this is
// the difference it would have made observable: a second call returning false, and a strip
// that was overwritten in between never coming back.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceDeliverIsIdempotentTest,
	"Stratocracy.StratPlay.T-INT-05.DeliverLatestGuidanceIsIdempotent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceDeliverIsIdempotentTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceRoute;
	ExpectTheUnmeshedFixtureWarning(*this);

	FRouteHarness H;
	FString Error;
	if (!TestTrue(TEXT("a world with a findable HUD and no strip yet"),
			H.SpawnWorldAndActors(/*bStratController*/ false, Error)))
	{
		AddError(Error);
		return false;
	}

	TSharedRef<FRecordingDecorator> Recorder = RegisterRecorder(H);
	if (!TestTrue(TEXT("the match starts and pushes"), H.StartTheMatch(Error)) ||
		!TestTrue(TEXT("the decorator produced an active directive"),
			Recorder->LastProduced.bActive) ||
		!TestTrue(TEXT("a strip is installed afterwards"), H.InstallStrip(Error)))
	{
		AddError(Error);
		return false;
	}

	const FStratGuidanceView Expected = Recorder->LastProduced;

	FStratGuidanceView Held;
	TestTrue(TEXT("the first delivery reports a delivery"), H.Hud->DeliverLatestGuidance());
	TestTrue(TEXT("the second reports one too -- nothing was consumed"),
		H.Hud->DeliverLatestGuidance());
	if (!TestTrue(TEXT("the strip is readable"), H.ReadStrip(Held)))
	{
		return false;
	}
	TestTrue(*FString::Printf(
			TEXT("and twice delivered is the same value as once (expected: %s; strip: %s)"),
			*Describe(Expected), *Describe(Held)),
		SameGuidance(Held, Expected));

	// THE STRIP IS MOVED OUT FROM UNDER IT, through the widget's own push and not the HUD's,
	// so the HUD's cache is not updated. A latch would have nothing left to restore.
	if (!TestTrue(TEXT("something else writes the strip directly"),
			StratTestPushAtGuidanceStripDirectly(H.Hud, FStratGuidanceView())))
	{
		return false;
	}
	if (!TestTrue(TEXT("the strip is readable"), H.ReadStrip(Held)))
	{
		return false;
	}
	if (!TestFalse(TEXT("the strip really did move off the delivered value"),
			SameGuidance(Held, Expected)))
	{
		return false;
	}

	TestTrue(TEXT("a third delivery still reports a delivery"), H.Hud->DeliverLatestGuidance());
	if (!TestTrue(TEXT("the strip is readable"), H.ReadStrip(Held)))
	{
		return false;
	}
	TestTrue(*FString::Printf(
			TEXT("and it replays the cached value onto the strip again (expected: %s; strip: %s)"),
			*Describe(Expected), *Describe(Held)),
		SameGuidance(Held, Expected));

	return true;
}

// ---------------------------------------------------------------------------
// THE LATEST, AND NEVER A BACKLOG. The cache changes WHEN a value is delivered, never WHICH
// value -- so the strip stays a function of the last model applied and not of the sequence of
// models applied, which is the property `ApplyView`'s unconditional push holds one level up.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceDeliverReplaysOnlyTheLatestTest,
	"Stratocracy.StratPlay.T-INT-05.DeliverLatestGuidanceReplaysOnlyTheLatestPush",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceDeliverReplaysOnlyTheLatestTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceRoute;
	ExpectTheUnmeshedFixtureWarning(*this);

	FRouteHarness H;
	FString Error;
	if (!TestTrue(TEXT("a live match with a findable HUD and NO strip on it"),
			H.Arm(/*bWithStrip*/ false, Error)))
	{
		AddError(Error);
		return false;
	}

	// Two genuinely different views, both authored by the module: the guided opening's own
	// directive, and the block an undecorated rebuild leaves behind.
	FStratViewModel Decorated;
	FStratViewModel Rebuilt;
	if (!TestTrue(TEXT("the decorator produces an active directive"),
			H.BuildDecorated(Decorated, Error)) ||
		!TestTrue(TEXT("and the subsystem rebuilds an undecorated one"),
			H.BuildUndecorated(Rebuilt, Error)))
	{
		AddError(Error);
		return false;
	}
	if (!TestFalse(TEXT("the two views this clause turns on are distinguishable"),
			SameGuidance(Decorated.Guidance, Rebuilt.Guidance)))
	{
		return false;
	}

	// Both dropped: there is no strip to receive either.
	H.Hud->PushGuidance(Decorated.Guidance);
	H.Hud->PushGuidance(Rebuilt.Guidance);

	if (!TestTrue(TEXT("a strip is installed after both"), H.InstallStrip(Error)))
	{
		AddError(Error);
		return false;
	}
	TestTrue(TEXT("delivery reports a delivery"), H.Hud->DeliverLatestGuidance());

	FStratGuidanceView Held;
	if (!TestTrue(TEXT("the strip is readable"), H.ReadStrip(Held)))
	{
		return false;
	}
	TestTrue(*FString::Printf(
			TEXT("the strip carries the LAST push in every field (last: %s; strip: %s)"),
			*Describe(Rebuilt.Guidance), *Describe(Held)),
		SameGuidance(Held, Rebuilt.Guidance));
	TestFalse(*FString::Printf(
			TEXT("and not the earlier one -- this is not a queue being drained (earlier: %s)"),
			*Describe(Decorated.Guidance)),
		SameGuidance(Held, Decorated.Guidance));

	return true;
}

// ---------------------------------------------------------------------------
// DEFECT 2, THE SEAM ITSELF. `BuildViewModelForPresentation` runs a registered decorator and
// `BuildViewModel` does not -- a deliberate carve-out rather than an oversight: `IsAiTurnDue`
// and `RunAiTurnsNow`'s loop read models nobody draws, and decorating there would run
// `FStratGuidedOpening::Observe` -- the only thing that advances a beat -- for a question with
// no screen behind it.
//
// BOTH DIRECTIONS IN ONE CLAUSE ON PURPOSE. Split apart, each half is satisfiable by a
// decorator that never runs at all, and the pair is what makes either meaningful.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceUndecoratedBuildStaysUndecoratedTest,
	"Stratocracy.StratPlay.T-INT-05.UndecoratedBuildStaysUndecorated",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceUndecoratedBuildStaysUndecoratedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceRoute;
	ExpectTheUnmeshedFixtureWarning(*this);

	FRouteHarness H;
	FString Error;
	if (!TestTrue(TEXT("a live match with a findable HUD and a strip on it"),
			H.Arm(/*bWithStrip*/ true, Error)))
	{
		AddError(Error);
		return false;
	}

	TSharedRef<FRecordingDecorator> Recorder = RegisterRecorder(H);
	TestTrue(TEXT("the subsystem reports a decorator registered"),
		H.Subsystem->HasViewDecorator());

	const int32 CallsAtRegistration = Recorder->Calls;

	FStratViewModel Plain;
	if (!TestTrue(TEXT("BuildViewModel builds"), H.Subsystem->BuildViewModel(Plain, Error)))
	{
		AddError(Error);
		return false;
	}
	TestEqual(TEXT("BuildViewModel does NOT run the registered decorator"),
		Recorder->Calls, CallsAtRegistration);
	TestFalse(TEXT("so the model it produced carries no active directive"),
		Plain.Guidance.bActive);

	FStratViewModel Presented;
	if (!TestTrue(TEXT("BuildViewModelForPresentation builds"),
			H.Subsystem->BuildViewModelForPresentation(Presented, Error)))
	{
		AddError(Error);
		return false;
	}
	TestEqual(TEXT("BuildViewModelForPresentation runs it exactly once"),
		Recorder->Calls, CallsAtRegistration + 1);
	TestTrue(TEXT("and the model it produced carries the decorator's active directive"),
		Presented.Guidance.bActive);
	TestTrue(*FString::Printf(
			TEXT("in every field the decorator wrote (decorator: %s; model: %s)"),
			*Describe(Recorder->LastProduced), *Describe(Presented.Guidance)),
		SameGuidance(Presented.Guidance, Recorder->LastProduced));

	return true;
}

// ---------------------------------------------------------------------------
// `RefreshPresentation` decorates, and does it BEFORE applying -- which is what the strip
// holding the DECORATED block, rather than the built one, is evidence of. A decoration run
// after `ApplyView` would leave the strip carrying the undecorated model and this clause red.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceRefreshPresentationDecoratesTest,
	"Stratocracy.StratPlay.T-INT-05.RefreshPresentationDecoratesBeforeApplying",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceRefreshPresentationDecoratesTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceRoute;
	ExpectTheUnmeshedFixtureWarning(*this);

	FRouteHarness H;
	FString Error;
	if (!TestTrue(TEXT("a live match with a findable HUD and a strip on it"),
			H.Arm(/*bWithStrip*/ true, Error)))
	{
		AddError(Error);
		return false;
	}

	// CLEARED FIRST, so "the strip is active" afterwards cannot be satisfied by a value the
	// start reconcile happened to leave there.
	H.Hud->PushGuidance(FStratGuidanceView());

	TSharedRef<FRecordingDecorator> Recorder = RegisterRecorder(H);
	const int32 CallsAtRegistration = Recorder->Calls;

	FString RefreshReason;
	const bool bRefreshed = RefreshAndReport(H, RefreshReason);

	TestEqual(*FString::Printf(
			TEXT("RefreshPresentation ran the registered decorator once (it returned %s: '%s')"),
			bRefreshed ? TEXT("true") : TEXT("false"), *RefreshReason),
		Recorder->Calls, CallsAtRegistration + 1);

	if (!TestTrue(TEXT("and the decorator had an active directive to contribute"),
			Recorder->LastProduced.bActive))
	{
		return false;
	}

	FStratGuidanceView Held;
	if (!TestTrue(TEXT("the strip is readable"), H.ReadStrip(Held)))
	{
		return false;
	}
	TestTrue(*FString::Printf(
			TEXT("the strip carries the DECORATED block, so the decoration preceded the apply ")
			TEXT("(decorator: %s; strip: %s)"),
			*Describe(Recorder->LastProduced), *Describe(Held)),
		SameGuidance(Held, Recorder->LastProduced));

	return true;
}

// ---------------------------------------------------------------------------
// THE MEASURED REPRODUCTION, and the clause whose absence let the defect ship. Driven and
// watched in PIE on 2026-08-21: a decorated refresh put a live directive on Sec 2.11.6's
// strip, and the very next `SetViewingSide` left the widget reading `bActive=False,
// Beat=None, DirectiveText=""` one second later.
//
// THE HAND-OVER IS TO THE SAME SIDE FIRST, and that is not a weakening. `SetViewingSide` calls
// `RefreshPresentation` unconditionally, so a same-side call exercises the exact path the
// defect lived on while holding the MODEL still -- which lets the assertion be "the directive
// is STILL ACTIVE and still equals what the decorator produced" rather than the weaker
// "whatever it produced arrived". The other seat is then driven too, without the activity
// assertion, because whether the guided seat's beats survive the viewing side moving is a
// Sec 2.11.6 question this clause has no business answering.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceSetViewingSideKeepsDirectiveTest,
	"Stratocracy.StratPlay.T-INT-05.SetViewingSideDoesNotClearAnActiveDirective",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceSetViewingSideKeepsDirectiveTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceRoute;
	ExpectTheUnmeshedFixtureWarning(*this);

	FRouteHarness H;
	FString Error;
	if (!TestTrue(TEXT("a live match with a findable HUD and a strip on it"),
			H.Arm(/*bWithStrip*/ true, Error)))
	{
		AddError(Error);
		return false;
	}

	TSharedRef<FRecordingDecorator> Recorder = RegisterRecorder(H);

	// The live directive is put on the strip the way the player would see it arrive.
	FString RefreshReason;
	const bool bRefreshed = RefreshAndReport(H, RefreshReason);

	FStratGuidanceView Held;
	if (!TestTrue(*FString::Printf(
				TEXT("the decorator put an active directive on the model first (refresh ")
				TEXT("returned %s: '%s')"),
				bRefreshed ? TEXT("true") : TEXT("false"), *RefreshReason),
			Recorder->LastProduced.bActive) ||
		!TestTrue(TEXT("the strip is readable"), H.ReadStrip(Held)) ||
		!TestTrue(*FString::Printf(
				TEXT("and it reached the strip before it is asked to survive (strip: %s)"),
				*Describe(Held)),
			SameGuidance(Held, Recorder->LastProduced)))
	{
		return false;
	}

	// ---- The hand-over that used to wipe it -------------------------------
	const int32 CallsBefore = Recorder->Calls;
	const int32 CurrentSide = H.Subsystem->GetViewingSide();

	FString    SideReason;
	const bool bSameSide = H.Subsystem->SetViewingSide(CurrentSide, SideReason);

	TestTrue(*FString::Printf(
			TEXT("the hand-over's own reconcile went through the decorator (SetViewingSide ")
			TEXT("returned %s: '%s')"),
			bSameSide ? TEXT("true") : TEXT("false"), *SideReason),
		Recorder->Calls > CallsBefore);

	TestTrue(TEXT("the directive the decorator produces is still an active one"),
		Recorder->LastProduced.bActive);

	if (!TestTrue(TEXT("the strip is readable after the hand-over"), H.ReadStrip(Held)))
	{
		return false;
	}

	TestTrue(TEXT("and the strip is STILL ACTIVE -- the reconcile did not clear it"),
		Held.bActive);
	TestTrue(*FString::Printf(
			TEXT("carrying the decorator's block in every field (decorator: %s; strip: %s)"),
			*Describe(Recorder->LastProduced), *Describe(Held)),
		SameGuidance(Held, Recorder->LastProduced));

	// ---- And the real hot-seat move, to the other seat --------------------
	const int32 CallsBeforeSwap = Recorder->Calls;
	FString     SwapReason;
	const bool  bSwapped = H.Subsystem->SetViewingSide(1 - CurrentSide, SwapReason);

	TestTrue(*FString::Printf(
			TEXT("a hand-over to the other seat also reconciles through the decorator ")
			TEXT("(SetViewingSide returned %s: '%s')"),
			bSwapped ? TEXT("true") : TEXT("false"), *SwapReason),
		Recorder->Calls > CallsBeforeSwap);

	if (!TestTrue(TEXT("the strip is readable after the swap"), H.ReadStrip(Held)))
	{
		return false;
	}
	TestTrue(*FString::Printf(
			TEXT("and the strip agrees with what the decorator produced for THAT model, ")
			TEXT("whatever it says (decorator: %s; strip: %s)"),
			*Describe(Recorder->LastProduced), *Describe(Held)),
		SameGuidance(Held, Recorder->LastProduced));

	return true;
}

// ---------------------------------------------------------------------------
// `StartMatchInternal`'s own first reconciliation, the other subsystem-side build that used to
// apply an undecorated model. The strip is installed BEFORE the match starts here, so the push
// lands directly and nothing about `DeliverLatestGuidance` is involved -- this clause is about
// the MODEL, and `StripCreatedAfterAPushStillCarriesIt` is about the DELIVERY.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceStartMatchAppliesDecoratedTest,
	"Stratocracy.StratPlay.T-INT-05.StartMatchAppliesADecoratedModel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceStartMatchAppliesDecoratedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceRoute;
	ExpectTheUnmeshedFixtureWarning(*this);

	FRouteHarness H;
	FString Error;
	if (!TestTrue(TEXT("a world with a findable HUD"),
			H.SpawnWorldAndActors(/*bStratController*/ false, Error)) ||
		!TestTrue(TEXT("and a strip on it BEFORE any match exists"), H.InstallStrip(Error)))
	{
		AddError(Error);
		return false;
	}

	TSharedRef<FRecordingDecorator> Recorder = RegisterRecorder(H);

	// The control: a strip that has been handed nothing is not already showing a directive.
	FStratGuidanceView Held;
	if (!TestTrue(TEXT("the strip is readable before the match"), H.ReadStrip(Held)) ||
		!TestFalse(TEXT("and holds no active directive yet"), Held.bActive))
	{
		return false;
	}

	if (!TestTrue(TEXT("the match starts"), H.StartTheMatch(Error)))
	{
		AddError(Error);
		return false;
	}

	TestTrue(TEXT("StartMatch's own reconcile went through the registered decorator"),
		Recorder->Calls > 0);
	if (!TestTrue(TEXT("and that decorator had an active directive to contribute"),
			Recorder->LastProduced.bActive))
	{
		return false;
	}

	if (!TestTrue(TEXT("the strip is readable after the match starts"), H.ReadStrip(Held)))
	{
		return false;
	}
	TestTrue(TEXT("the strip is active straight out of StartMatch"), Held.bActive);
	TestTrue(*FString::Printf(
			TEXT("carrying the decorator's block in every field (decorator: %s; strip: %s)"),
			*Describe(Recorder->LastProduced), *Describe(Held)),
		SameGuidance(Held, Recorder->LastProduced));

	return true;
}

// ---------------------------------------------------------------------------
// THE DELIBERATE ASYMMETRY, flagged by the engineer as the thing a future "simplification"
// would remove. `AStratPlayerController::RefreshFromMachine` calls `DecorateForPresentation`
// DIRECTLY rather than going through `BuildViewModelForPresentation`, so it decorates because
// it is that controller and not because a registration happened to be in place. Routed the
// tidier-looking way, a controller driven before or without `BeginPlay` -- which is every
// fixture that spawns one into a world it built itself, including this one -- would silently
// stop decorating.
//
// THE PREMISE IS MEASURED AND NOT ASSUMED: `HasViewDecorator()` is asserted false first, so a
// day when spawning a controller DOES register one turns this clause red rather than letting
// it quietly start proving the opposite thing.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceRefreshFromMachineNeedsNoRegistrationTest,
	"Stratocracy.StratPlay.T-INT-05.RefreshFromMachineDecoratesWithNoRegistration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceRefreshFromMachineNeedsNoRegistrationTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceRoute;
	ExpectTheUnmeshedFixtureWarning(*this);

	FRouteHarness H;
	FString Error;
	if (!TestTrue(TEXT("a world holding the game's own controller"),
			H.SpawnWorldAndActors(/*bStratController*/ true, Error)) ||
		!TestTrue(TEXT("a live match"), H.StartTheMatch(Error)) ||
		!TestTrue(TEXT("and a strip to observe"), H.InstallStrip(Error)))
	{
		AddError(Error);
		return false;
	}

	if (!TestNotNull(TEXT("the controller spawned"), H.StratController))
	{
		return false;
	}

	// THE PREMISE. `BeginPlay` is not dispatched in this world, so its registration line never
	// ran and there is nothing bound.
	if (!TestFalse(TEXT("nothing is registered as the subsystem's view decorator"),
			H.Subsystem->HasViewDecorator()))
	{
		return false;
	}

	// THE SECOND PREMISE, AND IT IS ASKED RATHER THAN ASSUMED BECAUSE IT USED TO BE INHERITED
	// FROM DISK. §2.11.6 suppresses the entire opening when the resolved save slot records a
	// completed match, and `TryArmGuidedOpening` applies that inside the very refresh below --
	// so an unset strip is evidence about DECORATION only once suppression is excluded.
	// `MakeConfig` points the config at `kAbsentSlotName`; this reads the subsystem's own
	// answer instead of trusting that, so a slot of that name appearing on some future machine
	// reddens this line by name rather than reappearing as the unexplained unset strip
	// measured on 2026-09-01.
	if (!TestFalse(TEXT("the fixture's own save slot records no completed match, so §2.11.6 ")
			TEXT("does not suppress the opening this clause is about"),
			H.Subsystem->HasCompletedAMatchOnSave(FString())))
	{
		return false;
	}

	// Cleared first, so an active strip afterwards cannot be the start reconcile's leftovers.
	H.Hud->PushGuidance(FStratGuidanceView());

	// THE RETURN IS REPORTED AND NOT ASSERTED, for the reason `RefreshAndReport` records at
	// length: this fixture's HUD holds no scoreboard widget, so the LAST step of a refresh
	// refuses -- measured, "scoreboard refresh refused: there is no scoreboard widget to
	// refresh" -- long after `ApplyView` has already run and already pushed. Asserting the
	// return would be asserting the presence of a scoreboard asset, which belongs to another
	// lane. The reason travels into the message below so a refusal from an EARLIER step is
	// not mistaken for this known one.
	FString    RefreshReason;
	const bool bRefreshed = H.StratController->RefreshFromMachine(RefreshReason);

	FStratGuidanceView Held;
	if (!TestTrue(TEXT("the strip is readable"), H.ReadStrip(Held)))
	{
		return false;
	}

	TestTrue(*FString::Printf(
			TEXT("the controller's own refresh decorated with nobody registered (it returned ")
			TEXT("%s: '%s')"),
			bRefreshed ? TEXT("true") : TEXT("false"), *RefreshReason),
		Held.bActive);

	// The expectation is a SECOND run of the module-side author over the same board, built the
	// way `FRouteHarness::BuildDecorated` builds it. Nothing here types a field.
	FStratViewModel Expected;
	if (!TestTrue(TEXT("the module authors a directive for this board independently"),
			H.BuildDecorated(Expected, Error)))
	{
		AddError(Error);
		return false;
	}
	TestTrue(*FString::Printf(
			TEXT("and the block the controller wrote matches the module's own, field for field ")
			TEXT("(module: %s; strip: %s)"),
			*Describe(Expected.Guidance), *Describe(Held)),
		SameGuidance(Held, Expected.Guidance));

	return true;
}
