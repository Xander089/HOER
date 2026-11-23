// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorValidatorBase.h"
#include "Validator_InteractableObject.generated.h"

/**
 * Editor Data Validator validating that interactable objects have properly configured collision,
 * interaction prompt widgets, and non-empty required item settings.
 */
UCLASS()
class HOEREDITOR_API UValidator_InteractableObject : public UEditorValidatorBase
{
	GENERATED_BODY()
	
public:
	UValidator_InteractableObject();

	virtual bool CanValidateAsset_Implementation(
		const FAssetData& InAssetData,
		UObject* InObject,
		FDataValidationContext& InContext) const override;

	virtual EDataValidationResult ValidateLoadedAsset_Implementation(
		const FAssetData& InAssetData,
		UObject* InAsset,
		FDataValidationContext& Context) override;
};
