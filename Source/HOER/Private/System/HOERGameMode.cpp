// Fill out your copyright notice in the Description page of Project Settings.


#include "System/HOERGameMode.h"
#include "Character/HOERCharacter.h"
#include "Controller/HOERPlayerController.h"
#include "Save/SaveSubsystem.h"
#include "UI/Actors/HOERHUD.h"

AHOERGameMode::AHOERGameMode()
{
	DefaultPawnClass = AHOERCharacter::StaticClass();
	PlayerControllerClass = AHOERPlayerController::StaticClass();
	HUDClass = AHOERHUD::StaticClass();
}

void AHOERGameMode::StartPlay()
{
	// Super spawns the player pawns, so saved data must be applied after it
	Super::StartPlay();

	const UGameInstance* GameInstance = GetGameInstance();
	if (!IsValid(GameInstance))
	{
		return;
	}

	if (USaveSubsystem* SaveSubsystem = GameInstance->GetSubsystem<USaveSubsystem>(); IsValid(SaveSubsystem))
	{
		SaveSubsystem->ApplyToWorld(GetWorld());
	}
}


void AHOERGameMode::TriggerGameOver(AController* CurrentLocalController){
	
	// we only want to set game over once
	if(bIsGameOver){
		return;
	}

	AHOERPlayerController* PC = Cast<AHOERPlayerController>(CurrentLocalController);
	if (!IsValid(PC))
	{
		return;
	}
	
	bIsGameOver = true;
		
	// Stop player's movement
	if (APawn* LocalPawn = PC->GetPawn(); IsValid(LocalPawn))
	{
		LocalPawn->DisableInput(PC);
	}
	
	// Stop AIs
	PC->SetPause(true);
	
	// called to add game over widget to the viewport
	BP_TriggerGameOver();
	
}