// THE FOUR CAPTIONS ON THE OPTIONS SCREEN THAT NO `BindWidget` MEMBER REACHES, AND THE REGRESSION
// THAT SHIPPED THROUGH EVERY INSTRUMENT IN THIS PROJECT BECAUSE OF IT.
//
// ---------------------------------------------------------------------------------------
// WHY THIS EXISTS. `BackLabel` -- the caption inside `WBP_Options`'s back button -- was deleted by
// an asset pass and SHIPPED that way. The user found it by playing the game. Nothing else could,
// and the four reasons are worth stating because each one is a class of blindness this clause is
// answering, not a mistake anybody made:
//
//   - THE SUITE COULD NOT MOVE. `git show 46321a6:` carries no occurrence of `BackLabel`,
//     `LabelMaster`, `LabelSfx` or `LabelMusic` anywhere under `Source/` -- no `BindWidget`, no
//     clause, no fixture. A count cannot fall when nothing was counting.
//   - `Stratocracy.StratUI.GATE-TITLEMENU.ShippedOptionsWidgetCarriesReturnToTitleButton` PASSED,
//     truthfully. Its positive control is `BackButton`, and the BUTTON survived; only its child
//     died. **A control on a parent proves nothing about that parent's children**, which is why
//     the positive control below is `MasterSlider` and not `BackButton`.
//   - THE ASSET LANE'S BYTE SCAN PASSED, truthfully. It searched for the three names it was
//     ADDING, against invented negative controls, and never asked whether anything had GONE.
//   - THE WIDGET TOTAL WENT 16 -> 18 while the pass reported "16 -> 19", because an addition and
//     a deletion cancel in a total.
//
// The mechanism is recorded in `Tools/architect/state/content.md`: `duplicate_widget` cloned a
// subtree without renaming the clone's child, so two widgets were briefly named `BackLabel`, and
// the back-out used `remove_widget` BY NAME -- which cannot be aimed when two widgets share a
// name, and took the original.
//
// ---------------------------------------------------------------------------------------
// THE HONEST WEAKENING, SAID AT THE SITE RATHER THAN BURIED. THIS PROJECT'S STANDING RULE IS
// NEVER TO COMPUTE OR RETYPE AN EXPECTATION WHEN A MODULE-SIDE VALUE EXISTS TO READ. **For these
// four names no module-side value exists.** They are not `BindWidget` members, not
// `BindWidgetOptional` members, not any C++ member of any class in this tree; they exist only in
// `Content/UI/WBP_Options.uasset`. So:
//
//   - **THE FOUR NAMES BELOW ARE LITERALS, AND THEY ARE THIS CLAUSE'S SUBJECT rather than the
//     expectation side of a comparison.** There is no second authority to compare them against.
//   - **A RENAME IN THE ASSET IS THEREFORE INDISTINGUISHABLE FROM A DELETION HERE.** This clause
//     reds identically for both and its message cannot tell them apart. That is a real limit and
//     it is not dressed up: a `BindWidget` contract IS a name match, so for a bound member a
//     rename is a broken binding and the clause is right to red; for an unbound caption a rename
//     may be a harmless refactor that this clause will call a regression. The cost of that false
//     red is a test edit. The cost of NOT having the clause has been measured once already.
//   - **THE TYPE EXPECTATION IS A LITERAL TOO** -- `UTextBlock::StaticClass()`, written out. The
//     alternative was considered and REFUSED: `UStratOptionsWidget::MasterValueText` is declared
//     `TObjectPtr<UTextBlock>`, so its `FObjectPropertyBase::PropertyClass` would give a
//     reflection-read `UTextBlock` and would LOOK like a module-side expectation. It is not one.
//     That member is a READOUT, not a caption; borrowing its declared class would let a change to
//     an unrelated member silently move what this clause asserts, which is worse than a literal
//     that says what it means. Reading a value from the wrong subject is not obedience to the
//     rule -- it is the rule's letter with its purpose inverted.
//
// WHAT THE LITERALS COST IS BOUNDED BY THE CONTROLS. Every literal here is a SUBJECT, and a wrong
// subject is RED (the lookup returns null and the clause fails naming the key), never silently
// green. The controls in step (5) are what make that true -- see below.
//
// ---------------------------------------------------------------------------------------
// THE ACCEPTANCE ID: `GATE-AUDIO-SETTINGS`, and nothing is minted. Three of the four captions
// label the three volume sliders, which is that gate's own subject; the fourth labels the back
// button, whose behaviour clause
// `Stratocracy.StratUI.GATE-AUDIO-SETTINGS.TheOptionsBackButtonDismissesWithoutCommitting`
// already sits under the same ID. `GATE-TITLEMENU` was refused: `global.md`'s 2026-09-05 ruling
// places a control under it when *"the subject is the ROUTE, not the screen that requests it"*,
// and a caption reaches no `EStratShellRoute` arm. `T-UI-03` was refused for this surface
// already, on the ground that no `strat::UiSnapshot` field stands behind it -- and that refusal
// is doubly right here, since a static caption is not a drawn value at all.
//
// LOADING A `/Game/` PATH IS THE PERMITTED EXCEPTION, on `StratShippedOptionsExitControlParity.cpp`'s
// stated reading: `CLAUDE.md` names automation-test fixtures as a standing exception to the
// no-path-literals rule, which exists so that GAMEPLAY code cannot hard-wire an asset. A clause
// whose entire subject IS the shipped asset has to name it.
//
// EVERY FAILURE PATH IS RED, INCLUDING "COULD NOT LOAD". A clause about a shipped asset that
// cannot find that asset has observed nothing, and reporting "could not tell" as "fine" hides
// exactly the state it exists to catch.
//
// ---------------------------------------------------------------------------------------
// WHY ONE CLAUSE AND NOT FOUR, ARGUED RATHER THAN ASSUMED. Four separate test declarations --
// one macro apiece, of the kind spelled out once at the bottom of this file -- would name the
// missing caption in the test NAME rather than only in the message, which is a real benefit.
// It was outweighed three ways. (1) The controls
// are what this clause is worth, and they must run in the SAME run and against the SAME loaded
// archetype as the subject assertions, or an absence is not distinguishable from an instrument
// that cannot speak; four clauses means four copies of the control block, and four places for one
// of those copies to rot. (2) The per-caption assertions below are NON-FATAL, so a single run
// already reports EVERY missing caption by name in its own message -- the deletion of two
// captions surfaces as two messages here, and would surface as two red tests there, at four times
// the code. (3) A test count is not a coverage measure and this clause should not inflate one.
// (The macro's own name is deliberately not spelled out in this paragraph. The banner sweep's
// macro census greps for that token by shape, comments included, so naming it here makes one
// clause census as two and reddens the sweep. Please leave it unspelt.)
//
// ---------------------------------------------------------------------------------------
// WHY THE EXACT CAPTION TEXT IS **NOT** ASSERTED, AND WHAT IS ASSERTED INSTEAD.
// The obvious clause is `TestEqual(caption->GetText().ToString(), TEXT("BACK"))`. It was refused
// on three grounds, of which the first is decisive and is a measured property of the framework:
//
//   1. **`TestEqual` ON `FString` IS CASE-INSENSITIVE IN THIS ENGINE.** `FString::operator==`,
//      `Contains` and `TestEqual` all compare without regard to case. So an equality assertion
//      against `TEXT("BACK")` would pass on a caption reading `back`, `Back` or `bAcK` -- it would
//      APPEAR to pin the string while being blind to the most likely way a caption's text
//      actually regresses. An assertion that cannot see the defect it is named for is worse than
//      no assertion, because it retires the question.
//   2. **THE DISPLAYED STRING IS CULTURE-DEPENDENT BY DESIGN.** `FText` is the localisable type;
//      a future localisation pass makes the correct caption read something other than `BACK` and
//      reds this clause for doing its job. The authoring routes differ on this screen already --
//      `content.md` records the three slider captions and `BackLabel` as `INVTEXT(...)` and
//      `ReturnToTitleLabel` as `NSLOCTEXT("", "<hash>", ...)` -- and `INVTEXT` is culture-invariant
//      while `NSLOCTEXT` is not, so an equality clause would mean two different things depending
//      on which route a designer happened to take.
//   3. The four expected strings would be a SECOND set of literals with no more authority than
//      the first, doubling this file's unanchored surface for a property it cannot check soundly.
//
// **WHAT IS ASSERTED INSTEAD IS NON-EMPTINESS**, and it has exactly one authority, which is the
// defect itself: **a caption authored with empty text is indistinguishable on screen from a
// caption that was deleted.** It is culture-independent, case-independent, and route-independent.
// It does not pin the words. Nothing here claims it does.
//
// `UTextBlock::GetText()` is the right reader for an archetype, MEASURED and not remembered:
// `TextBlock.cpp` (UE 5.8) returns `MyTextBlock->GetText()` only `if (MyTextBlock.IsValid())` and
// otherwise returns the authored `Text` property -- and an archetype has no Slate widget, so this
// reads the authored value. Step (5c) is the control that this reader can return empty at all.
//
// ---------------------------------------------------------------------------------------
// THE MUTANT, AND ITS DISCRIMINATION IS THE WHOLE POINT OF THIS CLAUSE.
//
// **IT HAS NOT BEEN RUN. THE SENTENCES BELOW ARE PREDICTIONS, NOT MEASUREMENTS.** The honest
// mutant is deleting a caption from `Content/UI/WBP_Options.uasset` -- which is the editor lane's
// asset and not this lane's, and the test lane neither mutates it nor runs the suite. No mutant
// had been run against this clause as of the exported report `reportCreatedOn 2026.09.07-23.24.24`.
// The same debt is recorded in `Tools/architect/state/tests.md`. Read the list as the
// discrimination this clause was DESIGNED for, and not as discrimination anyone has observed:
//
//   - delete `BackLabel` from the asset -- the exact 2026-09-07 regression -- predicted RED on the
//     subject naming `BackLabel`, GREEN on both controls, so the message points at the asset and
//     not at the instrument. This is the one that matters and it is the one that is unrun.
//   - delete any of the other three -- same shape, and the other three captions still report, so
//     a double deletion reads as two messages rather than one.
//   - blank a caption's `Text` -- predicted RED on the non-emptiness assertion, GREEN on presence
//     and type. A byte scan for the NAME cannot see this mutant at all.
//   - re-type a caption to a non-`UTextBlock` -- predicted RED on the type assertion.
//   - point `kOptionsClassPath` at a class that does not exist -- predicted RED at step (2).
//
// **WHAT CAN BE SAID WITHOUT LEAVING THIS LANE, AND WHAT IT DOES NOT PROVE.** The lookup
// instrument's discrimination IS exercised on every run, by construction rather than by
// prediction: `kAbsentCaptionName` in step (5b) is a name the asset does not carry and is
// asserted ABSENT in the same run, on the same archetype, through the same `FindWidget` call that
// step (6) uses. So "the instrument answers every name" is ruled out by measurement each time the
// suite runs. **That is strictly weaker than the deletion mutant and must not be read as it**: it
// proves `FindWidget` can say no, and says nothing about whether this clause would notice THIS
// asset losing THAT caption. Only deleting a caption and observing the red proves that, and it is
// owed.
//
// ---------------------------------------------------------------------------------------
// WHAT IT DOES **NOT** PIN.
//   - THE CAPTION WORDS. See above. `BACK`, `MASTER`, `SFX`, `MUSIC` are not asserted anywhere in
//     this file, deliberately, and a caption reading `MSATER` is green here.
//   - PARENTAGE. That `BackLabel` is a child of `BackButton` -- the shape `content.md` records the
//     repair as restoring -- is not asserted. A widget tree is not a layout, a caption's parent
//     can legitimately move in a re-layout, and a caption under the wrong parent is a visual
//     defect a human sees immediately. What shipped and was invisible was ABSENCE.
//   - THAT ANY CAPTION IS VISIBLE, POSITIONED, SIZED OR LEGIBLE. A widget tree entry is not
//     geometry, and no clause in this suite can observe Slate layout. A human at the keyboard
//     remains the only instrument for any visual claim.
//   - THE WIDGET TOTAL. A total is what cancelled an addition against a deletion in the pass that
//     caused this; pinning one here would carry the same defect forward and would red on every
//     legitimate addition. This clause names what must be PRESENT and one name that must be
//     ABSENT, which is the direction `content.md`'s own standing lesson asks for.
//   - ANYTHING ABOUT THE THREE SLIDERS, THE BACK BUTTON OR THE EXIT CONTROL BEYOND THE ONE
//     POSITIVE CONTROL. Those are bound members with their own clauses.
// ---------------------------------------------------------------------------------------

