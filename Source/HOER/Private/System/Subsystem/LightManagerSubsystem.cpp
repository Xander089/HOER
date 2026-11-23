// Fill out your copyright notice in the Description page of Project Settings.


#include "System/Subsystem/LightManagerSubsystem.h"
#include "Gameplay/LightControls/LightActor.h"
#include "Gameplay/LightControls/LightInterface.h"
#include "Kismet/GameplayStatics.h"


void ULightManagerSubSystem::Register(ALightActor* InLight)
{
	if (IsValid(InLight))
	{
		const int32 AddedLightIndex = Lights.Emplace(InLight);
		LightToIndexMap.Add(InLight, AddedLightIndex);
		InLight->OnLightStateChanged.AddDynamic(this, &ULightManagerSubSystem::HandleLightStateChanged);
	}
}

void ULightManagerSubSystem::Unregister(ALightActor* InLight)
{
	if (!IsValid(InLight))
	{
		return;
	}
	
	if (Lights.Contains(InLight))
	{
		Lights.Remove(InLight);
	}
	
	if (LightToIndexMap.Contains(InLight))
	{
		LightToIndexMap.Remove(InLight);
	}
	
	if (InLight->OnLightStateChanged.IsBound())
	{
		InLight->OnLightStateChanged.RemoveDynamic(this, &ULightManagerSubSystem::HandleLightStateChanged);
	}

}

void ULightManagerSubSystem::Reserve(const int32 InLightCount)
{
	if (Lights.IsEmpty() && LightToIndexMap.IsEmpty())
	{
		Lights.Reserve(InLightCount);
		LightToIndexMap.Reserve(InLightCount);
		
	}
}

void ULightManagerSubSystem::GetLast_K_SwitchedOnLightsFromHistory(const int32 K, TArray<ALightActor*>& OutLights) const
{
	OutLights.Reset();

	// check invalid input parameter
	if (K <= 0)
	{
		return;
	}

	// safe number of lights to extract, checking if the history count is less than K
	const int32 LightToRetrieveCount = FMath::Min(K, TurnedOnHistory.Num());

	if (LightToRetrieveCount == 0)
	{
		return;
	}
	
	OutLights.Reserve(LightToRetrieveCount);

	for (int32 i = 0; i < LightToRetrieveCount; ++i)
	{
		const int32 HistoryIndex = TurnedOnHistory.Num() - 1 - i;
		const int32 LightIndex = TurnedOnHistory[HistoryIndex];
	
		// check LightIndex validity
		if (!Lights.IsValidIndex(LightIndex)){ continue; }
		
		// check the retrieved weak ptr validity
		if (!Lights[LightIndex].IsValid()) { continue; }
		
		// if all checks passed, add the valid light to the output TArray
		OutLights.Add(Lights[LightIndex].Get());
		
	}
}

int32 ULightManagerSubSystem::GetLightCount() const
{
	return Lights.Num();
}

int32 ULightManagerSubSystem::GetSwitchedOnLightCount() const
{
	return TurnedOnHistory.Num();
}

float ULightManagerSubSystem::CalculateFitness(const float& AdjacentLightsOnCountWeight) const
{
	int32 AdjacentLightsOnCount = 0;
	
	for (int32 i = 1; i < Lights.Num(); ++i)
	{
		//check index validity
		if (!Lights.IsValidIndex(i)){ continue; }
		if (!Lights.IsValidIndex(i - 1)){ continue; }
		
		// check the current i and i-1 weak ptr validity
		if (!Lights[i].IsValid() && !Lights[i]->Implements<ULightInterface>()) { continue; }
		if (!Lights[i - 1].IsValid()) { continue; }
		
		// check whether i and i-1 lights implements LightInterface
		if (!Lights[i]->Implements<ULightInterface>()) { continue; }
		if (!Lights[i -1]->Implements<ULightInterface>()) { continue; }
		
		// check whether previous and current lights are both on
		const bool CurrentLightIsOn = ILightInterface::Execute_IsLightOn(Lights[i].Get());
		const bool PreviousLightIsOn = ILightInterface::Execute_IsLightOn(Lights[i - 1].Get());
		
		if (CurrentLightIsOn && PreviousLightIsOn)
		{
			AdjacentLightsOnCount++;
		}
		
	}
	const float ContinuityFitness = static_cast<float>(AdjacentLightsOnCount) / static_cast<float>(GetLightCount());
	const float LightOnFitness = static_cast<float>(GetSwitchedOnLightCount()) / static_cast<float>(GetLightCount());
	
	const float LightsOnCountWeight = 1.0f - AdjacentLightsOnCountWeight;
	
	return LightsOnCountWeight * LightOnFitness + AdjacentLightsOnCountWeight * ContinuityFitness;
}

void ULightManagerSubSystem::HandleLightStateChanged(ALightActor* InLight)
{
	// check input light validity
	if (!IsValid(InLight))
	{
		return;
	}
	
	if (!InLight->Implements<ULightInterface>())
	{
		return;
	}
	
	UpdateLightStateHistory(InLight);
	
	
	// this will be linked to the AI director, notified whenever a light is switched on or off
	if (OnLightStateChanged.IsBound())
	{
		OnLightStateChanged.Broadcast(ILightInterface::Execute_IsLightOn(InLight));
	}
}

void ULightManagerSubSystem::HandleStartIlluminate(AActor* OtherActor) const
{
	if (OnStartIlluminate.IsBound())
	{
		OnStartIlluminate.Broadcast(OtherActor);
	}
}


void ULightManagerSubSystem::HandleStopIlluminate(AActor* OtherActor) const
{
	if (OnStopIlluminate.IsBound())
	{
		OnStopIlluminate.Broadcast(OtherActor);
	}
}


void ULightManagerSubSystem::UpdateLightStateHistory(ALightActor* InLight)
{
	
	// check input light validity
	if (!IsValid(InLight))
	{
		return;
	}
	
	if (!InLight->Implements<ULightInterface>())
	{
		return;
	}
	
	// 1. retrieve the int32 light index from the map
	const int32* IndexPtr = LightToIndexMap.Find(InLight);
	if (!IndexPtr) // map does not contain the Input Light
	{
		return;
	}

	const int32 Index = *IndexPtr;
	
	// 2.1 - if light was switched on and history does not already contain the light --> add to history
	if (ILightInterface::Execute_IsLightOn(InLight) && !TurnedOnHistory.Contains(Index))
	{
		TurnedOnHistory.Add(Index);
	}
	// 2.2 - else if light was switched off and history contains the light --> remove from history
	else if(!ILightInterface::Execute_IsLightOn(InLight) && TurnedOnHistory.Contains(Index))
	{
		TurnedOnHistory.RemoveSingle(Index);
	}
}
