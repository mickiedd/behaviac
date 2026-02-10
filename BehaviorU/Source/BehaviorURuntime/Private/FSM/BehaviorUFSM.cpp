// BehaviorU UE5 Plugin
// Licensed under the BSD 3-Clause License.

#include "FSM/BehaviorUFSM.h"
#include "BehaviorUAgent.h"

// ===================================================================
// FSM TRANSITIONS
// ===================================================================

UBehaviorUFSMTransition::UBehaviorUFSMTransition()
	: TargetStateId(-1)
{
}

bool UBehaviorUFSMTransition::Evaluate(UBehaviorUAgentComponent* Agent) const
{
	return false;
}

void UBehaviorUFSMTransition::LoadFromProperties(const TArray<FBehaviorUProperty>& Properties)
{
	for (const FBehaviorUProperty& Prop : Properties)
	{
		if (Prop.Name == TEXT("TargetStateId"))
		{
			TargetStateId = FCString::Atoi(*Prop.Value);
		}
	}
}

bool UBehaviorUTransitionCondition::Evaluate(UBehaviorUAgentComponent* Agent) const
{
	if (!Agent) return false;

	FString LeftStr = LeftOperand;
	FString RightStr = RightOperand;

	if (LeftStr.StartsWith(TEXT("Self.")))
		LeftStr = Agent->GetPropertyValue(LeftStr);
	if (RightStr.StartsWith(TEXT("Self.")))
		RightStr = Agent->GetPropertyValue(RightStr);

	if (LeftStr.IsNumeric() && RightStr.IsNumeric())
	{
		double Left = FCString::Atod(*LeftStr);
		double Right = FCString::Atod(*RightStr);

		switch (Operator)
		{
		case EBehaviorUOperatorType::Equal:			return FMath::IsNearlyEqual(Left, Right);
		case EBehaviorUOperatorType::NotEqual:		return !FMath::IsNearlyEqual(Left, Right);
		case EBehaviorUOperatorType::Greater:			return Left > Right;
		case EBehaviorUOperatorType::Less:			return Left < Right;
		case EBehaviorUOperatorType::GreaterEqual:	return Left >= Right;
		case EBehaviorUOperatorType::LessEqual:		return Left <= Right;
		default: return false;
		}
	}

	return LeftStr == RightStr;
}

bool UBehaviorUWaitTransition::Evaluate(UBehaviorUAgentComponent* Agent) const
{
	// Wait transitions are time-based; evaluation is handled by the state task
	return false;
}

// ===================================================================
// FSM STATE
// ===================================================================

UBehaviorUFSMState::UBehaviorUFSMState()
	: StateId(-1)
	, bIsInitialState(false)
	, bIsFinalState(false)
{
}

UBehaviorUBehaviorTask* UBehaviorUFSMState::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUFSMStateTask>(Outer);
}

void UBehaviorUFSMState::LoadFromProperties(int32 Version, const FString& InAgentType, const TArray<FBehaviorUProperty>& Properties)
{
	Super::LoadFromProperties(Version, InAgentType, Properties);

	for (const FBehaviorUProperty& Prop : Properties)
	{
		if (Prop.Name == TEXT("StateId"))
		{
			StateId = FCString::Atoi(*Prop.Value);
		}
		else if (Prop.Name == TEXT("IsInitial"))
		{
			bIsInitialState = (Prop.Value == TEXT("true"));
		}
		else if (Prop.Name == TEXT("IsFinal"))
		{
			bIsFinalState = (Prop.Value == TEXT("true"));
		}
		else if (Prop.Name == TEXT("EnterAction"))
		{
			EnterAction = Prop.Value;
		}
		else if (Prop.Name == TEXT("ExitAction"))
		{
			ExitAction = Prop.Value;
		}
	}
}

bool UBehaviorUFSMStateTask::OnEnter(UBehaviorUAgentComponent* Agent)
{
	const UBehaviorUFSMState* StateNode = Cast<UBehaviorUFSMState>(Node);
	if (StateNode && !StateNode->EnterAction.IsEmpty() && Agent)
	{
		Agent->ExecuteMethod(StateNode->EnterAction);
	}
	return true;
}

void UBehaviorUFSMStateTask::OnExit(UBehaviorUAgentComponent* Agent, EBehaviorUStatus InStatus)
{
	const UBehaviorUFSMState* StateNode = Cast<UBehaviorUFSMState>(Node);
	if (StateNode && !StateNode->ExitAction.IsEmpty() && Agent)
	{
		Agent->ExecuteMethod(StateNode->ExitAction);
	}
}

EBehaviorUStatus UBehaviorUFSMStateTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	const UBehaviorUFSMState* StateNode = Cast<UBehaviorUFSMState>(Node);

	if (StateNode && StateNode->bIsFinalState)
	{
		return EBehaviorUStatus::Success;
	}

	// Execute the child behavior (if any)
	if (ChildTask)
	{
		return ChildTask->Execute(Agent, ChildStatus);
	}

	return EBehaviorUStatus::Running;
}

// ===================================================================
// WAIT FRAMES STATE / WAIT STATE
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUWaitFramesState::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUFSMStateTask>(Outer);
}

