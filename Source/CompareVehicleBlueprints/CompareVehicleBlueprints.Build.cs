// Copyright John Farrow (c) 2023. All Rights Reserved.


using UnrealBuildTool;

public class CompareVehicleBlueprints : ModuleRules
{
	public CompareVehicleBlueprints(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"EditorFramework",
				"UnrealEd",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "AnimGraph",
                "UnrealEd",
                "PhysicsUtilities",
                "InputBlueprintNodes",
                "BlueprintGraph",
                "EditorScriptingUtilities",
                "CommonInput",
                "EnhancedInput",
                "PythonScriptPlugin",
                "SubobjectDataInterface",
                "ChaosVehicles",
				"PropertyEditor"

				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
