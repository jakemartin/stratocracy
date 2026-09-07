// `UStratOptionsWidget`'s EXIT ROW -- the `ReturnToTitle` control added 2026-09-07, under
// `GATE-TITLEMENU`.
//
// ---------------------------------------------------------------------------------------
// THE ACCEPTANCE ID, AND WHY IT IS NOT THE ONE THE BRIEF ASKED FOR. The engineer's proposal
// named these clauses `Stratocracy.StratUI.T-UI-03.*`. **`T-UI-03` IS REFUSED FOR THIS
// SURFACE, AND IT WAS ALREADY REFUSED IN WRITING FOR THE SURFACE NEXT DOOR.**
// `Tools/architect/state/global.md`'s third acceptance-ID ruling of 2026-09-05 turned down
// `T-UI-03` for `UStratCommandBarWidget::OptionsButton` in terms: that ID's clause is *"every
// number a widget draws equals exactly one `strat::UiSnapshot` field"*, and a button that
// *"draws no value and reads no snapshot field -- it broadcasts ... and nothing else"* has *"no
// snapshot-fidelity fact for `T-UI-03` to own"*. The ruling adds the sentence that decides this
// file: *"Sitting on the SAME command bar as clauses that do ride `T-UI-03` does not pull
// `OptionsButton` onto that ID; the test is the subject sentence, not the file."* The exit row
// is the identical shape. There is no `strat::UiSnapshot` behind a route permission -- the
// sentence it draws is written by `UStratShellSubsystem::IsRoutePermitted`, one module over --
// so `T-UI-03` has nothing here to own. `StratOptionsWidget.h` itself calls the reason label
// *"`T-UI-03`'s clause on this surface"*, which is the same category slip and is a finding
// reported rather than repeated [FALSE SINCE THE ENGINEER'S 2026-09-07 SECOND PASS; SEE THE
// STAMP BELOW. The ID RULING above is untouched -- only the report of an open finding is stale].
//
// STAMPED 2026-09-07 BY `strat-test-author` OVER BASE `46321a6`. The finding was ACCEPTED and
// REPAIRED by `strat-gameplay-engineer` in the same tree, in a pass that reached this file's
// header after it was written. The header now carries an `AMENDED 2026-09-07 (SECOND PASS)` block
// retracting the name, and the words this file quoted survive only inside `RETRACTED>` lines --
// which is why the quotation is left standing rather than deleted: the sentence is still in the
// file, and a reader who greps for it should land on an explanation rather than on nothing.
// INSTRUMENT, re-run over the tree this stamp was written in:
// `grep -c "clause on this surface" Source/StratUI/StratOptionsWidget.h` returns `2`, and both
// hits open `RETRACTED>`. Only one of the two is the exit reason label (`ReturnToTitleReason`);
// the other is on `MasterValueText`, a volume readout no clause in this file touches -- it reads
// *"one field, one drawn NUMBER"*, which is how the two are told apart without a line number.
//
// `GATE-TITLEMENU` INSTEAD, ON DIRECT PRECEDENT AND MINTING NOTHING. The same ruling places
// `OptionsButton` -- a `StratUI` button whose whole job is reaching an `EStratShellRoute` arm
// from a surface that is not the title screen -- under `GATE-TITLEMENU`, because *"`GATE-
// TITLEMENU`'s subject is the ROUTE, not the screen that requests it"* and *"the fact that
// decides it -- which route it reaches -- has nothing to do with which widget class asks."*
// `ReturnToTitleButton` is that sentence word for word with a different arm of the same enum,
// and `Stratocracy.StratUI.GATE-TITLEMENU.TheCommandBarOptionsButtonBroadcastsExactlyOncePerClick`
// (`StratCommandBarOptionsClauses.cpp`) is the landed precedent for the module prefix. It is
// NOT `GATE-AUDIO-SETTINGS`: no volume, persistence or mix is touched by anything below, which
// is the same test that ruling applied to keep `OptionsButton` off it.
//
// ---------------------------------------------------------------------------------------
// WHERE THE EXPECTATIONS COME FROM.
//   - Every drawn value is compared against `UStratOptionsWidget::ExitModel`'s OWN FIELD, read
//     back off the widget after the shipped sync ran -- never against the literal that was
//     pushed. The literals below are INPUTS. No assertion states what any of them becomes.
//   - The enabled bit is read from `UWidget::GetIsEnabled()` on the planted `UButton`, which
//     is the object the shipped `SyncExitWidgetsToModel` wrote to.
//   - The broadcast counts are the properties themselves and are asserted as deltas on a
//     reading taken first.
//   - Clause (2)'s "unchanged" side is compared against a copy of the model taken BEFORE the
//     drag, not against the literal that seeded it.
//
// WHAT THESE CLAUSES DO **NOT** PIN, STATED RATHER THAN LEFT TO BE INFERRED.
//   - WHERE THE BIT COMES FROM. Everything here is downstream of a push. That the pushed value
//     is `IsRoutePermitted`'s and not a locally written `bMatchIsLive` test is
//     `Source/StratPlay/Tests/StratOptionsExitRouteClauses.cpp`'s subject, and it cannot be
//     asked in this module: `EStratShellRoute` and `IsRoutePermitted` are `StratPlay` names and
//     a call to either from here is `LNK2019`.
//   - THAT ANY SHIPPED `WBP_Options` CARRIES `ReturnToTitleButton`. IT DOES NOT, TODAY [FALSE
//     SINCE THE 2026-09-07 ASSET PASS; SEE THE STAMP BELOW], and
//     that is why the member is `BindWidgetOptional` -- the header argues it at length and
//     names a clause reading the shipped CDO as its discharge condition. **That clause is not
//     in this file and cannot be written yet** [FALSE SINCE 2026-09-07; IT IS WRITTEN, IN
//     `StratShippedOptionsExitControlParity.cpp`, AND IT IS STILL NOT IN THIS FILE]; the editor
//     lane's asset pass is still owed [FALSE; IT LANDED -- SEE THE STAMP BELOW]. A
//     reader must not take clause (1) as evidence that the shipped screen has an exit control.
//
//     STAMPED 2026-09-07 BY `strat-test-author` OVER BASE `46321a6`. **THE BULLET'S POINT IS
//     UNCHANGED AND THAT IS WHY IT IS CORRECTED RATHER THAN DELETED:** not one of the four
//     clauses below loads a package or reads an asset -- every one runs against a planted
//     `UStratOptionsWidgetDouble` -- so "no clause in this file is evidence about the shipped
//     screen" is as true today as when it was written. Only the two claims about the WORLD
//     outside this file went stale.
//     INSTRUMENT for the asset half, run in the tree this stamp was written in and quoted rather
//     than summarised: `grep -a -c ReturnToTitleButton Content/UI/WBP_Options.uasset` returns
//     `1`; positive control `MasterSlider` returns `1`; negative control `ZZZNope` returns `0`.
//     The file's `sha256` is
//     `7e32ef67836b2d9241ecd70e760905d32e16def2ec7df9e2accdc4a346098adb` at 42696 bytes, against
//     the committed LFS oid `367f21e199a50bd66e671607e95c1c58baddad986afdcc94b6cd5075bc953f44`
//     at 39771 bytes -- so the control arrived in a working-tree change and not in `46321a6`.
//     A BLINDNESS WORTH KNOWING BEFORE ANYONE REPEATS THIS SCAN: `strings -a` over the same file
//     returns `0` for `ReturnToTitleButton` AND for the positive control `MasterSlider`, under
//     every `-e` encoding. `strings` cannot see these names at all; `grep -a` can. An absence
//     measured with `strings` here would have been an instrument failure reported as a fact.
//     WHAT THE SCAN STILL DOES NOT ESTABLISH, in `StratOptionsWidget.h`'s own words: *"a name
//     present in a `.uasset`'s bytes is a NAME, not a `UButton` at a known place in a compiled
//     widget tree"*. That is `StratShippedOptionsExitControlParity.cpp`'s subject, and it is a
//     separate file precisely so that this one keeps loading nothing.
//   - THAT THE CONTROL IS VISIBLE, LAID OUT, OR CLICKABLE BY A MOUSE. No clause in this suite
//     can observe Slate geometry; a human at the keyboard remains the only instrument.
//   - `OnExitOptionsRefreshed`. A `BlueprintImplementableEvent` with no Blueprint under it is
//     a no-op, which `StratOptionsWidgetDouble.h` already records for its audio twin.
//   - A RE-CHECK OF THE BIT INSIDE `HandleReturnToTitleClicked`. There is deliberately none --
//     the guard is the disabled `UButton`, which does not broadcast -- so clause (4) drives
//     the button's own `OnClicked` and asserts the broadcast, and asserts NOTHING about what a
//     click on a disabled control would do. `UButton::OnClicked.Broadcast()` from a clause
//     bypasses the enabled state entirely and would measure the fixture, not the widget.
// ---------------------------------------------------------------------------------------

