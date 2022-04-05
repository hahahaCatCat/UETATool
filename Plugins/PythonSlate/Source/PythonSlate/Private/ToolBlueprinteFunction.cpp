#include "ToolBluprintFunction.h"
#include "PythonSlate.h"

void UPythonSlateTool::InitSlateTool(const FString& SlateJson)
{
	FPythonSlateModule::Get()->InitSlate(SlateJson);
}