#pragma once

enum class SynchronizerState
{
	NotConnected,
	Handshake,
	Hashing,
	Messaging
};