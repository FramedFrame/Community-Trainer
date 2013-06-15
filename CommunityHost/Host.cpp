#include "Host.h"

#include "Session.h"
#include "ClientManager.h"


Host::Host(ClientManager* pClientManager,LibSocket::ServiceProvider& serviceProvider)
{

	this->m_server.reset(new LibSocket::Server(std::bind(&Host::NewSession,this,std::placeholders::_1),
		serviceProvider));

	this->m_pClientManager = pClientManager;
}


Host::~Host(void)
{
}

void Host::Start()
{
	this->m_server->Start();
}
void Host::Stop()
{
	this->m_server->Stop();
}

void Host::NewSession( LibSocket::Session& session )
{
	Session* pSession = new Session(this->m_pClientManager,session);
}
