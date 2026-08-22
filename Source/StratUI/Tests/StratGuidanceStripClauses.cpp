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
#include "UObject/UObjectGlobals.h"

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
// ===========================================================================
// The two facts that live in .uasset bytes and in nothing else.
//
// Everything above this line is a ruling about C++ reflection data, which a compiler and a
// diff can both see. The two clauses below are about SHIPPED ASSETS, and an asset regresses
// silently: nothing fails to compile when a Blueprint's parent is re-pointed or a class
// default is cleared to None, and a `None` on exactly this HUD property already went
// unnoticed for a whole phase. That is the entire reason these exist.
//
// LOADING A `/Game/` PATH HERE IS THE PERMITTED EXCEPTION, not an oversight of CLAUDE.md's
// no-path-literals rule: automation-test fixtures and the import commandlet are its two
// standing exceptions, and `Source/StratBridge/Tests/StratBridgeParity.cpp` reads DT_Units and
// DT_Terrain by path for the same reason. The rule exists so that GAMEPLAY code cannot hard-
// wire an asset; a clause whose entire subject IS the shipped asset has to name it.
//
// WHERE THE EXPECTATIONS COME FROM. Both expectations are `UStratGuidanceWidget::StaticClass()`
// -- the very C++ class the assets claim kinship with, read from reflection, never a typed
// class-name string. The only literals are the two asset PATHS, which are subjects and not
// expectations, and which were verified against the tree (`Content/UI/`) rather than assumed.
//
// EVERY FAILURE PATH IS RED, INCLUDING "COULD NOT LOAD". A clause about a shipped asset that
// cannot find that asset has observed nothing, and reporting "could not tell" as "fine" hides
// exactly the state it is supposed to catch. If these ever go red because the automation
// environment cannot load a `/Game/` package at all, that is a real finding about the harness
// and must be reported as one -- not softened into a skip.
// ===========================================================================
namespace StratGuidanceStripAssets
{
	/** The strip Widget Blueprint's GENERATED class, as against the `UWidgetBlueprint` asset
	 *  object that carries the editor-only designer graph. The `_C` object is what a
	 *  `TSubclassOf` reference resolves and what `CreateWidget` instantiates, so it is the
	 *  only object that can answer what the shipped strip actually derives from. */
	const TCHAR* const kDirectiveStripClassPath =
		TEXT("/Game/UI/WBP_DirectiveStrip.WBP_DirectiveStrip_C");

	/** The shipped HUD Blueprint's generated class, for the same reason. Its CDO is the object
	 *  a spawned HUD copies its class defaults from. */
	const TCHAR* const kScoreboardHudClassPath =
		TEXT("/Game/UI/BP_StratScoreboardHUD.BP_StratScoreboardHUD_C");

	/**
	 * Loads a generated Blueprint class with NO base-class filter.
	 *
	 * `UObject::StaticClass()` deliberately, and not the class the caller is about to assert
	 * kinship with. `StaticLoadClass` returns null when the loaded class fails the filter, so
	 * filtering on `UStratGuidanceWidget` would turn "the asset's parent was re-pointed" --
	 * the defect these clauses exist to catch -- into "the asset did not load", and the
	 * failure message would blame the wrong thing entirely.
	 */
	static UClass* LoadGeneratedClass(const TCHAR* ObjectPath)
	{
		return StaticLoadClass(UObject::StaticClass(), nullptr, ObjectPath);
	}
}

