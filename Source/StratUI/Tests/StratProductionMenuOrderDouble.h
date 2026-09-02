// TEST-ONLY. A concrete `UUserWidget` that reports THE MOMENT IT IS TAKEN OFF THE PANEL.
//
// WHY THIS EXISTS AND WHY IT IS A SECOND DOUBLE RATHER THAN A FIELD ON THE FIRST.
// `StratProductionMenuHostDouble.h` states, in terms, that it "ADDS NOTHING AND OVERRIDES
// NOTHING", and gives the reason: a double that kept its own bookkeeping would let a clause go
// green comparing the double against itself while the production member it stands in for was
// never touched. That rule is correct for every clause in that file and it is kept -- that
// class is not edited, and the clauses that use it still read the HUD's own property and
// `UUserWidget`'s own `IsInViewport()`.
//
// THIS CLASS EXISTS BECAUSE ONE PROPERTY IS AN ORDER, AND AN ORDER IS NOT VISIBLE IN THE STATE
// THAT FOLLOWS IT. `AStratPlayerController::CloseProductionMenu` performs two acts on two
// objects and its own declaration calls the ORDER the contract: the panel comes down
// (`AStratScoreboardHUD::CloseProductionMenuWidget`) and THEN the rows are cleared
// (`UStratMatchSubsystem::CloseProductionMenu`), because the reverse leaves a live panel bound
// to an empty row array. After the call returns, BOTH orders have produced exactly the same
// final state: no widget, no rows. So no clause that only inspects the aftermath can tell a
// correct implementation from the defective one, and a clause named for the order while
// measuring only the conjunction would carry a subject broader than what it pins -- the failure
// this directory has paid for before.
//
// WHAT IT DOES, AND IT IS THE MINIMUM THAT MAKES THE ORDER OBSERVABLE. It overrides
// `UWidget::RemoveFromParent`, which is the one virtual `CloseProductionMenuWidget` calls
// (`ProductionMenu->RemoveFromParent(); ProductionMenu = nullptr;`), and invokes a `TFunction`
// the installing clause supplied. The clause's own lambda then reads the SUBSYSTEM'S state at
// that instant -- `UStratMatchSubsystem::IsProductionMenuOpen()`, the production answer, not a
// recording made by this class. So the double supplies a MOMENT and never an expectation, and
// the "comparing the double against itself" hazard the other double's block names is not
// reachable through it: every value compared is the subsystem's own.
//
// IT IS STILL NOT A CAPABILITY THE GAME LACKS. `RemoveFromParent` is called on the shipped path
// by the shipped code; this class adds no route, no ordering and no state to production. The
// hook is null unless a clause set one, and `Super::RemoveFromParent()` runs either way.
//
// THIS HEADER IS NOT INCLUDABLE FROM StratPlay and must not be made so, for
// `StratProductionMenuHostDouble.h`'s measured reason: it includes `Blueprint/UserWidget.h`,
// and `UMG` is a PRIVATE dependency of StratUI (~60 x `LNK2019`, recorded in
// `StratGuidanceRouteProbe.h`). The UMG-free surface a StratPlay clause installs this through
// is `StratProductionMenuHostProbe.h`.
#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "StratProductionMenuOrderDouble.generated.h"

/**
 * Concrete `UUserWidget` that calls `OnRemovedFromParent` when the HUD takes it down.
 *
 * THE HOOK IS A PLAIN MEMBER AND NOT A `UPROPERTY`, because `TFunction` is not a reflected
 * type and nothing in the reflection system needs to see it. The object's lifetime is the
 * HUD's -- the probe outers it there -- so the hook cannot outlive an owner that could still
 * fire it.
 */
UCLASS()
class UStratProductionMenuOrderDouble : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Called from inside `RemoveFromParent`, BEFORE `Super`, and never called by this class
	 * for any other reason.
	 *
	 * BEFORE `Super` DELIBERATELY. The subject is which of the CALLER'S two acts happened
	 * first, and every part of `RemoveFromParent`'s own body is inside the caller's first
	 * act -- so firing at the top or the bottom of it observes the same instant as far as
	 * the caller's ordering goes, and the top is the one that cannot be skipped by an early
	 * return added to the base class later.
	 *
	 * A CLAUSE MUST CLEAR IT before its fixture tears down if the lambda captures anything
	 * raw; `StratTestClearProductionMenuOrderHook` exists for that, and the clause that uses
	 * this captures weakly in any case.
	 */
	TFunction<void()> OnRemovedFromParent;

	virtual void RemoveFromParent() override;
};