#include "Misc/AutomationTest.h"

#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Containers/UnrealString.h"
#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/UnrealType.h"

#include "StratOptionsWidget.h"

namespace StratShippedOptionsCaptions
{
	/**
	 * The options Widget Blueprint's GENERATED class -- the `_C` object and not the
	 * `UWidgetBlueprint` asset object, on `StratShippedOptionsExitControlParity.cpp`'s stated
	 * ground: the generated class is what a `TSubclassOf` resolves and what `CreateWidget`
	 * instantiates, so it is the only object that can answer what the shipped screen contains.
	 */
	const TCHAR* const kOptionsClassPath = TEXT("/Game/UI/WBP_Options.WBP_Options_C");

	/**
	 * THE SUBJECT. Four captions that exist in `Content/UI/WBP_Options.uasset` and NOWHERE ELSE.
	 *
	 * These are literals because nothing in `Source/` names them -- see this file's header for
	 * the full statement of what that costs. They are the expectation side of nothing; they are
	 * what this clause is ABOUT.
	 */
	const TCHAR* const kCaptionNames[] = {
		TEXT("BackLabel"),   // the caption inside `BackButton`; the one that was deleted and shipped
		TEXT("LabelMaster"), // the three slider captions, in the reading order the screen uses
		TEXT("LabelSfx"),
		TEXT("LabelMusic"),
	};

