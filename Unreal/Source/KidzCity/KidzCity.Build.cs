using UnrealBuildTool;
public class KidzCity : ModuleRules
{
    public KidzCity(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp20;
        PublicDependencyModuleNames.AddRange(new[] {
            "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
            "AIModule", "NavigationSystem", "GameplayTasks", "ChaosVehicles",
            "PhysicsCore"
        });
    }
}
