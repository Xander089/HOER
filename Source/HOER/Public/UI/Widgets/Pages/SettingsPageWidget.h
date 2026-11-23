// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/Common/HOERPageWidget.h"
#include "SettingsPageWidget.generated.h"

class USliderRowWidget;

/**
 * Settings menu page widget hosting volume sliders for background music and sound effects,
 * automatically applying and saving user audio preferences.
 */
UCLASS()
class HOER_API USettingsPageWidget : public UHOERPageWidget
{
	GENERATED_BODY()
	
public:
	
	/**
	 * Blueprint event triggered when music volume slider is adjusted.
	 *
	 * @param Value Normalized music volume value between 0.0 and 1.0.
	 */
	UFUNCTION(BlueprintImplementableEvent, DisplayName="Handle On Music Value Changed")
	void BP_HandleOnMusicValueChanged(float Value);

	/**
	 * Blueprint event triggered when SFX volume slider is adjusted.
	 *
	 * @param Value Normalized SFX volume value between 0.0 and 1.0.
	 */
	UFUNCTION(BlueprintImplementableEvent, DisplayName="Handle On SFX Value Changed")
	void BP_HandleOnSFXValueChanged(float Value);
	
	/**
	 * Blueprint event triggered when this settings page is closed or removed from the viewport.
	 * Use in Blueprints to save user settings to disk.
	 */
	UFUNCTION(BlueprintImplementableEvent, DisplayName="Handle On Remove Page")
	void BP_HandleOnPageRemoved();
	
	/**
	 * Blueprint event triggered when this settings page is opened or added to viewport.
	 * Use in Blueprints to initialize slider values from saved user settings.
	 */
	UFUNCTION(BlueprintImplementableEvent, DisplayName="Handle On Add Page")
	void BP_HandleOnPageAdded();
	
	virtual void OnPageAdded() override;
	virtual void OnPageRemoved() override;
	
protected:
	
	UFUNCTION()
	void HandleOnMusicValueChanged(float Value);

	UFUNCTION()
	void HandleOnSFXValueChanged(float Value);
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativePreConstruct() override;
	
	/** Bound slider row controlling music master volume. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USliderRowWidget> MusicSlider = nullptr;

	/** Bound slider row controlling sound effects master volume. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USliderRowWidget> SFXSlider = nullptr;
};
