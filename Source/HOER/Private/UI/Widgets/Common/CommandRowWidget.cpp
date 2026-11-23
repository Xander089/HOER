// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Common/CommandRowWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


void UCommandRowWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (IsValid(CommandTextWidget))
	{
		CommandTextWidget->SetText(CommandText);
	}
	
	if (IsValid(IconWidget) && IsValid(IconTexture))
	{
		IconWidget->SetBrushFromTexture(IconTexture);
	}
	
}
