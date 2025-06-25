using System;
using System.IO;
using UnrealBuildTool;

public class ThirdParty : ModuleRules
{
    public ThirdParty(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        /**Include Paths*/
        PublicIncludePaths.AddRange(new string[] {
            Path.Combine(ModuleDirectory, "Public"),
        });

        PrivateIncludePaths.AddRange(new string[] {
            Path.Combine(ModuleDirectory, "Private"),
        });

        /**Module Names*/
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            
            
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            
        });

        // Add any import libraries or static libraries (in case I need any)
        //PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib", "foo.a"));
    }
}
