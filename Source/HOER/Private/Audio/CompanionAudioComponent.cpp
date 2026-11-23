// Fill out your copyright notice in the Description page of Project Settings.


#include "Audio/CompanionAudioComponent.h"

#include "Character/CompanionCharacter.h"
#include "Data/AudioSettingsData.h"


UCompanionAudioComponent::UCompanionAudioComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UCompanionAudioComponent::BeginPlay()
{
	Super::BeginPlay();
	SubscribeToOwnerInputEvents();
	
}

void UCompanionAudioComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnSubscribeFromOwnerInputEvents();
	Super::EndPlay(EndPlayReason);
}

void UCompanionAudioComponent::SubscribeToOwnerInputEvents()
{
	AActor* Owner =  GetOwner();
	if (!IsValid(Owner)){ return; }

	ACompanionCharacter* OwnerCharacter = Cast<ACompanionCharacter>(Owner);
	if (!IsValid(OwnerCharacter)){ return;}

	OwnerCharacter->OnLaunch.AddDynamic( this, &UCompanionAudioComponent::PlayOnFirstLanding);
	OwnerCharacter->OnJump.AddDynamic( this, &UCompanionAudioComponent::PlayJumping);
}

void UCompanionAudioComponent::UnSubscribeFromOwnerInputEvents()
{
	AActor* Owner =  GetOwner();
	if (!IsValid(Owner)){return;}

	ACompanionCharacter* OwnerCharacter = Cast<ACompanionCharacter>(Owner);
	if (!IsValid(OwnerCharacter)){ return;}

	if (OwnerCharacter->OnLaunch.IsBound())
	{
		OwnerCharacter->OnLaunch.RemoveDynamic( this, &UCompanionAudioComponent::PlayOnFirstLanding);
	}

	if (OwnerCharacter->OnJump.IsBound())
	{
		OwnerCharacter->OnJump.RemoveDynamic( this, &UCompanionAudioComponent::PlayJumping);
	}
}

void UCompanionAudioComponent::PlayJumping()
{
	if (CheckAudioComponentStateValidity())
	{
		SetTriggerParameter(AudioConfig->ConfigData.OnJumpName);
	}
}

void UCompanionAudioComponent::PlayOnFirstLanding()
{
	if (CheckAudioComponentStateValidity())
	{
		SetTriggerParameter(AudioConfig->ConfigData.OnLandingName);
	}
}

