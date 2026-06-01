// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TileMappingDataAsset_plugin.generated.h"

class UPaperTileSet;

USTRUCT(BlueprintType)
struct FTileMappingInfo_plugin
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
	FName TileType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tile")
	UPaperTileSet* TileSet = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tile")
	int32 TileIndex = 0;
};

UCLASS()
class JSONTOTILEGAME_API UTileMappingDataAsset_plugin : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tile Mapping", meta = (EditFixedSize))
	TArray<FTileMappingInfo_plugin> TileMappings;

	bool FindTileInfo(FName TileType, FTileMappingInfo_plugin& OutInfo) const;
};
