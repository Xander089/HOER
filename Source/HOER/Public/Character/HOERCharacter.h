// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HOERCharacterBase.h"
#include "Save/SavableActorInterface.h"
#include "HOERCharacter.generated.h"

class UHeldItem;
class UPostProcessComponent;
class UPanicReceiverComponent;
class UInputMappingContext;
class AHOERPlayerController;
struct FInputActionValue;
class UCameraComponent;
class UCharacterAudioComponent;
class UHOERCameraSettingsData;
class USpringArmComponent;
class UGrabAbilityComponent;
struct FInputActionInstance;
class ACompanionCharacter;
class UInteractionComponent;

/**
 * High-level locomotion and movement state of the primary player character.
 */
UENUM(BlueprintType, meta = (ScriptName = "HOERPlayerState"))
enum class EPlayerState : uint8
{
	/** Character is stationary on ground. */
	Idle,

	/** Character is currently moving / traversing ground. */
	Moving,

	/** Character is airborne or falling. */
	Falling
};

/** Broadcast when the player character transitions from stationary to moving. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartMoving);

/** Broadcast when the player character comes to a complete halt. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopMoving);

/** Broadcast when the companion plush character is launched/thrown. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnThrowing);

/** Broadcast when the player begins grabbing a pushable puzzle actor. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrabbing);

/**
 * Primary player character class for HOER, featuring first-person navigation,
 * object interaction/grabbing, companion plush throwing with trajectory preview, and panic system integration.
 */
UCLASS()
class HOER_API AHOERCharacter : public AHOERCharacterBase, public ISavableActorInterface
{
	GENERATED_BODY()

public:
	
	AHOERCharacter();
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	/** Broadcast when the character starts moving. */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStartMoving OnStartMoving;

	/** Broadcast when the character stops moving. */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStopMoving OnStopMoving;
	
	/** Broadcast when launching the companion character. */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnThrowing OnThrowing;
	
	/** Broadcast when grabbing a pushable object. */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGrabbing OnGrabbing;
	
	// Savable Interface Methods
	virtual void OnActorSaved() override;
	virtual void OnActorLoaded() override;
	
	/** Checks if the player is currently inside an AI chase safe zone. */
	bool IsInSafeZone() const;

	/** Sets whether the player is in an AI safe zone. */
	void SetIsInSafeZone(const bool bInIsInSafeZone);

	/** Returns the interaction component managing player world interactions. */
	FORCEINLINE UInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }

	/** Returns the grab ability component managing pushable object interaction. */
	FORCEINLINE UGrabAbilityComponent* GetGrabAbilityComponent() const { return GrabAbilityComponent; }

protected:
	
	/** Bone socket name on the character mesh where the companion plush is held before throwing. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	FName RightHandCompanionSocketName = FName("PelucheSocket");
	
	/** Component managing proximity-based panic accumulation and decay. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPanicReceiverComponent> PanicReceiverComponent;
	
	/** Camera spring arm positioning the first person viewpoint. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> CameraSpringArm;
	
	/** First-person perspective camera component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> FirstPersonCamera;
	
	/** Component displaying and tracking items held in hand (keys, tools). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UHeldItem> HeldItem;

	/** Component managing detection, focus, and execution of world interactions. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInteractionComponent> InteractionComponent;

	/** Component managing object grabbing and push mechanics. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UGrabAbilityComponent> GrabAbilityComponent;

	/** Audio component triggering player footsteps, breathing, and heartbeat sounds. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCharacterAudioComponent> CharacterAudioComponent;
	
	/** Visual mesh placeholder representing the companion plush in hand. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DummyCompanionCharacterMesh;
	
	/** Instanced mesh component rendering trajectory preview beads when aiming companion throw. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trajectory")
	UInstancedStaticMeshComponent* TrajectoryISM;
	
	/** Scale multiplier applied to each trajectory preview point mesh. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trajectory")
	float TrajectoryPointScale = 0.3f; 

	/** Controls the point density / spacing along the trajectory curve (1 = every point, 2 = skip 1, etc.). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trajectory", meta = (ClampMin = "1"))
	int32 TrajectoryPointSpacing = 1;

	/** Data asset containing field of view, sensitivity, and camera orientation limits. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UHOERCameraSettingsData> CameraSettings;
	
	/** Post-process component driving dynamic visual distortion and vignettes as panic rises. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Panic System|Visuals")
	UPostProcessComponent* PanicPostProcess;
	
	void ApplyCameraSettings() const;

	// Input Handling
	virtual void Move(const FInputActionValue& Value) override;
	void StartChargingThrowCompanionCharacter(const FInputActionValue& Value);
    void UpdateChargingThrowCompanionCharacter(const FInputActionInstance& Instance); // Draws trajectory
	void ExecuteThrowCompanionCharacter(const FInputActionValue& Value);
	
	UFUNCTION()
	void HandleInteractionAttempted(AActor* TargetActor);
	
	UFUNCTION()
	void HandleMovementUpdated(float DeltaSeconds, FVector OldLocation, FVector OldVelocity);
	
	// Throwing State
	float CurrentCompanionCharacterChargeTime = 0.0f;
	bool bIsChargingThrowCompanionCharacter = false;
	FVector CalculatedThrowCompanionCharacterVelocity;
	   
	/** Current locomotion state machine state. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "State")
	EPlayerState CurrentState = EPlayerState::Idle;
	
	void TransitionToState(EPlayerState NewState);
	void OnEnterState(EPlayerState State) const;
	void OnExitState(EPlayerState State) const;
	
	virtual void SwitchCharacter(const FInputActionValue& Value) override;
    
    // While grabbing, pass a zero input to the pushable object so it can stop. Use to fade out the push sound
	virtual void StopMove(const FInputActionValue& Value) override;

	UPROPERTY()
	TObjectPtr<ACompanionCharacter> SpawnedCompanionCharacter;
	
	bool bIsInSafeZone = false;
	
	UFUNCTION()
	void OnPanicChanged(const float InPanicValue);
	
    // Called to trigger game over logic
	void OnPanicMaxReached() const;
	
private:
	const FVector DefaultHeldItemRelativeLocation = FVector(28.5f, -35.0f, -27.0f);
};
