// Behaviac UE5 Plugin
// Licensed under the BSD 3-Clause License.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "BehaviacBehaviorTreeFactory.generated.h"

/**
 * Factory for creating UBehaviacBehaviorTree assets in the editor.
 * Supports creating new empty trees and importing from XML files.
 */
UCLASS()
class BEHAVIACEDITOR_API UBehaviacBehaviorTreeFactory : public UFactory
{
	GENERATED_BODY()

public:
	UBehaviacBehaviorTreeFactory();

	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ShouldShowInNewMenu() const override { return true; }
	virtual FText GetDisplayName() const override;
	virtual uint32 GetMenuCategories() const override;
};

/**
 * Factory for importing XML behavior tree files into UBehaviacBehaviorTree assets.
 */
UCLASS()
class BEHAVIACEDITOR_API UBehaviacBehaviorTreeImportFactory : public UFactory
{
	GENERATED_BODY()

public:
	UBehaviacBehaviorTreeImportFactory();

	// UFactory interface
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
	virtual bool FactoryCanImport(const FString& Filename) override;
};
