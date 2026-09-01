// GDD §4.9 part 2 -- the bridge module. NOT generated and NOT vendored: unlike
// StratRules.Build.cs, this file has no counterpart in the crew repo and T-INT-01
// asserts nothing about it. It sits outside `Source/StratRules/`, which that ID
// requires to contain nothing unaccounted for.
//
// WHY THE BRIDGE IS ITS OWN MODULE, and this is the reason rather than a preference.
// An editor target is a MODULAR build: every UBT module becomes its own DLL, and
// Unreal exports only symbols annotated with an `_API` macro. The vendored `strat::`
// sources carry none, by §4.9's own rule that they hold no engine headers -- so
// `UnrealEditor-StratRules.dll` exports exactly one symbol,
// `ThisIsAnUnrealEngineModule`, measured with `dumpbin /EXPORTS`. Any call from
// another module to `strat::` therefore cannot link, which is what happened the
// first time anything tried: 8 x LNK2019.
//
// That was invisible for four commits because nothing had ever CALLED the module.
// It compiled, it linked, it produced a DLL and an import library, and every one of
// those facts was true and none of them was the one that mattered. The check that
// would have caught it is a caller, not a build.
//
// So the rules sources are compiled INTO this module, beside the bridge that calls
// them, and the DLL boundary moves outward to a surface that does carry an API
// macro -- `FStratBridge`. Nothing is exported that was not written to be exported,
// and no platform-specific export list is maintained.
//
// The two settings below are the ones `StratRules.Build.cs` gives its own module,
// for the same two reasons, and they stay HERE rather than moving to
// `Stratocracy.Build.cs`: that file's own comment scopes the shadow-variable
// relaxation to one module and says it relaxes nothing for `Source/Stratocracy`.
// Folding these sources into the game module would have broken that promise.

using UnrealBuildTool;

public class StratBridge : ModuleRules
{
	public StratBridge(ReadOnlyTargetRules Target) : base(Target)
	{
		// NoPCHs, and it is load-bearing rather than tidiness. A module that shares
		// the engine's PCH is compiled in that PCH's compile environment, and the
		// per-module warning override below does NOT take effect there -- measured:
		// with UseExplicitOrSharedPCHs the C4456 below stayed an error even with the
		// level set to Warning, and only stopped being one here. StratRules.Build.cs
		// carries the same pair of settings for the same reason.
		PCHUsage = PCHUsageMode.NoPCHs;

		// The vendored sources carry same-named helpers in anonymous namespaces
		// (`num`, `parseInt` in more than one module). Unity would merge them into
		// one translation unit and they would collide -- measured, not assumed: a
		// single combined TU fails with C2568/C2264 on exactly those names.
		bUseUnity = false;

		// Driver.good.cpp shadows a local `r`. Both targets set
		// DefaultBuildSettings = V7, which raises this to an error, and the vendored
		// sources are certified bytes so a fix in the tree is not available. This
		// downgrades the diagnostic rather than silencing it, and its scope is this
		// module.
		CppCompileWarningSettings.ShadowVariableWarningLevel = WarningLevel.Warning;

		// Engine + CoreUObject for the DataTable mapping §4.9 part 2's Load bullet
		// names; Core for FString and the file helpers.
		//
		// THE DEPENDENCY ON `Stratocracy` RUNS IN THIS DIRECTION ON PURPOSE. The row
		// structs FUnitRow / FTerrainRow are the engine side of the Load bullet and
		// they live in the game module. Moving them here would have been the other
		// way to break the cycle, and it is refused: their script paths are baked
		// into the DT_Units / DT_Terrain assets as /Script/Stratocracy.UnitRow, so
		// the move would silently break every shipped DataTable. Nothing in the game
		// module calls the bridge yet, so nothing needs the arrow reversed.
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"Stratocracy"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		// THE VENDORED SOURCES ARE COMPILED HERE ONLY WHERE THE DLL BOUNDARY IS.
		//
		// The block at the top of this file explains why `Vendored/*.strat.cpp`
		// exist: in a MODULAR build every module is its own DLL, the vendored
		// sources carry no `_API` macro, and a cross-module `strat::` call cannot
		// link -- 8 x LNK2019. Compiling them into this module deletes the boundary
		// the call would have had to cross. That reason is a statement about the
		// boundary, and it stops being true when the boundary does.
		//
		// A MONOLITHIC target links every module into one binary. There the shims
		// are the defect rather than the fix, because `Stratocracy.Build.cs`
		// depends on the `StratRules` MODULE -- for T-DATA-05's `Combat.h`, per its
		// own comment -- so the twelve `*.good.cpp` are already compiled once as
		// that module's objects, and the twelve shims compile them again. Both sets
		// enter one `Stratocracy.exe`. Measured 2026-08-31, before this switch:
		//
		//     Build.bat Stratocracy Win64 Development
		//       Ai.strat.cpp.obj : error LNK2005: "bool __cdecl
		//       strat::buildPriorityLess(struct strat::UnitDef const &, struct
		//       strat::UnitDef const &)" already defined in Ai.good.cpp.obj
		//       ... 110 x LNK2005 ...
		//       Stratocracy.exe : fatal error LNK1169: one or more multiply
		//       defined symbols found
		//
		// UBT labels that run `Result: Failed (OtherCompilationError)`. It is a
		// pure link failure -- every compile action succeeded -- and the label is
		// not to be read as a compile diagnostic.
		//
		// So the shims switch off exactly where their reason is absent, and this
		// module takes an ordinary dependency on `StratRules` in that build
		// instead. One copy of the code either way; in monolithic it is the copy
		// the vendored module was written to produce, compiled under
		// `StratRules.Build.cs`'s own NoPCHs / bUseUnity=false / shadow-warning
		// settings, which are the same three this file sets.
		//
		// WHAT WAS REJECTED, and why.
		//   - Making `Stratocracy`'s `StratRules` dependency conditional instead
		//     (keeping the shims as the single copy in every target). It works,
		//     but it edits a module this milestone does not own, and it would have
		//     to re-supply `Combat.h` to the parity harness as a bare include path
		//     -- an include edge with no arrow behind it, in a module whose tests
		//     name `strat::` types. Refused for blast radius, not because it fails.
		//   - `IS_MONOLITHIC`, UBT's own global, answering the `#if` directly. That
		//     leaves the dependency decision in this file and the compile decision
		//     in twelve others, agreeing only by inspection. The definition below
		//     comes from the SAME C# expression as the dependency, so the two
		//     halves cannot drift apart; and each shim `#error`s on an ABSENT
		//     definition rather than treating it as 0, so a future refactor that
		//     drops it fails at compile in the modular build instead of at link.
		//
		// The dependency is PRIVATE. No public header of this module names a
		// `strat::` symbol -- `StratBridge.h` is the only header that includes the
		// vendored headers at all, and it is forbidden to any header declaring a
		// UCLASS/USTRUCT/UENUM -- so a public arrow would re-export a link-line
		// entry no dependent could use. `PublicIncludePaths` below already carries
		// the include half, in both link types, and is untouched by this.
		bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
		PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
		if (!bCompileVendoredRulesHere)
		{
			PrivateDependencyModuleNames.Add("StratRules");
		}

		// The vendored headers, by short name. This is a PUBLIC include path because
		// the game module's T-DATA-05 parity pass includes `Combat.h` to compare
		// EUnitType against the real `strat::UnitType`, and it reaches it through a
		// dependency on this module.
		PublicIncludePaths.AddRange(new string[] {
			"StratBridge",
			"StratRules"
		});
	}
}
