// Copyright (C) 2015-2017 THL A29 Limited, a Tencent company.
// Licensed under the BSD 3-Clause License. UE5 plugin port.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FBehaviacRuntimeModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Get this module */
	static FBehaviacRuntimeModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FBehaviacRuntimeModule>("BehaviacRuntime");
	}

	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("BehaviacRuntime");
	}
};
