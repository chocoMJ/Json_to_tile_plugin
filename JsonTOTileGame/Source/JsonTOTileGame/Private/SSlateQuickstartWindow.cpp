// Fill out your copyright notice in the Description page of Project Settings.


#include "SSlateQuickstartWindow.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "DesktopPlatformModule.h"
#include "Dom/JsonObject.h"
#include "Editor.h"
#include "HAL/FileManager.h"
#include "IDesktopPlatform.h"
#include "Framework/Application/SlateApplication.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "PaperTileLayer.h"
#include "PaperTileMap.h"
#include "PaperTileMapActor.h"
#include "PaperTileMapComponent.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"

#include "ObjectMappingDataAsset_plugin.h"
#include "TileMappingDataAsset_plugin.h"
#include "PropertyCustomizationHelpers.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SSlateQuickstartWindow::Construct(const FArguments& InArgs)
{
    ChildSlot
        [
            SNew(SVerticalBox)

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10.0f)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString(TEXT("JSON Map Importer")))
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10.0f)
                [
                    SNew(SHorizontalBox)

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString(TEXT("JSON File: ")))
                        ]

                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        .VAlign(VAlign_Center)
                        .Padding(8.0f, 0.0f)
                        [
                            SAssignNew(JsonPathTextBlock, STextBlock)
                                .Text(FText::FromString(TEXT("No JSON selected")))
                        ]

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        [
                            SNew(SButton)
                                .Text(FText::FromString(TEXT("Browse")))
                                .OnClicked(this, &SSlateQuickstartWindow::OnSelectJsonClicked)
                        ]
                ]

            + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10.0f)
                [
                    SNew(SHorizontalBox)

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString(TEXT("Tile Mapping DataAsset: ")))
                        ]

                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        .VAlign(VAlign_Center)
                        .Padding(8.0f, 0.0f)
                        [
                            SNew(SObjectPropertyEntryBox)
                                .AllowedClass(UTileMappingDataAsset_plugin::StaticClass())
                                .ObjectPath(this, &SSlateQuickstartWindow::GetSelectedTileMappingAssetPath)
                                .OnObjectChanged(this, &SSlateQuickstartWindow::OnTileMappingAssetSelected)
                        ]

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        [
                            SNew(SButton)
                                .Text(FText::FromString(TEXT("Create Mapping DA")))
                                .IsEnabled(this, &SSlateQuickstartWindow::CanCreateMappingDA)
                                .OnClicked(this, &SSlateQuickstartWindow::OnCreateMappingDAClicked)
                        ]
                ]

            + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10.0f)
                [
                    SNew(SHorizontalBox)

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString(TEXT("Object Mapping DataAsset: ")))
                        ]

                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        .VAlign(VAlign_Center)
                        .Padding(8.0f, 0.0f)
                        [
                            SNew(SObjectPropertyEntryBox)
                                .AllowedClass(UObjectMappingDataAsset_plugin::StaticClass())
                                .ObjectPath(this, &SSlateQuickstartWindow::GetSelectedObjectMappingAssetPath)
                                .OnObjectChanged(this, &SSlateQuickstartWindow::OnObjectMappingAssetSelected)
                        ]

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        [
                            SNew(SButton)
                                .Text(FText::FromString(TEXT("Create Object Mapping DA")))
                                .IsEnabled(this, &SSlateQuickstartWindow::CanCreateObjectMappingDA)
                                .OnClicked(this, &SSlateQuickstartWindow::OnCreateObjectMappingDAClicked)
                        ]
                ]

            + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10.0f, 0.0f, 10.0f, 10.0f)
                [
                    SAssignNew(StatusTextBlock, STextBlock)
                        .Text(FText::FromString(TEXT("Select a JSON file first.")))
                ]

            + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10.0f)
                [
                    SNew(SButton)
                        .Text(FText::FromString(TEXT("Generate Map")))
                        .OnClicked(this, &SSlateQuickstartWindow::OnGenerateClicked)
                ]
        ];
}
FReply SSlateQuickstartWindow::OnSelectJsonClicked()
{
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

    if (!DesktopPlatform)
    {
        UE_LOG(LogTemp, Warning, TEXT("DesktopPlatform is not available."));
        return FReply::Handled();
    }

    TArray<FString> OutFiles;

    void* ParentWindowHandle = nullptr;

    const TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindBestParentWindowForDialogs(nullptr);

    if (ParentWindow.IsValid() && ParentWindow->GetNativeWindow().IsValid())
    {
        ParentWindowHandle = ParentWindow->GetNativeWindow()->GetOSWindowHandle();
    }

    const bool bFileSelected = DesktopPlatform->OpenFileDialog(
        ParentWindowHandle,
        TEXT("Select JSON File"),
        FPaths::ProjectDir(),
        TEXT(""),
        TEXT("JSON Files (*.json)|*.json"),
        EFileDialogFlags::None,
        OutFiles
    );

    if (bFileSelected && OutFiles.Num() > 0)
    {
        SelectedJsonPath = OutFiles[0];

        UE_LOG(LogTemp, Warning, TEXT("Selected JSON Path: %s"), *SelectedJsonPath);

        if (JsonPathTextBlock.IsValid())
        {
            JsonPathTextBlock->SetText(FText::FromString(SelectedJsonPath));
        }

        SetStatusMessage(TEXT("JSON selected. You can create a Mapping DA now."));
    }

    return FReply::Handled();
}
FReply SSlateQuickstartWindow::OnCreateMappingDAClicked()
{
    TArray<FName> TileTypes;

    if (!CollectTileTypesFromSelectedJson(TileTypes))
    {
        return FReply::Handled();
    }

    if (TileTypes.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("No tile types found in selected JSON."));
        SetStatusMessage(TEXT("No tile types found in selected JSON."));
        return FReply::Handled();
    }

    const FString BasePackageName = TEXT("/Game/JsonTOTileGame/DA_TileMapping");

    FString PackageName;
    FString AssetName;
    FAssetToolsModule::GetModule().Get().CreateUniqueAssetName(
        BasePackageName,
        TEXT(""),
        PackageName,
        AssetName
    );

    UPackage* Package = CreatePackage(*PackageName);

    if (!Package)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create package: %s"), *PackageName);
        SetStatusMessage(TEXT("Failed to create Mapping DA package."));
        return FReply::Handled();
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
        SetStatusMessage(TEXT("Failed to create Mapping DA asset."));
        return FReply::Handled();
    }

    for (const FName& TileType : TileTypes)
    {
        FTileMappingInfo_plugin MappingInfo;
        MappingInfo.TileType = TileType;
        NewMappingAsset->TileMappings.Add(MappingInfo);
    }

    FAssetRegistryModule::AssetCreated(NewMappingAsset);
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

    const bool bSaved = UPackage::SavePackage(
        Package,
        NewMappingAsset,
        *PackageFileName,
        SaveArgs
    );

    if (!bSaved)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save Tile Mapping DataAsset: %s"), *PackageName);
        SetStatusMessage(TEXT("Failed to save Mapping DA. Check the Output Log."));
        return FReply::Handled();
    }

    SelectedTileMappingDataAsset = NewMappingAsset;

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Created Tile Mapping DataAsset: %s with %d tile types."),
        *NewMappingAsset->GetPathName(),
        TileTypes.Num()
    );

    SetStatusMessage(FString::Printf(
        TEXT("Created %s with %d tile types."),
        *NewMappingAsset->GetName(),
        TileTypes.Num()
    ));

    if (GEditor)
    {
        TArray<UObject*> ObjectsToSync;
        ObjectsToSync.Add(NewMappingAsset);
        GEditor->SyncBrowserToObjects(ObjectsToSync);
    }

    return FReply::Handled();
}
FReply SSlateQuickstartWindow::OnCreateObjectMappingDAClicked()
{
    TArray<FName> ObjectTypes;

    if (!CollectObjectTypesFromSelectedJson(ObjectTypes))
    {
        return FReply::Handled();
    }

    if (ObjectTypes.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("No object types found in selected JSON."));
        SetStatusMessage(TEXT("No object types found in selected JSON."));
        return FReply::Handled();
    }

    const FString BasePackageName = TEXT("/Game/JsonTOTileGame/DA_ObjectMapping");

    FString PackageName;
    FString AssetName;
    FAssetToolsModule::GetModule().Get().CreateUniqueAssetName(
        BasePackageName,
        TEXT(""),
        PackageName,
        AssetName
    );

    UPackage* Package = CreatePackage(*PackageName);

    if (!Package)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create package: %s"), *PackageName);
        SetStatusMessage(TEXT("Failed to create Object Mapping DA package."));
        return FReply::Handled();
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
        SetStatusMessage(TEXT("Failed to create Object Mapping DA asset."));
        return FReply::Handled();
    }

    for (const FName& ObjectType : ObjectTypes)
    {
        FObjectMappingInfo_plugin MappingInfo;
        MappingInfo.ObjectType = ObjectType;
        NewMappingAsset->ObjectMappings.Add(MappingInfo);
    }

    FAssetRegistryModule::AssetCreated(NewMappingAsset);
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

    const bool bSaved = UPackage::SavePackage(
        Package,
        NewMappingAsset,
        *PackageFileName,
        SaveArgs
    );

    if (!bSaved)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save Object Mapping DataAsset: %s"), *PackageName);
        SetStatusMessage(TEXT("Failed to save Object Mapping DA. Check the Output Log."));
        return FReply::Handled();
    }

    SelectedObjectMappingDataAsset = NewMappingAsset;

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Created Object Mapping DataAsset: %s with %d object types."),
        *NewMappingAsset->GetPathName(),
        ObjectTypes.Num()
    );

    SetStatusMessage(FString::Printf(
        TEXT("Created %s with %d object types."),
        *NewMappingAsset->GetName(),
        ObjectTypes.Num()
    ));

    if (GEditor)
    {
        TArray<UObject*> ObjectsToSync;
        ObjectsToSync.Add(NewMappingAsset);
        GEditor->SyncBrowserToObjects(ObjectsToSync);
    }

    return FReply::Handled();
}
FReply SSlateQuickstartWindow::OnGenerateClicked()
{
    if (SelectedJsonPath.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("No JSON file selected."));
        SetStatusMessage(TEXT("Select a JSON file before generating a map."));
        return FReply::Handled();
    }

    if (!SelectedTileMappingDataAsset.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("No Tile Mapping DataAsset selected."));
        SetStatusMessage(TEXT("Select or create a Mapping DA before generating a map."));
        return FReply::Handled();
    }

    GenerateTileMapFromSelectedJson();

    return FReply::Handled();
}
bool SSlateQuickstartWindow::CanCreateMappingDA() const
{
    return !SelectedJsonPath.IsEmpty();
}
bool SSlateQuickstartWindow::CanCreateObjectMappingDA() const
{
    return !SelectedJsonPath.IsEmpty();
}
bool SSlateQuickstartWindow::CollectTileTypesFromSelectedJson(TArray<FName>& OutTileTypes)
{
    OutTileTypes.Reset();

    TSharedPtr<FJsonObject> JsonObject;

    if (!LoadSelectedJsonObject(JsonObject))
    {
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>>* TilesArray = nullptr;

    if (!JsonObject->TryGetArrayField(TEXT("tiles"), TilesArray))
    {
        UE_LOG(LogTemp, Error, TEXT("No tiles array found in JSON: %s"), *SelectedJsonPath);
        SetStatusMessage(TEXT("Selected JSON has no tiles array."));
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
bool SSlateQuickstartWindow::CollectObjectTypesFromSelectedJson(TArray<FName>& OutObjectTypes)
{
    OutObjectTypes.Reset();

    TSharedPtr<FJsonObject> JsonObject;

    if (!LoadSelectedJsonObject(JsonObject))
    {
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>>* ObjectsArray = nullptr;

    if (!JsonObject->TryGetArrayField(TEXT("objects"), ObjectsArray))
    {
        UE_LOG(LogTemp, Error, TEXT("No objects array found in JSON: %s"), *SelectedJsonPath);
        SetStatusMessage(TEXT("Selected JSON has no objects array."));
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
bool SSlateQuickstartWindow::LoadSelectedJsonObject(TSharedPtr<FJsonObject>& OutJsonObject)
{
    OutJsonObject.Reset();

    if (SelectedJsonPath.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("No JSON file selected."));
        SetStatusMessage(TEXT("Select a JSON file first."));
        return false;
    }

    FString JsonText;

    if (!FFileHelper::LoadFileToString(JsonText, *SelectedJsonPath))
    {
        UE_LOG(LogTemp, Error, TEXT("JSON file load failed: %s"), *SelectedJsonPath);
        SetStatusMessage(TEXT("Failed to load selected JSON file."));
        return false;
    }

    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);

    if (!FJsonSerializer::Deserialize(Reader, OutJsonObject) || !OutJsonObject.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("JSON parse failed: %s"), *SelectedJsonPath);
        SetStatusMessage(TEXT("Failed to parse selected JSON file."));
        return false;
    }

    return true;
}
bool SSlateQuickstartWindow::GenerateTileMapFromSelectedJson()
{
    TSharedPtr<FJsonObject> JsonObject;

    if (!LoadSelectedJsonObject(JsonObject))
    {
        return false;
    }

    UTileMappingDataAsset_plugin* MappingData = SelectedTileMappingDataAsset.Get();

    if (!MappingData)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Tile Mapping DataAsset selected."));
        SetStatusMessage(TEXT("Select or create a Mapping DA before generating a map."));
        return false;
    }

    const TSharedPtr<FJsonObject>* MapObjectPtr = nullptr;

    if (!JsonObject->TryGetObjectField(TEXT("map"), MapObjectPtr) || !MapObjectPtr || !MapObjectPtr->IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("No map object found in JSON: %s"), *SelectedJsonPath);
        SetStatusMessage(TEXT("Selected JSON has no map object."));
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
        SetStatusMessage(TEXT("Map object requires width, height, and tileSize."));
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>>* TilesArray = nullptr;

    if (!JsonObject->TryGetArrayField(TEXT("tiles"), TilesArray))
    {
        UE_LOG(LogTemp, Error, TEXT("No tiles array found in JSON: %s"), *SelectedJsonPath);
        SetStatusMessage(TEXT("Selected JSON has no tiles array."));
        return false;
    }

    const FString JsonBaseName = FPaths::GetBaseFilename(SelectedJsonPath);
    const FString BasePackageName = TEXT("/Game/JsonTOTileGame/GeneratedMaps/") + JsonBaseName + TEXT("_TileMap");

    FString PackageName;
    FString AssetName;
    FAssetToolsModule::GetModule().Get().CreateUniqueAssetName(
        BasePackageName,
        TEXT(""),
        PackageName,
        AssetName
    );

    UPackage* Package = CreatePackage(*PackageName);

    if (!Package)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create TileMap package: %s"), *PackageName);
        SetStatusMessage(TEXT("Failed to create TileMap package."));
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
        SetStatusMessage(TEXT("Failed to create TileMap asset."));
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
        SetStatusMessage(TEXT("Failed to create TileLayer."));
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

        if (!MappingData->FindTileInfo(FName(*TypeString), MappingInfo) || !MappingInfo.TileSet)
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
        SetStatusMessage(TEXT("No tiles placed. Check Mapping DA TileSet assignments."));
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

    const bool bSaved = UPackage::SavePackage(
        Package,
        NewTileMap,
        *PackageFileName,
        SaveArgs
    );

    if (!bSaved)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save TileMap asset: %s"), *PackageName);
        SetStatusMessage(TEXT("Failed to save TileMap asset. Check the Output Log."));
        return false;
    }

    UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;

    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("Editor world is null. TileMap asset was created but not placed."));
        SetStatusMessage(TEXT("TileMap asset created, but editor world was not available for placement."));
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
        SetStatusMessage(TEXT("TileMap asset created, but actor placement failed."));
        return true;
    }

    UPaperTileMapComponent* TileMapComponent = TileMapActor->GetRenderComponent();

    if (!TileMapComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("TileMapComponent is null."));
        SetStatusMessage(TEXT("TileMap asset created, but actor component was missing."));
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
        UObjectMappingDataAsset_plugin* ObjectMappingData = SelectedObjectMappingDataAsset.Get();

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

    SetStatusMessage(FString::Printf(
        TEXT("Generated %s. Tiles: %d. Objects: %d. Missing mappings: %d/%d."),
        *AssetName,
        PlacedTileCount,
        SpawnedObjectCount,
        MissingMappingCount,
        MissingObjectMappingCount
    ));

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
void SSlateQuickstartWindow::SetStatusMessage(const FString& Message)
{
    if (StatusTextBlock.IsValid())
    {
        StatusTextBlock->SetText(FText::FromString(Message));
    }
}
void SSlateQuickstartWindow::OnTileMappingAssetSelected(const FAssetData& AssetData)
{
    UObject* SelectedAsset = AssetData.GetAsset();

    SelectedTileMappingDataAsset = Cast<UTileMappingDataAsset_plugin>(SelectedAsset);

    if (SelectedTileMappingDataAsset.IsValid())
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Selected Tile Mapping DataAsset: %s"),
            *SelectedTileMappingDataAsset->GetName()
        );
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid Tile Mapping DataAsset selected."));
    }
}
FString SSlateQuickstartWindow::GetSelectedTileMappingAssetPath() const
{
    if (SelectedTileMappingDataAsset.IsValid())
    {
        return SelectedTileMappingDataAsset->GetPathName();
    }

    return FString();
}
void SSlateQuickstartWindow::OnObjectMappingAssetSelected(const FAssetData& AssetData)
{
    UObject* SelectedAsset = AssetData.GetAsset();

    SelectedObjectMappingDataAsset = Cast<UObjectMappingDataAsset_plugin>(SelectedAsset);

    if (SelectedObjectMappingDataAsset.IsValid())
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Selected Object Mapping DataAsset: %s"),
            *SelectedObjectMappingDataAsset->GetName()
        );
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid Object Mapping DataAsset selected."));
    }
}
FString SSlateQuickstartWindow::GetSelectedObjectMappingAssetPath() const
{
    if (SelectedObjectMappingDataAsset.IsValid())
    {
        return SelectedObjectMappingDataAsset->GetPathName();
    }

    return FString();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