#include "Misc/AutomationTest.h"

#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Containers/UnrealString.h"
#include "Internationalization/Text.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "StratOptionsWidget.h"
#include "StratOptionsWidgetDouble.h"

namespace StratOptionsExitClauses
{
	/**
	 * A planted, natively-constructed options screen with all three observers bound.
	 *
	 * IT PLANTS THE EXIT ROW AS WELL AS THE SEVEN, and both plants are checked: an unplanted
	 * `ReturnToTitleButton` makes `SyncExitWidgetsToModel` write to nothing, and a clause on
	 * the enabled bit would then be VACUOUS rather than red -- which is worse than failing.
	 *
	 * IT DOES NOT PUSH AN EXIT MODEL. Clause (3) is about exactly the unpushed state, and the
	 * other three push their own. The audio seed IS taken, because clause (2) needs a screen
	 * whose gains are somewhere other than their defaults for the drag to move.
	 */
	struct FPlantedExitScreen
	{
		TStrongObjectPtr<UStratOptionsWidgetDouble> Widget;

		/** Empty when the fixture stood up; otherwise the first thing that went wrong. */
		FString Failure;

		FPlantedExitScreen()
		{
			Widget = TStrongObjectPtr<UStratOptionsWidgetDouble>(
				NewObject<UStratOptionsWidgetDouble>());
			if (!Widget.IsValid())
			{
				Failure = TEXT("could not construct a UStratOptionsWidgetDouble");
				return;
			}

			Widget->PlantBoundWidgets();
			Widget->PlantExitRow();
			if (!Widget->AreBoundWidgetsPlanted())
			{
				Failure = TEXT("one or more of the seven audio sub-widgets failed to allocate");
				return;
			}
			if (!Widget->IsExitRowPlanted())
			{
				Failure = TEXT("the exit button or its reason label failed to allocate; without "
					"them SyncExitWidgetsToModel writes to nothing and every assertion about "
					"the exit row below would be vacuous rather than red");
				return;
			}

			Widget->OnAudioOptionsCommitted.AddDynamic(
				Widget.Get(), &UStratOptionsWidgetDouble::HandleCommitted);
			Widget->OnOptionsDismissed.AddDynamic(
				Widget.Get(), &UStratOptionsWidgetDouble::HandleDismissed);
			Widget->OnReturnToTitleRequested.AddDynamic(
				Widget.Get(), &UStratOptionsWidgetDouble::HandleReturnToTitleRequested);

			// THE SHIPPED BIND BODY, RUN OVER THE PLANT. Without it the exit button is an inert
			// object, `OnClicked` reaches nothing, and the `SyncExitWidgetsToModel()` call at
			// the end of `NativeConstruct` -- which clause (3) is entirely about -- never runs.
			Widget->RunNativeConstruct();
		}

