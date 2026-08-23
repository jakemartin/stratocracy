// The gate on §2.11.5's production-menu WIDGET HOST -- GATE-BUILDMENU. GDD §2.11.5, §2.11.4.
//
// WHAT THIS FILE'S SUBJECT IS. `AStratScoreboardHUD` grew four members and four methods to host
// §2.11.5's menu asset, and the interesting ones are the two that could each have been written
// a simpler, wrong way: `IsProductionMenuWidgetOpen()`, which reads the VIEWPORT rather than the
// pointer, and `CloseProductionMenuWidget()`, which drops the pointer UNCONDITIONALLY rather
// than only when the widget is on screen. Both of those choices are recorded as deliberate in
// the class's own comments; this file is what makes them fail if they are undone.
//
// WHAT IS GATED ELSEWHERE, and is deliberately not restated here:
//  - The controller's affordance -- the latch, the two refusals, and the two acts of a close --
//    is `Source/StratPlay/Tests/StratProductionMenuAffordance.cpp`. It has to live there:
//    `AStratPlayerController` and `UStratMatchSubsystem` are StratPlay types and this module
//    sits BELOW StratPlay, so naming either here would be `LNK2019` rather than a test.
//  - The subsystem's rows -- refresh, submit, close, reseed -- are
//    `Source/StratPlay/Tests/StratProductionMenuSeam.cpp`'s ten `GATE-BUILDMENU` clauses.
//  - The view-model leg is `StratProductionMenuRouting.cpp` in this directory.
//
// THE ONE CONDITION THAT SHAPES EVERY CLAUSE BELOW, MEASURED IN THE ENGINE RATHER THAN ASSUMED.
// `UUserWidget::AddToViewport` routes to `UGameViewportSubsystem::AddToScreen`, which returns
// early -- with a `KismetExecutionMessage` and no widget added -- when the widget's world is
// not a game world OR when that world has no `UGameViewportClient`. A headless `-nullrhi`
// automation run satisfies neither, so NO WIDGET IN THIS FILE IS EVER IN A VIEWPORT. Two
// consequences, and the file is honest about both rather than trading on the first:
//   - It is what makes `MenuOpenReadsTheViewportAndNotThePointer` a real gate. The pointer is
//     non-null and the viewport is empty, which is the ONE state where a pointer-only reader
//     and the shipped reader disagree. Both halves are asserted, so a green cannot mean
//     "there was no widget".
//   - It is also why three of the engineer's proposed clauses are NOT here. See the block
//     immediately below.
//
// THREE PROPOSED CLAUSES WERE REFUSED AS UNFALSIFIABLE, and they are named rather than dropped
// silently:
//   - "a refused open leaves no latch" (the unwind branch in
//     `AStratPlayerController::ToggleProductionMenu`). The latch is only ever SET after
//     `HexUnderCursor` succeeds, and `HexUnderCursor` needs `GetHitResultUnderCursor`, which
//     needs a `ULocalPlayer` with a viewport client. Headless there is none, so the toggle
//     returns at "the cursor is not on the board" BEFORE the latch is set and before
//     `OpenProductionMenuWidget` is called. A clause asserting the unwind would be asserting
//     `false == false` over a branch that never executed.
//   - "a second open is refused because one is already open". That refusal sits behind
//     `IsProductionMenuWidgetOpen()`, which is false in every reachable headless state, so the
//     branch is unreachable and the stale-pointer branch below it is taken instead.
//   - "an unset menu class leaves `LastFailureReason` describing the scoreboard, never the
//     menu". `AStratScoreboardHUD::BeginPlay`'s production-menu block is followed
//     UNCONDITIONALLY by the `ScoreboardWidgetClass` block, which assigns `LastFailureReason`
//     on every path that reaches it. An implementation that wrongly wrote the menu's reason
//     into that member would have it overwritten before any clause could read it, so the
//     assertion is green on the defect and on the fix alike. What IS falsifiable about that
//     claim is the LOG line and its verbosity, and that is
//     `AnUnsetMenuClassIsReportedAtBeginPlayWithoutRaisingAFailure` below.
//
// WHERE EVERY EXPECTATION COMES FROM. Nothing below writes down a widget state, a pointer value
// or a reason sentence and then checks the code against it. The viewport answer is read off
// `UUserWidget::IsInViewport()` through the probe; the pointer answer is read off the HUD's own
// public `ProductionMenu`; the two refusal sentences are read out of the HUD and compared for
// SUBSTANCE and for difference from one another, never for exact text. The one literal in the
// file is the property NAME `ProductionMenuWidgetClass`, which is a C++ identifier a refusal is
// required to name so that a Blueprint author can act on it.
//
// THE CASE-INSENSITIVITY TRAP. UE's `FString::operator==`, `FString::Contains` and
// `FAutomationTestBase::TestEqual` are ALL case-insensitive, measured on this project. No clause
// below turns on a casing distinction, and the two reason clauses assert on substance
// (`Contains` of an identifier that differs by more than case) plus a strict inequality between
// two different sentences, so neither could pass on the wrong one.

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "HAL/CriticalSection.h"
#include "Logging/LogVerbosity.h"
#include "Misc/OutputDevice.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"

