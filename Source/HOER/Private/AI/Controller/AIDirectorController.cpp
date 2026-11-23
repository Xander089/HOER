// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controller/AIDirectorController.h"

#include "Data/AI/AIDirectorConfigDataAsset.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Controller/AIEnemyController.h"
#include "GameFramework/Character.h"
#include "Gameplay/LightControls/LightActor.h"
#include "System/Subsystem/LightManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "AI/Character/AICharacter.h"
#include "Misc/DataValidation.h"
#include "AI/AIKeys.h"
#include "Character/HOERCharacter.h"
#include "Gameplay/Corridors/CorridorSpawnPoint.h"
#include "System/Subsystem/CorridorManagerSubsystem.h"

void AAIDirectorController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn); // this handles the BT and BB initialization
	
	
	// Initialize Blackboard and Behavior Tree
	
	InitializeBlackboardValues();
	
	if (IsValid(BehaviorTreeAsset))
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
	
	
	SubscribeToLightManager();
}

void AAIDirectorController::BeginPlay()
{
	Super::BeginPlay();
	
	//Cache reference to AI Enemy Controller
	InitializeAIEnemyControllerReference();
	
	//Register to the Corridor Manager Subsystem
	if (const UWorld* World = GetWorld(); IsValid(World))
	{
		if (UCorridorManagerSubsystem* CorridorManager = World->GetSubsystem<UCorridorManagerSubsystem>(); IsValid(CorridorManager))
		{
			CorridorManager->RegisterAIDirectorController(this);
		}
	}

}

