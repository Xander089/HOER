// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/PanicSystem/PanicEmitterComponent.h"

#include "Misc/DataValidation.h"
#include "System/Subsystem/PanicSystemManager.h"


// Sets default values for this component's properties
UPanicEmitterComponent::UPanicEmitterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPanicEmitterComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (const UWorld* World = GetWorld())
	{
		if (UPanicSystemManager* PanicManager = World->GetSubsystem<UPanicSystemManager>())
		{
			PanicManager->RegisterEmitter(this);
		}
	}
	
}

void UPanicEmitterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const UWorld* World = GetWorld())
	{
		if (UPanicSystemManager* PanicManager = World->GetSubsystem<UPanicSystemManager>())
		{
			PanicManager->UnregisterEmitter(this);
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

float UPanicEmitterComponent::GetMinDistance() const
{
	return MinDistance;
}

void UPanicEmitterComponent::SetMinDistance(float Distance)
{
	this->MinDistance = Distance;
}

float UPanicEmitterComponent::GetOutOfSightDistance() const
{
	return OutOfSightDistance;
}

void UPanicEmitterComponent::SetOutOfSightDistance(float Distance)
{
	this->OutOfSightDistance = Distance;
}

float UPanicEmitterComponent::GetBaseThreat() const
{
	return BaseThreat;
}

void UPanicEmitterComponent::SetBaseThreat(float InBaseThreat)
{
	this->BaseThreat = InBaseThreat;
}

UCurveFloat* UPanicEmitterComponent::GetFalloffCurve() const
{
	return FalloffCurve;
}

void UPanicEmitterComponent::SetFalloffCurve(UCurveFloat* InFalloffCurve)
{
	this->FalloffCurve = InFalloffCurve;
}

#if WITH_EDITOR
EDataValidationResult UPanicEmitterComponent::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (OutOfSightDistance <= MinDistance)
	{
		const FText ErrorMessage = FText::Format(
			FText::FromString(TEXT("Panic Emitter Error: 'Out Of Sight Distance' ({0}) must be greater than 'Min Distance' ({1}).")),
			FText::AsNumber(OutOfSightDistance),
			FText::AsNumber(MinDistance)
		);

		Context.AddError(ErrorMessage);
        
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif