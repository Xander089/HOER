// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HOER/Public/UI/Widgets/Common/HOERPageWidget.h"
#include "HOERMainMenuPageWidget.generated.h"

class UFooterDataAsset;
class UHOERButtonWidget;

DECLARE_DYNAMIC_DELEGATE(FOnLoadBtnClicked);

/**
 * Main Menu full-screen page widget handling navigation to Start Game, Continue, Settings,
 * Controls/Commands, Credits, and Quit options.
 */
UCLASS()
class HOER_API UHOERMainMenuPageWidget : public UHOERPageWidget
{
	GENERATED_BODY()
	
public:
	
	/** Blueprint event triggered when Start Game button is clicked. Override to start a new campaign. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Start Button Clicked")
	void BP_OnStartButtonClicked();
	
	/** Blueprint event triggered when Quit button is clicked. Override to prompt or exit game. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Quit Button Clicked")
	void BP_OnQuitButtonClicked();

	/** Blueprint event triggered when Continue button is clicked. Override to load the latest save slot. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Continue Button Clicked")
	void BP_OnContinueButtonClicked();
	
	/** Blueprint event triggered when Credits button is clicked. Override to push the credits page. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Credits Button Clicked")
	void BP_OnCreditsButtonClicked();

	/** Blueprint event triggered when Settings button is clicked. Override to push the settings page. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Settings Button Clicked")
	void BP_OnSettingsButtonClicked();

	/** Blueprint event triggered when Commands button is clicked. Override to push the controls page. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Commands Button Clicked")
	void BP_OnCommandsButtonClicked();
	
	/** Blueprint event triggered when Start Game button receives focus. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Start Button Focused")
	void BP_OnStartButtonFocused();
	
	/** Blueprint event triggered when Quit button receives focus. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Quit Button Focused")
	void BP_OnQuitButtonFocused();

	/** Blueprint event triggered when Continue button receives focus. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Continue Button Focused")
	void BP_OnContinueButtonFocused();
	
	/** Blueprint event triggered when Credits button receives focus. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Credits Button Focused")
	void BP_OnCreditsButtonFocused();

	/** Blueprint event triggered when Settings button receives focus. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Settings Button Focused")
	void BP_OnSettingsButtonFocused();

	/** Blueprint event triggered when Commands button receives focus. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Commands Button Focused")
	void BP_OnCommandsButtonFocused();

	virtual void OnPageRemovedFromTop() override;
	
protected:
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void CacheDataAssets() override;
	
	/** Bound button widget for starting a new game session. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHOERButtonWidget> StartButton = nullptr;
	
	/** Bound button widget for resuming from the latest checkpoint save. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHOERButtonWidget> ContinueButton = nullptr;
	
	/** Bound button widget navigating to options and settings. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHOERButtonWidget> SettingsButton = nullptr;
	
	/** Bound button widget opening the attribution credits page. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHOERButtonWidget> CreditsButton = nullptr;
	
	/** Bound button widget to exit the game. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHOERButtonWidget> QuitButton = nullptr;

	/** Bound button widget opening the key commands / controls page. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHOERButtonWidget> CommandsButton = nullptr;
	
	/** Footer prompt asset for New Game button. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
	TObjectPtr<UFooterDataAsset> NewGameDataAsset = nullptr;
	
	/** Footer prompt asset for Continue button. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
	TObjectPtr<UFooterDataAsset> ContinueDataAsset = nullptr;
	
	/** Footer prompt asset for Credits button. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
	TObjectPtr<UFooterDataAsset> CreditsDataAsset = nullptr;
	
	/** Footer prompt asset for Settings / Options button. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
	TObjectPtr<UFooterDataAsset> OptionsDataAsset = nullptr;
	
	/** Footer prompt asset for Quit button. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
	TObjectPtr<UFooterDataAsset> QuitDataAsset = nullptr;

	/** Footer prompt asset for Commands button. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
	TObjectPtr<UFooterDataAsset> CommandsDataAsset = nullptr;

private:
	
	UFUNCTION()
	void OnStartButtonClicked(UHOERButtonWidget* InButton);

	UFUNCTION()
	void OnQuitButtonClicked(UHOERButtonWidget* InButton);
	
	UFUNCTION()
	void OnContinueButtonClicked(UHOERButtonWidget* InButton);
	
	UFUNCTION()
	void OnCreditsButtonClicked(UHOERButtonWidget* InButton);
	
	UFUNCTION()
	void OnSettingsButtonClicked(UHOERButtonWidget* InButton);

	UFUNCTION()
	void OnCommandsButtonClicked(UHOERButtonWidget* InButton);

	UFUNCTION()
	void OnStartButtonFocused(UHOERButtonWidget* InButton);

	UFUNCTION()
	void OnQuitButtonFocused(UHOERButtonWidget* InButton);
	
	UFUNCTION()
	void OnContinueButtonFocused(UHOERButtonWidget* InButton);
	
	UFUNCTION()
	void OnCreditsButtonFocused(UHOERButtonWidget* InButton);
	
	UFUNCTION()
	void OnSettingsButtonFocused(UHOERButtonWidget* InButton);

	UFUNCTION()
	void OnCommandsButtonFocused(UHOERButtonWidget* InButton);
	
	void SetBindings();
	void RemoveBindings();
	void CacheButtons();
	void SetFocus(UHOERButtonWidget* InButton) const;
	void OnPageAddedOnTop(const UHOERButtonWidget* InButton);
	
	// TArray to cache a weak reference to the buttons
	UPROPERTY()
	TArray<TWeakObjectPtr<UHOERButtonWidget>> Buttons;
};
