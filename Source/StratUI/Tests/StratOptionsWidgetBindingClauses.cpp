// `UStratOptionsWidget`'s SEVEN BOUND SUB-WIDGETS, under `GATE-AUDIO-SETTINGS`.
//
// THE ID IS RULED AND NOT CHOSEN HERE. `Tools/architect/state/global.md`'s third acceptance-ID
// ruling of 2026-09-05 covers this subject in terms: `GATE-AUDIO-SETTINGS`'s own authorizing
// text already names *"`UStratOptionsWidget`'s push-model setters"* as one of its three facets,
// and the ruling extends that to *"the two sliders and texts not yet exercised, and the back
// button"* on the grounds that they are *"the SAME class and the SAME push-model facet"*. No
// name is minted here. `T-UI-03` was refused for this surface by `StratOptionsModelClauses.cpp`
// (no `strat::UiSnapshot` behind an audio gain) and `T-UI-05` by the ruling (its subject is the
// `+X/turn` income line).
//
// ---------------------------------------------------------------------------------------
// WHAT `StratOptionsModelClauses.cpp` ALREADY PINS, SO A READER KNOWS WHAT IS NEW HERE.
// That file has two clauses and BOTH are about the class with NO widget tree: the model
// builder's clamp-and-render, and the fact that `PushAudioOptions` broadcasts nothing while the
// three setters each broadcast once. Neither touches a bound sub-widget, because a native
// subclass constructed by `NewObject` has all seven pointers null and every bind inside
// `NativeConstruct` skipped. THE SHIPPED SCREEN IS THE OTHER CASE, and until this file nothing
// observed it: a slider wired to the wrong channel, a value text drawing another row's string,
// or a back button wired to a commit instead of a dismiss would all have been invisible.
//
// HOW THE GAP IS CLOSED: `UStratOptionsWidgetDouble::PlantBoundWidgets` fills the seven members
// with real `USlider` / `UTextBlock` / `UButton` objects -- what the Blueprint compiler would
// have done -- and `RunNativeConstruct` then runs the SHIPPED bind body over them. Everything
// asserted below is read back off `UStratOptionsWidget::Model` or off a planted widget the
// shipped code wrote to. Nothing here composes a percent string or decides a channel.
//
// WHERE THE EXPECTATIONS COME FROM, ROW BY ROW.
//   - The three value texts are compared against `Model.<Channel>VolumeText`, the widget's own
//     field, asked of the widget after the shipped sync ran. This file never calls
//     `StratBuildAudioOptionsModel` to predict a string and never writes "45%".
//   - The three slider values are compared against `Model.<Channel>Volume`, likewise.
//   - The committed model is compared against the widget's own `Model`, which is
//     `StratOptionsModelClauses.cpp`'s established shape for this delegate.
//   - The only literals are the RAW inputs handed to the setters, and they are inputs rather
//     than expectations: no assertion states what any of them becomes.
//
// WHAT THESE CLAUSES DO **NOT** PIN, STATED RATHER THAN LEFT TO BE INFERRED.
//   - THE RE-ENTRANCY GUARD, IN PART -- AND THE OLD WORDING OF THIS BULLET WAS MEASURED FALSE.
//     It read: *"Measured on this tree: UMG's `SetValue` does NOT broadcast -- the Slate side
//     fires that delegate on user interaction only -- so the guard's early return is UNREACHABLE
//     from a headless clause ... A guard deleted outright stays green here."* THAT IS WRONG, AND
//     THE ENGINE SOURCE SAYS SO. `USlider::SetValue` (`Slider.cpp`) calls
//     `MySlider->SetValue` inside an `if (MySlider.IsValid())`, and then, in a SEPARATE and
//     UNGUARDED `if (Value != InValue)` block, assigns and calls `HandleOnValueChanged(InValue)`
//     -- which broadcasts `OnValueChanged`. The reasoning behind the old sentence was about
//     `SSlider`, the Slate widget, and the widget under test is the `U`. A headless
//     `UStratOptionsWidget` with no `MySlider` still broadcasts on every value CHANGE, so the
//     re-entrant path is reached on the shipped sync path and the three handler guards
//     (`HandleMasterSliderChanged`, `HandleSfxSliderChanged`, `HandleMusicSliderChanged`) ARE
//     load-bearing: deleting them reddens three clauses below. Measured 2026-09-05 in the mutant
//     battery. No seam is needed and none is coming.
//     WHAT IS STILL NOT PINNED IS THE RECURSION BRAKE -- the `if (bSyncingBoundWidgets) return;`
//     at the top of `SyncBoundWidgetsToModel` ITSELF, which is a different line from the three
//     handler guards. Deleting that one alone is green across the whole suite, because the
//     handlers stop the loop one level earlier. `StratOptionsWidget.cpp` records the same split
//     from the other side. So: the PROPERTY (a sync commits nothing, a setter commits exactly
//     once) is pinned; the brake as an independent line is not.
//   - THAT ANY SHIPPED `WBP_Options` CARRIES THESE SEVEN NAMES. That is an asset fact and a
//     different instrument; `BindWidget` enforces four of them at Blueprint compile time.
//   - THAT THE SLIDERS ARE LAID OUT, VISIBLE, OR OPERABLE BY A MOUSE. No clause in this suite
//     can observe Slate geometry; a human at the keyboard remains the only instrument.
//   - `OnAudioOptionsRefreshed`. A `BlueprintImplementableEvent` with no Blueprint under it is
//     a no-op, which `StratOptionsWidgetDouble.h` already records.
// ---------------------------------------------------------------------------------------

