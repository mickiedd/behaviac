// BehaviorU UE5 Plugin
// Licensed under the BSD 3-Clause License.

#include "BehaviorUAgent.h"
#include "BehaviorTree/BehaviorUBehaviorTree.h"
#include "BehaviorTree/BehaviorUBehaviorTask.h"
#include "BehaviorTree/BehaviorUBehaviorNode.h"

UBehaviorUAgentComponent::UBehaviorUAgentComponent()
	: bAutoTick(true)
	, DefaultBehaviorTree(nullptr)
	, CurrentTreeTask(nullptr)
	, CurrentTreeAsset(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UBehaviorUAgentComponent::BeginPlay()
{
	Super::BeginPlay();

	if (DefaultBehaviorTree)
	{
		LoadBehaviorTree(DefaultBehaviorTree);
	}
}

void UBehaviorUAgentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bAutoTick && CurrentTreeTask)
	{
		TickBehaviorTree();
	}
}

void UBehaviorUAgentComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopBehaviorTree();
	Super::EndPlay(EndPlayReason);
}

// --- Behavior Tree Management ---

bool UBehaviorUAgentComponent::LoadBehaviorTree(UBehaviorUBehaviorTree* TreeAsset)
{
	if (!TreeAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BehaviorU] Cannot load null behavior tree"));
		return false;
	}

	StopBehaviorTree();

	CurrentTreeAsset = TreeAsset;

	UBehaviorUBehaviorNode* RootNode = TreeAsset->GetRootNode();
	if (!RootNode)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BehaviorU] Behavior tree has no root node: %s"), *TreeAsset->GetName());
		return false;
	}

	// Create the root task (BehaviorTreeTask wrapping the root node)
	CurrentTreeTask = NewObject<UBehaviorUBehaviorTreeTask>(this);
	CurrentTreeTask->Init(RootNode);

	UE_LOG(LogTemp, Log, TEXT("[BehaviorU] Loaded behavior tree: %s"), *TreeAsset->GetName());
	return true;
}

bool UBehaviorUAgentComponent::LoadBehaviorTreeByPath(const FString& RelativePath)
{
	// Try to find the asset
	FString AssetPath = FString::Printf(TEXT("/Game/BehaviorUData/%s"), *RelativePath);
	UBehaviorUBehaviorTree* TreeAsset = LoadObject<UBehaviorUBehaviorTree>(nullptr, *AssetPath);

	if (TreeAsset)
	{
		return LoadBehaviorTree(TreeAsset);
	}

	UE_LOG(LogTemp, Warning, TEXT("[BehaviorU] Could not find behavior tree at path: %s"), *AssetPath);
	return false;
}

EBehaviorUStatus UBehaviorUAgentComponent::TickBehaviorTree()
{
	if (!CurrentTreeTask)
	{
		return EBehaviorUStatus::Invalid;
	}

	EBehaviorUStatus Result = CurrentTreeTask->Tick(this);

	// If tree completed, it stays completed until manually reset
	return Result;
}

void UBehaviorUAgentComponent::StopBehaviorTree()
{
	if (CurrentTreeTask)
	{
		CurrentTreeTask->Reset(this);
		CurrentTreeTask = nullptr;
	}

	CurrentTreeAsset = nullptr;
}

void UBehaviorUAgentComponent::ResetBehaviorTree()
{
	if (CurrentTreeTask)
	{
		CurrentTreeTask->Reset(this);
	}
}

EBehaviorUStatus UBehaviorUAgentComponent::GetBehaviorTreeStatus() const
{
	if (CurrentTreeTask)
	{
		return CurrentTreeTask->GetTreeStatus();
	}
	return EBehaviorUStatus::Invalid;
}

// --- Property System ---

void UBehaviorUAgentComponent::SetPropertyValue(const FString& PropertyName, const FString& Value)
{
	FScopeLock Lock(&PropertyLock);

	// Strip "Self." prefix if present
	FString CleanName = PropertyName;
	if (CleanName.StartsWith(TEXT("Self.")))
	{
		CleanName = CleanName.Mid(5);
	}

	Properties.Add(CleanName, Value);
}

