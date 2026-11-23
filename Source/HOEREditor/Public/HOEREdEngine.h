// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Editor/UnrealEdEngine.h"
#include "HOEREdEngine.generated.h"

/**
 * Custom UnrealEdEngine extension for HOER editor sessions, enforcing pre-save data validations.
 */
UCLASS()
class HOEREDITOR_API UHOEREdEngine : public UUnrealEdEngine
{
	GENERATED_BODY()
	
public:
	/** Override Init to register source control state branches */
	virtual void Init(IEngineLoop* InEngineLoop) override;
	virtual bool CanSavePackage(UPackage* PackageToSave) override;
};
