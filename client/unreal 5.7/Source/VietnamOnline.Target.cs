using UnrealBuildTool;
using System.Collections.Generic;

public class VietnamOnlineTarget : TargetRules
{
    public VietnamOnlineTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V6;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("VietnamOnline");
    }
}
