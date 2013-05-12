#include "PluginCore.h"

#include <string>

#include <m_langpack.h>
#include <m_clist.h>
#include <m_database.h>
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

INT_PTR EnableServiceFunction(void *core, LPARAM param1, LPARAM param2)
{
	// TODO: Enable synchronization if not enabled.
	return 0;
}

void PluginCore::InitializeMainMenu()
{
	CreateServiceFunctionObj(EnableServiceName, &EnableServiceFunction, this);

	auto mi = CLISTMENUITEM();
	mi.cbSize = sizeof(mi);
	mi.position = -0x7FFFFFFF;
	mi.flags = CMIF_UNICODE;
	mi.hIcon = LoadSkinnedIcon(SKINICON_OTHER_MIRANDA);
	mi.ptszName = LPGENT("&Enable Styx");
	mi.pszService = const_cast<char*>(EnableServiceName);

	Menu_AddMainMenuItem(&mi);
}

int EventAddedHook(void *core, WPARAM wParam, LPARAM lParam)
{
	auto hDbEvent = reinterpret_cast<HANDLE>(lParam);

	auto eventInfo = DBEVENTINFO();
	CallService(MS_DB_EVENT_GET, reinterpret_cast<WPARAM>(hDbEvent), reinterpret_cast<LPARAM>(&eventInfo));

	if (eventInfo.eventType != EVENTTYPE_MESSAGE)
	{
		// We handle only messages here.
		return 0;
	}

	auto messageTime = eventInfo.timestamp;
	auto wText = DbGetEventTextW(&eventInfo, 0);
	auto text = std::wstring(wText);
	mir_free(wText);
	
	if (eventInfo.flags & DBEF_SENT)
	{
		// TODO: Handle sent message.
	}
	else
	{
		// TODO: Handle received message.
	}

	return 0;
}

void PluginCore::InitializeHooks()
{
	HookEventObj(ME_DB_EVENT_ADDED, EventAddedHook, this);
}