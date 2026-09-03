// TEST-ONLY. A unit actor whose two tween-relevant Blueprint properties are non-zero.
//
// WHY IT EXISTS AT ALL, AND WHY IT IS NOT AN OPTIONAL CONVENIENCE. `AStratUnitActor` ships
// with `MoveTweenSeconds = 0.0f` and `BodyZOffset = 0.0f` as its C++ defaults, and both are
// `EditDefaultsOnly` -- the shipped values live on `BP_StratUnit`, a `/Game/` asset no
// headless clause may name and no `Tests/` file may author. Against the raw C++ class:
//
//   - `MoveTweenSeconds <= 0` takes `ApplyUnitView`'s no-animation path, so NO TWEEN IS EVER
//     ARMED and every waypoint clause below would assert over an empty polyline. It would
//     pass, and it would pass equally over a route implementation, a chord implementation and
//     no implementation.
//   - `BodyZOffset == 0` makes the height clause VACUOUS. Its whole subject is that the
//     waypoints sit at the height the unit is DRAWN at rather than on the tile plane, and at
//     zero lift those two are the same number. The defect it exists to catch -- differencing
//     the route against the actor's location instead of against the route's own last point --
//     is invisible at zero and grows exactly as `BodyZOffset` does.
//
// So the clause needs an actor with both non-zero, and the only route to one from inside
// `Tests/` is a subclass: both members are `protected`, which a derived class may write and a
// clause may not. This is that subclass and it is the whole of it.
//
// IT SETS TWO VALUES AND OVERRIDES NOTHING. No virtual is replaced, no component is added, no
// behaviour is observed here -- every measurement in `StratMatchReconcile.cpp` is taken from
// `AStratUnitActor`'s own accessors on the shipped code path. This class cannot make a clause
// pass; it can only make one REACHABLE. That is the line `StratProductionMenuHostDouble.h`
// draws for the widget doubles and it holds here unchanged.
//
// [AMENDED 2026-09-02, SAME DAY, BY THE AI ROUTE SLIDE'S PARK. WRITTEN FLAT RATHER THAN
// APPENDED TO, BECAUSE THE SENTENCE ABOVE SAID "NO BEHAVIOUR IS OBSERVED HERE" AND THAT IS NO
// LONGER THE WHOLE TRUTH.] It said:
// RETRACTED> "No virtual is replaced, no component is added, no behaviour is observed here."
// Two CONST READS were added below -- `HasBody` and `BodyRelativeLocation`. What is retracted
// is only the observation half; NO VIRTUAL IS STILL REPLACED, NO COMPONENT IS STILL ADDED, and
// nothing here can make a clause pass.
//
// WHY THEY EXIST AND WHY A SUBCLASS IS THE ONLY ROUTE. `AStratUnitActor::PlayRouteSlide` PARKS:
// it retires with `Body` at `TweenRestOffset` rather than at zero, so "the picture is home" and
// "the picture is stranded over an intermediate hex" are two DIFFERENT states that
// `GetTweenWaypointCount() == 0` reports IDENTICALLY -- both have an empty polyline. The
// distinguishing fact is `Body`'s relative location, and `Body` is a protected member with no
// public accessor. `T-TURN-09.SkipLeavesNoPictureStranded` and
// `T-INT-05.AnActorThatMovedWithNoRouteClearsItsPark` are about exactly that difference, so
// without these two reads both would pass over an implementation that emptied the arrays and
// left the picture parked forever.
//
// THEY READ AND THEY DO NOT DERIVE. `BodyRelativeLocation` returns
// `USceneComponent::GetRelativeLocation()` verbatim -- the same value `FinishTween` writes and
// `Tick` lerps -- and computes nothing. `HasBody` is separate on purpose: a null `Body` would
// otherwise be indistinguishable from a picture at rest, which is the vacuity a clause asserting
// "exactly zero" is most exposed to.
//
// THE TWO NUMBERS ARE FIXTURE INPUT AND NEVER AN EXPECTATION, and they are deliberately NOT
// the shipped Blueprint's. `Tools/architect/state/content.md` records `BP_StratUnit` carrying
// `BodyZOffset = 50.0` and `MoveTweenSeconds = 0.2`; copying those here would invite a later
// reader to treat this file as a second, unhash-gated statement of the authored defaults and
// to "fix" a drift between them. 37 and 0.25 are chosen to look like what they are. No clause
// asserts either value: the height clause reads the lift back off the drawn actor as
// `GetActorLocation().Z - WorldLocationOfHex(hex).Z` and asserts only that it is non-zero, so
// the measurement follows this file if it is ever retuned.
#pragma once

#include "CoreMinimal.h"

#include "StratUnitActor.h"

#include "StratRouteTweenUnitDouble.generated.h"

UCLASS()
class AStratRouteTweenUnitDouble : public AStratUnitActor
{
	GENERATED_BODY()

public:
	AStratRouteTweenUnitDouble();

	/**
	 * Whether this actor has a body component at all.
	 *
	 * ASKED SEPARATELY FROM THE POSITION, AND THAT IS NOT PEDANTRY: `BodyRelativeLocation`
	 * answers `FVector::ZeroVector` for a null `Body`, which is the SAME answer a picture at
	 * rest gives. A clause whose whole assertion is "exactly zero" would then be passed by an
	 * actor with no body at all.
	 */
	bool HasBody() const;

	/**
	 * Where `Body` sits relative to this actor, read verbatim.
	 *
	 * NOTHING IS DERIVED HERE. This is `USceneComponent::GetRelativeLocation()` -- the value
	 * `AStratUnitActor::FinishTween` writes from `TweenRestOffset` and the value `Tick` lerps
	 * along `TweenOffsets`. It is the ONLY thing that distinguishes a retired tween that came
	 * home from a retired tween that parked, because both report a waypoint count of zero.
	 */
	FVector BodyRelativeLocation() const;
};
