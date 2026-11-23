// Fill out your copyright notice in the Description page of Project Settings.

#include "Audio/CharacterAudioComponent.h"
#include "Character/HOERCharacter.h"
#include "Data/AudioSettingsData.h"

UCharacterAudioComponent::UCharacterAudioComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UCharacterAudioComponent::BeginPlay()
{
	Super::BeginPlay();
	SubscribeToOwnerInputEvents();
	LoadMetaSoundAsync();
}

void UCharacterAudioComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnSubscribeFromOwnerInputEvents();
	Super::EndPlay(EndPlayReason);
}

void UCharacterAudioComponent::StartPlayingFootsteps()
{
	if (CheckAudioComponentStateValidity())
	{
		SetTriggerParameter(AudioConfig->ConfigData.OnStartWalkingName);
	}
}

void UCharacterAudioComponent::StopPlayingFootsteps()
{
	if (CheckAudioComponentStateValidity())
	{
		SetTriggerParameter(AudioConfig->ConfigData.OnStopWalkingName);
	}
}

void UCharacterAudioComponent::PlayJumping()
{
	if (CheckAudioComponentStateValidity())
	{
		SetTriggerParameter(AudioConfig->ConfigData.OnJumpName);
	}
}

void UCharacterAudioComponent::PlayThrowingSound()
{
	if (CheckAudioComponentStateValidity())
	{
		SetTriggerParameter(AudioConfig->ConfigData.OnThrowingName);
	}
}

void UCharacterAudioComponent::PlayGrabbingSound()
{
	if (CheckAudioComponentStateValidity())
	{
		SetTriggerParameter(AudioConfig->ConfigData.OnGrabName);
	}
}

void UCharacterAudioComponent::SubscribeToOwnerInputEvents()
{
	AActor* Owner =  GetOwner();
	
	if (!IsValid(Owner))
	{
		return;
	}
	
	AHOERCharacter* OwnerCharacter = Cast<AHOERCharacter>(Owner);
	
	if (!IsValid(OwnerCharacter)){ return;}
	
	OwnerCharacter->OnStartMoving.AddDynamic( this, &UCharacterAudioComponent::StartPlayingFootsteps);
	OwnerCharacter->OnStopMoving.AddDynamic( this, &UCharacterAudioComponent::StopPlayingFootsteps);
	OwnerCharacter->OnJump.AddDynamic( this, &UCharacterAudioComponent::PlayJumping);
	OwnerCharacter->OnThrowing.AddDynamic( this, &UCharacterAudioComponent::PlayThrowingSound);
	OwnerCharacter->OnGrabbing.AddDynamic( this, &UCharacterAudioComponent::PlayGrabbingSound);
	
}

void UCharacterAudioComponent::UnSubscribeFromOwnerInputEvents()
{
	AActor* Owner =  GetOwner();
	
	if (!IsValid(Owner))
	{
		return;
	}
	
	AHOERCharacter* OwnerCharacter = Cast<AHOERCharacter>(Owner);
	
	if (!IsValid(OwnerCharacter)){ return;}
	
	if (OwnerCharacter->OnStartMoving.IsBound())
	{
		OwnerCharacter->OnStartMoving.RemoveDynamic( this, &UCharacterAudioComponent::StartPlayingFootsteps);
	}
	
	if (OwnerCharacter->OnStopMoving.IsBound())
	{
		OwnerCharacter->OnStopMoving.RemoveDynamic( this, &UCharacterAudioComponent::StopPlayingFootsteps);
	}
	
	if (OwnerCharacter->OnJump.IsBound())
	{
		OwnerCharacter->OnJump.RemoveDynamic( this, &UCharacterAudioComponent::PlayJumping);
	}
	
	if (OwnerCharacter->OnThrowing.IsBound())
	{
		OwnerCharacter->OnThrowing.RemoveDynamic( this, &UCharacterAudioComponent::PlayThrowingSound);
	}
	
	if (OwnerCharacter->OnGrabbing.IsBound())
	{
		OwnerCharacter->OnGrabbing.RemoveDynamic( this, &UCharacterAudioComponent::PlayGrabbingSound);
	}
}
