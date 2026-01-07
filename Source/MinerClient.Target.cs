// Copyright Schuyler Zheng. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Desktop)]
public class MinerClientTarget : TargetRules
{
    public MinerClientTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Client;
        DefaultBuildSettings = BuildSettingsVersion.V6;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;

        ExtraModuleNames.AddRange(new string[] { "Miner", "ThirdParty" });

        bUseChecksInShipping = true;
        //bUseIris = true;
    }
}