#include "StratScoreboardHUD.h"

#include "Tests/StratProductionMenuHostProbe.h"

// The bridge, included from a .cpp and never from a UHT-parsed header -- the same constraint
// StratScoreboardHUD.h records about itself.
#include "StratBridge.h"

namespace StratProductionMenuHost
{
	/** The harness pin for which side moves first, on `StratScoreboardHUDSeam.cpp`'s stated
	 *  precedent: Replay.h says no rule in this project decides it. */
	static const int32 kFirstSide = 0;

	static UDataTable* LoadTable(const TCHAR* ObjectPath)
	{
		return LoadObject<UDataTable>(nullptr, ObjectPath);
	}

	/** Brings a bridge to the seeded opening. Same helper, same shape and same reason an
	 *  automation fixture may name a `/Game/` path at all, as `StratScoreboardHUDSeam.cpp`. */
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

		const FString ScenarioPath =
			FPaths::Combine(FPaths::ProjectDir(), TEXT("Data"), TEXT("ferrum_crossing.json"));
		const FStratResult Seeded = Bridge.LoadScenarioFromFile(ScenarioPath, kFirstSide);
		if (!Seeded.bOk)
		{
			OutError = FString::Printf(TEXT("LoadScenarioFromFile refused: %s"), *Seeded.Reason);
			return false;
		}
		return true;
	}

	/** The transient world, with the engine context teardown wants. Same shape and same
	 *  destruction order as `StratProductionMenuSeam.cpp`'s, for the reason recorded there. */
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

		FTestWorldScope(const FTestWorldScope&)            = delete;
		FTestWorldScope& operator=(const FTestWorldScope&) = delete;
	};

	/**
	 * One captured line, with the verbosity it was emitted at.
	 *
	 * THE VERBOSITY IS THE SUBJECT of the BeginPlay clause and is therefore kept, not dropped.
	 * "Reported as a configuration and not as a failure" is a claim about WHICH channel the
	 * sentence went out on, and a capture that stored only the text could not fail on it.
	 */
	struct FCapturedLine
	{
		FString              Text;
		ELogVerbosity::Type  Verbosity = ELogVerbosity::NoLogging;
	};

	/**
	 * A `GLog` capture over the three "no widget class is set" sentences `BeginPlay` can emit.
	 *
	 * `CanBeUsedOnMultipleThreads()` RETURNS TRUE AND THE OVERRIDE IS LOAD-BEARING. Measured on
	 * this project 2026-08-14: a capture without it failed 1 run in 4 on byte-identical code.
	 * `FOutputDeviceRedirector` queues lines it cannot broadcast on the primary-thread fast path
	 * and drains that queue to whichever devices sit in `BufferedOutputDevices` AT DRAIN TIME, so
	 * a buffered capture inherits the tail of somebody else's queue. The override routes this
	 * device to `UnbufferedOutputDevices`, fed only by the synchronous broadcast inside the
	 * emitting `UE_LOG`, which makes the window it sees equal its own lifetime by construction
	 * rather than by timing. All four existing captures in `Source/StratPlay/Tests/` carry it.
	 *
	 * THE FILTER IS LOOSE AND ONLY THE ASSERTION IS STRICT, on this project's standing rule. It
	 * matches `WidgetClass` case-INSENSITIVELY, so a casing defect in the emitter still reaches
	 * the clause and is judged there rather than vanishing at the instrument.
	 */
	struct FWidgetClassCapture final : public FOutputDevice
	{
		TArray<FCapturedLine> Lines;

		FWidgetClassCapture()
		{
			if (GLog != nullptr)
			{
				GLog->AddOutputDevice(this);
			}
		}

		virtual ~FWidgetClassCapture()
		{
			if (GLog != nullptr)
			{
				GLog->RemoveOutputDevice(this);
			}
		}

		/** See the block above. Removing this line reopens the late-delivery hole. */
		virtual bool CanBeUsedOnMultipleThreads() const override { return true; }

		virtual void Serialize(const TCHAR* Message, ELogVerbosity::Type Verbosity,
		                       const FName& /*Category*/) override
		{
			const FString Line(Message);
			if (Line.Contains(TEXT("WidgetClass"), ESearchCase::IgnoreCase))
			{
				FScopeLock Lock(&Mutex);
				Lines.Add(FCapturedLine{ Line, Verbosity });
			}
		}

		/** The first captured line naming `Needle`, or nullptr. Case-insensitive on purpose. */
		const FCapturedLine* Find(const TCHAR* Needle) const
		{
			for (const FCapturedLine& Line : Lines)
			{
				if (Line.Text.Contains(Needle, ESearchCase::IgnoreCase))
				{
					return &Line;
				}
			}
			return nullptr;
		}

		/** Everything seen, for a failure message that names what the instrument DID hear. */
		FString Transcript() const
		{
			TArray<FString> Slice;
			for (const FCapturedLine& Line : Lines)
			{
				Slice.Add(FString::Printf(TEXT("[%s] %s"),
					ToString(Line.Verbosity), *Line.Text));
			}
			return Slice.Num() > 0 ? FString::Join(Slice, TEXT(" | ")) : FString(TEXT("<nothing>"));
		}

	private:
		FCriticalSection Mutex;
	};
}

