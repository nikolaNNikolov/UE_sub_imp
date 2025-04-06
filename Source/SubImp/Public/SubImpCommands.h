// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "SubImpStyle.h"

class FSubImpCommands : public TCommands<FSubImpCommands>
{
public:

	FSubImpCommands()
		: TCommands<FSubImpCommands>(TEXT("sub-imp"), NSLOCTEXT("Contexts", "sub-imp", "sub-imp Subtitle Importer"), NAME_None, FSubImpStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
