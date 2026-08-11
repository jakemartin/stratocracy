// GDD §4.7 Stub 8 / §2.11 -- the widget module. It holds the UMG surface that binds
// to the view model and NOTHING ELSE: no rules, no state, no arithmetic over what the
// snapshot already carries (T-UI-03's no-widget-side-arithmetic clause).
//
// WHY THIS IS ITS OWN MODULE RATHER THAN LIVING IN `StratBridge`, which is the only
// other module that could host it today. Two reasons, and the second is the one that
// cannot be undone later.
//
// FIRST: `StratBridge.h` states that it declares no reflected types on purpose, so the
// vendored `strat` headers it includes are never put in front of UHT. Adding a UCLASS
// to that module would put a reflected header beside those includes and start eroding
// the property that file was written to hold. Widgets live outside it instead.
//
// SECOND, AND IRREVERSIBLE: a Blueprint that derives from a UMG class bakes that
// class's script path into the asset -- `/Script/StratUI.StratScoreboardWidget`. This
// is the same trap `StratBridge.Build.cs` records for FUnitRow / FTerrainRow, whose
// `/Script/Stratocracy.UnitRow` paths are baked into DT_Units and DT_Terrain and which
// is why the bridge's dependency arrow points at the game module rather than the other
// way. Deciding the widget's module AFTER a WBP_ asset derives from it costs asset
// surgery; deciding it now costs this file.
//
// THE DEPENDENCY RUNS StratUI -> StratBridge AND NEVER BACK. The bridge owns the
// authoritative `strat::GameState`; this module owns pixels. A reverse dependency
// would let the bridge reach a widget, which is the arrangement §4.1's "never own
// rules" exists to prevent -- and it would also reintroduce the UHT problem above.
//
// NOTE ON WHAT IS NOT HERE: the game module `Stratocracy` is deliberately not a
// dependency. Nothing in this module needs FUnitRow or FTerrainRow -- it binds to the
// projected view model, not to the DataTable rows the bridge maps in -- and reaching
// for the game module would couple the widget layer to the row structs for no gain.

using UnrealBuildTool;

public class StratUI : ModuleRules
{
	public StratUI(ReadOnlyTargetRules Target) : base(Target)
	{
		// The same pair `StratBridge.Build.cs` and `StratRules.Build.cs` carry, for the
		// same underlying reason rather than by imitation: this module's .cpp files
		// include `StratBridge.h`, which includes the vendored `strat` headers, so the
		// compile environment those headers land in is this module's. Sharing the
		// engine PCH would put them in an environment whose warning settings this file
		// does not control. Kept conservative here; if a later pass measures that a
		// shared PCH is safe for this module, the comment should record the
		// measurement rather than the setting being dropped quietly.
		PCHUsage = PCHUsageMode.NoPCHs;
		bUseUnity = false;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			// The view-model source. `StratBridge` re-exports the vendored headers by
			// short name through its own PublicIncludePaths, so this module reaches
			// `Ui.h` and `Replay.h` through that dependency and vendors nothing itself.
			"StratBridge"
		});

		// UMG pulls Slate and SlateCore behind it, but a module that names UUserWidget
		// in its own headers declares all three rather than relying on that.
		PrivateDependencyModuleNames.AddRange(new string[] {
			"UMG",
			"Slate",
			"SlateCore"
		});

		PublicIncludePaths.AddRange(new string[] {
			"StratUI"
		});
	}
}
