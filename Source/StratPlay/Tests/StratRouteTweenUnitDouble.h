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
//
// [AMENDED 2026-09-06 BY THE DAMAGE FLASH. WRITTEN FLAT, IN THIS FILE'S OWN `RETRACTED>`
// CONVENTION, BECAUSE THE HEADING SENTENCE COUNTED THE VALUES AND A READER GREPS THAT COUNT.]
// It said, in its opening line and again below it:
// RETRACTED> "A unit actor whose two tween-relevant Blueprint properties are non-zero."
// RETRACTED> "IT SETS TWO VALUES AND OVERRIDES NOTHING."
// IT NOW SETS FOUR AND STILL OVERRIDES NOTHING. `DamageFlashSeconds` and
// `DamageShakeAmplitude` join `MoveTweenSeconds` and `BodyZOffset` for the identical reason the
// first two are here: both ship at `0.0f` as C++ defaults, both are `EditDefaultsOnly`, and at
// their shipped values `AStratUnitActor::PlayDamageFlash` RETURNS HAVING WRITTEN NOTHING -- so
// every damage-flash clause would assert over an actor that never armed, and would pass equally
// over a correct implementation, a broken one, and no implementation at all. NO VIRTUAL IS
// STILL REPLACED AND NO COMPONENT IS STILL ADDED.
//
// `DamageShakeFrequency` IS DELIBERATELY NOT SET, AND THE OMISSION IS THE POINT. It already
// ships non-zero, and nothing headless reads it -- `AStratUnitActor::Tick` never runs in these
// fixtures, so frequency, amplitude curve, decay shape and smoothness are all unobservable.
// Restating it here would make this file a second home for a number no clause can see.
//
// `DamageFlashMaterial` IS SET BY THE CLAUSE AND NOT BY THIS CONSTRUCTOR, WHICH IS A DECISION
// AND NOT AN OVERSIGHT. A constructor runs on the CDO at module load, and a `NewObject` there
// would hang a transient object off a class default for the life of the process. So the fixture
// material arrives through `SetDamageFlashMaterialForFixture` below, from a clause that made
// it -- which also lets that clause compare `BodyOverlayMaterial()` against THE VERY POINTER IT
// HANDED OVER, rather than against "not null". It is a transient `NewObject<UMaterial>` and
// never a `/Game/` asset: automation fixtures are one of the two places
// `.agents/ue-project-context.md` licenses that literal, and there is no reason to spend it on
// an object nothing renders.
//
// THE FOUR CONST READS BELOW ARE READS AND NOT DERIVATIONS, on `BodyRelativeLocation`'s stated
// permission and for its reason: `Body` and `Shake` are protected members with no public
// accessor, and the properties every damage clause is about are that component's transform and
// that mesh's overlay channel. `HasShake` is separate from `ShakeRelativeLocation` for exactly
// the reason `HasBody` is separate from `BodyRelativeLocation` -- a null component answers
// `FVector::ZeroVector`, which is the SAME answer the retired state gives, so a clause whose
// whole assertion is "exactly zero" would otherwise be satisfied by an actor with no `Shake`.
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

	/**
	 * Whether this actor has the damage shake's transform component at all.
	 *
	 * ASKED SEPARATELY FROM THE POSITION FOR `HasBody`'S REASON, VERBATIM: a null `Shake` makes
	 * `ShakeRelativeLocation()` answer `FVector::ZeroVector`, which is the SAME answer
	 * `AStratUnitActor::FinishDamageFlash` writes on retirement -- so the exactly-zero clause
	 * would be passed by an actor that has no shake component at all. It is also the control
	 * for the whole arming path: `AStratUnitActor::PlayDamageFlash` REFUSES on a null `Shake`,
	 * so a fixture without one would report "no flash armed" for a reason that has nothing to
	 * do with the property under test.
	 */
	bool HasShake() const;

	/**
	 * Where `Shake` sits relative to `Body`, read verbatim.
	 *
	 * NOTHING IS DERIVED HERE. This is `USceneComponent::GetRelativeLocation()` -- the value
	 * `AStratUnitActor::TickDamageFlash` writes and the value `FinishDamageFlash` puts back to
	 * exact zero through `SetRelativeLocation_Direct` plus `UpdateComponentToWorld`.
	 */
	FVector ShakeRelativeLocation() const;

	/**
	 * `Body`'s OVERLAY material, read verbatim through `UMeshComponent::GetOverlayMaterial`.
	 *
	 * THE OVERLAY CHANNEL AND NOT SLOT 0, WHICH IS THE DISTINCTION THE WHOLE FEATURE RESTS ON:
	 * `AStratUnitActor::ApplyUnitView` rewrites slot 0 on every refresh, and the flash writes a
	 * channel that function never touches. A clause reading `GetMaterial(0)` would be reading
	 * the side colour and would be green over a flash that never happened.
	 *
	 * IT IS A POINTER AND NOT A COLOUR. Nothing here can say anything red reached a screen --
	 * the suite runs `-nullrhi`. This answers only WHICH OBJECT is assigned.
	 */
	UMaterialInterface* BodyOverlayMaterial() const;

	/**
	 * Hands this actor a flash material a clause made, because a constructor may not.
	 *
	 * SEE THE HEADER BLOCK. It is a WRITE and it is fixture input; no clause asserts anything
	 * about the object itself, only that the overlay channel holds THE POINTER THAT WAS HANDED
	 * OVER while a flash runs, and `nullptr` once it retires.
	 */
	void SetDamageFlashMaterialForFixture(UMaterialInterface* Material);

	/**
	 * `DamageFlashSeconds` as this actor actually holds it.
	 *
	 * FOR A CONTROL AND NEVER FOR AN EXPECTATION. Every clause that arms a flash is vacuous if
	 * the duration is the shipped `0.0f`, and every clause that asserts a flash is REFUSED is
	 * vacuous if it is not -- so both directions read this back off the actor rather than
	 * trusting the constructor above, or the C++ default, to still be what it was.
	 */
	float DamageFlashSecondsForFixture() const;
};

