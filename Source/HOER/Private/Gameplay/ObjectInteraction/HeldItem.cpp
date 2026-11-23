// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/ObjectInteraction/HeldItem.h"

#include "Gameplay/ObjectInteraction/PickupableObject.h"

// Sets default values for this component's properties
UHeldItem::UHeldItem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	UStaticMeshComponent::SetCollisionProfileName(TEXT("NoCollision"));
}

void UHeldItem::PickItem(const TSubclassOf<APickupableObject> PickupableObjectClass, UStaticMesh* NewMesh)
{
	if (NewMesh)
	{
		SetStaticMesh(NewMesh);
		bIsHolding = true;
		HeldItemClass = PickupableObjectClass;
		EmitItemPickupEvent();
	}
}

void UHeldItem::DropItem()
{
	if (HeldItemClass)
	{
		const FTransform SpawnTransform = GetComponentTransform();
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		// Spawn the actor in the world using the class and transform
		APickupableObject* PickupableObject = GetWorld()->SpawnActor<APickupableObject>(HeldItemClass, SpawnTransform, SpawnParams);
		if (!PickupableObject)
		{
			return;
		}
		
		
		EmitItemDropEvent(PickupableObject);

		DestroyItem();
	}
}

void UHeldItem::DestroyItem()
{
	// Clear the mesh and reset state
	SetStaticMesh(nullptr);
	HeldItemClass = nullptr;
	bIsHolding = false;
}

bool UHeldItem::IsHolding() const
{
	return bIsHolding;
}

void UHeldItem::SetIsHolding(const bool bInIsHolding)
{
	this->bIsHolding = bInIsHolding;
}

TSubclassOf<APickupableObject> UHeldItem::GetHeldItemClass() const
{
	return HeldItemClass;
}

void UHeldItem::SetHeldItemClass(const TSubclassOf<APickupableObject>& InHeldItemClass)
{
	this->HeldItemClass = InHeldItemClass;
}

void UHeldItem::EmitItemPickupEvent()
{
	BP_OnItemPickup(HeldItemClass);
	OnItemPickup.Broadcast(HeldItemClass);
}

void UHeldItem::EmitItemDropEvent(APickupableObject* PickupableObject)
{
	BP_OnItemDrop(HeldItemClass, PickupableObject);
	OnItemDrop.Broadcast(HeldItemClass, PickupableObject);
}
