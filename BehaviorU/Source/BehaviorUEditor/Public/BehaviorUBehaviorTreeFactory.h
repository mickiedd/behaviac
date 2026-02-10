// BehaviorU UE5 Plugin
// Licensed under the BSD 3-Clause License.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "BehaviorUBehaviorTreeFactory.generated.h"

/**
 * Factory for creating UBehaviorUBehaviorTree assets in the editor.
 * Supports creating new empty trees and importing from XML files.
 */
UCLASS()
class BEHAVIOUREDITOR_API UBehaviorUBehaviorTreeFactory : public UFactory
{
	GENERATED_BODY()

public:
	UBehaviorUBehaviorTreeFactory();

	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ShouldShowInNewMenu() const override { return true; }
	virtual FText GetDisplayName() const override;
	virtual uint32 GetMenuCategories() const override;
};

/**
 * Factory for importing XML behavior tree files into UBehaviorUBehaviorTree assets.
 */
UCLASS()
class BEHAVIOUREDITOR_API UBehaviorUBehaviorTreeImportFactory : public UFactory
{
	GENERATED_BODY()

public:
	UBehaviorUBehaviorTreeImportFactory();

	// UFactory interface
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
	virtual bool FactoryCanImport(const FString& Filename) override;
};
