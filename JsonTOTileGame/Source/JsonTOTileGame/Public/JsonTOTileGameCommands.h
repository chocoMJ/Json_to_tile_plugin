// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "JsonTOTileGameStyle.h"

class FJsonTOTileGameCommands : public TCommands<FJsonTOTileGameCommands>
{
public:

	FJsonTOTileGameCommands()
		: TCommands<FJsonTOTileGameCommands>(TEXT("JsonTOTileGame"), NSLOCTEXT("Contexts", "JsonTOTileGame", "JsonTOTileGame Plugin"), NAME_None, FJsonTOTileGameStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};