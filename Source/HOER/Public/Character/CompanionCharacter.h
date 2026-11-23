// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HOERCharacterBase.h"
#include "CompanionCharacter.generated.h"

class UCompanionAudioComponent;
struct FInputActionValue;
class AHOERCharacter;
class UCameraComponent;
class USpringArmComponent;

/** Broadcast when the companion character is launched into the air. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLaunch);

/**
 * Controllable companion plush character that can be thrown into tight spaces,
 * navigated via third-person perspective, and recalled back to the main character.
 */
UCLASS()
class HOER_API ACompanionCharacter : public AHOERCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACompanionCharacter();
	virtual void BeginPlay() override;
	
	virtual void Landed(const FHitResult& Hit) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride) override;
	
	/** Returns the thrower character (main player) who launched this companion. */
	AHOERCharacter* GetThrower() const;

	/** Sets the reference to the thrower character. */
	void SetThrower(AHOERCharacter* InThrower);

	/** Checks whether the companion has touched down after being thrown. */
	bool HasLanded() const;

	/** Sets landing status flag. */
	void SetHasLanded(const bool bInHasLanded);
	
	virtual void UnPossessed() override;
	
	/** Broadcast when launched. */
	FOnLaunch OnLaunch;

protected:
	
	/** Spring arm component governing third-person camera distance and pitch. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	/** Third-person follow camera component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> ThirdPersonCamera;
	
	/** Audio component playing companion voice cues, squeaks, and collision impacts. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCompanionAudioComponent> CompanionAudioComponent = nullptr;

	virtual void Move(const FInputActionValue& Value) override;
	virtual void SwitchCharacter(const FInputActionValue& Value) override;
	
	UFUNCTION()
	void HandleCompanionMovementUpdated(float DeltaSeconds, FVector OldLocation, FVector OldVelocity);
    
	void ReturnToThrower() const;
	
private:
	UPROPERTY()
	TWeakObjectPtr<AHOERCharacter> Thrower;
	
	bool bHasLanded = false;
};
