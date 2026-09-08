// THE ONE FACT ABOUT THE EXIT ROW THAT LIVES IN `.uasset` BYTES AND IN NOTHING ELSE.
//
// `Source/StratUI/Tests/StratOptionsExitClauses.cpp` pins what `UStratOptionsWidget` DOES with an
// exit control, against a planted double. It loads no package and it says so in terms. This file
// is the other half and its subject is the SHIPPED ASSET: that `Content/UI/WBP_Options.uasset`
// carries a child the C++ member `ReturnToTitleButton` can actually bind to.
//
// ---------------------------------------------------------------------------------------
// WHY IT EXISTS, QUOTED FROM THE THING THAT ASKED FOR IT. `StratOptionsWidget.h` declares
// `ReturnToTitleButton` as `BindWidgetOptional` and names its own discharge condition:
// *"DISCHARGED BY a clause that reads the shipped `WBP_Options` CDO's `ReturnToTitleButton`,
// after which this may become `BindWidget` in a pass that touches nothing else. The clause is
// the test lane's"*. The same block records the condition as HALF met -- the asset half -- and
// names precisely what the missing half is worth: *"a name present in a `.uasset`'s bytes is a
// NAME, not a `UButton` at a known place in a compiled widget tree, and it cannot distinguish a
// live control from a stale entry."* That sentence is this clause's whole specification.
//
// **WRITING THIS CLAUSE DOES NOT PROMOTE ANYTHING.** `ReturnToTitleButton` is still
// `BindWidgetOptional` and `UStratCommandBarWidget::OptionsButton` still is too. The promotion is
// a C++ edit in `strat-gameplay-engineer`'s lane, and that lane declined it on this pass on the
// stated ground that a hard bind reddening a whole screen should rest on a clause. The clause now
// exists; the decision to spend it remains that lane's.
//
// ---------------------------------------------------------------------------------------
// THE ACCEPTANCE ID. `GATE-TITLEMENU`, for the reason `StratOptionsExitClauses.cpp` sets out at
// length and does not need repeating: `global.md`'s third acceptance-ID ruling of 2026-09-05
// places a control whose whole job is reaching an `EStratShellRoute` arm under `GATE-TITLEMENU`
// because *"`GATE-TITLEMENU`'s subject is the ROUTE, not the screen that requests it"*, and
// refuses `T-UI-03` for any surface with no `strat::UiSnapshot` field behind it. Nothing is
// minted. The name says `ShippedOptionsWidgetCarriesReturnToTitleButton` and the clause asserts
// exactly that and nothing wider -- see WHAT IT DOES NOT PIN.
//
// LOADING A `/Game/` PATH IS THE PERMITTED EXCEPTION, on `StratGuidanceStripClauses.cpp`'s
// stated reading of it: `CLAUDE.md` names automation-test fixtures and the import commandlet as
// the two standing exceptions to the no-path-literals rule, which exists so that GAMEPLAY code
// cannot hard-wire an asset. A clause whose entire subject IS the shipped asset has to name it.
//
// ---------------------------------------------------------------------------------------
// WHERE THE EXPECTATIONS COME FROM, AND THIS IS THE PART WORTH READING.
//   - THE NAME SEARCHED FOR IN THE WIDGET TREE IS THE C++ MEMBER'S OWN `FName`, taken from
//     `UStratOptionsWidget`'s reflection data (`FProperty::GetFName()`), NOT a string literal
//     retyped here. A `BindWidget` contract IS a name match, so the name is the whole fact; a
//     literal on both sides would let a member rename go green while the binding silently broke.
//   - THE WIDGET CLASS EXPECTED IS THE MEMBER'S OWN DECLARED `PropertyClass`
//     (`FObjectPropertyBase::PropertyClass`), NOT `UButton::StaticClass()` written out here. If
//     the member's type ever changes, the expectation moves with it, which is the point.
//   - THE ONLY LITERALS ARE THE ASSET PATH AND THE TWO PROPERTY LOOKUP KEYS. Those are SUBJECTS
//     -- what this clause is about -- and never the expectation side of a comparison. A wrong
//     lookup key is red (`FindPropertyByName` returns null and the clause fails saying so), not
//     silently green.
//
// EVERY FAILURE PATH IS RED, INCLUDING "COULD NOT LOAD", for `StratGuidanceStripClauses.cpp`'s
// reason: a clause about a shipped asset that cannot find that asset has observed nothing, and
// reporting "could not tell" as "fine" hides exactly the state it exists to catch. If this ever
// reds because the automation environment cannot load a `/Game/` package at all, that is a real
// finding about the harness and must be reported as one, not softened into a skip.
//
// ---------------------------------------------------------------------------------------
// WHY IT READS THE WIDGET TREE ARCHETYPE AND NOT THE CDO'S POINTER, SAID RATHER THAN LEFT TO BE
// DISCOVERED -- THE DISCHARGE CONDITION SAYS "CDO" AND THE LITERAL READING OF IT IS VACUOUS.
// `BindWidget` members are assigned in `UUserWidget::Initialize`, on a CONSTRUCTED instance, from
// a duplicate of the class's widget tree. The class default object is never initialised that way,
// so `CDO->ReturnToTitleButton` is null on a perfectly correct asset and a clause asserting it
// non-null would be red forever, while one asserting it null would pass with the control deleted.
// The object that can answer is `UWidgetBlueprintGeneratedClass`'s widget tree ARCHETYPE, which
// is what `Initialize` binds from. Step (5) below MEASURES the CDO pointer being null rather than
// asserting that reasoning from memory -- if it ever stops being null, this file's choice of
// instrument is wrong and should red until someone re-reads it.
//
// WHAT IT DOES **NOT** PIN.
//   - `ReturnToTitleReasonText`. Deliberately out of scope: the clause name says
//     `ReturnToTitleButton`, and a name is read without its header. That member is
//     `BindWidgetOptional` on the ordinary reading -- it only displays a value -- and no C++
//     depends on its presence. A separate clause could pin it; this one must not, because an
//     over-broad clause under a narrow name misreports what went wrong when it reds.
//   - `ReturnToTitleLabel`. Present in the asset's bytes; not a C++ member of any class here, so
//     there is no module-side value to compare it against and nothing to say about it.
//   - THAT THE CONTROL IS VISIBLE, LAID OUT, PARENTED SOMEWHERE SENSIBLE, OR CLICKABLE. A widget
//     tree entry is not geometry. No clause in this suite can observe Slate layout; a human at
//     the keyboard remains the only instrument for that.
//   - THAT ANY BINDING ACTUALLY HAPPENS AT RUNTIME. This reads the archetype `Initialize` binds
//     FROM. `Initialize` itself needs a game world and a player and is not driven here.
//   - THAT THE BUTTON IS WIRED TO ANYTHING IN THE BLUEPRINT GRAPH. It does not need to be:
//     `UStratOptionsWidget::NativeConstruct` binds `OnClicked` in C++, which
//     `StratOptionsExitClauses.cpp` clause (4) pins.
// ---------------------------------------------------------------------------------------

