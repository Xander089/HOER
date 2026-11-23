#include "HOER/Public/UI/Widgets/Pages/HOERCreditsPageWidget.h"
#include "HOER/Public/UI/Widgets/Common/CreditRowWidget.h"
#include "Components/ScrollBox.h"
#include "Data/UI/HOERCreditsDataAsset.h"
#include "System/HOERBlueprintFunctionLibrary.h"

void UHOERCreditsPageWidget::NativeConstruct()
{
    Super::NativeConstruct();

    bHasReachedEnd = false;
    CurrentScrollOffset = 0.0f;
    
    if (IsValid(CreditsScrollBox))
    {
        CreditsScrollBox->SetScrollOffset(CurrentScrollOffset);
    }

    PopulateCredits();
}

#include "Components/Spacer.h" // Add this at the very top of your .cpp file

void UHOERCreditsPageWidget::PopulateCredits()
{
    if (!IsValid(CreditsScrollBox) || !IsValid(CreditsDataAsset) || !IsValid(CreditRowClass))
    {
        return;
    }

    CreditsScrollBox->ClearChildren();

    // Add a massive spacer at the TOP so the first text block starts off-screen
    USpacer* TopSpacer = NewObject<USpacer>(this);
    TopSpacer->SetSize(FVector2D(1.0f, 1080.0f)); // 1080 is the height of a 1080p screen
    CreditsScrollBox->AddChild(TopSpacer);

    // Generate the text rows
    for (const auto& [CurrentCategoryName, People] : CreditsDataAsset->Credits)
    {
        if (UCreditRowWidget* RowWidget = CreateWidget<UCreditRowWidget>(this, CreditRowClass); IsValid(RowWidget))
        {
            RowWidget->SetCreditData(CurrentCategoryName, People);
            CreditsScrollBox->AddChild(RowWidget);
        }
    }

    // Add a massive spacer at the BOTTOM so the last text block can scroll fully off-screen
    USpacer* BottomSpacer = NewObject<USpacer>(this);
    BottomSpacer->SetSize(FVector2D(1.0f, 1080.0f));
    CreditsScrollBox->AddChild(BottomSpacer);
}

void UHOERCreditsPageWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!IsValid(CreditsScrollBox) || bHasReachedEnd)
    {
        return;
    }

    // Stop at the end of the scrollbox and trigger the end event
    if (CurrentScrollOffset >= CreditsScrollBox->GetScrollOffsetOfEnd() && CreditsScrollBox->GetScrollOffsetOfEnd() > 0.0f)
    {
        bHasReachedEnd = true;
        OnCreditsEnded(); // Trigger the C++ / BP Native event
        return;
    }
    
    // Update the offset
    CurrentScrollOffset = FMath::Min(CurrentScrollOffset + (ScrollSpeed * InDeltaTime), CreditsScrollBox->GetScrollOffsetOfEnd());
    
    // Scroll
    CreditsScrollBox->SetScrollOffset(CurrentScrollOffset);
}

void UHOERCreditsPageWidget::OnCreditsEnded_Implementation()
{
    UHOERBlueprintFunctionLibrary::RemovePage(this);
}