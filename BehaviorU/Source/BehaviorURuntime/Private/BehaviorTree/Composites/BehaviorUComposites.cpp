// BehaviorU UE5 Plugin
// Licensed under the BSD 3-Clause License.

#include "BehaviorTree/Composites/BehaviorUComposites.h"
#include "BehaviorUAgent.h"

// ===================================================================
// SELECTOR
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUSelector::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUSelectorTask>(Outer);
}

void UBehaviorUSelector::LoadFromProperties(int32 Version, const FString& InAgentType, const TArray<FBehaviorUProperty>& Properties)
{
	Super::LoadFromProperties(Version, InAgentType, Properties);
}

bool UBehaviorUSelector::Evaluate(UBehaviorUAgentComponent* Agent) const
{
	for (UBehaviorUBehaviorNode* Child : Children)
	{
		const UBehaviorUSelector* ChildSelector = Cast<UBehaviorUSelector>(Child);
		if (ChildSelector && ChildSelector->Evaluate(Agent))
		{
			return true;
		}
	}
	return false;
}

bool UBehaviorUSelector::CheckIfInterrupted(UBehaviorUAgentComponent* Agent) const
{
	return false;
}

bool UBehaviorUSelectorTask::OnEnter(UBehaviorUAgentComponent* Agent)
{
	ActiveChildIndex = 0;
	return true;
}

void UBehaviorUSelectorTask::OnExit(UBehaviorUAgentComponent* Agent, EBehaviorUStatus InStatus)
{
}

EBehaviorUStatus UBehaviorUSelectorTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	// If the current child returned success or is still running, propagate that
	if (ChildStatus == EBehaviorUStatus::Success)
	{
		return EBehaviorUStatus::Success;
	}

	// Current child failed, try next
	if (ChildStatus == EBehaviorUStatus::Failure)
	{
		ActiveChildIndex++;
	}

	// Try children from current index
	while (ChildTasks.IsValidIndex(ActiveChildIndex))
	{
		EBehaviorUStatus Result = ChildTasks[ActiveChildIndex]->Execute(Agent, EBehaviorUStatus::Invalid);

		if (Result == EBehaviorUStatus::Running)
		{
			return EBehaviorUStatus::Running;
		}

		if (Result == EBehaviorUStatus::Success)
		{
			return EBehaviorUStatus::Success;
		}

		// Failure: move to next child
		ActiveChildIndex++;
	}

	// All children failed
	return EBehaviorUStatus::Failure;
}

// ===================================================================
// SEQUENCE
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUSequence::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUSequenceTask>(Outer);
}

void UBehaviorUSequence::LoadFromProperties(int32 Version, const FString& InAgentType, const TArray<FBehaviorUProperty>& Properties)
{
	Super::LoadFromProperties(Version, InAgentType, Properties);
}

bool UBehaviorUSequenceTask::OnEnter(UBehaviorUAgentComponent* Agent)
{
	ActiveChildIndex = 0;
	return true;
}

void UBehaviorUSequenceTask::OnExit(UBehaviorUAgentComponent* Agent, EBehaviorUStatus InStatus)
{
}

EBehaviorUStatus UBehaviorUSequenceTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	// If current child is running, keep running
	if (ChildStatus == EBehaviorUStatus::Running)
	{
		if (ChildTasks.IsValidIndex(ActiveChildIndex))
		{
			return ChildTasks[ActiveChildIndex]->Execute(Agent, EBehaviorUStatus::Invalid);
		}
	}

	// If current child failed, propagate failure
	if (ChildStatus == EBehaviorUStatus::Failure)
	{
		return EBehaviorUStatus::Failure;
	}

	// Current child succeeded, move to next
	if (ChildStatus == EBehaviorUStatus::Success)
	{
		ActiveChildIndex++;
	}

	// Try children from current index
	while (ChildTasks.IsValidIndex(ActiveChildIndex))
	{
		EBehaviorUStatus Result = ChildTasks[ActiveChildIndex]->Execute(Agent, EBehaviorUStatus::Invalid);

		if (Result == EBehaviorUStatus::Running)
		{
			return EBehaviorUStatus::Running;
		}

		if (Result == EBehaviorUStatus::Failure)
		{
			return EBehaviorUStatus::Failure;
		}

		// Success: move to next child
		ActiveChildIndex++;
	}

	// All children succeeded
	return EBehaviorUStatus::Success;
}

// ===================================================================
// PARALLEL
// ===================================================================

UBehaviorUParallel::UBehaviorUParallel()
	: FailurePolicy(EBehaviorUParallelPolicy::FailOnOne_SucceedOnAll)
	, ChildFinishPolicy(EBehaviorUChildFinishPolicy::Once)
{
}

