#pragma once
#include <memory>

#include <CommunityLib/LibSocket.h>

struct ClientContext;
class Session
{
public:
	Session(class ClientManager* pClientManager,LibSocket::Session& session);
	~Session(void);

	void Stop();

	uint32_t operator()();
	void AppendData(std::shared_ptr<ClientContext>& pData);
private:
	std::unique_ptr<LibSocket::Session> m_session;
	bool m_fIsHandshake;
	uint32_t m_uProcessId;

	void OnMessage(std::vector<uint8_t> vMessage);
	void OnDisconnect();

	class ClientManager* m_pClientManager;
	std::shared_ptr<ClientContext> m_clientContext;
};

