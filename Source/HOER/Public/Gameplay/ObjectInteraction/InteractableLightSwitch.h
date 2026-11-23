// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableObject.h"
#include "InteractableLightSwitch.generated.h"

class ALightActor;
class ILightInterface;

/**
 * Delegate broadcast when a light switch is toggled.
 *
 * @param Interactor The actor that toggled the switch.
 * @param InteractableLightSwitch The light switch actor instance.
 * @param LightActor The target light actor affected.
 * @param bIsOn True if the light was turned on, false if turned off.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnToggleLight, AActor*, Interactor, AInteractableLightSwitch*, InteractableLightSwitch, ALightActor*, LightActor, bool, bIsOn);

/**
 * Interactable switch actor that toggles one or more linked light actors in the level.
 */
UCLASS()
class HOER_API AInteractableLightSwitch : public AInteractableObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInteractableLightSwitch();
	
	virtual bool Interact(AActor* Interactor) override;

	/** Broadcast when this switch toggles its connected lights. */
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnToggleLight OnToggleLight;
	
	/**
	 * Links an additional light actor to be controlled by this switch.
	 *
	 * @param Light The light actor to add.
	 */
	void AddTargetLight(ALightActor* Light);

protected:
	/** Collection of target light actors toggled by this switch in the level. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Switch", meta = (MustImplement = "/Script/HOER.LightInterface"))
	TArray<ALightActor*> TargetLights;

	/**
	 * Blueprint event triggered when this light switch toggles.
	 *
	 * @param Interactor The actor toggling the switch.
	 * @param InteractableLightSwitch This light switch actor instance.
	 * @param LightActor The target light actor toggled.
	 * @param bIsOn The new on/off state of the light.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction|Events", meta = (DisplayName = "On Light Toggle"))
	void BP_OnToggleLight(AActor* Interactor, AInteractableLightSwitch* InteractableLightSwitch, ALightActor* LightActor, bool bIsOn);
	
	void EmitToggleLightEvent(AActor* Interactor, ALightActor* LightActor, bool bIsOn);
};
