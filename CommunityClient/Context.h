#pragma once
#include <memory>
#include <stdint.h>
#include <Windows.h>

#include <CommunityLib/LibSocket.h>

struct Context
{
	std::shared_ptr<class Client> Client;
	std::shared_ptr<LibSocket::ServiceProvider> ServiceProvider;
	HWND Window;
	bool Shown;
};

extern std::shared_ptr<Context> ContextInstance;
