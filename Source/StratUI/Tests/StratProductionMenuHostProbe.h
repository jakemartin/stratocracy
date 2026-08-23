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
