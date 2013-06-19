#pragma once
#include <memory.h>

#include <CommunityLib/LibSocket.h>
#include "ClientManager.h"
#include "Host.h"

struct Context
{
	std::shared_ptr<Host> Host;
	std::shared_ptr<ClientManager> ClientManager;
	std::shared_ptr<LibSocket::ServiceProvider> ServiceProvider;
};

static std::shared_ptr<Context> ContextInstance;