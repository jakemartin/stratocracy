// GDD Sec 2.11.6 -- the rulings about the guidance strip that only StratUI can see.
//
// T-INT-05, "rebuild the screen from the view model alone". The behavioural half of this
// property lives in `Source/StratPlay/Tests/StratGuidanceRouteClauses.cpp`, because
// `UStratMatchSubsystem` is a StratPlay type; what is left here is the half that turns on
// StratUI's own reflection data, which StratPlay cannot reach at all. `UMG` is a PRIVATE
// dependency of StratUI (measured: `StratUI.Build.cs`), so `UStratGuidanceWidget::
// StaticClass()` is nameable in this module and in no other.
//
// EVERY ASSERTION BELOW IS ABOUT WHAT DOES NOT EXIST, which is why every one of them is
// preceded by a control. `FindFunctionByName` returning null proves nothing until the same
// call is shown finding something; `FindFProperty` likewise. An absence with no control is an
// untested instrument wearing a green tick.
#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"

#include "StratGuidanceWidget.h"
#include "StratScoreboardHUD.h"
#include "StratViewModel.h"

// ---------------------------------------------------------------------------
// Blueprint has no way to WRITE a guidance value.
//
// `UStratGuidanceWidget::PushGuidance` is plain C++ on purpose: every argument it takes is
// reflectable, so nothing FORCES it out of reflection the way `UStratScoreboardWidget::Refresh`
// is forced. Its header states the ruling -- reflecting it "would let a graph push a fabricated
// beat into the strip" -- and until now nothing mechanical held a later author to it. One
// `UFUNCTION()` keyword, added in good faith by somebody wiring a designer button, reopens the
// hole with a green build and a green suite. This clause is the thing that would not be green.
//
// `AStratScoreboardHUD::PushGuidance` carries the same ruling in the same words, so it is
// pinned in the same clause: the two are one decision held in two files, and a report that
// named only one of them would send a reader to fix half of it.
//
// AND THE PROPERTY IS THE OTHER DOOR TO THE SAME ROOM. A `BlueprintReadWrite` `Guidance` would
// let a graph state a directive without any reflected setter existing at all.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratGuidanceHasNoBlueprintWriterTest,
	"Stratocracy.StratUI.T-INT-05.GuidanceHasNoBlueprintWriter",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratGuidanceHasNoBlueprintWriterTest::RunTest(const FString& /*Parameters*/)
{
	UClass* const WidgetClass = UStratGuidanceWidget::StaticClass();
	UClass* const HudClass    = AStratScoreboardHUD::StaticClass();
	if (!TestNotNull(TEXT("UStratGuidanceWidget publishes a class"), WidgetClass) ||
		!TestNotNull(TEXT("AStratScoreboardHUD publishes a class"), HudClass))
	{
		return false;
	}

	// ---- the control: the instrument can find a reflected function ----------
	if (!TestNotNull(
			TEXT("FindFunctionByName finds OnGuidanceRefreshed, so a null answer below means "
			     "'not reflected' rather than 'the lookup does not work'"),
			WidgetClass->FindFunctionByName(TEXT("OnGuidanceRefreshed"))))
	{
		return false;
	}
	if (!TestNotNull(
			TEXT("and it finds RefreshScoreboard on the HUD, the same control for that class"),
			HudClass->FindFunctionByName(TEXT("RefreshScoreboard"))))
	{
		return false;
	}

	// ---- the ruling ---------------------------------------------------------
	TestNull(
		TEXT("UStratGuidanceWidget::PushGuidance is not reflected -- a UFUNCTION here would let a "
		     "Blueprint graph state a guidance value, making the strip a thing two authors write"),
		WidgetClass->FindFunctionByName(TEXT("PushGuidance")));

	TestNull(
		TEXT("AStratScoreboardHUD::PushGuidance is not reflected either, for the same reason"),
		HudClass->FindFunctionByName(TEXT("PushGuidance")));

	// ---- the other door: a writable property -------------------------------
	FStructProperty* const Held =
		FindFProperty<FStructProperty>(WidgetClass, FName(TEXT("Guidance")));
	if (!TestNotNull(
			TEXT("UStratGuidanceWidget::Guidance IS reflected, so a WBP can bind to it -- this is "
			     "the control for the flag assertions below"),
			Held))
	{
		return false;
	}

	TestTrue(
		TEXT("and it is exactly FStratGuidanceView -- the type and name WBP_DirectiveStrip's "
		     "existing bindings re-resolve against when it is reparented"),
		Held->Struct == FStratGuidanceView::StaticStruct());

	TestTrue(TEXT("it is visible to Blueprint"),
		Held->HasAnyPropertyFlags(CPF_BlueprintVisible));
	TestTrue(
		TEXT("and READ-ONLY to Blueprint -- a writable property would be the reflected-setter "
		     "hole arrived at from the other direction"),
		Held->HasAnyPropertyFlags(CPF_BlueprintReadOnly));

	// The strip's owner is likewise something a graph can read and not replace.
	FObjectPropertyBase* const StripProperty =
		FindFProperty<FObjectPropertyBase>(HudClass, FName(TEXT("GuidanceStrip")));
	if (TestNotNull(TEXT("AStratScoreboardHUD::GuidanceStrip is reflected"), StripProperty))
	{
		TestTrue(TEXT("and read-only to Blueprint, so a graph cannot substitute a second strip"),
			StripProperty->HasAnyPropertyFlags(CPF_BlueprintReadOnly));
	}

	// The asset reference that instantiates the strip is a Blueprint DEFAULT, never a /Game/
	// path literal in C++, and never something a graph edits at runtime.
	FClassProperty* const ClassProperty =
		FindFProperty<FClassProperty>(HudClass, FName(TEXT("GuidanceWidgetClass")));
	if (TestNotNull(TEXT("AStratScoreboardHUD::GuidanceWidgetClass is reflected"), ClassProperty))
	{
		TestTrue(TEXT("it is an EditDefaultsOnly asset reference"),
			ClassProperty->HasAnyPropertyFlags(CPF_Edit) &&
			ClassProperty->HasAnyPropertyFlags(CPF_DisableEditOnInstance));
		TestFalse(TEXT("and it is not writable from a Blueprint graph"),
			ClassProperty->HasAnyPropertyFlags(CPF_BlueprintVisible) &&
			!ClassProperty->HasAnyPropertyFlags(CPF_BlueprintReadOnly));
	}

	return true;
}
