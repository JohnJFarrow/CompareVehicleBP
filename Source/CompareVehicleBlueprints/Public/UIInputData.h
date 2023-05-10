// Copyright John Farrow (c) 2023. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

class FInputData 
{
	// controls the execution of vehicle comparison
public:
	FInputData()
	{
		VehicleAssetPaths.AddDefaulted(2);

		VehicleAssetPaths[0] = "/Game/Experimental/Porsche_911_GT3/BP_Car.BP_Car";
		VehicleAssetPaths[1] = "/Game/VehicleTemplate/Blueprints/SportsCar/SportsCar_Pawn.SportsCar_Pawn";
	}

public:
	TArray<FString> VehicleAssetPaths;
};