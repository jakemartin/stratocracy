// GDD Sec 2.11.5 -- the volume screen's model builder and its three setters.
//
// THE WHOLE OF THIS FILE'S JOB, repeated here because this is where it is possible to break it:
// there is exactly ONE arithmetic expression below -- the percent -- and it exists so that the
// expression does not appear in a Blueprint graph. Every other line is a clamp through
// `StratClampVolume`, an assignment, or an event. Nothing here reads a bridge, names a `strat`
// type, or touches a `USoundMix`; this module cannot reach one and the file header says why that
// is structural rather than disciplined.
//
// THE THREE SETTERS ARE DELIBERATELY NOT COLLAPSED INTO A HELPER TAKING AN INDEX OR AN ENUM. A
// channel selector would put "which slider am I" into a runtime value, and the way that goes
// wrong -- the music slider passing the SFX channel -- is silent, plausible, and exactly the
// class of defect `UStratSoundBank`'s seven named properties refuse for the same reason. Three
// near-identical functions are the shape in which that mistake is a visible copy-paste error.

// AMENDED 2026-09-05 -- THE NATIVE WIRING. Four `AddDynamic` calls, four `RemoveDynamic` calls,
// and one function that copies `Model` onto whatever sub-widgets the asset supplied. The header
// records why the wiring is here rather than in a WBP graph (measured: no graph can be authored
// headlessly) and why the readouts bind optionally while the controls do not.
//
// THE UMG INCLUDES ARE HERE AND MUST NOT MIGRATE INTO THE HEADER, which holds the four members
// as `TObjectPtr`s over forward declarations. `UMG` is a PRIVATE dependency of `StratUI` and
// `StratGuidanceRouteProbe.h` records the ~60 `LNK2019` that fact was measured with; the header
// already reaches `Blueprint/UserWidget.h` and adding three more widget headers to it would
// widen that surface for no caller's benefit.

#include "StratOptionsWidget.h"

#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

#include "StratSoundCues.h"

FStratAudioOptionsModel StratBuildAudioOptionsModel(const float InMasterVolume,
                                                    const float InSfxVolume,
                                                    const float InMusicVolume)
{
	// THE ONE ARITHMETIC EXPRESSION IN THIS MODULE'S OPTIONS SURFACE, AND IT IS CONTAINED HERE
	// SO THAT A GRAPH NEVER WRITES IT. `Round` and not `Trunc`: a slider at its stop reports
	// "100%" rather than "99%". See the declaration for the cost of that choice -- 0.996 also
	// reads "100%", so the string is a label and never an inverse of the float.
	const auto PercentText = [](const float Volume)
	{
		return FText::FromString(
			FString::Printf(TEXT("%d%%"), FMath::RoundToInt(Volume * 100.0f)));
	};

	FStratAudioOptionsModel Model;

	// CLAMPED FIRST, THEN FORMATTED FROM THE CLAMPED VALUE. Formatting the argument would let a
	// screen display a percentage the stored gain does not have -- the two-source drift this
	// struct pairs its fields specifically to prevent.
	Model.MasterVolume = StratClampVolume(InMasterVolume);
	Model.SfxVolume    = StratClampVolume(InSfxVolume);
	Model.MusicVolume  = StratClampVolume(InMusicVolume);

	Model.MasterVolumeText = PercentText(Model.MasterVolume);
	Model.SfxVolumeText    = PercentText(Model.SfxVolume);
	Model.MusicVolumeText  = PercentText(Model.MusicVolume);

	return Model;
}

void UStratOptionsWidget::PushAudioOptions(const FStratAudioOptionsModel& InModel)
{
	// REBUILT FROM THE ARGUMENT'S FLOATS RATHER THAN COPIED, WHICH IS THE POINT OF THE LINE. A
	// straight assignment would trust the caller's three strings, and a caller that built its
	// model by hand -- a fixture, a graph, a future owner -- could then put a percentage on
	// screen that its own gain does not have. Rebuilding makes the pairing an invariant of this
	// class instead of a promise about its callers.
	Model = StratBuildAudioOptionsModel(InModel.MasterVolume, InModel.SfxVolume,
	                                    InModel.MusicVolume);

	// THE SEEDING PATH IS THE ONE THAT HAS TO MOVE THE THUMBS. A `WBP` opened for the first time
	// draws its authored slider values, which are whatever the designer left them at; without
	// this line the screen would show unity while the model held a gain the player chose three
	// sessions ago, and the first drag would jump.
	SyncBoundWidgetsToModel();

	// REFRESH ONLY. `OnAudioOptionsCommitted` IS DELIBERATELY NOT FIRED HERE -- see the
	// declaration: seeding a screen with what the player chose last time is not the player
	// choosing again, and firing here would make opening the options screen re-save the slot.
	OnAudioOptionsRefreshed(Model);
}

