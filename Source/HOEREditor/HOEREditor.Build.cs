using UnrealBuildTool;

public class HOEREditor : ModuleRules
{
    public HOEREditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "UnrealEd"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UnrealEd",
                "DataValidation",
                "BlueprintGraph",
                "KismetCompiler",
                "HOER",
                "InputBlueprintNodes"
            }
        );
    }
}