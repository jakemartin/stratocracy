// GDD Sec 2.11.5 ("No settings screen beyond volume + resolution is budgeted") -- the volume
// screen's model and its widget base. The sixth `UUserWidget` subclass in this module and the
// first one that is a CONTROL rather than a READOUT.
//
// WHAT GAP THIS CLOSES. `UStratSoundDirector::CommitVolumes` writes `UStratAudioSettings` and
// `EStratShellRoute::Options` routes a menu to a screen, and until this file there was no screen
// -- three persisted gains reachable by a Blueprint author, a console and a fixture, and by no
// player. Sec 2.11.5's budget for a settings screen is exactly "volume + resolution"; this is the
// first of those two, and the other is deliberately not here (see below).
//
// IT IS A CONTROL, WHICH IS THE ONE THING THE OTHER FIVE WIDGETS IN THIS MODULE ARE NOT, AND
// EVERY DECISION BELOW FALLS OUT OF THAT. The scoreboard, the info panel, the command bar, the
// guidance strip and the result screen are all fed a model by something above them and give
// nothing back. This one is fed a model AND produces a value. So it needs a second direction,
// and the second direction is where a widget usually starts computing:
//   - THE SETTERS CLAMP AND REBUILD, AND THE MODEL IS WHAT THE SCREEN DRAWS. A slider that drew
//     its own thumb position from its own local state would be a number on screen that is not a
//     model field, which is `T-UI-03`'s clause with the arrow reversed. Every setter's last act
//     is to replace `Model` and fire `OnAudioOptionsRefreshed`, so the WBP redraws from the model
//     it was just handed -- exactly as the read-only widgets do.
//   - THE PERCENT STRINGS ARE FIELDS AND NOT `Volume * 100` NODES. That multiplication is
//     arithmetic, it is the single most likely piece of arithmetic to appear in a volume WBP, and
//     `StratBuildAudioOptionsModel` does it once in C++ so the graph never has to. It is the
//     same move `FStratMatchResultModel::bViewerWon` makes about `Winner == ViewingSide`.
//   - NOTHING HERE APPLIES A VOLUME. `UGameplayStatics::SetSoundMixClassOverride` needs a
//     `USoundMix` and a `USoundClass`, both of which arrive on `UStratSoundBank` in `StratPlay`,
//     and the arrow runs `StratPlay -> StratUI` and never back. So this class cannot apply
//     anything even if it wanted to, which is a structural guarantee rather than a discipline.
//
// HOW THE VALUE GETS OUT, AND IT IS A DELEGATE BECAUSE IT CANNOT BE A CALL. `OnAudioOptionsCommitted`
// is a `BlueprintAssignable` multicast carrying the whole model; a `StratPlay`-side owner binds it
// to `UStratSoundDirector::CommitVolumes` and the join costs no `#include` in either direction.
// The two shapes that were rejected:
//   - THE WBP CALLING `Get World Subsystem -> Commit Volumes` DIRECTLY. It works, and it makes
//     the asset the thing that decides when a value is committed. Then the tested path and the
//     shipped path are different, and the difference lives in a `.uasset` no clause reads.
//   - A `BlueprintImplementableEvent` PER CHANNEL. Three events, each carrying one float, each
//     wired by hand -- three chances to wire the music slider to the SFX gain, with nothing able
//     to tell. One event carrying one model has no such degree of freedom.
//
// THE BINDER DOES NOT EXIST YET, AND THAT IS SAID HERE RATHER THAN LEFT TO BE DISCOVERED,
// BECAUSE THIS PROJECT HAS A NAMED DEFECT FOR IT: "a reflected verb with no caller reads as
// built". As of this file's first commit nothing binds `OnAudioOptionsCommitted` and no WBP
// derives from this class, so a player who reaches `EStratShellRoute::Options` today sees
// nothing. What exists is the whole path from a slider value to a written slot, with every step
// reachable by a clause. DISCHARGED BY the WBP and an owner -- `AStratShellHUD` is the obvious
// one, since it already creates the menu on `MenuWidgetClass` and already holds the shell -- that
// creates this widget on `IsOptionsPanelOpen` and binds the delegate. That owner is a later
// phase's and is not this file's to write.
//
// NOT IN THIS ROUND, with reasons:
// - RESOLUTION, WHICH IS THE OTHER HALF OF Sec 2.11.5's OWN SENTENCE. It is `UGameUserSettings`:
//   the engine owns it, persists it in `GameUserSettings.ini`, and enumerates the modes a display
//   actually supports. A `TArray<FIntPoint>` composed here would be a second, worse copy of that
//   enumeration, and `UStratAudioSettings`'s block refuses the same duplication from the save
//   side. When it lands it is rows on this screen and a second model section, not a second route.
// - AUDIO OUTPUT DEVICE, SUBTITLES, MASTER MUTE. Not budgeted, and the mute in particular is
//   `MasterVolume == 0` -- a second representation of it would be two answers to "is this
//   silent", which `EStratSoundDisposition` already refuses one layer down.
// - A "RESET TO DEFAULTS" BUTTON. It is one line (`SetAll(1, 1, 1)`) and it is left out because
//   `FStratAudioOptionsModel`'s defaults ARE unity, so the button would be the only control on
//   this screen whose meaning a reader has to look up. A WBP that wants one calls the three
//   setters.
// - LOCALIZATION. `FText::FromString` throughout, matching `StratResultLineFor` and
//   `StratGuidedOpening::DirectiveTextFor`; see `FStratGuidanceView`'s block on why localization
//   is deliberately not started.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "StratOptionsWidget.generated.h"

