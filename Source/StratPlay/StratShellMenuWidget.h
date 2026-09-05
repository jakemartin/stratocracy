// GDD §2.11.5 -- the C++ parent for the title menu, and the OPTIONS row's only route onto it.
//
// WHAT GAP THIS CLOSES, IN THE WORDS THE GAP WAS RECORDED IN. `StratShellHUD.h` lists this class
// among the things it deliberately does not contain: *"A MENU WIDGET BASE CLASS. `WBP_TitleMenu`
// is a plain `UUserWidget` that binds to the subsystem's reflected surface by index; the slot
// below is typed `UUserWidget` so it accepts that asset as it stands. A `UStratShellMenuWidget`
// with `BindWidget` members would be the StratUI-side follow-up and it is a different lane's
// asset work."* This file is that class. **THE NAME IS TAKEN FROM THAT SENTENCE AND THE MODULE IS
// NOT** -- see the arrow argument below, which is the one finding in this file.
//
// AND THE GAP IS NOW LOAD-BEARING RATHER THAN TIDY. `EStratShellRoute` acquired a fifth
// enumerator, `Options`, in the audio milestone. `UStratShellSubsystem::BuildMenuModel` returns
// EVERY declared route ALWAYS, in declaration order -- its own block says the model "does not
// shrink with the facts" -- so `FStratShellMenuModel::Options` is now five rows long.
// `WBP_TitleMenu` has exactly four buttons, `Btn_0`..`Btn_3`, each with its own graph binding
// reading `Array_Get_Item` at a fixed index. So the fifth row is BUILT ON EVERY REFRESH AND
// DRAWN BY NOTHING: the volume screen the whole audio milestone exists to reach has no control
// that opens it at the title.
//
// ============================================================================================
// WHY THIS CLASS IS IN `StratPlay` AND NOT IN `StratUI`, WHICH IS WHERE EVERY OTHER WIDGET
// PARENT IN THIS PROJECT LIVES. THIS IS THE FINDING, AND IT REVERSES WHAT WAS ASKED FOR.
// ============================================================================================
//
// The request was for a widget class in `StratUI` that binds an `OptionsButton` natively to
// `UStratShellSubsystem::ExecuteRoute(EStratShellRoute::Options)`. **THAT CLASS CANNOT BE
// WRITTEN.** `UStratShellSubsystem` is declared in `StratPlay`; `StratPlay -> StratUI` publicly
// and never the reverse; `StratUI.Build.cs` states in terms that a reverse arrow "would let the
// bridge reach a widget". A `StratUI` class cannot name the shell subsystem, cannot name
// `EStratShellRoute`, and cannot call `ExecuteRoute`. The two escapes are a reflection lookup by
// class name -- untypeable and unfalsifiable -- or moving the shell into `StratUI`, which drags
// level travel and `UStratMatchSubsystem` with it. `StratOptionsPresenter.h` records the same
// boundary defeating the same shape of request one milestone earlier.
//
// SO THE CLASS GOES WHERE THE THING IT MUST CALL IS. And that is not a concession: it is
// `StratCommandBarWidget.h`'s OWN RULE APPLIED HONESTLY. That file argues it belongs in `StratUI`
// because *"`FStratCommandBarView` lives in StratUI"* -- a widget parent goes where its model
// lives. **`FStratShellMenuModel` and `FStratShellOption` live in `StratPlay`.** By that rule,
// stated for the opposite conclusion, this class belongs here. Nothing about the title menu is a
// projection of `FStratViewModel`; it draws no rules fact, no snapshot field and no scoreboard
// row, and `StratUI` has nothing it needs.
//
// WHAT IT COSTS, SAID PLAINLY. `Source/StratPlay/` now contains a `UUserWidget` subclass, which
// is a shape a reader will expect to find one module over. The `.Build.cs` cost is zero -- `UMG`
// is already a private dependency of this module and `StratOptionsPresenter.h` already includes a
// UMG header, both recorded in `StratPlay.Build.cs` -- so nothing measurable was paid. WHAT WOULD
// MOVE THIS CLASS TO `StratUI`: a shell surface reachable from `StratUI`, which today would mean
// the routes becoming a `StratUI`-side model that `StratPlay` executes. That is a larger change
// than this milestone, and it is not owed.
//
// ============================================================================================
// WHAT THIS CLASS DOES NOT DO, AND THE FIRST ONE IS THE WHOLE OF ITS RESTRAINT.
// ============================================================================================
//
// - IT DOES NOT TOUCH `Btn_0`..`Btn_3`. The four existing buttons keep their graph bindings and
//   their `Array_Get_Item` indices, untouched and unreferenced from here. Reparenting a
//   `UUserWidget` asset onto a subclass of `UUserWidget` leaves its widget tree and its event
//   graph exactly as they were. Adopting the other four would mean four more `BindWidget`
//   members whose names must match an asset nobody is about to re-author, and would put this
//   class in the way of a menu that already works.
// - IT DECIDES NO LABEL. `OptionsLabel`'s text is `FStratShellOption::Label` assigned across,
//   and never a literal. `BuildMenuModel` chooses the words -- that is where "Restart Match"
//   versus "New Match" is already decided -- and a literal here would be a second author of a
//   string the shell owns. The same for `bEnabled`: it is copied onto the button, not derived.
// - IT COMPUTES NOTHING. No comparison, no arithmetic, no conjunction. `T-UI-03`'s clause is
//   about the match HUD, but the discipline is the project's and this file honours it.
// - IT PLAYS NO CLICK CUE, AND THE ABSENCE IS DELIBERATE RATHER THAN FORGOTTEN. `StratSoundClick`
//   is a file-static in `StratPlayerController.cpp` taking an `AStratPlayerController*`, and the
//   title map runs a bare `APlayerController` -- `StratOptionsPresenter.h` records that
//   measurement and `AStratShellGameMode`'s constructor is where it is visible. There is no
//   controller here to emit from. The four sibling buttons emit none either, so a fifth that
//   clicked would be the odd one out on its own menu. DISCHARGED BY the title map acquiring a
//   controller class of ours, at which point all five want one and not just this one.
// - NO `TSubclassOf` AND NO `/Game/` PATH. `AStratShellHUD::MenuWidgetClass` is the
//   `EditDefaultsOnly` slot that names the asset, and it is typed `TSubclassOf<UUserWidget>`, so
//   **it accepts this class's subclasses with no property change and no asset re-pointing.**
//
// IRREVERSIBLE ONCE `WBP_TitleMenu` DERIVES FROM THIS. The asset bakes
// `/Script/StratPlay.StratShellMenuWidget` into its own bytes, exactly as
// `StratCommandBarWidget.h` and `StratUI.Build.cs` both warn. CONFIRM THE MODULE BEFORE
// REPARENTING -- the paragraph above is the argument, and moving the class afterwards costs
// asset surgery rather than a file rename.
//
// NO `.Build.cs` CHANGE WAS NEEDED FOR THIS FILE, and that is worth stating because a widget
// class in a non-widget module looks like it must need one. `UMG` is already on
// `StratPlay.Build.cs`'s private line for `StratShellHUD.cpp`'s `CreateWidget`, and that file's
// `RETRACTED>` block already records the day a header in this module first included a UMG
// header. No linker error was needed to justify anything here, because nothing new was named.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "StratShellMenuWidget.generated.h"

