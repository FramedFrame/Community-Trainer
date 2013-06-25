#include "ClientManager.h"
#include "ClientContext.h"
#include <Windows.h>
#include <iostream>
#include <CommunityLib/Detour.h>


ClientManager::ClientManager(std::string strMaple)
{
	this->m_strMaple = strMaple;


	char szDirBuffer[1024];

	this->m_strLibary = "ERROR";

	if(!GetCurrentDirectoryA(1024,szDirBuffer))
		return;

	//this->m_strLibary = std::string(szDirBuffer);

	//this->m_strLibary += "\\Libary\\CommunityClient.dll";
	this->m_strLibary = "C:\\Users\\Home\\Documents\\Community-Trainer\\Debug\\CommunityClient.dll";
}

ClientManager::~ClientManager( void )
{

}

bool ClientManager::NewSession( Session& session )
{
	uint32_t uProcessId = session();

	auto x = this->m_mapProcessId.find(session());

	if(x == this->m_mapProcessId.end())
		return false;

	auto y = x->second;
	y->Session.reset(&session);
	/*
	Add Read Methods
	*/

	this->m_mapProcessId.erase(uProcessId);
	this->m_mapSession[&session] = y;
	session.AppendData(y);

	return true;
}
void ClientManager::SessionDisconnect(Session& session)
{
	auto x = this->m_mapSession[&session];

	if(!x)
		return;//No active session

	//May Terminate the process if it's valid
	uint32_t uWaitCode = WaitForSingleObject(x->Process,0);
	if(uWaitCode == WAIT_ABANDONED || uWaitCode == WAIT_OBJECT_0)//Process is running
		TerminateProcess(x->Process,EXIT_SUCCESS);
	
	CloseHandle(x->Process);//Clean Sys Resources
	CloseHandle(x->MainThread);

	//To DO!
	/*x->Panel->close();
	Force Redraw,requirs sync due async service
	*/

	this->m_mapSession.erase(&session);

	return;
}

bool ClientManager::StartClient(std::shared_ptr<ClientContext> clientContext)
{
	auto x = Memory::Detour::CreateProcessWithDll(this->m_strMaple,this->m_strLibary);
	if(!x.first)
	{
		std::cout << "Start up Error: " << GetLastError() << std::endl;
		return false;
	}

	clientContext->MainThread = x.second.hThread;
	clientContext->Process = x.second.hProcess;
	clientContext->ProcessId = x.second.dwProcessId;

	this->m_mapProcessId[x.second.dwProcessId] = clientContext;
	return true;
}