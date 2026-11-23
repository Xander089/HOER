// Fill out your copyright notice in the Description page of Project Settings.


#include "HOER/Public/UI/Widgets/Pages/HOERPausePageWidget.h"
#include "UI/Widgets/Common/HOERButtonWidget.h"

void UHOERPausePageWidget::OnPageRemovedFromTop()
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

void UHOERPausePageWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CacheButtons();
	SetBindings();
	CacheDataAssets();
}

void UHOERPausePageWidget::NativeDestruct()
{
	RemoveBindings();
	Super::NativeDestruct();
}

void UHOERPausePageWidget::OnRestartButtonClicked(UHOERButtonWidget* InButton)
{
	BP_OnReStartButtonClicked();
	CacheFocusedWidget(InButton->GetButton());
}

void UHOERPausePageWidget::OnContinueButtonClicked(UHOERButtonWidget* InButton)
{
	BP_OnContinueButtonClicked();
	CacheFocusedWidget(InButton->GetButton());
}

void UHOERPausePageWidget::OnLoadButtonClicked(UHOERButtonWidget *InButton)
{
	BP_OnLoadButtonClicked();
	CacheFocusedWidget(InButton->GetButton());
}

void UHOERPausePageWidget::OnCommandsButtonClicked(UHOERButtonWidget* InButton)
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

void UHOERPausePageWidget::OnSettingsButtonClicked(UHOERButtonWidget *InButton)
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

void UHOERPausePageWidget::OnPageAddedOnTop(const UHOERButtonWidget* InButton)
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

void UHOERPausePageWidget::OnRestartButtonFocused(UHOERButtonWidget* InButton)
{
	BP_OnReStartButtonFocused();
	CacheFocusedWidget(InButton->GetButton());
}

void UHOERPausePageWidget::OnContinueButtonFocused(UHOERButtonWidget* InButton)
{
	BP_OnContinueButtonFocused();
	CacheFocusedWidget(InButton->GetButton());
}

void UHOERPausePageWidget::OnLoadButtonFocused(UHOERButtonWidget *InButton)
{
	BP_OnLoadButtonFocused();
	CacheFocusedWidget(InButton->GetButton());
}

void UHOERPausePageWidget::OnSettingsButtonFocused(UHOERButtonWidget *InButton)
{
	BP_OnSettingsButtonFocused();
	CacheFocusedWidget(InButton->GetButton());
}

void UHOERPausePageWidget::OnCommandsButtonFocused(UHOERButtonWidget* InButton)
{
	BP_OnCommandsButtonFocused();
	CacheFocusedWidget(InButton->GetButton());
}

void UHOERPausePageWidget::SetBindings()
{
	if (IsValid(RestartButton))
	{
		RestartButton->OnButtonClicked.AddDynamic(this, &UHOERPausePageWidget::OnRestartButtonClicked);
		RestartButton->OnButtonFocused.AddDynamic(this, &UHOERPausePageWidget::OnRestartButtonFocused);
	}
	
	
	if (IsValid(ContinueButton))
	{
		ContinueButton->OnButtonClicked.AddDynamic(this, &UHOERPausePageWidget::OnContinueButtonClicked);
		ContinueButton->OnButtonFocused.AddDynamic(this, &UHOERPausePageWidget::OnContinueButtonFocused);
	}	
	
	if (IsValid(LoadButton))
	{
		LoadButton->OnButtonClicked.AddDynamic(this, &UHOERPausePageWidget::OnLoadButtonClicked);
		LoadButton->OnButtonFocused.AddDynamic(this, &UHOERPausePageWidget::OnLoadButtonFocused);
	}
	if (IsValid(SettingsButton))
	{
		SettingsButton->OnButtonClicked.AddDynamic(this, &UHOERPausePageWidget::OnSettingsButtonClicked);
		SettingsButton->OnButtonFocused.AddDynamic(this, &UHOERPausePageWidget::OnSettingsButtonFocused);
	}

	if (IsValid(CommandsButton))
	{
		CommandsButton->OnButtonClicked.AddDynamic(this, &UHOERPausePageWidget::OnCommandsButtonClicked);
		CommandsButton->OnButtonFocused.AddDynamic(this, &UHOERPausePageWidget::OnCommandsButtonFocused);
	}
}

void UHOERPausePageWidget::RemoveBindings()
{
	if (IsValid(RestartButton))
	{
		RestartButton->OnButtonClicked.RemoveDynamic(this, &UHOERPausePageWidget::OnRestartButtonClicked);
		RestartButton->OnButtonFocused.RemoveDynamic(this, &UHOERPausePageWidget::OnRestartButtonFocused);
	}
	
	if (IsValid(ContinueButton))
	{
		ContinueButton->OnButtonClicked.RemoveDynamic(this, &UHOERPausePageWidget::OnContinueButtonClicked);
		ContinueButton->OnButtonFocused.RemoveDynamic(this, &UHOERPausePageWidget::OnContinueButtonFocused);
	}
	
	if (IsValid(LoadButton))
	{
		LoadButton->OnButtonClicked.RemoveDynamic(this, &UHOERPausePageWidget::OnLoadButtonClicked);
		LoadButton->OnButtonFocused.RemoveDynamic(this, &UHOERPausePageWidget::OnLoadButtonFocused);
	}
	if (IsValid(SettingsButton))
	{
		SettingsButton->OnButtonClicked.RemoveDynamic(this, &UHOERPausePageWidget::OnSettingsButtonClicked);
		SettingsButton->OnButtonFocused.RemoveDynamic(this, &UHOERPausePageWidget::OnSettingsButtonFocused);
	}
	if (IsValid(CommandsButton))
	{
		CommandsButton->OnButtonClicked.RemoveDynamic(this, &UHOERPausePageWidget::OnCommandsButtonClicked);
		CommandsButton->OnButtonFocused.RemoveDynamic(this, &UHOERPausePageWidget::OnCommandsButtonFocused);
	}
}

void UHOERPausePageWidget::CacheButtons()
{
	Buttons.Empty();
	Buttons.Reserve(6);
	
	if (IsValid(RestartButton))
	{
		Buttons.Add(RestartButton);
	}
	
	if (IsValid(ContinueButton))
	{
		Buttons.Add(ContinueButton);
	}

	if (IsValid(SettingsButton))
	{
		Buttons.Add(SettingsButton);
	}

	if (IsValid(LoadButton))
	{
		Buttons.Add(LoadButton);
	}

	if (IsValid(CommandsButton))
	{
		Buttons.Add(CommandsButton);
	}
}

void UHOERPausePageWidget::CacheDataAssets()
{
	FooterDataMap.Reserve(4);
	FooterDataMap.Add(RestartButton, RestartDataAsset);
	FooterDataMap.Add(ContinueButton, ContinueDataAsset);
	FooterDataMap.Add(LoadButton, LoadDataAsset);
	FooterDataMap.Add(SettingsButton, SettingsDataAsset);
	FooterDataMap.Add(CommandsButton, CommandsDataAsset);
}
