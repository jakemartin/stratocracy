// GDD Sec 2.11.5 -- the owner of the volume screen. The object that puts
// `UStratOptionsWidget` on the viewport when `UStratShellSubsystem::IsOptionsPanelOpen` says so,
// and takes it off again when it stops saying so.
//
// WHAT GAP THIS CLOSES, IN THE WORDS THE GAP WAS RECORDED IN. `StratShellSubsystem.h` states of
// its own `Options` route: *"on the tree as it stands, clicking the row runs
// `RequestOptionsPanel` and nothing appears"*, and names what would discharge it -- *"the options
// WBP and an owner that shows it on `IsOptionsPanelOpen`"*. `StratOptionsWidget.h` states the
// same debt from the other side: *"as of this file's first commit nothing binds
// `OnAudioOptionsCommitted` and no WBP derives from this class, so a player who reaches
// `EStratShellRoute::Options` today sees nothing"*. THIS FILE IS THE OWNER BOTH OF THOSE BLOCKS
// NAME. The WBP is the editor lane's and is not here.
//
// ============================================================================================
// WHY A `UWorldSubsystem` AND NOT `AStratShellHUD`, WHICH IS WHAT WAS ASKED FOR. The request was
// for the title HUD to hold a `TSubclassOf<UStratOptionsWidget>` and create the widget itself.
// That works for the title map and covers exactly half the requirement, and the half it misses
// is the half that was asked for in the same breath -- THE SAME SCREEN REACHABLE DURING A MATCH.
//
// THE ARROW IS WHAT DECIDES IT, AND IT IS WORTH STATING AS A FINDING RATHER THAN A DETAIL. The
// match HUD is `AStratScoreboardHUD`, which lives in **`StratUI`**. `UStratShellSubsystem` lives
// in **`StratPlay`**. `.agents/ue-project-context.md` draws that arrow as `StratUI -> StratPlay`
// -- StratPlay depends on StratUI publicly and never the reverse, and `StratUI.Build.cs` states
// in terms that the reverse "would let the bridge reach a widget". **So the match HUD cannot
// name the shell subsystem, cannot read `IsOptionsPanelOpen`, and cannot be the owner.** No
// amount of care inside `AStratScoreboardHUD` fixes that; it is a module boundary, and the only
// ways past it are a reflection lookup by class name (untypeable, unfalsifiable) or moving the
// shell into `StratUI` (which would drag level travel and `UStratMatchSubsystem` with it).
//
// THE PLAYER CONTROLLER WAS THE OTHER CANDIDATE AND IT FAILS ON A MEASUREMENT, NOT ON TASTE.
// `AStratPlayerController` is in `StratPlay` and could name both sides. But
// `AStratGameMode`'s constructor sets `PlayerControllerClass = AStratPlayerController::
// StaticClass()` and `AStratShellGameMode`'s constructor sets `DefaultPawnClass` and the save
// slot AND NOTHING ELSE -- the title map runs a bare `APlayerController`. So the controller
// route covers the match and not the title, which is the first candidate's failure mirrored.
//
// A WORLD SUBSYSTEM IS THE ONE OWNER SHAPE THAT EXISTS IN BOTH WORLDS WITHOUT ASKING ANY ASSET
// TO OPT IN. Every `Game`/`PIE` world gets one, whatever its GameMode, HUD or controller class
// is; `UStratSoundDirector` in this module is the same shape for the same reason, and its
// `DoesSupportWorldType` is copied here rather than invented.
//
// THE COST, SAID PLAINLY BECAUSE IT IS REAL AND BECAUSE THIS PROJECT HAS A NAMED DEFECT FOR
// EXACTLY IT: a subsystem has no details panel, so `OptionsWidgetClass` must be handed in, and
// the two GameModes that hand it in are two Blueprint defaults naming the same asset with
// nothing asserting they agree. `StratShellSubsystem.h` refuses precisely this shape for its two
// level destinations -- *"one shared `FStratShellLevels` struct on both, which would put the
// SAME two asset references in TWO Blueprint defaults with nothing asserting they agree"* -- and
// that refusal was available to it because each GameMode holds the ONE destination it can route
// to. It is not available here: both worlds need the SAME widget class, so there is no split
// that makes the duplication go away. It is taken on as a debt with a named discharge (a shipped
// -Blueprint parity clause, on `StratShellBlueprintSlotParity.cpp`'s existing precedent) rather
// than dressed up as a design.
//
// ============================================================================================
// IT RECONCILES; IT DOES NOT REACT. `ReconcileOptionsPanel` reads `IsOptionsPanelOpen` and makes
// the viewport match it, and every path into this class ends in that one function -- the world's
// `BeginPlay`, the shell's delegate, and a direct Blueprint call. THE CONSEQUENCE IS THE POINT:
// there is no ordering in which this class can be left showing a panel the flag says is closed,
// because nothing here acts on an event's CONTENT. A missed broadcast costs a late reconcile and
// never a wrong screen. That is the same discipline the board and unit actors are built on.
//
// AND IT RECONCILES ONCE AT `OnWorldBeginPlay`, WHICH IS NOT REDUNDANT. `bOptionsPanelOpen` is a
// `UGameInstanceSubsystem` member and SURVIVES LEVEL TRAVEL; its own declaration says so and
// says the fix is *"a `CloseOptionsPanel` in that owner's `EndPlay`"*. This class does exactly
// that in `Deinitialize`, and the opening reconcile is the belt to that braces -- a world that
// begins with the flag already true draws the panel instead of inheriting an invisible one.
//
// HOW IT NOTICES, AND THE THREE SHAPES THAT WERE ON OFFER.
//   - A DELEGATE ON THE SHELL, WHICH IS WHAT LANDED. Both writers of the flag are functions in
//     `StratShellSubsystem.cpp`, so an edge exists to fire on. `FStratOptionsPanelStateChanged`
//     was added there and its declaration carries the reasoning.
//   - A POLL ON TICK. Rejected. It spends a frame budget discovering an edge the writer already
//     knows about, and this project has paid for a tick that had to be reasoned about --
//     `bCanEverTick = false` swallowing all input on a controller. `AStratShellHUD`'s block
//     makes the same argument against a `Tick` override for its own menu.
//   - A PUSH FROM `ExecuteRoute`. Rejected, and it is the one that looks cheapest. It would put
//     the "show the panel" call on the SHIPPED path only: `RequestOptionsPanel` exists as a
//     separate public entry point precisely because `ExecuteRoute`'s other arms are unsurvivable
//     in a fixture, so a push written in `ExecuteRoute` would be invisible to every clause that
//     drives the flag directly -- the exact defect `PendingSlotForRoute` was extracted to fix.
//
// NOT IN THIS ROUND, with reasons:
// - AN INPUT MODE. `AStratShellHUD::ApplyMenuInputMode` already takes `FInputModeUIOnly` on the
//   title map and this project has measured that a UI-only mode SURVIVES LEVEL TRAVEL, so the
//   title path is covered. The MATCH path is not, and it is left alone rather than guessed at:
//   `AStratPlayerController` calls `SetInputMode` nowhere, changing that would move the hover
//   and click behaviour of the whole match, and no clause in this tree can observe an input
//   mode. DISCHARGED BY a human at the keyboard reporting whether the in-match panel is
//   operable; if it is not, the fix is a mode taken and given back around this panel's lifetime,
//   and `AStratPlayerController.h` already carries the note about who gives it back.
//     DISCHARGED 2026-09-05, IN THE DIRECTION THAT LEAVES THE CODE ALONE. The condition above
//   was put to a human at the keyboard, with candidate symptoms named in advance so that a "yes"
//   would not be a "yes" to nothing -- clicks landing on the board behind the panel, sliders
//   that will not drag, hover still highlighting the hexes underneath. The report was that the
//   options button works, the sliders work, the panel exits, and that it worked IN A MATCH too;
//   no symptom was reported against any of the three. So the match path needs no mode, and none
//   is taken. **NO EXECUTABLE BYTE MOVED FOR THIS DISCHARGE.**
//     WHAT THAT REPORT DOES NOT COVER, stated because a discharge that overstates its evidence
//   is worse than an open debt. It rests on ONE human report and nothing else -- it was not
//   measured, and no clause in this tree can observe an input mode, which is the same reason the
//   debt was taken rather than guessed at in the first place. It says the panel is OPERABLE. It
//   says NOTHING about the mouse-axis question, which is a separate rejection on separate
//   grounds: `AStratPlayerController.cpp`'s block rejects `SetInputMode` with capture because
//   `bShowMouseCursor = true` is load-bearing and capture changes how a click lands, and that
//   argument is untouched by this and still stands. Do not read this discharge as settling it.
//     AND IT IS REVERSIBLE. A later playtest that contradicts it re-opens the item, and the
//   recipe in the sentence above -- a mode taken and given back around this panel's lifetime --
//   is still the fix should that happen.
// - PAUSING THE MATCH. `Options` does not travel and does not stop the world. A pause would be a
//   rules-adjacent decision this class has no standing to make, and the match is turn-based and
//   waits for input anyway.
// - RESOLUTION. Sec 2.11.5's other half. `UGameUserSettings` owns it; `StratOptionsWidget.h`
//   states why it is not composed here.
// - A `/Game/` LITERAL. There is none. `OptionsWidgetClass` arrives from an `EditDefaultsOnly`
//   property on a GameMode Blueprint, which is the project rule and is why this class has a
//   `Configure...` call at all instead of a constructor.
// THIS HEADER INCLUDES `StratOptionsWidget.h`, WHICH MAKES IT THE FIRST HEADER IN `StratPlay`
// TO REACH A UMG HEADER, AND THE EXCEPTION IS ARGUED HERE AND RECORDED IN `StratPlay.Build.cs`
// RATHER THAN TAKEN QUIETLY.
//   WHY IT IS FORCED. `HandleAudioOptionsCommitted` is bound to a DYNAMIC multicast delegate, so
// it must be a `UFUNCTION`, so UHT generates a thunk containing
// `P_GET_STRUCT_REF(FStratAudioOptionsModel, ...)` into `Module.StratPlay.gen.cpp` -- a
// translation unit that includes this header and nothing else of ours. A struct-by-reference
// parameter cannot be marshalled from a forward declaration, so the complete type has to be
// visible from HERE and not merely from the `.cpp`. That is the same mechanism, from the other
// direction, that `StratScoreboardHUD.h` refuses `StratBridge.h` for.
//   WHY IT IS SAFE HERE AND WOULD NOT BE ELSEWHERE, MEASURED RATHER THAN ASSERTED: **no module
// depends on `StratPlay`.** `grep -n "StratPlay" Source/*/*.Build.cs Stratocracy.uproject`
// returns exactly two lines, this module's own `.Build.cs` and the `uproject`'s `Modules` array,
// so this module's headers have no external consumers and `UMG` staying `Private` costs nothing.
// THE DAY A MODULE DEPENDS ON `StratPlay`, this include is the first thing that will bite --
// either that module gains `UMG`, or `FStratAudioOptionsModel` moves to a header of its own that
// no UMG type reaches.
//   THE SHAPES THAT WERE REJECTED. A param-free handler reading `OptionsWidget->Model` instead:
// `AddDynamic` requires an exact signature match, so it does not compile. Three float
// parameters: three chances to pass the channels in the wrong order, which is the defect
// `FStratAudioOptionsModel` was made a single value to prevent.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Templates/SubclassOf.h"

