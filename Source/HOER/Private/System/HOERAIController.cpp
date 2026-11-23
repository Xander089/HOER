// Fill out your copyright notice in the Description page of Project Settings.


#include "System/HOERAIController.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"

AHOERAIController::AHOERAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	
	BrainComponent = BehaviorTreeComponent;
}

void AHOERAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (!IsValid(BehaviorTreeAsset))
	{
		return;
	}
	
	if (!IsValid(BehaviorTreeAsset->GetBlackboardAsset()))
	{
		return;
	}

	BlackboardComponent->InitializeBlackboard(*BehaviorTreeAsset->GetBlackboardAsset());
	

}

void AHOERAIController::InitializeBlackboardValues()
{
}

bool AHOERAIController::ValidateLocation(FVector& InCandidateLocation, const float BoxExtent) const
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	
	if (!IsValid(NavSys)){ return false; }
	
	FNavLocation NavLocation;

	if (NavSys->ProjectPointToNavigation(
			InCandidateLocation,
			NavLocation,
			FVector(BoxExtent, BoxExtent, BoxExtent)))
	{
		// Candidate is on/near the navmesh.
		InCandidateLocation = NavLocation.Location;
			
		return true;
	}
	
	return false;
}

void AHOERAIController::ToggleControl(const bool bShouldPause) const
{
	UBrainComponent* Brain = GetBrainComponent();
	if (!IsValid(Brain)){ return; }

	if (bShouldPause)
	{
		Brain->PauseLogic(TEXT("ManualPause"));
	}
	else
	{
		Brain->ResumeLogic(TEXT("ManualResume"));
	}
}

void AHOERAIController::StopControl() const
{
	if (UBrainComponent* Brain = GetBrainComponent(); IsValid(BrainComponent))
	{
		Brain->StopLogic(TEXT("Elimination"));
	}
}

