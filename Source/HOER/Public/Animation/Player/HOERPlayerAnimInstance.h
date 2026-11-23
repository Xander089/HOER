// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HOERPlayerAnimInstance.generated.h"

class AHOERCharacterBase;
class UCharacterMovementComponent;

/**
 * Animation Instance driving character locomotion BlendSpaces and state transitions (idle, running, jumping, falling).
 */
UCLASS()
class HOER_API UHOERPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// Analogous to BeginPlay. Runs once when the animation starts.
	virtual void NativeInitializeAnimation() override;

	// Analogous to Tick. Runs every frame to update variables.
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	/** Reference to the owning HOER character pawn. */
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TObjectPtr<AHOERCharacterBase> HOERCharacter;

	/** Reference to the character's movement component. */
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	/** Current horizontal speed of the character across the ground plane (in cm/s). */
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;

	/** True if the character has active ground speed exceeding the minimum locomotion threshold. */
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bShouldMove;

	/** True if the character is currently airborne or falling. */
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsFalling;
};
