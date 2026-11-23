// Fill out your copyright notice in the Description page of Project Settings.

#include "Audio/PropAudioComponent.h"

#include "MetasoundSource.h"
#include "Data/AudioSettingsData.h"
#include "System/Subsystem/AudioSubsystem.h"

UPropAudioComponent::UPropAudioComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UPropAudioComponent::BeginPlay()
{
	Super::BeginPlay();

	LoadMetaSoundAsync();
}

void UPropAudioComponent::Play(float StartTime)
{
	if (!bAudioAssetLoaded)
	{
		return;
	}
	
	if (EventInputName == NAME_None)
	{
		Super::Play(StartTime);
	}
	else
	{
		SetTriggerParameter(EventInputName);
	}
}

void UPropAudioComponent::LoadMetaSoundAsync()
{
	const AActor* Owner = GetOwner();
	if (!IsValid(Owner)) { return;}


	const UGameInstance* GI = Owner->GetGameInstance();
	if (!IsValid(GI)) { return; }
	
	UAudioSubsystem* AudioSubsystem = GI->GetSubsystem<UAudioSubsystem>();
	
	if (!IsValid(AudioSubsystem)) { return;}
	
	AudioSubsystem->LoadSoundAsync(AudioAsset, FOnSoundLoaded::CreateUObject(this, &UPropAudioComponent::OnSoundLoaded));
}

void UPropAudioComponent::OnSoundLoaded(UMetaSoundSource* InSound)
{
	if (IsValid(InSound))
	{
		SetSound(InSound);
		bAudioAssetLoaded = true;

		if (bPlayAfterLoading)
		{
			Play(0);
		}
	}
}

TArray<FName> UPropAudioComponent::GetEventNameOptions() const
{
	if (!IsValid(AudioConfig)){ return TArray<FName>(); }

	return AudioConfig->GetFNameValues();
}

