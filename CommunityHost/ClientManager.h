#pragma once
#include <map>
#include <string>
#include <vector>

#include "Session.h"

struct ClientContext;

class ClientManager
{
public:
	ClientManager(std::string strMaple);
	~ClientManager(void);

	bool NewSession(Session& session);
	void SessionDisconnect(Session& session);

	bool StartClient(std::shared_ptr<ClientContext> clientContext);

private:
	std::map<Session*,std::shared_ptr<ClientContext>> m_mapSession;
	std::map<uint32_t,std::shared_ptr<ClientContext>> m_mapProcessId;
	std::string m_strMaple,m_strLibary;
};

