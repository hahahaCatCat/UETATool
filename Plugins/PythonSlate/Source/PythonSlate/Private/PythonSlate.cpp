// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PythonSlate.h"
#include "PythonSlateStyle.h"
#include "PythonSlateCommands.h"
#include "Misc/MessageDialog.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Input/SButton.h"
#include "IPythonScriptPlugin.h"

#include "LevelEditor.h"
#include "Misc/FileHelper.h"
#include "Json.h"

static const FName PythonSlateTabName("PythonSlate");

#define LOCTEXT_NAMESPACE "FPythonSlateModule"

void FPythonSlateModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FPythonSlateStyle::Initialize();
	FPythonSlateStyle::ReloadTextures();

	FPythonSlateCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FPythonSlateCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FPythonSlateModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FPythonSlateModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FPythonSlateModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
}

void FPythonSlateModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FPythonSlateStyle::Shutdown();

	FPythonSlateCommands::Unregister();
}

void FPythonSlateModule::PluginButtonClicked()
{
	if (SlateWindow.IsValid())
	{
		return;
	}

	SlateWindow = nullptr;

	// 从c++调用python 意味着 面这段话必须存在
	IPythonScriptPlugin::Get()->ExecPythonCommand(TEXT("from SlateWindow import * "));
	IPythonScriptPlugin::Get()->ExecPythonCommand(TEXT("window = SlateWindow()"));	
}

void FPythonSlateModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FPythonSlateCommands::Get().PluginAction);
}

void FPythonSlateModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FPythonSlateCommands::Get().PluginAction);
}

void FPythonSlateModule::InitSlate(const FString& SlateJson)
{
	FString Result;
	FFileHelper::LoadFileToString(Result, *SlateJson);

	if (!Result.IsEmpty())
	{
		TSharedRef < TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Result);
		TSharedPtr<FJsonObject> ResultJsonObject;
		FJsonSerializer::Deserialize(Reader, ResultJsonObject);

		if (ResultJsonObject.IsValid())
		{
			LayoutJson = MakeShared<FSlateJsonStrut>();

			BuildLayoutJson(ResultJsonObject,nullptr);
		}
	}
}

void FPythonSlateModule::BuildLayoutJson(const TSharedPtr<FJsonObject> JsonObject, TSharedPtr<SWidget> ParentWidget)
{
	JsonObject->TryGetStringField(TEXT("Name"), LayoutJson->Name);

	FString SizeString;
	JsonObject->TryGetStringField(TEXT("Size"), SizeString);
	LayoutJson->Size.InitFromString(SizeString);

	JsonObject->TryGetStringField(TEXT("FilePath"), LayoutJson->FilePath);
	JsonObject->TryGetStringField(TEXT("Type"), LayoutJson->SlateType);

	JsonObject->TryGetStringField(TEXT("PythonCMD"), LayoutJson->PythonCMD);
	JsonObject->TryGetStringField(TEXT("OnClick"), LayoutJson->OnClickEvent);
	JsonObject->TryGetStringField(TEXT("Text"), LayoutJson->Text);
	

	if (LayoutJson->SlateType.Equals(TEXT("SWindow")))
	{
		if (SlateWindow.IsValid())
		{
			FChildren* Children = SlateWindow->GetAllChildren();
			
			for (int i = 0; i < Children->Num(); i++)
			{
				TSharedRef<SWidget> Child = Children->GetChildAt(i);
					
				Child->ConditionallyDetatchParentWidget(Child->GetParentWidget().Get());
			}
		}
		else
		{
			SlateWindow = SNew(SWindow)
				.Title(FText::FromString(LayoutJson->Name))
				.ClientSize(LayoutJson->Size);

			SlateWindow->SetOnWindowClosed(FOnWindowClosed::CreateLambda([this](const TSharedRef<SWindow>& WindowArg) {
				SlateWindow.Reset();
			}));

			FSlateApplication::Get().AddWindow(SlateWindow.ToSharedRef());
		}
		ParentWidget = SlateWindow;
	}

	if (LayoutJson->SlateType.Equals(TEXT("SButton")))
	{
		TSharedPtr<SButton> ButtonPtr = SNew(SButton);
		if (!LayoutJson->OnClickEvent.IsEmpty())
		{
			ButtonPtr->SetOnClicked(FOnClicked::CreateLambda([this]() ->FReply {
				IPythonScriptPlugin::Get()->ExecPythonCommand(*LayoutJson->OnClickEvent);
				return FReply::Handled();
			}));
		}

		//ButtonPtr->SetHAlign

		AddChild(ParentWidget, ButtonPtr);
		ParentWidget = ButtonPtr;
	}
	else if (LayoutJson->SlateType.Equals(TEXT("STextBlock")))
	{
		TSharedPtr<STextBlock> TextBlock = SNew(STextBlock)
			.Text(FText::FromString(LayoutJson->Text));
		
		AddChild(ParentWidget, TextBlock);
	}

	if (JsonObject->HasField("Slots"))
	{
		const TArray<TSharedPtr<FJsonValue>>& Childs = JsonObject->GetArrayField(TEXT("Slots"));
		for (const TSharedPtr<FJsonValue>& Child : Childs)
		{
			BuildLayoutJson(Child->AsObject(), ParentWidget);
		}
	}
}

void FPythonSlateModule::AddChild(TSharedPtr<SWidget> ParentWidget, TSharedPtr<SWidget> ChildWidget)
{
	if (!ParentWidget)return;

	if(ParentWidget->GetType().ToString().Equals(TEXT("SWindow"),ESearchCase::IgnoreCase))
	{
		TSharedPtr<SWindow> Window = StaticCastSharedPtr<SWindow>(ParentWidget);
		Window->SetContent(ChildWidget.ToSharedRef());
	}
	else if (ParentWidget->GetType().ToString().Equals(TEXT("SButton"), ESearchCase::IgnoreCase))
	{
		TSharedPtr<SButton> Button = StaticCastSharedPtr<SButton>(ParentWidget);
		Button->SetContent(ChildWidget.ToSharedRef());
	}
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPythonSlateModule, PythonSlate)