void UBehaviorUWaitFramesState::LoadFromProperties(int32 Version, const FString& InAgentType, const TArray<FBehaviorUProperty>& Properties)
{
	Super::LoadFromProperties(Version, InAgentType, Properties);
	WaitFrameCount = 1;

	for (const FBehaviorUProperty& Prop : Properties)
	{
		if (Prop.Name == TEXT("Frames"))
		{
			WaitFrameCount = FCString::Atoi(*Prop.Value);
		}
	}
}

UBehaviorUBehaviorTask* UBehaviorUWaitState::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUFSMStateTask>(Outer);
}

void UBehaviorUWaitState::LoadFromProperties(int32 Version, const FString& InAgentType, const TArray<FBehaviorUProperty>& Properties)
{
	Super::LoadFromProperties(Version, InAgentType, Properties);
	WaitDuration = 1.0f;

	for (const FBehaviorUProperty& Prop : Properties)
	{
		if (Prop.Name == TEXT("Time"))
		{
			WaitDuration = FCString::Atof(*Prop.Value);
		}
	}
}

// ===================================================================
// FSM NODE
// ===================================================================

UBehaviorUFSMNode::UBehaviorUFSMNode()
	: InitialStateId(0)
{
}

UBehaviorUBehaviorTask* UBehaviorUFSMNode::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUFSMTask>(Outer);
}

void UBehaviorUFSMNode::LoadFromProperties(int32 Version, const FString& InAgentType, const TArray<FBehaviorUProperty>& Properties)
{
	Super::LoadFromProperties(Version, InAgentType, Properties);

	for (const FBehaviorUProperty& Prop : Properties)
	{
		if (Prop.Name == TEXT("InitialId"))
		{
			InitialStateId = FCString::Atoi(*Prop.Value);
		}
	}
}

// ===================================================================
// FSM TASK
// ===================================================================

UBehaviorUFSMTask::UBehaviorUFSMTask()
	: CurrentStateIndex(-1)
{
}

bool UBehaviorUFSMTask::OnEnter(UBehaviorUAgentComponent* Agent)
{
	const UBehaviorUFSMNode* FSMNode = Cast<UBehaviorUFSMNode>(Node);
	if (!FSMNode)
	{
		return false;
	}

	// Find the initial state
	for (int32 i = 0; i < ChildTasks.Num(); i++)
	{
		if (UBehaviorUBehaviorTask* Task = ChildTasks[i])
		{
			const UBehaviorUFSMState* StateNode = Cast<UBehaviorUFSMState>(Task->GetNode());
			if (StateNode && StateNode->StateId == FSMNode->InitialStateId)
			{
				CurrentStateIndex = i;
				return true;
			}
		}
	}

	// Default to first child
	CurrentStateIndex = ChildTasks.Num() > 0 ? 0 : -1;
	return CurrentStateIndex >= 0;
}

void UBehaviorUFSMTask::OnExit(UBehaviorUAgentComponent* Agent, EBehaviorUStatus InStatus)
{
	// Exit current state
	if (ChildTasks.IsValidIndex(CurrentStateIndex))
	{
		ChildTasks[CurrentStateIndex]->Reset(Agent);
	}
}

EBehaviorUStatus UBehaviorUFSMTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	return UpdateFSM(Agent, ChildStatus);
}

EBehaviorUStatus UBehaviorUFSMTask::UpdateFSM(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	if (!ChildTasks.IsValidIndex(CurrentStateIndex))
	{
		return EBehaviorUStatus::Failure;
	}

	// Execute current state
	UBehaviorUBehaviorTask* CurrentStateTask = ChildTasks[CurrentStateIndex];
	EBehaviorUStatus StateResult = CurrentStateTask->Execute(Agent, ChildStatus);

	// Check transitions from current state
	const UBehaviorUFSMState* CurrentState = Cast<UBehaviorUFSMState>(CurrentStateTask->GetNode());
	if (CurrentState)
	{
		// Check if this is a final state and it completed
		if (CurrentState->bIsFinalState && StateResult != EBehaviorUStatus::Running)
		{
			return StateResult;
		}

		// Check transitions
		for (UBehaviorUFSMTransition* Transition : CurrentState->Transitions)
		{
			if (Transition && Transition->Evaluate(Agent))
			{
				// Exit current state
				CurrentStateTask->Reset(Agent);

				// Find and enter target state
				UBehaviorUBehaviorTask* TargetTask = FindStateTaskById(Transition->TargetStateId);
				if (TargetTask)
				{
					for (int32 i = 0; i < ChildTasks.Num(); i++)
					{
						if (ChildTasks[i] == TargetTask)
						{
							CurrentStateIndex = i;
							break;
						}
					}
				}

				return EBehaviorUStatus::Running;
			}
		}
	}

	return StateResult == EBehaviorUStatus::Running ? EBehaviorUStatus::Running : StateResult;
}

UBehaviorUBehaviorTask* UBehaviorUFSMTask::FindStateTaskById(int32 StateId) const
{
	for (UBehaviorUBehaviorTask* Task : ChildTasks)
	{
		if (Task)
		{
			const UBehaviorUFSMState* StateNode = Cast<UBehaviorUFSMState>(Task->GetNode());
			if (StateNode && StateNode->StateId == StateId)
			{
				return Task;
			}
		}
	}
	return nullptr;
}
