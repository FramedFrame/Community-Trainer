#pragma once
#include <memory>

#include <CommunityLib/LibSocket.h>

class Client
{
public:
	Client(LibSocket::ServiceProvider& serviceProvider);
	~Client(void);

	void Start();
	void Stop();
private:
	std::unique_ptr<LibSocket::Session> m_session;

	void OnMessage(std::vector<uint8_t> vMessage);
	void OnDisconnect();

};

