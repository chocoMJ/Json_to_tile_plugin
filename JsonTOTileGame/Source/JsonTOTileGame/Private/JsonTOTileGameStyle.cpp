// Copyright Epic Games, Inc. All Rights Reserved.

#include "JsonTOTileGameStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FJsonTOTileGameStyle::StyleInstance = nullptr;

void FJsonTOTileGameStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FJsonTOTileGameStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FJsonTOTileGameStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("JsonTOTileGameStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FJsonTOTileGameStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("JsonTOTileGameStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("JsonTOTileGame")->GetBaseDir() / TEXT("Resources"));

	Style->Set("JsonTOTileGame.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));

	return Style;
}

void FJsonTOTileGameStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FJsonTOTileGameStyle::Get()
{
	return *StyleInstance;
}