// ---------------------------------------------------------------------------
// T-UI-02 -- the shipped strip asset IS a guidance widget.
//
// `AStratScoreboardHUD::GuidanceWidgetClass` is a `TSubclassOf<UStratGuidanceWidget>`, so the
// EDITOR will refuse to assign an unrelated widget to it. What the editor does not hold is the
// other end: WBP_DirectiveStrip's own parent class. Re-parenting that asset to a plain
// `UUserWidget` compiles, saves and cooks without complaint -- and then the HUD property that
// pointed at it silently holds nothing assignable, `PushGuidance` reaches no strip, and the
// guided opening advances beats against a screen that never draws them.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratDirectiveStripAssetIsAGuidanceWidgetTest,
	"Stratocracy.StratUI.T-UI-02.TheDirectiveStripAssetIsAGuidanceWidget",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratDirectiveStripAssetIsAGuidanceWidgetTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceStripAssets;

	UClass* const StripClass = LoadGeneratedClass(kDirectiveStripClassPath);
	if (!TestNotNull(
			*FString::Printf(
				TEXT("the shipped strip Widget Blueprint's generated class loads from '%s'. If "
				     "this is red the ASSET Content/UI/WBP_DirectiveStrip.uasset is missing, "
				     "renamed or moved -- this clause can conclude nothing about its parent, so "
				     "it fails rather than passing. Fix the asset or fix this path; do not "
				     "soften this into a skip."),
				kDirectiveStripClassPath),
			StripClass))
	{
		return false;
	}

	// The subject must be the ASSET's class, not a native one. If the path ever resolved to
	// C++, the kinship assertion below could pass while testing no asset at all.
	if (!TestTrue(
			*FString::Printf(
				TEXT("the class resolved from '%s' is Blueprint-generated and not native C++ "
				     "(read: '%s'), so this clause's subject really is the shipped asset"),
				kDirectiveStripClassPath, *StripClass->GetName()),
			!StripClass->HasAnyClassFlags(CLASS_Native)))
	{
		return false;
	}

	TestTrue(
		*FString::Printf(
			TEXT("ASSET Content/UI/WBP_DirectiveStrip.uasset: its generated class '%s' has "
			     "UStratGuidanceWidget as an ancestor (its immediate parent reads '%s'). If "
			     "this is red the Widget Blueprint has been re-parented, and nothing else in "
			     "the build says so: AStratScoreboardHUD::GuidanceWidgetClass can no longer "
			     "hold it, PushGuidance reaches no strip, and Sec 2.11.6's directive never "
			     "draws."),
			*StripClass->GetName(),
			StripClass->GetSuperClass() != nullptr
				? *StripClass->GetSuperClass()->GetName()
				: TEXT("<none>")),
		StripClass->IsChildOf(UStratGuidanceWidget::StaticClass()));

	return true;
}

