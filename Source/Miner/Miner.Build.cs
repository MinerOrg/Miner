<<<<<<< HEAD
version https://git-lfs.github.com/spec/v1
oid sha256:31f7f75e189806d860e181aceae7284a3a4e78603c3b8729f4bb59919bc936c1
size 1036
=======
// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Miner : ModuleRules
{
	public Miner(ReadOnlyTargetRules Target) : base(Target)
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
			/* The next 3 lines are for the dynamic terrain */
			"GeometryFramework",
			"GeometryCore",
            "DynamicMesh",
        });

		PrivateDependencyModuleNames.AddRange(new string[] {
            "OnlineSubsystem", // For online features
            "GeometryScriptingCore",
    });

		PublicIncludePaths.AddRange(new string[] {
			"Miner",
			"Miner/Variant_Horror",
			"Miner/Variant_Shooter",
			"Miner/Variant_Shooter/AI"
		});
	}
}
>>>>>>> ff6bcf8 (Cleaned up and tried to make thridparty work)