class UButton;
class UTextBlock;

/**
 * §2.11.5's title menu, as far as the OPTIONS row is concerned.
 *
 * A parent for `WBP_TitleMenu`. It adds one control to a menu it otherwise leaves entirely
 * alone; see this file's header block for what it deliberately does not adopt.
 *
 * Abstract and Blueprintable because the WBP_ asset derives from it and does the layout. This
 * class draws nothing and positions nothing.
 */
UCLASS(Abstract, Blueprintable, meta = (DisplayName = "Strat Shell Menu"))
class STRATPLAY_API UStratShellMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Re-reads the menu model and brings `OptionsLabel` and `OptionsButton` up to its
	 * `Options` row.
	 *
	 * `BlueprintCallable` SO THE ASSET CAN DRIVE THE CADENCE, which is the discharge
	 * `UStratShellSubsystem::GetMenuModel` names for its own cost: *"the WBP refreshing this on
	 * demand -- on construction, and on the events that can change the answer -- rather than
	 * binding it per frame"*. This class calls it once, from `NativeConstruct`. A graph that
	 * wants it again after an event calls it again; a property binding that calls it per frame
	 * pays the per-frame disk read that node's block describes, and this comment is the warning.
	 *
	 * IT LOOKS THE ROW UP BY `Route` AND NOT BY INDEX, WHICH IS THE OPPOSITE OF WHAT THE FOUR
	 * SIBLING BUTTONS DO AND IS DELIBERATE. `FStratShellMenuModel`'s own block says the list is
	 * assertable BY INDEX and that a clause should pin ordering that way -- that is advice to a
	 * TEST, whose job is to fail when the order moves. A SHIPPED widget wants the opposite: a
	 * sixth route inserted before `Options` must not silently relabel this button, and by-route
	 * lookup makes that impossible rather than merely unlikely. The four graph-bound buttons
	 * cannot be given the same protection without a graph, which is the same authoring limit
	 * that forced this class to exist.
	 *
	 * A MISSING ROW IS SILENT AND LEAVES BOTH CONTROLS UNTOUCHED. It cannot happen while
	 * `BuildMenuModel` emits every declared route, and if it ever does, the honest behaviour is
	 * to draw whatever the asset author put there rather than to blank a button.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Shell")
	void RefreshOptionsRow();

