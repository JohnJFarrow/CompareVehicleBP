// Copyright John Farrow (c) 2023. All Rights Reserved.

#include "DifferenceTile.h"
#include "CompareVehicleBlueprintsStyle.h"
#include "Difference.h"
#include "Widgets/Layout/SGridPanel.h"

namespace
{
	constexpr int32 Padding = 10;
	const FMargin TopSlotMargin(10, 10, 1, 10);
	const FMargin BottomSlotMargin(10, 0, 10, 10);
	constexpr int32 SpaceBetweenItems = 1;
	const FSlateColor OuterBorderColor(FLinearColor::Gray);
	const FMargin BorderTopMargin(0, 0, 0, SpaceBetweenItems);

	TSharedRef<SWidget> WidgetForMessage(
		const FString& StyleName, 
		const FString& Message
		)
	{
		return SNew(SBorder)
			.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
			.BorderBackgroundColor(OuterBorderColor)
			.Padding(BorderTopMargin)
			.VAlign(VAlign_Top)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
				.Padding(0)
				.VAlign(VAlign_Top)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.Padding(Padding)
					.HAlign(HAlign_Left)
					.AutoHeight()
					[
						SNew(STextBlock)
						.TextStyle(FCompareVehicleBlueprintsStyle::Get(), *StyleName)
						.Text(FText::FromString(Message))
						.AutoWrapText(true)
					]
				]
			];

	}
}

void SDifferenceTile::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	// set the item, then get callbacks to fill the columns in GenerateWidgetForColumn()
	Item = InArgs._InItem;
	FSuperRowType::Construct(FSuperRowType::FArguments().Padding(0), InOwnerTableView);
}

TSharedRef<SWidget> SDifferenceTile::GenerateWidgetForColumn(const FName& ColumnName)
{

	if (ColumnName == TEXT("Property"))
	{
		if (Item->Type == EDifferenceType::Info)
		{
			return WidgetForMessage("Difference.InfoText", Item->Message);
		}
		
		if (Item->Type == EDifferenceType::Warning)
		{
			return WidgetForMessage("Difference.WarningText", Item->Message);
		}
		
		if (Item->Type == EDifferenceType::Error)
		{
			return WidgetForMessage("Difference.ErrorText", Item->Message);
		}
		
		if (Item->Type == EDifferenceType::Difference)
		{
			return SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
				.BorderBackgroundColor(OuterBorderColor)
				.Padding(BorderTopMargin)
				.VAlign(VAlign_Top)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
					.Padding(0)
					.VAlign(VAlign_Top)
					[
						SNew(SVerticalBox)
				
						+ SVerticalBox::Slot()
						.Padding(TopSlotMargin)
						.HAlign(HAlign_Left)
						.AutoHeight()
						[
							SNew(STextBlock)
							.Text(FText::FromString(Item->Paths[0]))
						]

						+ SVerticalBox::Slot()
						.HAlign(HAlign_Left)
						.Padding(BottomSlotMargin)
						.AutoHeight()
						[
							SNew(STextBlock)
							.Text(FText::FromString(Item->Paths[1]))
						]
					]
				];

		}
	}
	else if( ColumnName == TEXT("Value") )
	{
		// for messages the text is in the left column, do this to generate the border and backgrounds
		if (Item->Type == EDifferenceType::Info)
		{
			TSharedRef<SWidget> W = WidgetForMessage("Difference.InfoText", "");
			W->SetVisibility(EVisibility::Collapsed);
			return W;
		}

		if (Item->Type == EDifferenceType::Warning)
		{
			TSharedRef<SWidget> W = WidgetForMessage("Difference.WarningText", "");
			W->SetVisibility(EVisibility::Collapsed);
			return W;
		}

		if (Item->Type == EDifferenceType::Error)
		{
			TSharedRef<SWidget> W = WidgetForMessage("Difference.ErrorText", "");
			W->SetVisibility(EVisibility::Collapsed);
			return W;
		}


		if (Item->Type == EDifferenceType::Difference)
		{
			return SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
				.BorderBackgroundColor(OuterBorderColor)
				.Padding(BorderTopMargin)
				.VAlign(VAlign_Top)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
					.Padding(0)
					.VAlign(VAlign_Top)
					[
						SNew(SVerticalBox)
				
						+ SVerticalBox::Slot()
						.Padding(TopSlotMargin)
						.HAlign(HAlign_Left)
						.AutoHeight()
						[
							SNew(STextBlock)
							.Text(FText::FromString(Item->ValuesAsString[0]))
						]

						+ SVerticalBox::Slot()
						.HAlign(HAlign_Left)
						.Padding(BottomSlotMargin)
						.AutoHeight()
						[
							SNew(STextBlock)
							.Text(FText::FromString(Item->ValuesAsString[1]))
						]
					]
				];

		}
	}

	return SNew(STextBlock);
}
