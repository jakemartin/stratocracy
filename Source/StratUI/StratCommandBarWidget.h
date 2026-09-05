// GDD §2.11.2 / §2.11.5 / T-UI-03 -- the Blueprint-reachable surface for the on-screen
// command bar: §2.11.5's BUILD control and §2.11.2's END TURN control.
//
// WHAT GAP THIS CLOSES, AND IT IS THE PLAYER'S OWN REPORT RATHER THAN AN AUDIT'S. After
// playing the prototype the player said there is no on-screen way to act: every command is a
// keyboard binding -- `B` toggles §2.11.5's menu, End Turn is `Enter`/`Space` -- and nothing
// on screen says either exists. §2.11.2's persistent HUD was planned as W3 on the recorded
// wave plan ("Fame pool, +X/turn, purse, End Turn, idle-unit count") and never built. This
// file is the END TURN half of W3 finally landing, plus the BUILD control, which is new.
//
// IT IS ALSO THE SAME GAP `StratInfoPanelWidget.h` AND `StratGuidanceWidget.h` EACH OPEN BY
// RECORDING: a projection that is built, correct, and reachable by nothing. Without a
// Blueprint-accessible property of the view type, a WBP authored against
// `FStratCommandBarView` would be INERT -- which is what happened to three assets authored
// against `FStratGuidanceView` before `UStratGuidanceWidget` existed. This class is where the
// value ARRIVES.
//
// ---------------------------------------------------------------------------
// NO COMPOSE STEP AND NO `FStratCommandBarModel`, UNLIKE THE INFO PANEL. SAID, BECAUSE THE
// MISSING STEP READS AS AN OMISSION.
// ---------------------------------------------------------------------------
//
// `UStratInfoPanelWidget` carries `FStratInfoPanelModel` and `StratComposeInfoPanelModel`
// because §2.11.2's `yours` / `neutral` / `enemy` clause is VIEWER-RELATIVE and
// `FStratInfoPanelView` deliberately carries a SIDE rather than a boolean, so somebody has to
// resolve `HexOwner` against `ViewingSide` and it must not be a graph.
//
// NOTHING ON THIS SURFACE IS LEFT UNRESOLVED IN THAT WAY. `StratDecorateCommandBar` already
// resolved every viewer-relative question against `Model.ViewingSide` when it wrote the
// block: `bShowBuildButton` is the answer to "is the focused factory this seat's",
// `bEndTurnSuggested` is the answer to "has this seat finished", and `bEndTurnDimmed` and
// `EndTurnHoverText` are declared copies of the guidance block. There is no second operand
// left for a compose to take. Adding one would be a second author of a resolution that was
// already made once, which is the defect `FStratInfoPanelModel`'s own "TWO MEMBERS AND NOT A
// COMPOSED MODEL" note describes from the cache's side.
//
// SO THE CONDITION THAT WOULD CREATE A COMPOSE STEP HERE IS NAMED RATHER THAN LEFT OPEN: a
// field on `FStratCommandBarView` that carried a SIDE, an id, or any other value whose
// meaning depends on who is looking. There is none today and the decorator's own block
// explains why each field is a boolean or a copy instead.
//
// WHAT THIS CLASS DECIDES: NOTHING. It holds one value and announces that the value changed.
// It chooses no words -- `BUILD`, `END TURN`, the highlight, the dim, the layout and the
// anchors are all the WBP's -- makes no comparison, computes no number and decides no
// visibility. A WBP binds the BUILD control's visibility to `bShowBuildButton`, ONE BOOL AND
// NO CONJUNCTION, exactly as the directive strip binds to `bActive` and the info panel to
// `bHasHex`.
//
// T-UI-03 IS WHY THE THREE END-TURN BITS ARE THREE FIELDS AND NOT ONE PLUS A GRAPH. That
// clause forbids widget-side arithmetic, comparison and boolean conjunction. A graph spelling
// `CountViewingSideUnitsAbleToAct == 0` would be a comparison; a graph spelling
// `bEndTurnSuggested AND NOT bEndTurnGated` would be a conjunction; a graph reading the
// suggestion off this model and the gate off `Model.Guidance` would be reading one control
// from two sources. `StratDecorateCommandBar` resolves all three in C++ and this class
// carries the finished answers. See `FStratCommandBarView`'s own block, which is where those
// decisions are recorded.
//
// THE IDLE NUMBER, IF THE WBP DRAWS ONE, COMES FROM
// `UStratViewModelLibrary::CountViewingSideUnitsAbleToAct` AND NOT FROM A FIELD HERE.
// `StratViewModelLibrary.h` argues that at length: a count field on the model would be "the
// first piece of ARITHMETIC INSIDE THE MODEL". The widget calls a named `BlueprintPure` whose
// derivation is written down and pinnable, which is not the same thing as summing a list in a
// graph. That is also the single-authoring guarantee for the highlight -- the decorator
// computes the suggestion bit OVER that same function, so the number the HUD prints and the
// highlight it draws cannot disagree.
//
// WHY THIS HEADER DOES NOT INCLUDE StratBridge.h, and must never start to. It declares a
// `UCLASS`, so UHT parses it; `StratBridge.h` includes the vendored `strat` headers and is
// safe only where no reflected type is declared. Nothing here needs the bridge in any case --
// this class asks no rules question and holds no state but the pushed value.
//
// WHAT IS DELIBERATELY NOT IN THIS FILE.
// - NO VERB. Nothing here opens a menu, closes one, or ends a turn. The BUILD control's
//   `OnClicked` casts to `AStratPlayerController` and calls
//   `OpenProductionMenuAtFocusedFactory`; END TURN calls `RequestEndTurn`; the production
//   menu's exit calls `CloseProductionMenu`. All three are `BlueprintCallable` verbs on the
//   controller, where the input path lives, and a verb here would be a second route into a
//   sequence whose ordering is written down in exactly one place.
//
//   [AMENDED 2026-09-05 -- AND THE AMENDMENT IS AN EXCEPTION TO THE SENTENCE ABOVE RATHER
//   THAN A RESTATEMENT OF IT, WRITTEN HERE SO THAT A READER MEETS IT WITH THE RULE INSTEAD OF
//   DISCOVERING IT IN THE `.cpp`.] `OptionsButton` below is bound NATIVELY, in
//   `NativeConstruct`, which the three controls above are not. The rule the sentence states is
//   unchanged and this class STILL DECIDES NOTHING AND CALLS NO VERB -- `HandleOptionsClicked`
//   broadcasts `OnOptionsRequested` and returns; every question about what that means is
//   answered in `StratPlay`, one module over, by whatever bound the delegate.
//
//   WHAT FORCED IT, MEASURED RATHER THAN PREFERRED. The three controls above are wired in the
//   WBP's EVENT GRAPH, and this project has no route to author a widget graph: the editor lane
//   holds no Python or Lua API that can add a graph node to a `UWidgetBlueprint`, measured
//   2026-09-05 by the only agent that can drive the editor at all. So a button added to
//   `WBP_CommandBar`'s tree by the only available instrument arrives with NO `OnClicked`
//   WIRING AND NO WAY TO ACQUIRE ANY. A native bind in a C++ parent is the one remaining
//   route, and the same forcing produced `UStratOptionsWidget`'s four native binds and
//   `UStratShellMenuWidget` in `StratPlay`.
//
//   WHY THE CONTRACT IS NOT SIMPLY AMENDED TO "NATIVE BINDS ARE FINE". Because the reason for
//   it was never that native binds are distasteful -- it is that a VERB here would be a second
//   route into a sequence written down in one place, and that reason survives intact. The
//   distinction the contract now draws is between BINDING an input and DECIDING what it means.
//   This class does the first and still refuses the second. The day the editor lane can author
//   a widget graph, `OptionsButton` may go back to the graph and this exception may be
//   deleted; nothing below depends on it staying.
//
//   IT CANNOT CALL `AStratPlayerController::RequestOptionsScreen` DIRECTLY AND THE ARROW IS
//   WHY. That verb is in `StratPlay`; this module is `StratUI`; `StratPlay -> StratUI` and
//   never back, exactly as `StratUI.Build.cs` states. So the request leaves by a
//   `BlueprintAssignable` delegate on `UStratOptionsWidget::OnOptionsDismissed`'s precedent,
//   and `UStratMatchSubsystem` -- which already reaches this widget through
//   `AStratScoreboardHUD::CommandBar` on every `ApplyView` -- is what binds it and what calls
//   the controller. The brief that asked for this named a direct call; the direct call does
//   not compile.
// - NO `TSubclassOf` AND NO `/Game/` PATH. This class is a parent, not a spawner. The asset
//   reference that instantiates it lives on `AStratScoreboardHUD::CommandBarWidgetClass` as
//   an `EditDefaultsOnly` property set on a Blueprint default.
// - NO VISIBILITY DECISION AND NO `RemoveFromParent`. `bShowBuildButton` false is a STATE of
//   the command bar -- the ordinary state, most of the time -- and not the bar's absence. The
//   bar itself has the guidance strip's lifetime: created at `BeginPlay`, alive all match.
// - NO ARITHMETIC AND NOTHING TO DO IT WITH. No `+`, no `-`, no `==`.
//
// IRREVERSIBLE ONCE AN ASSET DERIVES FROM THIS. A Widget Blueprint reparented onto this class
// bakes `/Script/StratUI.StratCommandBarWidget` into its own bytes. StratUI and not StratPlay
// is correct for `UStratInfoPanelWidget`'s stated reason: `FStratCommandBarView` lives in
// StratUI, StratPlay depends on StratUI, and UMG is already a dependency of this module and
// of no other. CONFIRM THE PARENT MODULE BEFORE CREATING THE ASSET -- deciding it afterwards
// costs asset surgery.
//
// NO `.Build.cs` CHANGE WAS NEEDED FOR THIS FILE, and that is worth stating because a new
// widget class looks like it needs one. `StratUI.Build.cs` already carries private `UMG`,
// `Slate` and `SlateCore`; nothing new is named here.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "StratViewModel.h"

