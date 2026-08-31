// GDD §4.1 / §4.9 -- the GameMode. It configures the match and starts it, and it owns
// nothing.
//
// WHAT GAP THIS CLOSES. `UStratMatchSubsystem` is created with the world and has no way to
// learn which DataTables, which scenario file or which actor classes this map is supposed
// to use -- a subsystem has no Blueprint defaults to read them from. This class is the
// object that has them: one `EditDefaultsOnly` `FStratMatchConfig`, set on a Blueprint
// default in phase 5, handed to the subsystem on `BeginPlay`.
//
// IT HOLDS THE CONFIGURATION AND NOT THE STATE, and that split is the whole of §4.1's
// "never own rules" applied to this class. A GameMode is an ACTOR, so putting the
// authoritative `strat::GameState` here would reinstate the exact debt
// `StratScoreboardHUD.h` was opened to record and `UStratMatchSubsystem` was written to
// discharge. This class passes a struct and keeps a copy of the struct; the bridge lives
// one object away, in something that is not an actor.
//
// WHY `AGameModeBase` AND NOT `AGameMode`. `AGameMode` adds the match-state machine
// (`WaitingToStart` / `InProgress` / `WaitingPostMatch`) and the `MatchState` replication
// around it. §2.8's match state is the rules module's -- turn, cap, result tier, all in
// `strat::UiMatchView` -- and a second, engine-side match state beside it is two answers to
// "is the match over" that would agree until they did not. `AGameModeBase` has no such
// machine.
//
// `DefaultPawnClass` IS SET IN C++ AND THAT IS NOT AN ASSET PATH. It is a `UClass*` from
// `AStratCameraPawn::StaticClass()`, resolved by the linker rather than by a package
// lookup, so no `/Game/` literal appears and a renamed asset cannot break a compile. The
// reason it is set at all: `AGameModeBase` otherwise spawns `ADefaultPawn`, which arrives
// with a flying-spectator movement component and the engine's default bindings already
// attached -- a second input path beside the one phase 4 is about to build, and the one
// that wins for any key both of them name.
//
// `HUDClass` IS DELIBERATELY NOT SET HERE, even though this module can see
// `AStratScoreboardHUD`. The scoreboard needs its own DataTables and its WBP class on a
// Blueprint subclass; naming the bare C++ class here would put a HUD on screen that refuses
// every refresh in its own words and looks like a bug in the bridge. Phase 5's GameMode
// Blueprint sets `HUDClass` to the configured BP_ subclass, and until it does the match
// runs without a scoreboard -- which `UStratMatchSubsystem::HandBridgeToScoreboard` treats
// as a configuration rather than a fault.
//
// `PlayerControllerClass` IS SET, AND IT IS THE ONE PLACE THIS CLASS DIFFERS FROM `HUDClass`
// ABOVE. Phase 4 landed `AStratPlayerController` and set it here in the same pass, as this
// block previously said it would. The asymmetry with the HUD is deliberate and is about how
// the two behave unconfigured: a bare `AStratPlayerController` with null input assets is
// inert, logs one line saying so, and leaves a correct match on screen; a bare
// `AStratScoreboardHUD` refuses every refresh in its own words and looks like a bridge bug.
// Left unset, the engine's own `APlayerController` would bind nothing at all, and a seeded
// match that no click can touch reads as an input-asset fault rather than as a missing
// controller class -- which is the wrong file to send the next reader to.
//
// THE SHELL IS CONFIGURED FROM HERE TOO, AS OF W6, AND IT IS THE SAME RELATIONSHIP TWICE.
// `TitleLevel` below is to `UStratShellSubsystem` exactly what `MatchConfig` is to
// `UStratMatchSubsystem`: a value a subsystem cannot hold because it has no details panel,
// living on the one object in the world that does, handed over on `BeginPlay`. This class
// still owns nothing and still decides nothing; it now passes two structs instead of one.
//
// NOT IN THIS ROUND, with reasons:
// - Turn hand-over and end-of-match. Both read or move rules state, which is the
//   subsystem's and the bridge's. A GameMode that ended the match would be deciding a
//   §2.8 outcome the rules module already decides.
//   **RESTART WAS NAMED ON THIS LINE UNTIL W6 AND IS STAMPED OFF IT, because it is now
//   reachable and the sentence would otherwise read as a live refusal of a shipped route.**
//   The reasoning survives intact and is why the route lands where it does: restarting is
//   `UStratShellSubsystem`'s `NewMatch` route, which opens this map again and lets
//   `BeginPlay` seed it from `MatchConfig` the way a fresh boot does. No code in this class
//   ends, resets or re-seeds a match, and none may.
// - Any `/Game/` path. The whole configuration is properties, `TitleLevel` included.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "StratMatchSubsystem.h"

