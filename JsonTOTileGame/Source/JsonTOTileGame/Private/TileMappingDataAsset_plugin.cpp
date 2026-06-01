// Fill out your copyright notice in the Description page of Project Settings.


#include "TileMappingDataAsset_plugin.h"

bool UTileMappingDataAsset_plugin::FindTileInfo(FName TileType, FTileMappingInfo_plugin& OutInfo) const
{
	for (const FTileMappingInfo_plugin& Info : TileMappings)
	{
		if (Info.TileType == TileType)
		{
			OutInfo = Info;
			return true;
		}
	}

	return false;
}
