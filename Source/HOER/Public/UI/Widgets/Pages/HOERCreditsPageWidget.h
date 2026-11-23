#pragma once

#include "CoreMinimal.h"
#include "HOER/Public/UI/Widgets/Common/HOERPageWidget.h"
#include "HOERCreditsPageWidget.generated.h"

class UHOERCreditsDataAsset;
class UCreditRowWidget;
class UScrollBox;

/**
 * Credits menu page widget automatically scrolling through attribution categories and team members.
 */
UCLASS()
class HOER_API UHOERCreditsPageWidget : public UHOERPageWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** Instantiates credit rows from the assigned credits data asset and adds them to the scrollbox. */
	void PopulateCredits();

	/**
	 * Blueprint native event triggered when credit roll completes and hits the bottom of the scroll view.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Credits")
	void OnCreditsEnded();
	virtual void OnCreditsEnded_Implementation();

	/** Bound ScrollBox container housing dynamically instantiated credit rows. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UScrollBox> CreditsScrollBox = nullptr;

	/** Scrolling speed in Slate units per second. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits|Config")
	float ScrollSpeed = 50.0f;

	/** Data Asset holding credits categories and names. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits|Config")
	TObjectPtr<UHOERCreditsDataAsset> CreditsDataAsset = nullptr;

	/** Widget class to instantiate for each credit row. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits|Config")
	TSubclassOf<UCreditRowWidget> CreditRowClass = nullptr;

private:
	float CurrentScrollOffset = 0.0f;
	bool bHasReachedEnd = false;
};
