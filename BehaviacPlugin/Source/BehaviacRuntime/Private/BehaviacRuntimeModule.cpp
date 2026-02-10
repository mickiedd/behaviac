// Copyright (C) 2015-2017 THL A29 Limited, a Tencent company.
// Licensed under the BSD 3-Clause License. UE5 plugin port.

#include "BehaviacRuntimeModule.h"

#define LOCTEXT_NAMESPACE "FBehaviacRuntimeModule"

void FBehaviacRuntimeModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("BehaviacRuntime module started. Version 1.0.0 (ported from behaviac 3.6.39)"));
}

void FBehaviacRuntimeModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("BehaviacRuntime module shut down."));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBehaviacRuntimeModule, BehaviacRuntime)
