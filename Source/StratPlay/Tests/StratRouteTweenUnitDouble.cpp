// TEST-ONLY. See the header for why a subclass is the only route to a non-zero
// `MoveTweenSeconds`, a non-zero `BodyZOffset` and a non-zero `DamageFlashSeconds` from inside
// `Tests/`, and for why `AStratShippedDefaultUnitDouble` at the bottom of this file writes
// nothing at all.
#include "Tests/StratRouteTweenUnitDouble.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"

AStratRouteTweenUnitDouble::AStratRouteTweenUnitDouble()
{
	// A POSITIVE DURATION, WHICH IS WHAT MAKES `ApplyUnitView`'S ARMING BRANCH REACHABLE AT
	// ALL. Its exact value is never asserted and cannot be: `Tick` never runs in these
	// fixtures -- no `BeginPlay`, no world tick -- so nothing here observes a second of it.
	MoveTweenSeconds = 0.25f;

	// A NON-ZERO LIFT, WHICH IS WHAT MAKES THE HEIGHT CLAUSE ABLE TO FAIL. At zero the tile
	// plane and the drawn unit are the same height and the clause asserts a tautology.
	BodyZOffset = 37.0f;

	// A POSITIVE FLASH DURATION, WHICH IS WHAT MAKES `PlayDamageFlash`'S ARMING BRANCH
	// REACHABLE AT ALL -- its FIRST statement is `if (Shake == nullptr || DamageFlashSeconds <=
	// 0.0f) { return; }`, and the shipped C++ default is `0.0f`. Its exact value is never
	// asserted and cannot be, for `MoveTweenSeconds`' reason above: nothing here advances a
	// clock. Every clause that arms reads this back through `DamageFlashSecondsForFixture()`
	// and asserts only that it is POSITIVE, so the measurement follows this file if it is
	// retuned.
	DamageFlashSeconds = 0.20f;

	// A NON-ZERO AMPLITUDE, WHICH NO CLAUSE OBSERVES AND WHICH IS SET ANYWAY. `TickDamageFlash`
	// is the only reader and it never runs headless, so a zero here would change no assertion
	// in this project today. It is non-zero so that the fixture is not itself the reason a
	// future clause -- one written against a seam that does advance the clock -- would measure
	// a displacement of nothing. `DamageShakeFrequency` is deliberately NOT set; see the
	// header on why the omission is the point.
	DamageShakeAmplitude = 8.0f;
}

bool AStratRouteTweenUnitDouble::HasBody() const
{
	return Body != nullptr;
}

FVector AStratRouteTweenUnitDouble::BodyRelativeLocation() const
{
	// A READ AND NOT A DERIVATION. The null answer is `FVector::ZeroVector` deliberately -- it
	// is the least surprising value -- and it is EXACTLY why `HasBody` is a separate question:
	// a clause asserting "the picture is home" must not be satisfied by an actor that has no
	// picture. See the header.
	return Body != nullptr ? Body->GetRelativeLocation() : FVector::ZeroVector;
}

bool AStratRouteTweenUnitDouble::HasShake() const
{
	return Shake != nullptr;
}

FVector AStratRouteTweenUnitDouble::ShakeRelativeLocation() const
{
	// A READ AND NOT A DERIVATION, on `BodyRelativeLocation`'s terms and for its reason. The
	// null answer is `FVector::ZeroVector`, which is EXACTLY why `HasShake` is a separate
	// question: it is the same value retirement writes.
	return Shake != nullptr ? Shake->GetRelativeLocation() : FVector::ZeroVector;
}

UMaterialInterface* AStratRouteTweenUnitDouble::BodyOverlayMaterial() const
{
	// `GetOverlayMaterial` AND NOT `GetMaterial(0)`. See the header: they are different
	// channels, and slot 0 is the side colour `ApplyUnitView` rewrites on every refresh.
	return Body != nullptr ? Body->GetOverlayMaterial() : nullptr;
}

void AStratRouteTweenUnitDouble::SetDamageFlashMaterialForFixture(UMaterialInterface* const Material)
{
	DamageFlashMaterial = Material;
}

float AStratRouteTweenUnitDouble::DamageFlashSecondsForFixture() const
{
	return DamageFlashSeconds;
}

// ---------------------------------------------------------------------------
// `AStratShippedDefaultUnitDouble` -- FIVE ACCESSORS AND NOT ONE WRITE. See the header: the
// absence of a constructor is the property, so there is nowhere a value could be set.
// ---------------------------------------------------------------------------

bool AStratShippedDefaultUnitDouble::HasShake() const
{
	return Shake != nullptr;
}

FVector AStratShippedDefaultUnitDouble::ShakeRelativeLocation() const
{
	return Shake != nullptr ? Shake->GetRelativeLocation() : FVector::ZeroVector;
}

UMaterialInterface* AStratShippedDefaultUnitDouble::BodyOverlayMaterial() const
{
	return Body != nullptr ? Body->GetOverlayMaterial() : nullptr;
}

void AStratShippedDefaultUnitDouble::SetDamageFlashMaterialForFixture(UMaterialInterface* const Material)
{
	// THE ONE WRITE THIS CLASS PERMITS, AND IT IS THE FIXTURE'S AND NOT THE CLASS'S. It exists
	// so `NoDamageFlashAtTheShippedDefault`'s overlay assertion is about the `<= 0` GUARD
	// rather than about an unset material; see the header. `DamageFlashSeconds` is untouched by
	// it and stays at the shipped C++ default, which is the whole subject of that clause.
	DamageFlashMaterial = Material;
}

float AStratShippedDefaultUnitDouble::DamageFlashSecondsForFixture() const
{
	return DamageFlashSeconds;
}
