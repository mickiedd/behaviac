// BehaviorU UE5 Plugin
// Licensed under the BSD 3-Clause License.

#include "BehaviorTree/Actions/BehaviorUActions.h"
#include "BehaviorUAgent.h"

// ===================================================================
// ACTION
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUAction::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUActionTask>(Outer);
}

void UBehaviorUAction::LoadFromProperties(int32 Version, const FString& InAgentType, const TArray<FBehaviorUProperty>& Properties)
{
	Super::LoadFromProperties(Version, InAgentType, Properties);
	ResultOption = EBehaviorUStatus::Success;

	for (const FBehaviorUProperty& Prop : Properties)
	{
		if (Prop.Name == TEXT("Method"))
		{
			MethodName = Prop.Value;
		}
		else if (Prop.Name == TEXT("ResultOption"))
		{
			if (Prop.Value == TEXT("BT_SUCCESS")) ResultOption = EBehaviorUStatus::Success;
			else if (Prop.Value == TEXT("BT_FAILURE")) ResultOption = EBehaviorUStatus::Failure;
			else if (Prop.Value == TEXT("BT_RUNNING")) ResultOption = EBehaviorUStatus::Running;
		}
	}
}

EBehaviorUStatus UBehaviorUActionTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	const UBehaviorUAction* ActionNode = Cast<UBehaviorUAction>(Node);
	if (!ActionNode || !Agent)
	{
		return EBehaviorUStatus::Failure;
	}

	// Call the method on the agent
	EBehaviorUStatus Result = Agent->ExecuteMethod(ActionNode->MethodName);

	if (Result != EBehaviorUStatus::Invalid)
	{
		return Result;
	}

	return ActionNode->ResultOption;
}

// ===================================================================
// ASSIGNMENT
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUAssignment::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUAssignmentTask>(Outer);
}

void UBehaviorUAssignment::LoadFromProperties(int32 Version, const FString& InAgentType, const TArray<FBehaviorUProperty>& Properties)
{
	Super::LoadFromProperties(Version, InAgentType, Properties);
	bCastFromRight = false;

	for (const FBehaviorUProperty& Prop : Properties)
	{
		if (Prop.Name == TEXT("Opl"))
		{
			PropertyName = Prop.Value;
		}
		else if (Prop.Name == TEXT("Opr"))
		{
			PropertyValue = Prop.Value;
		}
		else if (Prop.Name == TEXT("CastRight"))
		{
			bCastFromRight = (Prop.Value == TEXT("true"));
		}
	}
}

EBehaviorUStatus UBehaviorUAssignmentTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	const UBehaviorUAssignment* AssignNode = Cast<UBehaviorUAssignment>(Node);
	if (!AssignNode || !Agent)
	{
		return EBehaviorUStatus::Failure;
	}

	FString Value = AssignNode->PropertyValue;

	// Resolve if it references another property
	if (Value.StartsWith(TEXT("Self.")))
	{
		Value = Agent->GetPropertyValue(Value);
	}

	Agent->SetPropertyValue(AssignNode->PropertyName, Value);
	return EBehaviorUStatus::Success;
}

// ===================================================================
// COMPUTE
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUCompute::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUComputeTask>(Outer);
}

void UBehaviorUCompute::LoadFromProperties(int32 Version, const FString& InAgentType, const TArray<FBehaviorUProperty>& Properties)
{
	Super::LoadFromProperties(Version, InAgentType, Properties);
	Operator = EBehaviorUOperatorType::Add;

	for (const FBehaviorUProperty& Prop : Properties)
	{
		if (Prop.Name == TEXT("Opl"))
		{
			ResultProperty = Prop.Value;
		}
		else if (Prop.Name == TEXT("Opr1"))
		{
			LeftOperand = Prop.Value;
		}
		else if (Prop.Name == TEXT("Opr2"))
		{
			RightOperand = Prop.Value;
		}
		else if (Prop.Name == TEXT("Operator"))
		{
			if (Prop.Value == TEXT("Add")) Operator = EBehaviorUOperatorType::Add;
			else if (Prop.Value == TEXT("Sub")) Operator = EBehaviorUOperatorType::Subtract;
			else if (Prop.Value == TEXT("Mul")) Operator = EBehaviorUOperatorType::Multiply;
			else if (Prop.Value == TEXT("Div")) Operator = EBehaviorUOperatorType::Divide;
		}
	}
}

EBehaviorUStatus UBehaviorUComputeTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	const UBehaviorUCompute* ComputeNode = Cast<UBehaviorUCompute>(Node);
	if (!ComputeNode || !Agent)
	{
		return EBehaviorUStatus::Failure;
	}

	FString LeftStr = ComputeNode->LeftOperand;
	FString RightStr = ComputeNode->RightOperand;

	if (LeftStr.StartsWith(TEXT("Self.")))
	{
		LeftStr = Agent->GetPropertyValue(LeftStr);
	}
	if (RightStr.StartsWith(TEXT("Self.")))
	{
		RightStr = Agent->GetPropertyValue(RightStr);
	}

	double Left = FCString::Atod(*LeftStr);
	double Right = FCString::Atod(*RightStr);
	double Result = 0.0;

	switch (ComputeNode->Operator)
	{
	case EBehaviorUOperatorType::Add:		Result = Left + Right; break;
	case EBehaviorUOperatorType::Subtract:	Result = Left - Right; break;
	case EBehaviorUOperatorType::Multiply:	Result = Left * Right; break;
	case EBehaviorUOperatorType::Divide:		Result = (Right != 0.0) ? Left / Right : 0.0; break;
	default: break;
	}

	Agent->SetPropertyValue(ComputeNode->ResultProperty, FString::SanitizeFloat(Result));
	return EBehaviorUStatus::Success;
}

