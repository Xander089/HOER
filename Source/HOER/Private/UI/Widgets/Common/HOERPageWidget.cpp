// Fill out your copyright notice in the Description page of Project Settings.

#include "HOER/Public/UI/Widgets/Common/HOERPageWidget.h"
#include "GameFramework/PlayerController.h"
#include "UI/Widgets/Common/PageFooterWidget.h"

void UHOERPageWidget::CacheFocusedWidget(UWidget* InWidget)
{
	LastFocusedWidget = InWidget;
}

void UHOERPageWidget::RestoreFocus() const
{
	if (!LastFocusedWidget.IsValid()){ return;}
	
	// Delayed to the next tick: the page has just been restored, this allows to find a valid focus path to the widget
	TWeakObjectPtr<UWidget> WeakWidgetToFocus = LastFocusedWidget;
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this, WeakWidgetToFocus]()
	{
		if (!WeakWidgetToFocus.IsValid()){ return; }

		if (APlayerController* PC = GetOwningPlayer();IsValid(PC)){ 
			WeakWidgetToFocus.Get()->SetUserFocus(PC);
		}
	}));
}

void UHOERPageWidget::OnPageAdded()
{
}

void UHOERPageWidget::OnPageRemoved()
{
}

void UHOERPageWidget::OnPageRemovedFromTop()
{
	
	//Restore Footer Visibility
	if (IsValid(Footer))
	{
		Footer->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UHOERPageWidget::NotifyGamepadConnectionChanged(bool bIsConnected)
{
	if(!LastFocusedWidget.IsValid()) {return;}

	const TObjectPtr<UFooterDataAsset>* CurrentDataAssetPtr = FooterDataMap.Find(LastFocusedWidget.Get());
	if(!CurrentDataAssetPtr) {return;}

	const TObjectPtr<UFooterDataAsset> CurrentDataAsset = *CurrentDataAssetPtr;
	if(!CurrentDataAsset) {return;}

	if(IsValid(Footer)){
		Footer->SetData(CurrentDataAsset->ConfigData, bIsConnected);
	}
}

void UHOERPageWidget::CacheDataAssets()
{
}
