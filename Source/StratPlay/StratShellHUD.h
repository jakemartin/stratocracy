// GDD §2.11.5 / GATE-TITLEMENU -- the title map's HUD. The object that creates the menu,
// and the reason it exists is an ORDERING rather than a widget.
//
// WHAT GAP THIS CLOSES, AND THE RECORD NAMES IT IN TERMS. W6 shipped the shell with no HUD,
// so `WBP_TitleMenu` is created from `BP_StratShellGameMode`'s own graph -- Create Widget,
// Add to Viewport, input mode, cursor -- which is the one place in this project where a
// widget reaches the screen without a C++ HUD owning a widget-class slot. The editor lane
// flagged the departure and filed the alternative rather than taking it:
// `Tools/architect/state/content.md`, W6's asset batch, *"The alternative is a shell HUD
// class with a widget slot, which would match the convention and is an engineer-lane item;
// it was not taken and the user was told."* This class is that item.
//
// THE HALF THAT IS NOT COSMETIC, AND IT IS THE WHOLE POINT. `AStratShellGameMode::BeginPlay`
// calls `Super::BeginPlay()` -- which is what fires a Blueprint's `Event BeginPlay` -- and
// only afterwards calls `ConfigureMatchDestination`. A menu created off `Event BeginPlay`
// therefore asks `UStratShellSubsystem::GetMenuModel` before the shell has been given
// `MatchLevel`, and every row comes back greyed reading "No match level is configured."
// The graph works around it by hopping one tick. That hop is correct and it is INVISIBLE:
// nothing in the tree asserts it, deleting it leaves a menu that still draws four plausible
// rows, and `global.md` records the clause that would have pinned it being REFUSED on the
// grounds that a clause asserting the current ordering would go red the day the ordering was
// repaired -- it would pin a hazard as a requirement and block its own remedy.
//
//   THIS CLASS TURNS THAT HAZARD INTO A REQUIREMENT. It does not assert when
//   `ConfigureMatchDestination` runs. It asks the shell WHETHER IT HAS BEEN CONFIGURED and
//   creates the menu only once the answer is yes, waiting a bounded number of ticks for it.
//   The property the suite can then pin is *"the shell is configured before the menu is
//   asked for its model"*, which stays green under any reordering that keeps it true --
//   including the repair of moving `ConfigureMatchDestination` ahead of `Super::BeginPlay()`,
//   which would make the wait zero ticks and change no clause. That is the shape this
//   project's own note asks for: pin the requirement, not the hazard.
//
// WHY THE SHELL GREW `HasMatchDestinationBeenConfigured` RATHER THAN THIS CLASS INFERRING IT.
// Every value the configuration carries has a legal default that a configured shell can also
// hold: `MatchLevel` may be null on a genuinely unconfigured Blueprint, and `SaveSlotName` is
// non-empty by construction on `AStratShellGameMode`, so neither can distinguish "nobody has
// called" from "somebody called with these values". This project already carries a defect of
// exactly that species -- a real default that could not signal unset, which wrote the
// player's save every run -- so the flag records THE CALL rather than its arguments, and an
// unconfigured shell and a shell configured with nothing in it stay two different states.
//
// WHY AN AHUD. `StratScoreboardHUD.h` gives three reasons and two of them bind here: a map
// chooses its HUD as ASSET DATA through `HUDClass`, so no module has to name a `/Game/` path
// to put a menu on screen; and a HUD is destroyed with the world it was spawned into, which
// is the lifetime a title-screen menu has. The third -- per-player and client-local -- is
// what a menu is. The shell SUBSYSTEM keeps its own `UGameInstanceSubsystem` lifetime for
// the reason its header gives: it must survive the travel it schedules. Menu and shell have
// different lifetimes on purpose, and this class holds the shorter one.
//
// IT IS NOT A SUBCLASS OF `AStratScoreboardHUD`, and the shape is named because it is the one
// a reader will reach for. That class seeds an `FStratBridge`, loads two DataTables and owns
// five match widgets; a title screen has no board, no rules state and no scenario, and
// inheriting would hand the menu map a details panel full of properties that must stay unset.
// They share `AHUD` and a convention, and nothing else -- the same relationship
// `AStratShellGameMode` states with `AStratGameMode`, for the same reason.
//
// NOT IN THIS ROUND, with reasons:
// - REMOVING THE GRAPH-SIDE CREATION IN `BP_StratShellGameMode`. It is `Content/` and
//   therefore the editor lane's, and this module cannot touch it. UNTIL IT IS REMOVED AND
//   `HUDClass` IS SET, ADOPTING THIS CLASS WOULD PUT TWO MENUS ON SCREEN. That is a
//   single-commit asset change, it is stated here rather than left to be discovered, and
//   this class is inert until an asset points at it -- nothing spawns a HUD that no
//   `HUDClass` names.
// - A MENU WIDGET BASE CLASS. `WBP_TitleMenu` is a plain `UUserWidget` that binds to the
//   subsystem's reflected surface by index; the slot below is typed `UUserWidget` so it
//   accepts that asset as it stands. A `UStratShellMenuWidget` with `BindWidget` members
//   would be the StratUI-side follow-up and it is a different lane's asset work.
// - REFRESHING THE MENU. `GetMenuModel` touches the disk on every call and the widget
//   refreshes itself at Construct; a HUD-driven refresh cadence is not knowable until the
//   menu has events that can move the answer. This class creates the menu and nothing else.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Templates/SubclassOf.h"

