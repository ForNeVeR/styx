#include "WsaEvent.h"

#include "WsaException.h"

WsaEvent::WsaEvent()
	: _handle(WSA_INVALID_EVENT)
{
	_handle = WSACreateEvent();
	if (_handle == WSA_INVALID_EVENT)
	{
		throw WsaException("Cannot register event", WSAGetLastError());
	}
}

WsaEvent::~WsaEvent()
{
	if (_handle != WSA_INVALID_EVENT)
	{
		WSACloseEvent(_handle);
	}
}

WSAEVENT WsaEvent::handle() const
{
	return _handle;
}
