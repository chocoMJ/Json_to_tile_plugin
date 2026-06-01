// Copyright Epic Games, Inc. All Rights Reserved.

#include "JsonTOTileGame.h"
#include "JsonTOTileGameStyle.h"
#include "JsonTOTileGameCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "SSlateQuickstartWindow.h"

static const FName JsonTOTileGameTabName("JsonTOTileGame");

#define LOCTEXT_NAMESPACE "FJsonTOTileGameModule"

void FJsonTOTileGameModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FJsonTOTileGameStyle::Initialize();
	FJsonTOTileGameStyle::ReloadTextures();

	FJsonTOTileGameCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FJsonTOTileGameCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FJsonTOTileGameModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FJsonTOTileGameModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(JsonTOTileGameTabName, FOnSpawnTab::CreateRaw(this, &FJsonTOTileGameModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FJsonTOTileGameTabTitle", "JsonTOTileGame"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FJsonTOTileGameModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FJsonTOTileGameStyle::Shutdown();

	FJsonTOTileGameCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(JsonTOTileGameTabName);
}

TSharedRef<SDockTab> FJsonTOTileGameModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SSlateQuickstartWindow)
		];
}

void FJsonTOTileGameModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(JsonTOTileGameTabName);
}

void FJsonTOTileGameModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FJsonTOTileGameCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FJsonTOTileGameCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FJsonTOTileGameModule, JsonTOTileGame)