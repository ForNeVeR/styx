#pragma once

#include <cstdint>
#include <string>

#include <boost/optional.hpp>

#include "WindowsIncludes.h"

class MirandaContact
{
public:
	static boost::optional<MirandaContact> fromHandle(const HANDLE contact);
	
	static boost::optional<MirandaContact> getFirst();
	static boost::optional<MirandaContact> getNext(const MirandaContact &contact);
	
	MirandaContact(const HANDLE handle);

	HANDLE handle() const;
	std::wstring uid() const;
	boost::optional<HANDLE> getFirstEventHandle() const;
	boost::optional<HANDLE> getNextEventHandle(HANDLE handle) const;
	boost::optional<int64_t> MirandaContact::getLastSentMessageTimestamp() const;

private:
	HANDLE _handle;
};

