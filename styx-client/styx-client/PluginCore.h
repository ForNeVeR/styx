#pragma once

#include <memory>

#include "WindowsIncludes.h"

#include <newpluginapi.h>

class Connector;

class PluginCore
{
public:
	PluginCore(PLUGININFOEX &pluginInfo);
	~PluginCore();
	
	void Initialize();
	void Deinitialize();

	void EnableSynchronization();
	void DisableSynchronization();

private:
	void InitializeLangpack();
	void InitializeMainMenu();
	void InitializeHooks();

	PLUGININFOEX &_pluginInfo;
	bool _isSynchronizationEnabled;
	std::unique_ptr<Connector> _connector;
};