/**
 * Everything the volume screen draws, in one value.
 *
 * THE THREE GAINS AND THE THREE STRINGS THAT DISPLAY THEM. The pairing is deliberate duplication,
 * on `FStratScoreboardRow`'s stated precedent: the number is what a clause compares and the
 * string is what the screen shows, and both are set from one read of one value so they cannot
 * disagree. A screen that had only the float would compute the string; a screen that had only the
 * string could not be asserted numerically.
 *
 * EVERY FIELD IS `BlueprintReadOnly`. `UStratOptionsWidget`'s three setters are the only writers,
 * so a graph cannot fabricate a displayed gain that no clamp ever saw.
 */
USTRUCT(BlueprintType)
struct FStratAudioOptionsModel
{
	GENERATED_BODY()

	/** `[0, 1]`, guaranteed by `StratClampVolume` -- see `StratBuildAudioOptionsModel`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Options")
	float MasterVolume = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Options")
	float SfxVolume = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Options")
	float MusicVolume = 1.0f;

	/** `MasterVolume` as the screen says it -- "100%", "45%", "0%". THE MULTIPLICATION HAPPENS
	 *  HERE AND NOT IN A GRAPH; see the file header. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Options")
	FText MasterVolumeText;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Options")
	FText SfxVolumeText;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Options")
	FText MusicVolumeText;
};

/**
 * The model for three gains. THE ONLY PLACE a volume becomes a string in this project.
 *
 * A FREE FUNCTION AND NOT A METHOD ON THE WIDGET, on `StratBuildScoreboardModel`'s stated reason:
 * it draws nothing, holds no state, needs no Slate in existence, and can therefore be swept over
 * every interesting input in a `-nullrhi` clause with no widget constructed at all.
 *
 * IT CLAMPS THROUGH `StratClampVolume` AND DOES NOT RESTATE THE RANGE. That function is the one
 * authority and `UStratAudioSettings::Sanitize` in `StratPlay` is its other caller; the header on
 * that declaration records why the rule has to live below both modules.
 *
 * THE PERCENT IS `FMath::RoundToInt(Volume * 100)`, WHICH IS THE ARITHMETIC THIS FUNCTION EXISTS
 * TO CONTAIN. It rounds rather than truncating, so a gain of 0.999 reads "100%" instead of "99%"
 * -- a slider dragged to its stop must not report one short of full, and truncation is the way
 * that bug is usually written. The consequence, said rather than discovered: a gain of 0.996 also
 * reads "100%" while not being 1.0, so the STRING is not a faithful inverse of the float and no
 * caller should treat it as one. The float is the value; the string is the label.
 *
 * TOTAL AND INFALLIBLE. Every triple of floats has a model, including NaNs -- `StratClampVolume`
 * maps those to silence and states why.
 */
STRATUI_API FStratAudioOptionsModel StratBuildAudioOptionsModel(float InMasterVolume,
                                                                float InSfxVolume,
                                                                float InMusicVolume);

/**
 * Fired when a setter has accepted a value. Carries the whole model, never one float.
 *
 * A DYNAMIC MULTICAST SO A BLUEPRINT *AND* A `StratPlay`-SIDE OWNER CAN BOTH BIND. The shipped
 * binder is expected to be C++ (see the file header); the delegate is dynamic anyway so that an
 * asset is not locked out of a screen whose whole purpose is asset-facing.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStratAudioOptionsCommitted,
                                            const FStratAudioOptionsModel&, Model);

/**
 * Sec 2.11.5's volume screen.
 *
 * Holds a built model, pushes it at Blueprint, and reports every accepted change. It reads no
 * `strat` type, applies no gain, and performs no arithmetic -- see this file's header block for
 * why each of those is an acceptance criterion rather than a style preference.
 *
 * Abstract and Blueprintable because the WBP_ asset derives from it and does the layout.
 * `StratUI.Build.cs` records the irreversible part: the derived asset bakes
 * `/Script/StratUI.StratOptionsWidget` into itself, so this class's module and name are expensive
 * to change once the first asset exists.
 */
UCLASS(Abstract, Blueprintable, meta = (DisplayName = "Strat Options"))
class STRATUI_API UStratOptionsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Replaces `Model` wholesale and fires `OnAudioOptionsRefreshed`. Does NOT fire
	 * `OnAudioOptionsCommitted`.
	 *
	 * THE ASYMMETRY IS THE WHOLE CONTRACT AND IT IS THE EASIEST THING IN THIS FILE TO GET WRONG.
	 * This is how the screen is SEEDED -- an owner reads `UStratSoundDirector::GetAudioSettings`
	 * and pushes what the player chose last time. Firing the commit delegate here would make
	 * opening the screen write the slot, so a player who opened options and closed it again would
	 * have re-saved values they never touched, and any clause counting commits would count that
	 * one. Seeding is not choosing.
	 *
	 * NOT A `UFUNCTION`, on `UStratMatchResultWidget::PushMatchResult`'s reasoning: every argument
	 * is reflectable, so nothing forces this to be plain C++; it is plain C++ so that Blueprint
	 * has no way to seed a model that did not come from the settings object.
	 *
	 * IT CLAMPS, BECAUSE A CALLER CAN HAND IT ANYTHING. The model it stores is always built by
	 * `StratBuildAudioOptionsModel`, never assigned from the argument, so there is no path by
	 * which an out-of-range gain reaches `Model` and gets drawn.
	 */
	void PushAudioOptions(const FStratAudioOptionsModel& InModel);

