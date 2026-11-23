// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_HandlePlayerChased.h"

#include "System/HOERGameMode.h"

UBTTask_HandlePlayerChased::UBTTask_HandlePlayerChased()
{
	NodeName = "Handle Player Chased";
}

EBTNodeResult::Type UBTTask_HandlePlayerChased::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UWorld* OwnerWorld = OwnerComp.GetWorld();
	if (!IsValid(OwnerWorld))
	{
		return EBTNodeResult::Failed;
	}

	APlayerController* PC =  OwnerWorld->GetFirstPlayerController();
	if (!IsValid(PC))
	{
		return EBTNodeResult::Failed;
	}
	
	if (AHOERGameMode* GM = GetWorld()->GetAuthGameMode<AHOERGameMode>(); IsValid(GM))
	{
		GM->TriggerGameOver(PC);
	}
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
