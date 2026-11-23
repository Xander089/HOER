// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/ObjectInteraction/InteractableLightSwitch.h"

#include "Gameplay/LightControls/LightActor.h"
#include "Gameplay/LightControls/LightInterface.h"


// Sets default values
AInteractableLightSwitch::AInteractableLightSwitch()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

bool AInteractableLightSwitch::Interact(AActor* Interactor)
{
	if (!Super::Interact(Interactor))
	{
		return false;
	}
	
	for (ALightActor* TargetLight : TargetLights)
	{
		// Validate the actor and interface at runtime before executing
		if (IsValid(TargetLight) && TargetLight->Implements<ULightInterface>())
		{
			const bool bTargetValue = !TargetLight->IsLightOn_Implementation();
			ILightInterface::Execute_ToggleLight(TargetLight, bTargetValue);
			EmitToggleLightEvent(Interactor, TargetLight, bTargetValue);
		}
	}
	
	return true;
}

void AInteractableLightSwitch::AddTargetLight(ALightActor* Light)
{
	TargetLights.Add(Light);
}

void AInteractableLightSwitch::EmitToggleLightEvent(AActor* Interactor, ALightActor* LightActor, const bool bIsOn)
{
	// No EmitEvent here: Super::Interact already fired OnInteractionSuccess once
	BP_OnToggleLight(Interactor, this, LightActor, bIsOn);
	OnToggleLight.Broadcast(Interactor, this, LightActor, bIsOn);
}




