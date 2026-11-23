// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FooterDataAsset.generated.h"

class UImage;
class UTextBlock;

/**
 * Data structure specifying icons and button labels displayed in standard UI page footers.
 */
USTRUCT(BlueprintType)
struct FFooterData
{
	GENERATED_BODY()
	
	/** Texture icon for keyboard confirm action (e.g. Enter / E). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footer Data")
	TObjectPtr<UTexture2D> ConfirmIconTexture = nullptr;
	
	/** Texture icon for keyboard cancel action (e.g. Escape / Q). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footer Data")
	TObjectPtr<UTexture2D> CancelIconTexture = nullptr;

	/** Texture icon for gamepad confirm button (e.g. A / Cross). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footer Data")
	TObjectPtr<UTexture2D> ConfirmIconGamepadTexture = nullptr;
	
	/** Texture icon for gamepad cancel button (e.g. B / Circle). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footer Data")
	TObjectPtr<UTexture2D> CancelIconGamepadTexture = nullptr;
 
	/** Informational description text shown in the footer bar. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footer Data")
	FText DescriptionText = FText::GetEmpty();

	/** Label text for the Confirm action prompt. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footer Data")
	FText ConfirmText = FText::GetEmpty();

	/** Label text for the Cancel / Back action prompt. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footer Data")
	FText CancelText = FText::GetEmpty();
};

/**
 * Data Asset configuring bottom-bar footer button prompts and iconography across menu screens.
 */
UCLASS(BlueprintType)
class HOER_API UFooterDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:

	/** Configuration struct defining footer icons and localized prompt strings. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Data")
	FFooterData ConfigData;
};
