// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_NotifyDirectorPatrolEnded.generated.h"

/**
 * Behavior Tree task node notifying the AI Director that the enemy NPC has concluded its current patrol routine.
 */
UCLASS()
class HOER_API UBTTask_NotifyDirectorPatrolEnded : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_NotifyDirectorPatrolEnded();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