#include "StratOptionsWidget.h"

#include "StratOptionsPresenter.generated.h"

class UStratShellSubsystem;

/**
 * Shows and hides Sec 2.11.5's volume screen, in every world that has one.
 *
 * IT HOLDS NO GAIN, NO SETTINGS OBJECT AND NO RULES STATE. Everything it knows it asks for at
 * the moment it needs it: the flag from `UStratShellSubsystem`, the persisted gains from
 * `UStratSoundDirector::GetAudioSettings`, the widget class from whichever GameMode configured
 * it. A cached gain here would be a second answer to "how loud is it", which
 * `UStratAudioSettings`'s own block refuses one layer down.
 */
UCLASS()
class STRATPLAY_API UStratOptionsPresenter : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Game and PIE only, copied from `UStratSoundDirector::DoesSupportWorldType` -- an editor
	 *  preview world has no viewport to put a panel on and no player to operate it. */
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;

	/**
	 * The widget class and where it sits in the viewport stack.
	 *
	 * CALLED BY A GameMode's `BeginPlay` FROM ITS OWN `EditDefaultsOnly` PROPERTIES, exactly as
	 * `UStratShellSubsystem::ConfigureMatchDestination` is. A subsystem has no details panel, so
	 * this is the only way an asset reference reaches this class without a `/Game/` literal.
	 *
	 * UNSET IS A CONFIGURATION AND NOT AN ERROR, on `AStratScoreboardHUD`'s and
	 * `AStratShellHUD`'s stated rule: a map whose GameMode leaves the class alone is saying
	 * "no options panel here", and `LastFailureReason` records it at `Log`.
	 *
	 * IT MAY BE CALLED MORE THAN ONCE AND THE LAST CALL WINS. There is no merge and no refusal:
	 * a second configure is a second GameMode `BeginPlay` in the same world, which does not
	 * happen, or a deliberate override from Blueprint, which is the caller's business.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Options")
	void ConfigureOptionsPanel(TSubclassOf<UStratOptionsWidget> InOptionsWidgetClass,
	                           int32                            InPanelZOrder);

	/**
	 * Makes the viewport match `UStratShellSubsystem::IsOptionsPanelOpen`. Idempotent.
	 *
	 * THE ONE FUNCTION EVERY PATH ENDS IN -- see the file header on reconciling rather than
	 * reacting. `BlueprintCallable` so a WBP or a fixture can force a pass without owning the
	 * delegate, matching `AStratShellHUD::CreateMenuWidget`'s reasoning for being reflected.
	 *
	 * A MISSING SHELL READS AS "CLOSED" AND NOT AS A FAILURE, matching
	 * `UStratShellSubsystem::GatherFacts`'s treatment of a missing match subsystem: on a world
	 * with no game instance there is no route that could have opened the panel, so closed is the
	 * correct answer rather than an error to report.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Options")
	void ReconcileOptionsPanel();

	/** Whether the panel is on screen right now. `AStratShellHUD::IsMenuLive`'s shape. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Options")
	bool IsPanelLive() const { return OptionsWidget != nullptr; }

	/**
	 * How many times this presenter has put a panel on the viewport.
	 *
	 * THE SAME INSTRUMENT `GetOptionsPanelRequestCount` IS AND FOR THE SAME REASON, one layer
	 * out: `IsPanelLive` cannot distinguish "the panel was shown, hidden and shown again" from
	 * "it has been up the whole time", and a clause proving the reconcile actually ran needs
	 * something only running can move. Never reset.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Options")
	int32 GetPanelsShownCount() const { return PanelsShownCount; }

	/**
	 * How many commits this presenter has forwarded to `UStratSoundDirector::CommitVolumes`.
	 *
	 * IT COUNTS THE FORWARD AND NOT THE BROADCAST, which is the distinction that makes it worth
	 * having. `UStratOptionsWidgetDouble::CommitCount` already counts what the widget said; this
	 * counts what this class did with it, so a binding that was never made -- the defect this
	 * whole file exists to fix -- reads zero here while reading non-zero there.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Options")
	int32 GetVolumeCommitsForwardedCount() const { return VolumeCommitsForwardedCount; }

	/** Why there is no panel, or why the last attempt to show one failed. Empty on success. */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Options")
	FString LastFailureReason;

	/** The panel, once it exists. Null before, and null on any world that configured no class. */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Options")
	TObjectPtr<UStratOptionsWidget> OptionsWidget;

