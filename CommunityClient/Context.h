#pragma once
#include <memory>
#include <stdint.h>
#include "Client.h"
#include <CommunityLib/LibSocket.h>

struct Context
{
	std::shared_ptr<Client> Client;
	std::shared_ptr<LibSocket::ServiceProvider> ServiceProvider;
};

extern std::shared_ptr<Context> ContextInstance;
