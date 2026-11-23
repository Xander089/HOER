// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HOER/Public/UI/Widgets/Common/HOERPageWidget.h"
#include "UI/Widgets/Common/PageFooterWidget.h"
#include "HOERPausePageWidget.generated.h"

class UHOERButtonWidget;

/**
 * Pause Menu page widget presented during active gameplay, enabling resume, reloading checkpoints,
 * accessing settings, and returning to the main menu.
 */
UCLASS()
class HOER_API UHOERPausePageWidget : public UHOERPageWidget
{
	GENERATED_BODY()

public:
	
	/** Blueprint event triggered when Restart / Return to Main Menu button is clicked. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Back To Main Menu Button Clicked")
	void BP_OnReStartButtonClicked();

	/** Blueprint event triggered when Continue / Resume button is clicked. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Continue Button Clicked")
	void BP_OnContinueButtonClicked();
	
	/** Blueprint event triggered when Reload from Last Checkpoint button is clicked. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On ReLoad From Last CheckPoint Button Clicked")
	void BP_OnLoadButtonClicked();

	/** Blueprint event triggered when Settings button is clicked. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Settings Button Clicked")
	void BP_OnSettingsButtonClicked();

	/** Blueprint event triggered when Commands button is clicked. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Commands Button Clicked")
	void BP_OnCommandsButtonClicked();

	/** Blueprint event triggered when Restart / Return to Main Menu button acquires focus. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Back To Main Menu Button Focused")
	void BP_OnReStartButtonFocused();

	/** Blueprint event triggered when Continue / Resume button acquires focus. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Continue Button Focused")
	void BP_OnContinueButtonFocused();
	
	/** Blueprint event triggered when Reload from Last Checkpoint button acquires focus. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On ReLoad From Last CheckPoint Button Focused")
	void BP_OnLoadButtonFocused();

	/** Blueprint event triggered when Settings button acquires focus. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Settings Button Focused")
	void BP_OnSettingsButtonFocused();

	/** Blueprint event triggered when Commands button acquires focus. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Commands Button Focused")
	void BP_OnCommandsButtonFocused();

	virtual void OnPageRemovedFromTop() override;
	
protected:
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void CacheDataAssets() override;

	/** Bound button resuming active gameplay. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHOERButtonWidget> ContinueButton = nullptr;
	
	/** Bound button reloading the latest save slot. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHOERButtonWidget> LoadButton = nullptr;

	/** Bound button opening settings/audio options. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHOERButtonWidget> SettingsButton = nullptr;

	/** Bound button returning player to the main menu. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHOERButtonWidget> RestartButton = nullptr;

	/** Bound button reviewing game controls. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHOERButtonWidget> CommandsButton = nullptr;

	/** Footer asset for Continue button. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
	TObjectPtr<UFooterDataAsset> ContinueDataAsset = nullptr;
	
	/** Footer asset for Load Checkpoint button. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
	TObjectPtr<UFooterDataAsset> LoadDataAsset = nullptr;
	
	/** Footer asset for Settings button. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
	TObjectPtr<UFooterDataAsset> SettingsDataAsset = nullptr;
	
	/** Footer asset for Restart / Main Menu button. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
	TObjectPtr<UFooterDataAsset> RestartDataAsset = nullptr;

	/** Footer asset for Commands button. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
	TObjectPtr<UFooterDataAsset> CommandsDataAsset = nullptr;

private:
	
	UFUNCTION()
	void OnRestartButtonClicked(UHOERButtonWidget* InButton);
	
	UFUNCTION()
	void OnContinueButtonClicked(UHOERButtonWidget* InButton);
	
	UFUNCTION()
	void OnLoadButtonClicked(UHOERButtonWidget* InButton);
	
	UFUNCTION()
	void OnSettingsButtonClicked(UHOERButtonWidget* InButton);

	UFUNCTION()
	void OnCommandsButtonClicked(UHOERButtonWidget* InButton);

	UFUNCTION()
	void OnRestartButtonFocused(UHOERButtonWidget* InButton);
	
	UFUNCTION()
	void OnContinueButtonFocused(UHOERButtonWidget* InButton);
	
	UFUNCTION()
	void OnLoadButtonFocused(UHOERButtonWidget* InButton);
	
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
