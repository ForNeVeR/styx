#include "HashingHelper.h"

std::uint64_t HashingHelper::calculateHash(const ru::org::codingteam::styx::Message &message)
{
	// TODO: This is a very naive implementation.
	auto value = message.text().size() + message.user_id().size() + message.direction();
	return value;
}
