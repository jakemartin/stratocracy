// GDD §2.4 / §4.7 Stub 8 -- one unit, implementation side.
//
// EVERY LINE BELOW IS A COPY OUT OF THE VIEW MODEL. Nothing here reads a rule, computes a
// value, or decides what a unit may do. No `strat::` type appears and `StratBridge.h` is
// not included.

#include "StratUnitActor.h"

#include "StratPlay.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"

AStratUnitActor::AStratUnitActor()
{
	// NO POLLING, AND NO TICK AT ALL UNLESS A VISUAL OFFSET IS DECAYING. The posture is
	// unchanged from the one this comment stated when the line below read
	// `bCanEverTick = false`: the unit is reconciled against the model WHEN THE MODEL CHANGED,
	// by whoever changed it -- the same posture the board and the scoreboard take -- and
	// nothing here ever asks the world whether something happened. The two lines are rewritten
	// rather than left standing under prose they would have contradicted.
	//
	// WHAT TICKS IS A DECAYING VISUAL OFFSET ON `Body` AND NOTHING ELSE. `Tick` writes a
	// relative location and switches itself off when it reaches zero; the ACTOR TRANSFORM is
	// still written in exactly one place, inside `ApplyUnitView`. `bStartWithTickEnabled` is
	// false, so an actor whose Blueprint leaves `MoveTweenSeconds` at its zero default never
	// ticks once -- not a tick that early-returns, no tick at all, which is the old behaviour
	// exactly and not an approximation of it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// AN EMPTY ROOT ABOVE THE BODY, AND IT IS THE WHOLE MOVE TWEEN. `Body` was the root until
	// 2026-09-02 and a root's relative location IS the actor's world location, so there was
	// nowhere to put a visual offset that `GetActorLocation()` did not also report. With
	// `UnitRoot` in front, `SetActorLocation` snaps to the destination as it always did and
	// `Body`'s relative location carries the offset that eases to zero. Same shape as
	// `AStratBoardActor`'s `BoardRoot` and `AStratCameraPawn`'s `PivotRoot`.
	UnitRoot = CreateDefaultSubobject<USceneComponent>(TEXT("UnitRoot"));
	SetRootComponent(UnitRoot);

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(UnitRoot);

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

	// §2.11.6-B's turn-1a marker. Attached to `Body` and NOT to `UnitRoot`, so it follows the
	// unit with no second placement path -- the same reason `ApplyUnitView` takes the world
	// location rather than computing one.
	//
	// THAT CHOICE STOPPED BEING FREE ON 2026-09-02 AND IS NOW LOAD-BEARING, so it is stated
	// here rather than left as the shape it happens to have. `Body` is no longer the root: it
	// carries the move tween's decaying visual offset. A marker on `Body` therefore follows
	// THE VISUAL, arriving with the unit; a marker on `UnitRoot` would follow THE DESTINATION
	// and would sit at the far hex while its unit was still sliding towards it. All three
	// markers stay on `Body` for that reason, and moving any of them up to the root is a
	// visible defect with a green build.
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
	// above -- attached to `Body` so they follow the unit with no second placement path AND so
	// they ride the move tween's visual offset rather than the destination, which is the
	// paragraph above -- and HIDDEN BY DEFAULT for that component's stated reason: a unit
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

namespace
{
	/**
	 * §the trapezoid: normalised DISPLACEMENT at normalised time `A`, for an ease fraction `E`.
	 *
	 * IT IS THE INTEGRAL OF A TRAPEZOIDAL VELOCITY PROFILE and is written as the closed form
	 * rather than accumulated, so it is exact at both ends by algebra instead of by however
	 * many frames happened to elapse. `f(0) == 0` and `f(1) == 1` for every `E` in [0, 0.5].
	 *
	 * `V` IS THE PLATEAU SPEED AND IT IS DERIVED, NOT TUNED. Total displacement must be 1, and
	 * a trapezoid of height `V` over a unit base with ramps of width `E` has area `V*(1-E)`;
	 * setting that to 1 gives `V = 1/(1-E)`. So easing harder makes the middle FASTER by exactly
	 * enough to arrive on time, which is why the caller never has to compensate.
	 *
	 * `E == 0` IS GUARDED AND RETURNS `A`, WHICH IS BOTH THE DIVIDE GUARD AND THE CORRECT
	 * ANSWER. With no ramps the profile is a rectangle and displacement is linear; the general
	 * branches would divide by `2E`.
	 *
	 * A FREE FUNCTION IN AN ANONYMOUS NAMESPACE and not a method: it reads no member, and a
	 * member function would let a later edit reach `TweenElapsed` from inside a curve.
	 */
	double StratTweenTrapezoidDisplacement(double A, double E)
	{
		if (E <= 0.0)
		{
			return A;
		}

		const double V = 1.0 / (1.0 - E);

		if (A <= E)
		{
			return V * A * A / (2.0 * E);
		}

		if (A <= 1.0 - E)
		{
			return V * (E * 0.5 + (A - E));
		}

		const double Remaining = 1.0 - A;
		return 1.0 - V * Remaining * Remaining / (2.0 * E);
	}
}

void AStratUnitActor::ApplyUnitView(const FStratUnitView& View, const FVector& WorldLocation, int32 ViewingSide,
                                    bool bGuidanceActive, const TArray<FVector>& RouteWorldPoints)
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

	// THE ACTOR TELEPORTS. THE PICTURE OF IT CAN SLIDE. Those are two different claims and
	// this block used to make only the first; it is rewritten rather than left standing under
	// a line whose meaning changed.
	//
	// WHAT IS STILL TRUE, AND IT IS THE HEADER BLOCK'S REASON UNWEAKENED: nothing here may
	// LABEL a position difference. Animating a MOVE -- as against an attack, a retreat, a
	// build, or simply a different model being applied -- needs the ordered event list §4.9
	// part 2 names and `StratBridge.h` records as not built, and inferring one from a position
	// difference is the conflation that header warns about by name. Nothing below asks what
	// happened. The offset is applied identically whatever caused the hex to differ.
	//
	// THE START IS MEASURED OFF THE COMPONENT AND IS NEVER A REMEMBERED FIELD, and this is
	// load-bearing twice. (1) It is read from `Body->GetComponentLocation()` BEFORE the snap,
	// so it is where the unit is ACTUALLY DRAWN this instant -- a second `ApplyView` arriving
	// mid-tween restarts from the drawn position and the unit cannot pop. A remembered
	// destination would restart from where the LAST model put it, which is somewhere the unit
	// visibly is not. (2) It means there is no `LastAppliedLocation` field and no
	// `bHasAppliedAView` bit on this actor, which matters because
	// `T-INT-05.NoActorHoldsPresentationBits` is precisely about an actor holding bits the
	// model does not. The one thing the model cannot supply -- "this actor is new, do not
	// animate" -- is known by the spawner and said by it, through `SnapToWorldLocation`.
	const FVector Drawn = (Body != nullptr) ? Body->GetComponentLocation() : FVector::ZeroVector;

	// AND THE ACTOR'S OWN PREVIOUS LOCATION, READ ON THE SAME LINE OF REASONING AND ONE LINE
	// LATER. This is the RE-ENTRANCY GUARD and it is here because A PLAYER MOVE PRODUCES TWO
	// `ApplyView` CALLS, NOT ONE -- `AStratPlayerController::HandleSelectionEvent` submits, the
	// subsystem's own `RefreshPresentation` applies once, and `RefreshFromMachine` applies again
	// with IDENTICAL positions, an ordering the controller documents as deliberate. With a
	// single-chord tween the second apply re-armed a slide of length nothing and was harmless.
	// With a ROUTE it would be fatal in the frame it happened: the second apply's own route is
	// empty, so it would retire the polyline and COLLAPSE THE ROUTE BACK TO A CHORD -- or worse,
	// to a snap -- in the same frame the first one armed it.
	//
	// SO THE ARMING BELOW REQUIRES THAT THE ACTOR ACTUALLY MOVED. Apply #2 finds
	// `PreviousActorLocation` equal to the location it is about to write, arms nothing, retires
	// nothing, and leaves the running route completely alone.
	//
	// THIS IS NEITHER AN EVENT INFERENCE NOR A REMEMBERED BIT, AND BOTH HALVES ARE WORTH SAYING.
	// It is not an inference about what HAPPENED: it does not ask whether the unit moved, was
	// attacked or was re-applied unchanged, and it labels nothing -- it asks only whether THIS
	// CLASS'S OWN LAST TRANSFORM WRITE differs from the one it is about to make. That is the
	// same permission `Drawn` above already exercises, reading back a transform this class wrote
	// itself one call earlier. And it is not a remembered bit: it is a local that does not
	// survive the call, so there is no `LastAppliedLocation` member and
	// `T-INT-05.NoActorHoldsPresentationBits` is untouched.
	const FVector PreviousActorLocation = GetActorLocation();

	// UNCHANGED, BYTE FOR BYTE, AND DELIBERATELY SO. Every synchronous reader of
	// `GetActorLocation()` after `UStratMatchSubsystem::ApplyView` -- notably
	// `T-INT-05.ApplyViewSpawnsMovesAndDestroys` -- sees exactly what it saw before the tween
	// existed, because this is the only line in this class that writes the actor transform and
	// it still writes the destination.
	SetActorLocation(WorldLocation + FVector(0.0, 0.0, static_cast<double>(BodyZOffset)));

	// AND THEN THE PICTURE IS PUT BACK WHERE IT WAS, TO CATCH UP ON ITS OWN CLOCK.
	//
	// `MoveTweenSeconds <= 0` IS THE SHIPPED C++ DEFAULT AND TAKES THE OLD PATH EXACTLY: no
	// offset is written, no tick is enabled, `Body`'s relative location keeps the zero the
	// constructor gave it. The guard is `<= 0` and not `== 0` because a negative duration is a
	// mis-authored Blueprint default and "no animation" is the safe reading of it -- never a
	// division by a negative number.
	//
	// A WORLD-SPACE DIFFERENCE IS APPLIED AS A RELATIVE LOCATION, WHICH IS EXACT ONLY WHILE
	// THIS ACTOR IS UNROTATED. It is true of every path today -- `SetActorLocation` above is
	// this class's ONLY transform write, nothing here ever sets a rotation, and
	// `UStratMatchSubsystem::ApplyView` spawns at `FTransform::Identity` -- and it is NOT
	// ENFORCED ANYWHERE. Written down rather than assumed: if a Blueprint ever rotates a unit,
	// the fix is one call, `UnitRoot->GetComponentTransform().InverseTransformVectorNoScale(...)`
	// on the difference before it is stored, and the tween would then be visibly skewed until
	// somebody made it.
	// AN EMPTY ROUTE IS NO ARMING AT ALL, AND IT IS THE FIRST CONDITION BECAUSE IT IS THE ONE
	// THAT CARRIES A USER DECISION. AI moves must not slide, and the way that is achieved is
	// that nothing notes a route for them: `UStratMatchSubsystem::PendingMoveRoutes` is filled
	// only by an accepted PLAYER command and is emptied on every apply. So the map's own
	// contents are the signal, there is no AI detection anywhere in this feature, and a load, a
	// reseed, a spawn and a refused path query all reach this arm and all snap.
	//
	// THIS DELETED THE DEGENERATE TWO-POINT FALLBACK RATHER THAN ADDING TO IT, AND THAT IS A
	// BEHAVIOUR CHANGE WORTH NAMING: before this, "no route" meant a straight chord from source
	// to destination. It now means SNAP. That is intended -- a chord is the artefact this whole
	// change exists to remove, and keeping it as a fallback would have made the commonest AI
	// move slide along the very line the player was told units do not take.
	const bool bActorMoved = !PreviousActorLocation.Equals(GetActorLocation());

	if (Body != nullptr && MoveTweenSeconds > 0.0f && RouteWorldPoints.Num() > 0 && bActorMoved)
	{
		const FVector Destination = GetActorLocation();

		// THIS PATH NEVER PARKS, AND SAYING SO HERE IS WHAT KEEPS `TweenOffsets`' TERMINAL-ZERO
		// PROPERTY TRUE OF IT. The actor has just been put where the model says the unit is, so
		// the picture's home is exact relative zero -- and it is written rather than assumed
		// because a PREVIOUS `PlayRouteSlide` may have left a park behind on this same actor,
		// and `FinishTween` reads this field. Set before the branch below, so the zero-length
		// retirement gets it too.
		TweenRestOffset = FVector::ZeroVector;

		TweenOffsets.Reset(RouteWorldPoints.Num());

		// THE OFFSETS ARE MEASURED AGAINST THE ROUTE'S OWN LAST POINT AND NOT AGAINST
		// `Destination`, WHICH IS A ONE-TOKEN DIFFERENCE AND THE ONLY PLACE `BodyZOffset` COULD
		// HAVE ENTERED THIS ARITHMETIC WRONG. `RouteWorldPoints` comes from
		// `AStratBoardActor::WorldLocationOfHex` and therefore sits on the TILE PLANE, while
		// `Destination` is that same plane plus `BodyZOffset` -- so subtracting `Destination`
		// would put every intermediate waypoint `BodyZOffset` BELOW where the unit is drawn, and
		// the picture would dip into the board and climb back out.
		//
		// [CORRECTED 2026-09-02, SAME DAY, BY THE LANE THAT WROTE IT. WRITTEN FLAT BECAUSE A
		// CORRECTION THAT ONLY ADDS LEAVES BOTH CLAIMS LIVE, AND BECAUSE THE RETRACTED SENTENCE
		// ARGUED THIS FIX WAS OPTIONAL.] It said:
		// RETRACTED> "It is invisible today because the shipped `BodyZOffset` is 0, which is the
		// RETRACTED>  worst kind of latent defect."
		// `0.0f` IS THE C++ DEFAULT AND A BLUEPRINT OVERRIDES IT. `Tools/architect/state/content.md`
		// records `strat-editor-builder` reading the live CDO on 2026-09-02 as
		// `BodyZOffset = 50.0`, twice and in separate passes -- once enumerating the surviving
		// `EditDefaultsOnly` overrides and once as the positive control for a different reading.
		// That is ANOTHER LANE'S MEASUREMENT, cited as such: this lane cannot read it from a
		// checkout, because `BP_StratUnit.uasset` is binary and LFS and `git show` hands back the
		// pointer. SO THE DEFECT WOULD HAVE BEEN VISIBLE AND NOT LATENT -- with the shipped
		// Blueprint every multi-hex move would have dipped the unit fifty units into the board
		// and climbed back out, on screen, every time.
		// AND THE JUSTIFICATION DOES NOT REST ON THAT FIGURE, which is why the number being
		// another lane's is survivable: the expression below is correct for EVERY value of
		// `BodyZOffset`, including zero. The 50 says how loudly this would have failed; the
		// cancellation says why it cannot.
		//
		// Differencing two points from the SAME source cancels the lift exactly, needs no
		// knowledge of it here, and leaves this function with no second copy of the
		// `+ BodyZOffset` expression the declaration pairs across `ApplyUnitView` and
		// `SnapToWorldLocation`.
		//
		// THE ROUTE'S LAST POINT IS SKIPPED, NOT CONVERTED, for the reason the next block gives.
		const FVector RouteEnd = RouteWorldPoints.Last();

		for (int32 Index = 0; Index < RouteWorldPoints.Num() - 1; ++Index)
		{
			TweenOffsets.Add(RouteWorldPoints[Index] - RouteEnd);
		}

		// EXACTLY ZERO, APPENDED RATHER THAN COMPUTED. `RouteEnd - RouteEnd` would also be zero
		// today, and that is the point: the terminal-zero claim in `TweenOffsets`' own block
		// would then rest on an ARITHMETIC IDENTITY between two floating-point expressions
		// instead of on CONSTRUCTION. One of those survives a later edit to how a waypoint is
		// derived and the other does not.
		TweenOffsets.Add(FVector::ZeroVector);

		// AND ELEMENT 0 IS OVERWRITTEN WITH WHERE THE UNIT IS ACTUALLY DRAWN, which is the same
		// `Drawn - Destination` the single-offset tween used and is here for the same reason.
		// At rest route point 0 coincides with `Drawn` and this write changes nothing; the two
		// differ ONLY when an order arrives mid-slide, and then `Drawn` is right and route[0] is
		// a visible pop back to the hex the unit already left.
		//
		// IT HAS A COST AND THE COST IS STATED: in that one case the FIRST SEGMENT is no longer
		// exactly one hex long, so constant speed is perturbed for one segment of one case. The
		// alternative -- keeping route[0] and popping -- trades an invisible speed wobble for a
		// visible teleport, which is the wrong way round.
		TweenOffsets[0] = Drawn - Destination;

		// CUMULATIVE ARC LENGTH, BUILT ONCE HERE BECAUSE NOTHING CAN CHANGE IT BEFORE
		// RETIREMENT. See `TweenArcLengths`: `Tick` turns an eased DISPLACEMENT into a distance
		// and needs to find the segment that distance lands in, which is a search over this.
		TweenArcLengths.Reset(TweenOffsets.Num());
		TweenArcLengths.Add(0.0);
		for (int32 Index = 1; Index < TweenOffsets.Num(); ++Index)
		{
			TweenArcLengths.Add(TweenArcLengths[Index - 1]
				+ static_cast<double>(FVector::Dist(TweenOffsets[Index - 1], TweenOffsets[Index])));
		}

		// A POLYLINE OF NO LENGTH IS NOT A TWEEN, and this is the successor to the old
		// `Offset.IsNearlyZero()` guard rather than a new idea. A one-point route -- the null
		// move -- and a route whose start already coincides with its end both land here. Retired
		// through `FinishTween` and not by falling through, so the arrays end EMPTY and `Body`
		// at the exact zero it last wrote, rather than holding a spent polyline with no clock.
		if (TweenArcLengths.Last() <= 0.0)
		{
			FinishTween();
		}
		else
		{
			TweenElapsed = 0.0f;

			Body->SetRelativeLocation(TweenOffsets[0]);
			SetActorTickEnabled(true);
		}
	}
	else if (bActorMoved && (TweenOffsets.Num() > 0 || !TweenRestOffset.IsZero()))
	{
		// THE ACTOR GENUINELY MOVED AND NOTHING ARMED, SO A PARK LEFT BY §2.11.2's TOUR MUST GO
		// -- WHEREVER THE PICTURE IS HEADING IT MUST NOW END AT THE NEW TRANSFORM. This is a
		// BEHAVIOUR CHANGE and it owes a clause; it is not a tidy-up. `PlayRouteSlide` can leave
		// `Body` resting over an intermediate hex with no clock behind it, and the next model
		// that actually relocates this unit is the point at which that rest position stops
		// describing anything.
		//
		// **IT IS CONDITIONED ON `bActorMoved` AND MUST NEVER BE UNCONDITIONAL.**
		// `AStratPlayerController::Tick` calls `UpdateHoverFromCursor` every frame, a hover
		// crossing into a new hex reaches `ApplyView`, and so this function FIRES DURING A TOUR
		// whenever the player moves the mouse across a hex boundary. An unconditional clear here
		// would pop every parked unit to its final hex on the next mouse move, and it would read
		// as a random glitch rather than as a rule. A hover applies the same positions, so
		// `bActorMoved` is false and the park survives -- the same guard, and the same reasoning,
		// as the re-entrancy case the arming condition already documents.
		//
		// A HARD CUT AND NOT A ZEROED REST OFFSET, WHICH WAS THE OTHER SHAPE AND IS WRONG.
		// Writing `TweenRestOffset = FVector::ZeroVector` and letting a running slide finish
		// would walk the polyline to `TweenOffsets.Last()` -- the park -- and only THEN snap to
		// zero, so the gentler-looking option is the one that pops. `CancelRouteSlide` retires
		// immediately and `Body` is home in the same call.
		//
		// THE SECOND TERM IS WHAT KEEPS THE SHIPPED DEFAULT BIT-IDENTICAL. At
		// `MoveTweenSeconds <= 0` no slide ever arms and no park is ever written, so both terms
		// are false, this branch is not entered, and no automation fixture executes a line of it.
		CancelRouteSlide();
	}

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

void AStratUnitActor::SnapToWorldLocation(const FVector& WorldLocation)
{
	// THE TWEEN IS CLEARED FIRST AND UNCONDITIONALLY. "No animation" has to mean the offset is
	// gone, not merely that this call did not start one -- a hard cut is only a repair if it
	// cannot leave a slide running from a start that is now meaningless.
	//
	// THROUGH `FinishTween` SINCE 2026-09-02, RATHER THAN FOUR HAND-COPIED LINES. There are now
	// two arrays and a scalar to clear instead of one vector and a scalar, and a fifth caller
	// that cleared two of the three would leave a spent polyline behind `GetTweenWaypointCount`.
	// The declaration already named this function as a caller of the retirement path; it is now
	// literally one.
	//
	// AND THE PARK IS CLEARED FIRST, WHICH IS ORDERING AND NOT STYLE. `FinishTween` retires to
	// `TweenRestOffset`, so calling it with a park standing would retire INTO the park -- a
	// "hard cut" that left the picture over some other hex. A hard cut must clear a park by
	// definition: this call site is the repair, and a repair that preserved the thing being
	// repaired would be no repair at all.
	TweenRestOffset = FVector::ZeroVector;
	FinishTween();

	// THE SAME EXPRESSION `ApplyUnitView` WRITES, DUPLICATED ON PURPOSE. Factoring it into a
	// shared helper would have edited that function's location line, and that line staying
	// byte-identical is what makes "this feature does not move the actor" checkable by reading
	// instead of by argument. The declaration names the duplication so the two are maintained
	// as a pair; there are exactly two and this is the second.
	SetActorLocation(WorldLocation + FVector(0.0, 0.0, static_cast<double>(BodyZOffset)));
}

float AStratUnitActor::PlayRouteSlide(const TArray<FVector>& RouteWorldPoints,
                                      const FVector& AnchorWorldPoint)
{
	// FEWER THAN TWO POINTS IS NOT A SLIDE, and `< 2` rather than `> 0` because unlike
	// `ApplyUnitView` there is no terminal waypoint appended here -- a one-point route would
	// arm a polyline of one element, which `Tick` retires on its own guard anyway. Refused
	// early so the answer is 0 seconds and not "0 seconds after some writes".
	//
	// NOTHING IS WRITTEN ON ANY REFUSAL PATH, WHICH IS WHAT MAKES THE SHIPPED DEFAULT
	// BIT-IDENTICAL TO A TREE WITHOUT THIS FUNCTION. `MoveTweenSeconds <= 0` is the C++ default
	// and every automation fixture runs at it, so every call from `UStratMatchSubsystem`'s tour
	// returns here having touched nothing, the tour's interval collapses to
	// `AiPlaybackStepSeconds`, and no clause can observe that this function was called.
	if (Body == nullptr || MoveTweenSeconds <= 0.0f || RouteWorldPoints.Num() < 2)
	{
		return 0.0f;
	}

	// WHETHER A SLIDE IS ALREADY RUNNING, CAPTURED BEFORE THE ARRAY IS REBUILT. It decides one
	// thing only, forty lines down: whether waypoint 0 is the route's own first hex or the
	// position the picture is at this instant.
	const bool bSlideInFlight = TweenOffsets.Num() > 0;

	// THE ANCHOR IS THE CALLER'S AND IS NOT DERIVED, AND THIS IS THE ONE PLACE THE ARITHMETIC
	// COULD GO WRONG. Every offset below is a difference of two points that both came from
	// `AStratBoardActor::WorldLocationOfHex`, so they both sit on the TILE PLANE and
	// `BodyZOffset` cancels exactly -- this function never names that property and holds no
	// second copy of the `+ BodyZOffset` expression `ApplyUnitView` and `SnapToWorldLocation`
	// are paired on. Deriving the anchor as `GetActorLocation() - FVector(0, 0, BodyZOffset)`
	// would reintroduce it; see the declaration, which records the measurement that caught the
	// same defect on `ApplyUnitView`'s path.
	TweenOffsets.Reset(RouteWorldPoints.Num());
	for (const FVector& Point : RouteWorldPoints)
	{
		TweenOffsets.Add(Point - AnchorWorldPoint);
	}

	// AND THE LAST ELEMENT IS THE PARK. It is COMPUTED here rather than appended as a zero,
	// which is the deliberate difference from `ApplyUnitView` and is why `TweenOffsets`' own
	// terminal-zero paragraph was re-scoped rather than weakened: this slide ends over the
	// route's last hex, and only on a unit's FINAL move of the hand-over is that also the hex
	// the actor stands on. `FinishTween` retires to it.
	TweenRestOffset = TweenOffsets.Last();

	// WAYPOINT 0 IS OVERWRITTEN WITH THE DRAWN POSITION ONLY WHEN A SLIDE IS ACTUALLY IN
	// FLIGHT, AND THE CONDITION IS THE CORRECTION. `ApplyUnitView` overwrites unconditionally,
	// for a recorded anti-pop reason: an order arriving mid-slide must restart from where the
	// unit is DRAWN, not from a hex it visibly already left. That reason is real here too and
	// is honoured -- but applying it unconditionally would BREAK THE COMMON CASE, and that is
	// worth stating because the shape was briefed to this lane and is wrong.
	//
	// THE MEASUREMENT, BY READING RATHER THAN BY RUNNING. Before a tour, the board already
	// holds the FINAL state: `RunAiTurnsNow` reconciles to it, so every actor is at its last
	// hex and every picture is at exact relative zero. So on a unit's FIRST slide of a tour
	// `Drawn - GetActorLocation()` is ZERO, while `RouteWorldPoints[0] - AnchorWorldPoint` is
	// where the move began. An unconditional overwrite therefore sets waypoint 0 to zero, and
	// for the commonest reel entry of all -- a one-step move, two waypoints -- the polyline
	// becomes {0, 0}, the arc length is zero and NOTHING ANIMATES. The feature would be absent
	// exactly where it is most expected.
	//
	// AT REST WITH A PARK THE TWO EXPRESSIONS ARE EQUAL ANYWAY, WHICH IS WHY THE CONDITION
	// COSTS NOTHING. A unit's second move begins from a picture parked at its first move's last
	// hex; that park is `previousRoute.Last() - Anchor`, this route's `[0]` is that same hex,
	// and both go through the same conversion of the same hex, so they agree to the bit. The
	// two differ only mid-slide, and mid-slide is the case the overwrite exists for.
	//
	// MID-SLIDE IS NOT REACHABLE FROM THE TOUR AS PACED TODAY -- `ArmNextPlaybackStep` waits out
	// the seconds this function returns before showing the next step -- so this is a guard
	// against a re-arm nobody has written yet, kept because the pacing is a property of another
	// class and this one should not depend on it.
	//
	// [AMENDED 2026-09-02, SAME DAY, BY THE PRE-PARK. WRITTEN FLAT BECAUSE THE MEASUREMENT ABOVE
	// IS A CLAIM ABOUT WHAT THE TOUR LOOKS LIKE AND THE TOUR CHANGED.] The sentence "on a unit's
	// FIRST slide of a tour `Drawn - GetActorLocation()` is ZERO" DESCRIBED THE TOUR AS IT WAS
	// BEFORE `UStratMatchSubsystem::PreParkPicturesForTour` EXISTED, and it is why that function
	// had to be written -- the picture really was at the destination, which is exactly the mass
	// snap a human reported from PIE.
	// **IT IS NO LONGER TRUE OF ANY PRODUCTION PATH.** A unit is pre-parked at its first move's
	// starting hex before step one is shown, so at that slide `Drawn - GetActorLocation()`
	// equals `RouteWorldPoints[0] - AnchorWorldPoint` -- the two expressions agree, and the
	// guard therefore selects between two identical values everywhere in the running game.
	// **THE GUARD STAYS AND IS STILL LOAD-BEARING FOR A DIRECT CALLER**, which is not a
	// hypothetical: `T-INT-05.AFirstRouteSlideArmsANonDegeneratePolyline` calls this function on
	// a picture at rest that nothing pre-parked, and its named mutation -- making the overwrite
	// unconditional -- still collapses that polyline to {0, 0} and still reddens it. What has
	// changed is only the DEMONSTRATION: the collapse is no longer reachable from a tour, and
	// the clause is what keeps it reachable at all.
	if (bSlideInFlight)
	{
		TweenOffsets[0] = Body->GetComponentLocation() - GetActorLocation();
	}

	// CUMULATIVE ARC LENGTH, ON `ApplyUnitView`'S OWN REASONING AND WITH THE SAME LOOP: `Tick`
	// turns an eased displacement into a distance and has to find the segment it lands in.
	TweenArcLengths.Reset(TweenOffsets.Num());
	TweenArcLengths.Add(0.0);
	for (int32 Index = 1; Index < TweenOffsets.Num(); ++Index)
	{
		TweenArcLengths.Add(TweenArcLengths[Index - 1]
			+ static_cast<double>(FVector::Dist(TweenOffsets[Index - 1], TweenOffsets[Index])));
	}

	// A POLYLINE OF NO LENGTH IS NOT A SLIDE, AND RETIRING INTO THE PARK IS THE RIGHT ANSWER
	// RATHER THAN AN ACCIDENT OF ORDERING. `TweenRestOffset` is already written, so `FinishTween`
	// puts the picture over the route's last hex -- which is where a zero-length route says it
	// belongs -- and the caller is told 0 seconds, because nothing is going to move and nothing
	// should wait.
	if (TweenArcLengths.Last() <= 0.0)
	{
		FinishTween();
		return 0.0f;
	}

	TweenElapsed = 0.0f;

	Body->SetRelativeLocation(TweenOffsets[0]);
	SetActorTickEnabled(true);

	// THE ACTOR ANSWERS THE QUESTION IT IS THE ONLY OBJECT ABLE TO ANSWER. See the declaration:
	// `UStratMatchSubsystem` must not read `MoveTweenSeconds`, because `AiPlaybackStepSeconds`'
	// own block states that it never does and that sentence has to stay true.
	return static_cast<float>(TweenDurationSeconds());
}

void AStratUnitActor::CancelRouteSlide()
{
	// THE PARK FIRST AND THE RETIREMENT SECOND, which is `SnapToWorldLocation`'s ordering and
	// the same reason: `FinishTween` retires TO `TweenRestOffset`, so clearing it afterwards
	// would leave `Body` sitting over the parked hex with the field claiming otherwise.
	//
	// NO POSITION IS COMPUTED AND NO TRANSFORM IS READ. `Body` goes to relative zero, which is
	// where it sits at rest on every path that does not park -- so this verb has no opinion
	// about where the unit is and cannot disagree with `UStratMatchSubsystem::ApplyView` about
	// it. That is what makes it safe to call unconditionally over every unit actor.
	TweenRestOffset = FVector::ZeroVector;
	FinishTween();
}

void AStratUnitActor::ParkPictureAt(const FVector& ParkWorldPoint, const FVector& AnchorWorldPoint)
{
	// THE SHIPPED DEFAULT PARKS NOTHING, AND THIS GUARD IS WHY THE FIX CANNOT REACH AN
	// AUTOMATION FIXTURE. At `MoveTweenSeconds <= 0` no step of any tour will ever move a
	// picture back, so a park here would leave a unit drawn hexes from where the model says it
	// is for the whole tour. `<= 0` and not `== 0` on `ApplyUnitView`'s reasoning: a negative
	// duration is a mis-authored Blueprint default and "no animation" is the safe reading.
	if (Body == nullptr || MoveTweenSeconds <= 0.0f)
	{
		return;
	}

	// TWO POINTS FROM ONE SOURCE, SO THE LIFT CANCELS AND THIS FUNCTION NEVER NAMES
	// `BodyZOffset`. Both arrive from `AStratBoardActor::WorldLocationOfHex` through
	// `UStratMatchSubsystem::PreParkPicturesForTour`, which is the same discipline
	// `PlayRouteSlide` states at length and the same one whose violation
	// `T-INT-05.RouteSlideWaypointsSitAtTheDrawnUnitsHeight` was measured able to catch.
	TweenRestOffset = ParkWorldPoint - AnchorWorldPoint;

	// AND THE RETIREMENT PATH DOES THE REST -- ARRAYS EMPTY, `Body` AT THE REST OFFSET, CLOCK
	// ZERO, TICK OFF. That is precisely "a park with nothing running", so this reuses the one
	// retirement path rather than hand-copying its four lines a sixth time. It also means a
	// slide somehow in flight on this actor is retired rather than left racing a park, which is
	// the only safe answer and costs nothing to get right here.
	FinishTween();
}

void AStratUnitActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// A NULL BODY OR A NON-POSITIVE DURATION ENDS THE TWEEN RATHER THAN SUSPENDING IT. The
	// second case is reachable: a Blueprint can write `MoveTweenSeconds` to zero while a slide
	// is in flight, and the only wrong answer is to leave `Body` displaced forever with no
	// clock to bring it back. There is no division by zero on any path out of here.
	if (Body == nullptr || MoveTweenSeconds <= 0.0f)
	{
		FinishTween();
		return;
	}

	// A POLYLINE THAT IS NOT THERE, OR THAT HAS NO LENGTH, RETIRES RATHER THAN DIVIDES. Neither
	// is reachable from `ApplyUnitView`, which retires both cases at arming time; they are
	// checked here because `Tick` must have no path to a division by zero that depends on
	// another function having been careful.
	if (TweenOffsets.Num() < 2 || TweenArcLengths.Num() != TweenOffsets.Num()
		|| TweenArcLengths.Last() <= 0.0)
	{
		FinishTween();
		return;
	}

	TweenElapsed += DeltaSeconds;

	// THE DURATION IS PER HEX STEP AND THE WHOLE SLIDE IS THE PRODUCT -- A USER DECISION, stated
	// at `MoveTweenSeconds` and restated here because this is the line that implements it. There
	// are `Num() - 1` segments over `Num()` waypoints, so a one-step route runs for exactly
	// `MoveTweenSeconds` and a four-step route for four times that. Speed then reads as constant
	// across moves of different lengths, which a fixed total duration does not.
	//
	// [MOVED INTO `TweenDurationSeconds()` 2026-09-02, SAME DAY. The expression is unchanged --
	// same operands, same casts, same order -- and it moved because `PlayRouteSlide` must REPORT
	// this number to a tour that paces itself by it. Two copies of the product is a clock and a
	// picture that disagree silently the day the profile changes.]
	const double Duration = TweenDurationSeconds();

	const double Alpha = FMath::Clamp(static_cast<double>(TweenElapsed) / Duration, 0.0, 1.0);

	if (Alpha >= 1.0)
	{
		FinishTween();
		return;
	}

	// THE TRAPEZOID, AND THE `SmoothStep` BLOCK'S ARGUMENT SURVIVES IT RATHER THAN BEING
	// DELETED WITH IT. What that block actually argued was never "this particular curve" -- it
	// was ZERO VELOCITY AT BOTH ENDS, because a tween here can be replaced mid-flight by the
	// next `ApplyUnitView` and the replacement begins at whatever velocity its own profile has
	// at alpha 0. A linear ramp starts at full speed, so a restart is a visible jerk; a
	// one-sided ease-out starts at full speed too. This profile's velocity is zero at 0 and at
	// 1 and is continuous throughout, so an interrupted slide and the slide that replaces it
	// both begin from rest and the seam is invisible. IDENTICAL PROPERTY, DIFFERENT CURVE.
	//
	// WHY THE CURVE CHANGED AT ALL: `SmoothStep` has no constant middle -- its velocity peaks at
	// the midpoint -- so a unit crossing four hexes visibly races the middle two. The user asked
	// for constant speed between eased ends, which is a trapezoid and is not expressible as a
	// smoothstep. See `MoveTweenEaseFraction`, which declares the one-hex-hop regression this
	// causes and names the two-profile alternative that was refused.
	//
	// CLAMPED WHERE IT IS READ AND NOT WHERE IT IS DECLARED, because the property is authored on
	// a Blueprint and an out-of-range value must degrade rather than divide: above 0.5 the ramps
	// would overlap and the closed form would stop being monotonic.
	const double Ease = FMath::Clamp(static_cast<double>(MoveTweenEaseFraction), 0.0, 0.5);

	// THE PROFILE PRODUCES A DISTANCE AND NOT AN INDEX, WHICH IS THE HALF THAT MAKES SPEED
	// CONSTANT RATHER THAN MERELY EASED. Interpolating by segment index would give every segment
	// the same TIME regardless of its length, and the first segment is not one hex long whenever
	// an order arrived mid-slide -- see `ApplyUnitView`, which states that cost.
	const double Distance = StratTweenTrapezoidDisplacement(Alpha, Ease) * TweenArcLengths.Last();

	// THE BRACKETING SEGMENT, FOUND BY WALKING. The array is a hex route, so it is a handful of
	// entries and a linear scan is cheaper than the branch a binary search would cost. The loop
	// stops at `Num() - 1` so `Segment + 1` is always valid, and `Alpha < 1` above guarantees
	// `Distance < TweenArcLengths.Last()`, so the scan cannot run off the end.
	int32 Segment = 0;
	while (Segment + 2 < TweenArcLengths.Num() && TweenArcLengths[Segment + 1] <= Distance)
	{
		++Segment;
	}

	const double SegmentLength = TweenArcLengths[Segment + 1] - TweenArcLengths[Segment];

	// A ZERO-LENGTH SEGMENT SITS AT ITS OWN START RATHER THAN DIVIDING. Two coincident waypoints
	// are not producible by a hex route, but element 0 is overwritten with a drawn position and
	// could in principle coincide with route point 1.
	const double SegmentAlpha = (SegmentLength > 0.0)
		? (Distance - TweenArcLengths[Segment]) / SegmentLength
		: 0.0;

	Body->SetRelativeLocation(
		FMath::Lerp(TweenOffsets[Segment], TweenOffsets[Segment + 1], SegmentAlpha));
}

