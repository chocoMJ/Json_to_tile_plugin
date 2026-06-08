#pragma once

#include "CoreMinimal.h"

class UObjectMappingDataAsset_plugin;
class UTileMappingDataAsset_plugin;

class FJsonTOTileGameImporter
{
public:
	static bool CollectTileTypesFromJsonFile(const FString& JsonPath, TArray<FName>& OutTileTypes, FString& OutStatusMessage);
	static bool CollectObjectTypesFromJsonFile(const FString& JsonPath, TArray<FName>& OutObjectTypes, FString& OutStatusMessage);

	static UTileMappingDataAsset_plugin* CreateTileMappingDataAsset(const TArray<FName>& TileTypes, FString& OutStatusMessage);
	static UObjectMappingDataAsset_plugin* CreateObjectMappingDataAsset(const TArray<FName>& ObjectTypes, FString& OutStatusMessage);

	static bool GenerateTileMapFromJsonFile(
		const FString& JsonPath,
		UTileMappingDataAsset_plugin* TileMappingData,
		UObjectMappingDataAsset_plugin* ObjectMappingData,
		FString& OutStatusMessage
	);
};