UBehaviorUBehaviorTask* UBehaviorUParallel::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUParallelTask>(Outer);
}

void UBehaviorUParallel::LoadFromProperties(int32 Version, const FString& InAgentType, const TArray<FBehaviorUProperty>& Properties)
{
	Super::LoadFromProperties(Version, InAgentType, Properties);

	for (const FBehaviorUProperty& Prop : Properties)
	{
		if (Prop.Name == TEXT("FailurePolicy"))
		{
			if (Prop.Value == TEXT("FAIL_ON_ONE"))
				FailurePolicy = EBehaviorUParallelPolicy::FailOnOne_SucceedOnAll;
			else if (Prop.Value == TEXT("FAIL_ON_ALL"))
				FailurePolicy = EBehaviorUParallelPolicy::FailOnAll_SucceedOnOne;
		}
		else if (Prop.Name == TEXT("ChildFinishPolicy"))
		{
			if (Prop.Value == TEXT("CHILDFINISH_LOOP"))
				ChildFinishPolicy = EBehaviorUChildFinishPolicy::Loop;
			else
				ChildFinishPolicy = EBehaviorUChildFinishPolicy::Once;
		}
	}
}

UBehaviorUParallelTask::UBehaviorUParallelTask()
{
}

void UBehaviorUParallelTask::Init(UBehaviorUBehaviorNode* InNode)
{
	Super::Init(InNode);
	ChildStatuses.SetNum(ChildTasks.Num());
	for (int32 i = 0; i < ChildStatuses.Num(); i++)
	{
		ChildStatuses[i] = EBehaviorUStatus::Invalid;
	}
}

void UBehaviorUParallelTask::Reset(UBehaviorUAgentComponent* Agent)
{
	Super::Reset(Agent);
	for (int32 i = 0; i < ChildStatuses.Num(); i++)
	{
		ChildStatuses[i] = EBehaviorUStatus::Invalid;
	}
}

bool UBehaviorUParallelTask::OnEnter(UBehaviorUAgentComponent* Agent)
{
	return true;
}

EBehaviorUStatus UBehaviorUParallelTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	const UBehaviorUParallel* ParallelNode = Cast<UBehaviorUParallel>(Node);
	if (!ParallelNode)
	{
		return EBehaviorUStatus::Failure;
	}

	int32 SuccessCount = 0;
	int32 FailCount = 0;
	int32 RunningCount = 0;

	for (int32 i = 0; i < ChildTasks.Num(); i++)
	{
		// Skip completed children if policy is Once
		if (ParallelNode->ChildFinishPolicy == EBehaviorUChildFinishPolicy::Once &&
			ChildStatuses[i] != EBehaviorUStatus::Invalid &&
			ChildStatuses[i] != EBehaviorUStatus::Running)
		{
			if (ChildStatuses[i] == EBehaviorUStatus::Success) SuccessCount++;
			else if (ChildStatuses[i] == EBehaviorUStatus::Failure) FailCount++;
			continue;
		}

		EBehaviorUStatus Result = ChildTasks[i]->Execute(Agent, EBehaviorUStatus::Invalid);
		ChildStatuses[i] = Result;

		switch (Result)
		{
		case EBehaviorUStatus::Success:	SuccessCount++; break;
		case EBehaviorUStatus::Failure:	FailCount++; break;
		case EBehaviorUStatus::Running:	RunningCount++; break;
		default: break;
		}
	}

	// Determine result based on policy
	switch (ParallelNode->FailurePolicy)
	{
	case EBehaviorUParallelPolicy::FailOnOne_SucceedOnAll:
		if (FailCount > 0) return EBehaviorUStatus::Failure;
		if (SuccessCount == ChildTasks.Num()) return EBehaviorUStatus::Success;
		break;

	case EBehaviorUParallelPolicy::FailOnAll_SucceedOnOne:
		if (SuccessCount > 0) return EBehaviorUStatus::Success;
		if (FailCount == ChildTasks.Num()) return EBehaviorUStatus::Failure;
		break;

	case EBehaviorUParallelPolicy::FailOnOne_SucceedOnOne:
		if (FailCount > 0) return EBehaviorUStatus::Failure;
		if (SuccessCount > 0) return EBehaviorUStatus::Success;
		break;
	}

	return EBehaviorUStatus::Running;
}

// ===================================================================
// IF-ELSE
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUIfElse::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUIfElseTask>(Outer);
}

