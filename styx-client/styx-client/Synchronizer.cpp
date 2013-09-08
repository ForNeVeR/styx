#include "Synchronizer.h"

#include <boost/log/trivial.hpp>

#include "Connector.h"
#include "ChunkHashDef.pb.h"
#include "HashingHelper.h"
#include "MessageFactory.h"

using namespace ru::org::codingteam::styx;

Synchronizer::Synchronizer()
	: _state(SynchronizerState::NotConnected),
	_contact(),
	_eventHandle()
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
		if (_contact)
		{
			_eventHandle = _contact->getFirstEventHandle();
		}

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

	if (!_eventHandle)
	{
		_contact = MirandaContact::getNext(_contact.get());
		if (_contact)
		{
			_eventHandle = _contact->getFirstEventHandle();
		}
		hashingStep(connector, socket);
		return;
	}

	// TODO: Now, for simplicity, all chunks are of size 1.

	// Let's find a first valid message.
	auto message = boost::optional<Message>();
	while (!message)
	{
		auto eventHandle = _eventHandle.get();
		message = MessageFactory::fromMirandaHandles(_contact->handle(), eventHandle);
		_eventHandle = _contact->getNextEventHandle(eventHandle);
		if (_eventHandle)
		{
			hashingStep(connector, socket);
			return;
		}
	}
	
	auto hashValue = HashingHelper::calculateHash(message.get());
	auto chunkHash = ChunkHash();
	chunkHash.set_timestamp(message->timestamp());
	chunkHash.set_count(1);
	chunkHash.set_hash(hashValue);

	connector.sendChunkHash(socket, chunkHash);
}