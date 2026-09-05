// TEST-ONLY. A concrete `UStratOptionsWidget` that records what it is told was committed.
//
// WHY IT EXISTS AT ALL. `UStratOptionsWidget` is `UCLASS(Abstract, Blueprintable)` -- the
// shipped occupant is a `WBP_` asset the editor lane owns and which no headless run may depend
// on -- so a clause cannot construct one. The double supplies the one thing the base class is
// missing: concreteness. Same shape and same reason as
// `Source/StratUI/Tests/StratProductionMenuHostDouble.h`.
//
// IT ADDS ONE THING THAT DOUBLE DELIBERATELY DOES NOT, AND THE DIFFERENCE IS ARGUED RATHER THAN
// COPIED. `StratProductionMenuHostDouble.h` states the rule -- a double that keeps its own
// bookkeeping lets a clause go green comparing the double against itself. That rule binds a
// double standing in for an object whose OWN state is the subject. Here the subject is a
// BROADCAST: `FStratAudioOptionsCommitted` is a dynamic multicast delegate, and a dynamic
// delegate can only be bound to a `UFUNCTION` on a `UObject`. There is no way to observe a
// broadcast without an observer. So the counter below is not a second copy of the widget's
// state -- it is the receiving end of the thing being measured, and every clause still reads
// the widget's own `Model` for the values.
//
// IT OVERRIDES NOTHING ON THE BASE CLASS. In particular it does not and cannot override
// `OnAudioOptionsRefreshed`, which is a `BlueprintImplementableEvent`: that function has no C++
// body to extend, and with no Blueprint under it the call is a no-op. What a refresh DID is
// therefore observed through `UStratOptionsWidget::Model`, which the widget writes before it
// fires the event, and the firing itself is not pinned by any clause here -- said in the
// clause file too, where a reader looking for it will be.
//
// THIS HEADER IS NOT INCLUDABLE FROM StratPlay and must not be made so. It reaches
// `Blueprint/UserWidget.h` through `StratOptionsWidget.h`, and `UMG` is a PRIVATE dependency of
// StratUI -- `StratGuidanceRouteProbe.h` records the ~60 `LNK2019` that measurement cost.
#pragma once

#include "CoreMinimal.h"

#include "StratOptionsWidget.h"

#include "StratOptionsWidgetDouble.generated.h"

/** Concrete `UStratOptionsWidget` for automation only. Holds a commit tally and nothing else. */
UCLASS()
class UStratOptionsWidgetDouble : public UStratOptionsWidget
{
	GENERATED_BODY()

public:
	/** Bind with `OnAudioOptionsCommitted.AddDynamic(Double, &UStratOptionsWidgetDouble::
	 *  HandleCommitted)`. A dynamic multicast delegate cannot bind a lambda or a plain
	 *  function, which is the whole reason this class carries a member at all. */
	UFUNCTION()
	void HandleCommitted(const FStratAudioOptionsModel& InModel)
	{
		++CommitCount;
		LastCommitted = InModel;
	}

	/** How many times the widget has broadcast a commit to this observer. */
	int32 CommitCount = 0;

	/** The model carried by the most recent broadcast. Meaningless while `CommitCount` is 0,
	 *  and every clause checks the count before reading it. */
	UPROPERTY()
	FStratAudioOptionsModel LastCommitted;
};
