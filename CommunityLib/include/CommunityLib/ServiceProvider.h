#pragma once
#include "Network.h"

namespace Network
{
class ServiceProvider
{
public:
	ServiceProvider(void);
	~ServiceProvider(void);

	void* operator()();
	void Run();
	void Stop();

private:
	void* m_pio_service;
};
}

