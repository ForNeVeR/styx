#pragma once

#include <concurrent_queue.h>
#include <memory>

#include "Datagram.pb.h"
#include "Handle.h"
#include "WindowsIncludes.h"
#include "WsaSocket.h"

class Connector
{
public:
	Connector();
	~Connector();

	void start();
	void stop();

	void queueMessage(const ru::org::codingteam::styx::Message &message);

private:
	static DWORD WINAPI loop(LPVOID self);

	bool _started;
	DWORD _threadId;

	Handle _queueEventHandle;
	concurrency::concurrent_queue<ru::org::codingteam::styx::Message> _messageQueue;

	std::unique_ptr<addrinfo, decltype(&freeaddrinfo)> GetServerAddress();

	void sendMessage(WsaSocket &socket);
	void receiveData(WsaSocket &socket);
};
