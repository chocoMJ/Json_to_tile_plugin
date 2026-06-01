// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameFramework/Actor.h"
#include "ObjectMappingDataAsset_plugin.generated.h"

USTRUCT(BlueprintType)
struct FObjectMappingInfo_plugin
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Object")
	FName ObjectType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Object")
	TSubclassOf<AActor> ActorClass = nullptr;
};

UCLASS()
class JSONTOTILEGAME_API UObjectMappingDataAsset_plugin : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Object Mapping", meta = (EditFixedSize))
	TArray<FObjectMappingInfo_plugin> ObjectMappings;

	bool FindObjectInfo(FName ObjectType, FObjectMappingInfo_plugin& OutInfo) const;
};