bool UBehaviorUIfElseTask::OnEnter(UBehaviorUAgentComponent* Agent)
{
	return ChildTasks.Num() >= 2;
}

EBehaviorUStatus UBehaviorUIfElseTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	if (ChildTasks.Num() < 2)
	{
		return EBehaviorUStatus::Failure;
	}

	// First child is the condition
	if (ActiveChildIndex == 0)
	{
		EBehaviorUStatus CondResult = ChildTasks[0]->Execute(Agent, EBehaviorUStatus::Invalid);

		if (CondResult == EBehaviorUStatus::Running)
		{
			return EBehaviorUStatus::Running;
		}

		// Select branch: index 1 for true, index 2 for false
		ActiveChildIndex = (CondResult == EBehaviorUStatus::Success) ? 1 : 2;
	}

	// Execute selected branch
	if (ChildTasks.IsValidIndex(ActiveChildIndex))
	{
		return ChildTasks[ActiveChildIndex]->Execute(Agent, ChildStatus);
	}

	return EBehaviorUStatus::Failure;
}

// ===================================================================
// SELECTOR LOOP
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUSelectorLoop::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUSelectorLoopTask>(Outer);
}

bool UBehaviorUSelectorLoopTask::OnEnter(UBehaviorUAgentComponent* Agent)
{
	ActiveChildIndex = 0;
	return true;
}

EBehaviorUStatus UBehaviorUSelectorLoopTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	// Re-evaluate from the beginning to check if higher priority child is valid
	for (int32 i = 0; i < ChildTasks.Num(); i++)
	{
		// If a higher-priority child can now run, interrupt current
		if (i < ActiveChildIndex)
		{
			UBehaviorUBehaviorNode* ChildNode = Node ? Node->GetChild(i) : nullptr;
			if (ChildNode)
			{
				// Reset and try this child
				EBehaviorUStatus Result = ChildTasks[i]->Execute(Agent, EBehaviorUStatus::Invalid);
				if (Result != EBehaviorUStatus::Failure)
				{
					// Interrupt current child
					if (ChildTasks.IsValidIndex(ActiveChildIndex))
					{
						ChildTasks[ActiveChildIndex]->Reset(Agent);
					}
					ActiveChildIndex = i;
					return Result;
				}
			}
		}
		else if (i == ActiveChildIndex)
		{
			EBehaviorUStatus Result = ChildTasks[i]->Execute(Agent, ChildStatus);

			if (Result == EBehaviorUStatus::Running)
			{
				return EBehaviorUStatus::Running;
			}

			if (Result == EBehaviorUStatus::Success)
			{
				return EBehaviorUStatus::Success;
			}

			ActiveChildIndex++;
		}
	}

	return EBehaviorUStatus::Failure;
}

// ===================================================================
// SELECTOR PROBABILITY
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUSelectorProbability::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUSelectorProbabilityTask>(Outer);
}

bool UBehaviorUSelectorProbabilityTask::OnEnter(UBehaviorUAgentComponent* Agent)
{
	return ChildTasks.Num() > 0;
}

EBehaviorUStatus UBehaviorUSelectorProbabilityTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	if (ChildTasks.Num() == 0)
	{
		return EBehaviorUStatus::Failure;
	}

	// Pick weighted random child (weights from decorator weight nodes)
	// For simplicity, use uniform random if no weights
	if (ActiveChildIndex < 0 || ActiveChildIndex >= ChildTasks.Num())
	{
		ActiveChildIndex = FMath::RandRange(0, ChildTasks.Num() - 1);
	}

	return ChildTasks[ActiveChildIndex]->Execute(Agent, ChildStatus);
}

// ===================================================================
// SELECTOR STOCHASTIC
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUSelectorStochastic::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUSelectorStochasticTask>(Outer);
}

UBehaviorUSelectorStochasticTask::UBehaviorUSelectorStochasticTask()
{
}

void UBehaviorUSelectorStochasticTask::Reset(UBehaviorUAgentComponent* Agent)
{
	Super::Reset(Agent);
	ShuffledOrder.Empty();
}

bool UBehaviorUSelectorStochasticTask::OnEnter(UBehaviorUAgentComponent* Agent)
{
	ActiveChildIndex = 0;

	// Create shuffled order
	ShuffledOrder.SetNum(ChildTasks.Num());
	for (int32 i = 0; i < ShuffledOrder.Num(); i++)
	{
		ShuffledOrder[i] = i;
	}

	// Fisher-Yates shuffle
	for (int32 i = ShuffledOrder.Num() - 1; i > 0; i--)
	{
		int32 j = FMath::RandRange(0, i);
		ShuffledOrder.Swap(i, j);
	}

	return true;
}

