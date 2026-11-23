// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/ObjectInteraction/PushableObject.h"

#include "Audio/PropAudioComponent.h"
#include "Gameplay/ObjectInteraction/GrabAbilityComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/Widgets/Common/HOERInteractionPromptWidget.h"

static TAutoConsoleVariable<int32> CVarDebugPushableObjects(
    TEXT("HOER.DebugPushableObjects"),
    0,
    TEXT("Draws debug spheres for the Pushable Objects' interaction field.\n")
    TEXT("0: Off\n")
    TEXT("1: On"),
    ECVF_Cheat
);

APushableObject::APushableObject()
{
    PrimaryActorTick.bCanEverTick = false;
    
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetMobility(EComponentMobility::Movable);
    StaticMesh->SetupAttachment(RootComponent);
    
    PropAudioComponent = CreateDefaultSubobject<UPropAudioComponent>(TEXT("AudioComponent"));
}

void APushableObject::BeginPlay()
{
    Super::BeginPlay();
}

void APushableObject::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (bIsGrabbed)
    {
        StopPushing();
    }
    Super::EndPlay(EndPlayReason);
}

bool APushableObject::Interact(AActor* Interactor)
{
    if (!bIsGrabbed)
    {
        // Only block new grabs: releasing must always work, even if bSingleUse disabled us on grab
        if (!bIsInteractable) return false;

        CurrentInteractor = Cast<ACharacter>(Interactor);
        if (!CurrentInteractor) return false;

        const FVector ForwardVector = CurrentInteractor->GetActorForwardVector();
        FVector SnappedDirection = FVector::ZeroVector;

        if (FMath::Abs(ForwardVector.X) > FMath::Abs(ForwardVector.Y))
        {
            SnappedDirection.X = FMath::Sign(ForwardVector.X);
        }
        else
        {
            SnappedDirection.Y = FMath::Sign(ForwardVector.Y);
        }
        
        PushableDirection = SnappedDirection;

        if (CanGrab())
        {
           bIsGrabbed = true;
           
           if (UGrabAbilityComponent* GrabAbility = CurrentInteractor->FindComponentByClass<UGrabAbilityComponent>())
           {
               GrabAbility->Grab(this);
           }
           
           CurrentInteractor->GetCharacterMovement()->DisableMovement();
           CurrentInteractor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
           
           if (const UHOERInteractionPromptWidget* PromptUI = Cast<UHOERInteractionPromptWidget>(InteractionPromptWidget->GetWidget()))
           {
               PromptUI->SetInteractionPromptText(FText::FromString(TEXT("Press X To Let Go")));
           }
            
            EmitPushStartEvent(Interactor);
            SetWasInteractedWith(true);
            return true;
        }
        
        CurrentInteractor = nullptr;
        return false;
    }
    else
    {
        StopPushing();
        EmitPushEndEvent(Interactor);
    }
    
    return true;
}

void APushableObject::ApplyPushInput(const FVector2D& MoveInput)
{
    // If we aren't moving, do nothing
    if (!bIsGrabbed || !CurrentInteractor) return;

    if (MoveInput.IsNearlyZero())
    {
        OnStopPushing();
        return;
    }

    const float DeltaTime = GetWorld()->GetDeltaSeconds();
    
    const FVector RightDirection = FVector::CrossProduct(FVector::UpVector, PushableDirection);
    
    const FVector ForwardDelta = PushableDirection * MoveInput.Y;
    const FVector RightDelta = RightDirection * MoveInput.X;
    
    const FVector MoveDelta = (ForwardDelta + RightDelta) * PushSpeed * DeltaTime;
    
    // Get the true geometric center and size directly from the Static Mesh
    const FVector Center = StaticMesh->Bounds.Origin;
    // Start at the true center, but lift it 5 units up so it ignores the floor
    const FVector StartLocation = Center + FVector(0.0f, 0.0f, 5.0f);
    const FVector NextLocation = StartLocation + MoveDelta;
    
    // Check Restrictions
    if (IsFloorMissing(NextLocation) || IsObjectOnTop(NextLocation))
    {
        OnStopPushing();
        return;
    }

    // The root is the overlap-only trigger sphere, so sweeping the actor never hits walls.
    // Sweep the mesh's box shape ourselves before moving.
    FCollisionQueryParams WallQueryParams;
    WallQueryParams.AddIgnoredActor(this);
    WallQueryParams.AddIgnoredActor(CurrentInteractor);

    FHitResult Hit;
    const bool bHitWall = GetWorld()->SweepSingleByChannel(Hit, StartLocation, NextLocation, GetActorQuat(),
        ECC_Visibility, FCollisionShape::MakeBox(StaticMesh->Bounds.BoxExtent * 0.95f), WallQueryParams);

    if (bHitWall)
    {
        OnStopPushing();
        return;
    }

    AddActorWorldOffset(MoveDelta, false);

    if (!bIsBeingPushed)
    {
        OnStartPushing();
    }
}

