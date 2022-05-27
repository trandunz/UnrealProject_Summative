// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GDP03_Summative2 : ModuleRules
{
	public GDP03_Summative2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{
			"Core", 
			"CoreUObject",
			"Engine",
			"InputCore", 
			"HeadMountedDisplay" ,
			"OnlineSubsystem",
			"OnlineSubsystemSteam"
		});
	}
}