		FPlantedExitScreen(const FPlantedExitScreen&)            = delete;
		FPlantedExitScreen& operator=(const FPlantedExitScreen&) = delete;
	};
}

// ---------------------------------------------------------------------------------------
// (1) The exit control draws exactly its pushed model, in both directions.
//
// THE DEFECT THIS IS FOR: `SyncExitWidgetsToModel` calling `SetIsEnabled(true)` unconditionally,
// or drawing the reason on the enabled path as well. Either ships a live exit on the title
// screen -- a control a player can click that then silently refuses, which
// `UStratShellSubsystem`'s own header calls *"one step worse than a greyed row"*.
//
// TWO PUSHES AND NOT ONE, WHICH IS WHAT MAKES IT DISCRIMINATING. A single disabled push is
// satisfied by a widget that never enables anything, and a single enabled push by one that
// never disables. The clause pushes refused, then permitted, then refused again -- so the
// control is required to move in BOTH directions and is not a latch.
//
// THE DRAWN STRING IS COMPARED AGAINST `ExitModel.ReturnToTitleReason` AND NEVER AGAINST THE
// LITERAL PUSHED. One field, one drawn string, no expression between them.
//
// MUTANTS:
//   - `ReturnToTitleButton->SetIsEnabled(true)` -- red on the first refused push.
//   - `SetText(FText::GetEmpty())` in place of the model field -- red on the reason.
//   - the second `SetIsEnabled` line deleted so the control latches -- red on the third push.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratOptionsExitDrawsItsModelTest,
	"Stratocracy.StratUI.GATE-TITLEMENU.TheOptionsExitControlDrawsExactlyItsPushedModel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratOptionsExitDrawsItsModelTest::RunTest(const FString& /*Parameters*/)
{
	StratOptionsExitClauses::FPlantedExitScreen Screen;
	if (!Screen.Failure.IsEmpty())
	{
		AddError(Screen.Failure);
		return false;
	}

	UStratOptionsWidgetDouble* const Widget = Screen.Widget.Get();

	// ---- 1. A REFUSED PUSH GREYS THE CONTROL AND EXPLAINS ITSELF --------------------------
	{
		FStratOptionsExitModel Refused;
		Refused.bReturnToTitleEnabled = false;
		Refused.ReturnToTitleReason   = FText::FromString(TEXT("No match in progress."));
		Widget->PushExitOptions(Refused);

		TestFalse(TEXT("a refused push greys the exit button -- and a disabled UButton does not "
			"broadcast OnClicked, which is the whole requirement"),
			Widget->Exit()->GetIsEnabled());
		TestEqual(TEXT("and the widget stored the pushed bit"),
			Widget->ExitModel.bReturnToTitleEnabled, false);
		TestTrue(*FString::Printf(TEXT("the reason label draws ExitModel.ReturnToTitleReason "
			"(it says '%s'; the field says '%s')"),
			*Widget->ExitReasonText()->GetText().ToString(),
			*Widget->ExitModel.ReturnToTitleReason.ToString()),
			Widget->ExitReasonText()->GetText().EqualTo(Widget->ExitModel.ReturnToTitleReason));
		TestFalse(TEXT("control: the drawn reason is not empty, so the equality above is not "
			"two empty strings agreeing"),
			Widget->ExitReasonText()->GetText().IsEmpty());
	}

	// ---- 2. A PERMITTED PUSH ARMS IT AND CLEARS THE EXPLANATION ---------------------------
	{
		FStratOptionsExitModel Permitted;
		Permitted.bReturnToTitleEnabled = true;
		Permitted.ReturnToTitleReason   = FText::GetEmpty();
		Widget->PushExitOptions(Permitted);

		TestTrue(TEXT("a permitted push arms the exit button"), Widget->Exit()->GetIsEnabled());
		TestEqual(TEXT("and the widget stored the pushed bit"),
			Widget->ExitModel.bReturnToTitleEnabled, true);
		TestTrue(*FString::Printf(TEXT("the reason label follows the model to empty (it says "
			"'%s')"), *Widget->ExitReasonText()->GetText().ToString()),
			Widget->ExitReasonText()->GetText().EqualTo(Widget->ExitModel.ReturnToTitleReason));
	}

	// ---- 3. AND BACK, SO THE CONTROL IS NOT A ONE-WAY LATCH -------------------------------
	{
		FStratOptionsExitModel RefusedAgain;
		RefusedAgain.bReturnToTitleEnabled = false;
		RefusedAgain.ReturnToTitleReason =
			FText::FromString(TEXT("No title level is configured."));
		Widget->PushExitOptions(RefusedAgain);

		TestFalse(TEXT("a second refusal greys the control again -- an exit that could only ever "
			"be armed would stay live after the match it belonged to ended"),
			Widget->Exit()->GetIsEnabled());
		TestTrue(*FString::Printf(TEXT("and the label draws the NEW reason (it says '%s'; the "
			"field says '%s')"), *Widget->ExitReasonText()->GetText().ToString(),
			*Widget->ExitModel.ReturnToTitleReason.ToString()),
			Widget->ExitReasonText()->GetText().EqualTo(Widget->ExitModel.ReturnToTitleReason));
	}

	// ---- 4. AND NONE OF IT WAS A COMMIT OR A DISMISS ---------------------------------------
	// `PushExitOptions` has no committing twin -- its declaration says so -- and the audio
	// counters are the only instrument that can show it did not grow one.
	TestEqual(TEXT("pushing an exit availability commits no volume"), Widget->CommitCount, 0);
	TestEqual(TEXT("and dismisses nothing"), Widget->DismissCount, 0);
	TestEqual(TEXT("and asks for no route"), Widget->ReturnToTitleRequestCount, 0);

	return true;
}