protected:
	/** Binds the shell's delegate and reconciles once. See the file header on why both. */
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	/**
	 * Unbinds, removes the panel, and CLOSES THE FLAG.
	 *
	 * THE CLOSE IS THE DEBT `bOptionsPanelOpen`'S OWN DECLARATION NAMES: that member outlives
	 * the map, so an owner that does not close it leaves the next world's presenter opening a
	 * panel nobody asked for. It is done here rather than in an `EndPlay` because this class is
	 * a subsystem and `Deinitialize` is the lifetime hook it has.
	 */
	virtual void Deinitialize() override;

private:
	/** The class handed in by a GameMode. Never a path, never loaded by name. */
	UPROPERTY(Transient)
	TSubclassOf<UStratOptionsWidget> OptionsWidgetClass;

	/** Viewport stacking for the panel. Above the match HUD's widgets by default. */
	UPROPERTY(Transient)
	int32 PanelZOrder = 100;

	/** See `GetPanelsShownCount`. */
	UPROPERTY(Transient)
	int32 PanelsShownCount = 0;

	/** See `GetVolumeCommitsForwardedCount`. */
	UPROPERTY(Transient)
	int32 VolumeCommitsForwardedCount = 0;

	/** The shell, or null on a world with no game instance. */
	UStratShellSubsystem* FindShell() const;

	/** Creates the panel, seeds it, binds it and adds it to the viewport. Reason on false. */
	bool ShowPanel(FString& OutFailureReason);

	/** Unbinds and removes the panel. A no-op when there is none. */
	void HidePanel();

	/**
	 * Reads the persisted gains and pushes them at the panel.
	 *
	 * THROUGH `PushAudioOptions` AND NOT THROUGH THE THREE SETTERS, and the difference is the
	 * one that declaration calls the easiest thing in the file to get wrong: a setter fires
	 * `OnAudioOptionsCommitted`, so seeding through setters would re-save the slot every time a
	 * player merely LOOKED at the options screen, and would make this class's own forwarded
	 * count non-zero without a player having touched anything.
	 */
	void SeedPanel();

	/** Bound to `UStratShellSubsystem::OnOptionsPanelStateChanged`. Reconciles; ignores the
	 *  argument, deliberately -- see the file header on reconciling rather than reacting. */
	UFUNCTION()
	void HandleOptionsPanelStateChanged(bool bIsOpen);

	/** Bound to `UStratOptionsWidget::OnAudioOptionsCommitted`. Forwards to the director. */
	UFUNCTION()
	void HandleAudioOptionsCommitted(const FStratAudioOptionsModel& InModel);

	/** Bound to `UStratOptionsWidget::OnOptionsDismissed`. Closes the shell's flag, which comes
	 *  back through the delegate and takes the panel down -- one route, not two. */
	UFUNCTION()
	void HandleOptionsDismissed();
};
