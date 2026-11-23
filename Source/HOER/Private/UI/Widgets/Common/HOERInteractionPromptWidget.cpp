// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Common/HOERInteractionPromptWidget.h"
#include "Components/TextBlock.h"


void UHOERInteractionPromptWidget::SetInteractionPromptText(const FText& InText) const
{
	InteractionPromptText->SetText(InText);
}
