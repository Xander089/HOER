// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/ObjectInteraction/PickupableObject.h"

#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Gameplay/ObjectInteraction/HeldItem.h"
#include "Save/SaveSubsystem.h"

// Sets default values
APickupableObject::APickupableObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	
	// MAKE THE MESH THE ROOT! Now the whole Actor moves when physics pushes the mesh.
	SetRootComponent(StaticMesh);
    
	// Attach the inherited components TO the Static Mesh
	if (SphereComponent)
	{
		SphereComponent->SetupAttachment(StaticMesh);
	}
	if (InteractionPromptWidget)
	{
		InteractionPromptWidget->SetupAttachment(StaticMesh);
	}
	
	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->BodyInstance.bOverrideMass = true;
	StaticMesh->BodyInstance.SetMassOverride(DefaultMassOverride, true);
}

bool APickupableObject::Interact(AActor* Interactor)
{
	if (bIsInteractable && Interactor && StaticMesh && StaticMesh->GetStaticMesh())
	{
		if (UHeldItem* HeldItemComponent = Interactor->FindComponentByClass<UHeldItem>())
		{
			if (HeldItemComponent->IsHolding())
			{
				return false; // Already holding an item
			}
			
			HeldItemComponent->PickItem(this->GetClass(), StaticMesh->GetStaticMesh());
			
			// Fire events BEFORE Destroy() so 'this' is still valid for Blueprint listeners
			EmitPickupEvent(Interactor);
			this->Destroy();
			return true;
		}
	}
	
	return false;
}

void APickupableObject::EmitPickupEvent(AActor* Interactor)
{
	EmitEvent(Interactor);
	BP_OnPickupSuccess(Interactor, this);
	OnPickupSuccess.Broadcast(Interactor, this);
}

FString APickupableObject::GetSaveLabel() const
{
	return SaveLabel.IsEmpty() ? GetName() : SaveLabel;
}

void APickupableObject::BeginPlay()
{
	Super::BeginPlay();
	
	if (const UWorld* CurrentWorld = GetWorld(); IsValid(CurrentWorld))
	{
		if (USaveSubsystem* SaveSubsystem = CurrentWorld->GetGameInstance()->GetSubsystem<USaveSubsystem>(); IsValid(SaveSubsystem))
		{
			SaveSubsystem->Register(this);
		}
	}
}

void APickupableObject::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const UWorld* CurrentWorld = GetWorld(); IsValid(CurrentWorld))
	{
		if (USaveSubsystem* SaveSubsystem = CurrentWorld->GetGameInstance()->GetSubsystem<USaveSubsystem>(); IsValid(SaveSubsystem))
		{
			SaveSubsystem->Unregister(this);
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

