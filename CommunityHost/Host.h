#pragma once
#include <memory>
#include <CommunityLib/LibSocket.h>

class Host
{
public:
	Host();
	~Host(void);

	void Start();
	void Stop();

	void NewSession(LibSocket::Session& session);
private:
	std::unique_ptr<LibSocket::Server> m_server;
};

