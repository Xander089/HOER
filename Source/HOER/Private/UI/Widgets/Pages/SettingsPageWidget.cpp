// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Pages/SettingsPageWidget.h"

#include "UI/Widgets/Common/SliderRowWidget.h"


void USettingsPageWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if(IsValid(MusicSlider))
	{
		MusicSlider->SetFocus(true);
		MusicSlider->OnSliderChanged.RemoveAll(this);
		MusicSlider->OnSliderChanged.AddDynamic(this, &USettingsPageWidget::HandleOnMusicValueChanged);
	}
	
	if(IsValid(SFXSlider))
	{
		SFXSlider->OnSliderChanged.RemoveAll(this);
		SFXSlider->OnSliderChanged.AddDynamic(this, &USettingsPageWidget::HandleOnSFXValueChanged);
	}
}

void USettingsPageWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	if(IsValid(MusicSlider))
	{
		MusicSlider->OnSliderChanged.RemoveAll(this);
	}
	
	if(IsValid(SFXSlider))
	{
		SFXSlider->OnSliderChanged.RemoveAll(this);
	}
}

void USettingsPageWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
}

void USettingsPageWidget::OnPageAdded()
{
	Super::OnPageAdded();
	BP_HandleOnPageAdded();
}

void USettingsPageWidget::OnPageRemoved()
{
	BP_HandleOnPageRemoved();
	MusicSlider->BP_OnRemovedFromFocusPath(false);
	SFXSlider->BP_OnRemovedFromFocusPath(false);
	Super::OnPageRemoved();
}

void USettingsPageWidget::HandleOnMusicValueChanged(float Value)
{
	BP_HandleOnMusicValueChanged(Value);
}

void USettingsPageWidget::HandleOnSFXValueChanged(float Value)
{
	BP_HandleOnSFXValueChanged(Value);
}
