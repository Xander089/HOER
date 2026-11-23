// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Pages/HOERGameEndPageWidget.h"

#include "UI/Widgets/Common/HOERButtonWidget.h"
#include "UI/Widgets/Common/PageFooterWidget.h"

UHOERGameEndPageWidget::UHOERGameEndPageWidget(const FObjectInitializer& ObjectInitializer)
{
	// Game Over page can't be closed
	bCanBeClosedByBack = false;
}

void UHOERGameEndPageWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetBindings();

	if (IsValid(Footer))
	{
		Footer->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UHOERGameEndPageWidget::NativeDestruct()
{
	RemoveBindings();
	Super::NativeDestruct();
}

void UHOERGameEndPageWidget::OnRetryButtonClicked(UHOERButtonWidget* InButton)
{
	BP_OnRetryButtonClicked();
	CacheFocusedWidget(InButton->GetButton());
}

void UHOERGameEndPageWidget::OnMainMenuButtonClicked(UHOERButtonWidget* InButton)
{
	BP_OnMainMenuButtonClicked();
	CacheFocusedWidget(InButton->GetButton());
}

void UHOERGameEndPageWidget::SetBindings()
{
	if (IsValid(RetryButton))
	{
		RetryButton->OnButtonClicked.AddDynamic(this, &UHOERGameEndPageWidget::OnRetryButtonClicked);
	}

	if (IsValid(MainMenuButton))
	{
		MainMenuButton->OnButtonClicked.AddDynamic(this, &UHOERGameEndPageWidget::OnMainMenuButtonClicked);
	}
}

void UHOERGameEndPageWidget::RemoveBindings()
{
	if (IsValid(RetryButton))
	{
		RetryButton->OnButtonClicked.RemoveDynamic(this, &UHOERGameEndPageWidget::OnRetryButtonClicked);
	}

	if (IsValid(MainMenuButton))
	{
		MainMenuButton->OnButtonClicked.RemoveDynamic(this, &UHOERGameEndPageWidget::OnMainMenuButtonClicked);
	}
}
