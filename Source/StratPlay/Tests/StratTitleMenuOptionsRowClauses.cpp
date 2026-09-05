// `UStratShellMenuWidget` -- the title menu's own C++ parent, under `GATE-TITLEMENU`.
//
// THE ID IS RULED AND NOT CHOSEN HERE. `Tools/architect/state/global.md`'s third acceptance-ID
// ruling of 2026-09-05: *"This is the title menu's own C++ parent, named in `GATE-TITLEMENU`'s
// own subject already ... nothing about a by-route label instead of a by-index one changes which
// domain owns the class."* No name is minted.
//
// ---------------------------------------------------------------------------------------
// THE PROPERTY, AND WHY IT IS THE OPPOSITE OF WHAT `StratShellRouteClauses.cpp` PINS.
//
// `BuildMenuModel` emits one row per declared `EStratShellRoute`, IN DECLARATION ORDER, and that
// ordering is pinned by-index in `StratShellRouteClauses.cpp` -- correctly, because the shipped
// title menu's other four buttons are wired from a Blueprint graph by row index and would all
// shift together if the enum grew a member in the middle.
//
// `UStratShellMenuWidget` DELIBERATELY DOES THE OPPOSITE, and its declaration says so:
// *"IT LOOKS THE ROW UP BY `Route` AND NOT BY INDEX, WHICH IS THE OPPOSITE OF WHAT THE FOUR"*
// graph-wired buttons do. THE CONSEQUENCE IS THE WHOLE CLAUSE: a sixth enumerator inserted
// AHEAD of `Options` moves the options row's index, and a by-index implementation would then
// label the Options button with whatever the new route is called -- a button reading "Load
// Replay" that opens the volume screen. That defect is invisible to every existing clause,
// because the model itself would be perfectly correct.
//
// HOW IT IS MADE FALSIFIABLE WITHOUT ADDING AN ENUMERATOR. Inserting a sixth route is a change
// to `Source/StratPlay/StratShellSubsystem.h`, which is not this lane's to make, so the clause
// instead pins the SHAPE that makes a by-index read wrong:
//   1. The label drawn equals the label of the row the model itself reports for
//      `EStratShellRoute::Options` -- found by predicate, exactly as the shipped code finds it,
//      with no index written here.
//   2. Every row's label in the model is PAIRWISE DISTINCT. That is what makes (1)
//      discriminating: with five distinct labels, an implementation that read ANY fixed index
//      other than the options row's own would draw a different string and be red.
//   3. The options row is not at index 0 and not the only row, so the two most likely
//      hardcodings are covered by (2) rather than by luck.
// A reader is owed the limit as well: if a future `BuildMenuModel` ever emitted two rows with
// the same label, (2) would report it as a red control rather than silently weakening (1) --
// which is the correct behaviour and is why the control is asserted rather than assumed.
//
// WHERE THE EXPECTATIONS COME FROM. Every string compared is read from
// `UStratShellSubsystem::GetMenuModel()`, the subsystem's own published model, at the moment of
// the comparison. This file writes no label, names no index and does not know the word
// "Options" appears on the button.
//
// WHAT THIS DOES **NOT** PIN.
//   - **THAT THE CAPTION IS READ FROM THE MODEL AT ALL, AS OPPOSED TO WRITTEN BESIDE IT.** This
//     is the limit a reader is most likely to assume away, and it was MEASURED on 2026-09-05: a
//     mutant replacing the assignment with `SetText(FText::FromString(TEXT("Options")))` is
//     GREEN. `BuildMenuModel` labels this row from no fact -- a literal `TEXT("Options")` -- so
//     the expected and the hardcoded strings are identical in every reachable state and no
//     equality clause can tell them apart. What (1)-(3) above DO pin is by-route-not-by-index:
//     the mutant that reads a fixed index is red, because the OTHER rows' labels differ. The two
//     are separate properties and only the second is held here.
//     THE ONLY THING THAT WOULD REACH THE FIRST IS A SEAM, AND IT IS NOT THIS LANE'S TO ADD.
//     `RefreshOptionsRow` calls `Shell->GetMenuModel()` itself, so a fixture has no way to hand
//     it a model whose Options label differs from the shipped literal. Splitting the read from
//     the write -- `RefreshOptionsRow` keeping the lookup and delegating to an
//     `ApplyOptionsRow(const FStratShellOption&)` a fixture could call with a planted row --
//     would make it reachable, and that is a change to `Source/StratPlay/StratShellMenuWidget.h`
//     and `.cpp`. It is named here as the change that would be needed, not proposed as one that
//     should be made; the existing clause stays green either way.
//   - THE ROW'S ENABLED STATE AGAINST ANY PARTICULAR FACT. `RefreshOptionsRow` copies
//     `Row->bEnabled` across without a branch, deliberately -- *"a branch would be this class
//     holding an opinion about the answer"* -- and WHAT that answer should be is
//     `IsRoutePermitted`'s and `StratShellRouteClauses.cpp`'s. This line used to end "What is
//     pinned here is the COPY", and that overclaims: `IsRoutePermitted` grants `Options` with no
//     branch on any fact, so the copy assertion compares `true` against `true` and a constant
//     `true` passes it. The COPY is pinned only once that arm can answer `false`.
//   - THAT `WBP_TitleMenu` IS REPARENTED TO THIS CLASS OR CARRIES THESE TWO CHILDREN. Both are
//     `BindWidgetOptional`, so nothing enforces them at Blueprint compile time; that is an
//     asset fact and a different instrument.
//   - THAT CLICKING REACHES `ExecuteRoute`. `HandleOptionsClicked` is exercised here only for
//     the route-request count; the arm's behaviour is
//     `GATE-TITLEMENU.ExecuteRouteOptionsRequestsThePanelOncePerCall`'s.
// ---------------------------------------------------------------------------------------

