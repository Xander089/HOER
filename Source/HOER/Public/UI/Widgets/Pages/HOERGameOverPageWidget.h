// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HOER/Public/UI/Widgets/Common/HOERPageWidget.h"
#include "HOERGameOverPageWidget.generated.h"

class UFooterDataAsset;
class UHOERButtonWidget;

/**
 * Game Over screen page widget displayed when the player fails or is caught, providing Retry and Main Menu options.
 */
UCLASS()
class HOER_API UHOERGameOverPageWidget : public UHOERPageWidget
{
	GENERATED_BODY()

public:

	UHOERGameOverPageWidget(const FObjectInitializer& ObjectInitializer);

	/** Blueprint event triggered when player clicks Retry to reload the checkpoint. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Retry Button Clicked")
	void BP_OnRetryButtonClicked();

	/** Blueprint event triggered when player clicks to return to the title screen. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Main Menu Button Clicked")
	void BP_OnMainMenuButtonClicked();

protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** Bound button to reload the last save point. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHOERButtonWidget> RetryButton = nullptr;

	/** Bound button to return to the main menu. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHOERButtonWidget> MainMenuButton = nullptr;

private:

	UFUNCTION()
	void OnRetryButtonClicked(UHOERButtonWidget* InButton);

	UFUNCTION()
	void OnMainMenuButtonClicked(UHOERButtonWidget* InButton);

	void SetBindings();
	void RemoveBindings();
};
