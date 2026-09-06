// `FStratAudioOptionsModel` and the options widget's push model. The local ID
// `GATE-AUDIO-SETTINGS` is authorized for this subject by the 2026-09-05 ruling in
// `Tools/architect/state/global.md`, which names *"`UStratOptionsWidget`'s push-model setters"*
// as one of the three facets that ID covers, mints no acceptance ID and claims none.
//
// ---------------------------------------------------------------------------------------
// WHY THIS IS NOT `T-UI-03`, WHICH IS THE ID THE ENGINEER'S HANDOFF PROPOSED. `T-UI-03`'s
// defining clause is a single sentence -- *"no widget-side arithmetic; every number a widget
// draws equals exactly one `strat::UiSnapshot` field"* -- and it is not a general rule about
// widgets. It is a rule about the JOIN between a widget and the rules module's snapshot, and its
// whole force is that the widget does no arithmetic BECAUSE the snapshot already carries the
// number.
//
// `FStratAudioOptionsModel` IS NOT BUILT FROM A `UiSnapshot`, AND THE DIFFERENCE IS NOT
// COSMETIC: there is no `strat::` field for a volume to equal, because a player's gain is not a
// rules fact and the rules module has never heard of it. `StratBuildAudioOptionsModel` DOES
// arithmetic -- it multiplies by a hundred and rounds -- and that is correct here and would be a
// `T-UI-03` violation anywhere `T-UI-03` applies. Naming this ID `T-UI-03` would therefore have
// asserted the opposite of what this file measures, and would have put a clause under an ID
// whose own sentence it visibly breaks. The clause below pins the property that actually
// matters for a model with no snapshot behind it: THE TEXT IS A RENDERING OF THE MODEL'S OWN
// STORED FIELD, so the number a player reads and the number that will be saved cannot drift.
//
// ---------------------------------------------------------------------------------------
// WHERE THE EXPECTATIONS COME FROM. No percent string is written in this file. The text clauses
// compare one module-side call against another module-side call -- `StratBuildAudioOptionsModel`
// asked twice, once with the raw input and once with what the module's own `StratClampVolume`
// makes of it -- so a change to the format string moves both sides and is not this clause's
// business, while a text that stopped tracking its own field moves one side only. The clamp
// interval is never written here either; `StratSoundCues.h` owns it.
//
// WHAT THIS FILE DOES NOT PIN.
//   - THAT `OnAudioOptionsRefreshed` FIRES. It is a `BlueprintImplementableEvent`: with no
//     Blueprint under the class the call is a no-op, and C++ cannot override it to observe one.
//     What IS observed is the state the widget writes BEFORE firing it -- `Model` -- which is
//     the thing any refresh would have to carry. A widget that wrote the model and deleted the
//     event call would pass here. That is stated, not implied.
//   - ANY PIXEL, ANY SLIDER, ANY LAYOUT. No `WBP_` asset derives from `UStratOptionsWidget`
//     yet; the widget header says so itself and records what discharges it.
//   - THAT A COMMIT REACHES A SLOT. The binder from this delegate to
//     `UStratSoundDirector::CommitVolumes` does not exist yet -- again the widget header's own
//     statement. `Stratocracy.StratPlay.GATE-AUDIO-SETTINGS.CommittedVolumesRoundTripThroughASlot`
//     pins the far side of that join independently.
//
// STAMPED 2026-09-05 -- THE TWO BULLETS ABOVE HAVE TRUE SUBJECTS AND FALSE PARENTHESES, AND ONLY
// THE PARENTHESES ARE STAMPED. Both bullets' HEADINGS still hold and are the reason the bullets
// exist: this file constructs no Slate, so it pins no pixel, no slider and no layout; and no
// clause in this file observes a commit reaching a slot. Neither heading is touched. What went
// false is the aside each one appends about what does not exist in the tree, and each was
// measured rather than taken from the report that raised it:
//   - A `WBP_` ASSET DOES DERIVE FROM `UStratOptionsWidget`. `Content/UI/WBP_Options.uasset`
//     carries `/Script/StratUI.StratOptionsWidget` 4x and `Default__StratOptionsWidget` once, by
//     `grep -a -o "[A-Za-z/._]*StratOptionsWidget[A-Za-z_]*"` piped through `sort | uniq -c`.
//     `grep -a -c` reports 2 on the same file because it counts LINES and this is a binary, which
//     is why the two figures differ and neither is wrong. THE NEGATIVE CONTROL IS NOT OPTIONAL
//     HERE: `strings` IS NOT ON THIS BOX (`command -v strings` prints nothing), so a
//     `strings`-based check returns zero lines on a 39 KB asset -- a zero that reads exactly like
//     "not found". The same `grep -a` was therefore run against `Content/UI/WBP_TitleMenu.uasset`:
//     0 for `StratOptionsWidget` while 16 for `Widget`, which is the instrument shown able to
//     speak before its silence is read as an absence. The asset is pinned independently by
//     `Stratocracy.StratPlay.GATE-TITLEMENU.AllThreeShippedGameModesNameOneOptionsWidgetClass`.
//   - THE BINDER EXISTS. `Source/StratPlay/StratOptionsPresenter.cpp:189` is
//     `Created->OnAudioOptionsCommitted.AddDynamic(this, &UStratOptionsPresenter::HandleAudioOptionsCommitted)`,
//     and `:308` inside that handler is `Director->CommitVolumes(InModel.MasterVolume, ...)`. The
//     near side of the join is pinned by
//     `Stratocracy.StratPlay.GATE-AUDIO-SETTINGS.ThePresenterSeedsThePanelWithoutForwardingACommitAndForwardsARealOne`,
//     and the far side by the clause the bullet already names, which is unchanged and still
//     correct.
//
// WHY "YET" DOES NOT SAVE EITHER SENTENCE, since a true sentence stamped false is as much a
// defect as the reverse and this was the one judgement call in the repair. A bare "yet" is not a
// time qualifier: "no asset derives from it YET" asserts the absence NOW and adds only an
// expectation that it will change. Contrast the sentence in `StratOptionsWidget.h` that that
// file's own stamp explicitly SPARES -- *"As of this file's first commit ... no WBP derives from
// this class"* -- which anchors to a named moment and is therefore still true today. Neither
// sentence here has an anchor, so neither survives, and the difference between the two forms is
// the whole reason that header could stamp one claim and spare the other.
//
// AND THE CITATION IS STAMPED, NOT ONLY THE CLAIM, WHICH IS THE PART WORTH READING. Both bullets
// rest their authority on the same file -- *"the widget header says so itself"*, *"again the
// widget header's own statement"* -- and that authority HAD ALREADY BEEN CORRECTED at the moment
// this block was still being read as current. `git log --reverse -S "WHAT IS TRUE NOW, each part
// measured rather than asserted" -- Source/StratUI/StratOptionsWidget.h` returns `c69e519`, and
// this file carried no stamp of any kind at that commit or before it. So a correction landed at
// the cited authority and did not announce itself at the site citing it, and a reader who trusted
// the citation instead of following it got the pre-correction answer. A CITATION IS A CLAIM ABOUT
// ANOTHER FILE'S CURRENT TEXT AND AGES EXACTLY AS ANY OTHER CLAIM DOES. `StratOptionsWidget.h`'s
// block is still the right thing to read here; what was wrong is the summary of it above, and
// this file is now the place that says so rather than leaving the two to be reconciled by
// whoever next notices they disagree.
// ---------------------------------------------------------------------------------------

