#include "Connector.h"

#include <exception>

#include "MemoryUtils.h"
#include "WsaException.h"
#include "WsaInit.h"
#include "WsaSocket.h"

Connector::Connector()
	: _started(false), _threadId(0)
{
}

Connector::~Connector()
{
}

void Connector::start()
{
	if (_started)
	{
		throw std::exception("Thread already started");
	}

	CreateThread(nullptr, 0, &loop, this, 0, &_threadId);
}

void Connector::stop()
{
	if (_started)
	{
		// TODO: send signal to the thread.
	}
}

std::unique_ptr<addrinfo, decltype(&freeaddrinfo)> Connector::GetServerAddress()
{
	const auto host = "127.0.0.1";
	const auto defaultPort = "6060";

	auto address = addrinfo();
	address.ai_family = AF_UNSPEC;
	address.ai_socktype = SOCK_STREAM;
	address.ai_protocol = IPPROTO_TCP;

	addrinfo *temp = nullptr;

	// Resolve the server address and port:
	auto code = getaddrinfo(host, defaultPort, &address, &temp);

	auto deleter = &freeaddrinfo;
	auto result = MemoryUtils::MakeUniquePtr(temp, deleter);

	if (code)
	{
		throw WsaException("getaddrinfo failed", code);
	}

	return result;
}

DWORD Connector::loop(LPVOID self)
{
	auto connector = static_cast<Connector*>(self);
	auto init = WsaInit();
	auto address = connector->GetServerAddress();
	WsaSocket socket(address->ai_family, address->ai_socktype, address->ai_protocol);
	socket.connect(*address);

	while (true)
	{
		// TODO: Read from socket or wait for some event.
		Sleep(1000);
	}
}
