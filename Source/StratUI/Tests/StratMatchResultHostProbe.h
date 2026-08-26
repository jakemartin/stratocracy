// TEST-ONLY. The UMG-free seam that lets a StratPlay clause place and read §2.11.4's end-of-match
// screen on `AStratScoreboardHUD`.
//
// WHY THIS FILE EXISTS, and it is a module boundary rather than a convenience. The same wall
// `StratGuidanceRouteProbe.h` and `StratProductionMenuHostProbe.h` record, measured the same way:
// `StratUI.Build.cs` lists `UMG`, `Slate` and `SlateCore` as PRIVATE dependencies, so StratPlay
// can neither include `Blueprint/UserWidget.h` nor link `UUserWidget`'s symbols.
// `StratScoreboardHUD.h` forward declares `UStratMatchResultWidget` and holds `MatchResultScreen`
// as a `TObjectPtr` precisely so StratPlay can include the HUD without acquiring UMG -- but a
// StratPlay clause still cannot CONSTRUCT one, and cannot safely dereference the `TObjectPtr` it
// can see. Both signatures below are expressible in types StratPlay already has.
//
// WHAT IS AND IS NOT A SEAM HERE. Nothing in this file is production code and nothing in
// production calls it; it lives under `Tests/` in both directions. It adds no capability the game
// has -- `InstallMatchResultDouble` writes the same public member
// `AStratScoreboardHUD::CreateMatchResultWidget` writes, and the clause then observes that
// production property, never a recording made by a double.
//
// WHY THERE IS NO `...IsInViewport` FUNCTION HERE, where the production-menu probe has one. That
// probe's third function exists to serve a clause whose whole subject is the disagreement between
// the pointer and the viewport (`IsProductionMenuWidgetOpen`). §2.11.4's clause has a different
// subject -- that a restart CLEARS the screen -- and the pointer is the only witness to that,
// because `HideMatchResult` is documented to drop the pointer unconditionally rather than
// conditionally on the viewport. Adding a viewport reader here would be a function no clause
// needs, which is untested code shaped like a seam.
//
// WHERE THE CLAUSE LIVES. `Source/StratPlay/Tests/StratMatchResultTrigger.cpp`. It has to live
// there: `UStratMatchSubsystem` is a StratPlay type and StratUI sits BELOW StratPlay, so naming
// it here would be `LNK2019` rather than a test.
#pragma once

#include "CoreMinimal.h"

class AStratScoreboardHUD;

/**
 * Constructs a concrete `UStratMatchResultWidget` and installs it on `Hud->MatchResultScreen`.
 *
 * DOES NOT CALL `AddToViewport`, and could not usefully: a headless `-nullrhi` run has no game
 * viewport, so `UGameViewportSubsystem::AddToScreen` refuses and `IsInViewport()` stays false.
 * See `StratMatchResultHostDouble.h`.
 *
 * `NewObject` AND NOT `CreateWidget`, for the reason `StratProductionMenuHostProbe.cpp` measured:
 * `UUserWidget::CreateWidgetInstance` refuses a player controller that is not a LOCAL player
 * controller, and a transient world has no `ULocalPlayer`.
 *
 * @return false, with `Hud->MatchResultScreen` untouched, if `Hud` is null or construction failed.
 */
STRATUI_API bool StratTestInstallMatchResultDouble(AStratScoreboardHUD* Hud);

/**
 * Whether `Hud->MatchResultScreen` currently holds a widget AT ALL -- the pointer, and nothing
 * about the viewport.
 *
 * THIS IS THE WITNESS FOR "A RESTART TOOK THE PRIOR MATCH'S VERDICT DOWN".
 * `AStratScoreboardHUD::HideMatchResult` calls `RemoveFromParent` and then clears the pointer
 * unconditionally, and `IsMatchResultWidgetOpen()` is false headlessly whether or not that
 * happened -- so the HUD's own reader cannot tell a cleared screen from an uncleared one here,
 * and this can.
 */
STRATUI_API bool StratTestHasMatchResultWidget(const AStratScoreboardHUD* Hud);
