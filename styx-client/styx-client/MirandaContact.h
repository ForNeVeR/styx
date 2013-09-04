#pragma once

#include <string>

#include "WindowsIncludes.h"

class MirandaContact
{
public:
	static MirandaContact GetFirst();
	
	MirandaContact(HANDLE handle);

	HANDLE handle() const;
	std::wstring uid() const;

private:
	HANDLE _handle;
};

