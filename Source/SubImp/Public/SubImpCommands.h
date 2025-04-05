// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "SubImpStyle.h"

class FSubImpCommands : public TCommands<FSubImpCommands>
{
public:

	FSubImpCommands()
		: TCommands<FSubImpCommands>(TEXT("SubImp"), NSLOCTEXT("Contexts", "SubImp", "SubImp Plugin"), NAME_None, FSubImpStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