#include "Misc/AutomationTest.h"

#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Containers/UnrealString.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "StratOptionsWidget.h"
#include "StratOptionsWidgetDouble.h"

namespace StratOptionsWidgetBinding
{
	/**
	 * A planted, natively-constructed options screen with a commit observer bound.
	 *
	 * THREE DELIBERATELY DISTINCT SEED GAINS. Every clause below can be satisfied by a widget
	 * that puts one channel's value in all three rows if the three rows agree, so they must not:
	 * 0.8 / 0.4 / 0.1 round to three different percents and are three different floats.
	 */
	struct FPlantedScreen
	{
		TStrongObjectPtr<UStratOptionsWidgetDouble> Widget;

		/** Empty when the fixture stood up; otherwise the first thing that went wrong. */
		FString Failure;

		FPlantedScreen()
		{
			Widget = TStrongObjectPtr<UStratOptionsWidgetDouble>(
				NewObject<UStratOptionsWidgetDouble>());
			if (!Widget.IsValid())
			{
				Failure = TEXT("could not construct a UStratOptionsWidgetDouble");
				return;
			}

			Widget->PlantBoundWidgets();
			if (!Widget->AreBoundWidgetsPlanted())
			{
				Failure = TEXT("one or more of the seven bound sub-widgets failed to allocate; "
					"a null plant silently skips the bind it exists for, so nothing below "
					"would have been observed");
				return;
			}

			Widget->OnAudioOptionsCommitted.AddDynamic(
				Widget.Get(), &UStratOptionsWidgetDouble::HandleCommitted);
			Widget->OnOptionsDismissed.AddDynamic(
				Widget.Get(), &UStratOptionsWidgetDouble::HandleDismissed);

			// THE SHIPPED BIND BODY, RUN OVER THE PLANT. This is the line the whole file is
			// about: without it the four controls are inert objects and every broadcast below
			// reaches nothing.
			Widget->RunNativeConstruct();

			// Seeded through `PushAudioOptions`, which by `StratOptionsModelClauses.cpp`'s
			// existing clause commits nothing -- so the observer's counters are still zero and
			// each clause below measures only its own action.
			Widget->PushAudioOptions(StratBuildAudioOptionsModel(0.8f, 0.4f, 0.1f));
		}

		FPlantedScreen(const FPlantedScreen&)            = delete;
		FPlantedScreen& operator=(const FPlantedScreen&) = delete;
	};
}

