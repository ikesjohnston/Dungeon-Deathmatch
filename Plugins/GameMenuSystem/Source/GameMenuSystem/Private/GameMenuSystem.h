#pragma once

#include "ModuleManager.h"

class GameMenuSystemImpl : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	void StartupModule();
	void ShutdownModule();
};