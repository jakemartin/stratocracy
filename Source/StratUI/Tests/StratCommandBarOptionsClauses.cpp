// `UStratCommandBarWidget::OptionsButton` -- the second entry point onto the `Options` route,
// under `GATE-TITLEMENU`.
//
// THE ID IS RULED AND NOT CHOSEN HERE, AND THE OBVIOUS ALTERNATIVE WAS REFUSED ON EVIDENCE.
// `Tools/architect/state/global.md`'s third acceptance-ID ruling of 2026-09-05 covers this
// subject: *"The button's whole job, by its own header, is reaching the `Options` route from a
// second surface -- a match, rather than the title screen -- and `GATE-TITLEMENU`'s subject is
// the ROUTE, not the screen that requests it."* Two names were refused there:
//   - `T-UI-05`, which the engineer proposed as `T-UI-05.CommandBarOptions*`. That ID's subject
//     is snapshot fidelity for the `+X/turn` INCOME LINE (`decisions.md:849`). A route-request
//     button is not an income projection.
//   - `T-UI-03`, which governs this very file's SIBLING clauses in `StratCommandBarClauses.cpp`
//     and does so because those check a widget-DRAWN value against a `strat::UiSnapshot` field.
//     `OptionsButton` draws no value and reads no snapshot field. Sitting on the same command
//     bar does not pull it onto that ID; the test is the subject sentence, not the file.
//
// ---------------------------------------------------------------------------------------
// WHY A SEPARATE FILE FROM `StratCommandBarClauses.cpp`. That file's header is a careful account
// of a decorator-and-model surface where the class decides nothing and draws from a snapshot.
// These two clauses are about a native BIND and a BROADCAST, which is the one place this class
// departs from that account -- its own header calls the departure out. Putting them there would
// have made that header wrong by addition, which is `StratShellOptionsRouteClauses.cpp`'s stated
// reason for existing as a separate file too.
//
// WHERE THE EXPECTATIONS COME FROM. There is no module-side count of "how many times a click
// should broadcast" to read, because the property IS the count: one click, one broadcast. The
// numbers below are therefore the property itself and not a derived expectation, and every one
// of them is asserted as a DELTA on an observer whose zero was checked first. Nothing here
// computes what the bar should draw, and nothing here reads a snapshot.
//
// WHAT THESE CLAUSES DO **NOT** PIN.
//   - THAT ANYTHING HAPPENS AS A RESULT. `OnOptionsRequested` reaching
//     `UStratMatchSubsystem::HandleCommandBarOptionsRequested` and from there the shell's
//     `Options` route is `StratCommandBarOptionsBindClauses.cpp`'s subject, in StratPlay, which
//     is the only module that can see both ends.
//   - THAT `WBP_CommandBar` CARRIES A CHILD NAMED `OptionsButton`. `BindWidgetOptional` does
//     not enforce it -- deliberately, per the member's own block -- so that is an asset fact
//     and a different instrument.
//   - THAT THE BUTTON IS VISIBLE OR HIT-TESTABLE. No clause in this suite observes Slate
//     geometry; a human at the keyboard remains the only instrument for that.
// ---------------------------------------------------------------------------------------

#include "Misc/AutomationTest.h"

#include "Components/Button.h"
#include "Containers/UnrealString.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "StratCommandBarOptionsDouble.h"

// ---------------------------------------------------------------------------------------
// (1) One click, one broadcast -- and the bind comes back off.
//
// THE DEFECT THIS IS FOR: an unguarded re-bind. `AddDynamic` on a dynamic multicast does NOT
// deduplicate, so a `NativeConstruct` that ran twice -- which a widget re-added to the viewport
// does -- would broadcast twice per click and open the options route twice. The count is
// therefore asserted as EXACTLY one and not as "at least one".
//
// THE SECOND CLICK IS THE POSITIVE CONTROL AND IS NOT DECORATION: a handler that latched after
// its first delivery passes the first delta perfectly and leaves the button dead from the
// player's second press onward.
//
// MUTANTS:
//   - `UStratCommandBarWidget::HandleOptionsClicked` bodied as `{}` -- red on the first delta.
//   - `NativeConstruct` calling `OptionsButton->OnClicked.AddDynamic(...)` twice -- red on
//     "exactly one", with the message printing 2.
//   - `NativeDestruct`'s `RemoveDynamic` deleted -- red on the last assertion.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCommandBarOptionsClickBroadcastsOnceTest,
	"Stratocracy.StratUI.GATE-TITLEMENU.TheCommandBarOptionsButtonBroadcastsExactlyOncePerClick",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCommandBarOptionsClickBroadcastsOnceTest::RunTest(const FString& /*Parameters*/)
{
	const TStrongObjectPtr<UStratCommandBarOptionsDouble> Bar(
		NewObject<UStratCommandBarOptionsDouble>());
	if (!TestTrue(TEXT("a UStratCommandBarOptionsDouble was constructed"), Bar.IsValid()))
	{
		return false;
	}

	Bar->PlantOptionsButton();
	if (!TestNotNull(TEXT("FIXTURE: the OptionsButton plant took. A null plant makes the bind "
			"inside NativeConstruct a no-op and nothing below would have been observed"),
			Bar->Button()))
	{
		return false;
	}

	Bar->OnOptionsRequested.AddDynamic(
		Bar.Get(), &UStratCommandBarOptionsDouble::HandleRequested);

	// PREMISE: the button reaches nothing YET. This is what makes the first delta below a
	// measurement of `NativeConstruct` rather than of the plant.
	Bar->Button()->OnClicked.Broadcast();
	if (!TestEqual(TEXT("premise: before NativeConstruct the planted button is unwired, so a "
			"click reaches nothing. If this is non-zero the bind is happening somewhere this "
			"clause is not looking and the measurement below is not the one it claims"),
			Bar->RequestCount, 0))
	{
		return false;
	}

	// ---- THE SHIPPED BIND ------------------------------------------------------------------
	Bar->RunNativeConstruct();

	const int32 Before = Bar->RequestCount;
	Bar->Button()->OnClicked.Broadcast();

	TestEqual(TEXT("one click on the command bar's Options button broadcasts OnOptionsRequested "
		"exactly once. Two here means an unguarded re-bind and one press opening the options "
		"route twice"), Bar->RequestCount - Before, 1);

	// ---- THE SECOND CLICK ------------------------------------------------------------------
	const int32 Between = Bar->RequestCount;
	Bar->Button()->OnClicked.Broadcast();
	TestEqual(TEXT("and the second click broadcasts once too -- the handler is not a latch"),
		Bar->RequestCount - Between, 1);

	// ---- THE UNBIND ------------------------------------------------------------------------
	Bar->RunNativeDestruct();
	const int32 AfterDestruct = Bar->RequestCount;

	Bar->Button()->OnClicked.Broadcast();
	TestEqual(TEXT("after NativeDestruct the button no longer reaches the bar, so a stale click "
		"on a torn-down HUD cannot request the options route"),
		Bar->RequestCount, AfterDestruct);

	return true;
}