	/**
	 * THE POSITIVE CONTROL, and this clause is worth very little without it. A widget-tree lookup
	 * that returned null for EVERYTHING -- wrong archetype, stripped tree, a `FindWidget` that
	 * cannot see this depth -- would fail all four subject assertions and read as "the editor lane
	 * deleted the captions", which is a true-looking report of a false thing.
	 *
	 * `MasterSlider` and DELIBERATELY NOT `BackButton`. Both are hard `BindWidget` members the
	 * asset cannot compile without, so either would do as a control for LOADING. `BackButton` is
	 * refused because it is the exact shape of the blindness this file exists to answer: in the
	 * 2026-09-07 regression `BackButton` survived while its child `BackLabel` died, and a clause
	 * controlled on `BackButton` reported green about a screen with a blank button on it.
	 * **A control on a parent proves nothing about that parent's children.** A control that
	 * happens to be the parent of a subject invites exactly the wrong inference from a reader.
	 *
	 * The NAME and the CLASS are both read from the member's own reflection data below; this
	 * literal is only the lookup key, and a wrong one is red at step (1).
	 */
	const TCHAR* const kControlMember = TEXT("MasterSlider");

	/** A name no widget on this screen has. The negative control on `FindWidget` itself. */
	const TCHAR* const kAbsentCaptionName = TEXT("ZzzNoSuchCaptionOnTheOptionsScreen");