FString UBehaviorUAgentComponent::GetPropertyValue(const FString& PropertyName) const
{
	FScopeLock Lock(&PropertyLock);

	FString CleanName = PropertyName;
	if (CleanName.StartsWith(TEXT("Self.")))
	{
		CleanName = CleanName.Mid(5);
	}

	const FString* Found = Properties.Find(CleanName);
	return Found ? *Found : FString();
}

bool UBehaviorUAgentComponent::HasProperty(const FString& PropertyName) const
{
	FScopeLock Lock(&PropertyLock);

	FString CleanName = PropertyName;
	if (CleanName.StartsWith(TEXT("Self.")))
	{
		CleanName = CleanName.Mid(5);
	}

	return Properties.Contains(CleanName);
}

void UBehaviorUAgentComponent::SetIntProperty(const FString& PropertyName, int32 Value)
{
	SetPropertyValue(PropertyName, FString::FromInt(Value));
}

int32 UBehaviorUAgentComponent::GetIntProperty(const FString& PropertyName) const
{
	return FCString::Atoi(*GetPropertyValue(PropertyName));
}

void UBehaviorUAgentComponent::SetFloatProperty(const FString& PropertyName, float Value)
{
	SetPropertyValue(PropertyName, FString::SanitizeFloat(Value));
}

float UBehaviorUAgentComponent::GetFloatProperty(const FString& PropertyName) const
{
	return FCString::Atof(*GetPropertyValue(PropertyName));
}

void UBehaviorUAgentComponent::SetBoolProperty(const FString& PropertyName, bool Value)
{
	SetPropertyValue(PropertyName, Value ? TEXT("true") : TEXT("false"));
}

bool UBehaviorUAgentComponent::GetBoolProperty(const FString& PropertyName) const
{
	FString Val = GetPropertyValue(PropertyName);
	return Val.Equals(TEXT("true"), ESearchCase::IgnoreCase) || Val == TEXT("1");
}

// --- Method System ---

EBehaviorUStatus UBehaviorUAgentComponent::ExecuteMethod(const FString& MethodName)
{
	// First check registered C++ handlers
	if (TFunction<EBehaviorUStatus()>* Handler = MethodHandlers.Find(MethodName))
	{
		return (*Handler)();
	}

	// Try Blueprint delegate
	if (OnMethodCalled.IsBound())
	{
		EBehaviorUStatus Result = EBehaviorUStatus::Invalid;
		OnMethodCalled.Broadcast(MethodName, Result);
		if (Result != EBehaviorUStatus::Invalid)
		{
			return Result;
		}
	}

	// Fall back to Blueprint implementable event
	EBehaviorUStatus BlueprintResult = OnExecuteMethod(MethodName);
	if (BlueprintResult != EBehaviorUStatus::Invalid)
	{
		return BlueprintResult;
	}

	UE_LOG(LogTemp, Verbose, TEXT("[BehaviorU] No handler for method: %s"), *MethodName);
	return EBehaviorUStatus::Invalid;
}

void UBehaviorUAgentComponent::RegisterMethodHandler(const FString& MethodName, TFunction<EBehaviorUStatus()> Handler)
{
	MethodHandlers.Add(MethodName, MoveTemp(Handler));
}

// --- Signal System ---

void UBehaviorUAgentComponent::SendSignal(const FString& SignalName)
{
	ActiveSignals.Add(SignalName);
	OnSignalReceived.Broadcast(SignalName);
}

bool UBehaviorUAgentComponent::IsSignalSet(const FString& SignalName) const
{
	return ActiveSignals.Contains(SignalName);
}

void UBehaviorUAgentComponent::ClearSignal(const FString& SignalName)
{
	ActiveSignals.Remove(SignalName);
}

void UBehaviorUAgentComponent::ClearAllSignals()
{
	ActiveSignals.Empty();
}

// --- Event System ---

void UBehaviorUAgentComponent::FireEvent(const FString& EventName)
{
	PendingEvents.Add(EventName);
}

bool UBehaviorUAgentComponent::HasPendingEvent(const FString& EventName) const
{
	return PendingEvents.Contains(EventName);
}

void UBehaviorUAgentComponent::ConsumeEvent(const FString& EventName)
{
	PendingEvents.Remove(EventName);
}
