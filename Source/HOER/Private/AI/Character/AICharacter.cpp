// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Character/AICharacter.h"

#include "AI/Controller/AIEnemyController.h"
#include "Audio/EnemyAudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gameplay/PanicSystem/PanicEmitterComponent.h"
#include "Misc/DataValidation.h"



// Sets default values
AAICharacter::AAICharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// Custom AI controller class
	AIControllerClass = AAIEnemyController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	PanicEmitterComponent = CreateDefaultSubobject<UPanicEmitterComponent>(TEXT("PanicEmitterComponent"));
	
	EnemyAudioComponent = CreateDefaultSubobject<UEnemyAudioComponent>(TEXT("AudioComponent"));
}

void AAICharacter::ReactToRiskAwarenessChange(const float& RiskAwareness)
{
	// RiskAwareness is between 0 - 1: remapped to 1 - 10 so that it can match the curve table lookup 
	const float MappedRiskAwareness = FMath::Clamp(FMath::Floor(RiskAwareness * 10.0f + 1.0f), 0.0f, 10.0f);
	
	static const FString ContextString(TEXT("Stats Evaluation Context"));
	
	if (const FRealCurve* RealCurve = GetCurveByEnum(ContextString, EAIStatConfig::MaxWalkSpeed))
	{
		UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
		
		if (IsValid(MovementComponent))
		{
			MovementComponent->MaxWalkSpeed = RealCurve->Eval(MappedRiskAwareness);
		}
	}

	if (const FRealCurve* RealCurve = GetCurveByEnum(ContextString, EAIStatConfig::SwitchLightOffRadius))
	{
		SwitchLightOffRadius = RealCurve->Eval(MappedRiskAwareness);
	}
}

void AAICharacter::OnActorSaved()
{
	ISavableActorInterface::OnActorSaved();
}

void AAICharacter::OnActorLoaded()
{
	ISavableActorInterface::OnActorLoaded();
}

FRealCurve* AAICharacter::GetCurveByEnum(const FString& Context, const EAIStatConfig& EnumValue)
{
	if (const FCurveTableRowHandle* Handle = StatCurveHandles.Find(EnumValue))
	{
		if (FRealCurve* Curve = Handle->GetCurve(Context))
		{
			return Curve;
		}
	}
	
	return nullptr;
}

#if WITH_EDITOR
EDataValidationResult AAICharacter::IsDataValid(class FDataValidationContext& Context) const
{
	// Call the Super implementation
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	// check whether AIControllerClass is set
	if (AIControllerClass == nullptr)
	{

		Context.AddError(FText::FromString(TEXT("AI Controller Blueprint Reference in property 'AIControllerClass' is not set: please assign a valid AI Controller.")));
		Result = EDataValidationResult::Invalid;
        
	}

	return Result;
}
#endif

// Called when the game starts or when spawned
void AAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}