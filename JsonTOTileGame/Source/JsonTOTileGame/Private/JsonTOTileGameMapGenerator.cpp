#include "JsonTOTileGameMapGenerator.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Dom/JsonObject.h"
#include "Editor.h"
#include "HAL/FileManager.h"
#include "JsonTOTileGameJsonReader.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "ObjectMappingDataAsset_plugin.h"
#include "PaperTileLayer.h"
#include "PaperTileMap.h"
#include "PaperTileMapActor.h"
#include "PaperTileMapComponent.h"
#include "TileMappingDataAsset_plugin.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

bool FJsonTOTileGameMapGenerator::GenerateTileMapFromJsonFile(
	const FString& JsonPath,
	UTileMappingDataAsset_plugin* TileMappingData,
	UObjectMappingDataAsset_plugin* ObjectMappingData,
	FString& OutStatusMessage
)
{
	OutStatusMessage.Reset();

	TSharedPtr<FJsonObject> JsonObject;

	if (!FJsonTOTileGameJsonReader::LoadJsonObjectFromFile(JsonPath, JsonObject, OutStatusMessage))
	{
		return false;
	}

	if (!TileMappingData)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Tile Mapping DataAsset selected."));
		OutStatusMessage = TEXT("Select or create a Mapping DA before generating a map.");
		return false;
	}

	const TSharedPtr<FJsonObject>* MapObjectPtr = nullptr;

	if (!JsonObject->TryGetObjectField(TEXT("map"), MapObjectPtr) || !MapObjectPtr || !MapObjectPtr->IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("No map object found in JSON: %s"), *JsonPath);
		OutStatusMessage = TEXT("Selected JSON has no map object.");
		return false;
	}

	const TSharedPtr<FJsonObject> MapObject = *MapObjectPtr;
	int32 Width = 0;
	int32 Height = 0;
	int32 TileSize = 0;

	if (!MapObject->TryGetNumberField(TEXT("width"), Width) ||
		!MapObject->TryGetNumberField(TEXT("height"), Height) ||
		!MapObject->TryGetNumberField(TEXT("tileSize"), TileSize))
	{
		UE_LOG(LogTemp, Error, TEXT("Map object requires width, height, and tileSize."));
		OutStatusMessage = TEXT("Map object requires width, height, and tileSize.");
		return false;
	}

	const TArray<TSharedPtr<FJsonValue>>* TilesArray = nullptr;

	if (!JsonObject->TryGetArrayField(TEXT("tiles"), TilesArray))
	{
		UE_LOG(LogTemp, Error, TEXT("No tiles array found in JSON: %s"), *JsonPath);
		OutStatusMessage = TEXT("Selected JSON has no tiles array.");
		return false;
	}

	const FString JsonBaseName = FPaths::GetBaseFilename(JsonPath);
	const FString BasePackageName = TEXT("/Game/JsonTOTileGame/GeneratedMaps/") + JsonBaseName + TEXT("_TileMap");

	FString PackageName;
	FString AssetName;
	FAssetToolsModule::GetModule().Get().CreateUniqueAssetName(BasePackageName, TEXT(""), PackageName, AssetName);

	UPackage* Package = CreatePackage(*PackageName);

	if (!Package)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create TileMap package: %s"), *PackageName);
		OutStatusMessage = TEXT("Failed to create TileMap package.");
		return false;
	}

	UPaperTileMap* NewTileMap = NewObject<UPaperTileMap>(
		Package,
		UPaperTileMap::StaticClass(),
		*AssetName,
		RF_Public | RF_Standalone
	);

	if (!NewTileMap)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create TileMap asset."));
		OutStatusMessage = TEXT("Failed to create TileMap asset.");
		return false;
	}

	NewTileMap->MapWidth = Width;
	NewTileMap->MapHeight = Height;
	NewTileMap->TileWidth = TileSize;
	NewTileMap->TileHeight = TileSize;
	NewTileMap->PixelsPerUnrealUnit = 1.0f;

	UPaperTileLayer* NewLayer = NewObject<UPaperTileLayer>(
		NewTileMap,
		UPaperTileLayer::StaticClass(),
		TEXT("TileLayer")
	);

	if (!NewLayer)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create TileLayer."));
		OutStatusMessage = TEXT("Failed to create TileLayer.");
		return false;
	}

	NewLayer->LayerName = FText::FromString(TEXT("GroundLayer"));
	NewLayer->ResizeMap(Width, Height);
	NewTileMap->TileLayers.Add(NewLayer);

	int32 PlacedTileCount = 0;
	int32 MissingMappingCount = 0;

	for (const TSharedPtr<FJsonValue>& TileValue : *TilesArray)
	{
		const TSharedPtr<FJsonObject> TileObject = TileValue->AsObject();

		if (!TileObject.IsValid())
		{
			continue;
		}

		int32 JsonX = 0;
		int32 JsonY = 0;
		FString TypeString;

		if (!TileObject->TryGetNumberField(TEXT("x"), JsonX) ||
			!TileObject->TryGetNumberField(TEXT("y"), JsonY) ||
			!TileObject->TryGetStringField(TEXT("type"), TypeString))
		{
			continue;
		}

		FTileMappingInfo_plugin MappingInfo;

		if (!TileMappingData->FindTileInfo(FName(*TypeString), MappingInfo) || !MappingInfo.TileSet)
		{
			++MissingMappingCount;
			UE_LOG(LogTemp, Warning, TEXT("Missing tile mapping or TileSet: %s"), *TypeString);
			continue;
		}

		FPaperTileInfo TileInfo;
		TileInfo.TileSet = MappingInfo.TileSet;
		TileInfo.PackedTileIndex = MappingInfo.TileIndex;

		const int32 TileX = JsonX;
		const int32 TileY = Height - 1 - JsonY;

		if (TileX < 0 || TileX >= Width || TileY < 0 || TileY >= Height)
		{
			UE_LOG(LogTemp, Warning, TEXT("Tile coordinate out of bounds: (%d, %d)"), JsonX, JsonY);
			continue;
		}

		NewLayer->SetCell(TileX, TileY, TileInfo);
		++PlacedTileCount;
	}

	if (PlacedTileCount == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No tiles were placed. Check Mapping DA TileSet assignments."));
		OutStatusMessage = TEXT("No tiles placed. Check Mapping DA TileSet assignments.");
		return false;
	}

	FAssetRegistryModule::AssetCreated(NewTileMap);
	Package->MarkPackageDirty();

	const FString PackageFileName = FPackageName::LongPackageNameToFilename(
		PackageName,
		FPackageName::GetAssetPackageExtension()
	);

	const FString PackageDirectory = FPaths::GetPath(PackageFileName);
	IFileManager::Get().MakeDirectory(*PackageDirectory, true);

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	SaveArgs.SaveFlags = SAVE_NoError;

	if (!UPackage::SavePackage(Package, NewTileMap, *PackageFileName, SaveArgs))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save TileMap asset: %s"), *PackageName);
		OutStatusMessage = TEXT("Failed to save TileMap asset. Check the Output Log.");
		return false;
	}

	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;

	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("Editor world is null. TileMap asset was created but not placed."));
		OutStatusMessage = TEXT("TileMap asset created, but editor world was not available for placement.");
		return true;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Name = NAME_None;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.ObjectFlags = RF_Transactional;

	APaperTileMapActor* TileMapActor = World->SpawnActor<APaperTileMapActor>(
		APaperTileMapActor::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (!TileMapActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to place TileMapActor."));
		OutStatusMessage = TEXT("TileMap asset created, but actor placement failed.");
		return true;
	}

	UPaperTileMapComponent* TileMapComponent = TileMapActor->GetRenderComponent();

	if (!TileMapComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("TileMapComponent is null."));
		OutStatusMessage = TEXT("TileMap asset created, but actor component was missing.");
		return true;
	}

	TileMapComponent->SetTileMap(NewTileMap);
	TileMapActor->SetActorLabel(AssetName);
	TileMapActor->Modify();
	TileMapComponent->Modify();

	int32 SpawnedObjectCount = 0;
	int32 MissingObjectMappingCount = 0;
	const TArray<TSharedPtr<FJsonValue>>* ObjectsArray = nullptr;

	if (JsonObject->TryGetArrayField(TEXT("objects"), ObjectsArray) && ObjectsArray)
	{
		if (!ObjectMappingData)
		{
			MissingObjectMappingCount = ObjectsArray->Num();
			UE_LOG(LogTemp, Warning, TEXT("Objects found, but no Object Mapping DataAsset selected."));
		}
		else
		{
			for (const TSharedPtr<FJsonValue>& ObjectValue : *ObjectsArray)
			{
				const TSharedPtr<FJsonObject> ObjectData = ObjectValue->AsObject();

				if (!ObjectData.IsValid())
				{
					continue;
				}

				int32 JsonX = 0;
				int32 JsonY = 0;
				FString TypeString;

				if (!ObjectData->TryGetNumberField(TEXT("x"), JsonX) ||
					!ObjectData->TryGetNumberField(TEXT("y"), JsonY) ||
					!ObjectData->TryGetStringField(TEXT("type"), TypeString))
				{
					continue;
				}

				FObjectMappingInfo_plugin MappingInfo;

				if (!ObjectMappingData->FindObjectInfo(FName(*TypeString), MappingInfo) || !MappingInfo.ActorClass)
				{
					++MissingObjectMappingCount;
					UE_LOG(LogTemp, Warning, TEXT("Missing object mapping or ActorClass: %s"), *TypeString);
					continue;
				}

				const float WorldX = JsonX * TileSize + TileSize * 0.5f;
				const float WorldZ = -(Height - JsonY - 0.5f) * TileSize;
				const FVector SpawnLocation = TileMapActor->GetActorLocation() + FVector(WorldX, 0.0f, WorldZ);

				FActorSpawnParameters ObjectSpawnParams;
				ObjectSpawnParams.Name = NAME_None;
				ObjectSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				ObjectSpawnParams.ObjectFlags = RF_Transactional;

				AActor* SpawnedActor = World->SpawnActor<AActor>(
					MappingInfo.ActorClass,
					SpawnLocation,
					FRotator::ZeroRotator,
					ObjectSpawnParams
				);

				if (!SpawnedActor)
				{
					++MissingObjectMappingCount;
					UE_LOG(LogTemp, Warning, TEXT("Failed to spawn object: %s"), *TypeString);
					continue;
				}

				SpawnedActor->SetActorLabel(TypeString);
				SpawnedActor->Modify();
				++SpawnedObjectCount;
			}
		}
	}

	World->Modify();
	World->MarkPackageDirty();

	if (GEditor)
	{
		TArray<UObject*> ObjectsToSync;
		ObjectsToSync.Add(NewTileMap);
		GEditor->SyncBrowserToObjects(ObjectsToSync);
		GEditor->RedrawAllViewports();
	}

	OutStatusMessage = FString::Printf(
		TEXT("Generated %s. Tiles: %d. Objects: %d. Missing mappings: %d/%d."),
		*AssetName,
		PlacedTileCount,
		SpawnedObjectCount,
		MissingMappingCount,
		MissingObjectMappingCount
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Generated TileMap: %s. Placed tiles: %d. Spawned objects: %d. Missing tile mappings: %d. Missing object mappings: %d."),
		*NewTileMap->GetPathName(),
		PlacedTileCount,
		SpawnedObjectCount,
		MissingMappingCount,
		MissingObjectMappingCount
	);

	return true;
}
