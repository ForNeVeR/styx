#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <string>

class WsaSocket
{
public:
	WsaSocket(int family, int type, int protocol);
	~WsaSocket();

	void connect(addrinfo &address);
	void send(const char *bytes, int length);
	void send(const std::string &string);

private:
	SOCKET _socket;
	bool _connected;
};