#include "StratGameMode.generated.h"

class UWorld;

/**
 * Starts a Stratocracy match in this world.
 *
 * A map gets one by pointing its World Settings at a Blueprint subclass of this class --
 * asset data, never an `#include`. That indirection is what lets the game module reach
 * this module without a C++ dependency, which it cannot have: `StratBridge -> Stratocracy`
 * already exists and the reverse arrow would be a cycle.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Strat Game Mode"))
class STRATPLAY_API AStratGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AStratGameMode();

	/**
	 * The whole of what this map's match is made of.
	 *
	 * ONE PROPERTY AND NOT SEVEN, because it is passed as one argument to
	 * `UStratMatchSubsystem::StartMatch` and a details panel shows a struct as a group.
	 * Every field is documented on `FStratMatchConfig`; none of them is a rules value and
	 * none of them is an asset path.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stratocracy|Match")
	FStratMatchConfig MatchConfig;

	/**
	 * §2.11.5's title/menu map -- the level a `ReturnToTitle` shell route opens.
	 *
	 * IT IS HERE AND NOT ON `AStratShellGameMode` BECAUSE THIS IS THE MAP THAT CAN LEAVE.
	 * `StratShellSubsystem.h` states the split: each GameMode holds exactly the one
	 * destination it is able to route to, so the two level references are never duplicated
	 * across two Blueprint defaults with nothing asserting they agree.
	 *
	 * A SOFT REFERENCE, NEVER A `/Game/` LITERAL, and it is never loaded by the shell --
	 * `UStratShellSubsystem::ResolveDestination` asks only whether it is null. Left unset,
	 * `Return to Title` comes back from the model greyed with "No title level is
	 * configured." and this map is exactly as playable as it was before the property
	 * existed.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stratocracy|Shell")
	TSoftObjectPtr<UWorld> TitleLevel;

	/**
	 * Why there is no match, when there is none. Empty on success.
	 *
	 * KEPT AS STATE RATHER THAN ONLY LOGGED, for the reason `AStratScoreboardHUD` keeps its
	 * own: a log line is gone by the time anyone asks the question in-editor, and a debug
	 * overlay that wants to say "no match: <reason>" must not have to re-run the failing
	 * load to find out why.
	 */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Match")
	FString LastFailureReason;

protected:
	/**
	 * Starts the match.
	 *
	 * ON BeginPlay AND NOT IN `InitGame`, and the difference matters for the handover. The
	 * PlayerController -- and therefore its HUD -- is spawned during login, before the
	 * world's BeginPlay, so by the time this runs `UStratMatchSubsystem` can FIND a
	 * scoreboard to hand its bridge to. Started from `InitGame` there would be no player
	 * yet, the handover would silently no-op, and the HUD would go on drawing from a bridge
	 * it seeded for itself -- a second `strat::GameState` for the same map.
	 *
	 * THE HUD's OWN BeginPlay MAY HAVE RUN FIRST, and that is handled rather than avoided:
	 * `AdoptBridge` is documented to work called before OR after BeginPlay, destroying any
	 * bridge the HUD had allocated for itself. So this ordering is robust in both
	 * directions and depends on the contract rather than on the engine's actor iteration
	 * order.
	 */
	virtual void BeginPlay() override;
};