	/**
	 * THE NEGATIVE CONTROL ON THE **TEXT** READER, which is a different instrument from the
	 * lookup and needs its own.
	 *
	 * `ReturnToTitleReasonText` is authored with EMPTY text on purpose -- `content.md` records the
	 * reason, and it is C++-anchored: `UStratOptionsWidget::NativeConstruct` calls
	 * `SyncExitWidgetsToModel()` unconditionally, which assigns `ExitModel.ReturnToTitleReason`,
	 * and that field defaults empty, so any authored placeholder would be a string no view-model
	 * field ever produced. That makes it the one widget on this screen whose `GetText()` MUST come
	 * back empty, and therefore the only available proof that step (6)'s non-emptiness assertion is
	 * not satisfied by a reader that returns something for everything.
	 *
	 * IF STEP (5c) EVER REDS, READ IT BEFORE TOUCHING STEP (6). Two different things produce it:
	 * the reader is broken, or this widget was legitimately given text by a later pass. In the
	 * second case the CONTROL moves to another empty widget or is retired with its reason written
	 * down -- the subject assertions are not weakened to accommodate it.
	 */
	const TCHAR* const kEmptyTextControlWidget = TEXT("ReturnToTitleReasonText");
}

// ---------------------------------------------------------------------------------------
// GATE-AUDIO-SETTINGS -- the shipped options screen still has its captions.
//
// THE DEFECT THIS IS FOR: a caption disappearing from the asset with nothing to notice. It has
// happened once, it shipped, and the user found it by playing the game.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratShippedOptionsWidgetCarriesItsUnboundCaptionsTest,
	"Stratocracy.StratUI.GATE-AUDIO-SETTINGS.ShippedOptionsWidgetCarriesItsUnboundCaptions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratShippedOptionsWidgetCarriesItsUnboundCaptionsTest::RunTest(
	const FString& /*Parameters*/)
{
	using namespace StratShippedOptionsCaptions;

	// ---- 1. THE ONE MODULE-SIDE VALUE THIS CLAUSE HAS ---------------------------------------
	// The positive control's name and declared class come from `UStratOptionsWidget`'s own
	// reflection data, not from a string retyped here. The four SUBJECT names cannot -- no member
	// declares them, which is the whole reason this file exists.
	const FObjectPropertyBase* const ControlProperty = CastField<FObjectPropertyBase>(
		UStratOptionsWidget::StaticClass()->FindPropertyByName(FName(kControlMember)));
	if (!TestNotNull(*FString::Printf(
			TEXT("FIXTURE: UStratOptionsWidget declares the control member '%s'. If this is red the "
			     "C++ member was renamed and this clause has lost its positive control -- point it "
			     "at another hard BindWidget member; do not run without one"), kControlMember),
			ControlProperty))
	{
		return false;
	}

	UClass* const ExpectedControlClass = ControlProperty->PropertyClass;
	if (!TestNotNull(TEXT("FIXTURE: the control member declares a widget class"),
			ExpectedControlClass))
	{
		return false;
	}

	// ---- 2. THE SUBJECT: THE SHIPPED CLASS ---------------------------------------------------
	// No base-class filter, for `StratShippedOptionsExitControlParity.cpp`'s measured reason:
	// filtering on `UStratOptionsWidget` would turn "the asset was re-parented" -- a defect step
	// (3) exists to catch -- into "the asset did not load", and blame the wrong thing.
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
	if (!TestTrue(*FString::Printf(
			TEXT("ASSET Content/UI/WBP_Options.uasset: its generated class '%s' has "
			     "UStratOptionsWidget as an ancestor (its immediate parent reads '%s'). If this is "
			     "red the Widget Blueprint has been re-parented, the positive control below is no "
			     "longer guaranteed by any compiler, and this clause's instrument is unproved"),
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

	// `FindWidgetTreeOwningClass` and not the class's own member, for the reason
	// `StratShippedOptionsExitControlParity.cpp` established: a WBP that inherits its layout
	// carries no tree of its own, and reading the null would report "no captions" about a screen
	// whose captions are on its parent.
	const UWidgetBlueprintGeneratedClass* const TreeOwner =
		GeneratedClass->FindWidgetTreeOwningClass();
	if (!TestNotNull(TEXT("some class in the shipped hierarchy owns a widget tree"), TreeOwner))
	{
		return false;
	}

	const UWidgetTree* const Tree = TreeOwner->GetWidgetTreeArchetype();
	if (!TestNotNull(TEXT("that class's widget tree archetype is present -- this is the object "
			"UUserWidget::Initialize duplicates, and the only object that knows what the shipped "
			"screen contains"), Tree))
	{
		return false;
	}

	// ---- 5. THE INSTRUMENTS, MEASURED BEFORE THEY ARE TRUSTED --------------------------------
	// Three controls, all in THIS run and against THIS archetype. An absence measured without them
	// is not distinguishable from an instrument that cannot speak.

	// (a) POSITIVE CONTROL ON THE LOOKUP. A hard `BindWidget` member: `WBP_Options` cannot compile
	//     without it, so a null here is a broken instrument and not a missing widget.
	UWidget* const ControlWidget = Tree->FindWidget(ControlProperty->GetFName());
	if (!TestNotNull(*FString::Printf(
			TEXT("CONTROL: the widget tree contains '%s', a BindWidget (not optional) member the "
			     "asset cannot compile without. If THIS is null the caption lookups below can say "
			     "nothing -- the archetype or the search is wrong, and a 'captions are missing' "
			     "report from this clause would be false"),
			*ControlProperty->GetFName().ToString()),
			ControlWidget))
	{
		return false;
	}
	if (!TestTrue(*FString::Printf(
			TEXT("CONTROL: and it is a '%s', as the C++ member itself declares (read: '%s')"),
			*ExpectedControlClass->GetName(),
			ControlWidget->GetClass() != nullptr ? *ControlWidget->GetClass()->GetName()
			                                     : TEXT("<none>")),
			ControlWidget->IsA(ExpectedControlClass)))
	{
		return false;
	}

	// (b) NEGATIVE CONTROL ON THE LOOKUP. A search that answered every name would make step (6)
	//     vacuous -- it would report four captions present on an empty tree.
	if (!TestNull(*FString::Printf(
			TEXT("CONTROL: the widget tree does NOT contain '%s', so FindWidget can say no and the "
			     "four assertions below are not satisfied by an instrument that answers everything"),
			kAbsentCaptionName),
			Tree->FindWidget(FName(kAbsentCaptionName))))
	{
		return false;
	}

	// (c) NEGATIVE CONTROL ON THE TEXT READER, which is a separate instrument from the lookup.
	//     See `kEmptyTextControlWidget`'s declaration for why this widget is empty by design and
	//     what to do if this ever reds. Non-fatal: a broken text control does not invalidate the
	//     PRESENCE assertions below, only the non-emptiness ones, and reporting all of them is
	//     more useful than stopping here.
	if (const UTextBlock* const EmptyTextControl =
			Cast<UTextBlock>(Tree->FindWidget(FName(kEmptyTextControlWidget))))
	{
		TestTrue(*FString::Printf(
			TEXT("CONTROL: '%s' reads back EMPTY through UTextBlock::GetText(), which is how it is "
			     "authored on purpose. This is the proof that the reader below can return empty at "
			     "all -- without it, 'every caption has text' could be an artefact of a reader that "
			     "never returns nothing. If this is red, read this file's note on it before "
			     "touching anything below (read: '%s')"),
			kEmptyTextControlWidget, *EmptyTextControl->GetText().ToString()),
			EmptyTextControl->GetText().IsEmpty());
	}
	else
	{
		// Not a failure of the subject, and deliberately not silent either: an unavailable control
		// is a fact about how much the run below is worth, and it belongs in the log.
		AddWarning(FString::Printf(
			TEXT("CONTROL UNAVAILABLE: '%s' is not a UTextBlock in the shipped widget tree, so the "
			     "text reader's ability to report EMPTY is unproved on this run. The presence and "
			     "type assertions below are unaffected; the non-emptiness ones are weaker than "
			     "they read. Re-point this control at another deliberately-empty widget"),
			kEmptyTextControlWidget));
	}

	// ---- 6. THE CLAUSE -----------------------------------------------------------------------
	// NON-FATAL AND ON PURPOSE: every caption is checked on every run, so two deletions surface as
	// two messages rather than as one message and three unknowns. Nothing below returns early.
	for (const TCHAR* const CaptionName : kCaptionNames)
	{
		UWidget* const Caption = Tree->FindWidget(FName(CaptionName));
		if (!TestNotNull(*FString::Printf(
				TEXT("ASSET Content/UI/WBP_Options.uasset: its widget tree contains a child named "
				     "'%s'. If this is red that caption has been DELETED or RENAMED -- this clause "
				     "cannot tell those apart, because no C++ member names it and there is no "
				     "second authority to ask. A deletion is what happened on 2026-09-07 and it "
				     "shipped: the suite could not move, the shipped-asset parity clause stayed "
				     "green on its BackButton control, and the user found it by playing the game"),
				CaptionName),
				Caption))
		{
			continue;
		}

		const UTextBlock* const CaptionText = Cast<UTextBlock>(Caption);
		if (!TestNotNull(*FString::Printf(
				TEXT("and '%s' is a UTextBlock rather than merely a child of that name (read: "
				     "'%s'). A byte scan of the .uasset for the NAME cannot tell these apart, and a "
				     "caption that is not a text block draws no words"),
				CaptionName,
				Caption->GetClass() != nullptr ? *Caption->GetClass()->GetName() : TEXT("<none>")),
				CaptionText))
		{
			continue;
		}

		// NON-EMPTINESS, NOT THE WORDS. See this file's header for the three reasons the exact
		// string is refused -- the decisive one being that TestEqual on FString is case-insensitive
		// in this engine, so an equality assertion would be blind to the likeliest text regression
		// while appearing to pin it.
		TestFalse(*FString::Printf(
			TEXT("and '%s' carries non-empty authored text. An empty caption is indistinguishable "
			     "on screen from a deleted one, which is this clause's whole subject. THE WORDS ARE "
			     "NOT ASSERTED -- this is green on a caption reading anything at all (read: '%s')"),
			CaptionName, *CaptionText->GetText().ToString()),
			CaptionText->GetText().IsEmpty());
	}

	return true;
}