void AAIDirectorController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unregister from the Corridor Manager Subsystem
	if (const UWorld* World = GetWorld(); IsValid(World))
	{
		if (UCorridorManagerSubsystem* CorridorManager = World->GetSubsystem<UCorridorManagerSubsystem>(); IsValid(CorridorManager))
		{
			CorridorManager->UnregisterAIDirectorController(this);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void AAIDirectorController::InitializeBlackboardValues()
{
	if (IsValid(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsFloat(AIBlackboardKeys::NewAssignLocationCooldownTime, AIConfigDataAsset->ConfigData.NewAssignLocationCooldownTime);
		BlackboardComponent->SetValueAsBool(AIBlackboardKeys::NPCReachedWaypoint, true);

	}
	
	Super::InitializeBlackboardValues();
}

void AAIDirectorController::AssignNewRandomLocation()
{
	
	// in order to assign a new location, AI Enemy and Config Datashould be valid
	if (!AIEnemyController.IsValid() || !IsValid(AIConfigDataAsset))
	{
		return;
	}
	
	//Check Success Rate: new location is assigned only if a random number lies between 0 and the Success Rate defined Config Data
	if (const double SuccessRate = FMath::RandRange(0.0f, 1.0f); SuccessRate >= AIConfigDataAsset->ConfigData.AssignNewLocationSuccessRate)
	{
		return;
	}
	
	// 1. Get current player's location
	FVector PlayerLocation = FVector::ZeroVector;
	GetPlayerLocation(PlayerLocation);
	
	
	// 2. Get references to the latest lights turned on
	TArray<ALightActor*> LightActors;
	RetrieveLatestLightsTurnedOn(LightActors);
	
	if (LightActors.IsEmpty())
	{
		return;
	}
	
	// 3. calculate a new random location
	FVector LocationToAssign = CalculateNewRandomLocation(PlayerLocation, LightActors);
	
	// 4 Validate LocationToAssign against the Navigation System, so that it becomes an actual reachable location
	if (ValidateLocation(LocationToAssign, AIConfigDataAsset->ConfigData.ProjectPointToNavigationExtent))
	{
		// 5. If Location is valid, Share this location to the AI Enemy Controller
		AIEnemyController->SetAssignedLocation(LocationToAssign);
	}
	
	//Notify the AI Enemy brain with an up-to-date version of the Risk Awareness
	AIEnemyController->SetRiskAwareness(GetRiskAwareness());
	
	UpdateBlackboardShouldAssignNewLocation(false); //trigger sequence node re-evaluation in behavior tree
	
}

void AAIDirectorController::SetPlayerSpawnLocation(ACorridorSpawnPoint* CorridorSpawnPoint) const
{
	if (!IsValid(CorridorSpawnPoint))
	{
		return;
	}
	
	if (IsValid(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsObject(AIBlackboardKeys::CurrentPlayerCorridor, CorridorSpawnPoint);
	}
}

void AAIDirectorController::NotifyPatrolEnded() const
{
	if (IsValid(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsBool(AIBlackboardKeys::NPCReachedWaypoint, true);
	}
}

void AAIDirectorController::HandleLightStateChanged(const bool bOn)
{
	bLatestLightTurnedOnOrOff = bOn; // cache whether this event was a turn light on or off
	
	UpdateBlackboardShouldAssignNewLocation(true); //trigger sequence node re-evaluation in behavior tree
}

void AAIDirectorController::SubscribeToLightManager()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}
	
	LightManagerSubSystem = World->GetSubsystem<ULightManagerSubSystem>();
	
	if (!LightManagerSubSystem.IsValid())
	{
		return;
	}
	
	LightManagerSubSystem->OnLightStateChanged.AddDynamic(this, &AAIDirectorController::HandleLightStateChanged);
	LightManagerSubSystem->OnStartIlluminate.AddDynamic(this, &AAIDirectorController::HandleLightStartIlluminating);
	LightManagerSubSystem->OnStopIlluminate.AddDynamic(this, &AAIDirectorController::HandleLightStopIlluminating);
}

void AAIDirectorController::GetPlayerLocation(FVector& OutLocation) const
{
	const ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

	if (IsValid(PlayerCharacter))
	{
		OutLocation = PlayerCharacter->GetActorLocation();
	}
}

void AAIDirectorController::RetrieveLatestLightsTurnedOn(TArray<ALightActor*>& LightActors) const
{
	if (!LightManagerSubSystem.IsValid())
	{
		return;
	}
	
	if(!IsValid(AIConfigDataAsset))
	{
		return;
	}
	
	const int32 LightCountToRetrieve = AIConfigDataAsset->ConfigData.LightEventsShortTermMemoryCount;
	
	LightManagerSubSystem->GetLast_K_SwitchedOnLightsFromHistory(LightCountToRetrieve, LightActors);
}

float AAIDirectorController::GetRiskAwareness() const
{
	if (!LightManagerSubSystem.IsValid())
	{
		return 0.0f;
	}
	
	if(!IsValid(AIConfigDataAsset))
	{
		return 0.0f;
	}

	return LightManagerSubSystem->CalculateFitness(AIConfigDataAsset->ConfigData.AdjacentLightOnWeight);
}

FVector AAIDirectorController::CalculateNewRandomLocation(
	const FVector& PlayerLocation,
	const TArray<ALightActor*>& LightActors) const
{
	// check Light Manager validity
	if (!LightManagerSubSystem.IsValid())
	{
		return FVector::ZeroVector;
	}
	
	// check config validity
	if (!IsValid(AIConfigDataAsset))
	{
		return FVector::ZeroVector;
	}
	

	// check LightCount validity
	if (LightActors.Num() == 0)
	{
		return FVector::ZeroVector;
	}

	
	// Linear weighted average. The i-th weight is calculated as 2*i/N/N+1
	const double WeightFactor = 2.0f / static_cast<double>(LightActors.Num()) / (static_cast<double>(LightActors.Num()) + 1);
	
	FVector LinearWeightedLocation = FVector::ZeroVector;

	for (int32 i = 0; i < LightActors.Num(); i++)
	{
		const ALightActor* LightActor = LightActors[i];
		if (!IsValid(LightActor)){ continue;}
		
		LinearWeightedLocation+= WeightFactor * (i + 1) * LightActor->GetActorLocation();
	}
	
	
	const float AdjacentLightOnWeight = AIConfigDataAsset->ConfigData.AdjacentLightOnWeight;
	const float CurrentLevelCompletionPercentageWeight = LightManagerSubSystem->CalculateFitness(AdjacentLightOnWeight);
	
	// Given: LC = %LevelCompletion | WL = weighted location | PL = Player's location | K = weight to balance between WL and PL when LC is greater than 0.
	// The final location is calculated as: (1 - K * LC) * WL + (K * LC * PL).
	// Currently, K is 0.5f, but it can be later promoted to an Editable UPROPERTY according to future design requirements
	return (1 - 0.5f * CurrentLevelCompletionPercentageWeight) * LinearWeightedLocation + (PlayerLocation * 0.5f * CurrentLevelCompletionPercentageWeight);
}

void AAIDirectorController::UpdateBlackboardShouldAssignNewLocation(bool bShouldAssignNewLocation)
{
	// Notify the Director's blackboard that a new location should be calculated and assigned to the AI enemy controller
	UBlackboardComponent* DirectorBlackboardComponent = GetBlackboardComponent();
	
	if (!IsValid(DirectorBlackboardComponent))
	{
		return;
	}
	
	DirectorBlackboardComponent->SetValueAsBool(AIBlackboardKeys::ShouldAssignNewLocation, bShouldAssignNewLocation);
}

void AAIDirectorController::InitializeAIEnemyControllerReference()
{
	const UWorld* World = GetWorld();
	
	if (!IsValid(World))
	{
		return;
	}
	
	const AActor* AIEnemyActor = UGameplayStatics::GetActorOfClass(World, AAICharacter::StaticClass());
	
	if (!IsValid(AIEnemyActor))
	{
		return;
	}
	
	const AAICharacter* AICharacter = Cast<AAICharacter>(AIEnemyActor);

	if (!IsValid(AICharacter))
	{
		return;
	}
	
	AController* Controller = AICharacter->GetController();
	
	if (IsValid(Controller))
	{
		AIEnemyController = Cast<AAIEnemyController>(Controller);
	}
	
}

void AAIDirectorController::SetIgnoreDetection(const AActor* OtherActor, const bool bIgnore) const
{
	if (!IsValid(OtherActor)){ return; }
	
	const AHOERCharacter* CurrentPlayer = Cast<AHOERCharacter>(OtherActor);
	
	if (!IsValid(CurrentPlayer)){ return; }
	
	if (AIEnemyController.IsValid()){ return; }
	
	AIEnemyController->SetIgnoreDetection(bIgnore);
}
void AAIDirectorController::HandleLightStartIlluminating(AActor* OtherActor)
{
	SetIgnoreDetection(OtherActor, true);
}

void AAIDirectorController::HandleLightStopIlluminating(AActor* OtherActor)
{
	SetIgnoreDetection(OtherActor,false);
}

#if WITH_EDITOR
EDataValidationResult AAIDirectorController::IsDataValid(FDataValidationContext& Context) const
{
    // Call the Super implementation
    EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

    // check whether DataAsset is set
    if (AIConfigDataAsset == nullptr)
    {
    	
    	Context.AddError(FText::FromString(TEXT("Configuration Data Asset 'AIConfigDataAsset' is null: please assign a valid UDataAsset.")));
        Result = EDataValidationResult::Invalid;
    }

    return Result;
}
#endif