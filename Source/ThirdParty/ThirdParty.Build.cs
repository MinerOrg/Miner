<<<<<<< HEAD
version https://git-lfs.github.com/spec/v1
oid sha256:f6b38ae83cd4a8b4a846eecf078d0f6003aac9cebaad31b81091016da7a6c294
size 546
=======
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
        PublicDefinitions.Add("FastNoiseLite=1");

        // Add any include paths for the plugin
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "FastNoiseLite"));
    }
}
>>>>>>> 9a2e41e (Fixed noiselibrary stuff)
