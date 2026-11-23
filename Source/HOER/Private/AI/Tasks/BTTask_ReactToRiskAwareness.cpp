// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/BTTask_ReactToRiskAwareness.h"
#include "AIController.h"
#include "AI/Character/AICharacter.h"
#include "AI/Controller/AIEnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

UBTTask_ReactToRiskAwareness::UBTTask_ReactToRiskAwareness()
{
	NodeName = "React To Risk Awareness";
	
	// Filter only float keys in the editor dropdown
	RiskAwarenessKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ReactToRiskAwareness, RiskAwarenessKey));
}

EBTNodeResult::Type UBTTask_ReactToRiskAwareness::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	// Check in editor BB Key has been selected
	if (RiskAwarenessKey.SelectedKeyName.IsNone())
	{
		return EBTNodeResult::Failed;
	}
	
	// Get a reference of the Director Controller and call the method to assign the AI enemy a new random patrol location
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
	
	ACharacter* Character = AIEnemyController->GetCharacter();
	
	if (!IsValid(Character))
	{
		return EBTNodeResult::Failed;
	}

	AAICharacter* AICharacter = Cast<AAICharacter>(Character);
	
	if (!IsValid(AICharacter))
	{
		return EBTNodeResult::Failed;
	}
	
	if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
	{
		const float CurrentRisk = BlackboardComponent->GetValueAsFloat(RiskAwarenessKey.SelectedKeyName);
		AIEnemyController->ReactToRiskAwarenessChange(CurrentRisk);
		AICharacter->ReactToRiskAwarenessChange(CurrentRisk);
		
		BlackboardComponent->SetValueAsFloat(RiskAwarenessKey.SelectedKeyName, 0.0f);

	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