#include "Misc/AutomationTest.h"

#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Widget.h"
#include "Containers/UnrealString.h"
#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/UnrealType.h"

#include "StratOptionsWidget.h"

namespace StratShippedOptionsExitControl
{
	/**
	 * The options Widget Blueprint's GENERATED class.
	 *
	 * The `_C` object and not the `UWidgetBlueprint` asset object, on
	 * `StratGuidanceStripClauses.cpp`'s stated ground: the generated class is what a
	 * `TSubclassOf` resolves and what `CreateWidget` instantiates, so it is the only object that
	 * can answer what the shipped screen actually contains.
	 */
	const TCHAR* const kOptionsClassPath = TEXT("/Game/UI/WBP_Options.WBP_Options_C");

	/** The C++ member whose `BindWidgetOptional` this clause is the discharge condition for. */
	const TCHAR* const kSubjectMember = TEXT("ReturnToTitleButton");

	/**
	 * A `BindWidget` -- NOT optional -- member of the same class and the same declared type.
	 *
	 * THE POSITIVE CONTROL, and the clause is worth very little without it. A widget-tree lookup
	 * that returns null for EVERYTHING -- wrong archetype, stripped tree, a `FindWidget` that
	 * cannot see this depth -- would fail the subject assertion and read as "the editor lane
	 * never added the button", which is a true-looking report of a false thing. `BackButton` is
	 * a hard bind, so `WBP_Options` cannot compile without it; if the instrument cannot find
	 * THAT, the instrument is broken and the clause says so instead.
	 */
	const TCHAR* const kControlMember = TEXT("BackButton");

	/** A name no widget on this screen has. The negative control on `FindWidget` itself. */
	const TCHAR* const kAbsentWidgetName = TEXT("ZzzNoSuchWidgetOnTheOptionsScreen");
}

