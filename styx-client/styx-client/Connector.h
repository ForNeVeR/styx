#pragma once

#include <memory>
#include <vector>

#include <concurrent_queue.h>

#include "MessageDef.pb.h"
#include "MessageTypeDef.pb.h"
#include "Handle.h"
#include "WindowsIncludes.h"
#include "WsaSocket.h"

class Synchronizer;

class Connector
{
public:
	Connector();
	~Connector();

	void start();
	void stop();

	void queueMessage(const ru::org::codingteam::styx::Message &message);

	void sendLogin(WsaSocket &socket);
	void sendMessage(WsaSocket &socket, const ru::org::codingteam::styx::Message &message);

private:
	static void _cdecl loop(void *self);

	bool _started;
	uintptr_t _threadId;
	Handle _queueEventHandle;
	std::vector<char> _socketBuffer;

	concurrency::concurrent_queue<ru::org::codingteam::styx::Message> _messageQueue;

	std::unique_ptr<addrinfo, decltype(&freeaddrinfo)> getServerAddress();

	void dispatchMessages(Synchronizer &synchronizer, WsaSocket &socket);
	void dispatchData(Synchronizer &synchronizer, WsaSocket &socket);

	void sendDatagram(
		WsaSocket &socket,
		const ru::org::codingteam::styx::MessageType &type,
		const google::protobuf::Message &message);
};
