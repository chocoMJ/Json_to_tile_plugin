#pragma once

#include "CoreMinimal.h"

class FJsonObject;

class FJsonTOTileGameJsonReader
{
public:
	static bool CollectTileTypesFromJsonFile(const FString& JsonPath, TArray<FName>& OutTileTypes, FString& OutStatusMessage);
	static bool CollectObjectTypesFromJsonFile(const FString& JsonPath, TArray<FName>& OutObjectTypes, FString& OutStatusMessage);
	static bool LoadJsonObjectFromFile(const FString& JsonPath, TSharedPtr<FJsonObject>& OutJsonObject, FString& OutStatusMessage);
};
