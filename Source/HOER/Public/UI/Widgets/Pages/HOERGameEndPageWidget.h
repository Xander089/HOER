// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/Common/HOERPageWidget.h"
#include "HOERGameEndPageWidget.generated.h"

/**
 * Victory / Game End screen page widget displayed upon successful completion of the game or level.
 */
UCLASS()
class HOER_API UHOERGameEndPageWidget : public UHOERPageWidget
{
	GENERATED_BODY()
	
public:

	UHOERGameEndPageWidget(const FObjectInitializer& ObjectInitializer);

	/** Blueprint event triggered when player clicks Retry / Play Again. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Retry Button Clicked")
	void BP_OnRetryButtonClicked();

	/** Blueprint event triggered when player clicks Back to Main Menu. */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Main Menu Button Clicked")
	void BP_OnMainMenuButtonClicked();

protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** Bound button to restart or replay the level. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHOERButtonWidget> RetryButton = nullptr;

	/** Bound button to return to the title screen. */
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
