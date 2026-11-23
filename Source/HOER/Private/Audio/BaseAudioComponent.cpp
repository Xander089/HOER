// Fill out your copyright notice in the Description page of Project Settings.


#include "Audio/BaseAudioComponent.h"

#include "MetasoundSource.h"
#include "Data/AudioSettingsData.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Misc/DataValidation.h"


// Sets default values for this component's properties
UBaseAudioComponent::UBaseAudioComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UBaseAudioComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
EDataValidationResult UBaseAudioComponent::IsDataValid(class FDataValidationContext& Context) const
{
	// Call the Super implementation
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);
	
	if (AudioAsset.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Metasound Source reference AudioAsset is not set: please assign a valid reference audio asset.")));
		Result = EDataValidationResult::Invalid;
	}
	
	if (AudioConfig.IsNull())
	{
		Context.AddError(FText::FromString(TEXT("Data asset AudioConfig is not set: please assign a valid reference.")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif


void UBaseAudioComponent::LoadMetaSoundAsync()
{
	if (AudioAsset.IsNull()) return;

	// If already loaded, trigger the callback event immediately as a fallback
	if (AudioAsset.IsValid())
	{
		OnMetaSoundLoaded();
		return;
	}
	
	// Request the async load for the metasound asset
	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		AudioAsset.ToSoftObjectPath(), 
		FStreamableDelegate::CreateUObject(this, &UBaseAudioComponent::OnMetaSoundLoaded)
	);
}

void UBaseAudioComponent::OnMetaSoundLoaded()
{
	if (AudioAsset.IsValid())
	{
		SetSound(AudioAsset.Get());
	}
}

void UBaseAudioComponent::StopSoundEventFadeOut()
{
	if (CheckAudioComponentStateValidity())
	{
		SetTriggerParameter(AudioConfig->ConfigData.OnStopName);
	}
}

void UBaseAudioComponent::SubscribeToOwnerInputEvents()
{
}

void UBaseAudioComponent::UnSubscribeFromOwnerInputEvents()
{
}

bool UBaseAudioComponent::CheckAudioComponentStateValidity()
{
	return IsValid(AudioConfig) && IsValid(GetSound());
}
