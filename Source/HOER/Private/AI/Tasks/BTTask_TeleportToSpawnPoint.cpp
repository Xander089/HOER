// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_TeleportToSpawnPoint.h"

#include "AI/Controller/AIEnemyController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Gameplay/Corridors/CorridorSpawnPoint.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

UBTTask_TeleportToSpawnPoint::UBTTask_TeleportToSpawnPoint()
{
	NodeName = "Teleport To Spawn Point";
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_TeleportToSpawnPoint, BlackboardKey));

}

EBTNodeResult::Type UBTTask_TeleportToSpawnPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!IsValid(BlackboardComponent)) { return EBTNodeResult::Failed; }

	const AAIEnemyController* AIEnemyController = Cast<AAIEnemyController>(OwnerComp.GetAIOwner());
	if (!IsValid(AIEnemyController)) { return EBTNodeResult::Failed; }

	APawn* Pawn = AIEnemyController->GetPawn();
	if (!IsValid(Pawn)) { return EBTNodeResult::Failed; }

	
	if (!Pawn->TeleportTo(BlackboardComponent->GetValueAsVector(GetSelectedBlackboardKey()), Pawn->GetActorRotation()))
	{
		return EBTNodeResult::Failed;
	}

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
