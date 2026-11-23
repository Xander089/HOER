// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HOERBaseWidget.h"
#include "CommandRowWidget.generated.h"

class UImage;
class UTextBlock;

/**
 * Individual row widget displayed on the Controls/Commands page, associating an input key icon with an action description.
 */
UCLASS()
class HOER_API UCommandRowWidget : public UHOERBaseWidget
{
	GENERATED_BODY()

protected:

	virtual void NativePreConstruct() override;
	
	/** Localized description text explaining the input action (e.g. 'Jump', 'Sprint'). */
	UPROPERTY(EditAnywhere, Category = "Command Row")
	FText CommandText = FText::GetEmpty();
	
	/** Icon texture illustrating the corresponding button or key. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Command Row")
	TObjectPtr<UTexture2D> IconTexture = nullptr;
	
	/** Bound TextBlock widget presenting the command name. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CommandTextWidget = nullptr;

	/** Bound Image widget presenting the input button icon. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> IconWidget = nullptr;
};
