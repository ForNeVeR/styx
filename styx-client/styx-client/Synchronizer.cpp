#include "Synchronizer.h"

#include <boost/log/trivial.hpp>

#include "Connector.h"
#include "ChunkHashDef.pb.h"
#include "HashingHelper.h"
#include "MessageFactory.h"

using namespace ru::org::codingteam::styx;

Synchronizer::Synchronizer()
	: _state(SynchronizerState::NotConnected),
	_message()
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
		hashingStep(connector, socket);
		break;
	default:
		throw std::exception("Invalid synchronizer state");
	}
}

void Synchronizer::dispatchMessage(Connector &connector, WsaSocket &socket, const ChunkHashResult &message)
{
	if (message.positive())
	{
		hashingStep(connector, socket);
	}
	else
	{
		connector.sendMessage(socket, _message.get());
	}
}

void Synchronizer::hashingStep(Connector &connector, WsaSocket &socket)
{
	// Loop over all of the contacts:
	auto contact = MirandaContact::getFirst();
	while (contact)
	{
		// TODO: Optimize so we don't need to loop over all contacts and all messages every time.
		auto lastSentTimestamp = contact->getLastSentMessageTimestamp();
		auto eventHandle = contact->getFirstEventHandle();
		while (eventHandle)
		{
			auto message = MessageFactory::fromMirandaHandles(contact->handle(), *eventHandle);
			if (message)
			{
				auto messageTimestamp = message->timestamp();
				if (messageTimestamp > lastSentTimestamp) // TODO: Do something in case we have multiple messages on one timestamp.
				{
					connector.sendMessage(socket, *message);
					return;
				}
			}

			eventHandle = contact->getNextEventHandle(*eventHandle);
		}

		contact = MirandaContact::getNext(*contact);
	}
	
	BOOST_LOG_TRIVIAL(info) << "Enter messaging state";
	_state = SynchronizerState::Messaging;
}