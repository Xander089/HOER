// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_ClearSelectedKey.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ClearSelectedKey::UBTTask_ClearSelectedKey()
{
	NodeName = "Clear Selected Blackboard Key";
}

EBTNodeResult::Type UBTTask_ClearSelectedKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	 UBlackboardComponent* BlackboardComponent =  OwnerComp.GetBlackboardComponent();
	
	if (!IsValid(BlackboardComponent))
	{
		return EBTNodeResult::Failed;
	}
	
	BlackboardComponent->ClearValue(GetSelectedBlackboardKey());
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
