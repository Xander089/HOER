// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LightInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class ULightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface implemented by light actors and controllable light sources to toggle states and query on/off status.
 */
class HOER_API ILightInterface
{
	GENERATED_BODY()

public:
	
	/**
	 * Toggles the light on or off.
	 *
	 * @param InbOn True to turn the light on, false to turn it off.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Light")
	void ToggleLight(bool InbOn);
	
	/**
	 * Checks whether this light is currently turned on.
	 *
	 * @return True if the light is on, false if off.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Light")
	bool IsLightOn();

	virtual void ToggleLight_Implementation(bool InbOn) = 0;
	virtual bool IsLightOn_Implementation() const = 0;
};