// ---------------------------------------------------------------------------------------
// GATE-TITLEMENU -- the shipped options screen carries the exit control.
//
// THE DEFECT THIS IS FOR: the asset regressing silently. A Widget Blueprint child can be renamed,
// deleted or re-typed without anything failing to compile in C++, because the member is
// `BindWidgetOptional` -- which is exactly what "optional" buys and exactly what it costs. A
// player would then open Options in a live match and find no way out of it, and every one of the
// seven exit clauses in this tree would stay green, because all seven run against a plant.
//
// MUTANTS -- **ONE OF THE FOUR BELOW HAS BEEN RUN, AND IT KILLED THIS CLAUSE.**
//
// [CORRECTED 2026-09-07, over base commit `525ad5c`. THE BLOCK THAT STOOD HERE OPENED
// *"MUTANTS -- NONE OF THE FOUR BELOW HAS BEEN RUN ... they are PREDICTIONS, not measurements"* and
// added *"No mutant had been run against this clause as of the exported suite report
// `reportCreatedOn 2026.09.07-21.25.09`"*. **BOTH SENTENCES WERE TRUE WHEN WRITTEN AND ARE FALSE AS
// OF THE MUTANT RUN OVER BASE `525ad5c`.** They are quoted here rather than deleted so a reader who
// arrived by a citation to them lands on the correction and not on the stale claim.]
//
// WHAT WAS RUN, STATED INLINE RATHER THAN CITED. The run's exported reports are untracked, so no
// checkout has them and a path to one would be unfalsifiable. The asset child
// `ReturnToTitleButton` was RENAMED to `ReturnToTitleButtonMUT` in `/Game/UI/WBP_Options` through
// `rename_widget`; the tree was then rebuilt and the full suite run. **THIS CLAUSE WENT RED, AND
// ALONE** -- no other clause in the suite moved -- and the failure message named the ASSET and the
// consequence rather than the instrument, which is exactly what the two controls below are for. The
// rename was then reverted, the suite re-ran clean, and the asset's `sha256` returned to
// `2bebaccbbd291a0351069a51354983903150631b113c923f4697a28293de4667`, the value the `525ad5c`
// commit message itself quotes. A baseline before the first mutant and a baseline after the last
// revert were both clean, so the red is attributable to the mutant and not to the tree.
//
// MUTANTS -- THE FIRST IS NOW MEASURED; THE OTHER THREE REMAIN PREDICTIONS AND ARE MARKED AS SUCH:
//   - rename the child in `WBP_Options` to anything else -- **MEASURED RED**, alone, on the subject
//     assertion, with both controls green, so the message pointed at the asset and not at the
//     instrument. That is precisely what this line predicted before it was run.
//   - re-type the child from `UButton` to a `UBorder` (a shape a designer reaching for a styled
//     control could plausibly produce) -- **PREDICTED, UNRUN**: red on the TYPE assertion. A byte
//     scan for the name cannot see this mutant at all, which is the gap this clause was asked to
//     close. The rename mutant above does NOT cover it -- it reds the presence assertion and
//     retires before the type assertion is reached.
//   - delete `ReturnToTitleButton` from `UStratOptionsWidget` -- **PREDICTED, UNRUN**: red at step
//     (1), naming the member, rather than reporting a missing asset child.
//   - point `kOptionsClassPath` at a class that does not exist -- **PREDICTED, UNRUN**: red at step
//     (2), by design.
//
// The corresponding correction is recorded in `Tools/architect/state/tests.md`.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShippedOptionsWidgetCarriesReturnToTitleButtonTest,
	"Stratocracy.StratUI.GATE-TITLEMENU.ShippedOptionsWidgetCarriesReturnToTitleButton",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShippedOptionsWidgetCarriesReturnToTitleButtonTest::RunTest(
	const FString& /*Parameters*/)
{
	using namespace StratShippedOptionsExitControl;

	// ---- 1. THE EXPECTATION SIDE, READ OFF THE C++ CLASS ------------------------------------
	// Both the name to look for and the class to require come from the member's own reflection
	// data. Nothing below retypes either.
	const FObjectPropertyBase* const SubjectProperty = CastField<FObjectPropertyBase>(
		UStratOptionsWidget::StaticClass()->FindPropertyByName(FName(kSubjectMember)));
	if (!TestNotNull(*FString::Printf(
			TEXT("UStratOptionsWidget declares an object property named '%s'. If this is red the "
			     "C++ member was renamed or removed and this clause's subject no longer exists -- "
			     "fix the lookup key or retire the clause; do not soften it"), kSubjectMember),
			SubjectProperty))
	{
		return false;
	}

	const FObjectPropertyBase* const ControlProperty = CastField<FObjectPropertyBase>(
		UStratOptionsWidget::StaticClass()->FindPropertyByName(FName(kControlMember)));
	if (!TestNotNull(*FString::Printf(
			TEXT("FIXTURE: UStratOptionsWidget declares the control member '%s'"), kControlMember),
			ControlProperty))
	{
		return false;
	}

	UClass* const ExpectedWidgetClass = SubjectProperty->PropertyClass;
	UClass* const ExpectedControlClass = ControlProperty->PropertyClass;
	if (!TestNotNull(TEXT("FIXTURE: the subject member declares a widget class"),
			ExpectedWidgetClass))
	{
		return false;
	}
	// The control's class is guarded on the same terms as the subject's, for symmetry rather than
	// for a reachable defect: reflection does not produce an object property with a null
	// `PropertyClass`, so neither guard is expected to fire. An unguarded dereference downstream
	// would crash the runner instead of failing the clause, and a crash erases its own evidence.
	if (!TestNotNull(TEXT("FIXTURE: the control member declares a widget class"),
			ExpectedControlClass))
	{
		return false;
	}

	// ---- 2. THE SUBJECT: THE SHIPPED CLASS ---------------------------------------------------
	// No base-class filter, for `StratGuidanceStripClauses.cpp`'s measured reason: filtering on
	// `UStratOptionsWidget` would turn "the asset was re-parented" -- a defect step (3) exists to
	// catch -- into "the asset did not load", and blame the wrong thing.
	UClass* const OptionsClass = StaticLoadClass(UObject::StaticClass(), nullptr, kOptionsClassPath);
	if (!TestNotNull(*FString::Printf(
			TEXT("the shipped options Widget Blueprint's generated class loads from '%s'. If this "
			     "is red the ASSET Content/UI/WBP_Options.uasset is missing, renamed or moved -- "
			     "this clause can conclude nothing about its contents, so it fails rather than "
			     "passing"), kOptionsClassPath),
			OptionsClass))
	{
		return false;
	}

	if (!TestTrue(*FString::Printf(
			TEXT("the class resolved from '%s' is Blueprint-generated and not native C++ (read: "
			     "'%s'), so this clause's subject really is the shipped asset"),
			kOptionsClassPath, *OptionsClass->GetName()),
			!OptionsClass->HasAnyClassFlags(CLASS_Native)))
	{
		return false;
	}

	// ---- 3. IT IS STILL AN OPTIONS SCREEN ----------------------------------------------------
	// A re-parented WBP binds nothing to this C++ class, so every assertion below would be about
	// a widget tree no `UStratOptionsWidget` will ever read.
	if (!TestTrue(*FString::Printf(
			TEXT("ASSET Content/UI/WBP_Options.uasset: its generated class '%s' has "
			     "UStratOptionsWidget as an ancestor (its immediate parent reads '%s'). If this "
			     "is red the Widget Blueprint has been re-parented and no BindWidget member of "
			     "UStratOptionsWidget binds to anything in it"),
			*OptionsClass->GetName(),
			OptionsClass->GetSuperClass() != nullptr
				? *OptionsClass->GetSuperClass()->GetName()
				: TEXT("<none>")),
			OptionsClass->IsChildOf(UStratOptionsWidget::StaticClass())))
	{
		return false;
	}

	// ---- 4. THE WIDGET TREE `Initialize` WOULD BIND FROM -------------------------------------
	UWidgetBlueprintGeneratedClass* const GeneratedClass =
		Cast<UWidgetBlueprintGeneratedClass>(OptionsClass);
	if (!TestNotNull(TEXT("the generated class is a UWidgetBlueprintGeneratedClass, which is the "
			"only object carrying a widget tree"), GeneratedClass))
	{
		return false;
	}

	// `FindWidgetTreeOwningClass` and not the class's own member: a WBP that inherits its layout
	// carries no tree of its own, and reading the null would report "no exit control" about a
	// screen whose exit control is on its parent.
	const UWidgetBlueprintGeneratedClass* const TreeOwner =
		GeneratedClass->FindWidgetTreeOwningClass();
	if (!TestNotNull(TEXT("some class in the shipped hierarchy owns a widget tree"), TreeOwner))
	{
		return false;
	}

	const UWidgetTree* const Tree = TreeOwner->GetWidgetTreeArchetype();
	if (!TestNotNull(TEXT("that class's widget tree archetype is present -- this is the object "
			"UUserWidget::Initialize duplicates and binds every BindWidget member from"), Tree))
	{
		return false;
	}

	// ---- 5. THE INSTRUMENT, MEASURED BEFORE IT IS TRUSTED ------------------------------------
	// (a) POSITIVE CONTROL. A hard-bound member of the same declared type: `WBP_Options` cannot
	//     compile without it, so a null here is a broken instrument and not a missing control.
	UWidget* const ControlWidget = Tree->FindWidget(ControlProperty->GetFName());
	if (!TestNotNull(*FString::Printf(
			TEXT("CONTROL: the widget tree contains '%s', a BindWidget (not optional) member the "
			     "asset cannot compile without. If THIS is null the lookup below can say nothing "
			     "-- the archetype or the search is wrong, and a 'missing exit control' report "
			     "from this clause would be false"), *ControlProperty->GetFName().ToString()),
			ControlWidget))
	{
		return false;
	}
	if (!TestTrue(*FString::Printf(
			TEXT("CONTROL: and it is a '%s', as the C++ member declares (read: '%s')"),
			*ExpectedControlClass->GetName(),
			ControlWidget->GetClass() != nullptr ? *ControlWidget->GetClass()->GetName()
			                                     : TEXT("<none>")),
			ControlWidget->IsA(ExpectedControlClass)))
	{
		return false;
	}

	// (b) NEGATIVE CONTROL. A lookup that answered every name would make step (6) vacuous.
	if (!TestNull(*FString::Printf(
			TEXT("CONTROL: the widget tree does NOT contain '%s', so FindWidget can say no and "
			     "the assertion below is not satisfied by an instrument that answers everything"),
			kAbsentWidgetName),
			Tree->FindWidget(FName(kAbsentWidgetName))))
	{
		return false;
	}

	// (c) AND THE CDO POINTER IS NULL, MEASURED. The discharge condition says "reads the CDO's
	//     ReturnToTitleButton"; read literally that is vacuous, and this is the measurement
	//     rather than the memory. See this file's header. If it ever stops being null, the
	//     instrument choice above wants re-reading -- which is why this reds rather than logs.
	const UObject* const OptionsCDO = OptionsClass->GetDefaultObject();
	if (TestNotNull(TEXT("FIXTURE: the shipped class has a default object"), OptionsCDO))
	{
		TestNull(*FString::Printf(
			TEXT("CONTROL: the CDO's '%s' pointer is NULL on a correct asset, because BindWidget "
			     "members are assigned in UUserWidget::Initialize on a constructed instance and "
			     "never on the class default object. This is why the assertion below reads the "
			     "widget tree archetype instead"), kSubjectMember),
			SubjectProperty->GetObjectPropertyValue_InContainer(OptionsCDO));
	}

	// ---- 6. THE CLAUSE -----------------------------------------------------------------------
	UWidget* const ExitWidget = Tree->FindWidget(SubjectProperty->GetFName());
	if (!TestNotNull(*FString::Printf(
			TEXT("ASSET Content/UI/WBP_Options.uasset: its widget tree contains a child named "
			     "'%s', which is the name UStratOptionsWidget's member binds by. If this is red "
			     "the shipped options screen has NO WAY OUT OF A MATCH: the member is "
			     "BindWidgetOptional, so nothing fails to compile, and every clause in "
			     "StratOptionsExitClauses.cpp stays green because all of them run against a "
			     "planted double"), *SubjectProperty->GetFName().ToString()),
			ExitWidget))
	{
		return false;
	}

	TestTrue(*FString::Printf(
		TEXT("and it is a '%s' -- the class the C++ member itself declares -- rather than merely "
		     "a child of that name (read: '%s'). A name in the asset's bytes cannot tell these "
		     "apart; a re-typed control binds to nothing and the exit row is dead"),
		*ExpectedWidgetClass->GetName(),
		ExitWidget->GetClass() != nullptr ? *ExitWidget->GetClass()->GetName() : TEXT("<none>")),
		ExitWidget->IsA(ExpectedWidgetClass));

	return true;
}
