// Fill out your copyright notice in the Description page of Project Settings.


#include "HOER/Public/UI/Widgets/Pages/HOERGameOverPageWidget.h"
#include "UI/Widgets/Common/HOERButtonWidget.h"
#include "UI/Widgets/Common/PageFooterWidget.h"

UHOERGameOverPageWidget::UHOERGameOverPageWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Game Over page can't be closed
	bCanBeClosedByBack = false;
}

void UHOERGameOverPageWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetBindings();

	if (IsValid(Footer))
	{
		Footer->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UHOERGameOverPageWidget::NativeDestruct()
{
	RemoveBindings();
	Super::NativeDestruct();
}

void UHOERGameOverPageWidget::OnRetryButtonClicked(UHOERButtonWidget* InButton)
{
	BP_OnRetryButtonClicked();
	CacheFocusedWidget(InButton->GetButton());
}

void UHOERGameOverPageWidget::OnMainMenuButtonClicked(UHOERButtonWidget* InButton)
{
	BP_OnMainMenuButtonClicked();
	CacheFocusedWidget(InButton->GetButton());
}

void UHOERGameOverPageWidget::SetBindings()
{
	if (IsValid(RetryButton))
	{
		RetryButton->OnButtonClicked.AddDynamic(this, &UHOERGameOverPageWidget::OnRetryButtonClicked);
	}

	if (IsValid(MainMenuButton))
	{
		MainMenuButton->OnButtonClicked.AddDynamic(this, &UHOERGameOverPageWidget::OnMainMenuButtonClicked);
	}
}

void UHOERGameOverPageWidget::RemoveBindings()
{
	if (IsValid(RetryButton))
	{
		RetryButton->OnButtonClicked.RemoveDynamic(this, &UHOERGameOverPageWidget::OnRetryButtonClicked);
	}

	if (IsValid(MainMenuButton))
	{
		MainMenuButton->OnButtonClicked.RemoveDynamic(this, &UHOERGameOverPageWidget::OnMainMenuButtonClicked);
	}
}
