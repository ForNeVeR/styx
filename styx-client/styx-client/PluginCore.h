#pragma once

#include <windows.h>
#include <tchar.h>

#include <newpluginapi.h>

class PluginCore
{
public:
	PluginCore(PLUGININFOEX &pluginInfo);
	~PluginCore();
	
	void initialize();
	void deinitialize();

private:
	void InitializeLangpack();
	void InitializeMainMenu();
	void InitializeHooks();

	PLUGININFOEX &_pluginInfo;
};

