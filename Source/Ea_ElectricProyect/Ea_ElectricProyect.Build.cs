// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Ea_ElectricProyect : ModuleRules
{
	public Ea_ElectricProyect(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Ea_ElectricProyect",
			"Ea_ElectricProyect/Variant_Platforming",
			"Ea_ElectricProyect/Variant_Platforming/Animation",
			"Ea_ElectricProyect/Variant_Combat",
			"Ea_ElectricProyect/Variant_Combat/AI",
			"Ea_ElectricProyect/Variant_Combat/Animation",
			"Ea_ElectricProyect/Variant_Combat/Gameplay",
			"Ea_ElectricProyect/Variant_Combat/Interfaces",
			"Ea_ElectricProyect/Variant_Combat/UI",
			"Ea_ElectricProyect/Variant_SideScrolling",
			"Ea_ElectricProyect/Variant_SideScrolling/AI",
			"Ea_ElectricProyect/Variant_SideScrolling/Gameplay",
			"Ea_ElectricProyect/Variant_SideScrolling/Interfaces",
			"Ea_ElectricProyect/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
