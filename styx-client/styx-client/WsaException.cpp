#include "WsaException.h"


WsaException::WsaException(std::string &&message, int code)
{
	_message = message + ": " + std::to_string(code);
}

const char *WsaException::what() const
{
	return _message.c_str();
}
