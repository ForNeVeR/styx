#include <functional>
#include <iostream>
#include <memory>

#include "WsaException.h"
#include "WsaInit.h"
#include "WsaSocket.h"

#include "Message.pb.h"

using namespace ru::org::codingteam::styx;

const auto host = "127.0.0.1";
const auto defaultPort = "6060";

void start()
{
	auto init = WsaInit();

	auto address = addrinfo();
	address.ai_family = AF_UNSPEC;
	address.ai_socktype = SOCK_STREAM;
	address.ai_protocol = IPPROTO::IPPROTO_TCP;

	addrinfo *temp = nullptr;

	// Resolve the server address and port
	auto iResult = getaddrinfo(host, defaultPort, &address, &temp);

	auto deleter = &freeaddrinfo;
	auto result = std::unique_ptr<addrinfo, decltype(deleter)>(temp, deleter);

	if (iResult)
	{
		throw WsaException("getaddrinfo failed", iResult);
	}

	WsaSocket socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	socket.connect(*result);

	auto p = Ping();
	p.set_message("10005");	
	socket.send(p.SerializeAsString());

	p.set_message(std::string(10240, 'x'));
	socket.send(p.SerializeAsString());
	
	std::cout << "Data sent successfully" << std::endl;
}

int main()
{
	try
	{
		start();
	}
	catch (const std::exception &exception)
	{
		std::cout << exception.what() << std::endl;
		return 1;
	}

	std::cout << "Normal exit" << std::endl;
	return 0;
}