// ---------------------------------------------------------------------------------------
// (2) A volume drag does not clear the exit availability.
//
// THIS IS THE MUTANT THE TWO-STRUCT DESIGN EXISTS TO PREVENT, AND IT IS THE ONE CLAUSE IN THIS
// FILE THAT WOULD HAVE CAUGHT A DEFECT NOBODY WOULD HAVE FOUND BY READING.
// `StratBuildAudioOptionsModel` REBUILDS `FStratAudioOptionsModel` FROM THREE FLOATS on every
// setter -- `Model = StratBuildAudioOptionsModel(InVolume, Model.SfxVolume, Model.MusicVolume)`
// -- so an exit field parked on THAT struct, which is the obvious place to put it, would be
// reset to its default by every slider drag. `StratOptionsWidget.h` states the consequence:
// *"The exit row would have gone grey the first time a player touched the volume, and nothing
// in this file would have looked wrong."*
//
// IT DRIVES THE SLIDER'S DELEGATE AND NOT THE SETTER, so the path is the shipped one a player
// takes, and it drives all three channels because a fold could be written into any one of them.
//
// THE UNCHANGED SIDE IS COMPARED AGAINST A COPY TAKEN BEFORE THE DRAG, never against the
// literal that seeded it -- so this clause states no expectation about what an exit model
// SHOULD contain, only that dragging a volume is not a writer of one.
//
// MUTANT (the one it is for): move `bReturnToTitleEnabled` and `ReturnToTitleReason` onto
// `FStratAudioOptionsModel` and have `SyncExitWidgetsToModel` read them from `Model`. Green on
// clauses (1), (3) and (4) and on every audio clause in this suite; red here.
// A cheaper mutant with the same signature: add `ExitModel = FStratOptionsExitModel();` to
// `UStratOptionsWidget::SetMasterVolume`.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratOptionsVolumeDragLeavesTheExitAloneTest,
	"Stratocracy.StratUI.GATE-TITLEMENU.AVolumeDragDoesNotClearTheExitAvailability",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratOptionsVolumeDragLeavesTheExitAloneTest::RunTest(const FString& /*Parameters*/)
{
	StratOptionsExitClauses::FPlantedExitScreen Screen;
	if (!Screen.Failure.IsEmpty())
	{
		AddError(Screen.Failure);
		return false;
	}

	UStratOptionsWidgetDouble* const Widget = Screen.Widget.Get();

	// The screen as an owner leaves it in a live match: gains seeded, exit ARMED.
	Widget->PushAudioOptions(StratBuildAudioOptionsModel(0.8f, 0.4f, 0.1f));

	FStratOptionsExitModel Permitted;
	Permitted.bReturnToTitleEnabled = true;
	Permitted.ReturnToTitleReason   = FText::GetEmpty();
	Widget->PushExitOptions(Permitted);

	if (!TestTrue(TEXT("premise: the exit is armed before anything is dragged"),
			Widget->Exit()->GetIsEnabled()))
	{
		return false;
	}

	// The whole model as it stood before the drag. THE EXPECTATION SIDE, and it is the widget's
	// own field rather than anything this clause composed.
	const FStratOptionsExitModel Before = Widget->ExitModel;

	// ---- THREE DRAGS, ONE PER CHANNEL, THROUGH THE SHIPPED SLIDER BINDING ------------------
	Widget->Master()->OnValueChanged.Broadcast(0.125f);
	Widget->Sfx()->OnValueChanged.Broadcast(0.625f);
	Widget->Music()->OnValueChanged.Broadcast(0.375f);

	// ---- 1. CONTROL: THE DRAGS REALLY HAPPENED --------------------------------------------
	// Without this the clause is satisfied by three broadcasts that reached nothing, which is
	// the shape a "nothing changed" assertion fails in most quietly.
	if (!TestEqual(TEXT("CONTROL: the three drags each produced a commit, so the setters really "
			"ran and the assertions below are about a screen that was used"),
			Widget->CommitCount, 3))
	{
		return false;
	}
	TestEqual(TEXT("CONTROL: and the master gain moved to the driven value"),
		Widget->Model.MasterVolume, 0.125f);

	// ---- 2. THE EXIT MODEL IS UNTOUCHED ----------------------------------------------------
	TestEqual(TEXT("dragging every volume slider leaves bReturnToTitleEnabled alone. A false "
		"here means the exit fields were folded into the struct StratBuildAudioOptionsModel "
		"rebuilds on every setter, and the exit greys the first time a player touches a volume"),
		Widget->ExitModel.bReturnToTitleEnabled, Before.bReturnToTitleEnabled);
	TestTrue(TEXT("and leaves ReturnToTitleReason alone"),
		Widget->ExitModel.ReturnToTitleReason.EqualTo(Before.ReturnToTitleReason));

	// ---- 3. AND THE CONTROL ON SCREEN IS STILL ARMED ---------------------------------------
	// The model surviving is not the same fact as the button surviving: a sync that re-ran with
	// a stale local would be red here and green above.
	TestTrue(TEXT("and the exit button is still armed after three drags"),
		Widget->Exit()->GetIsEnabled());
	TestTrue(TEXT("and the reason label still draws the model's own field"),
		Widget->ExitReasonText()->GetText().EqualTo(Widget->ExitModel.ReturnToTitleReason));

	// ---- 4. AND NO DRAG ASKED TO LEAVE THE MATCH -------------------------------------------
	TestEqual(TEXT("no amount of slider dragging asks for the title screen"),
		Widget->ReturnToTitleRequestCount, 0);

	return true;
}

