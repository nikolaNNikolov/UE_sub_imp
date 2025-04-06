// Copyright Epic Games, Inc. All Rights Reserved.

#include "SubImp.h"

#include "SSubImpWindow.h"
#include "SubImpStyle.h"
#include "SubImpCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "Interfaces/IMainFrameModule.h"

static const FName SubImpTabName("SubImp");

#define LOCTEXT_NAMESPACE "FSubImpModule"

void FSubImpModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FSubImpStyle::Initialize();
	FSubImpStyle::ReloadTextures();

	FSubImpCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FSubImpCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FSubImpModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FSubImpModule::RegisterMenus));
}

void FSubImpModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	CloseSubImpWindow();

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FSubImpStyle::Shutdown();

	FSubImpCommands::Unregister();
}

void FSubImpModule::PluginButtonClicked()
{
	OpenSubImpWindow(FModuleManager::GetModuleChecked<IMainFrameModule>(TEXT("MainFrame")).GetParentWindow());
}

void FSubImpModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FSubImpCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FSubImpCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSubImpModule, SubImp)