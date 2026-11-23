// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HOERBaseWidget.h"
#include "Data/UI/FooterDataAsset.h"
#include "PageFooterWidget.generated.h"

class UImage;
class UTextBlock;

/**
 * Standard footer bar displayed at the bottom of HOER menu pages showing confirm, back, and contextual actions.
 */
UCLASS()
class HOER_API UPageFooterWidget : public UHOERBaseWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;
	
	/**
	 * Populates the footer bar with prompt strings and device-appropriate action icons.
	 *
	 * @param InData The footer data record containing strings and textures.
	 * @param bIsGamePadConnected Whether gamepad prompt icons should be chosen instead of keyboard/mouse.
	 */
	UFUNCTION(BlueprintCallable, Category = "Footer")
	void SetData(const FFooterData& InData, bool bIsGamePadConnected = false);

protected:

	/** Fallback footer configuration data asset. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
	TObjectPtr<UFooterDataAsset> FooterDataAsset = nullptr;
	
	/** Bound TextBlock for Confirm button text. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category="Footer")
	TObjectPtr<UTextBlock> ConfirmTextBlock = nullptr;
	
	/** Bound TextBlock for Cancel / Back button text. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category="Footer")
	TObjectPtr<UTextBlock> CancelTextBlock = nullptr;

	/** Bound Image displaying the Confirm button icon texture. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category="Footer")
	TObjectPtr<UImage> ConfirmImage = nullptr;

	/** Bound Image displaying the Cancel button icon texture. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category="Footer")
	TObjectPtr<UImage> CancelImage = nullptr;

	/** Bound TextBlock displaying descriptive guidance text. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category="Footer")
	TObjectPtr<UTextBlock> DescriptionTextBlock = nullptr;
};
