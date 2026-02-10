// Copyright (C) 2015-2017 THL A29 Limited, a Tencent company.
// Licensed under the BSD 3-Clause License. UE5 plugin port.

#include "BehaviacEditorModule.h"
#include "BehaviacBehaviorTreeFactory.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"

#define LOCTEXT_NAMESPACE "FBehaviacEditorModule"

void FBehaviacEditorModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("BehaviacEditor module started."));
}

void FBehaviacEditorModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("BehaviacEditor module shut down."));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBehaviacEditorModule, BehaviacEditor)
