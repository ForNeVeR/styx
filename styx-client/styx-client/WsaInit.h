#pragma once

#include "WindowsIncludes.h"

class WsaInit
{
public:
	WsaInit();
	~WsaInit();

private:
	WSADATA _wsaData;
};

