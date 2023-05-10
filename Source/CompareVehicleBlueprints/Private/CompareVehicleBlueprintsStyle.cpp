// Copyright John Farrow (c) 2023. All Rights Reserved.


#include "CompareVehicleBlueprintsStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"
#include "Styling/SlateTypes.h"
#include "Styling/CoreStyle.h"
#include "Styling/StarshipCoreStyle.h"
#include "Runtime\SlateCore\Public\Styling\StyleColors.h"


TSharedPtr<FSlateStyleSet> FCompareVehicleBlueprintsStyle::StyleInstance = nullptr;


void FCompareVehicleBlueprintsStyle::Initialize()
{

	// Only register once
	if (StyleInstance.IsValid())
	{
		return;
	}

	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}

	// Const icon sizes
	const FVector2D Icon24x24(24.0f, 24.0f);
	const FVector2D Icon16x16(16.0f, 16.0f);


	StyleInstance->SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

	const FStyleFonts& StyleFonts = FStyleFonts::Get();

	StyleInstance->Set("NormalFont", StyleFonts.Normal);
	StyleInstance->Set("SmallFont", StyleFonts.Small);
	StyleInstance->Set("Font.Large", StyleFonts.Large);
	StyleInstance->Set("NormalFontBold", StyleFonts.NormalBold);
	StyleInstance->Set("SmallFontBold", StyleFonts.SmallBold);
	StyleInstance->Set("Font.Large.Bold", StyleFonts.LargeBold);
	StyleInstance->Set("NormalFontItalic", StyleFonts.NormalItalic);
	StyleInstance->Set("NormalFontBoldItalic", StyleFonts.NormalBoldItalic);

	StyleInstance->Set("HeadingMedium", StyleFonts.HeadingMedium);
	StyleInstance->Set("HeadingSmall", StyleFonts.HeadingSmall);
	StyleInstance->Set("HeadingExtraSmall", StyleFonts.HeadingExtraSmall);

	// Plugins Manager
	const FTextBlockStyle NormalText = FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");
	const FTextBlockStyle ButtonText = FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("PrimaryButtonText");
	FTextBlockStyle LargeText = FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("Text.Large");

	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("CompareVehicleBlueprintsStyle"));
	StyleInstance->SetContentRoot(IPluginManager::Get().FindPlugin("CompareVehicleBlueprints")->GetBaseDir() / TEXT("Resources"));

	// macros like IMAGE_BRUSH_SVG rely on having RootToContentDir defined
#define RootToContentDir FCompareVehicleBlueprintsStyle::InResources
#define RootToCoreContentDir StyleInstance->RootToCoreContentDir

	// Peset Tile

	FTextBlockStyle GeneralText = FTextBlockStyle(LargeText)
		.SetColorAndOpacity(FStyleColors::Foreground);
	StyleInstance->Set("Difference.GeneralText", GeneralText);

	FTextBlockStyle ErrorText = FTextBlockStyle(LargeText)
		.SetColorAndOpacity(FStyleColors::Error);
	StyleInstance->Set("Difference.ErrorText", ErrorText );

	FTextBlockStyle WarningText = FTextBlockStyle(NormalText)
		.SetColorAndOpacity(FStyleColors::Warning);
	StyleInstance->Set("Difference.WarningText", WarningText);

	FTextBlockStyle InfoText = FTextBlockStyle(NormalText)
		.SetColorAndOpacity(FStyleColors::AccentBlue);
	StyleInstance->Set("Difference.InfoText", InfoText);

	FTextBlockStyle DifferenceHeading = FTextBlockStyle(NormalText)
		.SetColorAndOpacity(FStyleColors::AccentBlue);
	StyleInstance->Set("Difference.DifferenceHeading", DifferenceHeading );

	FTextBlockStyle DifferencePath = FTextBlockStyle(NormalText)
		.SetColorAndOpacity(FStyleColors::AccentBlue);
	StyleInstance->Set("Difference.Path", DifferencePath);

	FTextBlockStyle DifferenceValue = FTextBlockStyle(NormalText)
		.SetColorAndOpacity(FStyleColors::AccentBlue);
	StyleInstance->Set("Difference.Value", DifferenceValue );

}

void FCompareVehicleBlueprintsStyle::Shutdown()
{
	if (StyleInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
		ensure(StyleInstance.IsUnique());
		StyleInstance.Reset();
	}
}

FName FCompareVehicleBlueprintsStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("CompareVehicleBlueprintsStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FCompareVehicleBlueprintsStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("CompareVehicleBlueprintsStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("CompareVehicleBlueprints")->GetBaseDir() / TEXT("Resources"));

	Style->Set("CompareVehicleBlueprints.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));

	return Style;
}

void FCompareVehicleBlueprintsStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FCompareVehicleBlueprintsStyle::Get()
{
	check(StyleInstance);
	return *StyleInstance;
}


FString FCompareVehicleBlueprintsStyle::InContent(const FString& RelativePath, const TCHAR* Extension)
{
	static FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("CompareVehicleBlueprints"))->GetContentDir();
	return (ContentDir / RelativePath) + Extension;
}

FString FCompareVehicleBlueprintsStyle::InResources(const FString& RelativePath, const TCHAR* Extension)
{
	static FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("CompareVehicleBlueprints"))->GetBaseDir() / TEXT("Resources");
	return (ContentDir / RelativePath) + Extension;
}


