#pragma once
#include <memory>

#include <CommunityLib/LibSocket.h>

struct ClientContext;
class Session
{
public:
	Session(LibSocket::Session& session);
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

	std::shared_ptr<ClientContext> m_clientContext;
};

