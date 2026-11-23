// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_HandlePlayerChased.generated.h"

/**
 * Behavior Tree task node executed when the AI enemy begins or maintains active pursuit of the player,
 * driving audio tension and music cues.
 */
UCLASS()
class HOER_API UBTTask_HandlePlayerChased : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HandlePlayerChased();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
