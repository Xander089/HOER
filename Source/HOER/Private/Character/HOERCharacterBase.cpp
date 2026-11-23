// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/HOERCharacterBase.h"
#include "EnhancedInputComponent.h"
#include "Controller/HOERPlayerController.h"
#include "Data/HOERCharacterSettingsData.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AHOERCharacterBase::AHOERCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AHOERCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	// Apply Data Asset settings once at the start of the game
	ApplyCharacterSettings();
}


void AHOERCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (!CharacterSettings)
		{
			UE_LOG(LogTemp, Warning, TEXT("No Character Settings Data Asset on %s"), *GetName());
			return;
		}
		
		// Bind Movement and Look
		EnhancedInputComponent->BindAction(CharacterSettings->MoveAction, ETriggerEvent::Triggered, this, &AHOERCharacterBase::Move);
		EnhancedInputComponent->BindAction(CharacterSettings->LookAction, ETriggerEvent::Triggered, this, &AHOERCharacterBase::Look);
        
		// Bind Jump
		EnhancedInputComponent->BindAction(CharacterSettings->JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(CharacterSettings->JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Bind Switch Control (Give brain back to human)
		EnhancedInputComponent->BindAction(CharacterSettings->SwitchControlAction, ETriggerEvent::Started, this, &AHOERCharacterBase::SwitchCharacter);

		// Bind Back Action so that the player can request to go back to the previous menu
		EnhancedInputComponent->BindAction(CharacterSettings->BackAction, ETriggerEvent::Started, this, &AHOERCharacterBase::RequestBack);

		// Bind Pause Action so that the player can pause the game
		EnhancedInputComponent->BindAction(CharacterSettings->PauseAction, ETriggerEvent::Started, this, &AHOERCharacterBase::PauseGame);

		EnhancedInputComponent->BindAction(CharacterSettings->MoveAction, ETriggerEvent::Completed, this, &AHOERCharacterBase::StopMove);
		EnhancedInputComponent->BindAction(CharacterSettings->MoveAction, ETriggerEvent::Canceled,  this, &AHOERCharacterBase::StopMove);
		
	}
}

bool AHOERCharacterBase::IsTooFarFromCharacter(const ACharacter* InCharacter, const float MaxDistance) const
{
	if (!IsValid(InCharacter))
	{
		return false; 
	}
	
	// DistSquared to avoid the expensive CPU cost of square roots
	const float CurrentDistanceSquared = FVector::DistSquared(GetActorLocation(), InCharacter->GetActorLocation());
	const float MaxDistanceSquared = FMath::Square(MaxDistance);
	
	return CurrentDistanceSquared > MaxDistanceSquared;
}

void AHOERCharacterBase::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();
	
	if(OnJump.IsBound()){
		OnJump.Broadcast();
	}
}

void AHOERCharacterBase::ApplyCharacterSettings() const
{
	if (CharacterSettings && GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = CharacterSettings->MaxWalkSpeed;
		GetCharacterMovement()->JumpZVelocity = CharacterSettings->JumpZVelocity;
		GetCharacterMovement()->AirControl = CharacterSettings->AirControl;
	}
}

void AHOERCharacterBase::Move(const FInputActionValue& Value)
{
}

void AHOERCharacterBase::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxis = Value.Get<FVector2D>();
	const float Sensitivity = CharacterSettings ? CharacterSettings->LookSensitivity : 1.0f;
    
	AddControllerYawInput(LookAxis.X * Sensitivity);
	AddControllerPitchInput(LookAxis.Y * Sensitivity);
}

void AHOERCharacterBase::SwitchCharacter(const FInputActionValue& Value)
{
	if (OnSwitchCharacter.IsBound())
	{
		OnSwitchCharacter.Broadcast();
	}
}

void AHOERCharacterBase::StopMove(const FInputActionValue& Value)
{
}

void AHOERCharacterBase::RequestBack()
{
	if (const AHOERPlayerController* PC = GetPlayerController(); IsValid(PC))
	{
		PC->RequestBack();
	}
}

void AHOERCharacterBase::PauseGame()
{
	if (AHOERPlayerController* PC = GetPlayerController(); IsValid(PC))
	{
		PC->PauseGame();
	}
}

AHOERPlayerController* AHOERCharacterBase::GetPlayerController() const
{
	return Cast<AHOERPlayerController>(GetController());
}

