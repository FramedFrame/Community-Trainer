#pragma once
#include <memory>

#include <CommunityLib/LibSocket.h>
class Session
{
public:
	Session(class ClientManager* pClientManager,LibSocket::Session& session);
	~Session(void);

	void Stop();

	uint32_t operator()();
private:
	std::unique_ptr<LibSocket::Session> m_session;
	bool m_fIsHandshake;
	uint32_t m_uProcessId;

	void OnMessage(std::vector<uint8_t> vMessage);
	void OnDisconnect();

	class ClientManager* m_pClientManager;
};

