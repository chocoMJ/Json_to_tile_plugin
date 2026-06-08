#include "JsonTOTileGameJsonReader.h"

#include "Dom/JsonObject.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

bool FJsonTOTileGameJsonReader::CollectTileTypesFromJsonFile(
	const FString& JsonPath,
	TArray<FName>& OutTileTypes,
	FString& OutStatusMessage
)
{
	OutTileTypes.Reset();
	OutStatusMessage.Reset();

	TSharedPtr<FJsonObject> JsonObject;

	if (!LoadJsonObjectFromFile(JsonPath, JsonObject, OutStatusMessage))
	{
		return false;
	}

	const TArray<TSharedPtr<FJsonValue>>* TilesArray = nullptr;

	if (!JsonObject->TryGetArrayField(TEXT("tiles"), TilesArray))
	{
		UE_LOG(LogTemp, Error, TEXT("No tiles array found in JSON: %s"), *JsonPath);
		OutStatusMessage = TEXT("Selected JSON has no tiles array.");
		return false;
	}

	TSet<FName> UniqueTileTypes;

	for (const TSharedPtr<FJsonValue>& TileValue : *TilesArray)
	{
		const TSharedPtr<FJsonObject> TileObject = TileValue->AsObject();

		if (!TileObject.IsValid())
		{
			continue;
		}

		FString TypeString;

		if (!TileObject->TryGetStringField(TEXT("type"), TypeString) || TypeString.IsEmpty())
		{
			continue;
		}

		UniqueTileTypes.Add(FName(*TypeString));
	}

	OutTileTypes = UniqueTileTypes.Array();
	OutTileTypes.Sort([](const FName& Left, const FName& Right)
	{
		return Left.ToString() < Right.ToString();
	});

	return true;
}

bool FJsonTOTileGameJsonReader::CollectObjectTypesFromJsonFile(
	const FString& JsonPath,
	TArray<FName>& OutObjectTypes,
	FString& OutStatusMessage
)
{
	OutObjectTypes.Reset();
	OutStatusMessage.Reset();

	TSharedPtr<FJsonObject> JsonObject;

	if (!LoadJsonObjectFromFile(JsonPath, JsonObject, OutStatusMessage))
	{
		return false;
	}

	const TArray<TSharedPtr<FJsonValue>>* ObjectsArray = nullptr;

	if (!JsonObject->TryGetArrayField(TEXT("objects"), ObjectsArray))
	{
		UE_LOG(LogTemp, Error, TEXT("No objects array found in JSON: %s"), *JsonPath);
		OutStatusMessage = TEXT("Selected JSON has no objects array.");
		return false;
	}

	TSet<FName> UniqueObjectTypes;

	for (const TSharedPtr<FJsonValue>& ObjectValue : *ObjectsArray)
	{
		const TSharedPtr<FJsonObject> ObjectData = ObjectValue->AsObject();

		if (!ObjectData.IsValid())
		{
			continue;
		}

		FString TypeString;

		if (!ObjectData->TryGetStringField(TEXT("type"), TypeString) || TypeString.IsEmpty())
		{
			continue;
		}

		UniqueObjectTypes.Add(FName(*TypeString));
	}

	OutObjectTypes = UniqueObjectTypes.Array();
	OutObjectTypes.Sort([](const FName& Left, const FName& Right)
	{
		return Left.ToString() < Right.ToString();
	});

	return true;
}

bool FJsonTOTileGameJsonReader::LoadJsonObjectFromFile(
	const FString& JsonPath,
	TSharedPtr<FJsonObject>& OutJsonObject,
	FString& OutStatusMessage
)
{
	OutJsonObject.Reset();

	if (JsonPath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No JSON file selected."));
		OutStatusMessage = TEXT("Select a JSON file first.");
		return false;
	}

	FString JsonText;

	if (!FFileHelper::LoadFileToString(JsonText, *JsonPath))
	{
		UE_LOG(LogTemp, Error, TEXT("JSON file load failed: %s"), *JsonPath);
		OutStatusMessage = TEXT("Failed to load selected JSON file.");
		return false;
	}

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);

	if (!FJsonSerializer::Deserialize(Reader, OutJsonObject) || !OutJsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("JSON parse failed: %s"), *JsonPath);
		OutStatusMessage = TEXT("Failed to parse selected JSON file.");
		return false;
	}

	return true;
}
