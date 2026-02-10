// Copyright (C) 2015-2017 THL A29 Limited, a Tencent company.
// Licensed under the BSD 3-Clause License.
// UE5 plugin port.

using UnrealBuildTool;

public class BehaviacEditor : ModuleRules
{
	public BehaviacEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"BehaviacRuntime",
			"XmlParser",
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"Slate",
			"SlateCore",
			"UnrealEd",
			"AssetTools",
			"ContentBrowser",
			"PropertyEditor",
		});
	}
}
