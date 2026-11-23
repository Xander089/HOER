// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_SelectNextSpawnPoint.h"

#include "AI/AIKeys.h"
#include "AI/Character/AICharacter.h"
#include "AI/Controller/AIEnemyController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Gameplay/Corridors/CorridorSpawnPoint.h"
#include "Kismet/GameplayStatics.h"
#include "System/Subsystem/CorridorManagerSubsystem.h"

UBTTask_SelectNextSpawnPoint::UBTTask_SelectNextSpawnPoint()
{
	NodeName = "Select Next Spawn Point";
}

EBTNodeResult::Type UBTTask_SelectNextSpawnPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!IsValid(BlackboardComponent))
	{
		return EBTNodeResult::Failed;
	}

	AAIEnemyController* AIEnemyController = GetAIEnemyController(OwnerComp.GetWorld());
	if (!IsValid(AIEnemyController))
	{
		return EBTNodeResult::Failed;
	}

	const UCorridorManagerSubsystem* CorridorManager = OwnerComp.GetWorld()->GetSubsystem<UCorridorManagerSubsystem>();
	if (!IsValid(CorridorManager))
	{
		return EBTNodeResult::Failed;
	}

	// Get the current player corridor from the blackboard, if set
	ACorridorSpawnPoint* CurrentPlayerCorridor = nullptr;
	if (UObject* CurrentPlayerCorridorObject = BlackboardComponent->GetValueAsObject(AIBlackboardKeys::CurrentPlayerCorridor); IsValid(CurrentPlayerCorridorObject))
	{
		CurrentPlayerCorridor = Cast<ACorridorSpawnPoint>(CurrentPlayerCorridorObject);
	}
	
	ACorridorSpawnPoint* SpawnPoint = CorridorManager->SelectNextSpawnPoint(CurrentPlayerCorridor);
	if (!IsValid(SpawnPoint))
	{
		return EBTNodeResult::Failed;
	}

	AIEnemyController->SetNextSpawnLocation(SpawnPoint->GetActorLocation());
	AIEnemyController->SetNextWaypointLocation(SpawnPoint->GetWaypointVector());

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

AAIEnemyController* UBTTask_SelectNextSpawnPoint::GetAIEnemyController(const UWorld* World)
{
	
	if (!IsValid(World))
	{
		return nullptr;
	}
	
	const AActor* AIEnemyActor = UGameplayStatics::GetActorOfClass(World, AAICharacter::StaticClass());
	
	if (!IsValid(AIEnemyActor))
	{
		return nullptr;
	}
	
	const AAICharacter* AICharacter = Cast<AAICharacter>(AIEnemyActor);

	if (!IsValid(AICharacter))
	{
		return nullptr;
	}

	if (AAIEnemyController* Controller = Cast<AAIEnemyController>(AICharacter->GetController()); IsValid(Controller))
	{
		return Controller;
	}
	
	
	return nullptr;
}
