// GDD §2.11.6 -- the Blueprint-reachable surface for the guided opening's projection.
//
// WHAT GAP THIS CLOSES, and it was a C++ gap wearing a content bug's clothes. Three
// Widget Blueprints were authored against `FStratGuidanceView` and every one of them was
// inert, because there was NO Blueprint-reachable route to `FStratViewModel::Guidance`.
// Measured, three independent ways, on the tree at `c1a8436`:
//   - `UStratMatchSubsystem::GetViewModel()` is deliberately not a `UFUNCTION`, and its
//     own declaration states the reason -- UHT does not accept a reference return, and
//     returning a copy of a 99-hex model to satisfy reflection would make the cheap read
//     expensive. That is a ruling, not an oversight, and this class exists so it can
//     stand.
//   - `UStratMatchSubsystem::AppliedModel` is `UPROPERTY(Transient)` with no
//     `BlueprintReadOnly`, and it is a RECORD of what was applied rather than an input to
//     anything; publishing it would invite a graph to read the screen back out of it.
//   - `UStratMatchSubsystem` is a bare `UCLASS()`, so it is not even `BlueprintType`.
// None of those three wanted changing. What was missing was a place for the value to
// ARRIVE, which is what this is.
//
// WHY A WIDGET AND NOT AN ACCESSOR. The value is PUSHED here on the reconcile path, it
// is never PULLED from here by a graph asking the subsystem for it. That is the same
// property `UStratMatchSubsystem::ApplyView` is built on -- "presentation is reconciled,
// not evented" -- and it is what makes "rebuild the screen from the view model alone"
// structurally true for the directive strip too. A `BlueprintPure` getter on the
// subsystem would have given Blueprint a second way to obtain guidance whose freshness
// depended on when it happened to ask; there is no such way here, because the only
// writer is `PushGuidance` and its only caller is the reconcile point.
//
// WHY THE PROPERTY IS CALLED `Guidance` AND IS OF EXACTLY `FStratGuidanceView`. This is
// load-bearing for a REPARENT that has not happened yet. `WBP_DirectiveStrip` already
// carries a Blueprint member named `Guidance` of type `FStratGuidanceView` -- measured on
// the asset bytes: `/Script/StratUI` once and `StratGuidanceView` thirty-three times --
// and drives four properties off pure functions that break it. Matching the inherited
// name AND the type is what lets those bindings survive being reparented onto this
// class: the Blueprint's own `Guidance` variable is deleted and the existing getter nodes
// re-resolve, by name and type, to the inherited one. A different name here (`Model`,
// say, mirroring the scoreboard) would have orphaned all thirty-three.
// NOTE FOR WHOEVER PERFORMS THAT REPARENT: the collision is not silent and must not be
// worked around by renaming the C++ property. Unreal refuses to compile a Blueprint whose
// variable shadows an inherited property; deleting the Blueprint variable is the step,
// and it is the step that makes the bindings live.
//
// WHY THIS HEADER DOES NOT INCLUDE StratBridge.h, and must never start to. It declares a
// `UCLASS`, so UHT parses it. `StratBridge.h` includes the vendored `strat` headers and
// its own block records that this is safe there only because it "declares no reflected
// types on purpose". Including it here would put those headers in front of the header
// tool. Nothing in this class needs the bridge in any case: the value it holds has
// already been decided by `FStratGuidedOpening::DecorateViewModel` before it arrives, and
// asking the rules anything from a widget would be the second-opinion failure `T-UI-03`
// names about the scoreboard.
//
// WHAT IS DELIBERATELY NOT IN THIS FILE.
// - NO ARITHMETIC, AND NOTHING TO DO IT WITH. `FStratGuidanceView`'s own block records
//   that every field is something a widget draws or dims and nothing is a fact that must
//   be combined with another. This class copies one struct and fires one event; there is
//   no place in it where a `+` could be written.
// - NO WAY TO ADVANCE A BEAT. `FStratGuidedOpening` (`Source/StratPlay/`) owns which beat
//   is outstanding. A widget deriving from this can draw the strip and cannot retire
//   anything, which is the same line `FStratGuidanceView` draws around itself.
// - NO ONE-SHOT TIP LAYER. `WBP_OneShotTip`'s `CurrentTip` / `TipIsShowing` / `TipId` are
//   Blueprint-only names with no counterpart anywhere in `Source/` (grep: zero hits), and
//   `StratGuidedOpening.h` declines to own tips because they run on a different clock.
//   The tip view struct is a separate design and folding a guess at it into this class
//   would bake that guess into an asset the moment one derived from it.
// - NO `TSubclassOf` AND NO `/Game/` PATH. This class is a parent, not a spawner. The
//   asset reference that instantiates it lives on `AStratScoreboardHUD` as an
//   `EditDefaultsOnly` property set on a Blueprint default.
//
// IRREVERSIBLE ONCE AN ASSET DERIVES FROM THIS. A Widget Blueprint reparented onto this
// class bakes `/Script/StratUI.StratGuidanceWidget` into its own bytes. The module and
// the class name are therefore expensive to change after the first reparent, which is the
// same trap `StratUI.Build.cs` records for `UStratScoreboardWidget`. StratUI and not
// StratPlay is correct for the same reason it was correct there: `FStratGuidanceView`
// lives in StratUI, StratPlay depends on StratUI, and UMG is already a dependency of this
// module and of no other.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "StratViewModel.h"

