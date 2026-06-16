using UnrealBuildTool;
using System.IO;

public class VietnamOnline : ModuleRules
{
    public VietnamOnline(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core", "CoreUObject", "Engine", "InputCore",
            "EnhancedInput", "UMG", "Slate", "SlateCore",
            "HTTP", "Json", "JsonUtilities", "NavigationSystem",
            "AIModule", "AnimGraphRuntime"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "GameplayTasks"
        });

        // Shared C++ contracts (schemas, constants, Hoi An data)
        string SharedPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../shared/include"));
        PublicIncludePaths.Add(SharedPath);
    }
}
