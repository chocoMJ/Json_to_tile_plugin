#pragma once

#include "CoreMinimal.h"

class UObjectMappingDataAsset_plugin;
class UTileMappingDataAsset_plugin;

class FJsonTOTileGameMapGenerator
{
public:
	static bool GenerateTileMapFromJsonFile(
		const FString& JsonPath,
		UTileMappingDataAsset_plugin* TileMappingData,
		UObjectMappingDataAsset_plugin* ObjectMappingData,
		FString& OutStatusMessage
	);
};
