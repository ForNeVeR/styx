#pragma once

#include "WindowsIncludes.h"

class Handle
{
public:
	Handle(HANDLE handle);
	~Handle();

	operator HANDLE();

private:
	HANDLE _handle;
};

