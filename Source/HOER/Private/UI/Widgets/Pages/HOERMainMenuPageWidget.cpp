// Fill out your copyright notice in the Description page of Project Settings.


#include "HOER/Public/UI/Widgets/Pages/HOERMainMenuPageWidget.h"

#include "UI/Widgets/Common/HOERButtonWidget.h"
#include "UI/Widgets/Common/PageFooterWidget.h"

void UHOERMainMenuPageWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CacheButtons();
	SetBindings();
	CacheDataAssets();
}

void UHOERMainMenuPageWidget::NativeDestruct()
{
	RemoveBindings();
	Super::NativeDestruct();
}

void UHOERMainMenuPageWidget::OnStartButtonClicked(UHOERButtonWidget* InButton)
{
	BP_OnStartButtonClicked();
	CacheFocusedWidget(InButton->GetButton());
}

void UHOERMainMenuPageWidget::OnQuitButtonClicked(UHOERButtonWidget* InButton)
{
	BP_OnQuitButtonClicked();
	CacheFocusedWidget(InButton->GetButton());
}

void UHOERMainMenuPageWidget::OnContinueButtonClicked(UHOERButtonWidget* InButton)
{
	BP_OnContinueButtonClicked();
	CacheFocusedWidget(InButton->GetButton());
}
void UHOERMainMenuPageWidget::OnCreditsButtonClicked(UHOERButtonWidget* InButton)
{
	BP_OnCreditsButtonClicked();
	
	CacheFocusedWidget(InButton->GetButton());
	InButton->SetHasPageOnTop(); // Keep focus on the button when a page is added on top
	OnPageAddedOnTop(InButton);  // Notify other buttons that a page has been added on top

	// Hide Footer
	if (IsValid(Footer))
	{
		Footer->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UHOERMainMenuPageWidget::OnCommandsButtonClicked(UHOERButtonWidget* InButton)
{
	BP_OnCommandsButtonClicked();
	
	CacheFocusedWidget(InButton->GetButton());
	InButton->SetHasPageOnTop(); // Keep focus on the button when a page is added on top
	OnPageAddedOnTop(InButton);  // Notify other buttons that a page has been added on top

	// Hide Footer
	if (IsValid(Footer))
	{
		Footer->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UHOERMainMenuPageWidget::OnSettingsButtonClicked(UHOERButtonWidget* InButton)
{
	BP_OnSettingsButtonClicked();
	CacheFocusedWidget(InButton->GetButton());
	InButton->SetHasPageOnTop(); // Keep focus on the button when a page is added on top
	OnPageAddedOnTop(InButton);  // Notify other buttons that a page has been added on top

	// Hide Footer
	if (IsValid(Footer))
	{
		Footer->SetVisibility(ESlateVisibility::Collapsed);
	}
}


void UHOERMainMenuPageWidget::OnStartButtonFocused(UHOERButtonWidget* InButton)
{
	BP_OnStartButtonFocused();
	CacheFocusedWidget(InButton->GetButton());
}

void UHOERMainMenuPageWidget::OnQuitButtonFocused(UHOERButtonWidget* InButton)
{
	BP_OnQuitButtonFocused();
	CacheFocusedWidget(InButton->GetButton());
}

void UHOERMainMenuPageWidget::OnContinueButtonFocused(UHOERButtonWidget* InButton)
{
	BP_OnContinueButtonFocused();
	CacheFocusedWidget(InButton->GetButton());
}
void UHOERMainMenuPageWidget::OnCreditsButtonFocused(UHOERButtonWidget* InButton)
{
	BP_OnCreditsButtonFocused();
	CacheFocusedWidget(InButton->GetButton());
}

void UHOERMainMenuPageWidget::OnSettingsButtonFocused(UHOERButtonWidget* InButton)
{
	BP_OnSettingsButtonFocused();
	CacheFocusedWidget(InButton->GetButton());
}

void UHOERMainMenuPageWidget::OnCommandsButtonFocused(UHOERButtonWidget* InButton)
{
	BP_OnCommandsButtonFocused();
	CacheFocusedWidget(InButton->GetButton());
}


void UHOERMainMenuPageWidget::SetBindings()
{
	if (IsValid(StartButton))
	{
		StartButton->OnButtonClicked.AddDynamic(this, &UHOERMainMenuPageWidget::OnStartButtonClicked);
		StartButton->OnButtonFocused.AddDynamic(this, &UHOERMainMenuPageWidget::OnStartButtonFocused);
	}
	
	if (IsValid(QuitButton))
	{
		QuitButton->OnButtonClicked.AddDynamic(this, &UHOERMainMenuPageWidget::OnQuitButtonClicked);
		QuitButton->OnButtonFocused.AddDynamic(this, &UHOERMainMenuPageWidget::OnQuitButtonFocused);
	}
	
	if (IsValid(ContinueButton))
	{
		ContinueButton->OnButtonClicked.AddDynamic(this, &UHOERMainMenuPageWidget::OnContinueButtonClicked);
		ContinueButton->OnButtonFocused.AddDynamic(this, &UHOERMainMenuPageWidget::OnContinueButtonFocused);
	}
	
	if (IsValid(CreditsButton))
	{
		CreditsButton->OnButtonClicked.AddDynamic(this, &UHOERMainMenuPageWidget::OnCreditsButtonClicked);
		CreditsButton->OnButtonFocused.AddDynamic(this, &UHOERMainMenuPageWidget::OnCreditsButtonFocused);
	}
	if (IsValid(SettingsButton))
	{
		SettingsButton->OnButtonClicked.AddDynamic(this, &UHOERMainMenuPageWidget::OnSettingsButtonClicked);
		SettingsButton->OnButtonFocused.AddDynamic(this, &UHOERMainMenuPageWidget::OnSettingsButtonFocused);
	}

	if (IsValid(CommandsButton))
	{
		CommandsButton->OnButtonClicked.AddDynamic(this, &UHOERMainMenuPageWidget::OnCommandsButtonClicked);
		CommandsButton->OnButtonFocused.AddDynamic(this, &UHOERMainMenuPageWidget::OnCommandsButtonFocused);
	}
}

void UHOERMainMenuPageWidget::RemoveBindings()
{
	if (IsValid(StartButton))
	{
		StartButton->OnButtonClicked.RemoveDynamic(this, &UHOERMainMenuPageWidget::OnStartButtonClicked);
		StartButton->OnButtonFocused.RemoveDynamic(this, &UHOERMainMenuPageWidget::OnStartButtonClicked);
	}
	
	if (IsValid(QuitButton))
	{
		QuitButton->OnButtonClicked.RemoveDynamic(this, &UHOERMainMenuPageWidget::OnQuitButtonClicked);
		QuitButton->OnButtonFocused.RemoveDynamic(this, &UHOERMainMenuPageWidget::OnQuitButtonFocused);
	}
	
	if (IsValid(ContinueButton))
	{
		ContinueButton->OnButtonClicked.RemoveDynamic(this, &UHOERMainMenuPageWidget::OnContinueButtonClicked);
		ContinueButton->OnButtonFocused.RemoveDynamic(this, &UHOERMainMenuPageWidget::OnContinueButtonFocused);
	}
	
	if (IsValid(CreditsButton))
	{
		CreditsButton->OnButtonClicked.RemoveDynamic(this, &UHOERMainMenuPageWidget::OnCreditsButtonClicked);
		CreditsButton->OnButtonFocused.RemoveDynamic(this, &UHOERMainMenuPageWidget::OnCreditsButtonFocused);
	}
	if (IsValid(SettingsButton))
	{
		SettingsButton->OnButtonClicked.RemoveDynamic(this, &UHOERMainMenuPageWidget::OnSettingsButtonClicked);
		SettingsButton->OnButtonFocused.RemoveDynamic(this, &UHOERMainMenuPageWidget::OnSettingsButtonFocused);
	}

	if (IsValid(CommandsButton))
	{
		CommandsButton->OnButtonClicked.RemoveDynamic(this, &UHOERMainMenuPageWidget::OnCommandsButtonClicked);
		CommandsButton->OnButtonFocused.RemoveDynamic(this, &UHOERMainMenuPageWidget::OnCommandsButtonFocused);
	}
}

void UHOERMainMenuPageWidget::CacheButtons()
{
	Buttons.Empty();
	Buttons.Reserve(6);
	
	if (IsValid(StartButton))
	{
		Buttons.Add(StartButton);
	}
	
	if (IsValid(QuitButton))
	{
		Buttons.Add(QuitButton);
	}
	
	if (IsValid(ContinueButton))
	{
		Buttons.Add(ContinueButton);
	}

	if (IsValid(CreditsButton))
	{
		Buttons.Add(CreditsButton);
	}

	if (IsValid(SettingsButton))
	{
		Buttons.Add(SettingsButton);
	}
	
	if (IsValid(CommandsButton))
	{
		Buttons.Add(CommandsButton);
	}
}

void UHOERMainMenuPageWidget::SetFocus(UHOERButtonWidget* InButton) const
{
	if (!IsValid(InButton)){	return; }
	
	// Get the player controller
	APlayerController* PlayerController = GetOwningPlayer();
	if (!IsValid(PlayerController)){ return; }
	
	InButton->SetUserFocus(PlayerController);
	
}
void UHOERMainMenuPageWidget::OnPageAddedOnTop(const UHOERButtonWidget* InButton)
{
	for (TWeakObjectPtr<UHOERButtonWidget> Button : Buttons)
	{
		if (Button.IsValid())
		{
			if (Button.Get() == InButton)
			{
				continue;
			}
			Button->BP_OnPageAddedOnTop();
		}
	}
}


void UHOERMainMenuPageWidget::CacheDataAssets()
{
	FooterDataMap.Reserve(5);
	FooterDataMap.Add(StartButton, NewGameDataAsset);
	FooterDataMap.Add(ContinueButton, ContinueDataAsset);
	FooterDataMap.Add(CreditsButton, CreditsDataAsset);
	FooterDataMap.Add(SettingsButton, OptionsDataAsset);
	FooterDataMap.Add(QuitButton, QuitDataAsset);
	FooterDataMap.Add(CommandsButton, CommandsDataAsset);
}

void UHOERMainMenuPageWidget::OnPageRemovedFromTop()
{
	Super::OnPageRemovedFromTop();
	
	if (!LastFocusedWidget.IsValid())
	{
		return;
	}
	
	for (TWeakObjectPtr<UHOERButtonWidget> Button : Buttons)
	{
		if (!Button.IsValid())
		{
			continue;
		}
		
		if (Button.Get() == LastFocusedWidget.Get())
		{
				continue;
		}
		
		Button->BP_OnPageRemovedFromTop();
	}
}