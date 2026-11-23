// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_TeleportToSpawnPoint.generated.h"

/**
 * Behavior Tree task node teleporting the enemy NPC pawn to the corridor spawn point stored in the selected Blackboard key.
 */
UCLASS()
class HOER_API UBTTask_TeleportToSpawnPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:

	UBTTask_TeleportToSpawnPoint();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
