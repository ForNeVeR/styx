#pragma once

#include <string>

#include "WindowsIncludes.h"

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

