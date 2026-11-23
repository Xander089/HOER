// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "PanicSystemSettings.generated.h"

/**
 * Project Developer Settings for global panic system update rate and performance tuning.
 */
UCLASS(config=Game, defaultconfig, meta=(DisplayName="Panic System Settings"))
class HOER_API UPanicSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPanicSystemSettings();
	
	/** Interval in seconds between global panic state recalculation passes across all active emitters and receivers. */
	UPROPERTY(Config, EditAnywhere, Category = "Performance", meta = (Units="Seconds", ClampMin="0.001"))
	float UpdateInterval = 0.01f;
};
