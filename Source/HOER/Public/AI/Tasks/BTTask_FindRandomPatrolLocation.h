// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindRandomPatrolLocation.generated.h"

/**
 * Behavior Tree task node sampling a reachable random point on the navigation mesh around the AI.
 */
UCLASS()
class HOER_API UBTTask_FindRandomPatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_FindRandomPatrolLocation();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	/** Blackboard key selector where the random navigable location will be written. */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector RandomLocationKey;
};
