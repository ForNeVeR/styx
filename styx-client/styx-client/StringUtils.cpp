#include "StringUtils.h"

#include <windows.h>

#include "MemoryUtils.h"

std::string StringUtils::EncodeAsUTF8(const std::wstring &string)
{
	auto convert = [&string](char *buffer, int size)
	{
		return ::WideCharToMultiByte(
			CP_UTF8,
			0,
			string.c_str(),
			-1,
			buffer,
			size,
			nullptr,
			nullptr);
	};
	
	auto requiredSize = convert(nullptr, 0);
	auto buffer = MemoryUtils::MakeUniquePtr(new char[requiredSize], std::default_delete<char[]>());
	if (convert(buffer.get(), requiredSize))
	{
		throw std::exception("Error when converting to UTF-8");
	}

	return std::string(buffer.get(), requiredSize);
}
