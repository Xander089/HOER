// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/ObjectInteraction/PlaceableArea.h"

#include "Gameplay/ObjectInteraction/HeldItem.h"
#include "Gameplay/ObjectInteraction/PickupableObject.h"


class UHeldItem;
// Sets default values
APlaceableArea::APlaceableArea()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);
}

bool APlaceableArea::Interact(AActor* Interactor)
{
	if (bIsInteractable && Interactor && StaticMesh)
	{
		UHeldItem* HeldItemComponent = Interactor->FindComponentByClass<UHeldItem>();
		if (!HeldItemComponent)
		{
			return false;
		}
		
		if (StaticMesh->IsVisible())
		{
			if (HeldItemComponent->IsHolding())
			{
				return false;
			}
			HeldItemComponent->PickItem(PickupableObject, StaticMesh->GetStaticMesh());
			HideMesh();
			EmitItemRemovedEvent(Interactor);
			return true;
		} else
		{
			// Check if the classes match (HeldItemClass is null when the player's hands are empty)
			const TSubclassOf<APickupableObject> HeldClass = HeldItemComponent->GetHeldItemClass();
			if (PickupableObject && HeldItemComponent->IsHolding() && HeldClass && HeldClass->IsChildOf(PickupableObject))
			{
				HeldItemComponent->DestroyItem(); 
				ShowMesh();
				EmitItemPlacedEvent(Interactor);
				return true;
			}
		}
		
	}
	
	return false;
}

void APlaceableArea::BeginPlay()
{
	Super::BeginPlay();
	
	HideMesh();
}

void APlaceableArea::HideMesh() const
{
	StaticMesh->SetVisibility(false);
}

void APlaceableArea::ShowMesh() const
{
	StaticMesh->SetVisibility(true);
}

void APlaceableArea::EmitItemPlacedEvent(AActor* Interactor)
{
	SetUnlocked(true);
	EmitEvent(Interactor);
	BP_OnItemPlaced(Interactor, this, PickupableObject);
	OnItemPlaced.Broadcast(Interactor, this, PickupableObject);
}

void APlaceableArea::EmitItemRemovedEvent(AActor* Interactor)
{
	SetUnlocked(false);
	EmitEvent(Interactor);
	BP_OnItemRemoved(Interactor, this, PickupableObject);
	OnItemRemoved.Broadcast(Interactor, this, PickupableObject);
}

bool APlaceableArea::IsUnlocked_Implementation() const
{
	return bIsUnlocked;
}

void APlaceableArea::Unlock_Implementation(const bool bUnlock)
{
	bIsUnlocked = bUnlock;
	ShowMesh();
}

void APlaceableArea::SetUnlocked(const bool bInUnlocked)
{
	bIsUnlocked = bInUnlocked;
}

FName APlaceableArea::GetKeyName_Implementation() const
{
	return KeyName;
}
