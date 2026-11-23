// Fill out your copyright notice in the Description page of Project Settings.

#include "Controller/HOERPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "UI/Actors/HOERHUD.h"

AHOERPlayerController::AHOERPlayerController()
{
	// UI and Cursor settings are fine here, as the Controller handles the player's screen/window interaction.
	bShowMouseCursor = false;
}
void AHOERPlayerController::RequestBack() const
{
	if (const AHOERHUD* GameHUD = GetHUD<AHOERHUD>(); IsValid(GameHUD))
	{
		if (GameHUD->OnBackRequest.IsBound())
		{
			GameHUD->OnBackRequest.Broadcast();
		}
	}
}

void AHOERPlayerController::SwapInputMappingContext(GameTypes::EInputType InInputType) const
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (DefaultMappingContext && UIMappingContext)
		{
			UInputMappingContext* ToBeRemovedMappingContext = nullptr;
			UInputMappingContext* ToBeAddedMappingContext = nullptr;
			
			switch (InInputType)
			{
			case GameTypes::EInputType::GamePlay:
				ToBeRemovedMappingContext = UIMappingContext;
				ToBeAddedMappingContext = DefaultMappingContext;
				break;
			case GameTypes::EInputType::UI:
				ToBeRemovedMappingContext = DefaultMappingContext;
				ToBeAddedMappingContext = UIMappingContext;
				break;
			}
			
			Subsystem->RemoveMappingContext(ToBeRemovedMappingContext);
			Subsystem->AddMappingContext(ToBeAddedMappingContext, 0);
		}
	}
}

void AHOERPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}


void AHOERPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (UInputDeviceSubsystem* InputSubsystem = UInputDeviceSubsystem::Get(); IsValid(InputSubsystem))
	{
		InputSubsystem->OnInputHardwareDeviceChanged.AddDynamic(this, &AHOERPlayerController::OnControllerChanged);
	}
	
	SetGamepadConnected();

	//	Bug Fix: SetInputMode(FInputModeGameAndUI) sets mouse capture mode to "capture only while a mouse button is held"
	if (AHOERHUD* GameHUD = GetHUD<AHOERHUD>(); IsValid(GameHUD))
	{
		GameHUD->UpdateInputMode();
	}
	else
	{
		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(false);
	}
}

void AHOERPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UInputDeviceSubsystem* InputSubsystem = UInputDeviceSubsystem::Get(); IsValid(InputSubsystem))
	{
		InputSubsystem->OnInputHardwareDeviceChanged.RemoveAll(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

void AHOERPlayerController::SetIsMainMenuLevel(const bool bInIsMainMenu)
{
	bIsMainMenuLevel = bInIsMainMenu;
}

void AHOERPlayerController::OnControllerChanged(const FPlatformUserId UserId, const FInputDeviceId DeviceId)
{
	const UInputDeviceSubsystem* InputSubsystem = UInputDeviceSubsystem::Get();
	if (!IsValid(InputSubsystem)) return;
 
	const FHardwareDeviceIdentifier HardwareInfo = InputSubsystem->GetInputDeviceHardwareIdentifier(DeviceId);
	 
	if (HardwareInfo.HardwareDeviceIdentifier.IsValid())
	{
		const bool bIsGamePad = HardwareInfo.HardwareDeviceIdentifier.ToString().Contains(TEXT("Gamepad"));
		if (AHOERHUD* CurrentHUD = Cast<AHOERHUD>(GetHUD<AHOERHUD>()); IsValid(CurrentHUD))
		{
			CurrentHUD->NotifyGamepadConnectionChanged(bIsGamePad);
		}
		
	}
}

void AHOERPlayerController::SetGamepadConnected()
{
	const UInputDeviceSubsystem* InputSubsystem = UInputDeviceSubsystem::Get();
	if (!IsValid(InputSubsystem)) { return; }
	
	const ULocalPlayer* CurrentPlayer = GetLocalPlayer();
	if (!IsValid(CurrentPlayer)) { return; }
	
	const FHardwareDeviceIdentifier HardwareInfo = InputSubsystem->GetMostRecentlyUsedHardwareDevice(CurrentPlayer->GetPlatformUserId());
	if (HardwareInfo.HardwareDeviceIdentifier.IsValid() && HardwareInfo.HardwareDeviceIdentifier.ToString().Contains(TEXT("Gamepad")))
	{
		bIsGamepadConnected = true;
	}
	else
	{
		bIsGamepadConnected = false;
	}
}



void AHOERPlayerController::PauseGame()
{
	if (bIsMainMenuLevel){ return; }
	BP_PauseGame();
}