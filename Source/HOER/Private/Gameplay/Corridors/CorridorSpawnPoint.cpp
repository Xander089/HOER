// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Corridors/CorridorSpawnPoint.h"

#include "Character/HOERCharacter.h"
#include "Components/BoxComponent.h"
#include "System/Subsystem/CorridorManagerSubsystem.h"

ACorridorSpawnPoint::ACorridorSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->SetBoxExtent(FVector(50.0f, 200.0f, 100.0f));
	RootComponent = TriggerBox;

}

FVector& ACorridorSpawnPoint::GetWaypointVector()
{
	return WaypointLocation;
}

void ACorridorSpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	if (UCorridorManagerSubsystem* CorridorManager = GetWorld()->GetSubsystem<UCorridorManagerSubsystem>(); IsValid(CorridorManager))
	{
		CorridorManager->Register(this);
	}
}

void ACorridorSpawnPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UCorridorManagerSubsystem* CorridorManager = GetWorld()->GetSubsystem<UCorridorManagerSubsystem>(); IsValid(CorridorManager))
	{
		CorridorManager->Unregister(this);
	}
}


void ACorridorSpawnPoint::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!IsValid(Cast<AHOERCharacter>(OtherActor)))
	{
		return;
	}

	UCorridorManagerSubsystem* CorridorManager = GetWorld()->GetSubsystem<UCorridorManagerSubsystem>();
	if (IsValid(CorridorManager))
	{
		CorridorManager->NotifyPlayerReachedSpawnPoint(this);
	}
}
