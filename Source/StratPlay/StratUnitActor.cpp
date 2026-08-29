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

	// §2.11.2's TWO on-map markers, added 2026-08-29. Constructed exactly like `GuidedMarker`
	// above -- attached to `Body` so they follow the unit through `SetActorLocation` with no
	// second placement path, and HIDDEN BY DEFAULT for that component's stated reason: a unit
	// that is never observed is a unit with no marker, where defaulting to visible would put
	// an `H` on all ten units for the whole of any path that spawns an actor without applying
	// a view.
	//
	// THE THREE COLLISION SETTINGS ARE REPEATED PER COMPONENT AND NOT INHERITED FROM
	// `SetActorEnableCollision(false)` BELOW, which is the discipline the constructor comment
	// twenty lines up asked for by name: "clearing the actor-level flags stops a later
	// component added to this actor from quietly reintroducing a blocker". These are two more
	// such components and they get the settings explicitly.
	FlagMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMarker"));
	FlagMarker->SetupAttachment(Body);
	FlagMarker->SetVisibility(false);
	FlagMarker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlagMarker->SetCollisionProfileName(TEXT("NoCollision"));
	FlagMarker->SetCanEverAffectNavigation(false);
	FlagMarker->SetGenerateOverlapEvents(false);
	FlagMarker->SetCastShadow(false);

	UnactedPip = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnactedPip"));
	UnactedPip->SetupAttachment(Body);
	UnactedPip->SetVisibility(false);
	UnactedPip->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UnactedPip->SetCollisionProfileName(TEXT("NoCollision"));
	UnactedPip->SetCanEverAffectNavigation(false);
	UnactedPip->SetGenerateOverlapEvents(false);
	UnactedPip->SetCastShadow(false);

	SetActorEnableCollision(false);
}

void AStratUnitActor::ConfigureMarker(UStaticMeshComponent* const Marker, UStaticMesh* const Mesh,
                                      UMaterialInterface* const Material, const FVector& Offset,
                                      const TCHAR* const MarkerName)
{
	// NULL IS TOLERATED AND RETURNS. A Blueprint can fail to construct a component; a match
	// should not end because one did. This mirrors the guard `BeginPlay` carried inline
	// before this helper existed.
	if (Marker == nullptr)
	{
		return;
	}

	Marker->SetRelativeLocation(Offset);

	if (Mesh != nullptr)
	{
		Marker->SetStaticMesh(Mesh);
		if (Material != nullptr)
		{
			Marker->SetMaterial(0, Material);
		}
		return;
	}

	// ONCE PER ACTOR AT SPAWN, not once per `ApplyUnitView`, which runs on every refresh for
	// every unit. Logged at all because an unconfigured marker and a unit that simply is not
	// marked are indistinguishable on screen -- `IsGuidedMarkerVisible` measured that: the
	// accessor answers TRUE for a marked unit whose marker has no mesh, because
	// `USceneComponent::IsVisible` never consults the static mesh. THIS LOG IS THEREFORE THE
	// ONLY DISCRIMINATOR THE PROJECT HAS, and it is why the three markers share one helper
	// rather than three hand-copied blocks: an omitted log is silent by construction.
	//
	// AT Log AND NOT Warning, for the reason the missing-body-mesh line gives: an unassigned
	// mesh is a content-lane configuration gap, not a failure of the match.
	UE_LOG(LogStratPlay, Log,
		TEXT("Unit actor '%s' has no mesh set for its %s marker; it will not draw."),
		*GetName(), MarkerName);
}

