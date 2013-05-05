#include "WsaInit.h"
#include "WsaException.h"

WsaInit::WsaInit()
{
	_wsaData = WSADATA();
	auto result = WSAStartup(MAKEWORD(2,2), &_wsaData);
	if (result)
	{
		throw WsaException("WSA initialization error", result);
	}
}

WsaInit::~WsaInit()
{
	WSACleanup();
}
