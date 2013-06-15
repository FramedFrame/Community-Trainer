#pragma once
#include <string>
#include <vector>

#include "Session.h"

class ClientManager
{
public:
	ClientManager(std::string strMaple);
	~ClientManager(void);

	bool NewSession(Session& session);
	void SessionDisconnect(Session& session);

	bool StartClient();

private:
	std::vector<Session*> m_vSession;
	std::vector<uint32_t> m_vProcessId;
	std::string m_strMaple,m_strLibary;
};

