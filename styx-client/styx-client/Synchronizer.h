#pragma once
#include "SynchronizerState.h"

#include "Datagram.pb.h"
#include "WsaSocket.h"

class Connector;

class Synchronizer
{
public:
	Synchronizer();

	void dispatchConnected(Connector &connector, WsaSocket &socket);
	void dispatchMessage(Connector &connector, WsaSocket &socket, ru::org::codingteam::styx::Message &message);

private:
	SynchronizerState _state;
};

