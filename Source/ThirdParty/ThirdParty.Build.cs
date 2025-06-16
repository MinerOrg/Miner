using System;
using System.IO;
using UnrealBuildTool;

public class ThirdParty : ModuleRules
{
    public ThirdParty(ReadOnlyTargetRules Target) : base(Target)
    {
        // Very bad way to include fast noise lite, but I think I will only import that so it should be fine
        Type = ModuleType.External;

        // Add any macros that need to be set
        PublicDefinitions.Add("FastNoiseLite.h=1");

        // Add any include paths for the plugin
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "FastNoiseLite"));
    }
}