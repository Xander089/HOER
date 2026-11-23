// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "HOERGameInstance.generated.h"

/**
 * Main Game Instance class for HOER, persisting cross-level state and hosting global subsystems.
 */
UCLASS()
class HOER_API UHOERGameInstance : public UGameInstance
{
	GENERATED_BODY()
};
