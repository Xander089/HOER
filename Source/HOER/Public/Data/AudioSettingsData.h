// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AudioSettingsData.generated.h"

/**
 * Settings structure defining MetaSound input parameter and trigger pin names.
 */
USTRUCT(BlueprintType)
struct FAAudioSettingsData
{
	GENERATED_BODY()
	
	/** MetaSound trigger pin name for beginning walking footsteps. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Names")
	FName OnStartWalkingName = "OnStartWalking";
	
	/** MetaSound trigger pin name for stopping footsteps. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Names")
	FName OnStopWalkingName = "OnStopWalking";
	
	/** MetaSound trigger pin name when grabbing objects. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Names")
	FName OnGrabName = "OnGrab";
	
	/** MetaSound trigger pin name when colliding or taking damage. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Names")
	FName OnHitName = "OnHit";
	
	/** MetaSound trigger pin name when executing a jump. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Names")
	FName OnJumpName = "OnJump";
	
	/** Time delay in seconds between successive footstep audio triggers while walking. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|DefaultValues", meta=(ClampMin=0.2, ClampMax=2))
	float WalkingFootstepPeriod = 0.2f;
	
	/** Time delay in seconds between successive footstep audio triggers while sprinting. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|DefaultValues", meta=(ClampMin=0.2, ClampMax=2))
	float SprintFootstepPeriod = 0.4f;
	
	/** MetaSound trigger pin name when a light turns on. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Lights|Names")
	FName OnLightOnName = "OnLightOn";

	/** MetaSound trigger pin name when throwing the companion plush. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character|Names")
	FName OnThrowingName = "OnThrowing";
	
	/** MetaSound trigger pin name when companion touches down on a surface. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Companion|Names")
	FName OnLandingName = "OnLanding";
	
	/** MetaSound trigger pin name for enemy creature grunt/roar vocals. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="NPC|Names")
	FName OnGruntingName = "OnGrunting";
	
	/** MetaSound trigger pin name when halting audio events. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="NPC|Names")
	FName OnStopName = "OnStop";
};

/**
 * Data Asset containing MetaSound parameter names and timing constants for characters and props.
 */
UCLASS()
class HOER_API UAudioSettingsData : public UDataAsset
{
	GENERATED_BODY()
	
public:

	/** MetaSound input pin name configuration mappings. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	FAAudioSettingsData ConfigData;
	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

	/**
	 * Extracts all configured FName parameter values for reflection or validation dropdowns.
	 *
	 * @return Array of all defined parameter FNames.
	 */
	TArray<FName> GetFNameValues()
	{
		TArray<FName> Result;

		for (TFieldIterator<FNameProperty> It(FAAudioSettingsData::StaticStruct()); It; ++It)
		{
			const FNameProperty* Property = *It;
			
			if (Property == nullptr) { continue; }
			
			if (const FName* PropValuePtr = Property->ContainerPtrToValuePtr<FName>(&ConfigData))
			{
				Result.Add(*PropValuePtr);
			}
		}
		return Result;
	}
	
};