#include "Misc/AutomationTest.h"

#include "Containers/UnrealString.h"
#include "Internationalization/Text.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "StratOptionsWidget.h"
#include "StratSoundCues.h"

#include "StratOptionsWidgetDouble.h"

// ---------------------------------------------------------------------------------------
// The text a row draws is a rendering of the model's own clamped field.
//
// WHAT IT PINS, IN ONE SENTENCE: `Model.MasterVolumeText` is what you get by asking the module
// to build a model FROM `Model.MasterVolume`. Not from the raw input, and not from some other
// channel's field.
//
// THE THREE WAYS THIS CAN GO WRONG AND HOW EACH IS CAUGHT.
//   1. THE TEXT RENDERS THE RAW INPUT INSTEAD OF THE STORED FIELD. Caught by the out-of-range
//      case: a model built from 1.4 stores 1.0 and must READ as a model built from 1.0 reads. A
//      text rendering 1.4 would say something else.
//   2. THE TEXT RENDERS ANOTHER CHANNEL'S FIELD. Caught by building with three distinct values
//      and asserting the three texts differ. A body that wrote the master text into all three
//      is red.
//   3. THE ROUNDING MOVES THE STORED FIELD. This is the control the brief named and it is the
//      subtle one: 0.999 must READ as 1.0 reads -- a hundredth of a percent is not a number to
//      put in front of a player -- while the FIELD stays 0.999, because that is what gets
//      saved and re-applied. A build that rounded the float itself would pass the text half and
//      is caught by the field half. The negative control at 0.994 keeps the text equality from
//      being satisfied by a model whose texts are all the same string.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratOptionsModelTextTracksItsFieldTest,
	"Stratocracy.StratUI.GATE-AUDIO-SETTINGS.OptionsModelTextRendersItsOwnClampedField",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratOptionsModelTextTracksItsFieldTest::RunTest(const FString& /*Parameters*/)
{
	// ---- 1. THE STORED FIELD IS THE CLAMPED INPUT ---------------------------
	const float RawMaster = 1.4f;
	const float RawSfx    = -0.5f;
	const float RawMusic  = 0.25f;

	const FStratAudioOptionsModel Model =
		StratBuildAudioOptionsModel(RawMaster, RawSfx, RawMusic);

	TestEqual(TEXT("the master field is the module's own clamp of the input"),
		Model.MasterVolume, StratClampVolume(RawMaster));
	TestEqual(TEXT("the sfx field is the module's own clamp of the input"),
		Model.SfxVolume, StratClampVolume(RawSfx));
	TestEqual(TEXT("the music field is unchanged when it was already legal"),
		Model.MusicVolume, StratClampVolume(RawMusic));

	// ---- 2. THE TEXT IS A RENDERING OF THAT FIELD ---------------------------
	// The right-hand side is the module asked a second time, with the value the model actually
	// stored. Nothing here writes a percent string.
	const FStratAudioOptionsModel FromStored =
		StratBuildAudioOptionsModel(Model.MasterVolume, Model.SfxVolume, Model.MusicVolume);

	TestTrue(*FString::Printf(TEXT("the master text renders the stored field, not the raw "
		"input (it says '%s'; the stored field renders as '%s')"),
		*Model.MasterVolumeText.ToString(), *FromStored.MasterVolumeText.ToString()),
		Model.MasterVolumeText.EqualTo(FromStored.MasterVolumeText));
	TestTrue(*FString::Printf(TEXT("the sfx text renders the stored field (it says '%s'; the "
		"stored field renders as '%s')"),
		*Model.SfxVolumeText.ToString(), *FromStored.SfxVolumeText.ToString()),
		Model.SfxVolumeText.EqualTo(FromStored.SfxVolumeText));
	TestTrue(*FString::Printf(TEXT("the music text renders the stored field (it says '%s'; the "
		"stored field renders as '%s')"),
		*Model.MusicVolumeText.ToString(), *FromStored.MusicVolumeText.ToString()),
		Model.MusicVolumeText.EqualTo(FromStored.MusicVolumeText));

	// ---- 3. THE THREE ROWS ARE NOT ONE ROW THREE TIMES ----------------------
	// Master clamps to the top of the range and music sits at a quarter, so these two must
	// differ. Without this, every text equality above is satisfiable by a builder that puts one
	// string in all three fields.
	TestFalse(*FString::Printf(TEXT("control: the master and music rows draw different text "
		"('%s' vs '%s')"), *Model.MasterVolumeText.ToString(),
		*Model.MusicVolumeText.ToString()),
		Model.MasterVolumeText.EqualTo(Model.MusicVolumeText));

	// ---- 4. THE ROUNDING IS ON THE DISPLAY AND NOT ON THE FIELD -------------
	const FStratAudioOptionsModel NearlyFull = StratBuildAudioOptionsModel(0.999f, 0.5f, 0.5f);
	const FStratAudioOptionsModel Full       = StratBuildAudioOptionsModel(1.0f,   0.5f, 0.5f);
	const FStratAudioOptionsModel JustUnder  = StratBuildAudioOptionsModel(0.994f, 0.5f, 0.5f);

	TestTrue(*FString::Printf(TEXT("a gain a thousandth below unity reads the same as unity "
		"('%s' vs '%s')"), *NearlyFull.MasterVolumeText.ToString(),
		*Full.MasterVolumeText.ToString()),
		NearlyFull.MasterVolumeText.EqualTo(Full.MasterVolumeText));

	TestNotEqual(TEXT("but the field it will save is NOT unity -- the rounding is a display "
		"decision and must not move the stored gain"),
		NearlyFull.MasterVolume, Full.MasterVolume);

	TestFalse(*FString::Printf(TEXT("control: a gain far enough below unity reads differently, "
		"so the equality above is not satisfied by every text being equal ('%s' vs '%s')"),
		*JustUnder.MasterVolumeText.ToString(), *Full.MasterVolumeText.ToString()),
		JustUnder.MasterVolumeText.EqualTo(Full.MasterVolumeText));

	return true;
}

