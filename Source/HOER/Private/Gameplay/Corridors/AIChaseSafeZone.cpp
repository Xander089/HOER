// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Corridors/AIChaseSafeZone.h"

#include "Character/HOERCharacter.h"
#include "Components/BoxComponent.h"
#include "Save/SaveSubsystem.h"
#include "System/HOERBlueprintFunctionLibrary.h"
#include "System/Subsystem/CorridorManagerSubsystem.h"


// Sets default values
AAIChaseSafeZone::AAIChaseSafeZone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->SetBoxExtent(FVector(50.0f, 200.0f, 100.0f));
	RootComponent = TriggerBox;
}

void AAIChaseSafeZone::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	AHOERCharacter* PlayerCharacter = Cast<AHOERCharacter>(OtherActor);
	
	if (!IsValid(PlayerCharacter))
	{
		return;
	}
	
	PlayerCharacter->SetIsInSafeZone(true);
	NotifyCorridorManager(true);
	
	BP_OnPlayerEnter();
	TriggerSaveAsync();
}

void AAIChaseSafeZone::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	
	AHOERCharacter* PlayerCharacter = Cast<AHOERCharacter>(OtherActor);
	
	if (!IsValid(PlayerCharacter))
	{
		return;
	}
	
	PlayerCharacter->SetIsInSafeZone(false);
	
	NotifyCorridorManager(false);
}

void AAIChaseSafeZone::NotifyCorridorManager(const bool bEnteredSafeZone) const
{
	if (const UWorld* World = GetWorld(); IsValid(World))
	{
		if (const UCorridorManagerSubsystem* CorridorManager = World->GetSubsystem<UCorridorManagerSubsystem>(); IsValid(CorridorManager))
		{
			CorridorManager->NotifyPlayerEnteredSafeZone(bEnteredSafeZone);
		}
	}
}

void AAIChaseSafeZone::TriggerSaveAsync() const
{
	const UWorld* CurrentWorld = GetWorld();
	if (!IsValid(CurrentWorld)){ return; }
	
	if (USaveSubsystem* SaveSubsystem = CurrentWorld->GetGameInstance()->GetSubsystem<USaveSubsystem>(); IsValid(SaveSubsystem))
	{
		SaveSubsystem->AsyncSaveGame();
	}
}

