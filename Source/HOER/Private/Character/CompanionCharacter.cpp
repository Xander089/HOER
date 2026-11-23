// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CompanionCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"
#include "Audio/CompanionAudioComponent.h"
#include "Character/HOERCharacter.h"
#include "Controller/HOERPlayerController.h"
#include "Data/HOERCharacterSettingsData.h"

// Sets default values
ACompanionCharacter::ACompanionCharacter()
{
	// Don't rotate the character's body when the mouse/camera rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
    
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bRunPhysicsWithNoController = true;
        
		// Tell the character to automatically turn its body to face the direction it's walking
		GetCharacterMovement()->bOrientRotationToMovement = true; 
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // How fast it turns
	}
    
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->InitCapsuleSize(7.0f, 17.0f);
	}
    
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 100.0f; 
	SpringArm->bUsePawnControlRotation = true; // This keeps the camera using the mouse!

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	ThirdPersonCamera->bUsePawnControlRotation = false;
	
	CompanionAudioComponent = CreateDefaultSubobject<UCompanionAudioComponent>(TEXT("AudioComponent"));
}

void ACompanionCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Hook into the movement update, granting us a highly performant "Tick" 
	// that only runs when the physics engine actually moves the character.
	OnCharacterMovementUpdated.AddDynamic(this, &ACompanionCharacter::HandleCompanionMovementUpdated);
}

void ACompanionCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	bHasLanded = true;
    
	if (Controller == nullptr)
	{
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->StopMovementImmediately();
        
			// This completely disables the physics tick from sliding the character.
			// It is mathematically impossible for the character to move now!
			MoveComp->SetMovementMode(MOVE_None); 
		}
	}
}

void ACompanionCharacter::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (Controller == nullptr)
	{
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			// If we bump into something while we are flying through the air...
			if (MoveComp->IsFalling())
			{
				// Kill all forward/side momentum, but KEEP gravity (Z-axis).
				// This makes the companion hit the wall and drop perfectly straight down to the floor.
				MoveComp->Velocity.X = 0.0f;
				MoveComp->Velocity.Y = 0.0f;
			}
		}
	}
}

void ACompanionCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}

}

void ACompanionCharacter::LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride)
{
	Super::LaunchCharacter(LaunchVelocity, bXYOverride, bZOverride);
	
	if(OnLaunch.IsBound())
	{
		OnLaunch.Broadcast();
	}
}

AHOERCharacter* ACompanionCharacter::GetThrower() const
{
	return Thrower.Get();
}

void ACompanionCharacter::SetThrower(AHOERCharacter* InThrower)
{
	Thrower = InThrower;
}


bool ACompanionCharacter::HasLanded() const
{
	return bHasLanded;
}

void ACompanionCharacter::SetHasLanded(const bool bInHasLanded)
{
	this->bHasLanded = bInHasLanded;
}

void ACompanionCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (!Controller) return;

	// Find out which way is forward based on the Camera
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDir, MovementVector.Y);
	AddMovementInput(RightDir, MovementVector.X);
}

void ACompanionCharacter::SwitchCharacter(const FInputActionValue& Value)
{
	Super::SwitchCharacter(Value);
	ReturnToThrower();
}

void ACompanionCharacter::UnPossessed()
{
	Super::UnPossessed();
	
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->SetMovementMode(MOVE_None);
	}
}

void ACompanionCharacter::HandleCompanionMovementUpdated(float DeltaSeconds, FVector OldLocation, FVector OldVelocity)
{
	// Only enforce the leash if the player is currently actively controlling the companion.
	if (!Controller) return;

	if (AHOERCharacter* ThrowerCharacter = Thrower.Get())
	{
		if (CharacterSettings)
		{
			// If we run too far away, automatically snap control back to the human
			if (IsTooFarFromCharacter(ThrowerCharacter, CharacterSettings->MaxCompanionLeashDistance))
			{
				ReturnToThrower();
			}
		}
	}
}

void ACompanionCharacter::ReturnToThrower() const
{
	if (AHOERCharacter* ThrowerCharacter = Thrower.Get())
	{
		if (AHOERPlayerController* PC = GetPlayerController())
		{
			// Transfer the Brain back to the Human
			PC->Possess(ThrowerCharacter);
		}
	}
}

