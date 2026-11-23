// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Save/SavableActorInterface.h"
#include "AICharacter.generated.h"

class UEnemyAudioComponent;
class UPanicEmitterComponent;

/**
 * Enumeration of dynamic stats modulated by risk curves on the enemy AI character.
 */
UENUM(BlueprintType)
enum class EAIStatConfig : uint8
{
	/** Locomotion speed of the AI character. */
	MaxWalkSpeed UMETA(DisplayName = "Max Walk Speed"),

	/** Radius around the AI within which lights are turned off. */
	SwitchLightOffRadius UMETA(DisplayName = "Switch Light Off Radius"),
};

struct FCurveTableRowHandle;
struct FRealCurve;

/** Delegate broadcast when the AI character detects the player. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDetected);

/** Delegate broadcast when the AI character loses sight and forgets the player. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerForgotten);

/**
 * Base enemy NPC character driven by behavior tree and perception systems,
 * dynamically adjusting speed and light extinguishing aura based on player tension and risk awareness.
 */
UCLASS()
class HOER_API AAICharacter : public ACharacter, public ISavableActorInterface
{
	GENERATED_BODY()
	
public:
	AAICharacter();

	/** Broadcast when the AI successfully perceives and initiates pursuit on the player. */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Tension")
	FOnPlayerDetected OnPlayerDetected;

	/** Broadcast when the AI loses track of the player and returns to search/patrol. */
	FOnPlayerForgotten OnPlayerForgotten;
	
	virtual void BeginPlay() override;

	/**
	 * Callback responding to changes in director risk awareness, adjusting curves and speeds.
	 *
	 * @param RiskAwareness Normalized risk value between 0.0 and 1.0.
	 */
	UFUNCTION()
	void ReactToRiskAwarenessChange(const float& RiskAwareness);
	
	// Savable Interface
	virtual void OnActorSaved() override;
	virtual void OnActorLoaded() override;
	
protected:
	
	/** Audio component emitting creature roars, footsteps, and chase stingers. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEnemyAudioComponent> EnemyAudioComponent = nullptr;
	
	/** Curve table handles mapping risk awareness to walk speed and light-extinguish radius. */
	UPROPERTY(EditAnywhere, Category = "AI Stats")
	TMap<EAIStatConfig, FCurveTableRowHandle> StatCurveHandles;
	
	/** Proximity radius within which nearby lights will flicker and turn off. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Stats")
	float SwitchLightOffRadius = 3000.0f;
	
	/** Component radiating panic influence to nearby players within proximity or line of sight. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPanicEmitterComponent> PanicEmitterComponent;
	
private:
	
	FRealCurve* GetCurveByEnum(const FString& Context, const EAIStatConfig& EnumValue);

public:
	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

};
