// BehaviorU UE5 Plugin
// Licensed under the BSD 3-Clause License.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "BehaviorUTypes.generated.h"

// Logging category
BEHAVIORURUNTIME_API DECLARE_LOG_CATEGORY_EXTERN(LogBehaviorU, Log, All);

/** Return values of node execution and valid states for behaviors. */
UENUM(BlueprintType)
enum class EBehaviorUStatus : uint8
{
	Invalid		UMETA(DisplayName = "Invalid"),
	Success		UMETA(DisplayName = "Success"),
	Failure		UMETA(DisplayName = "Failure"),
	Running		UMETA(DisplayName = "Running"),
};

/** Action result for preconditions/effectors. */
UENUM(BlueprintType)
enum class EBehaviorUActionResult : uint8
{
	Success		UMETA(DisplayName = "Success"),
	Failure		UMETA(DisplayName = "Failure"),
	All			UMETA(DisplayName = "All"),
};

/** Operator types used in conditions and computations. */
UENUM(BlueprintType)
enum class EBehaviorUOperatorType : uint8
{
	Invalid,
	Assign,			// =
	Add,			// +
	Subtract,		// -
	Multiply,		// *
	Divide,			// /
	Equal,			// ==
	NotEqual,		// !=
	Greater,		// >
	Less,			// <
	GreaterEqual,	// >=
	LessEqual,		// <=
};

/** Parallel node policy for child completion. */
UENUM(BlueprintType)
enum class EBehaviorUParallelPolicy : uint8
{
	/** Succeed when all children succeed, fail when any fails */
	FailOnOne_SucceedOnAll		UMETA(DisplayName = "Fail On One, Succeed On All"),
	/** Succeed when any child succeeds, fail when all fail */
	FailOnAll_SucceedOnOne		UMETA(DisplayName = "Fail On All, Succeed On One"),
	/** Succeed when any child succeeds, fail when any fails */
	FailOnOne_SucceedOnOne		UMETA(DisplayName = "Fail On One, Succeed On One"),
};

/** Parallel node exit policy for child that completed early. */
UENUM(BlueprintType)
enum class EBehaviorUChildFinishPolicy : uint8
{
	/** Keep ticking completed children */
	Loop		UMETA(DisplayName = "Loop"),
	/** Stop ticking completed children */
	Once		UMETA(DisplayName = "Once"),
};

/** Precondition phase. */
UENUM(BlueprintType)
enum class EBehaviorUPreconditionPhase : uint8
{
	Enter,
	Update,
	Both,
};

/** Effector phase. */
UENUM(BlueprintType)
enum class EBehaviorUEffectorPhase : uint8
{
	Success,
	Failure,
	Both,
};

/** File format for behavior tree data. */
UENUM(BlueprintType)
enum class EBehaviorUFileFormat : uint8
{
	XML,
	BSON,
};

/** Invalid node ID constant. */
static constexpr int32 BEHAVIORU_INVALID_NODE_ID = -2;

/** Property container used during node loading. */
USTRUCT(BlueprintType)
struct BEHAVIORURUNTIME_API FBehaviorUProperty
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorU")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorU")
	FString Value;

	FBehaviorUProperty() {}
	FBehaviorUProperty(const FString& InName, const FString& InValue)
		: Name(InName), Value(InValue)
	{}
};