	/**
	 * Set the master gain. Clamps, replaces `Model`, then fires BOTH events.
	 *
	 * `BlueprintCallable` BECAUSE A SLIDER IS THE CALLER, which is the one reason any writer in
	 * this module is reflected. The three setters are separate functions rather than one
	 * `SetVolumes(float, float, float)` because a WBP wires one slider to one node, and a
	 * three-argument setter would make each slider responsible for restating the other two --
	 * three chances per drag to write a stale value over a fresh one.
	 *
	 * REFRESH FIRES BEFORE COMMIT, AND THE ORDER IS NOT ARBITRARY. The screen should show the
	 * accepted value before anything acts on it, so that a commit which is slow, refuses, or
	 * (today) is bound to nothing at all cannot leave the thumb somewhere the model is not.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Options")
	void SetMasterVolume(float InVolume);

	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Options")
	void SetSfxVolume(float InVolume);

	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Options")
	void SetMusicVolume(float InVolume);

	/** The last model. Everything the WBP draws comes from here.
	 *
	 *  READ-ONLY TO BLUEPRINT ON PURPOSE, on `UStratMatchResultWidget::Model`'s reasoning: the
	 *  four writers above are the only ones, and a writable property would let a graph draw a
	 *  gain that no clamp had seen and no setter had reported. */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Options")
	FStratAudioOptionsModel Model;

	/**
	 * Fired after `Model` is replaced, by a seed OR by a setter. Redraw here.
	 *
	 * The model is passed as an argument as well as being readable off the widget, for the reason
	 * `OnScoreboardRefreshed`, `OnGuidanceRefreshed` and `OnMatchResultRefreshed` all give: so a
	 * graph has no motive to cache its own copy, a cached copy being a value that can go stale.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Stratocracy|Options")
	void OnAudioOptionsRefreshed(const FStratAudioOptionsModel& InModel);

	/**
	 * Fired after a SETTER, and never after `PushAudioOptions`. Bind this to persist and apply.
	 *
	 * THE COUNT OF THESE IS THE MEASURABLE THING A CLAUSE SHOULD PIN. "The screen shows 0.4" is a
	 * statement about `Model`; "the screen ASKED for 0.4 exactly once" is a statement about this
	 * delegate, and only the second can tell a seeded screen from a used one -- which is the
	 * distinction `PushAudioOptions` exists to preserve.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Stratocracy|Options")
	FStratAudioOptionsCommitted OnAudioOptionsCommitted;
};
