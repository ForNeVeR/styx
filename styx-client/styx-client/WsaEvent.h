#pragma once

#include "WindowsIncludes.h"

class WsaEvent
{
public:
	WsaEvent();
	~WsaEvent();

	WSAEVENT handle();

private:
	WSAEVENT _handle;
};
