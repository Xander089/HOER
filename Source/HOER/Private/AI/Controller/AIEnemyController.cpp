// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controller/AIEnemyController.h"

#include "NavigationSystem.h"
#include "Data/AI/AIEnemyConfigDataAsset.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Engine/CurveTable.h"
#include "Curves/RealCurve.h"
#include "Misc/DataValidation.h"
#include "AI/AIKeys.h"
#include "AI/Character/AICharacter.h"
#include "Character/HOERCharacter.h"
#include "Data/AI/AIWaypointsDataAsset.h"
#include "System/Subsystem/CorridorManagerSubsystem.h"

AAIEnemyController::AAIEnemyController()
{
	PrimaryActorTick.bCanEverTick = true;
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));

	if (!IsValid(AIPerceptionComponent))
	{
		return;
	}

	InitializeSenseConfigsFromDataAsset();

	if (!IsValid(HearingConfig) || !IsValid(SightConfig))
	{
		return;
	}

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->ConfigureSense(*HearingConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AAIEnemyController::SetNextWaypointLocation(FVector& Location)
{
	if (!IsValid(BlackboardComponent)){ return; }
	if (ValidateLocation(Location)) // validating the waypoint against the actual navigation mesh
	{
		BlackboardComponent->SetValueAsVector(AIBlackboardKeys::RandomLocation, Location);
	}
}

void AAIEnemyController::SetNextSpawnLocation(const FVector& Location) const
{
	if (!IsValid(BlackboardComponent)){ return; }
	BlackboardComponent->SetValueAsVector(AIBlackboardKeys::NextSpawnPoint, Location);
}

void AAIEnemyController::SetAssignedLocation(const FVector& Location) const
{
	if (!IsValid(BlackboardComponent))
	{
		return;
	}

	BlackboardComponent->SetValueAsVector(AIBlackboardKeys::AssignedLocation, Location);
}

void AAIEnemyController::SetRiskAwareness(const float& RiskAwareness) const
{
	if (!IsValid(BlackboardComponent))
	{
		return;
	}

	BlackboardComponent->SetValueAsFloat(AIBlackboardKeys::RiskAwareness, RiskAwareness);
}

void AAIEnemyController::ReactToRiskAwarenessChange(const float& RiskAwareness)
{
	if (!IsValid(SightConfig) || !IsValid(HearingConfig))
	{
		return;
	}

	// RiskAwareness is between 0 - 1: remapped to 0 - 10 so that it can match the curve table lookup
	const float MappedRiskAwareness = FMath::Clamp(FMath::Floor(RiskAwareness * 10.0f), 0.0f, 10.0f);

	static const FString ContextString(TEXT("Evaluation Context"));

	if (const FRealCurve* RealCurve = GetCurveByEnum(ContextString, EPerceptionConfig::SightRadius))
	{
		SightConfig->SightRadius = RealCurve->Eval(MappedRiskAwareness);
	}

	if (const FRealCurve* RealCurve = GetCurveByEnum(ContextString, EPerceptionConfig::LoseSightRadius))
	{
		SightConfig->LoseSightRadius = RealCurve->Eval(MappedRiskAwareness);
	}

	if (const FRealCurve* RealCurve = GetCurveByEnum(ContextString, EPerceptionConfig::PeripheralVisionAngle))
	{
		SightConfig->PeripheralVisionAngleDegrees = RealCurve->Eval(MappedRiskAwareness);
	}

	if (const FRealCurve* RealCurve = GetCurveByEnum(ContextString, EPerceptionConfig::AutoSuccessRangeFromLastSeenLocation))
	{
		SightConfig->AutoSuccessRangeFromLastSeenLocation = RealCurve->Eval(MappedRiskAwareness);
	}

	if (const FRealCurve* RealCurve = GetCurveByEnum(ContextString, EPerceptionConfig::MaxAge))
	{
		SightConfig->SetMaxAge(RealCurve->Eval(MappedRiskAwareness));
	}

	if (const FRealCurve* RealCurve = GetCurveByEnum(ContextString, EPerceptionConfig::HearingRange))
	{
		HearingConfig->HearingRange = RealCurve->Eval(MappedRiskAwareness);
	}
	
	if (const FRealCurve* RealCurve = GetCurveByEnum(ContextString, EPerceptionConfig::MaxReachableRadius))
	{
		AIConfigDataAsset->BaseConfiguration.MaxReachableRadius = RealCurve->Eval(MappedRiskAwareness);
	}

}

void AAIEnemyController::FindReachablePoint(FVector& OutLocation) const
{
	// Check Owner Pawn reference
	if (!OwnerAICharacter.IsValid()){ return; }
	
	// Check Owner Pawn reference
	if (!IsValid(AIConfigDataAsset)){ return; }

	// Check Navigation System validity
	const UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!IsValid(NavSys)){ return; }
	
	const float Radius = AIConfigDataAsset->BaseConfiguration.MaxReachableRadius;
	FNavLocation NavLocation;
	
	// tries to find a reachable navigable point
	if (NavSys->GetRandomReachablePointInRadius(OwnerAICharacter->GetActorLocation(), Radius, NavLocation))
	{
		OutLocation = NavLocation.Location;
	}
}

