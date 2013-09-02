#pragma once
#include "SynchronizerState.h"

#include "LoginResultDef.pb.h"
#include "MessageDef.pb.h"
#include "WsaSocket.h"

class Connector;

class Synchronizer
{
public:
	Synchronizer();

	void dispatchConnected(Connector &connector, WsaSocket &socket);
	void dispatchMessage(Connector &connector, WsaSocket &socket, const ru::org::codingteam::styx::Message &message);
	void dispatchMessage(Connector &connector, WsaSocket &socket, const ru::org::codingteam::styx::LoginResult &message);

private:
	SynchronizerState _state;

	void hashingStep(Connector &connector, WsaSocket &socket);
};

