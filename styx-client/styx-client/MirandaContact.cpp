#include "MirandaContact.h"

#include <exception>
#include <sstream>

#include <m_contacts.h>
#include <m_core.h>

std::wstring MirandaContact::GetUID(HANDLE contactHandle)
{
	auto contactInfo = CONTACTINFO();
	contactInfo.hContact = contactHandle;
	contactInfo.dwFlag = CNF_UNIQUEID | CNF_UNICODE;

	if (CallService(MS_CONTACT_GETCONTACTINFO, 0, reinterpret_cast<LPARAM>(&contactInfo)))
	{
		throw std::exception("Contact not found");
	}

	// There are unique identifiers of different types, so:
	auto stream = std::wostringstream();
	switch (contactInfo.type)
	{
	case CNFT_BYTE:
		stream << contactInfo.bVal;
		break;
	case CNFT_WORD:
		stream << contactInfo.wVal;
		break;
	case CNFT_DWORD:
		stream << contactInfo.dVal;
		break;
	case CNFT_ASCIIZ:
		stream << contactInfo.pszVal;
	default:
		throw std::exception("Unknown contact info value type");
	}

	return stream.str();
}