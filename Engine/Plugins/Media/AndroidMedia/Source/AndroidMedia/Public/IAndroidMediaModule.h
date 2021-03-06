// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Templates/SharedPointer.h"
#include "Modules/ModuleInterface.h"

class IMediaPlayer;

/**
 * Interface for the AndroidMedia module.
 */
class IAndroidMediaModule
	: public IModuleInterface
{
public:

	/**
	 * Creates a Android based media player.
	 *
	 * @return A new media player, or nullptr if a player couldn't be created.
	 */
	virtual TSharedPtr<IMediaPlayer, ESPMode::ThreadSafe> CreatePlayer() = 0;

public:

	/** Virtual destructor. */
	virtual ~IAndroidMediaModule() { }
};
