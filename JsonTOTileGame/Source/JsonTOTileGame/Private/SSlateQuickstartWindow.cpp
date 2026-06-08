// Fill out your copyright notice in the Description page of Project Settings.


#include "SSlateQuickstartWindow.h"
#include "JsonTOTileGameImporter.h"
#include "DesktopPlatformModule.h"
#include "Editor.h"
#include "IDesktopPlatform.h"
#include "Framework/Application/SlateApplication.h"
#include "Misc/Paths.h"

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
                                .IsEnabled(this, &SSlateQuickstartWindow::CanCreateMappingDA)
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
    FString StatusMessage;

    if (!FJsonTOTileGameImporter::CollectTileTypesFromJsonFile(SelectedJsonPath, TileTypes, StatusMessage))
    {
        SetStatusMessage(StatusMessage);
        return FReply::Handled();
    }

    if (TileTypes.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("No tile types found in selected JSON."));
        SetStatusMessage(TEXT("No tile types found in selected JSON."));
        return FReply::Handled();
    }

    UTileMappingDataAsset_plugin* NewMappingAsset = FJsonTOTileGameImporter::CreateTileMappingDataAsset(
        TileTypes,
        StatusMessage
    );
    
    if (!NewMappingAsset)
    {
        SetStatusMessage(StatusMessage);
        return FReply::Handled();
    }

    SelectedTileMappingDataAsset = NewMappingAsset;
    SetStatusMessage(StatusMessage);

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
    FString StatusMessage;

    if (!FJsonTOTileGameImporter::CollectObjectTypesFromJsonFile(SelectedJsonPath, ObjectTypes, StatusMessage))
    {
        SetStatusMessage(StatusMessage);
        return FReply::Handled();
    }

    if (ObjectTypes.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("No object types found in selected JSON."));
        SetStatusMessage(TEXT("No object types found in selected JSON."));
        return FReply::Handled();
    }

    UObjectMappingDataAsset_plugin* NewMappingAsset = FJsonTOTileGameImporter::CreateObjectMappingDataAsset(
        ObjectTypes,
        StatusMessage
    );
    
    if (!NewMappingAsset)
    {
        SetStatusMessage(StatusMessage);
        return FReply::Handled();
    }

    SelectedObjectMappingDataAsset = NewMappingAsset;
    SetStatusMessage(StatusMessage);

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

    FString StatusMessage;
    FJsonTOTileGameImporter::GenerateTileMapFromJsonFile(
        SelectedJsonPath,
        SelectedTileMappingDataAsset.Get(),
        SelectedObjectMappingDataAsset.Get(),
        StatusMessage
    );

    SetStatusMessage(StatusMessage);

    return FReply::Handled();
}
bool SSlateQuickstartWindow::CanCreateMappingDA() const
{
    return !SelectedJsonPath.IsEmpty();
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