EBehaviorUStatus UBehaviorUSelectorStochasticTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	if (ChildStatus == EBehaviorUStatus::Success)
	{
		return EBehaviorUStatus::Success;
	}

	if (ChildStatus == EBehaviorUStatus::Failure)
	{
		ActiveChildIndex++;
	}

	while (ActiveChildIndex < ShuffledOrder.Num())
	{
		int32 ChildIdx = ShuffledOrder[ActiveChildIndex];
		if (ChildTasks.IsValidIndex(ChildIdx))
		{
			EBehaviorUStatus Result = ChildTasks[ChildIdx]->Execute(Agent, EBehaviorUStatus::Invalid);
			if (Result != EBehaviorUStatus::Failure)
			{
				return Result;
			}
		}
		ActiveChildIndex++;
	}

	return EBehaviorUStatus::Failure;
}

// ===================================================================
// SEQUENCE STOCHASTIC
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUSequenceStochastic::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUSequenceStochasticTask>(Outer);
}

UBehaviorUSequenceStochasticTask::UBehaviorUSequenceStochasticTask()
{
}

void UBehaviorUSequenceStochasticTask::Reset(UBehaviorUAgentComponent* Agent)
{
	Super::Reset(Agent);
	ShuffledOrder.Empty();
}

bool UBehaviorUSequenceStochasticTask::OnEnter(UBehaviorUAgentComponent* Agent)
{
	ActiveChildIndex = 0;

	ShuffledOrder.SetNum(ChildTasks.Num());
	for (int32 i = 0; i < ShuffledOrder.Num(); i++)
	{
		ShuffledOrder[i] = i;
	}

	for (int32 i = ShuffledOrder.Num() - 1; i > 0; i--)
	{
		int32 j = FMath::RandRange(0, i);
		ShuffledOrder.Swap(i, j);
	}

	return true;
}

EBehaviorUStatus UBehaviorUSequenceStochasticTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	if (ChildStatus == EBehaviorUStatus::Failure)
	{
		return EBehaviorUStatus::Failure;
	}

	if (ChildStatus == EBehaviorUStatus::Success)
	{
		ActiveChildIndex++;
	}

	while (ActiveChildIndex < ShuffledOrder.Num())
	{
		int32 ChildIdx = ShuffledOrder[ActiveChildIndex];
		if (ChildTasks.IsValidIndex(ChildIdx))
		{
			EBehaviorUStatus Result = ChildTasks[ChildIdx]->Execute(Agent, EBehaviorUStatus::Invalid);
			if (Result != EBehaviorUStatus::Success)
			{
				return Result;
			}
		}
		ActiveChildIndex++;
	}

	return EBehaviorUStatus::Success;
}

// ===================================================================
// REFERENCE BEHAVIOR
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUReferenceBehavior::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUReferenceBehaviorTask>(Outer);
}

void UBehaviorUReferenceBehavior::LoadFromProperties(int32 Version, const FString& InAgentType, const TArray<FBehaviorUProperty>& Properties)
{
	Super::LoadFromProperties(Version, InAgentType, Properties);

	for (const FBehaviorUProperty& Prop : Properties)
	{
		if (Prop.Name == TEXT("ReferenceFilename") || Prop.Name == TEXT("ReferenceBehavior"))
		{
			ReferencedTreePath = Prop.Value;
		}
	}
}

bool UBehaviorUReferenceBehaviorTask::OnEnter(UBehaviorUAgentComponent* Agent)
{
	// The sub-tree is loaded via the workspace and linked during Init
	return true;
}

EBehaviorUStatus UBehaviorUReferenceBehaviorTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	if (SubTreeTask)
	{
		return SubTreeTask->Tick(Agent);
	}

	if (ChildTask)
	{
		return ChildTask->Execute(Agent, ChildStatus);
	}

	return EBehaviorUStatus::Failure;
}

// ===================================================================
// WITH PRECONDITION
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUWithPrecondition::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUWithPreconditionTask>(Outer);
}

EBehaviorUStatus UBehaviorUWithPreconditionTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	if (ChildTasks.Num() < 2)
	{
		return EBehaviorUStatus::Failure;
	}

	// First child: precondition
	EBehaviorUStatus PrecondResult = ChildTasks[0]->Execute(Agent, EBehaviorUStatus::Invalid);

	if (PrecondResult != EBehaviorUStatus::Success)
	{
		return EBehaviorUStatus::Failure;
	}

	// Second child: the actual action
	return ChildTasks[1]->Execute(Agent, ChildStatus);
}
