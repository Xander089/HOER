// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/Common/HOERBaseWidget.h"
#include "HOERHUDWidget.generated.h"

/**
 * Main in-game HUD overlay widget displaying active player meters, reticles, and panic indicators.
 */
UCLASS(Abstract)
class HOER_API UHOERHUDWidget : public UHOERBaseWidget
{
	GENERATED_BODY()
};