// ---------------------------------------------------------------------------------------
// (1) Every value text equals its own model field, and every slider its own model float.
//
// THE DEFECT THIS IS FOR: a screen whose SFX row draws the music percent. It looks like a
// working screen -- three sliders, three labels, all moving -- and it is wrong in the one way a
// player would report as "the volume settings don't stick", because the row they dragged is not
// the row they read.
//
// THE THREE CROSS-CHECKS AT THE END ARE THE CLAUSE. Three equalities alone are satisfied by a
// widget that assigns one string to all three labels IF the three strings happen to agree, so
// the clause also asserts the three drawn strings are pairwise DIFFERENT. That is what makes
// the equalities discriminating rather than decorative.
//
// MUTANT: in `SyncBoundWidgetsToModel`, `SfxValueText->SetText(Model.MusicVolumeText)`.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratOptionsValueTextsTrackTheirOwnFieldsTest,
	"Stratocracy.StratUI.GATE-AUDIO-SETTINGS.EveryOptionsValueTextEqualsItsOwnModelField",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratOptionsValueTextsTrackTheirOwnFieldsTest::RunTest(const FString& /*Parameters*/)
{
	StratOptionsWidgetBinding::FPlantedScreen Screen;
	if (!Screen.Failure.IsEmpty())
	{
		AddError(Screen.Failure);
		return false;
	}

	UStratOptionsWidgetDouble* const Widget = Screen.Widget.Get();
	const FStratAudioOptionsModel& Model = Widget->Model;

	// ---- 1. EACH LABEL DRAWS ITS OWN FIELD ------------------------------------------------
	TestTrue(*FString::Printf(
		TEXT("the master label draws Model.MasterVolumeText (it says '%s'; the field says '%s')"),
		*Widget->MasterText()->GetText().ToString(), *Model.MasterVolumeText.ToString()),
		Widget->MasterText()->GetText().EqualTo(Model.MasterVolumeText));

	TestTrue(*FString::Printf(
		TEXT("the sfx label draws Model.SfxVolumeText (it says '%s'; the field says '%s')"),
		*Widget->SfxText()->GetText().ToString(), *Model.SfxVolumeText.ToString()),
		Widget->SfxText()->GetText().EqualTo(Model.SfxVolumeText));

	TestTrue(*FString::Printf(
		TEXT("the music label draws Model.MusicVolumeText (it says '%s'; the field says '%s')"),
		*Widget->MusicText()->GetText().ToString(), *Model.MusicVolumeText.ToString()),
		Widget->MusicText()->GetText().EqualTo(Model.MusicVolumeText));

	// ---- 2. EACH SLIDER CARRIES ITS OWN FLOAT ---------------------------------------------
	TestEqual(TEXT("the master slider carries Model.MasterVolume"),
		Widget->Master()->GetValue(), Model.MasterVolume);
	TestEqual(TEXT("the sfx slider carries Model.SfxVolume"),
		Widget->Sfx()->GetValue(), Model.SfxVolume);
	TestEqual(TEXT("the music slider carries Model.MusicVolume"),
		Widget->Music()->GetValue(), Model.MusicVolume);

	// ---- 3. THE THREE ROWS ARE THREE ROWS --------------------------------------------------
	// Without these, every equality above is satisfied by one string in all three labels.
	TestFalse(*FString::Printf(TEXT("control: the master and sfx labels differ ('%s' vs '%s')"),
		*Widget->MasterText()->GetText().ToString(), *Widget->SfxText()->GetText().ToString()),
		Widget->MasterText()->GetText().EqualTo(Widget->SfxText()->GetText()));
	TestFalse(*FString::Printf(TEXT("control: the sfx and music labels differ ('%s' vs '%s')"),
		*Widget->SfxText()->GetText().ToString(), *Widget->MusicText()->GetText().ToString()),
		Widget->SfxText()->GetText().EqualTo(Widget->MusicText()->GetText()));
	TestFalse(*FString::Printf(TEXT("control: the master and music labels differ ('%s' vs '%s')"),
		*Widget->MasterText()->GetText().ToString(), *Widget->MusicText()->GetText().ToString()),
		Widget->MasterText()->GetText().EqualTo(Widget->MusicText()->GetText()));

	// ---- 4. AND THE SEED ITSELF COMMITTED NOTHING ------------------------------------------
	// The premise every count in the sibling clauses rests on, asserted where it is cheapest.
	TestEqual(TEXT("seeding the planted screen still commits nothing, now that there are real "
		"sliders for the sync to write to"), Widget->CommitCount, 0);

	return true;
}

