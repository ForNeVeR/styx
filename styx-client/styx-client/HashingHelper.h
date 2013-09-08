#pragma once

#include <cstdint>

#include "MessageDef.pb.h"

class HashingHelper
{
public:
	static std::uint64_t calculateHash(const ru::org::codingteam::styx::Message &message);
};