void AAIEnemyController::GetNextWaypoint(FVector& OutLocation)
{
	if(!IsValid(WayPointsDataAsset)){ return; }

	if(WayPointsDataAsset->Waypoints.IsEmpty()){ return; }

	NextWaypoint++;
	if(NextWaypoint >= WayPointsDataAsset->Waypoints.Num()){
		NextWaypoint = 0;
	}

	OutLocation = WayPointsDataAsset->Waypoints[NextWaypoint];

	if(!ValidateLocation(OutLocation, AIConfigDataAsset->BaseConfiguration.MaxReachableRadius)){
		OutLocation = FVector::ZeroVector;
	}
}

void AAIEnemyController::SetIgnoreDetection(const bool bIgnore)
{
	if (IsValid(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsBool(AIBlackboardKeys::PlayerIlluminated, bIgnore );
	}
}

void AAIEnemyController::NotifyPlayerEnteredSafeZone(const bool bEntered)
{
	// Allow sound event to play again next time the player is detected
	if (bEntered)
	{
		bPlayerDetectedBroadcast = false;
	}
	
	if (IsValid(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsBool(AIBlackboardKeys::PlayerInSafeZone, bEntered);
		
		// Also clear the player actor reference when the player enters a safe zone, so that the AI stops chasing the player
		if (bEntered)
		{
			BlackboardComponent->ClearValue(AIBlackboardKeys::PlayerActor);
		}
	}
}

void AAIEnemyController::BeginPlay()
{
	Super::BeginPlay();
	
	//Register to the Corridor Manager Subsystem
	if (const UWorld* World = GetWorld(); IsValid(World))
	{
		if (UCorridorManagerSubsystem* CorridorManager = World->GetSubsystem<UCorridorManagerSubsystem>(); IsValid(CorridorManager))
		{
			CorridorManager->RegisterAIEnemyController(this);
		}
	}
}

void AAIEnemyController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unregister from the Corridor Manager Subsystem
	if (const UWorld* World = GetWorld(); IsValid(World))
	{
		if (UCorridorManagerSubsystem* CorridorManager = World->GetSubsystem<UCorridorManagerSubsystem>(); IsValid(CorridorManager))
		{
			CorridorManager->UnregisterAIEnemyController(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}


void AAIEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsValid(InPawn))
	{
		OwnerAICharacter = Cast<AAICharacter>(InPawn);
	}
	
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAIEnemyController::OnTargetDetected);

	// Initialize Blackboard and Behavior Tree

	InitializeBlackboardValues();

	if (IsValid(BehaviorTreeAsset))
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void AAIEnemyController::InitializeBlackboardValues()
{
	Super::InitializeBlackboardValues();

	if (IsValid(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsFloat(AIBlackboardKeys::AssignedLocationIdleTime, AIConfigDataAsset->BaseConfiguration.AssignedLocationIdleTime);
		BlackboardComponent->SetValueAsFloat(AIBlackboardKeys::RandomPatrolIdleTime, AIConfigDataAsset->BaseConfiguration.RandomPatrolIdleTime);
		BlackboardComponent->ClearValue(AIBlackboardKeys::RandomLocation);
		BlackboardComponent->SetValueAsFloat(AIBlackboardKeys::TeleportDelay, AIConfigDataAsset->BaseConfiguration.TeleportDelay);
		BlackboardComponent->ClearValue(AIBlackboardKeys::NextSpawnPoint);
	}
}

// cannot be made const because it is linked to the AIPerceptionComponent->OnTargetPerceptionUpdated's delegate, which requires a non-const method
void AAIEnemyController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{

	if (!IsValid(BlackboardComponent))
	{
		return;
	}
	
	// AI will ignore the player when it enters a safe zone (e.g., any room)
	if (BlackboardComponent->GetValueAsBool(AIBlackboardKeys::PlayerInSafeZone))
	{
		return;	
	}
	
	if (Stimulus.WasSuccessfullySensed())
	{
		// If target is the local player
		if (const AHOERCharacter* SensedCharacter = Cast<AHOERCharacter>(Actor); IsValid(SensedCharacter))
		{
			BlackboardComponent->SetValueAsObject(AIBlackboardKeys::PlayerActor, Actor);
			
			// Broadcast to Possessed AI Character. Broadcast only once, until the player enters a safe zone
			if(!bPlayerDetectedBroadcast && OwnerAICharacter.IsValid() && OwnerAICharacter->OnPlayerDetected.IsBound())
			{
				bPlayerDetectedBroadcast = true;
				OwnerAICharacter->OnPlayerDetected.Broadcast();
			}
		}
	}
	else
	{
		BlackboardComponent->SetValueAsObject(AIBlackboardKeys::PlayerActor, nullptr);

		// Broadcast to Possessed AI Character
		if(OwnerAICharacter.IsValid() && OwnerAICharacter->OnPlayerForgotten.IsBound())
		{
			OwnerAICharacter->OnPlayerForgotten.Broadcast();
		}
	}
}



void AAIEnemyController::InitializeSenseConfigsFromDataAsset()
{

	// Creating hearing and sight config components
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

	if (!IsValid(HearingConfig) || !IsValid(SightConfig))
	{
		return;
	}


	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;


	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
}

FRealCurve* AAIEnemyController::GetCurveByEnum(const FString& Context, const EPerceptionConfig& EnumValue)
{
	if (const FCurveTableRowHandle* Handle = StatCurveHandles.Find(EnumValue))
	{
		if (FRealCurve* Curve = Handle->GetCurve(Context))
		{
			return Curve;
		}
	}

	return nullptr;
}


#if WITH_EDITOR
EDataValidationResult AAIEnemyController::IsDataValid(FDataValidationContext& Context) const
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