// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Components/NPCLightSwitcherComponent.h"
#include "Gameplay/LightControls/LightActor.h"
#include "Gameplay/LightControls/LightInterface.h"
#include "System/Subsystem/LightManagerSubsystem.h"
#include "TimerManager.h"

UNPCLightSwitcherComponent::UNPCLightSwitcherComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = CheckInterval;
}


bool UNPCLightSwitcherComponent::NotifyNearby(AActor *Light)
{

	if (!IsValid(Light)){ return false; }
	if (!Light->Implements<ULightInterface>()){ return false; }

	const double Now = GetWorld()->GetTimeSeconds();
	const bool bLightIsOn = ILightInterface::Execute_IsLightOn(Light);

	if (bLightIsOn)
	{

		ILightInterface::Execute_ToggleLight(Light, false); // turn off the light
		SwitchedOffLights.Add(Light, Now); // add it to the list of switched off lights
		return true;
	}

	return false;
}

void UNPCLightSwitcherComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	RestoreNearbyLightsStatus(); // ticks every CheckInterval seconds
}

void UNPCLightSwitcherComponent::RestoreNearbyLightsStatus()
{
	for (auto It = SwitchedOffLights.CreateIterator(); It; ++It)
	{
		AActor* Light = It.Key().Get();
		if (!IsValid(Light))
		{
			It.RemoveCurrent();
			continue;
		}

		const double Now = GetWorld()->GetTimeSeconds();
		if (Now - It.Value() < RestoreDelay){ continue; }

		ILightInterface::Execute_ToggleLight(Light, true);
		It.RemoveCurrent();
	}
}