#include "Misc/AutomationTest.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Containers/UnrealString.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "StratOptionsPanelDoubles.h"
#include "StratShellMenuWidget.h"
#include "StratShellSubsystem.h"

namespace StratTitleMenuOptionsRow
{
	/** A route's reflected name, for failure messages. Asked of the `UENUM` rather than
	 *  switched over here, on `StratShellOptionsRouteClauses.cpp`'s precedent. Display only. */
	FString RouteWord(const EStratShellRoute Route)
	{
		if (const UEnum* const Reflected = StaticEnum<EStratShellRoute>())
		{
			return Reflected->GetNameStringByValue(static_cast<int64>(Route));
		}
		return FString::Printf(TEXT("<unreflected route %d>"), static_cast<int32>(Route));
	}

	/** A world with a game instance whose subsystem collection is live, and nothing else. The
	 *  widget is outered to the world so `UUserWidget::GetWorld` resolves and the shipped
	 *  `RefreshOptionsRow` can reach the shell the way it does in a real title map. */
	struct FMenuScope
	{
		UWorld*                         World = nullptr;
		TStrongObjectPtr<UGameInstance> Instance;

		FString Failure;

		FMenuScope()
		{
			World = UWorld::CreateWorld(EWorldType::Game, /*bInformEngineOfWorld*/ false);
			if (World == nullptr)
			{
				Failure = TEXT("UWorld::CreateWorld returned null");
				return;
			}

			if (GEngine != nullptr)
			{
				FWorldContext& Context = GEngine->CreateNewWorldContext(EWorldType::Game);
				Context.SetCurrentWorld(World);
			}

			// `InitializeStandalone` is what stands the subsystem collection up; a bare
			// `NewObject<UGameInstance>` has none. Measured in `StratShellHudCallSiteClauses.cpp`.
			Instance = TStrongObjectPtr<UGameInstance>(NewObject<UGameInstance>(GEngine));
			if (!Instance.IsValid())
			{
				Failure = TEXT("could not allocate a UGameInstance");
				return;
			}
			Instance->InitializeStandalone(TEXT("StratTitleMenuOptionsRow"), nullptr);
			World->SetGameInstance(Instance.Get());
		}

		~FMenuScope()
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

		UStratShellSubsystem* Shell() const
		{
			return Instance.IsValid()
				? Instance->GetSubsystem<UStratShellSubsystem>() : nullptr;
		}

		FMenuScope(const FMenuScope&)            = delete;
		FMenuScope& operator=(const FMenuScope&) = delete;
	};
}

