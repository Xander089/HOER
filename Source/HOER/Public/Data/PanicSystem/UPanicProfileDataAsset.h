// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UPanicProfileDataAsset.generated.h"

/**
 * Defines an individual panic tier, its activation threshold, and identifier name.
 */
USTRUCT(BlueprintType)
struct FPanicStateDefinition
{
	GENERATED_BODY()

	/** Unique name identifying this panic stage (e.g. Calm, Uneasy, Panicked, Hysterical). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Panic State")
	FName StateName = "Calm";

	/** Numeric threshold at which the receiver transitions into this state. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Panic State", meta=(ClampMin="0.0"))
	float Threshold = 0.f;

};

/**
 * Data Asset defining the discrete panic stages, thresholds, and min/max bounds for the panic system.
 */
UCLASS(BlueprintType)
class HOER_API UPanicProfileDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * List of configured panic states and thresholds.
	 * (Note: Should be sorted in descending order from highest to lowest threshold).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Panic Profile")
	TArray<FPanicStateDefinition> PanicStates;

	/**
	 * Resolves the appropriate panic state for a given numeric panic level.
	 *
	 * @param PanicLevel Current numeric panic value.
	 * @param OutState The resolved panic state definition struct.
	 * @return True if a matching state was found, false otherwise.
	 */
	bool GetStateForLevel(const float PanicLevel, FPanicStateDefinition& OutState) const;

	/** Returns minimum panic threshold across all configured states. */
	float GetMinPanicLevel() const;

	/** Returns maximum panic threshold across all configured states. */
	float GetMaxPanicLevel() const;
	
	/** Returns the complete array of panic state definitions. */
	TArray<FPanicStateDefinition> GetPanicStates() const;
	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
