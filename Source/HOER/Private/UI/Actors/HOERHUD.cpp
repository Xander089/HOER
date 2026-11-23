// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Actors/HOERHUD.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Controller/HOERPlayerController.h"
#include "UI/Widgets/Common/HOERBaseWidget.h"
#include "UI/Widgets/Common/HOERPageWidget.h"
#include "UI/Widgets/HUD/HOERHUDWidget.h"
#include "UI/Widgets/Pages/HOERMainMenuPageWidget.h"

void AHOERHUD::BeginPlay()
{
	Super::BeginPlay();
	
	CreateHUDWidget();
	OnBackRequest.AddDynamic(this, &AHOERHUD::HandleBackRequest);

}

void AHOERHUD::AddPage(const TSubclassOf<UHOERPageWidget> InPageClass, const FName& InPageName, int32 InZOrder, bool bSwapContext)
{
	// 0. Try to find 
	const TObjectPtr<UHOERPageWidget>* PageToAddPtr = PagesMap.Find(InPageName);
	
	TObjectPtr<UHOERPageWidget> PageToAdd = nullptr;
		
	// 1. Cache the page in the map if not found, else dereference it
	if (PageToAddPtr == nullptr)
	{
		PageToAdd = CachePage(InPageClass, InPageName);
	}
	else
	{
		PageToAdd = *PageToAddPtr;
	}
	
	// 2. Check validity of the cached page
	if (!ensureMsgf(PageToAdd, TEXT("No valid page with name %s"), *InPageName.ToString()))
	{
		return;
	}
	
	// Block mouse input to the covered page.
	if (PagesStack.Num() > 0)
	{
		if (UHOERPageWidget* CoveredPage = PagesStack.Last(); IsValid(CoveredPage) && CoveredPage != PageToAdd)
		{
			CoveredPage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}

	// 3. Add to viewport, remembering which input context the page needs when it is back on top
	PageToAdd->SetSwapsInputContext(bSwapContext);
	PageToAdd->AddToViewport(InZOrder);
	PageToAdd->OnPageAdded();
	
	// 4. Add the page to the stack
	PagesStack.Emplace(PageToAdd);
	
	// 5. Set focus to the new top page
	APlayerController* PC = GetOwningPlayerController();
	if (IsValid(PC))
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(PageToAdd->TakeWidget());
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(true);
		
		if (bSwapContext)
		{
			SwapInputMappingContext(GameTypes::EInputType::UI);
		}
	}
}

void AHOERHUD::RemovePage()
{
	// 1. If stack is empty, early return
	if (PagesStack.Num() == 0)
	{
		return;
	}

	// 2. Remove the top page if valid
	UHOERPageWidget* TopWidget = PagesStack.Pop();
	if (!ensure(TopWidget))
	{
		return;
	}
	TopWidget->RemoveFromParent();
	TopWidget->OnPageRemoved();


	// 3. Restore the previous page visibility, if valid
	if (PagesStack.Num() > 0)
	{
		if (UHOERPageWidget* NewTop = PagesStack.Last(); IsValid(NewTop))
		{
			NewTop->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			NewTop->RestoreFocus();
			NewTop->OnPageRemovedFromTop();
		}
	}

	UpdateInputMode();
}

void AHOERHUD::UpdateInputMode()
{
	APlayerController* PC = GetOwningPlayerController();
	if (!IsValid(PC)) {return;}

	// If Stack empty, Switch to game input
	if (PagesStack.Num() ==  0)
	{
		const FInputModeGameOnly Mode;
		PC->SetInputMode(Mode);
		PC->SetShowMouseCursor(false);
		SwapInputMappingContext(GameTypes::EInputType::GamePlay);
		return;
	}
	
	// Else, Switch to UI input, restoring the mapping context requested by the top page
	if (UHOERPageWidget* TopPage = PagesStack.Last(); IsValid(TopPage))
	{
		FInputModeGameAndUI Mode;
		Mode.SetWidgetToFocus(TopPage->TakeWidget());
		Mode.SetHideCursorDuringCapture(false);
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SwapInputMappingContext(TopPage->SwapsInputContext() ? GameTypes::EInputType::UI : GameTypes::EInputType::GamePlay);
		PC->SetInputMode(Mode);
		PC->SetShowMouseCursor(true);
	}
}

