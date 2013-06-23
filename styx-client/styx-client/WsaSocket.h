#pragma once

#include <cstdint>
#include <string>

#include "WindowsIncludes.h"

class WsaSocket
{
public:
	WsaSocket(int family, int type, int protocol);
	~WsaSocket();

	void connect(addrinfo &address);
	void send(const char *bytes, int length);
	void send(std::uint32_t data);
	void send(const std::string &string);

	SOCKET handle() const;

private:
	SOCKET _socket;
	bool _connected;
};

