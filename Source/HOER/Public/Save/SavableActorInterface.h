// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SavableActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class USavableActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface implemented by actors that participate in the game's save/load lifecycle.
 */
class HOER_API ISavableActorInterface
{
	GENERATED_BODY()

public:
	
	/** Called on the actor when world save data is being captured. */
	virtual void OnActorSaved();

	/** Called on the actor after saved world state has been restored to it. */
	virtual void OnActorLoaded();
};
