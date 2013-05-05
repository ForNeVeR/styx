#include <exception>
#include <string>

#pragma once
class WsaException : public std::exception
{
public:
	WsaException(std::string &&message, int code);
	const char *what() const;
private:
	std::string _message;
};