// ---------------------------------------------------------------------------------------------
// The clause the whole file exists for.
// ---------------------------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuOpenReadsViewport,
	"Stratocracy.StratUI.GATE-BUILDMENU.MenuOpenReadsTheViewportAndNotThePointer",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * A widget on the member, off the viewport: `IsProductionMenuWidgetOpen()` must say NO.
 *
 * THIS IS THE ONE CLAUSE A POINTER-ONLY IMPLEMENTATION FAILS. Every other clause in this file
 * and in the affordance file passes just as well against
 * `return ProductionMenu != nullptr;` -- the pointer and the viewport agree everywhere except
 * here. `AStratScoreboardHUD::IsProductionMenuWidgetOpen`'s own comment names the shipping
 * consequence: a `WBP_ProductionMenu` with a Cancel button that removes ITSELF leaves the
 * pointer non-null and the widget off screen, and a pointer-only reader would then answer "open"
 * and cost the player a second keypress to reopen a menu that is not there.
 *
 * BOTH HALVES ARE ASSERTED SO A GREEN CANNOT MEAN "THERE WAS NO WIDGET". The pointer is read
 * non-null and the engine's own `IsInViewport()` is read false, in the same frame, before the
 * HUD is asked anything. Without the first of those, this clause would pass on a HUD whose
 * install had silently failed -- which is a gate that has stopped covering its subject.
 */
