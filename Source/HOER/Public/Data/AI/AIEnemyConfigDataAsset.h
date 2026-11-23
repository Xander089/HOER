// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AIEnemyConfigDataAsset.generated.h"

/**
 * Perception parameters configuring sight, hearing, and peripheral vision on the enemy AI.
 */
USTRUCT(BlueprintType)
struct FAIPerceptionConfig
{
	GENERATED_BODY()
	
	/** Maximum sight distance to acquire a target (in cm). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception|Sight", meta = (UIMin = 0.0, ClampMin = 0.0, Units="Centimeters"))
	float SightRadius = 3000.0f;
 
	/** Distance beyond which visual tracking of an already acquired target is lost (in cm). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception|Sight", meta = (UIMin = 0.0, ClampMin = 0.0, Units="Centimeters"))
	float LoseSightRadius = 3500.0f;
 
	/** Half-angle in degrees from the facing vector defining the peripheral vision cone. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception|Sight", meta=(UIMin = 0.0, ClampMin = 0.0, UIMax = 180.0, ClampMax = 180.0, DisplayName="Peripheral Vision Half Angle", Units="Degrees"))
	float PeripheralVisionAngle = 90.0f;
	
	/** Distance around last-known position where the target remains automatically detected. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception|Sight", meta = (Units="Centimeters"))
	float AutoSuccessRangeFromLastSeenLocation = 100.0f;
	
	/** Backward offset for the vision cone origin to grant close-proximity awareness. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception|Sight", meta = (UIMin = 0.0, ClampMin = 0.0, Units="Centimeters"))
	float PointOfViewBackwardOffset = 0.0f;

	/** Near clipping radius for close-by peripheral awareness. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception|Sight", meta = (UIMin = 0.0, ClampMin = 0.0, Units="Centimeters"))
	float NearClippingRadius = 0.0f;
	
	/** Time in seconds before an unseen target is dropped and forgotten. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception|Sight", meta = (UIMin = 1.0, ClampMin = 1.0, Units="Seconds"))
	float MaxAge = 1.0f;
 
	/** Maximum hearing distance to perceive noise stimuli (in cm). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception|Hearing")
	float HearingRange = 2000.0f;
};

/**
 * Base pacing parameters for AI enemy idle times, reachability ranges, and teleport delays.
 */
USTRUCT(BlueprintType)
struct FAIEnemyConfig
{
	GENERATED_BODY()
	
	/** Time in seconds the AI stays idle upon reaching a director-assigned location. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cooldown", meta = (ClampMin = 0.0, ClampMax = 20.0, Units="Seconds"))
	float AssignedLocationIdleTime = 5.0f;
	
	/** Time in seconds the AI stays idle upon reaching a random patrol location. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cooldown", meta = (ClampMin = 0.0, ClampMax = 20.0, Units="Seconds"))
	float RandomPatrolIdleTime = 5.0f;

	/** Maximum search radius when sampling reachable points on the NavMesh (in cm). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Patrol", meta = (ClampMin = 100.0, ClampMax = 1000.0, Units="Centimeters"))
	float MaxReachableRadius = 100.0f;

	/** Delay in seconds before teleporting the AI to the corridor spawn point entered by the player. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Corridor", meta = (ClampMin = 0.0, Units="Seconds"))
	float TeleportDelay = 2.0f;
};

/**
 * Data Asset bundling enemy NPC perception parameters, idle timings, and target affiliation filters.
 */
UCLASS(BlueprintType)
class HOER_API UAIEnemyConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	/** Base timing and locomotion reachability configuration. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	FAIEnemyConfig BaseConfiguration;
	
	/** Sight and hearing perception sense tuning properties. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Perception")
	FAIPerceptionConfig PerceptionConfiguration;
	
	/** Whether enemy affiliation targets trigger detection. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Perception")
	bool bDetectEnemies = true;
    
	/** Whether friendly affiliation targets trigger detection. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Perception")
	bool bDetectFriendlies = false;
    
	/** Whether neutral affiliation targets trigger detection. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Perception")
	bool bDetectNeutrals = false;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

};