#include "StratCommandBarWidget.generated.h"

class UButton;

/**
 * Fired when the command bar's OPTIONS control is clicked. Carries nothing.
 *
 * NO PAYLOAD, AND THE EMPTINESS IS THE POINT. The click is an intention and not a value; the
 * only thing a binder needs to know is that it happened. A payload -- the model, the viewing
 * side, anything -- would be this class handing over a fact it does not own, and every fact it
 * holds is already readable off `Model` by anyone who has the pointer.
 *
 * IT ANNOUNCES A REQUEST AND NOT AN OUTCOME. Nothing here knows whether the options screen
 * opened; `UStratShellSubsystem::IsRoutePermitted` decides that a module away, and
 * `AStratPlayerController::RequestOptionsScreen` returns the refusal sentence to whoever asked.
 * A delegate named `OnOptionsOpened` would be a claim this class is in no position to make --
 * the same refusal `OnOptionsDismissed` makes on the volume screen's own back button.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStratCommandBarOptionsRequested);

/**
 * §2.11.2's persistent command bar: the BUILD control and the END TURN control.
 *
 * Holds the pushed block and announces that it changed. It reads no `strat` type, chooses no
 * words, makes no comparison of its own and decides no visibility -- see this file's header
 * block for why each of those is the acceptance criterion rather than a style preference.
 *
 * Abstract and Blueprintable because the WBP_ asset derives from it and does the layout.
 * This class draws nothing.
 */