bool FStratProductionMenuOpenReadsViewport::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuHost;

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	AStratScoreboardHUD* const Hud = Scope.World->SpawnActor<AStratScoreboardHUD>();
	if (!TestNotNull(TEXT("the scoreboard HUD spawned"), Hud))
	{
		return false;
	}

	// The state before anything is installed, read rather than assumed.
	TestFalse(TEXT("a HUD with no menu widget reports no menu open"),
		Hud->IsProductionMenuWidgetOpen());
	TestFalse(TEXT("a HUD with no menu widget holds no menu pointer either"),
		StratTestHasProductionMenuWidget(Hud));

	if (!TestTrue(TEXT("the concrete menu double installed on the HUD's own member"),
			StratTestInstallProductionMenuDouble(Hud)))
	{
		return false;
	}

	// --- THE POSITIVE CONTROL. Without these two reads the assertion below is worthless. ---
	if (!TestTrue(TEXT("the HUD is now holding a menu widget on ProductionMenu"),
			StratTestHasProductionMenuWidget(Hud)))
	{
		return false;
	}
	if (!TestFalse(TEXT("and the engine reports that widget is NOT in a viewport, which is the "
	                    "state this clause needs and the only state the two readers disagree in"),
			StratTestProductionMenuWidgetIsInViewport(Hud)))
	{
		// A headless run that somehow HAD a viewport would make this clause vacuous rather than
		// wrong, and it must say so instead of going green.
		AddError(TEXT("the menu double reached a viewport in a headless run; this clause can no "
		              "longer distinguish a viewport reader from a pointer reader and must be "
		              "re-derived before it is trusted"));
		return false;
	}

	// --- THE ASSERTION. ---
	TestFalse(TEXT("with a widget on the pointer and nothing in the viewport, "
	               "IsProductionMenuWidgetOpen() reads the viewport and answers false"),
		Hud->IsProductionMenuWidgetOpen());

	return true;
}

// ---------------------------------------------------------------------------------------------
// The close.
// ---------------------------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuCloseDropsThePanel,
	"Stratocracy.StratUI.GATE-BUILDMENU.CloseDropsThePanelUnconditionallyAndIsSafeWithNoneUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * `CloseProductionMenuWidget()` drops the pointer whether or not the widget was on screen, and
 * is a no-op when there is nothing to close.
 *
 * THE UNCONDITIONAL HALF IS THE SUBJECT AND IT IS EXACTLY WHAT THIS ENVIRONMENT CAN TEST.
 * `AStratScoreboardHUD::CloseProductionMenuWidget` deliberately carries NO `IsInViewport()`
 * guard -- its own comment says a guard "would make the pointer-clear conditional on a state
 * this function's whole job is to leave behind". Headless, the widget is never in a viewport, so
 * an implementation that acquired such a guard would leave the pointer set and this clause
 * reddens. That makes the absence of the guard falsifiable rather than merely documented.
 *
 * THE SECOND CALL IS NOT PADDING. `AStratScoreboardHUD::EndPlay` calls this function
 * unconditionally, including on a HUD that never opened a menu, so "safe with none up" is a
 * shipping path and not a hypothetical.
 */
bool FStratProductionMenuCloseDropsThePanel::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuHost;

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	AStratScoreboardHUD* const Hud = Scope.World->SpawnActor<AStratScoreboardHUD>();
	if (!TestNotNull(TEXT("the scoreboard HUD spawned"), Hud))
	{
		return false;
	}

	// --- Nothing up. The close must be a no-op rather than a fault. ---
	Hud->CloseProductionMenuWidget();
	TestFalse(TEXT("closing with no menu up leaves no menu pointer"),
		StratTestHasProductionMenuWidget(Hud));
	TestFalse(TEXT("closing with no menu up leaves no menu open"),
		Hud->IsProductionMenuWidgetOpen());

	// --- A widget up, off the viewport. The close must still drop it. ---
	if (!TestTrue(TEXT("the concrete menu double installed on the HUD's own member"),
			StratTestInstallProductionMenuDouble(Hud)))
	{
		return false;
	}
	if (!TestTrue(TEXT("the HUD is holding a menu widget before the close"),
			StratTestHasProductionMenuWidget(Hud)))
	{
		return false;
	}
	// The condition that makes the assertion below mean something: the widget the close is
	// about is NOT on screen, so only an unconditional clear reaches it.
	if (!TestFalse(TEXT("and that widget is not in a viewport, so only an unguarded clear "
	                    "will drop it"),
			StratTestProductionMenuWidgetIsInViewport(Hud)))
	{
		AddError(TEXT("the menu double reached a viewport in a headless run; this clause no "
		              "longer distinguishes an unconditional clear from a guarded one"));
		return false;
	}

	Hud->CloseProductionMenuWidget();

	TestFalse(TEXT("the close dropped the menu pointer even though the widget was never on screen"),
		StratTestHasProductionMenuWidget(Hud));
	TestFalse(TEXT("and the HUD reports no menu open afterwards"),
		Hud->IsProductionMenuWidgetOpen());

	return true;
}