#include "StratShellHUD.generated.h"

class UStratShellSubsystem;
class UUserWidget;

/**
 * What to do about the menu at the moment this HUD is asked.
 *
 * THREE STATES AND NOT A BOOL, so that "wait" and "give up waiting" are distinguishable in a
 * clause and in a log line. A bool would have collapsed the two, and the second one is the
 * arm a player can actually see.
 */
UENUM(BlueprintType)
enum class EStratShellMenuTiming : uint8
{
	/** The shell reports configured. Create the menu now. */
	CreateNow,

	/** The shell is not configured yet and the wait budget has ticks left. */
	WaitForConfiguration,

	/**
	 * The wait budget is spent and the shell is still unconfigured. Create the menu anyway.
	 *
	 * THE ARM IS DELIBERATE AND `AStratShellGameMode::BeginPlay` ALREADY ARGUES FOR IT in its
	 * own words -- it refuses to skip configuring on a null `MatchLevel` because "refusing to
	 * configure at all would replace that readable menu with an empty one and a log line
	 * nobody is looking at". The same trade holds one level up. An unconfigured shell still
	 * builds all four rows, greyed, each naming its own reason, which is §2.11.5's rule that
	 * unavailable options are greyed and named and never hidden. A blank screen is the worse
	 * of the two failures, so the budget expiring costs a `LastFailureReason` and a log line,
	 * not a menu.
	 */
	CreateUnconfigured
};