/**
 * TEST-ONLY. `AStratUnitActor` WITH NOTHING WRITTEN TO IT AND FIVE ACCESSORS BOLTED ON.
 *
 * WHY IT EXISTS SEPARATELY FROM THE DOUBLE ABOVE, AND WHY IT IS NOT THAT DOUBLE WITH A FLAG.
 * `T-INT-05.NoDamageFlashAtTheShippedDefault` is about what the SHIPPED C++ CLASS does when
 * `PlayDamageFlash()` is called on it: nothing arms, no tick is enabled, no material is
 * touched. That property is only assertable against an actor whose properties are the C++
 * defaults, and the double above is defined by NOT having them.
 *
 * IT DECLARES NO CONSTRUCTOR, WHICH IS THE POINT AND NOT AN ECONOMY. There is no constructor
 * body in which a property could be written, by this pass or by a later one, so the class holds
 * `AStratUnitActor`'s own defaults BY CONSTRUCTION rather than by inspection.
 *
 * IT STILL TAKES `SetDamageFlashMaterialForFixture`, AND THAT IS WHAT MAKES THE OVERLAY HALF OF
 * ITS CLAUSE MEAN ANYTHING. `AStratUnitActor::PlayDamageFlash` writes the overlay only when
 * `DamageFlashMaterial != nullptr`, so against an actor with no material assigned the overlay
 * is `nullptr` WHETHER OR NOT the `<= 0` guard is honoured -- the assertion would be green over
 * the very mutant it exists to kill. With a material assigned and the duration left at the
 * shipped default, `nullptr` in the overlay channel is evidence about THE GUARD.
 */
UCLASS()
class AStratShippedDefaultUnitDouble : public AStratUnitActor
{
	GENERATED_BODY()

public:
	/** See `AStratRouteTweenUnitDouble::HasShake`. */
	bool HasShake() const;

	/** See `AStratRouteTweenUnitDouble::ShakeRelativeLocation`. */
	FVector ShakeRelativeLocation() const;

	/** See `AStratRouteTweenUnitDouble::BodyOverlayMaterial`. */
	UMaterialInterface* BodyOverlayMaterial() const;

	/** See `AStratRouteTweenUnitDouble::SetDamageFlashMaterialForFixture`. */
	void SetDamageFlashMaterialForFixture(UMaterialInterface* Material);

	/** See `AStratRouteTweenUnitDouble::DamageFlashSecondsForFixture`. */
	float DamageFlashSecondsForFixture() const;
};