protected:
	/**
	 * §2.11.5's OPTIONS row on the title menu.
	 *
	 * `BindWidgetOptional` AND NOT `BindWidget`, for `UStratCommandBarWidget::OptionsButton`'s
	 * stated reason and one more of its own. The shared reason: `BindWidget` is enforced by the
	 * Widget Blueprint compiler, and `WBP_TitleMenu` has no such button today, so a hard bind
	 * would red the entire title menu the moment this class is used as its parent. The extra
	 * reason is that the reparent and the button are TWO editor steps in an order this class
	 * does not control -- an asset cannot be reparented onto a class whose hard bind it does not
	 * yet satisfy, so `BindWidget` would make the reparent itself impossible.
	 *
	 * IT IS OPTIONAL AND IT PRODUCES A VALUE, which is the exception to
	 * `UStratOptionsWidget.h`'s stated aesthetic and is called out for the same reason it is
	 * called out on the command bar: a reader applying that rule would read the optionality as a
	 * claim that this control is decorative, and it is the only control this class has.
	 * DISCHARGED BY the asset acquiring the button, after which this may become `BindWidget`.
	 *
	 * `protected` FOR `UStratOptionsWidget::MasterSlider`'S REASON: so a test double can plant
	 * one. `BindWidget*` is resolved by reflection at Blueprint compile time and is indifferent
	 * to access.
	 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> OptionsButton;

	/**
	 * The OPTIONS row's caption.
	 *
	 * `BindWidgetOptional` ON THE PROJECT'S ORDINARY READING THIS TIME: it only DISPLAYS a
	 * value. A menu whose fifth button carries its caption inside the button -- which is how
	 * `Btn_0`..`Btn_3` are built -- has no separate text block to bind, and the row is still
	 * clickable and still opens the volume screen without one.
	 *
	 * ITS TEXT IS `FStratShellOption::Label` AND IS NEVER COMPOSED HERE. One field, one drawn
	 * string, no expression between them.
	 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> OptionsLabel;

	/** Binds `OptionsButton` and calls `RefreshOptionsRow` once. Safe on a native subclass with
	 *  no widget tree: both pointers are null there and every use is guarded, exactly as
	 *  `UStratOptionsWidget::NativeConstruct` records for its own four. */
	virtual void NativeConstruct() override;

	/** Unbinds in the same shape. See `UStratOptionsWidget::NativeDestruct`. */
	virtual void NativeDestruct() override;

	/**
	 * Takes `EStratShellRoute::Options`.
	 *
	 * A `UFUNCTION` BECAUSE `FOnButtonClickedEvent` IS A DYNAMIC MULTICAST and can bind nothing
	 * else -- the mechanical forcing `UStratOptionsWidget::HandleBackClicked` records.
	 *
	 * IT CALLS `ExecuteRoute` DIRECTLY, WHICH IS WHAT THE OTHER FOUR BUTTONS DO FROM THE GRAPH,
	 * AND IT IS THE CONVENTION RATHER THAN A SHORTCUT. `EStratShellRoute::Options`'s own block
	 * gives the reason the route exists at all instead of being a method on `AStratShellHUD`:
	 * *"a WBP author would have had two wiring conventions and a clause would have had two
	 * surfaces to sweep"*. Reaching `RequestOptionsPanel` from here would reintroduce exactly
	 * that -- four buttons through the permission check and one around it. The in-match control
	 * goes through `AStratPlayerController::RequestOptionsScreen` instead, which is itself one
	 * more caller of `ExecuteRoute`; the two paths differ in who calls it and not in what is
	 * called.
	 *
	 * A REFUSAL IS LOGGED AND NOT PROPAGATED. A click has nowhere to return a reason to.
	 * `IsRoutePermitted` grants `Options` in every fact combination today, so a refusal here
	 * means the shell is missing or the route has acquired a precondition -- both worth a line.
	 */
	UFUNCTION()
	void HandleOptionsClicked();
};
