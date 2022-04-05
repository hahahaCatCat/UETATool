#pragma once

#include "CoreMinimal.h"
#include "SlateJsonStrut.generated.h"

USTRUCT()
struct FSlateJsonStrut
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FString Name;

	UPROPERTY()
	FString Text;

	UPROPERTY()
	FString SlateType;

	UPROPERTY()
	FString ClickEvent;

	UPROPERTY()
	FString FilePath;

	UPROPERTY()
	FString PythonCMD;

	UPROPERTY()
	FString OnClickEvent;

	UPROPERTY()
	FVector2D Size;

}; 
