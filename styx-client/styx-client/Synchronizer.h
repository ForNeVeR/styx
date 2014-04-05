#pragma once

#include "SynchronizerState.h"

#include <boost/optional.hpp>

#include "ChunkHashResultDef.pb.h"
#include "LoginResultDef.pb.h"
#include "MessageDef.pb.h"
#include "MirandaContact.h"
#include "WsaSocket.h"

class Connector;

class Synchronizer
{
public:
	Synchronizer();

	void dispatchConnected(Connector &connector, WsaSocket &socket);
	void dispatchContactAdded(Connector &connector, WsaSocket &socket);
	void dispatchContactDeleted(Connector &connector, WsaSocket &socket);

	void dispatchMessage(Connector &connector, WsaSocket &socket, const ru::org::codingteam::styx::Message &message);
	void dispatchMessage(Connector &connector, WsaSocket &socket, const ru::org::codingteam::styx::LoginResult &message);
	void dispatchMessage(Connector &connector, WsaSocket &socket, const ru::org::codingteam::styx::ChunkHashResult &message);

private:
	SynchronizerState _state;
	boost::optional<ru::org::codingteam::styx::Message> _message;

	void getFirstMessage();
	void getNextMessage();

	void hashingStep(Connector &connector, WsaSocket &socket);
};

