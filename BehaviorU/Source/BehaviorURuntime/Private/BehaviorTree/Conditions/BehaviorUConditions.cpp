// BehaviorU UE5 Plugin
// Licensed under the BSD 3-Clause License.

#include "BehaviorTree/Conditions/BehaviorUConditions.h"
#include "BehaviorUAgent.h"

// ===================================================================
// CONDITION
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUCondition::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUConditionTask>(Outer);
}

void UBehaviorUCondition::LoadFromProperties(int32 Version, const FString& InAgentType, const TArray<FBehaviorUProperty>& Properties)
{
	Super::LoadFromProperties(Version, InAgentType, Properties);
	Operator = EBehaviorUOperatorType::Equal;

	for (const FBehaviorUProperty& Prop : Properties)
	{
		if (Prop.Name == TEXT("Opl"))
		{
			LeftOperand = Prop.Value;
		}
		else if (Prop.Name == TEXT("Opr"))
		{
			RightOperand = Prop.Value;
		}
		else if (Prop.Name == TEXT("Operator"))
		{
			if (Prop.Value == TEXT("Equal")) Operator = EBehaviorUOperatorType::Equal;
			else if (Prop.Value == TEXT("NotEqual")) Operator = EBehaviorUOperatorType::NotEqual;
			else if (Prop.Value == TEXT("Greater")) Operator = EBehaviorUOperatorType::Greater;
			else if (Prop.Value == TEXT("Less")) Operator = EBehaviorUOperatorType::Less;
			else if (Prop.Value == TEXT("GreaterEqual")) Operator = EBehaviorUOperatorType::GreaterEqual;
			else if (Prop.Value == TEXT("LessEqual")) Operator = EBehaviorUOperatorType::LessEqual;
		}
	}
}

EBehaviorUStatus UBehaviorUConditionTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	const UBehaviorUCondition* CondNode = Cast<UBehaviorUCondition>(Node);
	if (!CondNode || !Agent)
	{
		return EBehaviorUStatus::Failure;
	}

	FString LeftStr = CondNode->LeftOperand;
	FString RightStr = CondNode->RightOperand;

	// Resolve property references
	if (LeftStr.StartsWith(TEXT("Self.")))
	{
		LeftStr = Agent->GetPropertyValue(LeftStr);
	}
	if (RightStr.StartsWith(TEXT("Self.")))
	{
		RightStr = Agent->GetPropertyValue(RightStr);
	}

	return EvaluateComparison(LeftStr, RightStr, CondNode->Operator) ?
		EBehaviorUStatus::Success : EBehaviorUStatus::Failure;
}

bool UBehaviorUConditionTask::EvaluateComparison(const FString& Left, const FString& Right, EBehaviorUOperatorType Op) const
{
	// Try numeric comparison
	if (Left.IsNumeric() && Right.IsNumeric())
	{
		double LeftVal = FCString::Atod(*Left);
		double RightVal = FCString::Atod(*Right);

		switch (Op)
		{
		case EBehaviorUOperatorType::Equal:			return FMath::IsNearlyEqual(LeftVal, RightVal);
		case EBehaviorUOperatorType::NotEqual:		return !FMath::IsNearlyEqual(LeftVal, RightVal);
		case EBehaviorUOperatorType::Greater:			return LeftVal > RightVal;
		case EBehaviorUOperatorType::Less:			return LeftVal < RightVal;
		case EBehaviorUOperatorType::GreaterEqual:	return LeftVal >= RightVal;
		case EBehaviorUOperatorType::LessEqual:		return LeftVal <= RightVal;
		default: return false;
		}
	}

	// String comparison
	int32 Cmp = Left.Compare(Right);
	switch (Op)
	{
	case EBehaviorUOperatorType::Equal:			return Cmp == 0;
	case EBehaviorUOperatorType::NotEqual:		return Cmp != 0;
	case EBehaviorUOperatorType::Greater:			return Cmp > 0;
	case EBehaviorUOperatorType::Less:			return Cmp < 0;
	case EBehaviorUOperatorType::GreaterEqual:	return Cmp >= 0;
	case EBehaviorUOperatorType::LessEqual:		return Cmp <= 0;
	default: return false;
	}
}

// ===================================================================
// AND
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUAnd::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUAndTask>(Outer);
}

EBehaviorUStatus UBehaviorUAndTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	for (UBehaviorUBehaviorTask* Child : ChildTasks)
	{
		if (!Child) continue;

		EBehaviorUStatus Result = Child->Execute(Agent, EBehaviorUStatus::Invalid);
		if (Result != EBehaviorUStatus::Success)
		{
			return Result; // Failure or Running propagates
		}
	}
	return EBehaviorUStatus::Success;
}

// ===================================================================
// OR
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUOr::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUOrTask>(Outer);
}

EBehaviorUStatus UBehaviorUOrTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	for (UBehaviorUBehaviorTask* Child : ChildTasks)
	{
		if (!Child) continue;

		EBehaviorUStatus Result = Child->Execute(Agent, EBehaviorUStatus::Invalid);
		if (Result == EBehaviorUStatus::Success)
		{
			return EBehaviorUStatus::Success;
		}
		if (Result == EBehaviorUStatus::Running)
		{
			return EBehaviorUStatus::Running;
		}
	}
	return EBehaviorUStatus::Failure;
}

// ===================================================================
// TRUE / FALSE
// ===================================================================

UBehaviorUBehaviorTask* UBehaviorUTrue::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUTrueTask>(Outer);
}

EBehaviorUStatus UBehaviorUTrueTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	return EBehaviorUStatus::Success;
}

UBehaviorUBehaviorTask* UBehaviorUFalse::CreateTask(UObject* Outer) const
{
	return NewObject<UBehaviorUFalseTask>(Outer);
}

EBehaviorUStatus UBehaviorUFalseTask::OnUpdate(UBehaviorUAgentComponent* Agent, EBehaviorUStatus ChildStatus)
{
	return EBehaviorUStatus::Failure;
}
