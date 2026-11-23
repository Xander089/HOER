// Fill out your copyright notice in the Description page of Project Settings.


#include "HOER/Public/UI/Widgets/Common/HOERButtonWidget.h"

#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

void UHOERButtonWidget::SetButtonText(const FText& InText)
{
	if (!TextBlock)
	{
		return;
	}

	TextBlock->SetText(InText);
}

void UHOERButtonWidget::SetButtonWidth(const float InWidth)
{
	if (IsValid(ButtonSizeBox))
	{
		ButtonSizeBox->SetWidthOverride(InWidth);
	}
}

void UHOERButtonWidget::SetButtonFocus() const
{
	if (IsValid(Button))
	{
		Button->SetFocus();
	}
}

UWidget* UHOERButtonWidget::GetButton() const
{
	return Button;
}

void UHOERButtonWidget::SetHasPageOnTop()
{
	bHasPageOnTop = true;
}

bool UHOERButtonWidget::IsFocused()
{
	return bIsFocused;
}

void UHOERButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// used to bind the native OnClicked UButton Delegate to this class public Delegate
	SetBindings();
}

void UHOERButtonWidget::NativeDestruct()
{
	RemoveBindings();
	Super::NativeDestruct();
}

void UHOERButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetButtonText(ButtonText);
	SetButtonWidth(ButtonWidth);
}

void UHOERButtonWidget::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);
	UE_LOG(LogTemp, Warning, TEXT("Button added to focus path"));
	BP_OnAddedToFocusPath();
	bIsFocused = true;
	
	if (OnButtonFocused.IsBound())
	{
		OnButtonFocused.Broadcast(this);
	}
}

void UHOERButtonWidget::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	if (bHasPageOnTop)
	{
		bHasPageOnTop = false;
	}
	else
	{
		BP_OnRemovedFromFocusPath();
		bIsFocused = false;
	}
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
	UE_LOG(LogTemp, Warning, TEXT("Button removed from focus path"));
}


void UHOERButtonWidget::NativeButtonClicked()
{
	//only called on blueprints
	BP_OnButtonClicked();
	
	if (OnButtonClicked.IsBound())
	{
		OnButtonClicked.Broadcast(this);
	}
}

void UHOERButtonWidget::SetBindings()
{
	if (IsValid(Button))
	{
		Button->OnClicked.AddDynamic(this, &UHOERButtonWidget::NativeButtonClicked);
	}
}

void UHOERButtonWidget::RemoveBindings() const
{
	if (IsValid(Button))
	{
		Button->OnClicked.RemoveAll(this);
	}
}


