// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PythonSlateCommands.h"

#define LOCTEXT_NAMESPACE "FPythonSlateModule"

void FPythonSlateCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "PythonSlate", "Execute PythonSlate action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