/**
 * Runs §2.11.5's title/menu screen.
 *
 * A map gets one by pointing its GameMode's `HUDClass` at a Blueprint subclass -- asset data,
 * never an `#include`, which is how every widget in this project reaches the screen.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Strat Shell HUD"))
class STRATPLAY_API AStratShellHUD : public AHUD
{
	GENERATED_BODY()

public:
	AStratShellHUD();

	// ---- CONFIGURATION, from this HUD's Blueprint defaults. ----

	/**
	 * The menu asset.
	 *
	 * `EditDefaultsOnly` AND NOT A PATH, which is the project rule at its sharpest: a menu's
	 * whole job is to be an asset, and naming `WBP_TitleMenu` in this file would be the
	 * `/Game/` literal that `StratShellSubsystem.h` refuses in terms.
	 *
	 * UNSET IS A CONFIGURATION AND NOT AN ERROR, on `AStratScoreboardHUD`'s stated reason:
	 * a Blueprint that leaves this alone is saying "this map draws no menu", and the log
	 * records it at Log rather than Error so a genuine failure downstream stays legible.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Shell")
	TSubclassOf<UUserWidget> MenuWidgetClass;

	/** Presentation, so a later overlay can be layered over or under the menu from an asset. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Shell")
	int32 MenuZOrder = 0;

	/**
	 * Take a UI-only input mode and show the cursor once the menu is up.
	 *
	 * ON BY DEFAULT AND ON THIS PROPERTY RATHER THAN IN THE WIDGET, because the input mode is
	 * a statement about the whole screen and not about one widget: `AStratShellGameMode` sets
	 * `DefaultPawnClass` to a bare `APawn` precisely so that nothing is listening behind the
	 * menu, and this is the other half of that sentence. A map that wants a menu over a live
	 * scene clears it.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Shell")
	bool bTakeUiOnlyInputMode = true;

	/**
	 * How many ticks to wait for `ConfigureMatchDestination` before drawing anyway.
	 *
	 * FOUR, WHICH IS SLACK AND NOT A MEASUREMENT, and it is said here rather than implied by
	 * the number. One tick is what the shipped Blueprint graph needs today, because the
	 * configuring call is one `Super::BeginPlay()` away. The budget is larger so that a HUD
	 * spawned earlier in the same frame, or a GameMode whose BeginPlay is reached later in
	 * the actor iteration, is absorbed rather than discovered by a player. Nothing here
	 * depends on the exact value: at zero the class still draws, on the `CreateUnconfigured`
	 * arm, which is why that arm exists.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Shell", meta = (ClampMin = "0"))
	int32 MaxConfigurationWaitTicks = 4;

	// ---- THE DECIDER. World-free, so a clause can reach every arm. ----

	/**
	 * What to do about the menu, given the shell's state and how long we have waited.
	 *
	 * `static` AND TAKING FACTS RATHER THAN A SUBSYSTEM POINTER, on `UStratShellSubsystem`'s
	 * own stated split: everything that DECIDES touches no world, so a headless clause can
	 * assert every arm without a viewport, a cursor or a level travel. What is left over in
	 * `CreateMenuWidget` below is one `CreateWidget` and one `AddToViewport`.
	 */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Shell")
	static EStratShellMenuTiming DecideMenuTiming(bool bShellIsConfigured,
	                                              int32 TicksWaited,
	                                              int32 MaxTicksToWait);

	// ---- THE LIVE SURFACE. ----

	/**
	 * Creates the menu and puts it on the viewport. Returns false with a reason.
	 *
	 * `BlueprintCallable` SO THE ARM IS REACHABLE WITHOUT BeginPlay, matching
	 * `AStratScoreboardHUD::CreateScoreboardWidget`. It does not consult the shell: the
	 * decision of WHEN is `DecideMenuTiming`'s and calling this directly is a deliberate
	 * override rather than an accident.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Shell")
	bool CreateMenuWidget(FString& OutFailureReason);

	/** Whether a menu is on screen right now. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Shell")
	bool IsMenuLive() const { return MenuWidget != nullptr; }

	/** The menu, once it exists. Null before, and null on any map that sets no class. */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Shell")
	TObjectPtr<UUserWidget> MenuWidget;

	/** Why there is no menu, or why the one on screen is drawing an unconfigured shell. */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Shell")
	FString LastFailureReason;

	/**
	 * Whether the shell reported configured at the instant the menu was created.
	 *
	 * IT IS THE REQUIREMENT MADE OBSERVABLE, and that is the only reason it is a member. The
	 * decision lives in `DecideMenuTiming` where a clause can reach it; this records what was
	 * actually true on the shipped path, so the property can be read off a live PIE session
	 * as well as asserted headlessly. False here with a menu on screen is the
	 * `CreateUnconfigured` arm, and `LastFailureReason` names it.
	 */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Shell")
	bool bMenuDrewOnAConfiguredShell = false;

	/** How many ticks the menu waited for the shell. Zero when it never had to wait. */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Shell")
	int32 ConfigurationTicksWaited = 0;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** One pass of the decider. BeginPlay runs it once and it re-arms itself while waiting. */
	void ResolveMenuTiming();

	/** The shell, or null on any world that has no game instance subsystem. */
	UStratShellSubsystem* FindShell() const;

	/** Applies `bTakeUiOnlyInputMode`. Separate so a failed create leaves input alone. */
	void ApplyMenuInputMode();
};
