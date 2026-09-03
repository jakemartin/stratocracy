// TEST-ONLY. See the header for why a subclass is the only route to a non-zero
// `MoveTweenSeconds` and a non-zero `BodyZOffset` from inside `Tests/`.
#include "Tests/StratRouteTweenUnitDouble.h"

#include "Components/StaticMeshComponent.h"

AStratRouteTweenUnitDouble::AStratRouteTweenUnitDouble()
{
	// A POSITIVE DURATION, WHICH IS WHAT MAKES `ApplyUnitView`'S ARMING BRANCH REACHABLE AT
	// ALL. Its exact value is never asserted and cannot be: `Tick` never runs in these
	// fixtures -- no `BeginPlay`, no world tick -- so nothing here observes a second of it.
	MoveTweenSeconds = 0.25f;

	// A NON-ZERO LIFT, WHICH IS WHAT MAKES THE HEIGHT CLAUSE ABLE TO FAIL. At zero the tile
	// plane and the drawn unit are the same height and the clause asserts a tautology.
	BodyZOffset = 37.0f;
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
