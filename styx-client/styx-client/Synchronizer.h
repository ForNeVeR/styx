#pragma once

#include "SynchronizerState.h"

#include <boost/optional.hpp>

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

private:
	SynchronizerState _state;
	boost::optional<MirandaContact> _contact;
	boost::optional<HANDLE> _eventHandle;

	boost::optional<MirandaContact> getActualContact();
	void hashingStep(Connector &connector, WsaSocket &socket);
};

