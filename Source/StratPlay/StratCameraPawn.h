// GDD §2.11 / §4.7 Stub 8 -- the camera. A top-down look at the board, and the pawn the
// player possesses so that a cursor has somewhere to be cast from.
//
// WHAT GAP THIS CLOSES. Without a pawn of our own, `AGameModeBase` spawns `ADefaultPawn`,
// which arrives with a flying-spectator movement component and the engine's default
// bindings already attached. That is a second, invisible input path beside the one phase 4
// is about to build, and it would be the one that wins for any key both of them name. This
// class is a pawn that moves ONLY when something asks it to.
//
// WHY A PAWN AND NOT A CAMERA ACTOR PLUS `SetViewTarget`. The PlayerController needs a
// possessed pawn for `DeprojectMousePositionToWorld` to have a sensible player-camera
// manager behind it, and phase 4's cursor picking is a deprojection. A view target with no
// pawn works and then costs an exception in every input path that assumes a pawn.
//
// A SPRING ARM AND NOT A BARE CAMERA, so that "where the camera looks" and "how far away
// it is" are two properties instead of one transform a designer has to solve backwards.
// `PivotRoot` is the point on the board the camera orbits; the arm carries the pitch and
// the distance. Panning moves the pivot; zooming moves the arm length. Neither ever
// touches the camera component, which is why there is no state on it to get out of step.
//
// IT HAS NO INPUT BINDINGS AND THAT IS PHASE 3's SCOPE FENCE, not an oversight. Enhanced
// Input, the mapping context and the PlayerController are phase 4's, and
// `StratPlay.Build.cs` deliberately does not declare `EnhancedInput` yet -- a module that
// declares an input dependency before it has an input path invites the first person who
// needs a key binding to put it in the nearest actor. The three verbs below (`PanBy`,
// `ZoomBy`, `FocusWorldLocation`) are what a binding will call; they are ordinary methods
// and they work from Blueprint, from a console command, or from a test with no input at
// all.
//
// IT KNOWS NOTHING ABOUT HEXES. `FocusWorldLocation` takes a world location, and the
// caller gets that location from `AStratBoardActor::WorldLocationOfHex` -- the only axial
// -> world conversion in the project. A `FocusHex` overload here would either duplicate
// that formula or need a pointer to the board, and a camera holding a pointer to the board
// is a camera that eventually gets asked what is on a hex.
//
// NOT IN THIS ROUND, with reasons:
// - Rotation / yaw. §2.11's board is read top-down and a rotatable board makes "north" a
//   per-player fact that every subsequent UI element has to respect. Adding it later costs
//   one property; removing it later costs every screenshot.
// - Edge-of-screen scrolling, momentum, smoothing. All input feel, and input is phase 4's.
//   `PanBy` applies its delta immediately so that whatever drives it owns the curve.
// - Bounds. Clamping the pivot to the board needs the board's extent, which needs the
//   board, which is the pointer the paragraph above refuses. Phase 4's controller has both
//   and is where the clamp belongs.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "StratCameraPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;

/**
 * The player's viewpoint on the board.
 *
 * Set as `AStratGameMode`'s `DefaultPawnClass` in C++ -- a class reference, not an asset
 * path -- so a map with that GameMode gets this camera without a Blueprint having to
 * remember to say so.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Strat Camera Pawn"))
class STRATPLAY_API AStratCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	AStratCameraPawn();

	/**
	 * Slides the pivot across the board plane.
	 *
	 * IN THE PAWN'S OWN XY AND NOT THE CAMERA'S, deliberately: with no yaw the two agree,
	 * and when a later pass adds yaw a pan that followed the camera would change meaning
	 * silently for every existing caller. The delta is applied whole and immediately -- no
	 * smoothing, no clamping -- so whatever drives it owns the feel and the bounds.
	 *
	 * @param Delta  X and Y in Unreal units, in the board plane. Z is not pannable; the
	 *               camera's height is `ArmLength` and its angle is `ArmPitch`.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Camera")
	void PanBy(FVector2D Delta);

	/**
	 * Changes the camera's distance from the pivot, clamped to the configured range.
	 *
	 * CLAMPED HERE AND NOT AT THE CALL SITE, because the range is this class's property and
	 * a caller that clamped would be holding a copy of it. A zoom past either end is an
	 * ordinary input, not an error, so it is absorbed silently.
	 *
	 * @param Delta  positive moves the camera away from the board.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Camera")
	void ZoomBy(float Delta);

	/**
	 * Centres the view on a world location, keeping the current distance and pitch.
	 *
	 * TAKES A WORLD LOCATION AND NOT A HEX. See the header block: a `FocusHex` overload
	 * would either duplicate the board's axial -> world formula or require a pointer to the
	 * board. Only the XY are used -- the pivot stays on this actor's own Z plane, so
	 * focusing never changes the camera's height.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Camera")
	void FocusWorldLocation(const FVector& WorldLocation);

	/** The current arm length, i.e. how far the camera is from the pivot. A reader; the
	 *  clamp lives in `ZoomBy`. */
	UFUNCTION(BlueprintPure, Category = "Stratocracy|Camera")
	float GetArmLength() const;

protected:
	virtual void BeginPlay() override;

	/** The point on the board the camera orbits. Panning moves this and nothing else. */
	UPROPERTY(VisibleAnywhere, Category = "Stratocracy|Camera")
	TObjectPtr<USceneComponent> PivotRoot;

	/** Carries the pitch and the distance, so the two are separate properties rather than
	 *  one transform a designer has to solve backwards. */
	UPROPERTY(VisibleAnywhere, Category = "Stratocracy|Camera")
	TObjectPtr<USpringArmComponent> Arm;

	UPROPERTY(VisibleAnywhere, Category = "Stratocracy|Camera")
	TObjectPtr<UCameraComponent> Camera;

	// ---- Configuration. All EditDefaultsOnly, all set on a Blueprint default. --------
	// None of these is a gameplay value: no rule reads one, no snapshot field corresponds
	// to one, and changing any of them changes what the player sees and nothing about what
	// is true.

	/** Starting distance from the pivot. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Camera")
	float DefaultArmLength = 1200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Camera")
	float MinArmLength = 400.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Camera")
	float MaxArmLength = 3000.0f;

	/** Downward pitch, in degrees. Negative looks down; -60 is a readable three-quarter
	 *  view of a hex board and -90 is straight down. A property rather than a constant
	 *  because which one reads better depends on tile meshes that do not exist yet. */
	UPROPERTY(EditDefaultsOnly, Category = "Stratocracy|Camera")
	float ArmPitch = -60.0f;
};
