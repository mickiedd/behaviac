// Behaviac UE5 Plugin
// Licensed under the BSD 3-Clause License.

#include "BehaviacBehaviorTreeFactory.h"
#include "BehaviorTree/BehaviacBehaviorTree.h"
#include "AssetToolsModule.h"
#include "Misc/FileHelper.h"

// ===================================================================
// Create New Factory
// ===================================================================

UBehaviacBehaviorTreeFactory::UBehaviacBehaviorTreeFactory()
{
	SupportedClass = UBehaviacBehaviorTree::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UBehaviacBehaviorTreeFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UBehaviacBehaviorTree* NewTree = NewObject<UBehaviacBehaviorTree>(InParent, InClass, InName, Flags);
	NewTree->TreeName = InName.ToString();
	return NewTree;
}

FText UBehaviacBehaviorTreeFactory::GetDisplayName() const
{
	return FText::FromString(TEXT("Behaviac Behavior Tree"));
}

uint32 UBehaviacBehaviorTreeFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Misc;
}

// ===================================================================
// Import Factory
// ===================================================================

UBehaviacBehaviorTreeImportFactory::UBehaviacBehaviorTreeImportFactory()
{
	SupportedClass = UBehaviacBehaviorTree::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
	bText = false;

	Formats.Add(TEXT("xml;Behaviac Behavior Tree XML"));
}

bool UBehaviacBehaviorTreeImportFactory::FactoryCanImport(const FString& Filename)
{
	return Filename.EndsWith(TEXT(".xml"));
}

UObject* UBehaviacBehaviorTreeImportFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	FString FileContent;

	if (!FFileHelper::LoadFileToString(FileContent, *Filename))
	{
		UE_LOG(LogTemp, Error, TEXT("[Behaviac] Failed to read import file: %s"), *Filename);
		bOutOperationCanceled = false;
		return nullptr;
	}

	UBehaviacBehaviorTree* NewTree = NewObject<UBehaviacBehaviorTree>(InParent, InClass, InName, Flags);
	NewTree->TreeName = InName.ToString();
	NewTree->SourceFilePath = Filename;

	if (!NewTree->LoadFromXML(FileContent))
	{
		UE_LOG(LogTemp, Error, TEXT("[Behaviac] Failed to parse behavior tree from: %s"), *Filename);
		bOutOperationCanceled = false;
		return nullptr;
	}

	UE_LOG(LogTemp, Log, TEXT("[Behaviac] Successfully imported behavior tree: %s"), *Filename);
	bOutOperationCanceled = false;
	return NewTree;
}
