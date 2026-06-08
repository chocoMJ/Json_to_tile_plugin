#pragma once

#include "CoreMinimal.h"

class UObjectMappingDataAsset_plugin;
class UTileMappingDataAsset_plugin;

class FJsonTOTileGameAssetFactory
{
public:
	static UTileMappingDataAsset_plugin* CreateTileMappingDataAsset(const TArray<FName>& TileTypes, FString& OutStatusMessage);
	static UObjectMappingDataAsset_plugin* CreateObjectMappingDataAsset(const TArray<FName>& ObjectTypes, FString& OutStatusMessage);

private:
	static bool SaveCreatedAsset(
		UPackage* Package,
		UObject* Asset,
		const FString& PackageName,
		const FString& FailureLogPrefix,
		const FString& FailureStatusMessage,
		FString& OutStatusMessage
	);
};
