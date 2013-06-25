#pragma once
#include <memory>
#include <stdint.h>
#include <vector>

#include <CommunityLib/LibSocket.h>
#include <CommunityLib/Reader.h>
#include <CommunityLib/Writer.h>
#include <CommunityLib/Detour.h>

class Client
{
public:
	Client(LibSocket::ServiceProvider& serviceProvider);
	~Client(void);

	void Start();
	void Stop();

	void Send(std::vector<uint8_t>& vData);
private:
	std::unique_ptr<LibSocket::Session> m_session;
	std::shared_ptr<Memory::Detour> m_detourWindow;

	void OnMessage(std::vector<uint8_t> vMessage);
	void OnDisconnect();

};