// ---------------------------------------------------------------------------------------
// (2) A bound slider reaches only its own channel, and exactly once.
//
// THE DEFECT THIS IS FOR: `MusicSlider->OnValueChanged` bound to `HandleSfxSliderChanged`. Two
// sliders then move one gain, one gain is unreachable, and NOTHING about the screen looks
// broken -- both labels update, because both are synced from a model that really did change.
//
// IT DRIVES THE DELEGATE AND NOT THE SETTER, WHICH IS THE POINT. Calling `SetSfxVolume`
// directly proves the setter works and says nothing about which slider is wired to it;
// `StratOptionsModelClauses.cpp` already does that and it is not repeated here. Broadcasting
// `SfxSlider->OnValueChanged` runs the shipped `NativeConstruct` binding, which is the fact.
//
// THE OTHER TWO CHANNELS ARE ASSERTED UNCHANGED AGAINST THE SEED READ BEFORE THE BROADCAST --
// not against a literal -- so a setter that wrote all three would be red on the two it should
// not have touched.
//
// MUTANT: in `UStratOptionsWidget::NativeConstruct`,
// `MusicSlider->OnValueChanged.AddDynamic(this, &UStratOptionsWidget::HandleSfxSliderChanged)`.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratOptionsSliderReachesOneChannelTest,
	"Stratocracy.StratUI.GATE-AUDIO-SETTINGS.EachOptionsSliderReachesOnlyItsOwnChannel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratOptionsSliderReachesOneChannelTest::RunTest(const FString& /*Parameters*/)
{
	StratOptionsWidgetBinding::FPlantedScreen Screen;
	if (!Screen.Failure.IsEmpty())
	{
		AddError(Screen.Failure);
		return false;
	}

	UStratOptionsWidgetDouble* const Widget = Screen.Widget.Get();

	// A value that is none of the three seeds, so an "unchanged" assertion below cannot be
	// satisfied by coincidence.
	const float Driven = 0.625f;

	// ---- SFX -------------------------------------------------------------------------------
	{
		const FStratAudioOptionsModel Before = Widget->Model;
		const int32 CommitsBefore = Widget->CommitCount;

		Widget->Sfx()->OnValueChanged.Broadcast(Driven);

		TestEqual(TEXT("dragging the sfx slider produces exactly one commit"),
			Widget->CommitCount - CommitsBefore, 1);
		TestEqual(TEXT("and the sfx gain is now the driven value"),
			Widget->Model.SfxVolume, Driven);
		TestEqual(TEXT("the master gain is untouched by the sfx slider"),
			Widget->Model.MasterVolume, Before.MasterVolume);
		TestEqual(TEXT("the music gain is untouched by the sfx slider"),
			Widget->Model.MusicVolume, Before.MusicVolume);
		TestEqual(TEXT("and the commit carries the widget's own model, not a third value"),
			Widget->LastCommitted.SfxVolume, Widget->Model.SfxVolume);
	}

	// ---- MUSIC -----------------------------------------------------------------------------
	{
		const FStratAudioOptionsModel Before = Widget->Model;
		const int32 CommitsBefore = Widget->CommitCount;
		const float DrivenMusic = 0.375f;

		Widget->Music()->OnValueChanged.Broadcast(DrivenMusic);

		TestEqual(TEXT("dragging the music slider produces exactly one commit"),
			Widget->CommitCount - CommitsBefore, 1);
		TestEqual(TEXT("and the music gain is now the driven value"),
			Widget->Model.MusicVolume, DrivenMusic);
		TestEqual(TEXT("the master gain is untouched by the music slider"),
			Widget->Model.MasterVolume, Before.MasterVolume);
		TestEqual(TEXT("the sfx gain is untouched by the music slider -- this is the wire the "
			"mutant crosses"), Widget->Model.SfxVolume, Before.SfxVolume);
	}

	// ---- MASTER ----------------------------------------------------------------------------
	{
		const FStratAudioOptionsModel Before = Widget->Model;
		const int32 CommitsBefore = Widget->CommitCount;
		const float DrivenMaster = 0.125f;

		Widget->Master()->OnValueChanged.Broadcast(DrivenMaster);

		TestEqual(TEXT("dragging the master slider produces exactly one commit"),
			Widget->CommitCount - CommitsBefore, 1);
		TestEqual(TEXT("and the master gain is now the driven value"),
			Widget->Model.MasterVolume, DrivenMaster);
		TestEqual(TEXT("the sfx gain is untouched by the master slider"),
			Widget->Model.SfxVolume, Before.SfxVolume);
		TestEqual(TEXT("the music gain is untouched by the master slider"),
			Widget->Model.MusicVolume, Before.MusicVolume);
	}

	// ---- AND THE LABEL FOLLOWED ------------------------------------------------------------
	// A setter that wrote the model but never re-synced would pass everything above. The label
	// is compared against the widget's own field, as in clause (1).
	TestTrue(*FString::Printf(TEXT("the master label followed the master slider (it says '%s'; "
		"the field says '%s')"), *Widget->MasterText()->GetText().ToString(),
		*Widget->Model.MasterVolumeText.ToString()),
		Widget->MasterText()->GetText().EqualTo(Widget->Model.MasterVolumeText));

	// ---- AND NOTHING WAS DISMISSED ---------------------------------------------------------
	TestEqual(TEXT("no amount of slider dragging dismisses the screen"), Widget->DismissCount, 0);

	return true;
}

