#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "ToolBluprintFunction.generated.h"

UCLASS()
class  UPythonSlateTool : public UBlueprintFunctionLibrary
{
     GENERATED_BODY()
public:
        UFUNCTION(BlueprintCallable, Category = "ToolFunction")
        static void InitSlateTool(const FString& SlateJson);
};