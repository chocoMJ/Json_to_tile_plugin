// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectMappingDataAsset_plugin.h"

bool UObjectMappingDataAsset_plugin::FindObjectInfo(FName ObjectType, FObjectMappingInfo_plugin& OutInfo) const
{
	for (const FObjectMappingInfo_plugin& Info : ObjectMappings)
	{
		if (Info.ObjectType == ObjectType)
		{
			OutInfo = Info;
			return true;
		}
	}

	return false;
}
