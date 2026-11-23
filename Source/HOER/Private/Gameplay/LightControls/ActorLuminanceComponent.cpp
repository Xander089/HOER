// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/LightControls/ActorLuminanceComponent.h"

UActorLuminanceComponent::UActorLuminanceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UActorLuminanceComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UActorLuminanceComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	OnLightChange.Clear();
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

const float UActorLuminanceComponent::AddLuminance(AActor* lightFont, float value)
{
	float total = 0;
	
	if (LuminanceValueForLight.Contains(lightFont))
	{
		LuminanceValueForLight[lightFont] = value;
		total = SetTotalLuminance();
	}
	else
	{
		LuminanceValueForLight.Emplace(lightFont, value);
		total = AddToTotalLuminance(value);
	}
	return total;
	
}

void UActorLuminanceComponent::RemoveLuminance(AActor* lightFont)
{
	if (LuminanceValueForLight.Contains(lightFont))
	{
		LuminanceValueForLight.Remove(lightFont);
		SetTotalLuminance();
	}
}

const float UActorLuminanceComponent::SetTotalLuminance()
{
	TArray<float> LuminanceValues;
	LuminanceValueForLight.GenerateValueArray(LuminanceValues);
	float total = 0;
	for (float luminanceValue : LuminanceValues)
		total += luminanceValue;
	
	if (LuminanceValue != total)
	{
		OnLightChange.Broadcast(total);
		LuminanceValue = total;
	}
	
	return LuminanceValue;
}

const float UActorLuminanceComponent::AddToTotalLuminance(float value)
{
	if (value != 0)
	{
		LuminanceValue = FMath::Max(0,LuminanceValue + value);
		OnLightChange.Broadcast(LuminanceValue);
	}
	return LuminanceValue;
}

void UActorLuminanceComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

