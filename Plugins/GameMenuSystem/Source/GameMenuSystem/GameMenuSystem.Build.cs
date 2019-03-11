using UnrealBuildTool;
using System.IO;

public class GameMenuSystem : ModuleRules
{
    public GameMenuSystem(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivatePCHHeaderFile = "Private/GameMenuSystemPrivatePCH.h";

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "UMG",
            "Slate",
            "SlateCore",
            "OnlineSubsystem",
            "OnlineSubsystemSteam",
            "RHI"
        });
    }
}