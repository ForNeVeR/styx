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

WsaSocket::~WsaSocket(void)
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

void WsaSocket::send(const std::string &string)
{
	send(string.c_str(), string.length());
}
