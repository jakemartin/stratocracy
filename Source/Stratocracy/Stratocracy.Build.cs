// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Stratocracy : ModuleRules
{
	public Stratocracy(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"NavigationSystem",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"Niagara",
			"UMG",
			"Slate",

			// GATE-DATA-VENDOR reads Data/StratData.manifest.json to confirm the
			// CSVs T-DATA-05 compares against are the bytes the manifest records
			// at the crew commit.
			"Json",

			// The vendored headless rules module (GDD §4.9, §4.11 row 9).
			// T-DATA-05 asserts EUnitType mirrors `strat::UnitType` EXACTLY, and
			// a mirror check that cannot see the thing it mirrors would assert
			// nothing -- so the parity harness includes StratRules' Combat.h and
			// compares against the real enum rather than against a second copy
			// of its values. This adds no dependency in the other direction:
			// StratRules stays engine-free, which is what T-INT-04 asserts by
			// compiling it standalone outside UBT.
			"StratRules"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Stratocracy",
			"Stratocracy/Variant_Strategy",
			"Stratocracy/Variant_Strategy/UI",
			"Stratocracy/Variant_TwinStick",
			"Stratocracy/Variant_TwinStick/AI",
			"Stratocracy/Variant_TwinStick/Gameplay",
			"Stratocracy/Variant_TwinStick/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
