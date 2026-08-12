// GDD §2.11 / §4.7 Stub 8 -- the camera, implementation side.
//
// NO INPUT IS BOUND HERE AND NONE MAY BE THIS PHASE. See the header block: Enhanced Input
// and the PlayerController are phase 4's, and `StratPlay.Build.cs` does not declare
// `EnhancedInput` yet. `SetupPlayerInputComponent` is deliberately not overridden, so
// nothing in this file can quietly become the project's second input path.
//
// NO `strat::` TYPE AND NO RULES QUESTION. This class knows about distance and pitch.

#include "StratCameraPawn.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

AStratCameraPawn::AStratCameraPawn()
{
	// NO TICK. Nothing here integrates over time -- `PanBy` and `ZoomBy` apply their delta
	// whole and immediately, and the header block records that smoothing belongs to
	// whatever drives them. A tick would exist only to interpolate towards a target this
	// class does not keep.
	PrimaryActorTick.bCanEverTick = false;

	// THE ENGINE'S DEFAULT BINDINGS ARE REFUSED BY DERIVATION, and there is nothing to
	// switch off here. `bAddDefaultMovementBindings` is `ADefaultPawn`'s, not `APawn`'s --
	// MEASURED: error C2065, "'bAddDefaultMovementBindings': undeclared identifier",
	// StratCameraPawn.cpp(27). `APawn` ships no bindings at all, which is the whole reason
	// this class derives from it rather than from `ADefaultPawn`: the flying-spectator
	// movement component and the WASD hookup would be a second input path beside the one
	// phase 4 is about to build, and the one that wins for any key both of them name.
	//
	// So the refusal is structural rather than a flag, and a later change that reparents
	// this pawn to `ADefaultPawn` for its movement component would silently reacquire the
	// bindings with no line here to notice it. That is what this comment is for.

	// It is a camera, not a body. Nothing collides with it and nothing it does affects
	// navigation.
	SetCanBeDamaged(false);
	SetActorEnableCollision(false);

	PivotRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PivotRoot"));
	SetRootComponent(PivotRoot);

	Arm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));
	Arm->SetupAttachment(PivotRoot);

	// NO LAG AND NO COLLISION TEST ON THE ARM. Collision test is what makes a spring arm
	// snap forward when geometry is between it and the pivot -- correct behaviour for a
	// third-person follow camera, and wrong for a board camera, where the "geometry"
	// would be the board. Lag is smoothing, which the header block assigns to the caller.
	Arm->bDoCollisionTest = false;
	Arm->bEnableCameraLag = false;
	Arm->bEnableCameraRotationLag = false;

	// THE ARM CARRIES ITS OWN ROTATION AND DOES NOT INHERIT THE PAWN'S OR THE
	// CONTROLLER'S. A board camera whose pitch followed the controller would tilt whenever
	// a later pass gave the controller a rotation for any other reason.
	Arm->bUsePawnControlRotation = false;
	Arm->bInheritPitch = false;
	Arm->bInheritYaw = false;
	Arm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(Arm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
}

void AStratCameraPawn::BeginPlay()
{
	Super::BeginPlay();

	// Applied here rather than in the constructor because these are Blueprint defaults, and
	// a constructor running on the CDO would read the C++ values and not the configured
	// ones.
	//
	// CLAMPED AGAINST THE SAME RANGE `ZoomBy` USES, so a Blueprint that sets a default
	// outside its own configured limits starts inside them rather than at a distance no
	// zoom can return to.
	Arm->TargetArmLength = FMath::Clamp(DefaultArmLength, MinArmLength, MaxArmLength);
	Arm->SetRelativeRotation(FRotator(ArmPitch, 0.0f, 0.0f));
}

void AStratCameraPawn::PanBy(FVector2D Delta)
{
	// IN THE PAWN'S OWN XY. With no yaw this is the same as the camera's; when a later pass
	// adds yaw, a pan that had followed the camera would change meaning for every existing
	// caller without a single line of theirs changing.
	AddActorWorldOffset(FVector(Delta.X, Delta.Y, 0.0));
}

void AStratCameraPawn::ZoomBy(float Delta)
{
	// CLAMPED HERE because the range is this class's property. A zoom past either end is an
	// ordinary input, not an error, so it is absorbed rather than reported.
	Arm->TargetArmLength = FMath::Clamp(Arm->TargetArmLength + Delta, MinArmLength, MaxArmLength);
}

void AStratCameraPawn::FocusWorldLocation(const FVector& WorldLocation)
{
	// ONLY THE XY. The pivot stays on this actor's own Z plane, so focusing a hex never
	// changes the camera's height -- height is `ArmLength` and angle is `ArmPitch`, and
	// those are the two things a focus must not silently move.
	const FVector Current = GetActorLocation();
	SetActorLocation(FVector(WorldLocation.X, WorldLocation.Y, Current.Z));
}

float AStratCameraPawn::GetArmLength() const
{
	return Arm != nullptr ? Arm->TargetArmLength : 0.0f;
}
