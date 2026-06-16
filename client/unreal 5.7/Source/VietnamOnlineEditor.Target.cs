using UnrealBuildTool;
using System.Collections.Generic;

public class VietnamOnlineEditorTarget : TargetRules
{
    public VietnamOnlineEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V6;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("VietnamOnline");
    }
}