// ---------------------------------------------------------------------------------------
// Seeding the options screen commits nothing.
//
// WHAT IT PINS, AND WHY IT IS THE CLAUSE THIS SURFACE MOST NEEDED. The owner that opens this
// screen must first tell it what the current gains ARE -- that is `PushAudioOptions`. If seeding
// also broadcast a commit, then merely OPENING the options screen would write the settings slot,
// and it would do it with whatever values the owner happened to seed: on a machine where the
// owner seeded defaults, opening the screen and closing it would silently reset the player's
// volumes. THAT DEFECT IS INVISIBLE FROM THE SCREEN. Nothing on it would look wrong.
//
// AND THE OTHER DIRECTION IS ASSERTED IN THE SAME CLAUSE, because a widget that never committed
// at all would pass the first half perfectly: the three setters must each broadcast exactly
// once, carrying the model the widget itself now holds.
//
// THE SETTERS ARE ASKED ONE AT A TIME AND EACH IS CHECKED AGAINST THE WIDGET'S OWN `Model`, so
// a setter wired to the wrong field -- the music slider writing the SFX gain, which the widget
// header names as the exact hazard the one-model delegate exists to remove -- is caught by the
// value carried, not merely by the count.
// ---------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FStratOptionsSeedingCommitsNothingTest,
	"Stratocracy.StratUI.GATE-AUDIO-SETTINGS.SeedingTheOptionsScreenCommitsNothing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStratOptionsSeedingCommitsNothingTest::RunTest(const FString& /*Parameters*/)
{
	const TStrongObjectPtr<UStratOptionsWidgetDouble> Widget(
		NewObject<UStratOptionsWidgetDouble>());
	if (!Widget.IsValid())
	{
		AddError(TEXT("could not construct a UStratOptionsWidget double; nothing was "
			"observed."));
		return false;
	}

	Widget->OnAudioOptionsCommitted.AddDynamic(Widget.Get(),
		&UStratOptionsWidgetDouble::HandleCommitted);

	// ---- SEEDING ------------------------------------------------------------
	const FStratAudioOptionsModel Seed = StratBuildAudioOptionsModel(0.2f, 0.4f, 0.6f);

	Widget->PushAudioOptions(Seed);

	TestEqual(TEXT("seeding the screen commits nothing"), Widget->CommitCount, 0);

	// AND IT DID SOMETHING, so the zero above is not explained by a push that was a no-op. The
	// widget's own model is the observation; the refresh EVENT is not observable here and this
	// file's header says so.
	TestEqual(TEXT("but the seeded master gain reached the widget's model"),
		Widget->Model.MasterVolume, Seed.MasterVolume);
	TestEqual(TEXT("and the seeded sfx gain"),
		Widget->Model.SfxVolume, Seed.SfxVolume);
	TestEqual(TEXT("and the seeded music gain"),
		Widget->Model.MusicVolume, Seed.MusicVolume);

	// ---- ONE SETTER, ONE COMMIT --------------------------------------------
	Widget->SetMasterVolume(0.9f);

	TestEqual(TEXT("moving the master slider commits exactly once"), Widget->CommitCount, 1);
	TestEqual(TEXT("and the commit carries the widget's own new master gain"),
		Widget->LastCommitted.MasterVolume, Widget->Model.MasterVolume);
	TestEqual(TEXT("and it carries the OTHER channels unchanged, so a commit is a whole model "
		"and not one field"),
		Widget->LastCommitted.SfxVolume, Seed.SfxVolume);

	Widget->SetSfxVolume(0.1f);

	TestEqual(TEXT("moving the sfx slider commits exactly once more"), Widget->CommitCount, 2);
	TestEqual(TEXT("and the commit carries the widget's own new sfx gain"),
		Widget->LastCommitted.SfxVolume, Widget->Model.SfxVolume);
	TestEqual(TEXT("while the master gain set a moment ago survives it"),
		Widget->LastCommitted.MasterVolume, Widget->Model.MasterVolume);

	Widget->SetMusicVolume(0.3f);

	TestEqual(TEXT("moving the music slider commits exactly once more"), Widget->CommitCount, 3);
	TestEqual(TEXT("and the commit carries the widget's own new music gain"),
		Widget->LastCommitted.MusicVolume, Widget->Model.MusicVolume);

	// AND THE THREE SETTERS MOVED THREE DIFFERENT FIELDS. Without this, a widget whose three
	// setters all wrote the master gain would satisfy every equality above -- each assertion
	// compares the commit against the widget's own model, and both would be wrong together.
	TestNotEqual(TEXT("control: the master and music gains differ after three setters, so the "
		"three are not one field under three names"),
		Widget->Model.MasterVolume, Widget->Model.MusicVolume);
	TestNotEqual(TEXT("control: and so do the sfx and music gains"),
		Widget->Model.SfxVolume, Widget->Model.MusicVolume);

	// ---- SEEDING AGAIN STILL COMMITS NOTHING -------------------------------
	// The positive control for the first assertion, run after the delegate has demonstrably
	// fired three times: a push that arrives on an already-open screen must not write the slot
	// either, and by now the observer is proven alive.
	const int32 CommitsBeforeReseed = Widget->CommitCount;
	Widget->PushAudioOptions(Seed);

	TestEqual(TEXT("re-seeding an open screen still commits nothing, on an observer that has "
		"just fired three times"), Widget->CommitCount, CommitsBeforeReseed);
	TestEqual(TEXT("and the re-seed did reach the model"),
		Widget->Model.MusicVolume, Seed.MusicVolume);

	return true;
}
