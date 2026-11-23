// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Player/HOERPlayerAnimInstance.h"
#include "Character/HOERCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UHOERPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	if (APawn* OwnerPawn = TryGetPawnOwner())
	{
		HOERCharacter = Cast<AHOERCharacterBase>(OwnerPawn);
		if (HOERCharacter)
		{
			MovementComponent = HOERCharacter->GetCharacterMovement();
		}
	}
}

void UHOERPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!HOERCharacter || !MovementComponent) return;

	// Calculate Speed (ignoring Z velocity for walking/running)
	FVector Velocity = HOERCharacter->GetVelocity();
	Velocity.Z = 0.0f;
	GroundSpeed = Velocity.Size();

	// Check if falling
	bIsFalling = MovementComponent->IsFalling();

	// Determine if the character should be playing moving animations.
	// (They might have speed because they are falling, so we check acceleration too)
	bool bHasInput = MovementComponent->GetCurrentAcceleration().Size() > 0.0f;
	bShouldMove = (GroundSpeed > 3.0f) && bHasInput;
}
