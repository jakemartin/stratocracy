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

	// §2.11.6-B's turn-1a marker. Attached to `Body` and not to the actor root separately,
	// so it follows the unit through `SetActorLocation` with no second placement path -- the
	// same reason `ApplyUnitView` takes the world location rather than computing one.
	//
	// HIDDEN BY DEFAULT, which is the safe direction: a unit that is never observed is a unit
	// with no marker, where a marker defaulting to visible would put one on all ten units for
	// the whole of any path that spawns an actor without applying a view.
	GuidedMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GuidedMarker"));
	GuidedMarker->SetupAttachment(Body);
	GuidedMarker->SetVisibility(false);

	// THE COMPONENT THE CONSTRUCTOR COMMENT ABOVE ANTICIPATED. "Clearing the actor-level
	// flags stops a later component added to this actor from quietly reintroducing a
	// blocker" -- this is that later component, and it is given the same three settings
	// explicitly rather than relying on the actor-level flag alone.
	GuidedMarker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GuidedMarker->SetCollisionProfileName(TEXT("NoCollision"));
	GuidedMarker->SetCanEverAffectNavigation(false);
	GuidedMarker->SetGenerateOverlapEvents(false);
	GuidedMarker->SetCastShadow(false);

	SetActorEnableCollision(false);
}

void AStratUnitActor::BeginPlay()
{
	Super::BeginPlay();

	// The marker's mesh and material are applied here rather than in the constructor because
	// the properties they read are Blueprint defaults, and a constructor running on the CDO
	// sets them on the wrong object -- the reason `AStratBoardActor::BeginPlay` gives for the
	// overlays. Unset is a content-lane gap and not a match failure.
	if (GuidedMarker == nullptr)
	{
		return;
	}

	GuidedMarker->SetRelativeLocation(FVector(0.0, 0.0, static_cast<double>(GuidedMarkerZOffset)));

	if (GuidedMarkerMesh != nullptr)
	{
		GuidedMarker->SetStaticMesh(GuidedMarkerMesh);
		if (GuidedMarkerMaterial != nullptr)
		{
			GuidedMarker->SetMaterial(0, GuidedMarkerMaterial);
		}
	}
	else
	{
		// ONCE PER ACTOR AT SPAWN, not once per `ApplyUnitView`, which runs on every refresh
		// for every unit. Logged at all because an unconfigured marker and an unmarked unit
		// are indistinguishable on screen and have entirely different fixes -- the same
		// argument the missing-mesh line above it makes.
		UE_LOG(LogStratPlay, Log,
			TEXT("Unit actor '%s' has no GuidedMarkerMesh set; the guided opening's turn-1a marker will "
			     "not draw."),
			*GetName());
	}
}

void AStratUnitActor::ApplyUnitView(const FStratUnitView& View, const FVector& WorldLocation, int32 ViewingSide,
                                    bool bGuidanceActive)
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

	// §2.11.6-B's turn-1a marker. A PASS-THROUGH OF THREE PUBLISHED FIELDS ANDed -- no
	// comparison against a scenario hex, no lookup, no arithmetic. See the declaration of
	// `GuidedMarker` on why a hex-keyed derivation would unmark the unit at the moment the
	// mark is needed, and on the two rulings that added the second and third operands.
	//
	// THE SIDE TEST IS THE RULING AND IT IS NOT AN OPTIMISATION. The shipped scenario authors
	// a `guidedOpening` for BOTH seats, so two units carry `bIsGuidedMarked` at once; without
	// this the enemy seat's Infantry is marked on the player's screen, telling them to select
	// a unit they cannot select. `ViewingSide` is `FStratViewModel::ViewingSide` off the same
	// model `View` came from -- not `UStratMatchSubsystem::GetViewingSide`, which would be a
	// second input into what should be on screen.
	//
	// [CORRECTED 2026-08-24, AFTER A HUMAN PLAYTEST, BY THE LANE THAT WROTE THE DEFECT.] The
	// two operands above are BOTH CONSTANT FOR THE WHOLE MATCH -- `bIsGuidedMarked` is the
	// rules module's derivation off `placement`, which is exactly why it never moves, and
	// `View.Side` and `ViewingSide` are fixed. So the conjunction could never go false once
	// true and THE MARKER STAYED LIT FOR THE REST OF THE MATCH. Observed at the keyboard: the
	// objective ring cleared and the Infantry's marker did not. The paragraph below was
	// already in this file when that happened, and it was satisfied -- the WRITE ran in both
	// directions on every call, over an operand set that had no false in it. **BOTH
	// DIRECTIONS IS A PROPERTY OF THE OPERANDS AND NOT ONLY OF THE WRITER**, which is the
	// lesson this line cost, and it is why `bGuidanceActive` is now the first operand.
	//
	// `bGuidanceActive` IS `FStratViewModel::Guidance.bActive` -- THE SAME BIT THE STRIP AND
	// THE RING RIDE, AND NOT "BEAT 1a RETIRED". §2.11.6 says the ring "and the turn-1a unit
	// marker clear in the same frame as the strip", and the strip IS `bActive`. Every one of
	// `FStratGuidedOpening`'s three writers of `bHasObjective = false` -- `SkipGuidance`, the
	// turn-4 window close, and the all-beats-retired branch -- sets `bActive = false` in the
	// same statement group, and `DecorateViewModel` assigns the guidance block whole, so the
	// ring going out already proved `bActive` went out. Keying the marker on the same bit
	// makes "the same frame" structural for the marker too rather than a promise: one value,
	// one `ApplyView`, three surfaces.
	//
	// SET UNCONDITIONALLY IN BOTH DIRECTIONS, on every call, like every other line in this
	// function. A writer that only ever SHOWS is a writer whose hide can be missed on one
	// path -- and for this bit that means a permanent marker on a unit §2.11.6's window
	// closed on. `FStratGuidedOpening::PublishLocks` records the same rule for the lock set.
	if (GuidedMarker != nullptr)
	{
		GuidedMarker->SetVisibility(bGuidanceActive && View.bIsGuidedMarked && View.Side == ViewingSide);
	}
}

bool AStratUnitActor::IsGuidedMarkerVisible() const
{
	// Off the component and never off `LastAppliedView` -- see the declaration.
	return GuidedMarker != nullptr && GuidedMarker->IsVisible();
}
