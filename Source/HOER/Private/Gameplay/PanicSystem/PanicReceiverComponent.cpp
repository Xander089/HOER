// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/PanicSystem/PanicReceiverComponent.h"

#include "Data/PanicSystem/UPanicProfileDataAsset.h"
#include "Misc/DataValidation.h"
#include "System/Subsystem/PanicSystemManager.h"

// Sets default values for this component's properties
UPanicReceiverComponent::UPanicReceiverComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPanicReceiverComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (const UWorld* World = GetWorld())
	{
		if (UPanicSystemManager* PanicManager = World->GetSubsystem<UPanicSystemManager>())
		{
			PanicManager->RegisterReceiver(this);
		}
	}
}

void UPanicReceiverComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const UWorld* World = GetWorld())
	{
		if (UPanicSystemManager* PanicManager = World->GetSubsystem<UPanicSystemManager>())
		{
			PanicManager->UnregisterReceiver(this);
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

float UPanicReceiverComponent::GetCurrentPanicLevel() const
{
	return CurrentPanicLevel;
}

void UPanicReceiverComponent::SetCurrentPanicLevel(float InCurrentPanicLevel)
{
	this->CurrentPanicLevel = InCurrentPanicLevel;
	OnPanicValueChanged.Broadcast(CurrentPanicLevel);
	
	if (PanicProfile)
	{
		if (FPanicStateDefinition EvaluatedState; PanicProfile->GetStateForLevel(CurrentPanicLevel, EvaluatedState))
		{
			if (EvaluatedState.StateName != CurrentStateName)
			{
				CurrentStateName = EvaluatedState.StateName;
                
				OnPanicStateChanged.Broadcast(EvaluatedState, CurrentPanicLevel);
			}
		}
	}
}

bool UPanicReceiverComponent::CanSeeActor(const AActor* TargetActor) const
{
    if (!TargetActor) return false;
    
    const AActor* Owner = GetOwner();
    if (!Owner) return false;

    const FVector StartLocation = GetSightLocation();

    // Get the target's collision bounds
    FVector TargetOrigin;
    FVector TargetBoxExtent;
    // true = only consider components with collision enabled
    TargetActor->GetActorBounds(true, TargetOrigin, TargetBoxExtent);

    // Check Field of View (Cone of Vision) against the center of the target
    if (bUseConeOfVision)
    {
       const FVector DirectionToTarget = (TargetOrigin - StartLocation).GetSafeNormal();
       const FVector ForwardVector = Owner->GetActorForwardVector();

       // Calculate the threshold for our field of view using Cosine
       const float MinDotProduct = FMath::Cos(FMath::DegreesToRadians(FieldOfViewDegrees / 2.0f));

       // If the center of the target is outside our cone of vision, we can't see them
       if (const float CurrentDotProduct = FVector::DotProduct(ForwardVector, DirectionToTarget); CurrentDotProduct < MinDotProduct)
       {
          return false;
       }
    }

    // Define the points on the target's body to check
    // We multiply the Z extent by 0.8f to avoid tracing at the extreme edge, which often clips floors/ceilings
    const FVector Center = TargetOrigin;
    const FVector Top = TargetOrigin + FVector(0.f, 0.f, TargetBoxExtent.Z * 0.8f);
    const FVector Bottom = TargetOrigin - FVector(0.f, 0.f, TargetBoxExtent.Z * 0.8f);

    // TInlineAllocator<3> prevents expensive heap allocations for this small, fixed-size array
    TArray<FVector, TInlineAllocator<3>> PointsToCheck = { Center, Top, Bottom };

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Owner);
    QueryParams.AddIgnoredActor(TargetActor);

    FHitResult HitResult;

    for (const FVector& Point : PointsToCheck)
    {
        const bool bHit = GetWorld()->LineTraceSingleByChannel(
           HitResult,
           StartLocation,
           Point,
           SightCollisionChannel,
           QueryParams
        );

        // If we hit NOTHING, it means the ray successfully reached the point on the target without hitting a wall.
        // We have Line of Sight!
        if (!bHit)
        {
            return true;
        }
    }

    // If the loop finishes and every single point hit a wall, they are completely hidden
    return false;
}

FVector UPanicReceiverComponent::GetSightLocation() const
{
	const AActor* Owner = GetOwner();
	if (!Owner) return FVector::ZeroVector;

	// If we provided a socket name, try to find a Skeletal Mesh and get the socket's location
	if (EyeSocketName != NAME_None)
	{
		if (const USkeletalMeshComponent* Mesh = Owner->FindComponentByClass<USkeletalMeshComponent>())
		{
			if (Mesh->DoesSocketExist(EyeSocketName))
			{
				return Mesh->GetSocketLocation(EyeSocketName);
			}
		}
	}

	// Fallback: Just return the center of the Actor
	return Owner->GetActorLocation();
}

float UPanicReceiverComponent::GetDecayRate() const
{
	return DecayRate;
}


float UPanicReceiverComponent::GetMinPanicLevel() const
{
	return this->PanicProfile->GetMinPanicLevel();
}

float UPanicReceiverComponent::GetMaxPanicLevel() const
{
	return this->PanicProfile->GetMaxPanicLevel();
}

#if WITH_EDITOR
EDataValidationResult UPanicReceiverComponent::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (EyeSocketName != NAME_None)
	{
		if (const AActor* Owner = GetOwner())
		{
			if (const USkeletalMeshComponent* Mesh = Owner->FindComponentByClass<USkeletalMeshComponent>(); !Mesh)
			{
				Context.AddError(FText::FromString("Receiver must be attached to an Actor with a Skeletal Mesh."));
				Result = EDataValidationResult::Invalid;
			}
			else if (!Mesh->DoesSocketExist(EyeSocketName))
			{
				Context.AddError(FText::FromString("Invalid Eye Socket Name."));
				Result = EDataValidationResult::Invalid;
			}
		}
	}

	return Result;
}
#endif