#pragma once

#include <boost/optional.hpp>

#include "MessageDef.pb.h"
#include "WindowsIncludes.h"

class MessageFactory
{
public:
	static boost::optional<ru::org::codingteam::styx::Message> fromMirandaHandles(
		const HANDLE contactHandle,
		const HANDLE dbEventHandle);
};

