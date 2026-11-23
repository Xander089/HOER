// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/ObjectInteraction/InteractionComponent.h"
#include "Gameplay/ObjectInteraction/HeldItem.h"
#include "Gameplay/ObjectInteraction/PushableObject.h"
#include "Gameplay/ObjectInteraction/InteractableInterface.h"
#include "GameFramework/Actor.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		HeldItemComponent = Owner->FindComponentByClass<UHeldItem>();
	}
}

void UInteractionComponent::Interact()
{
	PerformInteraction();
}

bool UInteractionComponent::PerformInteraction()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	if (!HeldItemComponent)
	{
		HeldItemComponent = Owner->FindComponentByClass<UHeldItem>();
	}

	const bool bIsHoldingItem = HeldItemComponent && HeldItemComponent->IsHolding();
	AActor* ClosestInteractable = FindBestInteractable();
	CurrentInteractable = ClosestInteractable;

	if (ClosestInteractable)
	{
		// Cannot push heavy objects while holding an item in hand
		if (bIsHoldingItem && ClosestInteractable->IsA(APushableObject::StaticClass()))
		{
			return false;
		}

		OnInteractionAttempted.Broadcast(ClosestInteractable);

		const bool bSuccess = IInteractableInterface::Execute_Interact(ClosestInteractable, Owner);
		if (bSuccess)
		{
			OnInteractionSuccess.Broadcast(ClosestInteractable);
		}
		else
		{
			OnInteractionFailed.Broadcast(ClosestInteractable);
		}

		return bSuccess;
	}
	else if (bIsHoldingItem && bDropHeldItemIfNoInteractable)
	{
		HeldItemComponent->DropItem();
		OnHeldItemDropped.Broadcast();
		return true;
	}

	return false;
}

AActor* UInteractionComponent::FindBestInteractable() const
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}

	TArray<AActor*> OverlappingActors;
	Owner->GetOverlappingActors(OverlappingActors);

	AActor* ClosestInteractable = nullptr;
	float ClosestDistanceSq = -1.0f;
	const FVector OwnerLocation = Owner->GetActorLocation();

	for (AActor* Actor : OverlappingActors)
	{
		if (!IsValid(Actor) || Actor == Owner)
		{
			continue;
		}

		if (!CanInteractWith(Actor))
		{
			continue;
		}

		const float DistanceSq = FVector::DistSquared(OwnerLocation, Actor->GetActorLocation());
		if (!ClosestInteractable || DistanceSq < ClosestDistanceSq)
		{
			ClosestDistanceSq = DistanceSq;
			ClosestInteractable = Actor;
		}
	}

	return ClosestInteractable;
}

bool UInteractionComponent::CanInteractWith(AActor* TargetActor) const
{
	if (!IsValid(TargetActor))
	{
		return false;
	}

	if (!TargetActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		return false;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	return IInteractableInterface::Execute_CanInteract(TargetActor, Owner);
}
