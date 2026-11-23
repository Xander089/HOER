// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_FindRandomPatrolLocation.h"

#include "AIController.h"
#include "AI/Controller/AIEnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindRandomPatrolLocation::UBTTask_FindRandomPatrolLocation()
{
	NodeName = "Find Random Location";
	
	// Filter only Vector keys in the editor dropdown
	RandomLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindRandomPatrolLocation, RandomLocationKey));
}

EBTNodeResult::Type UBTTask_FindRandomPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Check in editor BB Key has been selected
	if (RandomLocationKey.SelectedKeyName.IsNone())
	{
		return EBTNodeResult::Failed;
	}
	
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!IsValid(AIController))
	{
		return EBTNodeResult::Failed;
	}
	
	AAIEnemyController* AIEnemyController = Cast<AAIEnemyController>(AIController);
	
	if (!IsValid(AIEnemyController))
	{
		return EBTNodeResult::Failed;
	}
	
	FVector PatrolLocation = FVector::ZeroVector;
	
	AIEnemyController->FindReachablePoint(PatrolLocation);
	
	if (PatrolLocation.IsZero())
	{
		return EBTNodeResult::Failed;
	}
	
	if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
	{
		BlackboardComponent->SetValueAsVector(RandomLocationKey.SelectedKeyName, PatrolLocation);
	}
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
