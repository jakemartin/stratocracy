// TEST-ONLY. The UMG-free seam that lets a StratPlay clause place and read §2.11.5's menu
// widget on `AStratScoreboardHUD`.
//
// WHY THIS FILE EXISTS, and it is a module boundary rather than a convenience. This is the
// same wall `StratGuidanceRouteProbe.h` records, measured the same way: `StratUI.Build.cs`
// lists `UMG`, `Slate` and `SlateCore` as PRIVATE dependencies, so StratPlay can neither
// include `Blueprint/UserWidget.h` nor link `UUserWidget`'s symbols. `AStratScoreboardHUD.h`
// forward declares `UUserWidget` and holds `ProductionMenu` as a `TObjectPtr` precisely so
// StratPlay can include the HUD without acquiring UMG -- but a StratPlay clause still cannot
// CONSTRUCT one, and cannot safely dereference the `TObjectPtr` it can see. Every signature
// below is expressible in types StratPlay already has.
//
// WHAT IS AND IS NOT A SEAM HERE. Nothing in this file is production code and nothing in
// production calls it; it lives under `Tests/` in both directions. It adds no capability the
// game has -- `InstallProductionMenuDouble` writes the same public member
// `AStratScoreboardHUD::CreateProductionMenuWidget` writes, and the clauses then observe the
// production property and `AStratScoreboardHUD::IsProductionMenuWidgetOpen`, never a recording
// made by a double.
//
// WHERE THE CLAUSES LIVE, and why they are split across two modules:
//   - `Source/StratUI/Tests/StratProductionMenuHostClauses.cpp` -- everything about
//     `AStratScoreboardHUD`'s widget-host half on its own.
//   - `Source/StratPlay/Tests/StratProductionMenuAffordance.cpp` -- everything that needs
//     `AStratPlayerController` or `UStratMatchSubsystem` in the same frame as the HUD.
//     StratPlay is the only module that can see all three.
// A clause put in the other module of each pair would be `LNK2019`, not a test.
#pragma once

#include "CoreMinimal.h"

class AStratScoreboardHUD;

/**
 * Constructs a concrete `UUserWidget` and installs it on `Hud->ProductionMenu`.
 *
 * DOES NOT CALL `AddToViewport`, and could not usefully: a headless `-nullrhi` run has no game
 * viewport, so `UGameViewportSubsystem::AddToScreen` refuses and `IsInViewport()` stays false.
 * See `StratProductionMenuHostDouble.h` -- that state is the SUBJECT of a clause rather than a
 * shortcut around one.
 *
 * @return false, with `Hud->ProductionMenu` untouched, if `Hud` is null or construction failed.
 */
STRATUI_API bool StratTestInstallProductionMenuDouble(AStratScoreboardHUD* Hud);

/**
 * Whether `Hud->ProductionMenu` currently holds a widget AT ALL -- the pointer, and nothing
 * about the viewport.
 *
 * THE POINT OF HAVING BOTH THIS AND `AStratScoreboardHUD::IsProductionMenuWidgetOpen()`. The
 * two answers are what tells a clause apart from a tautology: a menu-open reader that consulted
 * only the pointer would agree with this function always, and the shipped one -- which reads
 * `UUserWidget::IsInViewport()` too -- disagrees with it in exactly the headless case.
 */
STRATUI_API bool StratTestHasProductionMenuWidget(const AStratScoreboardHUD* Hud);

/**
 * `UUserWidget::IsInViewport()` read off the installed widget, straight from the engine.
 *
 * @return false when there is no widget, which a caller must therefore rule out first with
 *         `StratTestHasProductionMenuWidget` -- "no widget" and "a widget off the viewport"
 *         are two different states and no clause below may confuse them.
 */
STRATUI_API bool StratTestProductionMenuWidgetIsInViewport(const AStratScoreboardHUD* Hud);

// ---------------------------------------------------------------------------------------------
// THE ORDER-OBSERVING DOUBLE, ADDED 2026-09-02 FOR `T-UI-03.CloseProductionMenuTakesThePanel-
// DownBeforeClearingTheRows`.
//
// WHY THE SURFACE GREW. `AStratPlayerController::CloseProductionMenu` performs two acts on two
// objects and its declaration calls the ORDER the contract. Both orders leave the identical
// final state -- no widget, no rows -- so the order is invisible to any clause that inspects
// only the aftermath. `StratProductionMenuOrderDouble.h` supplies the missing MOMENT; the two
// functions below are how a StratPlay clause reaches it without acquiring UMG.
//
// THE DOUBLE SUPPLIES A MOMENT AND NEVER AN EXPECTATION. Everything the hook's lambda reads is
// the subsystem's own answer, which is what keeps `StratProductionMenuHostDouble.h`'s "a double
// that kept its own bookkeeping" hazard out of reach.
// ---------------------------------------------------------------------------------------------

/**
 * Installs a `UStratProductionMenuOrderDouble` on `Hud->ProductionMenu` and arms its hook.
 *
 * `NewObject` AND NOT `CreateWidget`, for `StratTestInstallProductionMenuDouble`'s measured
 * reason: `UUserWidget::CreateWidgetInstance` refuses a player controller that is not a LOCAL
 * player controller, and a transient world has none.
 *
 * @param OnPanelDown fired from inside `UWidget::RemoveFromParent` -- i.e. inside
 *                    `AStratScoreboardHUD::CloseProductionMenuWidget`, which is the caller's
 *                    FIRST act. Capture weakly: a fixture teardown can reach the same path.
 * @return false, with `Hud->ProductionMenu` untouched, if `Hud` is null or construction failed.
 */
STRATUI_API bool StratTestInstallProductionMenuOrderDouble(
	AStratScoreboardHUD* Hud,
	TFunction<void()>    OnPanelDown);

/**
 * Drops the hook from whatever order double is installed, leaving the widget alone.
 *
 * FOR TEARDOWN AND NOT FOR MEASUREMENT. A clause calls this once it has read what it needed, so
 * that a later `RemoveFromParent` -- `AStratScoreboardHUD::EndPlay` calls one unguarded -- runs
 * no clause code. Safe to call when nothing is installed.
 */
STRATUI_API void StratTestClearProductionMenuOrderHook(AStratScoreboardHUD* Hud);
