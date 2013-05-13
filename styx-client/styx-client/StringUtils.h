#pragma once

#include <string>

struct StringUtils
{
	static std::string EncodeAsUTF8(const std::wstring &string);
};