void UStratOptionsWidget::SetMasterVolume(const float InVolume)
{
	// THE OTHER TWO ARE CARRIED THROUGH UNCHANGED, FROM THE MODEL AND NOT FROM ANY CACHE. The
	// model is the only state this class has, which is what makes "the screen equals the model"
	// true by construction rather than by maintenance.
	Model = StratBuildAudioOptionsModel(InVolume, Model.SfxVolume, Model.MusicVolume);

	// "THE THUMB MUST BE WHERE THE MODEL IS" IS NOW A LINE AND NOT ONLY A SENTENCE. Until the
	// native binding landed, this class had no way to move a thumb and the ordering below was
	// the whole of the guarantee; `SyncBoundWidgetsToModel` is what makes it enforceable, and it
	// matters most on the clamping paths -- a control that submitted 1.4 or NaN is left sitting
	// somewhere `Model` is not.
	SyncBoundWidgetsToModel();

	// REFRESH BEFORE COMMIT. The thumb must be where the model is before anything acts on it --
	// see the declaration. Today nothing is bound to the commit at all, and that ordering is
	// what makes the screen still correct in that state.
	OnAudioOptionsRefreshed(Model);
	OnAudioOptionsCommitted.Broadcast(Model);
}

void UStratOptionsWidget::SetSfxVolume(const float InVolume)
{
	Model = StratBuildAudioOptionsModel(Model.MasterVolume, InVolume, Model.MusicVolume);

	SyncBoundWidgetsToModel();
	OnAudioOptionsRefreshed(Model);
	OnAudioOptionsCommitted.Broadcast(Model);
}

void UStratOptionsWidget::SetMusicVolume(const float InVolume)
{
	Model = StratBuildAudioOptionsModel(Model.MasterVolume, Model.SfxVolume, InVolume);

	SyncBoundWidgetsToModel();
	OnAudioOptionsRefreshed(Model);
	OnAudioOptionsCommitted.Broadcast(Model);
}

// ---------------------------------------------------------------------------
// THE NATIVE WIRING.

void UStratOptionsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// EVERY BIND IS GUARDED EVEN THOUGH THREE OF THE FOUR ARE `BindWidget`, AND THE GUARD IS
	// LOAD-BEARING RATHER THAN BELT-AND-BRACES. `BindWidget` is enforced by the WIDGET BLUEPRINT
	// COMPILER, so it says nothing at all about a NATIVE subclass -- `UStratOptionsWidgetDouble`
	// is a concrete C++ `UStratOptionsWidget` with no widget tree, every one of these members is
	// null in it, and an unguarded `AddDynamic` would crash every clause that constructs one.
	if (MasterSlider != nullptr)
	{
		// THE DOMAIN IS SET BEFORE THE BIND, so that a range correction cannot be mistaken for a
		// player's drag by a handler that is already listening. See the header on why the range
		// is C++'s and not the asset's.
		MasterSlider->SetMinValue(0.0f);
		MasterSlider->SetMaxValue(1.0f);
		MasterSlider->OnValueChanged.AddDynamic(
			this, &UStratOptionsWidget::HandleMasterSliderChanged);
	}

	if (SfxSlider != nullptr)
	{
		SfxSlider->SetMinValue(0.0f);
		SfxSlider->SetMaxValue(1.0f);
		SfxSlider->OnValueChanged.AddDynamic(this, &UStratOptionsWidget::HandleSfxSliderChanged);
	}

	if (MusicSlider != nullptr)
	{
		MusicSlider->SetMinValue(0.0f);
		MusicSlider->SetMaxValue(1.0f);
		MusicSlider->OnValueChanged.AddDynamic(
			this, &UStratOptionsWidget::HandleMusicSliderChanged);
	}

	if (BackButton != nullptr)
	{
		BackButton->OnClicked.AddDynamic(this, &UStratOptionsWidget::HandleBackClicked);
	}

	// THE SCREEN IS DRAWN FROM WHATEVER `Model` ALREADY HOLDS, AND THAT IS UNITY UNLESS AN OWNER
	// SEEDED FIRST. `CreateWidget` returns before `NativeConstruct` runs only if the widget is
	// not yet added to the viewport, so an owner that seeds after `AddToViewport` still reaches
	// `PushAudioOptions`, which syncs again. Doing it here as well means a widget constructed and
	// never seeded still draws a screen consistent with itself rather than a designer's leftover
	// thumb positions.
	SyncBoundWidgetsToModel();
}

