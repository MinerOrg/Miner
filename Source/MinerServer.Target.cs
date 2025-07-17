// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.Collections.Generic;
using UnrealBuildTool;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class MinerServerTarget : TargetRules
{
    public MinerServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;

        ExtraModuleNames.AddRange(new string[] { "Miner" });

        //MinerGameTarget.ApplySharedMinerTargetSettings(this); // IDK what this does

        bUseChecksInShipping = false;
    }
}