// ---------------------------------------------------------------------------------------------
// The unset class, on both channels it speaks on.
// ---------------------------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuUnsetClassRefusesTheOpen,
	"Stratocracy.StratUI.GATE-BUILDMENU.AnUnsetMenuClassRefusesTheOpenInItsOwnWords",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * `OpenProductionMenuWidget` refuses an unset class, names the property, and leaves the HUD
 * exactly as it found it.
 *
 * WHY THE REFUSAL EXISTS AT ALL, given `BeginPlay` already said so once: the HUD's own comment
 * says a caller told nothing "would have no way to distinguish that from a menu that opened".
 * `AStratPlayerController::ToggleProductionMenu` forwards this sentence unchanged, so it is what
 * the player-facing log line ends up carrying.
 *
 * THE PROPERTY NAME IS THE ONE LITERAL IN THIS FILE, and it is a C++ identifier rather than a
 * sentence -- a refusal that did not name the property would leave a Blueprint author with
 * nothing to act on. `Contains` is case-insensitive in this engine and that is fine here,
 * because no other string the HUD can produce differs from this one by case alone; the clause
 * additionally pins that this reason is NOT the create-path reason.
 */
bool FStratProductionMenuUnsetClassRefusesTheOpen::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuHost;

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	AStratScoreboardHUD* const Hud = Scope.World->SpawnActor<AStratScoreboardHUD>();
	if (!TestNotNull(TEXT("the scoreboard HUD spawned"), Hud))
	{
		return false;
	}

	FString Reason = TEXT("<the HUD wrote nothing>");
	TestFalse(TEXT("a HUD with no ProductionMenuWidgetClass refuses to open a menu"),
		Hud->OpenProductionMenuWidget(Reason));

	TestTrue(TEXT("the refusal names the property a Blueprint author would have to set"),
		Reason.Contains(TEXT("ProductionMenuWidgetClass"), ESearchCase::IgnoreCase));
	TestTrue(TEXT("and says it is unset rather than describing a failed creation"),
		Reason.Contains(TEXT("is set"), ESearchCase::IgnoreCase) ||
		Reason.Contains(TEXT("unset"), ESearchCase::IgnoreCase));
	TestFalse(TEXT("the refusal is not the CreateWidget failure, which is a different fault "
	               "with a different answer"),
		Reason.Contains(TEXT("CreateWidget returned null"), ESearchCase::IgnoreCase));

	// A refusal must not have half-opened anything.
	TestFalse(TEXT("a refused open left no menu pointer behind"),
		StratTestHasProductionMenuWidget(Hud));
	TestFalse(TEXT("and the HUD still reports no menu open"),
		Hud->IsProductionMenuWidgetOpen());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratProductionMenuUnsetClassIsReportedQuietly,
	"Stratocracy.StratUI.GATE-BUILDMENU.AnUnsetMenuClassIsReportedAtBeginPlayWithoutRaisingAFailure",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * `BeginPlay` says once that no menu was requested, and says it on the Log channel.
 *
 * WHAT THIS PINS THAT `LastFailureReason` COULD NOT. The engineer's claim was that an unset
 * `ProductionMenuWidgetClass` is "a configuration, not a failure", and that "not configured" and
 * "failed to open" stay distinguishable. The `LastFailureReason` half of that claim is NOT
 * testable -- the `ScoreboardWidgetClass` block runs after the production-menu block and assigns
 * that member on every path, so a defect there is overwritten before a clause could see it. The
 * VERBOSITY half is testable and is the half that carries the meaning: this project treats
 * Warning and Error as failures, so the difference between `Log` and anything above it is the
 * difference between a configuration and a fault.
 *
 * THE ADOPTED BRIDGE IS WHAT MAKES THE BLOCK REACHABLE AT ALL. A bare HUD fails `SeedBridge` and
 * returns from `BeginPlay` long before the production-menu block, so a clause built on one would
 * pass without the subject ever executing. Adopting a seeded bridge takes the branch
 * `StratScoreboardHUDSeam.cpp` calls "the subsystem's path", which runs the whole function.
 *
 * THE INSTRUMENT IS GIVEN A CONTROL. An absence proves nothing until the instrument is shown able
 * to speak, so the clause requires the guidance strip's and the scoreboard's sibling lines in the
 * same capture. If the capture is deaf, it fails naming its own transcript instead of quietly
 * reporting that the menu line was fine.
 */