void UStratOptionsWidget::NativeDestruct()
{
	// UNBOUND IN THE SAME SHAPE IT WAS BOUND. `RemoveDynamic` on an unbound delegate is a no-op,
	// so the null guards here are about the POINTER and not about the binding.
	if (MasterSlider != nullptr)
	{
		MasterSlider->OnValueChanged.RemoveDynamic(
			this, &UStratOptionsWidget::HandleMasterSliderChanged);
	}

	if (SfxSlider != nullptr)
	{
		SfxSlider->OnValueChanged.RemoveDynamic(
			this, &UStratOptionsWidget::HandleSfxSliderChanged);
	}

	if (MusicSlider != nullptr)
	{
		MusicSlider->OnValueChanged.RemoveDynamic(
			this, &UStratOptionsWidget::HandleMusicSliderChanged);
	}

	if (BackButton != nullptr)
	{
		BackButton->OnClicked.RemoveDynamic(this, &UStratOptionsWidget::HandleBackClicked);
	}

	Super::NativeDestruct();
}

void UStratOptionsWidget::SyncBoundWidgetsToModel()
{
	// SEE THE DECLARATION, WHICH NOW RECORDS WHAT `USlider::SetValue` ACTUALLY DOES -- it
	// re-broadcasts `OnValueChanged`, measured, so this is a real loop and not a hypothetical
	// one. THIS early return is the RECURSION BRAKE and is not the line that makes "a sync
	// cannot produce a commit" true; the three handler guards below are that line. Deleting
	// this one alone is green across the whole suite and deleting those three reddens three
	// clauses, which is the measurement that tells the two apart.
	if (bSyncingBoundWidgets)
	{
		return;
	}

	TGuardValue<bool> Guard(bSyncingBoundWidgets, true);

	if (MasterSlider != nullptr)
	{
		MasterSlider->SetValue(Model.MasterVolume);
	}

	if (SfxSlider != nullptr)
	{
		SfxSlider->SetValue(Model.SfxVolume);
	}

	if (MusicSlider != nullptr)
	{
		MusicSlider->SetValue(Model.MusicVolume);
	}

	// ONE FIELD, ONE DRAWN NUMBER. `T-UI-03`'s clause: these are assignments of an `FText` the
	// model already holds, never `FText::AsNumber` and never a format string. The only percent
	// arithmetic in this module is in `StratBuildAudioOptionsModel` above.
	if (MasterValueText != nullptr)
	{
		MasterValueText->SetText(Model.MasterVolumeText);
	}

	if (SfxValueText != nullptr)
	{
		SfxValueText->SetText(Model.SfxVolumeText);
	}

	if (MusicValueText != nullptr)
	{
		MusicValueText->SetText(Model.MusicVolumeText);
	}
}

void UStratOptionsWidget::HandleMasterSliderChanged(const float InValue)
{
	// A SYNC IN PROGRESS IS NOT A PLAYER. `USlider::SetValue` DOES re-broadcast -- the
	// declaration carries the measurement -- so this is the line that stops a seed from being
	// recorded as a commit, which is the exact distinction `PushAudioOptions`'s block calls the
	// easiest thing in this file to get wrong. It is load-bearing on the shipped path rather
	// than belt-and-braces: deleting these three guards reddens three clauses.
	if (bSyncingBoundWidgets)
	{
		return;
	}

	SetMasterVolume(InValue);
}

void UStratOptionsWidget::HandleSfxSliderChanged(const float InValue)
{
	if (bSyncingBoundWidgets)
	{
		return;
	}

	SetSfxVolume(InValue);
}

void UStratOptionsWidget::HandleMusicSliderChanged(const float InValue)
{
	if (bSyncingBoundWidgets)
	{
		return;
	}

	SetMusicVolume(InValue);
}

void UStratOptionsWidget::HandleBackClicked()
{
	// THE CLICK CUE IS EMITTED FROM `StratPlay`, NOT HERE, AND THE ABSENCE IS DELIBERATE.
	// `StratSoundClick` takes a `UObject*` world context and reaches `UStratSoundDirector`,
	// which is a `StratPlay` world subsystem this module cannot name. The six existing cue
	// sites are all controller-side for that reason; a seventh here would need a route this
	// arrow does not permit. DISCHARGED BY the owner emitting it on `OnOptionsDismissed`.
	OnOptionsDismissed.Broadcast();
}
