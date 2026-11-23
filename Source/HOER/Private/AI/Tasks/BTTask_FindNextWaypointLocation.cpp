// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_FindNextWaypointLocation.h"

#include "AI/Controller/AIEnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindNextWaypointLocation::UBTTask_FindNextWaypointLocation()
{
	NodeName = "Find Next Patrol Location";
	
	// Filter only Vector keys in the editor dropdown
	WaypointLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindNextWaypointLocation, WaypointLocationKey));
}

EBTNodeResult::Type UBTTask_FindNextWaypointLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Check in editor BB Key has been selected
	if (WaypointLocationKey.SelectedKeyName.IsNone())
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
	
	/** Set the next waypoint, projected onto an equivalent valid navigable point  */
	AIEnemyController->GetNextWaypoint(PatrolLocation);
	
	if (PatrolLocation.IsZero())
	{
		return EBTNodeResult::Failed;
	}

	
	
	if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
	{
		BlackboardComponent->SetValueAsVector(WaypointLocationKey.SelectedKeyName, PatrolLocation);
	}
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
