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
#include "StratScoreboardHUD.h"
#include "StratSelectionMachine.h"
#include "StratUnitActor.h"
#include "StratViewModel.h"

#include "StratBridge.h"

namespace StratGuidanceRoute
{
	static const int32 kFirstSide = 0;

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
		bool                  bHasStrip  = false;

		bool Arm(bool bWithStrip, FString& OutError)
		{
			if (Scope.World == nullptr)
			{
				OutError = TEXT("no transient world was created");
				return false;
			}

			Controller = Scope.World->SpawnActor<APlayerController>();
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

			if (bWithStrip)
			{
				if (!StratTestInstallGuidanceStripDouble(Hud))
				{
					OutError = TEXT("the test-only concrete guidance widget did not install");
					return false;
				}
				bHasStrip = true;
			}

			return true;
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
			// The one struct field is FIntPoint; move it.
			FIntPoint* const Point = AsStruct->ContainerPtrToValuePtr<FIntPoint>(&InOut);
			Point->X += 3;
			Point->Y += 5;
			return true;
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
