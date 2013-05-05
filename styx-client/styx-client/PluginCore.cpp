#include "PluginCore.h"

#include <m_langpack.h>
#include <m_clist.h>
#include <m_skin.h>

const auto EnableServiceName = "Styx/EnableCommand";

PluginCore::PluginCore(PLUGININFOEX &pluginInfo)
	: _pluginInfo(pluginInfo)
{
}

PluginCore::~PluginCore()
{
}

void PluginCore::initialize()
{
	InitializeLangpack();
	InitializeMainMenu();
	InitializeHooks();
}

void PluginCore::deinitialize()
{
	// TODO: Disable synchronization if enabled.
}

void PluginCore::InitializeLangpack()
{
	mir_getLP(&_pluginInfo);
}

INT_PTR EnableCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Enable synchronization if not enabled.
	return 0;
}

void PluginCore::InitializeMainMenu()
{
	CreateServiceFunction(EnableServiceName, &EnableCommand);

	auto mi = CLISTMENUITEM();
	mi.cbSize = sizeof(mi);
	mi.position = -0x7FFFFFFF;
	mi.flags = CMIF_UNICODE;
	mi.hIcon = LoadSkinnedIcon(SKINICON_OTHER_MIRANDA);
	mi.ptszName = LPGENT("&Enable Styx");
	mi.pszService = const_cast<char*>(EnableServiceName);

	Menu_AddMainMenuItem(&mi);
}

void PluginCore::InitializeHooks()
{

}