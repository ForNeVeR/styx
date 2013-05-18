#pragma once

#include <memory>

#include "WindowsIncludes.h"

class Connector
{
public:
	Connector();
	~Connector();

	void start();
	void stop();

private:
	static DWORD WINAPI loop(LPVOID self);

	std::unique_ptr<addrinfo, decltype(&freeaddrinfo)> GetServerAddress();

	bool _started;
	DWORD _threadId;
};

