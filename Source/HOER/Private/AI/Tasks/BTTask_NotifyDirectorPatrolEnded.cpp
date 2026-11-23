// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_NotifyDirectorPatrolEnded.h"

#include "System/Subsystem/CorridorManagerSubsystem.h"


UBTTask_NotifyDirectorPatrolEnded::UBTTask_NotifyDirectorPatrolEnded()
{
	NodeName = "Notify Director Patrol Ended";
}

EBTNodeResult::Type UBTTask_NotifyDirectorPatrolEnded::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UCorridorManagerSubsystem* CorridorManager = OwnerComp.GetWorld()->GetSubsystem<UCorridorManagerSubsystem>();
	if (!IsValid(CorridorManager))
	{
		return EBTNodeResult::Failed;
	}

	CorridorManager->NotifyPatrolEnded();
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
