// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindNextWaypointLocation.generated.h"

/**
 * Behavior Tree task node retrieving the next sequential patrol waypoint coordinate and setting it to the target blackboard key.
 */
UCLASS()
class HOER_API UBTTask_FindNextWaypointLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_FindNextWaypointLocation();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	/** Blackboard key selector where the resolved waypoint location vector will be stored. */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector WaypointLocationKey;
};
