#include "PluginCore.h"

PLUGININFOEX pluginInfo =
{
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

auto Core = PluginCore(pluginInfo);

HINSTANCE libraryInstance = nullptr;

int hLangpack;

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	libraryInstance = instance;
	return TRUE;
}

extern "C" __declspec(dllexport) PLUGININFOEX *MirandaPluginInfoEx(DWORD mirandaVersion)
{
	return &pluginInfo;
}

extern "C" __declspec(dllexport) const MUUID interfaces[] = { MIID_LAST };

extern "C" __declspec(dllexport) int Load()
{
	Core.Initialize();
	return 0;
}

extern "C" __declspec(dllexport) int Unload(void)
{
	Core.Deinitialize();
	return 0;
}
