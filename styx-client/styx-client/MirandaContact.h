#pragma once

#include <string>

#include "WindowsIncludes.h"

struct MirandaContact
{
	static std::wstring GetUID(HANDLE contactHandle);
};