void APushableObject::StopPushing()
{
    if (!bIsGrabbed && !CurrentInteractor)
    {
        return;
    }

    bIsGrabbed = false;

    if (CurrentInteractor)
    {
        ACharacter* InteractorToReset = CurrentInteractor;
        CurrentInteractor = nullptr;

        if (UGrabAbilityComponent* GrabAbility = InteractorToReset->FindComponentByClass<UGrabAbilityComponent>())
        {
            if (GrabAbility->GetGrabbedObject() == this)
            {
                GrabAbility->ReleaseGrab();
            }
        }
        
        if (UCharacterMovementComponent* MovementComp = InteractorToReset->GetCharacterMovement())
        {
            MovementComp->SetMovementMode(MOVE_Walking);
        }
        InteractorToReset->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    }

    if (const UHOERInteractionPromptWidget* PromptUI = Cast<UHOERInteractionPromptWidget>(InteractionPromptWidget->GetWidget()))
    {
       PromptUI->SetInteractionPromptText(PromptText);
    }

    OnStopPushing();
}

void APushableObject::EmitPushStartEvent(AActor* Interactor)
{
    EmitEvent(Interactor);
    BP_OnPushStart(Interactor, this);
    OnPushStart.Broadcast(Interactor, this);
}

void APushableObject::EmitPushEndEvent(AActor* Interactor)
{
    EmitEvent(Interactor);
    BP_OnPushEnd(Interactor, this);
    OnPushEnd.Broadcast(Interactor, this);
}

// Just a simple check to ensure the player is facing the block before grabbing
bool APushableObject::CanGrab() const
{
    if (!CurrentInteractor || CurrentInteractor->GetCharacterMovement()->IsFalling()) return false;
    
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(CurrentInteractor);
    FHitResult HitResult;
    
    const FVector PlayerLocation = CurrentInteractor->GetActorLocation();
    const FVector EndLocation = PlayerLocation + (CurrentInteractor->GetActorForwardVector() * 100.0f);
    
    const FCollisionShape SphereShape = FCollisionShape::MakeSphere(CanGrabTraceRadius);
    
    const bool bHitSomething = GetWorld()->SweepSingleByChannel(HitResult, PlayerLocation, EndLocation, FQuat::Identity, ECC_Visibility, SphereShape, QueryParams);
    
    if (CVarDebugPushableObjects.GetValueOnGameThread() > 0)
    {
        const FColor DebugColor = (bHitSomething && HitResult.GetActor() == this) ? FColor::Green : FColor::Red;
        
        // Draw the thick sphere trace capsule
        DrawDebugCapsule(GetWorld(), 
            PlayerLocation + (EndLocation - PlayerLocation) * 0.5f, // Center point
            (EndLocation - PlayerLocation).Size() * 0.5f,           // Half height
            CanGrabTraceRadius,                                     // Radius
            FRotationMatrix::MakeFromZ(CurrentInteractor->GetActorForwardVector()).ToQuat(), 
            DebugColor, false, 2.0f);
    }
    
    return (bHitSomething && HitResult.GetActor() == this);
}

