// Copyright John Farrow (c) 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class FCompareVehicleBlueprintsStyle
{
public:

	static void Initialize();

	static void Shutdown();

	/** reloads textures used by slate renderer */
	static void ReloadTextures();

	/** @return The Slate style set for the Shooter game */
	static const ISlateStyle& Get();

	static FName GetStyleSetName();

private:

	static TSharedRef< class FSlateStyleSet > Create();

	static FString InContent(const FString& RelativePath, const TCHAR* Extension);
	static FString InResources(const FString& RelativePath, const TCHAR* Extension);

private:

	static TSharedPtr< class FSlateStyleSet > StyleInstance;
};