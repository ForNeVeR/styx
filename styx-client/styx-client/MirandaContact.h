#pragma once

#include <string>

#include <windows.h>

struct MirandaContact
{
	static std::wstring GetUID(HANDLE contactHandle);
};

