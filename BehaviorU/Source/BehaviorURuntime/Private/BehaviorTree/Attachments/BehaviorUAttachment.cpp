// BehaviorU UE5 Plugin
// Licensed under the BSD 3-Clause License.

#include "BehaviorTree/Attachments/BehaviorUAttachment.h"
#include "BehaviorUAgent.h"

// ===================================================================
// UBehaviorUAttachment
// ===================================================================

UBehaviorUAttachment::UBehaviorUAttachment()
	: PreconditionPhase(EBehaviorUPreconditionPhase::Both)
	, EffectorPhase(EBehaviorUEffectorPhase::Both)
	, ActionResult(EBehaviorUActionResult::All)
	, bNegate(false)
{
}

void UBehaviorUAttachment::LoadFromProperties(int32 Version, const FString& AgentType, const TArray<FBehaviorUProperty>& Properties)
{
	for (const FBehaviorUProperty& Prop : Properties)
	{
		if (Prop.Name == TEXT("Phase"))
		{
			if (Prop.Value == TEXT("Enter"))
				PreconditionPhase = EBehaviorUPreconditionPhase::Enter;
			else if (Prop.Value == TEXT("Update"))
				PreconditionPhase = EBehaviorUPreconditionPhase::Update;
			else
				PreconditionPhase = EBehaviorUPreconditionPhase::Both;
		}
		else if (Prop.Name == TEXT("Negate"))
		{
			bNegate = (Prop.Value == TEXT("true"));
		}
	}
}

bool UBehaviorUAttachment::Evaluate(UBehaviorUAgentComponent* Agent) const
{
	return true;
}

void UBehaviorUAttachment::Apply(UBehaviorUAgentComponent* Agent, bool bSuccess) const
{
}

bool UBehaviorUAttachment::AppliesToPhase(EBehaviorUPreconditionPhase Phase) const
{
	return PreconditionPhase == EBehaviorUPreconditionPhase::Both || PreconditionPhase == Phase;
}

// ===================================================================
// UBehaviorUPrecondition
// ===================================================================

UBehaviorUPrecondition::UBehaviorUPrecondition()
	: Operator(EBehaviorUOperatorType::Equal)
{
}

bool UBehaviorUPrecondition::AppliesToPhase(EBehaviorUPreconditionPhase Phase) const
{
	return PreconditionPhase == EBehaviorUPreconditionPhase::Both || PreconditionPhase == Phase;
}

bool UBehaviorUPrecondition::Evaluate(UBehaviorUAgentComponent* Agent) const
{
	if (!Agent)
	{
		return false;
	}

	// Get left and right values from agent properties
	FString LeftValue = Agent->GetPropertyValue(LeftOperand);
	FString RightValue = RightOperand;

	// If right operand references a property, resolve it
	if (RightOperand.StartsWith(TEXT("Self.")))
	{
		RightValue = Agent->GetPropertyValue(RightOperand);
	}

	bool bResult = false;

	// Try numeric comparison first
	bool bIsNumeric = LeftValue.IsNumeric() && RightValue.IsNumeric();

	if (bIsNumeric)
	{
		double Left = FCString::Atod(*LeftValue);
		double Right = FCString::Atod(*RightValue);

		switch (Operator)
		{
		case EBehaviorUOperatorType::Equal:			bResult = FMath::IsNearlyEqual(Left, Right); break;
		case EBehaviorUOperatorType::NotEqual:		bResult = !FMath::IsNearlyEqual(Left, Right); break;
		case EBehaviorUOperatorType::Greater:			bResult = Left > Right; break;
		case EBehaviorUOperatorType::Less:			bResult = Left < Right; break;
		case EBehaviorUOperatorType::GreaterEqual:	bResult = Left >= Right; break;
		case EBehaviorUOperatorType::LessEqual:		bResult = Left <= Right; break;
		default: break;
		}
	}
	else
	{
		// String comparison
		int32 Cmp = LeftValue.Compare(RightValue);
		switch (Operator)
		{
		case EBehaviorUOperatorType::Equal:			bResult = (Cmp == 0); break;
		case EBehaviorUOperatorType::NotEqual:		bResult = (Cmp != 0); break;
		case EBehaviorUOperatorType::Greater:			bResult = (Cmp > 0); break;
		case EBehaviorUOperatorType::Less:			bResult = (Cmp < 0); break;
		case EBehaviorUOperatorType::GreaterEqual:	bResult = (Cmp >= 0); break;
		case EBehaviorUOperatorType::LessEqual:		bResult = (Cmp <= 0); break;
		default: break;
		}
	}

	return bNegate ? !bResult : bResult;
}

// ===================================================================
// UBehaviorUEffector
// ===================================================================

UBehaviorUEffector::UBehaviorUEffector()
{
}

void UBehaviorUEffector::Apply(UBehaviorUAgentComponent* Agent, bool bSuccess) const
{
	if (!Agent)
	{
		return;
	}

	bool bShouldApply = false;

	switch (EffectorPhase)
	{
	case EBehaviorUEffectorPhase::Success:	bShouldApply = bSuccess; break;
	case EBehaviorUEffectorPhase::Failure:	bShouldApply = !bSuccess; break;
	case EBehaviorUEffectorPhase::Both:		bShouldApply = true; break;
	}

	if (bShouldApply && !PropertyName.IsEmpty())
	{
		Agent->SetPropertyValue(PropertyName, PropertyValue);
	}
}

// ===================================================================
// UBehaviorUEventAttachment
// ===================================================================

UBehaviorUEventAttachment::UBehaviorUEventAttachment()
	: bTriggeredOnce(true)
{
}
