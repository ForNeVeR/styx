#pragma once

#include "WindowsIncludes.h"

class WsaEvent
{
public:
	WsaEvent();
	~WsaEvent();

	WSAEVENT handle() const;

private:
	WSAEVENT _handle;
};