UCLASS(Abstract, Blueprintable, meta = (DisplayName = "Strat Command Bar"))
class STRATUI_API UStratCommandBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Replaces `Model` and fires `OnCommandBarRefreshed`.
	 *
	 * NOT A UFUNCTION, and for `UStratInfoPanelWidget::PushInfoPanel`'s reasoning -- which
	 * is `UStratGuidanceWidget::PushGuidance`'s -- cited rather than re-argued. Every
	 * argument here is reflectable, so nothing FORCES this to be plain C++. It is plain C++
	 * so that Blueprint has no way to write a command-bar value at all. Reflecting it would
	 * let a graph push a fabricated `bShowBuildButton` -- a BUILD control over a factory this
	 * seat does not hold -- or a fabricated `bEndTurnSuggested`, into the two surfaces the
	 * player uses to act, and the bar must be readable off `FStratViewModel::CommandBar`
	 * alone or T-INT-05's "rebuild the screen from the view model" is a claim rather than a
	 * fact.
	 *
	 * VOID, AND IT CANNOT REFUSE. It is handed a finished value; there was no build here to
	 * decline, because `StratDecorateCommandBar` has no failure channel.
	 *
	 * ASSIGNS UNCONDITIONALLY, including when nothing is focused and nothing is suggested.
	 * That is the state the bar spends most of its life in; skipping the assignment on it
	 * would leave a BUILD control on screen after the player clicked away -- the delta-shaped
	 * thinking `UStratMatchSubsystem::ApplyView` refuses at the other end of this same call.
	 * No equality test against the previous value either, for the same reason.
	 */
	void PushCommandBar(const FStratCommandBarView& InCommandBar);

	/**
	 * The last pushed block. Everything the WBP draws comes from here.
	 *
	 * READ-ONLY TO BLUEPRINT ON PURPOSE. `PushCommandBar` is the only writer. A writable
	 * property would be the same second-author problem as a reflected setter, arrived at from
	 * a different direction.
	 *
	 * TRANSIENT because it is a projection of live match state and means nothing on disk; a
	 * saved copy would be a second, staler statement about a factory the player has since
	 * lost.
	 */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|CommandBar")
	FStratCommandBarView Model;

	/**
	 * Fired after `Model` is replaced.
	 *
	 * The block is passed as an argument as well as being readable off the widget, for the
	 * reason `OnScoreboardRefreshed`, `OnGuidanceRefreshed`, `OnInfoPanelRefreshed` and
	 * `OnMatchResultRefreshed` all give: so a graph has no motive to cache its own copy, a
	 * cached copy being a value that can go stale.
	 *
	 * FIRED ON EVERY PUSH, NOT ONLY ON A CHANGE. A fire conditioned on a change would need
	 * this class to hold what the previous block was; a graph that wants edge behaviour --
	 * an animation on the END TURN highlight appearing, say -- compares against what it drew,
	 * in the one place that already knows.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Stratocracy|CommandBar")
	void OnCommandBarRefreshed(const FStratCommandBarView& InCommandBar);

	// ---- §2.11.5's OPTIONS control, reached from inside a match --------------------------
	// See this file's header block for why this one control is bound natively while BUILD and
	// END TURN are wired in the graph, and for why the request leaves by a delegate instead of
	// by the direct call the brief asked for.

	/**
	 * Broadcast by `HandleOptionsClicked`, once per click, whatever comes of it.
	 *
	 * `BlueprintAssignable` SO THAT A GRAPH CAN BIND IT TOO, on `OnAudioOptionsCommitted`'s
	 * reasoning: the delegate is the sanctioned outward route, and a second binder is not a
	 * second author of anything, because this class produces no value for either of them to
	 * disagree about.
	 *
	 * NOTHING IN THIS MODULE BINDS IT. `UStratMatchSubsystem` does, in `StratPlay`. If that
	 * binding is ever removed, this control becomes a button that broadcasts into an empty
	 * delegate and nothing on screen says so -- which is the same inert-surface condition this
	 * file's header records `FStratGuidanceView` having lived in, and it is why the binder's
	 * side is pinned by a clause rather than left to inspection.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Stratocracy|CommandBar")
	FStratCommandBarOptionsRequested OnOptionsRequested;

protected:
	/**
	 * §2.11.5's OPTIONS control on the in-match bar.
	 *
	 * `BindWidgetOptional` AND NOT `BindWidget`, AND THE CHOICE IS ABOUT THE SHIPPED ASSET
	 * RATHER THAN ABOUT THIS CONTROL'S IMPORTANCE. `WBP_CommandBar` already derives from this
	 * class and has no such button in its tree; `BindWidget` is enforced by the Widget
	 * Blueprint compiler, so a hard bind would turn the entire command bar -- BUILD, END TURN
	 * and all -- into a compile error the moment this class lands, and would stay one until an
	 * asset pass caught up. The two changes are in two different lanes and cannot be atomic.
	 *
	 * THIS IS THEREFORE THE ONE PLACE IN THE PROJECT WHERE `BindWidgetOptional` DOES NOT MEAN
	 * "ONLY DISPLAYS A VALUE". `UStratOptionsWidget.h` states the aesthetic -- produces a value
	 * means `BindWidget`, displays one means `BindWidgetOptional` -- and this member produces
	 * one and is optional anyway. Said here rather than left to be inferred, because a reader
	 * applying that rule to this member would read the optionality as a claim that the button
	 * is decorative. DISCHARGED BY `WBP_CommandBar` acquiring the button: at that point this
	 * may become `BindWidget` in a pass that touches nothing else.
	 *
	 * `protected` FOR `UStratOptionsWidget::MasterSlider`'S STATED REASON: so a test double can
	 * plant one. `BindWidget*` is indifferent to access, being resolved by reflection at
	 * Blueprint compile time.
	 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> OptionsButton;

	/** Binds `OptionsButton`. Safe on a native subclass with no widget tree: the pointer is
	 *  null there and the bind is guarded, exactly as `UStratOptionsWidget::NativeConstruct`
	 *  records for its own four. */
	virtual void NativeConstruct() override;

	/** Unbinds in the same shape. See `UStratOptionsWidget::NativeDestruct`. */
	virtual void NativeDestruct() override;

	/**
	 * Broadcasts `OnOptionsRequested`. Decides nothing and calls no verb.
	 *
	 * A `UFUNCTION` BECAUSE `FOnButtonClickedEvent` IS A DYNAMIC MULTICAST and can bind nothing
	 * else -- the same mechanical forcing `UStratOptionsWidget::HandleBackClicked` records.
	 *
	 * NO CLICK CUE HERE, AND THE ABSENCE IS DELIBERATE AND WOULD OTHERWISE BE A DOUBLE.
	 * `AStratPlayerController::RequestOptionsScreen` already emits `StratSoundClick` at entry,
	 * unconditionally, before any check -- it is the sixth site of that rule -- and that
	 * function is what this broadcast reaches. A cue here would play two clicks on one press.
	 * `HandleBackClicked` gives the other half of the reason: the cue helper is `StratPlay`'s
	 * and this module cannot name it.
	 */
	UFUNCTION()
	void HandleOptionsClicked();
};
