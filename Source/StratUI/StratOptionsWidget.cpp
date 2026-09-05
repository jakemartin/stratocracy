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

#include "StratOptionsWidget.h"

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

	// REFRESH BEFORE COMMIT. The thumb must be where the model is before anything acts on it --
	// see the declaration. Today nothing is bound to the commit at all, and that ordering is
	// what makes the screen still correct in that state.
	OnAudioOptionsRefreshed(Model);
	OnAudioOptionsCommitted.Broadcast(Model);
}

void UStratOptionsWidget::SetSfxVolume(const float InVolume)
{
	Model = StratBuildAudioOptionsModel(Model.MasterVolume, InVolume, Model.MusicVolume);

	OnAudioOptionsRefreshed(Model);
	OnAudioOptionsCommitted.Broadcast(Model);
}

void UStratOptionsWidget::SetMusicVolume(const float InVolume)
{
	Model = StratBuildAudioOptionsModel(Model.MasterVolume, Model.SfxVolume, InVolume);

	OnAudioOptionsRefreshed(Model);
	OnAudioOptionsCommitted.Broadcast(Model);
}