#include "StratGuidanceWidget.generated.h"

/**
 * §2.11.6's guided-opening projection, held where a Widget Blueprint can bind to it.
 *
 * Abstract and Blueprintable because the WBP_ asset derives from it and does the layout.
 * This class draws nothing: it holds one value and announces that the value changed.
 */
UCLASS(Abstract, Blueprintable, meta = (DisplayName = "Strat Guidance"))
class STRATUI_API UStratGuidanceWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Replaces `Guidance` and fires `OnGuidanceRefreshed`.
	 *
	 * NOT A UFUNCTION, AND THAT IS THE POINT RATHER THAN A LIMITATION. Every argument it
	 * takes is reflectable, so unlike `UStratScoreboardWidget::Refresh` nothing FORCES
	 * this to be plain C++. It is plain C++ so that Blueprint has no way to write a
	 * guidance value at all. Reflecting it would let a graph push a fabricated beat into
	 * the strip, which would make the screen a thing two authors can state -- the rules
	 * module's projection, and whatever a designer wired up on a button. The strip must
	 * be readable off `FStratViewModel::Guidance` alone or the reconcile property is a
	 * claim rather than a fact.
	 *
	 * VOID, AND IT CANNOT REFUSE. Reconciliation is not a request. There is no failure
	 * mode here to report: the value has already been decided, and "the widget did not
	 * like it" is not a state this system has. Compare `UStratScoreboardWidget::Refresh`,
	 * which returns `bool` because it BUILDS its model from the bridge and the build can
	 * be refused; this one is handed a finished value.
	 *
	 * ASSIGNS UNCONDITIONALLY, including when the incoming view is inactive. `bActive`
	 * false is a real state with three causes the strip is deliberately not allowed to
	 * tell apart -- see `FStratGuidanceView` -- and skipping the assignment on it would
	 * leave the last active directive on screen after the window closed, which is the one
	 * outcome §2.11.6 cannot produce.
	 */
	void PushGuidance(const FStratGuidanceView& InGuidance);

	/**
	 * The guidance projection as of the last reconcile. Everything the WBP draws comes
	 * from here.
	 *
	 * READ-ONLY TO BLUEPRINT ON PURPOSE. `PushGuidance` is the only writer. A writable
	 * property would be the same second-author problem as a reflected setter, arrived at
	 * from a different direction.
	 *
	 * TRANSIENT because it is a projection of live match state and means nothing on disk;
	 * a saved copy would be a second, staler statement about a window that has closed.
	 */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Guidance")
	FStratGuidanceView Guidance;

	/**
	 * Fired after `Guidance` is replaced.
	 *
	 * The view is passed as an argument as well as being readable off the widget, for the
	 * reason `UStratScoreboardWidget::OnScoreboardRefreshed` gives about its own model: so
	 * that a graph has no motive to cache its own copy of a value, a cached copy being a
	 * value that can go stale.
	 *
	 * FIRED ON EVERY PUSH, NOT ONLY ON A CHANGE. A widget that animated only on
	 * transitions would need to know what the previous view was, which is state this class
	 * refuses to hold; and an unconditional fire is what makes the strip's contents a
	 * function of the current model alone. A graph that wants edge behaviour compares
	 * against what it drew, in the one place that already knows.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Stratocracy|Guidance")
	void OnGuidanceRefreshed(const FStratGuidanceView& InGuidance);
};
