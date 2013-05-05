#include <windows.h>
#include <tchar.h>

#include <newpluginapi.h>
#include <m_langpack.h>
#include <m_clist.h>
#include <m_skin.h>

const auto EnableServiceName = "Styx/EnableCommand";

HINSTANCE libraryInstance = nullptr;

// Do not remove - part of API.
int hLangpack;

PLUGININFOEX pluginInfo = {
	sizeof(PLUGININFOEX),
	"Styx History Synchronizer",
	PLUGIN_MAKE_VERSION(0, 0, 0, 1),
	"History synchronizer with dedicated server.",
	"F. von Never",
	"neverthness@gmail.com",
	"© 2013 F. von Never",
	"https://github.com/ForNeVeR",
	UNICODE_AWARE,
	{ 0xf38aa380, 0x2267, 0x4ff7, { 0x9b, 0x19, 0xae, 0x96, 0x6b, 0xac, 0x49, 0xd8 } } // {F38AA380-2267-4FF7-9B19-AE966BAC49D8}
};

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	libraryInstance = instance;
	return TRUE;
}

INT_PTR EnableCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Enable synchronization if not enabled.
	return 0;
}

extern "C" __declspec(dllexport) PLUGININFOEX *MirandaPluginInfoEx(DWORD mirandaVersion)
{
	return &pluginInfo;
}

extern "C" __declspec(dllexport) const MUUID interfaces[] = { MIID_LAST };

extern "C" __declspec(dllexport) int Load()
{
	mir_getLP(&pluginInfo);

	CreateServiceFunction(EnableServiceName, &EnableCommand);

	auto mi = CLISTMENUITEM();
	mi.cbSize = sizeof(mi);
	mi.position = -0x7FFFFFFF;
	mi.flags = CMIF_UNICODE;
	mi.hIcon = LoadSkinnedIcon(SKINICON_OTHER_MIRANDA);
	mi.ptszName = LPGENT("&Enable Styx");
	mi.pszService = const_cast<char*>(EnableServiceName);

	Menu_AddMainMenuItem(&mi);

	return 0;
}

extern "C" __declspec(dllexport) int Unload(void)
{
	// TODO: Disable synchronization if enabled.
	return 0;
}
