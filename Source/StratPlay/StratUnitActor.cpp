// GDD §2.4 / §4.7 Stub 8 -- one unit, implementation side.
//
// EVERY LINE BELOW IS A COPY OUT OF THE VIEW MODEL. Nothing here reads a rule, computes a
// value, or decides what a unit may do. No `strat::` type appears and `StratBridge.h` is
// not included.

#include "StratUnitActor.h"

#include "StratPlay.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"

AStratUnitActor::AStratUnitActor()
{
	// NO TICK. The unit is reconciled against the model when the model changed, by whoever
	// changed it -- the same posture the board and the scoreboard take.
	PrimaryActorTick.bCanEverTick = false;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	SetRootComponent(Body);

	// NO COLLISION, and the header block records why this is load-bearing rather than a
	// performance choice: the cursor's job is to name a HEX, it does that by tracing to the
	// board's tile instances, and a unit that blocked the trace would shadow the tile
	// underneath it. The hex a player most needs to click is the one an enemy is standing
	// on.
	//
	// Set three ways on purpose. `SetCollisionEnabled` is the one that matters; the profile
	// makes the intent readable in the details panel, and clearing the actor-level flags
	// stops a later component added to this actor from quietly reintroducing a blocker.
	Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Body->SetCollisionProfileName(TEXT("NoCollision"));
	Body->SetCanEverAffectNavigation(false);
	Body->SetGenerateOverlapEvents(false);

	SetActorEnableCollision(false);
}

void AStratUnitActor::ApplyUnitView(const FStratUnitView& View, const FVector& WorldLocation)
{
	// THE ID FIRST, because everything else is a description of THIS unit and a mismatch
	// here means the caller reconciled against the wrong actor. Overwritten rather than
	// checked: `UStratMatchSubsystem` keys its map by the same value, so a disagreement
	// would be a defect in that map rather than in this actor, and asserting here would
	// crash the game to report somebody else's bookkeeping.
	UnitId = View.UnitId;

	// TOTAL, per the declaration. The whole view is cached, including the two presentation
	// bits, so nothing in this class ever holds a fragment of a model it cannot re-derive
	// by looking at what it was last handed.
	LastAppliedView = View;

	if (Body != nullptr)
	{
		UStaticMesh* Desired = nullptr;
		if (const TObjectPtr<UStaticMesh>* const Configured = MeshByDefId.Find(View.DefId))
		{
			Desired = *Configured;
		}
		else
		{
			Desired = FallbackMesh;

			// Logged at Log and not Warning: an unassigned mesh is a phase-5 configuration
			// gap, not a failure of the match. It is logged at all because an invisible unit
			// is otherwise indistinguishable from a unit the rules module does not have,
			// and those have very different fixes.
			UE_LOG(LogStratPlay, Log,
				TEXT("No mesh assigned for unit definition '%s' (unit %d); using the fallback."),
				*View.DefId.ToString(), View.UnitId);
		}

		// ASSIGNED UNCONDITIONALLY RATHER THAN ONLY WHEN IT CHANGED. `SetStaticMesh` is
		// already a no-op for the same mesh, and a guard here would be a second place the
		// "has it changed" question is answered -- see the declaration on why this call is
		// total.
		if (Body->GetStaticMesh() != Desired)
		{
			Body->SetStaticMesh(Desired);
		}

		// SIDE IS A MATERIAL. An index past the end leaves the mesh's own material, which
		// reads as "both sides look the same" -- visibly wrong and harmless -- rather than
		// as a crash in the middle of a turn.
		if (SideMaterials.IsValidIndex(View.Side) && SideMaterials[View.Side] != nullptr)
		{
			Body->SetMaterial(0, SideMaterials[View.Side]);
		}
	}

	// TELEPORTED, NOT INTERPOLATED. The header block records why: animating a move needs
	// the ordered event list §4.9 part 2 names and `StratBridge.h` records as not built.
	// Sliding towards a position difference would be inferring what HAPPENED from what IS,
	// which is the conflation that header warns about by name.
	SetActorLocation(WorldLocation + FVector(0.0, 0.0, static_cast<double>(BodyZOffset)));
}