bool APushableObject::IsFloorMissing(const FVector& NextLocation) const
{
    const FVector Extents = StaticMesh->Bounds.BoxExtent;
    
    // Calculate how far down we need to trace to find the floor.
    // NextLocation is 5 units above the center. So we go down half the height + 20 units for safety.
    const float TraceDepth = Extents.Z + 20.0f; 
    
    // Shrink the corners by 5% so we don't accidentally stop if a corner perfectly scrapes a wall
    const float OffsetX = Extents.X * 0.95f;
    const float OffsetY = Extents.Y * 0.95f;

    // Define the 4 corners in Local Space (Forward/Right, Forward/Left, Back/Right, Back/Left)
    const FVector LocalCorners[4] = {
        FVector(OffsetX, OffsetY, 0.0f),
        FVector(OffsetX, -OffsetY, 0.0f),
        FVector(-OffsetX, OffsetY, 0.0f),
        FVector(-OffsetX, -OffsetY, 0.0f)
    };

    // Get the Block's current rotation so the corners rotate with it
    const FTransform ActorTransform = GetActorTransform();
    
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    if (CurrentInteractor) QueryParams.AddIgnoredActor(CurrentInteractor);

    // Fire 4 individual line traces
    for (int32 i = 0; i < 4; i++)
    {
        // Rotate the local corner offsets to match the block's world rotation
        const FVector WorldOffset = ActorTransform.TransformVector(LocalCorners[i]);
        
        const FVector TraceStart = NextLocation + WorldOffset;
        const FVector TraceEnd = TraceStart - FVector(0.0f, 0.0f, TraceDepth);
        
        FHitResult HitResult;
        const bool bHitFloor = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
        
        if (CVarDebugPushableObjects.GetValueOnGameThread() > 0)
        {
            const FColor DebugColor = bHitFloor ? FColor::Green : FColor::Red;
            DrawDebugLine(GetWorld(), TraceStart, TraceEnd, DebugColor, false, 0.0f, 0, 2.0f);
        }
        
        // If even ONE corner misses the floor, the floor is missing! Stop moving!
        if (!bHitFloor)
        {
            return true; 
        }
    }

    // If we survived the loop, all 4 corners have solid ground underneath them.
    return false;
}

bool APushableObject::IsObjectOnTop(const FVector& StartLocation) const
{
    const FQuat Rotation = GetActorRotation().Quaternion();
    const FVector Extents = StaticMesh->Bounds.BoxExtent;
    const FVector SweepExtents = Extents * 0.95f;
    const FVector StaticMeshFullHeight = FVector(0.0f, 0.0f, 2 * Extents.Z);
    
    const FVector TraceEnd = StartLocation + StaticMeshFullHeight;
    
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    if (CurrentInteractor) QueryParams.AddIgnoredActor(CurrentInteractor);
             
    const bool bIsObjectOnTop = GetWorld()->SweepSingleByChannel(HitResult, StartLocation, TraceEnd, Rotation, ECC_Visibility, FCollisionShape::MakeBox(SweepExtents), QueryParams);

    if (CVarDebugPushableObjects.GetValueOnGameThread() > 0)
    {
        const FColor DebugColor = !bIsObjectOnTop ? FColor::Green : FColor::Red;
        DrawDebugBox(GetWorld(), TraceEnd, SweepExtents, GetActorRotation().Quaternion(), DebugColor, false, 0.0f);
        DrawDebugLine(GetWorld(), StartLocation, TraceEnd, DebugColor, false, 0.0f, 0, 2.0f);
    }
    
    return bIsObjectOnTop;
}

void APushableObject::OnStartPushing()
{
    if (!bIsBeingPushed)
    {
        bIsBeingPushed = true;

        if(IsValid(PropAudioComponent)){
            PropAudioComponent->Play(0.0f);
        }
    }
    
}

void APushableObject::OnStopPushing()
{
    if (bIsBeingPushed)
    {
        bIsBeingPushed = false;
        if(PropAudioComponent){
            PropAudioComponent->StopSoundEventFadeOut();
        }
    }
}


bool APushableObject::CanInteract_Implementation(AActor* Interactor) const
{
	if (!bIsGrabbed && !bIsInteractable)
	{
		return false;
	}

	return true;
}
