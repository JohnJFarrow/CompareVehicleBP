// Copyright John Farrow (c) 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "CompareVehicleBlueprintsStyle.h"

class FCompareVehicleBlueprintsCommands : public TCommands<FCompareVehicleBlueprintsCommands>
{
public:

	FCompareVehicleBlueprintsCommands()
		: TCommands<FCompareVehicleBlueprintsCommands>(TEXT("CompareVehicleBlueprints"), NSLOCTEXT("Contexts", "CompareVehicleBlueprints", "CompareVehicleBlueprints Plugin"), NAME_None, FCompareVehicleBlueprintsStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};