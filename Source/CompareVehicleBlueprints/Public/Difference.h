// Copyright John Farrow (c) 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EDifferenceType : uint8
{
	Info,
	Error,
	Warning,
	Difference
};

// pass different between two vehicles 
class FDifference 
{
public:	
	TArray<FString> Paths;

	TArray<FString> ValuesAsString;

	FString Message;

	EDifferenceType Type;
};
