// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HOERCharacterBase.generated.h"

class UHOERCharacterSettingsData;
struct FInputActionValue;
class AHOERPlayerController;

/** Broadcast when the character jumps off the ground. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJumping);

/** Broadcast when player switches control between the main character and companion. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchCharacter);

/**
 * Base character class in HOER providing shared input routing, movement parameter application,
 * and character swapping foundations.
 */
UCLASS()
class HOER_API AHOERCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AHOERCharacterBase();
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	/**
	 * Checks whether this character exceeds a given spatial distance from another character.
	 *
	 * @param InCharacter The target character to measure against.
	 * @param MaxDistance Distance threshold in Unreal units.
	 * @return True if distance is greater than MaxDistance, false otherwise.
	 */
	bool IsTooFarFromCharacter(const ACharacter* InCharacter, const float MaxDistance) const;

	/** Delegate triggered upon jumping. */
	FOnJumping OnJump;
	
	/** Broadcast when swapping active control to another character. */
	UPROPERTY(BlueprintAssignable, Category= "Events")
	FOnSwitchCharacter OnSwitchCharacter;

	/** Engine event called when character successfully jumps off the ground. */
	virtual void OnJumped_Implementation() override;

protected:
	/** Applies speed, acceleration, and jump metrics from CharacterSettings data asset. */
	void ApplyCharacterSettings() const;
	
	/** Data asset containing movement speeds, gravity scale, and jump velocity parameters. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UHOERCharacterSettingsData> CharacterSettings;
	
	virtual void Move(const FInputActionValue& Value);
	virtual void Look(const FInputActionValue& Value);
	virtual void SwitchCharacter(const FInputActionValue& Value);

	// Called when the Move input is released or canceled, i.e. Completed / Canceled trigger events.
	virtual void StopMove(const FInputActionValue& Value);
	
	void RequestBack();
	void PauseGame();
	
	AHOERPlayerController* GetPlayerController() const;
};
