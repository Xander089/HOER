// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Common/SliderRowWidget.h"

#include "Components/SizeBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

void USliderRowWidget::SetFocus(const bool IsFocused)
{
	bSetInitialKeyboardFocus = IsFocused;
}

void USliderRowWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetBindings();
}

void USliderRowWidget::NativeDestruct()
{
	Super::NativeDestruct();
	RemoveBindings();
}

void USliderRowWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetSliderWidth(SliderRowWidth);
}

void USliderRowWidget::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);
	UE_LOG(LogTemp, Warning, TEXT("Slider Row Focused Path"));
	BP_OnAddedToFocusPath();
}

void USliderRowWidget::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	BP_OnRemovedFromFocusPath();
	UE_LOG(LogTemp, Warning, TEXT("Slider Row Focused Path Removed"));
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
}


void USliderRowWidget::NativeSliderChanged(const float Value)
{
	// prevents running the Slider's delegate OnSliderChanged when the widget is rendered and the value is read from GameDeveloperSettings the first time
	if (bIsSettingValue)
	{
		return;
	}

	if (OnSliderChanged.IsBound())
	{
		OnSliderChanged.Broadcast(Value);
	}
	
	// Slider step is 0.05
	UpdateValueText(Value);
}

void USliderRowWidget::SetBindings()
{
	if(IsValid(SliderWidget))
	{
		if (bSetInitialKeyboardFocus)
		{
			SliderWidget->SetKeyboardFocus();
		}
		
		SliderWidget->OnValueChanged.RemoveAll(this);
		SliderWidget->OnValueChanged.AddDynamic(this, &USliderRowWidget::NativeSliderChanged);
	}
}

void USliderRowWidget::RemoveBindings() const
{
	if(IsValid(SliderWidget))
	{
		SliderWidget->OnValueChanged.RemoveAll(this);
	}
}

void USliderRowWidget::SetSliderWidth(float InWidth)
{
	SizeBoxWidget->SetWidthOverride(InWidth);
}

void USliderRowWidget::SetSliderValue(const float InValue)
{
	if (IsValid(SliderWidget))
	{
		TGuardValue<bool> SettingValueGuard(bIsSettingValue, true);
		SliderWidget->SetValue(InValue);
	}
	UpdateValueText(InValue);
}

void USliderRowWidget::UpdateValueText(const float Value) const
{
	const int ValueAsInt = FMath::RoundToInt(Value * 20);

	if (IsValid(ValueTextWidget))
	{
		ValueTextWidget->SetText(FText::AsNumber(ValueAsInt));
	}
}
