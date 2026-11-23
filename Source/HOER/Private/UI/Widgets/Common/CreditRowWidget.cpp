#include "HOER/Public/UI/Widgets/Common/CreditRowWidget.h"
#include "Components/TextBlock.h"
#include "Data/UI/HOERCreditsDataAsset.h"

void UCreditRowWidget::SetCreditData(const FText& InCategory, const TArray<FHOERCreditPerson>& InPeople) const
{
	if (IsValid(CategoryText))
	{
		CategoryText->SetText(InCategory);
		// Force the category text to be centered
		CategoryText->SetJustification(ETextJustify::Center);
	}

	if (IsValid(NamesText))
	{
		FString CombinedNames;

		for (int32 i = 0; i < InPeople.Num(); ++i)
		{
			const auto& [Name, Surname, Github] = InPeople[i];
            
			FString PersonLine = FString::Printf(TEXT("%s %s"), *Name, *Surname);

			if (!Github.IsEmpty())
			{
				PersonLine += FString::Printf(TEXT("\n(%s)"), *Github);
			}

			CombinedNames += PersonLine;
            
			if (i < InPeople.Num() - 1)
			{
				CombinedNames += TEXT("\n\n");
			}
		}
        
		NamesText->SetText(FText::FromString(CombinedNames));
       
		// Force the names text to be centered
		NamesText->SetJustification(ETextJustify::Center);
	}
}
