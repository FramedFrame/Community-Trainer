#pragma once

#include <memory>
#include <stdint.h>
#include <Windows.h>

#include "ClientPanel.h"
#include "Session.h"

#include <CommunityLib\Host.h>
#include <CommunityLib\Accessor.h>

struct ClientContext
{
	HWND Window;
	uint32_t ProcessId;
	HANDLE Process;
	HANDLE MainThread;
	std::shared_ptr<Session> Session;
	std::shared_ptr<UI::ClientPanel> Panel;
	std::shared_ptr<Plugin::Host> Host;
	std::shared_ptr<Memory::BasicAccessor> Memory;
};

