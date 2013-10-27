#pragma once

#include <exception>
#include <string>

class WsaException : public std::exception
{
public:
	WsaException(std::string &&message, int code);
	const char *what() const;
private:
	std::string _message;
};
