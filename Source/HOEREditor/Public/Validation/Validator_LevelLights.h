// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorValidatorBase.h"
#include "Validator_LevelLights.generated.h"

/**
 * Editor Data Validator ensuring level light actors have non-zero attenuation radii and assigned components.
 */
UCLASS()
class HOEREDITOR_API UValidator_LevelLights : public UEditorValidatorBase
{
	GENERATED_BODY()
	
public:
	UValidator_LevelLights();

	virtual bool CanValidateAsset_Implementation(
		const FAssetData& InAssetData,
		UObject* InObject,
		FDataValidationContext& InContext) const override;

	virtual EDataValidationResult ValidateLoadedAsset_Implementation(
		const FAssetData& InAssetData,
		UObject* InAsset,
		FDataValidationContext& Context) override;
};
