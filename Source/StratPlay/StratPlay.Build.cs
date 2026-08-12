// GDD §4.7 Stub 8 / §4.9 / §2.11 -- the gameplay module. It holds the actors that put a
// match on screen: the board, the units, the camera, the match owner and the GameMode
// that configures them. It holds NO rules, NO arithmetic over a rules answer, and NOT
// ONE `strat::` type.
//
// WHY THIS IS ITS OWN MODULE RATHER THAN LIVING IN `Stratocracy`, which is the module
// Unreal's own template would have put a GameMode and a Pawn in. The arrow already runs
// `StratBridge -> Stratocracy`, recorded in StratBridge.Build.cs and load-bearing there:
// FUnitRow / FTerrainRow bake `/Script/Stratocracy.UnitRow` into DT_Units and DT_Terrain,
// so the row structs cannot move and the arrow cannot be reversed. A gameplay actor in
// the game module that needed `FStratBridge` would therefore need `Stratocracy ->
// StratBridge` as well, which is a cycle UBT refuses outright. This module sits on the
// far side of the existing chain instead, and `Source/Stratocracy/` is not touched.
//
// WHY IT DEPENDS ON `StratUI`. `StratViewModel.h` -- the complete, reflected description
// of what should be on screen -- lives there, and its own header block records why: it is
// "the lowest module both the widgets (here, today) and phase 3's `StratPlay` actors can
// see". StratUI also holds `AStratScoreboardHUD`, whose `AdoptBridge` is the seam the
// match subsystem hands its bridge across.
//
// AND WHY IT ALSO NAMES `StratBridge` DIRECTLY, WHICH THIS FILE FIRST DID NOT. The first
// version of this list omitted it on the reasoning that StratUI declares StratBridge as a
// PUBLIC dependency, so the include paths and the link would arrive transitively. The
// include paths do. THE LINK DOES NOT, MEASURED: 4 x LNK2019 on
// `UnrealEditor-StratPlay.dll`, naming `__imp_?LoadDefinitions@FStratBridge@@`,
// `__imp_?LoadScenarioFromFile@FStratBridge@@`, and the imported constructor and
// destructor -- the last of those referenced from `UE::Core::Private::PimplPtr::
// DeleterFunc<FStratBridge>`, i.e. from the `TPimplPtr` member itself. The declarations
// came through as `__declspec(dllimport)`, so the header was found and `STRATBRIDGE_API`
// resolved correctly; what was missing was `UnrealEditor-StratBridge.lib` on this module's
// link line. A module that CALLS a symbol declares the module that exports it, and the
// transitive arrow is not a substitute for that. Note which methods did NOT appear in the
// error: `IsSeeded()` and `GetBridge()` are inline in the header and linked fine, which is
// exactly how this omission would have survived a smaller caller.
//
// THE ARROW RUNS `StratPlay -> StratUI` AND NEVER BACK. A widget that reached an actor
// would make the screen a function of the world rather than of the view model, which is
// the exact property T-INT-05 exists to hold. `StratViewModel.h` states the posture:
// `ApplyView` spawns, moves and destroys actors to match the model on every refresh, so
// every arrow of influence runs model -> actor and nothing runs the other way.
//
// NOTE ON WHAT IS NOT HERE, and each absence is a decision rather than an oversight:
//
// - `EnhancedInput`. The PlayerController, the mapping contexts and the selection state
//   machine are phase 4's, and a module that declares an input dependency before it has
//   an input path invites the first person who needs a key binding to put it in the
//   nearest actor. Added in the phase that adds the controller, not before.
//
// - `Stratocracy`. Nothing here names FUnitRow or FTerrainRow. The two DataTables arrive
//   as `UDataTable*` properties and are handed straight to `FStratBridge::LoadDefinitions`
//   without this module ever reading a row -- the same refusal `StratUI.Build.cs` records
//   under its own "NOTE ON WHAT IS NOT HERE", and for the same reason.
//
// - `UMG` / `Slate`. This module creates no widget. The scoreboard is StratUI's and is
//   reached as an actor class (`AStratScoreboardHUD`), which is engine reflection and not
//   Slate.
//
// - Anything under `Content/AdvancedTurnBasedTileToolkit/`. That plugin content is a
//   harvest source for MESHES AND MATERIALS ONLY. Its grid and its pathfinding would be a
//   second source of truth for movement beside `FStratBridge::Reachable`, which is the
//   substitution T-UI-02 was written to catch. Nothing in this module references it in
//   C++ at all; assets arrive through EditDefaultsOnly properties set on Blueprints.

using UnrealBuildTool;

public class StratPlay : ModuleRules
{
	public StratPlay(ReadOnlyTargetRules Target) : base(Target)
	{
		// The same pair `StratRules`, `StratBridge` and `StratUI` all carry, and for the
		// same underlying reason rather than by imitation: `StratMatchSubsystem.cpp`
		// includes `StratBridge.h`, which includes the vendored `strat` headers, so the
		// compile environment those headers land in is this module's. Sharing the engine
		// PCH would put them in an environment whose warning settings this file does not
		// control. If a later pass MEASURES that a shared PCH is safe here, the comment
		// should record the measurement rather than the setting being dropped quietly.
		PCHUsage = PCHUsageMode.NoPCHs;
		bUseUnity = false;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			// Engine carries AActor, APawn, AGameModeBase, UWorldSubsystem, the
			// instanced-static-mesh components, the spring arm and the camera. All of
			// this module's presentation is engine primitives; none of it is a plugin.
			"Engine",
			// The view model and the scoreboard HUD.
			"StratUI",
			// `FStratBridge` itself: `StratMatchSubsystem.cpp` constructs one, calls
			// LoadDefinitions and LoadScenarioFromFile on it, and holds it in a TPimplPtr
			// whose deleter references the imported destructor. See the header block for
			// the 4 x LNK2019 this line was added to fix -- the transitive arrow through
			// StratUI carried the includes and not the import library.
			"StratBridge"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"StratPlay"
		});
	}
}
