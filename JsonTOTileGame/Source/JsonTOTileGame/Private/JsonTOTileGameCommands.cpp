// Copyright Epic Games, Inc. All Rights Reserved.

#include "JsonTOTileGameCommands.h"

#define LOCTEXT_NAMESPACE "FJsonTOTileGameModule"

void FJsonTOTileGameCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "JsonTOTileGame", "Bring up JsonTOTileGame window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
