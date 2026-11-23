// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AssignPatrolLocation.generated.h"

/**
 * Behavior Tree task node executed by the AI Director to calculate and assign an optimal patrol/search location for the enemy.
 */
UCLASS()
class HOER_API UBTTask_AssignPatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_AssignPatrolLocation();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
