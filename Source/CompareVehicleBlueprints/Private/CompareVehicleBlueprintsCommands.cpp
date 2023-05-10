// Copyright John Farrow (c) 2023. All Rights Reserved.


#include "CompareVehicleBlueprintsCommands.h"

#define LOCTEXT_NAMESPACE "FCompareVehicleBlueprintsModule"

void FCompareVehicleBlueprintsCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Compare Vehicle Blueprints", "Bring up Compare Vehicle Blueprints window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
