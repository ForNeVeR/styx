#pragma once

#include <windows.h>
#include <tchar.h>

#include <newpluginapi.h>

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
};

