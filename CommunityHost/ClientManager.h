#pragma once
#include <string>
#include <vector>

#include "MainWindow.h"
#include "Session.h"

class ClientManager
{
public:
	ClientManager(std::string strMaple,UI::MainWindow* pMainWindow);
	~ClientManager(void);

	bool NewSession(Session& session);
	void SessionDisconnect(Session& session);

	bool StartClient();

private:
	std::vector<Session*> m_vSession;
	std::vector<uint32_t> m_vProcessId;
	std::string m_strMaple,m_strLibary;
	UI::MainWindow* m_pMainWindow;
};

