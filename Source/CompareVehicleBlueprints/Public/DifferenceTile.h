// Copyright John Farrow (c) 2023. All Rights Reserved.

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Difference.h"

#pragma once

class SGridPanel;
class SButton;
class SCheckBox;


class SDifferenceTile : public SMultiColumnTableRow<TSharedPtr< FDifference > >
{
	SLATE_BEGIN_ARGS(SDifferenceTile) 
		: _InItem(nullptr)
	{

	}

	SLATE_ARGUMENT(TSharedPtr<FDifference>, InItem)
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);

	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;

private:
	TSharedPtr<FDifference> Item;
};

