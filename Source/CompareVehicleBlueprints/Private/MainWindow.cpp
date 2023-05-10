// Copyright John Farrow (c) 2023. All Rights Reserved.

#include "MainWindow.h"
#include "CompareVehicleBlueprintsStyle.h"
#include "Widgets/Text/STextBlock.h"
#include "PropertyCustomizationHelpers.h"
#include "WheeledVehiclePawn.h"
#include "UIInputData.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "VehicleCompareImpl.h"
#include "DifferenceTile.h"
#include "Widgets/Layout/SScrollBox.h"

namespace {
#define LOCTEXT_NAMESPACE "CompareVehicleBlueprints"
}

SMainWindow::~SMainWindow()
{

}

void SMainWindow::Construct(const FArguments& Args, TSharedPtr < FInputData >& InInputData)
{
	check(InInputData);

	TSharedPtr ThumbnailPool = UThumbnailManager::Get().GetSharedThumbnailPool();

	InputData = InInputData;

	constexpr float HorizontalTilePadding = 20.0f;
	constexpr float VerticalTilePadding = 10.0f;
	constexpr float PaddingAmount = 12.0f;
	constexpr float ThumbnailImageSize = 32.0f;

	TSharedPtr<SObjectPropertyEntryBox> ObjectPropertyEntryBox;

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
		.Padding(FMargin(HorizontalTilePadding, VerticalTilePadding))
		.VAlign(VAlign_Top)
		[
			SAssignNew(VerticalBox, SVerticalBox)

			// instructions
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Bottom)
				[
					SNew(STextBlock)
					.Text( LOCTEXT("PresetVersionLabel", "Select two different vehicle blueprints and press the compare button"))
					.TextStyle(FCompareVehicleBlueprintsStyle::Get(), "Difference.GeneralText")
				]
			]
		]
	];

	// add two similar slots, maybe in the future add more

	for( int i = 0; i < 2; ++i )
	{
		FString Prompt = "Vehicle " + FString::FromInt(i + 1);
		VerticalBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(5)
			[
				SNew(STextBlock)
				.Text(FText::AsCultureInvariant(Prompt) )
				.TextStyle(FCompareVehicleBlueprintsStyle::Get(), "Difference.GeneralText")
			]

			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Bottom)
			.Padding(10)
			[

				SAssignNew(ObjectPropertyEntryBox, SObjectPropertyEntryBox)
				.AllowedClass(UBlueprint::StaticClass())
				.AllowCreate(false)
				.DisplayThumbnail(true)
				.DisplayBrowse(true)
				.ThumbnailPool(ThumbnailPool)
				.AllowClear(true)
				.ObjectPath_Lambda([&,i]() -> FString
				{
					if (InputData)
					{
						return InputData->VehicleAssetPaths[i];
					}

					return TEXT("");
				})
				.OnObjectChanged_Lambda([&,i](const FAssetData& AssetData) -> void
				{
					if (InputData && AssetData.IsValid())
					{
						InputData->VehicleAssetPaths[i] = AssetData.GetObjectPathString();
					}
				})
				.OnShouldFilterAsset_Lambda( [](const FAssetData & AssetData)->bool 
				{
					// does the blueprint create something which derives from AWheeledVehiclePawn
					if (AssetData.IsValid() )
					{
						if (AssetData.AssetClassPath.ToString() == "/Script/Engine.Blueprint")
						{
							UBlueprint* BlueprintAsset = Cast<UBlueprint>(AssetData.GetAsset());
							if (BlueprintAsset)
							{
								if (BlueprintAsset->ParentClass == AWheeledVehiclePawn::StaticClass())
								{
									return false;
								}

								if (BlueprintAsset->GeneratedClass)
								{
									const UObject* CDO = BlueprintAsset->GeneratedClass->GetDefaultObject();
									const AWheeledVehiclePawn* Pawn = Cast< AWheeledVehiclePawn >(CDO);
									if (Pawn)
									{
										return false;
									}
								}
							}
						}
					}
					return true;
				})
			]
		];
	}

	// add compare button
	constexpr int OverrideValueButtonColumnWidth = 120;

	const FText CompareButtonLabel = LOCTEXT("WindowWidgetText", "Compare");

	VerticalBox->AddSlot()
	.AutoHeight()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		[
			SNew(SSpacer)
		]

		+SHorizontalBox::Slot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			SNew(SBox)
			.WidthOverride(OverrideValueButtonColumnWidth)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.VAlign(VAlign_Top)
				.Padding(12.0)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.IsEnabled_Lambda([this]() -> bool
					{
							return InputData->VehicleAssetPaths[0] != "" &&
								InputData->VehicleAssetPaths[1] != "" &&
								InputData->VehicleAssetPaths[0] != InputData->VehicleAssetPaths[1];
					})
					.ContentPadding(FMargin(4.0f, 4.0f))
					.OnClicked_Raw(this, &SMainWindow::OnCompareButtonClicked)
					[
						SNew(STextBlock)
						.Text(CompareButtonLabel)
						.Margin(2.0f)
					]
				]
			]
		]

		+ SHorizontalBox::Slot()
		[
			SNew(SSpacer)
		]

	];

	// list of results
	VerticalBox->AddSlot()
	[
		SNew(SScrollBox)
		+SScrollBox::Slot()
		.Padding(10, 5)
		[
			SAssignNew(ListViewWidget,SListView< TSharedRef< FDifference >>)
			.ItemHeight(24)
			.ListItemsSource(&Results)
			.SelectionMode(ESelectionMode::None)
			.ListViewStyle(FAppStyle::Get(), "SimpleListView")
			.OnGenerateRow(this, &SMainWindow::OnGenerateRow)
			.HeaderRow(

				SNew(SHeaderRow)
				+ SHeaderRow::Column("Property")
				.FillWidth(0.6)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Property")))
				]
				+ SHeaderRow::Column("Value")
				.FillWidth(0.4)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Value")))
				]
			)
		]
	];
}


FReply SMainWindow::OnCompareButtonClicked()
{
	UVehicleCompareImpl* Impl = NewObject<UVehicleCompareImpl>();

	Impl->CompareVehicleBlueprints(InputData->VehicleAssetPaths[0], InputData->VehicleAssetPaths[1]);

	Results = Impl->GetResults();

	if (ListViewWidget.IsValid())
	{
		ListViewWidget->RequestListRefresh();
	}

	return FReply::Handled();
}

TSharedRef<ITableRow> SMainWindow::OnGenerateRow(TSharedRef<FDifference> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{

	return SNew(SDifferenceTile, OwnerTable)
		.InItem(InItem);
}