void AStratUnitActor::FinishTween()
{
	// EXACTLY ZERO, WRITTEN RATHER THAN APPROACHED, AND BOTH ARRAYS EMPTIED RATHER THAN LEFT
	// SPENT. The terminal value of the offset is the claim the header block's narrowed "no state
	// the model holds" paragraph rests on, and a curve that merely converges would leave a
	// sub-pixel residue that is state by any honest reading; a polyline this actor is no longer
	// walking is state by the same reading. This is also the only place either is retired, so
	// there is one answer to "how does it get back to zero" rather than one per exit -- and it
	// is what makes `GetTweenWaypointCount() == 0` mean "no tween in flight" for the clause
	// rather than merely by intention.
	TweenOffsets.Reset();
	TweenArcLengths.Reset();
	TweenElapsed = 0.0f;

	if (Body != nullptr)
	{
		// `TweenRestOffset` AND NOT A ZERO LITERAL SINCE 2026-09-02, AND THE FIELD IS ZERO ON
		// EVERY PATH BUT THE TWO THAT PARK -- `PlayRouteSlide` and `ParkPictureAt` -- so this is
		// the old value for `ApplyUnitView`'s tween, for `SnapToWorldLocation`, for
		// `CancelRouteSlide` and at the shipped `MoveTweenSeconds <= 0` default. See
		// `TweenRestOffset`, which names its three zeroing writers and why a tour is what
		// guarantees one of them always runs.
		//
		// AND IT IS WRITTEN THROUGH THE RELATIVE SETTER PLUS AN EXPLICIT UPDATE RATHER THAN
		// THROUGH `SetRelativeLocation`, WHICH IS THE DIFFERENCE BETWEEN "EXACTLY ZERO" BEING
		// TRUE AND BEING NEARLY TRUE. **MEASURED 2026-09-02, and it is an engine behaviour and
		// not a rounding opinion:** `USceneComponent::SetRelativeLocation` is a WORLD-SPACE move
		// under the hood -- it converts the requested relative location to a world location,
		// calls `MoveComponent`, and then converts the result BACK through the parent's inverse
		// transform -- so the value it stores is a round trip and not the value it was handed.
		// With this class's own probe in `CancelRouteSlide`, asking for `FVector::ZeroVector`
		// stored an exact zero for unit 4 and `X=0.000 Y=-0.000 Z=0.000` for unit 13 in the same
		// call, on the same line, differing only in the board coordinates of the parent. The
		// residue is ~1e-14 uu on a 100 uu tile and is invisible on a screen -- **and it is
		// visible to `IsZero()`, which is what two clauses assert and what this file's own
		// "written rather than approached" claim promises.**
		// `SetRelativeLocation_Direct` stores the field verbatim and `UpdateComponentToWorld`
		// then recomputes the WORLD transform FROM it -- the correct direction, with no inverse
		// and no round trip -- so the stored value is the value asked for, by construction. This
		// is safe here for a reason specific to this component and stated rather than assumed:
		// `Body` carries NO COLLISION (see the constructor, where that is set three ways and is
		// load-bearing for the cursor), so there is no sweep, no overlap and no physics state
		// that the move path would have been responsible for.
		Body->SetRelativeLocation_Direct(TweenRestOffset);
		Body->UpdateComponentToWorld();
	}

	// AND THE TICK GOES BACK OFF, which is what keeps the class's stated posture -- reconcile
	// when the model changed, never poll -- true of the steady state and not merely of the
	// design intent.
	SetActorTickEnabled(false);
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
