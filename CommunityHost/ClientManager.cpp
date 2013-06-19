#include "ClientManager.h"
#include <Windows.h>

#include <CommunityLib/Detour.h>


ClientManager::ClientManager(std::string strMaple,UI::MainWindow* pMainWindow)
{
	this->m_pMainWindow = pMainWindow;

	this->m_strMaple = strMaple;


	char szDirBuffer[1024];

	this->m_strLibary = "ERROR";

	if(!GetCurrentDirectoryA(1024,szDirBuffer))
		return;

	this->m_strLibary = std::string(szDirBuffer);

	this->m_strLibary += "\\Libary\\CommunityClient.dll";
}

ClientManager::~ClientManager( void )
{

}

bool ClientManager::NewSession( Session& session )
{
	uint32_t uProcessId = session();

	auto x = std::find(this->m_vProcessId.begin(),this->m_vProcessId.end(),uProcessId);

	if(x == this->m_vProcessId.end())
		return false;

	this->m_vProcessId.erase(x);

	return true;
}
void ClientManager::SessionDisconnect(Session& session)
{
	auto x = std::find(this->m_vSession.begin(),this->m_vSession.end(),&session);

	if(x == this->m_vSession.end())
		return;

	this->m_vSession.erase(x);

	return;
}

bool ClientManager::StartClient()
{
	auto x = Memory::Detour::CreateProcessWithDll(this->m_strMaple,this->m_strLibary);

	if(!x.first)
		return false;

	this->m_vProcessId.push_back(x.second.dwProcessId);
	return true;
}