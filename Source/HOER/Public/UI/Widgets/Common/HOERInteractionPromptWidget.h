// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HOERBaseWidget.h"
#include "HOERInteractionPromptWidget.generated.h"

class UTextBlock;

/**
 * 3D in-world widget displayed above interactable actors, showing action key prompts and instructions.
 */
UCLASS()
class HOER_API UHOERInteractionPromptWidget : public UHOERBaseWidget
{
	GENERATED_BODY()
	
public:
	/**
	 * Updates the displayed prompt text on the in-world widget.
	 *
	 * @param InText The localized prompt text to display.
	 */
	void SetInteractionPromptText(const FText& InText) const;
	
protected:
	/** Bound TextBlock displaying the interaction prompt string. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> InteractionPromptText;
};
