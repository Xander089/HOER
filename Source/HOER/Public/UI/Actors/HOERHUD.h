// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "System/GameTypes.h"
#include "HOERHUD.generated.h"

class UHOERHUDWidget;
class UInputMappingContext;
class UHOERBaseWidget;
class UHOERPageWidget;

// Pages Forward Declaration
class UHOERMainMenuPageWidget;
class UHOERPausePageWidget;
class ULoadPageWidget;
class USettingsPageWidget;

/** Delegate broadcast when a back or cancel navigation action is requested. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBackRequest);

/**
 * Custom HUD actor managing the hierarchical stack of UI pages (main menu, settings, pause, game over),
 * screen focus restoration, and toast notifications.
 */
UCLASS()
class HOER_API AHOERHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	/** Broadcast when the player inputs a back/cancel navigation command. */
	UPROPERTY(BlueprintAssignable, Category = "HUD Events")
	FOnBackRequest OnBackRequest;
	
	/**
	 * Instantiates or activates a UI page widget, pushing it onto the HUD stack and displaying it on viewport.
	 *
	 * @param InPageClass The widget blueprint class of the page to instantiate.
	 * @param InPageName Unique identifier name for caching and referencing this page.
	 * @param InZOrder Viewport Z-order priority.
	 * @param bSwapContext Whether the player controller should swap to UI input mapping context.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	void AddPage(TSubclassOf<UHOERPageWidget> InPageClass, const FName& InPageName, int32 InZOrder, bool bSwapContext = true);

	/**
	 * Pops the top page from the HUD stack and restores focus to the underlying page or gameplay context.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game HUD")
	void RemovePage();
	
	/**
	 * Forwards controller connection change notifications to the active top page.
	 *
	 * @param bIsConnected True if gamepad connected, false if disconnected.
	 */
	void NotifyGamepadConnectionChanged(bool bIsConnected);

	/* Updates the input mode based on the current page */
	void UpdateInputMode();

	/**
	 * Displays a timed toast notification on screen.
	 *
	 * @param InToastWidgetSubClass The widget class to spawn.
	 * @param Message Localized message text.
	 * @param Duration Time in seconds to display before auto-dismissal.
	 */
	void ShowToastNotification(TSubclassOf<UHOERBaseWidget> InToastWidgetSubClass, const FText& Message, float Duration);

	/** Default gameplay HUD widget class displayed during active gameplay. */
	UPROPERTY(EditDefaultsOnly, Category = "Game HUD")
	TSubclassOf<UHOERHUDWidget> HUDWidgetClass;
	
protected:

	virtual void BeginPlay() override;

private:

	/* Creates a new widget instance */
	UHOERBaseWidget* CreateWidget(TSubclassOf<UHOERBaseWidget> InWidgetClass);
	UHOERBaseWidget* CreateAndAddWidget(TSubclassOf<UHOERBaseWidget> InWidgetClass, int32 InZOrder);
	/* Caches a page widget instance in the map */
	UHOERPageWidget* CachePage(const TSubclassOf<UHOERPageWidget> InPageClass, const FName& InPageName);
	
	/* Map to store cached page widgets */
	UPROPERTY()
	TMap<FName, TObjectPtr<UHOERPageWidget>> PagesMap;

	/* Array to store the pages stack */
	UPROPERTY()
	TArray<TObjectPtr<UHOERPageWidget>> PagesStack;

	/* Removes the top page only if it can be closed by the Back input action */
	UFUNCTION()
	void HandleBackRequest();

	/* Method called to swap input mapping context when a UI page is added or removed */
	void SwapInputMappingContext(GameTypes::EInputType InInputType) const;
	void CreateHUDWidget();
	void RemoveToastNotification() const;
	
	UPROPERTY()
	TObjectPtr<UHOERHUDWidget> HUDWidget = nullptr;

	UPROPERTY()
	TObjectPtr<UHOERBaseWidget> ToastWidget = nullptr;
	
	FTimerHandle ToastTimerHandle;
};