bool FStratProductionMenuUnsetClassIsReportedQuietly::RunTest(const FString& /*Parameters*/)
{
	using namespace StratProductionMenuHost;

	FStratBridge Bridge;
	FString      Error;
	if (!TestTrue(TEXT("the fixture bridge seeded"), SeedBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	FTestWorldScope Scope;
	if (!TestNotNull(TEXT("a transient world was created"), Scope.World))
	{
		return false;
	}

	AStratScoreboardHUD* const Hud = Scope.World->SpawnActor<AStratScoreboardHUD>();
	if (!TestNotNull(TEXT("the scoreboard HUD spawned"), Hud))
	{
		return false;
	}

	// Adopted BEFORE play begins -- the subsystem's path. `BeginPlay` then skips `SeedBridge`
	// and runs through to the scoreboard block, which is what puts the production-menu block
	// on the executed path.
	if (!TestTrue(TEXT("the seeded bridge is adopted before play begins"),
			Hud->AdoptBridge(Bridge, Error)))
	{
		AddError(Error);
		return false;
	}

	// None of the three widget classes is set on this HUD, which is the configuration under
	// test: three absences, three sentences, and the question is what channel each goes out on.
	{
		FWidgetClassCapture Capture;
		Hud->DispatchBeginPlay();

		// --- THE CONTROL. The instrument must be shown able to hear its siblings. ---
		const FCapturedLine* const Guidance  = Capture.Find(TEXT("GuidanceWidgetClass"));
		const FCapturedLine* const Scoreboard = Capture.Find(TEXT("ScoreboardWidgetClass"));
		if (Guidance == nullptr || Scoreboard == nullptr)
		{
			AddError(FString::Printf(
				TEXT("the log capture did not hear BeginPlay's sibling lines, so its silence "
				     "about the production menu proves nothing. Transcript: %s"),
				*Capture.Transcript()));
			return false;
		}

		// --- THE ASSERTION. ---
		const FCapturedLine* const Menu = Capture.Find(TEXT("ProductionMenuWidgetClass"));
		if (Menu == nullptr)
		{
			AddError(FString::Printf(
				TEXT("BeginPlay said nothing about the unset ProductionMenuWidgetClass, so a "
				     "misconfigured Blueprint leaves no witness in the session log. "
				     "Transcript: %s"),
				*Capture.Transcript()));
			return false;
		}

		TestEqual(TEXT("the unset menu class is reported at Log, which is what makes it a "
		               "configuration rather than a failure"),
			static_cast<int32>(Menu->Verbosity), static_cast<int32>(ELogVerbosity::Log));

		// It is reported BESIDE the other two, which is the whole reason it sits in BeginPlay
		// rather than only at the moment the player first presses the key.
		TestEqual(TEXT("the guidance strip's absence is reported on the same channel"),
			static_cast<int32>(Guidance->Verbosity), static_cast<int32>(ELogVerbosity::Log));
	}

	// The HUD never allocated a bridge of its own on the adopted path, so nothing here can have
	// disturbed the fixture's.
	TestTrue(TEXT("the adopted bridge is still seeded after BeginPlay"), Bridge.IsSeeded());

	return true;
}
