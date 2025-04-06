// Copyright Epic Games, Inc. All Rights Reserved.

#include "SubImpCommands.h"

#define LOCTEXT_NAMESPACE "FSubImpModule"

void FSubImpCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "sub-imp", "Open the sub-imp Subtitle Importer window.", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
