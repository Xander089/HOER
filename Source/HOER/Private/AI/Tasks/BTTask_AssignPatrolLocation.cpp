// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_AssignPatrolLocation.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Controller/AIDirectorController.h"

UBTTask_AssignPatrolLocation::UBTTask_AssignPatrolLocation()
{
	NodeName = "Assign New Patrol Location";
}

EBTNodeResult::Type UBTTask_AssignPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	// Get a reference of the Director Controller and call the method to assign the AI enemy a new random patrol location
	AAIController* AIController = OwnerComp.GetAIOwner();
	
	if (!IsValid(AIController))
	{
		return EBTNodeResult::Failed;
	}

	AAIDirectorController* AIDirectorController = Cast<AAIDirectorController>(AIController);
	
	if (!IsValid(AIDirectorController))
	{
		return EBTNodeResult::Failed;
	}
	
	AIDirectorController->AssignNewRandomLocation();
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}


