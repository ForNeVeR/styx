#include "MessageFactory.h"

#include <m_database.h>

#include "MemoryUtils.h"
#include "MirandaContact.h"
#include "StringUtils.h"

using namespace ru::org::codingteam::styx;

boost::optional<Message> MessageFactory::fromMirandaHandles(const HANDLE contactHandle, const HANDLE dbEventHandle)
{
	// Prepare buffer for BLOB:
	auto blobSize = db_event_getBlobSize(dbEventHandle);
	auto blob = MemoryUtils::MakeUniquePtr(new unsigned char[blobSize], [](unsigned char *p){ delete[] p; });

	auto eventInfo = DBEVENTINFO();
	eventInfo.cbSize = sizeof eventInfo;
	eventInfo.cbBlob = blobSize;
	eventInfo.pBlob = blob.get();

	if (db_event_get(dbEventHandle, &eventInfo))
	{
		throw std::exception("Invalid DB event handle");
	}

	if (eventInfo.eventType != EVENTTYPE_MESSAGE)
	{
		// We handle only messages here.
		return boost::optional<Message>();
	}

	auto protocol = eventInfo.szModule;
	auto contact = MirandaContact(contactHandle);
	auto contactUid = contact.uid();
	auto wText = MemoryUtils::MakeUniquePtr(DbGetEventTextW(&eventInfo, 0), mir_free);
	auto direction = eventInfo.flags & DBEF_SENT ? Message_Direction_OUTGOING : Message_Direction_INCOMING;

	auto message = Message();
	message.set_timestamp(eventInfo.timestamp);
	message.set_protocol(eventInfo.szModule);
	message.set_user_id(StringUtils::EncodeAsUTF8(contactUid));
	message.set_text(StringUtils::EncodeAsUTF8(std::wstring(wText.get())));
	message.set_direction(direction);

	return boost::make_optional(message);
}
