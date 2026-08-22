// TEST-ONLY. The seam that lets a StratPlay clause observe a StratUI widget.
//
// WHY THIS FILE EXISTS, and it is a module boundary rather than a convenience. MEASURED, not
// assumed: `StratUI.Build.cs` lists `UMG`, `Slate` and `SlateCore` as PRIVATE dependencies.
// StratPlay therefore cannot name `UStratGuidanceWidget` at all -- not by including its
// header (which includes `Blueprint/UserWidget.h`, whose include path StratPlay does not
// have) and not by linking (`UUserWidget`'s symbols are unresolved there). Attempting it is
// not a style problem; it is 60-odd `LNK2019`s naming `UUserWidget::NativeTick` and friends,
// which is how this file's shape was arrived at.
//
// `AStratScoreboardHUD.h` already records the same boundary from the other side: it FORWARD
// DECLARES `UStratGuidanceWidget` and holds it as a `TObjectPtr`, precisely so that StratPlay
// can include the HUD without acquiring UMG. This header does the same thing for the test
// lane -- every signature below is expressible in types StratPlay already has.
//
// WHAT IS AND IS NOT A SEAM HERE. Nothing in this file is production code and nothing in
// production calls it; it lives under `Tests/` in both directions. It adds no capability the
// game has -- `InstallStripDouble` writes the same member `AStratScoreboardHUD::
// CreateGuidanceWidget` writes, and `ReadStrip` reads the same property a WBP binds to. The
// clauses observe the production property, not a recording made by a double.
//
// WHERE THE CLAUSES LIVE, and why they are split across two modules:
//   - `Source/StratPlay/Tests/StratGuidanceRouteClauses.cpp` -- everything about
//     `UStratMatchSubsystem::ApplyView` and `AStratScoreboardHUD::PushGuidance`. StratPlay is
//     the only module that can see the subsystem.
//   - `Source/StratUI/Tests/StratGuidanceStripClauses.cpp` -- everything about
//     `UStratGuidanceWidget` itself. StratUI is the only module that can see it.
// A clause put in the other module of each pair would be `LNK2019`, not a test.
#pragma once

#include "CoreMinimal.h"

#include "StratViewModel.h"

class AStratScoreboardHUD;

/**
 * Constructs a concrete `UStratGuidanceWidget` and installs it on `Hud->GuidanceStrip`.
 *
 * `UStratGuidanceWidget` is `UCLASS(Abstract)` -- a ruling `Tests/` may not weaken -- so the
 * concrete class is a test-only subclass declared in `StratGuidanceRouteDouble.h`. That
 * subclass adds and overrides nothing, so what a clause then observes is the inherited
 * property a real `WBP_DirectiveStrip` binds to.
 *
 * DOES NOT CALL `AddToViewport`, unlike `AStratScoreboardHUD::CreateGuidanceWidget`. A
 * headless `-nullrhi` run has no game viewport; the clauses are about what the route delivers,
 * not about who parented the receiver.
 *
 * @return false, with `Hud->GuidanceStrip` untouched, if `Hud` is null or construction failed.
 */
STRATUI_API bool StratTestInstallGuidanceStripDouble(AStratScoreboardHUD* Hud);

/**
 * Reads `UStratGuidanceWidget::Guidance` off the HUD's installed strip.
 *
 * @return false, with `Out` untouched, when there is no strip -- so "no strip" and "a strip
 *         holding a default view" are never confused for one another by a clause.
 */
STRATUI_API bool StratTestReadGuidanceStrip(const AStratScoreboardHUD* Hud, FStratGuidanceView& Out);

/**
 * Calls `UStratGuidanceWidget::PushGuidance` on the HUD's strip DIRECTLY, bypassing
 * `AStratScoreboardHUD::PushGuidance`.
 *
 * KEPT SEPARATE FROM THE HUD'S FORWARDER ON PURPOSE. The widget's unconditional assignment and
 * the HUD's field-blind forwarding are two different rulings held by two different files. A
 * clause that could only reach the widget THROUGH the HUD would conflate them, and a
 * regression in either would redden both -- which is a report that does not say what broke.
 *
 * @return false when there is no strip to push at.
 */
STRATUI_API bool StratTestPushAtGuidanceStripDirectly(
	AStratScoreboardHUD* Hud, const FStratGuidanceView& In);
