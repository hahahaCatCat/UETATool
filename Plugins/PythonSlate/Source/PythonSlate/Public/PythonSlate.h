// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "SlateJsonStrut.h"

class FToolBarBuilder;
class FMenuBuilder;
class SWindow;

class FPythonSlateModule : public IModuleInterface
{
public:

	static FPythonSlateModule* Get()
	{
		static const FName ModuleName = "PythonSlate";
		return FModuleManager::GetModulePtr<FPythonSlateModule>(ModuleName);
	}

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	void PluginButtonClicked();

	void InitSlate(const FString& SlateJson);	 
private:
	void AddToolbarExtension(FToolBarBuilder& Builder);
	void AddMenuExtension(FMenuBuilder& Builder);
	
	void AddChild(TSharedPtr<SWidget> ParentWidget, TSharedPtr<SWidget> ChildWidget);

	void BuildLayoutJson(const TSharedPtr<FJsonObject> JsonObject, TSharedPtr<SWidget> ParentWidget);

private:
	TSharedPtr<class FUICommandList> PluginCommands;

	TSharedPtr<FSlateJsonStrut> LayoutJson;
	TSharedPtr<SWindow> SlateWindow;
};
