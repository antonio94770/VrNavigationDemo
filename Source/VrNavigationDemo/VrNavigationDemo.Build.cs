// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VrNavigationDemo : ModuleRules
{
	public VrNavigationDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "Navmesh", "HeadMountedDisplay", "ProceduralMeshComponent"});

		PrivateDependencyModuleNames.AddRange(new string[] { "Navmesh" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