// ---------------------------------------------------------------------------------------
// (2) A command bar with no Options button constructs, and broadcasts nothing.
//
// THIS IS THE SHIPPED CONFIGURATION AND NOT AN EDGE CASE. `OptionsButton` is
// `BindWidgetOptional`, which its own block argues for explicitly: a `WBP_CommandBar` that has
// not yet acquired the child must still COMPILE and must still run. So the null path is a
// supported state of the shipped asset, and the thing that would break it -- an unguarded
// `OptionsButton->OnClicked.AddDynamic` -- is a null dereference inside `NativeConstruct`, which
// takes the whole HUD down rather than merely losing a button.
//
// A CRASH IS NOT THE ONLY FAILURE THIS LOOKS FOR, AND THAT IS DELIBERATE. This clause reports a
// clean run through `NativeConstruct` AND `NativeDestruct` with a zero broadcast count, so a
// body that survived the null but wired something else -- or that broadcast on construction --
// is red rather than silently green. A clause shaped only around the anticipated crash would
// not have seen either.
//
// MUTANT: drop the `if (OptionsButton != nullptr)` guard in `NativeConstruct`.
//
// AND THE MUTANT ERASES ITS OWN EVIDENCE, WHICH IS AN INSTRUMENT CAVEAT AND NOT A DEFECT IN THE
// CLAUSE. Measured 2026-09-05 in the mutant battery: the null dereference takes the PROCESS down
// with an access violation, so `UnrealEditor-Cmd.exe` never reaches its export step and
// `Saved/AutomationReport/index.json` IS NOT WRITTEN AT ALL. A reader who then opens that file
// -- the instrument this project treats as authoritative for `succeeded`/`failed`/`notRun` --
// reads THE PREVIOUS RUN'S numbers and sees `failed: 0`. A crashed mutant and a killed clause
// are therefore INDISTINGUISHABLE BY THE COUNT ALONE, and the count is the wrong instrument for
// any crashing mutant.
//
// HOW TO TELL THEM APART, since this clause cannot do it from inside itself -- a clause cannot
// report on a process that died before it could report:
//   1. Check `index.json`'s OWN `reportCreatedOn` against the run you just started. If the
//      timestamp did not move, the run produced no report and every figure in it is stale. This
//      is the check that generalises; it is the same family as this project's existing "exit
//      code is not a verdict" and "the log undercounts by exactly one".
//   2. The engine log DOES name the currently-running test alongside the access violation, so
//      the log distinguishes them even when the report cannot. Note that this line previously
//      claimed only that -- "the run reports the access violation against this test's name" --
//      which is true of the LOG and false of the REPORT, and the report is what gets read.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratCommandBarWithNoOptionsButtonConstructsTest,
	"Stratocracy.StratUI.GATE-TITLEMENU.ACommandBarWithNoOptionsButtonConstructsAndBroadcastsNothing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratCommandBarWithNoOptionsButtonConstructsTest::RunTest(const FString& /*Parameters*/)
{
	const TStrongObjectPtr<UStratCommandBarOptionsDouble> Bar(
		NewObject<UStratCommandBarOptionsDouble>());
	if (!TestTrue(TEXT("a UStratCommandBarOptionsDouble was constructed"), Bar.IsValid()))
	{
		return false;
	}

	// NOTHING IS PLANTED. This is the state of a native subclass and of a shipped WBP whose
	// designer has not added the child yet.
	if (!TestNull(TEXT("premise: OptionsButton is null, which is the configuration under test"),
			Bar->Button()))
	{
		return false;
	}

	Bar->OnOptionsRequested.AddDynamic(
		Bar.Get(), &UStratCommandBarOptionsDouble::HandleRequested);

	Bar->RunNativeConstruct();
	Bar->RunNativeDestruct();

	TestEqual(TEXT("a command bar with no Options button ran NativeConstruct and NativeDestruct "
		"and broadcast nothing. Reaching this line at all is half the clause -- an unguarded "
		"bind is an access violation here, not a failed assertion"), Bar->RequestCount, 0);

	return true;
}
