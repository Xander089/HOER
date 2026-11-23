// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/HOERAIController.h"
#include "AIEnemyController.generated.h"

class AAICharacter;
class UAIPerceptionComponent;
class UAISenseConfig_Hearing;
class UAISenseConfig_Sight;
class UAIEnemyConfigDataAsset;
struct FCurveTableRowHandle;
struct FRealCurve;
struct FAIEnemyConfig;
class UAIWaypointsDataAsset;

/**
 * Perception sense and navigation configuration properties modulated across risk awareness levels.
 */
UENUM(BlueprintType)
enum class EPerceptionConfig : uint8
{
	SightRadius UMETA(DisplayName = "Sight Radius"),
	LoseSightRadius UMETA(DisplayName = "Lose Sight Radius"),
	PeripheralVisionAngle UMETA(DisplayName = "Peripheral Vision Angle"),
	AutoSuccessRangeFromLastSeenLocation UMETA(DisplayName = "Auto Success Range"),
	MaxAge UMETA(DisplayName = "Max Age"),
	HearingRange UMETA(DisplayName = "Hearing Range"),
	MaxReachableRadius UMETA(DisplayName = "Reachable Radius"),
};

/**
 * Controller driving enemy NPC behavior via Behavior Trees and AI Perception (Sight and Hearing),
 * responding to director commands and safe zones.
 */
UCLASS()
class HOER_API AAIEnemyController : public AHOERAIController
{
	GENERATED_BODY()

public:
	AAIEnemyController();

	/** Assigns the next waypoint location on the blackboard. */
	void SetNextWaypointLocation(FVector& Location);

	/** Sets the teleport destination spawn point on the blackboard. */
	void SetNextSpawnLocation(const FVector& Location) const;

	/** Sets the specific patrol/search location assigned by the AI Director. */
	void SetAssignedLocation(const FVector& Location) const;
	
	/** Updates the current risk awareness scalar on the blackboard. */
	void SetRiskAwareness(const float& RiskAwareness) const;
	
	/** Modulates perception senses (sight distance, peripheral angle, hearing) based on risk awareness. */
	void ReactToRiskAwarenessChange(const float& RiskAwareness);
	
	/** Finds a valid navigable point on the NavMesh within reachable radius. */
	void FindReachablePoint(FVector& OutLocation) const;  
	
	/** Advances to and returns the next sequential waypoint coordinate. */
	void GetNextWaypoint(FVector& OutLocation); 
	
	/** Toggles whether perception detection should ignore the player (e.g. inside safe zones). */
	void SetIgnoreDetection(bool bIgnore);
	
	/** Notifies controller that the player entered or exited a safe zone. */
	void NotifyPlayerEnteredSafeZone(const bool bEntered);

protected:
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	/** Data asset containing base perception parameters and tuning metrics. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Data")
	TObjectPtr<UAIEnemyConfigDataAsset> AIConfigDataAsset = nullptr;
	
	/** Data asset containing pre-placed patrol waypoints for this level. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Waypoints")
	TObjectPtr<UAIWaypointsDataAsset> WayPointsDataAsset = nullptr;
	
	/** AI Perception Component managing sensory input from sight and hearing. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent = nullptr;
	
	/** Sight sense configuration profile. */
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightConfig = nullptr;

	/** Hearing sense configuration profile. */
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig = nullptr;

	/** Curve table mapping risk awareness to perception parameter multipliers. */
	UPROPERTY(EditAnywhere, Category = "Perception Stats")
	TMap<EPerceptionConfig, FCurveTableRowHandle> StatCurveHandles;
	
	virtual void OnPossess(APawn* InPawn) override;
	
	/** Used to initialize the Blackboard with default values */
	virtual void InitializeBlackboardValues() override;
	
	/** Bound handler called when AIPerceptionComponent detects or loses a target. */
	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);

private:
	
	/** Uses config values from UDataAssets to initialize Sight and Hearing configuration */ 
	void InitializeSenseConfigsFromDataAsset();
	FRealCurve* GetCurveByEnum(const FString& Context, const EPerceptionConfig& EnumValue);
	
	UPROPERTY()
	TWeakObjectPtr<AAICharacter> OwnerAICharacter;
	
	UPROPERTY()
	int NextWaypoint = 0;

	// Set to true once OnPlayerDetected has been broadcast. Reset when the player enters a safe zone
	bool bPlayerDetectedBroadcast = false;

public:
	
#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

};
