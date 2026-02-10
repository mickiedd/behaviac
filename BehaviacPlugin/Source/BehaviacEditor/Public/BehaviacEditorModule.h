// Copyright (C) 2015-2017 THL A29 Limited, a Tencent company.
// Licensed under the BSD 3-Clause License. UE5 plugin port.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FBehaviacEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
