#include "Connector.h"

#include <array>
#include <exception>

#include <boost/log/trivial.hpp>

#include <process.h>

#include "ChunkHashResultDef.pb.h"
#include "LoginDef.pb.h"
#include "LoginResultDef.pb.h"
#include "MemoryUtils.h"
#include "Synchronizer.h"
#include "WsaEvent.h"
#include "WsaException.h"
#include "WsaInit.h"

using namespace ru::org::codingteam::styx;

Connector::Connector()
	: _started(false),
	_threadId(0),
	_queueEventHandle(::CreateEventW(nullptr, false, false, L"ConnectorQueueEvent")),
	_socketBuffer(),
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

	_threadId = _beginthread(&loop, 0, this);
	if (_threadId == -1L)
	{
		auto message = "Cannot start thread; errno = " + std::to_string(errno);
		throw std::exception(message.c_str());
	}

	_started = true;
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

std::unique_ptr<addrinfo, decltype(&freeaddrinfo)> Connector::getServerAddress()
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

	auto result = MemoryUtils::MakeUniquePtr(temp, &freeaddrinfo);

	if (code)
	{
		throw WsaException("getaddrinfo failed", code);
	}

	return result;
}

void _cdecl Connector::loop(void *self)
{
	auto connector = static_cast<Connector*>(self);
	auto synchronizer = Synchronizer();

	auto init = WsaInit();
	auto address = connector->getServerAddress();
	auto socket = WsaSocket(address->ai_family, address->ai_socktype, address->ai_protocol);
	socket.connect(*address);

	synchronizer.dispatchConnected(*connector, socket);
	
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
		try
		{
			std::array<HANDLE, 2> events = { connector->_queueEventHandle, eventHandle };
			auto waitResult = WaitForMultipleObjects(events.size(), events.data(), false, INFINITE);
			switch (waitResult)
			{
			case WAIT_OBJECT_0:
				connector->dispatchMessages(synchronizer, socket);
				break;
			case WAIT_OBJECT_0 + 1:
				connector->dispatchData(synchronizer, socket);
				break;
			default:
				throw std::exception("Wait failed");
			}
		}
		catch (std::exception &exception)
		{
			BOOST_LOG_TRIVIAL(error) << exception.what();
			return;
		}
	}
}

void Connector::dispatchMessages(Synchronizer &synchronizer, WsaSocket &socket)
{
	Message message;
	while(_messageQueue.try_pop(message))
	{
		synchronizer.dispatchMessage(*this, socket, message);
	}
}

void Connector::dispatchData(Synchronizer &synchronizer, WsaSocket &socket)
{
	uint8_t buffer[1024];
	
	auto dataSize = 0;
	while (dataSize = socket.recv(buffer, sizeof(buffer)))
	{
		if (dataSize == SOCKET_ERROR)
		{
			auto code = WSAGetLastError();
			if (code == WSAEWOULDBLOCK)
			{
				// Yup, this is normal. No time to explain, just return.
				return;
			}

			throw WsaException("recv error", code);
		}

		for (int i = 0; i < dataSize; ++i)
		{
			_socketBuffer.push_back(buffer[i]);
		}

		const auto headerSize = sizeof(uint32_t) * 2;

		auto size = _socketBuffer.size();
		if (size >= headerSize)
		{
			auto data = _socketBuffer.data();
			auto intPtr = reinterpret_cast<const uint32_t*>(data);
			auto type = ntohl(intPtr[0]);
			auto length = ntohl(intPtr[1]);
			if (size < headerSize + length)
			{
				continue;
			}

			auto body = data + headerSize;
			auto message = std::vector<char>(body, body + length);
			_socketBuffer.erase(_socketBuffer.begin(), _socketBuffer.begin() + headerSize + length);

			switch (type)
			{
			case MessageType::LoginResponse:
				{
					auto loginResponse = readMessage<LoginResult>(message.data(), message.size());
					synchronizer.dispatchMessage(*this, socket, loginResponse);
				}
				break;
			case MessageType::ChunkHashResponse:
				{
					auto chunkHashResult = readMessage<ChunkHashResult>(message.data(), message.size());
					synchronizer.dispatchMessage(*this, socket, chunkHashResult);
				}
			default:
				throw std::exception("Unknown message type");
			}
		}
	}
}

template<class T> T Connector::readMessage(void *data, int size)
{
	auto t = T();
	if (!t.ParseFromArray(data, size))
	{
		throw std::exception("Cannot parse object");
	}

	return t;
}

void Connector::sendLogin(WsaSocket &socket)
{
	// TODO: Get the user name and the password from the database.

	auto message = Login();
	message.set_username("user");
	message.set_password("password");

	sendDatagram(socket, MessageType::LoginRequest, message);
}

void Connector::sendMessage(WsaSocket &socket, const Message &message)
{
	sendDatagram(socket, MessageType::MessageRequest, message);
}

void Connector::sendChunkHash(WsaSocket &socket, const ChunkHash &chunkHash)
{
	sendDatagram(socket, MessageType::ChunkHashRequest, chunkHash);
}

void Connector::sendDatagram(WsaSocket &socket, const MessageType &type, const google::protobuf::Message &message)
{
	auto size = message.ByteSize();
	
	socket.send(type);
	socket.send(size);
	socket.send(message.SerializeAsString());
}