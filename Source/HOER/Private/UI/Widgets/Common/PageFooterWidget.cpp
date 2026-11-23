// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Common/PageFooterWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UPageFooterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(FooterDataAsset))
	{
		SetData(FooterDataAsset->ConfigData);
	}
}

void UPageFooterWidget::SetData(const FFooterData& InData, const bool bIsGamePadConnected)
{
	if (IsValid(ConfirmTextBlock))
	{
		ConfirmTextBlock->SetText(InData.ConfirmText);
	}
	
	if (IsValid(CancelTextBlock))
	{
		CancelTextBlock->SetText(InData.CancelText);
	}
	
	if (IsValid(DescriptionTextBlock))
	{
		DescriptionTextBlock->SetText(InData.DescriptionText);
	}
	
	UTexture2D* CurrentConfirmIconTexture = bIsGamePadConnected ? InData.ConfirmIconGamepadTexture : InData.ConfirmIconTexture;
	UTexture2D* CurrentCancelIconTexture = bIsGamePadConnected ? InData.CancelIconGamepadTexture : InData.CancelIconTexture;
	
	if(IsValid(ConfirmImage))
	{
		if (CurrentConfirmIconTexture)
		{
			ConfirmImage->SetBrushFromTexture(CurrentConfirmIconTexture);
			ConfirmImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			ConfirmImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if(IsValid(CancelImage))
	{
		if (CurrentCancelIconTexture)
		{
			CancelImage->SetBrushFromTexture(CurrentCancelIconTexture);
			CancelImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			CancelImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
