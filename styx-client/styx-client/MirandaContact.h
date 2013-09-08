#pragma once

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

private:
	HANDLE _handle;
};

