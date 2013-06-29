#include "Connector.h"

#include <array>
#include <exception>

#include "MemoryUtils.h"
#include "WsaEvent.h"
#include "WsaException.h"
#include "WsaInit.h"

using namespace ru::org::codingteam::styx;

Connector::Connector()
	: _started(false),
	_threadId(0),
	_queueEventHandle(::CreateEventW(nullptr, false, false, L"ConnectorQueueEvent")),
	_messageQueue()
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

void Connector::queueMessage(const Message &message)
{
	_messageQueue.push(message);
	::SetEvent(_queueEventHandle);
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
	auto socket = WsaSocket(address->ai_family, address->ai_socktype, address->ai_protocol);
	socket.connect(*address);
	auto event = WsaEvent();
	auto socketHandle = socket.handle();
	auto eventHandle = event.handle();
	auto selectResult = WSAEventSelect(socketHandle, eventHandle, FD_READ | FD_CLOSE);
	if (selectResult)
	{
		throw WsaException("Cannot select WSA event", selectResult);
	}
	
	while (true) // TODO: Find the way to stop the thread.
	{
		std::array<HANDLE, 2> events = { connector->_queueEventHandle, eventHandle };
		auto waitResult = WaitForMultipleObjects(events.size(), events.data(), false, INFINITE);
		switch (waitResult)
		{
		case WAIT_OBJECT_0:
			connector->sendMessage(socket);
			break;
		case WAIT_OBJECT_0 + 1:
			connector->receiveData(socket);
			break;
		default:
			throw std::exception("Wait failed");
		}
	}
}

void Connector::sendMessage(WsaSocket &socket)
{
	Message message;
	while(_messageQueue.try_pop(message))
	{
		auto size = message.ByteSize();

		socket.send(size);
		socket.send(message.SerializeAsString());
	}
}

void Connector::receiveData(WsaSocket &socket)
{
	// TODO: Receive data from server.
}
