// Copyright Epic Games, Inc. All Rights Reserved.

#include "SubImpCommands.h"

#define LOCTEXT_NAMESPACE "FSubImpModule"

void FSubImpCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "SubImp", "Execute SubImp action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