// ===================================================================
// NOOP
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUNoop::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUNoopTask>(Outer);
}

EBehaviorUStatus UBehaviorUNoopTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	return EBehaviorUStatus::Success;
}

// ===================================================================
// END
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUEnd::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUEndTask>(Outer);
}

void UBehaviorUEnd::LoadFromProperties(int32 Version, const FString& InAgentType, const TArray<FBehaviorUProperty>& Properties)
{
	Super::LoadFromProperties(Version, InAgentType, Properties);
	EndStatus = EBehaviorUStatus::Success;
	bEndOutermost = false;

	for (const FBehaviorUProperty& Prop : Properties)
	{
		if (Prop.Name == TEXT("EndStatus"))
		{
			if (Prop.Value == TEXT("BT_SUCCESS")) EndStatus = EBehaviorUStatus::Success;
			else if (Prop.Value == TEXT("BT_FAILURE")) EndStatus = EBehaviorUStatus::Failure;
		}
		else if (Prop.Name == TEXT("EndOutermost"))
		{
			bEndOutermost = (Prop.Value == TEXT("true"));
		}
	}
}

EBehaviorUStatus UBehaviorUEndTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	const UBehaviorUEnd* EndNode = Cast<UBehaviorUEnd>(Node);
	return EndNode ? EndNode->EndStatus : EBehaviorUStatus::Success;
}

// ===================================================================
// WAIT
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUWait::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUWaitTask>(Outer);
}

void UBehaviorUWait::LoadFromProperties(int32 Version, const FString& InAgentType, const TArray<FBehaviorUProperty>& Properties)
{
	Super::LoadFromProperties(Version, InAgentType, Properties);
	Duration = 1.0f;

	for (const FBehaviorUProperty& Prop : Properties)
	{
		if (Prop.Name == TEXT("Time"))
		{
			Duration = FCString::Atof(*Prop.Value);
		}
	}
}

UBehaviorUWaitTask::UBehaviorUWaitTask()
	: StartTime(0.0)
	, WaitDuration(0.0f)
{
}

bool UBehaviorUWaitTask::OnEnter(UBehaviorUAgentComponent* Agent)
{
	const UBehaviorUWait* WaitNode = Cast<UBehaviorUWait>(Node);
	WaitDuration = WaitNode ? WaitNode->Duration : 1.0f;

	if (UWorld* World = Agent ? Agent->GetWorld() : nullptr)
	{
		StartTime = World->GetTimeSeconds();
	}
	else
	{
		StartTime = FPlatformTime::Seconds();
	}

	return true;
}

EBehaviorUStatus UBehaviorUWaitTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	double CurrentTime = 0.0;

	if (UWorld* World = Agent ? Agent->GetWorld() : nullptr)
	{
		CurrentTime = World->GetTimeSeconds();
	}
	else
	{
		CurrentTime = FPlatformTime::Seconds();
	}

	if ((CurrentTime - StartTime) >= WaitDuration)
	{
		return EBehaviorUStatus::Success;
	}

	return EBehaviorUStatus::Running;
}

// ===================================================================
// WAIT FRAMES
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUWaitFrames::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUWaitFramesTask>(Outer);
}

void UBehaviorUWaitFrames::LoadFromProperties(int32 Version, const FString& InAgentType, const TArray<FBehaviorUProperty>& Properties)
{
	Super::LoadFromProperties(Version, InAgentType, Properties);
	FrameCount = 1;

	for (const FBehaviorUProperty& Prop : Properties)
	{
		if (Prop.Name == TEXT("Frames"))
		{
			FrameCount = FCString::Atoi(*Prop.Value);
		}
	}
}

UBehaviorUWaitFramesTask::UBehaviorUWaitFramesTask()
	: StartFrame(0)
	, TargetFrames(0)
{
}

bool UBehaviorUWaitFramesTask::OnEnter(UBehaviorUAgentComponent* Agent)
{
	const UBehaviorUWaitFrames* WFNode = Cast<UBehaviorUWaitFrames>(Node);
	TargetFrames = WFNode ? WFNode->FrameCount : 1;
	StartFrame = GFrameCounter;
	return true;
}

EBehaviorUStatus UBehaviorUWaitFramesTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	int32 Elapsed = static_cast<int32>(GFrameCounter - StartFrame);
	if (Elapsed >= TargetFrames)
	{
		return EBehaviorUStatus::Success;
	}

	return EBehaviorUStatus::Running;
}

// ===================================================================
// WAIT FOR SIGNAL
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUWaitForSignal::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUWaitForSignalTask>(Outer);
}

void UBehaviorUWaitForSignal::LoadFromProperties(int32 Version, const FString& InAgentType, const TArray<FBehaviorUProperty>& Properties)
{
	Super::LoadFromProperties(Version, InAgentType, Properties);

	for (const FBehaviorUProperty& Prop : Properties)
	{
		if (Prop.Name == TEXT("Signal"))
		{
			SignalName = Prop.Value;
		}
	}
}

EBehaviorUStatus UBehaviorUWaitForSignalTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	const UBehaviorUWaitForSignal* WFSNode = Cast<UBehaviorUWaitForSignal>(Node);
	if (!WFSNode || !Agent)
	{
		return EBehaviorUStatus::Failure;
	}

	if (Agent->IsSignalSet(WFSNode->SignalName))
	{
		return EBehaviorUStatus::Success;
	}

	return EBehaviorUStatus::Running;
}
