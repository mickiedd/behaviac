// BehaviorU UE5 Plugin
// Licensed under the BSD 3-Clause License.

#include "BehaviorUEditorModule.h"
#include "BehaviorUBehaviorTreeFactory.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"

#define LOCTEXT_NAMESPACE "FBehaviorUEditorModule"

void FBehaviorUEditorModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("BehaviorUEditor module started."));
}

void FBehaviorUEditorModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("BehaviorUEditor module shut down."));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBehaviorUEditorModule, BehaviorUEditor)
