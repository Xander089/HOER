// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ReactToRiskAwareness.generated.h"

/**
 * Behavior Tree task node adjusting AI movement speed, vision, and light-switch aura according to risk awareness.
 */
UCLASS()
class HOER_API UBTTask_ReactToRiskAwareness : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_ReactToRiskAwareness();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/** Blackboard key selector reading the current risk awareness float value. */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector RiskAwarenessKey;
};
