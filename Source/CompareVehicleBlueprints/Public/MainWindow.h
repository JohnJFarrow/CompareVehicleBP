// Copyright John Farrow (c) 2023. All Rights Reserved.

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Difference.h"

class FInputData;

// main window fore settingh inputs, viewing outputs

class SMainWindow : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SMainWindow)
	{
	}

	SLATE_END_ARGS()

	virtual ~SMainWindow();

	/** Widget constructor */
	void Construct(const FArguments& Args, TSharedPtr < FInputData >& );

	TSharedRef<ITableRow> OnGenerateRow(TSharedRef<FDifference> Item, const TSharedRef<STableViewBase>& OwnerTable);


private:

	FReply OnCompareButtonClicked();

private:
	// input data
	UPROPERTY()
	TSharedPtr < FInputData > InputData;

	// hold results for display in UI
	UPROPERTY()
	TArray< TSharedRef< class FDifference > > Results;

	// results box in UI
	TSharedPtr< SVerticalBox > VerticalBox;

	// the list view
	TSharedPtr< SListView< TSharedRef<FDifference> > > ListViewWidget;

};