// ---------------------------------------------------------------------------------------
// (3) An options screen nobody has pushed to has a DISABLED exit.
//
// THE DEFECT THIS IS FOR, AND IT IS A REAL SHIPPED-STATE HAZARD RATHER THAN A THEORETICAL ONE.
// A `UButton` authored in a WBP is ENABLED. `FStratOptionsExitModel::bReturnToTitleEnabled`
// defaults false, but a default in a struct nobody reads changes nothing on screen -- so the
// line that makes the shipped state safe is the `SyncExitWidgetsToModel()` call at the end of
// `NativeConstruct`, which draws the default before any owner has pushed. Without it, a screen
// constructed on a map whose presenter never seeds shows a LIVE exit wired to whatever happens
// to be bound. `StratOptionsWidget.cpp`'s own comment names this exact case.
//
// IT ALSO PINS THE `false` DEFAULT AGAINST A FUTURE FLIP, which is the second half: a
// `bReturnToTitleEnabled = true` default would make the same construct draw an armed control.
//
// THE CLAUSE PUSHES NOTHING, DELIBERATELY. Any push at all destroys the subject.
//
// MUTANTS:
//   - the `SyncExitWidgetsToModel()` line at the end of `NativeConstruct` deleted -- red, with
//     the button reading enabled because that is a `UButton`'s own default.
//   - `bReturnToTitleEnabled = true` as the struct's default -- red on both assertions.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratOptionsUnpushedExitIsDisabledTest,
	"Stratocracy.StratUI.GATE-TITLEMENU.AnUnpushedOptionsScreenHasADisabledExit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratOptionsUnpushedExitIsDisabledTest::RunTest(const FString& /*Parameters*/)
{
	StratOptionsExitClauses::FPlantedExitScreen Screen;
	if (!Screen.Failure.IsEmpty())
	{
		AddError(Screen.Failure);
		return false;
	}

	UStratOptionsWidgetDouble* const Widget = Screen.Widget.Get();

	// CONTROL, AND IT IS THE PREMISE THE WHOLE CLAUSE RESTS ON: a freshly allocated `UButton`
	// is ENABLED, so the assertion below is a statement about the shipped construct body and
	// not about UMG's own default. Measured here rather than asserted from memory.
	UButton* const Fresh = NewObject<UButton>(Widget);
	if (!TestNotNull(TEXT("FIXTURE: a control UButton allocated"), Fresh))
	{
		return false;
	}
	if (!TestTrue(TEXT("CONTROL: a freshly allocated UButton is ENABLED, so 'the exit is "
			"disabled' below is a fact about NativeConstruct and not about UMG"),
			Fresh->GetIsEnabled()))
	{
		return false;
	}

	TestFalse(TEXT("premise: no owner has pushed an exit availability"),
		Widget->ExitModel.bReturnToTitleEnabled);

	TestFalse(TEXT("a screen nobody seeded draws a GREYED exit. An armed one here would be a "
		"control broadcasting into whatever happens to be bound, on a map whose presenter "
		"never said the exit was legal"), Widget->Exit()->GetIsEnabled());

	TestTrue(TEXT("and its reason label is empty, because the default model carries no "
		"sentence and this class composes none"),
		Widget->ExitReasonText()->GetText().IsEmpty());

	return true;
}