void AHOERHUD::SwapInputMappingContext(GameTypes::EInputType NewInputType) const
{
	APlayerController* PC = GetOwningPlayerController();
	if (!IsValid(PC)) {return;}
	
	//Cast to HOERPlayerController to call the SwapInputMappingContext method
	if (AHOERPlayerController* HOERPC = Cast<AHOERPlayerController>(PC); IsValid(HOERPC))
	{
		HOERPC->SwapInputMappingContext(NewInputType);
	}
}

void AHOERHUD::CreateHUDWidget()
{
	HUDWidget = Cast<UHOERHUDWidget>(CreateAndAddWidget(HUDWidgetClass, 100));
}

UHOERBaseWidget* AHOERHUD::CreateWidget(const TSubclassOf<UHOERBaseWidget> InWidgetClass)
{
	if (!ensureMsgf(InWidgetClass, TEXT("Invalid Widget Class")))
	{
		return nullptr;
	}

	UHOERBaseWidget* Widget = Cast<UHOERBaseWidget>(
		UWidgetBlueprintLibrary::Create(this, InWidgetClass, GetOwningPlayerController()));

	if (!ensureMsgf(IsValid(Widget), TEXT("Widget creation failed")))
	{
		return nullptr;
	}

	return Widget;
}

UHOERBaseWidget* AHOERHUD::CreateAndAddWidget(TSubclassOf<UHOERBaseWidget> InWidgetClass, int32 InZOrder)
{
	UHOERBaseWidget* Widget = Cast<UHOERBaseWidget>(CreateWidget(InWidgetClass));
	Widget->AddToViewport(InZOrder);
	return Widget;
}

UHOERPageWidget* AHOERHUD::CachePage(const TSubclassOf<UHOERPageWidget> InPageClass, const FName& InPageName)
{
	if (PagesMap.Contains(InPageName))
	{
		ensureMsgf(false, TEXT("Page already exists!"));
		return nullptr;
	}

	UHOERPageWidget* PageToAdd = Cast<UHOERPageWidget>(CreateWidget(InPageClass));
	if (!ensure(IsValid(PageToAdd)))
	{
		return nullptr;
	}

	PagesMap.Emplace(InPageName, PageToAdd);
	return PageToAdd;
}


void AHOERHUD::NotifyGamepadConnectionChanged(const bool bIsConnected){
   
	if(PagesStack.Num() == 0){ return; }

	if(const TObjectPtr<UHOERPageWidget> TopPage = PagesStack.Top(); IsValid(TopPage))
	{
		TopPage->NotifyGamepadConnectionChanged(bIsConnected);
	}
	
}

void AHOERHUD::ShowToastNotification(
	TSubclassOf<UHOERBaseWidget> InToastWidgetSubClass, 
	const FText& Message,
	float Duration)
{
	if(!ToastWidget)
	{
		ToastWidget = CreateWidget(InToastWidgetSubClass);
	}
	if(!IsValid(ToastWidget)){ return; }
	
	// ToastWidget->SetText(Message);
	ToastWidget->RemoveFromParent();
	ToastWidget->AddToViewport(100);
   
	// Notification removed after <Duration> seconds - false: not looping
	GetWorldTimerManager().SetTimer(ToastTimerHandle, this, &AHOERHUD::RemoveToastNotification, Duration, false);
}

void AHOERHUD::RemoveToastNotification() const
{
	if(IsValid(ToastWidget)){
		ToastWidget->RemoveFromParent();
	}
}

void AHOERHUD::HandleBackRequest()
{
	if (PagesStack.Num() == 0)
	{
		return;
	}

	// Ignore Back if the top page doesn't allow it (e.g. Game Over)
	if (const UHOERPageWidget* TopPage = PagesStack.Last(); IsValid(TopPage) && !TopPage->CanBeClosedByBack())
	{
		return;
	}

	RemovePage();
}