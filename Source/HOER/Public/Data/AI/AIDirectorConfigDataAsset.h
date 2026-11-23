// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AIDirectorConfigDataAsset.generated.h"

/**
 * Pacing and balancing parameters configuring the AI Director's risk assessment and patrol assignments.
 */
USTRUCT(BlueprintType)
struct FAIDirectorConfig
{
	GENERATED_BODY()

	/** Weight used to calculate risk related to the absolute count of lights turned on in the level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Risk Awareness", meta=(ClampMin=0, ClampMax=1))
	float LightOnWeight = 0.2f;

	/** Weight used to calculate risk related to consecutive adjacent lights turned on along corridors. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Risk Awareness", meta=(ClampMin=0, ClampMax=1))
	float AdjacentLightOnWeight = 0.8f;
	
	/** Number of recent light-switching events the AI Director retains in short-term history memory. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Risk Awareness", meta=(ClampMin=1, ClampMax=10))
	int32 LightEventsShortTermMemoryCount = 5;
	
	/** Cooldown interval in seconds before the director can assign another patrol location. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cooldown", meta=(ClampMin=1, ClampMax=10))
	float NewAssignLocationCooldownTime = 5.0f;
	
	/** Search box half-extent in cm when projecting assigned coordinates onto the NavMesh. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Path Finding", meta=(ClampMin=10.0, ClampMax=200.0))
	float ProjectPointToNavigationExtent = 100.0f;

	/** Probability rate (0.0 to 1.0) that a candidate location is dispatched to the enemy blackboard. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Path Finding", meta=(ClampMin=0.0, ClampMax=1.0))
	float AssignNewLocationSuccessRate = 0.8f;
};

/**
 * Data Asset holding director configuration tuning weights and navigation limits.
 */
UCLASS(BlueprintType)
class HOER_API UAIDirectorConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:

	/** Configuration values for the AI Director controller. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	FAIDirectorConfig ConfigData;
	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

};
