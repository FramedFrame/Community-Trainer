#pragma once
#include <vector>
#include <functional>
#include <stdint.h>

#ifndef _LIB
#pragma comment(lib,"CommunityLib.lib")
#endif

namespace Network
{
	class _Server;
	class _Session;
	class ServiceProvider;
}


namespace LibSocket
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

class Session
{
public:
	Session(LibSocket::ServiceProvider& serviceProvider);
	Session(void* pBase);
	~Session(void);

	void LinkFunctions(std::function<void(std::vector<uint8_t>&)> funOnData,std::function<void()> funOnDisconnect);

	void StartConnect();

	void Start();
	void Stop();

	void Send(std::vector<uint8_t>& vBuffer);
	void SendRaw(std::vector<uint8_t>& vBuffer);
private:
	void* m_pClass;
};


class Server
{
public:
	Server(std::function<void(LibSocket::Session&)> funAccept,LibSocket::ServiceProvider& serviceProvider);
	~Server(void);

	void Start();
	void Stop();
private:
	void* m_pClass;
};

}