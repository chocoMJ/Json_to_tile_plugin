#include "JsonTOTileGameAssetFactory.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "HAL/FileManager.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "ObjectMappingDataAsset_plugin.h"
#include "TileMappingDataAsset_plugin.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

UTileMappingDataAsset_plugin* FJsonTOTileGameAssetFactory::CreateTileMappingDataAsset(
	const TArray<FName>& TileTypes,
	FString& OutStatusMessage
)
{
	OutStatusMessage.Reset();

	const FString BasePackageName = TEXT("/Game/JsonTOTileGame/DA_TileMapping");

	FString PackageName;
	FString AssetName;
	FAssetToolsModule::GetModule().Get().CreateUniqueAssetName(BasePackageName, TEXT(""), PackageName, AssetName);

	UPackage* Package = CreatePackage(*PackageName);

	if (!Package)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create package: %s"), *PackageName);
		OutStatusMessage = TEXT("Failed to create Mapping DA package.");
		return nullptr;
	}

	UTileMappingDataAsset_plugin* NewMappingAsset = NewObject<UTileMappingDataAsset_plugin>(
		Package,
		UTileMappingDataAsset_plugin::StaticClass(),
		*AssetName,
		RF_Public | RF_Standalone
	);

	if (!NewMappingAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create Tile Mapping DataAsset."));
		OutStatusMessage = TEXT("Failed to create Mapping DA asset.");
		return nullptr;
	}

	for (const FName& TileType : TileTypes)
	{
		FTileMappingInfo_plugin MappingInfo;
		MappingInfo.TileType = TileType;
		NewMappingAsset->TileMappings.Add(MappingInfo);
	}

	if (!SaveCreatedAsset(
		Package,
		NewMappingAsset,
		PackageName,
		TEXT("Failed to save Tile Mapping DataAsset"),
		TEXT("Failed to save Mapping DA. Check the Output Log."),
		OutStatusMessage
	))
	{
		return nullptr;
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Created Tile Mapping DataAsset: %s with %d tile types."),
		*NewMappingAsset->GetPathName(),
		TileTypes.Num()
	);

	OutStatusMessage = FString::Printf(
		TEXT("Created %s with %d tile types."),
		*NewMappingAsset->GetName(),
		TileTypes.Num()
	);

	return NewMappingAsset;
}

UObjectMappingDataAsset_plugin* FJsonTOTileGameAssetFactory::CreateObjectMappingDataAsset(
	const TArray<FName>& ObjectTypes,
	FString& OutStatusMessage
)
{
	OutStatusMessage.Reset();

	const FString BasePackageName = TEXT("/Game/JsonTOTileGame/DA_ObjectMapping");

	FString PackageName;
	FString AssetName;
	FAssetToolsModule::GetModule().Get().CreateUniqueAssetName(BasePackageName, TEXT(""), PackageName, AssetName);

	UPackage* Package = CreatePackage(*PackageName);

	if (!Package)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create package: %s"), *PackageName);
		OutStatusMessage = TEXT("Failed to create Object Mapping DA package.");
		return nullptr;
	}

	UObjectMappingDataAsset_plugin* NewMappingAsset = NewObject<UObjectMappingDataAsset_plugin>(
		Package,
		UObjectMappingDataAsset_plugin::StaticClass(),
		*AssetName,
		RF_Public | RF_Standalone
	);

	if (!NewMappingAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create Object Mapping DataAsset."));
		OutStatusMessage = TEXT("Failed to create Object Mapping DA asset.");
		return nullptr;
	}

	for (const FName& ObjectType : ObjectTypes)
	{
		FObjectMappingInfo_plugin MappingInfo;
		MappingInfo.ObjectType = ObjectType;
		NewMappingAsset->ObjectMappings.Add(MappingInfo);
	}

	if (!SaveCreatedAsset(
		Package,
		NewMappingAsset,
		PackageName,
		TEXT("Failed to save Object Mapping DataAsset"),
		TEXT("Failed to save Object Mapping DA. Check the Output Log."),
		OutStatusMessage
	))
	{
		return nullptr;
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Created Object Mapping DataAsset: %s with %d object types."),
		*NewMappingAsset->GetPathName(),
		ObjectTypes.Num()
	);

	OutStatusMessage = FString::Printf(
		TEXT("Created %s with %d object types."),
		*NewMappingAsset->GetName(),
		ObjectTypes.Num()
	);

	return NewMappingAsset;
}

bool FJsonTOTileGameAssetFactory::SaveCreatedAsset(
	UPackage* Package,
	UObject* Asset,
	const FString& PackageName,
	const FString& FailureLogPrefix,
	const FString& FailureStatusMessage,
	FString& OutStatusMessage
)
{
	FAssetRegistryModule::AssetCreated(Asset);
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

	if (!UPackage::SavePackage(Package, Asset, *PackageFileName, SaveArgs))
	{
		UE_LOG(LogTemp, Error, TEXT("%s: %s"), *FailureLogPrefix, *PackageName);
		OutStatusMessage = FailureStatusMessage;
		return false;
	}

	return true;
}