// ---------------------------------------------------------------------------------------
// (3) The back button dismisses, and dismissing commits nothing.
//
// THE TWO HALVES ARE ONE CLAUSE BECAUSE EITHER ALONE IS SATISFIED BY A DEFECT. A back button
// wired to nothing passes "commits nothing" perfectly and leaves the player stuck on a screen
// with no exit -- which `StratOptionsWidget.h` calls worse than a screen that fails to compile.
// A back button that ALSO committed would re-save the slot on the way out, which is the same
// spurious-echo hazard `SeedingTheOptionsScreenCommitsNothing` pins on the other end.
//
// AND THE UNBIND IS ASSERTED, because `NativeDestruct` dropping the button's side is what keeps
// a re-created panel from firing one dismiss per lifetime it has ever had. It is checked by
// broadcasting the button AFTER the destruct and finding the counter still.
//
// MUTANT: `UStratOptionsWidget::HandleBackClicked` bodied as
// `OnAudioOptionsCommitted.Broadcast(Model);` -- red on the commit count and on the dismiss
// count both. A weaker mutant, an empty `HandleBackClicked`, is red on the dismiss count alone.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratOptionsBackButtonDismissesTest,
	"Stratocracy.StratUI.GATE-AUDIO-SETTINGS.TheOptionsBackButtonDismissesWithoutCommitting",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratOptionsBackButtonDismissesTest::RunTest(const FString& /*Parameters*/)
{
	StratOptionsWidgetBinding::FPlantedScreen Screen;
	if (!Screen.Failure.IsEmpty())
	{
		AddError(Screen.Failure);
		return false;
	}

	UStratOptionsWidgetDouble* const Widget = Screen.Widget.Get();

	TestEqual(TEXT("premise: nothing has been dismissed yet"), Widget->DismissCount, 0);
	TestEqual(TEXT("premise: nothing has been committed yet"), Widget->CommitCount, 0);

	Widget->Back()->OnClicked.Broadcast();

	TestEqual(TEXT("clicking Back dismisses the screen exactly once"), Widget->DismissCount, 1);
	TestEqual(TEXT("and clicking Back commits nothing -- leaving the screen must not re-save "
		"the settings slot"), Widget->CommitCount, 0);

	// A second click is a second dismiss, not a latch. An owner that reconciles rather than
	// reacts (which `UStratOptionsPresenter` does) needs the second one to arrive.
	Widget->Back()->OnClicked.Broadcast();
	TestEqual(TEXT("a second click dismisses again -- the broadcast is not one-shot"),
		Widget->DismissCount, 2);

	// ---- THE UNBIND ------------------------------------------------------------------------
	Widget->RunNativeDestruct();
	const int32 AfterDestruct = Widget->DismissCount;

	Widget->Back()->OnClicked.Broadcast();
	TestEqual(TEXT("after NativeDestruct the button no longer reaches the widget, so a stale "
		"click cannot dismiss a screen that is gone"), Widget->DismissCount, AfterDestruct);

	return true;
}