// ---------------------------------------------------------------------------------------
// (4) The exit button asks to leave, once per click, and does nothing else.
//
// THE DEFECT THIS IS FOR: `ReturnToTitleButton->OnClicked` bound to `HandleBackClicked` -- a
// wiring slip between two buttons on one screen that would make the exit merely CLOSE the
// options panel. Nothing about the screen would look broken; the player would press the exit,
// land back in the match, and report it as "the quit button doesn't work".
//
// AND THE UNBIND, for `TheOptionsBackButtonDismissesWithoutCommitting`'s stated reason:
// `NativeDestruct` dropping this button's side is what keeps a re-created panel from asking to
// leave once per lifetime it has ever had -- and this delegate's consequence is a level travel
// rather than a closed panel.
//
// IT ASSERTS NOTHING ABOUT A CLICK ON A DISABLED CONTROL. `OnClicked.Broadcast()` from a clause
// bypasses the enabled state, so such an assertion would measure the fixture. That the guard is
// the disabled button and not a re-check inside the handler is the design's own statement
// (`StratOptionsWidget.h`, `OnReturnToTitleRequested`), and it is deliberately unpinned here.
//
// MUTANTS:
//   - `ReturnToTitleButton->OnClicked.AddDynamic(this, &UStratOptionsWidget::HandleBackClicked)`
//     -- red on the request count (0) and on the dismiss count (1).
//   - `HandleReturnToTitleClicked` bodied as `{}` -- red on the request count alone.
//   - the `RemoveDynamic` in `NativeDestruct` deleted -- red on the post-destruct assertion.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratOptionsExitButtonAsksToLeaveTest,
	"Stratocracy.StratUI.GATE-TITLEMENU.TheOptionsExitButtonBroadcastsTheRequestAndDismissesNothing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratOptionsExitButtonAsksToLeaveTest::RunTest(const FString& /*Parameters*/)
{
	StratOptionsExitClauses::FPlantedExitScreen Screen;
	if (!Screen.Failure.IsEmpty())
	{
		AddError(Screen.Failure);
		return false;
	}

	UStratOptionsWidgetDouble* const Widget = Screen.Widget.Get();

	TestEqual(TEXT("premise: nothing has asked to leave yet"),
		Widget->ReturnToTitleRequestCount, 0);

	Widget->Exit()->OnClicked.Broadcast();

	TestEqual(TEXT("clicking the exit asks to leave exactly once"),
		Widget->ReturnToTitleRequestCount, 1);
	TestEqual(TEXT("and does NOT dismiss the panel -- the owner must, because leaving the flag "
		"open across a level travel puts an options screen over the title menu the player just "
		"arrived at"), Widget->DismissCount, 0);
	TestEqual(TEXT("and commits no volume"), Widget->CommitCount, 0);

	// A second click is a second request, not a latch. An owner whose route REFUSED must be
	// reachable again.
	Widget->Exit()->OnClicked.Broadcast();
	TestEqual(TEXT("a second click asks again -- the broadcast is not one-shot, which matters "
		"because a refused route leaves the player on this screen"),
		Widget->ReturnToTitleRequestCount, 2);

	// CONTROL: the back button still does its own job, so the two are two wires and not one.
	Widget->Back()->OnClicked.Broadcast();
	TestEqual(TEXT("CONTROL: Back still dismisses"), Widget->DismissCount, 1);
	TestEqual(TEXT("and Back does not ask to leave the match"),
		Widget->ReturnToTitleRequestCount, 2);

	// ---- THE UNBIND ------------------------------------------------------------------------
	Widget->RunNativeDestruct();
	const int32 AfterDestruct = Widget->ReturnToTitleRequestCount;

	Widget->Exit()->OnClicked.Broadcast();
	TestEqual(TEXT("after NativeDestruct the exit button no longer reaches the widget, so a "
		"stale click cannot travel a screen that is gone"),
		Widget->ReturnToTitleRequestCount, AfterDestruct);

	return true;
}
