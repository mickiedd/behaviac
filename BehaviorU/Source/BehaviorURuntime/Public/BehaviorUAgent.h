// BehaviorU UE5 Plugin
// Licensed under the BSD 3-Clause License.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BehaviorUTypes.h"
#include "BehaviorUAgent.generated.h"

class UBehaviorUBehaviorTree;
class UBehaviorUBehaviorTreeTask;
class UBehaviorUBehaviorNode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBehaviorUMethodDelegate, const FString&, MethodName, EBehaviorUStatus&, OutResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBehaviorUSignalDelegate, const FString&, SignalName);

/**
 * UBehaviorUAgentComponent: The central AI agent component for Unreal Engine 5.
 *
 * Attach this to any Actor to give it behavior tree / FSM / HTN capabilities.
 * This replaces the original behavioru Agent class, integrated as a UActorComponent.
 *
 * Features:
 * - Load and execute behavior trees by asset path
 * - Property system (blackboard-like key/value store)
 * - Method binding via delegates and Blueprint events
 * - Signal system for WaitForSignal nodes
 * - Multiple behavior tree support (stack)
 */
UCLASS(ClassGroup = (AI), meta = (BlueprintSpawnableComponent), DisplayName = "BehaviorU Agent")
class BEHAVIORURUNTIME_API UBehaviorUAgentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBehaviorUAgentComponent();

	// --- Lifecycle ---

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// --- Behavior Tree Management ---

	/** Load and start a behavior tree from an asset */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Agent")
	bool LoadBehaviorTree(UBehaviorUBehaviorTree* TreeAsset);

	/** Load a behavior tree by relative path (for XML/BSON loading) */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Agent")
	bool LoadBehaviorTreeByPath(const FString& RelativePath);

	/** Execute one tick of the current behavior tree */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Agent")
	EBehaviorUStatus TickBehaviorTree();

	/** Stop and unload the current behavior tree */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Agent")
	void StopBehaviorTree();

	/** Reset the current behavior tree to its initial state */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Agent")
	void ResetBehaviorTree();

	/** Get the current behavior tree status */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Agent")
	EBehaviorUStatus GetBehaviorTreeStatus() const;

	/** Whether automatic ticking is enabled */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorU|Agent")
	bool bAutoTick;

	/** The default behavior tree to load on BeginPlay */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorU|Agent")
	UBehaviorUBehaviorTree* DefaultBehaviorTree;

	// --- Property System (Blackboard) ---

	/** Set a property value by name */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Properties")
	void SetPropertyValue(const FString& PropertyName, const FString& Value);

	/** Get a property value by name */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Properties")
	FString GetPropertyValue(const FString& PropertyName) const;

	/** Check if a property exists */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Properties")
	bool HasProperty(const FString& PropertyName) const;

	/** Set an integer property */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Properties")
	void SetIntProperty(const FString& PropertyName, int32 Value);

	/** Get an integer property */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Properties")
	int32 GetIntProperty(const FString& PropertyName) const;

	/** Set a float property */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Properties")
	void SetFloatProperty(const FString& PropertyName, float Value);

	/** Get a float property */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Properties")
	float GetFloatProperty(const FString& PropertyName) const;

	/** Set a boolean property */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Properties")
	void SetBoolProperty(const FString& PropertyName, bool Value);

	/** Get a boolean property */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Properties")
	bool GetBoolProperty(const FString& PropertyName) const;

	// --- Method System ---

	/** Execute a named method on this agent. Override in Blueprints or bind delegates. */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Methods")
	EBehaviorUStatus ExecuteMethod(const FString& MethodName);

	/** Blueprint event called when a method needs to be executed */
	UFUNCTION(BlueprintImplementableEvent, Category = "BehaviorU|Methods")
	EBehaviorUStatus OnExecuteMethod(const FString& MethodName);

	/** Delegate for binding method implementations from C++ */
	UPROPERTY(BlueprintAssignable, Category = "BehaviorU|Methods")
	FBehaviorUMethodDelegate OnMethodCalled;

	/** Register a method handler (C++ callback) */
	void RegisterMethodHandler(const FString& MethodName, TFunction<EBehaviorUStatus()> Handler);

	// --- Signal System ---

	/** Send a signal to this agent */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Signals")
	void SendSignal(const FString& SignalName);

	/** Check if a signal has been set */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Signals")
	bool IsSignalSet(const FString& SignalName) const;

	/** Clear a signal */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Signals")
	void ClearSignal(const FString& SignalName);

	/** Clear all signals */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Signals")
	void ClearAllSignals();

	/** Delegate fired when a signal is received */
	UPROPERTY(BlueprintAssignable, Category = "BehaviorU|Signals")
	FBehaviorUSignalDelegate OnSignalReceived;

	// --- Event System ---

	/** Fire a named event on this agent (for event attachments) */
	UFUNCTION(BlueprintCallable, Category = "BehaviorU|Events")
	void FireEvent(const FString& EventName);

	/** Check if an event has been triggered */
	bool HasPendingEvent(const FString& EventName) const;

	/** Consume a pending event */
	void ConsumeEvent(const FString& EventName);

protected:
	/** Property storage (blackboard) */
	UPROPERTY()
	TMap<FString, FString> Properties;

	/** Active signals */
	UPROPERTY()
	TSet<FString> ActiveSignals;

	/** Pending events */
	UPROPERTY()
	TSet<FString> PendingEvents;

	/** Current behavior tree task (runtime execution) */
	UPROPERTY()
	UBehaviorUBehaviorTreeTask* CurrentTreeTask;

	/** Loaded behavior tree definition */
	UPROPERTY()
	UBehaviorUBehaviorTree* CurrentTreeAsset;

	/** Registered C++ method handlers */
	TMap<FString, TFunction<EBehaviorUStatus()>> MethodHandlers;

	/** Critical section for thread safety */
	mutable FCriticalSection PropertyLock;
};
