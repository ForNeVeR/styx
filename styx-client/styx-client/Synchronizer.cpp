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
	_eventHandle(),
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
		getFirstMessage();

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

void Synchronizer::getFirstMessage()
{
	_contact = MirandaContact::getFirst();
	getNextMessage();
}

void Synchronizer::getNextMessage()
{
	if (_contact && _eventHandle)
	{
		_eventHandle = _contact->getNextEventHandle(*_eventHandle);
	}

	while (_contact && !_eventHandle)
	{
		_eventHandle = _contact->getFirstEventHandle();
		if (!_eventHandle)
		{
			_contact = MirandaContact::getNext(*_contact);
		}
	}
}

void Synchronizer::hashingStep(Connector &connector, WsaSocket &socket)
{
	if (!_contact || !_eventHandle)
	{
		BOOST_LOG_TRIVIAL(info) << "Enter messaging state";
		_state = SynchronizerState::Messaging;
		
		_contact = boost::optional<MirandaContact>();
		_eventHandle = boost::optional<HANDLE>();

		return;
	}
	
	// TODO: Now, for simplicity, all chunks are of size 1.
	auto message = MessageFactory::fromMirandaHandles(_contact->handle(), *_eventHandle);	
	auto hashValue = HashingHelper::calculateHash(message.get());
	auto chunkHash = ChunkHash();
	chunkHash.set_protocol(message->protocol());
	chunkHash.set_userid(message->user_id());
	chunkHash.set_timestamp(message->timestamp());
	chunkHash.set_count(1);
	chunkHash.set_hash(hashValue);

	_message = message;
	connector.sendChunkHash(socket, chunkHash);
}