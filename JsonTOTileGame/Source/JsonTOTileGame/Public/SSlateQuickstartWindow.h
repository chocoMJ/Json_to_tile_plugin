// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UTileMappingDataAsset_plugin;
class UObjectMappingDataAsset_plugin;

/**
 * 
 */
class JSONTOTILEGAME_API SSlateQuickstartWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSlateQuickstartWindow)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
	FReply OnSelectJsonClicked();
	FReply OnCreateMappingDAClicked();
	FReply OnCreateObjectMappingDAClicked();
	FReply OnGenerateClicked();

	bool CanCreateMappingDA() const;
	void SetStatusMessage(const FString& Message);

	void OnTileMappingAssetSelected(const FAssetData& AssetData);
	FString GetSelectedTileMappingAssetPath() const;
	void OnObjectMappingAssetSelected(const FAssetData& AssetData);
	FString GetSelectedObjectMappingAssetPath() const;

private:
	FString SelectedJsonPath;
	TSharedPtr<class STextBlock> JsonPathTextBlock;
	TSharedPtr<class STextBlock> StatusTextBlock;
	TWeakObjectPtr<UTileMappingDataAsset_plugin> SelectedTileMappingDataAsset;
	TWeakObjectPtr<UObjectMappingDataAsset_plugin> SelectedObjectMappingDataAsset;
};
