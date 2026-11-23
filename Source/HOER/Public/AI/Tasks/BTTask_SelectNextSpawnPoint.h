// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_SelectNextSpawnPoint.generated.h"

class AAIEnemyController;

/**
 * Behavior Tree task node requesting the Corridor Manager to choose a valid spawn point in a corridor
 * separate from the player's position, storing the result in the selected Blackboard key.
 */
UCLASS()
class HOER_API UBTTask_SelectNextSpawnPoint : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTTask_SelectNextSpawnPoint();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:

	AAIEnemyController* GetAIEnemyController(const UWorld* World);
};
