// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/ObjectInteraction/InteractableObject.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Gameplay/ObjectInteraction/HeldItem.h"
#include "Gameplay/ObjectInteraction/PickupableObject.h"
#include "UI/Widgets/Common/HOERInteractionPromptWidget.h"
#include "GameFramework/Pawn.h"
#include "System/Subsystem/InteractableDebugSubsystem.h"
#include "Engine/World.h"

AInteractableObject::AInteractableObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SphereComponent->SetCollisionProfileName(TEXT("Trigger"));
    SphereComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
    SphereComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    RootComponent = SphereComponent;

    InteractionPromptWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionPromptWidget"));
    InteractionPromptWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    InteractionPromptWidget->SetWidgetSpace(EWidgetSpace::Screen);
    InteractionPromptWidget->SetVisibility(false);
    InteractionPromptWidget->SetupAttachment(SphereComponent);
}

bool AInteractableObject::Interact(AActor* Interactor)
{
    if (!bIsInteractable || !Interactor)
    {
        return false;
    }
    
    SetWasInteractedWith(true);

    // If the list is already empty, the object is fully unlocked
    if (RequiredItems.IsEmpty())
    {
        EmitEvent(Interactor);
        return true;
    }

    // Check if the interactor is holding a valid item
    UHeldItem* HeldItemComponent = Interactor->FindComponentByClass<UHeldItem>();
    if (!HeldItemComponent || !HeldItemComponent->IsHolding())
    {
        EmitFailedEvent(Interactor);
        return false;
    }

    const TSubclassOf<APickupableObject> PlayerItemClass = HeldItemComponent->GetHeldItemClass();
    if (!PlayerItemClass)
    {
        EmitFailedEvent(Interactor);
        return false;
    }

    TSubclassOf<APickupableObject> MatchedClass = nullptr;
    for (const TSubclassOf<APickupableObject>& ReqClass : RequiredItems)
    {
        if (ReqClass && PlayerItemClass->IsChildOf(ReqClass))
        {
            MatchedClass = ReqClass;
            break;
        }
    }

    // Player is holding the wrong item
    if (!MatchedClass)
    {
        EmitFailedEvent(Interactor);
        return false;
    }

    // Consume the item from the player's hand so they can't reuse it and notify Blueprints/listeners
    RequiredItems.Remove(MatchedClass);
    HeldItemComponent->DestroyItem();

    BP_OnItemConsumed(Interactor, this, MatchedClass);
    OnItemConsumed.Broadcast(Interactor, this, MatchedClass);

    // If that was the final required item, trigger full unlock!
    if (RequiredItems.IsEmpty())
    {
        EmitEvent(Interactor);
        return true;
    }

    return false;
}
bool AInteractableObject::DoesInteractionRequireItems() const
{
	return !RequiredItems.IsEmpty();
}
void AInteractableObject::BeginPlay()
{
    Super::BeginPlay();

    if (SphereComponent)
    {
        SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AInteractableObject::OnSphereBeginOverlap);
        SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AInteractableObject::OnSphereEndOverlap);
    }

    // Force widget initialization so GetUserWidgetObject() is valid even while hidden
    if (InteractionPromptWidget)
    {
        InteractionPromptWidget->InitWidget();
        SetPromptText(PromptText);
        InteractionPromptWidget->SetVisibility(false);
    }

    if (UWorld* World = GetWorld())
    {
        if (UInteractableDebugSubsystem* DebugSubsystem = World->GetSubsystem<UInteractableDebugSubsystem>())
        {
            DebugSubsystem->RegisterInteractable(this);
        }
    }

    SetInteractable(bIsInteractable);
}

void AInteractableObject::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UWorld* World = GetWorld())
    {
        if (UInteractableDebugSubsystem* DebugSubsystem = World->GetSubsystem<UInteractableDebugSubsystem>())
        {
            DebugSubsystem->UnregisterInteractable(this);
        }
    }

    Super::EndPlay(EndPlayReason);
}

void AInteractableObject::SetInteractable(bool bNewInteractable)
{
    bIsInteractable = bNewInteractable;

    if (SphereComponent)
    {
        SphereComponent->SetGenerateOverlapEvents(bIsInteractable);
    }

    if (!bIsInteractable && InteractionPromptWidget)
    {
        InteractionPromptWidget->SetVisibility(false);
    }
}

void AInteractableObject::SetPromptText(const FText& NewPromptText)
{
    PromptText = NewPromptText;

    if (InteractionPromptWidget)
    {
        if (UHOERInteractionPromptWidget* PromptUI = Cast<UHOERInteractionPromptWidget>(InteractionPromptWidget->GetUserWidgetObject()))
        {
            PromptUI->SetInteractionPromptText(PromptText);
        }
    }
}

void AInteractableObject::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!bIsInteractable)
    {
        return;
    }

    const APawn* OverlappingPawn = Cast<APawn>(OtherActor);
    if (OverlappingPawn && OverlappingPawn->IsPlayerControlled() && InteractionPromptWidget)
    {
        InteractionPromptWidget->SetVisibility(true);
    }
}

void AInteractableObject::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    const APawn* OverlappingPawn = Cast<APawn>(OtherActor);
    if (OverlappingPawn && OverlappingPawn->IsPlayerControlled() && InteractionPromptWidget)
    {
        InteractionPromptWidget->SetVisibility(false);
    }
}

void AInteractableObject::EmitEvent(AActor* Interactor)
{
    if (bSingleUse)
    {
        SetInteractable(false);
    }

    BP_OnInteractionSuccess(Interactor, this);
    OnInteractionSuccess.Broadcast(Interactor, this);
}

void AInteractableObject::EmitFailedEvent(AActor* Interactor)
{
    BP_OnInteractionFailed(Interactor, this);
    OnInteractionFailed.Broadcast(Interactor, this);
}


bool AInteractableObject::Interact_Implementation(AActor* Interactor)
{
	return Interact(Interactor);
}

bool AInteractableObject::CanInteract_Implementation(AActor* Interactor) const
{
	return IsInteractable();
}

FText AInteractableObject::GetInteractionPrompt_Implementation() const
{
	return PromptText;
}

bool AInteractableObject::WasInteractedWith_Implementation() const
{
    return bWasInteractedWith;
}

void AInteractableObject::SetInteractedWith_Implementation(const bool bInteracted)
{
    SetWasInteractedWith(bInteracted);
}

void AInteractableObject::SetWasInteractedWith(bool bNewWasInteractedWith)
{
    bWasInteractedWith = bNewWasInteractedWith;
}

void AInteractableObject::OnActorSaved()
{
    
}

void AInteractableObject::OnActorLoaded()
{
    
}