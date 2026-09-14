using UnrealBuildTool;
using System.Collections.Generic;
public class KidzCityTarget : TargetRules
{
    public KidzCityTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
        ExtraModuleNames.Add("KidzCity");
    }
}
