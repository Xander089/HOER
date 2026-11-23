// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Trigger/GameEndTriggerBox.h"

#include "Character/HOERCharacterBase.h"
#include "Components/ShapeComponent.h"
#include "Controller/HOERPlayerController.h"
#include "Misc/DataValidation.h"
#include "System/HOERBlueprintFunctionLibrary.h"
#include "UI/Widgets/Common/HOERPageWidget.h"


// Sets default values
AGameEndTriggerBox::AGameEndTriggerBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// Retrieve the root shape component from the base ATriggerBox
	if (UShapeComponent* TriggerShape = GetCollisionComponent())
	{
		// Tell it to ignore all physical collisions and traces by default
		TriggerShape->SetCollisionResponseToAllChannels(ECR_Ignore);
        
		// Tell it to ONLY overlap with the Pawn channel (which characters use)
		TriggerShape->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		
	}
}

void AGameEndTriggerBox::NotifyActorBeginOverlap(AActor* OtherActor)
{
	// Always call the base class implementation first
	Super::NotifyActorBeginOverlap(OtherActor);
	
	// Cast to your base character class
	if (const AHOERCharacterBase* OverlappingCharacter = Cast<AHOERCharacterBase>(OtherActor))
	{
		// Get the class of the specific character that just overlapped
		UClass* OverlappingClass = OverlappingCharacter->GetClass();

		// Check if that class is inside your array of classes
		if (CharactersToDetect.Contains(OverlappingClass))
		{
			
			// we only want to set game over once
			if(bIsGameEnd){
				return;
			}

			AHOERPlayerController* PC = Cast<AHOERPlayerController>(GetWorld()->GetFirstPlayerController());
			if (!IsValid(PC))
			{
				return;
			}
	
			bIsGameEnd = true;
		
			// Stop player's movement
			if (APawn* LocalPawn = PC->GetPawn(); IsValid(LocalPawn))
			{
				LocalPawn->DisableInput(PC);
			}
	
			// Stop AIs
			PC->SetPause(true);
			
			// FName requires .IsNone() to check for empty/invalid
			if (!GameEndPageWidget || GameEndPageName.IsNone())
			{
				return;
			}
	
			UHOERBlueprintFunctionLibrary::AddPage(GetWorld(), GameEndPageWidget, GameEndPageName, 4000, true);
			
			EmitTriggerGameEnd();
			
		}
	}
}

#if WITH_EDITOR
EDataValidationResult AGameEndTriggerBox::IsDataValid(FDataValidationContext& Context) const
{
	// Always call Super first to get the base class validation result
	EDataValidationResult Result = Super::IsDataValid(Context);

	// Check if we actually have characters to detect
	if (CharactersToDetect.IsEmpty())
	{
		Context.AddError(FText::FromString(TEXT("CharactersToDetect array is empty! The trigger will not detect anything.")));
		Result = EDataValidationResult::Invalid;
	}

	// Check if the widget subclass is assigned
	if (!GameEndPageWidget)
	{
		Context.AddError(FText::FromString(TEXT("GameEndPageWidget is not assigned! The page will not load.")));
		Result = EDataValidationResult::Invalid;
	}

	// Check if the FName is filled out
	if (GameEndPageName.IsNone())
	{
		Context.AddError(FText::FromString(TEXT("GameEndPageName is empty! The page will not load.")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif

void AGameEndTriggerBox::EmitTriggerGameEnd()
{
	BP_TriggerGameEnd();
	OnTriggerGameEnd.Broadcast();
}

