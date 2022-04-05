// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "PythonSlateStyle.h"

class FPythonSlateCommands : public TCommands<FPythonSlateCommands>
{
public:

	FPythonSlateCommands()
		: TCommands<FPythonSlateCommands>(TEXT("PythonSlate"), NSLOCTEXT("Contexts", "PythonSlate", "PythonSlate Plugin"), NAME_None, FPythonSlateStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
