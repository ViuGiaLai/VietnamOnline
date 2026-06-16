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

        // Module root so #include "Core/Foo.h" finds Core/VNOFoo.h etc.
        PublicIncludePaths.Add(ModuleDirectory);

        // Shared C++ contracts (schemas, constants, Hoi An data)
        // Shared C++ contracts: project root /shared/include
        string SharedPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../../shared/include"));
        PublicIncludePaths.Add(SharedPath);
    }
}
