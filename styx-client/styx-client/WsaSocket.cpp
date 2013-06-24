#include "WsaSocket.h"

#include "WsaException.h"

WsaSocket::WsaSocket(int family, int type, int protocol)
{
	_socket = socket(family, type, protocol);
	if (_socket == INVALID_SOCKET)
	{
		throw WsaException("Error at socket()", WSAGetLastError());
	}
}

WsaSocket::~WsaSocket()
{
	if (_connected)
	{
		closesocket(_socket);
	}
}

void WsaSocket::connect(addrinfo &address)
{
	auto result = ::connect(_socket, address.ai_addr, address.ai_addrlen);
	_connected = true;
	if (result == SOCKET_ERROR)
	{
		throw WsaException("Connection error", WSAGetLastError());
	}
}

void WsaSocket::send(const char *bytes, int length)
{
	auto result = ::send(_socket, bytes, length, 0);
	if (result == SOCKET_ERROR)
	{
		throw WsaException("Sending error", WSAGetLastError());
	}
}

void WsaSocket::send(std::uint32_t data)
{
	const auto size = sizeof(std::uint32_t);
	union
	{
		std::uint32_t data;
		char bytes[size];
	} converter;

	converter.data = static_cast<std::uint32_t>(::htonl(data));
	send(converter.bytes, size);
}

void WsaSocket::send(const std::string &string)
{
	send(string.c_str(), string.length());
}

SOCKET WsaSocket::handle() const
{
	return _socket;
}
