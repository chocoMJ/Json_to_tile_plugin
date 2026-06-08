#include "JsonTOTileGameImporter.h"

#include "JsonTOTileGameAssetFactory.h"
#include "JsonTOTileGameJsonReader.h"
#include "JsonTOTileGameMapGenerator.h"

bool FJsonTOTileGameImporter::CollectTileTypesFromJsonFile(
	const FString& JsonPath,
	TArray<FName>& OutTileTypes,
	FString& OutStatusMessage
)
{
	return FJsonTOTileGameJsonReader::CollectTileTypesFromJsonFile(JsonPath, OutTileTypes, OutStatusMessage);
}

bool FJsonTOTileGameImporter::CollectObjectTypesFromJsonFile(
	const FString& JsonPath,
	TArray<FName>& OutObjectTypes,
	FString& OutStatusMessage
)
{
	return FJsonTOTileGameJsonReader::CollectObjectTypesFromJsonFile(JsonPath, OutObjectTypes, OutStatusMessage);
}

UTileMappingDataAsset_plugin* FJsonTOTileGameImporter::CreateTileMappingDataAsset(
	const TArray<FName>& TileTypes,
	FString& OutStatusMessage
)
{
	return FJsonTOTileGameAssetFactory::CreateTileMappingDataAsset(TileTypes, OutStatusMessage);
}

UObjectMappingDataAsset_plugin* FJsonTOTileGameImporter::CreateObjectMappingDataAsset(
	const TArray<FName>& ObjectTypes,
	FString& OutStatusMessage
)
{
	return FJsonTOTileGameAssetFactory::CreateObjectMappingDataAsset(ObjectTypes, OutStatusMessage);
}

bool FJsonTOTileGameImporter::GenerateTileMapFromJsonFile(
	const FString& JsonPath,
	UTileMappingDataAsset_plugin* TileMappingData,
	UObjectMappingDataAsset_plugin* ObjectMappingData,
	FString& OutStatusMessage
)
{
	return FJsonTOTileGameMapGenerator::GenerateTileMapFromJsonFile(
		JsonPath,
		TileMappingData,
		ObjectMappingData,
		OutStatusMessage
	);
}
