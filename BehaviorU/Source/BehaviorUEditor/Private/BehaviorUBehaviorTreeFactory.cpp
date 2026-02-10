// BehaviorU UE5 Plugin
// Licensed under the BSD 3-Clause License.

#include "BehaviorUBehaviorTreeFactory.h"
#include "BehaviorTree/BehaviorUBehaviorTree.h"
#include "AssetToolsModule.h"
#include "Misc/FileHelper.h"

// ===================================================================
// Create New Factory
// ===================================================================

UBehaviorUBehaviorTreeFactory::UBehaviorUBehaviorTreeFactory()
{
	SupportedClass = UBehaviorUBehaviorTree::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UBehaviorUBehaviorTreeFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UBehaviorUBehaviorTree* NewTree = NewObject<UBehaviorUBehaviorTree>(InParent, InClass, InName, Flags);
	NewTree->TreeName = InName.ToString();
	return NewTree;
}

FText UBehaviorUBehaviorTreeFactory::GetDisplayName() const
{
	return FText::FromString(TEXT("BehaviorU Behavior Tree"));
}

uint32 UBehaviorUBehaviorTreeFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Misc;
}

// ===================================================================
// Import Factory
// ===================================================================

UBehaviorUBehaviorTreeImportFactory::UBehaviorUBehaviorTreeImportFactory()
{
	SupportedClass = UBehaviorUBehaviorTree::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
	bText = false;

	Formats.Add(TEXT("xml;BehaviorU Behavior Tree XML"));
}

bool UBehaviorUBehaviorTreeImportFactory::FactoryCanImport(const FString& Filename)
{
	return Filename.EndsWith(TEXT(".xml"));
}

UObject* UBehaviorUBehaviorTreeImportFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	FString FileContent;

	if (!FFileHelper::LoadFileToString(FileContent, *Filename))
	{
		UE_LOG(LogTemp, Error, TEXT("[BehaviorU] Failed to read import file: %s"), *Filename);
		bOutOperationCanceled = false;
		return nullptr;
	}

	UBehaviorUBehaviorTree* NewTree = NewObject<UBehaviorUBehaviorTree>(InParent, InClass, InName, Flags);
	NewTree->TreeName = InName.ToString();
	NewTree->SourceFilePath = Filename;

	if (!NewTree->LoadFromXML(FileContent))
	{
		UE_LOG(LogTemp, Error, TEXT("[BehaviorU] Failed to parse behavior tree from: %s"), *Filename);
		bOutOperationCanceled = false;
		return nullptr;
	}

	UE_LOG(LogTemp, Log, TEXT("[BehaviorU] Successfully imported behavior tree: %s"), *Filename);
	bOutOperationCanceled = false;
	return NewTree;
}
