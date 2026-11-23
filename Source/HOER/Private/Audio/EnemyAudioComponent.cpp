#include "Audio/EnemyAudioComponent.h"

#include "AI/Character/AICharacter.h"
#include "Data/AudioSettingsData.h"

UEnemyAudioComponent::UEnemyAudioComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UEnemyAudioComponent::BeginPlay()
{
	Super::BeginPlay();
	SubscribeToOwnerInputEvents();
	
}

void UEnemyAudioComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnSubscribeFromOwnerInputEvents();
	Super::EndPlay(EndPlayReason);
}

void UEnemyAudioComponent::SubscribeToOwnerInputEvents()
{
	AActor* Owner =  GetOwner();
	if (!IsValid(Owner)){ return; }

	AAICharacter* OwnerCharacter = Cast<AAICharacter>(Owner);
	if (!IsValid(OwnerCharacter)){ return;}

	OwnerCharacter->OnPlayerDetected.AddDynamic( this, &UEnemyAudioComponent::PlayGrunt);
	OwnerCharacter->OnPlayerForgotten.AddDynamic( this, &UEnemyAudioComponent::StopSoundEventFadeOut);
}

void UEnemyAudioComponent::UnSubscribeFromOwnerInputEvents()
{
	AActor* Owner =  GetOwner();
	if (!IsValid(Owner)){return;}

	AAICharacter* OwnerCharacter = Cast<AAICharacter>(Owner);
	if (!IsValid(OwnerCharacter)){ return;}

	if (OwnerCharacter->OnPlayerDetected.IsBound())
	{
		OwnerCharacter->OnPlayerDetected.RemoveDynamic( this, &UEnemyAudioComponent::PlayGrunt);
	}
	if (OwnerCharacter->OnPlayerForgotten.IsBound())
	{
		OwnerCharacter->OnPlayerForgotten.RemoveDynamic( this,  &UEnemyAudioComponent::StopSoundEventFadeOut);
	}
}

void UEnemyAudioComponent::PlayGrunt()
{
	if (CheckAudioComponentStateValidity())
	{
		SetTriggerParameter(AudioConfig->ConfigData.OnGruntingName);
	}
}

