#include "Handle.h"

#include <exception>
#include <sstream>

Handle::Handle(HANDLE handle)
{
	_handle = handle;
}

Handle::~Handle()
{
	if (CloseHandle(_handle))
	{
		auto message = std::stringstream();
		message << "Error closing handle: " << GetLastError();
		throw std::exception(message.str().c_str());
	}
}

Handle::operator HANDLE()
{
	return _handle;
}
