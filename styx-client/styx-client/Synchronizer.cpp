#include "Synchronizer.h"

#include <boost/log/trivial.hpp>

#include "Connector.h"

using namespace ru::org::codingteam::styx;

Synchronizer::Synchronizer()
	: _state(SynchronizerState::NotConnected),
	_contact()
{
}

void Synchronizer::dispatchConnected(Connector &connector, WsaSocket &socket)
{
	_state = SynchronizerState::Handshake;
	connector.sendLogin(socket);
}

void Synchronizer::dispatchContactAdded(Connector &connector, WsaSocket &socket)
{
	// TODO: implement
}

void Synchronizer::dispatchContactDeleted(Connector &connector, WsaSocket &socket)
{
	// TODO: implement
}

void Synchronizer::dispatchMessage(Connector &connector, WsaSocket &socket, const Message &message)
{
	switch (_state)
	{
	case SynchronizerState::Hashing:
		// TODO: Check current contact and message contact, invalidate some chunks if needed.
		break;
	case SynchronizerState::Messaging:
		connector.sendMessage(socket, message);
		break;
	}
}

void Synchronizer::dispatchMessage(Connector &connector, WsaSocket &socket, const LoginResult &message)
{
	if (!message.logged())
	{
		throw std::exception("Cannot log in");
	}

	switch (_state)
	{
	case SynchronizerState::Handshake:
		_state = SynchronizerState::Hashing;
		BOOST_LOG_TRIVIAL(info) << "Successfully logged in";
		_contact = MirandaContact::getFirst();
		// TODO: Get first contact message
		hashingStep(connector, socket);
		break;
	default:
		throw std::exception("Invalid synchronizer state");
	}
}

void Synchronizer::hashingStep(Connector &connector, WsaSocket &socket)
{
	if (!_contact)
	{
		_state = SynchronizerState::Messaging;
		return;
	}

	// TODO: Get current message.
	// TODO: If current message is `none`, get next contact and set its first message as current.
}