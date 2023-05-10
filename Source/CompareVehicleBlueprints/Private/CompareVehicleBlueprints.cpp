// Copyright John Farrow (c) 2023. All Rights Reserved.

#include "CompareVehicleBlueprints.h"
#include "CompareVehicleBlueprintsStyle.h"
#include "CompareVehicleBlueprintsCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "UIInputData.h"
#include "MainWindow.h"

static const FName CompareVehicleBlueprintsTabName("Compare Vehicle Blueprints");

#define LOCTEXT_NAMESPACE "FCompareVehicleBlueprintsModule"

void FCompareVehicleBlueprintsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FCompareVehicleBlueprintsStyle::Initialize();
	FCompareVehicleBlueprintsStyle::ReloadTextures();

	FCompareVehicleBlueprintsCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FCompareVehicleBlueprintsCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FCompareVehicleBlueprintsModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FCompareVehicleBlueprintsModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(CompareVehicleBlueprintsTabName, FOnSpawnTab::CreateRaw(this, &FCompareVehicleBlueprintsModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FCompareVehicleBlueprintsTabTitle", "Compare Vehicle Blueprints"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	InputData = MakeShared< FInputData >();
}

void FCompareVehicleBlueprintsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FCompareVehicleBlueprintsStyle::Shutdown();

	FCompareVehicleBlueprintsCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(CompareVehicleBlueprintsTabName);
}

TSharedRef<SDockTab> FCompareVehicleBlueprintsModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	const float PaddingAmount = 10.0f;

	TSharedRef<SDockTab> ResultTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab);

	TSharedRef<SVerticalBox> MainContent = SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(PaddingAmount)
		.AutoHeight()
		;

	TSharedRef<SMainWindow> TabContentWidget = SAssignNew(MainWindowWidget, SMainWindow, InputData);

	ResultTab->SetContent(TabContentWidget);

	return ResultTab;
}

void FCompareVehicleBlueprintsModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(CompareVehicleBlueprintsTabName);
}

void FCompareVehicleBlueprintsModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FCompareVehicleBlueprintsCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FCompareVehicleBlueprintsCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCompareVehicleBlueprintsModule, CompareVehicleBlueprints)