// ---------------------------------------------------------------------------
// T-UI-02 -- the shipped HUD actually NAMES a strip class.
//
// `AStratScoreboardHUD::CreateGuidanceWidget` treats an unset `GuidanceWidgetClass` as a
// legitimate configuration and logs it at Log verbosity, by design: a build that ships without
// a guided opening is a real build, and a bool refusal there would have meant "configured as
// intended" on every reconcile of every such session. The consequence is that a `None` on the
// SHIPPED HUD produces no warning, no error and no failing test -- which is how one went
// unnoticed for a phase. This clause is the thing that would have been red.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShippedHudNamesAGuidanceWidgetClassTest,
	"Stratocracy.StratUI.T-UI-02.TheShippedHudNamesAGuidanceWidgetClass",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShippedHudNamesAGuidanceWidgetClassTest::RunTest(const FString& /*Parameters*/)
{
	using namespace StratGuidanceStripAssets;

	// Read through reflection rather than through the member, which is `protected` -- and which
	// also puts the property NAME into every message below, so a failure says which property
	// regressed and not merely which asset.
	FClassProperty* const ClassProperty = FindFProperty<FClassProperty>(
		AStratScoreboardHUD::StaticClass(), FName(TEXT("GuidanceWidgetClass")));
	if (!TestNotNull(
			TEXT("AStratScoreboardHUD::GuidanceWidgetClass is a reflected class property -- "
			     "without it there is no class default for this clause to read"),
			ClassProperty))
	{
		return false;
	}

	// ---- PREMISE: the C++ default is null, so a non-null read below is the ASSET's ---------
	//
	// Fatal, and for the same reason T-UI-03.TheShippedGameModeOptsIn checks its own premise:
	// if the native default ever became non-null, this clause would pass on an asset that
	// overrode nothing while still wearing the name "TheShippedHudNamesAGuidanceWidgetClass".
	{
		const UObject* const NativeCdo = AStratScoreboardHUD::StaticClass()->GetDefaultObject();
		if (!TestNotNull(TEXT("the native AStratScoreboardHUD CDO exists"), NativeCdo))
		{
			return false;
		}
		if (!TestNull(
				TEXT("PREMISE: AStratScoreboardHUD::GuidanceWidgetClass is UNSET in C++. This "
				     "clause exists to prove BP_StratScoreboardHUD supplies it; if the native "
				     "default is non-null the clause can no longer tell an asset default from "
				     "an inherited one and is not a gate on the asset at all."),
				ClassProperty->GetObjectPropertyValue_InContainer(NativeCdo)))
		{
			return false;
		}
	}

	UClass* const HudClass = LoadGeneratedClass(kScoreboardHudClassPath);
	if (!TestNotNull(
			*FString::Printf(
				TEXT("the shipped HUD Blueprint's generated class loads from '%s'. If this is "
				     "red the ASSET Content/UI/BP_StratScoreboardHUD.uasset is missing, renamed "
				     "or moved, and this clause fails rather than reporting 'could not tell' as "
				     "'fine'."),
				kScoreboardHudClassPath),
			HudClass))
	{
		return false;
	}

	if (!TestTrue(
			*FString::Printf(
				TEXT("the class resolved from '%s' is Blueprint-generated and not native C++ "
				     "(read: '%s'). A native class here would silently turn this into a read of "
				     "the member initialiser instead of the asset's class default."),
				kScoreboardHudClassPath, *HudClass->GetName()),
			!HudClass->HasAnyClassFlags(CLASS_Native)))
	{
		return false;
	}

	if (!TestTrue(
			*FString::Printf(
				TEXT("the resolved class derives from AStratScoreboardHUD (read: '%s'), so its "
				     "CDO actually carries a GuidanceWidgetClass"),
				*HudClass->GetName()),
			HudClass->IsChildOf(AStratScoreboardHUD::StaticClass())))
	{
		return false;
	}

	const UObject* const Cdo = HudClass->GetDefaultObject();
	if (!TestNotNull(
			TEXT("the shipped HUD Blueprint's class default object exists -- this is the object "
			     "a spawned HUD copies its class defaults from"),
			Cdo))
	{
		return false;
	}

	// ---- The clause, in two halves that fail for different reasons ------------------------
	UObject* const DefaultValue = ClassProperty->GetObjectPropertyValue_InContainer(Cdo);
	if (!TestNotNull(
			TEXT("ASSET Content/UI/BP_StratScoreboardHUD.uasset, PROPERTY GuidanceWidgetClass: "
			     "its class default is NOT None. A None here draws no directive strip at all "
			     "and produces no warning -- CreateGuidanceWidget treats an unset class as a "
			     "legitimate configuration and logs it at Log verbosity -- so nothing but this "
			     "clause would report it."),
			DefaultValue))
	{
		return false;
	}

	UClass* const StripClass = Cast<UClass>(DefaultValue);
	if (!TestNotNull(
			*FString::Printf(
				TEXT("ASSET Content/UI/BP_StratScoreboardHUD.uasset, PROPERTY "
				     "GuidanceWidgetClass: its class default reads as a UClass (read: '%s')"),
				*DefaultValue->GetName()),
			StripClass))
	{
		return false;
	}

	TestTrue(
		*FString::Printf(
			TEXT("ASSET Content/UI/BP_StratScoreboardHUD.uasset, PROPERTY GuidanceWidgetClass: "
			     "its class default '%s' derives from UStratGuidanceWidget, so the HUD can "
			     "actually push a directive into what it creates. If this is red the property "
			     "names a widget class that is not a strip."),
			*StripClass->GetName()),
		StripClass->IsChildOf(UStratGuidanceWidget::StaticClass()));

	return true;
}