// ---------------------------------------------------------------------------------------
// The title menu labels its options row from the model, by route.
//
// MUTANTS, AND WHAT EACH WAS ACTUALLY MEASURED TO DO. This block listed three and predicted
// "red" for all three. The mutant battery of 2026-09-05 -- fifteen mutants run in place with a
// clean rebuild between each -- killed the first and left the second GREEN, and the third is
// green by construction. Two of the three predictions here were wrong, and they are corrected
// rather than deleted, because the reason each survives is the useful part.
//
//   - (KILLED, AND THIS IS THE CLAUSE'S SUBJECT) `RefreshOptionsRow` reading `Model.Options[0]`
//     -- or any fixed index -- instead of the `FindByPredicate`. Red on the label comparison,
//     with the message printing both strings. This is the property the file is named for and it
//     is genuinely pinned.
//
//   - (SURVIVED -- MEASURED GREEN, NOT PREDICTED) `OptionsLabel->SetText(FText::FromString(
//     TEXT("Options")))`, a hardcoded caption. The old prediction was "Red, because the model's
//     own label for that row is compared, not a literal this file wrote." The premise is true
//     and the conclusion does not follow. `BuildMenuModel`'s `EStratShellRoute::Options` arm
//     sets the label to the literal `TEXT("Options")` and its own comment states there is "no
//     state in which 'Options' means something else" -- the row is labelled from NO fact. So the
//     model's string and the hardcoded string are the same string in every state any fixture can
//     reach, and an equality comparison cannot separate a caption READ FROM THE MODEL from one
//     WRITTEN BESIDE IT. This is a property of the comparison, not of the wording: no message,
//     no premise and no extra control fixes it while both sides are `"Options"`.
//
//   - (SURVIVES BY CONSTRUCTION -- DERIVED FROM THE SOURCE, NOT RUN IN THE BATTERY)
//     `OptionsButton->SetIsEnabled(true)` unconditionally. `IsRoutePermitted`'s
//     `EStratShellRoute::Options` arm returns `true` with no branch on any fact, so
//     `OptionsRow.bEnabled` is `true` in every fact combination a fixture can build, and the
//     enabled-copy assertion below compares `true` against `true`. It is kept because it becomes
//     discriminating the day that arm grows a refusal -- which its own comment says is where a
//     refusal would go -- but TODAY it does not distinguish a copy from a constant. Recorded
//     here so no reader takes it for a live measurement.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratTitleMenuLabelsItsOptionsRowByRouteTest,
	"Stratocracy.StratPlay.GATE-TITLEMENU.TheTitleMenuLabelsItsOptionsRowByRouteAndNotByIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratTitleMenuLabelsItsOptionsRowByRouteTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratTitleMenuOptionsRow;

	FMenuScope Scope;
	if (!Scope.Failure.IsEmpty())
	{
		AddError(Scope.Failure);
		return false;
	}

	UStratShellSubsystem* const Shell = Scope.Shell();
	if (!TestNotNull(TEXT("FIXTURE: this game instance has a UStratShellSubsystem"), Shell))
	{
		return false;
	}

	const TStrongObjectPtr<UStratShellMenuWidgetDouble> Menu(
		NewObject<UStratShellMenuWidgetDouble>(Scope.World));
	if (!TestTrue(TEXT("FIXTURE: a UStratShellMenuWidgetDouble was constructed"), Menu.IsValid()))
	{
		return false;
	}

	Menu->PlantOptionsRow();
	if (!TestNotNull(TEXT("FIXTURE: the OptionsLabel plant took. Both members are "
			"BindWidgetOptional, so a null plant makes RefreshOptionsRow write to nothing and "
			"this clause would pass over an empty measurement"), Menu->Label())
		|| !TestNotNull(TEXT("FIXTURE: the OptionsButton plant took"), Menu->Button()))
	{
		return false;
	}

	// ---- THE CONTROL THAT MAKES THE LABEL COMPARISON DISCRIMINATING ------------------------
	//
	// FIVE DISTINCT LABELS. Without this, "the button draws the options row's label" is
	// satisfiable by an implementation that read any index whose row happened to say the same
	// thing. It is asserted first because every assertion after it depends on it.
	const FStratShellMenuModel Model = Shell->GetMenuModel();

	if (!TestTrue(TEXT("premise: the shell's menu model has rows at all. An empty model makes "
			"every loop below vacuous"), Model.Options.Num() > 0))
	{
		return false;
	}

	for (int32 Left = 0; Left < Model.Options.Num(); ++Left)
	{
		for (int32 Right = Left + 1; Right < Model.Options.Num(); ++Right)
		{
			TestFalse(*FString::Printf(
				TEXT("control: the %s row and the %s row draw different labels ('%s' vs '%s'). "
				     "If two rows ever share a caption, the by-route assertion below stops "
				     "being able to tell a by-index read apart from a by-route one"),
				*RouteWord(Model.Options[Left].Route), *RouteWord(Model.Options[Right].Route),
				*Model.Options[Left].Label.ToString(), *Model.Options[Right].Label.ToString()),
				Model.Options[Left].Label.EqualTo(Model.Options[Right].Label));
		}
	}

	// ---- THE OPTIONS ROW, FOUND THE WAY THE SHIPPED CODE FINDS IT --------------------------
	const int32 OptionsIndex = Model.Options.IndexOfByPredicate(
		[](const FStratShellOption& Candidate)
		{
			return Candidate.Route == EStratShellRoute::Options;
		});

	if (!TestTrue(TEXT("premise: the shell's menu model carries an Options row at all"),
			Model.Options.IsValidIndex(OptionsIndex)))
	{
		return false;
	}

	TestTrue(*FString::Printf(
		TEXT("control: the Options row is not the only row (there are %d), so a fixed-index "
		     "read has somewhere else to land and the clause is not trivially satisfied"),
		Model.Options.Num()), Model.Options.Num() > 1);

	const FStratShellOption& OptionsRow = Model.Options[OptionsIndex];

	// ---- THE REFRESH -----------------------------------------------------------------------
	//
	// PREMISE FIRST: the planted label says nothing yet, so the assertion after the refresh is a
	// measurement of the refresh rather than of the plant.
	if (!TestTrue(TEXT("premise: the planted label is empty before RefreshOptionsRow runs"),
			Menu->Label()->GetText().IsEmpty()))
	{
		return false;
	}

	Menu->RefreshOptionsRow();

	TestTrue(*FString::Printf(
		TEXT("the options row's caption is the model's own label for the Options ROUTE, which "
		     "sits at index %d of %d (the button says '%s'; that row says '%s'). A by-index "
		     "read is red here the day a sixth route is declared ahead of Options"),
		OptionsIndex, Model.Options.Num(), *Menu->Label()->GetText().ToString(),
		*OptionsRow.Label.ToString()),
		Menu->Label()->GetText().EqualTo(OptionsRow.Label));

	// ---- AND THE ENABLED STATE IS COPIED, NOT DECIDED --------------------------------------
	//
	// NON-DISCRIMINATING TODAY, AND SAID HERE SO THE GREEN IS NOT READ AS A MEASUREMENT.
	// `IsRoutePermitted`'s `Options` arm returns `true` unconditionally, so both sides of this
	// comparison are `true` in every state a fixture can build and an unconditional
	// `SetIsEnabled(true)` passes it. Kept because it goes live the day that arm grows a
	// refusal, which its own comment names as the place one would go. See the header block.
	TestEqual(TEXT("the options button's enabled state is the model row's own, copied across "
		"without a branch"), Menu->Button()->GetIsEnabled(), OptionsRow.bEnabled);

	// ---- CLICKING REACHES THE ROUTE --------------------------------------------------------
	//
	// THE BIND IS PART OF THE SUBJECT. `NativeConstruct` is what wires the button, and a menu
	// whose caption is perfect and whose button reaches nothing is the defect
	// `StratShellSubsystem.h` recorded as *"clicking the row runs RequestOptionsPanel and
	// nothing appears"*, one layer further back.
	Menu->RunNativeConstruct();

	const int32 RequestsBefore = Shell->GetOptionsPanelRequestCount();
	Menu->Button()->OnClicked.Broadcast();

	TestEqual(TEXT("one click on the title menu's Options button requests the options panel "
		"exactly once, through ExecuteRoute"),
		Shell->GetOptionsPanelRequestCount() - RequestsBefore, 1);

	// AND THE UNBIND, so a torn-down title menu cannot request a route from the map after it.
	Menu->RunNativeDestruct();
	const int32 AfterDestruct = Shell->GetOptionsPanelRequestCount();
	Menu->Button()->OnClicked.Broadcast();

	TestEqual(TEXT("after NativeDestruct the button no longer reaches the menu"),
		Shell->GetOptionsPanelRequestCount(), AfterDestruct);

	return true;
}