void AStratUnitActor::BeginPlay()
{
	Super::BeginPlay();

	// The markers' meshes and materials are applied here rather than in the constructor
	// because the properties they read are Blueprint defaults, and a constructor running on
	// the CDO sets them on the wrong object -- the reason `AStratBoardActor::BeginPlay` gives
	// for the overlays. Unset is a content-lane gap and not a match failure.
	//
	// [RESHAPED 2026-08-29. The guided marker's nine lines used to sit inline here, opening
	// with an EARLY `return` on `GuidedMarker == nullptr`. That return was correct while there
	// was one marker and became a HAZARD the moment there were three: a null guided marker
	// would have skipped the flag marker's and the pip's placement entirely, silently, with a
	// green build. The guard is now per-marker inside `ConfigureMarker` and there is no early
	// exit from this function.]
	//
	// THE GUIDED MARKER'S PLACEMENT IS UNCHANGED. `GuidedMarkerZOffset` is still the scalar
	// it was and is widened at this call site, so the shipped offset is bit-identical to what
	// `BP_StratUnit` produced before -- see `FlagMarkerOffset` on why the two NEW markers take
	// a vector instead.
	ConfigureMarker(GuidedMarker, GuidedMarkerMesh, GuidedMarkerMaterial,
		FVector(0.0, 0.0, static_cast<double>(GuidedMarkerZOffset)), TEXT("guided-opening turn-1a"));

	ConfigureMarker(FlagMarker, FlagMarkerMesh, FlagMarkerMaterial, FlagMarkerOffset,
		TEXT("flag H"));

	ConfigureMarker(UnactedPip, UnactedPipMesh, UnactedPipMaterial, UnactedPipOffset,
		TEXT("unacted pip"));
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

	// §2.11.2's flag `H`. ONE PUBLISHED FIELD, NOT ANDed WITH ANYTHING, and the absence of a
	// side test is the specification: the earn-your-pixels row is `Flag `H` marker (both
	// sides, always visible)`. "Both sides" rules out `View.Side == ViewingSide`; "always
	// visible" rules out any window bit. The line directly above HAS a side test, so the
	// contrast is stated here rather than left for a reader to infer -- that filter was a user
	// ruling about a marker that says "select this", and this one says "this is the flag",
	// about a unit the other seat is meant to HUNT.
	//
	// ITS OPERANDS ARE MATCH-CONSTANT AND THAT IS CORRECT HERE, WHICH IS THE ONE CLAIM ON
	// THIS LINE THAT HAD TO BE ARGUED. The 2026-08-24 defect above was a conjunction that
	// could not go false; `bIsFlag` cannot either. The difference is that §2.11.2 ASKS for a
	// permanent marker here where §2.11.6 said the turn-1a marker clears. The question that
	// finding really poses -- by what route does this ever stop drawing? -- is answered and
	// not ducked: THE FLAG UNIT'S DEATH DESTROYS THE ACTOR. `FStratViewModel::Units` is every
	// LIVING unit and `UStratMatchSubsystem::ApplyView` destroys the actor for any id the
	// model no longer carries, so the hide path is actor destruction rather than a visibility
	// write, and §2.4 ends the match in the same breath.
	//
	// SET UNCONDITIONALLY IN BOTH DIRECTIONS ANYWAY, like every other line in this function.
	// The write costs nothing and a writer that only ever SHOWS is a writer whose hide can be
	// missed on a path nobody has thought of yet.
	if (FlagMarker != nullptr)
	{
		FlagMarker->SetVisibility(View.bIsFlag);
	}

	// §2.11.2's unacted pip. TWO PUBLISHED FIELDS -- `!View.bDone` and `View.Side ==
	// ViewingSide` -- both off the ONE model this function was handed. Nothing computed,
	// nothing looked up, no hex compared.
	//
	// `bDone` AND NOT `bHasMoved && bHasActed`, AND THE GDD PICKED IT. §2.11.1: "Every surface
	// in §2.11 that says a unit *has not acted* binds to the machine's bit: ... the idle count
	// and the unacted pip (§2.11.2)", and it names the failure of the alternative in the same
	// breath -- "a waited unit would keep its pip". A WAIT reaches DONE without spending
	// either turn flag (`StratSelectionMachine.h`), so a flag-derived pip would stay lit on a
	// unit the player has already retired, which is the exact opposite of what the row
	// promises: *which units I can still give an order to*.
	//
	// `bLockedThisTurn` IS NOT AN OPERAND, and the omission is deliberate rather than missed.
	// See the declaration of `UnactedPip`: §2.11.2's row and §2.11.1's enumeration both name
	// the DONE bit and only the DONE bit, and adding a second would be this class deciding a
	// content rule the GDD did not state.
	//
	// THE SIDE TEST IS THE ROW'S OWN WORDS -- "Unacted pip on OWN UNITS" -- and `ViewingSide`
	// is `FStratViewModel::ViewingSide` off the same model `View` came from, for the reason
	// the marker above it gives.
	//
	// THIS OPERAND SET HAS A REAL FALSE-GOER, which is the check the 2026-08-24 correction
	// demands of every new marker here: `bDone` moves several times a turn and resets at the
	// turn boundary, so this conjunction goes false and true again inside one match under
	// ordinary play. Both directions is a property of the operands and not only of the writer.
	if (UnactedPip != nullptr)
	{
		UnactedPip->SetVisibility(!View.bDone && View.Side == ViewingSide);
	}
}

bool AStratUnitActor::IsGuidedMarkerVisible() const
{
	// Off the component and never off `LastAppliedView` -- see the declaration.
	return GuidedMarker != nullptr && GuidedMarker->IsVisible();
}

bool AStratUnitActor::IsFlagMarkerVisible() const
{
	// Off the component and never off `LastAppliedView.bIsFlag` -- see the declaration, which
	// also states the measured limit this inherits: `IsVisible` never consults the static
	// mesh, so this reports a flag and not pixels.
	return FlagMarker != nullptr && FlagMarker->IsVisible();
}

bool AStratUnitActor::IsUnactedPipVisible() const
{
	// Off the component and never off `LastAppliedView.bDone` -- see the declaration.
	return UnactedPip != nullptr && UnactedPip->IsVisible();
}
